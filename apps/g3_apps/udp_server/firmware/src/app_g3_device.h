/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app_g3_device.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_G3_DEVICE_Initialize" and "APP_G3_DEVICE_Tasks" prototypes) and some of
    them are only used internally by the application (such as the
    "APP_G3_DEVICE_STATES" definition). Both are defined here for convenience.
*******************************************************************************/

#ifndef _APP_G3_DEVICE_H
#define _APP_G3_DEVICE_H

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

/* If APP_G3_CONFORMANCE_TEST macro is defined, ADP parameters are configured
 * according to Conformance test */
//#define APP_G3_CONFORMANCE_TEST

/* PSK / Network authentication Key (16 bytes) */
#define APP_G3_PSK_KEY {0xAB, 0x10, 0x34, 0x11, 0x45, 0x11, 0x1B, 0xC3, 0xC1, 0x2D, 0xE8, 0xFF, 0x11, 0x14, 0x22, 0x04}

/* Context information table index 0:
 * Context 0 with value IPv6_PREFIX & PAN_ID (length = 80 bits) */
#define APP_G3_CONTEXT_INFO_TABLE_0 {0x02, 0x00, 0x01, 0x50, 0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x1D}

/* Context information  table index 1:
 * Context 1 with value 0x112233445566 (length = 48 bits) */
#define APP_G3_CONTEXT_INFO_TABLE_1 {0x02, 0x00, 0x01, 0x30, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66}

/* Routing table entry TTL: 180 minutes */
#define APP_G3_ROUTING_TABLE_ENTRY_TTL 180

/* Routing table entry TTL for Conformance: 5 minutes */
#define APP_G3_ROUTING_TABLE_ENTRY_TTL_CONFORMANCE 5

/* Maximum Join Time: 90 seconds */
#define APP_G3_MAX_JOIN_WAIT_TIME 90

/* Maximum Join Time for Conformance: 20 seconds */
#define APP_G3_MAX_JOIN_WAIT_TIME_CONFORMANCE 20

/* Maximum Hops: 10 */
#define APP_G3_MAX_HOPS 10

/* Maximum Hops for Conformance: 8 */
#define APP_G3_MAX_HOPS_CONFORMANCE 8

/* Default Route to Coordinator Enabled: True */
#define APP_G3_DEVICE_DEFAULT_COORD_ROUTE_ENABLED 1

/* Duty Cycle Limit RF: 90 (2.5% out of 3600) */
#define APP_G3_DEVICE_DUTY_CYCLE_LIMIT_RF 90
/* Duty Cycle Limit RF: 3600 (100% out of 3600) */
//#define APP_G3_DEVICE_DUTY_CYCLE_LIMIT_RF 3600

/* Blacklist table entry TTL for Conformance: 2 minutes */
#define APP_G3_BLACKLIST_TABLE_ENTRY_TTL_CONFORMANCE 2

/* Group Table index 0: 0x8001 (note that the IPv6 layer must listen to
 * ff02::1 in correspondence to this group) */
#define APP_G3_GROUP_TABLE_0_CONFORMANCE 0x8001

/* Group Table index 1: 0x8567 (note that the IPv6 layer must listen to
 * ff12:30:1122:3344:5566:0:123:4567 in correspondence to this group) */
#define APP_G3_GROUP_TABLE_1_CONFORMANCE 0x8567

/* Tone-Map Response TTL for Conformance: 2 minutes */
#define APP_G3_TMR_TTL_CONFORMANCE 2

/* Destination Address Set index 0 for Conformance: address added */
#define APP_G3_DEST_ADDR_SET_0_CONFORMANCE 0x7FFF

/* Maximum CSMA Back-offs RF for Conformance: 50 */
#define APP_G3_MAX_CSMA_BACKOFFS_RF_CONFORMANCE 50

/* Maximum Frame Retries RF for Conformance: 5 */
#define APP_G3_MAX_FRAME_RETRIES_RF_CONFORMANCE 5

/* POS Table Entry TTL for Conformance: 3 minutes */
#define APP_G3_POS_TABLE_TTL_CONFORMANCE 3

/* Routing parameters for Conformance */
#define APP_G3_KR_CONFORMANCE     0
#define APP_G3_KM_CONFORMANCE     0
#define APP_G3_KC_CONFORMANCE     0
#define APP_G3_KQ_CONFORMANCE     0
#define APP_G3_KH_CONFORMANCE     4
#define APP_G3_KRT_CONFORMANCE    0
#define APP_G3_KQ_RF_CONFORMANCE  0
#define APP_G3_KH_RF_CONFORMANCE  8
#define APP_G3_KRT_RF_CONFORMANCE 0
#define APP_G3_KDC_RF_CONFORMANCE 10

