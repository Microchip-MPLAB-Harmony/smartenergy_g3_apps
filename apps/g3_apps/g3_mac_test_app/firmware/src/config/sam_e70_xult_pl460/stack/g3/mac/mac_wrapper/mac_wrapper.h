/*******************************************************************************
  G3 MAC Wrapper Header File

  Company:
    Microchip Technology Inc.

  File Name:
    mac_wrapper.h

  Summary:
    G3 MAC Wrapper API Header File

  Description:
    This file contains definitions of the primitives and related types
    to be used by upper layers when accessing G3 MAC layers.
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

#ifndef _MAC_WRAPPER_H
#define _MAC_WRAPPER_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include "mac_wrapper_defs.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* MAC Wrapper Data Request Parameters

   Summary:
    Defines the Parameters for the MAC Data Request primitive.

   Description:
    The structure contains the fields used by the MAC Data Request primitive.

   Remarks:
    None.
*/
typedef struct
{
    /* Source address mode 0, 16, 64 bits */
    MAC_WRP_ADDRESS_MODE srcAddressMode;
    /* The PAN identifier of the entity to which the MSDU is being transferred */
    MAC_WRP_PAN_ID destPanId;
    /* The device address of the entity to which the MSDU is being transferred */
    MAC_WRP_ADDRESS destAddress;
    /* The number of octets contained in the MSDU to be transmitted */
    uint16_t msduLength;
    /* Pointer to the set of octets forming the MSDU to be transmitted */
    const uint8_t *msdu;
    /* The handle associated with the MSDU to be transmitted */
    uint8_t msduHandle;
    /* Transmission options for this MSDU: 0 unacknowledged, 1 acknowledged */
    uint8_t txOptions;
    /* The security level to be used: 0x00 unecrypted, 0x05 encrypted */
    MAC_WRP_SECURITY_LEVEL securityLevel;
    /* The index of the encryption key to be used */
    uint8_t keyIndex;
    /* The QOS of the MSDU: 0x00 normal priority, 0x01 high priority */
    MAC_WRP_QUALITY_OF_SERVICE qualityOfService;
    /* The Media Type to use on Request (only used if both PLC and RF MACs are present) */
    MAC_WRP_MEDIA_TYPE_REQUEST mediaType;
} MAC_WRP_DATA_REQUEST_PARAMS;

// *****************************************************************************
/* MAC Wrapper Data Confirm Parameters

   Summary:
    Defines the Parameters for the MAC Data Confirm event handler function.

   Description:
    The structure contains the fields reported by the MAC Data Confirm
    event handler function.

   Remarks:
    None.
*/
typedef struct
{
    /* Transmission time, refered to MAC milliseconds counter */
    MAC_WRP_TIMESTAMP timestamp;
    /* The handle associated with the MSDU being confirmed */
    uint8_t msduHandle;
    /* The result of the last MSDU transmission */
    MAC_WRP_STATUS status;
    /* The Confirm result Media Type */
    MAC_WRP_MEDIA_TYPE_CONFIRM mediaType;
} MAC_WRP_DATA_CONFIRM_PARAMS;

// *****************************************************************************
/* MAC Wrapper Data Indication Parameters

   Summary:
    Defines the Parameters for the MAC Data Indication event handler function.

   Description:
    The structure contains the fields reported by the MAC Data Indication
    event handler function.

   Remarks:
    None.
*/
typedef struct
{
    /* The PAN identifier of the device from which the frame was received */
    MAC_WRP_PAN_ID srcPanId;
    /* The address of the device which sent the frame */
    MAC_WRP_ADDRESS srcAddress;
    /* The PAN identifier of the entity to which the MSDU is being transferred */
    MAC_WRP_PAN_ID destPanId;
    /* The address of the entity to which the MSDU is being transferred */
    MAC_WRP_ADDRESS destAddress;
    /* The number of octets of the MSDU to be indicated to the upper layer */
    uint16_t msduLength;
    /* Pointer to the set of octets forming the received MSDU */
    uint8_t *msdu;
    /* The LQI value measured during reception of the frame */
    uint8_t linkQuality;
    /* The Data Sequence Number of the received frame */
    uint8_t dsn;
    /* Reception time, refered to MAC milliseconds counter */
    MAC_WRP_TIMESTAMP timestamp;
    /* Security level of the received frame: 0x00 unecrypted, 0x05 encrypted */
    MAC_WRP_SECURITY_LEVEL securityLevel;
    /* The index of the key used for decryption */
    uint8_t keyIndex;
    /* The QOS of the MSDU: 0x00 normal priority, 0x01 high priority */
    MAC_WRP_QUALITY_OF_SERVICE qualityOfService;
    /* Modulation Type of the received frame */
    uint8_t rxModulation;
    /* Modulation scheme of the received frame */
    uint8_t rxModulationScheme;
    /* Tone Map of the received frame */
    MAC_WRP_TONE_MAP rxToneMap;
    /* Weakest Modulation Type in which the frame could have been received */
    uint8_t computedModulation;
    /* Weakest Modulation Scheme in which the frame could have been received */
    uint8_t computedModulationScheme;
    /* Weakest Tone Map with which the frame could have been received */
    MAC_WRP_TONE_MAP computedToneMap;
    /* Phase Differential compared to Node that sent the frame */
    uint8_t phaseDifferential;
    /* The Data Indication Media Type */
    MAC_WRP_MEDIA_TYPE_INDICATION mediaType;
} MAC_WRP_DATA_INDICATION_PARAMS;

