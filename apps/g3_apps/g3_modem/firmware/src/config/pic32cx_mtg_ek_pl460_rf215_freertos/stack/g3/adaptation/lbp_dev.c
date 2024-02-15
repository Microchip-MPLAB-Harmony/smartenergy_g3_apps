/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    lbp_dev.c

  Summary:
    LBP Bootstrapping duties on PAN Device.

  Description:
    The LoWPAN Bootstrapping Protocol (LBP) provides a simple interface to
    manage the G3 boostrap process Adaptation Layer. This file provides the
    interface to manage LBP process for device.
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

/* ***************************************************************************** */
/* ***************************************************************************** */
/* Section: File includes */
/* ***************************************************************************** */
/* ***************************************************************************** */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "adp.h"
#include "adp_api_types.h"
#include "eap_psk.h"
#include "lbp_dev.h"
#include "lbp_encode_decode.h"
#include "lbp_defs.h"
#include "stack/g3/mac/mac_wrapper/mac_wrapper.h"
#include "routing_wrapper.h"
#include "system/time/sys_time.h"
#include "service/log_report/srv_log_report.h"
#include "service/random/srv_random.h"

/* ***************************************************************************** */
/* ***************************************************************************** */
/* Section: Macro Definitions */
/* ***************************************************************************** */
/* ***************************************************************************** */

/* Reserved value to consider a Key is not set */
#define UNSET_KEY                                   0xFFU

/* States of Bootstrap process for EndDevice */
#define LBP_STATE_BOOT_NOT_JOINED                   0x00U
#define LBP_STATE_BOOT_SENT_FIRST_JOINING           0x01U
#define LBP_STATE_BOOT_WAIT_EAPPSK_FIRST_MESSAGE    0x02U
#define LBP_STATE_BOOT_SENT_EAPPSK_SECOND_JOINING   0x04U
#define LBP_STATE_BOOT_WAIT_EAPPSK_THIRD_MESSAGE    0x08U
#define LBP_STATE_BOOT_SENT_EAPPSK_FOURTH_JOINING   0x10U
#define LBP_STATE_BOOT_WAIT_ACCEPTED                0x20U
#define LBP_STATE_BOOT_JOINED                       0x40U

/* EXT field types */
#define EAP_EXT_TYPE_CONFIGURATION_PARAMETERS       0x02U

/* Seconds to wait for reser after Kick reception */
#define KICK_WAIT_RESET_SECONDS                     2U

/* ***************************************************************************** */
/* ***************************************************************************** */
/* Section: Data Types */
/* ***************************************************************************** */
/* ***************************************************************************** */
#pragma pack(push,2)

typedef struct {
    /* Upper layer notifications */
    LBP_NOTIFICATIONS_DEV lbpNotifications;
    /* Timer to control the join or rekey process if no response is received */
    SYS_TIME_HANDLE joinTimer;
    /* Timer to control the delay to reset G3 stack after a Kick is received */
    SYS_TIME_HANDLE kickTimer;
    /* This parameter specifies the PAN ID: 0xFFFF means not connected to PAN */
    uint16_t panId;
    /* 16-bit address for new device which is unique inside the PAN */
    uint16_t shortAddress;
    /* information related to joining */
    uint16_t lbaAddress;
    uint16_t joiningShortAddress;
    /* Media Type to use between LBD and LBA. It is encoded in LBP frames */
    uint8_t mediaType;
    /* Disable Backup Flag, used to precise Media Type. It is encoded in LBP frames */
    uint8_t disableBackupFlag;
    /* State of the bootstrap process */
    uint8_t bootstrapState;
    /* Number of pending message confirms */
    uint8_t pendingConfirms;
    bool joinTimerExpired;
    bool rekeyTimerExpired;
    bool kickTimerExpired;
    /* Available MAC layers */
    ADP_AVAILABLE_MAC_LAYERS availableMacLayers;
    /* EUI64 address of the device */
    ADP_EXTENDED_ADDRESS EUI64Address;
    /* Holds the GMK keys */
    ADP_GROUP_MASTER_KEY groupMasterKey[2];
    /* keeps authentication context */
    EAP_PSK_CONTEXT pskContext;

} LBP_CONTEXT_DEV;

#pragma pack(pop)

typedef enum {
    LBP_JOIN_CALLBACK = 0,
    LBP_LEAVE_CALLBACK,
    LBP_DUMMY_CALLBACK,

} LBP_CALLBACK_TYPES;

/* ***************************************************************************** */
/* ***************************************************************************** */
/* Section: File Scope Variables */
/* ***************************************************************************** */
/* ***************************************************************************** */

/* Default value for PSK
 * This parameter is intended to be set from Application through LBP_IB_PSK IB */
static EAP_PSK_KEY sEapPskKey = {
    {0xAB, 0x10, 0x34, 0x11, 0x45, 0x11, 0x1B, 0xC3, 0xC1, 0x2D, 0xE8, 0xFF, 0x11, 0x14, 0x22, 0x04}
};

/* Default value for ID_P
 * This parameter is intended to be set from Application through LBP_IB_IDP IB */
static EAP_PSK_NETWORK_ACCESS_ID_P sIdP = {0};

/* Default value for RAND_P
 * This parameter is intended to be set from Application through LBP_IB_RANDP IB */
static EAP_PSK_RAND sRandP = {{0}};

static LBP_CONTEXT_DEV lbpContext;

static uint8_t sLbpBuffer[ADP_LBP_MAX_NSDU_LENGTH];

static uint8_t sNsduHandle = 0;
static uint8_t sMaxHops = 0;

static LBP_CALLBACK_TYPES sLbpCallbackType;

/* ***************************************************************************** */
/* ***************************************************************************** */
/* Section: Auxiliary external functions */
/* ***************************************************************************** */
/* ***************************************************************************** */

/* MISRA C-2012 deviation block start */
/* MISRA C-2012 Rule 8.6 deviated 8 times.  Deviation record ID - H3_MISRAC_2012_R_8_6_DR_1 */

extern bool AdpMac_SetRcCoordSync(uint16_t u16RcCoord);
extern bool AdpMac_SecurityResetSync(void);
extern bool AdpMac_DeleteGroupMasterKeySync(uint8_t keyId);
extern bool AdpMac_SetGroupMasterKeySync(const ADP_GROUP_MASTER_KEY *pMasterKey);
extern bool AdpMac_SetShortAddressSync(uint16_t u16ShortAddress);
extern bool AdpMac_GetExtendedAddressSync(ADP_EXTENDED_ADDRESS *pExtendedAddress);
extern bool AdpMac_SetExtendedAddressSync(const ADP_EXTENDED_ADDRESS *pExtendedAddress);
extern bool AdpMac_SetPanIdSync(uint16_t panId);

/* MISRA C-2012 deviation block end */

/* ***************************************************************************** */
/* ***************************************************************************** */
/* Section: local functions */
/* ***************************************************************************** */
/* ***************************************************************************** */

static void lLBP_SetBootState(uint8_t state)
{
    lbpContext.bootstrapState = state;

    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "lLBP_SetBootState() Set state to %s Pending Confirms: %d\r\n",
            state == LBP_STATE_BOOT_NOT_JOINED ? "LBP_STATE_BOOT_NOT_JOINED" :
            state == LBP_STATE_BOOT_SENT_FIRST_JOINING ? "LBP_STATE_BOOT_SENT_FIRST_JOINING" :
            state == LBP_STATE_BOOT_WAIT_EAPPSK_FIRST_MESSAGE ? "LBP_STATE_BOOT_WAIT_EAPPSK_FIRST_MESSAGE" :
            state == LBP_STATE_BOOT_SENT_EAPPSK_SECOND_JOINING ? "LBP_STATE_BOOT_SENT_EAPPSK_SECOND_JOINING" :
            state == LBP_STATE_BOOT_WAIT_EAPPSK_THIRD_MESSAGE ? "LBP_STATE_BOOT_WAIT_EAPPSK_THIRD_MESSAGE" :
            state == LBP_STATE_BOOT_SENT_EAPPSK_FOURTH_JOINING ? "LBP_STATE_BOOT_SENT_EAPPSK_FOURTH_JOINING" :
            state == LBP_STATE_BOOT_WAIT_ACCEPTED ? "LBP_STATE_BOOT_WAIT_ACCEPTED" :
            state == LBP_STATE_BOOT_JOINED ? "LBP_STATE_BOOT_JOINED" : "?????",
            lbpContext.pendingConfirms
            );

    if (state == LBP_STATE_BOOT_NOT_JOINED)
    {
        lbpContext.pendingConfirms = 0;
    }
}

static bool lLBP_IsBootState(uint8_t stateMask)
{
    return ((lbpContext.bootstrapState & stateMask) != 0U) ||
           (lbpContext.bootstrapState == stateMask); /* special case for NotJoined (== 0) */
}

