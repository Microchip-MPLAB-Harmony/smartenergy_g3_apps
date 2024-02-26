/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    MCHP_Crypto_Hash.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_Initialize" and "APP_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

#ifndef MCHP_CRYPTO_HASH_H
#define MCHP_CRYPTO_HASH_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "crypto/common_crypto/MCHP_Crypto_Common.h"
#include "crypto/common_crypto/MCHP_Crypto_Hash_Config.h"
// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************
typedef enum {
	CRYPTO_HASH_INVALID = 0,
            
#ifdef CRYPTO_HASH_SHA1_EN            
	CRYPTO_HASH_SHA1 = 1,
#endif /* CRYPTO_HASH_SHA1_EN */
            
#ifdef CRYPTO_HASH_SHA2_224_EN            
	CRYPTO_HASH_SHA2_224 = 2,
#endif /* CRYPTO_HASH_SHA2_224_EN */
            
#ifdef CRYPTO_HASH_SHA2_256_EN            
	CRYPTO_HASH_SHA2_256 = 3,
#endif /* CRYPTO_HASH_SHA2_256_EN */
            
#ifdef CRYPTO_HASH_SHA2_384_EN            
	CRYPTO_HASH_SHA2_384 = 4,
#endif /* CRYPTO_HASH_SHA2_384_EN */
            
#ifdef CRYPTO_HASH_SHA2_512_EN            
	CRYPTO_HASH_SHA2_512 = 5,
#endif /* CRYPTO_HASH_SHA2_512_EN */

#ifdef CRYPTO_HASH_SHA2_512_224_EN            
	CRYPTO_HASH_SHA2_512_224 = 6,
#endif /* CRYPTO_HASH_SHA2_512_224_EN */

#ifdef CRYPTO_HASH_SHA2_512_256_EN            
	CRYPTO_HASH_SHA2_512_256 = 7,
#endif /* CRYPTO_HASH_SHA2_512_256_EN */            

#ifdef CRYPTO_HASH_SHA3_224_EN            
	CRYPTO_HASH_SHA3_224 = 8,
#endif /* CRYPTO_HASH_SHA3_224_EN */

#ifdef CRYPTO_HASH_SHA3_256_EN            
	CRYPTO_HASH_SHA3_256 = 9,
#endif /* CRYPTO_HASH_SHA3_256_EN */

#ifdef CRYPTO_HASH_SHA3_384_EN            
	CRYPTO_HASH_SHA3_384 = 10,
#endif /* CRYPTO_HASH_SHA3_384_EN */

#ifdef CRYPTO_HASH_SHA3_512_EN                   
	CRYPTO_HASH_SHA3_512 = 11,
#endif /* CRYPTO_HASH_SHA3_512_EN */            
 
#ifdef CRYPTO_HASH_SHA3_SHAKE128_EN            
	CRYPTO_HASH_SHA3_SHAKE128 = 12,
#endif /* CRYPTO_HASH_SHA3_SHAKE128_EN */
            
#ifdef CRYPTO_HASH_SHA3_SHAKE256_EN            
	CRYPTO_HASH_SHA3_SHAKE256 = 13,
#endif /* CRYPTO_HASH_SHA3_SHAKE256_EN */            

#ifdef CRYPTO_HASH_BLAKE2B_EN 
	CRYPTO_HASH_BLAKE2B = 14,
#endif /* CRYPTO_HASH_BLAKE2B_EN */
            
#ifdef CRYPTO_HASH_BLAKE2S_EN            
	CRYPTO_HASH_BLAKE2S = 15,
#endif /* CRYPTO_HASH_BLAKE2S_EN */            
            
#ifdef CRYPTO_HASH_MD5_EN            
	CRYPTO_HASH_MD5 = 16,
#endif /* CRYPTO_HASH_MD5_EN */

#ifdef CRYPTO_HASH_RIPEMD160_EN           
	CRYPTO_HASH_RIPEMD160 = 17,
#endif /* CRYPTO_HASH_RIPEMD160_EN */
            
	CRYPTO_HASH_MAX
}crypto_Hash_Algo_E;

typedef enum {
    CRYPTO_HASH_ERROR_NOTSUPPTED = -127,
    CRYPTO_HASH_ERROR_CTX = -126,
    CRYPTO_HASH_ERROR_INPUTDATA = -125,
    CRYPTO_HASH_ERROR_OUTPUTDATA = -124,
    CRYPTO_HASH_ERROR_SID = -123,
    CRYPTO_HASH_ERROR_ALGO = -122,
    CRYPTO_HASH_ERROR_KEY = -121,
    CRYPTO_HASH_ERROR_ARG = -120,
    CRYPTO_HASH_ERROR_HDLR = -119,
	CRYPTO_HASH_ERROR_FAIL = -118,
	CRYPTO_HASH_SUCCESS = 0
}crypto_Hash_Status_E;

