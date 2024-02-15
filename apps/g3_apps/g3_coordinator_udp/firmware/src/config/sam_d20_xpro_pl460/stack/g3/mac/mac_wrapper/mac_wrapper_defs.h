/*******************************************************************************
  G3 MAC Wrapper Definitions Header File

  Company:
    Microchip Technology Inc.

  File Name:
    mac_wrapper_defs.h

  Summary:
    G3 MAC Wrapper Definitions Header File

  Description:
    This file contains definitions, types, enums and structures to be used by
	upper layers when accessing G3 MAC layers.
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

#ifndef MAC_WRAPPER_DEFS_H
#define MAC_WRAPPER_DEFS_H

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

#define MAC_WRP_MAX_TONES                (72U)
#define MAC_WRP_MAX_TONE_GROUPS          (24U)

#define MAC_WRP_PAN_ID_BROADCAST         (0xFFFFU)
#define MAC_WRP_SHORT_ADDRESS_BROADCAST  (0xFFFFU)
#define MAC_WRP_SHORT_ADDRESS_UNDEFINED  (0xFFFFU)

#define MAC_WRP_SECURITY_KEY_LENGTH      (16U)

#define MAC_WRP_PIB_MAX_VALUE_LENGTH     (144U)

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* MAC Wrapper Handle

  Summary:
    Identifies an instance of MAC Wrapper module.

  Description:
    Handle is returned upon MAC Wrapper Open routine and is used for every other
    API function to identify module instance.

  Remarks:
    None.
*/
typedef uintptr_t MAC_WRP_HANDLE;

// *****************************************************************************
/* Invalid MAC Wrapper handle value to an instance

  Summary:
    Invalid handle value to a MAC Wrapper instance.

  Description:
    Defines the invalid handle value to a MAC Wrapper instance.

  Remarks:
    None.
*/
#define MAC_WRP_HANDLE_INVALID   ((MAC_WRP_HANDLE) (-1))

// *****************************************************************************
/* MAC Wrapper Bands definition

   Summary:
    Identifies the possible PLC band values, or if No PLC is used.

   Description:
    This enumeration identifies the possible PLC working band values.

   Remarks:
    It is only used when PLC MAC layer is present.
*/
typedef enum
{
    MAC_WRP_BAND_CENELEC_A = 0,
    MAC_WRP_BAND_CENELEC_B = 1,
    MAC_WRP_BAND_FCC = 2,
    MAC_WRP_BAND_ARIB = 3,
    MAC_WRP_BAND_RF_ONLY = 255,
} MAC_WRP_BAND;

// *****************************************************************************
/* MAC Wrapper Media Type options on Transmission Request

   Summary:
    Identifies the possible Media Type values on Transmission Request primitives.

   Description:
    This enumeration identifies the possible Media Type options when calling
    a Transmission Request primitive, which include just PLC, just RF, both,
    or one medium allowing the other as backup in case of failure.

   Remarks:
    It is only used when both PLC and RF MAC layers are present.
*/
typedef enum
{
    MAC_WRP_MEDIA_TYPE_REQ_PLC_BACKUP_RF = 0x00,
    MAC_WRP_MEDIA_TYPE_REQ_RF_BACKUP_PLC = 0x01,
    MAC_WRP_MEDIA_TYPE_REQ_BOTH = 0x02,
    MAC_WRP_MEDIA_TYPE_REQ_PLC_NO_BACKUP = 0x03,
    MAC_WRP_MEDIA_TYPE_REQ_RF_NO_BACKUP = 0x04,
} MAC_WRP_MEDIA_TYPE_REQUEST;

// *****************************************************************************
/* MAC Wrapper Media Type options on Transmission Confirm

   Summary:
    Identifies the possible Media Type values on Transmission Confirm callbacks.

   Description:
    This enumeration identifies the possible Media Type options when invoking
    a Transmission Confirm callback, which include sent on PLC as requested,
    sent on RF as requested, sent on both, or sent on the backup medium after
    being requested on the other.

   Remarks:
    It is only used when both PLC and RF MAC layers are present.
*/
typedef enum
{
    MAC_WRP_MEDIA_TYPE_CONF_PLC = 0x00,
    MAC_WRP_MEDIA_TYPE_CONF_RF = 0x01,
    MAC_WRP_MEDIA_TYPE_CONF_BOTH = 0x02,
    MAC_WRP_MEDIA_TYPE_CONF_PLC_AS_BACKUP = 0x03,
    MAC_WRP_MEDIA_TYPE_CONF_RF_AS_BACKUP = 0x04,
} MAC_WRP_MEDIA_TYPE_CONFIRM;

// *****************************************************************************
/* MAC Wrapper Media Type options on Reception Indication

   Summary:
    Identifies the possible Media Type values on Reception Indication callbacks.

   Description:
    This enumeration identifies the possible Media Type options when invoking
    a Reception Indication callback, which include received on PLC or received
    on RF.

   Remarks:
    It is only used when both PLC and RF MAC layers are present.
*/
typedef enum
{
    MAC_WRP_MEDIA_TYPE_IND_PLC = 0x00,
    MAC_WRP_MEDIA_TYPE_IND_RF = 0x01,
} MAC_WRP_MEDIA_TYPE_INDICATION;

// *****************************************************************************
/* MAC Wrapper Available MAC layers

   Summary:
    Defines the possible values for MAC layers availability.

   Description:
    This enumeration identifies the availability of MAC layers, so upper layers
    may know in advance which MAC layer(s) are available below.

   Remarks:
    None.
*/
typedef enum
{
    MAC_WRP_AVAILABLE_MAC_PLC = 0x00,
    MAC_WRP_AVAILABLE_MAC_RF = 0x01,
    MAC_WRP_AVAILABLE_MAC_BOTH = 0x02,
} MAC_WRP_AVAILABLE_MAC_LAYERS;

// *****************************************************************************
/* MAC Wrapper Tone Map definition

   Summary:
    Defines the G3 Tone Map at Mac Wrapper level.

   Description:
    Tone Map is a bitmap in which each bit represents one of the subbands
    present in the PLC work band (6 subbands in CEN-A, 4 subbands in CEN-B,
    24 subbands in FCC). '1' indicates the subband is used, '0' indicates
    the subband is not used.

   Remarks:
    It is only used when PLC MAC layer is present.
*/
typedef struct
{
    uint8_t toneMap[(MAC_WRP_MAX_TONE_GROUPS + 7) / 8];
} MAC_WRP_TONE_MAP;

// *****************************************************************************
/* MAC Wrapper Tone Mask definition

   Summary:
    Defines the G3 Tone Mask at Mac Wrapper level.

   Description:
    Tone Mask is a bitmap in which each bit represents one of the carriers
    present in the PLC work band (36 carriers in CEN-A, 16 carriers in CEN-B,
    72 carriers in FCC). '1' indicates the carrier is used, '0' indicates
    the carrier is not used.

   Remarks:
    It is only used when PLC MAC layer is present.
*/
typedef struct
{
    uint8_t toneMask[(MAC_WRP_MAX_TONES + 7) / 8];
} MAC_WRP_TONE_MASK;

