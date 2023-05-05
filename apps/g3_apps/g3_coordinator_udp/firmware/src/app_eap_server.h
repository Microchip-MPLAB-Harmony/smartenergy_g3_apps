/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app_eap_server.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_EAP_SERVER_Initialize" and "APP_EAP_SERVER_Tasks" prototypes) and some
    of them are only used internally by the application (such as the
    "APP_EAP_SERVER_STATES" definition). Both are defined here for convenience.
*******************************************************************************/

#ifndef _APP_EAP_SERVER_H
#define _APP_EAP_SERVER_H

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
// Section: Macro Definitions
// *****************************************************************************
// *****************************************************************************

/* PSK / Network authentication Key (16 bytes) */
#define APP_EAP_SERVER_PSK_KEY {0xAB, 0x10, 0x34, 0x11, 0x45, 0x11, 0x1B, 0xC3, 0xC1, 0x2D, 0xE8, 0xFF, 0x11, 0x14, 0x22, 0x04}

/* GMK (Global Master Key) (16 bytes) */
#define APP_EAP_SERVER_GMK_KEY {0xAF, 0x4D, 0x6D, 0xCC, 0xF1, 0x4D, 0xE7, 0xC1, 0xC4, 0x23, 0x5E, 0x6F, 0xEF, 0x6C, 0x15, 0x1F}

/* Maximum number of devices that can join the network */
#define APP_EAP_SERVER_MAX_DEVICES 500

/* Initial short address to assign */
#define APP_EAP_SERVER_INITIAL_SHORT_ADDRESS 0x0001

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
    This enumeration defines the valid application states.  These states
    determine the behavior of the application at various times.
*/

typedef enum
{
    /* Application's state machine's initial state. */
    APP_EAP_SERVER_STATE_WAIT_NETWORK_STARTED = 0,

    /* G3 network started */
    APP_EAP_SERVER_NETWORK_STARTED,

    /* Error state */
    APP_EAP_SERVER_STATE_ERROR,

} APP_EAP_SERVER_STATES;


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
    /* Number of short addresses assigned */
    uint16_t numShortAddrAssigned;

    /* Number of devices joined to the network */
    uint16_t numDevicesJoined;

    /* Re-key index */
    uint16_t rekeyIndex;

    /* Size of blacklist */
    uint16_t blacklistSize;

    /* The application's current state */
    APP_EAP_SERVER_STATES state;

    /* Re-key process in progress */
    bool rekey;

    /* Distribute re-key phase flag  */
    bool rekeyPhaseDistribute;

} APP_EAP_SERVER_DATA;

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_EAP_SERVER_Initialize ( void )

  Summary:
     MPLAB Harmony application initialization routine.

  Description:
    This function initializes the Harmony application.  It places the
    application in its initial state and prepares it to run so that its
    APP_EAP_SERVER_Tasks function can be called.

  Precondition:
    All other system initialization routines should be called before calling
    this routine (in "SYS_Initialize").

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_EAP_SERVER_Initialize();
    </code>

  Remarks:
    This routine must be called from the SYS_Initialize function.
*/

void APP_EAP_SERVER_Initialize ( void );

/*******************************************************************************
  Function:
    void APP_EAP_SERVER_Tasks ( void )

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
    APP_EAP_SERVER_Tasks();
    </code>

  Remarks:
    This routine must be called from SYS_Tasks() routine.
 */

void APP_EAP_SERVER_Tasks( void );

// *****************************************************************************
// *****************************************************************************
// Section: Application Interface Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_EAP_SERVER_LaunchRekeying(void)

  Summary:
    Launch the re-keying process.

  Description:
    This function launches the re-keying process.

  Precondition:
    APP_EAP_SERVER_Initialize should be called before calling this routine.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_EAP_SERVER_LaunchRekeying();
    </code>

  Remarks:
    None.
*/

void APP_EAP_SERVER_LaunchRekeying(void);

/*******************************************************************************
  Function:
    void APP_EAP_SERVER_KickDevice(uint16_t shortAddress)

  Summary:
    Kicks a device from the network.

  Description:
    This function kicks a device from the G3 network.

  Precondition:
    APP_EAP_SERVER_Initialize should be called before calling this routine.

  Parameters:
    shortAddress - Short address of the device to kick.

  Returns:
    None.

  Example:
    <code>
    APP_EAP_SERVER_KickDevice(0x0001);
    </code>

  Remarks:
    None.
*/

void APP_EAP_SERVER_KickDevice(uint16_t shortAddress);

/*******************************************************************************
  Function:
    void APP_EAP_SERVER_AddToBlacklist(uint8_t* extendedAddress)

  Summary:
    Adds a device to the blacklist.

  Description:
    This function adds a device to the blacklist. If that device tries to join,
    it will be denied.

  Precondition:
    APP_EAP_SERVER_Initialize should be called before calling this routine.

  Parameters:
    extendedAddress - Pointer to extended address (8 bytes).

  Returns:
    None.

  Example:
    <code>
    uint8_t extAddress[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    APP_EAP_SERVER_AddToBlacklist(&extAddress);
    </code>

  Remarks:
    None.
*/

void APP_EAP_SERVER_AddToBlacklist(uint8_t* extendedAddress);

/*******************************************************************************
  Function:
    uint16_t APP_EAP_SERVER_GetNumDevicesJoined(void)

  Summary:
    Gets number of devices joined to the network.

  Description:
    This function gets number of devices joined to the G3 network.

  Precondition:
    APP_EAP_SERVER_Initialize should be called before calling this routine.

  Parameters:
    None.

  Returns:
    Number of devices joined to the G3 network.

  Example:
    <code>
    uint16_t numDevices;
    numDevices = APP_EAP_SERVER_GetNumDevicesJoined();
    </code>

  Remarks:
    None.
*/

uint16_t APP_EAP_SERVER_GetNumDevicesJoined(void);

/*******************************************************************************
  Function:
    uint16_t APP_EAP_SERVER_GetDeviceAddress(uint16_t index, uint8_t* pEUI64)

  Summary:
    Gets the address of a connected device for a specified index.

  Description:
    This function gets the addresses (short and extended) of a connected device
    for a specified index.

  Precondition:
    APP_EAP_SERVER_Initialize should be called before calling this routine.

  Parameters:
    index  - Index of the device node to get the address
    pEUI64 - Pointer to store the EUI64 (Extended Address)

  Returns:
    Short address for the specified device index.

  Example:
    <code>
    uint8_t eui64[8];
    uint16_t shortAddress;

    shortAddress = APP_EAP_SERVER_GetDeviceAddress(0, eui64);
    </code>

  Remarks:
    None.
*/

uint16_t APP_EAP_SERVER_GetDeviceAddress(uint16_t index, uint8_t* pEUI64);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* _APP_EAP_SERVER_H */

/*******************************************************************************
 End of File
 */

