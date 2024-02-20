/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    MCHP_Crypto_Sym_WolfcryptWrapper.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_Initialize" and "APP_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

#ifndef MCHP_CRYPTO_SYM_WOLFCRYPTWRAPPER_H
#define MCHP_CRYPTO_SYM_WOLFCRYPTWRAPPER_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
//AES
#if (defined(CRYPTO_SYM_WC_AES_EN) && (!defined(NO_AES)) )
crypto_Sym_Status_E Crypto_Sym_Wc_Aes_Init(void *ptr_aesCtx, crypto_CipherOper_E symCipherOper_en, uint8_t *ptr_key, uint32_t keySize, uint8_t *ptr_initVect);
crypto_Sym_Status_E Crypto_Sym_Wc_Aes_Encrypt(void *ptr_aesCtx, crypto_Sym_OpModes_E symAlgoMode_en, uint8_t *ptr_inputData, uint32_t dataLen, uint8_t *ptr_outData);
crypto_Sym_Status_E Crypto_Sym_Wc_Aes_Decrypt(void *ptr_aesCtx, crypto_Sym_OpModes_E symAlgoMode_en, uint8_t *ptr_inputData, uint32_t dataLen, uint8_t *ptr_outData);

crypto_Sym_Status_E Crypto_Sym_Wc_AesXts_Init(void *ptr_aesCtx, crypto_CipherOper_E symCipherOper_en, uint8_t *ptr_key, uint32_t keySize);
crypto_Sym_Status_E Crypto_Sym_Wc_AesXts_Encrypt(void *ptr_aesXtsCtx, uint8_t *ptr_inputData, uint32_t dataLen, uint8_t *ptr_outData, uint8_t *ptr_tweak);
crypto_Sym_Status_E Crypto_Sym_Wc_AesXts_Decrypt(void *ptr_aesXtsCtx, uint8_t *ptr_inputData, uint32_t dataLen, uint8_t *ptr_outData, uint8_t *ptr_tweak);

crypto_Sym_Status_E Crypto_Sym_Wc_Aes_EncryptDirect(crypto_Sym_OpModes_E symAlgoMode_en, uint8_t *ptr_inputData, uint32_t dataLen, uint8_t *ptr_outData,
                                                        uint8_t *ptr_key, uint32_t keySize, uint8_t *ptr_initVect);
crypto_Sym_Status_E Crypto_Sym_Wc_Aes_DecryptDirect(crypto_Sym_OpModes_E symAlgoMode_en, uint8_t *ptr_inputData, uint32_t dataLen, uint8_t *ptr_outData,
                                                        uint8_t *ptr_key, uint32_t keySize, uint8_t *ptr_initVect);
#endif /* CRYPTO_SYM_WC_AES_EN  && !NO_AES*/

//Camellia
#if (defined(CRYPTO_SYM_WC_CAMELLIA_EN) && defined(HAVE_CAMELLIA))
crypto_Sym_Status_E Crypto_Sym_Wc_Camellia_Init(void *ptr_camCtx, uint8_t *ptr_key, uint32_t keySize, uint8_t *ptr_initVect);
crypto_Sym_Status_E Crypto_Sym_Wc_Camellia_Encrypt(void *ptr_camCtx, crypto_Sym_OpModes_E symAlgoMode_en, uint8_t *ptr_inputData, uint32_t dataLen, uint8_t *ptr_outData);
crypto_Sym_Status_E Crypto_Sym_Wc_Camellia_Decrypt(void *ptr_camCtx, crypto_Sym_OpModes_E symAlgoMode_en, uint8_t *ptr_inputData, uint32_t dataLen, uint8_t *ptr_outData);
crypto_Sym_Status_E Crypto_Sym_Wc_Camellia_EncryptDirect(crypto_Sym_OpModes_E symAlgoMode_en, uint8_t *ptr_inputData, uint32_t dataLen, uint8_t *ptr_outData,
                                                                    uint8_t *ptr_key, uint32_t keySize, uint8_t *ptr_initVect);
crypto_Sym_Status_E Crypto_Sym_Wc_Camellia_DecryptDirect(crypto_Sym_OpModes_E symAlgoMode_en, uint8_t *ptr_inputData, uint32_t dataLen, uint8_t *ptr_outData,
                                                                    uint8_t *ptr_key, uint32_t keySize, uint8_t *ptr_initVect);
#endif /* CRYPTO_SYM_WC_CAMELLIA_EN && HAVE_CAMELLIA */

