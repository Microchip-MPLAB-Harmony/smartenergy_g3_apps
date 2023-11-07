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

#define LBP_NUM_SLOTS               1U
#define BOOTSTRAP_MSG_MAX_RETRIES   1U
#define INITIAL_KEY_INDEX           0U

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

#pragma pack(push,2)

typedef struct
{
    uint32_t timeout;
    uint32_t nonce;
    uint16_t lbaAddress;
    uint16_t assignedShortAddress;
    uint16_t lbpDataLength;
    uint8_t txHandle;
    uint8_t txAttempts;
    uint8_t pendingConfirms;
    uint8_t pendingTxHandle;
    uint8_t mediaType;
    uint8_t disableBackupMedium;
    EAP_PSK_RAND randS;
    ADP_EXTENDED_ADDRESS lbdAddress;
    LBP_SLOT_STATE slotState;
    EAP_PSK_CONTEXT pskContext;
    uint8_t lbpData[ADP_LBP_MAX_NSDU_LENGTH];

} LBP_SLOT;

#pragma pack(pop)

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Variables
// *****************************************************************************
// *****************************************************************************

/* Default value for Current GMK
 * This parameter is intended to be set from Application through LBP_IB_GMK IB */
static uint8_t sCurrGMK[16]  =
    {0xAF, 0x4D, 0x6D, 0xCC, 0xF1, 0x4D, 0xE7, 0xC1, 0xC4, 0x23, 0x5E, 0x6F, 0xEF, 0x6C, 0x15, 0x1F};

/* Default value for Rekeying GMK
 * This parameter is intended to be set from Application through LBP_IB_REKEY_GMK IB */
static uint8_t sRekeyGMK[16] =
    {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16};

/* Default value for PSK
 * This parameter is intended to be set from Application through LBP_IB_PSK IB */
static EAP_PSK_KEY sEapPskKey = {
    {0xAB, 0x10, 0x34, 0x11, 0x45, 0x11, 0x1B, 0xC3, 0xC1, 0x2D, 0xE8, 0xFF, 0x11, 0x14, 0x22, 0x04}
};

/* Default value for ID_S
 * This parameter is intended to be set from Application through LBP_IB_IDS IB */
static EAP_PSK_NETWORK_ACCESS_ID_S sIdS;

/* Default value for waiting an LBP response, in seconds
 * This parameter is intended to be set from Application through LBP_IB_MSG_TIMEOUT IB */
static uint16_t sMsgTimeoutSeconds = 300;

static uint8_t sNsduHandle = 0;
static uint8_t sMaxHops = 0;
static uint8_t sCurrKeyIndex = 0;

static const EAP_PSK_NETWORK_ACCESS_ID_S sIdSArib =
    {LBP_NETWORK_ACCESS_ID_SIZE_S_ARIB,
    {0x53, 0x4D, 0xAD, 0xB2, 0xC4, 0xD5, 0xE6, 0xFA, 0x53, 0x4D, 0xAD, 0xB2, 0xC4, 0xD5, 0xE6, 0xFA,
    0x53, 0x4D, 0xAD, 0xB2, 0xC4, 0xD5, 0xE6, 0xFA, 0x53, 0x4D, 0xAD, 0xB2, 0xC4, 0xD5, 0xE6, 0xFA,
    0x53, 0x4D}};

static const EAP_PSK_NETWORK_ACCESS_ID_S sIdSCenFcc =
    {LBP_NETWORK_ACCESS_ID_SIZE_S_CENELEC_FCC,
    {0x81, 0x72, 0x63, 0x54, 0x45, 0x36, 0x27, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00}};

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

static uint8_t lLBP_GetAdpMaxHops(void)
{
    ADP_GET_CFM_PARAMS getConfirm;
    ADP_GetRequestSync((uint32_t)ADP_IB_MAX_HOPS, 0U, &getConfirm);
    return getConfirm.attributeValue[0];
}

static void lLBP_SetDeviceTypeCoord(void)
{
    ADP_SET_CFM_PARAMS setConfirm;
    uint8_t devType = 1U;

    // Set on ADP
    ADP_SetRequestSync((uint32_t)ADP_IB_DEVICE_TYPE, 0U, 1U, &devType, &setConfirm);
}

static void lLBP_LogShowSlotStatus(LBP_SLOT *pSlot)
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

static void  lLBP_InitLbpSlots(void)
{
    uint8_t idx;

    for (idx = 0U; idx < LBP_NUM_SLOTS; idx++)
    {
        sLbpSlot[idx].slotState =  LBP_STATE_WAITING_JOINNING;
        sLbpSlot[idx].pendingConfirms = 0U;
        sLbpSlot[idx].txHandle =  0xffU;
        sLbpSlot[idx].nonce =  0U;

        (void) memset(sLbpSlot[idx].lbdAddress.value, 0xff, sizeof(sLbpSlot[idx].lbdAddress.value));
    }
}

static uint8_t lLBP_GetNextNsduHandle(void)
{
    return sNsduHandle++;
}

