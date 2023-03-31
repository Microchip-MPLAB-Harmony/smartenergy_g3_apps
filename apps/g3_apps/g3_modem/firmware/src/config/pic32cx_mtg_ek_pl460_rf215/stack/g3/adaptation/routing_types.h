/*******************************************************************************
  Routing Types Header File

  Company:
    Microchip Technology Inc.

  File Name:
    routing_types.h

  Summary:
    Routing Types Header File

  Description:
    The Routing Wrapper provides a simple interface to manage the Routing
    Adaptation Layer. This file provides data types definition for Routing
    Wrapper.
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

#ifndef _ROUTING_TYPES_H
#define _ROUTING_TYPES_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************
#include "adp.h"
#include "system/time/sys_time.h"
#include "stack/g3/mac/mac_wrapper/mac_wrapper_defs.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Macro Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* RRER Code No Available Route

   Summary:
    Defines the error code when there is no available route.

   Description:
    This macro defines the error code when there is no available route to
    destination.

   Remarks:
    None.
*/
#define RERR_CODE_NO_AVAILABLE_ROUTE   0

// *****************************************************************************
/* RRER Code Hop Limit Exceeded

   Summary:
    Defines the error code when hop limit is excided.

   Description:
    This macro defines the error code when hop limit is excided.

   Remarks:
    None.
*/
#define RERR_CODE_HOP_LIMIT_EXCEEDED   1

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Routing Wrapper Discover Route Event Handler Function Pointer

  Summary:
    Pointer to a Discover Route Event handler function.

  Description:
    This data type defines the required function signature for the Routing
    Wrapper Discover Route event handling callback function.

    A client must register a pointer using the event handling function whose
    function signature (parameter and return value types) matches the types
    specified by this function pointer in order to receive Discover Route
    events back from module.

  Parameters:
    status    - Route Discovery result

    dstAddr   - Final destination address

    nextHop   - Address of next hop

    pUserData - User data given when setting the callback

  Example:
    <code>
    App_DiscoverRouteCallback(uint8_t status, uint16_t dstAddr,
        uint16_t nextHop, void* pUserData)
    {
        // Check result
        if (status == G3_SUCCESS)
        {
            
        }
    }
    </code>

  Remarks:
    None.
*/
typedef void (*ROUTING_WRP_DISCOVER_ROUTE_CALLBACK)(uint8_t status,
    uint16_t dstAddr, uint16_t nextHop, void* pUserData);

// *****************************************************************************
/* Routing Wrapper Discover Path Event Handler Function Pointer

  Summary:
    Pointer to a Discover Path Event handler function.

  Description:
    This data type defines the required function signature for the Routing
    Wrapper Discover Path event handling callback function.

    A client must register a pointer using the event handling function whose
    function signature (parameter and return value types) matches the types
    specified by this function pointer in order to receive Discover Path
    events back from module.

  Parameters:
    status          - Path Discovery result

    pPathDescriptor - Pointer to path descriptor parameters

  Example:
    <code>
    App_DiscoverPathCallback(uint8_t status,
        ADP_PATH_DESCRIPTOR *pPathDescriptor)
    {
        // Check result
        if (status == G3_SUCCESS)
        {
            
        }
    }
    </code>

  Remarks:
    None.
*/
typedef void (*ROUTING_WRP_DISCOVER_PATH_CALLBACK)(uint8_t status,
    ADP_PATH_DESCRIPTOR *pPathDescriptor);

