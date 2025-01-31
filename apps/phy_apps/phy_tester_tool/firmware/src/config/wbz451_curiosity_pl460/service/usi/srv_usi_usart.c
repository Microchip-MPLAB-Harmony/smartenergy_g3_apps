/*******************************************************************************
  USART wrapper used from USI service Implementation

  Company:
    Microchip Technology Inc.

  File Name:
    srv_usi_usart.c

  Summary:
    USART wrapper used from USI service implementation.

  Description:
    The USART wrapper provides a simple interface to manage the USART_PLIB
    module on Microchip microcontrollers. This file implements the core
    interface routines for the USI PLC service.
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
// *****************************************************************************
// *****************************************************************************
// Section: Macro Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <stddef.h>
#include "configuration.h"
#include "driver/driver_common.h"
#include "system/int/sys_int.h"
#include "srv_usi_local.h"
#include "srv_usi_usart.h"
#include "srv_usi_definitions.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data
// *****************************************************************************
// *****************************************************************************
/* This is the service instance object array. */
const SRV_USI_DEV_DESC srvUSIUSARTDevDesc =
{
    .init                       = USI_USART_Initialize,
    .open                       = USI_USART_Open,
    .setReadCallback            = USI_USART_RegisterCallback,
    .writeData                  = USI_USART_Write,
    .task                       = USI_USART_Tasks,
    .close                      = USI_USART_Close,
    .status                     = USI_USART_Status,
};

static USI_USART_OBJ gUsiUsartOBJ[SRV_USI_USART_CONNECTIONS] = {0};

#define USI_USART_GET_INSTANCE(index)    (((index) >= SRV_USI_USART_CONNECTIONS)? NULL : &gUsiUsartOBJ[index])

// *****************************************************************************
// *****************************************************************************
// Section: File scope functions
// *****************************************************************************
// *****************************************************************************

