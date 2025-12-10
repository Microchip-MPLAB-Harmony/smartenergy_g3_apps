/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_storage_pic32cxmt.c

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
#include "app_storage_pic32cxmt.h"
#include "peripheral/rstc/plib_rstc.h"
#include "peripheral/sefc/plib_sefc0.h"
#include "peripheral/supc/plib_supc.h"
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
    This structure should be initialized by the APP_STORAGE_PIC32CXMT_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_STORAGE_PIC32CXMT_DATA app_storage_pic32cxmtData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

static void _SUPC_PowerDownCallback(uint32_t supc_status, uintptr_t context)
{
    if ((supc_status & SUPC_ISR_VDD3V3SMEV_Msk) != 0)
    {
        /* VDD3V3 supply monitor event */
        if (app_storage_pic32cxmtData.validNonVolatileData == true)
        {
            uint32_t userSignatureData[APP_STORAGE_NON_VOLATILE_DATA_USER_SIGNATURE_SIZE];

            /* Write non-volatile data in User Signature. Put key in first 32
             * bits. */
            userSignatureData[0] = APP_STORAGE_NON_VOLATILE_DATA_KEY_USER_SIGNATURE;
            memcpy(&userSignatureData[1], (void*) context, sizeof(ADP_NON_VOLATILE_DATA_IND_PARAMS));
            SEFC0_UserSignatureWrite(userSignatureData,
                APP_STORAGE_NON_VOLATILE_DATA_USER_SIGNATURE_SIZE, BLOCK_0, PAGE_0);
        }
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

static void _APP_STORAGE_WriteNonVolatileDataGPBR(void)
{
    uint32_t gpbr0Data;

    /* Write non-volatile data data in GPBR in order to read it at non-power-up
     * reset.
     * GPBR0: Discover Sequence Number + Broadcast Sequence Number + one-byte
     * key to detect valid data.
     * GPBR1: MAC PLC Frame Counter.
     * GPBR2: MAC RF Frame Counter. */
    gpbr0Data = app_storage_pic32cxmtData.nonVolatileData.discoverSeqNumber;
    gpbr0Data += (uint32_t) app_storage_pic32cxmtData.nonVolatileData.broadcastSeqNumber << 16;
    gpbr0Data += APP_STORAGE_NON_VOLATILE_DATA_KEY_GPBR << 24;
    SUPC_GPBRWrite(GPBR_REGS_0, gpbr0Data);
    SUPC_GPBRWrite(GPBR_REGS_1, app_storage_pic32cxmtData.nonVolatileData.frameCounter);
    SUPC_GPBRWrite(GPBR_REGS_2, app_storage_pic32cxmtData.nonVolatileData.frameCounterRF);
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_STORAGE_PIC32CXMT_Initialize ( void )

  Remarks:
    See prototype in app_storage_pic32cxmt.h.
 */

void APP_STORAGE_PIC32CXMT_Initialize ( void )
{
    /* Disable User Signature write protection */
    SEFC0_WriteProtectionSet(0);

    /* Enable write and read User Signature (block 0 / area 1) rights */
    SEFC0_UserSignatureRightsSet(SEFC_EEFC_USR_RDENUSB1_Msk | SEFC_EEFC_USR_WRENUSB1_Msk);

    if (RSTC_ResetCauseGet() == RSTC_RESET_CAUSE_GENERAL)
    {
        uint32_t userSignatureData[APP_STORAGE_NON_VOLATILE_DATA_USER_SIGNATURE_SIZE];

        /* Power-on reset. Read non-volatile data from User Signature. */
        SEFC0_UserSignatureRead(userSignatureData,
                APP_STORAGE_NON_VOLATILE_DATA_USER_SIGNATURE_SIZE, BLOCK_0, PAGE_0);

        /* Check key in first 32 bits */
        if (userSignatureData[0] == APP_STORAGE_NON_VOLATILE_DATA_KEY_USER_SIGNATURE)
        {
            /* Valid key read from User Signature */
            app_storage_pic32cxmtData.validNonVolatileData = true;
            memcpy(&app_storage_pic32cxmtData.nonVolatileData, &userSignatureData[1],
                    sizeof(ADP_NON_VOLATILE_DATA_IND_PARAMS));

            /* Write non-volatile data in GPBR */
            _APP_STORAGE_WriteNonVolatileDataGPBR();
        }
        else
        {
            /* Invalid key read from User Signature */
            app_storage_pic32cxmtData.validNonVolatileData = false;
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
        if ((gpbr0Data >> 24) == APP_STORAGE_NON_VOLATILE_DATA_KEY_GPBR)
        {
            /* Valid key read from GPBR0 */
            app_storage_pic32cxmtData.nonVolatileData.discoverSeqNumber = (uint16_t) gpbr0Data;
            app_storage_pic32cxmtData.nonVolatileData.broadcastSeqNumber = (uint8_t) (gpbr0Data >> 16);
            app_storage_pic32cxmtData.nonVolatileData.frameCounter = SUPC_GPBRRead(GPBR_REGS_1);
            app_storage_pic32cxmtData.nonVolatileData.frameCounterRF = SUPC_GPBRRead(GPBR_REGS_2);
            app_storage_pic32cxmtData.validNonVolatileData = true;
        }
        else
        {
            /* Invalid key read from GPBR0 */
            app_storage_pic32cxmtData.validNonVolatileData = false;
        }
    }

    if (app_storage_pic32cxmtData.validNonVolatileData == true)
    {
        /* Check MAC Frame Counters */
        if (app_storage_pic32cxmtData.nonVolatileData.frameCounter == 0xFFFFFFFF)
        {
            /* Invalid MAC PLC Frame Counter */
            app_storage_pic32cxmtData.nonVolatileData.frameCounter = 0;
            app_storage_pic32cxmtData.validNonVolatileData = false;
        }

        if (app_storage_pic32cxmtData.nonVolatileData.frameCounterRF == 0xFFFFFFFF)
        {
            /* Invalid MAC RF Frame Counter */
            app_storage_pic32cxmtData.nonVolatileData.frameCounterRF = 0;
            app_storage_pic32cxmtData.validNonVolatileData = false;
        }
    }

    /* Register SUPC power-down callback to write non-volatile data in User
     * Signature */
    SUPC_CallbackRegister(_SUPC_PowerDownCallback, (uintptr_t) &app_storage_pic32cxmtData.nonVolatileData);

    if (RSTC_ResetCauseGet() == RSTC_RESET_CAUSE_GENERAL)
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, "APP_STORAGE: Power-on reset\r\n");
    }
    else
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, "APP_STORAGE: Not power-on reset\r\n");
    }

    /* Create semaphore. It is used to suspend task. */
    OSAL_SEM_Create(&app_storage_pic32cxmtData.semaphoreID, OSAL_SEM_TYPE_BINARY, 0, 0);
}

