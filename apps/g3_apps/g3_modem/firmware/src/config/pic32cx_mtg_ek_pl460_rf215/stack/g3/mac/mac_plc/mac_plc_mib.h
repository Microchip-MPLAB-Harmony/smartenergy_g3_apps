/*******************************************************************************
  G3 MAC PLC MIB Header File

  Company:
    Microchip Technology Inc.

  File Name:
    mac_plc_mib.h

  Summary:
    G3 MAC PLC MIB API Header File

  Description:
    This file contains definitions of the MAC PLC Information Base (IB).
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

#ifndef MAC_PLC_MIB_H
#define MAC_PLC_MIB_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include "mac_plc_defs.h"

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

#pragma pack(push,2)

// *****************************************************************************
/* MAC PLC Tables Structure

   Summary:
    Defines MAC PLC Tables and Sizes.

   Description:
    This structure contains pointers to MAC PLC Tables (hosted outside the
    MAC PLC library) and their sizes.

   Remarks:
    None.
*/
typedef struct
{
    // Pointers
    MAC_DEVICE_TABLE_ENTRY *macPlcDeviceTable;
    // Sizes
    uint16_t macPlcDeviceTableSize;
} MAC_PLC_TABLES;

// *****************************************************************************
/* MAC PLC MIB definition

   Summary:
    Defines the MAC PLC Information Base.

   Description:
    This structure holds the MAC PLC IB, it contains one field per each
    defined PIB.

   Remarks:
    None.
*/
typedef struct
{
    MAC_DEVICE_TABLE_ENTRY *deviceTable;
    uint32_t txDataPacketCount;
    uint32_t rxDataPacketCount;
    uint32_t txCmdPacketCount;
    uint32_t rxCmdPacketCount;
    uint32_t csmaFailCount;
    uint32_t rxDataBroadcastCount;
    uint32_t txDataBroadcastCount;
    uint32_t frameCounter;
    uint32_t rxInvalidFrameLengthCount;
    uint32_t rxWrongAddrModeCount;
    uint32_t rxUnsupportedSecurityCount;
    uint32_t rxWrongKeyIdCount;
    uint32_t rxInvalidKeyCount;
    uint32_t rxWrongFCCount;
    uint32_t rxDecryptionErrorCount;
    uint16_t deviceTableSize;
    MAC_SHORT_ADDRESS coordShortAddress;
    MAC_PLC_MODULATION_SCHEME lastRxModScheme;
    MAC_PLC_MODULATION_TYPE lastRxModType;
    bool bcnFrameReceived;
    bool plcDisable;
    bool plcAvailable;
    bool freqNotching;
} MAC_PLC_MIB;

#pragma pack(pop)

/* MISRA C-2012 deviation block start */
/* MISRA C-2012 Rule 5.2 deviated once.  Deviation record ID - H3_MISRAC_2012_R_5_2_DR_1 */

