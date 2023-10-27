/*******************************************************************************
  G3 MAC Wrapper Source File

  Company:
    Microchip Technology Inc.

  File Name:
    mac_wrapper.c

  Summary:
    G3 MAC Wrapper API Source File

  Description:
    This file contains implementation of the API
    to be used by upper layers when accessing G3 MAC layers.
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
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "system/system.h"
#include "configuration.h"
#include "mac_wrapper.h"
#include "mac_wrapper_defs.h"
#include "../mac_common/mac_common.h"
#include "../mac_plc/mac_plc.h"
#include "service/log_report/srv_log_report.h"
#include "service/usi/srv_usi.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

#pragma pack(push,2)

typedef struct
{
    /* State of the MAC Wrapper module */
    MAC_WRP_STATE state;
    /* Callbacks */
    MAC_WRP_HANDLERS macWrpHandlers;
    /* Mac Wrapper instance handle */
    MAC_WRP_HANDLE macWrpHandle;
    /* Time of next task in milliseconds */
    uint32_t nextTaskTimeMs;
    /* PIB serialization debug set length */
    uint8_t debugSetLength;
    /* Mac Serialization handle */
    MAC_WRP_HANDLE macSerialHandle;
    /* USI handle for MAC serialization */
    SRV_USI_HANDLE usiHandle;
    /* Flag to indicate initialize through serial interface */
    bool serialInitialize;
    /* Flag to indicate reset request through serial interface */
    bool serialResetRequest;
    /* Flag to indicate start request through serial interface */
    bool serialStartRequest;
    /* Flag to indicate scan request through serial interface */
    bool serialScanRequest;
    /* Flag to indicate scan request in progress */
    bool scanRequestInProgress;
} MAC_WRP_DATA;

typedef struct
{
    MAC_DATA_REQUEST_PARAMS dataReqParams;
    MAC_WRP_MEDIA_TYPE_REQUEST dataReqMediaType;
    bool serialDataRequest;
    bool used;
} MAC_WRP_DATA_REQ_ENTRY;

/* Data Request Queue size */
#define MAC_WRP_DATA_REQ_QUEUE_SIZE   2U

typedef enum
{
    MAC_WRP_SERIAL_STATUS_SUCCESS = 0,
    MAC_WRP_SERIAL_STATUS_NOT_ALLOWED,
    MAC_WRP_SERIAL_STATUS_UNKNOWN_COMMAND,
    MAC_WRP_SERIAL_STATUS_INVALID_PARAMETER
} MAC_WRP_SERIAL_STATUS;

typedef enum
{
    /* Generic messages */
    MAC_WRP_SERIAL_MSG_STATUS = 0,

    /* MAC access request messages */
    MAC_WRP_SERIAL_MSG_MAC_INITIALIZE = 50,
    MAC_WRP_SERIAL_MSG_MAC_DATA_REQUEST,
    MAC_WRP_SERIAL_MSG_MAC_GET_REQUEST,
    MAC_WRP_SERIAL_MSG_MAC_SET_REQUEST,
    MAC_WRP_SERIAL_MSG_MAC_RESET_REQUEST,
    MAC_WRP_SERIAL_MSG_MAC_SCAN_REQUEST,
    MAC_WRP_SERIAL_MSG_MAC_START_REQUEST,

    /* MAC response/indication messages */
    MAC_WRP_SERIAL_MSG_MAC_DATA_CONFIRM = 60,
    MAC_WRP_SERIAL_MSG_MAC_DATA_INDICATION,
    MAC_WRP_SERIAL_MSG_MAC_GET_CONFIRM,
    MAC_WRP_SERIAL_MSG_MAC_SET_CONFIRM,
    MAC_WRP_SERIAL_MSG_MAC_RESET_CONFIRM,
    MAC_WRP_SERIAL_MSG_MAC_SCAN_CONFIRM,
    MAC_WRP_SERIAL_MSG_MAC_BEACON_NOTIFY,
    MAC_WRP_SERIAL_MSG_MAC_START_CONFIRM,
    MAC_WRP_SERIAL_MSG_MAC_COMM_STATUS_INDICATION,
    MAC_WRP_SERIAL_MSG_MAC_SNIFFER_INDICATION

} MAC_WRP_SERIAL_MSG_ID;


#pragma pack(pop)

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Variables
// *****************************************************************************
// *****************************************************************************

// This is the module data object
static MAC_WRP_DATA macWrpData;

// Data Service Control
static MAC_WRP_DATA_REQ_ENTRY dataReqQueue[MAC_WRP_DATA_REQ_QUEUE_SIZE];

#define MAC_MAX_DEVICE_TABLE_ENTRIES_PLC    128

static MAC_PLC_TABLES macPlcTables;
static MAC_DEVICE_TABLE_ENTRY macPlcDeviceTable[MAC_MAX_DEVICE_TABLE_ENTRIES_PLC];

static uint8_t serialRspBuffer[512];

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Functions
// *****************************************************************************
// *****************************************************************************

static MAC_WRP_DATA_REQ_ENTRY *lMAC_WRP_GetFreeDataReqEntry(void)
{
    uint8_t index;
    MAC_WRP_DATA_REQ_ENTRY *found = NULL;

    for (index = 0U; index < MAC_WRP_DATA_REQ_QUEUE_SIZE; index++)
    {
        if (dataReqQueue[index].used == false)
        {
            found = &dataReqQueue[index];
            dataReqQueue[index].used = true;
            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "lMAC_WRP_GetFreeDataReqEntry() Found free data request entry on index %u\r\n", index);
            break;
        }
    }

    return found;
}

static MAC_WRP_DATA_REQ_ENTRY *lMAC_WRP_GetDataReqEntryByHandle(uint8_t handle)
{
    uint8_t index;
    MAC_WRP_DATA_REQ_ENTRY *found = NULL;

    for (index = 0U; index < MAC_WRP_DATA_REQ_QUEUE_SIZE; index++)
    {
        if ((dataReqQueue[index].used == true) &&
            (dataReqQueue[index].dataReqParams.msduHandle == handle))
        {
            found = &dataReqQueue[index];
            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "lMAC_WRP_GetDataReqEntryByHandle() Found matching data request entry on index %u, Handle: 0x%02X\r\n", index, handle);
            break;
        }
    }

    return found;
}

static bool lMAC_WRP_IsSharedAttribute(MAC_WRP_PIB_ATTRIBUTE attribute)
{
    /* Check if attribute in the list of shared between MAC layers */
    if ((attribute == MAC_WRP_PIB_MANUF_EXTENDED_ADDRESS) ||
        (attribute == MAC_WRP_PIB_PAN_ID) ||
        (attribute == MAC_WRP_PIB_PROMISCUOUS_MODE) ||
        (attribute == MAC_WRP_PIB_SHORT_ADDRESS) ||
        (attribute == MAC_WRP_PIB_POS_TABLE_ENTRY_TTL) ||
        (attribute == MAC_WRP_PIB_POS_RECENT_ENTRY_THRESHOLD) ||
        (attribute == MAC_WRP_PIB_RC_COORD) ||
        (attribute == MAC_WRP_PIB_KEY_TABLE))
    {
        /* Shared IB */
        return true;
    }
    else
    {
        /* Non-shared IB */
        return false;
    }
}

static void lMemcpyToUsiEndianessUint32(uint8_t* pDst, uint8_t* pSrc)
{
    uint32_t aux;

    (void) memcpy((uint8_t *) &aux, pSrc, 4);

    *pDst++ = (uint8_t) (aux >> 24);
    *pDst++ = (uint8_t) (aux >> 16);
    *pDst++ = (uint8_t) (aux >> 8);
    *pDst = (uint8_t) aux;
}

static void lMemcpyToUsiEndianessUint16(uint8_t* pDst, uint8_t* pSrc)
{
    uint16_t aux;

    (void) memcpy((uint8_t *) &aux, pSrc, 2);

    *pDst++ = (uint8_t) (aux >> 8);
    *pDst = (uint8_t) aux;
}

static void lMemcpyFromUsiEndianessUint32(uint8_t* pDst, uint8_t* pSrc)
{
    uint32_t aux;

    aux = ((uint32_t) *pSrc++) << 24;
    aux += ((uint32_t) *pSrc++) << 16;
    aux += ((uint32_t) *pSrc++) << 8;
    aux += (uint32_t) *pSrc;

    (void) memcpy(pDst, (uint8_t *) &aux, 4);
}

static void lMemcpyFromUsiEndianessUint16(uint8_t* pDst, uint8_t* pSrc)
{
    uint16_t aux;

    aux = ((uint16_t) *pSrc++) << 8;
    aux += (uint16_t) *pSrc;

    (void) memcpy(pDst, (uint8_t *) &aux, 2);
}

static void lMAC_WRP_StringifyMsgStatus(MAC_WRP_SERIAL_STATUS status, MAC_WRP_SERIAL_MSG_ID command)
{
    uint8_t serialRspLen = 0;

    /* Fill serial response buffer */
    serialRspBuffer[serialRspLen++] = (uint8_t) MAC_WRP_SERIAL_MSG_STATUS;
    serialRspBuffer[serialRspLen++] = (uint8_t) status;
    serialRspBuffer[serialRspLen++] = (uint8_t) command;

    /* Send through USI */
    SRV_USI_Send_Message(macWrpData.usiHandle, SRV_USI_PROT_ID_MAC_G3, serialRspBuffer, serialRspLen);
}

static void lMAC_WRP_StringifyDataConfirm(MAC_WRP_DATA_CONFIRM_PARAMS* dcParams)
{
    uint8_t serialRspLen = 0;

    /* Fill serial response buffer */
    serialRspBuffer[serialRspLen++] = (uint8_t) MAC_WRP_SERIAL_MSG_MAC_DATA_CONFIRM;
    serialRspBuffer[serialRspLen++] = dcParams->msduHandle;
    serialRspBuffer[serialRspLen++] = (uint8_t) dcParams->status;
    serialRspBuffer[serialRspLen++] = (uint8_t) (dcParams->timestamp >> 24);
    serialRspBuffer[serialRspLen++] = (uint8_t) (dcParams->timestamp >> 16);
    serialRspBuffer[serialRspLen++] = (uint8_t) (dcParams->timestamp >> 8);
    serialRspBuffer[serialRspLen++] = (uint8_t) dcParams->timestamp;
    serialRspBuffer[serialRspLen++] = (uint8_t) dcParams->mediaType;

    /* Send through USI */
    SRV_USI_Send_Message(macWrpData.usiHandle, SRV_USI_PROT_ID_MAC_G3, serialRspBuffer, serialRspLen);
}

