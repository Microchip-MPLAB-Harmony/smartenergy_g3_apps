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

/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app_metrology.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_METROLOGY_Initialize" and "APP_METROLOGY_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_METROLOGY_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

#ifndef _APP_METROLOGY_H
#define _APP_METROLOGY_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************
typedef enum
{
    APP_METROLOGY_PHASE_A = 1 << 8,
    APP_METROLOGY_PHASE_B = 1 << 9,
    APP_METROLOGY_PHASE_C = 1 << 10,
} APP_METROLOGY_PHASE_ID;

typedef enum
{
    CONTROL_STATE_CTRL = 0,
    CONTROL_FEATURE_CTRL,
    CONTROL_AFE_SELECTION,
    CONTROL_CHANNEL_MATRIX,
    CONTROL_HARMONIC_CTRL,
    CONTROL_METER_TYPE,
    CONTROL_M,
    CONTROL_N_MAX,
    CONTROL_PULSE0_CTRL,
    CONTROL_PULSE0_K_t,
    CONTROL_PULSE1_CTRL,
    CONTROL_PULSE1_K_t,
    CONTROL_PULSE2_CTRL,
    CONTROL_PULSE2_K_t,
    CONTROL_SYNTH_ADDR,
    CONTROL_CREEP_THR_P,
    CONTROL_CREEP_THR_P_A,
    CONTROL_CREEP_THR_P_B,
    CONTROL_CREEP_THR_P_C,
    CONTROL_CREEP_THR_Q,
    CONTROL_CREEP_THR_Q_A,
    CONTROL_CREEP_THR_Q_B,
    CONTROL_CREEP_THR_Q_C,
    CONTROL_CREEP_THR_I,
    CONTROL_CREEP_THR_I_A,
    CONTROL_CREEP_THR_I_B,
    CONTROL_CREEP_THR_I_C,
    CONTROL_CREEP_THR_S,
    CONTROL_PWR_OFFS_CTRL,
    CONTROL_PWR_OFFS_P,
    CONTROL_PWR_OFFS_Q,
    CONTROL_PWR_OFFS_S,
    CONTROL_SWELL_THR_VA,
    CONTROL_SWELL_THR_VB,
    CONTROL_SWELL_THR_VC,
    CONTROL_SAG_THR_VA,
    CONTROL_SAG_THR_VB,
    CONTROL_SAG_THR_VC,
    CONTROL_INT_THR_VA,
    CONTROL_INT_THR_VB,
    CONTROL_INT_THR_VC,
    CONTROL_RESERVED_C41,
    CONTROL_RESERVED_C42,
    CONTROL_RESERVED_C43,
    CONTROL_K_IA,
    CONTROL_K_VA,
    CONTROL_K_IB,
    CONTROL_K_VB,
    CONTROL_K_IC,
    CONTROL_K_VC,
    CONTROL_K_IN,
    CONTROL_K_VD,
    CONTROL_CAL_M_IA,
    CONTROL_CAL_M_VA,
    CONTROL_CAL_M_IB,
    CONTROL_CAL_M_VB,
    CONTROL_CAL_M_IC,
    CONTROL_CAL_M_VC,
    CONTROL_CAL_M_IN,
    CONTROL_CAL_M_VD,
    CONTROL_CAL_PH_IA,
    CONTROL_CAL_PH_VA,
    CONTROL_CAL_PH_IB,
    CONTROL_CAL_PH_VB,
    CONTROL_CAL_PH_IC,
    CONTROL_CAL_PH_VC,
    CONTROL_CAL_PH_IN,
    CONTROL_RESERVED_C67,
    CONTROL_CPTR_CTRL,
    CONTROL_CPTR_BUFF_SIZE,
    CONTROL_CPTR_ADDR,
    CONTROL_RESERVED_C71,
    CONTROL_RESERVED_C72,
    CONTROL_RESERVED_C73,
    CONTROL_AFE_CTRL,
    CONTROL_RESERVED_C75,
    CONTROL_RESERVED_C76,
    CONTROL_RESERVED_C77,
    CONTROL_PWR_OFFS_P_A,
    CONTROL_PWR_OFFS_P_B,
    CONTROL_PWR_OFFS_P_C,
    CONTROL_PWR_OFFS_Q_A,
    CONTROL_PWR_OFFS_Q_B,
    CONTROL_PWR_OFFS_Q_C,
    CONTROL_REG_NUM,
} CONTROL_REG_ID;

