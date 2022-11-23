/*******************************************************************************
  G3 MAC Wrapper Source File

  Company:
    Microchip Technology Inc.

  File Name:
    mac_wrapper.c

  Summary:
    G3 MAC Wrapper API Source File

  Description:
    This file contains implementation of the API
    to be used by upper layers when accessing G3 MAC layers.
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
#include <string.h>
#include "system/system.h"
#include "configuration.h"
#include "mac_wrapper.h"
#include "mac_wrapper_defs.h"
#include "../mac_common/mac_common.h"
#include "../mac_plc/mac_plc.h"
#include "service/log_report/srv_log_report.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Variables
// *****************************************************************************
// *****************************************************************************

// This is the module data object
static MAC_WRP_DATA macWrpData;

#define MAC_MAX_DEVICE_TABLE_ENTRIES_PLC    128

static MAC_PLC_TABLES macPlcTables;
MAC_DEVICE_TABLE_ENTRY macPlcDeviceTable[MAC_MAX_DEVICE_TABLE_ENTRIES_PLC];

static bool _macWrpIsSharedAttribute(MAC_WRP_PIB_ATTRIBUTE attribute)
{
    /* Check if attribute in the list of shared between MAC layers */
    if ((attribute == MAC_WRP_PIB_MANUF_EXTENDED_ADDRESS) ||
        (attribute == MAC_WRP_PIB_PAN_ID) ||
        (attribute == MAC_WRP_PIB_PROMISCUOUS_MODE) ||
        (attribute == MAC_WRP_PIB_SHORT_ADDRESS) ||
        (attribute == MAC_WRP_PIB_POS_TABLE_ENTRY_TTL) ||
        (attribute == MAC_WRP_PIB_RC_COORD) ||
        (attribute == MAC_WRP_PIB_KEY_TABLE))
    {
        /* Shared IB */
        return true;
    }
    else
    {
        /* Non-shared IB */
        return false;
    }
}

/* ------------------------------------------------ */
/* ---------- Callbacks from MAC Layers ----------- */
/* ------------------------------------------------ */

static void _Callback_MacPlcDataConfirm(MAC_DATA_CONFIRM_PARAMS *dcParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "_Callback_MacPlcDataConfirm() Handle: 0x%02X Status: %u", dcParams->msduHandle, (uint8_t)dcParams->status);

    MAC_WRP_DATA_CONFIRM_PARAMS dataConfirmParams;

    if (macWrpData.macWrpHandlers.dataConfirmCallback != NULL)
    {
        /* Copy dcParams from Mac and fill Media Type */
        memcpy(&dataConfirmParams, dcParams, sizeof(MAC_DATA_CONFIRM_PARAMS));
        dataConfirmParams.mediaType = MAC_WRP_MEDIA_TYPE_CONF_PLC;
        macWrpData.macWrpHandlers.dataConfirmCallback(&dataConfirmParams);
    }
}

static void _Callback_MacPlcDataIndication(MAC_DATA_INDICATION_PARAMS *diParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "_Callback_MacPlcDataIndication");

    MAC_WRP_DATA_INDICATION_PARAMS dataIndicationParams;

    if (macWrpData.macWrpHandlers.dataIndicationCallback != NULL)
    {
        /* Copy diParams from Mac and fill Media Type */
        memcpy(&dataIndicationParams, diParams, sizeof(MAC_DATA_INDICATION_PARAMS));
        dataIndicationParams.mediaType = MAC_WRP_MEDIA_TYPE_IND_PLC;
        macWrpData.macWrpHandlers.dataIndicationCallback(&dataIndicationParams);
    }
}

static void _Callback_MacPlcResetConfirm(MAC_RESET_CONFIRM_PARAMS *rcParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "_Callback_MacPlcResetConfirm: Status: %u", rcParams->status);

    if (macWrpData.macWrpHandlers.resetConfirmCallback != NULL)
    {
        macWrpData.macWrpHandlers.resetConfirmCallback((MAC_WRP_RESET_CONFIRM_PARAMS *)rcParams);
    }
}