static bool lLBP_IsJoined(void)
{
    return (lbpContext.shortAddress != 0xFFFFU);
}

static void lLBP_ForceJoinStatus(bool joined)
{
    if (joined)
    {
        lLBP_SetBootState(LBP_STATE_BOOT_JOINED);
    }
    else
    {
        lLBP_SetBootState(LBP_STATE_BOOT_NOT_JOINED);
        lbpContext.shortAddress = 0xFFFFU;
        lbpContext.joiningShortAddress = 0xFFFFU;
    }
}

static uint16_t lLBP_GetCoordShortAddress(void)
{
    ADP_GET_CFM_PARAMS getConfirm;
    uint16_t coordShortAddress = 0U;
    ADP_GetRequestSync((uint32_t)ADP_IB_COORD_SHORT_ADDRESS, 0U, &getConfirm);
    if (getConfirm.status == (uint8_t)G3_SUCCESS)
    {
        (void) memcpy((void*)&coordShortAddress, (void*)getConfirm.attributeValue, sizeof(coordShortAddress));
    }
    return coordShortAddress;
}

static uint8_t lLBP_GetActiveKeyIndex(void)
{
    ADP_GET_CFM_PARAMS getConfirm;
    ADP_GetRequestSync((uint32_t)ADP_IB_ACTIVE_KEY_INDEX, 0U, &getConfirm);
    return getConfirm.attributeValue[0];
}

static void lLBP_SetPanId(uint16_t panId)
{
    ADP_SET_CFM_PARAMS setConfirm;
    uint8_t panIdArray[2];

    (void) memcpy((void*)panIdArray, (void*)&panId, sizeof(panIdArray));
    /* Set on ADP */
    ADP_SetRequestSync((uint32_t)ADP_IB_MANUF_PAN_ID, 0U, 2U, panIdArray, &setConfirm);
    if (setConfirm.status == (uint8_t)G3_SUCCESS)
    {
        /* Set on MAC */
        (void) AdpMac_SetPanIdSync(panId);
    }
}

static bool lLBP_SetShortAddress(uint16_t shortAddress)
{
    ADP_SET_CFM_PARAMS setConfirm;
    uint8_t shortAddressArray[2];
    bool result = false;

    (void) memcpy((void*)shortAddressArray, (void*)&shortAddress, sizeof(shortAddressArray));
    /* Set on ADP */
    ADP_SetRequestSync((uint32_t)ADP_IB_MANUF_SHORT_ADDRESS, 0U, 2U, shortAddressArray, &setConfirm);
    if (setConfirm.status == (uint8_t)G3_SUCCESS)
    {
        /* Set on MAC */
        result = AdpMac_SetShortAddressSync(shortAddress);
    }

    return result;
}

static void lLBP_SetActiveKeyIndex(uint8_t index)
{
    ADP_SET_CFM_PARAMS setConfirm;
    ADP_SetRequestSync((uint32_t)ADP_IB_ACTIVE_KEY_INDEX, 0U, 1U, (uint8_t *)&index, &setConfirm);
}

static uint16_t lLBP_GetMaxJoinWaitTime(void)
{
    ADP_GET_CFM_PARAMS getConfirm;
    uint16_t maxJoinWaitTime = 0U;
    ADP_GetRequestSync((uint32_t)ADP_IB_MAX_JOIN_WAIT_TIME, 0U, &getConfirm);
    (void) memcpy((void*)&maxJoinWaitTime, (void*)getConfirm.attributeValue, sizeof(maxJoinWaitTime));
    return maxJoinWaitTime;
}

static void lLBP_GetIdP(EAP_PSK_NETWORK_ACCESS_ID_P *pIdP)
{
    ADP_MAC_GET_CFM_PARAMS getConfirm;
    if (sIdP.size == 0U)
    {
        /* IdP not set, use Extended Address */
        ADP_MacGetRequestSync((uint32_t)MAC_WRP_PIB_MANUF_EXTENDED_ADDRESS, 0U, &getConfirm);
        pIdP->size = getConfirm.attributeLength;
        (void) memcpy(pIdP->value, getConfirm.attributeValue, sizeof(pIdP->value));
    }
    else {
        /* Use stored IdP */
        pIdP->size = sIdP.size;
        (void) memcpy(pIdP->value, sIdP.value, sizeof(pIdP->value));
    }
}

static uint8_t lLBP_GetAdpMaxHops(void)
{
    ADP_GET_CFM_PARAMS getConfirm;
    ADP_GetRequestSync((uint32_t)ADP_IB_MAX_HOPS, 0U, &getConfirm);
    return getConfirm.attributeValue[0];
}

static void lLBP_SetDeviceTypeDev(void)
{
    ADP_SET_CFM_PARAMS setConfirm;
    uint8_t devTpe = 0U;

    /* Set on ADP */
    ADP_SetRequestSync((uint32_t)ADP_IB_DEVICE_TYPE, 0U, 1U, &devTpe, &setConfirm);
}

static void lLBP_ClearEapContext(void)
{
    (void) memset(&lbpContext.pskContext.tek, 0,
            sizeof(lbpContext.pskContext.tek));

    (void) memset(&lbpContext.pskContext.idS, 0,
            sizeof(lbpContext.pskContext.idS));

    (void) memset(&lbpContext.pskContext.randP, 0,
            sizeof(lbpContext.pskContext.randP));

    (void) memset(&lbpContext.pskContext.randS, 0,
            sizeof(lbpContext.pskContext.randS));
}

