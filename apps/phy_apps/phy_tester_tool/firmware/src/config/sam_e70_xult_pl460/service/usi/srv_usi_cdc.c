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
#include "usb/usb_device.h"
#include "usb/usb_device_cdc.h"
#include "srv_usi_local.h"
#include "srv_usi_definitions.h"
#include "srv_usi_cdc.h"
#include "definitions.h"                // SYS function prototypes

// *****************************************************************************
// *****************************************************************************
// Section: Global Data
// *****************************************************************************
// *****************************************************************************
/* This is the service instance object array. */
const SRV_USI_DEV_DESC srvUSICDCDevDesc =
{
    .init                       = USI_CDC_Initialize,
    .open                       = USI_CDC_Open,
    .setReadCallback            = USI_CDC_RegisterCallback,
    .writeData                  = USI_CDC_Write,
    .task                       = USI_CDC_Tasks,
    .close                      = USI_CDC_Close,
    .status                     = USI_CDC_Status,
};

static USI_CDC_OBJ gUsiCdcOBJ[SRV_USI_CDC_CONNECTIONS] = {0};

#define USI_CDC_GET_INSTANCE(index)    (((index) >= SRV_USI_CDC_CONNECTIONS)? NULL : &gUsiCdcOBJ[index])

// *****************************************************************************
// *****************************************************************************
// Section: File scope functions
// *****************************************************************************
// *****************************************************************************

static void lUSI_CDC_TransferReceivedData(USI_CDC_OBJ* dObj)
{
    uint8_t *pData = dObj->cdcReadBuffer;

    while(dObj->cdcNumBytesRead > 0U)
    {
        bool store = false;
        uint8_t charStore = 0;

        switch(dObj->devStatus)
        {
            case USI_CDC_IDLE:
                /* Waiting to MSG KEY */
                if (*pData == USI_ESC_KEY_7E)
                {
                    /* Start of USI Message */
                    dObj->usiNumBytesRead = 0;
                    /* New Message, start reception */
                    dObj->devStatus = USI_CDC_RCV;
                }
                break;

            case USI_CDC_RCV:
                if (*pData == USI_ESC_KEY_7E)
                {
                    if ((dObj->usiNumBytesRead > 0U) && (dObj->cbFunc != NULL))
                    {
                        /* Report via USI callback */
                        dObj->cbFunc(dObj->usiReadBuffer, dObj->usiNumBytesRead, dObj->context);
                    }

                    /* End of USI Message */
                    dObj->devStatus = USI_CDC_IDLE;
                }
                else if (*pData == USI_ESC_KEY_7D)
                {
                    /* Escape character */
                    dObj->devStatus = USI_CDC_ESC;
                }
                else
                {
                    store = true;
                    charStore = *pData;
                }

                break;

            case USI_CDC_ESC:
            default:
                if (*pData == USI_ESC_KEY_5E)
                {
                    /* Store character after escape it */
                    store = true;
                    charStore = USI_ESC_KEY_7E;
                    dObj->devStatus = USI_CDC_RCV;
                }
                else if (*pData == USI_ESC_KEY_5D)
                {
                    /* Store character after escape it */
                    store = true;
                    charStore = USI_ESC_KEY_7D;
                    dObj->devStatus = USI_CDC_RCV;
                }
                else
                {
                    /* ERROR: Escape format - restore USI buffer */
                    dObj->devStatus = USI_CDC_IDLE;
                }

                break;
        }

        if (store)
        {
            if (dObj->usiNumBytesRead < dObj->usiBufferSize)
            {
                /* Store character */
                dObj->usiReadBuffer[dObj->usiNumBytesRead++] = charStore;
            }
            else
            {
                /* ERROR: Full buffer - restore USI buffer */
                dObj->devStatus = USI_CDC_IDLE;
            }
        }

        if (dObj->cdcNumBytesRead > 0U)
        {
            dObj->cdcNumBytesRead--;
            pData++;
        }
    }
}

/*******************************************************
 * USB CDC Device Events - Event Handler
 *******************************************************/
static USB_DEVICE_CDC_EVENT_RESPONSE lUSB_CDC_DeviceCDCEventHandler(USB_DEVICE_CDC_INDEX index,
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

            (void) USB_DEVICE_ControlSend(dObj->devHandle, &dObj->getLineCodingData, sizeof(USB_CDC_LINE_CODING));

            break;

        case USB_DEVICE_CDC_EVENT_SET_LINE_CODING:

            /* This means the host wants to set the line coding.
             * This is a control transfer request. Use the
             * USB_DEVICE_ControlReceive() function to receive the
             * data from the host */

            (void) USB_DEVICE_ControlReceive(dObj->devHandle, &dObj->setLineCodingData, sizeof(USB_CDC_LINE_CODING));

            break;

        case USB_DEVICE_CDC_EVENT_SET_CONTROL_LINE_STATE:

            /* This means the host is setting the control line state.
             * Read the control line state. We will accept this request
             * for now. */

            controlLineStateData = (USB_CDC_CONTROL_LINE_STATE *)pData;
            dObj->controlLineStateData.dtr = controlLineStateData->dtr;
            dObj->controlLineStateData.carrier = controlLineStateData->carrier;

            (void) USB_DEVICE_ControlStatus(dObj->devHandle, USB_DEVICE_CONTROL_STATUS_OK);

            break;

        case USB_DEVICE_CDC_EVENT_SEND_BREAK:

            /* This means that the host is requesting that a break of the
             * specified duration be sent. Read the break duration */

            dObj->breakData = ((USB_DEVICE_CDC_EVENT_DATA_SEND_BREAK *)pData)->breakDuration;

            /* Complete the control transfer by sending a ZLP  */
            (void) USB_DEVICE_ControlStatus(dObj->devHandle, USB_DEVICE_CONTROL_STATUS_OK);

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

            (void) USB_DEVICE_ControlStatus(dObj->devHandle, USB_DEVICE_CONTROL_STATUS_OK);
            break;

        case USB_DEVICE_CDC_EVENT_CONTROL_TRANSFER_DATA_SENT:

            /* This means the GET LINE CODING function data is valid. */
            break;

        case USB_DEVICE_CDC_EVENT_WRITE_COMPLETE:
            /* This means that the data write got completed. */
            break;

        /* MISRA C-2012 deviation block start */
        /* MISRA C-2012 Rule 16.4 deviated once. Deviation record ID - H3_MISRAC_2012_R_16_4_DR_1 */
        default:
            break;

        /* MISRA C-2012 deviation block end */
    }

    return USB_DEVICE_CDC_EVENT_RESPONSE_NONE;
}

