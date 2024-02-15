/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    pal_rf.c

  Summary:
    RF Platform Abstraction Layer (PAL) Interface source file.

  Description:
    RF Platform Abstraction Layer (PAL) Interface source file. The RF PAL module
    provides a simple interface to manage the RF PHY layer.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*
Copyright (C) 2024, Microchip Technology Inc., and its subsidiaries. All rights reserved.

The software and documentation is provided by microchip and its contributors
"as is" and any express, implied or statutory warranties, including, but not
limited to, the implied warranties of merchantability, fitness for a particular
purpose and non-infringement of third party intellectual property rights are
disclaimed to the fullest extent permitted by law. In no event shall microchip
or its contributors be liable for any direct, indirect, incidental, special,
exemplary, or consequential damages (including, but not limited to, procurement
of substitute goods or services; loss of use, data, or profits; or business
interruption) however caused and on any theory of liability, whether in contract,
strict liability, or tort (including negligence or otherwise) arising in any way
out of the use of the software and documentation, even if advised of the
possibility of such damage.

Except as expressly permitted hereunder and subject to the applicable license terms
for any third-party software incorporated in the software and any applicable open
source software license terms, no license or other rights, whether express or
implied, are granted under any patent or other intellectual property rights of
Microchip or any third party.
*/
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "configuration.h"
#include "system/time/sys_time.h"
#include "driver/driver.h"
#include "stack_config.h"
#include "driver/IEEE_802154_PHY/phy/inc/phy.h"
#include "driver/IEEE_802154_PHY/phy/inc/phy_tasks.h"
#include "phy_constants.h"
#include "pal_rf.h"
#include "pal_rf_local.h"
#include "osal/osal.h"

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Variables
// *****************************************************************************
// *****************************************************************************

static PAL_RF_DATA palRfData = {0};
static PAL_RF_PHY_STATUS palRfPhyStatus[] = {
    PAL_RF_PHY_SUCCESS, // *PHY_SUCCESS
    PAL_RF_PHY_TRX_OFF, // PHY_TRX_ASLEEP
    PAL_RF_PHY_TRX_OFF, // PHY_TRX_AWAKE
    PAL_RF_PHY_TX_CANCELLED, // *PHY_RF_REQ_ABORTED
    PAL_RF_PHY_TRX_OFF, // *PHY_RF_UNAVAILABLE
    PAL_RF_PHY_ERROR, // *PHY_FAILURE
    PAL_RF_PHY_BUSY_TX, // **PHY_BUSY
    PAL_RF_PHY_SUCCESS, // *PHY_FRAME_PENDING
    PAL_RF_PHY_INVALID_PARAM, // PHY_INVALID_PARAMETER
    PAL_RF_PHY_INVALID_PARAM, // PHY_UNSUPPORTED_ATTRIBUTE
    PAL_RF_PHY_BUSY_TX, // PHY_CHANNEL_BUSY
    PAL_RF_PHY_TRX_OFF, // PHY_CHANNEL_IDLE
    PAL_RF_PHY_SUCCESS, // *PHY_NO_ACK
    PAL_RF_PHY_CHANNEL_ACCESS_FAILURE // *PHY_CHANNEL_ACCESS_FAILURE
};

// Transceiver Configuration
#define CHANNEL_TRANSMIT_RECEIVE (11U)
#define CHANNEL_PAGE_TRANSMIT_RECEIVE (0U)
#define CCA_MODE (1U)
#define PROMISCUOUS_MODE (true)
#define AUTOACK_MODE (false)
#define PDTLEVEL (8U)

/* Define a semaphore to signal the PAL RF Tasks to process Tx/Rx packets */
static OSAL_SEM_DECLARE(palRFSemID);

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Functions
// *****************************************************************************
// *****************************************************************************
static void lPAL_RF_ReportResultTX(PAL_RF_PHY_STATUS status, uint64_t timeStamp, uint8_t nBytesSent)
{
    PAL_RF_STATISTICS *rfStats;

    rfStats = &palRfData.stats;
    palRfData.txCfmStatus = status;

    switch (status)
    {
        case PAL_RF_PHY_SUCCESS:
            rfStats->txTotalBytes += nBytesSent;
            rfStats->txTotalPackets++;
            break;

        case PAL_RF_PHY_CHANNEL_ACCESS_FAILURE:
            rfStats->txErrorBusyChannel++;
            break;

        case PAL_RF_PHY_BUSY_TX:
            if (PHY_GetTrxStatus() == PHY_BUSY_TX)
            {
                rfStats->txErrorBusyTX++;
            }
            else
            {
                rfStats->txErrorBusyRX++;
            }
            break;

        case PAL_RF_PHY_TIMEOUT:
            rfStats->txErrorTimeout++;
            break;

        case PAL_RF_PHY_INVALID_PARAM:
            rfStats->txErrorFormat++;
            break;

        case PAL_RF_PHY_TX_CANCELLED:
            rfStats->txErrorAborted++;
            break;

        case PAL_RF_PHY_TX_ERROR:
            rfStats->txErrorTxError++;
            break;

        case PAL_RF_PHY_TRX_OFF:
            rfStats->txErrorTrxOff++;
            break;

        case PAL_RF_PHY_ERROR:
        default:
            rfStats->txErrorPhyErrors++;
            break;
    }

    if (status != PAL_RF_PHY_SUCCESS)
    {
        rfStats->txTotalErrors++;
        palRfData.txTimeIniCount = timeStamp;
        palRfData.txTimeEndCount = timeStamp;
    }

    // Report TX Result through TX CFM callback
    if (palRfData.rfPhyHandlers.palRfTxConfirm != NULL)
    {
        rfStats->txTotalCfmPackets++;
        rfStats->txCfmNotHandled--;
        palRfData.rfPhyHandlers.palRfTxConfirm(palRfData.txCfmStatus,
                    palRfData.txTimeIniCount, palRfData.txTimeEndCount);
    }

}

