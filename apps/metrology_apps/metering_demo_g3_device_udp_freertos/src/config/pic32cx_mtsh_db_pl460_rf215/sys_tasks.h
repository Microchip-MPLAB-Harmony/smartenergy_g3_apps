/*******************************************************************************
 System Tasks Header File

  File Name:
    sys_tasks.h

  Summary:
    This file contains declarations for task handles.

  Description:
    Task handles declared in this header file can be used by the application
    to control the behavior of the tasks.

  Remarks:
    None
 *******************************************************************************/

// DOM-IGNORE-BEGIN
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
// DOM-IGNORE-END

#ifndef SYS_TASKS_H
#define SYS_TASKS_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "configuration.h"
#include "definitions.h"

// *****************************************************************************
// *****************************************************************************
// Section: RTOS "Tasks" Handles
// *****************************************************************************
// *****************************************************************************
/* Declaration of  APP_METROLOGY_Tasks task handle */
extern TaskHandle_t xAPP_METROLOGY_Tasks;

/* Declaration of  APP_CONSOLE_Tasks task handle */
extern TaskHandle_t xAPP_CONSOLE_Tasks;

/* Declaration of  APP_DATALOG_Tasks task handle */
extern TaskHandle_t xAPP_DATALOG_Tasks;

/* Declaration of  APP_DISPLAY_Tasks task handle */
extern TaskHandle_t xAPP_DISPLAY_Tasks;

/* Declaration of  APP_ENERGY_Tasks task handle */
extern TaskHandle_t xAPP_ENERGY_Tasks;

/* Declaration of  APP_EVENTS_Tasks task handle */
extern TaskHandle_t xAPP_EVENTS_Tasks;

/* Declaration of  APP_G3_MANAGEMENT_Tasks task handle */
extern TaskHandle_t xAPP_G3_MANAGEMENT_Tasks;

/* Declaration of  APP_UDP_RESPONDER_Tasks task handle */
extern TaskHandle_t xAPP_UDP_RESPONDER_Tasks;

/* Declaration of  APP_STORAGE_PIC32CXMT_Tasks task handle */
extern TaskHandle_t xAPP_STORAGE_PIC32CXMT_Tasks;

/* Declaration of  APP_UDP_METROLOGY_Tasks task handle */
extern TaskHandle_t xAPP_UDP_METROLOGY_Tasks;

/* Declaration of  APP_TCPIP_MANAGEMENT_Tasks task handle */
extern TaskHandle_t xAPP_TCPIP_MANAGEMENT_Tasks;


/* Declaration of SYS_COMMAND task handle */
extern TaskHandle_t xSYS_CMD_Tasks;



#endif //SYS_TASKS_H