/*******************************************************************************
  RF215 Driver Definitions Header File

  Company:
    Microchip Technology Inc.

  File Name:
    drv_rf215_definitions.h

  Summary:
    RF215 Driver Definitions Header File

  Description:
    This file provides implementation-specific definitions for the RF215
    driver's system interface.
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

#ifndef DRV_RF215_DEFINITIONS_H
#define DRV_RF215_DEFINITIONS_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************
#include <stdint.h>
#include "system/int/sys_int.h"
#include "peripheral/spi/spi_master/plib_spi_master_common.h"

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
/* RF215 Driver PHY Band and Operating Mode

  Summary:
    Defines the available PHY bands and operating modes.

  Description:
    This data type defines the list of available bands and operating modes for
    the RF215 PHY. It is associated to the PIB
    RF215_PIB_PHY_BAND_OPERATING_MODE.

  Remarks:
    This data type is just to simplify the PHY band and operating mode
    configuration, for the most common ones, defined in IEEE 802.15.4-2020 +
    2022-aa amendment (see ieee_15_4_sun_fsk.h and ieee_15_4_sun_ofdm.h). But it
    is also possible to apply a different PHY configuration, using the
    DRV_RF215_PHY_CFG_OBJ data type (PIB RF215_PIB_PHY_CONFIG).
*/

