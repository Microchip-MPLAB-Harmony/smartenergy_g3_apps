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

#ifndef _AES_WRAPPER_H
#define _AES_WRAPPER_H

#ifdef __cplusplus // Provide C++ Compatibility
 extern "C" {
#endif

//******************************************************************************
/* Function:
    int AES_Wrapper_Encrypt(const unsigned char *in, unsigned char *out)

  Summary:
	Encrypts one block of data.

  Description:
	This function encrypts one block of data, equal to the AES block size.

  Precondition:
	Key must be set earlier with a call to AES_Wrapper_KeySet.
	
  Parameters:
    in		- Pointer to buffer where the data to encrypt is located.
    out		- Pointer to buffer to store the results of the encryption.

  Returns:
	None.

  Example:
    <code>
	char in1[CRYPT_AES_BLOCK_SIZE];
    char out1[CRYPT_AES_BLOCK_SIZE];
    uint8_t key[] = { some 16, 24, or 32 byte key }

    AES_Wrapper_KeySet(key, sizeof(key));
    AES_Wrapper_Encrypt(in1, out1);
    </code>

  Remarks:
	Input and output buffers must be equal in size (CRYPT_AES_BLOCK_SIZE).
*/

void AES_Wrapper_Encrypt(const unsigned char *in, unsigned char *out);

//******************************************************************************
/* Function:
    int AES_Wrapper_Decrypt(const unsigned char *in, unsigned char *out)

  Summary:
	Decrypts one block of data.

  Description:
	This function decrypts one block of data, equal to the AES block size.

  Precondition:
	Key must be set earlier with a call to AES_Wrapper_KeySet.
	
  Parameters:
    in		- Pointer to buffer where the data to decrypt is located.
    out		- Pointer to buffer to store the results of the decryption.

  Returns:
	None.

  Example:
    <code>
	char in1[CRYPT_AES_BLOCK_SIZE];
    char out1[CRYPT_AES_BLOCK_SIZE];
    uint8_t key[] = { some 16, 24, or 32 byte key }
	
    AES_Wrapper_KeySet(key, sizeof(key));
    AES_Wrapper_Decrypt(in1, out1);
    </code>

  Remarks:
	Input and output buffers must be equal in size (CRYPT_AES_BLOCK_SIZE).
*/

void AES_Wrapper_Decrypt(const unsigned char *in, unsigned char *out);

//******************************************************************************
/* Function:
    void AES_Wrapper_KeySet(const unsigned char *key, unsigned int keyLen)

  Summary:
    Sets the key for further encryption and decryption.

  Description:
    This function sets the key for further encryption and decryption.

  Precondition:
	The crypto initialization routines should be called before calling
    this routine (in "SYS_Initialize").
	
  Parameters:
    key		- Pointer to buffer holding the key itself.
    keyLen	- Length of key in bytes.

  Returns:
	None.

  Example:
    <code>
	uint8_t key[] = { some 16, 24, or 32 byte key }
    AES_Wrapper_KeySet(key, sizeof(key));
    </code>

  Remarks:
    None.
*/

void AES_Wrapper_KeySet(const unsigned char *key, unsigned int keyLen);

//******************************************************************************
/* Function:
    void AES_Wrapper_SetEncryptKey(const unsigned char *key, unsigned int keyLen)

  Summary:
    Sets the key for further encryption.

  Description:
    This function sets the key for further encryption.

  Precondition:
	The crypto initialization routines should be called before calling
    this routine (in "SYS_Initialize").
	
  Parameters:
    key		- Pointer to buffer holding the key itself.
    keyBits	- Length of key in bytes.

  Returns:
	None.

  Example:
    <code>
    uint8_t key[] = { some 16, 24, or 32 byte key }
    AES_Wrapper_SetEncryptKey(key, sizeof(key));
    </code>

  Remarks:
    None.
*/

void AES_Wrapper_SetEncryptKey(const unsigned char *key, unsigned int keyLen);

//******************************************************************************
/* Function:
    int AES_Wrapper_EncryptEcb(const unsigned char *in, unsigned char *out)

  Summary:
	Encrypts one block of data in ECB mode.

  Description:
	This function encrypts one block of data, equal to the AES block size,
	in ECB mode.

  Precondition:
	Key must be set earlier with a call to AES_Wrapper_SetEncryptKey.
	
  Parameters:
    in		- Pointer to buffer where the data to encrypt is located.
    out		- Pointer to buffer to store the results of the encryption.

  Returns:
	None.

  Example:
    <code>
	char in1[CRYPT_AES_BLOCK_SIZE];
    char out1[CRYPT_AES_BLOCK_SIZE];
    uint8_t key[] = { some 16, 24, or 32 byte key }
	
    AES_Wrapper_SetEncryptKey(key, sizeof(key));
    AES_Wrapper_EncryptEcb(in1, out1);
    </code>

  Remarks:
	Input and output buffers must be equal in size (CRYPT_AES_BLOCK_SIZE).
*/

void AES_Wrapper_EncryptEcb(const unsigned char *in, unsigned char *out);

#ifdef __cplusplus // Provide C++ Compatibility
 }
#endif

#endif /* _AES_WRAPPER_H */
