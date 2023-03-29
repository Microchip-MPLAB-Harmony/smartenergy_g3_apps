/*******************************************************************************
  G3 Routing Wrapper Source File

  Company:
    Microchip Technology Inc.

  File Name:
    routing_wrapper.c

  Summary:
    G3 Routing Wrapper API Source File

  Description:
    This file contains implementation of the API to be used by upper layers when
    accessing G3 Routing layers.
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
#include "routing_wrapper.h"
#include "loadng.h"
#include "service/log_report/srv_log_report.h"

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Variables
// *****************************************************************************
// *****************************************************************************

/* Routing tables */
ROUTING_PENDING_RREQ_ENTRY routingWrpPendingRReqTable[LOADNG_PENDING_RREQ_TABLE_SIZE];
ROUTING_RREP_GENERATION_ENTRY routingWrpRRepGenerationTable[LOADNG_RREP_GEN_TABLE_SIZE];
ROUTING_RREQ_FORWARDING_ENTRY routingWrpRReqForwardingTable[LOADNG_RREQ_FORWARD_TABLE_SIZE];
ROUTING_DISCOVER_ROUTE_ENTRY routingWrpDiscoverRouteTable[LOADNG_DISCOVER_ROUTE_TABLE_SIZE];
ROUTING_TABLE_ENTRY routingWrpRoutingTable[G3_ADP_ROUTING_TABLE_SIZE];
ROUTING_BLACKLIST_ENTRY routingWrpBlacklistTable[G3_ADP_BLACKLIST_TABLE_SIZE];
ROUTING_TABLE_ENTRY routingWrpRoutingSet[G3_ADP_ROUTING_SET_SIZE];
uint16_t routingWrpDestinationAddressSet[G3_ADP_DESTINATION_ADDR_SET_SIZE];

// *****************************************************************************
// *****************************************************************************
// Section: Routing Wrapper Interface Routines
// *****************************************************************************
// *****************************************************************************

void ROUTING_WRP_Reset(MAC_WRP_HANDLE macWrpHandle)
{
    ROUTING_TABLES routingTables;

    /* Store the table sizes, to make LoadNG library independent of table sizes */
    routingTables.adpRoutingTableSize = G3_ADP_ROUTING_TABLE_SIZE;
    routingTables.adpBlacklistTableSize = G3_ADP_BLACKLIST_TABLE_SIZE;
    routingTables.adpRoutingSetSize = G3_ADP_ROUTING_SET_SIZE;
    routingTables.adpDestinationAddressSetSize = G3_ADP_DESTINATION_ADDR_SET_SIZE;
    routingTables.pendingRREQTableSize = LOADNG_PENDING_RREQ_TABLE_SIZE;
    routingTables.rrepGenerationTableSize = LOADNG_RREP_GEN_TABLE_SIZE;
    routingTables.discoverRouteTableSize = LOADNG_DISCOVER_ROUTE_TABLE_SIZE;
    routingTables.rreqForwardingTableSize = LOADNG_RREQ_FORWARD_TABLE_SIZE;

    /* Store pointers to tables */
    routingTables.adpRoutingTable = routingWrpRoutingTable;
    routingTables.adpBlacklistTable = routingWrpBlacklistTable;
    routingTables.adpRoutingSet = routingWrpRoutingSet;
    routingTables.adpDestinationAddressSet = routingWrpDestinationAddressSet;
    routingTables.pendingRREQTable = routingWrpPendingRReqTable;
    routingTables.rrepGenerationTable = routingWrpRRepGenerationTable;
    routingTables.discoverRouteTable = routingWrpDiscoverRouteTable;
    routingTables.rreqForwardingTable = routingWrpRReqForwardingTable;

    LOADNG_Reset(&routingTables, macWrpHandle);
}

bool ROUTING_WRP_IsDisabled()
{
    ADP_GET_CFM_PARAMS getConfirm;
    ROUTING_WRP_GetMib(ADP_IB_DISABLE_DEFAULT_ROUTING, 0, &getConfirm);
    return (bool)getConfirm.attributeValue[0];
}

bool ROUTING_WRP_IsAutoRReqDisabled()
{
    ADP_GET_CFM_PARAMS getConfirm;
    ROUTING_WRP_GetMib(ADP_IB_MANUF_DISABLE_AUTO_RREQ, 0, &getConfirm);
    return (bool)getConfirm.attributeValue[0];
}

bool ROUTING_WRP_IsDefaultCoordRouteEnabled()
{
    ADP_GET_CFM_PARAMS getConfirm;
    ROUTING_WRP_GetMib(ADP_IB_DEFAULT_COORD_ROUTE_ENABLED, 0, &getConfirm);
    return (bool)getConfirm.attributeValue[0];
}

uint8_t ROUTING_WRP_GetRRepWait()
{
    ADP_GET_CFM_PARAMS getConfirm;
    ROUTING_WRP_GetMib(ADP_IB_RREP_WAIT, 0, &getConfirm);
    return getConfirm.attributeValue[0];
}

