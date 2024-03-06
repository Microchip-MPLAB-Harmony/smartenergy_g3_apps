/******************************************************************************
  DRV_G3_MACRT_LOCAL_COMM Profile Layer

  Company:
    Microchip Technology Inc.

  File Name:
    drv_g3_macrt_local_comm.c

  Summary:
    G3 MAC RT Driver Local Communication Profile Layer

  Description:
    This file contains the source code for the implementation of the G3 Profile
    Layer.
*******************************************************************************/

//DOM-IGNORE-BEGIN
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
//DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Include Files
// *****************************************************************************
// *****************************************************************************
#include <string.h>
#include <stdbool.h>
#include "configuration.h"
#include "system/system.h"
#include "driver/plc/common/drv_plc_hal.h"
#include "driver/plc/common/drv_plc_boot.h"
#include "driver/plc/g3MacRt/drv_g3_macrt.h"
#include "driver/plc/g3MacRt/drv_g3_macrt_local.h"
#include "driver/plc/g3MacRt/drv_g3_macrt_local_comm.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data
// *****************************************************************************
// *****************************************************************************

/* This is the driver instance object array. */
static DRV_G3_MACRT_OBJ *gG3MacRtObj = NULL;

/* Buffer definition to communicate with G3 MAC RT device */
static CACHE_ALIGN uint8_t gG3StatusInfo[CACHE_ALIGNED_SIZE_GET(DRV_G3_MACRT_STATUS_LENGTH)];
static CACHE_ALIGN uint8_t gG3TxData[CACHE_ALIGNED_SIZE_GET((DRV_G3_MACRT_DATA_MAX_SIZE + 2))];
static CACHE_ALIGN uint8_t gG3RxData[2][CACHE_ALIGNED_SIZE_GET(DRV_G3_MACRT_DATA_MAX_SIZE)];
static CACHE_ALIGN uint8_t gG3RxParameters[2][CACHE_ALIGNED_SIZE_GET(DRV_G3_MACRT_RX_PAR_SIZE)];
static CACHE_ALIGN uint8_t gG3CommStatus[CACHE_ALIGNED_SIZE_GET(DRV_G3_MACRT_COMM_STATUS_SIZE)];
static CACHE_ALIGN uint8_t gG3TxConfirm[CACHE_ALIGNED_SIZE_GET(DRV_G3_MACRT_TX_CFM_SIZE)];
static CACHE_ALIGN uint8_t gG3RegResponse[CACHE_ALIGNED_SIZE_GET(DRV_G3_MACRT_REG_PKT_SIZE)];

// *****************************************************************************
// *****************************************************************************
// Section: File scope functions
// *****************************************************************************
// *****************************************************************************
static bool lDRV_G3_MACRT_COMM_CheckComm(DRV_PLC_HAL_INFO *info)
{
    bool result = false;

    if (info->key == DRV_PLC_HAL_KEY_CORTEX)
    {
        /* Communication correct */
        result = true;
    }
    else if (info->key == DRV_PLC_HAL_KEY_BOOT)
    {
        /* PLC doesn't need boot process to upload firmware */
        DRV_PLC_BOOT_Restart(DRV_PLC_BOOT_RESTART_SOFT);
        if (gG3MacRtObj->exceptionCallback != NULL)
        {
            gG3MacRtObj->exceptionCallback(DRV_G3_MACRT_EXCEPTION_RESET);
        }
    }
    else
    {
        /* PLC needs boot process to upload firmware */
        DRV_PLC_BOOT_Restart(DRV_PLC_BOOT_RESTART_HARD);
        if (gG3MacRtObj->exceptionCallback != NULL)
        {
            gG3MacRtObj->exceptionCallback(DRV_G3_MACRT_EXCEPTION_UNEXPECTED_KEY);
        }
    }

    return result;
}

