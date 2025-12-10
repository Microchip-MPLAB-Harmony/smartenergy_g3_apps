/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    eap_psk.c

  Summary:
    LBP EAP-PSK Protocol implementation.

  Description:
    The EAP-PSK protocol implements security functions required during
    Bootstrapping of G3 devices.
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
#include "eap_psk.h"
#include "service/security/aes_wrapper.h"
#include "service/security/cipher_wrapper.h"
#include "service/log_report/srv_log_report.h"


// *****************************************************************************
// *****************************************************************************
// Section: Macro Definitions
// *****************************************************************************
// *****************************************************************************

#define member_size(type, member) sizeof(((type *)0)->member)

/* IANA allocated value */
#define EAP_PSK_IANA_TYPE           0x2FU
/* T Subfield mask */
#define EAP_T_SUBFIELD_MASK         0xC0U
/* Protected Channel Result mask */
#define EAP_P_CHANNEL_RESULT_MASK   0xC0U

// *****************************************************************************
// *****************************************************************************
// Section: Interface Function Definitions
// *****************************************************************************
// *****************************************************************************

void EAP_PSK_Initialize(EAP_PSK_KEY *pKey, EAP_PSK_CONTEXT *pPskContext)
{
    uint8_t block[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t res[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    (void) memset(pPskContext, 0, sizeof(EAP_PSK_CONTEXT));

    /* Set key and trigger the AES ECB */
    AES_Wrapper_SetEncryptEcbKey(pKey->value);
    AES_Wrapper_EncryptEcb(block, res);

    /* Xor with c1 = "1" */
    res[15] ^= 0x01U;

    /* Generate AK */
    AES_Wrapper_EncryptEcb(res, pPskContext->ak.value);

    /* Generate AK */
    /* Xor with c1 = "2" */
    res[15] ^= 0x03U; /* 3 instead of 2 because it has been already xor'ed with 1 and we want to get back the initial value */

    /* Generate KDK */
    AES_Wrapper_EncryptEcb(res, pPskContext->kdk.value);
}

void EAP_PSK_InitializeTEKMSK(
    EAP_PSK_RAND *pRandP,
    EAP_PSK_CONTEXT *pPskContext
    )
{
    uint8_t res[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t idx;

    /* Set key and trigger the AES ECB */
    AES_Wrapper_SetEncryptEcbKey(pPskContext->kdk.value);
    AES_Wrapper_EncryptEcb(pRandP->value, res);

    /* Xor with c1 = "1" */
    res[15] ^= 0x01U;

    /* Generate TEK */
    AES_Wrapper_EncryptEcb(res, pPskContext->tek.value);

    /* Undo xor to generate next block*/
    res[15] ^= 0x01U;

    /* Generate MSK with subsequent blocks */
    for (idx = 0U; idx < 4U; idx++)
    {
        /* Xor with c1 = "(idx + 2)" */
        res[15] ^= (idx + 2U);
        /* Get next block for MSK at corresponding index */
        AES_Wrapper_EncryptEcb(res, &pPskContext->msk.value[idx << 4]);
        /* Undo xor to generate next block*/
        res[15] ^= (idx + 2U);
    }
}

bool EAP_PSK_DecodeMessage(
    uint16_t messageLength,
    uint8_t *pMessage,
    uint8_t *pCode,
    uint8_t *pIdentifier,
    uint8_t *pTSubfield,
    uint16_t *pEAPDataLength,
    uint8_t **pEAPData
    )
{
    bool retVal = false;
    uint16_t eapMessageLength = 0U;

    if (messageLength >= 4U)
    {
        *pCode = pMessage[0];
        *pIdentifier = pMessage[1];
        eapMessageLength = (((uint16_t)pMessage[2] << 8) | pMessage[3]);

        /* A message with the Length field set to a value larger than the number of received octets MUST be silently discarded. */
        retVal = (eapMessageLength <= messageLength);

        if (retVal && (eapMessageLength >= 6U))
        {
            *pTSubfield = (pMessage[5] & EAP_T_SUBFIELD_MASK);
            *pEAPData = &pMessage[6];
            *pEAPDataLength = eapMessageLength - 6U; /* 4 for the size of the header + 2 eaptype and T field */

            retVal = (pMessage[4] == EAP_PSK_IANA_TYPE);
        }
    }

    return retVal;
}

bool EAP_PSK_DecodeMessage1(
    uint16_t messageLength,
    uint8_t *pMessage,
    EAP_PSK_RAND *pRandS,
    EAP_PSK_NETWORK_ACCESS_ID_S *pIdS
    )
{
    bool retVal = false;

    /* Check the length of the message */
    if (messageLength >= sizeof(pRandS->value))
    {
        uint16_t sizeIdS;

        (void) memcpy(pRandS->value, pMessage, sizeof(pRandS->value));

        sizeIdS = messageLength - (uint16_t)sizeof(pRandS->value);
        pIdS->size = (uint8_t)sizeIdS;
        (void) memcpy(pIdS->value, &pMessage[sizeof(pRandS->value)], pIdS->size);
        retVal = true;
    }

    return retVal;
}

uint16_t EAP_PSK_EncodeMessage2(
    EAP_PSK_CONTEXT *pPskContext,
    uint8_t identifier,
    EAP_PSK_RAND *pRandS,
    EAP_PSK_RAND *pRandP,
    EAP_PSK_NETWORK_ACCESS_ID_S *pIdS,
    EAP_PSK_NETWORK_ACCESS_ID_P *pIdP,
    uint16_t memoryBufferLength,
    uint8_t *pMemoryBuffer
    )
{
    uint16_t encodeSize = 0U;
    int32_t ret;
    uint8_t macP[16];
    uint8_t seed[(2U * member_size(EAP_PSK_NETWORK_ACCESS_ID_P, value))
        + (2U * member_size(EAP_PSK_RAND, value))];
    uint16_t seedUsedSize = 0U;

    /* check the size of the buffer */
    if (memoryBufferLength >= 90U)
    {
        /* Compute first MacP = CMAC-AES-128(AK, IdP||IdS||RandS||RandP) */
        (void) memcpy(seed, pIdP->value, pIdP->size);
        seedUsedSize += pIdP->size;

        (void) memcpy(&seed[seedUsedSize], pIdS->value, pIdS->size);
        seedUsedSize += pIdS->size;

        (void) memcpy(&seed[seedUsedSize], pRandS->value, sizeof(pRandS->value));
        seedUsedSize += (uint16_t)sizeof(pRandS->value);

        (void) memcpy(&seed[seedUsedSize], pRandP->value, sizeof(pRandP->value));
        seedUsedSize += (uint16_t)sizeof(pRandP->value);

        ret = CIPHER_Wrapper_AesCmacDirect(seed, seedUsedSize, macP, pPskContext->ak.value);
        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "CIPHER_Wrapper_AesCmacDirect returned %d\r\n", ret);
        (void)(ret);

        SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, pPskContext->ak.value, sizeof(pPskContext->ak.value), "AK ");
        SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, seed, seedUsedSize, "Seed ");
        SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, macP, sizeof(macP), "MacP ");

        /* Encode the EAP header; length field will be set at the end of the block */
        pMemoryBuffer[0] = EAP_RESPONSE;
        pMemoryBuffer[1] = identifier;
        pMemoryBuffer[4] = EAP_PSK_IANA_TYPE;
        pMemoryBuffer[5] = EAP_PSK_T1;
        encodeSize = 6U;

        /* EAP message: add PSK fields */
        (void) memcpy(&pMemoryBuffer[encodeSize], pRandS->value, sizeof(pRandS->value));
        encodeSize += (uint16_t)sizeof(pRandS->value);
        (void) memcpy(&pMemoryBuffer[encodeSize], pRandP->value, sizeof(pRandP->value));
        encodeSize += (uint16_t)sizeof(pRandP->value);
        (void) memcpy(&pMemoryBuffer[encodeSize], macP, sizeof(macP));
        encodeSize += (uint16_t)sizeof(macP);
        (void) memcpy(&pMemoryBuffer[encodeSize], pIdP->value, pIdP->size);
        encodeSize += pIdP->size;

        /* Update the EAP header length field */
        pMemoryBuffer[2] = (uint8_t)(encodeSize >> 8);
        pMemoryBuffer[3] = (uint8_t)encodeSize;
    }

    return encodeSize;
}

