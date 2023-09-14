/*******************************************************************************
* Copyright (C) 2023 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/

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
#if SYS_FS_AUTOMOUNT_ENABLE
    #define DATALOG_MOUNT_NAME      SYS_FS_MEDIA_IDX0_MOUNT_NAME_VOLUME_IDX0
    #define DATALOG_DEVICE_NAME     SYS_FS_MEDIA_IDX0_DEVICE_NAME_VOLUME_IDX0
#else
    #define DATALOG_MOUNT_NAME      "/mnt/myDrive1"
    #define DATALOG_DEVICE_NAME     "/dev/mtda1"
    #define DATALOG_FS_TYPE         FAT
#endif

#define DATALOG_TASK_DELAY_MS_UNTIL_FILE_SYSTEM_READY   100


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

/* Work buffer used by FAT FS during Format */
uint8_t CACHE_ALIGN work[SYS_FS_FAT_MAX_SS];

/* Define a queue to signal the Datalog Tasks to store data */
APP_DATALOG_QUEUE appDatalogQueue;

/* Buffer to get a string name from User IDs */
static char *userToString[APP_DATALOG_USER_NUM] = {"metrology", "calibration", "tou", "rtc", "console", "events", "energy", "demand"};

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************


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

#if SYS_FS_AUTOMOUNT_ENABLE

static bool APP_DATALOG_TaskDelay(uint32_t ms, SYS_TIME_HANDLE* handle)
{
    // Check if there is the timer has been created and running
    if (*handle == SYS_TIME_HANDLE_INVALID)
    {
        // Create timer
        if (SYS_TIME_DelayMS(ms, handle) != SYS_TIME_SUCCESS)
        {
            return false;
        }
    }
    else
    {
        // Check timer
        if (SYS_TIME_DelayIsComplete(*handle) == true)
        {
            *handle = SYS_TIME_HANDLE_INVALID;
            return true;
        }
    }
    
    return false;
}

static void APP_DATALOG_SysFSEventHandler(SYS_FS_EVENT event, void* eventData, uintptr_t context)
{
    switch(event)
    {
        // If the event is mount then check which media has been mounted
        case SYS_FS_EVENT_MOUNT:
            if(strcmp((const char *)eventData, DATALOG_MOUNT_NAME) == 0)
            {
                app_datalogData.diskMounted = true;
            }
            break;

        case SYS_FS_EVENT_MOUNT_WITH_NO_FILESYSTEM:
        {
            if(strcmp((const char *)eventData, DATALOG_MOUNT_NAME) == 0)
            {
                app_datalogData.diskFormatRequired = true;
            }
            break;
        }

        // If the event is unmount then check which media has been unmount
        case SYS_FS_EVENT_UNMOUNT:
            if(strcmp((const char *)eventData, DATALOG_MOUNT_NAME) == 0)
            {
                app_datalogData.diskMounted = false;
                app_datalogData.diskFormatRequired = false;

                if (app_datalogData.state == APP_DATALOG_STATE_READY)
                {
                    app_datalogData.state = APP_DATALOG_STATE_MOUNT_WAIT;
                }
                else
                {
                    app_datalogData.state = APP_DATALOG_STATE_ERROR;
                }
            }

            break;

        case SYS_FS_EVENT_ERROR:
        default:
            break;
    }
}
#endif

static void APP_DATALOG_GetFileNameByDate(APP_DATALOG_USER userId, APP_DATALOG_DATE *date, char *fileName)
{
    // If date not provided or invalid, buld filename with userId only
    if (date == NULL)
    {
        sprintf(fileName, "%s/%s", userToString[userId], userToString[userId]);
    }
    else if ((date->year == APP_DATALOG_INVALID_YEAR) || (date->month == APP_DATALOG_INVALID_MONTH))
    {
        sprintf(fileName, "%s/%s", userToString[userId], userToString[userId]);
    }
    else
    {
        // Build name using date
        sprintf(fileName, "%s/%02d%02d", userToString[userId], date->year, date->month);
    }
}


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

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
    bool APP_DATALOG_FileExists(APP_DATALOG_USER userId, APP_DATALOG_DATE *date)

  Remarks:
    See prototype in app_datalog.h.
 */

