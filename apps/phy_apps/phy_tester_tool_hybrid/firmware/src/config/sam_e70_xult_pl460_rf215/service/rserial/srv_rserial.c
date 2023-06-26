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
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <string.h>
#include "srv_rserial.h"
#include "system/time/sys_time.h"

// *****************************************************************************
// *****************************************************************************
// Section: Macro definitions
// *****************************************************************************
// *****************************************************************************

/* Buffer sizes */
#define RSERIAL_RX_MSG_HEADER_SIZE  15
#define RSERIAL_RX_MSG_MAX_SIZE     (DRV_RF215_MAX_PSDU_LEN + RSERIAL_RX_MSG_HEADER_SIZE)

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

typedef struct
{
    DRV_RF215_TX_HANDLE txHandle;
    uint8_t             txId;
    bool                inUse;

} SRV_RSERIAL_TX_HANDLE;

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Data
// *****************************************************************************
// *****************************************************************************

static uint64_t srvRserialPrevSysTime = 0;
static uint32_t srvRserialPrevTimeUS = 0;
static SRV_RSERIAL_TX_HANDLE srvRserialTxHandles[DRV_RF215_TX_BUFFERS_NUMBER];
static uint8_t srvRserialBuffer[RSERIAL_RX_MSG_MAX_SIZE];
static uint8_t srvRserialLastTxId;

// *****************************************************************************
// *****************************************************************************
// Section: File scope functions
// *****************************************************************************
// *****************************************************************************

static void _SRV_RSERIAL_memcpyRev(uint8_t *pDataDst, uint8_t *pDataSrc, size_t length)
{
    uint8_t *pMemDst, *pMemSrc;
    uint16_t indexRev;

    if (length <= 4)
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
        memcpy(pDataDst, pDataSrc, length);
    }
}

static uint32_t _SRV_RSERIAL_SysTimeToUS(uint64_t sysTime)
{
    uint64_t sysTimeDiff;
    uint32_t sysTimeDiffNumHigh, sysTimeDiffRemaining;
    uint32_t timeUS = srvRserialPrevTimeUS;

    /* Difference between current and previous system time */
    sysTimeDiff = sysTime - srvRserialPrevSysTime;
    sysTimeDiffNumHigh = (uint32_t) (sysTimeDiff / 0x10000000);
    sysTimeDiffRemaining = (uint32_t) (sysTimeDiff % 0x10000000);

    /* Convert system time to microseconds and add to previous time */
    timeUS += (SYS_TIME_CountToUS(0x10000000) * sysTimeDiffNumHigh);
    timeUS += SYS_TIME_CountToUS(sysTimeDiffRemaining);

    /* Store times for next computation */
    srvRserialPrevSysTime = sysTime;
    srvRserialPrevTimeUS = timeUS;

    return timeUS;
}

// *****************************************************************************
// *****************************************************************************
// Section: RF PHY Layer Serialization Interface Implementation
// *****************************************************************************
// *****************************************************************************

SRV_RSERIAL_COMMAND SRV_RSERIAL_GetCommand(uint8_t* pData)
{
    /* Extract Command */
    return (SRV_RSERIAL_COMMAND) *pData;
}

uint8_t* SRV_RSERIAL_ParsePIB (
    uint8_t* pDataSrc,
    DRV_RF215_TRX_ID* pTrxId,
    DRV_RF215_PIB_ATTRIBUTE* pPibAttr,
    uint8_t* pPibSize
)
{
    uint16_t pibAttr;

    /* Skip command */
    pDataSrc++;

    /* Extract TRX identifier (Sub-1GHz, 2.4GHz) */
    *pTrxId = (DRV_RF215_TRX_ID) *pDataSrc++;

    /* Extract parameters of PIB */
    pibAttr = ((uint16_t) *pDataSrc++) << 8;
    pibAttr += (uint16_t) *pDataSrc++;
    *pPibAttr = (DRV_RF215_PIB_ATTRIBUTE) pibAttr;
    *pPibSize = *pDataSrc++;

    return pDataSrc;
}

uint8_t* SRV_RSERIAL_SerialGetPIB (
    DRV_RF215_TRX_ID trxId,
    DRV_RF215_PIB_ATTRIBUTE pibAttr,
    uint8_t pibSize,
    DRV_RF215_PIB_RESULT pibResult,
    uint8_t* pPibData,
    size_t* pMsgLen
)
{
    uint8_t* pDataDest = srvRserialBuffer;

    /* Command type */
    *pDataDest++ = (uint8_t) SRV_RSERIAL_CMD_PHY_GET_CFG_RSP;

    /* TRX identifier (Sub-1GHz, 2.4GHz) */
    *pDataDest++ = (uint8_t) trxId;

    /* PIB identifier, length and result */
    *pDataDest++ = (uint8_t) (pibAttr >> 8);
    *pDataDest++ = (uint8_t) (pibAttr);
    *pDataDest++ = (uint8_t) (pibSize);
    *pDataDest++ = (uint8_t) (pibResult);

    /* PIB data */
    _SRV_RSERIAL_memcpyRev(pDataDest, pPibData, pibSize);

    *pMsgLen = (size_t) pibSize + 6;
    return srvRserialBuffer;
}

