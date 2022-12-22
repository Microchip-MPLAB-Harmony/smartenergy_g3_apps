/*******************************************************************************
  RF215 Driver PHY Header File

  Company:
    Microchip Technology Inc.

  File Name:
    rf215_phy.h

  Summary:
    RF215 Driver Modules Header File

  Description:
    The RF215 driver PHY manages the different modules of the RF215 Physical
    Layer (PLL, Frontend, Baseband Core) of the RF215. This file provides the
    interface definition for the RF215 driver PHY.
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

#ifndef _RF215_PHY_H
#define _RF215_PHY_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************
#include "driver/rf215/drv_rf215_definitions.h"
#include "driver/rf215/drv_rf215_local.h"
#include "driver/rf215/hal/rf215_hal.h"
#include "configuration.h"

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
/* RF215 Driver Time Execution Delays

  Summary:
    Time execution delays that affect the TX and RX time accuracy. These delays
    depend on the core, the CPU clock frequency, cache / TCM configuration and
    compiler options used.

  Remarks:
    To achieve best accuracy it should be calibrated for every core, cache / TCM
    configuration  and compiler option. Anyway, the error shouldn't be more than
    +/- ~10us.
    The unit is us with 5 comma bits (Q5, or cycles of 32MHz).
*/

/* Convert execution cycles to us with 5 comma bits (Q5, or cycles of 32MHz) */
#define EX_CYCL_TO_USQ5(x)       ((uint32_t) \
    DIV_ROUND((uint64_t) (x * 32000000ULL), SYS_TIME_CPU_CLOCK_FREQUENCY))

/* Delay between RF215 and SYS_TIME counter reads for synchronization */
#define RF215_SYNC_DELAY_US_Q5   EX_CYCL_TO_USQ5(DRV_RF215_TIME_SYNC_EXECUTION_CYCLES)

/* Delay between call to _RF215_TX_Start and start of SPI transfer */
#define RF215_TX_CMD_DELAY_US_Q5 EX_CYCL_TO_USQ5(DRV_RF215_TX_COMMAND_EXECUTION_CYCLES)

// *****************************************************************************
/* RF215 TRX Transition Times

  Summary:
    TRX transition times [Table 10-7].

  Remarks:
    The unit is us with 5 comma bits (Q5, or cycles of 32MHz).
*/

/* From TRXOFF to TXPREP in us. Max. 200us.
 * From RX to TXPREP in us [uQ0.5]. Max. 200ns.
 * From TXPREP to RX in us [uQ0.5]. Max. 200ns.
 * From TXPREP to TX in us [uQ0.5]. Max. 200ns.
 * From RX to TX (CCATX) in us [uQ0.5]. Max. 400ns.
 * Energy detection (EDM_SINGLE) delay. 4.125us [uQ3.5] (not in data-sheet).
 * tx_start_delay. Typ. 4 us. */
#define RF215_TRXOFF_TXPREP_TIME_US_Q5  (200 << 5)
#define RF215_RX_TXPREP_TIME_US_Q5      6
#define RF215_TXPREP_RX_TIME_US_Q5      6
#define RF215_TXPREP_TX_TIME_US_Q5      6
#define RF215_RX_TX_TIME_US_Q5          13
#define RF215_RX_CCA_ED_TIME_US_Q5      132
#define RF215_TX_START_DELAY_US_Q5      (4 << 5)

// *****************************************************************************
/* RF215 Driver Scheduled TX Time Delays

  Summary:
    Time execution delays and margins related to scheduled TX. Not critical, but
    they can affect the TX time accuracy due to higher/same priority interrupts
    or critical regions. These delays depend on the core, the CPU clock
    frequency, cache / TCM configuration and compiler options used.

  Remarks:
    The unit is us with 5 comma bits (Q5, or cycles of 32MHz).
*/

/* Margin for higher/same priority interrupts or critical regions:
 * 50 us + 10000 execution cycles. */
#define RF215_TX_IRQ_MARGIN_US_Q5       ((50 << 5) + EX_CYCL_TO_USQ5(10000))

/* Delay of TRXRDY interrupt handling. 7500 execution cycles + 6 SPI bytes */
#define RF215_TX_TRXRDY_DELAY_US_Q5     (EX_CYCL_TO_USQ5(7500) + \
    (RF215_SPI_BYTE_DURATION_US_Q5 * 6))

