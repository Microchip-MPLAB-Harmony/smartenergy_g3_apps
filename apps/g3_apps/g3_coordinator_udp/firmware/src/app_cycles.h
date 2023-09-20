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
#include "app_udp_responder.h"

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

/* If APP_CYCLES_METROLOGY_DATA_REQUEST macro is defined, metrology data is
 * requested in cycles through UDP */
//#define APP_CYCLES_METROLOGY_DATA_REQUEST

/* Time to wait before start cycling in ms */
#define APP_CYCLES_TIME_WAIT_CYCLE_MS 120000

/* Time between device cycles in ms */
#define APP_CYCLES_TIME_BTW_DEVICE_CYCLES_MS 30

/* Timeout in ms to consider reply not received */
#define APP_CYCLES_TIMEOUT_MS 10000

#ifndef APP_CYCLES_METROLOGY_DATA_REQUEST
    /* UDP packet size in bytes */
    #define APP_CYCLES_PACKET_SIZE_1 64
    #define APP_CYCLES_PACKET_SIZE_2 300
    #define APP_CYCLES_PACKET_SIZE_3 1000

    /* UDP port for UDP responder (conformance) */
    #define APP_CYCLES_SOCKET_PORT APP_UDP_RESPONDER_SOCKET_PORT_CONFORMANCE
#else
    /* Port number for UDP metrology. This port can be compressed using 6LowPAN
     * (rfc4944, rfc6282) (0xF0B0 - 0xF0BF) */
    #define APP_CYCLES_SOCKET_PORT 0xF0B0
#endif

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

    /* State to wait for first UDP cycle */
    APP_CYCLES_STATE_WAIT_FIRST_CYCLE,

    /* Cycling state: Sending UDP requests to registered devices */
    APP_CYCLES_STATE_CYCLING,

    /* State to wait for the next device UDP cycle */
    APP_CYCLES_STATE_WAIT_NEXT_DEVICE_CYCLE,

    /* Conformance state: Cycling disabled */
    APP_CYCLES_STATE_CONFORMANCE,

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
    /* Total time count between UDP requests and replies */
    uint64_t timeCountTotal;

    /* Total number of cycles */
    uint32_t numCycles;

    /* Total number of UDP requests sent for this device */
    uint32_t numUdpRequests;

    /* Total number of UDP replies received for this device */
    uint32_t numUdpReplies;

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
    /* Total time count between UDP requests and replies */
    uint64_t timeCountTotal;

    /* Total time count between UDP requests and replies for the current cycle */
    uint64_t timeCountTotalCycle;

    /* Time counter corresponding to UDP request */
    uint64_t timeCountUdpRequest;

    /* TCP/IP Network handle */
    TCPIP_NET_HANDLE netHandle;

    /* Handle for waiting time before first cycle and timeout */
    SYS_TIME_HANDLE timeHandle;

    /* UDP socket handle */
    UDP_SOCKET socket;

    /* Pointer to current statistics entry */
    APP_CYCLES_STATISTICS_ENTRY* pStatsEntry;

    /* Total number of UDP requests sent */
    uint32_t numUdpRequests;

    /* Total number of UDP replies received */
    uint32_t numUdpReplies;

    /* UDP cycle index */
    uint32_t cycleIndex;

    /* Device index to send ping to all connected devices */
    uint16_t deviceIndex;

    /* Number of devices joined to the network */
    uint16_t numDevicesJoined;

#ifndef APP_CYCLES_METROLOGY_DATA_REQUEST
    /* UDP packet size */
    uint16_t packetSize;
#endif

    /* The application's current state */
    APP_CYCLES_STATES state;

    /* Timer expired flag */
    bool timeExpired;

    /* Flag to indicate that ADP buffers are available */
    bool availableBuffers;

    /* Flag to indicate that packet is pending to be sent because of buffer
     * availability */
    bool packetPending;

} APP_CYCLES_DATA;

#ifdef APP_CYCLES_METROLOGY_DATA_REQUEST
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

} APP_CYCLES_METROLOGY_DATA;
#endif

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

/*******************************************************************************
  Function:
    void APP_CYCLES_SetConformanceConfig ( void )

  Summary:
    Disables cycling operation.

  Description:
    This function configures cycles application for Conformance Test. Cycling
    operation is disabled.

  Precondition:
    APP_CYCLES_Initialize should be called before calling this routine.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_CYCLES_SetConformanceConfig();
    </code>

  Remarks:
    None.
*/

void APP_CYCLES_SetConformanceConfig ( void );

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* _APP_CYCLES_H */

/*******************************************************************************
 End of File
 */

