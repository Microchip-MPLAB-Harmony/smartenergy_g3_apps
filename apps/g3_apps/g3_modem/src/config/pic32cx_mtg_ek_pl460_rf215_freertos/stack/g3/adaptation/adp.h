/*******************************************************************************
  ADP Interface Header File

  Company:
    Microchip Technology Inc.

  File Name:
    adp.h

  Summary:
    ADP Interface Header File

  Description:
    The ADP provides a simple interface to manage the G3 Adaptation Layer. This
    file provides the interface definition for the ADP.
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

#ifndef ADP_H
#define ADP_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************
#include "adp_shared_types.h"
#include "adp_api_types.h"
#include "system/system.h"

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
/* ADP LBP Maximum NSDU length

   Summary:
    Defines the maximum length of a LBP NSDU.

   Description:
    This macro defines the maximum length of a LBP NSDU.

   Remarks:
    None.
*/
#define ADP_LBP_MAX_NSDU_LENGTH   100

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

#pragma pack(push,2)

// *****************************************************************************
/* ADP Data Confirm Parameters

   Summary:
    Defines the parameters for the ADP Data Confirm event handler function.

   Description:
    The structure contains the fields reported by the ADP Data Confirm
    event handler function.

   Remarks:
    None.
*/
typedef struct
{
    /* The handle of the NSDU confirmed by this primitive */
    uintptr_t nsduHandle;

    /* The status code (result) of a previous ADP Data Request */
    uint8_t status;

} ADP_DATA_CFM_PARAMS;

// *****************************************************************************
/* ADP Data Confirm Event Handler Function Pointer

  Summary:
    Pointer to a Data Confirm Event handler function.

  Description:
    This data type defines the required function signature for the ADP Data
    Confirm event handling callback function. The ADP Data Confirm primitive
    allows the upper layer to be notified of the completion of an ADP Data
    Request.

    A client must register a pointer using the event handling
    function whose function signature (parameter and return value types) matches
    the types specified by this function pointer in order to receive Data
    Confirm events back from module.

  Parameters:
    pDataConfirm - Pointer to structure containing parameters related to confirm

  Example:
    <code>
    App_DataConfirm(ADP_DATA_CFM_PARAMS *params)
    {
        if (params->status == G3_SUCCESS)
        {
            txHandler = params->nsduHandle;
        }
    }
    </code>

  Remarks:
    None.
*/
typedef void (*ADP_DATA_CFM_CALLBACK)(ADP_DATA_CFM_PARAMS* pDataCfm);

// *****************************************************************************
/* ADP Data Indication Parameters

   Summary:
    Defines the parameters for the ADP Data Indication event handler function.

   Description:
    The structure contains the fields reported by the ADP Data Indication
    event handler function.

   Remarks:
    None.
*/
typedef struct
{
    /* The received NSDU */
    const uint8_t* pNsdu;

    /* The size of the NSDU, in bytes; Up to 1280 bytes */
    uint16_t nsduLength;

    /* The value of the link quality during reception of the frame */
    uint8_t linkQualityIndicator;

} ADP_DATA_IND_PARAMS;

// *****************************************************************************
/* ADP Data Indication Event Handler Function Pointer

  Summary:
    Pointer to a Data Indication Event handler function.

  Description:
    This data type defines the required function signature for the ADP Data
    Indication event handling callback function. The ADP Data Indication
    primitive is used to transfer received data from the adaptation sublayer to
    the upper layer.

    A client must register a pointer using the event handling
    function whose function signature (parameter and return value types) matches
    the types specified by this function pointer in order to receive Data
    Indication events back from module.

  Parameters:
    pDataIndication - Pointer to structure containing parameters related to
                      indication

  Example:
    <code>
    App_DataIndication(ADP_DATA_IND_PARAMS *params)
    {
        if (params->linkQualityIndicator > 40)
        {

        }
    }
    </code>

  Remarks:
    None.
*/
typedef void (*ADP_DATA_IND_CALLBACK)(ADP_DATA_IND_PARAMS* pDataInd);

// *****************************************************************************
/* ADP Discovery Confirm Event Handler Function Pointer

  Summary:
    Pointer to a Discovery Confirm Event handler function.

  Description:
    This data type defines the required function signature for the ADP Discovery
    Confirm event handling callback function. The ADP Discovery Confirm
    primitive allows the upper layer to be notified of the completion of an
    ADP Discovery Request.

    A client must register a pointer using the event handling
    function whose function signature (parameter and return value types) matches
    the types specified by this function pointer in order to receive Discovery
    Confirm events back from module.

  Parameters:
    status - The status of the discovery request

  Example:
    <code>
    App_DiscoveryConfirm(uint8_t status)
    {
        if (status == G3_SUCCESS)
        {

        }
    }
    </code>

  Remarks:
    None.
*/
typedef void (*ADP_DISCOVERY_CFM_CALLBACK)(uint8_t status);

// *****************************************************************************
/* ADP Discovery Indication Event Handler Function Pointer

  Summary:
    Pointer to a Discovery Indication Event handler function.

  Description:
    This data type defines the required function signature for the ADP Discovery
    Indication event handling callback function. The ADP Discovery Indication
    primitive is generated by the ADP layer to notify the application about the
    discovery of a new PAN coordinator or LBA.

    A client must register a pointer using the event handling
    function whose function signature (parameter and return value types) matches
    the types specified by this function pointer in order to receive Discovery
    Indication events back from module.

  Parameters:
    pPanDescriptor - Pointer to PAN descriptor containing information about the
                     PAN

  Example:
    <code>
    App_DiscoveryIndication(ADP_PAN_DESCRIPTOR *panDescriptor)
    {
        uint8_t mediaType = MAC_WRP_MEDIA_TYPE_REQ_PLC_BACKUP_RF;

        if (panDescriptor->mediaType == MAC_WRP_MEDIA_TYPE_IND_RF)
        {
            mediaType = MAC_WRP_MEDIA_TYPE_REQ_RF_BACKUP_PLC;
        }

        LBP_AdpNetworkJoinRequest(panDescriptor->panId, panDescriptor->lbaAddress, mediaType);
    }
    </code>

  Remarks:
    None.
*/
typedef void (*ADP_DISCOVERY_IND_CALLBACK)(ADP_PAN_DESCRIPTOR* pPanDescriptor);

// *****************************************************************************
/* ADP Network Start Confirm Event Handler Function Pointer

  Summary:
    Pointer to a Network Start Confirm Event handler function.

  Description:
    This data type defines the required function signature for the ADP Network
    Start Confirm event handling callback function. The ADP Network Start
    Confirm primitive allows the upper layer to be notified of the completion of
    an ADP Network Start Request.

    A client must register a pointer using the event handling
    function whose function signature (parameter and return value types) matches
    the types specified by this function pointer in order to receive Network
    Start Confirm events back from module.

  Parameters:
    status - The status of the request

  Example:
    <code>
    App_NetworkStartConfirm(uint8_t status)
    {
        if (status == G3_SUCCESS)
        {

        }
    }
    </code>

  Remarks:
    None.
*/
typedef void (*ADP_NETWORK_START_CFM_CALLBACK)(uint8_t status);

// *****************************************************************************
/* ADP Reset Confirm Event Handler Function Pointer

  Summary:
    Pointer to a Reset Confirm Event handler function.

  Description:
    This data type defines the required function signature for the ADP Reset
    Confirm event handling callback function. The ADP Reset Confirm primitive
    allows the upper layer to be notified of the completion of an ADP Reset
    Request.

    A client must register a pointer using the event handling
    function whose function signature (parameter and return value types) matches
    the types specified by this function pointer in order to receive Reset
    Confirm events back from module.

  Parameters:
    status - The status of the request

  Example:
    <code>
    App_ResetConfirm(uint8_t status)
    {
        if (status == G3_SUCCESS)
        {

        }
    }
    </code>

  Remarks:
    None.
*/
typedef void (*ADP_RESET_CFM_CALLBACK)(uint8_t status);