static PAL_RF_PIB_RESULT lPAL_RF_setRFNetworkParameters(void)
{
    PibValue_t pibValue;
    PHY_Retval_t attributeStatus;

    pibValue.pib_value_8bit = (uint8_t)CHANNEL_TRANSMIT_RECEIVE;
    attributeStatus = PHY_PibSet(phyCurrentChannel, &pibValue);
    if (attributeStatus != PHY_SUCCESS)
    {
        return PAL_RF_PIB_INVALID_PARAM;
    }

    pibValue.pib_value_8bit = (uint8_t)CHANNEL_PAGE_TRANSMIT_RECEIVE;
    attributeStatus = PHY_PibSet(phyCurrentPage, &pibValue);
    if (attributeStatus != PHY_SUCCESS)
    {
        return PAL_RF_PIB_INVALID_PARAM;
    }

    pibValue.pib_value_8bit = (uint8_t)CCA_MODE;
    attributeStatus = PHY_PibSet(phyCCAMode, &pibValue);
    if (attributeStatus != PHY_SUCCESS)
    {
        return PAL_RF_PIB_INVALID_PARAM;
    }

    (void) PHY_ConfigRxPromiscuousMode(PROMISCUOUS_MODE);
    pibValue.pib_value_bool = (bool)PROMISCUOUS_MODE;
    attributeStatus = PHY_PibSet(macPromiscuousMode, &pibValue);
    if (attributeStatus != PHY_SUCCESS)
    {
        return PAL_RF_PIB_INVALID_PARAM;
    }

    attributeStatus = PHY_ConfigAutoAck(AUTOACK_MODE);
    if (attributeStatus != PHY_SUCCESS)
    {
        return PAL_RF_PIB_INVALID_PARAM;
    }

    // Set the PDT level
    (void) PHY_ConfigRxSensitivity(PDTLEVEL);

    return PAL_RF_PIB_SUCCESS;
}

// *****************************************************************************
// *****************************************************************************
// Section: Local Callbacks
// *****************************************************************************
// *****************************************************************************

/* MISRA C-2012 deviation block start */
/* MISRA C-2012 Rule 5.8 deviated twice. Deviation record ID - H3_MISRAC_2012_R_5_8_DR_1 */
/* MISRA C-2012 Rule 8.6 deviated twice. Deviation record ID - H3_MISRAC_2012_R_8_6_DR_1 */

void PHY_TxDoneCallback(PHY_Retval_t status, PHY_FrameInfo_t *frame)
{
    uint64_t timeCount;
    uint32_t phyDurationSymb;
    uint32_t phyDurationCount;
    PAL_RF_PHY_STATUS palRfStatus;
    uint8_t nBytesSent;

    timeCount = SYS_TIME_Counter64Get();
    palRfStatus = palRfPhyStatus[(uint8_t)status & 0x0FU];
    /* Get received data lentgh */
    nBytesSent = frame->mpdu[0];
    palRfData.txTransmitting = false;

    if (palRfStatus == PAL_RF_PHY_SUCCESS)
    {
        /* The number of symbols per octet for the current PHY is 2 */
        palRfData.stats.txLastPaySymbols = (uint16_t)nBytesSent * 2U;
        /* The duration of the synchronization header (SHR) in symbols is 10 */
        phyDurationSymb = 10U + (uint32_t)palRfData.stats.txLastPaySymbols;
        /* Symbol rate : 62.5 Ksymb/s -> 16us/symbol */
        phyDurationCount = SYS_TIME_USToCount(phyDurationSymb << 4);
        palRfData.txTimeEndCount = palRfData.txTimeIniCount + phyDurationCount;
    }
    else
    {
        palRfData.txTimeEndCount = palRfData.txTimeIniCount;
    }

    // Report TX CFM
    lPAL_RF_ReportResultTX(palRfStatus, timeCount, nBytesSent);

}

void PHY_RxFrameCallback(PHY_FrameInfo_t *rxFrame)
{
    uint64_t rxTimeEndCount;
    PAL_RF_RX_PARAMETERS rxParameters;
    PAL_RF_STATISTICS *rfStats;
    uint32_t phyDurationSymb;
    uint8_t frameLen ;

    /* Get Time Counter */
    rxTimeEndCount = SYS_TIME_Counter64Get();

    rfStats = &palRfData.stats;

    /* Get Payload Length */
    frameLen  = rxFrame->mpdu[0];
    if (frameLen  > aMaxPHYPacketSize)
    {
        /* Free-up the buffer which was used for reception once the frame is extracted. */
        bmm_buffer_free(rxFrame->buffer_header);

        rfStats->rxErrorLength++;
        return;
    }

    /* Get Payload */
    (void) memcpy(palRfData.rxBuffer, &rxFrame->mpdu[1], frameLen);
    /* Get LQI */
    palRfData.lastRxPktLQI = rxFrame->mpdu[frameLen + LQI_LEN];
    /* Get ED_LEVEL */
    palRfData.lastRxPktED = (int8_t)rxFrame->mpdu[frameLen + LQI_LEN + ED_VAL_LEN];
    /* Get RSSI */
    rxParameters.rssi = palRfData.lastRxPktED + PHY_GetRSSIBaseVal();
    rxParameters.fcsOk = true;

    /* Update RX statistics */
    rfStats->rxTotalPackets++;
    rfStats->rxTotalBytes += frameLen;

    /* The number of symbols per octet for the current PHY is 2 */
    rfStats->rxLastPaySymbols = (uint16_t)frameLen;
    rfStats->rxLastPaySymbols <<= 1;
    /* The duration of the synchronization header (SHR) in symbols is 10 */
    phyDurationSymb = 10U + (uint32_t)rfStats->rxLastPaySymbols;
    /* Symbol rate : 62.5 Ksymb/s -> 16us/symbol */
    rxParameters.timeIniCount = rxTimeEndCount - SYS_TIME_USToCount(phyDurationSymb << 4);
    rxParameters.timeEndCount = rxTimeEndCount;

    /* Free-up the buffer which was used for reception once the frame is extracted. */
    bmm_buffer_free(rxFrame->buffer_header);

    if (palRfData.rfPhyHandlers.palRfDataIndication != NULL)
    {
        palRfData.rfPhyHandlers.palRfDataIndication(palRfData.rxBuffer, frameLen, &rxParameters);
    }

}

