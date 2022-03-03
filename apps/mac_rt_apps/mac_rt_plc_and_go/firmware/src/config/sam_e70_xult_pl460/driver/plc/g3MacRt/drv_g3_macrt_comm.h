/*******************************************************************************
  MAC RT Driver Communication Definitions Header File

  Company:
    Microchip Technology Inc.

  File Name:
    drv_g3_macrt_comm.h

  Summary:
    MAC RT Driver Communication Definitions Header File

  Description:
    This file provides implementation-specific definitions for G3 MAC RT 
    interface.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2021 Microchip Technology Inc. and its subsidiaries.
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

#ifndef DRV_G3_MACRT_COMM_H
#define DRV_G3_MACRT_COMM_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <device.h>


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
/* G3 Bandplan */
#define G3_CEN_A                        0
#define G3_CEN_B                        1
#define G3_FCC                          2
#define G3_ARIB                         3
#define G3_INVALID                      0xFF

/* Tone Map size for Cenelec(A,B) bandplan */
#define TONE_MAP_SIZE_CENELEC           1
/* Tone Map size for FCC bandplan */
#define TONE_MAP_SIZE_FCC               3
/* Tone Map size for ARIB bandplan */
#define TONE_MAP_SIZE_ARIB              3

#define MAC_RT_MAX_PAYLOAD_SIZE         (404u)
#define MAC_RT_FULL_HEADER_SIZE         (32)
#define MAC_RT_DATA_MAX_SIZE            (MAC_RT_MAX_PAYLOAD_SIZE + MAC_RT_FULL_HEADER_SIZE)
#define MAC_RT_SECURITY_HEADER_SIZE     (6)
#define MAC_RT_SEGMENT_CONTROL_SIZE     (3)
#define MAC_RT_FCS_SIZE                 (2)
        
#define MAC_RT_SHORT_ADDRESS_BROADCAST  (0xFFFFu)
#define MAC_RT_SHORT_ADDRESS_UNDEFINED  (0xFFFFu)
        
#define MAC_RT_PIB_MAX_VALUE_LENGTH     (144)
#define MAC_RT_PHY_DATA_MAX_SIZE        (494)

#define MAX_PHY_TONES                   72
#define MAX_PHY_TONE_GROUPS             24

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************  
// *****************************************************************************
        
