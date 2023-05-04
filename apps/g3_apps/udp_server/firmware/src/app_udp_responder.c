/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_udp_responder.c

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
#include "tcpip_manager_control.h"
#include "app_udp_responder.h"

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
    This structure should be initialized by the APP_UDP_RESPONDER_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_UDP_RESPONDER_DATA app_udp_responderData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

static void _APP_UDP_SetConformanceParameters(void)
{
    IPV6_ADDR multicastAddr;

    /* Set multicast addresses */
    TCPIP_Helper_StringToIPv6Address(APP_UDP_IPV6_MULTICAST_0_CONFORMANCE, &multicastAddr);
    TCPIP_IPV6_MulticastListenerAdd(app_udp_responderData.netHandle, &multicastAddr);
    TCPIP_Helper_StringToIPv6Address(APP_UDP_IPV6_MULTICAST_1_CONFORMANCE, &multicastAddr);
    TCPIP_IPV6_MulticastListenerAdd(app_udp_responderData.netHandle, &multicastAddr);
}

static void _APP_UDP_SetIPv6Addresses(void)
{
    uint16_t shortAddr, panId;
    uint8_t* eui64;

    /* Configure link-local address, based on PAN ID and Short Address */
    TCPIP_Helper_StringToIPv6Address(APP_UDP_IPV6_LINK_LOCAL_ADDRESS_G3, &app_udp_responderData.linkLocalAddress);
    shortAddr = APP_G3_GetShortAddress();
    panId = APP_G3_GetPanId();
    app_udp_responderData.linkLocalAddress.v[8] = (uint8_t) (panId >> 8);
    app_udp_responderData.linkLocalAddress.v[9] = (uint8_t) panId;
    app_udp_responderData.linkLocalAddress.v[14] = (uint8_t) (shortAddr >> 8);
    app_udp_responderData.linkLocalAddress.v[15] = (uint8_t) shortAddr;
    TCPIP_IPV6_UnicastAddressAdd(app_udp_responderData.netHandle,
            &app_udp_responderData.linkLocalAddress, 0, false);

    /* Configure Unique Local Link (ULA) address, based on PAN ID and Extended
     * Address */
    TCPIP_Helper_StringToIPv6Address(APP_UDP_IPV6_NETWORK_PREFIX_G3, &app_udp_responderData.ulaAddress);
    eui64 = APP_G3_GetExtendedAddress();
    app_udp_responderData.ulaAddress.v[6] = (uint8_t) (panId >> 8);
    app_udp_responderData.ulaAddress.v[7] = (uint8_t) panId;
    app_udp_responderData.ulaAddress.v[8] = eui64[7];
    app_udp_responderData.ulaAddress.v[9] = eui64[6];
    app_udp_responderData.ulaAddress.v[10] = eui64[5];
    app_udp_responderData.ulaAddress.v[11] = eui64[4];
    app_udp_responderData.ulaAddress.v[12] = eui64[3];
    app_udp_responderData.ulaAddress.v[13] = eui64[2];
    app_udp_responderData.ulaAddress.v[14] = eui64[1];
    app_udp_responderData.ulaAddress.v[15] = eui64[0];
    TCPIP_IPV6_UnicastAddressAdd(app_udp_responderData.netHandle,
            &app_udp_responderData.ulaAddress, APP_UDP_IPV6_NETWORK_PREFIX_G3_LEN, false);
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_UDP_RESPONDER_Initialize ( void )

  Remarks:
    See prototype in app_udp_responder.h.
 */

void APP_UDP_RESPONDER_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    app_udp_responderData.state = APP_UDP_RESPONDER_STATE_WAIT_TCPIP_READY;


}

/******************************************************************************
  Function:
    void APP_UDP_RESPONDER_Tasks ( void )

  Remarks:
    See prototype in app_udp_responder.h.
 */

