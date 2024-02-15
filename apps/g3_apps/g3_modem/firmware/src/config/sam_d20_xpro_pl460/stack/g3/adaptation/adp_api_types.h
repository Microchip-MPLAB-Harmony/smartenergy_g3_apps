/*******************************************************************************
  ADP API Types Header File

  Company:
    Microchip Technology Inc.

  File Name:
    adp_api_types.h

  Summary:
    ADP API Types Header File

  Description:
    The ADP provides a simple interface to manage the G3 Adaptation Layer. This
    file provides API types definition for the ADP.
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

#ifndef ADP_API_TYPES_H
#define ADP_API_TYPES_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************
#include <stdint.h>
#include <stdbool.h>
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
/* ADP 16-bit Address Definition

   Summary:
    Defines the size in bytes of a 16-bit address.

   Description:
    This macro defines the size in bytes of a 16-bit address (short address).

   Remarks:
    None.
*/
#define ADP_ADDRESS_16BITS 2U

// *****************************************************************************
/* ADP 64-bit Address Definition

   Summary:
    Defines the size in bytes of a 64-bit address.

   Description:
    This macro defines the size in bytes of a 64-bit address (extended address).

   Remarks:
    None.
*/
#define ADP_ADDRESS_64BITS 8U

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

#pragma pack(push,2)

// *****************************************************************************
/* ADP Modulation Definition

   Summary:
    Identifies the available modulation values for PLC communication.

   Description:
    This enumeration identifies the possible modulations used when transmitting
    and receiving frames over PLC medium.

   Remarks:
    It is only used when PLC MAC layer is present.
*/
typedef enum
{
    MOD_ROBO,
    MOD_BPSK,
    MOD_DBPSK,
    MOD_QPSK,
    MOD_DQPSK,
    MOD_8PSK,
    MOD_D8PSK,
    MOD_16QAM,
    MOD_UNKNOWN = 255

} ADP_MODULATION_PLC;

// *****************************************************************************
/* ADP Extended Address Definition

   Summary:
    Defines the structure of an extended address.

   Description:
    This data type defines the structure of an extended address.

   Remarks:
    None.
*/
typedef struct
{
    uint8_t value[8];

} ADP_EXTENDED_ADDRESS;

// *****************************************************************************
/* ADP Address Definition

   Summary:
    Defines the structure of an ADP address.

   Description:
    This data type defines the structure of an ADP address. It can be short
    address (16 bits) or extended address (64 bits).

   Remarks:
    None.
*/
typedef struct
{
    union
    {
        /* Short address */
        uint16_t shortAddr;

        /* Extended address */
        ADP_EXTENDED_ADDRESS extendedAddr;
    };

    /* Address size (ADP_ADDRESS_16BITS or ADP_ADDRESS_64BITS) */
    uint8_t addrSize;

} ADP_ADDRESS;

// *****************************************************************************
/* ADP Bands Definition

   Summary:
    Identifies the possible PLC bands, or if No PLC is used.

   Description:
    This enumeration identifies the list of Bands that can be used in PLC
    lower layers, it is set upon ADP_Open and propagated to lower layers.

   Remarks:
    None.
*/
typedef enum {
    ADP_BAND_CENELEC_A = 0,
    ADP_BAND_CENELEC_B = 1,
    ADP_BAND_FCC = 2,
    ADP_BAND_ARIB = 3,
    ADP_BAND_RF_ONLY = 255

} ADP_BAND;

// *****************************************************************************
/* ADP Available MAC layers

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
    ADP_AVAILABLE_MAC_PLC = 0x00,
    ADP_AVAILABLE_MAC_RF = 0x01,
    ADP_AVAILABLE_MAC_BOTH = 0x02

} ADP_AVAILABLE_MAC_LAYERS;

// *****************************************************************************
/* ADP PAN Descriptior Definition

   Summary:
    Defines the structure of a PAN Descriptor.

   Description:
    This data type defines the structure of a PAN Descriptor. It used in ADP
    Discovery Indication to store the parameters of the discovered PAN.

   Remarks:
    None.
*/
typedef struct
{
    /* The 16-bit PAN identifier */
    uint16_t panId;

    /* The 16 bit short address of a device in this PAN to be used as the LBA
     * by the associating device */
    uint16_t lbaAddress;

    /* The estimated route cost from LBA to the coordinator */
    uint16_t rcCoord;

    /* The 8-bit link quality of LBA */
    uint8_t linkQuality;

    /* The media type (PLC or RF) used to communicate with LBA */
    uint8_t mediaType;

} ADP_PAN_DESCRIPTOR;

