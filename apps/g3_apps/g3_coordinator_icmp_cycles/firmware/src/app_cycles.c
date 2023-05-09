/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_cycles.c

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
    This structure should be initialized by the APP_CYCLES_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_CYCLES_DATA app_cyclesData;

static APP_CYCLES_STATISTICS_ENTRY app_cyclesStatistics[APP_EAP_SERVER_MAX_DEVICES];

// *****************************************************************************
// *****************************************************************************
// Section: Function Declaration
// *****************************************************************************
// *****************************************************************************

static void _APP_CYCLES_NextPacket(void);

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

static void _APP_CYCLES_IcmpCallback (
    TCPIP_NET_HANDLE hNetIf,
    uint8_t type,
    const IPV6_ADDR * localIP,
    const IPV6_ADDR * remoteIP,
    void * header)
{
    if ((type == ICMPV6_INFO_ECHO_REPLY) && (memcmp(remoteIP, &app_cyclesData.targetAddress, sizeof(IPV6_ADDR)) == 0))
    {
        uint64_t elapsedTimeCount;
        uint64_t currentTimeCount = SYS_TIME_Counter64Get();

        /* Echo reply received successfully */
        SYS_TIME_TimerDestroy(app_cyclesData.timeHandle);

        /* Update statistics */
        app_cyclesData.numEchoReplies++;
        elapsedTimeCount = currentTimeCount - app_cyclesData.timeCountEchoRequest;
        app_cyclesData.pStatsEntry->timeCountTotal += elapsedTimeCount;
        app_cyclesData.pStatsEntry->numEchoReplies++;

        SYS_DEBUG_PRINT(SYS_ERROR_DEBUG, "APP_CYCLES: ICMPv6 echo reply received (%u ms)\r\n",
                SYS_TIME_CountToMS(elapsedTimeCount));

        /* Next ICMPv6 echo request */
        _APP_CYCLES_NextPacket();
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

static void _APP_CYCLES_SendPacket(void)
{
    /* Send ICMPv6 echo request */
    app_cyclesData.timeCountEchoRequest = SYS_TIME_Counter64Get();
    app_cyclesData.icmpResult = TCPIP_ICMPV6_EchoRequestSend(app_cyclesData.netHandle, &app_cyclesData.targetAddress,
            app_cyclesData.sequenceNumber, 0, app_cyclesData.packetSize);

    /* Force neighbor reachable status in NDP */
    TCPIP_NDP_NborReachConfirm(app_cyclesData.netHandle, &app_cyclesData.targetAddress);

    SYS_DEBUG_PRINT(SYS_ERROR_DEBUG, "APP_CYCLES: Ping packet size %hu\r\n", app_cyclesData.packetSize);

    if (app_cyclesData.icmpResult == true)
    {
        /* Create timer for timeout to detect echo reply not received */
        app_cyclesData.timeExpired = false;
        app_cyclesData.timeHandle = SYS_TIME_CallbackRegisterMS(APP_SYS_TIME_CallbackSetFlag,
                (uintptr_t) &app_cyclesData.timeExpired, APP_CYCLES_TIMEOUT_MS,
                SYS_TIME_SINGLE);

        /* Update statistics */
        app_cyclesData.numEchoRequests++;
        app_cyclesData.pStatsEntry->numEchoRequests++;
    }
    else
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_ERROR, "APP_CYCLES: Error in TCPIP_ICMPV6_EchoRequestSend\r\n");
    }
}