static void _Callback_MacPlcBeaconNotify(MAC_BEACON_NOTIFY_INDICATION_PARAMS *bnParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "_Callback_MacPlcBeaconNotify: Pan ID: %04X", bnParams->panDescriptor.panId);

    MAC_WRP_BEACON_NOTIFY_INDICATION_PARAMS notifyIndicationParams;

    /* Copy bnParams from Mac. Media Type will be filled later */
    memcpy(&notifyIndicationParams, bnParams, sizeof(MAC_BEACON_NOTIFY_INDICATION_PARAMS));

    if (macWrpData.macWrpHandlers.beaconNotifyIndicationCallback != NULL)
    {
        notifyIndicationParams.panDescriptor.mediaType = MAC_WRP_MEDIA_TYPE_IND_PLC;
        macWrpData.macWrpHandlers.beaconNotifyIndicationCallback(&notifyIndicationParams);
    }
}

static void _Callback_MacPlcScanConfirm(MAC_SCAN_CONFIRM_PARAMS *scParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "_Callback_MacPlcScanConfirm: Status: %u", scParams->status);

    if (macWrpData.macWrpHandlers.scanConfirmCallback != NULL)
    {
        macWrpData.macWrpHandlers.scanConfirmCallback((MAC_WRP_SCAN_CONFIRM_PARAMS *)scParams);
    }
}

static void _Callback_MacPlcStartConfirm(MAC_START_CONFIRM_PARAMS *scParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "_Callback_MacPlcStartConfirm: Status: %u", scParams->status);

    if (macWrpData.macWrpHandlers.startConfirmCallback != NULL)
    {
        macWrpData.macWrpHandlers.startConfirmCallback((MAC_WRP_START_CONFIRM_PARAMS *)scParams);
    }
}

static void _Callback_MacPlcCommStatusIndication(MAC_COMM_STATUS_INDICATION_PARAMS *csParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "_Callback_MacPlcCommStatusIndication: Status: %u", csParams->status);

    MAC_WRP_COMM_STATUS_INDICATION_PARAMS commStatusIndicationParams;

    /* Copy csParams from Mac. Media Type will be filled later */
    memcpy(&commStatusIndicationParams, csParams, sizeof(MAC_COMM_STATUS_INDICATION_PARAMS));

    if (macWrpData.macWrpHandlers.commStatusIndicationCallback != NULL)
    {
        commStatusIndicationParams.mediaType = MAC_WRP_MEDIA_TYPE_IND_PLC;
        macWrpData.macWrpHandlers.commStatusIndicationCallback(&commStatusIndicationParams);
    }
}

static void _Callback_MacPlcMacSnifferIndication(MAC_SNIFFER_INDICATION_PARAMS *siParams)
{
    SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, siParams->msdu, siParams->msduLength, "_Callback_MacPlcMacSnifferIndication:  MSDU:");

    if (macWrpData.macWrpHandlers.snifferIndicationCallback != NULL)
    {
        macWrpData.macWrpHandlers.snifferIndicationCallback((MAC_WRP_SNIFFER_INDICATION_PARAMS *)siParams);
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Interface Function Definitions
// *****************************************************************************
// *****************************************************************************

SYS_MODULE_OBJ MAC_WRP_Initialize(const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init)
{
    bool initError = false;

    /* Validate the request */
    if (index >= G3_MAC_WRP_INSTANCES_NUMBER)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    if (macWrpData.inUse == true)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    macWrpData.inUse = true;
    macWrpData.state = MAC_WRP_STATE_NOT_READY;

    if (initError)
    {
        return SYS_MODULE_OBJ_INVALID;
    }
    else
    {
        return (SYS_MODULE_OBJ)0; 
    }
}

MAC_WRP_HANDLE MAC_WRP_Open(SYS_MODULE_INDEX index)
{
    // Single instance allowed
    if (index >= G3_MAC_WRP_INSTANCES_NUMBER)
    {
        return MAC_WRP_HANDLE_INVALID;
    }
    else
    {
        macWrpData.state = MAC_WRP_STATE_IDLE;
        macWrpData.macWrpHandle = (MAC_WRP_HANDLE)0;
        return macWrpData.macWrpHandle;
    }
}

void MAC_WRP_Tasks(SYS_MODULE_OBJ object)
{
    if (object != (SYS_MODULE_OBJ)0)
    {
        // Invalid object
        return;
    }
    MAC_PLC_Tasks();
}

void MAC_WRP_Init(MAC_WRP_HANDLE handle, MAC_WRP_INIT *init)
{
    MAC_PLC_INIT plcInitData;

    /* Validate the request */
    if (handle != macWrpData.macWrpHandle)
    {
        return;
    }

    /* Set init data */
    macWrpData.macWrpHandlers = init->macWrpHandlers;
    macWrpData.plcBand = init->plcBand;

    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "MAC_WRP_Init: Initializing PLC MAC...");

    plcInitData.macPlcHandlers.macPlcDataConfirm = _Callback_MacPlcDataConfirm;
    plcInitData.macPlcHandlers.macPlcDataIndication = _Callback_MacPlcDataIndication;
    plcInitData.macPlcHandlers.macPlcResetConfirm = _Callback_MacPlcResetConfirm;
    plcInitData.macPlcHandlers.macPlcBeaconNotifyIndication = _Callback_MacPlcBeaconNotify;
    plcInitData.macPlcHandlers.macPlcScanConfirm = _Callback_MacPlcScanConfirm;
    plcInitData.macPlcHandlers.macPlcStartConfirm = _Callback_MacPlcStartConfirm;
    plcInitData.macPlcHandlers.macPlcCommStatusIndication = _Callback_MacPlcCommStatusIndication;
    plcInitData.macPlcHandlers.macPlcMacSnifferIndication = _Callback_MacPlcMacSnifferIndication;

    memset(macPlcDeviceTable, 0, sizeof(macPlcDeviceTable));

    macPlcTables.macPlcDeviceTableSize = MAC_MAX_DEVICE_TABLE_ENTRIES_PLC;
    macPlcTables.macPlcDeviceTable = macPlcDeviceTable;

    plcInitData.macPlcTables = &macPlcTables;
    plcInitData.plcBand = (MAC_PLC_BAND)init->plcBand;

    MAC_PLC_Init(&plcInitData);

    MAC_COMMON_Init();
}

