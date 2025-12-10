/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app.c

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
#include "definitions.h"
#include "user.h"

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
    This structure should be initialized by the APP_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_DATA appData;

// *****************************************************************************
// *****************************************************************************
// Section: Function Declaration
// *****************************************************************************
// *****************************************************************************

static void _APP_WriteNonVolatileDataGPBR(void);

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

static void _APP_SetEUI64NonVolatileDataCallback (
    ADP_EXTENDED_ADDRESS* pEUI64,
    ADP_NON_VOLATILE_DATA_IND_PARAMS* pNonVolatileData
)
{
    /* Set Extended Address (EUI64) */
    *pEUI64 = appData.eui64;

    if (appData.validNonVolatileData == true)
    {
        /* Set non-volatile data */
        *pNonVolatileData = appData.nonVolatileData;
    }
    else
    {
        /* Set non-volatile data to 0's */
        memset(pNonVolatileData, 0, sizeof(ADP_NON_VOLATILE_DATA_IND_PARAMS));
    }
}

static void _APP_UpdateNonVolatileDataCallback(ADP_NON_VOLATILE_DATA_IND_PARAMS* pNonVolatileData)
{
    /* Store non-volatile data to write it in user signature at power-down */
    appData.nonVolatileData = *pNonVolatileData;
    appData.validNonVolatileData = true;

    /* Write non-volatile data data in GPBR in order to read it at non-power-up
     * reset. */
    _APP_WriteNonVolatileDataGPBR();
}

static void _SUPC_PowerDownCallback(uint32_t supc_status, uintptr_t context)
{
    if ((supc_status & SUPC_ISR_VDD3V3SMEV_Msk) != 0)
    {
        /* VDD3V3 supply monitor event */
        if (appData.validNonVolatileData == true)
        {
            uint32_t userSignatureData[APP_NON_VOLATILE_DATA_USER_SIGNATURE_SIZE];

            /* Write non-volatile data in User Signature. Put key in first 32
             * bits. */
            userSignatureData[0] = APP_NON_VOLATILE_DATA_KEY_USER_SIGNATURE;
            memcpy(&userSignatureData[1], (const void*) context,
                    sizeof(ADP_NON_VOLATILE_DATA_IND_PARAMS));
            SEFC0_UserSignatureWrite(userSignatureData,
                APP_NON_VOLATILE_DATA_USER_SIGNATURE_SIZE, BLOCK_0, PAGE_0);
        }
    }
}