/* G3 MAC RT PIB list

   Summary
    The list of G3 MAC RT PIBs.

   Remarks:
    None
*/
typedef enum {
    MAC_RT_PIB_MAX_BE = 0x00000047,
    MAC_RT_PIB_BSN = 0x00000049,
    MAC_RT_PIB_DSN = 0x0000004C,
    MAC_RT_PIB_MAX_CSMA_BACKOFFS = 0x0000004E,
    MAC_RT_PIB_MIN_BE = 0x0000004F,
    MAC_RT_PIB_PAN_ID = 0x00000050,
    MAC_RT_PIB_PROMISCUOUS_MODE = 0x00000051,
    MAC_RT_PIB_SHORT_ADDRESS = 0x00000053,
    MAC_RT_PIB_MAX_FRAME_RETRIES = 0x00000059,
    MAC_RT_PIB_DUPLICATE_DETECTION_TTL = 0x00000078,
    MAC_RT_PIB_HIGH_PRIORITY_WINDOW_SIZE = 0x00000100,
    MAC_RT_PIB_CSMA_NO_ACK_COUNT = 0x00000106,
    MAC_RT_PIB_BAD_CRC_COUNT = 0x00000109,
    MAC_RT_PIB_NEIGHBOUR_TABLE = 0x0000010A,
    MAC_RT_PIB_CSMA_FAIRNESS_LIMIT = 0x0000010C,
    MAC_RT_PIB_TMR_TTL = 0x0000010D,
    MAC_RT_PIB_POS_TABLE_ENTRY_TTL = 0x0000010E,
    MAC_RT_PIB_RC_COORD = 0x0000010F,
    MAC_RT_PIB_TONE_MASK = 0x00000110,
    MAC_RT_PIB_BEACON_RANDOMIZATION_WINDOW_LENGTH = 0x00000111,
    MAC_RT_PIB_A = 0x00000112,
    MAC_RT_PIB_K = 0x00000113,
    MAC_RT_PIB_MIN_CW_ATTEMPTS = 0x00000114,
    MAC_RT_PIB_CENELEC_LEGACY_MODE = 0x00000115,
    MAC_RT_PIB_FCC_LEGACY_MODE = 0x00000116,
    MAC_RT_PIB_BROADCAST_MAX_CW_ENABLE = 0x0000011E,
    MAC_RT_PIB_TRANSMIT_ATTEN = 0x0000011F,
    MAC_RT_PIB_POS_TABLE = 0x00000120,
    MAC_RT_PIB_POS_RECENT_ENTRY_THRESHOLD = 0x00000121,
    MAC_RT_PIB_POS_RECENT_ENTRIES = 0x00000122,
    /* manufacturer specific */
    /* Extended address of this node. */
    MAC_RT_PIB_MANUF_EXTENDED_ADDRESS = 0x08000001,
    /* provides access to neighbour table by short address (transmitted as index) */
    MAC_RT_PIB_MANUF_NEIGHBOUR_TABLE_ELEMENT = 0x08000002,
    /* returns the maximum number of tones used by the band */
    MAC_RT_PIB_MANUF_BAND_INFORMATION = 0x08000003,
    /* Forces Modulation Scheme in every transmitted frame
        0 - Not forced, 1 - Force Differential, 2 - Force Coherent */
    MAC_RT_PIB_MANUF_FORCED_MOD_SCHEME = 0x08000007,
    /* Forces Modulation Type in every transmitted frame
        0 - Not forced, 1 - Force BPSK_ROBO, 2 - Force BPSK, 3 - Force QPSK, 4 - Force 8PSK */
    MAC_RT_PIB_MANUF_FORCED_MOD_TYPE = 0x08000008,
    /* Forces ToneMap in every transmitted frame
        {0} - Not forced, other value will be used as tonemap */
    MAC_RT_PIB_MANUF_FORCED_TONEMAP = 0x08000009,
    /* Forces Modulation Scheme bit in Tone Map Response
        0 - Not forced, 1 - Force Differential, 2 - Force Coherent */
    MAC_RT_PIB_MANUF_FORCED_MOD_SCHEME_ON_TMRESPONSE = 0x0800000A,
    /* Forces Modulation Type bits in Tone Map Response
        0 - Not forced, 1 - Force BPSK_ROBO, 2 - Force BPSK, 3 - Force QPSK, 4 - Force 8PSK */
    MAC_RT_PIB_MANUF_FORCED_MOD_TYPE_ON_TMRESPONSE = 0x0800000B,
    /* Forces ToneMap field Tone Map Response
        {0} - Not forced, other value will be used as tonemap field */
    MAC_RT_PIB_MANUF_FORCED_TONEMAP_ON_TMRESPONSE = 0x0800000C,
    /* Indicates whether an LBP frame for other destination has been received */
    MAC_RT_PIB_MANUF_LBP_FRAME_RECEIVED = 0x0800000F,
    /* Indicates whether an LBP frame for other destination has been received */
    MAC_RT_PIB_MANUF_LNG_FRAME_RECEIVED = 0x08000010,
    /* Gets number of valid elements in the Neighbour Table */
    MAC_RT_PIB_MANUF_NEIGHBOUR_TABLE_COUNT = 0x08000012,
    /* Gets number of discarded packets due to Other Destination */
    MAC_RT_PIB_MANUF_RX_OTHER_DESTINATION_COUNT = 0x08000013,
    /* Gets number of discarded packets due to MAC Repetition */
    MAC_RT_PIB_MANUF_RX_MAC_REPETITION_COUNT = 0x08000015,
    /* Gets number of discarded packets due to Segment Decode Error */
    MAC_RT_PIB_MANUF_RX_SEGMENT_DECODE_ERROR_COUNT = 0x0800001C,
    /* Enables MAC Sniffer */
    MAC_RT_PIB_MANUF_ENABLE_MAC_SNIFFER = 0x0800001D,
    /* Gets number of valid elements in the POS Table */
    MAC_RT_PIB_MANUF_POS_TABLE_COUNT = 0x0800001E,
    /* Gets or Sets number of retires left before forcing ROBO mode */
    MAC_RT_PIB_MANUF_RETRIES_LEFT_TO_FORCE_ROBO = 0x0800001F,
    /* Gets internal MAC RT version */
    MAC_RT_PIB_MANUF_MAC_RT_INTERNAL_VERSION = 0x08000022,
    /* Enable/Disable Sleep Mode */
    MAC_RT_PIB_SLEEP_MODE = 0x08000024,
    /* Set PLC Debug Mode */
    MAC_RT_PIB_DEBUG_SET = 0x08000025,
    /* Read PL360 Debug Information */
    MAC_RT_PIB_DEBUG_READ = 0x08000026,
    /* Provides access to POS table by short address (referenced as index) */
    MAC_RT_PIB_MANUF_POS_TABLE_ELEMENT = 0x08000027,
    /* Minimum LQI to consider a neighbour for Trickle */
    MAC_RT_PIB_MANUF_TRICKLE_MIN_LQI = 0x08000028,
    /* LQI for a given neighbour, which short address will be indicated by index. 8 bits. */
    MAC_RT_PIB_MANUF_NEIGHBOUR_LQI = 0x08000029,
    /* Best LQI found in neighbour table. 8 bits. */
    MAC_RT_PIB_MANUF_BEST_LQI = 0x0800002A,
    /* Flag to indicate whether next transmission is in High Priority window. 8 bits. */
    MAC_RT_PIB_TX_HIGH_PRIORITY = 0x0800002B,
    /* IB used to set the complete MIB structure at once */
    MAC_RT_PIB_GET_SET_ALL_MIB = 0x08000100,
    /* Gets or sets a parameter in Phy layer. Index will be used to contain PHY parameter ID.
       Check 'enum EPhyParam' below for available Phy parameter IDs */
    MAC_RT_PIB_MANUF_PHY_PARAM = 0x08000020
} MAC_RT_PIB;        

