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
#define EAP_PSK_IANA_TYPE           0x2F
/* T Subfield mask */
#define EAP_T_SUBFIELD_MASK         0xC0
/* Protected Channel Result mask */
#define EAP_P_CHANNEL_RESULT_MASK   0xC0

// *****************************************************************************
// *****************************************************************************
// Section: Interface Function Definitions
// *****************************************************************************
// *****************************************************************************

void EAP_PSK_Initialize(
    const EAP_PSK_KEY *pKey,
    EAP_PSK_CONTEXT *pPskContext
    )
{
    uint8_t block[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t res[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    memset(pPskContext, 0, sizeof(EAP_PSK_CONTEXT));

    /* Initialize the AES */
    AES_Wrapper_ContextInit();
    /* Trigger the AES */
    AES_Wrapper_SetEncryptKey(pKey->value, 16);
    AES_Wrapper_EncryptEcb(block, res);

    /* Xor with c1 = "1" */
    res[15] ^= 0x01;

    /* Generate AK */
    AES_Wrapper_EncryptEcb(res, pPskContext->ak.value);

    /* Generate AK */
    /* Xor with c1 = "2" */
    res[15] ^= 0x03; /* 3 instead of 2 because it has been already xor'ed with 1 and we want to get back the initial value */

    /* Generate KDK */
    AES_Wrapper_EncryptEcb(res, pPskContext->kdk.value);

    /* Free the AES */
    AES_Wrapper_ContextFree();
}

void EAP_PSK_InitializeTEKMSK(
    const EAP_PSK_RAND *pRandP,
    EAP_PSK_CONTEXT *pPskContext
    )
{
    uint8_t res[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t idx;

    /* Initialize the AES */
    AES_Wrapper_ContextInit();
    /* Trigger the AES */
    AES_Wrapper_SetEncryptKey(pPskContext->kdk.value, 16);
    AES_Wrapper_EncryptEcb(pRandP->value, res);

    /* Xor with c1 = "1" */
    res[15] ^= 0x01;

    /* Generate TEK */
    AES_Wrapper_EncryptEcb(res, pPskContext->tek.value);

    /* Undo xor to generate next block*/
    res[15] ^= 0x01;

    /* Generate MSK with subsequent blocks */
    for (idx = 0; idx < 4; idx++)
    {
        /* Xor with c1 = "(idx + 2)" */
        res[15] ^= (idx + 2);
        /* Get next block for MSK at corresponding index */
        AES_Wrapper_EncryptEcb(res, &pPskContext->msk.value[idx * 16]);
        /* Undo xor to generate next block*/
        res[15] ^= (idx + 2);
    }

    /* Free the AES */
    AES_Wrapper_ContextFree();
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
    uint16_t eapMessageLength = 0;

    if (messageLength >= 4)
    {
        *pCode = pMessage[0];
        *pIdentifier = pMessage[1];
        eapMessageLength = ((pMessage[2] << 8) | pMessage[3]);

        /* A message with the Length field set to a value larger than the number of received octets MUST be silently discarded. */
        retVal = (eapMessageLength <= messageLength);

        if (retVal && (eapMessageLength >= 6))
        {
            *pTSubfield = (pMessage[5] & EAP_T_SUBFIELD_MASK);
            *pEAPData = &pMessage[6];
            *pEAPDataLength = eapMessageLength - 6; /* 4 for the size of the header + 2 eaptype and T field */

            retVal = (pMessage[4] == EAP_PSK_IANA_TYPE);
        }
    }

    return retVal;
}

bool EAP_PSK_DecodeMessage1(
    uint16_t messageLength,
    uint8_t *pMessage,
    EAP_PSK_RAND *pRandS,
    EAP_PSK_NETWORK_ACCESS_IDENTIFIER_S *pIdS
    )
{
    bool retVal = false;

    /* Check the length of the message */
    if (messageLength >= sizeof(pRandS->value))
    {
        memcpy(pRandS->value, pMessage, sizeof(pRandS->value));

        pIdS->size = messageLength - sizeof(pRandS->value);
        memcpy(pIdS->value, &pMessage[sizeof(pRandS->value)], pIdS->size);
        retVal = true;
    }

    return retVal;
}

uint16_t EAP_PSK_EncodeMessage2(
    const EAP_PSK_CONTEXT *pPskContext,
    uint8_t identifier,
    const EAP_PSK_RAND *pRandS,
    const EAP_PSK_RAND *pRandP,
    const EAP_PSK_NETWORK_ACCESS_IDENTIFIER_S *pIdS,
    const EAP_PSK_NETWORK_ACCESS_IDENTIFIER_P *pIdP,
    uint16_t memoryBufferLength,
    uint8_t *pMemoryBuffer
    )
{
    uint16_t encodeSize = 0;
    int ret;
    uint8_t macP[16];
    uint8_t seed[2 * member_size(EAP_PSK_NETWORK_ACCESS_IDENTIFIER_P, value)
        + 2 * member_size(EAP_PSK_RAND, value)];
    uint16_t seedUsedSize = 0;

    /* check the size of the buffer */
    if (memoryBufferLength >= 90)
    {
        /* Compute first MacP = CMAC-AES-128(AK, IdP||IdS||RandS||RandP) */
        memcpy(seed, pIdP->value, pIdP->size);
        seedUsedSize += pIdP->size;

        memcpy(&seed[seedUsedSize], pIdS->value, pIdS->size);
        seedUsedSize += pIdS->size;

        memcpy(&seed[seedUsedSize], pRandS->value, sizeof(pRandS->value));
        seedUsedSize += sizeof(pRandS->value);

        memcpy(&seed[seedUsedSize], pRandP->value, sizeof(pRandP->value));
        seedUsedSize += sizeof(pRandP->value);

        ret = CIPHER_Wrapper_CmacStart(pPskContext->ak.value, sizeof(pPskContext->ak.value));
        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "CIPHER_Wrapper_CmacStart returned %d\r\n", ret);

        ret = CIPHER_Wrapper_CmacUpdate(seed, seedUsedSize);
        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "CIPHER_Wrapper_CmacUpdate returned %d\r\n", ret);

        ret = CIPHER_Wrapper_CmacFinish(macP);
        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "CIPHER_Wrapper_CmacFinish returned %d\r\n", ret);

        SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, pPskContext->ak.value, sizeof(pPskContext->ak.value), "Seed ");
        SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, seed, seedUsedSize, "Seed ");
        SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, macP, sizeof(macP), "MacP ");

        /* Encode the EAP header; length field will be set at the end of the block */
        pMemoryBuffer[0] = EAP_RESPONSE;
        pMemoryBuffer[1] = identifier;
        pMemoryBuffer[4] = EAP_PSK_IANA_TYPE;
        pMemoryBuffer[5] = EAP_PSK_T1;
        encodeSize = 6;

        /* EAP message: add PSK fields */
        memcpy(&pMemoryBuffer[encodeSize], pRandS->value, sizeof(pRandS->value));
        encodeSize += sizeof(pRandS->value);
        memcpy(&pMemoryBuffer[encodeSize], pRandP->value, sizeof(pRandP->value));
        encodeSize += sizeof(pRandP->value);
        memcpy(&pMemoryBuffer[encodeSize], macP, sizeof(macP));
        encodeSize += sizeof(macP);
        memcpy(&pMemoryBuffer[encodeSize], pIdP->value, pIdP->size);
        encodeSize += pIdP->size;

        /* Update the EAP header length field */
        pMemoryBuffer[2] = (uint8_t)((encodeSize >> 8) & 0x00FF);
        pMemoryBuffer[3] = (uint8_t)(encodeSize & 0x00FF);

        (void)(ret);
    }

    return encodeSize;
}

