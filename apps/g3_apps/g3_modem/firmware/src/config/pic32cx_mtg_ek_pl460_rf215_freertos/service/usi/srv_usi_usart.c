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
    .writeIsBusy                = USI_USART_WriteIsBusy,
    .task                       = USI_USART_Tasks,
    .close                      = USI_USART_Close,
    .status                     = USI_USART_Status,
};

static USI_USART_OBJ gUsiUsartOBJ[SRV_USI_USART_CONNECTIONS] = {0};
static USI_USART_MSG gUsiUsartMsgPool[SRV_USI_MSG_POOL_SIZE] = {0};
static USI_USART_MSG_QUEUE gUsiUsartMsgQueue[SRV_USI_USART_CONNECTIONS] = {NULL};

#define USI_USART_GET_INSTANCE(index)    (((index) >= SRV_USI_USART_CONNECTIONS)? NULL : &gUsiUsartOBJ[index])

static uint32_t usiUsartCounterDiscardMsg;

// *****************************************************************************
// *****************************************************************************
// Section: File scope functions
// *****************************************************************************
// *****************************************************************************

static USI_USART_MSG* lUSI_USART_PutMsgToQueue( USI_USART_OBJ* dObj )
{
    USI_USART_MSG* pMsg;
    uint8_t index;

    /* Get free buffer from POOL */
    for (index = 0; index < SRV_USI_MSG_POOL_SIZE; index++)
    {
        pMsg = &gUsiUsartMsgPool[index];
        if (pMsg->inUse == false)
        {
            USI_USART_MSG_QUEUE* dObjQueue;

            /* Initialize Message data */
            pMsg->inUse = 1;
            pMsg->next = NULL;

            /* Update queue */
            dObjQueue = dObj->pMsgQueue;
            if (dObjQueue->rear == NULL)
            {   /* Queue is empty */
                dObjQueue->front = dObjQueue->rear = pMsg;
            }
            else
            {   /* Queue is not empty. Add new msg */
                dObjQueue->rear->next = (struct USI_USART_MSG_tag*)pMsg;
                dObjQueue->rear = pMsg;
            }

            return pMsg;
        }
    }

    return NULL;
}

static void lUSI_USART_GetMsgFromQueue( USI_USART_OBJ* dObj )
{
    USI_USART_MSG_QUEUE* dObjQueue;

    /* Get Queue */
    dObjQueue = dObj->pMsgQueue;

    if (dObjQueue->front == NULL)
    {   /* Queue is empty */
        return;
    }

    /* Update buffer content */
    dObjQueue->front->inUse = 0;

    /* Update queue front */
    dObjQueue->front = (USI_USART_MSG*)dObjQueue->front->next;

    if (dObjQueue->front == NULL)
    {   /* There isn't anymore elements */
        dObjQueue->rear = NULL;
    }
}

static void lUSI_USART_AbortMsgInQueue( USI_USART_OBJ* dObj )
{
    USI_USART_MSG_QUEUE* dObjQueue;
    USI_USART_MSG* pMsgTmp;
    USI_USART_MSG* pMsgPrev;

    /* Get Queue */
    dObjQueue = dObj->pMsgQueue;

    /* Get first element in queue */
    pMsgTmp = dObjQueue->front;

    if (pMsgTmp == dObj->pRcvMsg)
    {
        /* Empty queue */
        /* Update buffer in use. Return it to pool */
        pMsgTmp->inUse = false;
        dObjQueue->front = NULL;
        dObjQueue->rear = NULL;
        return;
    }

    /* Get Prev message in queue */
    pMsgPrev = pMsgTmp;

    /* Check next element */
    pMsgTmp = (USI_USART_MSG*)pMsgTmp->next;

    while (pMsgTmp != NULL)
    {
        if (pMsgTmp == dObj->pRcvMsg)
        {
            /* Found message to be discarded */
            /* Update buffer in use. Return it to pool */
            pMsgTmp->inUse = false;
            /* Update last link */
            pMsgTmp->next = NULL;
            /* Update queue to the previous element */
            dObjQueue->rear = pMsgPrev;
            return;
        }
        /* Get Prev message in queue */
        pMsgPrev = pMsgTmp;
        /* Check next element */
        pMsgTmp = (USI_USART_MSG*)pMsgTmp->next;
    }
}