void APP_UDP_RESPONDER_Tasks ( void )
{
    /* Refresh Watchdog */
    CLEAR_WATCHDOG();

    /* Check the application's current state. */
    switch ( app_udp_responderData.state )
    {
        /* Application's initial state. */
        case APP_UDP_RESPONDER_STATE_WAIT_TCPIP_READY:
        {
            SYS_STATUS tcpipStat = TCPIP_STACK_Status(sysObj.tcpip);
            if (tcpipStat <= SYS_STATUS_ERROR)
            {
                /* Some error occurred */
                SYS_DEBUG_MESSAGE(SYS_ERROR_ERROR, "APP_UDP_RESPONDER: TCP/IP stack initialization failed!\r\n");
                app_udp_responderData.state = APP_UDP_RESPONDER_STATE_ERROR;
            }
            else if(tcpipStat == SYS_STATUS_READY)
            {
                app_udp_responderData.netHandle = TCPIP_STACK_NetHandleGet("G3ADPMAC");
                if (TCPIP_STACK_NetIsReady(app_udp_responderData.netHandle) == true)
                {
                    _APP_UDP_SetIPv6Addresses();
                    if (app_udp_responderData.conformanceTest == true)
                    {
                        _APP_UDP_SetConformanceParameters();
                    }

                    SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "APP_UDP_RESPONDER: TCP/IP stack ready\r\n");
                    app_udp_responderData.state = APP_UDP_RESPONDER_STATE_OPENING_SERVER;
                }
            }

            break;
        }

        /* Waiting to G3 network join event */
        case APP_UDP_RESPONDER_STATE_WAIT_NETWORK_JOIN:
        {
            /* Nothing to do. State will be changed from
             * APP_UDP_NetworkJoined */
            break;
        }

        /* Opening UDP server */
        case APP_UDP_RESPONDER_STATE_OPENING_SERVER:
        {
            app_udp_responderData.socket = TCPIP_UDP_ServerOpen(IP_ADDRESS_TYPE_IPV6,
                    APP_UDP_SOCKET_PORT_CONFORMANCE, 0);
            if (app_udp_responderData.socket != INVALID_SOCKET)
            {
                app_udp_responderData.state = APP_UDP_RESPONDER_STATE_WAIT_FOR_CONNECTION;
            }

            break;
        }

        /* Waiting for connection ready */
        case APP_UDP_RESPONDER_STATE_WAIT_FOR_CONNECTION:
        {
            if (TCPIP_UDP_IsConnected(app_udp_responderData.socket) == true)
            {
                /* We got a connection */
                app_udp_responderData.state = APP_UDP_RESPONDER_STATE_SERVING_CONNECTION;
                SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, "APP_UDP_RESPONDER: UDP socket connected\r\n");
            }
            else
            {
                break;
            }
        }

        /* Serving connection on UDP port */
        case APP_UDP_RESPONDER_STATE_SERVING_CONNECTION:
        {
            uint16_t rxPayloadSize;
            uint8_t payloadFragment[32];
            uint8_t udpProtocol;

            if (TCPIP_UDP_IsConnected(app_udp_responderData.socket) == false)
            {
                app_udp_responderData.state = APP_UDP_RESPONDER_STATE_WAIT_FOR_CONNECTION;
                SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, "APP_UDP_RESPONDER: UDP socket disconnected\r\n");
                break;
            }

            /* Get number of bytes received */
            rxPayloadSize = TCPIP_UDP_GetIsReady(app_udp_responderData.socket);

            if (rxPayloadSize == 0)
            {
                /* No data received */
                break;
            }

            SYS_DEBUG_PRINT(SYS_ERROR_DEBUG, "APP_UDP_RESPONDER: %u bytes received\r\n", rxPayloadSize);

            /* Read first received byte (protocol) */
            TCPIP_UDP_Get(app_udp_responderData.socket, &udpProtocol);

            switch (udpProtocol)
            {
                case 1:
                {
                    /* In order to validate RFC6282 UDP header compression, an
                     * exchange of frames transported over UDP is required. For
                     * this purpose a very simple UDP responder needs to be
                     * implemented.
                     *  - The device listen to port 0xF0BF over UDP.
                     *  - The first byte of UDP payload indicate the message
                     *  type, the rest of the UDP payload correspond to the
                     *  message data:
                     *      - 0x01(UDP request): upon reception, the device must
                     *      send back an UDP frame to the original sender, using
                     *      the received frame source and destination ports for
                     *      the destination and source ports (respectively) of
                     *      the response frame, setting the message type to 0x02
                     *      (UDP reply) and copying the message data from the
                     *      request.
                     *      - 0x02 (UDP reply): this message is dropped upon
                     *      reception. */
                    uint16_t responseSize, availableTxSize, chunkSize;

                    /* Put the first byte (2: UDP reply) */
                    TCPIP_UDP_Put(app_udp_responderData.socket, 2);

                    /* Check available TX bytes in UDP socket */
                    responseSize = rxPayloadSize - 1;
                    availableTxSize = TCPIP_UDP_TxPutIsReady(app_udp_responderData.socket, responseSize);
                    if (responseSize > availableTxSize)
                    {
                        responseSize = availableTxSize;
                    }

                    /* Read the remaining UDP payload bytes and insert in UDP
                     * reply. Implemented in a loop, processing up to 32 bytes
                     * at a time. This limits memory usage while maximizing
                     * performance. */
                    chunkSize = 32;
                    for (uint16_t written = 0; written < responseSize; written += chunkSize)
                    {
                        if (written + chunkSize > responseSize)
                        {
                            /* Treat the last chunk */
                            chunkSize = responseSize - written;
                        }

                        TCPIP_UDP_ArrayGet(app_udp_responderData.socket, payloadFragment, chunkSize);
                        TCPIP_UDP_ArrayPut(app_udp_responderData.socket,
                                (const uint8_t *) payloadFragment, chunkSize);
                    }

                    /* Send the UDP reply */
                    TCPIP_UDP_Flush(app_udp_responderData.socket);
                    break;
                }

                case 3:
                {
                    /* The following extension is added to the UDP responder, in
                     * order to make the IUT generate ICMPv6 ECHO Request
                     * frames. The new message type 0x03 (ICMPv6 ECHO request
                     * trigger) is added: upon reception, the device must send
                     * back an ICMPv6 ECHO request frame to the original sender.
                     * The ICMPv6 Identifier, Sequence Number and Data fields
                     * are filled (in that order) using the received message
                     * data.
                     * Example: If an UDP message with a payload of
                     * "03 010203040506070809" is received, then an ICMPv6 echo
                     * request is sent back with an ICMPv6 content of
                     * "80 00 xxxx 0102 0304 0506070809" (where xxxx correspond
                     * to the ICMP checksum). */
                    UDP_SOCKET_INFO socketInfo;
                    IPV6_PACKET * pkt;
                    uint16_t identifier, sequenceNumber, icmpPayloadSize, availableTxSize, chunkSize;

                    /* Check payload length */
                    if (rxPayloadSize < 5)
                    {
                        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "APP_UDP_RESPONDER: Invalid "
                                "length in ICMPv6 ECHO request trigger message");
                    }

                    /* Get socket info to know the source and destination
                     * addresses */
                    TCPIP_UDP_SocketInfoGet(app_udp_responderData.socket, &socketInfo);

                    /* Get ICMPv6 identifier and sequence number fields */
                    TCPIP_UDP_ArrayGet(app_udp_responderData.socket, (uint8_t*) &identifier, 2);
                    TCPIP_UDP_ArrayGet(app_udp_responderData.socket, (uint8_t*) &sequenceNumber, 2);
                    identifier = TCPIP_Helper_ntohs(identifier);
                    sequenceNumber = TCPIP_Helper_ntohs(sequenceNumber);

                    /* Create ICMPv6 ECHO request packet */
                    pkt = TCPIP_ICMPV6_HeaderEchoRequestPut (app_udp_responderData.netHandle,
                            (const IPV6_ADDR*) &socketInfo.sourceIPaddress.v6Add,
                            (const IPV6_ADDR*) &socketInfo.destIPaddress.v6Add,
                            ICMPV6_INFO_ECHO_REQUEST, identifier, sequenceNumber);

                    /* Check available TX bytes in IPv6 packet */
                    icmpPayloadSize = rxPayloadSize - 5;
                    availableTxSize = TCPIP_IPV6_TxIsPutReady(pkt, icmpPayloadSize);
                    if (icmpPayloadSize > availableTxSize)
                    {
                        icmpPayloadSize = availableTxSize;
                    }

                    /* Read the remaining UDP payload bytes and insert in ICMPv6
                     * payload. Implemented in a loop, processing up to 32 bytes
                     * at a time. This limits memory usage while maximizing
                     * performance. */
                    chunkSize = 32;
                    for (uint16_t written = 0; written < icmpPayloadSize; written += chunkSize)
                    {
                        if (written + chunkSize > icmpPayloadSize)
                        {
                            /* Treat the last chunk */
                            chunkSize = icmpPayloadSize - written;
                        }

                        TCPIP_UDP_ArrayGet(app_udp_responderData.socket, payloadFragment, chunkSize);
                        TCPIP_IPV6_PutArray(pkt, (const uint8_t *) payloadFragment, chunkSize);
                    }

                    /* Send the ICMPv6 packet */
                    TCPIP_ICMPV6_Flush(pkt);
                    break;
                }

                case 5:
                {
                    /* The following extension is added to the UDP responder, in
                     * order to make the IUT change his RF configuration. The
                     * new message type 0x05 (RF configuration change) and 0x06
                     * (RF configuration confirmation) are added: upon reception
                     * of a RF configuration change message, the device must
                     * change its configuration according to the configuration
                     * indicated in the received message, then send back an RF
                     * configuration confirmation indicating the status of the
                     * change.
                     * RF configuration change (type 0x05): The configuration is
                     * defined using 3 or 4 bytes, directly following the
                     * message type identifier:
                     *  - The frequency band (1 byte), corresponding to the
                     *  parameter macFrequencyBand_RF defined in [1]
                     *  - The mode (1 byte), corresponding to the parameter
                     *  macOperatingMode_RF defined in [1]
                     *  - The frequency hopping activation (1 byte). The
                     *  following 2 values are possible:
                     *      - 0x00: Indicate that the frequency hopping
                     *      mechanism is deactivated, and so that the single
                     *      carrier mode is used. In this case, the channel
                     *      number that has to be used will be indicated by
                     *      the next byte.
                     *      - 0x01: Indicate that the frequency hopping
                     *      mechanism is activated (this case is not yet
                     *      possible as the frequency hopping mechanism has not
                     *      been specified)
                     *  - If (frequency hopping activation == 0x00), the channel
                     *  number (1 byte) is included, corresponding to the
                     *  parameter macChannelNumber_RF defined in [1]
                     *
                     * RF configuration confirmation (type 0x06): The status is
                     * defined by one byte directly following the message type
                     * identifier. The following values are possible:
                     * - 0x00, indicating that the RF configuration is successful
                     * - 0x01, indicating that the RF configuration failed
                     * - 0x02, indicating that the requested RF configuration is
                     *  unsupported by the device */
                    uint8_t result;

                    if ((rxPayloadSize >= 4) && (rxPayloadSize <= 5))
                    {
                        uint8_t rfConfigData[4];

                        /* Read configuration data */
                        TCPIP_UDP_ArrayGet(app_udp_responderData.socket,
                                rfConfigData, rxPayloadSize - 1);

                        /* Set RF configuration */
                        result = APP_G3_SetConfigRF(rfConfigData);
                        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "APP_UDP_RESPONDER: Change "
                                "RF configuration request");
                    }
                    else
                    {
                        result = 2;
                        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "APP_UDP_RESPONDER: Invalid "
                                "length in Change RF configuration request");
                    }

                    /* Send result */
                    TCPIP_UDP_Put(app_udp_responderData.socket, 6);
                    TCPIP_UDP_Put(app_udp_responderData.socket, result);
                    TCPIP_UDP_Flush(app_udp_responderData.socket);
                    break;
                }

                case 7:
                {
                    /* Trickle/Clusterhead/Jitter control request (type 0x07):
                     * upon reception, the device must change its configuration
                     * to set the following parameters and send back the
                     * Trickle/Clusterhead/Jitter control confirmation message
                     * (type 0x08) to confirm the activation. The configuration
                     * to apply will be defined by one byte directly following
                     * the message type identifier:
                     *      - 0x00 (deactivation):
                     *      - 0x01 (activation):
                     * Trickle/Clusterhead/Jitter control confirmation
                     * (type 0x08): this message is dropped upon reception. The
                     * status is defined by one byte directly following the
                     * message type identifier. The following values are
                     * possible:
                     * - 0x00, indicating that the requested change is successful
                     * - 0x01, indicating that the requested change failed */
                    uint8_t trickleActivation, result;

                    TCPIP_UDP_Get(app_udp_responderData.socket, &trickleActivation);
                    result = APP_G3_SetConformanceTrickleConfig(trickleActivation);

                    /* Send result */
                    TCPIP_UDP_Put(app_udp_responderData.socket, 8);
                    TCPIP_UDP_Put(app_udp_responderData.socket, result);
                    TCPIP_UDP_Flush(app_udp_responderData.socket);
                    SYS_DEBUG_PRINT(SYS_ERROR_INFO, "APP_UDP_RESPONDER: Trickle/Cluster/Jitter "
                            "activation (%hhu)\r\n", trickleActivation);
                    break;
                }

                case 9:
                {
                    /* Upon reception of a Start RF continuous TX mode message,
                     * the DUT shall activate the transmission of a D-M1 signal
                     * as defined in ETSI EN 303 204 v3.1.1 & 5.2.7. This
                     * unmodulated signal is emitted continuously, at the center
                     * frequency of the current band and channel which can be
                     * selected using Change RF configuration message (0x05). If
                     * the DUT is in a Frequency hopping mode, it shall use
                     * channel 0 for the continuous TX mode. This transmission
                     * shall be stopped when rebooting the DUT: after power-up,
                     * the DUT shall recover its normal behaviour.
                    */
                    APP_G3_SetContinuousTxRF();
                    SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "APP_UDP_RESPONDER: RF continuous TX request\r\n");
                    break;
                }

                default:
                {
                    SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "Drop UDP message\r\n");
                }
            }

            TCPIP_UDP_Discard(app_udp_responderData.socket);

            break;
        }

        /* Error state */
        case APP_UDP_RESPONDER_STATE_ERROR:
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