uint8_t* SRV_RSERIAL_SerialSetPIB (
    DRV_RF215_TRX_ID trxId,
    DRV_RF215_PIB_ATTRIBUTE pibAttr,
    uint8_t pibSize,
    DRV_RF215_PIB_RESULT pibResult,
    size_t* pMsgLen
)
{
    uint8_t* pDataDest = srvRserialBuffer;

    /* Command type */
    *pDataDest++ = (uint8_t) SRV_RSERIAL_CMD_PHY_SET_CFG_RSP;

    /* TRX identifier (Sub-1GHz, 2.4GHz) */
    *pDataDest++ = (uint8_t) trxId;

    /* PIB identifier, length and result */
    *pDataDest++ = (uint8_t) (pibAttr >> 8);
    *pDataDest++ = (uint8_t) (pibAttr);
    *pDataDest++ = (uint8_t) (pibSize);
    *pDataDest++ = (uint8_t) (pibResult);

    *pMsgLen = 6;
    return srvRserialBuffer;
}

DRV_RF215_TRX_ID SRV_RSERIAL_ParseTxMessageTrxId(uint8_t* pDataSrc)
{
    /* Skip command */
    pDataSrc++;

    /* Extract TRX identifier (Sub-1GHz, 2.4GHz) */
    return (DRV_RF215_TRX_ID) *pDataSrc++;
}

bool SRV_RSERIAL_ParseTxMessage (
    uint8_t* pDataSrc,
    DRV_RF215_TX_REQUEST_OBJ* pDataDst,
    DRV_RF215_TX_HANDLE* pTxHandleCancel
)
{
    uint32_t txTimeUS;
    uint8_t timeMode;
    bool txCancel;

    /* Skip command and TRX identifier */
    pDataSrc += 2;

    /* Extract TX parameters */
    txTimeUS = ((uint32_t) *pDataSrc++) << 24;
    txTimeUS += ((uint32_t) *pDataSrc++) << 16;
    txTimeUS += ((uint32_t) *pDataSrc++) << 8;
    txTimeUS += (uint32_t) *pDataSrc++;
    pDataDst->psduLen = ((uint16_t) *pDataSrc++) << 8;
    pDataDst->psduLen += (uint16_t) *pDataSrc++;
    pDataDst->modScheme = (DRV_RF215_PHY_MOD_SCHEME) *pDataSrc++;
    pDataDst->ccaMode = (DRV_RF215_PHY_CCA_MODE) *pDataSrc++;
    timeMode = *pDataSrc++;
    pDataDst->cancelByRx = (bool) *pDataSrc++;
    pDataDst->txPwrAtt = *pDataSrc++;
    srvRserialLastTxId = *pDataSrc++;
    pDataSrc++; // Skip CCA contention window

    /* Pointer to TX data */
    pDataDst->psdu = pDataSrc;

    /* Parse time mode and TX time */
    txCancel = false;
    switch (timeMode)
    {
        case 0:
            /* Absolute */
        {
            uint32_t timeDiffNumHigh, timeDiffRemaining;
            uint64_t sysTime = srvRserialPrevSysTime;
            uint32_t timeDiffUS = txTimeUS - srvRserialPrevTimeUS;

            /* Convert microseconds to system time and add to previous time */
            timeDiffNumHigh = timeDiffUS / 0x10000000;
            timeDiffRemaining = timeDiffUS % 0x10000000;
            sysTime += (SYS_TIME_USToCount(0x10000000) * timeDiffNumHigh);
            sysTime += SYS_TIME_USToCount(timeDiffRemaining);

            pDataDst->timeMode = TX_TIME_ABSOLUTE;
            pDataDst->timeCount = sysTime;
            break;
        }

        case 1:
            /* Relative */
            pDataDst->timeMode = TX_TIME_RELATIVE;
            pDataDst->timeCount = SYS_TIME_USToCount(txTimeUS);
            break;

        case 2:
            /* Instantaneous */
            pDataDst->timeMode = TX_TIME_RELATIVE;
            pDataDst->timeCount = 0;
            break;

        case 3:
            /* Cancel */
        default:
        {
            DRV_RF215_TX_HANDLE txHandle = DRV_RF215_TX_HANDLE_INVALID;
            txCancel = true;
            for (uint8_t txBufIdx = 0; txBufIdx < DRV_RF215_TX_BUFFERS_NUMBER; txBufIdx++)
            {
                SRV_RSERIAL_TX_HANDLE* txIdHandle = &srvRserialTxHandles[txBufIdx];
                if ((txIdHandle->inUse == true) && (txIdHandle->txId == srvRserialLastTxId))
                {
                    txHandle = txIdHandle->txHandle;
                    txIdHandle->inUse = false;
                }
            }

            *pTxHandleCancel = txHandle;
            break;
        }
    }

    return txCancel;
}

