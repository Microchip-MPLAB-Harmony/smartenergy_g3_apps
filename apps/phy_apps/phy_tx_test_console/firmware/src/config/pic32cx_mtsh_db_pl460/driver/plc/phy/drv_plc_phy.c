/******************************************************************************
  DRV_PLC_PHY Library Interface Implementation

  Company:
    Microchip Technology Inc.

  File Name:
    drv_plc_phy.c

  Summary:
    PLC Driver Library Interface implementation

  Description:
    The PLC Library provides a interface to access the PLC external device.
    This file implements the PLC Library interface.
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
// Section: Include Files
// *****************************************************************************
// *****************************************************************************
#include "configuration.h"
#include "driver/plc/common/drv_plc_boot.h"
#include "driver/plc/phy/drv_plc_phy.h"
#include "driver/plc/phy/drv_plc_phy_local_comm.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data
// *****************************************************************************
// *****************************************************************************

/* This is the driver instance object array. */
static DRV_PLC_PHY_OBJ gDrvPlcPhyObj;

// *****************************************************************************
// *****************************************************************************
// Section: PLC Driver Common Interface Implementation
// *****************************************************************************
// *****************************************************************************

SYS_MODULE_OBJ DRV_PLC_PHY_Initialize(
    const SYS_MODULE_INDEX index,
    const SYS_MODULE_INIT * const init
)
{
    /* MISRA C-2012 deviation block start */
    /* MISRA C-2012 Rule 11.3 deviated once. Deviation record ID - H3_MISRAC_2012_R_11_3_DR_1 */
    const DRV_PLC_PHY_INIT * const plcPhyInit = (const DRV_PLC_PHY_INIT * const)init;
    /* MISRA C-2012 deviation block end */

    /* Validate the request */
    if(index >= DRV_PLC_PHY_INSTANCES_NUMBER)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    if(gDrvPlcPhyObj.inUse == true)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    gDrvPlcPhyObj.status                = SYS_STATUS_UNINITIALIZED;

    gDrvPlcPhyObj.inUse                 = true;
    gDrvPlcPhyObj.nClients              = 0;

    gDrvPlcPhyObj.plcHal                = plcPhyInit->plcHal;
    gDrvPlcPhyObj.nClientsMax           = plcPhyInit->numClients;
    gDrvPlcPhyObj.plcProfile            = plcPhyInit->plcProfile;
    gDrvPlcPhyObj.binSize               = plcPhyInit->binEndAddress - plcPhyInit->binStartAddress;
    gDrvPlcPhyObj.binStartAddress       = plcPhyInit->binStartAddress;
    gDrvPlcPhyObj.secure                = plcPhyInit->secure;

    /* Callbacks initialization */
    gDrvPlcPhyObj.txCfmCallback         = NULL;
    gDrvPlcPhyObj.dataIndCallback       = NULL;
    gDrvPlcPhyObj.exceptionCallback     = NULL;
    gDrvPlcPhyObj.bootDataCallback      = NULL;

    /* HAL init */
    gDrvPlcPhyObj.plcHal->init((DRV_PLC_PLIB_INTERFACE *)plcPhyInit->plcHal->plcPlib);

    /* Update status */
    gDrvPlcPhyObj.status                = SYS_STATUS_BUSY;

    /* Return the object structure */
    return ( (SYS_MODULE_OBJ)index );

}

SYS_STATUS DRV_PLC_PHY_Status( const SYS_MODULE_INDEX index )
{
    /* Avoid warning */
    (void)index;
    /* Return the driver status */
    return (gDrvPlcPhyObj.status);
}

