/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    lbp_coord.c

  Summary:
    LBP Bootstrapping duties on PAN Coordinator.

  Description:
    The LoWPAN Bootstrapping Protocol (LBP) provides a simple interface to
    manage the G3 boostrap process Adaptation Layer. This file provides the
    interface to manage LBP process for coordinator.
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

#include "adp.h"
#include "adp_api_types.h"
#include "stack/g3/mac/mac_wrapper/mac_wrapper.h"
#include "eap_psk.h"
#include "lbp_coord.h"
#include "lbp_encode_decode.h"
#include "lbp_defs.h"
#include "service/log_report/srv_log_report.h"
#include "service/random/srv_random.h"

// *****************************************************************************
// *****************************************************************************
// Section: Macro Definitions
// *****************************************************************************
// *****************************************************************************

/* TODO: Make all defines MCC configurable */
/* LBP_NUM_SLOTS defines the number of parallel bootstrap procedures that can be carried out */
#define LBP_NUM_SLOTS 5
#define BOOTSTRAP_MSG_MAX_RETRIES 1
#define G3_COORDINATOR_PAN_ID                   0x781D
#define MAX_LBDS                                500
#define LBS_INVALID_SHORT_ADDRESS               0
#define INITIAL_KEY_INDEX                       0

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

typedef enum
{
    LBP_STATE_WAITING_JOINNING = 0,
    LBP_STATE_SENT_EAP_MSG_1,
    LBP_STATE_WAITING_EAP_MSG_2,
    LBP_STATE_SENT_EAP_MSG_3,
    LBP_STATE_WAITING_EAP_MSG_4,
    LBP_STATE_SENT_EAP_MSG_ACCEPTED,
    LBP_STATE_SENT_EAP_MSG_DECLINED,

} LBP_SLOT_STATE;

typedef struct
{
    LBP_SLOT_STATE slotState;
    ADP_EXTENDED_ADDRESS lbdAddress;
    uint16_t lbaAddress;
    uint16_t assignedShortAddress;
    uint8_t txHandle;
    uint32_t timeout;
    uint8_t txAttempts;
    EAP_PSK_RAND randS;
    uint32_t nonce;
    uint8_t pendingConfirms;
    uint8_t pendingTxHandle;
    uint8_t lbpData[ADP_LBP_MAX_NSDU_LENGTH];
    uint16_t lbpDataLength;
    EAP_PSK_CONTEXT pskContext;
    uint8_t mediaType;
    uint8_t disableBackupMedium;

} LBP_SLOT;

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Variables
// *****************************************************************************
// *****************************************************************************

static uint8_t sNsduHandle = 0;
static uint8_t sMaxHops = 0;
static uint16_t sMsgTimeoutSeconds = 300;
static uint8_t sCurrKeyIndex = 0;
static uint8_t sCurrGMK[16]  =
    {0xAF, 0x4D, 0x6D, 0xCC, 0xF1, 0x4D, 0xE7, 0xC1, 0xC4, 0x23, 0x5E, 0x6F, 0xEF, 0x6C, 0x15, 0x1F};
static uint8_t sRekeyGMK[16] =
    {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16};
static EAP_PSK_KEY sEapPskKey = {
    {0xAB, 0x10, 0x34, 0x11, 0x45, 0x11, 0x1B, 0xC3, 0xC1, 0x2D, 0xE8, 0xFF, 0x11, 0x14, 0x22, 0x04}
};
static EAP_PSK_NETWORK_ACCESS_IDENTIFIER_S sIdS;
static const EAP_PSK_NETWORK_ACCESS_IDENTIFIER_S sIdSArib =
    {LBP_NETWORK_ACCESS_ID_SIZE_S_ARIB,
    {0x53, 0x4D, 0xAD, 0xB2, 0xC4, 0xD5, 0xE6, 0xFA, 0x53, 0x4D, 0xAD, 0xB2, 0xC4, 0xD5, 0xE6, 0xFA,
    0x53, 0x4D, 0xAD, 0xB2, 0xC4, 0xD5, 0xE6, 0xFA, 0x53, 0x4D, 0xAD, 0xB2, 0xC4, 0xD5, 0xE6, 0xFA,
    0x53, 0x4D}};
static const EAP_PSK_NETWORK_ACCESS_IDENTIFIER_S sIdSCenFcc =
    {LBP_NETWORK_ACCESS_ID_SIZE_S_CENELEC_FCC,
    {0x81, 0x72, 0x63, 0x54, 0x45, 0x36, 0x27, 0x18}};
static bool sAribBand;
static uint8_t sEAPIdentifier = 0;
static bool sRekey;
static uint16_t sInternalAssignedAddress = 0;

static LBP_NOTIFICATIONS_COORD sLbpNotifications = {NULL};

static LBP_SLOT sLbpSlot[LBP_NUM_SLOTS];

// *****************************************************************************
// *****************************************************************************
// Section: local functions
// *****************************************************************************
// *****************************************************************************

static uint8_t _GetAdpMaxHops(void)
{
    ADP_GET_CFM_PARAMS getConfirm;
    ADP_GetRequestSync(ADP_IB_MAX_HOPS, 0, &getConfirm);
    return getConfirm.attributeValue[0];
}

static bool _SetDeviceTypeCoord(void)
{
    ADP_SET_CFM_PARAMS setConfirm;
    uint8_t devType = 1;
    bool result = false;

    // Set on ADP
    ADP_SetRequestSync(ADP_IB_DEVICE_TYPE, 0, sizeof(devType), &devType, &setConfirm);
    if (setConfirm.status == G3_SUCCESS)
    {
        result = true;
    }

    return result;
}

