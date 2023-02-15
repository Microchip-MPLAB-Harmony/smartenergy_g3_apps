/******************************************************************************
  RF215 Driver PHY Layer Implementation.

  Company:
    Microchip Technology Inc.

  File Name:
    rf215_phy.c

  Summary:
    Source code for the RF215 Driver PHY Layer implementation.

  Description:
    The RF215 driver PHY manages the different modules of the RF215 Physical
    Layer (PLL, Front-end, Baseband Core) of the RF215. This file provides
    source code for the implementation of the RF215 driver PHY.
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

// *****************************************************************************
// *****************************************************************************
// Section: Include Files
// *****************************************************************************
// *****************************************************************************
#include <stddef.h>
#include <string.h>
#include "driver/rf215/phy/rf215_phy.h"
#include "driver/rf215/phy/ieee_15_4_sun_fsk.h"
#include "driver/rf215/phy/ieee_15_4_sun_ofdm.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Constant Data
// *****************************************************************************
// *****************************************************************************

/* RF215 register base addresses */
const RF215_REG_BASE_ADDR rf215RegBaseAddr[2] = {
    {
        .RFn = RF215_BASE_ADDR_RF09,
        .BBCn = RF215_BASE_ADDR_BBC0,
        .frameBufBBCn = RF215_BASE_ADDR_FRAME_BUF_BBC0,
    },
    {
        .RFn = RF215_BASE_ADDR_RF24,
        .BBCn = RF215_BASE_ADDR_BBC1,
        .frameBufBBCn = RF215_BASE_ADDR_FRAME_BUF_BBC1,
    }
};

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Constant Data
// *****************************************************************************
// *****************************************************************************

/* RF215 PLL constants */
static const RF215_PLL_CONST_OBJ rf215PllConst[DRV_RF215_NUM_TRX] = {
    /* Sub-GHz Transceiver */
    {
        .freqRanges = {
            {
                .freqMin = PLL_FREQ_MIN_RF09_RNG1_Hz,
                .freqMax = PLL_FREQ_MAX_RF09_RNG1_Hz,
            },
            {
                .freqMin = PLL_FREQ_MIN_RF09_RNG2_Hz,
                .freqMax = PLL_FREQ_MAX_RF09_RNG2_Hz,
            }
        },

        .fineFreqRes = {
            PLL_FINE_FREQ_RES_RF09_RNG1_Hz,
            PLL_FINE_FREQ_RES_RF09_RNG2_Hz
        },

        .fineFreqOffset = {
            PLL_FINE_FREQ_OFFSET_RF09_RNG1_Hz,
            PLL_FINE_FREQ_OFFSET_RF09_RNG2_Hz
        },

        .ieeeFreqOffset = PLL_IEEE_FREQ_OFFSET09_Hz,
        .fskTolT0 = PLL_DELTA_FSK_T0_RF09_Q45,
        .fineChnMode = {RF215_RFn_CNM_CM_FINE_389, RF215_RFn_CNM_CM_FINE_779},
        .numFreqRanges = 2
    },

    /* 2.4 GHz Transceiver */
    {
        .freqRanges = {
            {
                .freqMin = PLL_FREQ_MIN_RF24_RNG3_Hz,
                .freqMax = PLL_FREQ_MAX_RF24_RNG3_Hz,
            },
            {0}
        },

        .fineFreqRes = {PLL_FINE_FREQ_RES_RF24_RNG3_Hz, 0},
        .fineFreqOffset = {PLL_FINE_FREQ_OFFSET_RF24_RNG3_Hz, 0},
        .ieeeFreqOffset = PLL_IEEE_FREQ_OFFSET24_Hz,
        .fskTolT0 = PLL_DELTA_FSK_T0_RF24_Q45,
        .fineChnMode = {RF215_RFn_CNM_CM_FINE_2400, 0},
        .numFreqRanges = 1
    },

};

/* RF215 FSK constants for each symbol rate */
static const RF215_FSK_SYM_RATE_CONST_OBJ fskSymRateConst[6] = {
    /* 50 kHz */
    {
        .Hz = 50000,
        .kHz = 50,
        .txBaseBandDelayUSq5 = 1344, /* 42.0 us */
        .txPreEmphasisDelay1USq5 = 640, /* 20.0 us */
        .txPreEmphasisDelay2USq5 = 248, /* 7.75 us */
        .rxBaseBandDelayUSq5 = 12, /* 0.375 us */
        .RFn_RXDFE_SR = RF215_RFn_RXDFE_SR_400kHz,
        .RFn_TXDFE_SR = RF215_RFn_TXDFE_SR_500kHz,
        .RFn_TXCUT_PARAMP = RF215_RFn_TXCUTC_PARAMP_32us,
        .BBCn_FSKPE0 = 0x02,
        .BBCn_FSKPE1 = 0x03,
        .BBCn_FSKPE2 = 0xFC
    },

    /* 100 kHz */
    {
        .Hz = 100000,
        .kHz = 100,
        .txBaseBandDelayUSq5 = 672, /* 21.0 us */
        .txPreEmphasisDelay1USq5 = 328, /* 10.25 us */
        .txPreEmphasisDelay2USq5 = 112, /* 3.5 us */
        .rxBaseBandDelayUSq5 = 68, /* 2.125 us */
        .RFn_RXDFE_SR = RF215_RFn_RXDFE_SR_800kHz,
        .RFn_TXDFE_SR = RF215_RFn_TXDFE_SR_1000kHz,
        .RFn_TXCUT_PARAMP = RF215_RFn_TXCUTC_PARAMP_16us,
        .BBCn_FSKPE0 = 0x0E,
        .BBCn_FSKPE1 = 0x0F,
        .BBCn_FSKPE2 = 0xF0
    },

    /* 150 kHz */
    {
        .Hz = 150000,
        .kHz = 150,
        .txBaseBandDelayUSq5 = 608, /* 19.0 us */
        .txPreEmphasisDelay1USq5 = 184, /* 5.75 us */
        .txPreEmphasisDelay2USq5 = 100, /* 3.125 us */
        .rxBaseBandDelayUSq5 = 212, /* 6.625 us */
        .RFn_RXDFE_SR = RF215_RFn_RXDFE_SR_1000kHz,
        .RFn_TXDFE_SR = RF215_RFn_TXDFE_SR_2000kHz,
        .RFn_TXCUT_PARAMP = RF215_RFn_TXCUTC_PARAMP_16us,
        .BBCn_FSKPE0 = 0x3C,
        .BBCn_FSKPE1 = 0x3F,
        .BBCn_FSKPE2 = 0xC0
    },

    /* 200 kHz */
    {
        .Hz = 200000,
        .kHz = 200,
        .txBaseBandDelayUSq5 = 352, /* 11.0 us */
        .txPreEmphasisDelay1USq5 = 176, /* 5.5 us */
        .txPreEmphasisDelay2USq5 = 32, /* 1.0 us */
        .rxBaseBandDelayUSq5 = 48, /* 1.5 us */
        .RFn_RXDFE_SR = RF215_RFn_RXDFE_SR_1000kHz,
        .RFn_TXDFE_SR = RF215_RFn_TXDFE_SR_2000kHz,
        .RFn_TXCUT_PARAMP = RF215_RFn_TXCUTC_PARAMP_16us,
        .BBCn_FSKPE0 = 0x74,
        .BBCn_FSKPE1 = 0x7F,
        .BBCn_FSKPE2 = 0x80
    },

    /* 300 kHz */
    {
        .Hz = 300000,
        .kHz = 300,
        .txBaseBandDelayUSq5 = 304, /* 9.5 us */
        .txPreEmphasisDelay1USq5 = 88, /* 2.75 us */
        .txPreEmphasisDelay2USq5 = 76, /* 2.375 us */
        .rxBaseBandDelayUSq5 = 124, /* 3.875 us */
        .RFn_RXDFE_SR = RF215_RFn_RXDFE_SR_2000kHz,
        .RFn_TXDFE_SR = RF215_RFn_TXDFE_SR_4000kHz,
        .RFn_TXCUT_PARAMP = RF215_RFn_TXCUTC_PARAMP_8us,
        .BBCn_FSKPE0 = 0x05,
        .BBCn_FSKPE1 = 0x3C,
        .BBCn_FSKPE2 = 0xC3
    },

    /* 400 kHz */
    {
        .Hz = 400000,
        .kHz = 400,
        .txBaseBandDelayUSq5 = 176, /* 5.5 us */
        .txPreEmphasisDelay1USq5 = 88, /* 2.75 us */
        .txPreEmphasisDelay2USq5 = 20, /* 0.675 us */
        .rxBaseBandDelayUSq5 = 20, /* 0.625 us */
        .RFn_RXDFE_SR = RF215_RFn_RXDFE_SR_2000kHz,
        .RFn_TXDFE_SR = RF215_RFn_TXDFE_SR_4000kHz,
        .RFn_TXCUT_PARAMP = RF215_RFn_TXCUTC_PARAMP_8us,
        .BBCn_FSKPE0 = 0x13,
        .BBCn_FSKPE1 = 0x29,
        .BBCn_FSKPE2 = 0xC7
    }
};

/* RF215 OFDM constants for each bandwidth option */
static const RF215_OFDM_BW_OPT_CONST_OBJ ofdmBwOptConst[4] = {
    /* Option 1 */
    {
        .bwHz = 1094000,
        .txBaseBandDelayUSq5 = 16, /* 0.5 us */
        .rxBaseBandDelayUSq5 = {2936, 3112}, /* 91.75, 97.25 us */
        .phrSymbols = {3, 4},
        .dataCarriers = 96,
        .RFn_RXDFE_SR = RF215_RFn_RXDFE_SR_1333kHz,
        .RFn_TXDFE_SR = RF215_RFn_TXDFE_SR_1333kHz,
        /* Higher threshold than in Table 6-93 to avoid false detections */
        .BBCn_OFDMSW_PDT = RF215_BBCn_OFDMSW_PDT(6),
        .minMCS = OFDM_MCS_0
    },

    /* Option 2 */
    {
        .bwHz = 552000,
        .txBaseBandDelayUSq5 = 16, /* 0.5 us */
        .rxBaseBandDelayUSq5 = {1988, 2030}, /* 62.125, 63.44 us */
        .phrSymbols = {6, 8},
        .dataCarriers = 48,
        .RFn_RXDFE_SR = RF215_RFn_RXDFE_SR_1333kHz,
        .RFn_TXDFE_SR = RF215_RFn_TXDFE_SR_1333kHz,
        .BBCn_OFDMSW_PDT = RF215_BBCn_OFDMSW_PDT(5),
        .minMCS = OFDM_MCS_0
    },

    /* Option 3 */
    {
        .bwHz = 281000,
        .txBaseBandDelayUSq5 = 32, /* 1 us */
        .rxBaseBandDelayUSq5 = {1677, 1720}, /* 52.41, 53.75 us */
        .phrSymbols = {6, 6},
        .dataCarriers = 24,
        .RFn_RXDFE_SR = RF215_RFn_RXDFE_SR_667kHz,
        .RFn_TXDFE_SR = RF215_RFn_TXDFE_SR_667kHz,
        .BBCn_OFDMSW_PDT = RF215_BBCn_OFDMSW_PDT(4),
        .minMCS = OFDM_MCS_1 /* MCS0 not available in option 3 */
    },

    /* Option 4 */
    {
        .bwHz = 156000,
        .txBaseBandDelayUSq5 = 72, /* 2.25 us */
        .rxBaseBandDelayUSq5 = {1596, 1640}, /* 49.875, 51.25 us */
        .phrSymbols = {6, 6},
        .dataCarriers = 12,
        .RFn_RXDFE_SR = RF215_RFn_RXDFE_SR_667kHz,
        .RFn_TXDFE_SR = RF215_RFn_TXDFE_SR_667kHz,
        /* Higher threshold than in Table 6-93 to avoid false detections */
        .BBCn_OFDMSW_PDT = RF215_BBCn_OFDMSW_PDT(4),
        .minMCS = OFDM_MCS_2 /* MCS0 and MCS1 not available in option 4 */
    }
};

/* RF215 OFDM constants for each MCS */
static const RF215_OFDM_MCS_CONST_OBJ ofdmMcsConst[7] = {
    /* MCS0 */
    {
        .repFactorShift = 2,
        .bitsCarrierShift = 0,
        .minTxPwrAttMin = 0
    },

    /* MCS1 */
    {
        .repFactorShift = 1,
        .bitsCarrierShift = 0,
        .minTxPwrAttMin = 0
    },

    /* MCS2 */
    {
        .repFactorShift = 1,
        .bitsCarrierShift = 1,
        .minTxPwrAttMin = 0
    },

    /* MCS3 */
    {
        .repFactorShift = 0,
        .bitsCarrierShift = 1,
        .minTxPwrAttMin = 0
    },

    /* MCS4 */
    {
        .repFactorShift = 0,
        .bitsCarrierShift = 1,
        .minTxPwrAttMin = 1
    },

    /* MCS5 */
    {
        .repFactorShift = 0,
        .bitsCarrierShift = 2,
        .minTxPwrAttMin = 3
    },

    /* MCS6 */
    {
        .repFactorShift = 0,
        .bitsCarrierShift = 2,
        .minTxPwrAttMin = 5
    }
};

/* Transmitter Analog Front-end low-pass cut-off frequencies (TXCUTC.LPFCUT) or
 * Receiver Analog Front-end band-pass half bandwidth (RXBWC.BW) in Hz */
static const uint32_t rf215AfeCutOffFreq[12] = {80000, 100000, 125000, 160000, \
    200000, 250000, 315000, 400000, 500000, 625000, 800000, 1000000};

/* Transmitter front-end delay in us [uQ4.5] [Table 6-1] when TXDFE.RCUT=4.
 * It depends on TXDFE.SR (1, 2, 3, 4, 5, 6, 8, 10): 2.0, 4.0, 5.25, 6.25, 8.3,
 * 8.5, 10.25, 13.75 us */
static const uint16_t rf215TxDfeProcDelay[11] = {0, 64, 128, 168, 200, 266, 272,
    0, 328, 0, 440};

/* Transmitter front-end delay in us [uQ6.5] [Table 6-1] when TXDFE.RCUT!=4.
 * It depends on TXDFE.SR (1, 2, 3, 4, 5, 6, 8, 10): 4.0, 8.5, 11.0, 15.0, 18.5,
 * 20.5, 28.5, 34.0 us */
static const uint16_t rf215TxDfeProcRcutDelay[11] = {0, 128, 272, 352, 480, 592,
    656, 0, 912, 0, 1088};

/* Receiver front-end delay in us [uQ5.5] (not in data-sheet).
 * When RXDFE.RCUT!=4, it depends on RXDFE.SR (1, 2, 3, 4, 5, 6, 8, 10)
 * 1.75, 3.625, 5.625, 7.5, 8.75, 11.125, 14, 17.25 us */
static const uint16_t rf215RxDfeProcDelay[11] = {0, 56, 116, 180, 232, 280, \
    356, 0, 448, 0, 552};

/* AGC Update Time in us (ceiling) [Figure 6-7]
 * AGCC.AGCI=0: Ordered by TXDFE.SR (1, 2, 3, 4, 5, 6, 8, 10) [Table 6-12]
 * AGCC.AVGS=0: 8.75, 14.5, 21.75, 29, 36.25, 43.5, 48, 65 us
 * AGCC.AVGS=1: 10.75, 18.5, 27.75, 37, 46.25, 55.5, 74, 85 us
 * AGCC.AVGS=2: 14.75, 26.5, 39.75, 53, 66.25, 79.5, 106, 125 us
 * AGCC.AVGS=3: 22.75, 42.5, 63.75, 85, 106.25, 127.5, 170, 205 us */
static const uint8_t rf215AgcUpdTime0[11] = {0, 9, 15, 22, 29, 37, 44, 0, 48, \
    0, 65};

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Data
// *****************************************************************************
// *****************************************************************************

/* RF215 PHY objects */
static RF215_PHY_OBJ rf215PhyObj[DRV_RF215_NUM_TRX] = {0};

/* RX indication object and PSDU buffer used to notify receive indication */
static DRV_RF215_RX_INDICATION_OBJ rf215PhyRxInd;
static uint8_t rf215PhyRxPsdu[DRV_RF215_MAX_PSDU_LEN];

/* RF_IQIFC1 register */
static uint8_t rf215PhyRegRF_IQIFC1;

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Function Declarations
// *****************************************************************************
// *****************************************************************************

static void _RF215_TX_PrepareTimeExpired(uintptr_t context);
static DRV_RF215_PIB_RESULT _RF215_PHY_SetPhyConfig (
    uint8_t trxIdx,
    DRV_RF215_PHY_CFG_OBJ* phyCfgNew,
    uint16_t chnNumNew
);

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Functions
// *****************************************************************************
// *****************************************************************************

static inline bool _RF215_FSK_CheckConfig(DRV_RF215_FSK_CFG_OBJ* fskConfig)
{
    DRV_RF215_FSK_SYM_RATE symRate = fskConfig->symRate;
    DRV_RF215_FSK_MOD_IDX modIdx = fskConfig->modIdx;
    DRV_RF215_FSK_MOD_ORD modOrd = fskConfig->modOrd;

    if ((symRate > FSK_SYM_RATE_400kHz) || (symRate < FSK_SYM_RATE_50kHz))
    {
        /* Invalid symbol rate */
        return false;
    }

    if (modIdx < FSK_MOD_IDX_1_0)
    {
        /* Invalid modulation index */
        return false;
    }

    if (modOrd == FSK_MOD_ORD_2FSK)
    {
        /* 2-FSK: 1.0 and 0.5 modulation indexes supported */
        return (bool) (modIdx <= FSK_MOD_IDX_0_5);
    }
    else if (modOrd == FSK_MOD_ORD_4FSK)
    {
        /* 4-FSK: 1.0 modulation index supported */
        return (bool) (modIdx == FSK_MOD_IDX_1_0);
    }

    /* Invalid modulation order */
    return false;
}

static inline void _RF215_FSK_Regs (
    DRV_RF215_FSK_CFG_OBJ* fskCfg,
    RF215_PHY_REGS_OBJ* phyRegs
)
{
    uint8_t fskc0, fskc3;
    DRV_RF215_FSK_MOD_IDX modIdx = fskCfg->modIdx;
    DRV_RF215_FSK_SYM_RATE symRate = fskCfg->symRate;
    const RF215_FSK_SYM_RATE_CONST_OBJ* fskConst = &fskSymRateConst[symRate];

    /* FSKC0.BT: FSK Bandwidth Time Product = 2.0. SUN FSK does not specify
     * GFSK modulator, so set to the maximum value. Furthermore, we are
     * using direct modulation with pre-emphasis and BT is ignored in that
     * case (data-sheet 6.10.4.2)
     * FSKC0.MORD: FSK modulation order */
    fskc0 = RF215_BBCn_FSKC0_BT_2_0 | RF215_BBCn_FSKC0_MORD(fskCfg->modOrd);

    /* FSKC0.MIDX/MIDXS: FSK modulation index */
    if (modIdx == FSK_MOD_IDX_1_0)
    {
        fskc0 |= (RF215_BBCn_FSKC0_MIDX_1_0 | RF215_BBCn_FSKC0_MIDXS_1_0);
    }
    else
    {
        /* FSK_MOD_IDX_0_5 */
        fskc0 |= (RF215_BBCn_FSKC0_MIDX_0_5 | RF215_BBCn_FSKC0_MIDXS_1_0);
    }

    /* BBCn_FSKC0 */
    phyRegs->BBCn_FSKC0 = fskc0;

    /* BBCn_FSKC1
     * SRATE: FSK symbol rate
     * FSKPLH=0: Preamble length high byte (fixed to 8 octets)
     * FI=0: Sign of FSK deviation frequency not inverted */
    phyRegs->BBCn_FSKC1 = RF215_BBCn_FSKC1_SRATE(symRate);

    /* BBCn_FSKC2: Not needed to modify default register value.
     * FECIE=1: FEC interleaving enabled (NRNSC)
     * FECS=0: FEC scheme NRNSC (phyFskFecScheme = 0)
     * PRI=0: Preamble frequency deviation not inverted
     * MSE=0: Mode Switch disabled
     * RXPTO=0: Resynchronization if correlation is below threshold
     * RXO=2: Receiver restarted by >18dB stronger frame
     * PDTM=0: Enable only if preamble length is <8 (fixed to 8) */
    phyRegs->BBCn_FSKC2 = RF215_BBCn_FSKC2_Rst;

    /* FSKC3.SFDT: SFD Detection Threshold default value 8 */
    fskc3 = RF215_BBCn_FSKC3_SFDT(8);

    /* FSKC3.PDT: Preamble Detection Threshold */
    if ((modIdx == FSK_MOD_IDX_0_5) && (symRate >= FSK_SYM_RATE_150kHz))
    {
        /* Higher threshold to avoid false detections (spurious) */
        fskc3 |= RF215_BBCn_FSKC3_PDT(6);
    }
    else
    {
        /* Default value 5 */
        fskc3 |= RF215_BBCn_FSKC3_PDT(5);
    }

    /* BBCn_FSKC3 */
    phyRegs->BBCn_FSKC3 = fskc3;

    /* BBCn_FSKC4: Not needed to modify default register value.
     * CSFD0=0: SFD0 used for uncoded (FEC disabled) IEEE mode
     * CSFD1=2: SFD1 used for coded (FEC enabled) IEEE mode
     * RAWRBIT=1: RAW mode not used
     * SFD32=0: Dual SFD mode
     * SFDQ=0: Use soft decisions for SFD search */

    /* BBCn_FSKPLL: Not needed to modify default register value.
     * FSKPLL=8: Preamble length fixed to 8 octets */

    /* BBCn_FSKSFD0L/H: Not needed to modify default register value.
     * FSKSFD0L=0x09; FSKSFD0H=0x72: 802.15.4 SUN FSK SFD value for
     * uncoded format (phySunFskSfd=0)  */

    /* BBCn_FSKSFD1L/H: Not needed to modify default register value.
     * FSKSFD1L=0xF6; FSKSFD1H=0x72: 802.15.4 SUN FSK SFD value for
     * coded format (phySunFskSfd=0)  */

    /* BBCn_FSKPHRTX: Not needed to modify default register value.
     * RB1/2=0: PHR reserved bits set to 0
     * DW=1: Data withening enabled
     * SFD=0: SFD0 used for TX (uncoded, FEC disabled) */

    /* FSK Direct modulation and pre-emphashis settings. Recommended to enable
     * direct modulation and pre-emphasis filtering to improve the modulation
     * quality [Table 6-57].
     * BBCn_FSKDM: Enable direct modulation and pre-emphasis */
    phyRegs->BBCn_FSKDM = RF215_BBCn_FSKDM_EN | RF215_BBCn_FSKDM_PE;
    phyRegs->BBCn_FSKPE0 = fskConst->BBCn_FSKPE0;
    phyRegs->BBCn_FSKPE1 = fskConst->BBCn_FSKPE1;
    phyRegs->BBCn_FSKPE2 = fskConst->BBCn_FSKPE2;
}

static inline uint8_t _RF215_FSK_SymbolsPerOctet (
    DRV_RF215_FSK_CFG_OBJ* fskCfg,
    DRV_RF215_PHY_MOD_SCHEME modScheme
)
{
    uint8_t symbols = 8;

    if (modScheme == FSK_FEC_ON)
    {
        /* If FEC enabled, the number of symbols per octet is doubled */
        symbols <<= 1;
    }

    if (fskCfg->modOrd == FSK_MOD_ORD_4FSK)
    {
        /* In 4-FSK, one symbol carries 2 bits */
        symbols >>= 1;
    }

    return symbols;
}

static inline uint32_t _RF215_FSK_PpduDuration (
    DRV_RF215_FSK_CFG_OBJ* fskCfg,
    DRV_RF215_PHY_MOD_SCHEME modScheme,
    uint16_t psduLen,
    uint16_t* pSymbolsPayload
)
{
    uint32_t symbolsAux, durationUS;
    uint16_t symbolsTotal, symbolsPayload;
    uint8_t symbolsOctet;
    uint8_t tailPadOctets = 0;

    /* Compute number of FSK symbols per octet (PHR + Payload) */
    symbolsOctet = _RF215_FSK_SymbolsPerOctet(fskCfg, modScheme);

    /* Payload: PSDU + tail + padding. Tail + padding only if FEC enabled */
    if (modScheme == FSK_FEC_ON)
    {
        if ((psduLen & 1) != 0)
        {
            /* PSDU length odd: 3 tail bits + 5 padding bits */
            tailPadOctets = 1;
        }
        else
        {
            /* PSDU length even: 3 tail bits + 13 padding bits */
            tailPadOctets = 2;
        }
    }

    /* Compute frame duration in microseconds.
     * SHR (Preamble + SFD): Preamble fixed to 8 octets, SFD 2 octets. 8 symbols
     * per octet (not affected by modulation order and FEC).
     * PHR: 2 octets. Symbols depend on modulation order and FEC.
     * Payload: PSDU+tail+padding. Symbols depend on modulation order and FEC */
    symbolsPayload = (psduLen + tailPadOctets) * symbolsOctet;
    symbolsTotal = (10 << 3) + (symbolsOctet << 1) + symbolsPayload;
    symbolsAux = (uint32_t) symbolsTotal * 1000;
    *pSymbolsPayload = symbolsPayload;
    durationUS = DIV_ROUND(symbolsAux, fskSymRateConst[fskCfg->symRate].kHz);
    return SYS_TIME_USToCount(durationUS);
}

