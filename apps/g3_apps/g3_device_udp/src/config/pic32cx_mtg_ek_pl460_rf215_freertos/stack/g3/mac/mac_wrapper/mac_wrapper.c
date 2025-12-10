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
/*
Copyright (C) 2024, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
#include "../mac_rf/mac_rf.h"
#include "service/log_report/srv_log_report.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

#pragma pack(push,2)

typedef struct
{
    /* State of the MAC Wrapper module */
    MAC_WRP_STATE state;
    /* Callbacks */
    MAC_WRP_HANDLERS macWrpHandlers;
    /* Mac Wrapper instance handle */
    MAC_WRP_HANDLE macWrpHandle;
    /* Flag to indicate scan request in progress */
    bool scanRequestInProgress;
} MAC_WRP_DATA;

/* Buffer size to store data to be sent as Mac Data Request */
#define HYAL_BACKUP_BUF_SIZE   400U

typedef struct
{
    MAC_DATA_REQUEST_PARAMS dataReqParams;
    MAC_WRP_MEDIA_TYPE_REQUEST dataReqMediaType;
    uint8_t backupBuffer[HYAL_BACKUP_BUF_SIZE];
    MAC_STATUS firstConfirmStatus;
    bool waitingSecondConfirm;
    uint8_t probingInterval;
    bool used;
} MAC_WRP_DATA_REQ_ENTRY;

/* Data Request Queue size */
#define MAC_WRP_DATA_REQ_QUEUE_SIZE   2U

typedef struct
{
    uint16_t srcAddress;
    uint16_t msduLen;
    uint16_t crc;
    MAC_WRP_MEDIA_TYPE_INDICATION mediaType;
} HYAL_DUPLICATES_ENTRY;

typedef struct
{
    MAC_STATUS firstScanConfirmStatus;
    bool waitingSecondScanConfirm;
    MAC_STATUS firstResetConfirmStatus;
    bool waitingSecondResetConfirm;
    MAC_STATUS firstStartConfirmStatus;
    bool waitingSecondStartConfirm;
    bool mediaProbing;
} HYAL_DATA;


#pragma pack(pop)

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Variables
// *****************************************************************************
// *****************************************************************************

// This is the module data object
static MAC_WRP_DATA macWrpData;

// Data Service Control
static MAC_WRP_DATA_REQ_ENTRY dataReqQueue[MAC_WRP_DATA_REQ_QUEUE_SIZE];

#define MAC_MAX_DEVICE_TABLE_ENTRIES_PLC    128

static MAC_PLC_TABLES macPlcTables;
static MAC_DEVICE_TABLE_ENTRY macPlcDeviceTable[MAC_MAX_DEVICE_TABLE_ENTRIES_PLC];

#define MAC_MAX_POS_TABLE_ENTRIES_RF        100
#define MAC_MAX_DSN_TABLE_ENTRIES_RF        8
#define MAC_MAX_DEVICE_TABLE_ENTRIES_RF     128

static MAC_RF_TABLES macRfTables;
static MAC_RF_POS_TABLE_ENTRY macRfPOSTable[MAC_MAX_POS_TABLE_ENTRIES_RF];
static MAC_DEVICE_TABLE_ENTRY macRfDeviceTable[MAC_MAX_DEVICE_TABLE_ENTRIES_RF];
static MAC_RF_DSN_TABLE_ENTRY macRfDsnTable[MAC_MAX_DSN_TABLE_ENTRIES_RF];

static const uint16_t crc16_tab[256] = {
  0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
  0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
  0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
  0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
  0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
  0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
  0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
  0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
  0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
  0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
  0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
  0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
  0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
  0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
  0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
  0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
  0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
  0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
  0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
  0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
  0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
  0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
  0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
  0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
  0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
  0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
  0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
  0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
  0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
  0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
  0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
  0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0,
};

#define HYAL_DUPLICATES_TABLE_SIZE   3U

static HYAL_DUPLICATES_ENTRY hyALDuplicatesTable[HYAL_DUPLICATES_TABLE_SIZE] = {{0}};

static const HYAL_DATA hyalDataDefaults = {
  MAC_STATUS_SUCCESS, // firstScanConfirmStatus
  false, // waitingSecondScanConfirm
  MAC_STATUS_SUCCESS, // firstResetConfirmStatus
  false, // waitingSecondResetConfirm
  MAC_STATUS_SUCCESS, // firstStartConfirmStatus
  false, // waitingSecondStartConfirm
  false, // mediaProbing
};

static HYAL_DATA hyalData;

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

static uint16_t lMAC_WRP_HyalCrc16(const uint8_t *dataBuf, uint32_t length)
{
    uint16_t crc = 0;

    // polynom(16): X16 + X12 + X5 + 1 = 0x1021
    while ((length--) > 0U)
    {
        crc = crc16_tab[(uint8_t)(crc >> 8) ^ (*dataBuf ++)] ^ (crc << 8);
    }
    return crc;
}

static bool lMAC_WRP_HyalCheckDuplicates(uint16_t srcAddr, uint8_t *msdu, uint16_t msduLen, MAC_WRP_MEDIA_TYPE_INDICATION mediaType)
{
    bool duplicate = false;
    uint8_t index = 0U;
    uint16_t crc;

    // Calculate CRC for incoming frame
    crc = lMAC_WRP_HyalCrc16(msdu, msduLen);

    // Look for entry in the Duplicates Table
    HYAL_DUPLICATES_ENTRY *entry = &hyALDuplicatesTable[0];
    while (index < HYAL_DUPLICATES_TABLE_SIZE)
    {
        // Look for same fields and different MediaType
        if ((entry->srcAddress == srcAddr) && (entry->msduLen == msduLen) &&
            (entry->crc == crc) && (entry->mediaType != mediaType))
        {
            duplicate = true;
            break;
        }
        index ++;
        entry ++;
    }

    if (!duplicate)
    {
        // Entry not found, store it
        (void) memmove(&hyALDuplicatesTable[1], &hyALDuplicatesTable[0],
            (HYAL_DUPLICATES_TABLE_SIZE - 1U) * sizeof(HYAL_DUPLICATES_ENTRY));
        // Populate the new entry.
        hyALDuplicatesTable[0].srcAddress = srcAddr;
        hyALDuplicatesTable[0].msduLen = msduLen;
        hyALDuplicatesTable[0].crc = crc;
        hyALDuplicatesTable[0].mediaType = mediaType;
    }

    // Return duplicate or not
    return duplicate;
}

