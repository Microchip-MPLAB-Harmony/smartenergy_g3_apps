/*******************************************************************************
  PAL Source

  File Name:
    pal.c

  Summary:
    Performs interface functionalities between the PHY layer and Harmony
    drivers

  Description:
 *  None
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2024 Microchip Technology Inc. and its subsidiaries.
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
// DOM-IGNORE-END

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */
#include <string.h>
#include <stdio.h>
#include "pal.h"
#include "system/time/sys_time.h"
#include "phy.h"

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Types                                                   */
/* ************************************************************************** */
/* ************************************************************************** */



/* Structure holding the PAL Timer details*/
typedef struct pal_timers_tag
{
    /* Sys Timer Handle*/
    SYS_TIME_HANDLE timerId;
    /* Callback handler invoked upon timer expiry*/
    appCallback_t appCallback;
    /* Argument for the callback handler*/
    void *param_cb;
    /* Status of timer start*/
    bool isTimerStarted;
}PAL_Timers_t;

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Macros                                                  */
/* ************************************************************************** */
/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Prototypes                                                   */
/* ************************************************************************** */
/* ************************************************************************** */
/* Helper function to find the timer is started or not*/
static bool palTimerIsStarted(TimerId_t timerId);

/* Variable holding number of PAL Timers*/
static uint8_t numOfTimers = 0;

/* PAL Timer Callback Handler through which app/stack timer 
 * callbacks will be called*/
static void palTimerCallback(uintptr_t paramCb);

/* Max number of software timers*/
static uint8_t numMaxTimers = (uint8_t)SYS_TIME_MAX_TIMERS;

/* PAL timer array holding timer details*/
static PAL_Timers_t palTimers[SYS_TIME_MAX_TIMERS];

/* 
 * Function to initialize the PAL Timer array
 */
static PAL_Status_t PAL_TimerInit(void);

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Function Implementation                                                   */
/* ************************************************************************** */
/* ************************************************************************** */
/**
 * @brief Initialization of PAL
 *
 * This function initializes the PAL. .
 *
 * @return PAL_SUCCESS  if PAL initialization is successful, PAL_FAILURE otherwise
 */
PAL_Status_t PAL_Init(void)
{
    PAL_Status_t status = PAL_SUCCESS;
        
    status = PAL_TimerInit();  
   
    return status;
}

/**
 * @brief Initialization of PAL Timer array
 *
 * This function initializes the PAL Timer array. .
 *
 * @return PAL_SUCCESS  if PAL initialization is successful, PAL_FAILURE otherwise
 */
static PAL_Status_t PAL_TimerInit(void)
{
    PAL_Status_t status = PAL_SUCCESS;
    SYS_TIME_RESULT sysStatus = SYS_TIME_ERROR;
    numOfTimers = 0;
    /* Variable holding the status of Timer initialization*/
    static bool palTimerInitialized = false;
    
    for (uint8_t i = 0; i < numMaxTimers; i++)
    {
        palTimers[i].isTimerStarted = false;
        if(palTimerInitialized)
        {
            if(palTimers[i].timerId != SYS_TIME_HANDLE_INVALID)
            {
                sysStatus = SYS_TIME_TimerDestroy(palTimers[i].timerId); 
            }
        }
    }
    
    palTimerInitialized = true;
    (void)sysStatus;
    return status;
}

/**
 * @brief Start regular timer
 *
 * This function starts a regular timer and installs the corresponding
 * callback function handle the timeout event.
 *
 * @param timer_id Timer identifier
 * @param timer_count Timeout in microseconds
 * @param timeout_type @ref TIMEOUT_RELATIVE or @ref TIMEOUT_ABSOLUTE
 * @param timer_cb Callback handler invoked upon timer expiry
 * @param param_cb Argument for the callback handler
 * @param callback_type @ref CALLBACK_SINGLE or @ref CALLBACK_PERIODIC
 *
 * @return
 *          - @ref PAL_TMR_INVALID_ID  if the timer identifier is undefined,
 *          - @ref PAL_INVALID_PARAMETER if the callback function for this timer
 *                 is NULL,
 *          - @ref PAL_TMR_ALREADY_RUNNING if the timer is already running.
 *          - @ref PAL_SUCCESS if timer is started or
 *          - @ref PAL_TMR_INVALID_TIMEOUT if timeout is not within timeout
 * range.
 */