// *****************************************************************************
/* MAC PLC Parameter Information Base definition

   Summary:
    Lists the available objects in the MAC PLC Parameter Information Base (PIB).

   Description:
    MAC PIB is a collection of objects that can be read/written in order to
    retrieve information and/or configure the MAC layer.

   Remarks:
    None.
*/
typedef enum
{
    MAC_PIB_MAX_BE = 0x00000047,
    MAC_PIB_BSN = 0x00000049,
    MAC_PIB_DSN = 0x0000004C,
    MAC_PIB_MAX_CSMA_BACKOFFS = 0x0000004E,
    MAC_PIB_MIN_BE = 0x0000004F,
    MAC_PIB_MAX_FRAME_RETRIES = 0x00000059,
    MAC_PIB_TIMESTAMP_SUPPORTED = 0x0000005C,
    MAC_PIB_SECURITY_ENABLED = 0x0000005D,
    MAC_PIB_FRAME_COUNTER = 0x00000077,
    MAC_PIB_DUPLICATE_DETECTION_TTL = 0x00000078,
    MAC_PIB_HIGH_PRIORITY_WINDOW_SIZE = 0x00000100,
    MAC_PIB_TX_DATA_PACKET_COUNT = 0x00000101,
    MAC_PIB_RX_DATA_PACKET_COUNT = 0x00000102,
    MAC_PIB_TX_CMD_PACKET_COUNT = 0x00000103,
    MAC_PIB_RX_CMD_PACKET_COUNT = 0x00000104,
    MAC_PIB_CSMA_FAIL_COUNT = 0x00000105,
    MAC_PIB_CSMA_NO_ACK_COUNT = 0x00000106,
    MAC_PIB_RX_DATA_BROADCAST_COUNT = 0x00000107,
    MAC_PIB_TX_DATA_BROADCAST_COUNT = 0x00000108,
    MAC_PIB_BAD_CRC_COUNT = 0x00000109,
    MAC_PIB_NEIGHBOUR_TABLE = 0x0000010A,
    MAC_PIB_FREQ_NOTCHING = 0x0000010B,
    MAC_PIB_CSMA_FAIRNESS_LIMIT = 0x0000010C,
    MAC_PIB_TMR_TTL = 0x0000010D,
    MAC_PIB_TONE_MASK = 0x00000110,
    MAC_PIB_BEACON_RANDOMIZATION_WINDOW_LENGTH = 0x00000111,
    MAC_PIB_A = 0x00000112,
    MAC_PIB_K = 0x00000113,
    MAC_PIB_MIN_CW_ATTEMPTS = 0x00000114,
    MAC_PIB_CENELEC_LEGACY_MODE = 0x00000115,
    MAC_PIB_FCC_LEGACY_MODE = 0x00000116,
    MAC_PIB_BROADCAST_MAX_CW_ENABLE = 0x0000011E,
    MAC_PIB_TRANSMIT_ATTEN = 0x0000011F,
    MAC_PIB_POS_TABLE = 0x00000120,
    MAC_PIB_POS_RECENT_ENTRIES = 0x00000122,
    MAC_PIB_PLC_DISABLE = 0x00000123,
    MAC_PIB_PREAMBLE_LENGTH = 0x00000124,
    // manufacturer specific
    // provides access to device table
    MAC_PIB_MANUF_DEVICE_TABLE = 0x08000000,
    // provides access to neighbour table by short address (transmitted as index)
    MAC_PIB_MANUF_NEIGHBOUR_TABLE_ELEMENT = 0x08000002,
    // returns the maximum number of tones used by the band
    MAC_PIB_MANUF_BAND_INFORMATION = 0x08000003,
    // Short address of the coordinator.
    MAC_PIB_MANUF_COORD_SHORT_ADDRESS = 0x08000004,
    // Maximal payload supported by MAC.
    MAC_PIB_MANUF_MAX_MAC_PAYLOAD_SIZE = 0x08000005,
    // Resets the device table upon a GMK activation.
    MAC_PIB_MANUF_SECURITY_RESET = 0x08000006,
    // Forces Modulation Scheme in every transmitted frame
    // 0 - Not forced, 1 - Force Differential, 2 - Force Coherent
    MAC_PIB_MANUF_FORCED_MOD_SCHEME = 0x08000007,
    // Forces Modulation Type in every transmitted frame
    // 0 - Not forced, 1 - Force BPSK_ROBO, 2 - Force BPSK, 3 - Force QPSK, 4 - Force 8PSK
    MAC_PIB_MANUF_FORCED_MOD_TYPE = 0x08000008,
    // Forces ToneMap in every transmitted frame
    // {0} - Not forced, other value will be used as tonemap
    MAC_PIB_MANUF_FORCED_TONEMAP = 0x08000009,
    // Forces Modulation Scheme bit in Tone Map Response
    // 0 - Not forced, 1 - Force Differential, 2 - Force Coherent
    MAC_PIB_MANUF_FORCED_MOD_SCHEME_ON_TMRESPONSE = 0x0800000A,
    // Forces Modulation Type bits in Tone Map Response
    // 0 - Not forced, 1 - Force BPSK_ROBO, 2 - Force BPSK, 3 - Force QPSK, 4 - Force 8PSK
    MAC_PIB_MANUF_FORCED_MOD_TYPE_ON_TMRESPONSE = 0x0800000B,
    // Forces ToneMap field Tone Map Response
    // {0} - Not forced, other value will be used as tonemap field
    MAC_PIB_MANUF_FORCED_TONEMAP_ON_TMRESPONSE = 0x0800000C,
    // Gets Modulation Scheme of last received frame
    MAC_PIB_MANUF_LAST_RX_MOD_SCHEME = 0x0800000D,
    // Gets Modulation Scheme of last received frame
    MAC_PIB_MANUF_LAST_RX_MOD_TYPE = 0x0800000E,
    // Indicates whether an LBP frame for other destination has been received
    MAC_PIB_MANUF_LBP_FRAME_RECEIVED = 0x0800000F,
    // Indicates whether an LBP frame for other destination has been received
    MAC_PIB_MANUF_LNG_FRAME_RECEIVED = 0x08000010,
    // Indicates whether an Beacon frame from other nodes has been received
    MAC_PIB_MANUF_BCN_FRAME_RECEIVED = 0x08000011,
    // Gets number of valid elements in the Neighbour Table
    MAC_PIB_MANUF_NEIGHBOUR_TABLE_COUNT = 0x08000012,
    // Gets number of discarded packets due to Other Destination
    MAC_PIB_MANUF_RX_OTHER_DESTINATION_COUNT = 0x08000013,
    // Gets number of discarded packets due to Invalid Frame Lenght
    MAC_PIB_MANUF_RX_INVALID_FRAME_LENGTH_COUNT = 0x08000014,
    // Gets number of discarded packets due to MAC Repetition
    MAC_PIB_MANUF_RX_MAC_REPETITION_COUNT = 0x08000015,
    // Gets number of discarded packets due to Wrong Addressing Mode
    MAC_PIB_MANUF_RX_WRONG_ADDR_MODE_COUNT = 0x08000016,
    // Gets number of discarded packets due to Unsupported Security
    MAC_PIB_MANUF_RX_UNSUPPORTED_SECURITY_COUNT = 0x08000017,
    // Gets number of discarded packets due to Wrong Key Id
    MAC_PIB_MANUF_RX_WRONG_KEY_ID_COUNT = 0x08000018,
    // Gets number of discarded packets due to Invalid Key
    MAC_PIB_MANUF_RX_INVALID_KEY_COUNT = 0x08000019,
    // Gets number of discarded packets due to Wrong Frame Counter
    MAC_PIB_MANUF_RX_WRONG_FC_COUNT = 0x0800001A,
    // Gets number of discarded packets due to Decryption Error
    MAC_PIB_MANUF_RX_DECRYPTION_ERROR_COUNT = 0x0800001B,
    // Gets number of discarded packets due to Segment Decode Error
    MAC_PIB_MANUF_RX_SEGMENT_DECODE_ERROR_COUNT = 0x0800001C,
    // Enables MAC Sniffer
    MAC_PIB_MANUF_ENABLE_MAC_SNIFFER = 0x0800001D,
    // Gets number of valid elements in the POS Table
    MAC_PIB_MANUF_POS_TABLE_COUNT = 0x0800001E,
    // Gets or Sets number of retires left before forcing ROBO mode
    MAC_PIB_MANUF_RETRIES_LEFT_TO_FORCE_ROBO = 0x0800001F,
    // Gets internal MAC version
    MAC_PIB_MANUF_MAC_INTERNAL_VERSION = 0x08000021,
    // Gets internal MAC RT version
    MAC_PIB_MANUF_MAC_RT_INTERNAL_VERSION = 0x08000022,
    // Resets MAC statistics
    MAC_PIB_MANUF_RESET_MAC_STATS = 0x08000023,
    // Enable/Disable Sleep Mode
    MAC_PIB_MANUF_MAC_RT_SLEEP_MODE = 0x08000024,
    // Set PLC Debug Mode
    MAC_PIB_MANUF_MAC_RT_DEBUG_SET = 0x08000025,
    // Read PLC Debug Information
    MAC_PIB_MANUF_MAC_RT_DEBUG_READ = 0x08000026,
    // Provides access to POS table by short address (referenced as index)
    MAC_PIB_MANUF_POS_TABLE_ELEMENT = 0x08000027,
    // Minimum LQI to consider a neighbour for Trickle
    MAC_PIB_MANUF_TRICKLE_MIN_LQI = 0x08000028,
    // LQI for a given neighbour, which short address will be indicated by index. 8 bits.
    MAC_PIB_MANUF_NEIGHBOUR_LQI = 0x08000029,
    // Best LQI found in neighbour table. 8 bits.
    MAC_PIB_MANUF_BEST_LQI = 0x0800002A,
    // PLC Interface availability. 8 bits (bool).
    MAC_PIB_MANUF_PLC_IFACE_AVAILABLE = 0x0800002C,
    // Last PLC frame duration in ms. 16 bits.
    MAC_PIB_MANUF_LAST_FRAME_DURATION_PLC = 0x0800002D,
    // Resets TMR TTL for the Short Address contained in Index. 8 bits.
    MAC_PIB_MANUF_RESET_TMR_TTL = 0x0800002E,
    // Gets or sets a parameter in Phy layer. Index will be used to contain PHY parameter ID.
    // Check 'enum EPhyParam' in MacRtMib.h for available Phy parameter IDs
    MAC_PIB_MANUF_PHY_PARAM = 0x08000020
} MAC_PLC_PIB_ATTRIBUTE;