//MD5 Algorithm
#ifdef CRYPTO_HASH_MD5_EN
typedef struct{
	uint32_t md5SessionId;
	crypto_HandlerType_E md5Handler_en;
	uint8_t arr_md5DataCtx[110];// __attribute__((alinged (8)));
}st_Crypto_Hash_Md5_Ctx;
#endif /* CRYPTO_HASH_MD5_EN */

//RIPEMD-160 Algorithm
#ifdef CRYPTO_HASH_RIPEMD160_EN
typedef struct{
	uint32_t ripemd160SessionId;
	crypto_HandlerType_E ripedmd160Handler_en;
	uint8_t arr_ripemd160DataCtx[110];// __attribute__((alinged (8)));
}st_Crypto_Hash_Ripemd160_Ctx;
#endif /* CRYPTO_HASH_RIPEMD160_EN */

//SHA-1, SHA-2, SHA-3(Except SHAKE)
#if (defined(CRYPTO_HASH_SHA1_EN) || defined(CRYPTO_HASH_SHA2_EN) || defined(CRYPTO_HASH_SHA3_EN))
typedef struct{
	uint32_t shaSessionId;
	crypto_Hash_Algo_E shaAlgo_en;
	crypto_HandlerType_E shaHandler_en;
	uint8_t arr_shaDataCtx[110];// __attribute__((alinged (8)));
}st_Crypto_Hash_Sha_Ctx;
#endif /* CRYPTO_HASH_SHA1_EN || CRYPTO_HASH_SHA2_EN || CRYPTO_HASH_SHA3_EN */

//SHA-3 only SHAKE
#ifdef CRYPTO_HASH_SHA3_SHAKE_EN
typedef struct{
	uint32_t shakeSessionId;
	uint32_t digestLen;
	crypto_Hash_Algo_E shakeAlgo_en;
	crypto_HandlerType_E shakeHandler_en;
	uint8_t arr_shakeDataCtx[110];// __attribute__((alinged (8)));
}st_Crypto_Hash_Shake_Ctx;
#endif /* CRYPTO_HASH_SHA3_SHAKE_EN */

//BLAKE Algorithm
#ifdef CRYPTO_HASH_BLAKE2_EN
typedef struct{
	uint32_t blakeSessionId;
	uint32_t digestLen;
	crypto_Hash_Algo_E blakeAlgo_en;
	crypto_HandlerType_E blakeHandler_en;
	uint8_t *ptr_key;
	uint32_t keyLen;
	uint8_t arr_blakeDataCtx[400];// __attribute__((alinged (8)));
}st_Crypto_Hash_Blake_Ctx;
#endif /* CRYPTO_HASH_BLAKE2_EN */
// *****************************************************************************

//MD5 Algorithm
#ifdef CRYPTO_HASH_MD5_EN
crypto_Hash_Status_E Crypto_Hash_Md5_Digest(crypto_HandlerType_E md5Handler_en, uint8_t *ptr_data, uint32_t dataLen, uint8_t *ptr_digest, uint32_t md5SessionId);
crypto_Hash_Status_E Crypto_Hash_Md5_Init(st_Crypto_Hash_Md5_Ctx *ptr_md5Ctx_st, crypto_HandlerType_E md5HandlerType_en, uint32_t md5SessionId);
crypto_Hash_Status_E Crypto_Hash_Md5_Update(st_Crypto_Hash_Md5_Ctx * ptr_md5Ctx_st, uint8_t *ptr_data, uint32_t dataLen);
crypto_Hash_Status_E Crypto_Hash_Md5_Final(st_Crypto_Hash_Md5_Ctx * ptr_md5Ctx_st, uint8_t *ptr_digest);
#endif /* CRYPTO_HASH_MD5_EN */

//RIPEMD-160 Algorithm
#ifdef CRYPTO_HASH_RIPEMD160_EN
crypto_Hash_Status_E Crypto_Hash_Ripemd160_Digest(crypto_HandlerType_E ripedmd160Handler_en, uint8_t *ptr_data, uint32_t dataLen, uint8_t *ptr_digest, uint32_t ripemdSessionId);
crypto_Hash_Status_E Crypto_Hash_Ripemd160_Init(st_Crypto_Hash_Ripemd160_Ctx *ptr_ripemdCtx_st, crypto_HandlerType_E ripedmd160Handler_en, uint32_t ripemdSessionId);
crypto_Hash_Status_E Crypto_Hash_Ripemd160_Update(st_Crypto_Hash_Ripemd160_Ctx *ptr_ripemdCtx_st, uint8_t *ptr_data, uint32_t dataLen);
crypto_Hash_Status_E Crypto_Hash_Ripemd160_Final(st_Crypto_Hash_Ripemd160_Ctx *ptr_ripemdCtx_st, uint8_t *ptr_digest);
#endif /* CRYPTO_HASH_RIPEMD160_EN */