bool EAP_PSK_DecodeMessage3(
    uint16_t messageLength,
    uint8_t *pMessage,
    EAP_PSK_CONTEXT *pPskContext,
    uint16_t headerLength,
    uint8_t *pHeader,
    EAP_PSK_RAND *pRandS,
    uint32_t *pNonce,
    uint8_t *pPChannelResult,
    uint16_t *pPChannelDataLength,
    uint8_t **pPChannelData
    )
{
    bool retVal = false;
    uint8_t macS[16];
    int32_t ret;
    uint8_t seed[(member_size(EAP_PSK_NETWORK_ACCESS_ID_P, value))
        + (member_size(EAP_PSK_RAND, value))];
    uint16_t seedUsedSize = 0U;

    if (messageLength >= 59U)
    {
        (void) memcpy(pRandS->value, pMessage, sizeof(pRandS->value));

        /* Verify MacS: MAC_S = CMAC-AES-128(AK, IdS||RandP) */
        (void) memcpy(seed, pPskContext->idS.value, pPskContext->idS.size);
        seedUsedSize += pPskContext->idS.size;

        (void) memcpy(&seed[seedUsedSize], pPskContext->randP.value, sizeof(pPskContext->randP.value));
        seedUsedSize += (uint16_t)sizeof(pPskContext->randP.value);

        ret = CIPHER_Wrapper_AesCmacDirect(seed, seedUsedSize, macS, pPskContext->ak.value);
        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "CIPHER_Wrapper_AesCmacDirect returned %d\r\n", ret);
        (void)(ret);

        if (memcmp(macS, &pMessage[sizeof(pRandS->value)], sizeof(macS)) == 0)
        {
            uint8_t auxNonce[16];
            uint8_t *pAuxNonce = &pMessage[32];
            uint8_t *pTag = &pMessage[36];
            uint8_t *pProtectedData = &pMessage[52];
            uint16_t protectedDataLength = messageLength - 52U;

            /* Prepare 16 bytes nonce */
            /* Nonce is big endian */
            (void)memset(auxNonce, 0, sizeof(auxNonce));
            auxNonce[12] = pAuxNonce[0];
            auxNonce[13] = pAuxNonce[1];
            auxNonce[14] = pAuxNonce[2];
            auxNonce[15] = pAuxNonce[3];

            /* The protected data is the 22 bytes header of the EAP message. */
            /* The G3 specifies a slightly modified EAP header but in the same time */
            /* the specification requires to compute the authentication tag over the */
            /* on the original EAP header */
            /* So we change the header to make it "EAP compliant", we compute the */
            /* auth tag and then we change back the header */

            /* Right shift Code field with 2 bits as indicated in the EAP specification */
            pHeader[0] >>= 2;

            SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, pPskContext->tek.value, sizeof(pPskContext->tek.value), "TEK: ");
            SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, auxNonce, 16U, "Nonce/IV: ");
            SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, pHeader, headerLength, "Header: ");
            SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, pProtectedData, protectedDataLength, "Data-encr: ");
            SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, pTag, 16U, "Tag: ");

            /* Decrypt P-CHANNEL */
            /* P-CHANNEL uses the TEK key */
            /* CIPHER_WRAPPER_RETURN_GOOD is returned if the input tag matches that for the decrypted message */
            if (CIPHER_WRAPPER_RETURN_GOOD == CIPHER_Wrapper_AesEaxDecrypt(
                                                  pProtectedData,          /* the message buffer        */
                                                  protectedDataLength,     /* and its length in bytes   */
                                                  auxNonce,                /* the initialization vector */
                                                  16U,                     /* and its length in bytes   */
                                                  pHeader,                 /* the header buffer         */
                                                  headerLength,            /* and its length in bytes   */
                                                  pTag,                    /* the buffer for the tag    */
                                                  16U,                     /* and its length in bytes   */
                                                  pPskContext->tek.value)) /* the key                   */
            {
                /* Retrieve protected parameters */
                *pPChannelResult = ((pProtectedData[0] & EAP_P_CHANNEL_RESULT_MASK) >> 6);

                *pPChannelDataLength = protectedDataLength - 1U;
                *pPChannelData = &pProtectedData[1];

                ((uint8_t *)pNonce)[0] = pAuxNonce[3];
                ((uint8_t *)pNonce)[1] = pAuxNonce[2];
                ((uint8_t *)pNonce)[2] = pAuxNonce[1];
                ((uint8_t *)pNonce)[3] = pAuxNonce[0];

                retVal = true;
            }
            else
            {
                SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "Decode FAILED\r\n");
            }

            SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, pProtectedData, protectedDataLength, "Data-plain: ");

            /* Fix EAP header: left shift Code field with 2 bits as indicated in the G3 specification */
            pHeader[0] <<= 2;
        }
        else
        {
            /* Cannot verify MacS */
            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_ERROR, "Cannot verify MAC_S\r\n");
        }
    }

    return retVal;
}