static void lLBP_SetKeyingTable(uint8_t keyIndex, uint8_t *key)
{
    ADP_SET_CFM_PARAMS setConfirm;

    /* Set on MAC and ADP */
    ADP_MacSetRequestSync((uint32_t) MAC_WRP_PIB_KEY_TABLE, keyIndex, 16U, key, &setConfirm);
    ADP_SetRequestSync((uint32_t) ADP_IB_ACTIVE_KEY_INDEX, 0U, 1U, &keyIndex, &setConfirm);
    sCurrKeyIndex = keyIndex;
}

static void lLBP_ProcessAcceptedGMKactivation(ADP_EXTENDED_ADDRESS *pLBPEUI64Address, LBP_SLOT *pSlot)
{
    unsigned char *pMemoryBuffer = &pSlot->lbpData[0];
    uint16_t memoryBufferLength = (uint16_t)sizeof(pSlot->lbpData);
    uint8_t pdata[3];
    uint8_t newKeyIndex;

    /* Get current key index and set the new one to the other */
    if (sCurrKeyIndex == 0U)
    {
        newKeyIndex = 1U;
    }
    else
    {
        newKeyIndex = 0U;
    }

    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Accepted(GMK-activation).\r\n");

    /* Prepare the protected data carring the key and short addr */
    pdata[0] = LBP_CONF_PARAM_GMK_ACTIVATION;
    pdata[1] = 0x01U;
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
}

static void lLBP_ProcessJoining0(ADP_EXTENDED_ADDRESS *pLBPEUI64Address, LBP_SLOT *pSlot)
{
    uint8_t *pMemoryBuffer = &pSlot->lbpData[0];
    uint16_t memoryBufferLength = (uint16_t)sizeof(pSlot->lbpData);

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

static bool lLBP_ProcessJoiningEAPT1(ADP_EXTENDED_ADDRESS lbpEUI64Address, uint16_t EAPDataLength,
        uint8_t *pEAPData, LBP_SLOT *pSlot)
{
    EAP_PSK_RAND randS;
    EAP_PSK_RAND randP;
    uint8_t *pMemoryBuffer = &pSlot->lbpData[0];
    uint16_t memoryBufferLength = (uint16_t)sizeof(pSlot->lbpData);
    uint8_t pdata[50];
    uint16_t PDataLen = 0U;
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
        pdata[PDataLen++] = 0x02U; /* ext field */

        if (!sRekey)
        {
            pdata[PDataLen++] = LBP_CONF_PARAM_SHORT_ADDR;
            pdata[PDataLen++] = 2U;
            pdata[PDataLen++] = (uint8_t)(shortAddr >> 8);
            pdata[PDataLen++] = (uint8_t)shortAddr;

            pdata[PDataLen++] = LBP_CONF_PARAM_GMK;
            pdata[PDataLen++] = 17U;
            pdata[PDataLen++] = sCurrKeyIndex; /* key id */
            (void) memcpy(&pdata[PDataLen], sCurrGMK, 16U); /* key */
            PDataLen += 16U;

            pdata[PDataLen++] = LBP_CONF_PARAM_GMK_ACTIVATION;
            pdata[PDataLen++] = 1U;
            pdata[PDataLen++] = sCurrKeyIndex; /* key id */
        }
        else
        {
            /* Get current key index and set the new one to the other */
            if (sCurrKeyIndex == 0U)
            {
                newKeyIndex = 1U;
            }
            else
            {
                newKeyIndex = 0U;
            }

            pdata[PDataLen++] = LBP_CONF_PARAM_GMK;
            pdata[PDataLen++] = 17U;
            pdata[PDataLen++] = newKeyIndex; /* key id */
            (void) memcpy(&pdata[PDataLen], sRekeyGMK, 16U); /* key */
            PDataLen += 16U;
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
        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] ERROR: lLBP_ProcessJoiningEAPT1.\r\n");
        return false;
    }
}

static bool lLBP_ProcessJoiningEAPT3(ADP_EXTENDED_ADDRESS lbpEUI64Address, uint8_t *pLbpData, uint16_t EAPDataLength,
        uint8_t *pEAPData, LBP_SLOT *pSlot)
{
    EAP_PSK_RAND randS;
    uint8_t PChannelResult = 0U;
    uint32_t auxNonce = 0U;
    uint16_t PChannelDataLength = 0U;
    uint8_t *pPChannelData = NULL;
    uint8_t *pMemoryBuffer = &pSlot->lbpData[0];
    uint16_t memoryBufferLength = (uint16_t)sizeof(pSlot->lbpData);

    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Process Joining EAP T3.\r\n");

    if (EAP_PSK_DecodeMessage4(EAPDataLength, pEAPData, &pSlot->pskContext,
            22U, pLbpData, &randS, &auxNonce, &PChannelResult,
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
        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] ERROR: lLBP_ProcessJoiningEAPT3.\r\n");
        return false;
    }
}

