/*******************************************************************************
  USB CDC wrapper used from USI service Implementation

  Company:
    Microchip Technology Inc.

  File Name:
    srv_usi_cdc.c

  Summary:
    USB CDC wrapper used from USI service implementation.

  Description:
    The USB CDC wrapper provides a simple interface to manage the USB
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
#include "usb/usb_device.h"
#include "usb/usb_device_cdc.h"
#include "srv_usi_local.h"
#include "srv_usi_definitions.h"
#include "srv_usi_cdc.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data
// *****************************************************************************
// *****************************************************************************
/* This is the service instance object array. */
const SRV_USI_DEV_DESC srvUSICDCDevDesc =
{
    .usiDevice                  = SRV_USI_DEV_USB_CDC,
    .intent                     = DRV_IO_INTENT_READWRITE,
    .init                       = USI_CDC_Initialize,
    .open                       = USI_CDC_Open,
    .setReadCallback            = USI_CDC_RegisterCallback,
    .write                      = USI_CDC_Write,
    .task                       = USI_CDC_Tasks,
    .flush                      = USI_CDC_Flush,
    .close                      = USI_CDC_Close,
    .status                     = USI_CDC_Status,
};

static USI_CDC_OBJ gUsiCdcOBJ[SRV_USI_CDC_CONNECTIONS] = {0};

#define USI_CDC_GET_INSTANCE(index)    (index >= SRV_USI_CDC_CONNECTIONS)? NULL : &gUsiCdcOBJ[index]

// *****************************************************************************
// *****************************************************************************
// Section: File scope functions
// *****************************************************************************
// *****************************************************************************
static uint32_t _USI_CDC_Transfer_Received_Data(USI_CDC_OBJ* dObj)
{
    uint8_t *pData = dObj->cdcReadBuffer;
    uint8_t *bookmark;
    
    while(dObj->cdcNumBytesRead)
    {
        switch(dObj->devStatus)
        {
            case USI_CDC_IDLE:
                /* Waiting to MSG KEY */
                if (*pData == USI_ESC_KEY_7E)
                {
                    dObj->usiIsReadComplete = false;
                    /* Start of USI Message */
                    dObj->usiNumBytesRead = 0;
                    /* New Message, start reception */
                    dObj->devStatus = USI_CDC_RCV;
                    /* Store the initial position of USI buffer input */
                    bookmark = dObj->usiRdInIndex;
                }            
                break;

            case USI_CDC_RCV:
                if (*pData == USI_ESC_KEY_7E)
                {
                    /* End of USI Message */    /////////// Como sacar varios mensajes en una misma recepcion??????????????
                    dObj->usiIsReadComplete = true;
                    dObj->devStatus = USI_CDC_IDLE;
                }              
                else if (*pData == USI_ESC_KEY_7D)
                {
                    /* Escape character */
                    dObj->devStatus = USI_CDC_ESC;
                } 
                else
                {
                    /* Store character */
                    *dObj->usiRdInIndex++ = *pData;
                    dObj->usiNumBytesRead++;
                }

                break;

            case USI_CDC_ESC:
                if (*pData == USI_ESC_KEY_5E)
                {
                    /* Store character after escape it */
                    *dObj->usiRdInIndex++ = USI_ESC_KEY_7E;
                    dObj->devStatus = USI_CDC_RCV;
                    dObj->usiNumBytesRead++;
                }  
                else if (*pData == USI_ESC_KEY_5D)
                {
                    /* Store character after escape it */
                    *dObj->usiRdInIndex++ = USI_ESC_KEY_7D;
                    dObj->devStatus = USI_CDC_RCV;
                    dObj->usiNumBytesRead++;
                }
                else
                {
                    /* ERROR: Escape format - restore USI buffer */
                    dObj->usiRdInIndex = bookmark;
                    dObj->usiNumBytesRead = 0;
                    dObj->devStatus = USI_CDC_IDLE;
                }

                break;
        }    
        
        pData++;
        dObj->cdcNumBytesRead--;
    }
    
    return dObj->usiNumBytesRead;    
}

/*******************************************************
 * USB CDC Device Events - Event Handler
 *******************************************************/
