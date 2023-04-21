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

#ifndef _IEEE_15_4_SUN_FSK_H
#define _IEEE_15_4_SUN_FSK_H

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
			.symRate = FSK_SYM_RATE_50kHz,	\
			.modIdx = FSK_MOD_IDX_1_0, \
			.modOrd = FSK_MOD_ORD_2FSK \
		}, \
		.chnF0Hz = 470200000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 0, \
		.chnNumMax = 198, \
		.chnNumMin2 = 0xFFFF, \
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
		.chnF0Hz = 470200000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 0, \
		.chnNumMax = 198, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 470200000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 0, \
		.chnNumMax = 198, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
		.ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
		.ccaEdThresholdDBm = SUN_FSK_150kHz_CCA_THRESHOLD, \
	}

/************************* Frequency band 779-787 MHz *************************/

/* Operating mode #1 */
#define SUN_FSK_BAND_779_787_OPM1               (DRV_RF215_PHY_CFG_OBJ) \
	{ \
		.phyType = PHY_TYPE_FSK, \
		.phyTypeCfg.fsk = { \
			.symRate = FSK_SYM_RATE_50kHz,	\
			.modIdx = FSK_MOD_IDX_1_0, \
			.modOrd = FSK_MOD_ORD_2FSK \
		}, \
		.chnF0Hz = 779200000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 0, \
		.chnNumMax = 38, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 779400000, \
		.chnSpaHz = 400000, \
		.chnNumMin = 0, \
		.chnNumMax = 18, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 779400000, \
		.chnSpaHz = 400000, \
		.chnNumMin = 0, \
		.chnNumMax = 18, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
		.ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
		.ccaEdThresholdDBm = SUN_FSK_100kHz_CCA_THRESHOLD + 3, \
	}

/************* Frequency band 863-870 MHz (RF-PLC Hybrid default) *************/

/* Operating mode #1 */
#define SUN_FSK_BAND_863_870_OPM1               (DRV_RF215_PHY_CFG_OBJ) \
	{ \
		.phyType = PHY_TYPE_FSK, \
		.phyTypeCfg.fsk = { \
			.symRate = FSK_SYM_RATE_50kHz,	\
			.modIdx = FSK_MOD_IDX_0_5, \
			.modOrd = FSK_MOD_ORD_2FSK \
		}, \
		.chnF0Hz = 863100000, \
		.chnSpaHz = 100000, \
		.chnNumMin = 0, \
		.chnNumMax = 68, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 863100000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 0, \
		.chnNumMax = 34, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 863100000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 0, \
		.chnNumMax = 34, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
		.ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
		.ccaEdThresholdDBm = SUN_FSK_150kHz_CCA_THRESHOLD, \
	}

/************************* Frequency band 865-867 MHz *************************/

/* Operating mode #1 */
#define SUN_FSK_BAND_865_867_OPM1               (DRV_RF215_PHY_CFG_OBJ) \
	{ \
		.phyType = PHY_TYPE_FSK, \
		.phyTypeCfg.fsk = { \
			.symRate = FSK_SYM_RATE_50kHz,	\
			.modIdx = FSK_MOD_IDX_0_5, \
			.modOrd = FSK_MOD_ORD_2FSK \
		}, \
		.chnF0Hz = 865100000, \
		.chnSpaHz = 100000, \
		.chnNumMin = 0, \
		.chnNumMax = 18, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 865100000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 0, \
		.chnNumMax = 9, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 865100000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 0, \
		.chnNumMax = 9, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
		.ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
		.ccaEdThresholdDBm = SUN_FSK_150kHz_CCA_THRESHOLD, \
	}

/************************* Frequency band 866-869 MHz *************************/

