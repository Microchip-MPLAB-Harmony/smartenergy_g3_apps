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

/* If APP_G3_MANAGEMENT_CONFORMANCE_TEST macro is defined, ADP parameters are
 * configured according to Conformance test */
//#define APP_G3_MANAGEMENT_CONFORMANCE_TEST

/* PAN ID */
#define APP_G3_MANAGEMENT_PAN_ID 0x781D

/* Size of PAN ID list */
#define APP_G3_MANAGEMENT_PAN_ID_LIST_SIZE 10

/* Coordinator short address */
#define APP_G3_MANAGEMENT_SHORT_ADDRESS 0x0000

/* PSK / Network authentication Key (16 bytes) for Conformance */
#define APP_G3_MANAGEMENT_PSK_KEY_CONFORMANCE {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF}

/* Context information table index 0:
 * Context 0 with value IPv6_PREFIX & PAN_ID (length = 80 bits) */
#define APP_G3_MANAGEMENT_CONTEXT_INFO_TABLE_0 {0xFF, 0xFF, 0x01, 0x50, 0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x1D}

/* Context information  table index 1:
 * Context 1 with value 0x112233445566 (length = 48 bits) */
#define APP_G3_MANAGEMENT_CONTEXT_INFO_TABLE_1 {0xFF, 0xFF, 0x01, 0x30, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66}

/* Routing table entry TTL: 180 minutes */
#define APP_G3_MANAGEMENT_ROUTING_TABLE_ENTRY_TTL 180

/* Routing table entry TTL for Conformance: 5 minutes */
#define APP_G3_MANAGEMENT_ROUTING_TABLE_ENTRY_TTL_CONFORMANCE 5

/* Maximum Hops: 10 */
#define APP_G3_MANAGEMENT_MAX_HOPS 10

/* Maximum Hops for Conformance: 8 */
#define APP_G3_MANAGEMENT_MAX_HOPS_CONFORMANCE 8

/* Duty Cycle Limit RF: 360 (10% out of 3600) */
#define APP_G3_MANAGEMENT_DUTY_CYCLE_LIMIT_RF 360
/* Duty Cycle Limit RF: 3600 (100% out of 3600) */
//#define APP_G3_MANAGEMENT_DUTY_CYCLE_LIMIT_RF 3600

/* Blacklist table entry TTL for Conformance: 2 minutes */
#define APP_G3_MANAGEMENT_BLACKLIST_TABLE_ENTRY_TTL_CONFORMANCE 2

/* Broadcast Log table entry TTL for Conformance: 2 minutes */
#define APP_G3_MANAGEMENT_BROADCAST_LOG_TABLE_ENTRY_TTL_CONFORMANCE 2

/* Group Table index 0: 0x8001 (note that the IPv6 layer must listen to
 * ff02::1 in correspondence to this group) */
#define APP_G3_MANAGEMENT_GROUP_TABLE_0_CONFORMANCE 0x8001

/* Group Table index 1: 0x8567 (note that the IPv6 layer must listen to
 * ff12:30:1122:3344:5566:0:123:4567 in correspondence to this group) */
#define APP_G3_MANAGEMENT_GROUP_TABLE_1_CONFORMANCE 0x8567

/* Weak LQI for Conformance: 60 */
#define APP_G3_MANAGEMENT_WEAK_LQI_CONFORMANCE 60

/* RF Weak LQI for Conformance: 100 */
#define APP_G3_MANAGEMENT_WEAK_LQI_RF_CONFORMANCE 100

/* Trickle Enabled for Conformance */
#define APP_G3_MANAGEMENT_TRICKLE_DATA_ENABLED_CONFORMANCE 1

/* Trickle Adaptive Ki for Conformance: True */
#define APP_G3_MANAGEMENT_TRICKLE_ADAPTIVE_KI_CONFORMANCE 1

/* Jitter delays for Conformance: 1500ms and 100ms */
#define APP_G3_MANAGEMENT_DELAY_LOW_LQI_CONFORMANCE 1500
#define APP_G3_MANAGEMENT_DELAY_HIGH_LQI_CONFORMANCE 100

/* Cluster Trickle Enabled for Conformance */
#define APP_G3_MANAGEMENT_CLUSTER_TRICKLE_ENABLED_CONFORMANCE 1

/* Cluster Trickle I for Conformance: 1400ms */
#define APP_G3_MANAGEMENT_CLUSTER_TRICKLE_I_CONFORMANCE 1400

