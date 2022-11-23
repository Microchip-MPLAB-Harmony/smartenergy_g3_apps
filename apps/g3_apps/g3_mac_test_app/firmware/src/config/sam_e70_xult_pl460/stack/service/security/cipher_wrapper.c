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

#include <string.h>
#include <stdbool.h>
#include <stdint.h>
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
Cmac sCmacCtx;

/* CCM context used in this wrapper */
Aes sCcmCtx;

/* EAX context used in this wrapper */
eax_ctx sEaxCtx;

// *****************************************************************************
// *****************************************************************************
// Section: File scope functions
// *****************************************************************************
// *****************************************************************************

int CIPHER_Wrapper_CmacStart(const unsigned char *key, unsigned int keyLen)
{
    return wc_InitCmac(&sCmacCtx, key, keyLen, AES_ENCRYPTION, NULL);
}

int CIPHER_Wrapper_CmacUpdate(const unsigned char *input, unsigned int ilen)
{
    return wc_CmacUpdate(&sCmacCtx, input, ilen);
}

int CIPHER_Wrapper_CmacFinish(unsigned char *output)
{
    unsigned int outputLen = AES_BLOCK_SIZE; 

    return wc_CmacFinal(&sCmacCtx, output, &outputLen);
}

int CIPHER_Wrapper_CcmSetkey(const unsigned char *key, unsigned int keyLen)
{
    return wc_AesCcmSetKey(&sCcmCtx, key, keyLen);
}

int CIPHER_Wrapper_CcmAuthDecrypt(unsigned int length, const unsigned char *iv,
		                  unsigned int ivLen, const unsigned char *add, 
                                  unsigned int addLen, 
                                  const unsigned char *input,
		                  unsigned char *output, 
                                  const unsigned char *tag, 
                                  unsigned int tagLen)
{
    return wc_AesCcmDecrypt(&sCcmCtx, output, input, length, iv, ivLen, tag, 
                            tagLen, add, addLen);
}

int CIPHER_Wrapper_CcmEncryptAndTag(unsigned int length, 
                                    const unsigned char *iv,
		                    unsigned int ivLen, 
                                    const unsigned char *add, 
                                    unsigned int addLen, 
                                    const unsigned char *input,
		                    unsigned char *output, unsigned char *tag, 
                                    unsigned int tagLen)
{
    return wc_AesCcmEncrypt(&sCcmCtx, output, input, length, iv, ivLen, tag, 
                            tagLen, add, addLen);
}

int CIPHER_Wrapper_EaxInitKey(const unsigned char *key, unsigned int keyLen)
{
    return eax_init_and_key(key, keyLen, &sEaxCtx);
}

int CIPHER_Wrapper_EaxEncrypt(const unsigned char *iv, unsigned int ivLen,
		              const unsigned char *hdr, unsigned int hdrLen, 
                              unsigned char *msg, unsigned int msgLen,
		              unsigned char *tag, unsigned int tagLen)
{
    return eax_encrypt_message(iv, ivLen, hdr, hdrLen, msg, msgLen, tag, 
                               tagLen, &sEaxCtx);
}

int CIPHER_Wrapper_EaxDecrypt(const unsigned char *iv, unsigned int ivLen,
		              const unsigned char *hdr, unsigned int hdrLen, 
                              unsigned char *msg, unsigned int msgLen,
		              const unsigned char *tag, unsigned int tagLen)
{
    return eax_decrypt_message(iv, ivLen, hdr, hdrLen, msg, msgLen, tag, 
                               tagLen, &sEaxCtx);
}

int CIPHER_Wrapper_EaxEnd(void) 
{
    return eax_end(&sEaxCtx);
}

#if defined(__cplusplus)
}
#endif
