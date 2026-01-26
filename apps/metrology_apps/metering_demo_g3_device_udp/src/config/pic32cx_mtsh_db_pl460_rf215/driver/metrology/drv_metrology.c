/*******************************************************************************
  Metrology Driver.

  Company:
    Microchip Technology Inc.

  File Name:
    drv_metrology.c

  Summary:
    Metrology Driver source file.

  Description:
    None
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*
Copyright (C) 2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

// *****************************************************************************
// *****************************************************************************
// Section: Include Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "system/int/sys_int.h"
#include "drv_metrology.h"
#include "drv_metrology_definitions.h"
#include "drv_metrology_local.h"
#include "peripheral/pio/plib_pio.h"
#include "peripheral/rstc/plib_rstc.h"
#include "peripheral/clk/plib_clk.h"
#include "interrupts.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data
// *****************************************************************************
// *****************************************************************************

#define MAX_WAIT_LOOPS 100000UL
#define CAL_NUM_PRE_INTEGRATION_PERIODS        2U
#define CAL_NUM_INTEGRATION_PERIODS            4U
#define THRESHOLD_PQ_DIR                       -1.0f

typedef enum {
    PENERGY = 0U,
    QENERGY = 1U,
} DRV_METROLOGY_ENERGY_TYPE;

/* This is the driver instance object array. */
static DRV_METROLOGY_OBJ gDrvMetObj;

static const DRV_METROLOGY_REGS_CONTROL gDrvMetControlDefault =
{
    STATE_CTRL_STATE_CTRL_RESET_Val,                  /* 00 STATE_CTRL */
    (uint32_t)(DRV_METROLOGY_CONF_FCTRL),             /* 01 FEATURE_CTRL */
    (uint32_t)(DRV_METROLOGY_CONF_AFE_SEL),           /* 02 AFE_SELECTION */
    (uint32_t)(DRV_METROLOGY_CONF_CHN_MATRIX),        /* 03 CHANNEL_MATRIX */
    (uint32_t)(DRV_METROLOGY_CONF_HARMONIC_CTRL),     /* 04 HARMONIC CTRL */
    (uint32_t)(DRV_METROLOGY_CONF_MT),                /* 05 METER_TYPE sensor_type =0 CT, 1 SHUNT, 2 ROGOWSKI */
    (uint32_t)(0x00000000UL),                         /* 06 M M=50->50Hz M=60->60Hz */
    (uint32_t)(0x00001130UL),                         /* 07 N_MAX 4400=0x1130 */
    (uint32_t)(DRV_METROLOGY_CONF_PULSE0_CTRL),       /* 08 PULSE0_CTRL */
    (uint32_t)(DRV_METROLOGY_CONF_PULSE0_KT),         /* 09 PULSE0_K_t */
    (uint32_t)(DRV_METROLOGY_CONF_PULSE1_CTRL),       /* 10 PULSE1_CTRL */
    (uint32_t)(DRV_METROLOGY_CONF_PULSE1_KT),         /* 11 PULSE1_K_t */
    (uint32_t)(DRV_METROLOGY_CONF_PULSE2_CTRL),       /* 12 PULSE2_CTRL */
    (uint32_t)(DRV_METROLOGY_CONF_PULSE2_KT),         /* 13 PULSE2_K_t */
    (uint32_t)(DRV_METROLOGY_CONF_SYNTH_ADDR),        /* 14 SYNTHESIZER_ADDR */
    (uint32_t)(DRV_METROLOGY_CONF_CREEP_P),           /* 15 CREEP_THR_P */
    (uint32_t)(DRV_METROLOGY_CONF_CREEP_PA),          /* 16 CREEP_THR_P_A */
    (uint32_t)(DRV_METROLOGY_CONF_CREEP_PB),          /* 17 CREEP_THR_P_B */
    (uint32_t)(DRV_METROLOGY_CONF_CREEP_PC),          /* 18 CREEP_THR_P_C */
    (uint32_t)(DRV_METROLOGY_CONF_CREEP_Q),           /* 19 CREEP_THR_Q */
    (uint32_t)(DRV_METROLOGY_CONF_CREEP_QA),          /* 20 CREEP_THR_Q_A */
    (uint32_t)(DRV_METROLOGY_CONF_CREEP_QB),          /* 21 CREEP_THR_Q_B */
    (uint32_t)(DRV_METROLOGY_CONF_CREEP_QC),          /* 22 CREEP_THR_Q_C */
    (uint32_t)(DRV_METROLOGY_CONF_CREEP_I),           /* 23 CREEP_THR_I */
    (uint32_t)(DRV_METROLOGY_CONF_CREEP_IA),          /* 24 CREEP_THR_I_A */
    (uint32_t)(DRV_METROLOGY_CONF_CREEP_IB),          /* 25 CREEP_THR_I_B */
    (uint32_t)(DRV_METROLOGY_CONF_CREEP_IC),          /* 26 CREEP_THR_I_C */
    (uint32_t)(DRV_METROLOGY_CONF_CREEP_S),           /* 27 CREEP_THR_S */
    (uint32_t)(0x00000000UL),                         /* 28 POWER_OFFSET_CTRL */
    (int32_t)(0x00000000L),                           /* 29 POWER_OFFSET_P */
    (int32_t)(0x00000000L),                           /* 30 POWER_OFFSET_Q */
    (int32_t)(0x00000000L),                           /* 31 POWER_OFFSET_S */
    (uint32_t)(DRV_METROLOGY_CONF_SWELLA),            /* 32 SWELL_THR_VA */
    (uint32_t)(DRV_METROLOGY_CONF_SWELLB),            /* 33 SWELL_THR_VB */
    (uint32_t)(DRV_METROLOGY_CONF_SWELLC),            /* 34 SWELL_THR_VC */
    (uint32_t)(DRV_METROLOGY_CONF_SAGA),              /* 35 SAG_THR_VA */
    (uint32_t)(DRV_METROLOGY_CONF_SAGB),              /* 36 SAG_THR_VB */
    (uint32_t)(DRV_METROLOGY_CONF_SAGC),              /* 37 SAG_THR_VC */
    (uint32_t)(DRV_METROLOGY_CONF_INTA),              /* 38 INTERRUPT_THR_VA */
    (uint32_t)(DRV_METROLOGY_CONF_INTB),              /* 39 INTERRUPT_THR_VB */
    (uint32_t)(DRV_METROLOGY_CONF_INTC),              /* 40 INTERRUPT_THR_VC */
    (uint32_t)(0x00000000UL),                         /* 41 RESERVED_C41 */
    (uint32_t)(0x00000000UL),                         /* 42 RESERVED_C42 */
    (uint32_t)(0x00000000UL),                         /* 43 RESERVED_C43 */
    (uint32_t)(DRV_METROLOGY_CONF_KIA),               /* 44 K_IA */
    (uint32_t)(DRV_METROLOGY_CONF_KVA),               /* 45 K_VA */
    (uint32_t)(DRV_METROLOGY_CONF_KIB),               /* 46 K_IB */
    (uint32_t)(DRV_METROLOGY_CONF_KVB),               /* 47 K_VB */
    (uint32_t)(DRV_METROLOGY_CONF_KIC),               /* 48 K_IC */
    (uint32_t)(DRV_METROLOGY_CONF_KVC),               /* 49 K_VC */
    (uint32_t)(DRV_METROLOGY_CONF_KIN),               /* 50 K_IN */
    (uint32_t)(DRV_METROLOGY_CONF_KVD),               /* 51 K_VD */
    (int32_t)(0x20000000L),                           /* 52 CAL_M_IA */
    (int32_t)(0x20000000L),                           /* 53 CAL_M_VA */
    (int32_t)(0x20000000L),                           /* 54 CAL_M_IB */
    (int32_t)(0x20000000L),                           /* 55 CAL_M_VB */
    (int32_t)(0x20000000L),                           /* 56 CAL_M_IC */
    (int32_t)(0x20000000L),                           /* 57 CAL_M_VC */
    (int32_t)(0x20000000L),                           /* 58 CAL_M_IN */
    (int32_t)(0x20000000L),                           /* 59 CAL_M_VD */
    (int32_t)(0x00000000L),                           /* 60 CAL_PH_IA */
    (int32_t)(0x00000000L),                           /* 61 CAL_PH_VA */
    (int32_t)(0x00000000L),                           /* 62 CAL_PH_IB */
    (int32_t)(0x00000000L),                           /* 63 CAL_PH_VB */
    (int32_t)(0x00000000L),                           /* 64 CAL_PH_IC */
    (int32_t)(0x00000000L),                           /* 65 CAL_PH_VC */
    (int32_t)(0x00000000L),                           /* 66 CAL_PH_IN */
    (uint32_t)(0x00000000UL),                         /* 67 RESERVED_C67 */
    (uint32_t)(0x00000000UL),                         /* 68 CAPTURE_CTRL */
    (uint32_t)(0x00000000UL),                         /* 69 CAPTURE_BUFF_SIZE */
    (uint32_t)(0x00000000UL),                         /* 70 CAPTURE_ADDR */
    (uint32_t)(0x00000000UL),                         /* 71 RESERVED_C71 */
    (uint32_t)(0x00000000UL),                         /* 72 RESERVED_C72 */
    (uint32_t)(0x00000000UL),                         /* 73 RESERVED_C73 */
    (uint32_t)(DRV_METROLOGY_CONF_AFE_CTRL),          /* 74 AFE_CTRL */
    (uint32_t)(0x00000000UL),                         /* 75 RESERVED_C75 */
    (uint32_t)(0x00000000UL),                         /* 76 RESERVED_C76 */
    (uint32_t)(0x00000000UL),                         /* 77 RESERVED_C77 */
    (int32_t)(0x00000000L),                           /* 78 POWER_OFFSET_P_A */
    (int32_t)(0x00000000L),                           /* 79 POWER_OFFSET_P_B */
    (int32_t)(0x00000000L),                           /* 80 POWER_OFFSET_P_C */
    (int32_t)(0x00000000L),                           /* 81 POWER_OFFSET_Q_A */
    (int32_t)(0x00000000L),                           /* 82 POWER_OFFSET_Q_B */
    (int32_t)(0x00000000L),                           /* 83 POWER_OFFSET_Q_C */
};

static const char gDrvAFEDescription[] = "1 ATSENSE203";

static const char gDrvPhyChannel0Name[] = "TEMP";
static const char gDrvPhyChannel1Name[] = "I_A";
static const char gDrvPhyChannel2Name[] = "V_A";
static const char gDrvPhyChannel3Name[] = "I_B";
static const char gDrvPhyChannel4Name[] = "V_B";

static const DRV_METROLOGY_CHANNEL gDrvMetChannelsDefault[DRV_METROLOGY_CHANNELS_NUMBER] =
{
    {(char *)&gDrvPhyChannel0Name, GAIN_1, SENSOR_TEMP},
    {(char *)&gDrvPhyChannel1Name, GAIN_1, SENSOR_CT},
    {(char *)&gDrvPhyChannel2Name, GAIN_1, SENSOR_VRD},
    {(char *)&gDrvPhyChannel3Name, GAIN_1, SENSOR_CT},
    {(char *)&gDrvPhyChannel4Name, GAIN_1, SENSOR_VRD},
};

// *****************************************************************************
// *****************************************************************************
// Section: File scope functions
// *****************************************************************************
// *****************************************************************************

