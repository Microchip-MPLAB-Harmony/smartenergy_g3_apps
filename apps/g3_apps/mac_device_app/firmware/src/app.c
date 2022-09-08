/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app.c

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
#include "app.h"
#include "g3/pal/plc/pal_plc.h"

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

APP_DATA appData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

static void _plcDataIndication(uint8_t *pData, uint16_t length)
{
    
}

static void _plcMacSnifferIndication(uint8_t *pData, uint16_t length)
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
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize ( void )
{
    PAL_PLC_INIT palPlcInitData;
    
    appData.performPalReset = false;
    
    palPlcInitData.macRtBand = G3_FCC;
    palPlcInitData.macRtHandlers.palPlcDataIndication = _plcDataIndication;
    palPlcInitData.macRtHandlers.palPlcMacSnifferIndication = _plcMacSnifferIndication;
    palPlcInitData.macRtHandlers.palPlcCommStatusIndication = _plcCommStatusIndication;
    palPlcInitData.macRtHandlers.palPlcTxConfirm = _plcTxConfirm;
    palPlcInitData.macRtHandlers.palPlcRxParamsIndication = _plcRxParamsIndication;
            
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_WAITING_PAL_INIT;

    /* Initialize the PAL PLC module */
    appData.palObj = PAL_PLC_Initialize( PAL_PLC_PHY_INDEX, (const SYS_MODULE_INIT *) &palPlcInitData );
}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{
    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_WAITING_PAL_INIT:
        {
            /* Open PAL PLC */
            appData.palHandler = PAL_PLC_Open(PAL_PLC_PHY_INDEX, 0);
            if (appData.palHandler == DRV_HANDLE_INVALID)
            {
                appData.state = APP_STATE_ERROR;
            }
            else
            {
                appData.state = APP_STATE_WAITING_PAL_READY;
            }
            
            break;
        }
        case APP_STATE_WAITING_PAL_READY:
        {
            if (PAL_PLC_Status(appData.palObj) == SYS_STATUS_READY)
            {
                appData.state = APP_STATE_GET_PHY_VERSION;
            }
            break;
        }

        case APP_STATE_GET_PHY_VERSION:
        {
            appData.pibObj.pib = MAC_RT_PIB_MANUF_PHY_PARAM;
            appData.pibObj.index = PHY_PIB_VERSION_NUM;
            appData.pibObj.length = 4;
            
            if (PAL_PLC_GetMacRtPib(appData.palHandler, &appData.pibObj) == PAL_PLC_SUCCESS)
            {
                appData.phyVersion = *(uint32_t *)appData.pibObj.pData;
                appData.state = APP_STATE_SET_PANID;
            }
            else
            {
                appData.prevState = APP_STATE_GET_PHY_VERSION;
                appData.state = APP_STATE_ERROR;
            }
            break;
        }

        case APP_STATE_SET_PANID:
        {
            appData.pibObj.pib = MAC_RT_PIB_PAN_ID;
            appData.pibObj.index = 0;
            appData.pibObj.length = 2;
            appData.pibObj.pData[0] = 0x12;
            appData.pibObj.pData[1] = 0x34;
            
            if (PAL_PLC_SetMacRtPib(appData.palHandler, &appData.pibObj) == PAL_PLC_SUCCESS)
            {
                appData.phyVersion = *(uint32_t *)appData.pibObj.pData;
                appData.state = APP_STATE_GET_PANID;
            }
            else
            {
                appData.prevState = APP_STATE_SET_PANID;
                appData.state = APP_STATE_ERROR;
            }
            break;
        }

        case APP_STATE_GET_PANID:
        {
            appData.pibObj.pib = MAC_RT_PIB_PAN_ID;
            appData.pibObj.index = 0;
            appData.pibObj.length = 2;
            
            if (PAL_PLC_GetMacRtPib(appData.palHandler, &appData.pibObj) == PAL_PLC_SUCCESS)
            {
                appData.panId = *(uint16_t *)appData.pibObj.pData;
                appData.state = APP_STATE_RESET_PAL;
            }
            else
            {
                appData.prevState = APP_STATE_GET_PANID;
                appData.state = APP_STATE_ERROR;
            }
            break;
        }

        case APP_STATE_RESET_PAL:
        {
            if (appData.performPalReset)
            {
                PAL_PLC_Reset(appData.palHandler, true);
            }
            
            appData.state = APP_STATE_WAITING_PAL_READY;
            
            break;
        }

        case APP_STATE_ERROR:
        {
            if (PAL_PLC_Status(appData.palObj) == SYS_STATUS_READY)
            {
                appData.state = appData.prevState;
            }
            break;
        }
        
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
