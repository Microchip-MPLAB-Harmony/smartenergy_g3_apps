/******************************************************************************
  DRV_G3_MACRT Library Interface Implementation

  Company:
    Microchip Technology Inc.

  File Name:
    drv_g3_macrt.c

  Summary:
    G3 MAC RT Driver Library Interface implementation

  Description:
    The G3 MAC RT Library provides an interface to access the PLC external
    device. This file implements the G3 MAC Real Time Library interface.
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
#include "driver/plc/g3MacRt/drv_g3_macrt.h"
#include "driver/plc/g3MacRt/drv_g3_macrt_local_comm.h"
#include "drv_g3_macrt_local.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data
// *****************************************************************************
// *****************************************************************************

/* This is the driver instance object array. */
static DRV_G3_MACRT_OBJ gDrvG3MacRtObj = {.semaphoreID = NULL};

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
    /* MISRA C-2012 deviation block start */
    /* MISRA C-2012 Rule 11.3 deviated once. Deviation record ID - H3_MISRAC_2012_R_11_3_DR_1 */
    const DRV_G3_MACRT_INIT * const g3MacRtInit = (const DRV_G3_MACRT_INIT * const)init;
    /* MISRA C-2012 deviation block end */

    /* Validate the request */
    if (index >= DRV_G3_MACRT_INSTANCES_NUMBER)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    if ((gDrvG3MacRtObj.state != DRV_G3_MACRT_STATE_UNINITIALIZED) && (gDrvG3MacRtObj.state != DRV_G3_MACRT_STATE_INITIALIZED))
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    gDrvG3MacRtObj.plcHal                = g3MacRtInit->plcHal;
    gDrvG3MacRtObj.plcProfile            = g3MacRtInit->plcProfile;
    gDrvG3MacRtObj.binSize               = g3MacRtInit->binEndAddress - g3MacRtInit->binStartAddress;
    gDrvG3MacRtObj.binStartAddress       = g3MacRtInit->binStartAddress;
    gDrvG3MacRtObj.secure                = g3MacRtInit->secure;

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

    if (gDrvG3MacRtObj.semaphoreID == NULL)
    {
        /* Create semaphore. It is used to suspend and resume task */
        OSAL_RESULT semResult = OSAL_SEM_Create(&gDrvG3MacRtObj.semaphoreID, OSAL_SEM_TYPE_BINARY, 0, 0);
        if ((semResult != OSAL_RESULT_SUCCESS) || (gDrvG3MacRtObj.semaphoreID == NULL))
        {
            /* Error: Not enough memory to create semaphore */
            gDrvG3MacRtObj.state = DRV_G3_MACRT_STATE_ERROR;
        }
    }

    /* Return the object structure */
    return ( (SYS_MODULE_OBJ)index );

}

DRV_G3_MACRT_STATE DRV_G3_MACRT_Status( const SYS_MODULE_INDEX index )
{
    /* Validate the request */
    if (index >= DRV_G3_MACRT_INSTANCES_NUMBER)
    {
        return DRV_G3_MACRT_STATE_ERROR;
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

    DRV_PLC_BOOT_Start(&bootInfo, gDrvG3MacRtObj.plcHal);

    gDrvG3MacRtObj.state = DRV_G3_MACRT_STATE_BUSY;

    /* Post semaphore to resume task */
    if (gDrvG3MacRtObj.semaphoreID != NULL)
    {
        (void) OSAL_SEM_Post(&gDrvG3MacRtObj.semaphoreID);
    }

    return ((DRV_HANDLE)0);
}

void DRV_G3_MACRT_Close( const DRV_HANDLE handle )
{
    if ((handle != DRV_HANDLE_INVALID) && (handle == 0U))
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
    if ((handle != DRV_HANDLE_INVALID) && (handle == 0U))
    {
        gDrvG3MacRtObj.txCfmCallback = callback;
    }
}

void DRV_G3_MACRT_DataIndCallbackRegister(
    const DRV_HANDLE handle,
    const DRV_G3_MACRT_DATA_IND_CALLBACK callback
)
{
    if ((handle != DRV_HANDLE_INVALID) && (handle == 0U))
    {
        gDrvG3MacRtObj.dataIndCallback = callback;
    }
}

void DRV_G3_MACRT_RxParamsIndCallbackRegister(
    const DRV_HANDLE handle,
    const DRV_G3_MACRT_RX_PARAMS_IND_CALLBACK callback
)
{
    if ((handle != DRV_HANDLE_INVALID) && (handle == 0U))
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
    if ((handle != DRV_HANDLE_INVALID) && (handle == 0U) &&
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
    if ((handle != DRV_HANDLE_INVALID) && (handle == 0U))
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
    if ((handle != DRV_HANDLE_INVALID) && (handle == 0U) &&
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
    if ((handle != DRV_HANDLE_INVALID) && (handle == 0U))
    {
        gDrvG3MacRtObj.exceptionCallback = callback;
    }
}

void DRV_G3_MACRT_Tasks( SYS_MODULE_OBJ object )
{
    /* Validate the request */
    if (object >= DRV_G3_MACRT_INSTANCES_NUMBER)
    {
        return;
    }

    /* Suspend task until semaphore is posted or timeout expires */
    if (gDrvG3MacRtObj.semaphoreID != NULL)
    {
        uint16_t waitMS = 1;

        /* If PLC device is running, wait forever. Otherwise, wait for 1 ms. */
        if ((gDrvG3MacRtObj.state == DRV_G3_MACRT_STATE_READY) ||
            (gDrvG3MacRtObj.state == DRV_G3_MACRT_STATE_WAITING_TX_CFM))
        {
            waitMS = (uint16_t)OSAL_WAIT_FOREVER;
        }

        (void) OSAL_SEM_Pend(&gDrvG3MacRtObj.semaphoreID, waitMS);
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
            if (gDrvG3MacRtObj.initCallback != NULL)
            {
                gDrvG3MacRtObj.initCallback(true);
            }
        }
        else
        {
            gDrvG3MacRtObj.state = DRV_G3_MACRT_STATE_ERROR;
            if (gDrvG3MacRtObj.initCallback != NULL)
            {
                gDrvG3MacRtObj.initCallback(false);
            }
        }
    }
    else
    {
        /* DRV_G3_MACRT_STATE_ERROR: Nothing to do */
    }
}

void DRV_G3_MACRT_EnableTX( const DRV_HANDLE handle, bool enable )
{
    if((handle != DRV_HANDLE_INVALID) && (handle == 0U))
    {
        /* Set Tx Enable pin */
        gDrvG3MacRtObj.plcHal->setTxEnable(enable);
    }
}