void MAC_WRP_DataRequest(MAC_WRP_HANDLE handle, MAC_WRP_DATA_REQUEST_PARAMS *drParams)
{
    MAC_WRP_DATA_CONFIRM_PARAMS dataConfirm;

    if (handle != macWrpData.macWrpHandle)
    {
        /* Handle error */
        /* Send confirm to upper layer and return */
        if (macWrpData.macWrpHandlers.dataConfirmCallback != NULL)
        {
            dataConfirm.msduHandle = drParams->msduHandle;
            dataConfirm.status = MAC_WRP_STATUS_INVALID_HANDLE;
            dataConfirm.timestamp = 0;
            dataConfirm.mediaType = (MAC_WRP_MEDIA_TYPE_CONFIRM)drParams->mediaType;
            macWrpData.macWrpHandlers.dataConfirmCallback(&dataConfirm);
        }

        return;
    }

    MAC_DATA_REQUEST_PARAMS dataReq;
    SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_INFO, drParams->msdu, drParams->msduLength, "MAC_WRP_DataRequest (Handle %02X): ", drParams->msduHandle);
    // Copy data to Mac struct (media type is not copied as it is the last field of pParameters)
    memcpy(&dataReq, drParams, sizeof(dataReq));
    MAC_PLC_DataRequest(&dataReq);
}

MAC_WRP_STATUS MAC_WRP_GetRequestSync(MAC_WRP_HANDLE handle, MAC_WRP_PIB_ATTRIBUTE attribute, uint16_t index, MAC_WRP_PIB_VALUE *pibValue)
{
    if (handle != macWrpData.macWrpHandle)
    {
        // Handle error
        pibValue->length = 0;
        return MAC_WRP_STATUS_INVALID_HANDLE;
    }

    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "MAC_WRP_GetRequestSync: Attribute: %08X; Index: %u", attribute, index);

    /* Check attribute ID range to redirect to Common or PLC MAC */
    if (_macWrpIsSharedAttribute(attribute))
    {
        /* Get from MAC Common */
        return (MAC_WRP_STATUS)(MAC_COMMON_GetRequestSync((MAC_COMMON_PIB_ATTRIBUTE)attribute, index, (MAC_PIB_VALUE *)pibValue));
    }
    else
    {
        /* RF Available IB has to be handled here */
        if (attribute == MAC_WRP_PIB_MANUF_RF_IFACE_AVAILABLE) 
        {
            pibValue->length = 1;
            pibValue->value[0] = 0;
            return MAC_WRP_STATUS_SUCCESS;
        }
        else 
        {
            /* Get from PLC MAC */
            return (MAC_WRP_STATUS)(MAC_PLC_GetRequestSync((MAC_PLC_PIB_ATTRIBUTE)attribute, index, (MAC_PIB_VALUE *)pibValue));
        }
    }
}

