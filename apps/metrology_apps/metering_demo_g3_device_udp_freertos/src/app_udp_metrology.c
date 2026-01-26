/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_udp_metrology.c

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
#include "tcpip_private.h"

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
    This structure should be initialized by the APP_UDP_METROLOGY_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_UDP_METROLOGY_DATA app_udp_metrologyData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

void _APP_UDP_METROLOGY_UdpRxCallback(UDP_SOCKET hUDP, TCPIP_NET_HANDLE hNet, TCPIP_UDP_SIGNAL_TYPE sigType, const void* param)
{
    uint16_t rxPayloadSize;
    uint8_t udpProtocol;

    /* Get number of bytes received */
    rxPayloadSize = TCPIP_UDP_GetIsReady(hUDP);

    if (rxPayloadSize == 0)
    {
        /* No data received */
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "APP_UDP_RESPONDER: UDP message without payload\r\n");
        return;
    }

    SYS_DEBUG_PRINT(SYS_ERROR_DEBUG, "APP_UDP_METROLOGY: %u bytes received\r\n", rxPayloadSize);

    /* Read first received byte (protocol) */
    TCPIP_UDP_Get(hUDP, &udpProtocol);

    switch (udpProtocol)
    {
        case 1:
        {
            /* Metrology data request. The response is 0x02 with
             * metrology data (RMS instantaneous values) */
            APP_UDP_METROLOGY_RESPONSE_DATA metData;

            /* Put the first byte (0x02: Metrology data response) */
            TCPIP_UDP_Put(hUDP, 2);

            /* Get RMS voltage values */
            APP_METROLOGY_GetMeasure(MEASURE_UA_RMS, &metData.rmsUA, false);
            APP_METROLOGY_GetMeasure(MEASURE_UB_RMS, &metData.rmsUB, false);
            APP_METROLOGY_GetMeasure(MEASURE_UC_RMS, &metData.rmsUC, false);

            /* Get RMS current values */
            APP_METROLOGY_GetMeasure(MEASURE_IA_RMS, &metData.rmsIA, false);
            APP_METROLOGY_GetMeasure(MEASURE_IB_RMS, &metData.rmsIB, false);
            APP_METROLOGY_GetMeasure(MEASURE_IC_RMS, &metData.rmsIC, false);
            APP_METROLOGY_GetMeasure(MEASURE_INI_RMS, &metData.rmsINI, false);
            APP_METROLOGY_GetMeasure(MEASURE_INM_RMS, &metData.rmsINM, false);
            APP_METROLOGY_GetMeasure(MEASURE_INMI_RMS, &metData.rmsINMI, false);

            /* Get RMS active power values */
            APP_METROLOGY_GetMeasure(MEASURE_PT, &metData.rmsPT, false);
            APP_METROLOGY_GetMeasure(MEASURE_PA, &metData.rmsPA, false);
            APP_METROLOGY_GetMeasure(MEASURE_PB, &metData.rmsPB, false);
            APP_METROLOGY_GetMeasure(MEASURE_PC, &metData.rmsPC, false);
            
            /* Get RMS reactive power values */
            APP_METROLOGY_GetMeasure(MEASURE_QT, &metData.rmsQT, false);
            APP_METROLOGY_GetMeasure(MEASURE_QA, &metData.rmsQA, false);
            APP_METROLOGY_GetMeasure(MEASURE_QB, &metData.rmsQB, false);
            APP_METROLOGY_GetMeasure(MEASURE_QC, &metData.rmsQC, false);

            /* Get RMS aparent power values */
            APP_METROLOGY_GetMeasure(MEASURE_ST, &metData.rmsST, NULL);
            APP_METROLOGY_GetMeasure(MEASURE_SA, &metData.rmsSA, NULL);
            APP_METROLOGY_GetMeasure(MEASURE_SB, &metData.rmsSB, NULL);
            APP_METROLOGY_GetMeasure(MEASURE_SC, &metData.rmsSC, NULL);

            /* Get frequency of the line voltage fundamental harmonic
             * component determined by the Metrology library using the
             * dominant phase */
            APP_METROLOGY_GetMeasure(MEASURE_FREQ, &metData.freq, NULL);

            /* Get angles between the voltage and current vectors */
            APP_METROLOGY_GetMeasure(MEASURE_ANGLEA, &metData.angleA, false);
            APP_METROLOGY_GetMeasure(MEASURE_ANGLEB, &metData.angleB, false);
            APP_METROLOGY_GetMeasure(MEASURE_ANGLEC, &metData.angleC, false);
            APP_METROLOGY_GetMeasure(MEASURE_ANGLEN, &metData.angleN, false);

            /* Insert metrology data in UDP reply */
            TCPIP_UDP_ArrayPut(hUDP, (const uint8_t *) &metData, sizeof(metData));

            /* Send the UDP reply */
            TCPIP_UDP_Flush(hUDP);
            break;
        }

        default:
        {
            SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "APP_UDP_METROLOGY: Drop UDP message\r\n");
            break;
        }
    }

    /* Received UDP frame processed, we can discard it */
    TCPIP_UDP_Discard(hUDP);
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_UDP_METROLOGY_Initialize ( void )

  Remarks:
    See prototype in app_udp_metrology.h.
 */

