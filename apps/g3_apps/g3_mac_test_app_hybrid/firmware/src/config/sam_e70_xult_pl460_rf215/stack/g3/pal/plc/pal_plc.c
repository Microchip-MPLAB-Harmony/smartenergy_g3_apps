/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    pal_plc.c

  Summary:
    Platform Abstraction Layer PLC (PAL PLC) Interface source file.

  Description:
    Platform Abstraction Layer PLC (PAL PLC) Interface header.
    The PAL PLC module provides a simple interface to manage the PLC PHY driver.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2022 Microchip Technology Inc. and its subsidiaries.
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
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "configuration.h"
#include "driver/driver.h"
#include "driver/plc/g3MacRt/drv_g3_macrt.h"
#include "service/pcoup/srv_pcoup.h"
#include "service/pvddmon/srv_pvddmon.h"
#include "pal_plc.h"

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Variables
// *****************************************************************************
// *****************************************************************************
/* G3 MAC RT Driver Initialization Data (initialization.c) */
extern DRV_G3_MACRT_INIT drvG3MacRtInitData;
extern uint8_t g3_mac_rt_bin_start;
extern uint8_t g3_mac_rt_bin_end;
extern uint8_t g3_mac_rt_bin2_start;
extern uint8_t g3_mac_rt_bin2_end;

static PAL_PLC_DATA palPlcData = {0};

// *****************************************************************************
// *****************************************************************************
// Section: local functions
// *****************************************************************************
// *****************************************************************************
static void _palPlcGetMibBackupInfo(void)
{
	palPlcData.plcPIB.pib = MAC_RT_PIB_GET_SET_ALL_MIB;
    palPlcData.plcPIB.index = 0;
    palPlcData.plcPIB.length = sizeof(MAC_RT_MIB_INIT_OBJ);
    DRV_G3_MACRT_PIBGet(palPlcData.drvG3MacRtHandle, &palPlcData.plcPIB);
    
	memcpy(&palPlcData.mibInitData, palPlcData.plcPIB.pData, 
            sizeof(MAC_RT_MIB_INIT_OBJ));
}

static void _palPlcSetMibBackupInfo(void)
{
    memcpy(palPlcData.plcPIB.pData, &palPlcData.mibInitData, 
            sizeof(MAC_RT_MIB_INIT_OBJ));
    
	palPlcData.plcPIB.pib = MAC_RT_PIB_GET_SET_ALL_MIB;
    palPlcData.plcPIB.index = 0;
    palPlcData.plcPIB.length = sizeof(MAC_RT_MIB_INIT_OBJ);
    DRV_G3_MACRT_PIBSet(palPlcData.drvG3MacRtHandle, &palPlcData.plcPIB);
    
}

