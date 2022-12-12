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

#include <string.h>
#include "definitions.h"
#include "app_rf.h"
#include "stack/g3/pal/rf/pal_rf.h"

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
    This structure should be initialized by the APP_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_RF_DATA appRFData = {0};

static char appRFMessage[] = "Hello RF world\r\n";

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

static void _rfDataIndication(uint8_t *pData, uint16_t length, PAL_RF_RX_PARAMETERS *pParameters)
{
    
}

static void _rfTxConfirm(PAL_RF_PHY_STATUS status, uint32_t timeIni, uint32_t timeEnd)
{
    appRFData.txStatus = status;
    appRFData.txTimeIniCount = timeIni;
    appRFData.txTimeEndCount = timeEnd;
    
    appRFData.pendingTxCfm = false;
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************





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
    appRFData.state = APP_RF_STATE_INIT;
    
    memset(appRFData.pibObj.pData, 0, sizeof(appRFData.pibObj.pData));
    
    appRFData.pendingTxCfm = false;
}


/******************************************************************************
  Function:
    void APP_RF_Tasks ( void )

  Remarks:
    See prototype in app_rf.h.
 */

void APP_RF_Tasks ( void )
{
    /* Check the application's current state. */
    switch (appRFData.state)
    {
        /* Application's initial state. */
        case APP_RF_STATE_INIT:
        {
            /* Wait Console initialization */
            if (SYS_CONSOLE_Status(SYS_CONSOLE_INDEX_0) == SYS_STATUS_READY)
            {
                PAL_RF_INIT palRfInitData;

                appRFData.consoleHandler = SYS_CONSOLE_HandleGet(SYS_CONSOLE_INDEX_0);
                SYS_CONSOLE_Message(appRFData.consoleHandler, "PAL RF initializing...\r\n");
                    
                palRfInitData.rfPhyHandlers.palRfDataIndication = _rfDataIndication;
                palRfInitData.rfPhyHandlers.palRfTxConfirm = _rfTxConfirm;
                /* Initialize the PAL RF module */
                appRFData.palRfObj = PAL_RF_Initialize(PAL_RF_PHY_INDEX, (const SYS_MODULE_INIT *) &palRfInitData);
                
                appRFData.state = APP_RF_STATE_WAITING_READY;
            }
            break;
        }
        
        case APP_RF_STATE_WAITING_READY:
        {
            if (PAL_RF_Status(appRFData.palRfObj) == PAL_RF_STATUS_READY)
            {
                appRFData.palHandler = PAL_RF_HandleGet(PAL_RF_PHY_INDEX);
                if (appRFData.palHandler == PAL_RF_HANDLE_INVALID)
                {
                    appRFData.state = APP_RF_STATE_ERROR;
                }
                else
                {
                    appRFData.state = APP_RF_STATE_GET_PHY_VERSION;
                }
            }
            break;
        }

        case APP_RF_STATE_GET_PHY_VERSION:
        {
            appRFData.pibObj.pib = RF215_PIB_FW_VERSION;
            
            if (PAL_RF_GetRfPhyPib(appRFData.palHandler, &appRFData.pibObj) == PAL_RF_PIB_SUCCESS)
            {
                uint8_t size = PAL_RF_GetRfPhyPibLength(appRFData.palHandler, RF215_PIB_FW_VERSION);
                memcpy(&appRFData.fwVersion, appRFData.pibObj.pData, size);
                appRFData.state = APP_RF_STATE_GET_PHY_CONFIG;
            }
            else
            {
                appRFData.prevState = APP_RF_STATE_GET_PHY_VERSION;
                appRFData.state = APP_RF_STATE_ERROR;
            }
            break;
        }

        case APP_RF_STATE_GET_PHY_CONFIG:
        {
            appRFData.pibObj.pib = RF215_PIB_PHY_CONFIG;
            
            if (PAL_RF_GetRfPhyPib(appRFData.palHandler, &appRFData.pibObj) == PAL_RF_PIB_SUCCESS)
            {
                memcpy(&appRFData.rfPhyConfig, appRFData.pibObj.pData, sizeof(appRFData.rfPhyConfig));
                appRFData.state = APP_RF_STATE_RESET;
            }
            else
            {
                appRFData.prevState = APP_RF_STATE_GET_PHY_VERSION;
                appRFData.state = APP_RF_STATE_ERROR;
            }
            break;
        }

        case APP_RF_STATE_RESET:
        {
            if (appRFData.performRfPhyReset)
            {
                PAL_RF_Reset(appRFData.palHandler);
                appRFData.state = APP_RF_STATE_WAITING_READY;
            }
            else
            {
                appRFData.state = APP_RF_STATE_DEINIT;
            }
            break;
        }

        case APP_RF_STATE_DEINIT:
        {
            if (appRFData.performRfPhyDeinit)
            {
                PAL_RF_Deinitialize(PAL_RF_PHY_INDEX);
                appRFData.state = APP_RF_STATE_INIT;
            }
            else
            {
                appRFData.state = APP_RF_STATE_SEND_MESSAGE;
            }
            break;
        }
        
        case APP_RF_STATE_SEND_MESSAGE:
        {
            PAL_RF_TX_PARAMETERS txParameters;

            SYS_CONSOLE_Message(appRFData.consoleHandler, "PAL RF sending message...\r\n");

            txParameters.csmaEnable = false;
            txParameters.txPowerAttenuation = 0;
            txParameters.timeCount = SYS_TIME_Counter64Get() + SYS_TIME_MSToCount(1000);

            appRFData.pendingTxCfm = true;

            PAL_RF_TxRequest(appRFData.palHandler, (uint8_t *)appRFMessage, sizeof(appRFMessage), &txParameters);

            if (appRFData.pendingTxCfm)
            {
                appRFData.state = APP_RF_STATE_WAITING_TXCFM;
            }
            break;
        }
        
        case APP_RF_STATE_WAITING_TXCFM:
        {
            if (!appRFData.pendingTxCfm)
            {
                SYS_CONSOLE_Print(appRFData.consoleHandler, "PAL RF -> _rfTxConfirm(): timeIniCount = %u - timeEndCount = %u - ",
                        appRFData.txTimeIniCount, appRFData.txTimeEndCount);

                switch(appRFData.txStatus)
                {
                    case PAL_RF_PHY_SUCCESS:
                    {
                        SYS_CONSOLE_Message(appRFData.consoleHandler, "PAL_RF_PHY_SUCCESS\r\n");
                        break;
                    }

                    case PAL_RF_PHY_CHANNEL_ACCESS_FAILURE:
                    {
                        SYS_CONSOLE_Message(appRFData.consoleHandler, "PAL_RF_PHY_CHANNEL_ACCESS_FAILURE\r\n");
                        break;
                    }

                    case PAL_RF_PHY_BUSY_TX:
                    {
                        SYS_CONSOLE_Message(appRFData.consoleHandler, "PAL_RF_PHY_BUSY_TX\r\n");
                        break;
                    }

                    case PAL_RF_PHY_TIMEOUT:
                    {
                        SYS_CONSOLE_Message(appRFData.consoleHandler, "PAL_RF_PHY_TIMEOUT\r\n");
                        break;
                    }

                    case PAL_RF_PHY_INVALID_PARAM:
                    {
                        SYS_CONSOLE_Message(appRFData.consoleHandler, "PAL_RF_PHY_INVALID_PARAM\r\n");
                        break;
                    }

                    case PAL_RF_PHY_TX_CANCELLED:
                    {
                        SYS_CONSOLE_Message(appRFData.consoleHandler, "PAL_RF_PHY_TX_CANCELLED\r\n");
                        break;
                    }

                    case PAL_RF_PHY_TX_ERROR:
                    {
                        SYS_CONSOLE_Message(appRFData.consoleHandler, "PAL_RF_PHY_TX_ERROR\r\n");
                        break;
                    }

                    case PAL_RF_PHY_TRX_OFF:
                    {
                        SYS_CONSOLE_Message(appRFData.consoleHandler, "PAL_RF_PHY_TRX_OFF\r\n");
                        break;
                    }

                    case PAL_RF_PHY_ERROR:
                    {
                        SYS_CONSOLE_Message(appRFData.consoleHandler, "PAL_RF_PHY_ERROR\r\n");
                        break;
                    }
                }
            
            appRFData.state = APP_RF_STATE_SEND_MESSAGE;
            
            }
            break;
        }

        case APP_RF_STATE_ERROR:
        {
            if (PAL_PLC_Status(appRFData.palRfObj) == PAL_PLC_STATUS_READY)
            {
                appRFData.state = appRFData.prevState;
            }
            break;
        }
        
        case APP_RF_STATE_RUNNING:
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
