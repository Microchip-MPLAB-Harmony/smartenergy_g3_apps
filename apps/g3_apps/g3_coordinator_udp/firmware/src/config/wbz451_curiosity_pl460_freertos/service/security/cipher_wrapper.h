 /*******************************************************************************
  Header for the cipher wrapper between G3 stack and Crypto

  Company:
    Microchip Technology Inc.

  File Name:
    cipher_wrapper.h

  Summary:
    Interface definition of the wrapper between G3 and Crypto.

  Description:
    This file defines the interface for the wrapper between G3 and Crypto.
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

#ifndef CIPHER_WRAPPER_H
#define CIPHER_WRAPPER_H

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
// Section: Constant Definitions
// *****************************************************************************
// *****************************************************************************

/* Successful function return */
#define CIPHER_WRAPPER_RETURN_GOOD    0

// *****************************************************************************
// *****************************************************************************
// Section: Service Interface Functions
// *****************************************************************************
// *****************************************************************************

//******************************************************************************
/* Function:
    int32_t CIPHER_Wrapper_AesCmacDirect(uint8_t *input, uint32_t inputLen,
                                         uint8_t *outputMac, uint8_t *key)

  Summary:
    Performs AES-CMAC to generate the MAC in single step without initialization.

  Description:
    This function performs AES-CMAC to generate the MAC in single step without
    initialization. To verify, the user can compare the generated MAC with the
    MAC received from sender with message. The size of the key is 16 bytes. The
    size of the MAC is equal to the AES block size (16 bytes).

  Precondition:
    None.

  Parameters:
    input     - Pointer to buffer holding the input data.
    inputLen  - Length of the input data in bytes.
    outputMac - Pointer to store the output data (MAC). The size of the buffer
                must be equal or larger than the AES block size (16 bytes).
    key       - Pointer to buffer holding the 16-byte key itself.

  Returns:
    - CIPHER_WRAPPER_RETURN_GOOD: Successful process.
    - Any other value: Error in the process.

  Example:
    <code>
    int32_t ret;
    uint8_t in1[] = { some input data };
    uint8_t out1[16];
    uint8_t key[16] = { some key };

    ret = CIPHER_Wrapper_AesCmacDirect(in1, sizeof(in1), out1, key);
    </code>

  Remarks:
    None.
*/

int32_t CIPHER_Wrapper_AesCmacDirect(uint8_t *input, uint32_t inputLen,
                                     uint8_t *outputMac, uint8_t *key);

//******************************************************************************
/* Function:
    int32_t CIPHER_Wrapper_AesCcmSetkey(uint8_t *key)

  Summary:
    Initializes the AES-CCM context and sets the encryption key.

  Description:
    This function initializes the AES-CCM context and sets the 16-byte
    encryption key.

  Precondition:
    None.

  Parameters:
    key - Pointer to buffer holding the 16-byte key itself.

  Returns:
    - CIPHER_WRAPPER_RETURN_GOOD: Successful initialization.
    - Any other value: Error in the initialization.

  Example:
    <code>
    int32_t ret;
    uint8_t key[16] = { some key };
    ret = CIPHER_Wrapper_AesCcmSetkey(key);
    </code>

  Remarks:
    None.
*/

int32_t CIPHER_Wrapper_AesCcmSetkey(uint8_t *key);

