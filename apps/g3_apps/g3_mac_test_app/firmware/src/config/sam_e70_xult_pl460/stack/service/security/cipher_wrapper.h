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

#ifndef _CIPHER_WRAPPER_H
#define _CIPHER_WRAPPER_H

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

//******************************************************************************
/* Function:
    int CIPHER_Wrapper_CmacStart(const unsigned char *key, unsigned int keyLen)

  Summary:
	Sets the CMAC key and prepares to authenticate the input data.
	
  Description:
	This function sets the CMAC key and prepares to authenticate the input data.

  Precondition:
	The crypto initialization routines should be called before calling
    this routine (in "SYS_Initialize").
	
  Parameters:
    key		- Pointer to buffer holding the key itself.
    keyLen	- Length of key in bytes.

  Returns:
	- CIPHER_WRAPPER_RETURN_GOOD   - Successful initalization.
	- Any other value	           - Error in the initialization.

  Example:
    <code>
	int ret;
    uint8_t key[] = { some key }
    ret = CIPHER_Wrapper_CmacStart(key, sizeof(key));
    </code>

  Remarks:
	None.
*/

int CIPHER_Wrapper_CmacStart(const unsigned char *key, unsigned int keyLen);

//******************************************************************************
/* Function:
    int CIPHER_Wrapper_CmacUpdate(const unsigned char *input, unsigned int ilen)

  Summary:
	Feeds an input buffer into an ongoing CMAC computation.

  Description:
	This function feeds an input buffer into an ongoing CMAC computation.

  Precondition:
	The computation must be started earlier with a call to 
    CIPHER_Wrapper_CmaStart.
	
  Parameters:
    input		- Buffer holding the input data.
    ilen		- Lenght of the input data.

  Returns:
	- CIPHER_WRAPPER_RETURN_GOOD   - Successful process.
	- Any other value	           - Error in the process.

  Example:
    <code>
	int ret;
	uint8_t in1[] = { some input data };
    uint8_t key[] = { some key }
	
    ret = CIPHER_Wrapper_CmacStart(key, sizeof(key));
    ret = CIPHER_Wrapper_CmacUpdate(in1, sizeof(in1));
    </code>

  Remarks:
	None.
*/

int CIPHER_Wrapper_CmacUpdate(const unsigned char *input, unsigned int ilen);

//******************************************************************************
/* Function:
    int CIPHER_Wrapper_CmacFinish(unsigned char *output)

  Summary:
	Finishes the CMAC operation and writes the result to the output buffer.

  Description:
	This function finishes the CMAC operation and writes the result to the 
	output buffer.

  Precondition:
	The computation must be started earlier with a call to 
    CIPHER_Wrapper_CmacStart.
	
  Parameters:
    output		- Buffer holding the output data.

  Returns:
	- CIPHER_WRAPPER_RETURN_GOOD   - Successful process.
	- Any other value	           - Error in the process.

  Example:
    <code>
	int ret;
	uint8_t in1[] = { some input data };
	uint8_t out1[];
    uint8_t key[] = { some key }
	
    ret = CIPHER_Wrapper_CmacStart(key, sizeof(key));
    ret = CIPHER_Wrapper_CmacUpdate(in1, sizeof(in1));
	ret = CIPHER_Wrapper_CmacFinish(out1);
    </code>

  Remarks:
	None.
*/

int CIPHER_Wrapper_CmacFinish(unsigned char *output);

//******************************************************************************
/* Function:
    int CIPHER_Wrapper_CcmSetkey(const unsigned char *key, unsigned int keyLen)

  Summary:
	Initializes the CCM context and sets the encryption key.
	
  Description:
	This function initializes the CCM context and sets the encryption key.

  Precondition:
	The crypto initialization routines should be called before calling
    this routine (in "SYS_Initialize").
	
  Parameters:
    key		- Pointer to buffer holding the key itself.
    keyLen	- Length of key in bytes.

  Returns:
	- CIPHER_WRAPPER_RETURN_GOOD   - Successful initalization.
	- Any other value	           - Error in the initialization.

  Example:
    <code>
	int ret;
    uint8_t key[] = { some key }
    ret = CIPHER_Wrapper_CcmSetkey(key, sizeof(key));
    </code>

  Remarks:
	None.
*/

int CIPHER_Wrapper_CcmSetkey(const unsigned char *key, unsigned int keyLen);

