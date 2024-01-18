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

/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app_display.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_DISPLAY_Initialize" and "APP_DISPLAY_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_DISPLAY_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

#ifndef _APP_DISPLAY_H
#define _APP_DISPLAY_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "configuration.h"
#include "system/time/sys_time.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application states

  Summary:
    Application states enumeration

  Description:
    This enumeration defines the valid application states. These states
    determine the behavior of the application at various times.
*/

typedef enum
{
    /* Application's state machine's initial state. */
    APP_DISPLAY_STATE_INIT=0,
    APP_DISPLAY_STATE_SERVICE_TASKS,
    APP_DISPLAY_STATE_DELAY_LOW_POWER,

} APP_DISPLAY_STATES;

// *****************************************************************************
/* Display information 
 * 
  Summary:
    Display information enumeration

  Description:
    This enumeration defines the valid display information . These determines
    which information will be shown in the display.
*/
typedef enum 
{
	APP_DISPLAY_TOTAL_ENERGY = 0,
	APP_DISPLAY_TOU1_ENERGY,
	APP_DISPLAY_TOU2_ENERGY,
	APP_DISPLAY_TOU3_ENERGY,
	APP_DISPLAY_TOU4_ENERGY,
	APP_DISPLAY_RTC_TIME,
	APP_DISPLAY_RTC_DATE,
	APP_DISPLAY_VA_RMS,
	APP_DISPLAY_VB_RMS,
	APP_DISPLAY_VC_RMS,
	APP_DISPLAY_IA_RMS,
	APP_DISPLAY_IB_RMS,
	APP_DISPLAY_IC_RMS,
	APP_DISPLAY_TOTAL_MAX_DEMAND,
	APP_DISPLAY_TOU1_MAX_DEMAND,
	APP_DISPLAY_TOU2_MAX_DEMAND,
	APP_DISPLAY_TOU3_MAX_DEMAND,
	APP_DISPLAY_TOU4_MAX_DEMAND,
	APP_DISPLAY_APP_INFO,
	APP_DISPLAY_BOARD_ID,
	APP_DISPLAY_DEMO_VERSION,
	APP_DISPLAY_MAX_TYPE,
            
} APP_DISPLAY_INFO;

// *****************************************************************************
/* Circular display direction
 * 
  Summary:
    Circular display direction enumeration

  Description:
    This enumeration defines the valid circular display directions. These 
    determines the direction in which the information will be shown in the 
    display.
*/
typedef enum
{
    APP_DISPLAY_FORWARD = 0x05,
    APP_DISPLAY_BACKWARD = 0x50,

} APP_DISPLAY_DIRECTION;

// *****************************************************************************
/* External communication signal status
 * 
  Summary:
    External communication signal enumeration

  Description:
    This enumeration defines the status of the signal of external communications.
*/
typedef enum
{
    APP_DISPLAY_COM_SIGNAL_OFF = 0,
    APP_DISPLAY_COM_SIGNAL_LOW,
    APP_DISPLAY_COM_SIGNAL_MED,
    APP_DISPLAY_COM_SIGNAL_HIGH,

} APP_DISPLAY_COM_SIGNAL;

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    Application strings and buffers are be defined outside this structure.
 */

typedef struct
{
    /* The application's current state */
    APP_DISPLAY_STATES state;

    /* Flag to indicate if the scroll up button has been pressed */
    volatile bool scrup_pressed;
    
    /* Flag to indicate if the scroll down button has been pressed */
    volatile bool scrdown_pressed;
    
    /* Application information */
    uint8_t app_info[8];
    
    /* Flag to indicate if there is application information to show */
    bool app_info_en;
    
    /* Reload time to display each information in the display (in seconds) */
	uint32_t reload_display_time;    
    
    /* Time to display each information in the display (in seconds) */
	uint32_t display_time;     
    
    /* Information about the external communication signal */
	APP_DISPLAY_COM_SIGNAL comm_signal;     
    
    /* Information shown in display */
	APP_DISPLAY_INFO display_info;
    
    /* Direction of the circular information in the display */
	APP_DISPLAY_DIRECTION direction;
    
    /* Circular information loop */
    APP_DISPLAY_INFO loop_info[APP_DISPLAY_MAX_TYPE];
    
    /* Maximum index in the information loop */
    uint8_t loop_max;
    
    /* Current index in the information loop */
    uint8_t loop_idx;
    
    /* Flag to indicate if DWDT0 should be reloaded */
    bool reloadDWDT0;
    
    /* Timer to handle delays */
    SYS_TIME_HANDLE timer;
    
    /* Flag to handle the display task */
    volatile bool timerFlag;

} APP_DISPLAY_DATA;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Routines
// *****************************************************************************
// *****************************************************************************
/* These routines are called by drivers when certain events occur.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_DISPLAY_Initialize ( void )

  Summary:
    MPLAB Harmony application initialization routine for the display.

  Description:
    This function initializes the Harmony application for the display.  
    It places the application in its initial state and prepares it to run 
    so that the APP_DISPLAY_Tasks function can be called.

  Precondition:
    All other system initialization routines should be called before calling
    this routine (in "SYS_Initialize").

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_DISPLAY_Initialize();
    </code>

  Remarks:
    This routine must be called from the SYS_Initialize function.
*/