static void _APP_CYCLES_StartDeviceCycle(void)
{
    APP_CYCLES_STATISTICS_ENTRY* pFreeStatsEntry = NULL;
    APP_CYCLES_STATISTICS_ENTRY* pStatsEntry = NULL;
    uint16_t shortAddress, panId;
    uint8_t eui64[8];
    char targetAddressString[50 + 1];

    /* Create link-local address based on short address and PAN ID */
    shortAddress = APP_EAP_SERVER_GetDeviceAddress(app_cyclesData.deviceIndex, eui64);
    panId = APP_G3_MANAGEMENT_GetPanId();
    TCPIP_Helper_StringToIPv6Address(APP_TCPIP_IPV6_LINK_LOCAL_ADDRESS_G3, &app_cyclesData.targetAddress);
    app_cyclesData.targetAddress.v[8] = (uint8_t) (panId >> 8);
    app_cyclesData.targetAddress.v[9] = (uint8_t) panId;
    app_cyclesData.targetAddress.v[14] = (uint8_t) (shortAddress >> 8);
    app_cyclesData.targetAddress.v[15] = (uint8_t) shortAddress;
    TCPIP_Helper_IPv6AddressToString(&app_cyclesData.targetAddress, targetAddressString, sizeof(targetAddressString) - 1);

    SYS_DEBUG_PRINT(SYS_ERROR_DEBUG, "APP_CYCLES: Starting cycle for %s (Short Address: 0x%04X,"
            " EUI64: 0x%02X%02X%02X%02X%02X%02X%02X%02X)\r\n", targetAddressString, shortAddress,
            eui64[0], eui64[1], eui64[2], eui64[3], eui64[4], eui64[5], eui64[6], eui64[7]);

    /* Look for statistics entry */
    for (uint16_t i = 0; i < APP_EAP_SERVER_MAX_DEVICES; i++)
    {
        if (app_cyclesStatistics[i].shortAddress == shortAddress)
        {
            /* Statistics entry matching short address found */
            pStatsEntry = &app_cyclesStatistics[i];
            break;
        }
        else if ((pFreeStatsEntry == NULL) && (app_cyclesStatistics[i].shortAddress == 0xFFFF))
        {
            /* Free statistics entry found */
            pFreeStatsEntry = &app_cyclesStatistics[i];
        }
    }

    if (pStatsEntry == NULL)
    {
        pStatsEntry = pFreeStatsEntry;
        pStatsEntry->shortAddress = shortAddress;
    }

    app_cyclesData.pStatsEntry = pStatsEntry;
}

static void _APP_CYCLES_StartCycle(void)
{
    app_cyclesData.deviceIndex = 0;
    app_cyclesData.packetSize = APP_CYCLES_PACKET_SIZE_1;
    app_cyclesData.numDevicesJoined = APP_EAP_SERVER_GetNumDevicesJoined();
    SYS_DEBUG_PRINT(SYS_ERROR_INFO, "APP_CYCLES: Starting cycle %u. %hu nodes in cycle\r\n",
            app_cyclesData.cycleIndex, app_cyclesData.numDevicesJoined);
    app_cyclesData.timeCountCycleStart = SYS_TIME_Counter64Get();
}

static void _APP_CYCLES_NextPacket(void)
{
    if (app_cyclesData.packetSize == APP_CYCLES_PACKET_SIZE_1)
    {
        /* Next size */
        app_cyclesData.packetSize = APP_CYCLES_PACKET_SIZE_2;
    }
    else if (app_cyclesData.packetSize == APP_CYCLES_PACKET_SIZE_2)
    {
        /* Next size */
        app_cyclesData.packetSize = APP_CYCLES_PACKET_SIZE_3;
    }
    else if (app_cyclesData.packetSize == APP_CYCLES_PACKET_SIZE_3)
    {
        /* Next device */
        app_cyclesData.packetSize = APP_CYCLES_PACKET_SIZE_1;
        app_cyclesData.deviceIndex++;
        if (app_cyclesData.deviceIndex == app_cyclesData.numDevicesJoined)
        {
            uint32_t numErrors;
            uint8_t successRate;
            uint64_t currentTimeCount = SYS_TIME_Counter64Get();
            uint64_t elapsedTimeCount = currentTimeCount - app_cyclesData.timeCountCycleStart;
            uint64_t elapsedTimeCountTotal = currentTimeCount - app_cyclesData.timeCountFirstCycleStart;
            SYS_DEBUG_PRINT(SYS_ERROR_INFO, "APP_CYCLES: Cycle %u finished. Duration %u ms "
                    "(average total: %u ms; average device: %u ms)\r\n",
                    app_cyclesData.cycleIndex, SYS_TIME_CountToMS(elapsedTimeCount),
                    SYS_TIME_CountToMS(elapsedTimeCountTotal / (app_cyclesData.cycleIndex + 1)),
                    SYS_TIME_CountToMS(elapsedTimeCount / app_cyclesData.numDevicesJoined));

            SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "APP_CYCLES: Summary\r\n");

            /* Print statistics */
            for (uint16_t i = 0; i < APP_EAP_SERVER_MAX_DEVICES; i++)
            {
                if (app_cyclesStatistics[i].shortAddress != 0xFFFF)
                {
                    numErrors = app_cyclesStatistics[i].numEchoRequests - app_cyclesStatistics[i].numEchoReplies;
                    successRate = (app_cyclesStatistics[i].numEchoReplies * 100) / app_cyclesStatistics[i].numEchoRequests;
                    SYS_DEBUG_PRINT(SYS_ERROR_INFO, "Short address 0x%04X: Sent %u, Success %u (%hhu %%), Errors %u,"
                            " Average duration %u\r\n", app_cyclesStatistics[i].shortAddress,
                            app_cyclesStatistics[i].numEchoRequests, app_cyclesStatistics[i].numEchoReplies,
                            successRate, numErrors,
                            SYS_TIME_CountToMS(app_cyclesStatistics[i].timeCountTotal / (app_cyclesData.cycleIndex + 1)));
                }
            }

            numErrors = app_cyclesData.numEchoRequests - app_cyclesData.numEchoReplies;
            successRate = (app_cyclesData.numEchoReplies * 100) / app_cyclesData.numEchoRequests;
            SYS_DEBUG_PRINT(SYS_ERROR_INFO, "TOTAL: Sent %u, Success %u (%hhu %%), Errors %u\r\n",
                    app_cyclesData.numEchoRequests, app_cyclesData.numEchoReplies, successRate, numErrors);

            /* Start again with first device */
            app_cyclesData.sequenceNumber++;
            app_cyclesData.cycleIndex++;
            _APP_CYCLES_StartCycle();
        }

        _APP_CYCLES_StartDeviceCycle();
    }

    _APP_CYCLES_SendPacket();
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_CYCLES_Initialize ( void )

  Remarks:
    See prototype in app_cycles.h.
 */