// *****************************************************************************
/* MAC Wrapper Modulation Type definition

   Summary:
    Identifies the available Modulation Type Values for PLC communication.

   Description:
    This enumeration identifies the possible Modulation Types used when
    transmitting and receiving frames over PLC medium.

   Remarks:
    It is only used when PLC MAC layer is present.
*/
typedef enum
{
    MAC_WRP_MODULATION_ROBUST = 0x00,
    MAC_WRP_MODULATION_DBPSK_BPSK = 0x01,
    MAC_WRP_MODULATION_DQPSK_QPSK = 0x02,
    MAC_WRP_MODULATION_D8PSK_8PSK = 0x03,
    MAC_WRP_MODULATION_16_QAM = 0x04,
} MAC_WRP_MODULATION_TYPE;

// *****************************************************************************
/* MAC Wrapper Modulation Scheme definition

   Summary:
    Identifies the available Modulation Scheme Values for PLC communication.

   Description:
    This enumeration identifies the possible Modulation Schemes used when
    transmitting and receiving frames over PLC medium.

   Remarks:
    It is only used when PLC MAC layer is present.
*/
typedef enum
{
    MAC_WRP_MODULATION_SCHEME_DIFFERENTIAL = 0x00,
    MAC_WRP_MODULATION_SCHEME_COHERENT = 0x01,
} MAC_WRP_MODULATION_SCHEME;

// *****************************************************************************
/* MAC Wrapper Tone Map Response data definition

   Summary:
    Identifies the available fields in a Tone Map Response frame.

   Description:
    This structure contains the fields present in the Tone Map Response frames
    used in PLC communication.

   Remarks:
    It is only used when PLC MAC layer is present.
*/
typedef struct
{
    MAC_WRP_MODULATION_TYPE modulationType;
    MAC_WRP_MODULATION_SCHEME modulationScheme;
    MAC_WRP_TONE_MAP toneMap;
} MAC_WRP_TONE_MAP_RESPONSE_DATA;

// *****************************************************************************
/* MAC Wrapper PAN ID definition

   Summary:
    Defines the PAN ID as an unsigned 16-bit integer.

   Description:
    Creates an unsigned 16-bit integer specific type for PAN ID definition.

   Remarks:
    None.
*/
typedef uint16_t MAC_WRP_PAN_ID;

// *****************************************************************************
/* MAC Wrapper Short Address definition

   Summary:
    Defines the Short Address as an unsigned 16-bit integer.

   Description:
    Creates an unsigned 16-bit integer specific type for Short Address definition.

   Remarks:
    None.
*/
typedef uint16_t MAC_WRP_SHORT_ADDRESS;

// *****************************************************************************
/* MAC Wrapper Extended Address definition

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
} MAC_WRP_EXTENDED_ADDRESS;

// *****************************************************************************
/* MAC Wrapper Address Mode definition

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
    MAC_WRP_ADDRESS_MODE_NO_ADDRESS = 0x00,
    MAC_WRP_ADDRESS_MODE_SHORT = 0x02,
    MAC_WRP_ADDRESS_MODE_EXTENDED = 0x03,
} MAC_WRP_ADDRESS_MODE;

#pragma pack(push,2)

// *****************************************************************************
/* MAC Wrapper Address definition

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
        MAC_WRP_SHORT_ADDRESS shortAddress;
        MAC_WRP_EXTENDED_ADDRESS extendedAddress;
    };
    MAC_WRP_ADDRESS_MODE addressMode;
} MAC_WRP_ADDRESS;

// *****************************************************************************
/* MAC Wrapper Timestamp definition

   Summary:
    Defines the Timestamp as an unsigned 32-bit integer.

   Description:
    Creates an unsigned 32-bit integer specific type for Timestamp definition.

   Remarks:
    None.
*/
typedef uint32_t MAC_WRP_TIMESTAMP;

/* MAC Wrapper PAN Descriptor definition

   Summary:
    Defines the fields of a PAN Descriptor object.

   Description:
    This structure contains the fields which define a PAN Descriptor object,
    which contains information of the Beacon frames seen on a G3 network.

   Remarks:
    mediaType field is only relevant if both MAC layers are used,
    otherwise it is fixed to the available MAC.
*/
typedef struct
{
    uint16_t rcCoord;
    MAC_WRP_PAN_ID panId;
    MAC_WRP_SHORT_ADDRESS lbaAddress;
    uint8_t linkQuality;
    MAC_WRP_MEDIA_TYPE_INDICATION mediaType;
} MAC_WRP_PAN_DESCRIPTOR;

#pragma pack(pop)

// *****************************************************************************
/* MAC Wrapper Security Level definition

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
    MAC_WRP_SECURITY_LEVEL_NONE = 0x00,
    MAC_WRP_SECURITY_LEVEL_ENC_MIC_32 = 0x05,
} MAC_WRP_SECURITY_LEVEL;

// *****************************************************************************
/* MAC Wrapper Quality of Service values definition

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
    MAC_WRP_QUALITY_OF_SERVICE_NORMAL_PRIORITY = 0x00,
    MAC_WRP_QUALITY_OF_SERVICE_HIGH_PRIORITY = 0x01,
} MAC_WRP_QUALITY_OF_SERVICE;

// *****************************************************************************
/* MAC Wrapper TX Option values definition

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
    MAC_WRP_TX_OPTION_NO_ACK = 0x00,
    MAC_WRP_TX_OPTION_ACK = 0x01,
} MAC_WRP_TX_OPTIONS;

// *****************************************************************************
/* MAC Wrapper Status values definition

   Summary:
    Identifies the possible return values for the status field on MAC Wrapper
    Confirm and Indication callbacks.

   Description:
    Whenever a Confirm or Indication callback is generated from MAC Wrapper to
    indicate an event, a Status value is included with the event result.

   Remarks:
    None.
*/
typedef enum
{
    MAC_WRP_STATUS_SUCCESS = 0x00,
    MAC_WRP_STATUS_CHANNEL_ACCESS_FAILURE = 0xE1,
    MAC_WRP_STATUS_COUNTER_ERROR = 0xDB,
    MAC_WRP_STATUS_DENIED = 0xE2,
    MAC_WRP_STATUS_FRAME_TOO_LONG = 0xE5,
    MAC_WRP_STATUS_INVALID_HANDLE = 0xE7,
    MAC_WRP_STATUS_INVALID_INDEX = 0xF9,
    MAC_WRP_STATUS_INVALID_PARAMETER = 0xE8,
    MAC_WRP_STATUS_LIMIT_REACHED = 0xFA,
    MAC_WRP_STATUS_NO_ACK = 0xE9,
    MAC_WRP_STATUS_NO_SHORT_ADDRESS = 0xEC,
    MAC_WRP_STATUS_READ_ONLY = 0xFB,
    MAC_WRP_STATUS_SECURITY_ERROR = 0xE4,
    MAC_WRP_STATUS_TRANSACTION_OVERFLOW = 0xF1,
    MAC_WRP_STATUS_UNAVAILABLE_KEY = 0xF3,
    MAC_WRP_STATUS_UNSUPPORTED_ATTRIBUTE = 0xF4,
    MAC_WRP_STATUS_UNSUPPORTED_SECURITY = 0xDF,
    MAC_WRP_STATUS_ALTERNATE_PANID_DETECTION = 0x80,
    MAC_WRP_STATUS_QUEUE_FULL = 0xD0,
} MAC_WRP_STATUS;