static void lUSI_USART_PlibCallback( uintptr_t context)
{
    USI_USART_OBJ* dObj;
    USI_USART_MSG* pMsg;
    bool store;
    uint8_t charStore;

    dObj = (USI_USART_OBJ*)context;
    pMsg = dObj->pRcvMsg;
    store = false;
    charStore = 0;

    switch(dObj->devStatus)
    {
        case USI_USART_IDLE:
            /* Waiting to MSG KEY */
            if (dObj->rcvChar == USI_ESC_KEY_7E)
            {
                uint8_t *pData;

                /* Restart Byte Counter */
                if (dObj->pMsgQueue->front == NULL)
                {
                    /* Not received anymore characters: Restart reception buffer */
                    dObj->byteCount = 0;
                }

                /* Create new message */
                pMsg = lUSI_USART_PutMsgToQueue(dObj);

                if (pMsg != NULL)
                {
                    /* Fill in the message */
                    pData = (uint8_t *)(dObj->pRdBuffer);
                    pMsg->pMessage = pData + dObj->byteCount;
                    pMsg->pDataRd = pMsg->pMessage;
                    pMsg->length = 0;

                    /* Update Msg in reception data */
                    dObj->pRcvMsg = pMsg;
                }

                /* New Message, start reception */
                dObj->devStatus = USI_USART_RCV;
                /* Start Counter to discard uncompleted Message */
                usiUsartCounterDiscardMsg = 0x100;

                /* Post semaphore to resume task */
                if (dObj->semaphoreID != NULL)
                {
                    (void) OSAL_SEM_PostISR(&dObj->semaphoreID);
                }
            }
            break;

        case USI_USART_RCV:
            if (dObj->pRcvMsg == NULL)
            {
                if (dObj->rcvChar == USI_ESC_KEY_7E)
                {
                    dObj->devStatus = USI_USART_IDLE;
                }
                break;
            }

            if (dObj->rcvChar == USI_ESC_KEY_7E)
            {
                /* End of Message */
                ptrdiff_t size = pMsg->pDataRd - pMsg->pMessage;
                pMsg->length = (size_t)size;
                dObj->pRcvMsg = NULL;
                dObj->devStatus = USI_USART_IDLE;

                /* Stop Counter to discard uncompleted Message */
                usiUsartCounterDiscardMsg = 0;

                /* Post semaphore to resume task */
                if (dObj->semaphoreID != NULL)
                {
                    (void) OSAL_SEM_PostISR(&dObj->semaphoreID);
                }
            }
            else if (dObj->rcvChar == USI_ESC_KEY_7D)
            {
                /* Escape character */
                dObj->devStatus = USI_USART_ESC;
            }
            else
            {
                /* Store character */
                store = true;
                charStore = dObj->rcvChar;
            }

            break;

        case USI_USART_ESC:
        default:
            if (dObj->rcvChar == USI_ESC_KEY_5E)
            {
                /* Store character after escape it */
                store = true;
                charStore = USI_ESC_KEY_7E;
                dObj->devStatus = USI_USART_RCV;
            }
            else if (dObj->rcvChar == USI_ESC_KEY_5D)
            {
                /* Store character after escape it */
                store = true;
                charStore = USI_ESC_KEY_7D;
                dObj->devStatus = USI_USART_RCV;
            }
            else
            {
                /* ERROR: Escape format */
                lUSI_USART_AbortMsgInQueue(dObj);
                dObj->pRcvMsg = NULL;
                dObj->devStatus = USI_USART_IDLE;
            }

            break;
    }

    /* Update pointers */
    if (store)
    {
        dObj->byteCount++;
        if (dObj->byteCount > dObj->rdBufferSize)
        {
            /* ERROR: Overflow */
            lUSI_USART_AbortMsgInQueue(dObj);
            dObj->pRcvMsg = NULL;
            dObj->byteCount = 0;
        }
        else
        {
            *pMsg->pDataRd++ = charStore;
        }
    }

    /* Read next char */
    (void) dObj->plib->readData(&dObj->rcvChar, 1);
}

