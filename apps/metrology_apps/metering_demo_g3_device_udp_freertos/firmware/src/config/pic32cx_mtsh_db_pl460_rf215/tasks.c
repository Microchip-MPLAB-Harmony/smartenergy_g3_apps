/*******************************************************************************
 System Tasks File

  File Name:
    tasks.c

  Summary:
    This file contains source code necessary to maintain system's polled tasks.

  Description:
    This file contains source code necessary to maintain system's polled tasks.
    It implements the "SYS_Tasks" function that calls the individual "Tasks"
    functions for all polled MPLAB Harmony modules in the system.

  Remarks:
    This file requires access to the systemObjects global data structure that
    contains the object handles to all MPLAB Harmony module objects executing
    polled in the system.  These handles are passed into the individual module
    "Tasks" functions to identify the instance of the module to maintain.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
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

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "configuration.h"
#include "definitions.h"
#include "sys_tasks.h"


// *****************************************************************************
// *****************************************************************************
// Section: RTOS "Tasks" Routine
// *****************************************************************************
// *****************************************************************************
static void lDRV_G3_MACRT_Tasks(  void *pvParameters  )
{
    while(true)
    {
        /* Maintain G3 MAC RT Driver */
        DRV_G3_MACRT_Tasks(sysObj.drvG3MacRt);
    }
}

static void lDRV_RF215_Tasks(  void *pvParameters  )
{
    while(true)
    {
        /* Maintain RF215 Driver */
        DRV_RF215_Tasks(sysObj.drvRf215);
    }
}

static void lG3_STACK_Tasks(  void *pvParameters  )
{
    while(true)
    {
        /* Maintain G3 MAC */
        MAC_WRP_Tasks(sysObj.g3MacWrapper);

        /* Maintain G3 ADP */
        ADP_Tasks(sysObj.g3Adp);

        vTaskDelay(G3_STACK_RTOS_TASK_DELAY_MS / portTICK_PERIOD_MS);
    }
}

static void lDRV_MEMORY_0_Tasks(  void *pvParameters  )
{
    while(true)
    {
        DRV_MEMORY_Tasks(sysObj.drvMemory0);
        vTaskDelay(DRV_MEMORY_RTOS_DELAY_IDX0 / portTICK_PERIOD_MS);
    }
}

static void lDRV_METROLOGY_Tasks(  void *pvParameters  )
{
    while(true)
    {
        DRV_METROLOGY_Tasks(sysObj.drvMet);
    }
}

/* Handle for the APP_METROLOGY_Tasks. */
TaskHandle_t xAPP_METROLOGY_Tasks;

static void lAPP_METROLOGY_Tasks(  void *pvParameters  )
{   
    while(true)
    {
        APP_METROLOGY_Tasks();
    }
}
/* Handle for the APP_CONSOLE_Tasks. */
TaskHandle_t xAPP_CONSOLE_Tasks;

static void lAPP_CONSOLE_Tasks(  void *pvParameters  )
{   
    while(true)
    {
        APP_CONSOLE_Tasks();
    }
}
/* Handle for the APP_DATALOG_Tasks. */
TaskHandle_t xAPP_DATALOG_Tasks;

static void lAPP_DATALOG_Tasks(  void *pvParameters  )
{   
    while(true)
    {
        APP_DATALOG_Tasks();
        vTaskDelay(100U / portTICK_PERIOD_MS);
    }
}
/* Handle for the APP_DISPLAY_Tasks. */
TaskHandle_t xAPP_DISPLAY_Tasks;

static void lAPP_DISPLAY_Tasks(  void *pvParameters  )
{   
    while(true)
    {
        APP_DISPLAY_Tasks();
    }
}
/* Handle for the APP_ENERGY_Tasks. */
TaskHandle_t xAPP_ENERGY_Tasks;

static void lAPP_ENERGY_Tasks(  void *pvParameters  )
{   
    while(true)
    {
        APP_ENERGY_Tasks();
    }
}
/* Handle for the APP_EVENTS_Tasks. */
TaskHandle_t xAPP_EVENTS_Tasks;

static void lAPP_EVENTS_Tasks(  void *pvParameters  )
{   
    while(true)
    {
        APP_EVENTS_Tasks();
    }
}
/* Handle for the APP_G3_MANAGEMENT_Tasks. */
TaskHandle_t xAPP_G3_MANAGEMENT_Tasks;