bool EAP_PSK_DecodeMessage3(
    uint16_t messageLength,
    uint8_t *pMessage,
    const EAP_PSK_CONTEXT *pPskContext,
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
    int ret;
    uint8_t seed[member_size(EAP_PSK_NETWORK_ACCESS_IDENTIFIER_P, value)
        + member_size(EAP_PSK_RAND, value)];
    uint16_t seedUsedSize = 0;

    if (messageLength >= 59)
    {
        memcpy(pRandS->value, pMessage, sizeof(pRandS->value));

        /* Verify MacS: MAC_S = CMAC-AES-128(AK, IdS||RandP) */
        memcpy(seed, pPskContext->idS.value, pPskContext->idS.size);
        seedUsedSize += pPskContext->idS.size;

        memcpy(&seed[seedUsedSize], pPskContext->randP.value, sizeof(pPskContext->randP.value));
        seedUsedSize += sizeof(pPskContext->randP.value);

        ret = CIPHER_Wrapper_CmacStart(pPskContext->ak.value, sizeof(pPskContext->ak.value));
        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "CIPHER_Wrapper_CmacStart returned %d\r\n", ret);

        ret = CIPHER_Wrapper_CmacUpdate(seed, seedUsedSize);
        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "CIPHER_Wrapper_CmacUpdate returned %d\r\n", ret);

        ret = CIPHER_Wrapper_CmacFinish(macS);
        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "CIPHER_Wrapper_CmacFinish returned %d\r\n", ret);

        if (memcmp(macS, &pMessage[sizeof(pRandS->value)], sizeof(macS)) == 0)
        {
            /* Decrypt P-CHANNEL */
            /* P-CHANNEL uses the TEK key */
            if (CIPHER_WRAPPER_RETURN_GOOD == 
                    CIPHER_Wrapper_EaxInitKey(pPskContext->tek.value, sizeof(pPskContext->tek.value)))
            {
                uint8_t auxNonce[16];
                uint8_t *pAuxNonce = &pMessage[32];
                uint8_t *pTag = &pMessage[36];
                uint8_t *pProtectedData = &pMessage[52];
                uint16_t protectedDataLength = messageLength - 52;

                /* Prepare 16 bytes nonce */
                /* Nonce is big endian */
                memset(auxNonce, 0, sizeof(auxNonce));
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
                SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, auxNonce, 16, "Nonce/IV: ");
                SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, pHeader, headerLength, "Header: ");
                SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, pProtectedData, protectedDataLength, "Data-encr: ");
                SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, pTag, 16, "Tag: ");

                /* CIPHER_WRAPPER_RETURN_GOOD is returned if the input tag matches that for the decrypted message */
                if (CIPHER_WRAPPER_RETURN_GOOD == 
                        CIPHER_Wrapper_EaxDecrypt(auxNonce, /* the initialization vector */
                            16, /* and its length in bytes */
                            pHeader, /* the header buffer */
                            headerLength, /* and its length in bytes */
                            pProtectedData, /* the message buffer */
                            protectedDataLength, /* and its length in bytes */
                            pTag, /* the buffer for the tag */
                            16) /* and its length in bytes */
                            )
                {
                    /* Retrieve protected parameters */
                    *pPChannelResult = ((pProtectedData[0] & EAP_P_CHANNEL_RESULT_MASK) >> 6);

                    *pPChannelDataLength = protectedDataLength - 1;
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

                CIPHER_Wrapper_EaxEnd();
            }

            (void)(ret);
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
    const EAP_PSK_CONTEXT *pPskContext,
    uint8_t identifier,
    const EAP_PSK_RAND *pRandS,
    uint32_t nonce,
    uint8_t PChannelResult,
    uint16_t PChannelDataLength,
    uint8_t *pPChannelData,
    uint16_t memoryBufferLength,
    uint8_t *pMemoryBuffer
    )
{
    uint16_t encodeSize = 0;
    uint8_t *pTag = 0L;
    uint8_t auxNonce[16];
    uint8_t *pProtectedData = 0L;
    uint16_t protectedDataLength = 0;

    /* Check the size of the buffer */
    if (memoryBufferLength >= 43 + PChannelDataLength)
    {
        /* encode the EAP header; length field will be set at the end of the block */
        pMemoryBuffer[0] = EAP_RESPONSE;
        pMemoryBuffer[1] = identifier;
        pMemoryBuffer[4] = EAP_PSK_IANA_TYPE;
        pMemoryBuffer[5] = EAP_PSK_T3;
        encodeSize = 6;

        /* EAP message: add PSK fields */
        memcpy(&pMemoryBuffer[encodeSize], pRandS->value, sizeof(pRandS->value));
        encodeSize += sizeof(pRandS->value);

        /* Nonce is big endian */
        memset(auxNonce, 0, sizeof(auxNonce));
        auxNonce[12] = pMemoryBuffer[encodeSize] = (uint8_t)((nonce >> 24) & 0xFF);
        auxNonce[13] = pMemoryBuffer[encodeSize + 1] = (uint8_t)((nonce >> 16) & 0xFF);
        auxNonce[14] = pMemoryBuffer[encodeSize + 2] = (uint8_t)((nonce >> 8) & 0xFF);
        auxNonce[15] = pMemoryBuffer[encodeSize + 3] = (uint8_t)(nonce & 0xFF);
        encodeSize += 4;

        /* Tag will be set after data protection */
        pTag = &pMemoryBuffer[encodeSize];
        encodeSize += 16;

        /* Protected data */
        pProtectedData = &pMemoryBuffer[encodeSize];
        if (PChannelDataLength > 0)
        {
            /* result / extension = 1 */
            pProtectedData[protectedDataLength] = (PChannelResult << 6) | 0x20;
            protectedDataLength++;

            memcpy(&pProtectedData[protectedDataLength], pPChannelData, PChannelDataLength);
            protectedDataLength += PChannelDataLength;
        }
        else
        {
            /* result / extension = 0 */
            pProtectedData[protectedDataLength] = (PChannelResult << 6);
            protectedDataLength++;
        }

        encodeSize += protectedDataLength;

        /* Protect data in P-Channel (just the last byte) */
        /* P-CHANNEL uses the TEK key */
        if (CIPHER_WRAPPER_RETURN_GOOD == 
                CIPHER_Wrapper_EaxInitKey(pPskContext->tek.value, sizeof(pPskContext->tek.value)))
        {
            /* Update the EAP header length field */
            pMemoryBuffer[2] = (uint8_t)((encodeSize >> 8) & 0x00FF);
            pMemoryBuffer[3] = (uint8_t)(encodeSize & 0x00FF);

            /* The protected data is the 22 bytes header of the EAP message. */
            /* The G3 specifies a slightly modified EAP header but in the same time */
            /* the specification requires to compute the authentication tag over the */
            /* the original EAP header */
            /* So we change the header to make it "EAP compliant", we compute the */
            /* auth tag and then we change back the header */

            /* Right shift Code field with 2 bits as indicated in the EAP specification */
            pMemoryBuffer[0] >>= 2;

            SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, pPskContext->tek.value, sizeof(pPskContext->tek.value), "TEK: ");
            SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, auxNonce, 16, "Nonce/IV: ");
            SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, pMemoryBuffer, 22, "Header: ");
            SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, pProtectedData, protectedDataLength, "Data-plain: ");

            if (CIPHER_WRAPPER_RETURN_GOOD != CIPHER_Wrapper_EaxEncrypt(
                    auxNonce, /* the initialization vector    */
                    16, /* and its length in bytes      */
                    pMemoryBuffer, /* the header buffer            */
                    22, /* and its length in bytes      */
                    pProtectedData, /* the message buffer           */
                    protectedDataLength, /* and its length in bytes      */
                    pTag, /* the buffer for the tag       */
                    16) /* and its length in bytes      */
                    )
            {
                encodeSize = 0;
            }

            SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, pProtectedData, protectedDataLength, "Data-encr: ");
            SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, pTag, 16, "Tag: ");

            /* Fix EAP header: left shift Code field with 2 bits as indicated in the G3 specification */
            pMemoryBuffer[0] <<= 2;

            CIPHER_Wrapper_EaxEnd();
        }
        else
        {
            encodeSize = 0;
        }
    }

    return encodeSize;
}

