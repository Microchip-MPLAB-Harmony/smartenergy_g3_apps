/*******************************************************************************
  G3 ADP Serialization Source File

  Company:
    Microchip Technology Inc.

  File Name:
    adp_serial.c

  Summary:
    G3 ADP Serialization Source File.

  Description:
    The G3 ADP Serialization allows to serialize the ADP and LBP API through
    USI interface in order to run the application on an external device. This
    file contains the implementation of the G3 ADP Serialization.
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
#include <string.h>
#include "adp_serial.h"
#include "configuration.h"
#include "lbp_dev.h"
#include "lbp_coord.h"
#include "stack/g3/mac/mac_wrapper/mac_wrapper.h"
#include "service/usi/srv_usi.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

typedef enum
{
    /* Generic messages */
    ADP_SERIAL_MSG_STATUS = 0,

    /* ADP access request messages */
    ADP_SERIAL_MSG_ADP_INITIALIZE = 10,
    ADP_SERIAL_MSG_ADP_DATA_REQUEST,
    ADP_SERIAL_MSG_ADP_DISCOVERY_REQUEST,
    ADP_SERIAL_MSG_ADP_NETWORK_START_REQUEST,
    ADP_SERIAL_MSG_ADP_NETWORK_JOIN_REQUEST,
    ADP_SERIAL_MSG_ADP_NETWORK_LEAVE_REQUEST,
    ADP_SERIAL_MSG_ADP_RESET_REQUEST,
    ADP_SERIAL_MSG_ADP_SET_REQUEST,
    ADP_SERIAL_MSG_ADP_GET_REQUEST,
    ADP_SERIAL_MSG_ADP_LBP_REQUEST,
    ADP_SERIAL_MSG_ADP_ROUTE_DISCOVERY_REQUEST,
    ADP_SERIAL_MSG_ADP_PATH_DISCOVERY_REQUEST,
    ADP_SERIAL_MSG_ADP_MAC_SET_REQUEST,
    ADP_SERIAL_MSG_ADP_MAC_GET_REQUEST,
    ADP_SERIAL_MSG_ADP_NO_IP_DATA_REQUEST,

    /* ADP response/indication messages */
    ADP_SERIAL_MSG_ADP_DATA_CONFIRM = 30,
    ADP_SERIAL_MSG_ADP_DATA_INDICATION,
    ADP_SERIAL_MSG_ADP_NETWORK_STATUS_INDICATION,
    ADP_SERIAL_MSG_ADP_DISCOVERY_CONFIRM,
    ADP_SERIAL_MSG_ADP_NETWORK_START_CONFIRM,
    ADP_SERIAL_MSG_ADP_NETWORK_JOIN_CONFIRM,
    ADP_SERIAL_MSG_ADP_NETWORK_LEAVE_CONFIRM,
    ADP_SERIAL_MSG_ADP_NETWORK_LEAVE_INDICATION,
    ADP_SERIAL_MSG_ADP_RESET_CONFIRM,
    ADP_SERIAL_MSG_ADP_SET_CONFIRM,
    ADP_SERIAL_MSG_ADP_GET_CONFIRM,
    ADP_SERIAL_MSG_ADP_LBP_CONFIRM,
    ADP_SERIAL_MSG_ADP_LBP_INDICATION,
    ADP_SERIAL_MSG_ADP_ROUTE_DISCOVERY_CONFIRM,
    ADP_SERIAL_MSG_ADP_PATH_DISCOVERY_CONFIRM,
    ADP_SERIAL_MSG_ADP_MAC_SET_CONFIRM,
    ADP_SERIAL_MSG_ADP_MAC_GET_CONFIRM,
    ADP_SERIAL_MSG_ADP_BUFFER_INDICATION,
    ADP_SERIAL_MSG_ADP_DISCOVERY_INDICATION,
    ADP_SERIAL_MSG_ADP_PREQ_INDICATION,
    ADP_SERIAL_MSG_ADP_UPD_NON_VOLATILE_DATA_INDICATION,
    ADP_SERIAL_MSG_ADP_ROUTE_NOT_FOUND_INDICATION,

    ADP_SERIAL_MSG_LBP_SET_REQUEST = 60,
    ADP_SERIAL_MSG_LBP_DEV_FORCE_REGISTER,
    ADP_SERIAL_MSG_LBP_COORD_KICK_DEVICE,
    ADP_SERIAL_MSG_LBP_COORD_REKEY,
    ADP_SERIAL_MSG_LBP_COORD_SET_REKEY_PHASE,
    ADP_SERIAL_MSG_LBP_COORD_ACTIVATE_NEW_KEY,
    ADP_SERIAL_MSG_LBP_COORD_SHORT_ADDRESS_ASSIGN,

    ADP_SERIAL_MSG_LBP_SET_CONFIRM = 70,
    ADP_SERIAL_MSG_LBP_COORD_JOIN_REQUEST_INDICATION,
    ADP_SERIAL_MSG_LBP_COORD_JOIN_COMPLETE_INDICATION,
    ADP_SERIAL_MSG_LBP_COORD_LEAVE_INDICATION,

} ADP_SERIAL_MSG_ID;

typedef enum
{
    ADP_SERIAL_STATUS_SUCCESS = 0,
    ADP_SERIAL_STATUS_NOT_ALLOWED,
    ADP_SERIAL_STATUS_UNKNOWN_COMMAND,
    ADP_SERIAL_STATUS_INVALID_PARAMETER

} ADP_SERIAL_STATUS;

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Data
// *****************************************************************************
// *****************************************************************************

/* USI handle for ADP Serialization */
static SRV_USI_HANDLE adpSerialUsiHandle;

/* Buffer to send commands through USI */
static uint8_t adpSerialRspBuffer[1536];

/* Coordinator flag */
static bool adpSerialCoord;

/* PLC ARIB band flag */
static bool adpSerialAribBand;

/* ADP initializing flag */
static bool adpSerialInitializing;

/* ADP Serial notifications */
static ADP_SERIAL_NOTIFICATIONS adpSerialNotifications;

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Functions
// *****************************************************************************
// *****************************************************************************

static void _memcpyToUsiEndianessUint32(uint8_t* pDst, uint8_t* pSrc)
{
    uint32_t aux;

    memcpy((uint8_t *) &aux, pSrc, 4);

    *pDst++ = (uint8_t) (aux >> 24);
    *pDst++ = (uint8_t) (aux >> 16);
    *pDst++ = (uint8_t) (aux >> 8);
    *pDst = (uint8_t) aux;
}

static void _memcpyToUsiEndianessUint16(uint8_t* pDst, uint8_t* pSrc)
{
    uint16_t aux;

    memcpy((uint8_t *) &aux, pSrc, 2);

    *pDst++ = (uint8_t) (aux >> 8);
    *pDst = (uint8_t) aux;
}

static void _memcpyFromUsiEndianessUint32(uint8_t* pDst, uint8_t* pSrc)
{
    uint32_t aux = 0;

    aux = (*pSrc++) << 24;
    aux += (*pSrc++) << 16;
    aux += (*pSrc++) << 8;
    aux += *pSrc;

    memcpy(pDst, (uint8_t *)&aux, 4);
}

static void _memcpyFromUsiEndianessUint16(uint8_t* pDst, uint8_t* pSrc)
{
    uint16_t aux = 0;

    aux += (*pSrc++) << 8;
    aux += *pSrc;

    memcpy(pDst, (uint8_t *)&aux, 2);
}

static void _StringifyMsgStatus(ADP_SERIAL_STATUS status, ADP_SERIAL_MSG_ID command)
{
    uint8_t serialRspLen = 0;

    /* Fill serial response buffer */
    adpSerialRspBuffer[serialRspLen++] = ADP_SERIAL_MSG_STATUS;
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) status;
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) command;

    /* Send through USI */
    SRV_USI_Send_Message(adpSerialUsiHandle, SRV_USI_PROT_ID_ADP_G3, adpSerialRspBuffer, serialRspLen);
}

static void _StringifyBufferIndication(bool bufferReady)
{
    uint8_t serialRspLen = 0;

    /* Fill serial response buffer */
    adpSerialRspBuffer[serialRspLen++] = ADP_SERIAL_MSG_ADP_BUFFER_INDICATION;
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) (bufferReady);

    /* Send through USI */
    SRV_USI_Send_Message(adpSerialUsiHandle, SRV_USI_PROT_ID_ADP_G3, adpSerialRspBuffer, serialRspLen);
}

static void _StringifyPreqIndication(void)
{
    uint8_t serialRspLen = 0;

    /* Fill serial response buffer */
    adpSerialRspBuffer[serialRspLen++] = ADP_SERIAL_MSG_ADP_PREQ_INDICATION;

    /* Send through USI */
    SRV_USI_Send_Message(adpSerialUsiHandle, SRV_USI_PROT_ID_ADP_G3, adpSerialRspBuffer, serialRspLen);
}

static void _StoreNonVolatileDataIndication(ADP_NON_VOLATILE_DATA_IND_PARAMS* pNonVolatileDataInd)
{
    if (adpSerialNotifications.nonVolatileDataIndication != NULL)
    {
        adpSerialNotifications.nonVolatileDataIndication(pNonVolatileDataInd);
    }
}