static void lAPP_G3_MANAGEMENT_Tasks(  void *pvParameters  )
{   
    while(true)
    {
        APP_G3_MANAGEMENT_Tasks();
    }
}
/* Handle for the APP_UDP_RESPONDER_Tasks. */
TaskHandle_t xAPP_UDP_RESPONDER_Tasks;

static void lAPP_UDP_RESPONDER_Tasks(  void *pvParameters  )
{   
    while(true)
    {
        APP_UDP_RESPONDER_Tasks();
        vTaskDelay(5U / portTICK_PERIOD_MS);
    }
}
/* Handle for the APP_STORAGE_PIC32CXMT_Tasks. */
TaskHandle_t xAPP_STORAGE_PIC32CXMT_Tasks;

static void lAPP_STORAGE_PIC32CXMT_Tasks(  void *pvParameters  )
{   
    while(true)
    {
        APP_STORAGE_PIC32CXMT_Tasks();
    }
}
/* Handle for the APP_UDP_METROLOGY_Tasks. */
TaskHandle_t xAPP_UDP_METROLOGY_Tasks;

static void lAPP_UDP_METROLOGY_Tasks(  void *pvParameters  )
{   
    while(true)
    {
        APP_UDP_METROLOGY_Tasks();
        vTaskDelay(5U / portTICK_PERIOD_MS);
    }
}