/* MISRA C-2012 deviation block end */

static void lPAL_RF_DelayedTxCallback(uintptr_t context)
{
    palRfData.txDelayedPending = true;

    /* Signal thread to transmit a new packet */
    (void) OSAL_SEM_PostISR(&palRFSemID);
}

// *****************************************************************************
// *****************************************************************************
// Section: Interface Function Definitions
// *****************************************************************************
// *****************************************************************************

SYS_MODULE_OBJ PAL_RF_Initialize(const SYS_MODULE_INDEX index,
                                 const SYS_MODULE_INIT * const init)
{
    /* MISRA C-2012 deviation block start */
    /* MISRA C-2012 Rule 11.3 deviated once. Deviation record ID - H3_MISRAC_2012_R_11_3_DR_1 */
    const PAL_RF_INIT * const palInit = (const PAL_RF_INIT * const)init;
    /* MISRA C-2012 deviation block end */

    /* Check Single instance */
    if (index != PAL_RF_PHY_INDEX)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    /* Check previously initialized */
    if (palRfData.status != PAL_RF_STATUS_UNINITIALIZED)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    palRfData.rfPhyHandlers.palRfDataIndication = palInit->rfPhyHandlers.palRfDataIndication;
    palRfData.rfPhyHandlers.palRfTxConfirm = palInit->rfPhyHandlers.palRfTxConfirm;

    /* Get Phy SW Version */
    palRfData.phySWVersion = PHY_GetSWVersion();

    /* Init Pal RF Data */
    palRfData.txDelayedPending = false;
    palRfData.txTransmitting = false;
    palRfData.txContinuousMode = false;
    palRfData.sleepMode = false;
    (void) memset(&palRfData.stats, 0, sizeof (palRfData.stats));
    palRfData.txTimer = SYS_TIME_HANDLE_INVALID;

    /* Set RF Network Parameters */
    if (lPAL_RF_setRFNetworkParameters() != PAL_RF_PIB_SUCCESS)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    /* Enable Transceiver */
    if (PHY_RxEnable(PHY_STATE_RX_ON) != PHY_RX_ON)
    {
        return SYS_MODULE_OBJ_INVALID;
    }


    /* Create the PAL RF Semaphore */
    if (OSAL_SEM_Create(&palRFSemID, OSAL_SEM_TYPE_BINARY, 0, 0) != OSAL_RESULT_SUCCESS)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    /* Update Status */
    palRfData.status = PAL_RF_STATUS_READY;

    return (SYS_MODULE_OBJ)PAL_RF_PHY_INDEX;
}

PAL_RF_HANDLE PAL_RF_HandleGet(const SYS_MODULE_INDEX index)
{
    /* Check Single instance */
    if (index != PAL_RF_PHY_INDEX)
    {
        return PAL_RF_HANDLE_INVALID;
    }

    /* Check previously initialized */
    if (palRfData.status == PAL_RF_STATUS_UNINITIALIZED)
    {
        return PAL_RF_HANDLE_INVALID;
    }

    return (PAL_RF_HANDLE)&palRfData;
}

PAL_RF_STATUS PAL_RF_Status(SYS_MODULE_OBJ object)
{
    if (object != (SYS_MODULE_OBJ)PAL_RF_PHY_INDEX)
    {
        return PAL_RF_STATUS_INVALID_OBJECT;
    }

    return palRfData.status;
}

void PAL_RF_Deinitialize(SYS_MODULE_OBJ object)
{
    if (object != (SYS_MODULE_OBJ)PAL_RF_PHY_INDEX)
    {
        return;
    }

    /* Check status */
    if (palRfData.status == PAL_RF_STATUS_UNINITIALIZED)
    {
        return;
    }

    /* Disable Transceiver */
    (void) PHY_RxEnable(PHY_STATE_TRX_OFF);

    /* Delete Semaphore */
    (void) OSAL_SEM_Delete(&palRFSemID);

    palRfData.status = PAL_RF_STATUS_UNINITIALIZED;
}

