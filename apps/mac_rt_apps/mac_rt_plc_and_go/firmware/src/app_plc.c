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
#define DRV_G3_MACRT_INDEX_0   0

#define div_round(a, b)      (((a) + (b >> 1)) / (b))

extern uint8_t g3_mac_rt_bin_start;
extern uint8_t g3_mac_rt_bin_end;
extern uint8_t g3_mac_rt_bin2_start;
extern uint8_t g3_mac_rt_bin2_end;

/* G3 MAC RT Driver Initialization Data (initialization.c) */
extern DRV_G3_MACRT_INIT drvG3MacRtInitData;

// *****************************************************************************
/* Pointer to PLC Coupling configuration data

  Summary:
    Holds PLC configuration data

  Description:
    This structure holds the PLC coupling configuration data.

  Remarks:
    Parameters are defined in srv_pcoup.h file
 */

SRV_PLC_PCOUP *appPLCCoupling;

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

static CACHE_ALIGN uint8_t appPlcPibTxFrameBuffer[CACHE_ALIGNED_SIZE_GET(MAC_RT_DATA_MAX_SIZE)];
static CACHE_ALIGN uint8_t appPlcPibRxFrameBuffer[CACHE_ALIGNED_SIZE_GET(MAC_RT_DATA_MAX_SIZE)];

static void APP_PLC_SetCouplingConfiguration ( SRV_PLC_PCOUP_BRANCH branch )
{
    appPLCCoupling = SRV_PCOUP_Get_Config(branch);

    appPlc.plcPIB.pib = MAC_RT_PIB_MANUF_PHY_PARAM;
    
    appPlc.plcPIB.index = PHY_PIB_PLC_IC_DRIVER_CFG;
    appPlc.plcPIB.length = 1;
    *appPlc.plcPIB.pData = appPLCCoupling->lineDrvConf;
    DRV_G3_MACRT_PIBSet(appPlc.drvPl360Handle, &appPlc.plcPIB);

    appPlc.plcPIB.index = PHY_PIB_DACC_TABLE_CFG;
    appPlc.plcPIB.length = sizeof(appPLCCoupling->daccTable);
    memcpy(appPlc.plcPIB.pData, (uint8_t *)appPLCCoupling->daccTable,
            appPlc.plcPIB.length);
    DRV_G3_MACRT_PIBSet(appPlc.drvPl360Handle, &appPlc.plcPIB);

    appPlc.plcPIB.index = PHY_PIB_NUM_TX_LEVELS;
    appPlc.plcPIB.length = 1;
    *appPlc.plcPIB.pData = appPLCCoupling->numTxLevels;
    DRV_G3_MACRT_PIBSet(appPlc.drvPl360Handle, &appPlc.plcPIB);

    appPlc.plcPIB.index = PHY_PIB_MAX_RMS_TABLE_HI;
    appPlc.plcPIB.length = sizeof(appPLCCoupling->rmsHigh);
    memcpy(appPlc.plcPIB.pData, (uint8_t *)appPLCCoupling->rmsHigh,
            appPlc.plcPIB.length);
    DRV_G3_MACRT_PIBSet(appPlc.drvPl360Handle, &appPlc.plcPIB);

    appPlc.plcPIB.index = PHY_PIB_MAX_RMS_TABLE_VLO;
    appPlc.plcPIB.length = sizeof(appPLCCoupling->rmsVLow);
    memcpy(appPlc.plcPIB.pData, (uint8_t *)appPLCCoupling->rmsVLow,
            appPlc.plcPIB.length);
    DRV_G3_MACRT_PIBSet(appPlc.drvPl360Handle, &appPlc.plcPIB);

    appPlc.plcPIB.index = PHY_PIB_THRESHOLDS_TABLE_HI;
    appPlc.plcPIB.length = sizeof(appPLCCoupling->thrsHigh);
    memcpy(appPlc.plcPIB.pData, (uint8_t *)appPLCCoupling->thrsHigh,
            appPlc.plcPIB.length);
    DRV_G3_MACRT_PIBSet(appPlc.drvPl360Handle, &appPlc.plcPIB);

    appPlc.plcPIB.index = PHY_PIB_THRESHOLDS_TABLE_VLO;
    appPlc.plcPIB.length = sizeof(appPLCCoupling->thrsVLow);
    memcpy(appPlc.plcPIB.pData, (uint8_t *)appPLCCoupling->thrsVLow,
            appPlc.plcPIB.length);
    DRV_G3_MACRT_PIBSet(appPlc.drvPl360Handle, &appPlc.plcPIB);

    appPlc.plcPIB.index = PHY_PIB_GAIN_TABLE_HI;
    appPlc.plcPIB.length = sizeof(appPLCCoupling->gainHigh);
    memcpy(appPlc.plcPIB.pData, (uint8_t *)appPLCCoupling->gainHigh,
            appPlc.plcPIB.length);
    DRV_G3_MACRT_PIBSet(appPlc.drvPl360Handle, &appPlc.plcPIB);

    appPlc.plcPIB.index = PHY_PIB_GAIN_TABLE_VLO;
    appPlc.plcPIB.length = sizeof(appPLCCoupling->gainVLow);
    memcpy(appPlc.plcPIB.pData, (uint8_t *)appPLCCoupling->gainVLow,
            appPlc.plcPIB.length);
    DRV_G3_MACRT_PIBSet(appPlc.drvPl360Handle, &appPlc.plcPIB);

    appPlc.plcPIB.index = PHY_PIB_PREDIST_COEF_TABLE_HI;
    appPlc.plcPIB.length = appPLCCoupling->equSize;
    memcpy(appPlc.plcPIB.pData, (uint8_t *)appPLCCoupling->equHigh,
            appPlc.plcPIB.length);
    DRV_G3_MACRT_PIBSet(appPlc.drvPl360Handle, &appPlc.plcPIB);

    appPlc.plcPIB.index = PHY_PIB_PREDIST_COEF_TABLE_VLO;
    /* Not use size of array. It depends on PHY band in use */
    appPlc.plcPIB.length = appPLCCoupling->equSize;
    memcpy(appPlc.plcPIB.pData, (uint8_t *)appPLCCoupling->equVlow,
            appPlc.plcPIB.length);
    DRV_G3_MACRT_PIBSet(appPlc.drvPl360Handle, &appPlc.plcPIB);
}

