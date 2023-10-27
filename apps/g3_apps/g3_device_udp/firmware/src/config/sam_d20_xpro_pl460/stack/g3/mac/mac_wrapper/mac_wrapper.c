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

typedef struct
{
    /* State of the MAC Wrapper module */
    MAC_WRP_STATE state;
    /* Callbacks */
    MAC_WRP_HANDLERS macWrpHandlers;
    /* Mac Wrapper instance handle */
    MAC_WRP_HANDLE macWrpHandle;
    /* Time of next task in milliseconds */
    uint32_t nextTaskTimeMs;
    /* Flag to indicate scan request in progress */
    bool scanRequestInProgress;
} MAC_WRP_DATA;

typedef struct
{
    MAC_DATA_REQUEST_PARAMS dataReqParams;
    MAC_WRP_MEDIA_TYPE_REQUEST dataReqMediaType;
    bool used;
} MAC_WRP_DATA_REQ_ENTRY;

/* Data Request Queue size */
#define MAC_WRP_DATA_REQ_QUEUE_SIZE   2U

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Variables
// *****************************************************************************
// *****************************************************************************

// This is the module data object
static MAC_WRP_DATA macWrpData;

// Data Service Control
static MAC_WRP_DATA_REQ_ENTRY dataReqQueue[MAC_WRP_DATA_REQ_QUEUE_SIZE];

#define MAC_MAX_DEVICE_TABLE_ENTRIES_PLC    64

static MAC_PLC_TABLES macPlcTables;
static MAC_DEVICE_TABLE_ENTRY macPlcDeviceTable[MAC_MAX_DEVICE_TABLE_ENTRIES_PLC];

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Functions
// *****************************************************************************
// *****************************************************************************

static MAC_WRP_DATA_REQ_ENTRY *lMAC_WRP_GetFreeDataReqEntry(void)
{
    uint8_t index;
    MAC_WRP_DATA_REQ_ENTRY *found = NULL;

    for (index = 0U; index < MAC_WRP_DATA_REQ_QUEUE_SIZE; index++)
    {
        if (dataReqQueue[index].used == false)
        {
            found = &dataReqQueue[index];
            dataReqQueue[index].used = true;
            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "lMAC_WRP_GetFreeDataReqEntry() Found free data request entry on index %u\r\n", index);
            break;
        }
    }

    return found;
}

static MAC_WRP_DATA_REQ_ENTRY *lMAC_WRP_GetDataReqEntryByHandle(uint8_t handle)
{
    uint8_t index;
    MAC_WRP_DATA_REQ_ENTRY *found = NULL;

    for (index = 0U; index < MAC_WRP_DATA_REQ_QUEUE_SIZE; index++)
    {
        if ((dataReqQueue[index].used == true) &&
            (dataReqQueue[index].dataReqParams.msduHandle == handle))
        {
            found = &dataReqQueue[index];
            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "lMAC_WRP_GetDataReqEntryByHandle() Found matching data request entry on index %u, Handle: 0x%02X\r\n", index, handle);
            break;
        }
    }

    return found;
}

static bool lMAC_WRP_IsSharedAttribute(MAC_WRP_PIB_ATTRIBUTE attribute)
{
    /* Check if attribute in the list of shared between MAC layers */
    if ((attribute == MAC_WRP_PIB_MANUF_EXTENDED_ADDRESS) ||
        (attribute == MAC_WRP_PIB_PAN_ID) ||
        (attribute == MAC_WRP_PIB_PROMISCUOUS_MODE) ||
        (attribute == MAC_WRP_PIB_SHORT_ADDRESS) ||
        (attribute == MAC_WRP_PIB_POS_TABLE_ENTRY_TTL) ||
        (attribute == MAC_WRP_PIB_POS_RECENT_ENTRY_THRESHOLD) ||
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

// *****************************************************************************
// *****************************************************************************
// Section: Callbacks from MAC Layers
// *****************************************************************************
// *****************************************************************************

static void lMAC_WRP_CallbackMacPlcDataConfirm(MAC_DATA_CONFIRM_PARAMS *dcParams)
{
    MAC_WRP_DATA_CONFIRM_PARAMS dataConfirmParams;
    MAC_WRP_DATA_REQ_ENTRY *matchingDataReq;

    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "lMAC_WRP_CallbackMacPlcDataConfirm() Handle: 0x%02X Status: %u\r\n", dcParams->msduHandle, (uint8_t)dcParams->status);

    /* Get Data Request entry matching confirm */
    matchingDataReq = lMAC_WRP_GetDataReqEntryByHandle(dcParams->msduHandle);

    /* Avoid unmached handling */
    if (matchingDataReq == NULL)
    {
        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_ERROR, "lMAC_WRP_CallbackMacPlcDataConfirm() Confirm does not match any previous request!!\r\n");
        return;
    }

    /* Copy dcParams from Mac */
    (void) memcpy((void *) &dataConfirmParams, (void *) dcParams, sizeof(MAC_DATA_CONFIRM_PARAMS));

    /* Fill Media Type */
    dataConfirmParams.mediaType = MAC_WRP_MEDIA_TYPE_CONF_PLC;

    /* Release Data Req entry and send confirm to upper layer */
    matchingDataReq->used = false;
    if (macWrpData.macWrpHandlers.dataConfirmCallback != NULL)
    {
        macWrpData.macWrpHandlers.dataConfirmCallback(&dataConfirmParams);
    }
}