void PAL_RF_Tasks(void)
{
    PAL_RF_DATA *palRf = &palRfData;

    if (palRf->status == PAL_RF_STATUS_UNINITIALIZED)
    {
        vTaskDelay(50U / portTICK_PERIOD_MS);
        return;
    }

    /* Wait for the PAL RF semaphore to get to process Tx/Rx packets */
    (void) OSAL_SEM_Pend(&palRFSemID, 10U); //OSAL_WAIT_FOREVER);

    palRf->phyStatus = PHY_GetTrxStatus();
    if (palRf->phyStatus == PHY_TRX_OFF)
    {
        /* Enable Transceiver */
        (void) PHY_RxEnable(PHY_STATE_RX_ON);

        palRf->stats.phyResets++;
        if (palRfData.txTransmitting == true)
        {
            palRfData.stats.phyResetsInTx++;
            palRfData.txTransmitting = false;
        }
    }

    if (palRf->txDelayedPending)
    {
        uint8_t phyStatus;
        PAL_RF_PHY_STATUS palRfStatus;

        palRf->txDelayedPending = false;
        palRf->txTimer = SYS_TIME_HANDLE_INVALID;

        palRf->txTimeIniCount = SYS_TIME_Counter64Get();
        phyStatus = (uint8_t)PHY_TxFrame(&palRf->txFrame, palRf->csmaMode, false);
        palRfStatus = palRfPhyStatus[phyStatus & 0x0FU];
        if (palRfStatus != PAL_RF_PHY_SUCCESS)
        {
            // Report TX CFM
            lPAL_RF_ReportResultTX(palRfStatus, palRf->txTimeIniCount, 0);
        }
        else
        {
            palRfData.txTransmitting = true;
        }
    }
}

PAL_RF_TX_HANDLE PAL_RF_TxRequest(PAL_RF_HANDLE handle, uint8_t *pData,
                                  uint16_t length, PAL_RF_TX_PARAMETERS *txParameters)
{
    int64_t timeDiffCount;
    uint32_t dalayTimeUs;
    int16_t pwrDbm;

    palRfData.stats.txCfmNotHandled++;
    palRfData.stats.txTotalTxRequest++;

    if ((handle != (PAL_RF_HANDLE) & palRfData) ||
        (palRfData.txContinuousMode == true) || (palRfData.sleepMode == true))
    {
        // Report TX CFM error
        lPAL_RF_ReportResultTX(PAL_RF_PHY_ERROR, SYS_TIME_Counter64Get(), 0);

        return PAL_RF_TX_HANDLE_INVALID;
    }

    if (palRfData.txTimer != SYS_TIME_HANDLE_INVALID)
    {
        // Report TX CFM error
        lPAL_RF_ReportResultTX(PAL_RF_PHY_BUSY_TX, SYS_TIME_Counter64Get(), 0);

        return PAL_RF_TX_HANDLE_INVALID;
    }

    if (length > aMaxPHYPacketSize)
    {
        // Report TX CFM error
        lPAL_RF_ReportResultTX(PAL_RF_PHY_INVALID_PARAM, SYS_TIME_Counter64Get(), 0);

        return PAL_RF_TX_HANDLE_INVALID;
    }

    palRfData.txFrame.mpdu = palRfData.txBuffer;
    palRfData.txBuffer[0] = (uint8_t)length;
    (void) memcpy(&palRfData.txBuffer[1], pData, (uint8_t)length);

    if (txParameters->csmaEnable)
    {
        palRfData.csmaMode = CSMA_UNSLOTTED;
    }
    else
    {
        palRfData.csmaMode = NO_CSMA_NO_IFS;
    }

    /* Set Tx Power (Power value in dBm (-14dBm to 12dBm))*/
    pwrDbm = 12 - (int16_t)txParameters->txPowerAttenuation;
    if (pwrDbm < -14)
    {
        pwrDbm = -14;
    }

    (void) PHY_ConfigTxPwr(PWR_DBM_VALUE, (int8_t)pwrDbm);

    timeDiffCount = (int64_t)txParameters->timeCount - (int64_t)SYS_TIME_Counter64Get();
    if (timeDiffCount <= 0)
    {
        dalayTimeUs = 0U;
    }
    else
    {
        dalayTimeUs = SYS_TIME_CountToUS((uint32_t)timeDiffCount);
        if (dalayTimeUs < 1000U)
        {
            dalayTimeUs = 0U;
        }
    }

    if (dalayTimeUs > 0U)
    {
        palRfData.txTimer = SYS_TIME_CallbackRegisterUS(lPAL_RF_DelayedTxCallback,
                (uintptr_t) & palRfData, dalayTimeUs, SYS_TIME_SINGLE);

        if (palRfData.txTimer == SYS_TIME_HANDLE_INVALID)
        {
            // Report TX CFM error
            lPAL_RF_ReportResultTX(PAL_RF_PHY_TIMEOUT, SYS_TIME_Counter64Get(), 0);
            return PAL_RF_TX_HANDLE_INVALID;
        }
    }
    else
    {
        uint8_t phyStatus;
        PAL_RF_PHY_STATUS palRfStatus;

        // Immediate transmission
        palRfData.txTimeIniCount = SYS_TIME_Counter64Get();
        phyStatus = (uint8_t)PHY_TxFrame(&palRfData.txFrame, palRfData.csmaMode, false);
        palRfStatus = palRfPhyStatus[phyStatus & 0x0FU];
        if (palRfStatus != PAL_RF_PHY_SUCCESS)
        {
            // Report TX CFM error
            lPAL_RF_ReportResultTX(palRfStatus, palRfData.txTimeIniCount, 0);
            return PAL_RF_TX_HANDLE_INVALID;
        }
        else
        {
            palRfData.txTransmitting = true;
        }
    }

    return (PAL_RF_HANDLE)&palRfData;
}