bool APP_DATALOG_FileExists(APP_DATALOG_USER userId, APP_DATALOG_DATE *date)
{
    SYS_FS_HANDLE fileHandle;

    // If Filesystem not yet ready, return false
    if (app_datalogData.state <= APP_DATALOG_STATE_CHECK_DIRECTORIES)
    {
        return false;
    }

    // Get file name from parameters
    APP_DATALOG_GetFileNameByDate(userId, date, app_datalogData.fileName);

    // Check whether file exist trying to open it
    fileHandle = SYS_FS_FileOpen(app_datalogData.fileName, (SYS_FS_FILE_OPEN_READ));

    if (fileHandle != SYS_FS_HANDLE_INVALID)
    {
        // File exists, close it and return true
        SYS_FS_FileClose(fileHandle);
        return true;
    }
    else
    {
        // File does not exist, return false
        return false;
    }
}

/*******************************************************************************
  Function:
    void APP_DATALOG_ClearData(APP_DATALOG_USER userId)

  Remarks:
    See prototype in app_datalog.h.
 */

void APP_DATALOG_ClearData(APP_DATALOG_USER userId)
{
    SYS_FS_HANDLE dirHandle;
    bool eod = false;

    // Open directory
    dirHandle = SYS_FS_DirOpen(userToString[userId]);
    
    // Stat struct has to be initialized to NULL before calling the API
    memset(&app_datalogData.stat, 0, sizeof(SYS_FS_FSTAT));

    if (dirHandle != SYS_FS_HANDLE_INVALID)
    {
        while (!eod)
        {
            // Directory open is successful
            if(SYS_FS_DirRead(dirHandle, &app_datalogData.stat) == SYS_FS_RES_FAILURE)
            {
                // Directory read failed.
                eod = true;
            }
            else
            {
                // Directory read succeeded.
                if (app_datalogData.stat.fname[0] == '\0')
                {
                    // Reached the end of the directory.
                    eod = true;
                }
                else
                {
                    // Remove next found file
                    sprintf(app_datalogData.filePath, "%s/%s", userToString[userId], app_datalogData.stat.fname);
                    SYS_FS_FileDirectoryRemove(app_datalogData.filePath);
                }
            }
        }
    }
}

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
    
    /* Initialize the app state to wait for media attach. */
#if SYS_FS_AUTOMOUNT_ENABLE
    app_datalogData.state = APP_DATALOG_STATE_MOUNT_WAIT;
    app_datalogData.timer = SYS_TIME_HANDLE_INVALID;

    SYS_FS_EventHandlerSet(APP_DATALOG_SysFSEventHandler,(uintptr_t)NULL);
#else
    app_datalogData.state = APP_DATALOG_STATE_MOUNT_DISK;
#endif
    
    /* Init DataLog Queue */
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
#if SYS_FS_AUTOMOUNT_ENABLE
        case APP_DATALOG_STATE_MOUNT_WAIT:
        {
            if (app_datalogData.diskFormatRequired == true)
            {
                // Mount was successful. Format the disk.
                app_datalogData.state = APP_DATALOG_STATE_FORMAT_DISK;
            }
            else if (app_datalogData.diskMounted == true)
            {
                // Set current drive to access files directly without path
                SYS_FS_CurrentDriveSet(DATALOG_MOUNT_NAME);

                // Mount was successful. Check directories.
                app_datalogData.state = APP_DATALOG_STATE_CHECK_DIRECTORIES;
            }

            app_datalogData.nextState = app_datalogData.state;
            app_datalogData.state = APP_DATALOG_STATE_DELAY;
            app_datalogData.delayMs = DATALOG_TASK_DELAY_MS_UNTIL_FILE_SYSTEM_READY;

            break;
        }
