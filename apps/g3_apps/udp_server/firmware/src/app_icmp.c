/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_icmp.c

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
    This structure should be initialized by the APP_ICMP_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_ICMP_DATA app_icmpData;

APP_ICMP_STATISTICS_ENTRY app_icmpStatistics[APP_G3_COORDINATOR_MAX_DEVICES];

// *****************************************************************************
// *****************************************************************************
// Section: Function Declaration
// *****************************************************************************
// *****************************************************************************

static void _APP_ICMP_NextPing(void);

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

static void _APP_ICMP_TimeExpiredSetFlag(uintptr_t context)
{
    /* Context holds the flag's address */
    *((bool *) context) = true;
}

static void _APP_ICMP_Callback (
    TCPIP_NET_HANDLE hNetIf,
    uint8_t type,
    const IPV6_ADDR * localIP,
    const IPV6_ADDR * remoteIP,
    void * header)
{
    if ((type == ICMPV6_INFO_ECHO_REPLY) && (memcmp(remoteIP, &app_icmpData.targetAddress, sizeof(IPV6_ADDR)) == 0))
    {
        uint64_t elapsedTimeCount;
        uint64_t currentTimeCount = SYS_TIME_Counter64Get();

        /* Echo reply received successfully */
        SYS_TIME_TimerDestroy(app_icmpData.timeHandle);

        /* Update statistics */
        app_icmpData.numEchoReplies++;
        elapsedTimeCount = currentTimeCount - app_icmpData.timeCountEchoRequest;
        if (app_icmpData.pStatsEntry != NULL)
        {
            app_icmpData.pStatsEntry->timeCountTotal += elapsedTimeCount;
            app_icmpData.pStatsEntry->numEchoReplies++;
        }

        SYS_DEBUG_PRINT(SYS_ERROR_DEBUG, "APP_ICMP: ICMPv6 echo reply received (%u ms)\r\n",
                SYS_TIME_CountToMS(elapsedTimeCount));

        /* Next ICMPv6 echo request */
        _APP_ICMP_NextPing();
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

static void _APP_ICMP_SendPing(void)
{
    uint16_t shortAddress;
    uint8_t eui64[8];
    char targetAddressString[50 + 1];

    /* Create link-local address based on short address and PAN ID */
    shortAddress = APP_G3_COORDINATOR_GetDeviceAddress(app_icmpData.deviceIndex, eui64);
    TCPIP_Helper_StringToIPv6Address(APP_UDP_IPV6_LINK_LOCAL_ADDRESS_G3, &app_icmpData.targetAddress);
    app_icmpData.targetAddress.v[8] = (uint8_t) (APP_G3_COORDINATOR_PAN_ID >> 8);
    app_icmpData.targetAddress.v[9] = (uint8_t) APP_G3_COORDINATOR_PAN_ID;
    app_icmpData.targetAddress.v[14] = (uint8_t) (shortAddress >> 8);
    app_icmpData.targetAddress.v[15] = (uint8_t) shortAddress;

    /* Send ICMP */
    app_icmpData.timeCountEchoRequest = SYS_TIME_Counter64Get();
    app_icmpData.icmpResult = TCPIP_ICMPV6_EchoRequestSend(app_icmpData.netHandle, &app_icmpData.targetAddress,
            app_icmpData.sequenceNumber, 0, app_icmpData.packetSize);

    TCPIP_Helper_IPv6AddressToString(&app_icmpData.targetAddress, targetAddressString, sizeof(targetAddressString) - 1);
    SYS_DEBUG_PRINT(SYS_ERROR_DEBUG, "APP_ICMP: Ping %s (Short Address: 0x%04X, EUI64: 0x%02X%02X%02X%02X%02X%02X%02X%02X)."
            " Packet size: %hu\r\n", targetAddressString, shortAddress, eui64[0], eui64[1], eui64[2], eui64[3],
            eui64[4], eui64[5], eui64[6], eui64[7], app_icmpData.packetSize);

    if (app_icmpData.icmpResult == true)
    {
        APP_ICMP_STATISTICS_ENTRY* pFreeStatsEntry = NULL;
        APP_ICMP_STATISTICS_ENTRY* pStatsEntry = NULL;

        /* Create timer for timeout to detect echo reply not received */
        app_icmpData.timeExpired = false;
        app_icmpData.timeHandle = SYS_TIME_CallbackRegisterMS(_APP_ICMP_TimeExpiredSetFlag,
                (uintptr_t) &app_icmpData.timeExpired, APP_ICMP_TIMEOUT_MS,
                SYS_TIME_SINGLE);

        /* Update statistics */
        app_icmpData.numEchoRequests++;
        for (uint16_t i = 0; i < APP_G3_COORDINATOR_MAX_DEVICES; i++)
        {
            if (app_icmpStatistics[i].shortAddress == shortAddress)
            {
                /* Statistics entry matching short address found */
                pStatsEntry = &app_icmpStatistics[i];
                break;
            }
            else if ((pFreeStatsEntry == NULL) && (app_icmpStatistics[i].shortAddress == 0xFFFF))
            {
                /* Free statistics entry found */
                pFreeStatsEntry = &app_icmpStatistics[i];
            }
        }

        if (pStatsEntry == NULL)
        {
            pStatsEntry = pFreeStatsEntry;
        }

        app_icmpData.pStatsEntry = pStatsEntry;
        if (pStatsEntry != NULL)
        {
            pStatsEntry->shortAddress = shortAddress;
            pStatsEntry->numEchoRequests++;
        }
    }
    else
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_ERROR, "APP_ICMP: Error in TCPIP_ICMPV6_EchoRequestSend\r\n");
    }
}