uint16_t EAP_PSK_EncodeMessage4(
    EAP_PSK_CONTEXT *pPskContext,
    uint8_t identifier,
    EAP_PSK_RAND *pRandS,
    uint32_t nonce,
    uint8_t PChannelResult,
    uint16_t PChannelDataLength,
    uint8_t *pPChannelData,
    uint16_t memoryBufferLength,
    uint8_t *pMemoryBuffer
    )
{
    uint16_t encodeSize = 0U;
    uint8_t *pTag = NULL;
    uint8_t auxNonce[16];
    uint8_t *pProtectedData = NULL;
    uint16_t protectedDataLength = 0U;

    /* Check the size of the buffer */
    if (memoryBufferLength >= 43U + PChannelDataLength)
    {
        /* encode the EAP header; length field will be set at the end of the block */
        pMemoryBuffer[0] = EAP_RESPONSE;
        pMemoryBuffer[1] = identifier;
        pMemoryBuffer[4] = EAP_PSK_IANA_TYPE;
        pMemoryBuffer[5] = EAP_PSK_T3;
        encodeSize = 6U;

        /* EAP message: add PSK fields */
        (void) memcpy(&pMemoryBuffer[encodeSize], pRandS->value, sizeof(pRandS->value));
        encodeSize += (uint16_t)sizeof(pRandS->value);

        /* Nonce is big endian */
        (void) memset(auxNonce, 0, sizeof(auxNonce));
        auxNonce[12] = pMemoryBuffer[encodeSize++] = (uint8_t)(nonce >> 24);
        auxNonce[13] = pMemoryBuffer[encodeSize++] = (uint8_t)(nonce >> 16);
        auxNonce[14] = pMemoryBuffer[encodeSize++] = (uint8_t)(nonce >> 8);
        auxNonce[15] = pMemoryBuffer[encodeSize++] = (uint8_t)nonce;

        /* Tag will be set after data protection */
        pTag = &pMemoryBuffer[encodeSize];
        encodeSize += 16U;

        /* Protected data */
        pProtectedData = &pMemoryBuffer[encodeSize];
        if (PChannelDataLength > 0U)
        {
            /* result / extension = 1 */
            pProtectedData[protectedDataLength] = (PChannelResult << 6) | 0x20U;
            protectedDataLength++;

            (void) memcpy(&pProtectedData[protectedDataLength], pPChannelData, PChannelDataLength);
            protectedDataLength += PChannelDataLength;
        }
        else
        {
            /* result / extension = 0 */
            pProtectedData[protectedDataLength] = (PChannelResult << 6);
            protectedDataLength++;
        }

        encodeSize += protectedDataLength;

        /* Update the EAP header length field */
        pMemoryBuffer[2] = (uint8_t)(encodeSize >> 8);
        pMemoryBuffer[3] = (uint8_t)encodeSize;

        /* The protected data is the 22 bytes header of the EAP message. */
        /* The G3 specifies a slightly modified EAP header but in the same time */
        /* the specification requires to compute the authentication tag over the */
        /* the original EAP header */
        /* So we change the header to make it "EAP compliant", we compute the */
        /* auth tag and then we change back the header */

        /* Right shift Code field with 2 bits as indicated in the EAP specification */
        pMemoryBuffer[0] >>= 2;

        SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, pPskContext->tek.value, sizeof(pPskContext->tek.value), "TEK: ");
        SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, auxNonce, 16U, "Nonce/IV: ");
        SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, pMemoryBuffer, 22U, "Header: ");
        SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, pProtectedData, protectedDataLength, "Data-plain: ");

        /* Protect data in P-Channel (just the last byte) */
        /* P-CHANNEL uses the TEK key */
        if (CIPHER_WRAPPER_RETURN_GOOD != CIPHER_Wrapper_AesEaxEncrypt(
                                              pProtectedData,          /* the message buffer        */
                                              protectedDataLength,     /* and its length in bytes   */
                                              auxNonce,                /* the initialization vector */
                                              16U,                     /* and its length in bytes   */
                                              pMemoryBuffer,           /* the header buffer         */
                                              22U,                     /* and its length in bytes   */
                                              pTag,                    /* the buffer for the tag    */
                                              16U,                     /* and its length in bytes   */
                                              pPskContext->tek.value)) /* the key                   */
        {
            encodeSize = 0U;
        }

        SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, pProtectedData, protectedDataLength, "Data-encr: ");
        SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, pTag, 16U, "Tag: ");

        /* Fix EAP header: left shift Code field with 2 bits as indicated in the G3 specification */
        pMemoryBuffer[0] <<= 2;
    }

    return encodeSize;
}

