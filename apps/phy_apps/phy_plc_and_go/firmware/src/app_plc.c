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

/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_plc.c

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

#include <string.h>
#include "definitions.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************
#define DRV_PLC_PHY_INDEX_0   0

#define div_round(a, b)      (((a) + (b >> 1)) / (b))

/* PLC Driver Initialization Data (initialization.c) */
extern DRV_PLC_PHY_INIT drvPlcPhyInitData;

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_PLC_DATA appPlc;
APP_PLC_DATA_TX appPlcTx;

static CACHE_ALIGN uint8_t appPlcPibDataBuffer[CACHE_ALIGNED_SIZE_GET(APP_PLC_PIB_BUFFER_SIZE)];
static CACHE_ALIGN uint8_t appPlcTxDataBuffer[CACHE_ALIGNED_SIZE_GET(APP_PLC_BUFFER_SIZE)];
static CACHE_ALIGN uint8_t appPlcStaticNotching[CACHE_ALIGNED_SIZE_GET(NUM_CARRIERS_CENELEC_A)] = APP_PLC_TONE_MASK_STATIC_NOTCHING_EXAMPLE;

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

static void APP_PLC_SetInitialConfiguration ( void )
{
    DRV_PLC_PHY_PIB_OBJ pibObj;
    uint8_t plcCrcEnable;
    bool applyStaticNotching = false;

    /* Apply PLC coupling configuration */
    SRV_PCOUP_Set_Config(appPlc.drvPlcHandle, appPlcTx.couplingBranch);

    /* Force Transmission to VLO mode by default in order to maximize signal level in anycase */
    /* Disable autodetect mode */
    appPlcTx.txAuto = 0;
    pibObj.id = PLC_ID_CFG_AUTODETECT_IMPEDANCE;
    pibObj.length = 1;
    pibObj.pData = (uint8_t *)&appPlcTx.txAuto;
    DRV_PLC_PHY_PIBSet(appPlc.drvPlcHandle, &pibObj);

    /* Set VLO mode */
    appPlcTx.txImpedance = 2;
    pibObj.id = PLC_ID_CFG_IMPEDANCE;
    pibObj.length = 1;
    pibObj.pData = (uint8_t *)&appPlcTx.txImpedance;
    DRV_PLC_PHY_PIBSet(appPlc.drvPlcHandle, &pibObj);

    /* Get PLC PHY version */
    pibObj.id = PLC_ID_VERSION_NUM;
    pibObj.length = 4;
    pibObj.pData = (uint8_t *)&appPlcTx.plcPhyVersion;
    DRV_PLC_PHY_PIBGet(appPlc.drvPlcHandle, &pibObj);

    /* Adjust ToneMap Info */
    switch ((uint8_t)(appPlcTx.plcPhyVersion >> 16))
    {
        case 1:
            /* CEN A */
            appPlcTx.toneMapSize = TONE_MAP_SIZE_CENELEC;
            appPlcTx.plcPhyTx.toneMap[0] = 0x3F;
            if (appPlc.staticNotchingEnable)
            {
                /* Caution: Example provided only for CEN-A band */
                applyStaticNotching = true;
            }
            break;

        case 2:
            /* FCC */
            appPlcTx.toneMapSize = TONE_MAP_SIZE_FCC;
            appPlcTx.plcPhyTx.toneMap[0] = 0xFF;
            appPlcTx.plcPhyTx.toneMap[1] = 0xFF;
            appPlcTx.plcPhyTx.toneMap[2] = 0xFF;
            break;

        case 3:
            /* ARIB */
            appPlcTx.toneMapSize = TONE_MAP_SIZE_ARIB;
            appPlcTx.plcPhyTx.toneMap[0] = 0x03;
            appPlcTx.plcPhyTx.toneMap[1] = 0xFF;
            appPlcTx.plcPhyTx.toneMap[2] = 0xFF;
            break;

        case 4:
            /* CEN B */
            appPlcTx.toneMapSize = TONE_MAP_SIZE_CENELEC;
            appPlcTx.plcPhyTx.toneMap[0] = 0x0F;
            break;
    }

    /* Enable CRC calculation for transmission and reception in PLC PHY layer */
    /* In Transmission, 16-bit CRC is computed and added to data payload by PHY layer in PLC device */
    /* In Reception, CRC is checked by PHY layer in PLC device and the result is reported in crcOk field in DRV_PLC_PHY_RECEPTION_OBJ structure */
    /* The CRC format is the same that uses the G3-PLC stack, which is described in the IEEE 802.15.4 standard. */
    plcCrcEnable = 1;
    pibObj.id = PLC_ID_CRC_TX_RX_CAPABILITY;
    pibObj.length = 1;
    pibObj.pData = (uint8_t *)&plcCrcEnable;
    DRV_PLC_PHY_PIBSet(appPlc.drvPlcHandle, &pibObj);

    if (applyStaticNotching)
    {
        /* Caution: Example provided only for CEN-A band */
        pibObj.id = PLC_ID_TONE_MASK;
        pibObj.length = NUM_CARRIERS_CENELEC_A;
        pibObj.pData = (uint8_t *)&appPlcStaticNotching;
        DRV_PLC_PHY_PIBSet(appPlc.drvPlcHandle, &pibObj);
    }

    /* Get maximum data length allowed with configured Tx Parameters */
    APP_PLC_SetModScheme(appPlcTx.plcPhyTx.modType, appPlcTx.plcPhyTx.modScheme);
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

void APP_PLC_Timer1_Callback (uintptr_t context)
{
    appPlc.tmr1Expired = true;
}

void APP_PLC_Timer2_Callback (uintptr_t context)
{
    appPlc.tmr2Expired = true;
}

#ifndef APP_PLC_DISABLE_PVDDMON
static void APP_PLC_PVDDMonitorCb( SRV_PVDDMON_CMP_MODE cmpMode, uintptr_t context )
{
    (void)context;

    if (cmpMode == SRV_PVDDMON_CMP_MODE_OUT)
    {
        /* PLC Transmission is not permitted */
        DRV_PLC_PHY_EnableTX(appPlc.drvPlcHandle, false);
        appPlc.pvddMonTxEnable = false;
        /* Restart PVDD Monitor to check when VDD is within the comparison window */
        SRV_PVDDMON_Restart(SRV_PVDDMON_CMP_MODE_IN);
    }
    else
    {
        /* PLC Transmission is permitted again */
        DRV_PLC_PHY_EnableTX(appPlc.drvPlcHandle, true);
        appPlc.pvddMonTxEnable = true;
        /* Restart PVDD Monitor to check when VDD is out of the comparison window */
        SRV_PVDDMON_Restart(SRV_PVDDMON_CMP_MODE_OUT);
    }
}
#endif

static void APP_PLC_SleepModeDisableCb( uintptr_t context )
{
    /* Avoid warning */
    (void)context;

    /* Apply PLC initial configuration */
    APP_PLC_SetInitialConfiguration();

    /* Set PLC state */
    appPlc.state = APP_PLC_STATE_WAITING;
}

static void APP_PLC_ExceptionCb(DRV_PLC_PHY_EXCEPTION exceptionObj, uintptr_t context )
{
    /* Avoid warning */
    (void)context;
    (void)exceptionObj;

    /* Update PLC TX Status */
    appPlc.plcTxState = APP_PLC_TX_STATE_IDLE;
    /* Go to Exception state to restart PLC Driver */
    appPlc.state = APP_PLC_STATE_EXCEPTION;
}

static void APP_PLC_DataCfmCb(DRV_PLC_PHY_TRANSMISSION_CFM_OBJ *cfmObj, uintptr_t context )
{
    /* Avoid warning */
    (void)context;

    /* Update PLC TX Status */
    appPlc.plcTxState = APP_PLC_TX_STATE_IDLE;

    /* Capture TX result of the last transmission */
    appPlc.lastTxResult = cfmObj->result;
}

static void APP_PLC_DataIndCb( DRV_PLC_PHY_RECEPTION_OBJ *indObj, uintptr_t context )
{
    /* Avoid warning */
    (void)context;

    if (indObj->crcOk == 1)
    {
        uint16_t us_len;

        us_len = (uint16_t)indObj->pReceivedData[0] << 8;
        us_len += (uint16_t)indObj->pReceivedData[1];

        if (us_len > (indObj->dataLength))
        {
            /* Length error: length in message content should never be more than total data length from PHY */
            APP_CONSOLE_Print("\rRx ERROR: length error\r\n");
        }
        else
        {
            /* Turn on indication LED and start timer to turn it off */
            SYS_TIME_TimerDestroy(appPlc.tmr2Handle);
            USER_PLC_IND_LED_On();
            appPlc.tmr2Handle = SYS_TIME_CallbackRegisterMS(APP_PLC_Timer2_Callback, 0, LED_PLC_RX_MSG_RATE_MS, SYS_TIME_SINGLE);

            APP_CONSOLE_Print("\rRx (");
            /* Show Modulation of received frame */
            if (indObj->modScheme == MOD_SCHEME_DIFFERENTIAL)
            {
                if (indObj->modType == MOD_TYPE_BPSK_ROBO)
                {
                    APP_CONSOLE_Print("BPSK Robust Differential, ");
                }
                else if (indObj->modType == MOD_TYPE_BPSK)
                {
                    APP_CONSOLE_Print("BPSK Differential, ");
                }
                else if (indObj->modType == MOD_TYPE_QPSK)
                {
                    APP_CONSOLE_Print("QPSK Differential, ");
                }
                else if (indObj->modType == MOD_TYPE_8PSK)
                {
                    APP_CONSOLE_Print("8PSK Differential, ");
                }
            }
            else
            {   /* MOD_SCHEME_COHERENT */
                if (indObj->modType == MOD_TYPE_BPSK_ROBO)
                {
                    APP_CONSOLE_Print("BPSK Robust Coherent, ");
                }
                else if (indObj->modType == MOD_TYPE_BPSK)
                {
                    APP_CONSOLE_Print("BPSK Coherent, ");
                }
                else if (indObj->modType == MOD_TYPE_QPSK)
                {
                    APP_CONSOLE_Print("QPSK Coherent, ");
                }
                else if (indObj->modType == MOD_TYPE_8PSK)
                {
                    APP_CONSOLE_Print("8PSK Coherent, ");
                }
            }
            /* Show RSSI (Received Signal Strength Indicator) in dBuV */
            APP_CONSOLE_Print("RSSI %udBuV, ", indObj->rssi);
            /* Show LQI (Link Quality Indicator). It is in quarters of dB and 10-dB offset: SNR(dB) = (LQI - 40) / 4 */
            APP_CONSOLE_Print("LQI %ddB): ", div_round((int16_t)indObj->lqi - 40, 4));
            APP_CONSOLE_Print("%.*s", us_len, indObj->pReceivedData + 2);
        }
    }
    else
    {
        APP_CONSOLE_Print("\rRx ERROR: CRC error\r\n");
    }

    APP_CONSOLE_Print(MENU_CMD_PROMPT);
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************
/*******************************************************************************
  Function:
    void APP_PLC_Initialize(void)

  Remarks:
    See prototype in app_plc.h.
 */
void APP_PLC_Initialize ( void )
{
    /* Init PLC PIB buffer */
    appPlc.plcPIB.pData = appPlcPibDataBuffer;

    /* Init PLC objects */
    appPlcTx.pDataTx = appPlcTxDataBuffer;
    appPlcTx.plcPhyTx.pTransmitData = appPlcTx.pDataTx;

    /* Set PLC state */
    appPlc.state = APP_PLC_STATE_IDLE;

    /* Set PVDD Monitor tracking data */
    appPlc.pvddMonTxEnable = true;

    /* Init PLC TX status */
    appPlc.plcTxState = APP_PLC_TX_STATE_IDLE;

    /* Init Timer handler */
    appPlc.tmr1Handle = SYS_TIME_HANDLE_INVALID;
    appPlc.tmr2Handle = SYS_TIME_HANDLE_INVALID;
    appPlc.tmr1Expired = false;
    appPlc.tmr2Expired = false;

}

/******************************************************************************
  Function:
    void APP_PLC_Tasks ( void )

  Remarks:
    See prototype in app_plc.h.
 */

void APP_PLC_Tasks ( void )
{
    /* Signalling */
    if (appPlc.tmr1Expired)
    {
        appPlc.tmr1Expired = false;
        USER_BLINK_LED_Toggle();
    }

    if (appPlc.tmr2Expired)
    {
        appPlc.tmr2Expired = false;
        USER_PLC_IND_LED_Off();
    }

    /* Check the application's current state. */
    switch ( appPlc.state )
    {
        case APP_PLC_STATE_IDLE:
        {
            /* Set Static Notching capability (example only valid for CEN-A band */
            /* Caution: Example provided only for CEN-A band */
            appPlc.staticNotchingEnable = APP_PLC_STATIC_NOTCHING_ENABLE;

            /* Set configuration by default */
            appPlcTx.plcPhyTx.timeIni = 0;
            appPlcTx.plcPhyTx.attenuation = 0;
            appPlcTx.plcPhyTx.modScheme = MOD_SCHEME_DIFFERENTIAL;
            appPlcTx.plcPhyTx.modType = MOD_TYPE_BPSK;
            appPlcTx.plcPhyTx.delimiterType = DT_SOF_NO_RESP;
            appPlcTx.plcPhyTx.mode = TX_MODE_FORCED | TX_MODE_RELATIVE;
            /* Set 1 Reed-Solomon block. In this example it cannot be configured dynamically. To test 2 Reed-Solomon blocks change 0 by 1 (Only for FCC). */
            appPlcTx.plcPhyTx.rs2Blocks = 0;
            appPlcTx.plcPhyTx.pdc = 0;
            appPlcTx.plcPhyTx.pTransmitData = appPlcTx.pDataTx;
            appPlcTx.plcPhyTx.dataLength = 0;

            memset(appPlcTx.plcPhyTx.preemphasis, 0, sizeof(appPlcTx.plcPhyTx.preemphasis));

            /* Set PLC Multiband / Couling Branch flag */
            appPlcTx.couplingBranch = SRV_PCOUP_Get_Default_Branch();
            if (SRV_PCOUP_Get_Config(SRV_PLC_PCOUP_AUXILIARY_BRANCH) == NULL) {
                /* Auxiliary branch is not configured. Single branch */
                appPlc.plcMultiband = false;
                appPlcTx.bin2InUse = false;
            } else {
                /* Dual branch */
                appPlc.plcMultiband = true;
                if (appPlcTx.couplingBranch == SRV_PLC_PCOUP_MAIN_BRANCH)
                {
                    appPlcTx.bin2InUse = false;
                }
                else
                {
                    appPlcTx.bin2InUse = true;
                }
            }

            /* Initialize PLC driver */
            appPlc.state = APP_PLC_STATE_INIT;
        }
        break;

        case APP_PLC_STATE_INIT:
        {
            SYS_STATUS drvPlcStatus = DRV_PLC_PHY_Status(DRV_PLC_PHY_INDEX);

            /* Select PLC Binary file for multi-band solution */
            if (appPlc.plcMultiband && (drvPlcStatus == SYS_STATUS_UNINITIALIZED))
            {
                if (appPlcTx.bin2InUse)
                {
                    drvPlcPhyInitData.binStartAddress = (uint32_t)&plc_phy_bin2_start;
                    drvPlcPhyInitData.binEndAddress = (uint32_t)&plc_phy_bin2_end;
                    /* Set Coupling Auxiliary branch */
                    appPlcTx.couplingBranch = SRV_PLC_PCOUP_AUXILIARY_BRANCH;
                }
                else
                {
                    drvPlcPhyInitData.binStartAddress = (uint32_t)&plc_phy_bin_start;
                    drvPlcPhyInitData.binEndAddress = (uint32_t)&plc_phy_bin_end;
                    /* Set Coupling Main branch */
                    appPlcTx.couplingBranch = SRV_PLC_PCOUP_MAIN_BRANCH;
                }

                /* Initialize PLC Driver Instance */
                sysObj.drvPlcPhy = DRV_PLC_PHY_Initialize(DRV_PLC_PHY_INDEX, (SYS_MODULE_INIT *)&drvPlcPhyInitData);
                /* Register Callback function to handle PLC interruption */
                PIO_PinInterruptCallbackRegister(DRV_PLC_EXT_INT_PIN, DRV_PLC_PHY_ExternalInterruptHandler, sysObj.drvPlcPhy);
            }

            /* Open PLC driver */
            appPlc.drvPlcHandle = DRV_PLC_PHY_Open(DRV_PLC_PHY_INDEX_0, NULL);

            if (appPlc.drvPlcHandle != DRV_HANDLE_INVALID)
            {
                appPlc.state = APP_PLC_STATE_OPEN;
            }
            else
            {
                appPlc.state = APP_PLC_STATE_ERROR;
            }
            break;
        }

        case APP_PLC_STATE_OPEN:
        {
            /* Check PLC transceiver */
            if (DRV_PLC_PHY_Status(DRV_PLC_PHY_INDEX_0) == SYS_STATUS_READY)
            {
                /* Configure PLC callbacks */
                DRV_PLC_PHY_ExceptionCallbackRegister(appPlc.drvPlcHandle, APP_PLC_ExceptionCb, 0);
                DRV_PLC_PHY_TxCfmCallbackRegister(appPlc.drvPlcHandle, APP_PLC_DataCfmCb, 0);
                DRV_PLC_PHY_DataIndCallbackRegister(appPlc.drvPlcHandle, APP_PLC_DataIndCb, 0);
                DRV_PLC_PHY_SleepDisableCallbackRegister(appPlc.drvPlcHandle, APP_PLC_SleepModeDisableCb, 0);

                /* Apply PLC initial configuration */
                APP_PLC_SetInitialConfiguration();

#ifndef APP_PLC_DISABLE_PVDDMON                
                /* Disable TX Enable at the beginning */
                DRV_PLC_PHY_EnableTX(appPlc.drvPlcHandle, false);
                appPlc.pvddMonTxEnable = false;
                /* Enable PLC PVDD Monitor Service */
                SRV_PVDDMON_CallbackRegister(APP_PLC_PVDDMonitorCb, 0);
                SRV_PVDDMON_Start(SRV_PVDDMON_CMP_MODE_IN);
#else
                /* Enable TX Enable at the beginning */
                DRV_PLC_PHY_EnableTX(appPlc.drvPlcHandle, true);
                appPlc.pvddMonTxEnable = true;
#endif

                /* Init Timer to handle blinking led */
                appPlc.tmr1Handle = SYS_TIME_CallbackRegisterMS(APP_PLC_Timer1_Callback, 0, LED_BLINK_RATE_MS, SYS_TIME_PERIODIC);

                /* Set PLC state */
                appPlc.state = APP_PLC_STATE_WAITING;
            }
        }
        break;

        case APP_PLC_STATE_WAITING:
        {
            break;
        }

        case APP_PLC_STATE_WAITING_TX_CFM:
        {
            if (appPlc.plcTxState != APP_PLC_TX_STATE_WAIT_TX_CFM)
            {
                 appPlc.state = APP_PLC_STATE_WAITING;
            }
            break;
        }

        case APP_PLC_STATE_SET_BAND:
        {
            if (!appPlc.plcMultiband)
            {
                /* PLC Multi-band is not supported */
                appPlc.state = APP_PLC_STATE_WAITING;
                break;
            }

            /* Close PLC Driver */
            DRV_PLC_PHY_Close(appPlc.drvPlcHandle);

            /* Stop timer */
            SYS_TIME_TimerDestroy(appPlc.tmr1Handle);

            /* Restart PLC Driver */
            appPlc.state = APP_PLC_STATE_INIT;
            appPlc.plcTxState = APP_PLC_TX_STATE_IDLE;
            break;
        }

        case APP_PLC_STATE_EXCEPTION:
        {
            /* Close Driver and go to INIT state for reinitialization */
            DRV_PLC_PHY_Close(appPlc.drvPlcHandle);
            appPlc.state = APP_PLC_STATE_INIT;
            SYS_TIME_TimerDestroy(appPlc.tmr1Handle);
            break;
        }

        /* The default state should never be executed. */
        default:
        {
            /* Handle error in application's state machine. */
            break;
        }
    }
}

bool APP_PLC_SendData ( uint8_t* pData, uint16_t length )
{
    if (appPlc.state == APP_PLC_STATE_WAITING)
    {
        if (appPlc.pvddMonTxEnable)
        {
            if ((length > 0) && ((length + 2) <= APP_PLC_BUFFER_SIZE))
            {
                /* Fill 2 first bytes with data length */
                /* Physical Layer may add padding bytes in order to complete symbols with data */
                /* It is needed to include real data length in the message because otherwise at reception is not possible to know if there is padding or not */
                appPlcTx.pDataTx[0] = length >> 8;
                appPlcTx.pDataTx[1] = length & 0xFF;

                /* Set data length in Tx Parameters structure */
                /* It should be equal or less than Maximum Data Length (see APP_PLC_SetModScheme) */
                /* Otherwise DRV_PLC_PHY_TX_RESULT_INV_LENGTH will be reported in Tx Confirm */
                appPlcTx.plcPhyTx.dataLength = length + 2;
                memcpy(appPlcTx.pDataTx + 2, pData, length);

                appPlc.plcTxState = APP_PLC_TX_STATE_WAIT_TX_CFM;

                DRV_PLC_PHY_TxRequest(appPlc.drvPlcHandle, &appPlcTx.plcPhyTx);

                /* Set PLC state */
                if (appPlc.plcTxState == APP_PLC_TX_STATE_WAIT_TX_CFM)
                {
                    appPlc.state = APP_PLC_STATE_WAITING_TX_CFM;
                    return true;
                }
            }
        }
    }

    return false;
}

void APP_PLC_SetModScheme ( DRV_PLC_PHY_MOD_TYPE modType, DRV_PLC_PHY_MOD_SCHEME modScheme )
{
    DRV_PLC_PHY_MAX_PSDU_LEN_PARAMS parameters;

    if (appPlc.state == APP_PLC_STATE_WAITING)
    {
        appPlcTx.plcPhyTx.modScheme = modScheme;
        appPlcTx.plcPhyTx.modType = modType;

        parameters.modScheme = modScheme;
        parameters.modType = modType;
        parameters.rs2Blocks = appPlcTx.plcPhyTx.rs2Blocks;
        memset(parameters.toneMap, 0, TONE_MAP_SIZE_MAX);
        memcpy(parameters.toneMap, appPlcTx.plcPhyTx.toneMap, appPlcTx.toneMapSize);

        /* Set parameters for MAX_PSDU_LEN computation in PLC PHY */
        appPlc.plcPIB.id = PLC_ID_MAX_PSDU_LEN_PARAMS;
        appPlc.plcPIB.length = sizeof(parameters);
        memcpy(appPlc.plcPIB.pData, (uint8_t *)&parameters, appPlc.plcPIB.length);
        DRV_PLC_PHY_PIBSet(appPlc.drvPlcHandle, &appPlc.plcPIB);

        /* Get MAX_PSDU_LEN from PLC PHY */
        appPlc.plcPIB.id = PLC_ID_MAX_PSDU_LEN;
        appPlc.plcPIB.length = 2;
        DRV_PLC_PHY_PIBGet(appPlc.drvPlcHandle, &appPlc.plcPIB);
        appPlcTx.maxPsduLen = appPlc.plcPIB.pData[0];
        appPlcTx.maxPsduLen += (uint16_t)appPlc.plcPIB.pData[1] << 8;
    }
}

bool APP_PLC_SetSleepMode ( bool enable )
{
    bool sleepIsEnabled = (appPlc.state == APP_PLC_STATE_SLEEP);

    if (sleepIsEnabled != enable)
    {
        DRV_PLC_PHY_Sleep(appPlc.drvPlcHandle, enable);
        if (enable)
        {
            appPlc.state = APP_PLC_STATE_SLEEP;
        }

        return true;
    }

    return false;
}

/*******************************************************************************
 End of File
 */