PAL_Status_t PAL_TimerStart(TimerId_t timerId,
        uint32_t timerCount,
        TimeoutType_t timeoutType,
        appCallback_t timerCb,
        void *paramCb, CallbackType_t callbackType)
{

    SYS_TIME_RESULT status = SYS_TIME_ERROR;
    SYS_TIME_HANDLE timerHandle  = SYS_TIME_HANDLE_INVALID;
    SYS_TIME_CALLBACK_TYPE type = (SYS_TIME_CALLBACK_TYPE)callbackType;

    PAL_Status_t retStatus = PAL_FAILURE;
    
    if (timeoutType == TIMEOUT_RELATIVE)
    {
        timerHandle = SYS_TIME_TimerCreate(0, SYS_TIME_USToCount(timerCount), 
            &palTimerCallback, (uintptr_t)&palTimers[timerId].timerId, type );
    }
    else
    {
        timerHandle = SYS_TIME_TimerCreate(SYS_TIME_USToCount(timerCount), 0, 
            &palTimerCallback, (uintptr_t)&palTimers[timerId].timerId, type );
    }
    
     
    if(timerHandle != SYS_TIME_HANDLE_INVALID)
    {    
        palTimers[timerId].timerId = timerHandle;
        status = SYS_TIME_TimerStart(timerHandle);
        palTimers[timerId].appCallback = timerCb;
        palTimers[timerId].param_cb = paramCb;
        
    }
    else
    {
        retStatus = PAL_FAILURE;
    }

    if (SYS_TIME_SUCCESS == status) {       
        palTimers[timerId].isTimerStarted = true; 
        retStatus = PAL_SUCCESS;
    }
    else{
        palTimers[timerId].isTimerStarted = false;
        retStatus = PAL_FAILURE;
    }
    return retStatus;
    
}

/**
 * @brief Stops a running timer
 *
 * This function stops a running timer with specified timer_id
 *
 * @param timer_id Timer identifier
 *
 * @return
 *          - @ref PAL_SUCCESS if timer stopped successfully,
 *          - @ref PAL_TMR_INVALID_ID if the specifed timer id is undefined.
 */
PAL_Status_t PAL_TimerStop(TimerId_t timerId)
{
    PAL_Status_t status = PAL_SUCCESS;
    SYS_TIME_RESULT sysStatus = SYS_TIME_ERROR;
        
    if(timerId <= numOfTimers)
    {   
        sysStatus = SYS_TIME_TimerDestroy(palTimers[timerId].timerId);
        palTimers[timerId].isTimerStarted = false;
    }
    else
    {
        status = PAL_TMR_INVALID_ID;
    }
    
    (void)sysStatus;
    return status;
}

/**
 * @brief Returns a timer id to be used before starting a timer
 * @param timer_id Value of the id returned by the function
 */
PAL_Status_t PAL_TimerGetId(TimerId_t *timerId)
{
    PAL_Status_t status = PAL_FAILURE;

    if (numOfTimers < numMaxTimers) {
        *timerId = numOfTimers;
        numOfTimers++;
        status = PAL_SUCCESS;
    
    } 
    return status;
}

/**
 * @brief Checks if the timer of requested timer identifier is running
 *
 * This function checks if the timer of requested timer identifier is running.
 *
 * @param timer_id Timer identifier
 *
 * @return
 * - true if timer with requested timer id is running,
 * - false otherwise.
 */
bool PAL_TimerIsRunning(TimerId_t timerId)
{
    bool isRunning = false;
      
    if(palTimerIsStarted(timerId))
    {
        isRunning = !SYS_TIME_TimerPeriodHasExpired(palTimers[timerId].timerId);
    }
         
    return isRunning;
}

/**
 * @brief Generates blocking delay
 *
 * This functions generates a blocking delay of specified time.
 *
 * @param delay in microseconds
 */
void PAL_TimerDelay(uint32_t delay)    
{ 
    SYS_TIME_HANDLE delay_timer = SYS_TIME_HANDLE_INVALID;
    SYS_TIME_RESULT sysStatus = SYS_TIME_ERROR;
     
    sysStatus = SYS_TIME_DelayUS(delay, &delay_timer);
    if(delay_timer != SYS_TIME_HANDLE_INVALID && (sysStatus != SYS_TIME_ERROR))
    {
        while (SYS_TIME_DelayIsComplete(delay_timer) == false)
        {
           ;
        }
    }
    
    return;
}

