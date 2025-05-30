/*******************************************************************************
  IEEE 802.15.4 (2020) SUN OFDM Header File

  Company:
    Microchip Technology Inc.

  File Name:
    ieee_15_4_sun_ofdm.h

  Summary:
    IEEE 802.15.4 (2020) SUN OFDM Header File

  Description:
    This file provides the definition of IEEE 802.15.4 (2020) SUN OFDM modes for
    RF215.
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

#ifndef IEEE_15_4_SUN_OFDM_H
#define IEEE_15_4_SUN_OFDM_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************
#include "driver/rf215/drv_rf215_definitions.h"

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

/************** CCA Energy Detection common configuration (FSK) **************/

/* Duration in us of CCA ED. From IEEE 802.15.4 section 10.2.6 Receiver ED: The
 * ED measurement time, to average over, shall be equal to 8 symbol periods.
 * For SUN OFDM, the symbol duration is fixed to 120 us */
#define SUN_OFDM_CCA_ED_DURATION                960

/* Threshold in dBm of CCA ED. From IEEE 802.15.4 section 10.2.8 Clear channel
 * assessment (CCA): Except for the SUN O-QPSK PHY, the ED threshold shall
 * correspond to a received signal power of at most 10 dB greater than the
 * specified receiver sensitivity for that PHY, or in accordance with local
 * regulations.
 * From IEEE 802.15.4 section 20.5.3 Receiver sensitivity:
 * Table 20-21 - Sensitivity requirements for OFDM options and MCS levels
 * Sensitivity for MCS6 is selected (worst case) */
#define SUN_OFDM_OPT1_CCA_THRESHOLD             -75
#define SUN_OFDM_OPT2_CCA_THRESHOLD             -78
#define SUN_OFDM_OPT3_CCA_THRESHOLD             -81
#define SUN_OFDM_OPT4_CCA_THRESHOLD             -84

/********** SUN OFDM configurations for all bands and operating modes *********/

/* AT86RF215 PHY configuration of SUN OFDM modes as defined in 802.15.4 (2020).
 * Sections 10.1.3.9 Channel numbering for SUN and TVWS PHYs / 20.5.1 Operating
 * frequency range */

/************************* Frequency band 470-510 MHz *************************/

/* OFDM Bandwidth Option 4 */
#define SUN_OFDM_BAND_470_510_OPT4              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_4, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 470200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 198U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT4_CCA_THRESHOLD, \
    }

/************************* Frequency band 779-787 MHz *************************/

/* OFDM Bandwidth Option 4 */
#define SUN_OFDM_BAND_779_787_OPT4              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_4, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 779200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 38U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT4_CCA_THRESHOLD, \
    }

/* OFDM Bandwidth Option 3 */
#define SUN_OFDM_BAND_779_787_OPT3              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_3, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 779400000UL, \
        .chnSpaHz = 400000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 18U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT3_CCA_THRESHOLD, \
    }

/* OFDM Bandwidth Option 2 */
#define SUN_OFDM_BAND_779_787_OPT2              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_2, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 779800000UL, \
        .chnSpaHz = 800000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 8U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT2_CCA_THRESHOLD, \
    }

/* OFDM Bandwidth Option 1 */
#define SUN_OFDM_BAND_779_787_OPT1              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_1, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 780200000UL, \
        .chnSpaHz = 1200000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 5U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT1_CCA_THRESHOLD, \
    }

/************* Frequency band 863-870 MHz (RF-PLC Hybrid default) *************/

/* OFDM Bandwidth Option 4 */
#define SUN_OFDM_BAND_863_870_OPT4              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_4, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 863100000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 34U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT4_CCA_THRESHOLD, \
    }

/************************* Frequency band 865-867 MHz *************************/

/* OFDM Bandwidth Option 4 */
#define SUN_OFDM_BAND_865_867_OPT4              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_4, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 865100000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 9U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT4_CCA_THRESHOLD, \
    }

/************************* Frequency band 866-869 MHz *************************/

/* OFDM Bandwidth Option 4 */
#define SUN_OFDM_BAND_866_869_OPT4              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_4, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 863100000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 15U, \
        .chnNumMax = 29U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT4_CCA_THRESHOLD, \
    }

/* OFDM Bandwidth Option 3 */
#define SUN_OFDM_BAND_866_869_OPT3              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_3, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 863200000UL, \
        .chnSpaHz = 400000U, \
        .chnNumMin = 8U, \
        .chnNumMax = 14U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT3_CCA_THRESHOLD, \
    }

/************************* Frequency band 870-876 MHz *************************/

