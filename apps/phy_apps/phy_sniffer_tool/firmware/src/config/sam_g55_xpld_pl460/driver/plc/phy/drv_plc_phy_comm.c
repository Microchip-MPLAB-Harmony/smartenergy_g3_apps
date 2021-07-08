/******************************************************************************
  DRV_PLC_PHY G3 Profile Layer

  Company:
    Microchip Technology Inc.

  File Name:
    drv_plc_phy_comm.c

  Summary:
    PLC Driver G3 Profile Layer

  Description:
    This file contains the source code for the implementation of the G3 Profile 
    Layer.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
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
#include "configuration.h"
#include "system/system.h"
#include "driver/plc/phy/drv_plc_phy.h"
#include "driver/plc/common/drv_plc_hal.h"
#include "driver/plc/common/drv_plc_boot.h"
#include "driver/plc/phy/drv_plc_phy_local_comm.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data
// *****************************************************************************
// *****************************************************************************

/* This is the driver instance object array. */
static DRV_PLC_PHY_OBJ *gPlcPhyObj;

/* Buffer definition to communicate with PLC */
static uint8_t sDataInfo[PLC_STATUS_LENGTH];
static uint8_t sDataTxPar[PLC_TX_PAR_SIZE];
static uint8_t sDataRxPar[PLC_RX_PAR_SIZE];
static uint8_t sDataRxDat[PLC_DATA_PKT_SIZE];
static uint8_t sDataTxCfm[PLC_CMF_PKT_SIZE];
static uint8_t sDataReg[PLC_REG_PKT_SIZE];

// *****************************************************************************
// *****************************************************************************
// Section: File scope functions
// *****************************************************************************
// *****************************************************************************

static uint32_t _DRV_PLC_PHY_COMM_GetPibBaseAddress(DRV_PLC_PHY_ID id)
{
    uint32_t addr;

    addr = 0;

    if (id & DRV_PLC_PHY_REG_ADC_MASK) {
        addr = (uint32_t)DRV_PLC_PHY_REG_ADC_BASE;
    } else if (id & DRV_PLC_PHY_REG_DAC_MASK) {
        addr = (uint32_t)DRV_PLC_PHY_REG_DAC_BASE;
    } else if (id & DRV_PLC_PHY_FUSES_MASK) {
        addr = (uint32_t)DRV_PLC_PHY_FUSES_BASE;
    } else if ((id & DRV_PLC_PHY_REG_MASK) && (id < PLC_ID_END_ID)) {
        addr = (uint32_t)DRV_PLC_PHY_REG_BASE;
    }

    return addr;
}

uint16_t _DRV_PLC_PHY_COMM_GetDelayUs(DRV_PLC_PHY_ID id)
{
    uint16_t delay = 50;

    if ((id & DRV_PLC_PHY_REG_MASK) && (id < PLC_ID_END_ID)) 
    {
        switch (id) 
        {
            case PLC_ID_TONE_MASK:
            delay = 600;
            break;

            case PLC_ID_PREDIST_COEF_TABLE_HI:
            case PLC_ID_PREDIST_COEF_TABLE_LO:
            delay = 250;
            break;

            case PLC_ID_PREDIST_COEF_TABLE_VLO:
            delay = 350;
            break;

            default:
            delay = 50;
            break;
        }
    }

    return delay;
}

static size_t _DRV_PLC_PHY_COMM_TxStringify(DRV_PLC_PHY_TRANSMISSION_OBJ *pSrc)
{
    uint8_t *pDst;
    size_t size;
    
    pDst = sDataTxPar;
    
    *pDst++ = (uint8_t)pSrc->time;
    *pDst++ = (uint8_t)(pSrc->time >> 8);
    *pDst++ = (uint8_t)(pSrc->time >> 16);
    *pDst++ = (uint8_t)(pSrc->time >> 24);
    *pDst++ = (uint8_t)pSrc->dataLength;
    *pDst++ = (uint8_t)(pSrc->dataLength >> 8);
    memcpy(pDst, pSrc->preemphasis, sizeof(pSrc->preemphasis));
    pDst += sizeof(pSrc->preemphasis);
    memcpy(pDst, pSrc->toneMap, sizeof(pSrc->toneMap));
    pDst += sizeof(pSrc->toneMap);
    *pDst++ = pSrc->mode;
    *pDst++ = pSrc->attenuation;
    *pDst++ = pSrc->modType;
    *pDst++ = pSrc->modScheme;
    *pDst++ = pSrc->pdc;
    *pDst++ = pSrc->rs2Blocks;
    *pDst++ = pSrc->delimiterType;

    size = (uint16_t)(pDst - sDataTxPar);
    
    return size;
    
}

