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
    This structure should be initialized by the APP_UDP_METROLOGY_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_UDP_METROLOGY_DATA app_udp_metrologyData;

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
                /* TCP/IP Stack ready */
                app_udp_metrologyData.netHandle = TCPIP_STACK_NetHandleGet("G3ADPMAC");

                /* Next state (without break): open UDP server */
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
            app_udp_metrologyData.socket = TCPIP_UDP_ServerOpen(IP_ADDRESS_TYPE_IPV6,
                    APP_UDP_METROLOGY_SOCKET_PORT, NULL);
            if (app_udp_metrologyData.socket != INVALID_SOCKET)
            {
                app_udp_metrologyData.state = APP_UDP_METROLOGY_STATE_SERVING_CONNECTION;
            }

            break;
        }

        /* Serving connection on UDP port */
        case APP_UDP_METROLOGY_STATE_SERVING_CONNECTION:
        {
            uint16_t rxPayloadSize;
            uint8_t udpProtocol;

            /* Get number of bytes received */
            rxPayloadSize = TCPIP_UDP_GetIsReady(app_udp_metrologyData.socket);

            if (rxPayloadSize == 0)
            {
                /* No data received */
                break;
            }

            SYS_DEBUG_PRINT(SYS_ERROR_DEBUG, "APP_UDP_METROLOGY: %u bytes received\r\n", rxPayloadSize);

            /* Read first received byte (protocol) */
            TCPIP_UDP_Get(app_udp_metrologyData.socket, &udpProtocol);

            switch (udpProtocol)
            {
                case 1:
                {
                    /* Metrology data request. The response is 0x02 with
                     * metrology data (RMS instantaneous values) */
                    APP_UDP_METROLOGY_RESPONSE_DATA metData;
                    DRV_METROLOGY_RMS_SIGN rmsSign;

                    /* Put the first byte (0x02: Metrology data response) */
                    TCPIP_UDP_Put(app_udp_metrologyData.socket, 2);

                    /* Get RMS voltage values (without sign) */
                    APP_METROLOGY_GetRMS(RMS_UA, &metData.rmsUA, NULL);
                    APP_METROLOGY_GetRMS(RMS_UB, &metData.rmsUB, NULL);
                    APP_METROLOGY_GetRMS(RMS_UC, &metData.rmsUC, NULL);

                    /* Get RMS current values (without sign) */
                    APP_METROLOGY_GetRMS(RMS_IA, &metData.rmsIA, NULL);
                    APP_METROLOGY_GetRMS(RMS_IB, &metData.rmsIB, NULL);
                    APP_METROLOGY_GetRMS(RMS_IC, &metData.rmsIC, NULL);
                    APP_METROLOGY_GetRMS(RMS_INI, &metData.rmsINI, NULL);
                    APP_METROLOGY_GetRMS(RMS_INM, &metData.rmsINM, NULL);
                    APP_METROLOGY_GetRMS(RMS_INMI, &metData.rmsINMI, NULL);

                    /* Get RMS active power values (with sign) */
                    APP_METROLOGY_GetRMS(RMS_PT, (uint32_t*) &metData.rmsPT, &rmsSign);
                    if (rmsSign == RMS_SIGN_NEGATIVE)
                    {
                        metData.rmsPT = -metData.rmsPT;
                    }

                    APP_METROLOGY_GetRMS(RMS_PA, (uint32_t*) &metData.rmsPA, &rmsSign);
                    if (rmsSign == RMS_SIGN_NEGATIVE)
                    {
                        metData.rmsPA = -metData.rmsPA;
                    }

                    APP_METROLOGY_GetRMS(RMS_PB, (uint32_t*) &metData.rmsPB, &rmsSign);
                    if (rmsSign == RMS_SIGN_NEGATIVE)
                    {
                        metData.rmsPB = -metData.rmsPB;
                    }

                    APP_METROLOGY_GetRMS(RMS_PC, (uint32_t*) &metData.rmsPC, &rmsSign);
                    if (rmsSign == RMS_SIGN_NEGATIVE)
                    {
                        metData.rmsPC = -metData.rmsPC;
                    }

                    /* Get RMS reactive power values (with sign) */
                    APP_METROLOGY_GetRMS(RMS_QT, (uint32_t*) &metData.rmsQT, &rmsSign);
                    if (rmsSign == RMS_SIGN_NEGATIVE)
                    {
                        metData.rmsQT = -metData.rmsQT;
                    }

                    APP_METROLOGY_GetRMS(RMS_QA, (uint32_t*) &metData.rmsQA, &rmsSign);
                    if (rmsSign == RMS_SIGN_NEGATIVE)
                    {
                        metData.rmsQA = -metData.rmsQA;
                    }

                    APP_METROLOGY_GetRMS(RMS_QB, (uint32_t*) &metData.rmsQB, &rmsSign);
                    if (rmsSign == RMS_SIGN_NEGATIVE)
                    {
                        metData.rmsQB = -metData.rmsQB;
                    }

                    APP_METROLOGY_GetRMS(RMS_QC, (uint32_t*) &metData.rmsQC, &rmsSign);
                    if (rmsSign == RMS_SIGN_NEGATIVE)
                    {
                        metData.rmsQC = -metData.rmsQC;
                    }

                    /* Get RMS aparent power values (without sign) */
                    APP_METROLOGY_GetRMS(RMS_ST, &metData.rmsST, NULL);
                    APP_METROLOGY_GetRMS(RMS_SA, &metData.rmsSA, NULL);
                    APP_METROLOGY_GetRMS(RMS_SB, &metData.rmsSB, NULL);
                    APP_METROLOGY_GetRMS(RMS_SC, &metData.rmsSC, NULL);

                    /* Get frequency of the line voltage fundamental harmonic
                     * component determined by the Metrology library using the
                     * dominant phase */
                    APP_METROLOGY_GetRMS(RMS_FREQ, &metData.freq, NULL);

                    /* Get angles between the voltage and current vectors
                     * (with sign) */
                    APP_METROLOGY_GetRMS(RMS_ANGLEA, (uint32_t*) &metData.angleA, &rmsSign);
                    if (rmsSign == RMS_SIGN_NEGATIVE)
                    {
                        metData.angleA = -metData.angleA;
                    }

                    APP_METROLOGY_GetRMS(RMS_ANGLEB, (uint32_t*) &metData.angleB, &rmsSign);
                    if (rmsSign == RMS_SIGN_NEGATIVE)
                    {
                        metData.angleB = -metData.angleB;
                    }

                    APP_METROLOGY_GetRMS(RMS_ANGLEC, (uint32_t*) &metData.angleC, &rmsSign);
                    if (rmsSign == RMS_SIGN_NEGATIVE)
                    {
                        metData.angleC = -metData.angleC;
                    }

                    APP_METROLOGY_GetRMS(RMS_ANGLEN, (uint32_t*) &metData.angleN, &rmsSign);
                    if (rmsSign == RMS_SIGN_NEGATIVE)
                    {
                        metData.angleN = -metData.angleN;
                    }

                    /* Insert metrology data in UDP reply */
                    TCPIP_UDP_ArrayPut(app_udp_metrologyData.socket,
                        (const uint8_t *) &metData, sizeof(metData));

                    /* Send the UDP reply */
                    TCPIP_UDP_Flush(app_udp_metrologyData.socket);
                    break;
                }

                default:
                {
                    SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "APP_UDP_METROLOGY: Drop UDP message\r\n");
                    break;
                }
            }

            /* Received UDP frame processed, we can discard it */
            TCPIP_UDP_Discard(app_udp_metrologyData.socket);

            break;
        }

        /* Error state */
        case APP_UDP_METROLOGY_STATE_ERROR:
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


/*******************************************************************************
 End of File
 */
