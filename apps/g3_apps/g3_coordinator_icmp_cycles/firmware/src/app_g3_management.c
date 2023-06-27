/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_g3_management.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "definitions.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_G3_MANAGEMENT_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_G3_MANAGEMENT_DATA app_g3_managementData;

static uint16_t app_g3_managementPanIdList[APP_G3_MANAGEMENT_PAN_ID_LIST_SIZE];

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Constants
// *****************************************************************************
// *****************************************************************************

static const APP_G3_MANAGEMENT_CONSTANTS app_g3_managementConst = {
    /* G3 Application parameters */
    .contextInfoTable0 = APP_G3_MANAGEMENT_CONTEXT_INFO_TABLE_0,
    .contextInfoTable1 = APP_G3_MANAGEMENT_CONTEXT_INFO_TABLE_1,
    .shortAddress = APP_G3_MANAGEMENT_SHORT_ADDRESS,
    .routingTableEntryTTL = APP_G3_MANAGEMENT_ROUTING_TABLE_ENTRY_TTL,
    .maxJoinWaitTime = APP_G3_MANAGEMENT_MAX_JOIN_WAIT_TIME,
    .maxHops = APP_G3_MANAGEMENT_MAX_HOPS,
    .broadcastRouteAll = APP_G3_MANAGEMENT_BROADCAST_ROUTE_ALL,
    .dutyCycleLimitRF = APP_G3_MANAGEMENT_DUTY_CYCLE_LIMIT_RF

};

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

static void _ADP_NetworkStartConfirm(uint8_t status)
{
    if (status == G3_SUCCESS)
    {
        IPV6_ADDR networkPrefix;
        uint8_t prefixData[27];
        ADP_SET_CFM_PARAMS setConfirm;
        ADP_GET_CFM_PARAMS getConfirm;

        /* Configure Network Prefix in ADP */
        TCPIP_Helper_StringToIPv6Address(APP_TCPIP_IPV6_NETWORK_PREFIX_G3, &networkPrefix);
        networkPrefix.v[6] = (uint8_t) (app_g3_managementData.panId >> 8);
        networkPrefix.v[7] = (uint8_t) app_g3_managementData.panId;
        prefixData[0] = APP_TCPIP_IPV6_NETWORK_PREFIX_G3_LEN;
        prefixData[1] = 1; // OnLink flag
        prefixData[2] = 1; // AutonomuosConfiguration flag
        *((uint32_t*) &prefixData[3]) = 0x7FFFFFFF; // valid lifetime
        *((uint32_t*) &prefixData[7]) = 0x7FFFFFFF; // preferred lifetime
        memcpy(&prefixData[11], &networkPrefix, 16);
        ADP_SetRequestSync(ADP_IB_PREFIX_TABLE, 0, 27, (const uint8_t*) prefixData, &setConfirm);

        /* Update Context Information Table index 0. Only update PAN ID. */
        ADP_GetRequestSync(ADP_IB_CONTEXT_INFORMATION_TABLE, 0, &getConfirm);
        if (getConfirm.status == G3_SUCCESS)
        {
            getConfirm.attributeValue[12] = (uint8_t) (app_g3_managementData.panId >> 8);
            getConfirm.attributeValue[13] = (uint8_t) app_g3_managementData.panId;
            ADP_SetRequestSync(ADP_IB_CONTEXT_INFORMATION_TABLE, 0, getConfirm.attributeLength,
                (const uint8_t*) getConfirm.attributeValue, &setConfirm);
        }

        /* Network started */
        app_g3_managementData.state = APP_G3_MANAGEMENT_STATE_NETWORK_STARTED;
    }
    else
    {
        /* Error in Network Start. Try with another PAN ID */
        app_g3_managementData.panId += 1;
        ADP_NetworkStartRequest(app_g3_managementData.panId);
        SYS_DEBUG_MESSAGE(SYS_ERROR_ERROR, "APP_G3_MANAGEMENT: Error in ADP Network Start\r\n");
    }
}

