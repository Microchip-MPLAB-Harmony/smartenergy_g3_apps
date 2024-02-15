/*******************************************************************************
  LOADNG Interface Header File

  Company:
    Microchip Technology Inc.

  File Name:
    loadng.h

  Summary:
    LOADNG Interface Header File

  Description:
    The LOADNG provides a simple interface to manage the LOADNG Adaptation
    Layer. This file provides the interface definition for LOADNG.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*
Copyright (C) 2024, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#ifndef LOADNG_H
#define LOADNG_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************
#include "routing_types.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: LOADNG Interface Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    void LOADNG_Reset(ROUTING_TABLES *routingTables, MAC_WRP_HANDLE macWrpHandle);

  Summary:
    Resets the LOADNG module data.

  Description:
    This routine initializes the LOADNG data structures.

  Precondition:
    None.

  Parameters:
    routingTables - Pointer to routing tables
    macWrpHandle  - Handler to the MAC Wrapper

  Returns:
    None.

  Example:
    <code>
    ROUTING_TABLES routingTables;
    MAC_WRP_HANDLE macWrpHandle;

    routingTables.adpRoutingTableSize = G3_ADP_ROUTING_TABLE_SIZE;
    routingTables.adpBlacklistTableSize = G3_ADP_BLACKLIST_TABLE_SIZE;
    routingTables.adpRoutingSetSize = G3_ADP_ROUTING_SET_SIZE;
    routingTables.adpDestinationAddressSetSize = G3_ADP_DESTINATION_ADDR_SET_SIZE;

    macWrpHandle = MAC_WRP_Open(G3_MAC_WRP_INDEX_0, MAC_WRP_BAND_CENELEC_A);
    ...

    LOADNG_Reset(&routingTables, macWrpHandle);
    </code>

  Remarks:
    None.
*/
void LOADNG_Reset(ROUTING_TABLES *routingTables, MAC_WRP_HANDLE macWrpHandle);

// *****************************************************************************
/* Function:
    void LOADNG_DiscoverPath(uint16_t dstAddr, uint8_t metricType,
        ROUTING_WRP_DISCOVER_PATH_CALLBACK callback);

  Summary:
    Discover path to a given destination address.

  Description:
    This primitive discovers a path to a given destination address. When path
    discovery finishes a function is called back.

  Precondition:
    None.

  Parameters:
    dstAddr    - Path destination address

    metricType -

    callback   - Pointer to function to call back when path discovery finishes

  Returns:
    None.

  Example:
    <code>
    static void _PathDiscovery_Callback(uint8_t status,
        ADP_PATH_DESCRIPTOR *pPathDescriptor)
    {

    }

    LOADNG_DiscoverPath(0x0001, 1, _PathDiscovery_Callback);
    </code>

  Remarks:
    None.
*/
void LOADNG_DiscoverPath(uint16_t dstAddr, uint8_t metricType,
    ROUTING_WRP_DISCOVER_PATH_CALLBACK callback);

// *****************************************************************************
/* Function:
    void LOADNG_ProcessMessage(uint16_t macSrcAddr, uint8_t mediaType,
        ADP_MODULATION_PLC modulation, uint8_t activeTones, uint8_t subCarriers,
        uint8_t lqi, uint16_t messageLength, uint8_t *pMessageBuffer);

  Summary:
    Processes a received LOADNG message.

  Description:
    This routine processes a received LOADNG message.

  Precondition:
    None.

  Parameters:
    macSrcAddr     - MAC source address

    mediaType      - Media type (PLC or RF) from which the message was received

    modulation     - Computed modulation to communicate with the transmitter.
                     Only relevant for PLC.

    activeTones    - Computed number of active tones to communicate with the
                     transmitter. Only relevant for PLC.

    subCarriers    - Computed number of subcarriers to communicate with the
                     transmitter. Only relevant for PLC.

    lqi            - Link Quality Indicator of the received message

    messageLength  - Message length in bytes

    pMessageBuffer - Pointer to LOADNG message buffer

  Returns:
    None.

  Example:
    <code>
    App_DataIndication(MAC_WRP_DATA_INDICATION_PARAMS *params)
    {
        if (params->destPanId == myPanId)
        {
            if (params->destAddress.addressMode == MAC_WRP_ADDRESS_MODE_SHORT)
            {
                if (params->destAddress.shortAddress == myShortAddress)
                {
                    uint16_t payloadLength = 0;
                    uint8_t commandId = 0;
                    uint8_t* pPayload = NULL;

                    if (LoWPAN_Decode_EscHeader(params->msduLength,
                        params->msdu, &commandId, &payloadLength, &pPayload))
                    {
                        if (commandId == COMMAND_LOADNG)
                        {
                            MAC_WRP_TONE_MASK toneMask;
                            ADP_MODULATION_PLC modulation;
                            uint8_t activeTones, subCarriers;

                            modulation = _ConvertModulation(
                                params->computedModulation,
                                params->computedModulationScheme);

                            activeTones = _ComputeActiveTones(
                                &params->computedToneMap);

                            subCarriers = _CalculateSubCarriers(
                                &params->computedToneMap,
                                &toneMask, params->computedModulationScheme);

                          LOADNG_ProcessMessage(
                              params->srcAddress.shortAddress,
                              params->mediaType, modulation, activeTones,
                              subCarriers, params->linkQuality,
                              payloadLength, pPayload);
                        }
                    }
                }
            }
        }
    }
    </code>

  Remarks:
    None.
*/
void LOADNG_ProcessMessage(uint16_t macSrcAddr, uint8_t mediaType,
    ADP_MODULATION_PLC modulation, uint8_t activeTones, uint8_t subCarriers,
    uint8_t lqi, uint16_t messageLength, uint8_t *pMessageBuffer);


