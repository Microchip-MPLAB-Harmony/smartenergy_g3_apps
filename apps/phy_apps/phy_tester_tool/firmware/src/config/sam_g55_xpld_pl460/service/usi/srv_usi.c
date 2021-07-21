/*******************************************************************************
  USI Service Implementation.

  Company:
    Microchip Technology Inc.

  File Name:
    srv_usi.c

  Summary:
    Source code for the USI Service implementation.

  Description:
    This file contains the source code for the implementation of the
    USI Service.
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
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "configuration.h"
#include "driver/driver_common.h"
#include "service/usi/srv_usi.h"
#include "service/pcrc/srv_pcrc.h"
#include "srv_usi_local.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data
// *****************************************************************************
// *****************************************************************************
/* This is the service instance object array. */
static SRV_USI_OBJ gSrvUSIOBJ[SRV_USI_INSTANCES_NUMBER] = {NULL};

/* This is the USI callback object for each USI instance. */
static SRV_USI_CALLBACK gSrvUSICallbackOBJ[SRV_USI_INSTANCES_NUMBER][10] = {NULL};

// *****************************************************************************
// *****************************************************************************
// Section: File scope functions
// *****************************************************************************
// *****************************************************************************
static SRV_USI_CALLBACK_INDEX _SRV_USI_GetCallbackIndexFromProtocol(SRV_USI_PROTOCOL_ID protocol)
{
    SRV_USI_CALLBACK_INDEX callbackIndex;

    switch(protocol)
    {
        case SRV_USI_PROT_ID_MNGP_PRIME_GETQRY:
        case SRV_USI_PROT_ID_MNGP_PRIME_GETRSP:
        case SRV_USI_PROT_ID_MNGP_PRIME_SET:
        case SRV_USI_PROT_ID_MNGP_PRIME_RESET:
        case SRV_USI_PROT_ID_MNGP_PRIME_REBOOT:
        case SRV_USI_PROT_ID_MNGP_PRIME_FU:
        case SRV_USI_PROT_ID_MNGP_PRIME_GETQRY_EN:
        case SRV_USI_PROT_ID_MNGP_PRIME_GETRSP_EN:
            callbackIndex = 0;
            break;

        case SRV_USI_PROT_ID_SNIF_PRIME:
            callbackIndex = 1;
            break;
            
        case SRV_USI_PROT_ID_PHY_SERIAL_PRIME:
            callbackIndex = 2;
            break;
            
        case SRV_USI_PROT_ID_PHY:
            callbackIndex = 3;
            break;
            
        case SRV_USI_PROT_ID_SNIFF_G3:
            callbackIndex = 4;
            break;
            
        case SRV_USI_PROT_ID_MAC_G3:
            callbackIndex = 5;
            break;
            
        case SRV_USI_PROT_ID_ADP_G3:
            callbackIndex = 6;
            break;
            
        case SRV_USI_PROT_ID_COORD_G3:
            callbackIndex = 7;
            break;
            
        case SRV_USI_PROT_ID_PHY_MICROPLC:
            callbackIndex = 8;
            break;
            
        case SRV_USI_PROT_ID_PRIME_API:
            callbackIndex = 9;
            break;

        case SRV_USI_PROT_ID_INVALID:
        default:
            callbackIndex = SRV_USI_CALLBACK_INDEX_INVALID;
    }

    return callbackIndex;    
}

static PCRC_CRC_TYPE _SRV_USI_GetCRCTypeFromProtocol(SRV_USI_PROTOCOL_ID protocol)
{
    PCRC_CRC_TYPE crcType;

    switch(protocol)
    {
        case SRV_USI_PROT_ID_MNGP_PRIME_GETQRY:
        case SRV_USI_PROT_ID_MNGP_PRIME_GETRSP:
        case SRV_USI_PROT_ID_MNGP_PRIME_SET:
        case SRV_USI_PROT_ID_MNGP_PRIME_RESET:
        case SRV_USI_PROT_ID_MNGP_PRIME_REBOOT:
        case SRV_USI_PROT_ID_MNGP_PRIME_FU:
        case SRV_USI_PROT_ID_MNGP_PRIME_GETQRY_EN:
        case SRV_USI_PROT_ID_MNGP_PRIME_GETRSP_EN:
            crcType = PCRC_CRC32;
            break;

        case SRV_USI_PROT_ID_SNIF_PRIME:
        case SRV_USI_PROT_ID_PHY_SERIAL_PRIME:
        case SRV_USI_PROT_ID_PHY:
        case SRV_USI_PROT_ID_SNIFF_G3:
        case SRV_USI_PROT_ID_MAC_G3:
        case SRV_USI_PROT_ID_ADP_G3:
        case SRV_USI_PROT_ID_COORD_G3:
            crcType = PCRC_CRC16;
            break;
            
        case SRV_USI_PROT_ID_PRIME_API:
        case SRV_USI_PROT_ID_PHY_MICROPLC:
        case SRV_USI_PROT_ID_INVALID:
        default:
            crcType = PCRC_CRC8;
            break;
    }

    return crcType;    
}

