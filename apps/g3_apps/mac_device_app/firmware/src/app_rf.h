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

#ifndef _APP_RF_H
#define _APP_RF_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "configuration.h"
#include "stack/g3/pal/rf/pal_rf.h"

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
    APP_RF_STATE_INIT = 0,
    APP_RF_STATE_WAITING_READY,
    APP_RF_STATE_GET_PHY_VERSION,
    APP_RF_STATE_GET_PHY_CONFIG,
    APP_RF_STATE_SEND_MESSAGE,
    APP_RF_STATE_WAITING_TXCFM,
    APP_RF_STATE_RESET,
    APP_RF_STATE_DEINIT,
    APP_RF_STATE_ERROR,
    APP_RF_STATE_RUNNING,

} APP_RF_STATES;


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
    /* The application's current state */
    APP_RF_STATES state;
    
    APP_RF_STATES prevState;

    SYS_MODULE_OBJ palRfObj;

    PAL_RF_HANDLE palHandler;
    
    SYS_CONSOLE_HANDLE consoleHandler;    
    
    PAL_RF_PIB_OBJ pibObj;
    
    uint8_t fwVersion[6];
    
    DRV_RF215_PHY_CFG_OBJ rfPhyConfig;
    
    uint16_t panId;
    
    bool performRfPhyReset;
    
    bool performRfPhyDeinit;
    
    bool pendingTxCfm;
    
    PAL_RF_PHY_STATUS txStatus;
    
    uint32_t txTimeIniCount;
    
    uint32_t txTimeEndCount;

} APP_RF_DATA;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Routines
// *****************************************************************************
// *****************************************************************************
/* These routines are called by drivers when certain events occur.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_RF_Initialize ( void )

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
    APP_RF_Initialize();
    </code>

  Remarks:
    This routine must be called from the SYS_Initialize function.
*/

void APP_RF_Initialize ( void );


/*******************************************************************************
  Function:
    void APP_RF_Tasks ( void )

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
    APP_RF_Tasks();
    </code>

  Remarks:
    This routine must be called from SYS_Tasks() routine.
 */

void APP_RF_Tasks( void );

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* _APP_RF_H */

/*******************************************************************************
 End of File
 */

