/*******************************************************************************
  G3 MAC RF MIB Header File

  Company:
    Microchip Technology Inc.

  File Name:
    mac_rf_mib.h

  Summary:
    G3 MAC RF MIB API Header File

  Description:
    This file contains definitions of the MAC RF Information Base (IB).
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

#ifndef MAC_RF_MIB_H
#define MAC_RF_MIB_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include "mac_rf_defs.h"

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

#define MAC_PIB_OPERATING_MODE_RF_MIN_VALUE                  1
#define MAC_PIB_OPERATING_MODE_RF_MAX_VALUE                  6

#define MAC_PIB_CHANNEL_NUMBER_RF_MAX_VALUE                  7279

#define MAC_PIB_FREQUENCY_BAND_RF_MAX_VALUE                  23

#define MAC_PIB_TRANSMIT_ATTEN_RF_MAX_VALUE                  64

#define MAC_PIB_ADAPTIVE_POWER_STEP_RF_MAX_VALUE             15

#define MAC_PIB_DUTY_CYCLE_PERIOD_RF_MIN_VALUE               1
#define MAC_PIB_DUTY_CYCLE_LIMIT_RF_MIN_VALUE                1
#define MAC_PIB_DUTY_CYCLE_THRESHOLD_RF_MIN_VALUE            1

/* MISRA C-2012 deviation block start */
/* MISRA C-2012 Rule 5.4 deviated once.  Deviation record ID - H3_MISRAC_2012_R_5_4_DR_1 */

#define MAC_PIB_MANUF_ACK_TX_DELAY_RF_MIN_VALUE              1000 // us
#define MAC_PIB_MANUF_ACK_TX_DELAY_RF_MAX_VALUE              9000 // us
#define MAC_PIB_MANUF_ACK_RX_WAIT_TIME_RF_MIN_VALUE          20000 // us
#define MAC_PIB_MANUF_ACK_CONFIRM_WAIT_TIME_RF_MIN_VALUE     20000 // us
#define MAC_PIB_MANUF_DATA_CONFIRM_WAIT_TIME_RF_MIN_VALUE    85000 // us

/* MISRA C-2012 deviation block end */

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* MAC RF POS Table Entry definition

   Summary:
    Defines the fields of an entry in the MAC RF POS Table.

   Description:
    This structure contains the fields which define a MAC RF POS Table entry.
    This table maintains information related to neighbouring nodes.

   Remarks:
    None.
*/
typedef struct __attribute__((packed))
{
    MAC_SHORT_ADDRESS shortAddress;
    uint8_t forwardLqi;
    uint8_t reverseLqi;
    uint8_t dutyCycle;
    uint8_t forwardTxPowerOffset;
    uint8_t reverseTxPowerOffset;
    uint16_t posValidTime;
    uint16_t reverseLqiValidTime;
} MAC_RF_POS_TABLE_ENTRY;

// *****************************************************************************
/* MAC RF DSN Table Entry definition

   Summary:
    Defines the fields of an entry in the MAC RF DSN Table.

   Description:
    This structure contains the fields which define a MAC RF DSN Table entry.
    This table maintains Sequence Number of received frames.

   Remarks:
    None.
*/
typedef struct __attribute__((packed))
{
    MAC_ADDRESS address;
    uint8_t dsn;
    uint8_t dsnValidTime;
} MAC_RF_DSN_TABLE_ENTRY;

// *****************************************************************************
/* MAC RF Tables Structure

   Summary:
    Defines MAC RF Tables and Sizes.

   Description:
    This structure contains pointers to MAC RF Tables (hosted outside the
    MAC RF library) and their sizes.

   Remarks:
    None.
*/
typedef struct
{
    // Pointers
    MAC_RF_POS_TABLE_ENTRY *macRfPosTable;
    MAC_DEVICE_TABLE_ENTRY *macRfDeviceTable;
    MAC_RF_DSN_TABLE_ENTRY *macRfDsnTable;
    // Sizes
    uint16_t macRfPosTableSize;
    uint16_t macRfDeviceTableSize;
    uint16_t macRfDsnTableSize;
} MAC_RF_TABLES;