static void _logShowSlotStatus(LBP_SLOT *pSlot)
{
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, 
            "[LBP] Updating slot with LBD_ADDR: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X, \
            state: %hu, handler: %hu  pending_cfrms: %hu  Timeout: %u, Current_Time: %u\r\n",
            pSlot->lbdAddress.value[0], pSlot->lbdAddress.value[1],
            pSlot->lbdAddress.value[2], pSlot->lbdAddress.value[3],
            pSlot->lbdAddress.value[4], pSlot->lbdAddress.value[5],
            pSlot->lbdAddress.value[6], pSlot->lbdAddress.value[7],
            pSlot->slotState, pSlot->txHandle, pSlot->pendingConfirms,
            pSlot->timeout, MAC_WRP_GetMsCounter());

    (void)(pSlot);
}

static void  _initLbpSlots(void)
{
    uint8_t idx;

    for (idx = 0; idx < LBP_NUM_SLOTS; idx++)
    {
        sLbpSlot[idx].slotState =  LBP_STATE_WAITING_JOINNING;
        sLbpSlot[idx].pendingConfirms = 0;
        sLbpSlot[idx].txHandle =  0xff;
        sLbpSlot[idx].nonce =  0;

        memset(sLbpSlot[idx].lbdAddress.value, 0xff, sizeof(sLbpSlot[idx].lbdAddress.value));
    }
}

static uint8_t _getNextNsduHandle(void)
{
    return sNsduHandle++;
}

static void _setKeyingTable(uint8_t keyIndex, uint8_t *key)
{
    ADP_SET_CFM_PARAMS setConfirm;

    /* Set on MAC and ADP */
    ADP_MacSetRequestSync(MAC_WRP_PIB_KEY_TABLE, keyIndex, 16, key, &setConfirm);
    ADP_SetRequestSync(ADP_IB_ACTIVE_KEY_INDEX, 0, sizeof(keyIndex), &keyIndex, &setConfirm);
    sCurrKeyIndex = keyIndex;
}

static uint8_t _process_accepted_GMK_activation(ADP_EXTENDED_ADDRESS *pLBPEUI64Address, LBP_SLOT *pSlot)
{
    unsigned char *pMemoryBuffer = &pSlot->lbpData[0];
    unsigned short memoryBufferLength = sizeof(pSlot->lbpData);
    uint8_t pdata[3];
    uint8_t result = 1;
    uint8_t newKeyIndex;

    /* Get current key index and set the new one to the other */
    if (sCurrKeyIndex == 0)
    {
        newKeyIndex = 1;
    }
    else
    {
        newKeyIndex = 0;
    }

    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Accepted(GMK-activation).\r\n");

    /* Prepare the protected data carring the key and short addr */
    pdata[0] = LBP_CONF_PARAM_GMK_ACTIVATION;
    pdata[1] = 0x01;
    pdata[2] = newKeyIndex; /* key id */

    pSlot->lbpDataLength = EAP_PSK_EncodeGMKActivation(
            pdata,                 /* PCHANNEL data */
            memoryBufferLength,
            pMemoryBuffer);

    /* Encode now the LBP message */
    pSlot->lbpDataLength = LBP_EncodeAcceptedRequest(
            pLBPEUI64Address,
            pSlot->mediaType,
            pSlot->disableBackupMedium,
            pSlot->lbpDataLength,
            memoryBufferLength,
            pMemoryBuffer);

    return(result);
}

static void _processJoining0(ADP_EXTENDED_ADDRESS *pLBPEUI64Address, LBP_SLOT *pSlot)
{
    uint8_t *pMemoryBuffer = &pSlot->lbpData[0];
    uint16_t memoryBufferLength = sizeof(pSlot->lbpData);

    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Process Joining 0.\r\n");

    EAP_PSK_Initialize(&sEapPskKey, &pSlot->pskContext);

    /* Initialize RandS */
    SRV_RANDOM_Get128bits(pSlot->randS.value);

    pSlot->lbpDataLength = EAP_PSK_EncodeMessage1(
            sEAPIdentifier,
            &pSlot->randS,
            &sIdS,
            memoryBufferLength,
            pMemoryBuffer
            );

    sEAPIdentifier++;

    pSlot->lbpDataLength = LBP_EncodeChallengeRequest(
            pLBPEUI64Address,
            pSlot->mediaType,
            pSlot->disableBackupMedium,
            pSlot->lbpDataLength,
            memoryBufferLength,
            pMemoryBuffer
            );
}

static bool _processJoiningEAPT1(ADP_EXTENDED_ADDRESS lbpEUI64Address, uint16_t EAPDataLength,
        uint8_t *pEAPData, LBP_SLOT *pSlot)
{
    EAP_PSK_RAND randS;
    EAP_PSK_RAND randP;
    uint8_t *pMemoryBuffer = &pSlot->lbpData[0];
    uint16_t memoryBufferLength = sizeof(pSlot->lbpData);
    uint8_t pdata[50];
    uint16_t PDataLen = 0;
    uint16_t shortAddr;
    uint8_t newKeyIndex;

    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Process Joining EAP T1.\r\n");

    if (EAP_PSK_DecodeMessage2(sAribBand, EAPDataLength, pEAPData, &pSlot->pskContext, &sIdS, &randS, &randP))
    {
        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Decoded Message2.\r\n");

        if (memcmp(randS.value, pSlot->randS.value, sizeof(randS.value)) != 0)
        {
            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] ERROR: Bad RandS received\r\n");
            return false;
        }

        EAP_PSK_InitializeTEKMSK(&randP, &pSlot->pskContext);

        /* Encode and send the message T2 */
        shortAddr = pSlot->assignedShortAddress;

        /* Prepare the protected data carring the key and short addr */
        pdata[PDataLen++] = 0x02; /* ext field */

        if (!sRekey)
        {
            pdata[PDataLen++] = LBP_CONF_PARAM_SHORT_ADDR;
            pdata[PDataLen++] = 2;
            pdata[PDataLen++] = (uint8_t)((shortAddr & 0xFF00) >> 8);
            pdata[PDataLen++] = (uint8_t)(shortAddr & 0x00FF);

            pdata[PDataLen++] = LBP_CONF_PARAM_GMK;
            pdata[PDataLen++] = 17;
            pdata[PDataLen++] = sCurrKeyIndex; /* key id */
            memcpy(&pdata[PDataLen], sCurrGMK, 16); /* key */
            PDataLen += 16;

            pdata[PDataLen++] = LBP_CONF_PARAM_GMK_ACTIVATION;
            pdata[PDataLen++] = 1;
            pdata[PDataLen++] = sCurrKeyIndex; /* key id */
        }
        else
        {
            /* Get current key index and set the new one to the other */
            if (sCurrKeyIndex == 0)
            {
                newKeyIndex = 1;
            }
            else
            {
                newKeyIndex = 0;
            }

            pdata[PDataLen++] = LBP_CONF_PARAM_GMK;
            pdata[PDataLen++] = 17;
            pdata[PDataLen++] = newKeyIndex; /* key id */
            memcpy(&pdata[PDataLen], sRekeyGMK, 16); /* key */
            PDataLen += 16;
        }

        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Encoding Message3.\r\n");
        pSlot->lbpDataLength = EAP_PSK_EncodeMessage3(
                &pSlot->pskContext,
                sEAPIdentifier,
                &randS,
                &randP,
                &sIdS,
                pSlot->nonce,
                PCHANNEL_RESULT_DONE_SUCCESS,
                PDataLen,
                pdata,
                memoryBufferLength,
                pMemoryBuffer
                );

        sEAPIdentifier++;
        pSlot->nonce++;

        /* Encode now the LBP message */
        pSlot->lbpDataLength = LBP_EncodeChallengeRequest(
                &lbpEUI64Address,
                pSlot->mediaType,
                pSlot->disableBackupMedium,
                pSlot->lbpDataLength,
                memoryBufferLength,
                pMemoryBuffer
                );

        return true;
    }
    else
    {
        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] ERROR: _processJoiningEAPT1.\r\n");
        return false;
    }
}

