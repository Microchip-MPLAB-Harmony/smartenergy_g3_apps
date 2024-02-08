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
    app.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_Initialize" and "APP_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_STATE" definition).  Both
    are defined here for convenience.
*******************************************************************************/

#ifndef _APP_H
#define _APP_H

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
#define APP_SERIAL_DATA_BUFFER_SIZE   512
#define APP_PLC_DATA_BUFFER_SIZE      512
#define APP_PLC_PIB_BUFFER_SIZE       256

#define LED_BLINK_RATE_MS             500
#define LED_BLINK_PLC_MSG_MS          100

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
    APP_STATE_IDLE=0,
    APP_STATE_INIT,
    APP_STATE_REGISTER,
    APP_STATE_CONFIG_PLC,
    APP_STATE_CONFIG_USI,
    APP_STATE_SEND_PLC_MSG,
    APP_STATE_SEND_USI_MSG,
    APP_STATE_READY,
    APP_STATE_EXCEPTION,
    APP_STATE_ERROR

} APP_STATE;

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
    APP_STATE state;

    SYS_TIME_HANDLE tmr1Handle;

    volatile bool tmr1Expired;

    SYS_TIME_HANDLE tmr2Handle;

    volatile bool tmr2Expired;

    DRV_HANDLE drvPlcHandle;

    SRV_USI_HANDLE srvUSIHandle;

    bool plc_phy_exception;

    uint32_t plc_phy_err_unexpected;

    uint32_t plc_phy_err_critical;

    uint32_t plc_phy_err_reset;

    uint32_t plc_phy_err_unknow;

    uint8_t *pSerialData;

    DRV_PLC_PHY_TRANSMISSION_OBJ plcTxObj;

    DRV_PLC_PHY_TRANSMISSION_CFM_OBJ plcTxCfmObj;

    DRV_PLC_PHY_RECEPTION_OBJ plcRxObj;

    DRV_PLC_PHY_PIB_OBJ plcPIB;

    bool pvddMonTxEnable;

    APP_PLC_TX_STATE plcTxState;

} APP_DATA;

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_WBZ451_Initialize ( void )

  Summary:
     MPLAB Harmony application initialization routine.

  Description:
    This function initializes the Harmony application.  It places the
    application in its initial state and prepares it to run so that its
    APP_Tasks function can be called.

  Precondition:
    All other system initialization routines should be called before calling
    this routine (in "SYS_Initialize").

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_WBZ451_Initialize();
    </code>

  Remarks:
    This routine must be called from the SYS_Initialize function.
*/

void APP_WBZ451_Initialize ( void );


/*******************************************************************************
  Function:
    void APP_WBZ451_Tasks ( void )

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
    APP_WBZ451_Tasks();
    </code>

  Remarks:
    This routine must be called from SYS_Tasks() routine.
 */

void APP_WBZ451_Tasks( void );



#endif /* _APP_H */

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

/*******************************************************************************
 End of File
 */