/* G3 PHY Parameters list

   Summary
    The list of G3 PHY parameters.

   Remarks:
    None
*/
typedef enum {
    PHY_PIB_PRODID = 0,
    PHY_PIB_MODEL,
    PHY_PIB_VERSION_STR,
    PHY_PIB_VERSION_NUM,
    PHY_PIB_TONE_MASK,
    PHY_PIB_TONE_MAP_RSP_DATA,
    PHY_PIB_TX_TOTAL,
    PHY_PIB_TX_TOTAL_BYTES,
    PHY_PIB_TX_TOTAL_ERRORS,
    PHY_PIB_TX_BAD_BUSY_TX,
    PHY_PIB_TX_BAD_BUSY_CHANNEL,
    PHY_PIB_TX_BAD_LEN,
    PHY_PIB_TX_BAD_FORMAT,
    PHY_PIB_TX_TIMEOUT,
    PHY_PIB_RX_TOTAL,
    PHY_PIB_RX_TOTAL_BYTES,
    PHY_PIB_RX_RS_ERRORS,
    PHY_PIB_RX_EXCEPTIONS,
    PHY_PIB_RX_BAD_LEN,
    PHY_PIB_RX_BAD_CRC_FCH,
    PHY_PIB_RX_FALSE_POSITIVE,
    PHY_PIB_RX_BAD_FORMAT,
    PHY_PIB_ENABLE_AUTO_NOISE_CAPTURE,
    PHY_PIB_TIME_BETWEEN_NOISE_CAPTURES,
    PHY_PIB_DELAY_NOISE_CAPTURE_AFTER_RX,
    PHY_PIB_RRC_NOTCH_ACTIVE,
    PHY_PIB_RRC_NOTCH_INDEX,
    PHY_PIB_NOISE_PEAK_POWER,
    PHY_PIB_CRC_TX_RX_CAPABILITY,
    PHY_PIB_RX_BAD_CRC_PAY,
    PHY_PIB_CFG_AUTODETECT_IMPEDANCE,
    PHY_PIB_CFG_IMPEDANCE,
    PHY_PIB_ZC_PERIOD,
    PHY_PIB_FCH_SYMBOLS,
    PHY_PIB_PAY_SYMBOLS_TX,
    PHY_PIB_PAY_SYMBOLS_RX,
    PHY_PIB_RRC_NOTCH_AUTODETECT,
    PHY_PIB_MAX_RMS_TABLE_HI,
    PHY_PIB_MAX_RMS_TABLE_VLO,
    PHY_PIB_THRESHOLDS_TABLE_HI,
    PHY_PIB_THRESHOLDS_TABLE_LO,
    PHY_PIB_THRESHOLDS_TABLE_VLO,
    PHY_PIB_PREDIST_COEF_TABLE_HI,
    PHY_PIB_PREDIST_COEF_TABLE_LO,
    PHY_PIB_PREDIST_COEF_TABLE_VLO,
    PHY_PIB_GAIN_TABLE_HI,
    PHY_PIB_GAIN_TABLE_LO,
    PHY_PIB_GAIN_TABLE_VLO,
    PHY_PIB_DACC_TABLE_CFG,
    PHY_PIB_RSV0,
    PHY_PIB_NUM_TX_LEVELS,
    PHY_PIB_CORRECTED_RMS_CALC,
    PHY_PIB_RRC_NOTCH_THR_ON,
    PHY_PIB_RRC_NOTCH_THR_OFF,
    PHY_PIB_CURRENT_GAIN,
    PHY_PIB_ZC_CONF_INV,
    PHY_PIB_ZC_CONF_FREQ,
    PHY_PIB_ZC_CONF_DELAY,
    PHY_PIB_NOISE_PER_CARRIER,
    PHY_PIB_SYNC_XCORR_THRESHOLD,
    PHY_PIB_SYNC_XCORR_PEAK_VALUE,
    PHY_PIB_SYNC_SYNCM_THRESHOLD,
    PHY_PIB_TONE_MAP_RSP_ENABLED_MODS,
    PHY_PIB_PPM_CALIB_ON,
    PHY_PIB_SFO_ESTIMATION_LAST_RX,
    PHY_PIB_PDC_TX_LAST_RX,
    PHY_PIB_MAX_PSDU_LEN_PARAMS,
    PHY_PIB_MAX_PSDU_LEN,
    PHY_PIB_RESET_STATS,
    PHY_PIB_PLC_IC_DRIVER_CFG,
    PHY_PIB_RX_CHN_EST_REAL,
    PHY_PIB_RX_CHN_EST_IMAG,
    PHY_PIB_TX_DISABLE,
    PHY_PIB_TX_HIGH_TEMP_120,
    PHY_PIB_TX_CANCELLED,
    PHY_PIB_ZC_VALUE,
    PHY_PIB_PDC_RX_LAST_RX,
    PHY_PIB_PDC_ZC_TIME_LAST_RX,
    PHY_PIB_PDC_ZC_PERIOD_LAST_RX,
    PHY_PIB_PREAMBLE_NUM_SYNCP
} MAC_RT_PHY_PIB;