static void lDRV_METROLOGY_UpdateEvents(void)
{
    uint32_t stateFlagReg = gDrvMetObj.metRegisters->MET_STATUS.STATE_FLAG;
    float *afeMeasure = gDrvMetObj.metAFEData.cycleMeasure;
    DRV_METROLOGY_AFE_EVENTS *pEvents = &gDrvMetObj.metAFEData.afeEvents;

    /* Update Swell/Sag/Creep/Phase Active events */
    pEvents->swellA = (stateFlagReg & STATUS_STATE_FLAG_SWELL_DET_VA_Msk) > 0U? 1U : 0U;
    pEvents->sagA = (stateFlagReg & STATUS_STATE_FLAG_SAG_DET_VA_Msk) > 0U? 1U : 0U;
    pEvents->creepPA = (stateFlagReg & STATUS_STATE_FLAG_CREEP_DET_PA_Msk) > 0U? 1U : 0U;
    pEvents->creepQA = (stateFlagReg & STATUS_STATE_FLAG_CREEP_DET_QA_Msk) > 0U? 1U : 0U;
    pEvents->creepIA = (stateFlagReg & STATUS_STATE_FLAG_CREEP_DET_IA_Msk) > 0U? 1U : 0U;
    pEvents->paDir = (*(afeMeasure + (uint8_t)MEASURE_PA) < THRESHOLD_PQ_DIR)? 1U : 0U;
    pEvents->qaDir = (*(afeMeasure + (uint8_t)MEASURE_QA) < THRESHOLD_PQ_DIR)? 1U : 0U;
    pEvents->pafDir = (*(afeMeasure + (uint8_t)MEASURE_PAF) < THRESHOLD_PQ_DIR)? 1U : 0U;
    pEvents->qafDir = (*(afeMeasure + (uint8_t)MEASURE_QAF) < THRESHOLD_PQ_DIR)? 1U : 0U;
    pEvents->phActiveA = (stateFlagReg & STATUS_STATE_FLAG_PH_A_ACTIVE_Msk) > 0U? 1U : 0U;
    pEvents->swellB = (stateFlagReg & STATUS_STATE_FLAG_SWELL_DET_VB_Msk) > 0U? 1U : 0U;
    pEvents->sagB = (stateFlagReg & STATUS_STATE_FLAG_SAG_DET_VB_Msk) > 0U? 1U : 0U;
    pEvents->creepPB = (stateFlagReg & STATUS_STATE_FLAG_CREEP_DET_PB_Msk) > 0U? 1U : 0U;
    pEvents->creepQB = (stateFlagReg & STATUS_STATE_FLAG_CREEP_DET_QB_Msk) > 0U? 1U : 0U;
    pEvents->creepIB = (stateFlagReg & STATUS_STATE_FLAG_CREEP_DET_IB_Msk) > 0U? 1U : 0U;
    pEvents->pbDir = (*(afeMeasure + (uint8_t)MEASURE_PB) < THRESHOLD_PQ_DIR)? 1U : 0U;
    pEvents->qbDir = (*(afeMeasure + (uint8_t)MEASURE_QB) < THRESHOLD_PQ_DIR)? 1U : 0U;
    pEvents->pbfDir = (*(afeMeasure + (uint8_t)MEASURE_PBF) < THRESHOLD_PQ_DIR)? 1U : 0U;
    pEvents->qbfDir = (*(afeMeasure + (uint8_t)MEASURE_QBF) < THRESHOLD_PQ_DIR)? 1U : 0U;
    pEvents->phActiveB = (stateFlagReg & STATUS_STATE_FLAG_PH_B_ACTIVE_Msk) > 0U? 1U : 0U;
    pEvents->swellC = (stateFlagReg & STATUS_STATE_FLAG_SWELL_DET_VC_Msk) > 0U? 1U : 0U;
    pEvents->sagC = (stateFlagReg & STATUS_STATE_FLAG_SAG_DET_VC_Msk) > 0U? 1U : 0U;
    pEvents->creepPC = (stateFlagReg & STATUS_STATE_FLAG_CREEP_DET_PC_Msk) > 0U? 1U : 0U;
    pEvents->creepQC = (stateFlagReg & STATUS_STATE_FLAG_CREEP_DET_QC_Msk) > 0U? 1U : 0U;
    pEvents->creepIC = (stateFlagReg & STATUS_STATE_FLAG_CREEP_DET_IC_Msk) > 0U? 1U : 0U;
    pEvents->pcDir = (*(afeMeasure + (uint8_t)MEASURE_PC) < THRESHOLD_PQ_DIR)? 1U : 0U;
    pEvents->qcDir = (*(afeMeasure + (uint8_t)MEASURE_QC) < THRESHOLD_PQ_DIR)? 1U : 0U;
    pEvents->pcfDir = (*(afeMeasure + (uint8_t)MEASURE_PCF) < THRESHOLD_PQ_DIR)? 1U : 0U;
    pEvents->qcfDir = (*(afeMeasure + (uint8_t)MEASURE_QCF) < THRESHOLD_PQ_DIR)? 1U : 0U;
    pEvents->phActiveC = (stateFlagReg & STATUS_STATE_FLAG_PH_C_ACTIVE_Msk) > 0U? 1U : 0U;
    pEvents->creepP = (stateFlagReg & STATUS_STATE_FLAG_CREEP_DET_P_Msk) > 0U? 1U : 0U;
    pEvents->creepQ = (stateFlagReg & STATUS_STATE_FLAG_CREEP_DET_Q_Msk) > 0U? 1U : 0U;
    pEvents->creepS = (stateFlagReg & STATUS_STATE_FLAG_CREEP_DET_S_Msk) > 0U? 1U : 0U;
    pEvents->ptDir = (*(afeMeasure + (uint8_t)MEASURE_PT) < THRESHOLD_PQ_DIR)? 1U : 0U;
    pEvents->qtDir = (*(afeMeasure + (uint8_t)MEASURE_QT) < THRESHOLD_PQ_DIR)? 1U : 0U;
    pEvents->ptfDir = (*(afeMeasure + (uint8_t)MEASURE_PTF) < THRESHOLD_PQ_DIR)? 1U : 0U;
    pEvents->qtfDir = (*(afeMeasure + (uint8_t)MEASURE_QTF) < THRESHOLD_PQ_DIR)? 1U : 0U;
}

void IPC1_InterruptHandler (void)
{
    uint32_t status = IPC1_REGS->IPC_ISR;
    status &= IPC1_REGS->IPC_IMR;

    if ((status & DRV_METROLOGY_IPC_INIT_IRQ_MSK) != 0UL)
    {
        if (gDrvMetObj.metRegisters->MET_STATUS.STATUS == STATUS_STATUS_RESET)
        {
            gDrvMetObj.status = DRV_METROLOGY_STATUS_INIT_DSP;
        }
    }

    if ((status & DRV_METROLOGY_IPC_INTEGRATION_IRQ_MSK) != 0UL)
    {
        if (gDrvMetObj.metRegisters->MET_STATUS.STATUS == STATUS_STATUS_DSP_RUNNING)
        {
            /* Update Accumulators Data */
            (void) memcpy(&gDrvMetObj.metAccData, &gDrvMetObj.metRegisters->MET_ACCUMULATORS, sizeof(DRV_METROLOGY_REGS_ACCUMULATORS));
            /* Store samples in period */
            gDrvMetObj.samplesInPeriod = gDrvMetObj.metRegisters->MET_STATUS.N;
            /* Update Frequency Data */
            gDrvMetObj.metFreqData.freq = gDrvMetObj.metRegisters->MET_STATUS.FREQ;
            gDrvMetObj.metFreqData.freqA = gDrvMetObj.metRegisters->MET_STATUS.FREQ_VA;
            gDrvMetObj.metFreqData.freqB = gDrvMetObj.metRegisters->MET_STATUS.FREQ_VB;
            gDrvMetObj.metFreqData.freqC = gDrvMetObj.metRegisters->MET_STATUS.FREQ_VC;
            /* Update Zero-Cross Data */
            gDrvMetObj.metZCData.zcA = gDrvMetObj.metRegisters->MET_STATUS.ZC_N_VA;
            gDrvMetObj.metZCData.zcB = gDrvMetObj.metRegisters->MET_STATUS.ZC_N_VB;
            gDrvMetObj.metZCData.zcC = gDrvMetObj.metRegisters->MET_STATUS.ZC_N_VC;

            if (gDrvMetObj.harmonicAnalysisData.holdRegs == false)
            {
                /* Update Harmonics Data */
                (void) memcpy(&gDrvMetObj.metHarData, &gDrvMetObj.metRegisters->MET_HARMONICS, sizeof(DRV_METROLOGY_REGS_HARMONICS));
            }
        }

        gDrvMetObj.integrationFlag = true;
    }

    if ((status & DRV_METROLOGY_IPC_FULLCYCLE_IRQ_MSK) != 0UL)
    {
        gDrvMetObj.fullCycleFlag = true;

        /* Update Per-Cycle Accumulators Data */
        (void) memcpy(&gDrvMetObj.metPerCycleAccData, &gDrvMetObj.metRegisters->MET_PERCYCLE_ACC, sizeof(DRV_METROLOGY_REGS_PERCYCLE_ACC));
        /* Store samples in cycle */
        gDrvMetObj.samplesInCycle = gDrvMetObj.metRegisters->MET_STATUS.N_CYCLE;
        /* Update Frequency Data */
        gDrvMetObj.metFreqData.freq = gDrvMetObj.metRegisters->MET_STATUS.FREQ;
        gDrvMetObj.metFreqData.freqA = gDrvMetObj.metRegisters->MET_STATUS.FREQ_VA;
        gDrvMetObj.metFreqData.freqB = gDrvMetObj.metRegisters->MET_STATUS.FREQ_VB;
        gDrvMetObj.metFreqData.freqC = gDrvMetObj.metRegisters->MET_STATUS.FREQ_VC;
        /* Update Zero-Cross Data */
        gDrvMetObj.metZCData.zcA = gDrvMetObj.metRegisters->MET_STATUS.ZC_N_VA;
        gDrvMetObj.metZCData.zcB = gDrvMetObj.metRegisters->MET_STATUS.ZC_N_VB;
        gDrvMetObj.metZCData.zcC = gDrvMetObj.metRegisters->MET_STATUS.ZC_N_VC;
    }

    if ((status & DRV_METROLOGY_IPC_HALFCYCLE_IRQ_MSK) != 0UL)
    {
        /* Update events */
        lDRV_METROLOGY_UpdateEvents();

        if (gDrvMetObj.halfCycleCallback != NULL)
        {
            gDrvMetObj.halfCycleCallback();
        }
    }

    IPC1_REGS->IPC_ICCR = status;

    gDrvMetObj.ipcInterruptFlag = true;
}

static double lDRV_Metrology_GetHarmonicRMS(int32_t real, int32_t imag, uint32_t k)
{
    double rmsRe, rmsIm, rms;
    double m, kx;
    uint64_t divisor;

    /* k [uQ22.10] */
    divisor = (1ULL << FORMAT_CONST_uQ2210);
    kx = (double)k / (double)divisor;

    /* Get Real contribution [sQ13.18] */
    divisor = (1ULL << FORMAT_CONST_sQ1318);
    m = (double)real / (double)divisor;

    rmsRe = m * kx;

    /* Get Imag contribution [sQ13.18] */
    m = (double)imag / (double)divisor;

    rmsIm = m * kx;

    rms = 2.0 * (rmsRe * rmsRe + rmsIm * rmsIm);
    if (rms > 0.0)
    {
        rms = sqrt(rms);
        rms = rms / (double)gDrvMetObj.samplesInPeriod;
    }
    else
    {
        rms = 0.0;
    }

    return rms;
}

static double lDRV_Metrology_GetVIRMS(uint64_t val, uint32_t k_x, double samples)
{
    double m, k;
    uint64_t divisor;

    /* k [uQ22.10] */
    divisor = (1ULL << FORMAT_CONST_uQ2210);
    k = (double)k_x / (double)divisor;

    /* value [uQ24.40] */
    divisor = (1ULL << FORMAT_CONST_uQ2440);
    m = (double)val / (double)divisor;
    m = m / samples;

    if (m > 0.0)
    {
        m = sqrt(m);
        m = m * k;
    }
    else
    {
        m = 0.0;
    }

    return m;
}

static double lDRV_Metrology_GetInxRMS(uint64_t val, double samples)
{
    double m;
    uint64_t divisor;

    /* value [uQ44.20] */
    divisor = (1ULL << FORMAT_CONST_uQ4420);
    m = (double)val / (double)divisor;
    m = m / samples;

    if (m < 0.0)
    {
        m = 0.0;
    }
    else
    {
        m = sqrt(m);
    }

    return m;
}

static double lDRV_Metrology_GetPQ(int64_t val, uint32_t k_ix, uint32_t k_vx, double samples)
{
    double m, ki, kv;
    uint64_t divisor;

    /* k [uQ22.10] */
    divisor = (1ULL << FORMAT_CONST_uQ2210);
    ki = (double)k_ix / (double)divisor;
    kv = (double)k_vx / (double)divisor;

    /* value [sQ23.40] */
    divisor = (1ULL << FORMAT_CONST_sQ2340);
    m = (double)val / (double)divisor;
    m = m / samples;

    m *= (ki * kv);

    return m;
}

static double lDRV_Metrology_GetS(uint64_t i_val, uint64_t v_val, uint32_t k_ix, uint32_t k_vx, double samples)
{
    double m;
    double mi, mv;

    mi = lDRV_Metrology_GetVIRMS(i_val, k_ix, samples);
    mv = lDRV_Metrology_GetVIRMS(v_val, k_vx, samples);

    m = mi * mv;

    return m;
}

static double lDRV_Metrology_GetPQSOffsetTimesFreq(int32_t powerOffsetReg)
{
    double offset, freq;
    uint64_t divisor;

    /* Power Offset [sQ1.30] */
    divisor = (1ULL << FORMAT_CONST_sQ130);
    offset = (double)powerOffsetReg / (double)divisor;

    /* Frecuency [uQ20.12] */
    divisor = (1ULL << FORMAT_CONST_uQ2012);
    freq = (double)gDrvMetObj.metFreqData.freq / (double)divisor;

    offset *= freq;

    return offset;
}

static double lDRV_Metrology_GetPowerOffset(int32_t powerOffsetReg)
{
    double offset;

    offset = lDRV_Metrology_GetPQSOffsetTimesFreq(powerOffsetReg);
    offset = offset * SECS_IN_HOUR_DOUBLE; /* offset = offset * 3600 * freq (Wh/Var/VA) */

    return offset;
}

static double lDRV_Metrology_GetPOffset(void)
{
    double offsetP = 0.0;

    if ((gDrvMetObj.metRegisters->MET_CONTROL.POWER_OFFSET_CTRL & POWER_OFFSET_CTRL_P_OFFSET_PUL_Msk) != 0U)
    {
        /* Compute global active power offset in W */
        offsetP = lDRV_Metrology_GetPowerOffset(gDrvMetObj.metRegisters->MET_CONTROL.POWER_OFFSET_P);
    }

    return offsetP;
}

static double lDRV_Metrology_GetQOffset(void)
{
    double offsetQ = 0.0;

    if ((gDrvMetObj.metRegisters->MET_CONTROL.POWER_OFFSET_CTRL & POWER_OFFSET_CTRL_Q_OFFSET_PUL_Msk) != 0U)
    {
        /* Compute global active power offset in Var */
        offsetQ = lDRV_Metrology_GetPowerOffset(gDrvMetObj.metRegisters->MET_CONTROL.POWER_OFFSET_Q);
    }

    return offsetQ;
}

static double lDRV_Metrology_GetAngle(int64_t p, int64_t q)
{
    double angle, pd, qd;

    pd = (double)p;
    qd = (double)q;
    angle = atan2(qd, pd);
    angle = 180.0f * angle;
    angle = angle / M_PI;

    return angle;
}