// *****************************************************************************
/* ADP Set Confirm Parameters

   Summary:
    Defines the parameters for the ADP Set Confirm event handler function.

   Description:
    The structure contains the fields reported by the ADP Set Confirm
    event handler function.

   Remarks:
    None.
*/
typedef struct
{
    /* The identifier of the IB attribute to set */
    uint32_t attributeId;

    /* The index within the table of the specified IB attribute */
    uint16_t attributeIndex;

    /* The status of the set request */
    uint8_t status;

} ADP_SET_CFM_PARAMS;

// *****************************************************************************
/* ADP Set Confirm Event Handler Function Pointer

  Summary:
    Pointer to a Set Confirm Event handler function.

  Description:
    This data type defines the required function signature for the ADP Set
    Confirm event handling callback function. The ADP Set Confirm primitive
    allows the upper layer to be notified of the completion of an ADP Set
    Request.

    A client must register a pointer using the event handling
    function whose function signature (parameter and return value types) matches
    the types specified by this function pointer in order to receive Set
    Confirm events back from module.

  Parameters:
    pSetCfm - Pointer to structure containing parameters related to confirm

  Example:
    <code>
    App_SetConfirm(ADP_SET_CFM_PARAMS *params)
    {
        if (params->status == G3_SUCCESS)
        {

        }
    }
    </code>

  Remarks:
    None.
*/
typedef void (*ADP_SET_CFM_CALLBACK)(ADP_SET_CFM_PARAMS* pSetCfm);

// *****************************************************************************
/* ADP MAC Set Confirm Event Handler Function Pointer

  Summary:
    Pointer to a MAC Set Confirm Event handler function.

  Description:
    This data type defines the required function signature for the ADP MAC Set
    Confirm event handling callback function. The ADP MAC Set Confirm primitive
    allows the upper layer to be notified of the completion of an ADP MAC Set
    Request.

    A client must register a pointer using the event handling
    function whose function signature (parameter and return value types) matches
    the types specified by this function pointer in order to receive MAC Set
    Confirm events back from module.

  Parameters:
    pSetCfm - Pointer to structure containing parameters related to confirm

  Example:
    <code>
    App_MacSetConfirm(ADP_SET_CFM_PARAMS *params)
    {
        if (params->status == G3_SUCCESS)
        {

        }
    }
    </code>

  Remarks:
    None.
*/
typedef void (*ADP_MAC_SET_CFM_CALLBACK)(ADP_SET_CFM_PARAMS* pSetCfm);

// *****************************************************************************
/* ADP Get Confirm Parameters

   Summary:
    Defines the parameters for the ADP Get Confirm event handler function.

   Description:
    The structure contains the fields reported by the ADP Get Confirm
    event handler function.

   Remarks:
    None.
*/
typedef struct
{
    /* The identifier of the IB attribute read */
    uint32_t attributeId;

    /* The index within the table of the specified IB attribute read */
    uint16_t attributeIndex;

    /* The status of the get request */
    uint8_t status;

    /* The length of the value of the attribute read from the IB */
    uint8_t attributeLength;

    /* The value of the attribute read from the IB */
    uint8_t attributeValue[64];

} ADP_GET_CFM_PARAMS;

// *****************************************************************************
/* ADP Get Confirm Event Handler Function Pointer

  Summary:
    Pointer to a Get Confirm Event handler function.

  Description:
    This data type defines the required function signature for the ADP Get
    Confirm event handling callback function. The ADP Get Confirm primitive
    allows the upper layer to be notified of the completion of an ADP Get
    Request.

    A client must register a pointer using the event handling
    function whose function signature (parameter and return value types) matches
    the types specified by this function pointer in order to receive Get
    Confirm events back from module.

  Parameters:
    pGetCfm - Pointer to structure containing parameters related to confirm

  Example:
    <code>
    App_GetConfirm(ADP_GET_CFM_PARAMS *params)
    {
        if (params->status == G3_SUCCESS)
        {

        }
    }
    </code>

  Remarks:
    None.
*/
typedef void (*ADP_GET_CFM_CALLBACK)(ADP_GET_CFM_PARAMS* pGetCfm);

// *****************************************************************************
/* ADP MAC Get Confirm Parameters

   Summary:
    Defines the parameters for the ADP MAC Get Confirm event handler function.

   Description:
    The structure contains the fields reported by the ADP MAC Get Confirm
    event handler function.

   Remarks:
    None.
*/
typedef struct
{
    /* The identifier of the IB attribute read */
    uint32_t attributeId;

    /* The index within the table of the specified IB attribute read */
    uint16_t attributeIndex;

    /* The status of the set request */
    uint8_t status;

    /* The length of the value of the attribute read from the IB */
    uint8_t attributeLength;

    /* The value of the attribute read from the IB */
    uint8_t attributeValue[144];

} ADP_MAC_GET_CFM_PARAMS;

// *****************************************************************************
/* ADP MAC Get Confirm Event Handler Function Pointer

  Summary:
    Pointer to a MAC Get Confirm Event handler function.

  Description:
    This data type defines the required function signature for the ADP MAC Get
    Confirm event handling callback function. The ADP MAC Get Confirm primitive
    allows the upper layer to be notified of the completion of an
    ADP MAC Get Request.

    A client must register a pointer using the event handling
    function whose function signature (parameter and return value types) matches
    the types specified by this function pointer in order to receive MAC Get
    Confirm events back from module.

  Parameters:
    pGetCfm - Pointer to structure containing parameters related to confirm

  Example:
    <code>
    App_MacGetConfirm(ADP_MAC_GET_CFM_PARAMS *params)
    {
        if (params->status == G3_SUCCESS)
        {

        }
    }
    </code>

  Remarks:
    None.
*/
typedef void (*ADP_MAC_GET_CFM_CALLBACK)(ADP_MAC_GET_CFM_PARAMS* pGetCfm);

// *****************************************************************************
/* ADP LBP Confirm Parameters

   Summary:
    Defines the parameters for the ADP LBP Confirm event handler function.

   Description:
    The structure contains the fields reported by the ADP LBP Confirm
    event handler function.

   Remarks:
    None.
*/
typedef struct
{
    /* The handle of the NSDU confirmed by this primitive */
    uintptr_t nsduHandle;

    /* The status code of a previous ADP LBP Request */
    uint8_t status;

} ADP_LBP_CFM_PARAMS;

// *****************************************************************************
/* ADP LBP Confirm Event Handler Function Pointer

  Summary:
    Pointer to a LBP Confirm Event handler function.

  Description:
    This data type defines the required function signature for the ADP LBP
    Confirm event handling callback function. The ADP LBP Confirm primitive
    allows the upper layer to be notified of the completion of an LBP Request.

    A client must register a pointer using the event handling
    function whose function signature (parameter and return value types) matches
    the types specified by this function pointer in order to receive LBP
    Confirm events back from module.

  Parameters:
    pLbpCfm - Pointer to structure containing parameters related to confirm

  Example:
    <code>
    App_LbpConfirm(ADP_LBP_CFM_PARAMS *params)
    {
        if (params->status == G3_SUCCESS)
        {

        }
    }
    </code>

  Remarks:
    None.
*/
typedef void (*ADP_LBP_CFM_CALLBACK)(ADP_LBP_CFM_PARAMS* pLbpCfm);