/* Operating mode #1 */
#define SUN_FSK_BAND_866_869_OPM1               (DRV_RF215_PHY_CFG_OBJ) \
	{ \
		.phyType = PHY_TYPE_FSK, \
		.phyTypeCfg.fsk = { \
			.symRate = FSK_SYM_RATE_50kHz,	\
			.modIdx = FSK_MOD_IDX_0_5, \
			.modOrd = FSK_MOD_ORD_2FSK \
		}, \
		.chnF0Hz = 863100000, \
		.chnSpaHz = 100000, \
		.chnNumMin = 30, \
		.chnNumMax = 58, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 863100000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 15, \
		.chnNumMax = 29, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 863100000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 15, \
		.chnNumMax = 29, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 863200000, \
		.chnSpaHz = 400000, \
		.chnNumMin = 8, \
		.chnNumMax = 14, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 863200000, \
		.chnSpaHz = 400000, \
		.chnNumMin = 8, \
		.chnNumMax = 14, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
		.ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
		.ccaEdThresholdDBm = SUN_FSK_300kHz_CCA_THRESHOLD, \
	}

/************************* Frequency band 870-876 MHz *************************/

/* Operating mode #1 */
#define SUN_FSK_BAND_870_876_OPM1               (DRV_RF215_PHY_CFG_OBJ) \
	{ \
		.phyType = PHY_TYPE_FSK, \
		.phyTypeCfg.fsk = { \
			.symRate = FSK_SYM_RATE_50kHz,	\
			.modIdx = FSK_MOD_IDX_0_5, \
			.modOrd = FSK_MOD_ORD_2FSK \
		}, \
		.chnF0Hz = 870100000, \
		.chnSpaHz = 100000, \
		.chnNumMin = 0, \
		.chnNumMax = 58, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 870200000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 0, \
		.chnNumMax = 29, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 870200000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 0, \
		.chnNumMax = 29, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
		.ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
		.ccaEdThresholdDBm = SUN_FSK_150kHz_CCA_THRESHOLD, \
	}

/************************* Frequency band 902-928 MHz *************************/

/* Operating mode #1 */
#define SUN_FSK_BAND_902_928_OPM1               (DRV_RF215_PHY_CFG_OBJ) \
	{ \
		.phyType = PHY_TYPE_FSK, \
		.phyTypeCfg.fsk = { \
			.symRate = FSK_SYM_RATE_50kHz,	\
			.modIdx = FSK_MOD_IDX_1_0, \
			.modOrd = FSK_MOD_ORD_2FSK \
		}, \
		.chnF0Hz = 902200000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 0, \
		.chnNumMax = 128, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902400000, \
		.chnSpaHz = 400000, \
		.chnNumMin = 0, \
		.chnNumMax = 63, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902400000, \
		.chnSpaHz = 400000, \
		.chnNumMin = 0, \
		.chnNumMax = 63, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
		.ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
		.ccaEdThresholdDBm = SUN_FSK_200kHz_CCA_THRESHOLD, \
	}

/******************* Frequency band 902-928 MHz (alternate) *******************/

/* Operating mode #1 */
#define SUN_FSK_BAND_902_928_ALT_OPM1           (DRV_RF215_PHY_CFG_OBJ) \
	{ \
		.phyType = PHY_TYPE_FSK, \
		.phyTypeCfg.fsk = { \
			.symRate = FSK_SYM_RATE_50kHz,	\
			.modIdx = FSK_MOD_IDX_1_0, \
			.modOrd = FSK_MOD_ORD_2FSK \
		}, \
		.chnF0Hz = 902200000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 0, \
		.chnNumMax = 128, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902200000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 0, \
		.chnNumMax = 128, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902200000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 0, \
		.chnNumMax = 128, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902400000, \
		.chnSpaHz = 400000, \
		.chnNumMin = 0, \
		.chnNumMax = 63, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902400000, \
		.chnSpaHz = 400000, \
		.chnNumMin = 0, \
		.chnNumMax = 63, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
		.ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
		.ccaEdThresholdDBm = SUN_FSK_300kHz_CCA_THRESHOLD, \
	}

/******************* Frequency band 902-907.5 & 915-928 MHz *******************/

