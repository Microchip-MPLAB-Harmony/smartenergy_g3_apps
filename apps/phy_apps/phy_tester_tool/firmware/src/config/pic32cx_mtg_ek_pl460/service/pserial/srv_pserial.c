/*******************************************************************************
  Phy layer serialization service used by Microchip PLC Tools

  Company:
    Microchip Technology Inc.

  File Name:
    srv_pserial.c

  Summary:
    Phy layer serialization service used by Microchip PLC Tools.

  Description:
    The Phy layer serialization provides a service to format messages
    through serial connection in order to communicate with PLC Tools provided
    by Microchip.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*
Copyright (C) 2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "stddef.h"
#include "string.h"
#include "srv_pserial.h"
#include "driver/plc/phy/drv_plc_phy_comm.h"

// *****************************************************************************
// *****************************************************************************
// Section: File scope functions
// *****************************************************************************
// *****************************************************************************

static void lSRV_SERIAL_memcpyRev(uint8_t *pDataDst, uint8_t *pDataSrc, size_t length)
{
    uint8_t *pMemDst, *pMemSrc;
    uint16_t indexRev;

    if (length <= 4U)
    {
        pMemDst = pDataDst + length - 1;
        pMemSrc = pDataSrc;
        for (indexRev = 0; indexRev < length; indexRev++)
        {
            *pMemDst-- = (uint8_t)*pMemSrc++;
        }
    }
    else
    {
        (void) memcpy(pDataDst, pDataSrc, length);
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: SRV_PSERIAL Interface Routines
// *****************************************************************************
// *****************************************************************************

SRV_PSERIAL_COMMAND SRV_PSERIAL_GetCommand(uint8_t* pData)
{
    /* Extract Command */
    return (SRV_PSERIAL_COMMAND)*pData;
}

void SRV_PSERIAL_ParseGetPIB(DRV_PLC_PHY_PIB_OBJ* pDataDst, uint8_t* pDataSrc)
{
    uint16_t id;

    /* Skip command */
    pDataSrc++;
    /* Extract parameters of PIB */
    id = (uint16_t)*pDataSrc++ << 8;
    id += (uint16_t)*pDataSrc++;
    pDataDst->id = (DRV_PLC_PHY_ID)id;
    pDataDst->length = *pDataSrc;
}

size_t SRV_PSERIAL_SerialGetPIB(uint8_t* pDataDst, DRV_PLC_PHY_PIB_OBJ* pDataSrc)
{
    uint8_t* pData;

    pData = pDataDst;

    /* Insert command */
    *pData++ = (uint8_t)SRV_PSERIAL_CMD_PHY_GET_CFG_RSP;

    /* Serialize parameters of PIB */
    *pData++ = (uint8_t)((uint16_t)pDataSrc->id >> 8);
    *pData++ = (uint8_t)pDataSrc->id;
    *pData++ = (uint8_t)pDataSrc->length;
    lSRV_SERIAL_memcpyRev(pData, pDataSrc->pData, pDataSrc->length);

    return (size_t)pDataSrc->length + 4U;
}

void SRV_PSERIAL_ParseSetPIB(DRV_PLC_PHY_PIB_OBJ* pDataDst, uint8_t* pDataSrc)
{
    uint16_t id;

    /* Skip command */
    pDataSrc++;
    /* Extract parameters of PIB */
    id = (uint16_t)*pDataSrc++ << 8;
    id += (uint16_t)*pDataSrc++;
    pDataDst->id = (DRV_PLC_PHY_ID)id;
    pDataDst->length = *pDataSrc++;
    lSRV_SERIAL_memcpyRev(pDataDst->pData, pDataSrc, pDataDst->length);
}

size_t SRV_PSERIAL_SerialSetPIB(uint8_t* pDataDst, DRV_PLC_PHY_PIB_OBJ* pDataSrc)
{
    uint8_t* pData;

    pData = pDataDst;

    /* Insert command */
    *pData++ = (uint8_t)SRV_PSERIAL_CMD_PHY_SET_CFG_RSP;

    /* Serialize parameters of PIB */
    *pData++ = (uint8_t)((uint16_t)pDataSrc->id >> 8);
    *pData++ = (uint8_t)pDataSrc->id;
    *pData++ = (uint8_t)pDataSrc->length;
    *pData = 1U;

    return 5U;
}

void SRV_PSERIAL_ParseTxMessage(DRV_PLC_PHY_TRANSMISSION_OBJ* pDataDst, uint8_t* pDataSrc)
{
    uint8_t indexTM;

    /* Skip command */
    pDataSrc++;

    /* Extract parameters of transmission */
    pDataDst->mode = *pDataSrc++;
    pDataDst->attenuation = *pDataSrc++;
    pDataDst->modType = (DRV_PLC_PHY_MOD_TYPE)(*pDataSrc++);
    pDataDst->modScheme = (DRV_PLC_PHY_MOD_SCHEME)(*pDataSrc++);
    pDataDst->pdc = *pDataSrc++;

    for (indexTM = 0; indexTM < PSERIAL_TONEMAP_SIZE; indexTM++)
    {
        pDataDst->toneMap[PSERIAL_TONEMAP_SIZE - (indexTM + 1U)] = *pDataSrc++;
    }

    pDataDst->rs2Blocks = *pDataSrc++;

    (void) memcpy(pDataDst->preemphasis, pDataSrc, PSERIAL_SUBBANDS_SIZE);
    pDataSrc += PSERIAL_SUBBANDS_SIZE;

    pDataDst->delimiterType = (DRV_PLC_PHY_DEL_TYPE)(*pDataSrc++);
    pDataDst->timeIni = ((uint32_t)*pDataSrc++) << 24;
    pDataDst->timeIni += ((uint32_t)*pDataSrc++) << 16;
    pDataDst->timeIni += ((uint32_t)*pDataSrc++) << 8;
    pDataDst->timeIni += (uint32_t)*pDataSrc++;
    pDataDst->dataLength = ((uint16_t)*pDataSrc++) << 8;
    pDataDst->dataLength += (uint16_t)*pDataSrc++;

    if (pDataDst->dataLength <= PSERIAL_MAX_DATA_LEN)
    {
        /* copy data */
        (void) memcpy(pDataDst->pTransmitData, pDataSrc, pDataDst->dataLength);
    }
}