static void lMAC_WRP_CallbackMacPlcDataIndication(MAC_DATA_INDICATION_PARAMS *diParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "lMAC_WRP_CallbackMacPlcDataIndication");

    MAC_WRP_DATA_INDICATION_PARAMS dataIndicationParams;

     /* Copy diParams from Mac and fill Media Type */
    (void) memcpy((void *) &dataIndicationParams, (void *) diParams, sizeof(MAC_DATA_INDICATION_PARAMS));
    dataIndicationParams.mediaType = MAC_WRP_MEDIA_TYPE_IND_PLC;
    if (macWrpData.macWrpHandlers.dataIndicationCallback != NULL)
    {
        macWrpData.macWrpHandlers.dataIndicationCallback(&dataIndicationParams);
    }
}

static void lMAC_WRP_CallbackMacPlcResetConfirm(MAC_RESET_CONFIRM_PARAMS *rcParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "lMAC_WRP_CallbackMacPlcResetConfirm: Status: %u\r\n", rcParams->status);

    if (macWrpData.macWrpHandlers.resetConfirmCallback != NULL)
    {
        macWrpData.macWrpHandlers.resetConfirmCallback((void *)rcParams);
    }
}

static void lMAC_WRP_CallbackMacPlcBeaconNotify(MAC_BEACON_NOTIFY_INDICATION_PARAMS *bnParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "lMAC_WRP_CallbackMacPlcBeaconNotify: Pan ID: %04X\r\n", bnParams->panDescriptor.panId);

    MAC_WRP_BEACON_NOTIFY_INDICATION_PARAMS notifyIndicationParams;

    /* Copy bnParams from Mac and fill Media Type */
    (void) memcpy((void *) &notifyIndicationParams, (void *) bnParams, sizeof(MAC_BEACON_NOTIFY_INDICATION_PARAMS));
    notifyIndicationParams.panDescriptor.mediaType = MAC_WRP_MEDIA_TYPE_IND_PLC;

    if (macWrpData.macWrpHandlers.beaconNotifyIndicationCallback != NULL)
    {
        macWrpData.macWrpHandlers.beaconNotifyIndicationCallback(&notifyIndicationParams);
    }
}

static void lMAC_WRP_CallbackMacPlcScanConfirm(MAC_SCAN_CONFIRM_PARAMS *scParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "lMAC_WRP_CallbackMacPlcScanConfirm: Status: %u\r\n", scParams->status);

    /* Clear flag */
    macWrpData.scanRequestInProgress = false;

    /* Send confirm to upper layer */
    if (macWrpData.macWrpHandlers.scanConfirmCallback != NULL)
    {
        macWrpData.macWrpHandlers.scanConfirmCallback((void *)scParams);
    }
}

static void lMAC_WRP_CallbackMacPlcStartConfirm(MAC_START_CONFIRM_PARAMS *scParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "lMAC_WRP_CallbackMacPlcStartConfirm: Status: %u\r\n", scParams->status);

    if (macWrpData.macWrpHandlers.startConfirmCallback != NULL)
    {
        macWrpData.macWrpHandlers.startConfirmCallback((void *)scParams);
    }
}