static void _StringifyRouteNotFoundIndication(ADP_ROUTE_NOT_FOUND_IND_PARAMS* pRouteNotFoundInd)
{
    uint8_t serialRspLen = 0;

    /* Fill serial response buffer */
    adpSerialRspBuffer[serialRspLen++] = ADP_SERIAL_MSG_ADP_ROUTE_NOT_FOUND_INDICATION;
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) (pRouteNotFoundInd->srcAddr >> 8);
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) pRouteNotFoundInd->srcAddr;
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) (pRouteNotFoundInd->destAddr >> 8);
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) pRouteNotFoundInd->destAddr;
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) (pRouteNotFoundInd->nextHopAddr >> 8);
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) pRouteNotFoundInd->nextHopAddr;
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) (pRouteNotFoundInd->previousHopAddr >> 8);
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) pRouteNotFoundInd->previousHopAddr;
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) (pRouteNotFoundInd->routeCost >> 8);
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) pRouteNotFoundInd->routeCost;
    adpSerialRspBuffer[serialRspLen++] = pRouteNotFoundInd->hopCount;
    adpSerialRspBuffer[serialRspLen++] = pRouteNotFoundInd->weakLinkCount;
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) (pRouteNotFoundInd->routeJustBroken);
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) (pRouteNotFoundInd->compressedHeader);
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) (pRouteNotFoundInd->nsduLength >> 8);
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) pRouteNotFoundInd->nsduLength;
    memcpy(&adpSerialRspBuffer[serialRspLen], pRouteNotFoundInd->pNsdu, pRouteNotFoundInd->nsduLength);
    serialRspLen += pRouteNotFoundInd->nsduLength;

    /* Send through USI */
    SRV_USI_Send_Message(adpSerialUsiHandle, SRV_USI_PROT_ID_ADP_G3, adpSerialRspBuffer, serialRspLen);
}

static void _StringifyDataConfirm(ADP_DATA_CFM_PARAMS* pDataCfm)
{
    uint8_t serialRspLen = 0;

    /* Fill serial response buffer */
    adpSerialRspBuffer[serialRspLen++] = ADP_SERIAL_MSG_ADP_DATA_CONFIRM;
    adpSerialRspBuffer[serialRspLen++] = pDataCfm->status;
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) pDataCfm->nsduHandle;

    /* Send through USI */
    SRV_USI_Send_Message(adpSerialUsiHandle, SRV_USI_PROT_ID_ADP_G3, adpSerialRspBuffer, serialRspLen);
}

static void _StringifyDataIndication(ADP_DATA_IND_PARAMS* pDataInd)
{
    uint16_t serialRspLen = 0;

    /* Fill serial response buffer */
    adpSerialRspBuffer[serialRspLen++] = ADP_SERIAL_MSG_ADP_DATA_INDICATION;
    adpSerialRspBuffer[serialRspLen++] = pDataInd->linkQualityIndicator;
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) (pDataInd->nsduLength >> 8);
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) pDataInd->nsduLength;
    memcpy(&adpSerialRspBuffer[serialRspLen], pDataInd->pNsdu, pDataInd->nsduLength);
    serialRspLen += pDataInd->nsduLength;

    /* Send through USI */
    SRV_USI_Send_Message(adpSerialUsiHandle, SRV_USI_PROT_ID_ADP_G3, adpSerialRspBuffer, serialRspLen);
}

static void _StringifyNetworkStatusIndication(ADP_NETWORK_STATUS_IND_PARAMS* pNetworkStatusInd)
{
    uint8_t serialRspLen = 0;

    /* Fill serial response buffer */
    adpSerialRspBuffer[serialRspLen++] = ADP_SERIAL_MSG_ADP_NETWORK_STATUS_INDICATION;
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) (pNetworkStatusInd->panId >> 8);
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) pNetworkStatusInd->panId;
    adpSerialRspBuffer[serialRspLen++] = pNetworkStatusInd->srcDeviceAddress.addrSize;
    if (pNetworkStatusInd->srcDeviceAddress.addrSize == ADP_ADDRESS_16BITS)
    {
        adpSerialRspBuffer[serialRspLen++] = (uint8_t) (pNetworkStatusInd->srcDeviceAddress.shortAddr >> 8);
        adpSerialRspBuffer[serialRspLen++] = (uint8_t) pNetworkStatusInd->srcDeviceAddress.shortAddr;
    }
    else
    {
        /* ADP_ADDRESS_64BITS */
        memcpy(&adpSerialRspBuffer[serialRspLen], pNetworkStatusInd->srcDeviceAddress.extendedAddr.value,
                pNetworkStatusInd->srcDeviceAddress.addrSize);
        serialRspLen += pNetworkStatusInd->srcDeviceAddress.addrSize;
    }

    adpSerialRspBuffer[serialRspLen++] = pNetworkStatusInd->dstDeviceAddress.addrSize;
    if (pNetworkStatusInd->dstDeviceAddress.addrSize == ADP_ADDRESS_16BITS)
    {
        adpSerialRspBuffer[serialRspLen++] = (uint8_t) (pNetworkStatusInd->dstDeviceAddress.shortAddr >> 8);
        adpSerialRspBuffer[serialRspLen++] = (uint8_t) pNetworkStatusInd->dstDeviceAddress.shortAddr;
    }
    else
    {
        /* ADP_ADDRESS_64BITS */
        memcpy(&adpSerialRspBuffer[serialRspLen], pNetworkStatusInd->dstDeviceAddress.extendedAddr.value,
                pNetworkStatusInd->dstDeviceAddress.addrSize);
        serialRspLen += pNetworkStatusInd->dstDeviceAddress.addrSize;
    }

    adpSerialRspBuffer[serialRspLen++] = pNetworkStatusInd->status;
    adpSerialRspBuffer[serialRspLen++] = pNetworkStatusInd->securityLevel;
    adpSerialRspBuffer[serialRspLen++] = pNetworkStatusInd->keyIndex;
    adpSerialRspBuffer[serialRspLen++] = pNetworkStatusInd->mediaType;

    /* Send through USI */
    SRV_USI_Send_Message(adpSerialUsiHandle, SRV_USI_PROT_ID_ADP_G3, adpSerialRspBuffer, serialRspLen);
}

static void _StringifyDiscoveryConfirm(uint8_t status)
{
    uint8_t serialRspLen = 0;

    /* Fill serial response buffer */
    adpSerialRspBuffer[serialRspLen++] = ADP_SERIAL_MSG_ADP_DISCOVERY_CONFIRM;
    adpSerialRspBuffer[serialRspLen++] = status;

    /* Send through USI */
    SRV_USI_Send_Message(adpSerialUsiHandle, SRV_USI_PROT_ID_ADP_G3, adpSerialRspBuffer, serialRspLen);
}

static void _StringifyDiscoveryIndication(ADP_PAN_DESCRIPTOR* pPanDescriptor)
{
    uint8_t serialRspLen = 0;

    /* Fill serial response buffer */
    adpSerialRspBuffer[serialRspLen++] = ADP_SERIAL_MSG_ADP_DISCOVERY_INDICATION;
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) (pPanDescriptor->panId >> 8);
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) pPanDescriptor->panId;
    adpSerialRspBuffer[serialRspLen++] = pPanDescriptor->linkQuality;
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) (pPanDescriptor->lbaAddress >> 8);
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) pPanDescriptor->lbaAddress;
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) (pPanDescriptor->rcCoord >> 8);
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) pPanDescriptor->rcCoord;    
    adpSerialRspBuffer[serialRspLen++] = pPanDescriptor->mediaType;

    /* Send through USI */
    SRV_USI_Send_Message(adpSerialUsiHandle, SRV_USI_PROT_ID_ADP_G3, adpSerialRspBuffer, serialRspLen);
}

static void _StringifyNetworkStartConfirm(uint8_t status)
{
    uint8_t serialRspLen = 0;

    /* Fill serial response buffer */
    adpSerialRspBuffer[serialRspLen++] = ADP_SERIAL_MSG_ADP_NETWORK_START_CONFIRM;
    adpSerialRspBuffer[serialRspLen++] = status;

    /* Send through USI */
    SRV_USI_Send_Message(adpSerialUsiHandle, SRV_USI_PROT_ID_ADP_G3, adpSerialRspBuffer, serialRspLen);
}

static void _StringifyNetworkJoinConfirm(LBP_ADP_NETWORK_JOIN_CFM_PARAMS* pNetworkJoinCfm)
{
    uint8_t serialRspLen = 0;

    /* Fill serial response buffer */
    adpSerialRspBuffer[serialRspLen++] = ADP_SERIAL_MSG_ADP_NETWORK_JOIN_CONFIRM;
    adpSerialRspBuffer[serialRspLen++] = pNetworkJoinCfm->status;
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) (pNetworkJoinCfm->networkAddress >> 8);
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) pNetworkJoinCfm->networkAddress;
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) (pNetworkJoinCfm->panId >> 8);
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) pNetworkJoinCfm->panId;

    /* Send through USI */
    SRV_USI_Send_Message(adpSerialUsiHandle, SRV_USI_PROT_ID_ADP_G3, adpSerialRspBuffer, serialRspLen);
}

static void _StringifyNetworkLeaveIndication(void)
{
    uint8_t serialRspLen = 0;

    /* Fill serial response buffer */
    adpSerialRspBuffer[serialRspLen++] = ADP_SERIAL_MSG_ADP_NETWORK_LEAVE_INDICATION;

    /* Send through USI */
    SRV_USI_Send_Message(adpSerialUsiHandle, SRV_USI_PROT_ID_ADP_G3, adpSerialRspBuffer, serialRspLen);
}

static void _StringifyNetworkLeaveConfirm(uint8_t status)
{
    uint8_t serialRspLen = 0;

    /* Fill serial response buffer */
    adpSerialRspBuffer[serialRspLen++] = ADP_SERIAL_MSG_ADP_NETWORK_LEAVE_CONFIRM;
    adpSerialRspBuffer[serialRspLen++] = status;

    /* Send through USI */
    SRV_USI_Send_Message(adpSerialUsiHandle, SRV_USI_PROT_ID_ADP_G3, adpSerialRspBuffer, serialRspLen);
}

static void _StringifyResetConfirm(uint8_t status)
{
    uint8_t serialRspLen = 0;

    /* Fill serial response buffer */
    adpSerialRspBuffer[serialRspLen++] = ADP_SERIAL_MSG_ADP_RESET_CONFIRM;
    adpSerialRspBuffer[serialRspLen++] = status;

    /* Send through USI */
    SRV_USI_Send_Message(adpSerialUsiHandle, SRV_USI_PROT_ID_ADP_G3, adpSerialRspBuffer, serialRspLen);
}