uint16_t EAP_PSK_EncodeMessage1(
    uint8_t identifier,
    const EAP_PSK_RAND *pRandS,
    const EAP_PSK_NETWORK_ACCESS_ID_S *pIdS,
    uint16_t memoryBufferLength,
    uint8_t *pMemoryBuffer
    )
{
    uint16_t encodeSize = 0U;

    /* Check the size of the buffer */
    if (memoryBufferLength >= 30U)
    {
        /* Encode the EAP header; length field will be set at the end of the block */
        pMemoryBuffer[0] = EAP_REQUEST;
        pMemoryBuffer[1] = identifier;
        pMemoryBuffer[4] = EAP_PSK_IANA_TYPE;
        pMemoryBuffer[5] = EAP_PSK_T0;
        encodeSize = 6U;

        /* EAP message: add PSK fields */
        (void) memcpy(&pMemoryBuffer[encodeSize], pRandS->value, sizeof(pRandS->value));
        encodeSize += (uint16_t)sizeof(pRandS->value);
        (void) memcpy(&pMemoryBuffer[encodeSize], pIdS->value, pIdS->size);
        encodeSize += pIdS->size;

        /* Update the EAP header length field */
        pMemoryBuffer[2] = (uint8_t)(encodeSize >> 8);
        pMemoryBuffer[3] = (uint8_t)encodeSize;
    }

    return encodeSize;
}