static void lLBP_InitLbpMessage(LBP_SLOT *pSlot)
{
    pSlot->lbpDataLength = 0U;
    (void) memset(pSlot->lbpData, 0, sizeof(pSlot->lbpData));
}

static LBP_SLOT *lLBP_GetLbpSlotByAddress(uint8_t *eui64)
{
    uint8_t idx;
    LBP_SLOT *pOutSlot = NULL;

    /* Check if the lbd is already started */
    for (idx = 0U; idx < LBP_NUM_SLOTS; idx++)
    {
        if (memcmp(sLbpSlot[idx].lbdAddress.value, eui64, ADP_ADDRESS_64BITS) == 0)
        {
            pOutSlot = &sLbpSlot[idx];
            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] lLBP_GetLbpSlotByAddress --> Slot in use found: %d \r\n", idx);
            break;
        }
    }
    /* If lbd not in progress find free slot */
    if (pOutSlot == NULL)
    {
        for (idx = 0U; idx < LBP_NUM_SLOTS; idx++)
        {
            if (sLbpSlot[idx].slotState ==  LBP_STATE_WAITING_JOINNING)
            {
                pOutSlot = &sLbpSlot[idx];
                SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] lLBP_GetLbpSlotByAddress --> Slot free found: %d \r\n", idx);
                break;
            }
        }
    }

    if (pOutSlot == NULL)
    {
        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] lLBP_GetLbpSlotByAddress --> Slot not found \r\n");
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

    LBP_SLOT *pSlot = lLBP_GetLbpSlotByAddress(pEUI64Address->value);

    if (pSlot != NULL)
    {
        lLBP_InitLbpMessage(pSlot);
        /* DisableBackupFlag and MediaType set to 0x0 in Rekeying frames */
        pSlot->disableBackupMedium = 0U;
        pSlot->mediaType = 0U;
        if (distribute)
        {
            /* If re-keying in GMK distribution phase */
            /* Send ADPM-LBP.Request(EAPReq(mes1)) to each registered device */
            lLBP_ProcessJoining0(pEUI64Address, pSlot);
            pSlot->slotState = LBP_STATE_SENT_EAP_MSG_1;
            /* Fill slot address just in case a free slot was returned */
            (void) memcpy(pSlot->lbdAddress.value, pEUI64Address->value, ADP_ADDRESS_64BITS);
        }
        else
        {
            /* GMK activation phase */
            lLBP_ProcessAcceptedGMKactivation(pEUI64Address, pSlot);
            pSlot->slotState = LBP_STATE_SENT_EAP_MSG_ACCEPTED;
        }

        /* Send the previously prepared message */
        dstAddr.addrSize = ADP_ADDRESS_16BITS;
        dstAddr.shortAddr = shortAddress;

        if (pSlot->pendingConfirms > 0U)
        {
            pSlot->pendingTxHandle = pSlot->txHandle;
        }

        pSlot->timeout = MAC_WRP_GetMsCounter() + (1000U * (uint32_t)sMsgTimeoutSeconds);
        pSlot->txHandle = lLBP_GetNextNsduHandle();
        pSlot->txAttempts = 0U;
        pSlot->pendingConfirms++;
        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] ADP_LbpRequest Called, handler: %d \r\n", pSlot->txHandle);
        ADP_LbpRequest(
                (ADP_ADDRESS const *)&dstAddr,   /* Destination address */
                pSlot->lbpDataLength,                   /* NSDU length */
                &pSlot->lbpData[0],                     /* NSDU */
                pSlot->txHandle,                        /* NSDU handle */
                sMaxHops,                               /* Max. Hops */
                true,                                   /* Discover route */
                0U,                                      /* QoS */
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
    if (sCurrKeyIndex == 0U)
    {
        newKeyIndex = 1U;
    }
    else
    {
        newKeyIndex = 0U;
    }

    /* Set GMK from Rekey GMK */
    (void) memcpy(sCurrGMK, sRekeyGMK, sizeof(sCurrGMK));
    /* Set key table using the new index and new GMK */
    lLBP_SetKeyingTable(newKeyIndex, sCurrGMK);
}

