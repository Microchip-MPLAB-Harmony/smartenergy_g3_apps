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
/*******************************************************************************
* Copyright (C) 2023 Microchip Technology Inc. and its subsidiaries.
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
#include "interrupts.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data
// *****************************************************************************
// *****************************************************************************

typedef enum {
    PENERGY = 0U,
    QENERGY = 1U,
} DRV_METROLOGY_ENERGY_TYPE;

/* This is the driver instance object array. */
static DRV_METROLOGY_OBJ gDrvMetObj;

static CACHE_ALIGN uint32_t sCaptureBuffer[CACHE_ALIGNED_SIZE_GET(MET_CAPTURE_BUF_SIZE)];

static const DRV_METROLOGY_REGS_CONTROL gDrvMetControlDefault =
{
    STATE_CTRL_STATE_CTRL_RESET_Val,                  /* 00 STATE_CTRL */
    (uint32_t)(DRV_METROLOGY_CONF_FCTRL0),            /* 01 FEATURE_CTRL0 */
    (uint32_t)(DRV_METROLOGY_CONF_FCTRL1),            /* 02 FEATURE_CTRL1 */
    (uint32_t)(DRV_METROLOGY_CONF_MT),                /* 03 METER_TYPE sensor_type =0 CT, 1 SHUNT, 2 ROGOWSKI */
    (uint32_t)(0x00000000UL),                         /* 04 M M=50->50Hz M=60->60Hz */
    (uint32_t)(0x00001130UL),                         /* 05 N_MAX 4400=0x1130 */
    (uint32_t)(DRV_METROLOGY_CONF_PULSE0_CTRL),       /* 06 PULSE0_CTRL */
    (uint32_t)(DRV_METROLOGY_CONF_PULSE1_CTRL),       /* 07 PULSE1_CTRL */
    (uint32_t)(DRV_METROLOGY_CONF_PULSE2_CTRL),       /* 08 PULSE2_CTRL */
    (uint32_t)(DRV_METROLOGY_CONF_PKT),               /* 09 P_K_T */
    (uint32_t)(DRV_METROLOGY_CONF_PKT),               /* 10 Q_K_T */
    (uint32_t)(DRV_METROLOGY_CONF_PKT),               /* 11 I_K_T */
    (uint32_t)(DRV_METROLOGY_CONF_CREEP_P),           /* 12 CREEP_THR_P */
    (uint32_t)(DRV_METROLOGY_CONF_CREEP_Q),           /* 13 CREEP_THR_Q */
    (uint32_t)(DRV_METROLOGY_CONF_CREEP_I),           /* 14 CREEP_THR_I */
    (uint32_t)(0x00000000UL),                         /* 15 POWER_OFFSET_CTRL */
    (uint32_t)(0x00000000UL),                         /* 16 POWER_OFFSET_P */
    (uint32_t)(0x00000000UL),                         /* 17 POWER_OFFSET_Q */
    (uint32_t)(DRV_METROLOGY_CONF_SWELL),             /* 18 SWELL_THR_VA */
    (uint32_t)(DRV_METROLOGY_CONF_SWELL),             /* 19 SWELL_THR_VB */
    (uint32_t)(DRV_METROLOGY_CONF_SWELL),             /* 20 SWELL_THR_VC */
    (uint32_t)(DRV_METROLOGY_CONF_SAG),               /* 21 SAG_THR_VA */
    (uint32_t)(DRV_METROLOGY_CONF_SAG),               /* 22 SAG_THR_VB */
    (uint32_t)(DRV_METROLOGY_CONF_SAG),               /* 23 SAG_THR_VC */
    (uint32_t)(DRV_METROLOGY_CONF_KI),                /* 24 K_IA */
    (uint32_t)(DRV_METROLOGY_CONF_KV),                /* 25 K_VA */
    (uint32_t)(DRV_METROLOGY_CONF_KI),                /* 26 K_IB */
    (uint32_t)(DRV_METROLOGY_CONF_KV),                /* 27 K_VB */
    (uint32_t)(DRV_METROLOGY_CONF_KI),                /* 28 K_IC */
    (uint32_t)(DRV_METROLOGY_CONF_KV),                /* 29 K_VC */
    (uint32_t)(DRV_METROLOGY_CONF_KI),                /* 30 K_IN */
    (uint32_t)(0x20000000UL),                         /* 31 CAL_M_IA */
    (uint32_t)(0x20000000UL),                         /* 32 CAL_M_VA */
    (uint32_t)(0x20000000UL),                         /* 33 CAL_M_IB */
    (uint32_t)(0x20000000UL),                         /* 34 CAL_M_VB */
    (uint32_t)(0x20000000UL),                         /* 35 CAL_M_IC */
    (uint32_t)(0x20000000UL),                         /* 36 CAL_M_VC */
    (uint32_t)(0x20000000UL),                         /* 37 CAL_M_IN */
    (uint32_t)(0x00000000UL),                         /* 38 CAL_PH_IA */
    (uint32_t)(0x00000000UL),                         /* 39 CAL_PH_VA */
    (uint32_t)(0x00000000UL),                         /* 40 CAL_PH_IB */
    (uint32_t)(0x00000000UL),                         /* 41 CAL_PH_VB */
    (uint32_t)(0x00000000UL),                         /* 42 CAL_PH_IC */
    (uint32_t)(0x00000000UL),                         /* 43 CAL_PH_VC */
    (uint32_t)(0x00000000UL),                         /* 44 CAL_PH_IN */
    (uint32_t)(DRV_METROLOGY_CONF_WAVEFORM),          /* 45 CAPTURE_CTRL */
    (uint32_t)(DRV_METROLOGY_CAPTURE_BUF_SIZE),       /* 46 CAPTURE_BUFF_SIZE */
    (uint32_t)(sCaptureBuffer),                       /* 47 CAPTURE_ADDR */
    (uint32_t)(0x00000000UL),                         /* 48 RESERVED_C48 */
    (uint32_t)(0x00000000UL),                         /* 49 RESERVED_C49 */
    (uint32_t)(0x00000000UL),                         /* 50 RESERVED_C50 */
    (uint32_t)(DRV_METROLOGY_CONF_ATS2023),           /* 51 ATSENSE_CTRL_20_23 */
    (uint32_t)(DRV_METROLOGY_CONF_ATS2427),           /* 52 ATSENSE_CTRL_24_27 */
    (uint32_t)(0x00000003UL),                         /* 53 ATSENSE_CTRL_28_2B: MSB_MODE=0,OSR=3 */
    (uint32_t)(0x00000000UL),                         /* 54 RESERVED_C54 */
    (uint32_t)(0x00000000UL),                         /* 55 POWER_OFFSET_P_A */
    (uint32_t)(0x00000000UL),                         /* 56 POWER_OFFSET_P_B */
    (uint32_t)(0x00000000UL),                         /* 57 POWER_OFFSET_P_C */
    (uint32_t)(0x00000000UL),                         /* 58 POWER_OFFSET_Q_A */
    (uint32_t)(0x00000000UL),                         /* 59 POWER_OFFSET_Q_B */
    (uint32_t)(0x00000000UL)                          /* 60 POWER_OFFSET_Q_C */
};

