/*******************************************************************************
  G3 MAC Common Definitions Header File

  Company:
    Microchip Technology Inc.

  File Name:
    mac_common_defs.h

  Summary:
    G3 MAC Common Types and Definitions Header File

  Description:
    This file contains definitions of macros and types
    to be used by MAC Wrapper when accessing G3 MAC Common layer.
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

#ifndef MAC_COMMON_DEFS_H
#define MAC_COMMON_DEFS_H

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

#define MAC_MAX_MAC_PAYLOAD_SIZE     (400U)
#define MAC_SECURITY_OVERHEAD        (4U)
#define MAC_MAX_MAC_HEADER_SIZE      (32U)
#define MAC_SECURITY_HEADER_SIZE     (6U)

#define MAC_PAN_ID_BROADCAST         (0xFFFFU)
#define MAC_SHORT_ADDRESS_BROADCAST  (0xFFFFU)
#define MAC_SHORT_ADDRESS_UNDEFINED  (0xFFFFU)

#define MAC_KEY_TABLE_ENTRIES        (2U)
#define MAC_SECURITY_KEY_LENGTH      (16U)

#define MAX_TONE_GROUPS              (24U)

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* MAC Status values definition

   Summary:
    Identifies the possible return values for the status field on MAC
    Confirm and Indication callbacks.

   Description:
    Whenever a Confirm or Indication callback is generated from MAC to
    indicate an event, a Status value is included with the event result.

   Remarks:
    None.
*/
typedef enum
{
    MAC_STATUS_SUCCESS = 0x00,
    MAC_STATUS_CHANNEL_ACCESS_FAILURE = 0xE1,
    MAC_STATUS_COUNTER_ERROR = 0xDB,
    MAC_STATUS_DENIED = 0xE2,
    MAC_STATUS_FRAME_TOO_LONG = 0xE5,
    MAC_STATUS_INVALID_HANDLE = 0xE7,
    MAC_STATUS_INVALID_INDEX = 0xF9,
    MAC_STATUS_INVALID_PARAMETER = 0xE8,
    MAC_STATUS_LIMIT_REACHED = 0xFA,
    MAC_STATUS_NO_ACK = 0xE9,
    MAC_STATUS_NO_SHORT_ADDRESS = 0xEC,
    MAC_STATUS_READ_ONLY = 0xFB,
    MAC_STATUS_SECURITY_ERROR = 0xE4,
    MAC_STATUS_TRANSACTION_OVERFLOW = 0xF1,
    MAC_STATUS_UNAVAILABLE_KEY = 0xF3,
    MAC_STATUS_UNSUPPORTED_ATTRIBUTE = 0xF4,
    MAC_STATUS_UNSUPPORTED_SECURITY = 0xDF,
    MAC_STATUS_ALTERNATE_PANID_DETECTION = 0x80,
    MAC_STATUS_QUEUE_FULL = 0xD0,
} MAC_STATUS;

// *****************************************************************************
/* MAC Security Key definition

   Summary:
    Defines the fields of a Security Key object.

   Description:
    This structure contains the fields which define a Security Key object,
    which contains information of its validity and the Key itself.

   Remarks:
    None.
*/
typedef struct
{
    uint8_t key[MAC_SECURITY_KEY_LENGTH];
    bool valid;
} MAC_SECURITY_KEY;

// *****************************************************************************
/* MAC PAN ID definition

   Summary:
    Defines the PAN ID as an unsigned 16-bit integer.

   Description:
    Creates an unsigned 16-bit integer specific type for PAN ID definition.

   Remarks:
    None.
*/
typedef uint16_t MAC_PAN_ID;

// *****************************************************************************
/* MAC Short Address definition

   Summary:
    Defines the Short Address as an unsigned 16-bit integer.

   Description:
    Creates an unsigned 16-bit integer specific type for Short Address definition.

   Remarks:
    None.
*/
typedef uint16_t MAC_SHORT_ADDRESS;

// *****************************************************************************
/* MAC Extended Address definition

   Summary:
    Defines the Extended Address as an array of 8 unsigned 8-bit integers.

   Description:
    Creates an unsigned 8-bit integer array specific type for Extended Address
    definition.

   Remarks:
    Extended Address is equivalent to EUI-64 as defined in some related documents.
*/
typedef struct
{
    uint8_t address[8];
} MAC_EXTENDED_ADDRESS;