/* Operating mode #1 */
#define SUN_FSK_BAND_902_907_915_928_OPM1           (DRV_RF215_PHY_CFG_OBJ) \
	{ \
		.phyType = PHY_TYPE_FSK, \
		.phyTypeCfg.fsk = { \
			.symRate = FSK_SYM_RATE_50kHz,	\
			.modIdx = FSK_MOD_IDX_1_0, \
			.modOrd = FSK_MOD_ORD_2FSK \
		}, \
		.chnF0Hz = 902200000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 0, \
		.chnNumMax = 26, \
		.chnNumMin2 = 65, \
		.chnNumMax2 = 128, \
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
		.chnF0Hz = 902200000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 0, \
		.chnNumMax = 26, \
		.chnNumMin2 = 65, \
		.chnNumMax2 = 128, \
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
		.chnF0Hz = 902200000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 0, \
		.chnNumMax = 26, \
		.chnNumMin2 = 65, \
		.chnNumMax2 = 128, \
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
		.chnF0Hz = 902400000, \
		.chnSpaHz = 400000, \
		.chnNumMin = 0, \
		.chnNumMax = 12, \
		.chnNumMin2 = 32, \
		.chnNumMax2 = 63, \
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
		.chnF0Hz = 902400000, \
		.chnSpaHz = 400000, \
		.chnNumMin = 0, \
		.chnNumMax = 12, \
		.chnNumMin2 = 32, \
		.chnNumMax2 = 63, \
		.ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
		.ccaEdThresholdDBm = SUN_FSK_300kHz_CCA_THRESHOLD, \
	}

/************************* Frequency band 915-928 MHz *************************/

/* Operating mode #1 */
#define SUN_FSK_BAND_915_928_OPM1               (DRV_RF215_PHY_CFG_OBJ) \
	{ \
		.phyType = PHY_TYPE_FSK, \
		.phyTypeCfg.fsk = { \
			.symRate = FSK_SYM_RATE_50kHz,	\
			.modIdx = FSK_MOD_IDX_1_0, \
			.modOrd = FSK_MOD_ORD_2FSK \
		}, \
		.chnF0Hz = 902200000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 65, \
		.chnNumMax = 128, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902200000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 65, \
		.chnNumMax = 128, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902200000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 65, \
		.chnNumMax = 128, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902400000, \
		.chnSpaHz = 400000, \
		.chnNumMin = 32, \
		.chnNumMax = 63, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902400000, \
		.chnSpaHz = 400000, \
		.chnNumMin = 32, \
		.chnNumMax = 63, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
		.ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
		.ccaEdThresholdDBm = SUN_FSK_300kHz_CCA_THRESHOLD, \
	}

/************************* Frequency band 915-921 MHz *************************/

/* Operating mode #1 */
#define SUN_FSK_BAND_915_921_OPM1               (DRV_RF215_PHY_CFG_OBJ) \
	{ \
		.phyType = PHY_TYPE_FSK, \
		.phyTypeCfg.fsk = { \
			.symRate = FSK_SYM_RATE_50kHz,	\
			.modIdx = FSK_MOD_IDX_1_0, \
			.modOrd = FSK_MOD_ORD_2FSK \
		}, \
		.chnF0Hz = 902200000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 65, \
		.chnNumMax = 93, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902200000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 65, \
		.chnNumMax = 93, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902200000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 65, \
		.chnNumMax = 93, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902400000, \
		.chnSpaHz = 400000, \
		.chnNumMin = 32, \
		.chnNumMax = 46, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902400000, \
		.chnSpaHz = 400000, \
		.chnNumMin = 32, \
		.chnNumMax = 46, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
		.ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
		.ccaEdThresholdDBm = SUN_FSK_300kHz_CCA_THRESHOLD, \
	}

/************************* Frequency band 915-918 MHz *************************/

/* Operating mode #1 */
#define SUN_FSK_BAND_915_918_OPM1               (DRV_RF215_PHY_CFG_OBJ) \
	{ \
		.phyType = PHY_TYPE_FSK, \
		.phyTypeCfg.fsk = { \
			.symRate = FSK_SYM_RATE_50kHz,	\
			.modIdx = FSK_MOD_IDX_1_0, \
			.modOrd = FSK_MOD_ORD_2FSK \
		}, \
		.chnF0Hz = 902200000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 65, \
		.chnNumMax = 78, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902200000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 65, \
		.chnNumMax = 78, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902200000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 65, \
		.chnNumMax = 78, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902400000, \
		.chnSpaHz = 400000, \
		.chnNumMin = 32, \
		.chnNumMax = 38, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902400000, \
		.chnSpaHz = 400000, \
		.chnNumMin = 32, \
		.chnNumMax = 38, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
		.ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
		.ccaEdThresholdDBm = SUN_FSK_300kHz_CCA_THRESHOLD, \
	}

