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

#include "app_plc.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_PLC_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_PLC_DATA app_plcData;

static uint8_t plcDataTxBuffer[APP_PLC_DATA_BUFFER_SIZE];
static uint8_t plcDataPIBBuffer[APP_PLC_PIB_BUFFER_SIZE];
static uint8_t serialDataBuffer[APP_PLC_SERIAL_DATA_BUFFER_SIZE];

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

static void _APP_PLC_TimeExpired(uintptr_t context)
{
    *((bool *) context) = true;
}

static void _APP_PLC_ExceptionCb(DRV_PLC_PHY_EXCEPTION exceptionObj,
        uintptr_t context)
{
    /* Avoid warning */
    (void) context;

    switch (exceptionObj)
    {
        case DRV_PLC_PHY_EXCEPTION_UNEXPECTED_KEY:
            app_plcData.plc_phy_err_unexpected++;
            break;

        case DRV_PLC_PHY_EXCEPTION_CRITICAL_ERROR:
            app_plcData.plc_phy_err_critical++;
            break;

        case DRV_PLC_PHY_EXCEPTION_RESET:
            app_plcData.plc_phy_err_reset++;
            break;

        default:
            app_plcData.plc_phy_err_unknow++;
	}

	app_plcData.plc_phy_exception = true;
}

static void _APP_PLC_DataIndCb(DRV_PLC_PHY_RECEPTION_OBJ *indObj, uintptr_t context)
{
    size_t length;
    
    /* Avoid warning */
    (void) context;

    /* Turn on indication LED and start timer to turn it off */
    SYS_TIME_TimerDestroy(app_plcData.tmr2Handle);
    USER_PLC_IND_LED_On();
    app_plcData.tmr2Handle = SYS_TIME_CallbackRegisterMS(_APP_PLC_TimeExpired,
            (uintptr_t) &app_plcData.tmr2Expired, APP_PLC_LED_BLINK_PLC_MSG_MS, SYS_TIME_SINGLE);

    /* Serialize received message and send through USI */
    length = SRV_PSERIAL_SerialRxMessage(serialDataBuffer, indObj);
    SRV_USI_Send_Message(app_plcData.srvUSIHandle, SRV_USI_PROT_ID_PHY,
            serialDataBuffer, length);
}

static void _APP_PLC_DataCfmCb(DRV_PLC_PHY_TRANSMISSION_CFM_OBJ *cfmObj, uintptr_t context)
{
    size_t length;

    /* Avoid warning */
    (void) context;

    /* Serialize confirm and send through USI */
    length = SRV_PSERIAL_SerialCfmMessage(serialDataBuffer, cfmObj);
    SRV_USI_Send_Message(app_plcData.srvUSIHandle, SRV_USI_PROT_ID_PHY,
            serialDataBuffer, length);
}

static void _APP_PLC_PVDDMonitorCb( SRV_PVDDMON_CMP_MODE cmpMode, uintptr_t context )
{
    /* Avoid warning */
    (void) context;

    if (cmpMode == SRV_PVDDMON_CMP_MODE_OUT)
    {
        /* PLC Transmission is not permitted */
        DRV_PLC_PHY_EnableTX(app_plcData.drvPlcHandle, false);
        app_plcData.pvddMonTxEnable = false;
        /* Restart PVDD Monitor to check when VDD is within the comparison window */
        SRV_PVDDMON_Restart(SRV_PVDDMON_CMP_MODE_IN);
    }
    else
    {
        /* PLC Transmission is permitted again */
        DRV_PLC_PHY_EnableTX(app_plcData.drvPlcHandle, true);
        app_plcData.pvddMonTxEnable = true;
        /* Restart PVDD Monitor to check when VDD is out of the comparison window */
        SRV_PVDDMON_Restart(SRV_PVDDMON_CMP_MODE_OUT);
    }
}

