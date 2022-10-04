/******************************************************************************
  RF PHY Sniffer Serialization Implementation.

  Company:
    Microchip Technology Inc.

  File Name:
    srv_rsniffer.c

  Summary:
    Source code for the RF PHY sniffer serialization implementation.

  Description:
    The RF PHY sniffer serialization provides a service to format messages
    through serial connection in order to communicate with Hybrid Sniffer Tool
    provided by Microchip. This file contains the source code for the
    implementation of the RF PHY sniffer serialization.

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
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <string.h>
#include "srv_rsniffer.h"
#include "configuration.h"
#include "system/time/sys_time.h"

// *****************************************************************************
// *****************************************************************************
// Section: Macro definitions
// *****************************************************************************
// *****************************************************************************

/* G3 sniffer identifiers and version */
#define RSNIFFER_PHY_CMD_RECEIVE_MSG                  0x00
#define RSNIFFER_VERSION                              0x02
#define RSNIFFER_RF215_G3                             0x14
#define RSNIFFER_RF215_G3_EXTENDED                    0x34
#define RSNIFFER_RF215_G3_SIMULATOR                   0xD4

/* G3 sniffer modulation types (PLC modulation types + RF PHY types) */
#define RSNIFFER_MOD_TYPE_PLC_BPSK                    0x00
#define RSNIFFER_MOD_TYPE_PLC_QPSK                    0x01
#define RSNIFFER_MOD_TYPE_PLC_8PSK                    0x02
#define RSNIFFER_MOD_TYPE_PLC_16QAM                   0x03
#define RSNIFFER_MOD_TYPE_PLC_BPSK_ROBO               0x04
#define RSNIFFER_MOD_TYPE_RF_FSK50                    0x05
#define RSNIFFER_MOD_TYPE_RF_FSK100                   0x06
#define RSNIFFER_MOD_TYPE_RF_FSK150                   0x07
#define RSNIFFER_MOD_TYPE_RF_FSK200                   0x08
#define RSNIFFER_MOD_TYPE_RF_FSK300                   0x09
#define RSNIFFER_MOD_TYPE_RF_FSK400                   0x0A
#define RSNIFFER_MOD_TYPE_RF_4FSK50                   0x0B
#define RSNIFFER_MOD_TYPE_RF_4FSK100                  0x0C
#define RSNIFFER_MOD_TYPE_RF_4FSK150                  0x0D
#define RSNIFFER_MOD_TYPE_RF_4FSK200                  0x0E
#define RSNIFFER_MOD_TYPE_RF_4FSK300                  0x0F
#define RSNIFFER_MOD_TYPE_RF_4FSK400                  0x10
#define RSNIFFER_MOD_TYPE_RF_OFDM1                    0x11
#define RSNIFFER_MOD_TYPE_RF_OFDM2                    0x12
#define RSNIFFER_MOD_TYPE_RF_OFDM3                    0x13
#define RSNIFFER_MOD_TYPE_RF_OFDM4                    0x14

/* G3 sniffer modulation schemes (PLC modulation schemes + RF modulations) */
#define RSNIFFER_MOD_SCHEME_PLC_DIF                   0x00
#define RSNIFFER_MOD_SCHEME_PLC_COH                   0x01
#define RSNIFFER_MOD_SCHEME_RF_FSK_FEC_OFF            0x02
#define RSNIFFER_MOD_SCHEME_RF_FSK_FEC_ON             0x03
#define RSNIFFER_MOD_SCHEME_RF_OFDM_MCS0              0x04
#define RSNIFFER_MOD_SCHEME_RF_OFDM_MCS1              0x05
#define RSNIFFER_MOD_SCHEME_RF_OFDM_MCS2              0x06
#define RSNIFFER_MOD_SCHEME_RF_OFDM_MCS3              0x07
#define RSNIFFER_MOD_SCHEME_RF_OFDM_MCS4              0x08
#define RSNIFFER_MOD_SCHEME_RF_OFDM_MCS5              0x09
#define RSNIFFER_MOD_SCHEME_RF_OFDM_MCS6              0x0A

/* Buffer sizes */
#define RSNIFFER_MSG_HEADER_SIZE                      25
#define RSNIFFER_MSG_MAX_SIZE                         (DRV_RF215_MAX_PSDU_LEN + RSNIFFER_MSG_HEADER_SIZE)

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Data
// *****************************************************************************
// *****************************************************************************

