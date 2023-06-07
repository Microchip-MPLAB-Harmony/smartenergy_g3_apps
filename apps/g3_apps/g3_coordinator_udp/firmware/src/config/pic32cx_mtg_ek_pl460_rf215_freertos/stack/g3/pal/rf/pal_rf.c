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

//DOM-IGNORE-BEGIN
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
//DOM-IGNORE-END

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
#include "driver/rf215/drv_rf215.h"
#include "driver/rf215/drv_rf215_definitions.h"
#include "pal_rf.h"

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Variables
// *****************************************************************************
// *****************************************************************************
static PAL_RF_DATA palRfData = {0};

// *****************************************************************************
// *****************************************************************************
// Section: local callbacks
// *****************************************************************************
// *****************************************************************************

static void _palRfRxIndCallback(DRV_RF215_RX_INDICATION_OBJ* indObj, uintptr_t ctxt)
{
    uint8_t *pData;
    uint16_t len;
    PAL_RF_RX_PARAMETERS rxParameters;
    
    pData = indObj->psdu;
    len = indObj->psduLen;
    rxParameters.timeIniCount = indObj->timeIniCount;
    rxParameters.timeEndCount = indObj->timeIniCount + indObj->ppduDurationCount;
    rxParameters.rssi = indObj->rssiDBm;
    rxParameters.fcsOk = indObj->fcsOk;
        
    if (palRfData.rfPhyHandlers.palRfDataIndication)
    {
        palRfData.rfPhyHandlers.palRfDataIndication(pData, len, &rxParameters);
    }

}

static void _palRfTxCfmCallback (DRV_RF215_TX_HANDLE txHandle, DRV_RF215_TX_CONFIRM_OBJ *cfmObj,
    uintptr_t ctxt)
{
    PAL_RF_PHY_STATUS status = PAL_RF_PHY_ERROR;
    uint64_t timeIniCount;
    uint64_t timeEndCount;
    
    /* Get Frame times */
    timeIniCount = cfmObj->timeIniCount;
    timeEndCount = timeIniCount + cfmObj->ppduDurationCount;
    
    switch(cfmObj->txResult)
    {
        case RF215_TX_SUCCESS:
            status = PAL_RF_PHY_SUCCESS;
            break;

        case RF215_TX_BUSY_RX:
        case RF215_TX_BUSY_CHN:
        case RF215_TX_CANCEL_BY_RX:
            status = PAL_RF_PHY_CHANNEL_ACCESS_FAILURE;
            break;

        case RF215_TX_BUSY_TX:
        case RF215_TX_FULL_BUFFERS:
        case RF215_TX_TRX_SLEPT:
            status = PAL_RF_PHY_BUSY_TX;
            break;

        case RF215_TX_INVALID_LEN:
        case RF215_TX_INVALID_DRV_HANDLE:
        case RF215_TX_INVALID_PARAM:
            status = PAL_RF_PHY_INVALID_PARAM;
            break;

        case RF215_TX_ERROR_UNDERRUN:
        case RF215_TX_TIMEOUT:
            status = PAL_RF_PHY_TIMEOUT;
            break;

        case RF215_TX_CANCELLED:
            status = PAL_RF_PHY_TX_CANCELLED;
            break;

        case RF215_TX_ABORTED:
        default:
            status = PAL_RF_PHY_ERROR;
            break;      
    }
    
    if (palRfData.rfPhyHandlers.palRfTxConfirm)
    {
        palRfData.rfPhyHandlers.palRfTxConfirm(status, timeIniCount, timeEndCount);
    }

}

static void _palRfInitCallback(uintptr_t context, SYS_STATUS status)
{
    if (status == SYS_STATUS_ERROR)
    {
        palRfData.status = SYS_STATUS_ERROR;
        return;
    }

    palRfData.drvRfPhyHandle = DRV_RF215_Open(DRV_RF215_INDEX_0, RF215_TRX_ID_RF09);
    
    if (palRfData.drvRfPhyHandle == DRV_HANDLE_INVALID)
    {
        palRfData.status = SYS_STATUS_ERROR;
        return;
    }

    /* Register RF PHY driver callbacks */
    DRV_RF215_RxIndCallbackRegister(palRfData.drvRfPhyHandle, _palRfRxIndCallback, 0);
    DRV_RF215_TxCfmCallbackRegister(palRfData.drvRfPhyHandle, _palRfTxCfmCallback, 0);

    palRfData.status = SYS_STATUS_READY;
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

    palRfData.rfPhyModScheme = FSK_FEC_OFF;
    palRfData.status = SYS_STATUS_BUSY;
    palRfData.drvRfPhyHandle = DRV_HANDLE_INVALID;

    DRV_RF215_ReadyStatusCallbackRegister(DRV_RF215_INDEX_0, _palRfInitCallback, 0);

    return (SYS_MODULE_OBJ)PAL_RF_PHY_INDEX;
}

PAL_RF_HANDLE PAL_RF_HandleGet(const SYS_MODULE_INDEX index)
{    
    /* Check Single instance */
    if (index != PAL_RF_PHY_INDEX)
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
    
    palRfData.status = PAL_RF_STATUS_UNINITIALIZED;
    
    DRV_RF215_Close((DRV_HANDLE)palRfData.drvRfPhyHandle);
    palRfData.drvRfPhyHandle = DRV_HANDLE_INVALID;    
}
 
