/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app_storage_pic32cxmt.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_STORAGE_PIC32CXMT_Initialize" and "APP_STORAGE_PIC32CXMT_Tasks"
    prototypes) and some of them are only used internally by the application
    (such as the "APP_STORAGE_PIC32CXMT_STATES" definition). Both are defined
    here for convenience.
*******************************************************************************/

#ifndef _APP_STORAGE_PIC32CXMT_H
#define _APP_STORAGE_PIC32CXMT_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "configuration.h"
#include "stack/g3/adaptation/adp.h"
#include "osal/osal.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Macro Definitions
// *****************************************************************************
// *****************************************************************************

/* Key to detect valid non-volatile data in GPBR */
#define APP_STORAGE_NON_VOLATILE_DATA_KEY_GPBR 0xA5

/* Key to detect valid non-volatile data in User Signature */
#define APP_STORAGE_NON_VOLATILE_DATA_KEY_USER_SIGNATURE 0xA55AA55A

/* Size of non-volatile data in User Signature (in 32-bits words) */
#define APP_STORAGE_NON_VOLATILE_DATA_USER_SIGNATURE_SIZE (((sizeof(ADP_NON_VOLATILE_DATA_IND_PARAMS) + 3) / 4) + 1)

// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    Application strings and buffers are be defined outside this structure.
 */

typedef struct
{
    /* Semaphore identifier. Used to suspend task */
    OSAL_SEM_DECLARE(semaphoreID);

    /* Current non-volatile data */
    ADP_NON_VOLATILE_DATA_IND_PARAMS nonVolatileData;

    /* Flag to indicate if non-volatile data is valid */
    bool validNonVolatileData;

} APP_STORAGE_PIC32CXMT_DATA;

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_STORAGE_PIC32CXMT_Initialize ( void )

  Summary:
     MPLAB Harmony application initialization routine.

  Description:
    This function initializes the Harmony application.  It places the
    application in its initial state and prepares it to run so that its
    APP_STORAGE_PIC32CXMT_Tasks function can be called.

  Precondition:
    All other system initialization routines should be called before calling
    this routine (in "SYS_Initialize").

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_STORAGE_PIC32CXMT_Initialize();
    </code>

  Remarks:
    This routine must be called from the SYS_Initialize function.
*/

void APP_STORAGE_PIC32CXMT_Initialize ( void );

/*******************************************************************************
  Function:
    void APP_STORAGE_PIC32CXMT_Tasks ( void )

  Summary:
    MPLAB Harmony Demo application tasks function

  Description:
    This routine is the Harmony Demo application's tasks function.  It
    defines the application's state machine and core logic.

  Precondition:
    The system and application initialization ("SYS_Initialize") should be
    called before calling this.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_STORAGE_PIC32CXMT_Tasks();
    </code>

  Remarks:
    This routine must be called from SYS_Tasks() routine.
 */

void APP_STORAGE_PIC32CXMT_Tasks( void );

// *****************************************************************************
// *****************************************************************************
// Section: Application Interface Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_STORAGE_GetExtendedAddress(uint8_t* eui64)

  Summary:
    Gets extended address.

  Description:
    This function gets the extended address of the device, ensuring it is
    unique for each device.

  Precondition:
    APP_STORAGE_PIC32CXMT_Initialize should be called before calling this
    routine.

  Parameters:
    eui64 - Pointer to store extended address (8 bytes).

  Returns:
    None.

  Example:
    <code>
    uint8_t eui64[8];
    APP_STORAGE_GetExtendedAddress(eui64);
    </code>

  Remarks:
    In this implementation the extended address is derived from UniqueID.
*/

void APP_STORAGE_GetExtendedAddress(uint8_t* eui64);

/*******************************************************************************
  Function:
    ADP_NON_VOLATILE_DATA_IND_PARAMS* APP_STORAGE_GetNonVolatileData(void)

  Summary:
    Gets non-volatile data.

  Description:
    This function gets the G3 non-volatile data, if it is valid.

  Precondition:
    APP_STORAGE_PIC32CXMT_Initialize should be called before calling this
    routine.

  Parameters:
    None.

  Returns:
    Pointer to non-volatile data, or NULL if there is no valid non-volatile
    data.

  Example:
    <code>
    ADP_NON_VOLATILE_DATA_IND_PARAMS* pNonVolatileData;
    pNonVolatileData = APP_STORAGE_GetNonVolatileData();
    if (pNonVolatileData != NULL)
    {

    }
    </code>

  Remarks:
    In this implementation the non-volatile data is read from User Signature
    (power-on reset) or GPBR (not power-on reset).
*/

ADP_NON_VOLATILE_DATA_IND_PARAMS* APP_STORAGE_GetNonVolatileData(void);

/*******************************************************************************
  Function:
    void APP_STORAGE_UpdateNonVolatileData(ADP_NON_VOLATILE_DATA_IND_PARAMS* pNonVolatileData)

  Summary:
    Updates non-volatile data.

  Description:
    This function updates the G3 non-volatile data.

  Precondition:
    APP_STORAGE_PIC32CXMT_Initialize should be called before calling this
    routine.

  Parameters:
    pNonVolatileData - Pointer to new non-volatile data.

  Returns:
    None.

  Example:
    <code>
    static void _ADP_NonVolatileDataIndication(ADP_NON_VOLATILE_DATA_IND_PARAMS* pNonVolatileData)
    {
        APP_STORAGE_UpdateNonVolatileData(pNonVolatileData);
    }
    </code>

  Remarks:
    In this implementation the non-volatile data is stored in GPBR each time it
    is updated and in User Signature at power-down.
*/

void APP_STORAGE_UpdateNonVolatileData(ADP_NON_VOLATILE_DATA_IND_PARAMS* pNonVolatileData);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* _APP_STORAGE_PIC32CXMT_H */

/*******************************************************************************
 End of File
 */

