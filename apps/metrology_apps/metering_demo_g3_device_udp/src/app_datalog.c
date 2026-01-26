/*
Copyright (C) 2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_datalog.c

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

#include "app_datalog.h"
#include "definitions.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

#define DATALOG_TEMP_BUFFER_SIZE        4096

#define DATALOG_GEOMETRY_TBL_RD_ENTRY  (0)
#define DATALOG_GEOMETRY_TBL_WR_ENTRY  (1)
#define DATALOG_GEOMETRY_TBL_ER_ENTRY  (2)

#define DATALOG_EVENTS_OFFSET           0x400 // 1024

#define DATALOG_ENERGY_OFFSET           sizeof(APP_ENERGY_ACCUMULATORS) // 16
#define DATALOG_DEMAND_OFFSET           sizeof(APP_ENERGY_MAX_DEMAND)   // 40

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_DATALOG_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_DATALOG_DATA CACHE_ALIGN app_datalogData;

/* Define a queue to signal the Datalog Tasks to store data */
APP_DATALOG_QUEUE appDatalogQueue;

/* Array to check if data is present on NVM */
static uint32_t appDatalogNVMKeyContent[APP_DATALOG_USER_NUM] = {0};

/* Array to set start address per user ID (SST26VF write block size: 0x100) */
/* Address must be a multiple of the write block size */
/* For APP_DATALOG_USER_EVENTS, a full sector must be used */
static uint32_t appDatalogNVMStartAddress[APP_DATALOG_USER_NUM] = {
    0,
    0x200,
    0x300,
    0x400,
    0x500,
    0x600,
    0x800,
    0x1000
};