static void lMAC_WRP_CallbackMacPlcCommStatusIndication(MAC_COMM_STATUS_INDICATION_PARAMS *csParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "lMAC_WRP_CallbackMacPlcCommStatusIndication: Status: %u\r\n", csParams->status);

    MAC_WRP_COMM_STATUS_INDICATION_PARAMS commStatusIndicationParams;

    /* Copy csParams from Mac and fill Media Type */
    (void) memcpy((void *) &commStatusIndicationParams, (void *) csParams, sizeof(MAC_COMM_STATUS_INDICATION_PARAMS));
    commStatusIndicationParams.mediaType = MAC_WRP_MEDIA_TYPE_IND_PLC;

    if (macWrpData.macWrpHandlers.commStatusIndicationCallback != NULL)
    {
        macWrpData.macWrpHandlers.commStatusIndicationCallback(&commStatusIndicationParams);
    }
}

static void lMAC_WRP_CallbackMacPlcMacSnifferIndication(MAC_SNIFFER_INDICATION_PARAMS *siParams)
{
    SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, siParams->msdu, siParams->msduLength, "lMAC_WRP_CallbackMacPlcMacSnifferIndication:  MSDU:");

    if (macWrpData.macWrpHandlers.snifferIndicationCallback != NULL)
    {
        macWrpData.macWrpHandlers.snifferIndicationCallback((void *)siParams);
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: MAC Wrapper Interface Routines
// *****************************************************************************
// *****************************************************************************

SYS_MODULE_OBJ MAC_WRP_Initialize(const SYS_MODULE_INDEX index)
{
    /* Validate the request */
    if (index >= G3_MAC_WRP_INSTANCES_NUMBER)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    macWrpData.state = MAC_WRP_STATE_NOT_READY;
    macWrpData.macWrpHandle = (MAC_WRP_HANDLE) 0U;
    macWrpData.scanRequestInProgress = false;
    (void) memset(&macWrpData.macWrpHandlers, 0, sizeof(MAC_WRP_HANDLERS));
    for (uint8_t i = 0U; i < MAC_WRP_DATA_REQ_QUEUE_SIZE; i++)
    {
        dataReqQueue[i].used = false;
    }

    return (SYS_MODULE_OBJ)0;
}

MAC_WRP_HANDLE MAC_WRP_Open(SYS_MODULE_INDEX index, MAC_WRP_BAND plcBand)
{
    MAC_PLC_INIT plcInitData;

    /* Single instance allowed */
    if (index >= G3_MAC_WRP_INSTANCES_NUMBER)
    {
        return MAC_WRP_HANDLE_INVALID;
    }

    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "MAC_WRP_Open: Initializing PLC MAC...\r\n");

    plcInitData.macPlcHandlers.macPlcDataConfirm = lMAC_WRP_CallbackMacPlcDataConfirm;
    plcInitData.macPlcHandlers.macPlcDataIndication = lMAC_WRP_CallbackMacPlcDataIndication;
    plcInitData.macPlcHandlers.macPlcResetConfirm = lMAC_WRP_CallbackMacPlcResetConfirm;
    plcInitData.macPlcHandlers.macPlcBeaconNotifyIndication = lMAC_WRP_CallbackMacPlcBeaconNotify;
    plcInitData.macPlcHandlers.macPlcScanConfirm = lMAC_WRP_CallbackMacPlcScanConfirm;
    plcInitData.macPlcHandlers.macPlcStartConfirm = lMAC_WRP_CallbackMacPlcStartConfirm;
    plcInitData.macPlcHandlers.macPlcCommStatusIndication = lMAC_WRP_CallbackMacPlcCommStatusIndication;
    plcInitData.macPlcHandlers.macPlcMacSnifferIndication = lMAC_WRP_CallbackMacPlcMacSnifferIndication;

    (void) memset(macPlcDeviceTable, 0, sizeof(macPlcDeviceTable));

    macPlcTables.macPlcDeviceTableSize = MAC_MAX_DEVICE_TABLE_ENTRIES_PLC;
    macPlcTables.macPlcDeviceTable = macPlcDeviceTable;

    plcInitData.macPlcTables = &macPlcTables;
    plcInitData.plcBand = (MAC_PLC_BAND) plcBand;
    /* Get PAL index from configuration header */
    plcInitData.palPlcIndex = PAL_PLC_PHY_INDEX;

    MAC_PLC_Init(&plcInitData);

    MAC_COMMON_Init();

    macWrpData.state = MAC_WRP_STATE_IDLE;

    macWrpData.nextTaskTimeMs = MAC_WRP_GetMsCounter() + G3_STACK_TASK_RATE_MS;

    return macWrpData.macWrpHandle;
}