void LBP_UpdateLbpSlots(void)
{
    uint8_t idx;
    for (idx = 0U; idx < LBP_NUM_SLOTS; idx++)
    {
        if (sLbpSlot[idx].slotState != LBP_STATE_WAITING_JOINNING)
        {
            if (MAC_WRP_TimeIsPast((int32_t)(sLbpSlot[idx].timeout)))
            {
                if (sLbpSlot[idx].pendingConfirms == 0U)
                {
                    if (sLbpSlot[idx].txAttempts < BOOTSTRAP_MSG_MAX_RETRIES)
                    {
                        sLbpSlot[idx].txAttempts++;
                        if (sLbpSlot[idx].slotState == LBP_STATE_WAITING_EAP_MSG_2)
                        {
                            sLbpSlot[idx].slotState = LBP_STATE_SENT_EAP_MSG_1;
                            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Slot updated to LBP_STATE_SENT_EAP_MSG_1\r\n");
                            lLBP_LogShowSlotStatus(&sLbpSlot[idx]);
                        }
                        else{
                            if (sLbpSlot[idx].slotState == LBP_STATE_WAITING_EAP_MSG_4)
                            {
                                sLbpSlot[idx].slotState = LBP_STATE_SENT_EAP_MSG_3;
                                SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Slot updated to LBP_STATE_SENT_EAP_MSG_3\r\n");
                                lLBP_LogShowSlotStatus(&sLbpSlot[idx]);
                            }
                        }

                        ADP_ADDRESS dstAddr;

                        if (sLbpSlot[idx].lbpDataLength > 0U)
                        {
                            if (sLbpSlot[idx].lbaAddress == 0xFFFFU)
                            {
                                dstAddr.addrSize = 8U;
                                (void) memcpy(dstAddr.extendedAddr.value, &sLbpSlot[idx].lbdAddress.value, 8);
                            }
                            else
                            {
                                dstAddr.addrSize = 2U;
                                dstAddr.shortAddr = sLbpSlot[idx].lbaAddress;
                            }

                            if (sLbpSlot[idx].pendingConfirms > 0U)
                            {
                                sLbpSlot[idx].pendingTxHandle = sLbpSlot[idx].txHandle;
                            }

                            sLbpSlot[idx].txHandle = lLBP_GetNextNsduHandle();
                            sLbpSlot[idx].timeout = MAC_WRP_GetMsCounter() + (1000U * (uint32_t)sMsgTimeoutSeconds);
                            sLbpSlot[idx].pendingConfirms++;

                            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Timeout detected. Re-sending MSG for slot: %d Attempt: %d \r\n",
                                    idx, sLbpSlot[idx].txAttempts);
                            lLBP_LogShowSlotStatus(&sLbpSlot[idx]);
                            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] ADP_LbpRequest Called, handler: %d \r\n", sLbpSlot[idx].txHandle);
                            ADP_LbpRequest(
                                    (ADP_ADDRESS const *)&dstAddr,  /* Destination address */
                                    sLbpSlot[idx].lbpDataLength,    /* NSDU length */
                                    &sLbpSlot[idx].lbpData[0],      /* NSDU */
                                    sLbpSlot[idx].txHandle,         /* NSDU handle */
                                    sMaxHops,                       /* Max. Hops */
                                    true,                           /* Discover route */
                                    0U,                              /* QoS */
                                    false);                         /* Security enable */
                        }
                    }
                    else
                    {
                        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Reset slot %d:  \r\n", idx);
                        sLbpSlot[idx].slotState = LBP_STATE_WAITING_JOINNING;
                        sLbpSlot[idx].pendingConfirms = 0U;
                        sLbpSlot[idx].nonce =  0U;
                        sLbpSlot[idx].timeout = 0xFFFFFFFFUL;
                    }
                }
                else
                {
                    /* Pending confirm and timeout, reset slot */
                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Pending Confirm and Timeout --> Reset slot %d:  \r\n", idx);
                    sLbpSlot[idx].slotState = LBP_STATE_WAITING_JOINNING;
                    sLbpSlot[idx].pendingConfirms = 0U;
                    sLbpSlot[idx].nonce =  0U;
                    sLbpSlot[idx].timeout = 0xFFFFFFFFUL;
                }
            }
        }
    }
}

