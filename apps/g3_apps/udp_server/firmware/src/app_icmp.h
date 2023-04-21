/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app_icmp.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_ICMP_Initialize" and "APP_ICMP_Tasks" prototypes) and some of them are
    only used internally by the application (such as the "APP_ICMP_STATES"
    definition). Both are defined here for convenience.
*******************************************************************************/

#ifndef _APP_ICMP_H
#define _APP_ICMP_H

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
#include "system/time/sys_time.h"
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

/* Time to wait before start cycling in ms (approx. 60 sec per device) */
#define APP_ICMP_TIME_WAIT_CYCLE_MS 120000

/* Timeout in milliseconds to consider echo reply not received */
#define APP_ICMP_TIMEOUT_MS 10000

/* ICMPv6 echo request packet size in bytes */
#define APP_ICMP_PACKET_SIZE_1 64
#define APP_ICMP_PACKET_SIZE_2 300
#define APP_ICMP_PACKET_SIZE_3 1000

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
    APP_ICMP_STATE_WAIT_TCPIP_READY = 0,

    /* State to wait for the first device joined to the G3 network */
    APP_ICMP_STATE_WAIT_FIRST_JOIN,

    /* State to wait for first ICMPv6 cycle */
    APP_ICMP_STATE_WAIT_FIRST_CYCLE,

    /* Cycling state: Sending ICMPv6 echo requests to registered devices */
    APP_ICMP_STATE_CYCLING,

    /* Error state */
    APP_ICMP_STATE_ERROR,

} APP_ICMP_STATES;

// *****************************************************************************
/* ICMPv6 Statistics Entry

  Summary:
    Holds ICMPv6 statistics data for a specific device.

  Description:
    This structure holds the ICMPv6 statistics data for a specific device.

  Remarks:
    None.
 */

typedef struct
{
    /* Total time count between ICMPv6 echo requests and replies */
    uint64_t timeCountTotal;

    /* Total number of ICMPv6 echo requests sent for this device */
    uint32_t numEchoRequests;

    /* Total number of ICMPv6 echo replies received for this device */
    uint32_t numEchoReplies;

    /* Short address of this device */
    uint16_t shortAddress;

} APP_ICMP_STATISTICS_ENTRY;

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
    /* Target address to send ICMPv6 echo request */
    IPV6_ADDR targetAddress;

    /* Time counter corresponding to first ICMPv6 cycle start */
    uint64_t timeCountFirstCycleStart;

    /* Time counter corresponding to current ICMPv6 cycle start */
    uint64_t timeCountCycleStart;

    /* Time counter corresponding to ICMPv6 echo request */
    uint64_t timeCountEchoRequest;

    /* TCP/IP Network handle */
    TCPIP_NET_HANDLE netHandle;

    /* Handle for waiting time before first cycle and timeout */
    SYS_TIME_HANDLE timeHandle;

    /* Pointer to current statistics entry */
    APP_ICMP_STATISTICS_ENTRY* pStatsEntry;

    /* Total number of ICMPv6 echo requests sent */
    uint32_t numEchoRequests;

    /* Total number of ICMPv6 echo replies received */
    uint32_t numEchoReplies;

    /* ICMPv6 cycle index */
    uint32_t cycleIndex;

    /* Device index to send ping to all connected devices */
    uint16_t deviceIndex;

    /* Number of devices joined to the network */
    uint16_t numDevicesJoined;

    /* ICMPv6 sequence number */
    uint16_t sequenceNumber;

    /* ICMPv6 packet size */
    uint16_t packetSize;

    /* The application's current state */
    APP_ICMP_STATES state;

    /* Timer expired flag */
    bool timeExpired;

    /* ICMPv6 echo request result */
    bool icmpResult;

} APP_ICMP_DATA;

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
    void APP_ICMP_Initialize ( void )

  Summary:
     MPLAB Harmony application initialization routine.

  Description:
    This function initializes the Harmony application.  It places the
    application in its initial state and prepares it to run so that its
    APP_ICMP_Tasks function can be called.

  Precondition:
    All other system initialization routines should be called before calling
    this routine (in "SYS_Initialize").

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_ICMP_Initialize();
    </code>

  Remarks:
    This routine must be called from the SYS_Initialize function.
*/

void APP_ICMP_Initialize ( void );


/*******************************************************************************
  Function:
    void APP_ICMP_Tasks ( void )

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
    APP_ICMP_Tasks();
    </code>

  Remarks:
    This routine must be called from SYS_Tasks() routine.
 */

void APP_ICMP_Tasks( void );

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* _APP_ICMP_H */

/*******************************************************************************
 End of File
 */