// *****************************************************************************
// *****************************************************************************
// Section: USI USART Service Common Interface Implementation
// *****************************************************************************
// *****************************************************************************

void USI_USART_Initialize(uint32_t index, const void * const initData)
{
    OSAL_RESULT semResult;
    USI_USART_OBJ* dObj = USI_USART_GET_INSTANCE(index);
    const USI_USART_INIT_DATA * const dObjInit = (const USI_USART_INIT_DATA * const)initData;

    if (dObj == NULL)
    {
        return;
    }

    dObj->plib = (SRV_USI_USART_INTERFACE*)dObjInit->plib;
    dObj->pRdBuffer = dObjInit->pRdBuffer;
    dObj->rdBufferSize = dObjInit->rdBufferSize;

    dObj->pRcvMsg = NULL;
    dObj->pMsgQueue = &gUsiUsartMsgQueue[index];
    dObj->byteCount = 0;
    dObj->cbFunc = NULL;
    dObj->devStatus = USI_USART_IDLE;
    dObj->usiStatus = SRV_USI_STATUS_NOT_CONFIGURED;

    /* Create semaphore. It is used to suspend and resume task. */
    semResult = OSAL_SEM_Create(&dObj->semaphoreID, OSAL_SEM_TYPE_BINARY, 0, 0);
    if ((semResult != OSAL_RESULT_SUCCESS) || (dObj->semaphoreID == NULL))
    {
        /* Error: Not enough memory to create semaphore */
        dObj->usiStatus = SRV_USI_STATUS_ERROR;
    }
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

bool USI_USART_WriteIsBusy(uint32_t index)
{
    USI_USART_OBJ* dObj = USI_USART_GET_INSTANCE(index);

    /* Check handler */
    if (dObj == NULL)
    {
        return false;
    }

    if (dObj->usiStatus != SRV_USI_STATUS_CONFIGURED)
    {
        return false;
    }

    return dObj->plib->writeIsBusy();
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

    /* Set USART PLIB handler */
    dObj->plib->readCallbackRegister(lUSI_USART_PlibCallback, (uintptr_t)dObj);

    /* Set callback function */
    dObj->cbFunc = cbFunc;

    /* Set context related to cbFunc */
    dObj->context = context;

    /* Launch reception */
    (void) dObj->plib->readData(&dObj->rcvChar, 1);
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
    USI_USART_MSG* pMsg;
    bool interruptState;
    INT_SOURCE aSrcId;

    /* Check handler */
    if (dObj == NULL)
    {
        return;
    }

    /* Suspend task until semaphore is posted or timeout expires */
    if (dObj->semaphoreID != NULL)
    {
        uint16_t waitMS = 1;

        /* If counter to discard message is active, wait for 1 ms. Otherwise, wait forever. */
        if (usiUsartCounterDiscardMsg == 0U)
        {
            waitMS = (uint16_t)OSAL_WAIT_FOREVER;
        }

        (void) OSAL_SEM_Pend(&dObj->semaphoreID, waitMS);
    }

    if (dObj->usiStatus != SRV_USI_STATUS_CONFIGURED)
    {
        return;
    }

    if (usiUsartCounterDiscardMsg > 0U)
    {
        if (--usiUsartCounterDiscardMsg == 0U)
        {
            /* Discard incomplete message */
            lUSI_USART_AbortMsgInQueue(dObj);
            dObj->pRcvMsg = NULL;
            dObj->devStatus = USI_USART_IDLE;
        }
    }

    /* Handle callback functions */
    pMsg = dObj->pMsgQueue->front;
    if ((pMsg != NULL) && (pMsg != dObj->pRcvMsg))
    {
        if ((dObj->cbFunc != NULL) && (pMsg->length > 0U))
        {
            dObj->cbFunc(pMsg->pMessage, pMsg->length, dObj->context);
        }

        /* Critical Section */
        /* Save global interrupt state and disable interrupt */
        aSrcId = (INT_SOURCE)dObj->plib->intSource;
        interruptState = SYS_INT_SourceDisable(aSrcId);

        /* Remove Message from Queue */
        lUSI_USART_GetMsgFromQueue(dObj);

        /* Restore interrupt state */
        SYS_INT_SourceRestore(aSrcId, interruptState);
    }
}
