/*******************************************************************************
  LBP Protocol Encode/Decode

  Company:
    Microchip Technology Inc.

  File Name:
    lbp_encode_decode.c

  Summary:
    LBP Protocol Encode/Decode implementation.

  Description:
    This file contains the functions to Encode and Decode the LBP frames.
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
#include "adp_api_types.h"
#include "lbp_encode_decode.h"
#include "eap_psk.h"

// *****************************************************************************
// *****************************************************************************
// Section: Interface Function Definitions
// *****************************************************************************
// *****************************************************************************

uint16_t LBP_EncodeKickFromLBDRequest(
    const ADP_EXTENDED_ADDRESS *pEUI64Address,
    uint16_t bufferLength,
    uint8_t *pMessageBuffer)
{
    uint16_t encodedLength = 0U;

    /* Check the message buffer size */
    if (bufferLength >= ADP_ADDRESS_64BITS + 2U)
    {
        /* Start message encoding */
        /* DisableBackupFlag not set in Kick frames, MediaType set to 0x0 */
        pMessageBuffer[0] = (LBP_KICK_FROM_LBD << 4);
        pMessageBuffer[1] = 0; /* Transaction id is reserved */

        (void) memcpy(&pMessageBuffer[2], pEUI64Address->value, ADP_ADDRESS_64BITS);

        encodedLength = ADP_ADDRESS_64BITS + 2U;
    }

    return encodedLength;
}

uint16_t LBP_EncodeKickToLBD(
    const ADP_EXTENDED_ADDRESS *pEUI64Address,
    uint16_t bufferLength,
    uint8_t *pMessageBuffer)
{
    uint16_t encodedLength = 0U;

    /* Check the message buffer size */
    if (bufferLength >= ADP_ADDRESS_64BITS + 2U)
    {
        /* Start message encoding */
        /* Media Type set to 0x0 on Kick */
        pMessageBuffer[0] = (LBP_KICK_TO_LBD << 4);
        pMessageBuffer[1] = 0U; /* Transaction id is reserved */

        (void) memcpy(&pMessageBuffer[2], pEUI64Address->value, ADP_ADDRESS_64BITS);

        encodedLength = ADP_ADDRESS_64BITS + 2U;
    }

    return encodedLength;
}

uint16_t LBP_EncodeJoiningRequest(
    const ADP_EXTENDED_ADDRESS *pEUI64Address,
    uint8_t mediaType,
    uint8_t disableBackupMedium,
    uint16_t lbpDataLength,
    uint16_t bufferLength,
    uint8_t *pMessageBuffer)
{
    uint16_t encodedLength = 0U;

    /* Check the message buffer size */
    if (bufferLength >= (lbpDataLength + ADP_ADDRESS_64BITS + 2U))
    {
        /* As the bootstrapping data is already in the message buffer, */
        /* move it to its place after the A_LDB field */
        (void) memmove(&pMessageBuffer[ADP_ADDRESS_64BITS + 2U], pMessageBuffer, lbpDataLength);

        /* Start message encoding */
        pMessageBuffer[0] = (LBP_JOINING << 4) | (mediaType << 3) | (disableBackupMedium << 2);
        pMessageBuffer[1] = 0U; /* Transaction id is reserved */

        (void) memcpy(&pMessageBuffer[2], pEUI64Address->value, ADP_ADDRESS_64BITS);

        encodedLength = lbpDataLength + ADP_ADDRESS_64BITS + 2U;
    }

    return encodedLength;
}

