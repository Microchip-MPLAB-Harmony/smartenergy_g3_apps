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
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app_events.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_EVENTS_Initialize" and "APP_EVENTS_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_EVENTS_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

#ifndef _APP_EVENTS_H
#define _APP_EVENTS_H

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
    
#define EVENT_LOG_MAX_NUMBER                 10
#define EVENT_HOLDING_START_COUNTER          10//60
#define EVENT_HOLDING_END_COUNTER            10//60
	
typedef enum 
{
	NO_EVENT = 0,
	EVENT_HOLDING_START,
	EVENT_START,
	EVENT_HOLDING_END
} APP_EVENTS_EVENT_STATUS;

typedef enum 
{
	SAG_UA_EVENT_ID = 0,
	SAG_UB_EVENT_ID,
	SAG_UC_EVENT_ID,
	POW_UA_EVENT_ID,
	POW_UB_EVENT_ID,
	POW_UC_EVENT_ID,
    EVENTS_NUM_ID,  
    EVENT_INVALID_ID = 0xFF,        
} APP_EVENTS_EVENT_ID;

typedef struct 
{
    struct tm startTime;
    struct tm endTime;
} APP_EVENTS_EVENT_INFO;

typedef struct 
{
	APP_EVENTS_EVENT_STATUS status;
	APP_EVENTS_EVENT_INFO data[EVENT_LOG_MAX_NUMBER];
	uint16_t counter;
	uint8_t holdingCounter;
	uint8_t dataIndex;
} APP_EVENTS_EVENT_DATA;

typedef struct 
{
    APP_EVENTS_EVENT_DATA event[EVENTS_NUM_ID];
} APP_EVENTS_EVENTS;

typedef struct
{
    // Metrology AFE Events
    DRV_METROLOGY_AFE_EVENTS eventFlags;

    // Time stamp when events have been detected
    struct tm eventTime;

} APP_EVENTS_QUEUE_DATA;

typedef struct {
    uint32_t paDir : 1;
    uint32_t pbDir : 1;
    uint32_t pcDir : 1;
    uint32_t ptDir : 1;
    uint32_t qaDir : 1;
    uint32_t qbDir : 1;
    uint32_t qcDir : 1;
    uint32_t qtDir : 1;
    uint32_t sagA : 1;
    uint32_t sagB : 1;
    uint32_t sagC : 1;
    uint32_t swellA : 1;
    uint32_t swellB : 1;
    uint32_t swellC : 1;
} APP_EVENTS_FLAGS;

// *****************************************************************************
/* Application states

  Summary:
    Application states enumeration

  Description:
    This enumeration defines the valid application states.  These states
    determine the behavior of the application at various times.
*/

typedef enum
{
    APP_EVENTS_STATE_WAITING_DATALOG = 0,
    APP_EVENTS_STATE_INIT,
    APP_EVENTS_STATE_RUNNING,
    APP_EVENTS_STATE_ERROR

} APP_EVENTS_STATES;


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
    APP_EVENTS_STATES state;

    APP_EVENTS_QUEUE_DATA newEvent;
    
    APP_EVENTS_EVENTS events;
    
    APP_EVENTS_FLAGS flags;
    
    bool dataIsRdy;

} APP_EVENTS_DATA;

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
    void APP_EVENTS_Initialize ( void )

  Summary:
     MPLAB Harmony application initialization routine.

  Description:
    This function initializes the Harmony application.  It places the
    application in its initial state and prepares it to run so that its
    APP_EVENTS_Tasks function can be called.

  Precondition:
    All other system initialization routines should be called before calling
    this routine (in "SYS_Initialize").

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_EVENTS_Initialize();
    </code>

  Remarks:
    This routine must be called from the SYS_Initialize function.
*/

void APP_EVENTS_Initialize ( void );


/*******************************************************************************
  Function:
    void APP_EVENTS_Tasks ( void )

  Summary:
    MPLAB Harmony Demo application tasks function

  Description:
    This routine is the Harmony Demo application's tasks function.  It
    defines the application's state machine and core logic.

  Precondition:
    The system and application initialization ("SYS_Initialize") should be
    called before calling this.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_EVENTS_Tasks();
    </code>

  Remarks:
    This routine must be called from SYS_Tasks() routine.
 */

void APP_EVENTS_Tasks( void );


void APP_EVENTS_ClearEvents(void);
bool APP_EVENTS_GetNumEvents(APP_EVENTS_EVENT_ID eventId, uint8_t * counter);
bool APP_EVENTS_GetEventInfo(APP_EVENTS_EVENT_ID eventId, uint8_t offset, APP_EVENTS_EVENT_INFO *eventInfo);
void APP_EVENTS_GetLastEventFlags(APP_EVENTS_FLAGS *eventFlags);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* _APP_EVENTS_H */

/*******************************************************************************
 End of File
 */