/* RF Trickle LQI Thresholds for Conformance: 100 and 120 */
#define APP_G3_MANAGEMENT_TRICKLE_LQI_THRESHOLD_LOW_RF_CONFORMANCE 100
#define APP_G3_MANAGEMENT_TRICKLE_LQI_THRESHOLD_HIGH_RF_CONFORMANCE 120

/* Jitter LQI thresholds for Conformance: 100 and 170 */
#define APP_G3_MANAGEMENT_JITTER_LOW_LQI_RF_CONFORMANCE 100
#define APP_G3_MANAGEMENT_JITTER_HIGH_LQI_RF_CONFORMANCE 170

/* Cluster Min LQI for Conformance: 110 */
#define APP_G3_MANAGEMENT_CLUSTER_MIN_LQI_RF_CONFORMANCE 110

/* Tone-Map Response TTL for Conformance: 3 minutes */
#define APP_G3_MANAGEMENT_TMR_TTL_CONFORMANCE 3

/* Destination Address Set index 0 for Conformance: address added */
#define APP_G3_MANAGEMENT_DEST_ADDR_SET_0_CONFORMANCE 0x7FFF

/* Media Probing Interval for Conformance: 2 minutes */
#define APP_G3_MANAGEMENT_PROBING_INTERVAL_CONFORMANCE 2

/* Maximum CSMA Back-offs for Conformance: 5 */
#define APP_G3_MANAGEMENT_MAX_CSMA_BACKOFFS_CONFORMANCE 5

/* Maximum CSMA Back-offs RF for Conformance: 50 */
#define APP_G3_MANAGEMENT_MAX_CSMA_BACKOFFS_RF_CONFORMANCE 50

/* Maximum Frame Retries RF for Conformance: 5 */
#define APP_G3_MANAGEMENT_MAX_FRAME_RETRIES_RF_CONFORMANCE 5

/* POS Table Entry TTL for Conformance: 4 minutes */
#define APP_G3_MANAGEMENT_POS_TABLE_TTL_CONFORMANCE 4

/* POS Recent Entry Threshold for Conformance: 3 minutes */
#define APP_G3_MANAGEMENT_POS_RECENT_THRESHOLD_CONFORMANCE 3

/* Adaptive Power Low Bound for Conformance: 74 */
#define APP_G3_MANAGEMENT_ADAPTIVE_POWER_LOW_BOUND_CONFORMANCE 74

/* Routing parameters for Conformance */
#define APP_G3_MANAGEMENT_KR_CONFORMANCE     0
#define APP_G3_MANAGEMENT_KM_CONFORMANCE     0
#define APP_G3_MANAGEMENT_KC_CONFORMANCE     0
#define APP_G3_MANAGEMENT_KQ_CONFORMANCE     10
#define APP_G3_MANAGEMENT_KH_CONFORMANCE     4
#define APP_G3_MANAGEMENT_KRT_CONFORMANCE    0
#define APP_G3_MANAGEMENT_KQ_RF_CONFORMANCE  5
#define APP_G3_MANAGEMENT_KH_RF_CONFORMANCE  8
#define APP_G3_MANAGEMENT_KRT_RF_CONFORMANCE 0
#define APP_G3_MANAGEMENT_KDC_RF_CONFORMANCE 10

/* RF frequency band for conformance */
#define APP_G3_MANAGEMENT_RF_FREQ_BAND_CONFORMANCE 15

/* Period to blink LED in milliseconds */
#define APP_G3_MANAGEMENT_LED_BLINK_PERIOD_MS 500

