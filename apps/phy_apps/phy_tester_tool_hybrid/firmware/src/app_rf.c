/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_rf.c

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

#include "app_rf.h"

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
    This structure should be initialized by the APP_RF_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_RF_DATA app_rfData;

static uint8_t rfDataPIBBuffer[APP_RF_PIB_BUFFER_SIZE];

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

static void _APP_RF_TimeExpired(uintptr_t context)
{
    *((bool *) context) = true;
}

static void _APP_RF_RxIndCb(DRV_RF215_RX_INDICATION_OBJ* indObj, uintptr_t ctxt)
{
    DRV_RF215_PHY_CFG_OBJ phyCfg;
    DRV_HANDLE rf215Handle;
    uint8_t* pSerialData;
    size_t length;
    DRV_RF215_TRX_ID trxId = (DRV_RF215_TRX_ID) ctxt;

    if (trxId == RF215_TRX_ID_RF09)
    {
        rf215Handle = app_rfData.rf215HandleRF09;
    }
    else
    {
        rf215Handle = app_rfData.rf215HandleRF24;
    }

    /* Get RF PHY configuration */
    DRV_RF215_GetPib(rf215Handle, RF215_PIB_PHY_CONFIG, &phyCfg);

    /* Serialize received message and send through USI */
    pSerialData = SRV_RSERIAL_SerialRxMessage(indObj, trxId, &phyCfg, &length);
    SRV_USI_Send_Message(app_rfData.srvUSIHandle, SRV_USI_PROT_ID_PHY_RF215,
            pSerialData, length);
}

static void _APP_RF_TxCfmCb (
    DRV_RF215_TX_HANDLE txHandle,
    DRV_RF215_TX_CONFIRM_OBJ *cfmObj,
    uintptr_t ctxt
)
{
    uint8_t* pSerialData;
    size_t length;
    DRV_RF215_TRX_ID trxId = (DRV_RF215_TRX_ID) ctxt;

    /* Serialize confirm and send through USI */
    pSerialData = SRV_RSERIAL_SerialCfmMessage(cfmObj, trxId, txHandle, &length);
    SRV_USI_Send_Message(app_rfData.srvUSIHandle, SRV_USI_PROT_ID_PHY_RF215,
            pSerialData, length);
}

