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
#include "configuration.h"
#include "app_cycles.h"
#include "app_eap_server.h"

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

#ifndef APP_CYCLES_METROLOGY_DATA_REQUEST
static const uint8_t app_cyclesPayload[16] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, \
        0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
#endif

// *****************************************************************************
// *****************************************************************************
// Section: Function Declaration
// *****************************************************************************
// *****************************************************************************

static void _APP_CYCLES_NextPacket(void);

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

#if SYS_CONSOLE_DEVICE_MAX_INSTANCES > 0U
static void _APP_CYCLES_ShowReport(void)
{
    SYS_DEBUG_PRINT(SYS_ERROR_INFO, "APP_CYCLES: Cycle %u finished. Duration %u ms "
            "(average total: %u ms; average device: %u ms)\r\n",
            app_cyclesData.cycleIndex, SYS_TIME_CountToMS(app_cyclesData.timeCountTotalCycle),
            SYS_TIME_CountToMS(app_cyclesData.timeCountTotal / (app_cyclesData.cycleIndex + 1)),
            SYS_TIME_CountToMS(app_cyclesData.timeCountTotalCycle / app_cyclesData.numDevicesJoined));

    /* Print summary of each device in tasks to avoid console buffer to be
     * full */
    SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "APP_CYCLES: Summary\r\n");
    app_cyclesData.deviceIndex = 0;
    app_cyclesData.state = APP_CYCLES_STATE_SHOW_REPORT;
}
#endif

static void _APP_CYCLES_SendPacket(void)
{
#ifndef APP_CYCLES_METROLOGY_DATA_REQUEST
    uint16_t availableTxBytes, chunkSize, payloadSize;
#endif

    if (app_cyclesData.availableBuffers == false)
    {
        /* Full buffers, wait for availability to send the packet */
        app_cyclesData.packetPending = true;
        return;
    }

    app_cyclesData.packetPending = false;

#ifndef APP_CYCLES_METROLOGY_DATA_REQUEST
    /* Get the number of bytes that can be written to the socket */
    availableTxBytes = TCPIP_UDP_PutIsReady(app_cyclesData.socket);
    if (availableTxBytes < app_cyclesData.packetSize)
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_ERROR, "APP_CYCLES: Not enough bytes available in UDP socket\r\n");
        _APP_CYCLES_NextPacket();
        return;
    }
#endif

    /* Put the first byte: 0x01 (UDP request) */
    TCPIP_UDP_Put(app_cyclesData.socket, 1);

#ifndef APP_CYCLES_METROLOGY_DATA_REQUEST
    /* Write the remaining UDP payload bytes. Implemented in a loop, processing
     * up to 16 bytes at a time. This limits memory usage while maximizing
     * performance. */
    chunkSize = 16;
    payloadSize = app_cyclesData.packetSize - 1;
    for (uint16_t written = 0; written < payloadSize; written += chunkSize)
    {
        if (written + chunkSize > payloadSize)
        {
            /* Treat the last chunk */
            chunkSize = payloadSize - written;
        }

        TCPIP_UDP_ArrayPut(app_cyclesData.socket, app_cyclesPayload, chunkSize);
    }
#endif

    /* Send the UDP request */
    app_cyclesData.timeCountUdpRequest = SYS_TIME_Counter64Get();
    TCPIP_UDP_Flush(app_cyclesData.socket);

#ifndef APP_CYCLES_METROLOGY_DATA_REQUEST
    SYS_DEBUG_PRINT(SYS_ERROR_DEBUG, "APP_CYCLES: UDP packet size %hu\r\n", app_cyclesData.packetSize);
#endif

    /* Create timer for timeout to detect UDP reply not received */
    app_cyclesData.timeExpired = false;
    app_cyclesData.timeHandle = SYS_TIME_CallbackRegisterMS(APP_SYS_TIME_CallbackSetFlag,
            (uintptr_t) &app_cyclesData.timeExpired, APP_CYCLES_TIMEOUT_MS,
            SYS_TIME_SINGLE);

    /* Update statistics */
    app_cyclesData.numUdpRequests++;
    app_cyclesData.pStatsEntry->numUdpRequests++;
}

