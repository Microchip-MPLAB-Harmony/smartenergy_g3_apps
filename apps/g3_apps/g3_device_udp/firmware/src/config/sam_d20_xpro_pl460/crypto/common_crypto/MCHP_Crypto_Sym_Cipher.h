/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    MCHP_Crypto_Sym_Cipher.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_Initialize" and "APP_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

#ifndef MCHP_CRYPTO_SYM_CIPHER_H
#define MCHP_CRYPTO_SYM_CIPHER_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "crypto/common_crypto/MCHP_Crypto_Common.h"
#include "crypto/common_crypto/MCHP_Crypto_Sym_Config.h"

// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************

typedef enum
{
    CRYPTO_SYM_ERROR_CIPNOTSUPPTD = -127,   //Error when Cipher Algorithm is not supported by Crypto software component
    CRYPTO_SYM_ERROR_CTX = -126,            //Error when Context pointer is NULL
    CRYPTO_SYM_ERROR_KEY = -125,            //Error when Key length is above or below its range Or Key pointer is NULL
    CRYPTO_SYM_ERROR_HDLR = -124,           //Error when Handler Type is Invalid
    CRYPTO_SYM_ERROR_OPMODE = -123,         //Error when Operation Mode (ECB, CBC or CFB etc.) is Invalid
    CRYPTO_SYM_ERROR_IV = -122,             //Error when Initialization Vector pointer is NULL or Its length is 0  
    CRYPTO_SYM_ERROR_INPUTDATA = -121,      //Error when input data length is 0 or its pointer is NULL 
    CRYPTO_SYM_ERROR_OUTPUTDATA = -120,     //Error when Output Data pointer is NULL        
    CRYPTO_SYM_ERROR_CIPOPER = -119,        //Error when Cipher Operation (Encryption or Decryption) is Invalid
    CRYPTO_SYM_ERROR_SID = -118,            //Error when Session ID is 0 or Its value is more than Max. session configure in configurations
    CRYPTO_SYM_ERROR_ARG = -117,            //Error when any other argument is Invalid. For example Tweak value pointer is NULL for AES-XTS etc.
    CRYPTO_SYM_ERROR_CIPFAIL = -116,        //Error when Encryption or Decryption operation failed due to any reason
    CRYPTO_SYM_CIPHER_SUCCESS = 0,        
}crypto_Sym_Status_E;

typedef enum
{
    CRYPTO_SYM_OPMODE_INVALID = 0,

#if (defined(CRYPTO_SYM_AESECB_EN) || defined(CRYPTO_SYM_CAMECB_EN) || defined(CRYPTO_SYM_TDESECB_EN))           
    CRYPTO_SYM_OPMODE_ECB = 1,
#endif
            
#if (defined(CRYPTO_SYM_AESCBC_EN) || defined(CRYPTO_SYM_CAMCBC_EN) || defined(CRYPTO_SYM_TDESCBC_EN))            
    CRYPTO_SYM_OPMODE_CBC = 2,
#endif

#if (defined(CRYPTO_SYM_AESOFB_EN) || defined(CRYPTO_SYM_CAMOFB_EN))              
    CRYPTO_SYM_OPMODE_OFB = 3,
#endif
            
#if (defined(CRYPTO_SYM_AESCFB1_EN) || defined(CRYPTO_SYM_CAMCFB1_EN))            
    CRYPTO_SYM_OPMODE_CFB1 = 4,
#endif
            
#if (defined(CRYPTO_SYM_AESCFB8_EN) || defined(CRYPTO_SYM_CAMCFB8_EN))            
    CRYPTO_SYM_OPMODE_CFB8 = 5,
#endif
            
#if (defined(CRYPTO_SYM_AESCFB64_EN) || defined(CRYPTO_SYM_CAMCFB64_EN))          
    CRYPTO_SYM_OPMODE_CFB64 = 6,
#endif
            
#if (defined(CRYPTO_SYM_AESCFB128_EN) || defined(CRYPTO_SYM_CAMCFB128_EN))            
    CRYPTO_SYM_OPMODE_CFB128 = 7,
#endif
            
#if (defined(CRYPTO_SYM_AESCTR_EN) || defined(CRYPTO_SYM_CAMCTR_EN))            
    CRYPTO_SYM_OPMODE_CTR = 8,
#endif
            
#if (defined( CRYPTO_SYM_AESXTS_EN) || defined(CRYPTO_SYM_CAMXTS_EN))            
    CRYPTO_SYM_OPMODE_XTS = 9,
#endif
            
    CRYPTO_SYM_OPMODE_MAX
}crypto_Sym_OpModes_E;