// *****************************************************************************
/* Routing Table Entry Definition

   Summary:
    Defines the structure of a Routing Table entry.

   Description:
    This data type defines the structure of a Routing Table entry.

   Remarks:
    None.
*/
typedef struct
{
    /* Absolute time in milliseconds when the entry expires */
    int32_t validTime;

    /* 16-bit short link layer address of the final destination of a route */
    uint16_t dstAddr;

    /* 16-bit short link layer address of the next hop node to the destination */
    uint16_t nextHopAddr;

    uint16_t seqNo;
    uint16_t routeCost;
    uint8_t hopCount : 4;
    uint8_t weakLinkCount : 4;
    uint8_t validSeqNo : 1;
    uint8_t bidirectional : 1;
    uint8_t rrepSent : 1;
    uint8_t metricType : 2;
    uint8_t mediaType : 1;
    uint8_t isRouter : 1;
  
} ROUTING_TABLE_ENTRY;

// *****************************************************************************
/* Routing Blacklist Table Entry Definition

   Summary:
    Defines the structure of a Routing Blacklist Table entry.

   Description:
    This data type defines the structure of a Routing Blacklist Table entry. It
    defines a blacklisted neighbour.

   Remarks:
    None.
*/
typedef struct
{
    /* Absolute time in milliseconds when the entry expires */
    int32_t validTime;

    /* Blacklisted 16-bit short link layer address */
    uint16_t addr;

    /* Blacklisted media type */
    uint8_t mediaType;

} ROUTING_BLACKLIST_ENTRY;

// *****************************************************************************
/* Route Discovery Table Entry Definition

   Summary:
    Defines the structure of a Route Discovery Table entry.

   Description:
    This data type defines the structure of a Route Discovery Table entry.

   Remarks:
    None.
*/
typedef struct {
    /* Callback called when the discovery is finished */
    ROUTING_WRP_DISCOVER_ROUTE_CALLBACK callback;

    /* User data */
    void* userData;

    /* Timer to control the discovery process if no response is received */
    SYS_TIME_HANDLE timeHandle;

    /* Final destination of the discover */
    uint16_t dstAddr;

    /* Discover route sequence number */
    uint16_t seqNo;

    /* Maximum hops parameter */
    uint8_t maxHops;

    /* Repair route: true / false */
    bool repair;

    /* Current try number */
    uint8_t tryCount;

    /* Discover route timeout flag */
    bool timeoutExpired;

} ROUTING_DISCOVER_ROUTE_ENTRY;

// *****************************************************************************
/* Routing RREP Generation Table Entry Definition

   Summary:
    Defines the structure of a RREP Generation Table entry.

   Description:
    This data type defines the structure of a Routing RREP Generation Table
    entry.

   Remarks:
    None.
*/
typedef struct _ROUTING_RREP_GENERATION_ENTRY {
    /* Timer to control the RREP sending */
    SYS_TIME_HANDLE timeHandle;

    /* Pointer to RREP generation entry used when timer expires */
    struct _ROUTING_RREP_GENERATION_ENTRY* timerRrepGenEntry;

    /* RREQ originator (and final destination of RREP) */
    uint16_t origAddr;

    /* RREQ destination (and originator of RREP). Unused in SPEC-15 */
    uint16_t dstAddr;

    /* RREQ Sequence number to be able to manage different RREQ from same node */
    uint16_t rreqSeqNum;

    /* Flags received from RREQ */
    uint8_t flags;

    /* MetricType received from RREQ */
    uint8_t metricType;

    /* Flag to indicate entry is waiting for ACK, timer can be expired but RREP
     * were not sent due to channel saturation */
    uint8_t waitingForAck;

    /* RREP generation time expired flag */
    bool timeExpired;

} ROUTING_RREP_GENERATION_ENTRY;