// *****************************************************************************
/* MAC RF MIB definition

   Summary:
    Defines the MAC RF Information Base.

   Description:
    This structure holds the MAC RF IB, it contains one field per each
    defined PIB.

   Remarks:
    None.
*/
typedef struct
{
    uint32_t retryCountRF;
    uint32_t multipleRetryCountRF;
    uint32_t txFailCountRF;
    uint32_t txSuccessCountRF;
    uint32_t fcsErrorCountRF;
    uint32_t securityFailureCountRF;
    uint32_t duplicateFrameCountRF;
    uint32_t rxSuccessCountRF;
    uint32_t txDataPacketCountRF;
    uint32_t rxDataPacketCountRF;
    uint32_t txCmdPacketCountRF;
    uint32_t rxCmdPacketCountRF;
    uint32_t csmaFailCountRF;
    uint32_t rxDataBroadcastCountRF;
    uint32_t txDataBroadcastCountRF;
    uint32_t badCrcCountRF;
    MAC_RF_POS_TABLE_ENTRY *posTableRF;
    uint16_t posTableSizeRF;
    MAC_DEVICE_TABLE_ENTRY *deviceTableRF;
    uint16_t deviceTableSizeRF;
    MAC_RF_DSN_TABLE_ENTRY *dsnTableRF;
    uint16_t dsnTableSizeRF;
    uint8_t duplicateDetectionTtlRF;
    uint8_t eBsnRF;
    uint8_t dsnRF;
    uint32_t frameCounterRF;
    bool lbpFrameReceivedRF;
    bool lngFrameReceivedRF;
    bool bcnFrameReceivedRF;
    uint32_t rxOtherDestinationCountRF;
    uint32_t rxInvalidFrameLengthCountRF;
    uint32_t rxWrongAddrModeCountRF;
    uint32_t rxUnsupportedSecurityCountRF;
    uint32_t rxWrongKeyIdCountRF;
    uint32_t rxInvalidKeyCountRF;
    uint32_t rxWrongFCCountRF;
    uint32_t rxDecryptionErrorCountRF;
    bool macSnifferRF;
    uint8_t maxBeRF;
    uint8_t maxCsmaBackoffsRF;
    uint8_t maxFrameRetriesRF;
    uint8_t minBeRF;
    uint8_t operatingModeRF;
    uint16_t channelNumberRF;
    uint8_t dutyCycleUsageRF;
    uint16_t dutyCyclePeriodRF;
    uint16_t dutyCycleLimitRF;
    uint8_t dutyCycleThresholdRF;
    uint32_t ackTxDelayRF;
    uint32_t ackRxWaitTimeRF;
    uint32_t ackConfirmWaitTimeRF;
    uint32_t dataConfirmWaitTimeRF;
    uint16_t lastFrameDurationRF;
    uint16_t posRecentEntriesRF;
    bool disablePhyRF;
    bool rfAvailable;
    uint8_t frequencyBandRF;
    uint8_t transmitAttenRF;
    uint8_t adaptivePowerStepRF;
    uint8_t adaptivePowerHighBoundRF;
    uint8_t adaptivePowerLowBoundRF;
    uint8_t beaconRandomizationWindowLength;
    uint8_t trickleMinLQIRF;
} MAC_RF_MIB;

/* MISRA C-2012 deviation block start */
/* MISRA C-2012 Rule 5.2 deviated 17 times.  Deviation record ID - H3_MISRAC_2012_R_5_2_DR_1 */
/* MISRA C-2012 Rule 5.5 deviated 4 times.  Deviation record ID - H3_MISRAC_2012_R_5_5_DR_1 */