typedef struct 
{
    uint32_t cryptoSessionID;
    crypto_HandlerType_E symHandlerType_en;
    uint8_t *ptr_key;
    uint8_t *ptr_initVect;
    uint8_t arr_symDataCtx[70];
}st_Crypto_Sym_StreamCtx;

typedef struct 
{
    uint32_t cryptoSessionID;
    crypto_HandlerType_E symHandlerType_en;
    crypto_CipherOper_E symCipherOper_en;
    crypto_Sym_OpModes_E symAlgoMode_en;
    uint8_t *ptr_key;
    uint32_t symKeySize;
    uint8_t *ptr_initVect;
    uint8_t arr_symDataCtx[500];
}st_Crypto_Sym_BlockCtx;


//AES
#ifdef CRYPTO_SYM_AES_ENABLE
crypto_Sym_Status_E Crypto_Sym_Aes_Init(st_Crypto_Sym_BlockCtx *ptr_aesCtx_st, crypto_HandlerType_E handlerType_en, crypto_CipherOper_E cipherOpType_en, 
                                                crypto_Sym_OpModes_E opMode_en, uint8_t *ptr_key, uint32_t keyLen, uint8_t *ptr_initVect, uint32_t sessionID);

crypto_Sym_Status_E Crypto_Sym_Aes_Cipher(st_Crypto_Sym_BlockCtx *ptr_aesCtx_st, uint8_t *ptr_inputData, uint32_t dataLen, uint8_t *ptr_outData);


crypto_Sym_Status_E Crypto_Sym_Aes_EncryptDirect(crypto_HandlerType_E handlerType_en, crypto_Sym_OpModes_E opMode_en, uint8_t *ptr_inputData, 
                                                        uint32_t dataLen, uint8_t *ptr_outData, uint8_t *ptr_key, uint32_t keyLen, uint8_t *ptr_initVect, uint32_t sessionID);

crypto_Sym_Status_E Crypto_Sym_Aes_DecryptDirect(crypto_HandlerType_E handlerType_en, crypto_Sym_OpModes_E opMode_en, uint8_t *ptr_inputData, 
                                                        uint32_t dataLen, uint8_t *ptr_outData, uint8_t *ptr_key, uint32_t keyLen, uint8_t *ptr_initVect, uint32_t sessionID);
#ifdef CRYPTO_SYM_AESXTS_EN
crypto_Sym_Status_E Crypto_Sym_AesXts_Cipher(st_Crypto_Sym_BlockCtx *ptr_aesCtx_st, uint8_t *ptr_inputData, uint32_t dataLen, uint8_t *ptr_outData, uint8_t *ptr_tweak);
#endif /* CRYPTO_SYM_AESXTS_EN */
#endif /*CRYPTO_SYM_AES_ENABLE */

//Camellia
#ifdef CRYPTO_SYM_CAMELLIA_ENABLE
crypto_Sym_Status_E Crypto_Sym_Camellia_Init(st_Crypto_Sym_BlockCtx *ptr_camCtx_st, crypto_HandlerType_E handlerType_en, crypto_CipherOper_E cipherOpType_en, 
                                                crypto_Sym_OpModes_E opMode_en, uint8_t *ptr_key, uint32_t keyLen, uint8_t *ptr_initVect, uint32_t sessionID);
crypto_Sym_Status_E Crypto_Sym_Camellia_Cipher(st_Crypto_Sym_BlockCtx *ptr_camCtx_st, uint8_t *ptr_inputData, uint32_t dataLen, uint8_t *ptr_outData);
crypto_Sym_Status_E Crypto_Sym_Camellia_EncryptDirect(crypto_HandlerType_E handlerType_en, crypto_Sym_OpModes_E opMode_en, uint8_t *ptr_inputData, 
                                                        uint32_t dataLen, uint8_t *ptr_outData, uint8_t *ptr_key, uint32_t keyLen, uint8_t *ptr_initVect, uint32_t sessionID);
crypto_Sym_Status_E Crypto_Sym_Camellia_DecryptDirect(crypto_HandlerType_E handlerType_en, crypto_Sym_OpModes_E opMode_en, uint8_t *ptr_inputData, 
                                                        uint32_t dataLen, uint8_t *ptr_outData, uint8_t *ptr_key, uint32_t keyLen, uint8_t *ptr_initVect, uint32_t sessionID);