// *****************************************************************************
/* G3 Tone Map

   Summary
    Tone Map definition supported by G3 spec.

   Remarks:
    None
*/
typedef struct {
    uint8_t toneMap[(MAX_PHY_TONE_GROUPS + 7) / 8];
} MAC_RT_TONE_MAP;

// *****************************************************************************
/* G3 Tone Mask

   Summary
    Tone Mask definition supported by G3 spec.

   Remarks:
    None
*/
typedef struct {
    uint8_t toneMap[(MAX_PHY_TONES + 7) / 8];
} MAC_RT_TONE_MASK;

// *****************************************************************************
/* G3 Modulation types

   Summary
    The list of all types of modulation supported by G3 spec.

   Remarks:
    Ordered from higher to lower data rate and from higher to lower required 
    SNR (Signal to Noise Ratio): 16QAM, 8PSK, QPSK, BPSK, Robust BPSK
*/
typedef enum {
    MAC_RT_MOD_ROBUST = 0x00,
    MAC_RT_MOD_BPSK = 0x01,
    MAC_RT_MOD_QPSK = 0x02,
    MAC_RT_MOD_8PSK = 0x03,
    MAC_RT_MOD_16QAM = 0x04,
} MAC_RT_MOD_TYPE;

// *****************************************************************************
/* G3 Modulation schemes

   Summary
    The list of all modulation schemes supported by G3 spec.

   Remarks:
    Coherent Scheme supports worst SNR (about 3 dB) than Differential Scheme
    Differential Scheme provides a bit higher data rate because Coherent Scheme 
    uses some carriers for pilots
    Coherent Scheme requires an accurate crystal oscillator. G3-PLC specifies 
    that the frequency error must be less than 25 PPM
*/
typedef enum {
    MAC_RT_MOD_SCHEME_DIFFERENTIAL = 0x00,
    MAC_RT_MOD_SCHEME_COHERENT = 0x01
} MAC_RT_MOD_SCHEME;