/* MISRA C-2012 deviation block end */

// *****************************************************************************
/* Function:
    MAC_STATUS MAC_PLC_MIB_SetAttributeSync
    (
        MAC_COMMON_PIB_ATTRIBUTE attribute,
        uint16_t index,
        const MAC_PIB_VALUE *pibValue
    )

  Summary:
    The MAC_PLC_MIB_SetAttributeSync primitive sets the value of an attribute in the
    MAC layer Common Information Base (IB).

  Description:
    SetAttributeSync primitive is used to set the value of a MIB stored
    in MAC Common module.
    Sync suffix indicates that result of set operation is provided upon
    function call return, in the return status code.

  Precondition:
    MAC_PLC_Init routine must have been called before.

  Parameters:
    attribute - Identifier of the Attribute to provide value

    index - Index of element in case Attribute is a table
            Otherwise index must be set to '0'

    pibValue - Pointer to MAC_PIB_VALUE object where value is contained

  Returns:
    Result of set operation as a MAC_STATUS code.

  Example:
    <code>
    MAC_STATUS status;
    const MAC_PIB_VALUE value = {
        .length = 1,
        .value = {1}
    };

    status = MAC_PLC_MIB_SetAttributeSync(MAC_COMMON_PIB_PROMISCUOUS_MODE, 0, &value);
    if (status == MAC_STATUS_SUCCESS)
    {

    }
    </code>

  Remarks:
    None.
*/
MAC_STATUS MAC_PLC_MIB_SetAttributeSync(MAC_COMMON_PIB_ATTRIBUTE attribute,
    uint16_t index, const MAC_PIB_VALUE *pibValue);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // #ifndef MAC_PLC_MIB_H

/*******************************************************************************
 End of File
*/