static bool _processJoiningEAPT3(ADP_EXTENDED_ADDRESS lbpEUI64Address, uint8_t *pLbpData, uint16_t EAPDataLength,
        uint8_t *pEAPData, LBP_SLOT *pSlot)
{
    EAP_PSK_RAND randS;
    uint8_t PChannelResult = 0;
    uint32_t auxNonce = 0;
    uint16_t PChannelDataLength = 0;
    uint8_t *pPChannelData = 0L;
    uint8_t *pMemoryBuffer = &pSlot->lbpData[0];
    uint16_t memoryBufferLength = sizeof(pSlot->lbpData);

    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Process Joining EAP T3.\r\n");

    if (EAP_PSK_DecodeMessage4(EAPDataLength, pEAPData, &pSlot->pskContext,
            22, pLbpData, &randS, &auxNonce, &PChannelResult,
            &PChannelDataLength, &pPChannelData))
    {
        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Decoded Message4.\r\n");
        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Encoding Accepted.\r\n");
        /* Encode and send the message T2 */
        pSlot->lbpDataLength = EAP_PSK_EncodeEAPSuccess(
                sEAPIdentifier,
                memoryBufferLength,
                pMemoryBuffer
                );

        if (memcmp(randS.value, pSlot->randS.value, sizeof(randS.value)) != 0)
        {
            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] ERROR: Bad RandS received\r\n");
            return false;
        }

        sEAPIdentifier++;

        /* Encode now the LBP message */
        pSlot->lbpDataLength = LBP_EncodeAcceptedRequest(
                &lbpEUI64Address,
                pSlot->mediaType,
                pSlot->disableBackupMedium,
                pSlot->lbpDataLength,
                memoryBufferLength,
                pMemoryBuffer
                );

        return true;
    }
    else
    {
        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] ERROR: _processJoiningEAPT3.\r\n");
        return false;
    }
}

static void _initLbpMessage(LBP_SLOT *pSlot)
{
    pSlot->lbpDataLength = 0;
    memset(pSlot->lbpData, 0, sizeof(pSlot->lbpData));
}

static LBP_SLOT *_getLbpSlotByAddress(uint8_t *eui64)
{
    uint8_t idx;
    LBP_SLOT *pOutSlot = NULL;

    /* Check if the lbd is already started */
    for (idx = 0; idx < LBP_NUM_SLOTS; idx++)
    {
        if (!memcmp(sLbpSlot[idx].lbdAddress.value, eui64, ADP_ADDRESS_64BITS))
        {
            pOutSlot = &sLbpSlot[idx];
            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] _getLbpSlotByAddress --> Slot in use found: %d \r\n", idx);
            break;
        }
    }
    /* If lbd not in progress find free slot */
    if (!pOutSlot)
    {
        for (idx = 0; idx < LBP_NUM_SLOTS; idx++)
        {
            if (sLbpSlot[idx].slotState ==  LBP_STATE_WAITING_JOINNING)
            {
                pOutSlot = &sLbpSlot[idx];
                SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] _getLbpSlotByAddress --> Slot free found: %d \r\n", idx);
                break;
            }
        }
    }

    if (!pOutSlot)
    {
        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] _getLbpSlotByAddress --> Slot not found \r\n");
    }

    return pOutSlot;
}

// *****************************************************************************
// *****************************************************************************
// Section: Interface Function Definitions
// *****************************************************************************
// *****************************************************************************