static void _StringifySetConfirm(ADP_SET_CFM_PARAMS* pSetCfm)
{
    uint8_t serialRspLen = 0;

    /* Fill serial response buffer */
    adpSerialRspBuffer[serialRspLen++] = ADP_SERIAL_MSG_ADP_SET_CONFIRM;
    adpSerialRspBuffer[serialRspLen++] = pSetCfm->status;
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) (pSetCfm->attributeId >> 24);
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) (pSetCfm->attributeId >> 16);
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) (pSetCfm->attributeId >> 8);
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) pSetCfm->attributeId;
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) (pSetCfm->attributeIndex >> 8);
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) pSetCfm->attributeIndex;

    /* Send through USI */
    SRV_USI_Send_Message(adpSerialUsiHandle, SRV_USI_PROT_ID_ADP_G3, adpSerialRspBuffer, serialRspLen);
}

static void _StringifyGetConfirm(ADP_GET_CFM_PARAMS* pGetCfm)
{
    uint8_t prefixLenghBytes, contextLength;
    uint8_t serialRspLen = 0;

    /* Fill serial response buffer */
    adpSerialRspBuffer[serialRspLen++] = ADP_SERIAL_MSG_ADP_GET_CONFIRM;
    adpSerialRspBuffer[serialRspLen++] = pGetCfm->status;
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) (pGetCfm->attributeId >> 24);
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) (pGetCfm->attributeId >> 16);
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) (pGetCfm->attributeId >> 8);
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) pGetCfm->attributeId;
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) (pGetCfm->attributeIndex >> 8);
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) pGetCfm->attributeIndex;
    adpSerialRspBuffer[serialRspLen++] = pGetCfm->attributeLength;

    if (pGetCfm->status == G3_SUCCESS)
    {
        switch (pGetCfm->attributeId)
        {
            /* 8-bit IBs */
            case ADP_IB_SECURITY_LEVEL:
            case ADP_IB_METRIC_TYPE:
            case ADP_IB_LOW_LQI_VALUE:
            case ADP_IB_HIGH_LQI_VALUE:
            case ADP_IB_RREP_WAIT:
            case ADP_IB_RLC_ENABLED:
            case ADP_IB_ADD_REV_LINK_COST:
            case ADP_IB_UNICAST_RREQ_GEN_ENABLE:
            case ADP_IB_MAX_HOPS:
            case ADP_IB_DEVICE_TYPE:
            case ADP_IB_NET_TRAVERSAL_TIME:
            case ADP_IB_KR:
            case ADP_IB_KM:
            case ADP_IB_KC:
            case ADP_IB_KQ:
            case ADP_IB_KH:
            case ADP_IB_RREQ_RETRIES:
            case ADP_IB_RREQ_WAIT:
            case ADP_IB_WEAK_LQI_VALUE:
            case ADP_IB_KRT:
            case ADP_IB_PATH_DISCOVERY_TIME:
            case ADP_IB_ACTIVE_KEY_INDEX:
            case ADP_IB_DEFAULT_COORD_ROUTE_ENABLED:
            case ADP_IB_DISABLE_DEFAULT_ROUTING:
            case ADP_IB_RREQ_JITTER_LOW_LQI:
            case ADP_IB_RREQ_JITTER_HIGH_LQI:
            case ADP_IB_RREQ_JITTER_LOW_LQI_RF:
            case ADP_IB_RREQ_JITTER_HIGH_LQI_RF:
            case ADP_IB_TRICKLE_DATA_ENABLED:
            case ADP_IB_TRICKLE_LQI_THRESHOLD_LOW:
            case ADP_IB_TRICKLE_LQI_THRESHOLD_LOW_RF:
            case ADP_IB_TRICKLE_LQI_THRESHOLD_HIGH:
            case ADP_IB_TRICKLE_LQI_THRESHOLD_HIGH_RF:
            case ADP_IB_TRICKLE_STEP:
            case ADP_IB_TRICKLE_MAX_KI:
            case ADP_IB_TRICKLE_ADAPTIVE_I_MIN:
            case ADP_IB_TRICKLE_ADAPTIVE_KI:
            case ADP_IB_CLUSTER_TRICKLE_ENABLED:
            case ADP_IB_CLUSTER_MIN_LQI:
            case ADP_IB_CLUSTER_MIN_LQI_RF:
            case ADP_IB_CLUSTER_TRICKLE_K:
            case ADP_IB_CLUSTER_TRICKLE_K_RF:
            case ADP_IB_CLUSTER_RREQ_ROUTE_COST_DEVIATION:
            case ADP_IB_MANUF_IPV6_HEADER_COMPRESSION:
            case ADP_IB_MANUF_BROADCAST_SEQUENCE_NUMBER:
            case ADP_IB_MANUF_FORCED_NO_ACK_REQUEST:
            case ADP_IB_MANUF_LQI_TO_COORD:
            case ADP_IB_MANUF_BROADCAST_ROUTE_ALL:
            case ADP_IB_MANUF_MAX_REPAIR_RESEND_ATTEMPTS:
            case ADP_IB_MANUF_DISABLE_AUTO_RREQ:
            case ADP_IB_MANUF_GET_BAND_CONTEXT_TONES:
            case ADP_IB_MANUF_UPDATE_NON_VOLATILE_DATA:
            case ADP_IB_MANUF_DYNAMIC_FRAGMENT_DELAY_ENABLED:
            case ADP_IB_MANUF_HYBRID_PROFILE:
            case ADP_IB_MANUF_LAST_PHASEDIFF:
            case ADP_IB_LOW_LQI_VALUE_RF:
            case ADP_IB_HIGH_LQI_VALUE_RF:
            case ADP_IB_KQ_RF:
            case ADP_IB_KH_RF:
            case ADP_IB_KRT_RF:
            case ADP_IB_KDC_RF:
            case ADP_IB_WEAK_LQI_VALUE_RF:
            case ADP_IB_USE_BACKUP_MEDIA:
            case ADP_IB_LAST_GASP:
                adpSerialRspBuffer[serialRspLen++] = pGetCfm->attributeValue[0];
                break;

            /* 16-bit IBs */
            case ADP_IB_BROADCAST_LOG_TABLE_ENTRY_TTL:
            case ADP_IB_COORD_SHORT_ADDRESS:
            case ADP_IB_ROUTING_TABLE_ENTRY_TTL:
            case ADP_IB_BLACKLIST_TABLE_ENTRY_TTL:
            case ADP_IB_MAX_JOIN_WAIT_TIME:
            case ADP_IB_DELAY_LOW_LQI:
            case ADP_IB_DELAY_HIGH_LQI:
            case ADP_IB_DELAY_LOW_LQI_RF:
            case ADP_IB_DELAY_HIGH_LQI_RF:
            case ADP_IB_DESTINATION_ADDRESS_SET:
            case ADP_IB_TRICKLE_I_MIN:
            case ADP_IB_CLUSTER_TRICKLE_I:
            case ADP_IB_CLUSTER_TRICKLE_I_RF:
            case ADP_IB_MANUF_REASSEMBY_TIMER:
            case ADP_IB_MANUF_DATAGRAM_TAG:
            case ADP_IB_MANUF_DISCOVER_SEQUENCE_NUMBER: /* ADP_IB_MANUF_DISCOVER_ROUTE_GLOBAL_SEQ_NUM */
            case ADP_IB_MANUF_CIRCULAR_ROUTES_DETECTED:
            case ADP_IB_MANUF_LAST_CIRCULAR_ROUTE_ADDRESS:
            case ADP_IB_MANUF_IPV6_ULA_DEST_SHORT_ADDRESS:
            case ADP_IB_MANUF_ALL_NEIGHBORS_BLACKLISTED_COUNT:
            case ADP_IB_MANUF_QUEUED_ENTRIES_REMOVED_TIMEOUT_COUNT:
            case ADP_IB_MANUF_QUEUED_ENTRIES_REMOVED_ROUTE_ERROR_COUNT:
            case ADP_IB_MANUF_PENDING_DATA_IND_SHORT_ADDRESS:
            case ADP_IB_MANUF_FRAGMENT_DELAY:
            case ADP_IB_MANUF_DYNAMIC_FRAGMENT_DELAY_FACTOR:
            case ADP_IB_MANUF_BLACKLIST_TABLE_COUNT:
            case ADP_IB_MANUF_BROADCAST_LOG_TABLE_COUNT:
            case ADP_IB_MANUF_CONTEXT_INFORMATION_TABLE_COUNT:
            case ADP_IB_MANUF_GROUP_TABLE_COUNT:
            case ADP_IB_MANUF_PAN_ID:
            case ADP_IB_MANUF_SHORT_ADDRESS:
                _memcpyToUsiEndianessUint16(&adpSerialRspBuffer[serialRspLen], pGetCfm->attributeValue);
                serialRspLen += 2;
                break;

            /* 32-bit IBs */
            case ADP_IB_MANUF_ROUTING_TABLE_COUNT:
                _memcpyToUsiEndianessUint32(&adpSerialRspBuffer[serialRspLen], pGetCfm->attributeValue);
                serialRspLen += 4;
                break;

            /* Tables and lists */
            case ADP_IB_PREFIX_TABLE:
                /* prefixLength */
                adpSerialRspBuffer[serialRspLen++] = pGetCfm->attributeValue[0];
                /* onLinkFlag */
                adpSerialRspBuffer[serialRspLen++] = pGetCfm->attributeValue[1];
                /* autonomousAddressConfigurationFlag */
                adpSerialRspBuffer[serialRspLen++] = pGetCfm->attributeValue[2];
                /* validTime */
                _memcpyToUsiEndianessUint32(&adpSerialRspBuffer[serialRspLen], &pGetCfm->attributeValue[3]); 
                serialRspLen += 4;
                /* preferredTime */
                _memcpyToUsiEndianessUint32(&adpSerialRspBuffer[serialRspLen], &pGetCfm->attributeValue[7]);
                serialRspLen += 4;
                /* prefix */
                prefixLenghBytes = pGetCfm->attributeLength - 11;
                memcpy(&adpSerialRspBuffer[serialRspLen], &pGetCfm->attributeValue[11], prefixLenghBytes);
                serialRspLen += prefixLenghBytes;
                break;

            case ADP_IB_CONTEXT_INFORMATION_TABLE:
                /* validTime */
                _memcpyToUsiEndianessUint16(adpSerialRspBuffer, pGetCfm->attributeValue);
                serialRspLen += 2;
                /* validForCompression */
                adpSerialRspBuffer[serialRspLen++] = pGetCfm->attributeValue[2];
                /* bitsContextLength */
                adpSerialRspBuffer[serialRspLen++] = pGetCfm->attributeValue[3];
                /* context */
                contextLength = pGetCfm->attributeLength - 4;
                memcpy(&adpSerialRspBuffer[serialRspLen], &pGetCfm->attributeValue[4], contextLength);
                serialRspLen += contextLength;
                break;

            case ADP_IB_BROADCAST_LOG_TABLE:
                /* srcAddr */
                _memcpyToUsiEndianessUint16(adpSerialRspBuffer, pGetCfm->attributeValue);
                serialRspLen += 2;
                /* sequenceNumber */
                adpSerialRspBuffer[serialRspLen++] = pGetCfm->attributeValue[2];
                /* validTime */
                _memcpyToUsiEndianessUint16(&adpSerialRspBuffer[serialRspLen], &pGetCfm->attributeValue[3]);
                serialRspLen += 2;
                break;

            case ADP_IB_ROUTING_TABLE:
            case ADP_IB_MANUF_ROUTING_TABLE_ELEMENT:
                /* dstAddr */
                _memcpyToUsiEndianessUint16(adpSerialRspBuffer, pGetCfm->attributeValue);
                serialRspLen += 2;
                /* nextHopAddr */
                _memcpyToUsiEndianessUint16(&adpSerialRspBuffer[serialRspLen], &pGetCfm->attributeValue[2]);
                serialRspLen += 2;
                /* routeCost */
                _memcpyToUsiEndianessUint16(&adpSerialRspBuffer[serialRspLen], &pGetCfm->attributeValue[4]);
                serialRspLen += 2;
                /* hopCount, weakLinkCount */
                adpSerialRspBuffer[serialRspLen++] = pGetCfm->attributeValue[6];
                /* mediaType */
                adpSerialRspBuffer[serialRspLen++] = pGetCfm->attributeValue[7];
                /* validTime */
                _memcpyToUsiEndianessUint16(&adpSerialRspBuffer[serialRspLen], &pGetCfm->attributeValue[8]);
                serialRspLen += 2;
                break;

            case ADP_IB_GROUP_TABLE:
                /* groupAddress */
                _memcpyToUsiEndianessUint16(adpSerialRspBuffer, pGetCfm->attributeValue);
                serialRspLen += 2;
                /* valid */
                adpSerialRspBuffer[serialRspLen++] = pGetCfm->attributeValue[2];
                break;

            case ADP_IB_SOFT_VERSION:
            case ADP_IB_MANUF_ADP_INTERNAL_VERSION:
                /* Version */
                memcpy(adpSerialRspBuffer, pGetCfm->attributeValue, 6);
                serialRspLen += 6;
                break;

            case ADP_IB_BLACKLIST_TABLE:
                /* addr */
                _memcpyToUsiEndianessUint16(adpSerialRspBuffer, pGetCfm->attributeValue);
                serialRspLen += 2;
                /* mediaType */
                adpSerialRspBuffer[serialRspLen++] = pGetCfm->attributeValue[2];
                /* validTime */
                _memcpyToUsiEndianessUint16(&adpSerialRspBuffer[serialRspLen], &pGetCfm->attributeValue[3]);
                serialRspLen += 2;
                break;

            case ADP_IB_SNIFFER_MODE:
                /* TODO */
                break;

            default:
                break;
        }
    }

    /* Send through USI */
    SRV_USI_Send_Message(adpSerialUsiHandle, SRV_USI_PROT_ID_ADP_G3, adpSerialRspBuffer, serialRspLen);
}

