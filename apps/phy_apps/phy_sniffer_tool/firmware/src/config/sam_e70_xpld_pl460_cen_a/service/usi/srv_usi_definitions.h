/*******************************************************************************
  USI Service Definitions Header File

  Company:
    Microchip Technology Inc.

  File Name:
    srv_usi_definitions.h

  Summary:
    USI Service Definitions Header File

  Description:
    This file provides implementation-specific definitions for the USI
    service's system interface.
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

#ifndef SRV_USI_DEFINITIONS_H
#define SRV_USI_DEFINITIONS_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include "system/dma/sys_dma.h"

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
/* USI Service Serial Protocol Identifiers

  Summary:
    Defines the identifier to use for each USI serial protocol

  Description:
    This data type defines the identifier required to specify the
    serial protocol to use by USI service.

  Remarks:
    Do not rely on the underlying type as it may change in different versions.
*/

typedef enum
{
    /* PRIME Manager generic */
    SRV_USI_PROT_ID_MNGP_PRIME                = 0x00,

    /* PRIME Manager: GETQRY */
    SRV_USI_PROT_ID_MNGP_PRIME_GETQRY         = 0x00,

    /* PRIME Manager: GETRSP */
    SRV_USI_PROT_ID_MNGP_PRIME_GETRSP         = 0x01,

    /* PRIME Manager: SET */
    SRV_USI_PROT_ID_MNGP_PRIME_SET            = 0x02,

    /* PRIME Manager: RESET */
    SRV_USI_PROT_ID_MNGP_PRIME_RESET          = 0x03,

    /* PRIME Manager: REBOOT */
    SRV_USI_PROT_ID_MNGP_PRIME_REBOOT         = 0x04,

    /* PRIME Manager: Firmware Upgrade */
    SRV_USI_PROT_ID_MNGP_PRIME_FU             = 0x05,

    /* PRIME Manager: GETQRY enhanced */
    SRV_USI_PROT_ID_MNGP_PRIME_GETQRY_EN      = 0x06,

    /* PRIME Manager: GETRSP enhanced */
    SRV_USI_PROT_ID_MNGP_PRIME_GETRSP_EN      = 0x07,

    /* PRIME Sniffer */
    SRV_USI_PROT_ID_SNIF_PRIME                = 0x13,

    /* PRIME PHY Serial */
    SRV_USI_PROT_ID_PHY_SERIAL_PRIME          = 0x1F,

    /* Physical Layer  */
    SRV_USI_PROT_ID_PHY                       = 0x22,

    /* G3 Sniffer  */
    SRV_USI_PROT_ID_SNIFF_G3                  = 0x23,

    /* G3 MAC layer  */
    SRV_USI_PROT_ID_MAC_G3                    = 0x24,

    /* G3 ADP layer  */
    SRV_USI_PROT_ID_ADP_G3                    = 0x25,

    /* G3 Coordinator  */
    SRV_USI_PROT_ID_COORD_G3                  = 0x26,

    /* MicroPLC Physical Layer  */
    SRV_USI_PROT_ID_PHY_MICROPLC              = 0x27,

    /* PRIME API  */
    SRV_USI_PROT_ID_PRIME_API                 = 0x30,

    /* Invalid protocol  */
    SRV_USI_PROT_ID_INVALID                   = 0xFF

} SRV_USI_PROTOCOL_ID;

// *****************************************************************************
/* USI Service Error

  Summary:
    Defines the error values

  Description:
    This data type defines the error values for the errors occurred during transfer.

  Remarks:
    None.
*/

typedef enum
{
    /* Error in protocol identifier */
    SRV_USI_PROTOCOL_ERROR,

    /* Protocol is not registered */
    SRV_USI_PROTOCOL_NOT_REGISTERED,

    /* Error in the format of message */
    SRV_USI_FORMAT_ERROR,

    /* Error in PLIB communication */
    SRV_USI_PLIB_ERROR,

    /* Invalid operation */
    SRV_USI_INVALID

} SRV_USI_ERROR;

// *****************************************************************************
/* USI Serive Communication Interface

  Summary:
    Defines the Communication Interface values

  Description:
    This data type defines the Communication Interface values for the connection
    with peripherals.

  Remarks:
    None.
*/

typedef enum
{
    /* Invalid interface API */
    SRV_USI_INVALID_API,

    /* USART interface API */
    SRV_USI_USART_API,
    
    /* USB CDC Class interface API */
    SRV_USI_CDC_API,
    
    /* TCP interface API */
    SRV_USI_TCP_API

} SRV_USI_INTERFACE_API;

// *****************************************************************************
/* USART PLIB API Set needed by USI service */

typedef void (* PLIB_CALLBACK)( uintptr_t context );

typedef void(* USI_USART_PLIB_CALLBACK)( uintptr_t context);
typedef void(* USI_USART_PLIB_READ_CALLBACK_REG)(PLIB_CALLBACK callback, uintptr_t context);
typedef bool(* USI_USART_PLIB_WRRD)(void *buffer, const size_t size);
typedef void(* USI_USART_PLIB_WRITE_CALLBACK_REG)(PLIB_CALLBACK callback, uintptr_t context);
// *****************************************************************************
/* USI Service USART Interface Data

  Summary:
    Defines the data required to initialize the USI service USART Interface.

  Description:
    This data type defines the data required to initialize the USI service USART
    Interface.

  Remarks:
    None.
*/

typedef struct
{
    USI_USART_PLIB_READ_CALLBACK_REG readCallbackRegister;
    USI_USART_PLIB_WRRD read;
    USI_USART_PLIB_WRITE_CALLBACK_REG writeCallbackRegister;
    SYS_DMA_CHANNEL dmaChannelTx;
    void *usartAddressTx;

} SRV_USI_USART_INTERFACE;

// *****************************************************************************
/* USI Service Initialization Data

  Summary:
    Defines the data required to initialize the USI service

  Description:
    This data type defines the data required to initialize or the USI service.
    If the service is built statically, the members of this data structure are
    statically over-ridden by static override definitions in the
    configuration.h file.

  Remarks:
    None.
*/

typedef struct
{
    /* Identifies the interface with peripherals */
    SRV_USI_INTERFACE_API                    usiInterfaceApi;

    /* Identifies the USI interface API with peripherals */
    void*                                    usiApi;

    /* Pointer to the internal read buffer */
    void*                                    readBuffer;

    /* Number of bytes of the buffer to receive serial data */
    size_t                                   readSizeMax;

    /* Pointer to the internal write buffer */
    void*                                    writeBuffer;

    /* Number of bytes of the buffer to send serial data */
    size_t                                   writeSizeMax;   
} SRV_USI_INIT;

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // #ifndef SRV_USI_DEFINITIONS_H