//TDES or 3DES
#if (defined(CRYPTO_SYM_WC_TDES_EN) && !(defined(NO_DES3)))
crypto_Sym_Status_E Crypto_Sym_Wc_Tdes_Init(void *ptr_tdesCtx, crypto_CipherOper_E symCipherOper_en, uint8_t *ptr_key, uint8_t *ptr_initVect);
crypto_Sym_Status_E Crypto_Sym_Wc_Tdes_Encrypt(void *ptr_tdesCtx, crypto_Sym_OpModes_E symAlgoMode_en, uint8_t *ptr_inputData, uint32_t dataLen, uint8_t *ptr_outData);
crypto_Sym_Status_E Crypto_Sym_Wc_Tdes_Decrypt(void *ptr_tdesCtx, crypto_Sym_OpModes_E symAlgoMode_en, uint8_t *ptr_inputData, uint32_t dataLen, uint8_t *ptr_outData);
crypto_Sym_Status_E Crypto_Sym_Wc_Tdes_EncryptDirect(crypto_Sym_OpModes_E symAlgoMode_en, uint8_t *ptr_inputData, uint32_t dataLen, uint8_t *ptr_outData,
                                                                    uint8_t *ptr_key, uint8_t *ptr_initVect);
crypto_Sym_Status_E Crypto_Sym_Wc_Tdes_DecryptDirect(crypto_Sym_OpModes_E symAlgoMode_en, uint8_t *ptr_inputData, uint32_t dataLen, uint8_t *ptr_outData,
                                                                    uint8_t *ptr_key, uint8_t *ptr_initVect);
#endif /* CRYPTO_SYM_WC_TDES_EN && !NO_DES3 */

//AES-KW
#if (defined(CRYPTO_SYM_WC_AESKW_EN) && defined(HAVE_AES_KEYWRAP))
crypto_Sym_Status_E Crypto_Sym_Wc_AesKeyWrap_Init(void *ptr_aesCtx, crypto_CipherOper_E symCipherOper_en, uint8_t *ptr_key, uint32_t keySize, uint8_t *ptr_initVect);
crypto_Sym_Status_E Crypto_Sym_Wc_AesKeyWrap(void *ptr_aesCtx, uint8_t *ptr_inputData, uint32_t inputLen, uint8_t *ptr_outData, uint32_t outputLen, uint8_t *ptr_initVect);
crypto_Sym_Status_E Crypto_Sym_Wc_AesKeyUnWrap(void *ptr_aesCtx, uint8_t *ptr_inputData, uint32_t inputLen, uint8_t *ptr_outData, uint32_t outputLen, uint8_t *ptr_initVect);
crypto_Sym_Status_E Crypto_Sym_Wc_AesKeyWrapDirect(uint8_t *ptr_inputData, uint32_t inputLen, uint8_t *ptr_outData, uint32_t outputLen,
                                                                        uint8_t *ptr_key, uint32_t keySize, uint8_t *ptr_initVect);
crypto_Sym_Status_E Crypto_Sym_Wc_AesKeyUnWrapDirect(uint8_t *ptr_inputData, uint32_t inputLen, uint8_t *ptr_outData, uint32_t outputLen,
                                                                        uint8_t *ptr_key, uint32_t keySize, uint8_t *ptr_initVect);
#endif /* CRYPTO_SYM_WC_AESKW_EN && HAVE_AES_KEYWRAP */
//ChaCha
#if (defined(CRYPTO_SYM_WC_CHACHA20_EN) && defined(HAVE_CHACHA))
crypto_Sym_Status_E Crypto_Sym_Wc_ChaCha_Init(void *ptr_chaChaCtx, uint8_t *ptr_key, uint32_t keySize, uint8_t *ptr_initVect);
crypto_Sym_Status_E Crypto_Sym_Wc_ChaChaUpdate(void *ptr_chaChaCtx, uint8_t *ptr_inputData, uint32_t dataLen, uint8_t *ptr_outData);
crypto_Sym_Status_E Crypto_Sym_Wc_ChaChaDirect(uint8_t *ptr_inputData, uint32_t dataLen, uint8_t *ptr_outData, uint8_t *ptr_key, uint32_t keySize, uint8_t *ptr_initVect);
#endif /* CRYPTO_SYM_WC_CHACHA20_EN && HAVE_CHACHA*/


#endif //MCHP_CRYPTO_SYM_WOLFCRYPTWRAPPER_H