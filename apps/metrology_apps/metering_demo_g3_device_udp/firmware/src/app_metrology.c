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

/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_metrology.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "definitions.h"
#include "app_metrology.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

extern APP_DATALOG_QUEUE appDatalogQueue;
APP_DATALOG_QUEUE_DATA appMetrologyDatalogQueueData;

extern APP_ENERGY_QUEUE appEnergyQueue;
extern APP_EVENTS_QUEUE appEventsQueue;

extern DRV_METROLOGY_INIT drvMetrologyInitData;

const char * _met_control_desc[] =
{
  "00 STATE_CTRL",
  "01 FEATURE_CTRL0",
  "02 FEATURE_CTRL1",
  "03 METER_TYPE",
  "04 M",
  "05 N_MAX",
  "06 PULSE_CTRL0",
  "07 PULSE_CTRL1",
  "08 PULSE_CTRL2",
  "09 P_K_T",
  "10 Q_K_T",
  "11 I_K_T",
  "12 CREEP_THR_P",
  "13 CREEP_THR_Q",
  "14 CREEP_THR_I",
  "15 POWER_OS_CTRL",
  "16 POWER_OFFSET_P",
  "17 POWER_OFFSET_Q",
  "18 SWELL_THR_VA",
  "19 SWELL_THR_VB",
  "20 SWELL_THR_VC",
  "21 SAG_THR_VA",
  "22 SAG_THR_VB",
  "23 SAG_THR_VC",
  "24 K_IA",
  "25 K_VA",
  "26 K_IB",
  "27 K_VB",
  "28 K_IC",
  "29 K_VC",
  "30 K_IN",
  "31 CAL_M_IA",
  "32 CAL_M_VA",
  "33 CAL_M_IB",
  "34 CAL_M_VB",
  "35 CAL_M_IC",
  "36 CAL_M_VC",
  "37 CAL_M_IN",
  "38 CAL_PH_IA",
  "39 CAL_PH_VA",
  "40 CAL_PH_IB",
  "41 CAL_PH_VB",
  "42 CAL_PH_IC",
  "43 CAL_PH_VC",
  "44 CAL_PH_IN",
  "45 CAPTURE_CTRL",
  "46 CAPT_BUFF_SIZE",
  "47 CAPTURE_ADDR",
  "48 RESERVED_C48",
  "49 RESERVED_C49",
  "50 RESERVED_C50",
  "51 AT_CTRL_20_23",
  "52 AT_CTRL_24_27",
  "53 AT_CTRL_28_2B",
  "54 RESERVED_C54",
  "55 POWER_OS_P_A",
  "56 POWER_OS_P_B",
  "57 POWER_OS_P_C",
  "58 POWER_OS_Q_A",
  "59 POWER_OS_Q_B",
  "60 POWER_OS_Q_C"
};

const char * _met_status_desc[] =
{
  "00 VERSION",
  "01 STATUS",
  "02 STATE_FLAG",
  "03 CAPTURE_STATUS",
  "04 INTERVAL_NUM",
  "05 N",
  "06 PH_OFFSET",
  "07 FREQ",
  "08 FREQ_VA",
  "09 FREQ_VB",
  "10 FREQ_VC",
  "11 RESERVED_S11",
  "12 TEMPERATURE",
  "13 I_A_MAX",
  "14 I_B_MAX",
  "15 I_C_MAX",
  "16 I_Nm_MAX",
  "17 I_Ni_MAX",
  "18 V_A_MAX",
  "19 V_B_MAX",
  "20 V_C_MAX",
  "21 FEATURES",
  "22 RESERVED_S22",
  "23 RESERVED_S23",
  "24 RESERVED_S24",
  "25 RESERVED_S25",
  "26 RESERVED_S26",
  "27 RESERVED_S27",
  "28 RESERVED_S28",
  "29 RESERVED_S29",
  "30 ZC_N_VA",
  "31 ZC_N_VB",
  "32 ZC_N_VC",
  "33 AT_CAL_41_44",
  "34 AT_CAL_45_48"
};