static bool lMAC_WRP_IsAttributeInPLCRange(MAC_WRP_PIB_ATTRIBUTE attribute)
{
    uint32_t attrId = (uint32_t) attribute;

    /* Check attribute ID range to distinguish between PLC and RF MAC */
    if (attrId < 0x00000200U)
    {
        /* Standard PLC MAC IB */
        return true;
    }
    else if (attrId < 0x00000400U)
    {
        /* Standard RF MAC IB */
        return false;
    }
    else if (attrId < 0x08000200U)
    {
        /* Manufacturer PLC MAC IB */
        return true;
    }
    else
    {
        /* Manufacturer RF MAC IB */
        return false;
    }
}

static bool lMAC_WRP_CheckRFMediaProbing(uint8_t probingInterval, MAC_ADDRESS dstAddress)
{
    MAC_WRP_POS_ENTRY_RF posEntry;
    MAC_PIB_VALUE pibValue;
    MAC_WRP_STATUS status;
    uint8_t posTableEntryTtl;
    uint8_t lqiValidTime;

    /* Probing interval has to be greater than 0 */
    if (probingInterval == 0U)
    {
        return false;
    }

    /* Destination addressing has to be Short Addressing */
    if (dstAddress.addressMode != MAC_ADDRESS_MODE_SHORT)
    {
        return false;
    }

    /* Look for entry in RF POS Table */
    status = (MAC_WRP_STATUS) MAC_RF_GetRequestSync(MAC_PIB_MANUF_POS_TABLE_ELEMENT_RF,
        dstAddress.shortAddress, &pibValue);

    if (status == MAC_WRP_STATUS_SUCCESS)
    {
        (void) memcpy((void *) &posEntry, (void *) pibValue.value, sizeof(MAC_WRP_POS_ENTRY_RF));
        status = (MAC_WRP_STATUS) MAC_COMMON_GetRequestSync(MAC_COMMON_PIB_POS_TABLE_ENTRY_TTL,
            0, &pibValue);

        if (status == MAC_WRP_STATUS_SUCCESS)
        {
            posTableEntryTtl = pibValue.value[0];
            lqiValidTime = (uint8_t)((posEntry.reverseLqiValidTime + 59U) / 60U);
            if ((posTableEntryTtl > lqiValidTime) && ((posTableEntryTtl - lqiValidTime) >= probingInterval))
            {
                /* Conditions met to perform the media probing */
                return true;
            }
        }
    }

    /* If this point is reached, no probing is done */
    return false;
}

