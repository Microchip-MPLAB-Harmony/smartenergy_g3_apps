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

#ifndef SRV_PCRC_H    // Guards against multiple inclusion
#define SRV_PCRC_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
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
    The list of all header types supported by PRIME spec and an specific type
    to use with USI service.

   Remarks:
    None
*/

typedef enum
{
    PCRC_HT_GENERIC = 0, /* GENERIC Packet type */
    PCRC_HT_PRIME_GENERIC, /* PRIME Generic Packet type */
    PCRC_HT_PRIME_BEACON14, /* PRIME 1.4 Beacon Packet type */
    PCRC_HT_USI, /* USI Packet type */
    PCRC_HT_MM /* Meters And More Packet type */
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
    PCRC_CRC8 = 0, /* 8-bit CRC */
    PCRC_CRC16, /* 16-bit CRC */
    PCRC_CRC32, /* 32-bit CRC */
    PCRC_NOCRC  /* No CRC */
}PCRC_CRC_TYPE;

/* SRV_PCRC Handle Macro: Invalid CRC */
#define PCRC_INVALID     0xFFFFFFFFUL

/* SRV_PCRC Handle Macro: SNA size */
#define PCRC_SNA_SIZE    6


/* PRIME v1.4 constant CRC size: Constant CRC field size */
#define PCRC_CONST_BCN_PRIME_1_4_SIZE       4

// *****************************************************************************
// *****************************************************************************
// Section: CRC Service Interface Definition
// *****************************************************************************
// *****************************************************************************

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
    Obtains the CRC for a data stream.

  Description:
    This routine gets the CRC value (8, 16 or 32 bits, depending on arguments)
    of the data stream provided as argument.

  Precondition:
    None.

  Parameters:
    pData  -    Pointer to buffer containing the data stream.

    length -    Length of the data stream.

    hdrType -   Header type to determine the method to obtain CRC.

    crcType -   CRC type(8, 16 or 32 bits).

    initValue - Initialization value for CRC computation.

  Returns:
    If successful, the routine returns a valid CRC value.
    If an error occurs, the return value is PCRC_INVALID. Error can occur if
    hdrType or crcType are wrong.
    Returned CRC is always a 32-bit value, in case 8-bit or 16-bit CRC
    is calculated it is casted to 32-bit.

  Example:
    <code>
    uint32_t valueTmp32;

    valueTmp32 = SRV_PCRC_GetValue(pData, length, PCRC_HT_USI, PCRC_CRC32, 0);
    </code>

  Remarks:
    None.
*/
uint32_t SRV_PCRC_GetValue(uint8_t *pData, size_t length,
    PCRC_HEADER_TYPE hdrType, PCRC_CRC_TYPE crcType, uint32_t initValue);

// *****************************************************************************
/* Function:
    void SRV_PCRC_ConfigureSNA (uint8_t* sna);

  Summary:
    Sets SNA (Sub-Network Address) value to be used as initial value on
    further CRC calculations (used by PRIME stack).

  Description:
    This routine sets the value that will be set as the initial CRC value
    for computations related to PRIME stack, as stated on PRIME specification.

  Precondition:
    None.

  Parameters:
    sna    -    Pointer to buffer containing SNA value.

  Returns:
    None.

  Example:
    <code>
    uint8_t sna[PCRC_SNA_SIZE];

    SRV_PCRC_ConfigureSNA(sna);
    </code>

  Remarks:
    None.
*/
void SRV_PCRC_ConfigureSNA (uint8_t* sna);

#endif //SRV_PCRC_H