static void _palPlcUpdateMibBackupInfo(MAC_RT_PIB pib, uint8_t *pValue)
{
	switch (pib) {
        case MAC_RT_PIB_CSMA_NO_ACK_COUNT:
            palPlcData.mibInitData.csmaNoAckCount = *(uint32_t *)pValue;
            break;

        case MAC_RT_PIB_BAD_CRC_COUNT:
            palPlcData.mibInitData.badCrcCount = *(uint32_t *)pValue;
            break;

        case MAC_RT_PIB_MANUF_RX_SEGMENT_DECODE_ERROR_COUNT:
            palPlcData.mibInitData.rxSegmentDecodeErrorCount = *(uint32_t *)pValue;
            break;

        case MAC_RT_PIB_PAN_ID:
            palPlcData.mibInitData.panId = *(uint16_t *)pValue;
            break;

        case MAC_RT_PIB_SHORT_ADDRESS:
            palPlcData.mibInitData.shortAddress = *(uint16_t *)pValue;
            break;

        case MAC_RT_PIB_TONE_MASK:
            memcpy((uint8_t *)&palPlcData.mibInitData.toneMask, pValue, 
                    sizeof(MAC_RT_TONE_MASK));
            break;

        case MAC_RT_PIB_MANUF_EXTENDED_ADDRESS:
            memcpy((uint8_t *)&palPlcData.mibInitData.extendedAddress, pValue, 
                    sizeof(MAC_RT_EXT_ADDRESS));
            break;

        case MAC_RT_PIB_MANUF_FORCED_TONEMAP:
            memcpy((uint8_t *)&palPlcData.mibInitData.forcedToneMap, pValue, 
                    sizeof(MAC_RT_TONE_MAP));
            break;

        case MAC_RT_PIB_HIGH_PRIORITY_WINDOW_SIZE:
            palPlcData.mibInitData.highPriorityWindowSize = *pValue;
            break;

        case MAC_RT_PIB_CSMA_FAIRNESS_LIMIT:
            palPlcData.mibInitData.csmaFairnessLimit = *pValue;
            break;

        case MAC_RT_PIB_A:
            palPlcData.mibInitData.A = *pValue;
            break;

        case MAC_RT_PIB_K:
            palPlcData.mibInitData.K = *pValue;
            break;

        case MAC_RT_PIB_MIN_CW_ATTEMPTS:
            palPlcData.mibInitData.minCwAttempts = *pValue;
            break;

        case MAC_RT_PIB_MAX_BE:
            palPlcData.mibInitData.maxBe = *pValue;
            break;

        case MAC_RT_PIB_MAX_CSMA_BACKOFFS:
            palPlcData.mibInitData.maxCsmaBackoffs = *pValue;
            break;

        case MAC_RT_PIB_MAX_FRAME_RETRIES:
            palPlcData.mibInitData.maxFrameRetries = *pValue;
            break;

        case MAC_RT_PIB_MIN_BE:
            palPlcData.mibInitData.minBe = *pValue;
            break;

        case MAC_RT_PIB_MANUF_FORCED_MOD_SCHEME:
            palPlcData.mibInitData.forcedModScheme = *pValue;
            break;

        case MAC_RT_PIB_MANUF_FORCED_MOD_TYPE:
            palPlcData.mibInitData.forcedModType = *pValue;
            break;

        case MAC_RT_PIB_MANUF_RETRIES_LEFT_TO_FORCE_ROBO:
            palPlcData.mibInitData.retriesToForceRobo = *pValue;
            break;

        case MAC_RT_PIB_TRANSMIT_ATTEN:
            palPlcData.mibInitData.transmitAtten = *pValue;
            break;

        case MAC_RT_PIB_BROADCAST_MAX_CW_ENABLE:
            palPlcData.mibInitData.broadcastMaxCwEnable = *pValue;
            break;

        case MAC_RT_PIB_PREAMBLE_LENGTH:
            palPlcData.mibInitData.preambleLength = *pValue;
            break;

        case MAC_RT_PIB_GET_SET_ALL_MIB:
            memcpy((uint8_t *)&palPlcData.mibInitData, pValue, 
                    sizeof(MAC_RT_MIB_INIT_OBJ));
            break;

        default:
            break;
	}
}

static void _palPlcSetInitialConfiguration ( void )
{
    /* Apply PLC coupling configuration */
    SRV_PCOUP_Set_Config(palPlcData.drvG3MacRtHandle, palPlcData.plcBranch);

    /* Force Transmission to VLO mode by default in order to maximize signal level in any case */
    /* Disable auto-detect mode */
    palPlcData.plcPIB.pib = MAC_RT_PIB_MANUF_PHY_PARAM;
    palPlcData.plcPIB.index = PHY_PIB_CFG_AUTODETECT_IMPEDANCE;
    palPlcData.plcPIB.length = 1;
    palPlcData.plcPIB.pData[0] = 0;
    DRV_G3_MACRT_PIBSet(palPlcData.drvG3MacRtHandle, &palPlcData.plcPIB);

    /* Set VLO mode */
    palPlcData.plcPIB.index = PHY_PIB_CFG_IMPEDANCE;
    palPlcData.plcPIB.length = 1;
    palPlcData.plcPIB.pData[0] = 2;
    DRV_G3_MACRT_PIBSet(palPlcData.drvG3MacRtHandle, &palPlcData.plcPIB);

}

// *****************************************************************************
// *****************************************************************************
// Section: local callbacks
// *****************************************************************************
// *****************************************************************************
static void _palPlcPVDDMonitorCb( SRV_PVDDMON_CMP_MODE cmpMode, uintptr_t context )
{
    (void)context;
    
    if (cmpMode == SRV_PVDDMON_CMP_MODE_OUT)
    {
        /* PLC Transmission is not permitted */
        DRV_G3_MACRT_EnableTX(palPlcData.drvG3MacRtHandle, false);
        palPlcData.pvddMonTxEnable = false;
        /* Restart PVDD Monitor to check when VDD is within the comparison window */
        SRV_PVDDMON_Restart(SRV_PVDDMON_CMP_MODE_IN);
    }
    else
    {
        /* PLC Transmission is permitted again */
        DRV_G3_MACRT_EnableTX(palPlcData.drvG3MacRtHandle, true);
        palPlcData.pvddMonTxEnable = true;
        /* Restart PVDD Monitor to check when VDD is out of the comparison window */
        SRV_PVDDMON_Restart(SRV_PVDDMON_CMP_MODE_OUT);
    }
}