#define APP_G3_MANAGEMENT_STRING_HEADER "\r\n-- MCHP G3 Coordinator UDP Responder" \
    "/Cycles application --\r\n-- Compiled: "__DATE__" "__TIME__" --\r\n" \
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

    /* Saved RF parameters used restore configuration after kick event */
    uint8_t savedParamsRF[4];

    /* Conformance Test flag */
    bool conformanceTest;

    /* Flag to indicate if non-volatile data PIBs have to be written once ADP is
     * ready */
    bool writeNonVolatileData;

    /* Flag to indicate if LED blinking time has expired */
    bool timerLedExpired;

    /* Flag to indicate if RF configuration has to be restored after kick */
    bool configureParamsRF;

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
    /* LBP_IB_PSK: PSK / Network authentication Key for Conformance */
    const uint8_t pskConformance[16];

    /* ADP_IB_CONTEXT_INFORMATION_TABLE: Context information table index 0 */
    const uint8_t contextInfoTable0[14];

    /* ADP_IB_CONTEXT_INFORMATION_TABLE: Context information table index 1 */
    const uint8_t contextInfoTable1[10];

    /* MAC_WRP_PIB_SHORT_ADDRESS */
    const uint16_t shortAddress;

    /* ADP_IB_ROUTING_TABLE_ENTRY_TTL */
    const uint16_t routingTableEntryTTL;

    /* ADP_IB_ROUTING_TABLE_ENTRY_TTL for Conformance */
    const uint16_t routingTableEntryTTLconformance;

    /* MAC_WRP_PIB_DUTY_CYCLE_LIMIT_RF */
    const uint16_t dutyCycleLimitRF;

    /* ADP_IB_BLACKLIST_TABLE_ENTRY_TTL for Conformance */
    const uint16_t blacklistTableEntryTTLconformance;

    /* ADP_IB_BROADCAST_LOG_TABLE_ENTRY_TTL for Conformance */
    const uint16_t broadcastLogTableEntryTTLconformance;

    /* ADP_IB_GROUP_TABLE: Group Table index 0 for Conformance */
    const uint16_t gropTable0Conformance;

    /* ADP_IB_GROUP_TABLE: Group Table index 1 for Conformance */
    const uint16_t gropTable1Conformance;

    /* ADP_IB_DESTINATION_ADDRESS_SET: Destination Address Set index 0 for
     * Conformance */
    const uint16_t destAddrSet0Conformance;

    /* ADP_IB_DELAY_LOW_LQI / ADP_IB_DELAY_LOW_LQI_RF for Conformance */
    const uint16_t delayLowLQIconformance;

    /* ADP_IB_DELAY_HIGH_LQI / ADP_IB_DELAY_HIGH_LQI_RF for Conformance */
    const uint16_t delayHighLQIconformance;

    /* ADP_IB_RREQ_JITTER_LOW_LQI_RF for Conformance */
    const uint16_t rreqJitterLowLQIRFconformance;

    /* ADP_IB_RREQ_JITTER_HIGH_LQI_RF for Conformance */
    const uint16_t rreqJitterHighLQIRFconformance;

    /* ADP_IB_CLUSTER_MIN_LQI_RF for Conformance */
    const uint16_t clusterMinLQIRFconformance;

    /* ADP_IB_CLUSTER_TRICKLE_I / ADP_IB_CLUSTER_TRICKLE_I_RF for Conformance */
    const uint16_t clusterTrickleIconformance;

    /* ADP_IB_MAX_HOPS */
    const uint8_t maxHops;

    /* ADP_IB_MAX_HOPS for Conformance */
    const uint8_t maxHopsConformance;

    /* ADP_IB_WEAK_LQI_VALUE for Conformance */
    const uint8_t weakLQIvalueConformance;

    /* ADP_IB_WEAK_LQI_VALUE_RF for Conformance */
    const uint8_t weakLQIvalueRFconformance;

    /* ADP_IB_TRICKLE_DATA_ENABLED for Conformance */
    const uint8_t trickleDataEnabledConformance;

    /* ADP_IB_TRICKLE_ADAPTIVE_KI for Conformance */
    const uint8_t trickleAdaptiveKiConformance;

    /* ADP_IB_TRICKLE_LQI_THRESHOLD_LOW_RF for Conformance */
    const uint8_t trickleLQIthresholdLowRFconformance;

    /* ADP_IB_TRICKLE_LQI_THRESHOLD_HIGH_RF for Conformance */
    const uint8_t trickleLQIthresholdHighRFconformance;

    /* ADP_IB_CLUSTER_TRICKLE_ENABLED for Conformance */
    const uint8_t clusterTrickleEnabledConformance;

    /* ADP_IB_PROBING_INTERVAL for Conformance */
    const uint8_t probingIntervalConformance;

    /* MAC_WRP_PIB_TMR_TTL for Conformance */
    const uint8_t tmrTTLconformance;

    /* MAC_WRP_PIB_MAX_CSMA_BACKOFFS for Conformance */
    const uint8_t maxCSMAbackoffsConformance;

    /* MAC_WRP_PIB_MAX_CSMA_BACKOFFS_RF for Conformance */
    const uint8_t maxCSMAbackoffsRFconformance;

    /* MAC_WRP_PIB_MAX_FRAME_RETRIES_RF for Conformance */
    const uint8_t maxFrameRetriesRFconformance;

    /* MAC_WRP_PIB_POS_TABLE_ENTRY_TTL for Conformance */
    const uint8_t posTableEntryTTLconformance;

    /* MAC_WRP_PIB_POS_RECENT_ENTRY_THRESHOLD for Conformance */
    const uint8_t posRecentEntryThresholdConformance;

    /* MAC_WRP_PIB_ADAPTIVE_POWER_LOW_BOUND_RF for Conformance */
    const uint8_t adaptivePowerLowBoundRFconformance;

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

    /* MAC_WRP_PIB_FREQUENCY_BAND_RF for Conformance */
    const uint8_t rfFreqBandConformance;

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
    void APP_G3_MANAGEMENT_SetConformanceConfig ( void )

  Summary:
    Configures ADP/MAC parameters for Conformance Test.

  Description:
    This function configures ADP/MAC IB parameters for Conformance Test. The
    parameters values can be found above in this file in Macro Definitions
    section.

  Precondition:
    APP_G3_MANAGEMENT_Initialize should be called before calling this routine.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_G3_MANAGEMENT_SetConformanceConfig();
    </code>

  Remarks:
    In this demo application this function is only called at initialization if
    conformance test is enabled at compilation time.
    The intention of this function is to be called by any condition that enables
    the conformance test at execution time.