static bool lLBP_ProcessParameters(uint16_t dataLength, uint8_t *pData,
        uint8_t *pReceivedParametersMask,
        uint8_t *pParameterResultLength, uint8_t *pParameterResult)
{
    uint16_t offset = 0U;
    uint8_t attrId = 0U;
    uint8_t attrLen = 0U;
    uint8_t *pAttrValue = NULL;
    uint8_t result = LBP_RESULT_PARAMETER_SUCCESS;
    uint8_t parameterMask = 0U;
    uint16_t shortAddress = 0U;
    ADP_GROUP_MASTER_KEY gmk[2];
    uint8_t keyId;
    uint8_t activeKeyId = 0U;
    uint8_t deleteKeyId = 0U;

    /* Invalidate gmk indices to further tell which one is written */
    gmk[0].keyId = UNSET_KEY;
    gmk[1].keyId = UNSET_KEY;

    /* Bootstrapping data carries the configuration parameters: short address, gmk, gmk activation */
    /* Decode and process configuration parameters */
    while ((offset < dataLength) && (result == LBP_RESULT_PARAMETER_SUCCESS))
    {
        attrId = pData[offset++];
        attrLen = pData[offset++];
        pAttrValue = &pData[offset];

        if (offset + attrLen <= dataLength)
        {
            /* Extract attribute */
            if (attrId == LBP_CONF_PARAM_SHORT_ADDR)
            {
                /* Short Address */
                if (attrLen == 2U)
                {
                    shortAddress = ((uint16_t)pAttrValue[0] << 8) | pAttrValue[1];
                    parameterMask |= LBP_CONF_PARAM_SHORT_ADDR_MASK;
                }
                else
                {
                    result = LBP_RESULT_INVALID_PARAMETER_VALUE;
                }
            }
            else if (attrId == LBP_CONF_PARAM_GMK)
            {
                /* Provide a GMK key. On reception, the key is installed in the provided Key Identifier slot. */
                /* Contains the following fields: */
                /* id (1 byte): the Key Identifier of the GMK */
                /* gmk (16 bytes): the value of the current GMK */
                if (attrLen == 17U)
                {
                    keyId = pAttrValue[0];
                    if ((keyId == 0U) || (keyId == 1U))
                    {
                        gmk[keyId].keyId = keyId;
                        (void) memcpy(gmk[keyId].key, &pAttrValue[1], sizeof(gmk[keyId].key));
                        parameterMask |= LBP_CONF_PARAM_GMK_MASK;
                    }
                    else
                    {
                        result = LBP_RESULT_INVALID_PARAMETER_VALUE;
                    }
                }
                else
                {
                    result = LBP_RESULT_INVALID_PARAMETER_VALUE;
                }
            }
            else if (attrId == LBP_CONF_PARAM_GMK_ACTIVATION)
            {
                /* Indicate the GMK to use for outgoing messages */
                /* Contains the following field: */
                /* id (1 byte): the Key Identifier of the active GMK */
                if (attrLen == 1U)
                {
                    /* GMK to activate cannot be unset */
                    if ((lbpContext.groupMasterKey[pAttrValue[0]].keyId == UNSET_KEY) &&
                        (gmk[pAttrValue[0]].keyId == UNSET_KEY))
                    {
                        result = LBP_RESULT_INVALID_PARAMETER_VALUE;
                    }
                    else
                    {
                        activeKeyId = pAttrValue[0];
                        parameterMask |= LBP_CONF_PARAM_GMK_ACTIVATION_MASK;
                    }
                }
                else
                {
                    result = LBP_RESULT_INVALID_PARAMETER_VALUE;
                }
            }
            else if (attrId == LBP_CONF_PARAM_GMK_REMOVAL)
            {
                /* Indicate a GMK to delete */
                /* Contains the following field: */
                /* id (1 byte): the Key Identifier of the GMK to delete */
                if (attrLen == 1U)
                {
                    deleteKeyId = pAttrValue[0];
                    parameterMask |= LBP_CONF_PARAM_GMK_REMOVAL_MASK;
                }
                else
                {
                    result = LBP_RESULT_INVALID_PARAMETER_VALUE;
                }
            }
            else
            {
                SRV_LOG_REPORT_Message(SRV_LOG_REPORT_ERROR, "lLBP_ProcessParameters() Unsupported attribute id %u, len %u\r\n", attrId, attrLen);
                result = LBP_RESULT_UNKNOWN_PARAMETER_ID;
            }
        }
        else
        {
            result = LBP_RESULT_INVALID_PARAMETER_VALUE;
        }

        offset += attrLen;
    }

    if (result == LBP_RESULT_PARAMETER_SUCCESS)
    {
        /* Verify the validity of parameters */
        bool bParamsValid = true;
        attrId = 0U;
        if (!lLBP_IsJoined())
        {
            /* If device not joined yet, the following parameters are mandatory: */
            /* LBP_CONF_PARAM_SHORT_ADDR_MASK */
            /* LBP_CONF_PARAM_GMK_MASK */
            /* LBP_CONF_PARAM_GMK_ACTIVATION_MASK */
            if ((parameterMask & LBP_CONF_PARAM_SHORT_ADDR_MASK) != LBP_CONF_PARAM_SHORT_ADDR_MASK)
            {
                attrId = LBP_CONF_PARAM_SHORT_ADDR;
            }
            else if ((parameterMask & LBP_CONF_PARAM_GMK_MASK) != LBP_CONF_PARAM_GMK_MASK)
            {
                attrId = LBP_CONF_PARAM_GMK;
            }
            else
            {
                if ((parameterMask & LBP_CONF_PARAM_GMK_ACTIVATION_MASK) != LBP_CONF_PARAM_GMK_ACTIVATION_MASK)
                {
                    attrId = LBP_CONF_PARAM_GMK_ACTIVATION;
                }
            }
        }
        else
        {
            /* If device already joined, the message should contain one of the following parameters: */
            /* LBP_CONF_PARAM_GMK_MASK */
            /* LBP_CONF_PARAM_GMK_ACTIVATION_MASK */
            /* LBP_CONF_PARAM_GMK_REMOVAL_MASK */
            if ((parameterMask & (LBP_CONF_PARAM_GMK_MASK | LBP_CONF_PARAM_GMK_ACTIVATION_MASK | LBP_CONF_PARAM_GMK_REMOVAL_MASK)) != 0U)
            {
                /* One of required parameters has been found; nothing to do */
            }
            else
            {
                /* No required parameters was received; just send back an error related to GMK-ACTIVATION (as missing parameters) */
                attrId = LBP_CONF_PARAM_GMK_ACTIVATION;
            }
        }

        bParamsValid = (attrId == 0U);

        if (!bParamsValid)
        {
            result = LBP_RESULT_MISSING_REQUIRED_PARAMETER;
        }
        else
        {
            *pReceivedParametersMask = parameterMask;
            if ((parameterMask & LBP_CONF_PARAM_SHORT_ADDR_MASK) == LBP_CONF_PARAM_SHORT_ADDR_MASK)
            {
                /* short address will be set only after receiving the EAP-Success message */
                lbpContext.joiningShortAddress = shortAddress;
                SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "lLBP_ProcessParameters() ShortAddress %04X\r\n", lbpContext.joiningShortAddress);
            }

            if ((parameterMask & LBP_CONF_PARAM_GMK_MASK) == LBP_CONF_PARAM_GMK_MASK)
            {
                if (gmk[0].keyId != UNSET_KEY)
                {
                    lbpContext.groupMasterKey[0] = gmk[0];
                    SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, lbpContext.groupMasterKey[0].key, 16U, "lLBP_ProcessParameters() New GMK (id=%u) ", lbpContext.groupMasterKey[0].keyId);
                }
                if (gmk[1].keyId != UNSET_KEY)
                {
                    lbpContext.groupMasterKey[1] = gmk[1];
                    SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, lbpContext.groupMasterKey[1].key, 16U, "lLBP_ProcessParameters() New GMK (id=%u) ", lbpContext.groupMasterKey[1].keyId);
                }
            }

            if ((parameterMask & LBP_CONF_PARAM_GMK_ACTIVATION_MASK) == LBP_CONF_PARAM_GMK_ACTIVATION_MASK)
            {
                if (lLBP_GetActiveKeyIndex() != activeKeyId)
                {
                    /* On reception of the GMK-activation parameter, the peer shall empty its DeviceTable */
                    /* (in order to allow re-use of previously allocated short addresses). */
                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "lLBP_ProcessParameters() Key-id changed; Reset device table\r\n");
                    (void) AdpMac_SecurityResetSync();
                }

                lLBP_SetActiveKeyIndex(activeKeyId);
                SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "lLBP_ProcessParameters() Active GMK id: %u\r\n", activeKeyId);
            }

            if ((parameterMask & LBP_CONF_PARAM_GMK_REMOVAL_MASK) == LBP_CONF_PARAM_GMK_REMOVAL_MASK)
            {
                if (AdpMac_DeleteGroupMasterKeySync(deleteKeyId))
                {
                    /* Mark key as deleted */
                    lbpContext.groupMasterKey[deleteKeyId].keyId = UNSET_KEY;
                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_ERROR, "lLBP_ProcessParameters() GMK id %u was deleted!\r\n", deleteKeyId);
                }
                else
                {
                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_ERROR, "lLBP_ProcessParameters() Cannot delete GMK id: %u!\r\n", deleteKeyId);
                }
            }
        }
    }

    /* Prepare p-result */
    /* If one or more parameter were invalid or missing, the peer send a message 4 with R = DONE_FAILURE and */
    /* an embedded configuration field with at least one Parameter-result indicating the error. */
    pParameterResult[(*pParameterResultLength)++] = LBP_CONF_PARAM_RESULT;
    pParameterResult[(*pParameterResultLength)++] = 2U;
    pParameterResult[(*pParameterResultLength)++] = result;
    pParameterResult[(*pParameterResultLength)++] = (result == LBP_RESULT_PARAMETER_SUCCESS) ? 0U : attrId;

    return (result == LBP_RESULT_PARAMETER_SUCCESS);
}

static void lLBP_JoinTimerExpiredCallback(uintptr_t context)
{
    (void)(context);
    lbpContext.joinTimerExpired = true;
}

static void lLBP_KickTimerExpiredCallback(uintptr_t context)
{
    (void)(context);
    lbpContext.kickTimerExpired = true;
}

static void lLBP_RekeyTimerExpiredCallback(uintptr_t context)
{
    (void)(context);
    lbpContext.rekeyTimerExpired = true;
}

static void lLBP_JoinConfirm(ADP_RESULT status)
{
    uint8_t mediaType;
    LBP_ADP_NETWORK_JOIN_CFM_PARAMS joinConfirm;

    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO, "lLBP_JoinConfirm() Status: %u\r\n", status);
    (void) SYS_TIME_TimerDestroy(lbpContext.joinTimer);
    lbpContext.joinTimer = SYS_TIME_HANDLE_INVALID;
    lbpContext.joinTimerExpired = false;
    lLBP_ClearEapContext();

    if (status == G3_SUCCESS)
    {
        lLBP_SetBootState(LBP_STATE_BOOT_JOINED);
        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO,
            "NetworkJoin() JOIN FINISHED SUCCESSFULLY! PAN-ID:0x%04X SHORT-ADDRESS:0x%04X\r\n",
            lbpContext.panId, lbpContext.shortAddress);

        if ((!ROUTING_WRP_IsDisabled()) && ROUTING_WRP_IsDefaultCoordRouteEnabled())
        {
            bool tableFull;
            if ((lbpContext.mediaType == (uint8_t)MAC_WRP_MEDIA_TYPE_REQ_RF_BACKUP_PLC) ||
                (lbpContext.mediaType == (uint8_t)MAC_WRP_MEDIA_TYPE_REQ_RF_NO_BACKUP))
            {
                mediaType = 1U;
            }
            else
            {
                mediaType = 0U;
            }
            (void) ROUTING_WRP_AddRoute(lLBP_GetCoordShortAddress(), lbpContext.lbaAddress, mediaType, &tableFull);
        }

        /* Set LBP Status on ADP */
        ADP_SetLBPStatusConnection(true);
    }
    else
    {
        lLBP_SetBootState(LBP_STATE_BOOT_NOT_JOINED);
        lbpContext.shortAddress = 0xFFFFU;
        lbpContext.panId = 0xFFFFU;
        (void) lLBP_SetShortAddress(0xFFFFU);
        lLBP_SetPanId(0xFFFFU);
    }

    if (lbpContext.lbpNotifications.adpNetworkJoinConfirm != NULL)
    {
        joinConfirm.status = (uint8_t)status;
        joinConfirm.networkAddress = lbpContext.shortAddress;
        joinConfirm.panId = lbpContext.panId;
        lbpContext.lbpNotifications.adpNetworkJoinConfirm(&joinConfirm);
    }
}