static void _DRV_PLC_PHY_COMM_TxCfmEvent(DRV_PLC_PHY_TRANSMISSION_CFM_OBJ *pCfmObj)
{
    uint8_t *pSrc;
    
    pSrc = sDataTxCfm;
    
    pCfmObj->rmsCalc = (uint32_t)*pSrc++;
    pCfmObj->rmsCalc += (uint32_t)*pSrc++ << 8;
    pCfmObj->rmsCalc += (uint32_t)*pSrc++ << 16;
    pCfmObj->rmsCalc += (uint32_t)*pSrc++ << 24;

    pCfmObj->time = (uint32_t)*pSrc++;
    pCfmObj->time += (uint32_t)*pSrc++ << 8;
    pCfmObj->time += (uint32_t)*pSrc++ << 16;
    pCfmObj->time += (uint32_t)*pSrc++ << 24;

    pCfmObj->result = (DRV_PLC_PHY_TX_RESULT)*pSrc;
}

static void _DRV_PLC_PHY_COMM_RxEvent(DRV_PLC_PHY_RECEPTION_OBJ *pRxObj)
{
    uint8_t *pSrc;
    
    pSrc = sDataRxPar;
    
    /* Parse parameters of reception event */
    pRxObj->time = (uint32_t)*pSrc++;
    pRxObj->time += (uint32_t)*pSrc++ << 8;
    pRxObj->time += (uint32_t)*pSrc++ << 16;
    pRxObj->time += (uint32_t)*pSrc++ << 24;

    pRxObj->frameDuration = (uint32_t)*pSrc++;
    pRxObj->frameDuration += (uint32_t)*pSrc++ << 8;
    pRxObj->frameDuration += (uint32_t)*pSrc++ << 16;
    pRxObj->frameDuration += (uint32_t)*pSrc++ << 24;

    pRxObj->rssi = (uint16_t)*pSrc++;
    pRxObj->rssi += (uint16_t)*pSrc++ << 8;
    
    pRxObj->dataLength = (uint16_t)*pSrc++;
    pRxObj->dataLength += (uint16_t)*pSrc++ << 8;
    if (pRxObj->dataLength > PLC_DATA_PKT_SIZE) {
        pRxObj->dataLength = PLC_DATA_PKT_SIZE;
    }

    pRxObj->zctDiff = *pSrc++;
    pRxObj->rsCorrectedErrors = *pSrc++;
    pRxObj->modType = (DRV_PLC_PHY_MOD_TYPE)*pSrc++;
    pRxObj->modScheme = (DRV_PLC_PHY_MOD_SCHEME)*pSrc++;

    pRxObj->agcFactor = (uint32_t)*pSrc++;
    pRxObj->agcFactor += (uint32_t)*pSrc++ << 8;
    pRxObj->agcFactor += (uint32_t)*pSrc++ << 16;
    pRxObj->agcFactor += (uint32_t)*pSrc++ << 24;
    pRxObj->agcFine = (uint16_t)*pSrc++;
    pRxObj->agcFine += (uint16_t)*pSrc++ << 8;
    pRxObj->agcOffsetMeas = (uint16_t)*pSrc++;
    pRxObj->agcOffsetMeas += (uint16_t)*pSrc++ << 8;
    pRxObj->agcActive = *pSrc++;
    pRxObj->agcPgaValue = *pSrc++;
    pRxObj->snrFch = (uint16_t)*pSrc++;
    pRxObj->snrFch += (uint16_t)*pSrc++ << 8;
    pRxObj->snrPay = (uint16_t)*pSrc++;
    pRxObj->snrPay += (uint16_t)*pSrc++ << 8;
    pRxObj->payloadCorruptedCarriers = (uint16_t)*pSrc++;
    pRxObj->payloadCorruptedCarriers += (uint16_t)*pSrc++ << 8;
    pRxObj->payloadNoisedSymbols = (uint16_t)*pSrc++;
    pRxObj->payloadNoisedSymbols += (uint16_t)*pSrc++ << 8;
    pRxObj->payloadSnrWorstCarrier = *pSrc++;
    pRxObj->payloadSnrWorstCarrier = *pSrc++;
    pRxObj->payloadSnrImpulsive = *pSrc++;
    pRxObj->payloadSnrBand = *pSrc++;
    pRxObj->payloadSnrBackground = *pSrc++;
    pRxObj->lqi = *pSrc++;

    pRxObj->delimiterType = (DRV_PLC_PHY_DEL_TYPE)*pSrc++;
    pRxObj->crcOk = *pSrc++;
    memcpy(pRxObj->toneMap, pSrc, TONE_MAP_SIZE_MAX);
    pSrc += sizeof(pRxObj->toneMap);
    memcpy(pRxObj->carrierSnr, pSrc, PROTOCOL_CARRIERS_MAX);

    /* Set data content pointer */
    pRxObj->pReceivedData = sDataRxDat;
}