static void _APP_CYCLES_StartDeviceCycle(void)
{
    IPV6_ADDR targetAddress;
    APP_CYCLES_STATISTICS_ENTRY* pFreeStatsEntry = NULL;
    APP_CYCLES_STATISTICS_ENTRY* pStatsEntry = NULL;
    uint16_t shortAddress, panId;
    uint8_t eui64[8];
    char targetAddressString[50 + 1];

    /* Create link-local address based on short address and PAN ID */
    shortAddress = APP_EAP_SERVER_GetDeviceAddress(app_cyclesData.deviceIndex, eui64);
    panId = APP_G3_MANAGEMENT_GetPanId();
    TCPIP_Helper_StringToIPv6Address(APP_TCPIP_MANAGEMENT_IPV6_LINK_LOCAL_ADDRESS_G3, &targetAddress);
    targetAddress.v[8] = (uint8_t) (panId >> 8);
    targetAddress.v[9] = (uint8_t) panId;
    targetAddress.v[14] = (uint8_t) (shortAddress >> 8);
    targetAddress.v[15] = (uint8_t) shortAddress;
    TCPIP_Helper_IPv6AddressToString(&targetAddress, targetAddressString, sizeof(targetAddressString) - 1);

    /* Close socket if already opened */
    if (app_cyclesData.socket != INVALID_SOCKET)
    {
        TCPIP_UDP_Close(app_cyclesData.socket);
    }

    /* Open UDP client socket */
    app_cyclesData.socket = TCPIP_UDP_ClientOpen(IP_ADDRESS_TYPE_IPV6,
            APP_CYCLES_SOCKET_PORT, (IP_MULTI_ADDRESS*) &targetAddress);

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
    pStatsEntry->numCycles++;

    _APP_CYCLES_SendPacket();
}

static void _APP_CYCLES_StartCycle(void)
{
    app_cyclesData.deviceIndex = 0;
    app_cyclesData.timeCountTotalCycle = 0;
#ifndef APP_CYCLES_METROLOGY_DATA_REQUEST
    app_cyclesData.packetSize = APP_CYCLES_PACKET_SIZE_1;
#endif
    app_cyclesData.numDevicesJoined = APP_EAP_SERVER_GetNumDevicesJoined();
    SYS_DEBUG_PRINT(SYS_ERROR_INFO, "APP_CYCLES: Starting cycle %u. %hu nodes in cycle\r\n",
            app_cyclesData.cycleIndex, app_cyclesData.numDevicesJoined);
}

static void _APP_CYCLES_NextPacket(void)
{
#ifndef APP_CYCLES_METROLOGY_DATA_REQUEST
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
#if SYS_CONSOLE_DEVICE_MAX_INSTANCES > 0U
            _APP_CYCLES_ShowReport();
#else
            /* Start again with first device */
            app_cyclesData.cycleIndex++;
            _APP_CYCLES_StartCycle();

            /* Start timer to wait before next device cycle */
            app_cyclesData.timeExpired = false;
            app_cyclesData.timeHandle = SYS_TIME_CallbackRegisterMS(APP_SYS_TIME_CallbackSetFlag,
                    (uintptr_t) &app_cyclesData.timeExpired, APP_CYCLES_TIME_BTW_DEVICE_CYCLES_MS,
                   SYS_TIME_SINGLE);

            app_cyclesData.state = APP_CYCLES_STATE_WAIT_NEXT_DEVICE_CYCLE;
#endif
        }
        else
        {
            /* Start timer to wait before next device cycle */
            app_cyclesData.timeExpired = false;
            app_cyclesData.timeHandle = SYS_TIME_CallbackRegisterMS(APP_SYS_TIME_CallbackSetFlag,
                    (uintptr_t) &app_cyclesData.timeExpired, APP_CYCLES_TIME_BTW_DEVICE_CYCLES_MS,
                   SYS_TIME_SINGLE);

            app_cyclesData.state = APP_CYCLES_STATE_WAIT_NEXT_DEVICE_CYCLE;
        }        

        return;
    }

    _APP_CYCLES_SendPacket();