static bool lMAC_WRP_CheckPLCMediaProbing(uint8_t probingInterval, MAC_ADDRESS dstAddress)
{
    MAC_WRP_POS_ENTRY posEntry;
    MAC_WRP_NEIGHBOUR_ENTRY nbEntry;
    MAC_PIB_VALUE pibValue;
    MAC_WRP_STATUS status;
    uint8_t tmrTtl;
    uint8_t tmrValidTime;

    /* Probing interval has to be greater than 0 */
    if (probingInterval == 0U)
    {
        return false;
    }

    /* Destination addressing has to be Short Addressing */
    if (dstAddress.addressMode != MAC_ADDRESS_MODE_SHORT)
    {
        return false;
    }

    /* Look for entry in POS Table */
    status = (MAC_WRP_STATUS) MAC_PLC_GetRequestSync(MAC_PIB_MANUF_POS_TABLE_ELEMENT,
        dstAddress.shortAddress, &pibValue);

    if (status == MAC_WRP_STATUS_SUCCESS)
    {
        (void) memcpy((void *) &posEntry, (void *) pibValue.value, sizeof(MAC_WRP_POS_ENTRY));

        /* Look for entry in Neighbour Table to fill TMR Valid time */
        tmrValidTime = 0;
        status = (MAC_WRP_STATUS) MAC_PLC_GetRequestSync(MAC_PIB_MANUF_NEIGHBOUR_TABLE_ELEMENT,
            dstAddress.shortAddress, &pibValue);

        if (status == MAC_WRP_STATUS_SUCCESS)
        {
            (void) memcpy((void *) &nbEntry, (void *) pibValue.value, sizeof(MAC_WRP_NEIGHBOUR_ENTRY));
            tmrValidTime = (uint8_t)((nbEntry.tmrValidTime + 59U) / 60U);
        }

        status = (MAC_WRP_STATUS) MAC_PLC_GetRequestSync(MAC_PIB_TMR_TTL, 0, &pibValue);
        if (status == MAC_WRP_STATUS_SUCCESS)
        {
            tmrTtl = pibValue.value[0];
            if ((tmrTtl > tmrValidTime) && ((tmrTtl - tmrValidTime) >= probingInterval))
            {
                /* Conditions met to perform the media probing */
                /* Reset TMR TTL for entry before probing, so TMR is exchanged */
                /* (pibValue is not used, same variable as prevoius can be used) */
                (void) MAC_PLC_SetRequestSync(MAC_PIB_MANUF_RESET_TMR_TTL, dstAddress.shortAddress, &pibValue);
                return true;
            }
        }
    }

    /* If this point is reached, no probing is done */
    return false;
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
    MAC_PIB_VALUE pibValue;
    MAC_WRP_STATUS status;
    bool sendConfirm = false;

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

    switch (matchingDataReq->dataReqMediaType)
    {
        case MAC_WRP_MEDIA_TYPE_REQ_PLC_BACKUP_RF:
            if (dcParams->status == MAC_STATUS_SUCCESS)
            {
                /* Check Media Probing */
                if (lMAC_WRP_CheckRFMediaProbing(matchingDataReq->probingInterval, matchingDataReq->dataReqParams.destAddress))
                {
                    /* Perform Media probing */
                    hyalData.mediaProbing = true;
                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "RF Media Probing\r\n");
                    /* Set Msdu pointer to backup buffer, as current pointer is no longer valid */
                    matchingDataReq->dataReqParams.msdu = matchingDataReq->backupBuffer;
                    MAC_RF_DataRequest(&matchingDataReq->dataReqParams);
                }
                else
                {
                    hyalData.mediaProbing = false;
                    /* Fill Media Type */
                    dataConfirmParams.mediaType = MAC_WRP_MEDIA_TYPE_CONF_PLC;
                    /* Send confirm to upper layer */
                    sendConfirm = true;
                }
            }
            else
            {
                /* Check Dest Address mode and/or RF POS table before attempting data request */
                if (matchingDataReq->dataReqParams.destAddress.addressMode == MAC_ADDRESS_MODE_EXTENDED)
                {
                    status = MAC_WRP_STATUS_SUCCESS;
                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "Extended Address Dest allows backup medium\r\n");
                }
                else
                {
                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "Look for RF POS Table entry for %0004X\r\n", matchingDataReq->dataReqParams.destAddress.shortAddress);
                    status = (MAC_WRP_STATUS) MAC_RF_GetRequestSync(MAC_PIB_MANUF_POS_TABLE_ELEMENT_RF,
                            matchingDataReq->dataReqParams.destAddress.shortAddress, &pibValue);
                }

                /* Check status to try backup medium */
                if (status == MAC_WRP_STATUS_SUCCESS)
                {
                    /* Try on backup medium */
                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "Try RF as Backup Medium\r\n");
                    /* Set Msdu pointer to backup buffer, as current pointer is no longer valid */
                    matchingDataReq->dataReqParams.msdu = matchingDataReq->backupBuffer;
                    MAC_RF_DataRequest(&matchingDataReq->dataReqParams);
                }
                else
                {
                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "No POS entry found, discard backup medium\r\n");
                    /* Fill Media Type */
                    dataConfirmParams.mediaType = MAC_WRP_MEDIA_TYPE_CONF_PLC;
                    /* Send confirm to upper layer */
                    sendConfirm = true;
                }
            }
            break;
        case MAC_WRP_MEDIA_TYPE_REQ_RF_BACKUP_PLC:
            if (hyalData.mediaProbing)
            {
                /* PLC was probed after RF success. Send confirm to upper layer. */
                hyalData.mediaProbing = false;
                dataConfirmParams.status = MAC_WRP_STATUS_SUCCESS;
                if (dcParams->status == MAC_STATUS_SUCCESS)
                {
                    dataConfirmParams.mediaType = MAC_WRP_MEDIA_TYPE_CONF_RF;
                }
                else
                {
                    dataConfirmParams.mediaType = MAC_WRP_MEDIA_TYPE_CONF_RF_AS_BACKUP;
                }
            }
            else
            {
                /* PLC was used as backup medium. Send confirm to upper layer. */
                dataConfirmParams.mediaType = MAC_WRP_MEDIA_TYPE_CONF_PLC_AS_BACKUP;
            }

            sendConfirm = true;
            break;
        case MAC_WRP_MEDIA_TYPE_REQ_BOTH:
            if (matchingDataReq->waitingSecondConfirm)
            {
                /* Second Confirm arrived. Send confirm to upper layer depending on results */
                dataConfirmParams.mediaType = MAC_WRP_MEDIA_TYPE_CONF_BOTH;
                if ((matchingDataReq->firstConfirmStatus == MAC_STATUS_SUCCESS) ||
                        (dcParams->status == MAC_STATUS_SUCCESS))
                {
                    /* At least one SUCCESS, send confirm with SUCCESS */
                    dataConfirmParams.status = MAC_WRP_STATUS_SUCCESS;
                }
                else
                {
                    /* None SUCCESS. Return result from second confirm */
                    dataConfirmParams.status = (MAC_WRP_STATUS)dcParams->status;
                }

                /* Send confirm to upper layer */
                sendConfirm = true;
            }
            else
            {
                /* This is the First Confirm, store status and wait for Second */
                matchingDataReq->firstConfirmStatus = dcParams->status;
                matchingDataReq->waitingSecondConfirm = true;
            }
            break;
        case MAC_WRP_MEDIA_TYPE_REQ_PLC_NO_BACKUP:
            /* Fill Media Type */
            dataConfirmParams.mediaType = MAC_WRP_MEDIA_TYPE_CONF_PLC;
            /* Send confirm to upper layer */
            sendConfirm = true;
            break;
        case MAC_WRP_MEDIA_TYPE_REQ_RF_NO_BACKUP:
            /* PLC confirm not expected on RF_NO_BACKUP request. Ignore it */
            matchingDataReq->used = false;
            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_ERROR, "lMAC_WRP_CallbackMacPlcDataConfirm() called from a MEDIA_TYPE_REQ_RF_NO_BACKUP request!!\r\n");
            break;
        default: /* PLC only */
            /* Fill Media Type */
            dataConfirmParams.mediaType = MAC_WRP_MEDIA_TYPE_CONF_PLC;
            /* Send confirm to upper layer */
            sendConfirm = true;
            break;
    }

    if (sendConfirm == true)
    {
        /* Release Data Req entry and send confirm to upper layer */
        matchingDataReq->used = false;
        if (macWrpData.macWrpHandlers.dataConfirmCallback != NULL)
        {
            macWrpData.macWrpHandlers.dataConfirmCallback(&dataConfirmParams);
        }
    }
}