static void lLBP_JoinDataSendCallback(uint8_t status)
{
    lbpContext.pendingConfirms--;
    if (status == (uint8_t)G3_SUCCESS)
    {
        if (lbpContext.pendingConfirms == 0U)
        {
            /* Message successfully sent: Update state and wait for response or timeout */
            if (lLBP_IsBootState(LBP_STATE_BOOT_SENT_FIRST_JOINING))
            {
                lLBP_SetBootState(LBP_STATE_BOOT_WAIT_EAPPSK_FIRST_MESSAGE);
            }
            else if (lLBP_IsBootState(LBP_STATE_BOOT_SENT_EAPPSK_SECOND_JOINING))
            {
                lLBP_SetBootState(LBP_STATE_BOOT_WAIT_EAPPSK_THIRD_MESSAGE);
            }
            else
            {
                if (lLBP_IsBootState(LBP_STATE_BOOT_SENT_EAPPSK_FOURTH_JOINING))
                {
                    lLBP_SetBootState(LBP_STATE_BOOT_WAIT_ACCEPTED);
                }
            }
        }
    }
    else
    {
        lLBP_JoinConfirm((ADP_RESULT)status);
    }
}

static void lLBP_KickNotify(void)
{
    /* We have been kicked out of the network */
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "Device get KICKed off the network by the coordinator\r\n");

    /* Arm a timer and reset the mac after a few milliseconds */
    /* in order to give time to MAC/PHY to send the ACK */
    /* related to the Kick message */
    lbpContext.kickTimer =
        SYS_TIME_CallbackRegisterMS(lLBP_KickTimerExpiredCallback,
        (uintptr_t)0, KICK_WAIT_RESET_SECONDS * 1000U, SYS_TIME_SINGLE);
}

static void lLBP_LeaveCallback(uint8_t status)
{
    /* Whatever the result, Reset Stack */
    ADP_ResetRequest();
    lLBP_SetBootState(LBP_STATE_BOOT_NOT_JOINED);
    lbpContext.shortAddress = 0xFFFFU;
    lbpContext.panId = 0xFFFFU;
    (void) lLBP_SetShortAddress(0xFFFFU);
    lLBP_SetPanId(0xFFFFU);

    /* Set LBP Status on ADP */
    ADP_SetLBPStatusConnection(false);

    if (lbpContext.lbpNotifications.adpNetworkLeaveConfirm != NULL) {
        lbpContext.lbpNotifications.adpNetworkLeaveConfirm(status);
    }
}

static void lLBP_JoinProcessChallengeFirstMessage(const ADP_EXTENDED_ADDRESS *pEUI64Address,
        uint8_t EAPIdentifier, uint16_t EAPDataLength, uint8_t *pEAPData)
{
    EAP_PSK_RAND randS;
    EAP_PSK_NETWORK_ACCESS_ID_S idS;
    EAP_PSK_NETWORK_ACCESS_ID_P idP;
    lLBP_GetIdP(&idP);

    /* In order to detect if is a valid repetition we have to check the 2 elements */
    /* carried by the first EAP-PSK message: RandS and IdS */
    /* In case of a valid repetition we have to send back the same response message */
    if (EAP_PSK_DecodeMessage1(EAPDataLength, pEAPData, &randS, &idS))
    {
        ADP_ADDRESS dstAddr;
        uint8_t *pMemoryBuffer = sLbpBuffer;
        uint16_t memoryBufferLength = (uint16_t)sizeof(sLbpBuffer);
        uint16_t requestLength = 0U;
        bool repetition;

        repetition = (memcmp(randS.value, lbpContext.pskContext.randS.value, sizeof(randS.value)) == 0)
            && (memcmp(idS.value, lbpContext.pskContext.idS.value, lbpContext.pskContext.idS.size) == 0);

        if (!repetition)
        {
            uint8_t randPnull[16] = {0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U};

            /* Save current values; needed to detect repetitions */
            (void) memcpy(lbpContext.pskContext.randS.value, randS.value, sizeof(randS.value));
            (void) memcpy(lbpContext.pskContext.idS.value, idS.value, idS.size);
            lbpContext.pskContext.idS.size = idS.size;

            /* Process RandP (Use the value from MIB if set by user) */
            if (memcmp(sRandP.value, randPnull, 16) != 0)
            {
                (void) memcpy(lbpContext.pskContext.randP.value, sRandP.value, sizeof(EAP_PSK_RAND));
            }
            else
            {
                /* Initialize RandP with random content */
                SRV_RANDOM_Get128bits(lbpContext.pskContext.randP.value);
            }

            EAP_PSK_InitializeTEKMSK(&lbpContext.pskContext.randP, &lbpContext.pskContext);
        }

        requestLength = EAP_PSK_EncodeMessage2(&lbpContext.pskContext, EAPIdentifier,
                &randS, &lbpContext.pskContext.randP,
                &lbpContext.pskContext.idS, &idP,
                memoryBufferLength, pMemoryBuffer);

        /* Encode the LBP message */
        if (lLBP_IsJoined())
        {
            /* Rekeying frame, set Media Type and Disable Backup to 0 */
            requestLength = LBP_EncodeJoiningRequest(pEUI64Address, 0U, 0U, requestLength, memoryBufferLength, pMemoryBuffer);
        }
        else
        {
            requestLength = LBP_EncodeJoiningRequest(pEUI64Address, lbpContext.mediaType, lbpContext.disableBackupFlag, requestLength, memoryBufferLength, pMemoryBuffer);
        }

        dstAddr.addrSize = ADP_ADDRESS_16BITS;
        dstAddr.shortAddr = lbpContext.lbaAddress;

        /* If this is a rekey procedure (instead of a join) arm here the timer in order to control the rekey; */
        /* messages can be loss and we want to clean the context after a while */
        if (lLBP_IsJoined())
        {
            /* Start rekey timer */
            lbpContext.joinTimer =
                SYS_TIME_CallbackRegisterMS(lLBP_RekeyTimerExpiredCallback,
                (uintptr_t)0, (uint32_t)lLBP_GetMaxJoinWaitTime() * 1000U, SYS_TIME_SINGLE);
        }

        lbpContext.pendingConfirms++;
        lLBP_SetBootState(LBP_STATE_BOOT_SENT_EAPPSK_SECOND_JOINING);
        sLbpCallbackType = LBP_JOIN_CALLBACK;
        ADP_LbpRequest(&dstAddr, requestLength, pMemoryBuffer, sNsduHandle++, sMaxHops, true, 0U, false);
    }
    else
    {
        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_ERROR, "ADPM_Network_Join_Request() Invalid server response: EAP_PSK_DecodeMessage1\r\n");
        /* Wait for NetworkJoin timeout */
    }
}

