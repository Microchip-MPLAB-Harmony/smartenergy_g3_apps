/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_Initialize" and "APP_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

#ifndef _APP_H
#define _APP_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "configuration.h"
#include "definitions.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************

#define USER_BLINK_LED_On()           LED_GREEN_On()
#define USER_BLINK_LED_Off()          LED_GREEN_Off()
#define USER_BLINK_LED_Toggle()       LED_GREEN_Toggle()

#define USER_PLC_IND_LED_On()         LED_BLUE_On()
#define USER_PLC_IND_LED_Off()        LED_BLUE_Off()
#define USER_PLC_IND_LED_Toggle()     LED_BLUE_Toggle()

#define CLEAR_WATCHDOG()              DWDT_WDT0_Clear()

// *****************************************************************************
/* Application states

  Summary:
    Application states enumeration

  Description:
    This enumeration defines the valid application states.  These states
    determine the behavior of the application at various times.
*/

typedef enum
{
    /* Application's state machine's initial state. */
    APP_PLC_STATE_IDLE=0,
    APP_PLC_STATE_INIT,
    APP_PLC_STATE_OPEN,
    APP_PLC_STATE_SET_NEXT_TX,
    APP_PLC_STATE_WAITING,
    APP_PLC_STATE_TX,
    APP_PLC_STATE_WAITING_TX_CFM,
    APP_PLC_STATE_EXCEPTION,
    APP_PLC_STATE_ERROR

} APP_PLC_STATES;

/* PLC Transmission Status

  Summary:
    PLC Transmission states enumeration

  Description:
    This structure holds the PLC transmission's status.
 */

typedef enum
{
    APP_PLC_TX_STATE_IDLE=0,
    APP_PLC_TX_STATE_WAIT_TX_CFM

} APP_PLC_TX_STATE;

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    Application strings and buffers are be defined outside this structure.
 */

typedef struct
{
    SYS_TIME_HANDLE tmr1Handle;

    volatile bool tmr1Expired;

    SYS_TIME_HANDLE tmr2Handle;

    volatile bool tmr2Expired;

    SYS_TIME_HANDLE tmr3Handle;

    volatile bool tmr3Expired;

    DRV_HANDLE drvPlcHandle;

    bool pvddMonTxEnable;

    APP_PLC_STATES state;

    APP_PLC_TX_STATE plcTxState;

    MAC_RT_PIB_OBJ plcPIB;

    SRV_PLC_PCOUP_BRANCH couplingBranch;

} APP_PLC_DATA;

typedef struct
{
    uint8_t *pTxFrame;

    MAC_RT_HEADER txHeader;

    MAC_RT_RX_PARAMETERS_OBJ rxParams;

    MAC_RT_STATUS lastTxStatus;

    bool ackRequest;

} APP_PLC_DATA_TX;


/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Summary:
     MPLAB Harmony application initialization routine.

  Description:
    This function initializes the Harmony application.  It places the
    application in its initial state and prepares it to run so that its
    APP_Tasks function can be called.

  Precondition:
    All other system initialization routines should be called before calling
    this routine (in "SYS_Initialize").

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_Initialize();
    </code>

  Remarks:
    This routine must be called from the SYS_Initialize function.
*/

void APP_Initialize ( void );


/*******************************************************************************
  Function:
    void APP_Tasks ( void )

  Summary:
    MPLAB Harmony Demo application tasks function

  Description:
    This routine is the Harmony Demo application's tasks function.  It
    defines the application's state machine and core logic.

  Precondition:
    The system and application initialization ("SYS_Initialize") should be
    called before calling this.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_Tasks();
    </code>

  Remarks:
    This routine must be called from SYS_Tasks() routine.
 */

void APP_Tasks( void );


#endif /* _APP_PLC_H */

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

/*******************************************************************************
 End of File
 */