static uint64_t srvRsnifferPrevSysTime;
static uint32_t srvRsnifferPrevTimeUS;
static uint8_t srvRsnifferRxMsg[RSNIFFER_MSG_MAX_SIZE];
static uint8_t srvRsnifferTxMsg[DRV_RF215_TX_BUFFERS_NUMBER][RSNIFFER_MSG_MAX_SIZE];

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Functions
// *****************************************************************************
// *****************************************************************************

static uint8_t _SRV_RSNIFFER_ModType(DRV_RF215_PHY_CFG_OBJ* pPhyCfgObj)
{
    uint8_t modType;

    modType = (uint8_t) pPhyCfgObj->phyTypeCfg.fsk.symRate;
    if (pPhyCfgObj->phyTypeCfg.fsk.modOrd == FSK_MOD_ORD_2FSK)
    {
        modType += RSNIFFER_MOD_TYPE_RF_FSK50;
    }
    else /* FSK_MOD_ORD_4FSK */
    {
        modType += RSNIFFER_MOD_TYPE_RF_4FSK50;
    }

    return modType;
}

static uint32_t _SRV_RSNIFFER_SysTimeToUS(uint64_t sysTime)
{
    uint64_t sysTimeDiff;
    uint32_t timeUS = srvRsnifferPrevTimeUS;

    /* Difference between current and previous system time */
    sysTimeDiff = sysTime - srvRsnifferPrevSysTime;

    /* Convert system time to microseconds and add to previous time */
    while (sysTimeDiff > 0x10000000)
    {
        timeUS += SYS_TIME_CountToUS(0x10000000);
    }
    timeUS += SYS_TIME_CountToUS((uint32_t) sysTimeDiff);

    /* Store times for next computation */
    srvRsnifferPrevSysTime = sysTime;
    srvRsnifferPrevTimeUS = timeUS;

    return timeUS;
}

// *****************************************************************************
// *****************************************************************************
// Section: RF PHY Serialization Interface Implementation
// *****************************************************************************
// *****************************************************************************

uint8_t* SRV_RSNIFFER_SerialRxMessage (
    DRV_RF215_RX_INDICATION_OBJ* pIndObj,
    DRV_RF215_PHY_CFG_OBJ* pPhyCfgObj,
    uint16_t paySymbols,
    size_t* msgLen
)
{
    uint32_t timeIni, timeEnd;
    uint16_t psduLen;
    int16_t rssi;

    /* Receive message command */
    srvRsnifferRxMsg[0] = RSNIFFER_PHY_CMD_RECEIVE_MSG;

    /* Sniffer version and sniffer type */
    srvRsnifferRxMsg[1] = RSNIFFER_VERSION;
    srvRsnifferRxMsg[2] = RSNIFFER_RF215_G3;

    /* Modulation scheme */
    srvRsnifferRxMsg[3] = (uint8_t) pIndObj->modScheme + RSNIFFER_MOD_SCHEME_RF_FSK_FEC_OFF;

    /* Modulation type depending on RF PHY configuration */
    srvRsnifferRxMsg[4] = _SRV_RSNIFFER_ModType(pPhyCfgObj);

    /* Correct FCS flag */
    srvRsnifferRxMsg[5] = (uint8_t) pIndObj->fcsOk;

    /* Number of payload symbols */
    srvRsnifferRxMsg[7] = (uint8_t) (paySymbols >> 8);
    srvRsnifferRxMsg[8] = (uint8_t) (paySymbols);

    /* Initial and end time of RX frame */
    timeIni = _SRV_RSNIFFER_SysTimeToUS(pIndObj->timeIni);
    srvRsnifferRxMsg[11] = (uint8_t) (timeIni >> 24);
    srvRsnifferRxMsg[12] = (uint8_t) (timeIni >> 16);
    srvRsnifferRxMsg[13] = (uint8_t) (timeIni >> 8);
    srvRsnifferRxMsg[14] = (uint8_t) (timeIni);
    timeEnd = timeIni + pIndObj->ppduDurationUS;
    srvRsnifferRxMsg[15] = (uint8_t) (timeEnd >> 24);
    srvRsnifferRxMsg[16] = (uint8_t) (timeEnd >> 16);
    srvRsnifferRxMsg[17] = (uint8_t) (timeEnd >> 8);
    srvRsnifferRxMsg[18] = (uint8_t) (timeEnd);

    /* RSSI */
    rssi = (int16_t) pIndObj->rssiDBm;
    srvRsnifferRxMsg[19] = (uint8_t) (rssi >> 8);
    srvRsnifferRxMsg[20] = (uint8_t) (rssi);

    /* Data PSDU length (including G3-RF FCS) */
    psduLen = pIndObj->psduLen;
    srvRsnifferRxMsg[23] = (uint8_t) (psduLen >> 8);
    srvRsnifferRxMsg[24] = (uint8_t) (psduLen);

    /* Copy PSDU */
    memcpy(srvRsnifferRxMsg + RSNIFFER_MSG_HEADER_SIZE, pIndObj->psdu, psduLen);

    *msgLen = psduLen + RSNIFFER_MSG_HEADER_SIZE;
    return srvRsnifferRxMsg;
}