PAL_RF_TX_HANDLE PAL_RF_TxRequest(PAL_RF_HANDLE handle, uint8_t *pData, 
        uint16_t length, PAL_RF_TX_PARAMETERS *txParameters)
{
    DRV_RF215_TX_REQUEST_OBJ txReqObj;
    DRV_RF215_TX_RESULT txResult;
    DRV_RF215_TX_HANDLE rfPhyTxReqHandle;
            
    if (handle != (PAL_RF_HANDLE)&palRfData)
    {
        if (palRfData.rfPhyHandlers.palRfTxConfirm)
        {
            palRfData.rfPhyHandlers.palRfTxConfirm(PAL_RF_PHY_TRX_OFF, txParameters->timeCount, 
                    txParameters->timeCount);
        }
        return PAL_RF_TX_HANDLE_INVALID;
    }
    
    txReqObj.psdu = pData;
    txReqObj.psduLen = length;
    txReqObj.timeMode = TX_TIME_ABSOLUTE;
    txReqObj.timeCount = txParameters->timeCount;
    txReqObj.txPwrAtt = txParameters->txPowerAttenuation;
    txReqObj.modScheme = palRfData.rfPhyModScheme;
    
    if (txParameters->csmaEnable)
    {
        /* CSMA used: Energy above threshold and carrier sense CCA Mode */
        txReqObj.ccaMode = PHY_CCA_MODE_3;
        /* Programmed TX canceled once RX frame detected */
        txReqObj.cancelByRx = true;
    }
    else
    {
        /* CSMA not used */
        txReqObj.ccaMode = PHY_CCA_OFF;
        txReqObj.cancelByRx = false;
    }
    
    rfPhyTxReqHandle = DRV_RF215_TxRequest(palRfData.drvRfPhyHandle, &txReqObj, &txResult);

    if (rfPhyTxReqHandle == DRV_RF215_TX_HANDLE_INVALID)
    {
        DRV_RF215_TX_CONFIRM_OBJ cfmObj;
        
        cfmObj.txResult = txResult;
        cfmObj.timeIniCount = SYS_TIME_Counter64Get();
        cfmObj.ppduDurationCount = 0;
        _palRfTxCfmCallback(DRV_RF215_TX_HANDLE_INVALID, &cfmObj, 0);
    }
    
    return (PAL_RF_TX_HANDLE)rfPhyTxReqHandle;
}

void PAL_RF_TxCancel(PAL_RF_HANDLE handle, PAL_RF_TX_HANDLE txHandle)
{
    if (handle != (PAL_RF_HANDLE)&palRfData)
    {
        return;
    }
    
    DRV_RF215_TxCancel(palRfData.drvRfPhyHandle, (DRV_RF215_TX_HANDLE)txHandle);
}
 
void PAL_RF_Reset(PAL_RF_HANDLE handle)
{
    uint8_t resetValue = 1;
    
    if (handle != (PAL_RF_HANDLE)&palRfData)
    {
        return;
    }
    
    DRV_RF215_SetPib(palRfData.drvRfPhyHandle, RF215_PIB_DEVICE_RESET, &resetValue);
    DRV_RF215_SetPib(palRfData.drvRfPhyHandle, RF215_PIB_PHY_STATS_RESET, &resetValue);
}
 
PAL_RF_PIB_RESULT PAL_RF_GetRfPhyPib(PAL_RF_HANDLE handle, PAL_RF_PIB_OBJ *pibObj)
{
    if (handle != (PAL_RF_HANDLE)&palRfData)
    {
        return PAL_RF_PIB_INVALID_HANDLE;
    }
    
    if (palRfData.status != PAL_RF_STATUS_READY)
    {
        /* Ignore request */
        return PAL_RF_PIB_ERROR;
    }
    
    return (PAL_RF_PIB_RESULT)DRV_RF215_GetPib(palRfData.drvRfPhyHandle, pibObj->pib, 
            pibObj->pData);
}

PAL_RF_PIB_RESULT PAL_RF_SetRfPhyPib(PAL_RF_HANDLE handle, PAL_RF_PIB_OBJ *pibObj)
{
    if (handle != (PAL_RF_HANDLE)&palRfData)
    {
        return PAL_RF_PIB_INVALID_HANDLE;
    }
    
    if (palRfData.status != PAL_RF_STATUS_READY)
    {
        /* Ignore request */
        return PAL_RF_PIB_ERROR;
    }
    
    return (PAL_RF_PIB_RESULT)DRV_RF215_SetPib(palRfData.drvRfPhyHandle, pibObj->pib, 
            pibObj->pData);
}

uint8_t PAL_RF_GetRfPhyPibLength(PAL_RF_HANDLE handle, DRV_RF215_PIB_ATTRIBUTE attribute)
{
    if (handle != (PAL_RF_HANDLE)&palRfData)
    {
        return PAL_RF_PIB_INVALID_HANDLE;
    }
    
    return DRV_RF215_GetPibSize(attribute);
}