void PAL_RF_TxCancel(PAL_RF_HANDLE handle, PAL_RF_TX_HANDLE txHandle)
{
    if (handle != (PAL_RF_HANDLE) & palRfData)
    {
        return;
    }

    if (txHandle != (PAL_RF_HANDLE) & palRfData)
    {
        return;
    }

    if ((palRfData.txContinuousMode) || (palRfData.sleepMode))
    {
        return;
    }

    if (palRfData.txTimer != SYS_TIME_HANDLE_INVALID)
    {
        (void) SYS_TIME_TimerDestroy(palRfData.txTimer);
        palRfData.txTimer = SYS_TIME_HANDLE_INVALID;
    }

    if (palRfData.txTransmitting == true)
    {
        palRfData.txTransmitting = false;

        /* Reset Transceiver : no set PIBs to the default values */
        (void) PHY_Reset(false);

        /* Enable Transceiver */
        (void) PHY_RxEnable(PHY_STATE_RX_ON);
    }
}

void PAL_RF_Reset(PAL_RF_HANDLE handle)
{
    if (handle != (PAL_RF_HANDLE) & palRfData)
    {
        return;
    }

    if (palRfData.txTimer != SYS_TIME_HANDLE_INVALID)
    {
        (void) SYS_TIME_TimerDestroy(palRfData.txTimer);
        palRfData.txTimer = SYS_TIME_HANDLE_INVALID;

        /* Report TX canceled */
        lPAL_RF_ReportResultTX(PAL_RF_PHY_TX_CANCELLED, SYS_TIME_Counter64Get(), 0);
    }

    /* Reset Transceiver : no set PIBs to the default values */
    (void) PHY_Reset(false);

    /* Enable Transceiver */
    (void) PHY_RxEnable(PHY_STATE_RX_ON);

    /* Clear Pal Rf data flags and stats */
    palRfData.txDelayedPending = false;
    palRfData.txTransmitting = false;
    palRfData.txContinuousMode = false;
    palRfData.sleepMode = false;
    (void) memset(&palRfData.stats, 0, sizeof (palRfData.stats));
}

