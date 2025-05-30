/*******************************************************************************
  IEEE 802.15.4 (2020) SUN FSK Header File

  Company:
    Microchip Technology Inc.

  File Name:
    ieee_15_4_sun_fsk.h

  Summary:
    IEEE 802.15.4 (2020) SUN FSK Header File

  Description:
    This file provides the definition of IEEE 802.15.4 (2020 + 2022-aa
    amendment) SUN FSK modes for RF215.
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

#ifndef IEEE_15_4_SUN_FSK_H
#define IEEE_15_4_SUN_FSK_H

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
 * For SUN FSK, the symbol duration is chosen from operating mode #1 */
#define SUN_FSK_50kHz_CCA_ED_DURATION           160
#define SUN_FSK_100kHz_CCA_ED_DURATION          80
#define SUN_FSK_150kHz_CCA_ED_DURATION          53
#define SUN_FSK_200kHz_CCA_ED_DURATION          40
#define SUN_FSK_300kHz_CCA_ED_DURATION          27
#define SUN_FSK_400kHz_CCA_ED_DURATION          20

/* Threshold in dBm of CCA ED. From IEEE 802.15.4 section 10.2.8 Clear channel
 * assessment (CCA): Except for the SUN O-QPSK PHY, the ED threshold shall
 * correspond to a received signal power of at most 10 dB greater than the
 * specified receiver sensitivity for that PHY, or in accordance with local
 * regulations.
 * From IEEE 802.15.4 section 19.6.7 Receiver sensitivity:
 * S = (S0 + 10log[R/R0]) dBm. Where S0 = -91 (without FEC) / -97 (with FEC);
 * R0 = 50 kb/s; R = bit rate in kb/s
 * Sensitivity without FEC is selected (worst case) */
#define SUN_FSK_50kHz_CCA_THRESHOLD             -81
#define SUN_FSK_100kHz_CCA_THRESHOLD            -78
#define SUN_FSK_150kHz_CCA_THRESHOLD            -76
#define SUN_FSK_200kHz_CCA_THRESHOLD            -75
#define SUN_FSK_300kHz_CCA_THRESHOLD            -73
#define SUN_FSK_400kHz_CCA_THRESHOLD            -72

/********** SUN FSK configurations for all bands and operating modes **********/

/* AT86RF215 PHY configuration of SUN FSK modes as defined in 802.15.4 (2020).
 * Sections 10.1.3.9 Channel numbering for SUN and TVWS PHYs / 19.3 Modulation
 * and coding for SUN FSK */

/************************* Frequency band 470-510 MHz *************************/

/* Operating mode #1 */
#define SUN_FSK_BAND_470_510_OPM1               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_50kHz, \
            .modIdx = FSK_MOD_IDX_1_0, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 470200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 198U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0. \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_50kHz_CCA_THRESHOLD, \
    }

/* Operating mode #2 */
#define SUN_FSK_BAND_470_510_OPM2               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_100kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 470200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 198U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_100kHz_CCA_THRESHOLD, \
    }

/* Operating mode #3 */
#define SUN_FSK_BAND_470_510_OPM3               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_150kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 470200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 198U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_150kHz_CCA_THRESHOLD, \
    }

/************************* Frequency band 779-787 MHz *************************/

/* Operating mode #1 */
#define SUN_FSK_BAND_779_787_OPM1               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_50kHz, \
            .modIdx = FSK_MOD_IDX_1_0, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 779200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 38U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_50kHz_CCA_THRESHOLD, \
    }

/* Operating mode #2 */
#define SUN_FSK_BAND_779_787_OPM2               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_100kHz, \
            .modIdx = FSK_MOD_IDX_1_0, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 779400000UL, \
        .chnSpaHz = 400000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 18U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_100kHz_CCA_THRESHOLD, \
    }

/* Operating mode #3 */
#define SUN_FSK_BAND_779_787_OPM3               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_100kHz, \
            .modIdx = FSK_MOD_IDX_1_0, \
            .modOrd = FSK_MOD_ORD_4FSK \
        }, \
        .chnF0Hz = 779400000UL, \
        .chnSpaHz = 400000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 18U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_100kHz_CCA_THRESHOLD + 3, \
    }

/************* Frequency band 863-870 MHz (RF-PLC Hybrid default) *************/