static void lMAC_WRP_CallbackMacPlcDataIndication(MAC_DATA_INDICATION_PARAMS *diParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "lMAC_WRP_CallbackMacPlcDataIndication");

    MAC_WRP_DATA_INDICATION_PARAMS dataIndicationParams;

    /* Check if the same frame has been received on the other medium (duplicate detection), except for broadcast */
    if (MAC_SHORT_ADDRESS_BROADCAST != diParams->destAddress.shortAddress)
    {
        if (lMAC_WRP_HyalCheckDuplicates(diParams->srcAddress.shortAddress, diParams->msdu,
            diParams->msduLength, MAC_WRP_MEDIA_TYPE_IND_PLC))
        {
            /* Same frame was received on RF medium. Drop indication */
            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "Same frame was received on RF medium. Drop indication");
            return;
        }
    }

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

    MAC_WRP_RESET_CONFIRM_PARAMS resetConfirmParams;

    if (hyalData.waitingSecondResetConfirm)
    {
        /* Second Confirm arrived. Send confirm to upper layer depending on results */
        if ((hyalData.firstResetConfirmStatus == MAC_STATUS_SUCCESS) &&
                (rcParams->status == MAC_STATUS_SUCCESS))
        {
            /* Both SUCCESS, send confirm with SUCCESS */
            resetConfirmParams.status = MAC_WRP_STATUS_SUCCESS;
        }
        else
        {
            /* Check which reset failed and report its status */
            if (hyalData.firstResetConfirmStatus != MAC_STATUS_SUCCESS)
            {
                resetConfirmParams.status = (MAC_WRP_STATUS)hyalData.firstResetConfirmStatus;
            }
            else
            {
                resetConfirmParams.status = (MAC_WRP_STATUS)rcParams->status;
            }
        }

        if (macWrpData.macWrpHandlers.resetConfirmCallback != NULL)
        {
            macWrpData.macWrpHandlers.resetConfirmCallback(&resetConfirmParams);
        }
    }
    else
    {
        /* This is the First Confirm, store status and wait for Second */
        hyalData.firstResetConfirmStatus = rcParams->status;
        hyalData.waitingSecondResetConfirm = true;
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

    MAC_WRP_SCAN_CONFIRM_PARAMS scanConfirmParams;

    if (hyalData.waitingSecondScanConfirm)
    {
        /* Second Confirm arrived */
        if ((hyalData.firstScanConfirmStatus == MAC_STATUS_SUCCESS) ||
                (scParams->status == MAC_STATUS_SUCCESS))
        {
            /* One or Both SUCCESS, send confirm with SUCCESS */
            scanConfirmParams.status = MAC_WRP_STATUS_SUCCESS;
        }
        else
        {
            /* None of confirms SUCCESS, send confirm with latests status */
            scanConfirmParams.status = (MAC_WRP_STATUS)scParams->status;
        }

        /* Clear flag */
        macWrpData.scanRequestInProgress = false;

        /* Send confirm to upper layer */
        if (macWrpData.macWrpHandlers.scanConfirmCallback != NULL)
        {
            macWrpData.macWrpHandlers.scanConfirmCallback(&scanConfirmParams);
        }
    }
    else
    {
        /* This is the First Confirm, store status and wait for Second */
        hyalData.firstScanConfirmStatus = scParams->status;
        hyalData.waitingSecondScanConfirm = true;
    }
}

