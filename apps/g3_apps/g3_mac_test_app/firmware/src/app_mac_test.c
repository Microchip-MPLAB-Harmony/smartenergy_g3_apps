/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_mac_test.c

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
#include "app_mac_test.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

#define APP_SENDER

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_MAC_TEST_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_MAC_TEST_DATA app_mac_testData;

/* MAC parameter values */
#ifdef APP_SENDER
static uint8_t extAddress[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
static uint16_t shortAddress = 0xCAFE;
static uint16_t dstAddress = 0xBEEF;
#else
static uint8_t extAddress[8] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x00};
static uint16_t shortAddress = 0xBEEF;
static uint16_t dstAddress = 0xCAFE;
#endif
static uint16_t panID = 0x781D;
static uint8_t gmk[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16};
static uint8_t msdu[50] = {0x41, 0x60, 0x00, 0x00, 0x00, 0x00, 0x09, 0x3A, 0x01, 0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x78, 0x1D, 0x00, 0xFF, 0xFE, 0x00, 0x00, 0x2A, 0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x1D, 
        0x00, 0xFF, 0xFE, 0x00, 0x01, 0x0C, 0x80, 0x00, 0x8D, 0x41, 0x01, 0x02, 0x05, 0x06, 0x00};
static uint16_t msduLength = 50;
static uint8_t msduHandle = 0;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

static void APP_MAC_TEST_DataConfirm(MAC_WRP_DATA_CONFIRM_PARAMS *dcParams)
{
    /* MAC Data Confirm */
    /* Go to next state */
    app_mac_testData.state = APP_MAC_TEST_STATE_IDLE;
}

static void APP_MAC_TEST_DataIndication(MAC_WRP_DATA_INDICATION_PARAMS *diParams)
{
    /* MAC Data Indication */
    /* Go to next state */
    app_mac_testData.state = APP_MAC_TEST_STATE_IDLE;
}

static void APP_MAC_TEST_ResetConfirm(MAC_WRP_RESET_CONFIRM_PARAMS *rcParams)
{
    /* MAC Reset Confirm */
}

static void APP_MAC_TEST_BeaconIndication(MAC_WRP_BEACON_NOTIFY_INDICATION_PARAMS *bcnParams)
{
    /* MAC Beacon Indication */
}

static void APP_MAC_TEST_ScanConfirm(MAC_WRP_SCAN_CONFIRM_PARAMS *scParams)
{
    /* MAC Scan Confirm */
    //MAC_WRP_PIB_VALUE pibValue;
    
    /* Set again PAN ID, scan request sets it to FFFF */
    //pibValue.length = sizeof(panID);
    //memcpy(pibValue.value, &panID, sizeof(panID));
    //MAC_WRP_SetRequestSync(app_mac_testData.macWrpHandle, MAC_WRP_PIB_PAN_ID,
    //        0, (const MAC_WRP_PIB_VALUE *)&pibValue);

    /* Go to next state */
    app_mac_testData.state = APP_MAC_TEST_STATE_DATA_REQUEST;
}

static void APP_MAC_TEST_StartConfirm(MAC_WRP_START_CONFIRM_PARAMS *scParams)
{
    /* MAC Start Confirm */
}

static void APP_MAC_TEST_CommStatusIndication(MAC_WRP_COMM_STATUS_INDICATION_PARAMS *csParams)
{
    /* MAC Comm Status Indication */
}


// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

static bool APP_MAC_TEST_SetParams(void)
{
    MAC_WRP_STATUS status;
    MAC_WRP_PIB_VALUE pibValue;

    /* Set Extended Address */
    pibValue.length = sizeof(extAddress);
    memcpy(pibValue.value, extAddress, sizeof(extAddress));
    status = MAC_WRP_SetRequestSync(app_mac_testData.macWrpHandle, MAC_WRP_PIB_MANUF_EXTENDED_ADDRESS,
            0, (const MAC_WRP_PIB_VALUE *)&pibValue);

    /* Set Short Address */
    pibValue.length = sizeof(shortAddress);
    memcpy(pibValue.value, &shortAddress, sizeof(shortAddress));
    status |= MAC_WRP_SetRequestSync(app_mac_testData.macWrpHandle, MAC_WRP_PIB_SHORT_ADDRESS,
            0, (const MAC_WRP_PIB_VALUE *)&pibValue);

    /* Set PAN ID */
    pibValue.length = sizeof(panID);
    memcpy(pibValue.value, &panID, sizeof(panID));
    status |= MAC_WRP_SetRequestSync(app_mac_testData.macWrpHandle, MAC_WRP_PIB_PAN_ID,
            0, (const MAC_WRP_PIB_VALUE *)&pibValue);

    /* Set GMK */
    pibValue.length = sizeof(gmk);
    memcpy(pibValue.value, gmk, sizeof(gmk));
    status |= MAC_WRP_SetRequestSync(app_mac_testData.macWrpHandle, MAC_WRP_PIB_KEY_TABLE,
            0, (const MAC_WRP_PIB_VALUE *)&pibValue);
    
    if (status == MAC_WRP_STATUS_SUCCESS)
    {
        return true;
    }
    else
    {
        return false;
    }
}

static void APP_MAC_TEST_ScanRequest(void)
{
    MAC_WRP_SCAN_REQUEST_PARAMS params;

    /* Set params and call request */
    params.scanDuration = 15; /* seconds */

    MAC_WRP_ScanRequest(app_mac_testData.macWrpHandle, &params);
}