const char * _met_acc_desc[] =
{
  "00 I_A",
  "01 I_B",
  "02 I_C",
  "03 I_Ni",
  "04 I_Nm",
  "05 I_A_F",
  "06 I_B_F",
  "07 I_C_F",
  "08 I_Nmi",
  "09 RESERVED_A09",
  "10 RESERVED_A10",
  "11 RESERVED_A11",
  "12 RESERVED_A12",
  "13 RESERVED_A13",
  "14 RESERVED_A14",
  "15 P_A",
  "16 P_B",
  "17 P_C",
  "18 P_A_F",
  "19 P_B_F",
  "20 P_C_F",
  "21 P_N",
  "22 P_N_F",
  "23 RESERVED_A23",
  "24 Q_A",
  "25 Q_B",
  "26 Q_C",
  "27 Q_A_F",
  "28 Q_B_F",
  "29 Q_C_F",
  "30 Q_N",
  "31 Q_N_F",
  "32 RESERVED_A32",
  "33 V_A",
  "34 V_B",
  "35 V_C",
  "36 RESERVED_A36",
  "37 V_A_F",
  "38 V_B_F",
  "39 V_C_F",
  "40 RESERVED_A40",
  "41 V_AB",
  "42 V_BC",
  "43 V_CA",
  "44 V_AB_F",
  "45 V_BC_F",
  "46 V_CA_F",
  "47 RESERVED_A47",
  "48 RESERVED_A48",
  "49 RESERVED_A49",
  "50 ACC_T0",
  "51 ACC_T1",
  "52 ACC_T2",
  "53 RESERVED_A53",
  "54 RESERVED_A54"
};

const char * _met_har_desc[] =
{
  "00 I_A_m_R",
  "01 V_A_m_R",
  "02 I_B_m_R",
  "03 V_B_m_R",
  "04 I_C_m_R",
  "05 V_C_m_R",
  "06 I_N_m_R",
  "07 I_A_m_I",
  "08 V_A_m_I",
  "09 I_B_m_I",
  "10 V_B_m_I",
  "11 I_C_m_I",
  "12 V_C_m_I",
  "13 I_N_m_I",
};

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_METROLOGY_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_METROLOGY_DATA CACHE_ALIGN app_metrologyData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************
static void _APP_METROLOGY_GetNVMDataCallback(APP_DATALOG_RESULT result);

static void _APP_METROLOGY_LoadControlFromMemory(DRV_METROLOGY_REGS_CONTROL * controlReg)
{
    appMetrologyDatalogQueueData.userId = APP_DATALOG_USER_METROLOGY;
    appMetrologyDatalogQueueData.operation = APP_DATALOG_READ;
    appMetrologyDatalogQueueData.pData = (uint8_t *)controlReg;
    appMetrologyDatalogQueueData.dataLen = sizeof(DRV_METROLOGY_REGS_CONTROL);
    appMetrologyDatalogQueueData.endCallback = _APP_METROLOGY_GetNVMDataCallback;
    appMetrologyDatalogQueueData.date.month = APP_DATALOG_INVALID_MONTH;
    appMetrologyDatalogQueueData.date.year = APP_DATALOG_INVALID_YEAR;

    APP_DATALOG_SendDatalogData(&appMetrologyDatalogQueueData);
}

