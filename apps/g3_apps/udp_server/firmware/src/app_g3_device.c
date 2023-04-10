/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_g3_device.c

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

#include <string.h>
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
    Holds application data.

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_G3_DEVICE_Initialize
    function.

    Application strings and buffers are be defined outside this structure.
*/

APP_G3_DEVICE_DATA app_g3_deviceData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Constants
// *****************************************************************************
// *****************************************************************************

static const APP_G3_DEVICE_CONSTANTS app_g3_deviceConst = {
    /* G3 Application parameters */
    .psk = APP_G3_PSK_KEY,
    .contextInfoTable0 = APP_G3_CONTEXT_INFO_TABLE_0,
    .contextInfoTable1 = APP_G3_CONTEXT_INFO_TABLE_1,
    .routingTableEntryTTL = APP_G3_ROUTING_TABLE_ENTRY_TTL,
    .maxJoinWaitTime = APP_G3_MAX_JOIN_WAIT_TIME,
    .maxHops = APP_G3_MAX_HOPS,
    .dutyCycleLimitRF = APP_G3_DEVICE_DUTY_CYCLE_LIMIT_RF,
    .defaultCoordRouteEnabled = APP_G3_DEVICE_DEFAULT_COORD_ROUTE_ENABLED,

    /* G3 Conformance parameters */
    .blacklistTableEntryTTLconformance = APP_G3_BLACKLIST_TABLE_ENTRY_TTL_CONFORMANCE,
    .gropTable0Conformance = APP_G3_GROUP_TABLE_0_CONFORMANCE,
    .gropTable1Conformance = APP_G3_GROUP_TABLE_1_CONFORMANCE,
    .routingTableEntryTTLconformance = APP_G3_ROUTING_TABLE_ENTRY_TTL_CONFORMANCE,
    .maxJoinWaitTimeConformance = APP_G3_MAX_JOIN_WAIT_TIME_CONFORMANCE,
    .destAddrSet0Conformance = APP_G3_DEST_ADDR_SET_0_CONFORMANCE,
    .maxHopsConformance = APP_G3_MAX_HOPS_CONFORMANCE,
    .tmrTTLconformance = APP_G3_TMR_TTL_CONFORMANCE,
    .maxCSMAbackoffsRFconformance = APP_G3_MAX_CSMA_BACKOFFS_RF_CONFORMANCE,
    .maxFrameRetriesRFconformance = APP_G3_MAX_FRAME_RETRIES_RF_CONFORMANCE,
    .posTableEntryTTLconformance = APP_G3_POS_TABLE_TTL_CONFORMANCE,
    .krConformance = APP_G3_KRT_CONFORMANCE,
    .kmConformance = APP_G3_KM_CONFORMANCE,
    .kcConformance = APP_G3_KC_CONFORMANCE,
    .kqConformance = APP_G3_KQ_CONFORMANCE,
    .khConformance = APP_G3_KH_CONFORMANCE,
    .krtConformance = APP_G3_KRT_CONFORMANCE,
    .kqRFconformance = APP_G3_KQ_RF_CONFORMANCE,
    .khRFconformance = APP_G3_KH_RF_CONFORMANCE,
    .krtConformance = APP_G3_KRT_RF_CONFORMANCE,
    .kdcRFconformance = APP_G3_KDC_RF_CONFORMANCE

};

// *****************************************************************************
// *****************************************************************************
// Section: Function Declaration
// *****************************************************************************
// *****************************************************************************

static void _APP_G3_WriteNonVolatileDataGPBR(void);

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

static void _ADP_DiscoveryConfirm(uint8_t status)
{
    if ((status == G3_SUCCESS) && (app_g3_deviceData.bestNetwork.panId != 0xFFFF) &&
            (app_g3_deviceData.bestNetwork.lbaAddress != 0xFFFF))
    {
        /* Good network found. Start back-off before join to that network */
        app_g3_deviceData.state = APP_G3_DEVICE_STATE_START_BACKOFF_JOIN;
        app_g3_deviceData.joinRetries = 0;

        /* Initialize back-off window for joining */
        app_g3_deviceData.backoffWindowLow = APP_G3_DEVICE_JOIN_BACKOFF_LOW_MIN;
        app_g3_deviceData.backoffWindowHigh = APP_G3_DEVICE_JOIN_BACKOFF_HIGH_MIN;

        SYS_DEBUG_PRINT(SYS_ERROR_DEBUG, "APP_G3_DEVICE: Good network found. PAN ID: 0x%04X, "
                "LBA: 0x%04X, Route Cost Coordinator: %hu, Link Quality: %hhu, Media Type: %hhu\r\n",
                app_g3_deviceData.bestNetwork.panId, app_g3_deviceData.bestNetwork.lbaAddress,
                app_g3_deviceData.bestNetwork.rcCoord, app_g3_deviceData.bestNetwork.linkQuality,
                app_g3_deviceData.bestNetwork.mediaType);
    }
    else
    {
        /* No network found. Go back to back-off before network discovery */
        app_g3_deviceData.state = APP_G3_DEVICE_STATE_START_BACKOFF_DISCOVERY;

        /* Initialize back-off window for network discovery */
        app_g3_deviceData.backoffWindowLow = APP_G3_DEVICE_DISCOVERY_BACKOFF_LOW_MIN;
        app_g3_deviceData.backoffWindowHigh = APP_G3_DEVICE_DISCOVERY_BACKOFF_HIGH_MIN;

        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "APP_G3_DEVICE: No good network found in discovery\r\n");
    }
}

