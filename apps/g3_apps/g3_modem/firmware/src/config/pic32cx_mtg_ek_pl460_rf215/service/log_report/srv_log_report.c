/*******************************************************************************
  Source for the log report service

  Company:
    Microchip Technology Inc.

  File Name:
    srv_log_report.c

  Summary:
    Interface implementation for the log report service.

  Description:
    This file implements the interface for the debug report service.
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

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdio.h>
#include <stdarg.h>
#include "configuration.h"
#include "srv_log_report.h"

static char message[SYS_CONSOLE_PRINT_BUFFER_SIZE];

// *****************************************************************************
// *****************************************************************************
// Section: File scope functions
// *****************************************************************************
// *****************************************************************************

void SRV_LOG_REPORT_Message_With_Code(SRV_LOG_REPORT_LEVEL logLevel, 
                                      SRV_LOG_REPORT_CODE code, 
                                      const char *info, ...)
{
    va_list args = {0};
    
    /* Format the information */
    va_start(args, info);
    vsnprintf(message, SYS_CONSOLE_PRINT_BUFFER_SIZE, info, args);
    va_end(args);

    SYS_DEBUG_MESSAGE((SYS_ERROR_LEVEL)logLevel, message);

}

void SRV_LOG_REPORT_Message(SRV_LOG_REPORT_LEVEL logLevel, 
                            const char *info, ...)
{
    va_list args = {0};
    
    /* Format the information */
    va_start(args, info);
    vsnprintf(message, SYS_CONSOLE_PRINT_BUFFER_SIZE, info, args);
    va_end(args);
    
    SYS_DEBUG_PRINT((SYS_ERROR_LEVEL)logLevel, message);
}

void SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_LEVEL logLevel, 
                           const uint8_t *buffer, uint32_t bufferLength, 
                           const char *info, ...)
{
    va_list args = {0};
    uint32_t blockLength;
    uint32_t lastBlockLength;
    uint32_t lastPosition;
    uint8_t blockNumber;
    uint8_t lastBlock;
    
    /* Format the information */
    va_start(args, info);
    vsnprintf(message, SYS_CONSOLE_PRINT_BUFFER_SIZE, info, args);
    va_end(args);
    
    SYS_DEBUG_PRINT((SYS_ERROR_LEVEL)logLevel, message);
    
    /* The buffer will be printed in hex, so 1 byte turns into 2 chars */
    blockLength = SYS_CONSOLE_PRINT_BUFFER_SIZE / 2;
    blockNumber = bufferLength / blockLength;
    lastBlockLength = bufferLength % blockLength;
    if (lastBlockLength) 
    {
        ++blockNumber;
    } 
    else 
    {
        lastBlockLength = blockLength;
    }
    
    /* Format the buffer in hex */
    lastBlock = 0;
    while (lastBlock < blockNumber) 
    {
        if (lastBlock == (blockNumber - 1))
        {
            blockLength = lastBlockLength;
        }
    
        if (blockLength) 
        {
            lastPosition = lastBlock * (SYS_CONSOLE_PRINT_BUFFER_SIZE / 2);
            for (int i = 0; i < blockLength; i++) 
            {
                sprintf(&message[i*2], "%02x", buffer[lastPosition + i]);
            }
    
            SYS_DEBUG_PRINT((SYS_ERROR_LEVEL)logLevel, message);
        }
        
        ++lastBlock;
    }
    
    SYS_DEBUG_PRINT((SYS_ERROR_LEVEL)logLevel, "\r\n");
}