static void lMAC_WRP_CallbackMacPlcStartConfirm(MAC_START_CONFIRM_PARAMS *scParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "lMAC_WRP_CallbackMacPlcStartConfirm: Status: %u\r\n", scParams->status);

    MAC_WRP_START_CONFIRM_PARAMS startConfirmParams;

    if (hyalData.waitingSecondStartConfirm)
    {
        /* Second Confirm arrived. Send confirm to upper layer depending on results */
        if ((hyalData.firstStartConfirmStatus == MAC_STATUS_SUCCESS) &&
                (scParams->status == MAC_STATUS_SUCCESS))
        {
            /* Both SUCCESS, send confirm with SUCCESS */
            startConfirmParams.status = MAC_WRP_STATUS_SUCCESS;
        }
        else
        {
            /* Check which start failed and report its status */
            if (hyalData.firstStartConfirmStatus != MAC_STATUS_SUCCESS)
            {
                startConfirmParams.status = (MAC_WRP_STATUS)hyalData.firstStartConfirmStatus;
            }
            else
            {
                startConfirmParams.status = (MAC_WRP_STATUS)scParams->status;
            }
        }

        if (macWrpData.macWrpHandlers.startConfirmCallback != NULL)
        {
            macWrpData.macWrpHandlers.startConfirmCallback(&startConfirmParams);
        }
    }
    else
    {
        /* This is the First Confirm, store status and wait for Second */
        hyalData.firstStartConfirmStatus = scParams->status;
        hyalData.waitingSecondStartConfirm = true;
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

static void lMAC_WRP_CallbackMacRfDataConfirm(MAC_DATA_CONFIRM_PARAMS *dcParams)
{
    MAC_WRP_DATA_CONFIRM_PARAMS dataConfirmParams;
    MAC_WRP_DATA_REQ_ENTRY *matchingDataReq;
    MAC_PIB_VALUE pibValue;
    MAC_WRP_STATUS status;
    bool sendConfirm = false;

    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "lMAC_WRP_CallbackMacRfDataConfirm() Handle: 0x%02X Status: %u\r\n", dcParams->msduHandle, (uint8_t)dcParams->status);

    /* Get Data Request entry matching confirm */
    matchingDataReq = lMAC_WRP_GetDataReqEntryByHandle(dcParams->msduHandle);

    /* Avoid unmached handling */
    if (matchingDataReq == NULL)
    {
        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_ERROR, "lMAC_WRP_CallbackMacRfDataConfirm() Confirm does not match any previous request!!\r\n");
        return;
    }

    /* Copy dcParams from Mac */
    (void) memcpy((void *) &dataConfirmParams, (void *) dcParams, sizeof(MAC_DATA_CONFIRM_PARAMS));

    switch (matchingDataReq->dataReqMediaType)
    {
        case MAC_WRP_MEDIA_TYPE_REQ_PLC_BACKUP_RF:
            if (hyalData.mediaProbing)
            {
                /* RF was probed after PLC success. Send confirm to upper layer. */
                hyalData.mediaProbing = false;
                dataConfirmParams.status = MAC_WRP_STATUS_SUCCESS;
                if (dcParams->status == MAC_STATUS_SUCCESS)
                {
                    dataConfirmParams.mediaType = MAC_WRP_MEDIA_TYPE_CONF_PLC;
                }
                else
                {
                    dataConfirmParams.mediaType = MAC_WRP_MEDIA_TYPE_CONF_PLC_AS_BACKUP;
                }
            }
            else
            {
                /* RF was used as backup medium. Send confirm to upper layer */
                dataConfirmParams.mediaType = MAC_WRP_MEDIA_TYPE_CONF_RF_AS_BACKUP;
            }

            sendConfirm = true;
            break;
        case MAC_WRP_MEDIA_TYPE_REQ_RF_BACKUP_PLC:
            if (dcParams->status == MAC_STATUS_SUCCESS)
            {
                /* Check Media Probing */
                if (lMAC_WRP_CheckPLCMediaProbing(matchingDataReq->probingInterval, matchingDataReq->dataReqParams.destAddress))
                {
                    /* Perform Media probing */
                    hyalData.mediaProbing = true;
                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "PLC Media Probing\r\n");
                    /* Set Msdu pointer to backup buffer, as current pointer is no longer valid */
                    matchingDataReq->dataReqParams.msdu = matchingDataReq->backupBuffer;
                    MAC_PLC_DataRequest(&matchingDataReq->dataReqParams);
                }
                else
                {
                    /* Fill Media Type */
                    dataConfirmParams.mediaType = MAC_WRP_MEDIA_TYPE_CONF_RF;
                    /* Send confirm to upper layer */
                    sendConfirm = true;
                }
            }
            else
            {
                /* Check Dest Address mode and/or PLC POS table before attempting data request */
                if (matchingDataReq->dataReqParams.destAddress.addressMode == MAC_ADDRESS_MODE_EXTENDED)
                {
                    status = MAC_WRP_STATUS_SUCCESS;
                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "Extended Address Dest allows backup medium\r\n");
                }
                else
                {
                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "Look for PLC POS Table entry for %0004X\r\n", matchingDataReq->dataReqParams.destAddress.shortAddress);
                    status = (MAC_WRP_STATUS) MAC_PLC_GetRequestSync(MAC_PIB_MANUF_POS_TABLE_ELEMENT,
                            matchingDataReq->dataReqParams.destAddress.shortAddress, &pibValue);
                }

                /* Check status to try backup medium */
                if (status == MAC_WRP_STATUS_SUCCESS)
                {
                    /* Try on backup medium */
                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "Try PLC as Backup Medium\r\n");
                    /* Set Msdu pointer to backup buffer, as current pointer is no longer valid */
                    matchingDataReq->dataReqParams.msdu = matchingDataReq->backupBuffer;
                    MAC_PLC_DataRequest(&matchingDataReq->dataReqParams);
                }
                else
                {
                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "No POS entry found, discard backup medium\r\n");
                    /* Fill Media Type */
                    dataConfirmParams.mediaType = MAC_WRP_MEDIA_TYPE_CONF_RF;
                    /* Send confirm to upper layer */
                    sendConfirm = true;
                }
            }
            break;
        case MAC_WRP_MEDIA_TYPE_REQ_BOTH:
            if (matchingDataReq->waitingSecondConfirm)
            {
                /* Second Confirm arrived. Send confirm to upper layer depending on results */
                dataConfirmParams.mediaType = MAC_WRP_MEDIA_TYPE_CONF_BOTH;
                if ((matchingDataReq->firstConfirmStatus == MAC_STATUS_SUCCESS) ||
                        (dcParams->status == MAC_STATUS_SUCCESS))
                {
                    /* At least one SUCCESS, send confirm with SUCCESS */
                    dataConfirmParams.status = MAC_WRP_STATUS_SUCCESS;
                }
                else
                {
                    /* None SUCCESS. Return result from second confirm */
                    dataConfirmParams.status = (MAC_WRP_STATUS)dcParams->status;
                }

                /* Send confirm to upper layer */
                sendConfirm = true;
            }
            else
            {
                /* This is the First Confirm, store status and wait for Second */
                matchingDataReq->firstConfirmStatus = dcParams->status;
                matchingDataReq->waitingSecondConfirm = true;
            }
            break;
        case MAC_WRP_MEDIA_TYPE_REQ_PLC_NO_BACKUP:
            /* RF confirm not expected on PLC_NO_BACKUP request. Ignore it */
            matchingDataReq->used = false;
            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_ERROR, "lMAC_WRP_CallbackMacRfDataConfirm() called from a MEDIA_TYPE_REQ_PLC_NO_BACKUP request!!\r\n");
            break;
        case MAC_WRP_MEDIA_TYPE_REQ_RF_NO_BACKUP:
            /* Fill Media Type */
            dataConfirmParams.mediaType = MAC_WRP_MEDIA_TYPE_CONF_RF;
            /* Send confirm to upper layer */
            sendConfirm = true;
            break;
        default: /* RF only */
            /* Fill Media Type */
            dataConfirmParams.mediaType = MAC_WRP_MEDIA_TYPE_CONF_RF;
            /* Send confirm to upper layer */
            sendConfirm = true;
            break;
    }

    if (sendConfirm == true)
    {
        /* Release Data Req entry and send confirm to upper layer */
        matchingDataReq->used = false;
        if (macWrpData.macWrpHandlers.dataConfirmCallback != NULL)
        {
            macWrpData.macWrpHandlers.dataConfirmCallback(&dataConfirmParams);
        }
    }
}