static void _ADP_DiscoveryIndication(ADP_PAN_DESCRIPTOR* pPanDescriptor)
{
    /* Check minimum Link Quality and maximum route cost to Coordinator */
    if ((pPanDescriptor->linkQuality >= APP_G3_DEVICE_LQI_MIN) &&
            (pPanDescriptor->rcCoord < APP_G3_DEVICE_ROUTE_COST_COORD_MAX))
    {
        /* Update best network if route cost to Coordinator is better or if it
         * is equal and Link Quality is better */
        if ((pPanDescriptor->rcCoord < app_g3_deviceData.bestNetwork.rcCoord) ||
                ((pPanDescriptor->rcCoord == app_g3_deviceData.bestNetwork.rcCoord) &&
                (pPanDescriptor->linkQuality > app_g3_deviceData.bestNetwork.linkQuality)))
        {
            app_g3_deviceData.bestNetwork = *pPanDescriptor;
        }
    }

    SYS_DEBUG_PRINT(SYS_ERROR_DEBUG, "APP_G3_DEVICE: Network discovered. PAN ID: 0x%04X, LBA: 0x%04X, "
            "Route Cost Coordinator: %hu, Link Quality: %hhu, Media Type: %hhu\r\n",
            pPanDescriptor->panId, pPanDescriptor->lbaAddress, pPanDescriptor->rcCoord,
            pPanDescriptor->linkQuality, pPanDescriptor->mediaType);
}

static void _ADP_NonVolatileDataIndication(ADP_NON_VOLATILE_DATA_IND_PARAMS* pNonVolatileData)
{
    /* Store non-volatile data to write it in user signature at power-down */
    app_g3_deviceData.nonVolatileData = *pNonVolatileData;
    app_g3_deviceData.validNonVolatileData = true;

    /* Write non-volatile data data in GPBR in order to read it at non-power-up
     * reset. */
    _APP_G3_WriteNonVolatileDataGPBR();
}

static void _LBP_ADP_NetworkJoinConfirm(LBP_ADP_NETWORK_JOIN_CFM_PARAMS* pNetworkJoinCfm)
{
    if (pNetworkJoinCfm->status == G3_SUCCESS)
    {
        ADP_GET_CFM_PARAMS getConfirm;
        ADP_SET_CFM_PARAMS setConfirm;
        IPV6_ADDR networkPrefix;
        uint8_t prefixData[27];

        /* Successful join */
        app_g3_deviceData.state = APP_G3_DEVICE_STATE_JOINED;
        app_g3_deviceData.shortAddress = pNetworkJoinCfm->networkAddress;
        app_g3_deviceData.panId = pNetworkJoinCfm->panId;

        /* Update Context Information Table index 0. Only update PAN ID. */
        ADP_GetRequestSync(ADP_IB_CONTEXT_INFORMATION_TABLE, 0, &getConfirm);
        if (getConfirm.status == G3_SUCCESS)
        {
            getConfirm.attributeValue[12] = (uint8_t) (pNetworkJoinCfm->panId >> 8);
            getConfirm.attributeValue[13] = (uint8_t) pNetworkJoinCfm->panId;
            ADP_SetRequestSync(ADP_IB_CONTEXT_INFORMATION_TABLE, 0, getConfirm.attributeLength,
                (const uint8_t*) getConfirm.attributeValue, &setConfirm);
        }

        /* Start a route request to coordinator */
        ADP_GetRequestSync(ADP_IB_COORD_SHORT_ADDRESS, 0, &getConfirm);
        if (getConfirm.status == G3_SUCCESS)
        {

            uint16_t coordShortAddress = *((uint16_t*) getConfirm.attributeValue);
            uint8_t maxHops = APP_G3_MAX_HOPS;

            if (app_g3_deviceData.conformanceTest == true)
            {
                maxHops = APP_G3_MAX_HOPS_CONFORMANCE;
            }

            ADP_RouteDiscoveryRequest(coordShortAddress, maxHops);
        }

        /* Notify to UDP application: Configure IPv6 addresses */
        APP_UDP_NetworkJoined();

        /* Configure Network Prefix in ADP */
        TCPIP_Helper_StringToIPv6Address(APP_UDP_IPV6_NETWORK_PREFIX_G3, &networkPrefix);
        networkPrefix.v[6] = (uint8_t) (pNetworkJoinCfm->panId >> 8);
        networkPrefix.v[7] = (uint8_t) pNetworkJoinCfm->panId;
        prefixData[0] = APP_UDP_IPV6_NETWORK_PREFIX_G3_LEN;
        prefixData[1] = 1; // OnLink flag
        prefixData[2] = 1; // AutonomuosConfiguration flag
        *((uint32_t*) &prefixData[3]) = 2000000; // valid lifetime
        *((uint32_t*) &prefixData[7]) = 2000000; // preferred lifetime
        memcpy(&prefixData[11], &networkPrefix, 16);
        ADP_SetRequestSync(ADP_IB_PREFIX_TABLE, 0, 27, (const uint8_t*) prefixData, &setConfirm);

        SYS_DEBUG_PRINT(SYS_ERROR_INFO, "APP_G3_DEVICE: Joined to the network. PAN ID: 0x%04X,"
                " Short Address: 0x%04X\r\n", pNetworkJoinCfm->panId, pNetworkJoinCfm->networkAddress);
    }
    else
    {
        /* Unsuccessful join. Try at maximum 3 times. */
        if (++app_g3_deviceData.joinRetries > 2)
        {
            /* Try another time */
            app_g3_deviceData.state = APP_G3_DEVICE_STATE_START_BACKOFF_JOIN;
            app_g3_deviceData.backoffWindowLow = APP_G3_DEVICE_JOIN_BACKOFF_LOW_MIN;
            app_g3_deviceData.backoffWindowHigh = APP_G3_DEVICE_JOIN_BACKOFF_HIGH_MIN;

            SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, "APP_G3_DEVICE: Failed to join, retry\r\n");
        }
        else
        {
            /* Maximum join retries reached. Go to back-off before start
             * network discovery. */
            app_g3_deviceData.state = APP_G3_DEVICE_STATE_START_BACKOFF_DISCOVERY;
            app_g3_deviceData.backoffWindowLow = APP_G3_DEVICE_DISCOVERY_BACKOFF_LOW_MIN;
            app_g3_deviceData.backoffWindowHigh = APP_G3_DEVICE_DISCOVERY_BACKOFF_HIGH_MIN;

            SYS_DEBUG_MESSAGE(SYS_ERROR_WARNING, "APP_G3_DEVICE: Failed to join after last retry\r\n");
        }
    }
}