size_t SRV_PSERIAL_SerialRxMessage(uint8_t* pDataDst, DRV_PLC_PHY_RECEPTION_OBJ* pDataSrc)
{
    ptrdiff_t size;
    uint8_t* pData;
    uint8_t indexTM;

    pData = pDataDst;

    /* Insert command */
    *pData++ = (uint8_t)SRV_PSERIAL_CMD_PHY_RECEIVE_MSG;

    /* Serialize parameters of reception */
    *pData++ = (uint8_t)(pDataSrc->modType);
    *pData++ = (uint8_t)(pDataSrc->modScheme);

    for (indexTM = 0; indexTM < PSERIAL_TONEMAP_SIZE; indexTM++)
    {
        *pData++ = pDataSrc->toneMap[PSERIAL_TONEMAP_SIZE - (indexTM + 1U)];
    }

    *pData++ = (uint8_t)((uint16_t)pDataSrc->snrFch >> 8);
    *pData++ = (uint8_t)pDataSrc->snrFch;
    *pData++ = (uint8_t)((uint16_t)pDataSrc->snrPay >> 8);
    *pData++ = (uint8_t)pDataSrc->snrPay;
    *pData++ = (uint8_t)(pDataSrc->rssi >> 8);
    *pData++ = (uint8_t)pDataSrc->rssi;
    *pData++ = (uint8_t)(pDataSrc->agcFactor >> 24);
    *pData++ = (uint8_t)(pDataSrc->agcFactor >> 16);
    *pData++ = (uint8_t)(pDataSrc->agcFactor >> 8);
    *pData++ = (uint8_t)pDataSrc->agcFactor;
    *pData++ = pDataSrc->zctDiff;
    *pData++ = (uint8_t)pDataSrc->delimiterType;
    *pData++ = pDataSrc->lqi;

    (void) memcpy(pData, pDataSrc->carrierSnr, PSERIAL_CARRIERS_SIZE);
    pData += PSERIAL_CARRIERS_SIZE;

    *pData++ = pDataSrc->payloadSnrWorstCarrier;
    *pData++ = (uint8_t)(pDataSrc->payloadCorruptedCarriers >> 8);
    *pData++ = (uint8_t)pDataSrc->payloadCorruptedCarriers;
    *pData++ = (uint8_t)(pDataSrc->payloadNoisedSymbols >> 8);
    *pData++ = (uint8_t)pDataSrc->payloadNoisedSymbols;
    *pData++ = pDataSrc->payloadSnrWorstSymbol;
    *pData++ = pDataSrc->payloadSnrImpulsive;
    *pData++ = pDataSrc->payloadSnrBand;
    *pData++ = pDataSrc->payloadSnrBackground;
    *pData++ = pDataSrc->rsCorrectedErrors;
    *pData++ = (uint8_t)(pDataSrc->dataLength >> 8);
    *pData++ = (uint8_t)pDataSrc->dataLength;
    *pData++ = (uint8_t)(pDataSrc->timeEnd >> 24);
    *pData++ = (uint8_t)(pDataSrc->timeEnd >> 16);
    *pData++ = (uint8_t)(pDataSrc->timeEnd >> 8);
    *pData++ = (uint8_t)pDataSrc->timeEnd;
    *pData++ = (uint8_t)(pDataSrc->frameDuration >> 24);
    *pData++ = (uint8_t)(pDataSrc->frameDuration >> 16);
    *pData++ = (uint8_t)(pDataSrc->frameDuration >> 8);
    *pData++ = (uint8_t)pDataSrc->frameDuration;

    (void) memcpy(pData, pDataSrc->pReceivedData, pDataSrc->dataLength);
    pData += pDataSrc->dataLength;

    size = pData - pDataDst;
    return (size_t)size;
}

size_t SRV_PSERIAL_SerialCfmMessage(uint8_t* pDataDst, DRV_PLC_PHY_TRANSMISSION_CFM_OBJ* pDataSrc)
{
    uint8_t* pData;

    pData = pDataDst;

    /* Insert command */
    *pData++ = (uint8_t)SRV_PSERIAL_CMD_PHY_SEND_MSG_RSP;

    /* Serialize parameters of Confirm Message */
    *pData++ = (uint8_t)pDataSrc->result;
    *pData++ = (uint8_t)(pDataSrc->rmsCalc >> 24);
    *pData++ = (uint8_t)(pDataSrc->rmsCalc >> 16);
    *pData++ = (uint8_t)(pDataSrc->rmsCalc >> 8);
    *pData++ = (uint8_t)pDataSrc->rmsCalc;
    *pData++ = (uint8_t)(pDataSrc->timeEnd >> 24);
    *pData++ = (uint8_t)(pDataSrc->timeEnd >> 16);
    *pData++ = (uint8_t)(pDataSrc->timeEnd >> 8);
    *pData = (uint8_t)pDataSrc->timeEnd;

    return 10U;
}