static void lMAC_WRP_StringifyDataIndication(MAC_WRP_DATA_INDICATION_PARAMS* diParams)
{
    uint8_t srcAddrLen, dstAddrLen;
    uint16_t serialRspLen = 0U;

    /* Fill serial response buffer */
    serialRspBuffer[serialRspLen++] = (uint8_t) MAC_WRP_SERIAL_MSG_MAC_DATA_INDICATION;
    serialRspBuffer[serialRspLen++] = (uint8_t) (diParams->srcPanId >> 8);
    serialRspBuffer[serialRspLen++] = (uint8_t) diParams->srcPanId;

    if (diParams->srcAddress.addressMode == MAC_WRP_ADDRESS_MODE_SHORT)
    {
        srcAddrLen = 2U;
        serialRspBuffer[serialRspLen++] = srcAddrLen;
        serialRspBuffer[serialRspLen++] = (uint8_t) (diParams->srcAddress.shortAddress >> 8);
        serialRspBuffer[serialRspLen++] = (uint8_t) diParams->srcAddress.shortAddress;
    }
    else if (diParams->srcAddress.addressMode == MAC_WRP_ADDRESS_MODE_EXTENDED)
    {
        srcAddrLen = 8U;
        serialRspBuffer[serialRspLen++] = srcAddrLen;
        (void) memcpy(&serialRspBuffer[serialRspLen], diParams->srcAddress.extendedAddress.address, srcAddrLen);
        serialRspLen += srcAddrLen;
    }
    else
    {
        return; /* This line must never be reached */
    }

    serialRspBuffer[serialRspLen++] = (uint8_t) (diParams->destPanId >> 8);
    serialRspBuffer[serialRspLen++] = (uint8_t) diParams->destPanId;

    if (diParams->destAddress.addressMode == MAC_WRP_ADDRESS_MODE_SHORT)
    {
        dstAddrLen = 2U;
        serialRspBuffer[serialRspLen++] = dstAddrLen;
        serialRspBuffer[serialRspLen++] = (uint8_t) (diParams->destAddress.shortAddress >> 8);
        serialRspBuffer[serialRspLen++] = (uint8_t) diParams->destAddress.shortAddress;
    }
    else if (diParams->destAddress.addressMode == MAC_WRP_ADDRESS_MODE_EXTENDED)
    {
        dstAddrLen = 8U;
        serialRspBuffer[serialRspLen++] = dstAddrLen;
        (void) memcpy(&serialRspBuffer[serialRspLen], diParams->destAddress.extendedAddress.address, dstAddrLen);
        serialRspLen += dstAddrLen;
    }
    else
    {
        return; /* This line must never be reached */
    }

    serialRspBuffer[serialRspLen++] = diParams->linkQuality;
    serialRspBuffer[serialRspLen++] = diParams->dsn;

    serialRspBuffer[serialRspLen++] = (uint8_t) (diParams->timestamp >> 24);
    serialRspBuffer[serialRspLen++] = (uint8_t) (diParams->timestamp >> 16);
    serialRspBuffer[serialRspLen++] = (uint8_t) (diParams->timestamp >> 8);
    serialRspBuffer[serialRspLen++] = (uint8_t) diParams->timestamp;

    serialRspBuffer[serialRspLen++] = (uint8_t) diParams->securityLevel;
    serialRspBuffer[serialRspLen++] = diParams->keyIndex;
    serialRspBuffer[serialRspLen++] = (uint8_t) diParams->qualityOfService;

    serialRspBuffer[serialRspLen++] = diParams->rxModulation;
    serialRspBuffer[serialRspLen++] = diParams->rxModulationScheme;
    (void) memcpy(&serialRspBuffer[serialRspLen], diParams->rxToneMap.toneMap, 3);
    serialRspLen += 3U;
    serialRspBuffer[serialRspLen++] = diParams->computedModulation;
    serialRspBuffer[serialRspLen++] = diParams->computedModulationScheme;
    (void) memcpy(&serialRspBuffer[serialRspLen], diParams->computedToneMap.toneMap, 3);
    serialRspLen += 3U;

    serialRspBuffer[serialRspLen++] = (uint8_t) (diParams->msduLength >> 8);
    serialRspBuffer[serialRspLen++] = (uint8_t) diParams->msduLength;

    (void) memcpy(&serialRspBuffer[serialRspLen], diParams->msdu, diParams->msduLength);
    serialRspLen += diParams->msduLength;

    serialRspBuffer[serialRspLen++] = (uint8_t)diParams->mediaType;

    /* Send through USI */
    SRV_USI_Send_Message(macWrpData.usiHandle, SRV_USI_PROT_ID_MAC_G3, serialRspBuffer, serialRspLen);
}

static void lMAC_WRP_StringifySnifferIndication(MAC_WRP_SNIFFER_INDICATION_PARAMS* siParams)
{
    uint8_t srcAddrLen, dstAddrLen;
    uint16_t serialRspLen = 0U;

    /* Fill serial response buffer */
    serialRspBuffer[serialRspLen++] = (uint8_t) MAC_WRP_SERIAL_MSG_MAC_SNIFFER_INDICATION;
    serialRspBuffer[serialRspLen++] = siParams->frameType;
    serialRspBuffer[serialRspLen++] = (uint8_t) (siParams->srcPanId >> 8);
    serialRspBuffer[serialRspLen++] = (uint8_t) siParams->srcPanId;

    if (siParams->srcAddress.addressMode == MAC_WRP_ADDRESS_MODE_SHORT)
    {
        srcAddrLen = 2U;
        serialRspBuffer[serialRspLen++] = srcAddrLen;
        serialRspBuffer[serialRspLen++] = (uint8_t) (siParams->srcAddress.shortAddress >> 8);
        serialRspBuffer[serialRspLen++] = (uint8_t) siParams->srcAddress.shortAddress;
    }
    else if (siParams->srcAddress.addressMode == MAC_WRP_ADDRESS_MODE_EXTENDED)
    {
        srcAddrLen = 8U;
        serialRspBuffer[serialRspLen++] = srcAddrLen;
        (void) memcpy(&serialRspBuffer[serialRspLen], siParams->srcAddress.extendedAddress.address, srcAddrLen);
        serialRspLen += srcAddrLen;
    }
    else
    {
        srcAddrLen = 0U;
        serialRspBuffer[serialRspLen++] = srcAddrLen;
    }

    serialRspBuffer[serialRspLen++] = (uint8_t) (siParams->destPanId >> 8);
    serialRspBuffer[serialRspLen++] = (uint8_t) siParams->destPanId;

    if (siParams->destAddress.addressMode == MAC_WRP_ADDRESS_MODE_SHORT)
    {
        dstAddrLen = 2U;
        serialRspBuffer[serialRspLen++] = dstAddrLen;
        serialRspBuffer[serialRspLen++] = (uint8_t) (siParams->destAddress.shortAddress >> 8);
        serialRspBuffer[serialRspLen++] = (uint8_t) siParams->destAddress.shortAddress;
    }
    else if (siParams->destAddress.addressMode == MAC_WRP_ADDRESS_MODE_EXTENDED)
    {
        dstAddrLen = 8U;
        serialRspBuffer[serialRspLen++] = dstAddrLen;
        (void) memcpy(&serialRspBuffer[serialRspLen], siParams->destAddress.extendedAddress.address, dstAddrLen);
        serialRspLen += dstAddrLen;
    }
    else
    {
        dstAddrLen = 0U;
        serialRspBuffer[serialRspLen++] = dstAddrLen;
    }

    serialRspBuffer[serialRspLen++] = siParams->linkQuality;
    serialRspBuffer[serialRspLen++] = siParams->dsn;

    serialRspBuffer[serialRspLen++] = (uint8_t) (siParams->timestamp >> 24);
    serialRspBuffer[serialRspLen++] = (uint8_t) (siParams->timestamp >> 16);
    serialRspBuffer[serialRspLen++] = (uint8_t) (siParams->timestamp >> 8);
    serialRspBuffer[serialRspLen++] = (uint8_t) siParams->timestamp;

    serialRspBuffer[serialRspLen++] = (uint8_t) siParams->securityLevel;
    serialRspBuffer[serialRspLen++] = siParams->keyIndex;
    serialRspBuffer[serialRspLen++] = (uint8_t) siParams->qualityOfService;

    serialRspBuffer[serialRspLen++] = siParams->rxModulation;
    serialRspBuffer[serialRspLen++] = siParams->rxModulationScheme;
    (void) memcpy(&serialRspBuffer[serialRspLen], siParams->rxToneMap.toneMap, 3);
    serialRspLen += 3U;
    serialRspBuffer[serialRspLen++] = siParams->computedModulation;
    serialRspBuffer[serialRspLen++] = siParams->computedModulationScheme;
    (void) memcpy(&serialRspBuffer[serialRspLen], siParams->computedToneMap.toneMap, 3);
    serialRspLen += 3U;

    serialRspBuffer[serialRspLen++] = (uint8_t) (siParams->msduLength >> 8);
    serialRspBuffer[serialRspLen++] = (uint8_t) siParams->msduLength;

    (void) memcpy(&serialRspBuffer[serialRspLen], siParams->msdu, siParams->msduLength);
    serialRspLen += siParams->msduLength;

    /* Send through USI */
    SRV_USI_Send_Message(macWrpData.usiHandle, SRV_USI_PROT_ID_MAC_G3, serialRspBuffer, serialRspLen);
}

static void lMAC_WRP_StringifyResetConfirm(MAC_WRP_RESET_CONFIRM_PARAMS* rcParams)
{
    uint8_t serialRspLen = 0;

    /* Fill serial response buffer */
    serialRspBuffer[serialRspLen++] = (uint8_t) MAC_WRP_SERIAL_MSG_MAC_RESET_CONFIRM;
    serialRspBuffer[serialRspLen++] = (uint8_t) rcParams->status;

    /* Send through USI */
    SRV_USI_Send_Message(macWrpData.usiHandle, SRV_USI_PROT_ID_MAC_G3, serialRspBuffer, serialRspLen);
}

static void lMAC_WRP_StringifyBeaconNotIndication(MAC_WRP_BEACON_NOTIFY_INDICATION_PARAMS* bniParams)
{
    uint8_t serialRspLen = 0;

    /* Fill serial response buffer */
    serialRspBuffer[serialRspLen++] = (uint8_t) MAC_WRP_SERIAL_MSG_MAC_BEACON_NOTIFY;
    serialRspBuffer[serialRspLen++] = (uint8_t) (bniParams->panDescriptor.panId >> 8);
    serialRspBuffer[serialRspLen++] = (uint8_t) bniParams->panDescriptor.panId;
    serialRspBuffer[serialRspLen++] = bniParams->panDescriptor.linkQuality;
    serialRspBuffer[serialRspLen++] = (uint8_t) (bniParams->panDescriptor.lbaAddress >> 8);
    serialRspBuffer[serialRspLen++] = (uint8_t) bniParams->panDescriptor.lbaAddress;
    serialRspBuffer[serialRspLen++] = (uint8_t) (bniParams->panDescriptor.rcCoord >> 8);
    serialRspBuffer[serialRspLen++] = (uint8_t) bniParams->panDescriptor.rcCoord;
    serialRspBuffer[serialRspLen++] = (uint8_t) bniParams->panDescriptor.mediaType;

    /* Send through USI */
    SRV_USI_Send_Message(macWrpData.usiHandle, SRV_USI_PROT_ID_MAC_G3, serialRspBuffer, serialRspLen);
}

static void lMAC_WRP_StringifyScanConfirm(MAC_WRP_SCAN_CONFIRM_PARAMS* scParams)
{
    uint8_t serialRspLen = 0;

    /* Fill serial response buffer */
    serialRspBuffer[serialRspLen++] = (uint8_t) MAC_WRP_SERIAL_MSG_MAC_SCAN_CONFIRM;
    serialRspBuffer[serialRspLen++] = (uint8_t) scParams->status;

    /* Send through USI */
    SRV_USI_Send_Message(macWrpData.usiHandle, SRV_USI_PROT_ID_MAC_G3, serialRspBuffer, serialRspLen);
}