static inline DRV_RF215_PHY_MOD_SCHEME _RF215_FSK_ReadPHR(uint8_t phr)
{
    phr &= BBC_FSKPHRRX_MASK;
    if (phr == BBC_FSKPHRRX_FEC_OFF)
    {
        /* Valid PHR with SFD0 (uncoded) */
        return FSK_FEC_OFF;
    }
    else if (phr == BBC_FSKPHRRX_FEC_ON)
    {
        /* Valid PHR with SFD1 (coded) */
        return FSK_FEC_ON;
    }

    /* Invalid PHR */
    return MOD_SCHEME_INVALID;
}

static inline uint32_t _RF215_FSK_FreqTolQ45 (
    const RF215_PLL_CONST_OBJ* pllConst,
    RF215_PLL_PARAMS_OBJ* pllParams,
    DRV_RF215_FSK_CFG_OBJ* fskCfg
)
{
    /* FSK PHY: T<=min(50*10^-6, T0*R*h*F0/R0/h0/F) */
    uint64_t tolAuxQ45;

    /* Get T0, different for RF09 and RF24 */
    tolAuxQ45 = pllConst->fskTolT0;

    /* Multiply by R/R0 (R: FSK symbol rate; R0 = 50kHz)
     * Multiply by F0 (F0 = 915MHz)
     * Multiply by h/h0 (h: FSK modulation index; h0 = 1) */
    tolAuxQ45 *= (fskSymRateConst[fskCfg->symRate].kHz / 50);
    tolAuxQ45 *= 915000000;
    if (fskCfg->modIdx == FSK_MOD_IDX_0_5)
    {
        tolAuxQ45 >>= 1;
    }

    /* Divide by F (F: carrier frequency) */
    tolAuxQ45 /= pllParams->chnFreq;

    /* Maximum is 50 PPM */
    if (tolAuxQ45 > PLL_DELTA_FSK_TMAX_Q45)
    {
        tolAuxQ45 = PLL_DELTA_FSK_TMAX_Q45;
    }

    return (uint32_t) tolAuxQ45;
}

static inline uint32_t _RF215_FSK_RxStartDelayUSq5 (
    DRV_RF215_FSK_CFG_OBJ* fskCfg,
    DRV_RF215_PHY_MOD_SCHEME modScheme
)
{
    uint32_t delayUSq5, delayAux;
    uint8_t symbolsOctet;
    uint8_t symbolsDelay = 0;
    DRV_RF215_FSK_SYM_RATE symRate = fskCfg->symRate;
    const RF215_FSK_SYM_RATE_CONST_OBJ* fskConst = &fskSymRateConst[symRate];

    /* Receiver baseband delay */
    delayUSq5 = fskConst->rxBaseBandDelayUSq5;

    /* Compute number of FSK symbols per octet (PHR + Payload) */
    symbolsOctet = _RF215_FSK_SymbolsPerOctet(fskCfg, modScheme);

    /* Additional delay if FEC is enabled */
    if (modScheme == FSK_FEC_ON)
    {
        delayUSq5 += 44; /* 1.375 us */
        symbolsDelay = 34;
    }

    /* Compute RXFS delay in microseconds [uQ14.5].
     * SHR (Preamble + SFD): Preamble fixed to 8 octets, SFD 2 octets. 8 symbols
     * per octet (not affected by modulation order and FEC).
     * PHR: 2 octets. Symbols depend on modulation order and FEC. */
    symbolsDelay += ((10 << 3) + (symbolsOctet << 1));
    delayAux = (uint32_t) symbolsDelay * (1000 << 5);
    return (delayUSq5 + DIV_ROUND(delayAux, fskConst->kHz));
}

static inline bool _RF215_OFDM_CheckConfig(DRV_RF215_OFDM_CFG_OBJ* ofdmConfig)
{
    DRV_RF215_OFDM_BW_OPT opt = ofdmConfig->opt;
    DRV_RF215_OFDM_ITLV_MODE itlv = ofdmConfig->itlv;

    if ((opt > OFDM_BW_OPT_4) || (opt < OFDM_BW_OPT_1) ||
            (itlv > OFDM_INTERLEAVING_1) || (itlv < OFDM_INTERLEAVING_0))
    {
        /* Invalid bandwidth option or interleaving mode */
        return false;
    }

    /* Valid configuration */
    return true;
}

static inline void _RF215_OFDM_Regs (
    RF215_PHY_OBJ* phyObj,
    RF215_PHY_REGS_OBJ* phyRegs
)
{
    uint32_t freqLow, freqMid, freqHigh, bwHalf;
    uint8_t ofdmphrrx, ofdmc, ofdmsw, div;
    DRV_RF215_OFDM_CFG_OBJ *ofdmCfg = &phyObj->phyConfig.phyTypeCfg.ofdm;
    DRV_RF215_OFDM_BW_OPT opt = ofdmCfg->opt;
    const RF215_OFDM_BW_OPT_CONST_OBJ* ofdmConst = &ofdmBwOptConst[opt];

    /* OFDMPHRRX.SPC: RX Spurious Compensation. Recommended to activate if
     * the receive channel is a multiple of 26MHz or 32MHz.
     * If activated, the AGC target level (AGCS.TGT) should be set to 0 */
    freqMid = phyObj->pllParams.chnFreq;
    bwHalf = ofdmConst->bwHz >> 1;
    freqLow = freqMid - bwHalf;
    freqHigh = freqMid + bwHalf;
    ofdmphrrx = phyObj->phyRegs.BBCn_OFDMPHRRX & (~RF215_BBCn_OFDMPHRRX_SPC_EN);
    div = (uint8_t) (freqHigh / 26000000UL);
    if ((26000000UL * div) >= freqLow)
    {
        /* Multiple of 26MHz falls in receive bandwidth */
        ofdmphrrx |= RF215_BBCn_OFDMPHRRX_SPC_EN;
    }
    else
    {
        div = (uint8_t) (freqHigh / 32000000UL);
        if ((32000000UL * div) >= freqLow)
        {
            /* Multiple of 32MHz falls in receive bandwidth */
            ofdmphrrx |= RF215_BBCn_OFDMPHRRX_SPC_EN;
        }
    }

    /* BBCn_OFDMPHRRX */
    phyRegs->BBCn_OFDMPHRRX = ofdmphrrx;

    /* OFDMC.OPT: MR-OFDM Bandwidth Option
     * OFDMC.POI: PIB Attribute phyOFDMInterleaving
     * OFDMC.SSTX: Transmitter Scrambler Seed Configuration (0) */
    ofdmc = RF215_BBCn_OFDMC_OPT(opt) | RF215_BBCn_OFDMC_POI(ofdmCfg->itlv);

    /* OFDMC.LFO: Reception with Low Frequency Offset.
     * Enable if it is guaranteed that the absolute frequency offset of the
     * received OFDM signal is less than 57.3kHz.
     * Get maximum frequency offset due to tolerance, depending on PHY and
     * channel configuration. Multiply by 2 because offset is given for
     * single-sided clock */
    if ((phyObj->pllParams.freqDelta << 1) < 57300)
    {
        ofdmc |= RF215_BBCn_OFDMC_LFO_EN;
    }

    /* BBCn_OFDMC */
    phyRegs->BBCn_OFDMC = ofdmc;

    /* OFDMSW.PDT: Preamble Detection Threshold
     * OFDMSW.RXO = 1: Receiver restarted by >12dB stronger frame */
    ofdmsw = ofdmConst->BBCn_OFDMSW_PDT | RF215_BBCn_OFDMSW_RXO_12dB;

    /* BBCn_OFDMSW */
    phyRegs->BBCn_OFDMSW = ofdmsw;
}

static inline uint32_t _RF215_OFDM_PpduDuration (
    DRV_RF215_OFDM_CFG_OBJ* ofdmCfg,
    DRV_RF215_PHY_MOD_SCHEME modScheme,
    uint16_t psduLen,
    uint16_t* pSymbolsPayload
)
{
    uint32_t bitsPayTotal, durationUS;
    uint16_t symbolsTotal;
    uint16_t symbolsPay;
    uint16_t bitsSymb;
    uint8_t mcsIndex = (uint8_t) (modScheme - OFDM_MCS_0);
    const RF215_OFDM_BW_OPT_CONST_OBJ* optConst = &ofdmBwOptConst[ofdmCfg->opt];
    const RF215_OFDM_MCS_CONST_OBJ* mcsConst = &ofdmMcsConst[mcsIndex];
    uint8_t repFactShift = mcsConst->repFactorShift;

    /* Payload: PSDU bits + PPDU Tail (6 bits for convolutional encoder) */
    bitsPayTotal = (psduLen << 3) + 6;

    /* Payload: Convolutional encoder (rate depends on MCS) */
    if ((modScheme == OFDM_MCS_4) || (modScheme == OFDM_MCS_6))
    {
        /* Rate 3/4: Multiply by 4/3 */
        bitsPayTotal = DIV_CEIL(bitsPayTotal << 2, 3);
    }
    else
    {
        /* Rate 1/2: Multiply by 2 */
        bitsPayTotal <<= 1;
    }

    /* Frequency spreading repetition factor depending on MCS */
    bitsPayTotal <<= repFactShift;

    /* Compute number of OFDM payload symbols */
    bitsSymb = (uint16_t) optConst->dataCarriers << mcsConst->bitsCarrierShift;
    symbolsPay = (uint16_t) DIV_CEIL(bitsPayTotal, bitsSymb);

    /* If phyOfdmInterleaving=1, symbols multiple of repetition factor */
    if (ofdmCfg->itlv == OFDM_INTERLEAVING_1)
    {
        symbolsPay = DIV_CEIL(symbolsPay, 1 << repFactShift) << repFactShift;
    }

    /* Compute frame duration in microseconds
     * SHR (STF + LTF): 6 OFDM symbols (not affected by any parameter)
     * PHR: Number of symbols depending on option and phyOfdmInterleaving */
    symbolsTotal = 6 + optConst->phrSymbols[ofdmCfg->itlv] + symbolsPay;
    *pSymbolsPayload = symbolsPay;
    durationUS = (uint32_t) symbolsTotal * 120;
    return SYS_TIME_USToCount(durationUS);
}

static inline DRV_RF215_PHY_MOD_SCHEME _RF215_OFDM_ReadPHR (
    uint8_t phr,
    DRV_RF215_OFDM_BW_OPT opt
)
{
    DRV_RF215_PHY_MOD_SCHEME modScheme;

    /* Get MCS from PHR */
    phr &= RF215_BBCn_OFDMPHRRX_MCS_Msk;
    modScheme = (DRV_RF215_PHY_MOD_SCHEME) (phr + OFDM_MCS_0);

    if ((modScheme > OFDM_MCS_6) || (modScheme < ofdmBwOptConst[opt].minMCS))
    {
        return MOD_SCHEME_INVALID;
    }

    /* Valid MCS */
    return modScheme;
}

static inline uint32_t _RF215_OFDM_RxStartDelayUSq5 (
    DRV_RF215_OFDM_CFG_OBJ* ofdmCfg
)
{
    uint32_t delayUSq5;
    uint8_t symbolsDelay;
    DRV_RF215_OFDM_ITLV_MODE itlv = ofdmCfg->itlv;
    const RF215_OFDM_BW_OPT_CONST_OBJ* optConst = &ofdmBwOptConst[ofdmCfg->opt];

    /* Receiver baseband delay */
    delayUSq5 = optConst->rxBaseBandDelayUSq5[itlv];

    /* Compute RXFS delay in microseconds [uQ14.5]
     * SHR (STF + LTF): 6 OFDM symbols (not affected by any parameter)
     * PHR: Number of symbols depending on option and phyOfdmInterleaving */
    symbolsDelay = 6 + optConst->phrSymbols[itlv];
    return (delayUSq5 + ((uint32_t) symbolsDelay * (120 << 5)));
}

static void _RF215_BBC_Regs(RF215_PHY_OBJ* phyObj, RF215_PHY_REGS_OBJ* phyRegs)
{
    /* Get BBC registers for the specific PHY type.
     * Turnaround time used for slotted CSMA-CA (aTurnaroundTime):
     * RX-to-TX or TX-to-RX turnaround time. From IEEE 802.15.4 Table 11-1 PHY
     * constants: For the SUN, PHYs, the value is 1 ms expressed in symbol
     * periods, rounded up to the next integer number of symbol periods using
     * the ceiling() function.
     * For all FSK symbol rates it is 1 ms exact.
     * For all OFDM options it is 1.08 ms (9 symbols of 120 us). */
    if (phyObj->phyConfig.phyType == PHY_TYPE_FSK)
    {
        _RF215_FSK_Regs(&phyObj->phyConfig.phyTypeCfg.fsk, phyRegs);
        phyObj->turnaroundTimeUS = 1000;
    }
    else /* PHY_TYPE_OFDM */
    {
        _RF215_OFDM_Regs(phyObj, phyRegs);
        phyObj->turnaroundTimeUS = 1080;
    }
}

static void _RF215_BBC_WriteRegs(uint8_t trxIdx, RF215_PHY_REGS_OBJ* phyRegsNew)
{
    RF215_PHY_OBJ* pObj = &rf215PhyObj[trxIdx];

    /* Write BBC configuration, depending on PHY type */
    if (pObj->phyConfig.phyType == PHY_TYPE_FSK)
    {
        /* Write up to 4 registers: BBCn_FSKC0 to BBCn_FSKC3 */
        RF215_HAL_SpiWriteUpdate(RF215_BBCn_FSKC0(trxIdx),
                &phyRegsNew->BBCn_FSKC0, &pObj->phyRegs.BBCn_FSKC0, 4);

        /* Write up to 4 registers: BBCn_FSKDM to BBCn_FSKPE2 */
        RF215_HAL_SpiWriteUpdate(RF215_BBCn_FSKDM(trxIdx),
                &phyRegsNew->BBCn_FSKDM, &pObj->phyRegs.BBCn_FSKDM, 4);
    }
    else /* PHY_TYPE_OFDM */
    {
        /* Write up to 3 registers: BBCn_OFDMPHRRX to RF215_BBCn_OFDMSW */
        RF215_HAL_SpiWriteUpdate(RF215_BBCn_OFDMPHRRX(trxIdx),
                &phyRegsNew->BBCn_OFDMPHRRX, &pObj->phyRegs.BBCn_OFDMPHRRX, 3);
    }
}

static inline void _RF215_BBC_SetPhyControl(uint8_t trxIdx, uint8_t pc)
{
    RF215_PHY_OBJ* pObj = &rf215PhyObj[trxIdx];
    if (pObj->phyRegs.BBCn_PC != pc)
    {
        pObj->phyRegs.BBCn_PC = pc;
        RF215_HAL_SpiWrite(RF215_BBCn_PC(trxIdx), &pObj->phyRegs.BBCn_PC, 1);
    }
}

static inline void _RF215_BBC_BaseBandDisable(uint8_t trxIdx)
{
    uint8_t pc = rf215PhyObj[trxIdx].phyRegs.BBCn_PC & (~RF215_BBCn_PC_BBEN_Msk);
    _RF215_BBC_SetPhyControl(trxIdx, pc);
}

static inline void _RF215_BBC_BaseBandEnable(uint8_t trxIdx)
{
    uint8_t pc = rf215PhyObj[trxIdx].phyRegs.BBCn_PC | RF215_BBCn_PC_BBEN_Msk;
    _RF215_BBC_SetPhyControl(trxIdx, pc);
}

static inline void _RF215_BBC_SetFBLI(uint8_t trxIdx, uint16_t fbli)
{
    RF215_PHY_REGS_OBJ regsNew;
    RF215_PHY_REGS_OBJ* regsOld = &rf215PhyObj[trxIdx].phyRegs;

    /* Write FBLI value in registers. First byte is LSB. */
    regsNew.BBCn_FBLIL = (uint8_t) (fbli & 0xFF);
    regsNew.BBCn_FBLIH = (uint8_t) RF215_BBCn_FBLIH_FBLIH(fbli >> 8);

    /* If only BBCn_FBLIL is changed, BBCn_FBLIH must be written too */
    if (regsNew.BBCn_FBLIL != regsOld->BBCn_FBLIL)
    {
        regsOld->BBCn_FBLIH = regsNew.BBCn_FBLIH + 1;
    }

    /* Write up to 2 registers: BBCn_FBLIL, BBCn_FBLIH */
    RF215_HAL_SpiWriteUpdate(RF215_BBCn_FBLIL(trxIdx),
            &regsNew.BBCn_FBLIL, &regsOld->BBCn_FBLIL, 2);
}

static inline uint16_t _RF215_BBC_GetBestFBLI (
    DRV_RF215_PHY_CFG_OBJ* phyCfg,
    DRV_RF215_PHY_MOD_SCHEME modScheme,
    uint16_t psduLen
)
{
    uint32_t bitsPayTotal, octetsPayTotal, payloadUSq5;
    uint16_t octetUSq5, marginUSq5, bitsSymb;
    const RF215_OFDM_BW_OPT_CONST_OBJ* optConst;
    const RF215_OFDM_MCS_CONST_OBJ* mcsConst;
    uint8_t symbolsOctet;
    uint32_t numAux = 0;
    uint32_t denAux = 1;
    uint16_t bitsBlock = 8;
    uint8_t fecK = 0;
    uint16_t fbli = 2047;
    uint8_t fecFlushBits = 0;
    uint8_t fecDelayBits = 0;
    DRV_RF215_PHY_TYPE_CFG_OBJ* phyTypeCfg = &phyCfg->phyTypeCfg;

    if (phyCfg->phyType == PHY_TYPE_FSK)
    {
        /* Compute number of FSK symbols per octet */
        symbolsOctet = _RF215_FSK_SymbolsPerOctet(&phyTypeCfg->fsk, modScheme);

        /* Compute octet duration in us [uQ9.5] */
        numAux = (uint32_t) symbolsOctet * (1000 << 5);
        denAux = fskSymRateConst[phyTypeCfg->fsk.symRate].kHz;

        if (modScheme == FSK_FEC_ON)
        {
            /* FEC (and interleaver) enabled (K = 4). Interleaver works with
             * blocks of 16 code-symbols, so Frame Buffer is updated every
             * 2 octets */
            bitsBlock = 16;
            fecK = 4;
        }
    }
    else
    {
        /* OFDM constants depending on bandwidth option and MCS */
        optConst = &ofdmBwOptConst[phyTypeCfg->ofdm.opt];
        mcsConst = &ofdmMcsConst[(uint8_t) (modScheme - OFDM_MCS_0)];

        /* Total bits (coded, with repetition) of 1 OFDM symbol */
        bitsSymb = (uint16_t) optConst->dataCarriers << mcsConst->bitsCarrierShift;

        /* OctectDuration = (8*Rep*SymbDur)/(CC_rate*DataCarr*BitsCarr) */
        numAux = 120 << (mcsConst->repFactorShift + 8);
        denAux = bitsSymb;

        /* Number of bits (uncoded, with repetition) of 1 OFDM symbol */
        if ((modScheme == OFDM_MCS_4) || (modScheme == OFDM_MCS_6))
        {
            /* FEC rate 3/4: Multiply by 3/4 */
            bitsBlock = (bitsSymb * 3) >> 2;
            numAux <<= 2;
            denAux *= 3;
        }
        else
        {
            /* FEC rate 1/2: Divide by 2 */
            bitsBlock = bitsSymb >> 1;
            numAux <<= 1;
        }

        /* FEC (and interleaver) always enabled in OFDM (K = 7). Interleaver
         * works with blocks of 1 OFDM symbol (phyOfdmInterleaving=0) or FreqRep
         * OFDM symbols (phyOfdmInterleaving=1) */
        fecK = 7;
        if (phyTypeCfg->ofdm.itlv == OFDM_INTERLEAVING_0)
        {
            bitsBlock >>= mcsConst->repFactorShift;
        }
    }

    /* Compute RF octet duration in us [uQ9.5] */
    octetUSq5 = (uint16_t) DIV_ROUND(numAux, denAux);

    if (fecK >= 2)
    {
        /* FEC enabled. FlushingBits=K-1; Delay=2^(K-2) */
        fecFlushBits = fecK - 1;
        fecDelayBits = 1 << (fecK - 2);
    }

    /* Compute total payload octets, including FEC flushing and padding */
    bitsPayTotal = (psduLen << 3) + fecFlushBits;
    bitsPayTotal = DIV_CEIL(bitsPayTotal, bitsBlock) * bitsBlock;
    octetsPayTotal = DIV_CEIL(bitsPayTotal, 8);

    /* 500 us of margin between FBLI and RXFE in case there is another
     * interrupt in between to avoid delaying RXFE interrupt.
     * Added time of SPI transactions before reading buffer in FBLI interrupt:
     * 12 bytes (6 IRQS, 4 FBL, 2 SPI header) */
    marginUSq5 = (500 << 5) + (RF215_SPI_BYTE_DURATION_US_Q5 * 12);

    /* Payload duration: PsduLen * TimeRfOctect */
    payloadUSq5 = (uint32_t) octetUSq5 * octetsPayTotal;

    if (payloadUSq5 > marginUSq5)
    {
        uint16_t fbliBits, fbliBytes;

        /* Compute number of bytes for FBLI interrupt as:
         * (PayDuration - Margin) / (TimeByteSPI + TimeRfOctect) */
        numAux = payloadUSq5 - marginUSq5;
        denAux = octetUSq5 + RF215_SPI_BYTE_DURATION_US_Q5;
        fbliBytes = (uint16_t) (numAux / denAux);

        /* Force FBLI to be multiple of block size (flooring) */
        fbliBits = ((fbliBytes << 3) / bitsBlock) * bitsBlock;

        /* Remove FEC "delay" */
        if (fbliBits > fecDelayBits)
        {
            fbliBits -= fecDelayBits;
        }
        else
        {
            fbliBits = 0;
        }

        if (fbliBits > 8)
        {
            /* Convert to octets (flooring) */
            fbli = fbliBits >> 3;

            /* FBLI interrupt is triggered when Frame Buffer Level is higher
             * than BBCn_FBLI */
            fbli -= 1;
        }
    }

    return fbli;
}

static void _RF215_PLL_Params (
    const RF215_PLL_CONST_OBJ* pllConst,
    RF215_PLL_PARAMS_OBJ* pllParams,
    DRV_RF215_PHY_CFG_OBJ* phyCfg,
    uint16_t chnNum
)
{
    uint64_t chnFreqAux;
    uint32_t chnFreq;
    const RF215_PLL_FREQ_RNG_OBJ* freqRange;
    uint32_t freqTolQ45 = 0;
    uint8_t freqRng = 0xFF;
    uint8_t chnMode = 0;

    /* Compute channel frequency: F = F0 + (CS * CN) */
    chnFreqAux = (uint64_t) phyCfg->chnSpaHz * chnNum;
    chnFreqAux += phyCfg->chnF0Hz;

    /* Saturate value to 32 bits */
    if (chnFreqAux > UINT32_MAX)
    {
        chnFreqAux = UINT32_MAX;
    }

    chnFreq = (uint32_t) chnFreqAux;
    pllParams->chnFreq = chnFreq;

    /* Check if frequency is within allowed RF215 ranges */
    for (uint8_t rng = 0; rng < pllConst->numFreqRanges; rng++)
    {
        freqRange = &pllConst->freqRanges[rng];
        if ((chnFreq >= freqRange->freqMin) && (chnFreq <= freqRange->freqMax))
        {
            /* Frequency is within this supported range */
            freqRng = rng;

            if (((phyCfg->chnF0Hz % PLL_IEEE_FREQ_STEP_Hz) == 0) &&
                    ((phyCfg->chnSpaHz % PLL_IEEE_FREQ_STEP_Hz) == 0) &&
                    (phyCfg->chnSpaHz <= PLL_IEEE_CHN_SPA_MAX_Hz) &&
                    (chnNum <= PLL_IEEE_CHN_NUM_MAX))
            {
                /* IEEE Mode: Frequencies (F0 and CS) multiples of 25kHz,
                 * channel number (CN) fits in 9 bits and channel spacing (CS)
                 * fits in 8 bits (in 25kHz steps) */
                chnMode = RF215_RFn_CNM_CM_IEEE;
            }
            else
            {
                /* Fine Mode. Get channel mode depending on frequency range */
                chnMode = pllConst->fineChnMode[freqRng];
            }

            break;
        }
    }

    pllParams->freqRng = freqRng;
    pllParams->chnMode = chnMode;

    /* Compute maximum frequency offset due to tolerance (single-sided clock) */
    if (phyCfg->phyType == PHY_TYPE_FSK)
    {
        freqTolQ45 = _RF215_FSK_FreqTolQ45(pllConst, pllParams, &phyCfg->phyTypeCfg.fsk);
    }
    else /* PHY_TYPE_OFDM */
    {
        freqTolQ45 = PLL_DELTA_OFDM_TMAX_Q45;
    }

    /* Compute fDelta = T * F; T is in uQ0.45 */
    chnFreqAux = (uint64_t) chnFreq * freqTolQ45;
    pllParams->freqDelta = (uint32_t) (chnFreqAux >> 45);
}

