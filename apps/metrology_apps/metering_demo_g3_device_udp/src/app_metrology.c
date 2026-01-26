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
#include "peripheral/icm/plib_icm.h"
#include "app_metrology.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

/* ICM Hash area */
uint32_t appMetrologyOutputSHA[0x10] __ALIGNED(128);

extern APP_DATALOG_QUEUE appDatalogQueue;
APP_DATALOG_QUEUE_DATA appMetrologyDatalogQueueData;

extern APP_ENERGY_QUEUE appEnergyQueue;
extern APP_EVENTS_QUEUE appEventsQueue;

extern DRV_METROLOGY_INIT drvMetrologyInitData;

const char * _met_control_desc[] =
{
  "00 STATE_CTRL",
  "01 FEATURE_CTRL",
  "02 AFE_SELECTION",
  "03 CHANNEL_MATRIX",
  "04 HARMONIC_CTRL",
  "05 METER_TYPE",
  "06 M",
  "07 N_MAX",
  "08 PULSE0_CTRL",
  "09 PULSE0_K_t",
  "10 PULSE1_CTRL",
  "11 PULSE1_K_t",
  "12 PULSE2_CTRL",
  "13 PULSE2_K_t",
  "14 SYNTH_ADDR",
  "15 CREEP_THR_P",
  "16 CREEP_THR_P_A",
  "17 CREEP_THR_P_B",
  "18 CREEP_THR_P_C",
  "19 CREEP_THR_Q",
  "20 CREEP_THR_Q_A",
  "21 CREEP_THR_Q_B",
  "22 CREEP_THR_Q_C",
  "23 CREEP_THR_I",
  "24 CREEP_THR_I_A",
  "25 CREEP_THR_I_B",
  "26 CREEP_THR_I_C",
  "27 CREEP_THR_S",
  "28 PWR_OFFS_CTRL",
  "29 PWR_OFFS_P",
  "30 PWR_OFFS_Q",
  "31 PWR_OFFS_S",
  "32 SWELL_THR_VA",
  "33 SWELL_THR_VB",
  "34 SWELL_THR_VC",
  "35 SAG_THR_VA",
  "36 SAG_THR_VB",
  "37 SAG_THR_VC",
  "38 INT_THR_VA",
  "39 INT_THR_VB",
  "40 INT_THR_VC",
  "41 RESERVED_C41",
  "42 RESERVED_C42",
  "43 RESERVED_C43",
  "44 K_IA",
  "45 K_VA",
  "46 K_IB",
  "47 K_VB",
  "48 K_IC",
  "49 K_VC",
  "50 K_IN",
  "51 K_VD",
  "52 CAL_M_IA",
  "53 CAL_M_VA",
  "54 CAL_M_IB",
  "55 CAL_M_VB",
  "56 CAL_M_IC",
  "57 CAL_M_VC",
  "58 CAL_M_IN",
  "59 CAL_M_VD",
  "60 CAL_PH_IA",
  "61 CAL_PH_VA",
  "62 CAL_PH_IB",
  "63 CAL_PH_VB",
  "64 CAL_PH_IC",
  "65 CAL_PH_VC",
  "66 CAL_PH_IN",
  "67 RESERVED_C67",
  "68 CPTR_CTRL",
  "69 CPTR_BUFF_SIZE",
  "70 CPTR_ADDR",
  "71 RESERVED_C71",
  "72 RESERVED_C72",
  "73 RESERVED_C73",
  "74 AFE_CTRL",
  "75 RESERVED_C75",
  "76 RESERVED_C76",
  "77 RESERVED_C77",
  "78 PWR_OFFS_P_A",
  "79 PWR_OFFS_P_B",
  "80 PWR_OFFS_P_C",
  "81 PWR_OFFS_Q_A",
  "82 PWR_OFFS_Q_B",
  "83 PWR_OFFS_Q_C"
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
  "13 V_A_MAX",
  "14 V_B_MAX",
  "15 V_C_MAX",
  "16 RESERVED_S16",
  "17 I_A_MAX",
  "18 I_B_MAX",
  "19 I_C_MAX",
  "20 I_Nm_MAX",
  "21 I_Ni_MAX",
  "22 FEATURES",
  "23 N_CYCLE",
  "24 RESERVED_S24",
  "25 RESERVED_S25",
  "26 PULSE0_COUNTER",
  "27 PULSE1_COUNTER",
  "28 PULSE2_COUNTER",
  "29 RESERVED_S29",
  "30 ZC_N_VA",
  "31 ZC_N_VB",
  "32 ZC_N_VC",
  "33 RESERVED_S33",
  "34 ATCAL_41_44",
  "35 ATCAL_45_48"
};