typedef enum
{
    /* 863 (863-870 MHz) band. Europe, India */
    SUN_FSK_BAND_863_OPM1     = 0x0401,
    SUN_FSK_BAND_863_OPM2     = 0x0402,
    SUN_FSK_BAND_863_OPM3     = 0x0403,
    SUN_OFDM_BAND_863_OPT4    = 0x0440,

    /* 866 (865-867 MHz) band. */
    SUN_FSK_BAND_866_OPM1     = 0x0E01,
    SUN_FSK_BAND_866_OPM2     = 0x0E02,
    SUN_FSK_BAND_866_OPM3     = 0x0E03,
    SUN_OFDM_BAND_866_OPT4    = 0x0E40,

    /* 870 (870-876 MHz) band. Europe */
    SUN_FSK_BAND_870_OPM1     = 0x0F01,
    SUN_FSK_BAND_870_OPM2     = 0x0F02,
    SUN_FSK_BAND_870_OPM3     = 0x0F03,
    SUN_OFDM_BAND_870_OPT4    = 0x0F40,

    /* 915 (902-928 MHz) band. USA, Canada, Mexico, Colombia */
    SUN_FSK_BAND_915_OPM1     = 0x0701,
    SUN_FSK_BAND_915_OPM2     = 0x0702,
    SUN_FSK_BAND_915_OPM3     = 0x0703,
    SUN_OFDM_BAND_915_OPT4    = 0x0740,
    SUN_OFDM_BAND_915_OPT3    = 0x0730,
    SUN_OFDM_BAND_915_OPT2    = 0x0720,
    SUN_OFDM_BAND_915_OPT1    = 0x0710,

    /* 915-a (902-928 MHz alternate) band. USA, Canada, Mexico */
    SUN_FSK_BAND_915A_OPM1    = 0x1001,
    SUN_FSK_BAND_915A_OPM2    = 0x1002,
    SUN_FSK_BAND_915A_OPM3    = 0x1003,
    SUN_FSK_BAND_915A_OPM4    = 0x1004,
    SUN_FSK_BAND_915A_OPM5    = 0x1005,
    SUN_OFDM_BAND_915A_OPT4   = 0x1040,
    SUN_OFDM_BAND_915A_OPT3   = 0x1030,
    SUN_OFDM_BAND_915A_OPT2   = 0x1020,
    SUN_OFDM_BAND_915A_OPT1   = 0x1010,

    /* 915-b (902-907.5 & 915-928 MHz) band. Brazil */
    SUN_FSK_BAND_915B_OPM1    = 0x1101,
    SUN_FSK_BAND_915B_OPM2    = 0x1102,
    SUN_FSK_BAND_915B_OPM3    = 0x1103,
    SUN_FSK_BAND_915B_OPM4    = 0x1104,
    SUN_FSK_BAND_915B_OPM5    = 0x1105,
    SUN_OFDM_BAND_915B_OPT4   = 0x1140,
    SUN_OFDM_BAND_915B_OPT3   = 0x1130,
    SUN_OFDM_BAND_915B_OPT2   = 0x1120,
    SUN_OFDM_BAND_915B_OPT1   = 0x1110,

    /* 915-c (915-928 MHz) band. Argentina, Australia, New Zeland */
    SUN_FSK_BAND_915C_OPM1    = 0x1201,
    SUN_FSK_BAND_915C_OPM2    = 0x1202,
    SUN_FSK_BAND_915C_OPM3    = 0x1203,
    SUN_FSK_BAND_915C_OPM4    = 0x1204,
    SUN_FSK_BAND_915C_OPM5    = 0x1205,
    SUN_OFDM_BAND_915C_OPT4   = 0x1240,
    SUN_OFDM_BAND_915C_OPT3   = 0x1230,
    SUN_OFDM_BAND_915C_OPT2   = 0x1220,
    SUN_OFDM_BAND_915C_OPT1   = 0x1210,

    /* 919 (919-923 MHz) band. Malaysia */
    SUN_FSK_BAND_919_OPM1     = 0x1501,
    SUN_FSK_BAND_919_OPM2     = 0x1502,
    SUN_FSK_BAND_919_OPM3     = 0x1503,
    SUN_FSK_BAND_919_OPM4     = 0x1504,
    SUN_FSK_BAND_919_OPM5     = 0x1505,
    SUN_OFDM_BAND_919_OPT4    = 0x1540,
    SUN_OFDM_BAND_919_OPT3    = 0x1530,
    SUN_OFDM_BAND_919_OPT2    = 0x1520,
    SUN_OFDM_BAND_919_OPT1    = 0x1510,

    /* 920 (920-928 MHz) band. Japan */
    SUN_FSK_BAND_920_OPM1     = 0x0901,
    SUN_FSK_BAND_920_OPM2     = 0x0902,
    SUN_FSK_BAND_920_OPM3     = 0x0903,
    SUN_FSK_BAND_920_OPM4     = 0x0904,
    SUN_FSK_BAND_920_OPM5     = 0x0905,
    SUN_FSK_BAND_920_OPM6     = 0x0906,
    SUN_FSK_BAND_920_OPM7     = 0x0907,
    SUN_FSK_BAND_920_OPM8     = 0x0908,
    SUN_FSK_BAND_920_OPM9     = 0x0909,
    SUN_FSK_BAND_920_OPM12    = 0x090C,
    SUN_OFDM_BAND_920_OPT4    = 0x0940,
    SUN_OFDM_BAND_920_OPT3    = 0x0930,
    SUN_OFDM_BAND_920_OPT2    = 0x0920,
    SUN_OFDM_BAND_920_OPT1    = 0x0910,

    /* 920-b (920-923 MHz) band. Indonesia */
    SUN_FSK_BAND_920B_IND_OPM1  = 0x1701,
    SUN_FSK_BAND_920B_IND_OPM2  = 0x1702,
    SUN_FSK_BAND_920B_IND_OPM3  = 0x1703,
    SUN_FSK_BAND_920B_IND_OPM4  = 0x1704,
    SUN_FSK_BAND_920B_IND_OPM5  = 0x1705,
    SUN_OFDM_BAND_920B_IND_OPT4 = 0x1740,
    SUN_OFDM_BAND_920B_IND_OPT3 = 0x1730,
    SUN_OFDM_BAND_920B_IND_OPT2 = 0x1720,
    SUN_OFDM_BAND_920B_IND_OPT1 = 0x1710,

    /* 2450 (2400-2483.5 MHz) band. */
    SUN_FSK_BAND_2450_OPM1    = 0x0D01,
    SUN_FSK_BAND_2450_OPM2    = 0x0D02,
    SUN_FSK_BAND_2450_OPM3    = 0x0D03,
    SUN_OFDM_BAND_2450_OPT4   = 0x0D40,
    SUN_OFDM_BAND_2450_OPT3   = 0x0D30,
    SUN_OFDM_BAND_2450_OPT2   = 0x0D20,
    SUN_OFDM_BAND_2450_OPT1   = 0x0D10,

    /* Custom band and operating mode (DRV_RF215_PHY_CFG_OBJ) */
    DRV_RF215_BAND_OPM_CUSTOM = 0x0000

} DRV_RF215_PHY_BAND_OPM;

// *****************************************************************************
/* RF215 Driver FSK Symbol Rate

  Summary:
    Defines the available FSK symbol rates.

  Description:
    This data type defines the list of available symbol rates for the RF215 FSK
    PHY.

  Remarks:
    None.
*/