/* Back-off delay times for network discovery in milliseconds */
#define APP_G3_DEVICE_DISCOVERY_BACKOFF_LOW_MIN  1000
#define APP_G3_DEVICE_DISCOVERY_BACKOFF_LOW_MAX  20000
#define APP_G3_DEVICE_DISCOVERY_BACKOFF_HIGH_MIN 5000
#define APP_G3_DEVICE_DISCOVERY_BACKOFF_HIGH_MAX 100000

/* Network discovery (scan) duration in seconds */
#define APP_G3_DEVICE_DISCOVERY_DURATION 15

/* Minimum Link Quality to consider a good network to join */
#define APP_G3_DEVICE_LQI_MIN 53

/* Maximum Link Quality to consider a good network to join */
#define APP_G3_DEVICE_ROUTE_COST_COORD_MAX 0x7FFF

/* Back-off delay times for network discovery in milliseconds */
#define APP_G3_DEVICE_JOIN_BACKOFF_LOW_MIN  100
#define APP_G3_DEVICE_JOIN_BACKOFF_LOW_MAX  2000
#define APP_G3_DEVICE_JOIN_BACKOFF_HIGH_MIN 2500
#define APP_G3_DEVICE_JOIN_BACKOFF_HIGH_MAX 10000

/* Period to blink LED in milliseconds */
#define APP_G3_LED_BLINK_PERIOD_MS 500

/* Key to detect valid non-volatile data in GPBR */
#define APP_G3_DEVICE_NON_VOLATILE_DATA_KEY_GPBR 0xA5

/* Key to detect valid non-volatile data in User Signature */
#define APP_G3_DEVICE_NON_VOLATILE_DATA_KEY_USER_SIGNATURE 0xA55AA55A

/* Size of non-volatile data in User Signature (in 32-bits words) */
#define APP_G3_NON_VOLATILE_DATA_USER_SIGNATURE_SIZE (((sizeof(ADP_NON_VOLATILE_DATA_IND_PARAMS) + 3) / 4) + 1)

#define APP_G3_DEVICE_STRING_HEADER "\r\n-- MCHP G3 Device UDP Responder application --\r\n" \
	"-- Compiled: "__DATE__" "__TIME__" --\r\n" \
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
    This enumeration defines the valid application states. These states
    determine the behavior of the application at various times.
*/

typedef enum
{
    /* Application's state machine's initial state. Open ADP ADP. */
    APP_G3_DEVICE_STATE_ADP_OPEN = 0,

    /* State to wait for ADP to be ready */
    APP_G3_DEVICE_STATE_WAIT_ADP_READY,

    /* State to start back-off delay before start network discovery (scan) */
    APP_G3_DEVICE_STATE_START_BACKOFF_DISCOVERY,

    /* Back-off delay state before start network discovery (scan) */
    APP_G3_DEVICE_STATE_BACKOFF_DISCOVERY,

    /* Network discovery (scan) in progress */
    APP_G3_DEVICE_STATE_SCANNING,

    /* State to start back-off delay before join to the network */
    APP_G3_DEVICE_STATE_START_BACKOFF_JOIN,

    /* Back-off delay state before join to the network */
    APP_G3_DEVICE_STATE_BACKOFF_JOIN,

    /* Join to the network in progress */
    APP_G3_DEVICE_STATE_JOINING,

    /* Joined to the network */
    APP_G3_DEVICE_STATE_JOINED,

    /* Error state */
    APP_G3_DEVICE_STATE_ERROR,

} APP_G3_DEVICE_STATES;


// *****************************************************************************
/* Application Data

  Summary:
    Holds application data.

  Description:
    This structure holds the application's data.

  Remarks:
    Application strings and buffers are defined outside this structure.
 */

typedef struct
{
    /* PAN descriptor of the best network found during network discovery */
    ADP_PAN_DESCRIPTOR bestNetwork;

    /* Current non-volatile data */
    ADP_NON_VOLATILE_DATA_IND_PARAMS nonVolatileData;

    /* Device's extended address (EUI64). It must be unique for each device. */
    ADP_EXTENDED_ADDRESS eui64;

    /* Timer handle for LED blinking */
    SYS_TIME_HANDLE timerLedHandle;

    /* Upper bound of back-off window for network discovery or join */
    uint32_t backoffWindowHigh;

    /* Lower bound of back-off window for network discovery or join */
    uint32_t backoffWindowLow;

    /* Device short address assigned in network join */
    uint16_t shortAddress;

    /* PAN identifier of the network which the device is connected to */
    uint16_t panId;

    /* The application's current state */
    APP_G3_DEVICE_STATES state;

    /* Number of join retries */
    uint8_t joinRetries;

    /* Conformance Test flag */
    bool conformanceTest;

    /* Flag to indicate if non-volatile data PIBs have to be written once ADP is
     * ready */
    bool writeNonVolatileData;

    /* Flag to indicate if non-volatile data is valid */
    bool validNonVolatileData;

    /* Flag to indicate if back-off time has expired */
    bool backoffExpired;

    /* Flag to indicate if LED blinking time has expired */
    bool timerLedExpired;

} APP_G3_DEVICE_DATA;