// *****************************************************************************
/* G3 Tone map response data

   Summary
    This struct includes modulation type, modulation scheme and Tone Map data

   Remarks:
    For more information about Tone Map Response functionality, please refer to
    G3 Specification
*/
typedef struct {
    /* Modulation type */
    MAC_RT_MOD_TYPE modType;       
    /* Modulation scheme */             
    MAC_RT_MOD_SCHEME modScheme;   
    /* Tone Map */             
    MAC_RT_TONE_MAP toneMap;           
} MAC_RT_TONE_MAP_RSP_DATA;

// *****************************************************************************
/* G3 MAC RT Address Mode

   Summary
    The list of addressing modes supported by G3 spec.

   Remarks:
    None
*/
typedef enum {
    MAC_RT_NO_ADDRESS = 0x00,
    MAC_RT_SHORT_ADDRESS = 0x02,
    MAC_RT_EXTENDED_ADDRESS = 0x03
} MAC_RT_ADDRESS_MODE;

// *****************************************************************************
/* G3 MAC RT Extended Address

   Summary
    This struct includes extended address. 

   Remarks:
*/
typedef struct {
    uint8_t address[8];
} MAC_RT_EXT_ADDRESS;

// *****************************************************************************
/* G3 MAC RT Address

   Summary
    This struct includes addressing mode and address. 

   Remarks:
    The format of the address depends on the addressing mode field.
*/
typedef struct {
    /* Modulation type */
    MAC_RT_ADDRESS_MODE addressMode;       
    union {
        /* Short Address */
        uint16_t shortAddress;
        /* Extended Address */
        MAC_RT_EXT_ADDRESS extendedAddress;
    };        
} MAC_RT_ADDRESS;

// *****************************************************************************
/* G3 MAC RT Frame Type

   Summary
    This struct includes frame type. 

   Remarks:
    The format of the address depends on the addressing mode field.
*/
typedef enum {
    MAC_RT_FRAME_TYPE_BEACON = 0x00,
    MAC_RT_FRAME_TYPE_DATA = 0x01,
    MAC_RT_FRAME_TYPE_ACKNOWLEDGMENT = 0x02,
    MAC_RT_FRAME_TYPE_MAC_COMMAND = 0x03,
    MAC_RT_FRAME_TYPE_RESERVED_4 = 0x04,
    MAC_RT_FRAME_TYPE_RESERVED_5 = 0x05,
    MAC_RT_FRAME_TYPE_RESERVED_6 = 0x06,
    MAC_RT_FRAME_TYPE_RESERVED_7 = 0x07,
} MAC_RT_FRAME_TYPE;

// *****************************************************************************
/* G3 MAC RT Command

   Summary
    This struct includes mac rt commands. 

   Remarks:
*/
typedef enum {
    MAC_RT_COMMAND_BEACON_REQUEST = 0x07,
    MAC_RT_COMMAND_TONE_MAP_RESPONSE = 0x0A,
} MAC_RT_COMMAND;

