/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_tcpip_management.c

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
    This structure should be initialized by the APP_TCPIP_MANAGEMENT_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_TCPIP_MANAGEMENT_DATA app_tcpip_managementData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

static void _APP_TCPIP_MANAGEMENT_SetConformance(void)
{
    IPV6_ADDR multicastAddr;

    /* Set multi-cast addresses */
    TCPIP_Helper_StringToIPv6Address(APP_TCPIP_MANAGEMENT_IPV6_MULTICAST_0_CONFORMANCE, &multicastAddr);
    TCPIP_IPV6_MulticastListenerAdd(app_tcpip_managementData.netHandle, &multicastAddr);
    TCPIP_Helper_StringToIPv6Address(APP_TCPIP_MANAGEMENT_IPV6_MULTICAST_1_CONFORMANCE, &multicastAddr);
    TCPIP_IPV6_MulticastListenerAdd(app_tcpip_managementData.netHandle, &multicastAddr);
}

static void _APP_TCPIP_MANAGEMENT_SetIPv6Addresses(void)
{
    TCPIP_MAC_ADDR macAddrCoord;
    uint8_t* eui64;
    uint16_t shortAddr, panId, shortAddrCoord;

    /* Configure link-local address, based on PAN ID and Short Address */
    TCPIP_Helper_StringToIPv6Address(APP_TCPIP_MANAGEMENT_IPV6_LINK_LOCAL_ADDRESS_G3, &app_tcpip_managementData.linkLocalAddress);
    shortAddr = APP_G3_MANAGEMENT_GetShortAddress();
    panId = APP_G3_MANAGEMENT_GetPanId();
    app_tcpip_managementData.linkLocalAddress.v[8] = (uint8_t) (panId >> 8);
    app_tcpip_managementData.linkLocalAddress.v[9] = (uint8_t) panId;
    app_tcpip_managementData.linkLocalAddress.v[14] = (uint8_t) (shortAddr >> 8);
    app_tcpip_managementData.linkLocalAddress.v[15] = (uint8_t) shortAddr;
    TCPIP_IPV6_UnicastAddressAdd(app_tcpip_managementData.netHandle,
            &app_tcpip_managementData.linkLocalAddress, 0, false);

    /* Configure Unique Local Link (ULA) address, based on PAN ID and Extended
     * Address */
    TCPIP_Helper_StringToIPv6Address(APP_TCPIP_MANAGEMENT_IPV6_NETWORK_PREFIX_G3, &app_tcpip_managementData.uniqueLocalAddress);
    eui64 = APP_G3_MANAGEMENT_GetExtendedAddress();
    app_tcpip_managementData.uniqueLocalAddress.v[6] = (uint8_t) (panId >> 8);
    app_tcpip_managementData.uniqueLocalAddress.v[7] = (uint8_t) panId;
    app_tcpip_managementData.uniqueLocalAddress.v[8] = eui64[7];
    app_tcpip_managementData.uniqueLocalAddress.v[9] = eui64[6];
    app_tcpip_managementData.uniqueLocalAddress.v[10] = eui64[5];
    app_tcpip_managementData.uniqueLocalAddress.v[11] = eui64[4];
    app_tcpip_managementData.uniqueLocalAddress.v[12] = eui64[3];
    app_tcpip_managementData.uniqueLocalAddress.v[13] = eui64[2];
    app_tcpip_managementData.uniqueLocalAddress.v[14] = eui64[1];
    app_tcpip_managementData.uniqueLocalAddress.v[15] = eui64[0];
    TCPIP_IPV6_UnicastAddressAdd(app_tcpip_managementData.netHandle,
            &app_tcpip_managementData.uniqueLocalAddress, APP_TCPIP_MANAGEMENT_IPV6_NETWORK_PREFIX_G3_LEN, false);

    /* Set PAN ID in TCP/IP stack in order to recognize all link-local addresses
     * in the network as neighbors */
    TCPIP_IPV6_G3PLC_PanIdSet(app_tcpip_managementData.netHandle, panId);

    /* Build coordinator link-local address based on PAN ID and Short Address */
    TCPIP_Helper_StringToIPv6Address(APP_TCPIP_MANAGEMENT_IPV6_LINK_LOCAL_ADDRESS_G3, &app_tcpip_managementData.coordLinkLocalAddress);
    shortAddrCoord = APP_G3_MANAGEMENT_GetCoordinatorShortAddress();
    app_tcpip_managementData.coordLinkLocalAddress.v[8] = (uint8_t) (panId >> 8);
    app_tcpip_managementData.coordLinkLocalAddress.v[9] = (uint8_t) panId;
    app_tcpip_managementData.coordLinkLocalAddress.v[14] = (uint8_t) (shortAddrCoord >> 8);
    app_tcpip_managementData.coordLinkLocalAddress.v[15] = (uint8_t) shortAddrCoord;

    /* Build coordinator MAC address based on link-local address (not used) */
    memcpy(macAddrCoord.v, &app_tcpip_managementData.coordLinkLocalAddress.v[sizeof(IPV6_ADDR) - sizeof(TCPIP_MAC_ADDR)],
           sizeof(TCPIP_MAC_ADDR));

    /* Add coordinator address to NDP neighbor table as router */
    TCPIP_IPV6_NeighborAddressAdd(app_tcpip_managementData.netHandle, &app_tcpip_managementData.coordLinkLocalAddress,
            &macAddrCoord, TCPIP_IPV6_NEIGHBOR_FLAG_PERM | TCPIP_IPV6_NEIGHBOR_FLAG_ROUTER);
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_TCPIP_MANAGEMENT_Initialize ( void )

  Remarks:
    See prototype in app_tcpip_management.h.
 */

void APP_TCPIP_MANAGEMENT_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    app_tcpip_managementData.state = APP_TCPIP_MANAGEMENT_STATE_WAIT_TCPIP_READY;
    app_tcpip_managementData.conformanceTest = false;

    /* Create semaphore. It is used to suspend task. */
    OSAL_SEM_Create(&app_tcpip_managementData.semaphoreID, OSAL_SEM_TYPE_BINARY, 0, 0);
}