// *****************************************************************************
// *****************************************************************************
// Section: File scope functions
// *****************************************************************************
// *****************************************************************************

static double lDRV_Metrology_GetDouble(int64_t value)
{
    if (value < 0)
    {
        value = -value;
    }

    return (double)value;
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
            /* Update Harmonics Data */
            (void) memcpy(&gDrvMetObj.metHarData, &gDrvMetObj.metRegisters->MET_HARMONICS, sizeof(DRV_METROLOGY_REGS_HARMONICS));
        }

        gDrvMetObj.integrationFlag = true;
    }

    IPC1_REGS->IPC_ICCR = status;
    
    gDrvMetObj.ipcInterruptFlag = true;

}

static double lDRV_Metrology_GetHarmonicRMS(int32_t real, int32_t imag)
{
    double res, dre, dim;
    uint32_t measure, intPart, decPart;
    
    /* Get Real contribution */
    if (real < 0)
    {
        measure = (uint32_t)-real;
    }
    else
    {
        measure = (uint32_t)real;
    }
    
    /* sQ25.6 */
    intPart = measure >> 6;
    decPart = measure & 63U;
    dre = (double)decPart / 64.0;
    dre += (double)intPart;
    dre *= dre;
    
    /* Get Imaginary contribution */
    if (imag < 0)
    {
        measure = (uint32_t)-imag;
    }
    else
    {
        measure = (uint32_t)imag;
    }
    
    /* sQ25.6 */
    intPart = measure >> 6;
    decPart = measure & 63U;
    dim = (double)decPart / 64.0;
    dim += (double)intPart;
    dim *= dim;
    
    res = (dre + dim) * 2.0;
    if (res > 0.0)
    {
        res = sqrt(res);
        res /= (double)gDrvMetObj.metRegisters->MET_STATUS.N;
    }
    
    return res;
}

static uint32_t lDRV_Metrology_GetVIRMS(uint64_t val, uint32_t k_x)
{
    double m;

    m = (double)val;
    m = (m / (double)RMS_DIV_Q);
    m = (m / (double)gDrvMetObj.metRegisters->MET_STATUS.N);
    if (m < 0.0)
    {
        m = 0.0;
    }
    else
    {
        m = sqrt(m);
        m = m * (double)k_x / (double)RMS_DIV_G;
        m = m * (double)10000;
    }

    return ((uint32_t)(m));
}

static uint32_t lDRV_Metrology_GetInxRMS(uint64_t val)
{
    double m;

    m = (double)val;
    m = (m / (double)RMS_DIV_Inx_Q);
    m = (m / (double)gDrvMetObj.metRegisters->MET_STATUS.N);
    if (m < 0.0)
    {
        m = 0.0;
    }
    else
    {
        m = sqrt(m);
        m = m * (double)10000U;
    }

    return ((uint32_t)(m));
}

static uint32_t lDRV_Metrology_GetPQRMS(int64_t val, uint32_t k_ix, uint32_t k_vx)
{
    double m;
    double divisor, mult;

    m = lDRV_Metrology_GetDouble(val);

    m = m / (double)RMS_DIV_Q;
    m = m / (double)gDrvMetObj.metRegisters->MET_STATUS.N;
    mult = (double)k_ix * (double)k_vx;
    divisor = (double)RMS_DIV_G * (double)RMS_DIV_G;
    m = (m * mult) / divisor;
    m = m * 10.0;

    return ((uint32_t)(m));
}

static unsigned char lDRV_Metrology_CheckPQDir(int64_t val)
{
    if (val < 0)
    {
        return 1U;
    }
    else
    {
        return 0U;
    }
}

static uint32_t lDRV_Metrology_GetSRMS(int64_t pv, int64_t qv, uint32_t k_ix, uint32_t k_vx)
{
    double m, n;
    double divisor, mult;

    m = lDRV_Metrology_GetDouble(pv);

    m = m / (double)RMS_DIV_Q;
    m = m / (double)gDrvMetObj.metRegisters->MET_STATUS.N;
    mult = (double)k_ix * (double)k_vx;
    divisor = (double)RMS_DIV_G * (double)RMS_DIV_G;
    m = (m * mult) / divisor;
    m = m * 10.0;

    n = lDRV_Metrology_GetDouble(qv);

    n = n / (double)RMS_DIV_Q;
    n = n / (double)gDrvMetObj.metRegisters->MET_STATUS.N;
    mult = (double)k_ix * (double)k_vx;
    divisor = (double)RMS_DIV_G * (double)RMS_DIV_G;
    n = (n * mult) / divisor;
    n = n * 10.0;

    m = m * m;
    n = n * n;
    m = sqrt(m + n);

    return ((uint32_t)(m));
}

static uint32_t lDRV_Metrology_GetAngleRMS(int64_t p, int64_t q)
{
    double m, pd, qd;
    int32_t n;
    uint32_t angle;

    pd = lDRV_Metrology_GetDouble(p);
    qd = lDRV_Metrology_GetDouble(q);
    m = atan2(qd, pd);
    m = 180.0 * m;
    m = m * 100000.0;
    m = m / CONST_Pi;
    n = (int32_t)m;

    if (n < 0)
    {
        /* Get the positive value and set the MSB */
        n = -n;
        angle = (uint32_t)n;
        angle |= 0x80000000UL;
    }
    else
    {
        angle = (uint32_t)n;
    }

    return angle;
}

