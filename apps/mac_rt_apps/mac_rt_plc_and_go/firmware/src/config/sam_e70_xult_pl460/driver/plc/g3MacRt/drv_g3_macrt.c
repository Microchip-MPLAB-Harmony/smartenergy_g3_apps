/******************************************************************************
  DRV_G3_MACRT Library Interface Implementation

  Company:
    Microchip Technology Inc.

  File Name:
    drv_g3_macrt.c

  Summary:
    G3 MAC RT Driver Library Interface implementation

  Description:
    The G3 MAC RT Library provides a interface to access the PLC external device.
    This file implements the G3 Mac Real Time Library interface.
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
// Section: Include Files
// *****************************************************************************
// *****************************************************************************
#include "configuration.h"
#include "driver/plc/common/drv_plc_boot.h"
#include "driver/plc/g3MacRt/drv_g3_macrt.h"
#include "driver/plc/g3MacRt/drv_g3_macrt_local_comm.h"
#include "drv_g3_macrt_local.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data
// *****************************************************************************
// *****************************************************************************

/* This is the driver instance object array. */
DRV_G3_MACRT_OBJ gDrvG3MacRtObj;

// *****************************************************************************
// *****************************************************************************
// Section: PLC Driver Common Interface Implementation
// *****************************************************************************
// *****************************************************************************

SYS_MODULE_OBJ DRV_G3_MACRT_Initialize(
    const SYS_MODULE_INDEX index,
    const SYS_MODULE_INIT * const init
)
{
    DRV_G3_MACRT_INIT* g3MacRtInit = (DRV_G3_MACRT_INIT *)init;

    /* Validate the request */
    if (index >= DRV_G3_MACRT_INSTANCES_NUMBER)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    if (gDrvG3MacRtObj.state != DRV_G3_MACRT_STATE_UNINITIALIZED)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    gDrvG3MacRtObj.plcHal                = g3MacRtInit->plcHal;
    gDrvG3MacRtObj.plcProfile            = g3MacRtInit->plcProfile;
    gDrvG3MacRtObj.binSize               = g3MacRtInit->binEndAddress - g3MacRtInit->binStartAddress;
    gDrvG3MacRtObj.binStartAddress       = g3MacRtInit->binStartAddress;
    gDrvG3MacRtObj.secure                = g3MacRtInit->secure;
    gDrvG3MacRtObj.sleep                 = false;
    
    /* Callbacks initialization */
    gDrvG3MacRtObj.initCallback          = NULL;
    gDrvG3MacRtObj.bootDataCallback      = NULL;
    gDrvG3MacRtObj.txCfmCallback         = NULL;
    gDrvG3MacRtObj.dataIndCallback       = NULL;
    gDrvG3MacRtObj.rxParamsIndCallback   = NULL;
    gDrvG3MacRtObj.macSnifferIndCallback = NULL;
    gDrvG3MacRtObj.commStatusIndCallback = NULL;
    gDrvG3MacRtObj.phySnifferIndCallback = NULL;
    gDrvG3MacRtObj.exceptionCallback     = NULL;
    
    /* Clear PHY Sniffer Data Buffer */
    gDrvG3MacRtObj.pPhyDataSniffer       = NULL;

    /* HAL init */
    gDrvG3MacRtObj.plcHal->init((DRV_PLC_PLIB_INTERFACE *)g3MacRtInit->plcHal->plcPlib);

    /* Update status */
    gDrvG3MacRtObj.state                = DRV_G3_MACRT_STATE_INITIALIZED;

    /* Return the object structure */
    return ( (SYS_MODULE_OBJ)index );

}

DRV_G3_MACRT_STATE DRV_G3_MACRT_Status( const SYS_MODULE_INDEX index )
{
    /* Validate the request */
    if (index >= DRV_G3_MACRT_INSTANCES_NUMBER)
    {
        return SYS_STATUS_ERROR;
    }
    
    /* Return the driver status */
    return (gDrvG3MacRtObj.state);
}

