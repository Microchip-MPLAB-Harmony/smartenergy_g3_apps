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
#include "stack/g3/pal/plc/pal_plc.h"
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
    This structure should be initialized by the APP_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_PLC_DATA appPlcData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

static void _plcDataIndication(uint8_t *pData, uint16_t length)
{
    
}

static void _plcCommStatusIndication(uint8_t *pData)
{
    
}

static void _plcTxConfirm(MAC_RT_STATUS status, bool updateTimestamp)
{
    
}

static void _plcRxParamsIndication(MAC_RT_RX_PARAMETERS_OBJ *pParameters)
{
    
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
    void APP_PLC_Initialize ( void )

  Remarks:
    See prototype in app_plc.h.
 */

void APP_PLC_Initialize ( void )
{
    PAL_PLC_INIT palPlcInitData;
    
    appPlcData.performPalReset = false;
    
    palPlcInitData.macRtBand = G3_FCC;
    palPlcInitData.macRtHandlers.palPlcDataIndication = _plcDataIndication;
    palPlcInitData.macRtHandlers.palPlcCommStatusIndication = _plcCommStatusIndication;
    palPlcInitData.macRtHandlers.palPlcTxConfirm = _plcTxConfirm;
    palPlcInitData.macRtHandlers.palPlcRxParamsIndication = _plcRxParamsIndication;
    
    /* Place the App state machine in its initial state. */
    appPlcData.state = APP_PLC_STATE_WAITING_INIT;

    /* Initialize the PAL PLC module */
    appPlcData.palPlcObj = PAL_PLC_Initialize( PAL_PLC_PHY_INDEX, (const SYS_MODULE_INIT *) &palPlcInitData );
    
}


/******************************************************************************
  Function:
    void APP_PLC_Tasks ( void )

  Remarks:
    See prototype in app_plc.h.
 */

void APP_PLC_Tasks ( void )
{
    /* Check the application's current state. */
    switch ( appPlcData.state )
    {
        /* Application's initial state. */
        case APP_PLC_STATE_WAITING_INIT:
        {
            /* Open PAL PLC */
            appPlcData.palHandler = PAL_PLC_Open(PAL_PLC_PHY_INDEX, 0);
            if (appPlcData.palHandler == DRV_HANDLE_INVALID)
            {
                appPlcData.state = APP_PLC_STATE_ERROR;
            }
            else
            {
                appPlcData.state = APP_PLC_STATE_WAITING_READY;
            }
            
            break;
        }
        case APP_PLC_STATE_WAITING_READY:
        {
            if (PAL_PLC_Status(appPlcData.palPlcObj) == SYS_STATUS_READY)
            {
                appPlcData.state = APP_PLC_STATE_GET_PHY_VERSION;
            }
            break;
        }

        case APP_PLC_STATE_GET_PHY_VERSION:
        {
            appPlcData.pibObj.pib = MAC_RT_PIB_MANUF_PHY_PARAM;
            appPlcData.pibObj.index = PHY_PIB_VERSION_NUM;
            appPlcData.pibObj.length = 4;
            
            if (PAL_PLC_GetMacRtPib(appPlcData.palHandler, &appPlcData.pibObj) == PAL_PLC_SUCCESS)
            {
                appPlcData.phyVersion = *(uint32_t *)appPlcData.pibObj.pData;
                appPlcData.state = APP_PLC_STATE_SET_PANID;
            }
            else
            {
                appPlcData.prevState = APP_PLC_STATE_GET_PHY_VERSION;
                appPlcData.state = APP_PLC_STATE_ERROR;
            }
            break;
        }

        case APP_PLC_STATE_SET_PANID:
        {
            appPlcData.pibObj.pib = MAC_RT_PIB_PAN_ID;
            appPlcData.pibObj.index = 0;
            appPlcData.pibObj.length = 2;
            appPlcData.pibObj.pData[0] = 0x12;
            appPlcData.pibObj.pData[1] = 0x34;
            
            if (PAL_PLC_SetMacRtPib(appPlcData.palHandler, &appPlcData.pibObj) == PAL_PLC_SUCCESS)
            {
                appPlcData.phyVersion = *(uint32_t *)appPlcData.pibObj.pData;
                appPlcData.state = APP_PLC_STATE_GET_PANID;
            }
            else
            {
                appPlcData.prevState = APP_PLC_STATE_SET_PANID;
                appPlcData.state = APP_PLC_STATE_ERROR;
            }
            break;
        }

        case APP_PLC_STATE_GET_PANID:
        {
            appPlcData.pibObj.pib = MAC_RT_PIB_PAN_ID;
            appPlcData.pibObj.index = 0;
            appPlcData.pibObj.length = 2;
            
            if (PAL_PLC_GetMacRtPib(appPlcData.palHandler, &appPlcData.pibObj) == PAL_PLC_SUCCESS)
            {
                appPlcData.panId = *(uint16_t *)appPlcData.pibObj.pData;
                appPlcData.state = APP_PLC_STATE_RESET;
            }
            else
            {
                appPlcData.prevState = APP_PLC_STATE_GET_PANID;
                appPlcData.state = APP_PLC_STATE_ERROR;
            }
            break;
        }

        case APP_PLC_STATE_RESET:
        {
            if (appPlcData.performPalReset)
            {
                PAL_PLC_Reset(appPlcData.palHandler, true);
                appPlcData.state = APP_PLC_STATE_WAITING_READY;
            }
            else
            {
                appPlcData.state = APP_PLC_STATE_RUNNING;
            }
            break;
        }

        case APP_PLC_STATE_ERROR:
        {
            if (PAL_PLC_Status(appPlcData.palPlcObj) == SYS_STATUS_READY)
            {
                appPlcData.state = appPlcData.prevState;
            }
            break;
        }
        
        case APP_PLC_STATE_RUNNING:
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