// *****************************************************************************
/* ADP Hop Descriptior Definition

   Summary:
    Defines the structure of a Hop Descriptor.

   Description:
    This data type defines the structure of a Hop Descriptor. It used to
    describe a hop in a route.

   Remarks:
    None.
*/
typedef struct
{
    /* The hop / node address */
    uint16_t hopAddress;

    /* MetricNotSupported: 1 (the metric type is not supported by the hop) or 0
     * (if supported) */
    uint8_t mns;

    /* LinkCost of the hop */
    uint8_t linkCost;

    /* Phase Differential on link */
    uint8_t phaseDiff;

    /* Medium from which request is received */
    uint8_t mrx;

    /* Medium to which request is transmitted */
    uint8_t mtx;

    /* Reserved Bits. Read from incoming frame to propagate them correctly. */
    uint8_t rsvBits;

} ADP_HOP_DESCRIPTOR;

// *****************************************************************************
/* ADP Path Descriptior Definition

   Summary:
    Defines the structure of a Path Descriptor.

   Description:
    This data type defines the structure of a Path Descriptor. It defines all
    the hops needed to reach from originator to destination.

   Remarks:
    None.
*/
typedef struct
{
    /* Table with the information of each hop in forward direction (according to
     * forwardHopsCount) */
    ADP_HOP_DESCRIPTOR forwardPath[16];

    /* Table with the information of each hop in reverse direction (according to
     * reverseHopsCount) */
    ADP_HOP_DESCRIPTOR reversePath[16];

    /* The short unicast destination address of the path discovery */
    uint16_t dstAddr;

    /* The expected originator of the path reply */
    uint16_t expectedOrigAddr;

    /* The real originator of the path reply */
    uint16_t origAddr;

    /* Reserved Bits. Read from incoming frame to propagate them correctly. */
    uint16_t rsvBits;

    /* Path metric type */
    uint8_t metricType;

    /* Number of path hops in the forward table */
    uint8_t forwardHopsCount;

    /* Number of path hops in the reverse table */
    uint8_t reverseHopsCount;

} ADP_PATH_DESCRIPTOR;

#pragma pack(pop)

/* MISRA C-2012 deviation block start */
/* MISRA C-2012 Rule 5.2 deviated 4 times.  Deviation record ID - H3_MISRAC_2012_R_5_2_DR_1 */

