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
static void _DRV_G3_MACRT_Tasks(  void *pvParameters  )
{
    while(true)
    {
        /* Maintain G3 MAC RT Driver */
        DRV_G3_MACRT_Tasks(sysObj.drvG3MacRt);
    }
}

static void _G3_STACK_Tasks(  void *pvParameters  )
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

/* Handle for the APP_Tasks. */
TaskHandle_t xPHY_Tasks;

void _PHY_Tasks(  void *pvParameters  )
{
    while(1)
    {
        PHY_Tasks();
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

/* Handle for the APP_G3_MANAGEMENT_Tasks. */
TaskHandle_t xAPP_G3_MANAGEMENT_Tasks;

static void lAPP_G3_MANAGEMENT_Tasks(  void *pvParameters  )
{
    while(true)
    {
        APP_G3_MANAGEMENT_Tasks();
        vTaskDelay(50U / portTICK_PERIOD_MS);
    }
}
/* Handle for the APP_UDP_RESPONDER_Tasks. */
TaskHandle_t xAPP_UDP_RESPONDER_Tasks;

static void lAPP_UDP_RESPONDER_Tasks(  void *pvParameters  )
{
    while(true)
    {
        APP_UDP_RESPONDER_Tasks();
        vTaskDelay(50U / portTICK_PERIOD_MS);
    }
}
/* Handle for the APP_STORAGE_WBZ451_Tasks. */
TaskHandle_t xAPP_STORAGE_WBZ451_Tasks;

static void lAPP_STORAGE_WBZ451_Tasks(  void *pvParameters  )
{
    while(true)
    {
        APP_STORAGE_WBZ451_Tasks();
    }
}
/* Handle for the APP_EAP_SERVER_Tasks. */
TaskHandle_t xAPP_EAP_SERVER_Tasks;

static void lAPP_EAP_SERVER_Tasks(  void *pvParameters  )
{
    while(true)
    {
        APP_EAP_SERVER_Tasks();
        vTaskDelay(50U / portTICK_PERIOD_MS);
    }
}
/* Handle for the APP_CYCLES_Tasks. */
TaskHandle_t xAPP_CYCLES_Tasks;

static void lAPP_CYCLES_Tasks(  void *pvParameters  )
{
    while(true)
    {
        APP_CYCLES_Tasks();
        vTaskDelay(50U / portTICK_PERIOD_MS);
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



    /* Maintain Device Drivers */

    xTaskCreate( _DRV_G3_MACRT_Tasks,
        "DRV_G3_MACRT_TASKS",
        DRV_PLC_RTOS_STACK_SIZE,
        (void*)NULL,
        DRV_PLC_RTOS_TASK_PRIORITY,
        (TaskHandle_t*)NULL
    );



    /* Maintain Middleware & Other Libraries */

    xTaskCreate( _G3_STACK_Tasks,
        "G3_STACK_TASKS",
        G3_STACK_RTOS_STACK_SIZE,
        (void*)NULL,
        G3_STACK_RTOS_TASK_PRIORITY,
        (TaskHandle_t*)NULL
    );

    /* Create FreeRTOS task for IEEE_802154_PHY */
	 xTaskCreate((TaskFunction_t) _PHY_Tasks,
                "PHY_Tasks",
                1024,
                NULL,
                1,
                &xPHY_Tasks);


    xTaskCreate( _TCPIP_STACK_Task,
        "TCPIP_STACK_Tasks",
        TCPIP_RTOS_STACK_SIZE,
        (void*)NULL,
        TCPIP_RTOS_PRIORITY,
        (TaskHandle_t*)NULL
    );




    /* Maintain the application's state machine. */
        /* Create OS Thread for APP_G3_MANAGEMENT_Tasks. */
    (void) xTaskCreate((TaskFunction_t) lAPP_G3_MANAGEMENT_Tasks,
                "APP_G3_MANAGEMENT_Tasks",
                1024,
                NULL,
                1,
                &xAPP_G3_MANAGEMENT_Tasks);

    /* Create OS Thread for APP_UDP_RESPONDER_Tasks. */
    (void) xTaskCreate((TaskFunction_t) lAPP_UDP_RESPONDER_Tasks,
                "APP_UDP_RESPONDER_Tasks",
                1024,
                NULL,
                1,
                &xAPP_UDP_RESPONDER_Tasks);

    /* Create OS Thread for APP_STORAGE_WBZ451_Tasks. */
    (void) xTaskCreate((TaskFunction_t) lAPP_STORAGE_WBZ451_Tasks,
                "APP_STORAGE_WBZ451_Tasks",
                1024,
                NULL,
                1,
                &xAPP_STORAGE_WBZ451_Tasks);

    /* Create OS Thread for APP_EAP_SERVER_Tasks. */
    (void) xTaskCreate((TaskFunction_t) lAPP_EAP_SERVER_Tasks,
                "APP_EAP_SERVER_Tasks",
                1024,
                NULL,
                1,
                &xAPP_EAP_SERVER_Tasks);

    /* Create OS Thread for APP_CYCLES_Tasks. */
    (void) xTaskCreate((TaskFunction_t) lAPP_CYCLES_Tasks,
                "APP_CYCLES_Tasks",
                1024,
                NULL,
                1,
                &xAPP_CYCLES_Tasks);




    /* Start RTOS Scheduler. */

     /**********************************************************************
     * Create all Threads for APP Tasks before starting FreeRTOS Scheduler *
     ***********************************************************************/
    vTaskStartScheduler(); /* This function never returns. */

}

/*******************************************************************************
 End of File
 */