/******************************************************************************
  Function:
    void APP_TCPIP_MANAGEMENT_Tasks ( void )

  Remarks:
    See prototype in app_tcpip_management.h.
 */

void APP_TCPIP_MANAGEMENT_Tasks ( void )
{
    /* Check the application's current state. */
    switch ( app_tcpip_managementData.state )
    {
        /* Application's initial state. */
        case APP_TCPIP_MANAGEMENT_STATE_WAIT_TCPIP_READY:
        {
            SYS_STATUS tcpipStat = TCPIP_STACK_Status(sysObj.tcpip);
            if (tcpipStat <= SYS_STATUS_ERROR)
            {
                SYS_DEBUG_MESSAGE(SYS_ERROR_ERROR, "APP_TCPIP_MANAGEMENT: TCP/IP stack initialization failed!\r\n");
                app_tcpip_managementData.state = APP_TCPIP_MANAGEMENT_STATE_ERROR;
                break;
            }
            else if(tcpipStat == SYS_STATUS_READY)
            {
                /* TCP/IP Stack ready: Set IPv6 addresses */
                app_tcpip_managementData.netHandle = TCPIP_STACK_NetHandleGet("G3ADPMAC");
                _APP_TCPIP_MANAGEMENT_SetIPv6Addresses();
                if (app_tcpip_managementData.conformanceTest == true)
                {
                    _APP_TCPIP_MANAGEMENT_SetConformance();
                }

                /* Next state (without break): open UDP server */
                app_tcpip_managementData.state = APP_TCPIP_MANAGEMENT_STATE_CONFIGURED;
            }
            else
            {
                break;
            }
        }

        /* TCP/IP stack configured and IPv6 addresses set */
        case APP_TCPIP_MANAGEMENT_STATE_CONFIGURED:
        /* Error state */
        case APP_TCPIP_MANAGEMENT_STATE_ERROR:
        /* The default state should never be executed. */
        default:
        {
            /* Nothing more to do. Suspend task forever (RTOS mode) */
            if (app_tcpip_managementData.semaphoreID != 0)
            {
                OSAL_SEM_Pend(&app_tcpip_managementData.semaphoreID, OSAL_WAIT_FOREVER);
            }

            break;
        }
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Interface Functions
// *****************************************************************************
// *****************************************************************************

void APP_TCPIP_MANAGEMENT_SetConformanceConfig ( void )
{
    app_tcpip_managementData.conformanceTest = true;

    if ((app_tcpip_managementData.state > APP_TCPIP_MANAGEMENT_STATE_WAIT_TCPIP_READY) &&
            (app_tcpip_managementData.state != APP_TCPIP_MANAGEMENT_STATE_ERROR))
    {
        /* TCP/IP stack is ready: set conformance parameters */
        _APP_TCPIP_MANAGEMENT_SetConformance();
    }
}

void APP_TCPIP_MANAGEMENT_NetworkJoined(void)
{
    if ((app_tcpip_managementData.state > APP_TCPIP_MANAGEMENT_STATE_WAIT_TCPIP_READY) &&
            (app_tcpip_managementData.state != APP_TCPIP_MANAGEMENT_STATE_ERROR))
    {
        /* TCP/IP stack is ready: set IPv6 addresses */
        _APP_TCPIP_MANAGEMENT_SetIPv6Addresses();
    }
}

void APP_TCPIP_MANAGEMENT_NetworkDisconnected(void)
{
    /* Remove IPv6 addresses */
    TCPIP_IPV6_AddressUnicastRemove(app_tcpip_managementData.netHandle, &app_tcpip_managementData.linkLocalAddress);
    TCPIP_IPV6_AddressUnicastRemove(app_tcpip_managementData.netHandle, &app_tcpip_managementData.uniqueLocalAddress);

    /* Remove coordinator address from NDP neighbor table */
    TCPIP_IPV6_NeighborAddressDelete(app_tcpip_managementData.netHandle, &app_tcpip_managementData.coordLinkLocalAddress);
}

/*******************************************************************************
 End of File
 */