//******************************************************************************
/* Function:
    int CIPHER_Wrapper_CcmAuthDecrypt(unsigned int length, 
                                      const unsigned char *iv,
		                              unsigned int ivLen, 
                                      const unsigned char *add, 
						              unsigned int addLen, 
                                      const unsigned char *input,
		                              unsigned char *output, 
                                      const unsigned char *tag, 
						        	  unsigned int tagLen)

  Summary:
	Performs a CCM authenticated decryption of a buffer.

  Description:
	This function performs a CCM authenticated decryption of a buffer. 

  Precondition:
	Key must be set earlier with a call to CIPHER_Wrapper_CcmSetkey.
	
  Parameters:
	length  - Length of the input data in bytes.
    iv      - Initialization vector (nonce).
    ivLen   - Length of the nonce in bytes.
    add     - Additional data field.
    addLen  - Length of additional data in bytes.
    input   - Buffer holding the input data.
    output  - Buffer holding the output data.
    tag     - Buffer holding the authentication field.
    tagLen  - Length of the authentication field in bytes.

  Returns:
	- CIPHER_WRAPPER_RETURN_GOOD   - Successful decryption.
	- Any other value	           - Error in the decryption.

  Example:
    <code>
	int ret;
	nonce[] = { initialize nonce };
    cipher[] = { encrypted message };
    plain[sizeof(cipher)];
    authIn[] = { some 16 byte authentication input };
    tag[AES_BLOCK_SIZE] = { authentication tag received for verification };
    uint8_t key[] = { some key }

    ret = CIPHER_Wrapper_CcmSetkey(key, sizeof(key));
    ret = CIPHER_Wrapper_CcmAuthDecrypt(sizeof(plain), nonce, sizeof(nonce),
		                                authIn, sizeof(authIn), cipher, plain, 
						                tag, sizeof(tag));
    </code>

  Remarks:
	None.
*/

int CIPHER_Wrapper_CcmAuthDecrypt(unsigned int length, const unsigned char *iv,
		                          unsigned int ivLen, const unsigned char *add, 
						          unsigned int addLen, 
                                  const unsigned char *input,
		                          unsigned char *output, 
                                  const unsigned char *tag, 
						    	  unsigned int tagLen);

//******************************************************************************
/* Function:
    int CIPHER_Wrapper_CcmEncryptAndTag(unsigned int length, 
                                        const unsigned char *iv,
		                                unsigned int ivLen, 
                                        const unsigned char *add, 
		    						    unsigned int addLen, 
                                        const unsigned char *input,
		                                unsigned char *output, 
                                        unsigned char *tag, 
                                        unsigned int tagLen)

  Summary:
	Performs a CCM authenticated encryption of a buffer.

  Description:
	This function performs a CCM authenticated encryption of a buffer. 

  Precondition:
	Key must be set earlier with a call to CIPHER_Wrapper_CcmSetkey.
	
  Parameters:
	length  - Length of the input data in bytes.
    iv      - Initialization vector (nonce).
    ivLen   - Length of the nonce in bytes.
    add     - Additional data field.
    addLen  - Length of additional data in bytes.
    input   - Buffer holding the input data.
    output  - Buffer holding the output data.
    tag     - Buffer holding the authentication field.
    tagLen  - Length of the authentication field in bytes.

  Returns:
	- CIPHER_WRAPPER_RETURN_GOOD   - Successful decryption.
	- Any other value	           - Error in the decryption.

  Example:
    <code>
	int ret;
	nonce[] = { initialize nonce };
    plain[] = { some plain text message };
    cipher[sizeof(plain)];
    authIn[] = { some 16 byte authentication input };
    tag[AES_BLOCK_SIZE]; // will store authentication code
    uint8_t key[] = { some key }

    ret = CIPHER_Wrapper_CcmSetkey(key, sizeof(key));
    ret = CIPHER_Wrapper_CcmEncryptAndTag(sizeof(cipher), nonce, sizeof(nonce),
		                                  authIn, sizeof(authIn), plain, cipher, 
						                  tag, sizeof(tag));
    </code>

  Remarks:
	None.
*/

int CIPHER_Wrapper_CcmEncryptAndTag(unsigned int length, 
                                    const unsigned char *iv,
		                            unsigned int ivLen, 
                                    const unsigned char *add, 
									unsigned int addLen, 
                                    const unsigned char *input,
		                            unsigned char *output, 
                                    unsigned char *tag, 
									unsigned int tagLen);

//******************************************************************************
/* Function:
    int CIPHER_Wrapper_EaxInitKey(const unsigned char *key, unsigned int keyLen)

  Summary:
	Initializes the EAX context and sets the key.
	
  Description:
	This function initializes the EAX context and sets the key.

  Precondition:
	The crypto initialization routines should be called before calling
    this routine (in "SYS_Initialize").
	
  Parameters:
    key		- Pointer to buffer holding the key itself.
    keyLen	- Length of key in bytes.

  Returns:
	- CIPHER_WRAPPER_RETURN_GOOD   - Successful initalization.
	- Any other value	           - Error in the initialization.

  Example:
    <code>
	int ret;
    uint8_t key[] = { some key }
    ret = CIPHER_Wrapper_EaxInitKey(key, sizeof(key));
    </code>

  Remarks:
	None.
*/