void APP_UDP_METROLOGY_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    app_udp_metrologyData.state = APP_UDP_METROLOGY_STATE_WAIT_TCPIP_READY;

    /* Create semaphore. It is used to suspend task. */
    OSAL_SEM_Create(&app_udp_metrologyData.semaphoreID, OSAL_SEM_TYPE_BINARY, 0, 0);
}


/******************************************************************************
  Function:
    void APP_UDP_METROLOGY_Tasks ( void )

  Remarks:
    See prototype in app_udp_metrology.h.
 */

void APP_UDP_METROLOGY_Tasks ( void )
{
    /* Check the application's current state. */
    switch ( app_udp_metrologyData.state )
    {
        /* Application's initial state. */
        case APP_UDP_METROLOGY_STATE_WAIT_TCPIP_READY:
        {
            SYS_STATUS tcpipStat = TCPIP_STACK_Status(sysObj.tcpip);
            if (tcpipStat <= SYS_STATUS_ERROR)
            {
                SYS_DEBUG_MESSAGE(SYS_ERROR_ERROR, "APP_UDP_METROLOGY: TCP/IP stack initialization failed!\r\n");
                app_udp_metrologyData.state = APP_UDP_METROLOGY_STATE_ERROR;
                break;
            }
            else if(tcpipStat == SYS_STATUS_READY)
            {
                /* TCP/IP Stack ready.
                 * Next state (without break): open UDP server */
                app_udp_metrologyData.state = APP_UDP_METROLOGY_STATE_OPENING_SERVER;
            }
            else
            {
                break;
            }
        }

        /* Opening UDP server */
        case APP_UDP_METROLOGY_STATE_OPENING_SERVER:
        {
            UDP_SOCKET socket = TCPIP_UDP_ServerOpen(IP_ADDRESS_TYPE_IPV6,
                    APP_UDP_METROLOGY_SOCKET_PORT, NULL);
            if (socket != INVALID_SOCKET)
            {
                /* Register callback handler for RX data */
                TCPIP_UDP_SignalHandlerRegister(socket, TCPIP_UDP_SIGNAL_RX_DATA,
                        _APP_UDP_METROLOGY_UdpRxCallback, NULL);

                app_udp_metrologyData.state = APP_UDP_METROLOGY_STATE_SERVING_CONNECTION;
            }

            break;
        }

        /* Serving connection on UDP port */
        case APP_UDP_METROLOGY_STATE_SERVING_CONNECTION:
        /* Error state */
        case APP_UDP_METROLOGY_STATE_ERROR:
        /* The default state should never be executed. */
        default:
        {
            /* Nothing more to do. Suspend task forever (RTOS mode) */
            if (app_udp_metrologyData.semaphoreID != 0)
            {
                OSAL_SEM_Pend(&app_udp_metrologyData.semaphoreID, OSAL_WAIT_FOREVER);
            }
            break;
        }
    }
}


/*******************************************************************************
 End of File
 */