typedef enum
{
    FSK_SYM_RATE_50kHz  = 0,
    FSK_SYM_RATE_100kHz = 1,
    FSK_SYM_RATE_150kHz = 2,
    FSK_SYM_RATE_200kHz = 3,
    FSK_SYM_RATE_300kHz = 4,
    FSK_SYM_RATE_400kHz = 5,

} DRV_RF215_FSK_SYM_RATE;

// *****************************************************************************
/* RF215 Driver FSK Modulation Index

  Summary:
    Defines the available FSK modulation indexes.

  Description:
    This data type defines the list of available modulation indexes for the
    RF215 FSK PHY. The modulation index defines the maximum FSK frequency
    deviation: fdev = (symRate * modIdx) / 2.

  Remarks:
    The interpretation of modulation index for 4-FSK is slightly different to
    802.15.4 specification. 4-FSK and modIdx=0.33 (in 802.15.4) is equivalent to
    4-FSK and modIdx=1.0 (in RF215).

    For 4-FSK, the RF215 has the following restriction: modIdx >= 1.0.
*/

typedef enum
{
    /* modIdx = 1.0 */
    FSK_MOD_IDX_1_0 = 0,

    /* modIdx = 0.5 */
    FSK_MOD_IDX_0_5 = 1,

} DRV_RF215_FSK_MOD_IDX;

// *****************************************************************************
/* RF215 Driver FSK Modulation Order

  Summary:
    Defines the available FSK modulation orders.

  Description:
    This data type defines the list of available modulation orders for the RF215
    FSK PHY.

  Remarks:
    The interpretation of modulation index for 4-FSK is slightly different to
    802.15.4 specification. 4-FSK and modIdx=0.33 (in 802.15.4) is equivalent to
    4-FSK and modIdx=1.0 (in RF215).

    For 4-FSK, the RF215 has the following restriction: modIdx >= 1.0.
*/

typedef enum
{
    /* 2-FSK. Frequency deviations: {-fdev, +fdev} */
    FSK_MOD_ORD_2FSK = 0,

    /* 4-FSK. Frequency deviations: {-fdev, -fdev/3, +fdev/3, +fdev} */
    FSK_MOD_ORD_4FSK = 1,

} DRV_RF215_FSK_MOD_ORD;

// *****************************************************************************
/* RF215 Driver OFDM Bandwidth Option

  Summary:
    Defines the available OFDM bandwidth options.

  Description:
    This data type defines the list of available bandwidth options for the RF215
    OFDM PHY, as defined in 802.15.4.

  Remarks:
    None.
*/

typedef enum
{
    /* Option 1: Nominal bandwidth 1094 kHz, 96 data tones, 8 pilot tones */
    OFDM_BW_OPT_1 = 0,

    /* Option 2: Nominal bandwidth 552 kHz, 48 data tones, 4 pilot tones */
    OFDM_BW_OPT_2 = 1,

    /* Option 3: Nominal bandwidth 281 kHz, 24 data tones, 2 pilot tones */
    OFDM_BW_OPT_3 = 2,

    /* Option 4: Nominal bandwidth 156 kHz, 12 data tones, 2 pilot tones */
    OFDM_BW_OPT_4 = 3,

} DRV_RF215_OFDM_BW_OPT;

// *****************************************************************************
/* RF215 Driver OFDM Interleaving Mode

  Summary:
    Defines the available OFDM interleaving modes.

  Description:
    This data type defines the list of available interleaving modes for the
    RF215 OFDM PHY. This is equivalent to phyOFDMInterleaving in 802.15.4
    specification.

  Remarks:
    None.
*/

typedef enum
{
    /* Interleaving depth: one symbol */
    OFDM_INTERLEAVING_0 = 0,

    /* Interleaving depth: frequency domain spreading factor symbols */
    OFDM_INTERLEAVING_1 = 1,

} DRV_RF215_OFDM_ITLV_MODE;

// *****************************************************************************
/* RF215 Driver PHY Type

  Summary:
    Defines the available PHY types.

  Description:
    This data type defines the list of available RF215 PHY types.

  Remarks:
    Each PHY type must be enabled via MCC.
*/

typedef enum
{
    PHY_TYPE_FSK  = 0,
    PHY_TYPE_OFDM = 1,

} DRV_RF215_PHY_TYPE;

// *****************************************************************************
/* RF215 Driver PHY Modulation Scheme

  Summary:
    Defines the available PHY modulation schemes.

  Description:
    This data type defines the list of available modulation schemes, depending
    on the RF215 PHY type.

    This is a dynamic PHY configuration, meaning that a RF215 device is capable
    of receiving messages with any scheme associated to the configured PHY type
    (FSK or OFDM). That is why it is used as parameter in TX request and RX
    indication.

    For FSK, the dynamic parameter is Forward Error Correction (FEC), which can
    be enabled or disabled.

    For OFDM, the dynamic parameter is Modulation and Coding Scheme (MCS).

  Remarks:
    OFDM MCS1 is not available in bandwidth option 4. OFDM MCS0 is not available
    in bandwidth options 3 and 4.
*/

