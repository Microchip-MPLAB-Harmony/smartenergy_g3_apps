/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    pal_rf.c

  Summary:
    Platform Abstraction Layer RF (PAL RF) Interface source file.

  Description:
    Platform Abstraction Layer RF (PAL RF) Interface header.
    The PAL RF module provides a simple interface to manage the RF PHY driver.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
 * Copyright (C) 2022 Microchip Technology Inc. and its subsidiaries.
 *
 * Subject to your compliance with these terms, you may use Microchip software
 * and any derivatives exclusively with Microchip products. It is your
 * responsibility to comply with third party license terms applicable to your
 * use of third party software (including open source software) that may
 * accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
 * ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *******************************************************************************/
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
#include "IEEE_802154_PHY/phy/inc/phy.h"
#include "IEEE_802154_PHY/phy/inc/phy_tasks.h"
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
    PAL_RF_PHY_SUCCESS, // PHY_SUCCESS
    PAL_RF_PHY_TRX_OFF, // PHY_TRX_ASLEEP
    PAL_RF_PHY_TRX_OFF, // PHY_TRX_AWAKE
    PAL_RF_PHY_TX_CANCELLED, // PHY_RF_REQ_ABORTED
    PAL_RF_PHY_TRX_OFF, // PHY_RF_UNAVAILABLE
    PAL_RF_PHY_ERROR, // PHY_FAILURE
    PAL_RF_PHY_BUSY_TX, // PHY_BUSY
    PAL_RF_PHY_BUSY_TX, // PHY_FRAME_PENDING
    PAL_RF_PHY_INVALID_PARAM, // PHY_INVALID_PARAMETER
    PAL_RF_PHY_INVALID_PARAM, // PHY_UNSUPPORTED_ATTRIBUTE
    PAL_RF_PHY_BUSY_TX, // PHY_CHANNEL_BUSY
    PAL_RF_PHY_TRX_OFF, // PHY_CHANNEL_IDLE
    PAL_RF_PHY_ERROR, // PHY_NO_ACK
    PAL_RF_PHY_CHANNEL_ACCESS_FAILURE // PHY_CHANNEL_ACCESS_FAILURE
};

// Transceiver Configuration
#define CHANNEL_TRANSMIT_RECEIVE (11U)
#define CHANNEL_PAGE_TRANSMIT_RECEIVE (0U)
#define CCA_MODE (1U)
#define TRANSMIT_POWER_DBm (12U)
#define PROMISCUOUS_MODE (true)
#define AUTOACK_MODE (false)

/* Define a semaphore to signal the PAL RF Tasks to process Tx/Rx packets */
OSAL_SEM_DECLARE(palRFSemID);