void APP_UDP_SetConformanceConfig ( void )
{
    app_udp_responderData.conformanceTest = true;

    if (app_udp_responderData.state > APP_UDP_RESPONDER_STATE_WAIT_TCPIP_READY)
    {
        /* TCP/IP stack is ready: set conformance parameters */
        _APP_UDP_SetConformanceParameters();
    }
}

void APP_UDP_NetworkJoined(void)
{
    if (app_udp_responderData.state == APP_UDP_RESPONDER_STATE_WAIT_NETWORK_JOIN)
    {
        _APP_UDP_SetIPv6Addresses();
        app_udp_responderData.state = APP_UDP_RESPONDER_STATE_OPENING_SERVER;
    }
}

void APP_UDP_NetworkDisconnected(void)
{
    /* Remove IPv6 addresses */
    TCPIP_IPV6_AddressUnicastRemove(app_udp_responderData.netHandle, &app_udp_responderData.linkLocalAddress);
    TCPIP_IPV6_AddressUnicastRemove(app_udp_responderData.netHandle, &app_udp_responderData.ulaAddress);

    if (app_udp_responderData.conformanceTest == true)
    {
        IPV6_ADDR multicastAddr;

        /* Remove multicast addresses */
        TCPIP_Helper_StringToIPv6Address(APP_UDP_IPV6_MULTICAST_0_CONFORMANCE, &multicastAddr);
        TCPIP_IPV6_MulticastListenerRemove(app_udp_responderData.netHandle, &multicastAddr);
        TCPIP_Helper_StringToIPv6Address(APP_UDP_IPV6_MULTICAST_1_CONFORMANCE, &multicastAddr);
        TCPIP_IPV6_MulticastListenerRemove(app_udp_responderData.netHandle, &multicastAddr);
    }

    /* Close UDP socket */
    TCPIP_UDP_Close(app_udp_responderData.socket);

    /* Wait for device to join the network again */
    app_udp_responderData.state = APP_UDP_RESPONDER_STATE_WAIT_NETWORK_JOIN;
}

/*******************************************************************************
 End of File
 */
