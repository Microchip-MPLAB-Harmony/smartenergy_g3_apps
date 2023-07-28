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

/* Generic G3 IPv6 local-link address */
#define APP_TCPIP_IPV6_LINK_LOCAL_ADDRESS_G3 "FE80:0:0:0:781D:FF:FE00:0001"

/* Network prefix for G3 unique local address (ULA) */
#define APP_TCPIP_IPV6_NETWORK_PREFIX_G3     "FD00:0:2:781D:0:0:0:0"
#define APP_TCPIP_IPV6_NETWORK_PREFIX_G3_LEN 64

/* IPv6 Multi-cast groups required in Conformance Test */
#define APP_UDP_RESPONDER_IPV6_MULTICAST_0_CONFORMANCE "ff02:0:0:0:0:0:0:1"
#define APP_UDP_RESPONDER_IPV6_MULTICAST_1_CONFORMANCE "ff12:30:1122:3344:5566:0:123:4567"

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
    /* IPv6 link-local address */
    IPV6_ADDR linkLocalAddress;

    /* IPv6 unique local address (ULA) */
    IPV6_ADDR uniqueLocalAddress;

    /* Coordinator IPv6 link-local address */
    IPV6_ADDR coordLinkLocalAddress;

    /* TCP/IP Network handle */
    TCPIP_NET_HANDLE netHandle;

    /* UDP socket handle */
    UDP_SOCKET socket;

    /* The application's current state */
    APP_UDP_RESPONDER_STATES state;

    /* Conformance Test flag */
    bool conformanceTest;

} APP_UDP_RESPONDER_DATA;

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

// *****************************************************************************
// *****************************************************************************
// Section: Application Interface Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_UDP_RESPONDER_SetConformanceConfig ( void )

  Summary:
    Configures TCP/IP stack for Conformance Test.

  Description:
    This function configures TCP/IP stack parameters for Conformance Test.
    IPv6 multi-cast groups needed for Conformance are configured.

  Precondition:
    APP_UDP_RESPONDER_Initialize should be called before calling this routine.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_UDP_RESPONDER_SetConformanceConfig();
    </code>

  Remarks:
    None.
*/

void APP_UDP_RESPONDER_SetConformanceConfig ( void );

/*******************************************************************************
  Function:
    void APP_UDP_RESPONDER_NetworkJoined()

  Summary:
    Configures IPv6 addresses once the device is joined to the network.

  Description:
    This function configures IPv6 addresses once the device is joined to the
    network.

  Precondition:
    APP_UDP_RESPONDER_Initialize should be called before calling this routine.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_UDP_RESPONDER_NetworkJoined();
    </code>

  Remarks:
    None.
*/

void APP_UDP_RESPONDER_NetworkJoined();

/*******************************************************************************
  Function:
    void APP_UDP_RESPONDER_NetworkDisconnected(void)

  Summary:
    Removes IPv6 addresses once the device leaves the network.

  Description:
    This function removes IPv6 addresses once the device leaves the network.

  Precondition:
    APP_UDP_RESPONDER_Initialize should be called before calling this routine.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_UDP_RESPONDER_NetworkDisconnected();
    </code>

  Remarks:
    None.
*/

void APP_UDP_RESPONDER_NetworkDisconnected(void);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* _APP_UDP_RESPONDER_H */

/*******************************************************************************
 End of File
 */