static void _APP_METROLOGY_StoreControlInMemory(DRV_METROLOGY_REGS_CONTROL * controlReg)
{
    appMetrologyDatalogQueueData.userId = APP_DATALOG_USER_METROLOGY;
    appMetrologyDatalogQueueData.operation = APP_DATALOG_WRITE;
    appMetrologyDatalogQueueData.pData = (uint8_t *)controlReg;
    appMetrologyDatalogQueueData.dataLen = sizeof(DRV_METROLOGY_REGS_CONTROL);
    appMetrologyDatalogQueueData.endCallback = NULL;
    appMetrologyDatalogQueueData.date.month = APP_DATALOG_INVALID_MONTH;
    appMetrologyDatalogQueueData.date.year = APP_DATALOG_INVALID_YEAR;

    APP_DATALOG_SendDatalogData(&appMetrologyDatalogQueueData);
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

static void _APP_METROLOGY_IntegrationCallback(void)
{
    if ((app_metrologyData.state == APP_METROLOGY_STATE_RUNNING) ||
        (app_metrologyData.state == APP_METROLOGY_STATE_CHECK_CALIBRATION))
    {
        /* Signal Metrology thread to update measurements for an integration period */
        app_metrologyData.integrationFlag = true;
    }
}

static void _APP_METROLOGY_CalibrationCallback(bool result)
{
    if (result == true)
    {
        /* Store CONTROL Regs in NVM */
        _APP_METROLOGY_StoreControlInMemory(app_metrologyData.pMetControl);
    }

    if (app_metrologyData.pCalibrationCallback)
    {
        app_metrologyData.pCalibrationCallback(result);
    }

    /* Signal Metrology to exit calibration status */
    app_metrologyData.calibrationFlag = true;
}

static void _APP_METROLOGY_HarmonicAnalysisCallback(uint8_t harmonicNum)
{
    if (app_metrologyData.pHarmonicAnalysisCallback)
    {
        app_metrologyData.harmonicAnalysisPending = false;
        app_metrologyData.pHarmonicAnalysisCallback(harmonicNum);
    }
}

static void _APP_METROLOGY_GetNVMDataCallback(APP_DATALOG_RESULT result)
{
    if (result == APP_DATALOG_RESULT_SUCCESS)
    {
        app_metrologyData.dataIsRdy = true;
    }

    // Handle task state
    app_metrologyData.dataFlag = true;
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_METROLOGY_Initialize (void)

  Remarks:
    See prototype in app_metrology.h.
 */

void APP_METROLOGY_Initialize (void)
{
    /* Place the App state machine in its initial state. */
    app_metrologyData.state = APP_METROLOGY_STATE_WAITING_DATALOG;

    /* Flag to indicate if configuration should be applied */
    app_metrologyData.setConfiguration = false;

    /* Detection of the WDOG0 Reset */
    if (RSTC_ResetCauseGet() == RSTC_SR_RSTTYP(RSTC_SR_RSTTYP_WDT0_RST_Val))
    {
        app_metrologyData.startMode = DRV_METROLOGY_START_SOFT;
    }
    else
    {
        app_metrologyData.startMode = DRV_METROLOGY_START_HARD;
    }

    /* Get Pointers to metrology data regions */
    app_metrologyData.pMetControl = DRV_METROLOGY_GetControlData();
    app_metrologyData.pMetStatus = DRV_METROLOGY_GetStatusData();
    app_metrologyData.pMetAccData = DRV_METROLOGY_GetAccData();
    app_metrologyData.pMetHarData = DRV_METROLOGY_GetHarData();

    /* Set Callback for each metrology integration process */
    DRV_METROLOGY_IntegrationCallbackRegister(_APP_METROLOGY_IntegrationCallback);
    /* Set Callback for calibration process */
    DRV_METROLOGY_CalibrationCallbackRegister(_APP_METROLOGY_CalibrationCallback);
    /* Set Callback for harmonic analysis process */
    DRV_METROLOGY_HarmonicAnalysisCallbackRegister(_APP_METROLOGY_HarmonicAnalysisCallback);

    /* Clear Harmonic Analysis Data */
    app_metrologyData.harmonicAnalysisPending = false;
    app_metrologyData.pHarmonicAnalysisCallback = NULL;
    app_metrologyData.pHarmonicAnalysisResponse = NULL;

    /* Clear Calibration Data */
    app_metrologyData.pCalibrationCallback = NULL;
    app_metrologyData.calibrationFlag = false;

    /* Initialize integration Flag */
    app_metrologyData.integrationFlag = false;
    app_metrologyData.dataFlag = false;

}

/******************************************************************************
  Function:
    void APP_METROLOGY_Tasks (void)

  Remarks:
    See prototype in app_metrology.h.
 */
void APP_METROLOGY_Tasks (void)
{
    APP_ENERGY_QUEUE_DATA newMetrologyData;
    APP_EVENTS_QUEUE_DATA newEvent;

    /* Check the application's current state. */
    switch (app_metrologyData.state)
    {
        /* Application's initial state. */
        case APP_METROLOGY_STATE_WAITING_DATALOG:
        {
            if (APP_DATALOG_GetStatus() == APP_DATALOG_STATE_READY)
            {
                /* Check if there are Metrology data in memory */
                if (APP_DATALOG_FileExists(APP_DATALOG_USER_METROLOGY, NULL))
                {
                    /* Metrology data exists */
                    _APP_METROLOGY_LoadControlFromMemory(&app_metrologyData.configuration);
                    /* Wait for the loading data from memory */
                    app_metrologyData.state = APP_METROLOGY_STATE_WAIT_DATA;
                }
                else
                {
                    app_metrologyData.state = APP_METROLOGY_STATE_INIT;
                }
            }

            break;
        }

        case APP_METROLOGY_STATE_WAIT_DATA:
        {
            /* Apply Configuration Data */
            if (app_metrologyData.dataIsRdy)
            {
                /* Update Flag to apply external configuration */
                app_metrologyData.setConfiguration = true;
                app_metrologyData.state = APP_METROLOGY_STATE_INIT;
            }
            break;
        }

        case APP_METROLOGY_STATE_INIT:
        {
            DRV_METROLOGY_REGS_CONTROL * pConfiguration = NULL;

            /* Check if external configuration should be applied */
            if (app_metrologyData.setConfiguration)
            {
                pConfiguration = &app_metrologyData.configuration;
            }

            if (DRV_METROLOGY_Open(app_metrologyData.startMode, pConfiguration) == DRV_METROLOGY_SUCCESS)
            {
                if (app_metrologyData.startMode == DRV_METROLOGY_START_HARD)
                {
                    app_metrologyData.state = APP_METROLOGY_STATE_START;
                }
                else
                {
                    app_metrologyData.state = APP_METROLOGY_STATE_RUNNING;
                }
            }
            else
            {
                app_metrologyData.state = APP_METROLOGY_STATE_ERROR;
            }

            break;
        }

        case APP_METROLOGY_STATE_START:
        {
            if (DRV_METROLOGY_GetStatus() == DRV_METROLOGY_STATUS_READY)
            {
                /* Check if there are Metrology data in memory */
                if (APP_DATALOG_FileExists(APP_DATALOG_USER_METROLOGY, NULL) == false)
                {
                    /* Metrology data does not exists. Store in NVM */
                    _APP_METROLOGY_StoreControlInMemory(app_metrologyData.pMetControl);
                }

                if (DRV_METROLOGY_Start() == DRV_METROLOGY_SUCCESS)
                {
                    app_metrologyData.state = APP_METROLOGY_STATE_RUNNING;
                }
                else
                {
                    app_metrologyData.state = APP_METROLOGY_STATE_ERROR;
                }

            }

            break;
        }

        case APP_METROLOGY_STATE_RUNNING:
        {
            /* Wait for the integration flag to get measurements at the end of the integration period. */
            if (app_metrologyData.integrationFlag)
            {
                app_metrologyData.integrationFlag = false;

                // Send new Energy values to the Energy Task
                newMetrologyData.energy = DRV_METROLOGY_GetEnergyValue(true);
                newMetrologyData.Pt = DRV_METROLOGY_GetRMSValue(RMS_PT);
                if (APP_ENERGY_SendEnergyData(&newMetrologyData) == false)
                {
                    SYS_CMD_MESSAGE("ENERGY Queue is FULL!!!\r\n");
                }

                // Send new Events to the Events Task
                RTC_TimeGet(&newEvent.eventTime);
                DRV_METROLOGY_GetEventsData(&newEvent.eventFlags);
                if (APP_EVENTS_SendEventsData(&newEvent) == false)
                {
                    SYS_CMD_MESSAGE("EVENTS Queue is FULL!!!\r\n");
                }
            }

            break;
        }

        case APP_METROLOGY_STATE_CHECK_CALIBRATION:
        {
            /* Wait for the metrology semaphore to wait calibration ends. */
            if (app_metrologyData.calibrationFlag)
            {
                app_metrologyData.calibrationFlag = false;
                app_metrologyData.state = APP_METROLOGY_STATE_RUNNING;
            }

            break;
        }

        /* The default state should never be executed. */
        case APP_METROLOGY_STATE_ERROR:
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}

APP_METROLOGY_STATE APP_METROLOGY_GetState(void)
{
    return app_metrologyData.state;
}

bool APP_METROLOGY_GetControlRegister(CONTROL_REG_ID regId, uint32_t * regValue, char *regName)
{
    uint32_t *pData;

    if (regId >= CONTROL_REG_NUM)
    {
        return false;
    }

    pData = (uint32_t *)app_metrologyData.pMetControl;
    pData += regId;
    *regValue = *pData;

    if (regName)
    {
        sprintf(regName,"%s",_met_control_desc[regId]);
    }

    return true;

}

bool APP_METROLOGY_SetControlRegister(CONTROL_REG_ID regId, uint32_t value)
{
    uint32_t *pData;

    if (regId >= CONTROL_REG_NUM)
    {
        return false;
    }

    pData = (uint32_t *)app_metrologyData.pMetControl;
    pData += regId;
    *pData = value;

    return true;
}

bool APP_METROLOGY_GetStatusRegister(STATUS_REG_ID regId, uint32_t * regValue, char *regName)
{
    uint32_t *pData;

    if (regId >= STATUS_REG_NUM)
    {
        return false;
    }

    pData = (uint32_t *)app_metrologyData.pMetStatus;
    pData += regId;
    *regValue = *pData;

    if (regName)
    {
        sprintf(regName,"%s",_met_status_desc[regId]);
    }

    return true;
}

bool APP_METROLOGY_GetAccumulatorRegister(ACCUMULATOR_REG_ID regId, uint64_t * regValue, char *regName)
{
    uint64_t *pData;

    if (regId >= ACCUMULATOR_REG_NUM)
    {
        return false;
    }

    pData = (uint64_t *)app_metrologyData.pMetAccData;
    pData += regId;
    *regValue = *pData;

    if (regName)
    {
        sprintf(regName,"%s",_met_acc_desc[regId]);
    }

    return true;
}

bool APP_METROLOGY_GetHarmonicsRegister(HARMONICS_REG_ID regId, uint32_t * regValue, char *regName)
{
    uint32_t *pData;

    if (regId >= HARMONICS_REG_NUM)
    {
        return false;
    }

    pData = (uint32_t *)app_metrologyData.pMetHarData;
    pData += regId;
    *regValue = *pData;

    if (regName)
    {
        sprintf(regName,"%s",_met_har_desc[regId]);
    }

    return true;
}

bool APP_METROLOGY_GetRMS(DRV_METROLOGY_RMS_TYPE rmsId, uint32_t * rmsValue, DRV_METROLOGY_RMS_SIGN * sign)
{
    if (rmsId >= RMS_TYPE_NUM)
    {
        return false;
    }

    if (sign != NULL)
    {
        *sign = DRV_METROLOGY_GetRMSSign(rmsId);
    }

    *rmsValue = DRV_METROLOGY_GetRMSValue(rmsId);
    return true;
}

void APP_METROLOGY_SetControlByDefault(void)
{
    DRV_METROLOGY_REGS_CONTROL *pSrc;

    pSrc = DRV_METROLOGY_GetControlByDefault();
    DRV_METROLOGY_SetControl(pSrc);
}

void APP_METROLOGY_StoreMetrologyData(void)
{
    _APP_METROLOGY_StoreControlInMemory(app_metrologyData.pMetControl);
}

void APP_METROLOGY_SetConfiguration(DRV_METROLOGY_CONFIGURATION * config)
{
    DRV_METROLOGY_SetConfiguration(config);

    _APP_METROLOGY_StoreControlInMemory(app_metrologyData.pMetControl);
}

void APP_METROLOGY_StartCalibration(APP_METROLOGY_CALIBRATION * calibration)
{
    if (app_metrologyData.state == APP_METROLOGY_STATE_RUNNING)
    {
        DRV_METROLOGY_CALIBRATION_REFS * pCalibrationRefs;

        pCalibrationRefs = DRV_METROLOGY_GetCalibrationReferences();
        pCalibrationRefs->aimIA = calibration->aimIA;
        pCalibrationRefs->aimVA = calibration->aimVA;
        pCalibrationRefs->angleA = calibration->angleA;
        pCalibrationRefs->aimIB = calibration->aimIB;
        pCalibrationRefs->aimVB = calibration->aimVB;
        pCalibrationRefs->angleB = calibration->angleB;
        pCalibrationRefs->aimIC = calibration->aimIC;
        pCalibrationRefs->aimVC = calibration->aimVC;
        pCalibrationRefs->angleC = calibration->angleC;
        pCalibrationRefs->lineId = calibration->lineId;

        app_metrologyData.state = APP_METROLOGY_STATE_CHECK_CALIBRATION;
        DRV_METROLOGY_StartCalibration();
    }
}

void APP_METROLOGY_SetCalibrationCallback(DRV_METROLOGY_CALIBRATION_CALLBACK callback)
{
    app_metrologyData.pCalibrationCallback = callback;
}

size_t APP_METROLOGY_GetWaveformCaptureData(uint32_t *address)
{
    *address = (uint32_t)app_metrologyData.pMetControl->CAPTURE_ADDR;
    return (size_t)app_metrologyData.pMetControl->CAPTURE_BUFF_SIZE;
}

bool APP_METROLOGY_StartHarmonicAnalysis(uint8_t harmonicNum)
{
    if (app_metrologyData.harmonicAnalysisPending)
    {
        return false;
    }

    if (app_metrologyData.pHarmonicAnalysisCallback == NULL)
    {
        return false;
    }

    if (app_metrologyData.pHarmonicAnalysisResponse == NULL)
    {
        return false;
    }

    app_metrologyData.harmonicAnalysisPending = true;

    DRV_METROLOGY_StartHarmonicAnalysis(harmonicNum, app_metrologyData.pHarmonicAnalysisResponse);

    return true;
}

void APP_METROLOGY_SetHarmonicAnalysisCallback(DRV_METROLOGY_HARMONICS_CALLBACK callback,
        DRV_METROLOGY_HARMONICS_RMS * pHarmonicAnalysisResponse)
{
    app_metrologyData.pHarmonicAnalysisCallback = callback;
    app_metrologyData.pHarmonicAnalysisResponse = pHarmonicAnalysisResponse;
}

void APP_METROLOGY_Restart (void)
{
    DRV_METROLOGY_RESULT result;

    result = DRV_METROLOGY_Close();
    if (result == DRV_METROLOGY_SUCCESS)
    {
        app_metrologyData.state = APP_METROLOGY_STATE_INIT;
        app_metrologyData.startMode = DRV_METROLOGY_START_HARD;

        sysObj.drvMet = DRV_METROLOGY_Reinitialize((SYS_MODULE_INIT *)&drvMetrologyInitData);
    }
}

void APP_METROLOGY_SetLowPowerMode (void)
{
    DRV_METROLOGY_Close();
    SUPC_BackupModeEnter();
}

bool APP_METROLOGY_CheckPhaseEnabled (APP_METROLOGY_PHASE_ID phase)
{
    uint32_t regValue = app_metrologyData.pMetControl->FEATURE_CTRL0;

    if (regValue & phase)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*******************************************************************************
 End of File
 */