// *****************************************************************************
/* Function:
    void LOADNG_NotifyRouteError(uint16_t dstAddr, uint16_t unreachableAddress,
        uint8_t errorCode);

  Summary:
    None.

  Description:
    None.

  Precondition:
    None.

  Parameters:
    dstAddr            -

    unreachableAddress -

    errorCode          -

  Returns:
    None.

  Example:
    <code>
    </code>

  Remarks:
    None.
*/
void LOADNG_NotifyRouteError(uint16_t dstAddr, uint16_t unreachableAddress,
    uint8_t errorCode);

// *****************************************************************************
/* Function:
    void LOADNG_DiscoverRoute(uint16_t dstAddr, uint8_t maxHops, bool repair,
        void *pUserData, ROUTING_WRP_DISCOVER_ROUTE_CALLBACK callback);

  Summary:
    Discover route to a given destination address.

  Description:
    This primitive discovers a route to a given destination address. When route
    discovery finishes a function is called back.

  Precondition:
    None.

  Parameters:
    dstAddr   - Destination address to discover the route

    maxHops   - Maximum number of hops for the route

    repair    - Route repair flag

    pUserData - User data that is passed back in the callback

    callback  - Pointer to function to call back when route discovery finishes

  Returns:
    None.

  Example:
    <code>
    static void _RouteDiscovery_Callback(uint8_t status, uint16_t dstAddr,
        uint16_t nextHop, void *pUserData)
    {

    }

    LOADNG_DiscoverRoute(0x0001, 5, false, NULL, _RouteDiscovery_Callback);
    </code>

  Remarks:
    None.
*/
void LOADNG_DiscoverRoute(uint16_t dstAddr, uint8_t maxHops, bool repair,
    void *pUserData, ROUTING_WRP_DISCOVER_ROUTE_CALLBACK callback);

// *****************************************************************************
/* Function:
    void LOADNG_RefreshRoute(uint16_t dstAddr);

  Summary:
    Refresh the valid time of the route.

  Description:
    This primitive refreshes the valid time of the route for a given destination
    address. This function is called when a message is sent and confirmed by the
    MAC layer (also set the bidirectional flag).

  Precondition:
    None.

  Parameters:
    dstAddr - Destination address of the route to refresh

  Returns:
    None.

  Example:
    <code>
    LOADNG_RefreshRoute(0x0001);
    </code>

  Remarks:
    None.
*/
void LOADNG_RefreshRoute(uint16_t dstAddr);

// *****************************************************************************
/* Function:
    void LOADNG_AddCircularRoute(uint16_t lastCircularRouteAddress);

  Summary:
    .

  Description:
    .

  Precondition:
    None.

  Parameters:
    lastCircularRouteAddress -

  Returns:
    None.

  Example:
    <code>
    </code>

  Remarks:
    None.
*/
void LOADNG_AddCircularRoute(uint16_t lastCircularRouteAddress);