// *****************************************************************************
/* MAC RF Parameter Information Base definition

   Summary:
    Lists the available objects in the MAC RF Parameter Information Base (PIB).

   Description:
    MAC PIB is a collection of objects that can be read/written in order to
    retrieve information and/or configure the MAC layer.

   Remarks:
    None.
*/
typedef enum
{
    MAC_PIB_BEACON_RANDOMIZATION_WINDOW_LENGTH_RF = 0x00000111,
    MAC_PIB_DSN_RF = 0x00000200,
    MAC_PIB_MAX_BE_RF = 0x00000201,
    MAC_PIB_MAX_CSMA_BACKOFFS_RF = 0x00000202,
    MAC_PIB_MAX_FRAME_RETRIES_RF = 0x00000203,
    MAC_PIB_MIN_BE_RF = 0x00000204,
    MAC_PIB_TIMESTAMP_SUPPORTED_RF = 0x00000205,
    MAC_PIB_DEVICE_TABLE_RF = 0x00000206,
    MAC_PIB_FRAME_COUNTER_RF = 0x00000207,
    MAC_PIB_DUPLICATE_DETECTION_TTL_RF = 0x00000208,
    MAC_PIB_COUNTER_OCTETS_RF = 0x00000209,
    MAC_PIB_RETRY_COUNT_RF = 0x0000020A,
    MAC_PIB_MULTIPLE_RETRY_COUNT_RF = 0x0000020B,
    MAC_PIB_TX_FAIL_COUNT_RF = 0x0000020C,
    MAC_PIB_TX_SUCCESS_COUNT_RF = 0x0000020D,
    MAC_PIB_FCS_ERROR_COUNT_RF = 0x0000020E,
    MAC_PIB_SECURITY_FAILURE_COUNT_RF = 0x0000020F,
    MAC_PIB_DUPLICATE_FRAME_COUNT_RF = 0x00000210,
    MAC_PIB_RX_SUCCESS_COUNT_RF = 0x00000211,
    MAC_PIB_USE_ENHANCED_BEACON_RF = 0x00000213,
    MAC_PIB_EB_HEADER_IE_LIST_RF = 0x00000214,
    MAC_PIB_EB_PAYLOAD_IE_LIST_RF = 0x00000215,
    MAC_PIB_EB_FILTERING_ENABLED_RF = 0x00000216,
    MAC_PIB_EBSN_RF = 0x00000217,
    MAC_PIB_EB_AUTO_SA_RF = 0x00000218,
    MAC_PIB_SEC_SECURITY_LEVEL_LIST_RF = 0x0000021A,
    MAC_PIB_POS_TABLE_RF = 0x0000021D,
    MAC_PIB_OPERATING_MODE_RF = 0x0000021E,
    MAC_PIB_CHANNEL_NUMBER_RF = 0x0000021F,
    MAC_PIB_DUTY_CYCLE_USAGE_RF = 0x00000220,
    MAC_PIB_DUTY_CYCLE_PERIOD_RF = 0x00000221,
    MAC_PIB_DUTY_CYCLE_LIMIT_RF = 0x00000222,
    MAC_PIB_DUTY_CYCLE_THRESHOLD_RF = 0x00000223,
    MAC_PIB_DISABLE_PHY_RF = 0x00000224,
    MAC_PIB_FREQUENCY_BAND_RF = 0x00000225,
    MAC_PIB_TRANSMIT_ATTEN_RF = 0x00000226,
    MAC_PIB_ADAPTIVE_POWER_STEP_RF = 0x00000227,
    MAC_PIB_ADAPTIVE_POWER_HIGH_BOUND_RF = 0x00000228,
    MAC_PIB_ADAPTIVE_POWER_LOW_BOUND_RF = 0x00000229,
    MAC_PIB_POS_RECENT_ENTRIES_RF = 0x0000022A,

    // Manufacturer specific
    // Resets the device table upon a GMK activation.
    MAC_PIB_MANUF_SECURITY_RESET_RF = 0x08000203,
    // Indicates whether an LBP frame for other destination has been received
    MAC_PIB_MANUF_LBP_FRAME_RECEIVED_RF = 0x08000204,
    // Indicates whether an LBP frame for other destination has been received
    MAC_PIB_MANUF_LNG_FRAME_RECEIVED_RF = 0x08000205,
    // Indicates whether an Beacon frame from other nodes has been received
    MAC_PIB_MANUF_BCN_FRAME_RECEIVED_RF = 0x08000206,
    // Gets number of discarded packets due to Other Destination
    MAC_PIB_MANUF_RX_OTHER_DESTINATION_COUNT_RF = 0x08000207,
    // Gets number of discarded packets due to Invalid Frame Lenght
    MAC_PIB_MANUF_RX_INVALID_FRAME_LENGTH_COUNT_RF = 0x08000208,
    // Gets number of discarded packets due to Wrong Addressing Mode
    MAC_PIB_MANUF_RX_WRONG_ADDR_MODE_COUNT_RF = 0x08000209,
    // Gets number of discarded packets due to Unsupported Security
    MAC_PIB_MANUF_RX_UNSUPPORTED_SECURITY_COUNT_RF = 0x0800020A,
    // Gets number of discarded packets due to Wrong Key Id
    MAC_PIB_MANUF_RX_WRONG_KEY_ID_COUNT_RF = 0x0800020B,
    // Gets number of discarded packets due to Invalid Key
    MAC_PIB_MANUF_RX_INVALID_KEY_COUNT_RF = 0x0800020C,
    // Gets number of discarded packets due to Wrong Frame Counter
    MAC_PIB_MANUF_RX_WRONG_FC_COUNT_RF = 0x0800020D,
    // Gets number of discarded packets due to Decryption Error
    MAC_PIB_MANUF_RX_DECRYPTION_ERROR_COUNT_RF = 0x0800020E,
    // Gets number of transmitted Data packets
    MAC_PIB_MANUF_TX_DATA_PACKET_COUNT_RF = 0x0800020F,
    // Gets number of received Data packets
    MAC_PIB_MANUF_RX_DATA_PACKET_COUNT_RF = 0x08000210,
    // Gets number of transmitted Command packets
    MAC_PIB_MANUF_TX_CMD_PACKET_COUNT_RF = 0x08000211,
    // Gets number of received Command packets
    MAC_PIB_MANUF_RX_CMD_PACKET_COUNT_RF = 0x08000212,
    // Gets number of Channel Access failures
    MAC_PIB_MANUF_CSMA_FAIL_COUNT_RF = 0x08000213,
    // Gets number of received broadcast packets
    MAC_PIB_MANUF_RX_DATA_BROADCAST_COUNT_RF = 0x08000214,
    // Gets number of transmitted broadcast packets
    MAC_PIB_MANUF_TX_DATA_BROADCAST_COUNT_RF = 0x08000215,
    // Gets number of received packets with wrong CRC
    MAC_PIB_MANUF_BAD_CRC_COUNT_RF = 0x08000216,
    // Enables MAC Sniffer
    MAC_PIB_MANUF_ENABLE_MAC_SNIFFER_RF = 0x08000217,
    // Gets number of valid elements in the POS Table
    MAC_PIB_MANUF_POS_TABLE_COUNT_RF = 0x08000218,
    // Gets internal MAC version
    MAC_PIB_MANUF_MAC_INTERNAL_VERSION_RF = 0x08000219,
    // Resets MAC statistics
    MAC_PIB_MANUF_RESET_MAC_STATS_RF = 0x0800021A,
    // Provides access to POS table by short address (referenced as index)
    MAC_PIB_MANUF_POS_TABLE_ELEMENT_RF = 0x0800021B,
    // Configures time between a received frame and the transmission of its ACK
    MAC_PIB_MANUF_ACK_TX_DELAY_RF = 0x0800021C,
    // Configures time to wait for a requested ACK before timing out
    MAC_PIB_MANUF_ACK_RX_WAIT_TIME_RF = 0x0800021D,
    // Configures time to wait for an ACK Confirm before timing out
    MAC_PIB_MANUF_ACK_CONFIRM_WAIT_TIME_RF = 0x0800021E,
    // Configures time to wait for a Data Confirm before timing out
    MAC_PIB_MANUF_DATA_CONFIRM_WAIT_TIME_RF = 0x0800021F,
    // RF Interface availability. 8 bits (bool).
    MAC_PIB_MANUF_RF_IFACE_AVAILABLE = 0x08000221,
    // Last PLC frame duration in ms. 16 bits.
    MAC_PIB_MANUF_LAST_FRAME_DURATION_RF = 0x08000222,
    // Minimum LQI to consider a neighbour for Trickle
    MAC_PIB_MANUF_TRICKLE_MIN_LQI_RF = 0x08000223,
    // Gets or sets a parameter in Phy layer. Index will be used to contain PHY parameter ID
    MAC_PIB_MANUF_PHY_PARAM_RF = 0x08000220
} MAC_RF_PIB_ATTRIBUTE;

/* MISRA C-2012 deviation block end */

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // #ifndef MAC_RF_MIB_H

/*******************************************************************************
 End of File
*/
