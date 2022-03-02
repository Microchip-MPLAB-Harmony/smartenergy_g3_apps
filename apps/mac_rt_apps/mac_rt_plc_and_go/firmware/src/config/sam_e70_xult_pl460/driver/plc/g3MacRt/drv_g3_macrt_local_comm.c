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
/*******************************************************************************
* Copyright (C) 2021 Microchip Technology Inc. and its subsidiaries.
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
static CACHE_ALIGN uint8_t gG3TxData[CACHE_ALIGNED_SIZE_GET(DRV_G3_MACRT_DATA_MAX_SIZE + 2)];
static CACHE_ALIGN uint8_t gG3RxData[CACHE_ALIGNED_SIZE_GET(DRV_G3_MACRT_DATA_MAX_SIZE)];
static CACHE_ALIGN uint8_t gG3RxParameters[CACHE_ALIGNED_SIZE_GET(DRV_G3_MACRT_RX_PAR_SIZE)];
static CACHE_ALIGN uint8_t gG3CommStatus[CACHE_ALIGNED_SIZE_GET(DRV_G3_MACRT_COMM_STATUS_SIZE)];
static CACHE_ALIGN uint8_t gG3TxConfirm[CACHE_ALIGNED_SIZE_GET(DRV_G3_MACRT_TX_CFM_SIZE)];
static CACHE_ALIGN uint8_t gG3RegResponse[CACHE_ALIGNED_SIZE_GET(DRV_G3_MACRT_REG_PKT_SIZE)];

// *****************************************************************************
// *****************************************************************************
// Section: File scope functions
// *****************************************************************************
// *****************************************************************************
static bool _DRV_G3_MACRT_COMM_CheckComm(DRV_PLC_HAL_INFO *info)
{
    if (info->key == DRV_PLC_HAL_KEY_CORTEX)
    {
        /* Communication correct */
        return true;
    }
    else if (info->key == DRV_PLC_HAL_KEY_BOOT)
    {
        /* Communication Error : Check reset value */
        if (info->flags & DRV_PLC_HAL_FLAG_RST_WDOG)   
        {
            /* Debugger is connected */
            DRV_PLC_BOOT_Restart(false);
            if (gG3MacRtObj->exceptionCallback)
            {
                gG3MacRtObj->exceptionCallback(DRV_G3_MACRT_EXCEPTION_DEBUG, 
                        gG3MacRtObj->contextExc);
            }
        }
        else
        {
            /* PLC needs boot process to upload firmware */
            DRV_PLC_BOOT_Restart(true);
            if (gG3MacRtObj->exceptionCallback)
            {
                gG3MacRtObj->exceptionCallback(DRV_G3_MACRT_EXCEPTION_RESET, 
                        gG3MacRtObj->contextExc);
            }
            
            /* Update Driver Status */
            gG3MacRtObj->status = SYS_STATUS_BUSY;
        }
    
        /* Check if there is any tx_cfm pending to be reported */
        if (gG3MacRtObj->state == DRV_G3_MACRT_STATE_WAITING_TX_CFM)
        {
            gG3MacRtObj->evResetTxCfm = true;
        }
        
        return true;
    }
    else
    {
        /* PLC needs boot process to upload firmware */
        DRV_PLC_BOOT_Restart(true);
        if (gG3MacRtObj->exceptionCallback)
        {
            gG3MacRtObj->exceptionCallback(
                    DRV_G3_MACRT_EXCEPTION_UNEXPECTED_KEY, 
                    gG3MacRtObj->contextExc
            );
        }
            
        /* Update Driver Status */
        gG3MacRtObj->status = SYS_STATUS_ERROR;
        
        return false;
    }
}