#else
        case APP_DATALOG_STATE_MOUNT_DISK:
        {
            /* Mount the disk */
            if(SYS_FS_Mount(DATALOG_DEVICE_NAME, DATALOG_MOUNT_NAME, DATALOG_FS_TYPE, 0, NULL) != 0)
            {
                /* The disk could not be mounted. Try mounting again until
                 * the operation succeeds. */
                app_datalogData.state = APP_DATALOG_STATE_MOUNT_DISK;
            }
            else
            {
                /* Check If Mount was successful with no file system on media */
                if (SYS_FS_Error() == SYS_FS_ERROR_NO_FILESYSTEM)
                {
                    /* Format the disk. */
                    app_datalogData.state = APP_DATALOG_STATE_FORMAT_DISK;
                }
                else
                {
                    app_datalogData.state = APP_DATALOG_STATE_CHECK_DIRECTORIES;
                }
            }

            break;
        }
#endif

        case APP_DATALOG_STATE_FORMAT_DISK:
        {
            app_datalogData.formatOpt.fmt = SYS_FS_FORMAT_FAT;
            app_datalogData.formatOpt.au_size = 0;

            if (SYS_FS_DriveFormat(DATALOG_MOUNT_NAME, &app_datalogData.formatOpt, (void *)work, SYS_FS_FAT_MAX_SS) != SYS_FS_RES_SUCCESS)
            {
                /* Format of the disk failed. */
                app_datalogData.state = APP_DATALOG_STATE_ERROR;
            }
            else
            {
                /* Format succeeded. Check directories. */
                app_datalogData.state = APP_DATALOG_STATE_CHECK_DIRECTORIES;
            }

            break;
        }

        case APP_DATALOG_STATE_CHECK_DIRECTORIES:
        {
            uint8_t idx;
            SYS_FS_HANDLE dirHandle;

            // Check whether directories exist, otherwise create them
            for (idx = 0; idx < APP_DATALOG_USER_NUM; idx++)
            {
                dirHandle = SYS_FS_DirOpen(userToString[idx]);

                if (dirHandle != SYS_FS_HANDLE_INVALID)
                {
                    // Directory exists, close it
                    SYS_FS_DirClose(dirHandle);
                }
                else
                {
                    // Directory does not exist, create it
                    SYS_FS_DirectoryMake(userToString[idx]);
                }
            }
            // Ready to accept accesses
            app_datalogData.state = APP_DATALOG_STATE_READY;

            break;
        }

        case APP_DATALOG_STATE_READY:
        {
            // Wait messages in queue
            if (_APP_DATALOG_ReceiveDatalogData(&app_datalogData.newData.data))
            {
                // Get file name
                if ((app_datalogData.newData.data.date.year == APP_DATALOG_INVALID_YEAR) ||
                    (app_datalogData.newData.data.date.month == APP_DATALOG_INVALID_MONTH))
                {
                    APP_DATALOG_GetFileNameByDate(app_datalogData.newData.data.userId, NULL, app_datalogData.newData.fileName);
                }
                else
                {
                    APP_DATALOG_GetFileNameByDate(app_datalogData.newData.data.userId, &app_datalogData.newData.data.date, app_datalogData.newData.fileName);
                }

                // Check Read/Write operation
                if (app_datalogData.newData.data.operation == APP_DATALOG_READ)
                {
                    // Go to Read state
                    app_datalogData.state = APP_DATALOG_STATE_READ_FROM_FILE;
                }
                else if ((app_datalogData.newData.data.operation == APP_DATALOG_APPEND) ||
                         (app_datalogData.newData.data.operation == APP_DATALOG_WRITE))
                {
                    // Go to Write state
                    app_datalogData.state = APP_DATALOG_STATE_WRITE_TO_FILE;
                }
            }

            break;
        }

        case APP_DATALOG_STATE_READ_FROM_FILE:
        {
            if (app_datalogData.newData.data.operation == APP_DATALOG_READ)
            {
                // Read operation
                app_datalogData.newData.fileHandle = SYS_FS_FileOpen(app_datalogData.newData.fileName, (SYS_FS_FILE_OPEN_READ));
                if(app_datalogData.newData.fileHandle != SYS_FS_HANDLE_INVALID)
                {
                    // File open succeeded. Read Data.
                    if (SYS_FS_FileRead(app_datalogData.newData.fileHandle, app_datalogData.newData.data.pData, app_datalogData.newData.data.dataLen) != -1)
                    {
                        // Read success
                        app_datalogData.result = APP_DATALOG_RESULT_SUCCESS;
                    }
                    else
                    {
                        // Read error
                        app_datalogData.result = APP_DATALOG_RESULT_ERROR;
                    }
                    // Go to Close state
                    app_datalogData.state = APP_DATALOG_STATE_CLOSE_FILE;
                }
                else
                {
                    // File open failed.
                    app_datalogData.result = APP_DATALOG_RESULT_ERROR;
                    // Go to report state
                    app_datalogData.state = APP_DATALOG_STATE_REPORT_RESULT;
                }
            }

            break;
        }

        case APP_DATALOG_STATE_WRITE_TO_FILE:
        {
            if (app_datalogData.newData.data.operation == APP_DATALOG_APPEND)
            {
                // Append operation
                app_datalogData.newData.fileHandle = SYS_FS_FileOpen(app_datalogData.newData.fileName, (SYS_FS_FILE_OPEN_APPEND));
                if(app_datalogData.newData.fileHandle != SYS_FS_HANDLE_INVALID)
                {
                    // File open succeeded. Append Data.
                    if (SYS_FS_FileWrite(app_datalogData.newData.fileHandle, app_datalogData.newData.data.pData, app_datalogData.newData.data.dataLen) != -1)
                    {
                        // Write success
                        app_datalogData.result = APP_DATALOG_RESULT_SUCCESS;
                    }
                    else
                    {
                        // Write error
                        app_datalogData.result = APP_DATALOG_RESULT_ERROR;
                    }
                    // Go to Close state
                    app_datalogData.state = APP_DATALOG_STATE_CLOSE_FILE;
                }
                else
                {
                    // File open failed.
                    app_datalogData.result = APP_DATALOG_RESULT_ERROR;
                    // Go to report state
                    app_datalogData.state = APP_DATALOG_STATE_REPORT_RESULT;
                }
            }
            else if (app_datalogData.newData.data.operation == APP_DATALOG_WRITE)
            {
                // Write operation
                app_datalogData.newData.fileHandle = SYS_FS_FileOpen(app_datalogData.newData.fileName, (SYS_FS_FILE_OPEN_WRITE));
                if(app_datalogData.newData.fileHandle != SYS_FS_HANDLE_INVALID)
                {
                    // File open succeeded. Write Data.
                    if (SYS_FS_FileWrite(app_datalogData.newData.fileHandle, app_datalogData.newData.data.pData, app_datalogData.newData.data.dataLen) != -1)
                    {
                        // Write success
                        app_datalogData.result = APP_DATALOG_RESULT_SUCCESS;
                    }
                    else
                    {
                        // Write error
                        app_datalogData.result = APP_DATALOG_RESULT_ERROR;
                    }
                    // Go to Close state
                    app_datalogData.state = APP_DATALOG_STATE_CLOSE_FILE;
                }
                else
                {
                    // File open failed.
                    app_datalogData.result = APP_DATALOG_RESULT_ERROR;
                    // Go to report state
                    app_datalogData.state = APP_DATALOG_STATE_REPORT_RESULT;
                }
            }

            break;
        }

        case APP_DATALOG_STATE_CLOSE_FILE:
        {
            // Close current file
            if (SYS_FS_FileClose(app_datalogData.newData.fileHandle) == SYS_FS_RES_SUCCESS)
            {
                // Go to report state
                app_datalogData.state = APP_DATALOG_STATE_REPORT_RESULT;
            }
            else
            {
                // Go to Error state
                app_datalogData.state = APP_DATALOG_STATE_ERROR;
            }

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
        
#if SYS_FS_AUTOMOUNT_ENABLE
        case APP_DATALOG_STATE_DELAY:
        {
            // Wait delay time
            if (APP_DATALOG_TaskDelay(app_datalogData.delayMs, &app_datalogData.timer))
            {
                // Set next app state
                app_datalogData.state = app_datalogData.nextState;
            }
            break;
        }   
#endif

        // The default state should never be executed.
        default:
        {
            // TODO: Handle error in application's state machine.
            break;
        }
    }
}

bool APP_DATALOG_SendDatalogData(APP_DATALOG_QUEUE_DATA *datalogData)
{
    if (appDatalogQueue.dataSize < APP_DATALOG_QUEUE_DATA_SIZE)
    {
        /* Copy Energy data to the data queue */
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