static inline bool _RF215_PLL_CheckConfig (
    const RF215_PLL_CONST_OBJ* pllConst,
    RF215_PLL_PARAMS_OBJ* pllParams,
    DRV_RF215_PHY_CFG_OBJ* phyCfg,
    uint16_t chnNum
)
{
    /* Check if channel is out of configured range */
    if (((chnNum < phyCfg->chnNumMin) || (chnNum > phyCfg->chnNumMax)) &&
            ((chnNum < phyCfg->chnNumMin2) || (chnNum > phyCfg->chnNumMax2)))
    {
        return false;
    }

    /* Check if frequency is out of allowed RF215 ranges */
    if (pllParams->freqRng >= pllConst->numFreqRanges)
    {
        return false;
    }

    /* Valid channel configuration */
    return true;
}

static void _RF215_PLL_Regs (
    RF215_PHY_OBJ* phyObj,
    const RF215_PLL_CONST_OBJ* pllConst,
    RF215_PHY_REGS_OBJ* regsNew
)
{
    uint32_t f0;
    RF215_PLL_PARAMS_OBJ* pllParams = &phyObj->pllParams;
    uint8_t chnMode = pllParams->chnMode;
    RF215_PHY_REGS_OBJ* regsOld = &phyObj->phyRegs;

    if (chnMode == RF215_RFn_CNM_CM_IEEE)
    {
        /* IEEE-compliant Scheme (CNM.CM=0). Write 5 registers */
        uint16_t f025KHz;
        DRV_RF215_PHY_CFG_OBJ *phyCfg = &phyObj->phyConfig;
        uint16_t chnNum = phyObj->channelNum;

        /* RFn_CS - Channel Spacing. Convert to 25kHz steps */
        regsNew->RFn_CS = (uint8_t) (phyCfg->chnSpaHz / PLL_IEEE_FREQ_STEP_Hz);

        /* Channel Center Frequency F0. For RF24 there is a 1.5GHz offset */
        f0 = phyCfg->chnF0Hz - pllConst->ieeeFreqOffset;
        /* Convert to 25kHz steps */
        f025KHz = (uint16_t) (f0 / PLL_IEEE_FREQ_STEP_Hz);

        /* RFn_CCF0L - Channel Center Frequency F0 Low Byte */
        regsNew->RFn_CCF0L = (uint8_t) (f025KHz & 0xFF);
        /* RFn_CCF0H - Channel Center Frequency F0 High Byte */
        regsNew->RFn_CCF0H = (uint8_t) (f025KHz >> 8);
        /* RFn_CNL - Channel Number Low Byte */
        regsNew->RFn_CNL = (uint8_t) (chnNum & 0xFF);
        /* RFn_CNM - Channel Mode and Channel Number High Bit */
        regsNew->RFn_CNM = chnMode | (uint8_t) RF215_RFn_CNM_CNH(chnNum >> 8);
    }
    else
    {
        /* Fine Resolution Channel Scheme. Write 4 registers */
        uint32_t Nchannel;
        uint8_t freqRng = pllParams->freqRng;
        uint32_t freqOffset = pllConst->fineFreqOffset[freqRng];
        uint32_t freqRes = pllConst->fineFreqRes[freqRng];

        /* RFn_CS not used in Fine Resolution Channel Scheme */
        regsNew->RFn_CS = regsOld->RFn_CS;

        /* Channel Center Frequency F0. Offset/resolution depending on range */
        f0 = pllParams->chnFreq - freqOffset;

        /* Nchannel (24-bit) = (F0-offset)*2^16/resolution */
        Nchannel = (uint32_t) DIV_ROUND((uint64_t) f0 << 16, freqRes);

        /* RFn_CCF0L - Channel Center Frequency F0 Low Byte (middle byte) */
        regsNew->RFn_CCF0L = (uint8_t) ((Nchannel >> 8) & 0xFF);
        /* RFn_CCF0H - Channel Center Frequency F0 High Byte (high byte) */
        regsNew->RFn_CCF0H = (uint8_t) (Nchannel >> 16);
        /* RFn_CNL - Channel Number Low Byte (low byte) */
        regsNew->RFn_CNL = (uint8_t) (Nchannel & 0xFF);

        /* RFn_CNM - Channel Mode and Channel Number High Bit */
        regsNew->RFn_CNM = chnMode;

        /* Recompute channel frequency in Hz as:
         * ((Nchannel*resolution)/2^16)+offset */
        f0 = (uint32_t) DIV_ROUND((uint64_t) Nchannel * freqRes, 1 << 16);
        f0 += freqOffset;
        pllParams->chnFreq = f0;
    }

    if ((regsNew->RFn_CS != regsOld->RFn_CS) ||
            (regsNew->RFn_CCF0L != regsOld->RFn_CCF0L) ||
            (regsNew->RFn_CCF0H != regsOld->RFn_CCF0H) ||
            (regsNew->RFn_CNL != regsOld->RFn_CNL))
    {
        if (regsNew->RFn_CNM == regsOld->RFn_CNM)
        {
            /* RFn_CNM must always be written */
            regsOld->RFn_CNM = regsNew->RFn_CNM + 1;
        }
    }
}

static inline void _RF215_RXFE_SetEDD(uint8_t trxIdx, uint8_t edd)
{
    RF215_PHY_OBJ* pObj = &rf215PhyObj[trxIdx];
    if (pObj->phyRegs.RFn_EDD != edd)
    {
        pObj->phyRegs.RFn_EDD = edd;
        RF215_HAL_SpiWrite(RF215_RFn_EDD(trxIdx), &pObj->phyRegs.RFn_EDD, 1);
    }
}

static inline void _RF215_RXFE_SetAutoEDD(uint8_t trxIdx)
{
    /* Energy detection duration for automatic mode */
    _RF215_RXFE_SetEDD(trxIdx, RF215_RFn_EDD_DTB_128us | RF215_RFn_EDD_DF(63));
}

static inline void _RF215_RXFE_SetEnDetectDuration(uint8_t trxIdx, uint16_t eddUS)
{
    uint8_t eddDF = 16;
    uint8_t eddDTB = RF215_RFn_EDD_DTB_8us;

    /* Convert microseconds to EDD.DTB/DF subfields */
    if (eddUS <= (63 << 1))
    {
        /* EDD.DTB: 2us */
        eddDTB = RF215_RFn_EDD_DTB_2us;
        eddDF = (uint8_t) (eddUS >> 1);
    }
    else if (eddUS <= (63 << 3))
    {
        /* EDD.DTB: 8us */
        eddDF = (uint8_t) (eddUS >> 3);
    }
    else if (eddUS <= (63 << 5))
    {
        /* EDD.DTB: 32us */
        eddDTB = RF215_RFn_EDD_DTB_32us;
        eddDF = (uint8_t) (eddUS >> 5);
    }
    else
    {
        /* EDD.DTB: 128us */
        eddDTB = RF215_RFn_EDD_DTB_128us;
        eddDF = (uint8_t) (eddUS >> 7);
    }

    _RF215_RXFE_SetEDD(trxIdx, eddDTB | RF215_RFn_EDD_DF(eddDF));
}

static inline void _RF215_RXFE_AdjustEDD(uint8_t trxIdx)
{
    uint8_t agcUpdTimeUS;
    RF215_PHY_OBJ* pObj = &rf215PhyObj[trxIdx];
    uint8_t srRxVal = (pObj->phyRegs.RFn_RXDFE & RF215_RFn_RXDFE_SR_Msk) >> RF215_RFn_RXDFE_SR_Pos;
    uint16_t eddUS = pObj->phyConfig.ccaEdDurationUS;

    /* AGC update time, depending on RXDFE.SR, AGCC.AGCI and AGCC.AVGS.
     * AGCC.AVGS = 0 and AGCC.AGCI = 0 always.
     * Minimum EDD (Energy Detection Duration) is AGC update time. */
    agcUpdTimeUS = rf215AgcUpdTime0[srRxVal];
    if (eddUS < agcUpdTimeUS)
    {
        eddUS = agcUpdTimeUS;
    }

    /* Adjust EDD depending on RFn_EDD.DTB resolution (ceiling) */
    if (eddUS <= (63 << 1))
    {
        /* EDD.DTB: 2us */
        eddUS = ((eddUS + 1) >> 1) << 1;
    }
    else if (eddUS <= (63 << 3))
    {
        /* EDD.DTB: 8us */
        eddUS = ((eddUS + 7) >> 3) << 3;
    }
    else if (eddUS <= (63 << 5))
    {
        /* EDD.DTB: 32us */
        eddUS = ((eddUS + 31) >> 5) << 5;
    }
    else
    {
        /* EDD.DTB: 128us */
        eddUS = ((eddUS + 127) >> 7) << 7;
        if (eddUS > 63 << 7)
        {
            eddUS = 63 << 7;
        }
    }

    /* Update duration of ED for CCA */
    pObj->phyConfig.ccaEdDurationUS = eddUS;
}

static inline uint8_t _RF215_AFE_CutOff(uint32_t cutOffFreq)
{
    /* Find value so that fLPFCUT >= cutoffFreq */
    for (uint8_t idx = 0; idx < 12; idx++)
    {
        if (cutOffFreq <= rf215AfeCutOffFreq[idx])
        {
            /* Cut-off frequency found */
            return idx;
        }
    }

    /* Any cut-off frequency meets the requirement */
    return 11;
}

static inline uint8_t _RF215_DFE_CutOff(uint32_t cutoffFreq, uint8_t sr)
{
    uint32_t rcutHz[4];
    uint32_t fsHz;

    /* Get Transmitter digital front-end sampling frequency in Hz */
    fsHz = 4000000 / sr;

    /** fCUT = x*fs/2; fs is sampling rate; x is between 0.25 and 1 */
    /** Compute cut-off frequencies for fs used */
    /* Value 0: fCUT=0.25*fs/2 */
    rcutHz[0] = fsHz >> 3;
    /* Value 1: fCUT=0.375*fs/2 */
    rcutHz[1] = (fsHz * 3) >> 4;
    /* Value 2: fCUT=0.5*fs/2 */
    rcutHz[2] = fsHz >> 2;
    /* Value 3: fCUT=0.75*fs/2 */
    rcutHz[3] = (fsHz * 3) >> 3;
    /* Value 4: fCUT=1.00*fs/2 (bypassed) */

    /* Find value so that fCUT >= cutoffFreq */
    for (uint8_t idx = 0; idx < 4; idx++)
    {
        if (cutoffFreq <= rcutHz[idx])
        {
            /* Cut-off frequency found */
            return idx;
        }
    }

    /* Any RCUT cut-off frequency meet the requirement: bypass */
    return 4;
}

static void _RF215_TXRXFE_Regs(RF215_PHY_OBJ* phyObj, RF215_PHY_REGS_OBJ* regsNew)
{
    uint32_t freqDelta, freqDev, freqDevAux, bwFreqDelta, bwHz;
    const RF215_OFDM_BW_OPT_CONST_OBJ* ofdmConst;
    const RF215_FSK_SYM_RATE_CONST_OBJ* fskConst;
    uint8_t rxbwc, bwVal, lpfcutVal, rcutRxVal, rcutTxVal;
    uint32_t rxbwcBwHz = 0;
    uint32_t lpfcutFreq = 0;
    uint32_t rcutRxFreqHz = 0;
    uint32_t rcutTxFreqHz = 0;
    uint8_t rxdfe = 0;
    uint8_t txdfe = 0;
    uint8_t srRxVal = 0;
    uint8_t srTxVal = 0;
    uint8_t agcs = 0;
    uint8_t txcutc = 0;
    DRV_RF215_PHY_TYPE_CFG_OBJ* phyTypeCfg = &phyObj->phyConfig.phyTypeCfg;
    RF215_PHY_REGS_OBJ* regsOld = &phyObj->phyRegs;

    /* Get maximum frequency offset due to tolerance, depending on PHY and
     * channel configuration. Multiply by 2 because offset is given for
     * single-sided clock */
    freqDelta = phyObj->pllParams.freqDelta << 1;

    if (phyObj->phyConfig.phyType == PHY_TYPE_FSK)
    {
        /* Transmitter/Receiver front-end configuration for FSK */
        fskConst = &fskSymRateConst[phyTypeCfg->fsk.symRate];

        /* Frequency deviation, depending on modulation index:
         * fDev = (SymbRate * modIdx) / 2 */
        freqDev = fskConst->Hz >> 1;
        if (phyTypeCfg->fsk.modIdx == FSK_MOD_IDX_0_5)
        {
            freqDev >>= 1;
        }

        /* Reduce as much noise / interference as possible, but making sure that
         * in the worst case (frequency tolerance) the received signal is
         * attenuated <=3dB and in the best case (perfect frequency alignment)
         * the received signal is not attenuated.
         * fBW >= 2*(max(2.5*fDev, fDev + fDelta))
         * fCUT >= max(2.5*fDev, fDev + fDelta) */
        rxbwcBwHz = (freqDev * 5) >> 1;
        freqDevAux = freqDev + freqDelta;
        if (freqDevAux > rxbwcBwHz)
        {
            rxbwcBwHz = freqDevAux;
        }

        rcutRxFreqHz = rxbwcBwHz;

        /* RXDFE.SR: RX DFE sampling rate, depending on FSK symbol rate */
        rxdfe = fskConst->RFn_RXDFE_SR;
        srRxVal = rxdfe >> RF215_RFn_RXDFE_SR_Pos;

        /* AGCS.TGT: AGC target level -24dB [Tables 6-60 to 6-63]
         * AGCS.GCW: keep initial reset value 23 */
        agcs = RF215_RFn_AGCS_GCW(23) | RF215_RFn_AGCS_TGT_24dB;

        /* TXCUTC.PARAMP: Power Amplifier ramp time
         * TXDFE.SR: Transmitter digital front-end sampling rate
         * Depending on FSK symbol rate */
        txcutc = fskConst->RFn_TXCUT_PARAMP;
        txdfe = fskConst->RFn_TXDFE_SR;
        srTxVal = txdfe >> RF215_RFn_TXDFE_SR_Pos;

        /* TXDFE.DM: Direct modulation. It must be enabled for FSK (also in
         * FSKDM.EN). It improves the modulation quality */
        txdfe |= RF215_RFn_TXDFE_DM_EN;

        /* Reduce spurious transmissions as much as possible without attenuating
         * transmitted carrier (fdev). fLPFCUT >= 3*fdev; fCUT >= 5*fdev */
        lpfcutFreq = freqDev * 3;
        rcutTxFreqHz = freqDev * 5;
    }
    else /* PHY_TYPE_OFDM */
    {
        /* Transmitter/Receiver front-end configuration for OFDM */
        ofdmConst = &ofdmBwOptConst[phyTypeCfg->ofdm.opt];
        bwHz = ofdmConst->bwHz;

        /* Reduce as much noise / interference as possible, but making sure that
         * in the worst case (frequency tolerance) the received signal is
         * attenuated <=3dB and in the best case (perfect frequency alignment)
         * the received signal is not attenuated (BW / 2).
         * fBW >= 2*(max(0.5625*BW, (BW/2) + fdelta))
         * fCUT >= max(0.8125*BW, (BW/2) + fdelta) */
        bwFreqDelta = (bwHz >> 1) + freqDelta;
        rxbwcBwHz = (bwHz * 9) >> 4;
        if (bwFreqDelta > rxbwcBwHz)
        {
            rxbwcBwHz = bwFreqDelta;
        }

        rcutRxFreqHz = (bwHz * 13) >> 4;
        if (bwFreqDelta > rcutRxFreqHz)
        {
            rcutRxFreqHz = bwFreqDelta;
        }

        /* RXDFE.SR: RX DFE sampling rate, depending on FSK symbol rate */
        rxdfe = ofdmConst->RFn_RXDFE_SR;
        srRxVal = rxdfe >> RF215_RFn_RXDFE_SR_Pos;

        /* AGCS.GCW: keep initial reset value 23 */
        agcs = RF215_RFn_AGCS_GCW(23);

        /* AGCS.TGT: AGC target level, depending on OFDMPHRRX.SPC */
        if ((regsNew->BBCn_OFDMPHRRX & RF215_BBCn_OFDMPHRRX_SPC_EN) != 0)
        {
            /* Spurious compensation disabled: -21dB */
            agcs |= RF215_RFn_AGCS_TGT_21dB;
        }
        else
        {
            /* Spurious compensation disabled: -30dB [Table 6-92] */
            agcs |= RF215_RFn_AGCS_TGT_30dB;
        }

        /* TXCUTC.PARAMP: Power Amplifier ramp time
         * TXDFE.SR: Transmitter digital front-end sampling rate
         * Depending on OFDM bandwidth option */
        txcutc = RF215_RFn_TXCUTC_PARAMP_4us;
        txdfe = ofdmConst->RFn_TXDFE_SR;
        srTxVal = txdfe >> RF215_RFn_TXDFE_SR_Pos;

        /* Reduce spurious transmissions as much as possible without
         * attenuating transmitted signal (BW / 2).
         * fLPFCUT >= 0.875*BW; fCUT >= 0.875*BW */
        lpfcutFreq = (ofdmConst->bwHz * 7) >> 3;
        rcutTxFreqHz = lpfcutFreq;
    }

    /* RXBWC.BW: Receiver analog front-end band pass filter bandwidth */
    bwVal = _RF215_AFE_CutOff(rxbwcBwHz);
    rxbwc = RF215_RFn_RXBWC_BW(bwVal);

    /* RXBWC.IFS: Multiply fIF by 1.25 if fBW==fIF */
    if ((rxbwc == RF215_RFn_RXBWC_BW250_IF250kHz) ||
            (rxbwc == RF215_RFn_RXBWC_BW500_IF500kHz) ||
            (rxbwc == RF215_RFn_RXBWC_BW1000_IF1000kHz) ||
            (rxbwc == RF215_RFn_RXBWC_BW2000_IF2000kHz)) {
        rxbwc |= RF215_RFn_RXBWC_IFS;
    }

    /* RFn_RXBWC */
    regsNew->RFn_RXBWC = rxbwc;

    /* RXDFE.RCUT: Receiver digital front-end pre-filter normalized
     * cut-off frequency */
    rcutRxVal = _RF215_DFE_CutOff(rcutRxFreqHz, srRxVal);
    rxdfe |= RF215_RFn_RXDFE_RCUT(rcutRxVal);

    /* RFn_RXDFE */
    regsNew->RFn_RXDFE = rxdfe;
    /* RFn_AGCC
     * AGCC.EN = 1: Enable AGC
     * AGCC.AVGS = 0: 8 samples for averaging [Tables 6-60 to 6-63 and 6-93]
     * AGCC.AGCI = 0: use x0 signal (post-filtered) for AGC
     * AGCC.RSV = 1: keep initial reset value 1 ?? */
    regsNew->RFn_AGCC = RF215_RFn_AGCC_EN | RF215_RFn_AGCC_RSV;
    /* RFn_AGCS */
    regsNew->RFn_AGCS = agcs;
    /* RFn_RSSI (read-only) */
    regsNew->RFn_RSSI = regsOld->RFn_RSSI;
    /* RFn_EDC: Automatic Energy Detection Mode (triggered by reception) */
    regsNew->RFn_EDC = RF215_RFn_EDC_EDM_AUTO;
    /*  RFn_EDD: Energy detection duration for automatic mode */
    regsNew->RFn_EDD = RF215_RFn_EDD_DTB_128us | RF215_RFn_EDD_DF(63);
    /* RFn_EDV (read-only) */
    regsNew->RFn_EDV = regsOld->RFn_EDV;
    /* RFn_RNDV (read-only) */
    regsNew->RFn_RNDV = regsOld->RFn_RNDV;

    /* TXCUTC.LPFCUT: TX analog front-end low pass filter cut-off freq */
    lpfcutVal = _RF215_AFE_CutOff(lpfcutFreq);
    txcutc |= RF215_RFn_TXCUTC_LPFCUT(lpfcutVal);

    /* TXDFE.RCUT: TX digital front-end pre-filter normalized cut-off freq */
    rcutTxVal = _RF215_DFE_CutOff(rcutTxFreqHz, srTxVal);
    txdfe |= RF215_RFn_TXDFE_RCUT(rcutTxVal);

    /* RFn_TXCUTC */
    regsNew->RFn_TXCUTC = txcutc;
    /* RFn_TXDFE */
    regsNew->RFn_TXDFE = txdfe;
}

static void _RF215_PHY_SetFlag(uintptr_t context, void* pData, uint64_t time)
{
    bool* flag = (bool *) context;
    *flag = true;
}

static bool _RF215_PHY_CheckPhyCfg(DRV_RF215_PHY_CFG_OBJ* phyConfig)
{
    switch (phyConfig->phyType)
    {
        case PHY_TYPE_FSK:
            return _RF215_FSK_CheckConfig(&phyConfig->phyTypeCfg.fsk);

        case PHY_TYPE_OFDM:
            return _RF215_OFDM_CheckConfig(&phyConfig->phyTypeCfg.ofdm);

        default:
            /* PHY type not supported */
            return false;
    }
}

static uint32_t _RF215_PHY_PpduDuration (
    DRV_RF215_PHY_CFG_OBJ* phyConfig,
    DRV_RF215_PHY_MOD_SCHEME modScheme,
    uint16_t psduLen,
    uint16_t* pSymbolsPayload
)
{
    if (phyConfig->phyType == PHY_TYPE_FSK)
    {
        return _RF215_FSK_PpduDuration(&phyConfig->phyTypeCfg.fsk, modScheme,
                psduLen, pSymbolsPayload);
    }
    else /* PHY_TYPE_OFDM */
    {
        return _RF215_OFDM_PpduDuration(&phyConfig->phyTypeCfg.ofdm, modScheme,
                psduLen, pSymbolsPayload);
    }
}

