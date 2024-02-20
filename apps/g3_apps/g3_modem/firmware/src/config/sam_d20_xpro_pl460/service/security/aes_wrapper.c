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

#include "aes_wrapper.h"
#include "crypto/common_crypto/MCHP_Crypto_Sym_Cipher.h"

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Data
// *****************************************************************************
// *****************************************************************************

/* AES context used in this wrapper */
static st_Crypto_Sym_BlockCtx aesWrapperContext;

// *****************************************************************************
// *****************************************************************************
// Section: Service Interface Functions
// *****************************************************************************
// *****************************************************************************

void AES_Wrapper_SetEncryptEcbKey(uint8_t *key)
{
    (void) Crypto_Sym_Aes_Init(&aesWrapperContext, CRYPTO_HANDLER_SW_WOLFCRYPT,
            CRYPTO_CIOP_ENCRYPT, CRYPTO_SYM_OPMODE_ECB,
            key, CRYPTO_AESKEYSIZE_128, NULL, 1);
}

void AES_Wrapper_EncryptEcb(uint8_t *in, uint8_t *out)
{
    (void) Crypto_Sym_Aes_Cipher(&aesWrapperContext, in, 16, out);
}