#pragma pack(push,2)

// *****************************************************************************
/* MAC MIB Common definition

   Summary:
    Defines the MAC Common Information Base.

   Description:
    This structure holds the MAC Common IB, it contains one field per each
    defined PIB.

   Remarks:
    None.
*/
typedef struct
{
    uint16_t rcCoord;
    MAC_PAN_ID panId;
    MAC_SHORT_ADDRESS shortAddress;
    uint8_t posTableEntryTtl;
    uint8_t posRecentEntryThreshold;
    MAC_EXTENDED_ADDRESS extendedAddress;
    MAC_SECURITY_KEY keyTable[MAC_KEY_TABLE_ENTRIES];
    bool coordinator;
    bool promiscuousMode;
} MAC_COMMON_MIB;

// *****************************************************************************
/* MAC Address Mode definition

   Summary:
    Identifies the available Addressing Modes for G3 frames.

   Description:
    This enumeration identifies the possible Addressing modes to use in G3
    frames. It is a subset of Addressing Modes defined in IEEE 802.15.4 standard.

   Remarks:
    None.
*/
typedef enum
{
    MAC_ADDRESS_MODE_NO_ADDRESS = 0x00,
    MAC_ADDRESS_MODE_SHORT = 0x02,
    MAC_ADDRESS_MODE_EXTENDED = 0x03,
} MAC_ADDRESS_MODE;

// *****************************************************************************
/* MAC Address definition

   Summary:
    Identifies a G3 MAC layer Address.

   Description:
    This structure contains the Addressing Mode and the short/extended address
    for a G3 node.

   Remarks:
    None.
*/
typedef struct
{
    union {
        MAC_SHORT_ADDRESS shortAddress;
        MAC_EXTENDED_ADDRESS extendedAddress;
    };
    MAC_ADDRESS_MODE addressMode;
} MAC_ADDRESS;

// *****************************************************************************
/* MAC Auxiliary Security Header definition

   Summary:
    Defines the Auxiliary Security Header fields of the MAC Header.

   Description:
    This structure contains the fields which define the Auxiliary Security Header
    of a MAC frame header, as defined in IEEE 802.15.4.

   Remarks:
    None.
*/
typedef struct
{
    uint32_t frameCounter;
    uint8_t securityLevel : 3;
    uint8_t keyIdentifierMode : 2;
    uint8_t reserved : 3;
    uint8_t keyIdentifier;
} MAC_AUXILIARY_SECURITY_HEADER;

// *****************************************************************************
/* MAC Security Level definition

   Summary:
    Identifies the available Security Levels for G3 frames.

   Description:
    This enumeration identifies the possible Security Levels to use in G3
    frames. It is a subset of Security Levels defined in IEEE 802.15.4 standard.

   Remarks:
    None.
*/
typedef enum
{
    MAC_SECURITY_LEVEL_NONE = 0x00,
    MAC_SECURITY_LEVEL_ENC_MIC_32 = 0x05,
} MAC_SECURITY_LEVEL;

// *****************************************************************************
/* MAC Quality of Service values definition

   Summary:
    Identifies the available Quality of Service values for G3 frames.

   Description:
    This enumeration identifies the possible Quality of Service values to use
    in G3 frames. The meaning of this field is to use Normal or High Priority
    window for transmission.

   Remarks:
    None.
*/
typedef enum
{
    MAC_QUALITY_OF_SERVICE_NORMAL_PRIORITY = 0x00,
    MAC_QUALITY_OF_SERVICE_HIGH_PRIORITY = 0x01,
} MAC_QUALITY_OF_SERVICE;

// *****************************************************************************
/* MAC TX Option values definition

   Summary:
    Identifies the available TX Option values for G3 frames.

   Description:
    This enumeration identifies the possible TX Option values to use in G3
    frames. The meaning of this field is to ask for Acknowledge or not.

   Remarks:
    None.
*/
typedef enum
{
    MAC_TX_OPTION_NO_ACK = 0x00,
    MAC_TX_OPTION_ACK = 0x01,
} MAC_TX_OPTIONS;