static void lMAC_WRP_CallbackMacRfDataIndication(MAC_DATA_INDICATION_PARAMS *diParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "lMAC_WRP_CallbackMacRfDataIndication");

    MAC_WRP_DATA_INDICATION_PARAMS dataIndicationParams;

    /* Check if the same frame has been received on the other medium (duplicate detection), except for broadcast */
    if (MAC_SHORT_ADDRESS_BROADCAST != diParams->destAddress.shortAddress)
    {
        if (lMAC_WRP_HyalCheckDuplicates(diParams->srcAddress.shortAddress, diParams->msdu,
            diParams->msduLength, MAC_WRP_MEDIA_TYPE_IND_RF))
        {
            /* Same frame was received on PLC medium. Drop indication */
            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "Same frame was received on PLC medium. Drop indication");
            return;
        }
    }

    /* Copy diParams from Mac and fill Media Type */
    (void) memcpy((void *) &dataIndicationParams, (void *) diParams, sizeof(MAC_DATA_INDICATION_PARAMS));
    dataIndicationParams.mediaType = MAC_WRP_MEDIA_TYPE_IND_RF;
    if (macWrpData.macWrpHandlers.dataIndicationCallback != NULL)
    {
        macWrpData.macWrpHandlers.dataIndicationCallback(&dataIndicationParams);
    }
}

static void lMAC_WRP_CallbackMacRfResetConfirm(MAC_RESET_CONFIRM_PARAMS *rcParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "lMAC_WRP_CallbackMacRfResetConfirm: Status: %u\r\n", rcParams->status);

    MAC_WRP_RESET_CONFIRM_PARAMS resetConfirmParams;

    if (hyalData.waitingSecondResetConfirm)
    {
        /* Second Confirm arrived. Send confirm to upper layer depending on results */
        if ((hyalData.firstResetConfirmStatus == MAC_STATUS_SUCCESS) &&
                (rcParams->status == MAC_STATUS_SUCCESS))
        {
            /* Both SUCCESS, send confirm with SUCCESS */
            resetConfirmParams.status = MAC_WRP_STATUS_SUCCESS;
        }
        else
        {
            /* Check which reset failed and report its status */
            if (hyalData.firstResetConfirmStatus != MAC_STATUS_SUCCESS)
            {
                resetConfirmParams.status = (MAC_WRP_STATUS)hyalData.firstResetConfirmStatus;
            }
            else
            {
                resetConfirmParams.status = (MAC_WRP_STATUS)rcParams->status;
            }
        }

        if (macWrpData.macWrpHandlers.resetConfirmCallback != NULL)
        {
            macWrpData.macWrpHandlers.resetConfirmCallback(&resetConfirmParams);
        }
    }
    else
    {
        /* This is the First Confirm, store status and wait for Second */
        hyalData.firstResetConfirmStatus = rcParams->status;
        hyalData.waitingSecondResetConfirm = true;
    }
}

static void lMAC_WRP_CallbackMacRfBeaconNotify(MAC_BEACON_NOTIFY_INDICATION_PARAMS *bnParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "lMAC_WRP_CallbackMacRfBeaconNotify: Pan ID: %04X\r\n", bnParams->panDescriptor.panId);

    MAC_WRP_BEACON_NOTIFY_INDICATION_PARAMS notifyIndicationParams;

    /* Copy bnParams from Mac and fill Media Type */
    (void) memcpy((void *) &notifyIndicationParams, (void *) bnParams, sizeof(MAC_BEACON_NOTIFY_INDICATION_PARAMS));
    notifyIndicationParams.panDescriptor.mediaType = MAC_WRP_MEDIA_TYPE_IND_RF;

    if (macWrpData.macWrpHandlers.beaconNotifyIndicationCallback != NULL)
    {
        macWrpData.macWrpHandlers.beaconNotifyIndicationCallback(&notifyIndicationParams);
    }
}

static void lMAC_WRP_CallbackMacRfScanConfirm(MAC_SCAN_CONFIRM_PARAMS *scParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "lMAC_WRP_CallbackMacRfScanConfirm: Status: %u\r\n", scParams->status);

    MAC_WRP_SCAN_CONFIRM_PARAMS scanConfirmParams;

    if (hyalData.waitingSecondScanConfirm)
    {
        /* Second Confirm arrived */
        if ((hyalData.firstScanConfirmStatus == MAC_STATUS_SUCCESS) ||
                (scParams->status == MAC_STATUS_SUCCESS))
        {
            /* One or Both SUCCESS, send confirm with SUCCESS */
            scanConfirmParams.status = MAC_WRP_STATUS_SUCCESS;
        }
        else
        {
            /* None of confirms SUCCESS, send confirm with latests status */
            scanConfirmParams.status = (MAC_WRP_STATUS)scParams->status;
        }

        /* Clear flag */
        macWrpData.scanRequestInProgress = false;

        /* Send confirm to upper layer */
        if (macWrpData.macWrpHandlers.scanConfirmCallback != NULL)
        {
            macWrpData.macWrpHandlers.scanConfirmCallback(&scanConfirmParams);
        }
    }
    else
    {
        /* This is the First Confirm, store status and wait for Second */
        hyalData.firstScanConfirmStatus = scParams->status;
        hyalData.waitingSecondScanConfirm = true;
    }
}