// *****************************************************************************
/* MAC Wrapper Sniffer Indication Parameters

   Summary:
    Defines the Parameters for the MAC Sniffer Indication event handler function.

   Description:
    The structure contains the fields reported by the MAC Sniffer Indication
    event handler function.

   Remarks:
    None.
*/
typedef struct
{
    /* Frame Type as defined in IEEE 802.15.4 standard */
    uint8_t frameType;
    /* The PAN identifier of the device from which the frame was received */
    MAC_WRP_PAN_ID srcPanId;
    /* The address of the device which sent the frame */
    MAC_WRP_ADDRESS srcAddress;
    /* The PAN identifier of the entity to which the MSDU is being transferred */
    MAC_WRP_PAN_ID destPanId;
    /* The address of the entity to which the MSDU is being transferred */
    MAC_WRP_ADDRESS destAddress;
    /* The number of octets of the MSDU to be indicated to the upper layer */
    uint16_t msduLength;
    /* Pointer to the set of octets forming the received MSDU */
    uint8_t *msdu;
    /* The LQI value measured during reception of the frame */
    uint8_t linkQuality;
    /* The Data Sequence Number of the received frame */
    uint8_t dsn;
    /* Reception time, refered to MAC milliseconds counter */
    MAC_WRP_TIMESTAMP timestamp;
    /* Security level of the received frame: 0x00 unecrypted, 0x05 encrypted */
    MAC_WRP_SECURITY_LEVEL securityLevel;
    /* The index of the key used for decryption */
    uint8_t keyIndex;
    /* The QOS of the MSDU: 0x00 normal priority, 0x01 high priority */
    MAC_WRP_QUALITY_OF_SERVICE qualityOfService;
    /* Modulation Type of the received frame */
    uint8_t rxModulation;
    /* Modulation scheme of the received frame */
    uint8_t rxModulationScheme;
    /* Tone Map of the received frame */
    MAC_WRP_TONE_MAP rxToneMap;
    /* Weakest Modulation Type in which the frame could have been received */
    uint8_t computedModulation;
    /* Weakest Modulation Scheme in which the frame could have been received */
    uint8_t computedModulationScheme;
    /* Weakest Tone Map with which the frame could have been received */
    MAC_WRP_TONE_MAP computedToneMap;
    /* Phase Differential compared to Node that sent the frame */
    uint8_t phaseDifferential;
} MAC_WRP_SNIFFER_INDICATION_PARAMS;

// *****************************************************************************
/* MAC Wrapper Beacon Notify Indication Parameters

   Summary:
    Defines the Parameters for the MAC Beacon Notify Indication event handler
    function.

   Description:
    The structure contains the fields reported by the MAC Beacon Notify Indication
    event handler function.

   Remarks:
    None.
*/
typedef struct
{
    /* PAN Descriptor. Information of the Beacon frames seen on a G3 network */
    MAC_WRP_PAN_DESCRIPTOR panDescriptor;
} MAC_WRP_BEACON_NOTIFY_INDICATION_PARAMS;

// *****************************************************************************
/* MAC Wrapper Reset Request Parameters

   Summary:
    Defines the Parameters for the MAC Reset Request primitive.

   Description:
    The structure contains the fields expected by the MAC Reset Request
    primitive.

   Remarks:
    None.
*/
typedef struct
{
    /* True to reset the PIB to the default values, false otherwise */
    bool setDefaultPib;
} MAC_WRP_RESET_REQUEST_PARAMS;

// *****************************************************************************
/* MAC Wrapper Reset Confirm Parameters

   Summary:
    Defines the Parameters for the MAC Reset Confirm event handler function.

   Description:
    The structure contains the fields reported by the MAC Reset Confirm
    event handler function.

   Remarks:
    None.
*/
typedef struct
{
    /* Result of the Reset Request */
    MAC_WRP_STATUS status;
} MAC_WRP_RESET_CONFIRM_PARAMS;

// *****************************************************************************
/* MAC Wrapper Scan Request Parameters

   Summary:
    Defines the Parameters for the MAC Scan Request primitive.

   Description:
    The structure contains the fields expected by the MAC Scan Request
    primitive.

   Remarks:
    None.
*/
typedef struct
{
    /* Duration of the scan in seconds */
    uint16_t scanDuration;
} MAC_WRP_SCAN_REQUEST_PARAMS;

// *****************************************************************************
/* MAC Wrapper Scan Confirm Parameters

   Summary:
    Defines the Parameters for the MAC Scan Confirm event handler function.

   Description:
    The structure contains the fields reported by the MAC Scan Confirm
    event handler function.

   Remarks:
    None.
*/
typedef struct
{
    /* Result of the Scan Request */
    MAC_WRP_STATUS status;
} MAC_WRP_SCAN_CONFIRM_PARAMS;