static SRV_USI_HANDLE _SRV_USI_HandleValidate(SRV_USI_HANDLE handle)
{
    /* This function returns the same handle if the handle is valid. Returns 
       SRV_USI_HANDLE_INVALID otherwise. */

    uint8_t srvIndex;

    if((handle != SRV_USI_HANDLE_INVALID) && (handle != 0))
    {
        /* Look for the handle */
        for (srvIndex = 0; srvIndex < SRV_USI_INSTANCES_NUMBER; srvIndex++)
        {
            if (handle == (SRV_USI_HANDLE)(&gSrvUSIOBJ[srvIndex])) {
                return handle;
            }
        }
    }

    return(SRV_USI_HANDLE_INVALID);
}

static void _SRV_USI_Callback_Handle ( uint8_t *pData, uint16_t length, uintptr_t context ) 
{    
    SRV_USI_OBJ* dObj;
    uint32_t crcGetValue;
    uint32_t crcRcvValue;
    SRV_USI_PROTOCOL_ID protocol;
    PCRC_CRC_TYPE crcType;
    size_t dataLength;
    SRV_USI_CALLBACK_INDEX cbIndex;
    
    /* Check valid context : the driver handle */
    if (_SRV_USI_HandleValidate((SRV_USI_HANDLE)context) == SRV_USI_HANDLE_INVALID)
    {
        return;
    }

    dObj = (SRV_USI_OBJ*)context;
    
    if (length) 
    {      
        /* New received message */
        /* Extract Protocol */
        protocol = (SRV_USI_PROTOCOL_ID)USI_TYPE_PROTOCOL(*(pData + 1));
        
        /* Get CRC type from Protocol */
        crcType = _SRV_USI_GetCRCTypeFromProtocol(protocol);
        
        /* Extract data length */
        dataLength = USI_LEN_PROTOCOL(*pData, *(pData + 1));
                
        /* Add extended length */
        if ((protocol == SRV_USI_PROT_ID_ADP_G3) ||
            (protocol == SRV_USI_PROT_ID_COORD_G3) ||
            (protocol == SRV_USI_PROT_ID_PRIME_API))
        {
            dataLength += *(pData + 2) & USI_XLEN_MSK;
        }

        /* Check invalid length : remove Header and CRC bytes */
        if (dataLength != (length - (2 + (1 << crcType))))
        {
            /* Discard message */
            return;
        }
        
        /* Check CRC */
        crcGetValue = SRV_PCRC_GetValue(pData, length - (1<<crcType), 
                PCRC_HT_USI, crcType, 0);
 
        if (crcType == PCRC_CRC8)
        {
            crcRcvValue = (uint32_t)(*(pData + length - 1));
        } 
        else if (crcType == PCRC_CRC16)
        {
            crcRcvValue = (((uint32_t)(*(pData + length - 2))) << 8) + 
                          (uint32_t)(*(pData + length - 1));
        }
        else
        {            
            crcRcvValue = (((uint32_t)(*(pData + length - 4))) << 24) + 
                          (((uint32_t)(*(pData + length - 3))) << 16) + 
                          (((uint32_t)(*(pData + length - 2))) << 8) + 
                          (uint32_t)(*(pData + length - 1));
        }
        
        if (crcGetValue != crcRcvValue) 
        {
            /* Discard message */
            return;            
        }
    
        /* Launch USI callback */
        cbIndex = _SRV_USI_GetCallbackIndexFromProtocol(protocol);
        if (dObj->callback[cbIndex])
        {
            dObj->callback[cbIndex](pData + 2, dataLength);
        }        
    }
}

static uint8_t* _SRV_USI_EscapeData( uint8_t *pDstData, uint8_t *pSrcData,
                                      uint16_t length, uint8_t *pEndData )
{
    while (length)
    {
        if (*pSrcData == USI_ESC_KEY_7D)
        {
            *pDstData++ = USI_ESC_KEY_7D;
            *pDstData++ = USI_ESC_KEY_5D;
        } 
        else if (*pSrcData == USI_ESC_KEY_7E)
        {
            *pDstData++ = USI_ESC_KEY_7D;
            *pDstData++ = USI_ESC_KEY_5E;
        } 
        else
        {
            *pDstData++ = *pSrcData;
        }
                  
        if (pDstData == pEndData)
        {
            /* Escaped Message can't fit in Write buffer */
            return 0;
        }
        
        pSrcData++;
        length--;
    }
    
    return pDstData;
}

