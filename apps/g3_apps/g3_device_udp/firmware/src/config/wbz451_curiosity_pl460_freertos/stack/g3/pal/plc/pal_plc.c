/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    pal_plc.c

  Summary:
    PLC Platform Abstraction Layer (PAL) Interface source file.

  Description:
    PLC Platform Abstraction Layer (PAL) Interface source file. The PLC PAL
    module provides a simple interface to manage the G3-PLC MAC-RT layer.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2023 Microchip Technology Inc. and its subsidiaries.
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
#include "driver/plc/g3MacRt/drv_g3_macrt.h"
#include "service/pcoup/srv_pcoup.h"
#include "pal_plc_local.h"

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Variables
// *****************************************************************************
// *****************************************************************************
/* G3 MAC RT Driver Initialization Data (initialization.c) */
extern DRV_G3_MACRT_INIT drvG3MacRtInitData;

static PAL_PLC_DATA palPlcData = {0};

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Functions
// *****************************************************************************
// *****************************************************************************

static void lPAL_PLC_GetMibBackupInfo(void)
{
    palPlcData.plcPIB.pib = MAC_RT_PIB_GET_SET_ALL_MIB;
    palPlcData.plcPIB.index = 0;
    palPlcData.plcPIB.length = (uint8_t)sizeof(MAC_RT_MIB_INIT_OBJ);
    (void) DRV_G3_MACRT_PIBGet(palPlcData.drvG3MacRtHandle, &palPlcData.plcPIB);

    (void) memcpy((uint8_t *)&palPlcData.mibInitData, palPlcData.plcPIB.pData,
            sizeof(MAC_RT_MIB_INIT_OBJ));
}

static void lPAL_PLC_SetMibBackupInfo(void)
{
    (void) memcpy(palPlcData.plcPIB.pData, (uint8_t *)&palPlcData.mibInitData,
            sizeof(MAC_RT_MIB_INIT_OBJ));
    
    palPlcData.plcPIB.pib = MAC_RT_PIB_GET_SET_ALL_MIB;
    palPlcData.plcPIB.index = 0;
    palPlcData.plcPIB.length = (uint8_t)sizeof(MAC_RT_MIB_INIT_OBJ);
    (void) DRV_G3_MACRT_PIBSet(palPlcData.drvG3MacRtHandle, &palPlcData.plcPIB);
    
}