// *****************************************************************************
// *****************************************************************************
// Section: local callbacks
// *****************************************************************************
// *****************************************************************************
void PHY_TxDoneCallback(PHY_Retval_t status, PHY_FrameInfo_t *frame)
{
    uint64_t timeIniCount;
    uint64_t timeEndCount;
    PAL_RF_STATISTICS *rfStats;
    uint16_t phyDurationSymb;
    PAL_RF_PHY_STATUS palRfStatus;
    PHY_Retval_t attributeStatus;
    uint8_t phySHRSymbols;
    uint8_t phyNumSymbols;
    uint8_t nBytesSent;

    rfStats = &palRfData.stats;
    rfStats->txCfmNotHandled--;

    /* Get received data */
    nBytesSent = frame->mpdu[0];

    /* Get Frame Times */
    timeEndCount = SYS_TIME_Counter64Get();
    attributeStatus = PHY_PibGet(phySHRDuration, &phySHRSymbols);
    if (attributeStatus != PHY_SUCCESS)
    {
        phySHRSymbols = 0;
    }

    attributeStatus = PHY_PibGet(phySymbolsPerOctet, &phyNumSymbols);
    if (attributeStatus != PHY_SUCCESS)
    {
        phyNumSymbols = 0;
    }

    rfStats->txLastPaySymbols = nBytesSent * phyNumSymbols;
    phyDurationSymb = phySHRSymbols + rfStats->txLastPaySymbols;
    timeIniCount = timeEndCount - SYS_TIME_USToCount(phyDurationSymb << 4);

    // DBG: Check calculations - difference between IniCountCalc and IniCount
    palRfData.txTimeEndCount = timeEndCount;
    palRfData.txTimeIniCountCalc = timeIniCount;

    switch (status)
    {
    case PHY_SUCCESS:
        palRfStatus = PAL_RF_PHY_SUCCESS;
        rfStats->txTotalBytes += nBytesSent;
        rfStats->txTotalPackets++;
        break;

    case PHY_TRX_ASLEEP:
        palRfStatus = PAL_RF_PHY_TRX_OFF;
        rfStats->txTotalErrors++;
        break;

    case PHY_TRX_AWAKE:
        palRfStatus = PAL_RF_PHY_TRX_OFF;
        rfStats->txTotalErrors++;
        break;

    case PHY_RF_REQ_ABORTED:
        palRfStatus = PAL_RF_PHY_TX_CANCELLED;
        rfStats->txErrorAborted++;
        break;

    case PHY_RF_UNAVAILABLE:
        palRfStatus = PAL_RF_PHY_TRX_OFF;
        rfStats->txTotalErrors++;
        break;

    case PHY_FAILURE:
        palRfStatus = PAL_RF_PHY_ERROR;
        rfStats->txTotalErrors++;
        break;

    case PHY_BUSY:
    {
        palRfStatus = PAL_RF_PHY_BUSY_TX;

        // Gets the current status of trx
        if (PHY_GetTrxStatus() == PHY_BUSY_RX)
        {
            rfStats->txErrorBusyRX++;
        }
        else
        {
            rfStats->txErrorBusyTX++;
        }
        break;
    }

    case PHY_FRAME_PENDING:
        palRfStatus = PAL_RF_PHY_BUSY_TX;
        rfStats->txErrorBusyTX++;
        break;

    case PHY_INVALID_PARAMETER:
        palRfStatus = PAL_RF_PHY_INVALID_PARAM;
        rfStats->txErrorFormat++;
        break;

    case PHY_UNSUPPORTED_ATTRIBUTE:
        palRfStatus = PAL_RF_PHY_INVALID_PARAM;
        rfStats->txErrorFormat++;
        break;

    case PHY_CHANNEL_BUSY:
        palRfStatus = PAL_RF_PHY_BUSY_TX;
        rfStats->txErrorBusyChannel++;
        break;

    case PHY_CHANNEL_IDLE:
        palRfStatus = PAL_RF_PHY_TRX_OFF;
        rfStats->txTotalErrors++;
        break;

    case PHY_NO_ACK:
        palRfStatus = PAL_RF_PHY_ERROR;
        rfStats->txTotalErrors++;
        break;

    case PHY_CHANNEL_ACCESS_FAILURE:
        palRfStatus = PAL_RF_PHY_CHANNEL_ACCESS_FAILURE;
        rfStats->txErrorBusyTX++;
        break;

    default:
        palRfStatus = PAL_RF_PHY_ERROR;
        rfStats->txTotalErrors++;
        break;
    }

    if (palRfData.rfPhyHandlers.palRfTxConfirm)
    {
        palRfData.rfPhyHandlers.palRfTxConfirm(palRfStatus,
                palRfData.txTimeIniCountCalc, timeEndCount);
    }
}

