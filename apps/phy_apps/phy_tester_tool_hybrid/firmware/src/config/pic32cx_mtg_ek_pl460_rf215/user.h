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

#define USER_PLC_BLINK_LED_On()       LED_GREEN_On()
#define USER_PLC_BLINK_LED_Off()      LED_GREEN_Off()
#define USER_PLC_BLINK_LED_Toggle()   LED_GREEN_Toggle()

#define USER_PLC_IND_LED_On()         LED_BLUE_On()
#define USER_PLC_IND_LED_Off()        LED_BLUE_Off()
#define USER_PLC_IND_LED_Toggle()     LED_BLUE_Toggle()

#define USER_RF_BLINK_LED_On()        LED_MIKROBUS2_PWM_On()
#define USER_RF_BLINK_LED_Off()       LED_MIKROBUS2_PWM_Off()
#define USER_RF_BLINK_LED_Toggle()    LED_MIKROBUS2_PWM_Toggle()

#define USER_PLC_USI_INSTANCE_INDEX   SRV_USI_INDEX_0
#define USER_RF_USI_INSTANCE_INDEX    SRV_USI_INDEX_0

#define CLEAR_WATCHDOG()              DWDT_WDT0_Clear()

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // USER_H
/*******************************************************************************
 End of File
*/