//******************************************************************************
/* Function:
    int32_t CIPHER_Wrapper_AesCcmAuthDecrypt(uint8_t *data, uint32_t dataLen,
                                             uint8_t *iv, uint32_t ivLen,
                                             uint8_t *aad, uint32_t aadLen,
                                             uint8_t *tag, uint32_t tagLen)

  Summary:
    Performs AES-CCM authenticated decryption of a buffer.

  Description:
    This function performs a AES-CCM authenticated decryption of a buffer.

  Precondition:
    Key must be set earlier with a call to CIPHER_Wrapper_AesCcmSetkey.

  Parameters:
    data    - Pointer to buffer holding the input ciphered data. The output
              plain data is stored in the same buffer.
    dataLen - Length of the input/output data in bytes.
    iv      - Pointer to initialization vector (nonce).
    ivLen   - Length of the nonce in bytes.
    aad     - Pointer to additional authentication data field.
    aadLen  - Length of additional authentication data in bytes.
    tag     - Pointer to buffer holding the authentication tag.
    tagLen  - Length of the authentication tag in bytes.

  Returns:
    - CIPHER_WRAPPER_RETURN_GOOD: Successful decryption and authentication.
    - Any other value: Error in the decryption or authentication.

  Example:
    <code>
    int32_t ret;
    uint8_t nonce[] = { some initialization nonce };
    uint8_t data[] = { some ciphered message };
    uint8_t aad[] = { some additional authentication data };
    uint8_t tag[] = { authentication tag received for verification };
    uint8_t key[16] = { some key };

    ret = CIPHER_Wrapper_AesCcmSetkey(key, sizeof(key));
    ret = CIPHER_Wrapper_AesCcmAuthDecrypt(data, sizeof(data),
                                           nonce, sizeof(nonce),
                                           aad, sizeof(aad),
                                           tag, sizeof(tag));
    </code>

  Remarks:
    None.
*/

int32_t CIPHER_Wrapper_AesCcmAuthDecrypt(uint8_t *data, uint32_t dataLen,
                                         uint8_t *iv, uint32_t ivLen,
                                         uint8_t *aad, uint32_t aadLen,
                                         uint8_t *tag, uint32_t tagLen);

//******************************************************************************
/* Function:
    int32_t CIPHER_Wrapper_AesCcmEncryptAndTag(uint8_t *data, uint32_t dataLen,
                                               uint8_t *iv, uint32_t ivLen,
                                               uint8_t *aad, uint32_t aadLen,
                                               uint8_t *tag, uint32_t tagLen)

  Summary:
    Performs AES-CCM authenticated encryption of a buffer.

  Description:
    This function performs AES-CCM authenticated encryption of a buffer.

  Precondition:
    Key must be set earlier with a call to CIPHER_Wrapper_AesCcmSetkey.

  Parameters:
    data    - Pointer to buffer holding the input plain data. The output
              ciphered data is stored in the same buffer.
    dataLen - Length of the input/output data in bytes.
    iv      - Pointer to initialization vector (nonce).
    ivLen   - Length of the nonce in bytes.
    aad     - Pointer to additional authentication data field.
    aadLen  - Length of additional authentication data in bytes.
    tag     - Pointer to store the authentication tag.
    tagLen  - Length of the authentication tag in bytes.

  Returns:
    - CIPHER_WRAPPER_RETURN_GOOD: Successful encryption and authentication.
    - Any other value: Error in the encryption.

  Example:
    <code>
    int32_t ret;
    uint8_t nonce[] = { some initialization nonce };
    uint8_t data[] = { some plain message };
    uint8_t aad[] = { some additional authentication data };
    uint8_t tag[];
    uint8_t key[16] = { some key };

    ret = CIPHER_Wrapper_AesCcmSetkey(key);
    ret = CIPHER_Wrapper_AesCcmEncryptAndTag(data, sizeof(data),
                                             nonce, sizeof(nonce),
                                             aad, sizeof(aad),
                                             tag, sizeof(tag));
    </code>

  Remarks:
    None.
*/

int32_t CIPHER_Wrapper_AesCcmEncryptAndTag(uint8_t *data, uint32_t dataLen,
                                           uint8_t *iv, uint32_t ivLen,
                                           uint8_t *aad, uint32_t aadLen,
                                           uint8_t *tag, uint32_t tagLen);