// *****************************************************************************
/* Application Data

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
    /* LBP_IB_PSK: PSK / Network authentication Key */
    const uint8_t psk[16];

    /* ADP_IB_CONTEXT_INFORMATION_TABLE: Context information table index 0 */
    const uint8_t contextInfoTable0[14];

    /* ADP_IB_CONTEXT_INFORMATION_TABLE: Context information table index 1 */
    const uint8_t contextInfoTable1[10];

    /* ADP_IB_ROUTING_TABLE_ENTRY_TTL */
    const uint16_t routingTableEntryTTL;

    /* ADP_IB_ROUTING_TABLE_ENTRY_TTL for Conformance */
    const uint16_t routingTableEntryTTLconformance;

    /* ADP_IB_MAX_JOIN_WAIT_TIME */
    const uint16_t maxJoinWaitTime;

    /* ADP_IB_MAX_JOIN_WAIT_TIME for Conformance */
    const uint16_t maxJoinWaitTimeConformance;

    /* MAC_WRP_PIB_DUTY_CYCLE_LIMIT_RF */
    const uint16_t dutyCycleLimitRF;

    /* ADP_IB_BLACKLIST_TABLE_ENTRY_TTL for Conformance */
    const uint16_t blacklistTableEntryTTLconformance;

    /* ADP_IB_GROUP_TABLE: Group Table index 0 for Conformance */
    const uint16_t gropTable0Conformance;

    /* ADP_IB_GROUP_TABLE: Group Table index 1 for Conformance */
    const uint16_t gropTable1Conformance;

    /* ADP_IB_DESTINATION_ADDRESS_SET: Destination Address Set index 0 for
     * Conformance */
    const uint16_t destAddrSet0Conformance;

    /* ADP_IB_MAX_HOPS */
    const uint8_t maxHops;

    /* ADP_IB_MAX_HOPS for Conformance */
    const uint8_t maxHopsConformance;

    /* ADP_IB_DEFAULT_COORD_ROUTE_ENABLED */
    const uint8_t defaultCoordRouteEnabled;

    /* MAC_WRP_PIB_TMR_TTL for Conformance */
    const uint8_t tmrTTLconformance;

    /* MAC_WRP_PIB_MAX_CSMA_BACKOFFS_RF for Conformance */
    const uint8_t maxCSMAbackoffsRFconformance;

    /* MAC_WRP_PIB_MAX_FRAME_RETRIES_RF for Conformance */
    const uint8_t maxFrameRetriesRFconformance;

    /* MAC_WRP_PIB_POS_TABLE_ENTRY_TTL for Conformance */
    const uint8_t posTableEntryTTLconformance;

    /* ADP_IB_KR for Conformance */
    const uint8_t krConformance;

    /* ADP_IB_KM for Conformance */
    const uint8_t kmConformance;

    /* ADP_IB_KC for Conformance */
    const uint8_t kcConformance;

    /* ADP_IB_KQ for Conformance */
    const uint8_t kqConformance;

    /* ADP_IB_KH for Conformance */
    const uint8_t khConformance;

    /* ADP_IB_KRT for Conformance */
    const uint8_t krtConformance;

    /* ADP_IB_KQ_RF for Conformance */
    const uint8_t kqRFconformance;

    /* ADP_IB_KH_RF for Conformance */
    const uint8_t khRFconformance;

    /* ADP_IB_KRT_RF for Conformance */
    const uint8_t krtRFconformance;

    /* ADP_IB_KDC_RF for Conformance */
    const uint8_t kdcRFconformance;

} APP_G3_DEVICE_CONSTANTS;

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_G3_DEVICE_Initialize ( void )

  Summary:
     MPLAB Harmony application initialization routine.

  Description:
    This function initializes the Harmony application.  It places the
    application in its initial state and prepares it to run so that its
    APP_G3_DEVICE_Tasks function can be called.

  Precondition:
    All other system initialization routines should be called before calling
    this routine (in "SYS_Initialize").

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_G3_DEVICE_Initialize();
    </code>

  Remarks:
    This routine must be called from the SYS_Initialize function.
*/

void APP_G3_DEVICE_Initialize ( void );


/*******************************************************************************
  Function:
    void APP_G3_DEVICE_Tasks ( void )

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
    APP_G3_DEVICE_Tasks();
    </code>

  Remarks:
    This routine must be called from SYS_Tasks() routine.
 */