static bool _DRV_PLC_PHY_COMM_CheckComm(DRV_PLC_HAL_INFO *info)
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
            if (gPlcPhyObj->exceptionCallback)
            {
                gPlcPhyObj->exceptionCallback(DRV_PLC_PHY_EXCEPTION_DEBUG, gPlcPhyObj->contextExc);
            }
        }
        else
        {
            /* PLC needs boot process to upload firmware */
            DRV_PLC_BOOT_Restart(true);
            if (gPlcPhyObj->exceptionCallback)
            {
                gPlcPhyObj->exceptionCallback(DRV_PLC_PHY_EXCEPTION_RESET, gPlcPhyObj->contextExc);
            }
            
            /* Update Driver Status */
            gPlcPhyObj->status = SYS_STATUS_BUSY;
        }

        /* Check if there is any tx_cfm pending to be reported */
        if (gPlcPhyObj->state == DRV_PLC_PHY_STATE_WAITING_TX_CFM)
        {
            gPlcPhyObj->evResetTxCfm = true;
        }

        return true;
    }
    else
    {
        /* PLC needs boot process to upload firmware */
        DRV_PLC_BOOT_Restart(true);
        if (gPlcPhyObj->exceptionCallback)
        {
            gPlcPhyObj->exceptionCallback(DRV_PLC_PHY_EXCEPTION_UNEXPECTED_KEY, gPlcPhyObj->contextExc);
        }
            
        /* Update Driver Status */
        gPlcPhyObj->status = SYS_STATUS_ERROR;

        return false;
    }
}

static void _DRV_PLC_PHY_COMM_SpiWriteCmd(DRV_PLC_PHY_MEM_ID id, uint8_t *pData, uint16_t length)
{
    DRV_PLC_HAL_CMD halCmd;
    DRV_PLC_HAL_INFO halInfo;
    uint8_t failures = 0;
    
    /* Disable external interrupt from PLC */
    gPlcPhyObj->plcHal->enableExtInt(false);
    
    halCmd.cmd = DRV_PLC_HAL_CMD_WR;
    halCmd.memId = id;
    halCmd.length = length;
    halCmd.pData = pData;
    
    gPlcPhyObj->plcHal->sendWrRdCmd(&halCmd, &halInfo);
    
    /* Check communication integrity */
    while(!_DRV_PLC_PHY_COMM_CheckComm(&halInfo))
    {
        failures++;
        if (failures == 2) {
            if (gPlcPhyObj->exceptionCallback)
            {
                gPlcPhyObj->exceptionCallback(DRV_PLC_PHY_EXCEPTION_CRITICAL_ERROR, gPlcPhyObj->contextExc);
            }
            break;
        }
        gPlcPhyObj->plcHal->reset();
        gPlcPhyObj->plcHal->sendWrRdCmd(&halCmd, &halInfo);
    }  
    
    /* Enable external interrupt from PLC */
    gPlcPhyObj->plcHal->enableExtInt(true); 
}

