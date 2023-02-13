/*******************************************************************************
    G3 MAC RF Definitions Header File

    Company:
        Microchip Technology Inc.

    File Name:
        mac_rf_defs.h

    Summary:
        G3 MAC RF Types and Definitions Header File

    Description:
        This file contains definitions of macros and types
        to be used by MAC Wrapper when accessing G3 MAC RF layer.
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

#ifndef _MAC_RF_DEFS_H
#define _MAC_RF_DEFS_H

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
// Section: Macro Definitions
// *****************************************************************************
// *****************************************************************************

#define HIE_CID_LEN   3

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* MAC RF Frame Control definition

   Summary:
    Defines the Frame Control fields of the MAC Header.

   Description:
    This structure contains the fields which define the Frame Control of a MAC
    frame header, as defined in IEEE 802.15.4, 2015 version.

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
    uint16_t reserved : 2;
    uint16_t iePresent : 1;
    uint16_t destAddressingMode : 2;
    uint16_t frameVersion : 2;
    uint16_t srcAddressingMode : 2;
} MAC_RF_FRAME_CONTROL;

// *****************************************************************************
/* MAC RF Header definition

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
    MAC_RF_FRAME_CONTROL frameControl;
    uint8_t sequenceNumber;
    MAC_PAN_ID destPanId;
    MAC_ADDRESS destAddress;
    MAC_PAN_ID srcPanId;
    MAC_ADDRESS srcAddress;
    MAC_AUXILIARY_SECURITY_HEADER securityHeader;
} MAC_RF_HEADER;

// *****************************************************************************
/* MAC RF Header Information Element definition

   Summary:
    Defines the fields of the MAC Header Information Element.

   Description:
    This structure contains the fields of a MAC header Information Element,
    as defined in IEEE 802.15.4.

   Remarks:
    None.
*/
typedef struct
{
    uint16_t length : 7;
    uint16_t elementID : 8;
    uint16_t type : 1;
    uint8_t cid[HIE_CID_LEN];
    uint8_t subID;
} MAC_RF_HEADER_IE;

// *****************************************************************************
/* MAC RF Link Info Information Element definition

   Summary:
    Defines the fields of the MAC Link Info Information Element.

   Description:
    This structure contains the fields of a MAC Link Info Information Element,
    as defined in G3 Spec.

   Remarks:
    None.
*/
typedef struct
{
    MAC_RF_HEADER_IE headerIE;
    uint8_t dutyCycle;
    uint8_t txPowerOffset;
} MAC_RF_LI_IE;

// *****************************************************************************
/* MAC RF Reverse Link Quality Information Element definition

   Summary:
    Defines the fields of the MAC Reverse Link Quality Information Element.

   Description:
    This structure contains the fields of a MAC Reverse Link Quality
    Information Element, as defined in G3 Spec.

   Remarks:
    None.
*/
typedef struct
{
    MAC_RF_HEADER_IE headerIE;
    uint8_t reverseLqi;
} MAC_RF_RLQ_IE;

// *****************************************************************************
/* MAC RF Frame definition

   Summary:
    Defines the fields of a MAC RF Frame.

   Description:
    This structure contains the fields which define a MAC RF Frame.

   Remarks:
    None.
*/
typedef struct
{
    MAC_RF_HEADER header;
    MAC_RF_LI_IE liIE;
    MAC_RF_RLQ_IE rlqIE;
    uint16_t payloadLength;
    uint8_t *payload;
    uint32_t fcs;
} MAC_RF_FRAME;

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // #ifndef _MAC_RF_DEFS_H

/*******************************************************************************
 End of File
*/