void LBP_Rekey(uint16_t shortAddress, ADP_EXTENDED_ADDRESS *pEUI64Address, bool distribute)
{
    ADP_ADDRESS dstAddr;

    LBP_SLOT *pSlot = _getLbpSlotByAddress(pEUI64Address->value);

    if (pSlot)
    {
        _initLbpMessage(pSlot);
        /* DisableBackupFlag and MediaType set to 0x0 in Rekeying frames */
        pSlot->disableBackupMedium = 0;
        pSlot->mediaType = 0;
        if (distribute)
        {
            /* If re-keying in GMK distribution phase */
            /* Send ADPM-LBP.Request(EAPReq(mes1)) to each registered device */
            _processJoining0(pEUI64Address, pSlot);
            pSlot->slotState = LBP_STATE_SENT_EAP_MSG_1;
            /* Fill slot address just in case a free slot was returned */
            memcpy(pSlot->lbdAddress.value, pEUI64Address->value, ADP_ADDRESS_64BITS);
        }
        else
        {
            /* GMK activation phase */
            _process_accepted_GMK_activation(pEUI64Address, pSlot);
            pSlot->slotState = LBP_STATE_SENT_EAP_MSG_ACCEPTED;
        }

        /* Send the previously prepared message */
        dstAddr.addrSize = ADP_ADDRESS_16BITS;
        dstAddr.shortAddr = shortAddress;

        if (pSlot->pendingConfirms > 0)
        {
            pSlot->pendingTxHandle = pSlot->txHandle;
        }

        pSlot->timeout = MAC_WRP_GetMsCounter() + 1000 * sMsgTimeoutSeconds;
        pSlot->txHandle = _getNextNsduHandle();
        pSlot->txAttempts = 0;
        pSlot->pendingConfirms++;
        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] ADP_LbpRequest Called, handler: %d \r\n", pSlot->txHandle);
        ADP_LbpRequest(
                (ADP_ADDRESS const *)&dstAddr,   /* Destination address */
                pSlot->lbpDataLength,                   /* NSDU length */
                &pSlot->lbpData[0],                     /* NSDU */
                pSlot->txHandle,                        /* NSDU handle */
                sMaxHops,                               /* Max. Hops */
                true,                                   /* Discover route */
                0,                                      /* QoS */
                false);                                 /* Security enable */
    }
}

void LBP_SetRekeyPhase(bool rekeyStart)
{
    sRekey = rekeyStart;
}

void LBP_ActivateNewKey(void)
{
    uint8_t newKeyIndex;

    /* Get current key index and set the new one to the other */
    if (sCurrKeyIndex == 0)
    {
        newKeyIndex = 1;
    }
    else
    {
        newKeyIndex = 0;
    }

    /* Set GMK from Rekey GMK */
    memcpy(sCurrGMK, sRekeyGMK, sizeof(sCurrGMK));
    /* Set key table using the new index and new GMK */
    _setKeyingTable(newKeyIndex, sCurrGMK);
}

void LBP_UpdateLbpSlots(void)
{
    uint8_t idx;
    for (idx = 0; idx < LBP_NUM_SLOTS; idx++)
    {
        if (sLbpSlot[idx].slotState != LBP_STATE_WAITING_JOINNING)
        {
            if (MAC_WRP_TimeIsPast((int32_t)(sLbpSlot[idx].timeout)))
            {
                if (sLbpSlot[idx].pendingConfirms == 0)
                {
                    if (sLbpSlot[idx].txAttempts < BOOTSTRAP_MSG_MAX_RETRIES)
                    {
                        sLbpSlot[idx].txAttempts++;
                        if (sLbpSlot[idx].slotState == LBP_STATE_WAITING_EAP_MSG_2)
                        {
                            sLbpSlot[idx].slotState = LBP_STATE_SENT_EAP_MSG_1;
                            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Slot updated to LBP_STATE_SENT_EAP_MSG_1\r\n");
                            _logShowSlotStatus(&sLbpSlot[idx]);
                        }
                        else if (sLbpSlot[idx].slotState == LBP_STATE_WAITING_EAP_MSG_4)
                        {
                            sLbpSlot[idx].slotState = LBP_STATE_SENT_EAP_MSG_3;
                            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Slot updated to LBP_STATE_SENT_EAP_MSG_3\r\n");
                            _logShowSlotStatus(&sLbpSlot[idx]);
                        }

                        ADP_ADDRESS dstAddr;

                        if (sLbpSlot[idx].lbpDataLength > 0)
                        {
                            if (sLbpSlot[idx].lbaAddress == 0xFFFF)
                            {
                                dstAddr.addrSize = 8;
                                memcpy(dstAddr.extendedAddr.value, &sLbpSlot[idx].lbdAddress.value, 8);
                            }
                            else
                            {
                                dstAddr.addrSize = 2;
                                dstAddr.shortAddr = sLbpSlot[idx].lbaAddress;
                            }

                            if (sLbpSlot[idx].pendingConfirms > 0)
                            {
                                sLbpSlot[idx].pendingTxHandle = sLbpSlot[idx].txHandle;
                            }

                            sLbpSlot[idx].txHandle = _getNextNsduHandle();
                            sLbpSlot[idx].timeout = MAC_WRP_GetMsCounter() + 1000 * sMsgTimeoutSeconds;
                            sLbpSlot[idx].pendingConfirms++;

                            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Timeout detected. Re-sending MSG for slot: %d Attempt: %d \r\n",
                                    idx, sLbpSlot[idx].txAttempts);
                            _logShowSlotStatus(&sLbpSlot[idx]);
                            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] ADP_LbpRequest Called, handler: %d \r\n", sLbpSlot[idx].txHandle);
                            ADP_LbpRequest(
                                    (ADP_ADDRESS const *)&dstAddr,  /* Destination address */
                                    sLbpSlot[idx].lbpDataLength,    /* NSDU length */
                                    &sLbpSlot[idx].lbpData[0],      /* NSDU */
                                    sLbpSlot[idx].txHandle,         /* NSDU handle */
                                    sMaxHops,                       /* Max. Hops */
                                    true,                           /* Discover route */
                                    0,                              /* QoS */
                                    false);                         /* Security enable */
                        }
                    }
                    else
                    {
                        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Reset slot %d:  \r\n", idx);
                        sLbpSlot[idx].slotState = LBP_STATE_WAITING_JOINNING;
                        sLbpSlot[idx].pendingConfirms = 0;
                        sLbpSlot[idx].nonce =  0;
                        sLbpSlot[idx].timeout = 0xFFFFFFFF;
                    }
                }
                else
                {
                    /* Pending confirm and timeout, reset slot */
                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Pending Confirm and Timeout --> Reset slot %d:  \r\n", idx);
                    sLbpSlot[idx].slotState = LBP_STATE_WAITING_JOINNING;
                    sLbpSlot[idx].pendingConfirms = 0;
                    sLbpSlot[idx].nonce =  0;
                    sLbpSlot[idx].timeout = 0xFFFFFFFF;
                }
            }
        }
    }
}