/************************ Frequency band 917-923.5 MHz ************************/

/* Operating mode #1 */
#define SUN_FSK_BAND_917_923_OPM1               (DRV_RF215_PHY_CFG_OBJ) \
	{ \
		.phyType = PHY_TYPE_FSK, \
		.phyTypeCfg.fsk = { \
			.symRate = FSK_SYM_RATE_50kHz,	\
			.modIdx = FSK_MOD_IDX_1_0, \
			.modOrd = FSK_MOD_ORD_2FSK \
		}, \
		.chnF0Hz = 917100000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 0, \
		.chnNumMax = 31, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 917300000, \
		.chnSpaHz = 400000, \
		.chnNumMin = 0, \
		.chnNumMax = 15, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 917300000, \
		.chnSpaHz = 400000, \
		.chnNumMin = 0, \
		.chnNumMax = 15, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
		.ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
		.ccaEdThresholdDBm = SUN_FSK_200kHz_CCA_THRESHOLD, \
	}

/************************* Frequency band 919-923 MHz *************************/

/* Operating mode #1 */
#define SUN_FSK_BAND_919_923_OPM1               (DRV_RF215_PHY_CFG_OBJ) \
	{ \
		.phyType = PHY_TYPE_FSK, \
		.phyTypeCfg.fsk = { \
			.symRate = FSK_SYM_RATE_50kHz,	\
			.modIdx = FSK_MOD_IDX_1_0, \
			.modOrd = FSK_MOD_ORD_2FSK \
		}, \
		.chnF0Hz = 902200000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 85, \
		.chnNumMax = 103, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902200000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 85, \
		.chnNumMax = 103, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902200000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 85, \
		.chnNumMax = 103, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902400000, \
		.chnSpaHz = 400000, \
		.chnNumMin = 42, \
		.chnNumMax = 51, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902400000, \
		.chnSpaHz = 400000, \
		.chnNumMin = 42, \
		.chnNumMax = 51, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
		.ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
		.ccaEdThresholdDBm = SUN_FSK_300kHz_CCA_THRESHOLD, \
	}

/************************* Frequency band 920-928 MHz *************************/

/* Operating mode #1 */
#define SUN_FSK_BAND_920_928_OPM1               (DRV_RF215_PHY_CFG_OBJ) \
	{ \
		.phyType = PHY_TYPE_FSK, \
		.phyTypeCfg.fsk = { \
			.symRate = FSK_SYM_RATE_50kHz,	\
			.modIdx = FSK_MOD_IDX_1_0, \
			.modOrd = FSK_MOD_ORD_2FSK \
		}, \
		.chnF0Hz = 920600000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 0, \
		.chnNumMax = 36, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 920900000, \
		.chnSpaHz = 400000, \
		.chnNumMin = 0, \
		.chnNumMax = 17, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 920800000, \
		.chnSpaHz = 600000, \
		.chnNumMin = 0, \
		.chnNumMax = 11, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 920800000, \
		.chnSpaHz = 600000, \
		.chnNumMin = 0, \
		.chnNumMax = 11, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 920900000, \
		.chnSpaHz = 400000, \
		.chnNumMin = 0, \
		.chnNumMax = 17, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 920900000, \
		.chnSpaHz = 400000, \
		.chnNumMin = 0, \
		.chnNumMax = 17, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 920900000, \
		.chnSpaHz = 400000, \
		.chnNumMin = 0, \
		.chnNumMax = 17, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 920800000, \
		.chnSpaHz = 600000, \
		.chnNumMin = 0, \
		.chnNumMax = 11, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 921000000, \
		.chnSpaHz = 1000000, \
		.chnNumMin = 0, \
		.chnNumMax = 6, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 921000000, \
		.chnSpaHz = 1000000, \
		.chnNumMin = 0, \
		.chnNumMax = 6, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
		.ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
		.ccaEdThresholdDBm = SUN_FSK_400kHz_CCA_THRESHOLD + 3, \
	}

/*********************** Frequency band 920.5-924.5MHz ************************/

