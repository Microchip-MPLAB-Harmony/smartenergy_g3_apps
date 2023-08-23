/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app_cycles.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_CYCLES_Initialize" and "APP_CYCLES_Tasks" prototypes) and some of them
    are only used internally by the application (such as the "APP_CYCLES_STATES"
    definition). Both are defined here for convenience.
*******************************************************************************/

#ifndef _APP_CYCLES_H
#define _APP_CYCLES_H

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
#include "stack/g3/adaptation/adp.h"

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

/* Time to wait before start cycling in ms (approx. 60 sec per device) */
#define APP_CYCLES_TIME_WAIT_CYCLE_MS 120000

/* Timeout in ms to consider echo reply not received */
#define APP_CYCLES_TIMEOUT_MS 10000

/* ICMPv6 echo request packet size in bytes */
#define APP_CYCLES_PACKET_SIZE_1 64
#define APP_CYCLES_PACKET_SIZE_2 300
#define APP_CYCLES_PACKET_SIZE_3 1000

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
    APP_CYCLES_STATE_WAIT_TCPIP_READY = 0,

    /* State to wait for the first device joined to the G3 network */
    APP_CYCLES_STATE_WAIT_FIRST_JOIN,

    /* State to wait for first ICMPv6 cycle */
    APP_CYCLES_STATE_WAIT_FIRST_CYCLE,

    /* Cycling state: Sending ICMPv6 echo requests to registered devices */
    APP_CYCLES_STATE_CYCLING,

    /* Error state */
    APP_CYCLES_STATE_ERROR,

} APP_CYCLES_STATES;

// *****************************************************************************
/* Cycles Statistics Entry

  Summary:
    Holds cycles statistics data for a specific device.

  Description:
    This structure holds the cycles statistics data for a specific device.

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

} APP_CYCLES_STATISTICS_ENTRY;

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
    APP_CYCLES_STATISTICS_ENTRY* pStatsEntry;

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
    APP_CYCLES_STATES state;

    /* Timer expired flag */
    bool timeExpired;

    /* ICMPv6 echo request result */
    bool icmpResult;

    /* Flag to indicate that ADP buffers are available */
    bool availableBuffers;

    /* Flag to indicate that packet is pending to be sent because of buffer
     * availability */
    bool packetPending;

} APP_CYCLES_DATA;

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_CYCLES_Initialize ( void )

  Summary:
     MPLAB Harmony application initialization routine.

  Description:
    This function initializes the Harmony application.  It places the
    application in its initial state and prepares it to run so that its
    APP_CYCLES_Tasks function can be called.

  Precondition:
    All other system initialization routines should be called before calling
    this routine (in "SYS_Initialize").

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_CYCLES_Initialize();
    </code>

  Remarks:
    This routine must be called from the SYS_Initialize function.
*/

void APP_CYCLES_Initialize ( void );

/*******************************************************************************
  Function:
    void APP_CYCLES_Tasks ( void )

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
    APP_CYCLES_Tasks();
    </code>

  Remarks:
    This routine must be called from SYS_Tasks() routine.
 */

void APP_CYCLES_Tasks( void );

// *****************************************************************************
// *****************************************************************************
// Section: Application Interface Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_CYCLES_AdpBufferIndication(ADP_BUFFER_IND_PARAMS* bufferInd)

  Summary:
    Indicates to the cycles application an ADP Buffer Indication event.

  Description:
    This function indicates to the cycles application an ADP Buffer Indication
    event.

  Precondition:
    APP_CYCLES_Initialize should be called before calling this routine.

  Parameters:
    bufferInd - Pointer to bitmap providing availability of Tx buffers

  Returns:
    None.

  Example:
    <code>
    static void _ADP_BufferIndication(ADP_BUFFER_IND_PARAMS* bufferInd)
    {
        APP_CYCLES_AdpBufferIndication(bufferInd);
    }
    </code>

  Remarks:
    None.
*/

void APP_CYCLES_AdpBufferIndication(ADP_BUFFER_IND_PARAMS* bufferInd);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* _APP_CYCLES_H */

/*******************************************************************************
 End of File
 */

