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
    app_energy.c

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

#include <time.h>
#include "definitions.h"
#include "app_energy.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************
/* Define a queue to signal the Datalog Tasks to store data */
extern APP_DATALOG_QUEUE appDatalogQueue;
APP_DATALOG_QUEUE_DATA appEnergyDatalogQueueData;

/* Define a queue to signal the Energy Tasks to process new energy measurement */
APP_ENERGY_QUEUE appEnergyQueue;

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_ENERGY_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_ENERGY_DATA app_energyData;

#define APP_ENERGY_MIN_RTC_BACKUP     15

#define BUILD_TIME_HOUR     ((__TIME__[0] - '0') * 10 + __TIME__[1] - '0')
#define BUILD_TIME_MIN      ((__TIME__[3] - '0') * 10 + __TIME__[4] - '0')
#define BUILD_TIME_SEC      ((__TIME__[6] - '0') * 10 + __TIME__[7] - '0')


#define BUILD_DATE_YEAR     ((__DATE__[7] - '0') * 1000 + \
                             (__DATE__[8] - '0') * 100 + \
                             (__DATE__[9] - '0') * 10  + \
                             (__DATE__[10] - '0'))

#define BUILD_DATE_DAY      (((__DATE__[4] == 0x20)? 0:(__DATE__[4] - '0') * 10) + \
                             (__DATE__[5] - '0'))

const uint8_t app_energyDayTbl[12] = {0x31, 0x28, 0x31, 0x30, 0x31, 0x30, 0x31, 0x31, 0x30, 0x31, 0x30, 0x31};
const char app_energyMonthTbl[12][3] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
const uint8_t app_energyDayWeekTbl[12] = {6, 2, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};

const APP_ENERGY_TOU_TIME_ZONE gAppEnergyTOUDefault[APP_ENERGY_TOU_MAX_ZONES] =
{
    {TARIFF_2, _UINT32_(8), _UINT32_(30)},
    {TARIFF_3, _UINT32_(10), _UINT32_(30)},
    {TARIFF_1, _UINT32_(18), _UINT32_(30)},
    {TARIFF_4, _UINT32_(22), _UINT32_(00)},
    {TARIFF_INVALID, _UINT32_(0), _UINT32_(0)},
    {TARIFF_INVALID, _UINT32_(0), _UINT32_(0)},
    {TARIFF_INVALID, _UINT32_(0), _UINT32_(0)},
    {TARIFF_INVALID, _UINT32_(0), _UINT32_(0)}
};

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

uint32_t int_cnt = 0;
uint32_t int_cnt_timev = 0;
uint32_t int_cnt_calev = 0;

void RTC_callback(uint32_t int_cause, uintptr_t context)
{
    int_cnt++;
    if (int_cause & RTC_SR_TIMEV_Msk)
    {
        app_energyData.eventMinute = true;
        int_cnt_timev++;
    }

    if (int_cause & RTC_SR_CALEV_Msk)
    {
        app_energyData.eventMonth = true;
        int_cnt_calev++;
    }
}

void _APP_ENERGY_GetRTCDataLogCallback(APP_DATALOG_RESULT result)
{
    if (result == APP_DATALOG_RESULT_SUCCESS)
    {
        app_energyData.dataIsRdy = true;
    }

    /* Handle Task state */
    app_energyData.state = APP_ENERGY_STATE_SET_RTC;
}

void _APP_ENERGY_SetRTCDataLogCallback(APP_DATALOG_RESULT result)
{
    if (result == APP_DATALOG_RESULT_SUCCESS)
    {
        app_energyData.dataIsRdy = true;
    }

    /* Handle Task state */
    if (app_energyData.state == APP_ENERGY_STATE_WAIT_DATA)
    {
        app_energyData.state = APP_ENERGY_STATE_INIT_TOU;
    }
}

void _APP_ENERGY_GetTOUDataLogCallback(APP_DATALOG_RESULT result)
{
    if (result == APP_DATALOG_RESULT_SUCCESS)
    {
        app_energyData.dataIsRdy = true;
    }

    /* Handle Task state */
    app_energyData.state = APP_ENERGY_STATE_SET_TOU;
}

void _APP_ENERGY_SetTOUDataLogCallback(APP_DATALOG_RESULT result)
{
    if (result == APP_DATALOG_RESULT_SUCCESS)
    {
        app_energyData.dataIsRdy = true;
    }

    /* Handle Task state */
    if (app_energyData.state == APP_ENERGY_STATE_WAIT_DATA)
    {
        app_energyData.state = APP_ENERGY_STATE_INIT_ENERGY;
    }
}

void _APP_ENERGY_GetEnergyDataLogCallback(APP_DATALOG_RESULT result)
{
    if (result == APP_DATALOG_RESULT_SUCCESS)
    {
        app_energyData.dataIsRdy = true;
    }

    if (app_energyData.generateEnergyReponse == false)
    {
        /* Handle Task state */
        if (app_energyData.state == APP_ENERGY_STATE_WAIT_DATA)
        {
            app_energyData.state = APP_ENERGY_STATE_INIT_DEMAND;
        }
    }
    else
    {
        /* Handle Task state */
        app_energyData.state = APP_ENERGY_STATE_RESPONSE_MONTH_ENERGY;
    }
}