// *****************************************************************************
/* ADP PIB Attributes Definition

   Summary:
    Identifies the available ADP PIB attributes.

   Description:
    This enumeration identifies the list of available parameter information base
    (PIB) attributes for the ADP.

   Remarks:
    None.
*/
typedef enum
{
    ADP_IB_SECURITY_LEVEL = 0x00000000, /* 8 bits */
    ADP_IB_PREFIX_TABLE = 0x00000001, /* [11 to 27] Byte entries */
    ADP_IB_BROADCAST_LOG_TABLE_ENTRY_TTL = 0x00000002, /* 16 bits */
    ADP_IB_METRIC_TYPE = 0x00000003, /* 8 bits */
    ADP_IB_LOW_LQI_VALUE = 0x00000004, /* 8 bits */
    ADP_IB_HIGH_LQI_VALUE = 0x00000005, /* 8 bits */
    ADP_IB_RREP_WAIT = 0x00000006, /* 8 bits */
    ADP_IB_CONTEXT_INFORMATION_TABLE = 0x00000007, /* [4 to 20] Byte entries */
    ADP_IB_COORD_SHORT_ADDRESS = 0x00000008, /* 16 bits */
    ADP_IB_RLC_ENABLED = 0x00000009, /* 8 bits (bool) */
    ADP_IB_ADD_REV_LINK_COST = 0x0000000A, /* 8 bits */
    ADP_IB_BROADCAST_LOG_TABLE = 0x0000000B, /* 5 Byte entries */
    ADP_IB_ROUTING_TABLE = 0x0000000C, /* 9 Byte entries */
    ADP_IB_UNICAST_RREQ_GEN_ENABLE = 0x0000000D, /* 8 bits (bool) */
    ADP_IB_GROUP_TABLE = 0x0000000E, /* 2 Byte entries */
    ADP_IB_MAX_HOPS = 0x0000000F, /* 8 bits */
    ADP_IB_DEVICE_TYPE = 0x00000010, /* 8 bits */
    ADP_IB_NET_TRAVERSAL_TIME = 0x00000011, /* 8 bits */
    ADP_IB_ROUTING_TABLE_ENTRY_TTL = 0x00000012, /* 16 bits */
    ADP_IB_KR = 0x00000013, /* 8 bits */
    ADP_IB_KM = 0x00000014, /* 8 bits */
    ADP_IB_KC = 0x00000015, /* 8 bits */
    ADP_IB_KQ = 0x00000016, /* 8 bits */
    ADP_IB_KH = 0x00000017, /* 8 bits */
    ADP_IB_RREQ_RETRIES = 0x00000018, /* 8 bits */
    ADP_IB_RREQ_WAIT = 0x00000019, /* 8 bits */
    ADP_IB_WEAK_LQI_VALUE = 0x0000001A, /* 8 bits */
    ADP_IB_KRT = 0x0000001B, /* 8 bits */
    ADP_IB_SOFT_VERSION = 0x0000001C, /* 6 Byte array */
    ADP_IB_SNIFFER_MODE = 0x0000001D, /* 8 bits (bool) */
    ADP_IB_BLACKLIST_TABLE = 0x0000001E, /* 4 Byte entries */
    ADP_IB_BLACKLIST_TABLE_ENTRY_TTL = 0x0000001F, /* 16 bits */
    ADP_IB_MAX_JOIN_WAIT_TIME = 0x00000020, /* 16 bits */
    ADP_IB_PATH_DISCOVERY_TIME = 0x00000021, /* 8 bits */
    ADP_IB_ACTIVE_KEY_INDEX = 0x00000022, /* 8 bits */
    ADP_IB_DESTINATION_ADDRESS_SET = 0x00000023, /* 2 Byte entries */
    ADP_IB_DEFAULT_COORD_ROUTE_ENABLED = 0x00000024, /* 8 bits (bool) */
    ADP_IB_DELAY_LOW_LQI = 0x00000025, /* 16 bits */
    ADP_IB_DELAY_HIGH_LQI = 0x00000026, /* 16 bits */
    ADP_IB_RREQ_JITTER_LOW_LQI = 0x00000027, /* 8 bits */
    ADP_IB_RREQ_JITTER_HIGH_LQI = 0x00000028, /* 8 bits */
    ADP_IB_TRICKLE_DATA_ENABLED = 0x00000029, /* 8 bits */
    ADP_IB_TRICKLE_LQI_THRESHOLD_LOW = 0x0000002A, /* 8 bits */
    ADP_IB_TRICKLE_STEP = 0x0000002B, /* 8 bits */
    ADP_IB_TRICKLE_I_MIN = 0x0000002D, /* 16 bits */
    ADP_IB_TRICKLE_MAX_KI = 0x0000002E, /* 8 bits */
    ADP_IB_TRICKLE_ADAPTIVE_I_MIN = 0x0000002F, /* 8 bits */
    ADP_IB_TRICKLE_ADAPTIVE_KI = 0x00000030, /* 8 bits */
    ADP_IB_CLUSTER_TRICKLE_ENABLED = 0x00000031, /* 8 bits */
    ADP_IB_CLUSTER_MIN_LQI = 0x00000032, /* 8 bits */
    ADP_IB_CLUSTER_TRICKLE_K = 0x00000033, /* 8 bits */
    ADP_IB_CLUSTER_RREQ_ROUTE_COST_DEVIATION = 0x00000034, /* 8 bits */
    ADP_IB_CLUSTER_TRICKLE_I = 0x00000035, /* 16 bits */
    ADP_IB_TRICKLE_LQI_THRESHOLD_HIGH = 0x00000036, /* 8 bits */
    ADP_IB_LOW_LQI_VALUE_RF = 0x000000D0, /* 8 bits */
    ADP_IB_HIGH_LQI_VALUE_RF = 0x000000D1, /* 8 bits */
    ADP_IB_KQ_RF = 0x000000D2, /* 8 bits */
    ADP_IB_KH_RF = 0x000000D3, /* 8 bits */
    ADP_IB_KRT_RF = 0x000000D4, /* 8 bits */
    ADP_IB_KDC_RF = 0x000000D5, /* 8 bits */
    ADP_IB_USE_BACKUP_MEDIA = 0x000000D6, /* 8 bits (bool) */
    ADP_IB_WEAK_LQI_VALUE_RF = 0x000000D7, /* 8 bits */
    ADP_IB_TRICKLE_LQI_THRESHOLD_LOW_RF = 0x000000D8, /* 8 bits */
    ADP_IB_DELAY_LOW_LQI_RF = 0x000000D9, /* 16 bits */
    ADP_IB_DELAY_HIGH_LQI_RF = 0x000000DA, /* 16 bits */
    ADP_IB_RREQ_JITTER_LOW_LQI_RF = 0x000000DB, /* 8 bits */
    ADP_IB_RREQ_JITTER_HIGH_LQI_RF = 0x000000DC, /* 8 bits */
    ADP_IB_CLUSTER_TRICKLE_I_RF = 0x000000DD, /* 16 bits */
    ADP_IB_CLUSTER_TRICKLE_K_RF = 0x000000DE, /* 8 bits */
    ADP_IB_CLUSTER_MIN_LQI_RF = 0x000000DF, /* 8 bits */
    ADP_IB_LAST_GASP = 0x000000E0, /* 8 bits (bool) */
    ADP_IB_PROBING_INTERVAL = 0x000000E1, /* 8 bits */
    ADP_IB_TRICKLE_LQI_THRESHOLD_HIGH_RF = 0x000000E2, /* 8 bits */
    ADP_IB_DISABLE_DEFAULT_ROUTING = 0x000000F0, /* 8 bits (bool) */

    /* Manufacturer */
    ADP_IB_MANUF_REASSEMBY_TIMER = 0x080000C0, /* 16 bits */
    ADP_IB_MANUF_IPV6_HEADER_COMPRESSION = 0x080000C1, /* 8 bits (bool) */
    ADP_IB_MANUF_PAN_ID = 0x080000C3, /* 16 bits */
    ADP_IB_MANUF_BROADCAST_SEQUENCE_NUMBER = 0x080000C4, /* 8 bits */
    ADP_IB_MANUF_DATAGRAM_TAG = 0x080000C6, /* 16 bits */
    ADP_IB_MANUF_ROUTING_TABLE_COUNT = 0x080000C8, /* 32 bits */
    ADP_IB_MANUF_DISCOVER_SEQUENCE_NUMBER = 0x080000C9, /* 16 bits */
    ADP_IB_MANUF_FORCED_NO_ACK_REQUEST = 0x080000CA, /* 8 bits (bool) */
    ADP_IB_MANUF_LQI_TO_COORD = 0x080000CB, /* 8 bits */
    ADP_IB_MANUF_BROADCAST_ROUTE_ALL = 0x080000CC, /* 8 bits (bool) */
    ADP_IB_MANUF_ADP_INTERNAL_VERSION = 0x080000CE, /* 6 Byte array */
    ADP_IB_MANUF_CIRCULAR_ROUTES_DETECTED = 0x080000CF, /* 16 bits */
    ADP_IB_MANUF_LAST_CIRCULAR_ROUTE_ADDRESS = 0x080000D0, /* 16 bits */
    ADP_IB_MANUF_IPV6_ULA_DEST_SHORT_ADDRESS = 0x080000D1, /* 16 bits */
    ADP_IB_MANUF_MAX_REPAIR_RESEND_ATTEMPTS = 0x080000D2, /* 8 bits */
    ADP_IB_MANUF_DISABLE_AUTO_RREQ = 0x080000D3, /* 8 bits (bool) */
    ADP_IB_MANUF_SHORT_ADDRESS = 0x080000D4, /* 16 bits */
    ADP_IB_MANUF_ALL_NEIGHBORS_BLACKLISTED_COUNT = 0x080000D5, /* 16 bits */
    ADP_IB_MANUF_QUEUED_ENTRIES_REMOVED_TIMEOUT_COUNT = 0x080000D6, /* 16 bits */
    ADP_IB_MANUF_QUEUED_ENTRIES_REMOVED_ROUTE_ERROR_COUNT = 0x080000D7, /* 16 bits */
    ADP_IB_MANUF_PENDING_DATA_IND_SHORT_ADDRESS = 0x080000D8, /* 16 bits */
    ADP_IB_MANUF_GET_BAND_CONTEXT_TONES = 0x080000D9, /* 8 bits */
    ADP_IB_MANUF_UPDATE_NON_VOLATILE_DATA = 0x080000DA, /* 8 bits (bool) */
    ADP_IB_MANUF_DISCOVER_ROUTE_GLOBAL_SEQ_NUM = 0x080000C9, /* 16 bits */
    ADP_IB_MANUF_FRAGMENT_DELAY = 0x080000DC, /* 16 bits */
    ADP_IB_MANUF_DYNAMIC_FRAGMENT_DELAY_ENABLED = 0x080000DD, /* 8 bits (bool) */
    ADP_IB_MANUF_DYNAMIC_FRAGMENT_DELAY_FACTOR = 0x080000DE, /* 16 bits */
    ADP_IB_MANUF_BLACKLIST_TABLE_COUNT = 0x080000DF, /* 16 bits */
    ADP_IB_MANUF_BROADCAST_LOG_TABLE_COUNT = 0x080000E0, /* 16 bits */
    ADP_IB_MANUF_CONTEXT_INFORMATION_TABLE_COUNT = 0x080000E1, /* 16 bits */
    ADP_IB_MANUF_GROUP_TABLE_COUNT = 0x080000E2, /* 16 bits */
    ADP_IB_MANUF_ROUTING_TABLE_ELEMENT = 0x080000E3,  /* 9 Byte entries */
    ADP_IB_MANUF_HYBRID_PROFILE = 0x080000E5,  /* 8 bits (bool) */
    ADP_IB_MANUF_LAST_PHASEDIFF = 0x080000E6  /* 8 bits */

} ADP_PIB_ATTRIBUTE;