PAL_RF_PIB_RESULT PAL_RF_GetRfPhyPib(PAL_RF_HANDLE handle, PAL_RF_PIB_OBJ *pibObj)
{
    PAL_RF_PIB_RESULT result = PAL_RF_PIB_SUCCESS;
    void *pData;

    if (handle != (PAL_RF_HANDLE) & palRfData)
    {
        return PAL_RF_PIB_INVALID_HANDLE;
    }

    if (palRfData.status != PAL_RF_STATUS_READY)
    {
        /* Ignore request */
        return PAL_RF_PIB_ERROR;
    }

    pData = (void *)pibObj->pData;

    switch (pibObj->pib)
    {
    case PAL_RF_PIB_DEVICE_ID:
        *((uint16_t *)pData) = 0x451U;
        break;

    case PAL_RF_PIB_FW_VERSION:
        *((uint32_t *)pData) = palRfData.phySWVersion;
        break;

    case PAL_RF_PIB_TRX_SLEEP:
        *((bool *)pData) = palRfData.sleepMode;
        break;

    case PAL_RF_PIB_PHY_CHANNEL_NUM:
        *((uint16_t *)pData) = (uint16_t)palRfData.network.channel;
        break;

    case PAL_RF_PIB_PHY_CHANNEL_FREQ_HZ:
        // fc = 2405 + 5 (k ? 11) in megahertz, for k = 11, 12, ?, 26
        // k is the channel number
        *((uint32_t *)pData) = (2045U +
                5U * ((uint32_t)palRfData.network.channel - 11U)) *
                1000000UL;
        break;

    case PAL_RF_PIB_PHY_CHANNEL_PAGE:
        pibObj->pData[0] = palRfData.network.channelPage;
        break;

    case PAL_RF_PIB_PHY_CHANNELS_SUPPORTED:
        (void) PHY_PibGet(phyChannelsSupported, pibObj->pData);
        break;

    case PAL_RF_PIB_PHY_CCA_ED_DURATION_US:
        // 8 symbols(128us)
        *((uint16_t *)pData) = (uint16_t)8U << 4;
        break;

    case PAL_RF_PIB_PHY_CCA_ED_DURATION_SYMBOLS:
        // 8 symbols
        *((uint16_t *)pData) = (uint16_t)8U;
        break;

    case PAL_RF_PIB_PHY_CCA_ED_SAMPLE:
    {
        uint8_t edLevel = PHY_EdSample();

        // Convert the energy level to input power in Dbm
        int8_t pwrDbm = (int8_t)edLevel + PHY_GetRSSIBaseVal();

        *((int8_t *)pData) = pwrDbm;
        break;
    }

    case PAL_RF_PIB_PHY_CCA_ED_THRESHOLD_DBM:
    {
        *((int8_t *)pData) = -90;
        break;
    }

    case PAL_RF_PIB_PHY_CCA_ED_THRESHOLD_SENSITIVITY:
    {
        uint8_t pdtLevel;

        // Get RSSI base value of TRX
        int8_t trxBaseRSSI = PHY_GetRSSIBaseVal();

        // To get the PDT level configured
        (void) PHY_GetTrxConfig(RX_SENS, &pdtLevel);

        // THRS = RSSIBASE_VAL + 3 x (pdtLevel - 1)
        if (pdtLevel > 0U)
        {
            pdtLevel = 3U * (pdtLevel - 1U);
        }

        *((int8_t *)pData) = -90 - (trxBaseRSSI + (int8_t)pdtLevel);
        break;
    }

    case PAL_RF_PIB_PHY_SENSITIVITY:
    {
        uint8_t pdtLevel;

        // Get RSSI base value of TRX
        int8_t trxBaseRSSI = PHY_GetRSSIBaseVal();

        // To get the PDT level configured
        (void) PHY_GetTrxConfig(RX_SENS, &pdtLevel);

        // THRS = RSSIBASE_VAL + 3 x (pdtLevel - 1)
        if (pdtLevel > 0U)
        {
            pdtLevel = 3U * (pdtLevel - 1U);
        }

        *((int8_t *)pData) = trxBaseRSSI + (int8_t)pdtLevel;
        break;
    }

    case PAL_RF_PIB_PHY_MAX_TX_POWER:
        *((int8_t *)pData) = 12;
        break;

    case PAL_RF_PIB_PHY_TURNAROUND_TIME:
        /* aTurnaroundTime[symb] * 16 us/symb */
        *((uint16_t *)pData) = (uint16_t)aTurnaroundTime << 4;
        break;

    case PAL_RF_PIB_PHY_TX_PAY_SYMBOLS:
        *((uint16_t *)pData) = palRfData.stats.txLastPaySymbols;
        break;

    case PAL_RF_PIB_PHY_RX_PAY_SYMBOLS:
        *((uint16_t *)pData) = palRfData.stats.rxLastPaySymbols;
        break;

    case PAL_RF_PIB_PHY_TX_TOTAL:
        *((uint32_t *)pData) = palRfData.stats.txTotalPackets;
        break;

    case PAL_RF_PIB_PHY_TX_TOTAL_BYTES:
        *((uint32_t *)pData) = palRfData.stats.txTotalBytes;
        break;

    case PAL_RF_PIB_PHY_TX_ERR_TOTAL:
        *((uint32_t *)pData) = palRfData.stats.txTotalErrors;
        break;

    case PAL_RF_PIB_PHY_TX_ERR_BUSY_TX:
        *((uint32_t *)pData) = palRfData.stats.txErrorBusyTX;
        break;

    case PAL_RF_PIB_PHY_TX_ERR_BUSY_RX:
        *((uint32_t *)pData) = palRfData.stats.txErrorBusyRX;
        break;

    case PAL_RF_PIB_PHY_TX_ERR_BUSY_CHN:
        *((uint32_t *)pData) = palRfData.stats.txErrorBusyChannel;
        break;

    case PAL_RF_PIB_PHY_TX_ERR_BAD_LEN:
        *((uint32_t *)pData) = palRfData.stats.txErrorLength;
        break;

    case PAL_RF_PIB_PHY_TX_ERR_BAD_FORMAT:
        *((uint32_t *)pData) = palRfData.stats.txErrorFormat;
        break;

    case PAL_RF_PIB_PHY_TX_ERR_TIMEOUT:
        *((uint32_t *)pData) = palRfData.stats.txErrorTimeout;
        break;

    case PAL_RF_PIB_PHY_TX_ERR_ABORTED:
        *((uint32_t *)pData) = palRfData.stats.txErrorAborted;
        break;

    case PAL_RF_PIB_PHY_TX_CFM_NOT_HANDLED:
        *((uint32_t *)pData) = palRfData.stats.txCfmNotHandled;
        break;

    case PAL_RF_PIB_PHY_RX_TOTAL:
        *((uint32_t *)pData) = palRfData.stats.rxTotalPackets;
        break;

    case PAL_RF_PIB_PHY_RX_TOTAL_BYTES:
        *((uint32_t *)pData) = palRfData.stats.rxTotalBytes;
        break;

    case PAL_RF_PIB_PHY_RX_ERR_TOTAL:
        *((uint32_t *)pData) = palRfData.stats.rxTotalErrors;
        break;

    case PAL_RF_PIB_PHY_RX_ERR_FALSE_POSITIVE:
        *((uint32_t *)pData) = palRfData.stats.rxErrorFalsePositive;
        break;

    case PAL_RF_PIB_PHY_RX_ERR_BAD_LEN:
        *((uint32_t *)pData) = palRfData.stats.rxErrorLength;
        break;

    case PAL_RF_PIB_PHY_RX_ERR_BAD_FORMAT:
        *((uint32_t *)pData) = palRfData.stats.rxErrorFormat;
        break;

    case PAL_RF_PIB_PHY_RX_ERR_BAD_FCS_PAY:
        *((uint32_t *)pData) = palRfData.stats.rxErrorFCS;
        break;

    case PAL_RF_PIB_PHY_RX_ERR_ABORTED:
        *((uint32_t *)pData) = palRfData.stats.rxErrorAborted;
        break;

    case PAL_RF_PIB_PHY_RX_OVERRIDE:
        *((uint32_t *)pData) = palRfData.stats.rxErrorOverride;
        break;

    case PAL_RF_PIB_PHY_RX_IND_NOT_HANDLED:
        *((uint32_t *)pData) = palRfData.stats.rxErrorRcvDataNotHandled;
        break;

    case PAL_RF_PIB_SET_CONTINUOUS_TX_MODE:
    {
        *((bool *)pData) = palRfData.txContinuousMode;
        break;
    }

    case PAL_RF_PIB_MAC_UNIT_BACKOFF_PERIOD:
        /* aUnitBackoffPeriod[symb] * 16 us/symb */
        *((uint16_t *)pData) = (uint16_t)aUnitBackoffPeriod << 4;
        break;

    case PAL_RF_PIB_DEVICE_RESET:
    case PAL_RF_PIB_TRX_RESET:
    case PAL_RF_PIB_PHY_STATS_RESET:
        result = PAL_RF_PIB_WRITE_ONLY;
        break;

    case PAL_RF_PIB_TX_FSK_FEC:
    case PAL_RF_PIB_TX_OFDM_MCS:
    case PAL_RF_PIB_PHY_BAND_OPERATING_MODE:
    case PAL_RF_PIB_PHY_CONFIG:
    default:
        result = PAL_RF_PIB_INVALID_ATTR;
        break;
    }

    return result;
}