static void AdpLbpConfirmCoord(ADP_LBP_CFM_PARAMS *pLbpConfirm)
{
    uint8_t idx;
    LBP_SLOT *pCurrentSlot = NULL;
    bool isAcceptedConfirm = false;

    for (idx = 0; idx < LBP_NUM_SLOTS; idx++)
    {
        LBP_SLOT *pSlot = &sLbpSlot[idx];

        if (pSlot->pendingConfirms == 1 && pLbpConfirm->nsduHandle == pSlot->txHandle && pSlot->slotState != LBP_STATE_WAITING_JOINNING)
        {
            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] AdpNotification_LbpConfirm (%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X).\r\n",
                    pSlot->lbdAddress.value[0], pSlot->lbdAddress.value[1],
                    pSlot->lbdAddress.value[2], pSlot->lbdAddress.value[3],
                    pSlot->lbdAddress.value[4], pSlot->lbdAddress.value[5],
                    pSlot->lbdAddress.value[6], pSlot->lbdAddress.value[7]);

            pCurrentSlot = pSlot;
            pSlot->pendingConfirms--;
            if (pLbpConfirm->status == G3_SUCCESS)
            {
                switch (pSlot->slotState)
                {
                case LBP_STATE_SENT_EAP_MSG_1:
                    pSlot->slotState = LBP_STATE_WAITING_EAP_MSG_2;
                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Slot updated to LBP_STATE_WAITING_EAP_MSG_2\r\n");
                    _logShowSlotStatus(pSlot);
                    break;

                case LBP_STATE_SENT_EAP_MSG_3:
                    pSlot->slotState = LBP_STATE_WAITING_EAP_MSG_4;
                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Slot updated to LBP_STATE_WAITING_EAP_MSG_4\r\n");
                    _logShowSlotStatus(pSlot);
                    break;

                case LBP_STATE_SENT_EAP_MSG_ACCEPTED:
                    pSlot->slotState = LBP_STATE_WAITING_JOINNING;
                    pSlot->nonce =  0;
                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Slot updated to LBP_STATE_WAITING_JOINNING\r\n");
                    _logShowSlotStatus(pSlot);
                    isAcceptedConfirm = true;
                    break;

                case LBP_STATE_SENT_EAP_MSG_DECLINED:
                    pSlot->slotState = LBP_STATE_WAITING_JOINNING;
                    pSlot->nonce =  0;
                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Slot updated to LBP_STATE_WAITING_JOINNING\r\n");
                    _logShowSlotStatus(pSlot);
                    break;

                default:
                    pSlot->slotState = LBP_STATE_WAITING_JOINNING;
                    pSlot->nonce =  0;
                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Slot updated to LBP_STATE_WAITING_JOINNING\r\n");
                    _logShowSlotStatus(pSlot);
                    break;
                }
            }
            else
            {
                pSlot->slotState = LBP_STATE_WAITING_JOINNING;
                pSlot->nonce =  0;
                SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Slot updated to LBP_STATE_WAITING_JOINNING\r\n");
                _logShowSlotStatus(pSlot);
            }
        }
        else if (pSlot->pendingConfirms == 2 && pLbpConfirm->nsduHandle == pSlot->pendingTxHandle)
        {
            /* Confirm received is for first request (pendingTxHandle) */
            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] AdpNotification_LbpConfirm (%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X).\r\n",
                    pSlot->lbdAddress.value[0], pSlot->lbdAddress.value[1],
                    pSlot->lbdAddress.value[2], pSlot->lbdAddress.value[3],
                    pSlot->lbdAddress.value[4], pSlot->lbdAddress.value[5],
                    pSlot->lbdAddress.value[6], pSlot->lbdAddress.value[7]);
            pSlot->pendingConfirms--;
            _logShowSlotStatus(pSlot);
            pCurrentSlot = pSlot;
        }
        else if (pSlot->pendingConfirms == 2 && pLbpConfirm->nsduHandle == pSlot->txHandle)
        {
            /* Confirm received is for last request (txHandle) */
            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] AdpNotification_LbpConfirm (%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X).\r\n",
                    pSlot->lbdAddress.value[0], pSlot->lbdAddress.value[1],
                    pSlot->lbdAddress.value[2], pSlot->lbdAddress.value[3],
                    pSlot->lbdAddress.value[4], pSlot->lbdAddress.value[5],
                    pSlot->lbdAddress.value[6], pSlot->lbdAddress.value[7]);
            pSlot->pendingConfirms--;
            pSlot->txHandle = pSlot->pendingTxHandle;
            _logShowSlotStatus(pSlot);
            pCurrentSlot = pSlot;
        }
    }

    if (!pCurrentSlot)
    {
        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] AdpNotification_LbpConfirm from unkown node, status: %d  handler: %d \r\n",
                pLbpConfirm->status, pLbpConfirm->nsduHandle);
        isAcceptedConfirm = false;
    }
    else
    {
        pCurrentSlot->timeout = MAC_WRP_GetMsCounter() + 1000 * sMsgTimeoutSeconds;
    }

    if (pLbpConfirm->status == G3_SUCCESS && isAcceptedConfirm)
    {
        /* Upper layer indications */
        if (sLbpNotifications.joinCompleteIndication != NULL)
        {
            sLbpNotifications.joinCompleteIndication(pCurrentSlot->lbdAddress.value, pCurrentSlot->assignedShortAddress);
        }
    }
}

