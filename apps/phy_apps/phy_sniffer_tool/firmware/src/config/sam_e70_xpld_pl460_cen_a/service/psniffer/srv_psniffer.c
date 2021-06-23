/*******************************************************************************
  Implementation of Phy Sniffer Serialization.

  Company:
    Microchip Technology Inc.

  File Name:
    srv_psniffer.c

  Summary:
    Phy Sniffer serialization service used by Microchip PLC Sniffer Tool.

  Description:
    The Phy Sniffer serialization provides a service to format messages
    through serial connection in order to communicate with PLC Sniffer Tool 
    provided by Microchip.
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
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "stddef.h"
#include "string.h"
#include "srv_psniffer.h"
#include "driver/plc/phy/drv_plc_phy_comm.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************
static DRV_PLC_PHY_TRANSMISSION_OBJ gLastTxObj;
static uint8_t gLastTxData[512];
static uint16_t gLastRxPayloadSymbols;
static uint16_t gLastTxPayloadSymbols;

// *****************************************************************************
// *****************************************************************************
// Section: SRV_PSNIFFER Interface Routines
// *****************************************************************************
// *****************************************************************************
SRV_PSNIFFER_COMMAND SRV_PSNIFFER_GetCommand(uint8_t* pData)
{
    /* Extract Command */
    return (SRV_PSNIFFER_COMMAND)*pData;
}

void SRV_PSNIFFER_SetTxMessage(DRV_PLC_PHY_TRANSMISSION_OBJ* pDataDst)
{
    /* Use internal buffer to report TX messages as a received message when TX_CFM arrives */
    gLastTxObj.pTransmitData = gLastTxData;
    memcpy((uint8_t *)&gLastTxObj, (uint8_t *)pDataDst, sizeof(DRV_PLC_PHY_TRANSMISSION_OBJ));
    memcpy(gLastTxData, pDataDst->pTransmitData, pDataDst->dataLength);
}

void SRV_PSNIFFER_SetRxPayloadSymbols(uint16_t payloadSym)
{
    gLastRxPayloadSymbols = payloadSym;
}

void SRV_PSNIFFER_SetTxPayloadSymbols(uint16_t payloadSym)
{
    gLastTxPayloadSymbols = payloadSym;
}

size_t SRV_PSNIFFER_SerialRxMessage(uint8_t* pDataDst, DRV_PLC_PHY_RECEPTION_OBJ* pDataSrc)
{
    uint8_t* pData;
    uint32_t timeIni, timeEnd;
    
    pData = pDataDst;
    
    /* Insert command */
    *pData++ = SRV_PSNIFFER_CMD_RECEIVE_MSG; 

    /* Insert version identifiers */
    *pData++ = PSNIFFER_VERSION; 
    *pData++ = PSNIFFER_PROFILE; 
    
    /* Fill data depending on Data/ACK frame */
    if ((pDataSrc->delimiterType == DT_SOF_NO_RESP) || (pDataSrc->delimiterType == DT_SOF_RESP))
    {
        /* Data frame */
        /* ModType (high) + ModScheme (low) */
        *pData++ = (uint8_t)((pDataSrc->modScheme & 0x0F) + (((pDataSrc->modType) << 4) & 0xF0));
        /* Tone Map */
        if (PSNIFFER_TONEMAP_SIZE == 1) {
            *pData++ = 0;
            *pData++ = 0;
            *pData++ = pDataSrc->toneMap[0];
        } else {
            *pData++ = pDataSrc->toneMap[2];
            *pData++ = pDataSrc->toneMap[1];
            *pData++ = pDataSrc->toneMap[0];
        }

        /* Number of symbols (2 bytes) */
        *pData++ = (uint8_t)(gLastRxPayloadSymbols >> 8);
        *pData++ = (uint8_t)gLastRxPayloadSymbols;
        /* SNR */
        *pData++ = pDataSrc->lqi;

    }
    else
    {
        /* ACK */
        /* ModType (high) + ModScheme (low) */
        *pData++ = 0;
        /* Tone Map */
        *pData++ = 0;
        *pData++ = 0;
        *pData++ = 0;
        /* Number of symbols (2 bytes) */
        *pData++ = 0;
        *pData++ = 0;
        /* SNR */
        *pData++ = 0xFF;
    }

    /* Delimiter Type */
    *pData++ = pDataSrc->delimiterType;
    /* TimeIni */
    timeIni = pDataSrc->time - pDataSrc->frameDuration;

    *pData++ = (timeIni >> 24);
    *pData++ = (timeIni >> 16) & 0xFF;
    *pData++ = (timeIni >> 8) & 0xFF;
    *pData++ = timeIni & 0xFF;
    /* TimeEnd */
    timeEnd = pDataSrc->time;
    *pData++ = (timeEnd >> 24);
    *pData++ = (timeEnd >> 16) & 0xFF;
    *pData++ = (timeEnd >> 8) & 0xFF;
    *pData++ = timeEnd & 0xFF;
    /* RSSI */
    *pData++ = (uint8_t)(pDataSrc->rssi >> 8);
    *pData++ = (uint8_t)(pDataSrc->rssi);
    /* AGC_Factor */
    *pData++ = (uint8_t)(pDataSrc->agcFactor >> 8);
    *pData++ = (uint8_t)(pDataSrc->agcFactor);
    /* Length in bytes */
    *pData++ = (uint8_t)(pDataSrc->dataLength >> 8);
    *pData++ = (uint8_t)(pDataSrc->dataLength);
    /* PDU */
    memcpy(pData, pDataSrc->pReceivedData, pDataSrc->dataLength);
    pData += pDataSrc->dataLength;
    
    return (pData - pDataDst);    
}

