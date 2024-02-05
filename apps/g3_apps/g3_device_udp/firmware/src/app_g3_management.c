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
 ******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "definitions.h"
#include "service/random/srv_random.h"

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
    This structure should be initialized by the APP_G3_MANAGEMENT_Initialize
    function.

    Application strings and buffers are be defined outside this structure.
*/

APP_G3_MANAGEMENT_DATA app_g3_managementData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Constants
// *****************************************************************************
// *****************************************************************************

static const APP_G3_MANAGEMENT_CONSTANTS app_g3_managementConst = {
    /* G3 Application parameters */
    .psk = APP_G3_MANAGEMENT_PSK_KEY,
    .contextInfoTable0 = APP_G3_MANAGEMENT_CONTEXT_INFO_TABLE_0,
    .contextInfoTable1 = APP_G3_MANAGEMENT_CONTEXT_INFO_TABLE_1,
    .routingTableEntryTTL = APP_G3_MANAGEMENT_ROUTING_TABLE_ENTRY_TTL,
    .maxJoinWaitTime = APP_G3_MANAGEMENT_MAX_JOIN_WAIT_TIME,
    .maxHops = APP_G3_MANAGEMENT_MAX_HOPS,
    .dutyCycleLimitRF = APP_G3_MANAGEMENT_DUTY_CYCLE_LIMIT_RF,
    .defaultCoordRouteEnabled = APP_G3_MANAGEMENT_DEFAULT_COORD_ROUTE_ENABLED,

    /* G3 Conformance parameters */
    .pskConformance = APP_G3_MANAGEMENT_PSK_KEY_CONFORMANCE,
    .blacklistTableEntryTTLconformance = APP_G3_MANAGEMENT_BLACKLIST_TABLE_ENTRY_TTL_CONFORMANCE,
    .broadcastLogTableEntryTTLconformance = APP_G3_MANAGEMENT_BROADCAST_LOG_TABLE_ENTRY_TTL_CONFORMANCE,
    .gropTable0Conformance = APP_G3_MANAGEMENT_GROUP_TABLE_0_CONFORMANCE,
    .gropTable1Conformance = APP_G3_MANAGEMENT_GROUP_TABLE_1_CONFORMANCE,
    .routingTableEntryTTLconformance = APP_G3_MANAGEMENT_ROUTING_TABLE_ENTRY_TTL_CONFORMANCE,
    .maxJoinWaitTimeConformance = APP_G3_MANAGEMENT_MAX_JOIN_WAIT_TIME_CONFORMANCE,
    .destAddrSet0Conformance = APP_G3_MANAGEMENT_DEST_ADDR_SET_0_CONFORMANCE,
    .delayLowLQIconformance = APP_G3_MANAGEMENT_DELAY_LOW_LQI_CONFORMANCE,
    .delayHighLQIconformance = APP_G3_MANAGEMENT_DELAY_HIGH_LQI_CONFORMANCE,
    .rreqJitterLowLQIRFconformance = APP_G3_MANAGEMENT_JITTER_LOW_LQI_RF_CONFORMANCE,
    .rreqJitterHighLQIRFconformance = APP_G3_MANAGEMENT_JITTER_HIGH_LQI_RF_CONFORMANCE,
    .clusterMinLQIRFconformance = APP_G3_MANAGEMENT_CLUSTER_MIN_LQI_RF_CONFORMANCE,
    .clusterTrickleIconformance = APP_G3_MANAGEMENT_CLUSTER_TRICKLE_I_CONFORMANCE,
    .maxHopsConformance = APP_G3_MANAGEMENT_MAX_HOPS_CONFORMANCE,
    .weakLQIvalueConformance = APP_G3_MANAGEMENT_WEAK_LQI_CONFORMANCE,
    .weakLQIvalueRFconformance = APP_G3_MANAGEMENT_WEAK_LQI_RF_CONFORMANCE,
    .trickleDataEnabledConformance = APP_G3_MANAGEMENT_TRICKLE_DATA_ENABLED_CONFORMANCE,
    .trickleAdaptiveKiConformance = APP_G3_MANAGEMENT_ADAPTIVE_POWER_LOW_BOUND_CONFORMANCE,
    .trickleLQIthresholdLowRFconformance = APP_G3_MANAGEMENT_TRICKLE_LQI_THRESHOLD_LOW_RF_CONFORMANCE,
    .trickleLQIthresholdHighRFconformance = APP_G3_MANAGEMENT_TRICKLE_LQI_THRESHOLD_HIGH_RF_CONFORMANCE,
    .clusterTrickleEnabledConformance = APP_G3_MANAGEMENT_CLUSTER_TRICKLE_ENABLED_CONFORMANCE,
    .probingIntervalConformance = APP_G3_MANAGEMENT_PROBING_INTERVAL_CONFORMANCE,
    .tmrTTLconformance = APP_G3_MANAGEMENT_TMR_TTL_CONFORMANCE,
    .maxCSMAbackoffsConformance = APP_G3_MANAGEMENT_MAX_CSMA_BACKOFFS_CONFORMANCE,
    .maxCSMAbackoffsRFconformance = APP_G3_MANAGEMENT_MAX_CSMA_BACKOFFS_RF_CONFORMANCE,
    .maxFrameRetriesRFconformance = APP_G3_MANAGEMENT_MAX_FRAME_RETRIES_RF_CONFORMANCE,
    .posTableEntryTTLconformance = APP_G3_MANAGEMENT_POS_TABLE_TTL_CONFORMANCE,
    .posRecentEntryThresholdConformance = APP_G3_MANAGEMENT_POS_RECENT_THRESHOLD_CONFORMANCE,
    .adaptivePowerLowBoundRFconformance = APP_G3_MANAGEMENT_ADAPTIVE_POWER_LOW_BOUND_CONFORMANCE,
    .krConformance = APP_G3_MANAGEMENT_KRT_CONFORMANCE,
    .kmConformance = APP_G3_MANAGEMENT_KM_CONFORMANCE,
    .kcConformance = APP_G3_MANAGEMENT_KC_CONFORMANCE,
    .kqConformance = APP_G3_MANAGEMENT_KQ_CONFORMANCE,
    .khConformance = APP_G3_MANAGEMENT_KH_CONFORMANCE,
    .krtConformance = APP_G3_MANAGEMENT_KRT_CONFORMANCE,
    .kqRFconformance = APP_G3_MANAGEMENT_KQ_RF_CONFORMANCE,
    .khRFconformance = APP_G3_MANAGEMENT_KH_RF_CONFORMANCE,
    .krtConformance = APP_G3_MANAGEMENT_KRT_RF_CONFORMANCE,
    .kdcRFconformance = APP_G3_MANAGEMENT_KDC_RF_CONFORMANCE,
    .rfFreqBandConformance = APP_G3_MANAGEMENT_RF_FREQ_BAND_CONFORMANCE

};

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