static void _StringifyRouteDiscoveryConfirm(uint8_t status)
{
    uint8_t serialRspLen = 0;

    /* Fill serial response buffer */
    adpSerialRspBuffer[serialRspLen++] = ADP_SERIAL_MSG_ADP_ROUTE_DISCOVERY_CONFIRM;
    adpSerialRspBuffer[serialRspLen++] = status;

    /* Send through USI */
    SRV_USI_Send_Message(adpSerialUsiHandle, SRV_USI_PROT_ID_ADP_G3, adpSerialRspBuffer, serialRspLen);
}

static void _StringifyPathDiscoveryConfirm(ADP_PATH_DISCOVERY_CFM_PARAMS* pPathDiscoveryCfm)
{
    uint8_t index;
    uint8_t serialRspLen = 0;    

    /* Fill serial response buffer */
    adpSerialRspBuffer[serialRspLen++] = ADP_SERIAL_MSG_ADP_PATH_DISCOVERY_CONFIRM;
    adpSerialRspBuffer[serialRspLen++] = pPathDiscoveryCfm->status;
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) (pPathDiscoveryCfm->dstAddr >> 8);
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) pPathDiscoveryCfm->dstAddr;
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) (pPathDiscoveryCfm->origAddr >> 8);
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) pPathDiscoveryCfm->origAddr;
    adpSerialRspBuffer[serialRspLen++] = pPathDiscoveryCfm->metricType;
    adpSerialRspBuffer[serialRspLen++] = pPathDiscoveryCfm->forwardHopsCount;
    adpSerialRspBuffer[serialRspLen++] = pPathDiscoveryCfm->reverseHopsCount;

    for (index = 0; index < pPathDiscoveryCfm->forwardHopsCount; index++)
    {
        adpSerialRspBuffer[serialRspLen++] = (uint8_t) (pPathDiscoveryCfm->forwardPath[index].hopAddress >> 8);
        adpSerialRspBuffer[serialRspLen++] = (uint8_t) pPathDiscoveryCfm->forwardPath[index].hopAddress;
        adpSerialRspBuffer[serialRspLen++] = pPathDiscoveryCfm->forwardPath[index].mns;
        adpSerialRspBuffer[serialRspLen++] = pPathDiscoveryCfm->forwardPath[index].linkCost;
        adpSerialRspBuffer[serialRspLen++] = pPathDiscoveryCfm->forwardPath[index].phaseDiff;
        adpSerialRspBuffer[serialRspLen++] = pPathDiscoveryCfm->forwardPath[index].mrx;
        adpSerialRspBuffer[serialRspLen++] = pPathDiscoveryCfm->forwardPath[index].mtx;
    }

    for (index = 0; index < pPathDiscoveryCfm->reverseHopsCount; index++)
    {
        adpSerialRspBuffer[serialRspLen++] = (uint8_t) (pPathDiscoveryCfm->reversePath[index].hopAddress >> 8);
        adpSerialRspBuffer[serialRspLen++] = (uint8_t) pPathDiscoveryCfm->reversePath[index].hopAddress;
        adpSerialRspBuffer[serialRspLen++] = pPathDiscoveryCfm->reversePath[index].mns;
        adpSerialRspBuffer[serialRspLen++] = pPathDiscoveryCfm->reversePath[index].linkCost;
        adpSerialRspBuffer[serialRspLen++] = pPathDiscoveryCfm->reversePath[index].phaseDiff;
        adpSerialRspBuffer[serialRspLen++] = pPathDiscoveryCfm->reversePath[index].mrx;
        adpSerialRspBuffer[serialRspLen++] = pPathDiscoveryCfm->reversePath[index].mtx;
    }

    /* Send through USI */
    SRV_USI_Send_Message(adpSerialUsiHandle, SRV_USI_PROT_ID_ADP_G3, adpSerialRspBuffer, serialRspLen);
}

static void _StringifyLbpCoordJoinRequestIndication(uint8_t* pLbdAddress)
{
    uint8_t serialRspLen = 0;

    /* Fill serial response buffer */
    adpSerialRspBuffer[serialRspLen++] = ADP_SERIAL_MSG_LBP_COORD_JOIN_REQUEST_INDICATION;
    memcpy(&adpSerialRspBuffer[serialRspLen], pLbdAddress, 8);
    serialRspLen += 8;

    /* Send through USI */
    SRV_USI_Send_Message(adpSerialUsiHandle, SRV_USI_PROT_ID_ADP_G3, adpSerialRspBuffer, serialRspLen);
}

static void _StringifyLbpCoordJoinCompleteIndication(uint8_t* pLbdAddress, uint16_t assignedAddress)
{
    uint8_t serialRspLen = 0;

    /* Fill serial response buffer */
    adpSerialRspBuffer[serialRspLen++] = ADP_SERIAL_MSG_LBP_COORD_JOIN_COMPLETE_INDICATION;
    memcpy(&adpSerialRspBuffer[serialRspLen], pLbdAddress, 8);
    serialRspLen += 8;
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) (assignedAddress >> 8);
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) assignedAddress;

    /* Send through USI */
    SRV_USI_Send_Message(adpSerialUsiHandle, SRV_USI_PROT_ID_ADP_G3, adpSerialRspBuffer, serialRspLen);
}

static void _StringifyLbpCoordLeaveIndication(uint16_t networkAddress)
{
    uint8_t serialRspLen = 0;

    /* Fill serial response buffer */
    adpSerialRspBuffer[serialRspLen++] = ADP_SERIAL_MSG_LBP_COORD_LEAVE_INDICATION;
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) (networkAddress >> 8);
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) networkAddress;

    /* Send through USI */
    SRV_USI_Send_Message(adpSerialUsiHandle, SRV_USI_PROT_ID_ADP_G3, adpSerialRspBuffer, serialRspLen);
}