// *****************************************************************************
/* MAC Wrapper Comm Status Indication Parameters

   Summary:
    Defines the Parameters for the MAC Comm Status Indication event handler
    function.

   Description:
    The structure contains the fields reported by the MAC Comm Status Indication
    event handler function. This function reports network events such as other
    PAN detection or security errors, as defined in IEEE 802.15.4 standard.

   Remarks:
    None.
*/
typedef struct
{
    /* The PAN identifier from which the frame/event was received */
    MAC_WRP_PAN_ID panId;
    /* The address of the device which sent the frame */
    MAC_WRP_ADDRESS srcAddress;
    /* The address of the device intended to receive the frame */
    MAC_WRP_ADDRESS destAddress;
    /* Type of event detected */
    MAC_WRP_STATUS status;
    /* Security level of the received frame: 0x00 unecrypted, 0x05 encrypted */
    MAC_WRP_SECURITY_LEVEL securityLevel;
    /* The index of the key used for decryption */
    uint8_t keyIndex;
    /* The Comm Status Indication Media Type */
    MAC_WRP_MEDIA_TYPE_INDICATION mediaType;
} MAC_WRP_COMM_STATUS_INDICATION_PARAMS;

// *****************************************************************************
/* MAC Wrapper Network Start Request Parameters

   Summary:
    Defines the Parameters for the MAC Network Start Request primitive.

   Description:
    The structure contains the fields expected by the MAC Network Start Request
    primitive. This primitive is invoked by a PAN Coordinator to establish a
    new PAN to which devices can join to build a G3 Network.

   Remarks:
    None.
*/
typedef struct
{
    /* The PAN Identifier for the started Network */
    MAC_WRP_PAN_ID panId;
} MAC_WRP_START_REQUEST_PARAMS;

// *****************************************************************************
/* MAC Wrapper Start Confirm Parameters

   Summary:
    Defines the Parameters for the MAC Start Confirm event handler function.

   Description:
    The structure contains the fields reported by the MAC Start Confirm
    event handler function.

   Remarks:
    None.
*/
typedef struct
{
    /* Result of the Start Request */
    MAC_WRP_STATUS status;
} MAC_WRP_START_CONFIRM_PARAMS;

// *****************************************************************************
/* MAC Wrapper State Machine Definition

  Summary:
    Defines the possible states of the MAC Wrapper State Machine.

  Description:
    None.

  Remarks:
    None.
*/
typedef enum
{
    MAC_WRP_STATE_NOT_READY,
    MAC_WRP_STATE_IDLE,
    MAC_WRP_STATE_TX,
    MAC_WRP_STATE_WAITING_TX_CFM,
    MAC_WRP_STATE_ERROR,
}MAC_WRP_STATE;

// *****************************************************************************
/* MAC Wrapper Data Confirm Event Handler Function Pointer

  Summary:
    Pointer to a Data Confirm Event handler function.

  Description:
    This data type defines the required function signature for the MAC Wrapper
    Data Confirm event handling callback function. A client must register a
    pointer using the event handling function whose function signature
    (parameter and return value types) matches the types specified by this
    function pointer in order to receive Data Confirm events back from module.

  Parameters:
    dcParams - Pointer to structure containing parameters related to Confirm

  Example:
    <code>
    App_DataConfirm(MAC_WRP_DATA_CONFIRM_PARAMS *params)
    {
        // Check result
        if (params->status == MAC_WRP_STATUS_SUCCESS)
        {
            txHandler = params->msduHandle;
            // Dispatch according to handler
        }
    }
    </code>

  Remarks:
    None.
*/
typedef void (*MAC_WRP_DataConfirm)(MAC_WRP_DATA_CONFIRM_PARAMS *dcParams);

// *****************************************************************************
/* MAC Wrapper Data Indication Event Handler Function Pointer

  Summary:
    Pointer to a Data Indication Event handler function.

  Description:
    This data type defines the required function signature for the MAC Wrapper
    Data Indication event handling callback function. A client must register a
    pointer using the event handling function whose function signature
    (parameter and return value types) matches the types specified by this
    function pointer in order to receive Data Indication events back from module.

  Parameters:
    diParams - Pointer to structure containing parameters related to Indication

  Example:
    <code>
    App_DataIndication(MAC_WRP_DATA_INDICATION_PARAMS *params)
    {
        // Check addressing
        if (params->destPanId == myPanId)
        {
            if (params->destAddress.addressMode == MAC_WRP_ADDRESS_MODE_SHORT)
            {
                if (params->destAddress.shortAddress == myShortAddress) {
                    // Frame is for me
                }
            }
            else if (params->destAddress.addressMode == MAC_WRP_ADDRESS_MODE_EXTENDED)
            {
                if (params->destAddress.extendedAddress == myExtendedAddress) {
                    // Frame is for me
                }
            }
        }
    }
    </code>

  Remarks:
    None.
*/
typedef void (*MAC_WRP_DataIndication)(MAC_WRP_DATA_INDICATION_PARAMS *diParams);

// *****************************************************************************
/* MAC Wrapper Sniffer Indication Event Handler Function Pointer

  Summary:
    Pointer to a Sniffer Indication Event handler function.

  Description:
    This data type defines the required function signature for the MAC Wrapper
    Sniffer Indication event handling callback function. A client must register a
    pointer using the event handling function whose function signature
    (parameter and return value types) matches the types specified by this
    function pointer in order to receive Sniffer Indication events back from module.

  Parameters:
    siParams - Pointer to structure containing parameters related to Indication

  Example:
    <code>
    App_SnifferIndication(MAC_WRP_SNIFFER_INDICATION_PARAMS *params)
    {
        // Send frame to a sniffing handling module
        App_SnifferHandlingTool(params);
    }
    </code>

  Remarks:
    None.
*/
typedef void (*MAC_WRP_SnifferIndication)(MAC_WRP_SNIFFER_INDICATION_PARAMS *siParams);