// *****************************************************************************
/* Function:
    void LOADNG_DeleteRoute(uint16_t dstAddr);

  Summary:
    Deletes a route.

  Description:
    This primitive deletes a route for a given destination
    address.

  Precondition:
    None.

  Parameters:
    dstAddr - Destination address of the route to delete

  Returns:
    None.

  Example:
    <code>
    LOADNG_DeleteRoute(0x0001);
    </code>

  Remarks:
    None.
*/
void LOADNG_DeleteRoute(uint16_t dstAddr);

// *****************************************************************************
/* Function:
    bool LOADNG_RouteExists(uint16_t destinationAddress);

  Summary:
    Check if a route exists.

  Description:
    This function allows to check if a route to a given destination address
    exists.

  Precondition:
    None.

  Parameters:
    destinationAddress - Destination address to check

  Returns:
    Returns true if route is known and false otherwise.

  Example:
    <code>
    static void _RouteDiscovery_Callback(uint8_t status, uint16_t dstAddr,
        uint16_t nextHop, void *pUserData)
    {

    }

    if (LOADNG_RouteExists(0x0001) == true)
    {

    }
    else
    {
        LOADNG_DiscoverRoute(0x0001, 5, false, NULL, _RouteDiscovery_Callback);
    }
    </code>

  Remarks:
    None.
*/
bool LOADNG_RouteExists(uint16_t destinationAddress);

// *****************************************************************************
/* Function:
    uint16_t LOADNG_GetRouteAndMediaType(uint16_t destinationAddress,
        uint8_t *pMediaType);

  Summary:
    Gets route and media type for a given destination address.

  Description:
    This function allows to get the route and media type (PLC or RF) for a given
    destination address.

  Precondition:
    None.

  Parameters:
    destinationAddress - Destination address to get route and media type

    pMediaType         - Pointer to media type (result)

  Returns:
    Returns the next hop address.

  Example:
    <code>
    uint16_t nextHopAddr;
    uint8_t mediaType;

    if (LOADNG_RouteExists(0x0001) == true)
    {
        nextHopAddr = LOADNG_GetRouteAndMediaType(0x0001, &mediaType);
    }
    </code>

  Remarks:
    Before calling this function, check if route exists (LOADNG_RouteExists).
*/
uint16_t LOADNG_GetRouteAndMediaType(uint16_t destinationAddress,
    uint8_t *pMediaType);

// *****************************************************************************
/* Function:
    ROUTING_TABLE_ENTRY* LOADNG_AddRoute(uint16_t dstAddr, uint16_t nextHopAddr,
        uint8_t mediaType, bool *pTableFull);

  Summary:
    Add new candidate route.

  Description:
    .

  Precondition:
    None.

  Parameters:
    dstAddr     -

    nextHopAddr -

    mediaType   -

    pTableFull  -

  Returns:
    .

  Example:
    <code>
    </code>

  Remarks:
    None.
*/
ROUTING_TABLE_ENTRY* LOADNG_AddRoute(uint16_t dstAddr, uint16_t nextHopAddr,
    uint8_t mediaType, bool *pTableFull);

// *****************************************************************************
/* Function:
    ROUTING_TABLE_ENTRY* LOADNG_GetRouteEntry(uint16_t destinationAddress);

  Summary:
    Gets a pointer to Route Entry.

  Description:
    .

  Precondition:
    None.

  Parameters:
    destinationAddress -

  Returns:
    .

  Example:
    <code>
    </code>

  Remarks:
    Before calling this function, check if route exists (LOADNG_RouteExists).
*/
ROUTING_TABLE_ENTRY* LOADNG_GetRouteEntry(uint16_t destinationAddress);

// *****************************************************************************
/* Function:
    bool LOADNG_IsInDestinationAddressSet(uint16_t addr);

  Summary:
    .

  Description:
    Returns true if the address is in the Destination Address Set (CCTT#183).

  Precondition:
    None.

  Parameters:
    addr -

  Returns:
    .

  Example:
    <code>
    </code>

  Remarks:
    None.
*/
bool LOADNG_IsInDestinationAddressSet(uint16_t addr);

// *****************************************************************************
/* Function:
    void LOADNG_GetMib(uint32_t attributeId, uint16_t attributeIndex,
        ADP_GET_CFM_PARAMS* pGetConfirm);

  Summary:
    Gets LOADNG MIB value.

  Description:
    This function allows to get a LOADNG MIB value.

  Precondition:
    None.

  Parameters:
    attributeId    - The identifier of the LOADNG MIB attribute to read

    attributeIndex - The index within the table of the specified MIB attribute
                     to read

    pGetConfirm    - Pointer to Get Confirm parameters (output).

  Returns:
    None.

  Example:
    <code>
    ADP_GET_CFM_PARAMS getConfirm;

    LOADNG_GetMib(ADP_IB_DESTINATION_ADDRESS_SET, 0, &getConfirm);
    </code>

  Remarks:
    None.
*/
void LOADNG_GetMib(uint32_t attributeId, uint16_t attributeIndex,
    ADP_GET_CFM_PARAMS* pGetConfirm);