// *****************************************************************************
/* Routing RREQ Forwarding Table Entry Definition

   Summary:
    Defines the structure of a RREQ Forwarding Table entry.

   Description:
    This data type defines the structure of a Routing RREQ Forwarding Table
    entry.

   Remarks:
    None.
*/
typedef struct {
    /* Timer to control the RREQ sending on PLC */
    SYS_TIME_HANDLE timeHandlePLC;

    /* Timer to control the RREQ sending on RF */
    SYS_TIME_HANDLE timeHandleRF;

    /* RREQ originator */
    uint16_t origAddr;

    /* RREQ destination */
    uint16_t dstAddr;

    /* RREQ Sequence number */
    uint16_t seqNum;

    /* RREQ Route Cost */
    uint16_t routeCost;

    /* Flags received from RREQ */
    uint8_t flags;

    /* MetricType received from RREQ */
    uint8_t metricType;

    /* Hop Limit received from RREQ */
    uint8_t hopLimit;

    /* Hop Count left */
    uint8_t hopCount;

    /* RREQ Weak Link Count */
    uint8_t weakLinkCount;

    /* Reserved bits */
    uint8_t rsvBits;

    /* PLC Cluster Counter */
    uint8_t clusterCounterPLC;

    /* RF Cluster Counter */
    uint8_t clusterCounterRF;

    /* RREQ forwarding time expired for PLC */
    bool timeExpiredPLC;

    /* RREQ forwarding time expired for RF */
    bool timeExpiredRF;

} ROUTING_RREQ_FORWARDING_ENTRY;

// *****************************************************************************
/* Routing Pending RREQ Table Entry Definition

  Summary:
   Description of an element in the Pending RREQ Table.

  Description:
    This structure contains the data stored in each element of the Routing
    Pending RREQ table.

  Remarks:
    None.
*/
typedef struct _ROUTING_RREQ_TABLE_ENTRY
{
    /* Pointer to the previous object of the queue */
    struct _ROUTING_RREQ_TABLE_ENTRY *prev;

    /* Pointer to the next object of the queue */
    struct _ROUTING_RREQ_TABLE_ENTRY *next;

    /* Pointer to discover route entry */
    ROUTING_DISCOVER_ROUTE_ENTRY *pDiscoverRoute;

} ROUTING_PENDING_RREQ_ENTRY;

// *****************************************************************************
/* Routing TableS Definition

   Summary:
    Defines the structure of the Routing Tables.

   Description:
    This data type defines the structure of the Routing Tables. It defines the
    pointers and sizes of the different tables.

   Remarks:
    None.
*/
typedef struct
{
    /* Pointer to the Routing Table */
    ROUTING_TABLE_ENTRY* adpRoutingTable;

    /* Pointer to the list of blacklisted neighbours */
    ROUTING_BLACKLIST_ENTRY* adpBlacklistTable;

    /* Pointer to the Routing Set */
    ROUTING_TABLE_ENTRY* adpRoutingSet;
    
    /* Pointer to the list of destination addresses */
    uint16_t* adpDestinationAddressSet;

    /* Pointer to Pending RREQ Table */
    ROUTING_PENDING_RREQ_ENTRY* pendingRREQTable;

    /* Pointer to RREP Generation Table */
    ROUTING_RREP_GENERATION_ENTRY* rrepGenerationTable;

    /* Pointer to Route Discovery Table */
    ROUTING_DISCOVER_ROUTE_ENTRY* discoverRouteTable;

    /* Pointer to RREQ Forwarding Table */
    ROUTING_RREQ_FORWARDING_ENTRY* rreqForwardingTable;

    /* Number of entries in the Routing Table */
    uint16_t adpRoutingTableSize;

    /* Number of entries in the Routing Set */
    uint16_t adpRoutingSetSize;

    /* Number of entries in the Destination Address Set */
    uint16_t adpDestinationAddressSetSize;

    /* Number of entries in the list of blacklisted neighbours */
    uint8_t adpBlacklistTableSize;

    /* Number of entries in the Pending RREQ Table */
    uint8_t pendingRREQTableSize;

    /* Number of entries in the RREP Generation Table */
    uint8_t rrepGenerationTableSize;

    /* Number of entries in the Route Discovery Table */
    uint8_t discoverRouteTableSize;

    /* Number of entries in the RREQ Forwarding Table */
    uint8_t rreqForwardingTableSize;

} ROUTING_TABLES;

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // #ifndef _ROUTING_TYPES_H