static void lMAC_WRP_StringifyStartConfirm(MAC_WRP_START_CONFIRM_PARAMS* scParams)
{
    uint8_t serialRspLen = 0;

    /* Fill serial response buffer */
    serialRspBuffer[serialRspLen++] = (uint8_t) MAC_WRP_SERIAL_MSG_MAC_START_CONFIRM;
    serialRspBuffer[serialRspLen++] = (uint8_t) scParams->status;

    /* Send through USI */
    SRV_USI_Send_Message(macWrpData.usiHandle, SRV_USI_PROT_ID_MAC_G3, serialRspBuffer, serialRspLen);
}

static void lMAC_WRP_StringifyCommStatusIndication(MAC_WRP_COMM_STATUS_INDICATION_PARAMS* csiParams)
{
    uint8_t srcAddrLen, dstAddrLen;
    uint8_t serialRspLen = 0U;

    /* Fill serial response buffer */
    serialRspBuffer[serialRspLen++] = (uint8_t) MAC_WRP_SERIAL_MSG_MAC_COMM_STATUS_INDICATION;
    serialRspBuffer[serialRspLen++] = (uint8_t) (csiParams->panId >> 8);
    serialRspBuffer[serialRspLen++] = (uint8_t) csiParams->panId;

    if (csiParams->srcAddress.addressMode == MAC_WRP_ADDRESS_MODE_SHORT)
    {
        srcAddrLen = 2U;
        serialRspBuffer[serialRspLen++] = srcAddrLen;
        serialRspBuffer[serialRspLen++] = (uint8_t) (csiParams->srcAddress.shortAddress >> 8);
        serialRspBuffer[serialRspLen++] = (uint8_t) csiParams->srcAddress.shortAddress;
    }
    else if (csiParams->srcAddress.addressMode == MAC_WRP_ADDRESS_MODE_EXTENDED)
    {
        srcAddrLen = 8U;
        serialRspBuffer[serialRspLen++] = srcAddrLen;
        (void) memcpy(&serialRspBuffer[serialRspLen], csiParams->srcAddress.extendedAddress.address, srcAddrLen);
        serialRspLen += srcAddrLen;
    }
    else
    {
        return; /* This line must never be reached */
    }

    if (csiParams->destAddress.addressMode == MAC_WRP_ADDRESS_MODE_SHORT)
    {
        dstAddrLen = 2U;
        serialRspBuffer[serialRspLen++] = dstAddrLen;
        serialRspBuffer[serialRspLen++] = (uint8_t) (csiParams->destAddress.shortAddress >> 8);
        serialRspBuffer[serialRspLen++] = (uint8_t) csiParams->destAddress.shortAddress;
    }
    else if (csiParams->destAddress.addressMode == MAC_WRP_ADDRESS_MODE_EXTENDED)
    {
        dstAddrLen = 8U;
        serialRspBuffer[serialRspLen++] = dstAddrLen;
        (void) memcpy(&serialRspBuffer[serialRspLen], csiParams->destAddress.extendedAddress.address, dstAddrLen);
        serialRspLen += dstAddrLen;
    }
    else
    {
        return; /* This line must never be reached */
    }

    serialRspBuffer[serialRspLen++] = (uint8_t) csiParams->status;
    serialRspBuffer[serialRspLen++] = (uint8_t) csiParams->securityLevel;
    serialRspBuffer[serialRspLen++] = csiParams->keyIndex;

    serialRspBuffer[serialRspLen++] = (uint8_t)csiParams->mediaType;

    /* Send through USI */
    SRV_USI_Send_Message(macWrpData.usiHandle, SRV_USI_PROT_ID_MAC_G3, serialRspBuffer, serialRspLen);
}

static MAC_WRP_SERIAL_STATUS lMAC_WRP_ParseInitialize(uint8_t* pData)
{
    if (macWrpData.state == MAC_WRP_STATE_NOT_READY)
    {
        MAC_WRP_BAND band;

        /* Parse initialize message */
        band = (MAC_WRP_BAND) *pData;

        /* Open MAC Wrapper if it has not been opened yet */
        (void) MAC_WRP_Open(G3_MAC_WRP_INDEX_0, band);

        macWrpData.serialInitialize = true;
    }

    return MAC_WRP_SERIAL_STATUS_SUCCESS;
}

static MAC_WRP_SERIAL_STATUS lMAC_WRP_ParseDataRequest(uint8_t* pData)
{
    MAC_WRP_DATA_REQUEST_PARAMS drParams;
    uint8_t srcAddrLen, dstAddrLen;

    if (MAC_WRP_Status() != SYS_STATUS_READY)
    {
        /* MAC Wrapper not initialized */
        return MAC_WRP_SERIAL_STATUS_NOT_ALLOWED;
    }

    /* Parse data request message */
    drParams.msduHandle = *pData++;
    drParams.securityLevel =  (MAC_WRP_SECURITY_LEVEL) *pData++;
    drParams.keyIndex = *pData++;
    drParams.qualityOfService = (MAC_WRP_QUALITY_OF_SERVICE) *pData++;
    drParams.txOptions = *pData++;
    drParams.destPanId = ((MAC_WRP_PAN_ID) *pData++) << 8;
    drParams.destPanId += (MAC_WRP_PAN_ID) *pData++;
    srcAddrLen = *pData++;
    if (srcAddrLen == 2U)
    {
        drParams.srcAddressMode = MAC_WRP_ADDRESS_MODE_SHORT;
    }
    else if (srcAddrLen == 8U)
    {
        drParams.srcAddressMode = MAC_WRP_ADDRESS_MODE_EXTENDED;
    }
    else
    {
        return MAC_WRP_SERIAL_STATUS_INVALID_PARAMETER;
    }

    dstAddrLen = *pData++;
    if (dstAddrLen == 2U)
    {
        drParams.destAddress.addressMode = MAC_WRP_ADDRESS_MODE_SHORT;
        drParams.destAddress.shortAddress = ((MAC_WRP_SHORT_ADDRESS) *pData++) << 8;
        drParams.destAddress.shortAddress += (MAC_WRP_SHORT_ADDRESS) *pData++;
    }
    else if (dstAddrLen == 8U)
    {
        drParams.destAddress.addressMode = MAC_WRP_ADDRESS_MODE_EXTENDED;
        (void) memcpy(&drParams.destAddress.extendedAddress.address, pData, dstAddrLen);
        pData += dstAddrLen;
    }
    else
    {
        return MAC_WRP_SERIAL_STATUS_INVALID_PARAMETER;
    }

    drParams.mediaType = (MAC_WRP_MEDIA_TYPE_REQUEST) *pData++;

    drParams.msduLength = ((uint16_t)*pData++) << 8;
    drParams.msduLength += (uint16_t)*pData++;
    drParams.msdu = pData;

    /* Send data request to MAC */
    MAC_WRP_DataRequest(macWrpData.macSerialHandle, &drParams);

    return MAC_WRP_SERIAL_STATUS_SUCCESS;
}

static MAC_WRP_SERIAL_STATUS lMAC_WRP_ParseGetRequest(uint8_t* pData)
{
    uint32_t attribute;
    uint16_t index;
    MAC_WRP_PIB_VALUE pibValue;
    MAC_WRP_PIB_ATTRIBUTE pibAttr;
    MAC_WRP_STATUS getStatus;
    uint8_t serialRspLen = 0U;

    if (MAC_WRP_Status() != SYS_STATUS_READY)
    {
        /* MAC Wrapper not initialized */
        return MAC_WRP_SERIAL_STATUS_NOT_ALLOWED;
    }

    /* Get PIB from MAC */
    attribute = MAC_WRP_SerialParseGetRequest(pData, &index);
    pibAttr = (MAC_WRP_PIB_ATTRIBUTE) attribute;
    getStatus = MAC_WRP_GetRequestSync(macWrpData.macSerialHandle, pibAttr, index, &pibValue);

    /* Fill serial response buffer */
    serialRspBuffer[serialRspLen++] = (uint8_t) MAC_WRP_SERIAL_MSG_MAC_GET_CONFIRM;
    serialRspLen += MAC_WRP_SerialStringifyGetConfirm(&serialRspBuffer[serialRspLen],
            getStatus, pibAttr, index, pibValue.value, pibValue.length);

    /* Send get confirm through USI */
    SRV_USI_Send_Message(macWrpData.usiHandle, SRV_USI_PROT_ID_MAC_G3, serialRspBuffer, serialRspLen);

    return MAC_WRP_SERIAL_STATUS_SUCCESS;
}

static MAC_WRP_SERIAL_STATUS lMAC_WRP_ParseSetRequest(uint8_t* pData)
{
    uint16_t index;
    MAC_WRP_PIB_VALUE pibValue;
    MAC_WRP_PIB_ATTRIBUTE attribute;
    MAC_WRP_STATUS setStatus;
    uint8_t serialRspLen = 0U;

    if (MAC_WRP_Status() != SYS_STATUS_READY)
    {
        /* MAC Wrapper not initialized */
        return MAC_WRP_SERIAL_STATUS_NOT_ALLOWED;
    }

    /* Set MAC PIB */
    attribute = MAC_WRP_SerialParseSetRequest(pData, &index, &pibValue);
    setStatus = MAC_WRP_SetRequestSync(macWrpData.macSerialHandle, attribute, index, &pibValue);

    /* Fill serial response buffer */
    serialRspBuffer[serialRspLen++] = (uint8_t) MAC_WRP_SERIAL_MSG_MAC_SET_CONFIRM;
    serialRspLen += MAC_WRP_SerialStringifySetConfirm(&serialRspBuffer[serialRspLen],
            setStatus, attribute, index);

    /* Send set confirm through USI */
    SRV_USI_Send_Message(macWrpData.usiHandle, SRV_USI_PROT_ID_MAC_G3, serialRspBuffer, serialRspLen);

    return MAC_WRP_SERIAL_STATUS_SUCCESS;
}

static MAC_WRP_SERIAL_STATUS lMAC_WRP_ParseResetRequest(uint8_t* pData)
{
    MAC_WRP_RESET_REQUEST_PARAMS rrParams;

    if (MAC_WRP_Status() != SYS_STATUS_READY)
    {
        /* MAC Wrapper not initialized */
        return MAC_WRP_SERIAL_STATUS_NOT_ALLOWED;
    }

    /* Parse reset request message */
    rrParams.setDefaultPib = (bool) *pData;

    /* Send reset request to MAC */
    macWrpData.serialResetRequest = true;
    MAC_WRP_ResetRequest(macWrpData.macSerialHandle, &rrParams);

    return MAC_WRP_SERIAL_STATUS_SUCCESS;
}

static MAC_WRP_SERIAL_STATUS lMAC_WRP_ParseScanRequest(uint8_t* pData)
{
    MAC_WRP_SCAN_REQUEST_PARAMS srParams;

    if (MAC_WRP_Status() != SYS_STATUS_READY)
    {
        /* MAC Wrapper not initialized */
        return MAC_WRP_SERIAL_STATUS_NOT_ALLOWED;
    }

    /* Parse scan request message */
    srParams.scanDuration = ((uint16_t) *pData++) << 8;
    srParams.scanDuration += *pData;

    /* Send scan request to MAC */
    MAC_WRP_ScanRequest(macWrpData.macSerialHandle, &srParams);

    return MAC_WRP_SERIAL_STATUS_SUCCESS;
}

