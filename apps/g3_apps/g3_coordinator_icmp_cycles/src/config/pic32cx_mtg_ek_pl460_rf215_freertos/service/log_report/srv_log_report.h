/*******************************************************************************
  Header for log report service

  Company:
    Microchip Technology Inc.

  File Name:
    srv_log_report.h

  Summary:
    Interface definition for the log report service.

  Description:
    This file defines the interface for the log report service.
    Debug messages and log information is printed on the console.
    If a display is available, debug code errors will be shown.
*******************************************************************************/

//DOM-IGNORE-BEGIN
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
//DOM-IGNORE-END

#ifndef SRV_LOG_REPORT_H
#define SRV_LOG_REPORT_H

#ifdef __cplusplus // Provide C++ Compatibility
 extern "C" {
#endif

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Log level enumeration

   Summary:
    Log message priority levels.

   Description:
    This enumeration maps the log levels to the supported system error
    message priority values.

   Remarks:
    Used to add the right individual message priority before reporting through
    SYS_DEBUG.
*/

typedef enum
{
    /* Errors that have the potential to cause a system crash. */
    SRV_LOG_REPORT_FATAL = 0,

    /* Errors that have the potential to cause incorrect behavior. */
    SRV_LOG_REPORT_ERROR = 1,

    /* Warnings about potentially unexpected behavior or side effects. */
    SRV_LOG_REPORT_WARNING = 2,

    /* Information helpful to understanding potential errors and warnings. */
    SRV_LOG_REPORT_INFO = 3,

    /* Verbose information helpful during debugging and testing. */
    SRV_LOG_REPORT_DEBUG = 4

} SRV_LOG_REPORT_LEVEL;


#define SRV_LOG_REPORT_Message_With_Code(logLevel, code, info, ...)
#define SRV_LOG_REPORT_Message(logLevel, info, ...)
#define SRV_LOG_REPORT_Buffer(logLevel, buffer, bufferLength, info, ...)

#ifdef __cplusplus // Provide C++ Compatibility
 }
#endif

#endif /* SRV_LOG_REPORT_H */
