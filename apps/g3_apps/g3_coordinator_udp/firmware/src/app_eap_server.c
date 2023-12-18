/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_eap_server.c

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
#include "app_eap_server.h"
#include "stack/g3/adaptation/adp.h"
#include "stack/g3/adaptation/lbp_coord.h"
#include "service/pcoup/srv_pcoup.h"
#include "system/debug/sys_debug.h"

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
    This structure should be initialized by the APP_EAP_SERVER_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_EAP_SERVER_DATA app_eap_serverData;

/* List of extended addresses of joined devices */
static ADP_EXTENDED_ADDRESS app_eap_serverExtAddrList[APP_EAP_SERVER_MAX_DEVICES];

/* List of extended addresses of blacklisted devices */
static ADP_EXTENDED_ADDRESS app_eap_serverBlacklist[APP_EAP_SERVER_MAX_DEVICES];

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Constants
// *****************************************************************************
// *****************************************************************************

/* Keys */
static const uint8_t app_eap_serverPSK[16] = APP_EAP_SERVER_PSK_KEY;
static const uint8_t app_eap_serverPSKconformance[16] = APP_EAP_SERVER_PSK_KEY_CONFORMANCE;
static const uint8_t app_eap_serverGMK[16] = APP_EAP_SERVER_GMK_KEY;

/* Null extended address, all 0's */
static const ADP_EXTENDED_ADDRESS app_eap_serverNullAddress = {
    .value = {0, 0, 0, 0, 0, 0, 0, 0}
};

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

