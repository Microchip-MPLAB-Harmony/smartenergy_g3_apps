/*
Copyright (C) 2024, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
    app_display.c

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

#include "app_display.h"
#include "definitions.h"

// *****************************************************************************
// *****************************************************************************
// Section: Macro Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

/* Define a semaphore to signal the Display Tasks to handle information */
OSAL_SEM_DECLARE(appDisplaySemID);

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_DISPLAY_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_DISPLAY_DATA CACHE_ALIGN app_displayData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************
static void APP_DISPLAY_ScrollUp_Callback ( PIO_PIN pin, uintptr_t context)
{
    app_displayData.scrup_pressed = true;
    OSAL_SEM_PostISR(&appDisplaySemID);
}

static void APP_DISPLAY_ScrollDown_Callback ( PIO_PIN pin, uintptr_t context)
{
    app_displayData.scrdown_pressed = true;
    OSAL_SEM_PostISR(&appDisplaySemID);
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    Init LCD module.
 */

static status_code_t APP_DISPLAY_InitLCD(void)
{
    status_code_t status;

    /* Initialize the CL010 LCD glass component */
    status = cl010_init();
    if (status != STATUS_OK)
    {
        return STATUS_ERR_BUSY;
    }

    /* Show all symbols */
    cl010_show_all();

    return STATUS_OK;
}

/*******************************************************************************
  Function:
    Update the status of the external communication signal.
 */
static void _APP_DISPLAY_UpdateComSignal (APP_DISPLAY_COM_SIGNAL signal)
{
    if (signal == APP_DISPLAY_COM_SIGNAL_OFF)
    {
        cl010_clear_icon(CL010_ICON_COMM_SIGNAL_LOW);
        cl010_clear_icon(CL010_ICON_COMM_SIGNAL_MED);
        cl010_clear_icon(CL010_ICON_COMM_SIGNAL_HIG);
    }
    else
    {
        if (signal >= APP_DISPLAY_COM_SIGNAL_LOW)
        {
            cl010_show_icon(CL010_ICON_COMM_SIGNAL_LOW);
        }

        if (signal >= APP_DISPLAY_COM_SIGNAL_MED)
        {
            cl010_show_icon(CL010_ICON_COMM_SIGNAL_MED);
        }

        if (signal == APP_DISPLAY_COM_SIGNAL_HIGH)
        {
            cl010_show_icon(CL010_ICON_COMM_SIGNAL_HIG);
        }
    }
}

/*******************************************************************************
  Function:
    Timer in seconds for changing the info in loop mode.
 */

static void APP_DISPLAY_SetTimerLoop(uint32_t time_sec)
{
    if (time_sec)
    {
        /* Set the new time for loop mode */
        app_displayData.reload_display_time = time_sec;
    }
}

/*******************************************************************************
  Function:
    Add display info to the display loop.
 */

static void APP_DISPLAY_AddLoopInfo(APP_DISPLAY_INFO info)
{
    if ((info < APP_DISPLAY_MAX_TYPE) && (app_displayData.loop_max < APP_DISPLAY_MAX_TYPE))
    {
        /* Add info to display */
        app_displayData.loop_info[app_displayData.loop_max++] = info;
    }
}

/*******************************************************************************
  Function:
    Change the information in the display.
 */

static void APP_DISPLAY_ChangeInfo(void)
{
    if (app_displayData.loop_max <= APP_DISPLAY_MAX_TYPE)
    {
        if ((app_displayData.direction & APP_DISPLAY_FORWARD) == APP_DISPLAY_FORWARD)
        {
            if (++app_displayData.loop_idx >= app_displayData.loop_max)
            {
                app_displayData.loop_idx = 0;
            }
        }
        else if ((app_displayData.direction & APP_DISPLAY_BACKWARD) == APP_DISPLAY_BACKWARD)
        {
            if (--app_displayData.loop_idx >= app_displayData.loop_max)
            {
                app_displayData.loop_idx = app_displayData.loop_max - 1;
            }
        }

        app_displayData.display_info = app_displayData.loop_info[app_displayData.loop_idx];
    }
}

/*******************************************************************************
  Function:
    Show Energy data and units depending on its value
 */

static void APP_DISPLAY_ShowEnergyDataUnits(uint64_t value)
{
    uint8_t buff1[9];

    /* Check magnitude to select units to show */
    if (value > 999999999) {
        /* Format: xxxxxx.xx kWh */
        cl010_show_units(CL010_UNIT_kWh);
        value = value/100000;
        sprintf((char *)buff1, "%6u%02u", (unsigned int)(value/100),
                 (unsigned int)(value%100));
        cl010_show_icon(CL010_ICON_DOT_2);
    }
    else
    {
        /* Format: xxxxx.xxx Wh */
        cl010_show_units(CL010_UNIT_Wh);
        value = value/10;
        sprintf((char *)buff1, "%5u%03u", (unsigned int)(value/1000),
                 (unsigned int)(value%1000));
        cl010_show_icon(CL010_ICON_DOT_1);
    }

    cl010_show_numeric_string(CL010_LINE_UP, buff1);
}

/*******************************************************************************
  Function:
    Process information for the display.
 */

static void APP_DISPLAY_Process(void)
{
    uint64_t total;
    uint64_t upd_symbols = 1;
    uint32_t rmsValue;
    uint8_t buff1[9];
    uint8_t idx;
    struct tm current_time;
    APP_ENERGY_ACCUMULATORS EnergyAcc;
    APP_ENERGY_MAX_DEMAND MaxDemand;

    if (app_displayData.display_info != 0xFF)
    {
        cl010_clear_all();
    }

    /* Init internal buffer */
    memset(buff1, 0, sizeof(buff1));

    switch (app_displayData.display_info)
    {
        case APP_DISPLAY_TOTAL_ENERGY:
        {
            APP_ENERGY_GetCurrentEnergy(&EnergyAcc);
            total = 0;
            for (idx = 0; idx < TARIFF_NUM_TYPE; idx++)
            {
                total += EnergyAcc.tariff[idx];
            }
            APP_DISPLAY_ShowEnergyDataUnits(total);
            cl010_show_icon(CL010_ICON_CUM);
            sprintf((char *)buff1, "   1234");
            cl010_show_numeric_string(CL010_LINE_DOWN, buff1);

            break;
        }

        case APP_DISPLAY_TOU1_ENERGY:
        {
            APP_ENERGY_GetCurrentEnergy(&EnergyAcc);
            total = EnergyAcc.tariff[0];
            APP_DISPLAY_ShowEnergyDataUnits(total);
            cl010_show_icon(CL010_ICON_CUM);
            sprintf((char *)buff1, "      1");
            cl010_show_numeric_string(CL010_LINE_DOWN, buff1);

            break;
        }

        case APP_DISPLAY_TOU2_ENERGY:
        {
            APP_ENERGY_GetCurrentEnergy(&EnergyAcc);
            total = EnergyAcc.tariff[1];
            APP_DISPLAY_ShowEnergyDataUnits(total);
            cl010_show_icon(CL010_ICON_CUM);
            sprintf((char *)buff1, "      2");
            cl010_show_numeric_string(CL010_LINE_DOWN, buff1);

            break;
        }

        case APP_DISPLAY_TOU3_ENERGY:
        {
            APP_ENERGY_GetCurrentEnergy(&EnergyAcc);
            total = EnergyAcc.tariff[2];
            APP_DISPLAY_ShowEnergyDataUnits(total);
            cl010_show_icon(CL010_ICON_CUM);
            sprintf((char *)buff1, "      3");
            cl010_show_numeric_string(CL010_LINE_DOWN, buff1);

            break;
        }

        case APP_DISPLAY_TOU4_ENERGY:
        {
            APP_ENERGY_GetCurrentEnergy(&EnergyAcc);
            total = EnergyAcc.tariff[3];
            APP_DISPLAY_ShowEnergyDataUnits(total);
            cl010_show_icon(CL010_ICON_CUM);
            sprintf((char *)buff1, "      4");
            cl010_show_numeric_string(CL010_LINE_DOWN, buff1);

            break;
        }

        case APP_DISPLAY_VA_RMS:
        {
            APP_METROLOGY_GetRMS(RMS_UA, &rmsValue, NULL);
            sprintf((char *)buff1, "%5u%03u",
                    (unsigned int)(rmsValue/10000),
                    (unsigned int)((rmsValue%10000)/10));
            cl010_show_numeric_string(CL010_LINE_UP, buff1);
            cl010_show_units(CL010_UNIT_V);
            cl010_show_icon(CL010_ICON_DOT_1);
            cl010_show_icon(CL010_ICON_L1);

            break;
        }

        case APP_DISPLAY_VB_RMS:
        {
            APP_METROLOGY_GetRMS(RMS_UB, &rmsValue, NULL);
            sprintf((char *)buff1, "%5u%03u",
                    (unsigned int)(rmsValue/10000),
                    (unsigned int)((rmsValue%10000)/10));
            cl010_show_numeric_string(CL010_LINE_UP, buff1);
            cl010_show_units(CL010_UNIT_V);
            cl010_show_icon(CL010_ICON_DOT_1);
            cl010_show_icon(CL010_ICON_L2);

            break;
        }

        case APP_DISPLAY_VC_RMS:
        {
            APP_METROLOGY_GetRMS(RMS_UC, &rmsValue, NULL);
            sprintf((char *)buff1, "%5u%03u",
                    (unsigned int)(rmsValue/10000),
                    (unsigned int)((rmsValue%10000)/10));
            cl010_show_numeric_string(CL010_LINE_UP, buff1);
            cl010_show_units(CL010_UNIT_V);
            cl010_show_icon(CL010_ICON_DOT_1);
            cl010_show_icon(CL010_ICON_L3);

            break;
        }

        case APP_DISPLAY_IA_RMS:
        {
            APP_METROLOGY_GetRMS(RMS_IA, &rmsValue, NULL);
            sprintf((char *)buff1, "%5u%03u",
                    (unsigned int)(rmsValue/10000),
                    (unsigned int)((rmsValue%10000)/10));
            cl010_show_numeric_string(CL010_LINE_UP, buff1);
            cl010_show_units(CL010_UNIT_A);
            cl010_show_icon(CL010_ICON_DOT_1);
            cl010_show_icon(CL010_ICON_L1);

            break;
        }

        case APP_DISPLAY_IB_RMS:
        {
            APP_METROLOGY_GetRMS(RMS_IB, &rmsValue, NULL);
            sprintf((char *)buff1, "%5u%03u",
                    (unsigned int)(rmsValue/10000),
                    (unsigned int)((rmsValue%10000)/10));
            cl010_show_numeric_string(CL010_LINE_UP, buff1);
            cl010_show_units(CL010_UNIT_A);
            cl010_show_icon(CL010_ICON_DOT_1);
            cl010_show_icon(CL010_ICON_L2);

            break;
        }

        case APP_DISPLAY_IC_RMS:
        {
            APP_METROLOGY_GetRMS(RMS_IC, &rmsValue, NULL);
            sprintf((char *)buff1, "%5u%03u",
                    (unsigned int)(rmsValue/10000),
                    (unsigned int)((rmsValue%10000)/10));
            cl010_show_numeric_string(CL010_LINE_UP, buff1);
            cl010_show_units(CL010_UNIT_A);
            cl010_show_icon(CL010_ICON_DOT_1);
            cl010_show_icon(CL010_ICON_L3);

            break;
        }

        case APP_DISPLAY_RTC_TIME:
        {
            RTC_TimeGet(&current_time);
            sprintf((char *)buff1, "%02d%02d%02d  ",
                    (uint8_t)current_time.tm_hour,
                    (uint8_t)current_time.tm_min,
                    (uint8_t)current_time.tm_sec);
            cl010_show_numeric_string(CL010_LINE_UP, buff1);
            cl010_show_icon(CL010_ICON_TIME);
            cl010_show_icon(CL010_ICON_COL_1);
            cl010_show_icon(CL010_ICON_COL_2);
            upd_symbols = 0;

            break;
        }

        case APP_DISPLAY_RTC_DATE:
        {
            RTC_TimeGet(&current_time);
            sprintf((char *)buff1, "%02d%02d%02d  ",
                    (uint8_t)(current_time.tm_year - 100),
                    (uint8_t)(current_time.tm_mon + 1),
                    (uint8_t)(current_time.tm_mday));
            cl010_show_numeric_string(CL010_LINE_UP, buff1);
            cl010_show_icon(CL010_ICON_DATE);
            cl010_show_icon(CL010_ICON_COL_1);
            cl010_show_icon(CL010_ICON_COL_2);
            upd_symbols = 0;

            break;
        }

        case APP_DISPLAY_TOTAL_MAX_DEMAND:
        {
            APP_ENERGY_GetCurrentMaxDemand(&MaxDemand);
            sprintf((char *)buff1, "%8u", (unsigned int)MaxDemand.maxDemad.value);
            cl010_show_numeric_string(CL010_LINE_UP, buff1);
            cl010_show_units(CL010_UNIT_W);
            cl010_show_icon(CL010_ICON_MD);
            sprintf((char *)buff1, "   1234");
            cl010_show_numeric_string(CL010_LINE_DOWN, buff1);

            break;
        }

        case APP_DISPLAY_TOU1_MAX_DEMAND:
        {
            APP_ENERGY_GetCurrentMaxDemand(&MaxDemand);
            sprintf((char *)buff1, "%8u", (unsigned int)MaxDemand.tariff[0].value);
            cl010_show_numeric_string(CL010_LINE_UP, buff1);
            cl010_show_units(CL010_UNIT_W);
            cl010_show_icon(CL010_ICON_MD);
            sprintf((char *)buff1, "     01");
            cl010_show_numeric_string(CL010_LINE_DOWN, buff1);

            break;
        }

        case APP_DISPLAY_TOU2_MAX_DEMAND:
        {
            APP_ENERGY_GetCurrentMaxDemand(&MaxDemand);
            sprintf((char *)buff1, "%8u", (unsigned int)MaxDemand.tariff[1].value);
            cl010_show_numeric_string(CL010_LINE_UP, buff1);
            cl010_show_units(CL010_UNIT_W);
            cl010_show_icon(CL010_ICON_MD);
            sprintf((char *)buff1, "     02");
            cl010_show_numeric_string(CL010_LINE_DOWN, buff1);

            break;
        }

        case APP_DISPLAY_TOU3_MAX_DEMAND:
        {
            APP_ENERGY_GetCurrentMaxDemand(&MaxDemand);
            sprintf((char *)buff1, "%8u", (unsigned int)MaxDemand.tariff[2].value);
            cl010_show_numeric_string(CL010_LINE_UP, buff1);
            cl010_show_units(CL010_UNIT_W);
            cl010_show_icon(CL010_ICON_MD);
            sprintf((char *)buff1, "     03");
            cl010_show_numeric_string(CL010_LINE_DOWN, buff1);

            break;
        }

        case APP_DISPLAY_TOU4_MAX_DEMAND:
        {
            APP_ENERGY_GetCurrentMaxDemand(&MaxDemand);
            sprintf((char *)buff1, "%8u", (unsigned int)MaxDemand.tariff[3].value);
            cl010_show_numeric_string(CL010_LINE_UP, buff1);
            cl010_show_units(CL010_UNIT_W);
            cl010_show_icon(CL010_ICON_MD);
            sprintf((char *)buff1, "     04");
            cl010_show_numeric_string(CL010_LINE_DOWN, buff1);

            break;
        }

        case APP_DISPLAY_APP_INFO:
        {
            cl010_show_numeric_string(CL010_LINE_UP, app_displayData.app_info);
            upd_symbols = 0;

            break;
        }

        case APP_DISPLAY_BOARD_ID:
        {
            sprintf((char *)buff1, "%08x", APP_DISPLAY_BOARD_VERSION);
            cl010_show_numeric_string(CL010_LINE_UP, buff1);
            upd_symbols = 0;

            break;
        }

        case APP_DISPLAY_DEMO_VERSION:
        {
            sprintf((char *)buff1, "%d", DEMO_APP_VERSION);
            cl010_show_numeric_string(CL010_LINE_UP, buff1);
            upd_symbols = 0;

            break;
        }

        default:
            break;
    }

    if (upd_symbols)
    {
        APP_EVENTS_FLAGS eventFlags;
        APP_EVENTS_GetLastEventFlags(&eventFlags);

        if (APP_METROLOGY_CheckPhaseEnabled(APP_METROLOGY_PHASE_A) && (eventFlags.sagA))
        {
            /* "A alarm" phase */
            cl010_show_icon(CL010_ICON_PHASE_1);
        }

        if (APP_METROLOGY_CheckPhaseEnabled(APP_METROLOGY_PHASE_B) && (eventFlags.sagB))
        {
            /* "A alarm" phase */
            cl010_show_icon(CL010_ICON_PHASE_2);
        }

        if (APP_METROLOGY_CheckPhaseEnabled(APP_METROLOGY_PHASE_C) && (eventFlags.sagC))
        {
            /* "A alarm" phase */
            cl010_show_icon(CL010_ICON_PHASE_3);
        }

        if (eventFlags.ptDir)
        {
            /* active power is reverse */
            cl010_show_icon(CL010_ICON_P_MINUS);
        }
        else
        {
            /* active power is forward */
            cl010_show_icon(CL010_ICON_P_PLUS);
        }

        if (eventFlags.qtDir)
        {
            /* reactive power is reverse */
            cl010_show_icon(CL010_ICON_Q_MINUS);
        }
        else
        {
            /* reactive power is forward */
            cl010_show_icon(CL010_ICON_Q_PLUS);
        }

    }

    /* Display MCHP logo */
    cl010_show_icon(CL010_ICON_MICROCHIP);

    /* Update External Communication signal status */
    if (app_displayData.comm_signal > APP_DISPLAY_COM_SIGNAL_OFF)
    {
        _APP_DISPLAY_UpdateComSignal(app_displayData.comm_signal);
    }

}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_DISPLAY_Initialize ( void )

  Remarks:
    See prototype in app_display.h.
 */

void APP_DISPLAY_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    app_displayData.state = APP_DISPLAY_STATE_INIT;

    /* No buttons pressed */
    app_displayData.scrdown_pressed = false;
    app_displayData.scrup_pressed = false;

    /* Init LCD */
    APP_DISPLAY_InitLCD();

    /* Set startup display information */
    app_displayData.loop_max = 0;
    app_displayData.loop_idx = 0;

    /* Clear flag to disable application message */
    app_displayData.app_info_en = false;
    memset(app_displayData.app_info, 0, 8);

    /* Set display loop direction by default */
    app_displayData.direction = APP_DISPLAY_FORWARD;

    /* Init display info */
    app_displayData.display_info = (APP_DISPLAY_INFO)0xFF;

    /* Disable external communication signal */
    app_displayData.comm_signal = APP_DISPLAY_COM_SIGNAL_OFF;

    /* Set display time */
    app_displayData.display_time = 2;

    /* Configure display timer loop */
    APP_DISPLAY_SetTimerLoop(3);

    /* Configure Switches */
    PIO_PinInterruptCallbackRegister(SWITCH_SCRUP_PIN,
            APP_DISPLAY_ScrollUp_Callback, (uintptr_t)NULL);

    PIO_PinInterruptCallbackRegister(SWITCH_SCRDOWN_PIN,
            APP_DISPLAY_ScrollDown_Callback, (uintptr_t)NULL);

    /* Create the Display Semaphore */
    if (OSAL_SEM_Create(&appDisplaySemID, OSAL_SEM_TYPE_BINARY, 0, 0) == OSAL_RESULT_FALSE)
    {
        /* Handle error condition. Not sufficient memory to create semaphore */
    }

    /* Reload DWDT0 at startup */
    DWDT_WDT0_Clear();
    app_displayData.reloadDWDT0 = true;
}