typedef enum
{
    STATUS_VERSION = 0,
    STATUS_STATUS,
    STATUS_STATE_FLAG,
    STATUS_CAPTURE_STATUS,
    STATUS_INTERVAL_NUM,
    STATUS_N,
    STATUS_PH_OFFSET,
    STATUS_FREQ,
    STATUS_FREQ_VA,
    STATUS_FREQ_VB,
    STATUS_FREQ_VC,
    STATUS_RESERVED_S11,
    STATUS_TEMPERATURE,
    STATUS_V_A_MAX,
    STATUS_V_B_MAX,
    STATUS_V_C_MAX,
    STATUS_RESERVED_S16,
    STATUS_I_A_MAX,
    STATUS_I_B_MAX,
    STATUS_I_C_MAX,
    STATUS_I_Nm_MAX,
    STATUS_I_Ni_MAX,
    STATUS_FEATURES,
    STATUS_RESERVED_S23,
    STATUS_RESERVED_S24,
    STATUS_RESERVED_S25,
    STATUS_PULSE0_COUNTER,
    STATUS_PULSE1_COUNTER,
    STATUS_PULSE2_COUNTER,
    STATUS_RESERVED_S29,
    STATUS_ZC_N_VA,
    STATUS_ZC_N_VB,
    STATUS_ZC_N_VC,
    STATUS_RESERVED_S33,
    STATUS_ATSENSE_CAL_41_44,
    STATUS_ATSENSE_CAL_45_48,
    STATUS_REG_NUM,
} STATUS_REG_ID;

typedef enum
{
    ACCUMULATOR_V_A = 0,
    ACCUMULATOR_V_B,
    ACCUMULATOR_V_C,
    ACCUMULATOR_V_D,
    ACCUMULATOR_V_A_F,
    ACCUMULATOR_V_B_F,
    ACCUMULATOR_V_C_F,
    ACCUMULATOR_V_D_F,
    ACCUMULATOR_V_AB,
    ACCUMULATOR_V_BC,
    ACCUMULATOR_V_CA,
    ACCUMULATOR_V_AB_F,
    ACCUMULATOR_V_BC_F,
    ACCUMULATOR_V_CA_F,
    ACCUMULATOR_RESERVED_A14,
    ACCUMULATOR_RESERVED_A15,
    ACCUMULATOR_RESERVED_A16,
    ACCUMULATOR_I_A,
    ACCUMULATOR_I_B,
    ACCUMULATOR_I_C,
    ACCUMULATOR_I_Ni,
    ACCUMULATOR_I_Nm,
    ACCUMULATOR_I_A_F,
    ACCUMULATOR_I_B_F,
    ACCUMULATOR_I_C_F,
    ACCUMULATOR_I_Nmi,
    ACCUMULATOR_I_Nm_F,
    ACCUMULATOR_RESERVED_A27,
    ACCUMULATOR_RESERVED_A28,
    ACCUMULATOR_RESERVED_A29,
    ACCUMULATOR_RESERVED_A30,
    ACCUMULATOR_RESERVED_A31,
    ACCUMULATOR_P_A,
    ACCUMULATOR_P_B,
    ACCUMULATOR_P_C,
    ACCUMULATOR_P_A_F,
    ACCUMULATOR_P_B_F,
    ACCUMULATOR_P_C_F,
    ACCUMULATOR_P_N,
    ACCUMULATOR_P_N_F,
    ACCUMULATOR_RESERVED_A40,
    ACCUMULATOR_Q_A,
    ACCUMULATOR_Q_B,
    ACCUMULATOR_Q_C,
    ACCUMULATOR_Q_A_F,
    ACCUMULATOR_Q_B_F,
    ACCUMULATOR_Q_C_F,
    ACCUMULATOR_Q_N,
    ACCUMULATOR_Q_N_F,
    ACCUMULATOR_RESERVED_A49,
    ACCUMULATOR_ACC_T0,
    ACCUMULATOR_ACC_T1,
    ACCUMULATOR_ACC_T2,
    ACCUMULATOR_RESERVED_A53,
    ACCUMULATOR_RESERVED_A54,
    ACCUMULATOR_REG_NUM,
} ACCUMULATOR_REG_ID;