// *****************************************************************************
/* MAC Wrapper Reset Confirm Event Handler Function Pointer

  Summary:
    Pointer to a Reset Confirm Event handler function.

  Description:
    This data type defines the required function signature for the MAC Wrapper
    Reset Confirm event handling callback function. A client must register a
    pointer using the event handling function whose function signature
    (parameter and return value types) matches the types specified by this
    function pointer in order to receive Reset Confirm events back from module.

  Parameters:
    rcParams - Pointer to structure containing parameters related to Confirm

  Example:
    <code>
    App_ResetConfirm(MAC_WRP_RESET_CONFIRM_PARAMS *params)
    {
        // Check result
        if (params->status != MAC_WRP_STATUS_SUCCESS)
        {
            // Handle Reset error
        }
        else
        {
            // Reset performed correctly
            // Reinitialize MAC Wrapper module
        }
    }
    </code>

  Remarks:
    None.
*/
typedef void (*MAC_WRP_ResetConfirm)(MAC_WRP_RESET_CONFIRM_PARAMS *rcParams);

// *****************************************************************************
/* MAC Wrapper Beacon Notify Indication Event Handler Function Pointer

  Summary:
    Pointer to a Beacon Notify Indication Event handler function.

  Description:
    This data type defines the required function signature for the MAC Wrapper
    Beacon Notify Indication event handling callback function.
    A client must register a pointer using the event handling function
    whose function signature (parameter and return value types) matches
    the types specified by this function pointer in order to receive Beacon
    Notify Indication events back from module.

    Beacon Notify Indication events are reported every time a Beacon frame
    is received by MAC layer during a Network Scan period.

  Parameters:
    bcnParams - Pointer to structure containing parameters related to Indication

  Example:
    <code>
    MAC_WRP_PAN_DESCRIPTOR lowestRCDescriptor;
    lowestRCDescriptor.rcCoord = 0xFFFF;

    App_BeaconIndication(MAC_WRP_BEACON_NOTIFY_INDICATION_PARAMS *params)
    {
        // Keep the Beacon with lowest Route Cost to Coordinator
        // (criteria chosen for illustration purposes)
        if (params->panDescriptor.rcCoord < lowestRCDescriptor.rcCoord)
        {
            lowestRCDescriptor = params->panDescriptor;
        }
    }
    </code>

  Remarks:
    None.
*/
typedef void (*MAC_WRP_BeaconNotifyIndication)(MAC_WRP_BEACON_NOTIFY_INDICATION_PARAMS *bcnParams);

// *****************************************************************************
/* MAC Wrapper Scan Confirm Event Handler Function Pointer

  Summary:
    Pointer to a Scan Confirm Event handler function.

  Description:
    This data type defines the required function signature for the MAC Wrapper
    Scan Confirm event handling callback function. A client must register a
    pointer using the event handling function whose function signature
    (parameter and return value types) matches the types specified by this
    function pointer in order to receive Scan Confirm events back from module.

  Parameters:
    scParams - Pointer to structure containing parameters related to Confirm

  Example:
    <code>
    App_ScanConfirm(MAC_WRP_SCAN_CONFIRM_PARAMS *params)
    {
        // Check result
        if (params->status != MAC_WRP_STATUS_SUCCESS)
        {
            // Handle Scan error
        }
        else
        {
            // Scan performed correctly
            // Check received Beacons
        }
    }
    </code>

  Remarks:
    None.
*/
typedef void (*MAC_WRP_ScanConfirm)(MAC_WRP_SCAN_CONFIRM_PARAMS *scParams);

// *****************************************************************************
/* MAC Wrapper Start Confirm Event Handler Function Pointer

  Summary:
    Pointer to a Start Confirm Event handler function.

  Description:
    This data type defines the required function signature for the MAC Wrapper
    Start Confirm event handling callback function. A client must register a
    pointer using the event handling function whose function signature
    (parameter and return value types) matches the types specified by this
    function pointer in order to receive Start Confirm events back from module.

  Parameters:
    scParams - Pointer to structure containing parameters related to Confirm

  Example:
    <code>
    App_StartConfirm(MAC_WRP_START_CONFIRM_PARAMS *params)
    {
        // Check result
        if (params->status != MAC_WRP_STATUS_SUCCESS)
        {
            // Handle Start error
        }
        else
        {
            // Start performed correctly. PAN Coordinator ready.
        }
    }
    </code>

  Remarks:
    None.
*/
typedef void (*MAC_WRP_StartConfirm)(MAC_WRP_START_CONFIRM_PARAMS *scParams);

