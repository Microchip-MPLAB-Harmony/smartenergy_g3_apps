/*******************************************************************************
  USB CDC wrapper used from USI service Header File

  Company
    Microchip Technology Inc.

  File Name
    srv_usi_usart.h

  Summary
    USB CDC wrapper used from USI service interface.

  Description
    The USB CDC wrapper provides a simple interface to manage the USB
    module on Microchip microcontrollers. This file implements the core
    interface routines for the USI PLC service.

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

#ifndef SRV_USI_CDC_H    // Guards against multiple inclusion
#define SRV_USI_CDC_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "system/system.h"
#include "usb/usb_device_cdc.h"
#include "service/usi/srv_usi_definitions.h"

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

typedef void ( * USI_CDC_CALLBACK ) ( uint8_t *data, uint16_t length, uintptr_t context );

typedef enum
{
    USI_CDC_IDLE,
    USI_CDC_RCV,
    USI_CDC_ESC
} USI_CDC_STATE;

typedef void (* PLIB_CALLBACK)( uintptr_t context );

typedef void(* USI_CDC_PLIB_READ_CALLBACK_REG)(PLIB_CALLBACK callback, uintptr_t context);
typedef bool(* USI_CDC_PLIB_WRRD)(void *buffer, const size_t size);
typedef bool(* USI_CDC_PLIB_WRITE_ISBUSY)(void);

typedef struct
{
    USI_CDC_PLIB_READ_CALLBACK_REG readCallbackRegister;
    USI_CDC_PLIB_WRRD read;
    USI_CDC_PLIB_WRRD write;
    USI_CDC_PLIB_WRITE_ISBUSY writeIsBusy;
} SRV_USI_CDC_INTERFACE;

typedef struct
{
    uint8_t                                  cdcInstanceIndex;
    uint8_t*                                 cdcReadBuffer;
    uint8_t*                                 usiReadBuffer;
    size_t                                   cdcBufferSize;
    size_t                                   usiBufferSize;
} USI_CDC_INIT_DATA; 

typedef struct
{
    uint8_t                                  cdcInstanceIndex;
    USB_DEVICE_HANDLE                        devHandle;
    bool                                     sofEventHasOccurred;
    USB_CDC_LINE_CODING                      getLineCodingData;
    USB_CDC_LINE_CODING                      setLineCodingData;
    USB_CDC_CONTROL_LINE_STATE               controlLineStateData;
    uint16_t                                 breakData;
    USB_DEVICE_CDC_TRANSFER_HANDLE           readTransferHandle;
    USB_DEVICE_CDC_TRANSFER_HANDLE           writeTransferHandle;
    
    uint8_t*                                 cdcReadBuffer;
    uint8_t*                                 usiReadBuffer;
    uint8_t*                                 usiRdInIndex;
    uint8_t*                                 usiRdOutIndex;
    uint8_t*                                 usiEndIndex;
    volatile uint32_t                        cdcNumBytesRead;
    uint32_t                                 usiNumBytesRead;
    size_t                                   cdcBufferSize;
    volatile bool                            cdcIsReadComplete;
    
    USI_CDC_CALLBACK                         cbFunc;
    USI_CDC_STATE                            devStatus;
    SRV_USI_STATUS                           usiStatus;
    uintptr_t                                context;
} USI_CDC_OBJ;
        
// *****************************************************************************
// *****************************************************************************
// Section: SRV_USI Common Interface Implementation
// *****************************************************************************
// *****************************************************************************

DRV_HANDLE USI_CDC_Initialize(uint32_t index, const void* initData);

DRV_HANDLE USI_CDC_Open(uint32_t index);

void USI_CDC_Tasks (uint32_t index);

size_t USI_CDC_Write(uint32_t index, void* pData, size_t length);

void USI_CDC_RegisterCallback(uint32_t index, USI_CDC_CALLBACK cbFunc, uintptr_t context);

void USI_CDC_Close(uint32_t index);

SRV_USI_STATUS USI_CDC_Status(uint32_t index);

#endif //SRV_USI_CDC_H
