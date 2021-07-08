/*******************************************************************************
  USI Service Local Data Structures

  Company:
    Microchip Technology Inc.

  File Name:
    srv_usi_local.h

  Summary:
    USI Service Local Data Structures

  Description:
    Service Local Data Structures
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
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

#ifndef SRV_USI_LOCAL_H
#define SRV_USI_LOCAL_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "service/usi/srv_usi.h"
#include "service/usi/srv_usi_definitions.h"
#include "osal/osal.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************
#define USI_ESC_KEY_7E                0x7E
#define USI_ESC_KEY_5E                0x5E
#define USI_ESC_KEY_7D                0x7D
#define USI_ESC_KEY_5D                0x5D

#define USI_TYPE_OFFSET               1
#define USI_TYPE_MSK                  0x3F
#define USI_LEN_HI_OFFSET             0
#define USI_LEN_HI_MSK                0xFF
#define USI_LEN_HI_SHIFT              2
#define USI_LEN_LO_OFFSET             1
#define USI_LEN_LO_MSK                0xC0
#define USI_LEN_LO_SHIFT              6
#define USI_XLEN_OFFSET               2
#define USI_XLEN_MSK                  0xC0
#define USI_XLEN_SHIFT_L              4
#define USI_XLEN_SHIFT_R              10
#define USI_XLEN2_OFFSET              2
#define USI_XLEN2_MSK                 0xF0
#define USI_XLEN2_SHIFT_L             6
#define USI_XLEN2_SHIFT_R             6
#define USI_PAYLOAD_OFFSET            2
#define USI_CMD_OFFSET                2
#define USI_CMD_MSK                   0x3F
#define USI_CMD2_MSK                  0x0F
        
#define USI_TYPE_PROTOCOL(A)       ((A)&USI_TYPE_MSK)
#define USI_LEN_PROTOCOL(A, B)     ((((uint16_t)(A)) << USI_LEN_HI_SHIFT) \
                                   + ((B) >> USI_LEN_LO_SHIFT))
#define USI_XLEN_PROTOCOL(A, B, C) ((((uint16_t)(A)) << USI_LEN_HI_SHIFT) \
                                 + ((B) >> USI_LEN_LO_SHIFT) \
                                 + (((uint16_t)(C)&USI_XLEN_MSK) << USI_XLEN_SHIFT_L))
#define USI_XLEN2_PROTOCOL(A, B, C) ((((uint16_t)(A)) << USI_LEN_HI_SHIFT) \
                                  + ((B) >> USI_LEN_LO_SHIFT) \
                                  + (((uint16_t)(C)&USI_XLEN2_MSK) << USI_XLEN2_SHIFT_L))
#define USI_LEN_HI_PROTOCOL(A)    (((uint16_t)(A) >> USI_LEN_HI_SHIFT) & USI_LEN_HI_MSK)
#define USI_LEN_LO_PROTOCOL(A)    (((uint16_t)(A) << USI_LEN_LO_SHIFT) & USI_LEN_LO_MSK)
#define USI_LEN_EX_PROTOCOL(A)    (((uint16_t)(A & 0x0c00)) >> 4)
#define USI_CMD_PROTOCOL(A)       ((A)&USI_CMD_MSK)

#define USI_LEN_EX2_PROTOCOL(A)   (((uint16_t)(A & 0x3C00)) >> 6)
#define USI_CMD2_PROTOCOL(A)      ((A)&USI_CMD2_MSK)  

// *****************************************************************************
/* USI Callback Index

  Summary:
    Index to identify callback position in USI callback object.

  Description:
    None.

  Remarks:
    None.
*/

typedef uint8_t SRV_USI_CALLBACK_INDEX;


// *****************************************************************************
/* Invalid USI Callback Index

 Summary:
    Invalid USI Callback Index.

 Description:
    None.

 Remarks:
    None.
*/

#define SRV_USI_CALLBACK_INDEX_INVALID  (((SRV_USI_CALLBACK_INDEX) -1))

// *****************************************************************************
/* USI Callback Object

  Summary:
    Array of callback functions associated to every USI protocol.

  Description:
    There are 10 different USI protocols.

  Remarks:
    None.
*/

typedef SRV_USI_CALLBACK SRV_USI_CALLBACK_OBJ[10];

#endif //#ifndef SRV_USI_LOCAL_H