static void _DRV_G3_MACRT_COMM_SpiWriteCmd(DRV_G3_MACRT_MEM_ID id, uint8_t *pData, 
        uint16_t length)
{
    DRV_PLC_HAL_CMD halCmd;
    DRV_PLC_HAL_INFO halInfo;
    uint8_t failures = 0;
    
    /* Disable external interrupt from PLC */
    gG3MacRtObj->plcHal->enableExtInt(false);
    
    halCmd.cmd = DRV_PLC_HAL_CMD_WR;
    halCmd.memId = id;
    halCmd.length = length;
    halCmd.pData = pData;
    
    gG3MacRtObj->plcHal->sendWrRdCmd(&halCmd, &halInfo);
    
    /* Check communication integrity */
    while(!_DRV_G3_MACRT_COMM_CheckComm(&halInfo))
    {
        failures++;
        if (failures == 2) {
            if (gG3MacRtObj->exceptionCallback)
            {
                gG3MacRtObj->exceptionCallback(
                        DRV_G3_MACRT_EXCEPTION_CRITICAL_ERROR, 
                        gG3MacRtObj->contextExc
                );
            }
            break;
        }
        gG3MacRtObj->plcHal->reset();
        gG3MacRtObj->plcHal->sendWrRdCmd(&halCmd, &halInfo);
    }  
    
    /* Enable external interrupt from PLC */
    gG3MacRtObj->plcHal->enableExtInt(true); 
}

static void _DRV_G3_MACRT_COMM_SpiReadCmd(DRV_G3_MACRT_MEM_ID id, uint8_t *pData, 
        uint16_t length)
{
    DRV_PLC_HAL_CMD halCmd;
    DRV_PLC_HAL_INFO halInfo;
    uint8_t failures = 0;
    
    /* Disable external interrupt from PLC */
    gG3MacRtObj->plcHal->enableExtInt(false);
    
    halCmd.cmd = DRV_PLC_HAL_CMD_RD;
    halCmd.memId = id;
    halCmd.length = length;
    halCmd.pData = pData;
    
    gG3MacRtObj->plcHal->sendWrRdCmd(&halCmd, &halInfo);
    
    /* Check communication integrity */
    while(!_DRV_G3_MACRT_COMM_CheckComm(&halInfo))
    {
        failures++;
        if (failures == 2) {
            if (gG3MacRtObj->exceptionCallback)
            {
                gG3MacRtObj->exceptionCallback(
                        DRV_G3_MACRT_EXCEPTION_CRITICAL_ERROR, 
                        gG3MacRtObj->contextExc
                );
            }
            break;
        }
        gG3MacRtObj->plcHal->reset();
        gG3MacRtObj->plcHal->sendWrRdCmd(&halCmd, &halInfo);
    }    
    
    /* Enable external interrupt from PLC */
    gG3MacRtObj->plcHal->enableExtInt(true); 
}