bool EAP_PSK_DecodeMessage2(
    bool aribBand,
    uint16_t messageLength,
    uint8_t *pMessage,
    EAP_PSK_CONTEXT *pPskContext,
    EAP_PSK_NETWORK_ACCESS_ID_S *pIdS,
    EAP_PSK_RAND *pRandS,
    EAP_PSK_RAND *pRandP
    )
{
    bool retVal = false;
    uint8_t minMsgLength;
    uint8_t macP[16];
    uint8_t expectedMacP[16];
    EAP_PSK_NETWORK_ACCESS_ID_P idP;
    int32_t ret;

    if (aribBand)
    {
        minMsgLength = 49U;
    }
    else
    {
        minMsgLength = 56U;
    }

    if (messageLength >= minMsgLength)
    {
        /* EAP header already removed */
        if (aribBand)
        {
            /* In ARIB, ID_P can be range between 8 and 36 bytes, its length depends on the message length */
            uint16_t sizeIdP = messageLength - 48U;
            idP.size = (uint8_t)sizeIdP;
            /* Maximum size is checked */
            if (idP.size > LBP_NETWORK_ACCESS_ID_MAX_SIZE_P)
            {
                idP.size = LBP_NETWORK_ACCESS_ID_MAX_SIZE_P;
            }
        }
        else
        {
            idP.size = 8U;
        }

        uint8_t seed[(LBP_NETWORK_ACCESS_ID_MAX_SIZE_P + LBP_NETWORK_ACCESS_ID_MAX_SIZE_S) + (2U * member_size(EAP_PSK_RAND, value))];
        uint8_t seedSize = idP.size + pIdS->size + (2U * member_size(EAP_PSK_RAND, value));

        uint16_t decodeOffset = 0U;
        uint16_t seedOffset = 0U;

        (void) memcpy(pRandS->value, &pMessage[decodeOffset], sizeof(pRandS->value));
        decodeOffset += (uint16_t)sizeof(pRandS->value);

        (void) memcpy(pRandP->value, &pMessage[decodeOffset], sizeof(pRandP->value));
        decodeOffset += (uint16_t)sizeof(pRandP->value);

        (void) memcpy(macP, &pMessage[decodeOffset], sizeof(macP));
        decodeOffset += (uint16_t)sizeof(macP);

        (void) memcpy(idP.value, &pMessage[decodeOffset], idP.size);
        decodeOffset += idP.size;

        /* Compute MacP = CMAC-AES-128(AK, IdP||IdS||RandS||RandP) */
        (void) memcpy(seed, idP.value, idP.size);
        seedOffset += idP.size;

        (void) memcpy(&seed[seedOffset], pIdS->value, pIdS->size);
        seedOffset += pIdS->size;

        (void) memcpy(&seed[seedOffset], pRandS->value, sizeof(pRandS->value));
        seedOffset += (uint16_t)sizeof(pRandS->value);

        (void) memcpy(&seed[seedOffset], pRandP->value, sizeof(pRandP->value));
        seedOffset += (uint16_t)sizeof(pRandP->value);

        ret = CIPHER_Wrapper_AesCmacDirect(seed, seedSize, expectedMacP, pPskContext->ak.value);
        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "CIPHER_Wrapper_AesCmacDirect returned %d\r\n", ret);

        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "decodeOffset: %d, seedOffset: %d, seedSize: %d, idP.size: %d, pIdS->size: %d\n\r\n", decodeOffset, seedOffset, seedSize,
                idP.size, pIdS->size);

        retVal = (memcmp(expectedMacP, macP, sizeof(macP)) == 0);

        (void)(ret);
        (void)(decodeOffset);
    }

    return retVal;
}