void _APP_RF_UsiPhyProtocolEventCb(uint8_t *pData, size_t usiLength)
{
    uint8_t* pSerialData;
    size_t length;
    SRV_RSERIAL_COMMAND command;
    DRV_HANDLE rf215Handle;
    DRV_RF215_TRX_ID trxId;
    DRV_RF215_PIB_ATTRIBUTE pibAttr;
    DRV_RF215_PIB_RESULT pibResult;
    uint8_t pibSize, pibSizePhy;

    /* Protection for invalid length */
    if (usiLength == 0)
    {
        return;
    }

    /* Process received message from PLC Tool */
    command = SRV_RSERIAL_GetCommand(pData);

    switch (command) {
        case SRV_RSERIAL_CMD_PHY_GET_CFG:
        {
            /* Extract PIB information */
            SRV_RSERIAL_ParsePIB(pData, &trxId, &pibAttr, &pibSize);
            pibSizePhy = DRV_RF215_GetPibSize(pibAttr);

            if (pibSize >= pibSizePhy)
            {
                if (trxId == RF215_TRX_ID_RF09)
                {
                    rf215Handle = app_rfData.rf215HandleRF09;
                }
                else
                {
                    rf215Handle = app_rfData.rf215HandleRF24;
                }

                /* Get PIB from RF215 driver */
                pibResult = DRV_RF215_GetPib(rf215Handle, pibAttr, rfDataPIBBuffer);
            }
            else
            {
                /* Invalid length */
                pibResult = RF215_PIB_RESULT_INVALID_PARAM;
            }

            /* Serialize PIB get response and send through USI */
            pSerialData = SRV_RSERIAL_SerialGetPIB(trxId, pibAttr, pibSizePhy,
                    pibResult, rfDataPIBBuffer, &length);
            SRV_USI_Send_Message(app_rfData.srvUSIHandle,
                    SRV_USI_PROT_ID_PHY_RF215, pSerialData, length);
        }
        break;

        case SRV_RSERIAL_CMD_PHY_SET_CFG:
        {
            uint8_t *pPibValue;

            /* Extract PIB information */
            pPibValue = SRV_RSERIAL_ParsePIB(pData, &trxId, &pibAttr, &pibSize);
            pibSizePhy = DRV_RF215_GetPibSize(pibAttr);

            if (pibSize >= pibSizePhy)
            {
                if (trxId == RF215_TRX_ID_RF09)
                {
                    rf215Handle = app_rfData.rf215HandleRF09;
                }
                else
                {
                    rf215Handle = app_rfData.rf215HandleRF24;
                }

                /* Set PIB to RF215 driver */
                pibResult = DRV_RF215_SetPib(rf215Handle, pibAttr, pPibValue);
            }
            else
            {
                /* Invalid length */
                pibResult = RF215_PIB_RESULT_INVALID_PARAM;
            }

            /* Serialize PIB set response and send through USI */
            pSerialData = SRV_RSERIAL_SerialSetPIB(trxId, pibAttr, pibSizePhy,
                    pibResult, &length);
            SRV_USI_Send_Message(app_rfData.srvUSIHandle,
                    SRV_USI_PROT_ID_PHY_RF215, pSerialData, length);
        }
        break;

        case SRV_RSERIAL_CMD_PHY_SEND_MSG:
        {
            bool txCancel;
            DRV_RF215_PHY_CFG_OBJ phyCfg;
            DRV_RF215_TX_REQUEST_OBJ txReq;
            DRV_RF215_TX_HANDLE txHandle;

            /* Parse TRX identifier from USI */
            trxId = SRV_RSERIAL_ParseTxMessageTrxId(pData);

            if (trxId == RF215_TRX_ID_RF09)
            {
                rf215Handle = app_rfData.rf215HandleRF09;
            }
            else
            {
                rf215Handle = app_rfData.rf215HandleRF24;
            }

            /* Get RF PHY configuration */
            DRV_RF215_GetPib(rf215Handle, RF215_PIB_PHY_CONFIG, &phyCfg);

            /* Parse TX request data from USI */
            txCancel = SRV_RSERIAL_ParseTxMessage(pData, &phyCfg, &txReq, &txHandle);

            if (txCancel == false)
            {
                DRV_RF215_TX_RESULT txResult;

                /* Send Message through RF */
                txHandle = DRV_RF215_TxRequest(rf215Handle, &txReq, &txResult);
                SRV_RSERIAL_SetTxHandle(txHandle);

                if (txResult != RF215_TX_SUCCESS)
                {
                    DRV_RF215_TX_CONFIRM_OBJ txCfm;
                    txCfm.txResult = txResult;
                    txCfm.ppduDurationUS = 0;
                    txCfm.timeIni = SYS_TIME_Counter64Get();

                    /* TX request error */
                    _APP_RF_TxCfmCb(DRV_RF215_TX_HANDLE_INVALID, &txCfm,
                            (uintptr_t) trxId);
                }
            }
            else
            {
                /* Cancel TX request */
                DRV_RF215_TxCancel(rf215Handle, txHandle);
            }
        }
        break;

        default:
            break;
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_RF_Initialize ( void )

  Remarks:
    See prototype in app_rf.h.
 */

void APP_RF_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    app_rfData.state = APP_RF_STATE_INIT;

    /* Initialize handles */
    app_rfData.rf215HandleRF09 = DRV_HANDLE_INVALID;
    app_rfData.rf215HandleRF24 = DRV_HANDLE_INVALID;
    app_rfData.srvUSIHandle = DRV_HANDLE_INVALID;
    app_rfData.tmr1Handle = SYS_TIME_HANDLE_INVALID;
}


/******************************************************************************
  Function:
    void APP_RF_Tasks ( void )

  Remarks:
    See prototype in app_rf.h.
 */

void APP_RF_Tasks ( void )
{
    WDT_Clear();

    /* Signaling: LED Toggle */
    if (app_rfData.tmr1Expired)
    {
        app_rfData.tmr1Expired = false;
        USER_RF_BLINK_LED_Toggle();
    }

    /* Check the application's current state. */
    switch ( app_rfData.state )
    {
        /* Application's initial state. */
        case APP_RF_STATE_INIT:
        {
            /* Check status of RF215 driver */
            SYS_STATUS rf215Status = DRV_RF215_Status(sysObj.drvRf215);
            if (rf215Status == SYS_STATUS_READY)
            {
                /* RF215 driver is ready to be opened */
                app_rfData.rf215HandleRF09 = DRV_RF215_Open(DRV_RF215_INDEX_0, RF215_TRX_ID_RF09);
                app_rfData.rf215HandleRF24 = DRV_RF215_Open(DRV_RF215_INDEX_0, RF215_TRX_ID_RF24);

                if (app_rfData.rf215HandleRF09 != DRV_HANDLE_INVALID)
                {
                    /* Register RF215 driver callbacks */
                    DRV_RF215_RxIndCallbackRegister(app_rfData.rf215HandleRF09,
                            _APP_RF_RxIndCb, (uintptr_t) RF215_TRX_ID_RF09);
                    DRV_RF215_TxCfmCallbackRegister(app_rfData.rf215HandleRF09,
                            _APP_RF_TxCfmCb, (uintptr_t) RF215_TRX_ID_RF09);
                }

                if (app_rfData.rf215HandleRF24 != DRV_HANDLE_INVALID)
                {
                    /* Register RF215 driver callbacks */
                    DRV_RF215_RxIndCallbackRegister(app_rfData.rf215HandleRF24,
                            _APP_RF_RxIndCb, (uintptr_t) RF215_TRX_ID_RF24);
                    DRV_RF215_TxCfmCallbackRegister(app_rfData.rf215HandleRF24,
                            _APP_RF_TxCfmCb, (uintptr_t) RF215_TRX_ID_RF24);
                }

                if ((app_rfData.rf215HandleRF09 != DRV_HANDLE_INVALID) ||
                        (app_rfData.rf215HandleRF24 != DRV_HANDLE_INVALID))
                {
                    /* Open USI Service */
                    app_rfData.srvUSIHandle = SRV_USI_Open(SRV_USI_INDEX_1);

                    if (app_rfData.srvUSIHandle != DRV_HANDLE_INVALID)
                    {
                        /* Set Application to next state */
                    app_rfData.state = APP_RF_STATE_CONFIG_USI;
                    }
                    else
                    {
                        /* Set Application to ERROR state */
                        app_rfData.state = APP_RF_STATE_ERROR;
                    }
                }
                else
                {
                    /* Set Application to ERROR state */
                    app_rfData.state = APP_RF_STATE_ERROR;
                }
            }
            else if (rf215Status == SYS_STATUS_ERROR)
            {
                /* Set Application to ERROR state */
                app_rfData.state = APP_RF_STATE_ERROR;
            }

            break;
        }

        case APP_RF_STATE_CONFIG_USI:
        {
            SRV_USI_STATUS usiStatus = SRV_USI_Status(app_rfData.srvUSIHandle);

            if (usiStatus == SRV_USI_STATUS_CONFIGURED)
            {
                /* Register USI callback */
                SRV_USI_CallbackRegister(app_rfData.srvUSIHandle,
                        SRV_USI_PROT_ID_PHY_RF215, _APP_RF_UsiPhyProtocolEventCb);

                if (app_rfData.tmr1Handle == SYS_TIME_HANDLE_INVALID)
                {
                    /* Register Timer Callback */
                    app_rfData.tmr1Handle = SYS_TIME_CallbackRegisterMS(
                            _APP_RF_TimeExpired, (uintptr_t) &app_rfData.tmr1Expired,
                            APP_RF_LED_BLINK_RATE_MS, SYS_TIME_PERIODIC);
                }
                else
                {
                    SYS_TIME_TimerStart(app_rfData.tmr1Handle);
                }

                /* Set Application to next state */
                app_rfData.state = APP_RF_STATE_READY;
            }
            else if (usiStatus == SRV_USI_STATUS_ERROR)
            {
                /* Set Application to ERROR state */
                app_rfData.state = APP_RF_STATE_ERROR;
            }
            break;
        }

        case APP_RF_STATE_READY:
        {
            /* Check USI status in case of USI device has been reset */
            if (SRV_USI_Status(app_rfData.srvUSIHandle) == SRV_USI_STATUS_NOT_CONFIGURED)
            {
                /* Set Application to next state */
                app_rfData.state = APP_RF_STATE_CONFIG_USI;
                SYS_TIME_TimerStop(app_rfData.tmr1Handle);
                /* Disable Blink Led */
                USER_RF_BLINK_LED_Off();
            }
            break;
        }

        case APP_RF_STATE_ERROR:
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