static void _ADP_DiscoveryIndication(ADP_PAN_DESCRIPTOR* pPanDescriptor)
{
    if (app_g3_managementData.numNetworksFound < APP_G3_MANAGEMENT_PAN_ID_LIST_SIZE)
    {
        /* Store PAN ID in order to not repeat it */
        app_g3_managementPanIdList[app_g3_managementData.numNetworksFound++] = pPanDescriptor->panId;
    }
}

static void _ADP_DiscoveryConfirm(uint8_t status)
{
    ADP_SET_CFM_PARAMS setConfirm;
    bool checkAllPanId = true;

    while (checkAllPanId == true)
    {
        /* Check if there is already a network with the same PAN ID */
        checkAllPanId = false;
        for (uint8_t i = 0; i < app_g3_managementData.numNetworksFound; i++)
        {
            if (app_g3_managementPanIdList[i] == app_g3_managementData.panId)
            {
                /* Change PAN ID */
                app_g3_managementData.panId += 1;
                checkAllPanId = true;
                break;
            }
        }
    }

    /* Set short address for coordinator (0x0000) */
    ADP_MacSetRequestSync(MAC_WRP_PIB_SHORT_ADDRESS, 0, 2,
            (const uint8_t*) &app_g3_managementConst.shortAddress, &setConfirm);

    /* Start G3 network */
    app_g3_managementData.state = APP_G3_MANAGEMENT_STATE_STARTING_NETWORK;
    ADP_NetworkStartRequest(app_g3_managementData.panId);
}

static void _ADP_NonVolatileDataIndication(ADP_NON_VOLATILE_DATA_IND_PARAMS* pNonVolatileData)
{
    /* Send new non-volatile data to storage application */
    APP_STORAGE_UpdateNonVolatileData(pNonVolatileData);
}

static void _ADP_BufferIndication(ADP_BUFFER_IND_PARAMS* bufferInd)
{
    /* Notify cycles application */
    APP_CYCLES_AdpBufferIndication(bufferInd);
}

