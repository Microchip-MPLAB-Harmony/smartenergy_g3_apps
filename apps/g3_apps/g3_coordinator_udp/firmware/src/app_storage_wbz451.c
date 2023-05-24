/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_storage_wbz451.c

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
#include "app_storage_wbz451.h"
//#include "driver/pds/include/pds.h"
//#include "system/debug/sys_debug.h"
//#include "peripheral/trng/plib_trng.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

/* Semaphore identifier. Used to suspend task */
OSAL_SEM_DECLARE(appStorageSemaphoreID);

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_STORAGE_WBZ451_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_STORAGE_WBZ451_DATA app_storage_wbz451Data;

// *****************************************************************************
// *****************************************************************************
// Section: Local Variables
// *****************************************************************************
// *****************************************************************************

PDS_DECLARE_FILE(APP_STORAGE_PDS_ITEM_1, sizeof(APP_STORAGE_WBZ451_DATA), &app_storage_wbz451Data, NO_FILE_MARKS);

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************
void APP_STORAGE_PDSWriteCompleteCallback(PDS_MemId_t itemID)
{
    if (itemID == APP_STORAGE_PDS_ITEM_1)
    {
        app_storage_wbz451Data.writeMemConfirm = true;
    }

}

bool APP_STORAGE_PDSUpdateMemoryCallback(PDS_UpdateMemory_t *memory)
{
    if (memory->id == APP_STORAGE_PDS_ITEM_1)
    {
        app_storage_wbz451Data.updateMemConfirm = true;
    }

    return true;
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
    void APP_STORAGE_WBZ451_Initialize ( void )

  Remarks:
    See prototype in app_storage_wbz451.h.
 */

void APP_STORAGE_WBZ451_Initialize ( void )
{
    bool res;

    PDS_RegisterWriteCompleteCallback(APP_STORAGE_PDSWriteCompleteCallback);
    PDS_RegisterUpdateMemoryCallback(APP_STORAGE_PDSUpdateMemoryCallback);

    if (PDS_IsAbleToRestore(APP_STORAGE_PDS_ITEM_1))
    {
        /* Restore parameters */
        if (PDS_Restore(APP_STORAGE_PDS_ITEM_1))
        {
            if (app_storage_wbz451Data.key != APP_STORAGE_DATA_KEY)
            {
                SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, "APP_STORAGE: Key error\r\n");
            }
            else
            {
                app_storage_wbz451Data.writeMemConfirm = true;
            }
        }
    }
    else
    {
        uint32_t randomValue;

        /* Initialize Parameters */
        app_storage_wbz451Data.nonVolatileData.discoverSeqNumber = 0;
        app_storage_wbz451Data.nonVolatileData.broadcastSeqNumber = 0;
        app_storage_wbz451Data.nonVolatileData.frameCounter = 0;
        app_storage_wbz451Data.nonVolatileData.frameCounterRF = 0;

        randomValue = TRNG_ReadData();
        *(uint32_t *)&app_storage_wbz451Data.eui64[0] = randomValue;
        randomValue = TRNG_ReadData();
        *(uint32_t *)&app_storage_wbz451Data.eui64[4] = randomValue;

        app_storage_wbz451Data.key = APP_STORAGE_DATA_KEY;
        app_storage_wbz451Data.writeMemConfirm = false;
        app_storage_wbz451Data.updateMemConfirm = false;
        /* Store Data in NVM */
         res = PDS_Store(APP_STORAGE_PDS_ITEM_1);
        if (res)
        {
            randomValue = 0;
        }
    }

    /* Create semaphore. It is used to suspend task. */
    OSAL_SEM_Create(&appStorageSemaphoreID, OSAL_SEM_TYPE_BINARY, 0, 0);
}

/******************************************************************************
  Function:
    void APP_STORAGE_WBZ451_Tasks ( void )

  Remarks:
    See prototype in app_storage_wbz451.h.
 */

void APP_STORAGE_WBZ451_Tasks ( void )
{
    /* Nothing to do. Suspend task forever (RTOS mode) */
    if (appStorageSemaphoreID != 0)
    {
        OSAL_SEM_Pend(&appStorageSemaphoreID, OSAL_WAIT_FOREVER);
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Interface Functions
// *****************************************************************************
// *****************************************************************************

void APP_STORAGE_GetExtendedAddress(uint8_t* eui64)
{
    if (app_storage_wbz451Data.key != APP_STORAGE_DATA_KEY)
    {
        memset(eui64, 0xFF, sizeof(app_storage_wbz451Data.eui64));
        SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, "APP_STORAGE: Data is not valid (EUI64)\r\n");
    }
    else
    {
        memcpy(eui64, app_storage_wbz451Data.eui64,
               sizeof(app_storage_wbz451Data.eui64));
    }
}

ADP_NON_VOLATILE_DATA_IND_PARAMS* APP_STORAGE_GetNonVolatileData(void)
{
    if (PDS_IsAbleToRestore(APP_STORAGE_PDS_ITEM_1))
    {
        if (app_storage_wbz451Data.key == APP_STORAGE_DATA_KEY)
        {
            return &app_storage_wbz451Data.nonVolatileData;
        }
        else
        {
            SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, "APP_STORAGE: Data is not valid (GET)\r\n");
        }
    }
    else
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, "APP_STORAGE: Data is not found (GET)\r\n");
    }

    return NULL;
}

void APP_STORAGE_UpdateNonVolatileData(ADP_NON_VOLATILE_DATA_IND_PARAMS* pNonVolatileData)
{
    if (PDS_IsAbleToRestore(APP_STORAGE_PDS_ITEM_1))
    {
        if (app_storage_wbz451Data.key == APP_STORAGE_DATA_KEY)
        {
            app_storage_wbz451Data.nonVolatileData = *pNonVolatileData;
            app_storage_wbz451Data.key = APP_STORAGE_DATA_KEY;

            app_storage_wbz451Data.writeMemConfirm = false;
            PDS_Store(APP_STORAGE_PDS_ITEM_1);
        }
        else
        {
            SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, "APP_STORAGE: Data is not valid (SET)\r\n");
        }
    }
    else
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, "APP_STORAGE: Data is not found (SET)\r\n");
    }
}

/*******************************************************************************
 End of File
 */