static MAC_WRP_SERIAL_STATUS lMAC_WRP_ParseStartRequest(uint8_t* pData)
{
    MAC_WRP_START_REQUEST_PARAMS srParams;

    if (MAC_WRP_Status() != SYS_STATUS_READY)
    {
        /* MAC Wrapper not initialized */
        return MAC_WRP_SERIAL_STATUS_NOT_ALLOWED;
    }

    /* Parse start request message */
    srParams.panId = ((uint16_t) *pData++) << 8;
    srParams.panId += *pData;

    /* Send start request to MAC */
    macWrpData.serialStartRequest = true;
    MAC_WRP_StartRequest(macWrpData.macSerialHandle, &srParams);

    return MAC_WRP_SERIAL_STATUS_SUCCESS;
}

static void lMAC_WRP_CallbackUsiMacProtocol(uint8_t* pData, size_t length)
{
    uint8_t commandAux;
    MAC_WRP_SERIAL_MSG_ID command;
    MAC_WRP_SERIAL_STATUS status = MAC_WRP_SERIAL_STATUS_UNKNOWN_COMMAND;

    /* Protection for invalid length */
    if (length == 0U)
    {
        return;
    }

    /* Process received message */
    commandAux = (*pData++) & 0x7FU;
    command = (MAC_WRP_SERIAL_MSG_ID) commandAux;

    switch(command)
    {
        case MAC_WRP_SERIAL_MSG_MAC_INITIALIZE:
            status = lMAC_WRP_ParseInitialize(pData);
            break;

        case MAC_WRP_SERIAL_MSG_MAC_DATA_REQUEST:
            status = lMAC_WRP_ParseDataRequest(pData);
            break;

        case MAC_WRP_SERIAL_MSG_MAC_GET_REQUEST:
            status = lMAC_WRP_ParseGetRequest(pData);
            break;

        case MAC_WRP_SERIAL_MSG_MAC_SET_REQUEST:
            status = lMAC_WRP_ParseSetRequest(pData);
            break;

        case MAC_WRP_SERIAL_MSG_MAC_RESET_REQUEST:
            status = lMAC_WRP_ParseResetRequest(pData);
            break;

        case MAC_WRP_SERIAL_MSG_MAC_SCAN_REQUEST:
            status = lMAC_WRP_ParseScanRequest(pData);
            break;

        case MAC_WRP_SERIAL_MSG_MAC_START_REQUEST:
            status = lMAC_WRP_ParseStartRequest(pData);
            break;

        default:
            status = MAC_WRP_SERIAL_STATUS_UNKNOWN_COMMAND;
            break;
    }

    /* Initialize doesn't have confirm so send status if already initialized.
     * Other messages all have confirm. Send status only if there is a processing error */
    if ((status != MAC_WRP_SERIAL_STATUS_SUCCESS) ||
            ((command == MAC_WRP_SERIAL_MSG_MAC_INITIALIZE) && (macWrpData.serialInitialize == false)))
    {
        lMAC_WRP_StringifyMsgStatus(status, command);
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Callbacks from MAC Layers
// *****************************************************************************
// *****************************************************************************

static void lMAC_WRP_CallbackMacPlcDataConfirm(MAC_DATA_CONFIRM_PARAMS *dcParams)
{
    MAC_WRP_DATA_CONFIRM_PARAMS dataConfirmParams;
    MAC_WRP_DATA_REQ_ENTRY *matchingDataReq;

    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "lMAC_WRP_CallbackMacPlcDataConfirm() Handle: 0x%02X Status: %u\r\n", dcParams->msduHandle, (uint8_t)dcParams->status);

    /* Get Data Request entry matching confirm */
    matchingDataReq = lMAC_WRP_GetDataReqEntryByHandle(dcParams->msduHandle);

    /* Avoid unmached handling */
    if (matchingDataReq == NULL)
    {
        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_ERROR, "lMAC_WRP_CallbackMacPlcDataConfirm() Confirm does not match any previous request!!\r\n");
        return;
    }

    /* Copy dcParams from Mac */
    (void) memcpy((void *) &dataConfirmParams, (void *) dcParams, sizeof(MAC_DATA_CONFIRM_PARAMS));

    /* Fill Media Type */
    dataConfirmParams.mediaType = MAC_WRP_MEDIA_TYPE_CONF_PLC;

    /* Release Data Req entry and send confirm to upper layer */
    matchingDataReq->used = false;
    if (matchingDataReq->serialDataRequest == true)
    {
        lMAC_WRP_StringifyDataConfirm(&dataConfirmParams);
    }
    else
    {
        if (macWrpData.macWrpHandlers.dataConfirmCallback != NULL)
        {
            macWrpData.macWrpHandlers.dataConfirmCallback(&dataConfirmParams);
        }
    }
}

static void lMAC_WRP_CallbackMacPlcDataIndication(MAC_DATA_INDICATION_PARAMS *diParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "lMAC_WRP_CallbackMacPlcDataIndication");

    MAC_WRP_DATA_INDICATION_PARAMS dataIndicationParams;

     /* Copy diParams from Mac and fill Media Type */
    (void) memcpy((void *) &dataIndicationParams, (void *) diParams, sizeof(MAC_DATA_INDICATION_PARAMS));
    dataIndicationParams.mediaType = MAC_WRP_MEDIA_TYPE_IND_PLC;
    lMAC_WRP_StringifyDataIndication(&dataIndicationParams);
    if (macWrpData.macWrpHandlers.dataIndicationCallback != NULL)
    {
        macWrpData.macWrpHandlers.dataIndicationCallback(&dataIndicationParams);
    }
}

static void lMAC_WRP_CallbackMacPlcResetConfirm(MAC_RESET_CONFIRM_PARAMS *rcParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "lMAC_WRP_CallbackMacPlcResetConfirm: Status: %u\r\n", rcParams->status);

    if (macWrpData.serialResetRequest == true)
    {
        lMAC_WRP_StringifyResetConfirm((void *)rcParams);
        macWrpData.serialResetRequest = false;
    }
    else
    {
        if (macWrpData.macWrpHandlers.resetConfirmCallback != NULL)
        {
            macWrpData.macWrpHandlers.resetConfirmCallback((void *)rcParams);
        }
    }
}

static void lMAC_WRP_CallbackMacPlcBeaconNotify(MAC_BEACON_NOTIFY_INDICATION_PARAMS *bnParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "lMAC_WRP_CallbackMacPlcBeaconNotify: Pan ID: %04X\r\n", bnParams->panDescriptor.panId);

    MAC_WRP_BEACON_NOTIFY_INDICATION_PARAMS notifyIndicationParams;

    /* Copy bnParams from Mac and fill Media Type */
    (void) memcpy((void *) &notifyIndicationParams, (void *) bnParams, sizeof(MAC_BEACON_NOTIFY_INDICATION_PARAMS));
    notifyIndicationParams.panDescriptor.mediaType = MAC_WRP_MEDIA_TYPE_IND_PLC;

    lMAC_WRP_StringifyBeaconNotIndication(&notifyIndicationParams);
    if (macWrpData.macWrpHandlers.beaconNotifyIndicationCallback != NULL)
    {
        macWrpData.macWrpHandlers.beaconNotifyIndicationCallback(&notifyIndicationParams);
    }
}

static void lMAC_WRP_CallbackMacPlcScanConfirm(MAC_SCAN_CONFIRM_PARAMS *scParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "lMAC_WRP_CallbackMacPlcScanConfirm: Status: %u\r\n", scParams->status);

    /* Clear flag */
    macWrpData.scanRequestInProgress = false;

    /* Send confirm to upper layer */
    if (macWrpData.serialScanRequest == true)
    {
        lMAC_WRP_StringifyScanConfirm((void *)scParams);
    }
    else{
        if (macWrpData.macWrpHandlers.scanConfirmCallback != NULL)
        {
            macWrpData.macWrpHandlers.scanConfirmCallback((void *)scParams);
        }
    }
}

static void lMAC_WRP_CallbackMacPlcStartConfirm(MAC_START_CONFIRM_PARAMS *scParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "lMAC_WRP_CallbackMacPlcStartConfirm: Status: %u\r\n", scParams->status);

    if (macWrpData.serialStartRequest == true)
    {
        lMAC_WRP_StringifyStartConfirm((void *)scParams);
        macWrpData.serialStartRequest = false;
    }
    else
    {
        if (macWrpData.macWrpHandlers.startConfirmCallback != NULL)
        {
            macWrpData.macWrpHandlers.startConfirmCallback((void *)scParams);
        }
    }
}

static void lMAC_WRP_CallbackMacPlcCommStatusIndication(MAC_COMM_STATUS_INDICATION_PARAMS *csParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "lMAC_WRP_CallbackMacPlcCommStatusIndication: Status: %u\r\n", csParams->status);

    MAC_WRP_COMM_STATUS_INDICATION_PARAMS commStatusIndicationParams;

    /* Copy csParams from Mac and fill Media Type */
    (void) memcpy((void *) &commStatusIndicationParams, (void *) csParams, sizeof(MAC_COMM_STATUS_INDICATION_PARAMS));
    commStatusIndicationParams.mediaType = MAC_WRP_MEDIA_TYPE_IND_PLC;

    lMAC_WRP_StringifyCommStatusIndication(&commStatusIndicationParams);
    if (macWrpData.macWrpHandlers.commStatusIndicationCallback != NULL)
    {
        macWrpData.macWrpHandlers.commStatusIndicationCallback(&commStatusIndicationParams);
    }
}

static void lMAC_WRP_CallbackMacPlcMacSnifferIndication(MAC_SNIFFER_INDICATION_PARAMS *siParams)
{
    SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, siParams->msdu, siParams->msduLength, "lMAC_WRP_CallbackMacPlcMacSnifferIndication:  MSDU:");

    lMAC_WRP_StringifySnifferIndication((void *)siParams);
    if (macWrpData.macWrpHandlers.snifferIndicationCallback != NULL)
    {
        macWrpData.macWrpHandlers.snifferIndicationCallback((void *)siParams);
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: MAC Wrapper Interface Routines
// *****************************************************************************
// *****************************************************************************

SYS_MODULE_OBJ MAC_WRP_Initialize(const SYS_MODULE_INDEX index)
{
    /* Validate the request */
    if (index >= G3_MAC_WRP_INSTANCES_NUMBER)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    macWrpData.state = MAC_WRP_STATE_NOT_READY;
    macWrpData.macWrpHandle = (MAC_WRP_HANDLE) 0U;
    macWrpData.macSerialHandle = (MAC_WRP_HANDLE) 1U;
    macWrpData.usiHandle = SRV_USI_HANDLE_INVALID;
    macWrpData.serialInitialize = false;
    macWrpData.serialResetRequest = false;
    macWrpData.serialStartRequest = false;
    macWrpData.debugSetLength = 0;
    macWrpData.scanRequestInProgress = false;
    (void) memset(&macWrpData.macWrpHandlers, 0, sizeof(MAC_WRP_HANDLERS));
    for (uint8_t i = 0U; i < MAC_WRP_DATA_REQ_QUEUE_SIZE; i++)
    {
        dataReqQueue[i].used = false;
    }

    return (SYS_MODULE_OBJ)0;
}

MAC_WRP_HANDLE MAC_WRP_Open(SYS_MODULE_INDEX index, MAC_WRP_BAND band)
{
    MAC_PLC_INIT plcInitData;

    /* Single instance allowed */
    if (index >= G3_MAC_WRP_INSTANCES_NUMBER)
    {
        return MAC_WRP_HANDLE_INVALID;
    }

    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "MAC_WRP_Open: Initializing PLC MAC...\r\n");

    plcInitData.macPlcHandlers.macPlcDataConfirm = lMAC_WRP_CallbackMacPlcDataConfirm;
    plcInitData.macPlcHandlers.macPlcDataIndication = lMAC_WRP_CallbackMacPlcDataIndication;
    plcInitData.macPlcHandlers.macPlcResetConfirm = lMAC_WRP_CallbackMacPlcResetConfirm;
    plcInitData.macPlcHandlers.macPlcBeaconNotifyIndication = lMAC_WRP_CallbackMacPlcBeaconNotify;
    plcInitData.macPlcHandlers.macPlcScanConfirm = lMAC_WRP_CallbackMacPlcScanConfirm;
    plcInitData.macPlcHandlers.macPlcStartConfirm = lMAC_WRP_CallbackMacPlcStartConfirm;
    plcInitData.macPlcHandlers.macPlcCommStatusIndication = lMAC_WRP_CallbackMacPlcCommStatusIndication;
    plcInitData.macPlcHandlers.macPlcMacSnifferIndication = lMAC_WRP_CallbackMacPlcMacSnifferIndication;

    (void) memset(macPlcDeviceTable, 0, sizeof(macPlcDeviceTable));

    macPlcTables.macPlcDeviceTableSize = MAC_MAX_DEVICE_TABLE_ENTRIES_PLC;
    macPlcTables.macPlcDeviceTable = macPlcDeviceTable;

    plcInitData.macPlcTables = &macPlcTables;
    plcInitData.plcBand = (MAC_PLC_BAND) band;
    /* Get PAL index from configuration header */
    plcInitData.palPlcIndex = PAL_PLC_PHY_INDEX;

    MAC_PLC_Init(&plcInitData);

    MAC_COMMON_Init();

    macWrpData.state = MAC_WRP_STATE_IDLE;

    macWrpData.nextTaskTimeMs = MAC_WRP_GetMsCounter() + G3_STACK_TASK_RATE_MS;

    return macWrpData.macWrpHandle;
}

