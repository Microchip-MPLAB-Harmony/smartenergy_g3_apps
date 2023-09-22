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
    IPV6_ADDR ipv6Address;
    uint16_t panId;
    uint8_t* eui64;

    /* Configure link-local address, based on PAN ID and Short Address */
    TCPIP_Helper_StringToIPv6Address(APP_TCPIP_MANAGEMENT_IPV6_LINK_LOCAL_ADDRESS_G3, &ipv6Address);
    panId = APP_G3_MANAGEMENT_GetPanId();
    ipv6Address.v[8] = (uint8_t) (panId >> 8);
    ipv6Address.v[9] = (uint8_t) panId;
    ipv6Address.v[14] = (uint8_t) (APP_G3_MANAGEMENT_SHORT_ADDRESS >> 8);
    ipv6Address.v[15] = (uint8_t) APP_G3_MANAGEMENT_SHORT_ADDRESS;
    TCPIP_IPV6_UnicastAddressAdd(app_tcpip_managementData.netHandle, &ipv6Address, 0, false);

    /* Configure Unique Local Link (ULA) address, based on PAN ID and Extended
     * Address */
    TCPIP_Helper_StringToIPv6Address(APP_TCPIP_MANAGEMENT_IPV6_NETWORK_PREFIX_G3, &ipv6Address);
    eui64 = APP_G3_MANAGEMENT_GetExtendedAddress();
    ipv6Address.v[6] = (uint8_t) (panId >> 8);
    ipv6Address.v[7] = (uint8_t) panId;
    ipv6Address.v[8] = eui64[7];
    ipv6Address.v[9] = eui64[6];
    ipv6Address.v[10] = eui64[5];
    ipv6Address.v[11] = eui64[4];
    ipv6Address.v[12] = eui64[3];
    ipv6Address.v[13] = eui64[2];
    ipv6Address.v[14] = eui64[1];
    ipv6Address.v[15] = eui64[0];
    TCPIP_IPV6_UnicastAddressAdd(app_tcpip_managementData.netHandle,
            &ipv6Address, APP_TCPIP_MANAGEMENT_IPV6_NETWORK_PREFIX_G3_LEN, false);

    /* Set PAN ID in TCP/IP stack in order to recognize all link-local addresses
     * in the network as neighbors */
    TCPIP_IPV6_G3PLC_PanIdSet(app_tcpip_managementData.netHandle, panId);
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

/*******************************************************************************
 End of File
 */
