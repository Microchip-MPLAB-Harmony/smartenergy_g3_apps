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