static void lDRV_G3_MACRT_COMM_SpiWriteCmd(DRV_G3_MACRT_MEM_ID id, uint8_t *pData,
        uint16_t length)
{
    DRV_PLC_HAL_CMD halCmd;
    DRV_PLC_HAL_INFO halInfo;

    /* Disable external interrupt from PLC */
    gG3MacRtObj->plcHal->enableExtInt(false);

    halCmd.cmd = DRV_PLC_HAL_CMD_WR;
    halCmd.memId = (uint16_t)id;
    halCmd.length = length;
    halCmd.pData = pData;

    gG3MacRtObj->plcHal->sendWrRdCmd(&halCmd, &halInfo);

    /* Check communication integrity */
    if (lDRV_G3_MACRT_COMM_CheckComm(&halInfo) == false)
    {
        /* Check if there is any tx_cfm pending to be reported */
        if (gG3MacRtObj->state == DRV_G3_MACRT_STATE_WAITING_TX_CFM)
        {
            gG3MacRtObj->evResetTxCfm = true;

            /* Post semaphore to resume task */
            (void) OSAL_SEM_PostISR(&gG3MacRtObj->semaphoreID);
        }

        gG3MacRtObj->state = DRV_G3_MACRT_STATE_ERROR_COMM;
    }
    else
    {
        /* Enable external interrupt from PLC */
        gG3MacRtObj->plcHal->enableExtInt(true);
    }
}

static void lDRV_G3_MACRT_COMM_SpiReadCmd(DRV_G3_MACRT_MEM_ID id, uint8_t *pData,
        uint16_t length)
{
    DRV_PLC_HAL_CMD halCmd;
    DRV_PLC_HAL_INFO halInfo;

    /* Disable external interrupt from PLC */
    gG3MacRtObj->plcHal->enableExtInt(false);

    halCmd.cmd = DRV_PLC_HAL_CMD_RD;
    halCmd.memId = (uint16_t)id;
    halCmd.length = length;
    halCmd.pData = pData;

    gG3MacRtObj->plcHal->sendWrRdCmd(&halCmd, &halInfo);

    /* Check communication integrity */
    if (lDRV_G3_MACRT_COMM_CheckComm(&halInfo) == false)
    {
        /* Check if there is any tx_cfm pending to be reported */
        if (gG3MacRtObj->state == DRV_G3_MACRT_STATE_WAITING_TX_CFM)
        {
            gG3MacRtObj->evResetTxCfm = true;

            /* Post semaphore to resume task */
            (void) OSAL_SEM_PostISR(&gG3MacRtObj->semaphoreID);
        }

        gG3MacRtObj->state = DRV_G3_MACRT_STATE_ERROR_COMM;
    }

    /* Enable external interrupt from PLC */
    gG3MacRtObj->plcHal->enableExtInt(true);
}

static void lDRV_G3_MACRT_COMM_GetEventsInfo(DRV_G3_MACRT_EVENTS_OBJ *eventsObj)
{
    uint8_t *pData;
    DRV_PLC_HAL_CMD halCmd;
    DRV_PLC_HAL_INFO halInfo;

    pData = gG3StatusInfo;

    halCmd.cmd = DRV_PLC_HAL_CMD_RD;
    halCmd.memId = (uint16_t)STATUS_INFO_ID;
    halCmd.length = DRV_G3_MACRT_STATUS_LENGTH;
    halCmd.pData = pData;

    gG3MacRtObj->plcHal->sendWrRdCmd(&halCmd, &halInfo);

    /* Check communication integrity */
    if (lDRV_G3_MACRT_COMM_CheckComm(&halInfo) == false)
    {
        /* Check if there is any tx_cfm pending to be reported */
        if (gG3MacRtObj->state == DRV_G3_MACRT_STATE_WAITING_TX_CFM)
        {
            gG3MacRtObj->evResetTxCfm = true;

            /* Post semaphore to resume task */
            (void) OSAL_SEM_PostISR(&gG3MacRtObj->semaphoreID);
        }

        gG3MacRtObj->state = DRV_G3_MACRT_STATE_ERROR_COMM;
    }

    /* Extract Events information */
    eventsObj->evTxCfm = ((halInfo.flags & DRV_G3_MACRT_EV_TX_CFM_FLAG_MASK) != 0U);
    eventsObj->evDataInd = ((halInfo.flags & DRV_G3_MACRT_EV_DATA_IND_FLAG_MASK) != 0U);
    eventsObj->evMacSniffer = ((halInfo.flags & DRV_G3_MACRT_EV_MAC_SNF_FLAG_MASK) != 0U);
    eventsObj->evCommStatus = ((halInfo.flags & DRV_G3_MACRT_EV_COMM_STATUS_FLAG_MASK) != 0U);
    eventsObj->evRxParInd = ((halInfo.flags & DRV_G3_MACRT_EV_RX_PAR_IND_FLAG_MASK) != 0U);
    eventsObj->evRegRsp = ((halInfo.flags & DRV_G3_MACRT_EV_REG_RSP_MASK) != 0U);
    eventsObj->evPhySniffer = ((halInfo.flags & DRV_G3_MACRT_EV_PHY_SNF_FLAG_MASK) != 0U);

    /* Extract Timer info */
    eventsObj->timerRef = *pData++;
    eventsObj->timerRef += ((uint32_t)*pData++) << 8;
    eventsObj->timerRef += ((uint32_t)*pData++) << 16;
    eventsObj->timerRef += ((uint32_t)*pData++) << 24;

    /* Extract Lengths info */
    eventsObj->rcvDataLength = *pData++;
    eventsObj->rcvDataLength += ((uint16_t)*pData++) << 8;
    eventsObj->macSnifLength = *pData++;
    eventsObj->macSnifLength += ((uint16_t)*pData++) << 8;
    eventsObj->phySnifLength = *pData++;
    eventsObj->phySnifLength += ((uint16_t)*pData++) << 8;
    eventsObj->regRspLength = *pData++;
    eventsObj->regRspLength += ((uint16_t)*pData++) << 8;
}