static void lPAL_PLC_UpdateMibBackupInfo(MAC_RT_PIB pib, void *pValue)
{
    switch (pib) {
        case MAC_RT_PIB_PAN_ID:
            palPlcData.mibInitData.panId = *(uint16_t *)pValue;
            break;

        case MAC_RT_PIB_SHORT_ADDRESS:
            palPlcData.mibInitData.shortAddress = *(uint16_t *)pValue;
            break;

        case MAC_RT_PIB_RC_COORD:
            palPlcData.mibInitData.rcCoord = *(uint16_t *)pValue;
            break;

        case MAC_RT_PIB_TONE_MASK:
            (void) memcpy(palPlcData.mibInitData.toneMask.toneMask, (uint8_t *)pValue,
                    sizeof(MAC_RT_TONE_MASK));
            break;

        case MAC_RT_PIB_MANUF_EXTENDED_ADDRESS:
            (void) memcpy(palPlcData.mibInitData.extendedAddress.address, (uint8_t *)pValue,
                    sizeof(MAC_RT_EXT_ADDRESS));
            break;

        case MAC_RT_PIB_MANUF_FORCED_TONEMAP:
            (void) memcpy(palPlcData.mibInitData.forcedToneMap.toneMap, (uint8_t *)pValue,
                    sizeof(MAC_RT_TONE_MAP));
            break;

        case MAC_RT_PIB_MANUF_FORCED_TONEMAP_ON_TMRESPONSE:
            (void) memcpy(palPlcData.mibInitData.forcedToneMapOnTMResponse.toneMap, (uint8_t *)pValue,
                    sizeof(MAC_RT_TONE_MAP));
            break;

        case MAC_RT_PIB_HIGH_PRIORITY_WINDOW_SIZE:
            palPlcData.mibInitData.highPriorityWindowSize = *(uint8_t *)pValue;
            break;

        case MAC_RT_PIB_CSMA_FAIRNESS_LIMIT:
            palPlcData.mibInitData.csmaFairnessLimit = *(uint8_t *)pValue;
            break;

        case MAC_RT_PIB_A:
            palPlcData.mibInitData.A = *(uint8_t *)pValue;
            break;

        case MAC_RT_PIB_K:
            palPlcData.mibInitData.K = *(uint8_t *)pValue;
            break;

        case MAC_RT_PIB_MIN_CW_ATTEMPTS:
            palPlcData.mibInitData.minCwAttempts = *(uint8_t *)pValue;
            break;

        case MAC_RT_PIB_MAX_BE:
            palPlcData.mibInitData.maxBe = *(uint8_t *)pValue;
            break;

        case MAC_RT_PIB_BSN:
            palPlcData.mibInitData.bsn = *(uint8_t *)pValue;
            break;

        case MAC_RT_PIB_DSN:
            palPlcData.mibInitData.dsn = *(uint8_t *)pValue;
            break;

        case MAC_RT_PIB_MAX_CSMA_BACKOFFS:
            palPlcData.mibInitData.maxCsmaBackoffs = *(uint8_t *)pValue;
            break;

        case MAC_RT_PIB_MAX_FRAME_RETRIES:
            palPlcData.mibInitData.maxFrameRetries = *(uint8_t *)pValue;
            break;

        case MAC_RT_PIB_MIN_BE:
            palPlcData.mibInitData.minBe = *(uint8_t *)pValue;
            break;

        case MAC_RT_PIB_MANUF_FORCED_MOD_SCHEME:
            palPlcData.mibInitData.forcedModScheme = *(uint8_t *)pValue;
            break;

        case MAC_RT_PIB_MANUF_FORCED_MOD_TYPE:
            palPlcData.mibInitData.forcedModType = *(uint8_t *)pValue;
            break;

        case MAC_RT_PIB_MANUF_FORCED_MOD_SCHEME_ON_TMRESPONSE:
            palPlcData.mibInitData.forcedModSchemeOnTMResponse = *(uint8_t *)pValue;
            break;

        case MAC_RT_PIB_MANUF_FORCED_MOD_TYPE_ON_TMRESPONSE:
            palPlcData.mibInitData.forcedModTypeOnTMResponse = *(uint8_t *)pValue;
            break;

        case MAC_RT_PIB_MANUF_RETRIES_LEFT_TO_FORCE_ROBO:
            palPlcData.mibInitData.retriesToForceRobo = *(uint8_t *)pValue;
            break;

        case MAC_RT_PIB_TRANSMIT_ATTEN:
            palPlcData.mibInitData.transmitAtten = *(uint8_t *)pValue;
            break;

        case MAC_RT_PIB_POS_TABLE_ENTRY_TTL:
            palPlcData.mibInitData.posTableEntryTtl = *(uint8_t *)pValue;
            break;

        case MAC_RT_PIB_POS_RECENT_ENTRY_THRESHOLD:
            palPlcData.mibInitData.posRecentEntryThreshold = *(uint8_t *)pValue;
            break;

        case MAC_RT_PIB_MANUF_TRICKLE_MIN_LQI:
            palPlcData.mibInitData.trickleMinLQI = *(uint8_t *)pValue;
            break;

        case MAC_RT_PIB_DUPLICATE_DETECTION_TTL:
            palPlcData.mibInitData.duplicateDetectionTtl = *(uint8_t *)pValue;
            break;

        case MAC_RT_PIB_TMR_TTL:
            palPlcData.mibInitData.tmrTtl = *(uint8_t *)pValue;
            break;

        case MAC_RT_PIB_BEACON_RANDOMIZATION_WINDOW_LENGTH:
            palPlcData.mibInitData.beaconRandomizationWindowLength = *(uint8_t *)pValue;
            break;

        case MAC_RT_PIB_PREAMBLE_LENGTH:
            palPlcData.mibInitData.preambleLength = *(uint8_t *)pValue;
            break;

        case MAC_RT_PIB_BROADCAST_MAX_CW_ENABLE:
            palPlcData.mibInitData.broadcastMaxCwEnable = *(bool *)pValue;
            break;

        case MAC_RT_PIB_PROMISCUOUS_MODE:
            palPlcData.mibInitData.promiscuousMode = *(bool *)pValue;
            break;

        case MAC_RT_PIB_MANUF_ENABLE_MAC_SNIFFER:
            palPlcData.mibInitData.macSniffer = *(bool *)pValue;
            break;

        case MAC_RT_PIB_TX_HIGH_PRIORITY:
            palPlcData.mibInitData.txHighPriority = *(bool *)pValue;
            break;

        case MAC_RT_PIB_GET_SET_ALL_MIB:
            palPlcData.mibInitData = *(MAC_RT_MIB_INIT_OBJ *)pValue;
            break;

        /* MISRA C-2012 deviation block start */
        /* MISRA C-2012 Rule 16.4 deviated once. Deviation record ID - H3_MISRAC_2012_R_16_4_DR_1 */

        default:
            break;

    }
}