// *****************************************************************************
/* ADP LBP Indication Parameters

   Summary:
    Defines the parameters for the ADP LBP Indication event handler function.

   Description:
    The structure contains the fields reported by the ADP LBP Indication
    event handler function.

   Remarks:
    None.
*/
typedef struct
{
    /* The received NSDU */
    const uint8_t* pNsdu;

    /* The size of the NSDU, in bytes; Up to ADP_LBP_MAX_NSDU_LENGTH bytes */
    uint16_t nsduLength;

    /* Source Address of the LBP frame. Short Address for LBA or LBS frames,
     * extended for LBD. */
    ADP_ADDRESS srcAddr;

    /* The Security Level of the received frame */
    uint8_t securityLevel;

} ADP_LBP_IND_PARAMS;

// *****************************************************************************
/* ADP LBP Indication Event Handler Function Pointer

  Summary:
    Pointer to a LBP Indication Event handler function.

  Description:
    This data type defines the required function signature for the ADP LBP
    Indication event handling callback function. The ADP LBP Indication
    primitive is used to transfer received data from the adaptation sublayer to
    the upper layer.

    A client must register a pointer using the event handling
    function whose function signature (parameter and return value types) matches
    the types specified by this function pointer in order to receive LBP
    Indication events back from module.

  Parameters:
    pLbpInd - Pointer to structure containing parameters related to indication

  Example:
    <code>
    App_LbpIndication(ADP_LBP_IND_PARAMS *params)
    {
        if (params->linkQualityIndicator > 40)
        {

        }
    }
    </code>

  Remarks:
    None.
*/
typedef void (*ADP_LBP_IND_CALLBACK)(ADP_LBP_IND_PARAMS* pLbpInd);

// *****************************************************************************
/* ADP Route Discovery Confirm Event Handler Function Pointer

  Summary:
    Pointer to a Route Discovery Confirm Event handler function.

  Description:
    This data type defines the required function signature for the ADP Route
    Discovery Confirm event handling callback function. The ADP Route Discovery
    Confirm primitive allows the upper layer to be notified of the completion of
    an ADP Route Discovery Request.

    A client must register a pointer using the event handling
    function whose function signature (parameter and return value types) matches
    the types specified by this function pointer in order to receive Route
    Discovery Confirm events back from module.

  Parameters:
    status - The status of the route discovery

  Example:
    <code>
    App_DiscoveryConfirm(uint8_t status)
    {
        if (status == G3_SUCCESS)
        {

        }
    }
    </code>

  Remarks:
    None.
*/
typedef void (*ADP_ROUTE_DISCOVERY_CFM_CALLBACK)(uint8_t status);

// *****************************************************************************
/* ADP Path Discovery Confirm Parameters

   Summary:
    Defines the parameters for the ADP Path Discovery Confirm event handler
    function.

   Description:
    The structure contains the fields reported by the ADP Path Discovery Confirm
    event handler function.

   Remarks:
    None.
*/
typedef struct
{
    /* The short unicast destination address of the path discovery */
    uint16_t dstAddr;

    /* The originator of the path reply */
    uint16_t origAddr;

    /* The status of the path discovery (status can be INCOMPLETE and the other
     * parameters contain the	discovered path) */
    uint8_t status;

    /* Path metric type */
    uint8_t metricType;

    /* Number of path hops in the forward table */
    uint8_t forwardHopsCount;

    /* Number of path hops in the reverse table */
    uint8_t reverseHopsCount;

    /* Table with the information of each hop in forward direction (according
     * to forwardHopsCount) */
    ADP_HOP_DESCRIPTOR forwardPath[16];

    /* Table with the information of each hop in reverse direction (according
     * to reverseHopsCount) */
    ADP_HOP_DESCRIPTOR reversePath[16];

} ADP_PATH_DISCOVERY_CFM_PARAMS;

// *****************************************************************************
/* ADP Path Discovery Confirm Event Handler Function Pointer

  Summary:
    Pointer to a Path Discovery Confirm Event handler function.

  Description:
    This data type defines the required function signature for the ADP Path
    Discovery Confirm event handling callback function. The ADP Path Discovery
    Confirm primitive allows the upper layer to be notified of the completion of
    an ADP Path Discovery Request.

    A client must register a pointer using the event handling
    function whose function signature (parameter and return value types) matches
    the types specified by this function pointer in order to receive Path
    Discovery Confirm events back from module.

  Parameters:
    pPathDiscoveryCfm - Pointer to structure containing parameters related to
                        confirm

  Example:
    <code>
    App_PathDiscoveryConfirm(ADP_LBP_CFM_PARAMS *params)
    {
        if (params->status == G3_SUCCESS)
        {

        }
    }
    </code>

  Remarks:
    None.
*/
typedef void (*ADP_PATH_DISCOVERY_CFM_CALLBACK)(ADP_PATH_DISCOVERY_CFM_PARAMS* pPathDiscoveryCfm);

// *****************************************************************************
/* ADP Network Status Indication Parameters

   Summary:
    Defines the parameters for the ADP Network Status Indication event handler
    function.

   Description:
    The structure contains the fields reported by the ADP Network Status
    Indication event handler function.

   Remarks:
    None.
*/
typedef struct
{
    /* The 16-bit PAN identifier of the device from which the frame was received
     * or to which the frame was being sent */
    uint16_t panId;

    /* The status/event reported by the primitive */
    uint8_t status;

    /* The security level used by the received frame */
    uint8_t securityLevel;

    /* The index of the key used by the originator of the received
     * frame */
    uint8_t keyIndex;

    /* The medium (PLC/RF) from which the frame was received */
    uint8_t mediaType;

    /* The individual device address of the entity from which the frame causing
     * the error originated */
    ADP_ADDRESS srcDeviceAddress;

    /* The individual device address of the device for which the frame was
     * intended */
    ADP_ADDRESS dstDeviceAddress;

} ADP_NETWORK_STATUS_IND_PARAMS;

// *****************************************************************************
/* ADP Network Status Indication Event Handler Function Pointer

  Summary:
    Pointer to a Network Status Indication Event handler function.

  Description:
    This data type defines the required function signature for the ADP Network
    Status Indication event handling callback function. The ADP Network Status
    Indication primitive allows the next higher layer of a PAN coordinator or a
    coordinator to be notified when a particular event occurs on the PAN.

    A client must register a pointer using the event handling
    function whose function signature (parameter and return value types) matches
    the types specified by this function pointer in order to receive Network Status
    Indication events back from module.

  Parameters:
    pNetworkStatusInd - Pointer to structure containing parameters related to
                        indication

  Example:
    <code>
    App_NetworkStatusIndication(ADP_NETWORK_STATUS_IND_PARAMS *params)
    {

    }
    </code>

  Remarks:
    None.
*/
typedef void (*ADP_NETWORK_STATUS_IND_CALLBACK)(ADP_NETWORK_STATUS_IND_PARAMS* pNetworkStatusInd);

/* MISRA C-2012 deviation block start */
/* MISRA C-2012 Rule 6.1 deviated 4 times.  Deviation record ID - H3_MISRAC_2012_R_6_1_DR_1 */

// *****************************************************************************
/* ADP Buffer Indication Parameters

   Summary:
    Defines the parameters for the ADP Buffer Indication event handler
    function.

   Description:
    The structure contains the fields reported by the ADP Buffer
    Indication event handler function.

   Remarks:
    None.
*/
typedef union
{
    struct
    {
        /* Large buffers availability */
        /* '1' if buffers available, '0' otherwise */
        uint8_t largeBuffersAvailable : 1;

        /* Medium buffers availability */
        /* '1' if buffers available, '0' otherwise */
        uint8_t mediumBuffersAvailable : 1;

        /* Small buffers availability */
        /* '1' if buffers available, '0' otherwise */
        uint8_t smallBuffersAvailable : 1;

        /* Reserved */
        uint8_t :5;
    };

    /* Bitmap containig all previous information */
    uint8_t bufferIndicationBitmap;

} ADP_BUFFER_IND_PARAMS;

/* MISRA C-2012 deviation block end */