// *****************************************************************************
// *****************************************************************************
// Section: DRV_PLC Common Interface Implementation
// *****************************************************************************
// *****************************************************************************
void DRV_G3_MACRT_Init(DRV_G3_MACRT_OBJ *g3MacRt)
{
    gG3MacRtObj = g3MacRt;

    /* Clear PLC events information */
    gG3MacRtObj->evDataIndLength[0] = 0;
    gG3MacRtObj->evDataIndLength[1] = 0;
    gG3MacRtObj->evRegRspLength = 0;
    gG3MacRtObj->evMacSnifLength = 0;
    gG3MacRtObj->evPhySnifLength = 0;
    gG3MacRtObj->evCommStatus = false;
    gG3MacRtObj->evRxParams[0] = false;
    gG3MacRtObj->evRxParams[1] = false;
    gG3MacRtObj->evResetTxCfm = false;
    gG3MacRtObj->evTxCfm = false;
    gG3MacRtObj->evDataIndLengthIndex = 0;
    gG3MacRtObj->evRxParamsIndex = 0;

    /* Enable external interrupt from PLC */
    gG3MacRtObj->plcHal->enableExtInt(true);
}

void DRV_G3_MACRT_Task(void)
{
    /* Check event flags */
    if ((gG3MacRtObj->evTxCfm) || (gG3MacRtObj->evResetTxCfm))
    {
        MAC_RT_TX_CFM_OBJ *pTxCfmObj;
        MAC_RT_TX_CFM_OBJ txCfmObj;

        /* Reset event flag */
        gG3MacRtObj->evTxCfm = false;

        if (gG3MacRtObj->evResetTxCfm)
        {
            /* Reset event flag */
            gG3MacRtObj->evResetTxCfm = false;

            /* Fill Tx Cfm object in case of reset while transmission */
            txCfmObj.status = MAC_RT_STATUS_DENIED;
            txCfmObj.updateTimestamp = false;
            pTxCfmObj = &txCfmObj;
        }
        else
        {
            /* MISRA C-2012 deviation block start */
            /* MISRA C-2012 Rule 11.3 deviated once. Deviation record ID - H3_MISRAC_2012_R_11_3_DR_1 */
            pTxCfmObj = (MAC_RT_TX_CFM_OBJ *)gG3TxConfirm;
            /* MISRA C-2012 deviation block end */
        }

        /* Report to upper layer */
        if (gG3MacRtObj->txCfmCallback != NULL)
        {
            gG3MacRtObj->txCfmCallback(pTxCfmObj);
        }

        /* Update MAC RT state */
        gG3MacRtObj->state = DRV_G3_MACRT_STATE_READY;
    }

    if (gG3MacRtObj->evRxParams[0])
    {
        /* Reset event flag */
        gG3MacRtObj->evRxParams[0] = false;

        /* Report to upper layer */
        if (gG3MacRtObj->rxParamsIndCallback != NULL)
        {
            /* MISRA C-2012 deviation block start */
            /* MISRA C-2012 Rule 11.3 deviated once. Deviation record ID - H3_MISRAC_2012_R_11_3_DR_1 */
            gG3MacRtObj->rxParamsIndCallback((MAC_RT_RX_PARAMETERS_OBJ *)gG3RxParameters[0]);
            /* MISRA C-2012 deviation block end */
        }
    }
    else
    {
        if (gG3MacRtObj->evRxParams[1])
        {
            /* Reset event flag */
            gG3MacRtObj->evRxParams[1] = false;

            /* Report to upper layer */
            if (gG3MacRtObj->rxParamsIndCallback != NULL)
            {
                /* MISRA C-2012 deviation block start */
                /* MISRA C-2012 Rule 11.3 deviated once. Deviation record ID - H3_MISRAC_2012_R_11_3_DR_1 */
                gG3MacRtObj->rxParamsIndCallback((MAC_RT_RX_PARAMETERS_OBJ *)gG3RxParameters[1]);
                /* MISRA C-2012 deviation block end */
            }
        }
    }

    if (gG3MacRtObj->evDataIndLength[0] > 0U)
    {
        uint16_t evDataLength = gG3MacRtObj->evDataIndLength[0];

        /* Reset event flag */
        gG3MacRtObj->evDataIndLength[0] = 0U;

        if (gG3MacRtObj->dataIndCallback != NULL)
        {
            /* Report to upper layer */
            gG3MacRtObj->dataIndCallback(gG3RxData[0], evDataLength);
        }
    }
    else
    {
        if (gG3MacRtObj->evDataIndLength[1] > 0U)
        {
            uint16_t evDataLength = gG3MacRtObj->evDataIndLength[1];

            /* Reset event flag */
            gG3MacRtObj->evDataIndLength[1] = 0U;

            if (gG3MacRtObj->dataIndCallback != NULL)
            {
                /* Report to upper layer */
                gG3MacRtObj->dataIndCallback(gG3RxData[1], evDataLength);
            }
        }
    }

    if (gG3MacRtObj->evMacSnifLength > 0U)
    {
        uint16_t evDataLength = gG3MacRtObj->evMacSnifLength;

        /* Reset event flag */
        gG3MacRtObj->evMacSnifLength = 0U;

        if ((gG3MacRtObj->macSnifferIndCallback != NULL) && (gG3MacRtObj->pMacDataSniffer != NULL))
        {
            /* Report to upper layer */
            gG3MacRtObj->macSnifferIndCallback(gG3MacRtObj->pMacDataSniffer, evDataLength);
        }
    }

    if (gG3MacRtObj->evCommStatus)
    {
        /* Reset event flag */
        gG3MacRtObj->evCommStatus = false;

        if (gG3MacRtObj->commStatusIndCallback != NULL)
        {
            /* Report to upper layer */
            gG3MacRtObj->commStatusIndCallback(gG3CommStatus);
        }
    }

    if (gG3MacRtObj->evPhySnifLength > 0U)
    {
        uint16_t evDataLength = gG3MacRtObj->evPhySnifLength;

        /* Reset event flag */
        gG3MacRtObj->evPhySnifLength = 0U;

        if ((gG3MacRtObj->phySnifferIndCallback != NULL) && (gG3MacRtObj->pPhyDataSniffer != NULL))
        {
            /* Report to upper layer */
            gG3MacRtObj->phySnifferIndCallback(evDataLength);
        }
    }
}