// *****************************************************************************
/* MAC Wrapper Security Key definition

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
    uint8_t key[MAC_WRP_SECURITY_KEY_LENGTH];
    bool valid;
} MAC_WRP_SECURITY_KEY;

/* MISRA C-2012 deviation block start */
/* MISRA C-2012 Rule 5.2 deviated 41 times.  Deviation record ID - H3_MISRAC_2012_R_5_2_DR_1 */

// *****************************************************************************
/* MAC Wrapper Parameter Information Base definition

   Summary:
    Lists the available objects in the MAC Parameter Information Base (PIB).

   Description:
    MAC PIB is a collection of objects that can be read/written in order to
    retrieve information and/or configure the MAC layer.

   Remarks:
    None.
*/
typedef enum
{
    /*************************************************************************/
    /* PLC IBs definition */
    /*************************************************************************/
    MAC_WRP_PIB_MAX_BE = 0x00000047, /* 8 bits */
    MAC_WRP_PIB_BSN = 0x00000049, /* 8 bits */
    MAC_WRP_PIB_DSN = 0x0000004C, /* 8 bits */
    MAC_WRP_PIB_MAX_CSMA_BACKOFFS = 0x0000004E, /* 8 bits */
    MAC_WRP_PIB_MIN_BE = 0x0000004F, /* 8 bits */
    MAC_WRP_PIB_PAN_ID = 0x00000050, /* 16 bits */
    MAC_WRP_PIB_PROMISCUOUS_MODE = 0x00000051, /* 8 bits (bool) */
    MAC_WRP_PIB_SHORT_ADDRESS = 0x00000053, /* 16 bits */
    MAC_WRP_PIB_MAX_FRAME_RETRIES = 0x00000059, /* 8 bits */
    MAC_WRP_PIB_TIMESTAMP_SUPPORTED = 0x0000005C, /* 8 bits (bool) */
    MAC_WRP_PIB_SECURITY_ENABLED = 0x0000005D, /* 8 bits (bool) */
    MAC_WRP_PIB_KEY_TABLE = 0x00000071, /* 16 Byte entries */
    MAC_WRP_PIB_FRAME_COUNTER = 0x00000077, /* 32 bits */
    MAC_WRP_PIB_DUPLICATE_DETECTION_TTL = 0x00000078, /* 8 bits */
    MAC_WRP_PIB_HIGH_PRIORITY_WINDOW_SIZE = 0x00000100, /* 8 bits */
    MAC_WRP_PIB_TX_DATA_PACKET_COUNT = 0x00000101, /* 32 bits */
    MAC_WRP_PIB_RX_DATA_PACKET_COUNT = 0x00000102, /* 32 bits */
    MAC_WRP_PIB_TX_CMD_PACKET_COUNT = 0x00000103, /* 32 bits */
    MAC_WRP_PIB_RX_CMD_PACKET_COUNT = 0x00000104, /* 32 bits */
    MAC_WRP_PIB_CSMA_FAIL_COUNT = 0x00000105, /* 32 bits */
    MAC_WRP_PIB_CSMA_NO_ACK_COUNT = 0x00000106, /* 32 bits */
    MAC_WRP_PIB_RX_DATA_BROADCAST_COUNT = 0x00000107, /* 32 bits */
    MAC_WRP_PIB_TX_DATA_BROADCAST_COUNT = 0x00000108, /* 32 bits */
    MAC_WRP_PIB_BAD_CRC_COUNT = 0x00000109, /* 32 bits */
    MAC_WRP_PIB_NEIGHBOUR_TABLE = 0x0000010A, /* 16 Byte entries */
    MAC_WRP_PIB_FREQ_NOTCHING = 0x0000010B, /* 8 bits (bool) */
    MAC_WRP_PIB_CSMA_FAIRNESS_LIMIT = 0x0000010C, /* 8 bits */
    MAC_WRP_PIB_TMR_TTL = 0x0000010D, /* 8 bits */
    MAC_WRP_PIB_POS_TABLE_ENTRY_TTL = 0x0000010E, /* 8 bits */
    MAC_WRP_PIB_RC_COORD = 0x0000010F, /* 16 bits */
    MAC_WRP_PIB_TONE_MASK = 0x00000110, /* 9 Byte array */
    MAC_WRP_PIB_BEACON_RANDOMIZATION_WINDOW_LENGTH = 0x00000111, /* 8 bits */
    MAC_WRP_PIB_A = 0x00000112, /* 8 bits */
    MAC_WRP_PIB_K = 0x00000113, /* 8 bits */
    MAC_WRP_PIB_MIN_CW_ATTEMPTS = 0x00000114, /* 8 bits */
    MAC_WRP_PIB_CENELEC_LEGACY_MODE = 0x00000115, /* 8 bits */
    MAC_WRP_PIB_FCC_LEGACY_MODE = 0x00000116, /* 8 bits */
    MAC_WRP_PIB_BROADCAST_MAX_CW_ENABLE = 0x0000011E, /* 8 bits (bool) */
    MAC_WRP_PIB_TRANSMIT_ATTEN = 0x0000011F, /* 8 bits */
    MAC_WRP_PIB_POS_TABLE = 0x00000120, /* 5 Byte entries */
    MAC_WRP_PIB_POS_RECENT_ENTRY_THRESHOLD = 0x00000121, /* 8 bits */
    MAC_WRP_PIB_POS_RECENT_ENTRIES = 0x00000122, /* 16 bits */
    MAC_WRP_PIB_PLC_DISABLE = 0x00000123, /* 8 bits (bool) */
    /* manufacturer specific */
    /* provides access to device table. 6 Byte entries. */
    MAC_WRP_PIB_MANUF_DEVICE_TABLE = 0x08000000,
    /* Extended address of this node. 8 Byte array. */
    MAC_WRP_PIB_MANUF_EXTENDED_ADDRESS = 0x08000001,
    /* provides access to neighbour table by short address (transmitted as index) */
    /* 16 Byte entries */
    MAC_WRP_PIB_MANUF_NEIGHBOUR_TABLE_ELEMENT = 0x08000002,
    /* returns the maximum number of tones used by the band. 11 Byte struct. */
    MAC_WRP_PIB_MANUF_BAND_INFORMATION = 0x08000003,
    /* Short address of the coordinator. 16 bits. */
    MAC_WRP_PIB_MANUF_COORD_SHORT_ADDRESS = 0x08000004,
    /* Maximal payload supported by MAC. 16 bits. */
    MAC_WRP_PIB_MANUF_MAX_MAC_PAYLOAD_SIZE = 0x08000005,
    /* Resets the device table upon a GMK activation. 8 bits (bool). */
    MAC_WRP_PIB_MANUF_SECURITY_RESET = 0x08000006,
    /* Forces Modulation Scheme in every transmitted frame. 8 bits. */
    /* 0 - Not forced, 1 - Force Differential, 2 - Force Coherent */
    MAC_WRP_PIB_MANUF_FORCED_MOD_SCHEME = 0x08000007,
    /* Forces Modulation Type in every transmitted frame. 8 bits. */
    /* 0 - Not forced, 1 - Force BPSK_ROBO, 2 - Force BPSK, 3 - Force QPSK, 4 - Force 8PSK */
    MAC_WRP_PIB_MANUF_FORCED_MOD_TYPE = 0x08000008,
    /* Forces ToneMap in every transmitted frame. 3 Byte array. */
    /* {0} - Not forced, other value will be used as tonemap */
    MAC_WRP_PIB_MANUF_FORCED_TONEMAP = 0x08000009,
    /* Forces Modulation Scheme bit in Tone Map Response. 8 bits. */
    /* 0 - Not forced, 1 - Force Differential, 2 - Force Coherent */
    MAC_WRP_PIB_MANUF_FORCED_MOD_SCHEME_ON_TMRESPONSE = 0x0800000A,
    /* Forces Modulation Type bits in Tone Map Response. 8 bits. */
    /* 0 - Not forced, 1 - Force BPSK_ROBO, 2 - Force BPSK, 3 - Force QPSK, 4 - Force 8PSK */
    MAC_WRP_PIB_MANUF_FORCED_MOD_TYPE_ON_TMRESPONSE = 0x0800000B,
    /* Forces ToneMap field Tone Map Response. 3 Byte array. */
    /* {0} - Not forced, other value will be used as tonemap field */
    MAC_WRP_PIB_MANUF_FORCED_TONEMAP_ON_TMRESPONSE = 0x0800000C,
    /* Gets Modulation Scheme of last received frame. 8 bits. */
    MAC_WRP_PIB_MANUF_LAST_RX_MOD_SCHEME = 0x0800000D,
    /* Gets Modulation Scheme of last received frame. 8 bits. */
    MAC_WRP_PIB_MANUF_LAST_RX_MOD_TYPE = 0x0800000E,
    /* Indicates whether an LBP frame has been received. 8 bits (bool). */
    MAC_WRP_PIB_MANUF_LBP_FRAME_RECEIVED = 0x0800000F,
    /* Indicates whether an LNG frame has been received. 8 bits (bool). */
    MAC_WRP_PIB_MANUF_LNG_FRAME_RECEIVED = 0x08000010,
    /* Indicates whether an Beacon frame has been received. 8 bits (bool). */
    MAC_WRP_PIB_MANUF_BCN_FRAME_RECEIVED = 0x08000011,
    /* Gets number of valid elements in the Neighbour Table. 16 bits. */
    MAC_WRP_PIB_MANUF_NEIGHBOUR_TABLE_COUNT = 0x08000012,
    /* Gets number of discarded packets due to Other Destination. 32 bits. */
    MAC_WRP_PIB_MANUF_RX_OTHER_DESTINATION_COUNT = 0x08000013,
    /* Gets number of discarded packets due to Invalid Frame Length. 32 bits. */
    MAC_WRP_PIB_MANUF_RX_INVALID_FRAME_LENGTH_COUNT = 0x08000014,
    /* Gets number of discarded packets due to MAC Repetition. 32 bits. */
    MAC_WRP_PIB_MANUF_RX_MAC_REPETITION_COUNT = 0x08000015,
    /* Gets number of discarded packets due to Wrong Addressing Mode. 32 bits. */
    MAC_WRP_PIB_MANUF_RX_WRONG_ADDR_MODE_COUNT = 0x08000016,
    /* Gets number of discarded packets due to Unsupported Security. 32 bits. */
    MAC_WRP_PIB_MANUF_RX_UNSUPPORTED_SECURITY_COUNT = 0x08000017,
    /* Gets number of discarded packets due to Wrong Key Id. 32 bits. */
    MAC_WRP_PIB_MANUF_RX_WRONG_KEY_ID_COUNT = 0x08000018,
    /* Gets number of discarded packets due to Invalid Key. 32 bits. */
    MAC_WRP_PIB_MANUF_RX_INVALID_KEY_COUNT = 0x08000019,
    /* Gets number of discarded packets due to Wrong Frame Counter. 32 bits. */
    MAC_WRP_PIB_MANUF_RX_WRONG_FC_COUNT = 0x0800001A,
    /* Gets number of discarded packets due to Decryption Error. 32 bits. */
    MAC_WRP_PIB_MANUF_RX_DECRYPTION_ERROR_COUNT = 0x0800001B,
    /* Gets number of discarded packets due to Segment Decode Error. 32 bits. */
    MAC_WRP_PIB_MANUF_RX_SEGMENT_DECODE_ERROR_COUNT = 0x0800001C,
    /* Enables MAC Sniffer. 8 bits (bool). */
    MAC_WRP_PIB_MANUF_ENABLE_MAC_SNIFFER = 0x0800001D,
    /* Gets number of valid elements in the POS Table. Unused in SPEC-15. 16 bits. */
    MAC_WRP_PIB_MANUF_POS_TABLE_COUNT = 0x0800001E,
    /* Gets or Sets number of retires left before forcing ROBO mode. 8 bits. */
    MAC_WRP_PIB_MANUF_RETRIES_LEFT_TO_FORCE_ROBO = 0x0800001F,
    /* Gets internal MAC version. 6 Byte struct. */
    MAC_WRP_PIB_MANUF_MAC_INTERNAL_VERSION = 0x08000021,
    /* Gets internal MAC RT version. 6 Byte struct. */
    MAC_WRP_PIB_MANUF_MAC_RT_INTERNAL_VERSION = 0x08000022,
    /* Resets MAC statistics */
    MAC_WRP_PIB_MANUF_RESET_MAC_STATS = 0x08000023,
    /* Enable/Disable Sleep Mode */
    MAC_WRP_PIB_MANUF_SLEEP_MODE = 0x08000024,
    /* Set PLC in Debug Mode */
    MAC_WRP_PIB_MANUF_DEBUG_SET = 0x08000025,
    /* Read PLC debug information */
    MAC_WRP_PIB_MANUF_DEBUG_READ = 0x08000026,
    /* Provides access to POS table by short address (referenced as index). 5 Byte entries */
    MAC_WRP_PIB_MANUF_POS_TABLE_ELEMENT = 0x08000027,
    /* Minimum LQI to consider a neighbour for Trickle. 8 bits. */
    MAC_WRP_PIB_MANUF_TRICKLE_MIN_LQI = 0x08000028,
    /* LQI for a given neighbour, which short address will be indicated by index. 8 bits. */
    MAC_WRP_PIB_MANUF_NEIGHBOUR_LQI = 0x08000029,
    /* Best LQI found in neighbour table. 8 bits. */
    MAC_WRP_PIB_MANUF_BEST_LQI = 0x0800002A,
    /* PLC Interface availability. 8 bits (bool). */
    MAC_WRP_PIB_MANUF_PLC_IFACE_AVAILABLE = 0x0800002C,
    /* Last PLC frame duration in ms. 16 bits. */
    MAC_WRP_PIB_MANUF_LAST_FRAME_DURATION_PLC = 0x0800002D,
    /* Gets or sets a parameter in Phy layer. Index will be used to contain PHY parameter ID. */
    /* See definitions below */
    MAC_WRP_PIB_MANUF_PHY_PARAM = 0x08000020,
    /*************************************************************************/
    /* RF IBs definition */
    /*************************************************************************/
    MAC_WRP_PIB_DSN_RF = 0x00000200, /* 8 bits */
    MAC_WRP_PIB_MAX_BE_RF = 0x00000201, /* 8 bits */
    MAC_WRP_PIB_MAX_CSMA_BACKOFFS_RF = 0x00000202, /* 8 bits */
    MAC_WRP_PIB_MAX_FRAME_RETRIES_RF = 0x00000203, /* 8 bits */
    MAC_WRP_PIB_MIN_BE_RF = 0x00000204, /* 8 bits */
    MAC_WRP_PIB_TIMESTAMP_SUPPORTED_RF = 0x00000205, /* 8 bits (bool) */
    MAC_WRP_PIB_DEVICE_TABLE_RF = 0x00000206, /* 6 Byte entries. */
    MAC_WRP_PIB_FRAME_COUNTER_RF = 0x00000207, /* 32 bits */
    MAC_WRP_PIB_DUPLICATE_DETECTION_TTL_RF = 0x00000208, /* 8 bits */
    MAC_WRP_PIB_COUNTER_OCTETS_RF = 0x00000209, /* 8 bits */
    MAC_WRP_PIB_RETRY_COUNT_RF = 0x0000020A, /* 32 bits */
    MAC_WRP_PIB_MULTIPLE_RETRY_COUNT_RF = 0x0000020B, /* 32 bits */
    MAC_WRP_PIB_TX_FAIL_COUNT_RF = 0x0000020C, /* 32 bits */
    MAC_WRP_PIB_TX_SUCCESS_COUNT_RF = 0x0000020D, /* 32 bits */
    MAC_WRP_PIB_FCS_ERROR_COUNT_RF = 0x0000020E, /* 32 bits */
    MAC_WRP_PIB_SECURITY_FAILURE_COUNT_RF = 0x0000020F, /* 32 bits */
    MAC_WRP_PIB_DUPLICATE_FRAME_COUNT_RF = 0x00000210, /* 32 bits */
    MAC_WRP_PIB_RX_SUCCESS_COUNT_RF = 0x00000211, /* 32 bits */
    MAC_WRP_PIB_USE_ENHANCED_BEACON_RF = 0x00000213, /* 8 bits (bool) */
    MAC_WRP_PIB_EB_HEADER_IE_LIST_RF = 0x00000214, /* Array of 8 bit elements */
    MAC_WRP_PIB_EB_PAYLOAD_IE_LIST_RF = 0x00000215, /* Array of 8 bit elements */
    MAC_WRP_PIB_EB_FILTERING_ENABLED_RF = 0x00000216, /* 8 bits (bool) */
    MAC_WRP_PIB_EBSN_RF = 0x00000217, /* 8 bits */
    MAC_WRP_PIB_EB_AUTO_SA_RF = 0x00000218, /* 8 bits */
    MAC_WRP_PIB_SEC_SECURITY_LEVEL_LIST_RF = 0x0000021A, /* 4 Byte entries. */
    MAC_WRP_PIB_POS_TABLE_RF = 0x0000021D, /* 9 Byte entries. */
    MAC_WRP_PIB_OPERATING_MODE_RF = 0x0000021E, /* 8 bits */
    MAC_WRP_PIB_CHANNEL_NUMBER_RF = 0x0000021F, /* 16 bits */
    MAC_WRP_PIB_DUTY_CYCLE_USAGE_RF = 0x00000220, /* 8 bits */
    MAC_WRP_PIB_DUTY_CYCLE_PERIOD_RF = 0x00000221, /* 16 bits */
    MAC_WRP_PIB_DUTY_CYCLE_LIMIT_RF = 0x00000222, /* 16 bits */
    MAC_WRP_PIB_DUTY_CYCLE_THRESHOLD_RF = 0x00000223, /* 8 bits */
    MAC_WRP_PIB_DISABLE_PHY_RF = 0x00000224, /* 8 bits (bool) */
    MAC_WRP_PIB_FREQUENCY_BAND_RF = 0x00000225, /* 8 bits */
    MAC_WRP_PIB_TRANSMIT_ATTEN_RF = 0x00000226, /* 8 bits */
    MAC_WRP_PIB_ADAPTIVE_POWER_STEP_RF = 0x00000227, /* 8 bits */
    MAC_WRP_PIB_ADAPTIVE_POWER_HIGH_BOUND_RF = 0x00000228, /* 8 bits */
    MAC_WRP_PIB_ADAPTIVE_POWER_LOW_BOUND_RF = 0x00000229, /* 8 bits */
    MAC_WRP_PIB_POS_RECENT_ENTRIES_RF = 0x0000022A, /* 16 bits */
    /* Manufacturer specific */
    /* Resets the device table upon a GMK activation. 8 bits (bool) */
    MAC_WRP_PIB_MANUF_SECURITY_RESET_RF = 0x08000203,
    /* Indicates whether an LBP frame for other destination has been received. 8 bits (bool) */
    MAC_WRP_PIB_MANUF_LBP_FRAME_RECEIVED_RF = 0x08000204,
    /* Indicates whether an LBP frame for other destination has been received. 8 bits (bool) */
    MAC_WRP_PIB_MANUF_LNG_FRAME_RECEIVED_RF = 0x08000205,
    /* Indicates whether an Beacon frame from other nodes has been received. 8 bits (bool) */
    MAC_WRP_PIB_MANUF_BCN_FRAME_RECEIVED_RF = 0x08000206,
    /* Gets number of discarded packets due to Other Destination. 32 bits */
    MAC_WRP_PIB_MANUF_RX_OTHER_DESTINATION_COUNT_RF = 0x08000207,
    /* Gets number of discarded packets due to Invalid Frame Lenght. 32 bits */
    MAC_WRP_PIB_MANUF_RX_INVALID_FRAME_LENGTH_COUNT_RF = 0x08000208,
    /* Gets number of discarded packets due to Wrong Addressing Mode. 32 bits */
    MAC_WRP_PIB_MANUF_RX_WRONG_ADDR_MODE_COUNT_RF = 0x08000209,
    /* Gets number of discarded packets due to Unsupported Security. 32 bits */
    MAC_WRP_PIB_MANUF_RX_UNSUPPORTED_SECURITY_COUNT_RF = 0x0800020A,
    /* Gets number of discarded packets due to Wrong Key Id. 32 bits */
    MAC_WRP_PIB_MANUF_RX_WRONG_KEY_ID_COUNT_RF = 0x0800020B,
    /* Gets number of discarded packets due to Invalid Key. 32 bits */
    MAC_WRP_PIB_MANUF_RX_INVALID_KEY_COUNT_RF = 0x0800020C,
    /* Gets number of discarded packets due to Wrong Frame Counter. 32 bits */
    MAC_WRP_PIB_MANUF_RX_WRONG_FC_COUNT_RF = 0x0800020D,
    /* Gets number of discarded packets due to Decryption Error. 32 bits */
    MAC_WRP_PIB_MANUF_RX_DECRYPTION_ERROR_COUNT_RF = 0x0800020E,
    /* Gets number of transmitted Data packets. 32 bits */
    MAC_WRP_PIB_MANUF_TX_DATA_PACKET_COUNT_RF = 0x0800020F,
    /* Gets number of received Data packets. 32 bits */
    MAC_WRP_PIB_MANUF_RX_DATA_PACKET_COUNT_RF = 0x08000210,
    /* Gets number of transmitted Command packets. 32 bits */
    MAC_WRP_PIB_MANUF_TX_CMD_PACKET_COUNT_RF = 0x08000211,
    /* Gets number of received Command packets. 32 bits */
    MAC_WRP_PIB_MANUF_RX_CMD_PACKET_COUNT_RF = 0x08000212,
    /* Gets number of Channel Access failures. 32 bits */
    MAC_WRP_PIB_MANUF_CSMA_FAIL_COUNT_RF = 0x08000213,
    /* Gets number of received broadcast packets. 32 bits */
    MAC_WRP_PIB_MANUF_RX_DATA_BROADCAST_COUNT_RF = 0x08000214,
    /* Gets number of transmitted broadcast packets. 32 bits */
    MAC_WRP_PIB_MANUF_TX_DATA_BROADCAST_COUNT_RF = 0x08000215,
    /* Gets number of received packets with wrong CRC. 32 bits */
    MAC_WRP_PIB_MANUF_BAD_CRC_COUNT_RF = 0x08000216,
    /* Enables MAC Sniffer. 8 bits (bool) */
    MAC_WRP_PIB_MANUF_ENABLE_MAC_SNIFFER_RF = 0x08000217,
    /* Gets number of valid elements in the POS Table. 16 bits */
    MAC_WRP_PIB_MANUF_POS_TABLE_COUNT_RF = 0x08000218,
    /* Gets internal MAC version. 6 Byte struct. */
    MAC_WRP_PIB_MANUF_MAC_INTERNAL_VERSION_RF = 0x08000219,
    /* Resets MAC statistics. 8 bits (bool) */
    MAC_WRP_PIB_MANUF_RESET_MAC_STATS_RF = 0x0800021A,
    /* Provides access to POS table by short address (referenced as index). 9 Byte entries */
    MAC_WRP_PIB_MANUF_POS_TABLE_ELEMENT_RF = 0x0800021B,
    /* Configures time between a received frame and the transmission of its ACK. 32 bits */
    MAC_WRP_PIB_MANUF_ACK_TX_DELAY_RF = 0x0800021C,
    /* Configures time to wait for a requested ACK before timing out. 32 bits */
    MAC_WRP_PIB_MANUF_ACK_RX_WAIT_TIME_RF = 0x0800021D,
    /* Configures time to wait for an ACK Confirm before timing out. 32 bits */
    MAC_WRP_PIB_MANUF_ACK_CONFIRM_WAIT_TIME_RF = 0x0800021E,
    /* Configures time to wait for a Data Confirm before timing out. 32 bits */
    MAC_WRP_PIB_MANUF_DATA_CONFIRM_WAIT_TIME_RF = 0x0800021F,
    /* RF Interface availability. 8 bits (bool). */
    MAC_WRP_PIB_MANUF_RF_IFACE_AVAILABLE = 0x08000221,
    /* Last PLC frame duration in ms. 16 bits. */
    MAC_WRP_PIB_MANUF_LAST_FRAME_DURATION_RF = 0x08000222,
    /* Minimum LQI to consider a neighbour for Trickle. 8 bits. */
    MAC_WRP_PIB_MANUF_TRICKLE_MIN_LQI_RF = 0x08000223,
    /* Gets or sets a parameter in Phy layer. Index will be used to contain PHY parameter ID */
    MAC_WRP_PIB_MANUF_PHY_PARAM_RF = 0x08000220
} MAC_WRP_PIB_ATTRIBUTE;