static void _ADP_DiscoveryConfirm(uint8_t status)
{
    if ((status == G3_SUCCESS) && (app_g3_managementData.bestNetwork.panId != 0xFFFF) &&
            (app_g3_managementData.bestNetwork.lbaAddress != 0xFFFF))
    {
        /* Good network found. Start back-off before join to that network */
        app_g3_managementData.state = APP_G3_MANAGEMENT_STATE_START_BACKOFF_JOIN;
        app_g3_managementData.joinRetries = 0;

        /* Initialize back-off window for joining */
        app_g3_managementData.backoffWindowLow = APP_G3_MANAGEMENT_JOIN_BACKOFF_LOW_MIN;
        app_g3_managementData.backoffWindowHigh = APP_G3_MANAGEMENT_JOIN_BACKOFF_HIGH_MIN;

        SYS_DEBUG_PRINT(SYS_ERROR_INFO, "APP_G3_MANAGEMENT: Good network found. PAN ID: 0x%04X, "
                "LBA: 0x%04X, Route Cost Coordinator: %hu, Link Quality: %hhu, Media Type: %hhu\r\n",
                app_g3_managementData.bestNetwork.panId, app_g3_managementData.bestNetwork.lbaAddress,
                app_g3_managementData.bestNetwork.rcCoord, app_g3_managementData.bestNetwork.linkQuality,
                app_g3_managementData.bestNetwork.mediaType);
    }
    else
    {
        /* No network found. Go back to back-off before network discovery */
        app_g3_managementData.state = APP_G3_MANAGEMENT_STATE_START_BACKOFF_DISCOVERY;

        /* Initialize back-off window for network discovery */
        app_g3_managementData.backoffWindowLow = APP_G3_MANAGEMENT_DISCOVERY_BACKOFF_LOW_MIN;
        app_g3_managementData.backoffWindowHigh = APP_G3_MANAGEMENT_DISCOVERY_BACKOFF_HIGH_MIN;

        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "APP_G3_MANAGEMENT: No good network found in discovery\r\n");
    }
}

static void _ADP_DiscoveryIndication(ADP_PAN_DESCRIPTOR* pPanDescriptor)
{
    uint8_t minLQI = APP_G3_MANAGEMENT_LQI_MIN_PLC;

    /* Select minimum LQI depending on Media Type */
    if (pPanDescriptor->mediaType != MAC_WRP_MEDIA_TYPE_IND_PLC)
    {
        minLQI = APP_G3_MANAGEMENT_LQI_MIN_RF;
    }

    /* Check minimum Link Quality and maximum route cost to Coordinator */
    if ((pPanDescriptor->linkQuality >= minLQI) &&
            (pPanDescriptor->rcCoord < APP_G3_MANAGEMENT_ROUTE_COST_COORD_MAX))
    {
        /* Update best network if route cost to Coordinator is better or if it
         * is equal and Link Quality is better */
        if ((pPanDescriptor->rcCoord < app_g3_managementData.bestNetwork.rcCoord) ||
                ((pPanDescriptor->rcCoord == app_g3_managementData.bestNetwork.rcCoord) &&
                (pPanDescriptor->linkQuality > app_g3_managementData.bestNetwork.linkQuality)))
        {
            app_g3_managementData.bestNetwork = *pPanDescriptor;
        }
    }

    SYS_DEBUG_PRINT(SYS_ERROR_DEBUG, "APP_G3_MANAGEMENT: Network discovered. PAN ID: 0x%04X, LBA: 0x%04X, "
            "Route Cost Coordinator: %hu, Link Quality: %hhu, Media Type: %hhu\r\n",
            pPanDescriptor->panId, pPanDescriptor->lbaAddress, pPanDescriptor->rcCoord,
            pPanDescriptor->linkQuality, pPanDescriptor->mediaType);
}

static void _ADP_NonVolatileDataIndication(ADP_NON_VOLATILE_DATA_IND_PARAMS* pNonVolatileData)
{
    /* Send new non-volatile data to storage application */
    APP_STORAGE_UpdateNonVolatileData(pNonVolatileData);
}

static void _LBP_ADP_NetworkJoinConfirm(LBP_ADP_NETWORK_JOIN_CFM_PARAMS* pNetworkJoinCfm)
{
    if (pNetworkJoinCfm->status == G3_SUCCESS)
    {
        ADP_GET_CFM_PARAMS getConfirm;
        ADP_SET_CFM_PARAMS setConfirm;
        IPV6_ADDR networkPrefix;
        uint16_t shortAddress, panId;
        uint8_t prefixData[27];

        /* Successful join */
        app_g3_managementData.state = APP_G3_MANAGEMENT_STATE_JOINED;
        shortAddress = pNetworkJoinCfm->networkAddress;
        panId = pNetworkJoinCfm->panId;

        /* Update Context Information Table index 0. Only update PAN ID. */
        ADP_GetRequestSync(ADP_IB_CONTEXT_INFORMATION_TABLE, 0, &getConfirm);
        if (getConfirm.status == G3_SUCCESS)
        {
            getConfirm.attributeValue[12] = (uint8_t) (panId >> 8);
            getConfirm.attributeValue[13] = (uint8_t) panId;
            ADP_SetRequestSync(ADP_IB_CONTEXT_INFORMATION_TABLE, 0, getConfirm.attributeLength,
                (const uint8_t*) getConfirm.attributeValue, &setConfirm);
        }

        /* Start a route request to coordinator */
        ADP_GetRequestSync(ADP_IB_COORD_SHORT_ADDRESS, 0, &getConfirm);
        if (getConfirm.status == G3_SUCCESS)
        {

            uint16_t coordShortAddress = *((uint16_t*) getConfirm.attributeValue);
            uint8_t maxHops = APP_G3_MANAGEMENT_MAX_HOPS;

            if (app_g3_managementData.conformanceTest == true)
            {
                maxHops = APP_G3_MANAGEMENT_MAX_HOPS_CONFORMANCE;
            }

            ADP_RouteDiscoveryRequest(coordShortAddress, maxHops);
        }

        /* Notify UDP responder application to configure IPv6 addresses */
        app_g3_managementData.shortAddress = shortAddress;
        app_g3_managementData.panId = panId;
        APP_TCPIP_MANAGEMENT_NetworkJoined();

        /* Configure Network Prefix in ADP */
        TCPIP_Helper_StringToIPv6Address(APP_TCPIP_MANAGEMENT_IPV6_NETWORK_PREFIX_G3, &networkPrefix);
        networkPrefix.v[6] = (uint8_t) (panId >> 8);
        networkPrefix.v[7] = (uint8_t) panId;
        prefixData[0] = APP_TCPIP_MANAGEMENT_IPV6_NETWORK_PREFIX_G3_LEN;
        prefixData[1] = 1; // OnLink flag
        prefixData[2] = 1; // AutonomuosConfiguration flag
        *((uint32_t*) &prefixData[3]) = 0x7FFFFFFF; // valid lifetime
        *((uint32_t*) &prefixData[7]) = 0x7FFFFFFF; // preferred lifetime
        memcpy(&prefixData[11], &networkPrefix, 16);
        ADP_SetRequestSync(ADP_IB_PREFIX_TABLE, 0, 27, (const uint8_t*) prefixData, &setConfirm);

        SYS_DEBUG_PRINT(SYS_ERROR_INFO, "APP_G3_MANAGEMENT: Joined to the network. "
                "PAN ID: 0x%04X, Short Address: 0x%04X\r\n", panId, shortAddress);
    }
    else
    {
        /* Unsuccessful join. Try at maximum 3 times. */
        if (++app_g3_managementData.joinRetries > 2)
        {
            /* Try another time */
            app_g3_managementData.state = APP_G3_MANAGEMENT_STATE_START_BACKOFF_JOIN;
            app_g3_managementData.backoffWindowLow = APP_G3_MANAGEMENT_JOIN_BACKOFF_LOW_MIN;
            app_g3_managementData.backoffWindowHigh = APP_G3_MANAGEMENT_JOIN_BACKOFF_HIGH_MIN;

            SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, "APP_G3_MANAGEMENT: Failed to join, retry\r\n");
        }
        else
        {
            /* Maximum join retries reached. Go to back-off before start
             * network discovery. */
            app_g3_managementData.state = APP_G3_MANAGEMENT_STATE_START_BACKOFF_DISCOVERY;
            app_g3_managementData.backoffWindowLow = APP_G3_MANAGEMENT_DISCOVERY_BACKOFF_LOW_MIN;
            app_g3_managementData.backoffWindowHigh = APP_G3_MANAGEMENT_DISCOVERY_BACKOFF_HIGH_MIN;

            SYS_DEBUG_MESSAGE(SYS_ERROR_WARNING, "APP_G3_MANAGEMENT: Failed to join after last retry\r\n");
        }
    }
}