static void lLBP_JoinProcessChallengeThirdMessage(uint16_t headerLength, uint8_t *pHeader,
        uint8_t EAPIdentifier, uint16_t EAPDataLength, uint8_t *pEAPData)
{
    EAP_PSK_RAND randS;
    uint8_t PChannelResult = 0U;
    uint32_t nonce = 0U;
    uint16_t PChannelDataLength = 0U;
    uint8_t *pPChannelData = NULL;

    /* If rand_S does not meet with the one that identifies the */
    if (EAP_PSK_DecodeMessage3(EAPDataLength, pEAPData, &lbpContext.pskContext,
            headerLength, pHeader, &randS, &nonce, &PChannelResult,
            &PChannelDataLength, &pPChannelData) &&
            (PChannelResult == PCHANNEL_RESULT_DONE_SUCCESS))
    {
        if (memcmp(randS.value, lbpContext.pskContext.randS.value, sizeof(randS.value)) == 0)
        {
            ADP_ADDRESS dstAddr;
            uint8_t *pMemoryBuffer = sLbpBuffer;
            uint16_t memoryBufferLength = (uint16_t)sizeof(sLbpBuffer);
            uint16_t requestLength = 0U;
            uint8_t receivedParameters = 0U;
            uint8_t parameterResult[10]; /* buffer to encode the parameter result */
            uint8_t parameterResultLength = 0U;
            PChannelResult = PCHANNEL_RESULT_DONE_FAILURE;

            /* ParameterResult carry config parameters */
            parameterResult[0] = EAP_EXT_TYPE_CONFIGURATION_PARAMETERS;
            parameterResultLength++;

            /* If one or more parameter were invalid or missing, */
            /* the peer sends a message 4 with R = DONE_FAILURE and */
            /* an embedded configuration field with at least one */
            /* Parameter-result indicating the error. */

            /* Check if protected data carries EXT field */
            if (pPChannelData[0] == EAP_EXT_TYPE_CONFIGURATION_PARAMETERS)
            {
                if (lLBP_ProcessParameters(PChannelDataLength - 1U, &pPChannelData[1],
                        &receivedParameters, &parameterResultLength, parameterResult))
                {
                    PChannelResult = PCHANNEL_RESULT_DONE_SUCCESS;
                }
            }
            else
            {
                /* Build ParameterResult indicating missing GMK key */
                parameterResult[parameterResultLength++] = LBP_CONF_PARAM_RESULT;
                parameterResult[parameterResultLength++] = 2U;
                parameterResult[parameterResultLength++] = LBP_RESULT_MISSING_REQUIRED_PARAMETER;
                parameterResult[parameterResultLength++] = LBP_CONF_PARAM_GMK;
            }

            /* After receiving from the server a valid EAP-PSK message with Nonce */
            /* set to N, the peer will answer with an EAP-PSK message with Nonce set to N+1 */
            requestLength = EAP_PSK_EncodeMessage4(&lbpContext.pskContext, EAPIdentifier,
                    &randS, nonce + 1U, PChannelResult, parameterResultLength,
                    parameterResult, memoryBufferLength, pMemoryBuffer);

            /* Encode the LBP message */
            requestLength = LBP_EncodeJoiningRequest(&lbpContext.EUI64Address, lbpContext.mediaType,
                    lbpContext.disableBackupFlag, requestLength, memoryBufferLength, pMemoryBuffer);

            dstAddr.addrSize = ADP_ADDRESS_16BITS;
            dstAddr.shortAddr = lbpContext.lbaAddress;

            if (PChannelResult != PCHANNEL_RESULT_DONE_SUCCESS)
            {
                SRV_LOG_REPORT_Message(SRV_LOG_REPORT_ERROR, "ADPM_Network_Join_Request() EAP_PSK_DecodeMessage3() Invalid parameters\r\n");
                /* Wait for timeout */
            }

            lbpContext.pendingConfirms++;
            lLBP_SetBootState(LBP_STATE_BOOT_SENT_EAPPSK_FOURTH_JOINING);
            sLbpCallbackType = LBP_JOIN_CALLBACK;
            ADP_LbpRequest(&dstAddr, requestLength, pMemoryBuffer, sNsduHandle++, sMaxHops, true, 0, false);
        }
        else
        {
            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_ERROR, "ADPM_Network_Join_Request() EAP_PSK_DecodeMessage3()\r\n");
            /* Wait for timeout */
        }
    }
    else
    {
        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_ERROR, "ADPM_Network_Join_Request() EAP_PSK_DecodeMessage3()\r\n");
        /* Wait for timeout */
    }
}

static void lLBP_JoinProcessChallenge(const ADP_EXTENDED_ADDRESS *pEUI64Address,
        uint16_t lbpDataLength, uint8_t *pLbpData)
{
    uint8_t code = 0U;
    uint8_t identifier = 0U;
    uint8_t tSubfield = 0U;
    uint16_t EAPDataLength = 0U;
    uint8_t *pEAPData = NULL;

    if (EAP_PSK_DecodeMessage(lbpDataLength, pLbpData, &code, &identifier, &tSubfield,
            &EAPDataLength, &pEAPData))
    {
        /* Challenge is always a Request coming from the LBS */
        if (code == EAP_REQUEST)
        {
            /* Also only 2 kind of EAP messages are accepted as request: first and third message */
            if (tSubfield == EAP_PSK_T0)
            {
                /* This message can be received in the following states: */
                /* - LBP_STATE_BOOT_WAIT_EAPPSK_FIRST_MESSAGE: */
                /*    as normal bootstrap procedure */
                /* - LBP_STATE_BOOT_SENT_FIRST_JOINING or LBP_STATE_BOOT_SENT_EAPPSK_SECOND_JOINING or LBP_STATE_BOOT_SENT_EAPPSK_FOURTH_JOINING */
                /*    as a repetition related to a non response of the previous processing (maybe the response was lost) */
                /*    If a peer receives a valid duplicate Request for which it has already sent a Response, it MUST resend its */
                /*    original Response without reprocessing the Request. */
                /* - LBP_STATE_BOOT_WAIT_EAPPSK_THIRD_MESSAGE: */
                /*    as a repetition related to a non response of the previous processing (maybe the response was lost) */
                /*    If a peer receives a valid duplicate Request for which it has already sent a Response, it MUST resend its */
                /*    original Response without reprocessing the Request. */
                /* - LBP_STATE_BOOT_JOINED: during rekey procedure */
                if (lLBP_IsBootState(LBP_STATE_BOOT_WAIT_EAPPSK_FIRST_MESSAGE |
                        LBP_STATE_BOOT_WAIT_EAPPSK_THIRD_MESSAGE |
                        LBP_STATE_BOOT_JOINED |
                        LBP_STATE_BOOT_SENT_FIRST_JOINING |
                        LBP_STATE_BOOT_SENT_EAPPSK_SECOND_JOINING |
                        LBP_STATE_BOOT_SENT_EAPPSK_FOURTH_JOINING))
                {
                    /* Enforce the current state in order to correctly update state machine after sending this message */
                    lLBP_SetBootState(LBP_STATE_BOOT_WAIT_EAPPSK_FIRST_MESSAGE);
                    /* The function is able to detect a valid repetition */
                    lLBP_JoinProcessChallengeFirstMessage(pEUI64Address, identifier, EAPDataLength, pEAPData);
                }
                else
                {
                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_ERROR, "lLBP_JoinProcessChallenge() Drop unexpected CHALLENGE_1\r\n");
                }
            }
            else
            {
                if (tSubfield == EAP_PSK_T2)
                {
                    /* This message can be received in the following states: */
                    /* - LBP_STATE_BOOT_WAIT_EAPPSK_THIRD_MESSAGE: */
                    /*    as normal bootstrap procedure */
                    /* - LBP_STATE_BOOT_WAIT_ACCEPTED or  LBP_STATE_BOOT_SENT_EAPPSK_SECOND_JOINING or LBP_STATE_BOOT_SENT_EAPPSK_FOURTH_JOINING: */
                    /*    as a repetition related to a non response of the previous processing (maybe the response was lost) */
                    /*    If a peer receives a valid duplicate Request for which it has already sent a Response, it MUST resend its */
                    /*    original Response without reprocessing the Request. */
                    if (lLBP_IsBootState(LBP_STATE_BOOT_WAIT_EAPPSK_THIRD_MESSAGE |
                            LBP_STATE_BOOT_WAIT_ACCEPTED |
                            LBP_STATE_BOOT_SENT_EAPPSK_SECOND_JOINING |
                            LBP_STATE_BOOT_SENT_EAPPSK_FOURTH_JOINING))
                    {
                        /* Enforce the current state in order to correctly update state machine after sending this message */
                        lLBP_SetBootState(LBP_STATE_BOOT_WAIT_EAPPSK_THIRD_MESSAGE);
                        /* Hardcoded length of 22 bytes representing: the first 22 bytes of the EAP Request or Response packet used to compute the auth tag */
                        lLBP_JoinProcessChallengeThirdMessage(22U, pLbpData, identifier, EAPDataLength, pEAPData);
                    }
                    else
                    {
                        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_ERROR, "lLBP_JoinProcessChallenge() Drop unexpected CHALLENGE_2\r\n");
                    }
                }
                /* else invalid message */
            }
        }
        /* else invalid message */
    }
    /* else decode error */
}

