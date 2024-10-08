/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_display.c

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

#include "app_display.h"
#include "definitions.h"

extern APP_PLC_DATA appPlc;

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
    This structure should be initialized by the APP_DISPLAY_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_DISPLAY_DATA app_displayData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
*/


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_DISPLAY_Initialize ( void )

  Remarks:
    See prototype in app_display.h.
 */

void APP_DISPLAY_Initialize ( void )
{
    status_code_t status;

    /* Initialize the CL010 LCD glass component */
    status = cl010_init();
    if (status != STATUS_OK) 
    {
        app_displayData.state = APP_DISPLAY_STATE_ERROR;
    }
    else
    {
        /* Place the App state machine in its initial state. */
        app_displayData.state = APP_DISPLAY_STATE_INIT;
    }
}


/******************************************************************************
  Function:
    void APP_DISPLAY_Tasks ( void )

  Remarks:
    See prototype in app_display.h.
 */

void APP_DISPLAY_Tasks ( void )
{

    /* Check the application's current state. */
    switch ( app_displayData.state )
    {
        /* Application's initial state. */
        case APP_DISPLAY_STATE_INIT:
        {
            cl010_clear_all();
            cl010_show_numeric_string(CL010_LINE_DOWN, (const uint8_t *)"0000460");
            
            cl010_show_icon(CL010_ICON_P_PLUS);
            cl010_show_icon(CL010_ICON_P_MINUS);
            
            cl010_show_icon(CL010_ICON_MICROCHIP);
            
            app_displayData.state = APP_DISPLAY_STATE_SERVICE_TASKS;
            break;
        }

        case APP_DISPLAY_STATE_SERVICE_TASKS:
        {
            if (appPlc.pvddMonTxEnable)
            {
                cl010_clear_icon(CL010_ICON_SWITCH_OPEN);
                cl010_show_icon(CL010_ICON_PHASE_1);
            }
            else
            {
                cl010_show_icon(CL010_ICON_SWITCH_OPEN);
                cl010_clear_icon(CL010_ICON_PHASE_1);
            }
            break;
        }

        case APP_DISPLAY_STATE_ERROR:
        {
            cl010_show_all();
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