static void lLBP_AdpLbpConfirmCoord(ADP_LBP_CFM_PARAMS *pLbpConfirm)
{
    uint8_t idx;
    LBP_SLOT *pCurrentSlot = NULL;
    bool isAcceptedConfirm = false;

    for (idx = 0U; idx < LBP_NUM_SLOTS; idx++)
    {
        LBP_SLOT *pSlot = &sLbpSlot[idx];

        if ((pSlot->pendingConfirms == 1U) && (pLbpConfirm->nsduHandle == pSlot->txHandle) && (pSlot->slotState != LBP_STATE_WAITING_JOINNING))
        {
            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] AdpNotification_LbpConfirm (%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X).\r\n",
                    pSlot->lbdAddress.value[0], pSlot->lbdAddress.value[1],
                    pSlot->lbdAddress.value[2], pSlot->lbdAddress.value[3],
                    pSlot->lbdAddress.value[4], pSlot->lbdAddress.value[5],
                    pSlot->lbdAddress.value[6], pSlot->lbdAddress.value[7]);

            pCurrentSlot = pSlot;
            pSlot->pendingConfirms--;
            if (pLbpConfirm->status == (uint8_t)G3_SUCCESS)
            {
                switch (pSlot->slotState)
                {
                case LBP_STATE_SENT_EAP_MSG_1:
                    pSlot->slotState = LBP_STATE_WAITING_EAP_MSG_2;
                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Slot updated to LBP_STATE_WAITING_EAP_MSG_2\r\n");
                    lLBP_LogShowSlotStatus(pSlot);
                    break;

                case LBP_STATE_SENT_EAP_MSG_3:
                    pSlot->slotState = LBP_STATE_WAITING_EAP_MSG_4;
                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Slot updated to LBP_STATE_WAITING_EAP_MSG_4\r\n");
                    lLBP_LogShowSlotStatus(pSlot);
                    break;

                case LBP_STATE_SENT_EAP_MSG_ACCEPTED:
                    pSlot->slotState = LBP_STATE_WAITING_JOINNING;
                    pSlot->nonce =  0U;
                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Slot updated to LBP_STATE_WAITING_JOINNING\r\n");
                    lLBP_LogShowSlotStatus(pSlot);
                    isAcceptedConfirm = true;
                    break;

                case LBP_STATE_SENT_EAP_MSG_DECLINED:
                    pSlot->slotState = LBP_STATE_WAITING_JOINNING;
                    pSlot->nonce =  0U;
                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Slot updated to LBP_STATE_WAITING_JOINNING\r\n");
                    lLBP_LogShowSlotStatus(pSlot);
                    break;

                default:
                    pSlot->slotState = LBP_STATE_WAITING_JOINNING;
                    pSlot->nonce =  0U;
                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Slot updated to LBP_STATE_WAITING_JOINNING\r\n");
                    lLBP_LogShowSlotStatus(pSlot);
                    break;
                }
            }
            else
            {
                pSlot->slotState = LBP_STATE_WAITING_JOINNING;
                pSlot->nonce =  0U;
                SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Slot updated to LBP_STATE_WAITING_JOINNING\r\n");
                lLBP_LogShowSlotStatus(pSlot);
            }
        }
        else if ((pSlot->pendingConfirms == 2U) && (pLbpConfirm->nsduHandle == pSlot->pendingTxHandle))
        {
            /* Confirm received is for first request (pendingTxHandle) */
            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] AdpNotification_LbpConfirm (%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X).\r\n",
                    pSlot->lbdAddress.value[0], pSlot->lbdAddress.value[1],
                    pSlot->lbdAddress.value[2], pSlot->lbdAddress.value[3],
                    pSlot->lbdAddress.value[4], pSlot->lbdAddress.value[5],
                    pSlot->lbdAddress.value[6], pSlot->lbdAddress.value[7]);
            pSlot->pendingConfirms--;
            lLBP_LogShowSlotStatus(pSlot);
            pCurrentSlot = pSlot;
        }
        else{
            if ((pSlot->pendingConfirms == 2U) && (pLbpConfirm->nsduHandle == pSlot->txHandle))
            {
                /* Confirm received is for last request (txHandle) */
                SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] AdpNotification_LbpConfirm (%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X).\r\n",
                        pSlot->lbdAddress.value[0], pSlot->lbdAddress.value[1],
                        pSlot->lbdAddress.value[2], pSlot->lbdAddress.value[3],
                        pSlot->lbdAddress.value[4], pSlot->lbdAddress.value[5],
                        pSlot->lbdAddress.value[6], pSlot->lbdAddress.value[7]);
                pSlot->pendingConfirms--;
                pSlot->txHandle = pSlot->pendingTxHandle;
                lLBP_LogShowSlotStatus(pSlot);
                pCurrentSlot = pSlot;
            }
        }
    }

    if (pCurrentSlot == NULL)
    {
        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] AdpNotification_LbpConfirm from unkown node, status: %d  handler: %d \r\n",
                pLbpConfirm->status, pLbpConfirm->nsduHandle);
        isAcceptedConfirm = false;
    }
    else
    {
        pCurrentSlot->timeout = MAC_WRP_GetMsCounter() + (1000U * (uint32_t)sMsgTimeoutSeconds);
    }

    if ((pLbpConfirm->status == (uint8_t)G3_SUCCESS) && isAcceptedConfirm)
    {
        /* Upper layer indications */
        if (sLbpNotifications.joinCompleteIndication != NULL)
        {
            sLbpNotifications.joinCompleteIndication(pCurrentSlot->lbdAddress.value, pCurrentSlot->assignedShortAddress);
        }
    }
}

