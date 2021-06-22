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
#include "system/dma/sys_dma.h"
#include "srv_usi_local.h"
#include "srv_usi_usart.h"
#include "srv_usi_definitions.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data
// *****************************************************************************
// *****************************************************************************
/* This is the service instance object array. */
static USI_USART_OBJ gUsiUsartOBJ[SRV_USI_USART_CONNECTIONS] = {NULL};
static USI_USART_MSG gUsiUsartMsgPool[SRV_USI_MSG_POOL_SIZE] = {0};
static USI_USART_MSG_QUEUE gUsiUsartMsgQueue[SRV_USI_USART_CONNECTIONS] = {NULL};

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
    
    /* Get Queue */
    dObjQueue = dObj->pMsgQueue;
    
    if (dObjQueue->front == NULL)
    {   /* Queue is empty */
        return;
    }
    
    /* Find previous element in queue */
    pMsgTmp = dObjQueue->front;
    while (pMsgTmp != NULL)
    {
        if (pMsgTmp->next == (struct USI_USART_MSG*)dObjQueue->rear)
        {
            /* Update buffer in use. Return it to pool */
            dObjQueue->rear->inUse = false;
            /* Update last link */
            pMsgTmp->next = NULL;
            /* Update queue to the previous element */
            dObjQueue->rear = pMsgTmp;            
            return;
        }
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
    
    switch(dObj->status)
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
                
                /* Fill in the message */  
                pData = (uint8_t *)(dObj->pRdBuffer);
                pMsg->pMessage = pData + dObj->byteCount;
                pMsg->pDataRd = pMsg->pMessage;
                pMsg->length = 0;    
                
                /* Update Msg in reception data */
                dObj->pRcvMsg = pMsg;
                        
                /* New Message, start reception */
                dObj->status = USI_USART_RCV;
            }            
            break;
            
        case USI_USART_RCV:
            if (dObj->rcvChar == USI_ESC_KEY_7E)
            {
                /* End of Message */
                pMsg->length = pMsg->pDataRd - pMsg->pMessage;
                dObj->pRcvMsg = NULL;
                dObj->status = USI_USART_IDLE;
            }              
            else if (dObj->rcvChar == USI_ESC_KEY_7D)
            {
                /* Escape character */
                dObj->status = USI_USART_ESC;
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
                dObj->status = USI_USART_RCV;
            }  
            else if (dObj->rcvChar == USI_ESC_KEY_5D)
            {
                /* Store character after escape it */
                *pMsg->pDataRd++ = USI_ESC_KEY_7D;
                dObj->status = USI_USART_RCV;
            }
            else
            {
                /* ERROR: Escape format */
                _USI_USART_ABORT_MSG_IN_QUEUE(dObj);
                dObj->pRcvMsg = NULL;
                dObj->status = USI_USART_IDLE;
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

static USI_USART_OBJ* _USI_USART_GetFreeUsartObj(void)
{
    uint8_t index;
    
    for(index=0; index < SRV_USI_USART_CONNECTIONS; index++)
    {
        if (gUsiUsartOBJ[index].plib == NULL) {
            return &gUsiUsartOBJ[index];
        }
    }
    
    return NULL;
}

static USI_USART_OBJ* _USI_USART_CheckHandler(DRV_HANDLE handle)
{
    uint8_t index;
    
    for(index=0; index < SRV_USI_USART_CONNECTIONS; index++)
    {
        if ((DRV_HANDLE)(&gUsiUsartOBJ[index]) == handle) {
            return &gUsiUsartOBJ[index];
        }
    }
    
    return NULL;
}

// *****************************************************************************
// *****************************************************************************
// Section: USI USART Service Common Interface Implementation
// *****************************************************************************
// *****************************************************************************

DRV_HANDLE USI_USART_Initialize( const USI_USART_INIT* const init )
{
    USI_USART_INIT* dObjInit;
    USI_USART_OBJ* dObj;
    
    /* Find Free USI USART instance */
    dObj = _USI_USART_GetFreeUsartObj();
    
    if (dObj == NULL)
    {
        return DRV_HANDLE_INVALID;
    }
    
    /* Initialize USI USART instance */
    dObjInit = (USI_USART_INIT*)init;
    
    dObj->plib = (SRV_USI_USART_INTERFACE*)dObjInit->plib;
    dObj->pRdBuffer = dObjInit->pRdBuffer;
    dObj->rdBufferSize = dObjInit->rdBufferSize;
    dObj->pWrBuffer = dObjInit->pWrBuffer;
    dObj->wrBufferSize = dObjInit->wrBufferSize;
    
    dObj->pRcvMsg = NULL;
    dObj->pMsgQueue = gUsiUsartMsgQueue;
    dObj->byteCount = 0;
    dObj->cbFunc = NULL;
    dObj->status = USI_USART_IDLE;
    
    /* Set USART PLIB handler */
    dObj->plib->readCallbackRegister(_USI_USART_PLIB_CALLBACK, (uintptr_t)dObj);

    /* Configure DMA for USART */
    SYS_DMA_DataWidthSetup(dObj->plib->dmaChannelTx, SYS_DMA_WIDTH_8_BIT);
    SYS_DMA_AddressingModeSetup(dObj->plib->dmaChannelTx, 
            SYS_DMA_SOURCE_ADDRESSING_MODE_INCREMENTED, 
            SYS_DMA_DESTINATION_ADDRESSING_MODE_FIXED);

    return (DRV_HANDLE)dObj;
}

size_t USI_USART_Write( DRV_HANDLE handle, size_t length )
{
    USI_USART_OBJ* dObj;
    
    /* Check handler */
    dObj = _USI_USART_CheckHandler(handle);
    if (dObj == NULL)
    {
        /* Not valid handler */
        return 0;
    }
    
    if (length == 0)
    {
        return 0;
    }

    /* Waiting for USART is free */
    while (SYS_DMA_ChannelIsBusy(dObj->plib->dmaChannelTx));

    /* Launch transmission */
    if (DATA_CACHE_IS_ENABLED())
    {
        /* Invalidate cache lines having received buffer before using it
         * to load the latest data in the actual memory to the cache */
        DCACHE_CLEAN_BY_ADDR((uint32_t *)dObj->pWrBuffer, dObj->wrBufferSize);
    }

    SYS_DMA_ChannelTransfer (dObj->plib->dmaChannelTx, (const void *)dObj->pWrBuffer, 
            (const void *)dObj->plib->usartAddressTx, length);
    
    return length;
}

void USI_USART_RegisterCallback( DRV_HANDLE handle, USI_USART_CALLBACK cbFunc,
        uintptr_t context)
{
    USI_USART_OBJ* dObj;
    
    /* Check handler */
    dObj = _USI_USART_CheckHandler(handle);
    if (dObj == NULL)
    {
        return;
    }
    
    /* Set callback function */
    dObj->cbFunc = cbFunc;
    
    /* Set context related to cbFunc */
    dObj->context = context;
    
    /* Launch reception */
    dObj->plib->read(&dObj->rcvChar, 1);
}

void USI_USART_Flush(DRV_HANDLE handle)
{
    (void)handle;
}

void USI_USART_Close(DRV_HANDLE handle)
{
    (void)handle;
}

void USI_USART_Tasks ( DRV_HANDLE handle )
{
    USI_USART_OBJ* dObj;
    USI_USART_MSG* pMsg;
    
    /* Check handler */
    dObj = _USI_USART_CheckHandler(handle);
    if (dObj == NULL)
    {
        return;
    }
    
    /* Handle callback functions */
    pMsg = dObj->pMsgQueue->front;
    if ((pMsg != NULL) && (pMsg->length > 0))
    {        
        if (dObj->cbFunc)
        {
            dObj->cbFunc(pMsg->pMessage, pMsg->length, dObj->context);
        }
        
        /* Remove Message from Queue */
        _USI_USART_GET_MSG_FROM_QUEUE(dObj);
    }
}