static USB_DEVICE_CDC_EVENT_RESPONSE _USB_CDC_DeviceCDCEventHandler(USB_DEVICE_CDC_INDEX index,
    USB_DEVICE_CDC_EVENT event, void * pData, uintptr_t userData)
{
    USI_CDC_OBJ* dObj;
    USB_CDC_CONTROL_LINE_STATE * controlLineStateData;
    USB_DEVICE_CDC_EVENT_DATA_READ_COMPLETE * eventDataRead;
    
    dObj = (USI_CDC_OBJ *)userData;

    switch(event)
    {
        case USB_DEVICE_CDC_EVENT_GET_LINE_CODING:

            /* This means the host wants to know the current line
             * coding. This is a control transfer request. Use the
             * USB_DEVICE_ControlSend() function to send the data to
             * host.  */

            USB_DEVICE_ControlSend(dObj->devHandle, &dObj->getLineCodingData, sizeof(USB_CDC_LINE_CODING));

            break;

        case USB_DEVICE_CDC_EVENT_SET_LINE_CODING:

            /* This means the host wants to set the line coding.
             * This is a control transfer request. Use the
             * USB_DEVICE_ControlReceive() function to receive the
             * data from the host */

            USB_DEVICE_ControlReceive(dObj->devHandle, &dObj->setLineCodingData, sizeof(USB_CDC_LINE_CODING));

            break;

        case USB_DEVICE_CDC_EVENT_SET_CONTROL_LINE_STATE:

            /* This means the host is setting the control line state.
             * Read the control line state. We will accept this request
             * for now. */

            controlLineStateData = (USB_CDC_CONTROL_LINE_STATE *)pData;
            dObj->controlLineStateData.dtr = controlLineStateData->dtr;
            dObj->controlLineStateData.carrier = controlLineStateData->carrier;

            USB_DEVICE_ControlStatus(dObj->devHandle, USB_DEVICE_CONTROL_STATUS_OK);

            break;

        case USB_DEVICE_CDC_EVENT_SEND_BREAK:

            /* This means that the host is requesting that a break of the
             * specified duration be sent. Read the break duration */

            dObj->breakData = ((USB_DEVICE_CDC_EVENT_DATA_SEND_BREAK *)pData)->breakDuration;
            
            /* Complete the control transfer by sending a ZLP  */
            USB_DEVICE_ControlStatus(dObj->devHandle, USB_DEVICE_CONTROL_STATUS_OK);
            
            break;

        case USB_DEVICE_CDC_EVENT_READ_COMPLETE:

            /* This means that the host has sent some data*/
            eventDataRead = (USB_DEVICE_CDC_EVENT_DATA_READ_COMPLETE *)pData;
            
            if(eventDataRead->status != USB_DEVICE_CDC_RESULT_ERROR)
            {
                dObj->cdcIsReadComplete = true;
                
                dObj->cdcNumBytesRead = eventDataRead->length; 
            }
            break;

        case USB_DEVICE_CDC_EVENT_CONTROL_TRANSFER_DATA_RECEIVED:

            /* The data stage of the last control transfer is complete. */
            USB_DEVICE_ControlStatus(dObj->devHandle, USB_DEVICE_CONTROL_STATUS_OK);
            break;

        case USB_DEVICE_CDC_EVENT_CONTROL_TRANSFER_DATA_SENT:

            /* This means the GET LINE CODING function data is valid. */
            break;

        case USB_DEVICE_CDC_EVENT_WRITE_COMPLETE:

            /* This means that the data write got completed. */

            dObj->cdcIsWriteComplete = true;
            break;

        default:
            break;
    }

    return USB_DEVICE_CDC_EVENT_RESPONSE_NONE;
}

/***********************************************
 * USB Device Layer Event Handler.
 ***********************************************/
