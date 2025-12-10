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

// *****************************************************************************
/* Display board identifier

  Summary:
    Display board identifier.

  Description:
    This is the display board identifier.
*/

#define APP_DISPLAY_BOARD_VERSION           0x0002

// *****************************************************************************
/* Demo application major version

  Summary:
    Demo application major version.

  Description:
    This is the demo application major version.
    99 is the default value for the development version.
*/

#define DEMO_APP_MAJOR_VERSION   3

// *****************************************************************************
/* Demo application minor version

  Summary:
    Demo application minor version.

  Description:
    This is the demo application minor version.
    99 is the default value for the development version.
*/
#define DEMO_APP_MINOR_VERSION   0

// *****************************************************************************
/* Demo application patch version

  Summary:
    Demo application patch version.

  Description:
    This is the demo application patch version.
    99 is the default value for the development version.
*/

#define DEMO_APP_PATCH_VERSION   0

// *****************************************************************************
/* Demo application version

  Summary:
    Demo application version.

  Description:
    This is the demo application version: (MMmmpp - M(Major); m(minor); p(patch)).
    999999 is the default value for the development version.
*/

#define DEMO_APP_VERSION ((DEMO_APP_MAJOR_VERSION)*10000 + (DEMO_APP_MINOR_VERSION)*100 + (DEMO_APP_PATCH_VERSION))

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // USER_H
/*******************************************************************************
 End of File
*/