static void APP_PLC_SetInitialConfiguration ( void )
{
    /* Apply PLC coupling configuration */
    APP_PLC_SetCouplingConfiguration(appPlc.couplingBranch);

    /* Force Transmission to VLO mode by default in order to maximize signal level in any case */
    /* Disable auto-detect mode */
    appPlc.plcPIB.pib = MAC_RT_PIB_MANUF_PHY_PARAM;
    appPlc.plcPIB.index = PHY_PIB_CFG_AUTODETECT_IMPEDANCE;
    appPlc.plcPIB.length = 1;
    appPlc.plcPIB.pData[0] = 0;
    DRV_G3_MACRT_PIBSet(appPlc.drvPl360Handle, &appPlc.plcPIB);

    /* Set VLO mode */
    appPlc.plcPIB.index = PHY_PIB_CFG_IMPEDANCE;
    appPlc.plcPIB.length = 1;
    appPlc.plcPIB.pData[0] = 2;
    DRV_G3_MACRT_PIBSet(appPlc.drvPl360Handle, &appPlc.plcPIB);

    /* Get PLC PHY version */
    appPlc.plcPIB.index = PHY_PIB_VERSION_NUM;
    appPlc.plcPIB.length = 4;
    memcpy(appPlc.plcPIB.pData, (uint8_t *)&appPlc.phyVersion, 4);
    DRV_G3_MACRT_PIBGet(appPlc.drvPl360Handle, &appPlc.plcPIB);
    
    /* Fill MAC RT Header */
    appPlcTx.txHeader.frameControl.frameType = MAC_RT_FRAME_TYPE_DATA;
    appPlcTx.txHeader.frameControl.securityEnabled = 0;
    appPlcTx.txHeader.frameControl.framePending = 0;
    appPlcTx.txHeader.frameControl.ackRequest = 0;
    appPlcTx.txHeader.frameControl.panIdCompression = 0;
    appPlcTx.txHeader.frameControl.destAddressingMode = MAC_RT_SHORT_ADDRESS;
    appPlcTx.txHeader.frameControl.frameVersion = 1;
    appPlcTx.txHeader.frameControl.srcAddressingMode = MAC_RT_SHORT_ADDRESS;
    appPlcTx.txHeader.sequenceNumber = 0;
    appPlcTx.txHeader.destinationPAN = CONF_PAN_ID;
    appPlcTx.txHeader.destinationAddress.addressMode = MAC_RT_SHORT_ADDRESS;
    appPlcTx.txHeader.destinationAddress.shortAddress = MAC_RT_SHORT_ADDRESS_BROADCAST;
    appPlcTx.txHeader.sourcePAN = CONF_PAN_ID;
    appPlcTx.txHeader.sourceAddress.addressMode = MAC_RT_SHORT_ADDRESS;
    appPlcTx.txHeader.sourceAddress.shortAddress = (uint16_t)TRNG_ReadData();
}