static void lLBP_JoinProcessAcceptedEAP(uint16_t lbpDataLength, uint8_t *pLbpData)
{
    uint8_t code = 0U;
    uint8_t identifier = 0U;
    uint8_t tSubfield = 0U;
    uint16_t EAPDataLength = 0U;
    uint8_t *pEAPData = NULL;
    uint8_t activeKeyId;
    bool setResult = false;

    if (EAP_PSK_DecodeMessage(lbpDataLength, pLbpData, &code, &identifier, &tSubfield,
            &EAPDataLength, &pEAPData))
    {
        if (code == EAP_SUCCESS)
        {
            /* Set the encryption key into mac layer */
            activeKeyId = lLBP_GetActiveKeyIndex();
            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO,
                "NetworkJoin() Join / rekey process finish: Set the GMK encryption key %u into the MAC layer\r\n",
                lbpContext.groupMasterKey[activeKeyId].keyId);
            /* Check whether one or two keys have been received */
            if ((lbpContext.groupMasterKey[0].keyId != UNSET_KEY) &&
                (lbpContext.groupMasterKey[1].keyId != UNSET_KEY))
            {
                /* Both GMKs have to be set, acive index the second */
                bool setResultOut;
                if (activeKeyId == 0U)
                {
                    setResult = AdpMac_SetGroupMasterKeySync(&lbpContext.groupMasterKey[1]);
                    setResultOut = AdpMac_SetGroupMasterKeySync(&lbpContext.groupMasterKey[0]);
                    setResult = setResult && setResultOut;
                }
                else
                {
                    setResult = AdpMac_SetGroupMasterKeySync(&lbpContext.groupMasterKey[0]);
                    setResultOut = AdpMac_SetGroupMasterKeySync(&lbpContext.groupMasterKey[1]);
                    setResult = setResult && setResultOut;
                }
            }
            else if (lbpContext.groupMasterKey[0].keyId != UNSET_KEY)
            {
                /* GMK 0 has to be set */
                setResult = AdpMac_SetGroupMasterKeySync(&lbpContext.groupMasterKey[0]);
            }
            else
            {
                if (lbpContext.groupMasterKey[1].keyId != UNSET_KEY)
                {
                    /* GMK 1 has to be set */
                    setResult = AdpMac_SetGroupMasterKeySync(&lbpContext.groupMasterKey[1]);
                }
            }

            if (setResult)
            {
                SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG,
                    "NetworkJoin() Join / rekey process finish: Active key id is %u\r\n",
                    lLBP_GetActiveKeyIndex());
                /* If joining the network (not yet joined) we have to do more initialisation */
                if (!lLBP_IsJoined())
                {
                    /* A device shall initialise RC_COORD to 0x7FFF on association. */
                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG,
                        "NetworkJoin() Join process finish: Set the RcCoord into the MAC layer\r\n");
                    if (AdpMac_SetRcCoordSync(0x7FFFU))
                    {
                        /* Set the short address in the mac layer */
                        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG,
                            "NetworkJoin() Join process finish: Set the ShortAddress 0x%04X into the MAC layer\r\n",
                            lbpContext.joiningShortAddress);
                        if (lLBP_SetShortAddress(lbpContext.joiningShortAddress))
                        {
                            lbpContext.shortAddress = lbpContext.joiningShortAddress;
                            lLBP_JoinConfirm(G3_SUCCESS);
                        }
                        else
                        {
                            lLBP_JoinConfirm(G3_FAILED);
                        }
                    }
                    else
                    {
                        lLBP_JoinConfirm(G3_FAILED);
                    }
                }
                else
                {
                    /* Already joined; this is the rekey procedure; set the state to LBP_STATE_BOOT_JOINED */
                    lLBP_SetBootState(LBP_STATE_BOOT_JOINED);
                    (void) SYS_TIME_TimerDestroy(lbpContext.joinTimer);
                    lbpContext.joinTimer = SYS_TIME_HANDLE_INVALID;
                    lbpContext.rekeyTimerExpired = false;
                }
            }
        }
        else
        {
            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_ERROR,
                "ADPM_Network_Join_Request() code != EAP_SUCCESS\r\n");
            lLBP_JoinConfirm(G3_NOT_PERMITED);
        }
    }
    else
    {
        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_ERROR,
            "ADPM_Network_Join_Request() EAP_PSK_DecodeMessage\r\n");
        /* Wait for NetworkJoin timeout */
    }
}

static void lLBP_JoinProcessAcceptedConfiguration(uint16_t lbpDataLength, uint8_t *pLbpData)
{
    ADP_ADDRESS dstAddr;
    uint8_t *pMemoryBuffer = sLbpBuffer;
    uint16_t memoryBufferLength = (uint16_t)sizeof(sLbpBuffer);
    uint16_t requestLength = 0U;
    uint8_t receivedParametersMask = 0U;
    uint8_t parameterResult[10];
    uint8_t parameterResultLength = 0U;

    /* This may be rekey or key removal */
    if (lLBP_IsJoined())
    {
        (void) lLBP_ProcessParameters(lbpDataLength, pLbpData,
                &receivedParametersMask, &parameterResultLength, parameterResult);

        if (parameterResultLength <= memoryBufferLength)
        {
            (void) memcpy(pMemoryBuffer, parameterResult, parameterResultLength);
        }

        /* Encode the LBP message */
        requestLength = LBP_EncodeJoiningRequest(&lbpContext.EUI64Address, 0U, 0U,
                parameterResultLength, memoryBufferLength, pMemoryBuffer);

        dstAddr.addrSize = ADP_ADDRESS_16BITS;
        dstAddr.shortAddr = lbpContext.lbaAddress;

        sLbpCallbackType = LBP_DUMMY_CALLBACK;
        ADP_LbpRequest(&dstAddr, requestLength, pMemoryBuffer, sNsduHandle++, sMaxHops, true, 0, false);
    }

    /* If not joined, ignore this message */
}

static void lLBP_JoinProcessAccepted(uint16_t lbpDataLength, uint8_t *pLbpData)
{
    /* Check the first byte of the LBP data in order to detect */
    /* the type of the embedded message: EAP or Configuration */
    if ((pLbpData[0] & 0x01U) == 0x00U)
    {
        /* EAP message */
        /* Check state: this message can be received also when JOINED for re-key procedure */
        if (lLBP_IsBootState(LBP_STATE_BOOT_WAIT_ACCEPTED) ||
                lLBP_IsBootState(LBP_STATE_BOOT_SENT_EAPPSK_FOURTH_JOINING))
        {
            lLBP_JoinProcessAcceptedEAP(lbpDataLength, pLbpData);
        }
        else
        {
            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_ERROR,
                "lLBP_JoinProcessAccepted() Drop unexpected Accepted_EAP\r\n");
        }
    }
    else
    {
        /* Configuration message */
        if (lLBP_IsBootState(LBP_STATE_BOOT_JOINED))
        {
            lLBP_JoinProcessAcceptedConfiguration(lbpDataLength, pLbpData);
        }
        else
        {
            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_ERROR,
                "lLBP_JoinProcessAccepted() Drop unexpected Accepted_Configuration\r\n");
        }
    }
}

static void lLBP_JoinRequest(void)
{
    ADP_ADDRESS dstAddr;
    uint8_t *pMemoryBuffer = sLbpBuffer;
    uint16_t memoryBufferLength = (uint16_t)sizeof(sLbpBuffer);
    uint16_t requestLength = 0U;

    /* Reset Joining short address */
    lbpContext.joiningShortAddress = 0xFFFFU;
    /* Prepare and send the JoinRequest; no LBP data for the first request */
    requestLength = LBP_EncodeJoiningRequest(
        &lbpContext.EUI64Address, lbpContext.mediaType,
        lbpContext.disableBackupFlag, 0U, memoryBufferLength, pMemoryBuffer);

    dstAddr.addrSize = ADP_ADDRESS_16BITS;
    dstAddr.shortAddr = lbpContext.lbaAddress;

    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO,
        "Registering Network-JOIN timer: %u seconds\r\n",
        lLBP_GetMaxJoinWaitTime());

    /* Start MaxJoinWait timer */
    lbpContext.joinTimer =
        SYS_TIME_CallbackRegisterMS(lLBP_JoinTimerExpiredCallback,
        (uintptr_t)0, (uint32_t)lLBP_GetMaxJoinWaitTime() * 1000U, SYS_TIME_SINGLE);

    lbpContext.pendingConfirms++;
    lLBP_SetBootState(LBP_STATE_BOOT_SENT_FIRST_JOINING);
    sLbpCallbackType = LBP_JOIN_CALLBACK;
    ADP_LbpRequest(&dstAddr, requestLength, pMemoryBuffer, sNsduHandle++, sMaxHops, true, 0U, false);
}

static void lLBP_LeaveRequest(const ADP_EXTENDED_ADDRESS *pEUI64Address)
{
    ADP_ADDRESS dstAddr;
    uint8_t *pMemoryBuffer = sLbpBuffer;
    uint16_t memoryBufferLength = (uint16_t)sizeof(sLbpBuffer);
    uint16_t requestLength = 0U;

    requestLength = LBP_EncodeKickFromLBDRequest(pEUI64Address, memoryBufferLength, pMemoryBuffer);

    dstAddr.addrSize = ADP_ADDRESS_16BITS;
    dstAddr.shortAddr = lLBP_GetCoordShortAddress();

    sLbpCallbackType = LBP_LEAVE_CALLBACK;
    ADP_LbpRequest(&dstAddr, requestLength, pMemoryBuffer, sNsduHandle++, sMaxHops, true, 0, false);
}