DRV_HANDLE DRV_G3_MACRT_Open(
    const SYS_MODULE_INDEX index,
    const DRV_PLC_BOOT_DATA_CALLBACK callback
)
{
    DRV_PLC_BOOT_INFO bootInfo;
    
    /* Validate the request */
    if (index >= DRV_G3_MACRT_INSTANCES_NUMBER)
    {
        return DRV_HANDLE_INVALID;
    }

    if (gDrvG3MacRtObj.state != DRV_G3_MACRT_STATE_INITIALIZED)
    {
        return DRV_HANDLE_INVALID;
    }
    
    /* Launch boot start process */  
    bootInfo.binSize = gDrvG3MacRtObj.binSize;
    bootInfo.binStartAddress = gDrvG3MacRtObj.binStartAddress;
    bootInfo.pendingLength = gDrvG3MacRtObj.binSize;
    bootInfo.pSrc = gDrvG3MacRtObj.binStartAddress;    
    bootInfo.secure = gDrvG3MacRtObj.secure;
    if (callback)
    {
        bootInfo.bootDataCallback = callback;
        bootInfo.contextBoot = index;
    }
    else
    {
        bootInfo.bootDataCallback = NULL;
        bootInfo.contextBoot = 0;
    }
    
    DRV_PLC_BOOT_Start(&bootInfo, gDrvG3MacRtObj.plcHal);
    
    gDrvG3MacRtObj.state = DRV_G3_MACRT_STATE_BUSY;

    return ((DRV_HANDLE)0);
}

void DRV_G3_MACRT_Close( const DRV_HANDLE handle )
{
    if ((handle != DRV_HANDLE_INVALID) && (handle == 0))
    {
        gDrvG3MacRtObj.state = DRV_G3_MACRT_STATE_UNINITIALIZED;
        
        gDrvG3MacRtObj.plcHal->enableExtInt(false);
    }
}

void DRV_G3_MACRT_InitCallbackRegister( 
    const SYS_MODULE_INDEX index, 
    const DRV_G3_MACRT_INIT_CALLBACK callback
)
{
    /* Validate the request */
    if (index < DRV_G3_MACRT_INSTANCES_NUMBER)
    {
        gDrvG3MacRtObj.initCallback = callback;
    }
}

void DRV_G3_MACRT_TxCfmCallbackRegister( 
    const DRV_HANDLE handle, 
    const DRV_G3_MACRT_TX_CFM_CALLBACK callback
)
{
    if ((handle != DRV_HANDLE_INVALID) && (handle == 0))
    {
        gDrvG3MacRtObj.txCfmCallback = callback;
    }
}

void DRV_G3_MACRT_DataIndCallbackRegister( 
    const DRV_HANDLE handle, 
    const DRV_G3_MACRT_DATA_IND_CALLBACK callback
)
{
    if ((handle != DRV_HANDLE_INVALID) && (handle == 0))
    {
        gDrvG3MacRtObj.dataIndCallback = callback;
    }
}

void DRV_G3_MACRT_RxParamsIndCallbackRegister( 
    const DRV_HANDLE handle, 
    const DRV_G3_MACRT_RX_PARAMS_IND_CALLBACK callback
)
{
    if ((handle != DRV_HANDLE_INVALID) && (handle == 0))
    {
        gDrvG3MacRtObj.rxParamsIndCallback = callback;
    }
}

void DRV_G3_MACRT_MacSnifferCallbackRegister( 
    const DRV_HANDLE handle, 
    const DRV_G3_MACRT_MAC_SNIFFER_IND_CALLBACK callback,
	uint8_t* pDataBuffer
)
{
    if ((handle != DRV_HANDLE_INVALID) && (handle == 0) && 
            (pDataBuffer != NULL))
    {
        gDrvG3MacRtObj.macSnifferIndCallback = callback;
        gDrvG3MacRtObj.pMacDataSniffer = pDataBuffer;
    }
}

void DRV_G3_MACRT_CommStatusCallbackRegister( 
    const DRV_HANDLE handle, 
    const DRV_G3_MACRT_COMM_STATUS_IND_CALLBACK callback
)
{
    if ((handle != DRV_HANDLE_INVALID) && (handle == 0))
    {
        gDrvG3MacRtObj.commStatusIndCallback = callback;
    }
}

void DRV_G3_MACRT_PhySnifferCallbackRegister( 
    const DRV_HANDLE handle, 
    const DRV_G3_MACRT_PHY_SNIFFER_IND_CALLBACK callback,
	uint8_t* pDataBuffer
)
{
    if ((handle != DRV_HANDLE_INVALID) && (handle == 0) && 
            (pDataBuffer != NULL))
    {
        gDrvG3MacRtObj.phySnifferIndCallback = callback;
        gDrvG3MacRtObj.pPhyDataSniffer = pDataBuffer;
    }
}

