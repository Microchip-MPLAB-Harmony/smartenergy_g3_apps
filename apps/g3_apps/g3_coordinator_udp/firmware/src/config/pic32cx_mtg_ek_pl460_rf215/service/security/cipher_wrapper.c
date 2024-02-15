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

#include <string.h>
#include <stdbool.h>
#include "definitions.h"
#include "cipher_wrapper.h"
#include "eax.h"
#include "wolfssl/wolfcrypt/cmac.h"

// *****************************************************************************
// *****************************************************************************
// Section: Static Data
// *****************************************************************************
// *****************************************************************************

/* CMAC context used in this wrapper */
static Cmac sCmacCtx;

/* CCM context used in this wrapper */
static Aes sCcmCtx;

/* EAX context used in this wrapper */
static eax_ctx sEaxCtx;

// *****************************************************************************
// *****************************************************************************
// Section: Service Interface Functions
// *****************************************************************************
// *****************************************************************************

int32_t CIPHER_Wrapper_CmacStart(const uint8_t *key, uint32_t keyLen)
{
    return wc_InitCmac(&sCmacCtx, key, keyLen, (int32_t) WC_CMAC_AES, NULL);
}

int32_t CIPHER_Wrapper_CmacUpdate(const uint8_t *input, uint32_t iLen)
{
    return wc_CmacUpdate(&sCmacCtx, input, iLen);
}

int32_t CIPHER_Wrapper_CmacFinish(uint8_t *output)
{
    unsigned int outputLen = AES_BLOCK_SIZE;

    return wc_CmacFinal(&sCmacCtx, output, &outputLen);
}

int32_t CIPHER_Wrapper_CcmSetkey(const uint8_t *key, uint32_t keyLen)
{
    return wc_AesCcmSetKey(&sCcmCtx, key, keyLen);
}

int32_t CIPHER_Wrapper_CcmAuthDecrypt(uint32_t length,
                                      const uint8_t *iv, uint32_t ivLen,
                                      const uint8_t *add, uint32_t addLen,
                                      const uint8_t *input, uint8_t *output,
                                      const uint8_t *tag, uint32_t tagLen)
{
    return wc_AesCcmDecrypt(&sCcmCtx, output, input, length, iv, ivLen, tag,
                            tagLen, add, addLen);
}

int32_t CIPHER_Wrapper_CcmEncryptAndTag(uint32_t length,
                                        const uint8_t *iv, uint32_t ivLen,
                                        const uint8_t *add, uint32_t addLen,
                                        const uint8_t *input, uint8_t *output,
                                        uint8_t *tag, uint32_t tagLen)
{
    return wc_AesCcmEncrypt(&sCcmCtx, output, input, length, iv, ivLen, tag,
                            tagLen, add, addLen);
}

int32_t CIPHER_Wrapper_EaxInitKey(const uint8_t *key, uint32_t keyLen)
{
    return eax_init_and_key(key, keyLen, &sEaxCtx);
}

int32_t CIPHER_Wrapper_EaxEncrypt(const uint8_t *iv, uint32_t ivLen,
                                  const uint8_t *hdr, uint32_t hdrLen,
                                  uint8_t *msg, uint32_t msgLen,
                                  uint8_t *tag, uint32_t tagLen)
{
    return eax_encrypt_message(iv, ivLen, hdr, hdrLen, msg, msgLen, tag,
                               tagLen, &sEaxCtx);
}

int32_t CIPHER_Wrapper_EaxDecrypt(const uint8_t *iv, uint32_t ivLen,
                                  const uint8_t *hdr, uint32_t hdrLen,
                                  uint8_t *msg, uint32_t msgLen,
                                  const uint8_t *tag, uint32_t tagLen)
{
    return eax_decrypt_message(iv, ivLen, hdr, hdrLen, msg, msgLen, tag,
                               tagLen, &sEaxCtx);
}

int32_t CIPHER_Wrapper_EaxEnd(void)
{
    return eax_end(&sEaxCtx);
}
