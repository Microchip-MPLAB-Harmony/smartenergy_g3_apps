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
 ******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "definitions.h"
#include "tcpip_manager_control.h"

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
    This structure should be initialized by the APP_UDP_RESPONDER_Initialize
    function.

    Application strings and buffers are be defined outside this structure.
*/

APP_UDP_RESPONDER_DATA app_udp_responderData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

static void _APP_UDP_RESPONDER_SetConformance(void)
{
    IPV6_ADDR multicastAddr;

    /* Set multi-cast addresses */
    TCPIP_Helper_StringToIPv6Address(APP_UDP_RESPONDER_IPV6_MULTICAST_0_CONFORMANCE, &multicastAddr);
    TCPIP_IPV6_MulticastListenerAdd(app_udp_responderData.netHandle, &multicastAddr);
    TCPIP_Helper_StringToIPv6Address(APP_UDP_RESPONDER_IPV6_MULTICAST_1_CONFORMANCE, &multicastAddr);
    TCPIP_IPV6_MulticastListenerAdd(app_udp_responderData.netHandle, &multicastAddr);
}

static void _APP_UDP_RESPONDER_SetIPv6Addresses(void)
{
    TCPIP_MAC_ADDR macAddrCoord;
    uint8_t* eui64;
    uint16_t shortAddr, panId, shortAddrCoord;

    /* Configure link-local address, based on PAN ID and Short Address */
    TCPIP_Helper_StringToIPv6Address(APP_TCPIP_IPV6_LINK_LOCAL_ADDRESS_G3, &app_udp_responderData.linkLocalAddress);
    shortAddr = APP_G3_MANAGEMENT_GetShortAddress();
    panId = APP_G3_MANAGEMENT_GetPanId();
    app_udp_responderData.linkLocalAddress.v[8] = (uint8_t) (panId >> 8);
    app_udp_responderData.linkLocalAddress.v[9] = (uint8_t) panId;
    app_udp_responderData.linkLocalAddress.v[14] = (uint8_t) (shortAddr >> 8);
    app_udp_responderData.linkLocalAddress.v[15] = (uint8_t) shortAddr;
    TCPIP_IPV6_UnicastAddressAdd(app_udp_responderData.netHandle,
            &app_udp_responderData.linkLocalAddress, 0, false);

    /* Configure Unique Local Link (ULA) address, based on PAN ID and Extended
     * Address */
    TCPIP_Helper_StringToIPv6Address(APP_TCPIP_IPV6_NETWORK_PREFIX_G3, &app_udp_responderData.uniqueLocalAddress);
    eui64 = APP_G3_MANAGEMENT_GetExtendedAddress();
    app_udp_responderData.uniqueLocalAddress.v[6] = (uint8_t) (panId >> 8);
    app_udp_responderData.uniqueLocalAddress.v[7] = (uint8_t) panId;
    app_udp_responderData.uniqueLocalAddress.v[8] = eui64[7];
    app_udp_responderData.uniqueLocalAddress.v[9] = eui64[6];
    app_udp_responderData.uniqueLocalAddress.v[10] = eui64[5];
    app_udp_responderData.uniqueLocalAddress.v[11] = eui64[4];
    app_udp_responderData.uniqueLocalAddress.v[12] = eui64[3];
    app_udp_responderData.uniqueLocalAddress.v[13] = eui64[2];
    app_udp_responderData.uniqueLocalAddress.v[14] = eui64[1];
    app_udp_responderData.uniqueLocalAddress.v[15] = eui64[0];
    TCPIP_IPV6_UnicastAddressAdd(app_udp_responderData.netHandle,
            &app_udp_responderData.uniqueLocalAddress, APP_TCPIP_IPV6_NETWORK_PREFIX_G3_LEN, false);

    /* Set PAN ID in TCP/IP stack in order to recognize all link-local addresses
     * in the network as neighbors */
    TCPIP_IPV6_G3PLC_PanIdSet(app_udp_responderData.netHandle, panId);

    /* Build coordinator link-local address based on PAN ID and Short Address */
    TCPIP_Helper_StringToIPv6Address(APP_TCPIP_IPV6_LINK_LOCAL_ADDRESS_G3, &app_udp_responderData.coordLinkLocalAddress);
    shortAddrCoord = APP_G3_MANAGEMENT_GetCoordinatorShortAddress();
    app_udp_responderData.coordLinkLocalAddress.v[8] = (uint8_t) (panId >> 8);
    app_udp_responderData.coordLinkLocalAddress.v[9] = (uint8_t) panId;
    app_udp_responderData.coordLinkLocalAddress.v[14] = (uint8_t) (shortAddrCoord >> 8);
    app_udp_responderData.coordLinkLocalAddress.v[15] = (uint8_t) shortAddrCoord;

    /* Build coordinator MAC address based on link-local address (not used) */
    memcpy(macAddrCoord.v, &app_udp_responderData.coordLinkLocalAddress.v[sizeof(IPV6_ADDR) - sizeof(TCPIP_MAC_ADDR)],
           sizeof(TCPIP_MAC_ADDR));

    /* Add coordinator address to NDP neighbor table as router */
    TCPIP_IPV6_NeighborAddressAdd(app_udp_responderData.netHandle, &app_udp_responderData.coordLinkLocalAddress,
            &macAddrCoord, TCPIP_IPV6_NEIGHBOR_FLAG_PERM | TCPIP_IPV6_NEIGHBOR_FLAG_ROUTER);
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
    /* Check the application's current state. */
    switch ( app_udp_responderData.state )
    {
        /* Application's initial state. */
        case APP_UDP_RESPONDER_STATE_WAIT_TCPIP_READY:
        {
            SYS_STATUS tcpipStat = TCPIP_STACK_Status(sysObj.tcpip);
            if (tcpipStat <= SYS_STATUS_ERROR)
            {
                SYS_DEBUG_MESSAGE(SYS_ERROR_ERROR, "APP_UDP_RESPONDER: TCP/IP stack initialization failed!\r\n");
                app_udp_responderData.state = APP_UDP_RESPONDER_STATE_ERROR;
                break;
            }
            else if(tcpipStat == SYS_STATUS_READY)
            {
                /* TCP/IP Stack ready: Set IPv6 addresses */
                app_udp_responderData.netHandle = TCPIP_STACK_NetHandleGet("G3ADPMAC");
                _APP_UDP_RESPONDER_SetIPv6Addresses();
                if (app_udp_responderData.conformanceTest == true)
                {
                    _APP_UDP_RESPONDER_SetConformance();
                }

                /* Next state (without break): open UDP server */
                app_udp_responderData.state = APP_UDP_RESPONDER_STATE_OPENING_SERVER;
            }
            else
            {
                break;
            }
        }

        /* Opening UDP server */
        case APP_UDP_RESPONDER_STATE_OPENING_SERVER:
        {
            app_udp_responderData.socket = TCPIP_UDP_ServerOpen(IP_ADDRESS_TYPE_IPV6,
                    APP_UDP_RESPONDER_SOCKET_PORT_CONFORMANCE, NULL);
            if (app_udp_responderData.socket != INVALID_SOCKET)
            {
                app_udp_responderData.state = APP_UDP_RESPONDER_STATE_SERVING_CONNECTION;
            }

            break;
        }

        /* Serving connection on UDP port */
        case APP_UDP_RESPONDER_STATE_SERVING_CONNECTION:
        {
            uint16_t rxPayloadSize;
            uint8_t payloadFragment[32];
            uint8_t udpProtocol;

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
                    IPV6_PACKET * pkt;
                    UDP_SOCKET_INFO socketInfo;
                    uint16_t identifier, sequenceNumber, icmpPayloadSize, availableTxSize, chunkSize;

                    /* Check payload length */
                    if (rxPayloadSize < 5)
                    {
                        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "APP_UDP_RESPONDER: Invalid "
                                "length in ICMPv6 ECHO request trigger message");
                    }

                    /* Get ICMPv6 identifier and sequence number fields */
                    TCPIP_UDP_ArrayGet(app_udp_responderData.socket, (uint8_t*) &identifier, 2);
                    TCPIP_UDP_ArrayGet(app_udp_responderData.socket, (uint8_t*) &sequenceNumber, 2);
                    identifier = TCPIP_Helper_ntohs(identifier);
                    sequenceNumber = TCPIP_Helper_ntohs(sequenceNumber);

                    /* Get socket info to know the source and destination addresses */
                    TCPIP_UDP_SocketInfoGet(app_udp_responderData.socket, &socketInfo);

                    /* Create ICMPv6 ECHO request packet */
                    pkt = TCPIP_ICMPV6_HeaderEchoRequestPut (app_udp_responderData.netHandle,
                            &socketInfo.destIPaddress.v6Add,
                            &socketInfo.sourceIPaddress.v6Add,
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

                case 4:
                {
                    /* Multicast traffic trigger:
                     * Upon reception, the device must send an UDP frame to the
                     * ff02::1 multicast address, using the received frame
                     * source and destination ports for the destination and
                     * source ports (respectively) of the response frame,
                     * setting the message type to 0x02 (UDP reply) and copying
                     * the message data from the request */
                    uint16_t responseSize, availableTxSize, chunkSize;
                    IPV6_ADDR multicastAddr;

                    /* Set multi-cast address as destination */
                    TCPIP_Helper_StringToIPv6Address(
                            APP_UDP_RESPONDER_IPV6_MULTICAST_0_CONFORMANCE, &multicastAddr);
                    TCPIP_UDP_DestinationIPAddressSet(app_udp_responderData.socket,
                            IP_ADDRESS_TYPE_IPV6, (IP_MULTI_ADDRESS*) &multicastAddr);

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
                        result = APP_G3_MANAGEMENT_SetConfigRF(rfConfigData);
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
                    result = APP_G3_MANAGEMENT_SetConformanceTrickleConfig(trickleActivation);

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
                    APP_G3_MANAGEMENT_SetContinuousTxRF();
                    SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "APP_UDP_RESPONDER: RF continuous TX request\r\n");
                    break;
                }

                case 10:
                {
                    /* LastGasp mode activation:
                     * Upon reception of this message, adpLastGasp is set to
                     * TRUE by the IUT, then an ICMPv6 Echo request is sent to
                     * multicast address ff02::1 by the IUT (required to have
                     * the IUT generate a broadcast message on activation).
                     * No confirmation message is defined or required.
                     * Validation of the LastGasp feature uses normal ICMPv6
                     * Echo messages, no further application layer modifications
                     * are required. Disabling LastGasp mode will be done by
                     * rebooting the DUT: after power-up, the DUT shall start
                     * with adpLastGasp set to FALSE, as defined in G3-PLC
                     * specification */
                    IPV6_ADDR multicastAddr;

                    APP_G3_MANAGEMENT_SetLastGaspMode();

                    /* Send ICMPv6 Echo Request to multi-cast address */
                    TCPIP_Helper_StringToIPv6Address(
                            APP_UDP_RESPONDER_IPV6_MULTICAST_0_CONFORMANCE, &multicastAddr);
                    TCPIP_ICMPV6_EchoRequestSend(app_udp_responderData.netHandle,
                            &multicastAddr, 0, 0, 10);
                    SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "APP_UDP_RESPONDER: Last Gasp activation request\r\n");
                    break;
                }

                default:
                {
                    SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "APP_UDP_RESPONDER: Drop UDP message\r\n");
                    break;
                }
            }

            /* Received UDP frame processed, we can discard it */
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