void SRV_RSNIFFER_SetTxMessage (
    DRV_RF215_TX_REQUEST_OBJ* pReqObj,
    DRV_RF215_TX_HANDLE txHandle
)
{
    uint8_t* pMsgDest;
    uint16_t psduLen;
    int16_t rssi;
    uint8_t txBufIndex;

    /* Get buffer index from TX handle */
    txBufIndex = (uint8_t) (txHandle & 0xFF);
    pMsgDest = srvRsnifferTxMsg[txBufIndex];

    /* Modulation scheme */
    pMsgDest[3] = (uint8_t) pReqObj->modScheme + RSNIFFER_MOD_SCHEME_RF_FSK_FEC_OFF;

    /* RSSI */
    rssi = 14 - (int16_t) pReqObj->txPwrAtt;
    pMsgDest[19] = (uint8_t) (rssi >> 8);
    pMsgDest[20] = (uint8_t) (rssi);

    /* Data PSDU length (including G3-RF FCS) */
    psduLen = pReqObj->psduLen;
    pMsgDest[23] = (uint8_t) (psduLen >> 8);
    pMsgDest[24] = (uint8_t) (psduLen);

    /* Copy PHY data message (without FCS) */
    memcpy(pMsgDest + RSNIFFER_MSG_HEADER_SIZE, pReqObj->psdu, psduLen);

    /* FCS set to 0 if automatically computed */
    memset(pMsgDest + RSNIFFER_MSG_HEADER_SIZE + psduLen, 0, DRV_RF215_FCS_LEN);
}

uint8_t* SRV_RSNIFFER_SerialCfmMessage (
    DRV_RF215_TX_CONFIRM_OBJ* pCfmObj,
    DRV_RF215_TX_HANDLE txHandle,
    DRV_RF215_PHY_CFG_OBJ* pPhyCfgObj,
    uint16_t paySymbols,
    size_t* msgLen
)
{
    uint8_t* pMsgDest;
    uint32_t timeIni, timeEnd;
    uint16_t psduLen;
    uint8_t txBufIndex;

    if (pCfmObj->txResult != RF215_TX_SUCCESS)
    {
        /* Error in transmission: No report */
        *msgLen = 0;
        return NULL;
    }

    /* Get buffer index from TX handle */
    txBufIndex = (uint8_t) (txHandle & 0xFF);
    pMsgDest = srvRsnifferTxMsg[txBufIndex];

    /* Receive message command */
    pMsgDest[0] = RSNIFFER_PHY_CMD_RECEIVE_MSG;

    /* Sniffer version and sniffer type */
    pMsgDest[1] = RSNIFFER_VERSION;
    pMsgDest[2] = RSNIFFER_RF215_G3;

    /* Modulation type depending on RF PHY configuration */
    pMsgDest[4] = _SRV_RSNIFFER_ModType(pPhyCfgObj);

    /* Correct FCS flag */
    pMsgDest[5] = (uint8_t) true;

    /* Number of payload symbols */
    pMsgDest[7] = (uint8_t) (paySymbols >> 8);
    pMsgDest[8] = (uint8_t) (paySymbols);

    /* Initial and end time of RX frame */
    timeIni = _SRV_RSNIFFER_SysTimeToUS(pCfmObj->timeIni);
    pMsgDest[11] = (uint8_t) (timeIni >> 24);
    pMsgDest[12] = (uint8_t) (timeIni >> 16);
    pMsgDest[13] = (uint8_t) (timeIni >> 8);
    pMsgDest[14] = (uint8_t) (timeIni);
    timeEnd = timeIni + pCfmObj->ppduDurationUS;
    pMsgDest[15] = (uint8_t) (timeEnd >> 24);
    pMsgDest[16] = (uint8_t) (timeEnd >> 16);
    pMsgDest[17] = (uint8_t) (timeEnd >> 8);
    pMsgDest[18] = (uint8_t) (timeEnd);

    /* Data PSDU length (including G3-RF FCS) */
    psduLen = (uint16_t) (pMsgDest[23] << 8) + pMsgDest[24];

    *msgLen = psduLen + RSNIFFER_MSG_HEADER_SIZE;
    return pMsgDest;
}
