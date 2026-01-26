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
    app_console.c

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
#include "app_console.h"
#include "driver/metrology/drv_metrology_definitions.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

/* Structure containing data to be stored in non volatile memory */
typedef struct
{
    /* Meter ID */
    char meterID[7];

} APP_CONSOLE_STORAGE_DATA;

#define CONSOLE_TASK_DELAY_MS_UNTIL_DATALOG_READY      100
#define CONSOLE_TASK_DEFAULT_DELAY_MS_BETWEEN_STATES   10
#define CONSOLE_TASK_DELAY_MS_BETWEEN_REGS_PRINT       30
#define CONSOLE_TASK_DELAY_MS_BETWEEN_HAR_REGS_PRINT   50


// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_CONSOLE_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_CONSOLE_DATA CACHE_ALIGN app_consoleData;

/* Local variable to hold a duplicate of storage data */
APP_CONSOLE_STORAGE_DATA CACHE_ALIGN app_consoleStorageData;
/* Constant conaining default value for storage data */
const APP_CONSOLE_STORAGE_DATA CACHE_ALIGN app_defaultConsoleStorageData = {{'1', '2', '3', '4', '\0', '\0', '\0'}};

/* Local storage objects */
static APP_ENERGY_ACCUMULATORS energyData;
static APP_ENERGY_MAX_DEMAND maxDemandLocalObject;
static DRV_METROLOGY_HARMONICS_RMS harmonicAnalysisRMSData[DRV_METROLOGY_HARMONICS_MAX_ORDER];
static HARMONICS_REG_ID harRegsDisplayOrder[HARMONICS_REG_NUM] = {
    HARMONICS_I_A_m_R_ID,
    HARMONICS_I_A_m_I_ID,
    HARMONICS_V_A_m_R_ID,
    HARMONICS_V_A_m_I_ID,
    HARMONICS_I_B_m_R_ID,
    HARMONICS_I_B_m_I_ID,
    HARMONICS_V_B_m_R_ID,
    HARMONICS_V_B_m_I_ID,
    HARMONICS_I_C_m_R_ID,
    HARMONICS_I_C_m_I_ID,
    HARMONICS_V_C_m_R_ID,
    HARMONICS_V_C_m_I_ID,
    HARMONICS_I_N_m_R_ID,
    HARMONICS_I_N_m_I_ID
};

/* Local Queue element to request Datalog operations */
APP_DATALOG_QUEUE_DATA datalogQueueElement;

/* Reference to datalog queue */
extern APP_DATALOG_QUEUE appDatalogQueue;

/* Local array to hold password for Commands */
#define APP_CONSOLE_MET_PWD_SIZE             6U
static char metPwd[APP_CONSOLE_MET_PWD_SIZE] = APP_CONSOLE_DEFAULT_PWD;

static char * perCycle[] = {"", " per-cycle"};

/* Local array to print the sensor type */
static const char *gConsoleSensorTypes[SENSOR_NUM_TYPE] = {"CT", "SHUNT", "ROGOWSKI", "VRD", "TEMP", "NOT USED"};

/* Local array to print the AFE type */
static char gConsoleAFEDesc[14];