// *****************************************************************************
/* MAC Wrapper PHY PLC Parameters definition

   Summary:
    Lists the available PHY PLC layer objects accessible through the
    MAC Information Base (PIB).

   Description:
    Some PHY PLC layer objects can be read/written by means of a specific
    MAC PIB [MAC_WRP_PIB_MANUF_PHY_PARAM].
    This enum lists the PHY PLC layer objects which are accessible through the
    MAC Information Base API.

   Remarks:
    None.
*/
typedef enum
{
    /* Phy layer version number. 32 bits. */
    MAC_WRP_PHY_PARAM_VERSION = 0x010c,
    /* Correctly transmitted frame count. 32 bits. */
    MAC_WRP_PHY_PARAM_TX_TOTAL = 0x0110,
    /* Transmitted bytes count. 32 bits. */
    MAC_WRP_PHY_PARAM_TX_TOTAL_BYTES = 0x0114,
    /* Transmission errors count. 32 bits. */
    MAC_WRP_PHY_PARAM_TX_TOTAL_ERRORS = 0x0118,
    /* Transmission failure due to already in transmission. 32 bits. */
    MAC_WRP_PHY_PARAM_BAD_BUSY_TX = 0x011C,
    /* Transmission failure due to busy channel. 32 bits. */
    MAC_WRP_PHY_PARAM_TX_BAD_BUSY_CHANNEL = 0x0120,
    /* Bad len in message (too short - too long). 32 bits. */
    MAC_WRP_PHY_PARAM_TX_BAD_LEN = 0x0124,
    /* Message to transmit in bad format. 32 bits. */
    MAC_WRP_PHY_PARAM_TX_BAD_FORMAT = 0x0128,
    /* Timeout error in transmission. 32 bits. */
    MAC_WRP_PHY_PARAM_TX_TIMEOUT = 0x012C,
    /* Received correctly messages count. 32 bits. */
    MAC_WRP_PHY_PARAM_RX_TOTAL = 0x0130,
    /* Received bytes count. 32 bits. */
    MAC_WRP_PHY_PARAM_RX_TOTAL_BYTES = 0x0134,
    /* Reception RS errors count. 32 bits. */
    MAC_WRP_PHY_PARAM_RX_RS_ERRORS = 0x0138,
    /* Reception Exceptions count. 32 bits. */
    MAC_WRP_PHY_PARAM_RX_EXCEPTIONS = 0x013C,
    /* Bad len in message (too short - too long). 32 bits. */
    MAC_WRP_PHY_PARAM_RX_BAD_LEN = 0x0140,
    /* Bad CRC in received FCH. 32 bits. */
    MAC_WRP_PHY_PARAM_RX_BAD_CRC_FCH = 0x0144,
    /* CRC correct but invalid protocol. 32 bits. */
    MAC_WRP_PHY_PARAM_RX_FALSE_POSITIVE = 0x0148,
    /* Received message in bad format. 32 bits. */
    MAC_WRP_PHY_PARAM_RX_BAD_FORMAT = 0x014C,
    /* Time between noise captures (in ms). 32 bits. */
    MAC_WRP_PHY_PARAM_TIME_BETWEEN_NOISE_CAPTURES = 0x0158,
    /* Auto detect impedance. 8 bits (bool). */
    MAC_WRP_PHY_PARAM_CFG_AUTODETECT_BRANCH = 0x0161,
    /* Manual impedance configuration. 8 bits. */
    MAC_WRP_PHY_PARAM_CFG_IMPEDANCE = 0x0162,
    /* Indicate if notch filter is active or not. 8 bits (bool). */
    MAC_WRP_PHY_PARAM_RRC_NOTCH_ACTIVE = 0x0163,
    /* Index of the notch filter. 8 bits. */
    MAC_WRP_PHY_PARAM_RRC_NOTCH_INDEX = 0x0164,
    /* Enable periodic noise autodetect and adaptation. 8 bits (bool). */
    MAC_WRP_PHY_PARAM_ENABLE_AUTO_NOISE_CAPTURE = 0x0166,
    /* Noise detection timer reload after a correct reception. 8 bits (bool). */
    MAC_WRP_PHY_PARAM_DELAY_NOISE_CAPTURE_AFTER_RX = 0x0167,
    /* Disable PLC Tx/Rx. 8 bits (bool). */
    MAC_WRP_PHY_PARAM_PLC_DISABLE = 0x016A,
    /* Indicate noise power in dBuV for the noisier carrier. 8 bits. */
    MAC_WRP_PHY_PARAM_NOISE_PEAK_POWER = 0x016B,
    /* LQI value of the last received message. 8 bits. */
    MAC_WRP_PHY_PARAM_LAST_MSG_LQI = 0x016C,
    /* RSSI value of the last received message. 16 bits. */
    MAC_WRP_PHY_PARAM_LAST_MSG_RSSI = 0x016D,
    /* Success transmission of ACK packets. 16 bits. */
    MAC_WRP_PHY_PARAM_ACK_TX_CFM = 0x016E,
    /* Duration in ms of the last received message. 16 bits. */
    MAC_WRP_PHY_PARAM_LAST_MSG_DURATION = 0x016F,
    /* Inform PHY layer about enabled modulations on TMR. 8 bits. */
    MAC_WRP_PHY_PARAM_TONE_MAP_RSP_ENABLED_MODS = 0x0174,
    /* Reset Phy Statistics */
    MAC_WRP_PHY_PARAM_RESET_PHY_STATS = 0x0176,
    /* Set number pf SyncP symbols in preamble */
    MAC_WRP_PHY_PARAM_PREAMBLE_NUM_SYNCP = 0x0177
} MAC_WRP_PHY_PARAM;

