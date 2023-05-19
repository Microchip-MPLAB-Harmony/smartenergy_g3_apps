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

#ifndef _IEEE_15_4_SUN_OFDM_H
#define _IEEE_15_4_SUN_OFDM_H

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
 * Sensitivity for highest MCS is selected (worst case) */
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
		.chnF0Hz = 470200000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 0, \
		.chnNumMax = 198, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 779200000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 0, \
		.chnNumMax = 38, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 779400000, \
		.chnSpaHz = 400000, \
		.chnNumMin = 0, \
		.chnNumMax = 18, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 779800000, \
		.chnSpaHz = 800000, \
		.chnNumMin = 0, \
		.chnNumMax = 8, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 780200000, \
		.chnSpaHz = 1200000, \
		.chnNumMin = 0, \
		.chnNumMax = 5, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 863100000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 0, \
		.chnNumMax = 34, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 865100000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 0, \
		.chnNumMax = 9, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 863100000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 15, \
		.chnNumMax = 29, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 863200000, \
		.chnSpaHz = 400000, \
		.chnNumMin = 8, \
		.chnNumMax = 14, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 870200000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 0, \
		.chnNumMax = 29, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902200000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 0, \
		.chnNumMax = 128, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902400000, \
		.chnSpaHz = 400000, \
		.chnNumMin = 0, \
		.chnNumMax = 63, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902800000, \
		.chnSpaHz = 800000, \
		.chnNumMin = 0, \
		.chnNumMax = 30, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 903200000, \
		.chnSpaHz = 1200000, \
		.chnNumMin = 0, \
		.chnNumMax = 19, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902200000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 0, \
		.chnNumMax = 128, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902400000, \
		.chnSpaHz = 400000, \
		.chnNumMin = 0, \
		.chnNumMax = 63, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902800000, \
		.chnSpaHz = 800000, \
		.chnNumMin = 0, \
		.chnNumMax = 30, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 903200000, \
		.chnSpaHz = 1200000, \
		.chnNumMin = 0, \
		.chnNumMax = 19, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
		.ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
		.ccaEdThresholdDBm = SUN_OFDM_OPT1_CCA_THRESHOLD, \
	}

/******************* Frequency band 902-907.5 & 915-928 MHz *******************/

/* OFDM Bandwidth Option 4 */
#define SUN_OFDM_BAND_902_907_915_928_OPT4              (DRV_RF215_PHY_CFG_OBJ) \
	{ \
		.phyType = PHY_TYPE_OFDM, \
		.phyTypeCfg.ofdm = { \
			.opt = OFDM_BW_OPT_4, \
			.itlv = OFDM_INTERLEAVING_1 \
		}, \
		.chnF0Hz = 902200000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 0, \
		.chnNumMax = 26, \
		.chnNumMin2 = 65, \
		.chnNumMax2 = 128, \
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
		.chnF0Hz = 902400000, \
		.chnSpaHz = 400000, \
		.chnNumMin = 0, \
		.chnNumMax = 12, \
		.chnNumMin2 = 32, \
		.chnNumMax2 = 63, \
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
		.chnF0Hz = 902800000, \
		.chnSpaHz = 800000, \
		.chnNumMin = 0, \
		.chnNumMax = 5, \
		.chnNumMin2 = 16, \
		.chnNumMax2 = 30, \
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
		.chnF0Hz = 903200000, \
		.chnSpaHz = 1200000, \
		.chnNumMin = 0, \
		.chnNumMax = 3, \
		.chnNumMin2 = 11, \
		.chnNumMax2 = 19, \
		.ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
		.ccaEdThresholdDBm = SUN_OFDM_OPT1_CCA_THRESHOLD, \
	}

/************************* Frequency band 915-928 MHz *************************/