// *****************************************************************************
/* MAC Timestamp definition

   Summary:
    Defines the Timestamp as an unsigned 32-bit integer.

   Description:
    Creates an unsigned 32-bit integer specific type for Timestamp definition.

   Remarks:
    None.
*/
typedef uint32_t MAC_TIMESTAMP;

// *****************************************************************************
/* MAC Tone Map definition

   Summary:
    Defines the G3 Tone Map at Mac level.

   Description:
    Tone Map is a bitmap in which each bit represents one of the subbands
    present in the PLC work band (6 subbands in CEN-A, 4 subbands in CEN-B,
    24 subbands in FCC). '1' indicates the subband is used, '0' indicates
    the subband is not used.

   Remarks:
    None.
*/
typedef struct
{
    uint8_t toneMap[(MAX_TONE_GROUPS + 7) / 8];
} MAC_TONE_MAP;

// *****************************************************************************
/* MAC PAN Descriptor definition

   Summary:
    Defines the fields of a PAN Descriptor object.

   Description:
    This structure contains the fields which define a PAN Descriptor object,
    which contains information of the Beacon frames seen on a G3 network.

   Remarks:
    None.
*/
typedef struct
{
    uint16_t rcCoord;
    MAC_PAN_ID panId;
    MAC_SHORT_ADDRESS lbaAddress;
    uint8_t linkQuality;
} MAC_PAN_DESCRIPTOR;

// *****************************************************************************
/* MAC Device Table Entry definition

   Summary:
    Defines the fields of an entry in the MAC Device Table.

   Description:
    This structure contains the fields which define a MAC Device Table entry.
    This table maintains the Frame Counter received from neighbouring nodes.

   Remarks:
    None.
*/
typedef struct
{
    uint32_t frameCounter;
    MAC_SHORT_ADDRESS shortAddress;
} MAC_DEVICE_TABLE_ENTRY;

// *****************************************************************************
/* MAC Data Request Parameters

   Summary:
    Defines the Parameters for the MAC Data Request primitive.

   Description:
    The structure contains the fields used by the MAC Data Request primitive.

   Remarks:
    None.
*/
typedef struct
{
    /* Pointer to the set of octets forming the MSDU to be transmitted */
    const uint8_t *msdu;
    /* The PAN identifier of the entity to which the MSDU is being transferred */
    MAC_PAN_ID destPanId;
    /* The number of octets contained in the MSDU to be transmitted */
    uint16_t msduLength;
    /* Source address mode 0, 16, 64 bits */
    MAC_ADDRESS_MODE srcAddressMode;
    /* The device address of the entity to which the MSDU is being transferred */
    MAC_ADDRESS destAddress;
    /* The handle associated with the MSDU to be transmitted */
    uint8_t msduHandle;
    /* Transmission options for this MSDU: 0 unacknowledged, 1 acknowledged */
    uint8_t txOptions;
    /* The index of the encryption key to be used */
    uint8_t keyIndex;
    /* The QOS of the MSDU: 0x00 normal priority, 0x01 high priority */
    MAC_QUALITY_OF_SERVICE qualityOfService;
    /* The security level to be used: 0x00 unecrypted, 0x05 encrypted */
    MAC_SECURITY_LEVEL securityLevel;
} MAC_DATA_REQUEST_PARAMS;

// *****************************************************************************
/* MAC Data Confirm Parameters

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
    MAC_TIMESTAMP timestamp;
    /* The handle associated with the MSDU being confirmed */
    uint8_t msduHandle;
    /* The result of the last MSDU transmission */
    MAC_STATUS status;
} MAC_DATA_CONFIRM_PARAMS;