static bool _RF215_PHY_BandOpModeToPhyCfg (
    DRV_RF215_PHY_BAND_OPM bandOpMode,
    DRV_RF215_PHY_CFG_OBJ* phyConfig
)
{
    switch (bandOpMode) {
        case SUN_FSK_BAND_863_OPM1:
            *phyConfig = SUN_FSK_BAND_863_870_OPM1;
            break;

        case SUN_FSK_BAND_863_OPM2:
            *phyConfig = SUN_FSK_BAND_863_870_OPM2;
            break;

        case SUN_FSK_BAND_863_OPM3:
            *phyConfig = SUN_FSK_BAND_863_870_OPM3;
            break;

        case SUN_FSK_BAND_866_OPM1:
            *phyConfig = SUN_FSK_BAND_865_867_OPM1;
            break;

        case SUN_FSK_BAND_866_OPM2:
            *phyConfig = SUN_FSK_BAND_865_867_OPM2;
            break;

        case SUN_FSK_BAND_866_OPM3:
            *phyConfig = SUN_FSK_BAND_865_867_OPM3;
            break;

        case SUN_FSK_BAND_870_OPM1:
            *phyConfig = SUN_FSK_BAND_870_876_OPM1;
            break;

        case SUN_FSK_BAND_870_OPM2:
            *phyConfig = SUN_FSK_BAND_870_876_OPM2;
            break;

        case SUN_FSK_BAND_870_OPM3:
            *phyConfig = SUN_FSK_BAND_870_876_OPM3;
            break;

        case SUN_FSK_BAND_915_OPM1:
            *phyConfig = SUN_FSK_BAND_902_928_OPM1;
            break;

        case SUN_FSK_BAND_915_OPM2:
            *phyConfig = SUN_FSK_BAND_902_928_OPM2;
            break;

        case SUN_FSK_BAND_915_OPM3:
            *phyConfig = SUN_FSK_BAND_902_928_OPM3;
            break;

        case SUN_FSK_BAND_915A_OPM1:
            *phyConfig = SUN_FSK_BAND_902_928_ALT_OPM1;
            break;

        case SUN_FSK_BAND_915A_OPM2:
            *phyConfig = SUN_FSK_BAND_902_928_ALT_OPM2;
            break;

        case SUN_FSK_BAND_915A_OPM3:
            *phyConfig = SUN_FSK_BAND_902_928_ALT_OPM3;
            break;

        case SUN_FSK_BAND_915A_OPM4:
            *phyConfig = SUN_FSK_BAND_902_928_ALT_OPM4;
            break;

        case SUN_FSK_BAND_915A_OPM5:
            *phyConfig = SUN_FSK_BAND_902_928_ALT_OPM5;
            break;

        case SUN_FSK_BAND_915B_OPM1:
            *phyConfig = SUN_FSK_BAND_902_907_915_928_OPM1;
            break;

        case SUN_FSK_BAND_915B_OPM2:
            *phyConfig = SUN_FSK_BAND_902_907_915_928_OPM2;
            break;

        case SUN_FSK_BAND_915B_OPM3:
            *phyConfig = SUN_FSK_BAND_902_907_915_928_OPM3;
            break;

        case SUN_FSK_BAND_915B_OPM4:
            *phyConfig = SUN_FSK_BAND_902_907_915_928_OPM4;
            break;

        case SUN_FSK_BAND_915B_OPM5:
            *phyConfig = SUN_FSK_BAND_902_907_915_928_OPM5;
            break;

        case SUN_FSK_BAND_915C_OPM1:
            *phyConfig = SUN_FSK_BAND_915_928_OPM1;
            break;

        case SUN_FSK_BAND_915C_OPM2:
            *phyConfig = SUN_FSK_BAND_915_928_OPM2;
            break;

        case SUN_FSK_BAND_915C_OPM3:
            *phyConfig = SUN_FSK_BAND_915_928_OPM3;
            break;

        case SUN_FSK_BAND_915C_OPM4:
            *phyConfig = SUN_FSK_BAND_915_928_OPM4;
            break;

        case SUN_FSK_BAND_915C_OPM5:
            *phyConfig = SUN_FSK_BAND_915_928_OPM5;
            break;

        case SUN_FSK_BAND_2450_OPM1:
            *phyConfig = SUN_FSK_BAND_2400_2483_OPM1;
            break;

        case SUN_FSK_BAND_2450_OPM2:
            *phyConfig = SUN_FSK_BAND_2400_2483_OPM2;
            break;

        case SUN_FSK_BAND_2450_OPM3:
            *phyConfig = SUN_FSK_BAND_2400_2483_OPM3;
            break;

        case SUN_OFDM_BAND_863_OPT4:
            *phyConfig = SUN_OFDM_BAND_863_870_OPT4;
            break;

        case SUN_OFDM_BAND_866_OPT4:
            *phyConfig = SUN_OFDM_BAND_865_867_OPT4;
            break;

        case SUN_OFDM_BAND_870_OPT4:
            *phyConfig = SUN_OFDM_BAND_870_876_OPT4;
            break;

        case SUN_OFDM_BAND_915_OPT4:
            *phyConfig = SUN_OFDM_BAND_902_928_OPT4;
            break;

        case SUN_OFDM_BAND_915_OPT3:
            *phyConfig = SUN_OFDM_BAND_902_928_OPT3;
            break;

        case SUN_OFDM_BAND_915_OPT2:
            *phyConfig = SUN_OFDM_BAND_902_928_OPT2;
            break;

        case SUN_OFDM_BAND_915_OPT1:
            *phyConfig = SUN_OFDM_BAND_902_928_OPT1;
            break;

        case SUN_OFDM_BAND_915A_OPT4:
            *phyConfig = SUN_OFDM_BAND_902_928_ALT_OPT4;
            break;

        case SUN_OFDM_BAND_915A_OPT3:
            *phyConfig = SUN_OFDM_BAND_902_928_ALT_OPT3;
            break;

        case SUN_OFDM_BAND_915A_OPT2:
            *phyConfig = SUN_OFDM_BAND_902_928_ALT_OPT2;
            break;

        case SUN_OFDM_BAND_915A_OPT1:
            *phyConfig = SUN_OFDM_BAND_902_928_ALT_OPT1;
            break;

        case SUN_OFDM_BAND_915B_OPT4:
            *phyConfig = SUN_OFDM_BAND_902_907_915_928_OPT4;
            break;

        case SUN_OFDM_BAND_915B_OPT3:
            *phyConfig = SUN_OFDM_BAND_902_907_915_928_OPT3;
            break;

        case SUN_OFDM_BAND_915B_OPT2:
            *phyConfig = SUN_OFDM_BAND_902_907_915_928_OPT2;
            break;

        case SUN_OFDM_BAND_915B_OPT1:
            *phyConfig = SUN_OFDM_BAND_902_907_915_928_OPT1;
            break;

        case SUN_OFDM_BAND_915C_OPT4:
            *phyConfig = SUN_OFDM_BAND_915_928_OPT4;
            break;

        case SUN_OFDM_BAND_915C_OPT3:
            *phyConfig = SUN_OFDM_BAND_915_928_OPT3;
            break;

        case SUN_OFDM_BAND_915C_OPT2:
            *phyConfig = SUN_OFDM_BAND_915_928_OPT2;
            break;

        case SUN_OFDM_BAND_915C_OPT1:
            *phyConfig = SUN_OFDM_BAND_915_928_OPT1;
            break;

        case SUN_OFDM_BAND_2450_OPT4:
            *phyConfig = SUN_OFDM_BAND_2400_2483_OPT4;
            break;

        case SUN_OFDM_BAND_2450_OPT3:
            *phyConfig = SUN_OFDM_BAND_2400_2483_OPT3;
            break;

        case SUN_OFDM_BAND_2450_OPT2:
            *phyConfig = SUN_OFDM_BAND_2400_2483_OPT2;
            break;

        case SUN_OFDM_BAND_2450_OPT1:
            *phyConfig = SUN_OFDM_BAND_2400_2483_OPT1;
            break;

        default:
            return false;
    }

    return true;
}

static inline int32_t _RF215_PHY_USq5ToSysTimeCount(int32_t timeUSq5)
{
    uint32_t sysTimeFreq = SYS_TIME_FrequencyGet();
    return (int32_t) DIV_ROUND((int64_t) timeUSq5 * sysTimeFreq, 32000000);
}

static inline int32_t _RF215_PHY_EventTrxCountDiff(void* pData)
{
    int32_t trxCountDiff;
    uint8_t* pCNT = (uint8_t *) pData;

    /* Read counter (reset at TX start or RX frame start event).
     * BBCn_CNT0 is least significant byte */
    trxCountDiff = (int32_t) pCNT[0];
    trxCountDiff += ((int32_t) pCNT[1] << 8);
    trxCountDiff += ((int32_t) pCNT[2] << 16);
    trxCountDiff += ((int32_t) pCNT[3] << 24);

    /* Compensate delay between SYS_TIME and TRX counter reads */
    trxCountDiff += RF215_SYNC_DELAY_US_Q5;
    trxCountDiff -= (RF215_SPI_BYTE_DURATION_US_Q5 << 1);
    return trxCountDiff;
}

static void _RF215_PHY_CheckAborts(uint8_t trxIdx, bool reset)
{
    RF215_PHY_OBJ* pObj = &rf215PhyObj[trxIdx];

    switch (pObj->phyState)
    {
        case PHY_STATE_RX_HEADER:
        case PHY_STATE_RX_PAYLOAD:
            /* Reception in progress aborted */
            pObj->phyStatistics.rxErrAborted++;
            pObj->phyStatistics.rxErrTotal++;
            if (reset == false)
            {
                pObj->rxAbortState = pObj->phyState;
            }

            if (pObj->ledRxStatus == true)
            {
                /* Turn off RX LED */
                RF215_HAL_LedRx(false);
                pObj->ledRxStatus = false;
            }
            break;

        case PHY_STATE_TX:
            /* Turn off TX LED */
            RF215_HAL_LedTx(false);

        case PHY_STATE_TX_CCA_ED:
            /* ED aborted by leaving RX: Auto mode automatically restored */
            pObj->phyRegs.RFn_EDC = RF215_RFn_EDC_EDM_AUTO;

        case PHY_STATE_TX_TXPREP:
            /* Transmission in progress aborted. Set pending TX confirm. */
            RF215_PHY_SetTxCfm(pObj->txBufObj, RF215_TX_ABORTED);
            break;

        default:
            break;
    }
}

static inline void _RF215_TRX_Command(uint8_t trxIdx, const uint8_t* pCommand)
{
    RF215_HAL_SpiWrite(RF215_RFn_CMD(trxIdx), (void *) pCommand, 1);
}

static inline void _RF215_TRX_CommandSleep(uint8_t trxIdx)
{
    RF215_PHY_OBJ* pObj = &rf215PhyObj[trxIdx];

    /* SLEEP command */
    _RF215_TRX_Command(trxIdx, &rf215RegValues.RFn_CMD.sleep);

    /* Update PHY and TRX states */
    pObj->phyState = PHY_STATE_SLEPT;
    pObj->trxState = RF215_RFn_STATE_RF_RESET;
}

static inline void _RF215_TRX_CommandTrxOff(uint8_t trxIdx)
{
    RF215_PHY_OBJ* pObj = &rf215PhyObj[trxIdx];

    /* TRXOFF command */
    _RF215_TRX_Command(trxIdx, &rf215RegValues.RFn_CMD.trxoff);

    /* Update TRX state */
    pObj->trxState = RF215_RFn_STATE_RF_TRXOFF;
    pObj->trxRdy = false;
}

static inline void _RF215_TRX_CommandTxPrep(uint8_t trxIdx)
{
    RF215_PHY_OBJ* pObj = &rf215PhyObj[trxIdx];
    uint8_t trxState = pObj->trxState;

    /* TXPREP command */
    _RF215_TRX_Command(trxIdx, &rf215RegValues.RFn_CMD.txprep);

    /* Update TRX state */
    pObj->trxState = RF215_RFn_STATE_RF_TXPREP;

    /* Transceiver state transition time [Table 10-7]:
     * TRXOFF -> TXPREP: 200 us Max.
     * RX -> TXPREP: 200 ns Max ("instantaneous" transition).
     * TRXOFF -> TXPREP: 33 us Max. */
    if ((trxState == RF215_RFn_STATE_RF_TRXOFF) || (trxState == RF215_RFn_STATE_RF_TX))
    {
        /* Clear TRXRDY flag (it will be set later in TRXRDY interrupt) */
        pObj->trxRdy = false;
    }
}

static inline void _RF215_TRX_CommandTx(uint8_t trxIdx)
{
    RF215_PHY_OBJ* pObj = &rf215PhyObj[trxIdx];

    /* TX command */
    _RF215_TRX_Command(trxIdx, &rf215RegValues.RFn_CMD.tx);

    /* Turn on TX LED. Update TRX and PHY state */
    RF215_HAL_LedTx(true);
    pObj->trxState = RF215_RFn_STATE_RF_TX;
    pObj->phyState = PHY_STATE_TX;
}

static inline void _RF215_TRX_CommandRx(uint8_t trxIdx)
{
    /* RX command */
    _RF215_TRX_Command(trxIdx, &rf215RegValues.RFn_CMD.rx);

    /* Update TRX state */
    rf215PhyObj[trxIdx].trxState = RF215_RFn_STATE_RF_RX;
}

static inline void _RF215_TRX_CommandReset(uint8_t trxIdx)
{
    RF215_PHY_OBJ* pObj = &rf215PhyObj[trxIdx];

    /* RESET command */
    _RF215_TRX_Command(trxIdx, &rf215RegValues.RFn_CMD.reset);

    /* Update TRX state */
    pObj->trxState = RF215_RFn_STATE_RF_TRXOFF;
}

static void _RF215_TRX_RxListen(uint8_t trxIdx)
{
    RF215_PHY_OBJ* pObj = &rf215PhyObj[trxIdx];
    bool rxCmd = false;

    if (pObj->trxState != RF215_RFn_STATE_RF_RESET)
    {
        /* Update PHY state */
        pObj->phyState = PHY_STATE_RX_LISTEN;
    }

    if (pObj->phyCfgPending == true)
    {
        /* Pending PHY configuration */
        _RF215_PHY_SetPhyConfig(trxIdx, &pObj->phyConfigPending, pObj->channelNumPhyCfgPending);
    }
    switch (pObj->trxState)
    {
        case RF215_RFn_STATE_RF_RESET:
            /* RESET/SLEEP state: do nothing */
            return;

        case RF215_RFn_STATE_RF_TXPREP:
            /* TXPREP state: send RX command if PLL is locked (TRXRDY).
             * If PLL is not locked, RX command will be sent from TRXRDY IRQ. */
            if (pObj->trxRdy == true)
            {
                rxCmd = true;
            }
            break;

        default:
            /* TRXOFF/TX/RX state: send TXPREP and RX commands.
             * 2 commands can be sent consecutively (queue depth: 1 element).
             * If there is TX or RX in progress it will be aborted. */
            _RF215_TRX_CommandTxPrep(trxIdx);
            rxCmd = true;
            break;
    }

    /* If there was an Energy Detection, we need to restore 2 registers:
     * BBCn_PC: Enable Baseband (disabled during ED).
     * RFn_EDD: Restore ED duration for automatic mode.
     * Registers only written if value changes. */
    _RF215_BBC_BaseBandEnable(trxIdx);
    _RF215_RXFE_SetAutoEDD(trxIdx);

    /* RX command */
    if (rxCmd == true)
    {
        _RF215_TRX_CommandRx(trxIdx);
    }
}

static bool _RF215_TRX_SwitchTrxOff(uint8_t trxIdx)
{
    RF215_PHY_OBJ* pObj = &rf215PhyObj[trxIdx];

    switch (pObj->trxState)
    {
        case RF215_RFn_STATE_RF_RESET:
            /* RESET/SLEEP state: Send TRXOFF command to wake-up */
            break;

        case RF215_RFn_STATE_RF_TRXOFF:
            /* Already in TRXOFF state */
            return true;

        default:
            /* TXPREP/RX/TX state: Needed to switch to TRXOFF state */
            if (pObj->trxRdy == false)
            {
                /* Transition to TXPREP in progress. We need to wait for TRXRDY
                 * to send TRXOFF command and make sure TRXOFF is reached. */
                return false;
            }
            else
            {
                /* We can send TRXOFF command making sure TRXOFF will be reached
                 * "instantaneously".
                 * If there is TX or RX in progress it will be aborted. */
                _RF215_PHY_CheckAborts(trxIdx, false);
            }
            break;
    }

    /* TRXOFF command.
     * Transceiver state transition time [Table 10-7]:
     * TXPREP -> TRXOFF: 200 ns Max ("instantaneous" transition).
     * RX -> TRXOFF: 200 ns Max ("instantaneous" transition).
     * RX -> TRXOFF: 200 ns Max ("instantaneous" transition).
     * SLEEP -> TRXOFF: 1 us Typ ("instantaneous" transition).
     * DEEP_SLEEP -> TRXOFF: 500 us Max (indicated by Wake-up interrupt). */
    _RF215_TRX_CommandTrxOff(trxIdx);
    return true;
}

static bool _RF215_TRX_SwitchTxPrep(uint8_t trxIdx)
{
    RF215_PHY_OBJ* pObj = &rf215PhyObj[trxIdx];

    switch (pObj->trxState)
    {
        case RF215_RFn_STATE_RF_RESET:
            /* RESET/SLEEP state: do nothing */
            return false;

        case RF215_RFn_STATE_RF_TXPREP:
            /* Already in TXPREP state or in transition */
            break;

        case RF215_RFn_STATE_RF_RX:
        case RF215_RFn_STATE_RF_TX:
            if (pObj->trxRdy == false)
            {
                /* Transition to TXPREP in progress. We need to wait for TRXRDY
                 * to send TXPREP command and make sure TXPREP is reached. */
                return false;
            }

            /* If there is TX or RX in progress it will be aborted */
            _RF215_PHY_CheckAborts(trxIdx, false);

        default:
            /* TRXOFF/RX/TX state: Needed to switch to TXPREP state */
            _RF215_TRX_CommandTxPrep(trxIdx);
            break;
    }

    /* TXPREP has been reached if TRXDY flag is set.
     * Transceiver state transition time [Table 10-7]:
     * TRXOFF -> TXPREP: 200 us Max.
     * RX -> TXPREP: 200 ns Max ("instantaneous" transition).
     * TX -> TXPREP: 33 us Max.
     * PLL_CH_SW (channel switch time in TXPREP): 100 us Max. */
    return pObj->trxRdy;
}

static void _RF215_TRX_EnableTxContinuousMode(uint8_t trxIdx)
{
    RF215_PHY_OBJ* pObj = &rf215PhyObj[trxIdx];
    
    /* Swith to TRXOFF state before to configure continuous transmission */
    if ((pObj->txAutoInProgress == false) && (_RF215_TRX_SwitchTrxOff(trxIdx) == true))
    {
        uint8_t pac;
        RF215_PHY_REGS_OBJ* phyRegs = &pObj->phyRegs;
        RF215_PHY_OBJ* pObjOther = &rf215PhyObj[RF215_TRX_RF24_IDX];
        uint8_t trxIdxOther = RF215_TRX_RF24_IDX;
        RF215_PHY_STATE phyStateOther;

        if (trxIdx == RF215_TRX_RF24_IDX)
        {
            pObjOther = &rf215PhyObj[RF215_TRX_RF09_IDX];
            trxIdxOther = RF215_TRX_RF09_IDX;
        }

        /* Set other TRX to idle state (TRXOFF) */
        phyStateOther = pObjOther->phyState;
        if ((phyStateOther != PHY_STATE_TX_CONTINUOUS) && (phyStateOther != PHY_STATE_SLEPT))
        {
            if ((pObjOther->txAutoInProgress == true) || (_RF215_TRX_SwitchTrxOff(trxIdxOther) == false))
            {
                pObjOther->idlePending = true;
                pObj->txContinuousPending = true;
                return;
            }

            pObjOther->idlePending = false;
            pObjOther->phyState = PHY_STATE_IDLE;
        }

        /* Disable baseband by Chip Mode (RF_IQIFC1) to generate LO carrier.
         * Chip Mode must be set to 1 (both TRX disabled) */
        if ((rf215PhyRegRF_IQIFC1 & RF215_RF_IQIFC1_CHPM_Msk) == RF215_RF_IQIFC1_CHPM_BBRF)
        {
            /* Write RF_IQIFC1 register */
            rf215PhyRegRF_IQIFC1 = RF215_RF_IQIFC1_SKEWDRV_3_906ns | RF215_RF_IQIFC1_CHPM_RF;
            RF215_HAL_SpiWrite(RF215_RF_IQIFC1, &rf215PhyRegRF_IQIFC1, 1);
        }

        /* Transmitter Power Amplifier Control (RFn_PAC)
         * PAC.PACUR: Power Amplifier Current Control. No power amplifier
         * current reduction to achieve maximum output power.
         * PAC.TXPWR: Maximum Transmitter Output Power. */
        pac = RF215_RFn_PAC_PACUR_0mA | RF215_RFn_PAC_TXPWR_MAX;

        /* Check if TX power changes */
        if (phyRegs->RFn_PAC != pac)
        {
            phyRegs->RFn_PAC = pac;
            RF215_HAL_SpiWrite(RF215_RFn_PAC(trxIdx), &phyRegs->RFn_PAC, 1);
        }

        /* Switch TRX to TXPREP state before sending TX command */
        pObj->txContinuousPending = false;
        _RF215_TRX_CommandTxPrep(trxIdx);

        /* Overwrite DAC values to transmit continuous carrier at channel
         * center */
        phyRegs->RFn_TXDACI = RF215_RFn_TXDACI_ENTXDACID | RF215_RFn_TXDACI_TXDACID(0x7E);
        phyRegs->RFn_TXDACQ = RF215_RFn_TXDACQ_ENTXDACQD | RF215_RFn_TXDACQ_TXDACQD(0x3F);
        RF215_HAL_SpiWrite(RF215_RFn_TXDACI(trxIdx), &phyRegs->RFn_TXDACI, 2);

        /* Send TX command */
        _RF215_TRX_CommandTx(trxIdx);

        /* Update PHY state */
        pObj->phyState = PHY_STATE_TX_CONTINUOUS;

        if (pObjOther->txContinuousPending == true)
        {
            _RF215_TRX_EnableTxContinuousMode(trxIdxOther);
        }
    }
    else
    {
        /* Transition to TXPREP or TX automatic procedure in progress. We need
         * to wait for TRXRDY or automatic procedure end to send TRXOFF command
         * and make sure TRXOFF is reached. */
        pObj->txContinuousPending = true;
    }
}

static void _RF215_TRX_DisableTxContinuousMode(uint8_t trxIdx)
{
    RF215_PHY_OBJ* pObjOther = &rf215PhyObj[RF215_TRX_RF24_IDX];
    uint8_t trxIdxOther = RF215_TRX_RF24_IDX;
    RF215_PHY_OBJ* pObj = &rf215PhyObj[trxIdx];

    /* Stop TX continuous mode with TXPREP command */
    _RF215_TRX_CommandTxPrep(trxIdx);

    /* Restore DAC overwrite values */
    pObj->phyRegs.RFn_TXDACI = 0;
    pObj->phyRegs.RFn_TXDACQ = 0;
    RF215_HAL_SpiWrite(RF215_RFn_TXDACI(trxIdx), &pObj->phyRegs.RFn_TXDACI, 2);

    if (trxIdx == RF215_TRX_RF24_IDX)
    {
        pObjOther = &rf215PhyObj[RF215_TRX_RF09_IDX];
        trxIdxOther = RF215_TRX_RF09_IDX;
    }

    if (pObjOther->phyState == PHY_STATE_TX_CONTINUOUS)
    {
        /* The another TRX is in continuous TX mode */
        _RF215_TRX_CommandTrxOff(trxIdx);
        pObj->phyState = PHY_STATE_IDLE;
    }
    else
    {
        /* Enable baseband by Chip Mode (RF_IQIFC1) */
        rf215PhyRegRF_IQIFC1 = RF215_RF_IQIFC1_SKEWDRV_3_906ns | RF215_RF_IQIFC1_CHPM_BBRF;
        RF215_HAL_SpiWrite(RF215_RF_IQIFC1, &rf215PhyRegRF_IQIFC1, 1);

        /* Start listening again on both TRX */
        _RF215_TRX_RxListen(trxIdx);
        _RF215_TRX_RxListen(trxIdxOther);

        /* Turn off TX LED */
        RF215_HAL_LedTx(false);
    }
}

static void _RF215_TRX_Reset(uint8_t trxIdx)
{
    RF215_PHY_OBJ* pObj = &rf215PhyObj[trxIdx];

    if (pObj->txAutoInProgress == true)
    {
        /* Wait TX automatic procedure to finish */
        pObj->trxResetPending = true;
    }
    else
    {
        if (pObj->phyState == PHY_STATE_TX_CONTINUOUS)
        {
            /* Disable first TX continuous mode */
            _RF215_TRX_DisableTxContinuousMode(trxIdx);
        }

        /* Send RESET command */
        _RF215_TRX_CommandReset(trxIdx);
        pObj->trxResetPending = false;
        pObj->resetInProgress = true;
    }    
}

static void _RF215_TRX_Sleep(uint8_t trxIdx)
{
    RF215_PHY_OBJ* pObj = &rf215PhyObj[trxIdx];

    /* Swith to TRXOFF state before sending SLEEP command */
    if ((pObj->txAutoInProgress == false) && (_RF215_TRX_SwitchTrxOff(trxIdx) == true))
    {
        if (pObj->phyState == PHY_STATE_TX_CONTINUOUS)
        {
            /* Disable first TX continuous mode */
            _RF215_TRX_DisableTxContinuousMode(trxIdx);
        }
        
        /* Send SLEEP command */
        _RF215_TRX_CommandSleep(trxIdx);
        pObj->trxSleepPending = false;
    }
    else
    {
        /* Transition to TXPREP or TX automatic procedure in progress. We need
         * to wait for TRXRDY or automatic procedure end to send TRXOFF command
         * and make sure TRXOFF is reached. */
        pObj->trxSleepPending = true;
    }
}