static void lLBP_AdpLbpIndicationCoord(ADP_LBP_IND_PARAMS *pLbpIndication)
{
    uint16_t origAddress;
    uint8_t msgType;
    uint8_t *pLbpData;
    uint16_t lbpDataLength;
    ADP_ADDRESS dstAddr;
    /* Embedded EAP message */
    uint8_t code = 0U;
    uint8_t identifier = 0U;
    uint8_t TSubfield = 0U;
    uint16_t EAPDataLength = 0U;
    uint8_t *pEAPData = NULL;
    ADP_EXTENDED_ADDRESS currentLbdAddress;
    LBP_SLOT *pSlot;
    uint8_t mediaType;
    uint8_t disableBackupMedium;

    if (pLbpIndication->srcAddr.addrSize == ADP_ADDRESS_64BITS)
    {
        /* When directly communicating with the LBD(using extended addressing) this field is set to 0xFFFF. */
        origAddress = 0xFFFFU;
        /* Check frame coming from LBD is not secured */
        if (pLbpIndication->securityLevel != (uint8_t)MAC_WRP_SECURITY_LEVEL_NONE)
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

    /* MISRA C-2012 deviation block start */
    /* MISRA C-2012 Rule 11.8 deviated once. Deviation record ID - H3_MISRAC_2012_R_11_8_DR_1 */

    msgType = pLbpIndication->pNsdu[0] >> 4;
    mediaType = (pLbpIndication->pNsdu[0] & 0x08U) >> 3;
    disableBackupMedium = (pLbpIndication->pNsdu[0] & 0x04U) >> 2;
    pLbpData = (uint8_t *)&pLbpIndication->pNsdu[10];
    lbpDataLength = pLbpIndication->nsduLength - 10U;

    /* MISRA C-2012 deviation block end */

    (void) memcpy(currentLbdAddress.value, &pLbpIndication->pNsdu[2], ADP_ADDRESS_64BITS);

    /* Check differet conditions if frame comes directly from LBD or from LBA */
    if (origAddress == 0xFFFFU)
    {
        /* Frame from LBD */
        /* Originator address must be equal to the one in LBP frame */
        if (memcmp(&currentLbdAddress.value, pLbpIndication->srcAddr.extendedAddr.value, sizeof(ADP_EXTENDED_ADDRESS)) != 0)
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
        if (origAddress > 0x7FFFU)
        {
            return;
        }
        /* Security level already checked for frames between 16-bit addresses */
    }

    pSlot = lLBP_GetLbpSlotByAddress(currentLbdAddress.value);
    if (pSlot != NULL)
    {
        lLBP_InitLbpMessage(pSlot);
    }

    if (msgType == LBP_JOINING)
    {
        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Processing incoming LBP_JOINING... LBD Address: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X \r\n",
                currentLbdAddress.value[0], currentLbdAddress.value[1],
                currentLbdAddress.value[2], currentLbdAddress.value[3],
                currentLbdAddress.value[4], currentLbdAddress.value[5],
                currentLbdAddress.value[6], currentLbdAddress.value[7]);

        /* Check the bootstrapping data in order to see the progress of the joining process */
        if (lbpDataLength == 0U)
        {
            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] First joining request.\r\n");
            /* This is the first joining request. Responded only if no other BS was in progress. */
            if (pSlot != NULL)
            {
                if (pSlot->slotState == LBP_STATE_WAITING_JOINNING)
                {
                    /* Set Media Type and Disable backup flag from incoming LBP frame on Slot */
                    pSlot->mediaType = mediaType;
                    pSlot->disableBackupMedium = disableBackupMedium;
                    /* Store information to be used in Address Assign function */
                    pSlot->lbaAddress = origAddress;
                    (void) memcpy(pSlot->lbdAddress.value, currentLbdAddress.value,
                            sizeof(currentLbdAddress.value));
                    /* Check with upper layer if the joining device is accepted */
                    if (sLbpNotifications.joinRequestIndication != NULL)
                    {
                        sLbpNotifications.joinRequestIndication(currentLbdAddress.value);
                    }
                    else
                    {
                        /* If there is no configured callback, assign addresses sequentially */
                        sInternalAssignedAddress++;
                        LBP_ShortAddressAssign(currentLbdAddress.value, sInternalAssignedAddress);
                    }
                    /* Exit function, response will be sent on Address Assign function */
                    return;
                }
                else
                {
                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Repeated message processing Joining, silently ignored \r\n");
                }
            }
            else
            {
                SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] No slots available to process the request. Ignored.\r\n");
            }
        }
        else
        {
            /* Check if the message comes from a device currently under BS */
            if (pSlot != NULL)
            {
                /* Set Media Type and Disable backup flag from incoming LBP frame on Slot */
                pSlot->mediaType = mediaType;
                pSlot->disableBackupMedium = disableBackupMedium;
                /* check the type of the bootstrap data */
                if ((pLbpData[0] & 0x01U) != 0x01U)
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
                            if ((TSubfield == EAP_PSK_T1) && ((pSlot->slotState == LBP_STATE_WAITING_EAP_MSG_2) ||
                                    (pSlot->slotState == LBP_STATE_SENT_EAP_MSG_1)))
                            {
                                if (lLBP_ProcessJoiningEAPT1(currentLbdAddress, EAPDataLength, pEAPData, pSlot) != true)
                                {
                                    /* Abort current BS process */
                                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] LBP error processing EAP T1.\r\n");
                                    pSlot->slotState = LBP_STATE_WAITING_JOINNING;
                                    pSlot->pendingConfirms = 0U;
                                    pSlot->nonce =  0U;

                                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Slot updated to LBP_STATE_WAITING_JOINNING\r\n");
                                }
                                else
                                {
                                    pSlot->slotState = LBP_STATE_SENT_EAP_MSG_3;
                                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Slot updated to LBP_STATE_SENT_EAP_MSG_3\r\n");
                                }
                            }
                            else if ((TSubfield == EAP_PSK_T3) &&
                                    ((pSlot->slotState == LBP_STATE_WAITING_EAP_MSG_4) ||
                                    (pSlot->slotState == LBP_STATE_SENT_EAP_MSG_3)))
                            {
                                if (lLBP_ProcessJoiningEAPT3(currentLbdAddress, pLbpData, EAPDataLength, pEAPData,
                                        pSlot))
                                {
                                    pSlot->slotState = LBP_STATE_SENT_EAP_MSG_ACCEPTED;
                                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Slot updated to LBP_STATE_SENT_EAP_MSG_ACCEPTED\r\n");
                                }
                                else
                                {
                                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] LBP error processing EAP T3.\r\n");
                                    pSlot->slotState = LBP_STATE_WAITING_JOINNING;
                                    pSlot->pendingConfirms = 0U;
                                    pSlot->nonce =  0U;
                                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Slot updated to LBP_STATE_WAITING_JOINNING\r\n");
                                }
                            }
                            else
                            {
                                /* Abort current BS process */
                                SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] LBP protocol error.\r\n");
                                pSlot->slotState = LBP_STATE_WAITING_JOINNING;
                                pSlot->pendingConfirms = 0U;
                                pSlot->nonce =  0U;
                                SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Slot updated to LBP_STATE_WAITING_JOINNING\r\n");
                            }
                        }
                    }
                    else
                    {
                        /* Abort current BS process */
                        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] ERROR decoding message.\r\n");
                        pSlot->slotState = LBP_STATE_WAITING_JOINNING;
                        pSlot->pendingConfirms = 0U;
                        pSlot->nonce =  0U;
                        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Slot updated to LBP_STATE_WAITING_JOINNING\r\n");
                        lLBP_LogShowSlotStatus(pSlot);
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
        if (pSlot->lbpDataLength > 0U)
        {
            if (origAddress == 0xFFFFU)
            {
                dstAddr.addrSize = ADP_ADDRESS_64BITS;
                (void) memcpy(dstAddr.extendedAddr.value, pSlot->lbdAddress.value, ADP_ADDRESS_64BITS);
            }
            else
            {
                dstAddr.addrSize = ADP_ADDRESS_16BITS;
                dstAddr.shortAddr = origAddress;
            }

            if (pSlot->pendingConfirms > 0U)
            {
                pSlot->pendingTxHandle = pSlot->txHandle;
            }

            pSlot->txHandle = lLBP_GetNextNsduHandle();
            pSlot->timeout = MAC_WRP_GetMsCounter() + (1000U * (uint32_t)sMsgTimeoutSeconds);
            pSlot->txAttempts = 0U;
            pSlot->pendingConfirms++;

            lLBP_LogShowSlotStatus(pSlot);
            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] ADP_LbpRequest Called, handler: %d \r\n", pSlot->txHandle);
            ADP_LbpRequest(
                    (ADP_ADDRESS const *)&dstAddr,  /* Destination address */
                    pSlot->lbpDataLength,           /* NSDU length */
                    &pSlot->lbpData[0],             /* NSDU */
                    pSlot->txHandle,                /* NSDU handle */
                    sMaxHops,                       /* Max. Hops */
                    true,                           /* Discover route */
                    0U,                              /* QoS */
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

    notifications.lbpConfirm = lLBP_AdpLbpConfirmCoord;
    notifications.lbpIndication = lLBP_AdpLbpIndicationCoord;

    ADP_SetNotificationsToLbp(&notifications);

    sRekey = false;

    sAribBand = aribBand;
    if (aribBand)
    {
        sIdS.size = LBP_NETWORK_ACCESS_ID_SIZE_S_ARIB;
        (void) memcpy(sIdS.value, sIdSArib.value, LBP_NETWORK_ACCESS_ID_SIZE_S_ARIB);
    }
    else
    {
        sIdS.size = LBP_NETWORK_ACCESS_ID_SIZE_S_CENELEC_FCC;
        (void) memcpy(sIdS.value, sIdSCenFcc.value, LBP_NETWORK_ACCESS_ID_SIZE_S_CENELEC_FCC);
    }

    lLBP_SetKeyingTable(INITIAL_KEY_INDEX, sCurrGMK);

    lLBP_InitLbpSlots();

    sMaxHops = lLBP_GetAdpMaxHops();

    lLBP_SetDeviceTypeCoord();
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

    lbpDataLength = LBP_EncodeKickToLBD(pEUI64Address, (uint16_t)sizeof(lbpData), lbpData);

    /* If message was properly encoded, send it */
    if (lbpDataLength > 0U)
    {
        ADP_LbpRequest(
                (ADP_ADDRESS const *)&dstAddr,  /* Destination address */
                lbpDataLength,                  /* NSDU length */
                lbpData,                        /* NSDU */
                lLBP_GetNextNsduHandle(),           /* NSDU handle */
                sMaxHops,                       /* Max. Hops */
                true,                           /* Discover route */
                0U,                              /* QoS */
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
    pSetConfirm->status = LBP_STATUS_INVALID_LENGTH;

    switch ((LBP_ATTRIBUTE)attributeId)
    {
    case LBP_IB_IDS:
        if ((attributeLen == LBP_NETWORK_ACCESS_ID_SIZE_S_ARIB) ||
            (attributeLen == LBP_NETWORK_ACCESS_ID_SIZE_S_CENELEC_FCC))
        {
            sIdS.size = attributeLen;
            (void) memcpy(sIdS.value, pAttributeValue, attributeLen);
            pSetConfirm->status = LBP_STATUS_OK;
        }

        break;

    case LBP_IB_PSK:
        if (attributeLen == 16U)
        {
            (void) memcpy(sEapPskKey.value, pAttributeValue, sizeof(sEapPskKey.value));
            pSetConfirm->status = LBP_STATUS_OK;
        }

        break;

    /* MISRA C-2012 deviation block start */
    /* MISRA C-2012 Rule 11.8 deviated once. Deviation record ID - H3_MISRAC_2012_R_11_8_DR_1 */

    case LBP_IB_GMK:
        if (attributeLen == 16U)
        {
            /* Set GMK on LBP module */
            (void) memcpy(sCurrGMK, pAttributeValue, sizeof(sCurrGMK));
            /* Set key table on G3 stack using the new index and new GMK */
            lLBP_SetKeyingTable((uint8_t)attributeIndex, (uint8_t *)pAttributeValue);
            pSetConfirm->status = LBP_STATUS_OK;
        }

        break;

    /* MISRA C-2012 deviation block end */

    case LBP_IB_REKEY_GMK:
        if (attributeLen == 16U)
        {
            (void) memcpy(sRekeyGMK, pAttributeValue, sizeof(sRekeyGMK));
            pSetConfirm->status = LBP_STATUS_OK;
        }

        break;

    case LBP_IB_MSG_TIMEOUT:
        if (attributeLen == 2U)
        {
            sMsgTimeoutSeconds = ((uint16_t)pAttributeValue[1] << 8) | pAttributeValue[0];
            pSetConfirm->status = LBP_STATUS_OK;
        }

        break;

    default:
        /* Unknown LBP parameter */
        pSetConfirm->status = LBP_STATUS_UNSUPPORTED_PARAMETER;
        break;
    }
}

void LBP_ShortAddressAssign(uint8_t *pExtAddress, uint16_t assignedAddress)
{
    LBP_SLOT *pSlot;
    ADP_ADDRESS dstAddr;
    
    /* Get slot from extended address*/
    pSlot = lLBP_GetLbpSlotByAddress(pExtAddress);

    if (pSlot != NULL)
    {
        if (assignedAddress == 0xFFFFU)
        {
            // Device rejected
            pSlot->lbpDataLength = LBP_EncodeDecline(&pSlot->lbdAddress, pSlot->mediaType,
                pSlot->disableBackupMedium, sEAPIdentifier, (uint16_t)sizeof(pSlot->lbpData), &pSlot->lbpData[0]);
            pSlot->slotState = LBP_STATE_SENT_EAP_MSG_DECLINED;
            sEAPIdentifier++;
            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Slot updated to LBP_STATE_SENT_EAP_MSG_DECLINED\r\n");
        }
        else
        {
            pSlot->assignedShortAddress = assignedAddress;
            lLBP_ProcessJoining0(&pSlot->lbdAddress, pSlot);
            pSlot->slotState = LBP_STATE_SENT_EAP_MSG_1;
            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] Slot updated to LBP_STATE_SENT_EAP_MSG_1\r\n");
        }
        
        if (pSlot->lbpDataLength > 0U)
        {
            if (pSlot->lbaAddress == 0xFFFFU)
            {
                dstAddr.addrSize = ADP_ADDRESS_64BITS;
                (void) memcpy(dstAddr.extendedAddr.value, pSlot->lbdAddress.value, ADP_ADDRESS_64BITS);
            }
            else
            {
                dstAddr.addrSize = ADP_ADDRESS_16BITS;
                dstAddr.shortAddr = pSlot->lbaAddress;
            }

            if (pSlot->pendingConfirms > 0U)
            {
                pSlot->pendingTxHandle = pSlot->txHandle;
            }

            pSlot->txHandle = lLBP_GetNextNsduHandle();
            pSlot->timeout = MAC_WRP_GetMsCounter() + (1000U * (uint32_t)sMsgTimeoutSeconds);
            pSlot->txAttempts = 0U;
            pSlot->pendingConfirms++;

            lLBP_LogShowSlotStatus(pSlot);
            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "[LBP] ADP_LbpRequest Called, handler: %d \r\n", pSlot->txHandle);
            ADP_LbpRequest(
                    (ADP_ADDRESS const *)&dstAddr,  /* Destination address */
                    pSlot->lbpDataLength,           /* NSDU length */
                    &pSlot->lbpData[0],             /* NSDU */
                    pSlot->txHandle,                /* NSDU handle */
                    sMaxHops,                       /* Max. Hops */
                    true,                           /* Discover route */
                    0U,                              /* QoS */
                    false);                         /* Security enable */
        }
    }
}
