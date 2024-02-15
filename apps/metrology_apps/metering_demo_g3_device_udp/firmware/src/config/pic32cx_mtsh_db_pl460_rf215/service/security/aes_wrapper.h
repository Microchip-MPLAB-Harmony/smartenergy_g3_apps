/*******************************************************************************
  Header for the AES wrapper between G3 stack and AES

  Company:
    Microchip Technology Inc.

  File Name:
    aes_wrapper.h

  Summary:
    Interface definition of the AES wrapper between G3 and AES.

  Description:
    This file defines the interface for the wrapper between G3 and AES.
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

#ifndef AES_WRAPPER_H
#define AES_WRAPPER_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>

#ifdef __cplusplus // Provide C++ Compatibility
 extern "C" {
#endif

// *****************************************************************************
// *****************************************************************************
// Section: Service Interface Functions
// *****************************************************************************
// *****************************************************************************

//******************************************************************************
/* Function:
    void AES_Wrapper_ContextInit(void)

  Summary:
    Initializes AES context.

  Description:
    This function intializes AES context setting it to all zeros.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    uint8_t in1[CRYPT_AES_BLOCK_SIZE];
    uint8_t out1[CRYPT_AES_BLOCK_SIZE];
    uint8_t key[] = { some 16, 24, or 32 byte key };

    AES_Wrapper_ContextInit();
    AES_Wrapper_KeySet(key, sizeof(key));
    AES_Wrapper_Encrypt(in1, out1);
    AES_Wrapper_ContextFree();
    </code>

  Remarks:
    None.
*/

void AES_Wrapper_ContextInit(void);

//******************************************************************************
/* Function:
    void AES_Wrapper_ContextFree(void)

  Summary:
    Frees AES context.

  Description:
    This function frees AES context setting it to all zeros.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    uint8_t in1[CRYPT_AES_BLOCK_SIZE];
    uint8_t out1[CRYPT_AES_BLOCK_SIZE];
    uint8_t key[] = { some 16, 24, or 32 byte key };

    AES_Wrapper_ContextInit();
    AES_Wrapper_KeySet(key, sizeof(key));
    AES_Wrapper_Encrypt(in1, out1);
    AES_Wrapper_ContextFree();
    </code>

  Remarks:
    None.
*/

void AES_Wrapper_ContextFree(void);

//******************************************************************************
/* Function:
    void AES_Wrapper_Encrypt(const uint8_t *in, uint8_t *out)

  Summary:
    Encrypts one block of data.

  Description:
    This function encrypts one block of data, equal to the AES block size.

  Precondition:
    Key must be set earlier with a call to AES_Wrapper_KeySet.

  Parameters:
    in  - Pointer to buffer where the data to encrypt is located.
    out - Pointer to buffer to store the results of the encryption.

  Returns:
    None.

  Example:
    <code>
    uint8_t in1[CRYPT_AES_BLOCK_SIZE];
    uint8_t out1[CRYPT_AES_BLOCK_SIZE];
    uint8_t key[] = { some 16, 24, or 32 byte key };

    AES_Wrapper_KeySet(key, sizeof(key));
    AES_Wrapper_Encrypt(in1, out1);
    </code>

  Remarks:
    Input and output buffers must be equal in size (CRYPT_AES_BLOCK_SIZE).
*/

void AES_Wrapper_Encrypt(const uint8_t *in, uint8_t *out);

//******************************************************************************
/* Function:
    void AES_Wrapper_Decrypt(const uint8_t *in, uint8_t *out)

  Summary:
    Decrypts one block of data.

  Description:
    This function decrypts one block of data, equal to the AES block size.

  Precondition:
    Key must be set earlier with a call to AES_Wrapper_KeySet.

  Parameters:
    in  - Pointer to buffer where the data to decrypt is located.
    out - Pointer to buffer to store the results of the decryption.

  Returns:
    None.

  Example:
    <code>
    uint8_t in1[CRYPT_AES_BLOCK_SIZE];
    uint8_t out1[CRYPT_AES_BLOCK_SIZE];
    uint8_t key[] = { some 16, 24, or 32 byte key };

    AES_Wrapper_KeySet(key, sizeof(key));
    AES_Wrapper_Decrypt(in1, out1);
    </code>

  Remarks:
    Input and output buffers must be equal in size (CRYPT_AES_BLOCK_SIZE).
*/

void AES_Wrapper_Decrypt(const uint8_t *in, uint8_t *out);

//******************************************************************************
/* Function:
    void AES_Wrapper_KeySet(const uint8_t *key, uint32_t keyLen)

  Summary:
    Sets the key for further encryption and decryption.

  Description:
    This function sets the key for further encryption and decryption.

  Precondition:
    The crypto initialization routines should be called before calling this
    routine (in "SYS_Initialize").

  Parameters:
    key    - Pointer to buffer holding the key itself.
    keyLen - Length of key in bytes.

  Returns:
    None.

  Example:
    <code>
    uint8_t key[] = { some 16, 24, or 32 byte key };
    AES_Wrapper_KeySet(key, sizeof(key));
    </code>

  Remarks:
    None.
*/

void AES_Wrapper_KeySet(const uint8_t *key, uint32_t keyLen);

//******************************************************************************
/* Function:
    void AES_Wrapper_SetEncryptKey(const uint8_t *key, uint32_t keyLen)

  Summary:
    Sets the key for further encryption.

  Description:
    This function sets the key for further encryption.

  Precondition:
    The crypto initialization routines should be called before calling this
    routine (in "SYS_Initialize").

  Parameters:
    key     - Pointer to buffer holding the key itself.
    keyBits - Length of key in bytes.

  Returns:
    None.

  Example:
    <code>
    uint8_t key[] = { some 16, 24, or 32 byte key };
    AES_Wrapper_SetEncryptKey(key, sizeof(key));
    </code>

  Remarks:
    None.
*/

void AES_Wrapper_SetEncryptKey(const uint8_t *key, uint32_t keyLen);

//******************************************************************************
/* Function:
    void AES_Wrapper_EncryptEcb(const uint8_t *in, uint8_t *out)

  Summary:
    Encrypts one block of data in ECB mode.

  Description:
    This function encrypts one block of data, equal to the AES block size,
    in ECB mode.

  Precondition:
    Key must be set earlier with a call to AES_Wrapper_SetEncryptKey.

  Parameters:
    in  - Pointer to buffer where the data to encrypt is located.
    out - Pointer to buffer to store the results of the encryption.

  Returns:
    None.

  Example:
    <code>
    uint8_t in1[CRYPT_AES_BLOCK_SIZE];
    uint8_t out1[CRYPT_AES_BLOCK_SIZE];
    uint8_t key[] = { some 16, 24, or 32 byte key };

    AES_Wrapper_SetEncryptKey(key, sizeof(key));
    AES_Wrapper_EncryptEcb(in1, out1);
    </code>

  Remarks:
    Input and output buffers must be equal in size (CRYPT_AES_BLOCK_SIZE).
*/

void AES_Wrapper_EncryptEcb(const uint8_t *in, uint8_t *out);

#ifdef __cplusplus // Provide C++ Compatibility
}
#endif

#endif /* AES_WRAPPER_H */