static void _DRV_PLC_PHY_COMM_SpiReadCmd(DRV_PLC_PHY_MEM_ID id, uint8_t *pData, uint16_t length)
{
    DRV_PLC_HAL_CMD halCmd;
    DRV_PLC_HAL_INFO halInfo;
    uint8_t failures = 0;
    
    /* Disable external interrupt from PLC */
    gPlcPhyObj->plcHal->enableExtInt(false);
    
    halCmd.cmd = DRV_PLC_HAL_CMD_RD;
    halCmd.memId = id;
    halCmd.length = length;
    halCmd.pData = pData;
    
    gPlcPhyObj->plcHal->sendWrRdCmd(&halCmd, &halInfo);
    
    /* Check communication integrity */
    while(!_DRV_PLC_PHY_COMM_CheckComm(&halInfo))
    {
        failures++;
        if (failures == 2) {
            if (gPlcPhyObj->exceptionCallback)
            {
                gPlcPhyObj->exceptionCallback(DRV_PLC_PHY_EXCEPTION_CRITICAL_ERROR, gPlcPhyObj->contextExc);
            }
            break;
        }
        gPlcPhyObj->plcHal->reset();
        gPlcPhyObj->plcHal->sendWrRdCmd(&halCmd, &halInfo);
    }    
    
    /* Enable external interrupt from PLC */
    gPlcPhyObj->plcHal->enableExtInt(true); 
}

static void _DRV_PLC_PHY_COMM_GetEventsInfo(DRV_PLC_PHY_EVENTS_OBJ *eventsObj)
{
    uint8_t *pData;
    DRV_PLC_HAL_CMD halCmd;
    DRV_PLC_HAL_INFO halInfo;
    uint8_t failures = 0;
    
    pData = sDataInfo;    
    
    halCmd.cmd = DRV_PLC_HAL_CMD_RD;
    halCmd.memId = STATUS_ID;
    halCmd.length = PLC_STATUS_LENGTH;
    halCmd.pData = pData;
    
    gPlcPhyObj->plcHal->sendWrRdCmd(&halCmd, &halInfo);
    
    /* Check communication integrity */
    while(!_DRV_PLC_PHY_COMM_CheckComm(&halInfo))
    {
        failures++;
        if (failures == 2) {
            if (gPlcPhyObj->exceptionCallback)
            {
                gPlcPhyObj->exceptionCallback(DRV_PLC_PHY_EXCEPTION_CRITICAL_ERROR, gPlcPhyObj->contextExc);
            }
            break;
        }
        gPlcPhyObj->plcHal->sendWrRdCmd(&halCmd, &halInfo);
    }    
    
    /* Extract Events information */
    eventsObj->evCfm = (halInfo.flags & DRV_PLC_PHY_EV_FLAG_TX_CFM_MASK)? 1:0;
    eventsObj->evRxDat = (halInfo.flags & DRV_PLC_PHY_EV_FLAG_RX_DAT_MASK)? 1:0;
    eventsObj->evRxPar = (halInfo.flags & DRV_PLC_PHY_EV_FLAG_RX_PAR_MASK)? 1:0;
    eventsObj->evReg = (halInfo.flags & DRV_PLC_PHY_EV_FLAG_REG_MASK)? 1:0;
    
    /* Extract Timer info */
    eventsObj->timerRef = ((uint32_t)*pData++);
    eventsObj->timerRef += ((uint32_t)*pData++) << 8;
    eventsObj->timerRef += ((uint32_t)*pData++) << 16;
    eventsObj->timerRef += ((uint32_t)*pData++) << 24;

    /* Extract Lengths info */
    eventsObj->rcvDataLength = ((uint32_t)*pData++);
    eventsObj->rcvDataLength += ((uint32_t)*pData++) << 8;
    eventsObj->regRspLength = ((uint32_t)*pData++);
    eventsObj->regRspLength += ((uint32_t)*pData++) << 8;
}