// *****************************************************************************
/* MAC Wrapper PHY RF Parameters definition

   Summary:
    Lists the available PHY RF layer objects accessible through the
    MAC Information Base (PIB).

   Description:
    Some PHY RF layer objects can be read/written by means of a specific
    MAC PIB [MAC_WRP_PIB_MANUF_PHY_PARAM_RF].
    This enum lists the PHY RF layer objects which are accessible through the
    MAC Information Base API.

   Remarks:
    None.
*/
typedef enum
{
    /* RF device identifier. 16 bits */
    MAC_WRP_RF_PHY_PARAM_DEVICE_ID = 0x0000,
    /* RF PHY layer firmware version number. 6 bytes (see "DRV_RF215_FW_VERSION") */
    MAC_WRP_RF_PHY_PARAM_FW_VERSION = 0x0001,
    /* RF device reset (write-only) */
    MAC_WRP_RF_PHY_PARAM_DEVICE_RESET = 0x0002,
    /* RF transceiver (RF09 or RF24) reset (write-only) */
    MAC_WRP_RF_PHY_PARAM_TRX_RESET = 0x0080,
    /* RF transceiver (RF09 or RF24) sleep (write-only) */
    MAC_WRP_RF_PHY_PARAM_TRX_SLEEP = 0x0081,
    /* RF PHY configuration (see "DRV_RF215_PHY_CFG_OBJ") */
    MAC_WRP_RF_PHY_PARAM_PHY_CONFIG = 0x0100,
    /* RF PHY band and operating mode. 16 bits (see "DRV_RF215_PHY_BAND_OPM") */
    MAC_WRP_RF_PHY_PARAM_PHY_BAND_OPERATING_MODE = 0x0101,
    /* RF channel number used for transmission and reception. 16 bits */
    MAC_WRP_RF_PHY_PARAM_PHY_CHANNEL_NUM = 0x0120,
    /* RF frequency in Hz used for transmission and reception. 32 bits (read-only) */
    MAC_WRP_RF_PHY_PARAM_PHY_CHANNEL_FREQ_HZ = 0x0121,
    /* Duration in us of Energy Detection for CCA. 16 bits */
    MAC_WRP_RF_PHY_PARAM_PHY_CCA_ED_DURATION_US = 0x0141,
    /* Threshold in dBm for CCA with Energy Detection. 16 bits */
    MAC_WRP_RF_PHY_PARAM_PHY_CCA_ED_THRESHOLD_DBM = 0x0142,
    /* Duration in symbols of Energy Detection for CCA. 8 bits */
    MAC_WRP_RF_PHY_PARAM_PHY_CCA_ED_DURATION_SYMBOLS = 0x0143,
    /* Threshold in dB above sensitivity for CCA with Energy Detection. 8 bits */
    MAC_WRP_RF_PHY_PARAM_PHY_CCA_ED_THRESHOLD_SENSITIVITY = 0x0144,
    /* Sensitivity in dBm (according to 802.15.4). 8 bits */
    MAC_WRP_RF_PHY_PARAM_PHY_SENSITIVITY = 0x0150,
    /* Maximum TX power in dBm. 8 bits */
    MAC_WRP_RF_PHY_PARAM_PHY_MAX_TX_POWER = 0x0151,
    /* Turnaround time in us (aTurnaroundTime in IEEE 802.15.4). 16 bits (read-only) */
    MAC_WRP_RF_PHY_PARAM_PHY_TURNAROUND_TIME = 0x0160,
    /* Number of payload symbols in last transmitted message. 16 bits */
    MAC_WRP_RF_PHY_PARAM_PHY_TX_PAY_SYMBOLS = 0x0180,
    /* Number of payload symbols in last received message. 16 bits */
    MAC_WRP_RF_PHY_PARAM_PHY_RX_PAY_SYMBOLS = 0x0181,
    /* Successfully transmitted messages count. 32 bits */
    MAC_WRP_RF_PHY_PARAM_PHY_TX_TOTAL = 0x01A0,
    /* Successfully transmitted bytes count. 32 bits */
    MAC_WRP_RF_PHY_PARAM_PHY_TX_TOTAL_BYTES = 0x01A1,
    /* Transmission errors count. 32 bits */
    MAC_WRP_RF_PHY_PARAM_PHY_TX_ERR_TOTAL = 0x01A2,
    /* Transmission errors count due to already in transmission. 32 bits */
    MAC_WRP_RF_PHY_PARAM_PHY_TX_ERR_BUSY_TX = 0x01A3,
    /* Transmission errors count due to already in reception. 32 bits */
    MAC_WRP_RF_PHY_PARAM_PHY_TX_ERR_BUSY_RX = 0x01A4,
    /* Transmission errors count due to busy channel. 32 bits */
    MAC_WRP_RF_PHY_PARAM_PHY_TX_ERR_BUSY_CHN = 0x01A5,
    /* Transmission errors count due to bad message length. 32 bits */
    MAC_WRP_RF_PHY_PARAM_PHY_TX_ERR_BAD_LEN = 0x01A6,
    /* Transmission errors count due to bad format. 32 bits */
    MAC_WRP_RF_PHY_PARAM_PHY_TX_ERR_BAD_FORMAT = 0x01A7,
    /* Transmission errors count due to timeout. 32 bits */
    MAC_WRP_RF_PHY_PARAM_PHY_TX_ERR_TIMEOUT = 0x01A8,
    /* Transmission aborted count. 32 bits */
    MAC_WRP_RF_PHY_PARAM_PHY_TX_ERR_ABORTED = 0x01A9,
    /* Transmission confirms not handled by upper layer count. 32 bits */
    MAC_WRP_RF_PHY_PARAM_PHY_TX_CFM_NOT_HANDLED = 0x01AA,
    /* Successfully received messages count. 32 bits */
    MAC_WRP_RF_PHY_PARAM_PHY_RX_TOTAL = 0x01B0,
    /* Successfully received bytes count. 32 bits */
    MAC_WRP_RF_PHY_PARAM_PHY_RX_TOTAL_BYTES = 0x01B1,
    /* Reception errors count. 32 bits */
    MAC_WRP_RF_PHY_PARAM_PHY_RX_ERR_TOTAL = 0x01B2,
    /* Reception false positive count. 32 bits */
    MAC_WRP_RF_PHY_PARAM_PHY_RX_ERR_FALSE_POSITIVE = 0x01B3,
    /* Reception errors count due to bad message length. 32 bits */
    MAC_WRP_RF_PHY_PARAM_PHY_RX_ERR_BAD_LEN = 0x01B4,
    /* Reception errors count due to bad format or bad FCS in header. 32 bits */
    MAC_WRP_RF_PHY_PARAM_PHY_RX_ERR_BAD_FORMAT = 0x01B5,
    /* Reception errors count due to bad FCS in payload. 32 bits */
    MAC_WRP_RF_PHY_PARAM_PHY_RX_ERR_BAD_FCS_PAY = 0x01B6,
    /* Reception aborted count. 32 bits */
    MAC_WRP_RF_PHY_PARAM_PHY_RX_ERR_ABORTED = 0x01B7,
    /* Reception overrided (another message with higher signal level) count. 32 bits */
    MAC_WRP_RF_PHY_PARAM_PHY_RX_OVERRIDE = 0x01B8,
    /* Reception indications not handled by upper layer count. 32 bits */
    MAC_WRP_RF_PHY_PARAM_PHY_RX_IND_NOT_HANDLED = 0x01B9,
    /* Reset Phy Statistics (write-only) */
    MAC_WRP_RF_PHY_PARAM_PHY_STATS_RESET = 0x01C0,
    /* Set Continuous Tx Mode (write-only) */
    MAC_WRP_RF_PHY_PARAM_SET_CONTINUOUS_TX_MODE = 0x01C1,
    /* Backoff period unit in us (aUnitBackoffPeriod in IEEE 802.15.4) used for CSMA-CA . 16 bits (read-only) */
    MAC_WRP_RF_PHY_PARAM_MAC_UNIT_BACKOFF_PERIOD = 0x0200,
    /* SUN FSK FEC enabled or disabled for transmission (phyFskFecEnabled in IEEE 802.15.4). 8 bits */
    MAC_WRP_RF_PHY_PARAM_TX_FSK_FEC = 0x8000,
    /* SUN OFDM MCS (Modulation and coding scheme) used for transmission. 8 bits */
    MAC_WRP_RF_PHY_PARAM_TX_OFDM_MCS = 0x8001,
} MAC_WRP_PHY_PARAM_RF;