void _TCPIP_STACK_Task(  void *pvParameters  )
{
    while(1)
    {
        TCPIP_STACK_Task(sysObj.tcpip);
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

TaskHandle_t xSYS_CMD_Tasks;
void lSYS_CMD_Tasks(  void *pvParameters  )
{
    while(1)
    {
        SYS_CMD_Tasks();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}



static void lSYS_FS_Tasks(  void *pvParameters  )
{
    while(true)
    {
        SYS_FS_Tasks();
        vTaskDelay(10U / portTICK_PERIOD_MS);
    }
}


void _SLCDC_Tasks(  void *pvParameters  )
{
    while(1)
    {
        DRV_SLCDC_Update();
        vTaskDelay(60000 / portTICK_PERIOD_MS);
    }
}




// *****************************************************************************
// *****************************************************************************
// Section: System "Tasks" Routine
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void SYS_Tasks ( void )

  Remarks:
    See prototype in system/common/sys_module.h.
*/
void SYS_Tasks ( void )
{
    /* Maintain system services */
    

    (void) xTaskCreate( lSYS_CMD_Tasks,
        "SYS_CMD_TASKS",
        SYS_CMD_RTOS_STACK_SIZE,
        (void*)NULL,
        SYS_CMD_RTOS_TASK_PRIORITY,
        &xSYS_CMD_Tasks
    );



    (void) xTaskCreate( lSYS_FS_Tasks,
        "SYS_FS_TASKS",
        SYS_FS_STACK_SIZE,
        (void*)NULL,
        SYS_FS_PRIORITY,
        (TaskHandle_t*)NULL
    );



    /* Maintain Device Drivers */
    
    (void) xTaskCreate( lDRV_G3_MACRT_Tasks,
        "DRV_G3_MACRT_TASKS",
        DRV_PLC_RTOS_STACK_SIZE,
        (void*)NULL,
        DRV_PLC_RTOS_TASK_PRIORITY,
        (TaskHandle_t*)NULL
    );


    (void) xTaskCreate( lDRV_RF215_Tasks,
        "DRV_RF215_TASKS",
        DRV_RF215_RTOS_STACK_SIZE,
        (void*)NULL,
        DRV_RF215_RTOS_TASK_PRIORITY,
        (TaskHandle_t*)NULL
    );

    (void)xTaskCreate( lDRV_MEMORY_0_Tasks,
        "DRV_MEM_0_TASKS",
        DRV_MEMORY_STACK_SIZE_IDX0,
        (void*)NULL,
        DRV_MEMORY_PRIORITY_IDX0,
        (TaskHandle_t*)NULL
    );


    (void) xTaskCreate(lDRV_METROLOGY_Tasks,
        "DRV_METROLOGY_TASKS",
        DRV_METROLOGY_RTOS_STACK_SIZE,
        (void*)NULL,
        DRV_METROLOGY_RTOS_TASK_PRIORITY,
        (TaskHandle_t*)NULL
    );

    xTaskCreate( _SLCDC_Tasks,
        "SLCDC_Tasks",
        128,
        (void*)NULL,
        1,
        (TaskHandle_t*)NULL
    );



    /* Maintain Middleware & Other Libraries */
    
    (void) xTaskCreate( lG3_STACK_Tasks,
        "G3_STACK_TASKS",
        G3_STACK_RTOS_STACK_SIZE,
        (void*)NULL,
        G3_STACK_RTOS_TASK_PRIORITY,
        (TaskHandle_t*)NULL
    );


    xTaskCreate( _TCPIP_STACK_Task,
        "TCPIP_STACK_Tasks",
        TCPIP_RTOS_STACK_SIZE,
        (void*)NULL,
        TCPIP_RTOS_PRIORITY,
        (TaskHandle_t*)NULL
    );




    /* Maintain the application's state machine. */
        /* Create OS Thread for APP_METROLOGY_Tasks. */
    (void) xTaskCreate((TaskFunction_t) lAPP_METROLOGY_Tasks,
                "APP_METROLOGY_Tasks",
                1024,
                NULL,
                1,
                &xAPP_METROLOGY_Tasks);

    /* Create OS Thread for APP_CONSOLE_Tasks. */
    (void) xTaskCreate((TaskFunction_t) lAPP_CONSOLE_Tasks,
                "APP_CONSOLE_Tasks",
                1024,
                NULL,
                1,
                &xAPP_CONSOLE_Tasks);

    /* Create OS Thread for APP_DATALOG_Tasks. */
    (void) xTaskCreate((TaskFunction_t) lAPP_DATALOG_Tasks,
                "APP_DATALOG_Tasks",
                1024,
                NULL,
                1,
                &xAPP_DATALOG_Tasks);

    /* Create OS Thread for APP_DISPLAY_Tasks. */
    (void) xTaskCreate((TaskFunction_t) lAPP_DISPLAY_Tasks,
                "APP_DISPLAY_Tasks",
                1024,
                NULL,
                1,
                &xAPP_DISPLAY_Tasks);

    /* Create OS Thread for APP_ENERGY_Tasks. */
    (void) xTaskCreate((TaskFunction_t) lAPP_ENERGY_Tasks,
                "APP_ENERGY_Tasks",
                1024,
                NULL,
                1,
                &xAPP_ENERGY_Tasks);

    /* Create OS Thread for APP_EVENTS_Tasks. */
    (void) xTaskCreate((TaskFunction_t) lAPP_EVENTS_Tasks,
                "APP_EVENTS_Tasks",
                1024,
                NULL,
                1,
                &xAPP_EVENTS_Tasks);

    /* Create OS Thread for APP_G3_MANAGEMENT_Tasks. */
    (void) xTaskCreate((TaskFunction_t) lAPP_G3_MANAGEMENT_Tasks,
                "APP_G3_MANAGEMENT_Tasks",
                512,
                NULL,
                1,
                &xAPP_G3_MANAGEMENT_Tasks);

    /* Create OS Thread for APP_UDP_RESPONDER_Tasks. */
    (void) xTaskCreate((TaskFunction_t) lAPP_UDP_RESPONDER_Tasks,
                "APP_UDP_RESPONDER_Tasks",
                512,
                NULL,
                1,
                &xAPP_UDP_RESPONDER_Tasks);

    /* Create OS Thread for APP_STORAGE_PIC32CXMT_Tasks. */
    (void) xTaskCreate((TaskFunction_t) lAPP_STORAGE_PIC32CXMT_Tasks,
                "APP_STORAGE_PIC32CXMT_Tasks",
                128,
                NULL,
                1,
                &xAPP_STORAGE_PIC32CXMT_Tasks);

    /* Create OS Thread for APP_UDP_METROLOGY_Tasks. */
    (void) xTaskCreate((TaskFunction_t) lAPP_UDP_METROLOGY_Tasks,
                "APP_UDP_METROLOGY_Tasks",
                512,
                NULL,
                1,
                &xAPP_UDP_METROLOGY_Tasks);




    /* Start RTOS Scheduler. */
    
     /**********************************************************************
     * Create all Threads for APP Tasks before starting FreeRTOS Scheduler *
     ***********************************************************************/
    vTaskStartScheduler(); /* This function never returns. */

}

/*******************************************************************************
 End of File
 */