void MAC_WRP_SetCallbacks(MAC_WRP_HANDLE handle, MAC_WRP_HANDLERS* handlers)
{
    if ((handle == macWrpData.macWrpHandle) && (handlers != NULL))
    {
        macWrpData.macWrpHandlers = *handlers;
    }
}

void MAC_WRP_Tasks(SYS_MODULE_OBJ object)
{
    if (object != (SYS_MODULE_OBJ) 0)
    {
        /* Invalid object */
        return;
    }

    if (!MAC_COMMON_TimeIsPast((int32_t) macWrpData.nextTaskTimeMs))
    {
        /* Do nothing */
        return;
    }

    /* Time to execute MAC Wrapper task. Update time for next task. */
    macWrpData.nextTaskTimeMs += G3_STACK_TASK_RATE_MS;

    if (macWrpData.usiHandle == SRV_USI_HANDLE_INVALID)
    {
        /* Open USI instance for MAC serialization and register callback */
        macWrpData.usiHandle = SRV_USI_Open(G3_MAC_WRP_SERIAL_USI_INDEX);
        SRV_USI_CallbackRegister(macWrpData.usiHandle, SRV_USI_PROT_ID_MAC_G3, lMAC_WRP_CallbackUsiMacProtocol);
    }

    if ((macWrpData.serialInitialize == true) && (MAC_WRP_Status() == SYS_STATUS_READY))
    {
        /* Send MAC initialization confirm */
        macWrpData.serialInitialize = false;
        lMAC_WRP_StringifyMsgStatus(MAC_WRP_SERIAL_STATUS_SUCCESS, MAC_WRP_SERIAL_MSG_MAC_INITIALIZE);
    }

    MAC_PLC_Tasks();
    MAC_COMMON_GetMsCounter(); /* Just to avoid counter overflow */
}

SYS_STATUS MAC_WRP_Status(void)
{
    return MAC_PLC_Status();
}

void MAC_WRP_DataRequest(MAC_WRP_HANDLE handle, MAC_WRP_DATA_REQUEST_PARAMS *drParams)
{
    MAC_WRP_DATA_CONFIRM_PARAMS dataConfirm;
    MAC_WRP_DATA_REQ_ENTRY *dataReqEntry;

    if ((handle != macWrpData.macWrpHandle) && (handle != macWrpData.macSerialHandle))
    {
        /* Handle error */
        /* Send confirm to upper layer and return */
        if (macWrpData.macWrpHandlers.dataConfirmCallback != NULL)
        {
            dataConfirm.msduHandle = drParams->msduHandle;
            dataConfirm.status = MAC_WRP_STATUS_INVALID_HANDLE;
            dataConfirm.timestamp = 0;
            dataConfirm.mediaType = (MAC_WRP_MEDIA_TYPE_CONFIRM) drParams->mediaType;
            macWrpData.macWrpHandlers.dataConfirmCallback(&dataConfirm);
        }

        return;
    }

    SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_INFO, drParams->msdu, drParams->msduLength, "MAC_WRP_DataRequest (Handle: 0x%02X Media Type: %02X): ", drParams->msduHandle, drParams->mediaType);

    /* Look for free Data Request Entry */
    dataReqEntry = lMAC_WRP_GetFreeDataReqEntry();

    if (dataReqEntry == NULL)
    {
        /* Too many data requests */
        /* Send confirm to upper layer and return */
        dataConfirm.msduHandle = drParams->msduHandle;
        dataConfirm.status = MAC_WRP_STATUS_QUEUE_FULL;
        dataConfirm.timestamp = 0;
        dataConfirm.mediaType = (MAC_WRP_MEDIA_TYPE_CONFIRM)drParams->mediaType;
        if (handle == macWrpData.macSerialHandle)
        {
            lMAC_WRP_StringifyDataConfirm(&dataConfirm);
        }
        else
        {
            if (macWrpData.macWrpHandlers.dataConfirmCallback != NULL)
            {
                macWrpData.macWrpHandlers.dataConfirmCallback(&dataConfirm);
            }
        }

        return;
    }

    /* Accept request */
    /* Copy data to Mac struct (media type is not copied as it is the last field of drParams) */
    (void) memcpy((void *) &dataReqEntry->dataReqParams, (void *) drParams, sizeof(dataReqEntry->dataReqParams));
    if (handle == macWrpData.macSerialHandle)
    {
        dataReqEntry->serialDataRequest = true;
    }
    else
    {
        dataReqEntry->serialDataRequest = false;
    }

    MAC_PLC_DataRequest(&dataReqEntry->dataReqParams);
}

MAC_WRP_STATUS MAC_WRP_GetRequestSync(MAC_WRP_HANDLE handle, MAC_WRP_PIB_ATTRIBUTE attribute, uint16_t index, MAC_WRP_PIB_VALUE *pibValue)
{
    if ((handle != macWrpData.macWrpHandle) && (handle != macWrpData.macSerialHandle))
    {
        // Handle error
        pibValue->length = 0U;
        return MAC_WRP_STATUS_INVALID_HANDLE;
    }

    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "MAC_WRP_GetRequestSync: Attribute: %08X; Index: %u\r\n", attribute, index);

    /* Check attribute ID range to redirect to Common or PLC MAC */
    if (lMAC_WRP_IsSharedAttribute(attribute))
    {
        /* Get from MAC Common */
        return (MAC_WRP_STATUS)(MAC_COMMON_GetRequestSync((MAC_COMMON_PIB_ATTRIBUTE)attribute, index, (void *)pibValue));
    }
    else
    {
        /* RF Available IB has to be handled here */
        if (attribute == MAC_WRP_PIB_MANUF_RF_IFACE_AVAILABLE)
        {
            pibValue->length = 1U;
            pibValue->value[0] = 0U;
            return MAC_WRP_STATUS_SUCCESS;
        }
        else
        {
            /* Get from PLC MAC */
            return (MAC_WRP_STATUS)(MAC_PLC_GetRequestSync((MAC_PLC_PIB_ATTRIBUTE)attribute, index, (void *)pibValue));
        }
    }
}

MAC_WRP_STATUS MAC_WRP_SetRequestSync(MAC_WRP_HANDLE handle, MAC_WRP_PIB_ATTRIBUTE attribute, uint16_t index, const MAC_WRP_PIB_VALUE *pibValue)
{
    if ((handle != macWrpData.macWrpHandle) && (handle != macWrpData.macSerialHandle))
    {
        // Handle error
        return MAC_WRP_STATUS_INVALID_HANDLE;
    }

    SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, pibValue->value, pibValue->length, "MAC_WRP_SetRequestSync: Attribute: %08X; Index: %u; Value: ", attribute, index);

    /* Check attribute ID range to redirect to Common or PLC MAC */
    if (lMAC_WRP_IsSharedAttribute(attribute))
    {
        /* Set to MAC Common */
        return (MAC_WRP_STATUS)(MAC_COMMON_SetRequestSync((MAC_COMMON_PIB_ATTRIBUTE)attribute, index, (const void *)pibValue));
    }
    else
    {
        /* Set to PLC MAC */
        return (MAC_WRP_STATUS)(MAC_PLC_SetRequestSync((MAC_PLC_PIB_ATTRIBUTE)attribute, index, (const void *)pibValue));
    }
}

void MAC_WRP_ResetRequest(MAC_WRP_HANDLE handle, MAC_WRP_RESET_REQUEST_PARAMS *rstParams)
{
    if ((handle != macWrpData.macWrpHandle) && (handle != macWrpData.macSerialHandle))
    {
        /* Handle error */
        /* Send confirm to upper layer and return */
        MAC_WRP_RESET_CONFIRM_PARAMS resetConfirm;
        if (macWrpData.macWrpHandlers.resetConfirmCallback != NULL)
        {
            resetConfirm.status = MAC_WRP_STATUS_INVALID_HANDLE;
            macWrpData.macWrpHandlers.resetConfirmCallback(&resetConfirm);
        }

        return;
    }

    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "MAC_WRP_ResetRequest: Set default PIB: %u\r\n", rstParams->setDefaultPib);

    // Reset PLC MAC
    MAC_PLC_ResetRequest((void *)rstParams);
    // Reset Common MAC if IB has to be reset
    if (rstParams->setDefaultPib)
    {
        MAC_COMMON_Reset();
    }
}

void MAC_WRP_ScanRequest(MAC_WRP_HANDLE handle, MAC_WRP_SCAN_REQUEST_PARAMS *scanParams)
{
    MAC_WRP_SCAN_CONFIRM_PARAMS scanConfirm;

    if ((handle != macWrpData.macWrpHandle) && (handle != macWrpData.macSerialHandle))
    {
        /* Handle error */
        /* Send confirm to upper layer and return */
        if (macWrpData.macWrpHandlers.scanConfirmCallback != NULL)
        {
            scanConfirm.status = MAC_WRP_STATUS_INVALID_HANDLE;
            macWrpData.macWrpHandlers.scanConfirmCallback(&scanConfirm);
        }

        return;
    }

    if (macWrpData.scanRequestInProgress == true)
    {
        /* Scan request already in progress */
        /* Send confirm to upper layer and return */
        scanConfirm.status = MAC_WRP_STATUS_DENIED;
        if (handle == macWrpData.macSerialHandle)
        {
            lMAC_WRP_StringifyScanConfirm(&scanConfirm);
        }
        else
        {
            if (macWrpData.macWrpHandlers.scanConfirmCallback != NULL)
            {
                macWrpData.macWrpHandlers.scanConfirmCallback(&scanConfirm);
            }
        }

        return;
    }

    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "MAC_WRP_ScanRequest: Duration: %u\r\n", scanParams->scanDuration);

    // Set control variable
    macWrpData.scanRequestInProgress = true;
    if (handle == macWrpData.macSerialHandle)
    {
        macWrpData.serialScanRequest = true;
    }
    else
    {
        macWrpData.serialScanRequest = false;
    }

    // Set PLC MAC on Scan state
    MAC_PLC_ScanRequest((void *)scanParams);
}