// *****************************************************************************
/* ADP Buffer Indication Event Handler Function Pointer

  Summary:
    Pointer to a Buffer Indication Event handler function.

  Description:
    This data type defines the required function signature for the ADP Buffer
    Indication event handling callback function. The ADP Buffer Indication
    primitive allows the next higher layer to be notified when the modem has
    reached its capability limit to perform the next frame.

    A client must register a pointer using the event handling
    function whose function signature (parameter and return value types) matches
    the types specified by this function pointer in order to receive Buffer
    Indication events back from module.

  Parameters:
    bufferInd - Pointer to bitmap providing availability of Tx buffers

  Example:
    <code>
    App_BufferIndication(ADP_BUFFER_IND_PARAMS* bufferInd)
    {
        if (bufferInd->largeBuffersAvailable == 1)
        {

        }

        if (bufferInd->mediumBuffersAvailable == 1)
        {

        }

        if (bufferInd->smallBuffersAvailable == 1)
        {

        }
    }
    </code>

  Remarks:
    This function is invoked whenever the availability status of
    any kind of buffer changes.
*/
typedef void (*ADP_BUFFER_IND_CALLBACK)(ADP_BUFFER_IND_PARAMS* bufferInd);

// *****************************************************************************
/* ADP PREQ Indication Event Handler Function Pointer

  Summary:
    Pointer to a PREQ Indication Event handler function.

  Description:
    This data type defines the required function signature for the ADP PREQ
    Indication event handling callback function. The ADP PREQ Indication
    primitive allows the next higher layer to be notified when a PREQ frame is
    received in unicast mode with Originator Address equal to Coordinator
    Address and with Destination Address equal to Device Address.

    A client must register a pointer using the event handling function whose
    function signature (parameter and return value types) matches the types
    specified by this function pointer in order to receive PREQ Indication
    events back from module.

  Parameters:
    None.

  Example:
    <code>
    App_NetworkLeaveIndication(void)
    {

    }
    </code>

  Remarks:
    None.
*/
typedef void (*ADP_PREQ_IND_CALLBACK)(void);

// *****************************************************************************
/* ADP Non-Volatile Data Indication Parameters

   Summary:
    Defines the parameters for the ADP Non-Volatile Data Indication event
    handler function.

   Description:
    The structure contains the fields reported by the ADP Non-Volatile Data
    Indication event handler function.

   Remarks:
    None.
*/
typedef struct
{
    /* MAC frame counter for PLC */
    uint32_t frameCounter;

    /* MAC frame counter for RF */
    uint32_t frameCounterRF;

    /* Discover sequence number */
    uint16_t discoverSeqNumber;

    /* Broadcast sequence number */
    uint8_t broadcastSeqNumber;

} ADP_NON_VOLATILE_DATA_IND_PARAMS;

// *****************************************************************************
/* ADP Non-Volatile Data Indication Event Handler Function Pointer

  Summary:
    Pointer to a Non-Volatile Data Indication Event handler function.

  Description:
    This data type defines the required function signature for the ADP
    Non-Volatile Data Indication event handling callback function. The ADP
    Non-Volatile Data Indication primitive allows the next higher layer to be
    notified when non-volatile stored data must be updated to protect the system
    in case of critical failure.

    A client must register a pointer using the event handling
    function whose function signature (parameter and return value types) matches
    the types specified by this function pointer in order to receive Non-Volatile
    Data Indication events back from module.

  Parameters:
    pNonVolatileDataInd - Pointer to structure containing parameters related to
                          indication

  Example:
    <code>
    App_NonVolatileDataIndication(ADP_NON_VOLATILE_DATA_IND_PARAMS *params)
    {

    }
    </code>

  Remarks:
    None.
*/
typedef void (*ADP_NON_VOLATILE_DATA_IND_CALLBACK)(ADP_NON_VOLATILE_DATA_IND_PARAMS* pNonVolatileDataInd);

// *****************************************************************************
/* ADP Route Not Found Indication Parameters

   Summary:
    Defines the parameters for the ADP Route Not Found Indication event handler
    function.

   Description:
    The structure contains the fields reported by the ADP Route Not Found
    Indication event handler function.

   Remarks:
    None.
*/
typedef struct
{
    /* The NSDU failed to be delivered */
    const uint8_t* pNsdu;

    /* The size of the NSDU failed to be delivered, in bytes; Range: 0 - 1280 */
    uint16_t nsduLength;

    /* Address of the frame originator */
    uint16_t srcAddr;

    /* Address of the frame final destination */
    uint16_t destAddr;

    /* Address of the next hop to frame final destination */
    uint16_t nextHopAddr;

    /* Address of the previous hop to frame final destination */
    uint16_t previousHopAddr;

    /* Route Cost of the broken route (0xFFFF if route did not exist) */
    uint16_t routeCost;

    /* Hop Count of the broken route (0xFF if route did not exist) */
    uint8_t hopCount;

    /* Weak Link Count of the broken route (0xFF if route did not exist) */
    uint8_t weakLinkCount;

    /* Indicates whether the route existed and just failed */
    bool routeJustBroken;

    /* Indicates whether the Header of the frame dropped due to No Route was
     * compressed */
    bool compressedHeader;

} ADP_ROUTE_NOT_FOUND_IND_PARAMS;

// *****************************************************************************
/* ADP Route Not Found Indication Event Handler Function Pointer

  Summary:
    Pointer to a Route Not Found Indication Event handler function.

  Description:
    This data type defines the required function signature for the ADP Route Not
    Found Indication event handling callback function. The ADP Route Not Found
    Indication primitive is used to indicate the upper layer that a route is not
    available.

    A client must register a pointer using the event handling
    function whose function signature (parameter and return value types) matches
    the types specified by this function pointer in order to receive Route Not
    Found Indication events back from module.

  Parameters:
    pRouteNotFoundInd - Pointer to structure containing parameters related to
                        indication

  Example:
    <code>
    App_RouteNotFoundIndication(ADP_ROUTE_NOT_FOUND_IND_PARAMS *params)
    {

    }
    </code>

  Remarks:
    None.
*/
typedef void (*ADP_ROUTE_NOT_FOUND_IND_CALLBACK)(ADP_ROUTE_NOT_FOUND_IND_PARAMS* pRouteNotFoundInd);

// *****************************************************************************
/* ADP Data Callback Notifications Structure

   Summary:
    Set of event handler function pointers to receive data events from ADP.

   Description:
    Defines the set of callback functions that ADP uses to generate data events
    to upper layer.

   Remarks:
    In case an event is to be ignored, setting its corresponding callback
    function to NULL will lead to the event not being generated.
*/
typedef struct
{
    ADP_DATA_CFM_CALLBACK              dataConfirm;
    ADP_DATA_IND_CALLBACK              dataIndication;
} ADP_DATA_NOTIFICATIONS;

// *****************************************************************************
/* ADP Management Callback Notifications Structure

   Summary:
    Set of event handler function pointers to receive management events from
    ADP.

   Description:
    Defines the set of callback functions that ADP uses to generate management
    events to upper layer.

   Remarks:
    In case an event is to be ignored, setting its corresponding callback
    function to NULL will lead to the event not being generated.
*/
typedef struct
{
    ADP_DISCOVERY_CFM_CALLBACK         discoveryConfirm;
    ADP_DISCOVERY_IND_CALLBACK         discoveryIndication;
    ADP_NETWORK_START_CFM_CALLBACK     networkStartConfirm;
    ADP_RESET_CFM_CALLBACK             resetConfirm;
    ADP_SET_CFM_CALLBACK               setConfirm;
    ADP_MAC_SET_CFM_CALLBACK           macSetConfirm;
    ADP_GET_CFM_CALLBACK               getConfirm;
    ADP_MAC_GET_CFM_CALLBACK           macGetConfirm;
    ADP_ROUTE_DISCOVERY_CFM_CALLBACK   routeDiscoveryConfirm;
    ADP_PATH_DISCOVERY_CFM_CALLBACK    pathDiscoveryConfirm;
    ADP_NETWORK_STATUS_IND_CALLBACK    networkStatusIndication;
    ADP_PREQ_IND_CALLBACK              preqIndication;
    ADP_NON_VOLATILE_DATA_IND_CALLBACK nonVolatileDataIndication;
    ADP_ROUTE_NOT_FOUND_IND_CALLBACK   routeNotFoundIndication;
    ADP_BUFFER_IND_CALLBACK            bufferIndication;
} ADP_MANAGEMENT_NOTIFICATIONS;