/**********************************************************************************************************************/

/** The EAP_PSK_EncodeMessage3 primitive is used to encode the third EAP-PSK message (type 2)
 ***********************************************************************************************************************
 *
 **********************************************************************************************************************/
uint16_t EAP_PSK_EncodeMessage3(
    EAP_PSK_CONTEXT *pPskContext,
    uint8_t identifier,
    EAP_PSK_RAND *pRandS,
    EAP_PSK_RAND *pRandP,
    EAP_PSK_NETWORK_ACCESS_ID_S *pIdS,
    uint32_t nonce,
    uint8_t PChannelResult,
    uint16_t PChannelDataLength,
    uint8_t *pPChannelData,
    uint16_t memoryBufferLength,
    uint8_t *pMemoryBuffer
    )
{
    uint16_t encodeSize = 0U;
    uint8_t macS[16];
    uint8_t *pProtectedData = NULL;
    uint16_t protectedDataLength = 0U;
    uint8_t *pTag = NULL;
    uint8_t auxNonce[16];
    int32_t ret;
    uint8_t seed[LBP_NETWORK_ACCESS_ID_MAX_SIZE_S + member_size(EAP_PSK_RAND, value)];
    uint8_t seedSize = pIdS->size + (uint8_t)member_size(EAP_PSK_RAND, value);

    /* check the size of the buffer */
    if (memoryBufferLength >= 59U + PChannelDataLength)
    {
        (void) memcpy(seed, pIdS->value, pIdS->size);
        (void) memcpy(&seed[pIdS->size], pRandP->value, sizeof(pRandP->value));

        ret = CIPHER_Wrapper_AesCmacDirect(seed, seedSize, macS, pPskContext->ak.value);
        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "CIPHER_Wrapper_AesCmacDirect returned %d\r\n", ret);
        (void)(ret);

        /* Encode the EAP header; length field will be set at the end of the block */
        pMemoryBuffer[0] = EAP_REQUEST;
        pMemoryBuffer[1] = identifier;
        pMemoryBuffer[4] = EAP_PSK_IANA_TYPE;
        pMemoryBuffer[5] = EAP_PSK_T2;
        encodeSize = 6U;

        /* EAP message: add PSK fields */
        (void) memcpy(&pMemoryBuffer[encodeSize], pRandS->value, sizeof(pRandS->value));
        encodeSize += (uint16_t)sizeof(pRandS->value);
        (void) memcpy(&pMemoryBuffer[encodeSize], macS, sizeof(macS));
        encodeSize += (uint16_t)sizeof(macS);

        /* Prepare P-Channel content */
        /* Nonce is big endian */
        (void) memset(auxNonce, 0, sizeof(auxNonce));
        auxNonce[12] = pMemoryBuffer[encodeSize++] = (uint8_t)(nonce >> 24);
        auxNonce[13] = pMemoryBuffer[encodeSize++] = (uint8_t)(nonce >> 16);
        auxNonce[14] = pMemoryBuffer[encodeSize++] = (uint8_t)(nonce >> 8);
        auxNonce[15] = pMemoryBuffer[encodeSize++] = (uint8_t)nonce;

        /* tag will be added later */
        pTag = &pMemoryBuffer[encodeSize];
        encodeSize += 16U;

        /* protected data */
        pProtectedData = &pMemoryBuffer[encodeSize];
        if (PChannelDataLength > 0U)
        {
            /* result / extension = 1 */
            pProtectedData[protectedDataLength] = (PChannelResult << 6) | 0x20U;
            protectedDataLength++;

            (void) memcpy(&pProtectedData[protectedDataLength], pPChannelData, PChannelDataLength);
            protectedDataLength += PChannelDataLength;
        }
        else
        {
            /* result / extension = 0 */
            pProtectedData[protectedDataLength] = (PChannelResult << 6);
            protectedDataLength++;
        }

        encodeSize += protectedDataLength;

        /* Npdate the EAP header length field */
        pMemoryBuffer[2] = (uint8_t)(encodeSize >> 8);
        pMemoryBuffer[3] = (uint8_t)encodeSize;

        /* The protected data is the 22 bytes header of the EAP message. */
        /* The G3 specifies a slightly modified EAP header but in the same time */
        /* the specification requires to compute the authentication tag over the */
        /* on the original EAP header */
        /* So we change the header to make it "EAP compliant", we compute the */
        /* auth tag and then we change back the header */

        /* right shift Code field with 2 bits as indicated in the EAP specification */
        pMemoryBuffer[0] >>= 2;

        SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, pPskContext->tek.value, sizeof(pPskContext->tek.value), "TEK: ");

        SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, auxNonce, 16U, "Nonce/IV: ");
        SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, pMemoryBuffer, 22U, "Header: ");
        SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, pProtectedData, protectedDataLength, "Data-plain: ");

        /* Encrypt P-Channel using TEK key */
        if (CIPHER_WRAPPER_RETURN_GOOD != CIPHER_Wrapper_AesEaxEncrypt(
                                              pProtectedData,          /* the message buffer        */
                                              protectedDataLength,     /* and its length in bytes   */
                                              auxNonce,                /* the initialization vector */
                                              16U,                     /* and its length in bytes   */
                                              pMemoryBuffer,           /* the header buffer         */
                                              22U,                     /* and its length in bytes   */
                                              pTag,                    /* the buffer for the tag    */
                                              16U,                     /* and its length in bytes   */
                                              pPskContext->tek.value)) /* the key                   */
        {
            encodeSize = 0U;
        }

        SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, pProtectedData, protectedDataLength, "Data-encr: ");
        SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, pTag, 16U, "Tag: ");

        /* Fix EAP header: left shift Code field with 2 bits as indicated in the G3 specification */
        pMemoryBuffer[0] <<= 2;
    }

    return encodeSize;
}

