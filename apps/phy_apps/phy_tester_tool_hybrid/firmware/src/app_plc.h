/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app_plc.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_PLC_Initialize" and "APP_PLC_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_PLC_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

#ifndef _APP_PLC_H
#define _APP_PLC_H

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
#include "definitions.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Macro definitions
// *****************************************************************************
// *****************************************************************************

#define APP_PLC_DATA_BUFFER_SIZE        512
#define APP_PLC_SERIAL_DATA_BUFFER_SIZE (APP_PLC_DATA_BUFFER_SIZE + 128)
#define APP_PLC_PIB_BUFFER_SIZE         (PROTOCOL_CARRIERS_MAX * 2)

#define APP_PLC_LED_BLINK_RATE_MS       500
#define APP_PLC_LED_BLINK_PLC_MSG_MS    100

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
    APP_PLC_STATE_INIT=0,
    APP_PLC_STATE_REGISTER,
    APP_PLC_STATE_CONFIG_USI,
    APP_PLC_STATE_READY,
    APP_PLC_STATE_ERROR

} APP_PLC_STATES;

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
    APP_PLC_STATES state;

    SYS_TIME_HANDLE tmr1Handle;

    volatile bool tmr1Expired;

    SYS_TIME_HANDLE tmr2Handle;

    volatile bool tmr2Expired;

    DRV_HANDLE drvPlcHandle;

    SRV_USI_HANDLE srvUSIHandle;

    bool plc_phy_exception;

    uint32_t plc_phy_err_unexpected;

    uint32_t plc_phy_err_critical;

    uint32_t plc_phy_err_reset;

    uint32_t plc_phy_err_unknow;

    DRV_PLC_PHY_TRANSMISSION_OBJ plcTxObj;

    DRV_PLC_PHY_PIB_OBJ plcPIB;

    bool pvddMonTxEnable;

} APP_PLC_DATA;

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_PLC_Initialize ( void )

  Summary:
     MPLAB Harmony application initialization routine.

  Description:
    This function initializes the Harmony application.  It places the
    application in its initial state and prepares it to run so that its
    APP_PLC_Tasks function can be called.

  Precondition:
    All other system initialization routines should be called before calling
    this routine (in "SYS_Initialize").

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_PLC_Initialize();
    </code>

  Remarks:
    This routine must be called from the SYS_Initialize function.
*/

void APP_PLC_Initialize ( void );


/*******************************************************************************
  Function:
    void APP_PLC_Tasks ( void )

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
    APP_PLC_Tasks();
    </code>

  Remarks:
    This routine must be called from SYS_Tasks() routine.
 */

void APP_PLC_Tasks( void );

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* _APP_PLC_H */

/*******************************************************************************
 End of File
 */