static void _palPlcExceptionCb( DRV_G3_MACRT_EXCEPTION exceptionObj )
{
    switch (exceptionObj) 
    {
        case DRV_G3_MACRT_EXCEPTION_UNEXPECTED_KEY:
            palPlcData.statsErrorUnexpectedKey++;
            break;

        case DRV_G3_MACRT_EXCEPTION_RESET:
            palPlcData.statsErrorReset++;
            break;
	}
    
    /* Set restart Mib flag */
    palPlcData.restartMib = false;
    palPlcData.status = PAL_PLC_STATUS_ERROR;
}

static void _palPlcDataCfmCb( MAC_RT_TX_CFM_OBJ *cfmObj )
{
    palPlcData.waitingTxCfm = false;
    
    if (palPlcData.initHandlers.palPlcTxConfirm)
    {
        palPlcData.initHandlers.palPlcTxConfirm(cfmObj->status, cfmObj->updateTimestamp);
    }
}

static void _palPlcDataIndCb( uint8_t *pData, uint16_t length )
{
    /* TBD : Led handling. Led On -> Timer Callback : Led Off (Timer dependencies) ? */
    
    if (palPlcData.initHandlers.palPlcDataIndication)
    {
        palPlcData.initHandlers.palPlcDataIndication(pData, length);
    }
}