void DRV_G3_MACRT_TxRequest(const DRV_HANDLE handle, uint8_t *pData, uint16_t length)
{
    if((handle != DRV_HANDLE_INVALID) && (handle == 0U) &&
       (gG3MacRtObj->state == DRV_G3_MACRT_STATE_READY))
    {
        /* Check Length */
        if ((length > 0U) && (length <= DRV_G3_MACRT_DATA_MAX_SIZE))
        {
            uint8_t *pTxData;

            /* Update PLC state: transmitting */
            gG3MacRtObj->state = DRV_G3_MACRT_STATE_WAITING_TX_CFM;

            /* Build and Send TX data */
            pTxData = gG3TxData;
            *pTxData++= (uint8_t)length;
            *pTxData++= (uint8_t)(length >> 8);
            (void) memcpy(pTxData, pData, length);
            lDRV_G3_MACRT_COMM_SpiWriteCmd(TX_REQ_ID, gG3TxData, length + 2U);
        }
        else
        {
            MAC_RT_TX_CFM_OBJ cfmObj;

            cfmObj.status = MAC_RT_STATUS_INVALID_PARAMETER;
            cfmObj.updateTimestamp = 0;
            if (gG3MacRtObj->txCfmCallback != NULL)
            {
                gG3MacRtObj->txCfmCallback(&cfmObj);
            }
        }
    }
    else
    {
        MAC_RT_TX_CFM_OBJ cfmObj;

        cfmObj.status = MAC_RT_STATUS_DENIED;
        cfmObj.updateTimestamp = 0;
        if (gG3MacRtObj->txCfmCallback != NULL)
        {
            gG3MacRtObj->txCfmCallback(&cfmObj);
        }
    }
}