void APP_SYS_TIME_CallbackSetFlag(uintptr_t context)
{
    if (context != 0)
    {
        /* Context holds the flag's address */
        *((bool *) context) = true;
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

static void _APP_G3_MANAGEMENT_InitializeParameters(void)
{
    ADP_SET_CFM_PARAMS setConfirm;
    uint16_t multicastGroup;

    /* Set extended address (EUI64). It must be unique for each device. */
    ADP_MacSetRequestSync(MAC_WRP_PIB_MANUF_EXTENDED_ADDRESS, 0, 8,
            (const uint8_t*) app_g3_managementData.eui64.value, &setConfirm);

    /* Set user-specific ADP parameters */
    ADP_SetRequestSync(ADP_IB_CONTEXT_INFORMATION_TABLE, 0, 14,
            app_g3_managementConst.contextInfoTable0, &setConfirm);

    ADP_SetRequestSync(ADP_IB_CONTEXT_INFORMATION_TABLE, 1, 10,
            app_g3_managementConst.contextInfoTable1, &setConfirm);

    ADP_SetRequestSync(ADP_IB_MANUF_BROADCAST_ROUTE_ALL, 0, 1,
            &app_g3_managementConst.broadcastRouteAll, &setConfirm);

    ADP_SetRequestSync(ADP_IB_ROUTING_TABLE_ENTRY_TTL, 0, 2,
            (const uint8_t*) &app_g3_managementConst.routingTableEntryTTL, &setConfirm);

    ADP_SetRequestSync(ADP_IB_MAX_JOIN_WAIT_TIME, 0, 2,
            (const uint8_t*) &app_g3_managementConst.maxJoinWaitTime, &setConfirm);

    ADP_SetRequestSync(ADP_IB_MAX_HOPS, 0, 1, &app_g3_managementConst.maxHops, &setConfirm);

    /* Add short address to multi-cast group table in order to receive NDP
     * Neighbor Solicitation messages */
    multicastGroup = (uint8_t) app_g3_managementConst.shortAddress;
    multicastGroup |= (uint8_t) ((app_g3_managementConst.shortAddress >> 8) & 0x1F);
    multicastGroup |= 0x8000;
    ADP_SetRequestSync(ADP_IB_GROUP_TABLE, 0, 2, (const uint8_t*) &multicastGroup, &setConfirm);

    /* Set user-specific MAC parameters */
    ADP_MacSetRequestSync(MAC_WRP_PIB_DUTY_CYCLE_LIMIT_RF, 0, 2,
            (const uint8_t*) &app_g3_managementConst.dutyCycleLimitRF, &setConfirm);

    if (app_g3_managementData.writeNonVolatileData == true)
    {
        ADP_NON_VOLATILE_DATA_IND_PARAMS* pNonVolatileData = APP_STORAGE_GetNonVolatileData();

        if (pNonVolatileData != NULL)
        {
            /* Set ADP/MAC non-volatile data parameters */
            ADP_MacSetRequestSync(MAC_WRP_PIB_FRAME_COUNTER, 0, 4,
                    (const uint8_t*) &pNonVolatileData->frameCounter, &setConfirm);

            ADP_MacSetRequestSync(MAC_WRP_PIB_FRAME_COUNTER_RF, 0, 4,
                    (const uint8_t*) &pNonVolatileData->frameCounterRF, &setConfirm);

            ADP_SetRequestSync(ADP_IB_MANUF_DISCOVER_SEQUENCE_NUMBER, 0, 2,
                    (const uint8_t*) &pNonVolatileData->discoverSeqNumber, &setConfirm);

            ADP_SetRequestSync(ADP_IB_MANUF_BROADCAST_SEQUENCE_NUMBER, 0, 1,
                    (const uint8_t*) &pNonVolatileData->broadcastSeqNumber, &setConfirm);
        }

        /* Not needed to set ADP/MAC non-volatile data parameters anymore
         * because they are internally restored when ADP reset is performed */
        app_g3_managementData.writeNonVolatileData = false;
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_G3_MANAGEMENT_Initialize ( void )

  Remarks:
    See prototype in app_g3_management.h.
 */

void APP_G3_MANAGEMENT_Initialize ( void )
{
    ADP_MANAGEMENT_NOTIFICATIONS adpMngNotifications;

    /* Set ADP management call-backs */
    adpMngNotifications.discoveryConfirm = _ADP_DiscoveryConfirm;
    adpMngNotifications.discoveryIndication = _ADP_DiscoveryIndication;
    adpMngNotifications.networkStartConfirm = _ADP_NetworkStartConfirm;
    adpMngNotifications.resetConfirm = NULL;
    adpMngNotifications.setConfirm = NULL;
    adpMngNotifications.getConfirm = NULL;
    adpMngNotifications.macSetConfirm = NULL;
    adpMngNotifications.getConfirm = NULL;
    adpMngNotifications.macGetConfirm = NULL;
    adpMngNotifications.routeDiscoveryConfirm = NULL;
    adpMngNotifications.pathDiscoveryConfirm = NULL;
    adpMngNotifications.networkStatusIndication = NULL;
    adpMngNotifications.preqIndication = NULL;
    adpMngNotifications.nonVolatileDataIndication = _ADP_NonVolatileDataIndication;
    adpMngNotifications.routeNotFoundIndication = NULL;
    adpMngNotifications.bufferIndication = _ADP_BufferIndication;
    ADP_SetManagementNotifications(&adpMngNotifications);

    /* Place the application's state machine in its initial state. */
    app_g3_managementData.state = APP_G3_MANAGEMENT_STATE_ADP_OPEN;

    /* Initialize application variables */
    app_g3_managementData.timerLedHandle = SYS_TIME_HANDLE_INVALID;
    app_g3_managementData.panId = APP_G3_MANAGEMENT_PAN_ID;
    app_g3_managementData.timerLedExpired = false;
    app_g3_managementData.writeNonVolatileData = true;

    SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, APP_G3_MANAGEMENT_STRING_HEADER);
}


/******************************************************************************
  Function:
    void APP_G3_MANAGEMENT_Tasks ( void )

  Remarks:
    See prototype in app_g3_management.h.
 */

void APP_G3_MANAGEMENT_Tasks ( void )
{
    /* Refresh Watchdog */
    CLEAR_WATCHDOG();

    /* Signaling: LED Toggle */
    if (app_g3_managementData.timerLedExpired == true)
    {
        app_g3_managementData.timerLedExpired = false;
        USER_BLINK_LED_Toggle();
    }

    /* Check the application's current state. */
    switch ( app_g3_managementData.state )
    {
        /* Application's initial state. */
        case APP_G3_MANAGEMENT_STATE_ADP_OPEN:
        {
            SRV_PLC_PCOUP_BRANCH plcBranch;
            ADP_PLC_BAND plcBand;

            /* Get configured PLC band */
            plcBranch = SRV_PCOUP_Get_Default_Branch();
            plcBand = SRV_PCOUP_Get_Phy_Band(plcBranch);

            /* Open G3 Adaptation Layer (ADP) */
            ADP_Open(plcBand);

            /* Get Extended Address from storage application */
            APP_STORAGE_GetExtendedAddress(app_g3_managementData.eui64.value);

            /* Next state: Wait for ADP to be ready.
             * Don't break, go directly to next state */
            app_g3_managementData.state = APP_G3_MANAGEMENT_STATE_WAIT_ADP_READY;
        }

        /* State to wait for ADP to be ready */
        case APP_G3_MANAGEMENT_STATE_WAIT_ADP_READY:
        {
            /* Check ADP status */
            ADP_STATUS adpStatus = ADP_Status();
            if (adpStatus >= ADP_STATUS_READY)
            {
                /* ADP is ready. We can set ADP/MAC parameters. */
                _APP_G3_MANAGEMENT_InitializeParameters();

                /* Look for G3 networks in order to not repeat PAN ID */
                ADP_DiscoveryRequest(15);
                app_g3_managementData.numNetworksFound = 0;
                app_g3_managementData.state = APP_G3_MANAGEMENT_STATE_SCANNING;

                if (app_g3_managementData.timerLedHandle == SYS_TIME_HANDLE_INVALID)
                {
                    /* Register timer callback to blink LED */
                    app_g3_managementData.timerLedHandle = SYS_TIME_CallbackRegisterMS(
                            APP_SYS_TIME_CallbackSetFlag,
                            (uintptr_t) &app_g3_managementData.timerLedExpired,
                            APP_G3_MANAGEMENT_LED_BLINK_PERIOD_MS, SYS_TIME_PERIODIC);
                }

                SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "APP_G3_MANAGEMENT: ADP initialized successfully\r\n");
            }
            else
            {
                if (adpStatus == ADP_STATUS_ERROR)
                {
                    SYS_DEBUG_MESSAGE(SYS_ERROR_ERROR, "APP_G3_MANAGEMENT: Error in ADP initialization\r\n");
                    app_g3_managementData.state = APP_G3_MANAGEMENT_STATE_ERROR;
                }
            }

            break;
        }

        /* Network discovery (scan) in progress */
        case APP_G3_MANAGEMENT_STATE_SCANNING:
        {
            /* Nothing to do, state will be changed from _ADP_DiscoveryConfirm
             * callback */
            break;
        }

        /* Starting G3 network */
        case APP_G3_MANAGEMENT_STATE_STARTING_NETWORK:
        {
            /* Nothing to do, state will be changed from
             * _ADP_NetworkStartConfirm callback */
            break;
        }

        /* G3 network started */
        case APP_G3_MANAGEMENT_STATE_NETWORK_STARTED:
        {
            /* Nothing to do */
            break;
        }

        /* Error state */
        case APP_G3_MANAGEMENT_STATE_ERROR:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }

        /* The default state should never be executed. */
        default:
        {
            break;
        }
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Interface Functions
// *****************************************************************************
// *****************************************************************************

uint16_t APP_G3_MANAGEMENT_GetPanId(void)
{
    return app_g3_managementData.panId;
}

uint8_t* APP_G3_MANAGEMENT_GetExtendedAddress(void)
{
    return app_g3_managementData.eui64.value;
}

/*******************************************************************************
 End of File
 */
