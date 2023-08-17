/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app_g3_management.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_G3_MANAGEMENT_Initialize" and "APP_G3_MANAGEMENT_Tasks" prototypes) and
    some of them are only used internally by the application (such as the
    "APP_G3_MANAGEMENT_STATES" definition). Both are defined here for
    convenience.
*******************************************************************************/

#ifndef _APP_G3_MANAGEMENT_H
#define _APP_G3_MANAGEMENT_H

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
#include "stack/g3/adaptation/adp.h"
#include "system/time/sys_time.h"

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

/* PAN ID */
#define APP_G3_MANAGEMENT_PAN_ID 0x781D

/* Size of PAN ID list */
#define APP_G3_MANAGEMENT_PAN_ID_LIST_SIZE 10

/* Coordinator short address */
#define APP_G3_MANAGEMENT_SHORT_ADDRESS 0x0000

/* Context information table index 0:
 * Context 0 with value IPv6_PREFIX & PAN_ID (length = 80 bits) */
#define APP_G3_MANAGEMENT_CONTEXT_INFO_TABLE_0 {0xFF, 0xFF, 0x01, 0x50, 0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x1D}

/* Context information  table index 1:
 * Context 1 with value 0x112233445566 (length = 48 bits) */
#define APP_G3_MANAGEMENT_CONTEXT_INFO_TABLE_1 {0xFF, 0xFF, 0x01, 0x30, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66}

/* Routing table entry TTL: 180 minutes */
#define APP_G3_MANAGEMENT_ROUTING_TABLE_ENTRY_TTL 180

/* Maximum Hops: 10 */
#define APP_G3_MANAGEMENT_MAX_HOPS 10

/* Retransmit all broadcast messages: True */
#define APP_G3_MANAGEMENT_BROADCAST_ROUTE_ALL 1

/* Duty Cycle Limit RF: 360 (10% out of 3600) */
#define APP_G3_MANAGEMENT_DUTY_CYCLE_LIMIT_RF 360
/* Duty Cycle Limit RF: 3600 (100% out of 3600) */
//#define APP_G3_MANAGEMENT_DUTY_CYCLE_LIMIT_RF 3600

/* Period to blink LED in milliseconds */
#define APP_G3_MANAGEMENT_LED_BLINK_PERIOD_MS 500

#define APP_G3_MANAGEMENT_STRING_HEADER "\r\n-- MCHP G3 Coordinator ICMP " \
    "Cycles application --\r\n-- Compiled: "__DATE__" "__TIME__" --\r\n" \
	"-- HOST version: "DRV_G3_MACRT_HOST_DESC" --\r\n"

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
    /* Application's state machine's initial state. Open ADP ADP. */
    APP_G3_MANAGEMENT_STATE_ADP_OPEN = 0,

    /* State to wait for ADP to be ready */
    APP_G3_MANAGEMENT_STATE_WAIT_ADP_READY,

    /* Network discovery (scan) in progress */
    APP_G3_MANAGEMENT_STATE_SCANNING,

    /* Starting G3 network */
    APP_G3_MANAGEMENT_STATE_STARTING_NETWORK,

    /* G3 network started, accepting connection of devices */
    APP_G3_MANAGEMENT_STATE_NETWORK_STARTED,

    /* Error state */
    APP_G3_MANAGEMENT_STATE_ERROR,

} APP_G3_MANAGEMENT_STATES;


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
    /* Device's extended address (EUI64). It must be unique for each device. */
    ADP_EXTENDED_ADDRESS eui64;

    /* Timer handle for LED blinking */
    SYS_TIME_HANDLE timerLedHandle;

    /* G3 network PAN Identifier */
    uint16_t panId;

    /* The application's current state */
    APP_G3_MANAGEMENT_STATES state;

    /* Number of networks found in discovery */
    uint8_t numNetworksFound;

    /* Flag to indicate if non-volatile data PIBs have to be written once ADP is
     * ready */
    bool writeNonVolatileData;

    /* Flag to indicate if LED blinking time has expired */
    bool timerLedExpired;

} APP_G3_MANAGEMENT_DATA;

// *****************************************************************************
/* Application Constants

  Summary:
    Holds application constants.

  Description:
    This structure holds the G3 application's constants, i.e. the IB values to
    configure ADP/MAC.

  Remarks:
    Application strings and buffers are defined outside this structure.
 */