void PHY_RxFrameCallback(PHY_FrameInfo_t *rxFrame)
{
    PAL_RF_RX_PARAMETERS rxParameters;
    uint8_t *pData;
    PAL_RF_STATISTICS *rfStats;
    uint16_t phyDurationSymb;
    PHY_Retval_t attributeStatus;
    uint8_t phySHRSymbols;
    uint8_t phyNumSymbols;
    uint8_t payloadLen;
    uint8_t offset;

    /* Get Time Counter */
    palRfData.txTimeEndCount = SYS_TIME_Counter64Get();

    rfStats = &palRfData.stats;

    /* Get Payload Length */
    payloadLen = rxFrame->mpdu[0];
    if (payloadLen > LARGE_BUFFER_SIZE - 1)
    {
        /* Free-up the buffer which was used for reception once the frame is extracted. */
        bmm_buffer_free(rxFrame->buffer_header);

        rfStats->rxErrorLength++;
        return;
    }

    /* Get Payload */
    offset = LENGTH_FIELD_LEN;
    memcpy(palRfData.rxBuffer, &rxFrame->mpdu[offset], payloadLen);
    offset += payloadLen;
    pData = palRfData.rxBuffer;
    /* Get LQI */
    palRfData.lastRxPktLQI = rxFrame->mpdu[offset];
    offset += LQI_LEN;
    /* Get ED_LEVEL */
    palRfData.lastRxPktED = rxFrame->mpdu[offset];
    offset += ED_VAL_LEN;
    /* Get RSSI */
    rxParameters.rssi = rxFrame->mpdu[offset] + PHY_GetRSSIBaseVal();

    /* Get Frame Times */
    attributeStatus = PHY_PibGet(phySHRDuration, &phySHRSymbols);
    if (attributeStatus != PHY_SUCCESS)
    {
        phySHRSymbols = 0;
    }

    attributeStatus = PHY_PibGet(phySymbolsPerOctet, &phyNumSymbols);
    if (attributeStatus != PHY_SUCCESS)
    {
        phyNumSymbols = 0;
    }

    /* Update RX statistics */
    rfStats->rxTotalPackets++;
    rfStats->rxTotalBytes += payloadLen;
    rfStats->rxLastPaySymbols = payloadLen * phyNumSymbols;

    phyDurationSymb = phySHRSymbols + rfStats->rxLastPaySymbols;
    palRfData.txTimeIniCountCalc = palRfData.txTimeEndCount - SYS_TIME_USToCount(phyDurationSymb << 4);

    rxParameters.timeIniCount = palRfData.txTimeIniCountCalc;
    rxParameters.timeEndCount = palRfData.txTimeEndCount;

    /* Free-up the buffer which was used for reception once the frame is extracted. */
    bmm_buffer_free(rxFrame->buffer_header);

    if (palRfData.rfPhyHandlers.palRfDataIndication)
    {
        palRfData.rfPhyHandlers.palRfDataIndication(pData, payloadLen, &rxParameters);
    }
}

static void _PAL_RF_TxTimeCallback(uintptr_t context)
{
    palRfData.txPending = true;

    /* Signal thread to transmit a new packet */
    OSAL_SEM_PostISR(&palRFSemID);
}

// *****************************************************************************
// *****************************************************************************
// Section: local functions
// *****************************************************************************
// *****************************************************************************
static void _PAL_RF_ReportErrorTX(PAL_RF_PHY_STATUS status, uint64_t time)
{
    palRfData.stats.txTotalErrors++;

    palRfData.txCfmErrorPending = true;
    palRfData.txCfmErrorStatus = status;
    palRfData.txCfmErrorTime = time;

    /* Signal thread to transmit a new packet */
    OSAL_SEM_Post(&palRFSemID);
}

static PAL_RF_PIB_RESULT _PAL_RF_setRFNetworkParameters(void)
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

    PHY_ConfigRxPromiscuousMode(PROMISCUOUS_MODE);
    pibValue.pib_value_bool = (bool)PROMISCUOUS_MODE;
    attributeStatus = PHY_PibSet(macPromiscuousMode, &pibValue);
    if (attributeStatus != PHY_SUCCESS)
    {
        return PAL_RF_PIB_INVALID_PARAM;
    }

    attributeStatus = PHY_ConfigTxPwr(PWR_DBM_VALUE, (uint8_t)TRANSMIT_POWER_DBm);
    if (attributeStatus != PHY_SUCCESS)
    {
        return PAL_RF_PIB_INVALID_PARAM;
    }

    attributeStatus = PHY_ConfigAutoAck(AUTOACK_MODE);
    if (attributeStatus != PHY_SUCCESS)
    {
        return PAL_RF_PIB_INVALID_PARAM;
    }

    return PAL_RF_PIB_SUCCESS;
}

// *****************************************************************************
// *****************************************************************************
// Section: Interface Function Definitions
// *****************************************************************************
// *****************************************************************************