static void _DRV_G3_MACRT_COMM_GetEventsInfo(DRV_G3_MACRT_EVENTS_OBJ *eventsObj)
{
    uint8_t *pData;
    DRV_PLC_HAL_CMD halCmd;
    DRV_PLC_HAL_INFO halInfo;
    uint8_t failures = 0;
    
    pData = gG3Info;    
    
    halCmd.cmd = DRV_PLC_HAL_CMD_RD;
    halCmd.memId = STATUS_ID;
    halCmd.length = DRV_G3_MACRT_STATUS_LENGTH;
    halCmd.pData = pData;
    
    gG3MacRtObj->plcHal->sendWrRdCmd(&halCmd, &halInfo);
    
    /* Check communication integrity */
    while(!_DRV_G3_MACRT_COMM_CheckComm(&halInfo))
    {
        failures++;
        if (failures == 2) {
            if (gG3MacRtObj->exceptionCallback)
            {
                gG3MacRtObj->exceptionCallback(
                        DRV_G3_MACRT_EXCEPTION_CRITICAL_ERROR, 
                        gG3MacRtObj->contextExc
                );
            }
            break;
        }
        gG3MacRtObj->plcHal->sendWrRdCmd(&halCmd, &halInfo);
    }    
    
    /* Extract Events information */
    eventsObj->evTxCfm = (halInfo.flags & DRV_G3_MACRT_EV_TX_CFM_MASK)? 1:0;
    eventsObj->evDataInd = (halInfo.flags & DRV_G3_MACRT_EV_DATA_IND_MASK)? 1:0;
    eventsObj->evMacSniffer = (halInfo.flags & DRV_G3_MACRT_EV_MAC_SNF_FLAG_MASK)? 1:0;
    eventsObj->evCommStatus = (halInfo.flags & DRV_G3_MACRT_EV_COMM_STATUS_FLAG_MASK)? 1:0;
    eventsObj->evRxParInd = (halInfo.flags & DRV_G3_MACRT_EV_RX_PAR_IND_FLAG_MASK)? 1:0;
    eventsObj->evRegRsp = (halInfo.flags & DRV_G3_MACRT_EV_REG_RSP_MASK)? 1:0;
    eventsObj->evPhySniffer = (halInfo.flags & DRV_G3_MACRT_EV_PHY_SNF_FLAG_MASK)? 1:0;
    
    /* Extract Timer info */
    eventsObj->timerRef = ((uint32_t)*pData++);
    eventsObj->timerRef += ((uint32_t)*pData++) << 8;
    eventsObj->timerRef += ((uint32_t)*pData++) << 16;
    eventsObj->timerRef += ((uint32_t)*pData++) << 24;

    /* Extract Lengths info */
    eventsObj->rcvDataLength = ((uint32_t)*pData++);
    eventsObj->rcvDataLength += ((uint32_t)*pData++) << 8;
    eventsObj->macSnifLength = ((uint32_t)*pData++);
    eventsObj->macSnifLength += ((uint32_t)*pData++) << 8;
    eventsObj->phySnifLength = ((uint32_t)*pData++);
    eventsObj->phySnifLength += ((uint32_t)*pData++) << 8;
    eventsObj->regRspLength = ((uint32_t)*pData++);
    eventsObj->regRspLength += ((uint32_t)*pData++) << 8;
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
    gG3MacRtObj->evDataIndLength = 0;
    gG3MacRtObj->evRegRspLength = 0;
    gG3MacRtObj->evMacSnifLength = 0;
    gG3MacRtObj->evPhySnifLength = 0;
    gG3MacRtObj->evCommStatus = false;
    gG3MacRtObj->evRxParams = false;
    gG3MacRtObj->evResetTxCfm = false;
    gG3MacRtObj->evTxCfm = false;
    
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
    
        if (gG3MacRtObj->evResetTxCfm)
        {
            /* Reset event flag */
            gG3MacRtObj->evResetTxCfm = false;
            
            /* Fill Tx Cfm object in case of reset while transmission */
            pTxCfmObj = &txCfmObj;
            txCfmObj.status = MAC_RT_STATUS_DENIED;
            txCfmObj.updateTimestamp = false;
        } else {
            pTxCfmObj = (MAC_RT_TX_CFM_OBJ *)gG3TxConfirm;
        }
        
        /* Report to upper layer */
        if (gG3MacRtObj->txCfmCallback)
        {
            gG3MacRtObj->txCfmCallback(pTxCfmObj);
        }
        
        /* Update MAC RT state */
        gG3MacRtObj->state = DRV_G3_MACRT_STATE_IDLE;
        /* Reset event flag */
        gG3MacRtObj->evTxCfm = false;
    }
    
    if (gG3MacRtObj->evDataIndLength)
    {   
        if (gG3MacRtObj->dataIndCallback)
        {
            /* Report to upper layer */
            gG3MacRtObj->dataIndCallback(gG3RxData, gG3MacRtObj->evDataIndLength);
        }
        
        /* Reset event flag */
        gG3MacRtObj->evDataIndLength = 0;
    }
    
    if (gG3MacRtObj->evRxParams)
    {   
        if (gG3MacRtObj->rxParamsIndCallback)
        {
            /* Report to upper layer */
            gG3MacRtObj->rxParamsIndCallback((MAC_RT_RX_PARAMETERS_OBJ *)gG3RxParameters);
        }
        
        /* Reset event flag */
        gG3MacRtObj->evRxParams = false;
    }
    
    if (gG3MacRtObj->evMacSnifLength)
    {   
        if ((gG3MacRtObj->macSnifferIndCallback) && (gG3MacRtObj->pMacDataSniffer))
        {
            /* Report to upper layer */
            gG3MacRtObj->macSnifferIndCallback(gG3MacRtObj->pMacDataSniffer, gG3MacRtObj->evMacSnifLength);
        }
        
        /* Reset event flag */
        gG3MacRtObj->evMacSnifLength = 0;
    }
    
    if (gG3MacRtObj->evCommStatus)
    {   
        if (gG3MacRtObj->commStatusIndCallback)
        {
            /* Report to upper layer */
            gG3MacRtObj->commStatusIndCallback(gG3CommStatus);
        }
        
        /* Reset event flag */
        gG3MacRtObj->evCommStatus = false;
    }
    
    if (gG3MacRtObj->evPhySnifLength)
    {   
        if ((gG3MacRtObj->phySnifferIndCallback) && (gG3MacRtObj->pPhyDataSniffer))
        {
            /* Report to upper layer */
            gG3MacRtObj->phySnifferIndCallback(gG3MacRtObj->pPhyDataSniffer, gG3MacRtObj->evPhySnifLength);
        }
        
        /* Reset event flag */
        gG3MacRtObj->evPhySnifLength = 0;
    }
}