#else
    /* Next device */
    app_cyclesData.deviceIndex++;
    if (app_cyclesData.deviceIndex == app_cyclesData.numDevicesJoined)
    {
#if SYS_CONSOLE_DEVICE_MAX_INSTANCES > 0U
        _APP_CYCLES_ShowReport();
#else
        /* Start again with first device */
        app_cyclesData.cycleIndex++;
        _APP_CYCLES_StartCycle();

        /* Start timer to wait before next device cycle */
        app_cyclesData.timeExpired = false;
        app_cyclesData.timeHandle = SYS_TIME_CallbackRegisterMS(APP_SYS_TIME_CallbackSetFlag,
                (uintptr_t) &app_cyclesData.timeExpired, APP_CYCLES_TIME_BTW_DEVICE_CYCLES_MS,
               SYS_TIME_SINGLE);

        app_cyclesData.state = APP_CYCLES_STATE_WAIT_NEXT_DEVICE_CYCLE;
#endif
    }
    else
    {
        /* Start timer to wait before next device cycle */
        app_cyclesData.timeExpired = false;
        app_cyclesData.timeHandle = SYS_TIME_CallbackRegisterMS(APP_SYS_TIME_CallbackSetFlag,
                (uintptr_t) &app_cyclesData.timeExpired, APP_CYCLES_TIME_BTW_DEVICE_CYCLES_MS,
               SYS_TIME_SINGLE);

        app_cyclesData.state = APP_CYCLES_STATE_WAIT_NEXT_DEVICE_CYCLE;
    }