// *****************************************************************************
// *****************************************************************************
// Section: DRV_PLC_PHY Common Interface Implementation
// *****************************************************************************
// *****************************************************************************
void DRV_PLC_PHY_Init(DRV_PLC_PHY_OBJ *pl360)
{
    gPlcPhyObj = pl360;
    
    /* Clear information about PLC events */
    gPlcPhyObj->evTxCfm[0] = false;
    gPlcPhyObj->evRxPar = false;
    gPlcPhyObj->evRxDat = false;
    gPlcPhyObj->evRegRspLength = 0;
    gPlcPhyObj->evResetTxCfm = false;
    
    /* Enable external interrupt from PLC */
    gPlcPhyObj->plcHal->enableExtInt(true);
}

void DRV_PLC_PHY_Task(void)
{
    /* Check event flags */
    if ((gPlcPhyObj->evTxCfm[0]) || (gPlcPhyObj->evResetTxCfm))
    {
        DRV_PLC_PHY_TRANSMISSION_CFM_OBJ cfmObj;
        
        if (gPlcPhyObj->evResetTxCfm)
        {
            gPlcPhyObj->evResetTxCfm = false;
            gPlcPhyObj->state = DRV_PLC_PHY_STATE_IDLE;
            
            cfmObj.rmsCalc = 0;
            cfmObj.time = 0;
            cfmObj.result = DRV_PLC_PHY_TX_RESULT_NO_TX;
        } else {
            _DRV_PLC_PHY_COMM_TxCfmEvent(&cfmObj);            
        }
        
        if (gPlcPhyObj->dataCfmCallback)
        {
            /* Report to upper layer */
            gPlcPhyObj->dataCfmCallback(&cfmObj, gPlcPhyObj->contextCfm);
        }
        
        /* Reset event flag */
        gPlcPhyObj->evTxCfm[0] = false;
    }
    
    if (gPlcPhyObj->evRxPar && gPlcPhyObj->evRxDat)
    {
        DRV_PLC_PHY_RECEPTION_OBJ rxObj;
        
        _DRV_PLC_PHY_COMM_RxEvent(&rxObj);
        if (gPlcPhyObj->dataIndCallback)
        {
            /* Report to upper layer */
            gPlcPhyObj->dataIndCallback(&rxObj, gPlcPhyObj->contextInd);
        }
        
        /* Reset event flags */
        gPlcPhyObj->evRxPar = false;
        gPlcPhyObj->evRxDat = false;
    }
}

void DRV_PLC_PHY_Send(const DRV_HANDLE handle, DRV_PLC_PHY_TRANSMISSION_OBJ *transmitObj)
{    
    if((handle != DRV_HANDLE_INVALID) && (handle == 0) && (gPlcPhyObj->state == DRV_PLC_PHY_STATE_IDLE))
    {
        size_t size_params;
        
        size_params = _DRV_PLC_PHY_COMM_TxStringify(transmitObj);
        
        if (size_params)
        {
            /* Update PLC state: transmitting */
            gPlcPhyObj->state = DRV_PLC_PHY_STATE_TX;
            
            /* Send TX parameters */
            _DRV_PLC_PHY_COMM_SpiWriteCmd(TX_PAR_ID, sDataTxPar, size_params);
            
            /* Waiting CFM to avoid soon error responses */
            gPlcPhyObj->plcHal->delay(200);
            
            /* Send TX data */
            if (gPlcPhyObj->state == DRV_PLC_PHY_STATE_TX)
            {
                /* Send TX data content */
                _DRV_PLC_PHY_COMM_SpiWriteCmd(TX_DAT_ID, transmitObj->pTransmitData, transmitObj->dataLength);
            
                /* Update PLC state: waiting confirmation */
                gPlcPhyObj->state = DRV_PLC_PHY_STATE_WAITING_TX_CFM;
            }
        }
    }
}