/* Operating mode #1 */
#define SUN_FSK_BAND_863_870_OPM1               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_50kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 863100000UL, \
        .chnSpaHz = 100000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 68U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_50kHz_CCA_THRESHOLD, \
    }

/* Operating mode #2 */
#define SUN_FSK_BAND_863_870_OPM2               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_100kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 863100000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 34U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_100kHz_CCA_THRESHOLD, \
    }

/* Operating mode #3 */
#define SUN_FSK_BAND_863_870_OPM3               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_150kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 863100000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 34U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_150kHz_CCA_THRESHOLD, \
    }

/************************* Frequency band 865-867 MHz *************************/

/* Operating mode #1 */
#define SUN_FSK_BAND_865_867_OPM1               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_50kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 865100000UL, \
        .chnSpaHz = 100000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 18U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_50kHz_CCA_THRESHOLD, \
    }

/* Operating mode #2 */
#define SUN_FSK_BAND_865_867_OPM2               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_100kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 865100000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 9U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_100kHz_CCA_THRESHOLD, \
    }

/* Operating mode #3 */
#define SUN_FSK_BAND_865_867_OPM3               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_150kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 865100000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 9U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_150kHz_CCA_THRESHOLD, \
    }

/************************* Frequency band 866-869 MHz *************************/

/* Operating mode #1 */
#define SUN_FSK_BAND_866_869_OPM1               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_50kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 863100000UL, \
        .chnSpaHz = 100000U, \
        .chnNumMin = 30U, \
        .chnNumMax = 58U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_50kHz_CCA_THRESHOLD, \
    }

/* Operating mode #2 */
#define SUN_FSK_BAND_866_869_OPM2               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_100kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 863100000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 15U, \
        .chnNumMax = 29U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_100kHz_CCA_THRESHOLD, \
    }

/* Operating mode #3 */
#define SUN_FSK_BAND_866_869_OPM3               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_150kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 863100000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 15U, \
        .chnNumMax = 29U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_150kHz_CCA_THRESHOLD, \
    }

/* Operating mode #4 */
#define SUN_FSK_BAND_866_869_OPM4               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_200kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 863200000UL, \
        .chnSpaHz = 400000U, \
        .chnNumMin = 8U, \
        .chnNumMax = 14U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_200kHz_CCA_THRESHOLD, \
    }

/* Operating mode #5 */
#define SUN_FSK_BAND_866_869_OPM5               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_300kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 863200000UL, \
        .chnSpaHz = 400000U, \
        .chnNumMin = 8U, \
        .chnNumMax = 14U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_300kHz_CCA_THRESHOLD, \
    }

/************************* Frequency band 870-876 MHz *************************/

/* Operating mode #1 */
#define SUN_FSK_BAND_870_876_OPM1               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_50kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 870100000UL, \
        .chnSpaHz = 100000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 58U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_50kHz_CCA_THRESHOLD, \
    }

/* Operating mode #2 */
#define SUN_FSK_BAND_870_876_OPM2               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_100kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 870200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 29U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_100kHz_CCA_THRESHOLD, \
    }

/* Operating mode #3 */
#define SUN_FSK_BAND_870_876_OPM3               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_150kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 870200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 29U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_150kHz_CCA_THRESHOLD, \
    }

/************************* Frequency band 902-928 MHz *************************/

/* Operating mode #1 */
#define SUN_FSK_BAND_902_928_OPM1               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_50kHz, \
            .modIdx = FSK_MOD_IDX_1_0, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 902200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 128U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_50kHz_CCA_THRESHOLD, \
    }

/* Operating mode #2 */
#define SUN_FSK_BAND_902_928_OPM2               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_150kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 902400000UL, \
        .chnSpaHz = 400000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 63U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_150kHz_CCA_THRESHOLD, \
    }

/* Operating mode #3 */
#define SUN_FSK_BAND_902_928_OPM3               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_200kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 902400000UL, \
        .chnSpaHz = 400000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 63U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_200kHz_CCA_THRESHOLD, \
    }

/******************* Frequency band 902-928 MHz (alternate) *******************/

/* Operating mode #1 */
#define SUN_FSK_BAND_902_928_ALT_OPM1           (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_50kHz, \
            .modIdx = FSK_MOD_IDX_1_0, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 902200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 128U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_50kHz_CCA_THRESHOLD, \
    }