static void _LBP_ADP_NetworkLeaveIndication(void)
{
    /* The device left the network. ADP is reseted internally. Go to first
     * state. */
    app_g3_deviceData.state = APP_G3_DEVICE_STATE_WAIT_ADP_READY;
    APP_UDP_NetworkDisconnected();

    SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "APP_G3_DEVICE: Kicked from network\r\n");
}

static void _SUPC_PowerDownCallback(uint32_t supc_status, uintptr_t context)
{
    if ((supc_status & SUPC_ISR_VDD3V3SMEV_Msk) != 0)
    {
        /* VDD3V3 supply monitor event */
        if (app_g3_deviceData.validNonVolatileData == true)
        {
            uint32_t userSignatureData[APP_G3_NON_VOLATILE_DATA_USER_SIGNATURE_SIZE];

            /* Write non-volatile data in User Signature. Put key in first 32
             * bits. */
            userSignatureData[0] = APP_G3_DEVICE_NON_VOLATILE_DATA_KEY_USER_SIGNATURE;
            memcpy(&userSignatureData[1], (void*) context, sizeof(ADP_NON_VOLATILE_DATA_IND_PARAMS));
            SEFC0_UserSignatureWrite(userSignatureData,
                APP_G3_NON_VOLATILE_DATA_USER_SIGNATURE_SIZE, BLOCK_0, PAGE_0);
        }
    }
}