static ADP_SERIAL_STATUS _ParseInitialize(uint8_t* pData)
{
    ADP_DATA_NOTIFICATIONS adpDataNotifications;
    ADP_MANAGEMENT_NOTIFICATIONS adpMngNotifications;
    LBP_NOTIFICATIONS_DEV lbpDevNotifications;
    LBP_NOTIFICATIONS_COORD lbpCoordNotifications;
    ADP_PLC_BAND band;

    /* Parse initialize message */
    band = (ADP_PLC_BAND) pData[0];
    adpSerialAribBand = (bool) (band == ADP_BAND_ARIB);
    adpSerialCoord = (bool) (pData[1] != 0);

    /* Open ADP */
    ADP_Open(band);

    /* Set ADP Data callbacks */
    adpDataNotifications.dataConfirm = _StringifyDataConfirm;
    adpDataNotifications.dataIndication = _StringifyDataIndication;
    adpDataNotifications.bufferIndication = _StringifyBufferIndication;
    ADP_SetDataNotifications(&adpDataNotifications);

    /* Set ADP Management callbacks */
    adpMngNotifications.discoveryConfirm = _StringifyDiscoveryConfirm;
    adpMngNotifications.discoveryIndication = _StringifyDiscoveryIndication;
    adpMngNotifications.networkStartConfirm = _StringifyNetworkStartConfirm;
    adpMngNotifications.resetConfirm = _StringifyResetConfirm;
    adpMngNotifications.setConfirm = _StringifySetConfirm;
    adpMngNotifications.getConfirm = _StringifyGetConfirm;
    adpMngNotifications.macSetConfirm = NULL;
    adpMngNotifications.getConfirm = _StringifyGetConfirm;
    adpMngNotifications.macGetConfirm = NULL;
    adpMngNotifications.routeDiscoveryConfirm = _StringifyRouteDiscoveryConfirm;
    adpMngNotifications.pathDiscoveryConfirm = _StringifyPathDiscoveryConfirm;
    adpMngNotifications.networkStatusIndication = _StringifyNetworkStatusIndication;
    adpMngNotifications.preqIndication = _StringifyPreqIndication;
    adpMngNotifications.nonVolatileDataIndication = _StoreNonVolatileDataIndication;
    adpMngNotifications.routeNotFoundIndication = _StringifyRouteNotFoundIndication;
    ADP_SetManagementNotifications(&adpMngNotifications);

    /* Initialize LBP in device or coordinator mode */
    if (adpSerialCoord == true)
    {
        LBP_InitCoord(adpSerialAribBand);
        lbpCoordNotifications.joinRequestIndication = _StringifyLbpCoordJoinRequestIndication;
        lbpCoordNotifications.joinCompleteIndication = _StringifyLbpCoordJoinCompleteIndication;
        lbpCoordNotifications.leaveIndication = _StringifyLbpCoordLeaveIndication;
        LBP_SetNotificationsCoord(&lbpCoordNotifications);
    }
    else
    {
        LBP_InitDev();
        lbpDevNotifications.adpNetworkJoinConfirm = _StringifyNetworkJoinConfirm;
        lbpDevNotifications.adpNetworkLeaveConfirm = _StringifyNetworkLeaveConfirm;
        lbpDevNotifications.adpNetworkLeaveIndication = _StringifyNetworkLeaveIndication;
        LBP_SetNotificationsDev(&lbpDevNotifications);
    }

    adpSerialInitializing = true;
    return ADP_SERIAL_STATUS_SUCCESS;
}

static ADP_SERIAL_STATUS _ParseDataRequest(uint8_t* pData)
{
    const uint8_t *nsdu;
    uintptr_t nsduHandle;
    uint16_t nsduLength;
    uint8_t qualityOfService;
    bool discoverRoute;

    if (ADP_Status() < ADP_STATUS_READY)
    {
        /* ADP not initialized */
        return ADP_SERIAL_STATUS_NOT_ALLOWED;
    }

    /* Parse data request message */
    nsduHandle = (uintptr_t) *pData++;
    discoverRoute = (bool) *pData++;
    qualityOfService = *pData++;
    nsduLength = ((uint16_t) *pData++) << 8;
    nsduLength += (uint16_t) *pData++;
    nsdu = pData;

    /* Send data request to ADP */
    ADP_DataRequest(nsduLength, nsdu, nsduHandle, discoverRoute, qualityOfService);

    return ADP_SERIAL_STATUS_SUCCESS;
}

static ADP_SERIAL_STATUS _ParseNoIPDataRequest(uint8_t* pData)
{
    const uint8_t *nsdu;
    uintptr_t nsduHandle;
    uint16_t dstAddr, nsduLength;
    uint8_t qualityOfService;
    bool discoverRoute;    

    if (ADP_Status() < ADP_STATUS_READY)
    {
        /* ADP not initialized */
        return ADP_SERIAL_STATUS_NOT_ALLOWED;
    }

    /* Parse no IP data request message */
    dstAddr = ((uint16_t) *pData++) << 8;
    dstAddr += (uint16_t) *pData++;
    nsduHandle = (uintptr_t) *pData++;
    discoverRoute = (bool) *pData++;
    qualityOfService = *pData++;
    nsduLength = ((uint16_t) *pData++) << 8;
    nsduLength += (uint16_t) *pData++;
    nsdu = pData;

    /* Send no IP data request to ADP */
    ADP_NoIPDataRequest(nsduLength, nsdu, dstAddr, nsduHandle, discoverRoute, qualityOfService);

    return ADP_SERIAL_STATUS_SUCCESS;
}

static ADP_SERIAL_STATUS _ParseDiscoveryRequest(uint8_t* pData)
{
    if (ADP_Status() < ADP_STATUS_READY)
    {
        /* ADP not initialized */
        return ADP_SERIAL_STATUS_NOT_ALLOWED;
    }

    /* Send discovery request to ADP */
    ADP_DiscoveryRequest(pData[0]);

    return ADP_SERIAL_STATUS_SUCCESS;
}

static ADP_SERIAL_STATUS _ParseNetworkStartRequest(uint8_t* pData)
{
    uint16_t panId;

    if ((ADP_Status() < ADP_STATUS_READY) || (adpSerialCoord == false))
    {
        /* ADP not initialized or LBP initialized as device */
        return ADP_SERIAL_STATUS_NOT_ALLOWED;
    }

    /* Parse network start request message */
    panId = ((uint16_t) *pData++) << 8;
    panId += (uint16_t) *pData;

    /* Send network start request to ADP */
    ADP_NetworkStartRequest(panId);

    return ADP_SERIAL_STATUS_SUCCESS;
}

static ADP_SERIAL_STATUS _ParseNetworkJoinRequest(uint8_t* pData)
{
    uint16_t panId, lbaAddress;
    uint8_t mediaType;

    if ((ADP_Status() < ADP_STATUS_READY) || (adpSerialCoord == true))
    {
        /* ADP not initialized or LBP initialized as coordinator */
        return ADP_SERIAL_STATUS_NOT_ALLOWED;
    }

    /* Parse network join request message */
    panId = ((uint16_t) *pData++) << 8;
    panId += (uint16_t) *pData++;
    lbaAddress = ((uint16_t) *pData++) << 8;
    lbaAddress += (uint16_t) *pData;
    mediaType = *pData;

    /* Send network join request to LBP device */
    LBP_AdpNetworkJoinRequest(panId, lbaAddress, mediaType);

    return ADP_SERIAL_STATUS_SUCCESS;
}

static ADP_SERIAL_STATUS _ParseNetworkLeaveRequest(uint8_t* pData)
{
    if ((ADP_Status() < ADP_STATUS_READY) || (adpSerialCoord == true))
    {
        /* ADP not initialized or LBP initialized as coordinator */
        return ADP_SERIAL_STATUS_NOT_ALLOWED;
    }

    /* Send network leave request to LBP device */
    LBP_AdpNetworkLeaveRequest();

    return ADP_SERIAL_STATUS_SUCCESS;
}

static ADP_SERIAL_STATUS _ParseResetRequest(uint8_t* pData)
{
    if (ADP_Status() < ADP_STATUS_READY)
    {
        /* ADP not initialized */
        return ADP_SERIAL_STATUS_NOT_ALLOWED;
    }
    
    /* Send reset request to ADP */
    ADP_ResetRequest();
    
    return ADP_SERIAL_STATUS_SUCCESS;
}