typedef enum
{
    /* FSK: FEC enabled */
    FSK_FEC_OFF  = 0,

    /* FSK: FEC disabled */
    FSK_FEC_ON   = 1,

    /* OFDM: BPSK, 1/2 convolutional encoder rate, 4x frequency repetition */
    OFDM_MCS_0   = 0,

    /* OFDM: BPSK, 1/2 convolutional encoder rate, 2x frequency repetition */
    OFDM_MCS_1   = 1,

    /* OFDM: QPSK, 1/2 convolutional encoder rate, 2x frequency repetition */
    OFDM_MCS_2   = 2,

    /* OFDM: QPSK, 1/2 convolutional encoder rate */
    OFDM_MCS_3   = 3,

    /* OFDM: QPSK, 3/4 convolutional encoder rate */
    OFDM_MCS_4   = 4,

    /* OFDM: 16-QAM, 1/2 convolutional encoder rate */
    OFDM_MCS_5   = 5,

    /* OFDM: 16-QAM, 3/4 convolutional encoder rate */
    OFDM_MCS_6   = 6,

    /* Invalid modulation scheme */
    MOD_SCHEME_INVALID = 0xFF,

} DRV_RF215_PHY_MOD_SCHEME;

// *****************************************************************************
/* RF215 Driver PHY CCA Modes

  Summary:
    Defines the available PHY CCA (Clear Channel Assessment) modes.

  Description:
    This data type defines the list of available PHY CCA (Clear Channel
    Assessment) modes in RF215 driver, as defined in 802.15.4. It is used as
    parameter in TX request.

  Remarks:
    None.
*/

typedef enum
{
    /* Energy above threshold only */
    PHY_CCA_MODE_1 = 0,

    /* Carrier sense only */
    PHY_CCA_MODE_2 = 1,

    /* Energy above threshold and carrier sense */
    PHY_CCA_MODE_3 = 2,

    /* ALOHA. CCA always reports idle medium */
    PHY_CCA_MODE_4 = 3,

    /* Transmit always, even if payload reception in progress */
    PHY_CCA_OFF    = 4,

} DRV_RF215_PHY_CCA_MODE;

// *****************************************************************************
/* RF215 Driver TX Time Modes

  Summary:
    Defines the available TX time modes.

  Description:
    This data type defines the list of available transmission time modes. It is
    used as parameter in TX request.

  Remarks:
    None.
*/

typedef enum
{
    /* Absolute time, referred to system 64-bit counter */
    TX_TIME_ABSOLUTE = 0,

    /* Relative time, referred to current system time */
    TX_TIME_RELATIVE = 1,

} DRV_RF215_TX_TIME_MODE;

// *****************************************************************************
/* RF215 Driver PHY TX Results

  Summary:
    Defines the possible PHY transmission results.

  Description:
    This data type defines the list of possible PHY transmission results in
    RF215 driver.

  Remarks:
    None.
*/

typedef enum
{
    RF215_TX_SUCCESS            = 0,
    RF215_TX_ERROR_UNDERRUN     = 1,
    RF215_TX_ABORTED            = 2,
    RF215_TX_BUSY_TX            = 3,
    RF215_TX_BUSY_RX            = 4,
    RF215_TX_BUSY_CHN           = 5,
    RF215_TX_TRX_SLEPT          = 6,
    RF215_TX_CANCEL_BY_RX       = 7,
    RF215_TX_TIMEOUT            = 8,
    RF215_TX_FULL_BUFFERS       = 9,
    RF215_TX_INVALID_LEN        = 10,
    RF215_TX_INVALID_DRV_HANDLE = 11,
    RF215_TX_INVALID_PARAM      = 12,
    RF215_TX_CANCELLED          = 13,

} DRV_RF215_TX_RESULT;

/* MISRA C-2012 deviation block start */
/* MISRA C-2012 Rule 5.2 deviated once. Deviation record ID - H3_MISRAC_2012_R_5_2_DR_1 */