// *****************************************************************************
/* MAC Wrapper Comm Status Indication Event Handler Function Pointer

  Summary:
    Pointer to a Comm Status Indication Event handler function.

  Description:
    This data type defines the required function signature for the MAC Wrapper
    Comm Status Indication event handling callback function. A client must
    register a pointer using the event handling function whose function
    signature (parameter and return value types) matches the types specified
    by this function pointer in order to receive Comm Status Indication events
    back from module.

    Comm Status events are specified in IEEE 802.15.4 standard.
    G3 implements a subset of them, as specified in G3-PLC standard.

  Parameters:
    csParams - Pointer to structure containing parameters related to Indication

  Example:
    <code>
    App_CommStatusIndication(MAC_WRP_COMM_STATUS_INDICATION_PARAMS *params)
    {
        // Send event to a handling module
        if (csParams->status == MAC_WRP_STATUS_ALTERNATE_PANID_DETECTION)
        {
            // Frame from other PAN Id received
            App_AlternateIDHandlingTool(params);
        }
        else
        {
            // Security related issue
            App_SecurityIssueHandlingTool(params);
        }
    }
    </code>

  Remarks:
    None.
*/
typedef void (*MAC_WRP_CommStatusIndication)(MAC_WRP_COMM_STATUS_INDICATION_PARAMS *csParams);


// *****************************************************************************
// *****************************************************************************
// Section: MAC Wrapper Interface Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ MAC_WRP_Initialize
    (
      const SYS_MODULE_INDEX index,
      const SYS_MODULE_INIT * const init
    )

  Summary:
    Initializes the MAC Wrapper module for the specified Index.

  Description:
    This routine initializes the MAC Wrappermodule making it ready for clients
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
    // The following code snippet shows an example MAC Wrapper initialization.

    SYS_MODULE_INIT initData;
    SYS_MODULE_OBJ sysObjMacWrp;

    // Populate initData if needed

    sysObjMacWrp = MAC_WRP_Initialize(G3_MAC_WRP_INDEX_0, &initData);
    if (sysObjMacWrp == SYS_MODULE_OBJ_INVALID)
    {
        // Handle error
    }
    </code>

  Remarks:
    This routine must be called before any other MAC Wrapper routine is called.
*/
SYS_MODULE_OBJ MAC_WRP_Initialize(const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init);

// *****************************************************************************
/* Function:
    MAC_WRP_HANDLE MAC_WRP_Open
    (
      SYS_MODULE_INDEX index
    )

  Summary:
    Opens the specified MAC Wrapper instance and returns a handle to it.

  Description:
    This routine opens the specified MAC Wrapper instance and provides a
    handle that must be provided to all other client-level operations to
    identify the caller and the instance of the module.

  Precondition:
    MAC_WRP_Initialize routine must have been called before.

  Parameters:
    index - Index for the instance to be opened

  Returns:
    If successful, the routine returns a valid open-instance handle (a number
    identifying both the caller and the module instance).
    If an error occurs, the return value is MAC_WRP_HANDLE_INVALID.

  Example:
    <code>
    SYS_MODULE_INIT initData;
    MAC_WRP_HANDLE handle;
    SYS_MODULE_OBJ sysObjMacWrp;
    
    // Populate initData if needed

    sysObjMacWrp = MAC_WRP_Initialize(G3_MAC_WRP_INDEX_0, &initData);

    handle = MAC_WRP_Open(G3_MAC_WRP_INDEX_0);
    if (handle == MAC_WRP_HANDLE_INVALID)
    {
        // Handle error
    }
    </code>

  Remarks:
    None.
*/
MAC_WRP_HANDLE MAC_WRP_Open(SYS_MODULE_INDEX index);

// *****************************************************************************
/* Function:
    void MAC_WRP_Tasks
    (
      SYS_MODULE_OBJ object
    )

  Summary:
    Maintains MAC Wrapper State Machine.

  Description:
    MAC Wrapper does not have a State Machine to maintain, but is in charge
    of calling "_Tasks" routines of the MAC layers below.

  Precondition:
    MAC_WRP_Initialize routine must have been called before,
    and its returned Object used when calling this function.

  Parameters:
    object - Identifier for the object instance

  Returns:
    None.

  Example:
    <code>
    // ...
    SYS_MODULE_OBJ sysObjMacWrp;
    sysObjMacWrp = MAC_WRP_Initialize(G3_MAC_WRP_INDEX_0, &initData);
    // ...

    while (true)
    {
        MAC_WRP_Tasks(sysObjMacWrp);
    
        // Do other tasks
    }
    </code>

  Remarks:
    None.
*/
void MAC_WRP_Tasks(SYS_MODULE_OBJ object);

