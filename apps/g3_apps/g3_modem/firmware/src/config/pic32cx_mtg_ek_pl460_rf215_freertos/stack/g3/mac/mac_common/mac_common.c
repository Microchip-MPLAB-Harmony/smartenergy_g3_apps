/*******************************************************************************
  G3 MAC Common Source File

  Company:
    Microchip Technology Inc.

  File Name:
    mac_common.c

  Summary:
    G3 MAC Common Source File

  Description:
    This file contains implementation of the API
    to be used by MAC Wrapper when accessing G3 MAC layers.
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
#include "system/time/sys_time.h"
#include "osal/osal.h"
#include "mac_common.h"
#include "../mac_plc/mac_plc_mib.h"

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Variables
// *****************************************************************************
// *****************************************************************************

/* Time control variables */
static uint64_t previousCounter64 = 0;
static uint32_t auxMsCounter = 0;
static uint32_t currentMsCounter = 0;
static uint32_t currentSecondCounter = 0;
static OSAL_SEM_HANDLE_TYPE msCounterSemaphoreID = NULL;

MAC_COMMON_MIB macMibCommon;

static const MAC_COMMON_MIB macMibCommonDefaults = {
    0xFFFF, // rcCoord: set RC_COORD to its maximum value of 0xFFFF
    0xFFFF, // panId
    0xFFFF, // shortAddress
    255, // posTableEntryTtl
    120, // posRecentEntryThreshold
    {{0}}, // extendedAddress
    {{{0}, false}}, // keyTable
    false, // promiscuousMode
    false, // coordinator
};

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Functions
// *****************************************************************************
// *****************************************************************************

static MAC_STATUS lMAC_COMMON_PibGetExtendedAddress(MAC_PIB_VALUE *pibValue)
{
    pibValue->length = (uint8_t)sizeof(macMibCommon.extendedAddress);
    (void) memcpy(pibValue->value, macMibCommon.extendedAddress.address, pibValue->length);
    return MAC_STATUS_SUCCESS;
}

static MAC_STATUS lMAC_COMMON_PibSetExtendedAddress(const MAC_PIB_VALUE *pibValue)
{
    MAC_STATUS status = MAC_STATUS_SUCCESS;
    if (pibValue->length == sizeof(macMibCommon.extendedAddress))
    {
        (void) memcpy(macMibCommon.extendedAddress.address, pibValue->value, pibValue->length);
    }
    else
    {
        status = MAC_STATUS_INVALID_PARAMETER;
    }
    return status;
}

static MAC_STATUS lMAC_COMMON_PibGetPanId(MAC_PIB_VALUE *pibValue)
{
    pibValue->length = (uint8_t)sizeof(macMibCommon.panId);
    (void) memcpy((void *)pibValue->value, (void *)&macMibCommon.panId, pibValue->length);
    return MAC_STATUS_SUCCESS;
}

static MAC_STATUS lMAC_COMMON_PibSetPanId(const MAC_PIB_VALUE *pibValue)
{
    MAC_STATUS status = MAC_STATUS_SUCCESS;
    if (pibValue->length == sizeof(macMibCommon.panId))
    {
        (void) memcpy((void *)&macMibCommon.panId, (const void *)pibValue->value, pibValue->length);
    }
    else
    {
        status = MAC_STATUS_INVALID_PARAMETER;
    }
    return status;
}

static MAC_STATUS lMAC_COMMON_PibGetPromiscuousMode(MAC_PIB_VALUE *pibValue)
{
    pibValue->length = 1;
    pibValue->value[0] = (macMibCommon.promiscuousMode) ? 1U : 0U;
    return MAC_STATUS_SUCCESS;
}

static MAC_STATUS lMAC_COMMON_PibSetPromiscuousMode(const MAC_PIB_VALUE *pibValue)
{
    MAC_STATUS status = MAC_STATUS_SUCCESS;
    if ((pibValue->length == sizeof(macMibCommon.promiscuousMode)) && (pibValue->value[0] <= 1U))
    {
        macMibCommon.promiscuousMode = pibValue->value[0] != 0U;
    }
    else
    {
        status = MAC_STATUS_INVALID_PARAMETER;
    }
    return status;
}