static void lMAC_WRP_CallbackMacRfStartConfirm(MAC_START_CONFIRM_PARAMS *scParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "lMAC_WRP_CallbackMacRfStartConfirm: Status: %u\r\n", scParams->status);

    MAC_WRP_START_CONFIRM_PARAMS startConfirmParams;

    if (hyalData.waitingSecondStartConfirm)
    {
        /* Second Confirm arrived. Send confirm to upper layer depending on results */
        if ((hyalData.firstStartConfirmStatus == MAC_STATUS_SUCCESS) &&
                (scParams->status == MAC_STATUS_SUCCESS))
        {
            /* Both SUCCESS, send confirm with SUCCESS */
            startConfirmParams.status = MAC_WRP_STATUS_SUCCESS;
        }
        else
        {
            /* Check which start failed and report its status */
            if (hyalData.firstStartConfirmStatus != MAC_STATUS_SUCCESS)
            {
                startConfirmParams.status = (MAC_WRP_STATUS)hyalData.firstStartConfirmStatus;
            }
            else
            {
                startConfirmParams.status = (MAC_WRP_STATUS)scParams->status;
            }
        }

        if (macWrpData.macWrpHandlers.startConfirmCallback != NULL)
        {
            macWrpData.macWrpHandlers.startConfirmCallback(&startConfirmParams);
        }
    }
    else
    {
        /* This is the First Confirm, store status and wait for Second */
        hyalData.firstStartConfirmStatus = scParams->status;
        hyalData.waitingSecondStartConfirm = true;
    }
}

static void lMAC_WRP_CallbackMacRfCommStatusIndication(MAC_COMM_STATUS_INDICATION_PARAMS *csParams)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "lMAC_WRP_CallbackMacRfCommStatusIndication: Status: %u\r\n", csParams->status);

    MAC_WRP_COMM_STATUS_INDICATION_PARAMS commStatusIndicationParams;

    /* Copy csParams from Mac and fill Media Type */
    (void) memcpy((void *) &commStatusIndicationParams, (void *) csParams, sizeof(MAC_COMM_STATUS_INDICATION_PARAMS));
    commStatusIndicationParams.mediaType = MAC_WRP_MEDIA_TYPE_IND_RF;

    if (macWrpData.macWrpHandlers.commStatusIndicationCallback != NULL)
    {
        macWrpData.macWrpHandlers.commStatusIndicationCallback(&commStatusIndicationParams);
    }
}

static void lMAC_WRP_CallbackMacRfMacSnifferIndication(MAC_SNIFFER_INDICATION_PARAMS *siParams)
{
    SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, siParams->msdu, siParams->msduLength, "lMAC_WRP_CallbackMacRfMacSnifferIndication:  MSDU:");

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

MAC_WRP_HANDLE MAC_WRP_Open(SYS_MODULE_INDEX index, MAC_WRP_BAND band)
{
    MAC_PLC_INIT plcInitData;
    MAC_RF_INIT rfInitData;

    /* Single instance allowed */
    if (index >= G3_MAC_WRP_INSTANCES_NUMBER)
    {
        return MAC_WRP_HANDLE_INVALID;
    }

    /* Set default HyAL variables */
    hyalData = hyalDataDefaults;
    (void) memset(hyALDuplicatesTable, 0, sizeof(hyALDuplicatesTable));

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
    plcInitData.plcBand = (MAC_PLC_BAND) band;
    /* Get PAL index from configuration header */
    plcInitData.palPlcIndex = PAL_PLC_PHY_INDEX;

    MAC_PLC_Init(&plcInitData);

    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "MAC_WRP_Open: Initializing RF MAC...\r\n");

    rfInitData.macRfHandlers.macRfDataConfirm = lMAC_WRP_CallbackMacRfDataConfirm;
    rfInitData.macRfHandlers.macRfDataIndication = lMAC_WRP_CallbackMacRfDataIndication;
    rfInitData.macRfHandlers.macRfResetConfirm = lMAC_WRP_CallbackMacRfResetConfirm;
    rfInitData.macRfHandlers.macRfBeaconNotifyIndication = lMAC_WRP_CallbackMacRfBeaconNotify;
    rfInitData.macRfHandlers.macRfScanConfirm = lMAC_WRP_CallbackMacRfScanConfirm;
    rfInitData.macRfHandlers.macRfStartConfirm = lMAC_WRP_CallbackMacRfStartConfirm;
    rfInitData.macRfHandlers.macRfCommStatusIndication = lMAC_WRP_CallbackMacRfCommStatusIndication;
    rfInitData.macRfHandlers.macRfMacSnifferIndication = lMAC_WRP_CallbackMacRfMacSnifferIndication;

    (void) memset(macRfPOSTable, 0, sizeof(macRfPOSTable));
    (void) memset(macRfDeviceTable, 0, sizeof(macRfDeviceTable));
    (void) memset(macRfDsnTable, 0, sizeof(macRfDsnTable));

    macRfTables.macRfDeviceTableSize = MAC_MAX_DEVICE_TABLE_ENTRIES_RF;
    macRfTables.macRfDsnTableSize = MAC_MAX_DSN_TABLE_ENTRIES_RF;
    macRfTables.macRfPosTableSize = MAC_MAX_POS_TABLE_ENTRIES_RF;
    macRfTables.macRfPosTable = macRfPOSTable;
    macRfTables.macRfDeviceTable = macRfDeviceTable;
    macRfTables.macRfDsnTable = macRfDsnTable;

    rfInitData.macRfTables = &macRfTables;
    /* Get PAL index from configuration header */
    rfInitData.palRfIndex = PAL_RF_PHY_INDEX;

    MAC_RF_Init(&rfInitData);

    MAC_COMMON_Init();

    macWrpData.state = MAC_WRP_STATE_IDLE;

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

    MAC_PLC_Tasks();
    MAC_RF_Tasks();
}