static void _LBP_ADP_NetworkLeaveIndication(void)
{
    /* The device left the network. ADP is reseted internally. Go to first
     * state. Notify UDP responder application to remove IPv6 addresses */
    app_g3_managementData.state = APP_G3_MANAGEMENT_STATE_WAIT_ADP_READY;
    APP_TCPIP_MANAGEMENT_NetworkDisconnected();

    SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "APP_G3_MANAGEMENT: Kicked from network\r\n");
}

static void _APP_G3_MANAGEMENT_TimeExpiredSetFlag(uintptr_t context)
{
    /* Context holds the flag's address */
    *((bool *) context) = true;
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

static void _APP_G3_MANAGEMENT_SetConformanceParameters(void)
{
    ADP_SET_CFM_PARAMS setConfirm;

    /* Set ADP parameters needed for Conformance Test */
    ADP_SetRequestSync(ADP_IB_BLACKLIST_TABLE_ENTRY_TTL, 0, 2,
            (const uint8_t*) &app_g3_managementConst.blacklistTableEntryTTLconformance,
            &setConfirm);

    ADP_SetRequestSync(ADP_IB_BROADCAST_LOG_TABLE_ENTRY_TTL, 0, 2,
            (const uint8_t*) &app_g3_managementConst.broadcastLogTableEntryTTLconformance,
            &setConfirm);

    ADP_SetRequestSync(ADP_IB_GROUP_TABLE, 0, 2,
            (const uint8_t*) &app_g3_managementConst.gropTable0Conformance,
            &setConfirm);

    ADP_SetRequestSync(ADP_IB_GROUP_TABLE, 1, 2,
            (const uint8_t*) &app_g3_managementConst.gropTable1Conformance,
            &setConfirm);

    ADP_SetRequestSync(ADP_IB_ROUTING_TABLE_ENTRY_TTL, 0, 2,
            (const uint8_t*) &app_g3_managementConst.routingTableEntryTTLconformance,
            &setConfirm);

    ADP_SetRequestSync(ADP_IB_MAX_JOIN_WAIT_TIME, 0, 2,
            (const uint8_t*) &app_g3_managementConst.maxJoinWaitTimeConformance,
            &setConfirm);

    ADP_SetRequestSync(ADP_IB_DESTINATION_ADDRESS_SET, 0, 2,
            (const uint8_t*) &app_g3_managementConst.destAddrSet0Conformance,
            &setConfirm);

    ADP_SetRequestSync(ADP_IB_DELAY_LOW_LQI_RF, 0, 2,
            (const uint8_t*) &app_g3_managementConst.delayLowLQIconformance,
            &setConfirm);

    ADP_SetRequestSync(ADP_IB_DELAY_HIGH_LQI_RF, 0, 2,
            (const uint8_t*) &app_g3_managementConst.delayHighLQIconformance,
            &setConfirm);

    ADP_SetRequestSync(ADP_IB_RREQ_JITTER_LOW_LQI_RF, 0, 1,
            (const uint8_t*) &app_g3_managementConst.rreqJitterLowLQIRFconformance,
            &setConfirm);

    ADP_SetRequestSync(ADP_IB_RREQ_JITTER_HIGH_LQI_RF, 0, 1,
            (const uint8_t*) &app_g3_managementConst.rreqJitterHighLQIRFconformance,
            &setConfirm);

    ADP_SetRequestSync(ADP_IB_CLUSTER_MIN_LQI_RF, 0, 1,
            (const uint8_t*) &app_g3_managementConst.clusterMinLQIRFconformance,
            &setConfirm);

    ADP_SetRequestSync(ADP_IB_CLUSTER_TRICKLE_I_RF, 0, 2,
            (const uint8_t*) &app_g3_managementConst.clusterTrickleIconformance,
            &setConfirm);

    ADP_SetRequestSync(ADP_IB_DELAY_LOW_LQI, 0, 2,
            (const uint8_t*) &app_g3_managementConst.delayLowLQIconformance,
            &setConfirm);

    ADP_SetRequestSync(ADP_IB_DELAY_HIGH_LQI, 0, 2,
            (const uint8_t*) &app_g3_managementConst.delayHighLQIconformance,
            &setConfirm);

    ADP_SetRequestSync(ADP_IB_CLUSTER_TRICKLE_I, 0, 2,
            (const uint8_t*) &app_g3_managementConst.clusterTrickleIconformance,
            &setConfirm);

    ADP_SetRequestSync(ADP_IB_MAX_HOPS, 0, 1,
            &app_g3_managementConst.maxHopsConformance, &setConfirm);

    ADP_SetRequestSync(ADP_IB_WEAK_LQI_VALUE, 0, 1,
            &app_g3_managementConst.weakLQIvalueConformance, &setConfirm);

    ADP_SetRequestSync(ADP_IB_WEAK_LQI_VALUE_RF, 0, 1,
            &app_g3_managementConst.weakLQIvalueRFconformance, &setConfirm);

    ADP_SetRequestSync(ADP_IB_TRICKLE_DATA_ENABLED, 0, 1,
            &app_g3_managementConst.trickleDataEnabledConformance, &setConfirm);

    ADP_SetRequestSync(ADP_IB_TRICKLE_ADAPTIVE_KI, 0, 1,
            &app_g3_managementConst.adaptivePowerLowBoundRFconformance, &setConfirm);

    ADP_SetRequestSync(ADP_IB_TRICKLE_LQI_THRESHOLD_LOW_RF, 0, 1,
            &app_g3_managementConst.trickleLQIthresholdLowRFconformance, &setConfirm);

    ADP_SetRequestSync(ADP_IB_TRICKLE_LQI_THRESHOLD_HIGH_RF, 0, 1,
            &app_g3_managementConst.trickleLQIthresholdHighRFconformance, &setConfirm);

    ADP_SetRequestSync(ADP_IB_CLUSTER_TRICKLE_ENABLED, 0, 1,
            &app_g3_managementConst.clusterTrickleEnabledConformance, &setConfirm);

    ADP_SetRequestSync(ADP_IB_PROBING_INTERVAL, 0, 1,
            &app_g3_managementConst.probingIntervalConformance, &setConfirm);

    ADP_SetRequestSync(ADP_IB_KR, 0, 1,
            &app_g3_managementConst.krConformance, &setConfirm);

    ADP_SetRequestSync(ADP_IB_KM, 0, 1,
            &app_g3_managementConst.kmConformance, &setConfirm);

    ADP_SetRequestSync(ADP_IB_KC, 0, 1,
            &app_g3_managementConst.kcConformance, &setConfirm);

    ADP_SetRequestSync(ADP_IB_KQ, 0, 1,
            &app_g3_managementConst.kqConformance, &setConfirm);

    ADP_SetRequestSync(ADP_IB_KH, 0, 1,
            &app_g3_managementConst.khConformance, &setConfirm);

    ADP_SetRequestSync(ADP_IB_KRT, 0, 1,
            &app_g3_managementConst.krtConformance, &setConfirm);

    ADP_SetRequestSync(ADP_IB_KQ_RF, 0, 1,
            &app_g3_managementConst.kqRFconformance, &setConfirm);

    ADP_SetRequestSync(ADP_IB_KH_RF, 0, 1,
            &app_g3_managementConst.khRFconformance, &setConfirm);

    ADP_SetRequestSync(ADP_IB_KRT_RF, 0, 1,
            &app_g3_managementConst.krtRFconformance, &setConfirm);

    ADP_SetRequestSync(ADP_IB_KDC_RF, 0, 1,
            &app_g3_managementConst.kdcRFconformance, &setConfirm);

    /* Set MAC parameters needed for Conformance Test */
    ADP_MacSetRequestSync(MAC_WRP_PIB_FREQUENCY_BAND_RF, 0, 1,
            &app_g3_managementConst.rfFreqBandConformance, &setConfirm);

    ADP_MacSetRequestSync(MAC_WRP_PIB_TMR_TTL, 0, 1,
            &app_g3_managementConst.tmrTTLconformance, &setConfirm);

    ADP_MacSetRequestSync(MAC_WRP_PIB_MAX_CSMA_BACKOFFS, 0, 1,
            &app_g3_managementConst.maxCSMAbackoffsConformance, &setConfirm);

    ADP_MacSetRequestSync(MAC_WRP_PIB_MAX_CSMA_BACKOFFS_RF, 0, 1,
            &app_g3_managementConst.maxCSMAbackoffsRFconformance, &setConfirm);

    ADP_MacSetRequestSync(MAC_WRP_PIB_MAX_FRAME_RETRIES_RF, 0, 1,
            &app_g3_managementConst.maxFrameRetriesRFconformance, &setConfirm);

    ADP_MacSetRequestSync(MAC_WRP_PIB_POS_TABLE_ENTRY_TTL, 0, 1,
            &app_g3_managementConst.posTableEntryTTLconformance, &setConfirm);

    ADP_MacSetRequestSync(MAC_WRP_PIB_POS_RECENT_ENTRY_THRESHOLD, 0, 1,
            &app_g3_managementConst.posRecentEntryThresholdConformance, &setConfirm);

    ADP_MacSetRequestSync(MAC_WRP_PIB_ADAPTIVE_POWER_LOW_BOUND_RF, 0, 1,
            &app_g3_managementConst.adaptivePowerLowBoundRFconformance, &setConfirm);
}

static void _APP_G3_MANAGEMENT_InitializeParameters(void)
{
    ADP_SET_CFM_PARAMS setConfirm;

    /* Set extended address (EUI64). It must be unique for each device. */
    ADP_MacSetRequestSync(MAC_WRP_PIB_MANUF_EXTENDED_ADDRESS, 0, 8,
            (const uint8_t*) app_g3_managementData.eui64.value, &setConfirm);

    /* Set user-specific ADP parameters */
    ADP_SetRequestSync(ADP_IB_CONTEXT_INFORMATION_TABLE, 0, 14,
            app_g3_managementConst.contextInfoTable0, &setConfirm);

    ADP_SetRequestSync(ADP_IB_CONTEXT_INFORMATION_TABLE, 1, 10,
            app_g3_managementConst.contextInfoTable1, &setConfirm);

    ADP_SetRequestSync(ADP_IB_DEFAULT_COORD_ROUTE_ENABLED, 0, 1,
            &app_g3_managementConst.defaultCoordRouteEnabled, &setConfirm);

    /* Set user-specific MAC parameters */
    ADP_MacSetRequestSync(MAC_WRP_PIB_DUTY_CYCLE_LIMIT_RF, 0, 2,
            (const uint8_t*) &app_g3_managementConst.dutyCycleLimitRF, &setConfirm);

    if (app_g3_managementData.conformanceTest == true)
    {
        _APP_G3_MANAGEMENT_SetConformanceParameters();
    }
    else
    {
        /* Set user-specific ADP parameters that are set to different values in
         * Conformance Test */
        ADP_SetRequestSync(ADP_IB_ROUTING_TABLE_ENTRY_TTL, 0, 2,
                (const uint8_t*) &app_g3_managementConst.routingTableEntryTTL, &setConfirm);

        ADP_SetRequestSync(ADP_IB_MAX_JOIN_WAIT_TIME, 0, 2,
            (const uint8_t*) &app_g3_managementConst.maxJoinWaitTime, &setConfirm);

        ADP_SetRequestSync(ADP_IB_MAX_HOPS, 0, 1, &app_g3_managementConst.maxHops, &setConfirm);
    }

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

    if (app_g3_managementData.configureParamsRF == true)
    {
        /* In case we get here after a kick event, restore RF configuration set
         * by UDP responder */
        APP_G3_MANAGEMENT_SetConfigRF(app_g3_managementData.savedParamsRF);
    }
}

static bool _APP_G3_MANAGEMENT_CheckBeaconLOADngLBPframesDiscovery(void)
{
    ADP_MAC_GET_CFM_PARAMS getConfirm;

    /* Check Beacon frame received (PLC) */
    ADP_MacGetRequestSync(MAC_WRP_PIB_MANUF_BCN_FRAME_RECEIVED, 0, &getConfirm);
    if ((getConfirm.status == G3_SUCCESS) && (getConfirm.attributeValue[0] != 0))
    {
        /* At least one Beacon frame has been received through PLC */
        return true;
    }

    /* Check LOADng frame received (PLC) */
    ADP_MacGetRequestSync(MAC_WRP_PIB_MANUF_LNG_FRAME_RECEIVED, 0, &getConfirm);
    if ((getConfirm.status == G3_SUCCESS) && (getConfirm.attributeValue[0] != 0))
    {
        /* At least one LOADng frame has been received through PLC */
        return true;
    }

    /* Check LBP frame received (PLC) */
    ADP_MacGetRequestSync(MAC_WRP_PIB_MANUF_LBP_FRAME_RECEIVED, 0, &getConfirm);
    if ((getConfirm.status == G3_SUCCESS) && (getConfirm.attributeValue[0] != 0))
    {
        /* At least one LBP frame has been received through PLC */
        return true;
    }

    /* Check Beacon frame received (RF) */
    ADP_MacGetRequestSync(MAC_WRP_PIB_MANUF_BCN_FRAME_RECEIVED_RF, 0, &getConfirm);
    if ((getConfirm.status == G3_SUCCESS) && (getConfirm.attributeValue[0] != 0))
    {
        /* At least one Beacon frame has been received through RF */
        return true;
    }

    /* Check LOADng frame received (RF) */
    ADP_MacGetRequestSync(MAC_WRP_PIB_MANUF_LNG_FRAME_RECEIVED_RF, 0, &getConfirm);
    if ((getConfirm.status == G3_SUCCESS) && (getConfirm.attributeValue[0] != 0))
    {
        /* At least one LOADng frame has been received through RF */
        return true;
    }

    /* Check LBP frame received (RF) */
    ADP_MacGetRequestSync(MAC_WRP_PIB_MANUF_LBP_FRAME_RECEIVED_RF, 0, &getConfirm);
    if ((getConfirm.status == G3_SUCCESS) && (getConfirm.attributeValue[0] != 0))
    {
        /* At least one LBP frame has been received through RF */
        return true;
    }

    return false;
}

static bool _APP_G3_MANAGEMENT_CheckBeaconLOADngLBPframesJoin(void)
{
    ADP_MAC_GET_CFM_PARAMS getConfirm;
    MAC_WRP_PIB_ATTRIBUTE bcnAttribute, lngAttribute, lbpAttribute;

    if (app_g3_managementData.bestNetwork.mediaType == MAC_WRP_MEDIA_TYPE_IND_PLC)
    {
        bcnAttribute = MAC_WRP_PIB_MANUF_BCN_FRAME_RECEIVED;
        lngAttribute = MAC_WRP_PIB_MANUF_LNG_FRAME_RECEIVED;
        lbpAttribute = MAC_WRP_PIB_MANUF_LBP_FRAME_RECEIVED;
    }
    else
    {
        bcnAttribute = MAC_WRP_PIB_MANUF_BCN_FRAME_RECEIVED_RF;
        lngAttribute = MAC_WRP_PIB_MANUF_LNG_FRAME_RECEIVED_RF;
        lbpAttribute = MAC_WRP_PIB_MANUF_LBP_FRAME_RECEIVED_RF;
    }

    /* Check Beacon frame received */
    ADP_MacGetRequestSync(bcnAttribute, 0, &getConfirm);
    if ((getConfirm.status == G3_SUCCESS) && (getConfirm.attributeValue[0] != 0))
    {
        /* At least one Beacon frame has been received */
        return true;
    }

    /* Check LOADng frame received */
    ADP_MacGetRequestSync(lngAttribute, 0, &getConfirm);
    if ((getConfirm.status == G3_SUCCESS) && (getConfirm.attributeValue[0] != 0))
    {
        /* At least one LOADng frame has been received */
        return true;
    }

    /* Check LBP frame received */
    ADP_MacGetRequestSync(lbpAttribute, 0, &getConfirm);
    if ((getConfirm.status == G3_SUCCESS) && (getConfirm.attributeValue[0] != 0))
    {
        /* At least one LBP frame has been received */
        return true;
    }

    return false;
}

static void _APP_G3_MANAGEMENT_ResetBeaconLOADngLBPframesReceived(void)
{
    ADP_SET_CFM_PARAMS setConfirm;
    uint8_t resetFrameReceived = 0;

    /* Reset Beacon, LOADng and LBP frames received indicators */
    ADP_MacSetRequestSync(MAC_WRP_PIB_MANUF_BCN_FRAME_RECEIVED, 0, 1,
            (const uint8_t*) &resetFrameReceived, &setConfirm);

    ADP_MacSetRequestSync(MAC_WRP_PIB_MANUF_LNG_FRAME_RECEIVED, 0, 1,
            (const uint8_t*) &resetFrameReceived, &setConfirm);

    ADP_MacSetRequestSync(MAC_WRP_PIB_MANUF_LBP_FRAME_RECEIVED, 0, 1,
            (const uint8_t*) &resetFrameReceived, &setConfirm);

    /* Reset Beacon, LOADng and LBP frames received indicators (RF) */
    ADP_MacSetRequestSync(MAC_WRP_PIB_MANUF_BCN_FRAME_RECEIVED_RF, 0, 1,
            (const uint8_t*) &resetFrameReceived, &setConfirm);

    ADP_MacSetRequestSync(MAC_WRP_PIB_MANUF_LNG_FRAME_RECEIVED_RF, 0, 1,
            (const uint8_t*) &resetFrameReceived, &setConfirm);

    ADP_MacSetRequestSync(MAC_WRP_PIB_MANUF_LBP_FRAME_RECEIVED_RF, 0, 1,
            (const uint8_t*) &resetFrameReceived, &setConfirm);
}

static void _APP_G3_MANAGEMENT_ShowVersions(void)
{
    ADP_GET_CFM_PARAMS getAdpConfirm;
    ADP_MAC_GET_CFM_PARAMS getMacConfirm;

    /* Get G3 stack version */
    ADP_GetRequestSync(ADP_IB_SOFT_VERSION, 0, &getAdpConfirm);
    if (getAdpConfirm.status == G3_SUCCESS)
    {
        SYS_DEBUG_PRINT(SYS_ERROR_INFO, "APP_G3_MANAGEMENT: G3 stack version %hhu.%hhu.%hhu"
            " (Date: 20%hhu-%hhu-%hhu)\r\n",
            getAdpConfirm.attributeValue[0], getAdpConfirm.attributeValue[1],
            getAdpConfirm.attributeValue[2], getAdpConfirm.attributeValue[3],
            getAdpConfirm.attributeValue[4], getAdpConfirm.attributeValue[5]);
    }
    else
    {
        SYS_DEBUG_PRINT(SYS_ERROR_ERROR, "APP_G3_MANAGEMENT: Error getting G3 stack version\r\n");
    }

    /* Get ADP version */
    ADP_GetRequestSync(ADP_IB_MANUF_ADP_INTERNAL_VERSION, 0, &getAdpConfirm);
    if (getAdpConfirm.status == G3_SUCCESS)
    {
        SYS_DEBUG_PRINT(SYS_ERROR_INFO, "APP_G3_MANAGEMENT: ADP version %hhu.%hhu.%hhu"
            " (Date: 20%hhu-%hhu-%hhu)\r\n",
            getAdpConfirm.attributeValue[0], getAdpConfirm.attributeValue[1],
            getAdpConfirm.attributeValue[2], getAdpConfirm.attributeValue[3],
            getAdpConfirm.attributeValue[4], getAdpConfirm.attributeValue[5]);
    }
    else
    {
        SYS_DEBUG_PRINT(SYS_ERROR_ERROR, "APP_G3_MANAGEMENT: Error getting ADP version\r\n");
    }

    /* Check PLC interface availability */
    ADP_MacGetRequestSync(MAC_WRP_PIB_MANUF_PLC_IFACE_AVAILABLE, 0, &getMacConfirm);
    if ((getMacConfirm.status != G3_SUCCESS) || (getMacConfirm.attributeValue[0] == 0))
    {
        SYS_DEBUG_PRINT(SYS_ERROR_ERROR, "APP_G3_MANAGEMENT: PLC Interface not available\r\n");
    }
    else
    {
        SYS_DEBUG_PRINT(SYS_ERROR_ERROR, "APP_G3_MANAGEMENT: PLC Interface available\r\n");

        /* Get PLC MAC version */
        ADP_MacGetRequestSync(MAC_WRP_PIB_MANUF_MAC_INTERNAL_VERSION, 0, &getMacConfirm);
        if (getMacConfirm.status == G3_SUCCESS)
        {
            SYS_DEBUG_PRINT(SYS_ERROR_INFO, "\tPLC MAC version: %hhu.%hhu.%hhu"
                " (Date: 20%hhu-%hhu-%hhu)\r\n",
                getMacConfirm.attributeValue[0], getMacConfirm.attributeValue[1],
                getMacConfirm.attributeValue[2], getMacConfirm.attributeValue[3],
                getMacConfirm.attributeValue[4], getMacConfirm.attributeValue[5]);
        }
        else
        {
            SYS_DEBUG_PRINT(SYS_ERROR_ERROR, "APP_G3_MANAGEMENT: Error getting PLC MAC version\r\n");
        }

        /* Get PLC MAC version */
        ADP_MacGetRequestSync(MAC_WRP_PIB_MANUF_MAC_RT_INTERNAL_VERSION, 0, &getMacConfirm);
        if (getMacConfirm.status == G3_SUCCESS)
        {
            SYS_DEBUG_PRINT(SYS_ERROR_INFO, "\tPLC MAC-RT version: %hhu.%hhu.%hhu"
                " (Date: 20%hhu-%hhu-%hhu)\r\n",
                getMacConfirm.attributeValue[0], getMacConfirm.attributeValue[1],
                getMacConfirm.attributeValue[2], getMacConfirm.attributeValue[3],
                getMacConfirm.attributeValue[4], getMacConfirm.attributeValue[5]);
        }
        else
        {
            SYS_DEBUG_PRINT(SYS_ERROR_ERROR, "APP_G3_MANAGEMENT: Error getting PLC MAC-RT version\r\n");
        }

        /* Get PLC PHY version */
        ADP_MacGetRequestSync(MAC_WRP_PIB_MANUF_PHY_PARAM, MAC_WRP_PHY_PARAM_VERSION, &getMacConfirm);
        if (getMacConfirm.status == G3_SUCCESS)
        {
            SYS_DEBUG_PRINT(SYS_ERROR_INFO, "\tPLC PHY version: %02x.%02x.%02x.%02x\r\n",
                getMacConfirm.attributeValue[3], getMacConfirm.attributeValue[2],
                getMacConfirm.attributeValue[1], getMacConfirm.attributeValue[0]);
        }
        else
        {
            SYS_DEBUG_PRINT(SYS_ERROR_ERROR, "APP_G3_MANAGEMENT: Error getting PLC MAC-RT version\r\n");
        }
    }

    /* Check RF interface availability */
    ADP_MacGetRequestSync(MAC_WRP_PIB_MANUF_RF_IFACE_AVAILABLE, 0, &getMacConfirm);
    if ((getMacConfirm.status != G3_SUCCESS) || (getMacConfirm.attributeValue[0] == 0))
    {
        SYS_DEBUG_PRINT(SYS_ERROR_ERROR, "APP_G3_MANAGEMENT: RF Interface not available\r\n");
    }
    else
    {
        SYS_DEBUG_PRINT(SYS_ERROR_ERROR, "APP_G3_MANAGEMENT: RF Interface available\r\n");

        /* Get RF MAC version */
        ADP_MacGetRequestSync(MAC_WRP_PIB_MANUF_MAC_INTERNAL_VERSION_RF, 0, &getMacConfirm);
        if (getMacConfirm.status == G3_SUCCESS)
        {
            SYS_DEBUG_PRINT(SYS_ERROR_INFO, "\tRF MAC version: %hhu.%hhu.%hhu"
                " (Date: 20%hhu-%hhu-%hhu)\r\n",
                getMacConfirm.attributeValue[0], getMacConfirm.attributeValue[1],
                getMacConfirm.attributeValue[2], getMacConfirm.attributeValue[3],
                getMacConfirm.attributeValue[4], getMacConfirm.attributeValue[5]);
        }
        else
        {
            SYS_DEBUG_PRINT(SYS_ERROR_ERROR, "APP_G3_MANAGEMENT: Error getting RF MAC version\r\n");
        }

        /* Get RF MAC version */
        ADP_MacGetRequestSync(MAC_WRP_PIB_MANUF_PHY_PARAM_RF, MAC_WRP_RF_PHY_PARAM_FW_VERSION, &getMacConfirm);
        if (getMacConfirm.status == G3_SUCCESS)
        {
            SYS_DEBUG_PRINT(SYS_ERROR_INFO, "\tRF PHY version: %hhu.%hhu.%hhu"
                " (Date: 20%hhu-%hhu-%hhu)\r\n",
                getMacConfirm.attributeValue[0], getMacConfirm.attributeValue[1],
                getMacConfirm.attributeValue[2], getMacConfirm.attributeValue[3],
                getMacConfirm.attributeValue[4], getMacConfirm.attributeValue[5]);
        }
        else
        {
            SYS_DEBUG_PRINT(SYS_ERROR_ERROR, "APP_G3_MANAGEMENT: Error getting RF PHY version\r\n");
        }
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
    adpMngNotifications.networkStartConfirm = NULL;
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
    adpMngNotifications.bufferIndication = NULL;
    ADP_SetManagementNotifications(&adpMngNotifications);

    /* Place the application's state machine in its initial state. */
    app_g3_managementData.state = APP_G3_MANAGEMENT_STATE_ADP_OPEN;

    /* Initialize application variables */
    app_g3_managementData.timerLedHandle = SYS_TIME_HANDLE_INVALID;
    app_g3_managementData.timerLedExpired = false;
    app_g3_managementData.writeNonVolatileData = true;
    app_g3_managementData.configureParamsRF = false;

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

    if ((app_g3_managementData.state > APP_G3_MANAGEMENT_STATE_WAIT_ADP_READY) &&
            app_g3_managementData.state != APP_G3_MANAGEMENT_STATE_ERROR)
    {
        /* LBP Device tasks */
        LBP_TasksDev();
    }

    /* Check the application's current state */
    switch ( app_g3_managementData.state )
    {
        /* Application's initial state. */
        case APP_G3_MANAGEMENT_STATE_ADP_OPEN:
        {
            SRV_PLC_PCOUP_BRANCH plcBranch;
            ADP_BAND plcBand;

            /* Get configured PLC band */
            plcBranch = SRV_PCOUP_Get_Default_Branch();
            plcBand = SRV_PCOUP_Get_Phy_Band(plcBranch);

            /* Open G3 Adaptation Layer (ADP) */
            ADP_Open(plcBand);

            /* Get Extended Address from storage application */
            APP_STORAGE_GetExtendedAddress(app_g3_managementData.eui64.value);

#ifdef APP_G3_MANAGEMENT_CONFORMANCE_TEST
            /* Conformance Test enabled at compilation time.
             * APP_G3_MANAGEMENT_SetConformanceConfig could be used to enable Conformance
             * configuration at execution time. */
            APP_G3_MANAGEMENT_SetConformanceConfig();
#else
            app_g3_managementData.conformanceTest = false;
#endif

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
                LBP_NOTIFICATIONS_DEV lbpDevNotifications;
                LBP_SET_PARAM_CONFIRM lbpSetConfirm;

                /* ADP is ready. We can set ADP/MAC parameters. */
                _APP_G3_MANAGEMENT_ShowVersions();
                _APP_G3_MANAGEMENT_InitializeParameters();

                /* Initialize LoWPAN Bootstrapping Protocol (LBP) in Device,
                 * mode set call-backs and set PSK key */
                LBP_InitDev();
                lbpDevNotifications.adpNetworkJoinConfirm = _LBP_ADP_NetworkJoinConfirm;
                lbpDevNotifications.adpNetworkLeaveConfirm = NULL;
                lbpDevNotifications.adpNetworkLeaveIndication = _LBP_ADP_NetworkLeaveIndication;
                LBP_SetNotificationsDev(&lbpDevNotifications);
                if (app_g3_managementData.conformanceTest == false)
                {
                    LBP_SetParamDev(LBP_IB_PSK, 0, 16,
                            (const uint8_t*) &app_g3_managementConst.psk, &lbpSetConfirm);
                }
                else
                {
                    LBP_SetParamDev(LBP_IB_PSK, 0, 16,
                            (const uint8_t*) &app_g3_managementConst.pskConformance, &lbpSetConfirm);
                }

                if (app_g3_managementData.timerLedHandle == SYS_TIME_HANDLE_INVALID)
                {
                    /* Register timer callback to blink LED */
                    app_g3_managementData.timerLedHandle = SYS_TIME_CallbackRegisterMS(
                            _APP_G3_MANAGEMENT_TimeExpiredSetFlag,
                            (uintptr_t) &app_g3_managementData.timerLedExpired,
                            APP_G3_MANAGEMENT_LED_BLINK_PERIOD_MS, SYS_TIME_PERIODIC);
                }

                /* Initialize back-off window for network discovery */
                app_g3_managementData.backoffWindowLow = APP_G3_MANAGEMENT_DISCOVERY_BACKOFF_LOW_MIN;
                app_g3_managementData.backoffWindowHigh = APP_G3_MANAGEMENT_DISCOVERY_BACKOFF_HIGH_MIN;

                /* Next state (without break): Start back-off before start
                 * network discovery. */
                app_g3_managementData.state = APP_G3_MANAGEMENT_STATE_START_BACKOFF_DISCOVERY;

                SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "APP_G3_MANAGEMENT: ADP initialized successfully\r\n");
            }
            else
            {
                if (adpStatus == ADP_STATUS_ERROR)
                {
                    SYS_DEBUG_MESSAGE(SYS_ERROR_ERROR, "APP_G3_MANAGEMENT: Error in ADP initialization\r\n");
                    app_g3_managementData.state = APP_G3_MANAGEMENT_STATE_ERROR;
                }

                break;
            }
        }

        /* State to start back-off delay before start network discovery */
        case APP_G3_MANAGEMENT_STATE_START_BACKOFF_DISCOVERY:
        {
            SYS_TIME_HANDLE timeHandle;
            uint32_t backoffDelay;

            /* Get random back-off delay */
            backoffDelay = SRV_RANDOM_Get32bitsInRange(app_g3_managementData.backoffWindowLow,
                    app_g3_managementData.backoffWindowHigh);

            /* Register timer callback for the obtained back-off delay */
            app_g3_managementData.backoffExpired = false;
            timeHandle = SYS_TIME_CallbackRegisterMS(_APP_G3_MANAGEMENT_TimeExpiredSetFlag,
                    (uintptr_t) &app_g3_managementData.backoffExpired, backoffDelay, SYS_TIME_SINGLE);
            if (timeHandle != SYS_TIME_HANDLE_INVALID)
            {
                /* Next state: Back-off delay before start network discovery */
                app_g3_managementData.state = APP_G3_MANAGEMENT_STATE_BACKOFF_DISCOVERY;
                _APP_G3_MANAGEMENT_ResetBeaconLOADngLBPframesReceived();
            }

            break;
        }

        /* Back-off delay state before start network discovery (scan) */
        case APP_G3_MANAGEMENT_STATE_BACKOFF_DISCOVERY:
        {
            if (app_g3_managementData.backoffExpired == true)
            {
                /* Back-off delay finished. Check if beacon, LoadNG or LBP
                 * frames have been received. */
                if (_APP_G3_MANAGEMENT_CheckBeaconLOADngLBPframesDiscovery() == false)
                {
                    /* The channel is clean, start network discovery */
                    ADP_DiscoveryRequest(APP_G3_MANAGEMENT_DISCOVERY_DURATION);
                    app_g3_managementData.state = APP_G3_MANAGEMENT_STATE_SCANNING;

                    /* Initialize best network PAN descriptor */
                    app_g3_managementData.bestNetwork.panId = 0xFFFF;
                    app_g3_managementData.bestNetwork.lbaAddress = 0xFFFF;
                    app_g3_managementData.bestNetwork.rcCoord = 0xFFFF;
                    app_g3_managementData.bestNetwork.linkQuality = 0xFF;

                    SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, "APP_G3_MANAGEMENT: Network Discovery started\r\n");
                }
                else
                {
                    /* Beacon, LoadNG or LBP frames have been received during
                     * back-off. This means that another devices are trying to
                     * join to the network. Start a new back-off to avoid
                     * network congestion. */
                    app_g3_managementData.state = APP_G3_MANAGEMENT_STATE_START_BACKOFF_DISCOVERY;

                    /* Make back-off window wider */
                    app_g3_managementData.backoffWindowLow <<= 1;
                    if (app_g3_managementData.backoffWindowLow > APP_G3_MANAGEMENT_DISCOVERY_BACKOFF_LOW_MAX)
                    {
                        app_g3_managementData.backoffWindowLow = APP_G3_MANAGEMENT_DISCOVERY_BACKOFF_LOW_MAX;
                    }

                    app_g3_managementData.backoffWindowHigh <<= 1;
                    if (app_g3_managementData.backoffWindowHigh > APP_G3_MANAGEMENT_DISCOVERY_BACKOFF_HIGH_MAX)
                    {
                        app_g3_managementData.backoffWindowHigh = APP_G3_MANAGEMENT_DISCOVERY_BACKOFF_HIGH_MAX;
                    }
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

        /* State to start back-off delay before join to the network */
        case APP_G3_MANAGEMENT_STATE_START_BACKOFF_JOIN:
        {
            SYS_TIME_HANDLE timeHandle;
            uint32_t backoffDelay;

            /* Get random back-off delay */
            backoffDelay = SRV_RANDOM_Get32bitsInRange(app_g3_managementData.backoffWindowLow,
                    app_g3_managementData.backoffWindowHigh);

            /* Register timer callback for the obtained back-off delay */
            app_g3_managementData.backoffExpired = false;
            timeHandle = SYS_TIME_CallbackRegisterMS(_APP_G3_MANAGEMENT_TimeExpiredSetFlag,
                    (uintptr_t) &app_g3_managementData.backoffExpired, backoffDelay, SYS_TIME_SINGLE);
            if (timeHandle != SYS_TIME_HANDLE_INVALID)
            {
                /* Next state: Back-off delay before join to the network */
                app_g3_managementData.state = APP_G3_MANAGEMENT_STATE_BACKOFF_JOIN;
                _APP_G3_MANAGEMENT_ResetBeaconLOADngLBPframesReceived();
            }
            break;
        }

        /* Back-off delay state before join to the network */
        case APP_G3_MANAGEMENT_STATE_BACKOFF_JOIN:
        {
            if (app_g3_managementData.backoffExpired == true)
            {
                /* Back-off delay finished. Check if beacon, LoadNG or LBP
                 * frames have been received. */
                if (_APP_G3_MANAGEMENT_CheckBeaconLOADngLBPframesJoin() == false)
                {
                    /* The channel is clean, try to join to the network */
                    LBP_AdpNetworkJoinRequest(app_g3_managementData.bestNetwork.panId,
                            app_g3_managementData.bestNetwork.lbaAddress, app_g3_managementData.bestNetwork.mediaType);
                    app_g3_managementData.state = APP_G3_MANAGEMENT_STATE_JOINING;

                    SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, "APP_G3_MANAGEMENT: Network Join started\r\n");
                }
                else
                {
                    /* Beacon, LoadNG or LBP frames have been received during
                     * back-off. This means that another devices are trying to
                     * join to the network. Start a new back-off to avoid
                     * network congestion. */
                    app_g3_managementData.state = APP_G3_MANAGEMENT_STATE_START_BACKOFF_JOIN;

                    /* Make back-off window wider */
                    app_g3_managementData.backoffWindowLow <<= 1;
                    if (app_g3_managementData.backoffWindowLow > APP_G3_MANAGEMENT_JOIN_BACKOFF_LOW_MAX)
                    {
                        app_g3_managementData.backoffWindowLow = APP_G3_MANAGEMENT_JOIN_BACKOFF_LOW_MAX;
                    }

                    app_g3_managementData.backoffWindowHigh <<= 1;
                    if (app_g3_managementData.backoffWindowHigh > APP_G3_MANAGEMENT_JOIN_BACKOFF_HIGH_MAX)
                    {
                        app_g3_managementData.backoffWindowHigh = APP_G3_MANAGEMENT_JOIN_BACKOFF_HIGH_MAX;
                    }
                }
            }

            break;
        }

        /* Join to the network in progress */
        case APP_G3_MANAGEMENT_STATE_JOINING:
        {
            /* Nothing to do, state will be changed from
             * _LBP_ADP_NetworkJoinConfirm callback */
            break;
        }

        /* Joined to the network */
        case APP_G3_MANAGEMENT_STATE_JOINED:
        {
            /* Nothing to do. The device is joined to the network unless
             * _LBP_ADP_NetworkLeaveIndication is called */
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

uint16_t APP_G3_MANAGEMENT_GetShortAddress(void)
{
    return app_g3_managementData.shortAddress;
}

uint16_t APP_G3_MANAGEMENT_GetCoordinatorShortAddress(void)
{
    ADP_GET_CFM_PARAMS getConfirm;

    ADP_GetRequestSync(ADP_IB_COORD_SHORT_ADDRESS, 0, &getConfirm);

    return *((uint16_t*) getConfirm.attributeValue);
}

uint8_t* APP_G3_MANAGEMENT_GetExtendedAddress(void)
{
    return app_g3_managementData.eui64.value;
}

void APP_G3_MANAGEMENT_SetConformanceConfig ( void )
{
    app_g3_managementData.conformanceTest = true;

    if (ADP_Status() >= ADP_STATUS_READY)
    {
        LBP_SET_PARAM_CONFIRM lbpSetConfirm;

        /* ADP is ready: set conformance parameters */
        _APP_G3_MANAGEMENT_SetConformanceParameters();
        LBP_SetParamDev(LBP_IB_PSK, 0, 16,
                (const uint8_t*) &app_g3_managementConst.pskConformance, &lbpSetConfirm);
    }

    APP_TCPIP_MANAGEMENT_SetConformanceConfig();
}

uint8_t APP_G3_MANAGEMENT_SetConformanceTrickleConfig(uint8_t trickleActivation)
{
    ADP_SET_CFM_PARAMS setConfirm;
    uint16_t delayValue;

    if (trickleActivation == 1)
    {
        uint8_t valueTrue = 1;

        /* Disable RF PHY */
        ADP_MacSetRequestSync(MAC_WRP_PIB_DISABLE_PHY_RF, 0, 1, (const uint8_t*) &valueTrue, &setConfirm);

        /* Enable Trickle Data */
        ADP_SetRequestSync(ADP_IB_TRICKLE_DATA_ENABLED, 0, 1, (const uint8_t*) &valueTrue, &setConfirm);
        if (setConfirm.status != G3_SUCCESS)
        {
            return 1;
        }

        /* Enable Cluster Trickle */
        ADP_SetRequestSync(ADP_IB_CLUSTER_TRICKLE_ENABLED, 0, 1, (const uint8_t*) &valueTrue, &setConfirm);
        if (setConfirm.status != G3_SUCCESS)
        {
            return 1;
        }

        /* Set Delay Low LQI */
        delayValue = 1500;
        ADP_SetRequestSync(ADP_IB_DELAY_LOW_LQI, 0, 2, (const uint8_t*) &delayValue, &setConfirm);
        if (setConfirm.status != G3_SUCCESS)
        {
            return 1;
        }

        /* Set Delay High LQI */
        delayValue = 100;
        ADP_SetRequestSync(ADP_IB_DELAY_HIGH_LQI, 0, 2, (const uint8_t*) &delayValue, &setConfirm);
        if (setConfirm.status != G3_SUCCESS)
        {
            return 1;
        }

        /* Success */
        return 0;
    }
    else if (trickleActivation == 0)
    {
        uint8_t valueFalse = 0;

        /* Enable RF PHY */
        ADP_MacSetRequestSync(MAC_WRP_PIB_DISABLE_PHY_RF, 0, 1, (const uint8_t*) &valueFalse, &setConfirm);

        /* Disable Trickle Data */
        ADP_SetRequestSync(ADP_IB_TRICKLE_DATA_ENABLED, 0, 1, (const uint8_t*) &valueFalse, &setConfirm);
        if (setConfirm.status != G3_SUCCESS)
        {
            return 1;
        }

        /* Disable Cluster Trickle */
        ADP_SetRequestSync(ADP_IB_CLUSTER_TRICKLE_ENABLED, 0, 1, (const uint8_t*) &valueFalse, &setConfirm);
        if (setConfirm.status != G3_SUCCESS)
        {
            return 1;
        }

        /* Set Delay Low LQI */
        delayValue = 0;
        ADP_SetRequestSync(ADP_IB_DELAY_LOW_LQI, 0, 2, (const uint8_t*) &delayValue, &setConfirm);
        if (setConfirm.status != G3_SUCCESS)
        {
            return 1;
        }

        /* Set Delay High LQI */
        ADP_SetRequestSync(ADP_IB_DELAY_HIGH_LQI, 0, 2, (const uint8_t*) &delayValue, &setConfirm);
        if (setConfirm.status != G3_SUCCESS)
        {
            return 1;
        }

        /* Success */
        return 0;
    }

    /* Unexpected value */
    return 1;
}

uint8_t APP_G3_MANAGEMENT_SetConfigRF(uint8_t* pParameters)
{
    ADP_SET_CFM_PARAMS setConfirm;
    ADP_MAC_GET_CFM_PARAMS macGetConfirm;
    uint16_t freqBandOpMode;
    uint8_t frequencyBand, operatingMode, hoppingActivation;

    /* Check RF interface availability */
    ADP_MacGetRequestSync(MAC_WRP_PIB_MANUF_RF_IFACE_AVAILABLE, 0, &macGetConfirm);
    if ((macGetConfirm.status != G3_SUCCESS) || (macGetConfirm.attributeValue[0] == 0))
    {
        /* RF interface not available */
        return 1;
    }

    /* Save RF parameters to restore configuration after kick */
    app_g3_managementData.configureParamsRF = true;
    memcpy(app_g3_managementData.savedParamsRF, pParameters, 4);

    /* Decode parameters */
    frequencyBand = *pParameters++;
    operatingMode = *pParameters++;
    hoppingActivation = *pParameters++;

    /* Set frequency band and operating mode */
    freqBandOpMode = (uint16_t) operatingMode;
    freqBandOpMode += (uint16_t) frequencyBand << 8;
    ADP_MacSetRequestSync(MAC_WRP_PIB_MANUF_PHY_PARAM_RF, MAC_WRP_RF_PHY_PARAM_PHY_BAND_OPERATING_MODE,
            2, (const uint8_t*) &freqBandOpMode, &setConfirm);
    if (setConfirm.status != G3_SUCCESS)
    {
        /* Invalid/unsupported frequency band/operating mode */
        return 2;
    }

    if (hoppingActivation == 0)
    {
        uint16_t channelNumber = (uint16_t) *pParameters;

        /* Set channel */
        ADP_MacSetRequestSync(MAC_WRP_PIB_MANUF_PHY_PARAM_RF, MAC_WRP_RF_PHY_PARAM_PHY_CHANNEL_NUM,
            2, (const uint8_t*) &channelNumber, &setConfirm);
        if (setConfirm.status != G3_SUCCESS)
        {
            /* Invalid/unsupported channel number */
            return 1;
        }

        /* Success */
        return 0;
    }
    else
    {
        /* Hopping not yet supported */
        return 2;
    }
}

void APP_G3_MANAGEMENT_SetContinuousTxRF ( void )
{
    ADP_SET_CFM_PARAMS setConfirm;
    uint8_t continuousTxEnabled = 1;

    ADP_MacSetRequestSync(MAC_WRP_PIB_MANUF_PHY_PARAM_RF, MAC_WRP_RF_PHY_PARAM_SET_CONTINUOUS_TX_MODE,
            1, (const uint8_t*) &continuousTxEnabled, &setConfirm);
}

void APP_G3_MANAGEMENT_SetLastGaspMode(void)
{
    ADP_SET_CFM_PARAMS setConfirm;
    uint8_t valueTrue = 1;

    /* Enable Last Gasp */
    ADP_SetRequestSync(ADP_IB_LAST_GASP, 0, 1, (const uint8_t*) &valueTrue, &setConfirm);
}

/*******************************************************************************
 End of File
 */