static void _APP_G3_TimeExpiredSetFlag(uintptr_t context)
{
    /* Context holds the flag's address */
    *((bool *) context) = true;
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

static void _APP_G3_WriteNonVolatileDataGPBR(void)
{
    uint32_t gpbr0Data;

    /* Write non-volatile data data in GPBR in order to read it at non-power-up
     * reset.
     * GPBR0: Discover Sequence Number + Broadcast Sequence Number + one-byte
     * key to detect valid data.
     * GPBR1: MAC PLC Frame Counter.
     * GPBR2: MAC RF Frame Counter. */
    gpbr0Data = app_g3_deviceData.nonVolatileData.discoverSeqNumber;
    gpbr0Data += (uint32_t) app_g3_deviceData.nonVolatileData.broadcastSeqNumber << 16;
    gpbr0Data += APP_G3_DEVICE_NON_VOLATILE_DATA_KEY_GPBR << 24;
    SUPC_GPBRWrite(GPBR_REGS_0, gpbr0Data);
    SUPC_GPBRWrite(GPBR_REGS_1, app_g3_deviceData.nonVolatileData.frameCounter);
    SUPC_GPBRWrite(GPBR_REGS_2, app_g3_deviceData.nonVolatileData.frameCounterRF);
}

static void _APP_G3_SetConformanceParameters(void)
{
    ADP_SET_CFM_PARAMS setConfirm;

    /* Set ADP parameters needed for Conformance Test */
    ADP_SetRequestSync(ADP_IB_BLACKLIST_TABLE_ENTRY_TTL, 0, 2,
            (const uint8_t*) &app_g3_deviceConst.blacklistTableEntryTTLconformance,
            &setConfirm);

    ADP_SetRequestSync(ADP_IB_GROUP_TABLE, 0, 2,
            (const uint8_t*) &app_g3_deviceConst.gropTable0Conformance,
            &setConfirm);

    ADP_SetRequestSync(ADP_IB_GROUP_TABLE, 1, 2,
            (const uint8_t*) &app_g3_deviceConst.gropTable1Conformance,
            &setConfirm);

    ADP_SetRequestSync(ADP_IB_ROUTING_TABLE_ENTRY_TTL, 0, 2,
            (const uint8_t*) &app_g3_deviceConst.routingTableEntryTTLconformance,
            &setConfirm);

    ADP_SetRequestSync(ADP_IB_MAX_JOIN_WAIT_TIME, 0, 2,
            (const uint8_t*) &app_g3_deviceConst.maxJoinWaitTimeConformance,
            &setConfirm);

    ADP_SetRequestSync(ADP_IB_DESTINATION_ADDRESS_SET, 0, 2,
            (const uint8_t*) &app_g3_deviceConst.destAddrSet0Conformance,
            &setConfirm);

    ADP_SetRequestSync(ADP_IB_MAX_HOPS, 0, 1,
            &app_g3_deviceConst.maxHopsConformance, &setConfirm);

    ADP_SetRequestSync(ADP_IB_KR, 0, 1,
            &app_g3_deviceConst.krConformance, &setConfirm);

    ADP_SetRequestSync(ADP_IB_KM, 0, 1,
            &app_g3_deviceConst.kmConformance, &setConfirm);

    ADP_SetRequestSync(ADP_IB_KC, 0, 1,
            &app_g3_deviceConst.kcConformance, &setConfirm);

    ADP_SetRequestSync(ADP_IB_KQ, 0, 1,
            &app_g3_deviceConst.kqConformance, &setConfirm);

    ADP_SetRequestSync(ADP_IB_KH, 0, 1,
            &app_g3_deviceConst.khConformance, &setConfirm);

    ADP_SetRequestSync(ADP_IB_KRT, 0, 1,
            &app_g3_deviceConst.krtConformance, &setConfirm);

    ADP_SetRequestSync(ADP_IB_KQ_RF, 0, 1,
            &app_g3_deviceConst.kqRFconformance, &setConfirm);

    ADP_SetRequestSync(ADP_IB_KH_RF, 0, 1,
            &app_g3_deviceConst.khRFconformance, &setConfirm);

    ADP_SetRequestSync(ADP_IB_KRT_RF, 0, 1,
            &app_g3_deviceConst.krtRFconformance, &setConfirm);

    ADP_SetRequestSync(ADP_IB_KDC_RF, 0, 1,
            &app_g3_deviceConst.kdcRFconformance, &setConfirm);

    /* Set MAC parameters needed for Conformance Test */
    ADP_MacSetRequestSync(MAC_WRP_PIB_TMR_TTL, 0, 1,
            &app_g3_deviceConst.tmrTTLconformance, &setConfirm);

    ADP_MacSetRequestSync(MAC_WRP_PIB_MAX_CSMA_BACKOFFS_RF, 0, 1,
            &app_g3_deviceConst.maxCSMAbackoffsRFconformance, &setConfirm);

    ADP_MacSetRequestSync(MAC_WRP_PIB_MAX_FRAME_RETRIES_RF, 0, 1,
            &app_g3_deviceConst.maxFrameRetriesRFconformance, &setConfirm);

    ADP_MacSetRequestSync(MAC_WRP_PIB_POS_TABLE_ENTRY_TTL, 0, 1,
            &app_g3_deviceConst.posTableEntryTTLconformance, &setConfirm);
}

static void _APP_G3_DEVICE_InitializeParameters(void)
{
    ADP_SET_CFM_PARAMS setConfirm;
    LBP_SET_PARAM_CONFIRM lbpSetConfirm;

    /* Set extended address (EUI64). It must be unique for each device. */
    ADP_MacSetRequestSync(MAC_WRP_PIB_MANUF_EXTENDED_ADDRESS, 0, 8,
            (const uint8_t*) app_g3_deviceData.eui64.value, &setConfirm);

    /* Set user-specific ADP parameters */
    ADP_SetRequestSync(ADP_IB_CONTEXT_INFORMATION_TABLE, 0, 14,
            app_g3_deviceConst.contextInfoTable0, &setConfirm);

    ADP_SetRequestSync(ADP_IB_CONTEXT_INFORMATION_TABLE, 1, 10,
            app_g3_deviceConst.contextInfoTable1, &setConfirm);

    ADP_SetRequestSync(ADP_IB_DEFAULT_COORD_ROUTE_ENABLED, 0, 1,
            &app_g3_deviceConst.defaultCoordRouteEnabled, &setConfirm);

    /* Set user-specific MAC parameters */
    ADP_MacSetRequestSync(MAC_WRP_PIB_DUTY_CYCLE_LIMIT_RF, 0, 2,
            (const uint8_t*) &app_g3_deviceConst.dutyCycleLimitRF, &setConfirm);

    if (app_g3_deviceData.conformanceTest == true)
    {
        _APP_G3_SetConformanceParameters();
    }
    else
    {
        /* Set user-specific ADP parameters that are set to different values in
         * Conformance Test */
        ADP_SetRequestSync(ADP_IB_ROUTING_TABLE_ENTRY_TTL, 0, 2,
            (const uint8_t*) &app_g3_deviceConst.routingTableEntryTTL,
            &setConfirm);

        ADP_SetRequestSync(ADP_IB_MAX_JOIN_WAIT_TIME, 0, 2,
            (const uint8_t*) &app_g3_deviceConst.maxJoinWaitTime, &setConfirm);

        ADP_SetRequestSync(ADP_IB_MAX_HOPS, 0, 1,
            &app_g3_deviceConst.maxHops, &setConfirm);
    }

    if (app_g3_deviceData.writeNonVolatileData == true)
    {
        /* Set ADP/MAC non-volatile data parameters */
        ADP_MacSetRequestSync(MAC_WRP_PIB_FRAME_COUNTER, 0, 4,
            (const uint8_t*) &app_g3_deviceData.nonVolatileData.frameCounter,
            &setConfirm);

        ADP_MacSetRequestSync(MAC_WRP_PIB_FRAME_COUNTER_RF, 0, 4,
            (const uint8_t*) &app_g3_deviceData.nonVolatileData.frameCounterRF,
            &setConfirm);

        ADP_SetRequestSync(ADP_IB_MANUF_DISCOVER_SEQUENCE_NUMBER, 0, 2,
            (const uint8_t*) &app_g3_deviceData.nonVolatileData.discoverSeqNumber,
            &setConfirm);

        ADP_SetRequestSync(ADP_IB_MANUF_BROADCAST_SEQUENCE_NUMBER, 0, 1,
            (const uint8_t*) &app_g3_deviceData.nonVolatileData.broadcastSeqNumber,
            &setConfirm);

        /* Not needed to set ADP/MAC non-volatile data parameters anymore
         * because they are internally restored when ADP reset is performed */
        app_g3_deviceData.writeNonVolatileData = false;
    }

    /* Set PSK Key */
    LBP_SetParamDev(LBP_IB_PSK, 0, 16, (const uint8_t*) &app_g3_deviceConst.psk, &lbpSetConfirm);
}

static bool _APP_G3_DEVICE_CheckBeaconLOADngLBPframes(void)
{
    ADP_MAC_GET_CFM_PARAMS getConfirm;

    /* Check Beacon frame received */
    ADP_MacGetRequestSync(MAC_WRP_PIB_MANUF_BCN_FRAME_RECEIVED, 0, &getConfirm);
    if ((getConfirm.status == G3_SUCCESS) && (getConfirm.attributeValue[0] != 0))
    {
        /* At least one Beacon frame has been received */
        return true;
    }

    /* Check LOADng frame received */
    ADP_MacGetRequestSync(MAC_WRP_PIB_MANUF_LNG_FRAME_RECEIVED, 0, &getConfirm);
    if ((getConfirm.status == G3_SUCCESS) && (getConfirm.attributeValue[0] != 0))
    {
        /* At least one LOADng frame has been received */
        return true;
    }

    /* Check LBP frame received */
    ADP_MacGetRequestSync(MAC_WRP_PIB_MANUF_LBP_FRAME_RECEIVED, 0, &getConfirm);
    if ((getConfirm.status == G3_SUCCESS) && (getConfirm.attributeValue[0] != 0))
    {
        /* At least one LBP frame has been received */
        return true;
    }

    return false;
}

static void _APP_G3_DEVICE_ResetBeaconLOADngLBPframesReceived(void)
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
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_G3_DEVICE_Initialize ( void )

  Remarks:
    See prototype in app_g3_device.h.
 */

void APP_G3_DEVICE_Initialize ( void )
{
    ADP_MANAGEMENT_NOTIFICATIONS adpMngNotifications;
    LBP_NOTIFICATIONS_DEV lbpDevNotifications;
    uint8_t uniqueId[16];

    /* Disable User Signature write protection */
    SEFC0_WriteProtectionSet(0);

    /* Enable write and read User Signature (block 0 / area 1) rights */
    SEFC0_UserSignatureRightsSet(SEFC_EEFC_USR_RDENUSB1_Msk | SEFC_EEFC_USR_WRENUSB1_Msk);

    if (RSTC_ResetCauseGet() == RSTC_RESET_CAUSE_GENERAL)
    {
        uint32_t userSignatureData[APP_G3_NON_VOLATILE_DATA_USER_SIGNATURE_SIZE];

        /* Power-on reset. Read non-volatile data from User Signature. */
        SEFC0_UserSignatureRead(userSignatureData,
                APP_G3_NON_VOLATILE_DATA_USER_SIGNATURE_SIZE, BLOCK_0, PAGE_0);

        /* Check key in first 32 bits */
        if (userSignatureData[0] == APP_G3_DEVICE_NON_VOLATILE_DATA_KEY_USER_SIGNATURE)
        {
            /* Valid key read from User Signature */
            app_g3_deviceData.writeNonVolatileData = true;
            memcpy(&app_g3_deviceData.nonVolatileData, &userSignatureData[1],
                    sizeof(ADP_NON_VOLATILE_DATA_IND_PARAMS));

            /* Write non-volatile data in GPBR */
            _APP_G3_WriteNonVolatileDataGPBR();
        }
        else
        {
            /* Invalid key read from User Signature */
            app_g3_deviceData.writeNonVolatileData = false;
        }

        /* Erase User Signature to write it faster from SUPC power-down
         * callback */
        SEFC0_UserSignatureErase(BLOCK_0);
    }
    else
    {
        uint32_t gpbr0Data;

        /* Not power-on reset. Read non-volatile data from GPBR.
         * GPBR0: Discover Sequence Number + Broadcast Sequence Number +
         * one-byte key to detect valid data.
         * GPBR1: MAC PLC Frame Counter.
         * GPBR2: MAC RF Frame Counter. */
        gpbr0Data = SUPC_GPBRRead(GPBR_REGS_0);
        if ((gpbr0Data >> 24) == APP_G3_DEVICE_NON_VOLATILE_DATA_KEY_GPBR)
        {
            /* Valid key read from GPBR0 */
            app_g3_deviceData.nonVolatileData.discoverSeqNumber = (uint16_t) gpbr0Data;
            app_g3_deviceData.nonVolatileData.broadcastSeqNumber = (uint8_t) (gpbr0Data >> 16);
            app_g3_deviceData.nonVolatileData.frameCounter = SUPC_GPBRRead(GPBR_REGS_1);
            app_g3_deviceData.nonVolatileData.frameCounterRF = SUPC_GPBRRead(GPBR_REGS_2);
            app_g3_deviceData.writeNonVolatileData = true;
        }
        else
        {
            /* Invalid key read from GPBR0 */
            app_g3_deviceData.writeNonVolatileData = false;
        }
    }

    if (app_g3_deviceData.writeNonVolatileData == true)
    {
        /* Check MAC Frame Counters */
        if (app_g3_deviceData.nonVolatileData.frameCounter == 0xFFFFFFFF)
        {
            /* Invalid MAC PLC Frame Counter */
            app_g3_deviceData.nonVolatileData.frameCounter = 0;
            app_g3_deviceData.writeNonVolatileData = false;
        }

        if (app_g3_deviceData.nonVolatileData.frameCounterRF == 0xFFFFFFFF)
        {
            /* Invalid MAC RF Frame Counter */
            app_g3_deviceData.nonVolatileData.frameCounterRF = 0;
            app_g3_deviceData.writeNonVolatileData = false;
        }
    }

    /* If non-volatile data is valid, related ADP IBs will be set once ADP is
     * ready */
    app_g3_deviceData.validNonVolatileData = app_g3_deviceData.writeNonVolatileData;

    /* Register SUPC power-down callback to write non-volatile data in User
     * Signature */
    SUPC_CallbackRegister(_SUPC_PowerDownCallback, (uintptr_t) &app_g3_deviceData.nonVolatileData);

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
    ADP_SetManagementNotifications(&adpMngNotifications);

    /* Initialize LoWPAN Bootstrapping Protocol (LBP) in Device mode and set
     * call-backs */
    LBP_InitDev();
    lbpDevNotifications.adpNetworkJoinConfirm = _LBP_ADP_NetworkJoinConfirm;
    lbpDevNotifications.adpNetworkLeaveConfirm = NULL;
    lbpDevNotifications.adpNetworkLeaveIndication = _LBP_ADP_NetworkLeaveIndication;
    LBP_SetNotificationsDev(&lbpDevNotifications);

    /* Read UniqueID to set extended address (EUI64) */
    SEFC0_UniqueIdentifierRead((uint32_t*) uniqueId, 4);
    app_g3_deviceData.eui64.value[0] = uniqueId[0] ^ uniqueId[1];
    app_g3_deviceData.eui64.value[1] = uniqueId[2] ^ uniqueId[3];
    app_g3_deviceData.eui64.value[2] = uniqueId[4] ^ uniqueId[5];
    app_g3_deviceData.eui64.value[3] = uniqueId[6] ^ uniqueId[7];
    app_g3_deviceData.eui64.value[4] = uniqueId[8] ^ uniqueId[9];
    app_g3_deviceData.eui64.value[5] = uniqueId[10] ^ uniqueId[11];
    app_g3_deviceData.eui64.value[6] = uniqueId[12] ^ uniqueId[13];
    app_g3_deviceData.eui64.value[7] = uniqueId[14] ^ uniqueId[15];

    /* Place the application's state machine in its initial state. */
    app_g3_deviceData.state = APP_G3_DEVICE_STATE_ADP_OPEN;

    /* Initialize application variables */
    app_g3_deviceData.timerLedHandle = SYS_TIME_HANDLE_INVALID;
    app_g3_deviceData.timerLedExpired = false;
#ifdef APP_G3_CONFORMANCE_TEST
    /* Conformance Test enabled at compilation time.
     * APP_G3_DEVICE_SetConformanceConfig could be used to enable Conformance
     * configuration at execution time. */
    app_g3_deviceData.conformanceTest = true;
    APP_UDP_SetConformanceConfig();
#else
    app_g3_deviceData.conformanceTest = false;
#endif

    SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, APP_G3_DEVICE_STRING_HEADER);
}