MAC_RT_STATUS DRV_G3_MACRT_PIBGet(const DRV_HANDLE handle, MAC_RT_PIB_OBJ *pibObj)
{
    if ((handle != DRV_HANDLE_INVALID) && (handle == 0U))
    {
        uint8_t *pDst;
        uint16_t waitCounter;
        MAC_RT_STATUS result;

        /* Reset Event flag */
        gG3MacRtObj->evRegRspLength = 0U;

        /* Build command */
        pDst = gG3RegResponse;

        *pDst++ = DRV_G3_MACRT_REG_CMD_RD;
        *pDst++ = (uint8_t)((uint32_t)pibObj->pib >> 24);
        *pDst++ = (uint8_t)((uint32_t)pibObj->pib >> 16);
        *pDst++ = (uint8_t)((uint32_t)pibObj->pib >> 8);
        *pDst++ = (uint8_t)pibObj->pib;
        *pDst++ = (uint8_t)(pibObj->index >> 8);
        *pDst = (uint8_t)pibObj->index;

        /* Send PIB information request */
        lDRV_G3_MACRT_COMM_SpiWriteCmd(REG_RSP_ID, gG3RegResponse, 7U);

        /* Sync function: Wait to response from interrupt */
        waitCounter = 100U;
        while (gG3MacRtObj->evRegRspLength == 0U)
        {
            /* Wait for event (interrupt). The CPU is in sleep mode until an interrupt occurs. */
            __WFE();
            if ((waitCounter--) == 0U)
            {
                /* Error in get cmd */
                return MAC_RT_STATUS_TRANSACTION_OVERFLOW;
            }
        }

        /* Check Response Content */
        result = (MAC_RT_STATUS)*gG3RegResponse;
        if (result != MAC_RT_STATUS_SUCCESS)
        {
            /* Not success process */
            return result;
        }

        /* Copy Reg data in PIB object */
        (void) memcpy(pibObj->pData, gG3RegResponse + 8, gG3MacRtObj->evRegRspLength);
        pibObj->length = (uint8_t)gG3MacRtObj->evRegRspLength;

        /* Reset event flag */
        gG3MacRtObj->evRegRspLength = 0U;

        return MAC_RT_STATUS_SUCCESS;
    }

    return MAC_RT_STATUS_DENIED;
}

MAC_RT_STATUS DRV_G3_MACRT_PIBSet(const DRV_HANDLE handle, MAC_RT_PIB_OBJ *pibObj)
{
    if ((handle != DRV_HANDLE_INVALID) && (handle == 0U))
    {
        uint8_t *pDst;
        uint16_t waitCounter;
        MAC_RT_STATUS result;

        /* Check Length */
        if (pibObj->length > MAC_RT_PIB_MAX_VALUE_LENGTH) {
            /* Length error */
            return MAC_RT_STATUS_INVALID_PARAMETER;
        }

        /* Reset event flag */
        gG3MacRtObj->evRegRspLength = 0U;

        /* Build command */
        pDst = gG3RegResponse;

        *pDst++ = DRV_G3_MACRT_REG_CMD_WR;
        *pDst++ = (uint8_t)((uint32_t)pibObj->pib >> 24);
        *pDst++ = (uint8_t)((uint32_t)pibObj->pib >> 16);
        *pDst++ = (uint8_t)((uint32_t)pibObj->pib >> 8);
        *pDst++ = (uint8_t)pibObj->pib;
        *pDst++ = (uint8_t)(pibObj->index >> 8);
        *pDst++ = (uint8_t)pibObj->index;
        *pDst++ = pibObj->length;
        (void) memcpy(pDst, pibObj->pData, pibObj->length);

        /* Send PIB information request */
        lDRV_G3_MACRT_COMM_SpiWriteCmd(REG_RSP_ID, gG3RegResponse, (uint16_t)pibObj->length + 8U);

        /* Sync function: Wait to response from interrupt */
        waitCounter = 100U;
        while (gG3MacRtObj->evRegRspLength == 0U)
        {
            /* Wait for event (interrupt). The CPU is in sleep mode until an interrupt occurs. */
            __WFE();
            if ((waitCounter--) == 0U)
            {
                /* Error in get cmd */
                return MAC_RT_STATUS_TRANSACTION_OVERFLOW;
            }
        }

        /* Reset event flag */
        gG3MacRtObj->evRegRspLength = 0U;

        /* Check Response Content */
        result = (MAC_RT_STATUS)*gG3RegResponse;
        if (result != MAC_RT_STATUS_SUCCESS)
        {
            /* Not success process */
            return result;
        }

        return MAC_RT_STATUS_SUCCESS;
    }

    return MAC_RT_STATUS_DENIED;
}