bool DRV_PLC_PHY_PIBGet(const DRV_HANDLE handle, DRV_PLC_PHY_PIB_OBJ *pibObj)
{    
    if((handle != DRV_HANDLE_INVALID) && (handle == 0))
    {
        if (pibObj->id == PLC_ID_TIME_REF_ID)
        {
            /* Send PIB information request */
            _DRV_PLC_PHY_COMM_SpiReadCmd(STATUS_ID, pibObj->pData, pibObj->length);
            return true;
        }
        else if (pibObj->id & DRV_PLC_PHY_REG_ID_MASK)
        {
            uint8_t *pDst;
            uint32_t address;
            uint32_t offset;
            uint16_t secureCnt;
            uint16_t cmdLength;

            offset = (uint32_t)(pibObj->id & DRV_PLC_PHY_REG_OFFSET_MASK);

            /* Get address offset */
            address = _DRV_PLC_PHY_COMM_GetPibBaseAddress(pibObj->id);
            if (address == 0)
            {
                return false;
            }
            address += offset;
            
            /* Set CMD and length */
            cmdLength = DRV_PLC_PHY_CMD_READ | (pibObj->length & DRV_PLC_PHY_REG_LEN_MASK);

            /* Build command */
            pDst = sDataReg;

            *pDst++ = (uint8_t)(address >> 24);
            *pDst++ = (uint8_t)(address >> 16);
            *pDst++ = (uint8_t)(address >> 8);
            *pDst++ = (uint8_t)(address);
            *pDst++ = (uint8_t)(cmdLength >> 8);
            *pDst++ = (uint8_t)(cmdLength);

            /* Send PIB information request */
            _DRV_PLC_PHY_COMM_SpiWriteCmd(REG_INFO_ID, sDataReg, 8);

            /* Wait to the response : Check length of the register response */
            secureCnt = 0xFFFF;
            while (!gPlcPhyObj->evRegRspLength) {
                if (!secureCnt--) {
                    /* Didn't came the expected response */
                    return false;
                }
            }

            /* copy Register info in data pointer */
            memcpy(pibObj->pData, sDataReg, pibObj->length);
            /* Reset length of the register response */
            gPlcPhyObj->evRegRspLength = 0;

            return true;
        } 
        else 
        {
            uint32_t value;
            
            /* Get HOST information */
            switch(pibObj->id)
            {
                case PLC_ID_HOST_DESCRIPTION_ID:
                    memcpy(pibObj->pData, DRV_PLC_PHY_HOST_DESC, 10);
                    break;
                    
                case PLC_ID_HOST_MODEL_ID:
                    value = DRV_PLC_PHY_HOST_MODEL;
                    memcpy(pibObj->pData, (uint8_t*)&value, 2);
                    break;
                    
                case PLC_ID_HOST_PHY_ID:
                    value = DRV_PLC_PHY_HOST_PHY;
                    memcpy(pibObj->pData, (uint8_t*)&value, 4);
                    break;
                    
                case PLC_ID_HOST_PRODUCT_ID:
                    value = DRV_PLC_PHY_HOST_PRODUCT;
                    memcpy(pibObj->pData, (uint8_t*)&value, 2);
                    break;
                    
                case PLC_ID_HOST_VERSION_ID:
                    value = DRV_PLC_PHY_HOST_VERSION;
                    memcpy(pibObj->pData, (uint8_t*)&value, 4);
                    break;
                    
                case PLC_ID_HOST_BAND_ID:
                    value = DRV_PLC_PHY_HOST_BAND;
                    memcpy(pibObj->pData, (uint8_t*)&value, 1);
                    break;
                    
                default:
                    return false;
            }
            
            return true;
        }
    }
    else
    {
        return false;
    }
}