#endif
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
    app_cyclesData.socket = INVALID_SOCKET;
    app_cyclesData.timeCountTotal = 0;
    app_cyclesData.numUdpRequests = 0;
    app_cyclesData.numUdpReplies = 0;
    app_cyclesData.cycleIndex = 0;
    app_cyclesData.numDevicesJoined = 0;
    app_cyclesData.timeExpired = false;
    app_cyclesData.availableBuffers = true;
    app_cyclesData.packetPending = false;
    for (uint16_t i = 0; i < APP_EAP_SERVER_MAX_DEVICES; i++)
    {
        app_cyclesStatistics[i].timeCountTotal = 0;
        app_cyclesStatistics[i].numCycles = 0;
        app_cyclesStatistics[i].numUdpRequests = 0;
        app_cyclesStatistics[i].numUdpReplies = 0;
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
                /* TCP/IP stack ready */
                app_cyclesData.state = APP_CYCLES_STATE_WAIT_FIRST_JOIN;
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
                 * before first cycle */
                app_cyclesData.state = APP_CYCLES_STATE_WAIT_FIRST_CYCLE;
                app_cyclesData.timeHandle = SYS_TIME_CallbackRegisterMS(APP_SYS_TIME_CallbackSetFlag,
                        (uintptr_t) &app_cyclesData.timeExpired, APP_CYCLES_TIME_WAIT_CYCLE_MS,
                        SYS_TIME_SINGLE);
            }

            break;
        }

        /* State to wait for first UDP cycle */
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
                app_cyclesData.state = APP_CYCLES_STATE_CYCLING;
            }

            break;
        }

        /* Cycling state: Sending UDP requests to registered devices */
        case APP_CYCLES_STATE_CYCLING:
        {
            uint64_t currentTimeCount, elapsedTimeCount;
            uint16_t rxPayloadSize;
            uint8_t udpProtocol;
            bool payloadOk = true;

            if ((app_cyclesData.timeExpired == true) && (app_cyclesData.availableBuffers == true))
            {
                /* UDP reply not received */
                currentTimeCount = SYS_TIME_Counter64Get();
                elapsedTimeCount = currentTimeCount - app_cyclesData.timeCountUdpRequest;
                app_cyclesData.pStatsEntry->timeCountTotal += elapsedTimeCount;
                app_cyclesData.timeCountTotal += elapsedTimeCount;
                app_cyclesData.timeCountTotalCycle += elapsedTimeCount;

                SYS_DEBUG_PRINT(SYS_ERROR_ERROR, "APP_CYCLES: UDP reply not received (timeout %u ms)\r\n",
                        SYS_TIME_CountToMS(elapsedTimeCount));

                /* Next UDP request */
                _APP_CYCLES_NextPacket();
                break;
            }

            /* Get number of bytes received */
            rxPayloadSize = TCPIP_UDP_GetIsReady(app_cyclesData.socket);

            if (rxPayloadSize == 0)
            {
                /* No data received */
                break;
            }

            /* UDP frame received. Compute round-trip time. */
            elapsedTimeCount = SYS_TIME_Counter64Get() - app_cyclesData.timeCountUdpRequest;
            app_cyclesData.pStatsEntry->timeCountTotal += elapsedTimeCount;
            app_cyclesData.timeCountTotal += elapsedTimeCount;
            app_cyclesData.timeCountTotalCycle += elapsedTimeCount;
            SYS_TIME_TimerDestroy(app_cyclesData.timeHandle);

            /* Read first received byte (protocol) */
            TCPIP_UDP_Get(app_cyclesData.socket, &udpProtocol);

#ifndef APP_CYCLES_METROLOGY_DATA_REQUEST
            if (rxPayloadSize != app_cyclesData.packetSize)
            {
                /* Wrong UDP packet size */
                payloadOk = false;
            }
            else if (udpProtocol == 2)
            {
                uint16_t chunkSize, contentSize;
                uint8_t payloadFragment[16];

                /* Check the remaining UDP payload bytes. Implemented in a loop,
                 * processing up to 16 bytes at a time. This limits memory usage
                 * while maximizing performance. */
                chunkSize = 16;
                contentSize = rxPayloadSize - 1;
                for (uint16_t written = 0; written < contentSize; written += chunkSize)
                {
                    if (written + chunkSize > contentSize)
                    {
                        /* Treat the last chunk */
                        chunkSize = contentSize - written;
                    }

                    TCPIP_UDP_ArrayGet(app_cyclesData.socket, payloadFragment, chunkSize);
                    if (memcmp(payloadFragment, app_cyclesPayload, 16) != 0)
                    {
                        /* Wrong UDP reply content */
                        payloadOk = false;
                        break;
                    }
               }
            }
#else
            if (rxPayloadSize != (sizeof(APP_CYCLES_METROLOGY_DATA) + 1))
            {
                /* Wrong UDP packet size */
                payloadOk = false;
            }
            else if (udpProtocol == 2)
            {
                APP_CYCLES_METROLOGY_DATA metData;

                /* Read metrology data */
                TCPIP_UDP_ArrayGet(app_cyclesData.socket, (uint8_t *) &metData, sizeof(metData));

                SYS_DEBUG_PRINT(SYS_ERROR_DEBUG, "APP_CYCLES: RMS voltage Ua=%.3fV Ub=%.3fV Uc=%.3fV\r\n"
                        "RMS current Ia=%.4fA Ib=%.4fA Ic=%.4fA, Iini=%.4fA Inm=%.4fA Inmi=%.4fA\r\n"
                        "RMS active power Pt=%.1fW Pa=%.1fW Pb=%.1fW Pc=%.1fW\r\n"
                        "RMS reactive power Qt=%.1fW Qa=%.1fW Qb=%.1fW Qc=%.1fW\r\n"
                        "RMS aparent power St=%.1fW Sa=%.1fW Sb=%.1fW Sc=%.1fW\r\n"
                        "Frequency=%.2fHz\r\n"
                        "Angle_A=%.3f Angle_B=%.3f Angle_C=%.3f Angle_N=%.3f\r\n",
                        (float)metData.rmsUA/10000, (float)metData.rmsUB/10000, (float)metData.rmsUC/10000,
                        (float)metData.rmsIA/10000, (float)metData.rmsIB/10000, (float)metData.rmsIC/10000,
                        (float)metData.rmsINI/10000, (float)metData.rmsINM/10000, (float)metData.rmsINMI/10000,
                        (float)metData.rmsPT/10, (float)metData.rmsPA/10, (float)metData.rmsPB/10, (float)metData.rmsPC/10,
                        (float)metData.rmsQT/10, (float)metData.rmsQA/10, (float)metData.rmsQB/10, (float)metData.rmsQC/10,
                        (float)metData.rmsST/10, (float)metData.rmsSA/10, (float)metData.rmsSB/10, (float)metData.rmsSC/10,
                        (float)metData.freq/100,
                        (float)metData.angleA/100000, (float)metData.angleB/100000, (float)metData.angleC/100000, (float)metData.angleN/100000);
            }
#endif
            else
            {
                /* Wrong UDP protocol, it must be 0x02 (UDP reply) */
                payloadOk = false;
            }

            if (payloadOk == true)
            {
                /* Update statistics */
                app_cyclesData.numUdpReplies++;
                app_cyclesData.pStatsEntry->numUdpReplies++;

                SYS_DEBUG_PRINT(SYS_ERROR_DEBUG, "APP_CYCLES: UDP reply received (%u ms)\r\n",
                        SYS_TIME_CountToMS(elapsedTimeCount));
            }
            else
            {
                SYS_DEBUG_PRINT(SYS_ERROR_ERROR, "APP_CYCLES: Wrong UDP reply received (%u ms)\r\n",
                        SYS_TIME_CountToMS(elapsedTimeCount));
            }

            /* Received UDP frame processed, we can discard it */
            TCPIP_UDP_Discard(app_cyclesData.socket);

            /* Next UDP request */
            _APP_CYCLES_NextPacket();
            break;
        }

        /* State to wait for the next device UDP cycle */
        case APP_CYCLES_STATE_WAIT_NEXT_DEVICE_CYCLE:
        {
            if (app_cyclesData.timeExpired == true)
            {
                /* Waiting time expired. Start next device cycle */
                _APP_CYCLES_StartDeviceCycle();
                app_cyclesData.state = APP_CYCLES_STATE_CYCLING;
            }

            break;
        }