/******************************************************************************
  Function:
    void APP_DISPLAY_Tasks ( void )

  Remarks:
    See prototype in app_display.h.
 */

void APP_DISPLAY_Tasks ( void )
{
    if (app_displayData.reloadDWDT0)
    {
        DWDT_WDT0_Clear();
    }

    switch ( app_displayData.state )
    {
        case APP_DISPLAY_STATE_INIT:
        {
            if (APP_METROLOGY_GetState() == APP_METROLOGY_STATE_RUNNING)
            {
                /* Configure display measurements */
                APP_DISPLAY_AddLoopInfo(APP_DISPLAY_BOARD_ID);
                APP_DISPLAY_AddLoopInfo(APP_DISPLAY_DEMO_VERSION);
                APP_DISPLAY_AddLoopInfo(APP_DISPLAY_TOTAL_ENERGY);
                APP_DISPLAY_AddLoopInfo(APP_DISPLAY_TOU1_ENERGY);
                APP_DISPLAY_AddLoopInfo(APP_DISPLAY_TOU2_ENERGY);
                APP_DISPLAY_AddLoopInfo(APP_DISPLAY_TOU3_ENERGY);
                APP_DISPLAY_AddLoopInfo(APP_DISPLAY_TOU4_ENERGY);
                APP_DISPLAY_AddLoopInfo(APP_DISPLAY_RTC_TIME);
                APP_DISPLAY_AddLoopInfo(APP_DISPLAY_RTC_DATE);

                if (APP_METROLOGY_CheckPhaseEnabled(APP_METROLOGY_PHASE_A))
                {
                    APP_DISPLAY_AddLoopInfo(APP_DISPLAY_VA_RMS);
                }

                if (APP_METROLOGY_CheckPhaseEnabled(APP_METROLOGY_PHASE_B))
                {
                    APP_DISPLAY_AddLoopInfo(APP_DISPLAY_VB_RMS);
                }

                if (APP_METROLOGY_CheckPhaseEnabled(APP_METROLOGY_PHASE_C))
                {
                    APP_DISPLAY_AddLoopInfo(APP_DISPLAY_VC_RMS);
                }

                if (APP_METROLOGY_CheckPhaseEnabled(APP_METROLOGY_PHASE_A))
                {
                    APP_DISPLAY_AddLoopInfo(APP_DISPLAY_IA_RMS);
                }

                if (APP_METROLOGY_CheckPhaseEnabled(APP_METROLOGY_PHASE_B))
                {
                    APP_DISPLAY_AddLoopInfo(APP_DISPLAY_IB_RMS);
                }

                if (APP_METROLOGY_CheckPhaseEnabled(APP_METROLOGY_PHASE_C))
                {
                    APP_DISPLAY_AddLoopInfo(APP_DISPLAY_IC_RMS);
                }

                APP_DISPLAY_AddLoopInfo(APP_DISPLAY_TOTAL_MAX_DEMAND);
                APP_DISPLAY_AddLoopInfo(APP_DISPLAY_TOU1_MAX_DEMAND);
                APP_DISPLAY_AddLoopInfo(APP_DISPLAY_TOU2_MAX_DEMAND);
                APP_DISPLAY_AddLoopInfo(APP_DISPLAY_TOU3_MAX_DEMAND);
                APP_DISPLAY_AddLoopInfo(APP_DISPLAY_TOU4_MAX_DEMAND);

                /* Enable Switches interrupts */
                PIO_PinInterruptEnable(SWITCH_SCRUP_PIN);
                PIO_PinInterruptEnable(SWITCH_SCRDOWN_PIN);

                app_displayData.state = APP_DISPLAY_STATE_SERVICE_TASKS;
            }

            vTaskDelay(100 / portTICK_PERIOD_MS);
            break;
        }

        case APP_DISPLAY_STATE_SERVICE_TASKS:
        {
            /* Check if buttons were pressed or wait 1s */
            if (OSAL_SEM_Pend(&appDisplaySemID, 1000) == OSAL_RESULT_TRUE)
            {
                /* If any button has been pressed, change the information */
                if (app_displayData.scrdown_pressed)
                {
                    if (SWITCH_SCRUP_Get() == 0)
                    {
                        SYS_CMD_MESSAGE("Entering Low Power... Press FWUP/TAMPER switch to wake up.\r\n");

                        // Update display info
                        APP_DISPLAY_ShowLowPowerMode();

                        // Wait time to show message through the Console
                        vTaskDelay(100 / portTICK_PERIOD_MS);

                        // Go to Low Power mode
                        APP_METROLOGY_SetLowPowerMode();

                        // Execution should not come here during normal operation
                    }

                    app_displayData.scrdown_pressed = false;
                    app_displayData.direction = APP_DISPLAY_BACKWARD;
                }

                if (app_displayData.scrup_pressed)
                {
                    if (SWITCH_SCRDOWN_Get() == 0)
                    {
                        SYS_CMD_MESSAGE("Emulating application holds ... Resetting by DWDT0.\r\n");

                        // Forcing no reload DWDT0
                        app_displayData.reloadDWDT0 = false;
                    }

                    app_displayData.scrup_pressed = false;
                    app_displayData.direction = APP_DISPLAY_FORWARD;
                }

                APP_DISPLAY_ChangeInfo();
                APP_DISPLAY_Process();

                /* Reload time */
                app_displayData.display_time = app_displayData.reload_display_time;
            }
            else
            {
                /* Show information in display */
                if (app_displayData.display_time != 0)
                {
                    --app_displayData.display_time;
                }
                else
                {
                    APP_DISPLAY_ChangeInfo();
                    APP_DISPLAY_Process();

                    /* Reload time */
                    app_displayData.display_time = app_displayData.reload_display_time;
                }
            }

            break;
        }

        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}

/******************************************************************************
  Function:
    void APP_DISPLAY_SetAppInfo(const char *msg, uint8_t len)

  Remarks:
    See prototype in app_display.h.
 */

void APP_DISPLAY_SetAppInfo(const char *msg, uint8_t len)
{
    if (len > sizeof(app_displayData.app_info))
    {
        len = sizeof(app_displayData.app_info);
    }

    memcpy(app_displayData.app_info, msg, len);

    if (!app_displayData.app_info_en)
    {
        /* Set enable flag */
        app_displayData.app_info_en = true;
        /* Add APP display info */
        APP_DISPLAY_AddLoopInfo(APP_DISPLAY_APP_INFO);
    }

    /* Change display information and reload time */
    app_displayData.display_time = app_displayData.reload_display_time;
    app_displayData.display_info = APP_DISPLAY_APP_INFO;
}


/******************************************************************************
  Function:
    void APP_DISPLAY_SetSerialCommIcon(void)

  Remarks:
    See prototype in app_display.h.
 */

void APP_DISPLAY_SetSerialCommunication(void)
{
    cl010_show_icon(CL010_ICON_ARROW_LEFT);
}

/******************************************************************************
  Function:
    void APP_DISPLAY_SetCommnicationSignal(APP_DISPLAY_COM_SIGNAL signal)

  Remarks:
    See prototype in app_display.h.
 */

void APP_DISPLAY_SetCommnicationSignal(APP_DISPLAY_COM_SIGNAL signal)
{
    app_displayData.comm_signal = signal;

    _APP_DISPLAY_UpdateComSignal(signal);
}

/******************************************************************************
  Function:
    void APP_DISPLAY_ShowLowPowerMode(void)

  Remarks:
    See prototype in app_display.h.
 */

void APP_DISPLAY_ShowLowPowerMode(void)
{
    cl010_clear_all();
    cl010_show_icon(CL010_ICON_BATTERY_LOW);
}

/*******************************************************************************
 End of File
 */
