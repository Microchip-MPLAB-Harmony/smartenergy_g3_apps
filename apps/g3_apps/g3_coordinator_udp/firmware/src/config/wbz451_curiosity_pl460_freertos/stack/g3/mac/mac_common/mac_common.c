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
#include "system/time/sys_time.h"
#include "mac_common.h"
#include "../mac_plc/mac_plc_mib.h"

/* Time control variables */
static uint64_t previousCounter64 = 0;
static uint32_t auxMsCounter = 0;
static uint32_t currentMsCounter = 0;
static uint32_t currentSecondCounter = 0;

MAC_COMMON_MIB macMibCommon;

static const MAC_COMMON_MIB macMibCommonDefaults = {
    0xFFFF, // panId
    {{0}}, // extendedAddress
    0xFFFF, // shortAddress
    false, // promiscuousMode
    {{0}}, // keyTable
    0xFFFF, // rcCoord: set RC_COORD to its maximum value of 0xFFFF
    255, // posTableEntryTtl
    120, // posRecentEntryThreshold
    false, // coordinator
};

void MAC_COMMON_Init(void)
{
    macMibCommon = macMibCommonDefaults;
}

void MAC_COMMON_Reset(void)
{
    macMibCommon = macMibCommonDefaults;
}

static MAC_STATUS _macPibGetExtendedAddress(MAC_PIB_VALUE *pibValue)
{
    pibValue->length = sizeof(macMibCommon.extendedAddress);
    memcpy(pibValue->value, &macMibCommon.extendedAddress, pibValue->length);
    return MAC_STATUS_SUCCESS;
}

static MAC_STATUS _macPibSetExtendedAddress(const MAC_PIB_VALUE *pibValue)
{
    MAC_STATUS status = MAC_STATUS_SUCCESS;
    if (pibValue->length == sizeof(macMibCommon.extendedAddress))
    {
        memcpy(&macMibCommon.extendedAddress, pibValue->value, pibValue->length);
    }
    else
    {
        status = MAC_STATUS_INVALID_PARAMETER;
    }
    return status;
}

static MAC_STATUS _macPibGetPanId(MAC_PIB_VALUE *pibValue)
{
    pibValue->length = sizeof(macMibCommon.panId);
    memcpy(pibValue->value, &macMibCommon.panId, pibValue->length);
    return MAC_STATUS_SUCCESS;
}

static MAC_STATUS _macPibSetPanId(const MAC_PIB_VALUE *pibValue)
{
    MAC_STATUS status = MAC_STATUS_SUCCESS;
    if (pibValue->length == sizeof(macMibCommon.panId))
    {
        memcpy(&macMibCommon.panId, pibValue->value, pibValue->length);
    }
    else
    {
        status = MAC_STATUS_INVALID_PARAMETER;
    }
    return status;
}

static MAC_STATUS _macPibGetPromiscuousMode(MAC_PIB_VALUE *pibValue)
{
    pibValue->length = 1;
    pibValue->value[0] = (macMibCommon.promiscuousMode) ? 1 : 0;
    return MAC_STATUS_SUCCESS;
}

static MAC_STATUS _macPibSetPromiscuousMode(const MAC_PIB_VALUE *pibValue)
{
    MAC_STATUS status = MAC_STATUS_SUCCESS;
    uint8_t u8Value;
    memcpy(&u8Value, pibValue->value, sizeof(u8Value));
    if ((pibValue->length == sizeof(u8Value)) && (u8Value <= 1))
    {
        macMibCommon.promiscuousMode = u8Value != 0;
    }
    else
    {
        status = MAC_STATUS_INVALID_PARAMETER;
    }
    return status;
}

static MAC_STATUS _macPibGetShortAddress(MAC_PIB_VALUE *pibValue)
{
    pibValue->length = sizeof(macMibCommon.shortAddress);
    memcpy(pibValue->value, &macMibCommon.shortAddress, pibValue->length);
    return MAC_STATUS_SUCCESS;
}

static MAC_STATUS _macPibSetShortAddress(const MAC_PIB_VALUE *pibValue)
{
    MAC_STATUS status = MAC_STATUS_SUCCESS;
    if (pibValue->length == sizeof(macMibCommon.shortAddress))
    {
        memcpy(&macMibCommon.shortAddress, pibValue->value, pibValue->length);
    }
    else
    {
        status = MAC_STATUS_INVALID_PARAMETER;
    }
    return status;
}

