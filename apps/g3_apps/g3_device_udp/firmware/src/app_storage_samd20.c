/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_storage_samd20.c

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

#include "app_storage_samd20.h"
#include "definitions.h"

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
    This structure should be initialized by the APP_STORAGE_SAMD20_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_STORAGE_SAMD20_DATA app_storage_samd20Data;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

static void lWDT_EarlyWarningCallback(uintptr_t context)
{
    (void) NVMCTRL_PageBufferCommit(app_storage_samd20Data.nonVolatileDataAddress);
}

static void lSYSCTRL_BOD33DETCalbback (SYSCTRL_INTERRUPT_MASK interruptMask, uintptr_t context)
{
    (void) NVMCTRL_PageBufferCommit(app_storage_samd20Data.nonVolatileDataAddress);
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_STORAGE_SAMD20_Initialize ( void )

  Remarks:
    See prototype in app_storage_samd20.h.
 */

void APP_STORAGE_SAMD20_Initialize ( void )
{
    /* Set EEPROM emulation address */
    app_storage_samd20Data.nonVolatileDataAddress = NVMCTRL_EEPROM_START_ADDRESS;

    /* Register WDT callback to write non-volatile data EEPROM emulation */
    WDT_CallbackRegister(lWDT_EarlyWarningCallback, 0);
    
    /* Register BOD33 callback to write non-volatile data EEPROM emulation */
    SYSCTRL_CallbackRegister(lSYSCTRL_BOD33DETCalbback, 0);
    
    NVMCTRL_Read((uint32_t *)&app_storage_samd20Data.nonVolatileData, 
                 sizeof(app_storage_samd20Data.nonVolatileData), 
                 app_storage_samd20Data.nonVolatileDataAddress);
    
    if (app_storage_samd20Data.nonVolatileData.key == APP_STORAGE_NON_VOLATILE_DATA_KEY)
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, "APP_STORAGE: Restored Non-Volatile Data\r\n");
        app_storage_samd20Data.validNonVolatileData = true;
    }
    else
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, "APP_STORAGE: Invalid Non-Volatile Data\r\n");
        app_storage_samd20Data.validNonVolatileData = false;
    }

    if (app_storage_samd20Data.validNonVolatileData == true)
    {
        ADP_NON_VOLATILE_DATA_IND_PARAMS *adpData = &app_storage_samd20Data.nonVolatileData.data;
        
        /* Check MAC Frame Counters */
        if (adpData->frameCounter == 0xFFFFFFFF)
        {
            /* Invalid MAC PLC Frame Counter */
            adpData->frameCounter = 0U;
            app_storage_samd20Data.validNonVolatileData = false;
        }

        if (adpData->frameCounterRF == 0xFFFFFFFF)
        {
            /* Invalid MAC RF Frame Counter */
            adpData->frameCounterRF = 0U ;
            app_storage_samd20Data.validNonVolatileData = false;
        }
    }

    /* Create semaphore. It is used to suspend task. */
    OSAL_SEM_Create(&app_storage_samd20Data.semaphoreID, OSAL_SEM_TYPE_BINARY, 0, 0);
    
}


/******************************************************************************
  Function:
    void APP_STORAGE_SAMD20_Tasks ( void )

  Remarks:
    See prototype in app_storage_samd20.h.
 */

void APP_STORAGE_SAMD20_Tasks ( void )
{
    /* Nothing to do. Suspend task forever (RTOS mode) */
    if (app_storage_samd20Data.semaphoreID != 0)
    {
        OSAL_SEM_Pend(&app_storage_samd20Data.semaphoreID, OSAL_WAIT_FOREVER);
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Interface Functions
// *****************************************************************************
// *****************************************************************************

void APP_STORAGE_GetExtendedAddress(uint8_t* eui64)
{
    uint32_t serialNumber[4];
    uint8_t *pSerialNumber = (uint8_t *)&serialNumber[0];
    
    /* Read Serial Number to set extended address (EUI64) */
    serialNumber[0] = *(uint32_t *)0x0080A00C;
    serialNumber[1] = *(uint32_t *)0x0080A040;
    serialNumber[2] = *(uint32_t *)0x0080A044;
    serialNumber[3] = *(uint32_t *)0x0080A048;

    eui64[7] = pSerialNumber[4];
    eui64[6] = pSerialNumber[5];
    eui64[5] = pSerialNumber[6];
    eui64[4] = pSerialNumber[7];
    eui64[3] = (pSerialNumber[8] << 4) | (pSerialNumber[9] & 0x0F);
    eui64[2] = (pSerialNumber[10] << 4) | (pSerialNumber[11] & 0x0F);
    eui64[1] = (pSerialNumber[12] << 4) | (pSerialNumber[13] & 0x0F);
    eui64[0] = (pSerialNumber[14] << 4) | (pSerialNumber[15] & 0x0F);
}

ADP_NON_VOLATILE_DATA_IND_PARAMS* APP_STORAGE_GetNonVolatileData(void)
{
    if (app_storage_samd20Data.validNonVolatileData == false)
    {
        return NULL;
    }
    else
    {
        return &app_storage_samd20Data.nonVolatileData.data;
    }
}

void APP_STORAGE_UpdateNonVolatileData(ADP_NON_VOLATILE_DATA_IND_PARAMS* pNonVolatileData)
{
    uint32_t *pData = (uint32_t *)&app_storage_samd20Data.nonVolatileData.data;
    /* Store non-volatile data to write it in EEPROM emulation at power-down */
    app_storage_samd20Data.nonVolatileData.key = APP_STORAGE_NON_VOLATILE_DATA_KEY;
    app_storage_samd20Data.nonVolatileData.data = *pNonVolatileData;

    /* Write non-volatile data data in EEPROM emulated in order to read it at non-power-up
     * reset. */
    NVMCTRL_PageBufferWrite(pData, app_storage_samd20Data.nonVolatileDataAddress);
    
    if (app_storage_samd20Data.validNonVolatileData == false)
    {
        (void) NVMCTRL_PageBufferCommit(app_storage_samd20Data.nonVolatileDataAddress);
    }
    
    app_storage_samd20Data.validNonVolatileData = true;
}


/*******************************************************************************
 End of File
 */