uint16_t EAP_PSK_EncodeMessage1(
    uint8_t identifier,
    const EAP_PSK_RAND *pRandS,
    const EAP_PSK_NETWORK_ACCESS_IDENTIFIER_S *pIdS,
    uint16_t memoryBufferLength,
    uint8_t *pMemoryBuffer
    )
{
    uint16_t encodeSize = 0;

    /* Check the size of the buffer */
    if (memoryBufferLength >= 30)
    {
        /* Encode the EAP header; length field will be set at the end of the block */
        pMemoryBuffer[0] = EAP_REQUEST;
        pMemoryBuffer[1] = identifier;
        pMemoryBuffer[4] = EAP_PSK_IANA_TYPE;
        pMemoryBuffer[5] = EAP_PSK_T0;
        encodeSize = 6;

        /* EAP message: add PSK fields */
        memcpy(&pMemoryBuffer[encodeSize], pRandS->value, sizeof(pRandS->value));
        encodeSize += sizeof(pRandS->value);
        memcpy(&pMemoryBuffer[encodeSize], pIdS->value, pIdS->size);
        encodeSize += pIdS->size;

        /* Update the EAP header length field */
        pMemoryBuffer[2] = (uint8_t)((encodeSize >> 8) & 0x00FF);
        pMemoryBuffer[3] = (uint8_t)(encodeSize & 0x00FF);
    }

    return encodeSize;
}

