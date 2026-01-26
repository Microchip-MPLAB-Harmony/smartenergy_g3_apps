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
    app_events.c

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

#include "definitions.h"
#include "app_events.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

/* Define a semaphore to signal the Metrology Tasks to process new integration
 * data */
OSAL_SEM_DECLARE(appEventsSemID);

/* Define a queue to signal the Events Task to handle metrology events */
QueueHandle_t appEventsQueueID = NULL;

extern QueueHandle_t appDatalogQueueID;
APP_DATALOG_QUEUE_DATA appEventsDatalogQueueData;

/* Define the time in seconds to store events in NVM memory: 15 min by default */
#define APP_EVENTS_TIME_DIFF_TO_STORE_NVM     900

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_EVENTS_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_EVENTS_DATA app_eventsData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

void _APP_EVENTS_GetDataLogCallback(APP_DATALOG_RESULT result)
{
    if (result == APP_DATALOG_RESULT_SUCCESS)
    {
        app_eventsData.dataIsRdy = true;
    }
    else
    {
        app_eventsData.dataIsRdy = false;
    }

    // Post semaphore to wakeup task
    OSAL_SEM_Post(&appEventsSemID);
}

static void _APP_EVENTS_LoadEventsDataFromMemory(APP_EVENTS_EVENT_ID id)
{
    appEventsDatalogQueueData.userId = APP_DATALOG_USER_EVENTS;
    appEventsDatalogQueueData.eventId = (uint8_t )id;
    appEventsDatalogQueueData.operation = APP_DATALOG_READ;
    appEventsDatalogQueueData.pData = (uint8_t *)&app_eventsData.events.event[id];
    appEventsDatalogQueueData.dataLen = sizeof(APP_EVENTS_EVENT_DATA);
    appEventsDatalogQueueData.endCallback = _APP_EVENTS_GetDataLogCallback;
    appEventsDatalogQueueData.date.month = APP_DATALOG_INVALID_MONTH;
    appEventsDatalogQueueData.date.year = APP_DATALOG_INVALID_YEAR;

    xQueueSend(appDatalogQueueID, &appEventsDatalogQueueData, (TickType_t) 0);
}

static void _APP_EVENTS_StoreEventsDataInMemory(APP_EVENTS_EVENT_ID id)
{
    appEventsDatalogQueueData.userId = APP_DATALOG_USER_EVENTS;
    appEventsDatalogQueueData.eventId = (uint8_t )id;
    appEventsDatalogQueueData.operation = APP_DATALOG_WRITE;
    appEventsDatalogQueueData.pData = (uint8_t *)&app_eventsData.events.event[id];
    appEventsDatalogQueueData.dataLen = sizeof(APP_EVENTS_EVENT_DATA);
    appEventsDatalogQueueData.endCallback = _APP_EVENTS_GetDataLogCallback;
    appEventsDatalogQueueData.date.month = APP_DATALOG_INVALID_MONTH;
    appEventsDatalogQueueData.date.year = APP_DATALOG_INVALID_YEAR;

    xQueueSend(appDatalogQueueID, &appEventsDatalogQueueData, (TickType_t) 0);
}

static bool _APP_EVENTS_RegisterEvent(APP_EVENTS_EVENT_ID type, bool enabled, struct tm * timeEvent)
{
    bool registered = false;
    APP_EVENTS_EVENT_DATA * eventData;

    eventData = &app_eventsData.events.event[type];

    switch (eventData->status)
    {
        case NO_EVENT:
        {
            if (enabled)
            {
                APP_EVENTS_EVENT_INFO * eventInfo;

                /* Register Starting Event */
                eventInfo = &eventData->data[eventData->dataIndex];

                /* Register Starting Event */
                eventInfo->startTime = *timeEvent;
                memset(&eventInfo->endTime, 0, sizeof(struct tm));

                eventData->status = EVENT_START;
            }
            break;
        }

        case EVENT_START:
        {
            if (enabled == 0)
            {
                APP_EVENTS_EVENT_INFO * eventInfo;

                /* Register Ending Event */
                eventInfo = &eventData->data[eventData->dataIndex];
                eventInfo->endTime = *timeEvent;

                /* Set index to next logged data */
                eventData->dataIndex++;
                eventData->dataIndex %= EVENT_LOG_MAX_NUMBER;

                /* Clear data of the next index */
                eventInfo = &eventData->data[eventData->dataIndex];
                memset(eventInfo, 0, sizeof(APP_EVENTS_EVENT_INFO));

                eventData->counter++;
                eventData->status = NO_EVENT;

                /* Register event is completed */
                registered = true;
            }
            break;
        }
    }

    return registered;
}

