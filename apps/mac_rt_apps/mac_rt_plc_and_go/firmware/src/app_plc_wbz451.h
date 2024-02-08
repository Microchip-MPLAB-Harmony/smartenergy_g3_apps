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

#ifndef _APP_PLC_H
#define _APP_PLC_H

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

#define APP_PLC_PIB_BUFFER_SIZE                   256

/* Each carrier corresponding to the band can be notched (no energy is sent in those carriers) */
/* Each carrier is represented by one bit (1: carrier used; 0: carrier notched). By default it is all 1's in the PLC device */
/* The length is the max number of carriers of the broadest band, this is 72 bits (9 bytes), where only the number of carriers in band is used, in this case 72 (FCC) */
/* The same Tone Mask must be set in both transmitter and receiver. Otherwise they don't understand each other */
#define APP_PLC_STATIC_NOTCHING_ENABLE                0
#define APP_PLC_TONE_MASK_STATIC_NOTCHING_EXAMPLE     {0xFF, 0xFF, 0x01, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}

/* ACK request modes */
#define CONF_ACK_REQUEST_DISABLE           0
#define CONF_ACK_REQUEST_ENABLE            1

/* Configure ACK request */
#define CONF_ACK_REQUEST                   CONF_ACK_REQUEST_DISABLE

/* Configure PAN ID */
#define CONF_PAN_ID                        0x781D

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

/* PLC Transmission Status

  Summary:
    PLC Transmission states enumeration

  Description:
    This structure holds the PLC transmission's status.
 */

typedef enum
{
    APP_PLC_TX_STATE_IDLE=0,
    APP_PLC_TX_STATE_WAIT_TX_CFM,
    APP_PLC_TX_STATE_WAIT_TX_CANCEL

} APP_PLC_TX_STATE;

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

    DRV_HANDLE drvPlcHandle;

    bool plcMultiband;

    bool bin2InUse;

    bool staticNotchingEnable;

    bool pvddMonTxEnable;

    uint32_t phyVersion;

    uint8_t toneMapSize;

    APP_PLC_STATES state;

    uint16_t maxPsduLen;

    APP_PLC_TX_STATE plcTxState;

    MAC_RT_PIB_OBJ plcPIB;

    SRV_PLC_PCOUP_BRANCH couplingBranch;

} APP_PLC_DATA;

typedef struct
{
    uint8_t *pTxFrame;

    MAC_RT_HEADER txHeader;

    MAC_RT_RX_PARAMETERS_OBJ rxParams;

    MAC_RT_STATUS lastTxStatus;

    bool ackRequest;

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
    void APP_PLC_WBZ451_Initialize ( void )

  Summary:
     MPLAB Harmony application initialization routine.

  Description:
    This function initializes the Harmony application.  It places the
    application in its initial state and prepares it to run so that its
    APP_PLC_WBZ451Tasks function can be called.

  Precondition:
    All other system initialization routines should be called before calling
    this routine (in "SYS_Initialize").

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_PLC_WBZ451_Initialize();
    </code>

  Remarks:
    This routine must be called from the SYS_Initialize function.
*/

void APP_PLC_WBZ451_Initialize ( void );


/*******************************************************************************
  Function:
    void APP_PLC_WBZ451_Tasks ( void )

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
    APP_PLC_WBZ451_Tasks();
    </code>

  Remarks:
    This routine must be called from SYS_Tasks() routine.
 */

void APP_PLC_WBZ451_Tasks( void );


bool APP_PLC_SendData ( uint8_t* pData, uint16_t length );
bool APP_PLC_SetSleepMode ( bool enable );
void APP_PLC_SetSourceAddress ( uint16_t address );
void APP_PLC_SetDestinationAddress ( uint16_t address );
void APP_PLC_SetPANID ( uint16_t panid );



#endif /* _APP_PLC_H */

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

/*******************************************************************************
 End of File
 */