// *****************************************************************************
/* RF215 Driver PIB Attributes

  Summary:
    Defines the list of PIB attributes.

  Description:
    This data type defines the list of available PIB attributes in RF215 driver.
    PIB values can be read by DRV_RF215_GetPib and written by DRV_RF215_SetPib.
    DRV_RF215_GetPibSize can be used to know the size of each PIB.

  Remarks:
    If dual-band is used, the PIB values are different for each transceiver.
*/

typedef enum
{
    /* RF device identifier (read-only). 16 bits */
    RF215_PIB_DEVICE_ID                        = 0x0000,

    /* RF PHY layer firmware version number (read-only).
     * 6 bytes (see DRV_RF215_FW_VERSION) */
    RF215_PIB_FW_VERSION                       = 0x0001,

    /* RF device reset (write-only). 8 bits */
    RF215_PIB_DEVICE_RESET                     = 0x0002,

    /* RF transceiver reset (write-only). 8 bits */
    RF215_PIB_TRX_RESET                        = 0x0080,

    /* RF transceiver sleep. 8 bits */
    RF215_PIB_TRX_SLEEP                        = 0x0081,

    /* RF PHY configuration. 19 bytes (see DRV_RF215_PHY_CFG_OBJ) */
    RF215_PIB_PHY_CONFIG                       = 0x0100,

    /* RF PHY band and operating mode. 16 bits (see DRV_RF215_PHY_BAND_OPM) */
    RF215_PIB_PHY_BAND_OPERATING_MODE          = 0x0101,

    /* RF channel number used for transmission and reception. 16 bits */
    RF215_PIB_PHY_CHANNEL_NUM                  = 0x0120,

    /* RF frequency in Hz used for transmit and receive (read-only). 32 bits */
    RF215_PIB_PHY_CHANNEL_FREQ_HZ              = 0x0121,

    /* Duration in us of Energy Detection for CCA. 16 bits */
    RF215_PIB_PHY_CCA_ED_DURATION_US           = 0x0141,

    /* Threshold in dBm for CCA with Energy Detection. 8 bits */
    RF215_PIB_PHY_CCA_ED_THRESHOLD_DBM         = 0x0142,

    /* Duration in symbols of Energy Detection for CCA. 16 bits */
    RF215_PIB_PHY_CCA_ED_DURATION_SYMBOLS      = 0x0143,

    /* Threshold in dB above sensitivity for CCA with Energy Detection. 8 bits */
    RF215_PIB_PHY_CCA_ED_THRESHOLD_SENSITIVITY = 0x0144,

    /* Sensitivity in dBm (according to 802.15.4). 8 bits */
    RF215_PIB_PHY_SENSITIVITY = 0x0150,

    /* Maximum TX power in dBm. 8 bits */
    RF215_PIB_PHY_MAX_TX_POWER = 0x0151,

    /* Turnaround time in us (aTurnaroundTime in IEEE 802.15.4).
     * 16 bits (read-only) */
    RF215_PIB_PHY_TURNAROUND_TIME              = 0x0160,

    /* Number of payload symbols in last transmitted message (read-only).
     * 16 bits */
    RF215_PIB_PHY_TX_PAY_SYMBOLS               = 0x0180,

    /* Number of payload symbols in last received message (read-only).
     * 16 bits */
    RF215_PIB_PHY_RX_PAY_SYMBOLS               = 0x0181,

    /* Successfully transmitted messages count (read-only). 32 bits */
    RF215_PIB_PHY_TX_TOTAL                     = 0x01A0,

    /* Successfully transmitted bytes count (read-only). 32 bits */
    RF215_PIB_PHY_TX_TOTAL_BYTES               = 0x01A1,

    /* Transmission errors count (read-only). 32 bits */
    RF215_PIB_PHY_TX_ERR_TOTAL                 = 0x01A2,

    /* Transmission errors count due to already in transmission (read-only).
     * 32 bits */
    RF215_PIB_PHY_TX_ERR_BUSY_TX               = 0x01A3,

    /* Transmission errors count due to already in reception (read-only).
     * 32 bits */
    RF215_PIB_PHY_TX_ERR_BUSY_RX               = 0x01A4,

    /* Transmission errors count due to busy channel (read-only). 32 bits */
    RF215_PIB_PHY_TX_ERR_BUSY_CHN              = 0x01A5,

    /* Transmission errors count due to bad message length (read-only).
     * 32 bits */
    RF215_PIB_PHY_TX_ERR_BAD_LEN               = 0x01A6,

    /* Transmission errors count due to bad format (read-only). 32 bits */
    RF215_PIB_PHY_TX_ERR_BAD_FORMAT            = 0x01A7,

    /* Transmission errors count due to timeout (read-only). 32 bits */
    RF215_PIB_PHY_TX_ERR_TIMEOUT               = 0x01A8,

    /* Transmission aborted count (read-only). 32 bits */
    RF215_PIB_PHY_TX_ERR_ABORTED               = 0x01A9,

    /* Transmission confirms not handled by upper layer count (read-only).
     * 32 bits */
    RF215_PIB_PHY_TX_CFM_NOT_HANDLED           = 0x01AA,

    /* Successfully received messages count (read-only). 32 bits */
    RF215_PIB_PHY_RX_TOTAL                     = 0x01B0,

    /* Successfully received bytes count (read-only). 32 bits */
    RF215_PIB_PHY_RX_TOTAL_BYTES               = 0x01B1,

    /* Reception errors count (read-only). 32 bits */
    RF215_PIB_PHY_RX_ERR_TOTAL                 = 0x01B2,

    /* Reception false positive count (read-only). 32 bits */
    RF215_PIB_PHY_RX_ERR_FALSE_POSITIVE        = 0x01B3,

    /* Reception errors count due to bad message length (read-only). 32 bits */
    RF215_PIB_PHY_RX_ERR_BAD_LEN               = 0x01B4,

    /* Reception errors count due to bad format or bad FCS in header
     * (read-only). 32 bits */
    RF215_PIB_PHY_RX_ERR_BAD_FORMAT            = 0x01B5,

    /* Reception errors count due to bad FCS in payload (read-only). 32 bits */
    RF215_PIB_PHY_RX_ERR_BAD_FCS_PAY           = 0x01B6,

    /* Reception aborted count (read-only). 32 bits */
    RF215_PIB_PHY_RX_ERR_ABORTED               = 0x01B7,

    /* Reception override (another message with higher signal level) count
     * (read-only). 32 bits */
    RF215_PIB_PHY_RX_OVERRIDE                  = 0x01B8,

    /* Reception indications not handled by upper layer count (read-only).
     * 32 bits */
    RF215_PIB_PHY_RX_IND_NOT_HANDLED           = 0x01B9,

    /* Reset PHY Statistics (write-only) */
    RF215_PIB_PHY_STATS_RESET                  = 0x01C0,

    /* Continuous LO Carrier Tx Mode. 8 bits */
    RF215_PIB_PHY_TX_CONTINUOUS                = 0x01C1,

    /* Back-off period unit in us (aUnitBackoffPeriod in IEEE 802.15.4) used for
     * CSMA-CA . 16 bits (read-only) */
    RF215_PIB_MAC_UNIT_BACKOFF_PERIOD          = 0x0200,

} DRV_RF215_PIB_ATTRIBUTE;

