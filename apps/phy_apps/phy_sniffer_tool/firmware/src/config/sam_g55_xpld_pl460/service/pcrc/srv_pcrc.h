/*******************************************************************************
  CRC service used by PLC components Header File

  Company:
    Microchip Technology Inc.

  File Name:
    srv_pcrc.h

  Summary:
    CRC service used by PLC components Implementation.

  Description:
    The CRC wrapper provides a simple interface to manage the CRC needs
    for PLC components. This file implements the CRC core interface routines 
    for PLC. 

*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2021 Microchip Technology Inc. and its subsidiaries.
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
// DOM-IGNORE-END

#ifndef SRV_PCRC_H    // Guards against multiple inclusion
#define SRV_PCRC_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "system/system.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* USI Header Types

   Summary
    The list of all header types supported by PRIME spec and specific type to 
    use with USI service.

   Remarks:
    None
*/

typedef enum 
{
    PCRC_HT_GENERIC = 0,
    PCRC_HT_PROMOTION = 1,
    PCRC_HT_BEACON = 2,
    PCRC_HT_USI = 3
}PCRC_HEADER_TYPE;

// *****************************************************************************
/* CRC Types

   Summary
    The list of all CRC types supported (8, 16 and 32 bits).

   Remarks:
    None
*/

typedef enum 
{
    PCRC_CRC8 = 0,
    PCRC_CRC16 = 1,
    PCRC_CRC32 = 2,
    PCRC_NOCRC = 3
}PCRC_CRC_TYPE;

/* SRV_PCRC Handle Macro: Invalid CRC */  
#define PCRC_INVALID     0xFFFFFFFF

/* SRV_PCRC Handle Macro: SNA size */  
#define PCRC_SNA_SIZE 6

// *****************************************************************************
/* Function:
    uint32_t SRV_PCRC_GetValue(
      uint8_t *pData, 
      size_t length,
      PCRC_HEADER_TYPE hdrType, 
      PCRC_CRC_TYPE crcType,
      uint32_t initValue
    );

  Summary:
    Function to obtain the CRC value of the data.

  Description:
    This routine gets the CRC value of the data passed in arguments.

  Precondition:
    None.

  Parameters:
    pData  -    Pointer to the content of data to obtain CRC value.

    length -    Length of the data to obtain CRC value.

    hdrType -   Header type to determine what method to obtain CRC 
                should be used. It is different for USI and PRIME 
                packets (GENERIC, PROMOTION and BEACON).

    crcType -   CRC type(8, 16 or 32 bits). Valid only in case to use 
                PCRC_HT_USI.

    initValue - First value to start obtaining CRC value. Valid only in case 
                to use PCRC_HT_USI.

  Returns:
    If successful, the routine returns a valid CRC value.
    If an error occurs, the return value is PCRC_INVALID. Error can occur if
    hdrType or crcType are wrong.

  Example:
    <code>
    uint32_t valueTmp32;
    
    valueTmp32 = SRV_PCRC_GetValue(pData, length, PCRC_HT_USI, PCRC_CRC32);
    </code>

  Remarks:
    None.
*/        
uint32_t SRV_PCRC_GetValue(uint8_t *pData, size_t length,
        PCRC_HEADER_TYPE hdrType, PCRC_CRC_TYPE crcType, uint32_t initValue);

// *****************************************************************************
/* Function:
    void SRV_PCRC_SetSNAValue (uint8_t* sna);

  Summary:
    Function to set SNA value as initial value to get the CRC value of the data.

  Description:
    This routine gets the CRC value of the data passed in arguments.

  Precondition:
    None.

  Parameters:
    sna    -    Sub-Network Address used in PRIME specification to obtain
                CRC values in some PRIME control packets.

  Returns:
    None.

  Example:
    <code>
    uint8_t sna[PCRC_SNA_SIZE];
    
    SRV_PCRC_SetSNAValue(sna);
    </code>

  Remarks:
    None.
*/  
void SRV_PCRC_SetSNAValue (uint8_t* sna);

#endif //SRV_PCRC_H
