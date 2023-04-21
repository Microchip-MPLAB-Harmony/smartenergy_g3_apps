/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_g3_coordinator.c

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
    This structure should be initialized by the APP_G3_COORDINATOR_Initialize
    function.

    Application strings and buffers are be defined outside this structure.
*/

APP_G3_COORDINATOR_DATA app_g3_coordinatorData;

/* List of extended addresses of joined devices */
ADP_EXTENDED_ADDRESS app_g3_coordinatorExtAddrList[APP_G3_COORDINATOR_MAX_DEVICES];

/* List of extended addresses of blacklisted devices */
ADP_EXTENDED_ADDRESS app_g3_coordinatorBlacklist[APP_G3_COORDINATOR_MAX_DEVICES];

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Constants
// *****************************************************************************
// *****************************************************************************

static const APP_G3_COORDINATOR_CONSTANTS app_g3_coordinatorConst = {
    /* Null extended address */
    .nullAddress.value = {0, 0, 0, 0, 0, 0, 0, 0},

    /* G3 Application parameters */
    .psk = APP_G3_PSK_KEY,
    .gmk = APP_G3_COORDINATOR_GMK_KEY,
    .contextInfoTable0 = APP_G3_CONTEXT_INFO_TABLE_0,
    .contextInfoTable1 = APP_G3_CONTEXT_INFO_TABLE_1,
    .shortAddress = APP_G3_COORDINATOR_SHORT_ADDRESS,
    .routingTableEntryTTL = APP_G3_ROUTING_TABLE_ENTRY_TTL,
    .maxJoinWaitTime = APP_G3_MAX_JOIN_WAIT_TIME,
    .maxHops = APP_G3_MAX_HOPS,
    .dutyCycleLimitRF = APP_G3_COORDINATOR_DUTY_CYCLE_LIMIT_RF,

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

static void _ADP_NetworkStartConfirm(uint8_t status)
{
    if (status == G3_SUCCESS)
    {
        IPV6_ADDR networkPrefix;
        uint8_t prefixData[27];
        ADP_SET_CFM_PARAMS setConfirm;

        /* Network started */
        app_g3_coordinatorData.state = APP_G3_COORDINATOR_STATE_NETWORK_STARTED;
        APP_UDP_NetworkJoined();

        /* Configure Network Prefix in ADP */
        TCPIP_Helper_StringToIPv6Address(APP_UDP_IPV6_NETWORK_PREFIX_G3, &networkPrefix);
        networkPrefix.v[6] = (uint8_t) (APP_G3_COORDINATOR_PAN_ID >> 8);
        networkPrefix.v[7] = (uint8_t) APP_G3_COORDINATOR_PAN_ID;
        prefixData[0] = APP_UDP_IPV6_NETWORK_PREFIX_G3_LEN;
        prefixData[1] = 1; // OnLink flag
        prefixData[2] = 1; // AutonomuosConfiguration flag
        *((uint32_t*) &prefixData[3]) = 2000000; // valid lifetime
        *((uint32_t*) &prefixData[7]) = 2000000; // preferred lifetime
        memcpy(&prefixData[11], &networkPrefix, 16);
        ADP_SetRequestSync(ADP_IB_PREFIX_TABLE, 0, 27, (const uint8_t*) prefixData, &setConfirm);
    }
    else
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_ERROR, "APP_G3_COORDINATOR: Error in ADP Network Start\r\n");
        app_g3_coordinatorData.state = APP_G3_COORDINATOR_STATE_ERROR;
    }
}

static void _ADP_DiscoveryConfirm(uint8_t status)
{
    /* Start G3 network */
    app_g3_coordinatorData.state = APP_G3_COORDINATOR_STATE_STARTING_NETWORK;
    ADP_NetworkStartRequest(APP_G3_COORDINATOR_PAN_ID);
}

static void _ADP_NonVolatileDataIndication(ADP_NON_VOLATILE_DATA_IND_PARAMS* pNonVolatileData)
{
    /* Store non-volatile data to write it in user signature at power-down */
    app_g3_coordinatorData.nonVolatileData = *pNonVolatileData;
    app_g3_coordinatorData.validNonVolatileData = true;

    /* Write non-volatile data data in GPBR in order to read it at non-power-up
     * reset. */
    _APP_G3_WriteNonVolatileDataGPBR();
}