SYS_MODULE_OBJ PAL_RF_Initialize(const SYS_MODULE_INDEX index,
                                 const SYS_MODULE_INIT * const init)
{
    PAL_RF_INIT *palInit = (PAL_RF_INIT *)init;

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
    palRfData.txPending = false;
    palRfData.txCfmErrorPending = false;
    palRfData.txContinuousMode = false;
    palRfData.sleepMode = false;
    memset(&palRfData.stats, 0, sizeof (palRfData.stats));
    palRfData.txTimer = SYS_TIME_HANDLE_INVALID;

    /* Set RF Network Parameters */
    if (_PAL_RF_setRFNetworkParameters() != PAL_RF_PIB_SUCCESS)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    /* Enable Transceiver */
    if (PHY_RxEnable(PHY_STATE_RX_ON) != PHY_RX_ON)
    {
        return SYS_MODULE_OBJ_INVALID;
    }


    /* Create the PAL RF Semaphore */
    if (OSAL_SEM_Create(&palRFSemID, OSAL_SEM_TYPE_BINARY, 0, 0) == OSAL_RESULT_FALSE)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    /* Update Status */
    palRfData.status = SYS_STATUS_READY;

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
    PHY_TrxStatus_t status;

    if (object != (SYS_MODULE_OBJ)PAL_RF_PHY_INDEX)
    {
        return PAL_RF_STATUS_INVALID_OBJECT;
    }

    /* Check Transceiver */
    status = PHY_GetTrxStatus();
    switch (status)
    {
    case PHY_RX_ON:
    case PHY_TX_ON:
        break;

    case PHY_BUSY_RX:
    case PHY_BUSY_TX:
        return PAL_RF_STATUS_BUSY;
        break;

    case PHY_TRX_SLEEP:
    case PHY_TRX_DEEP_SLEEP:
        return PAL_RF_STATUS_ERROR;
        break;

    case PHY_TRX_OFF:
    default:
        PAL_RF_Deinitialize(PAL_RF_PHY_INDEX);
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
    PHY_RxEnable(PHY_STATE_TRX_OFF);

    /* Delete Semaphore */
    OSAL_SEM_Delete(&palRFSemID);

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
    OSAL_SEM_Pend(&palRFSemID, OSAL_WAIT_FOREVER);

    if (palRf->txPending)
    {
        palRf->txPending = false;
        PHY_Retval_t phyStatus;
        PAL_RF_PHY_STATUS palRfStatus;

        palRf->txTimer = SYS_TIME_HANDLE_INVALID;

        palRf->txTimeIniCount = SYS_TIME_Counter64Get();

        phyStatus = PHY_TxFrame(&palRf->txFrame, palRf->csmaMode, false);
        palRfStatus = palRfPhyStatus[phyStatus & 0x0F];

        if (palRfStatus != PAL_RF_PHY_SUCCESS)
        {
            palRf->txCfmErrorPending = true;
            palRf->txCfmErrorStatus = palRfStatus;
            palRf->txCfmErrorTime = palRf->txTimeIniCount;
        }
    }

    if (palRf->txCfmErrorPending)
    {
        palRf->txCfmErrorPending = false;
        // Report ERROR through TX CFM callback
        if (palRf->rfPhyHandlers.palRfTxConfirm)
        {
            palRf->rfPhyHandlers.palRfTxConfirm(palRf->txCfmErrorStatus,
                        palRf->txCfmErrorTime, palRf->txCfmErrorTime);
        }
    }
}

PAL_RF_TX_HANDLE PAL_RF_TxRequest(PAL_RF_HANDLE handle, uint8_t *pData,
                                  uint16_t length, PAL_RF_TX_PARAMETERS *txParameters)
{
    uint64_t timeCount;
    int64_t timeDiffCount;
    int8_t pwrDbm;

    timeCount = SYS_TIME_Counter64Get();

    if (handle != (PAL_RF_HANDLE) & palRfData)
    {
        // Report TX CFM error
        _PAL_RF_ReportErrorTX(PAL_RF_PHY_ERROR, timeCount);

        return (PAL_RF_HANDLE)&palRfData;
    }

    if ((palRfData.txTimer != SYS_TIME_HANDLE_INVALID) ||
            (palRfData.txContinuousMode) || (palRfData.sleepMode))
    {
        // Report TX CFM error
        palRfData.stats.txErrorBusyTX++;
        _PAL_RF_ReportErrorTX(PAL_RF_PHY_BUSY_TX, timeCount);

        return (PAL_RF_HANDLE)&palRfData;
    }

    if (length > LARGE_BUFFER_SIZE - 1)
    {
        // Report TX CFM error
        palRfData.stats.txErrorLength++;
        _PAL_RF_ReportErrorTX(PAL_RF_PHY_INVALID_PARAM, timeCount);

        return (PAL_RF_HANDLE)&palRfData;
    }

    palRfData.txFrame.mpdu = palRfData.txBuffer;
    palRfData.txBuffer[0] = (uint8_t)length;
    memcpy(&palRfData.txBuffer[1], pData, (uint8_t)length);

    if (txParameters->csmaEnable)
    {
        palRfData.csmaMode = CSMA_UNSLOTTED;
    }
    else
    {
        palRfData.csmaMode = NO_CSMA_NO_IFS;
    }

    /* Set Tx Power (Power value in dBm (-14dBm to 12dBm))*/
    pwrDbm = 12 - txParameters->txPowerAttenuation;
    if (pwrDbm < -14)
    {
        pwrDbm = -14;
    }
    PHY_ConfigTxPwr(PWR_DBM_VALUE, pwrDbm);

    timeDiffCount = txParameters->timeCount - timeCount;
    if (timeDiffCount < 0)
    {
        PHY_Retval_t phyStatus;
        PAL_RF_PHY_STATUS palRfStatus;

        // Immediate transmission
        palRfData.txTimeIniCount = SYS_TIME_Counter64Get();
        phyStatus = PHY_TxFrame(&palRfData.txFrame, palRfData.csmaMode, false);
        palRfStatus = palRfPhyStatus[phyStatus & 0x0F];
        if (palRfStatus != PAL_RF_PHY_SUCCESS)
        {
            // Report TX CFM error
            palRfData.stats.txErrorLength++;
            _PAL_RF_ReportErrorTX(palRfStatus, palRfData.txTimeIniCount);
        }
        else
        {
            palRfData.stats.txCfmNotHandled++;
        }
    }
    else
    {
        uint32_t timeDiffUs;

        timeDiffUs = SYS_TIME_CountToUS(timeDiffCount);
        palRfData.txTimer = SYS_TIME_CallbackRegisterUS(_PAL_RF_TxTimeCallback, (uintptr_t) & palRfData,
                                                        timeDiffUs, SYS_TIME_SINGLE);

        if (palRfData.txTimer == SYS_TIME_HANDLE_INVALID)
        {
            // Report TX CFM error
            palRfData.stats.txErrorTimeout++;
            _PAL_RF_ReportErrorTX(PAL_RF_PHY_TIMEOUT, timeCount);
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
        SYS_TIME_TimerDestroy(palRfData.txTimer);
    }

    if (PHY_GetTrxStatus() == PHY_BUSY_TX)
    {
        /* Reset Transceiver : no set PIBs to the default values */
        PHY_Reset(false);

        /* Enable Transceiver */
        PHY_RxEnable(PHY_STATE_RX_ON);

        // Report TX cancelled
        _PAL_RF_ReportErrorTX(PAL_RF_PHY_TX_CANCELLED, SYS_TIME_Counter64Get());
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
        SYS_TIME_TimerDestroy(palRfData.txTimer);
        palRfData.txTimer = SYS_TIME_HANDLE_INVALID;

        // Report TX cancelled
        _PAL_RF_ReportErrorTX(PAL_RF_PHY_TX_CANCELLED, SYS_TIME_Counter64Get());
    }

    /* Reset Transceiver : no set PIBs to the default values */
    PHY_Reset(false);

    /* Enable Transceiver */
    PHY_RxEnable(PHY_STATE_RX_ON);

    /* Update Pal Rf data flags */
    palRfData.txContinuousMode = false;
    palRfData.sleepMode = false;
}

PAL_RF_PIB_RESULT PAL_RF_GetRfPhyPib(PAL_RF_HANDLE handle, PAL_RF_PIB_OBJ *pibObj)
{
    PAL_RF_PIB_RESULT result = PAL_RF_PIB_SUCCESS;
    uint8_t *pData;

    if (handle != (PAL_RF_HANDLE) & palRfData)
    {
        return PAL_RF_PIB_INVALID_HANDLE;
    }

    if (palRfData.status != PAL_RF_STATUS_READY)
    {
        /* Ignore request */
        return PAL_RF_PIB_ERROR;
    }

    pData = pibObj->pData;

    switch (pibObj->pib)
    {
    case PAL_RF_PIB_DEVICE_ID:
        *((uint16_t *)pData) = 0x451;
        break;

    case PAL_RF_PIB_FW_VERSION:
        *((uint32_t *)pData) = palRfData.phySWVersion;
        break;

    case PAL_RF_PIB_TRX_SLEEP:
        *pData = (uint8_t)palRfData.sleepMode;
        break;

    case PAL_RF_PIB_PHY_CHANNEL_NUM:
        *((uint16_t *)pData) = (uint16_t)palRfData.network.channel;
        break;

    case PAL_RF_PIB_PHY_CHANNEL_FREQ_HZ:
        // fc = 2405 + 5 (k ? 11) in megahertz, for k = 11, 12, ?, 26
        // k is the channel number
        *((uint32_t *)pData) = (uint32_t)(2045 +
                5 * (palRfData.network.channel - 11)) *
                1000000;
        break;

    case PAL_RF_PIB_PHY_CHANNEL_PAGE:
        *pData = palRfData.network.channelPage;
        break;

    case PAL_RF_PIB_PHY_CHANNELS_SUPPORTED:
        PHY_PibGet(phyChannelsSupported, pData);
        break;

    case PAL_RF_PIB_PHY_CCA_ED_DURATION:
        // 8 symbols(128us)
        *((uint16_t *)pData) = (uint16_t)(8 << 4);
        break;

    case PAL_RF_PIB_PHY_CCA_ED_SAMPLE:
    {
        uint8_t edLevel = PHY_EdSample();

        // Convert the energy level to input power in Dbm
        int8_t pwrDbm = (int8_t)(edLevel + PHY_GetRSSIBaseVal());

        *((int8_t *)pData) = pwrDbm;
        break;
    }

    case PAL_RF_PIB_PHY_CCA_ED_THRESHOLD:
    {
        uint8_t pdtLevel;

        // Get RSSI base value of TRX
        int8_t trxBaseRSSI = PHY_GetRSSIBaseVal();

        // To get the PDT level configured
        PHY_GetTrxConfig(RX_SENS, &pdtLevel);

        // THRS = RSSIBASE_VAL + 3 x (pdtLevel - 1)
        *pData = (uint8_t)(trxBaseRSSI + 3 * (pdtLevel - 1));
        break;
    }

    case PAL_RF_PIB_PHY_TURNAROUND_TIME:
        *((uint16_t *)pData) = (uint16_t)aTurnaroundTime;
        break;

    case PAL_RF_PIB_PHY_TX_PAY_SYMBOLS:
        *((uint32_t *)pData) = palRfData.stats.txLastPaySymbols;
        break;

    case PAL_RF_PIB_PHY_RX_PAY_SYMBOLS:
        *((uint32_t *)pData) = palRfData.stats.rxLastPaySymbols;
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
        *pData = (uint8_t)palRfData.txContinuousMode;
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
    }

    return result;
}

PAL_RF_PIB_RESULT PAL_RF_SetRfPhyPib(PAL_RF_HANDLE handle, PAL_RF_PIB_OBJ *pibObj)
{
    PAL_RF_PIB_RESULT result = PAL_RF_PIB_SUCCESS;
    uint8_t *pData = pibObj->pData;

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
        uint8_t reset = *pData;

        if (reset)
        {
            PAL_RF_Reset((PAL_RF_HANDLE) & palRfData);
        }

        break;
    }

    case PAL_RF_PIB_TRX_SLEEP:
    {
        uint8_t sleep = *pData;

        if ((sleep) && (!palRfData.sleepMode))
        {
            palRfData.sleepMode = true;
            if (PHY_SUCCESS != PHY_TrxSleep(true))
            {
                result = PAL_RF_PIB_ERROR;
            }
        }
        else if ((!sleep) && (palRfData.sleepMode))
        {
            palRfData.sleepMode = false;
            if (PHY_SUCCESS != PHY_TrxSleep(false))
            {
                result = PAL_RF_PIB_ERROR;
            }
        }

        break;
    }

    case PAL_RF_PIB_PHY_CHANNEL_PAGE:
    {
        PibValue_t chnPage;

        chnPage.pib_value_8bit = pibObj->pData[0];
        if ((chnPage.pib_value_8bit == 0) || (chnPage.pib_value_8bit == 2) ||
                (chnPage.pib_value_8bit == 16) || (chnPage.pib_value_8bit == 17))
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
        if ((chnNum.pib_value_8bit >= 11) && (chnNum.pib_value_8bit <= 26))
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

        chnSupported.pib_value_32bit = *(uint32_t *)pibObj->pData;
        if (chnSupported.pib_value_32bit & 0xF80007FF)
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

    case PAL_RF_PIB_PHY_CCA_ED_THRESHOLD:
    {
        uint16_t edThreshold;
        uint8_t pdtLevel;

        // Get RSSI base value of TRX
        int8_t trxBaseRSSI = PHY_GetRSSIBaseVal();

        edThreshold = *(uint16_t *)pData;
        if (edThreshold >= trxBaseRSSI)
        {
            // THRS = RSSIBASE_VAL + 3 x (pdtLevel - 1)
            pdtLevel = ((edThreshold - trxBaseRSSI) / 3) + 1;

            if (pdtLevel > 15)
            {
                pdtLevel = 15;
            }

            // Set the PDT level configured
            if (PHY_ConfigRxSensitivity(pdtLevel) != PHY_SUCCESS)
            {
                result = PAL_RF_PIB_ERROR;
            }
            // THRS = RSSIBASE_VAL + 3 x (pdtLevel - 1)
            *pData = (uint8_t)(trxBaseRSSI + 3 * (pdtLevel - 1));
        }
        else
        {
            result = PAL_RF_PIB_INVALID_PARAM;
        }
        break;
    }

    case PAL_RF_PIB_PHY_CCA_ED_SAMPLE:
    case PAL_RF_PIB_PHY_CCA_ED_DURATION:
    case PAL_RF_PIB_DEVICE_ID:
    case PAL_RF_PIB_FW_VERSION:
    case PAL_RF_PIB_PHY_TURNAROUND_TIME:
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
        memset(&palRfData.stats, 0, sizeof (palRfData.stats));
        break;

    case PAL_RF_PIB_SET_CONTINUOUS_TX_MODE:
    {
        uint8_t enable = *pData;

        if ((enable) && (!palRfData.txContinuousMode))
        {
            palRfData.txContinuousMode = true;
            PHY_StartContinuousTransmit(PRBS_MODE, true);
        }
        else if ((!enable) && (palRfData.txContinuousMode))
        {
            palRfData.txContinuousMode = false;
            PHY_StopContinuousTransmit();
        }

        break;
    }

    case PAL_RF_PIB_PHY_CONFIG:
    case PAL_RF_PIB_PHY_BAND_OPERATING_MODE:
    case PAL_RF_PIB_PHY_CHANNEL_FREQ_HZ:
    case PAL_RF_PIB_TX_FSK_FEC:
    case PAL_RF_PIB_TX_OFDM_MCS:
    default:
        result = PAL_RF_PIB_INVALID_ATTR;
    }

    return result;
}

uint8_t PAL_RF_GetRfPhyPibLength(PAL_RF_HANDLE handle, PAL_RF_PIB_ATTRIBUTE attribute)
{
    uint8_t pibLen;

    if (handle != (PAL_RF_HANDLE) & palRfData)
    {
        return 0;
    }

    switch (attribute)
    {
    case PAL_RF_PIB_DEVICE_RESET:
    case PAL_RF_PIB_TRX_RESET:
    case PAL_RF_PIB_TRX_SLEEP:
    case PAL_RF_PIB_PHY_CCA_ED_THRESHOLD:
    case PAL_RF_PIB_PHY_CCA_ED_SAMPLE:
    case PAL_RF_PIB_PHY_STATS_RESET:
    case PAL_RF_PIB_SET_CONTINUOUS_TX_MODE:
    case PAL_RF_PIB_TX_FSK_FEC:
    case PAL_RF_PIB_TX_OFDM_MCS:
    case PAL_RF_PIB_PHY_CHANNEL_PAGE:
        pibLen = 1;
        break;

    case PAL_RF_PIB_DEVICE_ID:
    case PAL_RF_PIB_PHY_BAND_OPERATING_MODE:
    case PAL_RF_PIB_PHY_CHANNEL_NUM:
    case PAL_RF_PIB_PHY_CCA_ED_DURATION:
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
        pibLen = 19;
        break;

    default:
        pibLen = 0;
    }

    return pibLen;
}