/* Temporal buffer */
#define DATALOG_BUFFER_SIZE         1024U
static uint8_t appDatalogBuffer[DATALOG_BUFFER_SIZE] = {0};

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************
static void _APP_DATALOG_NVMTransferHandler
(
    DRV_MEMORY_EVENT event,
    uintptr_t context
)
{
    switch(event)
    {
        case DRV_MEMORY_EVENT_COMMAND_COMPLETE:
        {
            /* Wait until the last request is done */
            app_datalogData.xfer_done = true;
            break;
        }

        case DRV_MEMORY_EVENT_COMMAND_ERROR:
        {
            app_datalogData.state = APP_DATALOG_STATE_ERROR;
            break;
        }

        default:
        {
            break;
        }
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

static void _APP_DATALOG_InitDatalogQueue(void)
{
    /* Clear DataLog Queue data */
    memset(&appDatalogQueue, 0, sizeof(appDatalogQueue));

    /* Init Queue pointers */
    appDatalogQueue.dataRd = &appDatalogQueue.data[0];
    appDatalogQueue.dataWr = appDatalogQueue.dataRd;
}

static bool _APP_DATALOG_ReceiveDatalogData(APP_DATALOG_QUEUE_DATA *datalogData)
{
    if (appDatalogQueue.dataSize)
    {
        /* Copy data to the data pointer */
        memcpy(datalogData, appDatalogQueue.dataRd, sizeof(APP_DATALOG_QUEUE_DATA));

        /* Update Queue as a circular buffer */
        appDatalogQueue.dataSize--;
        if (appDatalogQueue.dataRd == &appDatalogQueue.data[APP_DATALOG_QUEUE_DATA_SIZE - 1])
        {
            appDatalogQueue.dataRd = &appDatalogQueue.data[0];
        }
        else
        {
            appDatalogQueue.dataRd++;
        }

        return true;
    }

    return false;
}

static uint16_t _APP_DATALOG_GetDataOffset(APP_DATALOG_QUEUE_DATA *pNewData, uint32_t *nvmOffset, uint32_t *dataOffset)
{
    uint16_t length;

    if (pNewData->userId == APP_DATALOG_USER_EVENTS)
    {
        /* Add 4 bytes due to data key */
        length = DATALOG_EVENTS_OFFSET;

        /* Get the block start offset inside the events region [event type] */
        *nvmOffset = DATALOG_EVENTS_OFFSET * pNewData->eventId;
        *dataOffset = 0;
    }
    else if (pNewData->userId == APP_DATALOG_USER_ENERGY)
    {
        /* Read all 12 months plus 4 bytes due to data key */
        length = (DATALOG_ENERGY_OFFSET * 12) + sizeof(uint32_t);

        /* Get the block start offset inside the energy region [months] */
        *nvmOffset = 0;
        *dataOffset = DATALOG_ENERGY_OFFSET * pNewData->date.month;
    }
    else if (pNewData->userId == APP_DATALOG_USER_DEMAND)
    {
        /* Read all 12 months plus 4 bytes due to data key */
        length = (DATALOG_DEMAND_OFFSET * 12) + sizeof(uint32_t);

        /* Get the block start offset inside the demand region [months] */
        *nvmOffset = 0;
        *dataOffset = DATALOG_DEMAND_OFFSET * pNewData->date.month;
    }
    else
    {
        /* Add 4 bytes due to data key */
        length = pNewData->dataLen + sizeof(uint32_t);
        *nvmOffset = 0;
        *dataOffset = 0;
    }

    return length;
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_DATALOG_Initialize ( void )

  Remarks:
    See prototype in app_datalog.h.
 */

void APP_DATALOG_Initialize ( void )
{
    /* Configure MATRIX to provide access to QSPI in mem mode for full range (2MB) */
    MATRIX1_REGS->MATRIX_PRTSR[0] = 9;

    /* Initialize Data Key */
    char *pBoardName = BOARD_NAME;
    memcpy((uint8_t *)&app_datalogData.key, pBoardName + 6, 4);
    *(char *)&app_datalogData.key = 0x53; // 'S': Standard id

    app_datalogData.state = APP_DATALOG_STATE_INIT;

    /* Initialize DataLog Queue */
    _APP_DATALOG_InitDatalogQueue();
}

/******************************************************************************
  Function:
    void APP_DATALOG_Tasks ( void )

  Remarks:
    See prototype in app_datalog.h.
 */

void APP_DATALOG_Tasks(void)
{
    // Check the application's current state.
    switch (app_datalogData.state)
    {
        case APP_DATALOG_STATE_INIT:
        {
            app_datalogData.nvmHandler = DRV_MEMORY_Open(DRV_MEMORY_INDEX_0, DRV_IO_INTENT_READWRITE);

            if (DRV_HANDLE_INVALID != app_datalogData.nvmHandler)
            {
                SYS_MEDIA_GEOMETRY *geometry;

                DRV_MEMORY_TransferHandlerSet(app_datalogData.nvmHandler,
                                              _APP_DATALOG_NVMTransferHandler,
                                              (uintptr_t)&app_datalogData);

                geometry = DRV_MEMORY_GeometryGet(app_datalogData.nvmHandler);

                if (geometry == NULL)
                {
                    app_datalogData.state = APP_DATALOG_STATE_ERROR;
                }
                else
                {
                    app_datalogData.rdBlockSize = geometry->geometryTable[DATALOG_GEOMETRY_TBL_RD_ENTRY].blockSize;
                    app_datalogData.wrBlockSize = geometry->geometryTable[DATALOG_GEOMETRY_TBL_WR_ENTRY].blockSize;
                    app_datalogData.erBlockSize = geometry->geometryTable[DATALOG_GEOMETRY_TBL_ER_ENTRY].blockSize;

                    app_datalogData.userId = (APP_DATALOG_USER)0U;
                    app_datalogData.state = APP_DATALOG_STATE_CHECK_CONTENT;
                }
            }

            break;
        }

        case APP_DATALOG_STATE_CHECK_CONTENT:
        {
            if (app_datalogData.userId < APP_DATALOG_USER_NUM)
            {
                uint32_t blockStart;
                uint32_t numBlocks;

                numBlocks = sizeof(uint32_t) / app_datalogData.rdBlockSize;
                if ((sizeof(uint32_t) % app_datalogData.rdBlockSize) != 0)
                {
                    numBlocks++;
                }
                blockStart = appDatalogNVMStartAddress[app_datalogData.userId] / app_datalogData.rdBlockSize;

                app_datalogData.xfer_done = false;
                DRV_MEMORY_AsyncRead(app_datalogData.nvmHandler, &app_datalogData.cmdHandler,
                                     (void *)appDatalogBuffer,
                                     blockStart, numBlocks);

                app_datalogData.state = APP_DATALOG_STATE_WAIT_CHECK_CONTENT;
            }
            else
            {
                app_datalogData.state = APP_DATALOG_STATE_READY;
            }

            break;
        }

        case APP_DATALOG_STATE_WAIT_CHECK_CONTENT:
        {
            if (app_datalogData.xfer_done == true)
            {
                uint32_t key = *(uint32_t *)appDatalogBuffer;

                appDatalogNVMKeyContent[app_datalogData.userId] = key;
                app_datalogData.userId++;
                app_datalogData.state = APP_DATALOG_STATE_CHECK_CONTENT;
            }

            break;
        }

        case APP_DATALOG_STATE_READY:
        {
            APP_DATALOG_QUEUE_DATA *pNewData = &app_datalogData.newData.data;

            // Wait messages in queue
            if (_APP_DATALOG_ReceiveDatalogData(pNewData))
            {
                if (pNewData->operation == APP_DATALOG_READ)
                {
                    // Go to Read state
                    app_datalogData.state = APP_DATALOG_STATE_READ;
                }
                else if (pNewData->operation == APP_DATALOG_WRITE)
                {
                    if ((pNewData->userId == APP_DATALOG_USER_ENERGY) ||
                        (pNewData->userId == APP_DATALOG_USER_DEMAND))
                    {
                        // Go to Read to update the temporal buffer
                        app_datalogData.state = APP_DATALOG_STATE_READ;
                    }
                    else
                    {
                        // Go to Write state
                        app_datalogData.state = APP_DATALOG_STATE_WRITE;
                    }
                }
                else if (pNewData->operation == APP_DATALOG_ERASE)
                {
                    // Go to Erase state
                    app_datalogData.state = APP_DATALOG_STATE_ERASE;
                }
            }

            break;
        }

        case APP_DATALOG_STATE_READ:
        {
            APP_DATALOG_QUEUE_DATA *pNewData = &app_datalogData.newData.data;
            uint32_t blockStart = 0;
            uint32_t numBlocks = 0;
            uint32_t nvmOffset = 0;
            uint32_t dataOffset = 0;
            uint16_t length = 0;

            blockStart = appDatalogNVMStartAddress[pNewData->userId] / app_datalogData.rdBlockSize;

            length = _APP_DATALOG_GetDataOffset(pNewData, &nvmOffset, &dataOffset);

            blockStart += nvmOffset / app_datalogData.rdBlockSize;
            numBlocks = length / app_datalogData.rdBlockSize;
            if ((length % app_datalogData.rdBlockSize) != 0)
            {
                numBlocks++;
            }

            app_datalogData.xfer_done = false;
            DRV_MEMORY_AsyncRead(app_datalogData.nvmHandler, &app_datalogData.cmdHandler,
                                 (void *)appDatalogBuffer,
                                 blockStart, numBlocks);

            app_datalogData.state = APP_DATALOG_STATE_WAIT_READ;

            break;
        }

        case APP_DATALOG_STATE_WAIT_READ:
        {
            APP_DATALOG_QUEUE_DATA *pNewData = &app_datalogData.newData.data;

            if (app_datalogData.xfer_done == true)
            {
                if (pNewData->operation == APP_DATALOG_WRITE)
                {
                    // Go to write state. Read operation was needed to update the temporal buffer
                    app_datalogData.state = APP_DATALOG_STATE_WRITE;
                }
                else
                {
                    uint32_t key = *(uint32_t *)appDatalogBuffer;

                    if (key == app_datalogData.key)
                    {
                        uint8_t *pData = appDatalogBuffer;
                        uint32_t nvmOffset = 0;
                        uint32_t dataOffset = 0;

                        pData += sizeof(uint32_t);

                        (void)_APP_DATALOG_GetDataOffset(pNewData, &nvmOffset, &dataOffset);

                        /* Point at the data */
                        pData += dataOffset;

                        memcpy(pNewData->pData, pData, pNewData->dataLen);

                        app_datalogData.result = APP_DATALOG_RESULT_SUCCESS;
                        // Go to report state
                        app_datalogData.state = APP_DATALOG_STATE_REPORT_RESULT;
                    }
                    else
                    {
                        app_datalogData.result = APP_DATALOG_RESULT_ERROR;
                        // Go to report state
                        app_datalogData.state = APP_DATALOG_STATE_REPORT_RESULT;
                    }
                }
            }

            break;
        }

        case APP_DATALOG_STATE_WRITE:
        {
            APP_DATALOG_QUEUE_DATA *pNewData = &app_datalogData.newData.data;
            uint32_t *pData = (uint32_t *)appDatalogBuffer;
            uint32_t blockStart = 0;
            uint32_t numBlocks = 0;
            uint32_t nvmOffset = 0;
            uint32_t dataOffset = 0;
            uint16_t length = 0;

            *pData++ = app_datalogData.key;

            blockStart = appDatalogNVMStartAddress[pNewData->userId] / app_datalogData.wrBlockSize;

            length = _APP_DATALOG_GetDataOffset(pNewData, &nvmOffset, &dataOffset);

            blockStart += nvmOffset / app_datalogData.wrBlockSize;
            numBlocks = length / app_datalogData.wrBlockSize;
            if ((length % app_datalogData.wrBlockSize) != 0)
            {
                numBlocks++;
            }

            /* Update content in the temporal buffer */
            memcpy(((uint8_t *)pData) + dataOffset, pNewData->pData, pNewData->dataLen);

            app_datalogData.xfer_done = false;
            DRV_MEMORY_AsyncEraseWrite(app_datalogData.nvmHandler, &app_datalogData.cmdHandler,
                                       (void *)appDatalogBuffer,
                                       blockStart, numBlocks);

            app_datalogData.state = APP_DATALOG_STATE_WAIT_WRITE;

            break;
        }

        case APP_DATALOG_STATE_WAIT_WRITE:
        {
            if (app_datalogData.xfer_done == true)
            {
                APP_DATALOG_QUEUE_DATA *pNewData = &app_datalogData.newData.data;
                uint32_t key = *(uint32_t *)appDatalogBuffer;

                /* update data key */
                if (pNewData->operation == APP_DATALOG_ERASE)
                {
                    appDatalogNVMKeyContent[app_datalogData.newData.data.userId] = 0;
                }
                else
                {
                    appDatalogNVMKeyContent[app_datalogData.newData.data.userId] = key;
                }

                if (key == app_datalogData.key)
                {
                    app_datalogData.result = APP_DATALOG_RESULT_SUCCESS;
                }
                else
                {
                    app_datalogData.result = APP_DATALOG_RESULT_ERROR;
                }

                // Go to report state
                app_datalogData.state = APP_DATALOG_STATE_REPORT_RESULT;

            }

            break;
        }

        case APP_DATALOG_STATE_ERASE:
        {
            APP_DATALOG_QUEUE_DATA *pNewData;
            uint32_t blockStart;
            uint32_t numBlocks;
            uint32_t nvmOffset = 0;
            uint32_t dataOffset = 0;
            uint16_t length = 0;

            pNewData = &app_datalogData.newData.data;

            if (pNewData->userId == APP_DATALOG_USER_EVENTS)
            {
                blockStart = appDatalogNVMStartAddress[APP_DATALOG_USER_EVENTS] / app_datalogData.erBlockSize;
                numBlocks = pNewData->dataLen / app_datalogData.erBlockSize;
                if ((pNewData->dataLen % app_datalogData.erBlockSize) != 0)
                {
                    numBlocks++;
                }

                app_datalogData.xfer_done = false;
                DRV_MEMORY_AsyncErase(app_datalogData.nvmHandler, &app_datalogData.cmdHandler,
                                      blockStart, numBlocks);
            }
            else
            {
                length = _APP_DATALOG_GetDataOffset(pNewData, &nvmOffset, &dataOffset);

                memset(appDatalogBuffer, 0, length);
                appDatalogNVMKeyContent[app_datalogData.newData.data.userId] = 0;

                numBlocks = length / app_datalogData.wrBlockSize;
                if ((length % app_datalogData.wrBlockSize) != 0)
                {
                    numBlocks++;
                }
                blockStart = appDatalogNVMStartAddress[pNewData->userId] / app_datalogData.wrBlockSize;

                app_datalogData.xfer_done = false;
                DRV_MEMORY_AsyncEraseWrite(app_datalogData.nvmHandler,
                                           &app_datalogData.cmdHandler,
                                           (void *)appDatalogBuffer,
                                           blockStart, numBlocks);

            }

            app_datalogData.state = APP_DATALOG_STATE_WAIT_WRITE;

            break;
        }

        case APP_DATALOG_STATE_REPORT_RESULT:
        {
            // Invoke callback with result from operation
            if (app_datalogData.newData.data.endCallback != NULL)
            {
                app_datalogData.newData.data.endCallback(app_datalogData.result);
            }
            // Go back to Ready state
            app_datalogData.state = APP_DATALOG_STATE_READY;

            break;
        }

        case APP_DATALOG_STATE_ERROR:
        {
            SYS_CONSOLE_PRINT("APP_DATALOG_STATE_ERROR!!!\r\n\r\n");
            break;
        }

        // The default state should never be executed.
        default:
        {
            // TODO: Handle error in application's state machine.
            break;
        }
    }
}

/*******************************************************************************
  Function:
    APP_DATALOG_STATES APP_DATALOG_GetStatus(void)

  Remarks:
    See prototype in app_datalog.h.
 */

APP_DATALOG_STATES APP_DATALOG_GetStatus(void)
{
    return app_datalogData.state;
}

/*******************************************************************************
  Function:
    bool APP_DATALOG_DataIsValid(APP_DATALOG_USER userId)

  Remarks:
    See prototype in app_datalog.h.
 */

bool APP_DATALOG_DataIsValid(APP_DATALOG_USER userId)
{
    // If datalog task not yet ready, return false
    if (app_datalogData.state < APP_DATALOG_STATE_READY)
    {
        return false;
    }

    if (appDatalogNVMKeyContent[userId] == app_datalogData.key)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool APP_DATALOG_SendDatalogData(APP_DATALOG_QUEUE_DATA *datalogData)
{
    if (appDatalogQueue.dataSize < APP_DATALOG_QUEUE_DATA_SIZE)
    {
        /* Copy data to the data queue */
        memcpy(appDatalogQueue.dataWr, datalogData, sizeof(APP_DATALOG_QUEUE_DATA));

        /* Update Queue as a circular buffer */
        appDatalogQueue.dataSize++;
        if (appDatalogQueue.dataWr == &appDatalogQueue.data[APP_DATALOG_QUEUE_DATA_SIZE - 1])
        {
            appDatalogQueue.dataWr = &appDatalogQueue.data[0];
        }
        else
        {
            appDatalogQueue.dataWr++;
        }

        return true;
    }

    return false;
}

/*******************************************************************************
 End of File
 */