/******************************************************************************
  Function:
    void APP_G3_DEVICE_Tasks ( void )

  Remarks:
    See prototype in app_g3_device.h.
 */

void APP_G3_DEVICE_Tasks ( void )
{
    /* Signaling: LED Toggle */
    if (app_g3_deviceData.timerLedExpired == true)
    {
        app_g3_deviceData.timerLedExpired = false;
        USER_BLINK_LED_Toggle();
    }

    /* LBP Device tasks */
    LBP_TasksDev();

    /* Check the application's current state */
    switch ( app_g3_deviceData.state )
    {
        /* Application's initial state. */
        case APP_G3_DEVICE_STATE_ADP_OPEN:
        {
            SRV_PLC_PCOUP_BRANCH plcBranch;
            ADP_PLC_BAND plcBand;

            /* Get configured PLC band */
            plcBranch = SRV_PCOUP_Get_Default_Branch();
            plcBand = SRV_PCOUP_Get_Phy_Band(plcBranch);

            /* Open G3 Adaptation Layer (ADP) */
            ADP_Open(plcBand);

            /* Next state */
            app_g3_deviceData.state = APP_G3_DEVICE_STATE_WAIT_ADP_READY;
        }

        /* State to wait for ADP to be ready */
        case APP_G3_DEVICE_STATE_WAIT_ADP_READY:
        {
            /* Check ADP status */
            ADP_STATUS adpStatus = ADP_Status();
            if (adpStatus >= ADP_STATUS_READY)
            {
                /* ADP is ready. We can set ADP/MAC parameters. */
                _APP_G3_DEVICE_InitializeParameters();

                if (app_g3_deviceData.timerLedHandle == SYS_TIME_HANDLE_INVALID)
                {
                    /* Register timer callback to blink LED */
                    app_g3_deviceData.timerLedHandle = SYS_TIME_CallbackRegisterMS(
                            _APP_G3_TimeExpiredSetFlag, (uintptr_t) &app_g3_deviceData.timerLedExpired,
                            APP_G3_LED_BLINK_PERIOD_MS, SYS_TIME_PERIODIC);
                }

                /* Initialize back-off window for network discovery */
                app_g3_deviceData.backoffWindowLow = APP_G3_DEVICE_DISCOVERY_BACKOFF_LOW_MIN;
                app_g3_deviceData.backoffWindowHigh = APP_G3_DEVICE_DISCOVERY_BACKOFF_HIGH_MIN;

                /* Next state: Start back-off before start network discovery.
                 * Don't break, go directly to next state */
                app_g3_deviceData.state = APP_G3_DEVICE_STATE_START_BACKOFF_DISCOVERY;

                SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "APP_G3_DEVICE: ADP initialized successfully\r\n");
            }
            else
            {
                if (adpStatus == ADP_STATUS_ERROR)
                {
                    SYS_DEBUG_MESSAGE(SYS_ERROR_ERROR, "APP_G3_DEVICE: Error in ADP initialization\r\n");
                    app_g3_deviceData.state = APP_G3_DEVICE_STATE_ERROR;
                }

                break;
            }
        }

        /* State to start back-off delay before start network discovery */
        case APP_G3_DEVICE_STATE_START_BACKOFF_DISCOVERY:
        {
            SYS_TIME_HANDLE timeHandle;
            uint32_t backoffDelay;

            /* Get random back-off delay */
            backoffDelay = SRV_RANDOM_Get32bitsInRange(app_g3_deviceData.backoffWindowLow,
                    app_g3_deviceData.backoffWindowHigh);

            /* Register timer callback for the obtained back-off delay */
            app_g3_deviceData.backoffExpired = false;
            timeHandle = SYS_TIME_CallbackRegisterMS(_APP_G3_TimeExpiredSetFlag,
                    (uintptr_t) &app_g3_deviceData.backoffExpired, backoffDelay, SYS_TIME_SINGLE);
            if (timeHandle != SYS_TIME_HANDLE_INVALID)
            {
                /* Next state: Back-off delay before start network discovery */
                app_g3_deviceData.state = APP_G3_DEVICE_STATE_BACKOFF_DISCOVERY;
                _APP_G3_DEVICE_ResetBeaconLOADngLBPframesReceived();
            }

            break;
        }

        /* Back-off delay state before start network discovery (scan) */
        case APP_G3_DEVICE_STATE_BACKOFF_DISCOVERY:
        {
            if (app_g3_deviceData.backoffExpired == true)
            {
                /* Back-off delay finished. Check if beacon, LoadNG or LBP
                 * frames have been received. */
                if (_APP_G3_DEVICE_CheckBeaconLOADngLBPframes() == false)
                {
                    /* The channel is clean, start network discovery */
                    ADP_DiscoveryRequest(APP_G3_DEVICE_DISCOVERY_DURATION);
                    app_g3_deviceData.state = APP_G3_DEVICE_STATE_SCANNING;

                    /* Initialize best network PAN descriptor */
                    app_g3_deviceData.bestNetwork.panId = 0xFFFF;
                    app_g3_deviceData.bestNetwork.lbaAddress = 0xFFFF;
                    app_g3_deviceData.bestNetwork.rcCoord = 0xFFFF;
                    app_g3_deviceData.bestNetwork.linkQuality = 0xFF;

                    SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, "APP_G3_DEVICE: Network Discovery started\r\n");
                }
                else
                {
                    /* Beacon, LoadNG or LBP frames have been received during
                     * back-off. This means that another devices are trying to
                     * join to the network. Start a new back-off to avoid
                     * network congestion. */
                    app_g3_deviceData.state = APP_G3_DEVICE_STATE_START_BACKOFF_DISCOVERY;

                    /* Make back-off window wider */
                    app_g3_deviceData.backoffWindowLow <<= 1;
                    if (app_g3_deviceData.backoffWindowLow > APP_G3_DEVICE_DISCOVERY_BACKOFF_LOW_MAX)
                    {
                        app_g3_deviceData.backoffWindowLow = APP_G3_DEVICE_DISCOVERY_BACKOFF_LOW_MAX;
                    }

                    app_g3_deviceData.backoffWindowHigh <<= 1;
                    if (app_g3_deviceData.backoffWindowHigh > APP_G3_DEVICE_DISCOVERY_BACKOFF_HIGH_MAX)
                    {
                        app_g3_deviceData.backoffWindowHigh = APP_G3_DEVICE_DISCOVERY_BACKOFF_HIGH_MAX;
                    }
                }
            }

            break;
        }

        /* Network discovery (scan) in progress */
        case APP_G3_DEVICE_STATE_SCANNING:
        {
            /* Nothing to do, state will be changed from _ADP_DiscoveryConfirm
             * callback */
            break;
        }

        /* State to start back-off delay before join to the network */
        case APP_G3_DEVICE_STATE_START_BACKOFF_JOIN:
        {
            SYS_TIME_HANDLE timeHandle;
            uint32_t backoffDelay;

            /* Get random back-off delay */
            backoffDelay = SRV_RANDOM_Get32bitsInRange(app_g3_deviceData.backoffWindowLow,
                    app_g3_deviceData.backoffWindowHigh);

            /* Register timer callback for the obtained back-off delay */
            app_g3_deviceData.backoffExpired = false;
            timeHandle = SYS_TIME_CallbackRegisterMS(_APP_G3_TimeExpiredSetFlag,
                    (uintptr_t) &app_g3_deviceData.backoffExpired, backoffDelay, SYS_TIME_SINGLE);
            if (timeHandle != SYS_TIME_HANDLE_INVALID)
            {
                /* Next state: Back-off delay before join to the network */
                app_g3_deviceData.state = APP_G3_DEVICE_STATE_BACKOFF_JOIN;
                _APP_G3_DEVICE_ResetBeaconLOADngLBPframesReceived();
            }
            break;
        }

        /* Back-off delay state before join to the network */
        case APP_G3_DEVICE_STATE_BACKOFF_JOIN:
        {
            if (app_g3_deviceData.backoffExpired == true)
            {
                /* Back-off delay finished. Check if beacon, LoadNG or LBP
                 * frames have been received. */
                if (_APP_G3_DEVICE_CheckBeaconLOADngLBPframes() == false)
                {
                    /* The channel is clean, try to join to the network */
                    LBP_AdpNetworkJoinRequest(app_g3_deviceData.bestNetwork.panId,
                            app_g3_deviceData.bestNetwork.lbaAddress, app_g3_deviceData.bestNetwork.mediaType);
                    app_g3_deviceData.state = APP_G3_DEVICE_STATE_JOINING;

                    SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, "APP_G3_DEVICE: Network Join started\r\n");
                }
                else
                {
                    /* Beacon, LoadNG or LBP frames have been received during
                     * back-off. This means that another devices are trying to
                     * join to the network. Start a new back-off to avoid
                     * network congestion. */
                    app_g3_deviceData.state = APP_G3_DEVICE_STATE_START_BACKOFF_JOIN;

                    /* Make back-off window wider */
                    app_g3_deviceData.backoffWindowLow <<= 1;
                    if (app_g3_deviceData.backoffWindowLow > APP_G3_DEVICE_JOIN_BACKOFF_LOW_MAX)
                    {
                        app_g3_deviceData.backoffWindowLow = APP_G3_DEVICE_JOIN_BACKOFF_LOW_MAX;
                    }

                    app_g3_deviceData.backoffWindowHigh <<= 1;
                    if (app_g3_deviceData.backoffWindowHigh > APP_G3_DEVICE_JOIN_BACKOFF_HIGH_MAX)
                    {
                        app_g3_deviceData.backoffWindowHigh = APP_G3_DEVICE_JOIN_BACKOFF_HIGH_MAX;
                    }
                }
            }

            break;
        }

        /* Join to the network in progress */
        case APP_G3_DEVICE_STATE_JOINING:
        {
            /* Nothing to do, state will be changed from
             * _LBP_ADP_NetworkJoinConfirm callback */
            break;
        }

        /* Joined to the */
        case APP_G3_DEVICE_STATE_JOINED:
        {
            /* Nothing to do. The device is joined to the network unless
             * _LBP_ADP_NetworkLeaveIndication is called */
            break;
        }

        /* Error state */
        case APP_G3_DEVICE_STATE_ERROR:
            break;

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