static uint8_t APP_PLC_BuildMacRTHeader ( uint8_t *pFrame, MAC_RT_HEADER *pHeader )
{
    uint8_t *pData;
    MAC_RT_FRAME_CONTROL *frameControl;
    
    pData = pFrame;
    frameControl = (MAC_RT_FRAME_CONTROL *)pHeader;
    
    /* Copy Frame Control and Sequence number */
    memcpy(pData, pHeader, 3);
    pData += 3;
    
    if (frameControl->destAddressingMode != MAC_RT_NO_ADDRESS)
    {
        *pData++ = (uint8_t)(pHeader->destinationPAN);
        *pData++ = (uint8_t)(pHeader->destinationPAN >> 8);
        if (frameControl->destAddressingMode == MAC_RT_SHORT_ADDRESS)
        {
            /* MAC_RT_SHORT_ADDRESS */
            *pData++ = (uint8_t)(pHeader->destinationAddress.shortAddress);
            *pData++ = (uint8_t)(pHeader->destinationAddress.shortAddress >> 8);
        }
        else
        {
            /* MAC_RT_EXTENDED_ADDRESS */
            memcpy(pData, pHeader->destinationAddress.extendedAddress.address, 8);
            pData += 8;
        }
    }
    
    if (frameControl->srcAddressingMode != MAC_RT_NO_ADDRESS)
    {
        *pData++ = (uint8_t)(pHeader->sourcePAN);
        *pData++ = (uint8_t)(pHeader->sourcePAN >> 8);
        if (frameControl->srcAddressingMode == MAC_RT_SHORT_ADDRESS)
        {
            /* MAC_RT_SHORT_ADDRESS */
            *pData++ = (uint8_t)(pHeader->sourceAddress.shortAddress);
            *pData++ = (uint8_t)(pHeader->sourceAddress.shortAddress >> 8);
        }
        else
        {
            /* MAC_RT_EXTENDED_ADDRESS */
            memcpy(pData, pHeader->sourceAddress.extendedAddress.address, 8);
            pData += 8;
        }
    }
    
    /* Return Header length */
    return (uint8_t)(pFrame - pData);
}