bool EAP_PSK_DecodeMessage2(
    bool aribBand,
    uint16_t messageLength,
    uint8_t *pMessage,
    const EAP_PSK_CONTEXT *pPskContext,
    const EAP_PSK_NETWORK_ACCESS_IDENTIFIER_S *pIdS,
    EAP_PSK_RAND *pRandS,
    EAP_PSK_RAND *pRandP
    )
{
    bool retVal = false;
    uint8_t minMsgLength;
    uint8_t macP[16];
    uint8_t expectedMacP[16];
    EAP_PSK_NETWORK_ACCESS_IDENTIFIER_P idP;
    int16_t ret;

    if (aribBand)
    {
        minMsgLength = 49;
    }
    else
    {
        minMsgLength = 56;
    }

    if (messageLength >= minMsgLength)
    {
        /* EAP header already removed */
        if (aribBand)
        {
            /* In ARIB, ID_P can be range between 8 and 36 bytes, its length depends on the message length */
            idP.size = messageLength - 48;
            /* Maximum size is checked */
            if (idP.size > LBP_NETWORK_ACCESS_ID_MAX_SIZE_P)
            {
                idP.size = LBP_NETWORK_ACCESS_ID_MAX_SIZE_P;
            }
        }
        else
        {
            idP.size = 8;
        }

        uint8_t seed[LBP_NETWORK_ACCESS_ID_MAX_SIZE_P + LBP_NETWORK_ACCESS_ID_MAX_SIZE_S + 2 * member_size(EAP_PSK_RAND, value)];
        uint8_t seedSize = idP.size + pIdS->size + 2 * member_size(EAP_PSK_RAND, value);

        uint16_t decodeOffset = 0;
        uint16_t seedOffset = 0;

        memcpy(pRandS->value, &pMessage[decodeOffset], sizeof(pRandS->value));
        decodeOffset += sizeof(pRandS->value);

        memcpy(pRandP->value, &pMessage[decodeOffset], sizeof(pRandP->value));
        decodeOffset += sizeof(pRandP->value);

        memcpy(macP, &pMessage[decodeOffset], sizeof(macP));
        decodeOffset += sizeof(macP);

        memcpy(idP.value, &pMessage[decodeOffset], idP.size);
        decodeOffset += idP.size;

        /* Compute MacP = CMAC-AES-128(AK, IdP||IdS||RandS||RandP) */
        memcpy(seed, idP.value, idP.size);
        seedOffset += idP.size;

        memcpy(&seed[seedOffset], pIdS->value, pIdS->size);
        seedOffset += pIdS->size;

        memcpy(&seed[seedOffset], pRandS->value, sizeof(pRandS->value));
        seedOffset += sizeof(pRandS->value);

        memcpy(&seed[seedOffset], pRandP->value, sizeof(pRandP->value));
        seedOffset += sizeof(pRandP->value);

        ret = CIPHER_Wrapper_CmacStart(pPskContext->ak.value, sizeof(pPskContext->ak.value));
        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "CIPHER_Wrapper_CmacStart returned %d\r\n", ret);

        ret = CIPHER_Wrapper_CmacUpdate(seed, seedSize);
        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "CIPHER_Wrapper_CmacUpdate returned %d\r\n", ret);

        ret = CIPHER_Wrapper_CmacFinish(expectedMacP);
        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "CIPHER_Wrapper_CmacFinish returned %d\r\n", ret);

        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "decodeOffset: %d, seedOffset: %d, seedSize: %d, idP.size: %d, pIdS->size: %d\n\r\n", decodeOffset, seedOffset, seedSize,
                idP.size, pIdS->size);

        retVal = (memcmp(expectedMacP, macP, sizeof(macP)) == 0);

        (void)(ret);
    }

    return retVal;
}