void APP_G3_DEVICE_SetConformanceConfig ( void )
{
    app_g3_deviceData.conformanceTest = true;

    if (ADP_Status() >= ADP_STATUS_READY)
    {
        /* ADP is ready: set conformance parameters */
        _APP_G3_SetConformanceParameters();
    }

    APP_UDP_SetConformanceConfig();
}

uint8_t APP_G3_SetConformanceTrickleConfig(uint8_t trickleActivation)
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

uint8_t APP_G3_SetConfigRF(uint8_t* pParameters)
{
    ADP_SET_CFM_PARAMS setConfirm;
    ADP_MAC_GET_CFM_PARAMS macGetConfirm;
    uint16_t freqBandOpMode;
    uint8_t frequencyBand, operatingMode, hoppingActivation;

    /* Check RF interface availability */
    ADP_MacGetRequestSync(MAC_WRP_PIB_MANUF_RF_IFACE_AVAILABLE, 0, &macGetConfirm);
    if (macGetConfirm.status != G3_SUCCESS)
    {
        return 1;
    }

    if (macGetConfirm.attributeValue[0] == 0)
    {
        /* RF interface not available */
        return 2;
    }

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
        /* Invalid/unsupported configuration */
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

void APP_G3_SetContinuousTxRF ( void )
{
    ADP_SET_CFM_PARAMS setConfirm;
    uint8_t continuousTxEnabled = 1;

    ADP_MacSetRequestSync(MAC_WRP_PIB_MANUF_PHY_PARAM_RF, MAC_WRP_RF_PHY_PARAM_SET_CONTINUOUS_TX_MODE,
            1, (const uint8_t*) &continuousTxEnabled, &setConfirm);
}

uint16_t APP_G3_GetShortAddress(void)
{
    return app_g3_deviceData.shortAddress;
}

uint8_t* APP_G3_GetExtendedAddress(void)
{
    return app_g3_deviceData.eui64.value;
}

uint16_t APP_G3_GetPanId(void)
{
    return app_g3_deviceData.panId;
}

/*******************************************************************************
 End of File
 */