/* OFDM Bandwidth Option 4 */
#define SUN_OFDM_BAND_870_876_OPT4              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_4, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 870200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 29U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT4_CCA_THRESHOLD, \
    }

/************************* Frequency band 902-928 MHz *************************/

/* OFDM Bandwidth Option 4 */
#define SUN_OFDM_BAND_902_928_OPT4              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_4, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 902200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 128U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT4_CCA_THRESHOLD, \
    }

/* OFDM Bandwidth Option 3 */
#define SUN_OFDM_BAND_902_928_OPT3              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_3, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 902400000UL, \
        .chnSpaHz = 400000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 63U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT3_CCA_THRESHOLD, \
    }

/* OFDM Bandwidth Option 2 */
#define SUN_OFDM_BAND_902_928_OPT2              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_2, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 902800000UL, \
        .chnSpaHz = 800000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 30U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT2_CCA_THRESHOLD, \
    }

/* OFDM Bandwidth Option 1 */
#define SUN_OFDM_BAND_902_928_OPT1              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_1, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 903200000UL, \
        .chnSpaHz = 1200000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 19U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT1_CCA_THRESHOLD, \
    }

/******************* Frequency band 902-928 MHz (alternate) *******************/

/* OFDM Bandwidth Option 4 */
#define SUN_OFDM_BAND_902_928_ALT_OPT4          (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_4, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 902200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 128U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT4_CCA_THRESHOLD, \
    }

/* OFDM Bandwidth Option 3 */
#define SUN_OFDM_BAND_902_928_ALT_OPT3          (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_3, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 902400000UL, \
        .chnSpaHz = 400000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 63U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT3_CCA_THRESHOLD, \
    }

/* OFDM Bandwidth Option 2 */
#define SUN_OFDM_BAND_902_928_ALT_OPT2          (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_2, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 902800000UL, \
        .chnSpaHz = 800000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 30U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT2_CCA_THRESHOLD, \
    }

/* OFDM Bandwidth Option 1 */
#define SUN_OFDM_BAND_902_928_ALT_OPT1          (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_1, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 903200000UL, \
        .chnSpaHz = 1200000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 19U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT1_CCA_THRESHOLD, \
    }

/******************* Frequency band 902-907.5 & 915-928 MHz *******************/

/* MISRA C-2012 deviation block start */
/* MISRA C-2012 Rule 5.4 deviated 3 times.  Deviation record ID - H3_MISRAC_2012_R_5_4_DR_1 */

/* OFDM Bandwidth Option 4 */
#define SUN_OFDM_BAND_902_907_915_928_OPT4              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_4, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 902200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 26U, \
        .chnNumMin2 = 65U, \
        .chnNumMax2 = 128U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT4_CCA_THRESHOLD, \
    }

/* OFDM Bandwidth Option 3 */
#define SUN_OFDM_BAND_902_907_915_928_OPT3              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_3, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 902400000UL, \
        .chnSpaHz = 400000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 12U, \
        .chnNumMin2 = 32U, \
        .chnNumMax2 = 63U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT3_CCA_THRESHOLD, \
    }

/* OFDM Bandwidth Option 2 */
#define SUN_OFDM_BAND_902_907_915_928_OPT2              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_2, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 902800000UL, \
        .chnSpaHz = 800000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 5U, \
        .chnNumMin2 = 16U, \
        .chnNumMax2 = 30U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT2_CCA_THRESHOLD, \
    }

/* OFDM Bandwidth Option 1 */
#define SUN_OFDM_BAND_902_907_915_928_OPT1              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_1, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 903200000UL, \
        .chnSpaHz = 1200000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 3U, \
        .chnNumMin2 = 11U, \
        .chnNumMax2 = 19U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT1_CCA_THRESHOLD, \
    }

/* MISRA C-2012 deviation block end */

/************************* Frequency band 915-928 MHz *************************/

/* OFDM Bandwidth Option 4 */
#define SUN_OFDM_BAND_915_928_OPT4              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_4, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 902200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 65U, \
        .chnNumMax = 128U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT4_CCA_THRESHOLD, \
    }

/* OFDM Bandwidth Option 3 */
#define SUN_OFDM_BAND_915_928_OPT3              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_3, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 902400000UL, \
        .chnSpaHz = 400000U, \
        .chnNumMin = 32U, \
        .chnNumMax = 63U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT3_CCA_THRESHOLD, \
    }

/* OFDM Bandwidth Option 2 */
#define SUN_OFDM_BAND_915_928_OPT2              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_2, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 902800000UL, \
        .chnSpaHz = 800000U, \
        .chnNumMin = 16U, \
        .chnNumMax = 30U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT2_CCA_THRESHOLD, \
    }