/**********************************************************************************************************************/

/** The EAP_PSK_EncodeMessage3 primitive is used to encode the third EAP-PSK message (type 2)
 ***********************************************************************************************************************
 *
 **********************************************************************************************************************/
uint16_t EAP_PSK_EncodeMessage3(
    const EAP_PSK_CONTEXT *pPskContext,
    uint8_t identifier,
    const EAP_PSK_RAND *pRandS,
    const EAP_PSK_RAND *pRandP,
    const EAP_PSK_NETWORK_ACCESS_IDENTIFIER_S *pIdS,
    uint32_t nonce,
    uint8_t PChannelResult,
    uint16_t PChannelDataLength,
    uint8_t *pPChannelData,
    uint16_t memoryBufferLength,
    uint8_t *pMemoryBuffer
    )
{
    uint16_t encodeSize = 0;
    uint8_t macS[16];
    uint8_t *pProtectedData = 0L;
    uint16_t protectedDataLength = 0;
    uint8_t *pTag = 0L;
    uint8_t auxNonce[16];
    int16_t ret;
    uint8_t seed[LBP_NETWORK_ACCESS_ID_MAX_SIZE_S + member_size(EAP_PSK_RAND, value)];
    uint8_t seedSize = pIdS->size + member_size(EAP_PSK_RAND, value);

    /* check the size of the buffer */
    if (memoryBufferLength >= 59 + PChannelDataLength)
    {
        memcpy(seed, pIdS->value, pIdS->size);
        memcpy(&seed[pIdS->size], pRandP->value, sizeof(pRandP->value));

        ret = CIPHER_Wrapper_CmacStart(pPskContext->ak.value, sizeof(pPskContext->ak.value));
        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "CIPHER_Wrapper_CmacStart returned %d\r\n", ret);

        ret = CIPHER_Wrapper_CmacUpdate(seed, seedSize);
        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "CIPHER_Wrapper_CmacUpdate returned %d\r\n", ret);

        ret = CIPHER_Wrapper_CmacFinish(macS);
        SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "CIPHER_Wrapper_CmacFinish returned %d\r\n", ret);

        (void)(ret);

        /* Encode the EAP header; length field will be set at the end of the block */
        pMemoryBuffer[0] = EAP_REQUEST;
        pMemoryBuffer[1] = identifier;
        pMemoryBuffer[4] = EAP_PSK_IANA_TYPE;
        pMemoryBuffer[5] = EAP_PSK_T2;
        encodeSize = 6;

        /* EAP message: add PSK fields */
        memcpy(&pMemoryBuffer[encodeSize], pRandS->value, sizeof(pRandS->value));
        encodeSize += sizeof(pRandS->value);
        memcpy(&pMemoryBuffer[encodeSize], macS, sizeof(macS));
        encodeSize += sizeof(macS);

        /* Prepare P-Channel content */
        /* Nonce is big endian */
        memset(auxNonce, 0, sizeof(auxNonce));
        auxNonce[12] = pMemoryBuffer[encodeSize] = (uint8_t)((nonce >> 24) & 0xFF);
        auxNonce[13] = pMemoryBuffer[encodeSize + 1] = (uint8_t)((nonce >> 16) & 0xFF);
        auxNonce[14] = pMemoryBuffer[encodeSize + 2] = (uint8_t)((nonce >> 8) & 0xFF);
        auxNonce[15] = pMemoryBuffer[encodeSize + 3] = (uint8_t)(nonce & 0xFF);
        encodeSize += 4;

        /* tag will be added later */
        pTag = &pMemoryBuffer[encodeSize];
        encodeSize += 16;

        /* protected data */
        pProtectedData = &pMemoryBuffer[encodeSize];
        if (PChannelDataLength > 0)
        {
            /* result / extension = 1 */
            pProtectedData[protectedDataLength] = (PChannelResult << 6) | 0x20;
            protectedDataLength++;

            memcpy(&pProtectedData[protectedDataLength], pPChannelData, PChannelDataLength);
            protectedDataLength += PChannelDataLength;
        }
        else
        {
            /* result / extension = 0 */
            pProtectedData[protectedDataLength] = (PChannelResult << 6);
            protectedDataLength++;
        }

        encodeSize += protectedDataLength;

        /* Encrypt P-Channel using TEK key */
        if (CIPHER_WRAPPER_RETURN_GOOD == 
                CIPHER_Wrapper_EaxInitKey(pPskContext->tek.value, sizeof(pPskContext->tek.value)))
        {
            /* Npdate the EAP header length field */
            pMemoryBuffer[2] = (uint8_t)((encodeSize >> 8) & 0x00FF);
            pMemoryBuffer[3] = (uint8_t)(encodeSize & 0x00FF);

            /* The protected data is the 22 bytes header of the EAP message. */
            /* The G3 specifies a slightly modified EAP header but in the same time */
            /* the specification requires to compute the authentication tag over the */
            /* on the original EAP header */
            /* So we change the header to make it "EAP compliant", we compute the */
            /* auth tag and then we change back the header */

            /* right shift Code field with 2 bits as indicated in the EAP specification */
            pMemoryBuffer[0] >>= 2;

            SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, pPskContext->tek.value, sizeof(pPskContext->tek.value), "TEK: ");

            SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, auxNonce, 16, "Nonce/IV: ");
            SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, pMemoryBuffer, 22, "Header: ");
            SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, pProtectedData, protectedDataLength, "Data-plain: ");

            if (CIPHER_WRAPPER_RETURN_GOOD != CIPHER_Wrapper_EaxEncrypt(
                    auxNonce, /* the initialization vector    */
                    16, /* and its length in bytes      */
                    pMemoryBuffer, /* the header buffer            */
                    22, /* and its length in bytes      */
                    pProtectedData, /* the message buffer           */
                    protectedDataLength, /* and its length in bytes      */
                    pTag, /* the buffer for the tag       */
                    16) /* and its length in bytes      */
                    )
            {
                encodeSize = 0;
            }

            SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, pProtectedData, protectedDataLength, "Data-encr: ");
            SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, pTag, 16, "Tag: ");

            /* Fix EAP header: left shift Code field with 2 bits as indicated in the G3 specification */
            pMemoryBuffer[0] <<= 2;

            CIPHER_Wrapper_EaxEnd();
        }
        else
        {
            encodeSize = 0;
        }
    }

    return encodeSize;
}