void DRV_G3_MACRT_TxRequest(const DRV_HANDLE handle, uint8_t *pData, uint16_t length)
{    
    if((handle != DRV_HANDLE_INVALID) && (handle == 0) && 
       (gG3MacRtObj->state == DRV_G3_MACRT_STATE_IDLE))
    {
        /* Check Length */
		if ((length > 0) && (length <= (DRV_G3_MACRT_DATA_MAX_SIZE))) {
            uint8_t *pTxData;
            
			/* Update PLC state: transmitting */
            gG3MacRtObj->state = DRV_G3_MACRT_STATE_TX;

            /* Build and Send TX data */
            pTxData = gG3TxData;
            *pTxData++= (uint8_t)(length >> 8);
            *pTxData++= (uint8_t)length;
            memcpy(pTxData, pData, length);
            _DRV_G3_MACRT_COMM_SpiWriteCmd(TX_REQ_ID, gG3TxData, length + 2);
		}
    }
}

MAC_RT_STATUS DRV_G3_MACRT_PIBGet(const DRV_HANDLE handle, MAC_RT_PIB_OBJ *pibObj)
{    
    if ((handle != DRV_HANDLE_INVALID) && (handle == 0))
    {        
        uint8_t *pDst;
        uint16_t waitCounter;
        uint16_t dummyValue;
        
        /* Check Length */
		if (pibObj->length > MAC_RT_PIB_MAX_VALUE_LENGTH) {
			/* Length error */
			return MAC_RT_STATUS_INVALID_PARAMETER;
		}
        
        /* Build command */
        pDst = gG3RegResponse;

        *pDst++ = DRV_G3_MACRT_REG_CMD_RD;
        *pDst++ = (uint8_t)(pibObj->pib >> 24);
        *pDst++ = (uint8_t)(pibObj->pib >> 16);
        *pDst++ = (uint8_t)(pibObj->pib >> 8);
        *pDst++ = (uint8_t)(pibObj->pib);
        *pDst++ = (uint8_t)(pibObj->index >> 8);
        *pDst++ = (uint8_t)(pibObj->index);
        *pDst++ = (uint8_t)(pibObj->length);
        memcpy(pDst, pibObj->pData, pibObj->length);
        pDst += pibObj->length;

        /* Send PIB information request */
        _DRV_G3_MACRT_COMM_SpiWriteCmd(REG_INFO_ID, gG3RegResponse, pDst - gG3RegResponse);
        
        /* Sync function: Wait to response from interrupt */
		waitCounter = 100;
		dummyValue = gG3MacRtObj->evRegRspLength;
		while (!dummyValue) {
			/* Wait for interrupt. The CPU is in sleep mode until an interrupt occurs. */
			__asm("WFI");
			if (!waitCounter--) {
				/* Error in get cmd */
				return MAC_RT_STATUS_TRANSACTION_OVERFLOW;
			}

			dummyValue = gG3MacRtObj->evRegRspLength;
		}

		/* Check Response Content */
		if (*gG3RegResponse != MAC_RT_STATUS_SUCCESS) {
			/* Not success process */
			return (MAC_RT_STATUS)*gG3RegResponse;
		}

        /* Copy Reg data in PIB object */
		memcpy(pibObj->pData, gG3RegResponse + 8, gG3MacRtObj->evRegRspLength);
		pibObj->length = gG3MacRtObj->evRegRspLength;

		/* Reset event flag */
		gG3MacRtObj->evRegRspLength = 0;

        return MAC_RT_STATUS_SUCCESS;
    }
    
    return MAC_RT_STATUS_DENIED;
}