static uint32_t lDRV_Metrology_GetPQEnergy(DRV_METROLOGY_ENERGY_TYPE id)
{
    double m, k;
    double divisor;
    double ki, kv;

    divisor = (double)RMS_DIV_G * (double)RMS_DIV_G;

    /* Calculated as absolute values */
    if (id == PENERGY)
    {
        m = lDRV_Metrology_GetDouble(gDrvMetObj.metAccData.P_A);
        ki = (double)gDrvMetObj.metRegisters->MET_CONTROL.K_IA;
        kv = (double)gDrvMetObj.metRegisters->MET_CONTROL.K_VA;
        m = (m * ki * kv) / divisor;    /* m =m*k_v*k_i */
        m = m / (double)RMS_DIV_Q;      /* k =k/2^40 */
        k = m / 4000.0;                 /* k =k/fs */

        m = lDRV_Metrology_GetDouble(gDrvMetObj.metAccData.P_B);
        ki = (double)gDrvMetObj.metRegisters->MET_CONTROL.K_IB;
        kv = (double)gDrvMetObj.metRegisters->MET_CONTROL.K_VB;
        m = (m * ki * kv) / divisor;    /* m =m*k_v*k_i */
        m = m / (double)RMS_DIV_Q;      /* k =k/2^40 */
        k += m / 4000.0;                 /* k =k/fs */

        m = lDRV_Metrology_GetDouble(gDrvMetObj.metAccData.P_C);
        ki = (double)gDrvMetObj.metRegisters->MET_CONTROL.K_IC;
        kv = (double)gDrvMetObj.metRegisters->MET_CONTROL.K_VC;
        m = (m * ki * kv) / divisor;    /* m =m*k_v*k_i */
        m = m / (double)RMS_DIV_Q;      /* k =k/2^40 */
        k += m / 4000.0;                 /* k =k/fs */
    }
    else
    {
        /* reactive energy */
        m = lDRV_Metrology_GetDouble(gDrvMetObj.metAccData.Q_A);
        ki = (double)gDrvMetObj.metRegisters->MET_CONTROL.K_IA;
        kv = (double)gDrvMetObj.metRegisters->MET_CONTROL.K_VA;
        m = (m * ki * kv) / divisor;    /* m =m*k_v*k_i */
        m = m / (double)RMS_DIV_Q;      /* k =k/2^40 */
        k = m / 4000.0;                 /* k =k/fs */

        m = lDRV_Metrology_GetDouble(gDrvMetObj.metAccData.Q_B);
        ki = (double)gDrvMetObj.metRegisters->MET_CONTROL.K_IB;
        kv = (double)gDrvMetObj.metRegisters->MET_CONTROL.K_VB;
        m = (m * ki * kv) / divisor;    /* m =m*k_v*k_i */
        m = m / (double)RMS_DIV_Q;      /* k =k/2^40 */
        k += m / 4000.0;                 /* k =k/fs */

        m = lDRV_Metrology_GetDouble(gDrvMetObj.metAccData.Q_C);
        ki = (double)gDrvMetObj.metRegisters->MET_CONTROL.K_IC;
        kv = (double)gDrvMetObj.metRegisters->MET_CONTROL.K_VC;
        m = (m * ki * kv) / divisor;    /* m =m*k_v*k_i */
        m = m / (double)RMS_DIV_Q;      /* k =k/2^40 */
        k += m / 4000.0;                 /* k =k/fs */
    }

    k = k / 3600.0;         /* xxxxxx (Wh/Varh) */
    k = k * 10000.0;        /* *10000 (kWh/kVarh) */

    return ((uint32_t)(k));  /* xxxx (kWh/kVarh) */
}

static void lDRV_Metrology_IpcInitialize (void)
{
    /* Clear interrupts */
    IPC1_REGS->IPC_ICCR = 0xFFFFFFFFUL;
    /* Enable interrupts */
    IPC1_REGS->IPC_IECR = DRV_METROLOGY_IPC_INIT_IRQ_MSK | 
        DRV_METROLOGY_IPC_INTEGRATION_IRQ_MSK;
}

static uint32_t lDRV_Metrology_CorrectCalibrationAngle (uint32_t measured, double reference)
{
    double bams, phase_correction;
    int64_t correction_angle;

    if ((measured & 0x80000000UL) != 0UL)
    {
        uint32_t measured_angle;
        measured_angle = 0x7FFFFFFFUL & measured;
        correction_angle = (int64_t)measured_angle;
        correction_angle = -correction_angle;
    }
    else
    {
        correction_angle = (int64_t)measured;
    }

    /* Improved angle accuracy */
    reference *= 100.0;
    correction_angle = correction_angle - (int64_t)reference;

    /* Correction angle should be between -180 and 180 degrees */
    while (correction_angle < (-18000000))
    {
        correction_angle += 36000000;
    }

    while (correction_angle > 18000000)
    {
        correction_angle -= 36000000;
    }

    bams = (double)correction_angle;
    bams = bams * (60.00 / gDrvMetObj.calibrationData.freq);
    bams = bams / 18000000.00; /* get bams and remove precision adjust */
    
    phase_correction = bams * 2147483648.00; /* sQ0.31 */
    correction_angle = (int64_t)phase_correction;
    
    return (uint32_t)correction_angle;
}