static inline void _RF215_TRX_ResetEvent(uint8_t trxIdx)
{
    RF215_PHY_REGS_OBJ regsNew;
    RF215_PHY_OBJ* pObj = &rf215PhyObj[trxIdx];
    RF215_PHY_REGS_OBJ* regsOld = &pObj->phyRegs;
    RF215_PHY_STATE phyState = pObj->phyState;
    RF215_REG_VALUES_OBJ* constRegs = (RF215_REG_VALUES_OBJ *) &rf215RegValues;

    if (phyState == PHY_STATE_SLEPT)
    {
        /* Unexpected reset while TRX slept. Send SLEEP command again */
        _RF215_TRX_CommandSleep(trxIdx);
        return;
    }
    else if ((phyState != PHY_STATE_RESET) && (phyState != PHY_STATE_TX_CONTINUOUS) && (phyState != PHY_STATE_IDLE))
    {
        /* Unexpected reset. Abort TX/RX in progress */
        _RF215_PHY_CheckAborts(trxIdx, true);

        /* Reset TRX to process reset under control.
         * Processed in new IRQ. TRX will be in TRXOFF */
        _RF215_TRX_CommandReset(trxIdx);
        pObj->phyState = PHY_STATE_RESET;
        return;
    }

    /* Write 2 registers: RFn_IRQM, RFn_AUXS */
    RF215_HAL_SpiWrite(RF215_RFn_IRQM(trxIdx), &constRegs->RFn_IRQM, 2);

    /* BBCn_PC: Enable baseband for configured PHY type
     * BBCn_IRQM: Enable Baseband Core interrupts */
    regsOld->BBCn_PC = BBC_PC_CFG_BBEN(pObj->phyConfig.phyType);
    regsOld->BBCn_IRQM = RF215_BBCn_IRQ_RXFS | RF215_BBCn_IRQ_RXFE |
            RF215_BBCn_IRQ_TXFE | RF215_BBCn_IRQ_AGCH |
            RF215_BBCn_IRQ_AGCR | RF215_BBCn_IRQ_FBLI;

    /* Write 2 registers: BBCn_IRQM, BBCn_PC */
    RF215_HAL_SpiWrite(RF215_BBCn_IRQM(trxIdx), &regsOld->BBCn_IRQM, 2);

    /* Initial values of RF215 registers after reset */
    regsOld->RFn_CS = RF215_RFn_CS_Rst;
    regsOld->RFn_CCF0L = RF215_RFn_CCF0L_Rst;
    regsOld->RFn_CCF0H = RF215_RFn_CCF0H_Rst;
    regsOld->RFn_CNL = RF215_RFn_CNL_Rst;
    regsOld->RFn_CNM = RF215_RFn_CNM_Rst;
    regsOld->RFn_RXBWC = RF215_RFn_RXBWC_Rst;
    regsOld->RFn_RXDFE = RF215_RFn_RXDFE_Rst;
    regsOld->RFn_AGCC = RF215_RFn_AGCC_Rst;
    regsOld->RFn_AGCS = RF215_RFn_AGCS_Rst;
    regsOld->RFn_EDC = RF215_RFn_EDC_Rst;
    regsOld->RFn_EDD = RF215_RFn_EDD_Rst;
    regsOld->RFn_TXCUTC = RF215_RFn_TXCUTC_Rst;
    regsOld->RFn_TXDFE = RF215_RFn_TXDFE_Rst;
    regsOld->RFn_PAC = RF215_RFn_PAC_Rst;
    regsOld->BBCn_TXFLL = RF215_BBCn_TXFLL_Rst;
    regsOld->BBCn_TXFLH = RF215_BBCn_TXFLH_Rst;
    regsOld->BBCn_FBLIL = RF215_BBCn_FBLIL_Rst;
    regsOld->BBCn_FBLIH = RF215_BBCn_FBLIH_Rst;
    regsOld->BBCn_OFDMPHRTX = RF215_BBCn_OFDMPHRTX_Rst;
    regsOld->BBCn_OFDMPHRRX = RF215_BBCn_OFDMPHRRX_Rst;
    regsOld->BBCn_OFDMC = RF215_BBCn_OFDMC_Rst;
    regsOld->BBCn_OFDMSW = RF215_BBCn_OFDMSW_Rst;
    regsOld->BBCn_AMCS = RF215_BBCn_AMCS_Rst;
    regsOld->BBCn_AMEDT = RF215_BBCn_AMEDT_Rst;
    regsOld->BBCn_FSKC0 = RF215_BBCn_FSKC0_Rst;
    regsOld->BBCn_FSKC1 = RF215_BBCn_FSKC1_Rst;
    regsOld->BBCn_FSKC2 = RF215_BBCn_FSKC2_Rst;
    regsOld->BBCn_FSKC3 = RF215_BBCn_FSKC3_Rst;
    regsOld->BBCn_FSKPHRTX = RF215_BBCn_FSKPHRTX_Rst;
    regsOld->BBCn_FSKDM = RF215_BBCn_FSKDM_Rst;
    regsOld->BBCn_FSKPE0 = RF215_BBCn_FSKPE0_Rst;
    regsOld->BBCn_FSKPE1 = RF215_BBCn_FSKPE1_Rst;
    regsOld->BBCn_FSKPE2 = RF215_BBCn_FSKPE2_Rst;

    /* Obtain new register values depending on PHY configuration */
    _RF215_PLL_Regs(pObj, &rf215PllConst[trxIdx], &regsNew);
    _RF215_BBC_Regs(pObj, &regsNew);
    _RF215_TXRXFE_Regs(pObj, &regsNew);

    /* Write up to 16 registers: RFn_CS to RFn_TXDFE */
    RF215_HAL_SpiWriteUpdate(RF215_RFn_CS(trxIdx),
            &regsNew.RFn_CS, &regsOld->RFn_CS, 16);

    /* Adjust CCA duration (minimum is AGC update time) */
    _RF215_RXFE_AdjustEDD(trxIdx);

    /* Write BBC configuration, depending on PHY type */
    _RF215_BBC_WriteRegs(trxIdx, &regsNew);

    /* Configure Timestamp Counter in free-running mode.
     * Counter reset at TX/RX event */
    RF215_HAL_SpiWrite(RF215_BBCn_CNTC(trxIdx), &constRegs->BBCn_CNTC, 1);

    pObj->trxState = RF215_RFn_STATE_RF_TRXOFF;
    if (phyState == PHY_STATE_TX_CONTINUOUS)
    {
        /* Restore continuous TX mode */
        _RF215_TRX_EnableTxContinuousMode(trxIdx);
    }
    else if (phyState != PHY_STATE_IDLE)
    {
        /* Start listening */
        _RF215_TRX_RxListen(trxIdx);
    }

    pObj->resetInProgress = false;
    if (pObj->txRequestPending == true)
    {
        /* Pending TX request because of TRX reset in progress */
        RF215_PHY_TxRequest(pObj->txBufObjPending);
        pObj->txRequestPending = false;
    }
}

static DRV_RF215_PIB_RESULT _RF215_PHY_SetPhyConfig (
    uint8_t trxIdx,
    DRV_RF215_PHY_CFG_OBJ* phyCfgNew,
    uint16_t chnNumNew
)
{
    RF215_PLL_PARAMS_OBJ pllParamsNew;
    RF215_PHY_REGS_OBJ regsNew;
    bool trxStateReached = false;
    RF215_PHY_OBJ* pObj = &rf215PhyObj[trxIdx];
    DRV_RF215_PHY_CFG_OBJ* phyCfg = &pObj->phyConfig;
    const RF215_PLL_CONST_OBJ* pllConst = &rf215PllConst[trxIdx];

    /* If channel 0, get first available channel */
    if (chnNumNew == 0)
    {
        chnNumNew = phyCfgNew->chnNumMin;
    }

    /* Check if PHY configuration changes */
    if ((memcmp(phyCfgNew, phyCfg, sizeof (DRV_RF215_PHY_CFG_OBJ)) == 0) &&
            (chnNumNew == pObj->channelNum))
    {
        return RF215_PIB_RESULT_SUCCESS;
    }

    /* Check correct PHY configuration */
    if (_RF215_PHY_CheckPhyCfg(phyCfgNew) == false)
    {
        return RF215_PIB_RESULT_INVALID_PARAM;
    }

    /* Compute channel frequency, range and mode of initial configuration */
    _RF215_PLL_Params(pllConst, &pllParamsNew, phyCfgNew, chnNumNew);

    /* Check correct channel configuration */
    if (_RF215_PLL_CheckConfig(pllConst, &pllParamsNew, phyCfgNew, chnNumNew) == false)
    {
        return RF215_PIB_RESULT_INVALID_PARAM;
    }

    /* Check that TX automatic procedure is not in progress */
    if (pObj->phyState == PHY_STATE_RX_LISTEN)
    {
        if ((phyCfgNew->phyType == phyCfg->phyType) &&
            (memcmp(&phyCfgNew->phyTypeCfg, &phyCfg->phyTypeCfg, sizeof(DRV_RF215_PHY_TYPE_CFG_OBJ)) == 0) &&
            (pllParamsNew.freqRng == pObj->pllParams.freqRng))
        {
            /* Only frequency changes and it is within same range.
             * It can be configured in TXPREP state. */
            trxStateReached = _RF215_TRX_SwitchTxPrep(trxIdx);
        }
        else
        {
            /* It must be configured in TRXOFF state */
            trxStateReached = _RF215_TRX_SwitchTrxOff(trxIdx);
        }
    }

    if (trxStateReached == false)
    {
        /* Transition to TXPREP or TX/RX in progress. We need to wait for TRXRDY
         * or TX/RX to finish. */
        pObj->phyCfgPending = true;
        pObj->phyConfigPending = *phyCfgNew;
        pObj->channelNumPhyCfgPending = chnNumNew;
        return RF215_PIB_RESULT_SUCCESS;
    }

    /* Update BBCn_PC if PHY type changes */
    if (phyCfgNew->phyType != phyCfg->phyType)
    {
        _RF215_BBC_SetPhyControl(trxIdx, BBC_PC_CFG_BBEN(phyCfgNew->phyType));

        /* Abort scheduled transmissions because of PHY type change */
        DRV_RF215_AbortTxByPhyConfig(trxIdx);
    }

    /* Store new PHY configuration */
    *phyCfg = *phyCfgNew;
    pObj->channelNum = chnNumNew;
    pObj->pllParams = pllParamsNew;
    pObj->phyCfgPending = false;

    /* Obtain new register values depending on PHY configuration */
    _RF215_PLL_Regs(pObj, pllConst, &regsNew);
    _RF215_BBC_Regs(pObj, &regsNew);
    _RF215_TXRXFE_Regs(pObj, &regsNew);

    /* Write up to 16 registers: RFn_CS to RFn_TXDFE */
    RF215_HAL_SpiWriteUpdate(RF215_RFn_CS(trxIdx),
            &regsNew.RFn_CS, &pObj->phyRegs.RFn_CS, 16);

    /* Adjust CCA duration (minimum is AGC update time) */
    _RF215_RXFE_AdjustEDD(trxIdx);

    /* Write BBC configuration, depending on PHY type */
    _RF215_BBC_WriteRegs(trxIdx, &regsNew);

    /* Start listening. Clear TRXRDY flag for the case in which frequency is
     * updated in TXPREP (within same frequency range). */
    pObj->trxRdy = false;
    _RF215_TRX_RxListen(trxIdx);

    return RF215_PIB_RESULT_SUCCESS;
}

static uint32_t _RF215_TX_ContentionWindowUS (DRV_RF215_TX_BUFFER_OBJ* txBufObj)
{
    uint32_t contentionWindowUS;
    DRV_RF215_PHY_CCA_MODE ccaMode = txBufObj->reqObj.ccaMode;
    uint8_t cw = txBufObj->reqObj.ccaContentionWindow;
    RF215_PHY_OBJ* pObj = &rf215PhyObj[txBufObj->clientObj->trxIndex];

    if (ccaMode == PHY_CCA_OFF)
    {
        /* CCA disabled: No contention window */
        return 0;
    }

    /* Minimum value of CW is 1 */
    if (cw == 0)
    {
        cw = 1;
    }

    /* Add turnaround time */
    contentionWindowUS = (uint32_t) pObj->turnaroundTimeUS * (cw - 1);

    if ((ccaMode == PHY_CCA_MODE_1) || (ccaMode == PHY_CCA_MODE_3))
    {
        /* CCA with energy detection: Add ED duration */
        contentionWindowUS += ((uint32_t) pObj->phyConfig.ccaEdDurationUS * cw);
    }

    return contentionWindowUS;
}

static uint32_t _RF215_TX_CommandDelayUSq5(DRV_RF215_TX_BUFFER_OBJ* txBufObj)
{
    const RF215_FSK_SYM_RATE_CONST_OBJ* fskConst;
    DRV_RF215_OFDM_BW_OPT opt;
    uint8_t txdfe, sr;
    RF215_PHY_OBJ* pObj = &rf215PhyObj[txBufObj->clientObj->trxIndex];
    DRV_RF215_PHY_CCA_MODE ccaMode = txBufObj->reqObj.ccaMode;
    uint32_t txCmdDelayUSq5 = RF215_SPI_BYTE_DURATION_US_Q5 * 3;

    /* Common: TX/CCATX SPI command delay + 3 SPI bytes */
    txCmdDelayUSq5 += RF215_TX_CMD_DELAY_US_Q5;

    if ((ccaMode == PHY_CCA_MODE_1) || (ccaMode == PHY_CCA_MODE_3))
    {
        /* Energy detection (EDM_SINGLE) delay */
        txCmdDelayUSq5 += RF215_RX_CCA_ED_TIME_US_Q5;

        /* Contention window length (incudes ED duration) */
        txCmdDelayUSq5 += (_RF215_TX_ContentionWindowUS(txBufObj) << 5);

        if (txBufObj->reqObj.ccaContentionWindow <= 1)
        {
            /* Delay of CCATX: ED (RX) -> TXPREP -> TX (2 state transitions) */
            txCmdDelayUSq5 += RF215_RX_TX_TIME_US_Q5;
        }
        else
        {
            /* Contention window: Next CCA is not the last one before TX */
            return txCmdDelayUSq5;
        }
    }
    else
    {
        /* Delay without ED: TXPREP -> TX transition */
        txCmdDelayUSq5 += RF215_TXPREP_TX_TIME_US_Q5;
    }

    /* No contention window: Next command is TX or CCATX.
     * Add front-end and baseband processing delays.
     * Baseband processing delay depends on PHY type. */
    if (pObj->phyConfig.phyType == PHY_TYPE_FSK)
    {
        /* Baseband processing delay for FSK */
        fskConst = &fskSymRateConst[pObj->phyConfig.phyTypeCfg.fsk.symRate];
        txCmdDelayUSq5 += fskConst->txBaseBandDelayUSq5;

        /* FSK pre-emphasis processing delay in us [uQ5.5] (not in data-sheet).
         * If pre-emphasis is enabled (FSKDM.PE=1), tx_bb_delay is reduced
         * because FSKC0.BT has no effect (GFSK modulator disabled). Delay1 and
         * Delay2 reduce tx_bb_delay. Delay2 has to be compensated in TX confirm
         * time. */
        if ((pObj->phyRegs.BBCn_FSKDM & RF215_BBCn_FSKDM_PE) != 0)
        {
            txCmdDelayUSq5 -= fskConst->txPreEmphasisDelay1USq5;
            txCmdDelayUSq5 -= fskConst->txPreEmphasisDelay2USq5;
        }
    }
    else
    {
        /* Baseband processing delay for OFDM */
        opt = pObj->phyConfig.phyTypeCfg.ofdm.opt;
        txCmdDelayUSq5 += ofdmBwOptConst[opt].txBaseBandDelayUSq5;
    }

    /* Transmitter front-end delay in us [uQ6.5] [Figure 6-3].
     * tx_start_delay: Typ. 4 us [Table 10-7].
     * Transmitter processing delay depends on TXDFE.SR.
     * Higher delay if TXDFE.RCUT != 4 (1.00). */
    txdfe = pObj->phyRegs.RFn_TXDFE;
    sr = (txdfe & RF215_RFn_TXDFE_SR_Msk) >> RF215_RFn_TXDFE_SR_Pos;
    txCmdDelayUSq5 += RF215_TX_START_DELAY_US_Q5;
    if ((txdfe & RF215_RFn_TXDFE_RCUT_Msk) == RF215_RFn_TXDFE_RCUT_1_00)
    {
        txCmdDelayUSq5 += rf215TxDfeProcDelay[sr];
    }
    else
    {
        txCmdDelayUSq5 += rf215TxDfeProcRcutDelay[sr];
    }

    return txCmdDelayUSq5;
}

static uint32_t _RF215_TX_PrepareDelayUSq5(DRV_RF215_TX_BUFFER_OBJ* txBufObj)
{
    uint32_t txPrepDelayUSq5 = RF215_TX_TRXRDY_DELAY_US_Q5;
    DRV_RF215_PHY_CCA_MODE ccaMode = txBufObj->reqObj.ccaMode;
    uint8_t spiBytes = 6;
    bool setLen = true;

    /* Common: 6 SPI bytes (TXPREP command, BBCn_AMCS) +
     * TRXRDY interrupt delay + IRQ margin + 5000 execution cycles. */
    txPrepDelayUSq5 += RF215_TX_IRQ_MARGIN_US_Q5 + EX_CYCL_TO_USQ5(5000);

    if ((ccaMode == PHY_CCA_MODE_1) || (ccaMode == PHY_CCA_MODE_3))
    {
        /* CCA with ED: 9 SPI bytes (BB disable, ED duration, RX command) */
        spiBytes += 9;

        if (txBufObj->reqObj.ccaContentionWindow > 1)
        {
            /* Contention window: Next ED CCA is not the last one before TX */
            setLen = false;
        }
    }

    if (setLen == true)
    {
        /* 4 SPI bytes (PSDU length) + 5000 execution cycles */
        spiBytes += 4;
        txPrepDelayUSq5 += EX_CYCL_TO_USQ5(5000);
    }

    /* Add duration of SPI bytes */
    txPrepDelayUSq5 += (uint32_t) spiBytes * RF215_SPI_BYTE_DURATION_US_Q5;
    return txPrepDelayUSq5;
}

static uint32_t _RF215_TX_TotalDelay(DRV_RF215_TX_BUFFER_OBJ* txBufObj)
{
    uint32_t txTotalDelayUSq5;
    uint8_t cw = txBufObj->reqObj.ccaContentionWindow;
    DRV_RF215_PHY_CCA_MODE ccaMode = txBufObj->reqObj.ccaMode;

    /* Delay between next SPI command (TX/EDM_SINGLE) and TX start time */
    txTotalDelayUSq5 = _RF215_TX_CommandDelayUSq5(txBufObj);

    /* Add required time before next SPI command in the worst case:
     * TX parameters configuration (only before last command (TX/CCATX)).
     * TX preparation (only if contention window with Energy Detection). */
    if ((cw <= 1) || ((ccaMode != PHY_CCA_MODE_1) && (ccaMode != PHY_CCA_MODE_3)))
    {
        txTotalDelayUSq5 += RF215_TX_PARAM_CFG_DELAY_US_Q5;
    }
    else
    {
        txTotalDelayUSq5 += _RF215_TX_PrepareDelayUSq5(txBufObj);
    }

    /* Convert total delay to SYS_TIME count units */
    return (uint32_t) _RF215_PHY_USq5ToSysTimeCount(txTotalDelayUSq5);
}

static void _RF215_TX_UpdStats(RF215_PHY_OBJ* phyObj, DRV_RF215_TX_RESULT result)
{
    if (result != RF215_TX_SUCCESS)
    {
        phyObj->phyStatistics.txErrTotal++;
    }

    switch (result)
    {
        case RF215_TX_SUCCESS:
            phyObj->phyStatistics.txTotal++;
            phyObj->phyStatistics.txTotalBytes += phyObj->txBufObj->reqObj.psduLen;
            break;

        case RF215_TX_ABORTED:
        case RF215_TX_CANCEL_BY_RX:
        case RF215_TX_CANCELLED:
            phyObj->phyStatistics.txErrAborted++;
            break;

        case RF215_TX_BUSY_TX:
        case RF215_TX_FULL_BUFFERS:
        case RF215_TX_TRX_SLEPT:
            phyObj->phyStatistics.txErrBusyTx++;
            break;

        case RF215_TX_BUSY_RX:
            phyObj->phyStatistics.txErrBusyRx++;
            break;

        case RF215_TX_BUSY_CHN:
            phyObj->phyStatistics.txErrBusyChn++;
            break;

        case RF215_TX_INVALID_LEN:
            phyObj->phyStatistics.txErrBadLen++;
            break;

        case RF215_TX_INVALID_PARAM:
            phyObj->phyStatistics.txErrBadFormat++;
            break;

        case RF215_TX_ERROR_UNDERRUN:
        case RF215_TX_TIMEOUT:
        default:
            phyObj->phyStatistics.txErrTimeout++;
            break;
    }
}

static void _RF215_TX_ReadPS(uintptr_t context, void* pData, uint64_t time)
{
    DRV_RF215_TX_RESULT result = RF215_TX_SUCCESS;
    uint8_t trxIdx = (uint8_t) context;
    RF215_PHY_OBJ* pObj = &rf215PhyObj[trxIdx];

    if ((*((uint8_t *) pData) & RF215_BBCn_PS_TXUR) != 0)
    {
        /* TX underrun error */
        result = RF215_TX_ERROR_UNDERRUN;
    }

    /* Set pending TX confirm */
    RF215_PHY_SetTxCfm(pObj->txBufObj, result);
    pObj->txCancelPending = false;

    /* Check pending TRX sleep, reset or continuous mode */
    if (pObj->trxSleepPending == true)
    {
        _RF215_TRX_Sleep(trxIdx);
        pObj->trxResetPending = false;
    }
    else if (pObj->trxResetPending == true)
    {
        _RF215_TRX_Reset(trxIdx);
    }
    else if (pObj->txContinuousPending == true)
    {
        _RF215_TRX_EnableTxContinuousMode(trxIdx);
    }
    else if (pObj->idlePending == true)
    {
        uint8_t trxIdxOther = RF215_TRX_RF24_IDX;
        if (trxIdx == RF215_TRX_RF24_IDX)
        {
            trxIdxOther = RF215_TRX_RF09_IDX;
        }
        
        /* Set continuous TX mode on the another TRX */
        _RF215_TRX_EnableTxContinuousMode(trxIdxOther);
    }
}

static void _RF215_TX_FrameEnd(uint8_t trxIdx)
{
    RF215_PHY_OBJ* pObj = &rf215PhyObj[trxIdx];
    DRV_RF215_TX_BUFFER_OBJ* txBufObj = pObj->txBufObj;

    if (pObj->phyState == PHY_STATE_TX)
    {
        DRV_RF215_PHY_CCA_MODE ccaMode = txBufObj->reqObj.ccaMode;

        /* Normal case: PHY state is TX */
        if ((ccaMode == PHY_CCA_MODE_1) || (ccaMode == PHY_CCA_MODE_3))
        {
            /* TRX switches automatically to TXPREP. Start listening again. */
            pObj->trxState = RF215_RFn_STATE_RF_TXPREP;
            _RF215_TRX_RxListen(trxIdx);
        }
        else
        {
            /* TRX switches automatically to TXPREP and RX (TX2RX enabled) */
            pObj->trxState = RF215_RFn_STATE_RF_RX;
            pObj->phyState = PHY_STATE_RX_LISTEN;
            pObj->txAutoInProgress = false;

            if (pObj->phyCfgPending == true)
            {
                /* Pending PHY configuration */
                _RF215_PHY_SetPhyConfig(trxIdx, &pObj->phyConfigPending, pObj->channelNumPhyCfgPending);
            }
        }

        /* Turn off TX LED */
        RF215_HAL_LedTx(false);
    }
    else
    {
        /* TX aborted just before, but it has actually finished. TRX state is
         * not changed, but TX confirm is filled, overwriting the aborted
         * result (if confirm still pending). */
        if (txBufObj->inUse == true)
        {
            /* Undo PHY statistics update */
            pObj->phyStatistics.txErrTotal--;
            pObj->phyStatistics.txErrAborted--;
            txBufObj->cfmPending = false;
        }
        else
        {
            /* Confirm already notified. Nothing to do. */
            return;
        }
    }

    /* Read TX Underrun status */
    RF215_HAL_SpiRead(RF215_BBCn_PS(trxIdx), &pObj->phyRegs.BBCn_PS, 1,
            _RF215_TX_ReadPS, (uintptr_t) trxIdx);

    /* Compute duration of the transmitted PPDU */
    txBufObj->cfmObj.ppduDurationCount = _RF215_PHY_PpduDuration(&pObj->phyConfig,
            txBufObj->reqObj.modScheme, txBufObj->reqObj.psduLen,
            &pObj->txPaySymbols);
}

static void _RF215_TX_ReadCNT(uintptr_t ctxt, void* pDat, uint64_t time)
{
    const RF215_FSK_SYM_RATE_CONST_OBJ* fskConst;
    int32_t trxCountDiff;
    RF215_PHY_OBJ* pObj = (RF215_PHY_OBJ *) ctxt;
    uint8_t txdfe = pObj->phyRegs.RFn_TXDFE;
    uint8_t sr = (txdfe & RF215_RFn_TXDFE_SR_Msk) >> RF215_RFn_TXDFE_SR_Pos;

    /* Read counter (reset at TX start event) */
    trxCountDiff = _RF215_PHY_EventTrxCountDiff(pDat);

    /* The TX start event occurs tx_bb_delay after TX command.
     * Compensate transmitter front-end delay in us [uQ6.5] [Figure 6-3].
     * tx_start_delay: Typ. 4 us [Table 10-7].
     * Transmitter processing delay depends on TXDFE.SR.
     * Higher delay if TXDFE.RCUT != 4 (1.00). */
    trxCountDiff -= RF215_TX_START_DELAY_US_Q5;
    if ((txdfe & RF215_RFn_TXDFE_RCUT_Msk) == RF215_RFn_TXDFE_RCUT_1_00)
    {
        trxCountDiff -= rf215TxDfeProcDelay[sr];
    }
    else
    {
        trxCountDiff -= rf215TxDfeProcRcutDelay[sr];
    }

    if (pObj->phyConfig.phyType == PHY_TYPE_FSK)
    {
        /* FSK pre-emphasis processing delay in us [uQ5.5] (not in data-sheet).
         * If pre-emphasis is enabled (FSKDM.PE=1), tx_bb_delay is reduced
         * because FSKC0.BT has no effect (GFSK modulator disabled). Delay1 and
         * Delay2 reduce tx_bb_delay. Delay2 has to be compensated in TX confirm
         * time. */
        if ((pObj->phyRegs.BBCn_FSKDM & RF215_BBCn_FSKDM_PE) != 0)
        {
            fskConst = &fskSymRateConst[pObj->phyConfig.phyTypeCfg.fsk.symRate];
            trxCountDiff += fskConst->txPreEmphasisDelay2USq5;
        }
    }

    /* Compute SYS_TIME counter associated to TX event */
    pObj->txBufObj->cfmObj.timeIniCount = time - (int64_t) _RF215_PHY_USq5ToSysTimeCount(trxCountDiff);
}