static void _LBP_COORD_JoinRequestIndication(uint8_t* pLbdAddress)
{
    uint16_t assignedAddress = 0xFFFF;

    if (app_eap_serverData.rekey == false)
    {
        bool blacklisted = false;

        /* Check if device is in blacklist */
        for (uint16_t i = 0; i < app_eap_serverData.blacklistSize; i++)
        {
            if (memcmp(pLbdAddress, app_eap_serverBlacklist[i].value, ADP_ADDRESS_64BITS) == 0)
            {
                blacklisted = true;
                break;
            }
        }

        if (blacklisted == false)
        {
            /* If EUI64 is already in list, remove it and give a new short
             * address */
            for (uint16_t i = 0; i < APP_EAP_SERVER_MAX_DEVICES; i++)
            {
                if (memcmp(app_eap_serverExtAddrList[i].value, pLbdAddress, ADP_ADDRESS_64BITS) == 0)
                {
                    /* EUI64 found in the list */
                    memset(app_eap_serverExtAddrList[i].value, 0, ADP_ADDRESS_64BITS);
                    app_eap_serverData.numDevicesJoined--;
                    break;
                }
            }

            if (app_eap_serverData.numShortAddrAssigned < APP_EAP_SERVER_MAX_DEVICES)
            {
                assignedAddress = APP_EAP_SERVER_INITIAL_SHORT_ADDRESS +
                        app_eap_serverData.numShortAddrAssigned++;
            }
            else if (app_eap_serverData.numDevicesJoined < APP_EAP_SERVER_MAX_DEVICES)
            {
                /* End of the list reached: Go through the list to find free
                 * positions. */
                for (uint16_t i = 0; i < APP_EAP_SERVER_MAX_DEVICES; i++)
                {
                    if (memcmp(app_eap_serverExtAddrList[i].value,
                            app_eap_serverNullAddress.value, ADP_ADDRESS_64BITS) == 0)
                    {
                        /* Free position */
                        assignedAddress = APP_EAP_SERVER_INITIAL_SHORT_ADDRESS + i;
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
    if (app_eap_serverData.rekey == false)
    {
        if ((assignedAddress >= APP_EAP_SERVER_INITIAL_SHORT_ADDRESS) &&
                (assignedAddress < (APP_EAP_SERVER_INITIAL_SHORT_ADDRESS + APP_EAP_SERVER_MAX_DEVICES)))
        {
            uint16_t index = assignedAddress - APP_EAP_SERVER_INITIAL_SHORT_ADDRESS;

            /* After the join process finishes, the entry is added to the
             * list */
            memcpy(app_eap_serverExtAddrList[index].value, pLbdAddress, ADP_ADDRESS_64BITS);
            app_eap_serverData.numDevicesJoined++;

            SYS_DEBUG_PRINT(SYS_ERROR_DEBUG, "APP_EAP_SERVER: New device joined (Short Address: 0x%04X,"
                    " EUI64: 0x%02X%02X%02X%02X%02X%02X%02X%02X)\r\n", assignedAddress,
                    pLbdAddress[0], pLbdAddress[1], pLbdAddress[2], pLbdAddress[3],
                    pLbdAddress[4], pLbdAddress[5], pLbdAddress[6], pLbdAddress[7]);
        }
    }
    else
    {
        uint16_t shortAddr;
        ADP_EXTENDED_ADDRESS* pExtAddr;
        bool rekeyFinished = true;

        /* Send the next re-keying process */
        for (uint16_t i = app_eap_serverData.rekeyIndex; i < app_eap_serverData.numShortAddrAssigned; i++)
        {
            /* Check not null address */
            if (memcmp(app_eap_serverExtAddrList[i].value,
                    app_eap_serverNullAddress.value, ADP_ADDRESS_64BITS) != 0)
            {
                shortAddr = APP_EAP_SERVER_INITIAL_SHORT_ADDRESS + i;
                pExtAddr = &app_eap_serverExtAddrList[i];

                LBP_Rekey(shortAddr, pExtAddr, app_eap_serverData.rekeyPhaseDistribute);
                app_eap_serverData.rekeyIndex = i + 1;
                rekeyFinished = false;
                break;
            }
        }

        if (rekeyFinished == true)
        {
            if (app_eap_serverData.rekeyPhaseDistribute == true)
            {
                /* All devices have been provided with the new GMK. Next phase:
                 * send GMK activation  to all joined devices */
                for (uint16_t i = 0; i < app_eap_serverData.numShortAddrAssigned; i++)
                {
                    if (memcmp(app_eap_serverExtAddrList[i].value,
                            app_eap_serverNullAddress.value, ADP_ADDRESS_64BITS) != 0)
                    {
                        /* First not null address */
                        shortAddr = APP_EAP_SERVER_INITIAL_SHORT_ADDRESS + i;
                        pExtAddr = &app_eap_serverExtAddrList[i];
                        LBP_Rekey(shortAddr, pExtAddr, false);
                        app_eap_serverData.rekeyIndex = i + 1;
                        app_eap_serverData.rekeyPhaseDistribute = false;
                        break;
                    }
                }
            }
            else
            {
                /* End of re-keying process */
                LBP_SetRekeyPhase(false);
                LBP_ActivateNewKey();
                app_eap_serverData.rekey = false;
            }
        }
    }
}

static void _LBP_COORD_LeaveIndication(uint16_t networkAddress)
{
    if ((app_eap_serverData.numDevicesJoined > 0) &&
            (networkAddress >= APP_EAP_SERVER_INITIAL_SHORT_ADDRESS) &&
            (networkAddress < (APP_EAP_SERVER_INITIAL_SHORT_ADDRESS + APP_EAP_SERVER_MAX_DEVICES)))
    {
        uint16_t index = networkAddress - APP_EAP_SERVER_INITIAL_SHORT_ADDRESS;

        /* Remove the device from the joined devices list */
        memset(app_eap_serverExtAddrList[index].value, 0, ADP_ADDRESS_64BITS);
        app_eap_serverData.numDevicesJoined--;

        SYS_DEBUG_PRINT(SYS_ERROR_DEBUG, "APP_EAP_SERVER: Device left "
                "(Short Address: 0x%04X)\r\n", networkAddress);
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_EAP_SERVER_Initialize ( void )

  Remarks:
    See prototype in app_eap_server.h.
 */

void APP_EAP_SERVER_Initialize ( void )
{
    /* Place the application's state machine in its initial state. */
    app_eap_serverData.state = APP_EAP_SERVER_STATE_WAIT_NETWORK_STARTED;

    /* Initialize application variables */
    memset(app_eap_serverExtAddrList, 0, sizeof(app_eap_serverExtAddrList));
    memset(app_eap_serverBlacklist, 0, sizeof(app_eap_serverBlacklist));
    app_eap_serverData.numShortAddrAssigned = 0;
    app_eap_serverData.numDevicesJoined = 0;
    app_eap_serverData.rekey = false;
}


/******************************************************************************
  Function:
    void APP_EAP_SERVER_Tasks ( void )

  Remarks:
    See prototype in app_eap_server.h.
 */

void APP_EAP_SERVER_Tasks ( void )
{
    /* Check the application's current state. */
    switch ( app_eap_serverData.state )
    {
        /* Application's initial state. */
        case APP_EAP_SERVER_STATE_WAIT_NETWORK_STARTED:
        {
            /* Check ADP status */
            ADP_STATUS adpStatus = ADP_Status();
            if (adpStatus == ADP_STATUS_LBP_CONNECTED)
            {
                SRV_PLC_PCOUP_BRANCH plcBranch;
                ADP_BAND plcBand;
                LBP_NOTIFICATIONS_COORD lbpCoordNotifications;
                LBP_SET_PARAM_CONFIRM lbpSetConfirm;

                /* Get configured PLC band */
                plcBranch = SRV_PCOUP_Get_Default_Branch();
                plcBand = SRV_PCOUP_Get_Phy_Band(plcBranch);

                /* Initialize LoWPAN Bootstrapping Protocol (LBP) in Coordinator
                 * mode, set call-backs and set PSK and GMK keys  */
                LBP_InitCoord(plcBand == ADP_BAND_ARIB);
                lbpCoordNotifications.joinRequestIndication = _LBP_COORD_JoinRequestIndication;
                lbpCoordNotifications.joinCompleteIndication = _LBP_COORD_JoinCompleteIndication;
                lbpCoordNotifications.leaveIndication = _LBP_COORD_LeaveIndication;
                LBP_SetNotificationsCoord(&lbpCoordNotifications);
                LBP_SetParamCoord(LBP_IB_GMK, 0, 16, app_eap_serverGMK, &lbpSetConfirm);
                if (app_eap_serverData.conformanceTest == false)
                {
                    LBP_SetParamCoord(LBP_IB_PSK, 0, 16, app_eap_serverPSK, &lbpSetConfirm);
                }
                else
                {
                    LBP_SetParamCoord(LBP_IB_PSK, 0, 16, app_eap_serverPSKconformance, &lbpSetConfirm);
                }

                /* G3 network started in ADP */
                app_eap_serverData.state = APP_EAP_SERVER_NETWORK_STARTED;
            }
            else if (adpStatus == ADP_STATUS_ERROR)
            {
                SYS_DEBUG_MESSAGE(SYS_ERROR_ERROR, "APP_EAP_SERVER: Error in ADP initialization\r\n");
                app_eap_serverData.state = APP_EAP_SERVER_STATE_ERROR;
            }

            break;
        }

        /* G3 network started */
        case APP_EAP_SERVER_NETWORK_STARTED:
        {
            /* LBP Coordinator tasks */
            LBP_UpdateLbpSlots();
            break;
        }

        /* Error state */
        case APP_EAP_SERVER_STATE_ERROR:
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

void APP_EAP_SERVER_LaunchRekeying(void)
{
    if (app_eap_serverData.numDevicesJoined > 0)
    {
        /* Start the re-keying process */
        app_eap_serverData.rekey = true;
        app_eap_serverData.rekeyPhaseDistribute = true;
        LBP_SetRekeyPhase(true);

        /* Send the first re-keying process */
        for (uint16_t i = 0; i < app_eap_serverData.numShortAddrAssigned; i++)
        {
            if (memcmp(app_eap_serverExtAddrList[i].value,
                    app_eap_serverNullAddress.value, ADP_ADDRESS_64BITS) != 0)
            {
                /* First not null address */
                LBP_Rekey(APP_EAP_SERVER_INITIAL_SHORT_ADDRESS + i, &app_eap_serverExtAddrList[i], true);
                app_eap_serverData.rekeyIndex = i + 1;
                break;
            }
        }
    }
}

void APP_EAP_SERVER_KickDevice(uint16_t shortAddress)
{
    if ((app_eap_serverData.numDevicesJoined > 0) &&
            (shortAddress >= APP_EAP_SERVER_INITIAL_SHORT_ADDRESS) &&
            (shortAddress < (APP_EAP_SERVER_INITIAL_SHORT_ADDRESS + APP_EAP_SERVER_MAX_DEVICES)))
    {
        uint16_t index = shortAddress - APP_EAP_SERVER_INITIAL_SHORT_ADDRESS;
        ADP_EXTENDED_ADDRESS* pExtAddr = &app_eap_serverExtAddrList[index];

        /* Check if device is in the list */
        if (memcmp(pExtAddr->value, app_eap_serverNullAddress.value, ADP_ADDRESS_64BITS) != 0)
        {
            if (LBP_KickDevice(shortAddress, pExtAddr) == true)
            {
                /* Remove the device from the joined devices list */
                memset(pExtAddr->value, 0, ADP_ADDRESS_64BITS);
            }
        }
    }
}

void APP_EAP_SERVER_AddToBlacklist(uint8_t* extendedAddress)
{
    if (app_eap_serverData.blacklistSize < APP_EAP_SERVER_MAX_DEVICES)
    {
        memcpy(app_eap_serverBlacklist[app_eap_serverData.blacklistSize++].value,
                extendedAddress, ADP_ADDRESS_64BITS);
    }
}

uint16_t APP_EAP_SERVER_GetNumDevicesJoined(void)
{
    return app_eap_serverData.numDevicesJoined;
}

uint16_t APP_EAP_SERVER_GetDeviceAddress(uint16_t index, uint8_t* pEUI64)
{
    uint16_t currentIndex = 0;

    for (uint16_t i = app_eap_serverData.rekeyIndex; i < app_eap_serverData.numShortAddrAssigned; i++)
    {
        /* Check not null address */
        if (memcmp(app_eap_serverExtAddrList[i].value,
                app_eap_serverNullAddress.value, ADP_ADDRESS_64BITS) != 0)
        {
            if (currentIndex == index)
            {
                memcpy(pEUI64, app_eap_serverExtAddrList[i].value, ADP_ADDRESS_64BITS);
                return APP_EAP_SERVER_INITIAL_SHORT_ADDRESS + i;
            }
            else
            {
                currentIndex++;
            }
        }
    }

    return 0xFFFF;
}

void APP_EAP_SERVER_SetConformanceConfig ( void )
{
    app_eap_serverData.conformanceTest = true;

    if ((app_eap_serverData.state > APP_EAP_SERVER_STATE_WAIT_NETWORK_STARTED) &&
            (app_eap_serverData.state != APP_EAP_SERVER_STATE_ERROR))
    {
        LBP_SET_PARAM_CONFIRM lbpSetConfirm;

        /* Network has been started: set PSK key for Conformance */
        LBP_SetParamCoord(LBP_IB_PSK, 0, 16, app_eap_serverPSKconformance, &lbpSetConfirm);
    }
}

/*******************************************************************************
 End of File
 */
