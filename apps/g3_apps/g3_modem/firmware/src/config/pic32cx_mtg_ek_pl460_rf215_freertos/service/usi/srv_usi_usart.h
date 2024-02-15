/*******************************************************************************
  USART wrapper used from USI service Header File

  Company
    Microchip Technology Inc.

  File Name
    srv_usi_usart.h

  Summary
    USART wrapper used from USI service interface.

  Description
    The USART wrapper provides a simple interface to manage the USART_PLIB
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

#ifndef SRV_USI_USART_H    // Guards against multiple inclusion
#define SRV_USI_USART_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "system/system.h"
#include "service/usi/srv_usi.h"
#include "osal/osal.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

extern const SRV_USI_DEV_DESC srvUSIUSARTDevDesc;

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

typedef void ( * USI_USART_CALLBACK ) ( uint8_t *data, uint16_t length, uintptr_t context );

typedef struct USI_USART_MSG_tag
{
    uint8_t*                                 pMessage;
    uint8_t*                                 pDataRd;
    size_t                                   length;
    bool                                     inUse;
    struct USI_USART_MSG_tag*                next;
} USI_USART_MSG;

typedef struct
{
    USI_USART_MSG*                           front;
    USI_USART_MSG*                           rear;
} USI_USART_MSG_QUEUE;

typedef enum
{
    USI_USART_IDLE,
    USI_USART_RCV,
    USI_USART_ESC
} USI_USART_STATE;

typedef void (* USI_USART_PLIB_CALLBACK)( uintptr_t context );

typedef void(* USI_USART_PLIB_READ_CALLBACK_REG)(USI_USART_PLIB_CALLBACK callback, uintptr_t context);
typedef bool(* USI_USART_PLIB_WRRD)(void *buffer, const size_t size);
typedef bool(* USI_USART_PLIB_WRITE_ISBUSY)(void);

typedef struct
{
    USI_USART_PLIB_READ_CALLBACK_REG readCallbackRegister;
    USI_USART_PLIB_WRRD readData;
    USI_USART_PLIB_WRRD writeData;
    USI_USART_PLIB_WRITE_ISBUSY writeIsBusy;
    IRQn_Type intSource;
} SRV_USI_USART_INTERFACE;

typedef struct
{
    void*                                    plib;
    void*                                    pRdBuffer;
    size_t                                   rdBufferSize;
} USI_USART_INIT_DATA;

typedef struct
{
    SRV_USI_USART_INTERFACE*                 plib;
    USI_USART_CALLBACK                       cbFunc;
    void*                                    pRdBuffer;
    size_t                                   rdBufferSize;
    size_t                                   byteCount;
    uint8_t                                  rcvChar;
    USI_USART_MSG*                           pRcvMsg;
    USI_USART_MSG_QUEUE*                     pMsgQueue;
    USI_USART_STATE                          devStatus;
    SRV_USI_STATUS                           usiStatus;
    uintptr_t                                context;
    OSAL_SEM_DECLARE(semaphoreID);
} USI_USART_OBJ;

// *****************************************************************************
// *****************************************************************************
// Section: SRV_USI Common Interface Implementation
// *****************************************************************************
// *****************************************************************************

void USI_USART_Initialize(uint32_t index, const void * const initData);

DRV_HANDLE USI_USART_Open(uint32_t index);

void USI_USART_Tasks (uint32_t index);

void USI_USART_Write(uint32_t index, void* pData, size_t length);

bool USI_USART_WriteIsBusy(uint32_t index);

void USI_USART_RegisterCallback(uint32_t index, USI_USART_CALLBACK cbFunc, uintptr_t context);

void USI_USART_Close(uint32_t index);

SRV_USI_STATUS USI_USART_Status(uint32_t index);

#endif //SRV_USI_USART_H
