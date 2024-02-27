/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app_nvm.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_NVM_Initialize" and "APP_NVM_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_NVM_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*
Copyright (C) 2022, Microchip Technology Inc., and its subsidiaries. All rights reserved.

The software and documentation is provided by microchip and its contributors
"as is" and any express, implied or statutory warranties, including, but not
limited to, the implied warranties of merchantability, fitness for a particular
purpose and non-infringement of third party intellectual property rights are
disclaimed to the fullest extent permitted by law. In no event shall microchip
or its contributors be liable for any direct, indirect, incidental, special,
exemplary, or consequential damages (including, but not limited to, procurement
of substitute goods or services; loss of use, data, or profits; or business
interruption) however caused and on any theory of liability, whether in contract,
strict liability, or tort (including negligence or otherwise) arising in any way
out of the use of the software and documentation, even if advised of the
possibility of such damage.

Except as expressly permitted hereunder and subject to the applicable license terms
for any third-party software incorporated in the software and any applicable open
source software license terms, no license or other rights, whether express or
implied, are granted under any patent or other intellectual property rights of
Microchip or any third party.
*/
// DOM-IGNORE-END

#ifndef APP_NVM_H
#define APP_NVM_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "configuration.h"
#include "definitions.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************

/* Will Erase, Write and Read 1KB of Data */
#define NVM_BUFFER_SIZE                 (1024U)

#define NVM_GEOMETRY_TABLE_READ_ENTRY   (0)
#define NVM_GEOMETRY_TABLE_WRITE_ENTRY  (1)
#define NVM_GEOMETRY_TABLE_ERASE_ENTRY  (2)

#define BLOCK_START                     0x0

// *****************************************************************************
/* Application states

  Summary:
    Application states enumeration

  Description:
    This enumeration defines the valid application states.  These states
    determine the behavior of the application at various times.
*/

typedef enum
{
    /* Open the flash driver */
    APP_NVM_STATE_OPEN_DRIVER,

    /* Get the geometry details */
    APP_NVM_STATE_GEOMETRY_GET,

    /* Write to Memory */
    APP_NVM_STATE_WRITE_MEMORY,

    /* Read From Memory */
    APP_NVM_STATE_READ_MEMORY,

    /* Erase Flash */
    APP_NVM_STATE_ERASE_FLASH,

    /* Wait for transfer to complete */
    APP_NVM_STATE_XFER_WAIT,

    /* Transfer success */
    APP_NVM_STATE_SUCCESS,

    /* Wait for commands */
    APP_NVM_STATE_CMD_WAIT,

    /* An app error has occurred */
    APP_NVM_STATE_ERROR

} APP_NVM_STATES;

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
    /* Application's current state */
    APP_NVM_STATES state;

    /* Driver Handle */
    DRV_HANDLE memoryHandle;

    /* Application transfer status */
    volatile bool xfer_done;

    /* Application transfer status */
    bool erase_done;

    /* Erase/Write/Read Command Handles */
    DRV_MEMORY_COMMAND_HANDLE eraseHandle;
    DRV_MEMORY_COMMAND_HANDLE writeHandle;
    DRV_MEMORY_COMMAND_HANDLE readHandle;

    /* Number of Read blocks */
    uint32_t numReadBlocks;

    /* Number of Write blocks */
    uint32_t numWriteBlocks;

    /* Number of Erase blocks */
    uint32_t numEraseBlocks;

    /* Pointer to store Data */
    uint8_t* pData;

    /* Pointer to NVM Data buffer */
    uint8_t* pNVMData;

    /* Length of data to store */
    size_t dataLength;

} APP_NVM_DATA;

extern APP_NVM_DATA appNvm;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Routines
// *****************************************************************************
// *****************************************************************************
/* These routines are called by drivers when certain events occur.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_NVM_Initialize ( void )

  Summary:
     MPLAB Harmony application initialization routine.

  Description:
    This function initializes the Harmony application.  It places the
    application in its initial state and prepares it to run so that its
    APP_NVM_Tasks function can be called.

  Precondition:
    All other system initialization routines should be called before calling
    this routine (in "SYS_Initialize").

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_NVM_Initialize();
    </code>

  Remarks:
    This routine must be called from the SYS_Initialize function.
*/

void APP_NVM_Initialize ( void );


/*******************************************************************************
  Function:
    void APP_NVM_Tasks ( void )

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
    APP_NVM_Tasks();
    </code>

  Remarks:
    This routine must be called from SYS_Tasks() routine.
 */

void APP_NVM_Tasks( void );


#endif /* APP_NVM_H */

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END