static void _APP_ICMP_StartCycle(void)
{
    app_icmpData.deviceIndex = 0;
    app_icmpData.packetSize = APP_ICMP_PACKET_SIZE_1;
    app_icmpData.numDevicesJoined = APP_G3_COORDINATOR_GetNumDevicesJoined();
    SYS_DEBUG_PRINT(SYS_ERROR_INFO, "APP_ICMP: Starting cycle %u. %hu nodes in cycle\r\n",
            app_icmpData.cycleIndex, app_icmpData.numDevicesJoined);
    app_icmpData.timeCountCycleStart = SYS_TIME_Counter64Get();
}

static void _APP_ICMP_NextPing(void)
{
    if (app_icmpData.packetSize == APP_ICMP_PACKET_SIZE_1)
    {
        /* Next size */
        app_icmpData.packetSize = APP_ICMP_PACKET_SIZE_2;
    }
    else if (app_icmpData.packetSize == APP_ICMP_PACKET_SIZE_2)
    {
        /* Next size */
        app_icmpData.packetSize = APP_ICMP_PACKET_SIZE_3;
    }
    else if (app_icmpData.packetSize == APP_ICMP_PACKET_SIZE_3)
    {
        /* Next device */
        app_icmpData.deviceIndex++;
        if (app_icmpData.deviceIndex == app_icmpData.numDevicesJoined)
        {
            uint32_t numErrors;
            uint8_t successRate;
            uint64_t currentTimeCount = SYS_TIME_Counter64Get();
            uint64_t elapsedTimeCount = currentTimeCount - app_icmpData.timeCountCycleStart;
            uint64_t elapsedTimeCountTotal = currentTimeCount - app_icmpData.timeCountFirstCycleStart;
            SYS_DEBUG_PRINT(SYS_ERROR_INFO, "APP_ICMP: Cycle %u finished. Duration %u ms "
                    "(average total: %u ms; average device: %u ms)\r\n",
                    app_icmpData.cycleIndex, SYS_TIME_CountToMS(elapsedTimeCount),
                    SYS_TIME_CountToMS(elapsedTimeCountTotal / (app_icmpData.cycleIndex + 1)),
                    SYS_TIME_CountToMS(elapsedTimeCount / app_icmpData.numDevicesJoined));

            SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "APP_ICMP: Summary\r\n");

            /* Print statistics */
            for (uint16_t i = 0; i < APP_G3_COORDINATOR_MAX_DEVICES; i++)
            {
                if (app_icmpStatistics[i].shortAddress != 0xFFFF)
                {
                    numErrors = app_icmpStatistics[i].numEchoRequests - app_icmpStatistics[i].numEchoReplies;
                    successRate = (app_icmpStatistics[i].numEchoReplies * 100) / app_icmpStatistics[i].numEchoRequests;
                    SYS_DEBUG_PRINT(SYS_ERROR_INFO, "Short address 0x%04X: Sent %u, Success %u (%hhu %%), Errors %u,"
                            " Average duration %u\r\n", app_icmpStatistics[i].shortAddress,
                            app_icmpStatistics[i].numEchoRequests, app_icmpStatistics[i].numEchoReplies,
                            successRate, numErrors,
                            SYS_TIME_CountToMS(app_icmpStatistics[i].timeCountTotal / (app_icmpData.cycleIndex + 1)));
                }
            }

            numErrors = app_icmpData.numEchoRequests - app_icmpData.numEchoReplies;
            successRate = (app_icmpData.numEchoReplies * 100) / app_icmpData.numEchoRequests;
            SYS_DEBUG_PRINT(SYS_ERROR_INFO, "TOTAL: Sent %u, Success %u (%hhu %%), Errors %u\r\n",
                    app_icmpData.numEchoRequests, app_icmpData.numEchoReplies, successRate, numErrors);

            /* Start again with first device */
            app_icmpData.sequenceNumber++;
            app_icmpData.cycleIndex++;
            _APP_ICMP_StartCycle();
        }
    }

    _APP_ICMP_SendPing();
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_ICMP_Initialize ( void )

  Remarks:
    See prototype in app_icmp.h.
 */