/* Operating mode #2 */
#define SUN_FSK_BAND_902_928_ALT_OPM2           (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_100kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 902200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 128U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_100kHz_CCA_THRESHOLD, \
    }

/* Operating mode #3 */
#define SUN_FSK_BAND_902_928_ALT_OPM3           (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_150kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 902200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 128U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_150kHz_CCA_THRESHOLD, \
    }

/* Operating mode #4 */
#define SUN_FSK_BAND_902_928_ALT_OPM4           (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_200kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 902400000UL, \
        .chnSpaHz = 400000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 63U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_200kHz_CCA_THRESHOLD, \
    }

/* Operating mode #5 */
#define SUN_FSK_BAND_902_928_ALT_OPM5           (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_300kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 902400000UL, \
        .chnSpaHz = 400000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 63U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_300kHz_CCA_THRESHOLD, \
    }

/******************* Frequency band 902-907.5 & 915-928 MHz *******************/

/* MISRA C-2012 deviation block start */
/* MISRA C-2012 Rule 5.4 deviated 4 times.  Deviation record ID - H3_MISRAC_2012_R_5_4_DR_1 */

/* Operating mode #1 */
#define SUN_FSK_BAND_902_907_915_928_OPM1           (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_50kHz, \
            .modIdx = FSK_MOD_IDX_1_0, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 902200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 26U, \
        .chnNumMin2 = 65U, \
        .chnNumMax2 = 128U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_50kHz_CCA_THRESHOLD, \
    }

/* Operating mode #2 */
#define SUN_FSK_BAND_902_907_915_928_OPM2           (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_100kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 902200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 26U, \
        .chnNumMin2 = 65U, \
        .chnNumMax2 = 128U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_100kHz_CCA_THRESHOLD, \
    }

/* Operating mode #3 */
#define SUN_FSK_BAND_902_907_915_928_OPM3           (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_150kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 902200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 26U, \
        .chnNumMin2 = 65U, \
        .chnNumMax2 = 128U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_150kHz_CCA_THRESHOLD, \
    }

/* Operating mode #4 */
#define SUN_FSK_BAND_902_907_915_928_OPM4           (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_200kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 902400000UL, \
        .chnSpaHz = 400000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 12U, \
        .chnNumMin2 = 32U, \
        .chnNumMax2 = 63U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_200kHz_CCA_THRESHOLD, \
    }

/* Operating mode #5 */
#define SUN_FSK_BAND_902_907_915_928_OPM5           (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_300kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 902400000UL, \
        .chnSpaHz = 400000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 12U, \
        .chnNumMin2 = 32U, \
        .chnNumMax2 = 63U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_300kHz_CCA_THRESHOLD, \
    }

/* MISRA C-2012 deviation block end */

/************************* Frequency band 915-928 MHz *************************/

/* Operating mode #1 */
#define SUN_FSK_BAND_915_928_OPM1               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_50kHz, \
            .modIdx = FSK_MOD_IDX_1_0, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 902200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 65U, \
        .chnNumMax = 128U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_50kHz_CCA_THRESHOLD, \
    }

/* Operating mode #2 */
#define SUN_FSK_BAND_915_928_OPM2               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_100kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 902200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 65U, \
        .chnNumMax = 128U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_100kHz_CCA_THRESHOLD, \
    }

/* Operating mode #3 */
#define SUN_FSK_BAND_915_928_OPM3               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_150kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 902200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 65U, \
        .chnNumMax = 128U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_150kHz_CCA_THRESHOLD, \
    }

/* Operating mode #4 */
#define SUN_FSK_BAND_915_928_OPM4               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_200kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 902400000UL, \
        .chnSpaHz = 400000U, \
        .chnNumMin = 32U, \
        .chnNumMax = 63U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_200kHz_CCA_THRESHOLD, \
    }

/* Operating mode #5 */
#define SUN_FSK_BAND_915_928_OPM5               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_300kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 902400000UL, \
        .chnSpaHz = 400000U, \
        .chnNumMin = 32U, \
        .chnNumMax = 63U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_300kHz_CCA_THRESHOLD, \
    }

/************************* Frequency band 915-921 MHz *************************/