// *****************************************************************************
/* Function:
    void MAC_WRP_DataRequest
    (
      MAC_WRP_HANDLE handle,
      MAC_WRP_DATA_REQUEST_PARAMS *drParams
    )

  Summary:
    The MAC_WRP_DataRequest primitive requests the transfer of a PDU
    to another device or multiple devices.

  Description:
    DataRequest primitive is used to transfer data to other nodes in the G3
    Network. Result is provided in the corresponding Confirm callback.

  Precondition:
    A valid MAC Wrapper Handle has to be obtained before.

  Parameters:
    handle - A valid handle which identifies the Mac Wrapper instance

    drParams - Pointer to structure containing required parameters for request

  Returns:
    None.

  Example:
    <code>
    // ...
    MAC_WRP_HANDLE handle;
    handle = MAC_WRP_Open(G3_MAC_WRP_INDEX_0);
    // ...

    MAC_WRP_DATA_REQUEST_PARAMS params = {
        .srcAddressMode = MAC_WRP_ADDRESS_MODE_SHORT,
        .destPanId = 0x1234,
        .destAddress = 0x0002,
        .msduLength = 20,
        .msdu = &txBuffer[0],
        .msduHandle = appHandle++,
        .txOptions = MAC_WRP_TX_OPTION_ACK,
        .securityLevel = MAC_WRP_SECURITY_LEVEL_ENC_MIC_32,
        .keyIndex = 0,
        .qualityOfService = MAC_WRP_QUALITY_OF_SERVICE_NORMAL_PRIORITY,
    };

    MAC_WRP_DataRequest(handle, &params);
    // Wait for Data Confirm
    </code>

  Remarks:
    None.
*/
void MAC_WRP_DataRequest(MAC_WRP_HANDLE handle, MAC_WRP_DATA_REQUEST_PARAMS *drParams);

// *****************************************************************************
/* Function:
    MAC_WRP_STATUS MAC_WRP_GetRequestSync
    (
      MAC_WRP_HANDLE handle,
      MAC_WRP_PIB_ATTRIBUTE attribute,
      uint16_t index,
      MAC_WRP_PIB_VALUE *pibValue
    )

  Summary:
    The MAC_WRP_GetRequestSync primitive gets the value of an attribute in the
    MAC layer Parameter Information Base (PIB).

  Description:
    GetRequestSync primitive is used to get the value of a PIB.
    Sync suffix indicates that result is provided upon function call return,
    in the pibValue parameter.

  Precondition:
    A valid MAC Wrapper Handle has to be obtained before.

  Parameters:
    handle - A valid handle which identifies the Mac Wrapper instance

    attribute - Identifier of the Attribute to retrieve value

    index - Index of element in case Attribute is a table
            Otherwise index must be set to '0'

    pibValue - Pointer to MAC_WRP_PIB_VALUE object where value will be returned

  Returns:
    Result of get operation as a MAC_WRP_STATUS code.

  Example:
    <code>
    // ...
    MAC_WRP_HANDLE handle;
    handle = MAC_WRP_Open(G3_MAC_WRP_INDEX_0);
    // ...

    MAC_WRP_STATUS status;
    MAC_WRP_PIB_VALUE value;
    status = MAC_WRP_GetRequestSync(handle, MAC_WRP_PIB_MAX_FRAME_RETRIES, 0, &value);
    if (status == MAC_WRP_STATUS_SUCCESS)
    {
        // Get value from 'value' parameter
    }
    </code>

  Remarks:
    None.
*/
MAC_WRP_STATUS MAC_WRP_GetRequestSync(MAC_WRP_HANDLE handle,
    MAC_WRP_PIB_ATTRIBUTE attribute, uint16_t index, MAC_WRP_PIB_VALUE *pibValue);

// *****************************************************************************
/* Function:
    MAC_WRP_STATUS MAC_WRP_SetRequestSync
    (
      MAC_WRP_HANDLE handle,
      MAC_WRP_PIB_ATTRIBUTE attribute,
      uint16_t index,
      const MAC_WRP_PIB_VALUE *pibValue
    )

  Summary:
    The MAC_WRP_SetRequestSync primitive sets the value of an attribute in the
    MAC layer Parameter Information Base (PIB).

  Description:
    SetRequestSync primitive is used to set the value of a PIB.
    Sync suffix indicates that result of set operation is provided upon
    function call return, in the return status code.

  Precondition:
    A valid MAC Wrapper Handle has to be obtained before.

  Parameters:
    handle - A valid handle which identifies the Mac Wrapper instance

    attribute - Identifier of the Attribute to provide value

    index - Index of element in case Attribute is a table
            Otherwise index must be set to '0'

    pibValue - Pointer to MAC_WRP_PIB_VALUE object where value is contained

  Returns:
    Result of set operation as a MAC_WRP_STATUS code.

  Example:
    <code>
    // ...
    MAC_WRP_HANDLE handle;
    handle = MAC_WRP_Open(G3_MAC_WRP_INDEX_0);
    // ...

    MAC_WRP_STATUS status;
    const MAC_WRP_PIB_VALUE value = {
        .length = 1,
        .value = 6
    };

    status = MAC_WRP_SetRequestSync(handle, MAC_WRP_PIB_MAX_FRAME_RETRIES, 0, &value);
    if (status == MAC_WRP_STATUS_SUCCESS)
    {
        // PIB correctly set
    }
    </code>

  Remarks:
    None.
*/
MAC_WRP_STATUS MAC_WRP_SetRequestSync(MAC_WRP_HANDLE handle,
    MAC_WRP_PIB_ATTRIBUTE attribute, uint16_t index, const MAC_WRP_PIB_VALUE *pibValue);

