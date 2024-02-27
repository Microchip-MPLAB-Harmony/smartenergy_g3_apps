/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_nvm.c

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

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "app_nvm.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

SYS_MEDIA_GEOMETRY *nvmGeometry = NULL;

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_NVM_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

CACHE_ALIGN APP_NVM_DATA appNvm;

/* NVM Data buffer */
static CACHE_ALIGN uint8_t pNvmBuffer[CACHE_ALIGNED_SIZE_GET(NVM_BUFFER_SIZE)];

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

void appNvmTransferHandler
(
    DRV_MEMORY_EVENT event,
    DRV_MEMORY_COMMAND_HANDLE commandHandle,
    uintptr_t context
)
{
    APP_NVM_DATA *app_data = (APP_NVM_DATA *)context;

    switch(event)
    {
        case DRV_MEMORY_EVENT_COMMAND_COMPLETE:
            if (commandHandle == app_data->readHandle)
            {
                appNvm.xfer_done = true;
                /* Update Data Source */
                if (appNvm.pData)
                {
                    memcpy(appNvm.pData, appNvm.pNVMData, appNvm.dataLength);
                }
            }
            else if (commandHandle == app_data->eraseHandle)
            {
                appNvm.erase_done = true;
                appNvm.state = APP_NVM_STATE_WRITE_MEMORY;
            }
            else if (commandHandle == app_data->writeHandle)
            {
                appNvm.xfer_done = true;
                appNvm.erase_done = false;
            }
            break;

        case DRV_MEMORY_EVENT_COMMAND_ERROR:
            appNvm.state = APP_NVM_STATE_ERROR;
            break;

        default:
            break;
    }
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
    void APP_NVM_Initialize ( void )

  Remarks:
    See prototype in app_nvm.h.
 */

void APP_NVM_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appNvm.state = APP_NVM_STATE_OPEN_DRIVER;
    appNvm.erase_done = false;
    appNvm.pNVMData = pNvmBuffer;
}


/******************************************************************************
  Function:
    void APP_NVM_Tasks ( void )

 Description:
    Demonstrates Erase, Write and Read operation of DRV_MEMORY in Buffer Model.
    Each case is a fall through when the request is queued up successfully.

  Remarks:
    See prototype in app_nvm.h.
 */

void APP_NVM_Tasks ( void )
{

    /* Check the application's current state. */
    switch ( appNvm.state )
    {
        case APP_NVM_STATE_OPEN_DRIVER:
        {
            appNvm.memoryHandle = DRV_MEMORY_Open(DRV_MEMORY_INDEX_0, DRV_IO_INTENT_READWRITE);

            if (DRV_HANDLE_INVALID != appNvm.memoryHandle)
            {
                DRV_MEMORY_TransferHandlerSet(appNvm.memoryHandle, appNvmTransferHandler, (uintptr_t)&appNvm);
                appNvm.state = APP_NVM_STATE_GEOMETRY_GET;
            }
            else
            {
                appNvm.state = APP_NVM_STATE_ERROR;
            }

            break;
        }

        case APP_NVM_STATE_GEOMETRY_GET:
        {
            nvmGeometry = DRV_MEMORY_GeometryGet(appNvm.memoryHandle);

            if (nvmGeometry == NULL)
            {
                appNvm.state = APP_NVM_STATE_ERROR;
                break;
            }

            appNvm.numReadBlocks  = (NVM_BUFFER_SIZE / nvmGeometry->geometryTable[NVM_GEOMETRY_TABLE_READ_ENTRY].blockSize);
            if (!appNvm.numReadBlocks)
            {
                appNvm.numReadBlocks = 1;
            }
            appNvm.numWriteBlocks = (NVM_BUFFER_SIZE / nvmGeometry->geometryTable[NVM_GEOMETRY_TABLE_WRITE_ENTRY].blockSize);
            if (!appNvm.numWriteBlocks)
            {
                appNvm.numWriteBlocks = 1;
            }
            appNvm.numEraseBlocks = (NVM_BUFFER_SIZE / nvmGeometry->geometryTable[NVM_GEOMETRY_TABLE_ERASE_ENTRY].blockSize);
            if (!appNvm.numEraseBlocks)
            {
                appNvm.numEraseBlocks = 1;
            }

            appNvm.state = APP_NVM_STATE_CMD_WAIT;

            break;
        }

        case APP_NVM_STATE_WRITE_MEMORY:
        {
            if (appNvm.erase_done) {
                DRV_MEMORY_AsyncWrite(appNvm.memoryHandle, &appNvm.writeHandle, (void *)appNvm.pNVMData, BLOCK_START, appNvm.numWriteBlocks);

                if (DRV_MEMORY_COMMAND_HANDLE_INVALID == appNvm.writeHandle)
                {
                    appNvm.state = APP_NVM_STATE_ERROR;
                }
                else
                {
                    appNvm.state = APP_NVM_STATE_XFER_WAIT;
                }
            }
            else
            {
                /* Update NVM data from Data Source */
                if (appNvm.dataLength > NVM_BUFFER_SIZE)
                {
                    appNvm.dataLength = NVM_BUFFER_SIZE;
                }
                memcpy(appNvm.pNVMData, appNvm.pData, appNvm.dataLength);

                appNvm.state = APP_NVM_STATE_ERASE_FLASH;
            }

            break;
        }

        case APP_NVM_STATE_ERASE_FLASH:
        {
            DRV_MEMORY_AsyncErase(appNvm.memoryHandle, &appNvm.eraseHandle, BLOCK_START, appNvm.numEraseBlocks);

            if (DRV_MEMORY_COMMAND_HANDLE_INVALID == appNvm.eraseHandle)
            {
                appNvm.state = APP_NVM_STATE_ERROR;
            }
            else
            {
                appNvm.state = APP_NVM_STATE_XFER_WAIT;
            }

            break;
        }

        case APP_NVM_STATE_READ_MEMORY:
        {
            DRV_MEMORY_AsyncRead(appNvm.memoryHandle, &appNvm.readHandle, (void *)appNvm.pNVMData, BLOCK_START, appNvm.numReadBlocks);

            if (DRV_MEMORY_COMMAND_HANDLE_INVALID == appNvm.readHandle)
            {
                appNvm.state = APP_NVM_STATE_ERROR;
            }
            else
            {
                appNvm.state = APP_NVM_STATE_XFER_WAIT;
            }

            break;
        }

        case APP_NVM_STATE_XFER_WAIT:
        {
            /* Wait until all the above queued transfer requests are done */
            if(appNvm.xfer_done)
            {
                appNvm.xfer_done = false;
                appNvm.state = APP_NVM_STATE_CMD_WAIT;
            }

            break;
        }

        case APP_NVM_STATE_SUCCESS:
        case APP_NVM_STATE_ERROR:
        case APP_NVM_STATE_CMD_WAIT:
        default:
        {
            break;
        }

    }
}