static ADP_SERIAL_STATUS _ParseAdpSetRequest(uint8_t* pData)
{
    uint32_t attributeId;
    uint16_t attributeIndex;
    uint8_t attributeValue[64];
    uint8_t prefixLenghBytes, attributeLength, contextLength;
    uint8_t attributeLengthCnt = 0;

    if (ADP_Status() < ADP_STATUS_READY)
    {
        /* ADP not initialized */
        return ADP_SERIAL_STATUS_NOT_ALLOWED;
    }

    /* Parse ADP set request message */
    attributeId = ((uint32_t) *pData++) << 24;
    attributeId += ((uint32_t) *pData++) << 16;
    attributeId += ((uint32_t) *pData++) << 8;
    attributeId += (uint32_t) *pData++;
    attributeIndex = ((uint16_t) *pData++) << 8;
    attributeIndex += (uint16_t) *pData++;
    attributeLength = *pData++;

    switch (attributeId)
    {
        /* 8-bit IBs */
        case ADP_IB_SECURITY_LEVEL:
        case ADP_IB_METRIC_TYPE:
        case ADP_IB_LOW_LQI_VALUE:
        case ADP_IB_HIGH_LQI_VALUE:
        case ADP_IB_RREP_WAIT:
        case ADP_IB_RLC_ENABLED:
        case ADP_IB_ADD_REV_LINK_COST:
        case ADP_IB_UNICAST_RREQ_GEN_ENABLE:
        case ADP_IB_MAX_HOPS:
        case ADP_IB_DEVICE_TYPE:
        case ADP_IB_NET_TRAVERSAL_TIME:
        case ADP_IB_KR:
        case ADP_IB_KM:
        case ADP_IB_KC:
        case ADP_IB_KQ:
        case ADP_IB_KH:
        case ADP_IB_RREQ_RETRIES:
        case ADP_IB_RREQ_WAIT:
        case ADP_IB_WEAK_LQI_VALUE:
        case ADP_IB_KRT:
        case ADP_IB_PATH_DISCOVERY_TIME:
        case ADP_IB_ACTIVE_KEY_INDEX:
        case ADP_IB_DEFAULT_COORD_ROUTE_ENABLED:
        case ADP_IB_DISABLE_DEFAULT_ROUTING:
        case ADP_IB_RREQ_JITTER_LOW_LQI:
        case ADP_IB_RREQ_JITTER_HIGH_LQI:
        case ADP_IB_RREQ_JITTER_LOW_LQI_RF:
        case ADP_IB_RREQ_JITTER_HIGH_LQI_RF:
        case ADP_IB_TRICKLE_DATA_ENABLED:
        case ADP_IB_TRICKLE_LQI_THRESHOLD_LOW:
        case ADP_IB_TRICKLE_LQI_THRESHOLD_LOW_RF:
        case ADP_IB_TRICKLE_LQI_THRESHOLD_HIGH:
        case ADP_IB_TRICKLE_LQI_THRESHOLD_HIGH_RF:
        case ADP_IB_TRICKLE_STEP:
        case ADP_IB_TRICKLE_MAX_KI:
        case ADP_IB_TRICKLE_ADAPTIVE_I_MIN:
        case ADP_IB_TRICKLE_ADAPTIVE_KI:
        case ADP_IB_CLUSTER_TRICKLE_ENABLED:
        case ADP_IB_CLUSTER_MIN_LQI:
        case ADP_IB_CLUSTER_MIN_LQI_RF:
        case ADP_IB_CLUSTER_TRICKLE_K:
        case ADP_IB_CLUSTER_TRICKLE_K_RF:
        case ADP_IB_CLUSTER_RREQ_ROUTE_COST_DEVIATION:
        case ADP_IB_MANUF_IPV6_HEADER_COMPRESSION:
        case ADP_IB_MANUF_BROADCAST_SEQUENCE_NUMBER:
        case ADP_IB_MANUF_FORCED_NO_ACK_REQUEST:
        case ADP_IB_MANUF_BROADCAST_ROUTE_ALL:
        case ADP_IB_MANUF_MAX_REPAIR_RESEND_ATTEMPTS:
        case ADP_IB_MANUF_DISABLE_AUTO_RREQ:
        case ADP_IB_MANUF_UPDATE_NON_VOLATILE_DATA:
        case ADP_IB_MANUF_DYNAMIC_FRAGMENT_DELAY_ENABLED:
        case ADP_IB_LOW_LQI_VALUE_RF:
        case ADP_IB_HIGH_LQI_VALUE_RF:
        case ADP_IB_KQ_RF:
        case ADP_IB_KH_RF:
        case ADP_IB_KRT_RF:
        case ADP_IB_KDC_RF:
        case ADP_IB_USE_BACKUP_MEDIA:
        case ADP_IB_WEAK_LQI_VALUE_RF:
        case ADP_IB_LAST_GASP:
            attributeValue[0] = *pData;
            break;

        /* 16-bit IBs */
        case ADP_IB_BROADCAST_LOG_TABLE_ENTRY_TTL:
        case ADP_IB_COORD_SHORT_ADDRESS:
        case ADP_IB_ROUTING_TABLE_ENTRY_TTL:
        case ADP_IB_BLACKLIST_TABLE_ENTRY_TTL:
        case ADP_IB_MAX_JOIN_WAIT_TIME:
        case ADP_IB_DELAY_LOW_LQI:
        case ADP_IB_DELAY_HIGH_LQI:
        case ADP_IB_DELAY_LOW_LQI_RF:
        case ADP_IB_DELAY_HIGH_LQI_RF:
        case ADP_IB_DESTINATION_ADDRESS_SET:
        case ADP_IB_TRICKLE_I_MIN:
        case ADP_IB_CLUSTER_TRICKLE_I:
        case ADP_IB_CLUSTER_TRICKLE_I_RF:
        case ADP_IB_MANUF_REASSEMBY_TIMER:
        case ADP_IB_MANUF_DATAGRAM_TAG:
        case ADP_IB_MANUF_DISCOVER_SEQUENCE_NUMBER: /* ADP_IB_MANUF_DISCOVER_ROUTE_GLOBAL_SEQ_NUM */
        case ADP_IB_MANUF_CIRCULAR_ROUTES_DETECTED:
        case ADP_IB_MANUF_LAST_CIRCULAR_ROUTE_ADDRESS:
        case ADP_IB_MANUF_IPV6_ULA_DEST_SHORT_ADDRESS:
        case ADP_IB_MANUF_ALL_NEIGHBORS_BLACKLISTED_COUNT:
        case ADP_IB_MANUF_FRAGMENT_DELAY:
        case ADP_IB_MANUF_DYNAMIC_FRAGMENT_DELAY_FACTOR:
        case ADP_IB_MANUF_BLACKLIST_TABLE_COUNT:
        case ADP_IB_MANUF_PAN_ID:
        case ADP_IB_MANUF_SHORT_ADDRESS:
        case ADP_IB_GROUP_TABLE:
            _memcpyFromUsiEndianessUint16(attributeValue, pData);
            break;

        /* 32-bit IBs */
        case ADP_IB_MANUF_ROUTING_TABLE_COUNT:
            _memcpyFromUsiEndianessUint32(attributeValue, pData);
            break;

        /* Tables and lists */
        case ADP_IB_PREFIX_TABLE:
            /* len = 0 => Delete Entry */
            if (attributeLength != 0)
            {
                /* prefixLength */
                attributeValue[attributeLengthCnt++] = *pData++;
                /* onLinkFlag */
                attributeValue[attributeLengthCnt++] = *pData++;
                /* autonomousAddressConfigurationFlag */
                attributeValue[attributeLengthCnt++] = *pData++;
                /* validTime */
                _memcpyFromUsiEndianessUint32(&attributeValue[attributeLengthCnt], pData);
                attributeLengthCnt += 4;
                pData += 4;
                /* preferredTime */
                _memcpyFromUsiEndianessUint32(&attributeValue[attributeLengthCnt], pData);
                attributeLengthCnt += 4;
                pData += 4;
                /* prefix */
                prefixLenghBytes = attributeLength - 11;
                memcpy(&attributeValue[attributeLengthCnt], pData, prefixLenghBytes);
            }
            break;

        case ADP_IB_CONTEXT_INFORMATION_TABLE:
            /* len = 0 => Delete Entry */
            if (attributeLength != 0)
            {
                /* validTime */
                _memcpyFromUsiEndianessUint16(attributeValue, pData);
                attributeLengthCnt = 2;
                pData += 2;
                /* validForCompression */
                attributeValue[attributeLengthCnt++] = *pData++;
                /* bitsContextLength */
                attributeValue[attributeLengthCnt++] = *pData++;
                /* context */
                contextLength  = attributeLength - 4;
                memcpy(&attributeValue[attributeLengthCnt], pData, contextLength);
            }
            break;

        case ADP_IB_ROUTING_TABLE:
        case ADP_IB_MANUF_ROUTING_TABLE_ELEMENT:
            /* len = 0 => Delete Entry */
            if (attributeLength != 0)
            {
                /* dstAddr */
                _memcpyFromUsiEndianessUint16(attributeValue, pData);
                attributeLengthCnt = 2;
                pData += 2;
                /* nextHopAddr */
                _memcpyFromUsiEndianessUint16(&attributeValue[attributeLengthCnt], pData);
                attributeLengthCnt += 2;
                pData += 2;
                /* routeCost */
                _memcpyFromUsiEndianessUint16(&attributeValue[attributeLengthCnt], pData);
                attributeLengthCnt += 2;
                pData += 2;
                /* hopCount, weakLinkCount */
                attributeValue[attributeLengthCnt++] = *pData++;
                /* mediaType */
                attributeValue[attributeLengthCnt++] = *pData++;
                /* validTime */
                _memcpyFromUsiEndianessUint16(&attributeValue[attributeLengthCnt], pData);
            }
            break;

        case ADP_IB_BLACKLIST_TABLE:
            /* addr */
            _memcpyFromUsiEndianessUint16(attributeValue, pData);
            attributeLengthCnt = 2;
            pData += 2;
            /* mediaType */
            attributeValue[attributeLengthCnt++] = *pData++;
            /* validTime */
            _memcpyFromUsiEndianessUint16(&attributeValue[attributeLengthCnt], pData);
            break;

        case ADP_IB_SOFT_VERSION:
        case ADP_IB_MANUF_ADP_INTERNAL_VERSION:
        case ADP_IB_BROADCAST_LOG_TABLE:
        case ADP_IB_MANUF_LQI_TO_COORD:
        case ADP_IB_MANUF_QUEUED_ENTRIES_REMOVED_TIMEOUT_COUNT:
        case ADP_IB_MANUF_QUEUED_ENTRIES_REMOVED_ROUTE_ERROR_COUNT:
        case ADP_IB_MANUF_PENDING_DATA_IND_SHORT_ADDRESS:
        case ADP_IB_MANUF_GET_BAND_CONTEXT_TONES:
        case ADP_IB_MANUF_BROADCAST_LOG_TABLE_COUNT:
        case ADP_IB_MANUF_CONTEXT_INFORMATION_TABLE_COUNT:
        case ADP_IB_MANUF_GROUP_TABLE_COUNT:
        case ADP_IB_MANUF_HYBRID_PROFILE:
        case ADP_IB_MANUF_LAST_PHASEDIFF:
            /* G3_READ_ONLY */
            break;

        case ADP_IB_SNIFFER_MODE:
            /* TODO */
            break;

        default:
            break;
    }

    /* Send set request to ADP */
    ADP_SetRequest(attributeId, attributeIndex, attributeLength, attributeValue);

    return ADP_SERIAL_STATUS_SUCCESS;
}

static ADP_SERIAL_STATUS _ParseAdpGetRequest(uint8_t* pData)
{
    uint32_t attributeId;
    uint16_t attributeIndex;
    
    if (ADP_Status() < ADP_STATUS_READY)
    {
        /* ADP not initialized */
        return ADP_SERIAL_STATUS_NOT_ALLOWED;
    }

    /* Parse ADP get request */
    attributeId = MAC_WRP_SerialParseGetRequest(pData, &attributeIndex);

    /* Send get request to ADP */
    ADP_GetRequest(attributeId, attributeIndex);

    return ADP_SERIAL_STATUS_SUCCESS;
}