// *****************************************************************************
/* Function:
    MAC_WRP_ResetRequest
    (
      MAC_WRP_HANDLE handle,
      MAC_WRP_RESET_REQUEST_PARAMS *rstParams
    )

  Summary:
    The MAC_WRP_ResetRequest primitive resets the MAC Wrapper module.

  Description:
    Reset operation initializes MAC Wrapper State Machine and PIB to their
    default values. Result is provided in the corresponding Confirm callback.

  Precondition:
    A valid MAC Wrapper Handle has to be obtained before.

  Parameters:
    handle - A valid handle which identifies the Mac Wrapper instance

    rstParams - Pointer to structure containing required parameters for request

  Returns:
    None.

  Example:
    <code>
    // ...
    MAC_WRP_HANDLE handle;
    handle = MAC_WRP_Open(G3_MAC_WRP_INDEX_0);
    // ...

    MAC_WRP_RESET_REQUEST_PARAMS params = {
        .setDefaultPib = true
    };

    MAC_WRP_ResetRequest(handle, &params);

    // Wait for Reset Confirm
    </code>

  Remarks:
    None.
*/
void MAC_WRP_ResetRequest(MAC_WRP_HANDLE handle, MAC_WRP_RESET_REQUEST_PARAMS *rstParams);

// *****************************************************************************
/* Function:
    MAC_WRP_ScanRequest
    (
      MAC_WRP_HANDLE handle,
      MAC_WRP_SCAN_REQUEST_PARAMS *scanParams
    )

  Summary:
    The MAC_WRP_ScanRequest primitive sets MAC layer(s) in Network Scan mode.

  Description:
    Scan operation asks MAC layer(s) to send a Beacon Request frame and wait
    for incoming Beacon frames.
    During the Scan period, Beacons received will be notified by means of
    MAC_WRP_BeaconNotifyIndication callback.
    Result is provided in the corresponding Confirm callback.

  Precondition:
    A valid MAC Wrapper Handle has to be obtained before.

  Parameters:
    handle - A valid handle which identifies the Mac Wrapper instance

    scanParams - Pointer to structure containing required parameters for request

  Returns:
    None.

  Example:
    <code>
    // ...
    MAC_WRP_HANDLE handle;
    handle = MAC_WRP_Open(G3_MAC_WRP_INDEX_0);
    // ...

    MAC_WRP_SCAN_REQUEST_PARAMS params = {
        .scanDuration = 15
    };

    MAC_WRP_ScanRequest(handle, &params);

    // Wait for Scan Confirm
    </code>

  Remarks:
    None.
*/
void MAC_WRP_ScanRequest(MAC_WRP_HANDLE handle, MAC_WRP_SCAN_REQUEST_PARAMS *scanParams);

// *****************************************************************************
/* Function:
    MAC_WRP_StartRequest
    (
      MAC_WRP_HANDLE handle,
      MAC_WRP_START_REQUEST_PARAMS *startParams
    )

  Summary:
    The MAC_WRP_StartRequest primitive starts a G3 Network and sets the device
    as the PAN Coordinator.

  Description:
    Start operation asks MAC layer(s) to start a G3 Network, turning the device
    into the PAN Coordinator of such Network, and setting the PAN Identifier.
    Result is provided in the corresponding Confirm callback.

  Precondition:
    A valid MAC Wrapper Handle has to be obtained before.

  Parameters:
    handle - A valid handle which identifies the Mac Wrapper instance

    startParams - Pointer to structure containing required parameters for request

  Returns:
    None.

  Example:
    <code>
    // ...
    MAC_WRP_HANDLE handle;
    handle = MAC_WRP_Open(G3_MAC_WRP_INDEX_0);
    // ...

    MAC_WRP_START_REQUEST_PARAMS params = {
        .panId = 0x1234
    };

    MAC_WRP_StartRequest(handle, &params);

    // Wait for Start Confirm
    </code>

  Remarks:
    This primitive is only used by the PAN Coordinator node,
    which is the one in charge of Starting the PAN.
*/
void MAC_WRP_StartRequest(MAC_WRP_HANDLE handle, MAC_WRP_START_REQUEST_PARAMS *startParams);

// *****************************************************************************
/* Function:
    MAC_WRP_AVAILABLE_MAC_LAYERS MAC_WRP_GetAvailableMacLayers
    (
      MAC_WRP_HANDLE handle
    )

  Summary:
    The MAC_WRP_GetAvailableMacLayers primitive gets the available MAC layer(s).

  Description:
    G3 projects may include PLC MAC, RF MAC, or both. This funtion allows upper
    layers to get which MAC layer(s) is/are available at runtime.

  Precondition:
    A valid MAC Wrapper Handle has to be obtained before.

  Parameters:
    handle - A valid handle which identifies the Mac Wrapper instance

  Returns:
    Mac layer availability as a MAC_WRP_AVAILABLE_MAC_LAYERS enumerated value.

  Example:
    <code>
    // ...
    MAC_WRP_HANDLE handle;
    handle = MAC_WRP_Open(G3_MAC_WRP_INDEX_0);
    // ...

    MAC_WRP_AVAILABLE_MAC_LAYERS availableLayers;
    availableLayers = MAC_WRP_GetAvailableMacLayers(handle);

    if (availableLayers == MAC_WRP_AVAILABLE_MAC_PLC) {
      // Only PLC MAC available
    }
    else if (availableLayers == MAC_WRP_AVAILABLE_MAC_RF) {
      // Only RF MAC available
    }
    else {
      // Both MAC layers available
    }
    </code>

  Remarks:
    None.
*/
MAC_WRP_AVAILABLE_MAC_LAYERS MAC_WRP_GetAvailableMacLayers(MAC_WRP_HANDLE handle);