static void AdpLbpIndicationCoord(ADP_LBP_IND_PARAMS *pLbpIndication)
{
    uint16_t origAddress;
    uint8_t msgType;
    uint8_t *pLbpData;
    uint16_t lbpDataLength;
    ADP_ADDRESS dstAddr;
    /* Embedded EAP message */
    uint8_t code = 0;
    uint8_t identifier = 0;
    uint8_t TSubfield = 0;
    uint16_t EAPDataLength = 0;
    uint8_t *pEAPData = 0L;
    ADP_EXTENDED_ADDRESS currentLbdAddress;
    LBP_SLOT *pSlot;
    uint8_t mediaType;
    uint8_t disableBackupMedium;

    if (pLbpIndication->srcAddr.addrSize == ADP_ADDRESS_64BITS)
    {
        /* When directly communicating with the LBD(using extended addressing) this field is set to 0xFFFF. */
        origAddress = 0xFFFF;
        /* Check frame coming from LBD is not secured */
        if (pLbpIndication->securityLevel != MAC_WRP_SECURITY_LEVEL_NONE)
        {
            return;
        }
    }
    else
    {
        /* When frame comes from Joined device or its LBA, srcAddr contains the Originator Address */
        origAddress = pLbpIndication->srcAddr.shortAddr;
    }

    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "LBPIndication_Notify(): LbaAddr: 0x%04X NsduLength: %hu.\r\n",origAddress, pLbpIndication->nsduLength);

    msgType = ((pLbpIndication->pNsdu[0] & 0xF0) >> 4);
    mediaType = ((pLbpIndication->pNsdu[0] & 0x08) >> 3);
    disableBackupMedium = ((pLbpIndication->pNsdu[0] & 0x04) >> 2);
    pLbpData = (uint8_t *)&pLbpIndication->pNsdu[10];
    lbpDataLength = pLbpIndication->nsduLength - 10;

    memcpy(currentLbdAddress.value, &pLbpIndication->pNsdu[2], ADP_ADDRESS_64BITS);

    /* Check differet conditions if frame comes directly from LBD or from LBA */
    if (origAddress == 0xFFFF)
    {
        /* Frame from LBD */
        /* Originator address must be equal to the one in LBP frame */
        if (memcmp(&currentLbdAddress, &pLbpIndication->srcAddr.extendedAddr, sizeof(ADP_EXTENDED_ADDRESS)) != 0)
        {
            return;
        }
        /* Frame type must be equal to JOINING */
        if (msgType != LBP_JOINING)
        {
            return;
        }
    }
    else
    {
        /* Frame from LBA */
        /* Src address must be between 0x0000 and 0x7FFF */
        if (origAddress > 0x7FFF)
        {
            return;
        }
        /* Security level already checked for frames between 16-bit addresses */
    }

    pSlot = _getLbpSlotByAddress(currentLbdAddress.value);
    if (pSlot != NULL)
    {
        _initLbpMessage(pSlot);
    }

    if (msgType == LBP_JOINING)
    {
        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Processing incoming LBP_JOINING... LBD Address: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X \r\n",
                currentLbdAddress.value[0], currentLbdAddress.value[1],
                currentLbdAddress.value[2], currentLbdAddress.value[3],
                currentLbdAddress.value[4], currentLbdAddress.value[5],
                currentLbdAddress.value[6], currentLbdAddress.value[7]);

        /* Check the bootstrapping data in order to see the progress of the joining process */
        if (lbpDataLength == 0)
        {
            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] First joining request.\r\n");
            /* This is the first joining request. Responded only if no other BS was in progress. */
            if (pSlot)
            {
                if (pSlot->slotState == LBP_STATE_WAITING_JOINNING)
                {
                    /* Set Media Type and Disable backup flag from incoming LBP frame on Slot */
                    pSlot->mediaType = mediaType;
                    pSlot->disableBackupMedium = disableBackupMedium;
                    /* Store information to be used in Address Assign function */
                    pSlot->lbaAddress = origAddress;
                    memcpy(pSlot->lbdAddress.value, currentLbdAddress.value,
                            sizeof(currentLbdAddress.value));
                    /* Check with upper layer if the joining device is accepted */
                    if (sLbpNotifications.joinRequestIndication != NULL) {
                        sLbpNotifications.joinRequestIndication(currentLbdAddress.value);
                    }
                    else {
                        /* If there is no configured callback, assign addresses sequentially */
                        sInternalAssignedAddress++;
                        LBP_ShortAddressAssign(currentLbdAddress.value, sInternalAssignedAddress);
                    }
                    /* Exit function, response will be sent on Address Assign function */
                    return;
                } else {
                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Repeated message processing Joining, silently ignored \r\n");
                }
            } else {
                SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] No slots available to process the request. Ignored.\r\n");
            }
        } else {
            /* Check if the message comes from a device currently under BS */
            if (pSlot) {
                /* Set Media Type and Disable backup flag from incoming LBP frame on Slot */
                pSlot->mediaType = mediaType;
                pSlot->disableBackupMedium = disableBackupMedium;
                /* check the type of the bootstrap data */
                if ((pLbpData[0] & 0x01) != 0x01)
                {
                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Successive joining request.\r\n");
                    if (EAP_PSK_DecodeMessage(
                            lbpDataLength,
                            pLbpData,
                            &code,
                            &identifier,
                            &TSubfield,
                            &EAPDataLength,
                            &pEAPData))
                    {
                        if (code == EAP_RESPONSE)
                        {
                            if (TSubfield == EAP_PSK_T1 && (pSlot->slotState == LBP_STATE_WAITING_EAP_MSG_2 ||
                                    pSlot->slotState == LBP_STATE_SENT_EAP_MSG_1))
                            {
                                if (_processJoiningEAPT1(currentLbdAddress, EAPDataLength, pEAPData, pSlot) != 1)
                                {
                                    /* Abort current BS process */
                                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] LBP error processing EAP T1.\r\n");
                                    pSlot->slotState = LBP_STATE_WAITING_JOINNING;
                                    pSlot->pendingConfirms = 0;
                                    pSlot->nonce =  0;

                                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Slot updated to LBP_STATE_WAITING_JOINNING\r\n");
                                }
                                else
                                {
                                    pSlot->slotState = LBP_STATE_SENT_EAP_MSG_3;
                                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Slot updated to LBP_STATE_SENT_EAP_MSG_3\r\n");
                                }
                            }
                            else if (TSubfield == EAP_PSK_T3 &&
                                    (pSlot->slotState == LBP_STATE_WAITING_EAP_MSG_4 || pSlot->slotState ==
                                    LBP_STATE_SENT_EAP_MSG_3))
                            {
                                if (_processJoiningEAPT3(currentLbdAddress, pLbpData, EAPDataLength, pEAPData,
                                        pSlot))
                                {
                                    pSlot->slotState = LBP_STATE_SENT_EAP_MSG_ACCEPTED;
                                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Slot updated to LBP_STATE_SENT_EAP_MSG_ACCEPTED\r\n");
                                }
                                else
                                {
                                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] LBP error processing EAP T3.\r\n");
                                    pSlot->slotState = LBP_STATE_WAITING_JOINNING;
                                    pSlot->pendingConfirms = 0;
                                    pSlot->nonce =  0;
                                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Slot updated to LBP_STATE_WAITING_JOINNING\r\n");
                                }
                            }
                            else
                            {
                                /* Abort current BS process */
                                SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] LBP protocol error.\r\n");
                                pSlot->slotState = LBP_STATE_WAITING_JOINNING;
                                pSlot->pendingConfirms = 0;
                                pSlot->nonce =  0;
                                SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Slot updated to LBP_STATE_WAITING_JOINNING\r\n");
                            }
                        }
                    }
                    else
                    {
                        /* Abort current BS process */
                        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] ERROR decoding message.\r\n");
                        pSlot->slotState = LBP_STATE_WAITING_JOINNING;
                        pSlot->pendingConfirms = 0;
                        pSlot->nonce =  0;
                        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Slot updated to LBP_STATE_WAITING_JOINNING\r\n");
                        _logShowSlotStatus(pSlot);
                    }
                }
            }
            else
            {
                SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Concurrent successive joining received. Ignored.\r\n");
            }
        }
    }
    else if (msgType == LBP_KICK_FROM_LBD)
    {
        /* Upper layer LEAVE callback will be invoked later */
    }
    else
    {
        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] ERROR: unknown incoming message.\r\n");
    }

    if (pSlot != NULL)
    {
        if (pSlot->lbpDataLength > 0)
        {
            if (origAddress == 0xFFFF)
            {
                dstAddr.addrSize = ADP_ADDRESS_64BITS;
                memcpy(dstAddr.extendedAddr.value, pSlot->lbdAddress.value, ADP_ADDRESS_64BITS);
            }
            else
            {
                dstAddr.addrSize = ADP_ADDRESS_16BITS;
                dstAddr.shortAddr = origAddress;
            }

            if (pSlot->pendingConfirms > 0) {
                pSlot->pendingTxHandle = pSlot->txHandle;
            }

            pSlot->txHandle = _getNextNsduHandle();
            pSlot->timeout = MAC_WRP_GetMsCounter() + 1000 * sMsgTimeoutSeconds;
            pSlot->txAttempts = 0;
            pSlot->pendingConfirms++;

            _logShowSlotStatus(pSlot);
            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] ADP_LbpRequest Called, handler: %d \r\n", pSlot->txHandle);
            ADP_LbpRequest(
                    (ADP_ADDRESS const *)&dstAddr,  /* Destination address */
                    pSlot->lbpDataLength,           /* NSDU length */
                    &pSlot->lbpData[0],             /* NSDU */
                    pSlot->txHandle,                /* NSDU handle */
                    sMaxHops,                       /* Max. Hops */
                    true,                           /* Discover route */
                    0,                              /* QoS */
                    false);                         /* Security enable */
        }
    }

    /* Upper layer indications */
    if (msgType == LBP_KICK_FROM_LBD)
    {
        if (sLbpNotifications.leaveIndication != NULL) {
            sLbpNotifications.leaveIndication(origAddress);
        }
    }
}