static uint32_t _APP_EVENTS_UpdateEvents(APP_EVENTS_QUEUE_DATA * newEvent)
{
    uint32_t eventMask = 0UL;

    if (_APP_EVENTS_RegisterEvent(SAG_UA_EVENT_ID, newEvent->eventFlags.sagA, &newEvent->eventTime))
    {
        eventMask |= (1 << SAG_UA_EVENT_ID);
    }

    if (_APP_EVENTS_RegisterEvent(SAG_UB_EVENT_ID, newEvent->eventFlags.sagB, &newEvent->eventTime))
    {
        eventMask |= (1 << SAG_UB_EVENT_ID);
    }

    if (_APP_EVENTS_RegisterEvent(SAG_UC_EVENT_ID, newEvent->eventFlags.sagC, &newEvent->eventTime))
    {
        eventMask |= (1 << SAG_UC_EVENT_ID);
    }

    if (_APP_EVENTS_RegisterEvent(SWELL_UA_EVENT_ID, newEvent->eventFlags.swellA, &newEvent->eventTime))
    {
        eventMask |= (1 << SWELL_UA_EVENT_ID);
    }

    if (_APP_EVENTS_RegisterEvent(SWELL_UB_EVENT_ID, newEvent->eventFlags.swellB, &newEvent->eventTime))
    {
        eventMask |= (1 << SWELL_UB_EVENT_ID);
    }

    if (_APP_EVENTS_RegisterEvent(SWELL_UC_EVENT_ID, newEvent->eventFlags.swellC, &newEvent->eventTime))
    {
        eventMask |= (1 << SWELL_UC_EVENT_ID);
    }

    if (_APP_EVENTS_RegisterEvent(POW_PA_EVENT_ID, newEvent->eventFlags.paDir, &newEvent->eventTime))
    {
        eventMask |= (1 << POW_PA_EVENT_ID);
    }

    if (_APP_EVENTS_RegisterEvent(POW_PB_EVENT_ID, newEvent->eventFlags.pbDir, &newEvent->eventTime))
    {
        eventMask |= (1 << POW_PB_EVENT_ID);
    }

    if (_APP_EVENTS_RegisterEvent(POW_PC_EVENT_ID, newEvent->eventFlags.pcDir, &newEvent->eventTime))
    {
        eventMask |= (1 << POW_PC_EVENT_ID);
    }

    /* Update Event Flags */
    app_eventsData.flags = newEvent->eventFlags;

    return eventMask;
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_EVENTS_Initialize ( void )

  Remarks:
    See prototype in app_events.h.
 */

void APP_EVENTS_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    app_eventsData.state = APP_EVENTS_STATE_WAITING_DATALOG;

    /* Initialize Events data */
    memset(&app_eventsData.events, 0, sizeof(APP_EVENTS_EVENTS));

    /* Create the Switches Semaphore. */
    if (OSAL_SEM_Create(&appEventsSemID, OSAL_SEM_TYPE_BINARY, 0, 0) == OSAL_RESULT_FALSE)
    {
        /* Handle error condition. Not sufficient memory to create semaphore */
        app_eventsData.state = APP_EVENTS_STATE_ERROR;
    }

    // Create a queue capable of containing APP_EVENTS_QUEUE_DATA_SIZE queue data elements.
    appEventsQueueID = xQueueCreate(APP_EVENTS_QUEUE_DATA_SIZE, sizeof(APP_EVENTS_QUEUE_DATA));

    if (appEventsQueueID == NULL)
    {
        // Queue was not created and must not be used.
        app_eventsData.state = APP_EVENTS_STATE_ERROR;
        return;
    }
}


/******************************************************************************
  Function:
    void APP_EVENTS_Tasks ( void )

  Remarks:
    See prototype in app_events.h.
 */