static double lDRV_Metrology_GetAngleVAx(uint64_t vaxf, uint64_t vaf, uint64_t vxf, uint32_t zcx)
{
    double angle, divisor;
    double zc_A, zc_X, zc_diff;
    double threshold, freq, periodSamples;
    uint32_t qFactor;

    if ((vaxf == 0UL) || (vaf == 0UL) || (vxf == 0UL))
    {
        return 0.0;
    }

    angle = (double)vaxf - ((double)vaf + (double)vxf);
    divisor = (double)vaf * (double)vxf;
    if (divisor > 0.0)
    {
        divisor = -2.0 * sqrt(divisor);
        angle = angle / divisor;

        if ((angle > 1.0) || (angle < -1.0))
        {
            return 0.0;
        }

        angle = acos(angle);
        angle = 180.0 * angle;
        angle = angle / M_PI;

        /* Frequency [uQ20.12] */
        qFactor = (1UL << FORMAT_CONST_uQ2012);
        freq = (double)gDrvMetObj.metFreqData.freq / (double)qFactor;
        periodSamples = (double)SAMPLING_FREQ / freq;

        /* Resolve the ambiguity of acos(x) using zero-crossing data [uQ20.12]  */
        zc_A = (double)gDrvMetObj.metZCData.zcA / (double)qFactor;
        zc_X = (double)zcx / (double)qFactor;
        zc_diff = zc_A - zc_X;
        if (zc_diff < 0.0)
        {
            /* Protection for the case when dominant voltage is not VA */
            zc_diff += periodSamples;
        }

        threshold = periodSamples / 2.0;
        if (zc_diff < threshold)
        {
            angle = -angle;
        }
    }
    else
    {
        angle = 0.0;
    }

    return angle;
}

static double lDRV_Metrology_GetEnergy(DRV_METROLOGY_ENERGY_TYPE id)
{
    double m, k, divisor;
    uint64_t div64;
    double ki, kv;
    double offset = 0.0;

    /* Power measurement [sQ23.40] */
    div64 = (1ULL << FORMAT_CONST_sQ2340);
    divisor = (double)div64 * (double)SAMPLING_FREQ;
    divisor *= (double)DIV_GAIN * (double)DIV_GAIN;

    /* Calculated as absolute values */
    if (id == PENERGY)
    {
        m = (double)gDrvMetObj.metAccData.P_A;
        ki = (double)gDrvMetObj.metRegisters->MET_CONTROL.K_IA;
        kv = (double)gDrvMetObj.metRegisters->MET_CONTROL.K_VA;
        k = (m * ki * kv) / divisor;

        m = (double)gDrvMetObj.metAccData.P_B;
        ki = (double)gDrvMetObj.metRegisters->MET_CONTROL.K_IB;
        kv = (double)gDrvMetObj.metRegisters->MET_CONTROL.K_VB;
        k += (m * ki * kv) / divisor;

        m = (double)gDrvMetObj.metAccData.P_C;
        ki = (double)gDrvMetObj.metRegisters->MET_CONTROL.K_IC;
        kv = (double)gDrvMetObj.metRegisters->MET_CONTROL.K_VC;
        k += (m * ki * kv) / divisor;

        if ((gDrvMetObj.metRegisters->MET_CONTROL.POWER_OFFSET_CTRL & POWER_OFFSET_CTRL_P_OFFSET_PUL_Msk) != 0U)
        {
            /* Compute global active power offset in Wh */
            offset = lDRV_Metrology_GetPQSOffsetTimesFreq(gDrvMetObj.metRegisters->MET_CONTROL.POWER_OFFSET_P);
        }
    }
    else
    {
        /* reactive energy */
        m = (double)gDrvMetObj.metAccData.Q_A;
        ki = (double)gDrvMetObj.metRegisters->MET_CONTROL.K_IA;
        kv = (double)gDrvMetObj.metRegisters->MET_CONTROL.K_VA;
        k = (m * ki * kv) / divisor;

        m = (double)gDrvMetObj.metAccData.Q_B;
        ki = (double)gDrvMetObj.metRegisters->MET_CONTROL.K_IB;
        kv = (double)gDrvMetObj.metRegisters->MET_CONTROL.K_VB;
        k += (m * ki * kv) / divisor;

        m = (double)gDrvMetObj.metAccData.Q_C;
        ki = (double)gDrvMetObj.metRegisters->MET_CONTROL.K_IC;
        kv = (double)gDrvMetObj.metRegisters->MET_CONTROL.K_VC;
        k += (m * ki * kv) / divisor;

        if ((gDrvMetObj.metRegisters->MET_CONTROL.POWER_OFFSET_CTRL & POWER_OFFSET_CTRL_Q_OFFSET_PUL_Msk) != 0U)
        {
            /* Compute global reactive power offset in Varh */
            offset = lDRV_Metrology_GetPQSOffsetTimesFreq(gDrvMetObj.metRegisters->MET_CONTROL.POWER_OFFSET_Q);
        }
    }

    k = k / SECS_IN_HOUR_DOUBLE; /* (Wh/Varh) */
    offset = offset * (double)gDrvMetObj.samplesInPeriod / SAMPLING_FREQ; /* offset = offset * num_cycles (Wh/Varh) */
    k -= offset; /* Compensate global offset */

    return k;  /* xxxx (kWh/kVarh) */
}

static void lDRV_Metrology_IpcInitialize (void)
{
    /* Clear interrupts */
    IPC1_REGS->IPC_ICCR = 0xFFFFFFFFUL;
    /* Enable interrupts */
    IPC1_REGS->IPC_IECR = DRV_METROLOGY_IPC_INIT_IRQ_MSK |
        DRV_METROLOGY_IPC_FULLCYCLE_IRQ_MSK |
        DRV_METROLOGY_IPC_HALFCYCLE_IRQ_MSK |
        DRV_METROLOGY_IPC_INTEGRATION_IRQ_MSK;
}

static int32_t lDRV_Metrology_CorrectAngle(double measured, double reference)
{
    double bams, correction_angle, freq;
    uint32_t qFactor;
    bool outRange;

    /* Get difference between measured and reference angles */
    correction_angle = measured - reference;

    /* Correction angle should be between -180 and 180 degrees */
    outRange = (bool)(correction_angle < -180.0);
    while (outRange == true)
    {
        correction_angle += 360.0;
        outRange = (bool)(correction_angle < -180.0);
    }

    outRange = (bool)(correction_angle >= 180.0);
    while (outRange == true)
    {
        correction_angle -= 360.0;
        outRange = (bool)(correction_angle >= 180.0);
    }

    qFactor = (1UL << FORMAT_CONST_uQ2012);
    freq = (double)gDrvMetObj.metFreqData.freq / (double)qFactor;

    bams = correction_angle * (60.00 / freq);
    bams = bams / 180.0;

    /* Apply Format [sQ0.31] */
    qFactor = (1UL << FORMAT_CONST_sQ031);
    bams = bams * (double)qFactor;

    return (int32_t)round(bams);
}

static void lDRV_METROLOGY_UpdateMeasurements(void)
{
    double samples;
    float *afeMeasure = NULL;
    float totalPower;
    float powerOffset;
    uint32_t divisor;

    /* Update Measure values */
    afeMeasure = gDrvMetObj.metAFEData.measure;

    samples = (double)gDrvMetObj.samplesInPeriod;

    if (samples > 0.0)
    {
        *(afeMeasure + (uint8_t)MEASURE_UA_RMS) = (float)lDRV_Metrology_GetVIRMS(gDrvMetObj.metAccData.V_A, gDrvMetObj.metRegisters->MET_CONTROL.K_VA, samples);
        *(afeMeasure + (uint8_t)MEASURE_UAF_RMS) = (float)lDRV_Metrology_GetVIRMS(gDrvMetObj.metAccData.V_A_F, gDrvMetObj.metRegisters->MET_CONTROL.K_VA, samples);

        *(afeMeasure + (uint8_t)MEASURE_UB_RMS) = (float)lDRV_Metrology_GetVIRMS(gDrvMetObj.metAccData.V_B, gDrvMetObj.metRegisters->MET_CONTROL.K_VB, samples);
        *(afeMeasure + (uint8_t)MEASURE_UBF_RMS) = (float)lDRV_Metrology_GetVIRMS(gDrvMetObj.metAccData.V_B_F, gDrvMetObj.metRegisters->MET_CONTROL.K_VB, samples);

        *(afeMeasure + (uint8_t)MEASURE_UC_RMS) = (float)lDRV_Metrology_GetVIRMS(gDrvMetObj.metAccData.V_C, gDrvMetObj.metRegisters->MET_CONTROL.K_VC, samples);
        *(afeMeasure + (uint8_t)MEASURE_UCF_RMS) = (float)lDRV_Metrology_GetVIRMS(gDrvMetObj.metAccData.V_C_F, gDrvMetObj.metRegisters->MET_CONTROL.K_VC, samples);

        *(afeMeasure + (uint8_t)MEASURE_IA_RMS) = (float)lDRV_Metrology_GetVIRMS(gDrvMetObj.metAccData.I_A, gDrvMetObj.metRegisters->MET_CONTROL.K_IA, samples);
        *(afeMeasure + (uint8_t)MEASURE_IAF_RMS) = (float)lDRV_Metrology_GetVIRMS(gDrvMetObj.metAccData.I_A_F, gDrvMetObj.metRegisters->MET_CONTROL.K_IA, samples);

        *(afeMeasure + (uint8_t)MEASURE_IB_RMS) = (float)lDRV_Metrology_GetVIRMS(gDrvMetObj.metAccData.I_B, gDrvMetObj.metRegisters->MET_CONTROL.K_IB, samples);
        *(afeMeasure + (uint8_t)MEASURE_IBF_RMS) = (float)lDRV_Metrology_GetVIRMS(gDrvMetObj.metAccData.I_B_F, gDrvMetObj.metRegisters->MET_CONTROL.K_IB, samples);

        *(afeMeasure + (uint8_t)MEASURE_IC_RMS) = (float)lDRV_Metrology_GetVIRMS(gDrvMetObj.metAccData.I_C, gDrvMetObj.metRegisters->MET_CONTROL.K_IC, samples);
        *(afeMeasure + (uint8_t)MEASURE_ICF_RMS) = (float)lDRV_Metrology_GetVIRMS(gDrvMetObj.metAccData.I_C_F, gDrvMetObj.metRegisters->MET_CONTROL.K_IC, samples);

        *(afeMeasure + (uint8_t)MEASURE_INI_RMS) = (float)lDRV_Metrology_GetInxRMS(gDrvMetObj.metAccData.I_Ni, samples);

        *(afeMeasure + (uint8_t)MEASURE_PA)  = (float)lDRV_Metrology_GetPQ(gDrvMetObj.metAccData.P_A, gDrvMetObj.metRegisters->MET_CONTROL.K_IA, gDrvMetObj.metRegisters->MET_CONTROL.K_VA, samples);
        *(afeMeasure + (uint8_t)MEASURE_QA)  = (float)lDRV_Metrology_GetPQ(gDrvMetObj.metAccData.Q_A, gDrvMetObj.metRegisters->MET_CONTROL.K_IA, gDrvMetObj.metRegisters->MET_CONTROL.K_VA, samples);
        *(afeMeasure + (uint8_t)MEASURE_SA)  = (float)lDRV_Metrology_GetS(gDrvMetObj.metAccData.I_A, gDrvMetObj.metAccData.V_A, gDrvMetObj.metRegisters->MET_CONTROL.K_IA, gDrvMetObj.metRegisters->MET_CONTROL.K_VA, samples);
        *(afeMeasure + (uint8_t)MEASURE_PAF)  = (float)lDRV_Metrology_GetPQ(gDrvMetObj.metAccData.P_A_F, gDrvMetObj.metRegisters->MET_CONTROL.K_IA, gDrvMetObj.metRegisters->MET_CONTROL.K_VA, samples);
        *(afeMeasure + (uint8_t)MEASURE_QAF)  = (float)lDRV_Metrology_GetPQ(gDrvMetObj.metAccData.Q_A_F, gDrvMetObj.metRegisters->MET_CONTROL.K_IA, gDrvMetObj.metRegisters->MET_CONTROL.K_VA, samples);
        *(afeMeasure + (uint8_t)MEASURE_SAF)  = (float)lDRV_Metrology_GetS(gDrvMetObj.metAccData.I_A_F, gDrvMetObj.metAccData.V_A_F, gDrvMetObj.metRegisters->MET_CONTROL.K_IA, gDrvMetObj.metRegisters->MET_CONTROL.K_VA, samples);

        *(afeMeasure + (uint8_t)MEASURE_PB)  = (float)lDRV_Metrology_GetPQ(gDrvMetObj.metAccData.P_B, gDrvMetObj.metRegisters->MET_CONTROL.K_IB, gDrvMetObj.metRegisters->MET_CONTROL.K_VB, samples);
        *(afeMeasure + (uint8_t)MEASURE_QB)  = (float)lDRV_Metrology_GetPQ(gDrvMetObj.metAccData.Q_B, gDrvMetObj.metRegisters->MET_CONTROL.K_IB, gDrvMetObj.metRegisters->MET_CONTROL.K_VB, samples);
        *(afeMeasure + (uint8_t)MEASURE_SB)  = (float)lDRV_Metrology_GetS(gDrvMetObj.metAccData.I_B, gDrvMetObj.metAccData.V_B, gDrvMetObj.metRegisters->MET_CONTROL.K_IB, gDrvMetObj.metRegisters->MET_CONTROL.K_VB, samples);
        *(afeMeasure + (uint8_t)MEASURE_PBF)  = (float)lDRV_Metrology_GetPQ(gDrvMetObj.metAccData.P_B_F, gDrvMetObj.metRegisters->MET_CONTROL.K_IB, gDrvMetObj.metRegisters->MET_CONTROL.K_VB, samples);
        *(afeMeasure + (uint8_t)MEASURE_QBF)  = (float)lDRV_Metrology_GetPQ(gDrvMetObj.metAccData.Q_B_F, gDrvMetObj.metRegisters->MET_CONTROL.K_IB, gDrvMetObj.metRegisters->MET_CONTROL.K_VB, samples);
        *(afeMeasure + (uint8_t)MEASURE_SBF)  = (float)lDRV_Metrology_GetS(gDrvMetObj.metAccData.I_B_F, gDrvMetObj.metAccData.V_B_F, gDrvMetObj.metRegisters->MET_CONTROL.K_IB, gDrvMetObj.metRegisters->MET_CONTROL.K_VB, samples);

        *(afeMeasure + (uint8_t)MEASURE_PC)  = (float)lDRV_Metrology_GetPQ(gDrvMetObj.metAccData.P_C, gDrvMetObj.metRegisters->MET_CONTROL.K_IC, gDrvMetObj.metRegisters->MET_CONTROL.K_VC, samples);
        *(afeMeasure + (uint8_t)MEASURE_QC)  = (float)lDRV_Metrology_GetPQ(gDrvMetObj.metAccData.Q_C, gDrvMetObj.metRegisters->MET_CONTROL.K_IC, gDrvMetObj.metRegisters->MET_CONTROL.K_VC, samples);
        *(afeMeasure + (uint8_t)MEASURE_SC)  = (float)lDRV_Metrology_GetS(gDrvMetObj.metAccData.I_C, gDrvMetObj.metAccData.V_C, gDrvMetObj.metRegisters->MET_CONTROL.K_IC, gDrvMetObj.metRegisters->MET_CONTROL.K_VC, samples);
        *(afeMeasure + (uint8_t)MEASURE_PCF)  = (float)lDRV_Metrology_GetPQ(gDrvMetObj.metAccData.P_C_F, gDrvMetObj.metRegisters->MET_CONTROL.K_IC, gDrvMetObj.metRegisters->MET_CONTROL.K_VC, samples);
        *(afeMeasure + (uint8_t)MEASURE_QCF)  = (float)lDRV_Metrology_GetPQ(gDrvMetObj.metAccData.Q_C_F, gDrvMetObj.metRegisters->MET_CONTROL.K_IC, gDrvMetObj.metRegisters->MET_CONTROL.K_VC, samples);
        *(afeMeasure + (uint8_t)MEASURE_SCF)  = (float)lDRV_Metrology_GetS(gDrvMetObj.metAccData.I_C_F, gDrvMetObj.metAccData.V_C_F, gDrvMetObj.metRegisters->MET_CONTROL.K_IC, gDrvMetObj.metRegisters->MET_CONTROL.K_VC, samples);

        powerOffset = (float)lDRV_Metrology_GetPOffset();

        totalPower = -powerOffset;
        totalPower += *(afeMeasure + (uint8_t)MEASURE_PA);
        totalPower += *(afeMeasure + (uint8_t)MEASURE_PB);
        totalPower += *(afeMeasure + (uint8_t)MEASURE_PC);
        *(afeMeasure + (uint8_t)MEASURE_PT) = totalPower;

        totalPower = -powerOffset;
        totalPower += *(afeMeasure + (uint8_t)MEASURE_PAF);
        totalPower += *(afeMeasure + (uint8_t)MEASURE_PBF);
        totalPower += *(afeMeasure + (uint8_t)MEASURE_PCF);
        *(afeMeasure + (uint8_t)MEASURE_PTF) = totalPower;

        powerOffset = (float)lDRV_Metrology_GetQOffset();

        totalPower = -powerOffset;
        totalPower += *(afeMeasure + (uint8_t)MEASURE_QA);
        totalPower += *(afeMeasure + (uint8_t)MEASURE_QB);
        totalPower += *(afeMeasure + (uint8_t)MEASURE_QC);
        *(afeMeasure + (uint8_t)MEASURE_QT) = totalPower;

        totalPower = -(float)powerOffset;
        totalPower += *(afeMeasure + (uint8_t)MEASURE_QAF);
        totalPower += *(afeMeasure + (uint8_t)MEASURE_QBF);
        totalPower += *(afeMeasure + (uint8_t)MEASURE_QCF);
        *(afeMeasure + (uint8_t)MEASURE_QTF) = totalPower;

        totalPower = 0.0f;
        totalPower += *(afeMeasure + (uint8_t)MEASURE_SA);
        totalPower += *(afeMeasure + (uint8_t)MEASURE_SB);
        totalPower += *(afeMeasure + (uint8_t)MEASURE_SC);
        *(afeMeasure + (uint8_t)MEASURE_ST) = totalPower;

        totalPower = -powerOffset;
        totalPower += *(afeMeasure + (uint8_t)MEASURE_SAF);
        totalPower += *(afeMeasure + (uint8_t)MEASURE_SBF);
        totalPower += *(afeMeasure + (uint8_t)MEASURE_SCF);
        *(afeMeasure + (uint8_t)MEASURE_STF) = totalPower;

        /* Freq [uQ20.12] */
        divisor = (1UL << FORMAT_CONST_uQ2012);
        *(afeMeasure + (uint8_t)MEASURE_FREQ)  = (float)gDrvMetObj.metFreqData.freq / (float)divisor;
        *(afeMeasure + (uint8_t)MEASURE_FREQA)  = (float)gDrvMetObj.metFreqData.freqA / (float)divisor;
        *(afeMeasure + (uint8_t)MEASURE_FREQB)  = (float)gDrvMetObj.metFreqData.freqB / (float)divisor;
        *(afeMeasure + (uint8_t)MEASURE_FREQC)  = (float)gDrvMetObj.metFreqData.freqC / (float)divisor;

        *(afeMeasure + (uint8_t)MEASURE_ANGLEA)  = (float)lDRV_Metrology_GetAngle(gDrvMetObj.metAccData.P_A, gDrvMetObj.metAccData.Q_A);
        *(afeMeasure + (uint8_t)MEASURE_ANGLEB)  = (float)lDRV_Metrology_GetAngle(gDrvMetObj.metAccData.P_B, gDrvMetObj.metAccData.Q_B);
        *(afeMeasure + (uint8_t)MEASURE_ANGLEC)  = (float)lDRV_Metrology_GetAngle(gDrvMetObj.metAccData.P_C, gDrvMetObj.metAccData.Q_C);
        *(afeMeasure + (uint8_t)MEASURE_ANGLEVAB)  = (float)lDRV_Metrology_GetAngleVAx(gDrvMetObj.metAccData.V_AB_F,
        gDrvMetObj.metAccData.V_A_F, gDrvMetObj.metAccData.V_B_F, gDrvMetObj.metZCData.zcB);
        *(afeMeasure + (uint8_t)MEASURE_ANGLEVAC)  = (float)lDRV_Metrology_GetAngleVAx(gDrvMetObj.metAccData.V_CA_F,
        gDrvMetObj.metAccData.V_A_F, gDrvMetObj.metAccData.V_C_F, gDrvMetObj.metZCData.zcC);

        gDrvMetObj.metAFEData.energy += (float)lDRV_Metrology_GetEnergy(PENERGY);


    }
}