void MAC_WRP_StartRequest(MAC_WRP_HANDLE handle, MAC_WRP_START_REQUEST_PARAMS *startParams)
{
    if ((handle != macWrpData.macWrpHandle) && (handle != macWrpData.macSerialHandle))
    {
        /* Handle error */
        /* Send confirm to upper layer and return */
        MAC_WRP_START_CONFIRM_PARAMS startConfirm;
        if (macWrpData.macWrpHandlers.startConfirmCallback != NULL)
        {
            startConfirm.status = MAC_WRP_STATUS_INVALID_HANDLE;
            macWrpData.macWrpHandlers.startConfirmCallback(&startConfirm);
        }

        return;
    }

    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "MAC_WRP_StartRequest: Pan ID: %u\r\n", startParams->panId);

    // Start Network on PLC MAC
    MAC_PLC_StartRequest((void *)startParams);
}

MAC_WRP_AVAILABLE_MAC_LAYERS MAC_WRP_GetAvailableMacLayers(MAC_WRP_HANDLE handle)
{
    return MAC_WRP_AVAILABLE_MAC_PLC;
}

uint32_t MAC_WRP_SerialParseGetRequest(uint8_t* pData, uint16_t* index)
{
    uint32_t attribute;
    uint16_t attrIndexAux;

    attribute = ((uint32_t) *pData++) << 24;
    attribute += ((uint32_t) *pData++) << 16;
    attribute += ((uint32_t) *pData++) << 8;
    attribute += (uint32_t) *pData++;

    attrIndexAux = ((uint16_t) *pData++) << 8;
    attrIndexAux += (uint16_t) *pData;
    *index = attrIndexAux;

    return attribute;
}

/* MISRA C-2012 deviation block start */
/* MISRA C-2012 Rule 16.4 deviated 4 times. Deviation record ID - H3_MISRAC_2012_R_16_4_DR_1 */

uint8_t MAC_WRP_SerialStringifyGetConfirm (
    uint8_t *serialData,
    MAC_WRP_STATUS getStatus,
    MAC_WRP_PIB_ATTRIBUTE attribute,
    uint16_t index,
    uint8_t* pibValue,
    uint8_t pibLength
)
{
    uint8_t serialRspLen = 0;
    MAC_WRP_NEIGHBOUR_ENTRY* pNeighbourEntry;
    MAC_WRP_POS_ENTRY* pPosEntry;

    serialData[serialRspLen++] = (uint8_t) getStatus;
    serialData[serialRspLen++] = (uint8_t) ((uint32_t) attribute >> 24);
    serialData[serialRspLen++] = (uint8_t) ((uint32_t) attribute >> 16);
    serialData[serialRspLen++] = (uint8_t) ((uint32_t) attribute >> 8);
    serialData[serialRspLen++] = (uint8_t) attribute;
    serialData[serialRspLen++] = (uint8_t) (index >> 8);
    serialData[serialRspLen++] = (uint8_t) index;

    serialData[serialRspLen++] = pibLength;

    if (getStatus == MAC_WRP_STATUS_SUCCESS)
    {
        switch (attribute)
        {
            /* 8-bit IBs */
            case MAC_WRP_PIB_PROMISCUOUS_MODE:
            case MAC_WRP_PIB_POS_TABLE_ENTRY_TTL:
            case MAC_WRP_PIB_POS_RECENT_ENTRY_THRESHOLD:
            case MAC_WRP_PIB_MANUF_PLC_IFACE_AVAILABLE:
            case MAC_WRP_PIB_MANUF_RF_IFACE_AVAILABLE:
            case MAC_WRP_PIB_BSN:
            case MAC_WRP_PIB_DSN:
            case MAC_WRP_PIB_MAX_BE:
            case MAC_WRP_PIB_MAX_CSMA_BACKOFFS:
            case MAC_WRP_PIB_MAX_FRAME_RETRIES:
            case MAC_WRP_PIB_MIN_BE:
            case MAC_WRP_PIB_SECURITY_ENABLED:
            case MAC_WRP_PIB_TIMESTAMP_SUPPORTED:
            case MAC_WRP_PIB_HIGH_PRIORITY_WINDOW_SIZE:
            case MAC_WRP_PIB_FREQ_NOTCHING:
            case MAC_WRP_PIB_CSMA_FAIRNESS_LIMIT:
            case MAC_WRP_PIB_TMR_TTL:
            case MAC_WRP_PIB_DUPLICATE_DETECTION_TTL:
            case MAC_WRP_PIB_BEACON_RANDOMIZATION_WINDOW_LENGTH:
            case MAC_WRP_PIB_A:
            case MAC_WRP_PIB_K:
            case MAC_WRP_PIB_MIN_CW_ATTEMPTS:
            case MAC_WRP_PIB_CENELEC_LEGACY_MODE:
            case MAC_WRP_PIB_FCC_LEGACY_MODE:
            case MAC_WRP_PIB_BROADCAST_MAX_CW_ENABLE:
            case MAC_WRP_PIB_PLC_DISABLE:
            case MAC_WRP_PIB_MANUF_FORCED_MOD_SCHEME:
            case MAC_WRP_PIB_MANUF_FORCED_MOD_TYPE:
            case MAC_WRP_PIB_MANUF_FORCED_MOD_SCHEME_ON_TMRESPONSE:
            case MAC_WRP_PIB_MANUF_FORCED_MOD_TYPE_ON_TMRESPONSE:
            case MAC_WRP_PIB_MANUF_LAST_RX_MOD_SCHEME:
            case MAC_WRP_PIB_MANUF_LAST_RX_MOD_TYPE:
            case MAC_WRP_PIB_MANUF_LBP_FRAME_RECEIVED:
            case MAC_WRP_PIB_MANUF_LNG_FRAME_RECEIVED:
            case MAC_WRP_PIB_MANUF_BCN_FRAME_RECEIVED:
            case MAC_WRP_PIB_MANUF_ENABLE_MAC_SNIFFER:
            case MAC_WRP_PIB_MANUF_RETRIES_LEFT_TO_FORCE_ROBO:
            case MAC_WRP_PIB_MANUF_SLEEP_MODE:
            case MAC_WRP_PIB_MANUF_TRICKLE_MIN_LQI:
                serialData[serialRspLen++] = pibValue[0];
                break;

            /* 16-bit IBs */
            case MAC_WRP_PIB_PAN_ID:
            case MAC_WRP_PIB_SHORT_ADDRESS:
            case MAC_WRP_PIB_RC_COORD:
            case MAC_WRP_PIB_POS_RECENT_ENTRIES:
            case MAC_WRP_PIB_MANUF_COORD_SHORT_ADDRESS:
            case MAC_WRP_PIB_MANUF_MAX_MAC_PAYLOAD_SIZE:
            case MAC_WRP_PIB_MANUF_NEIGHBOUR_TABLE_COUNT:
            case MAC_WRP_PIB_MANUF_POS_TABLE_COUNT:
            case MAC_WRP_PIB_MANUF_LAST_FRAME_DURATION_PLC:
                lMemcpyToUsiEndianessUint16(&serialData[serialRspLen], pibValue);
                serialRspLen += 2U;
                break;

            /* 32-bit IBs */
            case MAC_WRP_PIB_FRAME_COUNTER:
            case MAC_WRP_PIB_TX_DATA_PACKET_COUNT:
            case MAC_WRP_PIB_RX_DATA_PACKET_COUNT:
            case MAC_WRP_PIB_TX_CMD_PACKET_COUNT:
            case MAC_WRP_PIB_RX_CMD_PACKET_COUNT:
            case MAC_WRP_PIB_CSMA_FAIL_COUNT:
            case MAC_WRP_PIB_CSMA_NO_ACK_COUNT:
            case MAC_WRP_PIB_RX_DATA_BROADCAST_COUNT:
            case MAC_WRP_PIB_TX_DATA_BROADCAST_COUNT:
            case MAC_WRP_PIB_BAD_CRC_COUNT:
            case MAC_WRP_PIB_MANUF_RX_OTHER_DESTINATION_COUNT:
            case MAC_WRP_PIB_MANUF_RX_INVALID_FRAME_LENGTH_COUNT:
            case MAC_WRP_PIB_MANUF_RX_MAC_REPETITION_COUNT:
            case MAC_WRP_PIB_MANUF_RX_WRONG_ADDR_MODE_COUNT:
            case MAC_WRP_PIB_MANUF_RX_UNSUPPORTED_SECURITY_COUNT:
            case MAC_WRP_PIB_MANUF_RX_WRONG_KEY_ID_COUNT:
            case MAC_WRP_PIB_MANUF_RX_INVALID_KEY_COUNT:
            case MAC_WRP_PIB_MANUF_RX_WRONG_FC_COUNT:
            case MAC_WRP_PIB_MANUF_RX_DECRYPTION_ERROR_COUNT:
            case MAC_WRP_PIB_MANUF_RX_SEGMENT_DECODE_ERROR_COUNT:
                lMemcpyToUsiEndianessUint32(&serialData[serialRspLen], pibValue);
                serialRspLen += 4U;
                break;

            /* Tables and lists */
            case MAC_WRP_PIB_MANUF_EXTENDED_ADDRESS:
                (void) memcpy(&serialData[serialRspLen], pibValue, 8U);
                serialRspLen += 8U;
                break;

            case MAC_WRP_PIB_MANUF_DEVICE_TABLE:
                /* panId */
                lMemcpyToUsiEndianessUint16(&serialData[serialRspLen], pibValue);
                serialRspLen += 2U;
                /* shortAddress */
                lMemcpyToUsiEndianessUint16(&serialData[serialRspLen], &pibValue[2]);
                serialRspLen += 2U;
                /* frameCounter */
                lMemcpyToUsiEndianessUint32(&serialData[serialRspLen], &pibValue[4]);
                serialRspLen += 4U;
                break;

            case MAC_WRP_PIB_MANUF_MAC_INTERNAL_VERSION:
            case MAC_WRP_PIB_MANUF_MAC_RT_INTERNAL_VERSION:
                /* Version */
                (void) memcpy(&serialData[serialRspLen], pibValue, 6U);
                serialRspLen += 6U;
                break;

            case MAC_WRP_PIB_NEIGHBOUR_TABLE:
            case MAC_WRP_PIB_MANUF_NEIGHBOUR_TABLE_ELEMENT:
                pNeighbourEntry = (void*) pibValue;
                serialData[serialRspLen++] = (uint8_t) (pNeighbourEntry->shortAddress >> 8);
                serialData[serialRspLen++] = (uint8_t) pNeighbourEntry->shortAddress;
                (void) memcpy(&serialData[serialRspLen], pNeighbourEntry->toneMap.toneMap, (MAC_WRP_MAX_TONE_GROUPS + 7U) / 8U);
                serialRspLen += (MAC_WRP_MAX_TONE_GROUPS + 7U) / 8U;
                serialData[serialRspLen++] = (uint8_t) pNeighbourEntry->modulationType;
                serialData[serialRspLen++] = (uint8_t) pNeighbourEntry->txGain;
                serialData[serialRspLen++] = (uint8_t) pNeighbourEntry->txRes;
                (void) memcpy(&serialData[serialRspLen], pNeighbourEntry->txCoef.txCoef, 6U);
                serialRspLen += 6U;
                serialData[serialRspLen++] = (uint8_t) pNeighbourEntry->modulationScheme;
                serialData[serialRspLen++] = (uint8_t) pNeighbourEntry->phaseDifferential;
                serialData[serialRspLen++] = (uint8_t) pNeighbourEntry->lqi;
                serialData[serialRspLen++] = (uint8_t) (pNeighbourEntry->tmrValidTime >> 8);
                serialData[serialRspLen++] = (uint8_t) pNeighbourEntry->tmrValidTime;
                /* Length has to be incremented by 2 due to bitfields in the entry are serialized in separate fields */
                serialData[7] = pibLength + 2U;
                break;

            case MAC_WRP_PIB_POS_TABLE:
            case MAC_WRP_PIB_MANUF_POS_TABLE_ELEMENT:
                pPosEntry = (void*) pibValue;
                serialData[serialRspLen++] = (uint8_t) (pPosEntry->shortAddress >> 8);
                serialData[serialRspLen++] = (uint8_t) pPosEntry->shortAddress;
                serialData[serialRspLen++] = (uint8_t) pPosEntry->lqi;
                serialData[serialRspLen++] = (uint8_t) (pPosEntry->posValidTime >> 8);
                serialData[serialRspLen++] = (uint8_t) pPosEntry->posValidTime;
                break;

            case MAC_WRP_PIB_TONE_MASK:
                (void) memcpy(&serialData[serialRspLen], pibValue, (MAC_WRP_MAX_TONES + 7U) / 8U);
                serialRspLen += (MAC_WRP_MAX_TONES + 7U) / 8U;
                break;

            case MAC_WRP_PIB_MANUF_BAND_INFORMATION:
                /* flMax */
                lMemcpyToUsiEndianessUint16(&serialData[serialRspLen], pibValue);
                serialRspLen += 2U;
                /* band */
                serialData[serialRspLen++] = pibValue[2];
                /* tones */
                serialData[serialRspLen++] = pibValue[3];
                /* carriers */
                serialData[serialRspLen++] = pibValue[4];
                /* tonesInCarrier */
                serialData[serialRspLen++] = pibValue[5];
                /* flBand */
                serialData[serialRspLen++] = pibValue[6];
                /* maxRsBlocks */
                serialData[serialRspLen++] = pibValue[7];
                /* txCoefBits */
                serialData[serialRspLen++] = pibValue[8];
                /* pilotsFreqSpa */
                serialData[serialRspLen++] = pibValue[9];
                break;

            case MAC_WRP_PIB_MANUF_FORCED_TONEMAP:
            case MAC_WRP_PIB_MANUF_FORCED_TONEMAP_ON_TMRESPONSE:
                (void) memcpy(&serialData[serialRspLen], pibValue, 3U);
                serialRspLen += 3U;
                break;

            case MAC_WRP_PIB_MANUF_DEBUG_SET:
                (void) memcpy(&serialData[serialRspLen], pibValue, 7U);
                serialRspLen += 7U;
                break;

            case MAC_WRP_PIB_MANUF_DEBUG_READ:
                (void) memcpy(&serialData[serialRspLen], pibValue, macWrpData.debugSetLength);
                serialRspLen += macWrpData.debugSetLength;
                break;

            case MAC_WRP_PIB_KEY_TABLE:
                /* response will be MAC_WRP_STATUS_UNAVAILABLE_KEY */
                break;

            case MAC_WRP_PIB_MANUF_SECURITY_RESET:
            case MAC_WRP_PIB_MANUF_RESET_MAC_STATS:
                /* Response will be mac_status_denied */
                break;

            case MAC_WRP_PIB_MANUF_PHY_PARAM:
                switch ((MAC_WRP_PHY_PARAM) index)
                {
                    case MAC_WRP_PHY_PARAM_VERSION:
                    case MAC_WRP_PHY_PARAM_TX_TOTAL:
                    case MAC_WRP_PHY_PARAM_TX_TOTAL_BYTES:
                    case MAC_WRP_PHY_PARAM_TX_TOTAL_ERRORS:
                    case MAC_WRP_PHY_PARAM_BAD_BUSY_TX:
                    case MAC_WRP_PHY_PARAM_TX_BAD_BUSY_CHANNEL:
                    case MAC_WRP_PHY_PARAM_TX_BAD_LEN:
                    case MAC_WRP_PHY_PARAM_TX_BAD_FORMAT:
                    case MAC_WRP_PHY_PARAM_TX_TIMEOUT:
                    case MAC_WRP_PHY_PARAM_RX_TOTAL:
                    case MAC_WRP_PHY_PARAM_RX_TOTAL_BYTES:
                    case MAC_WRP_PHY_PARAM_RX_RS_ERRORS:
                    case MAC_WRP_PHY_PARAM_RX_EXCEPTIONS:
                    case MAC_WRP_PHY_PARAM_RX_BAD_LEN:
                    case MAC_WRP_PHY_PARAM_RX_BAD_CRC_FCH:
                    case MAC_WRP_PHY_PARAM_RX_FALSE_POSITIVE:
                    case MAC_WRP_PHY_PARAM_RX_BAD_FORMAT:
                    case MAC_WRP_PHY_PARAM_TIME_BETWEEN_NOISE_CAPTURES:
                        lMemcpyToUsiEndianessUint32(&serialData[serialRspLen], pibValue);
                        serialRspLen += 4U;
                        break;

                    case MAC_WRP_PHY_PARAM_LAST_MSG_RSSI:
                    case MAC_WRP_PHY_PARAM_ACK_TX_CFM:
                    case MAC_WRP_PHY_PARAM_LAST_MSG_DURATION:
                        lMemcpyToUsiEndianessUint16(&serialData[serialRspLen], pibValue);
                        serialRspLen += 2U;
                        break;

                    case MAC_WRP_PHY_PARAM_ENABLE_AUTO_NOISE_CAPTURE:
                    case MAC_WRP_PHY_PARAM_DELAY_NOISE_CAPTURE_AFTER_RX:
                    case MAC_WRP_PHY_PARAM_CFG_AUTODETECT_BRANCH:
                    case MAC_WRP_PHY_PARAM_CFG_IMPEDANCE:
                    case MAC_WRP_PHY_PARAM_RRC_NOTCH_ACTIVE:
                    case MAC_WRP_PHY_PARAM_RRC_NOTCH_INDEX:
                    case MAC_WRP_PHY_PARAM_PLC_DISABLE:
                    case MAC_WRP_PHY_PARAM_NOISE_PEAK_POWER:
                    case MAC_WRP_PHY_PARAM_LAST_MSG_LQI:
                    case MAC_WRP_PHY_PARAM_PREAMBLE_NUM_SYNCP:
                        serialData[serialRspLen++] = pibValue[0];
                        break;

                    default:
                        break;
                }
                break;

            default:
                break;
        }
    }

	return serialRspLen;
}