static void lDRV_METROLOGY_UpdateMeasurements(void)
{
    uint32_t *afeRMS = NULL;
    uint32_t stateFlagReg;
    uint32_t freq;

    /* Get State Flag Register */
    stateFlagReg = gDrvMetObj.metRegisters->MET_STATUS.STATE_FLAG;
    freq = gDrvMetObj.metRegisters->MET_STATUS.FREQ;

    /* Update RMS values */
    afeRMS = gDrvMetObj.metAFEData.RMS;

    afeRMS[RMS_UA] = lDRV_Metrology_GetVIRMS(gDrvMetObj.metAccData.V_A, gDrvMetObj.metRegisters->MET_CONTROL.K_VA);
    afeRMS[RMS_UB] = lDRV_Metrology_GetVIRMS(gDrvMetObj.metAccData.V_B, gDrvMetObj.metRegisters->MET_CONTROL.K_VB);
    afeRMS[RMS_UC] = lDRV_Metrology_GetVIRMS(gDrvMetObj.metAccData.V_C, gDrvMetObj.metRegisters->MET_CONTROL.K_VC);

    afeRMS[RMS_IA] = lDRV_Metrology_GetVIRMS(gDrvMetObj.metAccData.I_A, gDrvMetObj.metRegisters->MET_CONTROL.K_IA);
    afeRMS[RMS_IB] = lDRV_Metrology_GetVIRMS(gDrvMetObj.metAccData.I_B, gDrvMetObj.metRegisters->MET_CONTROL.K_IB);
    afeRMS[RMS_IC] = lDRV_Metrology_GetVIRMS(gDrvMetObj.metAccData.I_C, gDrvMetObj.metRegisters->MET_CONTROL.K_IC);

    afeRMS[RMS_INI] = lDRV_Metrology_GetInxRMS(gDrvMetObj.metAccData.I_Ni);
    afeRMS[RMS_INM] = lDRV_Metrology_GetVIRMS(gDrvMetObj.metAccData.I_Nm, gDrvMetObj.metRegisters->MET_CONTROL.K_IN);
    afeRMS[RMS_INMI] = lDRV_Metrology_GetInxRMS(gDrvMetObj.metAccData.I_Nmi);

    afeRMS[RMS_PA]  = lDRV_Metrology_GetPQRMS(gDrvMetObj.metAccData.P_A, gDrvMetObj.metRegisters->MET_CONTROL.K_IA, gDrvMetObj.metRegisters->MET_CONTROL.K_VA);
    gDrvMetObj.metAFEData.afeEvents.paDir = lDRV_Metrology_CheckPQDir(gDrvMetObj.metAccData.P_A);
    afeRMS[RMS_PB]  = lDRV_Metrology_GetPQRMS(gDrvMetObj.metAccData.P_B, gDrvMetObj.metRegisters->MET_CONTROL.K_IB, gDrvMetObj.metRegisters->MET_CONTROL.K_VB);
    gDrvMetObj.metAFEData.afeEvents.pbDir = lDRV_Metrology_CheckPQDir(gDrvMetObj.metAccData.P_B);
    afeRMS[RMS_PC]  = lDRV_Metrology_GetPQRMS(gDrvMetObj.metAccData.P_C, gDrvMetObj.metRegisters->MET_CONTROL.K_IC, gDrvMetObj.metRegisters->MET_CONTROL.K_VC);
    gDrvMetObj.metAFEData.afeEvents.pcDir = lDRV_Metrology_CheckPQDir(gDrvMetObj.metAccData.P_C);

    afeRMS[RMS_QA]  = lDRV_Metrology_GetPQRMS(gDrvMetObj.metAccData.Q_A, gDrvMetObj.metRegisters->MET_CONTROL.K_IA, gDrvMetObj.metRegisters->MET_CONTROL.K_VA);
    gDrvMetObj.metAFEData.afeEvents.qaDir = lDRV_Metrology_CheckPQDir(gDrvMetObj.metAccData.Q_A);
    afeRMS[RMS_QB]  = lDRV_Metrology_GetPQRMS(gDrvMetObj.metAccData.Q_B, gDrvMetObj.metRegisters->MET_CONTROL.K_IB, gDrvMetObj.metRegisters->MET_CONTROL.K_VB);
    gDrvMetObj.metAFEData.afeEvents.qbDir = lDRV_Metrology_CheckPQDir(gDrvMetObj.metAccData.Q_B);
    afeRMS[RMS_QC]  = lDRV_Metrology_GetPQRMS(gDrvMetObj.metAccData.Q_C, gDrvMetObj.metRegisters->MET_CONTROL.K_IC, gDrvMetObj.metRegisters->MET_CONTROL.K_VC);
    gDrvMetObj.metAFEData.afeEvents.qcDir = lDRV_Metrology_CheckPQDir(gDrvMetObj.metAccData.Q_C);

    afeRMS[RMS_SA]  = lDRV_Metrology_GetSRMS(gDrvMetObj.metAccData.P_A, gDrvMetObj.metAccData.Q_A, gDrvMetObj.metRegisters->MET_CONTROL.K_IA, gDrvMetObj.metRegisters->MET_CONTROL.K_VA);
    afeRMS[RMS_SB]  = lDRV_Metrology_GetSRMS(gDrvMetObj.metAccData.P_B, gDrvMetObj.metAccData.Q_B, gDrvMetObj.metRegisters->MET_CONTROL.K_IB, gDrvMetObj.metRegisters->MET_CONTROL.K_VB);
    afeRMS[RMS_SC]  = lDRV_Metrology_GetSRMS(gDrvMetObj.metAccData.P_C, gDrvMetObj.metAccData.Q_C, gDrvMetObj.metRegisters->MET_CONTROL.K_IC, gDrvMetObj.metRegisters->MET_CONTROL.K_VC);

    afeRMS[RMS_PT]  = afeRMS[RMS_PA] + afeRMS[RMS_PB] + afeRMS[RMS_PC];
    gDrvMetObj.metAFEData.afeEvents.ptDir = lDRV_Metrology_CheckPQDir(gDrvMetObj.metAccData.P_A + gDrvMetObj.metAccData.P_B + gDrvMetObj.metAccData.P_C);

    afeRMS[RMS_QT]  = afeRMS[RMS_QA] + afeRMS[RMS_QB] + afeRMS[RMS_QC];
    gDrvMetObj.metAFEData.afeEvents.qtDir = lDRV_Metrology_CheckPQDir(gDrvMetObj.metAccData.Q_A + gDrvMetObj.metAccData.Q_B + gDrvMetObj.metAccData.Q_C);

    afeRMS[RMS_ST]  = afeRMS[RMS_SA] + afeRMS[RMS_SB] + afeRMS[RMS_SC];

    afeRMS[RMS_FREQ]  = (freq * 100U) >> FREQ_Q;

    afeRMS[RMS_ANGLEA]  = lDRV_Metrology_GetAngleRMS(gDrvMetObj.metAccData.P_A, gDrvMetObj.metAccData.Q_A);
    afeRMS[RMS_ANGLEB]  = lDRV_Metrology_GetAngleRMS(gDrvMetObj.metAccData.P_B, gDrvMetObj.metAccData.Q_B);
    afeRMS[RMS_ANGLEC]  = lDRV_Metrology_GetAngleRMS(gDrvMetObj.metAccData.P_C, gDrvMetObj.metAccData.Q_C);
    afeRMS[RMS_ANGLEN]  = lDRV_Metrology_GetAngleRMS(gDrvMetObj.metAccData.P_N, gDrvMetObj.metAccData.Q_N);

    gDrvMetObj.metAFEData.energy += lDRV_Metrology_GetPQEnergy(PENERGY);

    /* Update Swell/Sag events */
    gDrvMetObj.metAFEData.afeEvents.sagA = (stateFlagReg & STATUS_STATE_FLAG_SAG_DET_VA_Msk) > 0U? 1U : 0U;
    gDrvMetObj.metAFEData.afeEvents.sagB = (stateFlagReg & STATUS_STATE_FLAG_SAG_DET_VB_Msk) > 0U? 1U : 0U;
    gDrvMetObj.metAFEData.afeEvents.sagC = (stateFlagReg & STATUS_STATE_FLAG_SAG_DET_VC_Msk) > 0U? 1U : 0U;
    gDrvMetObj.metAFEData.afeEvents.swellA = (stateFlagReg & STATUS_STATE_FLAG_SWELL_DET_VA_Msk) > 0U? 1U : 0U;
    gDrvMetObj.metAFEData.afeEvents.swellB = (stateFlagReg & STATUS_STATE_FLAG_SWELL_DET_VB_Msk) > 0U? 1U : 0U;
    gDrvMetObj.metAFEData.afeEvents.swellC = (stateFlagReg & STATUS_STATE_FLAG_SWELL_DET_VC_Msk) > 0U? 1U : 0U;

}

