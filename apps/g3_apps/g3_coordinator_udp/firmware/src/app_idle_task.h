/*******************************************************************************

  File Name:
    app_idle_task.h

  Summary:
    function prototype

  Description:
    This file contains the prototypes and definitions

 *******************************************************************************/

// DOM-IGNORE-BEGIN
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
// DOM-IGNORE-END

#ifndef APP_IDLE_HOOK_H
#define APP_IDLE_HOOK_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************



// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: System Functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/**
*@brief  This function performs the activities like PDS store, Sleep and other RF system idle activities
*    which can be performed when the complete system is idle.
*
*@param None
*
*@retval None
*/
void app_idle_task( void );

// *****************************************************************************
/**
*@brief  RTC based tickless idle mode Hook function records RTC counter value in each tick interrupt to ensure
*    the real time RTC counter value be recorded during system is active. Then RTC tickless idle mode
*    can use this value to calculate how much time passed during system sleep.
*
*@param cnt      -      RTC counter value
*
*@retval None
*/
void app_idle_updateRtcCnt(uint32_t cnt);


//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* APP_IDLE_HOOK_H */
/*******************************************************************************
 End of File
*/

