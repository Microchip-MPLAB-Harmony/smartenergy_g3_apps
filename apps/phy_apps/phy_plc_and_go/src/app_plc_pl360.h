/*
Copyright (C) 2022, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app_plc.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_PLC_Initialize" and "APP_PLC_Tasks" prototypes) and some of them are only used
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
#define LED_BLINK_RATE_MS                         500
#define LED_PLC_RX_MSG_RATE_MS                    50

#define APP_PLC_BUFFER_SIZE                       512
#define APP_PLC_PIB_BUFFER_SIZE                   256

/* Each carrier corresponding to the band can be notched (no energy is sent in those carriers) */
/* Each carrier is represented by one byte (0: carrier used; 1: carrier notched). By default it is all 0's in PLC device */
/* The length is the number of carriers corresponding to the band in use. */
/* In this example case 36 (only valid for CENELEC-A band). */
/* The same Tone Mask must be set in both transmitter and receiver. Otherwise they don't understand each other */
#define APP_PLC_STATIC_NOTCHING_ENABLE                0
#define APP_PLC_TONE_MASK_STATIC_NOTCHING_EXAMPLE     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}

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
    APP_PLC_STATE_WAITING,
    APP_PLC_STATE_TX,
    APP_PLC_STATE_WAITING_TX_CFM,
    APP_PLC_STATE_SET_BAND,
    APP_PLC_STATE_SLEEP,
    APP_PLC_STATE_EXCEPTION,
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

    DRV_PLC_PHY_TX_RESULT lastTxResult;

    bool plcMultiband;

    DRV_PLC_PHY_PIB_OBJ plcPIB;

    bool staticNotchingEnable;

} APP_PLC_DATA;

typedef struct
{
    uint32_t pl360PhyVersion;

    DRV_PLC_PHY_TRANSMISSION_OBJ pl360Tx;

    uint8_t *pDataTx;

	uint8_t txAuto;

	uint8_t txImpedance;

    uint8_t toneMapSize;

    bool bin2InUse;

    SRV_PLC_PCOUP_BRANCH couplingBranch;

    uint16_t maxPsduLen;

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
    APP_PLC_Tasks function can be called.

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


bool APP_PLC_SendData ( uint8_t* pData, uint16_t length );
void APP_PLC_SetModScheme ( DRV_PLC_PHY_MOD_TYPE modType, DRV_PLC_PHY_MOD_SCHEME modScheme );



#endif /* _APP_PLC_PL360_H */

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

/*******************************************************************************
 End of File
 */