typedef struct
{
    /* ADP_IB_CONTEXT_INFORMATION_TABLE: Context information table index 0 */
    const uint8_t contextInfoTable0[14];

    /* ADP_IB_CONTEXT_INFORMATION_TABLE: Context information table index 1 */
    const uint8_t contextInfoTable1[10];

    /* MAC_WRP_PIB_SHORT_ADDRESS */
    const uint16_t shortAddress;

    /* ADP_IB_ROUTING_TABLE_ENTRY_TTL */
    const uint16_t routingTableEntryTTL;

    /* MAC_WRP_PIB_DUTY_CYCLE_LIMIT_RF */
    const uint16_t dutyCycleLimitRF;

    /* ADP_IB_MAX_HOPS */
    const uint8_t maxHops;

    /* ADP_IB_MANUF_BROADCAST_ROUTE_ALL */
    const uint8_t broadcastRouteAll;

} APP_G3_MANAGEMENT_CONSTANTS;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Routines
// *****************************************************************************
// *****************************************************************************
/* These routines are called by drivers when certain events occur.
*/

/*******************************************************************************
  Function:
    void APP_SYS_TIME_CallbackSetFlag(uintptr_t context)

  Summary:
    SYS_TIME callback that sets the flag whose address comes in context.

  Description:
    This function can be used as SYS_TIME callback. It sets the flag (bool)
    whose address comes in context.

  Precondition:
    None.

  Parameters:
    context - Context that holds the flag's address. Context is given to
              SYS_TIME when callback is registered.

  Returns:
    None.

  Example:
    <code>
    static bool flag = false;

    SYS_TIME_CallbackRegisterMS(APP_SYS_TIME_CallbackSetFlag, (uintptr_t) &flag,
            500, SYS_TIME_PERIODIC);
    </code>

  Remarks:
    None.
*/

void APP_SYS_TIME_CallbackSetFlag(uintptr_t context);

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_G3_MANAGEMENT_Initialize ( void )

  Summary:
    MPLAB Harmony application initialization routine.

  Description:
    This function initializes the Harmony application.  It places the
    application in its initial state and prepares it to run so that its
    APP_G3_MANAGEMENT_Tasks function can be called.

  Precondition:
    All other system initialization routines should be called before calling
    this routine (in "SYS_Initialize").

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_G3_MANAGEMENT_Initialize();
    </code>

  Remarks:
    This routine must be called from the SYS_Initialize function.
*/

void APP_G3_MANAGEMENT_Initialize ( void );

/*******************************************************************************
  Function:
    void APP_G3_MANAGEMENT_Tasks ( void )

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
    APP_G3_MANAGEMENT_Tasks();
    </code>

  Remarks:
    This routine must be called from SYS_Tasks() routine.
 */

void APP_G3_MANAGEMENT_Tasks( void );

// *****************************************************************************
// *****************************************************************************
// Section: Application Interface Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    uint16_t APP_G3_MANAGEMENT_GetPanId(void)

  Summary:
    Gets PAN identifier.

  Description:
    This function gets the PAN identifier of the network.

  Precondition:
    APP_G3_MANAGEMENT_Initialize should be called before calling this routine.

  Parameters:
    None.

  Returns:
    PAN identifier.

  Example:
    <code>
    uint16_t panId = APP_G3_MANAGEMENT_GetPanId();
    </code>

  Remarks:
    None.
*/

uint16_t APP_G3_MANAGEMENT_GetPanId(void);

/*******************************************************************************
  Function:
    uint8_t* APP_G3_MANAGEMENT_GetExtendedAddress(void)

  Summary:
    Gets extended address.

  Description:
    This function gets the extended address of the device.

  Precondition:
    APP_G3_MANAGEMENT_Initialize should be called before calling this routine.

  Parameters:
    None.

  Returns:
    Pointer to extended address (8 bytes).

  Example:
    <code>
    uint8_t* extendedAddress = APP_G3_MANAGEMENT_GetExtendedAddress();
    </code>

  Remarks:
    None.
*/

uint8_t* APP_G3_MANAGEMENT_GetExtendedAddress(void);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* _APP_G3_MANAGEMENT_H */

/*******************************************************************************
 End of File
 */