void LBP_InitCoord(bool aribBand)
{
    ADP_NOTIFICATIONS_TO_LBP notifications;

    notifications.lbpConfirm = AdpLbpConfirmCoord;
    notifications.lbpIndication = AdpLbpIndicationCoord;

    ADP_SetNotificationsToLbp(&notifications);

    sRekey = false;

    sAribBand = aribBand;
    if (aribBand)
    {
        sIdS.size = LBP_NETWORK_ACCESS_ID_SIZE_S_ARIB;
        memcpy(sIdS.value, sIdSArib.value, LBP_NETWORK_ACCESS_ID_SIZE_S_ARIB);
    }
    else
    {
        sIdS.size = LBP_NETWORK_ACCESS_ID_SIZE_S_CENELEC_FCC;
        memcpy(sIdS.value, sIdSCenFcc.value, LBP_NETWORK_ACCESS_ID_SIZE_S_CENELEC_FCC);
    }

    _setKeyingTable(INITIAL_KEY_INDEX, sCurrGMK);

    _initLbpSlots();

    sMaxHops = _GetAdpMaxHops();

    _SetDeviceTypeCoord();
}

void LBP_SetNotificationsCoord(LBP_NOTIFICATIONS_COORD *pNotifications)
{
    if (pNotifications != NULL)
    {
        sLbpNotifications = *pNotifications;
    }
}

bool LBP_KickDevice(uint16_t shortAddress, ADP_EXTENDED_ADDRESS *pEUI64Address)
{
    ADP_ADDRESS dstAddr;
    uint8_t lbpData[10];
    uint16_t lbpDataLength;

    /* Send KICK to the device */
    dstAddr.addrSize = ADP_ADDRESS_16BITS;
    dstAddr.shortAddr = shortAddress;

    lbpDataLength = LBP_EncodeKickToLBD(pEUI64Address, sizeof(lbpData), lbpData);

    /* If message was properly encoded, send it */
    if (lbpDataLength)
    {
        ADP_LbpRequest(
                (ADP_ADDRESS const *)&dstAddr,  /* Destination address */
                lbpDataLength,                  /* NSDU length */
                lbpData,                        /* NSDU */
                _getNextNsduHandle(),           /* NSDU handle */
                sMaxHops,                       /* Max. Hops */
                true,                           /* Discover route */
                0,                              /* QoS */
                false);                         /* Security enable */

        return true;
    }
    else
    {
        return false;
    }
}