#endif /* CRYPTO_SYM_CAMELLIA_ENABLE */

//DES3 or Triple-DES
#ifdef CRYPTO_SYM_TDES_ENABLE
crypto_Sym_Status_E Crypto_Sym_Tdes_Init(st_Crypto_Sym_BlockCtx *ptr_tdesCtx_st, crypto_HandlerType_E handlerType_en, crypto_CipherOper_E cipherOpType_en, 
                                                crypto_Sym_OpModes_E opMode_en, uint8_t *ptr_key, uint8_t *ptr_initVect, uint32_t sessionID);
crypto_Sym_Status_E Crypto_Sym_Tdes_Cipher(st_Crypto_Sym_BlockCtx *ptr_tdesCtx_st, uint8_t *ptr_inputData, uint32_t dataLen, uint8_t *ptr_outData);
crypto_Sym_Status_E Crypto_Sym_Tdes_EncryptDirect(crypto_HandlerType_E handlerType_en, crypto_Sym_OpModes_E opMode_en, uint8_t *ptr_inputData, 
                                                        uint32_t dataLen, uint8_t *ptr_outData, uint8_t *ptr_key, uint8_t *ptr_initVect, uint32_t sessionID);
crypto_Sym_Status_E Crypto_Sym_Tdes_DecryptDirect(crypto_HandlerType_E handlerType_en, crypto_Sym_OpModes_E opMode_en, uint8_t *ptr_inputData, 
                                                        uint32_t dataLen, uint8_t *ptr_outData, uint8_t *ptr_key, uint8_t *ptr_initVect, uint32_t sessionID);
#endif /* CRYPTO_SYM_TDES_ENABLE */

//AES-KEYWRAP
#ifdef CRYPTO_SYM_AESKEYWRAP_ENABLE
crypto_Sym_Status_E Crypto_Sym_AesKeyWrap_Init(st_Crypto_Sym_BlockCtx *ptr_aesCtx_st, crypto_HandlerType_E handlerType_en, crypto_CipherOper_E cipherOpType_en, 
                                                                                      uint8_t *ptr_key, uint32_t keyLen, uint8_t *ptr_initVect, uint32_t sessionID);
crypto_Sym_Status_E Crypto_Sym_AesKeyWrap_Cipher(st_Crypto_Sym_BlockCtx *ptr_aesCtx_st, uint8_t *ptr_inputData, uint32_t dataLen, uint8_t *ptr_outData);
crypto_Sym_Status_E Crypto_Sym_AesKeyWrapDirect(crypto_HandlerType_E handlerType_en, uint8_t *ptr_inputData, uint32_t inputLen, 
                                                    uint8_t *ptr_outData, uint8_t *ptr_key, uint32_t keyLen, uint8_t *ptr_initVect, uint32_t sessionID);
crypto_Sym_Status_E Crypto_Sym_AesKeyUnWrapDirect(crypto_HandlerType_E handlerType_en, uint8_t *ptr_inputData, uint32_t inputLen, 
                                                    uint8_t *ptr_outData, uint8_t *ptr_key, uint32_t keyLen, uint8_t *ptr_initVect, uint32_t sessionID);
#endif /* CRYPTO_SYM_AESKEYWRAP_ENABLE */

//CHACHA-20
#ifdef CRYPTO_SYM_CHACHA20_ENABLE
crypto_Sym_Status_E Crypto_Sym_ChaCha20_Init(st_Crypto_Sym_StreamCtx *ptr_chaChaCtx_st, crypto_HandlerType_E handlerType_en, uint8_t *ptr_key, uint8_t *ptr_initVect, uint32_t sessionID);

crypto_Sym_Status_E Crypto_Sym_ChaCha20_Cipher(st_Crypto_Sym_StreamCtx *ptr_chaChaCtx_st, uint8_t *ptr_inputData, uint32_t dataLen, uint8_t *ptr_outData);

crypto_Sym_Status_E Crypto_Sym_ChaCha20Direct(crypto_HandlerType_E handlerType_en, uint8_t *ptr_inputData, uint32_t dataLen, 
                                                                uint8_t *ptr_outData, uint8_t *ptr_key, uint8_t *ptr_initVect, uint32_t sessionID);

#endif /* CRYPTO_SYM_CHACHA20_ENABLE */

#endif //MCHP_CRYPTO_SYM_CIPHER_H