static void _palPlcInitCallback(bool initResult)
{
    if (initResult == true) 
    {
        /* Configure PLC callbacks */
        DRV_G3_MACRT_ExceptionCallbackRegister(palPlcData.drvG3MacRtHandle, 
                _palPlcExceptionCb);
        DRV_G3_MACRT_TxCfmCallbackRegister(palPlcData.drvG3MacRtHandle, 
                _palPlcDataCfmCb);
        DRV_G3_MACRT_DataIndCallbackRegister(palPlcData.drvG3MacRtHandle, 
                _palPlcDataIndCb);
        DRV_G3_MACRT_RxParamsIndCallbackRegister(palPlcData.drvG3MacRtHandle, 
                palPlcData.initHandlers.palPlcRxParamsIndication);
        DRV_G3_MACRT_CommStatusCallbackRegister(palPlcData.drvG3MacRtHandle, 
                palPlcData.initHandlers.palPlcCommStatusIndication);

        /* Apply PLC initial configuration */
        _palPlcSetInitialConfiguration();
        
        if (palPlcData.restartMib)
        {
            /* Get MIB backup info by default from the MAC RT driver */
            _palPlcGetMibBackupInfo();
            
            /* Clear restart Mib flag */
            palPlcData.restartMib = false;
        }
        else
        {
            /* Set MIB backup info to the MAC RT driver */
            _palPlcSetMibBackupInfo();
        }

        /* Enable PLC Transmission */
        DRV_G3_MACRT_EnableTX(palPlcData.drvG3MacRtHandle, true);

        /* Enable PLC PVDD Monitor Service */
        SRV_PVDDMON_CallbackRegister(_palPlcPVDDMonitorCb, 0);
        SRV_PVDDMON_Start(SRV_PVDDMON_CMP_MODE_OUT);

        palPlcData.status = PAL_PLC_STATUS_READY;
        
        /* Check pending PLC transmissions */
        if (palPlcData.waitingTxCfm)
        {
            MAC_RT_TX_CFM_OBJ cfmObj;
            
            cfmObj.updateTimestamp = false;
            cfmObj.status = MAC_RT_STATUS_CHANNEL_ACCESS_FAILURE;
            _palPlcDataCfmCb(&cfmObj);
        }
    } 
    else 
    {
        palPlcData.status = PAL_PLC_STATUS_ERROR;
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Interface Function Definitions
// *****************************************************************************
// *****************************************************************************

SYS_MODULE_OBJ PAL_PLC_Initialize(const SYS_MODULE_INDEX index, 
        const SYS_MODULE_INIT * const init)
{
    PAL_PLC_INIT *palInit = (PAL_PLC_INIT *)init;
    MAC_RT_BAND plcBandMain;
    MAC_RT_BAND plcBandAux;
    bool macRtInitFlag = false;
    
    /* Check Single instance */
    if (index != PAL_PLC_PHY_INDEX)
    {
        return SYS_MODULE_OBJ_INVALID;
    }
    
    /* Check previously initialized */
    if (palPlcData.status != PAL_PLC_STATUS_UNINITIALIZED)
    {
        return SYS_MODULE_OBJ_INVALID;
    }
        
    palPlcData.initHandlers = palInit->macRtHandlers;
    palPlcData.plcBand = palInit->macRtBand;
    palPlcData.restartMib = palInit->initMIB;
    
    /* Clear exceptions statistics */
    palPlcData.statsErrorUnexpectedKey = 0;
    palPlcData.statsErrorReset = 0;

    /* Set PVDD Monitor tracking data */
    palPlcData.pvddMonTxEnable = true;

    palPlcData.waitingTxCfm = false;
    
    /* Manage G3 PLC Band */
    plcBandMain = (MAC_RT_BAND)SRV_PCOUP_Get_Phy_Band(SRV_PLC_PCOUP_MAIN_BRANCH);
    plcBandAux = (MAC_RT_BAND)SRV_PCOUP_Get_Phy_Band(SRV_PLC_PCOUP_AUXILIARY_BRANCH);
    if (plcBandMain == palPlcData.plcBand)
    {
        palPlcData.plcBranch = SRV_PLC_PCOUP_MAIN_BRANCH;
        if (drvG3MacRtInitData.binStartAddress != (uint32_t)&g3_mac_rt_bin_start)
        {
            drvG3MacRtInitData.binStartAddress = (uint32_t)&g3_mac_rt_bin_start;
            drvG3MacRtInitData.binEndAddress = (uint32_t)&g3_mac_rt_bin_end;
            macRtInitFlag = true;
        }
    } 
    else if (plcBandAux == palPlcData.plcBand) 
    {
        palPlcData.plcBranch = SRV_PLC_PCOUP_AUXILIARY_BRANCH;
        if (drvG3MacRtInitData.binStartAddress != (uint32_t)&g3_mac_rt_bin2_start)
        {
            drvG3MacRtInitData.binStartAddress = (uint32_t)&g3_mac_rt_bin2_start;
            drvG3MacRtInitData.binEndAddress = (uint32_t)&g3_mac_rt_bin2_end;
            macRtInitFlag = true;
        }
    }
    else
    {
        return SYS_MODULE_OBJ_INVALID; 
    }
    
    if (macRtInitFlag)
    {
        /* Initialize PLC Driver Instance */
        DRV_G3_MACRT_Initialize(DRV_G3_MACRT_INDEX, (SYS_MODULE_INIT *)&drvG3MacRtInitData);
    }
    
    DRV_G3_MACRT_InitCallbackRegister(DRV_G3_MACRT_INDEX, _palPlcInitCallback);
    
    /* Open PLC driver */
    palPlcData.drvG3MacRtHandle = DRV_G3_MACRT_Open(DRV_G3_MACRT_INDEX, NULL);

    if (palPlcData.drvG3MacRtHandle != PAL_PLC_HANDLE_INVALID)
    {
        palPlcData.status = PAL_PLC_STATUS_BUSY;
        return (SYS_MODULE_OBJ)PAL_PLC_PHY_INDEX;
    }
    else
    {
        palPlcData.status = PAL_PLC_STATUS_ERROR;
        return SYS_MODULE_OBJ_INVALID;
    }
}

PAL_PLC_HANDLE PAL_PLC_HandleGet(const SYS_MODULE_INDEX index)
{    
    /* Check Single instance */
    if (index != PAL_PLC_PHY_INDEX)
    {
        return PAL_PLC_HANDLE_INVALID;
    }

    return (PAL_PLC_HANDLE)&palPlcData;
}

PAL_PLC_STATUS PAL_PLC_Status(SYS_MODULE_OBJ object)
{
    if (object != (SYS_MODULE_OBJ)PAL_PLC_PHY_INDEX)
    {
        return PAL_PLC_STATUS_INVALID_OBJECT;
    }
    
    return palPlcData.status;
}
 
void PAL_PLC_Deinitialize(SYS_MODULE_OBJ object)
{
    if (object != (SYS_MODULE_OBJ)PAL_PLC_PHY_INDEX)
    {
        return;
    }
    
    palPlcData.status = PAL_PLC_STATUS_UNINITIALIZED;
    
    DRV_G3_MACRT_InitCallbackRegister(DRV_G3_MACRT_INDEX, NULL);
    DRV_G3_MACRT_Close(palPlcData.drvG3MacRtHandle);
}
 
void PAL_PLC_TxRequest(PAL_PLC_HANDLE handle, uint8_t *pData, 
        uint16_t length)
{
    MAC_RT_TX_CFM_OBJ cfmObj;
    
    cfmObj.updateTimestamp = true;
    
    if (handle != (PAL_PLC_HANDLE)&palPlcData)
    {
        cfmObj.status = MAC_RT_STATUS_DENIED;
        cfmObj.updateTimestamp = false;
    }
    
    if (palPlcData.status != PAL_PLC_STATUS_READY)
    {
        cfmObj.status = MAC_RT_STATUS_DENIED;
        cfmObj.updateTimestamp = false;
    }

    if (palPlcData.pvddMonTxEnable == false)
    {
        cfmObj.status = MAC_RT_STATUS_DENIED;
        cfmObj.updateTimestamp = false;
    }

    if (cfmObj.updateTimestamp)
    {
        palPlcData.waitingTxCfm = true;
        DRV_G3_MACRT_TxRequest(palPlcData.drvG3MacRtHandle, pData, length);
    }
    else
    {
        palPlcData.waitingTxCfm = false;
        _palPlcDataCfmCb(&cfmObj);
    }
}
 
void PAL_PLC_Reset(PAL_PLC_HANDLE handle, bool resetMib)
{
    PAL_PLC_INIT palInit;
    
    if (handle != (PAL_PLC_HANDLE)&palPlcData)
    {
        return;
    }
    
    palInit.macRtHandlers = palPlcData.initHandlers;
    palInit.macRtBand = palPlcData.plcBand;
    palInit.initMIB = resetMib;
    
    PAL_PLC_Deinitialize(PAL_PLC_PHY_INDEX);
    PAL_PLC_Initialize(PAL_PLC_PHY_INDEX, (const SYS_MODULE_INIT * const)&palInit);
}
 
uint32_t PAL_PLC_GetPhyTime(PAL_PLC_HANDLE handle)
{
    if (handle != (PAL_PLC_HANDLE)&palPlcData)
    {
        return 0;
    }
    
    if (palPlcData.status != PAL_PLC_STATUS_READY)
    {
        return 0;
    }
    
    return DRV_G3_MACRT_GetTimerReference(handle);
}

PAL_PLC_PIB_RESULT PAL_PLC_GetMacRtPib(PAL_PLC_HANDLE handle, MAC_RT_PIB_OBJ *pibObj)
{
    if (handle != (PAL_PLC_HANDLE)&palPlcData)
    {
        return PAL_PLC_PIB_INVALID_PARAMETER;
    }
    
    if (palPlcData.status != PAL_PLC_STATUS_READY)
    {
        /* Ignore request */
        return PAL_PLC_PIB_DENIED;
    }
    
    return (PAL_PLC_PIB_RESULT)DRV_G3_MACRT_PIBGet(palPlcData.drvG3MacRtHandle, pibObj);
}

PAL_PLC_PIB_RESULT PAL_PLC_SetMacRtPib(PAL_PLC_HANDLE handle, MAC_RT_PIB_OBJ *pibObj)
{
    PAL_PLC_PIB_RESULT result; 
    
    if (handle != (PAL_PLC_HANDLE)&palPlcData)
    {
        return PAL_PLC_PIB_INVALID_PARAMETER;
    }
    
    if (palPlcData.status != PAL_PLC_STATUS_READY)
    {
        /* Ignore request */
        return PAL_PLC_PIB_DENIED;
    }
    
    result = (PAL_PLC_PIB_RESULT)DRV_G3_MACRT_PIBSet(palPlcData.drvG3MacRtHandle, 
            pibObj);
    
    if (result == PAL_PLC_PIB_SUCCESS)
    {
        /* Update Backup MIB info */
        _palPlcUpdateMibBackupInfo(pibObj->pib, pibObj->pData);
    }
            
    return result;
}
