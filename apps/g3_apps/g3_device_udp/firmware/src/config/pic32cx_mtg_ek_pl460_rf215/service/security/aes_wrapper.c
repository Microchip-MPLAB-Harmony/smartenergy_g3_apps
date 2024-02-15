/*******************************************************************************
  Source for the AES wrapper between G3 stack and AES

  Company:
    Microchip Technology Inc.

  File Name:
    aes_wrapper.c

  Summary:
    Interface implementation of the AES wrapper between G3 and AES.

  Description:
    This file implements the interface for the wrapper between G3 and AES.
*******************************************************************************/

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

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <string.h>
#include <stdbool.h>
#include "definitions.h"
#include "aes_wrapper.h"

// *****************************************************************************
// *****************************************************************************
// Section: Static Data
// *****************************************************************************
// *****************************************************************************

/* Key in byte format */
static uint8_t sKey[256];

/* Key size in bytes */
static uint32_t sKeyLen;

/* AES context used in this wrapper */
static CRYPT_AES_CTX sAesCtx;

// *****************************************************************************
// *****************************************************************************
// Section: Service Interface Functions
// *****************************************************************************
// *****************************************************************************

void AES_Wrapper_ContextInit(void)
{
    (void) memset(&sAesCtx, 0, sizeof(sAesCtx));
}

void AES_Wrapper_ContextFree(void)
{
    (void) memset(&sAesCtx, 0, sizeof(sAesCtx));
}

void AES_Wrapper_Encrypt(const uint8_t *in, uint8_t *out)
{
    /* Set the AES key */
    (void) CRYPT_AES_KeySet(&sAesCtx, sKey, sKeyLen, NULL, CRYPT_AES_ENCRYPTION);

    /* Trigger the AES */
    (void) CRYPT_AES_DIRECT_Encrypt(&sAesCtx, out, in);
}

void AES_Wrapper_Decrypt(const uint8_t *in, uint8_t *out)
{
    /* Set the AES key */
    (void) CRYPT_AES_KeySet(&sAesCtx, sKey, sKeyLen, NULL, CRYPT_AES_DECRYPTION);

    /* Trigger the AES */
    (void) CRYPT_AES_DIRECT_Decrypt(&sAesCtx, out, in);
}

void AES_Wrapper_KeySet(const uint8_t *key, uint32_t keyLen)
{
    /* Store the key */
    (void) memcpy(sKey, key, keyLen);

    /* Store the key size */
    sKeyLen = keyLen;
}

void AES_Wrapper_SetEncryptKey(const uint8_t *key, uint32_t keyLen)
{
    (void) CRYPT_AES_KeySet(&sAesCtx, key, keyLen, NULL, CRYPT_AES_ENCRYPTION);
}

void AES_Wrapper_EncryptEcb(const uint8_t *in, uint8_t *out)
{
    (void) CRYPT_AES_DIRECT_Encrypt(&sAesCtx, out, in);
}