int CIPHER_Wrapper_EaxInitKey(const unsigned char *key, unsigned int keyLen);

//******************************************************************************
/* Function:
    int CIPHER_Wrapper_EaxEncrypt(const unsigned char *iv, unsigned int ivLen,
		                          const unsigned char *hdr, unsigned int hdrLen, 
						    	  unsigned char *msg, unsigned int msgLen,
		                          unsigned char *tag, unsigned int tagLen)

  Summary:
	Encrypts a message using the EAX context created earlier.

  Description:
	This function encrypts a message using the EAX context created earlier.

  Precondition:
	Key must be set earlier with a call to CIPHER_Wrapper_EaxInitKey.
	
  Parameters:
    iv      - Initialization vector (nonce).
    ivLen   - Length of the nonce in bytes.
    hdr     - Buffer holding the header.
    hdrLen  - Length of header in bytes.
    msg     - Buffer holding the message data.
    msgLen  - Length of the message data.
    tag     - Buffer holding the tag.
    tagLen  - Length of the tag in bytes.

  Returns:
	- CIPHER_WRAPPER_RETURN_GOOD   - Successful decryption.
	- Any other value	           - Error in the decryption.

  Example:
    <code>
	int ret;
	nonce[] = { initialize nonce };
    header[] = { some header };
    message[] = { some plain text message };
    tag[AES_BLOCK_SIZE]; // will store authentication code
    uint8_t key[] = { some key }

    ret = CIPHER_Wrapper_EaxInitKey(key, sizeof(key));
    ret = CIPHER_Wrapper_EaxEncrypt(nonce, sizeof(nonce), hdr, sizeof(hdr), 
	                                message, sizeof(message), tag, sizeof(tag));
    </code>

  Remarks:
	None.
*/

int CIPHER_Wrapper_EaxEncrypt(const unsigned char *iv, unsigned int ivLen,
		                      const unsigned char *hdr, unsigned int hdrLen, 
							  unsigned char *msg, unsigned int msgLen,
		                      unsigned char *tag, unsigned int tagLen);

//******************************************************************************
/* Function:
    int CIPHER_Wrapper_EaxDecrypt(const unsigned char *iv, unsigned int ivLen,
		                          const unsigned char *hdr, unsigned int hdrLen, 
						    	  unsigned char *msg, unsigned int msgLen,
		                          unsigned char *tag, unsigned int tagLen)

  Summary:
	Decrypts a message using the EAX context created earlier.

  Description:
	This function decrypts a message using the EAX context created earlier.

  Precondition:
	Key must be set earlier with a call to CIPHER_Wrapper_EaxInitKey.
	
  Parameters:
    iv      - Initialization vector (nonce).
    ivLen   - Length of the nonce in bytes.
    hdr     - Buffer holding the header.
    hdrLen  - Length of header in bytes.
    msg     - Buffer holding the message data.
    msgLen  - Length of the message data.
    tag     - Buffer holding the tag.
    tagLen  - Length of the tag in bytes.

  Returns:
	- CIPHER_WRAPPER_RETURN_GOOD   - Successful decryption.
	- Any other value	           - Error in the decryption.

  Example:
    <code>
	int ret;
	nonce[] = { initialize nonce };
    header[] = { some header };
    message[] = { some cipher text message };
    tag[AES_BLOCK_SIZE]; // will store authentication code
    uint8_t key[] = { some key }

    ret = CIPHER_Wrapper_EaxInitKey(key, sizeof(key));
    ret = CIPHER_Wrapper_EaxDecrypt(nonce, sizeof(nonce), hdr, sizeof(hdr), 
	                                message, sizeof(message), tag, sizeof(tag));
    </code>

  Remarks:
	None.
*/

int CIPHER_Wrapper_EaxDecrypt(const unsigned char *iv, unsigned int ivLen,
		                      const unsigned char *hdr, unsigned int hdrLen, 
							  unsigned char *msg, unsigned int msgLen,
		                      const unsigned char *tag, unsigned int tagLen);

//******************************************************************************
/* Function:
    int CIPHER_Wrapper_EaxEnd(void)

  Summary:
	Cleans up the EAX context and ends the EAX operation.
	
  Description:
	This function cleans up the EAX context and ends the EAX operation.

  Precondition:
	None.
	
  Parameters:
    None.

  Returns:
	- CIPHER_WRAPPER_RETURN_GOOD   - Successful termination.
	- Any other value	           - Error in the termination.

  Example:
    <code>
	int ret;
    ret = CIPHER_Wrapper_EaxEnd();
    </code>

  Remarks:
	None.
*/

int CIPHER_Wrapper_EaxEnd(void);

#ifdef __cplusplus // Provide C++ Compatibility
 }
#endif

#endif /* _CIPHER_WRAPPER_H */
