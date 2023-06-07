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
#include "stack_config.h"

// *****************************************************************************
// *****************************************************************************
// Section: Macro definitions
// *****************************************************************************
// *****************************************************************************

/* G3 sniffer identifiers and version */
#define RSNIFFER_PHY_CMD_RECEIVE_MSG                  0x00
#define RSNIFFER_VERSION                              0x02
#define RSNIFFER_RF215_G3                             0x14

/* G3 sniffer modulation types (PLC modulation types + RF PHY types) */
#define RSNIFFER_MOD_TYPE_RF_OQPSK                    0x80

/* G3 sniffer modulation schemes (PLC modulation schemes + RF modulations) */
#define RSNIFFER_MOD_SCHEME_RF_OQPSK                  0x80

/* Buffer sizes */
#define RSNIFFER_MSG_HEADER_SIZE                      25
#define RSNIFFER_MSG_MAX_SIZE                         (LARGE_BUFFER_SIZE + RSNIFFER_MSG_HEADER_SIZE)

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Data
// *****************************************************************************
// *****************************************************************************

static uint64_t srvRsnifferPrevSysTime = 0;
static uint32_t srvRsnifferPrevTimeUS = 0;
static uint8_t srvRsnifferRxMsg[RSNIFFER_MSG_MAX_SIZE];
static uint8_t srvRsnifferTxMsg[RSNIFFER_MSG_MAX_SIZE];

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Functions
// *****************************************************************************
// *****************************************************************************
static uint32_t _SRV_RSNIFFER_SysTimeToUS(uint64_t sysTime)
{
    uint64_t sysTimeDiff;
    uint32_t sysTimeDiffNumHigh, sysTimeDiffRemaining;
    uint32_t timeUS = srvRsnifferPrevTimeUS;

    /* Difference between current and previous system time */
    sysTimeDiff = sysTime - srvRsnifferPrevSysTime;
    sysTimeDiffNumHigh = (uint32_t) (sysTimeDiff / 0x10000000);
    sysTimeDiffRemaining = (uint32_t) (sysTimeDiff % 0x10000000);

    /* Convert system time to microseconds and add to previous time */
    timeUS += (SYS_TIME_CountToUS(0x10000000) * sysTimeDiffNumHigh);
    timeUS += SYS_TIME_CountToUS(sysTimeDiffRemaining);

    /* Store times for next computation */
    srvRsnifferPrevSysTime = sysTime;
    srvRsnifferPrevTimeUS = timeUS;

    return timeUS;
}

// *****************************************************************************
// *****************************************************************************
// Section: RF PHY Sniffer Serialization Interface Implementation
// *****************************************************************************
// *****************************************************************************

