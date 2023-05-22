/*******************************************************************************
  User Configuration Header

  File Name:
    user.h

  Summary:
    Build-time configuration header for the user defined by this project.

  Description:
    An MPLAB Project may have multiple configurations.  This file defines the
    build-time options for a single configuration.

  Remarks:
    It only provides macro definitions for build-time configuration options

*******************************************************************************/

#ifndef USER_H
#define USER_H

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: User Configuration macros
// *****************************************************************************
// *****************************************************************************

#define USER_BLINK_LED_On()           LED_On()
#define USER_BLINK_LED_Off()          LED_Off()
#define USER_BLINK_LED_Toggle()       LED_Toggle()
    
#define USER_PLC_IND_LED_On()         LED_EXT1_Pin4_On()
#define USER_PLC_IND_LED_Off()        LED_EXT1_Pin4_Off()
#define USER_PLC_IND_LED_Toggle()     LED_EXT1_Pin4_Toggle()

#define CLEAR_WATCHDOG()              WDT_Clear()

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // USER_H
/*******************************************************************************
 End of File
*/