static uint8_t APP_PLC_GetMacRTHeaderInfo ( uint8_t *pFrame )
{
    uint8_t *pData;
    MAC_RT_FRAME_CONTROL *frameControl;
    uint16_t address;
    
    /* Frame Struct :
     * Frame Control(2b) + Seq Num(1b) + Dest PAN ID(2b) + Dest Addr(2b) + 
     * Src PAN ID(2b) + Src Addr(2b) */
    
    pData = pFrame;
    frameControl = (MAC_RT_FRAME_CONTROL *)pData;
    pData += 2;
    
    pData++; /* Sequence number */
    
    if (frameControl->destAddressingMode != MAC_RT_NO_ADDRESS)
    {
        pData += 2; /* PAN ID */
        if (frameControl->destAddressingMode == MAC_RT_SHORT_ADDRESS)
        {
            /* SHORT ADDRESS */
            address = *pData++;
            address += (uint16_t)*pData++ << 8;
            APP_CONSOLE_Print("Destination Address: 0x%04X ", address);
        }
        else
        {
            pData += 8; /* EXTENDED ADDRESS */
        }
    }
    
    if (frameControl->srcAddressingMode != MAC_RT_NO_ADDRESS)
    {
        pData += 2; /* PAN ID */
        if (frameControl->destAddressingMode == MAC_RT_SHORT_ADDRESS)
        {
            /* SHORT ADDRESS */
            address = *pData++;
            address += (uint16_t)*pData++ << 8;
            APP_CONSOLE_Print(" - Source Address: 0x%04X\r\n", address);
        }
        else
        {
            pData += 8; /* EXTENDED ADDRESS */
        }
    }

    /* Return Header length */
    return (uint8_t)(pFrame - pData);
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************
void Timer1_Callback (uintptr_t context)
{
    appPlc.tmr1Expired = true;
}

void Timer2_Callback (uintptr_t context)
{
    appPlc.tmr2Expired = true;
}

static void APP_PLC_PVDDMonitorCb( SRV_PVDDMON_CMP_MODE cmpMode, uintptr_t context )
{
    (void)context;
    
    if (cmpMode == SRV_PVDDMON_CMP_MODE_OUT)
    {
        /* PLC Transmission is not permitted */
        DRV_G3_MACRT_Enable_TX(appPlc.drvPl360Handle, false);
        appPlc.pvddMonTxEnable = false;
        /* Restart PVDD Monitor to check when VDD is within the comparison window */
        SRV_PVDDMON_Restart(SRV_PVDDMON_CMP_MODE_IN);
    }
    else
    {
        /* PLC Transmission is permitted again */
        DRV_G3_MACRT_Enable_TX(appPlc.drvPl360Handle, true);
        appPlc.pvddMonTxEnable = true;
        /* Restart PVDD Monitor to check when VDD is out of the comparison window */
        SRV_PVDDMON_Restart(SRV_PVDDMON_CMP_MODE_OUT);
    }
}

static void APP_PLC_SleepModeDisableCb( uintptr_t context )
{
    /* Avoid warning */
    (void)context;

    /* Apply PLC initial configuration */
    APP_PLC_SetInitialConfiguration();

    /* Set PLC state */
    appPlc.state = APP_PLC_STATE_WAITING;
}

static void APP_PLC_ExceptionCb(DRV_G3_MACRT_EXCEPTION exceptionObj, uintptr_t context )
{
    /* Avoid warning */
    (void)context;
    (void)exceptionObj;

    /* Update PLC TX Status */
    appPlc.plcTxState = APP_PLC_TX_STATE_IDLE;
    /* Restart PLC task */
    appPlc.state = APP_PLC_STATE_IDLE;
}

static void APP_PLC_DataCfmCb(MAC_RT_TX_CFM_OBJ *cfmObj, uintptr_t context )
{
    /* Avoid warning */
    (void)context;

    /* Update PLC TX Status */
    appPlc.plcTxState = APP_PLC_TX_STATE_IDLE;
    
    /* Capture TX result of the last transmission */
    appPlcTx.lastTxStatus = cfmObj->status;
}

static void APP_PLC_DataIndCb( uint8_t *pData, uint16_t length )
{
    uint8_t *pFrame;
    uint8_t headerLength;
    
    /* Init Timer to handle PLC Reception led */
    USER_PLC_IND_LED_On();
    appPlc.tmr2Handle = SYS_TIME_CallbackRegisterMS(Timer2_Callback, 0, LED_PLC_RX_MSG_RATE_MS, SYS_TIME_SINGLE);

    APP_CONSOLE_Print("\rRx (");
    /* Show Modulation of received frame */
    if (appPlcTx.rxParams.modScheme == MAC_RT_MOD_SCHEME_DIFFERENTIAL)
    {
        if (appPlcTx.rxParams.modType == MAC_RT_MOD_ROBUST)
        {
            APP_CONSOLE_Print("BPSK Robust Differential, ");
        }
        else if (appPlcTx.rxParams.modType == MAC_RT_MOD_BPSK)
        {
            APP_CONSOLE_Print("BPSK Differential, ");
        }
        else if (appPlcTx.rxParams.modType == MAC_RT_MOD_QPSK)
        {
            APP_CONSOLE_Print("QPSK Differential, ");
        }
        else if (appPlcTx.rxParams.modType == MAC_RT_MOD_8PSK)
        {
            APP_CONSOLE_Print("8PSK Differential, ");
        }
    }
    else
    {   /* MOD_SCHEME_COHERENT */
        if (appPlcTx.rxParams.modType == MAC_RT_MOD_ROBUST)
        {
            APP_CONSOLE_Print("BPSK Robust Coherent, ");
        }
        else if (appPlcTx.rxParams.modType == MAC_RT_MOD_BPSK)
        {
            APP_CONSOLE_Print("BPSK Coherent, ");
        }
        else if (appPlcTx.rxParams.modType == MAC_RT_MOD_QPSK)
        {
            APP_CONSOLE_Print("QPSK Coherent, ");
        }
        else if (appPlcTx.rxParams.modType == MAC_RT_MOD_8PSK)
        {
            APP_CONSOLE_Print("8PSK Coherent, ");
        }
    }
    /* Show LQI (Link Quality Indicator). It is in quarters of dB and 10-dB 
     * offset: SNR(dB) = (LQI - 40) / 4 */
    APP_CONSOLE_Print("LQI %ddB): ", div_round((int16_t)appPlcTx.rxParams.lqi - 40, 4));
    
    /* Extract MAC RT Header */
    headerLength = APP_PLC_GetMacRTHeaderInfo(pFrame);
    pFrame += headerLength;
    
    /* Show Payload */
    APP_CONSOLE_Print("%.*s", length - headerLength, pFrame);
    
    APP_CONSOLE_Print(MENU_CMD_PROMPT);
}

static void APP_PLC_RxParamsIndCb( MAC_RT_RX_PARAMETERS_OBJ *pParameters )
{
    memcpy((uint8_t *)appPlcTx.rxParams, (uint8_t *)pParameters, sizeof(MAC_RT_RX_PARAMETERS_OBJ));
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
    /* Init PLC data buffers */
    appPlcTx.pTxFrame = appPlcPibTxFrameBuffer;
    appPlcTx.pRxFrame = appPlcPibRxFrameBuffer;
    
    /* Set PLC state */
    appPlc.state = APP_PLC_STATE_IDLE;
    
    /* Set PVDD Monitor tracking data */
    SRV_PVDDMON_Initialize();
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
            /* Set Static Notching capability (example only valid for FCC band */
            /* Caution: Example provided only for FCC band */
            appPlc.staticNotchingEnable = APP_PLC_STATIC_NOTCHING_ENABLE;

            /* Set PLC Multi-band / Coupling Branch flag */
            if (SRV_PCOUP_Get_Config(SRV_PLC_PCOUP_AUXILIARY_BRANCH) == SYS_STATUS_UNINITIALIZED) {
                /* Auxiliary branch is not configured. Single branch */
                appPlc.plcMultiband = false;
                appPlc.bin2InUse = false;
            } else {
                /* Dual branch */
                appPlc.plcMultiband = true;
                appPlc.couplingBranch = SRV_PCOUP_Get_Default_Branch();
                if (appPlc.couplingBranch == SRV_PLC_PCOUP_MAIN_BRANCH)
                {
                    appPlc.bin2InUse = false;
                }
                else
                {
                    appPlc.bin2InUse = true;
                }
            }

            /* Initialize PLC driver */
            appPlc.state = APP_PLC_STATE_INIT;
        }
        break;

        case APP_PLC_STATE_INIT:
        {
            SYS_STATUS drvG3MacRtStatus = DRV_G3_MACRT_Status(DRV_G3_MACRT_INDEX);
            
            /* Select PLC Binary file for multi-band solution */
            if (appPlc.plcMultiband && (drvG3MacRtStatus == SYS_STATUS_UNINITIALIZED))
            {
                if (appPlc.bin2InUse)
                {
                    drvG3MacRtInitData.binStartAddress = (uint32_t)&g3_mac_rt_bin2_start;
                    drvG3MacRtInitData.binEndAddress = (uint32_t)&g3_mac_rt_bin2_end;
                    /* Set Coupling Auxiliary branch */
                    appPlc.couplingBranch = SRV_PLC_PCOUP_AUXILIARY_BRANCH;
                }
                else
                {
                    drvG3MacRtInitData.binStartAddress = (uint32_t)&g3_mac_rt_bin_start;
                    drvG3MacRtInitData.binEndAddress = (uint32_t)&g3_mac_rt_bin_end;
                }

                /* Initialize PLC Driver Instance */
                sysObj.drvG3MacRt = DRV_G3_MACRT_Initialize(DRV_G3_MACRT_INDEX, (SYS_MODULE_INIT *)&drvG3MacRtInitData);
                /* Register Callback function to handle PLC interruption */
                PIO_PinInterruptCallbackRegister(DRV_PLC_EXT_INT_PIN, DRV_G3_MACRT_ExternalInterruptHandler, sysObj.drvG3MacRt);
            }
            
            /* Open PLC driver */
            appPlc.drvPl360Handle = DRV_G3_MACRT_Open(DRV_G3_MACRT_INDEX_0, NULL);

            if (appPlc.drvPl360Handle != DRV_HANDLE_INVALID)
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
            if (DRV_G3_MACRT_Status(DRV_G3_MACRT_INDEX_0) == SYS_STATUS_READY)
            {
                /* Configure PLC callbacks */
                DRV_G3_MACRT_ExceptionCallbackRegister(appPlc.drvPl360Handle, APP_PLC_ExceptionCb);
                DRV_G3_MACRT_TxCfmCallbackRegister(appPlc.drvPl360Handle, APP_PLC_DataCfmCb);
                DRV_G3_MACRT_DataIndCallbackRegister(appPlc.drvPl360Handle, APP_PLC_DataIndCb);
                DRV_G3_MACRT_RxParamsIndCallbackRegister(appPlc.drvPl360Handle, APP_PLC_RxParamsIndCb);
                
                DRV_G3_MACRT_SleepIndCallbackRegister(appPlc.drvPl360Handle, APP_PLC_SleepModeDisableCb);
                
                /* Apply PLC initial configuration */
                APP_PLC_SetInitialConfiguration();
                
                /* Enable PLC PVDD Monitor Service: ADC channel 0 */
                SRV_PVDDMON_RegisterCallback(APP_PLC_PVDDMonitorCb, 0);
                SRV_PVDDMON_Start(SRV_PVDDMON_CMP_MODE_OUT);
            
                /* Init Timer to handle blinking led */
                appPlc.tmr1Handle = SYS_TIME_CallbackRegisterMS(Timer1_Callback, 0, LED_BLINK_RATE_MS, SYS_TIME_PERIODIC);
                
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
            DRV_G3_MACRT_Close(appPlc.drvPl360Handle);

            /* Restart PLC Driver */
            appPlc.state = APP_PLC_STATE_INIT;
            appPlc.plcTxState = APP_PLC_TX_STATE_IDLE;
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

/*******************************************************************************
  Function:
    bool APP_PLC_Initialize(void)

  Remarks:
    See prototype in app_plc.h.
 */
bool APP_PLC_SendData ( uint8_t* pData, uint16_t length )
{
    if (appPlc.state == APP_PLC_STATE_WAITING)
    {
        if (appPlc.pvddMonTxEnable)
        {
            uint8_t *pFrame;
            uint8_t headerLen;
            
            if (length > MAC_RT_MAX_PAYLOAD_SIZE) {
                return false;
            }
            
            appPlc.plcTxState = APP_PLC_TX_STATE_WAIT_TX_CFM;
            
            pFrame = appPlcTx.pTxFrame;
            
            /* Build MAC RT Frame */
            headerLen = APP_PLC_BuildMacRTHeader(pFrame, &appPlcTx.txHeader);
            pFrame += headerLen;
            
            /* Fill Payload */
            memcpy(pFrame, pData, length);
            pFrame += length;

            /* Send MAC RT Frame */
            DRV_G3_MACRT_TxRequest(appPlc.drvPl360Handle, appPlcTx.pTxFrame, 
                    pFrame - appPlcTx.pTxFrame);

            /* Set PLC state */
            if (appPlc.plcTxState == APP_PLC_TX_STATE_WAIT_TX_CFM)
            {
                appPlc.state = APP_PLC_STATE_WAITING_TX_CFM;
                return true;
            }
        }
    }
    
    return false;
}

bool APP_PLC_SetSleepMode ( bool enable )
{
    bool sleepIsEnabled = (appPlc.state == APP_PLC_STATE_SLEEP);
    
    if (sleepIsEnabled != enable)
    {
        DRV_G3_MACRT_Sleep(appPlc.drvPl360Handle, enable);
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