static void lLBP_ForceJoined(uint16_t shortAddress, uint16_t panId, ADP_EXTENDED_ADDRESS *pEUI64Address)
{
    LBP_ADP_NETWORK_JOIN_CFM_PARAMS joinConfirm;

    lbpContext.shortAddress = shortAddress;
    lbpContext.panId = panId;
    (void) memcpy(lbpContext.EUI64Address.value, pEUI64Address->value, sizeof(ADP_EXTENDED_ADDRESS));
    lLBP_SetBootState(LBP_STATE_BOOT_JOINED);
    /* Set LBP Status on ADP */
    ADP_SetLBPStatusConnection(true);
    /* Invoke Join Confirm callback */
    if (lbpContext.lbpNotifications.adpNetworkJoinConfirm != NULL)
    {
        joinConfirm.status = (uint8_t)G3_SUCCESS;
        joinConfirm.networkAddress = shortAddress;
        joinConfirm.panId = panId;
        lbpContext.lbpNotifications.adpNetworkJoinConfirm(&joinConfirm);
    }
}

static void lLBP_AdpLbpConfirmDev(ADP_LBP_CFM_PARAMS *pLbpConfirm)
{
    if (sLbpCallbackType == LBP_JOIN_CALLBACK)
    {
        lLBP_JoinDataSendCallback(pLbpConfirm->status);
    }
    else if (sLbpCallbackType == LBP_LEAVE_CALLBACK)
    {
        lLBP_LeaveCallback(pLbpConfirm->status);
    }
    else
    {
        /* Do nothing */
    }
}

static void lLBP_AdpLbpIndicationDev(ADP_LBP_IND_PARAMS *pLbpIndication)
{
    /* The coordinator will never get here as is handled by the application using LDB messages */
    uint8_t msgType = 0U;
    ADP_EXTENDED_ADDRESS eui64Address;
    uint16_t lbpDataLength = 0U;
    uint8_t *pLbpData = NULL;
    bool result;

    /* MISRA C-2012 deviation block start */
    /* MISRA C-2012 Rule 11.8 deviated once. Deviation record ID - H3_MISRAC_2012_R_11_8_DR_1 */

    result = LBP_DecodeMessage(pLbpIndication->nsduLength, (uint8_t *)pLbpIndication->pNsdu,
                &msgType, &eui64Address, &lbpDataLength, &pLbpData);

    /* MISRA C-2012 deviation block end */

    if (result)
    {
        /* If we are not the coordinator and we are in the network and this bootstrap message is not for us */
        if (memcmp(eui64Address.value, lbpContext.EUI64Address.value,
                sizeof(ADP_EXTENDED_ADDRESS)) != 0)
        {
            if (lLBP_IsJoined())
            {
                /* LBA (agent): forward the message between server and device */
                ADP_ADDRESS dstAddr;
                uint8_t *pMemoryBuffer = sLbpBuffer;
                /* Message to be relayed to the server or to the device */
                if (msgType == LBP_JOINING)
                {
                    /* Check Src Address matches the one in LBP frame */
                    if (memcmp(eui64Address.value, pLbpIndication->srcAddr.extendedAddr.value,
                            sizeof(ADP_EXTENDED_ADDRESS)) == 0)
                    {
                        /* Check frame coming from LBD is not secured */
                        if (pLbpIndication->securityLevel == (uint8_t)MAC_WRP_SECURITY_LEVEL_NONE)
                        {
                            /* relay to the server */
                            dstAddr.addrSize = ADP_ADDRESS_16BITS;
                            dstAddr.shortAddr = lLBP_GetCoordShortAddress();
                        }
                        else
                        {
                            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG,
                                "Secured frame coming from LBD; Drop frame\r\n");
                            return;
                        }
                    }
                    else
                    {
                        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG,
                            "Source Address different from address in LBP frame; Drop frame\r\n");
                        return;
                    }
                }
                else if ((msgType == LBP_ACCEPTED) || (msgType == LBP_CHALLENGE)
                        || (msgType == LBP_DECLINE))
                {
                    /* Frame must come from a short address between 0x0000 and 0x7FFF */
                    if ((pLbpIndication->srcAddr.addrSize == ADP_ADDRESS_16BITS)
                            && (pLbpIndication->srcAddr.shortAddr < 0x8000U))
                    {
                        /* Frame must be secured */
                        /* Security level already checked for frames between 16-bit addresses */
                        /* Relay to the device */
                        dstAddr.addrSize = ADP_ADDRESS_64BITS;
                        (void) memcpy(&dstAddr.extendedAddr, &eui64Address, ADP_ADDRESS_64BITS);
                    }
                    else
                    {
                        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG,
                            "Invalid Source Address on frame to LBD; Drop frame\r\n");
                        return;
                    }
                }
                else
                {
                    /* 'T' bit and 'Code' not matching a valid frame (e.g. Joining to LBD or Challenge from LBD) */
                    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG,
                        "T and CODE fields mismatch; Drop frame\r\n");
                    return;
                }

                if (pLbpIndication->nsduLength <= sizeof(sLbpBuffer))
                {
                    (void) memcpy(pMemoryBuffer, pLbpIndication->pNsdu, pLbpIndication->nsduLength);
                }

                sLbpCallbackType = LBP_DUMMY_CALLBACK;
                ADP_LbpRequest(&dstAddr, pLbpIndication->nsduLength, pMemoryBuffer, sNsduHandle++, sMaxHops, true, 0U, false);
            }
            else
            {
                SRV_LOG_REPORT_Message(SRV_LOG_REPORT_ERROR,
                    "Received message for invalid EUI64; Drop it!\r\n");
            }
        }
        else
        {
            /* Only end-device will be handled here */
            /* Frame must come from a short address between 0x0000 and 0x7FFF */
            if ((pLbpIndication->srcAddr.addrSize == ADP_ADDRESS_16BITS) &&
                    (pLbpIndication->srcAddr.shortAddr < 0x8000U))
            {
                if (lLBP_IsJoined())
                {
                    if (msgType == LBP_CHALLENGE)
                    {
                        /* Reuse function from join */
                        lLBP_JoinProcessChallenge(&eui64Address, lbpDataLength, pLbpData);
                    }
                    else if (msgType == LBP_ACCEPTED)
                    {
                        /* Reuse function from join */
                        lLBP_JoinProcessAccepted(lbpDataLength, pLbpData);
                    }
                    else if (msgType == LBP_DECLINE)
                    {
                        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_ERROR,
                            "LBP_DECLINE: Not implemented\r\n");
                    }
                    else if (msgType == LBP_KICK_TO_LBD)
                    {
                        lLBP_KickNotify();
                    }
                    else if (msgType == LBP_KICK_FROM_LBD)
                    {
                        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_ERROR,
                            "LBP_KICK_FROM_LBD: Never should be here\r\n");
                    }
                    else if (msgType == LBP_JOINING)
                    {
                        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_ERROR,
                            "LBP_JOINING: Never should be here\r\n");
                    }
                    else
                    {
                        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_ERROR,
                            "Unsupported LBP message: %u\r\n", msgType);
                    }
                }
                else
                {
                    if (msgType == LBP_CHALLENGE)
                    {
                        lLBP_JoinProcessChallenge(&eui64Address, lbpDataLength, pLbpData);
                    }
                    else if (msgType == LBP_ACCEPTED)
                    {
                        lLBP_JoinProcessAccepted(lbpDataLength, pLbpData);
                    }
                    else if (msgType == LBP_DECLINE)
                    {
                        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO,
                            "LBP_DECLINE\r\n");
                        lLBP_JoinConfirm(G3_NOT_PERMITED);
                    }
                    else if (msgType == LBP_KICK_TO_LBD)
                    {
                        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_ERROR,
                            "LBP_KICK_TO_LBD: Not joined!\r\n");
                    }
                    else if (msgType == LBP_KICK_FROM_LBD)
                    {
                        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_ERROR,
                            "LBP_KICK_FROM_LBD: Never should be here\r\n");
                    }
                    else if (msgType == LBP_JOINING)
                    {
                        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_ERROR,
                            "LBP_JOINING: Never should be here\r\n");
                    }
                    else
                    {
                        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_ERROR,
                            "Unsupported LBP message: %u\r\n", msgType);
                    }
                }
            }
            else
            {
                SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG,
                    "Invalid Source Address on frame to LBD; Drop frame\r\n");
                return;
            }
        }
    }
    /* else decoding error */
}

/* ***************************************************************************** */
/* ***************************************************************************** */
/* Section: Interface Function Definitions */
/* ***************************************************************************** */
/* ***************************************************************************** */