typedef enum
{
    PER_CYCLE_ACC_V_A = 0,
    PER_CYCLE_ACC_V_B,
    PER_CYCLE_ACC_V_C,
    PER_CYCLE_ACC_V_A_F,
    PER_CYCLE_ACC_V_B_F,
    PER_CYCLE_ACC_V_C_F,
    PER_CYCLE_ACC_V_AB,
    PER_CYCLE_ACC_V_BC,
    PER_CYCLE_ACC_V_CA,
    PER_CYCLE_ACC_V_AB_F,
    PER_CYCLE_ACC_V_BC_F,
    PER_CYCLE_ACC_V_CA_F,
    PER_CYCLE_ACC_I_A,
    PER_CYCLE_ACC_I_B,
    PER_CYCLE_ACC_I_C,
    PER_CYCLE_ACC_I_Nm,
    PER_CYCLE_ACC_I_A_F,
    PER_CYCLE_ACC_I_B_F,
    PER_CYCLE_ACC_I_C_F,
    PER_CYCLE_ACC_I_Nm_F,
    PER_CYCLE_ACC_P_A,
    PER_CYCLE_ACC_P_B,
    PER_CYCLE_ACC_P_C,
    PER_CYCLE_ACC_P_A_F,
    PER_CYCLE_ACC_P_B_F,
    PER_CYCLE_ACC_P_C_F,
    PER_CYCLE_ACC_P_N,
    PER_CYCLE_ACC_P_N_F,
    PER_CYCLE_ACC_Q_A,
    PER_CYCLE_ACC_Q_B,
    PER_CYCLE_ACC_Q_C,
    PER_CYCLE_ACC_Q_A_F,
    PER_CYCLE_ACC_Q_B_F,
    PER_CYCLE_ACC_Q_C_F,
    PER_CYCLE_ACC_Q_N,
    PER_CYCLE_ACC_Q_N_F,
    PER_CYCLE_ACC_REG_NUM,
} PER_CYCLE_ACCUMULATOR_REG_ID;

typedef enum
{
    HARMONICS_I_A_m_R_ID = 0,
    HARMONICS_V_A_m_R_ID,
    HARMONICS_I_B_m_R_ID,
    HARMONICS_V_B_m_R_ID,
    HARMONICS_I_C_m_R_ID,
    HARMONICS_V_C_m_R_ID,
    HARMONICS_I_N_m_R_ID,
    HARMONICS_I_A_m_I_ID,
    HARMONICS_V_A_m_I_ID,
    HARMONICS_I_B_m_I_ID,
    HARMONICS_V_B_m_I_ID,
    HARMONICS_I_C_m_I_ID,
    HARMONICS_V_C_m_I_ID,
    HARMONICS_I_N_m_I_ID,
    HARMONICS_REG_NUM,
} HARMONICS_REG_ID;

// *****************************************************************************
/* Application states

  Summary:
    Application states enumeration

  Description:
    This enumeration defines the valid application states.  These states
    determine the behavior of the application at various times.
*/

typedef enum
{
    /* Application's state machine's initial state. */
    APP_METROLOGY_STATE_ICM_GET_HASH = 0,    
    APP_METROLOGY_STATE_ICM_START_MONITOR,
    APP_METROLOGY_STATE_WAITING_DATALOG,
    APP_METROLOGY_STATE_INIT,
    APP_METROLOGY_STATE_START,
    APP_METROLOGY_STATE_RUNNING,
    APP_METROLOGY_STATE_CHECK_CALIBRATION,
    APP_METROLOGY_STATE_WAIT_DATA,
    APP_METROLOGY_STATE_ERROR

} APP_METROLOGY_STATE;

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    Application strings and buffers are be defined outside this structure.
 */

typedef struct
{
    /* The application's current state */
    APP_METROLOGY_STATE state;

    DRV_METROLOGY_START_MODE startMode;
    DRV_METROLOGY_REGS_CONTROL configuration;

    DRV_METROLOGY_REGS_CONTROL * pMetControl;
    DRV_METROLOGY_REGS_STATUS * pMetStatus;
    DRV_METROLOGY_REGS_ACCUMULATORS * pMetAccData;
    DRV_METROLOGY_REGS_PERCYCLE_ACC * pMetPerCycleAccData;
    DRV_METROLOGY_REGS_HARMONICS * pMetHarData;
    
    uint32_t metBinStartAddress;
    uint32_t metBinSize;

    bool harmonicAnalysisPending;
    bool stopHarmonicAnalysis;
    bool sendHarmonicsToConsole;
    DRV_METROLOGY_HARMONICS_RMS * pHarmonicAnalysisResponse;
    DRV_METROLOGY_HARMONICS_CALLBACK pHarmonicAnalysisCallback;
    DRV_METROLOGY_REGS_HARMONICS harmonicsData;

    DRV_METROLOGY_CALIBRATION_CALLBACK pCalibrationCallback;

    DRV_METROLOGY_AFE_EVENTS_UNION eventFlagsPrev;

    uint32_t queueFree;

    bool setConfiguration;

    bool dataIsRdy;

    volatile bool halfCycleFlag;

    bool dataFlag;
    
    volatile bool metBinHashCompleted;
    
    volatile bool metBinMismatch;

} APP_METROLOGY_DATA;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Routines
// *****************************************************************************
// *****************************************************************************
/* These routines are called by drivers when certain events occur.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_METROLOGY_Initialize (void)

  Summary:
     MPLAB Harmony application initialization routine.

  Description:
    This function initializes the Harmony application.  It places the
    application in its initial state and prepares it to run so that its
    APP_METROLOGY_Tasks function can be called.

  Precondition:
    All other system initialization routines should be called before calling
    this routine (in "SYS_Initialize").

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_METROLOGY_Initialize();
    </code>

  Remarks:
    This routine must be called from the SYS_Initialize function.
*/