void APP_CYCLES_Initialize ( void )
{
    /* Place the application's state machine in its initial state. */
    app_cyclesData.state = APP_CYCLES_STATE_WAIT_TCPIP_READY;

    /* Initialize application variables */
    app_cyclesData.numEchoRequests = 0;
    app_cyclesData.numEchoReplies = 0;
    app_cyclesData.cycleIndex = 0;
    app_cyclesData.numDevicesJoined = 0;
    app_cyclesData.sequenceNumber = 0;
    app_cyclesData.timeExpired = false;
    for (uint16_t i = 0; i < APP_EAP_SERVER_MAX_DEVICES; i++)
    {
        app_cyclesStatistics[i].timeCountTotal = 0;
        app_cyclesStatistics[i].numEchoRequests = 0;
        app_cyclesStatistics[i].numEchoReplies = 0;
        app_cyclesStatistics[i].shortAddress = 0xFFFF;
    }
}


/******************************************************************************
  Function:
    void APP_CYCLES_Tasks ( void )

  Remarks:
    See prototype in app_cycles.h.
 */

void APP_CYCLES_Tasks ( void )
{
    /* Check the application's current state. */
    switch ( app_cyclesData.state )
    {
        /* Application's initial state. */
        case APP_CYCLES_STATE_WAIT_TCPIP_READY:
        {
            SYS_STATUS tcpipStat = TCPIP_STACK_Status(sysObj.tcpip);
            if (tcpipStat <= SYS_STATUS_ERROR)
            {
                SYS_DEBUG_MESSAGE(SYS_ERROR_ERROR, "APP_CYCLES: TCP/IP stack initialization failed!\r\n");
                app_cyclesData.state = APP_CYCLES_STATE_ERROR;
            }
            else if(tcpipStat == SYS_STATUS_READY)
            {
                IPV6_ADDR ipv6Addr;
                uint8_t* eui64;
                uint16_t panId;

                /* TCP/IP stack ready. Register ICMPv6 callback */
                TCPIP_ICMPV6_CallbackRegister(_APP_CYCLES_IcmpCallback);
                app_cyclesData.netHandle = TCPIP_STACK_NetHandleGet("G3ADPMAC");
                app_cyclesData.state = APP_CYCLES_STATE_WAIT_FIRST_JOIN;

                /* Configure link-local address, based on PAN ID and Short
                 * Address */
                TCPIP_Helper_StringToIPv6Address(APP_TCPIP_IPV6_LINK_LOCAL_ADDRESS_G3, &ipv6Addr);
                panId = APP_G3_MANAGEMENT_GetPanId();
                ipv6Addr.v[8] = (uint8_t) (panId >> 8);
                ipv6Addr.v[9] = (uint8_t) panId;
                ipv6Addr.v[14] = (uint8_t) (APP_G3_MANAGEMENT_SHORT_ADDRESS >> 8);
                ipv6Addr.v[15] = (uint8_t) APP_G3_MANAGEMENT_SHORT_ADDRESS;
                TCPIP_IPV6_UnicastAddressAdd(app_cyclesData.netHandle,
                        &ipv6Addr, 0, false);

                /* Configure Unique Local Link (ULA) address, based on PAN ID
                 * and EUI64 */
                TCPIP_Helper_StringToIPv6Address(APP_TCPIP_IPV6_NETWORK_PREFIX_G3, &ipv6Addr);
                eui64 = APP_G3_MANAGEMENT_GetExtendedAddress();
                ipv6Addr.v[6] = (uint8_t) (panId >> 8);
                ipv6Addr.v[7] = (uint8_t) panId;
                ipv6Addr.v[8] = eui64[7];
                ipv6Addr.v[9] = eui64[6];
                ipv6Addr.v[10] = eui64[5];
                ipv6Addr.v[11] = eui64[4];
                ipv6Addr.v[12] = eui64[3];
                ipv6Addr.v[13] = eui64[2];
                ipv6Addr.v[14] = eui64[1];
                ipv6Addr.v[15] = eui64[0];
                TCPIP_IPV6_UnicastAddressAdd(app_cyclesData.netHandle,
                        &ipv6Addr, APP_TCPIP_IPV6_NETWORK_PREFIX_G3_LEN, false);
            }

            break;
        }

        /* State to wait for the first device joined to the G3 network */
        case APP_CYCLES_STATE_WAIT_FIRST_JOIN:
        {
            /* Get number of joined devices */
            app_cyclesData.numDevicesJoined = APP_EAP_SERVER_GetNumDevicesJoined();

            if (app_cyclesData.numDevicesJoined > 0)
            {
                /* First device joined the G3 network. Start timer to wait
                 * before next cycle */
                app_cyclesData.state = APP_CYCLES_STATE_WAIT_FIRST_CYCLE;
                app_cyclesData.timeHandle = SYS_TIME_CallbackRegisterMS(APP_SYS_TIME_CallbackSetFlag,
                        (uintptr_t) &app_cyclesData.timeExpired, APP_CYCLES_TIME_WAIT_CYCLE_MS,
                        SYS_TIME_SINGLE);
            }

            break;
        }

        /* State to wait for first ICMPv6 cycle */
        case APP_CYCLES_STATE_WAIT_FIRST_CYCLE:
        {
            if (app_cyclesData.timeExpired == false)
            {
                /* Check number of joined devices */
                uint16_t numDevicesJoined = APP_EAP_SERVER_GetNumDevicesJoined();
                if (numDevicesJoined != app_cyclesData.numDevicesJoined)
                {
                    /* New device(s) joined or left the G3 network. Reload the
                     * timer to wait before first cycle */
                    SYS_TIME_TimerReload(app_cyclesData.timeHandle, 0, SYS_TIME_MSToCount(APP_CYCLES_TIME_WAIT_CYCLE_MS),
                            APP_SYS_TIME_CallbackSetFlag, (uintptr_t) &app_cyclesData.timeExpired, SYS_TIME_SINGLE);
                    app_cyclesData.numDevicesJoined = numDevicesJoined;
                }
            }
            else
            {
                /* Waiting time expired. Start first cycle */
                _APP_CYCLES_StartCycle();
                _APP_CYCLES_StartDeviceCycle();
                app_cyclesData.timeCountFirstCycleStart = app_cyclesData.timeCountCycleStart;
                _APP_CYCLES_SendPacket();
                app_cyclesData.state = APP_CYCLES_STATE_CYCLING;
            }

            break;
        }

        /* Cycling state: Sending ICMPv6 echo requests to registered devices */
        case APP_CYCLES_STATE_CYCLING:
        {
            if (app_cyclesData.icmpResult == false)
            {
                /* Error sending ICMPv6 echo request. Try again */
                _APP_CYCLES_SendPacket();
            }
            else if (app_cyclesData.timeExpired == true)
            {
                /* ICMPv6 echo reply not received */
                uint64_t elapsedTimeCount = SYS_TIME_Counter64Get() - app_cyclesData.timeCountEchoRequest;
                app_cyclesData.pStatsEntry->timeCountTotal += elapsedTimeCount;

                SYS_DEBUG_PRINT(SYS_ERROR_ERROR, "APP_CYCLES: ICMPv6 echo reply not received (timeout %u ms)\r\n",
                        SYS_TIME_CountToMS(elapsedTimeCount));

                /* Next ICMPv6 echo request */
                _APP_CYCLES_NextPacket();
            }

            break;
        }

        /* Error state */
        case APP_CYCLES_STATE_ERROR:
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

void APP_CYCLES_AdpBufferIndication(bool bufferReady)
{
    /* TODO */
}

/*******************************************************************************
 End of File
 */