// *****************************************************************************
/* ADP Callback Notifications to LBP Structure

   Summary:
    Set of event handler function pointers to receive events from ADP to LBP.

   Description:
    Defines the set of callback functions that ADP uses to generate events to
    upper layer for LBP.

   Remarks:
    In case an event is to be ignored, setting its corresponding callback
    function to NULL will lead to the event not being generated.
*/
typedef struct
{
    ADP_LBP_CFM_CALLBACK               lbpConfirm;
    ADP_LBP_IND_CALLBACK               lbpIndication;
} ADP_NOTIFICATIONS_TO_LBP;

// *****************************************************************************
/* ADP Initialization Data

  Summary:
    Defines the data required to initialize the ADP Layer.

  Description:
    This data type defines the data required to initialize the G3 ADP layer.

  Remarks:
    None.
*/

typedef struct
{
    /* Pointer to start of 1280-byte buffers */
    void*           pBuffers1280;

    /* Pointer to start of 400-byte buffers */
    void*           pBuffers400;

    /* Pointer to start of 100-byte buffers */
    void*           pBuffers100;

    /* Pointer to start of process queue entries */
    void*           pProcessQueueEntries;

    /* Pointer to start of fragmented transfer entries */
    void*           pFragmentedTransferEntries;

    /* ADP fragmentation size */
    uint16_t        fragmentSize;

    /* Number of 1280-byte buffers */
    uint8_t         numBuffers1280;

    /* Number of 400-byte buffers */
    uint8_t         numBuffers400;

    /* Number of 100-byte buffers */
    uint8_t         numBuffers100;

    /* Number of process queue entries */
    uint8_t         numProcessQueueEntries;

    /* Number of fragmented transfer entries */
    uint8_t         numFragmentedTransferEntries;

    /* ADP task rate in milliseconds */
    uint8_t         taskRateMs;

} ADP_INIT;

#pragma pack(pop)

// *****************************************************************************
// *****************************************************************************
// Section: ADP Interface Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ ADP_Initialize
    (
      const SYS_MODULE_INDEX index,
      const SYS_MODULE_INIT * const init
    )

  Summary:
    Initializes the ADP module for the specified index.

  Description:
    This routine initializes the ADP module making it ready for clients
    to open and use.

  Precondition:
    None.

  Parameters:
    index - Identifier for the instance to be initialized (single instance allowed)

    init  - Pointer to the init data structure containing any data necessary to
            initialize the module.

  Returns:
    If successful, returns a valid module instance object.
    Otherwise, returns SYS_MODULE_OBJ_INVALID.

  Example:
    <code>
    ADP_INIT initData;
    SYS_MODULE_OBJ sysObjAdp;

    sysObjAdp = ADP_Initialize(G3_ADP_INDEX_0, (SYS_MODULE_INIT *)&initData);
    if (sysObjAdp == SYS_MODULE_OBJ_INVALID)
    {

    }
    </code>

  Remarks:
    This routine must be called before any other ADP routine is called.
*/
SYS_MODULE_OBJ ADP_Initialize(const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init);

// *****************************************************************************
/* Function:
    void ADP_Tasks
    (
      SYS_MODULE_OBJ object
    )

  Summary:
    Maintains ADP State Machine.

  Description:
    Maintains the Adaptation Layer State Machine.

  Precondition:
    ADP_Initialize routine must have been called before,
    and its returned object used when calling this function.

  Parameters:
    object - Identifier for the object instance

  Returns:
    None.

  Example:
    <code>
    ADP_INIT initData;
    SYS_MODULE_OBJ sysObjAdp;
    sysObjAdp = ADP_Initialize(G3_ADP_INDEX_0, (SYS_MODULE_INIT *)&initData);

    while (true)
    {
        ADP_Tasks(sysObjAdp);
    }
    </code>

  Remarks:
    None.
*/
void ADP_Tasks(SYS_MODULE_OBJ object);

// *****************************************************************************
/* Function:
    void ADP_Open(ADP_BAND band)

  Summary:
    Opens the ADP module and starts the process that makes it ready for clients
    to use.

  Description:
    This routine opens the ADP module, initializes the ADP data structures and
    starts the process that makes it ready for clients to use.
    In case MAC PLC layer is present, PLC band is specified here.

    The APIs cannot be mixed, if the stack is initialized in ADP mode then only
    the ADP functions can be used and if the stack is initialized in MAC mode
    then only MAC functions can be used.

  Precondition:
    None.

  Parameters:
    band           - Working band for PLC (should be inline with the hardware)

  Returns:
    None.

  Example:
    <code>
    ADP_Open(ADP_BAND_CENELEC_A);
    </code>

  Remarks:
    This routine must be called before any other ADP API function, execpt
    ADP_Initialize.
    After calling this function, ADP_Status must be used to check if it is ready
    before calling any other ADP API function.
*/
void ADP_Open(ADP_BAND band);

// *****************************************************************************
/* Function:
    ADP_STATUS ADP_Status(void)

  Summary:
    The ADP_Status primitive retrieves the Status of G3 ADP layer.

  Description:
    This primitive is intended to be called before using G3 ADP layer to ensure
    they are ready to be used.

  Precondition:
    None.

  Parameters:
    None

  Returns:
    Status of G3 ADP layer(s) as a ADP_STATUS code.

  Example:
    <code>
    ADP_STATUS status;
    status = ADP_Status();
    if (status == ADP_STATUS_READY)
    {

    }
    </code>

  Remarks:
    None.
*/
ADP_STATUS ADP_Status(void);

// *****************************************************************************
/* Function:
    void ADP_SetLBPStatusConnection(bool lbpConnected)

  Summary:
    The ADP_SetLBPStatusConnection primitive sets the LBP status connection.

  Description:
    This primitive is intended to be called from LBP module to inform ADP of the
    availability of the G3 Network.

  Precondition:
    None

  Parameters:
    lbpConnected - Flag to update the status of the G3 Network connection

  Returns:
    None.

  Example:
    <code>
    ADP_SetLBPStatusConnection(true);
    </code>

  Remarks:
    None.
*/
void ADP_SetLBPStatusConnection(bool lbpConnected);

// *****************************************************************************
/* Function:
    void ADP_SetDataNotifications(ADP_DATA_NOTIFICATIONS* pNotifications);

  Summary:
    Sets ADP data notifications.

  Description:
    This routine sets the ADP data notifications. Callback handlers for event
    notification are set in this function.

  Precondition:
    None.

  Parameters:
    pNotifications - Structure with callbacks used to notify ADP data specific
                     events

  Returns:
    None.

  Example:
    <code>
    ADP_DATA_NOTIFICATIONS adpNotifications = {
        .dataConfirm = appDataConfirm,
        .dataIndication = appDataIndication
    };

    ADP_SetDataNotifications(&adpNotifications);
    </code>

  Remarks:
    None.
*/
void ADP_SetDataNotifications(ADP_DATA_NOTIFICATIONS* pNotifications);