/* Delay of TX parameter configuration (_RF215_TX_ParamCfg).
 * 2000 cycles + 9 SPI bytes + TRXOFF->TXPREP transition + TRXRDY delay +
 * + IRQ margin */
#define RF215_TX_PARAM_CFG_DELAY_US_Q5  (EX_CYCL_TO_USQ5(2000) + \
    (RF215_SPI_BYTE_DURATION_US_Q5 * 9) + RF215_TRXOFF_TXPREP_TIME_US_Q5 + \
    RF215_TX_TRXRDY_DELAY_US_Q5 + RF215_TX_IRQ_MARGIN_US_Q5)

/* Delay of time interrupt handling: IRQ margin + 5000 execution cycles */
#define RF215_TX_TIME_IRQ_DELAY_US_Q5   (RF215_TX_IRQ_MARGIN_US_Q5 + \
    EX_CYCL_TO_USQ5(5000))

// *****************************************************************************
/* RF215 PLL Frequency Ranges

  Summary:
    Definition of RF215 PLL (Frequency Synthesizer) frequency ranges in Hz
    [Table 6-21].
*/

/* First range: 389.5MHz - 510MHz (RF09) */
#define PLL_FREQ_MIN_RF09_RNG1_Hz           389500000
#define PLL_FREQ_MAX_RF09_RNG1_Hz           510000000
/* Second range: 779MHz - 1020MHz (RF09) */
#define PLL_FREQ_MIN_RF09_RNG2_Hz           779000000
#define PLL_FREQ_MAX_RF09_RNG2_Hz          1020000000
/* Third range: 2400MHz - 2483.5MHz (RF24) */
#define PLL_FREQ_MIN_RF24_RNG3_Hz          2400000000
#define PLL_FREQ_MAX_RF24_RNG3_Hz          2483500000

// *****************************************************************************
/* RF215 PLL IEEE-compliant Scheme

  Summary:
    Definitions for RF215 PLL (Frequency Synthesizer) IEEE-compliant Scheme
    (CNM.CM=0).
*/

/* Frequency resolution: 25kHz */
#define PLL_IEEE_FREQ_STEP_Hz              25000
/* Maximum channel number (9 bits) */
#define PLL_IEEE_CHN_NUM_MAX               511
/* Maximum channel spacing: 255 (8 bits) * 25kHz = 6.375MHz*/
#define PLL_IEEE_CHN_SPA_MAX_Hz            (PLL_IEEE_FREQ_STEP_Hz * UINT8_MAX)
/* Frequency offset (RF09): 0Hz */
#define PLL_IEEE_FREQ_OFFSET09_Hz          0
/* Frequency offset (RF24): 1.5GHz */
#define PLL_IEEE_FREQ_OFFSET24_Hz          1500000000

// *****************************************************************************
/* RF215 PLL Fine Resolution Channel Scheme

  Summary:
    Definitions for RF215 PLL (Frequency Synthesizer) Fine Resolution Channel
    Scheme (CNM.CM!=0).
*/

/* Frequency offset (RF09, first range, CNM.CM=1): 377MHz */
#define PLL_FINE_FREQ_OFFSET_RF09_RNG1_Hz   377000000
/* Frequency offset (RF09, second range, CNM.CM=2): 754MHz */
#define PLL_FINE_FREQ_OFFSET_RF09_RNG2_Hz   754000000
/* Frequency offset (RF24, third range, CNM.CM=3): 2366MHz */
#define PLL_FINE_FREQ_OFFSET_RF24_RNG3_Hz  2366000000
/* Frequency resolution (RF09, first range, CNM.CM=1): 6.5MHz / 2^16 */
#define PLL_FINE_FREQ_RES_RF09_RNG1_Hz        6500000
/* Frequency resolution (RF09, second range, CNM.CM=2): 13MHz / 2^16 */
#define PLL_FINE_FREQ_RES_RF09_RNG2_Hz       13000000
/* Frequency resolution (RF24, third range, CNM.CM=3): 26MHz / 2^16 */
#define PLL_FINE_FREQ_RES_RF24_RNG3_Hz       26000000