void APP_G3_DEVICE_Tasks( void );

// *****************************************************************************
// *****************************************************************************
// Section: Application Interface Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_G3_DEVICE_SetConformanceConfig ( void )

  Summary:
    Configures ADP/MAC parameters for Conformance Test.

  Description:
    This function configures ADP/MAC IB parameters for Conformance Test. The
    parameters values can be found above in this file in Macro Definitions
    section.

  Precondition:
    APP_G3_DEVICE_Initialize should be called before calling this routine.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_G3_DEVICE_SetConformanceConfig();
    </code>

  Remarks:
    In this demo application this function is never called because conformance
    test is enabled/disabled at compilation time.
    The intention of this function is to be called by any condition that enables
    the conformance test at execution time.
*/

void APP_G3_DEVICE_SetConformanceConfig ( void );

/*******************************************************************************
  Function:
    uint8_t APP_G3_SetConformanceTrickleConfig(uint8_t trickleActivation)

  Summary:
    Configures the ADP Conformance trickle mode from UDP responder message.

  Description:
    This function configures the ADP Conformance trickle mode from UDP responder
    message.

  Precondition:
    APP_G3_DEVICE_Initialize should be called before calling this routine.

  Parameters:
    trickleActivation - Trickle activation (0 disabled, 1 enabled).

  Returns:
    Result of configuration. 0 if successful, 1 otherwise.

  Example:
    <code>
    uint8_t result = APP_G3_SetConformanceTrickleConfig(1);
    </code>

  Remarks:
    None.
*/

uint8_t APP_G3_SetConformanceTrickleConfig(uint8_t trickleActivation);

/*******************************************************************************
  Function:
    uint8_t APP_G3_SetConfigRF(uint8_t* pParameters)

  Summary:
    Configures the RF PHY layer from UDP responder message.

  Description:
    This function configures the RF PHY layer from UDP responder message.

  Precondition:
    APP_G3_DEVICE_Initialize should be called before calling this routine.

  Parameters:
    pParameters - Pointer to RF configuration parameters.

  Returns:
    Result of configuration. 0 if successful, 1 if failed, 2 if unsupported.

  Example:
    <code>
    uint8_t parameters[4];

    uint8_t result = APP_G3_SetConfigRF(parameters);
    </code>

  Remarks:
    None.
*/

uint8_t APP_G3_SetConfigRF(uint8_t* pParameters);

/*******************************************************************************
  Function:
    void APP_G3_SetContinuousTxRF ( void )

  Summary:
    Configures the RF PHY in continuous transmission mode.

  Description:
    This function configures the RF PHY in continuous transmission mode. A D-M1
    signal is transmitted, as defined in ETSI EN 303 204, at the center of the
    configured channel.

  Precondition:
    APP_G3_DEVICE_Initialize should be called before calling this routine.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_G3_SetContinuousTxRF();
    </code>

  Remarks:
    None.
*/

void APP_G3_SetContinuousTxRF ( void );

/*******************************************************************************
  Function:
    uint16_t APP_G3_GetShortAddress(void)

  Summary:
    Gets short address.

  Description:
    This function gets the short address assigned to the device in network join.

  Precondition:
    APP_G3_DEVICE_Initialize should be called before calling this routine.

  Parameters:
    None.

  Returns:
    Short address.

  Example:
    <code>
    uint16_t shortAddress = APP_G3_GetShortAddress();
    </code>

  Remarks:
    None.
*/

uint16_t APP_G3_GetShortAddress(void);

/*******************************************************************************
  Function:
    uint8_t* APP_G3_GetExtendedAddress(void)

  Summary:
    Gets extended address.

  Description:
    This function gets the extended address of the device.

  Precondition:
    APP_G3_DEVICE_Initialize should be called before calling this routine.

  Parameters:
    None.

  Returns:
    Pointer to extended address (8 bytes).

  Example:
    <code>
    uint8_t* extendedAddress = APP_G3_GetExtendedAddress();
    </code>

  Remarks:
    None.
*/

uint8_t* APP_G3_GetExtendedAddress(void);

/*******************************************************************************
  Function:
    uint16_t APP_G3_GetPanId(void)

  Summary:
    Gets PAN identifier.

  Description:
    This function gets the PAN identifier of the network which the device is
    connected to.

  Precondition:
    APP_G3_DEVICE_Initialize should be called before calling this routine.

  Parameters:
    None.

  Returns:
    PAN identifier.

  Example:
    <code>
    uint16_t panId = APP_G3_GetPanId();
    </code>

  Remarks:
    None.
*/

uint16_t APP_G3_GetPanId(void);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* _APP_G3_DEVICE_H */

/*******************************************************************************
 End of File
 */