*/

void APP_G3_MANAGEMENT_SetConformanceConfig ( void );

/*******************************************************************************
  Function:
    uint8_t APP_G3_MANAGEMENT_SetConformanceTrickleConfig(uint8_t trickleActivation)

  Summary:
    Configures the ADP Conformance trickle mode from UDP responder message.

  Description:
    This function configures the ADP Conformance trickle mode from UDP responder
    message.

  Precondition:
    APP_G3_MANAGEMENT_Initialize should be called before calling this routine.

  Parameters:
    trickleActivation - Trickle activation (0 disabled, 1 enabled).

  Returns:
    Result of configuration. 0 if successful, 1 otherwise.

  Example:
    <code>
    uint8_t result = APP_G3_MANAGEMENT_SetConformanceTrickleConfig(1);
    </code>

  Remarks:
    None.
*/

uint8_t APP_G3_MANAGEMENT_SetConformanceTrickleConfig(uint8_t trickleActivation);

/*******************************************************************************
  Function:
    uint8_t APP_G3_MANAGEMENT_SetConfigRF(uint8_t* pParameters)

  Summary:
    Configures the RF PHY layer from UDP responder message.

  Description:
    This function configures the RF PHY layer from UDP responder message.

  Precondition:
    APP_G3_MANAGEMENT_Initialize should be called before calling this routine.

  Parameters:
    pParameters - Pointer to RF configuration parameters.

  Returns:
    Result of configuration. 0 if successful, 1 if failed, 2 if unsupported.

  Example:
    <code>
    uint8_t parameters[4];

    uint8_t result = APP_G3_MANAGEMENT_SetConfigRF(parameters);
    </code>

  Remarks:
    None.
*/

uint8_t APP_G3_MANAGEMENT_SetConfigRF(uint8_t* pParameters);

/*******************************************************************************
  Function:
    void APP_G3_MANAGEMENT_SetContinuousTxRF ( void )

  Summary:
    Configures the RF PHY in continuous transmission mode.

  Description:
    This function configures the RF PHY in continuous transmission mode. A D-M1
    signal is transmitted, as defined in ETSI EN 303 204, at the center of the
    configured channel.

  Precondition:
    APP_G3_MANAGEMENT_Initialize should be called before calling this routine.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_G3_MANAGEMENT_SetContinuousTxRF();
    </code>

  Remarks:
    None.
*/

void APP_G3_MANAGEMENT_SetContinuousTxRF ( void );

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

/*******************************************************************************
  Function:
    void APP_G3_MANAGEMENT_SetLastGaspMode(void)

  Summary:
    Configures the ADP Conformance Last Gasp Mode from UDP responder message.

  Description:
    This function configures the ADP Conformance Last Gasp Mode from UDP
    responder message.

  Precondition:
    APP_G3_MANAGEMENT_Initialize should be called before calling this routine.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_G3_MANAGEMENT_SetLastGaspMode();
    </code>

  Remarks:
    None.
*/

void APP_G3_MANAGEMENT_SetLastGaspMode(void);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* _APP_G3_MANAGEMENT_H */

/*******************************************************************************
 End of File
 */

