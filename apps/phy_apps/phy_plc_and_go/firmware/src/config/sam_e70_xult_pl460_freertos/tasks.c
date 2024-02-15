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
static void F_USB_DEVICE_Tasks(  void *pvParameters  )
{
    while(true)
    {
                /* USB Device layer tasks routine */
        USB_DEVICE_Tasks(sysObj.usbDevObject0);
        vTaskDelay(10U / portTICK_PERIOD_MS);
    }
}


static void lDRV_PLC_PHY_Tasks(  void *pvParameters  )
{
    while(true)
    {
        /* Maintain PLC PHY Driver */
        DRV_PLC_PHY_Tasks(sysObj.drvPlcPhy);
    }
}

static void F_DRV_USBHSV1_Tasks(  void *pvParameters  )
{
    while(true)
    {
                 /* USB HS Driver Task Routine */
        DRV_USBHSV1_Tasks(sysObj.drvUSBHSV1Object);
        vTaskDelay(10U / portTICK_PERIOD_MS);
    }
}

void lSYS_CONSOLE_0_Tasks(  void *pvParameters  )
{
    while(1)
    {
        SYS_CONSOLE_Tasks(SYS_CONSOLE_INDEX_0);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}


/* Handle for the APP_PLC_Tasks. */
TaskHandle_t xAPP_PLC_Tasks;

static void lAPP_PLC_Tasks(  void *pvParameters  )
{   
    while(true)
    {
        APP_PLC_Tasks();
        vTaskDelay(10U / portTICK_PERIOD_MS);
    }
}
/* Handle for the APP_CONSOLE_Tasks. */
TaskHandle_t xAPP_CONSOLE_Tasks;

static void lAPP_CONSOLE_Tasks(  void *pvParameters  )
{   
    while(true)
    {
        APP_CONSOLE_Tasks();
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
        xTaskCreate( lSYS_CONSOLE_0_Tasks,
        "SYS_CONSOLE_0_TASKS",
        SYS_CONSOLE_RTOS_STACK_SIZE_IDX0,
        (void*)NULL,
        SYS_CONSOLE_RTOS_TASK_PRIORITY_IDX0,
        (TaskHandle_t*)NULL
    );



    /* Maintain Device Drivers */
    
    (void) xTaskCreate( lDRV_PLC_PHY_Tasks,
        "DRV_PLC_PHY_TASKS",
        DRV_PLC_RTOS_STACK_SIZE,
        (void*)NULL,
        DRV_PLC_RTOS_TASK_PRIORITY,
        (TaskHandle_t*)NULL
    );



    /* Maintain Middleware & Other Libraries */
        /* Create OS Thread for USB_DEVICE_Tasks. */
    (void) xTaskCreate( F_USB_DEVICE_Tasks,
        "USB_DEVICE_TASKS",
        1024,
        (void*)NULL,
        1,
        (TaskHandle_t*)NULL
    );

    /* Create OS Thread for USB Driver Tasks. */
    (void) xTaskCreate( F_DRV_USBHSV1_Tasks,
        "DRV_USBHSV1_TASKS",
        1024,
        (void*)NULL,
        1,
        (TaskHandle_t*)NULL
    );



    /* Maintain the application's state machine. */
        /* Create OS Thread for APP_PLC_Tasks. */
    (void) xTaskCreate((TaskFunction_t) lAPP_PLC_Tasks,
                "APP_PLC_Tasks",
                1024,
                NULL,
                1,
                &xAPP_PLC_Tasks);

    /* Create OS Thread for APP_CONSOLE_Tasks. */
    (void) xTaskCreate((TaskFunction_t) lAPP_CONSOLE_Tasks,
                "APP_CONSOLE_Tasks",
                1024,
                NULL,
                1,
                &xAPP_CONSOLE_Tasks);




    /* Start RTOS Scheduler. */
    
     /**********************************************************************
     * Create all Threads for APP Tasks before starting FreeRTOS Scheduler *
     ***********************************************************************/
    vTaskStartScheduler(); /* This function never returns. */

}

/*******************************************************************************
 End of File
 */

