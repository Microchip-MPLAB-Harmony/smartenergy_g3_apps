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
//DOM-IGNORE-END

#ifndef _SRV_LOG_REPORT_H
#define _SRV_LOG_REPORT_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include "system/debug/sys_debug.h"

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
    SRV_LOG_REPORT_FATAL = SYS_ERROR_FATAL,
    
    /* Errors that have the potential to cause incorrect behavior. */
    SRV_LOG_REPORT_ERROR = SYS_ERROR_ERROR,

    /* Warnings about potentially unexpected behavior or side effects. */
    SRV_LOG_REPORT_WARNING = SYS_ERROR_WARNING,

    /* Information helpful to understanding potential errors and warnings. */
    SRV_LOG_REPORT_INFO = SYS_ERROR_INFO,

    /* Verbose information helpful during debugging and testing. */
    SRV_LOG_REPORT_DEBUG = SYS_ERROR_DEBUG

} SRV_LOG_REPORT_LEVEL;

// *****************************************************************************
/* Error/warning code enumeration

   Summary:
    Error/warning codes.

   Description:
    This enumeration lists the error and warning codes.

   Remarks:
    None.
*/

typedef enum
{
    /* Queue Management errors */
    QUEUE_FULL_INSERT_END          =   600,
    QUEUE_FULL_INSERT_BEFORE       =   601,
    QUEUE_FULL_INSERT_AFTER        =   602,
    QUEUE_FULL_INSERT_FIRST        =   603,
    QUEUE_EMPTY_REMOVE_HEAD        =   604,
    QUEUE_EMPTY_READ_REMOVE        =   605,
    QUEUE_FULL_APPEND              =   606,
    QUEUE_APPEND_BAD_ELEMENT       =   607,
    QUEUE_BAD_ELEMENT              =   608,
    QUEUE_BAD_TAIL                 =   609,
    QUEUE_APPEND_PRIO_SINGLE       =   610,
    QUEUE_NOT_INIT_NULL_SIZE       =   611,
    QUEUE_FIRST_BAD_INIT           =   612,
    QUEUE_LAST_NOT_TAIL            =   613,
    QUEUE_TOO_BIG                  =   614,
    QUEUE_BAD_NEXT_ELEMENT         =   615,
    QUEUE_WRONG_CHAIN              =   616,
    QUEUE_APPEND_AGAIN             =   617,
    QUEUE_APPEND_AGAIN_ONE_ELEMENT =   618,

    /* USI errors */
    USI_BAD_LENGTH                 =   9001, 
    USI_BAD_PROTOCOL               =   9002,  
    USI_BAD_CRC                    =   9003, 
    USI_INVALID_LENGTH             =   9004,  
    USI_ERROR_ESCAPE               =   9005

} SRV_LOG_REPORT_CODE;

//******************************************************************************
/* Function:
    void SRV_LOG_REPORT_Message_With_Code(SRV_LOG_REPORT_LEVEL logLevel, 
                                          SRV_LOG_REPORT_CODE code, 
                                          const char *info, ...)
  Summary:
    Reports an error/warning code with related information.

  Description:
    This function reports an error/warning code with related information.

  Precondition:
    The SYS_DEBUG initialization routines should be called before calling
    this routine (in "SYS_Initialize").
    
  Parameters:
    logLevel       - Log priority level 
    code           - Code of the reported error/warning
    info           - Formatted description of the reported error/warning

  Returns:
    None.

  Example:
    <code>
    SRV_LOG_REPORT_Message_With_Code(SRV_LOG_REPORT_WARNING, 100, 
                                     "Wrong input\r\n");
    </code>

  Remarks:
    The function does not add a newline after printing the information.
*/

void SRV_LOG_REPORT_Message_With_Code(SRV_LOG_REPORT_LEVEL logLevel, 
                                      SRV_LOG_REPORT_CODE code, 
                                      const char *info, ...);

//******************************************************************************
/* Function:
    void SRV_LOG_REPORT_Message(SRV_LOG_REPORT_LEVEL logLevel, 
                                const char *info, ...)
  Summary:
    Reports log information.

  Description:
    This function reports log information.

  Precondition:
    The SYS_DEBUG initialization routines should be called before calling
    this routine (in "SYS_Initialize").
    
  Parameters:
    logLevel       - Log priority level 
    info           - Formatted description of the information

  Returns:
    None.

  Example:
    <code>
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "No address\r\n");
    </code>

  Remarks:
    The function does not add a newline after printing the information.
*/

void SRV_LOG_REPORT_Message(SRV_LOG_REPORT_LEVEL logLevel, 
                            const char *info, ...);
                                      
//******************************************************************************
/* Function:
    void SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_LEVEL logLevel, 
                               const uint8_t *buffer, uint32_t bufferLength, 
                               const char *info, ...)

  Summary:
    Reports log information with a buffer.

  Description:
    This function reports log information with a buffer.

  Precondition:
    The SYS_DEBUG initialization routines should be called before calling
    this routine (in "SYS_Initialize").
    
  Parameters:
    logLevel          - Log priority level 
    buffer            - Buffer to be reported
    bufferLength      - Length of the buffer
    info              - Formatted description of the information
    
  Returns:
    None.

  Example:
    <code>
    uint8_t macAddress[6] = {0xAA, 0xBB, 0xCC, 0xDD, 0x00, 0x01};
    uint8_t maxNumberNodes = 100;
  
    SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_INFO, macAddress, 6, "MAC Address: ");
    </code>

  Remarks:
    The function automatically adds a newline after printing the buffer.
*/

void SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_LEVEL logLevel, 
                           const uint8_t *buffer, uint32_t bufferLength, 
                           const char *info, ...);

#ifdef __cplusplus // Provide C++ Compatibility
 }
#endif

#endif /* _SRV_LOG_REPORT_H */