//SHA-1, SHA-2, SHA-3(Except SHAKE)
#if (defined(CRYPTO_HASH_SHA1_EN) || defined(CRYPTO_HASH_SHA2_EN) || defined(CRYPTO_HASH_SHA3_EN))
crypto_Hash_Status_E Crypto_Hash_Sha_Digest(crypto_HandlerType_E shaHandler_en, uint8_t *ptr_data, uint32_t dataLen, uint8_t *ptr_digest, crypto_Hash_Algo_E shaAlgorithm_en, uint32_t shaSessionId);
crypto_Hash_Status_E Crypto_Hash_Sha_Init(st_Crypto_Hash_Sha_Ctx *ptr_shaCtx_st, crypto_Hash_Algo_E shaAlgorithm_en, crypto_HandlerType_E shaHandler_en, uint32_t shaSessionId);
crypto_Hash_Status_E Crypto_Hash_Sha_Update(st_Crypto_Hash_Sha_Ctx *ptr_shaCtx_st, uint8_t *ptr_data, uint32_t dataLen);
crypto_Hash_Status_E Crypto_Hash_Sha_Final(st_Crypto_Hash_Sha_Ctx *ptr_shaCtx_st, uint8_t *ptr_digest);
#endif /* CRYPTO_HASH_SHA1_EN || CRYPTO_HASH_SHA2_EN || CRYPTO_HASH_SHA3_EN */ 

//SHA-3 only SHAKE
#ifdef CRYPTO_HASH_SHA3_SHAKE_EN
crypto_Hash_Status_E Crypto_Hash_Shake_Digest(crypto_HandlerType_E shakeHandlerType_en, crypto_Hash_Algo_E shakeAlgorithm_en, uint8_t *ptr_data, uint32_t dataLen, uint8_t *ptr_digest, uint32_t digestLen, uint32_t shakeSessionId);
crypto_Hash_Status_E Crypto_Hash_Shake_Init(st_Crypto_Hash_Shake_Ctx* ptr_shakeCtx_st, crypto_Hash_Algo_E shakeAlgorithm_en, crypto_HandlerType_E shakeHandlerType_en, uint32_t digestLen, uint32_t shakeSessionId);
crypto_Hash_Status_E Crypto_Hash_Shake_Update(st_Crypto_Hash_Shake_Ctx* ptr_shakeCtx_st, uint8_t *ptr_data, uint32_t dataLen);
crypto_Hash_Status_E Crypto_Hash_Shake_Final(st_Crypto_Hash_Shake_Ctx* ptr_shakeCtx_st, uint8_t *ptr_digest);
#endif /* CRYPTO_HASH_SHA3_SHAKE_EN */

//BLAKE Algorithm
#ifdef CRYPTO_HASH_BLAKE2_EN
crypto_Hash_Status_E Crypto_Hash_Blake_Digest(crypto_HandlerType_E blakeHandlerType_en, crypto_Hash_Algo_E blakeAlgorithm_en, uint8_t *ptr_data, uint32_t dataLen, uint8_t *ptr_blakeKey, uint32_t keySize, uint8_t *ptr_digest, uint32_t digestLen, uint32_t blakeSessionId);
crypto_Hash_Status_E Crypto_Hash_Blake_Init(st_Crypto_Hash_Blake_Ctx *ptr_blakeCtx_st,crypto_Hash_Algo_E blakeAlgorithm_en, uint8_t *ptr_blakeKey, uint32_t keySize, uint32_t digestSize, crypto_HandlerType_E blakeHandlerType_en, uint32_t blakeSessionId);
crypto_Hash_Status_E Crypto_Hash_Blake_Update(st_Crypto_Hash_Blake_Ctx * ptr_blakeCtx_st, uint8_t *ptr_data, uint32_t dataLen);
crypto_Hash_Status_E Crypto_Hash_Blake_Final(st_Crypto_Hash_Blake_Ctx * ptr_blakeCtx_st, uint8_t *ptr_digest);
#endif /* CRYPTO_HASH_BLAKE2_EN */

#endif //MCHP_CRYPTO_HASH_H