/* MISRA C-2012 deviation block end */

// *****************************************************************************
/* MAC Wrapper PIB Value definition

   Summary:
    Defines the fields of a PIB Value object.

   Description:
    This structure contains the fields which define a PIB Value object,
    which contains information of its length and the value itself coded into an
    8-bit array format.

   Remarks:
    None.
*/
typedef struct
{
    uint8_t length;
    uint8_t value[MAC_WRP_PIB_MAX_VALUE_LENGTH];
} MAC_WRP_PIB_VALUE;

// *****************************************************************************
/* MAC Wrapper Tx Coefficient definition

   Summary:
    Defines the attenuation to apply on each of the subbands
    present in the PLC work band.

   Description:
    This structure is part of the Neighbour Table as defined in the G3-PLC
    standard. It allows applying different attenuation values to different
    subbands present in the PLC work band.

   Remarks:
    None.
*/
typedef struct
{
    uint8_t txCoef[6];
} MAC_WRP_TX_COEF;

/* MISRA C-2012 deviation block start */
/* MISRA C-2012 Rule 6.1 deviated 5 times.  Deviation record ID - H3_MISRAC_2012_R_6_1_DR_1 */

// *****************************************************************************
/* MAC Wrapper PLC Neighbour Table Entry definition

   Summary:
    The MAC PLC Neighbour Table Entry as defined in the G3-PLC standard.

   Description:
    This structure contains the fields that define a MAC PLC Neighbour table
    entry as defined in the G3-PLC standard.

   Remarks:
    None.
*/
typedef struct __attribute__((packed))
{
    MAC_WRP_SHORT_ADDRESS shortAddress;
    MAC_WRP_TONE_MAP toneMap;
    uint8_t modulationType : 3;
    uint8_t txGain : 4;
    uint8_t txRes : 1;
    MAC_WRP_TX_COEF txCoef;
    uint8_t modulationScheme : 1;
    uint8_t phaseDifferential : 3;
    uint8_t lqi;
    uint16_t tmrValidTime;
} MAC_WRP_NEIGHBOUR_ENTRY;

