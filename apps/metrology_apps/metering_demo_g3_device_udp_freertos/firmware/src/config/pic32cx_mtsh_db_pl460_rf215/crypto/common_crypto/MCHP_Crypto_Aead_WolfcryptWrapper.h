/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    MCHP_Crypto_Aead_WolfcryptWrapper.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_Initialize" and "APP_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

#ifndef MCHP_CRYPTO_AEAD_WOLFCRYPTWRAPPER_H
#define MCHP_CRYPTO_AEAD_WOLFCRYPTWRAPPER_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
#if (defined(HAVE_AESCCM) && defined(CRYPTO_AEAD_WC_AESCCM_EN))
crypto_Aead_Status_E Crypto_Aead_Wc_AesCcm_Init(void *ptr_aesCcmCtx, uint8_t *ptr_key, uint32_t keySize);
crypto_Aead_Status_E Crypto_Aead_Wc_AesCcm_Cipher(crypto_CipherOper_E cipherOper_en, void *ptr_aesCcmCtx, uint8_t *ptr_inputData, uint32_t dataLen, 
                                                    uint8_t *ptr_outData, uint8_t *ptr_nonce, uint32_t nonceLen, uint8_t *ptr_authTag,
                                                    uint32_t authTagLen, uint8_t *ptr_aad, uint32_t aadLen);
#endif /* HAVE_AESCCM && CRYPTO_AEAD_AESCCM_EN */

#if (defined(WOLFSSL_AES_EAX) && defined(CRYPTO_AEAD_WC_AESEAX_EN))
crypto_Aead_Status_E Crypto_Aead_Wc_AesEax_Init(void *ptr_aesEaxCtx, uint8_t *ptr_key, uint32_t keySize, 
                                                uint8_t *ptr_nonce, uint32_t nonceLen, uint8_t *ptr_aad, uint32_t aadLen);
crypto_Aead_Status_E Crypto_Aead_Wc_AesEax_Cipher(crypto_CipherOper_E cipherOper_en, void *ptr_aesEaxCtx, uint8_t *ptr_inputData, uint32_t dataLen, 
                                                    uint8_t *ptr_outData, uint8_t *ptr_aad, uint32_t aadLen);
crypto_Aead_Status_E Crypto_Aead_Wc_AesEax_Final(crypto_CipherOper_E cipherOper_en, void *ptr_aesEaxCtx, uint8_t *ptr_authTag, uint32_t authTagLen);

crypto_Aead_Status_E Crypto_Aead_Wc_AesEax_AddAadData(void *ptr_aesEaxCtx, uint8_t *ptr_aad, uint32_t aadLen);

crypto_Aead_Status_E Crypto_Aead_Wc_AesEax_EncDecAuthDirect(crypto_CipherOper_E cipherOper_en, uint8_t *ptr_inputData, uint32_t dataLen, 
                                                            uint8_t *ptr_outData, uint8_t *ptr_key, uint32_t keySize, uint8_t *ptr_nonce, 
                                                            uint32_t nonceLen, uint8_t *ptr_aad, uint32_t aadLen, uint8_t *ptr_authTag, uint32_t authTagLen);
#endif /* WOLFSSL_AES_EAX && CRYPTO_AEAD_WC_AESEAX_EN*/

#endif //MCHP_CRYPTO_AEAD_WOLFCRYPTWRAPPER_H