// *****************************************************************************
/* RF215 PLL Frequency Tolerance

  Summary:
    Definitions for maximum frequency tolerance.
*/

/* Convert PPM (10^-6) to uQ0.45 */
#define PLL_PPM_TO_Q45(x)                  DIV_ROUND(x << 45, 1000000)

/* FSK PHY: T<=min(50*10^-6, T0*R*h*F0/R0/h0/F) [IEEE 802.15.4 19.6.3] */
#define PLL_DELTA_FSK_TMAX_Q45             PLL_PPM_TO_Q45(50ULL)
#define PLL_DELTA_FSK_T0_RF09_Q45          PLL_PPM_TO_Q45(30ULL)
#define PLL_DELTA_FSK_T0_RF24_Q45          PLL_PPM_TO_Q45(40ULL)

/* OFDM PHY: T<=20*10^-6 [IEEE 802.15.4 20.5.9] */
#define PLL_DELTA_OFDM_TMAX_Q45            PLL_PPM_TO_Q45(20ULL)

// *****************************************************************************
/* RF215 BBC Register Values

  Summary:
    Values of BBC configuration registers.
*/

/* BBCn_PC register value (common for baseband enabled/disabled)
 * PT: PHY Type (pt enum DRV_RF215_PHY_TYPE)
 * PC.TXAFCS: Enable Transmitter Auto Frame Check Sequence
 * PC.FCSFE: Enable Frame Check Sequence Filter
 * PC.FCST: 32-bit FCS */
#define BBC_PC_COMMON(pt)        (RF215_BBCn_PC_PT(pt + 1) | \
    RF215_BBCn_PC_FCST_32 | RF215_BBCn_PC_TXAFCS_EN | RF215_BBCn_PC_FCSFE_EN)

/* BBCn_PC register value (Baseband enabled)
 * BBEN: Baseband Core Enable */
#define BBC_PC_CFG_BBEN(pt)      (RF215_BBCn_PC_BBEN_ON | BBC_PC_COMMON(pt))

/* BBCn_PC register value (Baseband disabled)
 * BBEN: Baseband Core Disabled */
#define BBC_PC_CFG_BBDIS(pt)     (RF215_BBCn_PC_BBEN_OFF | BBC_PC_COMMON(pt))

/* BBCn_FSKPHRTX register value (FEC disabled)
 * RB0/1: Reserved bits set to 0
 * DW: Data Whitening enabled
 * SFD: SFD0 used (uncoded) */
#define BBC_FSKPHRTX_FEC_OFF      (RF215_BBCn_FSKPHRTX_SFD_0 | \
    RF215_BBCn_FSKPHRTX_DW_EN)

/* BBCn_FSKPHRTX register value (FEC enabled)
 * RB0/1: Reserved bits set to 0
 * DW: Data Whitening enabled
 * SFD: SFD1 used (coded) */
#define BBC_FSKPHRTX_FEC_ON       (RF215_BBCn_FSKPHRTX_SFD_1 | \
    RF215_BBCn_FSKPHRTX_DW_EN)

/* BBCn_FSKPHRRX mask to check received PHR */
#define BBC_FSKPHRRX_MASK         (RF215_BBCn_FSKPHRRX_DW_Msk |	\
    RF215_BBCn_FSKPHRRX_SFD_Msk | RF215_BBCn_FSKPHRRX_MS | \
    RF215_BBCn_FSKPHRRX_FCST_Msk)

/* BBCn_FSKPHRRX register value (common for FEC enabled/disabled)
 * DW: Data Whitening enabled
 * MS: Mode switch not used (0)
 * FCST: FCS type 32-bit */
#define BBC_FSKPHRRX_COMMON      (RF215_BBCn_FSKPHRRX_DW_EN | \
    RF215_BBCn_FSKPHRRX_FCST_32)

/* BBCn_FSKPHRRX register value (FEC disabled)
 * SFD: SFD0 used (uncoded) */
#define BBC_FSKPHRRX_FEC_OFF      (RF215_BBCn_FSKPHRRX_SFD_0 | \
    BBC_FSKPHRRX_COMMON)

/* BBCn_FSKPHRRX register value (FEC enabled)
 * SFD: SFD1 used (coded) */
