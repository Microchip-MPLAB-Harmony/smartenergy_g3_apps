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
#include "service/usi/srv_usi.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

extern const SRV_USI_DEV_DESC srvUSICDCDevDesc;

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

typedef void (* USI_CDC_PLIB_CALLBACK)( uintptr_t context );

typedef void(* USI_CDC_PLIB_READ_CALLBACK_REG)(USI_CDC_PLIB_CALLBACK callback, uintptr_t context);
typedef bool(* USI_CDC_PLIB_WRRD)(void *buffer, const size_t size);
typedef bool(* USI_CDC_PLIB_WRITE_ISBUSY)(void);

typedef struct
{
    USI_CDC_PLIB_READ_CALLBACK_REG readCallbackRegister;
    USI_CDC_PLIB_WRRD readData;
    USI_CDC_PLIB_WRRD writeData;
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
    volatile uint32_t                        cdcNumBytesRead;
    size_t                                   usiBufferSize;
    size_t                                   usiNumBytesRead;
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

void USI_CDC_Initialize(uint32_t index, const void * const initData);

DRV_HANDLE USI_CDC_Open(uint32_t index);

void USI_CDC_Tasks (uint32_t index);

void USI_CDC_Write(uint32_t index, void* pData, size_t length);

void USI_CDC_RegisterCallback(uint32_t index, USI_CDC_CALLBACK cbFunc, uintptr_t context);

void USI_CDC_Close(uint32_t index);

SRV_USI_STATUS USI_CDC_Status(uint32_t index);

#endif //SRV_USI_CDC_H