// *****************************************************************************
/* Function:
    void ADP_SetManagementNotifications(ADP_MANAGEMENT_NOTIFICATIONS* pNotifications);

  Summary:
    Sets ADP management notifications.

  Description:
    This routine sets the ADP management notifications. Callback handlers for event
    notification are set in this function.

  Precondition:
    None.

  Parameters:
    pNotifications - Structure with callbacks used to notify ADP management
                     specific events

  Returns:
    None.

  Example:
    <code>
    ADP_MANAGEMENT_NOTIFICATIONS adpNotifications = {
        .discoveryConfirm = appDiscoveryConfirm,
        .discoveryIndication = appDiscoveryIndication,
        .networkStartConfirm = NULL,
        .resetConfirm = NULL,
        .setConfirm = NULL,
        .macSetConfirm = NULL,
        .getConfirm = NULL,
        .macGetConfirm = NULL,
        .routeDiscoveryConfirm = appRouteConfirm,
        .pathDiscoveryConfirm = appPathConfirm,
        .networkStatusIndication = appNetworkStatusIndication,
        .preqIndication = NULL,
        .nonVolatileDataIndication = appNonVolatileDataIndication,
        .routeNotFoundIndication = appRouteNotFoundIndication,
        .bufferIndication = appBufferIndication
    };

    ADP_SetManagementNotifications(&adpNotifications);
    </code>

  Remarks:
    None.
*/
void ADP_SetManagementNotifications(ADP_MANAGEMENT_NOTIFICATIONS* pNotifications);

// *****************************************************************************
/* Function:
    void ADP_SetNotificationsToLbp(ADP_NOTIFICATIONS_TO_LBP* pNotifications);

  Summary:
    Sets ADP notifications to LBP.

  Description:
    This routine sets the ADP notifications to LBP. Callback handlers for event
    notification are set in this function.

  Precondition:
    None.

  Parameters:
    pNotifications - Structure with callbacks used to notify ADP LBP specific
                     events

  Returns:
    None.

  Example:
    <code>
    ADP_NOTIFICATIONS_TO_LBP adpNotifications = {
        .lbpConfirm = appLbpConfirm,
        .lbpIndication = appLbpIndication
    };

    ADP_SetNotificationsToLbp(&adpNotifications);
    </code>

  Remarks:
    None.
*/
void ADP_SetNotificationsToLbp(ADP_NOTIFICATIONS_TO_LBP* pNotifications);

// *****************************************************************************
/* Function:
    ADP_AVAILABLE_MAC_LAYERS ADP_GetAvailableMacLayers
    (
      void
    )

  Summary:
    The ADP_GetAvailableMacLayers primitive gets the available MAC layer(s).

  Description:
    G3 projects may include PLC MAC, RF MAC, or both. This funtion allows upper
    layers to get which MAC layer(s) is/are available at runtime.

  Precondition:
    ADP_Initialize and ADP_Open must have been called before.

  Parameters:
    None

  Returns:
    Mac layer availability as an ADP_AVAILABLE_MAC_LAYERS enumerated value.

  Example:
    <code>
    ADP_Open(ADP_BAND_CENELEC_A);

    ADP_AVAILABLE_MAC_LAYERS availableLayers;
    availableLayers = ADP_GetAvailableMacLayers();

    if (availableLayers == ADP_AVAILABLE_MAC_PLC)
    {

    }
    else if (availableLayers == ADP_AVAILABLE_MAC_RF)
    {

    }
    else
    {

    }
    </code>

  Remarks:
    None.
*/
ADP_AVAILABLE_MAC_LAYERS ADP_GetAvailableMacLayers(void);

// *****************************************************************************
/* Function:
    void ADP_DataRequest(uint16_t nsduLength, const uint8_t *pNsdu,
        uintptr_t nsduHandle, bool discoverRoute, uint8_t qualityOfService);

  Summary:
    This primitive requests the transfer of a PDU to another device or multiple
    devices.

  Description:
    The ADP Data Request primitive is used to transfer data to to another device
    or multiple devices in the G3 Network.

    Result is provided in the corresponding ADP Data Confirm callback.

  Precondition:
    ADP_Initialize and ADP_Open must have been called before.

  Parameters:
    nsduLength       - The size of the NSDU, in bytes; Up to 1280

    pNsdu            - Pointer to NSDU to send; should be a valid IPv6 packet

    nsduHandle       - The handle of the NSDU to transmit. This parameter is
                       used to identify in the ADP Data Confirm primitive which
                       request it is concerned with. It can be randomly chosen
                       by the application layer.

    discoverRoute    - If true, a route discovery procedure will be performed
                       prior to sending the frame if a route to the destination
                       is not available in the routing table.
                       If false, no route discovery is performed.

    qualityOfService - The requested quality of service (QoS) of the frame to
                       send. Allowed values are 0 (normal priority) and 1 (high
                       priority).

  Returns:
    None.

  Example:
    <code>
    uint8_t nsdu[1280];
    uint16_t nsduLength;

    ADP_DataRequest(nsduLength, nsdu, 0, true, 0);
    </code>

  Remarks:
    None.
*/
void ADP_DataRequest(uint16_t nsduLength, const uint8_t *pNsdu,
    uintptr_t nsduHandle, bool discoverRoute, uint8_t qualityOfService);

// *****************************************************************************
/* Function:
    void ADP_NoIPDataRequest(uint16_t apduLength, const uint8_t *pApdu,
        uint16_t dstAddr, uintptr_t apduHandle, bool discoverRoute,
        uint8_t qualityOfService);

  Summary:
    This primitive requests the transfer of a PDU to another device or multiple
    devices providing a 6LowPAN 16-bit Destination Address.

  Description:
    The ADP No IP Data Request primitive is used to transfer data to to another device
    or multiple devices in the G3 Network, providing a 6LowPAN 16-bit
    Destination Address. PDU does not have to be an IPv6 packet, any protocol
    data can be sent using this primitive.

    Result is provided in the corresponding ADP Data Confirm callback.

  Precondition:
    ADP_Initialize and ADP_Open must have been called before.

  Parameters:
    apduLength       - The size of the APDU, in bytes; Up to 1280

    pApdu            - Pointer to APDU to send; can be any App protocol

    dstAddr          - The Destination Short Address. Can be a unicast address,
                       a 6LowPAN Group Address, or the 6LowPAN BROADCAST ADDRESS
                       (0x8001)

    apduHandle       - The handle of the APDU to transmit. This parameter is
                       used to identify in the ADP Data Confirm primitive which
                       request it is concerned with. It can be randomly chosen
                       by the application layer.

    discoverRoute    - If true, a route discovery procedure will be performed
                       prior to sending the frame if a route to the destination
                       is not available in the routing table.
                       If false, no route discovery is performed.

    qualityOfService - The requested quality of service (QoS) of the frame to
                       send. Allowed values are 0 (normal priority) and 1 (high
                       priority).

  Returns:
    None.

  Example:
    <code>
    uint8_t apdu[1280];
    uint16_t apduLength;

    ADP_NoIPDataRequest(apduLength, apdu, 0x0001, 0, true, 0);
    </code>

  Remarks:
    None.
*/
void ADP_NoIPDataRequest(uint16_t apduLength, const uint8_t *pApdu,
    uint16_t dstAddr, uintptr_t apduHandle, bool discoverRoute,
    uint8_t qualityOfService);

// *****************************************************************************
/* Function:
    void ADP_DiscoveryRequest(uint8_t duration);

  Summary:
    This primitive scans for networks operating in its POS.

  Description:
    The ADP Discovery Request primitive allows the upper layer to scan for
    networks operating in its POS.

    Result is provided in the corresponding ADP Discovery Confirm callback.

  Precondition:
    ADP_Initialize and ADP_Open must have been called before.

  Parameters:
    duration - The number of seconds the scan shall last

  Returns:
    None.

  Example:
    <code>
    ADP_DiscoveryRequest(60);
    </code>

  Remarks:
    None.
*/
void ADP_DiscoveryRequest(uint8_t duration);