static size_t _SRV_USI_BuildMessage( uint8_t *pDstData, size_t maxDstLength, 
                                     SRV_USI_PROTOCOL_ID protocol, 
                                     uint8_t *pData, size_t length )
{
    uint8_t* pNewData;
    uint8_t* pEndData;
    uint8_t valueTmp[4];
    uint32_t valueTmp32;
    PCRC_CRC_TYPE crcType;
    
    /* Get CRC type from Protocol */
    crcType = _SRV_USI_GetCRCTypeFromProtocol(protocol);
    
    /* Build new message */
    pNewData = pDstData;
    pEndData = pNewData + (maxDstLength - 3);
    
    /* Build header message */
    *pNewData++ = USI_ESC_KEY_7E;
    valueTmp[0] = USI_LEN_HI_PROTOCOL(length);
    valueTmp[1] = USI_LEN_LO_PROTOCOL(length) + USI_TYPE_PROTOCOL(protocol);
    
    /* Get CRC from USI header: 2 bytes */
    valueTmp32 = SRV_PCRC_GetValue(&valueTmp[0], 2, PCRC_HT_USI, PCRC_CRC16, 0);
    /* Escape USI header */
    pNewData = _SRV_USI_EscapeData(pNewData, valueTmp, 2, pEndData);
    if (pNewData == 0)
    {
        /* Error in Escape Data: can't fit in Destiny buffer */
        return 0;
    }
    
    /* Get CRC from USI data. */
    valueTmp32 = SRV_PCRC_GetValue(pData, length, PCRC_HT_USI, crcType, valueTmp32);
    /* Escape USI data */
    pNewData = _SRV_USI_EscapeData(pNewData, pData, length, pEndData);
    if (pNewData == 0)
    {
        /* Error in Escape Data: can't fit in Destiny buffer */
        return 0;
    }
    
    /* Escape CRC value */
    valueTmp[0] = (valueTmp32 >> 24) & 0xFF;
    valueTmp[1] = (valueTmp32 >> 16) & 0xFF;
    valueTmp[2] = (valueTmp32 >> 8) & 0xFF;
    valueTmp[3] = valueTmp32 & 0xFF;
    if (crcType == PCRC_CRC8)
    {
        pNewData = _SRV_USI_EscapeData(pNewData, &valueTmp[3], 1, pEndData);
    } 
    else if (crcType == PCRC_CRC16)
    {
        pNewData = _SRV_USI_EscapeData(pNewData, &valueTmp[2], 2, pEndData);
    }
    else
    {
        pNewData = _SRV_USI_EscapeData(pNewData, valueTmp, 4, pEndData);
    }
    
    if (pNewData == 0)
    {
        /* Error in Escape Data: can't fit in Destiny buffer */
        return 0;
    }
    
    *pNewData++ = USI_ESC_KEY_7E;
    
    return (pNewData - pDstData);
}

// *****************************************************************************
// *****************************************************************************
// Section: USI Service Common Interface Implementation
// *****************************************************************************
// *****************************************************************************

SYS_MODULE_OBJ SRV_USI_Initialize(
    const SYS_MODULE_INDEX index, 
    const SYS_MODULE_INIT * const init 
)
{
    SRV_USI_OBJ* dObj;
    SRV_USI_INIT* usiInit = (SRV_USI_INIT *)init ;

    /* Confirm valid arguments */
    if(index >= SRV_USI_INSTANCES_NUMBER)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    /* Allocate the service object */
    dObj = &gSrvUSIOBJ[index];
    
   if ((dObj->status == SRV_USI_STATUS_UNINITIALIZED) && usiInit)
    {
        dObj->inUse                 = false;
        dObj->status                = SRV_USI_STATUS_NOT_CONFIGURED;
        dObj->devIndex              = usiInit->deviceIndex;
        dObj->devDesc               = usiInit->consDevDesc;
	    dObj->pWrBuffer             = usiInit->pWrBuffer;
	    dObj->wrBufferSize          = usiInit->wrBufferSize;
        dObj->callback              = &gSrvUSICallbackOBJ[index][0];
    
        dObj->devDesc->init(dObj->devIndex, usiInit->deviceInitData);
        
        /* Return the object structure */
        return ( (SYS_MODULE_OBJ)index );
    }
    
    /* Return the object structure */
    return SYS_MODULE_OBJ_INVALID;
}