void LBP_SetParamCoord(uint32_t attributeId, uint16_t attributeIndex,
        uint8_t attributeLen, const uint8_t *pAttributeValue,
        LBP_SET_PARAM_CONFIRM *pSetConfirm)
{
    pSetConfirm->attributeId = attributeId;
    pSetConfirm->attributeIndex = attributeIndex;
    pSetConfirm->status = LBP_STATUS_UNSUPPORTED_PARAMETER;

    switch (attributeId)
    {
    case LBP_IB_IDS:
        if ((attributeLen == LBP_NETWORK_ACCESS_ID_SIZE_S_ARIB) ||
            (attributeLen == LBP_NETWORK_ACCESS_ID_SIZE_S_CENELEC_FCC))
        {
            sIdS.size = attributeLen;
            memcpy(sIdS.value, pAttributeValue, attributeLen);
            pSetConfirm->status = LBP_STATUS_OK;
        }
        else
        {
            pSetConfirm->status = LBP_STATUS_INVALID_LENGTH;
        }

        break;

    case LBP_IB_PSK:
        if (attributeLen == 16)
        {
            memcpy(sEapPskKey.value, pAttributeValue, sizeof(sEapPskKey.value));
            pSetConfirm->status = LBP_STATUS_OK;
        }
        else
        {
            /* Wrong parameter size */
            pSetConfirm->status = LBP_STATUS_INVALID_LENGTH;
        }

        break;

    case LBP_IB_GMK:
        if (attributeLen == 16)
        {
            /* Set GMK on LBP module */
            memcpy(sCurrGMK, pAttributeValue, sizeof(sCurrGMK));
            /* Set key table on G3 stack using the new index and new GMK */
            _setKeyingTable(attributeIndex, (uint8_t *)pAttributeValue);
            pSetConfirm->status = LBP_STATUS_OK;
        }
        else
        {
            /* Wrong parameter size */
            pSetConfirm->status = LBP_STATUS_INVALID_LENGTH;
        }

        break;

    case LBP_IB_REKEY_GMK:
        if (attributeLen == 16)
        {
            memcpy(sRekeyGMK, pAttributeValue, sizeof(sRekeyGMK));
            pSetConfirm->status = LBP_STATUS_OK;
        }
        else
        {
            /* Wrong parameter size */
            pSetConfirm->status = LBP_STATUS_INVALID_LENGTH;
        }

        break;

    case LBP_IB_MSG_TIMEOUT:
        if (attributeLen == 2)
        {
            sMsgTimeoutSeconds = ((pAttributeValue[1] << 8) | pAttributeValue[0]);
            pSetConfirm->status = LBP_STATUS_OK;
        }
        else
        {
            /* Wrong parameter size */
            pSetConfirm->status = LBP_STATUS_INVALID_LENGTH;
        }

        break;

    default:
        /* Unknown LBP parameter */
        break;
    }
}

void LBP_ShortAddressAssign(uint8_t *pExtAddress, uint16_t assignedAddress)
{
    LBP_SLOT *pSlot;
    ADP_ADDRESS dstAddr;
    
    /* Get slot from extended address*/
    pSlot = _getLbpSlotByAddress(pExtAddress);

    if (pSlot != NULL)
    {
        if (assignedAddress == 0xFFFF)
        {
            // Device rejected
            pSlot->lbpDataLength = LBP_EncodeDecline(&pSlot->lbdAddress, pSlot->mediaType,
                pSlot->disableBackupMedium, sEAPIdentifier, sizeof(pSlot->lbpData), &pSlot->lbpData[0]);
            pSlot->slotState = LBP_STATE_SENT_EAP_MSG_DECLINED;
            sEAPIdentifier++;
            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Slot updated to LBP_STATE_SENT_EAP_MSG_DECLINED\r\n");
        }
        else
        {
            pSlot->assignedShortAddress = assignedAddress;
            _processJoining0(&pSlot->lbdAddress, pSlot);
            pSlot->slotState = LBP_STATE_SENT_EAP_MSG_1;
            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Slot updated to LBP_STATE_SENT_EAP_MSG_1\r\n");
        }
        
        if (pSlot->lbpDataLength > 0)
        {
            if (pSlot->lbaAddress == 0xFFFF)
            {
                dstAddr.addrSize = ADP_ADDRESS_64BITS;
                memcpy(dstAddr.extendedAddr.value, pSlot->lbdAddress.value, ADP_ADDRESS_64BITS);
            }
            else
            {
                dstAddr.addrSize = ADP_ADDRESS_16BITS;
                dstAddr.shortAddr = pSlot->lbaAddress;
            }

            if (pSlot->pendingConfirms > 0)
            {
                pSlot->pendingTxHandle = pSlot->txHandle;
            }

            pSlot->txHandle = _getNextNsduHandle();
            pSlot->timeout = MAC_WRP_GetMsCounter() + 1000 * sMsgTimeoutSeconds;
            pSlot->txAttempts = 0;
            pSlot->pendingConfirms++;

            _logShowSlotStatus(pSlot);
            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] ADP_LbpRequest Called, handler: %d \r\n", pSlot->txHandle);
            ADP_LbpRequest(
                    (ADP_ADDRESS const *)&dstAddr,  /* Destination address */
                    pSlot->lbpDataLength,           /* NSDU length */
                    &pSlot->lbpData[0],             /* NSDU */
                    pSlot->txHandle,                /* NSDU handle */
                    sMaxHops,                       /* Max. Hops */
                    true,                           /* Discover route */
                    0,                              /* QoS */
                    false);                         /* Security enable */
        }
    }
}