static bool lDRV_METROLOGY_UpdateCalibrationValues(void)
{
    DRV_METROLOGY_CALIBRATION * pCalibrationData;
    DRV_METROLOGY_REGS_ACCUMULATORS * pMetAccRegs;

    pCalibrationData = &gDrvMetObj.calibrationData;
    pMetAccRegs = &gDrvMetObj.metAccData;

    if (pCalibrationData->numIntegrationPeriods != 0U)
    {
        pCalibrationData->numIntegrationPeriods--;

        /* Update Accumulators */
        pCalibrationData->dspAccIa += pMetAccRegs->I_A;
        pCalibrationData->dspAccIb += pMetAccRegs->I_B;
        pCalibrationData->dspAccIc += pMetAccRegs->I_C;
        pCalibrationData->dspAccUa += pMetAccRegs->V_A;
        pCalibrationData->dspAccUb += pMetAccRegs->V_B;
        pCalibrationData->dspAccUc += pMetAccRegs->V_C;
        pCalibrationData->dspAccPa += pMetAccRegs->P_A_F;
        pCalibrationData->dspAccPb += pMetAccRegs->P_B_F;
        pCalibrationData->dspAccPc += pMetAccRegs->P_C_F;
        pCalibrationData->dspAccQa += pMetAccRegs->Q_A_F;
        pCalibrationData->dspAccQb += pMetAccRegs->Q_B_F;
        pCalibrationData->dspAccQc += pMetAccRegs->Q_C_F;

        return false;
    }
    else
    {
        DRV_METROLOGY_REGS_CONTROL * pMetControlRegs;
        uint64_t m, m_div;
        uint32_t k;

        pMetControlRegs = &gDrvMetObj.metRegisters->MET_CONTROL;

        /* The number of the required integration periods has been completed */
        /* Get Calibration Values */
        if ((pCalibrationData->references.lineId == PHASE_A) || (pCalibrationData->references.lineId == PHASE_T))
        {
            /* Calibration I RMS */
            pCalibrationData->dspAccIa >>= 2U;
            k = lDRV_Metrology_GetVIRMS(pCalibrationData->dspAccIa, pMetControlRegs->K_IA);
            m = (uint64_t)pCalibrationData->references.aimIA;
            m = m << CAL_VI_Q;
            m_div = m / k;
            pMetControlRegs->CAL_M_IA = (uint32_t)m_div;
            if ((((m % k) * 10U) / k) > 4U)
            {
                pMetControlRegs->CAL_M_IA += 1U;
            }
            /* Calibration V RMS */
            pCalibrationData->dspAccUa >>= 2U;
            k = lDRV_Metrology_GetVIRMS(pCalibrationData->dspAccUa, pMetControlRegs->K_VA);
            m = (uint64_t)pCalibrationData->references.aimVA;
            m = m << CAL_VI_Q;
            m_div = m / k;
            pMetControlRegs->CAL_M_VA = (uint32_t)m_div;
            if ((((m % k) * 10U) / k) > 4U)
            {
                pMetControlRegs->CAL_M_VA += 1U;
            }

            /* Calibration Phase */
            pCalibrationData->dspAccPa /= 4;
            pCalibrationData->dspAccQa /= 4;
            k = lDRV_Metrology_GetAngleRMS(pCalibrationData->dspAccPa, pCalibrationData->dspAccQa);
            pMetControlRegs->CAL_PH_IA = lDRV_Metrology_CorrectCalibrationAngle(k, pCalibrationData->references.angleA);

            pCalibrationData->result = true;
        }

        if ((pCalibrationData->references.lineId == PHASE_B) || (pCalibrationData->references.lineId == PHASE_T))
        {
            /* Calibration I RMS */
            pCalibrationData->dspAccIb >>= 2U;
            k = lDRV_Metrology_GetVIRMS(pCalibrationData->dspAccIb, pMetControlRegs->K_IB);
            m = (uint64_t)pCalibrationData->references.aimIB;
            m = m << CAL_VI_Q;
            m_div = m / k;
            pMetControlRegs->CAL_M_IB = (uint32_t)m_div;
            if ((((m % k) * 10U) / k) > 4U)
            {
                pMetControlRegs->CAL_M_IB += 1U;
            }
            /* Calibration V RMS */
            pCalibrationData->dspAccUb >>= 2U;
            k = lDRV_Metrology_GetVIRMS(pCalibrationData->dspAccUb, pMetControlRegs->K_VB);
            m = (uint64_t)pCalibrationData->references.aimVB;
            m = m << CAL_VI_Q;
            m_div = m / k;
            pMetControlRegs->CAL_M_VB = (uint32_t)m_div;
            if ((((m % k) * 10U) / k) > 4U)
            {
                pMetControlRegs->CAL_M_VB += 1U;
            }

            /* Calibration Phase */
            pCalibrationData->dspAccPb /= 4;
            pCalibrationData->dspAccQb /= 4;
            k = lDRV_Metrology_GetAngleRMS(pCalibrationData->dspAccPb, pCalibrationData->dspAccQb);
            pMetControlRegs->CAL_PH_IB = lDRV_Metrology_CorrectCalibrationAngle(k, pCalibrationData->references.angleB);

            pCalibrationData->result = true;
        }

        if ((pCalibrationData->references.lineId == PHASE_C) || (pCalibrationData->references.lineId == PHASE_T))
        {
            /* Calibration I RMS */
            pCalibrationData->dspAccIc >>= 2U;
            k = lDRV_Metrology_GetVIRMS(pCalibrationData->dspAccIc, pMetControlRegs->K_IC);
            m = (uint64_t)pCalibrationData->references.aimIC;
            m = m << CAL_VI_Q;
            m_div = m / k;
            pMetControlRegs->CAL_M_IC = (uint32_t)m_div;
            if ((((m % k) * 10U) / k) > 4U)
            {
                pMetControlRegs->CAL_M_IC += 1U;
            }
            /* Calibration V RMS */
            pCalibrationData->dspAccUc >>= 2U;
            k = lDRV_Metrology_GetVIRMS(pCalibrationData->dspAccUc, pMetControlRegs->K_VC);
            m = (uint64_t)pCalibrationData->references.aimVC;
            m = m << CAL_VI_Q;
            m_div = m / k;
            pMetControlRegs->CAL_M_VC = (uint32_t)m_div;
            if ((((m % k) * 10U) / k) > 4U)
            {
                pMetControlRegs->CAL_M_VC += 1U;
            }

            /* Calibration Phase */
            pCalibrationData->dspAccPc /= 4;
            pCalibrationData->dspAccQc /= 4;
            k = lDRV_Metrology_GetAngleRMS(pCalibrationData->dspAccPc, pCalibrationData->dspAccQc);
            pMetControlRegs->CAL_PH_IC = lDRV_Metrology_CorrectCalibrationAngle(k, pCalibrationData->references.angleC);

            pCalibrationData->result = true;
        }

        /* Restore FEATURE_CTRL0 after calibration */
        pMetControlRegs->FEATURE_CTRL0 = pCalibrationData->featureCtrl0Backup;

        /* Calibration has been completed */
        pCalibrationData->running = false;

        return true;
    }
}