// *****************************************************************************
/* MAC Wrapper Callback Handlers Structure

   Summary:
    Set of Event Handler function pointers to receive events from MAC Wrapper.

   Description:
    Defines the set of callback functions that MAC Wrapper uses to generate
    events to upper layer.

   Remarks:
    In case an event is to be ignored, setting its corresponding callback
    function to NULL will lead to the event not being generated.
*/
typedef struct
{
    /* Callbacks */
    MAC_WRP_DataConfirm dataConfirmCallback;
    MAC_WRP_DataIndication dataIndicationCallback;
    MAC_WRP_ResetConfirm resetConfirmCallback;
    MAC_WRP_BeaconNotifyIndication beaconNotifyIndicationCallback;
    MAC_WRP_ScanConfirm scanConfirmCallback;
    MAC_WRP_StartConfirm startConfirmCallback;
    MAC_WRP_CommStatusIndication commStatusIndicationCallback;
    MAC_WRP_SnifferIndication snifferIndicationCallback;
} MAC_WRP_HANDLERS;

// *****************************************************************************
/* MAC Wrapper Init Structure

   Summary:
    Initialization Data for MAC Wrapper to be provided on Init routine.

   Description:
    Defines the set of callback functions that MAC Wrapper uses to generate
    events to upper layer and, in case of PLC MAC being present, the PLC
    band to use.

   Remarks:
    None.
*/
typedef struct
{
    /* Callbacks */
    MAC_WRP_HANDLERS macWrpHandlers;
    /* PLC working band. Only used if PLC MAC is present */
    MAC_WRP_BAND plcBand;
} MAC_WRP_INIT;

// *****************************************************************************
/* MAC Wrapper Data Structure

   Summary:
    Object used to keep any data required for an instance of the module.

   Description:
    Contains status of module state machine, runtime variables
    and callback definitions.

   Remarks:
    None.
*/
typedef struct
{
    /* Flag to indicate this object is in use  */
    bool inUse;
    /* State of the MAC Wrapper module */
    MAC_WRP_STATE state;
    /* Mac Wrapper instance handle */
    MAC_WRP_HANDLE macWrpHandle;
    /* Callbacks */
    MAC_WRP_HANDLERS macWrpHandlers;
    /* PLC working band. Only used if PLC MAC is present */
    MAC_WRP_BAND plcBand;
} MAC_WRP_DATA;

// *****************************************************************************
/* Function:
    void MAC_WRP_Init
    (
      MAC_WRP_HANDLE handle, 
      MAC_WRP_INIT *init
    )

  Summary:
    Initializes the MAC Wrapper module data for a given instance.

  Description:
    This routine initializes the MAC Wrapper data structures for a given instance.
    Callback handlers for event notification are set in this function.
    In case MAC PLC layer is present, PLC band is specified here, otherwise
    this parameter is not present in the init structure.

  Precondition:
    None.

  Parameters:
    handle - A valid handle which identifies the Mac Wrapper instance

    init  - Pointer to the init data structure containing any data necessary to
            initialize the module.

  Returns:
    None.

  Example:
    <code>
    // The following code snippet shows an example MAC Wrapper initialization.
    // ...
    MAC_WRP_HANDLE handle;
    handle = MAC_WRP_Open(G3_MAC_WRP_INDEX_0);
    // ...

    MAC_WRP_INIT macWrpInit = {
        .macWrpHandlers.dataConfirmCallback = appDataConfirm,
        .macWrpHandlers.dataIndicationCallback = appDataIndication,
        .macWrpHandlers.resetConfirmCallback = appResetConfirm,
        .macWrpHandlers.beaconNotifyIndicationCallback = appBeaconIndication,
        .macWrpHandlers.scanConfirmCallback = appScanConfirm,
        .macWrpHandlers.startConfirmCallback = NULL, // Start primitive not used
        .macWrpHandlers.commStatusIndicationCallback = appCommStatus,
        .macWrpHandlers.snifferIndicationCallback = NULL, // MAC Sniffer not used
        .plcBand = MAC_WRP_BAND_CENELEC_A,
    };

    MAC_WRP_Init(handle, &macWrpInit);
    </code>

  Remarks:
    This routine must be called before any other MAC Wrapper API function.
*/
void MAC_WRP_Init(MAC_WRP_HANDLE handle, MAC_WRP_INIT *init);

// *****************************************************************************
/* Function:
    SYS_STATUS MAC_WRP_Status
    (
      void
    )

  Summary:
    The MAC_WRP_Status primitive retrieves the Status of G3 MAC layer(s).

  Description:
    This primitive is intended to be called before using G3 MAC layer(s)
    to ensure they are ready to be used.

  Precondition:
    MAC_WRP_Init routine must have been called before.

  Parameters:
    None

  Returns:
    Status of G3 MAC layer(s) as a SYS_STATUS code.

  Example:
    <code>
    SYS_STATUS status;
    status = MAC_WRP_Status();
    if (status == SYS_STATUS_READY)
    {
        // MAC is ready to be used
    }
    </code>

  Remarks:
    None.
*/
SYS_STATUS MAC_WRP_Status(void);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // #ifndef _MAC_WRAPPER_H

/*******************************************************************************
 End of File
*/