MAC_RT_STATUS DRV_G3_MACRT_PIBSet(const DRV_HANDLE handle, MAC_RT_PIB_OBJ *pibObj)
{    
    if ((handle != DRV_HANDLE_INVALID) && (handle == 0))
    {        
        uint8_t *pDst;
        uint16_t waitCounter;
        uint16_t dummyValue;
        
        /* Check Length */
		if (pibObj->length > MAC_RT_PIB_MAX_VALUE_LENGTH) {
			/* Length error */
			return MAC_RT_STATUS_INVALID_PARAMETER;
		}
        
        /* Build command */
        pDst = gG3RegResponse;

        *pDst++ = DRV_G3_MACRT_REG_CMD_WR;
        *pDst++ = (uint8_t)(pibObj->pib >> 24);
        *pDst++ = (uint8_t)(pibObj->pib >> 16);
        *pDst++ = (uint8_t)(pibObj->pib >> 8);
        *pDst++ = (uint8_t)(pibObj->pib);
        *pDst++ = (uint8_t)(pibObj->index >> 8);
        *pDst++ = (uint8_t)(pibObj->index);
        *pDst++ = (uint8_t)(pibObj->length);
        memcpy(pDst, pibObj->pData, pibObj->length);
        pDst += pibObj->length;

        /* Send PIB information request */
        _DRV_G3_MACRT_COMM_SpiWriteCmd(REG_INFO_ID, gG3RegResponse, pDst - gG3RegResponse);
        
        /* Sync function: Wait to response from interrupt */
		waitCounter = 100;
		dummyValue = gG3MacRtObj->evRegRspLength;
		while (!dummyValue) {
			/* Wait for interrupt. The CPU is in sleep mode until an interrupt occurs. */
			__asm("WFI");
			if (!waitCounter--) {
				/* Error in get cmd */
				return MAC_RT_STATUS_TRANSACTION_OVERFLOW;
			}

			dummyValue = gG3MacRtObj->evRegRspLength;
		}

        return MAC_RT_STATUS_SUCCESS;
    }
    
    return MAC_RT_STATUS_DENIED;
}

void DRV_G3_MACRT_SetCoordinator(const DRV_HANDLE handle)
{    
    if ((handle != DRV_HANDLE_INVALID) && (handle == 0))
    {        
        uint8_t coordinator;
        
        coordinator = 1;
        /* Send Coordinator Capabilities Request */
        _DRV_G3_MACRT_COMM_SpiWriteCmd(SET_COORD_ID, &coordinator, 1);
    }
}

uint32_t DRV_G3_MACRT_GetTimerReference(const DRV_HANDLE handle)
{    
    uint32_t timerReference = 0;
    
    if ((handle != DRV_HANDLE_INVALID) && (handle == 0))
    {        
        /* Read G3 Internal Timer Reference */
        _DRV_G3_MACRT_COMM_SpiReadCmd(STATUS_INFO_ID, (uint8_t *)&timerReference, 4);
    }
    
    return timerReference;
}