#if SYS_CONSOLE_DEVICE_MAX_INSTANCES > 0U
        /* State to show cycle report */
        case APP_CYCLES_STATE_SHOW_REPORT:
        {
            /* Check console status */
            if (SYS_CONSOLE_WriteCountGet(SYS_CONSOLE_INDEX_0) <= 0)
            {
                uint32_t numErrors;
                uint8_t successRate;
                uint16_t idx = app_cyclesData.deviceIndex;
                bool printed = false;

                while((printed == false) && (idx < APP_EAP_SERVER_MAX_DEVICES))
                {
                    if ((app_cyclesStatistics[idx].shortAddress != 0xFFFF) && (app_cyclesStatistics[idx].numUdpRequests > 0))
                    {
                        numErrors = app_cyclesStatistics[idx].numUdpRequests - app_cyclesStatistics[idx].numUdpReplies;
                        successRate = (app_cyclesStatistics[idx].numUdpReplies * 100) / app_cyclesStatistics[idx].numUdpRequests;
                        SYS_DEBUG_PRINT(SYS_ERROR_INFO, "\tShort address 0x%04X: Sent %u, Success %u (%hhu %%), Errors %u,"
                                " Average duration %u\r\n", app_cyclesStatistics[idx].shortAddress,
                                app_cyclesStatistics[idx].numUdpRequests, app_cyclesStatistics[idx].numUdpReplies,
                                successRate, numErrors,
                                SYS_TIME_CountToMS(app_cyclesStatistics[idx].timeCountTotal / app_cyclesStatistics[idx].numCycles));
                    }

                    idx++;
                }

                app_cyclesData.deviceIndex = idx;

                if (idx == APP_EAP_SERVER_MAX_DEVICES)
                {
                    numErrors = app_cyclesData.numUdpRequests - app_cyclesData.numUdpReplies;
                    successRate = (app_cyclesData.numUdpReplies * 100) / app_cyclesData.numUdpRequests;
                    SYS_DEBUG_PRINT(SYS_ERROR_INFO, "TOTAL: Sent %u, Success %u (%hhu %%), Errors %u\r\n",
                            app_cyclesData.numUdpRequests, app_cyclesData.numUdpReplies, successRate, numErrors);

                    /* Start again with first device */
                    app_cyclesData.cycleIndex++;
                    _APP_CYCLES_StartCycle();

                    /* Start timer to wait before next device cycle */
                    app_cyclesData.timeExpired = false;
                    app_cyclesData.timeHandle = SYS_TIME_CallbackRegisterMS(APP_SYS_TIME_CallbackSetFlag,
                            (uintptr_t) &app_cyclesData.timeExpired, APP_CYCLES_TIME_BTW_DEVICE_CYCLES_MS,
                           SYS_TIME_SINGLE);

                    app_cyclesData.state = APP_CYCLES_STATE_WAIT_NEXT_DEVICE_CYCLE;
                }
            }

            break;
        }
#endif
        /* Conformance state: Cycling disabled */
        case APP_CYCLES_STATE_CONFORMANCE:
        {
            /* Nothing to do */
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

void APP_CYCLES_AdpBufferIndication(ADP_BUFFER_IND_PARAMS* bufferInd)
{
    if ((bufferInd->largeBuffersAvailable == 1) && (bufferInd->mediumBuffersAvailable == 1) &&
            (bufferInd->smallBuffersAvailable == 1))
    {
        /* All buffers are available */
        app_cyclesData.availableBuffers = true;

        if (app_cyclesData.packetPending == true)
        {
            /* Send pending packet */
            _APP_CYCLES_SendPacket();
        }
    }
    else
    {
        /* At least one type of buffers are full */
        app_cyclesData.availableBuffers = false;
    }
}

void APP_CYCLES_SetConformanceConfig ( void )
{
    /* Disable cycling operation and close socket if already opened */
    app_cyclesData.state = APP_CYCLES_STATE_CONFORMANCE;
    if (app_cyclesData.socket != INVALID_SOCKET)
    {
        TCPIP_UDP_Close(app_cyclesData.socket);
    }
}

/*******************************************************************************
 End of File
 */