// *****************************************************************************
/* G3 MAC RT DSN Table entry

   Summary
    This struct includes DSN Table entry definition. 

   Remarks:
*/
typedef struct {
    MAC_RT_ADDRESS address;
    uint8_t dsn;
    uint8_t dsnValidTime;
} MAC_RT_DSN_TABLE_ENTRY;

// *****************************************************************************
/* G3 MAC RT POS entry

   Summary
    This struct includes POS entry definition. 

   Remarks:
*/
typedef struct __attribute__((packed)) {
    uint16_t shortAddress;
    uint8_t lqi;
    uint16_t posValidTime;
} MAC_RT_POS_ENTRY;

// *****************************************************************************
/* G3 MAC RT TX coefficients

   Summary
    This struct includes TX coefficients definition. 

   Remarks:
*/
typedef struct {
    uint8_t txCoef[6];
} MAC_RT_TX_COEF;

// *****************************************************************************
/* G3 MAC RT neighbour entry

   Summary
    This struct includes neighbour entry definition. 

   Remarks:
*/
typedef struct __attribute__((packed)) {
    uint16_t shortAddress;
    MAC_RT_TONE_MAP toneMap;
    uint8_t modulationType : 3;
    uint8_t txGain : 4;
    uint8_t txRes : 1;
    MAC_RT_TX_COEF txCoef;
    uint8_t modulationScheme : 1;
    uint8_t phaseDifferential : 3;
    uint8_t lqi;
    uint16_t tmrValidTime;
} MAC_RT_NEIGHBOUR_ENTRY;

// *****************************************************************************
/* G3 Frame Control information

   Summary
    This struct includes information related to frame control

   Remarks:
    For more information, please refer to G3 Specification
    
*/
typedef struct {
    /* Frame Type */
    uint16_t frameType : 3;
    /* Security Enable */
    uint16_t securityEnabled : 1;
    /* Frame pending */
    uint16_t framePending : 1;
    /* Ack Request */
    uint16_t ackRequest : 1;
    /* PAN ID Compression */
    uint16_t panIdCompression : 1;
    /* Reserved */
    uint16_t reserved : 3;
    /* Destination Addressing Mode */
    uint16_t destAddressingMode : 2;
    /* Frame Version  */
    uint16_t frameVersion : 2;
    /* Source Addressing Mode */
    uint16_t srcAddressingMode : 2;  
} MAC_RT_FRAME_CONTROL;

// *****************************************************************************
/* G3 Segment Control information

   Summary
    This struct includes information related to segment control

   Remarks:
    For more information, please refer to G3 Specification
    
*/
typedef struct {
    /* Reserved */
    uint8_t reserved : 4;
    /* Tone Map Request */
    uint8_t toneMapRequest : 1;
    /* Contention Control */
    uint8_t contentionControl : 1;
    /* Channel Access Priority */
    uint8_t channelAccessPriority : 1;
    /* Last Segment Flag */
    uint8_t lastSegmentFlag : 1;
    /* Segment Count */
    uint16_t segmentCount : 6;
    /* Segment Length of MAC frame */
    uint16_t segmentLength : 10;  
} MAC_RT_SEGMENT_CONTROL;

// *****************************************************************************
/* G3 MAC RT Auxiliary Security Header

   Summary
    This struct includes information related to auxiliary security header

   Remarks:
    For more information, please refer to G3 Specification
    
*/
typedef struct {
    /* Security Level */
    uint8_t securityLevel : 3;
    /* Key Identifier Mode */
    uint8_t keyIdentifierMode : 2;
    /* Reserved */
    uint8_t reserved : 3;
    /* Frame Counter */
    uint32_t frameCounter;
    /* Key Identifier */
    uint8_t keyIdentifier;
} MAC_RT_AUX_SECURITY_HEADER;