static void lDRV_METROLOGY_UpdateCycleMeasurements(void)
{
    double samples;
    float *afeMeasure = NULL;
    float totalPower;
    float powerOffset;
    uint64_t divisor;

    /* Update Measure values */
    afeMeasure = gDrvMetObj.metAFEData.cycleMeasure;

    divisor = (1ULL << FORMAT_CONST_uQ2012);
    samples = (double)gDrvMetObj.samplesInCycle / (double)divisor;

    if (samples > 0.0)
    {
        *(afeMeasure + (uint8_t)MEASURE_UA_RMS) = (float)lDRV_Metrology_GetVIRMS(gDrvMetObj.metPerCycleAccData.V_A, gDrvMetObj.metRegisters->MET_CONTROL.K_VA, samples);
        *(afeMeasure + (uint8_t)MEASURE_UAF_RMS) = (float)lDRV_Metrology_GetVIRMS(gDrvMetObj.metPerCycleAccData.V_A_F, gDrvMetObj.metRegisters->MET_CONTROL.K_VA, samples);

        *(afeMeasure + (uint8_t)MEASURE_UB_RMS) = (float)lDRV_Metrology_GetVIRMS(gDrvMetObj.metPerCycleAccData.V_B, gDrvMetObj.metRegisters->MET_CONTROL.K_VB, samples);
        *(afeMeasure + (uint8_t)MEASURE_UBF_RMS) = (float)lDRV_Metrology_GetVIRMS(gDrvMetObj.metPerCycleAccData.V_B_F, gDrvMetObj.metRegisters->MET_CONTROL.K_VB, samples);

        *(afeMeasure + (uint8_t)MEASURE_UC_RMS) = (float)lDRV_Metrology_GetVIRMS(gDrvMetObj.metPerCycleAccData.V_C, gDrvMetObj.metRegisters->MET_CONTROL.K_VC, samples);
        *(afeMeasure + (uint8_t)MEASURE_UCF_RMS) = (float)lDRV_Metrology_GetVIRMS(gDrvMetObj.metPerCycleAccData.V_C_F, gDrvMetObj.metRegisters->MET_CONTROL.K_VC, samples);

        *(afeMeasure + (uint8_t)MEASURE_IA_RMS) = (float)lDRV_Metrology_GetVIRMS(gDrvMetObj.metPerCycleAccData.I_A, gDrvMetObj.metRegisters->MET_CONTROL.K_IA, samples);
        *(afeMeasure + (uint8_t)MEASURE_IAF_RMS) = (float)lDRV_Metrology_GetVIRMS(gDrvMetObj.metPerCycleAccData.I_A_F, gDrvMetObj.metRegisters->MET_CONTROL.K_IA, samples);

        *(afeMeasure + (uint8_t)MEASURE_IB_RMS) = (float)lDRV_Metrology_GetVIRMS(gDrvMetObj.metPerCycleAccData.I_B, gDrvMetObj.metRegisters->MET_CONTROL.K_IB, samples);
        *(afeMeasure + (uint8_t)MEASURE_IBF_RMS) = (float)lDRV_Metrology_GetVIRMS(gDrvMetObj.metPerCycleAccData.I_B_F, gDrvMetObj.metRegisters->MET_CONTROL.K_IB, samples);

        *(afeMeasure + (uint8_t)MEASURE_IC_RMS) = (float)lDRV_Metrology_GetVIRMS(gDrvMetObj.metPerCycleAccData.I_C, gDrvMetObj.metRegisters->MET_CONTROL.K_IC, samples);
        *(afeMeasure + (uint8_t)MEASURE_ICF_RMS) = (float)lDRV_Metrology_GetVIRMS(gDrvMetObj.metPerCycleAccData.I_C_F, gDrvMetObj.metRegisters->MET_CONTROL.K_IC, samples);

        *(afeMeasure + (uint8_t)MEASURE_PA)  = (float)lDRV_Metrology_GetPQ(gDrvMetObj.metPerCycleAccData.P_A, gDrvMetObj.metRegisters->MET_CONTROL.K_IA, gDrvMetObj.metRegisters->MET_CONTROL.K_VA, samples);
        *(afeMeasure + (uint8_t)MEASURE_QA)  = (float)lDRV_Metrology_GetPQ(gDrvMetObj.metPerCycleAccData.Q_A, gDrvMetObj.metRegisters->MET_CONTROL.K_IA, gDrvMetObj.metRegisters->MET_CONTROL.K_VA, samples);
        *(afeMeasure + (uint8_t)MEASURE_SA)  = (float)lDRV_Metrology_GetS(gDrvMetObj.metPerCycleAccData.I_A, gDrvMetObj.metPerCycleAccData.V_A, gDrvMetObj.metRegisters->MET_CONTROL.K_IA, gDrvMetObj.metRegisters->MET_CONTROL.K_VA, samples);
        *(afeMeasure + (uint8_t)MEASURE_PAF)  = (float)lDRV_Metrology_GetPQ(gDrvMetObj.metPerCycleAccData.P_A_F, gDrvMetObj.metRegisters->MET_CONTROL.K_IA, gDrvMetObj.metRegisters->MET_CONTROL.K_VA, samples);
        *(afeMeasure + (uint8_t)MEASURE_QAF)  = (float)lDRV_Metrology_GetPQ(gDrvMetObj.metPerCycleAccData.Q_A_F, gDrvMetObj.metRegisters->MET_CONTROL.K_IA, gDrvMetObj.metRegisters->MET_CONTROL.K_VA, samples);
        *(afeMeasure + (uint8_t)MEASURE_SAF)  = (float)lDRV_Metrology_GetS(gDrvMetObj.metPerCycleAccData.I_A_F, gDrvMetObj.metPerCycleAccData.V_A_F, gDrvMetObj.metRegisters->MET_CONTROL.K_IA, gDrvMetObj.metRegisters->MET_CONTROL.K_VA, samples);

        *(afeMeasure + (uint8_t)MEASURE_PB)  = (float)lDRV_Metrology_GetPQ(gDrvMetObj.metPerCycleAccData.P_B, gDrvMetObj.metRegisters->MET_CONTROL.K_IB, gDrvMetObj.metRegisters->MET_CONTROL.K_VB, samples);
        *(afeMeasure + (uint8_t)MEASURE_QB)  = (float)lDRV_Metrology_GetPQ(gDrvMetObj.metPerCycleAccData.Q_B, gDrvMetObj.metRegisters->MET_CONTROL.K_IB, gDrvMetObj.metRegisters->MET_CONTROL.K_VB, samples);
        *(afeMeasure + (uint8_t)MEASURE_SB)  = (float)lDRV_Metrology_GetS(gDrvMetObj.metPerCycleAccData.I_B, gDrvMetObj.metPerCycleAccData.V_B, gDrvMetObj.metRegisters->MET_CONTROL.K_IB, gDrvMetObj.metRegisters->MET_CONTROL.K_VB, samples);
        *(afeMeasure + (uint8_t)MEASURE_PBF)  = (float)lDRV_Metrology_GetPQ(gDrvMetObj.metPerCycleAccData.P_B_F, gDrvMetObj.metRegisters->MET_CONTROL.K_IB, gDrvMetObj.metRegisters->MET_CONTROL.K_VB, samples);
        *(afeMeasure + (uint8_t)MEASURE_QBF)  = (float)lDRV_Metrology_GetPQ(gDrvMetObj.metPerCycleAccData.Q_B_F, gDrvMetObj.metRegisters->MET_CONTROL.K_IB, gDrvMetObj.metRegisters->MET_CONTROL.K_VB, samples);
        *(afeMeasure + (uint8_t)MEASURE_SBF)  = (float)lDRV_Metrology_GetS(gDrvMetObj.metPerCycleAccData.I_B_F, gDrvMetObj.metPerCycleAccData.V_B_F, gDrvMetObj.metRegisters->MET_CONTROL.K_IB, gDrvMetObj.metRegisters->MET_CONTROL.K_VB, samples);

        *(afeMeasure + (uint8_t)MEASURE_PC)  = (float)lDRV_Metrology_GetPQ(gDrvMetObj.metPerCycleAccData.P_C, gDrvMetObj.metRegisters->MET_CONTROL.K_IC, gDrvMetObj.metRegisters->MET_CONTROL.K_VC, samples);
        *(afeMeasure + (uint8_t)MEASURE_QC)  = (float)lDRV_Metrology_GetPQ(gDrvMetObj.metPerCycleAccData.Q_C, gDrvMetObj.metRegisters->MET_CONTROL.K_IC, gDrvMetObj.metRegisters->MET_CONTROL.K_VC, samples);
        *(afeMeasure + (uint8_t)MEASURE_SC)  = (float)lDRV_Metrology_GetS(gDrvMetObj.metPerCycleAccData.I_C, gDrvMetObj.metPerCycleAccData.V_C, gDrvMetObj.metRegisters->MET_CONTROL.K_IC, gDrvMetObj.metRegisters->MET_CONTROL.K_VC, samples);
        *(afeMeasure + (uint8_t)MEASURE_PCF)  = (float)lDRV_Metrology_GetPQ(gDrvMetObj.metPerCycleAccData.P_C_F, gDrvMetObj.metRegisters->MET_CONTROL.K_IC, gDrvMetObj.metRegisters->MET_CONTROL.K_VC, samples);
        *(afeMeasure + (uint8_t)MEASURE_QCF)  = (float)lDRV_Metrology_GetPQ(gDrvMetObj.metPerCycleAccData.Q_C_F, gDrvMetObj.metRegisters->MET_CONTROL.K_IC, gDrvMetObj.metRegisters->MET_CONTROL.K_VC, samples);
        *(afeMeasure + (uint8_t)MEASURE_SCF)  = (float)lDRV_Metrology_GetS(gDrvMetObj.metPerCycleAccData.I_C_F, gDrvMetObj.metPerCycleAccData.V_C_F, gDrvMetObj.metRegisters->MET_CONTROL.K_IC, gDrvMetObj.metRegisters->MET_CONTROL.K_VC, samples);

        powerOffset = (float)lDRV_Metrology_GetPOffset();

        totalPower = -powerOffset;
        totalPower += *(afeMeasure + (uint8_t)MEASURE_PA);
        totalPower += *(afeMeasure + (uint8_t)MEASURE_PB);
        totalPower += *(afeMeasure + (uint8_t)MEASURE_PC);
        *(afeMeasure + (uint8_t)MEASURE_PT) = totalPower;

        totalPower = -powerOffset;
        totalPower += *(afeMeasure + (uint8_t)MEASURE_PAF);
        totalPower += *(afeMeasure + (uint8_t)MEASURE_PBF);
        totalPower += *(afeMeasure + (uint8_t)MEASURE_PCF);
        *(afeMeasure + (uint8_t)MEASURE_PTF) = totalPower;

        powerOffset = (float)lDRV_Metrology_GetQOffset();

        totalPower = -powerOffset;
        totalPower += *(afeMeasure + (uint8_t)MEASURE_QA);
        totalPower += *(afeMeasure + (uint8_t)MEASURE_QB);
        totalPower += *(afeMeasure + (uint8_t)MEASURE_QC);
        *(afeMeasure + (uint8_t)MEASURE_QT) = totalPower;

        totalPower = -powerOffset;
        totalPower += *(afeMeasure + (uint8_t)MEASURE_QAF);
        totalPower += *(afeMeasure + (uint8_t)MEASURE_QBF);
        totalPower += *(afeMeasure + (uint8_t)MEASURE_QCF);
        *(afeMeasure + (uint8_t)MEASURE_QTF) = totalPower;

        totalPower = 0.0f;
        totalPower += *(afeMeasure + (uint8_t)MEASURE_SA);
        totalPower += *(afeMeasure + (uint8_t)MEASURE_SB);
        totalPower += *(afeMeasure + (uint8_t)MEASURE_SC);
        *(afeMeasure + (uint8_t)MEASURE_ST) = totalPower;

        totalPower = -powerOffset;
        totalPower += *(afeMeasure + (uint8_t)MEASURE_SAF);
        totalPower += *(afeMeasure + (uint8_t)MEASURE_SBF);
        totalPower += *(afeMeasure + (uint8_t)MEASURE_SCF);
        *(afeMeasure + (uint8_t)MEASURE_STF) = totalPower;

        /* Freq [uQ20.12] */
        divisor = (1ULL << FORMAT_CONST_uQ2012);
        *(afeMeasure + (uint8_t)MEASURE_FREQ)  = (float)gDrvMetObj.metFreqData.freq / (float)divisor;
        *(afeMeasure + (uint8_t)MEASURE_FREQA)  = (float)gDrvMetObj.metFreqData.freqA / (float)divisor;
        *(afeMeasure + (uint8_t)MEASURE_FREQB)  = (float)gDrvMetObj.metFreqData.freqB / (float)divisor;
        *(afeMeasure + (uint8_t)MEASURE_FREQC)  = (float)gDrvMetObj.metFreqData.freqC / (float)divisor;

        *(afeMeasure + (uint8_t)MEASURE_ANGLEA)  = (float)lDRV_Metrology_GetAngle(gDrvMetObj.metPerCycleAccData.P_A, gDrvMetObj.metPerCycleAccData.Q_A);
        *(afeMeasure + (uint8_t)MEASURE_ANGLEB)  = (float)lDRV_Metrology_GetAngle(gDrvMetObj.metPerCycleAccData.P_B, gDrvMetObj.metPerCycleAccData.Q_B);
        *(afeMeasure + (uint8_t)MEASURE_ANGLEC)  = (float)lDRV_Metrology_GetAngle(gDrvMetObj.metPerCycleAccData.P_C, gDrvMetObj.metPerCycleAccData.Q_C);
        *(afeMeasure + (uint8_t)MEASURE_ANGLEVAB)  = (float)lDRV_Metrology_GetAngleVAx(gDrvMetObj.metPerCycleAccData.V_AB_F,
        gDrvMetObj.metPerCycleAccData.V_A_F, gDrvMetObj.metPerCycleAccData.V_B_F, gDrvMetObj.metZCData.zcB);
        *(afeMeasure + (uint8_t)MEASURE_ANGLEVAC)  = (float)lDRV_Metrology_GetAngleVAx(gDrvMetObj.metPerCycleAccData.V_CA_F,
        gDrvMetObj.metPerCycleAccData.V_A_F, gDrvMetObj.metPerCycleAccData.V_C_F, gDrvMetObj.metZCData.zcC);
    }
}