void _APP_ENERGY_SetEnergyDataLogCallback(APP_DATALOG_RESULT result)
{
    if (result == APP_DATALOG_RESULT_SUCCESS)
    {
        app_energyData.dataIsRdy = true;
    }

    /* Handle Task state */
    if (app_energyData.state == APP_ENERGY_STATE_WAIT_DATA)
    {
        app_energyData.state = APP_ENERGY_STATE_INIT_DEMAND;
    }
}

void _APP_ENERGY_GetDemandDataLogCallback(APP_DATALOG_RESULT result)
{
    if (result == APP_DATALOG_RESULT_SUCCESS)
    {
        app_energyData.dataIsRdy = true;
    }

    if (app_energyData.generateDemandReponse == false)
    {
        /* Update RTC */
        RTC_TimeGet(&app_energyData.time);
        /* Handle Task state */
        app_energyData.state = APP_ENERGY_STATE_RUNNING;
    }
    else
    {
        /* Handle Task state */
        app_energyData.state = APP_ENERGY_STATE_RESPONSE_MAX_DEMAND;
    }
}

void _APP_ENERGY_SetDemandDataLogCallback(APP_DATALOG_RESULT result)
{
    if (result == APP_DATALOG_RESULT_SUCCESS)
    {
        app_energyData.dataIsRdy = true;
    }

    /* Handle Task state */
    app_energyData.state = APP_ENERGY_STATE_RUNNING;
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

static void _APP_ENERGY_InitEnergyQueue(void)
{
    /* Clear DataLog Queue data */
    memset(&appEnergyQueue, 0, sizeof(appEnergyQueue));

    /* Init Queue pointers */
    appEnergyQueue.dataRd = &appEnergyQueue.data[0];
    appEnergyQueue.dataWr = appEnergyQueue.dataRd;
}

static bool _APP_ENERGY_ReceiveEnergyData(APP_ENERGY_QUEUE_DATA *energyData)
{
    if (appEnergyQueue.dataSize)
    {
        /* Copy data to the data pointer */
        memcpy(energyData, appEnergyQueue.dataRd, sizeof(APP_ENERGY_QUEUE_DATA));

        /* Update Queue as a circular buffer */
        appEnergyQueue.dataSize--;
        if (appEnergyQueue.dataRd == &appEnergyQueue.data[APP_ENERGY_QUEUE_DATA_SIZE - 1])
        {
            appEnergyQueue.dataRd = &appEnergyQueue.data[0];
        }
        else
        {
            appEnergyQueue.dataRd++;
        }

        return true;
    }

    return false;
}

static uint8_t _APP_ENERGY_getTOUIndex(struct tm * time)
{
    uint32_t value, slotValue;
    uint8_t index;
    uint8_t counterZones;
    uint8_t result = 0;

    counterZones = app_energyData.tou.usedZones;
    result = counterZones - 1;

    value = (time->tm_hour * 100) + time->tm_min;
    for (index = 0; index < counterZones; index++)
    {
        slotValue = (app_energyData.tou.timeZone[index].hour * 100) + app_energyData.tou.timeZone[index].minute;
        if (value < slotValue)
        {
            if (index != 0)
            {
                result = index - 1;
            }

            break;
        }
    }

    if ((app_energyData.tou.timeZone[result].hour == time->tm_hour) &&
        (app_energyData.tou.timeZone[result].minute == time->tm_min))
    {
        if (result == 0)
        {
            result = counterZones - 1;
        }
        else
        {
            result--;
        }

    }

    return result;
}

static APP_ENERGY_TARIFF_TYPE _APP_ENERGY_getTariffIndex(struct tm * time)
{
    uint8_t touIndex;

    touIndex = _APP_ENERGY_getTOUIndex(time);
    return (app_energyData.tou.timeZone[touIndex].tariff - 1);
}

static bool _APP_ENERGY_CheckRTCFromReset(void)
{
    uint32_t enabledInterrupts = RTC_REGS->RTC_IMR;

    if (enabledInterrupts & (RTC_INT_TIME | RTC_INT_CALENDAR))
    {
        return false;
    }
    else
    {
        return true;
    }
}

static void _APP_ENERGY_SetBuildTimeRTC(struct tm *time)
{
    char dateBuff[12];
    uint16_t y;
    uint8_t index, d , m;

    /* Set RTC time from COMPILER settings */
    time->tm_hour = BUILD_TIME_HOUR;
    time->tm_min = BUILD_TIME_MIN;
    time->tm_sec = BUILD_TIME_SEC;
    time->tm_year = BUILD_DATE_YEAR - 1900;
    time->tm_mday = BUILD_DATE_DAY;

    sprintf(dateBuff, "%s", __DATE__);
    time->tm_mon = 0;
    for (index = 0; index < 12; index++) {
        if (memcmp(&app_energyMonthTbl[index], dateBuff, 3) == 0) {
            time->tm_mon = index;
            break;
        }
    }

    /* Get day of the week: Keith's expression */
    /* 0: Sunday, 1 = Monday ... 6 = Saturday */
    y = BUILD_DATE_YEAR;
    m = time->tm_mon + 1;
    d = time->tm_mday;
    time->tm_wday = (d+=m<3 ? y-- : y-2,23*m/9+d+4+y/4-y/100+y/400) % 7;
}

static bool _APP_ENERGY_InitializeRTC(bool dataValid)
{
    struct tm *pTime = &app_energyData.time;
    bool updRtc = false;
    bool updNvm = false;

    if (!dataValid)
    {
        /* Set Build Time */
        _APP_ENERGY_SetBuildTimeRTC(pTime);
        updRtc = true;
        updNvm = true;
    }
    else
    {
        time_t rtcTime = mktime(&app_energyData.startupTime);
        time_t nvmTime = mktime(pTime);

        if (difftime(rtcTime, nvmTime) >= 0)
        {
            /* Update energy system time */
            *pTime = *(struct tm *)&app_energyData.startupTime;
            updRtc = false;
            updNvm = true;
        }
        else
        {
            updRtc = true;
            updNvm = false;
        }
    }

    if (updRtc == true)
    {
        bool rtcResult;

        /* Set RTC Time to current system time. */
        rtcResult = RTC_TimeSet(pTime);
        while (rtcResult == false)
        {
            rtcResult = RTC_TimeSet(pTime);
        }
    }

    RTC_CallbackRegister(RTC_callback, 0);
    RTC_InterruptEnable(RTC_INT_TIME | RTC_INT_CALENDAR);

    return updNvm;
}

static void _APP_ENERGY_InitializeTOU(bool dataValid)
{
    struct tm time;
    uint8_t counterZones = 0;
    uint8_t index;

    if (!dataValid)
    {
        /* Set TOU by default */
        memcpy(&app_energyData.tou.timeZone, &gAppEnergyTOUDefault, sizeof(app_energyData.tou.timeZone));
    }

    /* Count used zones */
    for (index = 0; index < APP_ENERGY_TOU_MAX_ZONES; index++)
    {
        if (app_energyData.tou.timeZone[index].tariff != TARIFF_INVALID)
        {
            counterZones++;
        }
    }
    app_energyData.tou.usedZones = counterZones;

    /* Update current Tariff Type */
    RTC_TimeGet(&time);
    app_energyData.currentTariffIndex = _APP_ENERGY_getTariffIndex(&time);
}

static bool _APP_ENERGY_UpdateDemand(float demand, struct tm * time)
{
    float demandPeriod;
    float demandMax = 0.0f;
    uint8_t minIndex = 0;
    bool update = false;

    /* Data comes from previous minute */
    if (time->tm_min == 0U)
    {
        minIndex = 59U;
    }
    else
    {
        minIndex = time->tm_min - 1U;
    }

    /* Write average power from previous minute to the demand window */
    minIndex %= 15;
    demandPeriod = demand / (float)app_energyData.counterIntegrationPeriods;
    app_energyData.demand.window[minIndex] = demandPeriod;

    /* Evaluate demand only at the end of each 15-minute window */
    if (minIndex == 14U)
    {
        /* Average demand over 15-minute window */
        for (uint8_t index = 0; index < 15U; index++)
        {
            demandMax += app_energyData.demand.window[index];
        }

        demandMax /= 15.0;
        
        /* Update Max Demand for the current tariff */
        if (app_energyData.demand.maxDemand.tariff[app_energyData.currentTariffIndex].value < demandMax)
        {
            app_energyData.demand.maxDemand.tariff[app_energyData.currentTariffIndex].value = demandMax;
            app_energyData.demand.maxDemand.tariff[app_energyData.currentTariffIndex].month = time->tm_mon;
            app_energyData.demand.maxDemand.tariff[app_energyData.currentTariffIndex].day = time->tm_mday;
            app_energyData.demand.maxDemand.tariff[app_energyData.currentTariffIndex].hour = time->tm_hour;
            app_energyData.demand.maxDemand.tariff[app_energyData.currentTariffIndex].minute = time->tm_min;

            update = true;
        }

        /* Update total Max Demand */
        if (app_energyData.demand.maxDemand.maxDemad.value < demandMax)
        {
            app_energyData.demand.maxDemand.maxDemad.value = demandMax;
            app_energyData.demand.maxDemand.maxDemad.month = time->tm_mon;
            app_energyData.demand.maxDemand.maxDemad.day = time->tm_mday;
            app_energyData.demand.maxDemand.maxDemad.hour = time->tm_hour;
            app_energyData.demand.maxDemand.maxDemad.minute = time->tm_min;

            update = true;
        }
    }

    return update;
}

static void _APP_ENERGY_SetMonthStore(APP_DATALOG_QUEUE_DATA *datalogQueueData, struct tm * time)
{
    if ((time->tm_mday == 1) && (time->tm_hour == 0) && (time->tm_min == 0))
    {
        /* Month has just changed. Set the previous month */
        if (time->tm_mon == 0)
        {
            datalogQueueData->date.month = 11;
            datalogQueueData->date.year = time->tm_year - 1;
        }
        else
        {
            datalogQueueData->date.month = time->tm_mon - 1;
            datalogQueueData->date.year = time->tm_year;
        }
    }
    else
    {
        /* Normal case */
        datalogQueueData->date.month = time->tm_mon;
        datalogQueueData->date.year = time->tm_year;
    }
}

static void _APP_ENERGY_LoadRTCDataFromMemory(void)
{
    appEnergyDatalogQueueData.userId = APP_DATALOG_USER_RTC;
    appEnergyDatalogQueueData.operation = APP_DATALOG_READ;
    appEnergyDatalogQueueData.pData = (uint8_t *)&app_energyData.time;
    appEnergyDatalogQueueData.dataLen = sizeof(struct tm);
    appEnergyDatalogQueueData.endCallback = _APP_ENERGY_GetRTCDataLogCallback;
    appEnergyDatalogQueueData.date.month = APP_DATALOG_INVALID_MONTH;
    appEnergyDatalogQueueData.date.year = APP_DATALOG_INVALID_YEAR;

    APP_DATALOG_SendDatalogData(&appEnergyDatalogQueueData);
}

static void _APP_ENERGY_StoreRTCDataInMemory(void)
{
    appEnergyDatalogQueueData.userId = APP_DATALOG_USER_RTC;
    appEnergyDatalogQueueData.operation = APP_DATALOG_WRITE;
    appEnergyDatalogQueueData.pData = (uint8_t *)&app_energyData.time;
    appEnergyDatalogQueueData.dataLen = sizeof(struct tm);
    appEnergyDatalogQueueData.endCallback = _APP_ENERGY_SetRTCDataLogCallback;
    appEnergyDatalogQueueData.date.month = APP_DATALOG_INVALID_MONTH;
    appEnergyDatalogQueueData.date.year = APP_DATALOG_INVALID_YEAR;

    APP_DATALOG_SendDatalogData(&appEnergyDatalogQueueData);
}

static void _APP_ENERGY_LoadTOUDataFromMemory(void)
{
    appEnergyDatalogQueueData.userId = APP_DATALOG_USER_TOU;
    appEnergyDatalogQueueData.operation = APP_DATALOG_READ;
    appEnergyDatalogQueueData.pData = (uint8_t *)&app_energyData.tou;
    appEnergyDatalogQueueData.dataLen = sizeof(APP_ENERGY_TOU);
    appEnergyDatalogQueueData.endCallback = _APP_ENERGY_GetTOUDataLogCallback;
    appEnergyDatalogQueueData.date.month = APP_DATALOG_INVALID_MONTH;
    appEnergyDatalogQueueData.date.year = APP_DATALOG_INVALID_YEAR;

    APP_DATALOG_SendDatalogData(&appEnergyDatalogQueueData);
}

static void _APP_ENERGY_StoreTOUDataInMemory(void)
{
    appEnergyDatalogQueueData.userId = APP_DATALOG_USER_TOU;
    appEnergyDatalogQueueData.operation = APP_DATALOG_WRITE;
    appEnergyDatalogQueueData.pData = (uint8_t *)&app_energyData.tou;
    appEnergyDatalogQueueData.dataLen = sizeof(APP_ENERGY_TOU);
    appEnergyDatalogQueueData.endCallback = _APP_ENERGY_SetTOUDataLogCallback;
    appEnergyDatalogQueueData.date.month = APP_DATALOG_INVALID_MONTH;
    appEnergyDatalogQueueData.date.year = APP_DATALOG_INVALID_YEAR;

    APP_DATALOG_SendDatalogData(&appEnergyDatalogQueueData);
}

static void _APP_ENERGY_LoadEnergyDataFromMemory(struct tm * time, APP_ENERGY_ACCUMULATORS *pEnergyAcc)
{
    appEnergyDatalogQueueData.userId = APP_DATALOG_USER_ENERGY;
    appEnergyDatalogQueueData.operation = APP_DATALOG_READ;
    appEnergyDatalogQueueData.pData = (uint8_t *)pEnergyAcc;
    appEnergyDatalogQueueData.dataLen = sizeof(APP_ENERGY_ACCUMULATORS);
    appEnergyDatalogQueueData.date.month = time->tm_mon;
    appEnergyDatalogQueueData.date.year = time->tm_year;
    appEnergyDatalogQueueData.endCallback = _APP_ENERGY_GetEnergyDataLogCallback;

    APP_DATALOG_SendDatalogData(&appEnergyDatalogQueueData);
}

static void _APP_ENERGY_StoreEnergyDataInMemory(struct tm * time)
{
    /* Copy Energy Accumulators to temporal buffer, to avoid it is cleared in month change */
    memcpy(&app_energyData.energyAccumulatorToStore, &app_energyData.energyAccumulator, sizeof(APP_ENERGY_ACCUMULATORS));

    appEnergyDatalogQueueData.userId = APP_DATALOG_USER_ENERGY;
    appEnergyDatalogQueueData.operation = APP_DATALOG_WRITE;
    appEnergyDatalogQueueData.pData = (uint8_t *)&app_energyData.energyAccumulatorToStore;
    appEnergyDatalogQueueData.dataLen = sizeof(APP_ENERGY_ACCUMULATORS);
    appEnergyDatalogQueueData.endCallback = _APP_ENERGY_SetEnergyDataLogCallback;
    _APP_ENERGY_SetMonthStore(&appEnergyDatalogQueueData, time);

    APP_DATALOG_SendDatalogData(&appEnergyDatalogQueueData);
}

static void _APP_ENERGY_LoadDemandDataFromMemory(struct tm * time, APP_ENERGY_MAX_DEMAND *pData)
{
    appEnergyDatalogQueueData.userId = APP_DATALOG_USER_DEMAND;
    appEnergyDatalogQueueData.operation = APP_DATALOG_READ;
    appEnergyDatalogQueueData.pData = (uint8_t *)pData;
    appEnergyDatalogQueueData.dataLen = sizeof(APP_ENERGY_MAX_DEMAND);
    appEnergyDatalogQueueData.date.month = time->tm_mon;
    appEnergyDatalogQueueData.date.year = time->tm_year;
    appEnergyDatalogQueueData.endCallback = _APP_ENERGY_GetDemandDataLogCallback;

    APP_DATALOG_SendDatalogData(&appEnergyDatalogQueueData);
}

static void _APP_ENERGY_StoreDemandDataInMemory(struct tm * time)
{
    /* Copy Demand to temporal buffer, to avoid it is cleared in month change */
    memcpy(&app_energyData.demandToStore, &app_energyData.demand.maxDemand, sizeof(APP_ENERGY_MAX_DEMAND));

    appEnergyDatalogQueueData.userId = APP_DATALOG_USER_DEMAND;
    appEnergyDatalogQueueData.operation = APP_DATALOG_WRITE;
    appEnergyDatalogQueueData.pData = (uint8_t *)&app_energyData.demandToStore;
    appEnergyDatalogQueueData.dataLen = sizeof(APP_ENERGY_MAX_DEMAND);
    appEnergyDatalogQueueData.endCallback = _APP_ENERGY_SetDemandDataLogCallback;
    _APP_ENERGY_SetMonthStore(&appEnergyDatalogQueueData, time);

    APP_DATALOG_SendDatalogData(&appEnergyDatalogQueueData);
}

static void _APP_ENERGY_SupplyMonitorCallback(uint32_t supc_status, uintptr_t context)
{
    if (supc_status & SUPC_IMR_VDD3V3SMEV_Msk)
    {
        // User may set Low Power mode upon this event
        // This application demonstrates Low Power mode through a console command
        // or through a switch button combination
        // Refer to documentation for more details
    }
}

static void _APP_ENERGY_CheckTamperDetection(void)
{
    /* Check the Tamper Event Counter value */
    if (RTC_REGS->RTC_SUB0[0].RTC_FSTR & RTC_FSTR_TEVCNT_Msk)
    {
        struct tm tamperTime;

        RTC_LastTimeStampGet(&tamperTime, 0);
        SYS_CMD_PRINT("\bDetected TAMPER event: [%02u/%02u/%04u %02u:%02u:%02u]\r\n>",
                  tamperTime.tm_mon + 1, tamperTime.tm_mday, tamperTime.tm_year + 1900,
                  tamperTime.tm_hour, tamperTime.tm_min, tamperTime.tm_sec);
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_ENERGY_Initialize (void)

  Remarks:
    See prototype in app_energy.h.
 */

void APP_ENERGY_Initialize (void)
{
    /* Place the App state machine in its initial state. */
    app_energyData.state = APP_ENERGY_STATE_WAITING_DATALOG;

     /* Initialize Energy callbacks */
    app_energyData.maxDemandCallback = NULL;
    app_energyData.monthEnergyCallback = NULL;

    /* Clear SUPC Interrupt Source */
    SYS_INT_SourceStatusClear(SUPC_IRQn);

    /* Set callback for the supply monitor */
    SUPC_CallbackRegister(_APP_ENERGY_SupplyMonitorCallback, 0);

    /* Clear RTC TIME & CALENDAR events */
    app_energyData.eventMinute = false;
    app_energyData.eventMonth = false;

    /* Set time to store RTC time in external memory as backup */
    app_energyData.minRtcBackup = APP_ENERGY_MIN_RTC_BACKUP;

    /* Clear Energy accumulators */
    memset(&app_energyData.energyAccumulator, 0, sizeof(APP_ENERGY_ACCUMULATORS));

    /* Clear Demand data */
    memset(&app_energyData.demand, 0, sizeof(APP_ENERGY_DEMAND));

    /* Clear the counter of integration periods */
    app_energyData.counterIntegrationPeriods = 0U;

    /* Init Energy Queue */
    _APP_ENERGY_InitEnergyQueue();

}


/******************************************************************************
  Function:
    void APP_ENERGY_Tasks (void)

  Remarks:
    See prototype in app_energy.h.
 */

void APP_ENERGY_Tasks (void)
{
    /* Check the application's current state. */
    switch (app_energyData.state)
    {
        case APP_ENERGY_STATE_WAITING_DATALOG:
        {
            if (APP_DATALOG_GetStatus() == APP_DATALOG_STATE_READY)
            {
                app_energyData.state = APP_ENERGY_STATE_INIT_RTC;
            }

            break;
        }

        case APP_ENERGY_STATE_INIT_RTC:
        {
            RTC_TimeGet(&app_energyData.startupTime);
            if (_APP_ENERGY_CheckRTCFromReset())
            {
                /* Reset flag to request data to datalog app */
                app_energyData.dataIsRdy = false;
                /* Check if there are RTC data in memory */
                if (APP_DATALOG_DataIsValid(APP_DATALOG_USER_RTC))
                {
                    /* RTC data exists */
                    _APP_ENERGY_LoadRTCDataFromMemory();
                    /* Wait for loading data from memory */
                    app_energyData.state = APP_ENERGY_STATE_WAIT_DATA;
                }
                else
                {
                    app_energyData.state = APP_ENERGY_STATE_SET_RTC;
                }
            }
            else
            {
                RTC_TimeGet(&app_energyData.time);
                app_energyData.dataIsRdy = true;

                app_energyData.state = APP_ENERGY_STATE_SET_RTC;
            }

            break;
        }

        case APP_ENERGY_STATE_SET_RTC:
        {
            if (_APP_ENERGY_InitializeRTC(app_energyData.dataIsRdy))
            {
                /* Update RTC Data in memory. */
                _APP_ENERGY_StoreRTCDataInMemory();
                app_energyData.state = APP_ENERGY_STATE_WAIT_DATA;
            }
            else
            {
                app_energyData.state = APP_ENERGY_STATE_INIT_TOU;
            }

            break;
        }

        case APP_ENERGY_STATE_INIT_TOU:
        {
            /* Reset flag to request data to datalog app */
            app_energyData.dataIsRdy = false;

            /* Check if there are TOU data in memory */
            if (APP_DATALOG_DataIsValid(APP_DATALOG_USER_TOU))
            {
                /* TOU data exists */
                _APP_ENERGY_LoadTOUDataFromMemory();
                /* Wait for loading data from memory */
                app_energyData.state = APP_ENERGY_STATE_WAIT_DATA;
            }
            else
            {
                app_energyData.state = APP_ENERGY_STATE_SET_TOU;
            }

            break;
        }

        case APP_ENERGY_STATE_SET_TOU:
        {
            _APP_ENERGY_InitializeTOU(app_energyData.dataIsRdy);

            if (app_energyData.dataIsRdy == false)
            {
                /* There is no valid data in memory. Create TOU Data in memory. */
                _APP_ENERGY_StoreTOUDataInMemory();
                app_energyData.state = APP_ENERGY_STATE_WAIT_DATA;
            }
            else
            {
                app_energyData.state = APP_ENERGY_STATE_INIT_ENERGY;
            }
            break;
        }

        case APP_ENERGY_STATE_INIT_ENERGY:
        {
            /* Reset flag to request data to datalog app */
            app_energyData.dataIsRdy = false;

            /* Check if there are ENERGY data in memory */
            if (APP_DATALOG_DataIsValid(APP_DATALOG_USER_ENERGY))
            {
                /* ENERGY data exists */
                _APP_ENERGY_LoadEnergyDataFromMemory(&app_energyData.time, &app_energyData.energyAccumulator);
                /* Wait for loading data from memory */
                app_energyData.state = APP_ENERGY_STATE_WAIT_DATA;
            }
            else
            {
                /* There is no valid data in memory. Create Energy Data in memory. */
                _APP_ENERGY_StoreEnergyDataInMemory(&app_energyData.time);

                app_energyData.state = APP_ENERGY_STATE_WAIT_DATA;
            }

            break;
        }

        case APP_ENERGY_STATE_INIT_DEMAND:
        {

            /* Reset flag to request data to datalog app */
            app_energyData.dataIsRdy = false;

            /* Check if there are DEMAND data in memory */
            if (APP_DATALOG_DataIsValid(APP_DATALOG_USER_DEMAND))
            {
                /* DEMAND data exists */
                _APP_ENERGY_LoadDemandDataFromMemory(&app_energyData.time, &app_energyData.demand.maxDemand);
                /* Wait for loading data from memory */
                app_energyData.state = APP_ENERGY_STATE_WAIT_DATA;
            }
            else
            {
                /* There is no valid data in memory. Create Demand Data in memory. */
                _APP_ENERGY_StoreDemandDataInMemory(&app_energyData.time);

                /* Update RTC */
                RTC_TimeGet(&app_energyData.time);

                app_energyData.state = APP_ENERGY_STATE_RUNNING;
            }

            break;
        }

        case APP_ENERGY_STATE_RUNNING:
        {
            if (_APP_ENERGY_ReceiveEnergyData(&app_energyData.newQueuedData))
            {
                /* Read RTC and update tariff for energy classification */
                RTC_TimeGet(&app_energyData.time);
                app_energyData.currentTariffIndex = _APP_ENERGY_getTariffIndex(&app_energyData.time);

                /* Update Energy Accumulator */
                app_energyData.energyAccumulator.tariff[app_energyData.currentTariffIndex] += app_energyData.newQueuedData.energy;

                /* Update Demand Accumulator and counter of integration periods */
                app_energyData.demandAccumulator += app_energyData.newQueuedData.Pt;
                app_energyData.counterIntegrationPeriods++;

                /* Check TIME Event (minute) */
                if (app_energyData.eventMinute)
                {
                    /* Clear TIME Event flag */
                    app_energyData.eventMinute = false;

                    /* Re-read RTC to ensure minute has changed */
                    RTC_TimeGet(&app_energyData.time);

                    /* Update demand values */
                    if (_APP_ENERGY_UpdateDemand(app_energyData.demandAccumulator, &app_energyData.time))
                    {
                        /* Update maximum demand */
                        _APP_ENERGY_StoreDemandDataInMemory(&app_energyData.time);
                    }

                    // Restart demand accumulator and counter of integration periods
                    app_energyData.demandAccumulator = 0.0f;
                    app_energyData.counterIntegrationPeriods = 0U;

                    /* Update ENERGY DATALOG once per hour. Ensure SST endurance. */
                    if (app_energyData.time.tm_min == 0)
                    {
                        _APP_ENERGY_StoreEnergyDataInMemory(&app_energyData.time);
                    }

                    /* Check Counter to store a RTC backup */
                    if (!app_energyData.minRtcBackup--)
                    {
                        app_energyData.minRtcBackup = APP_ENERGY_MIN_RTC_BACKUP;

                        /* Update RTC data in memory */
                        _APP_ENERGY_StoreRTCDataInMemory();
                    }
                }

                /* Check CALENDAR Event (month) */
                if (app_energyData.eventMonth)
                {
                    app_energyData.eventMonth = false;

                    /* Clear Energy Accumulators */
                    memset(&app_energyData.energyAccumulator, 0, sizeof(APP_ENERGY_ACCUMULATORS));

                    /* Clear MaxDemand data */
                    memset(&app_energyData.demand.maxDemand, 0, sizeof(APP_ENERGY_MAX_DEMAND));
                }
            }

            /* Check TAMPER detection */
            _APP_ENERGY_CheckTamperDetection();

            break;
        }

        case APP_ENERGY_STATE_GET_MAX_DEMAND:
        {
            /* Reset flag to request data to datalog app */
            app_energyData.dataIsRdy = false;

            /* Generate Demand response */
            app_energyData.generateDemandReponse = true;

            /* Check if there are ENERGY data in memory */
            if (APP_DATALOG_DataIsValid(APP_DATALOG_USER_DEMAND))
            {
                /* ENERGY data exists */
                _APP_ENERGY_LoadDemandDataFromMemory(&app_energyData.timeResponse, app_energyData.pMaxDemandResponse);
                /* Wait for loading data from memory */
                app_energyData.state = APP_ENERGY_STATE_WAIT_DATA;
            }
            else
            {
                app_energyData.state = APP_ENERGY_STATE_RESPONSE_MAX_DEMAND;
            }

            break;
        }

        case APP_ENERGY_STATE_RESPONSE_MAX_DEMAND:
        {
            app_energyData.maxDemandCallback(&app_energyData.timeResponse, app_energyData.dataIsRdy);

            app_energyData.state = APP_ENERGY_STATE_RUNNING;
            app_energyData.generateDemandReponse = false;

            break;
        }

        case APP_ENERGY_STATE_GET_MONTH_ENERGY:
        {
            struct tm sysTime;

            RTC_TimeGet(&sysTime);

            if ((sysTime.tm_mon == app_energyData.timeResponse.tm_mon) &&
                    (sysTime.tm_year == app_energyData.timeResponse.tm_year))
            {
                /* return current accumulated value */
                *app_energyData.pMonthEnergyResponse = app_energyData.energyAccumulator;
                app_energyData.dataIsRdy = true;
                app_energyData.state = APP_ENERGY_STATE_RESPONSE_MONTH_ENERGY;
            }
            else
            {
                /* Reset flag to request data to datalog app */
                app_energyData.dataIsRdy = false;

                /* Generate Energy response */
                app_energyData.generateEnergyReponse = true;

                /* Check if there are ENERGY data in memory */
                if (APP_DATALOG_DataIsValid(APP_DATALOG_USER_ENERGY))
                {
                    /* ENERGY data exists */
                    _APP_ENERGY_LoadEnergyDataFromMemory(&app_energyData.timeResponse, app_energyData.pMonthEnergyResponse);
                    /* Wait for loading data from memory */
                    app_energyData.state = APP_ENERGY_STATE_WAIT_DATA;
                }
                else
                {
                    app_energyData.state = APP_ENERGY_STATE_RESPONSE_MONTH_ENERGY;
                }
            }

            break;
        }

        case APP_ENERGY_STATE_RESPONSE_MONTH_ENERGY:
        {
            app_energyData.monthEnergyCallback(&app_energyData.timeResponse, app_energyData.dataIsRdy);

            app_energyData.state = APP_ENERGY_STATE_RUNNING;
            app_energyData.generateEnergyReponse = false;

            break;
        }

        case APP_ENERGY_STATE_WAIT_DATA:
        {
            /* Waiting Data from DataLog */
            break;
        }

        /* The default state should never be executed. */
        case APP_ENERGY_STATE_ERROR:
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}

APP_ENERGY_TOU_TIME_ZONE * APP_ENERGY_GetTOUTimeZone(void)
{
    return app_energyData.tou.timeZone;
}

void APP_ENERGY_SetTOUTimeZone(APP_ENERGY_TOU_TIME_ZONE *timeZone)
{
    /* Set TOU data */
    memcpy(&app_energyData.tou.timeZone, timeZone, sizeof(app_energyData.tou.timeZone));
    _APP_ENERGY_InitializeTOU(true);
    _APP_ENERGY_StoreTOUDataInMemory();
}

void APP_ENERGY_SetMonthEnergyCallback(APP_ENERGY_MONTH_CALLBACK callback,
        APP_ENERGY_ACCUMULATORS * pMonthEnergyResponse)
{
    app_energyData.monthEnergyCallback = callback;
    app_energyData.pMonthEnergyResponse = pMonthEnergyResponse;
}

bool APP_ENERGY_GetMonthEnergy(struct tm * time)
{
    if ((app_energyData.monthEnergyCallback) && (app_energyData.state == APP_ENERGY_STATE_RUNNING))
    {
        app_energyData.timeResponse = *time;
        app_energyData.state = APP_ENERGY_STATE_GET_MONTH_ENERGY;
        return true;
    }

    return false;
}

void APP_ENERGY_GetCurrentEnergy(APP_ENERGY_ACCUMULATORS * pEnergy)
{
    if (pEnergy)
    {
        *pEnergy = app_energyData.energyAccumulator;
    }
}

void APP_ENERGY_ClearEnergy(bool clearPersistentData)
{
    if (clearPersistentData)
    {
        /* Erase all the energy records stored in non volatile memory */
        appEnergyDatalogQueueData.userId = APP_DATALOG_USER_ENERGY;
        appEnergyDatalogQueueData.operation = APP_DATALOG_ERASE;
        appEnergyDatalogQueueData.pData = NULL;
        appEnergyDatalogQueueData.dataLen = sizeof(APP_ENERGY_ACCUMULATORS);
        appEnergyDatalogQueueData.endCallback = NULL;
        appEnergyDatalogQueueData.date.month = APP_DATALOG_INVALID_MONTH;
        appEnergyDatalogQueueData.date.year = APP_DATALOG_INVALID_YEAR;

        APP_DATALOG_SendDatalogData(&appEnergyDatalogQueueData);
    }
    /* Clear Energy Accumulators */
    memset(&app_energyData.energyAccumulator, 0, sizeof(APP_ENERGY_ACCUMULATORS));
}

void APP_ENERGY_SetMaxDemandCallback(APP_ENERGY_MAXDEMAND_CALLBACK callback,
        APP_ENERGY_MAX_DEMAND * pMaxDemandResponse)
{
    app_energyData.maxDemandCallback = callback;
    app_energyData.pMaxDemandResponse = pMaxDemandResponse;
}

bool APP_ENERGY_GetMonthMaxDemand(struct tm * time)
{
    if ((app_energyData.maxDemandCallback) && (app_energyData.state == APP_ENERGY_STATE_RUNNING))
    {
        app_energyData.timeResponse = *time;
        app_energyData.state = APP_ENERGY_STATE_GET_MAX_DEMAND;
        return true;
    }

    return false;
}

void APP_ENERGY_GetCurrentMaxDemand(APP_ENERGY_MAX_DEMAND * pMaxDemand)
{
    if (pMaxDemand)
    {
        *pMaxDemand = app_energyData.demand.maxDemand;
    }
}

void APP_ENERGY_ClearMaxDemand(bool clearPersistentData)
{
    if (clearPersistentData)
    {
        /* Erase all the demand records stored in non volatile memory */
        appEnergyDatalogQueueData.userId = APP_DATALOG_USER_DEMAND;
        appEnergyDatalogQueueData.operation = APP_DATALOG_ERASE;
        appEnergyDatalogQueueData.pData = NULL;
        appEnergyDatalogQueueData.dataLen = sizeof(APP_ENERGY_MAX_DEMAND);
        appEnergyDatalogQueueData.date.month = APP_DATALOG_INVALID_MONTH;
        appEnergyDatalogQueueData.date.year = APP_DATALOG_INVALID_YEAR;
        appEnergyDatalogQueueData.endCallback = NULL;

        APP_DATALOG_SendDatalogData(&appEnergyDatalogQueueData);
    }
    /* Clear Demand Accumulators */
    app_energyData.demandAccumulator = 0;
    /* Clear Demand data */
    memset(&app_energyData.demand, 0, sizeof(APP_ENERGY_DEMAND));
}

bool APP_ENERGY_SendEnergyData(APP_ENERGY_QUEUE_DATA *energyData)
{
    if (appEnergyQueue.dataSize < APP_ENERGY_QUEUE_DATA_SIZE)
    {
        /* Copy Energy data to the data queue */
        memcpy(appEnergyQueue.dataWr, energyData, sizeof(APP_ENERGY_QUEUE_DATA));

        /* Update Queue as a circular buffer */
        appEnergyQueue.dataSize++;
        if (appEnergyQueue.dataWr == &appEnergyQueue.data[APP_ENERGY_QUEUE_DATA_SIZE - 1])
        {
            appEnergyQueue.dataWr = &appEnergyQueue.data[0];
        }
        else
        {
            appEnergyQueue.dataWr++;
        }

        return true;
    }

    return false;
}

/*******************************************************************************
 End of File
 */