const char * _met_acc_desc[] =
{
  "00 V_A",
  "01 V_B",
  "02 V_C",
  "03 V_D",
  "04 V_A_F",
  "05 V_B_F",
  "06 V_C_F",
  "07 V_D_F",
  "08 V_AB",
  "09 V_BC",
  "10 V_CA",
  "11 V_AB_F",
  "12 V_BC_F",
  "13 V_CA_F",
  "14 RESERVED_A14",
  "15 RESERVED_A15",
  "16 RESERVED_A16",
  "17 I_A",
  "18 I_B",
  "19 I_C",
  "20 I_Ni",
  "21 I_Nm",
  "22 I_A_F",
  "23 I_B_F",
  "24 I_C_F",
  "25 I_Nmi",
  "26 I_Nm_F",
  "27 RESERVED_A27",
  "28 RESERVED_A28",
  "29 RESERVED_A29",
  "30 RESERVED_A30",
  "31 RESERVED_A31",
  "32 P_A",
  "33 P_B",
  "34 P_C",
  "35 P_A_F",
  "36 P_B_F",
  "37 P_C_F",
  "38 P_N",
  "39 P_N_F",
  "40 RESERVED_A40",
  "41 Q_A",
  "42 Q_B",
  "43 Q_C",
  "44 Q_A_F",
  "45 Q_B_F",
  "46 Q_C_F",
  "47 Q_N",
  "48 Q_N_F",
  "49 RESERVED_A49",
  "50 ACC_T0",
  "51 ACC_T1",
  "52 ACC_T2",
  "53 RESERVED_A53",
  "54 RESERVED_A54"
};

const char * _met_pcacc_desc[] =
{
  "00 CYCLE_V_A",
  "01 CYCLE_V_B",
  "02 CYCLE_V_C",
  "03 CYCLE_V_A_F",
  "04 CYCLE_V_B_F",
  "05 CYCLE_V_C_F",
  "06 CYCLE_V_AB",
  "07 CYCLE_V_BC",
  "08 CYCLE_V_CA",
  "09 CYCLE_V_AB_F",
  "10 CYCLE_V_BC_F",
  "11 CYCLE_V_CA_F",
  "12 CYCLE_I_A",
  "13 CYCLE_I_B",
  "14 CYCLE_I_C",
  "15 CYCLE_I_Nm",
  "16 CYCLE_I_A_F",
  "17 CYCLE_I_B_F",
  "18 CYCLE_I_C_F",
  "19 CYCLE_I_Nm_F",
  "20 CYCLE_P_A",
  "21 CYCLE_P_B",
  "22 CYCLE_P_C",
  "23 CYCLE_P_A_F",
  "24 CYCLE_P_B_F",
  "25 CYCLE_P_C_F",
  "26 CYCLE_P_N",
  "27 CYCLE_P_N_F",
  "28 CYCLE_Q_A",
  "29 CYCLE_Q_B",
  "30 CYCLE_Q_C",
  "31 CYCLE_Q_A_F",
  "32 CYCLE_Q_B_F",
  "33 CYCLE_Q_C_F",
  "34 CYCLE_Q_N",
  "35 CYCLE_Q_N_F"
};