// *****************************************************************************
/* Function:
    void ADP_NetworkStartRequest(uint16_t panId);

  Summary:
    This primitive requests the starting of a new network.

  Description:
    The ADP Network Start Request primitive allows the upper layer to request
    the starting of a new network. It shall only be invoked by a device
    designated as the PAN coordinator during the factory process.

    Result is provided in the corresponding ADP Network Start Confirm callback.

  Precondition:
    ADP_Initialize and ADP_Open must have been called before.

  Parameters:
    panId - The PANId of the network to create; determined at the application
            level

  Returns:
    None.

  Example:
    <code>
    ADP_NetworkStartRequest(0x1234);
    </code>

  Remarks:
    None.
*/
void ADP_NetworkStartRequest(uint16_t panId);

// *****************************************************************************
/* Function:
    void ADP_ResetRequest(void);

  Summary:
    This primitive performs a reset of the adaptation sublayer.

  Description:
    The ADP Reset Request primitive performs a reset of the adaptation sublayer
    and allows the resetting of the MIB attributes.

    Result is provided in the corresponding ADP Reset Confirm callback.

  Precondition:
    ADP_Initialize and ADP_Open must have been called before.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    ADP_ResetRequest();
    </code>

  Remarks:
    None.
*/
void ADP_ResetRequest(void);

// *****************************************************************************
/* Function:
    void ADP_GetRequest(uint32_t attributeId, uint16_t attributeIndex);

  Summary:
    This primitive gets the value of an attribute from the ADP information base.

  Description:
    The ADP Get Request primitive allows the upper layer to get the value of an
    attribute from the ADP information base.

    Result is provided in the corresponding ADP Get Confirm callback.

  Precondition:
    ADP_Initialize and ADP_Open must have been called before.

  Parameters:
    attributeId    - The identifier of the ADP IB attribute to read

    attributeIndex - The index within the table of the specified IB attribute to
                     read

  Returns:
    None.

  Example:
    <code>
    ADP_GetRequest(ADP_IB_SOFT_VERSION, 0);
    </code>

  Remarks:
    None.
*/
void ADP_GetRequest(uint32_t attributeId, uint16_t attributeIndex);

// *****************************************************************************
/* Function:
    void ADP_GetRequestSync(uint32_t u32AttributeId, uint16_t u16AttributeIndex,
        ADP_GET_CFM_PARAMS* pGetConfirm);

  Summary:
    This primitive gets the value of an attribute from the ADP information base
    synchronously.

  Description:
    The ADP Get Request primitive allows the upper layer to get the value of an
    attribute from the ADP information base synchronously.

  Precondition:
    ADP_Initialize and ADP_Open must have been called before.

  Parameters:
    attributeId    - The identifier of the ADP IB attribute to read

    attributeIndex - The index within the table of the specified IB attribute to
                     read

    pGetConfirm    - Pointer to Get Confirm parameters

  Returns:
    None.

  Example:
    <code>
    ADP_GET_CFM_PARAMS getConfirm;

    ADP_GetRequestSync(ADP_IB_SOFT_VERSION, 0, &getConfirm);

    if (getConfirm->status == G3_SUCCESS)
    {

    }
    </code>

  Remarks:
    None.
*/
void ADP_GetRequestSync(uint32_t attributeId, uint16_t attributeIndex,
    ADP_GET_CFM_PARAMS* pGetConfirm);

// *****************************************************************************
/* Function:
    void ADP_MacGetRequest(uint32_t attributeId, uint16_t attributeIndex);

  Summary:
    This primitive gets the value of an attribute from the MAC information base.

  Description:
    The ADP MAC Get Request primitive allows the upper layer to get the value of
    an attribute from the MAC information base. The upper layer cannot access
    directly the MAC layer while ADP is running.

    Result is provided in the corresponding ADP MAC Get Confirm callback.

  Precondition:
    ADP_Initialize and ADP_Open must have been called before.

  Parameters:
    attributeId    - The identifier of the MAC IB attribute to read

    attributeIndex - The index within the table of the specified IB attribute to
                     read

  Returns:
    None.

  Example:
    <code>
    ADP_MacGetRequest(MAC_WRP_PIB_SHORT_ADDRESS, 0);
    </code>

  Remarks:
    None.
*/
void ADP_MacGetRequest(uint32_t attributeId, uint16_t attributeIndex);

// *****************************************************************************
/* Function:
    void ADP_MacGetRequestSync(uint32_t u32AttributeId, uint16_t u16AttributeIndex,
        ADP_MAC_GET_CFM_PARAMS* pGetConfirm);

  Summary:
    This primitive gets the value of an attribute from the MAC information base
    synchronously.

  Description:
    The ADP MAC Get Request primitive allows the upper layer to get the value of an
    attribute from the MAC information base synchronously.

  Precondition:
    ADP_Initialize and ADP_Open must have been called before.

  Parameters:
    attributeId    - The identifier of the MAC IB attribute to read

    attributeIndex - The index within the table of the specified IB attribute to
                     read

    pGetConfirm    - Pointer to Get Confirm parameters

  Returns:
    None.

  Example:
    <code>
    ADP_MAC_GET_CFM_PARAMS getConfirm;

    ADP_MacGetRequestSync(MAC_WRP_PIB_SHORT_ADDRESS, 0, &getConfirm);

    if (getConfirm->status == G3_SUCCESS)
    {

    }
    </code>

  Remarks:
    None.
*/
void ADP_MacGetRequestSync(uint32_t u32AttributeId, uint16_t u16AttributeIndex,
    ADP_MAC_GET_CFM_PARAMS* pGetConfirm);

// *****************************************************************************
/* Function:
    void ADP_SetRequest(uint32_t attributeId, uint16_t attributeIndex,
        uint8_t attributeLength, const uint8_t *pAttributeValue);

  Summary:
    This primitive set the value of an attribute in the ADP information base.

  Description:
    The ADP Set Request primitive allows the upper layer to set the value of an
    attribute in the ADP information base.

    Result is provided in the corresponding ADP Set Confirm callback.

  Precondition:
    ADP_Initialize and ADP_Open must have been called before.

  Parameters:
    attributeId     - The identifier of the ADP IB attribute to set

    attributeIndex  - The index within the table of the specified IB attribute
                      to write

    attributeLength - The length of the value of the attribute to set

    pAttributeValue - Pointer to the value of the attribute to set

  Returns:
    None.

  Example:
    <code>
    uint8_t pAttributeValue[64];

    pAttributeValue[0] = 44;
    ADP_SetRequest(ADP_IB_LOW_LQI_VALUE, 0, 1, pAttributeValue);
    </code>

  Remarks:
    None.
*/
void ADP_SetRequest(uint32_t attributeId, uint16_t attributeIndex,
    uint8_t attributeLength, const uint8_t *pAttributeValue);

// *****************************************************************************
/* Function:
    void ADP_SetRequestSync(uint32_t attributeId, uint16_t attributeIndex,
        uint8_t attributeLength, const uint8_t *pAttributeValue,
        ADP_SET_CFM_PARAMS* pSetConfirm);

  Summary:
    This primitive set the value of an attribute in the ADP information base
    synchronously.

  Description:
    The ADP Set Request primitive allows the upper layer to set the value of an
    attribute in the ADP information base synchronously.

  Precondition:
    ADP_Initialize and ADP_Open must have been called before.

  Parameters:
    attributeId     - The identifier of the ADP IB attribute to set

    attributeIndex  - The index within the table of the specified IB attribute
                      to write

    attributeLength - The length of the value of the attribute to set

    pAttributeValue - Pointer to the value of the attribute to set

    pSetConfirm     - Pointer to Set Confirm parameters

  Returns:
    None.

  Example:
    <code>
    uint8_t pAttributeValue[64];
    ADP_SET_CFM_PARAMS setConfirm;

    pAttributeValue[0] = 44;
    ADP_SetRequestSync(ADP_IB_LOW_LQI_VALUE, 0, 1, pAttributeValue,
        &setConfirm);

    if (setConfirm->status == G3_SUCCESS)
    {

    }
    </code>

  Remarks:
    None.
*/
void ADP_SetRequestSync(uint32_t attributeId, uint16_t attributeIndex,
    uint8_t attributeLength, const uint8_t *pAttributeValue,
    ADP_SET_CFM_PARAMS* pSetConfirm);