/* MISRA C-2012 deviation block end */

// *****************************************************************************
/* RF215 Driver PIB Results

  Summary:
    Defines the list of PIB results.

  Description:
    This data type defines the list of possible results in PIB management
    routines (DRV_RF215_GetPib, DRV_RF215_SetPib).

  Remarks:
    None.
*/

typedef enum
{
    RF215_PIB_RESULT_SUCCESS        = 0,
    RF215_PIB_RESULT_INVALID_PARAM  = 1,
    RF215_PIB_RESULT_INVALID_ATTR   = 2,
    RF215_PIB_RESULT_INVALID_HANDLE = 3,
    RF215_PIB_RESULT_READ_ONLY      = 4,
    RF215_PIB_RESULT_WRITE_ONLY     = 5,

} DRV_RF215_PIB_RESULT;

// *****************************************************************************
/* RF215 Driver FSK PHY Configuration Data

  Summary:
    Defines the data required to configure the RF215 FSK PHY.

  Description:
    This data type defines the data required to configure the RF215 FSK PHY.

    Field description:
    - symRate: FSK symbol rate (see DRV_RF215_FSK_SYM_RATE)
    - modIdx: FSK modulation index (see DRV_RF215_FSK_MOD_IDX)
    - modOrd: FSK modulation order (see DRV_RF215_FSK_MOD_ORD)

  Remarks:
    None.
*/

typedef struct
{
    DRV_RF215_FSK_SYM_RATE       symRate;
    DRV_RF215_FSK_MOD_IDX        modIdx;
    DRV_RF215_FSK_MOD_ORD        modOrd;

} DRV_RF215_FSK_CFG_OBJ;