/* OFDM Bandwidth Option 1 */
#define SUN_OFDM_BAND_915_928_OPT1              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_1, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 903200000UL, \
        .chnSpaHz = 1200000U, \
        .chnNumMin = 11U, \
        .chnNumMax = 19U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT1_CCA_THRESHOLD, \
    }

/************************* Frequency band 915-921 MHz *************************/

/* OFDM Bandwidth Option 4 */
#define SUN_OFDM_BAND_915_921_OPT4              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_4, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 902200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 65U, \
        .chnNumMax = 93U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT4_CCA_THRESHOLD, \
    }

/* OFDM Bandwidth Option 3 */
#define SUN_OFDM_BAND_915_921_OPT3              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_3, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 902400000UL, \
        .chnSpaHz = 400000U, \
        .chnNumMin = 32U, \
        .chnNumMax = 46U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT3_CCA_THRESHOLD, \
    }

/************************* Frequency band 915-918 MHz *************************/

/* OFDM Bandwidth Option 4 */
#define SUN_OFDM_BAND_915_918_OPT4              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_4, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 902200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 65U, \
        .chnNumMax = 78U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT4_CCA_THRESHOLD, \
    }

/* OFDM Bandwidth Option 3 */
#define SUN_OFDM_BAND_915_918_OPT3              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_3, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 902400000UL, \
        .chnSpaHz = 400000U, \
        .chnNumMin = 32U, \
        .chnNumMax = 38U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT3_CCA_THRESHOLD, \
    }

/* OFDM Bandwidth Option 2 */
#define SUN_OFDM_BAND_915_918_OPT2              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_2, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 902800000UL, \
        .chnSpaHz = 800000U, \
        .chnNumMin = 16U, \
        .chnNumMax = 18U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT2_CCA_THRESHOLD, \
    }

/* OFDM Bandwidth Option 1 */
#define SUN_OFDM_BAND_915_918_OPT1              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_1, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 903200000UL, \
        .chnSpaHz = 1200000U, \
        .chnNumMin = 11U, \
        .chnNumMax = 11U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT1_CCA_THRESHOLD, \
    }

/************************ Frequency band 917-923.5 MHz ************************/

/* OFDM Bandwidth Option 4 */
#define SUN_OFDM_BAND_917_923_OPT4              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_4, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 917100000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 31U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT4_CCA_THRESHOLD, \
    }

/* OFDM Bandwidth Option 3 */
#define SUN_OFDM_BAND_917_923_OPT3              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_3, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 917300000UL, \
        .chnSpaHz = 400000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 15U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT3_CCA_THRESHOLD, \
    }

/* OFDM Bandwidth Option 2 */
#define SUN_OFDM_BAND_917_923_OPT2              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_2, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 917500000UL, \
        .chnSpaHz = 800000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 7U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT2_CCA_THRESHOLD, \
    }

/* OFDM Bandwidth Option 1 */
#define SUN_OFDM_BAND_917_923_OPT1              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_1, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 917900000UL, \
        .chnSpaHz = 1200000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 4U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT1_CCA_THRESHOLD, \
    }

/************************* Frequency band 919-923 MHz *************************/

/* OFDM Bandwidth Option 4 */
#define SUN_OFDM_BAND_919_923_OPT4              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_4, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 902200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 85U, \
        .chnNumMax = 103U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT4_CCA_THRESHOLD, \
    }

/* OFDM Bandwidth Option 3 */
#define SUN_OFDM_BAND_919_923_OPT3              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_3, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 902400000UL, \
        .chnSpaHz = 400000U, \
        .chnNumMin = 42U, \
        .chnNumMax = 51U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT3_CCA_THRESHOLD, \
    }

/* OFDM Bandwidth Option 2 */
#define SUN_OFDM_BAND_919_923_OPT2              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_2, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 902800000UL, \
        .chnSpaHz = 800000U, \
        .chnNumMin = 21U, \
        .chnNumMax = 24U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT2_CCA_THRESHOLD, \
    }

/* OFDM Bandwidth Option 1 */
#define SUN_OFDM_BAND_919_923_OPT1              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_1, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 903200000UL, \
        .chnSpaHz = 1200000U, \
        .chnNumMin = 14U, \
        .chnNumMax = 16U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT1_CCA_THRESHOLD, \
    }

/************************* Frequency band 920-928 MHz *************************/

/* OFDM Bandwidth Option 4 */
#define SUN_OFDM_BAND_920_928_OPT4              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_4, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 920200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 38U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT4_CCA_THRESHOLD, \
    }