void DRV_G3_MACRT_ExceptionCallbackRegister( 
    const DRV_HANDLE handle, 
    const DRV_G3_MACRT_EXCEPTION_CALLBACK callback
)
{
    if ((handle != DRV_HANDLE_INVALID) && (handle == 0))
    {
        gDrvG3MacRtObj.exceptionCallback = callback;
    }
}

void DRV_G3_MACRT_Tasks( SYS_MODULE_OBJ hSysObj )
{
    /* Validate the request */
    if (hSysObj >= DRV_G3_MACRT_INSTANCES_NUMBER)
    {
        return;
    }

    if ((gDrvG3MacRtObj.state == DRV_G3_MACRT_STATE_READY) ||
        (gDrvG3MacRtObj.state == DRV_G3_MACRT_STATE_WAITING_TX_CFM))
    {
        /* Run G3 MAC RT communication task */
        DRV_G3_MACRT_Task();
    }
    else if (gDrvG3MacRtObj.state == DRV_G3_MACRT_STATE_BUSY)
    {
        DRV_PLC_BOOT_STATUS state;
        
        /* Check bootloader process */
        state = DRV_PLC_BOOT_Status();
        if (state < DRV_PLC_BOOT_STATUS_READY)
        {
            DRV_PLC_BOOT_Tasks();
        } 
        else if (state == DRV_PLC_BOOT_STATUS_READY)
        {
            DRV_G3_MACRT_Init(&gDrvG3MacRtObj);
            gDrvG3MacRtObj.state = DRV_G3_MACRT_STATE_READY;
            /*if (gDrvG3MacRtObj.sleep && gDrvG3MacRtObj.sleepIndCallback)
            {
                gDrvG3MacRtObj.sleep = false;
                gDrvG3MacRtObj.sleepIndCallback(gDrvG3MacRtObj.contextSleep);
            }*/
            if (gDrvG3MacRtObj.initCallback)
            {
                gDrvG3MacRtObj.initCallback(true);
            }
        }
        else
        {
            gDrvG3MacRtObj.state = DRV_G3_MACRT_STATE_ERROR;
            if (gDrvG3MacRtObj.initCallback)
            {
                gDrvG3MacRtObj.initCallback(false);
            }
        }
    } 
    else
    {
        /* SYS_STATUS_ERROR: Nothing to do */
    }
}

void DRV_G3_MACRT_SleepIndCallbackRegister( 
    const DRV_HANDLE handle, 
    const DRV_G3_MACRT_SLEEP_IND_CALLBACK callback
)
{
    if((handle != DRV_HANDLE_INVALID) && (handle == 0))
    {
        gDrvG3MacRtObj.sleepIndCallback = callback;
    }
}

void DRV_G3_MACRT_Sleep( const DRV_HANDLE handle, bool enable )
{
    if((handle != DRV_HANDLE_INVALID) && (handle == 0))
    {
        if (gDrvG3MacRtObj.sleep != enable)
        {
            if (enable)
            {
                /* Disable PLC interrupt */
                gDrvG3MacRtObj.plcHal->enableExtInt(false);
                /* Set Stand By pin */
                gDrvG3MacRtObj.plcHal->setStandBy(true);
                /* Set Sleep flag */
                gDrvG3MacRtObj.sleep = true;
                gDrvG3MacRtObj.state = DRV_G3_MACRT_STATE_SLEEP;
            }
            else
            {
                /* Clear Stand By pin */
                gDrvG3MacRtObj.plcHal->setStandBy(false);
                
                /* Restart from Sleep mode */
                gDrvG3MacRtObj.state = DRV_G3_MACRT_STATE_BUSY;
                DRV_PLC_BOOT_Restart(DRV_PLC_BOOT_RESTART_SLEEP);
            }
        }
    }
}

void DRV_G3_MACRT_EnableTX( const DRV_HANDLE handle, bool enable )
{
     if((handle != DRV_HANDLE_INVALID) && (handle == 0))
    {
        /* Set Tx Enable pin */
        gDrvG3MacRtObj.plcHal->setTxEnable(enable);
    }
}
   