/* MISRA C-2012 deviation block end */

// *****************************************************************************
/* MAC Wrapper PLC POS Table Entry definition

   Summary:
    The MAC PLC POS Table Entry as defined in the G3-PLC standard.

   Description:
    This structure contains the fields that define a MAC PLC POS table
    entry as defined in the G3-PLC standard.

   Remarks:
    None.
*/
typedef struct __attribute__((packed))
{
    MAC_WRP_SHORT_ADDRESS shortAddress;
    uint8_t lqi;
    uint16_t posValidTime;
} MAC_WRP_POS_ENTRY;

// *****************************************************************************
/* MAC Wrapper RF POS Table Entry definition

   Summary:
    The MAC RF POS Table Entry as defined in the G3-PLC standard.

   Description:
    This structure contains the fields that define a MAC RF POS table
    entry as defined in the G3-PLC standard.

   Remarks:
    None.
*/
typedef struct __attribute__((packed))
{
    MAC_WRP_SHORT_ADDRESS shortAddress;
    uint8_t forwardLqi;
    uint8_t reverseLqi;
    uint8_t dutyCycle;
    uint8_t forwardTxPowerOffset;
    uint8_t reverseTxPowerOffset;
    uint16_t posValidTime;
    uint16_t reverseLqiValidTime;
} MAC_WRP_POS_ENTRY_RF;

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // #ifndef MAC_WRAPPER_DEFS_H

/*******************************************************************************
 End of File
*/