#define BBC_FSKPHRRX_FEC_ON       (RF215_BBCn_FSKPHRRX_SFD_1 | \
    BBC_FSKPHRRX_COMMON)

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* RF215 PHY State

  Summary:
    List of RF215 PHY states.
*/

typedef enum
{
    PHY_STATE_RESET      = 0,
    PHY_STATE_SLEPT      = 1,
    PHY_STATE_RX_LISTEN  = 2,
    PHY_STATE_RX_HEADER  = 3,
    PHY_STATE_RX_PAYLOAD = 4,
    PHY_STATE_TX_CONFIG  = 5,
    PHY_STATE_TX_TXPREP  = 6,
    PHY_STATE_TX_CCA_ED  = 7,
    PHY_STATE_TX         = 8,

} RF215_PHY_STATE;

// *****************************************************************************
/* RF215 FSK Symbol Rate Constant Data

  Summary:
    Object used to keep constant data for each RF215 FSK symbol rate.
*/

typedef struct
{
    /* FSK symbol rate in Hz */
    uint32_t                        Hz;

    /* FSK symbol rate in kHz */
    uint16_t                        kHz;

    /* FSK pre-emphasis processing delay in us [uQ5.5] (not in data-sheet).
     * If pre-emphasis is enabled (FSKDM.PE=1), tx_bb_delay is reduced because
     * FSKC0.BT has no effect (GFSK modulator disabled). Delay1 and Delay2
     * reduce tx_bb_delay. Delay2 has to be compensated in TX confirm time. */
    uint16_t                        txPreEmphasisDelay1USq5;
    uint16_t                        txPreEmphasisDelay2USq5;

    /* Transmitter baseband delay in microseconds [uQ6.5] [Table 6-2] */
    uint16_t                        txBaseBandDelayUSq5;

    /* Receiver baseband delay in microseconds [uQ3.5] (not in data-sheet) */
    uint8_t                         rxBaseBandDelayUSq5;

    /* TX/RX DFE sampling rate: TXDFE.SR, RXDFE.SR [Table 6-51] */
    uint8_t                         RFn_RXDFE_SR;
    uint8_t                         RFn_TXDFE_SR;

    /* Power Amplifier Ramp Time [Table 6-53] */
    uint8_t                         RFn_TXCUT_PARAMP;

    /* FSK pre-emphasis registers: BBCn_FSKPE0/1/2 [Table 6-57] */
    uint8_t                         BBCn_FSKPE0;
    uint8_t                         BBCn_FSKPE1;
    uint8_t                         BBCn_FSKPE2;

} RF215_FSK_SYM_RATE_CONST_OBJ;

// *****************************************************************************
/* RF215 OFDM Bandwidth Option Constant Data

  Summary:
    Object used to keep constant data for each RF215 OFDM bandwidth option.
*/

typedef struct
{
    /* OFDM bandwidth in Hz */
    uint32_t                        bwHz;

    /* Receiver baseband delay in microseconds [uQ7.5] (not in data-sheet),
     * depending on interleaving mode */
    uint16_t                        rxBaseBandDelayUSq5[2];

    /* Transmitter baseband delay in microseconds [uQ2.5]: < 3.5 us */
    uint8_t                         txBaseBandDelayUSq5;

    /* Number of OFDM PHR symbols, depending on interleaving mode */
    uint8_t                         phrSymbols[2];

    /* Number of OFDM data carriers */
    uint8_t                         dataCarriers;

    /* TX/RX DFE sampling rate: TXDFE.SR, RXDFE.SR [Table 6-51] */
    uint8_t                         RFn_RXDFE_SR;
    uint8_t                         RFn_TXDFE_SR;

    /* OFDM preamble detection threshold: OFDMSW.PDT [Table 6-93] */
    uint8_t                         BBCn_OFDMSW_PDT;

    /* Minimum MCS (lowest baud-rate, most robust) for the bandwidth option */
    DRV_RF215_PHY_MOD_SCHEME        minMCS;

} RF215_OFDM_BW_OPT_CONST_OBJ;

// *****************************************************************************
/* RF215 OFDM MCS Constant Data

  Summary:
    Object used to keep constant data for each RF215 OFDM MCS.
*/