static void _APP_TimeExpiredSetFlag(uintptr_t context)
{
    /* Context holds the flag's address */
    *((bool *) context) = true;
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

static void _APP_WriteNonVolatileDataGPBR(void)
{
    uint32_t gpbr0Data;

    /* Write non-volatile data data in GPBR in order to read it at non-power-up
     * reset.
     * GPBR0: Discover Sequence Number + Broadcast Sequence Number + one-byte
     * key to detect valid data.
     * GPBR1: MAC PLC Frame Counter.
     * GPBR2: MAC RF Frame Counter. */
    gpbr0Data = appData.nonVolatileData.discoverSeqNumber;
    gpbr0Data += (uint32_t) appData.nonVolatileData.broadcastSeqNumber << 16;
    gpbr0Data += APP_NON_VOLATILE_DATA_KEY_GPBR << 24;
    SUPC_GPBRWrite(GPBR_REGS_0, gpbr0Data);
    SUPC_GPBRWrite(GPBR_REGS_1, appData.nonVolatileData.frameCounter);
    SUPC_GPBRWrite(GPBR_REGS_2, appData.nonVolatileData.frameCounterRF);
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize ( void )
{
    ADP_SERIAL_NOTIFICATIONS callbacks;
    uint8_t uniqueId[16];

    /* Disable User Signature write protection */
    SEFC0_WriteProtectionSet(0);

    /* Enable write and read User Signature (block 0 / area 1) rights */
    SEFC0_UserSignatureRightsSet(SEFC_EEFC_USR_RDENUSB1_Msk | SEFC_EEFC_USR_WRENUSB1_Msk);

    if (RSTC_ResetCauseGet() == RSTC_RESET_CAUSE_GENERAL)
    {
        uint32_t userSignatureData[APP_NON_VOLATILE_DATA_USER_SIGNATURE_SIZE];

        /* Power-on reset. Read non-volatile data from User Signature. */
        SEFC0_UserSignatureRead(userSignatureData,
                APP_NON_VOLATILE_DATA_USER_SIGNATURE_SIZE, BLOCK_0, PAGE_0);

        /* Check key in first 32 bits */
        if (userSignatureData[0] == APP_NON_VOLATILE_DATA_KEY_USER_SIGNATURE)
        {
            /* Valid key read from User Signature */
            appData.validNonVolatileData = true;
            memcpy(&appData.nonVolatileData, &userSignatureData[1],
                    sizeof(ADP_NON_VOLATILE_DATA_IND_PARAMS));

            /* Write non-volatile data in GPBR */
            _APP_WriteNonVolatileDataGPBR();
        }
        else
        {
            /* Invalid key read from User Signature */
            appData.validNonVolatileData = false;
        }

        /* Erase User Signature to write it faster from SUPC power-down
         * callback */
        SEFC0_UserSignatureErase(BLOCK_0);
    }
    else
    {
        uint32_t gpbr0Data;

        /* Not power-on reset. Read non-volatile data from GPBR.
         * GPBR0: Discover Sequence Number + Broadcast Sequence Number +
         * one-byte key to detect valid data.
         * GPBR1: MAC PLC Frame Counter.
         * GPBR2: MAC RF Frame Counter. */
        gpbr0Data = SUPC_GPBRRead(GPBR_REGS_0);
        if ((gpbr0Data >> 24) == APP_NON_VOLATILE_DATA_KEY_GPBR)
        {
            /* Valid key read from GPBR0 */
            appData.nonVolatileData.discoverSeqNumber = (uint16_t) gpbr0Data;
            appData.nonVolatileData.broadcastSeqNumber = (uint8_t) (gpbr0Data >> 16);
            appData.nonVolatileData.frameCounter = SUPC_GPBRRead(GPBR_REGS_1);
            appData.nonVolatileData.frameCounterRF = SUPC_GPBRRead(GPBR_REGS_2);
            appData.validNonVolatileData = true;
        }
        else
        {
            /* Invalid key read from GPBR0 */
            appData.validNonVolatileData = false;
        }
    }

    if (appData.validNonVolatileData == true)
    {
        /* Check MAC Frame Counters */
        if (appData.nonVolatileData.frameCounter == 0xFFFFFFFF)
        {
            /* Invalid MAC PLC Frame Counter */
            appData.nonVolatileData.frameCounter = 0;
            appData.validNonVolatileData = false;
        }

        if (appData.nonVolatileData.frameCounterRF == 0xFFFFFFFF)
        {
            /* Invalid MAC RF Frame Counter */
            appData.nonVolatileData.frameCounterRF = 0;
            appData.validNonVolatileData = false;
        }
    }

    /* Register SUPC power-down callback to write non-volatile data in User
     * Signature */
    SUPC_CallbackRegister(_SUPC_PowerDownCallback, (uintptr_t) &appData.nonVolatileData);

    /* Read UniqueID to set extended address (EUI64) */
    SEFC0_UniqueIdentifierRead((uint32_t*) uniqueId, 4);
    appData.eui64.value[7] = uniqueId[4];
    appData.eui64.value[6] = uniqueId[5];
    appData.eui64.value[5] = uniqueId[6];
    appData.eui64.value[4] = uniqueId[7];
    appData.eui64.value[3] = (uniqueId[8] << 4) | (uniqueId[9] & 0x0F);
    appData.eui64.value[2] = (uniqueId[10] << 4) | (uniqueId[11] & 0x0F);
    appData.eui64.value[1] = (uniqueId[12] << 4) | (uniqueId[13] & 0x0F);
    appData.eui64.value[0] = (uniqueId[14] << 4) | (uniqueId[15] & 0x0F);

    /* Place the application state machine in its initial state */
    appData.state = APP_STATE_INIT;

    /* Initialize application variables */
    appData.timerLedExpired = false;

    /* Set ADP Serial call-backs */
    callbacks.setEUI64NonVolatileData = _APP_SetEUI64NonVolatileDataCallback;
    callbacks.nonVolatileDataIndication = _APP_UpdateNonVolatileDataCallback;
    ADP_SERIAL_SetNotifications(&callbacks);
}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{
    /* Refresh Watchdog */
    CLEAR_WATCHDOG();

    /* Signaling: LED Toggle */
    if (appData.timerLedExpired == true)
    {
        appData.timerLedExpired = false;
        USER_BLINK_LED_Toggle();
    }

    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
            /* Register timer callback to blink LED */
            SYS_TIME_HANDLE timeHandle = SYS_TIME_CallbackRegisterMS(
                    _APP_TimeExpiredSetFlag, (uintptr_t) &appData.timerLedExpired,
                    APP_LED_BLINK_PERIOD_MS, SYS_TIME_PERIODIC);

            if (timeHandle != SYS_TIME_HANDLE_INVALID)
            {
                appData.state = APP_STATE_SERVICE_TASKS;
                SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, APP_STRING_HEADER);
            }
            break;
        }

        case APP_STATE_SERVICE_TASKS:
        {
            /* Nothing to do */
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