// *****************************************************************************
/* MAC Data Indication Parameters

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
    /* Pointer to the set of octets forming the received MSDU */
    uint8_t *msdu;
    /* Reception time, refered to MAC milliseconds counter */
    MAC_TIMESTAMP timestamp;
    /* The PAN identifier of the device from which the frame was received */
    MAC_PAN_ID srcPanId;
    /* The PAN identifier of the entity to which the MSDU is being transferred */
    MAC_PAN_ID destPanId;
    /* The number of octets of the MSDU to be indicated to the upper layer */
    uint16_t msduLength;
    /* The address of the device which sent the frame */
    MAC_ADDRESS srcAddress;
    /* The address of the entity to which the MSDU is being transferred */
    MAC_ADDRESS destAddress;
    /* The LQI value measured during reception of the frame */
    uint8_t linkQuality;
    /* The Data Sequence Number of the received frame */
    uint8_t dsn;
    /* The index of the key used for decryption */
    uint8_t keyIndex;
    /* Modulation Type of the received frame */
    uint8_t rxModulation;
    /* Modulation scheme of the received frame */
    uint8_t rxModulationScheme;
    /* Weakest Modulation Type in which the frame could have been received */
    uint8_t computedModulation;
    /* Weakest Modulation Scheme in which the frame could have been received */
    uint8_t computedModulationScheme;
    /* Phase Differential compared to Node that sent the frame */
    uint8_t phaseDifferential;
    /* Security level of the received frame: 0x00 unecrypted, 0x05 encrypted */
    MAC_SECURITY_LEVEL securityLevel;
    /* The QOS of the MSDU: 0x00 normal priority, 0x01 high priority */
    MAC_QUALITY_OF_SERVICE qualityOfService;
    /* Tone Map of the received frame */
    MAC_TONE_MAP rxToneMap;
    /* Weakest Tone Map with which the frame could have been received */
    MAC_TONE_MAP computedToneMap;
} MAC_DATA_INDICATION_PARAMS;

// *****************************************************************************
/* MAC Sniffer Indication Parameters

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
    /* Pointer to the set of octets forming the received MSDU */
    uint8_t *msdu;
    /* Reception time, refered to MAC milliseconds counter */
    MAC_TIMESTAMP timestamp;
    /* The PAN identifier of the device from which the frame was received */
    MAC_PAN_ID srcPanId;
    /* The PAN identifier of the entity to which the MSDU is being transferred */
    MAC_PAN_ID destPanId;
    /* The number of octets of the MSDU to be indicated to the upper layer */
    uint16_t msduLength;
    /* The address of the device which sent the frame */
    MAC_ADDRESS srcAddress;
    /* The address of the entity to which the MSDU is being transferred */
    MAC_ADDRESS destAddress;
    /* The LQI value measured during reception of the frame */
    uint8_t linkQuality;
    /* The Data Sequence Number of the received frame */
    uint8_t dsn;
    /* The index of the key used for decryption */
    uint8_t keyIndex;
    /* Modulation Type of the received frame */
    uint8_t rxModulation;
    /* Modulation scheme of the received frame */
    uint8_t rxModulationScheme;
    /* Weakest Modulation Type in which the frame could have been received */
    uint8_t computedModulation;
    /* Weakest Modulation Scheme in which the frame could have been received */
    uint8_t computedModulationScheme;
    /* Phase Differential compared to Node that sent the frame */
    uint8_t phaseDifferential;
    /* Frame Type as defined in IEEE 802.15.4 standard */
    uint8_t frameType;
    /* Security level of the received frame: 0x00 unecrypted, 0x05 encrypted */
    MAC_SECURITY_LEVEL securityLevel;
    /* The QOS of the MSDU: 0x00 normal priority, 0x01 high priority */
    MAC_QUALITY_OF_SERVICE qualityOfService;
    /* Tone Map of the received frame */
    MAC_TONE_MAP rxToneMap;
    /* Weakest Tone Map with which the frame could have been received */
    MAC_TONE_MAP computedToneMap;
} MAC_SNIFFER_INDICATION_PARAMS;