void APP_DISPLAY_Initialize ( void );


/*******************************************************************************
  Function:
    void APP_DISPLAY_Tasks ( void )

  Summary:
    MPLAB Harmony Demo application tasks function for the display.

  Description:
    This routine is the Harmony Demo application tasks function for the 
    display. It defines the application state machine and core logic.

  Precondition:
    The system and application initialization ("SYS_Initialize" and 
    "APP_DISPLAY_Initialize") should be called before calling this.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_DISPLAY_Initialize();
    APP_DISPLAY_Tasks();
    </code>

  Remarks:
    This routine must be called from SYS_Tasks() routine.
 */

void APP_DISPLAY_Tasks( void );

/*******************************************************************************
  Function:
    void APP_DISPLAY_SetAppInfo(const char *msg, uint8_t len)

  Summary:
    Function to include application information in the display.

  Description:
    This function includes application information in the circular loop
    to be shown in the display.

  Precondition:
    The system and application initialization ("SYS_Initialize" and 
    "APP_DISPLAY_Initialize") should be called before calling this.

  Parameters:
    msg       - Pointer to the application message to show (only digits 0-9)
    len       - Length of the message (maximum 8 digits)

  Returns:
    None.

  Example:
    <code>
    char version_info[3] = {'1', '2', '19'};
  
    APP_DISPLAY_Initialize();
    APP_DISPLAY_Tasks();
    APP_DISPLAY_SetAppInfo(version_info, sizeof(version_info));
    </code>

  Remarks:
    None.
 */

void APP_DISPLAY_SetAppInfo(const char *msg, uint8_t len);

/*******************************************************************************
  Function:
    void APP_DISPLAY_SetSerialCommunication(void)

  Summary:
    Function to set the serial communication icon in the display.

  Description:
    This function set the serial communication icon in the display to indicate 
    that there is an ongoing communication through the console.

  Precondition:
    The system and application initialization ("SYS_Initialize" and 
    "APP_DISPLAY_Initialize") should be called before calling this.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    static void Command_xx(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
    {
        APP_DISPLAY_SetSerialCommunication(void);
    }
    </code>

  Remarks:
    This routine is called from the console application.
 */

void APP_DISPLAY_SetSerialCommunication(void);

/*******************************************************************************
  Function:
    void APP_DISPLAY_SetCommnicationSignal(APP_DISPLAY_COM_SIGNAL signal)

  Summary:
    Function to set the status of the external communication signal using 
    the specific signal icons of the display.

  Description:
    This function set the signal icons in the display to indicate the status
    of the external communications signal.

  Precondition:
    The system and application initialization ("SYS_Initialize" and 
    "APP_DISPLAY_Initialize") should be called before calling this.

  Parameters:
    signal - External communication signal status.

  Returns:
    None.

  Example:
    <code>
    if (ext_com_enable == True)
    {
        APP_DISPLAY_SetCommnicationSignal(APP_DISPLAY_COM_SIGNAL_HIGH);
    }
    </code>

  Remarks:
    None.
 */
void APP_DISPLAY_SetCommnicationSignal(APP_DISPLAY_COM_SIGNAL signal);

/*******************************************************************************
  Function:
    void APP_DISPLAY_ShowLowPowerMode(void)

  Summary:
    Function to show low power mode indicator.

  Description:
    This function clear the display and show low battery icon.

  Precondition:
    The system and application initialization ("SYS_Initialize" and 
    "APP_DISPLAY_Initialize") should be called before calling this.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    static void Command_xx(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
    {
        APP_DISPLAY_ShowLowPowerMode(void);
    }
    </code>

  Remarks:
    None.
 */

void APP_DISPLAY_ShowLowPowerMode(void);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* _APP_DISPLAY_H */

/*******************************************************************************
 End of File
 */