static ADP_SERIAL_STATUS _ParseRouteDiscoveryRequest(uint8_t* pData)
{
    uint16_t dstAddr;
    uint8_t maxHops;

    if (ADP_Status() < ADP_STATUS_READY)
    {
        /* ADP not initialized */
        return ADP_SERIAL_STATUS_NOT_ALLOWED;
    }

    /* Parse route discovery request */
    dstAddr = ((uint16_t) *pData++) << 8;
    dstAddr += (uint16_t) *pData++;
    maxHops = *pData;

    /* Send route discovery request to ADP */
    ADP_RouteDiscoveryRequest(dstAddr, maxHops);

    return ADP_SERIAL_STATUS_SUCCESS;
}

static ADP_SERIAL_STATUS _ParseMacSetRequest(uint8_t* pData)
{
    ADP_SET_CFM_PARAMS setConfirm;
    MAC_WRP_PIB_VALUE pibValue;
    MAC_WRP_PIB_ATTRIBUTE attributeId;
    MAC_WRP_STATUS setStatus;
    uint16_t attributeIndex;    
    uint8_t serialRspLen = 0;

    if (ADP_Status() < ADP_STATUS_READY)
    {
        /* ADP not initialized */
        return ADP_SERIAL_STATUS_NOT_ALLOWED;
    }

    /* Parse MAC set request message */
    attributeId = MAC_WRP_SerialParseSetRequest(pData, &attributeIndex, &pibValue);

    /* Set MAC PIB */
    ADP_MacSetRequestSync((uint32_t) attributeId, attributeIndex, pibValue.length, pibValue.value, &setConfirm);
    
    /* Fill serial response buffer */
    adpSerialRspBuffer[serialRspLen++] = ADP_SERIAL_MSG_ADP_MAC_SET_CONFIRM;
    setStatus = (MAC_WRP_STATUS) setConfirm.status;
    serialRspLen += MAC_WRP_SerialStringifySetConfirm(&adpSerialRspBuffer[serialRspLen],
            setStatus, attributeId, attributeIndex);

    /* Send set confirm through USI */
    SRV_USI_Send_Message(adpSerialUsiHandle, SRV_USI_PROT_ID_ADP_G3, adpSerialRspBuffer, serialRspLen);

    return ADP_SERIAL_STATUS_SUCCESS;
}

static ADP_SERIAL_STATUS _ParseMacGetRequest(uint8_t* pData)
{
    ADP_MAC_GET_CFM_PARAMS getConfirm;
    MAC_WRP_PIB_ATTRIBUTE macAttribute;
    MAC_WRP_STATUS getStatus;
    uint8_t* pibValue;
    uint32_t attributeId;
    uint16_t attributeIndex;
    uint8_t pibLength;
    uint8_t serialRspLen = 0;

    if (ADP_Status() < ADP_STATUS_READY)
    {
        /* ADP not initialized */
        return ADP_SERIAL_STATUS_NOT_ALLOWED;
    }

    /* Parse MAC get request message */
    attributeId = MAC_WRP_SerialParseGetRequest(pData, &attributeIndex);

    /* Get MAC PIB */
    ADP_MacGetRequestSync(attributeId, attributeIndex, &getConfirm);

    /* Fill serial response buffer */
    adpSerialRspBuffer[serialRspLen++] = ADP_SERIAL_MSG_ADP_MAC_GET_CONFIRM;
    getStatus = (MAC_WRP_STATUS) getConfirm.status;
    macAttribute = (MAC_WRP_PIB_ATTRIBUTE) attributeId;
    pibValue = getConfirm.attributeValue;
    pibLength = getConfirm.attributeLength;
    serialRspLen += MAC_WRP_SerialStringifyGetConfirm(&adpSerialRspBuffer[serialRspLen],
            getStatus, macAttribute, attributeIndex, pibValue, pibLength);

    /* Send get confirm through USI */
    SRV_USI_Send_Message(adpSerialUsiHandle, SRV_USI_PROT_ID_ADP_G3, adpSerialRspBuffer, serialRspLen);

    return ADP_SERIAL_STATUS_SUCCESS;
}

static ADP_SERIAL_STATUS _ParsePathDiscoveryRequest(uint8_t* pData)
{
    uint16_t dstAddr;
    uint8_t metricType;

    if (ADP_Status() < ADP_STATUS_READY)
    {
        /* ADP not initialized */
        return ADP_SERIAL_STATUS_NOT_ALLOWED;
    }

    /* Parse path discovery request message */
    dstAddr = ((uint16_t) *pData++) << 8;
    dstAddr += (uint16_t) *pData++;
    metricType = *pData;

    /* Send path discovery request to ADP */
    ADP_PathDiscoveryRequest(dstAddr, metricType);

    return ADP_SERIAL_STATUS_SUCCESS;
}

static ADP_SERIAL_STATUS _ParseLbpSetRequest(uint8_t* pData)
{
    LBP_SET_PARAM_CONFIRM setConfirm;
    uint32_t attributeId;
    uint16_t attributeIndex;
    uint8_t attributeValue[LBP_NETWORK_ACCESS_ID_SIZE_P_ARIB];
    uint8_t attributeLength;
    uint8_t serialRspLen = 0;

    if (ADP_Status() < ADP_STATUS_READY)
    {
        /* ADP not initialized */
        return ADP_SERIAL_STATUS_NOT_ALLOWED;
    }

    /* Parse LBP set request message */
    attributeId = ((uint32_t) *pData++) << 24;
    attributeId += ((uint32_t) *pData++) << 16;
    attributeId += ((uint32_t) *pData++) << 8;
    attributeId += (uint32_t) *pData++;
    attributeIndex = ((uint16_t) *pData++) << 8;
    attributeIndex += (uint16_t) *pData++;
    attributeLength = *pData++;

    switch (attributeId)
    {
        /* 16-bit IBs */
        case LBP_IB_MSG_TIMEOUT:
            _memcpyFromUsiEndianessUint16(attributeValue, pData);
            break;

        /* Tables and lists */
        case LBP_IB_IDS:
        case LBP_IB_IDP:
        case LBP_IB_PSK:
        case LBP_IB_GMK:
        case LBP_IB_REKEY_GMK:
        case LBP_IB_RANDP:
            memcpy(attributeValue, pData, attributeLength);
            break;

        default:
            break;
    }

    /* Set LBP paramter */
    if (adpSerialCoord == true)
    {
        LBP_SetParamCoord(attributeId, attributeIndex, attributeLength, attributeValue, &setConfirm);
    }
    else
    {
        LBP_SetParamDev(attributeId, attributeIndex, attributeLength, attributeValue, &setConfirm);
    }

    /* Fill serial response buffer */
    adpSerialRspBuffer[serialRspLen++] = ADP_SERIAL_MSG_LBP_SET_CONFIRM;
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) setConfirm.status;
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) (attributeId >> 24);
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) (attributeId >> 16);
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) (attributeId >> 8);
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) attributeId;
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) (attributeIndex >> 8);
    adpSerialRspBuffer[serialRspLen++] = (uint8_t) attributeIndex;

    /* Send get confirm through USI */
    SRV_USI_Send_Message(adpSerialUsiHandle, SRV_USI_PROT_ID_ADP_G3, adpSerialRspBuffer, serialRspLen);

    return ADP_SERIAL_STATUS_SUCCESS;
}

static ADP_SERIAL_STATUS _ParseLbpDevForceRegister(uint8_t* pData)
{
    ADP_EXTENDED_ADDRESS* pEUI64Address;
    ADP_GROUP_MASTER_KEY* pGMK;
    uint16_t shortAddress, panId;

    if ((ADP_Status() < ADP_STATUS_READY) || (adpSerialCoord == true))
    {
        /* ADP not initialized or initialized as coordinator */
        return ADP_SERIAL_STATUS_NOT_ALLOWED;
    }

    /* Parse LBP device force register message */
    pEUI64Address = (ADP_EXTENDED_ADDRESS*) pData;
    pData += 8;
    shortAddress = ((uint16_t) *pData++) << 8;
    shortAddress += (uint16_t) *pData++;
    panId = ((uint16_t) *pData++) << 8;
    panId += (uint16_t) *pData++;
    pGMK = (ADP_GROUP_MASTER_KEY*) pData;

    /* Send force register to LBP device */
    LBP_ForceRegister(pEUI64Address, shortAddress, panId, pGMK);

    return ADP_SERIAL_STATUS_SUCCESS;
}

static ADP_SERIAL_STATUS _ParseLbpCoordKickDevice(uint8_t* pData)
{
    ADP_EXTENDED_ADDRESS* pEUI64Address;
    uint16_t shortAddress;

    if ((ADP_Status() < ADP_STATUS_READY) || (adpSerialCoord == false))
    {
        /* ADP not initialized or initialized as device */
        return ADP_SERIAL_STATUS_NOT_ALLOWED;
    }

    /* Parse LBP coordinator kick device message */
    shortAddress = ((uint16_t) *pData++) << 8;
    shortAddress += (uint16_t) *pData++;
    pEUI64Address = (ADP_EXTENDED_ADDRESS*) pData;

    /* Send kick device to LBP coordinator */
    LBP_KickDevice(shortAddress, pEUI64Address);

    return ADP_SERIAL_STATUS_SUCCESS;
}

static ADP_SERIAL_STATUS _ParseLbpCoordRekey(uint8_t* pData)
{
    ADP_EXTENDED_ADDRESS* pEUI64Address;
    uint16_t shortAddress;
    bool distribute;

    if ((ADP_Status() < ADP_STATUS_READY) || (adpSerialCoord == false))
    {
        /* ADP not initialized or initialized as device */
        return ADP_SERIAL_STATUS_NOT_ALLOWED;
    }

    /* Parse LBP coordinator rekey message */
    shortAddress = ((uint16_t) *pData++) << 8;
    shortAddress += (uint16_t) *pData++;
    pEUI64Address = (ADP_EXTENDED_ADDRESS*) pData;
    pData += 8;
    distribute = (bool) *pData;

    /* Send rekey to LBP coordinator */
    LBP_Rekey(shortAddress, pEUI64Address, distribute);

    return ADP_SERIAL_STATUS_SUCCESS;
}