static MAC_STATUS _macPibGetRcCoord(MAC_PIB_VALUE *pibValue)
{
    pibValue->length = sizeof(macMibCommon.rcCoord);
    memcpy(pibValue->value, &macMibCommon.rcCoord, pibValue->length);
    return MAC_STATUS_SUCCESS;
}

static MAC_STATUS _macPibSetRcCoord(const MAC_PIB_VALUE *pibValue)
{
    MAC_STATUS status = MAC_STATUS_SUCCESS;
    if (pibValue->length == sizeof(macMibCommon.rcCoord))
    {
        memcpy(&macMibCommon.rcCoord, pibValue->value, pibValue->length);
    }
    else
    {
        status = MAC_STATUS_INVALID_PARAMETER;
    }
    return status;
}

static MAC_STATUS _macPibSetKeyTable(uint16_t index, const MAC_PIB_VALUE *pibValue)
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
                memcpy(&macMibCommon.keyTable[index].key, pibValue->value, MAC_SECURITY_KEY_LENGTH);
                macMibCommon.keyTable[index].valid = true;
            }
            status = MAC_STATUS_SUCCESS;
        }
        else if (pibValue->length == 0)
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

static MAC_STATUS _macPibGetPOSTableEntryTtl(MAC_PIB_VALUE *pibValue)
{
    pibValue->length = sizeof(macMibCommon.posTableEntryTtl);
    memcpy(pibValue->value, &macMibCommon.posTableEntryTtl, pibValue->length);
    return MAC_STATUS_SUCCESS;
}

static MAC_STATUS _macPibSetPOSTableEntryTtl(const MAC_PIB_VALUE *pibValue)
{
    MAC_STATUS status = MAC_STATUS_SUCCESS;
    if (pibValue->length == sizeof(macMibCommon.posTableEntryTtl))
    {
        memcpy(&macMibCommon.posTableEntryTtl, pibValue->value, pibValue->length);
    }
    else
    {
        status = MAC_STATUS_INVALID_PARAMETER;
    }
    return status;
}

static MAC_STATUS _macPibGetPOSRecentEntryThreshold(MAC_PIB_VALUE *pibValue)
{
    pibValue->length = sizeof(macMibCommon.posRecentEntryThreshold);
    memcpy(pibValue->value, &macMibCommon.posRecentEntryThreshold, pibValue->length);
    return MAC_STATUS_SUCCESS;
}

static MAC_STATUS _macPibSetPOSRecentEntryThreshold(const MAC_PIB_VALUE *pibValue)
{
    MAC_STATUS status = MAC_STATUS_SUCCESS;
    if (pibValue->length == sizeof(macMibCommon.posRecentEntryThreshold)) {
        memcpy(&macMibCommon.posRecentEntryThreshold, pibValue->value, pibValue->length);
    }
    else {
        status = MAC_STATUS_INVALID_PARAMETER;
    }
    return status;
}