/******************************************************************************
  Function:
    void APP_STORAGE_PIC32CXMT_Tasks ( void )

  Remarks:
    See prototype in app_storage_pic32cxmt.h.
 */

void APP_STORAGE_PIC32CXMT_Tasks ( void )
{
    /* Nothing to do. Suspend task forever (RTOS mode) */
    if (app_storage_pic32cxmtData.semaphoreID != 0)
    {
        OSAL_SEM_Pend(&app_storage_pic32cxmtData.semaphoreID, OSAL_WAIT_FOREVER);
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Interface Functions
// *****************************************************************************
// *****************************************************************************

void APP_STORAGE_GetExtendedAddress(uint8_t* eui64)
{
    uint8_t uniqueId[16];

    /* Read UniqueID to set extended address (EUI64) */
    SEFC0_UniqueIdentifierRead((uint32_t*) uniqueId, 4);
    eui64[7] = uniqueId[4];
    eui64[6] = uniqueId[5];
    eui64[5] = uniqueId[6];
    eui64[4] = uniqueId[7];
    eui64[3] = (uniqueId[8] << 4) | (uniqueId[9] & 0x0F);
    eui64[2] = (uniqueId[10] << 4) | (uniqueId[11] & 0x0F);
    eui64[1] = (uniqueId[12] << 4) | (uniqueId[13] & 0x0F);
    eui64[0] = (uniqueId[14] << 4) | (uniqueId[15] & 0x0F);
}

ADP_NON_VOLATILE_DATA_IND_PARAMS* APP_STORAGE_GetNonVolatileData(void)
{
    if (app_storage_pic32cxmtData.validNonVolatileData == false)
    {
        return NULL;
    }
    else
    {
        return &app_storage_pic32cxmtData.nonVolatileData;
    }
}

void APP_STORAGE_UpdateNonVolatileData(ADP_NON_VOLATILE_DATA_IND_PARAMS* pNonVolatileData)
{
    /* Store non-volatile data to write it in user signature at power-down */
    app_storage_pic32cxmtData.nonVolatileData = *pNonVolatileData;
    app_storage_pic32cxmtData.validNonVolatileData = true;

    /* Write non-volatile data data in GPBR in order to read it at non-power-up
     * reset. */
    _APP_STORAGE_WriteNonVolatileDataGPBR();
}

/*******************************************************************************
 End of File
 */