// *****************************************************************************
/* Function:
    void ADP_MacSetRequest(uint32_t attributeId, uint16_t attributeIndex,
        uint8_t attributeLength, const uint8_t *pAttributeValue);

  Summary:
    This primitive set the value of an attribute in the MAC information base.

  Description:
    The ADP MAC Set Request primitive allows the upper layer to set the value of an
    attribute in the MAC information base. The upper layer cannot access
    directly the MAC layer while ADP is running.

    Result is provided in the corresponding ADP MAC Set Confirm callback.

  Precondition:
    ADP_Initialize and ADP_Open must have been called before.

  Parameters:
    attributeId     - The identifier of the MAC IB attribute to set

    attributeIndex  - The index within the table of the specified IB attribute
                      to write

    attributeLength - The length of the value of the attribute to set

    pAttributeValue - Pointer to the value of the attribute to set

  Returns:
    None.

  Example:
    <code>
    uint8_t pAttributeValue[144];

    pAttributeValue[0] = 5;
    ADP_MacSetRequest(MAC_WRP_PIB_MAX_BE, 0, 1, pAttributeValue);
    </code>

  Remarks:
    None.
*/
void ADP_MacSetRequest(uint32_t attributeId, uint16_t attributeIndex,
    uint8_t attributeLength, const uint8_t *pAttributeValue);

// *****************************************************************************
/* Function:
    void ADP_MacSetRequestSync(uint32_t attributeId, uint16_t attributeIndex,
        uint8_t attributeLength, const uint8_t *pAttributeValue,
        ADP_SET_CFM_PARAMS* pSetConfirm);

  Summary:
    This primitive set the value of an attribute in the MAC information base
    synchronously.

  Description:
    The ADP MAC Set Request primitive allows the upper layer to set the value of an
    attribute in the MAC information base synchronously.

  Precondition:
    ADP_Initialize and ADP_Open must have been called before.

  Parameters:
    attributeId     - The identifier of the MAC IB attribute to set

    attributeIndex  - The index within the table of the specified IB attribute
                      to write

    attributeLength - The length of the value of the attribute to set

    pAttributeValue - Pointer to the value of the attribute to set

    pSetConfirm     - Pointer to Set Confirm parameters

  Returns:
    None.

  Example:
    <code>
    uint8_t pAttributeValue[144];
    ADP_SET_CFM_PARAMS setConfirm;

    pAttributeValue[0] = 5;
    ADP_MacSetRequestSync(MAC_WRP_PIB_MAX_BE, 0, 1, pAttributeValue,
        &setConfirm);

    if (setConfirm->status == G3_SUCCESS)
    {

    }
    </code>

  Remarks:
    None.
*/
void ADP_MacSetRequestSync(uint32_t attributeId, uint16_t attributeIndex,
    uint8_t attributeLength, const uint8_t *pAttributeValue,
    ADP_SET_CFM_PARAMS* pSetConfirm);

// *****************************************************************************
/* Function:
    void ADP_RouteDiscoveryRequest(uint16_t dstAddr, uint8_t maxHops);

  Summary:
    This primitive allows the upper layer to initiate a route discovery.

  Description:
    The ADP Route Discovery Request primitive allows the upper layer to initiate
    a route discovery.

    Result is provided in the corresponding ADP Route Discovery Confirm callback.

  Precondition:
    ADP_Initialize and ADP_Open must have been called before.

  Parameters:
    dstAddr - The short unicast destination address of the route discovery

    maxHops - This parameter indicates the maximum number of hops allowed for
              the route discovery (Range: 0x01 - 0x0E)

  Returns:
    None.

  Example:
    <code>
    ADP_RouteDiscoveryRequest(0x0001, 8);
    </code>

  Remarks:
    None.
*/
void ADP_RouteDiscoveryRequest(uint16_t dstAddr, uint8_t maxHops);

// *****************************************************************************
/* Function:
    void ADP_PathDiscoveryRequest(uint16_t dstAddr, uint8_t metricType);

  Summary:
    This primitive allows the upper layer to initiate a path discovery.

  Description:
    The ADP Path Discovery Request primitive allows the upper layer to initiate
    a path discovery.

    Result is provided in the corresponding ADP Path Discovery Confirm callback.

  Precondition:
    ADP_Initialize and ADP_Open must have been called before.

  Parameters:
    dstAddr    - The short unicast destination address of the path discovery

    metricType - The metric type to be used for the path discovery
                 (0x00, 0x0E, 0x0F)

  Returns:
    None.

  Example:
    <code>
    ADP_PathDiscoveryRequest(0x0001, 0x0F);
    </code>

  Remarks:
    None.
*/
void ADP_PathDiscoveryRequest(uint16_t dstAddr, uint8_t metricType);

// *****************************************************************************
/* Function:
  void ADP_LbpRequest(const ADP_ADDRESS *pDstAddr, uint16_t nsduLength,
        uint8_t *pNsdu, uintptr_t nsduHandle, uint8_t maxHops,
        bool discoveryRoute, uint8_t qualityOfService, bool securityEnable);

  Summary:
    This primitive allows the upper layer of the client to send an LBP message.

  Description:
    The ADP LBP Request primitive allows the upper layer of the client to send
    the LBP message to the server modem.

    Result is provided in the corresponding ADP LBP Confirm callback.

  Precondition:
    ADP_Initialize and ADP_Open must have been called before.

  Parameters:
    pDstAddr         - 16-bit address of LBA or LBS, or 64 bit address (extended
                       address of LBD)

    nsduLength       - The size of the NSDU, in bytes; limited to
                       ADP_LBP_MAX_NSDU_LENGTH

    pNsdu            - Pointer to NSDU to send

    nsduHandle       - The handle of the NSDU to transmit. This parameter is
                       used to identify in the ADP LBP Confirm primitive which
                       request is concerned. It can be randomly chosen by the
                       upper layer.

    maxHops          - The number of hops for the LBP frame

    discoverRoute    - If true, a route discovery procedure will be performed
                       prior to sending the frame if a route to the destination
                       is not available in the routing table.
                       If false, no route discovery is performed.

    qualityOfService - The requested quality of service (QoS) of the frame to
                       send. Allowed values are 0 (normal priority) and 1 (high
                       priority).

    bSecurityEnable  - If true, this parameter enables the MAC layer security
                       for sending the frame.

  Returns:
    None.

  Example:
    <code>
    uint8_t nsdu[ADP_LBP_MAX_NSDU_LENGTH];
    uint16_t nsduLength;
    ADP_ADDRESS destAddress;

    destAddress.addrSize = ADP_ADDRESS_16BITS;
    destAddress.shortAddr = 0x0001;

    ADP_LbpRequest(&destAddress, nsduLength, nsdu, 0, 8, true, 0, true);
    </code>

  Remarks:
    None.
*/
void ADP_LbpRequest(const ADP_ADDRESS *pDstAddr, uint16_t nsduLength,
    uint8_t *pNsdu, uintptr_t nsduHandle, uint8_t maxHops,
    bool discoveryRoute, uint8_t qualityOfService, bool securityEnable);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // #ifndef ADP_H