MAC_STATUS MAC_COMMON_GetRequestSync(MAC_COMMON_PIB_ATTRIBUTE attribute, uint16_t index, MAC_PIB_VALUE *pibValue)
{
    MAC_STATUS status;
    bool isArray = (attribute == MAC_COMMON_PIB_KEY_TABLE);
    if (!isArray && (index != 0))
    {
        status = MAC_STATUS_INVALID_INDEX;
    }
    else
    {
        switch (attribute)
        {
        case MAC_COMMON_PIB_PAN_ID:
            status = _macPibGetPanId(pibValue);
            break;
        case MAC_COMMON_PIB_PROMISCUOUS_MODE:
            status = _macPibGetPromiscuousMode(pibValue);
            break;
        case MAC_COMMON_PIB_SHORT_ADDRESS:
            status = _macPibGetShortAddress(pibValue);
            break;
        case MAC_COMMON_PIB_RC_COORD:
            status = _macPibGetRcCoord(pibValue);
            break;
        case MAC_COMMON_PIB_EXTENDED_ADDRESS:
            status = _macPibGetExtendedAddress(pibValue);
            break;
        case MAC_COMMON_PIB_POS_TABLE_ENTRY_TTL:
            status = _macPibGetPOSTableEntryTtl(pibValue);
            break;
        case MAC_COMMON_PIB_POS_RECENT_ENTRY_THRESHOLD:
            status = _macPibGetPOSRecentEntryThreshold(pibValue);
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
        pibValue->length = 0;
    }
    return status;
}

MAC_STATUS MAC_COMMON_SetRequestSync(MAC_COMMON_PIB_ATTRIBUTE attribute, uint16_t index, const MAC_PIB_VALUE *pibValue)
{
    MAC_STATUS status;
    bool isArray = (attribute == MAC_COMMON_PIB_KEY_TABLE);
    if (!isArray && (index != 0))
    {
        status = MAC_STATUS_INVALID_INDEX;
    }
    else
    {
        switch (attribute)
        {
        case MAC_COMMON_PIB_PAN_ID:
            status = _macPibSetPanId(pibValue);
            if (status == MAC_STATUS_SUCCESS)
            {
                /* Ignore result, as it depends on availability of PLC interface, which may be unavailable */
                MAC_PLC_MIB_SetAttributeSync(attribute, index, pibValue);
            }
            break;
        case MAC_COMMON_PIB_PROMISCUOUS_MODE:
            status = _macPibSetPromiscuousMode(pibValue);
            if (status == MAC_STATUS_SUCCESS)
            {
                /* Ignore result, as it depends on availability of PLC interface, which may be unavailable */
                MAC_PLC_MIB_SetAttributeSync(attribute, index, pibValue);
            }
            break;
        case MAC_COMMON_PIB_SHORT_ADDRESS:
            status = _macPibSetShortAddress(pibValue);
            if (status == MAC_STATUS_SUCCESS)
            {
                /* Ignore result, as it depends on availability of PLC interface, which may be unavailable */
                MAC_PLC_MIB_SetAttributeSync(attribute, index, pibValue);
            }
            break;
        case MAC_COMMON_PIB_RC_COORD:
            status = _macPibSetRcCoord(pibValue);
            if (status == MAC_STATUS_SUCCESS)
            {
                /* Ignore result, as it depends on availability of PLC interface, which may be unavailable */
                MAC_PLC_MIB_SetAttributeSync(attribute, index, pibValue);
            }
            break;
        case MAC_COMMON_PIB_EXTENDED_ADDRESS:
            status = _macPibSetExtendedAddress(pibValue);
            if (status == MAC_STATUS_SUCCESS)
            {
                /* Ignore result, as it depends on availability of PLC interface, which may be unavailable */
                MAC_PLC_MIB_SetAttributeSync(attribute, index, pibValue);
            }
            break;
        case MAC_COMMON_PIB_POS_TABLE_ENTRY_TTL:
            status = _macPibSetPOSTableEntryTtl(pibValue);
            if (status == MAC_STATUS_SUCCESS)
            {
                /* Ignore result, as it depends on availability of PLC interface, which may be unavailable */
                MAC_PLC_MIB_SetAttributeSync(attribute, index, pibValue);
            }
            break;
        case MAC_COMMON_PIB_POS_RECENT_ENTRY_THRESHOLD:
            status = _macPibSetPOSRecentEntryThreshold(pibValue);
            if (status == MAC_STATUS_SUCCESS)
            {
                /* Ignore result as it depends on availability of PLC interface, which may be unavailable */
                MAC_PLC_MIB_SetAttributeSync(attribute, index, pibValue);
            }
            break;
        case MAC_COMMON_PIB_KEY_TABLE:
            status = _macPibSetKeyTable(index, pibValue);
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
    uint64_t diffCounter64, currentCounter64;
    uint32_t elapsedMs;

    /* Get current timer counter */
    currentCounter64 = SYS_TIME_Counter64Get();
    /* Diff with previous */
    diffCounter64 = currentCounter64 - previousCounter64;
    /* Diff in Ms */
    elapsedMs = SYS_TIME_CountToMS((uint32_t)diffCounter64);
    /* Update Ms counter */
    currentMsCounter += elapsedMs;
    /* Update previous counter for next computation */
    previousCounter64 += SYS_TIME_MSToCount(elapsedMs);

    /* Check whether seconds counter has to be updated */
    if ((currentMsCounter - auxMsCounter) > 1000)
    {
        /* Assume no more than one second passed */
        /* This function is called every few program loops */
        currentSecondCounter++;
        auxMsCounter += 1000;
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