uint16_t ROUTING_WRP_GetDiscoverRouteGlobalSeqNo()
{
    uint16_t u16Value;
    ADP_GET_CFM_PARAMS getConfirm;
    ROUTING_WRP_GetMib(ADP_IB_MANUF_DISCOVER_SEQUENCE_NUMBER, 0, &getConfirm);
    memcpy(&u16Value, &getConfirm.attributeValue, 2);
    return u16Value;
}

void ROUTING_WRP_SetDiscoverRouteGlobalSeqNo(uint16_t seqNo)
{
    ADP_SET_CFM_PARAMS setConfirm;
    ROUTING_WRP_SetMib(ADP_IB_MANUF_DISCOVER_SEQUENCE_NUMBER, 0, 2,
        (uint8_t*) &seqNo, &setConfirm);
}

void ROUTING_WRP_GetMib(uint32_t attributeId, uint16_t attributeIndex,
    ADP_GET_CFM_PARAMS* pGetConfirm)
{
    LOADNG_GetMib(attributeId, attributeIndex, pGetConfirm);
}

void ROUTING_WRP_SetMib(uint32_t attributeId, uint16_t attributeIndex,
    uint8_t attributeLength, const uint8_t *pAttributeValue,
    ADP_SET_CFM_PARAMS* pSetConfirm)
{
    LOADNG_SetMib(attributeId, attributeIndex, attributeLength, pAttributeValue, pSetConfirm);
}

void ROUTING_WRP_DiscoverPath(uint16_t dstAddr, uint8_t metricType,
    ROUTING_WRP_DISCOVER_PATH_CALLBACK callback)
{
    if (ROUTING_WRP_IsDisabled() == false)
    {
        LOADNG_DiscoverPath(dstAddr, metricType, callback);
    }
    else
    {
        callback(G3_INVALID_REQUEST, NULL);
    }
}

void ROUTING_WRP_NotifyRouteError(uint16_t dstAddr, uint16_t unreachableAddress,
    uint8_t errorCode)
{
    LOADNG_NotifyRouteError(dstAddr, unreachableAddress, errorCode);
}

void ROUTING_WRP_DiscoverRoute(uint16_t dstAddr, uint8_t maxHops, bool repair,
    void *pUserData, ROUTING_WRP_DISCOVER_ROUTE_CALLBACK callback)
{
    if (ROUTING_WRP_IsDisabled() == false)
    {
        LOADNG_DiscoverRoute(dstAddr, maxHops, repair, pUserData, callback);
    }
    else
    {
        callback(G3_ROUTE_ERROR, dstAddr, 0xFFFF, NULL);
    }
}

bool ROUTING_WRP_IsInDestinationAddressSet(uint16_t addr)
{
    return LOADNG_IsInDestinationAddressSet(addr);
}

void ROUTING_WRP_ProcessMessage(uint16_t macSrcAddr, uint8_t mediaType,
    ADP_MODULATION_PLC modulation, uint8_t activeTones, uint8_t subCarriers,
    uint8_t lqi, uint16_t messageLength, uint8_t *pMessageBuffer)
{
    LOADNG_ProcessMessage(macSrcAddr, mediaType, modulation, activeTones,
        subCarriers, lqi, messageLength, pMessageBuffer);
}

ROUTING_TABLE_ENTRY *ROUTING_WRP_AddRoute(uint16_t dstAddr,
    uint16_t nextHopAddr, uint8_t mediaType, bool *pTableFull)
{
    return LOADNG_AddRoute(dstAddr, nextHopAddr, mediaType, pTableFull);
}

void ROUTING_WRP_RefreshRoute(uint16_t dstAddr)
{
    LOADNG_RefreshRoute(dstAddr);
}

void ROUTING_WRP_AddCircularRoute(uint16_t lastCircularRouteAddress)
{
    LOADNG_AddCircularRoute(lastCircularRouteAddress);
}

void ROUTING_WRP_DeleteRoute(uint16_t dstAddr)
{
    LOADNG_DeleteRoute(dstAddr);
}

bool ROUTING_WRP_RouteExists(uint16_t destinationAddress)
{
    return LOADNG_RouteExists(destinationAddress);
}

uint16_t ROUTING_WRP_GetRouteAndMediaType(uint16_t destinationAddress,
    uint8_t *pMediaType)
{
    return LOADNG_GetRouteAndMediaType(destinationAddress, pMediaType);
}

ROUTING_TABLE_ENTRY *ROUTING_WRP_GetRouteEntry(uint16_t destinationAddress)
{
    ROUTING_TABLE_ENTRY *pRet = NULL;
    if (ROUTING_WRP_IsDisabled() == false)
    {
        pRet = LOADNG_GetRouteEntry(destinationAddress);
    }
    return pRet;
}

void ROUTING_WRP_AddBlacklistOnMedium(uint16_t addr, uint8_t mediaType)
{
    LOADNG_AddBlacklistOnMedium(addr, mediaType);
}

void ROUTING_WRP_RemoveBlacklistOnMedium(uint16_t addr, uint8_t mediaType)
{
    LOADNG_RemoveBlacklistOnMedium(addr, mediaType);
}

bool ROUTING_WRP_IsRouterTo(uint16_t addr)
{
    return LOADNG_IsRouterTo(addr);
}

void ROUTING_WRP_Tasks(void)
{
    LOADNG_Tasks();
}