static double lDRV_METROLOGY_GetRefRmsRatio(double ref, double rms)
{
    uint64_t qFactor;
    double ratio;

    qFactor = (1ULL << FORMAT_CONST_sQ229);

    if (rms == 0.0)
    {
        /* Do not apply calibration (ratio=1) */
        ratio = 1.0;
    }
    else
    {
        ratio = ref / rms;
        if (ratio >= 4.0)
        {
            /* Calibration cannot be performed (overflow) */
            ratio = 1.0;
        }
    }
    /* Scale by Q Factor */
    ratio = ratio * (double)qFactor;

    return ratio;
}

static bool lDRV_METROLOGY_UpdateCalibrationValues(void)
{
    DRV_METROLOGY_CALIBRATION * pCalibrationData;
    DRV_METROLOGY_REGS_ACCUMULATORS * pMetAccRegs;

    pCalibrationData = &gDrvMetObj.calibrationData;
    pMetAccRegs = &gDrvMetObj.metAccData;

    if (pCalibrationData->numPreIntegrationPeriods != 0U)
    {
        pCalibrationData->numPreIntegrationPeriods--;

        return false;
    }
    else
    {
        if (pCalibrationData->numIntegrationPeriods != 0U)
        {
            pCalibrationData->numIntegrationPeriods--;

            /* Update Accumulators */
            pCalibrationData->dspAccIa += pMetAccRegs->I_A;
            pCalibrationData->dspAccIb += pMetAccRegs->I_B;
            pCalibrationData->dspAccIc += pMetAccRegs->I_C;
            pCalibrationData->dspAccIn += pMetAccRegs->I_Nm;
            pCalibrationData->dspAccUa += pMetAccRegs->V_A;
            pCalibrationData->dspAccUb += pMetAccRegs->V_B;
            pCalibrationData->dspAccUc += pMetAccRegs->V_C;
            pCalibrationData->dspAccUd += pMetAccRegs->V_D;
            pCalibrationData->dspAccUaf += pMetAccRegs->V_A_F;
            pCalibrationData->dspAccUbf += pMetAccRegs->V_B_F;
            pCalibrationData->dspAccUcf += pMetAccRegs->V_C_F;
            pCalibrationData->dspAccUdf += pMetAccRegs->V_D_F;
            pCalibrationData->dspAccUabf += pMetAccRegs->V_AB_F;
            pCalibrationData->dspAccUcaf += pMetAccRegs->V_CA_F;
            pCalibrationData->dspAccPa += pMetAccRegs->P_A_F;
            pCalibrationData->dspAccPb += pMetAccRegs->P_B_F;
            pCalibrationData->dspAccPc += pMetAccRegs->P_C_F;
            pCalibrationData->dspAccPn += pMetAccRegs->P_N_F;
            pCalibrationData->dspAccQa += pMetAccRegs->Q_A_F;
            pCalibrationData->dspAccQb += pMetAccRegs->Q_B_F;
            pCalibrationData->dspAccQc += pMetAccRegs->Q_C_F;
            pCalibrationData->dspAccQn += pMetAccRegs->Q_N_F;

            return false;
        }
        else
        {
            DRV_METROLOGY_REGS_CONTROL * pMetControlRegs;
            double m, rms, angle;
            double samples;
            DRV_METROLOGY_CALIBRATION_MASK calMask;
            int32_t calPhVbOffset, calPhVcOffset;

            pMetControlRegs = &gDrvMetObj.metRegisters->MET_CONTROL;
            samples = (double)gDrvMetObj.samplesInPeriod;

            /* The number of the required integration periods has been completed */
            /* Get Calibration Values */
            calMask = pCalibrationData->references.calMask;
            if (calMask.magnitudeIa != 0U)
            {
                /* Calibration of IA RMS */
                pCalibrationData->dspAccIa /= CAL_NUM_INTEGRATION_PERIODS;
                rms = lDRV_Metrology_GetVIRMS(pCalibrationData->dspAccIa, pMetControlRegs->K_IA, samples);
                m = lDRV_METROLOGY_GetRefRmsRatio(pCalibrationData->references.ia, rms);
                pMetControlRegs->CAL_M_IA = (int32_t)round(m);
            }

            if (calMask.magnitudeVa != 0U)
            {
                /* Calibration of VA RMS */
                pCalibrationData->dspAccUa /= CAL_NUM_INTEGRATION_PERIODS;
                rms = lDRV_Metrology_GetVIRMS(pCalibrationData->dspAccUa, pMetControlRegs->K_VA, samples);
                m = lDRV_METROLOGY_GetRefRmsRatio(pCalibrationData->references.va, rms);
                pMetControlRegs->CAL_M_VA = (int32_t)round(m);
            }

            if (calMask.magnitudeIb != 0U)
            {
                /* Calibration of IB RMS */
                pCalibrationData->dspAccIb /= CAL_NUM_INTEGRATION_PERIODS;
                rms = lDRV_Metrology_GetVIRMS(pCalibrationData->dspAccIb, pMetControlRegs->K_IB, samples);
                m = lDRV_METROLOGY_GetRefRmsRatio(pCalibrationData->references.ib, rms);
                pMetControlRegs->CAL_M_IB = (int32_t)round(m);
            }

            if (calMask.magnitudeVb != 0U)
            {
                /* Calibration of VB RMS */
                pCalibrationData->dspAccUb /= CAL_NUM_INTEGRATION_PERIODS;
                rms = lDRV_Metrology_GetVIRMS(pCalibrationData->dspAccUb, pMetControlRegs->K_VB, samples);
                m = lDRV_METROLOGY_GetRefRmsRatio(pCalibrationData->references.vb, rms);
                pMetControlRegs->CAL_M_VB = (int32_t)round(m);
            }

            if (calMask.magnitudeIc != 0U)
            {
                /* Calibration of IC RMS */
                pCalibrationData->dspAccIc /= CAL_NUM_INTEGRATION_PERIODS;
                rms = lDRV_Metrology_GetVIRMS(pCalibrationData->dspAccIc, pMetControlRegs->K_IC, samples);
                m = lDRV_METROLOGY_GetRefRmsRatio(pCalibrationData->references.ic, rms);
                pMetControlRegs->CAL_M_IC = (int32_t)round(m);
            }

            if (calMask.magnitudeVc != 0U)
            {
                /* Calibration of VC RMS */
                pCalibrationData->dspAccUc /= CAL_NUM_INTEGRATION_PERIODS;
                rms = lDRV_Metrology_GetVIRMS(pCalibrationData->dspAccUc, pMetControlRegs->K_VC, samples);
                m = lDRV_METROLOGY_GetRefRmsRatio(pCalibrationData->references.vc, rms);
                pMetControlRegs->CAL_M_VC = (int32_t)round(m);
            }

            if (calMask.magnitudeIn != 0U)
            {
                /* Calibration of IN RMS */
                pCalibrationData->dspAccIn /= CAL_NUM_INTEGRATION_PERIODS;
                rms = lDRV_Metrology_GetVIRMS(pCalibrationData->dspAccIn, pMetControlRegs->K_IN, samples);
                m = lDRV_METROLOGY_GetRefRmsRatio(pCalibrationData->references.in, rms);
                pMetControlRegs->CAL_M_IN = (int32_t)round(m);
            }

            if (calMask.magnitudeVd != 0U)
            {
                /* Calibration of VD RMS */
                pCalibrationData->dspAccUd /= CAL_NUM_INTEGRATION_PERIODS;
                rms = lDRV_Metrology_GetVIRMS(pCalibrationData->dspAccUd, pMetControlRegs->K_VD, samples);
                m = lDRV_METROLOGY_GetRefRmsRatio(pCalibrationData->references.vd, rms);
                pMetControlRegs->CAL_M_VD = (int32_t)round(m);
            }

            /* PH_VA not modified (reference) */

            if (calMask.anglePhaseA != 0U)
            {
                /* Calibration of PH_IA (angle between IA and VA) */
                pCalibrationData->dspAccPa /= (int64_t)CAL_NUM_INTEGRATION_PERIODS;
                pCalibrationData->dspAccQa /= (int64_t)CAL_NUM_INTEGRATION_PERIODS;
                angle = lDRV_Metrology_GetAngle(pCalibrationData->dspAccPa, pCalibrationData->dspAccQa);
                pMetControlRegs->CAL_PH_IA = lDRV_Metrology_CorrectAngle(angle, pCalibrationData->references.aa);
            }

            pCalibrationData->dspAccUaf /= CAL_NUM_INTEGRATION_PERIODS;
            calPhVbOffset = 0;
            if (calMask.angleVAB != 0U)
            {
                /* Calibration of PH_VB: Angle between voltage VB and VA */
                pCalibrationData->dspAccUabf /= CAL_NUM_INTEGRATION_PERIODS;
                pCalibrationData->dspAccUbf /= CAL_NUM_INTEGRATION_PERIODS;
                angle = lDRV_Metrology_GetAngleVAx(pCalibrationData->dspAccUabf, pCalibrationData->dspAccUaf,
                    pCalibrationData->dspAccUbf, gDrvMetObj.metZCData.zcB);
                calPhVbOffset = lDRV_Metrology_CorrectAngle(angle, pCalibrationData->references.avab);
                pMetControlRegs->CAL_PH_VB = calPhVbOffset;
            }

            if (calMask.anglePhaseB != 0U)
            {
                /* Calibration of PH_IB (angle between IB and VB) */
                pCalibrationData->dspAccPb /= (int64_t)CAL_NUM_INTEGRATION_PERIODS;
                pCalibrationData->dspAccQb /= (int64_t)CAL_NUM_INTEGRATION_PERIODS;
                angle = lDRV_Metrology_GetAngle(pCalibrationData->dspAccPb, pCalibrationData->dspAccQb);
                pMetControlRegs->CAL_PH_IB = lDRV_Metrology_CorrectAngle(angle, pCalibrationData->references.ab);
                pMetControlRegs->CAL_PH_IB += calPhVbOffset;
            }

            calPhVcOffset = 0;
            if (calMask.angleVAC != 0U)
            {
                /* Calibration of PH_VC: Angle between voltage VC and VA */
                pCalibrationData->dspAccUcaf /= CAL_NUM_INTEGRATION_PERIODS;
                pCalibrationData->dspAccUcf /= CAL_NUM_INTEGRATION_PERIODS;
                angle = lDRV_Metrology_GetAngleVAx(pCalibrationData->dspAccUcaf, pCalibrationData->dspAccUaf,
                    pCalibrationData->dspAccUcf, gDrvMetObj.metZCData.zcC);
                calPhVcOffset = lDRV_Metrology_CorrectAngle(angle, pCalibrationData->references.avac);
                pMetControlRegs->CAL_PH_VC = calPhVcOffset;
            }

            if (calMask.anglePhaseC != 0U)
            {
                /* Calibration of PH_IC (angle between IC and VC) */
                pCalibrationData->dspAccPc /= (int64_t)CAL_NUM_INTEGRATION_PERIODS;
                pCalibrationData->dspAccQc /= (int64_t)CAL_NUM_INTEGRATION_PERIODS;
                angle = lDRV_Metrology_GetAngle(pCalibrationData->dspAccPc, pCalibrationData->dspAccQc);
                pMetControlRegs->CAL_PH_IC = lDRV_Metrology_CorrectAngle(angle, pCalibrationData->references.ac);
                pMetControlRegs->CAL_PH_IC += calPhVcOffset;
            }

            if (calMask.anglePhaseN != 0U)
            {
                uint32_t timingVx;

                /* Calibration of PH_IN (angle between IN and dominant voltage) */
                pCalibrationData->dspAccPn /= (int64_t)CAL_NUM_INTEGRATION_PERIODS;
                pCalibrationData->dspAccQn /= (int64_t)CAL_NUM_INTEGRATION_PERIODS;
                angle = lDRV_Metrology_GetAngle(pCalibrationData->dspAccPn, pCalibrationData->dspAccQn);
                pMetControlRegs->CAL_PH_IN = lDRV_Metrology_CorrectAngle(angle, pCalibrationData->references.an);

                /* Check dominant voltage */
                timingVx = (gDrvMetObj.metRegisters->MET_STATUS.STATE_FLAG & STATUS_STATE_FLAG_TIMING_Vx_Msk) >> STATUS_STATE_FLAG_TIMING_Vx_Pos;
                if (timingVx != STATUS_STATE_FLAG_TIMING_Vx_A_Val)
                {
                    /* If dominant voltage is VB or VC and it has been modified, it has to be compensated */
                    if (timingVx == STATUS_STATE_FLAG_TIMING_Vx_B_Val)
                    {
                        /* VB is the dominant voltage */
                        pMetControlRegs->CAL_PH_IN += calPhVbOffset;
                    }
                    else
                    {
                        /* VC is the dominant voltage */
                        pMetControlRegs->CAL_PH_IN += calPhVcOffset;
                    }
                }
            }

            pCalibrationData->result = true;

            /* Restore FEATURE_CTRL after calibration */
            pMetControlRegs->FEATURE_CTRL = pCalibrationData->featureCtrlBackup;

            /* Calibration has been completed */
            pCalibrationData->running = false;

            return true;
        }
    }
}

