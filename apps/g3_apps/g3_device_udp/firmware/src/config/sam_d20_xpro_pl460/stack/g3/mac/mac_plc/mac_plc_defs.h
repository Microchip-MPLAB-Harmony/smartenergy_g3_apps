/*******************************************************************************
  G3 MAC PLC Definitions Header File

  Company:
    Microchip Technology Inc.

  File Name:
    mac_plc_defs.h

  Summary:
    G3 MAC PLC Types and Definitions Header File

  Description:
    This file contains definitions of macros and types
    to be used by MAC Wrapper when accessing G3 MAC PLC layer.
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

#ifndef MAC_PLC_DEFS_H
#define MAC_PLC_DEFS_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include "../mac_common/mac_common.h"
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

#pragma pack(push,2)

// *****************************************************************************
/* MAC PLC Modulation Type definition

   Summary:
    Identifies the available Modulation Type Values for PLC communication.

   Description:
    This enumeration identifies the possible Modulation Types used when
    transmitting and receiving frames over PLC medium.

   Remarks:
    None.
*/
typedef enum
{
    MODULATION_ROBUST = 0x00,
    MODULATION_DBPSK_BPSK = 0x01,
    MODULATION_DQPSK_QPSK = 0x02,
    MODULATION_D8PSK_8PSK = 0x03,
    MODULATION_16_QAM = 0x04,
} MAC_PLC_MODULATION_TYPE;

// *****************************************************************************
/* MAC PLC Modulation Scheme definition

   Summary:
    Identifies the available Modulation Scheme Values for PLC communication.

   Description:
    This enumeration identifies the possible Modulation Schemes used when
    transmitting and receiving frames over PLC medium.

   Remarks:
    None.
*/
typedef enum
{
    MODULATION_SCHEME_DIFFERENTIAL = 0x00,
    MODULATION_SCHEME_COHERENT = 0x01,
} MAC_PLC_MODULATION_SCHEME;

// *****************************************************************************
/* MAC PLC Frame Control definition

   Summary:
    Defines the Frame Control fields of the MAC Header.

   Description:
    This structure contains the fields which define the Frame Control of a MAC
    frame header, as defined in IEEE 802.15.4, 2006 version.

   Remarks:
    None.
*/
typedef struct
{
    uint16_t frameType : 3;
    uint16_t securityEnabled : 1;
    uint16_t framePending : 1;
    uint16_t ackRequest : 1;
    uint16_t panIdCompression : 1;
    uint16_t reserved : 3;
    uint16_t destAddressingMode : 2;
    uint16_t frameVersion : 2;
    uint16_t srcAddressingMode : 2;
} MAC_PLC_FRAME_CONTROL;

// *****************************************************************************
/* MAC PLC Header definition

   Summary:
    Defines the fields of the MAC Header.

   Description:
    This structure contains the fields of a MAC frame header,
    as defined in IEEE 802.15.4.

   Remarks:
    None.
*/
typedef struct
{
    MAC_PAN_ID srcPanId;
    MAC_PAN_ID destPanId;
    MAC_PLC_FRAME_CONTROL frameControl;
    MAC_ADDRESS destAddress;
    MAC_ADDRESS srcAddress;
    MAC_AUXILIARY_SECURITY_HEADER securityHeader;
    uint8_t sequenceNumber;
} MAC_PLC_HEADER;

// *****************************************************************************
/* MAC PLC Frame definition

   Summary:
    Defines the fields of a MAC PLC Frame.

   Description:
    This structure contains the fields which define a MAC PLC Frame.

   Remarks:
    None.
*/
typedef struct
{
    uint8_t *payload;
    uint16_t payloadLength;
    uint16_t fcs;
    MAC_PLC_HEADER header;
    uint8_t padLength;
} MAC_PLC_FRAME;

#pragma pack(pop)

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // #ifndef MAC_PLC_DEFS_H

/*******************************************************************************
 End of File
*/