// *****************************************************************************
/* G3 MAC RT header

   Summary
    This struct includes information related to MAC header

   Remarks:
    For more information, please refer to G3 Specification
    
*/
typedef struct __attribute__((packed, aligned(1))) {
    /* Frame Control Information */
    MAC_RT_FRAME_CONTROL frameControl;
    /* Sequence Number */
    uint8_t sequenceNumber;
    /* Destination PAN Identifier */
    uint16_t destinationPAN;
    /* Destination Address */
    MAC_RT_ADDRESS destinationAddress;
    /* Source PAN Identifier */
    uint16_t sourcePAN;
    /* Source Address */
    MAC_RT_ADDRESS sourceAddress;
    /* Security Header */
    MAC_RT_AUX_SECURITY_HEADER securityHeader;
} MAC_RT_HEADER;

// *****************************************************************************
/* G3 MAC RT Tone Map response

   Summary
    This struct includes information related to Tone Map response

   Remarks:
    For more information, please refer to G3 Specification
    
*/
typedef struct {
    MAC_RT_MOD_SCHEME modScheme;
    MAC_RT_MOD_TYPE modType;
    uint8_t txGain;
    uint8_t txRes;
    MAC_RT_TONE_MAP toneMap;
    uint8_t reserved1;
    uint8_t lqi;
    MAC_RT_TX_COEF txCoef;
    uint8_t reserved2;
} MAC_RT_TONE_MAP_RESPONSE;

// *****************************************************************************
/* MAC RT MIB Initialization Data

  Summary:
    Defines the data required to initialize the MIB layer

  Description:
    This data type defines the data required to initialize and track MIB layer.

  Remarks:
    None.
*/
typedef struct __attribute__((packed, aligned(1))) {
    uint32_t csmaNoAckCount;
    uint32_t badCrcCount;
    uint32_t rxSegmentDecodeErrorCount;
    uint32_t rxMACRepetitionCount;
    uint32_t rxOtherDestinationCount;
    uint16_t panId;
    uint16_t shortAddress;
    uint16_t pOSRecentEntries;
    uint16_t rcCoord;
    MAC_RT_TONE_MASK toneMask;
    MAC_RT_EXT_ADDRESS extendedAddress;
    MAC_RT_TONE_MAP forcedToneMap;
    MAC_RT_TONE_MAP forcedToneMapOnTMResponse;
    uint8_t highPriorityWindowSize;
    uint8_t csmaFairnessLimit;
    uint8_t A;
    uint8_t K;
    uint8_t minCwAttempts;
    uint8_t maxBe;
    uint8_t bsn;
    uint8_t dsn;
    uint8_t maxCsmaBackoffs;
    uint8_t maxFrameRetries;
    uint8_t minBe;
    uint8_t forcedModScheme;
    uint8_t forcedModType;
    uint8_t forcedModSchemeOnTMResponse;
    uint8_t forcedModTypeOnTMResponse;
    uint8_t retriesToForceRobo;
    uint8_t transmitAtten;
    uint8_t posTableEntryTtl;
    uint8_t posRecentEntryThreshold;
    uint8_t trickleMinLQI;
    uint8_t duplicateDetectionTtl;
    uint8_t tmrTtl;
    uint8_t beaconRandomizationWindowLength;
    bool broadcastMaxCwEnable;
    bool coordinator;
    bool promiscuousMode;
    bool macSniffer;
    bool lbpFrameReceived;
    bool lngFrameReceived;
    bool txHighPriority;
} MAC_RT_MIB_INIT_OBJ;

// *****************************************************************************
/* G3 MAC RT Information Base (PIB)

   Summary
    This struct includes all information to access any defined PIB.

   Remarks:
    None
*/
typedef struct {
    MAC_RT_PIB pib;
    uint16_t index;
    uint8_t length;
    uint8_t pData[MAC_RT_PIB_MAX_VALUE_LENGTH];
} MAC_RT_PIB_OBJ;