static void _RF215_TX_ReadCaptureTimeExpired(uintptr_t context)
{
    bool readTime = true;
    uint8_t trxIdx = (uint8_t) context;
    RF215_PHY_OBJ* pObj = &rf215PhyObj[trxIdx];

    /* Check if TX buffer is still in use */
    if (pObj->txBufObj->inUse == false)
    {
        return;
    }

    /* Critical region to avoid conflicts in PHY object data */
    RF215_HAL_EnterCritical();

    if ((pObj->phyState < PHY_STATE_TX_CCA_ED) && (pObj->txPendingState < PHY_STATE_TX_CCA_ED))
    {
        /* TX has already finished or has been aborted. TX time not read. */
        readTime = false;
    }
    else if (pObj->phyState != PHY_STATE_TX)
    {
        SYS_TIME_HANDLE timeHandle;
        uint32_t timeReadDelay;
        uint32_t timeReadDelayUSq5 = pObj->txCmdDelayUSq5 + (150 << 5);

        /* TX has not started yet. Create new timer to postpone TX time read */
        timeReadDelayUSq5 += RF215_TX_TRXRDY_DELAY_US_Q5;
        timeReadDelayUSq5 += RF215_TX_IRQ_MARGIN_US_Q5;
        timeReadDelay = (uint32_t) _RF215_PHY_USq5ToSysTimeCount(timeReadDelayUSq5);
        timeHandle = SYS_TIME_TimerCreate(0, timeReadDelay,
            _RF215_TX_ReadCaptureTimeExpired, context, SYS_TIME_SINGLE);

        if (timeHandle != SYS_TIME_HANDLE_INVALID)
        {
            /* Start the timer to read TX time later */
            SYS_TIME_TimerStart(timeHandle);
            readTime = false;
        }
    }

    if (readTime == true)
    {
        /* Read counter (reset at TX start event).
         * The TX start event occurs tx_bb_delay after TX command.
         * Read BBCn_CNT0..3 (4 bytes). BBCn_CNT0 is least significant byte. */
        RF215_HAL_SpiRead(RF215_BBCn_CNT0(trxIdx), &pObj->phyRegs.BBCn_CNT0, 4,
                    _RF215_TX_ReadCNT, (uintptr_t) pObj);
    }

    RF215_HAL_LeaveCritical();
}

static void _RF215_TX_ReadAMCS(uintptr_t context, void* pData, uint64_t time)
{
    uint8_t trxIdx = (uint8_t) context;
    uint8_t* pAMCS = (uint8_t *) pData;
    RF215_PHY_OBJ* pObj = &rf215PhyObj[trxIdx];
    uint8_t amcs = *pAMCS;

    /* Clear CCAED to avoid unnecessary write because of read-only bit.
     * CCATX automatic procedure finished. */
    *pAMCS &= ~RF215_BBCn_AMCS_CCAED;
    pObj->txAutoInProgress = false;

    /* Energy Detection finished with CCATX enabled. Check CCAED status bit. */
    if ((amcs & RF215_BBCn_AMCS_CCAED) == 0)
    {
        /* Clear channel. CCATX enables baseband automatically.
         * TRX state is automatically updated to TX. Turn on TX LED. */
        RF215_HAL_LedTx(true);
        pObj->phyRegs.BBCn_PC |= RF215_BBCn_PC_BBEN_Msk;
        pObj->trxState = RF215_RFn_STATE_RF_TX;
        pObj->phyState = PHY_STATE_TX;

        /* Process TXFE interrupt if it is pending */
        if (pObj->txfePending == true)
        {
            _RF215_TX_FrameEnd(trxIdx);
            pObj->txCancelPending = false;
        }
        else
        {
            /* Check pending TX cancel */
            if (pObj->txCancelPending == true)
            {
                pObj->txCancelPending = false;
                RF215_PHY_TxCancel(pObj->txBufObj);
            }
        }
    }
    else
    {
        /* Busy channel. CCATX does not enable baseband automatically.
         * Enable baseband in TXPREP state and start listening. */
        _RF215_TRX_RxListen(trxIdx);

        /* Report busy channel error in TX confirm */
        RF215_PHY_SetTxCfm(pObj->txBufObj, RF215_TX_BUSY_CHN);
        pObj->txCancelPending = false;
    }

    /* Check pending TRX sleep, reset or continuous mode */
    if (pObj->trxSleepPending == true)
    {
        _RF215_TRX_Sleep(trxIdx);
        pObj->trxResetPending = false;
    }
    else if (pObj->trxResetPending == true)
    {
        _RF215_TRX_Reset(trxIdx);
    }
    else if (pObj->txContinuousPending == true)
    {
        _RF215_TRX_EnableTxContinuousMode(trxIdx);
    }
    else if (pObj->idlePending == true)
    {
        uint8_t trxIdxOther = RF215_TRX_RF24_IDX;
        if (trxIdx == RF215_TRX_RF24_IDX)
        {
            trxIdxOther = RF215_TRX_RF09_IDX;
        }

        /* Set continuous TX mode on the another TRX */
        _RF215_TRX_EnableTxContinuousMode(trxIdxOther);
    }
}

static void _RF215_TX_ReadEDV(uintptr_t context, void* pData, uint64_t time)
{
    RF215_PHY_OBJ* pObj = (RF215_PHY_OBJ *) context;
    DRV_RF215_TX_BUFFER_OBJ* txBufObj = pObj->txBufObj;
    int8_t edv = *((int8_t *) pData);

    if (pObj->phyState != PHY_STATE_TX_CCA_ED)
    {
        /* TX aborted: Do nothing */
        return;
    }

    /* ED finished (without CCATX). Read RFn_EDV and compare with threshold. */
    if (edv > pObj->phyConfig.ccaEdThresholdDBm)
    {
        /* Busy channel. Report busy channel error in TX confirm. */
        RF215_PHY_SetTxCfm(txBufObj, RF215_TX_BUSY_CHN);
    }
    else if (txBufObj->timeHandle == SYS_TIME_HANDLE_INVALID)
    {
        /* Clear channel and no time interrupt scheduled: Timeout error. */
        RF215_PHY_SetTxCfm(txBufObj, RF215_TX_TIMEOUT);
    }
}

static inline void _RF215_TX_EnDetectComplete(uint8_t trxIdx)
{
    RF215_PHY_OBJ* pObj = &rf215PhyObj[trxIdx];
    RF215_PHY_REGS_OBJ* phyRegs = &pObj->phyRegs;

    if (pObj->txBufObj->reqObj.ccaContentionWindow == 0)
    {
        /* Energy Detection finished with CCATX enabled.
         * Read BBCn_AMCS to check busy/clear channel. */
        RF215_HAL_SpiRead(RF215_BBCn_AMCS(trxIdx), &phyRegs->BBCn_AMCS, 1,
            _RF215_TX_ReadAMCS, (uintptr_t) trxIdx);
    }
    else
    {
        /* Energy Detection finished with contention window (CCATX disabled).
         * Read RFn_EDV to check busy/clear channel. */
        RF215_HAL_SpiRead(RF215_RFn_EDV(trxIdx), &phyRegs->RFn_EDV, 1,
            _RF215_TX_ReadEDV, (uintptr_t) pObj);

        /* Start listening again before next Energy Detection */
        _RF215_TRX_RxListen(trxIdx);
    }
}

static inline void _RF215_TX_Start(uint8_t trxIdx)
{
    RF215_PHY_OBJ* pObj = &rf215PhyObj[trxIdx];
    DRV_RF215_TX_BUFFER_OBJ* txBufObj = pObj->txBufObj;
    DRV_RF215_PHY_CCA_MODE ccaMode = txBufObj->reqObj.ccaMode;
    bool writeBuffer = true;

    if ((ccaMode == PHY_CCA_MODE_1) || (ccaMode == PHY_CCA_MODE_3))
    {
        if (pObj->trxRdy == false)
        {
            /* We need to make sure that RX state is reached before starting ED.
             * If transition to TXPREP in progress, this function will be called
             * again from TRXRDY interrupt.  */
            pObj->txPendingState = PHY_STATE_TX_CCA_ED;
            return;
        }

        /* Start Energy Detection for CCA */
        pObj->phyRegs.RFn_EDC = RF215_RFn_EDC_EDM_SINGLE;
        RF215_HAL_SpiWrite(RF215_RFn_EDC(trxIdx), &pObj->phyRegs.RFn_EDC, 1);
        pObj->phyState = PHY_STATE_TX_CCA_ED;

        if (txBufObj->reqObj.ccaContentionWindow > 1)
        {
            /* This is not the last ED (contention window) */
            writeBuffer = false;
        }
    }
    else
    {
        if (pObj->phyState != PHY_STATE_TX_TXPREP)
        {
            /* We need to make sure that TXPREP command has been sent before
             * sending TX command. Otherwise, this function will be called
             * again from TRXRDY interrupt.  */
            pObj->txPendingState = PHY_STATE_TX;
            return;
        }

        /* Start transmission with TX command */
        _RF215_TRX_CommandTx(trxIdx);
    }

    if (writeBuffer == true)
    {
        /* Write TX data buffer, including FCS */
        RF215_HAL_SpiWrite(RF215_BBCn_FBTXS(trxIdx), &txBufObj->psdu,
                txBufObj->reqObj.psduLen);

        /* CCATX/TX2RX auto procedure in progress */
        pObj->txAutoInProgress = true;
    }
}

static void _RF215_TX_Prepare(uint8_t trxIdx)
{
    RF215_PHY_REGS_OBJ regsNew;
    uint16_t psduLen;
    RF215_PHY_OBJ* pObj = &rf215PhyObj[trxIdx];
    DRV_RF215_TX_BUFFER_OBJ* txBufObj = pObj->txBufObj;
    RF215_PHY_REGS_OBJ* phyRegs = &pObj->phyRegs;
    DRV_RF215_PHY_CCA_MODE ccaMode = txBufObj->reqObj.ccaMode;
    bool setLen = true;

    /* Switch TRX to TXPREP state to prepare for transmission */
    if (_RF215_TRX_SwitchTxPrep(trxIdx) == false)
    {
        /* If transition to TXPREP in progress, this function will be called
         * again from TRXRDY interrupt */
        pObj->txPendingState = PHY_STATE_TX_TXPREP;
        return;
    }

    if ((ccaMode == PHY_CCA_MODE_1) || (ccaMode == PHY_CCA_MODE_3))
    {
        /* CCA with Energy Detection:
         * Disable baseband to avoid decoding during ED.
         * Set ED Duration register.
         * Switch TRX to RX state for ED. */
        _RF215_BBC_BaseBandDisable(trxIdx);
        _RF215_RXFE_SetEnDetectDuration(trxIdx, pObj->phyConfig.ccaEdDurationUS);
        _RF215_TRX_CommandRx(trxIdx);
        if (txBufObj->reqObj.ccaContentionWindow <= 1)
        {
            /* Enable CCATX auto procedure (disable TX2RX) and set threshold */
            regsNew.BBCn_AMCS = RF215_BBCn_AMCS_CCATX;
            regsNew.BBCn_AMEDT = (uint8_t) pObj->phyConfig.ccaEdThresholdDBm;
        }
        else
        {
            /* Contention window: Disable CCATX auto procedure */
            regsNew.BBCn_AMCS = 0;
            regsNew.BBCn_AMEDT = phyRegs->BBCn_AMEDT;
            setLen = false;
        }
    }
    else
    {
        /* CCA without Energy Detection:
         * Enable TX2RX auto procedure (disable CCATX) */
        regsNew.BBCn_AMCS = RF215_BBCn_AMCS_TX2RX;
        regsNew.BBCn_AMEDT = phyRegs->BBCn_AMEDT;
    }

    /* Write up to 2 registers: BBCn_AMCS, BBCn_AMEDT */
    RF215_HAL_SpiWriteUpdate(RF215_BBCn_AMCS(trxIdx),
            &regsNew.BBCn_AMCS, &phyRegs->BBCn_AMCS, 2);

    if (setLen == true)
    {
        /* Write buffer length (PSDU length, including FCS).
         * Write up to 2 registers: BBCn_TXFLL, BBCn_TXFLH.
         * BBCn_TXFLL must always be written. */
        psduLen = txBufObj->reqObj.psduLen;
        regsNew.BBCn_TXFLL = (uint8_t) (psduLen & 0xFF);
        regsNew.BBCn_TXFLH = (uint8_t) RF215_BBCn_TXFLH_TXFLH(psduLen >> 8);
        phyRegs->BBCn_TXFLL = regsNew.BBCn_TXFLL + 1;
        RF215_HAL_SpiWriteUpdate(RF215_BBCn_TXFLL(trxIdx),
                &regsNew.BBCn_TXFLL, &phyRegs->BBCn_TXFLL, 2);
    }

    /* Update PHY state */
    pObj->phyState = PHY_STATE_TX_TXPREP;
}

static DRV_RF215_TX_RESULT _RF215_TX_ParamCfg(DRV_RF215_TX_BUFFER_OBJ* txBufObj)
{
    RF215_PHY_REGS_OBJ* phyRegs;
    uint8_t* pPHR;
    uint16_t addrPHR;
    uint8_t pac, phrtx, txPwrAtt, mcsIndex;
    uint8_t trxIdx = txBufObj->clientObj->trxIndex;
    RF215_PHY_OBJ* pObj = &rf215PhyObj[trxIdx];
    DRV_RF215_PHY_CCA_MODE ccaMode = txBufObj->reqObj.ccaMode;

    if ((pObj->txStarted == true) && (txBufObj != pObj->txBufObj))
    {
        /* Another TX in progress: Busy TX error */
        return RF215_TX_BUSY_TX;
    }

    if (RF215_PHY_CheckTxContentionWindow(txBufObj) == true)
    {
        /* Contention window requirements not satisfied: Busy RX error */
        return RF215_TX_BUSY_RX;
    }

    /* Check PHY state */
    switch (pObj->phyState)
    {
        case PHY_STATE_RESET:
            return RF215_TX_TIMEOUT;

        case PHY_STATE_SLEPT:
            return RF215_TX_TRX_SLEPT;

        case PHY_STATE_RX_HEADER:
            /* RX in progress: Busy RX error (if CCA uses carrier sense) */
            if ((ccaMode == PHY_CCA_MODE_2) || (ccaMode == PHY_CCA_MODE_3))
            {
                return RF215_TX_BUSY_RX;
            }
            break;

        case PHY_STATE_RX_PAYLOAD:
            /* RX payload in progress: Busy RX error (except for CCA off) */
            if (ccaMode != PHY_CCA_OFF)
            {
                return RF215_TX_BUSY_RX;
            }
            break;

        case PHY_STATE_TX_CONTINUOUS:
        case PHY_STATE_IDLE:
            /* Continuous TX in progress: Busy TX error */
            return RF215_TX_BUSY_TX;

        default:
            break;
    }

    /* Everything OK. Set TX in progress */
    pObj->txStarted = true;
    pObj->txBufObj = txBufObj;
    pObj->txCmdDelayUSq5 = _RF215_TX_CommandDelayUSq5(txBufObj);

    if ((txBufObj->reqObj.ccaContentionWindow > 1) &&
            ((ccaMode == PHY_CCA_MODE_1) || (ccaMode == PHY_CCA_MODE_3)))
    {
        /* TX parameters not configured if contention window (CW > 1) with ED */
        return RF215_TX_SUCCESS;
    }

    /* Check modulation scheme and TX power attenuation parameters */
    txPwrAtt = txBufObj->reqObj.txPwrAtt;
    phrtx = BBC_FSKPHRTX_FEC_OFF;
    phyRegs = &pObj->phyRegs;
    pPHR = &phyRegs->BBCn_FSKPHRTX;
    addrPHR = RF215_BBCn_FSKPHRTX(trxIdx);

    if (pObj->phyConfig.phyType == PHY_TYPE_FSK)
    {
        /* BBCn_FSKPHRTX depending on FEC mode */
        if (txBufObj->reqObj.modScheme == FSK_FEC_ON)
        {
            phrtx = BBC_FSKPHRTX_FEC_ON;
        }
    }
    else
    {
        /* OFDM: Minimum TX power attenuation to comply with EVM requirements */
        mcsIndex = (uint8_t) (txBufObj->reqObj.modScheme - OFDM_MCS_0);
        if (txPwrAtt < 31)
        {
            txPwrAtt += ofdmMcsConst[mcsIndex].minTxPwrAttMin;
        }

        /* BBCn_OFDMPHRTX register value, depending on MCS
         * RB5/17/RB18/RB21: Reserved bits set to 0
         * MCS: Modulation and coding scheme */
        phrtx = RF215_BBCn_OFDMPHRTX_MCS(mcsIndex);
        pPHR = &phyRegs->BBCn_OFDMPHRTX;
        addrPHR = RF215_BBCn_OFDMPHRTX(trxIdx);
    }

    /* Maximum TX power attenuation is 31 dB */
    if (txPwrAtt > 31)
    {
        txPwrAtt = 31;
    }

    /* Transmitter Power Amplifier Control (RFn_PAC)
     * PAC.PACUR: Power Amplifier Current Control. No power amplifier current
     * reduction to achieve maximum output power.
     * PAC.TXPWR: Transmitter Output Power. */
    pac = RF215_RFn_PAC_PACUR_0mA | RF215_RFn_PAC_TXPWR(31 - txPwrAtt);

    /* Check if modulation scheme or TX power change */
    if ((*pPHR != phrtx) || (phyRegs->RFn_PAC != pac))
    {
        /* Register(s) must be configured in TRXOFF state */
        if (_RF215_TRX_SwitchTrxOff(trxIdx) == true)
        {
            /* Write PHRTX register if it changes */
            if (*pPHR != phrtx)
            {
                *pPHR = phrtx;
                RF215_HAL_SpiWrite(addrPHR, pPHR, 1);
            }

            /* Write RFn_PAC register if it changes */
            if (phyRegs->RFn_PAC != pac)
            {
                phyRegs->RFn_PAC = pac;
                RF215_HAL_SpiWrite(RF215_RFn_PAC(trxIdx), &phyRegs->RFn_PAC, 1);
            }

            /* Transmission parameters configured. Update PHY state */
            pObj->phyState = PHY_STATE_TX_CONFIG;
        }
        else
        {
            /* If transition to TXPREP in progress, this function will be called
             * again from TRXRDY interrupt */
            pObj->txPendingState = PHY_STATE_TX_CONFIG;
        }
    }

    return RF215_TX_SUCCESS;
}

static SYS_TIME_HANDLE _RF215_TX_TimeSchedule (
    uint64_t time,
    bool force,
    SYS_TIME_CALLBACK timeCallback,
    uintptr_t timeContext
)
{
    int64_t txIntDelay;
    uint32_t txIntMargin, txTimeMaxError;
    SYS_TIME_HANDLE timeHandle = SYS_TIME_HANDLE_INVALID;

    /* Add margin for higher/same priority interrupts or critical regions */
    txIntMargin = (uint32_t) _RF215_PHY_USq5ToSysTimeCount(RF215_TX_TIME_IRQ_DELAY_US_Q5);
    time -= txIntMargin;

    /* Remaining time until the SYS_TIME interrupt is needed.
     * It must fit in 32 bits.
     * If it is already late, the maximum error must not be exceeded. */
    txIntDelay = (int64_t) time - SYS_TIME_Counter64Get();
    txTimeMaxError = SYS_TIME_USToCount(DRV_RF215_MAX_TX_TIME_DELAY_ERROR_US);
    if ((txIntDelay > UINT32_MAX) || (txIntDelay < (-((int64_t) txTimeMaxError))))
    {
        /* Error: delay too long or it is too late for the requested time */
        return timeHandle;
    }

    if (force == true)
    {
        if (txIntDelay <= 0)
        {
            /* We are late. Generate time interrupt "immediately". */
            txIntDelay = 1;
        }
    }
    else
    {
        if (txIntDelay < (int64_t) txIntMargin)
        {
            /* No time to wait for new time interrupt */
            return timeHandle;
        }
    }

    /* Create timer to schedule TX configuration/preparation or start */
    timeHandle = SYS_TIME_TimerCreate(0, (uint32_t) txIntDelay, timeCallback,
            timeContext, SYS_TIME_SINGLE);

    /* Start the timer */
    SYS_TIME_TimerStart(timeHandle);

    return timeHandle;
}

static void _RF215_TX_StartTimeExpired(uintptr_t context)
{
    uint64_t currentTime, txCommandTime;
    uint32_t txCommandDelay;
    bool spiFree;
    uint8_t trxIdx = (uint8_t) context;
    RF215_PHY_OBJ* pObj = &rf215PhyObj[trxIdx];
    DRV_RF215_TX_BUFFER_OBJ* txBufObj = pObj->txBufObj;
    uint32_t startDelayUSq5 = 0;
    uint64_t txTime = txBufObj->reqObj.timeCount;
    SYS_TIME_HANDLE timeHandle = SYS_TIME_HANDLE_INVALID;

    /* Check if TX buffer is still in use */
    if (txBufObj->inUse == false)
    {
        return;
    }

    /* Critical region to avoid new SPI transfers */
    spiFree = RF215_HAL_SpiLock();

    if (pObj->txStarted == false)
    {
        /* TX aborted: Nothing to do */
        RF215_HAL_SpiUnlock();
        return;
    }

    if (spiFree == false)
    {
        /* SPI is not free. Add estimated delay for SPI queue to be empty. */
        size_t spiQueueSize = RF215_HAL_GetSpiQueueSize();
        startDelayUSq5 += (uint32_t) spiQueueSize * RF215_SPI_BYTE_DURATION_US_Q5;
        startDelayUSq5 += EX_CYCL_TO_USQ5(2000);
    }

    if (pObj->trxRdy == false)
    {
        /* Transition to TXPREP in progress. Add estimated TRXRDY delay. */
        startDelayUSq5 += RF215_TX_TRXRDY_DELAY_US_Q5;
    }

    /* Time when next command (TX/EDM_SINGLE) has to be sent */
    txCommandDelay = (uint32_t) _RF215_PHY_USq5ToSysTimeCount(pObj->txCmdDelayUSq5);
    txCommandTime = txTime - txCommandDelay;
    currentTime = SYS_TIME_Counter64Get();

    if ((startDelayUSq5 > 0) && (txCommandTime > currentTime))
    {
        uint32_t startDelay;

        /* Not ready to start and it is not too late.
         * Create new timer to start TX later. */
        startDelay = (uint32_t) _RF215_PHY_USq5ToSysTimeCount(startDelayUSq5);
        timeHandle = SYS_TIME_TimerCreate(0, startDelay,
                _RF215_TX_StartTimeExpired, context, SYS_TIME_SINGLE);

        /* Start the timer */
        SYS_TIME_TimerStart(timeHandle);
    }

    /* If no timer created, we can start TX */
    if (timeHandle == SYS_TIME_HANDLE_INVALID)
    {
        uint64_t criticalTime;
        uint32_t txTimeMaxError;
        bool intStatus;

        /* Maximum TX time error delay allowed */
        txTimeMaxError = SYS_TIME_USToCount(DRV_RF215_MAX_TX_TIME_DELAY_ERROR_US);

        /* Wait until critical time (25 us before command) is reached */
        criticalTime = txCommandTime - SYS_TIME_USToCount(25);
        while (criticalTime > currentTime)
        {
            currentTime = SYS_TIME_Counter64Get();
        }

        /* Critical region to avoid delays in TX start.
         * Wait until TX command (TX/EDM_SINGLE) time is reached. */
        intStatus = SYS_INT_Disable();
        while (txCommandTime > currentTime)
        {
            currentTime = SYS_TIME_Counter64Get();
        }

        /* Check that TX time error delay is not exceeded */
        if ((currentTime - txCommandTime) < txTimeMaxError)
        {
            _RF215_TX_Start(trxIdx);
        }

        /* Leave critical region */
        SYS_INT_Restore(intStatus);

        if ((pObj->phyState > PHY_STATE_TX_TXPREP) || (pObj->txPendingState > PHY_STATE_TX_TXPREP))
        {
            uint32_t timeReadDelay, timeReadDelayUSq5;
            DRV_RF215_PHY_CCA_MODE ccaMode = txBufObj->reqObj.ccaMode;

            /* Update contention window counter (CW) */
            if ((txBufObj->reqObj.ccaContentionWindow > 0) &&
                ((ccaMode == PHY_CCA_MODE_1) || (ccaMode == PHY_CCA_MODE_3)))
            {
                txBufObj->reqObj.ccaContentionWindow--;
            }

            if ((txBufObj->reqObj.ccaContentionWindow > 0) &&
                ((ccaMode == PHY_CCA_MODE_1) || (ccaMode == PHY_CCA_MODE_3)))
            {
                uint64_t interruptTime;
                uint32_t txTotalDelay;

                /* There are pending ED for CCA contention window.
                 * Compute total TX delay (worst case) for next ED.
                 * Schedule new timer for next TX configuration/preparation */
                txTotalDelay = _RF215_TX_TotalDelay(txBufObj);
                interruptTime = txTime - txTotalDelay;
                timeHandle = _RF215_TX_TimeSchedule(interruptTime, false,
                        _RF215_TX_PrepareTimeExpired, (uintptr_t) txBufObj);
            }
            else
            {
                /* No ED contention window. At least TX command delay is needed
                 * for the TX capture event time to be ready in RF215.
                 * Add 150 us for margin. */
                timeReadDelayUSq5 = pObj->txCmdDelayUSq5 + (150 << 5);
                if (pObj->phyState <= PHY_STATE_TX_TXPREP)
                {
                    /* TX/CCATX command has not been sent. Add TRXRDY delay. */
                    timeReadDelayUSq5 += RF215_TX_TRXRDY_DELAY_US_Q5;
                    timeReadDelayUSq5 += RF215_TX_IRQ_MARGIN_US_Q5;
                }

                /* Create and start timer to read TX capture event time */
                timeReadDelay = (uint32_t) _RF215_PHY_USq5ToSysTimeCount(timeReadDelayUSq5);
                timeHandle = SYS_TIME_TimerCreate(0, timeReadDelay,
                    _RF215_TX_ReadCaptureTimeExpired, context, SYS_TIME_SINGLE);
                SYS_TIME_TimerStart(timeHandle);
            }

            /* Update SYS_TIME handle */
            txBufObj->timeHandle = timeHandle;
        }
        else
        {
            /* Continue listening and set pending TX confirm with TX error */
            _RF215_TRX_RxListen(trxIdx);
            RF215_PHY_SetTxCfm(txBufObj, RF215_TX_TIMEOUT);
        }
    }

    RF215_HAL_SpiUnlock();
}