PAL_RF_PIB_RESULT PAL_RF_SetRfPhyPib(PAL_RF_HANDLE handle, PAL_RF_PIB_OBJ *pibObj)
{
    PAL_RF_PIB_RESULT result = PAL_RF_PIB_SUCCESS;
    void *pData = (void *)pibObj->pData;

    if (handle != (PAL_RF_HANDLE) & palRfData)
    {
        return PAL_RF_PIB_INVALID_HANDLE;
    }

    if (palRfData.status != PAL_RF_STATUS_READY)
    {
        /* Ignore request */
        return PAL_RF_PIB_ERROR;
    }

    switch (pibObj->pib)
    {
    case PAL_RF_PIB_DEVICE_RESET:
    case PAL_RF_PIB_TRX_RESET:
    {
        PAL_RF_Reset((PAL_RF_HANDLE) & palRfData);

        break;
    }

    case PAL_RF_PIB_TRX_SLEEP:
    {
        uint8_t sleep = pibObj->pData[0];

        if ((sleep != 0U) && (!palRfData.sleepMode))
        {
            palRfData.sleepMode = true;
            if (PHY_SUCCESS != PHY_TrxSleep(SLEEP_MODE_1))
            {
                result = PAL_RF_PIB_ERROR;
            }
        }
        else
        {
            if ((sleep == 0U) && (palRfData.sleepMode))
            {
                palRfData.sleepMode = false;
                if (PHY_SUCCESS != PHY_TrxWakeup())
                {
                    result = PAL_RF_PIB_ERROR;
                }
            }
        }

        break;
    }

    case PAL_RF_PIB_PHY_CHANNEL_PAGE:
    {
        PibValue_t chnPage;

        chnPage.pib_value_8bit = pibObj->pData[0];
        if ((chnPage.pib_value_8bit == 0U) || (chnPage.pib_value_8bit == 2U) ||
                (chnPage.pib_value_8bit == 16U) || (chnPage.pib_value_8bit == 17U))
        {
            if (PHY_PibSet(phyCurrentPage, &chnPage) != PHY_SUCCESS)
            {
                result = PAL_RF_PIB_ERROR;
            }
        }
        else
        {
            result = PAL_RF_PIB_INVALID_PARAM;
        }

        break;
    }

    case PAL_RF_PIB_PHY_CHANNEL_NUM:
    {
        PibValue_t chnNum;

        chnNum.pib_value_8bit = pibObj->pData[0];
        if ((chnNum.pib_value_8bit >= 11U) && (chnNum.pib_value_8bit <= 26U))
        {
            if (PHY_PibSet(phyCurrentChannel, &chnNum) != PHY_SUCCESS)
            {
                result = PAL_RF_PIB_ERROR;
            }
        }
        else
        {
            result = PAL_RF_PIB_INVALID_PARAM;
        }

        break;
    }

    case PAL_RF_PIB_PHY_CHANNELS_SUPPORTED:
    {
        PibValue_t chnSupported;

        chnSupported.pib_value_32bit = *(uint32_t *)pData;
        if ((chnSupported.pib_value_32bit & 0xF80007FFUL) != 0U)
        {
            result = PAL_RF_PIB_INVALID_PARAM;
        }
        else
        {
            if (PHY_PibSet(phyChannelsSupported, &chnSupported) != PHY_SUCCESS)
            {
                result = PAL_RF_PIB_ERROR;
            }
        }

        break;
    }

    case PAL_RF_PIB_PHY_CCA_ED_THRESHOLD_DBM:
    case PAL_RF_PIB_PHY_CCA_ED_THRESHOLD_SENSITIVITY:
    case PAL_RF_PIB_PHY_CCA_ED_SAMPLE:
    case PAL_RF_PIB_PHY_CCA_ED_DURATION_US:
    case PAL_RF_PIB_PHY_CCA_ED_DURATION_SYMBOLS:
    case PAL_RF_PIB_DEVICE_ID:
    case PAL_RF_PIB_FW_VERSION:
    case PAL_RF_PIB_PHY_TURNAROUND_TIME:
    case PAL_RF_PIB_PHY_CHANNEL_FREQ_HZ:
    case PAL_RF_PIB_PHY_TX_PAY_SYMBOLS:
    case PAL_RF_PIB_PHY_RX_PAY_SYMBOLS:
    case PAL_RF_PIB_PHY_TX_TOTAL:
    case PAL_RF_PIB_PHY_TX_TOTAL_BYTES:
    case PAL_RF_PIB_PHY_TX_ERR_TOTAL:
    case PAL_RF_PIB_PHY_TX_ERR_BUSY_TX:
    case PAL_RF_PIB_PHY_TX_ERR_BUSY_RX:
    case PAL_RF_PIB_PHY_TX_ERR_BUSY_CHN:
    case PAL_RF_PIB_PHY_TX_ERR_BAD_LEN:
    case PAL_RF_PIB_PHY_TX_ERR_BAD_FORMAT:
    case PAL_RF_PIB_PHY_TX_ERR_TIMEOUT:
    case PAL_RF_PIB_PHY_TX_ERR_ABORTED:
    case PAL_RF_PIB_PHY_TX_CFM_NOT_HANDLED:
    case PAL_RF_PIB_PHY_RX_TOTAL:
    case PAL_RF_PIB_PHY_RX_TOTAL_BYTES:
    case PAL_RF_PIB_PHY_RX_ERR_TOTAL:
    case PAL_RF_PIB_PHY_RX_ERR_FALSE_POSITIVE:
    case PAL_RF_PIB_PHY_RX_ERR_BAD_LEN:
    case PAL_RF_PIB_PHY_RX_ERR_BAD_FORMAT:
    case PAL_RF_PIB_PHY_RX_ERR_BAD_FCS_PAY:
    case PAL_RF_PIB_PHY_RX_ERR_ABORTED:
    case PAL_RF_PIB_PHY_RX_OVERRIDE:
    case PAL_RF_PIB_PHY_RX_IND_NOT_HANDLED:
    case PAL_RF_PIB_MAC_UNIT_BACKOFF_PERIOD:
        result = PAL_RF_PIB_READ_ONLY;
        break;

    case PAL_RF_PIB_PHY_STATS_RESET:
        (void) memset(&palRfData.stats, 0, sizeof (palRfData.stats));
        break;

    case PAL_RF_PIB_SET_CONTINUOUS_TX_MODE:
    {
        uint8_t enable = pibObj->pData[0];

        if ((enable != 0U) && (!palRfData.txContinuousMode))
        {
            palRfData.txContinuousMode = true;
            PHY_StartContinuousTransmit(PRBS_MODE, true);
        }
        else
        {
            if ((enable == 0U) && (palRfData.txContinuousMode))
            {
                palRfData.txContinuousMode = false;
                PHY_StopContinuousTransmit();
            }
        }

        break;
    }

    case PAL_RF_PIB_PHY_CONFIG:
    case PAL_RF_PIB_PHY_BAND_OPERATING_MODE:
    case PAL_RF_PIB_TX_FSK_FEC:
    case PAL_RF_PIB_TX_OFDM_MCS:
    default:
        result = PAL_RF_PIB_INVALID_ATTR;
        break;
    }

    return result;
}