/* Waveform Capture Buffer (Worst case) */
#ifdef PIC32CXMTSH_DB
#define APP_CONSOLE_MET_CAPTURE_NUM_CHN       (4U)
#else
#define APP_CONSOLE_MET_CAPTURE_NUM_CHN       (8U)
#endif
// Samples must be an integer number
// 16000 / 60Hz = 266.66 samples/cycle * 3 cycles = 800 samples
#define APP_CONSOLE_MET_CAPTURE_CHN_SAMPLES   (800UL)
#define APP_CONSOLE_MET_CAPTURE_SAMPLES       (APP_CONSOLE_MET_CAPTURE_CHN_SAMPLES * APP_CONSOLE_MET_CAPTURE_NUM_CHN)
static uint32_t metCaptureData[APP_CONSOLE_MET_CAPTURE_SAMPLES];

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************
static void _commandBUF (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandCAL(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandDAR (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandDPCAR (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandDCB (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandDCD (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandDCM (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandDCR (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandDCS (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandDCW (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandDSR (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandENC (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandENR (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandEVEC(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandEVER(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandHAR (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandHRR (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandHRRX(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandIDR (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandIDW (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandMDC (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandMDR (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandPAR (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandPARC (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandRTCR(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandRTCW(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandTOUR(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandTOUW(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandRST (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandRLD (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandDEV (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandSYN (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandCAPT (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandHELP(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);

#define APP_CONSOLE_HAR_DESC_SIZE    7
const char * _console_har_desc[APP_CONSOLE_HAR_DESC_SIZE] =
{
  "Irms_Har_A(A)",
  "Irms_Har_B(A)",
  "Irms_Har_C(A)",
  "Irms_Har_N(A)",
  "Vrms_Har_A(V)",
  "Vrms_Har_B(V)",
  "Vrms_Har_C(V)"
};

static const SYS_CMD_DESCRIPTOR appCmdTbl[]=
{
    {"BUF", _commandBUF, ": Read waveform capture data (if a parameter is used, only a 512 samples sector is returned)"},
    {"CAL", _commandCAL, ": Automatic calibration"},
    {"DAR", _commandDAR, ": Read Metrology Accumulator registers"},
    {"DPCAR", _commandDPCAR, ": Read Metrology Per Cycle Accumulator registers"},
    {"DCB", _commandDCB, ": Go to low power mode"},
    {"DCD", _commandDCD, ": Load default metrology control register values"},
    {"DCM", _commandDCM, ": Write Metrology Control several registers"},
    {"DCR", _commandDCR, ": Read Metrology Control registers"},
    {"DCS", _commandDCS, ": Save metrology constants to non volatile memory"},
    {"DCW", _commandDCW, ": Write Metrology Control register"},
    {"DSR", _commandDSR, ": Read Metrology Status registers"},
    {"ENC", _commandENC, ": Clear all energy"},
    {"ENR", _commandENR, ": Read energy"},
    {"EVEC",_commandEVEC, ": Clear all event record"},
    {"EVER",_commandEVER, ": Read single event record"},
    {"HAR", _commandHAR, ": Read harmonic register"},
    {"HRR", _commandHRR, ": Read harmonic Irms/Vrms"},
    {"HRRX", _commandHRRX, ": Extended version of HRR, using Harmonics bitmap and Start/Stop functionality"},
    {"IDR", _commandIDR, ": Read meter id"},
    {"IDW", _commandIDW, ": Write meter id (id length limited to 6 characters)"},
    {"MDC", _commandMDC, ": Clear all maxim demand and happen time"},
    {"MDR", _commandMDR, ": Read maxim demand"},
    {"PAR", _commandPAR, ": Read measure parameter"},
    {"PARC", _commandPARC, ": Read measure parameter based on per-cyle measurement"},
    {"RTCR",_commandRTCR, ": Read meter RTC"},
    {"RTCW",_commandRTCW, ": Write meter RTC"},
    {"TOUR",_commandTOUR, ": Read meter TOU"},
    {"TOUW",_commandTOUW, ": Write meter TOU"},
    {"RST", _commandRST, ": System reset"},
    {"RLD", _commandRLD, ": Reload Metrology Coprocessor"},
    {"DEV", _commandDEV, ": Read AFE and Channels configuration"},
    {"SYN", _commandSYN, ": Configure Synthesizer Function"},
    {"CAPT", _commandCAPT, ": Configure Waveform Capture Process"},
    {"HELP",_commandHELP, ": Help on commands"}
};



/*******************************************************************************
  Function:
    Timer in milliseconds for creating and waiting the delay.
 */

static bool APP_CONSOLE_TaskDelay(uint32_t ms, SYS_TIME_HANDLE* handle)
{
    // Check if there is the timer has been created and running
    if (*handle == SYS_TIME_HANDLE_INVALID)
    {
        // Create timer
        if (SYS_TIME_DelayMS(ms, handle) != SYS_TIME_SUCCESS)
        {
            return false;
        }
    }
    else
    {
        // Check timer
        if (SYS_TIME_DelayIsComplete(*handle) == true)
        {
            *handle = SYS_TIME_HANDLE_INVALID;
            return true;
        }
    }

    return false;
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************
void _getSysTimeFromMonthIndex(struct tm *time, uint8_t index)
{
    struct tm sysTime;

    RTC_TimeGet(&sysTime);

    if (sysTime.tm_mon < index)
    {
        sysTime.tm_mon += 12;
        sysTime.tm_year--;
    }
    sysTime.tm_mon -= index;

    *time = sysTime;
}

uint8_t _getMonthIndexFromSysTime(struct tm *time)
{
    struct tm sysTime;
    int8_t index;

    RTC_TimeGet(&sysTime);

    index = sysTime.tm_mon - time->tm_mon;
    if (index < 0)
    {
        index += 12;
    }

    return (uint8_t)index;
}

bool _preprocessorCallback ( const SYS_CMD_DESCRIPTOR* pCmdTbl,
        SYS_CMD_DEVICE_NODE* pCmdIO, char* cmdBuff, size_t bufSize, void* hParam)
{
    char *pBuff = cmdBuff;
    char cmdChar;
    size_t idx;

    for(idx = 0; idx < bufSize; idx++, pBuff++)
    {
        cmdChar = *pBuff;
        if ((cmdChar >= 'a') && (cmdChar <= 'z'))
        {
            // Convert upper case
            *pBuff -= 32;
        }
        else if ((cmdChar == '\t') || (cmdChar == ',') || (cmdChar == ';') || (cmdChar == '[') ||
                (cmdChar == ']') || (cmdChar == '(') || (cmdChar == ')'))
        {
            // Replace command separator
            *pBuff = ' ';
        }
    }

    return false;
}

static void _consoleReadStorage(APP_DATALOG_RESULT result)
{
    // Check result and go to corresponding state
    if (result == APP_DATALOG_RESULT_SUCCESS)
    {
        app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
    }
    else
    {
        app_consoleData.state = APP_CONSOLE_STATE_READ_STORAGE_ERROR;
    }
}

static void _monthlyEnergyCallback(struct tm * time, bool dataValid)
{
    if (!dataValid)
    {
        memset(&energyData, 0, sizeof(energyData));
    }

    app_consoleData.timeRequest = *time;
    app_consoleData.state = APP_CONSOLE_STATE_PRINT_MONTHLY_ENERGY;
}

static void _maxDemandCallback(struct tm * time, bool dataValid)
{
    if (!dataValid)
    {
        memset(&maxDemandLocalObject, 0, sizeof(maxDemandLocalObject));
    }

    app_consoleData.timeRequest = *time;
    app_consoleData.state = APP_CONSOLE_STATE_PRINT_MAX_DEMAND;
}

static void _harmonicAnalysisCallback(uint32_t harmonicBitmap)
{
    app_consoleData.harmonicBitmap = harmonicBitmap;
    app_consoleData.numRegsPending = APP_CONSOLE_HAR_DESC_SIZE;
    app_consoleData.numHarmsPending = DRV_METROLOGY_HARMONICS_MAX_ORDER;
    app_consoleData.state = APP_CONSOLE_STATE_PRINT_ALL_HARMONIC_ANALYSIS;
}

static void _calibrationCallback(bool result)
{
    app_consoleData.calibrationResult = result;
    app_consoleData.state = APP_CONSOLE_STATE_PRINT_CALIBRATION_RESULT;
}

// *****************************************************************************
// Synthesizer Functions
// *****************************************************************************
static DRV_METROLOGY_SYN_DESCRIPTOR * _addNewSyncDesc(uint8_t channel, uint16_t numSamples)
{
    DRV_METROLOGY_SYN_DESCRIPTOR **pSynDescHeader = &app_consoleData.pSynDescriptor;
    DRV_METROLOGY_SYN_DESCRIPTOR *pSynCurrentDesc = *pSynDescHeader;
    DRV_METROLOGY_SYN_DESCRIPTOR *pNewDesc;
    uint32_t *pData;

    pNewDesc = (DRV_METROLOGY_SYN_DESCRIPTOR *)OSAL_Malloc(sizeof(DRV_METROLOGY_SYN_DESCRIPTOR));
    if (pNewDesc == NULL)
    {
        return NULL;
    }
    memset(pNewDesc, 0, sizeof(DRV_METROLOGY_SYN_DESCRIPTOR));

    pData = (uint32_t *)OSAL_Malloc(numSamples << 2);
    if (pData == NULL)
    {
        // Free reserved memory for descriptor
        OSAL_Free(pNewDesc);
        return NULL;
    }
    memset(pData, 0, numSamples << 2);

    // Set new sync descriptor
    pNewDesc->pData = pData;
    pNewDesc->control.channel = channel;
    pNewDesc->control.numSamples = numSamples;
    pNewDesc->control.key = 0;
    pNewDesc->next = NULL;
    pNewDesc->prev = NULL;

    if (*pSynDescHeader == NULL)
    {
        *pSynDescHeader = pNewDesc;
    }
    else
    {
        while (pSynCurrentDesc->next != NULL)
        {
            pSynCurrentDesc = pSynCurrentDesc->next;
        }
        pSynCurrentDesc->next = pNewDesc;
        pNewDesc->prev = pSynCurrentDesc;
    }

    app_consoleData.pSynCurrentDesc = pNewDesc;
    app_consoleData.pSynCurrentData = pNewDesc->pData;

    return pNewDesc;
}

static void _freeSyncDescriptors(DRV_METROLOGY_SYN_DESCRIPTOR *pSynDescTail)
{
    while (pSynDescTail != NULL)
    {
        DRV_METROLOGY_SYN_DESCRIPTOR *tmp = pSynDescTail;

        pSynDescTail = pSynDescTail->prev;
        OSAL_Free(tmp->pData);
        OSAL_Free(tmp);
    }

    app_consoleData.pSynDescriptor = NULL;
    app_consoleData.pSynCurrentDesc = NULL;
}

// *****************************************************************************
// COMMANDS
// *****************************************************************************
static inline void _removePrompt(void)
{
    SYS_CMD_MESSAGE("\b");
}

static void _commandHELP(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    if (argc == 2)
    {
        SYS_CMD_DESCRIPTOR *pCmd;
        uint8_t idx;
        size_t len;

        // Show help for a single command
        pCmd = (SYS_CMD_DESCRIPTOR *)appCmdTbl;
        for (idx = 0; idx < app_consoleData.numCommands; idx++, pCmd++)
        {
            len = strlen(argv[1]);

            if (strncmp(pCmd->cmdStr, argv[1], len) == 0)
            {
                SYS_CMD_PRINT("%s\t%s\r\n", pCmd->cmdStr, pCmd->cmdDescr);
                break;
            }
        }

        if (idx == app_consoleData.numCommands)
        {
            SYS_CMD_MESSAGE("Command is not found.\r\n");
        }
    }
    else
    {
        app_consoleData.state = APP_CONSOLE_STATE_PRINT_HELP;
        app_consoleData.cmdNumToShowHelp = app_consoleData.numCommands;
        app_consoleData.pCmdDescToShowHelp = (SYS_CMD_DESCRIPTOR *)appCmdTbl;

        // Remove Prompt symbol
        _removePrompt();
    }

    /* Show console communication icon */
    APP_DISPLAY_SetSerialCommunication();
}

static void _commandBUF(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    size_t captureSize;
    uint8_t idxMax;
    uint8_t idx = 0xFF;

    if (argc > 2)
    {
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
        return;
    }

    if (argc == 2)
    {
        // Extract index from parameters
        idx = (uint8_t)strtol(argv[1], NULL, 10U);
    }

    captureSize = (size_t)app_consoleData.captNumSamples;
    app_consoleData.rawDataLen = captureSize;
    app_consoleData.rawData = metCaptureData;

    idxMax = (captureSize - 1) >> 9;

    if (idx != 0xFF)
    {
        if (idx > idxMax)
        {
            SYS_CMD_MESSAGE("Parameter is out of range.\r\n");
            return;
        }

        if (captureSize > 512U)
        {
            /* Check if it is the last fragment */
            if (idx == idxMax)
            {
                app_consoleData.rawDataLen = captureSize - (idx << 9);
            }
            else
            {
                app_consoleData.rawDataLen = 512;
            }
        }

        app_consoleData.rawData += (idx << 9);
    }

    app_consoleData.rawDataFlag = true;
    app_consoleData.state = APP_CONSOLE_STATE_PRINT_WAVEFORM_DATA;

    /* Show console communication icon */
    APP_DISPLAY_SetSerialCommunication();

}

static bool _getCalibrationANREF(char * argv, unsigned int * value)
{
    char *p;
 
    if (strncmp(argv, "ANREF", 5) == 0) {
        // Get substring after '=' char
        p = strstr(argv, "=");
        if (p != NULL) {
            // Advance ptr to ignore '=' and extract value
            p++;
            if (strcmp(p, "VA") == 0) {
                *value = DOMINANT_V_PHASE_A;
                return true;
            } else if (strcmp(p, "VB") == 0) {
                *value = DOMINANT_V_PHASE_B;
                return true;
            } else if (strcmp(p, "VC") == 0) {
                *value = DOMINANT_V_PHASE_C;
                return true;
            }
        }
    }

    return false;
}

static bool _getCalibrationValue(char * argv, char * substring, double * value)
{
    char *p;

    if (strncmp(argv, substring, strlen(substring)) == 0) {
        // Get substring after '=' char
        p = strstr(argv, "=");
        if (p != NULL) {
            // Advance ptr to ignore '=' and extract value
            p++;
            *value = strtod(p, NULL);
            return true;
        }
    }

    return false;
}

static void _commandCAL(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    DRV_METROLOGY_CALIBRATION_REFS newCalibration = {0};
    uint8_t argIndex;
    unsigned int vRefPhaseN;
    bool parseError = false;

    if (argc < 2) {
        SYS_CMD_MESSAGE("Unsupported Command !\r\n");
        return;
    }

    newCalibration.calMask.vRefPhaseN = DOMINANT_V_DYNAMIC;
    for(argIndex = 1; argIndex < argc; argIndex++)
    {
        char *pArgv = argv[argIndex];

        if(_getCalibrationValue(pArgv, "UA", &newCalibration.va) == true)
        {
            newCalibration.calMask.magnitudeVa = 1;
            continue;
        }
        else if(_getCalibrationValue(pArgv, "IA", &newCalibration.ia) == true)
        {
            newCalibration.calMask.magnitudeIa = 1;
            continue;
        }
        else if(_getCalibrationValue(pArgv, "AA", &newCalibration.aa) == true)
        {
            newCalibration.calMask.anglePhaseA = 1;
            continue;
        }
        else if(_getCalibrationValue(pArgv, "UB", &newCalibration.vb) == true)
        {
            newCalibration.calMask.magnitudeVb = 1;
            continue;
        }
        else if(_getCalibrationValue(pArgv, "IB", &newCalibration.ib) == true)
        {
            newCalibration.calMask.magnitudeIb = 1;
            continue;
        }
        else if(_getCalibrationValue(pArgv, "AB", &newCalibration.ab) == true)
        {
            newCalibration.calMask.anglePhaseB = 1;
            continue;
        }
        else if(_getCalibrationValue(pArgv, "UC", &newCalibration.vc) == true)
        {
            newCalibration.calMask.magnitudeVc = 1;
            continue;
        }
        else if(_getCalibrationValue(pArgv, "IC", &newCalibration.ic) == true)
        {
            newCalibration.calMask.magnitudeIc = 1;
            continue;
        }
        else if(_getCalibrationValue(pArgv, "AC", &newCalibration.ac) == true)
        {
            newCalibration.calMask.anglePhaseC = 1;
            continue;
        }
        else if(_getCalibrationValue(pArgv, "IN", &newCalibration.in) == true)
        {
            newCalibration.calMask.magnitudeIn = 1;
            continue;
        }
        else if(_getCalibrationANREF(pArgv, &vRefPhaseN) == true)
        {
            newCalibration.calMask.vRefPhaseN = vRefPhaseN;
            continue;
        }
        else if(_getCalibrationValue(pArgv, "AN", &newCalibration.an) == true)
        {
            newCalibration.calMask.anglePhaseN = 1;
            continue;
        }
        else if(_getCalibrationValue(pArgv, "VD", &newCalibration.vd) == true)
        {
            newCalibration.calMask.magnitudeVd = 1;
            continue;
        }
        else if(_getCalibrationValue(pArgv, "AVAB", &newCalibration.avab) == true)
        {
            newCalibration.calMask.angleVAB = 1;
            continue;
        }
        else if(_getCalibrationValue(pArgv, "AVAC", &newCalibration.avac) == true)
        {
            newCalibration.calMask.angleVAC = 1;
            continue;
        }
        else
        {
            parseError = true;
        }
    }

    if (parseError) {
        SYS_CMD_MESSAGE("Unsupported Command !\r\n");
    }
    else
    {
        SYS_CMD_MESSAGE("Calibrating...\r\n");
        APP_METROLOGY_StartCalibration(&newCalibration);
    }
}

static void _commandDAR(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    uint8_t idx;

    if (argc == 1)
    {
        // Read all metrology accumulator registers
        app_consoleData.accumRegToRead = 0;
        app_consoleData.state = APP_CONSOLE_STATE_READ_ALL_ACCUM_REGS;

        // Capture all accumulator registers
        APP_METROLOGY_GetAccRegData(&app_consoleData.metAccRegs, &app_consoleData.pRegDescription);

        /* Show console communication icon */
        APP_DISPLAY_SetSerialCommunication();
    }
    else if (argc == 2)
    {
        // Extract register index from parameters
        idx = (uint8_t)strtol(argv[1], NULL, 10);
        if (idx < ACCUMULATOR_REG_NUM)
        {
            // Read register value
            app_consoleData.accumRegToRead = idx;
            app_consoleData.state = APP_CONSOLE_STATE_READ_ACCUM_REG;

            /* Show console communication icon */
            APP_DISPLAY_SetSerialCommunication();
        }
        else
        {
            // Invalid index
            SYS_CMD_MESSAGE("Invalid register index\r\n");
        }
    }
    else
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
    }
}

static void _commandDPCAR(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    uint8_t idx;

    if (argc == 1)
    {
        // Read all metrology accumulator registers
        app_consoleData.accumRegToRead = 0;
        app_consoleData.state = APP_CONSOLE_STATE_READ_ALL_PC_ACCUM_REGS;

        // Capture all per cycle accumulator registers
        APP_METROLOGY_GetPerCycleAccRegData(&app_consoleData.metPerCycleAccRegs, &app_consoleData.pRegDescription);

        /* Show console communication icon */
        APP_DISPLAY_SetSerialCommunication();
    }
    else if (argc == 2)
    {
        // Extract register index from parameters
        idx = (uint8_t)strtol(argv[1], NULL, 10);
        if (idx < PER_CYCLE_ACC_REG_NUM)
        {
            // Read register value
            app_consoleData.accumRegToRead = idx;
            app_consoleData.state = APP_CONSOLE_STATE_READ_PC_ACCUM_REG;

            /* Show console communication icon */
            APP_DISPLAY_SetSerialCommunication();
        }
        else
        {
            // Invalid index
            SYS_CMD_MESSAGE("Invalid register index\r\n");
        }
    }
    else
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
    }
}

static void _commandDCB(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    if (argc == 1)
    {
        app_consoleData.state = APP_CONSOLE_STATE_LOW_POWER_MODE;

        /* Show console communication icon */
        APP_DISPLAY_SetSerialCommunication();
    }
    else
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
    }
}

static void _commandDCD(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    if (argc == 1)
    {
        SYS_CMD_MESSAGE("Load Default Is Ok !\r\n");
        // Set default control register values
        APP_METROLOGY_SetControlByDefault();

        /* Show console communication icon */
        APP_DISPLAY_SetSerialCommunication();
    }
    else
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
    }
}

static void _commandDCM(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    uint8_t i;
    uint8_t numRegsToWrite;
    char *p;
    bool parseError = false;

    if (argc > 1)
    {
        // Each argument includes reg index and value
        numRegsToWrite = (argc - 1);
        for (i = 0; i < numRegsToWrite; i++)
        {
            // Extract register index and value from argument
            p = argv[i + 1];
            app_consoleData.regsToModify[i].index = (uint8_t)strtol(p, NULL, 10);
            // Look for ":" char and advance to next char
            p = strstr(p, ":");
            if (p != NULL)
            {
                p++;
                app_consoleData.regsToModify[i].value = (uint32_t)strtoul(p, NULL, 16);
            }
            else
            {
                SYS_CMD_MESSAGE("Unsupported Command !\r\n");
                parseError = true;
                break;
            }
        }

        if (!parseError)
        {
            // Write invalid values after last, to later detect it
            app_consoleData.regsToModify[i].index = 0xFF;
            app_consoleData.regsToModify[i].value = 0xFFFFFFFF;
            // Go to corresponding state
            app_consoleData.state = APP_CONSOLE_STATE_WRITE_CONTROL_REG;

            /* Show console communication icon */
            APP_DISPLAY_SetSerialCommunication();
        }
    }
    else
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
    }
}

static void _commandDCR(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    uint8_t idx;

    if (argc == 1)
    {
        // Read all metrology control registers
        app_consoleData.ctrlRegToRead = 0;
        app_consoleData.state = APP_CONSOLE_STATE_READ_ALL_CONTROL_REGS;
    }
    else if (argc == 2)
    {
        // Extract register index from parameters
        idx = (uint8_t)strtol(argv[1], NULL, 10);
        if (idx < CONTROL_REG_NUM)
        {
            // Read register value
            app_consoleData.ctrlRegToRead = idx;
            app_consoleData.state = APP_CONSOLE_STATE_READ_CONTROL_REG;

            /* Show console communication icon */
            APP_DISPLAY_SetSerialCommunication();
        }
        else
        {
            // Invalid index
            SYS_CMD_MESSAGE("Invalid register index\r\n");
        }
    }
    else
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
    }
}

static void _commandDCS(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    if (argc == 1)
    {
        SYS_CMD_MESSAGE("Save Data Is Ok !\r\n");
        // Save Metrology Constants and configuration settings to NVM
        APP_METROLOGY_StoreMetrologyData();

        /* Show console communication icon */
        APP_DISPLAY_SetSerialCommunication();
    }
    else
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
    }
}

static void _commandDCW(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    uint8_t idx;
    uint32_t regValue;

    if (argc == 3)
    {
        // Extract register index from parameters
        idx = (uint8_t)strtol(argv[1], NULL, 10);
        if (idx < CONTROL_REG_NUM)
        {
            // Extract register value
            regValue = (uint32_t)strtoul(argv[2], NULL, 16);
            // Write register value
            if (APP_METROLOGY_SetControlRegister((CONTROL_REG_ID)idx, regValue))
            {
                // Show response on console
                SYS_CMD_MESSAGE("Set Is Ok !\r\n");

                /* Show console communication icon */
                APP_DISPLAY_SetSerialCommunication();
            }
            else
            {
                // Cannot write register
                SYS_CMD_PRINT("Could not write register %02d\r\n", idx);
            }
        }
        else
        {
            // Invalid index
            SYS_CMD_MESSAGE("Invalid register index\r\n");
        }
    }
    else
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
    }
}

static void _commandDSR(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    uint8_t idx;

    if (argc == 1)
    {
        // Read all metrology status registers
        app_consoleData.statusRegToRead = 0;
        app_consoleData.state = APP_CONSOLE_STATE_READ_ALL_STATUS_REGS;

        /* Show console communication icon */
        APP_DISPLAY_SetSerialCommunication();
    }
    else if (argc == 2)
    {
        // Extract register index from parameters
        idx = (uint8_t)strtol(argv[1], NULL, 10);
        if (idx < STATUS_REG_NUM)
        {
            // Read register value
            app_consoleData.statusRegToRead = idx;
            app_consoleData.state = APP_CONSOLE_STATE_READ_STATUS_REG;

            /* Show console communication icon */
            APP_DISPLAY_SetSerialCommunication();
        }
        else
        {
            // Invalid index
            SYS_CMD_MESSAGE("Invalid register index\r\n");
        }
    }
    else
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
    }
}

static void _commandENC(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    if (argc == 2)
    {
        // Check password from parameters
        if (strncmp(argv[1], metPwd, APP_CONSOLE_MET_PWD_SIZE) == 0)
        {
            // Correct password, Clear Energy records
            APP_ENERGY_ClearEnergy(true);
            // Show response on console
            SYS_CMD_MESSAGE("Clear Energy is ok !\r\n");

            /* Show console communication icon */
            APP_DISPLAY_SetSerialCommunication();
        }
        else
        {
            // Invalid password
            SYS_CMD_MESSAGE("Invalid password\r\n");
        }
    }
    else
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
    }
}

static void _commandENR(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    uint8_t monthIndex;

    if (argc > 2)
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
    }

    if (argc == 2) {
        // Extract month index from parameters
        monthIndex = (uint8_t)strtol(argv[1], NULL, 10);
        monthIndex %= 12;
        app_consoleData.requestCounter = 1;
    }
    else
    {
        monthIndex = 0;
        // Start process to get full Monthly energy data
        app_consoleData.requestCounter = 12;
    }

    // Get SysTime
    _getSysTimeFromMonthIndex(&app_consoleData.sysTime, monthIndex);

    // Get monthly energy from energy app
    if (APP_ENERGY_GetMonthEnergy(&app_consoleData.sysTime) == false)
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param\r\n");
    }
    else
    {
        /* Show console communication icon */
        APP_DISPLAY_SetSerialCommunication();
    }
    // Response will be provided on _monthlyEnergyCallback function
}

static void _commandEVEC(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    if (argc == 2)
    {
        // Check password from parameters
        if (strncmp(argv[1], metPwd, APP_CONSOLE_MET_PWD_SIZE) == 0)
        {
            APP_EVENTS_ClearEvents();
            // Show response on console
            SYS_CMD_MESSAGE("Clear All Event is ok !\r\n");

            /* Show console communication icon */
            APP_DISPLAY_SetSerialCommunication();
        }
        else
        {
            // Invalid password
            SYS_CMD_MESSAGE("Invalid password\r\n");
        }
    }
    else
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
    }
}

static void _commandEVER(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    app_consoleData.eventIdRequest = EVENT_INVALID_ID;

    if (argc == 3)
    {
        // Extract event id from parameters
        if (strcmp(argv[1], "UASAG") == 0)
        {
            app_consoleData.eventIdRequest = SAG_UA_EVENT_ID;
        }
        else if (strcmp(argv[1], "UBSAG") == 0)
        {
            app_consoleData.eventIdRequest = SAG_UB_EVENT_ID;
        }
        else if (strcmp(argv[1], "UCSAG") == 0)
        {
            app_consoleData.eventIdRequest = SAG_UC_EVENT_ID;
        }
        else if (strcmp(argv[1], "UASWELL") == 0)
        {
            app_consoleData.eventIdRequest = SWELL_UA_EVENT_ID;
        }
        else if (strcmp(argv[1], "UBSWELL") == 0)
        {
            app_consoleData.eventIdRequest = SWELL_UB_EVENT_ID;
        }
        else if (strcmp(argv[1], "UCSWELL") == 0)
        {
            app_consoleData.eventIdRequest = SWELL_UC_EVENT_ID;
        }
        else if (strcmp(argv[1], "PA") == 0)
        {
            app_consoleData.eventIdRequest = POW_PA_EVENT_ID;
        }
        else if (strcmp(argv[1], "PB") == 0)
        {
            app_consoleData.eventIdRequest = POW_PB_EVENT_ID;
        }
        else if (strcmp(argv[1], "PC") == 0)
        {
            app_consoleData.eventIdRequest = POW_PC_EVENT_ID;
        }

        if (app_consoleData.eventIdRequest != EVENT_INVALID_ID)
        {
            // Extract last times from parameters
            app_consoleData.eventLastTimeRequest = (uint8_t)strtol(argv[2], NULL, 10);

            app_consoleData.state = APP_CONSOLE_STATE_PRINT_EVENT;

            /* Show console communication icon */
            APP_DISPLAY_SetSerialCommunication();
        }
        else
        {
            // Invalid Command
            SYS_CMD_MESSAGE("Unsupported Command !\r\n");
        }
    }
    else
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
    }
}

static void _commandHAR(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    uint8_t idx;

    if (argc == 1)
    {
        APP_METROLOGY_CaptureHarmonicData();

        // Read all metrology harmonics registers
        app_consoleData.harNumToRead = 0;
        app_consoleData.numRegsPending = HARMONICS_REG_NUM;
        app_consoleData.numHarmsPending = DRV_METROLOGY_HARMONICS_MAX_ORDER;
        app_consoleData.state = APP_CONSOLE_STATE_READ_ALL_HARMONICS_REGS;
    }
    else if (argc == 2)
    {
        // Extract register index from parameters
        idx = (uint8_t)strtol(argv[1], NULL, 10);
        if (idx < DRV_METROLOGY_HARMONICS_MAX_ORDER)
        {
            APP_METROLOGY_CaptureHarmonicData();

            // Read register value
            app_consoleData.harNumToRead = idx;
            app_consoleData.numRegsPending = HARMONICS_REG_NUM;
            app_consoleData.state = APP_CONSOLE_STATE_READ_HARMONIC_REGS;

            /* Show console communication icon */
            APP_DISPLAY_SetSerialCommunication();
        }
        else
        {
            // Invalid index
            SYS_CMD_MESSAGE("Invalid harmonic order\r\n");
        }
    }
    else
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
    }
}

static void _commandHRR(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    uint8_t num = 0xFF;
    uint32_t harmonicBitmap = 0xFFFFFFFF;

    if (argc == 1)
    {
        harmonicBitmap = 0x7FFFFFFF;
        num = 0;
    }
    else if (argc == 2)
    {
        // Extract harmonic number from parameters
        num = (uint8_t)strtol(argv[1], NULL, 10);
        if (num == 0)
        {
            harmonicBitmap = 0x7FFFFFFF;
        }
        else
        {
            // Turn into bitmap only if valid number
            if (num <= DRV_METROLOGY_HARMONICS_MAX_ORDER)
            {
                harmonicBitmap = 1 << (num - 1);
            }
        }
    }

    if (num != 0xFF)
    {
        if (num <= DRV_METROLOGY_HARMONICS_MAX_ORDER)
        {
            // Set harmonics calculation mode on metrology driver
            if (APP_METROLOGY_StartHarmonicAnalysis(harmonicBitmap, true) == false)
            {
                // Analysis already running
                SYS_CMD_MESSAGE("Previous harmonic analysis is running\r\n");
            }
            else
            {
                /* Show console communication icon */
                APP_DISPLAY_SetSerialCommunication();
            }
            // Response will be provided on _harmonicAnalysisCallback function
        }
        else
        {
            // Incorrect harmonic order
            SYS_CMD_MESSAGE("Invalid harmonic order\r\n");
        }
    }
    else
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
    }
}

static void _commandHRRX(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    uint8_t num = 0xFF;
    uint32_t harmonicBitmap = 0xFFFFFFFF;

    if (argc == 2)
    {
        // Extract Start/Stop from parameters
        num = (uint8_t)strtol(argv[1], NULL, 10);
        if (num == 1)
        {
            // Start command without bitmap argument, analyse all harmonics
            harmonicBitmap = 0x7FFFFFFF;
        }
        else if (num == 0)
        {
            // Stop command
            harmonicBitmap = 0x7FFFFFFF;
        }
        else
        {
            // Wrong command
            num = 0xFF;
        }
    }
    else if (argc == 3)
    {
        // This must be a Start command
        num = (uint8_t)strtol(argv[1], NULL, 10);
        if (num == 1)
        {
            // Extract bitmap from parameters
            harmonicBitmap = (uint32_t)strtoul(argv[2], NULL, 16);
        }
    }

    if ((num == 0xFF) && (harmonicBitmap == 0xFFFFFFFF))
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
    }
    else if ((num > 1) || (harmonicBitmap == 0xFFFFFFFF))
    {
        // Incorrect parameter format
        SYS_CMD_MESSAGE("Incorrect param format\r\n");
    }
    else
    {
        // Correct command
        if (num == 1)
        {
            // Start command
            // Set harmonics calculation mode on metrology driver
            if (APP_METROLOGY_StartHarmonicAnalysis(harmonicBitmap, false) == false)
            {
                // Analysis already running
                SYS_CMD_MESSAGE("Previous harmonic analysis is running\r\n");
            }
            else
            {
                /* Show console communication icon */
                APP_DISPLAY_SetSerialCommunication();
            }
            // Response will be provided on _harmonicAnalysisCallback function
        }
        else if (num == 0)
        {
            // Stop command
            APP_METROLOGY_StopHarmonicAnalysis();
        }
    }
}

static void _commandIDR(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    if (argc == 1)
    {
        // Read Meter ID
        app_consoleData.state = APP_CONSOLE_STATE_READ_METER_ID;

        /* Show console communication icon */
        APP_DISPLAY_SetSerialCommunication();
    }
    else
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
    }
}

static void _commandIDW(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    if (argc == 3)
    {
        // Check password from parameters
        if (strncmp(argv[1], metPwd, APP_CONSOLE_MET_PWD_SIZE) == 0)
        {
            // Correct password, write Meter ID
            memcpy(&app_consoleStorageData.meterID, argv[2], sizeof(app_consoleStorageData.meterID));
            // Build queue element to write it to storage
            datalogQueueElement.userId = APP_DATALOG_USER_CONSOLE;
            datalogQueueElement.operation = APP_DATALOG_WRITE;
            datalogQueueElement.endCallback = NULL;
            datalogQueueElement.date.year = APP_DATALOG_INVALID_YEAR; /* Not used */
            datalogQueueElement.date.month = APP_DATALOG_INVALID_MONTH; /* Not used */
            datalogQueueElement.dataLen = sizeof(app_consoleStorageData);
            datalogQueueElement.pData = (uint8_t*)&app_consoleStorageData;
            // Put it in queue
            APP_DATALOG_SendDatalogData(&datalogQueueElement);
            SYS_CMD_MESSAGE("Set Meter ID is Ok\r\n");

            /* Show console communication icon */
            APP_DISPLAY_SetSerialCommunication();
        }
        else
        {
            // Invalid password
            SYS_CMD_MESSAGE("Invalid password\r\n");
        }
    }
    else
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
    }
}

static void _commandMDC(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    if (argc == 2)
    {
        // Check password from parameters
        if (strncmp(argv[1], metPwd, APP_CONSOLE_MET_PWD_SIZE) == 0)
        {
            // Correct password, Clear Max Demand records
            APP_ENERGY_ClearMaxDemand(true);
            // Show response on console
            SYS_CMD_MESSAGE("Clear MaxDemand is ok !\r\n");

            /* Show console communication icon */
            APP_DISPLAY_SetSerialCommunication();
        }
        else
        {
            // Invalid password
            SYS_CMD_MESSAGE("Invalid password\r\n");
        }
    }
    else
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
    }
}

static void _commandMDR(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    uint8_t monthIndex;

    if (argc >2)
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
    }

    if (argc == 2) {
        // Extract month index from parameters
        monthIndex = (uint8_t)strtol(argv[1], NULL, 10);
        monthIndex %= 12;
        app_consoleData.requestCounter = 1;
    }
    else
    {
        monthIndex = 0;
        // Start process to get full Monthly energy data
        app_consoleData.requestCounter = 12;
    }

    // Get SysTime
    _getSysTimeFromMonthIndex(&app_consoleData.sysTime, monthIndex);

    // Get monthly energy from energy app
    if (APP_ENERGY_GetMonthMaxDemand(&app_consoleData.sysTime) == false)
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param\r\n");
    }
    else
    {
        /* Show console communication icon */
        APP_DISPLAY_SetSerialCommunication();
    }
    // Response will be provided on _maxDemandCallback function
}

static void _commandPAR(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    bool wakeup = false;

    if (argc == 2)
    {
        app_consoleData.perCyclePar = 0;
        // Extract data to retrieve from parameters
        if (strcmp(argv[1], "U") == 0)
        {
            app_consoleData.state = APP_CONSOLE_STATE_PRINT_VOLTAGE;
            wakeup = true;
        }
        else if (strcmp(argv[1], "I") == 0)
        {
            app_consoleData.state = APP_CONSOLE_STATE_PRINT_CURRENT;
            wakeup = true;
        }
        else if (strcmp(argv[1], "P") == 0)
        {
            app_consoleData.state = APP_CONSOLE_STATE_PRINT_ACTIVE_POWER;
            wakeup = true;
        }
        else if (strcmp(argv[1], "Q") == 0)
        {
            app_consoleData.state = APP_CONSOLE_STATE_PRINT_REACTIVE_POWER;
            wakeup = true;
        }
        else if (strcmp(argv[1], "S") == 0)
        {
            app_consoleData.state = APP_CONSOLE_STATE_PRINT_APARENT_POWER;
            wakeup = true;
        }
        else if (strcmp(argv[1], "UF") == 0)
        {
            app_consoleData.state = APP_CONSOLE_STATE_PRINT_FUNDAMENTAL_VOLTAGE;
            wakeup = true;
        }
        else if (strcmp(argv[1], "IF") == 0)
        {
            app_consoleData.state = APP_CONSOLE_STATE_PRINT_FUNDAMENTAL_CURRENT;
            wakeup = true;
        }
        else if (strcmp(argv[1], "PF") == 0)
        {
            app_consoleData.state = APP_CONSOLE_STATE_PRINT_FUNDAMENTAL_ACTIVE_POWER;
            wakeup = true;
        }
        else if (strcmp(argv[1], "QF") == 0)
        {
            app_consoleData.state = APP_CONSOLE_STATE_PRINT_FUNDAMENTAL_REACTIVE_POWER;
            wakeup = true;
        }
        else if (strcmp(argv[1], "SF") == 0)
        {
            app_consoleData.state = APP_CONSOLE_STATE_PRINT_FUNDAMENTAL_APARENT_POWER;
            wakeup = true;
        }
        else if (strcmp(argv[1], "F") == 0)
        {
            app_consoleData.state = APP_CONSOLE_STATE_PRINT_FREQUENCY;
            wakeup = true;
        }
        else if (strcmp(argv[1], "FT") == 0)
        {
            app_consoleData.state = APP_CONSOLE_STATE_PRINT_TOTAL_FREQUENCY;
            wakeup = true;
        }
        else if (strcmp(argv[1], "A") == 0)
        {
            app_consoleData.state = APP_CONSOLE_STATE_PRINT_ANGLE;
            wakeup = true;
        }
        else
        {
            // Invalid Command
            SYS_CMD_MESSAGE("Unsupported Command !\r\n");
        }

        if (wakeup)
        {
            /* Show console communication icon */
            APP_DISPLAY_SetSerialCommunication();
        }
    }
    else
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
    }
}

static void _commandPARC(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    bool wakeup = false;

    if (argc == 2)
    {
        app_consoleData.perCyclePar = 1;
        // Extract data to retrieve from parameters
        if (strcmp(argv[1], "U") == 0)
        {
            app_consoleData.state = APP_CONSOLE_STATE_PRINT_VOLTAGE;
            wakeup = true;
        }
        else if (strcmp(argv[1], "I") == 0)
        {
            app_consoleData.state = APP_CONSOLE_STATE_PRINT_CURRENT;
            wakeup = true;
        }
        else if (strcmp(argv[1], "P") == 0)
        {
            app_consoleData.state = APP_CONSOLE_STATE_PRINT_ACTIVE_POWER;
            wakeup = true;
        }
        else if (strcmp(argv[1], "Q") == 0)
        {
            app_consoleData.state = APP_CONSOLE_STATE_PRINT_REACTIVE_POWER;
            wakeup = true;
        }
        else if (strcmp(argv[1], "S") == 0)
        {
            app_consoleData.state = APP_CONSOLE_STATE_PRINT_APARENT_POWER;
            wakeup = true;
        }
        else if (strcmp(argv[1], "UF") == 0)
        {
            app_consoleData.state = APP_CONSOLE_STATE_PRINT_FUNDAMENTAL_VOLTAGE;
            wakeup = true;
        }
        else if (strcmp(argv[1], "IF") == 0)
        {
            app_consoleData.state = APP_CONSOLE_STATE_PRINT_FUNDAMENTAL_CURRENT;
            wakeup = true;
        }
        else if (strcmp(argv[1], "PF") == 0)
        {
            app_consoleData.state = APP_CONSOLE_STATE_PRINT_FUNDAMENTAL_ACTIVE_POWER;
            wakeup = true;
        }
        else if (strcmp(argv[1], "QF") == 0)
        {
            app_consoleData.state = APP_CONSOLE_STATE_PRINT_FUNDAMENTAL_REACTIVE_POWER;
            wakeup = true;
        }
        else if (strcmp(argv[1], "SF") == 0)
        {
            app_consoleData.state = APP_CONSOLE_STATE_PRINT_FUNDAMENTAL_APARENT_POWER;
            wakeup = true;
        }
        else if (strcmp(argv[1], "F") == 0)
        {
            app_consoleData.state = APP_CONSOLE_STATE_PRINT_FREQUENCY;
            wakeup = true;
        }
        else if (strcmp(argv[1], "FT") == 0)
        {
            app_consoleData.state = APP_CONSOLE_STATE_PRINT_TOTAL_FREQUENCY;
            wakeup = true;
        }
        else if (strcmp(argv[1], "A") == 0)
        {
            app_consoleData.state = APP_CONSOLE_STATE_PRINT_ANGLE;
            wakeup = true;
        }
        else
        {
            // Invalid Command
            SYS_CMD_MESSAGE("Unsupported Command !\r\n");
        }

        if (wakeup)
        {
            /* Show console communication icon */
            APP_DISPLAY_SetSerialCommunication();
        }
    }
    else
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
    }
}

static void _commandRTCR(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    if (argc == 1)
    {
        // Read RTC
        app_consoleData.state = APP_CONSOLE_STATE_READ_RTC;

        /* Show console communication icon */
        APP_DISPLAY_SetSerialCommunication();
    }
    else
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
    }
}

static void _commandRTCW(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    if (argc == 5)
    {
        if (strncmp(argv[1], metPwd, APP_CONSOLE_MET_PWD_SIZE) == 0)
        {
            char *p;

            // Get Date
            p = argv[2];
            // Year
            app_consoleData.sysTime.tm_year = (uint8_t)strtol(p, NULL, 10) + 2000 - 1900;
            // Look for "-" char and advance to next char
            p = strstr(p, "-");
            if (p != NULL)
            {
                p++;
                // Month
                app_consoleData.sysTime.tm_mon = (uint8_t)strtol(p, NULL, 10) - 1;
                // Look for "-" char and advance to next char
                p = strstr(p, "-");
                if (p != NULL)
                {
                    p++;
                    // Day
                    app_consoleData.sysTime.tm_mday = (uint8_t)strtol(p, NULL, 10);
                }
            }

            // Get Week Day
            p = argv[3];
            app_consoleData.sysTime.tm_wday = (uint8_t)strtol(p, NULL, 10) - 1;

            // Get Time
            p = argv[4];
            // Hour
            app_consoleData.sysTime.tm_hour = (uint8_t)strtol(p, NULL, 10);
            // Look for ":" char and advance to next char
            p = strstr(p, ":");
            if (p != NULL)
            {
                p++;
                // Minute
                app_consoleData.sysTime.tm_min = (uint8_t)strtol(p, NULL, 10);
                // Look for ":" char and advance to next char
                p = strstr(p, ":");
                if (p != NULL)
                {
                    p++;
                    // Second
                    app_consoleData.sysTime.tm_sec = (uint8_t)strtol(p, NULL, 10);
                }
            }

            if (RTC_TimeSet(&app_consoleData.sysTime))
            {
                // Build queue element to write it to storage
                datalogQueueElement.userId = APP_DATALOG_USER_RTC;
                datalogQueueElement.operation = APP_DATALOG_WRITE;
                datalogQueueElement.endCallback = NULL;
                datalogQueueElement.date.year = APP_DATALOG_INVALID_YEAR; /* Not used */
                datalogQueueElement.date.month = APP_DATALOG_INVALID_MONTH; /* Not used */
                datalogQueueElement.dataLen = sizeof(struct tm);
                datalogQueueElement.pData = (uint8_t*)&app_consoleData.sysTime;
                // Put it in queue
                APP_DATALOG_SendDatalogData(&datalogQueueElement);

                SYS_CMD_MESSAGE("Set RTC is ok!\r\n");

                /* Show console communication icon */
                APP_DISPLAY_SetSerialCommunication();
            }
            else
            {
                SYS_CMD_MESSAGE("Unsupported Command !\r\n");
            }
        }
        else
        {
            SYS_CMD_MESSAGE("Password Error !\r\n");
        }
    }
    else
    {
        SYS_CMD_MESSAGE("Unsupported Command !\r\n");
    }
}

static void _commandTOUR(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    if (argc == 1)
    {
        // Go to state to read TOU
        app_consoleData.state = APP_CONSOLE_STATE_READ_TOU;

        /* Show console communication icon */
        APP_DISPLAY_SetSerialCommunication();
    }
    else
    {
        // Incorrect param number
        SYS_CMD_MESSAGE("Unsupported Command !\r\n");
    }
}

static void _commandTOUW(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    char *p;
    APP_ENERGY_TOU_TIME_ZONE timeZone[APP_ENERGY_TOU_MAX_ZONES];
    uint8_t idx, argIdx;
    bool parseError = false;

    if ((argc > 3) && ((argc - 2) % 2 == 0))
    {
        // Check password from parameters
        if (strncmp(argv[1], metPwd, APP_CONSOLE_MET_PWD_SIZE) == 0)
        {
             // Correct password, write TOW
             for (idx = 0; idx < APP_ENERGY_TOU_MAX_ZONES; idx++)
             {
                // Check whether there are arguments left to write
                argIdx = ((idx << 1) + 2);
                if (argc > argIdx)
                {
                    // Extract hour, minute and rate
                    p = argv[argIdx];
                    // Extract hour from argument
                    timeZone[idx].hour = (uint8_t)strtol(p, NULL, 10);
                    // Look for ":" char and advance to next char
                    p = strstr(p, ":");
                    if (p != NULL)
                    {
                        p++;
                        // Extract minute from argument
                        timeZone[idx].minute = (uint8_t)strtol(p, NULL, 10);
                        // Extract rate from next argument
                        timeZone[idx].tariff = (uint8_t)strtol(argv[argIdx + 1], NULL, 10);
                    }
                    else
                    {
                        parseError = true;
                        break;
                    }

                    if ((timeZone[idx].hour > 23) ||
                        (timeZone[idx].minute > 59) ||
                        (timeZone[idx].tariff > TARIFF_4))
                    {
                            parseError = true;
                            break;
                    }
                }
                else
                {
                    // No more arguments, fill TOU index with invalid data
                    timeZone[idx].hour = 0;
                    timeZone[idx].minute = 0;
                    timeZone[idx].tariff = TARIFF_INVALID;
                }
             }
        }
        else
        {
            // Invalid password
            parseError = true;
        }
    }
    else
    {
        // Incorrect parameter number
        parseError = true;
    }

    if (parseError)
    {
        SYS_CMD_MESSAGE("Unsupported Command !\r\n");
    }
    else
    {
        APP_ENERGY_SetTOUTimeZone(timeZone);

        // Clear No-persistent energy/demand data
        APP_ENERGY_ClearEnergy(false);
        APP_ENERGY_ClearMaxDemand(false);

        SYS_CMD_MESSAGE("Set TOU is Ok !\r\n");

        /* Show console communication icon */
        APP_DISPLAY_SetSerialCommunication();
    }
}

static void _commandRST(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    if (argc == 2)
    {
        // Check password from parameters
        if (strncmp(argv[1], metPwd, APP_CONSOLE_MET_PWD_SIZE) == 0)
        {
            // Correct password, Reset System
            SYS_CMD_MESSAGE("Reset Command is Ok !\r\n");
            // Go to state to reset system
            app_consoleData.state = APP_CONSOLE_STATE_SW_RESET;

            /* Show console communication icon */
            APP_DISPLAY_SetSerialCommunication();
        }
        else
        {
            // Invalid password
            SYS_CMD_MESSAGE("Invalid password\r\n");
        }
    }
    else
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
    }
}

static void _commandRLD(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    if (argc == 2)
    {
        // Check password from parameters
        if (strncmp(argv[1], metPwd, APP_CONSOLE_MET_PWD_SIZE) == 0)
        {
            // Correct password, Reset System
            SYS_CMD_MESSAGE("Reloading Metrology...\r\n\r\n");
            // Reload Metrology coprocessor
            APP_METROLOGY_Restart(true);

            /* Show console communication icon */
            APP_DISPLAY_SetSerialCommunication();
        }
        else
        {
            // Invalid password
            SYS_CMD_MESSAGE("Invalid password\r\n");
        }
    }
    else
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
    }
}

static void _commandDEV(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    if (argc == 1)
    {
        // Read topology of device connections
        app_consoleData.state = APP_CONSOLE_STATE_READ_CHANNELS_CONFIG;

        /* Show console communication icon */
        APP_DISPLAY_SetSerialCommunication();
    }
    else
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
    }
}

static void _commandSYN(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    APP_CONSOLE_SYN_OPCMD opCmd;
    bool paramError = false;
    bool runCmd = true;

    opCmd = (APP_CONSOLE_SYN_OPCMD)strtol(argv[1], NULL, 10);

    if (app_consoleData.synIsRunning == true)
    {
        if (opCmd != APP_CONSOLE_SYN_OPCMD_DISABLE)
        {
            runCmd = false;
            // Synthesizer enabled
            SYS_CMD_MESSAGE("SYN_OPCMD Error: Synthesizer is enabled\r\n");
        }
    }
    else
    {
        if (opCmd == APP_CONSOLE_SYN_OPCMD_DISABLE)
        {
            runCmd = false;
            // Synthesizer disabled
            SYS_CMD_MESSAGE("SYN_OPCMD: Synthesizer is disabled\r\n");
        }
    }

    if (runCmd == true)
    {
        if (opCmd == APP_CONSOLE_SYN_OPCMD_DISABLE)
        {
            if (argc == 2)
            {
                // Go to state to disable synthesizer
                app_consoleData.state = APP_CONSOLE_STATE_SYN_DISABLE;
            }
            else
            {
                paramError = true;
            }
        }
        else if (opCmd == APP_CONSOLE_SYN_OPCMD_ENABLE)
        {
            if (argc == 2)
            {
                // Go to state to enable synthesizer
                app_consoleData.state = APP_CONSOLE_STATE_SYN_ENABLE;
            }
            else
            {
                paramError = true;
            }
        }
        else if (opCmd == APP_CONSOLE_SYN_OPCMD_CHN_TRANSFER_START)
        {
            if (argc == 4)
            {
                DRV_METROLOGY_SYN_DESCRIPTOR *pNewDesc;
                uint16_t numSamples;
                uint8_t channel;

                // Parameters in Start Transfer: SYN|TRANSFER_START|CHN|SAMPLES
                channel = (uint8_t)strtol(argv[2], NULL, 10);
                numSamples = (uint16_t)strtol(argv[3], NULL, 10);

                // Build new descriptor dynamically
                pNewDesc = _addNewSyncDesc(channel, numSamples);
                if (pNewDesc == NULL)
                {
                    // Lack of memory. Free all reserved memory used in descriptors and samples data
                    _freeSyncDescriptors(app_consoleData.pSynCurrentDesc);
                    SYS_CMD_MESSAGE("Not enough memory !\r\n");
                }
            }
            else
            {
                paramError = true;
            }
        }
        else if (opCmd == APP_CONSOLE_SYN_OPCMD_CHN_TRANSFERRING)
        {
            if ((argc == 6) && (app_consoleData.pSynCurrentData != NULL))
            {
                char *pData;
                char data[8];
                uint8_t chunkLen;
                uint8_t sample;

                // Parameters in Transferring: SYN|TRANSFER_CHUNK|CHN|CHUNKID|CHUNKLEN|CHUNKDATA
                chunkLen = (uint8_t)strtol(argv[4], NULL, 10);
                pData = argv[5];

                // Fill data in the dynamic table
                for(sample = 0; sample < chunkLen; sample++)
                {
                    memcpy(data, pData, 8);
                    *app_consoleData.pSynCurrentData++ = (uint32_t)strtoul(data, NULL, 16);
                    pData += 8;
                }
            }
            else
            {
                paramError = true;
            }
        }
        else if (opCmd == APP_CONSOLE_SYN_OPCMD_CHN_TRANSFER_END)
        {
            if ((argc == 3) && (app_consoleData.pSynCurrentDesc != NULL))
            {
                uint8_t channel;

                // Parameters in End Transfer: SYN|TRANSFER_END|CHN
                channel = (uint8_t)strtol(argv[2], NULL, 10);

                if (app_consoleData.pSynCurrentDesc->control.channel == channel)
                {
                    // Set Key to descriptor control
                    app_consoleData.pSynCurrentDesc->control.key = 0x5A;
                }
            }
            else
            {
                paramError = true;
            }
        }
        else
        {
            // Incorrect opCmd
            SYS_CMD_MESSAGE("Unsupported Command !\r\n");
            // Cancel settings
        }

        if (paramError)
        {
            // Incorrect parameter number
            SYS_CMD_MESSAGE("Incorrect param number\r\n");
            // Cancel settings
        }
    }
}

static void _commandCAPT(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    DRV_METROLOGY_CAPTURE_SOURCE source = CAPTURE_SRC_16KHz;
    DRV_METROLOGY_CAPTURE_TYPE type = CAPTURE_ONE_SHOT;
    uint8_t channelMask = 0;
    uint8_t enable;
    bool parseError = false;

    if (argc == 1U)
    {
        // Get Capture state
        DRV_METROLOGY_CAPTURE_STATE state;

        state = DRV_METROLOGY_CaptureGetState();
        if (state == CAPTURE_DISABLED)
        {
            SYS_CMD_MESSAGE("Waveform capture is DISABLE\r\n");
        }
        else if (state == CAPTURE_ACTIVE)
        {
            SYS_CMD_MESSAGE("Waveform capture is ACTIVE\r\n");
        }
        else if (state == CAPTURE_COMPLETE)
        {
            SYS_CMD_MESSAGE("Waveform capture is COMPLETE\r\n");
        }
        else
        {
            SYS_CMD_MESSAGE("Error in Waveform capture state\r\n");
        }

        return;
    }
    else if (argc == 2U)
    {
        // This must be a Stop command
        enable = (uint8_t)strtol(argv[1], NULL, 10U);
        if (enable != 0U)
        {
            parseError = true;
        }
    }
    else if(argc == 6U)
    {
        enable = (uint8_t)strtol(argv[1], NULL, 10U);
        if (enable != 1U)
        {
            parseError = true;
        }

        /* Get Channel mask */
        channelMask = (uint8_t)strtol(argv[2], NULL, 10U);

        /* Get capture source */
        source = (DRV_METROLOGY_CAPTURE_SOURCE)strtol(argv[3], NULL, 10U);
        if (source >= CAPTURE_SRC_NUM)
        {
            parseError = true;
        }

        /* Get capture type */
        type = (DRV_METROLOGY_CAPTURE_TYPE)strtol(argv[4], NULL, 10U);
        if (type >= CAPTURE_TYPE_NUM)
        {
            parseError = true;
        }

        /* Get the number of samples to capture */
        app_consoleData.captNumSamples = strtol(argv[5], NULL, 10U);
        if (app_consoleData.captNumSamples > APP_CONSOLE_MET_CAPTURE_SAMPLES)
        {
            app_consoleData.captNumSamples = APP_CONSOLE_MET_CAPTURE_SAMPLES;
        }
    }
    else
    {
        parseError = true;
    }

    if (parseError)
    {
        SYS_CMD_MESSAGE("Unsupported Command !\r\n");
    }
    else
    {
        if (enable == 0U)
        {
            DRV_METROLOGY_CaptureStop();
            SYS_CMD_MESSAGE("Stop Waveform capture\r\n");
        }
        else
        {
            DRV_METROLOGY_CAPTURE_STATE state;

            state = DRV_METROLOGY_CaptureGetState();
            if (state == CAPTURE_ACTIVE)
            {
                SYS_CMD_MESSAGE("Waveform capture is already enabled\r\n");
            }
            else
            {
                DRV_METROLOGY_CaptureStop();
                DRV_METROLOGY_CaptureSetOptions(source, type);
                DRV_METROLOGY_CaptureEnableChannels(channelMask);
                DRV_METROLOGY_CaptureStart(metCaptureData, app_consoleData.captNumSamples);
                SYS_CMD_MESSAGE("Launched Waveform capture\r\n");
            }
        }
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_CONSOLE_Initialize ( void )

  Remarks:
    See prototype in app_console.h.
 */

void APP_CONSOLE_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    app_consoleData.state = APP_CONSOLE_STATE_INIT;
    app_consoleData.numCommands = sizeof(appCmdTbl)/sizeof(SYS_CMD_DESCRIPTOR);
    app_consoleData.pSynDescriptor = NULL;
    app_consoleData.pSynCurrentDesc = NULL;
    app_consoleData.captNumSamples = 0;

    /* Init timer */
    app_consoleData.timer = SYS_TIME_HANDLE_INVALID;

    if (!SYS_CMD_ADDGRP(appCmdTbl, app_consoleData.numCommands, "App Console", ": Metering console commands"))
    {
        SYS_CONSOLE_Print(SYS_CONSOLE_INDEX_0, "Failed to create APP Console Commands\r\n");
        app_consoleData.numCommands = 0;
    }
    else
    {
        SYS_CMD_CallbackRegister(appCmdTbl, _preprocessorCallback, 0);
    }
}


/******************************************************************************
  Function:
    void APP_CONSOLE_Tasks ( void )

  Remarks:
    See prototype in app_console.h.
 */

void APP_CONSOLE_Tasks ( void )
{
    char regName[4][18];
    uint32_t regValue32[4];
    uint64_t regValue64[4];
    uint8_t numRegsPending;

    /* Check the application's current state. */
    switch ( app_consoleData.state )
    {
        /* Application's initial state. */
        case APP_CONSOLE_STATE_IDLE:
        case APP_CONSOLE_STATE_WAIT_DATA:
        {
            break;
        }

        case APP_CONSOLE_STATE_PROMPT:
        {
            SYS_CMD_MESSAGE(">");
            app_consoleData.state = APP_CONSOLE_STATE_IDLE;
            break;
        }

        case APP_CONSOLE_STATE_INIT:
        {
            if (SYS_CMD_READY_TO_READ())
            {
                /* Initialize Energy App callbacks */
                APP_ENERGY_SetMonthEnergyCallback(_monthlyEnergyCallback, &energyData);
                APP_ENERGY_SetMaxDemandCallback(_maxDemandCallback, &maxDemandLocalObject);

                /* Initialize Metrology App callbacks */
                APP_METROLOGY_SetHarmonicAnalysisCallback(_harmonicAnalysisCallback, &harmonicAnalysisRMSData[0]);
                APP_METROLOGY_SetCalibrationCallback(_calibrationCallback);

                app_consoleData.currentWaitForDatalogReady = 0;
                app_consoleData.state = APP_CONSOLE_STATE_WAIT_STORAGE_READY;

                // Set default console storage data just in case it cannot be read later
                app_consoleStorageData = app_defaultConsoleStorageData;
            }
            break;
        }

        case APP_CONSOLE_STATE_WAIT_STORAGE_READY:
        {
            if (APP_DATALOG_GetStatus() == APP_DATALOG_STATE_READY)
            {
               app_consoleData.state = APP_CONSOLE_STATE_READ_STORAGE;
            }
            break;
        }

        case APP_CONSOLE_STATE_READ_STORAGE:
        {
            // Build queue element
            datalogQueueElement.userId = APP_DATALOG_USER_CONSOLE;
            datalogQueueElement.operation = APP_DATALOG_READ;
            datalogQueueElement.endCallback = _consoleReadStorage;
            datalogQueueElement.date.year = APP_DATALOG_INVALID_YEAR; /* Not used */
            datalogQueueElement.date.month = APP_DATALOG_INVALID_MONTH; /* Not used */
            datalogQueueElement.dataLen = sizeof(app_consoleStorageData);
            datalogQueueElement.pData = (uint8_t*)&app_consoleStorageData;
            // Put it in queue
            APP_DATALOG_SendDatalogData(&datalogQueueElement);

            // Wait for data to be read (semaphore is released in callback)
            app_consoleData.state = APP_CONSOLE_STATE_WAIT_DATA;
            // Data read, depending on read result, state has changed to READ_OK or READ_ERROR
            break;
        }

        case APP_CONSOLE_STATE_READ_STORAGE_ERROR:
        {
            // Build queue element to write it to storage
            datalogQueueElement.userId = APP_DATALOG_USER_CONSOLE;
            datalogQueueElement.operation = APP_DATALOG_WRITE;
            datalogQueueElement.endCallback = NULL;
            datalogQueueElement.date.year = APP_DATALOG_INVALID_YEAR; /* Not used */
            datalogQueueElement.date.month = APP_DATALOG_INVALID_MONTH; /* Not used */
            datalogQueueElement.dataLen = sizeof(app_consoleStorageData);
            datalogQueueElement.pData = (uint8_t*)&app_consoleStorageData;
            // Put it in queue
            APP_DATALOG_SendDatalogData(&datalogQueueElement);
            // Go to Idle state
            app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            break;
        }

        case APP_CONSOLE_STATE_DATALOG_NOT_READY:
        {
            SYS_CMD_MESSAGE("Datalog Service not ready!\r\nApplication will run without storage capabilities\r\n");
            // Go to Idle state
            app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            break;
        }

        case APP_CONSOLE_STATE_READ_CONTROL_REG:
        {
            // Remove Prompt symbol
            _removePrompt();

            // Read register value
            if (APP_METROLOGY_GetControlRegister((CONTROL_REG_ID)app_consoleData.ctrlRegToRead, &regValue32[0], regName[0]))
            {
                SYS_CMD_PRINT("%s\r\n%X\r\n", regName[0], regValue32[0]);
            }
            else
            {
                // Cannot read register
                SYS_CMD_PRINT("Could not read register %02d\r\n", app_consoleData.ctrlRegToRead);
            }
            // Go back to Idle
            app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            break;
        }

        case APP_CONSOLE_STATE_WRITE_CONTROL_REG:
        {
            uint8_t idx;

            // Remove Prompt symbol
            _removePrompt();

            for (idx = 0; idx < APP_CONSOLE_MAX_REGS; idx++)
            {
                if (app_consoleData.regsToModify[idx].index != 0xFF)
                {
                    if (app_consoleData.regsToModify[idx].index < CONTROL_REG_NUM)
                    {
                        // Write register value
                        if (APP_METROLOGY_SetControlRegister((CONTROL_REG_ID)app_consoleData.regsToModify[idx].index,
                                app_consoleData.regsToModify[idx].value))
                        {
                            // Show response on console
                            SYS_CMD_PRINT("Set %02d Is Ok !\r\n", app_consoleData.regsToModify[idx].index);
                        }
                        else
                        {
                            // Cannot write register
                            SYS_CMD_PRINT("Could not write register %02d\r\n", app_consoleData.regsToModify[idx].index);
                        }
                    }
                    else
                    {
                        // Invalid index
                        SYS_CMD_PRINT("Invalid register index %02d\r\n", app_consoleData.regsToModify[idx].index);
                    }
                }
                else
                {
                    // All registers have been written
                    app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
                    break;
                }

                if ((idx % 10) == 0)
                {
                    app_consoleData.nextState = app_consoleData.state;
                    app_consoleData.state = APP_CONSOLE_STATE_DELAY;
                    app_consoleData.delayMs = CONSOLE_TASK_DEFAULT_DELAY_MS_BETWEEN_STATES;
                }
            }
            break;
        }

        case APP_CONSOLE_STATE_READ_ALL_CONTROL_REGS:
        {
            if (app_consoleData.ctrlRegToRead < CONTROL_REG_NUM)
            {
                if (app_consoleData.ctrlRegToRead == 0)
                {
                    // Remove Prompt symbol
                    _removePrompt();
                }

                // Check how many registers are pending to print, to format line
                numRegsPending = CONTROL_REG_NUM - app_consoleData.ctrlRegToRead;
                // Read and print register values
                if (numRegsPending >= 4)
                {
                    if ((APP_METROLOGY_GetControlRegister((CONTROL_REG_ID)app_consoleData.ctrlRegToRead, &regValue32[0], regName[0])) &&
                        (APP_METROLOGY_GetControlRegister((CONTROL_REG_ID)app_consoleData.ctrlRegToRead + 1, &regValue32[1], regName[1])) &&
                        (APP_METROLOGY_GetControlRegister((CONTROL_REG_ID)app_consoleData.ctrlRegToRead + 2, &regValue32[2], regName[2])) &&
                        (APP_METROLOGY_GetControlRegister((CONTROL_REG_ID)app_consoleData.ctrlRegToRead + 3, &regValue32[3], regName[3])))
                    {
                        SYS_CMD_PRINT("%-19s%-19s%-19s%-19s\r\n", regName[0], regName[1], regName[2], regName[3]);
                        SYS_CMD_PRINT("%-19X%-19X%-19X%-19X\r\n", regValue32[0], regValue32[1], regValue32[2], regValue32[3]);
                    }
                    else
                    {
                        // Cannot read register
                        SYS_CMD_PRINT("Could not read register %02d\r\n", app_consoleData.ctrlRegToRead);
                    }
                    // Advance to next register group
                    app_consoleData.ctrlRegToRead += 4;
                }
                else if (numRegsPending == 3)
                {
                    if ((APP_METROLOGY_GetControlRegister((CONTROL_REG_ID)app_consoleData.ctrlRegToRead, &regValue32[0], regName[0])) &&
                        (APP_METROLOGY_GetControlRegister((CONTROL_REG_ID)app_consoleData.ctrlRegToRead + 1, &regValue32[1], regName[1])) &&
                        (APP_METROLOGY_GetControlRegister((CONTROL_REG_ID)app_consoleData.ctrlRegToRead + 2, &regValue32[2], regName[2])))
                    {
                        SYS_CMD_PRINT("%-19s%-19s%-19s\r\n", regName[0], regName[1], regName[2]);
                        SYS_CMD_PRINT("%-19X%-19X%-19X\r\n", regValue32[0], regValue32[1], regValue32[2]);
                    }
                    else
                    {
                        // Cannot read register
                        SYS_CMD_PRINT("Could not read register %02d\r\n", app_consoleData.ctrlRegToRead);
                    }
                    // Advance to next register group
                    app_consoleData.ctrlRegToRead += 3;
                }
                else if (numRegsPending == 2)
                {
                    if ((APP_METROLOGY_GetControlRegister((CONTROL_REG_ID)app_consoleData.ctrlRegToRead, &regValue32[0], regName[0])) &&
                        (APP_METROLOGY_GetControlRegister((CONTROL_REG_ID)app_consoleData.ctrlRegToRead + 1, &regValue32[1], regName[1])))
                    {
                        SYS_CMD_PRINT("%-19s%-19s\r\n", regName[0], regName[1]);
                        SYS_CMD_PRINT("%-19X%-19X\r\n", regValue32[0], regValue32[1]);
                    }
                    else
                    {
                        // Cannot read register
                        SYS_CMD_PRINT("Could not read register %02d\r\n", app_consoleData.ctrlRegToRead);
                    }
                    // Advance to next register group
                    app_consoleData.ctrlRegToRead += 2;
                }
                else if (numRegsPending == 1)
                {
                    if ((APP_METROLOGY_GetControlRegister((CONTROL_REG_ID)app_consoleData.ctrlRegToRead, &regValue32[0], regName[0])))
                    {
                        SYS_CMD_PRINT("%-19s\r\n", regName[0]);
                        SYS_CMD_PRINT("%-19X\r\n", regValue32[0]);
                    }
                    else
                    {
                        // Cannot read register
                        SYS_CMD_PRINT("Could not read register %02d\r\n", app_consoleData.ctrlRegToRead);
                    }
                    // Advance to next register group
                    app_consoleData.ctrlRegToRead += 1;
                }
            }
            else
            {
                // All registers have been read
                app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            }

            app_consoleData.nextState = app_consoleData.state;
            app_consoleData.state = APP_CONSOLE_STATE_DELAY;
            app_consoleData.delayMs = CONSOLE_TASK_DELAY_MS_BETWEEN_REGS_PRINT;
            break;
        }

        case APP_CONSOLE_STATE_READ_ACCUM_REG:
        {
            // Remove Prompt symbol
            _removePrompt();

            // Read register value
            if (APP_METROLOGY_GetAccumulatorRegister((ACCUMULATOR_REG_ID)app_consoleData.accumRegToRead, &regValue64[0], regName[0]))
            {
                SYS_CMD_PRINT("%s\r\n%llX\r\n", regName[0], regValue64[0]);
            }
            else
            {
                // Cannot read register
                SYS_CMD_PRINT("Could not read register %02d\r\n", app_consoleData.accumRegToRead);
            }
            // Go back to Idle
            app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            break;
        }

        case APP_CONSOLE_STATE_READ_ALL_ACCUM_REGS:
        {
            if (app_consoleData.accumRegToRead < ACCUMULATOR_REG_NUM)
            {
                uint8_t idx = app_consoleData.accumRegToRead;
                uint64_t *pData = (uint64_t *)&app_consoleData.metAccRegs;
                char **pDesc = (char **)app_consoleData.pRegDescription;

                pData += idx;
                pDesc += idx;

                if (app_consoleData.accumRegToRead == 0)
                {
                    // Remove Prompt symbol
                    _removePrompt();
                }

                // Check how many registers are pending to print, to format line
                numRegsPending = ACCUMULATOR_REG_NUM - idx;
                // Read and print register values
                if (numRegsPending >= 4)
                {
                    SYS_CMD_PRINT("%-19s%-19s%-19s%-19s\r\n", *pDesc, *(pDesc + 1), *(pDesc + 2), *(pDesc + 3));
                    SYS_CMD_PRINT("%-19llX%-19llX%-19llX%-19llX\r\n", *pData, *(pData + 1), *(pData + 2), *(pData + 3));
                    // Advance to next register group
                    app_consoleData.accumRegToRead += 4;
                }
                else if (numRegsPending == 3)
                {
                    SYS_CMD_PRINT("%-19s%-19s%-19s\r\n", *pDesc, *(pDesc + 1), *(pDesc + 2));
                    SYS_CMD_PRINT("%-19llX%-19llX%-19llX\r\n", *pData, *(pData + 1), *(pData + 2));
                    // Advance to next register group
                    app_consoleData.accumRegToRead += 3;
                }
                else if (numRegsPending == 2)
                {
                    SYS_CMD_PRINT("%-19s%-19s\r\n", *pDesc, *(pDesc + 1));
                    SYS_CMD_PRINT("%-19llX%-19llX\r\n", *pData, *(pData + 1));
                    // Advance to next register group
                    app_consoleData.accumRegToRead += 2;
                }
                else if (numRegsPending == 1)
                {
                    SYS_CMD_PRINT("%-19s\r\n", *pDesc);
                    SYS_CMD_PRINT("%-19llX\r\n");
                    // Advance to next register group
                    app_consoleData.accumRegToRead += 1;
                }
            }
            else
            {
                // All registers have been read
                app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            }

            app_consoleData.nextState = app_consoleData.state;
            app_consoleData.state = APP_CONSOLE_STATE_DELAY;
            app_consoleData.delayMs = CONSOLE_TASK_DELAY_MS_BETWEEN_REGS_PRINT;
            break;
        }

        case APP_CONSOLE_STATE_READ_PC_ACCUM_REG:
        {
            // Remove Prompt symbol
            _removePrompt();

            // Read register value
            if (APP_METROLOGY_GetPCAccumulatorRegister((PER_CYCLE_ACCUMULATOR_REG_ID)app_consoleData.accumRegToRead, &regValue64[0], regName[0]))
            {
                SYS_CMD_PRINT("%s\r\n%llX\r\n", regName[0], regValue64[0]);
            }
            else
            {
                // Cannot read register
                SYS_CMD_PRINT("Could not read register %02d\r\n", app_consoleData.accumRegToRead);
            }
            // Go back to Idle
            app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            break;
        }

        case APP_CONSOLE_STATE_READ_ALL_PC_ACCUM_REGS:
        {
            if (app_consoleData.accumRegToRead < PER_CYCLE_ACC_REG_NUM)
            {
                uint8_t idx = app_consoleData.accumRegToRead;
                uint64_t *pData = (uint64_t *)&app_consoleData.metPerCycleAccRegs;
                char **pDesc = (char **)app_consoleData.pRegDescription;

                pData += idx;
                pDesc += idx;

                if (app_consoleData.accumRegToRead == 0)
                {
                    // Remove Prompt symbol
                    _removePrompt();
                }

                // Check how many registers are pending to print, to format line
                numRegsPending = PER_CYCLE_ACC_REG_NUM - idx;
                // Read and print register values
                if (numRegsPending >= 4)
                {
                    SYS_CMD_PRINT("%-19s%-19s%-19s%-19s\r\n", *pDesc, *(pDesc + 1), *(pDesc + 2), *(pDesc + 3));
                    SYS_CMD_PRINT("%-19llX%-19llX%-19llX%-19llX\r\n", *pData, *(pData + 1), *(pData + 2), *(pData + 3));
                    // Advance to next register group
                    app_consoleData.accumRegToRead += 4;
                }
                else if (numRegsPending == 3)
                {
                    SYS_CMD_PRINT("%-19s%-19s%-19s\r\n", *pDesc, *(pDesc + 1), *(pDesc + 2));
                    SYS_CMD_PRINT("%-19llX%-19llX%-19llX\r\n", *pData, *(pData + 1), *(pData + 2));
                    // Advance to next register group
                    app_consoleData.accumRegToRead += 3;
                }
                else if (numRegsPending == 2)
                {
                    SYS_CMD_PRINT("%-19s%-19s\r\n", *pDesc, *(pDesc + 1));
                    SYS_CMD_PRINT("%-19llX%-19llX\r\n", *pData, *(pData + 1));
                    // Advance to next register group
                    app_consoleData.accumRegToRead += 2;
                }
                else if (numRegsPending == 1)
                {
                    SYS_CMD_PRINT("%-19s\r\n", *pDesc);
                    SYS_CMD_PRINT("%-19llX\r\n");
                    // Advance to next register group
                    app_consoleData.accumRegToRead += 1;
                }
            }
            else
            {
                // All registers have been read
                app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            }

            app_consoleData.nextState = app_consoleData.state;
            app_consoleData.state = APP_CONSOLE_STATE_DELAY;
            app_consoleData.delayMs = CONSOLE_TASK_DELAY_MS_BETWEEN_REGS_PRINT;
            break;
        }

        case APP_CONSOLE_STATE_READ_STATUS_REG:
        {
            // Remove Prompt symbol
            _removePrompt();

            // Read register value
            if (APP_METROLOGY_GetStatusRegister((STATUS_REG_ID)app_consoleData.statusRegToRead, &regValue32[0], regName[0]))
            {
                SYS_CMD_PRINT("%s\r\n%X\r\n", regName[0], regValue32[0]);
            }
            else
            {
                // Cannot read register
                SYS_CMD_PRINT("Could not read register %02d\r\n", app_consoleData.statusRegToRead);
            }
            // Go back to Idle
            app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            break;
        }

        case APP_CONSOLE_STATE_READ_ALL_STATUS_REGS:
        {
            if (app_consoleData.statusRegToRead < STATUS_REG_NUM)
            {
                if (app_consoleData.statusRegToRead == 0)
                {
                    // Remove Prompt symbol
                    _removePrompt();
                }

                // Check how many registers are pending to print, to format line
                numRegsPending = STATUS_REG_NUM - app_consoleData.statusRegToRead;
                // Read and print register values
                if (numRegsPending >= 4)
                {
                    if ((APP_METROLOGY_GetStatusRegister((STATUS_REG_ID)app_consoleData.statusRegToRead, &regValue32[0], regName[0])) &&
                        (APP_METROLOGY_GetStatusRegister((STATUS_REG_ID)app_consoleData.statusRegToRead + 1, &regValue32[1], regName[1])) &&
                        (APP_METROLOGY_GetStatusRegister((STATUS_REG_ID)app_consoleData.statusRegToRead + 2, &regValue32[2], regName[2])) &&
                        (APP_METROLOGY_GetStatusRegister((STATUS_REG_ID)app_consoleData.statusRegToRead + 3, &regValue32[3], regName[3])))
                    {
                        SYS_CMD_PRINT("%-19s%-19s%-19s%-19s\r\n", regName[0], regName[1], regName[2], regName[3]);
                        SYS_CMD_PRINT("%-19X%-19X%-19X%-19X\r\n", regValue32[0], regValue32[1], regValue32[2], regValue32[3]);
                    }
                    else {
                        // Cannot read register
                        SYS_CMD_PRINT("Could not read register %02d\r\n", app_consoleData.statusRegToRead);
                    }
                    // Advance to next register group
                    app_consoleData.statusRegToRead += 4;
                }
                else if (numRegsPending == 3)
                {
                    if ((APP_METROLOGY_GetStatusRegister((STATUS_REG_ID)app_consoleData.statusRegToRead, &regValue32[0], regName[0])) &&
                        (APP_METROLOGY_GetStatusRegister((STATUS_REG_ID)app_consoleData.statusRegToRead + 1, &regValue32[1], regName[1])) &&
                        (APP_METROLOGY_GetStatusRegister((STATUS_REG_ID)app_consoleData.statusRegToRead + 2, &regValue32[2], regName[2])))
                    {
                        SYS_CMD_PRINT("%-19s%-19s%-19s\r\n", regName[0], regName[1], regName[2]);
                        SYS_CMD_PRINT("%-19X%-19X%-19X\r\n", regValue32[0], regValue32[1], regValue32[2]);
                    }
                    else
                    {
                        // Cannot read register
                        SYS_CMD_PRINT("Could not read register %02d\r\n", app_consoleData.statusRegToRead);
                    }
                    // Advance to next register group
                    app_consoleData.statusRegToRead += 3;
                }
                else if (numRegsPending == 2)
                {
                    if ((APP_METROLOGY_GetStatusRegister((STATUS_REG_ID)app_consoleData.statusRegToRead, &regValue32[0], regName[0])) &&
                        (APP_METROLOGY_GetStatusRegister((STATUS_REG_ID)app_consoleData.statusRegToRead + 1, &regValue32[1], regName[1])))
                    {
                        SYS_CMD_PRINT("%-19s%-19s\r\n", regName[0], regName[1]);
                        SYS_CMD_PRINT("%-19X%-19X\r\n", regValue32[0], regValue32[1]);
                    }
                    else
                    {
                        // Cannot read register
                        SYS_CMD_PRINT("Could not read register %02d\r\n", app_consoleData.statusRegToRead);
                    }
                    // Advance to next register group
                    app_consoleData.statusRegToRead += 2;
                }
                else if (numRegsPending == 1)
                {
                    if ((APP_METROLOGY_GetStatusRegister((STATUS_REG_ID)app_consoleData.statusRegToRead, &regValue32[0], regName[0])))
                    {
                        SYS_CMD_PRINT("%-19s\r\n", regName[0]);
                        SYS_CMD_PRINT("%-19X\r\n", regValue32[0]);
                    }
                    else
                    {
                        // Cannot read register
                        SYS_CMD_PRINT("Could not read register %02d\r\n", app_consoleData.statusRegToRead);
                    }
                    // Advance to next register group
                    app_consoleData.statusRegToRead += 1;
                }
            }
            else
            {
                // All registers have been read
                app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            }

            app_consoleData.nextState = app_consoleData.state;
            app_consoleData.state = APP_CONSOLE_STATE_DELAY;
            app_consoleData.delayMs = CONSOLE_TASK_DELAY_MS_BETWEEN_REGS_PRINT;
            break;
        }

        case APP_CONSOLE_STATE_READ_HARMONIC_REGS:
        {
            uint8_t numRegId;
            uint8_t harmonicNum;

            if (app_consoleData.numRegsPending == HARMONICS_REG_NUM)
            {
                // Remove Prompt symbol
                _removePrompt();
            }

            numRegId = HARMONICS_REG_NUM - app_consoleData.numRegsPending;
            harmonicNum = app_consoleData.harNumToRead;

            // Read and print register values
            if (app_consoleData.numRegsPending >= 4)
            {
                APP_METROLOGY_GetHarmonicRegister(numRegId, harmonicNum, &regValue32[0], regName[0]);
                APP_METROLOGY_GetHarmonicRegister(numRegId + 1, harmonicNum, &regValue32[1], regName[1]);
                APP_METROLOGY_GetHarmonicRegister(numRegId + 2, harmonicNum, &regValue32[2], regName[2]);
                APP_METROLOGY_GetHarmonicRegister(numRegId + 3, harmonicNum, &regValue32[3], regName[3]);
                SYS_CMD_PRINT("%-19s%-19s%-19s%-19s\r\n", regName[0], regName[1], regName[2], regName[3]);
                SYS_CMD_PRINT("%-19X%-19X%-19X%-19X\r\n", regValue32[0], regValue32[1], regValue32[2], regValue32[3]);

                // Advance to next register group
                app_consoleData.numRegsPending -= 4;
            }
            else if (app_consoleData.numRegsPending == 3)
            {
                APP_METROLOGY_GetHarmonicRegister(numRegId, harmonicNum, &regValue32[0], regName[0]);
                APP_METROLOGY_GetHarmonicRegister(numRegId + 1, harmonicNum, &regValue32[1], regName[1]);
                APP_METROLOGY_GetHarmonicRegister(numRegId + 2, harmonicNum, &regValue32[2], regName[2]);
                SYS_CMD_PRINT("%-19s%-19s%-19s\r\n", regName[0], regName[1], regName[2]);
                SYS_CMD_PRINT("%-19X%-19X%-19X\r\n", regValue32[0], regValue32[1], regValue32[2]);

                // Advance to next register group
                app_consoleData.numRegsPending -= 3;
            }
            else if (app_consoleData.numRegsPending == 2)
            {
                APP_METROLOGY_GetHarmonicRegister(numRegId, harmonicNum, &regValue32[0], regName[0]);
                APP_METROLOGY_GetHarmonicRegister(numRegId + 1, harmonicNum, &regValue32[1], regName[1]);
                SYS_CMD_PRINT("%-19s%-19s\r\n", regName[0], regName[1]);
                SYS_CMD_PRINT("%-19X%-19X\r\n", regValue32[0], regValue32[1]);

                // Advance to next register group
                app_consoleData.numRegsPending -= 2;
            }
            else if (app_consoleData.numRegsPending == 1)
            {
                APP_METROLOGY_GetHarmonicRegister(numRegId, harmonicNum, &regValue32[0], regName[0]);
                SYS_CMD_PRINT("%-19s\r\n", regName[0]);
                SYS_CMD_PRINT("%-19X\r\n", regValue32[0]);

                // Advance to next register group
                app_consoleData.numRegsPending -= 1;
            }
            else
            {
                // All registers have been read
                app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            }

            if (app_consoleData.state != APP_CONSOLE_STATE_PROMPT)
            {
                app_consoleData.nextState = app_consoleData.state;
                app_consoleData.state = APP_CONSOLE_STATE_DELAY;
                app_consoleData.delayMs = CONSOLE_TASK_DELAY_MS_BETWEEN_REGS_PRINT;
            }
            break;
        }

        case APP_CONSOLE_STATE_READ_ALL_HARMONICS_REGS:
        {
            if ((app_consoleData.numRegsPending == HARMONICS_REG_NUM) &&
                (app_consoleData.numHarmsPending == DRV_METROLOGY_HARMONICS_MAX_ORDER))
            {
                // Remove Prompt symbol
                _removePrompt();
            }

            // Read and print register values
            if (app_consoleData.numRegsPending == 0)
            {
                // All registers have been read
                app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            }
            else
            {
                uint8_t numRegId;
                uint8_t harmonicNum;

                numRegId = HARMONICS_REG_NUM - app_consoleData.numRegsPending;
                numRegId = harRegsDisplayOrder[numRegId];
                harmonicNum = DRV_METROLOGY_HARMONICS_MAX_ORDER - app_consoleData.numHarmsPending;

                // Read and print harmonic values
                while (app_consoleData.numHarmsPending > 0)
                {
                    if (app_consoleData.numHarmsPending >= 4)
                    {
                        APP_METROLOGY_GetHarmonicRegister(numRegId, harmonicNum, &regValue32[0], regName[0]);
                        APP_METROLOGY_GetHarmonicRegister(numRegId, harmonicNum + 1, &regValue32[1], regName[1]);
                        APP_METROLOGY_GetHarmonicRegister(numRegId, harmonicNum + 2, &regValue32[2], regName[2]);
                        APP_METROLOGY_GetHarmonicRegister(numRegId, harmonicNum + 3, &regValue32[3], regName[3]);

                        if (app_consoleData.numHarmsPending == DRV_METROLOGY_HARMONICS_MAX_ORDER)
                        {
                            SYS_CMD_PRINT("%-19s\r\n", regName[0]);
                        }

                        SYS_CMD_PRINT("%-19X%-19X%-19X%-19X\r\n", regValue32[0], regValue32[1], regValue32[2], regValue32[3]);

                        // Advance to next harmonic group
                        app_consoleData.numHarmsPending -= 4;
                    }
                    else if (app_consoleData.numHarmsPending == 3)
                    {
                        APP_METROLOGY_GetHarmonicRegister(numRegId, harmonicNum, &regValue32[0], regName[0]);
                        APP_METROLOGY_GetHarmonicRegister(numRegId, harmonicNum + 1, &regValue32[1], regName[1]);
                        APP_METROLOGY_GetHarmonicRegister(numRegId, harmonicNum + 2, &regValue32[2], regName[2]);

                        SYS_CMD_PRINT("%-19X%-19X%-19X\r\n", regValue32[0], regValue32[1], regValue32[2]);

                        // Advance to next harmonic group
                        app_consoleData.numHarmsPending -= 3;
                    }
                    else if (app_consoleData.numHarmsPending == 2)
                    {
                        APP_METROLOGY_GetHarmonicRegister(numRegId, harmonicNum, &regValue32[0], regName[0]);
                        APP_METROLOGY_GetHarmonicRegister(numRegId, harmonicNum + 1, &regValue32[1], regName[1]);

                        SYS_CMD_PRINT("%-19X%-19X\r\n", regValue32[0], regValue32[1]);

                        // Advance to next harmonic group
                        app_consoleData.numHarmsPending -= 2;
                    }
                    else if (app_consoleData.numHarmsPending == 1)
                    {
                        APP_METROLOGY_GetHarmonicRegister(numRegId, harmonicNum, &regValue32[0], regName[0]);

                        SYS_CMD_PRINT("%-19X\r\n", regValue32[0]);

                        // Advance to next harmonic group
                        app_consoleData.numHarmsPending--;
                    }
                    // Update harmonic number
                    harmonicNum = DRV_METROLOGY_HARMONICS_MAX_ORDER - app_consoleData.numHarmsPending;
                }

                // Advance to next register group
                app_consoleData.numHarmsPending = DRV_METROLOGY_HARMONICS_MAX_ORDER;
                app_consoleData.numRegsPending--;

                app_consoleData.nextState = app_consoleData.state;
                app_consoleData.state = APP_CONSOLE_STATE_DELAY;
                app_consoleData.delayMs = CONSOLE_TASK_DELAY_MS_BETWEEN_HAR_REGS_PRINT;
            }

            break;
        }

        case APP_CONSOLE_STATE_READ_METER_ID:
        {
            // Remove Prompt symbol
            _removePrompt();

            // Show response on console
            SYS_CMD_MESSAGE("Meter ID is:\r\n");
            SYS_CMD_PRINT("%s\r\n", app_consoleStorageData.meterID);
            // Go back to IDLE
            app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            break;
        }

        case APP_CONSOLE_STATE_READ_RTC:
        {
            // Remove Prompt symbol
            _removePrompt();

            // Read and print RTC
            RTC_TimeGet(&app_consoleData.sysTime);
            SYS_CMD_MESSAGE("Present RTC is (yy-mm-dd w hh:mm:ss):\r\n");
            SYS_CMD_PRINT("%02u-%02u-%02u %u %02u:%02u:%02u\r\n",
                    app_consoleData.sysTime.tm_year + 1900 - 2000, app_consoleData.sysTime.tm_mon + 1, app_consoleData.sysTime.tm_mday,
                    app_consoleData.sysTime.tm_wday + 1, app_consoleData.sysTime.tm_hour, app_consoleData.sysTime.tm_min, app_consoleData.sysTime.tm_sec);
            // Go back to IDLE
            app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            break;
        }

        case APP_CONSOLE_STATE_READ_TOU:
        {
            APP_ENERGY_TOU_TIME_ZONE * timeZone;
            uint8_t idx;

            timeZone = APP_ENERGY_GetTOUTimeZone();

            // Remove Prompt symbol
            _removePrompt();

            SYS_CMD_MESSAGE("TOU table is:\r\n");
            for (idx = 0; idx < APP_ENERGY_TOU_MAX_ZONES; idx++, timeZone++)
            {
                if (timeZone->tariff != TARIFF_INVALID)
                {
                    SYS_CMD_PRINT("TOU%d=%02d:%02d T%d ",
                        (idx + 1), timeZone->hour, timeZone->minute, timeZone->tariff);
                }
            }
            SYS_CMD_MESSAGE("\r\n");

            // Go back to IDLE
            app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            break;
        }

        case APP_CONSOLE_STATE_PRINT_ALL_HARMONIC_ANALYSIS:
        {
            if (app_consoleData.numRegsPending == APP_CONSOLE_HAR_DESC_SIZE)
            {
                // Remove Prompt symbol
                _removePrompt();
            }

            // Read and print register values
            if (app_consoleData.numRegsPending == 0)
            {
                // All registers have been read
                app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            }
            else
            {
                double rmsValues[4];
                uint8_t numRegId;
                uint8_t currHarmonic;
                uint8_t index, printCount;

                numRegId = APP_CONSOLE_HAR_DESC_SIZE - app_consoleData.numRegsPending;
                currHarmonic = DRV_METROLOGY_HARMONICS_MAX_ORDER - app_consoleData.numHarmsPending;

                if (app_consoleData.numHarmsPending == DRV_METROLOGY_HARMONICS_MAX_ORDER)
                {
                    // Print magnitude and active bitmap, since all harmonics will be printed
                    SYS_CMD_PRINT("%s, bitmap: 0x%08X\r\n", _console_har_desc[numRegId], app_consoleData.harmonicBitmap);
                }

                // Start or continue walking through harmonics from current position
                printCount = 0;
                for (index = currHarmonic; index < DRV_METROLOGY_HARMONICS_MAX_ORDER; index ++)
                {
                    // Active or not, print value (will be 0 if inactive)
                    app_consoleData.numHarmsPending--;
                    rmsValues[printCount] = *((double *)&harmonicAnalysisRMSData[index] + numRegId);
                    printCount++;
                    if (printCount == 4)
                    {
                        // Max prints per process, exit
                        break;
                    }
                }

                // Print
                if (printCount == 4)
                {
                    SYS_CMD_PRINT("%-19.3f%-19.3f%-19.3f%-19.3f\r\n",
                            rmsValues[0], rmsValues[1], rmsValues[2], rmsValues[3]);
                }
                else if (printCount == 3)
                {
                    SYS_CMD_PRINT("%-19.3f%-19.3f%-19.3f\r\n",
                            rmsValues[0], rmsValues[1], rmsValues[2]);
                }
                else if (printCount == 2)
                {
                    SYS_CMD_PRINT("%-19.3f%-19.3f\r\n", rmsValues[0], rmsValues[1]);
                }
                else if (printCount == 1)
                {
                    SYS_CMD_PRINT("%-19.3f\r\n", rmsValues[0]);
                }

                // Check whether all harmonics are walked
                if (app_consoleData.numHarmsPending == 0)
                {
                    // Advance to next register group
                    app_consoleData.numHarmsPending = DRV_METROLOGY_HARMONICS_MAX_ORDER;
                    app_consoleData.numRegsPending--;
                }

                // Delay before continue printing
                app_consoleData.nextState = app_consoleData.state;
                app_consoleData.state = APP_CONSOLE_STATE_DELAY;
                app_consoleData.delayMs = CONSOLE_TASK_DELAY_MS_BETWEEN_REGS_PRINT;
            }

            break;
        }

        case APP_CONSOLE_STATE_PRINT_MONTHLY_ENERGY:
        {
            float total = 0.0f;
            int8_t idx;

            for (idx = 0; idx < TARIFF_NUM_TYPE; idx++)
            {
                total += energyData.tariff[idx];
            }

            // Remove Prompt symbol
            _removePrompt();

            // Show received data on console
            idx = _getMonthIndexFromSysTime(&app_consoleData.timeRequest);
            SYS_CMD_PRINT("Last %d Month Energy is:\r\n", idx);

            SYS_CMD_PRINT("TT=%.2fkWh T1=%.2fkWh T2=%.2fkWh T3=%.2fkWh T4=%.2fkWh\r\n",
                          total / 1000, energyData.tariff[0] / 1000, energyData.tariff[1] / 1000,
                          energyData.tariff[2] / 1000, energyData.tariff[3] / 1000);

            /* Introduce a delay to wait console visualization */
            app_consoleData.nextState = APP_CONSOLE_STATE_PRINT_MONTHLY_ENERGY_NEXT;
            app_consoleData.state = APP_CONSOLE_STATE_DELAY;
            app_consoleData.delayMs = CONSOLE_TASK_DELAY_MS_BETWEEN_REGS_PRINT;

            break;
        }

        case APP_CONSOLE_STATE_PRINT_MONTHLY_ENERGY_NEXT:
        {
            // Check pending monthly requests
            app_consoleData.requestCounter--;
            if (app_consoleData.requestCounter > 0)
            {
                int8_t idx;

                idx = _getMonthIndexFromSysTime(&app_consoleData.timeRequest);

                _getSysTimeFromMonthIndex(&app_consoleData.timeRequest, idx + 1);
                APP_ENERGY_GetMonthEnergy(&app_consoleData.timeRequest);
            }

            // Go back to IDLE
            app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            break;
        }

        case APP_CONSOLE_STATE_PRINT_EVENT:
        {
            APP_EVENTS_EVENT_INFO eventInfo;
            uint8_t numEvents;
            struct tm invalidTime = {0};

            // Remove Prompt symbol
            _removePrompt();

            APP_EVENTS_GetNumEvents(app_consoleData.eventIdRequest, &numEvents);
            if (APP_EVENTS_GetEventInfo(app_consoleData.eventIdRequest, app_consoleData.eventLastTimeRequest, &eventInfo))
            {
                // Print Event ID and requested Times
                if (app_consoleData.eventIdRequest == SAG_UA_EVENT_ID)
                {
                    SYS_CMD_PRINT("Last %d Times Ua Sag is:\r\n", app_consoleData.eventLastTimeRequest);
                }
                else if (app_consoleData.eventIdRequest == SAG_UB_EVENT_ID)
                {
                    SYS_CMD_PRINT("Last %d Times Ub Sag is:\r\n", app_consoleData.eventLastTimeRequest);
                }
                else if (app_consoleData.eventIdRequest == SAG_UC_EVENT_ID)
                {
                    SYS_CMD_PRINT("Last %d Times Uc Sag is:\r\n", app_consoleData.eventLastTimeRequest);
                }
                else if (app_consoleData.eventIdRequest == SWELL_UA_EVENT_ID)
                {
                    SYS_CMD_PRINT("Last %d Times Ua Swell is:\r\n", app_consoleData.eventLastTimeRequest);
                }
                else if (app_consoleData.eventIdRequest == SWELL_UB_EVENT_ID)
                {
                    SYS_CMD_PRINT("Last %d Times Ub Swell is:\r\n", app_consoleData.eventLastTimeRequest);
                }
                else if (app_consoleData.eventIdRequest == SWELL_UC_EVENT_ID)
                {
                    SYS_CMD_PRINT("Last %d Times Uc Swell is:\r\n", app_consoleData.eventLastTimeRequest);
                }
                else if (app_consoleData.eventIdRequest == POW_PA_EVENT_ID)
                {
                    SYS_CMD_PRINT("Last %d Times Pa reverse is:\r\n", app_consoleData.eventLastTimeRequest);
                }
                else if (app_consoleData.eventIdRequest == POW_PB_EVENT_ID)
                {
                    SYS_CMD_PRINT("Last %d Times Pb reverse is:\r\n", app_consoleData.eventLastTimeRequest);
                }
                else if (app_consoleData.eventIdRequest == POW_PC_EVENT_ID)
                {
                    SYS_CMD_PRINT("Last %d Times Pc reverse is:\r\n", app_consoleData.eventLastTimeRequest);
                }

                // Show received data on console
                SYS_CMD_PRINT("Start time (yy-mm-dd hh:mm:ss)=");
                if (memcmp(&eventInfo.startTime, &invalidTime, sizeof(struct tm)) == 0)
                {
                    SYS_CMD_PRINT("invalid\r\n");
                }
                else
                {
                    SYS_CMD_PRINT("%02u-%02u-%02u %02u:%02u:%02u\r\n",
                    eventInfo.startTime.tm_year + 1900 - 2000,
                    eventInfo.startTime.tm_mon + 1,
                    eventInfo.startTime.tm_mday,
                    eventInfo.startTime.tm_hour,
                    eventInfo.startTime.tm_min,
                    eventInfo.startTime.tm_sec);
                }

                SYS_CMD_PRINT("End time   (yy-mm-dd hh:mm:ss)=");
                if (memcmp(&eventInfo.endTime, &invalidTime, sizeof(struct tm)) == 0)
                {
                    SYS_CMD_PRINT("invalid\r\n");
                }
                else
                {
                    SYS_CMD_PRINT("%02u-%02u-%02u %02u:%02u:%02u\r\n",
                    eventInfo.endTime.tm_year + 1900 - 2000,
                    eventInfo.endTime.tm_mon + 1,
                    eventInfo.endTime.tm_mday,
                    eventInfo.endTime.tm_hour,
                    eventInfo.endTime.tm_min,
                    eventInfo.endTime.tm_sec);
                }

                SYS_CMD_PRINT("Total Num=%d\r\n", numEvents);
            }
            else
            {
                SYS_CMD_MESSAGE("Maximum number of reported events exceeded: 10\r\n");
            }

            // Go back to IDLE
            app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            break;
        }

        case APP_CONSOLE_STATE_PRINT_MAX_DEMAND:
        {
            APP_ENERGY_DEMAND_DATA *pDataMax;
            APP_ENERGY_DEMAND_DATA *pDataTariff;
            int8_t idx;

            // Remove Prompt symbol
            _removePrompt();

            // Show received data on console
            idx = _getMonthIndexFromSysTime(&app_consoleData.timeRequest);
            SYS_CMD_PRINT("Last %d Month MaxDemand is:\r\n", idx);

            pDataMax = &maxDemandLocalObject.maxDemad;
            pDataTariff = &maxDemandLocalObject.tariff[0];
            SYS_CMD_PRINT("TT=%.3fkW %d-%d %02d:%02d T1=%.3fkW %d-%d %02d:%02d T2=%.3fkW %d-%d %02d:%02d T3=%.3fkW %d-%d %02d:%02d T4=%.3fkW %d-%d %02d:%02d\r\n",
                    (float)pDataMax->value/1000, pDataMax->month + 1, pDataMax->day, pDataMax->hour, pDataMax->minute,
                    (float)pDataTariff->value/1000, pDataTariff->month + 1, pDataTariff->day, pDataTariff->hour, pDataTariff->minute,
                    (float)(pDataTariff + 1)->value/1000, (pDataTariff + 1)->month + 1, (pDataTariff + 1)->day, (pDataTariff + 1)->hour, (pDataTariff + 1)->minute,
                    (float)(pDataTariff + 2)->value/1000, (pDataTariff + 2)->month + 1, (pDataTariff + 2)->day, (pDataTariff + 2)->hour, (pDataTariff + 2)->minute,
                    (float)(pDataTariff + 3)->value/1000, (pDataTariff + 3)->month + 1, (pDataTariff + 3)->day, (pDataTariff + 3)->hour, (pDataTariff + 3)->minute);

            /* Introduce a delay to wait console visualization */
            app_consoleData.nextState = APP_CONSOLE_STATE_PRINT_MAX_DEMAND_NEXT;
            app_consoleData.state = APP_CONSOLE_STATE_DELAY;
            app_consoleData.delayMs = CONSOLE_TASK_DELAY_MS_BETWEEN_REGS_PRINT;

            break;
        }

        case APP_CONSOLE_STATE_PRINT_MAX_DEMAND_NEXT:
        {
            // Check pending monthly requests
            app_consoleData.requestCounter--;
            if (app_consoleData.requestCounter > 0)
            {
                int8_t idx;

                idx = _getMonthIndexFromSysTime(&app_consoleData.timeRequest);

                _getSysTimeFromMonthIndex(&app_consoleData.timeRequest, idx + 1);
                APP_ENERGY_GetMonthMaxDemand(&app_consoleData.timeRequest);
            }

            // Go back to IDLE
            app_consoleData.state = APP_CONSOLE_STATE_PROMPT;

            break;
        }

        case APP_CONSOLE_STATE_PRINT_VOLTAGE:
        {
            float va, vb, vc;

            // Remove Prompt symbol
            _removePrompt();

            APP_METROLOGY_GetMeasure(MEASURE_UA_RMS, &va, app_consoleData.perCyclePar);
            APP_METROLOGY_GetMeasure(MEASURE_UB_RMS, &vb, app_consoleData.perCyclePar);
            APP_METROLOGY_GetMeasure(MEASURE_UC_RMS, &vc, app_consoleData.perCyclePar);
            // Show received data on console
            SYS_CMD_PRINT("Present voltage%s is :\r\nUa=%.3fV Ub=%.3fV Uc=%.3fV\r\n",
                          perCycle[app_consoleData.perCyclePar], va, vb, vc);

            // Go back to IDLE
            app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            break;
        }

        case APP_CONSOLE_STATE_PRINT_CURRENT:
        {
            float ia, ib, ic;
            float inm;

            // Remove Prompt symbol
            _removePrompt();

            APP_METROLOGY_GetMeasure(MEASURE_IA_RMS, &ia, app_consoleData.perCyclePar);
            APP_METROLOGY_GetMeasure(MEASURE_IB_RMS, &ib, app_consoleData.perCyclePar);
            APP_METROLOGY_GetMeasure(MEASURE_IC_RMS, &ic, app_consoleData.perCyclePar);
            APP_METROLOGY_GetMeasure(MEASURE_INM_RMS, &inm, app_consoleData.perCyclePar);

            // Show received data on console
            if (app_consoleData.perCyclePar == false)
            {
                float ini, inmi;

                APP_METROLOGY_GetMeasure(MEASURE_INI_RMS, &ini, app_consoleData.perCyclePar);
                APP_METROLOGY_GetMeasure(MEASURE_INMI_RMS, &inmi, app_consoleData.perCyclePar);

                SYS_CMD_PRINT("Present current%s is :\r\nIa=%.4fA Ib=%.4fA Ic=%.4fA Ini=%.4fA Inm=%.4fA Inmi=%.4fA\r\n",
                        perCycle[app_consoleData.perCyclePar], ia, ib, ic, ini, inm, inmi);
            }
            else
            {
                SYS_CMD_PRINT("Present current%s is :\r\nIa=%.4fA Ib=%.4fA Ic=%.4fA Inm=%.4fA\r\n",
                        perCycle[app_consoleData.perCyclePar], ia, ib, ic, inm);
            }

            // Go back to IDLE
            app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            break;
        }

        case APP_CONSOLE_STATE_PRINT_ACTIVE_POWER:
        {
            float pt, pa, pb, pc;

            // Remove Prompt symbol
            _removePrompt();

            APP_METROLOGY_GetMeasure(MEASURE_PT, &pt, app_consoleData.perCyclePar);
            APP_METROLOGY_GetMeasure(MEASURE_PA, &pa, app_consoleData.perCyclePar);
            APP_METROLOGY_GetMeasure(MEASURE_PB, &pb, app_consoleData.perCyclePar);
            APP_METROLOGY_GetMeasure(MEASURE_PC, &pc, app_consoleData.perCyclePar);
            // Show received data on console
            SYS_CMD_PRINT("Present active power%s is :\r\nPt=%.1fW Pa=%.1fW Pb=%.1fW Pc=%.1fW\r\n",
                   perCycle[app_consoleData.perCyclePar], pt, pa, pb, pc);

            // Go back to IDLE
            app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            break;
        }

        case APP_CONSOLE_STATE_PRINT_REACTIVE_POWER:
        {
            float qt, qa, qb, qc;

            // Remove Prompt symbol
            _removePrompt();

            APP_METROLOGY_GetMeasure(MEASURE_QT, &qt, app_consoleData.perCyclePar);
            APP_METROLOGY_GetMeasure(MEASURE_QA, &qa, app_consoleData.perCyclePar);
            APP_METROLOGY_GetMeasure(MEASURE_QB, &qb, app_consoleData.perCyclePar);
            APP_METROLOGY_GetMeasure(MEASURE_QC, &qc, app_consoleData.perCyclePar);
            // Show received data on console
            SYS_CMD_PRINT("Present reactive power%s is :\r\nQt=%.1fVAr Qa=%.1fVAr Qb=%.1fVAr Qc=%.1fVAr\r\n",
                   perCycle[app_consoleData.perCyclePar], qt, qa, qb, qc);

            // Go back to IDLE
            app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            break;
        }

        case APP_CONSOLE_STATE_PRINT_APARENT_POWER:
        {
            float st, sa, sb, sc;

            // Remove Prompt symbol
            _removePrompt();

            APP_METROLOGY_GetMeasure(MEASURE_ST, &st, app_consoleData.perCyclePar);
            APP_METROLOGY_GetMeasure(MEASURE_SA, &sa, app_consoleData.perCyclePar);
            APP_METROLOGY_GetMeasure(MEASURE_SB, &sb, app_consoleData.perCyclePar);
            APP_METROLOGY_GetMeasure(MEASURE_SC, &sc, app_consoleData.perCyclePar);
            // Show received data on console
            SYS_CMD_PRINT("Present apparent power%s is :\r\nSt=%.1fVA Sa=%.1fVA Sb=%.1fVA Sc=%.1fVA\r\n",
                   perCycle[app_consoleData.perCyclePar], st, sa, sb, sc);

            // Go back to IDLE
            app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            break;
        }

        case APP_CONSOLE_STATE_PRINT_FUNDAMENTAL_VOLTAGE:
        {
            float va, vb, vc;

            // Remove Prompt symbol
            _removePrompt();

            APP_METROLOGY_GetMeasure(MEASURE_UAF_RMS, &va, app_consoleData.perCyclePar);
            APP_METROLOGY_GetMeasure(MEASURE_UBF_RMS, &vb, app_consoleData.perCyclePar);
            APP_METROLOGY_GetMeasure(MEASURE_UCF_RMS, &vc, app_consoleData.perCyclePar);
            // Show received data on console
            SYS_CMD_PRINT("Present voltage%s (fundamental) is :\r\nUaf=%.3fV Ubf=%.3fV Ucf=%.3fV\r\n",
                perCycle[app_consoleData.perCyclePar], va, vb, vc);

            // Go back to IDLE
            app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            break;
        }

        case APP_CONSOLE_STATE_PRINT_FUNDAMENTAL_CURRENT:
        {
            float ia, ib, ic, inm;

            // Remove Prompt symbol
            _removePrompt();

            APP_METROLOGY_GetMeasure(MEASURE_IAF_RMS, &ia, app_consoleData.perCyclePar);
            APP_METROLOGY_GetMeasure(MEASURE_IBF_RMS, &ib, app_consoleData.perCyclePar);
            APP_METROLOGY_GetMeasure(MEASURE_ICF_RMS, &ic, app_consoleData.perCyclePar);
            APP_METROLOGY_GetMeasure(MEASURE_IMNF_RMS, &inm, app_consoleData.perCyclePar);
            // Show received data on console
            SYS_CMD_PRINT("Present current%s (fundamental) is :\r\nIaf=%.4fA Ibf=%.4fA Icf=%.4fA Inmf=%.4fA\r\n",
                    perCycle[app_consoleData.perCyclePar], ia, ib, ic, inm);

            // Go back to IDLE
            app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            break;
        }

        case APP_CONSOLE_STATE_PRINT_FUNDAMENTAL_ACTIVE_POWER:
        {
            float pt, pa, pb, pc;

            // Remove Prompt symbol
            _removePrompt();

            APP_METROLOGY_GetMeasure(MEASURE_PTF, &pt, app_consoleData.perCyclePar);
            APP_METROLOGY_GetMeasure(MEASURE_PAF, &pa, app_consoleData.perCyclePar);
            APP_METROLOGY_GetMeasure(MEASURE_PBF, &pb, app_consoleData.perCyclePar);
            APP_METROLOGY_GetMeasure(MEASURE_PCF, &pc, app_consoleData.perCyclePar);
            // Show received data on console
            SYS_CMD_PRINT("Present active power%s (fundamental) is :\r\nPtf=%.1fW Paf=%.1fW Pbf=%.1fW Pcf=%.1fW\r\n",
                   perCycle[app_consoleData.perCyclePar], pt, pa, pb, pc);

            // Go back to IDLE
            app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            break;
        }

        case APP_CONSOLE_STATE_PRINT_FUNDAMENTAL_REACTIVE_POWER:
        {
            float qt, qa, qb, qc;

            // Remove Prompt symbol
            _removePrompt();

            APP_METROLOGY_GetMeasure(MEASURE_QTF, &qt, app_consoleData.perCyclePar);
            APP_METROLOGY_GetMeasure(MEASURE_QAF, &qa, app_consoleData.perCyclePar);
            APP_METROLOGY_GetMeasure(MEASURE_QBF, &qb, app_consoleData.perCyclePar);
            APP_METROLOGY_GetMeasure(MEASURE_QCF, &qc, app_consoleData.perCyclePar);
            // Show received data on console
            SYS_CMD_PRINT("Present reactive power%s (fundamental) is :\r\nQtf=%.1fVAr Qaf=%.1fVAr Qbf=%.1fVAr Qcf=%.1fVAr\r\n",
                   perCycle[app_consoleData.perCyclePar],qt, qa, qb, qc);

            // Go back to IDLE
            app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            break;
        }

        case APP_CONSOLE_STATE_PRINT_FUNDAMENTAL_APARENT_POWER:
        {
            float st, sa, sb, sc;

            // Remove Prompt symbol
            _removePrompt();

            APP_METROLOGY_GetMeasure(MEASURE_STF, &st, app_consoleData.perCyclePar);
            APP_METROLOGY_GetMeasure(MEASURE_SAF, &sa, app_consoleData.perCyclePar);
            APP_METROLOGY_GetMeasure(MEASURE_SBF, &sb, app_consoleData.perCyclePar);
            APP_METROLOGY_GetMeasure(MEASURE_SCF, &sc, app_consoleData.perCyclePar);
            // Show received data on console
            SYS_CMD_PRINT("Present apparent power%s (fundamental) is :\r\nSt=%.1fVA Sa=%.1fVA Sb=%.1fVA Sc=%.1fVA\r\n",
                   perCycle[app_consoleData.perCyclePar], st, sa, sb, sc);

            // Go back to IDLE
            app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            break;
        }

        case APP_CONSOLE_STATE_PRINT_FREQUENCY:
        {
            float freq;

            // Remove Prompt symbol
            _removePrompt();

            APP_METROLOGY_GetMeasure(MEASURE_FREQ, &freq, app_consoleData.perCyclePar);
            // Show received data on console
            SYS_CMD_PRINT("Present frequency%s is : \r\nFreq=%.2fHz\r\n", perCycle[app_consoleData.perCyclePar], freq);

            // Go back to IDLE
            app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            break;
        }

        case APP_CONSOLE_STATE_PRINT_TOTAL_FREQUENCY:
        {
            float freq, freqA, freqB, freqC;

            // Remove Prompt symbol
            _removePrompt();

            APP_METROLOGY_GetMeasure(MEASURE_FREQ, &freq, app_consoleData.perCyclePar);
            APP_METROLOGY_GetMeasure(MEASURE_FREQA, &freqA, app_consoleData.perCyclePar);
            APP_METROLOGY_GetMeasure(MEASURE_FREQB, &freqB, app_consoleData.perCyclePar);
            APP_METROLOGY_GetMeasure(MEASURE_FREQC, &freqC, app_consoleData.perCyclePar);
            // Show received data on console
            SYS_CMD_PRINT("Dominant frequency%s is : \r\nFreq=%.2fHz\r\n", perCycle[app_consoleData.perCyclePar], freq);
            SYS_CMD_PRINT("FreqA=%.2fHz, FreqB=%.2fHz, FreqC=%.2fHz\r\n", freqA, freqB, freqC);

            // Go back to IDLE
            app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            break;
        }

        case APP_CONSOLE_STATE_PRINT_ANGLE:
        {
            float aa, ab, ac, an, aab, aac;

            // Remove Prompt symbol
            _removePrompt();

            APP_METROLOGY_GetMeasure(MEASURE_ANGLEA, &aa, app_consoleData.perCyclePar);
            APP_METROLOGY_GetMeasure(MEASURE_ANGLEB, &ab, app_consoleData.perCyclePar);
            APP_METROLOGY_GetMeasure(MEASURE_ANGLEC, &ac, app_consoleData.perCyclePar);
            APP_METROLOGY_GetMeasure(MEASURE_ANGLEN, &an, app_consoleData.perCyclePar);
            APP_METROLOGY_GetMeasure(MEASURE_ANGLEVAB, &aab, app_consoleData.perCyclePar);
            APP_METROLOGY_GetMeasure(MEASURE_ANGLEVAC, &aac, app_consoleData.perCyclePar);
            // Show received data on console
            SYS_CMD_PRINT("Voltage and current angle%s is : \r\nAngle_A=%.3f Angle_B=%.3f Angle_C=%.3f Angle_N=%.3f Angle_VAB=%.3f Angle_VAC=%.3f\r\n",
                     perCycle[app_consoleData.perCyclePar], aa, ab, ac, an, aab, aac);

            // Go back to IDLE
            app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            break;
        }

        case APP_CONSOLE_STATE_PRINT_WAVEFORM_DATA:
        {
            uint8_t idx;

            if (app_consoleData.rawDataFlag)
            {
                _removePrompt();
                app_consoleData.rawDataFlag = false;
            }

            for (idx = 0; idx < 10; idx++) {
                if (app_consoleData.rawDataLen > 0)
                {
                    // Print value
                    SYS_CMD_PRINT("%08X\r\n", *(app_consoleData.rawData));
                    // Advance to next value
                    app_consoleData.rawData++;
                    app_consoleData.rawDataLen--;
                }
                else
                {
                    // All registers have been read
                    app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
                    break;
                }
            }

            app_consoleData.nextState = app_consoleData.state;
            app_consoleData.state = APP_CONSOLE_STATE_DELAY;
            app_consoleData.delayMs = CONSOLE_TASK_DEFAULT_DELAY_MS_BETWEEN_STATES;
            break;
        }

        case APP_CONSOLE_STATE_PRINT_CALIBRATION_RESULT:
        {
            // Remove Prompt symbol
            _removePrompt();

            // Show calibration result
            if (app_consoleData.calibrationResult)
            {
                SYS_CMD_MESSAGE("Calibrating Done!\r\n");
            }
            else
            {
                SYS_CMD_MESSAGE("Calibrating Fails!\r\n");
            }

            // Go back to IDLE
            app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            break;
        }

        case APP_CONSOLE_STATE_PRINT_HELP:
        {
            uint8_t idx;
            uint8_t idxMax = 2;

            if (app_consoleData.cmdNumToShowHelp > 0)
            {
                if (app_consoleData.cmdNumToShowHelp < 5)
                {
                    idxMax = app_consoleData.cmdNumToShowHelp;
                }

                for (idx = 0; idx < idxMax; idx++, app_consoleData.pCmdDescToShowHelp++)
                {
                    SYS_CMD_PRINT("%s\t%s\r\n", app_consoleData.pCmdDescToShowHelp->cmdStr,
                            app_consoleData.pCmdDescToShowHelp->cmdDescr);
                    app_consoleData.cmdNumToShowHelp--;
                }
            }
            else
            {
                // All commands have been represented
                app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
                break;
            }

            app_consoleData.nextState = app_consoleData.state;
            app_consoleData.state = APP_CONSOLE_STATE_DELAY;
            app_consoleData.delayMs = CONSOLE_TASK_DEFAULT_DELAY_MS_BETWEEN_STATES;
            break;
        }

        case APP_CONSOLE_STATE_LOW_POWER_MODE:
        {
            // Remove Prompt symbol
            _removePrompt();

            SYS_CMD_MESSAGE("Entering Low Power... Press FWUP/TAMPER switch to wake up.\r\n");

            // Update display info
            APP_DISPLAY_ShowLowPowerMode();

            app_consoleData.nextState = app_consoleData.state;
            app_consoleData.state = APP_CONSOLE_STATE_DELAY;
            app_consoleData.delayMs = 100;
            break;
        }

        case APP_CONSOLE_STATE_SW_RESET:
        {
            /* Wait time to show message through the Console */
            app_consoleData.nextState = app_consoleData.state;
            app_consoleData.state = APP_CONSOLE_STATE_DELAY;
            app_consoleData.delayMs = 100;
            break;
        }

        case APP_CONSOLE_STATE_READ_CHANNELS_CONFIG:
        {
            DRV_METROLOGY_CHANNEL *pChannels;
            DRV_METROLOGY_AFE_TYPE afeType;
            uint8_t index;
            uint8_t indexMax;

            // Remove Prompt symbol
            _removePrompt();

            // Read AFE devices
            afeType = APP_METROLOGY_GetAFEDescription(gConsoleAFEDesc);
            SYS_CMD_PRINT("AFE device: %s (type %u)\r\n\r\n", gConsoleAFEDesc, afeType);

            // Read channels configuration
            SYS_CMD_PRINT("Channel X:\tName\tGAIN\tSENSOR TYPE\r\n");
            indexMax = APP_METROLOGY_GetChannelsDescription(&pChannels);
            for (index = 0; index < indexMax; index++)
            {
                SYS_CMD_PRINT("Channel %u:\t%s\tGAIN_%u\t%s\r\n", index,
                              pChannels->name, 1 << pChannels->gain,
                              gConsoleSensorTypes[pChannels->sensorType]);
                pChannels++;
            }

            // Go back to Idle
            app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            break;
        }

        case APP_CONSOLE_STATE_DELAY:
        {
            // Wait delay time
            if (APP_CONSOLE_TaskDelay(app_consoleData.delayMs, &app_consoleData.timer))
            {
                // Check low power state
                if (app_consoleData.nextState == APP_CONSOLE_STATE_LOW_POWER_MODE)
                {
                    // Go to Low Power mode
                    APP_METROLOGY_SetLowPowerMode();

                    // Execution should not come here during normal operation
                }
                else if (app_consoleData.nextState == APP_CONSOLE_STATE_SW_RESET)
                {
                    // Stop Metrology and its peripherals before reset
                    APP_METROLOGY_StopMetrology();
                    // Perform Reset
                    RSTC_Reset(RSTC_PROCESSOR_RESET);

                    // Execution should not come here during normal operation
                }
                else
                {
                    // Set next app state
                    app_consoleData.state = app_consoleData.nextState;
                }
            }
            break;
        }

        case APP_CONSOLE_STATE_SYN_DISABLE:
        {
            // Remove Prompt symbol
            _removePrompt();

            // Stop Synthesizer
            APP_METROLOGY_GetControlRegister(CONTROL_FEATURE_CTRL, &regValue32[0], regName[0]);
            regValue32[0] &= ~FEATURE_CTRL_SYNTH_EN_Msk;
            APP_METROLOGY_SetControlRegister(CONTROL_FEATURE_CTRL, regValue32[0]);

            // disable synthesizer
            app_consoleData.synIsRunning = false;

            // Free all reserved memory used in descriptors and samples data
            _freeSyncDescriptors(app_consoleData.pSynCurrentDesc);

            SYS_CMD_MESSAGE("Synthesizer stopped\r\n");

            // Go back to Idle
            app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            break;
        }

        case APP_CONSOLE_STATE_SYN_ENABLE:
        {
            DRV_METROLOGY_SYN_DESCRIPTOR *pDesc;

            // Remove Prompt symbol
            _removePrompt();

            // Check if at least 1 samples data has been configured
            pDesc = app_consoleData.pSynDescriptor;
            if ((pDesc != NULL) && (pDesc->pData != NULL))
            {
                // Set Synthesizer descriptor address
                regValue32[0] = (uint32_t)app_consoleData.pSynDescriptor;
                APP_METROLOGY_SetControlRegister(CONTROL_SYNTH_ADDR, regValue32[0]);

                // Start Synthesizer
                APP_METROLOGY_GetControlRegister(CONTROL_FEATURE_CTRL, &regValue32[0], regName[0]);
                regValue32[0] |= FEATURE_CTRL_SYNTH_EN_Msk;
                APP_METROLOGY_SetControlRegister(CONTROL_FEATURE_CTRL, regValue32[0]);

                // enable synthesizer
                app_consoleData.synIsRunning = true;

                SYS_CMD_MESSAGE("Synthesizer started\r\n");
            }
            else
            {
                SYS_CMD_MESSAGE("Synthesizer fails\r\n");
            }

            // Go back to Idle
            app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            break;
        }

        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}


/*******************************************************************************
 End of File
 */