static bool lDRV_METROLOGY_UpdateHarmonicAnalysisValues(void)
{
    uint32_t controlMReq;
    uint32_t statusMConf;

    controlMReq = (gDrvMetObj.metRegisters->MET_CONTROL.FEATURE_CTRL1 & FEATURE_CTRL1_HARMONIC_m_REQ_Msk);
    controlMReq >>= FEATURE_CTRL1_HARMONIC_m_REQ_Pos;

    statusMConf = (gDrvMetObj.metRegisters->MET_STATUS.STATE_FLAG & STATUS_STATE_FLAG_HARMONIC_m_CONF_Msk);
    statusMConf >>= STATUS_STATE_FLAG_HARMONIC_m_CONF_Pos;

    if (controlMReq == statusMConf)
    {
        DRV_METROLOGY_HARMONICS_RMS *pHarmonicsRsp = gDrvMetObj.harmonicAnalysisData.pHarmonicAnalysisResponse;
        int32_t harTemp[14];

        (void) memcpy((void *)harTemp, (void *)&gDrvMetObj.metHarData, sizeof(harTemp));

        pHarmonicsRsp->Irms_A_m = lDRV_Metrology_GetHarmonicRMS(harTemp[0], harTemp[7]);
        pHarmonicsRsp->Irms_B_m = lDRV_Metrology_GetHarmonicRMS(harTemp[2], harTemp[9]);
        pHarmonicsRsp->Irms_C_m = lDRV_Metrology_GetHarmonicRMS(harTemp[4], harTemp[11]);
        pHarmonicsRsp->Irms_N_m = lDRV_Metrology_GetHarmonicRMS(harTemp[6], harTemp[13]);
        pHarmonicsRsp->Vrms_A_m = lDRV_Metrology_GetHarmonicRMS(harTemp[1], harTemp[8]);
        pHarmonicsRsp->Vrms_B_m = lDRV_Metrology_GetHarmonicRMS(harTemp[3], harTemp[10]);
        pHarmonicsRsp->Vrms_C_m = lDRV_Metrology_GetHarmonicRMS(harTemp[5], harTemp[12]);
                
        /* Disable Harmonic Analysis */
        gDrvMetObj.metRegisters->MET_CONTROL.FEATURE_CTRL1 &= ~FEATURE_CTRL1_HARMONIC_EN_Msk;
        /* Clear Number of Harmonic for Analysis */
        gDrvMetObj.metRegisters->MET_CONTROL.FEATURE_CTRL1 &= ~FEATURE_CTRL1_HARMONIC_m_REQ_Msk;

        gDrvMetObj.harmonicAnalysisData.harmonicNum = (uint8_t)statusMConf;

        return true;
    }
    else
    {
        return false;
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Driver Common Interface Implementation
// *****************************************************************************
// *****************************************************************************

SYS_MODULE_OBJ DRV_METROLOGY_Initialize (SYS_MODULE_INIT * init, uint32_t resetCause)
{
    /* MISRA C-2012 deviation block start */
    /* MISRA C-2012 Rule 11.3 deviated once. Deviation record ID - H3_MISRAC_2012_R_11_3_DR_1 */
    DRV_METROLOGY_INIT *metInit = (DRV_METROLOGY_INIT *)init;
    /* MISRA C-2012 deviation block end */

    if ((gDrvMetObj.inUse == true) || (init == NULL))
    {
        return SYS_MODULE_OBJ_INVALID;
    }

	/* Clean the IPC interrupt generic flag */
    gDrvMetObj.ipcInterruptFlag = false;

    /* Disable IPC interrupts */
    (void) SYS_INT_SourceDisable(IPC1_IRQn);

	/* Clean the IPC interrupt flags */
    gDrvMetObj.integrationFlag = false;

    if (resetCause != RSTC_SR_RSTTYP(RSTC_SR_RSTTYP_WDT0_RST_Val))
    {
        uint32_t *pSrc;
        uint32_t *pDst;
        uint32_t tmp;

        /* Assert reset of the coprocessor and its peripherals */
        tmp = RSTC_REGS->RSTC_MR;
        tmp &= ~RSTC_MR_CPROCEN_Msk;
        RSTC_REGS->RSTC_MR = RSTC_MR_KEY_PASSWD | tmp;

        /* Disable coprocessor Clocks */
        PMC_REGS->PMC_SCDR = PMC_SCDR_CPKEY_PASSWD | PMC_SCDR_CPCK_Msk;

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
    /* MISRA C-2012 deviation block start */
    /* MISRA C-2012 Rule 11.3 deviated once. Deviation record ID - H3_MISRAC_2012_R_11_3_DR_1 */
    DRV_METROLOGY_INIT *metInit = (DRV_METROLOGY_INIT *)init;
    /* MISRA C-2012 deviation block end */
    uint32_t tmp;
    uint32_t *pSrc;
    uint32_t *pDst;

    if ((gDrvMetObj.inUse == false) || (init == NULL))
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    /* Disable IPC interrupts */
    (void) SYS_INT_SourceDisable(IPC1_IRQn);

    /* Assert reset of the coprocessor and its peripherals */
    tmp = RSTC_REGS->RSTC_MR;
    tmp &= ~RSTC_MR_CPROCEN_Msk;
    RSTC_REGS->RSTC_MR = RSTC_MR_KEY_PASSWD | tmp;

    /* Disable coprocessor Clocks */
    PMC_REGS->PMC_SCDR = PMC_SCDR_CPKEY_PASSWD | PMC_SCDR_CPCK_Msk;

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
        /* De-assert the reset of the coprocessor (Core 1) */
        RSTC_REGS->RSTC_MR |= (RSTC_MR_KEY_PASSWD | RSTC_MR_CPROCEN_Msk);

        /* Enable the coprocessor clock (Core 1) */
        PMC_REGS->PMC_SCER = (PMC_SCER_CPKEY_PASSWD | PMC_SCER_CPCK_Msk);

        /* Wait IPC Init interrupt */
        while(gDrvMetObj.status == DRV_METROLOGY_STATUS_WAITING_IPC)
        {
        }

        /* Keep Metrology Lib in reset */
        gDrvMetObj.metRegisters->MET_CONTROL.STATE_CTRL = STATE_CTRL_STATE_CTRL_RESET_Val;

        if ((pConfiguration != NULL) && (pConfiguration->ATSENSE_CTRL_20_23 != 0UL))
        {
            /* Overwrite STATE CTRL register */
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
    while (gDrvMetObj.metRegisters->MET_STATUS.STATUS != STATUS_STATUS_RESET)
    {
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
                (gDrvMetObj.harmonicAnalysisData.integrationPeriods == 0U) )
            {
                if (lDRV_METROLOGY_UpdateHarmonicAnalysisValues() == true)
                {
                    gDrvMetObj.harmonicAnalysisData.running = false;
                    
                    /* Launch calibration callback */
                    if (gDrvMetObj.harmonicAnalysisCallback != NULL)
                    {
                        gDrvMetObj.harmonicAnalysisCallback(gDrvMetObj.harmonicAnalysisData.harmonicNum);
                    }
                }
            }
        }
    }
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
    /* MISRA C-2012 Rule 11.8 deviated below. Deviation record ID -  
      H3_MISRAC_2012_R_11_8_DR_1*/
    return (DRV_METROLOGY_REGS_CONTROL *)&gDrvMetControlDefault;
   /* MISRAC 2012 deviation block end */
}

DRV_METROLOGY_REGS_ACCUMULATORS * DRV_METROLOGY_GetAccData (void)
{
    return &gDrvMetObj.metAccData;
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
    /* MISRA C-2012 Rule 11.8 deviated below. Deviation record ID -  
      H3_MISRAC_2012_R_11_8_DR_1*/
    /* Keep State Control Register value */
    (void) memcpy((void *)&gDrvMetObj.metRegisters->MET_CONTROL.FEATURE_CTRL0,
                  (void *)&pControl->FEATURE_CTRL0,
                  sizeof(DRV_METROLOGY_REGS_CONTROL) - sizeof(uint32_t));
   /* MISRAC 2012 deviation block end */
}

uint32_t DRV_METROLOGY_GetEnergyValue(bool restartEnergy)
{
    uint32_t energy = gDrvMetObj.metAFEData.energy;

    if (restartEnergy == true)
    {
        gDrvMetObj.metAFEData.energy = 0UL;
    }

    return energy;
}

uint32_t DRV_METROLOGY_GetRMSValue(DRV_METROLOGY_RMS_TYPE type)
{
    uint32_t value;

    value = gDrvMetObj.metAFEData.RMS[type];

    if (type >= RMS_ANGLEA)
    {
        if ((value & 0x80000000UL) != 0U)
        {
            value &= 0x7FFFFFFFUL;
        }

        /* Absolute value should be between 0 and 180 degrees */
        while ((value > 18000000UL) == true)
        {
            value -= 36000000UL;
        }
    }

    return value;
}

DRV_METROLOGY_RMS_SIGN DRV_METROLOGY_GetRMSSign(DRV_METROLOGY_RMS_TYPE type)
{
    DRV_METROLOGY_RMS_SIGN sign;

    if ((type == RMS_PA) && (gDrvMetObj.metAFEData.afeEvents.paDir == 1U))
    {
        sign = RMS_SIGN_NEGATIVE;
    }
    else if ((type == RMS_PB) && (gDrvMetObj.metAFEData.afeEvents.pbDir == 1U))
    {
        sign = RMS_SIGN_NEGATIVE;
    }
    else if ((type == RMS_PC) && (gDrvMetObj.metAFEData.afeEvents.pcDir == 1U))
    {
        sign = RMS_SIGN_NEGATIVE;
    }
    else if ((type == RMS_PT) && (gDrvMetObj.metAFEData.afeEvents.ptDir == 1U))
    {
        sign = RMS_SIGN_NEGATIVE;
    }
    else if ((type == RMS_QA) && (gDrvMetObj.metAFEData.afeEvents.qaDir == 1U))
    {
        sign = RMS_SIGN_NEGATIVE;
    }
    else if ((type == RMS_QB) && (gDrvMetObj.metAFEData.afeEvents.qbDir == 1U))
    {
        sign = RMS_SIGN_NEGATIVE;
    }
    else if ((type == RMS_QC) && (gDrvMetObj.metAFEData.afeEvents.qcDir == 1U))
    {
        sign = RMS_SIGN_NEGATIVE;
    }
    else if ((type == RMS_QT) && (gDrvMetObj.metAFEData.afeEvents.qtDir == 1U))
    {
        sign = RMS_SIGN_NEGATIVE;
    }
    else if ((type >= RMS_ANGLEA) && ((gDrvMetObj.metAFEData.RMS[type] & 0x80000000UL) != 0U))
    {
        sign = RMS_SIGN_NEGATIVE;
    }
    else
    {
        sign = RMS_SIGN_POSITIVE;
    }

    return sign;
}

void DRV_METROLOGY_SetConfiguration(DRV_METROLOGY_CONFIGURATION * config)
{
    double res;
    uint64_t m;
    DRV_METROLOGY_REGS_CONTROL *pControl;
    uint32_t i;
    uint32_t reg, gain;

    /* Store Calibration parameters */
    gDrvMetObj.calibrationData.freq = config->freq;

    pControl = &gDrvMetObj.calibrationData.metControlConf;
    *pControl = gDrvMetObj.metRegisters->MET_CONTROL;

    reg = 1000000000UL / (config->mc);
    m = (uint64_t)reg;
    m = (m << GAIN_P_K_T_Q);
    m = m / 1000000UL;
    reg = (uint32_t)m;

    pControl->P_K_t = reg;
    pControl->Q_K_t = reg;
    pControl->I_K_t = reg;

    reg = pControl->METER_TYPE;
    reg &= ~(METER_TYPE_SENSOR_TYPE_I_A_Msk | METER_TYPE_SENSOR_TYPE_I_B_Msk | METER_TYPE_SENSOR_TYPE_I_C_Msk);
    reg |= METER_TYPE_SENSOR_TYPE_I_A(config->st) |
            METER_TYPE_SENSOR_TYPE_I_B(config->st) | METER_TYPE_SENSOR_TYPE_I_C(config->st);
    pControl->METER_TYPE = reg;

    reg = pControl->ATSENSE_CTRL_20_23;
    reg &= ~(ATSENSE_CTRL_20_23_I0_GAIN_Msk | ATSENSE_CTRL_20_23_I1_GAIN_Msk | ATSENSE_CTRL_20_23_I2_GAIN_Msk);
    reg |= ATSENSE_CTRL_20_23_I0_GAIN(config->gain) |
            ATSENSE_CTRL_20_23_I1_GAIN(config->gain) | ATSENSE_CTRL_20_23_I2_GAIN(config->gain);
    pControl->ATSENSE_CTRL_20_23 = reg;

    reg = pControl->ATSENSE_CTRL_24_27;
    reg &= ~(ATSENSE_CTRL_24_27_I3_GAIN_Msk);
    reg |= ATSENSE_CTRL_24_27_I3_GAIN(config->gain);
    pControl->ATSENSE_CTRL_24_27 = reg;

    gain = (uint32_t)config->gain;
    gain = 1UL << gain;
    if (config->st == SENSOR_CT)
    {
        double divisor;

        res = config->tr * 1000000.0; /* improve accuracy * (ohm -> uohm) */
        divisor = config->rl * 1000000.0; /* improve accuracy * (ohm -> uohm) */
        divisor *= (double)gain;
        res = res / divisor;
    }
    else if (config->st == SENSOR_ROGOWSKI)
    {
        double divisor;

        res = 1000000.0 / config->tr;
        divisor = 60.0 / config->freq;
        divisor *= (double)gain;
        res = res / divisor;
    }
    else if (config->st == SENSOR_SHUNT)
    {
        double divisor;

        divisor = (double)gain * config->rl;
        res = 1000000.0 / divisor;
    }
    else
    {
        res = 0.0;
    }

    m = (uint64_t)res;
    m = m << GAIN_VI_Q; /* format Q22.10 */
    i = (uint32_t) m;

    pControl->K_IA = i;
    pControl->K_IB = i;
    pControl->K_IC = i;
    pControl->K_IN = i;

    i = (config->ku) << GAIN_VI_Q; /* format Q22.10 */
    pControl->K_VA = i;
    pControl->K_VB = i;
    pControl->K_VC = i;

    gDrvMetObj.metRegisters->MET_CONTROL = *pControl;

}

void DRV_METROLOGY_GetEventsData(DRV_METROLOGY_AFE_EVENTS * events)
{
    *events = gDrvMetObj.metAFEData.afeEvents;
}

void DRV_METROLOGY_StartCalibration(void)
{
    DRV_METROLOGY_CALIBRATION * pCalibrationData = &gDrvMetObj.calibrationData;

    if (pCalibrationData->running == false)
    {
        DRV_METROLOGY_REGS_CONTROL * pMetControlRegs = &gDrvMetObj.metRegisters->MET_CONTROL;

        /* Set the number of integration periods to improve the accuracy of the calibration */
        pCalibrationData->numIntegrationPeriods = 4U;

        /* Increase accuracy of references for calibrating procedure */
        pCalibrationData->references.aimIA *= 10000.0;
        pCalibrationData->references.aimVA *= 10000.0;
        pCalibrationData->references.angleA *= 1000.0;
        pCalibrationData->references.aimIB *= 10000.0;
        pCalibrationData->references.aimVB *= 10000.0;
        pCalibrationData->references.angleB *= 1000.0;
        pCalibrationData->references.aimIC *= 10000.0;
        pCalibrationData->references.aimVC *= 10000.0;
        pCalibrationData->references.angleC *= 1000.0;

        /* Save FEATURE_CTRL0 register value, to be restored after calibration */
        pCalibrationData->featureCtrl0Backup = pMetControlRegs->FEATURE_CTRL0;

        /* Init Accumulators */
        pCalibrationData->dspAccIa = 0U;
        pCalibrationData->dspAccIb = 0U;
        pCalibrationData->dspAccIc = 0U;
        pCalibrationData->dspAccIn = 0U;
        pCalibrationData->dspAccUa = 0U;
        pCalibrationData->dspAccUb = 0U;
        pCalibrationData->dspAccUc = 0U;
        pCalibrationData->dspAccUn = 0U;
        pCalibrationData->dspAccPa = 0;
        pCalibrationData->dspAccPb = 0;
        pCalibrationData->dspAccPc = 0;
        pCalibrationData->dspAccQa = 0;
        pCalibrationData->dspAccQb = 0;
        pCalibrationData->dspAccQc = 0;

        /* Initialize calibration registers to the default values */
        switch (pCalibrationData->references.lineId)
        {
            case PHASE_A:
                pMetControlRegs->FEATURE_CTRL0 = FEATURE_CTRL0_RZC_CHAN_SELECT(FEATURE_CTRL0_RZC_CHAN_SELECT_V1_Val) |
                        FEATURE_CTRL0_SYNCH(FEATURE_CTRL0_SYNCH_A_Val) | FEATURE_CTRL0_PHASE_A_EN_Msk;
                break;

            case PHASE_B:
                pMetControlRegs->FEATURE_CTRL0 = FEATURE_CTRL0_RZC_CHAN_SELECT(FEATURE_CTRL0_RZC_CHAN_SELECT_V2_Val) |
                        FEATURE_CTRL0_SYNCH(FEATURE_CTRL0_SYNCH_B_Val) | FEATURE_CTRL0_PHASE_B_EN_Msk;
                break;

            case PHASE_C:
                pMetControlRegs->FEATURE_CTRL0 = FEATURE_CTRL0_RZC_CHAN_SELECT(FEATURE_CTRL0_RZC_CHAN_SELECT_V3_Val) |
                        FEATURE_CTRL0_SYNCH(FEATURE_CTRL0_SYNCH_C_Val) | FEATURE_CTRL0_PHASE_C_EN_Msk;
                break;

            case PHASE_T:
                pMetControlRegs->FEATURE_CTRL0 = FEATURE_CTRL0_RZC_CHAN_SELECT(FEATURE_CTRL0_RZC_CHAN_SELECT_V1_Val) |
                        FEATURE_CTRL0_SYNCH(FEATURE_CTRL0_SYNCH_A_Val) | FEATURE_CTRL0_PHASE_A_EN_Msk |
                        FEATURE_CTRL0_PHASE_B_EN_Msk | FEATURE_CTRL0_PHASE_C_EN_Msk;
                break;

            default:
                pMetControlRegs->FEATURE_CTRL0 = FEATURE_CTRL0_RZC_CHAN_SELECT(FEATURE_CTRL0_RZC_CHAN_SELECT_V1_Val) |
                        FEATURE_CTRL0_SYNCH(FEATURE_CTRL0_SYNCH_A_Val) | FEATURE_CTRL0_PHASE_A_EN_Msk;
                break;

        }

        pCalibrationData->running = true;
        pCalibrationData->result = false;
    }
}

void DRV_METROLOGY_StartHarmonicAnalysis(uint8_t harmonicNum, DRV_METROLOGY_HARMONICS_RMS *pHarmonicResponse)
{
    if (gDrvMetObj.harmonicAnalysisData.running == false)
    {
        gDrvMetObj.harmonicAnalysisData.running = true;
        gDrvMetObj.harmonicAnalysisData.integrationPeriods = 2;

        /* Set Data pointer to store the Harmonic data result */
        gDrvMetObj.harmonicAnalysisData.pHarmonicAnalysisResponse = pHarmonicResponse;

        /* Set Number of Harmonic for Analysis */
        gDrvMetObj.metRegisters->MET_CONTROL.FEATURE_CTRL1 &= ~(FEATURE_CTRL1_HARMONIC_m_REQ_Msk);
        gDrvMetObj.metRegisters->MET_CONTROL.FEATURE_CTRL1 |= FEATURE_CTRL1_HARMONIC_m_REQ(harmonicNum);

        /* Enable Harmonic Analysis */
        gDrvMetObj.metRegisters->MET_CONTROL.FEATURE_CTRL1 |= FEATURE_CTRL1_HARMONIC_EN_Msk;
    }
}