void APP_ICMP_Initialize ( void )
{
    /* Place the application's state machine in its initial state. */
    app_icmpData.state = APP_ICMP_STATE_WAIT_TCPIP_READY;

    /* Initialize application variables */
    app_icmpData.pStatsEntry = NULL;
    app_icmpData.numEchoRequests = 0;
    app_icmpData.numEchoReplies = 0;
    app_icmpData.cycleIndex = 0;
    app_icmpData.numDevicesJoined = 0;
    app_icmpData.sequenceNumber = 0;
    app_icmpData.timeExpired = false;
    for (uint16_t i = 0; i < APP_G3_COORDINATOR_MAX_DEVICES; i++)
    {
        app_icmpStatistics[i].timeCountTotal = 0;
        app_icmpStatistics[i].numEchoRequests = 0;
        app_icmpStatistics[i].numEchoReplies = 0;
        app_icmpStatistics[i].shortAddress = 0xFFFF;
    }
}


/******************************************************************************
  Function:
    void APP_ICMP_Tasks ( void )

  Remarks:
    See prototype in app_icmp.h.
 */

void APP_ICMP_Tasks ( void )
{
    /* Check the application's current state. */
    switch ( app_icmpData.state )
    {
        /* Application's initial state. */
        case APP_ICMP_STATE_WAIT_TCPIP_READY:
        {
            SYS_STATUS tcpipStat = TCPIP_STACK_Status(sysObj.tcpip);
            if (tcpipStat <= SYS_STATUS_ERROR)
            {
                /* Some error occurred */
                SYS_DEBUG_MESSAGE(SYS_ERROR_ERROR, "APP_ICMP: TCP/IP stack initialization failed!\r\n");
                app_icmpData.state = APP_ICMP_STATE_ERROR;
            }
            else if(tcpipStat == SYS_STATUS_READY)
            {
                app_icmpData.netHandle = TCPIP_STACK_NetHandleGet("G3ADPMAC");
                if (TCPIP_STACK_NetIsReady(app_icmpData.netHandle) == true)
                {
                    SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "APP_ICMP: TCP/IP stack ready\r\n");
                    app_icmpData.state = APP_ICMP_STATE_WAIT_FIRST_JOIN;
                }
            }

            break;
        }

        /* State to wait for the first device joined to the G3 network */
        case APP_ICMP_STATE_WAIT_FIRST_JOIN:
        {
            /* Get number of joined devices */
            app_icmpData.numDevicesJoined = APP_G3_COORDINATOR_GetNumDevicesJoined();

            if (app_icmpData.numDevicesJoined > 0)
            {
                /* First device joined the G3 network. Start timer to wait
                 * before next cycle */
                app_icmpData.state = APP_ICMP_STATE_WAIT_FIRST_CYCLE;
                app_icmpData.timeHandle = SYS_TIME_CallbackRegisterMS(_APP_ICMP_TimeExpiredSetFlag,
                        (uintptr_t) &app_icmpData.timeExpired, APP_ICMP_TIME_WAIT_CYCLE_MS,
                        SYS_TIME_SINGLE);
            }

            break;
        }

        /* State to wait for first ICMPv6 cycle */
        case APP_ICMP_STATE_WAIT_FIRST_CYCLE:
        {
            if (app_icmpData.timeExpired == false)
            {
                /* Check number of joined devices */
                uint16_t numDevicesJoined = APP_G3_COORDINATOR_GetNumDevicesJoined();
                if (numDevicesJoined != app_icmpData.numDevicesJoined)
                {
                    /* New device(s) joined or left the G3 network. Reload the
                     * timer to wait before first cycle */
                    SYS_TIME_TimerReload(app_icmpData.timeHandle, 0, SYS_TIME_MSToCount(APP_ICMP_TIME_WAIT_CYCLE_MS),
                            _APP_ICMP_TimeExpiredSetFlag, (uintptr_t) &app_icmpData.timeExpired, SYS_TIME_SINGLE);
                    app_icmpData.numDevicesJoined = numDevicesJoined;
                }
            }
            else
            {
                /* Waiting time expired. Register ICMPv6 callback and start
                 * first cycle */
                TCPIP_ICMPV6_CallbackRegister(_APP_ICMP_Callback);
                _APP_ICMP_StartCycle();
                app_icmpData.timeCountFirstCycleStart = app_icmpData.timeCountCycleStart;
                _APP_ICMP_SendPing();
                app_icmpData.state = APP_ICMP_STATE_CYCLING;
            }

            break;
        }

        /* Cycling state: Sending ICMPv6 echo requests to registered devices */
        case APP_ICMP_STATE_CYCLING:
        {
            if (app_icmpData.icmpResult == false)
            {
                /* Error sending ICMPv6 echo request. Try again */
                _APP_ICMP_SendPing();
            }
            else if (app_icmpData.timeExpired == true)
            {
                /* ICMPv6 echo reply not received */
                uint64_t elapsedTimeCount = SYS_TIME_Counter64Get() - app_icmpData.timeCountEchoRequest;
                if (app_icmpData.pStatsEntry != NULL)
                {
                    app_icmpData.pStatsEntry->timeCountTotal += elapsedTimeCount;
                }

                SYS_DEBUG_PRINT(SYS_ERROR_ERROR, "APP_ICMP: ICMPv6 echo reply not received (timeout %u ms)\r\n",
                        SYS_TIME_CountToMS(elapsedTimeCount));

                /* Next ICMPv6 echo request */
                _APP_ICMP_NextPing();
            }

            break;
        }

        /* Error state */
        case APP_ICMP_STATE_ERROR:
        {
            break;
        }

        /* The default state should never be executed. */
        default:
        {
            break;
        }
    }
}

/*******************************************************************************
 End of File
 */
