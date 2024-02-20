/*******************************************************************************
  Source for the cipher wrapper between G3 stack and Crypto

  Company:
    Microchip Technology Inc.

  File Name:
    cipher_wrapper.h

  Summary:
    Interface implementation of the wrapper between G3 and Crypto.

  Description:
    This file implements the interface for the wrapper between G3 and Crypto.
    It includes calls to handle CCM, CMAC and EAX.
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

#include "cipher_wrapper.h"
#include "crypto/common_crypto/MCHP_Crypto_Mac_Cipher.h"
#include "crypto/common_crypto/MCHP_Crypto_Aead_Cipher.h"
#include <string.h>

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Data
// *****************************************************************************
// *****************************************************************************

/* CCM context used in this wrapper */
static st_Crypto_Aead_AesCcm_ctx cipherWrapperCcmContext;

/* Auxiliary buffer for EAX output */
static uint8_t cipherWrapperOutAux[128];

// *****************************************************************************
// *****************************************************************************
// Section: Service Interface Functions
// *****************************************************************************
// *****************************************************************************

int32_t CIPHER_Wrapper_AesCmacDirect(uint8_t *input, uint32_t inputLen,
                                     uint8_t *outputMac, uint8_t *key)
{
    return (int32_t) Crypto_Mac_AesCmac_Direct(CRYPTO_HANDLER_SW_WOLFCRYPT,
            input, inputLen, outputMac, 16, key, CRYPTO_AESKEYSIZE_128, 1);
}

int32_t CIPHER_Wrapper_AesCcmSetkey(uint8_t *key)
{
    return (int32_t) Crypto_Aead_AesCcm_Init(&cipherWrapperCcmContext,
            CRYPTO_HANDLER_SW_WOLFCRYPT, key, CRYPTO_AESKEYSIZE_128, 1);
}

int32_t CIPHER_Wrapper_AesCcmAuthDecrypt(uint8_t *data, uint32_t dataLen,
                                         uint8_t *iv, uint32_t ivLen,
                                         uint8_t *aad, uint32_t aadLen,
                                         uint8_t *tag, uint32_t tagLen)
{
    return (int32_t) Crypto_Aead_AesCcm_Cipher(&cipherWrapperCcmContext,
            CRYPTO_CIOP_DECRYPT, data, dataLen, data,
            iv, ivLen, tag, tagLen, aad, aadLen);
}

int32_t CIPHER_Wrapper_AesCcmEncryptAndTag(uint8_t *data, uint32_t dataLen,
                                           uint8_t *iv, uint32_t ivLen,
                                           uint8_t *aad, uint32_t aadLen,
                                           uint8_t *tag, uint32_t tagLen)
{
    return (int32_t) Crypto_Aead_AesCcm_Cipher(&cipherWrapperCcmContext,
            CRYPTO_CIOP_ENCRYPT, data, dataLen, data,
            iv, ivLen, tag, tagLen, aad, aadLen);
}

int32_t CIPHER_Wrapper_AesEaxEncrypt(uint8_t *data, uint32_t dataLen,
                                     uint8_t *iv, uint32_t ivLen,
                                     uint8_t *aad, uint32_t aadLen,
                                     uint8_t *tag, uint32_t tagLen,
                                     uint8_t *key)
{
    int32_t result = (int32_t) CRYPTO_AEAD_ERROR_INPUTDATA;

    if (dataLen <= sizeof(cipherWrapperOutAux))
    {
        result = (int32_t) Crypto_Aead_AesEax_EncryptAuthDirect(
                CRYPTO_HANDLER_SW_WOLFCRYPT, data, dataLen, cipherWrapperOutAux, key,
                CRYPTO_AESKEYSIZE_128, iv, ivLen, aad, aadLen, tag, tagLen, 1);

        (void) memcpy(data, cipherWrapperOutAux, dataLen);
    }

    return result;
}

int32_t CIPHER_Wrapper_AesEaxDecrypt(uint8_t *data, uint32_t dataLen,
                                     uint8_t *iv, uint32_t ivLen,
                                     uint8_t *aad, uint32_t aadLen,
                                     uint8_t *tag, uint32_t tagLen,
                                     uint8_t *key)
{
    int32_t result = (int32_t) CRYPTO_AEAD_ERROR_INPUTDATA;

    if (dataLen <= sizeof(cipherWrapperOutAux))
    {
        result = (int32_t) Crypto_Aead_AesEax_DecryptAuthDirect(
                CRYPTO_HANDLER_SW_WOLFCRYPT, data, dataLen, cipherWrapperOutAux, key,
                CRYPTO_AESKEYSIZE_128, iv, ivLen, aad, aadLen, tag, tagLen, 1);

        (void) memcpy(data, cipherWrapperOutAux, dataLen);
    }

    return result;
}