void _APP_PLC_UsiPhyProtocolEventCb(uint8_t *pData, size_t length)
{
    SRV_PSERIAL_COMMAND command;

    /* Protection for invalid us_length */
    if (!length)
    {
        return;
    }

    /* Process received message from PLC Tool */
    command = SRV_PSERIAL_GetCommand(pData);

    switch (command) {
        case SRV_PSERIAL_CMD_PHY_GET_CFG:
        {
            /* Extract PIB information */
            SRV_PSERIAL_ParseGetPIB(&app_plcData.plcPIB, pData);

            /* Get PIB from PLC driver */
            if (DRV_PLC_PHY_PIBGet(app_plcData.drvPlcHandle, &app_plcData.plcPIB))
            {
                size_t len;

                /* Serialize PIB get response and send through USI */
                len = SRV_PSERIAL_SerialGetPIB(serialDataBuffer, &app_plcData.plcPIB);
                SRV_USI_Send_Message(app_plcData.srvUSIHandle, SRV_USI_PROT_ID_PHY,
                        serialDataBuffer, len);
            }
        }
        break;

        case SRV_PSERIAL_CMD_PHY_SET_CFG:
        {
            /* Extract PIB information */
            SRV_PSERIAL_ParseSetPIB(&app_plcData.plcPIB, pData);

            /* Set PIB in PLC driver */
            if (DRV_PLC_PHY_PIBSet(app_plcData.drvPlcHandle, &app_plcData.plcPIB))
            {
                size_t len;

                /* Serialize PIB set response and send through USI */
                len = SRV_PSERIAL_SerialSetPIB(serialDataBuffer, &app_plcData.plcPIB);
                SRV_USI_Send_Message(app_plcData.srvUSIHandle, SRV_USI_PROT_ID_PHY,
                        serialDataBuffer, len);
            }
        }
        break;

        case SRV_PSERIAL_CMD_PHY_SEND_MSG:
        {
            if (app_plcData.pvddMonTxEnable == true)
            {
                /* Parse data from USI */
                SRV_PSERIAL_ParseTxMessage(&app_plcData.plcTxObj, pData);

                /* Send Message through PLC */
                DRV_PLC_PHY_TxRequest(app_plcData.drvPlcHandle, &app_plcData.plcTxObj);
            }
            else
            {
                DRV_PLC_PHY_TRANSMISSION_CFM_OBJ cfmData;

                cfmData.time = 0;
                cfmData.rmsCalc = 0;
                cfmData.result = DRV_PLC_PHY_TX_RESULT_NO_TX;
                _APP_PLC_DataCfmCb(&cfmData, 0);
            }
        }
        break;

        default:
            break;
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

static void APP_PLC_SetCouplingConfiguration(void)
{
    SRV_PLC_PCOUP_BRANCH plcBranch;

    plcBranch = SRV_PCOUP_Get_Default_Branch();
    SRV_PCOUP_Set_Config(app_plcData.drvPlcHandle, plcBranch);

    /* Disable AUTO mode and set VLO behavior by default in order to
     * maximize signal level in any case */
    app_plcData.plcPIB.id = PLC_ID_CFG_AUTODETECT_IMPEDANCE;
    app_plcData.plcPIB.length = 1;
    *app_plcData.plcPIB.pData = 0;
    DRV_PLC_PHY_PIBSet(app_plcData.drvPlcHandle, &app_plcData.plcPIB);

    app_plcData.plcPIB.id = PLC_ID_CFG_IMPEDANCE;
    app_plcData.plcPIB.length = 1;
    *app_plcData.plcPIB.pData = VLO_STATE;
    DRV_PLC_PHY_PIBSet(app_plcData.drvPlcHandle, &app_plcData.plcPIB);
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_PLC_Initialize ( void )

  Remarks:
    See prototype in app_plc.h.
 */

void APP_PLC_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    app_plcData.state = APP_PLC_STATE_INIT;

    /* Init Timer handler */
    app_plcData.tmr1Handle = SYS_TIME_HANDLE_INVALID;
    app_plcData.tmr2Handle = SYS_TIME_HANDLE_INVALID;
    app_plcData.tmr1Expired = false;
    app_plcData.tmr2Expired = false;

    /* Reset PLC exceptions statistics */
    app_plcData.plc_phy_err_unexpected = 0;
    app_plcData.plc_phy_err_critical = 0;
    app_plcData.plc_phy_err_reset = 0;
    app_plcData.plc_phy_err_unknow = 0;

    /* Initialize PLC buffers */
    app_plcData.plcTxObj.pTransmitData = plcDataTxBuffer;
    app_plcData.plcPIB.pData = plcDataPIBBuffer;

    /* Set PVDD Monitor tracking data */
    app_plcData.pvddMonTxEnable = true;
}


/******************************************************************************
  Function:
    void APP_PLC_Tasks ( void )

  Remarks:
    See prototype in app_plc.h.
 */

void APP_PLC_Tasks ( void )
{
    CLEAR_WATCHDOG();

    /* Signaling: LED Toggle */
    if (app_plcData.tmr1Expired)
    {
        app_plcData.tmr1Expired = false;
        USER_PLC_BLINK_LED_Toggle();
    }

    /* Signaling: PLC RX */
    if (app_plcData.tmr2Expired)
    {
        app_plcData.tmr2Expired = false;
        USER_PLC_IND_LED_Off();
    }

    /* Check the application's current state. */
    switch(app_plcData.state)
    {
        /* Application's initial state. */
        case APP_PLC_STATE_INIT:
        {
            /* Open PLC driver: Start uploading process */
            app_plcData.drvPlcHandle = DRV_PLC_PHY_Open(DRV_PLC_PHY_INDEX, NULL);

            if (app_plcData.drvPlcHandle != DRV_HANDLE_INVALID)
            {
                /* Set Application to next state */
                app_plcData.state = APP_PLC_STATE_REGISTER;
            }
            else
            {
                /* Set Application to ERROR state */
                app_plcData.state = APP_PLC_STATE_ERROR;
            }
            break;
        }

        /* Waiting PLC driver to be opened and register callback functions */
        case APP_PLC_STATE_REGISTER:
        {
            /* Check PLC driver status */
            SYS_STATUS plcStatus = DRV_PLC_PHY_Status(DRV_PLC_PHY_INDEX);
            if (plcStatus == SYS_STATUS_READY)
            {
                /* Register PLC callback */
                DRV_PLC_PHY_ExceptionCallbackRegister(app_plcData.drvPlcHandle,
                        _APP_PLC_ExceptionCb, DRV_PLC_PHY_INDEX);
                DRV_PLC_PHY_DataIndCallbackRegister(app_plcData.drvPlcHandle,
                        _APP_PLC_DataIndCb, DRV_PLC_PHY_INDEX);
                DRV_PLC_PHY_TxCfmCallbackRegister(app_plcData.drvPlcHandle,
                        _APP_PLC_DataCfmCb, DRV_PLC_PHY_INDEX);

                /* Set PLC coupling configuration */
                APP_PLC_SetCouplingConfiguration();

                /* Enable PLC PVDD Monitor Service */
                DRV_PLC_PHY_EnableTX(app_plcData.drvPlcHandle, true);
                SRV_PVDDMON_CallbackRegister(_APP_PLC_PVDDMonitorCb, 0);
                SRV_PVDDMON_Start(SRV_PVDDMON_CMP_MODE_OUT);

                /* Open USI Service */
                app_plcData.srvUSIHandle = SRV_USI_Open(USER_PLC_USI_INSTANCE_INDEX);

                if (app_plcData.srvUSIHandle != DRV_HANDLE_INVALID)
                {
                    /* Set Application to next state */
                    app_plcData.state = APP_PLC_STATE_CONFIG_USI;
                }
                else
                {
                    /* Set Application to ERROR state */
                    app_plcData.state = APP_PLC_STATE_ERROR;
                }
            }
            else if (plcStatus == SYS_STATUS_ERROR)
            {
                /* Set Application to ERROR state */
                app_plcData.state = APP_PLC_STATE_ERROR;
            }
            break;
        }

        case APP_PLC_STATE_CONFIG_USI:
        {
            SRV_USI_STATUS usiStatus = SRV_USI_Status(app_plcData.srvUSIHandle);
            
            if (usiStatus == SRV_USI_STATUS_CONFIGURED)
            {
                /* Register USI callback */
                SRV_USI_CallbackRegister(app_plcData.srvUSIHandle,
                        SRV_USI_PROT_ID_PHY, _APP_PLC_UsiPhyProtocolEventCb);

                if (app_plcData.tmr1Handle == SYS_TIME_HANDLE_INVALID)
                {
                    /* Register Timer Callback */
                    app_plcData.tmr1Handle = SYS_TIME_CallbackRegisterMS(
                            _APP_PLC_TimeExpired, (uintptr_t) &app_plcData.tmr1Expired,
                            APP_PLC_LED_BLINK_RATE_MS, SYS_TIME_PERIODIC);
                }
                else
                {
                    SYS_TIME_TimerStart(app_plcData.tmr1Handle);
                }

                /* Set Application to next state */
                app_plcData.state = APP_PLC_STATE_READY;
            }
            else if (usiStatus == SRV_USI_STATUS_ERROR)
            {
                /* Set Application to ERROR state */
                app_plcData.state = APP_PLC_STATE_ERROR;
            }
            break;
        }

        case APP_PLC_STATE_READY:
        {
            /* Check USI status in case of USI device has been reset */
            if (SRV_USI_Status(app_plcData.srvUSIHandle) == SRV_USI_STATUS_NOT_CONFIGURED)
            {
                /* Set Application to next state */
                app_plcData.state = APP_PLC_STATE_CONFIG_USI;
                SYS_TIME_TimerStop(app_plcData.tmr1Handle);
                /* Disable Blink Led */
                USER_PLC_BLINK_LED_Off();
            }
            break;
        }

        case APP_PLC_STATE_ERROR:
        {
            /* Handle error in application's state machine */
            break;
        }

        /* The default state should never be executed. */
        default:
        {
            break;
        }
    }
}


/*******************************************************************************
 End of File
 */