SYS_STATUS MAC_WRP_Status(void)
{
    SYS_STATUS plcStatus = MAC_PLC_Status();
    SYS_STATUS rfStatus = MAC_RF_Status();
    if ((plcStatus == SYS_STATUS_UNINITIALIZED) || (rfStatus == SYS_STATUS_UNINITIALIZED))
    {
        return SYS_STATUS_UNINITIALIZED;
    }
    if ((plcStatus == SYS_STATUS_BUSY) || (rfStatus == SYS_STATUS_BUSY))
    {
        return SYS_STATUS_BUSY;
    }
    else if ((plcStatus == SYS_STATUS_READY) || (rfStatus == SYS_STATUS_READY))
    {
        return SYS_STATUS_READY;
    }
    else
    {
        return SYS_STATUS_ERROR;
    }
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
    /* Copy MediaType */
    dataReqEntry->dataReqMediaType = drParams->mediaType;
    /* Copy Probing Interval */
    dataReqEntry->probingInterval = drParams->probingInterval;
    /* Copy data to backup buffer, just in case backup media has to be used, current pointer will not be valid later */
    if (drParams->msduLength <= HYAL_BACKUP_BUF_SIZE)
    {
        (void) memcpy(dataReqEntry->backupBuffer, drParams->msdu, drParams->msduLength);
    }

    switch (dataReqEntry->dataReqMediaType)
    {
        case MAC_WRP_MEDIA_TYPE_REQ_PLC_BACKUP_RF:
        case MAC_WRP_MEDIA_TYPE_REQ_PLC_NO_BACKUP:
            MAC_PLC_DataRequest(&dataReqEntry->dataReqParams);
            break;
        case MAC_WRP_MEDIA_TYPE_REQ_RF_BACKUP_PLC:
        case MAC_WRP_MEDIA_TYPE_REQ_RF_NO_BACKUP:
            MAC_RF_DataRequest(&dataReqEntry->dataReqParams);
            break;
        case MAC_WRP_MEDIA_TYPE_REQ_BOTH:
            /* Set control variable */
            dataReqEntry->waitingSecondConfirm = false;
            /* Request on both Media */
            MAC_PLC_DataRequest(&dataReqEntry->dataReqParams);
            MAC_RF_DataRequest(&dataReqEntry->dataReqParams);
            break;
        default: /* PLC only */
            dataReqEntry->dataReqMediaType = MAC_WRP_MEDIA_TYPE_REQ_PLC_NO_BACKUP;
            MAC_PLC_DataRequest(&dataReqEntry->dataReqParams);
            break;
    }
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

    /* Check attribute ID range to redirect to Common, PLC or RF MAC */
    if (lMAC_WRP_IsSharedAttribute(attribute))
    {
        /* Get from MAC Common */
        return (MAC_WRP_STATUS)(MAC_COMMON_GetRequestSync((MAC_COMMON_PIB_ATTRIBUTE)attribute, index, (void *)pibValue));
    }
    else if (lMAC_WRP_IsAttributeInPLCRange(attribute))
    {
        /* Get from PLC MAC */
        return (MAC_WRP_STATUS)(MAC_PLC_GetRequestSync((MAC_PLC_PIB_ATTRIBUTE)attribute, index, (void *)pibValue));
    }
    else
    {
        /* Get from RF MAC */
        return (MAC_WRP_STATUS)(MAC_RF_GetRequestSync((MAC_RF_PIB_ATTRIBUTE)attribute, index, (void *)pibValue));
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

    /* Check attribute ID range to redirect to Common, PLC or RF MAC */
    if (lMAC_WRP_IsSharedAttribute(attribute))
    {
        /* Set to MAC Common */
        return (MAC_WRP_STATUS)(MAC_COMMON_SetRequestSync((MAC_COMMON_PIB_ATTRIBUTE)attribute, index, (const void *)pibValue));
    }
    else if (lMAC_WRP_IsAttributeInPLCRange(attribute))
    {
        /* Set to PLC MAC */
        return (MAC_WRP_STATUS)(MAC_PLC_SetRequestSync((MAC_PLC_PIB_ATTRIBUTE)attribute, index, (const void *)pibValue));
    }
    else
    {
        /* Set to RF MAC */
        return (MAC_WRP_STATUS)(MAC_RF_SetRequestSync((MAC_RF_PIB_ATTRIBUTE)attribute, index, (const void *)pibValue));
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

    // Set control variable
    hyalData.waitingSecondResetConfirm = false;
    // Reset PLC MAC
    MAC_PLC_ResetRequest((void *)rstParams);
    // Reset RF MAC
    MAC_RF_ResetRequest((void *)rstParams);
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
    hyalData.waitingSecondScanConfirm = false;
    // Set PLC MAC on Scan state
    MAC_PLC_ScanRequest((void *)scanParams);
    // Set RF MAC on Scan state
    MAC_RF_ScanRequest((void *)scanParams);
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

    // Set control variable
    hyalData.waitingSecondStartConfirm = false;
    // Start Network on PLC MAC
    MAC_PLC_StartRequest((void *)startParams);
    // Start Network on PLC MAC
    MAC_RF_StartRequest((void *)startParams);
}

MAC_WRP_AVAILABLE_MAC_LAYERS MAC_WRP_GetAvailableMacLayers(MAC_WRP_HANDLE handle)
{
    return MAC_WRP_AVAILABLE_MAC_BOTH;
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