bool EAP_PSK_DecodeMessage4(
    uint16_t messageLength,
    uint8_t *pMessage,
    EAP_PSK_CONTEXT *pPskContext,
    uint16_t headerLength,
    uint8_t *pHeader,
    EAP_PSK_RAND *pRandS,
    uint32_t *pNonce,
    uint8_t *pPChannelResult,
    uint16_t *pPChannelDataLength,
    uint8_t **pPChannelData
    )
{
    bool retVal = false;
    uint8_t auxNonce[16];
    uint8_t *pAuxNonce = &pMessage[16];
    uint8_t *pTag = &pMessage[20];
    uint8_t *pProtectedData = &pMessage[36];
    uint16_t protectedDataLength = messageLength - 36U;

    if (messageLength >= 41U)
    {
        (void) memcpy(pRandS->value, pMessage, sizeof(pRandS->value));

        /* Prepare 16 bytes nonce */
        /* Nonce is big endian */
        (void)memset(auxNonce, 0, sizeof(auxNonce));
        auxNonce[12] = pAuxNonce[0];
        auxNonce[13] = pAuxNonce[1];
        auxNonce[14] = pAuxNonce[2];
        auxNonce[15] = pAuxNonce[3];

        /* The protected data is the 22 bytes header of the EAP message. */
        /* The G3 specifies a slightly modified EAP header but in the same time */
        /* the specification requires to compute the authentication tag over the */
        /* on the original EAP header */
        /* So we change the header to make it "EAP compliant", we compute the */
        /* auth tag and then we change back the header */

        /* right shift Code field with 2 bits as indicated in the EAP specification */
        pHeader[0] >>= 2;

        SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, pPskContext->tek.value, sizeof(pPskContext->tek.value), "TEK: ");
        SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, auxNonce, 16U, "Nonce/IV: ");
        SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, pHeader, headerLength, "Header: ");
        SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, pProtectedData, protectedDataLength, "Data-encr: ");
        SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, pTag, 16U, "Tag: ");

        /* Decrypt P-CHANNEL */
        /* P-CHANNEL uses the TEK key */
        /* CIPHER_WRAPPER_RETURN_GOOD is returned if the input tag matches that for the decrypted message */
        if (CIPHER_WRAPPER_RETURN_GOOD == CIPHER_Wrapper_AesEaxDecrypt(
                                              pProtectedData,          /* the message buffer        */
                                              protectedDataLength,     /* and its length in bytes   */
                                              auxNonce,                /* the initialization vector */
                                              16U,                     /* and its length in bytes   */
                                              pHeader,                 /* the header buffer         */
                                              headerLength,            /* and its length in bytes   */
                                              pTag,                    /* the buffer for the tag    */
                                              16U,                     /* and its length in bytes   */
                                              pPskContext->tek.value)) /* the key                   */
        {
            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "Decode SUCCESS\r\n");
            /* Retrieve protected parameters */
            *pPChannelResult = ((pProtectedData[0] & EAP_P_CHANNEL_RESULT_MASK) >> 6);
            *pPChannelData = &pProtectedData[1];
            *pPChannelDataLength = protectedDataLength - 1U;

            ((uint8_t *)pNonce)[0] = pAuxNonce[3];
            ((uint8_t *)pNonce)[1] = pAuxNonce[2];
            ((uint8_t *)pNonce)[2] = pAuxNonce[1];
            ((uint8_t *)pNonce)[3] = pAuxNonce[0];

            retVal = true;
        }
        else
        {
            SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "Decode FAILED\r\n");
        }

        SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, pProtectedData, protectedDataLength, "Data-plain: ");

        /* Fix EAP header: left shift Code field with 2 bits as indicated in the G3 specification */
        pHeader[0] <<= 2;
    }

    return retVal;
}