static MAC_STATUS lMAC_COMMON_PibGetShortAddress(MAC_PIB_VALUE *pibValue)
{
    pibValue->length = (uint8_t)sizeof(macMibCommon.shortAddress);
    (void) memcpy((void *)pibValue->value, (void *)&macMibCommon.shortAddress, pibValue->length);
    return MAC_STATUS_SUCCESS;
}

static MAC_STATUS lMAC_COMMON_PibSetShortAddress(const MAC_PIB_VALUE *pibValue)
{
    MAC_STATUS status = MAC_STATUS_SUCCESS;
    if (pibValue->length == sizeof(macMibCommon.shortAddress))
    {
        (void) memcpy((void *)&macMibCommon.shortAddress, (const void *)pibValue->value, pibValue->length);
    }
    else
    {
        status = MAC_STATUS_INVALID_PARAMETER;
    }
    return status;
}

static MAC_STATUS lMAC_COMMON_PibGetRcCoord(MAC_PIB_VALUE *pibValue)
{
    pibValue->length = (uint8_t)sizeof(macMibCommon.rcCoord);
    (void) memcpy((void *)pibValue->value, (void *)&macMibCommon.rcCoord, pibValue->length);
    return MAC_STATUS_SUCCESS;
}

static MAC_STATUS lMAC_COMMON_PibSetRcCoord(const MAC_PIB_VALUE *pibValue)
{
    MAC_STATUS status = MAC_STATUS_SUCCESS;
    if (pibValue->length == sizeof(macMibCommon.rcCoord))
    {
        (void) memcpy((void *)&macMibCommon.rcCoord, (const void *)pibValue->value, pibValue->length);
    }
    else
    {
        status = MAC_STATUS_INVALID_PARAMETER;
    }
    return status;
}

static MAC_STATUS lMAC_COMMON_PibSetKeyTable(uint16_t index, const MAC_PIB_VALUE *pibValue)
{
    MAC_STATUS status;
    if (index < MAC_KEY_TABLE_ENTRIES)
    {
        if (pibValue->length == MAC_SECURITY_KEY_LENGTH)
        {
            if (!macMibCommon.keyTable[index].valid ||
                (memcmp(&macMibCommon.keyTable[index].key, pibValue->value, MAC_SECURITY_KEY_LENGTH) != 0))
            {
                // Set value if invalid entry or different key
                (void) memcpy(&macMibCommon.keyTable[index].key, pibValue->value, MAC_SECURITY_KEY_LENGTH);
                macMibCommon.keyTable[index].valid = true;
            }
            status = MAC_STATUS_SUCCESS;
        }
        else if (pibValue->length == 0U)
        {
            macMibCommon.keyTable[index].valid = false;
            status = MAC_STATUS_SUCCESS;
        }
        else
        {
            status = MAC_STATUS_INVALID_PARAMETER;
        }
    }
    else
    {
        status = MAC_STATUS_INVALID_INDEX;
    }
    return status;
}

static MAC_STATUS lMAC_COMMON_PibGetPOSTableEntryTtl(MAC_PIB_VALUE *pibValue)
{
    pibValue->length = (uint8_t)sizeof(macMibCommon.posTableEntryTtl);
    pibValue->value[0] = macMibCommon.posTableEntryTtl;
    return MAC_STATUS_SUCCESS;
}

static MAC_STATUS lMAC_COMMON_PibSetPOSTableEntryTtl(const MAC_PIB_VALUE *pibValue)
{
    MAC_STATUS status = MAC_STATUS_SUCCESS;
    if (pibValue->length == sizeof(macMibCommon.posTableEntryTtl))
    {
        macMibCommon.posTableEntryTtl = pibValue->value[0];
    }
    else
    {
        status = MAC_STATUS_INVALID_PARAMETER;
    }
    return status;
}

static MAC_STATUS lMAC_COMMON_PibGetPOSRecentEntryThreshold(MAC_PIB_VALUE *pibValue)
{
    pibValue->length = (uint8_t)sizeof(macMibCommon.posRecentEntryThreshold);
    pibValue->value[0] = macMibCommon.posRecentEntryThreshold;
    return MAC_STATUS_SUCCESS;
}

static MAC_STATUS lMAC_COMMON_PibSetPOSRecentEntryThreshold(const MAC_PIB_VALUE *pibValue)
{
    MAC_STATUS status = MAC_STATUS_SUCCESS;
    if (pibValue->length == sizeof(macMibCommon.posRecentEntryThreshold))
    {
        macMibCommon.posRecentEntryThreshold = pibValue->value[0];
    }
    else
    {
        status = MAC_STATUS_INVALID_PARAMETER;
    }
    return status;
}

