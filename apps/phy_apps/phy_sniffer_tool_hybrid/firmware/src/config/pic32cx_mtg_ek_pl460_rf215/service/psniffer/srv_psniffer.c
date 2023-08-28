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
* Copyright (C) 2023 Microchip Technology Inc. and its subsidiaries.
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
static DRV_PLC_PHY_TRANSMISSION_OBJ srvPsnifferLastTxObj;
static uint8_t srvPsnifferLastTxData[512];
static uint16_t srvPsnifferLastRxPayloadSymbols;
static uint16_t srvPsnifferLastTxPayloadSymbols;

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

void SRV_PSNIFFER_SetTxMessage(DRV_PLC_PHY_TRANSMISSION_OBJ *pTxObj)
{
    /* Use internal buffer to report TX messages as a received message when TX_CFM arrives */
    srvPsnifferLastTxObj = *pTxObj;
    (void) memcpy(srvPsnifferLastTxData, pTxObj->pTransmitData, pTxObj->dataLength);
}

void SRV_PSNIFFER_SetRxPayloadSymbols(uint16_t payloadSym)
{
    srvPsnifferLastRxPayloadSymbols = payloadSym;
}

void SRV_PSNIFFER_SetTxPayloadSymbols(uint16_t payloadSym)
{
    srvPsnifferLastTxPayloadSymbols = payloadSym;
}