/* Operating mode #1 */
#define SUN_FSK_BAND_915_921_OPM1               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_50kHz, \
            .modIdx = FSK_MOD_IDX_1_0, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 902200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 65U, \
        .chnNumMax = 93U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_50kHz_CCA_THRESHOLD, \
    }

/* Operating mode #2 */
#define SUN_FSK_BAND_915_921_OPM2               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_100kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 902200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 65U, \
        .chnNumMax = 93U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_100kHz_CCA_THRESHOLD, \
    }

/* Operating mode #3 */
#define SUN_FSK_BAND_915_921_OPM3               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_150kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 902200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 65U, \
        .chnNumMax = 93U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_150kHz_CCA_THRESHOLD, \
    }

/* Operating mode #4 */
#define SUN_FSK_BAND_915_921_OPM4               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_200kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 902400000UL, \
        .chnSpaHz = 400000U, \
        .chnNumMin = 32U, \
        .chnNumMax = 46U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_200kHz_CCA_THRESHOLD, \
    }

/* Operating mode #5 */
#define SUN_FSK_BAND_915_921_OPM5               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_300kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 902400000UL, \
        .chnSpaHz = 400000U, \
        .chnNumMin = 32U, \
        .chnNumMax = 46U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_300kHz_CCA_THRESHOLD, \
    }

/************************* Frequency band 915-918 MHz *************************/

/* Operating mode #1 */
#define SUN_FSK_BAND_915_918_OPM1               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_50kHz, \
            .modIdx = FSK_MOD_IDX_1_0, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 902200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 65U, \
        .chnNumMax = 78U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_50kHz_CCA_THRESHOLD, \
    }

/* Operating mode #2 */
#define SUN_FSK_BAND_915_918_OPM2               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_100kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 902200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 65U, \
        .chnNumMax = 78U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_100kHz_CCA_THRESHOLD, \
    }

/* Operating mode #3 */
#define SUN_FSK_BAND_915_918_OPM3               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_150kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 902200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 65U, \
        .chnNumMax = 78U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_150kHz_CCA_THRESHOLD, \
    }

/* Operating mode #4 */
#define SUN_FSK_BAND_915_918_OPM4               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_200kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 902400000UL, \
        .chnSpaHz = 400000U, \
        .chnNumMin = 32U, \
        .chnNumMax = 38U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_200kHz_CCA_THRESHOLD, \
    }

/* Operating mode #5 */
#define SUN_FSK_BAND_915_918_OPM5               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_300kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 902400000UL, \
        .chnSpaHz = 400000U, \
        .chnNumMin = 32U, \
        .chnNumMax = 38U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_300kHz_CCA_THRESHOLD, \
    }

/************************ Frequency band 917-923.5 MHz ************************/

/* Operating mode #1 */
#define SUN_FSK_BAND_917_923_OPM1               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_50kHz, \
            .modIdx = FSK_MOD_IDX_1_0, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 917100000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 31U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_50kHz_CCA_THRESHOLD, \
    }

/* Operating mode #2 */
#define SUN_FSK_BAND_917_923_OPM2               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_150kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 917300000UL, \
        .chnSpaHz = 400000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 15U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_150kHz_CCA_THRESHOLD, \
    }

/* Operating mode #3 */
#define SUN_FSK_BAND_917_923_OPM3               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_200kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 917300000UL, \
        .chnSpaHz = 400000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 15U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_200kHz_CCA_THRESHOLD, \
    }

/************************* Frequency band 919-923 MHz *************************/

/* Operating mode #1 */
#define SUN_FSK_BAND_919_923_OPM1               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_50kHz, \
            .modIdx = FSK_MOD_IDX_1_0, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 902200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 85U, \
        .chnNumMax = 103U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_50kHz_CCA_THRESHOLD, \
    }

/* Operating mode #2 */
#define SUN_FSK_BAND_919_923_OPM2               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_100kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 902200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 85U, \
        .chnNumMax = 103U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_100kHz_CCA_THRESHOLD, \
    }

/* Operating mode #3 */
#define SUN_FSK_BAND_919_923_OPM3               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_150kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 902200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 85U, \
        .chnNumMax = 103U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_150kHz_CCA_THRESHOLD, \
    }