// *****************************************************************************
// *****************************************************************************
// Section: MAC Common Interface Routines
// *****************************************************************************
// *****************************************************************************

void MAC_COMMON_Init(void)
{
    macMibCommon = macMibCommonDefaults;

    if (msCounterSemaphoreID == NULL)
    {
        /* Create semaphore. It is used to protect ms counter calculation. */
        OSAL_RESULT semResult = OSAL_SEM_Create(&msCounterSemaphoreID, OSAL_SEM_TYPE_BINARY, 1, 1);
        if (semResult != OSAL_RESULT_SUCCESS)
        {
            if (msCounterSemaphoreID != NULL)
            {
                (void) OSAL_SEM_Delete(&msCounterSemaphoreID);
            }
        }
    }
}

void MAC_COMMON_Reset(void)
{
    macMibCommon = macMibCommonDefaults;
}

MAC_STATUS MAC_COMMON_GetRequestSync(MAC_COMMON_PIB_ATTRIBUTE attribute, uint16_t index, MAC_PIB_VALUE *pibValue)
{
    MAC_STATUS status;
    bool isArray = (attribute == MAC_COMMON_PIB_KEY_TABLE);
    if (!isArray && (index != 0U))
    {
        status = MAC_STATUS_INVALID_INDEX;
    }
    else
    {
        switch (attribute)
        {
        case MAC_COMMON_PIB_PAN_ID:
            status = lMAC_COMMON_PibGetPanId(pibValue);
            break;
        case MAC_COMMON_PIB_PROMISCUOUS_MODE:
            status = lMAC_COMMON_PibGetPromiscuousMode(pibValue);
            break;
        case MAC_COMMON_PIB_SHORT_ADDRESS:
            status = lMAC_COMMON_PibGetShortAddress(pibValue);
            break;
        case MAC_COMMON_PIB_RC_COORD:
            status = lMAC_COMMON_PibGetRcCoord(pibValue);
            break;
        case MAC_COMMON_PIB_EXTENDED_ADDRESS:
            status = lMAC_COMMON_PibGetExtendedAddress(pibValue);
            break;
        case MAC_COMMON_PIB_POS_TABLE_ENTRY_TTL:
            status = lMAC_COMMON_PibGetPOSTableEntryTtl(pibValue);
            break;
        case MAC_COMMON_PIB_POS_RECENT_ENTRY_THRESHOLD:
            status = lMAC_COMMON_PibGetPOSRecentEntryThreshold(pibValue);
            break;
        case MAC_COMMON_PIB_KEY_TABLE:
            status = MAC_STATUS_UNAVAILABLE_KEY;
            break;

        default:
            status = MAC_STATUS_UNSUPPORTED_ATTRIBUTE;
            break;
        }
    }

    if (status != MAC_STATUS_SUCCESS)
    {
        pibValue->length = 0U;
    }
    return status;
}