// *****************************************************************************
/* Function:
    void LOADNG_SetMib(uint32_t attributeId, uint16_t attributeIndex,
        uint8_t attributeLength, const uint8_t *pAttributeValue,
        ADP_SET_CFM_PARAMS* pSetConfirm);

  Summary:
    Sets LOADNG MIB value.

  Description:
    .

  Precondition:
    None.

  Parameters:
    attributeId     - The identifier of the LOADNG MIB attribute to set

    attributeIndex  - The index within the table of the specified MIB attribute
                      to write

    attributeLength - MIB attribute length in bytes

    pAttributeValue - Pointer to MIB attribute value

    pSetConfirm     - Pointer to Set Confirm parameters (output)

  Returns:
    None.

  Example:
    <code>
    uint8_t lowLqiValue = 44;
    ADP_SET_CFM_PARAMS setConfirm;

    LOADNG_SetMib(ADP_IB_LOW_LQI_VALUE, 0, 1, &lowLqiValue, &setConfirm);
    </code>

  Remarks:
    None.
*/
void LOADNG_SetMib(uint32_t attributeId, uint16_t attributeIndex,
    uint8_t attributeLength, const uint8_t *pAttributeValue,
    ADP_SET_CFM_PARAMS* pSetConfirm);

// *****************************************************************************
/* Function:
    void LOADNG_AddBlacklistOnMedium(uint16_t addr, uint8_t mediaType);

  Summary:
    Adds node to blacklist for a given medium.

  Description:
    This function allows to add a node to blacklist for a given medium.

  Precondition:
    None.

  Parameters:
    addr      - Node address to add to blacklist

    mediaType - Medium (PLC or RF) to add to blacklist

  Returns:
    None.

  Example:
    <code>
    LOADNG_AddBlacklistOnMedium(0x0001, MAC_WRP_MEDIA_TYPE_IND_PLC);
    </code>

  Remarks:
    None.
*/
void LOADNG_AddBlacklistOnMedium(uint16_t addr, uint8_t mediaType);

// *****************************************************************************
/* Function:
    void LOADNG_RemoveBlacklistOnMedium(uint16_t addr, uint8_t mediaType);

  Summary:
    Removes a node from blacklist for a given medium.

  Description:
    This function removes a node from blacklist for a given medium.

  Precondition:
    None.

  Parameters:
    addr      - Node address to remove from blacklist

    mediaType - Medium (PLC or RF) to remove from blacklist

  Returns:
    None.

  Example:
    <code>
    LOADNG_RemoveBlacklistOnMedium(0x0001, MAC_WRP_MEDIA_TYPE_IND_PLC);
    </code>

  Remarks:
    None.
*/
void LOADNG_RemoveBlacklistOnMedium(uint16_t addr, uint8_t mediaType);

// *****************************************************************************
/* Function:
    bool LOADNG_IsRouterTo(uint16_t addr);

  Summary:
    Returns true if node acts as a Router to addr.

  Description:
    Returns true if node is part of a LOADng route to addr.

  Precondition:
    None.

  Parameters:
    addr - Destination address.

  Returns:
    True if node acts as a Router to addr. Otherwise False.

  Example:
    <code>
    destAddr = extractDestinationFromPacket(pkt);

    if (LOADNG_IsRouterTo(destAddr))
    {

    }
    </code>

  Remarks:
    None.
*/

/**********************************************************************************************************************/
/** Returns true if node has a Route to u16Addr, to its next hop, and to any other node
 **********************************************************************************************************************/
bool LOADNG_IsRouterTo(uint16_t addr);

// *****************************************************************************
/* Function:
    void LOADNG_Tasks(void);

  Summary:
    Maintains LOADng State Machine.

  Description:
    Maintains the LOADng State Machine.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    while (true)
    {
        LOADNG_Tasks();
    }
    </code>

  Remarks:
    None.
*/
void LOADNG_Tasks(void);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // #ifndef LOADNG_H