static void lPAL_PLC_SetInitialConfiguration ( void )
{
    /* Apply PLC coupling configuration */
    (void) SRV_PCOUP_Set_Config(palPlcData.drvG3MacRtHandle, palPlcData.plcBranch);
}

// *****************************************************************************
// *****************************************************************************
// Section: Local Callbacks
// *****************************************************************************
// *****************************************************************************

static void lPAL_PLC_ExceptionCb( DRV_G3_MACRT_EXCEPTION exceptionObj )
{
    if (exceptionObj == DRV_G3_MACRT_EXCEPTION_UNEXPECTED_KEY)
    {
        palPlcData.statsErrorUnexpectedKey++;
    }

    if (exceptionObj == DRV_G3_MACRT_EXCEPTION_RESET)
    {
        palPlcData.statsErrorReset++;
    }

    /* Set restart Mib flag */
    palPlcData.restartMib = false;
    palPlcData.status = PAL_PLC_STATUS_ERROR;
}

static void lPAL_PLC_DataCfmCb( MAC_RT_TX_CFM_OBJ *cfmObj )
{
    palPlcData.waitingTxCfm = false;
    
    if (palPlcData.initHandlers.palPlcTxConfirm != NULL)
    {
        palPlcData.initHandlers.palPlcTxConfirm(cfmObj->status, cfmObj->updateTimestamp);
    }
}

static void lPAL_PLC_DataIndCb( uint8_t *pData, uint16_t length )
{
    /* TBD : Led handling. Led On -> Timer Callback : Led Off (Timer dependencies) ? */
    
    if (palPlcData.initHandlers.palPlcDataIndication != NULL)
    {
        palPlcData.initHandlers.palPlcDataIndication(pData, length);
    }
}