DRV_HANDLE DRV_PLC_PHY_Open(
    const SYS_MODULE_INDEX index,
    const DRV_PLC_BOOT_DATA_CALLBACK callback
)
{
    DRV_PLC_BOOT_INFO bootInfo;

    /* Validate the request */
    if (index >= DRV_PLC_PHY_INSTANCES_NUMBER)
    {
        return DRV_HANDLE_INVALID;
    }

    if((gDrvPlcPhyObj.status != SYS_STATUS_BUSY) || (gDrvPlcPhyObj.inUse == false) \
            || (gDrvPlcPhyObj.nClients >= gDrvPlcPhyObj.nClientsMax))
    {
        return DRV_HANDLE_INVALID;
    }

    /* Launch boot start process */
    bootInfo.binSize = gDrvPlcPhyObj.binSize;
    bootInfo.binStartAddress = gDrvPlcPhyObj.binStartAddress;
    bootInfo.pendingLength = gDrvPlcPhyObj.binSize;
    bootInfo.pSrc = gDrvPlcPhyObj.binStartAddress;
    bootInfo.secure = gDrvPlcPhyObj.secure;
    if (callback != NULL)
    {
        bootInfo.bootDataCallback = callback;
        bootInfo.contextBoot = index;
    }
    else
    {
        bootInfo.bootDataCallback = NULL;
        bootInfo.contextBoot = 0;
    }

    DRV_PLC_BOOT_Start(&bootInfo, gDrvPlcPhyObj.plcHal);

    gDrvPlcPhyObj.nClients++;

    return ((DRV_HANDLE)0);
}

void DRV_PLC_PHY_Close( const DRV_HANDLE handle )
{
    if((handle != DRV_HANDLE_INVALID) && (handle == 0U))
    {
        gDrvPlcPhyObj.nClients--;
        gDrvPlcPhyObj.inUse = false;
        gDrvPlcPhyObj.status = SYS_STATUS_UNINITIALIZED;
    }
}

void DRV_PLC_PHY_TxCfmCallbackRegister(
    const DRV_HANDLE handle,
    const DRV_PLC_PHY_TX_CFM_CALLBACK callback,
    const uintptr_t context
)
{
    if((handle != DRV_HANDLE_INVALID) && (handle == 0U))
    {
        gDrvPlcPhyObj.txCfmCallback = callback;
        gDrvPlcPhyObj.contextCfm = context;
    }
}

void DRV_PLC_PHY_DataIndCallbackRegister(
    const DRV_HANDLE handle,
    const DRV_PLC_PHY_DATA_IND_CALLBACK callback,
    const uintptr_t context
)
{
    if((handle != DRV_HANDLE_INVALID) && (handle == 0U))
    {
        gDrvPlcPhyObj.dataIndCallback = callback;
        gDrvPlcPhyObj.contextInd = context;
    }
}

void DRV_PLC_PHY_ExceptionCallbackRegister(
    const DRV_HANDLE handle,
    const DRV_PLC_PHY_EXCEPTION_CALLBACK callback,
    const uintptr_t context
)
{
    if((handle != DRV_HANDLE_INVALID) && (handle == 0U))
    {
        gDrvPlcPhyObj.exceptionCallback = callback;
        gDrvPlcPhyObj.contextExc = context;
    }
}

void DRV_PLC_PHY_Tasks( SYS_MODULE_OBJ object )
{
    if (gDrvPlcPhyObj.status == SYS_STATUS_READY)
    {
        /* Run PLC communication task */
        DRV_PLC_PHY_Task();
    }
    else if (gDrvPlcPhyObj.status == SYS_STATUS_BUSY)
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
            DRV_PLC_PHY_Init(&gDrvPlcPhyObj);
            gDrvPlcPhyObj.status = SYS_STATUS_READY;
            gDrvPlcPhyObj.state[0] = DRV_PLC_PHY_STATE_IDLE;
        }
        else
        {
            gDrvPlcPhyObj.status = SYS_STATUS_ERROR;
            gDrvPlcPhyObj.state[0] = DRV_PLC_PHY_STATE_IDLE;
        }
    }
    else
    {
        /* SYS_STATUS_ERROR: Nothing to do */
    }
}

void DRV_PLC_PHY_EnableTX( const DRV_HANDLE handle, bool enable )
{
    if((handle != DRV_HANDLE_INVALID) && (handle == 0U))
    {
        /* Set Tx Enable pin */
        gDrvPlcPhyObj.plcHal->setTxEnable(enable);
    }
}