// *****************************************************************************
/* RF215 Driver OFDM PHY Configuration Data

  Summary:
    Defines the data required to configure the RF215 OFDM PHY.

  Description:
    This data type defines the data required to configure the RF215 OFDM PHY.

    Field description:
    - opt: OFDM bandwidth option (see DRV_RF215_OFDM_BW_OPT)
    - itlv: OFDM interleaving mode (see DRV_RF215_OFDM_ITLV_MODE)

  Remarks:
    None.
*/

typedef struct
{
    DRV_RF215_OFDM_BW_OPT        opt;
    DRV_RF215_OFDM_ITLV_MODE     itlv;

} DRV_RF215_OFDM_CFG_OBJ;

// *****************************************************************************
/* RF215 Driver FSK/OFDM PHY Configuration Data

  Summary:
    Defines the data required to configure the RF215 FSK/OFDM PHY.

  Description:
    This data type defines the data required to configure the parameters
    associated to the corresponding RF215 PHY (FSK or OFDM).

    This is a static PHY configuration, meaning that a RF215 device will only
    receive messages if the configuration is the same as in the transmitter.

    Field description:
    - fsk: FSK PHY configuration (see DRV_RF215_FSK_CFG_OBJ)
    - ofdm: OFDM PHY configuration (see DRV_RF215_OFDM_CFG_OBJ)

  Remarks:
    None.
*/

typedef union
{
    DRV_RF215_FSK_CFG_OBJ        fsk;
    DRV_RF215_OFDM_CFG_OBJ       ofdm;

} DRV_RF215_PHY_TYPE_CFG_OBJ;

// *****************************************************************************
/* RF215 Driver PHY Configuration Data

  Summary:
    Defines the data required to configure the RF215 PHY.

  Description:
    This data type defines the data required to configure the RF215 PHY.

    This is a static PHY configuration, meaning that a RF215 device will only
    receive messages if the configuration is the same as in the transmitter.

  Remarks:
    None.
*/

typedef struct
{
    /* Channel center frequency F0 in Hz */
    uint32_t                     chnF0Hz;

    /* Channel spacing in Hz */
    uint32_t                     chnSpaHz;

    /* Minimum channel number */
    uint16_t                     chnNumMin;

    /* Maximum channel number */
    uint16_t                     chnNumMax;

    /* Minimum channel number (second range) (>0 for only one range) */
    uint16_t                     chnNumMin2;

    /* Maximum channel number (second range) (0 for only one range) */
    uint16_t                     chnNumMax2;

    /* Duration of Energy Detection for CCA (Clear Channel Assessment), in us */
    uint16_t                     ccaEdDurationUS;

    /* Energy Detection threshold for CCA, in dBm */
    int8_t                       ccaEdThresholdDBm;

    /* PHY type (FSK or OFDM) */
    DRV_RF215_PHY_TYPE           phyType;

    /* Configuration parameters for the specific PHY type (FSK or OFDM) */
    DRV_RF215_PHY_TYPE_CFG_OBJ   phyTypeCfg;

} DRV_RF215_PHY_CFG_OBJ;

// *****************************************************************************
/* RF215 Driver RX Indication Data

  Summary:
    Defines the data reported in RF215 RX indication.

  Description:
    This data type defines the data reported in the RF215 receive indication
    (DRV_RF215_RX_IND_CALLBACK).

  Remarks:
    None.
*/

typedef struct
{
    /* RX time (PPDU start), referred to system 64-bit time counter */
    uint64_t                     timeIniCount;

    /* Pointer to received PSDU data */
    uint8_t*                     psdu;

    /* PPDU duration in system time counter units */
    uint32_t                     ppduDurationCount;

    /* PSDU length in bytes (including FCS) */
    uint16_t                     psduLen;

    /* RSSI in dBm */
    int8_t                       rssiDBm;

    /* Modulation scheme for the specific PHY type (FSK or OFDM) */
    DRV_RF215_PHY_MOD_SCHEME     modScheme;

    /* Correct FCS flag */
    bool                         fcsOk;

} DRV_RF215_RX_INDICATION_OBJ;

// *****************************************************************************
/* RF215 Driver TX Request Data

  Summary:
    Defines the parameters for TX request.

  Description:
    This data type defines the parameters needed to request a transmission,
    using the DRV_RF215_TxRequest function.

  Remarks:
    None.
*/

