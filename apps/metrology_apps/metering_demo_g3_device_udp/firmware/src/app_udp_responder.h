/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app_udp_responder.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_UDP_RESPONDER_Initialize" and "APP_UDP_RESPONDER_Tasks" prototypes) and
    some of them are only used internally by the application (such as the
    "APP_UDP_RESPONDER_STATES" definition). Both are defined here for
    convenience.
*******************************************************************************/

#ifndef _APP_UDP_RESPONDER_H
#define _APP_UDP_RESPONDER_H

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
#include "library/tcpip/tcpip.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Macro Definitions
// *****************************************************************************
// *****************************************************************************

/* Port number for conformance UDP responder */
#define APP_UDP_RESPONDER_SOCKET_PORT_CONFORMANCE 0xF0BF

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
    /* Application's state machine's initial state */
    APP_UDP_RESPONDER_STATE_WAIT_TCPIP_READY = 0,

    /* Opening UDP server */
    APP_UDP_RESPONDER_STATE_OPENING_SERVER,

    /* Serving connection on UDP port */
    APP_UDP_RESPONDER_STATE_SERVING_CONNECTION,

    /* Error state */
    APP_UDP_RESPONDER_STATE_ERROR,

} APP_UDP_RESPONDER_STATES;


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
    APP_UDP_RESPONDER_STATES state;

} APP_UDP_RESPONDER_DATA;

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_UDP_RESPONDER_Initialize ( void )

  Summary:
     MPLAB Harmony application initialization routine.

  Description:
    This function initializes the Harmony application.  It places the
    application in its initial state and prepares it to run so that its
    APP_UDP_RESPONDER_Tasks function can be called.

  Precondition:
    All other system initialization routines should be called before calling
    this routine (in "SYS_Initialize").

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_UDP_RESPONDER_Initialize();
    </code>

  Remarks:
    This routine must be called from the SYS_Initialize function.
*/

void APP_UDP_RESPONDER_Initialize ( void );

/*******************************************************************************
  Function:
    void APP_UDP_RESPONDER_Tasks ( void )

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
    APP_UDP_RESPONDER_Tasks();
    </code>

  Remarks:
    This routine must be called from SYS_Tasks() routine.
 */

void APP_UDP_RESPONDER_Tasks( void );

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* _APP_UDP_RESPONDER_H */

/*******************************************************************************
 End of File
 */