MAC_STATUS MAC_COMMON_SetRequestSync(MAC_COMMON_PIB_ATTRIBUTE attribute, uint16_t index, const MAC_PIB_VALUE *pibValue)
{
    MAC_STATUS status;
    bool isArray = (attribute == MAC_COMMON_PIB_KEY_TABLE);
    if (!isArray && (index != 0U))
    {
        status = MAC_STATUS_INVALID_INDEX;
    }
    else
    {
        switch (attribute)
        {
        case MAC_COMMON_PIB_PAN_ID:
            status = lMAC_COMMON_PibSetPanId(pibValue);
            if (status == MAC_STATUS_SUCCESS)
            {
                /* Ignore result, as it depends on availability of PLC interface, which may be unavailable */
                (void) MAC_PLC_MIB_SetAttributeSync(attribute, index, pibValue);
            }
            break;
        case MAC_COMMON_PIB_PROMISCUOUS_MODE:
            status = lMAC_COMMON_PibSetPromiscuousMode(pibValue);
            if (status == MAC_STATUS_SUCCESS)
            {
                /* Ignore result, as it depends on availability of PLC interface, which may be unavailable */
                (void) MAC_PLC_MIB_SetAttributeSync(attribute, index, pibValue);
            }
            break;
        case MAC_COMMON_PIB_SHORT_ADDRESS:
            status = lMAC_COMMON_PibSetShortAddress(pibValue);
            if (status == MAC_STATUS_SUCCESS)
            {
                /* Ignore result, as it depends on availability of PLC interface, which may be unavailable */
                (void) MAC_PLC_MIB_SetAttributeSync(attribute, index, pibValue);
            }
            break;
        case MAC_COMMON_PIB_RC_COORD:
            status = lMAC_COMMON_PibSetRcCoord(pibValue);
            if (status == MAC_STATUS_SUCCESS)
            {
                /* Ignore result, as it depends on availability of PLC interface, which may be unavailable */
                (void) MAC_PLC_MIB_SetAttributeSync(attribute, index, pibValue);
            }
            break;
        case MAC_COMMON_PIB_EXTENDED_ADDRESS:
            status = lMAC_COMMON_PibSetExtendedAddress(pibValue);
            if (status == MAC_STATUS_SUCCESS)
            {
                /* Ignore result, as it depends on availability of PLC interface, which may be unavailable */
                (void) MAC_PLC_MIB_SetAttributeSync(attribute, index, pibValue);
            }
            break;
        case MAC_COMMON_PIB_POS_TABLE_ENTRY_TTL:
            status = lMAC_COMMON_PibSetPOSTableEntryTtl(pibValue);
            if (status == MAC_STATUS_SUCCESS)
            {
                /* Ignore result, as it depends on availability of PLC interface, which may be unavailable */
                (void) MAC_PLC_MIB_SetAttributeSync(attribute, index, pibValue);
            }
            break;
        case MAC_COMMON_PIB_POS_RECENT_ENTRY_THRESHOLD:
            status = lMAC_COMMON_PibSetPOSRecentEntryThreshold(pibValue);
            if (status == MAC_STATUS_SUCCESS)
            {
                /* Ignore result as it depends on availability of PLC interface, which may be unavailable */
                (void) MAC_PLC_MIB_SetAttributeSync(attribute, index, pibValue);
            }
            break;
        case MAC_COMMON_PIB_KEY_TABLE:
            status = lMAC_COMMON_PibSetKeyTable(index, pibValue);
            break;

        default:
            status = MAC_STATUS_UNSUPPORTED_ATTRIBUTE;
            break;
        }
    }
    return status;
}

uint32_t MAC_COMMON_GetMsCounter(void)
{
    uint64_t currentCounter64;
    int64_t diffCounter64;
    uint32_t elapsedMs = 0;

    if (msCounterSemaphoreID != NULL)
    {
        /* Suspend task if this function is being executed in another thread */
        (void) OSAL_SEM_Pend(&msCounterSemaphoreID, OSAL_WAIT_FOREVER);
    }

    /* Get current timer counter */
    currentCounter64 = SYS_TIME_Counter64Get();

    /* Diff with previous */
    diffCounter64 = (int64_t)currentCounter64 - (int64_t)previousCounter64;

    if (diffCounter64 > 0)
    {
        /* Diff in Ms */
        elapsedMs = SYS_TIME_CountToMS((uint32_t)diffCounter64);
    }

    /* Update Ms counter */
    currentMsCounter += elapsedMs;
    /* Update previous counter for next computation */
    previousCounter64 += SYS_TIME_MSToCount(elapsedMs);

    /* Check whether seconds counter has to be updated */
    if ((currentMsCounter - auxMsCounter) >= 1000U)
    {
        /* Assume no more than one second passed */
        /* This function is called every few program loops */
        currentSecondCounter++;
        auxMsCounter += 1000U;
    }

    if (msCounterSemaphoreID != NULL)
    {
        /* Post semaphore to resume task in another thread blocked at the
         * beginning of this function */
        (void) OSAL_SEM_Post(&msCounterSemaphoreID);
    }

    return currentMsCounter;
}

bool MAC_COMMON_TimeIsPast(int32_t timeValue)
{
    return (((int32_t)(MAC_COMMON_GetMsCounter()) - timeValue) > 0);
}

uint32_t MAC_COMMON_GetSecondsCounter(void)
{
    return currentSecondCounter;
}

bool MAC_COMMON_TimeIsPastSeconds(int32_t timeValue)
{
    return (((int32_t)currentSecondCounter - timeValue) > 0);
}

/*******************************************************************************
 End of File
*/