/* Operating mode #1 */
#define SUN_FSK_BAND_920_924_OPM1               (DRV_RF215_PHY_CFG_OBJ) \
	{ \
		.phyType = PHY_TYPE_FSK, \
		.phyTypeCfg.fsk = { \
			.symRate = FSK_SYM_RATE_50kHz,	\
			.modIdx = FSK_MOD_IDX_1_0, \
			.modOrd = FSK_MOD_ORD_2FSK \
		}, \
		.chnF0Hz = 902200000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 92, \
		.chnNumMax = 111, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902200000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 92, \
		.chnNumMax = 111, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902200000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 92, \
		.chnNumMax = 111, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
		.ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
		.ccaEdThresholdDBm = SUN_FSK_150kHz_CCA_THRESHOLD, \
	}

/************************* Frequency band 920-925 MHz *************************/

/* Operating mode #1 */
#define SUN_FSK_BAND_920_925_OPM1               (DRV_RF215_PHY_CFG_OBJ) \
	{ \
		.phyType = PHY_TYPE_FSK, \
		.phyTypeCfg.fsk = { \
			.symRate = FSK_SYM_RATE_50kHz,	\
			.modIdx = FSK_MOD_IDX_1_0, \
			.modOrd = FSK_MOD_ORD_2FSK \
		}, \
		.chnF0Hz = 902200000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 90, \
		.chnNumMax = 113, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902200000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 90, \
		.chnNumMax = 113, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902200000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 90, \
		.chnNumMax = 113, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902400000, \
		.chnSpaHz = 400000, \
		.chnNumMin = 45, \
		.chnNumMax = 56, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902400000, \
		.chnSpaHz = 400000, \
		.chnNumMin = 45, \
		.chnNumMax = 56, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
		.ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
		.ccaEdThresholdDBm = SUN_FSK_300kHz_CCA_THRESHOLD, \
	}

/************************* Frequency band 920-923 MHz *************************/

/* Operating mode #1 */
#define SUN_FSK_BAND_920_923_OPM1               (DRV_RF215_PHY_CFG_OBJ) \
	{ \
		.phyType = PHY_TYPE_FSK, \
		.phyTypeCfg.fsk = { \
			.symRate = FSK_SYM_RATE_50kHz,	\
			.modIdx = FSK_MOD_IDX_1_0, \
			.modOrd = FSK_MOD_ORD_2FSK \
		}, \
		.chnF0Hz = 902200000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 90, \
		.chnNumMax = 103, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902200000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 90, \
		.chnNumMax = 103, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902200000, \
		.chnSpaHz = 200000, \
		.chnNumMin = 90, \
		.chnNumMax = 103, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902400000, \
		.chnSpaHz = 400000, \
		.chnNumMin = 45, \
		.chnNumMax = 51, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 902400000, \
		.chnSpaHz = 400000, \
		.chnNumMin = 45, \
		.chnNumMax = 51, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
		.ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
		.ccaEdThresholdDBm = SUN_FSK_300kHz_CCA_THRESHOLD, \
	}

/************************* Frequency band 2400-2483.5 MHz *********************/

/* Operating mode #1 */
#define SUN_FSK_BAND_2400_2483_OPM1               (DRV_RF215_PHY_CFG_OBJ) \
	{ \
		.phyType = PHY_TYPE_FSK, \
		.phyTypeCfg.fsk = { \
			.symRate = FSK_SYM_RATE_50kHz,	\
			.modIdx = FSK_MOD_IDX_1_0, \
			.modOrd = FSK_MOD_ORD_2FSK \
		}, \
		.chnF0Hz = 2400200000,	\
		.chnSpaHz = 200000, \
		.chnNumMin = 0, \
		.chnNumMax = 415, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 2400400000,	\
		.chnSpaHz = 400000, \
		.chnNumMin = 0, \
		.chnNumMax = 206, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
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
		.chnF0Hz = 2400400000,	\
		.chnSpaHz = 400000, \
		.chnNumMin = 0, \
		.chnNumMax = 206, \
		.chnNumMin2 = 0xFFFF, \
		.chnNumMax2 = 0, \
		.ccaEdDurationUS = SUN_FSK_50kHz_CCA_ED_DURATION, \
		.ccaEdThresholdDBm = SUN_FSK_200kHz_CCA_THRESHOLD, \
	}

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* _IEEE_15_4_SUN_FSK_H */