/* Operating mode #4 */
#define SUN_FSK_BAND_919_923_OPM4               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_200kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 902400000UL, \
        .chnSpaHz = 400000U, \
        .chnNumMin = 42U, \
        .chnNumMax = 51U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_200kHz_CCA_THRESHOLD, \
    }

/* Operating mode #5 */
#define SUN_FSK_BAND_919_923_OPM5               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_300kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 902400000UL, \
        .chnSpaHz = 400000U, \
        .chnNumMin = 42U, \
        .chnNumMax = 51U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_300kHz_CCA_THRESHOLD, \
    }

/************************* Frequency band 920-928 MHz *************************/

/* Operating mode #1 */
#define SUN_FSK_BAND_920_928_OPM1               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_50kHz, \
            .modIdx = FSK_MOD_IDX_1_0, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 920600000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 36U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_50kHz_CCA_THRESHOLD, \
    }

/* Operating mode #2 */
#define SUN_FSK_BAND_920_928_OPM2               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_100kHz, \
            .modIdx = FSK_MOD_IDX_1_0, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 920900000UL, \
        .chnSpaHz = 400000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 17U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_100kHz_CCA_THRESHOLD, \
    }

/* Operating mode #3 */
#define SUN_FSK_BAND_920_928_OPM3               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_200kHz, \
            .modIdx = FSK_MOD_IDX_1_0, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 920800000UL, \
        .chnSpaHz = 600000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 11U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_200kHz_CCA_THRESHOLD, \
    }

/* Operating mode #4 */
#define SUN_FSK_BAND_920_928_OPM4               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_200kHz, \
            .modIdx = FSK_MOD_IDX_1_0, \
            .modOrd = FSK_MOD_ORD_4FSK \
        }, \
        .chnF0Hz = 920800000UL, \
        .chnSpaHz = 600000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 11U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_200kHz_CCA_THRESHOLD + 3, \
    }

/* Operating mode #5 */
#define SUN_FSK_BAND_920_928_OPM5               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_200kHz, \
            .modIdx = FSK_MOD_IDX_1_0, \
            .modOrd = FSK_MOD_ORD_4FSK \
        }, \
        .chnF0Hz = 920900000UL, \
        .chnSpaHz = 400000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 17U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_200kHz_CCA_THRESHOLD + 3, \
    }

/* Operating mode #6 */
#define SUN_FSK_BAND_920_928_OPM6               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_150kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 920900000UL, \
        .chnSpaHz = 400000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 17U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_150kHz_CCA_THRESHOLD + 3, \
    }

/* Operating mode #7 */
#define SUN_FSK_BAND_920_928_OPM7               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_300kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 920900000UL, \
        .chnSpaHz = 400000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 17U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_300kHz_CCA_THRESHOLD + 3, \
    }

/* Operating mode #8 */
#define SUN_FSK_BAND_920_928_OPM8               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_300kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 920800000UL, \
        .chnSpaHz = 600000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 11U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_300kHz_CCA_THRESHOLD + 3, \
    }

/* Operating mode #9 */
#define SUN_FSK_BAND_920_928_OPM9               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_400kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 921000000UL, \
        .chnSpaHz = 1000000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 6U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_400kHz_CCA_THRESHOLD + 3, \
    }

/* Operating mode #12 */
#define SUN_FSK_BAND_920_928_OPM12               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_400kHz, \
            .modIdx = FSK_MOD_IDX_1_0, \
            .modOrd = FSK_MOD_ORD_4FSK \
        }, \
        .chnF0Hz = 921000000UL, \
        .chnSpaHz = 1000000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 6U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_400kHz_CCA_THRESHOLD + 3, \
    }

/*********************** Frequency band 920.5-924.5MHz ************************/

/* Operating mode #1 */
#define SUN_FSK_BAND_920_924_OPM1               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_50kHz, \
            .modIdx = FSK_MOD_IDX_1_0, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 902200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 92U, \
        .chnNumMax = 111U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_50kHz_CCA_THRESHOLD, \
    }

/* Operating mode #2 */
#define SUN_FSK_BAND_920_924_OPM2               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_100kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 902200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 92U, \
        .chnNumMax = 111U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_100kHz_CCA_THRESHOLD, \
    }

/* Operating mode #3 */
#define SUN_FSK_BAND_920_924_OPM3               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_150kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 902200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 92U, \
        .chnNumMax = 111U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_150kHz_CCA_THRESHOLD, \
    }