static void _LBP_COORD_JoinRequestIndication(uint8_t* pLbdAddress)
{
    uint16_t assignedAddress = 0xFFFF;

    if (app_g3_coordinatorData.rekey == false)
    {
        bool blacklisted = false;

        /* Check if device is in blacklist */
        for (uint16_t i = 0; i < app_g3_coordinatorData.blacklistSize; i++)
        {
            if (memcmp(pLbdAddress, app_g3_coordinatorBlacklist[i].value, ADP_ADDRESS_64BITS) == 0)
            {
                blacklisted = true;
                break;
            }
        }

        if (blacklisted == false)
        {
            if (app_g3_coordinatorData.numShortAddrAssigned < APP_G3_COORDINATOR_MAX_DEVICES)
            {
                assignedAddress = APP_G3_COORDINATOR_INITIAL_SHORT_ADDRESS +
                        app_g3_coordinatorData.numShortAddrAssigned++;
            }
            else if (app_g3_coordinatorData.numDevicesJoined < APP_G3_COORDINATOR_MAX_DEVICES)
            {
                /* End of the list reached: Go through the list to find free
                 * positions. */
                for (uint16_t i = 0; i < APP_G3_COORDINATOR_MAX_DEVICES; i++)
                {
                    if (memcmp(app_g3_coordinatorExtAddrList[i].value,
                            app_g3_coordinatorConst.nullAddress.value, ADP_ADDRESS_64BITS) == 0)
                    {
                        /* Free position */
                        assignedAddress = APP_G3_COORDINATOR_INITIAL_SHORT_ADDRESS + i;
                        break;
                    }
                }
            }
        }
    }
    else
    {
        /* In case of re-keying, assign any value, except the invalid address,
         * so frame is accepted */
        assignedAddress = 0xFFF0;
    }

    LBP_ShortAddressAssign(pLbdAddress, assignedAddress);
}

static void _LBP_COORD_JoinCompleteIndication(uint8_t* pLbdAddress, uint16_t assignedAddress)
{
    if (app_g3_coordinatorData.rekey == false)
    {
        if ((assignedAddress >= APP_G3_COORDINATOR_INITIAL_SHORT_ADDRESS) &&
                (assignedAddress < (APP_G3_COORDINATOR_INITIAL_SHORT_ADDRESS + APP_G3_COORDINATOR_MAX_DEVICES)))
        {
            uint16_t index = assignedAddress - APP_G3_COORDINATOR_INITIAL_SHORT_ADDRESS;

            /* After the join process finishes, the entry is added to the
             * list */
            memcpy(app_g3_coordinatorExtAddrList[index].value, pLbdAddress, ADP_ADDRESS_64BITS);
            app_g3_coordinatorData.numDevicesJoined++;
        }
    }
    else
    {
        uint16_t shortAddr;
        ADP_EXTENDED_ADDRESS* pExtAddr;
        bool rekeyFinished = true;

        /* Send the next re-keying process */
        for (uint16_t i = app_g3_coordinatorData.rekeyIndex; i < app_g3_coordinatorData.numShortAddrAssigned; i++)
        {
            /* Check not null address */
            if (memcmp(app_g3_coordinatorExtAddrList[i].value,
                    app_g3_coordinatorConst.nullAddress.value, ADP_ADDRESS_64BITS) != 0)
            {
                shortAddr = APP_G3_COORDINATOR_INITIAL_SHORT_ADDRESS + i;
                pExtAddr = &app_g3_coordinatorExtAddrList[i];

                LBP_Rekey(shortAddr, pExtAddr, app_g3_coordinatorData.rekeyPhaseDistribute);
                app_g3_coordinatorData.rekeyIndex = i + 1;
                rekeyFinished = false;
                break;
            }
        }

        if (rekeyFinished == true)
        {
            if (app_g3_coordinatorData.rekeyPhaseDistribute == true)
            {
                /* All devices have been provided with the new GMK. Next phase:
                 * send GMK activation  to all joined devices */
                for (uint16_t i = 0; i < app_g3_coordinatorData.numShortAddrAssigned; i++)
                {
                    if (memcmp(app_g3_coordinatorExtAddrList[i].value,
                            app_g3_coordinatorConst.nullAddress.value, ADP_ADDRESS_64BITS) != 0)
                    {
                        /* First not null address */
                        shortAddr = APP_G3_COORDINATOR_INITIAL_SHORT_ADDRESS + i;
                        pExtAddr = &app_g3_coordinatorExtAddrList[i];
                        LBP_Rekey(shortAddr, pExtAddr, false);
                        app_g3_coordinatorData.rekeyIndex = i + 1;
                        app_g3_coordinatorData.rekeyPhaseDistribute = false;
                        break;
                    }
                }
            }
            else
            {
                /* End of re-keying process */
                LBP_SetRekeyPhase(false);
                LBP_ActivateNewKey();
                app_g3_coordinatorData.rekey = false;
            }
        }
    }
}