static void _USI_CDC_DeviceEventHandler(USB_DEVICE_EVENT event, void * eventData, 
    uintptr_t context)
{
    USB_DEVICE_EVENT_DATA_CONFIGURED *configuredEventData;
    USI_CDC_OBJ* dObj;
    
    dObj = (USI_CDC_OBJ*)context;

    switch(event)
    {
        case USB_DEVICE_EVENT_SOF:
            /* Flag determines SOF event occurrence */
            dObj->sofEventHasOccurred = true;
            break;

        case USB_DEVICE_EVENT_RESET:
            dObj->devStatus = USI_CDC_IDLE;
            dObj->usiStatus = SRV_USI_STATUS_UNINITIALIZED;
            dObj->cdcIsReadComplete = true;
            dObj->cdcIsWriteComplete = true;
            dObj->usiIsReadComplete = false;
            dObj->readTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
            dObj->writeTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
            break;

        case USB_DEVICE_EVENT_CONFIGURED:
            /* Check the configuration. We only support configuration 1 */
            configuredEventData = (USB_DEVICE_EVENT_DATA_CONFIGURED*)eventData;
            if (configuredEventData->configurationValue == 1)
            {
                /* Register the CDC Device event handler */
                USB_DEVICE_CDC_EventHandlerSet(dObj->cdcInstanceIndex, _USB_CDC_DeviceCDCEventHandler, (uintptr_t)dObj);
                /* Mark that the device is now configured */
                dObj->usiStatus = SRV_USI_STATUS_CONFIGURED;
            }
            break;

        case USB_DEVICE_EVENT_POWER_DETECTED:
            /* VBUS was detected. We can attach the device */
            USB_DEVICE_Attach(dObj->devHandle);
            break;

        case USB_DEVICE_EVENT_POWER_REMOVED:
            /* VBUS is not available. We can detach the device */
            USB_DEVICE_Detach(dObj->devHandle);
            dObj->usiStatus = SRV_USI_STATUS_NOT_CONFIGURED;
            break;

        case USB_DEVICE_EVENT_SUSPENDED:
        case USB_DEVICE_EVENT_RESUMED:
        case USB_DEVICE_EVENT_ERROR:
        default:
            
            break;
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: USI CDC Service Common Interface Implementation
// *****************************************************************************
// *****************************************************************************

DRV_HANDLE USI_CDC_Initialize(uint32_t index, const void* initData)
{
    USI_CDC_OBJ* dObj = USI_CDC_GET_INSTANCE(index);
    USI_CDC_INIT_DATA* dObjInit = (USI_CDC_INIT_DATA*)initData;
    
    if (dObj == NULL)
    {
        return DRV_HANDLE_INVALID;
    }
    
    if (index >= SRV_USI_CDC_CONNECTIONS)
    {
        return DRV_HANDLE_INVALID;
    }
    
    dObj->cdcInstanceIndex = dObjInit->cdcInstanceIndex;
    dObj->cdcReadBuffer = dObjInit->cdcReadBuffer;
    dObj->usiReadBuffer = dObjInit->usiReadBuffer;
    dObj->cdcBufferSize = dObjInit->cdcBufferSize;
    dObj->usiBufferSize = dObjInit->usiBufferSize;
    
    dObj->cdcNumBytesRead = 0;
    dObj->usiNumBytesRead = 0;
    dObj->usiRdInIndex = dObj->usiReadBuffer;
    dObj->usiRdOutIndex = dObj->usiReadBuffer;
    
    dObj->cbFunc = NULL;
    dObj->devStatus = USI_CDC_IDLE;
    dObj->usiStatus = SRV_USI_STATUS_NOT_CONFIGURED;

    dObj->cdcIsReadComplete = true;
    dObj->cdcIsWriteComplete = true;
    dObj->usiIsReadComplete = false;
    dObj->readTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
    dObj->writeTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;

    return (DRV_HANDLE)index;
}

DRV_HANDLE USI_CDC_Open(uint32_t index)
{
    USI_CDC_OBJ* dObj = USI_CDC_GET_INSTANCE(index);
    
    if (dObj == NULL)
    {
        return DRV_HANDLE_INVALID;
    }
    
    if (index >= SRV_USI_CDC_CONNECTIONS)
    {
        return DRV_HANDLE_INVALID;
    }

    /* Open the USB device layer */
    dObj->devHandle = USB_DEVICE_Open(dObj->cdcInstanceIndex, DRV_IO_INTENT_READWRITE);

    if(dObj->devHandle != USB_DEVICE_HANDLE_INVALID)
    {
        /* Register a callback with device layer to get event notification (for end point 0) */
        USB_DEVICE_EventHandlerSet(dObj->devHandle, _USI_CDC_DeviceEventHandler, (uintptr_t)dObj);
        return (DRV_HANDLE)index;
    }
    else
    {
        return DRV_HANDLE_INVALID;
    }
}

size_t USI_CDC_Write(uint32_t index, void* pData, size_t length)
{
    USI_CDC_OBJ* dObj = USI_CDC_GET_INSTANCE(index);
    USB_DEVICE_CDC_RESULT result;
    
    /* Check handler */
    if (dObj == NULL)
    {
        return 0;
    }
    
    if (index >= SRV_USI_CDC_CONNECTIONS)
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
    
    result = USB_DEVICE_CDC_Write(dObj->cdcInstanceIndex,
                &dObj->writeTransferHandle, pData, length,
                USB_DEVICE_CDC_TRANSFER_FLAGS_DATA_COMPLETE);
    
    if ((result != USB_DEVICE_CDC_RESULT_OK) || (dObj->writeTransferHandle == USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID))
    {
        return 0;
    }

    return length;
}

void USI_CDC_RegisterCallback(uint32_t index, USI_CDC_CALLBACK cbFunc,
        uintptr_t context)
{
    USI_CDC_OBJ* dObj = USI_CDC_GET_INSTANCE(index);
    
    /* Check handler */
    if (dObj == NULL)
    {
        return;
    }
    
    if (index >= SRV_USI_CDC_CONNECTIONS)
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

void USI_CDC_Flush(uint32_t index)
{
    (void)index;
}

void USI_CDC_Close(uint32_t index)
{
    USI_CDC_OBJ* dObj = USI_CDC_GET_INSTANCE(index);
    
    /* Check handler */    
    if (dObj == NULL)
    {
        return;
    }
    
    if (index >= SRV_USI_CDC_CONNECTIONS)
    {
        return;
    }
    
    dObj->usiStatus = SRV_USI_STATUS_NOT_CONFIGURED;
}

SRV_USI_STATUS USI_CDC_Status(uint32_t index)
{
    USI_CDC_OBJ* dObj = USI_CDC_GET_INSTANCE(index);
    
    /* Check handler */    
    if (dObj == NULL)
    {
        return SRV_USI_STATUS_ERROR;
    }
    
    if (index >= SRV_USI_CDC_CONNECTIONS)
    {
        return SRV_USI_STATUS_ERROR;
    }

    return dObj->usiStatus;
}

void USI_CDC_Tasks (uint32_t index)
{
    USI_CDC_OBJ* dObj = USI_CDC_GET_INSTANCE(index);
    
    /* Check handler */    
    if (dObj == NULL)
    {
        return;
    }
    
    if (index >= SRV_USI_CDC_CONNECTIONS)
    {
        return;
    }
    
    if (dObj->usiStatus != SRV_USI_STATUS_CONFIGURED)
    {
        return;
    }
    
    if(dObj->cdcIsReadComplete == true)
    {
        /* Extract CDC received data to USI buffer */
        if (dObj->cdcNumBytesRead)
        {
            _USI_CDC_Transfer_Received_Data(dObj);
        }
        
        /* Launch next CDC reception process */
        dObj->cdcIsReadComplete = false;
        USB_DEVICE_CDC_Read (dObj->cdcInstanceIndex, &dObj->readTransferHandle, dObj->cdcReadBuffer,
                        dObj->cdcBufferSize);
        
        /* Report Reception callback */
        if (dObj->usiIsReadComplete)
        {
            if (dObj->cbFunc)
            {
                dObj->cbFunc(dObj->usiRdOutIndex, dObj->usiNumBytesRead, dObj->context);
            }
            
            /* Update Out Pointer */
            dObj->usiRdOutIndex += dObj->usiNumBytesRead;
            if (dObj->usiRdOutIndex == dObj->usiRdInIndex) {
                /* Restart In/Out Pointers */
                dObj->usiRdOutIndex = dObj->usiReadBuffer;
                dObj->usiRdInIndex = dObj->usiReadBuffer;
            }
        }
    }            
}