typedef struct
{
    /* Frequency spreading repetition factor shift (log2(factor)) */
    uint8_t                         repFactorShift;

    /* Bits per sub-carrier shift (log2(bitsPerCarrier)) */
    uint8_t                         bitsCarrierShift;

    /* Minimum TX power attenuation to satisfy EVM requirements [Table 6-91] */
    uint8_t                         minTxPwrAttMin;

} RF215_OFDM_MCS_CONST_OBJ;

// *****************************************************************************
/* RF215 PLL Frequency Range Object

  Summary:
    Defines a RF215 frequency range (minimum and maximum frequency).
*/

typedef struct
{
    /* Minimum frequency in Hz */
    uint32_t                        freqMin;

    /* Maximum frequency in Hz */
    uint32_t                        freqMax;

} RF215_PLL_FREQ_RNG_OBJ;

// *****************************************************************************
/* RF215 PLL Constant Data

  Summary:
    Object used to keep constant data of the RF215 PLL (Frequency Synthesizer).

  Remarks:
    If both Sub-GHz and 2.4 GHz transceivers are enabled, there is one object
    for each transceiver.
*/

typedef struct
{
    /* Frequency ranges */
    RF215_PLL_FREQ_RNG_OBJ          freqRanges[2];

    /* Frequency resolution for Fine Resolution Channel Scheme (CNM.CM!=0) */
    uint32_t                        fineFreqRes[2];

    /* Frequency offset for Fine Resolution Channel Scheme (CNM.CM != 0) */
    uint32_t                        fineFreqOffset[2];

    /* Frequency offset for IEEE-compliant Scheme (CNM.CM = 0) */
    uint32_t                        ieeeFreqOffset;

    /* Frequency tolerance T0 for FSK PHY in uQ0.45 */
    uint32_t                        fskTolT0;

    /* Fine channel mode for each frequency range (CNM.CM)  */
    uint8_t                         fineChnMode[2];

    /* Number of frequency ranges (2 in RF09, 1 in RF24) */
    uint8_t                         numFreqRanges;

} RF215_PLL_CONST_OBJ;

// *****************************************************************************
/* RF215 PLL Parameters Object

  Summary:
    Object used to keep any data required to manage an instance of the RF215 PLL
   (Frequency Synthesizer).

  Remarks:
    If both Sub-GHz and 2.4 GHz transceivers are enabled, there is one object
    for each transceiver.
*/

typedef struct
{
    /* Current channel frequency in Hz */
    uint32_t                        chnFreq;

    /* Current maximum frequency offset due to tolerance (single-sided clock) */
    uint32_t                        freqDelta;

    /* Channel mode in use (CNM.CM) */
    uint8_t                         chnMode;

    /* Frequency range in use */
    uint8_t                         freqRng;

} RF215_PLL_PARAMS_OBJ;

// *****************************************************************************
/* RF215 Driver PHY Registers Object

  Summary:
    Object used to keep the values of RF215 PHY configuration registers.

  Remarks:
    If both Sub-GHz and 2.4 GHz transceivers are enabled, there is one object
    for each transceiver.
*/