uint8_t* SRV_RSNIFFER_SerialRxMessage ( SRV_RSNIFFER_PHY_DATA *rxData, size_t *pMsgLen )
{
    uint32_t timeIni, timeEnd;

    /* Receive message command */
    srvRsnifferRxMsg[0] = RSNIFFER_PHY_CMD_RECEIVE_MSG;

    /* Sniffer version and sniffer type */
    srvRsnifferRxMsg[1] = RSNIFFER_VERSION;
    srvRsnifferRxMsg[2] = RSNIFFER_RF215_G3;

    /* Modulation scheme */
    srvRsnifferRxMsg[3] = RSNIFFER_MOD_SCHEME_RF_OQPSK;

    /* Modulation type depending on RF PHY configuration */
    srvRsnifferRxMsg[4] = RSNIFFER_MOD_TYPE_RF_OQPSK;

    /* Correct FCS flag */
    srvRsnifferRxMsg[5] = (uint8_t) true;

    /* Number of payload symbols */
    srvRsnifferRxMsg[7] = (uint8_t) (rxData->paySymbols >> 8);
    srvRsnifferRxMsg[8] = (uint8_t) (rxData->paySymbols);

    /* Initial and end time of RX frame */
    timeIni = _SRV_RSNIFFER_SysTimeToUS(rxData->timeIniCount);
    srvRsnifferRxMsg[11] = (uint8_t) (timeIni >> 24);
    srvRsnifferRxMsg[12] = (uint8_t) (timeIni >> 16);
    srvRsnifferRxMsg[13] = (uint8_t) (timeIni >> 8);
    srvRsnifferRxMsg[14] = (uint8_t) (timeIni);
    timeEnd = timeIni + SYS_TIME_CountToUS(rxData->durationCount);
    srvRsnifferRxMsg[15] = (uint8_t) (timeEnd >> 24);
    srvRsnifferRxMsg[16] = (uint8_t) (timeEnd >> 16);
    srvRsnifferRxMsg[17] = (uint8_t) (timeEnd >> 8);
    srvRsnifferRxMsg[18] = (uint8_t) (timeEnd);

    /* RSSI */
    srvRsnifferRxMsg[19] = (uint8_t) (rxData->rssi >> 8);
    srvRsnifferRxMsg[20] = (uint8_t) (rxData->rssi);

    /* Data payload length */
    srvRsnifferRxMsg[23] = (uint8_t) (rxData->payloadLen >> 8);
    srvRsnifferRxMsg[24] = (uint8_t) (rxData->payloadLen);

    /* Copy payload */
    memcpy(srvRsnifferRxMsg + RSNIFFER_MSG_HEADER_SIZE, rxData->pData, rxData->payloadLen);

    *pMsgLen = (size_t) rxData->payloadLen + RSNIFFER_MSG_HEADER_SIZE;
    return srvRsnifferRxMsg;
}

uint8_t* SRV_RSNIFFER_SerialCfmMessage ( SRV_RSNIFFER_PHY_DATA *txData, size_t *pMsgLen )
{
    uint32_t timeIni, timeEnd;

    /* Receive message command */
    srvRsnifferTxMsg[0] = RSNIFFER_PHY_CMD_RECEIVE_MSG;

    /* Sniffer version and sniffer type */
    srvRsnifferTxMsg[1] = RSNIFFER_VERSION;
    srvRsnifferTxMsg[2] = RSNIFFER_RF215_G3;

    /* Modulation scheme */
    srvRsnifferRxMsg[3] = RSNIFFER_MOD_SCHEME_RF_OQPSK;

    /* Modulation type depending on RF PHY configuration */
    srvRsnifferTxMsg[4] = RSNIFFER_MOD_TYPE_RF_OQPSK;

    /* Correct FCS flag */
    srvRsnifferTxMsg[5] = (uint8_t) true;

    /* Number of payload symbols */
    srvRsnifferTxMsg[7] = (uint8_t) (txData->paySymbols >> 8);
    srvRsnifferTxMsg[8] = (uint8_t) (txData->paySymbols);

    /* Initial and end time of RX frame */
    timeIni = _SRV_RSNIFFER_SysTimeToUS(txData->timeIniCount);
    srvRsnifferTxMsg[11] = (uint8_t) (timeIni >> 24);
    srvRsnifferTxMsg[12] = (uint8_t) (timeIni >> 16);
    srvRsnifferTxMsg[13] = (uint8_t) (timeIni >> 8);
    srvRsnifferTxMsg[14] = (uint8_t) (timeIni);
    timeEnd = timeIni + SYS_TIME_CountToUS(txData->durationCount);
    srvRsnifferTxMsg[15] = (uint8_t) (timeEnd >> 24);
    srvRsnifferTxMsg[16] = (uint8_t) (timeEnd >> 16);
    srvRsnifferTxMsg[17] = (uint8_t) (timeEnd >> 8);
    srvRsnifferTxMsg[18] = (uint8_t) (timeEnd);

    /* Data payload length */
    srvRsnifferTxMsg[23] = (uint8_t) (txData->payloadLen >> 8);
    srvRsnifferTxMsg[24] = (uint8_t) (txData->payloadLen);

    /* Copy payload */
    memcpy(srvRsnifferTxMsg + RSNIFFER_MSG_HEADER_SIZE, txData->pData, txData->payloadLen);

    *pMsgLen = (size_t) txData->payloadLen + RSNIFFER_MSG_HEADER_SIZE;
    return srvRsnifferTxMsg;
}