/***********************************************
 * USB Device Layer Event Handler.
 ***********************************************/
static void lUSI_CDC_DeviceEventHandler(USB_DEVICE_EVENT event, void * eventData,
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
            break;

        case USB_DEVICE_EVENT_CONFIGURED:
            /* Check the configuration. We only support configuration 1 */
            configuredEventData = (USB_DEVICE_EVENT_DATA_CONFIGURED*)eventData;
            if (configuredEventData->configurationValue == 1U)
            {
                /* Register the CDC Device event handler */
                (void) USB_DEVICE_CDC_EventHandlerSet(dObj->cdcInstanceIndex, lUSB_CDC_DeviceCDCEventHandler, (uintptr_t)dObj);
                /* Mark that the device is now configured */
                dObj->usiStatus = SRV_USI_STATUS_CONFIGURED;
                /* Request first read */
                (void) USB_DEVICE_CDC_Read(dObj->cdcInstanceIndex, &dObj->readTransferHandle,
                        dObj->cdcReadBuffer, dObj->cdcBufferSize);
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
            break;

        /* MISRA C-2012 deviation block start */
        /* MISRA C-2012 Rule 16.4 deviated once. Deviation record ID - H3_MISRAC_2012_R_16_4_DR_1 */
        default:
            break;

        /* MISRA C-2012 deviation block end */
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: USI CDC Service Common Interface Implementation
// *****************************************************************************
// *****************************************************************************

void USI_CDC_Initialize(uint32_t index, const void * const initData)
{
    USI_CDC_OBJ* dObj = USI_CDC_GET_INSTANCE(index);
    const USI_CDC_INIT_DATA * const dObjInit = (const USI_CDC_INIT_DATA * const)initData;

    if (dObj == NULL)
    {
        return;
    }

    dObj->cdcInstanceIndex = dObjInit->cdcInstanceIndex;
    dObj->cdcReadBuffer = dObjInit->cdcReadBuffer;
    dObj->usiReadBuffer = dObjInit->usiReadBuffer;
    dObj->cdcBufferSize = dObjInit->cdcBufferSize;
    dObj->usiBufferSize = dObjInit->usiBufferSize;

    dObj->cbFunc = NULL;
    dObj->devStatus = USI_CDC_IDLE;
    dObj->usiStatus = SRV_USI_STATUS_NOT_CONFIGURED;

    dObj->cdcIsReadComplete = false;
    dObj->readTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
    dObj->writeTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
}

DRV_HANDLE USI_CDC_Open(uint32_t index)
{
    USI_CDC_OBJ* dObj = USI_CDC_GET_INSTANCE(index);

    if (dObj == NULL)
    {
        return DRV_HANDLE_INVALID;
    }

    /* Open the USB device layer */
    dObj->devHandle = USB_DEVICE_Open(dObj->cdcInstanceIndex, DRV_IO_INTENT_READWRITE);

    if(dObj->devHandle != USB_DEVICE_HANDLE_INVALID)
    {
        /* Register a callback with device layer to get event notification (for end point 0) */
        USB_DEVICE_EventHandlerSet(dObj->devHandle, lUSI_CDC_DeviceEventHandler, (uintptr_t)dObj);
        return (DRV_HANDLE)index;
    }
    else
    {
        return DRV_HANDLE_INVALID;
    }
}

void USI_CDC_Write(uint32_t index, void* pData, size_t length)
{
    USI_CDC_OBJ* dObj = USI_CDC_GET_INSTANCE(index);

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

    (void) USB_DEVICE_CDC_Write(dObj->cdcInstanceIndex,
            &dObj->writeTransferHandle, pData, length,
            USB_DEVICE_CDC_TRANSFER_FLAGS_DATA_COMPLETE);
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

    /* Set callback function */
    dObj->cbFunc = cbFunc;

    /* Set context related to cbFunc */
    dObj->context = context;
}

void USI_CDC_Close(uint32_t index)
{
    USI_CDC_OBJ* dObj = USI_CDC_GET_INSTANCE(index);

    /* Check handler */
    if (dObj == NULL)
    {
        return;
    }

    /* Close the USB device layer */
    USB_DEVICE_Close(dObj->devHandle);

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

    if (dObj->usiStatus != SRV_USI_STATUS_CONFIGURED)
    {
        return;
    }

    /* CDC reception process */
    if (dObj->cdcIsReadComplete == true)
    {
        dObj->cdcIsReadComplete = false;

        /* Extract CDC received data to USI buffer */
        lUSI_CDC_TransferReceivedData(dObj);

        /* Request next read */
        (void) USB_DEVICE_CDC_Read(dObj->cdcInstanceIndex, &dObj->readTransferHandle, dObj->cdcReadBuffer, dObj->cdcBufferSize);
    }
}