void LBP_InitDev(void)
{
    ADP_NOTIFICATIONS_TO_LBP notifications;

    notifications.lbpConfirm = lLBP_AdpLbpConfirmDev;
    notifications.lbpIndication = lLBP_AdpLbpIndicationDev;

    ADP_SetNotificationsToLbp(&notifications);

    (void) memset(&sRandP, 0, sizeof(sRandP));
    (void) memset(&lbpContext, 0, sizeof(lbpContext));
    lbpContext.shortAddress = 0xFFFF;
    lbpContext.groupMasterKey[0].keyId = UNSET_KEY;
    lbpContext.groupMasterKey[1].keyId = UNSET_KEY;
    EAP_PSK_Initialize(&sEapPskKey, &lbpContext.pskContext);

    lbpContext.joinTimer = SYS_TIME_HANDLE_INVALID;
    lbpContext.joinTimerExpired = false;
    lbpContext.rekeyTimerExpired = false;
    lbpContext.kickTimer = SYS_TIME_HANDLE_INVALID;
    lbpContext.kickTimerExpired = false;

    sMaxHops = lLBP_GetAdpMaxHops();
    lLBP_SetDeviceTypeDev();
}

void LBP_TasksDev(void)
{
    if (lbpContext.joinTimerExpired)
    {
        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "Join Timer Expired\r\n");
        lbpContext.joinTimerExpired = false;
        lbpContext.joinTimer = SYS_TIME_HANDLE_INVALID;

        lLBP_JoinConfirm(G3_TIMEOUT);
    }

    if (lbpContext.kickTimerExpired)
    {
        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "Kick Timer Expired; Reseting Stack..\r\n");
        lbpContext.kickTimerExpired = false;
        lbpContext.kickTimer = SYS_TIME_HANDLE_INVALID;

        /* Reset Stack */
        ADP_ResetRequest();
        lLBP_SetBootState(LBP_STATE_BOOT_NOT_JOINED);
        lbpContext.shortAddress = 0xFFFFU;
        lbpContext.panId = 0xFFFFU;
        (void) lLBP_SetShortAddress(0xFFFFU);
        lLBP_SetPanId(0xFFFFU);

        /* Set LBP Status on ADP */
        ADP_SetLBPStatusConnection(false);

        if (lbpContext.lbpNotifications.adpNetworkLeaveIndication != NULL)
        {
            lbpContext.lbpNotifications.adpNetworkLeaveIndication();
        }
    }

    if (lbpContext.rekeyTimerExpired)
    {
        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "Rekey Timer Expired\r\n");
        lbpContext.rekeyTimerExpired = false;
        lbpContext.joinTimer = SYS_TIME_HANDLE_INVALID;

        lLBP_ClearEapContext();
        if (lLBP_IsJoined())
        {
            /* set back automate state */
            lLBP_ForceJoinStatus(true);
        }
        else
        {
            /* never should be here but check anyway */
            lLBP_ForceJoinStatus(false);
        }
    }
}

void LBP_SetNotificationsDev(LBP_NOTIFICATIONS_DEV *pNotifications)
{
    if (pNotifications != NULL)
    {
        lbpContext.lbpNotifications = *pNotifications;
    }
}

void LBP_SetParamDev(uint32_t attributeId, uint16_t attributeIndex,
    uint8_t attributeLen, const uint8_t *pAttributeValue,
    LBP_SET_PARAM_CONFIRM *pSetConfirm)
{
    pSetConfirm->attributeId = attributeId;
    pSetConfirm->attributeIndex = attributeIndex;
    pSetConfirm->status = LBP_STATUS_INVALID_LENGTH;

    switch ((LBP_ATTRIBUTE)attributeId)
    {
    case LBP_IB_IDP:
        if ((attributeLen == LBP_NETWORK_ACCESS_ID_SIZE_P_ARIB) ||
                (attributeLen == LBP_NETWORK_ACCESS_ID_SIZE_P_CENELEC_FCC) ||
                (attributeLen == 0U)) /* 0 to invalidate value */
        {
            sIdP.size = attributeLen;
            if (attributeLen != 0U)
            {
                (void) memcpy(sIdP.value, pAttributeValue, attributeLen);
            }

            pSetConfirm->status = LBP_STATUS_OK;
        }
        break;

    case LBP_IB_RANDP:
        if (attributeLen == (uint8_t)sizeof(sRandP.value))
        {
            (void) memcpy(sRandP.value, pAttributeValue, sizeof(sRandP.value));
            pSetConfirm->status = LBP_STATUS_OK;
        }
        break;

    case LBP_IB_PSK:
        if (attributeLen == (uint8_t)sizeof(sEapPskKey.value))
        {
            (void) memcpy(sEapPskKey.value, pAttributeValue, sizeof(sEapPskKey.value));
            EAP_PSK_Initialize(&sEapPskKey, &lbpContext.pskContext);
            pSetConfirm->status = LBP_STATUS_OK;
        }
        break;

    default:
        /* Unknown LBP parameter */
        pSetConfirm->status = LBP_STATUS_UNSUPPORTED_PARAMETER;
        break;
    }
}

void LBP_ForceRegister(ADP_EXTENDED_ADDRESS *pEUI64Address,
    uint16_t shortAddress, uint16_t panId, ADP_GROUP_MASTER_KEY *pGMK)
{
    /* Set the information in G3 stack */
    (void) AdpMac_SetExtendedAddressSync(pEUI64Address);
    lLBP_SetPanId(panId);
    (void) lLBP_SetShortAddress(shortAddress);
    (void) AdpMac_SetGroupMasterKeySync(pGMK);

    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG,
        "LBP_ForceRegister: ShortAddr: 0x%04X PanId: 0x%04X\r\n",
        shortAddress, panId);
    SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG,
        pEUI64Address->value, 8,
        "LBP_ForceRegister: EUI64: ");
    SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG,
        pGMK->key, 16, "LBP_ForceRegister: Key: ");
    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG,
        "LBP_ForceRegister: KeyIndex: %u\r\n", pGMK->keyId);

    /* Force state in LBP */
    lLBP_ForceJoined(shortAddress, panId, pEUI64Address);
}

void LBP_AdpNetworkJoinRequest(uint16_t panId, uint16_t lbaAddress, uint8_t mediaType)
{
    ADP_EXTENDED_ADDRESS extendedAddress;

    SRV_LOG_REPORT_Message(SRV_LOG_REPORT_INFO,
        "AdpNetworkJoinRequest() PanID %04X Lba %04X MediaType %02X\r\n",
        panId, lbaAddress, mediaType);

    /* Get Available MAC layers */
    lbpContext.availableMacLayers = ADP_GetAvailableMacLayers();

    if (lbpContext.bootstrapState != LBP_STATE_BOOT_JOINED)
    {
        /* Before starting the network read the Extended address from the MAC layer */
        if (AdpMac_GetExtendedAddressSync(&extendedAddress))
        {
            /* Remember network info */
            lLBP_ForceJoinStatus(false); /* Not joined */
            lLBP_SetPanId(panId);
            lbpContext.panId = panId;
            lbpContext.lbaAddress = lbaAddress;
            if (lbpContext.availableMacLayers == ADP_AVAILABLE_MAC_PLC)
            {
                lbpContext.mediaType = (uint8_t)MAC_WRP_MEDIA_TYPE_REQ_PLC_BACKUP_RF;
                lbpContext.disableBackupFlag = 0;
            }
            else if (lbpContext.availableMacLayers == ADP_AVAILABLE_MAC_RF)
            {
                lbpContext.mediaType = (uint8_t)MAC_WRP_MEDIA_TYPE_REQ_RF_BACKUP_PLC;
                lbpContext.disableBackupFlag = 0;
            }
            else
            {
                /* Hybrid Profile */
                lbpContext.mediaType = mediaType;
                lbpContext.disableBackupFlag = 1;
            }
            (void) memcpy(lbpContext.EUI64Address.value, extendedAddress.value, ADP_ADDRESS_64BITS);

            SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG,
                lbpContext.EUI64Address.value, 8, "ExtendedAddress: ");

            /* join the network */
            lLBP_JoinRequest();
        }
        else
        {
            lLBP_JoinConfirm(G3_FAILED);
        }
    }
    else
    {
        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_ERROR,
            "AdpNetworkJoinRequest() Network already joined\r\n");
    }
}

void LBP_AdpNetworkLeaveRequest(void)
{
    if (lbpContext.bootstrapState == LBP_STATE_BOOT_JOINED)
    {
        lLBP_LeaveRequest(&lbpContext.EUI64Address);
    }
    else
    {
        if (lbpContext.lbpNotifications.adpNetworkLeaveConfirm != NULL)
        {
            lbpContext.lbpNotifications.adpNetworkLeaveConfirm((uint8_t)G3_INVALID_REQUEST);
        }
    }
}