// *****************************************************************************
/* MAC Beacon Notify Indication Parameters

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
    MAC_PAN_DESCRIPTOR panDescriptor;
} MAC_BEACON_NOTIFY_INDICATION_PARAMS;

// *****************************************************************************
/* MAC Reset Request Parameters

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
} MAC_RESET_REQUEST_PARAMS;

// *****************************************************************************
/* MAC Reset Confirm Parameters

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
    MAC_STATUS status;
} MAC_RESET_CONFIRM_PARAMS;

// *****************************************************************************
/* MAC Scan Request Parameters

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
} MAC_SCAN_REQUEST_PARAMS;

// *****************************************************************************
/* MAC Scan Confirm Parameters

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
    MAC_STATUS status;
} MAC_SCAN_CONFIRM_PARAMS;

// *****************************************************************************
/* MAC Comm Status Indication Parameters

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
    MAC_PAN_ID panId;
    /* The address of the device which sent the frame */
    MAC_ADDRESS srcAddress;
    /* The address of the device intended to receive the frame */
    MAC_ADDRESS destAddress;
    /* Type of event detected */
    MAC_STATUS status;
    /* Security level of the received frame: 0x00 unecrypted, 0x05 encrypted */
    MAC_SECURITY_LEVEL securityLevel;
    /* The index of the key used for decryption */
    uint8_t keyIndex;
} MAC_COMM_STATUS_INDICATION_PARAMS;

#pragma pack(pop)

// *****************************************************************************
/* MAC Network Start Request Parameters

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
    MAC_PAN_ID panId;
} MAC_START_REQUEST_PARAMS;

// *****************************************************************************
/* MAC Start Confirm Parameters

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
    MAC_STATUS status;
} MAC_START_CONFIRM_PARAMS;

// *****************************************************************************
/* MAC Data Confirm Event Handler Function Pointer

  Summary:
    Pointer to a Data Confirm Event handler function.

  Description:
    This data type defines the required function signature for the MAC
    Data Confirm event handling callback function. A client must register a
    pointer using the event handling function whose function signature
    (parameter and return value types) matches the types specified by this
    function pointer in order to receive Data Confirm events back from module.

  Parameters:
    dcParams - Pointer to structure containing parameters related to Confirm

  Example:
    <code>
    App_DataConfirm(MAC_DATA_CONFIRM_PARAMS *params)
    {
        if (params->status == MAC_STATUS_SUCCESS)
        {
            txHandler = params->msduHandle;
        }
    }
    </code>

  Remarks:
    None.
*/
typedef void (*MAC_DataConfirm)(MAC_DATA_CONFIRM_PARAMS *dcParams);

// *****************************************************************************
/* MAC Data Indication Event Handler Function Pointer

  Summary:
    Pointer to a Data Indication Event handler function.

  Description:
    This data type defines the required function signature for the MAC
    Data Indication event handling callback function. A client must register a
    pointer using the event handling function whose function signature
    (parameter and return value types) matches the types specified by this
    function pointer in order to receive Data Indication events back from module.

  Parameters:
    diParams - Pointer to structure containing parameters related to Indication

  Example:
    <code>
    App_DataIndication(MAC_DATA_INDICATION_PARAMS *params)
    {
        if (params->destPanId == myPanId)
        {
            if (params->destAddress.addressMode == MAC_ADDRESS_MODE_SHORT)
            {
                if (params->destAddress.shortAddress == myShortAddress)
                {

                }
            }
            else if (params->destAddress.addressMode == MAC_ADDRESS_MODE_EXTENDED)
            {
                if (params->destAddress.extendedAddress == myExtendedAddress)
                {

                }
            }
        }
    }
    </code>

  Remarks:
    None.
*/
typedef void (*MAC_DataIndication)(MAC_DATA_INDICATION_PARAMS *diParams);

// *****************************************************************************
/* MAC Sniffer Indication Event Handler Function Pointer

  Summary:
    Pointer to a Sniffer Indication Event handler function.

  Description:
    This data type defines the required function signature for the MAC
    Sniffer Indication event handling callback function. A client must register a
    pointer using the event handling function whose function signature
    (parameter and return value types) matches the types specified by this
    function pointer in order to receive Sniffer Indication events back from module.

  Parameters:
    siParams - Pointer to structure containing parameters related to Indication

  Example:
    <code>
    App_SnifferIndication(MAC_SNIFFER_INDICATION_PARAMS *params)
    {
        App_SnifferHandlingTool(params);
    }
    </code>

  Remarks:
    None.
*/
typedef void (*MAC_SnifferIndication)(MAC_SNIFFER_INDICATION_PARAMS *siParams);