static void APP_MAC_TEST_DataRequest(void)
{
    MAC_WRP_DATA_REQUEST_PARAMS params;

    /* Set params and call request */
    params.srcAddressMode = MAC_WRP_ADDRESS_MODE_SHORT;
    params.destPanId = panID;
    params.destAddress.addressMode = MAC_WRP_ADDRESS_MODE_SHORT;
    params.destAddress.shortAddress = dstAddress;
    params.msduLength = msduLength;
    params.msdu = &msdu[0];
    params.msduHandle = msduHandle;
    params.txOptions = MAC_WRP_TX_OPTION_ACK;
    params.securityLevel = MAC_WRP_SECURITY_LEVEL_ENC_MIC_32;
    params.keyIndex = 0;
    params.qualityOfService = MAC_WRP_QUALITY_OF_SERVICE_NORMAL_PRIORITY;
    params.mediaType = MAC_WRP_MEDIA_TYPE_REQ_PLC_NO_BACKUP;

    MAC_WRP_DataRequest(app_mac_testData.macWrpHandle, &params);
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_MAC_TEST_Initialize ( void )

  Remarks:
    See prototype in app_mac_test.h.
 */

void APP_MAC_TEST_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    app_mac_testData.state = APP_MAC_TEST_STATE_INIT;
}


/******************************************************************************
  Function:
    void APP_MAC_TEST_Tasks ( void )

  Remarks:
    See prototype in app_mac_test.h.
 */

void APP_MAC_TEST_Tasks(void)
{
    /* Clear watchdog */
    WDT_Clear();
    
    /* Check the application's current state. */
    switch (app_mac_testData.state)
    {
        /* Application's initial state. */
        case APP_MAC_TEST_STATE_INIT:
        {
            bool appInitialized = true;

            if (appInitialized)
            {
                app_mac_testData.state = APP_MAC_TEST_STATE_INITIALIZE_MAC;

                /* Open MAC Wrapper instance */
                app_mac_testData.macWrpHandle = MAC_WRP_Open(G3_MAC_WRP_INDEX_0);

                /* Set MAC Wrapper Init data */
                app_mac_testData.macWrpInit.plcBand = MAC_WRP_BAND_CENELEC_A;
                app_mac_testData.macWrpInit.macWrpHandlers.dataConfirmCallback = APP_MAC_TEST_DataConfirm;
                app_mac_testData.macWrpInit.macWrpHandlers.dataIndicationCallback = APP_MAC_TEST_DataIndication;
                app_mac_testData.macWrpInit.macWrpHandlers.resetConfirmCallback = APP_MAC_TEST_ResetConfirm;
                app_mac_testData.macWrpInit.macWrpHandlers.beaconNotifyIndicationCallback = APP_MAC_TEST_BeaconIndication;
                app_mac_testData.macWrpInit.macWrpHandlers.scanConfirmCallback = APP_MAC_TEST_ScanConfirm;
                app_mac_testData.macWrpInit.macWrpHandlers.startConfirmCallback = APP_MAC_TEST_StartConfirm;
                app_mac_testData.macWrpInit.macWrpHandlers.commStatusIndicationCallback = APP_MAC_TEST_CommStatusIndication;
                app_mac_testData.macWrpInit.macWrpHandlers.snifferIndicationCallback = NULL;
            }
            break;
        }

        case APP_MAC_TEST_STATE_INITIALIZE_MAC:
        {
            /* Initialize MAC layer */
            MAC_WRP_Init(app_mac_testData.macWrpHandle, &app_mac_testData.macWrpInit);
            /* Go to next state */
            app_mac_testData.state = APP_MAC_TEST_STATE_WAIT_MAC_READY;
            break;
        }

        case APP_MAC_TEST_STATE_WAIT_MAC_READY:
        {
            /* Check MAC Status */
            if (MAC_WRP_Status() == SYS_STATUS_READY) {
                /* Go to next state */
                app_mac_testData.state = APP_MAC_TEST_STATE_SET_PARAMS;
            }
            break;
        }

        case APP_MAC_TEST_STATE_SET_PARAMS:
        {
            APP_MAC_TEST_SetParams();
            /* Go to next state */
            #ifdef APP_SENDER
            app_mac_testData.state = APP_MAC_TEST_STATE_SCAN_REQUEST;
            #else
            app_mac_testData.state = APP_MAC_TEST_STATE_WAIT_FOR_DATA_INDICATION;
            #endif
            break;
        }

        case APP_MAC_TEST_STATE_SCAN_REQUEST:
        {
            APP_MAC_TEST_ScanRequest();
            /* Go to next state */
            app_mac_testData.state = APP_MAC_TEST_STATE_WAIT_FOR_SCAN_CONFIRM;
            break;
        }

        case APP_MAC_TEST_STATE_WAIT_FOR_SCAN_CONFIRM:
        {
            /* Do nothing. State will change on confirm function */
            break;
        }

        case APP_MAC_TEST_STATE_DATA_REQUEST:
        {
            APP_MAC_TEST_DataRequest();
            /* Go to next state */
            app_mac_testData.state = APP_MAC_TEST_STATE_WAIT_FOR_DATA_CONFIRM;
            break;
        }

        case APP_MAC_TEST_STATE_WAIT_FOR_DATA_CONFIRM:
        {
            /* Do nothing. State will change on confirm function */
            break;
        }

        case APP_MAC_TEST_STATE_WAIT_FOR_DATA_INDICATION:
        {
            /* Do nothing. State will change on indication function */
            break;
        }

        case APP_MAC_TEST_STATE_IDLE:
        {
            /* Do nothing */
            break;
        }

        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}


/*******************************************************************************
 End of File
 */
