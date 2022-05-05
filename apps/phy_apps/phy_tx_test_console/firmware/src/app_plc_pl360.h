/*******************************************************************************
* Copyright (C) 2022 Microchip Technology Inc. and its subsidiaries.
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
    app_plc_pl360.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_PLC_PL360_Initialize" and "APP_PLC_PL360_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_PLC_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

#ifndef _APP_PLC_PL360_H
#define _APP_PLC_PL360_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "configuration.h"
#include "definitions.h"

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
#define LED_RESET_BLINK_RATE_MS                   100
#define LED_RESET_BLINK_COUNTER                   30
    
#define LED_BLINK_RATE_MS                         500
#define LED_PLC_RX_MSG_RATE_MS                    50
    
#define APP_PLC_BUFFER_SIZE                       512    
#define APP_PLC_PIB_BUFFER_SIZE                   256
    
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
    /* Application's state machine's initial state. */
    APP_PLC_STATE_IDLE=0,
    APP_PLC_STATE_INIT,
    APP_PLC_STATE_OPEN,
    APP_PLC_STATE_READ_CONFIG,
    APP_PLC_STATE_WRITE_CONFIG,
    APP_PLC_STATE_CHECK_CONFIG,
    APP_PLC_STATE_WAIT_CONFIG,
    APP_PLC_STATE_WAITING,
    APP_PLC_STATE_TX,
    APP_PLC_STATE_STOP_TX,
    APP_PLC_STATE_SET_BAND,
    APP_PLC_STATE_ERROR,

} APP_PLC_STATES;

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
    SYS_TIME_HANDLE tmr1Handle;
    
    volatile bool tmr1Expired;
    
    SYS_TIME_HANDLE tmr2Handle;
    
    volatile bool tmr2Expired;
    
    APP_PLC_STATES state;
    
    DRV_HANDLE drvPl360Handle;
    
    bool waitingTxCfm;
    
    bool couplingConfig;
    
    bool plcMultiband;
    
    DRV_PLC_PHY_PIB_OBJ plcPIB;
    
    uint8_t signalResetCounter;
    
} APP_PLC_DATA;

typedef struct
{    
    uint16_t configKey;
    
    uint32_t pl360PhyVersion;
    
    DRV_PLC_PHY_TRANSMISSION_OBJ pl360Tx;
    
	uint32_t txEndTime;
    
	uint8_t txAuto;
    
	uint8_t txImpedance;
    
    uint8_t toneMapSize;
    
    bool inTx;
    
    bool bin2InUse;
    
    SRV_PLC_PCOUP_BRANCH couplingBranch;

} APP_PLC_DATA_TX;

extern APP_PLC_DATA appPlc;
extern APP_PLC_DATA_TX appPlcTx;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Routines
// *****************************************************************************
// *****************************************************************************
/* These routines are called by drivers when certain events occur.
*/

/*******************************************************************************
  Function:
    void APP_PLC_PL360_Initialize ( void )

  Summary:
     MPLAB Harmony application initialization routine.

  Description:
    This function initializes the Harmony application.  It places the
    application in its initial state and prepares it to run so that its
    APP_PLC_PL360_Tasks function can be called.

  Precondition:
    All other system initialization routines should be called before calling
    this routine (in "SYS_Initialize").

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_PLC_PL360_Initialize();
    </code>

  Remarks:
    This routine must be called from the SYS_Initialize function.
*/

void APP_PLC_PL360_Initialize ( void );


/*******************************************************************************
  Function:
    void APP_PLC_PL360_Tasks ( void )

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
    APP_PLC_PL360_Tasks();
    </code>

  Remarks:
    This routine must be called from SYS_Tasks() routine.
 */

void APP_PLC_PL360_Tasks( void );



#endif /* _APP_PLC_PL360_H */

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

/*******************************************************************************
 End of File
 */