// *****************************************************************************
/* G3 MAC RT Status

   Summary
    The list of status values.

   Remarks:
    None
*/
typedef enum {
    MAC_RT_STATUS_SUCCESS = 0x00,
    MAC_RT_STATUS_CHANNEL_ACCESS_FAILURE = 0xE1,
    MAC_RT_STATUS_DENIED = 0xE2,
    MAC_RT_STATUS_INVALID_INDEX = 0xF9,
    MAC_RT_STATUS_INVALID_PARAMETER = 0xE8,
    MAC_RT_STATUS_NO_ACK = 0xE9,
    MAC_RT_STATUS_READ_ONLY = 0xFB,
    MAC_RT_STATUS_TRANSACTION_OVERFLOW = 0xF1,
    MAC_RT_STATUS_UNSUPPORTED_ATTRIBUTE = 0xF4
} MAC_RT_STATUS;

// *****************************************************************************
/* G3 MAC RT State of Transmission

   Summary
    This list involves all available results from MCHP implementation

   Remarks:
    None
*/
typedef enum {
    /* Start of transmission */
    MAC_RT_TX_START,
    /* Start of CSMA_CA */
    MAC_RT_TX_CSMA_CA,
    /* CSMA_CA channel not idle */
    MAC_RT_TX_FAIL_CSMA_CA,
    /* Wait until a data request can be issued */
    MAC_RT_TX_WAIT_SEND,
    /* Wait for data confirm */
    MAC_RT_TX_WAIT_CONFIRM,
    /* Wait for ACK / NAK */
    MAC_RT_TX_WAIT_ACK,
    /* Segment send succeeded and positive ACK received / no ACK requested */
    MAC_RT_TX_SEND_OK,
    /* No ACK was received */
    MAC_RT_TX_BIG_FAIL,
    /* Negative ACK was received */
    MAC_RT_TX_LITTLE_FAIL,
    /* Unexpected PHY error */
    MAC_RT_TX_ABORT
} MAC_RT_TX_STATE;

// *****************************************************************************
/* G3 MAC RT Reception parameters

   Summary
    This struct includes information to describe any new received message.

   Remarks:
    None
*/
typedef struct __attribute__((packed, aligned(1))) {
    /* High Priority */
    bool highPriority;
    /* PDU Link Quality */
    uint8_t pduLinkQuality;
    /* Phase Differential */
    uint8_t phaseDifferential;
    /* Modulation Type */
    MAC_RT_MOD_TYPE modType;
    /* Modulation Scheme */
    MAC_RT_MOD_SCHEME modScheme;
    /* Tone map */
    MAC_RT_TONE_MAP toneMap;
    /* Tone map Response */
    MAC_RT_TONE_MAP_RSP_DATA toneMapRsp;
} MAC_RT_RX_PARAMETERS_OBJ;

// *****************************************************************************
/* G3 MAC RT Reception parameters

   Summary
    This struct includes information to describe any new received message.

   Remarks:
    None
*/
typedef struct {
    /* MAC RT Status */
    MAC_RT_STATUS status;
    /* Flag to indicate Timestamp should be updated */
    bool updateTimestamp;
} MAC_RT_TX_CFM_OBJ;

// *****************************************************************************
/* G3 PHY sniffer parameters

   Summary
    This struct includes information about G3 PHY Sniffer packet.

   Remarks:
    None
*/
typedef struct {
    /* SNIFFER_IF_PHY_COMMAND_G3_VERSION */
    uint8_t commandVersion;    
    /* SNIFFER_VERSION */
	uint8_t snifferVersion;     
    /* SNIFFER_PLC_G3 */
	uint8_t deviceVersion;  
    /* ModType (high) + ModScheme (low) */
	uint8_t modTypeScheme;      
    /* Tone Map */
	uint8_t toneMap[3];     
    /* Number of symbols */
	uint16_t symbols;  
    /* Link Quality Indicator */
	uint8_t lqi;      
    /* Delimiter Type */
	uint8_t dt;       
    /* Init time */
	uint32_t timeIni;            
    /* End time */
	uint32_t timeEnd;     
    /* RSSI */
	uint16_t rssi;      
    /* AGC factor */
	uint16_t agcFactor;       
    /* Data length */
	uint16_t dataLength;                       
} MAC_RT_SNIFFER_HEADER;

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // #ifndef DRV_G3_MACRT_COMM_H

/*******************************************************************************
 End of File
*/