void MAC_WRP_SetCallbacks(MAC_WRP_HANDLE handle, MAC_WRP_HANDLERS* handlers)
{
    if ((handle == macWrpData.macWrpHandle) && (handlers != NULL))
    {
        macWrpData.macWrpHandlers = *handlers;
    }
}

void MAC_WRP_Tasks(SYS_MODULE_OBJ object)
{
    if (object != (SYS_MODULE_OBJ) 0)
    {
        /* Invalid object */
        return;
    }

    if (!MAC_COMMON_TimeIsPast((int32_t) macWrpData.nextTaskTimeMs))
    {
        /* Do nothing */
        return;
    }

    /* Time to execute MAC Wrapper task. Update time for next task. */
    macWrpData.nextTaskTimeMs += G3_STACK_TASK_RATE_MS;

    MAC_PLC_Tasks();
    MAC_COMMON_GetMsCounter(); /* Just to avoid counter overflow */
}

SYS_STATUS MAC_WRP_Status(void)
{
    return MAC_PLC_Status();
}

void MAC_WRP_DataRequest(MAC_WRP_HANDLE handle, MAC_WRP_DATA_REQUEST_PARAMS *drParams)
{
    MAC_WRP_DATA_CONFIRM_PARAMS dataConfirm;
    MAC_WRP_DATA_REQ_ENTRY *dataReqEntry;

    if (handle != macWrpData.macWrpHandle)
    {
        /* Handle error */
        /* Send confirm to upper layer and return */
        if (macWrpData.macWrpHandlers.dataConfirmCallback != NULL)
        {
            dataConfirm.msduHandle = drParams->msduHandle;
            dataConfirm.status = MAC_WRP_STATUS_INVALID_HANDLE;
            dataConfirm.timestamp = 0;
            dataConfirm.mediaType = (MAC_WRP_MEDIA_TYPE_CONFIRM) drParams->mediaType;
            macWrpData.macWrpHandlers.dataConfirmCallback(&dataConfirm);
        }

        return;
    }

    SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_INFO, drParams->msdu, drParams->msduLength, "MAC_WRP_DataRequest (Handle: 0x%02X Media Type: %02X): ", drParams->msduHandle, drParams->mediaType);

    /* Look for free Data Request Entry */
    dataReqEntry = lMAC_WRP_GetFreeDataReqEntry();

    if (dataReqEntry == NULL)
    {
        /* Too many data requests */
        /* Send confirm to upper layer and return */
        dataConfirm.msduHandle = drParams->msduHandle;
        dataConfirm.status = MAC_WRP_STATUS_QUEUE_FULL;
        dataConfirm.timestamp = 0;
        dataConfirm.mediaType = (MAC_WRP_MEDIA_TYPE_CONFIRM)drParams->mediaType;
        if (macWrpData.macWrpHandlers.dataConfirmCallback != NULL)
        {
            macWrpData.macWrpHandlers.dataConfirmCallback(&dataConfirm);
        }

        return;
    }

    /* Accept request */
    /* Copy data to Mac struct (media type is not copied as it is the last field of drParams) */
    (void) memcpy((void *) &dataReqEntry->dataReqParams, (void *) drParams, sizeof(dataReqEntry->dataReqParams));
    MAC_PLC_DataRequest(&dataReqEntry->dataReqParams);
}