MAC_WRP_PIB_ATTRIBUTE MAC_WRP_SerialParseSetRequest (
    uint8_t* pData,
    uint16_t* index,
    MAC_WRP_PIB_VALUE* pibValue
)
{
    uint8_t pibLenCnt = 0;
    uint32_t attrAux;
    MAC_WRP_PIB_ATTRIBUTE attribute;
    uint16_t attrIndexAux;
    MAC_WRP_NEIGHBOUR_ENTRY pNeighbourEntry;
    MAC_WRP_POS_ENTRY pPosEntry;

    attrAux = ((uint32_t) *pData++) << 24;
    attrAux += ((uint32_t) *pData++) << 16;
    attrAux += ((uint32_t) *pData++) << 8;
    attrAux += (uint32_t) *pData++;
    attribute = (MAC_WRP_PIB_ATTRIBUTE) attrAux;

    attrIndexAux = ((uint16_t) *pData++) << 8;
    attrIndexAux += (uint16_t) *pData++;
    *index = attrIndexAux;

    pibValue->length = *pData++;

    switch (attribute)
    {
        /* 8-bit IBs */
        case MAC_WRP_PIB_PROMISCUOUS_MODE:
        case MAC_WRP_PIB_POS_TABLE_ENTRY_TTL:
        case MAC_WRP_PIB_POS_RECENT_ENTRY_THRESHOLD:
        case MAC_WRP_PIB_MANUF_PLC_IFACE_AVAILABLE:
        case MAC_WRP_PIB_MANUF_RF_IFACE_AVAILABLE:
        case MAC_WRP_PIB_BSN:
        case MAC_WRP_PIB_DSN:
        case MAC_WRP_PIB_MAX_BE:
        case MAC_WRP_PIB_MAX_CSMA_BACKOFFS:
        case MAC_WRP_PIB_MAX_FRAME_RETRIES:
        case MAC_WRP_PIB_MIN_BE:
        case MAC_WRP_PIB_HIGH_PRIORITY_WINDOW_SIZE:
        case MAC_WRP_PIB_FREQ_NOTCHING:
        case MAC_WRP_PIB_CSMA_FAIRNESS_LIMIT:
        case MAC_WRP_PIB_TMR_TTL:
        case MAC_WRP_PIB_DUPLICATE_DETECTION_TTL:
        case MAC_WRP_PIB_BEACON_RANDOMIZATION_WINDOW_LENGTH:
        case MAC_WRP_PIB_A:
        case MAC_WRP_PIB_K:
        case MAC_WRP_PIB_MIN_CW_ATTEMPTS:
        case MAC_WRP_PIB_BROADCAST_MAX_CW_ENABLE:
        case MAC_WRP_PIB_PLC_DISABLE:
        case MAC_WRP_PIB_MANUF_FORCED_MOD_SCHEME:
        case MAC_WRP_PIB_MANUF_FORCED_MOD_TYPE:
        case MAC_WRP_PIB_MANUF_FORCED_MOD_SCHEME_ON_TMRESPONSE:
        case MAC_WRP_PIB_MANUF_FORCED_MOD_TYPE_ON_TMRESPONSE:
        case MAC_WRP_PIB_MANUF_LBP_FRAME_RECEIVED:
        case MAC_WRP_PIB_MANUF_LNG_FRAME_RECEIVED:
        case MAC_WRP_PIB_MANUF_BCN_FRAME_RECEIVED:
        case MAC_WRP_PIB_MANUF_ENABLE_MAC_SNIFFER:
        case MAC_WRP_PIB_MANUF_RETRIES_LEFT_TO_FORCE_ROBO:
        case MAC_WRP_PIB_MANUF_SLEEP_MODE:
        case MAC_WRP_PIB_MANUF_TRICKLE_MIN_LQI:
            pibValue->value[0] = *pData;
            break;

        /* 16-bit IBs */
        case MAC_WRP_PIB_PAN_ID:
        case MAC_WRP_PIB_SHORT_ADDRESS:
        case MAC_WRP_PIB_RC_COORD:
        case MAC_WRP_PIB_POS_RECENT_ENTRIES:
        case MAC_WRP_PIB_MANUF_LAST_FRAME_DURATION_PLC:
            lMemcpyFromUsiEndianessUint16(pibValue->value, pData);
            break;

        /* 32-bit IBs */
        case MAC_WRP_PIB_FRAME_COUNTER:
        case MAC_WRP_PIB_TX_DATA_PACKET_COUNT:
        case MAC_WRP_PIB_RX_DATA_PACKET_COUNT:
        case MAC_WRP_PIB_TX_CMD_PACKET_COUNT:
        case MAC_WRP_PIB_RX_CMD_PACKET_COUNT:
        case MAC_WRP_PIB_CSMA_FAIL_COUNT:
        case MAC_WRP_PIB_CSMA_NO_ACK_COUNT:
        case MAC_WRP_PIB_RX_DATA_BROADCAST_COUNT:
        case MAC_WRP_PIB_TX_DATA_BROADCAST_COUNT:
        case MAC_WRP_PIB_BAD_CRC_COUNT:
        case MAC_WRP_PIB_MANUF_RX_OTHER_DESTINATION_COUNT:
        case MAC_WRP_PIB_MANUF_RX_INVALID_FRAME_LENGTH_COUNT:
        case MAC_WRP_PIB_MANUF_RX_MAC_REPETITION_COUNT:
        case MAC_WRP_PIB_MANUF_RX_WRONG_ADDR_MODE_COUNT:
        case MAC_WRP_PIB_MANUF_RX_UNSUPPORTED_SECURITY_COUNT:
        case MAC_WRP_PIB_MANUF_RX_WRONG_KEY_ID_COUNT:
        case MAC_WRP_PIB_MANUF_RX_INVALID_KEY_COUNT:
        case MAC_WRP_PIB_MANUF_RX_WRONG_FC_COUNT:
        case MAC_WRP_PIB_MANUF_RX_DECRYPTION_ERROR_COUNT:
        case MAC_WRP_PIB_MANUF_RX_SEGMENT_DECODE_ERROR_COUNT:
            lMemcpyFromUsiEndianessUint32(pibValue->value, pData);
            break;

        /* Tables and lists */
        case MAC_WRP_PIB_MANUF_EXTENDED_ADDRESS:
            /* m_au8Address */
            (void) memcpy(pibValue->value, pData, 8);
            break;

        case MAC_WRP_PIB_KEY_TABLE:
            (void) memcpy(pibValue->value, pData, MAC_WRP_SECURITY_KEY_LENGTH);
            break;

        case MAC_WRP_PIB_NEIGHBOUR_TABLE:
            pNeighbourEntry.shortAddress = ((uint16_t) *pData++) << 8;
            pNeighbourEntry.shortAddress += (uint16_t) *pData++;
            (void) memcpy(pNeighbourEntry.toneMap.toneMap, pData, (MAC_WRP_MAX_TONE_GROUPS + 7U) / 8U);
            pData += (MAC_WRP_MAX_TONE_GROUPS + 7U) / 8U;
            pNeighbourEntry.modulationType = *pData++;
            pNeighbourEntry.txGain = *pData++;
            pNeighbourEntry.txRes = *pData++;
            (void) memcpy(pNeighbourEntry.txCoef.txCoef, pData, 6U);
            pData += 6U;
            pNeighbourEntry.modulationScheme = *pData++;
            pNeighbourEntry.phaseDifferential = *pData++;
            pNeighbourEntry.lqi  = *pData++;
            pNeighbourEntry.tmrValidTime = ((uint16_t) *pData++) << 8;
            pNeighbourEntry.tmrValidTime += (uint16_t) *pData;
            (void) memcpy((void *) pibValue->value, (void *) &pNeighbourEntry, sizeof(MAC_WRP_NEIGHBOUR_ENTRY));
            /* Struct saves 2 bytes with bit-fields */
            pibValue->length -= 2U;
            break;

        case MAC_WRP_PIB_POS_TABLE:
            pPosEntry.shortAddress = ((uint16_t) *pData++) << 8;
            pPosEntry.shortAddress += (uint16_t) *pData++;
            pPosEntry.lqi  = *pData++;
            pPosEntry.posValidTime = ((uint16_t) *pData++) << 8;
            pPosEntry.posValidTime += (uint16_t) *pData;
            (void) memcpy((void *) pibValue->value, (void *) &pPosEntry, sizeof(MAC_WRP_POS_ENTRY));
            break;

        case MAC_WRP_PIB_TONE_MASK:
            (void) memcpy(pibValue->value, pData, (MAC_WRP_MAX_TONES + 7U) / 8U);
            break;

        case MAC_WRP_PIB_MANUF_FORCED_TONEMAP:
        case MAC_WRP_PIB_MANUF_FORCED_TONEMAP_ON_TMRESPONSE:
            pibValue->value[pibLenCnt++] = *pData++;
            pibValue->value[pibLenCnt++] = *pData++;
            pibValue->value[pibLenCnt] = *pData;
            break;

        case MAC_WRP_PIB_MANUF_DEBUG_SET:
        {
            uint16_t debugLength;
            (void) memcpy(pibValue->value, pData, 7U);
            lMemcpyFromUsiEndianessUint16((void *) &debugLength, &pData[5]);
            if (debugLength > MAC_PIB_MAX_VALUE_LENGTH)
            {
                macWrpData.debugSetLength = MAC_PIB_MAX_VALUE_LENGTH;
            }
            else
            {
                macWrpData.debugSetLength = (uint8_t) debugLength;
            }
            break;
        }

        case MAC_WRP_PIB_SECURITY_ENABLED:
        case MAC_WRP_PIB_TIMESTAMP_SUPPORTED:
        case MAC_WRP_PIB_CENELEC_LEGACY_MODE:
        case MAC_WRP_PIB_FCC_LEGACY_MODE:
        case MAC_WRP_PIB_MANUF_DEVICE_TABLE:
        case MAC_WRP_PIB_MANUF_NEIGHBOUR_TABLE_ELEMENT:
        case MAC_WRP_PIB_MANUF_BAND_INFORMATION:
        case MAC_WRP_PIB_MANUF_COORD_SHORT_ADDRESS:
        case MAC_WRP_PIB_MANUF_MAX_MAC_PAYLOAD_SIZE:
        case MAC_WRP_PIB_MANUF_LAST_RX_MOD_SCHEME:
        case MAC_WRP_PIB_MANUF_LAST_RX_MOD_TYPE:
        case MAC_WRP_PIB_MANUF_NEIGHBOUR_TABLE_COUNT:
        case MAC_WRP_PIB_MANUF_POS_TABLE_COUNT:
        case MAC_WRP_PIB_MANUF_MAC_INTERNAL_VERSION:
        case MAC_WRP_PIB_MANUF_MAC_RT_INTERNAL_VERSION:
        case MAC_WRP_PIB_MANUF_POS_TABLE_ELEMENT:
            /* MAC_WRP_STATUS_READ_ONLY */
            break;

        case MAC_WRP_PIB_MANUF_SECURITY_RESET:
            /* If length is 0 then DeviceTable is going to be reset else response will be MAC_WRP_STATUS_INVALID_PARAMETER */
            break;
        case MAC_WRP_PIB_MANUF_RESET_MAC_STATS:
            /* If length is 0 then MAC Statistics will be reset */
            break;

        case MAC_WRP_PIB_MANUF_PHY_PARAM:
            switch ((MAC_WRP_PHY_PARAM) attrIndexAux)
            {
                case MAC_WRP_PHY_PARAM_TX_TOTAL:
                case MAC_WRP_PHY_PARAM_TX_TOTAL_BYTES:
                case MAC_WRP_PHY_PARAM_TX_TOTAL_ERRORS:
                case MAC_WRP_PHY_PARAM_BAD_BUSY_TX:
                case MAC_WRP_PHY_PARAM_TX_BAD_BUSY_CHANNEL:
                case MAC_WRP_PHY_PARAM_TX_BAD_LEN:
                case MAC_WRP_PHY_PARAM_TX_BAD_FORMAT:
                case MAC_WRP_PHY_PARAM_TX_TIMEOUT:
                case MAC_WRP_PHY_PARAM_RX_TOTAL:
                case MAC_WRP_PHY_PARAM_RX_TOTAL_BYTES:
                case MAC_WRP_PHY_PARAM_RX_RS_ERRORS:
                case MAC_WRP_PHY_PARAM_RX_EXCEPTIONS:
                case MAC_WRP_PHY_PARAM_RX_BAD_LEN:
                case MAC_WRP_PHY_PARAM_RX_BAD_CRC_FCH:
                case MAC_WRP_PHY_PARAM_RX_FALSE_POSITIVE:
                case MAC_WRP_PHY_PARAM_RX_BAD_FORMAT:
                case MAC_WRP_PHY_PARAM_TIME_BETWEEN_NOISE_CAPTURES:
                    lMemcpyFromUsiEndianessUint32(pibValue->value, pData);
                    break;

                case MAC_WRP_PHY_PARAM_LAST_MSG_RSSI:
                case MAC_WRP_PHY_PARAM_ACK_TX_CFM:
                case MAC_WRP_PHY_PARAM_LAST_MSG_DURATION:
                    lMemcpyFromUsiEndianessUint16(pibValue->value, pData);
                    break;

                case MAC_WRP_PHY_PARAM_ENABLE_AUTO_NOISE_CAPTURE:
                case MAC_WRP_PHY_PARAM_DELAY_NOISE_CAPTURE_AFTER_RX:
                case MAC_WRP_PHY_PARAM_CFG_AUTODETECT_BRANCH:
                case MAC_WRP_PHY_PARAM_CFG_IMPEDANCE:
                case MAC_WRP_PHY_PARAM_RRC_NOTCH_ACTIVE:
                case MAC_WRP_PHY_PARAM_RRC_NOTCH_INDEX:
                case MAC_WRP_PHY_PARAM_PLC_DISABLE:
                case MAC_WRP_PHY_PARAM_LAST_MSG_LQI:
                case MAC_WRP_PHY_PARAM_PREAMBLE_NUM_SYNCP:
                    pibValue->value[0] = *pData;
                    break;

                case MAC_WRP_PHY_PARAM_VERSION:
                case MAC_WRP_PHY_PARAM_NOISE_PEAK_POWER:
                    /* MAC_WRP_STATUS_READ_ONLY */
                    break;

                default:
                    break;
            }
            break;

        default:
            break;
    }

    return attribute;
}