//******************************************************************************
/* Function:
    int32_t CIPHER_Wrapper_AesEaxEncrypt(uint8_t *data, uint32_t dataLen,
                                         uint8_t *iv, uint32_t ivLen,
                                         uint8_t *aad, uint32_t aadLen,
                                         uint8_t *tag, uint32_t tagLen,
                                         uint8_t *key)

  Summary:
    Performs AES-EAX authenticated encryption of a buffer.

  Description:
    This function performs AES-EAX encryption of a buffer and generates
    authentication tag. The size of the key is 16 bytes.

  Precondition:
    None.

  Parameters:
    data    - Pointer to buffer holding the input plain data. The output
              ciphered data is stored in the same buffer.
    dataLen - Length of the input/output data in bytes.
    iv      - Pointer to initialization vector (nonce).
    ivLen   - Length of the nonce in bytes.
    aad     - Pointer to additional authentication data field.
    aadLen  - Length of additional authentication data in bytes.
    tag     - Pointer to store the authentication tag.
    tagLen  - Length of the authentication tag in bytes.
    key     - Pointer to buffer holding the 16-byte key itself.

  Returns:
    - CIPHER_WRAPPER_RETURN_GOOD: Successful encryption and authentication.
    - Any other value: Error in the encryption.

  Example:
    <code>
    int32_t ret;
    uint8_t nonce[] = { some initialization nonce };
    uint8_t data[] = { some plain message };
    uint8_t aad[] = { some additional authentication data };
    uint8_t tag[];
    uint8_t key[16] = { some key };

    ret = CIPHER_Wrapper_AesEaxEncrypt(data, sizeof(data),
                                       nonce, sizeof(nonce),
                                       aad, sizeof(aad),
                                       tag, sizeof(tag),
                                       key);
    </code>

  Remarks:
    None.
*/

int32_t CIPHER_Wrapper_AesEaxEncrypt(uint8_t *data, uint32_t dataLen,
                                     uint8_t *iv, uint32_t ivLen,
                                     uint8_t *aad, uint32_t aadLen,
                                     uint8_t *tag, uint32_t tagLen,
                                     uint8_t *key);

//******************************************************************************
/* Function:
    int32_t CIPHER_Wrapper_AesEaxDecrypt(uint8_t *data, uint32_t dataLen,
                                         uint8_t *iv, uint32_t ivLen,
                                         uint8_t *aad, uint32_t aadLen,
                                         uint8_t *tag, uint32_t tagLen,
                                         uint8_t *key)

  Summary:
    Performs AES-EAX authenticated decryption of a buffer.

  Description:
    This function performs AES-EAX encryption of a buffer and authenticates
    using authentication tag. The size of the key is 16 bytes.

  Precondition:
    None.

  Parameters:
    data    - Pointer to buffer holding the input ciphered data. The output
              plain data is stored in the same buffer.
    dataLen - Length of the input/output data in bytes.
    iv      - Pointer to initialization vector (nonce).
    ivLen   - Length of the nonce in bytes.
    aad     - Pointer to additional authentication data field.
    aadLen  - Length of additional authentication data in bytes.
    tag     - Pointer to buffer holding the authentication tag.
    tagLen  - Length of the authentication tag in bytes.
    key     - Pointer to buffer holding the 16-byte key itself.

  Returns:
    - CIPHER_WRAPPER_RETURN_GOOD: Successful decryption and authentication.
    - Any other value: Error in the decryption or authentication.

  Example:
    <code>
    int32_t ret;
    uint8_t nonce[] = { some initialization nonce };
    uint8_t data[] = { some ciphered message };
    uint8_t aad[] = { some additional authentication data };
    uint8_t tag[] = { authentication tag received for verification };
    uint8_t key[16] = { some key };

    ret = CIPHER_Wrapper_AesEaxDecrypt(data, sizeof(data),
                                       nonce, sizeof(nonce),
                                       aad, sizeof(aad),
                                       tag, sizeof(tag),
                                       key);
    </code>

  Remarks:
    None.
*/

int32_t CIPHER_Wrapper_AesEaxDecrypt(uint8_t *data, uint32_t dataLen,
                                     uint8_t *iv, uint32_t ivLen,
                                     uint8_t *aad, uint32_t aadLen,
                                     uint8_t *tag, uint32_t tagLen,
                                     uint8_t *key);

#ifdef __cplusplus // Provide C++ Compatibility
 }
#endif

#endif /* CIPHER_WRAPPER_H */
