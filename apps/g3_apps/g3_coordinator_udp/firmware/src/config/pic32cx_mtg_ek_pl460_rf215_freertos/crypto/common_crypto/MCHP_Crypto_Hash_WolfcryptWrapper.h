/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    MCHP_Crypto_Hash_WolfCryptWrapper.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_Initialize" and "APP_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

#ifndef MCHP_CRYPTO_HASH_WOLFCRYPTWRAPPER_H
#define MCHP_CRYPTO_HASH_WOLFCRYPTWRAPPER_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#define CRYPTO_HASH_WC_SHACTX_SIZE (110)


// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************
#ifdef CRYPTO_HASH_WC_MD5_EN
crypto_Hash_Status_E Crypto_Hash_Wc_Md5Digest(uint8_t *ptr_data, uint32_t dataLen, uint8_t *ptr_digest);
crypto_Hash_Status_E Crypto_Hash_Wc_Md5Init(void *ptr_md5Ctx_st);
crypto_Hash_Status_E Crypto_Hash_Wc_Md5Update(void *ptr_md5Ctx_st, uint8_t *ptr_data, uint32_t dataLen);
crypto_Hash_Status_E Crypto_Hash_Wc_Md5Final(void *ptr_md5Ctx_st, uint8_t *ptr_digest);
#endif /* CRYPTO_HASH_WC_MD5_EN */

#ifdef CRYPTO_HASH_WC_RIPEMD160_EN
crypto_Hash_Status_E Crypto_Hash_Wc_Ripemd160Digest(uint8_t *ptr_data, uint32_t dataLen, uint8_t *ptr_digest);
crypto_Hash_Status_E Crypto_Hash_Wc_Ripemd160Init(void *ptr_ripemdCtx_st);
crypto_Hash_Status_E Crypto_Hash_Wc_Ripemd160Update(void *ptr_ripemdCtx_st, uint8_t *ptr_data, uint32_t dataLen);
crypto_Hash_Status_E Crypto_Hash_Wc_Ripemd160Final(void *ptr_ripemdCtx_st, uint8_t *ptr_digest);
#endif /* CRYPTO_HASH_WC_RIPEMD160_EN */

#if (defined(CRYPTO_HASH_WC_SHA1_EN) || defined(CRYPTO_HASH_WC_SHA2_EN) || defined(CRYPTO_HASH_WC_SHA3_EN))
crypto_Hash_Status_E Crypto_Hash_Wc_ShaDigest(uint8_t *ptr_data, uint32_t dataLen, uint8_t *ptr_digest, crypto_Hash_Algo_E hashAlgo_en);
crypto_Hash_Status_E Crypto_Hash_Wc_ShaInit(void *ptr_shaCtx_st, crypto_Hash_Algo_E hashAlgo_en);
crypto_Hash_Status_E Crypto_Hash_Wc_ShaUpdate(void *ptr_shaCtx_st, uint8_t *ptr_data, uint32_t dataLen, crypto_Hash_Algo_E hashAlgo_en);
crypto_Hash_Status_E Crypto_Hash_Wc_ShaFinal(void *ptr_shaCtx_st, uint8_t *ptr_digest, crypto_Hash_Algo_E hashAlgo_en);
#endif /* CRYPTO_HASH_WC_SHA1_EN || CRYPTO_HASH_WC_SHA2_EN || CRYPTO_HASH_WC_SHA3_EN */

#ifdef CRYPTO_HASH_WC_SHA3_SHAKE_EN
crypto_Hash_Status_E Crypto_Hash_Wc_ShakeDigest(uint8_t *ptr_data, uint32_t dataLen, uint8_t *ptr_digest, uint32_t digestLen, crypto_Hash_Algo_E hashAlgo_en);
crypto_Hash_Status_E Crypto_Hash_Wc_ShakeInit(void *ptr_shakeCtx_st, crypto_Hash_Algo_E hashAlgo_en);
crypto_Hash_Status_E Crypto_Hash_Wc_ShakeUpdate(void *ptr_shakeCtx_st, uint8_t *ptr_data, uint32_t dataLen, crypto_Hash_Algo_E hashAlgo_en);
crypto_Hash_Status_E Crypto_Hash_Wc_ShakeFinal(void *ptr_shakeCtx_st, uint8_t *ptr_digest, uint32_t digestLen, crypto_Hash_Algo_E hashAlgo_en);
#endif /* CRYPTO_HASH_WC_SHA3_SHAKE_EN */

#ifdef CRYPTO_HASH_WC_BLAKE2_EN
crypto_Hash_Status_E Crypto_Hash_Wc_BlakeDigest(uint8_t *ptr_data, uint32_t dataLen, uint8_t *ptr_blakeKey, uint32_t keySize, uint8_t *ptr_digest, uint32_t digestLen, crypto_Hash_Algo_E blakeAlgorithm_en);
crypto_Hash_Status_E Crypto_Hash_Wc_BlakeInit(void *ptr_blakeCtx_st, crypto_Hash_Algo_E hashAlgo_en, uint8_t *ptr_blakeKey, uint32_t keySize, uint32_t digestLen);   
crypto_Hash_Status_E Crypto_Hash_Wc_BlakeUpdate(void *ptr_blakeCtx_st, uint8_t *ptr_data, uint32_t dataLen, crypto_Hash_Algo_E hashAlgo_en);
crypto_Hash_Status_E Crypto_Hash_Wc_BlakeFinal(void *ptr_blakeCtx_st, uint8_t *ptr_digest, uint32_t digestLen, crypto_Hash_Algo_E hashAlgo_en);
#endif /* CRYPTO_HASH_WC_BLAKE2_EN */
#endif //MCHP_CRYPTO_HASH_WOLFCRYPTWRAPPER_H