void SRV_RSERIAL_SetTxHandle(DRV_RF215_TX_HANDLE txHandle)
{
    for (uint8_t txBufIdx = 0; txBufIdx < DRV_RF215_TX_BUFFERS_NUMBER; txBufIdx++)
    {
        SRV_RSERIAL_TX_HANDLE* txIdHandle = &srvRserialTxHandles[txBufIdx];
        if (txIdHandle->inUse == false)
        {
            txIdHandle->txHandle = txHandle;
            txIdHandle->txId = srvRserialLastTxId;
            txIdHandle->inUse = true;
        }
    }
}

uint8_t* SRV_RSERIAL_SerialRxMessage (
    DRV_RF215_RX_INDICATION_OBJ* pIndObj,
    DRV_RF215_TRX_ID trxId,
    size_t* pMsgLen
)
{
    uint8_t* pData;
    uint32_t timeIniUS, ppduDurationUS;
    uint16_t psduLen;

    /* Pointer to destination buffer */
    pData = srvRserialBuffer;

    /* Insert command */
    *pData++ = SRV_RSERIAL_CMD_PHY_RECEIVE_MSG;

    /* Insert TRX identifier (Sub-1GHz, 2.4GHz) */
    *pData++ = (uint8_t) trxId;

    /* Insert RX indication parameters */
    timeIniUS = _SRV_RSERIAL_SysTimeToUS(pIndObj->timeIniCount);
    *pData++ = (uint8_t) (timeIniUS >> 24);
    *pData++ = (uint8_t) (timeIniUS >> 16);
    *pData++ = (uint8_t) (timeIniUS >> 8);
    *pData++ = (uint8_t) (timeIniUS);
    ppduDurationUS = SYS_TIME_CountToUS(pIndObj->ppduDurationCount);
    *pData++ = (uint8_t) (ppduDurationUS >> 24);
    *pData++ = (uint8_t) (ppduDurationUS >> 16);
    *pData++ = (uint8_t) (ppduDurationUS >> 8);
    *pData++ = (uint8_t) (ppduDurationUS);
    psduLen = pIndObj->psduLen;
    *pData++ = (uint8_t) (psduLen >> 8);
    *pData++ = (uint8_t) (psduLen);
    *pData++ = (uint8_t) pIndObj->modScheme;
    *pData++ = (uint8_t) pIndObj->rssiDBm;
    *pData++ = (uint8_t) pIndObj->fcsOk;

    /* Insert RX data */
    memcpy(pData, pIndObj->psdu, psduLen);

    *pMsgLen = (size_t) psduLen + RSERIAL_RX_MSG_HEADER_SIZE;
    return srvRserialBuffer;
}

uint8_t* SRV_RSERIAL_SerialCfmMessage (
    DRV_RF215_TX_CONFIRM_OBJ* pCfmObj,
    DRV_RF215_TRX_ID trxId,
    DRV_RF215_TX_HANDLE txHandle,
    size_t* pMsgLen
)
{
    uint8_t* pData;
    uint32_t timeIniUS, ppduDurationUS;
    uint8_t txId = 0;

    /* Pointer to destination buffer */
    pData = srvRserialBuffer;

    /* Insert command */
    *pData++ = SRV_RSERIAL_CMD_PHY_SEND_MSG_RSP;

    /* Insert TRX identifier (Sub-1GHz, 2.4GHz) */
    *pData++ = (uint8_t) trxId;

    /* Insert TX confirm parameters */
    timeIniUS = _SRV_RSERIAL_SysTimeToUS(pCfmObj->timeIniCount);
    *pData++ = (uint8_t) (timeIniUS >> 24);
    *pData++ = (uint8_t) (timeIniUS >> 16);
    *pData++ = (uint8_t) (timeIniUS >> 8);
    *pData++ = (uint8_t) (timeIniUS);
    ppduDurationUS = SYS_TIME_CountToUS(pCfmObj->ppduDurationCount);
    *pData++ = (uint8_t) (ppduDurationUS >> 24);
    *pData++ = (uint8_t) (ppduDurationUS >> 16);
    *pData++ = (uint8_t) (ppduDurationUS >> 8);
    *pData++ = (uint8_t) (ppduDurationUS);
    for (uint8_t txBufIdx = 0; txBufIdx < DRV_RF215_TX_BUFFERS_NUMBER; txBufIdx++)
    {
        SRV_RSERIAL_TX_HANDLE* txIdHandle = &srvRserialTxHandles[txBufIdx];
        if ((txIdHandle->inUse == true) && (txIdHandle->txHandle == txHandle))
        {
            txId = txIdHandle->txId;
            txIdHandle->inUse = false;
        }
    }
    *pData++ = txId;
    *pData++ = (uint8_t) pCfmObj->txResult;

    *pMsgLen = (size_t) (pData - srvRserialBuffer);
    return srvRserialBuffer;
}