typedef struct
{
    /* PLL registers */
	uint8_t                         RFn_CS;
    uint8_t                         RFn_CCF0L;
    uint8_t                         RFn_CCF0H;
    uint8_t                         RFn_CNL;
    uint8_t                         RFn_CNM;

    /* RX front-end registers */
    uint8_t                         RFn_RXBWC;
    uint8_t                         RFn_RXDFE;
    uint8_t                         RFn_AGCC;
    uint8_t                         RFn_AGCS;
    uint8_t                         RFn_RSSI; /* read-only */
    uint8_t                         RFn_EDC;
    uint8_t                         RFn_EDD;
    uint8_t                         RFn_EDV; /* read-only */
    uint8_t                         RFn_RNDV; /* read-only */

    /* TX front-end registers */
    uint8_t                         RFn_TXCUTC;
    uint8_t                         RFn_TXDFE;
    uint8_t                         RFn_PAC;

    /* BBC configuration registers */
    uint8_t                         BBCn_IRQM;
    uint8_t                         BBCn_PC;
    uint8_t                         BBCn_PS; /* read-only */

    /* BBC Frame Buffer registers */
    uint8_t                         BBCn_RXFLL; /* read-only */
    uint8_t                         BBCn_RXFLH; /* read-only */
    uint8_t                         BBCn_TXFLL;
    uint8_t                         BBCn_TXFLH;
    uint8_t                         BBCn_FBLL; /* read-only */
    uint8_t                         BBCn_FBLH; /* read-only */
    uint8_t                         BBCn_FBLIL;
    uint8_t                         BBCn_FBLIH;

    /* OFDM configuration registers */
    uint8_t                         BBCn_OFDMPHRTX;
    uint8_t                         BBCn_OFDMPHRRX;
    uint8_t                         BBCn_OFDMC;
    uint8_t                         BBCn_OFDMSW;

    /* IEEE MAC Support Registers */
    uint8_t                         BBCn_AMCS;
    uint8_t                         BBCn_AMEDT;

    /* FSK configuration registers */
    uint8_t                         BBCn_FSKC0;
    uint8_t                         BBCn_FSKC1;
    uint8_t                         BBCn_FSKC2;
    uint8_t                         BBCn_FSKC3;
    uint8_t                         BBCn_FSKPHRTX;
    uint8_t                         BBCn_FSKPHRRX; /* read-only */
    uint8_t                         BBCn_FSKDM;
    uint8_t                         BBCn_FSKPE0;
    uint8_t                         BBCn_FSKPE1;
    uint8_t                         BBCn_FSKPE2;

    /* Timestamp Counter registers */
    uint8_t                         BBCn_CNT0; /* read-only */
    uint8_t                         BBCn_CNT1; /* read-only */
    uint8_t                         BBCn_CNT2; /* read-only */
    uint8_t                         BBCn_CNT3; /* read-only */

} RF215_PHY_REGS_OBJ;

// *****************************************************************************
/* RF215 Driver PHY Statistics Object

  Summary:
    Object used to keep the statistics of RF215 PHY.

  Remarks:
    If both Sub-GHz and 2.4 GHz transceivers are enabled, there is one object
    for each transceiver.
*/

typedef struct
{
    uint32_t                        txTotal;
    uint32_t                        txTotalBytes;
    uint32_t                        txErrTotal;
    uint32_t                        txErrBusyTx;
    uint32_t                        txErrBusyChn;
    uint32_t                        txErrBusyRx;
    uint32_t                        txErrBadLen;
    uint32_t                        txErrBadFormat;
    uint32_t                        txErrTimeout;
    uint32_t                        txErrAborted;
    uint32_t                        txCfmNotHandled;
    uint32_t                        rxTotal;
    uint32_t                        rxTotalBytes;
    uint32_t                        rxErrTotal;
    uint32_t                        rxErrFalsePositive;
    uint32_t                        rxErrBadLen;
    uint32_t                        rxErrBadFormat;
    uint32_t                        rxErrBadFcsPay;
    uint32_t                        rxErrAborted;
    uint32_t                        rxOverride;
    uint32_t                        rxIndNotHandled;

} RF215_PHY_STATISTICS_OBJ;

// *****************************************************************************
/* RF215 Driver PHY Instance Object

  Summary:
    Object used to keep any data required for an instance of the RF215 driver
    PHY.

  Remarks:
    If both Sub-GHz and 2.4 GHz transceivers are enabled, there is one object
    for each transceiver.
*/

