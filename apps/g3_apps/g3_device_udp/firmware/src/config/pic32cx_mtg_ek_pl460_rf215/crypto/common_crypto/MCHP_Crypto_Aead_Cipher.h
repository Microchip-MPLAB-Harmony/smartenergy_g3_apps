/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    MCHP_Crypto_Aead_Cipher.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_Initialize" and "APP_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

#ifndef MCHP_CRYPTO_AEAD_CIPHER_H
#define MCHP_CRYPTO_AEAD_CIPHER_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "MCHP_Crypto_Common.h"
#include "MCHP_Crypto_Aead_Config.h"

typedef enum
{
    CRYPTO_AEAD_ERROR_CIPNOTSUPPTD = -127,
    CRYPTO_AEAD_ERROR_CTX = -126,
    CRYPTO_AEAD_ERROR_KEY = -125,
    CRYPTO_AEAD_ERROR_HDLR = -124,
    CRYPTO_AEAD_ERROR_INPUTDATA = -123,
    CRYPTO_AEAD_ERROR_OUTPUTDATA = -122,        
    CRYPTO_AEAD_ERROR_NONCE = -121,
    CRYPTO_AEAD_ERROR_AUTHTAG = -120,
    CRYPTO_AEAD_ERROR_AAD = -119,        
    CRYPTO_AEAD_ERROR_CIPOPER = -118,
    CRYPTO_AEAD_ERROR_SID = -117,  ////session ID Error
    CRYPTO_AEAD_ERROR_ARG = -116,
    CRYPTO_AEAD_ERROR_CIPFAIL = -115,
    CRYPTO_AEAD_ERROR_AUTHFAIL = -114,        
    CRYPTO_AEAD_CIPHER_SUCCESS = 0,        
}crypto_Aead_Status_E;


// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************
typedef struct
{
    uint32_t cryptoSessionID;
    crypto_HandlerType_E aeadHandlerType_en;
    uint8_t *ptr_key;
    uint32_t aeadKeySize;
    uint8_t arr_aeadDataCtx[512];
}st_Crypto_Aead_AesCcm_ctx;

typedef struct
{
    uint32_t cryptoSessionID;
    crypto_HandlerType_E aeadHandlerType_en;
    crypto_CipherOper_E aeadCipherOper_en;
    uint8_t *ptr_key;
    uint32_t aeadKeySize;
    uint8_t *ptr_aeadNonce;
    uint32_t aeadNonuceLen;
    uint8_t arr_aeadDataCtx[512];
}st_Crypto_Aead_AesEax_ctx;
// *****************************************************************************
#ifdef CRYPTO_AEAD_AESCCM_EN 
crypto_Aead_Status_E Crypto_Aead_AesCcm_Init(st_Crypto_Aead_AesCcm_ctx *ptr_aesCcmCtx_st, crypto_HandlerType_E handlerType_en, 
                                              uint8_t *ptr_key, uint32_t keyLen, uint32_t sessionID);

crypto_Aead_Status_E Crypto_Aead_AesCcm_Cipher(st_Crypto_Aead_AesCcm_ctx *ptr_aesCcmCtx_st, crypto_CipherOper_E cipherOper_en, uint8_t *ptr_inputData, uint32_t dataLen, 
                                                    uint8_t *ptr_outData, uint8_t *ptr_nonce, uint32_t nonceLen, uint8_t *ptr_authTag,
                                                    uint32_t authTagLen, uint8_t *ptr_aad, uint32_t aadLen);
#endif /* CRYPTO_AEAD_AESCCM_EN */

#ifdef CRYPTO_AEAD_AESEAX_EN
crypto_Aead_Status_E Crypto_Aead_AesEax_Init(st_Crypto_Aead_AesEax_ctx *ptr_aesEaxCtx_st, crypto_HandlerType_E handlerType_en, crypto_CipherOper_E cipherOper_en, 
                                                uint8_t *ptr_key, uint32_t keyLen, uint8_t *ptr_nonce, uint32_t nonceLen, uint8_t *ptr_aad, uint32_t aadLen, uint32_t sessionID);

crypto_Aead_Status_E Crypto_Aead_AesEax_Cipher(st_Crypto_Aead_AesEax_ctx *ptr_aesEaxCtx_st, uint8_t *ptr_inputData, uint32_t dataLen, 
                                                    uint8_t *ptr_outData, uint8_t *ptr_aad, uint32_t aadLen);

crypto_Aead_Status_E Crypto_Aead_AesEax_Final(st_Crypto_Aead_AesEax_ctx *ptr_aesEaxCtx_st, uint8_t *ptr_authTag, uint32_t authTagLen);

crypto_Aead_Status_E Crypto_Aead_AesEax_AddAadData(st_Crypto_Aead_AesEax_ctx *ptr_aesEaxCtx_st, uint8_t *ptr_aad, uint32_t aadLen);

crypto_Aead_Status_E Crypto_Aead_AesEax_EncryptAuthDirect(crypto_HandlerType_E handlerType_en, uint8_t *ptr_inputData, uint32_t dataLen, 
                                                            uint8_t *ptr_outData, uint8_t *ptr_key, uint32_t keyLen, uint8_t *ptr_nonce, 
                                                            uint32_t nonceLen, uint8_t *ptr_aad, uint32_t aadLen, uint8_t *ptr_authTag, uint32_t authTagLen, uint32_t sessionID);

crypto_Aead_Status_E Crypto_Aead_AesEax_DecryptAuthDirect(crypto_HandlerType_E handlerType_en, uint8_t *ptr_inputData, uint32_t dataLen, 
                                                            uint8_t *ptr_outData, uint8_t *ptr_key, uint32_t keyLen, uint8_t *ptr_nonce, 
                                                            uint32_t nonceLen, uint8_t *ptr_aad, uint32_t aadLen, uint8_t *ptr_authTag, uint32_t authTagLen, uint32_t sessionID);
#endif /* CRYPTO_AEAD_AESEAX_EN */
#endif //MCHP_CRYPTO_AEAD_CIPHER_H