bool EAP_PSK_DecodeMessage4(
    uint16_t messageLength,
    uint8_t *pMessage,
    const EAP_PSK_CONTEXT *pPskContext,
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
    uint16_t protectedDataLength = messageLength - 36;

    if (messageLength >= 41)
    {
        memcpy(pRandS->value, pMessage, sizeof(pRandS->value));
        /* Decrypt P-CHANNEL */
        /* P-CHANNEL uses the TEK key */
        if (CIPHER_WRAPPER_RETURN_GOOD == 
                CIPHER_Wrapper_EaxInitKey(pPskContext->tek.value, sizeof(pPskContext->tek.value)))
        {
            /* Prepare 16 bytes nonce */
            /* Nonce is big endian */
            memset(auxNonce, 0, sizeof(auxNonce));
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
            SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, auxNonce, 16, "Nonce/IV: ");
            SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, pHeader, headerLength, "Header: ");
            SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, pProtectedData, protectedDataLength, "Data-encr: ");
            SRV_LOG_REPORT_Buffer(SRV_LOG_REPORT_DEBUG, pTag, 16, "Tag: ");

            /* CIPHER_WRAPPER_RETURN_GOOD is returned if the input tag matches that for the decrypted message */
            if (CIPHER_WRAPPER_RETURN_GOOD == CIPHER_Wrapper_EaxDecrypt(
                    auxNonce, /* the initialization vector */
                    16, /* and its length in bytes */
                    pHeader, /* the header buffer */
                    headerLength, /* and its length in bytes */
                    pProtectedData, /* the message buffer */
                    protectedDataLength, /* and its length in bytes */
                    pTag, /* the buffer for the tag */
                    16) /* and its length in bytes */
                    )
            {
                SRV_LOG_REPORT_Message(SRV_LOG_REPORT_DEBUG, "Decode SUCCESS\r\n");
                /* Retrieve protected parameters */
                *pPChannelResult = ((pProtectedData[0] & EAP_P_CHANNEL_RESULT_MASK) >> 6);
                *pPChannelData = &pProtectedData[1];
                *pPChannelDataLength = protectedDataLength - 1;

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

            CIPHER_Wrapper_EaxEnd();
        }
    }

    return retVal;
}

