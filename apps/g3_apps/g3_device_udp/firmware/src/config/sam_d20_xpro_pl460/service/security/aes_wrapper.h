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
    void AES_Wrapper_SetEncryptEcbKey(uint8_t *key)

  Summary:
    Sets the key for further AES ECB encryption.

  Description:
    This function sets the 16-byte key for further AES ECB encryption.

  Precondition:
    None.

  Parameters:
    key - Pointer to buffer holding the 16-byte key itself.

  Returns:
    None.

  Example:
    <code>
    uint8_t key[16];
    AES_Wrapper_SetEncryptEcbKey(key);
    </code>

  Remarks:
    None.
*/

void AES_Wrapper_SetEncryptEcbKey(uint8_t *key);

//******************************************************************************
/* Function:
    void AES_Wrapper_EncryptEcb(uint8_t *in, uint8_t *out)

  Summary:
    Encrypts one block of data in AES ECB mode.

  Description:
    This function encrypts one block of data, equal to the AES block size (16
    bytes), in ECB mode.

  Precondition:
    Key must be set earlier with a call to AES_Wrapper_SetEncryptEcbKey.

  Parameters:
    in  - Pointer to buffer where the data to encrypt is located.
    out - Pointer to buffer to store the results of the encryption.

  Returns:
    None.

  Example:
    <code>
    uint8_t in1[16];
    uint8_t out1[16];
    uint8_t key[16];

    AES_Wrapper_SetEncryptEcbKey(key);
    AES_Wrapper_EncryptEcb(in1, out1);
    </code>

  Remarks:
    Input and output buffers must be equal in size (16 bytes).
*/

void AES_Wrapper_EncryptEcb(uint8_t *in, uint8_t *out);

#ifdef __cplusplus // Provide C++ Compatibility
}
#endif

#endif /* AES_WRAPPER_H */
