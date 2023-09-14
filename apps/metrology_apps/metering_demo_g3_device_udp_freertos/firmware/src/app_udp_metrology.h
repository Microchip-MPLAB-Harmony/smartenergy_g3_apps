/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app_udp_metrology.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_UDP_METROLOGY_Initialize" and "APP_UDP_METROLOGY_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_UDP_METROLOGY_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

#ifndef _APP_UDP_METROLOGY_H
#define _APP_UDP_METROLOGY_H

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

/* Port number for UDP metrology. This port can be compressed using 6LowPAN
 * (rfc4944, rfc6282) (0xF0B0 - 0xF0BF) */
#define APP_UDP_METROLOGY_SOCKET_PORT 0xF0B0

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
    APP_UDP_METROLOGY_STATE_WAIT_TCPIP_READY = 0,

    /* Opening UDP server */
    APP_UDP_METROLOGY_STATE_OPENING_SERVER,

    /* Serving connection on UDP port */
    APP_UDP_METROLOGY_STATE_SERVING_CONNECTION,

    /* Error state */
    APP_UDP_METROLOGY_STATE_ERROR,

} APP_UDP_METROLOGY_STATES;


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
    /* TCP/IP Network handle */
    TCPIP_NET_HANDLE netHandle;

    /* UDP socket handle */
    UDP_SOCKET socket;

    /* The application's current state */
    APP_UDP_METROLOGY_STATES state;

} APP_UDP_METROLOGY_DATA;

// *****************************************************************************
/* Metrology RMS Data

  Summary:
    Holds metrology data.

  Description:
    This structure holds the metrology data to be sent through UDP
    (RMS instantaneous values).

  Remarks:
    None.
 */

typedef struct
{
    /* RMS voltage for phase A */
    uint32_t rmsUA;

    /* RMS voltage for phase B */
    uint32_t rmsUB;

    /* RMS voltage for phase C */
    uint32_t rmsUC;

    /* RMS current for phase A */
    uint32_t rmsIA;

    /* RMS current for phase B */
    uint32_t rmsIB;

    /* RMS current for phase C */
    uint32_t rmsIC;

    /* RMS current for neutral */
    uint32_t rmsINI;

    /* RMS current for neutral */
    uint32_t rmsINM;

    /* RMS current for neutral */
    uint32_t rmsINMI;

    /* RMS active power total */
    int32_t rmsPT;

    /* RMS active power for phase A */
    int32_t rmsPA;

    /* RMS active power for phase B */
    int32_t rmsPB;

    /* RMS active power for phase C */
    int32_t rmsPC;

    /* RMS reactive power total */
    int32_t rmsQT;

    /* RMS reactive power for phase A */
    int32_t rmsQA;

    /* RMS reactive power for phase B */
    int32_t rmsQB;

    /* RMS reactive power for phase C */
    int32_t rmsQC;

    /* RMS aparent power total */
    uint32_t rmsST;

    /* RMS aparent power for phase A */
    uint32_t rmsSA;

    /* RMS aparent power for phase B */
    uint32_t rmsSB;

    /* RMS aparent power for phase C */
    uint32_t rmsSC;

    /* Frequency of the line voltage fundamental harmonic component determined
     * by the Metrology library using the dominant phase */
    uint32_t freq;

    /* Angle between the voltage and current vectors for phase A */
    int32_t angleA;

    /* Angle between the voltage and current vectors for phase B */
    int32_t angleB;

    /* Angle between the voltage and current vectors for phase C */
    int32_t angleC;

    /* Angle between the voltage and current vectors for neutral */
    int32_t angleN;

} APP_UDP_METROLOGY_RESPONSE_DATA;

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
    void APP_UDP_METROLOGY_Initialize ( void )

  Summary:
     MPLAB Harmony application initialization routine.

  Description:
    This function initializes the Harmony application.  It places the
    application in its initial state and prepares it to run so that its
    APP_UDP_METROLOGY_Tasks function can be called.

  Precondition:
    All other system initialization routines should be called before calling
    this routine (in "SYS_Initialize").

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_UDP_METROLOGY_Initialize();
    </code>

  Remarks:
    This routine must be called from the SYS_Initialize function.
*/

void APP_UDP_METROLOGY_Initialize ( void );


/*******************************************************************************
  Function:
    void APP_UDP_METROLOGY_Tasks ( void )

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
    APP_UDP_METROLOGY_Tasks();
    </code>

  Remarks:
    This routine must be called from SYS_Tasks() routine.
 */

void APP_UDP_METROLOGY_Tasks( void );

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* _APP_UDP_METROLOGY_H */

/*******************************************************************************
 End of File
 */