static void _LBP_COORD_LeaveIndication(uint16_t networkAddress)
{
    if ((app_g3_coordinatorData.numDevicesJoined > 0) &&
            (networkAddress >= APP_G3_COORDINATOR_INITIAL_SHORT_ADDRESS) &&
            (networkAddress < (APP_G3_COORDINATOR_INITIAL_SHORT_ADDRESS + APP_G3_COORDINATOR_MAX_DEVICES)))
    {
        uint16_t index = networkAddress - APP_G3_COORDINATOR_INITIAL_SHORT_ADDRESS;

        /* Remove the device from the joined devices list */
        memset(app_g3_coordinatorExtAddrList[index].value, 0, ADP_ADDRESS_64BITS);
        app_g3_coordinatorData.numDevicesJoined--;
    }
}

static void _SUPC_PowerDownCallback(uint32_t supc_status, uintptr_t context)
{
    if ((supc_status & SUPC_ISR_VDD3V3SMEV_Msk) != 0)
    {
        /* VDD3V3 supply monitor event */
        if (app_g3_coordinatorData.validNonVolatileData == true)
        {
            uint32_t userSignatureData[APP_G3_NON_VOLATILE_DATA_USER_SIGNATURE_SIZE];

            /* Write non-volatile data in User Signature. Put key in first 32
             * bits. */
            userSignatureData[0] = APP_G3_COORDINATOR_NON_VOLATILE_DATA_KEY_USER_SIGNATURE;
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
    gpbr0Data = app_g3_coordinatorData.nonVolatileData.discoverSeqNumber;
    gpbr0Data += (uint32_t) app_g3_coordinatorData.nonVolatileData.broadcastSeqNumber << 16;
    gpbr0Data += APP_G3_COORDINATOR_NON_VOLATILE_DATA_KEY_GPBR << 24;
    SUPC_GPBRWrite(GPBR_REGS_0, gpbr0Data);
    SUPC_GPBRWrite(GPBR_REGS_1, app_g3_coordinatorData.nonVolatileData.frameCounter);
    SUPC_GPBRWrite(GPBR_REGS_2, app_g3_coordinatorData.nonVolatileData.frameCounterRF);
}

static void _APP_G3_SetConformanceParameters(void)
{
    ADP_SET_CFM_PARAMS setConfirm;

    /* Set ADP parameters needed for Conformance Test */
    ADP_SetRequestSync(ADP_IB_BLACKLIST_TABLE_ENTRY_TTL, 0, 2,
            (const uint8_t*) &app_g3_coordinatorConst.blacklistTableEntryTTLconformance,
            &setConfirm);

    ADP_SetRequestSync(ADP_IB_GROUP_TABLE, 0, 2,
            (const uint8_t*) &app_g3_coordinatorConst.gropTable0Conformance,
            &setConfirm);

    ADP_SetRequestSync(ADP_IB_GROUP_TABLE, 1, 2,
            (const uint8_t*) &app_g3_coordinatorConst.gropTable1Conformance,
            &setConfirm);

    ADP_SetRequestSync(ADP_IB_ROUTING_TABLE_ENTRY_TTL, 0, 2,
            (const uint8_t*) &app_g3_coordinatorConst.routingTableEntryTTLconformance,
            &setConfirm);

    ADP_SetRequestSync(ADP_IB_MAX_JOIN_WAIT_TIME, 0, 2,
            (const uint8_t*) &app_g3_coordinatorConst.maxJoinWaitTimeConformance,
            &setConfirm);

    ADP_SetRequestSync(ADP_IB_DESTINATION_ADDRESS_SET, 0, 2,
            (const uint8_t*) &app_g3_coordinatorConst.destAddrSet0Conformance,
            &setConfirm);

    ADP_SetRequestSync(ADP_IB_MAX_HOPS, 0, 1,
            &app_g3_coordinatorConst.maxHopsConformance, &setConfirm);

    ADP_SetRequestSync(ADP_IB_KR, 0, 1,
            &app_g3_coordinatorConst.krConformance, &setConfirm);

    ADP_SetRequestSync(ADP_IB_KM, 0, 1,
            &app_g3_coordinatorConst.kmConformance, &setConfirm);

    ADP_SetRequestSync(ADP_IB_KC, 0, 1,
            &app_g3_coordinatorConst.kcConformance, &setConfirm);

    ADP_SetRequestSync(ADP_IB_KQ, 0, 1,
            &app_g3_coordinatorConst.kqConformance, &setConfirm);

    ADP_SetRequestSync(ADP_IB_KH, 0, 1,
            &app_g3_coordinatorConst.khConformance, &setConfirm);

    ADP_SetRequestSync(ADP_IB_KRT, 0, 1,
            &app_g3_coordinatorConst.krtConformance, &setConfirm);

    ADP_SetRequestSync(ADP_IB_KQ_RF, 0, 1,
            &app_g3_coordinatorConst.kqRFconformance, &setConfirm);

    ADP_SetRequestSync(ADP_IB_KH_RF, 0, 1,
            &app_g3_coordinatorConst.khRFconformance, &setConfirm);

    ADP_SetRequestSync(ADP_IB_KRT_RF, 0, 1,
            &app_g3_coordinatorConst.krtRFconformance, &setConfirm);

    ADP_SetRequestSync(ADP_IB_KDC_RF, 0, 1,
            &app_g3_coordinatorConst.kdcRFconformance, &setConfirm);

    /* Set MAC parameters needed for Conformance Test */
    ADP_MacSetRequestSync(MAC_WRP_PIB_TMR_TTL, 0, 1,
            &app_g3_coordinatorConst.tmrTTLconformance, &setConfirm);

    ADP_MacSetRequestSync(MAC_WRP_PIB_MAX_CSMA_BACKOFFS_RF, 0, 1,
            &app_g3_coordinatorConst.maxCSMAbackoffsRFconformance, &setConfirm);

    ADP_MacSetRequestSync(MAC_WRP_PIB_MAX_FRAME_RETRIES_RF, 0, 1,
            &app_g3_coordinatorConst.maxFrameRetriesRFconformance, &setConfirm);

    ADP_MacSetRequestSync(MAC_WRP_PIB_POS_TABLE_ENTRY_TTL, 0, 1,
            &app_g3_coordinatorConst.posTableEntryTTLconformance, &setConfirm);
}

static void _APP_G3_COORDINATOR_InitializeParameters(void)
{
    ADP_SET_CFM_PARAMS setConfirm;
    LBP_SET_PARAM_CONFIRM lbpSetConfirm;
    uint8_t contextInfo0[sizeof(app_g3_coordinatorConst.contextInfoTable0)];

    /* Set extended address (EUI64). It must be unique for each device. */
    ADP_MacSetRequestSync(MAC_WRP_PIB_MANUF_EXTENDED_ADDRESS, 0, 8,
            (const uint8_t*) app_g3_coordinatorData.eui64.value, &setConfirm);

    /* Set short address for coordinator (0x0000) */
    ADP_MacSetRequestSync(MAC_WRP_PIB_SHORT_ADDRESS, 0, 2,
            (const uint8_t*) &app_g3_coordinatorConst.shortAddress, &setConfirm);

    /* Set user-specific ADP parameters */
    memcpy(contextInfo0, app_g3_coordinatorConst.contextInfoTable0,
            sizeof(app_g3_coordinatorConst.contextInfoTable0) - 2);
    contextInfo0[sizeof(app_g3_coordinatorConst.contextInfoTable0) - 2] = APP_G3_COORDINATOR_PAN_ID >> 8;
    contextInfo0[sizeof(app_g3_coordinatorConst.contextInfoTable0) - 1] = (uint8_t) APP_G3_COORDINATOR_PAN_ID;
    ADP_SetRequestSync(ADP_IB_CONTEXT_INFORMATION_TABLE, 0, 14,
            app_g3_coordinatorConst.contextInfoTable0, &setConfirm);

    ADP_SetRequestSync(ADP_IB_CONTEXT_INFORMATION_TABLE, 1, 10,
            app_g3_coordinatorConst.contextInfoTable1, &setConfirm);

    /* Set user-specific MAC parameters */
    ADP_MacSetRequestSync(MAC_WRP_PIB_DUTY_CYCLE_LIMIT_RF, 0, 2,
            (const uint8_t*) &app_g3_coordinatorConst.dutyCycleLimitRF, &setConfirm);

    if (app_g3_coordinatorData.conformanceTest == true)
    {
        _APP_G3_SetConformanceParameters();
    }
    else
    {
        /* Set user-specific ADP parameters that are set to different values in
         * Conformance Test */
        ADP_SetRequestSync(ADP_IB_ROUTING_TABLE_ENTRY_TTL, 0, 2,
            (const uint8_t*) &app_g3_coordinatorConst.routingTableEntryTTL,
            &setConfirm);

        ADP_SetRequestSync(ADP_IB_MAX_JOIN_WAIT_TIME, 0, 2,
            (const uint8_t*) &app_g3_coordinatorConst.maxJoinWaitTime, &setConfirm);

        ADP_SetRequestSync(ADP_IB_MAX_HOPS, 0, 1,
            &app_g3_coordinatorConst.maxHops, &setConfirm);
    }

    if (app_g3_coordinatorData.writeNonVolatileData == true)
    {
        /* Set ADP/MAC non-volatile data parameters */
        ADP_MacSetRequestSync(MAC_WRP_PIB_FRAME_COUNTER, 0, 4,
            (const uint8_t*) &app_g3_coordinatorData.nonVolatileData.frameCounter,
            &setConfirm);

        ADP_MacSetRequestSync(MAC_WRP_PIB_FRAME_COUNTER_RF, 0, 4,
            (const uint8_t*) &app_g3_coordinatorData.nonVolatileData.frameCounterRF,
            &setConfirm);

        ADP_SetRequestSync(ADP_IB_MANUF_DISCOVER_SEQUENCE_NUMBER, 0, 2,
            (const uint8_t*) &app_g3_coordinatorData.nonVolatileData.discoverSeqNumber,
            &setConfirm);

        ADP_SetRequestSync(ADP_IB_MANUF_BROADCAST_SEQUENCE_NUMBER, 0, 1,
            (const uint8_t*) &app_g3_coordinatorData.nonVolatileData.broadcastSeqNumber,
            &setConfirm);

        /* Not needed to set ADP/MAC non-volatile data parameters anymore
         * because they are internally restored when ADP reset is performed */
        app_g3_coordinatorData.writeNonVolatileData = false;
    }

    /* Set PSK and GMK keys */
    LBP_SetParamCoord(LBP_IB_PSK, 0, 16, (const uint8_t*) &app_g3_coordinatorConst.psk, &lbpSetConfirm);
    LBP_SetParamCoord(LBP_IB_GMK, 0, 16, (const uint8_t*) &app_g3_coordinatorConst.gmk, &lbpSetConfirm);
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_G3_COORDINATOR_Initialize ( void )

  Remarks:
    See prototype in app_g3_coordinator.h.
 */

void APP_G3_COORDINATOR_Initialize ( void )
{
    ADP_MANAGEMENT_NOTIFICATIONS adpMngNotifications;
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
        if (userSignatureData[0] == APP_G3_COORDINATOR_NON_VOLATILE_DATA_KEY_USER_SIGNATURE)
        {
            /* Valid key read from User Signature */
            app_g3_coordinatorData.writeNonVolatileData = true;
            memcpy(&app_g3_coordinatorData.nonVolatileData, &userSignatureData[1],
                    sizeof(ADP_NON_VOLATILE_DATA_IND_PARAMS));

            /* Write non-volatile data in GPBR */
            _APP_G3_WriteNonVolatileDataGPBR();
        }
        else
        {
            /* Invalid key read from User Signature */
            app_g3_coordinatorData.writeNonVolatileData = false;
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
        if ((gpbr0Data >> 24) == APP_G3_COORDINATOR_NON_VOLATILE_DATA_KEY_GPBR)
        {
            /* Valid key read from GPBR0 */
            app_g3_coordinatorData.nonVolatileData.discoverSeqNumber = (uint16_t) gpbr0Data;
            app_g3_coordinatorData.nonVolatileData.broadcastSeqNumber = (uint8_t) (gpbr0Data >> 16);
            app_g3_coordinatorData.nonVolatileData.frameCounter = SUPC_GPBRRead(GPBR_REGS_1);
            app_g3_coordinatorData.nonVolatileData.frameCounterRF = SUPC_GPBRRead(GPBR_REGS_2);
            app_g3_coordinatorData.writeNonVolatileData = true;
        }
        else
        {
            /* Invalid key read from GPBR0 */
            app_g3_coordinatorData.writeNonVolatileData = false;
        }
    }

    if (app_g3_coordinatorData.writeNonVolatileData == true)
    {
        /* Check MAC Frame Counters */
        if (app_g3_coordinatorData.nonVolatileData.frameCounter == 0xFFFFFFFF)
        {
            /* Invalid MAC PLC Frame Counter */
            app_g3_coordinatorData.nonVolatileData.frameCounter = 0;
            app_g3_coordinatorData.writeNonVolatileData = false;
        }

        if (app_g3_coordinatorData.nonVolatileData.frameCounterRF == 0xFFFFFFFF)
        {
            /* Invalid MAC RF Frame Counter */
            app_g3_coordinatorData.nonVolatileData.frameCounterRF = 0;
            app_g3_coordinatorData.writeNonVolatileData = false;
        }
    }

    /* If non-volatile data is valid, related ADP IBs will be set once ADP is
     * ready */
    app_g3_coordinatorData.validNonVolatileData = app_g3_coordinatorData.writeNonVolatileData;

    /* Register SUPC power-down callback to write non-volatile data in User
     * Signature */
    SUPC_CallbackRegister(_SUPC_PowerDownCallback, (uintptr_t) &app_g3_coordinatorData.nonVolatileData);

    /* Set ADP management call-backs */
    adpMngNotifications.discoveryConfirm = _ADP_DiscoveryConfirm;
    adpMngNotifications.discoveryIndication = NULL;
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
    ADP_SetManagementNotifications(&adpMngNotifications);

    /* Read UniqueID to set extended address (EUI64) */
    SEFC0_UniqueIdentifierRead((uint32_t*) uniqueId, 4);
    app_g3_coordinatorData.eui64.value[0] = uniqueId[0] ^ uniqueId[1];
    app_g3_coordinatorData.eui64.value[1] = uniqueId[2] ^ uniqueId[3];
    app_g3_coordinatorData.eui64.value[2] = uniqueId[4] ^ uniqueId[5];
    app_g3_coordinatorData.eui64.value[3] = uniqueId[6] ^ uniqueId[7];
    app_g3_coordinatorData.eui64.value[4] = uniqueId[8] ^ uniqueId[9];
    app_g3_coordinatorData.eui64.value[5] = uniqueId[10] ^ uniqueId[11];
    app_g3_coordinatorData.eui64.value[6] = uniqueId[12] ^ uniqueId[13];
    app_g3_coordinatorData.eui64.value[7] = uniqueId[14] ^ uniqueId[15];

    /* Place the application's state machine in its initial state. */
    app_g3_coordinatorData.state = APP_G3_COORDINATOR_STATE_ADP_OPEN;

    /* Initialize application variables */
    memset(app_g3_coordinatorExtAddrList, 0, sizeof(app_g3_coordinatorExtAddrList));
    memset(app_g3_coordinatorBlacklist, 0, sizeof(app_g3_coordinatorBlacklist));
    app_g3_coordinatorData.timerLedHandle = SYS_TIME_HANDLE_INVALID;
    app_g3_coordinatorData.timerLedExpired = false;
    app_g3_coordinatorData.numShortAddrAssigned = 0;
    app_g3_coordinatorData.numDevicesJoined = 0;
    app_g3_coordinatorData.rekey = false;
#ifdef APP_G3_CONFORMANCE_TEST
    /* Conformance Test enabled at compilation time.
     * APP_G3_COORDINATOR_SetConformanceConfig could be used to enable
     * Conformance configuration at execution time. */
    app_g3_coordinatorData.conformanceTest = true;
    APP_UDP_SetConformanceConfig();
#else
    app_g3_coordinatorData.conformanceTest = false;
#endif

    SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, APP_G3_COORDINATOR_STRING_HEADER);
}


/******************************************************************************
  Function:
    void APP_G3_COORDINATOR_Tasks ( void )

  Remarks:
    See prototype in app_g3_coordinator.h.
 */

void APP_G3_COORDINATOR_Tasks ( void )
{
    /* Signaling: LED Toggle */
    if (app_g3_coordinatorData.timerLedExpired == true)
    {
        app_g3_coordinatorData.timerLedExpired = false;
        USER_BLINK_LED_Toggle();
    }

    /* LBP Coordinator tasks */
    LBP_UpdateLbpSlots();

    /* Check the application's current state */
    switch ( app_g3_coordinatorData.state )
    {
        /* Application's initial state. */
        case APP_G3_COORDINATOR_STATE_ADP_OPEN:
        {
            SRV_PLC_PCOUP_BRANCH plcBranch;
            ADP_PLC_BAND plcBand;
            LBP_NOTIFICATIONS_COORD lbpCoordNotifications;

            /* Get configured PLC band */
            plcBranch = SRV_PCOUP_Get_Default_Branch();
            plcBand = SRV_PCOUP_Get_Phy_Band(plcBranch);

            /* Open G3 Adaptation Layer (ADP) */
            ADP_Open(plcBand);

            /* Initialize LoWPAN Bootstrapping Protocol (LBP) in Coordinator
             * mode and set call-backs */
            LBP_InitCoord(plcBand == ADP_BAND_ARIB);
            lbpCoordNotifications.joinRequestIndication = _LBP_COORD_JoinRequestIndication;
            lbpCoordNotifications.joinCompleteIndication = _LBP_COORD_JoinCompleteIndication;
            lbpCoordNotifications.leaveIndication = _LBP_COORD_LeaveIndication;
            LBP_SetNotificationsCoord(&lbpCoordNotifications);

            /* Next state */
            app_g3_coordinatorData.state = APP_G3_COORDINATOR_STATE_WAIT_ADP_READY;
        }

        /* State to wait for ADP to be ready */
        case APP_G3_COORDINATOR_STATE_WAIT_ADP_READY:
        {
            /* Check ADP status */
            ADP_STATUS adpStatus = ADP_Status();
            if (adpStatus >= ADP_STATUS_READY)
            {
                /* ADP is ready. We can set ADP/MAC parameters. */
                _APP_G3_COORDINATOR_InitializeParameters();

                if (app_g3_coordinatorData.timerLedHandle == SYS_TIME_HANDLE_INVALID)
                {
                    /* Register timer callback to blink LED */
                    app_g3_coordinatorData.timerLedHandle = SYS_TIME_CallbackRegisterMS(
                            _APP_G3_TimeExpiredSetFlag, (uintptr_t) &app_g3_coordinatorData.timerLedExpired,
                            APP_G3_LED_BLINK_PERIOD_MS, SYS_TIME_PERIODIC);
                }

                SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "APP_G3_COORDINATOR: ADP initialized successfully\r\n");

                if (app_g3_coordinatorData.conformanceTest == true)
                {
                    /* In conformance, send beacon request before starting
                     * network */
                    app_g3_coordinatorData.state = APP_G3_COORDINATOR_STATE_SCANNING;
                    ADP_DiscoveryRequest(15);
                }
                else
                {
                    /* Start G3 network */
                    app_g3_coordinatorData.state = APP_G3_COORDINATOR_STATE_STARTING_NETWORK;
                    ADP_NetworkStartRequest(APP_G3_COORDINATOR_PAN_ID);
                }
            }
            else
            {
                if (adpStatus == ADP_STATUS_ERROR)
                {
                    SYS_DEBUG_MESSAGE(SYS_ERROR_ERROR, "APP_G3_COORDINATOR: Error in ADP initialization\r\n");
                    app_g3_coordinatorData.state = APP_G3_COORDINATOR_STATE_ERROR;
                }

                break;
            }
        }

        /* Network discovery (scan) in progress (conformance) */
        case APP_G3_COORDINATOR_STATE_SCANNING:
        {
            /* Nothing to do, state will be changed from _ADP_DiscoveryConfirm
             * callback */
            break;
        }

        /* Starting G3 network */
        case APP_G3_COORDINATOR_STATE_STARTING_NETWORK:
        {
            /* Nothing to do, state will be changed from
             * _ADP_NetworkStartConfirm callback */
            break;
        }

        /* G3 network started, accepting connection of devices */
        case APP_G3_COORDINATOR_STATE_NETWORK_STARTED:
        {
            /* Nothing to do, list of devices maintained in LBP Coordinator
             * call-backs */
            break;
        }

        /* Error state */
        case APP_G3_COORDINATOR_STATE_ERROR:
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

void APP_G3_COORDINATOR_SetConformanceConfig ( void )
{
    app_g3_coordinatorData.conformanceTest = true;

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
    return APP_G3_COORDINATOR_SHORT_ADDRESS;
}

uint8_t* APP_G3_GetExtendedAddress(void)
{
    return app_g3_coordinatorData.eui64.value;
}

uint16_t APP_G3_GetPanId(void)
{
    return APP_G3_COORDINATOR_PAN_ID;
}

void APP_G3_COORDINATOR_LaunchRekeying(void)
{
    if (app_g3_coordinatorData.numDevicesJoined > 0)
    {
        /* Start the re-keying process */
        app_g3_coordinatorData.rekey = true;
        app_g3_coordinatorData.rekeyPhaseDistribute = true;
        LBP_SetRekeyPhase(true);

        /* Send the first re-keying process */
        for (uint16_t i = 0; i < app_g3_coordinatorData.numShortAddrAssigned; i++)
        {
            if (memcmp(app_g3_coordinatorExtAddrList[i].value,
                    app_g3_coordinatorConst.nullAddress.value, ADP_ADDRESS_64BITS) != 0)
            {
                /* First not null address */
                LBP_Rekey(APP_G3_COORDINATOR_INITIAL_SHORT_ADDRESS + i, &app_g3_coordinatorExtAddrList[i], true);
                app_g3_coordinatorData.rekeyIndex = i + 1;
                break;
            }
        }
    }
}

void APP_G3_COORDINATOR_KickDevice(uint16_t shortAddress)
{
    if ((app_g3_coordinatorData.numDevicesJoined > 0) &&
            (shortAddress >= APP_G3_COORDINATOR_INITIAL_SHORT_ADDRESS) &&
            (shortAddress < (APP_G3_COORDINATOR_INITIAL_SHORT_ADDRESS + APP_G3_COORDINATOR_MAX_DEVICES)))
    {
        uint16_t index = shortAddress - APP_G3_COORDINATOR_INITIAL_SHORT_ADDRESS;
        ADP_EXTENDED_ADDRESS* pExtAddr = &app_g3_coordinatorExtAddrList[index];

        /* Check if device is in the list */
        if (memcmp(pExtAddr->value, app_g3_coordinatorConst.nullAddress.value, ADP_ADDRESS_64BITS) != 0)
        {
            if (LBP_KickDevice(shortAddress, pExtAddr) == true)
            {
                /* Remove the device from the joined devices list */
                memset(pExtAddr->value, 0, ADP_ADDRESS_64BITS);
            }
        }
    }
}

void APP_G3_COORDINATOR_AddToBlacklist(uint8_t* extendedAddress)
{
    if (app_g3_coordinatorData.blacklistSize < APP_G3_COORDINATOR_MAX_DEVICES)
    {
        memcpy(app_g3_coordinatorBlacklist[app_g3_coordinatorData.blacklistSize++].value,
                extendedAddress, ADP_ADDRESS_64BITS);
    }
}

uint16_t APP_G3_COORDINATOR_GetNumDevicesJoined(void)
{
    return app_g3_coordinatorData.numDevicesJoined;
}

uint16_t APP_G3_COORDINATOR_GetDeviceAddress(uint16_t index, uint8_t* pEUI64)
{
    uint16_t currentIndex = 0;

    for (uint16_t i = app_g3_coordinatorData.rekeyIndex; i < app_g3_coordinatorData.numShortAddrAssigned; i++)
    {
        /* Check not null address */
        if (memcmp(app_g3_coordinatorExtAddrList[i].value,
                app_g3_coordinatorConst.nullAddress.value, ADP_ADDRESS_64BITS) != 0)
        {
            if (currentIndex == index)
            {
                memcpy(pEUI64, app_g3_coordinatorExtAddrList[i].value, ADP_ADDRESS_64BITS);
                return APP_G3_COORDINATOR_INITIAL_SHORT_ADDRESS + i;
            }
            else
            {
                currentIndex++;
            }
        }
    }

    return 0xFFFF;
}

/*******************************************************************************
 End of File
 */