size_t SRV_PSNIFFER_SerialRxMessage(uint8_t* pDataDst, DRV_PLC_PHY_RECEPTION_OBJ* pDataSrc)
{
    ptrdiff_t size;
    uint8_t* pData;
    uint32_t timeIni, timeEnd;
    
    pData = pDataDst;
    
    /* Insert command */
    *pData++ = (uint8_t)SRV_PSNIFFER_CMD_RECEIVE_MSG; 

    /* Insert version identifiers */
    *pData++ = PSNIFFER_VERSION; 
    *pData++ = PSNIFFER_PROFILE; 
    
    /* Fill data depending on Data/ACK frame */
    if ((pDataSrc->delimiterType == DT_SOF_NO_RESP) || (pDataSrc->delimiterType == DT_SOF_RESP))
    {
        /* Data frame */
        /* ModType (high) + ModScheme (low) */
        *pData++ = (uint8_t)pDataSrc->modScheme + ((uint8_t)pDataSrc->modType << 4);
        /* Tone Map */
        *pData++ = pDataSrc->toneMap[2];
        *pData++ = pDataSrc->toneMap[1];
        *pData++ = pDataSrc->toneMap[0];

        /* Number of symbols (2 bytes) */
        *pData++ = (uint8_t)(srvPsnifferLastRxPayloadSymbols >> 8);
        *pData++ = (uint8_t)srvPsnifferLastRxPayloadSymbols;
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
    *pData++ = (uint8_t)pDataSrc->delimiterType;
    /* TimeIni */
    timeIni = pDataSrc->timeEnd - pDataSrc->frameDuration;

    *pData++ = (uint8_t)(timeIni >> 24);
    *pData++ = (uint8_t)(timeIni >> 16);
    *pData++ = (uint8_t)(timeIni >> 8);
    *pData++ = (uint8_t)timeIni;
    /* TimeEnd */
    timeEnd = pDataSrc->timeEnd;
    *pData++ = (uint8_t)(timeEnd >> 24);
    *pData++ = (uint8_t)(timeEnd >> 16);
    *pData++ = (uint8_t)(timeEnd >> 8);
    *pData++ = (uint8_t)timeEnd;
    /* RSSI */
    *pData++ = (uint8_t)(pDataSrc->rssi >> 8);
    *pData++ = (uint8_t)pDataSrc->rssi;
    /* AGC_Factor */
    *pData++ = (uint8_t)(pDataSrc->agcFactor >> 8);
    *pData++ = (uint8_t)pDataSrc->agcFactor;
    /* Length in bytes */
    *pData++ = (uint8_t)(pDataSrc->dataLength >> 8);
    *pData++ = (uint8_t)pDataSrc->dataLength;
    /* PDU */
    (void) memcpy(pData, pDataSrc->pReceivedData, pDataSrc->dataLength);
    pData += pDataSrc->dataLength;

    size = pData - pDataDst;
    return (size_t)size;
}

size_t SRV_PSNIFFER_SerialCfmMessage(uint8_t* pDataDst, DRV_PLC_PHY_TRANSMISSION_CFM_OBJ* pCfmObj)
{
    ptrdiff_t size;
    uint8_t* pData;
    uint32_t timeIni, timeEnd;
    
    if (pCfmObj->result != DRV_PLC_PHY_TX_RESULT_SUCCESS)
    {
        /* Error in transmission: No report */
        return 0;
    }
    
    pData = pDataDst;
    
    /* Insert command */
    *pData++ = (uint8_t)SRV_PSNIFFER_CMD_RECEIVE_MSG;

    /* Insert version identifiers */
    *pData++ = PSNIFFER_VERSION; 
    *pData++ = PSNIFFER_PROFILE; 
    
    /* ModType (high) + ModScheme (low) */
    *pData++ = (uint8_t)srvPsnifferLastTxObj.modScheme + ((uint8_t)srvPsnifferLastTxObj.modType << 4);
    /* Tone Map */
    *pData++ = srvPsnifferLastTxObj.toneMap[2];
    *pData++ = srvPsnifferLastTxObj.toneMap[1];
    *pData++ = srvPsnifferLastTxObj.toneMap[0];

    /* Number of symbols (2 bytes) */
    *pData++ = (uint8_t)(srvPsnifferLastTxPayloadSymbols >> 8);
    *pData++ = (uint8_t)srvPsnifferLastTxPayloadSymbols;
    /* SNR */
    *pData++ = 0xFF;
    /* Delimiter Type */
    *pData++ = (uint8_t)srvPsnifferLastTxObj.delimiterType;
    /* TimeIni */
    timeIni = srvPsnifferLastTxObj.timeIni;

    *pData++ = (uint8_t)(timeIni >> 24);
    *pData++ = (uint8_t)(timeIni >> 16);
    *pData++ = (uint8_t)(timeIni >> 8);
    *pData++ = (uint8_t)timeIni;
    /* TimeEnd */
    timeEnd = pCfmObj->timeEnd;
    *pData++ = (uint8_t)(timeEnd >> 24);
    *pData++ = (uint8_t)(timeEnd >> 16);
    *pData++ = (uint8_t)(timeEnd >> 8);
    *pData++ = (uint8_t)timeEnd;
    /* RSSI */
    *pData++ = 0;
    *pData++ = 0xFF;
    /* AGC_Factor */
    *pData++ = 0;
    *pData++ = 0xFF;
    /* Length in bytes */
    *pData++ = (uint8_t)(srvPsnifferLastTxObj.dataLength >> 8);
    *pData++ = (uint8_t)srvPsnifferLastTxObj.dataLength;
    /* PDU */
    (void) memcpy(pData, srvPsnifferLastTxData, srvPsnifferLastTxObj.dataLength);
    pData += srvPsnifferLastTxObj.dataLength;

    size = pData - pDataDst;
    return (size_t)size;
}

void SRV_PSNIFFER_ConvertToneMask(uint8_t* pToneMaskDst, uint8_t* pToneMaskSrc)
{
    int8_t i, j;
    uint8_t bitField;

    /* Convert bit-field to byte-field, reversing order */
    for (i = 8; i >= 0; i--)
    {
        bitField = pToneMaskSrc[i];
        for (j = 0; j < 8; j++) 
        {
            pToneMaskDst[(i * 8) + j] = ((bitField & 1U) != 0U)? 0U: 1U;
            bitField >>= 1;
        }
    }
}