MAC_WRP_STATUS MAC_WRP_SetRequestSync(MAC_WRP_HANDLE handle, MAC_WRP_PIB_ATTRIBUTE attribute, uint16_t index, const MAC_WRP_PIB_VALUE *pibValue)
{
    if (handle != macWrpData.macWrpHandle)
    {
        // Handle error
        return MAC_WRP_STATUS_INVALID_HANDLE;
    }

    SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, pibValue->value, pibValue->length, "MAC_WRP_SetRequestSync: Attribute: %08X; Index: %u; Value: ", attribute, index);

    /* Check attribute ID range to redirect to Common or PLC MAC */
    if (_macWrpIsSharedAttribute(attribute))
    {
        /* Set to MAC Common */
        return (MAC_WRP_STATUS)(MAC_COMMON_SetRequestSync((MAC_COMMON_PIB_ATTRIBUTE)attribute, index, (const MAC_PIB_VALUE *)pibValue));
    }
    else
    {
        /* Set to PLC MAC */
        return (MAC_WRP_STATUS)(MAC_PLC_SetRequestSync((MAC_PLC_PIB_ATTRIBUTE)attribute, index, (const MAC_PIB_VALUE *)pibValue));
    }
}

void MAC_WRP_ResetRequest(MAC_WRP_HANDLE handle, MAC_WRP_RESET_REQUEST_PARAMS *rstParams)
{
    if (handle != macWrpData.macWrpHandle)
    {
        /* Handle error */
        /* Send confirm to upper layer and return */
        MAC_WRP_RESET_CONFIRM_PARAMS resetConfirm;
        if (macWrpData.macWrpHandlers.resetConfirmCallback != NULL)
        {
            resetConfirm.status = MAC_WRP_STATUS_INVALID_HANDLE;
            macWrpData.macWrpHandlers.resetConfirmCallback(&resetConfirm);
        }

        return;
    }

    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "MAC_WRP_ResetRequest: Set default PIB: %u", rstParams->setDefaultPib);

    // Reset PLC MAC
    MAC_PLC_ResetRequest((MAC_RESET_REQUEST_PARAMS *)rstParams);
    // Reset Common MAC if IB has to be reset
    if (rstParams->setDefaultPib)
    {
        MAC_COMMON_Reset();
    }
}

void MAC_WRP_ScanRequest(MAC_WRP_HANDLE handle, MAC_WRP_SCAN_REQUEST_PARAMS *scanParams)
{
    if (handle != macWrpData.macWrpHandle)
    {
        /* Handle error */
        /* Send confirm to upper layer and return */
        MAC_WRP_SCAN_CONFIRM_PARAMS scanConfirm;
        if (macWrpData.macWrpHandlers.scanConfirmCallback != NULL)
        {
            scanConfirm.status = MAC_WRP_STATUS_INVALID_HANDLE;
            macWrpData.macWrpHandlers.scanConfirmCallback(&scanConfirm);
        }

        return;
    }

    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "MAC_WRP_ScanRequest: Duration: %u", scanParams->scanDuration);

    // Set PLC MAC on Scan state
    MAC_PLC_ScanRequest((MAC_SCAN_REQUEST_PARAMS *)scanParams);
}

void MAC_WRP_StartRequest(MAC_WRP_HANDLE handle, MAC_WRP_START_REQUEST_PARAMS *startParams)
{
    if (handle != macWrpData.macWrpHandle)
    {
        /* Handle error */
        /* Send confirm to upper layer and return */
        MAC_WRP_START_CONFIRM_PARAMS startConfirm;
        if (macWrpData.macWrpHandlers.startConfirmCallback != NULL)
        {
            startConfirm.status = MAC_WRP_STATUS_INVALID_HANDLE;
            macWrpData.macWrpHandlers.startConfirmCallback(&startConfirm);
        }

        return;
    }

    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "MAC_WRP_StartRequest: Pan ID: %u", startParams->panId);

    // Start Network on PLC MAC
    MAC_PLC_StartRequest((MAC_START_REQUEST_PARAMS *)startParams);
}

MAC_WRP_AVAILABLE_MAC_LAYERS MAC_WRP_GetAvailableMacLayers(MAC_WRP_HANDLE handle)
{
    return MAC_WRP_AVAILABLE_MAC_PLC;
}

/*******************************************************************************
 End of File
*/