/************************* Frequency band 920-925 MHz *************************/

/* Operating mode #1 */
#define SUN_FSK_BAND_920_925_OPM1               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_50kHz, \
            .modIdx = FSK_MOD_IDX_1_0, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 902200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 90U, \
        .chnNumMax = 113U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_50kHz_CCA_THRESHOLD, \
    }

/* Operating mode #2 */
#define SUN_FSK_BAND_920_925_OPM2               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_100kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 902200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 90U, \
        .chnNumMax = 113U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_100kHz_CCA_THRESHOLD, \
    }

/* Operating mode #3 */
#define SUN_FSK_BAND_920_925_OPM3               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_150kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 902200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 90U, \
        .chnNumMax = 113U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_150kHz_CCA_THRESHOLD, \
    }

/* Operating mode #4 */
#define SUN_FSK_BAND_920_925_OPM4               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_200kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 902400000UL, \
        .chnSpaHz = 400000U, \
        .chnNumMin = 45U, \
        .chnNumMax = 56U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_200kHz_CCA_THRESHOLD, \
    }

/* Operating mode #5 */
#define SUN_FSK_BAND_920_925_OPM5               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_300kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 902400000UL, \
        .chnSpaHz = 400000U, \
        .chnNumMin = 45U, \
        .chnNumMax = 56U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_300kHz_CCA_THRESHOLD, \
    }

/************************* Frequency band 920-923 MHz *************************/

/* Operating mode #1 */
#define SUN_FSK_BAND_920_923_OPM1               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_50kHz, \
            .modIdx = FSK_MOD_IDX_1_0, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 902200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 90U, \
        .chnNumMax = 103U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_50kHz_CCA_THRESHOLD, \
    }

/* Operating mode #2 */
#define SUN_FSK_BAND_920_923_OPM2               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_100kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 902200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 90U, \
        .chnNumMax = 103U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_100kHz_CCA_THRESHOLD, \
    }

/* Operating mode #3 */
#define SUN_FSK_BAND_920_923_OPM3               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_150kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 902200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 90U, \
        .chnNumMax = 103U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_150kHz_CCA_THRESHOLD, \
    }

/* Operating mode #4 */
#define SUN_FSK_BAND_920_923_OPM4               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_200kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 902400000UL, \
        .chnSpaHz = 400000U, \
        .chnNumMin = 45U, \
        .chnNumMax = 51U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_200kHz_CCA_THRESHOLD, \
    }

/* Operating mode #5 */
#define SUN_FSK_BAND_920_923_OPM5               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_300kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 902400000UL, \
        .chnSpaHz = 400000U, \
        .chnNumMin = 45U, \
        .chnNumMax = 51U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_300kHz_CCA_THRESHOLD, \
    }

/************************* Frequency band 2400-2483.5 MHz *********************/

/* Operating mode #1 */
#define SUN_FSK_BAND_2400_2483_OPM1               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_50kHz, \
            .modIdx = FSK_MOD_IDX_1_0, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 2400200000UL, \
        .chnSpaHz = 200000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 415U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_50kHz_CCA_THRESHOLD, \
    }

/* Operating mode #2 */
#define SUN_FSK_BAND_2400_2483_OPM2               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_150kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 2400400000UL, \
        .chnSpaHz = 400000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 206U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_150kHz_CCA_THRESHOLD, \
    }

/* Operating mode #3 */
#define SUN_FSK_BAND_2400_2483_OPM3               (DRV_RF215_PHY_CFG_OBJ) \
    { \
        .phyType = PHY_TYPE_FSK, \
        .phyTypeCfg.fsk = { \
            .symRate = FSK_SYM_RATE_200kHz, \
            .modIdx = FSK_MOD_IDX_0_5, \
            .modOrd = FSK_MOD_ORD_2FSK \
        }, \
        .chnF0Hz = 2400400000UL, \
        .chnSpaHz = 400000U, \
        .chnNumMin = 0U, \
        .chnNumMax = 206U, \
        .chnNumMin2 = 0xFFFFU, \
        .chnNumMax2 = 0U, \
        .ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
        .ccaEdThresholdDBm = SUN_FSK_200kHz_CCA_THRESHOLD, \
    }

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* IEEE_15_4_SUN_FSK_H */