/**
 * @brief Gets current time
 *
 * This function returns the current time.
 *
 * @param[out] current_time Returns current system time
 */
uint32_t PAL_GetCurrentTime(void)
{
    uint32_t countVal; 

    /* This will avoid the hard faults, due to aligned nature of access */
    countVal = SYS_TIME_CounterGet();
    
    return SYS_TIME_CountToUS(countVal);
}

/* 
 * @brief Helper function to find the timer is started or not
 * @param timer_id Identifier of the timer which is being started/not
 */
static bool palTimerIsStarted(TimerId_t timerId)
{
    bool isTimerStarted = false;
    
    isTimerStarted = palTimers[timerId].isTimerStarted;
   
    return isTimerStarted;
}

/* 
 * @brief PAL Timer Callback Handler through which app/stack timer 
 *        callbacks will be called
 * @param param_cb - Callback parameter holding the timer_id of expired timer
 */
static void palTimerCallback(uintptr_t paramCb)
{
    SYS_TIME_HANDLE *timerId = (SYS_TIME_HANDLE *)paramCb;
    appCallback_t appCallback;
    
    if(timerId != NULL)
    {
        for (uint8_t i =0; i < numOfTimers; i++)
        {
            if((palTimers[i].timerId == (*timerId)) && (palTimers[i].isTimerStarted))
            {
                palTimers[i].isTimerStarted = false;
                appCallback = palTimers[i].appCallback;
                if(appCallback != NULL)
                {
                    appCallback(palTimers[i].param_cb);
                }
            }
        }
    }
}

/**
 * @brief Gets Random number
 * @param[rnOutput] random number @param[rnLength] size of Random number
 */

PAL_Status_t PAL_GetRandomNumber(uint8_t *rnOutput, uint16_t rnLength)
{
    uint32_t random_num;
    uint32_t remBytes;
    uint8_t *end = rnOutput;
    end += rnLength;
    for (uint16_t i = 0; i < (rnLength / sizeof(uint32_t)); i++)
    {
        random_num = TRNG_ReadData();
        (void)memcpy((uint8_t *)rnOutput, (uint8_t *)&random_num, sizeof(uint32_t));
        rnOutput += sizeof(uint32_t);
    }
    
    if ((remBytes = (rnLength % sizeof(uint32_t))) != 0U){
        random_num = TRNG_ReadData();
        (void)memcpy((uint8_t *)rnOutput, (uint8_t *)&random_num, remBytes);
    }

    return PAL_SUCCESS;
}

/**
 * @brief Gets the Antenna gain from the information block of device support library
 * @param[antGain] - Pointer holding the module's antenna gain value
 */
PAL_Status_t PAL_GetTrxAntennaGain(int8_t *antGain)
{
    bool valid = false;
    int8_t antennaGain = INT8_MAX;
    
#ifndef CUSTOM_ANT_GAIN   
    valid = IB_GetAntennaGain(&antennaGain);
    
    if(valid)
    {
        //If infoBlock having valid antennaGain value
        *antGain = antennaGain;
    }
    else
    {
        //If InfoBlock is not having valid antennaGain value, 
        //return default gain of WBZ45x
        *antGain = DEFAULT_ANT_GAIN;
    }
#else
    *antGain = CUSTOM_ANT_GAIN;
    
#endif
    
    return PAL_SUCCESS;    
}

/**
 * @brief Gets the Transceiver's maximum transmit power
 * @param[out] Integer value returning maximum tx power value that can be 
 *             set by higher  layer in dBm
 */
int8_t PAL_GetTrxTransmitPowerMax(void)
{
    int8_t txPwrMax = INT8_MAX;
    
    txPwrMax = PHY_TX_PWR_MAX;
    
    return txPwrMax;
}

/**
 * @brief Gets device type
  * @param[out] retrun boolean value for device type
 */

bool PAL_GetDeviceType(void)
{
    bool isHpaEnabled = false;
    
    return isHpaEnabled;

}

/**
 * @brief Sets CPS pin for WBZ451 HPA device
 * @param[bypass] holding CPS need to set or not
 */

void PAL_HpaSetCps(bool bypass)
{
    (void)bypass;
}