const char * _met_har_desc[] =
{
  "000 I_A_m_R",
  "031 V_A_m_R",
  "062 I_B_m_R",
  "093 V_B_m_R",
  "124 I_C_m_R",
  "155 V_C_m_R",
  "186 I_N_m_R",
  "217 I_A_m_I",
  "248 V_A_m_I",
  "279 I_B_m_I",
  "310 V_B_m_I",
  "341 I_C_m_I",
  "372 V_C_m_I",
  "403 I_N_m_I",
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

static void _APP_METROLOGY_ICMHashCompletedCallback(ICM_REGION_ID regionId)
{
    if (regionId == ICM_REGION_0)
    {
        app_metrologyData.metBinHashCompleted = true;
    }
}

static void _APP_METROLOGY_ICMDigestMismatchCallback(ICM_REGION_ID regionId)
{
    if (regionId == ICM_REGION_0)
    {
        /* MCU reset. Additional actions could be done: reload metrology library without restarting MCU,
        count number of mismatches and store it lin NVM, ... */
        app_metrologyData.metBinMismatch = true;
    }
}

static void _APP_METROLOGY_IntegrationCallback(void)
{
    APP_ENERGY_QUEUE_DATA newMetrologyData;

    /* It is safe to process the callback here because it is called from Tasks */
    if (app_metrologyData.state == APP_METROLOGY_STATE_RUNNING)
    {
        // Send new Energy values to the Energy Task
        newMetrologyData.energy = DRV_METROLOGY_GetEnergyValue(true);
        newMetrologyData.Pt = DRV_METROLOGY_GetMeasureValue(MEASURE_PT, false);
        if (APP_ENERGY_SendEnergyData(&newMetrologyData) == false)
        {
            SYS_CMD_MESSAGE("ENERGY Queue is FULL!!!\r\n");
        }
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

    /* It is safe to process the callback here because it is called from Tasks */
    if (app_metrologyData.state == APP_METROLOGY_STATE_CHECK_CALIBRATION)
    {
        app_metrologyData.state = APP_METROLOGY_STATE_RUNNING;
    }
}

static void _APP_METROLOGY_HarmonicAnalysisCallback(uint32_t harmonicBitmap)
{
    if (app_metrologyData.stopHarmonicAnalysis)
    {
        app_metrologyData.harmonicAnalysisPending = false;
        DRV_METROLOGY_StopHarmonicAnalysis();
    }

    if (app_metrologyData.pHarmonicAnalysisCallback)
    {
        if (app_metrologyData.sendHarmonicsToConsole)
        {
            app_metrologyData.sendHarmonicsToConsole = false;
            app_metrologyData.pHarmonicAnalysisCallback(harmonicBitmap);
        }
    }
}

static void _APP_METROLOGY_HalfCycleCallback(void)
{
    if (app_metrologyData.state == APP_METROLOGY_STATE_RUNNING)
    {
        /* Signal Metrology thread to update events for a Half/Full Cycle */
        app_metrologyData.halfCycleFlag = true;
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
    app_metrologyData.state = APP_METROLOGY_STATE_ICM_GET_HASH;

    /* Get met bin file descriptor to check integrity (ICM) */
    app_metrologyData.metBinStartAddress = drvMetrologyInitData.binStartAddress;
    app_metrologyData.metBinSize = drvMetrologyInitData.binEndAddress - drvMetrologyInitData.binStartAddress;
    app_metrologyData.metBinHashCompleted = false;
    app_metrologyData.metBinMismatch = false;

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
    app_metrologyData.pMetPerCycleAccData = DRV_METROLOGY_GetPerCycleAccData();
    app_metrologyData.pMetHarData = DRV_METROLOGY_GetHarData();

    /* Set Callback for each metrology integration process */
    DRV_METROLOGY_IntegrationCallbackRegister(_APP_METROLOGY_IntegrationCallback);
    /* Set Callback for calibration process */
    DRV_METROLOGY_CalibrationCallbackRegister(_APP_METROLOGY_CalibrationCallback);
    /* Set Callback for harmonic analysis process */
    DRV_METROLOGY_HarmonicAnalysisCallbackRegister(_APP_METROLOGY_HarmonicAnalysisCallback);
    /* Set Callback for half cycle */
    DRV_METROLOGY_HalfCycleCallbackRegister(_APP_METROLOGY_HalfCycleCallback);

    /* Clear Harmonic Analysis Data */
    app_metrologyData.harmonicAnalysisPending = false;
    app_metrologyData.pHarmonicAnalysisCallback = NULL;
    app_metrologyData.pHarmonicAnalysisResponse = NULL;

    /* Clear Calibration Data */
    app_metrologyData.pCalibrationCallback = NULL;

    /* Initialize Flags */
    app_metrologyData.halfCycleFlag = false;
    app_metrologyData.dataFlag = false;
    app_metrologyData.eventFlagsPrev.afeEventsMask = 0;

}

/******************************************************************************
  Function:
    void APP_METROLOGY_Tasks (void)

  Remarks:
    See prototype in app_metrology.h.
 */
void APP_METROLOGY_Tasks (void)
{
    APP_EVENTS_QUEUE_DATA newEvent;
    DRV_METROLOGY_AFE_EVENTS_UNION newEventFlags;


    if (app_metrologyData.metBinMismatch)
    {
        app_metrologyData.state = APP_METROLOGY_STATE_ERROR;
    }

    /* Check the application's current state. */
    switch (app_metrologyData.state)
    {
        case APP_METROLOGY_STATE_ICM_GET_HASH:
        {
            ICM_Disable();

            ICM_SetRegionDescriptorData(ICM_REGION_0,
                                        (uint32_t *)app_metrologyData.metBinStartAddress,
                                        app_metrologyData.metBinSize);

            ICM_SetHashAreaAddress((uint32_t)appMetrologyOutputSHA);

            // Set ICM callbacks (Hash Completed))
            ICM_CallbackRegister(ICM_INTERRUPT_RHC, _APP_METROLOGY_ICMHashCompletedCallback);
            ICM_EnableInterrupt(ICM_INTERRUPT_RHC, ICM_REGION_0);

            app_metrologyData.state = APP_METROLOGY_STATE_ICM_START_MONITOR;
            app_metrologyData.metBinHashCompleted = false;

            // Compute de Hash values
            ICM_Enable();

            break;
        }

        case APP_METROLOGY_STATE_ICM_START_MONITOR:
        {
            if (app_metrologyData.metBinHashCompleted == true)
            {
                ICM_REGION_DESCRIPTOR *pDescriptor;

                ICM_Disable();

                ICM_SetRegionDescriptorData(ICM_REGION_0, (uint32_t *)IRAM1_ADDR,
                                            app_metrologyData.metBinSize);

                // Disable ICM callbacks (Hash Completed))
                ICM_CallbackRegister(ICM_INTERRUPT_RHC, NULL);
                ICM_DisableInterrupt(ICM_INTERRUPT_RHC, ICM_REGION_0);

                // Set ICM Monitor Mode
                pDescriptor = ICM_GetRegionDescriptor(ICM_REGION_0);
                pDescriptor->config.bitfield.compareMode = 1;
                pDescriptor->config.bitfield.wrap = 1;
                pDescriptor->config.bitfield.endMonitor = 0;
                pDescriptor->config.bitfield.regHashIntDis = 1;
                pDescriptor->config.bitfield.mismatchIntDis = 0;

                // Set ICM callbacks (Digest mismatch)
                ICM_CallbackRegister(ICM_INTERRUPT_RDM, _APP_METROLOGY_ICMDigestMismatchCallback);
                ICM_EnableInterrupt(ICM_INTERRUPT_RDM, ICM_REGION_0);

                ICM_SetMonitorMode(true, 15);
                ICM_EnableRegionMonitor(ICM_REGION_0);

                ICM_Enable();

                app_metrologyData.state = APP_METROLOGY_STATE_WAITING_DATALOG;
            }
            break;
        }

        case APP_METROLOGY_STATE_WAITING_DATALOG:
        {
            if (APP_DATALOG_GetStatus() == APP_DATALOG_STATE_READY)
            {
                /* Check if there are Metrology data in memory */
                if (APP_DATALOG_DataIsValid(APP_DATALOG_USER_METROLOGY))
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
                    // Check ICM status before running metrology
                    if ((ICM_GetStatus() & ICM_SR_ENABLE_Msk) == 0)
                    {
                        ICM_Enable();
                    }
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
                if (APP_DATALOG_DataIsValid(APP_DATALOG_USER_METROLOGY) == false)
                {
                    /* Metrology data does not exists. Store in NVM */
                    _APP_METROLOGY_StoreControlInMemory(app_metrologyData.pMetControl);
                }

                if (DRV_METROLOGY_Start() == DRV_METROLOGY_SUCCESS)
                {
                    // Check ICM status before running metrology
                    if ((ICM_GetStatus() & ICM_SR_ENABLE_Msk) == 0)
                    {
                        ICM_Enable();
                    }
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
            /* Wait for the Half/Full Cycle to get events. */
            if (app_metrologyData.halfCycleFlag)
            {
                app_metrologyData.halfCycleFlag = false;

                // Send new Events to the Events Task
                RTC_TimeGet(&newEvent.eventTime);
                DRV_METROLOGY_GetEventsData(&newEventFlags);
                if (newEventFlags.afeEventsMask != app_metrologyData.eventFlagsPrev.afeEventsMask)
                {
                    app_metrologyData.eventFlagsPrev.afeEventsMask = newEventFlags.afeEventsMask;
                    newEvent.eventFlags = newEventFlags.afeEvents;
                    if (APP_EVENTS_SendEventsData(&newEvent) == false)
                    {
                        SYS_CMD_MESSAGE("EVENTS Queue is FULL!!!\r\n");
                    }
                }
            }

            break;
        }

        case APP_METROLOGY_STATE_CHECK_CALIBRATION:
        {
            /* Wait for calibration end. */
            break;
        }

        /* The default state should never be executed. */
        case APP_METROLOGY_STATE_ERROR:
        {
            if (app_metrologyData.metBinMismatch)
            {
                app_metrologyData.metBinMismatch = false;
                SYS_CMD_MESSAGE("ERROR: Metrology library has been corrupted. Results are not valid.\r\n");
            }
        }
            break;

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

    pData = (uint32_t *)&app_metrologyData.configuration;
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

bool APP_METROLOGY_GetPCAccumulatorRegister(PER_CYCLE_ACCUMULATOR_REG_ID regId, uint64_t * regValue, char *regName)
{
    uint64_t *pData;

    if (regId >= PER_CYCLE_ACC_REG_NUM)
    {
        return false;
    }

    pData = (uint64_t *)app_metrologyData.pMetPerCycleAccData;
    pData += regId;
    *regValue = *pData;

    if (regName)
    {
        sprintf(regName,"%s",_met_pcacc_desc[regId]);
    }

    return true;
}

void APP_METROLOGY_CaptureHarmonicData(void)
{
    (void) memcpy((uint8_t *)&app_metrologyData.harmonicsData,
            (uint8_t *)app_metrologyData.pMetHarData, sizeof(DRV_METROLOGY_REGS_HARMONICS));

}

bool APP_METROLOGY_GetHarmonicRegister(HARMONICS_REG_ID regId, uint8_t harmonicNum, uint32_t *regValue, char *regName)
{
    if (harmonicNum >= DRV_METROLOGY_HARMONICS_MAX_ORDER)
    {
        return false;
    }

    switch(regId)
    {
        case HARMONICS_I_A_m_R_ID:
            *regValue = app_metrologyData.harmonicsData.I_A_m_R[harmonicNum];
            break;

        case HARMONICS_I_B_m_R_ID:
            *regValue = app_metrologyData.harmonicsData.I_B_m_R[harmonicNum];
            break;

        case HARMONICS_I_C_m_R_ID:
            *regValue = app_metrologyData.harmonicsData.I_C_m_R[harmonicNum];
            break;

        case HARMONICS_I_N_m_R_ID:
            *regValue = app_metrologyData.harmonicsData.I_N_m_R[harmonicNum];
            break;

        case HARMONICS_V_A_m_R_ID:
            *regValue = app_metrologyData.harmonicsData.V_A_m_R[harmonicNum];
            break;

        case HARMONICS_V_B_m_R_ID:
            *regValue = app_metrologyData.harmonicsData.V_B_m_R[harmonicNum];
            break;

        case HARMONICS_V_C_m_R_ID:
            *regValue = app_metrologyData.harmonicsData.V_C_m_R[harmonicNum];
            break;

        case HARMONICS_I_A_m_I_ID:
            *regValue = app_metrologyData.harmonicsData.I_A_m_I[harmonicNum];
            break;

        case HARMONICS_I_B_m_I_ID:
            *regValue = app_metrologyData.harmonicsData.I_B_m_I[harmonicNum];
            break;

        case HARMONICS_I_C_m_I_ID:
            *regValue = app_metrologyData.harmonicsData.I_C_m_I[harmonicNum];
            break;

        case HARMONICS_I_N_m_I_ID:
            *regValue = app_metrologyData.harmonicsData.I_N_m_I[harmonicNum];
            break;

        case HARMONICS_V_A_m_I_ID:
            *regValue = app_metrologyData.harmonicsData.V_A_m_I[harmonicNum];
            break;

        case HARMONICS_V_B_m_I_ID:
            *regValue = app_metrologyData.harmonicsData.V_B_m_I[harmonicNum];
            break;

        case HARMONICS_V_C_m_I_ID:
            *regValue = app_metrologyData.harmonicsData.V_C_m_I[harmonicNum];
            break;

        case HARMONICS_REG_NUM:
        default:
            break;
    }

    if (regName)
    {
        sprintf(regName,"%s",_met_har_desc[regId]);
    }

    return true;
}

bool APP_METROLOGY_GetMeasure(DRV_METROLOGY_MEASURE_TYPE measureId, float * value, bool perCyleMeasure)
{
    if (measureId >= MEASURE_TYPE_NUM)
    {
        return false;
    }

    *value = DRV_METROLOGY_GetMeasureValue(measureId, perCyleMeasure);
    return true;
}

void APP_METROLOGY_SetControlByDefault(void)
{
    DRV_METROLOGY_REGS_CONTROL *pSrc;

    pSrc = DRV_METROLOGY_GetControlByDefault();
    DRV_METROLOGY_SetControl(pSrc);

    memcpy(&app_metrologyData.configuration, pSrc, sizeof(DRV_METROLOGY_REGS_CONTROL));
}

void APP_METROLOGY_StoreMetrologyData(void)
{
    _APP_METROLOGY_StoreControlInMemory(app_metrologyData.pMetControl);
}

void APP_METROLOGY_StartCalibration(DRV_METROLOGY_CALIBRATION_REFS * calibration)
{
    if (app_metrologyData.state == APP_METROLOGY_STATE_RUNNING)
    {
        DRV_METROLOGY_CALIBRATION_REFS * pCalibrationRefs;

        pCalibrationRefs = DRV_METROLOGY_GetCalibrationReferences();
        *pCalibrationRefs = *calibration;

        app_metrologyData.state = APP_METROLOGY_STATE_CHECK_CALIBRATION;
        DRV_METROLOGY_StartCalibration();
    }
}

void APP_METROLOGY_SetCalibrationCallback(DRV_METROLOGY_CALIBRATION_CALLBACK callback)
{
    app_metrologyData.pCalibrationCallback = callback;
}

bool APP_METROLOGY_StartHarmonicAnalysis(uint32_t harmonicBitmap, bool singleMode)
{
    if ((app_metrologyData.harmonicAnalysisPending) && (singleMode == true))
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

    if (DRV_METROLOGY_StartHarmonicAnalysis(harmonicBitmap, app_metrologyData.pHarmonicAnalysisResponse))
    {
        app_metrologyData.harmonicAnalysisPending = true;
        app_metrologyData.stopHarmonicAnalysis = singleMode;
        app_metrologyData.sendHarmonicsToConsole = true;
        return true;
    }
    else
    {
        return false;
    }
}

void APP_METROLOGY_StopHarmonicAnalysis(void)
{
    app_metrologyData.stopHarmonicAnalysis = true;
    app_metrologyData.sendHarmonicsToConsole = true;
}

void APP_METROLOGY_SetHarmonicAnalysisCallback(DRV_METROLOGY_HARMONICS_CALLBACK callback,
        DRV_METROLOGY_HARMONICS_RMS * pHarmonicAnalysisResponse)
{
    app_metrologyData.pHarmonicAnalysisCallback = callback;
    app_metrologyData.pHarmonicAnalysisResponse = pHarmonicAnalysisResponse;
}

void APP_METROLOGY_Restart (bool reloadRegsFromMemory)
{
    DRV_METROLOGY_RESULT result;

    result = DRV_METROLOGY_Close();
    if (result == DRV_METROLOGY_SUCCESS)
    {
        if (reloadRegsFromMemory)
        {
            app_metrologyData.state = APP_METROLOGY_STATE_WAITING_DATALOG;
        }
        else
        {
            app_metrologyData.state = APP_METROLOGY_STATE_INIT;
        }
        app_metrologyData.startMode = DRV_METROLOGY_START_HARD;

        /* Disable ICM before resetting peripheral clocks */
        ICM_Disable();

        sysObj.drvMet = DRV_METROLOGY_Reinitialize((SYS_MODULE_INIT *)&drvMetrologyInitData);
    }
}

void APP_METROLOGY_SetLowPowerMode (void)
{
    DRV_METROLOGY_Close();
    SUPC_BackupModeEnter();
}

void APP_METROLOGY_StopMetrology (void)
{
    DRV_METROLOGY_Close();
}

bool APP_METROLOGY_CheckPhaseEnabled (APP_METROLOGY_PHASE_ID phase)
{
    uint32_t regValue = app_metrologyData.pMetControl->FEATURE_CTRL;

    if (regValue & phase)
    {
        return true;
    }
    else
    {
        return false;
    }
}

DRV_METROLOGY_AFE_TYPE APP_METROLOGY_GetAFEDescription(char *pDescription)
{
    return DRV_METROLOGY_GetAFEDescription(pDescription);
}

uint8_t APP_METROLOGY_GetChannelsDescription(DRV_METROLOGY_CHANNEL **pChannelDesc)
{
    *pChannelDesc = DRV_METROLOGY_GetChannelDescription();

    return DRV_METROLOGY_CHANNELS_NUMBER;
}

void APP_METROLOGY_GetAccRegData(DRV_METROLOGY_REGS_ACCUMULATORS *pData, char **pDescription)
{
    if (pData != NULL)
    {
        memcpy(pData, app_metrologyData.pMetAccData, sizeof(DRV_METROLOGY_REGS_ACCUMULATORS));
        *pDescription = (char *)_met_acc_desc;
    }
}

void APP_METROLOGY_GetPerCycleAccRegData(DRV_METROLOGY_REGS_PERCYCLE_ACC *pData, char **pDescription)
{
    if (pData != NULL)
    {
        memcpy(pData, app_metrologyData.pMetPerCycleAccData, sizeof(DRV_METROLOGY_REGS_PERCYCLE_ACC));
        *pDescription = (char *)_met_pcacc_desc;
    }
}

/*******************************************************************************
 End of File
 */