/* OFDM Bandwidth Option 3 */
#define SUN_OFDM_BAND_920_928_OPT3              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_3, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 920400000UL, \
        .chnSpaHz = 400000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 18U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT3_CCA_THRESHOLD, \
    }

/* OFDM Bandwidth Option 2 */
#define SUN_OFDM_BAND_920_928_OPT2              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_2, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 920800000UL, \
        .chnSpaHz = 800000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 8U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT2_CCA_THRESHOLD, \
    }

/* OFDM Bandwidth Option 1 */
#define SUN_OFDM_BAND_920_928_OPT1              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_1, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 921200000UL, \
        .chnSpaHz = 1200000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 4U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT1_CCA_THRESHOLD, \
    }

/*********************** Frequency band 920.5-924.5MHz ************************/

/* OFDM Bandwidth Option 4 */
#define SUN_OFDM_BAND_920_924_OPT4              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_4, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 902200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 92U, \
        .chnNumMax = 111U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT4_CCA_THRESHOLD, \
    }

/************************* Frequency band 920-925 MHz *************************/

/* OFDM Bandwidth Option 4 */
#define SUN_OFDM_BAND_920_925_OPT4              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_4, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 902200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 90U, \
        .chnNumMax = 113U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT4_CCA_THRESHOLD, \
    }

/* OFDM Bandwidth Option 3 */
#define SUN_OFDM_BAND_920_925_OPT3              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_3, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 902400000UL, \
        .chnSpaHz = 400000U, \
        .chnNumMin = 45U, \
        .chnNumMax = 56U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT3_CCA_THRESHOLD, \
    }

/* OFDM Bandwidth Option 2 */
#define SUN_OFDM_BAND_920_925_OPT2              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_2, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 902800000UL, \
        .chnSpaHz = 800000U, \
        .chnNumMin = 22U, \
        .chnNumMax = 27U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT2_CCA_THRESHOLD, \
    }

/* OFDM Bandwidth Option 1 */
#define SUN_OFDM_BAND_920_925_OPT1              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_1, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 903200000UL, \
        .chnSpaHz = 1200000U, \
        .chnNumMin = 15U, \
        .chnNumMax = 17U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT1_CCA_THRESHOLD, \
    }

/************************* Frequency band 920-923 MHz *************************/

/* OFDM Bandwidth Option 4 */
#define SUN_OFDM_BAND_920_923_OPT4              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_4, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 902200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 90U, \
        .chnNumMax = 103U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT4_CCA_THRESHOLD, \
    }

/* OFDM Bandwidth Option 3 */
#define SUN_OFDM_BAND_920_923_OPT3              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_3, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 902400000UL, \
        .chnSpaHz = 400000U, \
        .chnNumMin = 45U, \
        .chnNumMax = 51U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT3_CCA_THRESHOLD, \
    }

/* OFDM Bandwidth Option 2 */
#define SUN_OFDM_BAND_920_923_OPT2              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_2, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 902800000UL, \
        .chnSpaHz = 800000U, \
        .chnNumMin = 22U, \
        .chnNumMax = 24U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT2_CCA_THRESHOLD, \
    }

/* OFDM Bandwidth Option 1 */
#define SUN_OFDM_BAND_920_923_OPT1              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_1, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 903200000UL, \
        .chnSpaHz = 1200000U, \
        .chnNumMin = 15U, \
        .chnNumMax = 16U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT1_CCA_THRESHOLD, \
    }

/************************* Frequency band 2400-2483.5 MHz *********************/

/* OFDM Bandwidth Option 4 */
#define SUN_OFDM_BAND_2400_2483_OPT4              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_4, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 2400200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 415U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT4_CCA_THRESHOLD, \
    }

/* OFDM Bandwidth Option 3 */
#define SUN_OFDM_BAND_2400_2483_OPT3              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_3, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 2400400000UL, \
        .chnSpaHz = 400000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 206U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT3_CCA_THRESHOLD, \
    }

/* OFDM Bandwidth Option 2 */
#define SUN_OFDM_BAND_2400_2483_OPT2              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_2, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 2400800000UL, \
        .chnSpaHz = 800000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 96U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT2_CCA_THRESHOLD, \
    }

/* OFDM Bandwidth Option 1 */
#define SUN_OFDM_BAND_2400_2483_OPT1              (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_OFDM, \
        .phyTypeCfg.ofdm = { \
            .opt = OFDM_BW_OPT_1, \
            .itlv = OFDM_INTERLEAVING_1 \
        }, \
        .chnF0Hz = 2401200000UL, \
        .chnSpaHz = 1200000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 63U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_OFDM_OPT1_CCA_THRESHOLD, \
    }

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* IEEE_15_4_SUN_OFDM_H */