void APP_UDP_RESPONDER_SetConformanceConfig ( void )
{
    app_udp_responderData.conformanceTest = true;

    if ((app_udp_responderData.state > APP_UDP_RESPONDER_STATE_WAIT_TCPIP_READY) &&
            (app_udp_responderData.state != APP_UDP_RESPONDER_STATE_ERROR))
    {
        /* TCP/IP stack is ready: set conformance parameters */
        _APP_UDP_RESPONDER_SetConformance();
    }
}

void APP_UDP_RESPONDER_NetworkJoined(void)
{
    if ((app_udp_responderData.state > APP_UDP_RESPONDER_STATE_WAIT_TCPIP_READY) &&
            (app_udp_responderData.state != APP_UDP_RESPONDER_STATE_ERROR))
    {
        /* TCP/IP stack is ready: set IPv6 addresses */
        _APP_UDP_RESPONDER_SetIPv6Addresses();
    }
}

void APP_UDP_RESPONDER_NetworkDisconnected(void)
{
    /* Remove IPv6 addresses */
    TCPIP_IPV6_AddressUnicastRemove(app_udp_responderData.netHandle, &app_udp_responderData.linkLocalAddress);
    TCPIP_IPV6_AddressUnicastRemove(app_udp_responderData.netHandle, &app_udp_responderData.uniqueLocalAddress);

    /* Remove coordinator address from NDP neighbor table */
    TCPIP_IPV6_NeighborAddressDelete(app_udp_responderData.netHandle, &app_udp_responderData.coordLinkLocalAddress);
}

/*******************************************************************************
 End of File
 */