typedef struct
{
    /* TX time (PPDU start), referred to system 64-bit time counter */
    uint64_t                     timeCount;

    /* Pointer to PSDU data to be transmitted */
    uint8_t*                     psdu;

    /* TX time mode (absolute/relative) */
    DRV_RF215_TX_TIME_MODE       timeMode;

    /* Modulation scheme for the specific PHY type (FSK or OFDM) */
    DRV_RF215_PHY_MOD_SCHEME     modScheme;

    /* Clear Channel Assessment (CCA) mode */
    DRV_RF215_PHY_CCA_MODE       ccaMode;

    /* PSDU length in bytes (including FCS) */
    uint16_t                     psduLen;

    /* Transmitter power attenuation in dB (max 31 dB) */
    uint8_t                      txPwrAtt;

    /* Cancel TX if PPDU received before programmed TX time */
    bool                         cancelByRx;

} DRV_RF215_TX_REQUEST_OBJ;

// *****************************************************************************
/* RF215 Driver TX Confirm Data

  Summary:
    Defines the data reported in RF215 TX confirm.

  Description:
    This data type defines the data reported in the RF215 transmit confirm
    callback function (DRV_RF215_TX_CFM_CALLBACK).

  Remarks:
    None.
*/

typedef struct
{
    /* TX time (PPDU start), referred to system 64-bit time counter */
    uint64_t                     timeIniCount;

    /* PPDU duration in system time counter units */
    uint32_t                     ppduDurationCount;

    /* Transmission result */
    DRV_RF215_TX_RESULT          txResult;

} DRV_RF215_TX_CONFIRM_OBJ;

// *****************************************************************************
/* RF215 Driver FW Version Data

  Summary:
    Defines the RF215 Driver firmware version data.

  Description:
    This data type defines the RF215 Driver firmware version data (PIB
    RF215_PIB_FW_VERSION).

  Remarks:
    None.
*/

typedef struct
{
    uint8_t                      major;
    uint8_t                      minor;
    uint8_t                      revision;
    uint8_t                      year; /* year since 2000 */
    uint8_t                      month;
    uint8_t                      day;

} DRV_RF215_FW_VERSION;

// *****************************************************************************
/* RF215 Driver PLIB SPI Is Busy

  Summary:
    Pointer to a PLIB function to check if SPI transmitter is busy.

  Description:
    This data type defines the required function signature for the RF215 driver
    to check if SPI transmitter is busy.

  Parameters:
    None.

  Returns:
    SPI transmitter busy (true) or free (false).

  Remarks:
    None.
*/

typedef bool ( *DRV_RF215_PLIB_SPI_IS_BUSY ) (void);

// *****************************************************************************
/* RF215 Driver PLIB SPI Set Chip Select

  Summary:
    Pointer to a PLIB function to set the SPI chip select line.

  Description:
    This data type defines the required function signature for the RF215 driver
    to set the SPI chip select line.

  Parameters:
    chipSelect - Chip select line to be set.

  Returns:
    None.

  Remarks:
    None.
*/

typedef void ( *DRV_RF215_PLIB_SPI_SET_CS ) (SPI_CHIP_SELECT chipSelect);

// *****************************************************************************
/* RF215 Driver Initialization Data

  Summary:
    Defines the data required to initialize the RF215 driver.

  Description:
    This data type defines the data required to initialize the RF215 driver.

  Remarks:
    None.
*/

typedef struct
{
    /* SPI transmit register address used for DMA operation */
    const void*                     spiTransmitAddress;

    /* SPI receive register address used for DMA operation */
    const void*                     spiReceiveAddress;

    /* Pointer to SPI PLIB is busy function */
    DRV_RF215_PLIB_SPI_IS_BUSY      spiPlibIsBusy;

    /* Pointer to SPI PLIB chip select function */
    DRV_RF215_PLIB_SPI_SET_CS       spiPlibSetChipSelect;

    /* Interrupt source ID for DMA */
    INT_SOURCE                      dmaIntSource;

    /* Interrupt source ID for SYS_TIME */
    INT_SOURCE                      sysTimeIntSource;

    /* Interrupt source ID for PLC external interrupt */
    INT_SOURCE                      plcExtIntSource;

    /* Initial PHY frequency band and operating mode for Sub-GHz transceiver */
    DRV_RF215_PHY_BAND_OPM          rf09PhyBandOpmIni;

    /* Initial PHY frequency channel number for Sub-GHz transceiver */
    uint16_t                        rf09PhyChnNumIni;

    /* Initial PHY frequency band and operating mode for 2.4 GHz transceiver */
    DRV_RF215_PHY_BAND_OPM          rf24PhyBandOpmIni;

    /* Initial PHY frequency channel number for 2.4 GHz transceiver */
    uint16_t                        rf24PhyChnNumIni;

} DRV_RF215_INIT;

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // #ifndef DRV_RF215_DEFINITIONS_H
