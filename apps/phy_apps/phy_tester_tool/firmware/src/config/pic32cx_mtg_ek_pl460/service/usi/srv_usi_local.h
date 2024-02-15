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
/*
Copyright (C) 2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
#define USI_ESC_KEY_7E                0x7EU
#define USI_ESC_KEY_5E                0x5EU
#define USI_ESC_KEY_7D                0x7DU
#define USI_ESC_KEY_5D                0x5DU

#define USI_TYPE_OFFSET               1U
#define USI_TYPE_MSK                  0x3FU
#define USI_LEN_HI_OFFSET             0U
#define USI_LEN_HI_MSK                0xFFU
#define USI_LEN_HI_SHIFT              2U
#define USI_LEN_LO_OFFSET             1U
#define USI_LEN_LO_MSK                0xC0U
#define USI_LEN_LO_SHIFT              6U
#define USI_XLEN_OFFSET               2U
#define USI_XLEN_MSK                  0x80U
#define USI_XLEN_SHIFT_L              3U
#define USI_XLEN_SHIFT_R              10U
#define USI_XLEN2_OFFSET              2U
#define USI_XLEN2_MSK                 0xF0U
#define USI_XLEN2_SHIFT_L             6U
#define USI_XLEN2_SHIFT_R             6U
#define USI_PAYLOAD_OFFSET            2U
#define USI_CMD_OFFSET                2U
#define USI_CMD_MSK                   0x7FU
#define USI_CMD2_MSK                  0x0FU

#define USI_TYPE_PROTOCOL(A)        ((A) & USI_TYPE_MSK)
#define USI_LEN_PROTOCOL(A, B)      ((((uint16_t)(A)) << USI_LEN_HI_SHIFT) \
                                     + ((B) >> USI_LEN_LO_SHIFT))
#define USI_XLEN_PROTOCOL(A, B, C)  ((((uint16_t)(A)) << USI_LEN_HI_SHIFT) \
                                     + ((B) >> USI_LEN_LO_SHIFT) \
                                     + (((uint16_t)(C) & USI_XLEN_MSK) << USI_XLEN_SHIFT_L))
#define USI_XLEN2_PROTOCOL(A, B, C) ((((uint16_t)(A)) << USI_LEN_HI_SHIFT) \
                                     + ((B) >> USI_LEN_LO_SHIFT) \
                                     + (((uint16_t)(C) & USI_XLEN2_MSK) << USI_XLEN2_SHIFT_L))
#define USI_LEN_HI_PROTOCOL(A)      (uint8_t)(((A) >> USI_LEN_HI_SHIFT) & USI_LEN_HI_MSK)
#define USI_LEN_LO_PROTOCOL(A)      (uint8_t)(((A) << USI_LEN_LO_SHIFT) & USI_LEN_LO_MSK)
#define USI_LEN_EX_PROTOCOL(A)      (uint8_t)((((A) & 0x0400U)) >> 3)
#define USI_CMD_PROTOCOL(A)         (uint8_t)((A) & USI_CMD_MSK)

#define USI_LEN_EX2_PROTOCOL(A)     (uint8_t)((((A) & 0x3C00U)) >> 6)
#define USI_CMD2_PROTOCOL(A)        (uint8_t)((A) & USI_CMD2_MSK)

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
/* USI Service Instance Object

  Summary:
    Object used to keep any data required for each instance of the USI Service.

  Description:
    Object used to keep any data required for each instance of the USI Service.

  Remarks:
    None.
*/

typedef struct
{
    /* State of this instance */
    SRV_USI_STATUS                           status;

    /* Device Descriptor */
    const SRV_USI_DEV_DESC*                  devDesc;

    /* Device index */
    uint32_t                                 devIndex;

    /* Identifies the USI callback object */
    SRV_USI_CALLBACK*                        callback;

    /* Pointer to data buffer used to send messages */
    void*                                    pWrBuffer;

    /* Max size of the write buffer */
    size_t                                   wrBufferSize;

} SRV_USI_OBJ;

#endif //#ifndef SRV_USI_LOCAL_H
