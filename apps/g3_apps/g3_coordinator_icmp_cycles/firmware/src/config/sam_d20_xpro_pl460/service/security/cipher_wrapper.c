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
/*******************************************************************************
* Copyright (C) 2024 Microchip Technology Inc. and its subsidiaries.
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