uint16_t LBP_EncodeChallengeRequest(
    const ADP_EXTENDED_ADDRESS *pEUI64Address,
    uint8_t mediaType,
    uint8_t disableBackupMedium,
    uint16_t lbpDataLength,
    uint16_t bufferLength,
    uint8_t *pMessageBuffer)
{
    uint16_t encodedLength = 0;

    /* Check the message buffer size */
    if (bufferLength >= (lbpDataLength + ADP_ADDRESS_64BITS + 2U))
    {
        /* As the bootstrapping data is already in the message buffer, */
        /* move it to its place after the A_LDB field */
        (void) memmove(&pMessageBuffer[ADP_ADDRESS_64BITS + 2U], pMessageBuffer, lbpDataLength);

        /* Start message encoding */
        pMessageBuffer[0] = (LBP_CHALLENGE << 4) | (mediaType << 3) | (disableBackupMedium << 2);
        pMessageBuffer[1] = 0U; /* Transaction id is reserved */

        (void) memcpy(&pMessageBuffer[2], pEUI64Address->value, ADP_ADDRESS_64BITS);

        encodedLength = lbpDataLength + ADP_ADDRESS_64BITS + 2U;
    }

    return encodedLength;
}

uint16_t LBP_EncodeAcceptedRequest(
    const ADP_EXTENDED_ADDRESS *pEUI64Address,
    uint8_t mediaType,
    uint8_t disableBackupMedium,
    uint16_t lbpDataLength,
    uint16_t bufferLength,
    uint8_t *pMessageBuffer)
{
    uint16_t encodedLength = 0U;

    /* Check the message buffer size */
    if (bufferLength >= (lbpDataLength + ADP_ADDRESS_64BITS + 2U))
    {
        /* As the bootstrapping data is already in the message buffer, */
        /* move it to its place after the A_LDB field */
        (void) memmove(&pMessageBuffer[8 + 2], pMessageBuffer, lbpDataLength);

        /* Start message encoding */
        pMessageBuffer[0] = (LBP_ACCEPTED << 4) | (mediaType << 3) | (disableBackupMedium << 2);
        pMessageBuffer[1] = 0U; /* Transaction id is reserved */

        (void) memcpy(&pMessageBuffer[2], pEUI64Address->value, ADP_ADDRESS_64BITS);

        encodedLength = lbpDataLength + ADP_ADDRESS_64BITS + 2U;
    }

    return encodedLength;
}

uint16_t LBP_EncodeDecline(
    const ADP_EXTENDED_ADDRESS *pEUI64Address,
    uint8_t mediaType,
    uint8_t disableBackupMedium,
    uint8_t EAPIdentifier,
    uint16_t bufferLength,
    uint8_t *pMessageBuffer)
{
    uint16_t encodedLength = 0U;

    if (bufferLength >= (ADP_ADDRESS_64BITS + 2U))
    {
        pMessageBuffer[0] = (LBP_DECLINE  << 4) | (mediaType << 3) | (disableBackupMedium << 2);
        pMessageBuffer[1] = 0; /* Transaction id is reserved */

        (void) memcpy(&pMessageBuffer[2], pEUI64Address->value, ADP_ADDRESS_64BITS);

        encodedLength = ADP_ADDRESS_64BITS + 2U;

        /* encode EAP Failure */
        encodedLength += EAP_PSK_EncodeEAPFailure(
            EAPIdentifier,
            bufferLength - encodedLength,
            (pMessageBuffer + encodedLength));
    }

    return encodedLength;
}

bool LBP_DecodeMessage(
    uint16_t bufferLength,
    uint8_t *pMessageBuffer,
    uint8_t *pMessageType,
    ADP_EXTENDED_ADDRESS *pEUI64Address,
    uint16_t *pLbpDataLength,
    uint8_t **pLbpData)
{
    bool retVal = false;

    if (bufferLength >= (ADP_ADDRESS_64BITS + 2U)) {
        *pMessageType = pMessageBuffer[0] >> 4;

        (void) memcpy(&pEUI64Address->value, &pMessageBuffer[2], ADP_ADDRESS_64BITS);

        *pLbpData = &pMessageBuffer[ADP_ADDRESS_64BITS + 2U];
        *pLbpDataLength = bufferLength - (ADP_ADDRESS_64BITS + 2U);

        retVal = true;
    }

    return retVal;
}