MAC_WRP_STATUS MAC_WRP_GetRequestSync(MAC_WRP_HANDLE handle, MAC_WRP_PIB_ATTRIBUTE attribute, uint16_t index, MAC_WRP_PIB_VALUE *pibValue)
{
    if (handle != macWrpData.macWrpHandle)
    {
        // Handle error
        pibValue->length = 0U;
        return MAC_WRP_STATUS_INVALID_HANDLE;
    }

    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "MAC_WRP_GetRequestSync: Attribute: %08X; Index: %u\r\n", attribute, index);

    /* Check attribute ID range to redirect to Common or PLC MAC */
    if (lMAC_WRP_IsSharedAttribute(attribute))
    {
        /* Get from MAC Common */
        return (MAC_WRP_STATUS)(MAC_COMMON_GetRequestSync((MAC_COMMON_PIB_ATTRIBUTE)attribute, index, (void *)pibValue));
    }
    else
    {
        /* RF Available IB has to be handled here */
        if (attribute == MAC_WRP_PIB_MANUF_RF_IFACE_AVAILABLE)
        {
            pibValue->length = 1U;
            pibValue->value[0] = 0U;
            return MAC_WRP_STATUS_SUCCESS;
        }
        else
        {
            /* Get from PLC MAC */
            return (MAC_WRP_STATUS)(MAC_PLC_GetRequestSync((MAC_PLC_PIB_ATTRIBUTE)attribute, index, (void *)pibValue));
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
    if (lMAC_WRP_IsSharedAttribute(attribute))
    {
        /* Set to MAC Common */
        return (MAC_WRP_STATUS)(MAC_COMMON_SetRequestSync((MAC_COMMON_PIB_ATTRIBUTE)attribute, index, (const void *)pibValue));
    }
    else
    {
        /* Set to PLC MAC */
        return (MAC_WRP_STATUS)(MAC_PLC_SetRequestSync((MAC_PLC_PIB_ATTRIBUTE)attribute, index, (const void *)pibValue));
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

    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "MAC_WRP_ResetRequest: Set default PIB: %u\r\n", rstParams->setDefaultPib);

    // Reset PLC MAC
    MAC_PLC_ResetRequest((void *)rstParams);
    // Reset Common MAC if IB has to be reset
    if (rstParams->setDefaultPib)
    {
        MAC_COMMON_Reset();
    }
}

void MAC_WRP_ScanRequest(MAC_WRP_HANDLE handle, MAC_WRP_SCAN_REQUEST_PARAMS *scanParams)
{
    MAC_WRP_SCAN_CONFIRM_PARAMS scanConfirm;

    if (handle != macWrpData.macWrpHandle)
    {
        /* Handle error */
        /* Send confirm to upper layer and return */
        if (macWrpData.macWrpHandlers.scanConfirmCallback != NULL)
        {
            scanConfirm.status = MAC_WRP_STATUS_INVALID_HANDLE;
            macWrpData.macWrpHandlers.scanConfirmCallback(&scanConfirm);
        }

        return;
    }

    if (macWrpData.scanRequestInProgress == true)
    {
        /* Scan request already in progress */
        /* Send confirm to upper layer and return */
        scanConfirm.status = MAC_WRP_STATUS_DENIED;
        if (macWrpData.macWrpHandlers.scanConfirmCallback != NULL)
        {
            macWrpData.macWrpHandlers.scanConfirmCallback(&scanConfirm);
        }

        return;
    }

    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "MAC_WRP_ScanRequest: Duration: %u\r\n", scanParams->scanDuration);

    // Set control variable
    macWrpData.scanRequestInProgress = true;
    // Set PLC MAC on Scan state
    MAC_PLC_ScanRequest((void *)scanParams);
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

    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "MAC_WRP_StartRequest: Pan ID: %u\r\n", startParams->panId);

    // Start Network on PLC MAC
    MAC_PLC_StartRequest((void *)startParams);
}

MAC_WRP_AVAILABLE_MAC_LAYERS MAC_WRP_GetAvailableMacLayers(MAC_WRP_HANDLE handle)
{
    return MAC_WRP_AVAILABLE_MAC_PLC;
}

uint32_t MAC_WRP_GetMsCounter(void)
{
    /* Call lower layer function */
    return MAC_COMMON_GetMsCounter();
}

bool MAC_WRP_TimeIsPast(int32_t timeValue)
{
    /* Call lower layer function */
    return MAC_COMMON_TimeIsPast(timeValue);
}

uint32_t MAC_WRP_GetSecondsCounter(void)
{
    /* Call lower layer function */
    return MAC_COMMON_GetSecondsCounter();
}

bool MAC_WRP_TimeIsPastSeconds(int32_t timeValue)
{
    /* Call lower layer function */
    return MAC_COMMON_TimeIsPastSeconds(timeValue);
}

/*******************************************************************************
 End of File
*/