/* MISRA C-2012 deviation block end */

uint8_t MAC_WRP_SerialStringifySetConfirm (
    uint8_t *serialData,
    MAC_WRP_STATUS setStatus,
    MAC_WRP_PIB_ATTRIBUTE attribute,
    uint16_t index
)
{
    uint8_t serialRspLen = 0;

    serialData[serialRspLen++] = (uint8_t) setStatus;
    serialData[serialRspLen++] = (uint8_t) ((uint32_t) attribute >> 24);
    serialData[serialRspLen++] = (uint8_t) ((uint32_t) attribute >> 16);
    serialData[serialRspLen++] = (uint8_t) ((uint32_t) attribute >> 8);
    serialData[serialRspLen++] = (uint8_t) attribute;
    serialData[serialRspLen++] = (uint8_t) (index >> 8);
    serialData[serialRspLen++] = (uint8_t) index;

    return serialRspLen;
}

uint32_t MAC_WRP_GetMsCounter(void)
{
    /* Call lower layer function */
    return MAC_COMMON_GetMsCounter();
}

bool MAC_WRP_TimeIsPast(int32_t timeValue)
{
    /* Call lower layer function */
    return MAC_COMMON_TimeIsPast(timeValue);
}

uint32_t MAC_WRP_GetSecondsCounter(void)
{
    /* Call lower layer function */
    return MAC_COMMON_GetSecondsCounter();
}

bool MAC_WRP_TimeIsPastSeconds(int32_t timeValue)
{
    /* Call lower layer function */
    return MAC_COMMON_TimeIsPastSeconds(timeValue);
}

/*******************************************************************************
 End of File
*/