static ADP_SERIAL_STATUS _ParseLbpCoordSetRekeyPhase(uint8_t* pData)
{
    bool rekeyStart;

    if ((ADP_Status() < ADP_STATUS_READY) || (adpSerialCoord == false))
    {
        /* ADP not initialized or initialized as device */
        return ADP_SERIAL_STATUS_NOT_ALLOWED;
    }

    /* Parse LBP coordinator set rekey phase message */
    rekeyStart = (bool) *pData;

    /* Send set rekey phase to LBP coordinator */
    LBP_SetRekeyPhase(rekeyStart);

    return ADP_SERIAL_STATUS_SUCCESS;
}

static ADP_SERIAL_STATUS _ParseLbpCoordActivateNewKey(uint8_t* pData)
{
    if ((ADP_Status() < ADP_STATUS_READY) || (adpSerialCoord == false))
    {
        /* ADP not initialized or initialized as device */
        return ADP_SERIAL_STATUS_NOT_ALLOWED;
    }

    /* Send activate new key to LBP coordinator */
    LBP_ActivateNewKey();

    return ADP_SERIAL_STATUS_SUCCESS;
}

static ADP_SERIAL_STATUS _ParseLbpCoordShortAddressAssign(uint8_t* pData)
{
    uint8_t* pEUI64Address;
    uint16_t assignedAddress;

    if ((ADP_Status() < ADP_STATUS_READY) || (adpSerialCoord == false))
    {
        /* ADP not initialized or initialized as device */
        return ADP_SERIAL_STATUS_NOT_ALLOWED;
    }

    /* Parse LBP coordinator short address assign message */
    pEUI64Address = pData;
    pData += 8;
    assignedAddress = ((uint16_t) *pData++) << 8;
    assignedAddress += (uint16_t) *pData;

    /* Send short address assign to LBP coordinator */
    LBP_ShortAddressAssign(pEUI64Address, assignedAddress);

    return ADP_SERIAL_STATUS_SUCCESS;
}

static void _Callback_UsiAdpProtocol(uint8_t* pData, size_t length)
{
    uint8_t command;
    ADP_SERIAL_STATUS status = ADP_SERIAL_STATUS_UNKNOWN_COMMAND;

    /* Protection for invalid length */
    if (length == 0)
    {
        return;
    }

    /* Process received message */
    command = (*pData++) & 0x7F;

    switch (command)
    {
        case ADP_SERIAL_MSG_ADP_INITIALIZE:
            status = _ParseInitialize(pData);
            break;

        case ADP_SERIAL_MSG_ADP_DATA_REQUEST:
            status = _ParseDataRequest(pData);
            break;

        case ADP_SERIAL_MSG_ADP_DISCOVERY_REQUEST:
            status = _ParseDiscoveryRequest(pData);
            break;

        case ADP_SERIAL_MSG_ADP_NETWORK_START_REQUEST:
            status = _ParseNetworkStartRequest(pData);
            break;

        case ADP_SERIAL_MSG_ADP_NETWORK_JOIN_REQUEST:
            status = _ParseNetworkJoinRequest(pData);
            break;

        case ADP_SERIAL_MSG_ADP_NETWORK_LEAVE_REQUEST:
            status = _ParseNetworkLeaveRequest(pData);
            break;

        case ADP_SERIAL_MSG_ADP_RESET_REQUEST:
            status = _ParseResetRequest(pData);
            break;

        case ADP_SERIAL_MSG_ADP_SET_REQUEST:
            _ParseAdpSetRequest(pData);
            break;

        case ADP_SERIAL_MSG_ADP_GET_REQUEST:
            _ParseAdpGetRequest(pData);
            break;

        case ADP_SERIAL_MSG_ADP_ROUTE_DISCOVERY_REQUEST:
            status = _ParseRouteDiscoveryRequest(pData);
            break;

        case ADP_SERIAL_MSG_ADP_PATH_DISCOVERY_REQUEST:
            status = _ParsePathDiscoveryRequest(pData);
            break;

        case ADP_SERIAL_MSG_ADP_MAC_SET_REQUEST:
            status = _ParseMacSetRequest(pData);
            break;

        case ADP_SERIAL_MSG_ADP_MAC_GET_REQUEST:
            status = _ParseMacGetRequest(pData);
            break;

        case ADP_SERIAL_MSG_ADP_NO_IP_DATA_REQUEST:
            status = _ParseNoIPDataRequest(pData);
            break;

        case ADP_SERIAL_MSG_LBP_SET_REQUEST:
            status = _ParseLbpSetRequest(pData);
            break;

        case ADP_SERIAL_MSG_LBP_DEV_FORCE_REGISTER:
            status = _ParseLbpDevForceRegister(pData);
            break;

        case ADP_SERIAL_MSG_LBP_COORD_KICK_DEVICE:
            status = _ParseLbpCoordKickDevice(pData);
            break;

        case ADP_SERIAL_MSG_LBP_COORD_REKEY:
            status = _ParseLbpCoordRekey(pData);
            break;

        case ADP_SERIAL_MSG_LBP_COORD_SET_REKEY_PHASE:
            status = _ParseLbpCoordSetRekeyPhase(pData);
            break;

        case ADP_SERIAL_MSG_LBP_COORD_ACTIVATE_NEW_KEY:
            status = _ParseLbpCoordActivateNewKey(pData);
            break;

        case ADP_SERIAL_MSG_LBP_COORD_SHORT_ADDRESS_ASSIGN:
            status = _ParseLbpCoordShortAddressAssign(pData);
            break;

        default:
            break;
    }

    /* Initialize doesn't have confirm so send status, but from tasks when initialization finishes.
     * Other messages all have confirm. Send status only if there is a processing error */
    if (status != ADP_SERIAL_STATUS_SUCCESS)
    {
        _StringifyMsgStatus(status, command);
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Interface Function Definitions
// *****************************************************************************
// *****************************************************************************

SYS_MODULE_OBJ ADP_SERIAL_Initialize(const SYS_MODULE_INDEX index)
{
    /* Validate the request */
    if (index >= G3_ADP_SERIAL_INSTANCES_NUMBER)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    /* Initialize variables */
    adpSerialUsiHandle = SRV_USI_HANDLE_INVALID;
    adpSerialInitializing = false;
    adpSerialNotifications.setEUI64NonVolatileData = NULL;
    adpSerialNotifications.nonVolatileDataIndication = NULL;

    return (SYS_MODULE_OBJ) G3_ADP_SERIAL_INDEX_0; 
}

void ADP_SERIAL_Tasks(SYS_MODULE_OBJ object)
{
    ADP_STATUS adpStatus;

    if (object != (SYS_MODULE_OBJ) G3_ADP_SERIAL_INDEX_0)
    {
        /* Invalid object */
        return;
    }

    if (adpSerialUsiHandle == SRV_USI_HANDLE_INVALID)
    {
        /* Open USI instance for MAC serialization and register callback */
        adpSerialUsiHandle = SRV_USI_Open(G3_ADP_SERIAL_USI_INDEX);
        SRV_USI_CallbackRegister(adpSerialUsiHandle, SRV_USI_PROT_ID_ADP_G3, _Callback_UsiAdpProtocol);
    }

    /* Check ADP status */
    adpStatus = ADP_Status();

    if ((adpSerialInitializing == true) && (adpStatus >= ADP_STATUS_READY))
    {
        adpSerialInitializing = false;
        if (adpSerialNotifications.setEUI64NonVolatileData != NULL)
        {
            ADP_SET_CFM_PARAMS setConfirm;
            ADP_EXTENDED_ADDRESS eui64;
            ADP_NON_VOLATILE_DATA_IND_PARAMS nonVolatileData;

            /* Call upper layer to obtain EUI64 and non-volatile data */
            adpSerialNotifications.setEUI64NonVolatileData(&eui64, &nonVolatileData);

            /* Set Extended Address (EUI64) */
            ADP_MacSetRequestSync(MAC_WRP_PIB_MANUF_EXTENDED_ADDRESS, 0, 8, eui64.value, &setConfirm);

            /* Set non-volatile data */
            ADP_MacSetRequestSync(MAC_WRP_PIB_FRAME_COUNTER, 0, 4,
                (const uint8_t*) &nonVolatileData.frameCounter, &setConfirm);
            ADP_MacSetRequestSync(MAC_WRP_PIB_FRAME_COUNTER_RF, 0, 4,
                (const uint8_t*) &nonVolatileData.frameCounterRF, &setConfirm);
            ADP_SetRequestSync(ADP_IB_MANUF_DISCOVER_SEQUENCE_NUMBER, 0, 2,
                (const uint8_t*) &nonVolatileData.discoverSeqNumber, &setConfirm);
            ADP_SetRequestSync(ADP_IB_MANUF_BROADCAST_SEQUENCE_NUMBER, 0, 1,
                (const uint8_t*) &nonVolatileData.broadcastSeqNumber, &setConfirm);
        }

        /* Send ADP initiazation confirm */
        _StringifyMsgStatus(ADP_SERIAL_STATUS_SUCCESS, ADP_SERIAL_MSG_ADP_INITIALIZE);
    }

    if (adpStatus >= ADP_STATUS_READY)
    {
        if (adpSerialCoord == true)
        {
            /* LBP coordinator tasks */
            LBP_UpdateLbpSlots();
        }
        else
        {
            /* LBP device tasks */
            LBP_TasksDev();
        }
    }
}

void ADP_SERIAL_SetNotifications(ADP_SERIAL_NOTIFICATIONS* pNotifications)
{
    if (pNotifications != NULL)
    {
        adpSerialNotifications = *pNotifications;
    }
}