void DRV_G3_MACRT_ExternalInterruptHandler(PIO_PIN pin, uintptr_t context)
{   
    /* Avoid warning */
    (void)context;

    if ((gG3MacRtObj) && (pin == (PIO_PIN)gG3MacRtObj->plcHal->plcPlib->extIntPin))
    {
        DRV_G3_MACRT_EVENTS_OBJ evObj;
        
        /* Time guard */
        gG3MacRtObj->plcHal->delay(20);
        
        /* Get PLC events information */
        _DRV_G3_MACRT_COMM_GetEventsInfo(&evObj);
        
        /* Check confirmation of the transmission event */
        if (evObj.evTxCfm)
        {
            _DRV_G3_MACRT_COMM_SpiReadCmd(TX_CFM_ID, gG3TxConfirm, 
                    DRV_G3_MACRT_TX_CFM_SIZE);
            /* update event flag */
            gG3MacRtObj->evTxCfm = true;
        }
        
        /* Check received new data event */
        if (evObj.evDataInd)
        {       
            if ((evObj.rcvDataLength == 0) || 
                (evObj.rcvDataLength > DRV_G3_MACRT_DATA_MAX_SIZE))
            {
                evObj.rcvDataLength = 1;
            }
            _DRV_G3_MACRT_COMM_SpiReadCmd(DATA_IND_ID, gG3RxData, 
                    evObj.rcvDataLength);
            /* update event flag */
            gG3MacRtObj->evDataIndLength = evObj.rcvDataLength;
        }
        
        /* Check MAC Sniffer event */
        if (evObj.evMacSniffer)
        {          
            if ((evObj.macSnifLength == 0) || 
                (evObj.macSnifLength > DRV_G3_MACRT_MAC_SNIF_MAX_SIZE))
            {
                evObj.macSnifLength = 1;
            }
            _DRV_G3_MACRT_COMM_SpiReadCmd(MAC_SNIF_ID, XXXXXXXXXXXXXXXXXXXXXXXXXXXX, 
                    evObj.macSnifLength);
            /* update event flag */
            gG3MacRtObj->evMacSnifLength = evObj.macSnifLength;
        }
        
        /* Check Comm Status event */
        if (evObj.evCommStatus)
        {        
            _DRV_G3_MACRT_COMM_SpiReadCmd(COMM_STATUS_ID, gG3CommStatus, 
                    DRV_G3_MACRT_COMM_STATUS_SIZE);
            /* update event flag */
            gG3MacRtObj->evCommStatus = true;
        }
        
        /* Check PHY Sniffer event */
        if (evObj.evPhySniffer)
        {            
            if ((evObj.phySnifLength == 0) || 
                (evObj.phySnifLength > DRV_G3_MACRT_PHY_SNIF_MAX_SIZE))
            {
                evObj.phySnifLength = 1;
            }
            _DRV_G3_MACRT_COMM_SpiReadCmd(PHY_SNIF_ID, XXXXXXXXXXXXXXXXXXXXXXXXXXXX, 
                    evObj.phySnifLength);
            /* update event flag */
            gG3MacRtObj->evPhySnifLength = evObj.phySnifLength;
        }
        
        /* Check RX paramenters indication event */
        if (evObj.evRxParInd)
        {        
            _DRV_G3_MACRT_COMM_SpiReadCmd(RX_PAR_IND_ID, gG3RxParameters, 
                    DRV_G3_MACRT_RX_PAR_SIZE);
            /* update event flag */
            gG3MacRtObj->evRxParams = true;
        }
        
        /* Check Register info event */
        if (evObj.evReg)
        {          
            if ((evObj.regRspLength == 0) || 
                (evObj.regRspLength > DRV_G3_MACRT_REG_PKT_SIZE))
            {
                evObj.regRspLength = 1;
            }
            _DRV_G3_MACRT_COMM_SpiReadCmd(REG_RSP_ID, gG3RegResponse, 
                    evObj.regRspLength + 8);
            /* update event flag */
            gG3MacRtObj->evRegRspLength = evObj.regRspLength;
        }
        
        /* Time guard */
        gG3MacRtObj->plcHal->delay(50);
    }
    
    /* PORT Interrupt Status Clear */
    ((pio_registers_t*)DRV_PLC_EXT_INT_PIO_PORT)->PIO_ISR;
}
