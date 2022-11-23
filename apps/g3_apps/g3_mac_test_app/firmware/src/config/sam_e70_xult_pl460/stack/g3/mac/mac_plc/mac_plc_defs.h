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

#ifndef _MAC_PLC_DEFS_H
#define _MAC_PLC_DEFS_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include "../mac_common/mac_common.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Macro Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

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
    MAC_PLC_FRAME_CONTROL frameControl;
    uint8_t sequenceNumber;
    MAC_PAN_ID destPanId;
    MAC_ADDRESS destAddress;
    MAC_PAN_ID srcPanId;
    MAC_ADDRESS srcAddress;
    MAC_AUXILIARY_SECURITY_HEADER securityHeader;
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
    MAC_PLC_HEADER header;
    uint16_t payloadLength;
    uint8_t *payload;
    uint8_t padLength;
    uint16_t fcs;
} MAC_PLC_FRAME;

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // #ifndef _MAC_PLC_DEFS_H

/*******************************************************************************
 End of File
*/