uint16_t EAP_PSK_EncodeEAPSuccess(
    uint8_t identifier,
    uint16_t memoryBufferLength,
    uint8_t *pMemoryBuffer
    )
{
    uint16_t encodeSize = 0U;

    /* Check the size of the buffer */
    if (memoryBufferLength >= 4U)
    {
        /* Encode the EAP header; length field will be set at the end of the block */
        pMemoryBuffer[0] = EAP_SUCCESS;
        pMemoryBuffer[1] = identifier;
        encodeSize = 4U;

        /* Update the EAP header length field */
        pMemoryBuffer[2] = (uint8_t)(encodeSize >> 8);
        pMemoryBuffer[3] = (uint8_t)encodeSize;
    }

    return encodeSize;
}

uint16_t EAP_PSK_EncodeEAPFailure(
    uint8_t identifier,
    uint16_t memoryBufferLength,
    uint8_t *pMemoryBuffer
    )
{
    uint16_t encodeSize = 0U;

    /* Check the size of the buffer */
    if (memoryBufferLength >= 4U)
    {
        /* Encode the EAP header; length field will be set at the end of the block */
        pMemoryBuffer[0] = EAP_FAILURE;
        pMemoryBuffer[1] = identifier;
        encodeSize = 4U;

        /* Update the EAP header length field */
        pMemoryBuffer[2] = (uint8_t)(encodeSize >> 8);
        pMemoryBuffer[3] = (uint8_t)encodeSize;
    }

    return encodeSize;
}

uint16_t EAP_PSK_EncodeGMKActivation(
    uint8_t *pPChannelData,
    uint16_t memoryBufferLength,
    uint8_t *pMemoryBuffer
    )
{
    uint16_t encodeSize = 0U;

    /* Check the size of the buffer */
    if (memoryBufferLength >= 3U)
    {
        /* Add GMK-activation EAP message */
        (void) memcpy(pMemoryBuffer, pPChannelData, 3);
        encodeSize = 3U;
    }

    return encodeSize;
}