static void _RF215_TX_PrepareTimeExpired(uintptr_t context)
{
    DRV_RF215_TX_RESULT result;
    DRV_RF215_TX_BUFFER_OBJ* txBufObj = (DRV_RF215_TX_BUFFER_OBJ *) context;
    uint8_t trxIdx = txBufObj->clientObj->trxIndex;
    RF215_PHY_OBJ* pObj = &rf215PhyObj[trxIdx];

    /* Check if TX buffer is still in use */
    if (txBufObj->inUse == false)
    {
        return;
    }

    /* Critical region to avoid conflicts in PHY object data */
    RF215_HAL_EnterCritical();

    if ((pObj->phyState == PHY_STATE_TX_CCA_ED) || (pObj->txPendingState == PHY_STATE_TX_CCA_ED))
    {
        /* CCA ED still in progress. New interrupt for later (ED duration) */
        txBufObj->timeHandle = SYS_TIME_CallbackRegisterUS(_RF215_TX_PrepareTimeExpired,
                context, pObj->phyConfig.ccaEdDurationUS, SYS_TIME_SINGLE);

        RF215_HAL_LeaveCritical();
        return;
    }

    if (txBufObj->cfmPending == true)
    {
        /* TX confirm is already pending: Nothing to do */
        RF215_HAL_LeaveCritical();
        return;
    }

    /* Carrier sense CCA and TX parameters configuration */
    result = _RF215_TX_ParamCfg(txBufObj);

    if (result == RF215_TX_SUCCESS)
    {
        uint64_t interruptTime;
        uint64_t txTime = txBufObj->reqObj.timeCount;
        SYS_TIME_HANDLE timeHandle = SYS_TIME_HANDLE_INVALID;

        /* Schedule a new time interrupt (if TX parameters not configured) */
        if ((pObj->phyState < PHY_STATE_TX_CONFIG) && (pObj->txPendingState < PHY_STATE_TX_CONFIG))
        {
            uint32_t txPrepDelayUSq5;

            /* Delay between next command (TX/EDM_SINGLE) and TX start time.
             * Add required time for TX preparation.
             * Convert total delay to SYS_TIME count units. */
            txPrepDelayUSq5 = pObj->txCmdDelayUSq5 + _RF215_TX_PrepareDelayUSq5(txBufObj);
            interruptTime = txTime - (uint32_t) _RF215_PHY_USq5ToSysTimeCount(txPrepDelayUSq5);

            /* Schedule timer for TX configuration/preparation */
            timeHandle = _RF215_TX_TimeSchedule(interruptTime, false,
                    _RF215_TX_PrepareTimeExpired, context);
        }

        if (timeHandle == SYS_TIME_HANDLE_INVALID)
        {
            /* No time to wait for new interrupt: Prepare transmission */
            _RF215_TX_Prepare(trxIdx);

            /* Delay between next command (TX/EDM_SINGLE) and TX start time */
            interruptTime = txTime - (uint32_t) _RF215_PHY_USq5ToSysTimeCount(pObj->txCmdDelayUSq5);

            /* Schedule timer for TX start */
            timeHandle = _RF215_TX_TimeSchedule(interruptTime, true,
                    _RF215_TX_StartTimeExpired, (uintptr_t) trxIdx);
        }

        if (timeHandle == SYS_TIME_HANDLE_INVALID)
        {
            /* Error: Timer could not be started. Continue listening. */
            _RF215_TRX_RxListen(trxIdx);
            result = RF215_TX_TIMEOUT;
        }
        else
        {
            /* Timer to prepare/start TX scheduled successfully */
            txBufObj->timeHandle = timeHandle;
        }
    }

    if (result != RF215_TX_SUCCESS)
    {
        /* Set pending TX confirm with TX error */
        RF215_PHY_SetTxCfm(txBufObj, result);
    }

    RF215_HAL_LeaveCritical();
}

static void _RF215_RX_PsduEnd(uint8_t trxIdx, bool fcsOk)
{
    RF215_PHY_OBJ* pObj = &rf215PhyObj[trxIdx];
    uint16_t psduLen = pObj->rxInd.psduLen;
    uint16_t offset = pObj->rxBufferOffset;
    uint16_t addrEDV = RF215_RFn_EDV(trxIdx);
    uintptr_t ctxt = (uintptr_t) &pObj->rxIndPending;
    void* pRSSI = &pObj->rxInd.rssiDBm;

    if (psduLen > offset)
    {
        void* buf = &pObj->rxPsdu[offset];
        uint16_t pending = psduLen - offset;
        uint16_t addrFBRX = RF215_BBCn_FBRXS(trxIdx) + offset;

        /* Get Energy Detection Value (RRSI) */
        RF215_HAL_SpiRead(addrEDV, pRSSI, 1, NULL, 0);

        /* Read the remaining bytes of the received PSDU (part of the payload
         * can be read in FBLI interrupt) */
        RF215_HAL_SpiRead(addrFBRX, buf, pending, _RF215_PHY_SetFlag, ctxt);
    }
    else
    {
        /* PSDU already read in FBLI IRQ. Get Energy Detection Value (RRSI) */
        RF215_HAL_SpiRead(addrEDV, pRSSI, 1, _RF215_PHY_SetFlag, ctxt);
    }

    /* FCS correct flag */
    pObj->rxInd.fcsOk = fcsOk;

    /* Check if there are programmed TX to cancel */
    DRV_RF215_AbortTxByRx(trxIdx);
}

static void _RF215_RX_FrameEnd(uint8_t trxIdx)
{
    RF215_PHY_OBJ* pObj = &rf215PhyObj[trxIdx];

    if (pObj->phyState == PHY_STATE_RX_PAYLOAD)
    {
        /* Normal case: TRX switches automatically to TXPREP.
         * Continue listening. */
        pObj->trxState = RF215_RFn_STATE_RF_TXPREP;
        _RF215_TRX_RxListen(trxIdx);
    }

    /* If automatic FCS is enabled, RXFE interrupt means that FCS is valid
     * (FCS Filter is enabled: BBCn_PC.FCSFE).
     * Process the end of completed PSDU reception. Read pending bytes from
     * RX Frame Buffer and Energy Detection Value (RRSI) */
	_RF215_RX_PsduEnd(trxIdx, true);
    pObj->phyStatistics.rxTotal++;
    pObj->phyStatistics.rxTotalBytes += pObj->rxInd.psduLen;
}

static void _RF215_RX_AgcReleaseReadFBL(uintptr_t context, void* pData, uint64_t time)
{
    uint16_t bufLevel;
    uint8_t trxIdx = (uint8_t) context;
    RF215_PHY_OBJ* pObj = &rf215PhyObj[trxIdx];
    uint8_t* pFBL = (uint8_t *) pData;

    /* Get frame buffer level from BBCn_FBLL and BBCn_FBLH */
    pFBL[1] &= RF215_BBCn_FBLH_FBLH_Msk;
    bufLevel = pFBL[0];
    bufLevel += ((uint16_t)pFBL[1] << 8);

    if (bufLevel == pObj->rxInd.psduLen)
    {
        /* PSDU received completely, but no RXFE interrupt. It
         * means that the FCS is not valid.
         * Process the end of completed PSDU reception. Read pending bytes from
         * RX Frame Buffer and Energy Detection Value (RRSI). */
        _RF215_RX_PsduEnd(trxIdx, false);
        pObj->phyStatistics.rxErrBadFcsPay++;
        pObj->phyStatistics.rxErrTotal++;
    }
    else
    {
        /* AGC released before PSDU completion (overridden by higher RSSI) */
        pObj->phyStatistics.rxOverride++;
    }
}

static void _RF215_RX_AgcRelease(uint8_t trxIdx)
{
    RF215_PHY_OBJ* pObj = &rf215PhyObj[trxIdx];
    RF215_PHY_STATE phyState = pObj->phyState;
    bool listen = false;

    /* Check current PHY state */
    if (phyState == PHY_STATE_RX_HEADER)
    {
        /* AGC released before RXFS interrupt. RF215 continues listening */
        pObj->phyStatistics.rxErrFalsePositive++;
        pObj->phyStatistics.rxErrTotal++;
        listen = true;
    }
    else if (phyState == PHY_STATE_RX_PAYLOAD)
    {
        /* Get Frame Buffer Level: Read 2 registers (BBCn_FBLL, BBCn_FBLH) */
        RF215_HAL_SpiRead(RF215_BBCn_FBLL(trxIdx), &pObj->phyRegs.BBCn_FBLL, 2,
                _RF215_RX_AgcReleaseReadFBL, (uintptr_t) trxIdx);
        listen = true;
    }

    if (listen == true)
    {
        /* RF215 continues listening */
        pObj->phyState = PHY_STATE_RX_LISTEN;

        /* Check pending PHY configuration */
        if (pObj->phyCfgPending == true)
        {
            _RF215_PHY_SetPhyConfig(trxIdx, &pObj->phyConfigPending, pObj->channelNumPhyCfgPending);
        }
    }
}

static void _RF215_RX_BuffLvlIntReadFBL(uintptr_t context, void* pData, uint64_t time)
{
    uint16_t bufLevel;
    uint8_t trxIdx = (uint8_t) context;
    RF215_PHY_OBJ* pObj = &rf215PhyObj[trxIdx];
    uint8_t* pFBL = (uint8_t *) pData;

    if (pObj->phyState != PHY_STATE_RX_PAYLOAD)
    {
        /* RX aborted just after FBLI interrupt */
        return;
    }

    /* Get frame buffer level from BBCn_FBLL and BBCn_FBLH */
    pFBL[1] &= RF215_BBCn_FBLH_FBLH_Msk;
    bufLevel = pFBL[0];
    bufLevel += ((uint16_t) pFBL[1] << 8);

    if ((bufLevel == 0) || (bufLevel > pObj->rxInd.psduLen))
    {
        /* Invalid buffer level */
        return;
    }

    /* Read PSDU bytes already stored in RX Frame Buffer */
    RF215_HAL_SpiRead(RF215_BBCn_FBRXS(trxIdx), pObj->rxPsdu, bufLevel, NULL, 0);
    pObj->rxBufferOffset = bufLevel;
}

static inline void _RF215_RX_BuffLvlInt(uint8_t trxIdx)
{
    RF215_PHY_OBJ* pObj = &rf215PhyObj[trxIdx];

    if (pObj->phyState != PHY_STATE_RX_PAYLOAD)
    {
        /* RX aborted or FBLI and RXFE were captured in the same IRQ */
        return;
    }

    /* Get Frame Buffer Level to know how many bytes can be read:
     * Read 2 registers (BBCn_FBLL, BBCn_FBLH) */
    RF215_HAL_SpiRead(RF215_BBCn_FBLL(trxIdx), &pObj->phyRegs.BBCn_FBLL, 2,
            _RF215_RX_BuffLvlIntReadFBL, (uintptr_t) trxIdx);
}

static void _RF215_RX_ReadCNT(uintptr_t ctxt, void* pDat, uint64_t time)
{
    int32_t trxCountDiff;
    RF215_PHY_OBJ* pObj = (RF215_PHY_OBJ *) ctxt;
    uint8_t rxdfe = pObj->phyRegs.RFn_RXDFE;
    DRV_RF215_PHY_CFG_OBJ *phyCfg = &pObj->phyConfig;

    /* Read counter (reset at RX frame start event) */
    trxCountDiff = _RF215_PHY_EventTrxCountDiff(pDat);

    /* Compensate RXFS interrupt and RX baseband delays */
    if (phyCfg->phyType == PHY_TYPE_FSK)
    {
        trxCountDiff += _RF215_FSK_RxStartDelayUSq5(&phyCfg->phyTypeCfg.fsk, pObj->rxInd.modScheme);
    }
    else /* PHY_TYPE_OFDM */
    {
        trxCountDiff += _RF215_OFDM_RxStartDelayUSq5(&phyCfg->phyTypeCfg.ofdm);
    }

    /* Compensate RX processing delay */
    if ((rxdfe & RF215_RFn_RXDFE_RCUT_Msk) != RF215_RFn_RXDFE_RCUT_1_00)
    {
        /* Delay with RXDFE.RCUT != 4: Depends on RXDFE.SR */
        uint8_t sr = (rxdfe & RF215_RFn_RXDFE_SR_Msk) >> RF215_RFn_RXDFE_SR_Pos;
        trxCountDiff += rf215RxDfeProcDelay[sr];
    }

    /* Compute SYS_TIME counter associated to RX event */
    pObj->rxInd.timeIniCount = time - (int64_t) _RF215_PHY_USq5ToSysTimeCount(trxCountDiff);
    pObj->rxTimeValid = true;
}

static void _RF215_RX_ReadPHR(uintptr_t context, void* pData, uint64_t time)
{
    uint16_t psduLen;
    DRV_RF215_PHY_MOD_SCHEME modScheme = FSK_FEC_OFF;
    bool phrErr = false;
    uint8_t phr = *((uint8_t *) pData);
    uint8_t trxIdx = (uint8_t) context;
    RF215_PHY_OBJ* pObj = &rf215PhyObj[trxIdx];
    RF215_PHY_REGS_OBJ* regsOld = &pObj->phyRegs;
    DRV_RF215_PHY_CFG_OBJ *phyCfg = &pObj->phyConfig;

    if (pObj->phyState != PHY_STATE_RX_PAYLOAD)
    {
        /* RX aborted */
        return;
    }

    /* Get frame length from BBCn_RXFLL and BBCn_RXFLH */
    regsOld->BBCn_RXFLH &= RF215_BBCn_RXFLH_RXFLH_Msk;
    psduLen = regsOld->BBCn_RXFLL;
    psduLen += ((uint16_t) regsOld->BBCn_RXFLH << 8);

    if ((psduLen <= DRV_RF215_FCS_LEN) || (psduLen > DRV_RF215_MAX_PSDU_LEN))
    {
        /* Invalid length in received PHR */
        phrErr = true;
        pObj->phyStatistics.rxErrBadLen++;
    }
    else
    {
        /* Check received PHR */
        if (phyCfg->phyType == PHY_TYPE_FSK)
        {
            modScheme = _RF215_FSK_ReadPHR(phr);
        }
        else /* PHY_TYPE_OFDM */
        {
            modScheme = _RF215_OFDM_ReadPHR(phr, phyCfg->phyTypeCfg.ofdm.opt);
        }

        if (modScheme == MOD_SCHEME_INVALID)
        {
            /* Invalid PHR */
            phrErr = true;
            pObj->phyStatistics.rxErrBadFormat++;
        }
    }

    if (phrErr == false)
    {
        /* PHR and frame length are valid */
        if (pObj->rxFlagsPending == 0)
        {
            /* Compute number of bytes to read in FBLI interrupt, optimized to
             * read less bytes as possible in RXFE interrupt, depending on the
             * RF frame parameters and SPI interface */
            uint16_t fbli = _RF215_BBC_GetBestFBLI(phyCfg, modScheme, psduLen);
            _RF215_BBC_SetFBLI(trxIdx, fbli);
        }

        /* Read counter (reset at RX frame start event).
         * The RX frame start event complies to the interrupt RXFS.
         * Read BBCn_CNT0..3 (4 bytes). BBCn_CNT0 is least significant byte.
         * Read from tasks to avoid issues with SYS_TIME_Counter64Get(). */
        RF215_HAL_SpiReadFromTasks(RF215_BBCn_CNT0(trxIdx), &pObj->phyRegs.BBCn_CNT0,
                    4, _RF215_RX_ReadCNT, (uintptr_t) pObj);

        /* Check pending RX indication */
        if (pObj->rxIndPending == true)
        {
            /* RX indication not handled, overridden by new received PSDU */
            pObj->rxIndPending = false;
            pObj->phyStatistics.rxIndNotHandled++;
        }

        /* Update RX indication parameters */
        pObj->rxBufferOffset = 0;
        pObj->rxInd.psduLen = psduLen;
        pObj->rxInd.modScheme = modScheme;
        pObj->rxInd.ppduDurationCount = _RF215_PHY_PpduDuration(&pObj->phyConfig,
            modScheme, psduLen, &pObj->rxPaySymbols);

        /* Process RXFE/AGCR interrupt if it is pending */
        if ((pObj->rxFlagsPending & RF215_BBCn_IRQ_RXFE) != 0)
        {
            _RF215_RX_FrameEnd(trxIdx);
        }
        else if ((pObj->rxFlagsPending & RF215_BBCn_IRQ_AGCR) != 0)
        {
            _RF215_RX_AgcRelease(trxIdx);
        }
        else
        {
            /* Turn on RX LED */
            RF215_HAL_LedRx(true);
            pObj->ledRxStatus = true;
        }
    }
    else
    {
        /* Abort ongoing RX and start listening again */
        _RF215_TRX_RxListen(trxIdx);

        /* Update PHY statistics */
        pObj->phyStatistics.rxErrTotal++;
    }
}

static inline void _RF215_RX_FrameStart(uint8_t trxIdx)
{
    RF215_PHY_OBJ* pObj = &rf215PhyObj[trxIdx];
    RF215_PHY_REGS_OBJ* regs = &pObj->phyRegs;
    uint8_t* pPHR = &regs->BBCn_FSKPHRRX;
    uint16_t addrPHR = RF215_BBCn_FSKPHRRX(trxIdx);

    /* Get frame length: Read 2 registers (BBCn_RXFLL, BBCn_RXFLH) */
    RF215_HAL_SpiRead(RF215_BBCn_RXFLL(trxIdx), &regs->BBCn_RXFLL, 2, NULL, 0);

    if (pObj->phyConfig.phyType == PHY_TYPE_OFDM)
    {
        /* PHRRX register for OFDM */
        pPHR = &regs->BBCn_OFDMPHRRX;
        addrPHR = RF215_BBCn_OFDMPHRRX(trxIdx);
    }

    /* Get PHRRX register */
    RF215_HAL_SpiRead(addrPHR, pPHR, 1, _RF215_RX_ReadPHR, (uintptr_t) trxIdx);

    /* PHY continues receiving the payload */
    pObj->phyState = PHY_STATE_RX_PAYLOAD;
}

static inline void _RF215_RX_AgcHold(RF215_PHY_OBJ* phyObj)
{
    RF215_PHY_STATE phyState = phyObj->phyState;
    if ((phyState == PHY_STATE_RX_HEADER) || (phyState == PHY_STATE_RX_PAYLOAD))
    {
        /* New preamble detected while reception in progress (overridden
         * by higher signal level detected) */
        phyObj->phyStatistics.rxOverride++;
    }

    /* Preamble detected: Update PHY state to RX_HEADER (SHR + PHR) */
    phyObj->phyState = PHY_STATE_RX_HEADER;
}

// *****************************************************************************
// *****************************************************************************
// Section: RF215 Driver PHY Interface Implementation
// *****************************************************************************
// *****************************************************************************

bool RF215_PHY_Initialize (
    uint8_t trxIdx,
    DRV_RF215_PHY_BAND_OPM bandOpMode,
    uint16_t channelNum
)
{
    DRV_RF215_PHY_CFG_OBJ phyConfig;
    RF215_PHY_OBJ* pObj = &rf215PhyObj[trxIdx];
    RF215_PLL_PARAMS_OBJ* pllParams = &pObj->pllParams;
    const RF215_PLL_CONST_OBJ* pllConst = &rf215PllConst[trxIdx];

    /* Convert frequency band / operating mode to PHY configuration object */
    if (_RF215_PHY_BandOpModeToPhyCfg(bandOpMode, &phyConfig) == false)
    {
        /* Invalid frequency band / operating mode */
        return false;
    }

    /* Initialize PHY object for the corresponding TRX */
    pObj->bandOpMode = bandOpMode;
    pObj->channelNum = channelNum;
    pObj->phyConfig = phyConfig;
    pObj->trxState = RF215_RFn_STATE_RF_TRXOFF;
    pObj->rxInd.psdu = rf215PhyRxPsdu;

    /* Zero initialization */
    memset(&pObj->phyStatistics, 0, sizeof(pObj->phyStatistics));
    pObj->phyState = PHY_STATE_RESET;
    pObj->rxAbortState = PHY_STATE_RESET;
    pObj->txPendingState = PHY_STATE_RESET;
    pObj->rxPaySymbols = 0;
    pObj->txPaySymbols = 0;
    pObj->rxFlagsPending = 0;
    pObj->trxRdy = false;
    pObj->rxIndPending = false;
    pObj->txfePending = false;
    pObj->ledRxStatus = false;
    pObj->txStarted = false;
    pObj->txAutoInProgress = false;
    pObj->rxTimeValid = false;
    pObj->trxResetPending = false;
    pObj->trxSleepPending = false;
    pObj->txContinuousPending = false;
    pObj->idlePending = false;
    pObj->phyCfgPending = false;
    pObj->txCancelPending = false;
    pObj->txRequestPending = false;
    pObj->resetInProgress = false;

    if (_RF215_PHY_CheckPhyCfg(&phyConfig) == false)
    {
        /* Invalid PHY configuration */
        return false;
    }

    /* Compute channel frequency, range and mode of initial configuration */
    _RF215_PLL_Params(pllConst, pllParams, &phyConfig, channelNum);

    /* Check channel configuration */
    return _RF215_PLL_CheckConfig(pllConst, pllParams, &phyConfig, channelNum);
}

void RF215_PHY_Tasks(uint8_t trxIdx)
{
    bool reportInd = false;
    RF215_PHY_OBJ* pObj = &rf215PhyObj[trxIdx];

    /* Check if there is receive indication pending */
    if (pObj->rxIndPending == true)
    {
        /* Critical region to avoid changes from interrupts */
        RF215_HAL_EnterCritical();

        /* Check flag again (it could be modified from interrupts) */
        if (pObj->rxIndPending == true)
        {
            /* Copy RX indication and PSDU to static object and buffer */
            rf215PhyRxInd = pObj->rxInd;
            memcpy(rf215PhyRxPsdu, pObj->rxPsdu, pObj->rxInd.psduLen);
            pObj->rxIndPending = false;
            reportInd = true;
        }

        /* Leave critical region. RX indication ready to be notified */
        RF215_HAL_LeaveCritical();
    }

    if (reportInd == true)
    {
        DRV_RF215_NotifyRxInd(trxIdx, &rf215PhyRxInd);
    }
}