void DRV_G3_MACRT_SetCoordinator(const DRV_HANDLE handle)
{
    if ((handle != DRV_HANDLE_INVALID) && (handle == 0U))
    {
        uint8_t coordinator;

        coordinator = 1U;
        /* Send Coordinator Capabilities Request */
        lDRV_G3_MACRT_COMM_SpiWriteCmd(SET_COORD_ID, &coordinator, 1U);
    }
}

void DRV_G3_MACRT_EnablePhySniffer(const DRV_HANDLE handle)
{
    if ((handle != DRV_HANDLE_INVALID) && (handle == 0U))
    {
        uint8_t sniffer;

        sniffer = 1U;
        /* Enable PHY Sniffer capabilities */
        lDRV_G3_MACRT_COMM_SpiWriteCmd(PHY_SNF_ID, &sniffer, 1U);
    }
}

uint32_t DRV_G3_MACRT_GetTimerReference(const DRV_HANDLE handle)
{
    uint32_t timerReference = 0U;

    if ((handle != DRV_HANDLE_INVALID) && (handle == 0U))
    {
        /* Read PLC Internal Timer Reference */
        lDRV_G3_MACRT_COMM_SpiReadCmd(STATUS_INFO_ID, (uint8_t *)&timerReference, 4U);
    }

    return timerReference;
}