/* MISRA C-2012 deviation block end */

// *****************************************************************************
/* ADP Result Definition

   Summary:
    Identifies the list of errors returned by the ADP.

   Description:
    This enumeration identifies the list of errors returned by the ADP.

   Remarks:
    None.
*/
typedef enum
{
    /* Success */
    G3_SUCCESS = 0x00,

    /* Invalid request */
    G3_INVALID_REQUEST = 0xA1,

    /* Request failed */
    G3_FAILED = 0xA2,

    /* Invalid IPv6 frame */
    G3_INVALID_IPV6_FRAME = 0xA3,

    /* Not permited */
    G3_NOT_PERMITED = 0xA4,

    /* No route to destination */
    G3_ROUTE_ERROR = 0xA5,

    /* Operation timed out */
    G3_TIMEOUT = 0xA6,

    /* An attempt to write to a PIB attribute that is in a table failed
     * because the specified table index was out of range */
    G3_INVALID_INDEX = 0xA7,

    /* A parameter in the primitive is either not supported or is out of
     * the valid range */
    G3_INVALID_PARAMETER = 0xA8,

    /* A scan operation failed to find any network beacons */
    G3_NO_BEACON = 0xA9,

    /* A Set Request was issued with the identifier of an attribute that is read
     * only */
    G3_READ_ONLY = 0xB0,

    /* A Set/Get Request was issued with the identifier of a PIB attribute that
     * is not supported */
    G3_UNSUPPORTED_ATTRIBUTE = 0xB1,

    /* The path discovery has only a part of the path to its desired final
     * destination */
    G3_INCOMPLETE_PATH = 0xB2,

    /* Busy: operation already in progress */
    G3_BUSY = 0xB3,

    /* Not enough resources */
    G3_NO_BUFFERS = 0xB4,

    /* Trickle redundancy detected */
    G3_TRICKLE_REDUNDANT = 0xB5,

    /* Error internal */
    G3_ERROR_INTERNAL = 0xFF

} ADP_RESULT;

// *****************************************************************************
/* ADP Status Definition

   Summary:
    Identifies the current status/state of the ADP.

   Description:
    This enumeration identifies the current status/state of the ADP.

   Remarks:
    None.
*/
typedef enum
{
    ADP_STATUS_UNINITIALIZED = SYS_STATUS_UNINITIALIZED,
    ADP_STATUS_BUSY = SYS_STATUS_BUSY,
    ADP_STATUS_READY = SYS_STATUS_READY,
    ADP_STATUS_LBP_CONNECTED = SYS_STATUS_READY_EXTENDED + 1,
    ADP_STATUS_ERROR = SYS_STATUS_ERROR,

} ADP_STATUS;

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // #ifndef ADP_API_TYPES_H