static void lPAL_PLC_InitCallback(bool initResult)
{
    if (initResult == true) 
    {
        /* Configure PLC callbacks */
        DRV_G3_MACRT_ExceptionCallbackRegister(palPlcData.drvG3MacRtHandle,
                lPAL_PLC_ExceptionCb);
        DRV_G3_MACRT_TxCfmCallbackRegister(palPlcData.drvG3MacRtHandle,
                lPAL_PLC_DataCfmCb);
        DRV_G3_MACRT_DataIndCallbackRegister(palPlcData.drvG3MacRtHandle,
                lPAL_PLC_DataIndCb);
        DRV_G3_MACRT_RxParamsIndCallbackRegister(palPlcData.drvG3MacRtHandle,
                palPlcData.initHandlers.palPlcRxParamsIndication);
        DRV_G3_MACRT_CommStatusCallbackRegister(palPlcData.drvG3MacRtHandle,
                palPlcData.initHandlers.palPlcCommStatusIndication);

        /* Apply PLC initial configuration */
        lPAL_PLC_SetInitialConfiguration();

        if (palPlcData.coordinator)
        {
            /* Restore coordinator configuration */
            DRV_G3_MACRT_SetCoordinator(palPlcData.drvG3MacRtHandle);
        }

        if (palPlcData.restartMib)
        {
            /* Get MIB backup info by default from the MAC RT driver */
            lPAL_PLC_GetMibBackupInfo();
            
            /* Clear restart Mib flag */
            palPlcData.restartMib = false;
        }
        else
        {
            /* Set MIB backup info to the MAC RT driver */
            lPAL_PLC_SetMibBackupInfo();
        }

        /* Enable PLC Transmission */
        DRV_G3_MACRT_EnableTX(palPlcData.drvG3MacRtHandle, true);

        palPlcData.status = PAL_PLC_STATUS_READY;
        
        /* Check pending PLC transmissions */
        if (palPlcData.waitingTxCfm)
        {
            MAC_RT_TX_CFM_OBJ cfmObj;
            
            cfmObj.updateTimestamp = false;
            cfmObj.status = MAC_RT_STATUS_CHANNEL_ACCESS_FAILURE;
            lPAL_PLC_DataCfmCb(&cfmObj);
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

/* MISRA C-2012 deviation block start */
/* MISRA C-2012 Rule 11.3 deviated twice. Deviation record ID - H3_MISRAC_2012_R_11_3_DR_1 */

SYS_MODULE_OBJ PAL_PLC_Initialize(const SYS_MODULE_INDEX index, 
        const SYS_MODULE_INIT * const init)
{
    const PAL_PLC_INIT * const palInit = (const PAL_PLC_INIT * const)init;
    MAC_RT_BAND plcBandMain;
    MAC_RT_BAND plcBandAux;
    
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
    palPlcData.coordinator = false;
    
    /* Clear exceptions statistics */
    palPlcData.statsErrorUnexpectedKey = 0;
    palPlcData.statsErrorReset = 0;

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
        }
    } 
    else if (plcBandAux == palPlcData.plcBand) 
    {
        palPlcData.plcBranch = SRV_PLC_PCOUP_AUXILIARY_BRANCH;
        if (drvG3MacRtInitData.binStartAddress != (uint32_t)&g3_mac_rt_bin2_start)
        {
            drvG3MacRtInitData.binStartAddress = (uint32_t)&g3_mac_rt_bin2_start;
            drvG3MacRtInitData.binEndAddress = (uint32_t)&g3_mac_rt_bin2_end;
        }
    }
    else
    {
        return SYS_MODULE_OBJ_INVALID; 
    }
    
    if (DRV_G3_MACRT_Status(DRV_G3_MACRT_INDEX) != DRV_G3_MACRT_STATE_INITIALIZED)
    {
        /* Initialize PLC Driver Instance */
        (void) DRV_G3_MACRT_Initialize(DRV_G3_MACRT_INDEX, (SYS_MODULE_INIT *)&drvG3MacRtInitData);
    }
    
    DRV_G3_MACRT_InitCallbackRegister(DRV_G3_MACRT_INDEX, lPAL_PLC_InitCallback);
    
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

/* MISRA C-2012 deviation block end */

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

    if (cfmObj.updateTimestamp)
    {
        palPlcData.waitingTxCfm = true;
        DRV_G3_MACRT_TxRequest(palPlcData.drvG3MacRtHandle, pData, length);
    }
    else
    {
        palPlcData.waitingTxCfm = false;
        lPAL_PLC_DataCfmCb(&cfmObj);
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

    /* MISRA C-2012 deviation block start */
    /* MISRA C-2012 Rule 11.3 deviated twice. Deviation record ID - H3_MISRAC_2012_R_11_3_DR_1 */

    (void) PAL_PLC_Initialize(PAL_PLC_PHY_INDEX, (SYS_MODULE_INIT *)&palInit);

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
        lPAL_PLC_UpdateMibBackupInfo(pibObj->pib, (void *)pibObj->pData);
    }
            
    return result;
}

void PAL_PLC_SetCoordinator(PAL_PLC_HANDLE handle)
{
    if (handle != (PAL_PLC_HANDLE)&palPlcData)
    {
        return;
    }

    /* Enable Coordinator capabilities */
    DRV_G3_MACRT_SetCoordinator(palPlcData.drvG3MacRtHandle);
    palPlcData.mibInitData.coordinator = true;
    palPlcData.coordinator = true;
}
