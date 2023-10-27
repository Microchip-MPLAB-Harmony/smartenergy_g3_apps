/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_samd20.c

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
    This structure should be initialized by the APP_SAMD20_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_SAMD20_DATA appData;

// *****************************************************************************
// *****************************************************************************
// Section: Function Declaration
// *****************************************************************************
// *****************************************************************************

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
        *pNonVolatileData = appData.nonVolatileData.data;
    }
    else
    {
        /* Set non-volatile data to 0's */
        memset(pNonVolatileData, 0, sizeof(ADP_NON_VOLATILE_DATA_IND_PARAMS));
    }
}

static void _APP_UpdateNonVolatileDataCallback(ADP_NON_VOLATILE_DATA_IND_PARAMS* pNonVolatileData)
{
    uint32_t *pData = (uint32_t *)&appData.nonVolatileData.data;
    
    /* Store non-volatile data to write it in user signature at power-down */
    appData.nonVolatileData.data = *pNonVolatileData;
    appData.nonVolatileData.key = APP_STORAGE_NON_VOLATILE_DATA_KEY;

    /* Write non-volatile data data in EEPROM emulated in order to read it at non-power-up
     * reset. */
    NVMCTRL_PageBufferWrite(pData, appData.nonVolatileDataAddress);
    
    if (appData.validNonVolatileData == false)
    {
        (void) NVMCTRL_PageBufferCommit(appData.nonVolatileDataAddress);
    }
    
    appData.validNonVolatileData = true;
}

static void _APP_TimeExpiredSetFlag(uintptr_t context)
{
    /* Context holds the flag's address */
    *((bool *) context) = true;
}

//static void _APP_WDT_EarlyWarningCallback(uintptr_t context)
//{
//    (void) NVMCTRL_PageBufferCommit(appData.nonVolatileDataAddress);
//}

static void _APP_SYSCTRL_BOD33DETCalbback (SYSCTRL_INTERRUPT_MASK interruptMask, uintptr_t context)
{
    (void) NVMCTRL_PageBufferCommit(appData.nonVolatileDataAddress);
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_SAMD20_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_SAMD20_Initialize ( void )
{
    ADP_SERIAL_NOTIFICATIONS callbacks;
    uint32_t serialNumber[4];
    uint8_t *pSerialNumber = (uint8_t *)&serialNumber[0];

    /* Set EEPROM emulation address */
    appData.nonVolatileDataAddress = NVMCTRL_EEPROM_START_ADDRESS;

    /* Register WDT callback to write non-volatile data EEPROM emulation */
//    WDT_CallbackRegister(_APP_WDT_EarlyWarningCallback, 0);
    
    /* Register BOD33 callback to write non-volatile data EEPROM emulation */
    SYSCTRL_CallbackRegister(_APP_SYSCTRL_BOD33DETCalbback, 0);
    
    NVMCTRL_Read((uint32_t *)&appData.nonVolatileData, 
                 sizeof(appData.nonVolatileData), 
                 appData.nonVolatileDataAddress);
    
    if (appData.nonVolatileData.key == APP_STORAGE_NON_VOLATILE_DATA_KEY)
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, "APP_STORAGE: Restored Non-Volatile Data\r\n");
        appData.validNonVolatileData = true;
    }
    else
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, "APP_STORAGE: Invalid Non-Volatile Data\r\n");
        appData.validNonVolatileData = false;
    }

    if (appData.validNonVolatileData == true)
    {
        ADP_NON_VOLATILE_DATA_IND_PARAMS *adpData = &appData.nonVolatileData.data;
        
        /* Check MAC Frame Counters */
        if (adpData->frameCounter == 0xFFFFFFFF)
        {
            /* Invalid MAC PLC Frame Counter */
            adpData->frameCounter = 0U;
            appData.validNonVolatileData = false;
        }

        if (adpData->frameCounterRF == 0xFFFFFFFF)
        {
            /* Invalid MAC RF Frame Counter */
            adpData->frameCounterRF = 0U ;
            appData.validNonVolatileData = false;
        }
    }

    /* Read Serial Number to set extended address (EUI64) */
    serialNumber[0] = *(uint32_t *)0x0080A00C;
    serialNumber[1] = *(uint32_t *)0x0080A040;
    serialNumber[2] = *(uint32_t *)0x0080A044;
    serialNumber[3] = *(uint32_t *)0x0080A048;
    
    appData.eui64.value[7] = pSerialNumber[4];
    appData.eui64.value[6] = pSerialNumber[5];
    appData.eui64.value[5] = pSerialNumber[6];
    appData.eui64.value[4] = pSerialNumber[7];
    appData.eui64.value[3] = (pSerialNumber[8] << 4) | (pSerialNumber[9] & 0x0F);
    appData.eui64.value[2] = (pSerialNumber[10] << 4) | (pSerialNumber[11] & 0x0F);
    appData.eui64.value[1] = (pSerialNumber[12] << 4) | (pSerialNumber[13] & 0x0F);
    appData.eui64.value[0] = (pSerialNumber[14] << 4) | (pSerialNumber[15] & 0x0F);

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

void APP_SAMD20_Tasks ( void )
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