uint8_t PAL_RF_GetRfPhyPibLength(PAL_RF_HANDLE handle, PAL_RF_PIB_ATTRIBUTE attribute)
{
    uint8_t pibLen = 0;

    if (handle != (PAL_RF_HANDLE) & palRfData)
    {
        return 0;
    }

    switch (attribute)
    {
    case PAL_RF_PIB_DEVICE_RESET:
    case PAL_RF_PIB_TRX_RESET:
    case PAL_RF_PIB_TRX_SLEEP:
    case PAL_RF_PIB_PHY_CCA_ED_THRESHOLD_DBM:
    case PAL_RF_PIB_PHY_CCA_ED_THRESHOLD_SENSITIVITY:
    case PAL_RF_PIB_PHY_CCA_ED_SAMPLE:
    case PAL_RF_PIB_PHY_STATS_RESET:
    case PAL_RF_PIB_SET_CONTINUOUS_TX_MODE:
    case PAL_RF_PIB_PHY_CHANNEL_PAGE:
        pibLen = 1;
        break;

    case PAL_RF_PIB_DEVICE_ID:
    case PAL_RF_PIB_PHY_CHANNEL_NUM:
    case PAL_RF_PIB_PHY_CCA_ED_DURATION_US:
    case PAL_RF_PIB_PHY_CCA_ED_DURATION_SYMBOLS:
    case PAL_RF_PIB_PHY_TURNAROUND_TIME:
    case PAL_RF_PIB_PHY_TX_PAY_SYMBOLS:
    case PAL_RF_PIB_PHY_RX_PAY_SYMBOLS:
    case PAL_RF_PIB_MAC_UNIT_BACKOFF_PERIOD:
        pibLen = 2;
        break;

    case PAL_RF_PIB_FW_VERSION:
    case PAL_RF_PIB_PHY_CHANNEL_FREQ_HZ:
    case PAL_RF_PIB_PHY_CHANNELS_SUPPORTED:
    case PAL_RF_PIB_PHY_TX_TOTAL:
    case PAL_RF_PIB_PHY_TX_TOTAL_BYTES:
    case PAL_RF_PIB_PHY_TX_ERR_TOTAL:
    case PAL_RF_PIB_PHY_TX_ERR_BUSY_TX:
    case PAL_RF_PIB_PHY_TX_ERR_BUSY_RX:
    case PAL_RF_PIB_PHY_TX_ERR_BUSY_CHN:
    case PAL_RF_PIB_PHY_TX_ERR_BAD_LEN:
    case PAL_RF_PIB_PHY_TX_ERR_BAD_FORMAT:
    case PAL_RF_PIB_PHY_TX_ERR_TIMEOUT:
    case PAL_RF_PIB_PHY_TX_ERR_ABORTED:
    case PAL_RF_PIB_PHY_TX_CFM_NOT_HANDLED:
    case PAL_RF_PIB_PHY_RX_TOTAL:
    case PAL_RF_PIB_PHY_RX_TOTAL_BYTES:
    case PAL_RF_PIB_PHY_RX_ERR_TOTAL:
    case PAL_RF_PIB_PHY_RX_ERR_FALSE_POSITIVE:
    case PAL_RF_PIB_PHY_RX_ERR_BAD_LEN:
    case PAL_RF_PIB_PHY_RX_ERR_BAD_FORMAT:
    case PAL_RF_PIB_PHY_RX_ERR_BAD_FCS_PAY:
    case PAL_RF_PIB_PHY_RX_ERR_ABORTED:
    case PAL_RF_PIB_PHY_RX_OVERRIDE:
    case PAL_RF_PIB_PHY_RX_IND_NOT_HANDLED:
        pibLen = 4;
        break;

    case PAL_RF_PIB_PHY_CONFIG:
    case PAL_RF_PIB_PHY_BAND_OPERATING_MODE:
    case PAL_RF_PIB_TX_FSK_FEC:
    case PAL_RF_PIB_TX_OFDM_MCS:
    default:
        pibLen = 0;
        break;
    }

    return pibLen;
}