/* OFDM Bandwidth Option 4 */
#define SUN_OFDM_BAND_915_928_OPT4              (DRV_RF215_PHY_CFG_OBJ) \
	{ \
		.phyType = PHY_TYPE_OFDM, \
		.phyTypeCfg.ofdm = { \
			.opt = OFDM_BW_OPT_4, \
			.itlv = OFDM_INTERLEAVING_1 \
		}, \
		.chnF0Hz = 902200000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 65, \
		.chnNumMax = 128, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902400000, \
		.chnSpaHz = 400000, \
		.chnNumMin = 32, \
		.chnNumMax = 63, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902800000, \
		.chnSpaHz = 800000, \
		.chnNumMin = 16, \
		.chnNumMax = 30, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 903200000, \
		.chnSpaHz = 1200000, \
		.chnNumMin = 11, \
		.chnNumMax = 19, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902200000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 65, \
		.chnNumMax = 93, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902400000, \
		.chnSpaHz = 400000, \
		.chnNumMin = 32, \
		.chnNumMax = 46, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902200000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 65, \
		.chnNumMax = 78, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902400000, \
		.chnSpaHz = 400000, \
		.chnNumMin = 32, \
		.chnNumMax = 38, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902800000, \
		.chnSpaHz = 800000, \
		.chnNumMin = 16, \
		.chnNumMax = 18, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 903200000, \
		.chnSpaHz = 1200000, \
		.chnNumMin = 11, \
		.chnNumMax = 11, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 917100000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 0, \
		.chnNumMax = 31, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 917300000, \
		.chnSpaHz = 400000, \
		.chnNumMin = 0, \
		.chnNumMax = 15, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 917500000, \
		.chnSpaHz = 800000, \
		.chnNumMin = 0, \
		.chnNumMax = 7, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 917900000, \
		.chnSpaHz = 1200000, \
		.chnNumMin = 0, \
		.chnNumMax = 4, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902200000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 85, \
		.chnNumMax = 103, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902400000, \
		.chnSpaHz = 400000, \
		.chnNumMin = 42, \
		.chnNumMax = 51, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902800000, \
		.chnSpaHz = 800000, \
		.chnNumMin = 21, \
		.chnNumMax = 24, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 903200000, \
		.chnSpaHz = 1200000, \
		.chnNumMin = 14, \
		.chnNumMax = 16, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 920200000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 0, \
		.chnNumMax = 38, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 920400000, \
		.chnSpaHz = 400000, \
		.chnNumMin = 0, \
		.chnNumMax = 18, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 920800000, \
		.chnSpaHz = 800000, \
		.chnNumMin = 0, \
		.chnNumMax = 8, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 921200000, \
		.chnSpaHz = 1200000, \
		.chnNumMin = 0, \
		.chnNumMax = 4, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902200000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 92, \
		.chnNumMax = 111, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902200000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 90, \
		.chnNumMax = 113, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902400000, \
		.chnSpaHz = 400000, \
		.chnNumMin = 45, \
		.chnNumMax = 56, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902800000, \
		.chnSpaHz = 800000, \
		.chnNumMin = 22, \
		.chnNumMax = 27, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 903200000, \
		.chnSpaHz = 1200000, \
		.chnNumMin = 15, \
		.chnNumMax = 17, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902200000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 90, \
		.chnNumMax = 103, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902400000, \
		.chnSpaHz = 400000, \
		.chnNumMin = 45, \
		.chnNumMax = 51, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902800000, \
		.chnSpaHz = 800000, \
		.chnNumMin = 22, \
		.chnNumMax = 24, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 903200000, \
		.chnSpaHz = 1200000, \
		.chnNumMin = 15, \
		.chnNumMax = 16, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 2400200000,	\
		.chnSpaHz = 200000, \
		.chnNumMin = 0, \
		.chnNumMax = 415, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 2400400000,	\
		.chnSpaHz = 400000, \
		.chnNumMin = 0, \
		.chnNumMax = 206, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 2400800000,	\
		.chnSpaHz = 800000, \
		.chnNumMin = 0, \
		.chnNumMax = 96, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 2401200000,	\
		.chnSpaHz = 1200000, \
		.chnNumMin = 0, \
		.chnNumMax = 63, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
		.ccaEdDurationUS = SUN_OFDM_CCA_ED_DURATION, \
		.ccaEdThresholdDBm = SUN_OFDM_OPT1_CCA_THRESHOLD, \
	}

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* _IEEE_15_4_SUN_OFDM_H */