void RF215_PHY_ExtIntEvent(uint8_t trxIdx, uint8_t rfIRQS, uint8_t bbcIRQS)
{
    uint8_t rxfe, rxfs, agch, agcr, fbli, txfe, trxrdy;
    RF215_PHY_OBJ* pObj = &rf215PhyObj[trxIdx];

    /* Check TRX Wake-up interrupt */
    if ((rfIRQS & RF215_RFn_IRQ_WAKEUP) != 0)
    {
        _RF215_TRX_ResetEvent(trxIdx);
        return;
    }

    /* Check Transceiver Ready interrupt */
    trxrdy = rfIRQS & RF215_RFn_IRQ_TRXRDY;
    if (trxrdy != 0)
    {
        pObj->trxRdy = true;
    }

    /* Check Energy Detection Completion interrupt */
    txfe = bbcIRQS & RF215_BBCn_IRQ_TXFE;
    if ((rfIRQS & RF215_RFn_IRQ_EDC) != 0)
    {
        /* Single ED measurement finished: Auto mode automatically restored */
        pObj->phyRegs.RFn_EDC = RF215_RFn_EDC_EDM_AUTO;

        if (pObj->phyState == PHY_STATE_TX_CCA_ED)
        {
            /* Check Energy Detection result for CCA */
            _RF215_TX_EnDetectComplete(trxIdx);

            /* TXFE interrupt processed once EDC is completely handled */
            if (txfe != 0)
            {
                pObj->txfePending = true;
                txfe = 0;
            }
            else
            {
                pObj->txfePending = false;
            }
        }
    }

    /* Check Transmitter Frame End interrupt */
    if (txfe != 0)
    {
        _RF215_TX_FrameEnd(trxIdx);
    }

    /* Check receiver interrupts */
    rxfe = bbcIRQS & RF215_BBCn_IRQ_RXFE;
    rxfs = bbcIRQS & RF215_BBCn_IRQ_RXFS;
    agch = bbcIRQS & RF215_BBCn_IRQ_AGCH;
    agcr = bbcIRQS & RF215_BBCn_IRQ_AGCR;
    fbli = bbcIRQS & RF215_BBCn_IRQ_FBLI;

    if (pObj->rxAbortState != PHY_STATE_RESET)
    {
        /* RX aborted since last external interrupt */
        if ((pObj->rxAbortState == PHY_STATE_RX_PAYLOAD) && (rxfe != 0))
        {
            /* RXFE IRQ and payload was being received when RX was aborted */
            if ((rxfs == 0) && (agch == 0))
            {
                /* RX aborted just before, but it has actually finished.
                 * Interrupt is handled, but TRX state is not changed. */
                _RF215_RX_FrameEnd(trxIdx);
                rxfe = 0;
                agcr = 0;

                /* Undo PHY statistics update */
                pObj->phyStatistics.rxErrTotal--;
                pObj->phyStatistics.rxErrAborted--;
            }
            else if (agcr == 0)
            {
                /* Aborted RX has actually finished, but a new frame is
                 * being received. Ignore this RXFE interrupt. */
                rxfe = 0;
            }
        }
        else if ((pObj->rxAbortState == PHY_STATE_RX_HEADER) && (rxfs != 0))
        {
            /* RXFS IRQ and header was being received when RX was aborted */
            if ((agch == 0) && (rxfe == 0))
            {
                /* Ignore this RXFS interrupt (it comes from the aborted RX) */
                rxfs = 0;
            }
        }

        /* Clear RX abort state */
        pObj->rxAbortState = PHY_STATE_RESET;
    }

    if ((pObj->phyState < PHY_STATE_RX_LISTEN) || (pObj->phyState > PHY_STATE_RX_PAYLOAD))
    {
        /* If TRX was listening (no frame detected) when RX was aborted, we can
         * ignore these RX interrupts */
        rxfe = 0;
        rxfs = 0;
        agch = 0;
    }

    /* Check AGC Hold interrupt */
    if (agch != 0)
    {
        _RF215_RX_AgcHold(pObj);
    }

    /* Check Receiver Frame Start interrupt */
    if (rxfs != 0)
    {
        _RF215_RX_FrameStart(trxIdx);

        /* RXFE/AGCR interrupt processed once RXFS is completely handled */
        pObj->rxFlagsPending = rxfe | agcr;
        rxfe = 0;
        agcr = 0;
        fbli = 0;
    }

    /* Check Receiver Frame End interrupt */
    if (rxfe != 0)
    {
        _RF215_RX_FrameEnd(trxIdx);

        /* Turn off RX LED */
        RF215_HAL_LedRx(false);
        pObj->ledRxStatus = false;
    }

    /* Check AGC Release interrupt */
    if (agcr != 0)
    {
        _RF215_RX_AgcRelease(trxIdx);
        if (pObj->ledRxStatus == true)
        {
            /* Turn off RX LED */
            RF215_HAL_LedRx(false);
            pObj->ledRxStatus = false;
        }
    }

    /* Check FBLI interrupt */
    if (fbli != 0)
    {
        _RF215_RX_BuffLvlInt(trxIdx);
    }

    /* Check Transceiver Ready interrupt */
    if (trxrdy != 0)
    {
        DRV_RF215_TX_BUFFER_OBJ* txBufObj = pObj->txBufObj;
        RF215_PHY_STATE pendState;

        /* Clear pending TX state */
        pObj->txPendingState = PHY_STATE_RESET;

        /* Check pending TRX sleep or PHY configuration */
        if (pObj->trxSleepPending == true)
        {
            _RF215_TRX_Sleep(trxIdx);
            return;
        }
        else if (pObj->txContinuousPending == true)
        {
            _RF215_TRX_EnableTxContinuousMode(trxIdx);
        }
        else if (pObj->idlePending == true)
        {
            uint8_t trxIdxOther = RF215_TRX_RF24_IDX;
            if (trxIdx == RF215_TRX_RF24_IDX)
            {
                trxIdxOther = RF215_TRX_RF09_IDX;
            }

            /* Set continuous TX mode on the another TRX */
            _RF215_TRX_EnableTxContinuousMode(trxIdxOther);
        }
        else if (pObj->phyCfgPending == true)
        {
            _RF215_PHY_SetPhyConfig(trxIdx, &pObj->phyConfigPending, pObj->channelNumPhyCfgPending);
        }

        /* Check if there is pending TX configuration */
        pendState = pObj->txPendingState;
        if ((pObj->txStarted == true) && (pObj->phyState < PHY_STATE_TX_CONFIG) &&
                (pendState >= PHY_STATE_TX_CONFIG))
        {
            /* Carrier sense CCA and TX parameters configuration */
            DRV_RF215_TX_RESULT txResult = _RF215_TX_ParamCfg(txBufObj);
            if (txResult != RF215_TX_SUCCESS)
            {
                /* Set pending TX confirm with TX error */
                RF215_PHY_SetTxCfm(txBufObj, txResult);
            }
        }

        if (pObj->txStarted == true)
        {
            /* Check if there is pending TX preparation */
            if ((pObj->phyState < PHY_STATE_TX_TXPREP) && (pendState >= PHY_STATE_TX_TXPREP))
            {
                _RF215_TX_Prepare(trxIdx);
            }

            /* Check if there is pending TX start */
            if ((pObj->phyState == PHY_STATE_TX_TXPREP) && (pendState > PHY_STATE_TX_TXPREP))
            {
                _RF215_TX_Start(trxIdx);
            }
        }

        if ((pObj->phyState == PHY_STATE_RX_LISTEN) &&
                (pObj->trxState != RF215_RFn_STATE_RF_RX))
        {
            /* Send pending RX command */
            _RF215_TRX_RxListen(trxIdx);
        }
    }
}

DRV_RF215_TX_RESULT RF215_PHY_TxRequest(DRV_RF215_TX_BUFFER_OBJ* txBufObj)
{
    uint8_t trxIdx = txBufObj->clientObj->trxIndex;
    uint16_t psduLen = txBufObj->reqObj.psduLen;
    DRV_RF215_PHY_MOD_SCHEME modScheme = txBufObj->reqObj.modScheme;
    DRV_RF215_PHY_CCA_MODE ccaMode = txBufObj->reqObj.ccaMode;
    DRV_RF215_TX_TIME_MODE timeMode = txBufObj->reqObj.timeMode;
    RF215_PHY_OBJ* pObj = &rf215PhyObj[trxIdx];
    DRV_RF215_TX_RESULT result = RF215_TX_SUCCESS;

    if ((psduLen > DRV_RF215_MAX_PSDU_LEN) || (psduLen <= DRV_RF215_FCS_LEN))
    {
        /* Error: invalid data length */
        result = RF215_TX_INVALID_LEN;
    }
    else if ((ccaMode > PHY_CCA_OFF) || (ccaMode < PHY_CCA_MODE_1))
    {
        /* Error: invalid CCA mode */
        result = RF215_TX_INVALID_PARAM;
    }
    else if ((timeMode > TX_TIME_RELATIVE) || (timeMode < TX_TIME_ABSOLUTE))
    {
        /* Error: invalid TX time mode */
        result = RF215_TX_INVALID_PARAM;
    }
    else if (pObj->phyConfig.phyType == PHY_TYPE_FSK)
    {
        if ((modScheme > FSK_FEC_ON) || (modScheme < FSK_FEC_OFF))
        {
            /* Error: invalid FSK FEC mode */
            result = RF215_TX_INVALID_PARAM;
        }
    }
    else
    {
        DRV_RF215_OFDM_BW_OPT opt = pObj->phyConfig.phyTypeCfg.ofdm.opt;
        if ((modScheme > OFDM_MCS_6) || (modScheme < ofdmBwOptConst[opt].minMCS))
        {
            /* Error: invalid OFDM MCS */
            result = RF215_TX_INVALID_PARAM;
        }
    }

    if (result == RF215_TX_SUCCESS)
    {
        uint64_t interruptTime;
        uint32_t txTotalDelay;
        SYS_TIME_HANDLE timeHandle;
        bool intStatus;
        uint64_t txTime = txBufObj->reqObj.timeCount;

        if (pObj->resetInProgress == true)
        {
            /* TRX reset is in progress: Wait to finish before TX request */
            pObj->txRequestPending = true;
            pObj->txBufObjPending = txBufObj;
            return result;
        }

        /* Total TX delay (worst case), in SYS_TIME count units */
        txTotalDelay = _RF215_TX_TotalDelay(txBufObj);

        if (timeMode == TX_TIME_RELATIVE)
        {
            if (txTime < txTotalDelay)
            {
                /* Delay too short: Adjust to total TX delay */
                txTime = txTotalDelay;
            }

            /* Relative time mode */
            txTime += SYS_TIME_Counter64Get();
            txBufObj->reqObj.timeCount = txTime;
        }

        /* Update TX initial time in confirm object */
        txBufObj->cfmObj.timeIniCount = txTime;

        /* Time when we need the scheduled interrupt */
        interruptTime = txTime - txTotalDelay;

        /* Critical region to avoid delays in current time computations */
        intStatus = SYS_INT_Disable();

        /* Schedule timer for the specified time */
        timeHandle = _RF215_TX_TimeSchedule(interruptTime, true,
                _RF215_TX_PrepareTimeExpired, (uintptr_t) txBufObj);

        if (timeHandle == SYS_TIME_HANDLE_INVALID)
        {
            /* Timer could not be created: Timeout error */
            result = RF215_TX_TIMEOUT;
        }
        else
        {
            /* Timer created and started */
            txBufObj->timeHandle = timeHandle;
        }

        /* Disable again time interrupt (enabled by SYS_TIME) */
        RF215_HAL_DisableTimeInt();

        /* Leave critical region */
        SYS_INT_Restore(intStatus);
    }

    if (result != RF215_TX_SUCCESS)
    {
        /* Critical region to avoid conflicts in PHY object data.
         * Update PHY statistics with TX error. */
        _RF215_TX_UpdStats(pObj, result);
    }

    return result;
}

void RF215_PHY_TxCancel(DRV_RF215_TX_BUFFER_OBJ* txBufObj)
{
    bool txCancel = true;
    uint8_t trxIdx = txBufObj->clientObj->trxIndex;
    RF215_PHY_OBJ* pObj = &rf215PhyObj[trxIdx];

    if ((pObj->phyState >= PHY_STATE_TX_CONFIG) && (pObj->txBufObj == txBufObj))
    {
        /* TX already started or in preparation */
        if (pObj->txAutoInProgress == true)
        {
            /* Wait automatic procedure to finish */
            pObj->txCancelPending = true;
            txCancel = false;
        }
        else
        {
            /* Abort TX in progress or in preparation. Start listening. */
            _RF215_TRX_RxListen(trxIdx);
        }
    }

    if (txCancel == true)
    {
        RF215_PHY_SetTxCfm(txBufObj, RF215_TX_CANCELLED);
        // SYS_TIME_TimerDestroy(txBufObj->timeHandle);

        /* Free TX buffer. TX confirm will not be notified */
        txBufObj->inUse = false;
    }
}

void RF215_PHY_SetTxCfm (
    DRV_RF215_TX_BUFFER_OBJ* txBufObj,
    DRV_RF215_TX_RESULT result
)
{
    RF215_PHY_OBJ* pObj = &rf215PhyObj[txBufObj->clientObj->trxIndex];

    /* Set 0 duration if not successful transmission */
    if ((result != RF215_TX_SUCCESS) && (result != RF215_TX_ERROR_UNDERRUN))
    {
        txBufObj->cfmObj.ppduDurationCount = 0;
    }

    /* Set pending TX confirm and update statistics */
    txBufObj->cfmObj.txResult = result;
    txBufObj->cfmPending = true;
    _RF215_TX_UpdStats(pObj, result);

    /* Clear TX flag if buffer corresponds to ongoing transmission */
    if (pObj->txBufObj == txBufObj)
    {
        pObj->txStarted = false;
        pObj->txPendingState = PHY_STATE_RESET;
    }
}

bool RF215_PHY_CheckTxContentionWindow(DRV_RF215_TX_BUFFER_OBJ* txBufObj)
{
    uint32_t cwDurationUS, cwDuration;
    RF215_PHY_OBJ* pObj = &rf215PhyObj[txBufObj->clientObj->trxIndex];

    if ((txBufObj->reqObj.ccaMode == PHY_CCA_OFF) || (pObj->rxTimeValid == false))
    {
        /* No RX indication received or CCA disabled */
        return false;
    }

    /* Compute contention window duration and convert to SYS_TIME count.
     * Convert RX PPDU duration to SYS_TIME count.
     * Compare end of received frame with start of contention window. */
    cwDurationUS = _RF215_TX_ContentionWindowUS(txBufObj);
    cwDuration = SYS_TIME_USToCount(cwDurationUS);
    if ((pObj->rxInd.timeIniCount + pObj->rxInd.ppduDurationCount) >= (txBufObj->reqObj.timeCount - cwDuration))
    {
        /* Busy channel detected during CCA (carrier sense) contention window */
        return true;
    }

    /* Clear channel detected during CCA (carrier sense) contention window */
    return false;
}

DRV_RF215_PIB_RESULT RF215_PHY_GetPib (
    uint8_t trxIndex,
    DRV_RF215_PIB_ATTRIBUTE attr,
    void* value
)
{
    RF215_PHY_OBJ* pObj = &rf215PhyObj[trxIndex];

    switch (attr)
    {
        case RF215_PIB_TRX_SLEEP:
            *((bool *) value) = (pObj->phyState == PHY_STATE_SLEPT);
            break;

        case RF215_PIB_PHY_CONFIG:
            memcpy((uint8_t *)value, (uint8_t *)&pObj->phyConfig, sizeof(DRV_RF215_PHY_CFG_OBJ));
            break;

        case RF215_PIB_PHY_BAND_OPERATING_MODE:
            *((DRV_RF215_PHY_BAND_OPM *) value) = pObj->bandOpMode;
            break;

        case RF215_PIB_PHY_CHANNEL_NUM:
            *((uint16_t *) value) = pObj->channelNum;
            break;

        case RF215_PIB_PHY_CHANNEL_FREQ_HZ:
            *((uint32_t *) value) = pObj->pllParams.chnFreq;
            break;

        case RF215_PIB_PHY_CCA_ED_DURATION:
            *((uint16_t *) value) = pObj->phyConfig.ccaEdDurationUS;
            break;

        case RF215_PIB_PHY_CCA_ED_THRESHOLD:
            *((int8_t *) value) = pObj->phyConfig.ccaEdThresholdDBm;
            break;

        case RF215_PIB_PHY_TURNAROUND_TIME:
            *((uint16_t *) value) = pObj->turnaroundTimeUS;
            break;

        case RF215_PIB_PHY_TX_PAY_SYMBOLS:
            *((uint16_t *) value) = pObj->txPaySymbols;
            break;

        case RF215_PIB_PHY_RX_PAY_SYMBOLS:
            *((uint16_t *) value) = pObj->rxPaySymbols;
            break;

        case RF215_PIB_PHY_TX_TOTAL:
            *((uint32_t *) value) = pObj->phyStatistics.txTotal;
            break;

        case RF215_PIB_PHY_TX_TOTAL_BYTES:
            *((uint32_t *) value) = pObj->phyStatistics.txTotalBytes;
            break;

        case RF215_PIB_PHY_TX_ERR_TOTAL:
            *((uint32_t *) value) = pObj->phyStatistics.txErrTotal;
            break;

        case RF215_PIB_PHY_TX_ERR_BUSY_TX:
            *((uint32_t *) value) = pObj->phyStatistics.txErrBusyTx;
            break;

        case RF215_PIB_PHY_TX_ERR_BUSY_RX:
            *((uint32_t *) value) = pObj->phyStatistics.txErrBusyRx;
            break;

        case RF215_PIB_PHY_TX_ERR_BUSY_CHN:
            *((uint32_t *) value) = pObj->phyStatistics.txErrBusyChn;
            break;

        case RF215_PIB_PHY_TX_ERR_BAD_LEN:
            *((uint32_t *) value) = pObj->phyStatistics.txErrBadLen;
            break;

        case RF215_PIB_PHY_TX_ERR_BAD_FORMAT:
            *((uint32_t *) value) = pObj->phyStatistics.txErrBadFormat;
            break;

        case RF215_PIB_PHY_TX_ERR_TIMEOUT:
            *((uint32_t *) value) = pObj->phyStatistics.txErrTimeout;
            break;

        case RF215_PIB_PHY_TX_ERR_ABORTED:
            *((uint32_t *) value) = pObj->phyStatistics.txErrAborted;
            break;

        case RF215_PIB_PHY_TX_CFM_NOT_HANDLED:
            *((uint32_t *) value) = pObj->phyStatistics.txCfmNotHandled;
            break;

        case RF215_PIB_PHY_RX_TOTAL:
            *((uint32_t *) value) = pObj->phyStatistics.rxTotal;
            break;

        case RF215_PIB_PHY_RX_TOTAL_BYTES:
            *((uint32_t *) value) = pObj->phyStatistics.rxTotalBytes;
            break;

        case RF215_PIB_PHY_RX_ERR_TOTAL:
            *((uint32_t *) value) = pObj->phyStatistics.rxErrTotal;
            break;

        case RF215_PIB_PHY_RX_ERR_FALSE_POSITIVE:
            *((uint32_t *) value) = pObj->phyStatistics.rxErrFalsePositive;
            break;

        case RF215_PIB_PHY_RX_ERR_BAD_LEN:
            *((uint32_t *) value) = pObj->phyStatistics.rxErrBadLen;
            break;

        case RF215_PIB_PHY_RX_ERR_BAD_FORMAT:
            *((uint32_t *) value) = pObj->phyStatistics.rxErrBadFormat;
            break;

        case RF215_PIB_PHY_RX_ERR_BAD_FCS_PAY:
            *((uint32_t *) value) = pObj->phyStatistics.rxErrBadFcsPay;
            break;

        case RF215_PIB_PHY_RX_ERR_ABORTED:
            *((uint32_t *) value) = pObj->phyStatistics.rxErrAborted;
            break;

        case RF215_PIB_PHY_RX_OVERRIDE:
            *((uint32_t *) value) = pObj->phyStatistics.rxOverride;
            break;

        case RF215_PIB_PHY_RX_IND_NOT_HANDLED:
            *((uint32_t *) value) = pObj->phyStatistics.rxIndNotHandled;
            break;

        case RF215_PIB_PHY_TX_CONTINUOUS:
            *((bool *) value) = (pObj->phyState == PHY_STATE_TX_CONTINUOUS);
            break;

        case RF215_PIB_MAC_UNIT_BACKOFF_PERIOD:
            *((uint16_t *) value) = pObj->turnaroundTimeUS + pObj->phyConfig.ccaEdDurationUS;
            break;

        default:
            return RF215_PIB_RESULT_INVALID_ATTR;
    }

    return RF215_PIB_RESULT_SUCCESS;
}

DRV_RF215_PIB_RESULT RF215_PHY_SetPib (
    uint8_t trxIndex,
    DRV_RF215_PIB_ATTRIBUTE attr,
    void* value
)
{
    DRV_RF215_PIB_RESULT result = RF215_PIB_RESULT_SUCCESS;
    RF215_PHY_OBJ* pObj = &rf215PhyObj[trxIndex];

    /* Critical region to avoid conflicts in PHY object data */
    RF215_HAL_EnterCritical();

    switch (attr)
    {
        case RF215_PIB_TRX_RESET:
            /* Send RESET command */
            _RF215_TRX_Reset(trxIndex);
            break;

        case RF215_PIB_TRX_SLEEP:
            if (*((bool *) value) == true)
            {
                if (pObj->phyState != PHY_STATE_SLEPT)
                {
                    /* Sleep */
                    _RF215_TRX_Sleep(trxIndex);
                }
            }
            else
            {
                if (pObj->trxSleepPending == true)
                {
                    pObj->trxSleepPending = false;
                }
                else if (pObj->phyState == PHY_STATE_SLEPT)
                {
                    /* Wake-up */
                    _RF215_TRX_SwitchTrxOff(trxIndex);
                    if ((rf215PhyRegRF_IQIFC1 & RF215_RF_IQIFC1_CHPM_Msk) == RF215_RF_IQIFC1_CHPM_BBRF)
                    {
                        /* Normal case */
                        pObj->phyState = PHY_STATE_RESET;
                    }
                    else
                    {
                        /* The another TRX is in TX continuous mode */
                        pObj->phyState = PHY_STATE_IDLE;
                    }
                }
            }
            break;

        case RF215_PIB_PHY_CONFIG:
            result = _RF215_PHY_SetPhyConfig(trxIndex, value, 0);
            if (result == RF215_PIB_RESULT_SUCCESS)
            {
                pObj->bandOpMode = DRV_RF215_BAND_OPM_CUSTOM;
            }
            break;

        case RF215_PIB_PHY_BAND_OPERATING_MODE:
        {
            DRV_RF215_PHY_CFG_OBJ phyCfgNew;
            DRV_RF215_PHY_BAND_OPM phyBandOpmNew = *((DRV_RF215_PHY_BAND_OPM *) value);

            /* Convert frequency band and operating mode to PHY configuration
             * object */
            if (_RF215_PHY_BandOpModeToPhyCfg(phyBandOpmNew, &phyCfgNew) == true)
            {
                result = _RF215_PHY_SetPhyConfig(trxIndex, &phyCfgNew, 0);
                if (result == RF215_PIB_RESULT_SUCCESS)
                {
                    pObj->bandOpMode = phyBandOpmNew;
                }
            }
            else
            {
                result = RF215_PIB_RESULT_INVALID_PARAM;
            }
            break;
        }

        case RF215_PIB_PHY_CHANNEL_NUM:
        {
            DRV_RF215_PHY_BAND_OPM chnNumNew = *((uint16_t *) value);
            result = _RF215_PHY_SetPhyConfig(trxIndex, &pObj->phyConfig, chnNumNew);
            break;
        }

        case RF215_PIB_PHY_CCA_ED_DURATION:
            pObj->phyConfig.ccaEdDurationUS = *((uint16_t *) value);
            _RF215_RXFE_AdjustEDD(trxIndex);
            break;

        case RF215_PIB_PHY_CCA_ED_THRESHOLD:
            pObj->phyConfig.ccaEdThresholdDBm = *((int8_t *) value);
            break;

        case RF215_PIB_PHY_STATS_RESET:
            memset(&pObj->phyStatistics, 0, sizeof(RF215_PHY_STATISTICS_OBJ));
            break;

        case RF215_PIB_PHY_TX_CONTINUOUS:
            if (*((bool *) value) == true)
            {
                if (pObj->phyState != PHY_STATE_TX_CONTINUOUS)
                {
                    /* Set continuous TX mode */
                    _RF215_TRX_EnableTxContinuousMode(trxIndex);
                }
            }
            else
            {
                if (pObj->txContinuousPending == true)
                {
                    pObj->txContinuousPending = false;
                }
                else if (pObj->phyState == PHY_STATE_TX_CONTINUOUS)
                {
                    /* Stop continuous TX mode */
                    _RF215_TRX_DisableTxContinuousMode(trxIndex);
                }
            }
            break;

        default:
            result = RF215_PIB_RESULT_INVALID_ATTR;
    }

    RF215_HAL_LeaveCritical();

    return result;
}

void RF215_PHY_Reset(uint8_t trxIndex)
{
    RF215_PHY_OBJ* pObj = &rf215PhyObj[trxIndex];
    
    /* Critical region to avoid conflicts in PHY object data */
    RF215_HAL_EnterCritical();

    if (pObj->phyState == PHY_STATE_TX_CONTINUOUS)
    {
        /* Disable first TX continuous mode */
        _RF215_TRX_DisableTxContinuousMode(trxIndex);
    }

    pObj->trxResetPending = false;
    pObj->resetInProgress = true;
    
    /* Leave critical region */
    RF215_HAL_LeaveCritical();
}

void RF215_PHY_DeviceReset()
{
    rf215PhyRegRF_IQIFC1 = RF215_RF_IQIFC1_Rst;
}