typedef struct
{
    /* PHY statistics */
    RF215_PHY_STATISTICS_OBJ        phyStatistics;

    /* PHY configuration in use */
    DRV_RF215_PHY_CFG_OBJ           phyConfig;

    /* PHY configuration pending to be set */
    DRV_RF215_PHY_CFG_OBJ           phyConfigPending;

    /* PLL (Frequency Synthesizer) object  */
    RF215_PLL_PARAMS_OBJ            pllParams;

    /* RX indication object */
    DRV_RF215_RX_INDICATION_OBJ     rxInd;

    /* Pointer to TX buffer being transmitted */
    DRV_RF215_TX_BUFFER_OBJ*        txBufObj;

    /* Pointer to TX buffer pending to be transmitted */
    DRV_RF215_TX_BUFFER_OBJ*        txBufObjPending;

    /* Frequency band / operating mode (simplified PHY configuration) in use */
    DRV_RF215_PHY_BAND_OPM          bandOpMode;

    /* Current PHY state */
    volatile RF215_PHY_STATE        phyState;

    /* PHY state when RX was aborted */
    RF215_PHY_STATE                 rxAbortState;

    /* Pending TX state to be continued in TRXRDY interrupt */
    RF215_PHY_STATE                 txPendingState;

    /* RF215 TRX counter read in capture mode for TX/RX time computation */
    uint32_t                        trxTimeCapture;

    /* Delay between TX/EDM_SINGLE command and TX start time in us Qx.5  */
    uint32_t                        txCmdDelayUSq5;

    /* Turnaround time in us, as defined in 802.15.4 */
    uint16_t                        turnaroundTimeUS;

    /* Frequency channel number in use */
    uint16_t                        channelNum;

    /* Frequency channel number pending to be set */
    uint16_t                        channelNumPending;

    /* RX buffer offset (number of bytes already read in FBLI interrupt) */
    uint16_t                        rxBufferOffset;

    /* Number of payload symbols of the PPDU being received */
    uint16_t                        rxPaySymbols;

    /* Number of payload symbols of the last transmitted PPDU */
    uint16_t                        txPaySymbols;

    /* Current RF215 TRX state */
    uint8_t                         trxState;

    /* RXFE/AGCR interrupt flags pending to be processed */
    uint8_t                         rxFlagsPending;

    /* TRXRDY flag (PLL lock status) */
    bool                            trxRdy;

    /* Flag to indicate a new RX indication needs to be notified */
    volatile bool                   rxIndPending;

    /* Flag to indicate that TXFE interrupt is pending to be processed */
    bool                            txfePending;

    /* RX LED status */
    bool                            ledRxStatus;

    /* Flag to indicate that TX parameters have been configured */
    bool                            txStarted;

    /* CCATX/TX2RX auto procedure in progress */
    bool                            txAutoInProgress;

    /* Flag to indicate that RX time is valid (at least one frame received) */
    bool                            rxTimeValid;

    /* Flag to indicate that TRX reset is pending */
    bool                            trxResetPending;

    /* Flag to indicate that TRX sleep is pending */
    bool                            trxSleepPending;

    /* Flag to indicate that PHY configuration is pending */
    bool                            phyCfgPending;

    /* Flag to indicate that TX cancel is pending */
    bool                            txCancelPending;

    /* Flag to indicate that TX request is pending */
    bool                            txRequestPending;

    /* TRX reset in progress */
    bool                            resetInProgress;

    /* RF215 PHY configuration register values */
    RF215_PHY_REGS_OBJ              phyRegs;

    /* Received PSDU buffer */
    uint8_t                         rxPsdu[DRV_RF215_MAX_PSDU_LEN];

} RF215_PHY_OBJ;

// *****************************************************************************
// *****************************************************************************
// Section: RF215 Driver PHY Interface
// *****************************************************************************
// *****************************************************************************

bool RF215_PHY_Initialize (
    uint8_t trxIdx,
    DRV_RF215_PHY_BAND_OPM bandOpMode,
    uint16_t channelNum
);

void RF215_PHY_Tasks (uint8_t trxIdx);

void RF215_PHY_ExtIntEvent(uint8_t trxIdx, uint8_t rfIRQS, uint8_t bbcIRQS);

DRV_RF215_TX_RESULT RF215_PHY_TxRequest(DRV_RF215_TX_BUFFER_OBJ* txBufObj);

void RF215_PHY_TxCancel(DRV_RF215_TX_BUFFER_OBJ* txBufObj);

void RF215_PHY_SetTxCfm (
    DRV_RF215_TX_BUFFER_OBJ* txBufObj,
    DRV_RF215_TX_RESULT result
);

DRV_RF215_PIB_RESULT RF215_PHY_GetPib (
    uint8_t trxIndex,
    DRV_RF215_PIB_ATTRIBUTE attr,
    void* value
);

DRV_RF215_PIB_RESULT RF215_PHY_SetPib (
    uint8_t trxIndex,
    DRV_RF215_PIB_ATTRIBUTE attr,
    void* value
);

void RF215_PHY_Reset(uint8_t trxIndex);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* _RF215_PHY_H */