SRV_USI_HANDLE SRV_USI_Open(
    const SYS_MODULE_INDEX index
)
{
    SRV_USI_OBJ* dObj = &gSrvUSIOBJ[index];

    /* Validate the request */
    if (index >= SRV_USI_INSTANCES_NUMBER)
    {
        return SRV_USI_HANDLE_INVALID;
    }

    if(dObj->status != SRV_USI_STATUS_NOT_CONFIGURED)
    {
        return SRV_USI_HANDLE_INVALID;
    }
    
    /* Open USI device */
    dObj->devDesc->open(dObj->devIndex);

    return ((SRV_USI_HANDLE)dObj);
}

void SRV_USI_Close( SRV_USI_HANDLE handle )
{
    SRV_USI_OBJ* dObj;

    /* Validate the driver handle */
    if (_SRV_USI_HandleValidate(handle) == SRV_USI_HANDLE_INVALID)
    {
        return;
    }

    dObj = (SRV_USI_OBJ*)handle;

    if((dObj->status != SRV_USI_STATUS_CONFIGURED) || (dObj->inUse == false))
    {
        return;
    }
    
    dObj->devDesc->close(dObj->devIndex);
    
    /* De-Allocate the service object */
    dObj->inUse = false;

}

SRV_USI_STATUS SRV_USI_Status( SRV_USI_HANDLE handle )
{
    SRV_USI_OBJ* dObj;

    /* Validate the driver handle */
    if (_SRV_USI_HandleValidate(handle) == SRV_USI_HANDLE_INVALID)
    {
        return SRV_USI_STATUS_ERROR;
    }

    dObj = (SRV_USI_OBJ*)handle;
    
    /* Check USI device status */
    dObj->status = dObj->devDesc->status(dObj->devIndex);

    return dObj->status;
}

void SRV_USI_CallbackRegister ( const SRV_USI_HANDLE handle, 
        SRV_USI_PROTOCOL_ID protocol, SRV_USI_CALLBACK callback )
{
    SRV_USI_CALLBACK_INDEX callbackIndex;
    SRV_USI_OBJ* dObj = (SRV_USI_OBJ*)handle;
    SRV_USI_CALLBACK *cb;

    /* Validate the driver handle */
    if (_SRV_USI_HandleValidate(handle) == SRV_USI_HANDLE_INVALID)
    {
        return;
    }

    /* Get callback index from USI protocol */
    callbackIndex = _SRV_USI_GetCallbackIndexFromProtocol(protocol);

    if (callbackIndex == SRV_USI_CALLBACK_INDEX_INVALID)
    {
        return;
    }

    /* Validate callback function */
    if (callback == NULL)
    {
        return;
    }
    
    /* Register callback to the USI protocol */ 
    cb = &(dObj->callback[callbackIndex]);
    *cb = callback;
    
    /* Register reception callback */
    dObj->devDesc->setReadCallback(dObj->devIndex, _SRV_USI_Callback_Handle, (uintptr_t)dObj);
    
    /* Set USI as used */ 
    dObj->inUse = true;

}

void SRV_USI_Tasks( const SYS_MODULE_INDEX index )
{
    SRV_USI_OBJ* dObj = &gSrvUSIOBJ[index];
    
    /* Validate the request */
    if(index >= SRV_USI_INSTANCES_NUMBER)
    {
        return;
    }

    /* Is the service instance already initialized? */
    if(dObj->inUse == false)
    {
        return;
    }
    
    dObj->devDesc->task(dObj->devIndex);
    
}

void SRV_USI_Send_Message( const SRV_USI_HANDLE handle, 
        SRV_USI_PROTOCOL_ID protocol, uint8_t *data, size_t length )
{
    SRV_USI_OBJ* dObj = (SRV_USI_OBJ*)handle;
    size_t writeLength;

    /* Validate the driver handle */
    if (_SRV_USI_HandleValidate(handle) == SRV_USI_HANDLE_INVALID)
    {
        return;
    }
    
    /* Check length */
    if ((length == 0) || (length > dObj->wrBufferSize))
    {
        return;
    }
    
    /* Build USI message */
    writeLength = _SRV_USI_BuildMessage(dObj->pWrBuffer, dObj->wrBufferSize, protocol, data, length);
    
    /* Send message */
    dObj->devDesc->write(dObj->devIndex, dObj->pWrBuffer, writeLength);
}