size_t SRV_PSNIFFER_SerialCfmMessage(uint8_t* pDataDst, DRV_PLC_PHY_TRANSMISSION_CFM_OBJ* pDataCfm)
{
    uint8_t* pData;
    uint32_t timeIni, timeEnd;
    
    if (pDataCfm->result != DRV_PLC_PHY_TX_RESULT_SUCCESS)
    {
        /* Error in transmission: No report */
        return 0;
    }
    
    pData = pDataDst;
    
    /* Insert command */
    *pData++ = SRV_PSNIFFER_CMD_RECEIVE_MSG; 

    /* Insert version identifiers */
    *pData++ = PSNIFFER_VERSION; 
    *pData++ = PSNIFFER_PROFILE; 
    
    /* ModType (high) + ModScheme (low) */
    *pData++ = (uint8_t)((gLastTxObj.modScheme & 0x0F) + (((gLastTxObj.modType) << 4) & 0xF0));
    /* Tone Map */
    if (PSNIFFER_TONEMAP_SIZE == 1) {
        *pData++ = 0;
        *pData++ = 0;
        *pData++ = gLastTxObj.toneMap[0];
    } else {
        *pData++ = gLastTxObj.toneMap[2];
        *pData++ = gLastTxObj.toneMap[1];
        *pData++ = gLastTxObj.toneMap[0];
    }

    /* Number of symbols (2 bytes) */
    *pData++ = (uint8_t)(gLastTxPayloadSymbols >> 8);
    *pData++ = (uint8_t)gLastTxPayloadSymbols;
    /* SNR */
    *pData++ = 0xFF;
    /* Delimiter Type */
    *pData++ = gLastTxObj.delimiterType;
    /* TimeIni */
    timeIni = gLastTxObj.time;

    *pData++ = (timeIni >> 24);
    *pData++ = (timeIni >> 16) & 0xFF;
    *pData++ = (timeIni >> 8) & 0xFF;
    *pData++ = timeIni & 0xFF;
    /* TimeEnd */
    timeEnd = pDataCfm->time;
    *pData++ = (timeEnd >> 24);
    *pData++ = (timeEnd >> 16) & 0xFF;
    *pData++ = (timeEnd >> 8) & 0xFF;
    *pData++ = timeEnd & 0xFF;
    /* RSSI */
    *pData++ = (uint8_t)(0xFF >> 8);
    *pData++ = (uint8_t)(0xFF);
    /* AGC_Factor */
    *pData++ = (uint8_t)(0xFF >> 8);
    *pData++ = (uint8_t)(0xFF);
    /* Length in bytes */
    *pData++ = (uint8_t)(gLastTxObj.dataLength >> 8);
    *pData++ = (uint8_t)(gLastTxObj.dataLength);
    /* PDU */
    memcpy(pData, gLastTxData, gLastTxObj.dataLength);
    pData += gLastTxObj.dataLength;
    
    return (pData - pDataDst);    
}

void SRV_PSNIFFER_ConvertToneMask(uint8_t* pToneMaskDst, uint8_t* pToneMaskSrc)
{
    int i, j;
    uint8_t bitField;

    /* Convert bit-field to byte-field, reversing order */
    for (i = 8; i >= 0; i--)
    {
        bitField = pToneMaskSrc[i];
        for (j = 0; j< 8; j++) 
        {
          pToneMaskDst[(i*8)+j] = (bitField & 0x01)? 0: 1;
          bitField >>=1;
        }
    }
}