uint16_t EAP_PSK_EncodeEAPSuccess(
    uint8_t identifier,
    uint16_t memoryBufferLength,
    uint8_t *pMemoryBuffer
    )
{
    uint16_t encodeSize = 0;

    /* Check the size of the buffer */
    if (memoryBufferLength >= 4)
    {
        /* Encode the EAP header; length field will be set at the end of the block */
        pMemoryBuffer[0] = EAP_SUCCESS;
        pMemoryBuffer[1] = identifier;
        encodeSize = 4;

        /* Update the EAP header length field */
        pMemoryBuffer[2] = (uint8_t)((encodeSize >> 8) & 0x00FF);
        pMemoryBuffer[3] = (uint8_t)(encodeSize & 0x00FF);
    }

    return encodeSize;
}

uint16_t EAP_PSK_EncodeEAPFailure(
    uint8_t identifier,
    uint16_t memoryBufferLength,
    uint8_t *pMemoryBuffer
    )
{
    uint16_t encodeSize = 0;

    /* Check the size of the buffer */
    if (memoryBufferLength >= 4)
    {
        /* Encode the EAP header; length field will be set at the end of the block */
        pMemoryBuffer[0] = EAP_FAILURE;
        pMemoryBuffer[1] = identifier;
        encodeSize = 4;

        /* Update the EAP header length field */
        pMemoryBuffer[2] = (uint8_t)((encodeSize >> 8) & 0x00FF);
        pMemoryBuffer[3] = (uint8_t)(encodeSize & 0x00FF);
    }

    return encodeSize;
}

uint16_t EAP_PSK_EncodeGMKActivation(
    uint8_t *pPChannelData,
    uint16_t memoryBufferLength,
    uint8_t *pMemoryBuffer
    )
{
    uint16_t encodeSize = 0;

    /* Check the size of the buffer */
    if (memoryBufferLength >= 3)
    {
        /* Add GMK-activation EAP message */
        memcpy(pMemoryBuffer, pPChannelData, 3);
        encodeSize = 3;
    }

    return encodeSize;
}
