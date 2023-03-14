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
#include "stddef.h"
#include "configuration.h"
#include "driver/driver_common.h"
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
    .write                      = USI_USART_Write,
    .writeIsBusy                = USI_USART_WriteIsBusy,
    .task                       = USI_USART_Tasks,
    .close                      = USI_USART_Close,
    .status                     = USI_USART_Status,
};

static USI_USART_OBJ gUsiUsartOBJ[SRV_USI_USART_CONNECTIONS] = {0};
static USI_USART_MSG gUsiUsartMsgPool[SRV_USI_MSG_POOL_SIZE] = {0};
static USI_USART_MSG_QUEUE gUsiUsartMsgQueue[SRV_USI_USART_CONNECTIONS] = {NULL};

#define USI_USART_GET_INSTANCE(index)    (index >= SRV_USI_USART_CONNECTIONS)? NULL : &gUsiUsartOBJ[index]

static uint32_t usiUsartCounterDiscardMsg;

// *****************************************************************************
// *****************************************************************************
// Section: File scope functions
// *****************************************************************************
// *****************************************************************************
static USI_USART_MSG* _USI_USART_PUT_MSG_TO_QUEUE( USI_USART_OBJ* dObj )
{
    USI_USART_MSG* pMsg;
    uint8_t index;
    
    /* Get free buffer from POOL */
    for (index = 0; index < SRV_USI_MSG_POOL_SIZE; index++)
    {
        pMsg = &gUsiUsartMsgPool[index];
        if (pMsg->inUse == 0)
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
                dObjQueue->rear->next = (struct USI_USART_MSG*)pMsg;
                dObjQueue->rear = pMsg;
            }           
            
            return pMsg;
        }
    }
    
    return NULL;
}

static void _USI_USART_GET_MSG_FROM_QUEUE( USI_USART_OBJ* dObj )
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

static void _USI_USART_ABORT_MSG_IN_QUEUE( USI_USART_OBJ* dObj )
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

static void _USI_USART_PLIB_CALLBACK( uintptr_t context)
{
    USI_USART_OBJ* dObj;
    USI_USART_MSG* pMsg;
    bool next;
    
    dObj = (USI_USART_OBJ*)context;
    pMsg = dObj->pRcvMsg;
    next = false;
    
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
                pMsg = _USI_USART_PUT_MSG_TO_QUEUE(dObj);
                
                if (pMsg)
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
                usiUsartCounterDiscardMsg = 0x10000;
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
                pMsg->length = pMsg->pDataRd - pMsg->pMessage;
                dObj->pRcvMsg = NULL;
                dObj->devStatus = USI_USART_IDLE;
				
                /* Stop Counter to discard uncompleted Message */
                usiUsartCounterDiscardMsg = 0;
            }              
            else if (dObj->rcvChar == USI_ESC_KEY_7D)
            {
                /* Escape character */
                dObj->devStatus = USI_USART_ESC;
            } 
            else
            {
                /* Store character */
                next = true;
            }
      
            break;
            
        case USI_USART_ESC:
            if (dObj->rcvChar == USI_ESC_KEY_5E)
            {
                /* Store character after escape it */
                *pMsg->pDataRd++ = USI_ESC_KEY_7E;
                dObj->devStatus = USI_USART_RCV;
            }  
            else if (dObj->rcvChar == USI_ESC_KEY_5D)
            {
                /* Store character after escape it */
                *pMsg->pDataRd++ = USI_ESC_KEY_7D;
                dObj->devStatus = USI_USART_RCV;
            }
            else
            {
                /* ERROR: Escape format */
                _USI_USART_ABORT_MSG_IN_QUEUE(dObj);
                dObj->pRcvMsg = NULL;
                dObj->devStatus = USI_USART_IDLE;
            }
      
            break;
    }    
    
    /* Update pointers */
    if (next)
    {
        dObj->byteCount++;
        if (dObj->byteCount == dObj->rdBufferSize)
        {
            /* ERROR: Overflow */
            _USI_USART_ABORT_MSG_IN_QUEUE(dObj);
            dObj->pRcvMsg = NULL;
            dObj->byteCount = 0;
        }
        else
        {
            *pMsg->pDataRd++ = dObj->rcvChar;
        }        
    }
    
    /* Read next char */
    dObj->plib->read(&dObj->rcvChar, 1);
}

// *****************************************************************************
// *****************************************************************************
// Section: USI USART Service Common Interface Implementation
// *****************************************************************************
// *****************************************************************************

DRV_HANDLE USI_USART_Initialize(uint32_t index, const void* initData)
{
    USI_USART_OBJ* dObj = USI_USART_GET_INSTANCE(index);
    USI_USART_INIT_DATA* dObjInit = (USI_USART_INIT_DATA*)initData;
    
    if (dObj == NULL)
    {
        return DRV_HANDLE_INVALID;
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

    return (DRV_HANDLE)index;
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

size_t USI_USART_Write(uint32_t index, void* pData, size_t length)
{
    USI_USART_OBJ* dObj = USI_USART_GET_INSTANCE(index);
    
    /* Check handler */
    if (dObj == NULL)
    {
        return 0;
    }

    if (length == 0)
    {
        return 0;
    }
    
    if (dObj->usiStatus != SRV_USI_STATUS_CONFIGURED)
    {
        return 0;
    }    

    dObj->plib->write(pData, length);
    
    return length;
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
    dObj->plib->readCallbackRegister(_USI_USART_PLIB_CALLBACK, (uintptr_t)dObj);
    
    /* Set callback function */
    dObj->cbFunc = cbFunc;
    
    /* Set context related to cbFunc */
    dObj->context = context;
    
    /* Launch reception */
    dObj->plib->read(&dObj->rcvChar, 1);
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

    if (dObj->usiStatus != SRV_USI_STATUS_CONFIGURED)
    {
        return;
    }
    
    if (usiUsartCounterDiscardMsg)
    {
        if (--usiUsartCounterDiscardMsg == 0)
        {
            /* Discard incomplete message */
            _USI_USART_ABORT_MSG_IN_QUEUE(dObj);
            dObj->pRcvMsg = NULL;
            dObj->devStatus = USI_USART_IDLE;
        }
    }
    
    /* Handle callback functions */
    pMsg = dObj->pMsgQueue->front;
    if ((pMsg != NULL) && (pMsg != dObj->pRcvMsg))
    {        
        if ((dObj->cbFunc) && (pMsg->length > 0))
        {
            dObj->cbFunc(pMsg->pMessage, pMsg->length, dObj->context);
        }

        /* Critical Section */
        /* Save global interrupt state and disable interrupt */
        aSrcId = (INT_SOURCE)dObj->plib->intSource;
        interruptState = SYS_INT_SourceDisable(aSrcId);

        /* Remove Message from Queue */
        _USI_USART_GET_MSG_FROM_QUEUE(dObj);
        
        /* Restore interrupt state */
        SYS_INT_SourceRestore(aSrcId, interruptState);
    }
}