// *****************************************************************************
/* MAC Reset Confirm Event Handler Function Pointer

  Summary:
    Pointer to a Reset Confirm Event handler function.

  Description:
    This data type defines the required function signature for the MAC
    Reset Confirm event handling callback function. A client must register a
    pointer using the event handling function whose function signature
    (parameter and return value types) matches the types specified by this
    function pointer in order to receive Reset Confirm events back from module.

  Parameters:
    rcParams - Pointer to structure containing parameters related to Confirm

  Example:
    <code>
    App_ResetConfirm(MAC_RESET_CONFIRM_PARAMS *params)
    {
        if (params->status != MAC_STATUS_SUCCESS)
        {

        }
        else
        {

        }
    }
    </code>

  Remarks:
    None.
*/
typedef void (*MAC_ResetConfirm)(MAC_RESET_CONFIRM_PARAMS *rcParams);

// *****************************************************************************
/* MAC Beacon Notify Indication Event Handler Function Pointer

  Summary:
    Pointer to a Beacon Notify Indication Event handler function.

  Description:
    This data type defines the required function signature for the MAC
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
    MAC_PAN_DESCRIPTOR lowestRCDescriptor;
    lowestRCDescriptor.rcCoord = 0xFFFF;

    App_BeaconIndication(MAC_BEACON_NOTIFY_INDICATION_PARAMS *params)
    {
        if (params->panDescriptor.rcCoord < lowestRCDescriptor.rcCoord)
        {
            lowestRCDescriptor = params->panDescriptor;
        }
    }
    </code>

  Remarks:
    None.
*/
typedef void (*MAC_BeaconNotifyIndication)(MAC_BEACON_NOTIFY_INDICATION_PARAMS *bcnParams);

// *****************************************************************************
/* MAC Scan Confirm Event Handler Function Pointer

  Summary:
    Pointer to a Scan Confirm Event handler function.

  Description:
    This data type defines the required function signature for the MAC
    Scan Confirm event handling callback function. A client must register a
    pointer using the event handling function whose function signature
    (parameter and return value types) matches the types specified by this
    function pointer in order to receive Scan Confirm events back from module.

  Parameters:
    scParams - Pointer to structure containing parameters related to Confirm

  Example:
    <code>
    App_ScanConfirm(MAC_SCAN_CONFIRM_PARAMS *params)
    {
        if (params->status != MAC_STATUS_SUCCESS)
        {

        }
        else
        {

        }
    }
    </code>

  Remarks:
    None.
*/
typedef void (*MAC_ScanConfirm)(MAC_SCAN_CONFIRM_PARAMS *scParams);

// *****************************************************************************
/* MAC Start Confirm Event Handler Function Pointer

  Summary:
    Pointer to a Start Confirm Event handler function.

  Description:
    This data type defines the required function signature for the MAC
    Start Confirm event handling callback function. A client must register a
    pointer using the event handling function whose function signature
    (parameter and return value types) matches the types specified by this
    function pointer in order to receive Start Confirm events back from module.

  Parameters:
    scParams - Pointer to structure containing parameters related to Confirm

  Example:
    <code>
    App_StartConfirm(MAC_START_CONFIRM_PARAMS *params)
    {
        if (params->status != MAC_STATUS_SUCCESS)
        {

        }
        else
        {

        }
    }
    </code>

  Remarks:
    None.
*/
typedef void (*MAC_StartConfirm)(MAC_START_CONFIRM_PARAMS *scParams);

// *****************************************************************************
/* MAC Comm Status Indication Event Handler Function Pointer

  Summary:
    Pointer to a Comm Status Indication Event handler function.

  Description:
    This data type defines the required function signature for the MAC
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
    App_CommStatusIndication(MAC_COMM_STATUS_INDICATION_PARAMS *params)
    {
        if (csParams->status == MAC_STATUS_ALTERNATE_PANID_DETECTION)
        {
            App_AlternateIDHandlingTool(params);
        }
        else
        {
            App_SecurityIssueHandlingTool(params);
        }
    }
    </code>

  Remarks:
    None.
*/
typedef void (*MAC_CommStatusIndication)(MAC_COMM_STATUS_INDICATION_PARAMS *csParams);

// *****************************************************************************
// *****************************************************************************
// Section: External Data
// *****************************************************************************
// *****************************************************************************

extern MAC_COMMON_MIB macMibCommon;

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // #ifndef MAC_COMMON_DEFS_H

/*******************************************************************************
 End of File
*/