bool DRV_PLC_PHY_PIBSet(const DRV_HANDLE handle, DRV_PLC_PHY_PIB_OBJ *pibObj)
{    
    if((handle != DRV_HANDLE_INVALID) && (handle == 0))
    {
        if (pibObj->id & DRV_PLC_PHY_REG_ID_MASK)
        {
            uint8_t *pDst;
            uint8_t *pSrc;
            uint32_t address;
            uint32_t offset;
            uint16_t delay;
            uint16_t cmdLength;

            offset = (uint32_t)(pibObj->id & DRV_PLC_PHY_REG_OFFSET_MASK);

            /* Get base address */
            address = _DRV_PLC_PHY_COMM_GetPibBaseAddress(pibObj->id);
            if (address == 0)
            {
                return false;
            }
            address += offset;
            
            /* Set CMD and length */
            cmdLength = DRV_PLC_PHY_CMD_WRITE | (pibObj->length & DRV_PLC_PHY_REG_LEN_MASK);

            /* Build command */
            pDst = sDataReg;

            *pDst++ = (uint8_t)(address >> 24);
            *pDst++ = (uint8_t)(address >> 16);
            *pDst++ = (uint8_t)(address >> 8);
            *pDst++ = (uint8_t)(address);
            *pDst++ = (uint8_t)(cmdLength >> 8);
            *pDst++ = (uint8_t)(cmdLength);
            
            pSrc = pibObj->pData;
            if (pibObj->length == 4) {
                *pDst++ = *pSrc++;
                *pDst++ = *pSrc++;
                *pDst++ = *pSrc++;
                *pDst++ = *pSrc++;
            } else if (pibObj->length == 2) {
                *pDst++ = *pSrc++;
                *pDst++ = *pSrc++;
            } else {
                memcpy(pDst, pSrc, pibObj->length);
                pDst += pibObj->length;
            }

            /* Send PIB information request */
            _DRV_PLC_PHY_COMM_SpiWriteCmd(REG_INFO_ID, sDataReg, pDst - sDataReg);

            /* Guard delay to ensure writing operation completion. */
            delay = _DRV_PLC_PHY_COMM_GetDelayUs(pibObj->id);
            gPlcPhyObj->plcHal->delay(delay);

            return true;
        }
    }
    
    return false;
}

void DRV_PLC_PHY_ExternalInterruptHandler(PIO_PIN pin, uintptr_t context)
{   
    /* Avoid warning */
    (void)context;

    if ((gPlcPhyObj) && (pin == (PIO_PIN)gPlcPhyObj->plcHal->plcPlib->extIntPin))
    {
        DRV_PLC_PHY_EVENTS_OBJ evObj;
        
        /* Time guard ?? */
        gPlcPhyObj->plcHal->delay(20);
        
        /* Get PLC events information */
        _DRV_PLC_PHY_COMM_GetEventsInfo(&evObj);
        
        /* Check confirmation of the transmission event */
        if (evObj.evCfm)
        {
            _DRV_PLC_PHY_COMM_SpiReadCmd(TX_CFM_ID, sDataTxCfm, PLC_CMF_PKT_SIZE);
            /* update event flag */
            gPlcPhyObj->evTxCfm[0] = true;
            /* Update PLC state: idle */
            gPlcPhyObj->state = DRV_PLC_PHY_STATE_IDLE;
        }
        
        /* Check received new parameters event (First event in RX) */
        if (evObj.evRxDat)
        {        
            _DRV_PLC_PHY_COMM_SpiReadCmd(RX_DAT_ID, sDataRxDat, evObj.rcvDataLength);
            /* update event flag */
            gPlcPhyObj->evRxDat = true;
        }
        
        /* Check received new data event (Second event in RX) */
        if (evObj.evRxPar)
        {
            _DRV_PLC_PHY_COMM_SpiReadCmd(RX_PAR_ID, sDataRxPar, PLC_RX_PAR_SIZE - 4);
            /* update event flag */
            gPlcPhyObj->evRxPar = true;
        }
        
        /* Check register info event */
        if (evObj.evReg)
        {     
            _DRV_PLC_PHY_COMM_SpiReadCmd(REG_INFO_ID, sDataReg, evObj.regRspLength);
            /* update event flag */
            gPlcPhyObj->evRegRspLength = evObj.regRspLength;
        }
        
        /* Time guard ?? */
        gPlcPhyObj->plcHal->delay(50);
    }
    
    /* PORTD Interrupt Status Clear */
    ((pio_registers_t*)DRV_PLC_EXT_INT_PIO_PORT)->PIO_ISR;
}