void APP_METROLOGY_Initialize (void);


/*******************************************************************************
  Function:
    void APP_METROLOGY_Tasks (void)

  Summary:
    MPLAB Harmony Demo application tasks function

  Description:
    This routine is the Harmony Demo application's tasks function.  It
    defines the application's state machine and core logic.

  Precondition:
    The system and application initialization ("SYS_Initialize") should be
    called before calling this.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_METROLOGY_Tasks();
    </code>

  Remarks:
    This routine must be called from SYS_Tasks() routine.
 */

void APP_METROLOGY_Tasks(void);
APP_METROLOGY_STATE APP_METROLOGY_GetState(void);
bool APP_METROLOGY_GetControlRegister(CONTROL_REG_ID regId, uint32_t * regValue, char *regName);
bool APP_METROLOGY_SetControlRegister(CONTROL_REG_ID regId, uint32_t value);
bool APP_METROLOGY_GetStatusRegister(STATUS_REG_ID regId, uint32_t * regValue, char *regName);
bool APP_METROLOGY_GetAccumulatorRegister(ACCUMULATOR_REG_ID regId, uint64_t * regValue, char *regName);
bool APP_METROLOGY_GetPCAccumulatorRegister(PER_CYCLE_ACCUMULATOR_REG_ID regId, uint64_t * regValue, char *regName);
void APP_METROLOGY_CaptureHarmonicData(void);
bool APP_METROLOGY_GetHarmonicRegister(HARMONICS_REG_ID regId, uint8_t harmonicNum, uint32_t *regValue, char *regName);
bool APP_METROLOGY_GetMeasure(DRV_METROLOGY_MEASURE_TYPE measureId, float * value, bool perCyleMeasure);
void APP_METROLOGY_SetControlByDefault(void);
void APP_METROLOGY_StoreMetrologyData(void);
void APP_METROLOGY_StartCalibration(DRV_METROLOGY_CALIBRATION_REFS * calibration);
void APP_METROLOGY_SetCalibrationCallback(DRV_METROLOGY_CALIBRATION_CALLBACK callback);
bool APP_METROLOGY_StartHarmonicAnalysis(uint32_t harmonicBitmap, bool singleMode);
void APP_METROLOGY_StopHarmonicAnalysis(void);
void APP_METROLOGY_SetHarmonicAnalysisCallback(DRV_METROLOGY_HARMONICS_CALLBACK callback,
        DRV_METROLOGY_HARMONICS_RMS * pHarmonicAnalysisResponse);
void APP_METROLOGY_Restart(bool reloadRegsFromMemory);
void APP_METROLOGY_SetLowPowerMode (void);
void APP_METROLOGY_StopMetrology (void);
bool APP_METROLOGY_CheckPhaseEnabled (APP_METROLOGY_PHASE_ID phase);
DRV_METROLOGY_AFE_TYPE APP_METROLOGY_GetAFEDescription(char *pDescription);
uint8_t APP_METROLOGY_GetChannelsDescription(DRV_METROLOGY_CHANNEL **pChannelDesc);
void APP_METROLOGY_GetAccRegData(DRV_METROLOGY_REGS_ACCUMULATORS *pData, char **pDescription);
void APP_METROLOGY_GetPerCycleAccRegData(DRV_METROLOGY_REGS_PERCYCLE_ACC *pData, char **pDescription);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* _APP_METROLOGY_H */

/*******************************************************************************
 End of File
 */