void DRV_G3_MACRT_ExternalInterruptHandler(uintptr_t context)
{
    /* Avoid warning */
    (void)context;

    if (gG3MacRtObj != NULL)
    {
        DRV_G3_MACRT_EVENTS_OBJ evObj;

        /* Time guard */
        gG3MacRtObj->plcHal->delay(20);

        /* Get PLC events information */
        lDRV_G3_MACRT_COMM_GetEventsInfo(&evObj);

        /* Check confirmation of the transmission event */
        if (evObj.evTxCfm)
        {
            lDRV_G3_MACRT_COMM_SpiReadCmd(TX_CFM_ID, gG3TxConfirm,
                    (uint16_t)DRV_G3_MACRT_TX_CFM_SIZE);
            /* update event flag */
            gG3MacRtObj->evTxCfm = true;

            /* Post semaphore to resume task */
            (void) OSAL_SEM_PostISR(&gG3MacRtObj->semaphoreID);
        }

        /* Check RX paramenters indication event */
        if (evObj.evRxParInd)
        {
            lDRV_G3_MACRT_COMM_SpiReadCmd(RX_PAR_IND_ID, gG3RxParameters[gG3MacRtObj->evRxParamsIndex],
                    (uint16_t)DRV_G3_MACRT_RX_PAR_SIZE);
            /* update event flag */
            gG3MacRtObj->evRxParams[gG3MacRtObj->evRxParamsIndex] = true;

            if (gG3MacRtObj->evRxParamsIndex == 1U)
            {
                gG3MacRtObj->evRxParamsIndex = 0;
            }
            else
            {
                gG3MacRtObj->evRxParamsIndex = 1;
            }

            /* Post semaphore to resume task */
            (void) OSAL_SEM_PostISR(&gG3MacRtObj->semaphoreID);
        }

        /* Check received new data event */
        if (evObj.evDataInd)
        {
            if ((evObj.rcvDataLength == 0U) ||
                (evObj.rcvDataLength > DRV_G3_MACRT_DATA_MAX_SIZE))
            {
                evObj.rcvDataLength = 1U;
            }
            lDRV_G3_MACRT_COMM_SpiReadCmd(DATA_IND_ID, gG3RxData[gG3MacRtObj->evDataIndLengthIndex],
                    evObj.rcvDataLength);
            /* update event flag */
            gG3MacRtObj->evDataIndLength[gG3MacRtObj->evDataIndLengthIndex] = evObj.rcvDataLength;

            if (gG3MacRtObj->evDataIndLengthIndex == 1U)
            {
                gG3MacRtObj->evDataIndLengthIndex = 0;
            }
            else
            {
                gG3MacRtObj->evDataIndLengthIndex = 1;
            }

            /* Post semaphore to resume task */
            (void) OSAL_SEM_PostISR(&gG3MacRtObj->semaphoreID);
        }

        /* Check MAC Sniffer event */
        if (evObj.evMacSniffer)
        {
            uint8_t *pData;
            uint8_t dummyData;

            if ((evObj.macSnifLength == 0U) ||
                (evObj.macSnifLength > DRV_G3_MACRT_DATA_MAX_SIZE))
            {
                evObj.macSnifLength = 1U;
            }

            if (gG3MacRtObj->pMacDataSniffer != NULL)
            {
                pData = gG3MacRtObj->pMacDataSniffer;
            }
            else
            {
                pData = &dummyData;
                evObj.macSnifLength = 1U;
            }

            lDRV_G3_MACRT_COMM_SpiReadCmd(MAC_SNIF_ID, pData, evObj.macSnifLength);

            /* update event flag */
            if (evObj.macSnifLength > 1U)
            {
                gG3MacRtObj->evMacSnifLength = evObj.macSnifLength;

                /* Post semaphore to resume task */
                (void) OSAL_SEM_PostISR(&gG3MacRtObj->semaphoreID);
            }
        }

        /* Check Comm Status event */
        if (evObj.evCommStatus)
        {
            lDRV_G3_MACRT_COMM_SpiReadCmd(COMM_STATUS_ID, gG3CommStatus,
                    DRV_G3_MACRT_COMM_STATUS_SIZE);
            /* update event flag */
            gG3MacRtObj->evCommStatus = true;

            /* Post semaphore to resume task */
            (void) OSAL_SEM_PostISR(&gG3MacRtObj->semaphoreID);
        }

        /* Check PHY Sniffer event */
        if (evObj.evPhySniffer)
        {
            uint8_t *pData;
            uint8_t dummyData;

            if ((evObj.phySnifLength == 0U) ||
                (evObj.phySnifLength > (MAC_RT_PHY_DATA_MAX_SIZE + sizeof(MAC_RT_PHY_SNIFFER_HEADER))))
            {
                evObj.phySnifLength = 1U;
            }

            if (gG3MacRtObj->pPhyDataSniffer != NULL)
            {
                pData = gG3MacRtObj->pPhyDataSniffer;
            }
            else
            {
                pData = &dummyData;
                evObj.phySnifLength = 1U;
            }

            lDRV_G3_MACRT_COMM_SpiReadCmd(PHY_SNF_ID, pData, evObj.phySnifLength);

            /* update event flag */
            if (evObj.phySnifLength > 1U)
            {
                gG3MacRtObj->evPhySnifLength = evObj.phySnifLength;

                /* Post semaphore to resume task */
                (void) OSAL_SEM_PostISR(&gG3MacRtObj->semaphoreID);
            }
        }

        /* Check Register info event */
        if (evObj.evRegRsp)
        {
            if ((evObj.regRspLength == 0U) ||
                ((evObj.regRspLength + 8U) > (uint16_t)DRV_G3_MACRT_REG_PKT_SIZE))
            {
                evObj.regRspLength = 1U;
            }
            lDRV_G3_MACRT_COMM_SpiReadCmd(REG_RSP_ID, gG3RegResponse,
                    evObj.regRspLength + 8U);
            /* update event flag */
            gG3MacRtObj->evRegRspLength = evObj.regRspLength;
        }

        /* Time guard */
        gG3MacRtObj->plcHal->delay(20);
    }
}