void APP_EVENTS_Tasks ( void )
{
    /* Check the application's current state. */
    switch ( app_eventsData.state )
    {
        case APP_EVENTS_STATE_WAITING_DATALOG:
        {
            if (APP_DATALOG_GetStatus() == APP_DATALOG_STATE_READY)
            {
                app_eventsData.eventId = (APP_EVENTS_EVENT_ID)0U;
                app_eventsData.state = APP_EVENTS_STATE_READ_EVENT;
            }

            vTaskDelay(10 / portTICK_PERIOD_MS);
            break;
        }

        case APP_EVENTS_STATE_READ_EVENT:
        {
            _APP_EVENTS_LoadEventsDataFromMemory(app_eventsData.eventId);

            /* Wait for the semaphore to load data from memory */
            OSAL_SEM_Pend(&appEventsSemID, OSAL_WAIT_FOREVER);

            if (app_eventsData.dataIsRdy == true)
            {
                app_eventsData.eventId++;
                if (app_eventsData.eventId >= EVENTS_NUM_ID)
                {
                    app_eventsData.state = APP_EVENTS_STATE_RUNNING;
                    RTC_TimeGet(&app_eventsData.lastNVMUpdTime);
                    app_eventsData.eventMask = 0;
                }
            }
            else
            {
                /* Store event and read it again */
                _APP_EVENTS_StoreEventsDataInMemory(app_eventsData.eventId);

                /* Wait for the semaphore to load data from memory */
                OSAL_SEM_Pend(&appEventsSemID, OSAL_WAIT_FOREVER);
            }

            vTaskDelay(10 / portTICK_PERIOD_MS);

            break;
        }

        case APP_EVENTS_STATE_RUNNING:
        {
            if (xQueueReceive(appEventsQueueID, &app_eventsData.newEvent, portMAX_DELAY) == pdPASS)
            {
                app_eventsData.eventMask |= _APP_EVENTS_UpdateEvents(&app_eventsData.newEvent);

                if (app_eventsData.eventMask > 0)
                {
                    time_t currTime;
                    time_t lastNvmUpdTime;

                    RTC_TimeGet(&app_eventsData.currentTime);

                    currTime = mktime(&app_eventsData.currentTime);
                    lastNvmUpdTime = mktime(&app_eventsData.lastNVMUpdTime);

                    if (difftime(currTime, lastNvmUpdTime) >= APP_EVENTS_TIME_DIFF_TO_STORE_NVM)
                    {
                        app_eventsData.state = APP_EVENTS_STATE_STORE_NVM;
                    }
                }
            }

            break;
        }

        case APP_EVENTS_STATE_STORE_NVM:
        {
            APP_EVENTS_EVENT_ID id = (APP_EVENTS_EVENT_ID)0U;
            uint32_t eventMask = app_eventsData.eventMask;

            while (eventMask != 0U)
            {
                if (eventMask & 0x01)
                {
                    _APP_EVENTS_StoreEventsDataInMemory(id);
                    /* Wait for the semaphore to load data from memory */
                    OSAL_SEM_Pend(&appEventsSemID, OSAL_WAIT_FOREVER);
                }
                eventMask >>= 1U;
                id++;
            }

            app_eventsData.eventMask = 0;
            RTC_TimeGet(&app_eventsData.lastNVMUpdTime);

            app_eventsData.state = APP_EVENTS_STATE_RUNNING;

            vTaskDelay(10 / portTICK_PERIOD_MS);

            break;
        }

        /* The default state should never be executed. */
        case APP_EVENTS_STATE_ERROR:
        default:
        {
            /* TODO: Handle error in application's state machine. */
            SYS_CMD_MESSAGE("ERROR: Events app has been corrupted.\r\n");
            /* Wait for the metrology semaphore */
            OSAL_SEM_Pend(&appEventsSemID, OSAL_WAIT_FOREVER);
            break;
        }
    }
}

void APP_EVENTS_ClearEvents(void)
{

    /* Erase all the event records stored in non volatile memory */
    appEventsDatalogQueueData.userId = APP_DATALOG_USER_EVENTS;
    appEventsDatalogQueueData.operation = APP_DATALOG_ERASE;
    appEventsDatalogQueueData.pData = NULL;
    /* Use full events size */
    appEventsDatalogQueueData.dataLen = sizeof(APP_EVENTS_EVENTS);
    appEventsDatalogQueueData.endCallback = NULL;
    appEventsDatalogQueueData.date.month = APP_DATALOG_INVALID_MONTH;
    appEventsDatalogQueueData.date.year = APP_DATALOG_INVALID_YEAR;

    xQueueSend(appDatalogQueueID, &appEventsDatalogQueueData, (TickType_t) 0);

    /* Clear all events data */
    memset(&app_eventsData.events, 0, sizeof(APP_EVENTS_EVENTS));
}

bool APP_EVENTS_GetNumEvents(APP_EVENTS_EVENT_ID eventId, uint8_t * counter)
{
    if (eventId >= EVENTS_NUM_ID)
    {
        return false;
    }

    *counter = app_eventsData.events.event[eventId].counter;

    return true;
}

bool APP_EVENTS_GetEventInfo(APP_EVENTS_EVENT_ID eventId, uint8_t offset, APP_EVENTS_EVENT_INFO *eventInfo)
{
    APP_EVENTS_EVENT_DATA *pEvent;
    uint8_t index;

    if (eventId >= EVENTS_NUM_ID)
    {
        return false;
    }

    if (offset >= EVENT_LOG_MAX_NUMBER)
    {
        return false;
    }

    pEvent = &app_eventsData.events.event[eventId];

    // Get index from the last event offset
    index = pEvent->dataIndex - offset;
    if (offset > pEvent->dataIndex)
    {
        index += EVENT_LOG_MAX_NUMBER;
    }

    *eventInfo = pEvent->data[index];

    return true;
}

void APP_EVENTS_GetLastEventFlags(DRV_METROLOGY_AFE_EVENTS *eventFlags)
{
    if (eventFlags)
    {
        *eventFlags = app_eventsData.flags;
    }
}


/*******************************************************************************
 End of File
 */