static void lUSI_USART_TransferReceivedData(USI_USART_OBJ* dObj, size_t bytesRcv)
{
    size_t numByte;

    for(numByte = 0; numByte < bytesRcv; numByte++)
    {
        uint8_t rcvChar = dObj->usartReadBuffer[numByte];

        switch (dObj->devStatus)
        {
            case USI_USART_IDLE:
                /* Waiting to MSG KEY */
                if ( rcvChar == USI_ESC_KEY_7E)
                {
                    /* Reset counter bytes received */
                    dObj->byteCount = 0;

                    /* New Message, start reception */
                    dObj->devStatus = USI_USART_RCV;
                }
                break;

            case USI_USART_RCV:

                if (rcvChar == USI_ESC_KEY_7E)
                {
                    if (dObj->byteCount == 0U)
                    {
                        /* Two consecutive 7E, synchronizing with the begin
                          of the message*/
                        break;
                    }

                    /* End of Message */
                     dObj->cbFunc(dObj->pRdBuffer, dObj->byteCount, dObj->context);

                    dObj->devStatus = USI_USART_IDLE;

                    /* Stop Counter to discard uncompleted Message */
                    dObj->byteCount = 0;
                }
                else if (rcvChar == USI_ESC_KEY_7D)
                {
                    /* Escape character */
                    dObj->devStatus = USI_USART_ESC;
                }
                else
                {
                    /* Store character */
                    dObj->pRdBuffer[dObj->byteCount++] = rcvChar;
                }

                break;

            case USI_USART_ESC:
            default:
                if (rcvChar == USI_ESC_KEY_5E)
                {
                    /* Store character after escape it */
                    dObj->pRdBuffer[dObj->byteCount++] = USI_ESC_KEY_7E;
                    dObj->devStatus = USI_USART_RCV;
                }
                else if (rcvChar == USI_ESC_KEY_5D)
                {
                    /* Store character after escape it */
                    dObj->pRdBuffer[dObj->byteCount++] = USI_ESC_KEY_7D;
                    dObj->devStatus = USI_USART_RCV;
                }
                else
                {
                    /* ERROR: Escape format, discard message */
                    dObj->byteCount = 0;
                    dObj->devStatus = USI_USART_IDLE;
                }

                break;

        }
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: USI USART Service Common Interface Implementation
// *****************************************************************************
// *****************************************************************************

void USI_USART_Initialize(uint32_t index, const void * const initData)
{
    USI_USART_OBJ* dObj = USI_USART_GET_INSTANCE(index);
    const USI_USART_INIT_DATA * const dObjInit = (const USI_USART_INIT_DATA * const)initData;

    if (dObj == NULL)
    {
        return;
    }

    dObj->plib = (SRV_USI_USART_INTERFACE*)dObjInit->plib;
    dObj->pRdBuffer = dObjInit->pRdBuffer;
    dObj->rdBufferSize = dObjInit->rdBufferSize;
    dObj->usartBufferSize = dObjInit->usartBufferSize;
    dObj->usartReadBuffer = dObjInit->usartReadBuffer;

    dObj->byteCount = 0;
    dObj->cbFunc = NULL;
    dObj->devStatus = USI_USART_IDLE;
    dObj->usiStatus = SRV_USI_STATUS_NOT_CONFIGURED;
}

DRV_HANDLE USI_USART_Open(uint32_t index)
{
    USI_USART_OBJ* dObj = USI_USART_GET_INSTANCE(index);

    if (dObj == NULL)
    {
        return DRV_HANDLE_INVALID;
    }

    dObj->usiStatus = SRV_USI_STATUS_CONFIGURED;

    return (DRV_HANDLE)index;
}

void USI_USART_Write(uint32_t index, void* pData, size_t length)
{
    USI_USART_OBJ* dObj = USI_USART_GET_INSTANCE(index);

    /* Check handler */
    if (dObj == NULL)
    {
        return;
    }

    if (length == 0U)
    {
        return;
    }

    if (dObj->usiStatus != SRV_USI_STATUS_CONFIGURED)
    {
        return;
    }

    (void) dObj->plib->writeData(pData, length);
}

void USI_USART_RegisterCallback(uint32_t index, USI_USART_CALLBACK cbFunc,
        uintptr_t context)
{
    USI_USART_OBJ* dObj = USI_USART_GET_INSTANCE(index);

    /* Check handler */
    if (dObj == NULL)
    {
        return;
    }

    if (dObj->usiStatus != SRV_USI_STATUS_CONFIGURED)
    {
        return;
    }

    /* Set callback function */
    dObj->cbFunc = cbFunc;

    /* Set context related to cbFunc */
    dObj->context = context;

}

void USI_USART_Close(uint32_t index)
{
    USI_USART_OBJ* dObj = USI_USART_GET_INSTANCE(index);

    /* Check handler */
    if (dObj == NULL)
    {
        return;
    }

    dObj->usiStatus = SRV_USI_STATUS_NOT_CONFIGURED;
}

SRV_USI_STATUS USI_USART_Status(uint32_t index)
{
    USI_USART_OBJ* dObj = USI_USART_GET_INSTANCE(index);

    /* Check handler */
    if (dObj == NULL)
    {
        return SRV_USI_STATUS_ERROR;
    }

    return dObj->usiStatus;
}

void USI_USART_Tasks (uint32_t index)
{
    USI_USART_OBJ* dObj = USI_USART_GET_INSTANCE(index);
    size_t bytesRcv;

    /* Check handler */
    if (dObj == NULL)
    {
        return;
    }

    if (dObj->usiStatus != SRV_USI_STATUS_CONFIGURED)
    {
        return;
    }

     /* Ring mode USART reception process */
    bytesRcv = dObj->plib->readData(dObj->usartReadBuffer, dObj->usartBufferSize);

    if (bytesRcv != 0U)
    {
        lUSI_USART_TransferReceivedData(dObj, bytesRcv);
    }
}