static void lDRV_METROLOGY_UpdateHarmonicAnalysisValues(void)
{
    DRV_METROLOGY_HARMONICS_RMS *pHarmonicsRsp = gDrvMetObj.harmonicAnalysisData.pHarmonicAnalysisResponse;
    DRV_METROLOGY_REGS_HARMONICS *pHarData = &gDrvMetObj.metHarData;
    int32_t real, imag;
    uint32_t k;
    uint8_t index;
    uint32_t harmonicBitmap = gDrvMetObj.harmonicAnalysisData.harmonicBitmap;

    for (index = 0; index < DRV_METROLOGY_HARMONICS_MAX_ORDER; index++)
    {
        if ((harmonicBitmap & (1UL << index)) != 0U)
        {
            real = (int32_t)pHarData->I_A_m_R[index];
            imag = (int32_t)pHarData->I_A_m_I[index];
            k = gDrvMetObj.metRegisters->MET_CONTROL.K_IA;
            pHarmonicsRsp->Irms_A_m = lDRV_Metrology_GetHarmonicRMS(real, imag, k);

            real = (int32_t)pHarData->I_B_m_R[index];
            imag = (int32_t)pHarData->I_B_m_I[index];
            k = gDrvMetObj.metRegisters->MET_CONTROL.K_IB;
            pHarmonicsRsp->Irms_B_m = lDRV_Metrology_GetHarmonicRMS(real, imag, k);

            real = (int32_t)pHarData->I_C_m_R[index];
            imag = (int32_t)pHarData->I_C_m_I[index];
            k = gDrvMetObj.metRegisters->MET_CONTROL.K_IC;
            pHarmonicsRsp->Irms_C_m = lDRV_Metrology_GetHarmonicRMS(real, imag, k);

            real = (int32_t)pHarData->I_N_m_R[index];
            imag = (int32_t)pHarData->I_N_m_I[index];
            k = gDrvMetObj.metRegisters->MET_CONTROL.K_IN;
            pHarmonicsRsp->Irms_N_m = lDRV_Metrology_GetHarmonicRMS(real, imag, k);

            real = (int32_t)pHarData->V_A_m_R[index];
            imag = (int32_t)pHarData->V_A_m_I[index];
            k = gDrvMetObj.metRegisters->MET_CONTROL.K_VA;
            pHarmonicsRsp->Vrms_A_m = lDRV_Metrology_GetHarmonicRMS(real, imag, k);

            real = (int32_t)pHarData->V_B_m_R[index];
            imag = (int32_t)pHarData->V_B_m_I[index];
            k = gDrvMetObj.metRegisters->MET_CONTROL.K_VB;
            pHarmonicsRsp->Vrms_B_m = lDRV_Metrology_GetHarmonicRMS(real, imag, k);

            real = (int32_t)pHarData->V_C_m_R[index];
            imag = (int32_t)pHarData->V_C_m_I[index];
            k = gDrvMetObj.metRegisters->MET_CONTROL.K_VC;
            pHarmonicsRsp->Vrms_C_m = lDRV_Metrology_GetHarmonicRMS(real, imag, k);
        }
        else
        {
            pHarmonicsRsp->Irms_A_m = 0.0;
            pHarmonicsRsp->Irms_B_m = 0.0;
            pHarmonicsRsp->Irms_C_m = 0.0;
            pHarmonicsRsp->Irms_N_m = 0.0;
            pHarmonicsRsp->Vrms_A_m = 0.0;
            pHarmonicsRsp->Vrms_B_m = 0.0;
            pHarmonicsRsp->Vrms_C_m = 0.0;
        }

        pHarmonicsRsp++;
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Driver Common Interface Implementation
// *****************************************************************************
// *****************************************************************************

SYS_MODULE_OBJ DRV_METROLOGY_Initialize (SYS_MODULE_INIT * init, uint32_t resetCause)
{
    /* MISRA C-2023 deviation block start */
    /* MISRA C-2023 Rule 11.3 deviated once. Deviation record ID - H3_MISRAC_2023_R_11_3_DR_1 */
    DRV_METROLOGY_INIT *metInit = (DRV_METROLOGY_INIT *)init;
    /* MISRA C-2023 deviation block end */

    if ((gDrvMetObj.inUse == true) || (init == NULL))
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    /* Clean the IPC interrupt generic flag */
    gDrvMetObj.ipcInterruptFlag = false;

    /* Disable IPC interrupts */
    (void) SYS_INT_SourceDisable(IPC1_IRQn);
    NVIC_ClearPendingIRQ(IPC1_IRQn);

    /* Clean the IPC interrupt flags */
    gDrvMetObj.integrationFlag = false;
    gDrvMetObj.fullCycleFlag = false;

    if (resetCause != RSTC_SR_RSTTYP(RSTC_SR_RSTTYP_WDT0_RST_Val))
    {
        uint32_t *pSrc;
        uint32_t *pDst;

        /* Assert reset of the coprocessor and its peripherals */
        RSTC_CoProcessorEnable(false);
        RSTC_CoProcessorPeripheralEnable(false);

        /* Disable coprocessor Clocks */
        CLK_Core1ProcessorClkDisable();

        /* De-assert reset of the coprocessor peripherals */
        RSTC_CoProcessorPeripheralEnable(true);

        gDrvMetObj.binStartAddress = metInit->binStartAddress;
        gDrvMetObj.binSize = metInit->binEndAddress - metInit->binStartAddress;

        /* Copy the Metrology bin file to SRAM1 */
        pSrc = (uint32_t *)gDrvMetObj.binStartAddress;
        pDst = (uint32_t *)IRAM1_ADDR;
        (void) memcpy(pDst, pSrc, gDrvMetObj.binSize);
    }

    /* Initialization of the interface with Metrology Lib */
    gDrvMetObj.metRegisters = (MET_REGISTERS *)metInit->regBaseAddress;
    gDrvMetObj.inUse = true;
    gDrvMetObj.integrationCallback = NULL;

    (void) memset(&gDrvMetObj.metAccData, 0, sizeof(DRV_METROLOGY_REGS_ACCUMULATORS));
    (void) memset(&gDrvMetObj.metHarData, 0, sizeof(DRV_METROLOGY_REGS_HARMONICS));
    (void) memset(&gDrvMetObj.calibrationData, 0, sizeof(DRV_METROLOGY_CALIBRATION));
    (void) memset(&gDrvMetObj.metAFEData, 0, sizeof(DRV_METROLOGY_AFE_DATA));

    /* Initialization of the Metrology object */
    gDrvMetObj.status = DRV_METROLOGY_STATUS_HALT;

    /* Configure IPC peripheral */
    lDRV_Metrology_IpcInitialize();

    return (SYS_MODULE_OBJ)&gDrvMetObj;
}

SYS_MODULE_OBJ DRV_METROLOGY_Reinitialize (SYS_MODULE_INIT * init)
{
    /* MISRA C-2023 deviation block start */
    /* MISRA C-2023 Rule 11.3 deviated once. Deviation record ID - H3_MISRAC_2023_R_11_3_DR_1 */
    DRV_METROLOGY_INIT *metInit = (DRV_METROLOGY_INIT *)init;
    /* MISRA C-2023 deviation block end */
    uint32_t *pSrc;
    uint32_t *pDst;

    if ((gDrvMetObj.inUse == false) || (init == NULL))
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    /* Disable IPC interrupts */
    (void) SYS_INT_SourceDisable(IPC1_IRQn);

    /* Clean the IPC interrupt flags */
    gDrvMetObj.integrationFlag = false;
    gDrvMetObj.fullCycleFlag = false;

    /* Assert reset of the coprocessor and its peripherals */
    RSTC_CoProcessorEnable(false);
    RSTC_CoProcessorPeripheralEnable(false);

    /* Disable coprocessor Clocks */
    CLK_Core1ProcessorClkDisable();

    /* De-assert reset of the coprocessor peripherals */
    RSTC_CoProcessorPeripheralEnable(true);

    gDrvMetObj.binStartAddress = metInit->binStartAddress;
    gDrvMetObj.binSize = metInit->binEndAddress - metInit->binStartAddress;

    /* Copy the Metrology bin file to SRAM1 */
    pSrc = (uint32_t *)gDrvMetObj.binStartAddress;
    pDst = (uint32_t *)IRAM1_ADDR;
    (void) memcpy(pDst, pSrc, gDrvMetObj.binSize);

    /* Initialization of the interface with Metrology Lib */
    gDrvMetObj.metRegisters = (MET_REGISTERS *)metInit->regBaseAddress;

    (void) memset(&gDrvMetObj.metAccData, 0, sizeof(DRV_METROLOGY_REGS_ACCUMULATORS));
    (void) memset(&gDrvMetObj.metHarData, 0, sizeof(DRV_METROLOGY_REGS_HARMONICS));
    (void) memset(&gDrvMetObj.calibrationData, 0, sizeof(DRV_METROLOGY_CALIBRATION));
    (void) memset(&gDrvMetObj.metAFEData, 0, sizeof(DRV_METROLOGY_AFE_DATA));

    /* Initialization of the Metrology object */
    gDrvMetObj.status = DRV_METROLOGY_STATUS_HALT;

    lDRV_Metrology_IpcInitialize();

    return (SYS_MODULE_OBJ)&gDrvMetObj;
}

DRV_METROLOGY_RESULT DRV_METROLOGY_Open (DRV_METROLOGY_START_MODE mode, DRV_METROLOGY_REGS_CONTROL * pConfiguration)
{
    if (gDrvMetObj.inUse == false)
    {
        return DRV_METROLOGY_ERROR;
    }

    if (gDrvMetObj.status != DRV_METROLOGY_STATUS_HALT)
    {
        return DRV_METROLOGY_ERROR;
    }

    gDrvMetObj.status = DRV_METROLOGY_STATUS_WAITING_IPC;

    /* Enable IPC1 Interrupt Source */
    SYS_INT_SourceStatusClear(IPC1_IRQn);
    SYS_INT_SourceEnable(IPC1_IRQn);

    if (mode == DRV_METROLOGY_START_HARD)
    {
        /* Enable the coprocessor clock (Core 1) */
        CLK_Core1ProcessorClkEnable();

        /* De-assert the reset of the coprocessor (Core 1) */
        RSTC_CoProcessorEnable(true);

        /* Wait IPC Init interrupt */
        while(gDrvMetObj.status == DRV_METROLOGY_STATUS_WAITING_IPC)
        {
        }

        if (pConfiguration != NULL)
        {
            /* Ensure the Metrology Lib in reset */
            pConfiguration->STATE_CTRL = STATE_CTRL_STATE_CTRL_RESET_Val;
            /* Load external Control values */
            (void) memcpy(&gDrvMetObj.metRegisters->MET_CONTROL, pConfiguration, sizeof(DRV_METROLOGY_REGS_CONTROL));
        }
        else
        {
            /* Load default Control values */
            (void) memcpy(&gDrvMetObj.metRegisters->MET_CONTROL, &gDrvMetControlDefault, sizeof(DRV_METROLOGY_REGS_CONTROL));
        }

        /* Set Metrology Lib state as Init */
        gDrvMetObj.metRegisters->MET_CONTROL.STATE_CTRL = STATE_CTRL_STATE_CTRL_INIT_Val;

        while(gDrvMetObj.metRegisters->MET_STATUS.STATUS != STATUS_STATUS_READY)
        {
        }
    }

    gDrvMetObj.status = DRV_METROLOGY_STATUS_READY;

    return DRV_METROLOGY_SUCCESS;
}

DRV_METROLOGY_RESULT DRV_METROLOGY_Close (void)
{
    uint32_t loopCount;

    if (gDrvMetObj.inUse == false)
    {
        return DRV_METROLOGY_ERROR;
    }

    /* Disable IPC1 Interrupt Source */
    (void) SYS_INT_SourceDisable(IPC1_IRQn);
    NVIC_ClearPendingIRQ(IPC1_IRQn);

    /* Keep Metrology Lib in reset */
    gDrvMetObj.metRegisters->MET_CONTROL.STATE_CTRL = STATE_CTRL_STATE_CTRL_RESET_Val;
    /* Wait until the metrology resets */
    loopCount = 0;
    while (gDrvMetObj.metRegisters->MET_STATUS.STATUS != STATUS_STATUS_RESET)
    {
        if (++loopCount > MAX_WAIT_LOOPS)
        {
            /* Way out */
            break;
        }
    }

    /* Update Driver state */
    gDrvMetObj.status = DRV_METROLOGY_STATUS_HALT;

    return DRV_METROLOGY_SUCCESS;

}

DRV_METROLOGY_RESULT DRV_METROLOGY_Start (void)
{
    if (gDrvMetObj.metRegisters->MET_STATUS.STATUS == STATUS_STATUS_READY)
    {
        /* Set Metrology Lib state as Run */
        gDrvMetObj.metRegisters->MET_CONTROL.STATE_CTRL = STATE_CTRL_STATE_CTRL_RUN_Val;
        /* Wait until the metrology running */
        while (gDrvMetObj.metRegisters->MET_STATUS.STATUS != STATUS_STATUS_DSP_RUNNING)
        {
        }
        gDrvMetObj.status = DRV_METROLOGY_STATUS_RUNNING;

        return DRV_METROLOGY_SUCCESS;
    }

    return DRV_METROLOGY_ERROR;
}

DRV_METROLOGY_STATUS DRV_METROLOGY_GetStatus(void)
{
    return gDrvMetObj.status;
}

DRV_METROLOGY_RESULT DRV_METROLOGY_IntegrationCallbackRegister (
    DRV_METROLOGY_CALLBACK callback
)
{
    if (callback == NULL)
    {
        return DRV_METROLOGY_ERROR;
    }

    gDrvMetObj.integrationCallback = callback;
    return DRV_METROLOGY_SUCCESS;
}

DRV_METROLOGY_RESULT DRV_METROLOGY_FullCycleCallbackRegister(DRV_METROLOGY_CALLBACK callback)
{
    if (callback == NULL)
    {
        return DRV_METROLOGY_ERROR;
    }

    gDrvMetObj.fullCycleCallback = callback;
    return DRV_METROLOGY_SUCCESS;
}

DRV_METROLOGY_RESULT DRV_METROLOGY_HalfCycleCallbackRegister(DRV_METROLOGY_CALLBACK callback)
{
    if (callback == NULL)
    {
        return DRV_METROLOGY_ERROR;
    }

    gDrvMetObj.halfCycleCallback = callback;
    return DRV_METROLOGY_SUCCESS;
}

DRV_METROLOGY_RESULT DRV_METROLOGY_CalibrationCallbackRegister (
    DRV_METROLOGY_CALIBRATION_CALLBACK callback
)
{
    if (callback == NULL)
    {
        return DRV_METROLOGY_ERROR;
    }

    gDrvMetObj.calibrationCallback = callback;
    return DRV_METROLOGY_SUCCESS;
}

DRV_METROLOGY_RESULT DRV_METROLOGY_HarmonicAnalysisCallbackRegister (
    DRV_METROLOGY_HARMONICS_CALLBACK callback
)
{
    if (callback == NULL)
    {
        return DRV_METROLOGY_ERROR;
    }

    gDrvMetObj.harmonicAnalysisCallback = callback;
    return DRV_METROLOGY_SUCCESS;
}

void DRV_METROLOGY_Tasks(SYS_MODULE_OBJ object)
{
    if (object == SYS_MODULE_OBJ_INVALID)
    {
        /* Invalid system object */
        return;
    }

    if (gDrvMetObj.ipcInterruptFlag == false)
    {
        /* There are not IPC interrupts */
        return;
    }

    /* Clear IPC interrupt flag */
    gDrvMetObj.ipcInterruptFlag = false;

    if (gDrvMetObj.fullCycleFlag == true)
    {
        gDrvMetObj.fullCycleFlag = false;

        /* Update cycle measurements from metrology library registers */
        lDRV_METROLOGY_UpdateCycleMeasurements();

        if (gDrvMetObj.fullCycleCallback != NULL)
        {
            gDrvMetObj.fullCycleCallback();
        }
    }

    if (gDrvMetObj.integrationFlag == true)
    {
        gDrvMetObj.integrationFlag = false;

        if (gDrvMetObj.harmonicAnalysisData.integrationPeriods > 0U)
        {
            gDrvMetObj.harmonicAnalysisData.integrationPeriods--;
        }

        /* Check if there is a calibration process running */
        if (gDrvMetObj.calibrationData.running == true)
        {
            if (lDRV_METROLOGY_UpdateCalibrationValues() == true)
            {
                /* Launch calibration callback */
                if (gDrvMetObj.calibrationCallback != NULL)
                {
                    gDrvMetObj.calibrationCallback(gDrvMetObj.calibrationData.result);
                }
            }
        }
        else
        {
            /* Update measurements from metrology library registers */
            lDRV_METROLOGY_UpdateMeasurements();

            /* Launch integration callback */
            if (gDrvMetObj.integrationCallback != NULL)
            {
                gDrvMetObj.integrationCallback();
            }

            /* Check if there is a harmonic analysis process running */
            if ((gDrvMetObj.harmonicAnalysisData.running == true) &&
                (gDrvMetObj.harmonicAnalysisData.integrationPeriods == 0U))
            {
                /* Prevent updating of harmonic registers */
                gDrvMetObj.harmonicAnalysisData.holdRegs = true;

                lDRV_METROLOGY_UpdateHarmonicAnalysisValues();
                /* Launch Harmonic Analysis callback */
                if (gDrvMetObj.harmonicAnalysisCallback != NULL)
                {
                    gDrvMetObj.harmonicAnalysisCallback(gDrvMetObj.harmonicAnalysisData.harmonicBitmap);
                }

                /* Reset harmonic registers update flag */
                gDrvMetObj.harmonicAnalysisData.holdRegs = false;
            }
        }
    }
}

DRV_METROLOGY_AFE_TYPE DRV_METROLOGY_GetAFEDescription (char *pDescription)
{
    if (pDescription != NULL)
    {
        (void) memcpy(pDescription, gDrvAFEDescription, sizeof(gDrvAFEDescription));
        *(pDescription + (sizeof(gDrvAFEDescription) + 1U)) = '\0';
    }
    return (DRV_METROLOGY_AFE_TYPE)DRV_METROLOGY_AFE_SELECTION;
}

DRV_METROLOGY_CHANNEL * DRV_METROLOGY_GetChannelDescription (void)
{
    return (DRV_METROLOGY_CHANNEL * )&gDrvMetChannelsDefault;
}

DRV_METROLOGY_REGS_STATUS * DRV_METROLOGY_GetStatusData (void)
{
    return &gDrvMetObj.metRegisters->MET_STATUS;
}

DRV_METROLOGY_REGS_CONTROL * DRV_METROLOGY_GetControlData (void)
{
    return &gDrvMetObj.metRegisters->MET_CONTROL;
}

DRV_METROLOGY_REGS_CONTROL * DRV_METROLOGY_GetControlByDefault (void)
{
    /* MISRA C-2023 Rule 11.8 deviated below. Deviation record ID -
      H3_MISRAC_2023_R_11_8_DR_1*/
    return (DRV_METROLOGY_REGS_CONTROL *)&gDrvMetControlDefault;
   /* MISRAC 2012 deviation block end */
}

DRV_METROLOGY_REGS_ACCUMULATORS * DRV_METROLOGY_GetAccData (void)
{
    return &gDrvMetObj.metAccData;
}

DRV_METROLOGY_REGS_PERCYCLE_ACC * DRV_METROLOGY_GetPerCycleAccData (void)
{
    return &gDrvMetObj.metPerCycleAccData;
}

DRV_METROLOGY_REGS_HARMONICS * DRV_METROLOGY_GetHarData (void)
{
    return &gDrvMetObj.metHarData;
}

DRV_METROLOGY_CALIBRATION_REFS * DRV_METROLOGY_GetCalibrationReferences (void)
{
    return &gDrvMetObj.calibrationData.references;
}

void DRV_METROLOGY_SetControl (DRV_METROLOGY_REGS_CONTROL * pControl)
{
    /* MISRA C-2023 Rule 11.8 deviated below. Deviation record ID -
      H3_MISRAC_2023_R_11_8_DR_1*/
    /* Keep State Control Register value */
    (void) memcpy((void *)&gDrvMetObj.metRegisters->MET_CONTROL.FEATURE_CTRL,
                  (void *)&pControl->FEATURE_CTRL,
                  sizeof(DRV_METROLOGY_REGS_CONTROL) - sizeof(uint32_t));
   /* MISRAC 2012 deviation block end */
}

float DRV_METROLOGY_GetEnergyValue(bool restartEnergy)
{
    float energy = gDrvMetObj.metAFEData.energy;

    if (restartEnergy == true)
    {
        gDrvMetObj.metAFEData.energy = 0.0f;
    }

    return energy;
}

float DRV_METROLOGY_GetMeasureValue(DRV_METROLOGY_MEASURE_TYPE type, bool perCycle)
{
    float value;

    if (type < MEASURE_TYPE_NUM)
    {
        if (perCycle == false)
        {
            value = gDrvMetObj.metAFEData.measure[type];
        }
        else
        {
            value = gDrvMetObj.metAFEData.cycleMeasure[type];
        }

        if (type >= MEASURE_ANGLEA)
        {
            /* Absolute value should be between 0 and 180 degrees */
            while (true)
            {
                if (value < -180.0f)
                {
                    value += 360.0f;
                }
                else if (value > 180.0f)
                {
                    value -= 360.0f;
                }
                else
                {
                    break;
                }
            }
        }
    }
    else
    {
        value = 0.0f;
    }

    return value;
}

void DRV_METROLOGY_GetEventsData(DRV_METROLOGY_AFE_EVENTS_UNION * events)
{
    *events = (DRV_METROLOGY_AFE_EVENTS_UNION) gDrvMetObj.metAFEData.afeEvents;
}

void DRV_METROLOGY_StartCalibration(void)
{
    DRV_METROLOGY_CALIBRATION * pCalibrationData = &gDrvMetObj.calibrationData;

    if (pCalibrationData->running == false)
    {
        uint32_t featureCtrlNew;
        DRV_METROLOGY_CALIBRATION_MASK calMask;
        DRV_METROLOGY_REGS_CONTROL * pMetControlRegs = &gDrvMetObj.metRegisters->MET_CONTROL;

        /* Initialize CAL registers to default value before calibration */
        calMask = pCalibrationData->references.calMask;
        if (calMask.magnitudeIa != 0U)
        {
            pMetControlRegs->CAL_M_IA = 0x20000000;
        }
        if (calMask.magnitudeVa != 0U)
        {
            pMetControlRegs->CAL_M_VA = 0x20000000;
        }
        if (calMask.magnitudeIb != 0U)
        {
            pMetControlRegs->CAL_M_IB = 0x20000000;
        }
        if (calMask.magnitudeVb != 0U)
        {
            pMetControlRegs->CAL_M_VB = 0x20000000;
        }
        if (calMask.magnitudeIc != 0U)
        {
            pMetControlRegs->CAL_M_IC = 0x20000000;
        }
        if (calMask.magnitudeVc != 0U)
        {
            pMetControlRegs->CAL_M_VC = 0x20000000;
        }
        if (calMask.magnitudeIn != 0U)
        {
            pMetControlRegs->CAL_M_IN = 0x20000000;
        }
        if (calMask.magnitudeVc != 0U)
        {
            pMetControlRegs->CAL_M_VD = 0x20000000;
        }
        pMetControlRegs->CAL_PH_VA = 0;
        if (calMask.anglePhaseA != 0U)
        {
            pMetControlRegs->CAL_PH_IA = 0;
        }
        if (calMask.anglePhaseB != 0U)
        {
            pMetControlRegs->CAL_PH_IB = 0;
        }
        if (calMask.anglePhaseC != 0U)
        {
            pMetControlRegs->CAL_PH_IC = 0;
        }
        if (calMask.anglePhaseN != 0U)
        {
            pMetControlRegs->CAL_PH_IN = 0;
        }
        if (calMask.angleVAB != 0U)
        {
            pMetControlRegs->CAL_PH_VB = 0;
        }
        if (calMask.angleVAC != 0U)
        {
            pMetControlRegs->CAL_PH_VC = 0;
        }

        /* Set the number of integration periods to wait before starting the calibration process */
        pCalibrationData->numPreIntegrationPeriods = CAL_NUM_PRE_INTEGRATION_PERIODS;

        /* Set the number of integration periods to improve the accuracy of the calibration */
        pCalibrationData->numIntegrationPeriods = CAL_NUM_INTEGRATION_PERIODS;

        /* Initialize Accumulators */
        pCalibrationData->dspAccIa = 0U;
        pCalibrationData->dspAccIb = 0U;
        pCalibrationData->dspAccIc = 0U;
        pCalibrationData->dspAccIn = 0U;
        pCalibrationData->dspAccUa = 0U;
        pCalibrationData->dspAccUb = 0U;
        pCalibrationData->dspAccUc = 0U;
        pCalibrationData->dspAccUd = 0U;
        pCalibrationData->dspAccUaf = 0U;
        pCalibrationData->dspAccUbf = 0U;
        pCalibrationData->dspAccUcf = 0U;
        pCalibrationData->dspAccUdf = 0U;
        pCalibrationData->dspAccUabf = 0U;
        pCalibrationData->dspAccUcaf = 0U;
        pCalibrationData->dspAccPa = 0;
        pCalibrationData->dspAccPb = 0;
        pCalibrationData->dspAccPc = 0;
        pCalibrationData->dspAccPn = 0;
        pCalibrationData->dspAccQa = 0;
        pCalibrationData->dspAccQb = 0;
        pCalibrationData->dspAccQc = 0;
        pCalibrationData->dspAccQn = 0;

        /* Save FEATURE_CTRL register value, to be restored after calibration */
        pCalibrationData->featureCtrlBackup = pMetControlRegs->FEATURE_CTRL;
        featureCtrlNew = pMetControlRegs->FEATURE_CTRL;

        if (calMask.anglePhaseN != 0U)
        {
            /* Select dominant voltage channel */
            featureCtrlNew &= ~(FEATURE_CTRL_SYNCH_Msk);
            featureCtrlNew |= FEATURE_CTRL_SYNCH(calMask.vRefPhaseN);
        }

        /* Enable Neutral current channel if it is going to be calibrated */
        if ((calMask.magnitudeIn != 0U) || (calMask.anglePhaseN != 0U))
        {
            featureCtrlNew &= ~FEATURE_CTRL_NEUTRAL_DIS_Msk;
        }

        /* Enable DC measurements in VD channel if it is going to be calibrated */
        if (calMask.magnitudeVd != 0U)
        {
            featureCtrlNew |= FEATURE_CTRL_VD_DC_EN_Msk;
        }

        pMetControlRegs->FEATURE_CTRL = featureCtrlNew;
        pCalibrationData->running = true;
        pCalibrationData->result = false;
    }
}

bool DRV_METROLOGY_StartHarmonicAnalysis(uint32_t harmonicBitmap, DRV_METROLOGY_HARMONICS_RMS *pHarmonicResponse)
{
    if (harmonicBitmap > HARMONIC_CTRL_HARMONIC_m_REQ_Msk)
    {
        /* Requested harmonics out of bounds */
        return false;
    }
    if (gDrvMetObj.harmonicAnalysisData.running)
    {
        if (gDrvMetObj.harmonicAnalysisData.integrationPeriods == 0U)
        {
            gDrvMetObj.harmonicAnalysisData.integrationPeriods = 1U;
        }
        else
        {
            return false;
        }
    }
    else
    {
        gDrvMetObj.harmonicAnalysisData.running = true;
        gDrvMetObj.harmonicAnalysisData.integrationPeriods = 2U;
        gDrvMetObj.harmonicAnalysisData.harmonicBitmap = harmonicBitmap;

        /* Set Number of Harmonic for Analysis */
        gDrvMetObj.metRegisters->MET_CONTROL.HARMONIC_CTRL = 0U;
        gDrvMetObj.metRegisters->MET_CONTROL.HARMONIC_CTRL = HARMONIC_CTRL_HARMONIC_m_REQ(harmonicBitmap);
        /* Enable Harmonic Analysis */
        gDrvMetObj.metRegisters->MET_CONTROL.HARMONIC_CTRL |= HARMONIC_CTRL_HARMONIC_EN_Msk;
    }

    /* Set Data pointer to store the Harmonic data result */
    gDrvMetObj.harmonicAnalysisData.pHarmonicAnalysisResponse = pHarmonicResponse;

    return true;
}

void DRV_METROLOGY_StopHarmonicAnalysis(void)
{
    /* Clear flags and disable in registers */
    gDrvMetObj.harmonicAnalysisData.running = false;
    gDrvMetObj.metRegisters->MET_CONTROL.HARMONIC_CTRL &=
        (HARMONIC_CTRL_HARMONIC_EN(HARMONIC_CTRL_HARMONIC_EN_DISABLED_Val) |
        HARMONIC_CTRL_HARMONIC_m_REQ_Msk);
}

void DRV_METROLOGY_CaptureSetOptions(DRV_METROLOGY_CAPTURE_SOURCE source, DRV_METROLOGY_CAPTURE_TYPE type)
{
    uint32_t reg;

    reg = gDrvMetObj.metRegisters->MET_CONTROL.CAPTURE_CTRL;

    if ((reg & CAPTURE_CTRL_CAPTURE_EN_Msk) == 0UL)
    {
        reg &= ~(CAPTURE_CTRL_CAPTURE_SOURCE_Msk | CAPTURE_CTRL_CAPTURE_TYPE_Msk);
        reg |= (CAPTURE_CTRL_CAPTURE_SOURCE(source) | CAPTURE_CTRL_CAPTURE_TYPE(type));
        gDrvMetObj.metRegisters->MET_CONTROL.CAPTURE_CTRL = reg;
    }
}

void DRV_METROLOGY_CaptureEnableChannels(uint8_t channelMask)
{
    uint32_t reg;
    uint32_t mask;

    reg = gDrvMetObj.metRegisters->MET_CONTROL.CAPTURE_CTRL;

    if ((reg & CAPTURE_CTRL_CAPTURE_EN_Msk) == 0UL)
    {
        mask = (uint32_t)channelMask << CAPTURE_CTRL_CH_SEL_IA_Pos;
        reg |= mask;
        gDrvMetObj.metRegisters->MET_CONTROL.CAPTURE_CTRL = reg;
    }
}

void DRV_METROLOGY_CaptureDisableChannels(uint8_t channelMask)
{
    uint32_t reg;
    uint32_t mask;

    reg = gDrvMetObj.metRegisters->MET_CONTROL.CAPTURE_CTRL;

    if ((reg & CAPTURE_CTRL_CAPTURE_EN_Msk) == 0UL)
    {
        mask = (uint32_t)channelMask << CAPTURE_CTRL_CH_SEL_IA_Pos;
        reg &= ~mask;
        gDrvMetObj.metRegisters->MET_CONTROL.CAPTURE_CTRL = reg;
    }
}

bool DRV_METROLOGY_CaptureStart(uint32_t *pData, uint32_t samplesNum)
{
    uint32_t reg;

    reg = gDrvMetObj.metRegisters->MET_CONTROL.CAPTURE_CTRL;

    if ((reg & CAPTURE_CTRL_CAPTURE_EN_Msk) > 0UL)
    {
        return false;
    }

    if (samplesNum == 0UL)
    {
        return false;
    }

    if (((uint32_t)pData < 0x20000000UL) || ((uint32_t)pData >= 0x20008000UL))
    {
        return false;
    }

    gDrvMetObj.metRegisters->MET_CONTROL.CAPTURE_ADDR = (uint32_t)pData;
    gDrvMetObj.metRegisters->MET_CONTROL.CAPTURE_BUFF_SIZE = samplesNum;

    reg |= CAPTURE_CTRL_CAPTURE_EN_Msk;
    gDrvMetObj.metRegisters->MET_CONTROL.CAPTURE_CTRL = reg;

    return true;
}

void DRV_METROLOGY_CaptureStop(void)
{
    uint32_t reg;

    reg = gDrvMetObj.metRegisters->MET_STATUS.CAPTURE_STATUS;
    if ((reg & CAPTURE_STATUS_CAPTURE_STATE_Msk) > 0UL)
    {
        reg = gDrvMetObj.metRegisters->MET_CONTROL.CAPTURE_CTRL;
        reg &= ~(CAPTURE_CTRL_CAPTURE_EN_Msk |
                 CAPTURE_CTRL_CH_SEL_IA_Msk | CAPTURE_CTRL_CH_SEL_VA_Msk |
                 CAPTURE_CTRL_CH_SEL_IB_Msk | CAPTURE_CTRL_CH_SEL_VB_Msk |
                 CAPTURE_CTRL_CH_SEL_IC_Msk | CAPTURE_CTRL_CH_SEL_VC_Msk |
                 CAPTURE_CTRL_CH_SEL_IN_Msk | CAPTURE_CTRL_CH_SEL_VD_Msk);

        gDrvMetObj.metRegisters->MET_CONTROL.CAPTURE_CTRL = reg;

        gDrvMetObj.metRegisters->MET_CONTROL.CAPTURE_ADDR = 0UL;
        gDrvMetObj.metRegisters->MET_CONTROL.CAPTURE_BUFF_SIZE = 0UL;
    }

    /* Wait to disable */
    reg = gDrvMetObj.metRegisters->MET_STATUS.CAPTURE_STATUS;
    while ((reg & CAPTURE_STATUS_CAPTURE_STATE_Msk) > 0UL)
    {
        reg = gDrvMetObj.metRegisters->MET_STATUS.CAPTURE_STATUS;
    }
}

DRV_METROLOGY_CAPTURE_STATE DRV_METROLOGY_CaptureGetState(void)
{
    uint32_t reg;

    reg = gDrvMetObj.metRegisters->MET_STATUS.CAPTURE_STATUS;
    reg &= CAPTURE_STATUS_CAPTURE_STATE_Msk;
    reg >>= CAPTURE_STATUS_CAPTURE_STATE_Pos;

    return (DRV_METROLOGY_CAPTURE_STATE)reg;
}

uint32_t DRV_METROLOGY_CaptureGetOffset(void)
{
    uint32_t reg;

    reg = gDrvMetObj.metRegisters->MET_STATUS.CAPTURE_STATUS;

    return (uint32_t)(reg & CAPTURE_STATUS_CAPTURE_OFFSET_Msk);
}

bool DRV_METROLOGY_CaptureCheckIsWrap(void)
{
    uint32_t reg;

    reg = gDrvMetObj.metRegisters->MET_STATUS.CAPTURE_STATUS;

    if ((reg & CAPTURE_STATUS_CAPTURE_WRAP_Msk) > 0UL)
    {
        return true;
    }
    else
    {
        return false;
    }
}

