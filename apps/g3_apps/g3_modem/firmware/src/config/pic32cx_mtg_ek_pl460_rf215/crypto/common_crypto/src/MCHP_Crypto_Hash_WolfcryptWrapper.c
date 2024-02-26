/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    MCHP_Crypto_Hash.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
*******************************************************************************/

 
// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "crypto/common_crypto/MCHP_Crypto_Common.h"
#include "crypto/common_crypto/MCHP_Crypto_Hash.h"
#include "crypto/common_crypto/MCHP_Crypto_Hash_WolfcryptWrapper.h"

#include "wolfssl/wolfcrypt/error-crypt.h"

#ifdef CRYPTO_HASH_WC_MD5_EN
#include "wolfssl/wolfcrypt/md5.h"
#endif /* CRYPTO_HASH_WC_MD5_EN */

#ifdef CRYPTO_HASH_WC_RIPEMD160_EN
#include "wolfssl/wolfcrypt/ripemd.h"
#endif /* CRYPTO_HASH_WC_RIPEMD160_EN */

#ifdef CRYPTO_HASH_WC_SHA1_EN
#include "wolfssl/wolfcrypt/sha.h"
#endif /* CRYPTO_HASH_WC_SHA1_EN */

#if (defined(CRYPTO_HASH_WC_SHA2_224_EN) || defined(CRYPTO_HASH_WC_SHA2_256_EN))
#include "wolfssl/wolfcrypt/sha256.h"
#endif /* CRYPTO_HASH_WC_SHA2_224_EN || CRYPTO_HASH_WC_SHA2_256_EN  */

#if (defined(CRYPTO_HASH_WC_SHA2_384_EN) || defined(CRYPTO_HASH_WC_SHA2_512_EN) || defined(CRYPTO_HASH_WC_SHA2_512_224_EN) || defined(CRYPTO_HASH_WC_SHA2_512_256_EN))
#include "wolfssl/wolfcrypt/sha512.h"
#endif /* CRYPTO_HASH_WC_SHA2_384_EN || CRYPTO_HASH_WC_SHA2_512_EN || CRYPTO_HASH_WC_SHA2_512_224_EN || CRYPTO_HASH_WC_SHA2_512_256_EN */

#if (defined(CRYPTO_HASH_WC_SHA3_EN) || defined(CRYPTO_HASH_WC_SHA3_SHAKE_EN))
#include "wolfssl/wolfcrypt/sha3.h"
#endif /* CRYPTO_HASH_WC_SHA3_EN || CRYPTO_HASH_WC_SHA3_SHAKE_EN  */

#ifdef CRYPTO_HASH_WC_BLAKE2_EN
#include "wolfssl/wolfcrypt/blake2.h"
#endif /* CRYPTO_HASH_WC_BLAKE2_EN */
// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
#ifdef CRYPTO_HASH_WC_MD5_EN
crypto_Hash_Status_E Crypto_Hash_Wc_Md5Digest(uint8_t *ptr_data, uint32_t dataLen, uint8_t *ptr_digest)
{
	crypto_Hash_Status_E ret_md5Stat_en = CRYPTO_HASH_ERROR_NOTSUPPTED; 
#ifndef NO_MD5
    if( (ptr_data != NULL) && (ptr_digest != NULL) && (dataLen > 0u) )
    {
        wc_Md5 ptr_md5Ctx_st[1];
        ret_md5Stat_en = Crypto_Hash_Wc_Md5Init(ptr_md5Ctx_st);
        if(ret_md5Stat_en == CRYPTO_HASH_SUCCESS)
        {
            ret_md5Stat_en = Crypto_Hash_Wc_Md5Update(ptr_md5Ctx_st, ptr_data, dataLen);
            if(ret_md5Stat_en == CRYPTO_HASH_SUCCESS)
            {
                ret_md5Stat_en = Crypto_Hash_Wc_Md5Final(ptr_md5Ctx_st, ptr_digest);
            }
        }
    }
    else
    {
        ret_md5Stat_en = CRYPTO_HASH_ERROR_ARG;
    }
#endif /* !NO_MD5 */
    return ret_md5Stat_en;
}

crypto_Hash_Status_E Crypto_Hash_Wc_Md5Init(void *ptr_md5Ctx_st)
{  	
	crypto_Hash_Status_E ret_md5Stat_en = CRYPTO_HASH_ERROR_NOTSUPPTED; 
#ifndef NO_MD5
    int wcMd5Status = BAD_FUNC_ARG;
    if(ptr_md5Ctx_st != NULL)
    {
        wcMd5Status = wc_InitMd5((wc_Md5*)ptr_md5Ctx_st);

        if(wcMd5Status == 0)
        {
            ret_md5Stat_en = CRYPTO_HASH_SUCCESS;
        }
        else if (wcMd5Status == BAD_FUNC_ARG)
        {
            ret_md5Stat_en = CRYPTO_HASH_ERROR_ARG;
        }
        else
        {
            ret_md5Stat_en = CRYPTO_HASH_ERROR_FAIL;
        }
    }
    else
    {
        ret_md5Stat_en = CRYPTO_HASH_ERROR_CTX;
    }
#endif /* !NO_MD5 */
    return ret_md5Stat_en;
}

crypto_Hash_Status_E Crypto_Hash_Wc_Md5Update(void *ptr_md5Ctx_st, uint8_t *ptr_data, uint32_t dataLen)
{
    crypto_Hash_Status_E ret_md5Stat_en = CRYPTO_HASH_ERROR_NOTSUPPTED;
#ifndef NO_MD5
    int wcMd5Status = BAD_FUNC_ARG;
    if(ptr_md5Ctx_st != NULL)
    {
        wcMd5Status = wc_Md5Update((wc_Md5*)ptr_md5Ctx_st, (const byte*)ptr_data, (word32)dataLen);
        
        if(wcMd5Status == 0)
        {
            ret_md5Stat_en = CRYPTO_HASH_SUCCESS;
        }
        else if (wcMd5Status == BAD_FUNC_ARG)
        {
            ret_md5Stat_en = CRYPTO_HASH_ERROR_ARG;
        }
        else
        {
            ret_md5Stat_en = CRYPTO_HASH_ERROR_FAIL;
        }
    }
    else
    {
       ret_md5Stat_en = CRYPTO_HASH_ERROR_CTX;
    }
#endif /* !NO_MD5 */
    return ret_md5Stat_en;
}

crypto_Hash_Status_E Crypto_Hash_Wc_Md5Final(void *ptr_md5Ctx_st, uint8_t *ptr_digest)
{
    crypto_Hash_Status_E ret_md5Stat_en = CRYPTO_HASH_ERROR_NOTSUPPTED;
#ifndef NO_MD5
    int wcMd5Status = BAD_FUNC_ARG;
    if(ptr_md5Ctx_st != NULL)
    {
        wcMd5Status = wc_Md5Final((wc_Md5*)ptr_md5Ctx_st, (byte*)ptr_digest);
        
        if(wcMd5Status == 0)
        {
            ret_md5Stat_en = CRYPTO_HASH_SUCCESS;
        }
        else if (wcMd5Status == BAD_FUNC_ARG)
        {
            ret_md5Stat_en = CRYPTO_HASH_ERROR_ARG;
        }
        else
        {
            ret_md5Stat_en = CRYPTO_HASH_ERROR_FAIL;
        }
    }
    else
    {
       ret_md5Stat_en = CRYPTO_HASH_ERROR_CTX;
    }
#endif /* !NO_MD5 */
    return ret_md5Stat_en; 
}
#endif /* CRYPTO_HASH_WC_MD5_EN */

#ifdef CRYPTO_HASH_WC_RIPEMD160_EN
crypto_Hash_Status_E Crypto_Hash_Wc_Ripemd160Digest(uint8_t *ptr_data, uint32_t dataLen, uint8_t *ptr_digest)
{
    crypto_Hash_Status_E ret_ripemdStat_en = CRYPTO_HASH_ERROR_NOTSUPPTED;  
#ifdef WOLFSSL_RIPEMD
    RipeMd ptr_ripemdCtx_st[1];
    if( (ptr_data != NULL) && (ptr_digest != NULL) && (dataLen != 0u) )
    {
        //Initialize the Ripemd160 context
        ret_ripemdStat_en = Crypto_Hash_Wc_Ripemd160Init(ptr_ripemdCtx_st);
        
        if(ret_ripemdStat_en == CRYPTO_HASH_SUCCESS)
        {
            ret_ripemdStat_en = Crypto_Hash_Wc_Ripemd160Update(ptr_ripemdCtx_st, ptr_data, dataLen);
        }
        if(ret_ripemdStat_en == CRYPTO_HASH_SUCCESS)
        {
            ret_ripemdStat_en = Crypto_Hash_Wc_Ripemd160Final(ptr_ripemdCtx_st, ptr_digest);
        }
        else
        {
            ret_ripemdStat_en = CRYPTO_HASH_ERROR_FAIL;
        }
    }
    else
    {
       ret_ripemdStat_en = CRYPTO_HASH_ERROR_ARG;
    }
#endif /* WOLFSSL_RIPEMD */
    return ret_ripemdStat_en;
}

crypto_Hash_Status_E Crypto_Hash_Wc_Ripemd160Init(void *ptr_ripemdCtx_st)
{
	crypto_Hash_Status_E ret_ripemdStat_en = CRYPTO_HASH_ERROR_NOTSUPPTED; 
#ifdef WOLFSSL_RIPEMD
    int wcRipemdStatus = BAD_FUNC_ARG;
    if(ptr_ripemdCtx_st != NULL)
    {
        wcRipemdStatus = wc_InitRipeMd((RipeMd*)ptr_ripemdCtx_st);

        if(wcRipemdStatus == 0)
        {
            ret_ripemdStat_en = CRYPTO_HASH_SUCCESS;
        }
        else if (wcRipemdStatus == BAD_FUNC_ARG)
        {
            ret_ripemdStat_en = CRYPTO_HASH_ERROR_ARG;
        }
        else
        {
            ret_ripemdStat_en = CRYPTO_HASH_ERROR_FAIL;
        }
    }
    else
    {
        ret_ripemdStat_en = CRYPTO_HASH_ERROR_CTX;
    }
#endif /* WOLFSSL_RIPEMD */    
    return ret_ripemdStat_en;
}

crypto_Hash_Status_E Crypto_Hash_Wc_Ripemd160Update(void *ptr_ripemdCtx_st, uint8_t *ptr_data, uint32_t dataLen)
{
    crypto_Hash_Status_E ret_ripemdStat_en = CRYPTO_HASH_ERROR_NOTSUPPTED;
#ifdef WOLFSSL_RIPEMD
    int wcRipemdStatus = BAD_FUNC_ARG;
    if(ptr_ripemdCtx_st != NULL)
    {
        wcRipemdStatus = wc_RipeMdUpdate((RipeMd*)ptr_ripemdCtx_st, (const byte*)ptr_data, (word32)dataLen);
        
        if(wcRipemdStatus == 0)
        {
            ret_ripemdStat_en = CRYPTO_HASH_SUCCESS;
        }
        else if (wcRipemdStatus == BAD_FUNC_ARG)
        {
            ret_ripemdStat_en = CRYPTO_HASH_ERROR_ARG;
        }
        else
        {
            ret_ripemdStat_en = CRYPTO_HASH_ERROR_FAIL;
        }
    }
    else
    {
       ret_ripemdStat_en = CRYPTO_HASH_ERROR_CTX;
    }
#endif /* WOLFSSL_RIPEMD */  
    return ret_ripemdStat_en;
}

crypto_Hash_Status_E Crypto_Hash_Wc_Ripemd160Final(void *ptr_ripemdCtx_st, uint8_t *ptr_digest)
{   
    crypto_Hash_Status_E ret_ripemdStat_en = CRYPTO_HASH_ERROR_NOTSUPPTED;
#ifdef WOLFSSL_RIPEMD 
    int wcRipemdStatus = BAD_FUNC_ARG;
    if( (ptr_ripemdCtx_st != NULL) && (ptr_digest != NULL) )
    {
        wcRipemdStatus = wc_RipeMdFinal((RipeMd*)ptr_ripemdCtx_st, (byte*)ptr_digest);
        
        if(wcRipemdStatus == 0)
        {
            ret_ripemdStat_en = CRYPTO_HASH_SUCCESS;
        }
        else if (wcRipemdStatus == BAD_FUNC_ARG)
        {
            ret_ripemdStat_en = CRYPTO_HASH_ERROR_ARG;
        }
        else
        {
            ret_ripemdStat_en = CRYPTO_HASH_ERROR_FAIL;
        }
    }
    else
    {
       ret_ripemdStat_en = CRYPTO_HASH_ERROR_ARG;
    }
#endif /* WOLFSSL_RIPEMD */  
    return ret_ripemdStat_en;
}
#endif /* CRYPTO_HASH_WC_RIPEMD160_EN */

#if (defined(CRYPTO_HASH_WC_SHA1_EN) || defined(CRYPTO_HASH_WC_SHA2_EN) || defined(CRYPTO_HASH_WC_SHA3_EN))
crypto_Hash_Status_E Crypto_Hash_Wc_ShaDigest(uint8_t *ptr_data, uint32_t dataLen, uint8_t *ptr_digest, crypto_Hash_Algo_E hashAlgo_en)
{
	crypto_Hash_Status_E ret_shaStat_en = CRYPTO_HASH_ERROR_NOTSUPPTED;
    uint8_t arr_shaDataCtx[CRYPTO_HASH_WC_SHACTX_SIZE];
    if( (ptr_data != NULL) && (dataLen > 0u) && (ptr_digest != NULL) )
    {
        ret_shaStat_en = Crypto_Hash_Wc_ShaInit(arr_shaDataCtx, hashAlgo_en);
        if(ret_shaStat_en == CRYPTO_HASH_SUCCESS)
        {
            ret_shaStat_en = Crypto_Hash_Wc_ShaUpdate(arr_shaDataCtx, ptr_data, dataLen, hashAlgo_en);
            if(ret_shaStat_en == CRYPTO_HASH_SUCCESS)
            {
                ret_shaStat_en = Crypto_Hash_Wc_ShaFinal(arr_shaDataCtx, ptr_digest, hashAlgo_en);
            }
        }
    }
    else
    {
        ret_shaStat_en = CRYPTO_HASH_ERROR_ARG;
    }
    return ret_shaStat_en;
}

crypto_Hash_Status_E Crypto_Hash_Wc_ShaInit(void *ptr_shaCtx_st, crypto_Hash_Algo_E hashAlgo_en)
{
    crypto_Hash_Status_E ret_shaStat_en = CRYPTO_HASH_ERROR_NOTSUPPTED;
	int wcShaStatus = BAD_FUNC_ARG;
	
    if(ptr_shaCtx_st != NULL)
    {
        switch(hashAlgo_en)
        {    
#if (!defined(NO_SHA) && defined(CRYPTO_HASH_WC_SHA1_EN))
            case CRYPTO_HASH_SHA1:
                wcShaStatus = wc_InitSha((wc_Sha*)ptr_shaCtx_st);
                break;
#endif /* !NO_SHA && CRYPTO_HASH_WC_SHA1_EN*/	

#ifndef NO_SHA256
#if (defined(WOLFSSL_SHA224) && defined(CRYPTO_HASH_WC_SHA2_224_EN))
            case CRYPTO_HASH_SHA2_224:
                wcShaStatus = wc_InitSha224((wc_Sha224*)ptr_shaCtx_st);
                break;
#endif /* WOLFSSL_SHA224 && CRYPTO_HASH_WC_SHA2_224_EN */	

#ifdef CRYPTO_HASH_WC_SHA2_256_EN 
            case CRYPTO_HASH_SHA2_256:
                wcShaStatus = wc_InitSha256((wc_Sha256*)ptr_shaCtx_st);
                break;
#endif /* CRYPTO_HASH_WC_SHA2_256_EN */                
#endif /* !NO_SHA256 */

#if (defined(WOLFSSL_SHA384) && defined(CRYPTO_HASH_WC_SHA2_384_EN))
            case CRYPTO_HASH_SHA2_384:
                wcShaStatus = wc_InitSha384((wc_Sha384*)ptr_shaCtx_st);
                break;
#endif /* WOLFSSL_SHA384 && CRYPTO_HASH_WC_SHA2_384_EN */

#if (defined(WOLFSSL_SHA512) && defined(CRYPTO_HASH_WC_SHA2_512_EN))
            case CRYPTO_HASH_SHA2_512:
                wcShaStatus = wc_InitSha512((wc_Sha512*)ptr_shaCtx_st);
                break;
#endif /* WOLFSSL_SHA512  && CRYPTO_HASH_WC_SHA2_512_EN */

#if (!defined(WOLFSSL_NOSHA512_224) && defined(CRYPTO_HASH_WC_SHA2_512_224_EN))            
            case CRYPTO_HASH_SHA2_512_224:
                wcShaStatus = wc_InitSha512_224((wc_Sha512*)ptr_shaCtx_st);
                break;
#endif /* !WOLFSSL_NOSHA512_224 && CRYPTO_HASH_WC_SHA2_512_224_EN */
            
#if (!defined(WOLFSSL_NOSHA512_256) && defined(CRYPTO_HASH_WC_SHA2_512_256_EN))                                  
            case CRYPTO_HASH_SHA2_512_256:
                wcShaStatus = wc_InitSha512_256((wc_Sha512*)ptr_shaCtx_st);
                break;
#endif /* !WOLFSSL_NOSHA512_256 && CRYPTO_HASH_WC_SHA2_512_256_EN */

#ifdef WOLFSSL_SHA3	
#ifdef CRYPTO_HASH_WC_SHA3_224_EN                
            case CRYPTO_HASH_SHA3_224:
                wcShaStatus = wc_InitSha3_224((wc_Sha3*)ptr_shaCtx_st, NULL, INVALID_DEVID);
            break;
#endif /* CRYPTO_HASH_WC_SHA3_224_EN */
            
#ifdef CRYPTO_HASH_WC_SHA3_256_EN            
            case CRYPTO_HASH_SHA3_256:
                wcShaStatus = wc_InitSha3_256((wc_Sha3*)ptr_shaCtx_st, NULL, INVALID_DEVID);
            break;
#endif /* CRYPTO_HASH_WC_SHA3_256_EN */

#ifdef CRYPTO_HASH_WC_SHA3_384_EN            
            case CRYPTO_HASH_SHA3_384:
                wcShaStatus = wc_InitSha3_384((wc_Sha3*)ptr_shaCtx_st, NULL, INVALID_DEVID);
            break;
#endif /* CRYPTO_HASH_WC_SHA3_384_EN */

#ifdef CRYPTO_HASH_WC_SHA3_512_EN            
            case CRYPTO_HASH_SHA3_512:
                wcShaStatus = wc_InitSha3_512((wc_Sha3*)ptr_shaCtx_st, NULL, INVALID_DEVID);
            break;
#endif /* CRYPTO_HASH_WC_SHA3_512_EN */            
#endif /* WOLFSSL_SHA3 */

            default:
                ret_shaStat_en = CRYPTO_HASH_ERROR_ALGO;
                break;
        }

        if(ret_shaStat_en == CRYPTO_HASH_ERROR_ALGO)
        {
            //do nothing
        }
        else if(wcShaStatus == 0)
        {
            ret_shaStat_en = CRYPTO_HASH_SUCCESS;
        }
        else if (wcShaStatus == BAD_FUNC_ARG)
        {
            ret_shaStat_en = CRYPTO_HASH_ERROR_ARG;
        }
        else
        {
            ret_shaStat_en = CRYPTO_HASH_ERROR_FAIL;
        }
    }
    else
    {
        ret_shaStat_en = CRYPTO_HASH_ERROR_CTX;
    }
    return ret_shaStat_en;
}

crypto_Hash_Status_E Crypto_Hash_Wc_ShaUpdate(void *ptr_shaCtx_st, uint8_t *ptr_data, uint32_t dataLen, crypto_Hash_Algo_E hashAlgo_en)
{
    crypto_Hash_Status_E ret_shaStat_en = CRYPTO_HASH_ERROR_NOTSUPPTED;
    int wcShaStatus = BAD_FUNC_ARG;
	
	switch(hashAlgo_en)
	{
#if (!defined(NO_SHA) && defined(CRYPTO_HASH_WC_SHA1_EN))
		case CRYPTO_HASH_SHA1:
			wcShaStatus = wc_ShaUpdate((wc_Sha*)ptr_shaCtx_st, (const byte*)ptr_data, (word32)dataLen);
            break;
#endif /* !NO_SHA && CRYPTO_HASH_WC_SHA1_EN */	 

#ifndef NO_SHA256            
#if (defined(WOLFSSL_SHA224) && defined(CRYPTO_HASH_WC_SHA2_224_EN))
		case CRYPTO_HASH_SHA2_224:
			wcShaStatus = wc_Sha224Update((wc_Sha224*)ptr_shaCtx_st, (const byte*)ptr_data, (word32)dataLen);
            break;
#endif /* WOLFSSL_SHA224 && CRYPTO_HASH_WC_SHA2_224_EN*/	            

#ifdef CRYPTO_HASH_WC_SHA2_256_EN            
		case CRYPTO_HASH_SHA2_256:
			wcShaStatus = wc_Sha256Update((wc_Sha256*)ptr_shaCtx_st, (const byte*)ptr_data, (word32)dataLen);
            break;
#endif /* CRYPTO_HASH_WC_SHA2_256_EN */            
#endif /* NO_SHA256 */

#if (defined(WOLFSSL_SHA384) && defined(CRYPTO_HASH_WC_SHA2_384_EN))
		case CRYPTO_HASH_SHA2_384:
			wcShaStatus = wc_Sha384Update((wc_Sha384*)ptr_shaCtx_st, (const byte*)ptr_data, (word32)dataLen);
            break;
#endif /* WOLFSSL_SHA384 && CRYPTO_HASH_WC_SHA2_384_EN*/

#if (defined(WOLFSSL_SHA512) && defined(CRYPTO_HASH_WC_SHA2_512_EN))
		case CRYPTO_HASH_SHA2_512:
			wcShaStatus = wc_Sha512Update((wc_Sha512*)ptr_shaCtx_st, (const byte*)ptr_data, (word32)dataLen);
            break;	
#endif /* WOLFSSL_SHA512 && CRYPTO_HASH_WC_SHA2_512_EN */

#if (!defined(WOLFSSL_NOSHA512_224) && defined(CRYPTO_HASH_WC_SHA2_512_224_EN))            
        case CRYPTO_HASH_SHA2_512_224:
            wcShaStatus = wc_Sha512_224Update((wc_Sha512*)ptr_shaCtx_st, (const byte*)ptr_data, (word32)dataLen);
            break;
#endif /* !WOLFSSL_NOSHA512_224 && CRYPTO_HASH_WC_SHA2_512_224_EN */
            
#if (!defined(WOLFSSL_NOSHA512_256) && defined(CRYPTO_HASH_WC_SHA2_512_256_EN))            
        case CRYPTO_HASH_SHA2_512_256:
            wcShaStatus = wc_Sha512_256Update((wc_Sha512*)ptr_shaCtx_st, (const byte*)ptr_data, (word32)dataLen);
            break;            
#endif /* !WOLFSSL_NOSHA512_256 && CRYPTO_HASH_WC_SHA2_512_256_EN */
            
#ifdef WOLFSSL_SHA3
#ifdef CRYPTO_HASH_WC_SHA3_224_EN             
		case CRYPTO_HASH_SHA3_224:
			wcShaStatus = wc_Sha3_224_Update((wc_Sha3*)ptr_shaCtx_st, (const byte*)ptr_data, (word32)dataLen);
            break;
#endif /* CRYPTO_HASH_WC_SHA3_224_EN */
            
#ifdef CRYPTO_HASH_WC_SHA3_256_EN               
		case CRYPTO_HASH_SHA3_256:
			wcShaStatus = wc_Sha3_256_Update((wc_Sha3*)ptr_shaCtx_st, (const byte*)ptr_data, (word32)dataLen);
            break;
#endif /* CRYPTO_HASH_WC_SHA3_256_EN */

#ifdef CRYPTO_HASH_WC_SHA3_384_EN              
		case CRYPTO_HASH_SHA3_384:
			wcShaStatus = wc_Sha3_384_Update((wc_Sha3*)ptr_shaCtx_st, (const byte*)ptr_data, (word32)dataLen);
            break;
#endif /* CRYPTO_HASH_WC_SHA3_384_EN */

#ifdef CRYPTO_HASH_WC_SHA3_512_EN               
		case CRYPTO_HASH_SHA3_512:
			wcShaStatus = wc_Sha3_512_Update((wc_Sha3*)ptr_shaCtx_st, (const byte*)ptr_data, (word32)dataLen);
            break;
#endif /* CRYPTO_HASH_WC_SHA3_512_EN */              
#endif /* WOLFSSL_SHA3 */ 
         
        default:
            ret_shaStat_en = CRYPTO_HASH_ERROR_ALGO;
            break;
	}

	if(wcShaStatus == 0)
	{
		ret_shaStat_en = CRYPTO_HASH_SUCCESS;
	}
    else if(ret_shaStat_en == CRYPTO_HASH_ERROR_ALGO)
    {
        //do nothing
    }
    else
    {
        ret_shaStat_en = CRYPTO_HASH_ERROR_FAIL;
    }
    
	return ret_shaStat_en;  
}

crypto_Hash_Status_E Crypto_Hash_Wc_ShaFinal(void *ptr_shaCtx_st, uint8_t *ptr_digest, crypto_Hash_Algo_E hashAlgo_en)
{
    crypto_Hash_Status_E ret_shaStat_en = CRYPTO_HASH_ERROR_NOTSUPPTED;
    int wcShaStatus = BAD_FUNC_ARG;
	
	switch(hashAlgo_en)
	{
#if (!defined(NO_SHA) && defined(CRYPTO_HASH_WC_SHA1_EN))
		case CRYPTO_HASH_SHA1:
			wcShaStatus = wc_ShaFinal((wc_Sha*)ptr_shaCtx_st, (byte*)ptr_digest);
            break;
#endif /* !NO_SHA && CRYPTO_HASH_WC_SHA1_EN */	 

#ifndef NO_SHA256            
#if (defined(WOLFSSL_SHA224) && defined(CRYPTO_HASH_WC_SHA2_224_EN))
		case CRYPTO_HASH_SHA2_224:
			wcShaStatus = wc_Sha224Final((wc_Sha224*)ptr_shaCtx_st, (byte*)ptr_digest);
            break;
#endif /* WOLFSSL_SHA224 && CRYPTO_HASH_WC_SHA2_224_EN*/	            

#ifdef CRYPTO_HASH_WC_SHA2_256_EN               
		case CRYPTO_HASH_SHA2_256:
			wcShaStatus = wc_Sha256Final((wc_Sha256*)ptr_shaCtx_st, (byte*)ptr_digest);
            break;
#endif /* #ifdef CRYPTO_HASH_WC_SHA2_256_EN */            
#endif /* !NO_SHA256 */

#if (defined(WOLFSSL_SHA384) && defined(CRYPTO_HASH_WC_SHA2_384_EN))
		case CRYPTO_HASH_SHA2_384:
			wcShaStatus = wc_Sha384Final((wc_Sha384*)ptr_shaCtx_st, (byte*)ptr_digest);
            break;
#endif /* WOLFSSL_SHA384 && CRYPTO_HASH_WC_SHA2_384_EN */

#if (defined(WOLFSSL_SHA512) && defined(CRYPTO_HASH_WC_SHA2_512_EN))
		case CRYPTO_HASH_SHA2_512:
			wcShaStatus = wc_Sha512Final((wc_Sha512*)ptr_shaCtx_st, (byte*)ptr_digest);
            break;	
#endif /* WOLFSSL_SHA512 && CRYPTO_HASH_WC_SHA2_512_EN */

#if (!defined(WOLFSSL_NOSHA512_224) && defined(CRYPTO_HASH_WC_SHA2_512_224_EN))             
        case CRYPTO_HASH_SHA2_512_224:
            wcShaStatus = wc_Sha512_224Final((wc_Sha512*)ptr_shaCtx_st, (byte*)ptr_digest);
            break;
#endif /* !WOLFSSL_NOSHA512_224 && CRYPTO_HASH_WC_SHA2_512_224_EN */
            
#if (!defined(WOLFSSL_NOSHA512_256) && defined(CRYPTO_HASH_WC_SHA2_512_256_EN))             
        case CRYPTO_HASH_SHA2_512_256:
            wcShaStatus = wc_Sha512_256Final((wc_Sha512*)ptr_shaCtx_st, (byte*)ptr_digest);
            break;
#endif /* !WOLFSSL_NOSHA512_256 && CRYPTO_HASH_WC_SHA2_512_256_EN */
            
#ifdef WOLFSSL_SHA3
#ifdef CRYPTO_HASH_WC_SHA3_224_EN             
		case CRYPTO_HASH_SHA3_224:
			wcShaStatus = wc_Sha3_224_Final((wc_Sha3*)ptr_shaCtx_st, (byte*)ptr_digest);
            break;
#endif /* CRYPTO_HASH_WC_SHA3_224_EN */
            
#ifdef CRYPTO_HASH_WC_SHA3_256_EN               
		case CRYPTO_HASH_SHA3_256:
			wcShaStatus = wc_Sha3_256_Final((wc_Sha3*)ptr_shaCtx_st, (byte*)ptr_digest);
            break;
#endif /* CRYPTO_HASH_WC_SHA3_256_EN */

#ifdef CRYPTO_HASH_WC_SHA3_384_EN              
		case CRYPTO_HASH_SHA3_384:
			wcShaStatus = wc_Sha3_384_Final((wc_Sha3*)ptr_shaCtx_st, (byte*)ptr_digest);
            break;
#endif /* CRYPTO_HASH_WC_SHA3_384_EN */

#ifdef CRYPTO_HASH_WC_SHA3_512_EN               
		case CRYPTO_HASH_SHA3_512:
			wcShaStatus = wc_Sha3_512_Final((wc_Sha3*)ptr_shaCtx_st, (byte*)ptr_digest);
            break;
#endif /* CRYPTO_HASH_WC_SHA3_512_EN */            
#endif /* WOLFSSL_SHA3 */ 
         
        default:
            ret_shaStat_en = CRYPTO_HASH_ERROR_ALGO;
            break;
	}

	if(wcShaStatus == 0)
	{
		ret_shaStat_en = CRYPTO_HASH_SUCCESS;
	}
    else if(ret_shaStat_en == CRYPTO_HASH_ERROR_ALGO)
    {
        //do nothing
    }
    else
    {
        ret_shaStat_en = CRYPTO_HASH_ERROR_FAIL;
    }
    
	return ret_shaStat_en;  
}
#endif /* CRYPTO_HASH_WC_SHA1_EN || CRYPTO_HASH_WC_SHA2_EN || CRYPTO_HASH_WC_SHA3_EN */

#ifdef CRYPTO_HASH_WC_SHA3_SHAKE_EN
crypto_Hash_Status_E Crypto_Hash_Wc_ShakeDigest(uint8_t *ptr_data, uint32_t dataLen, uint8_t *ptr_digest, uint32_t digestLen, crypto_Hash_Algo_E hashAlgo_en)
{
    crypto_Hash_Status_E ret_shakeStat_en = CRYPTO_HASH_ERROR_NOTSUPPTED;

#ifdef WOLFSSL_SHA3	
    if( (ptr_data != NULL) && (dataLen > 0u) && (ptr_digest != NULL) && (digestLen > 0u) )
    {
        wc_Shake ptr_shakeCtx_st[1];
        switch(hashAlgo_en)
        {
#if (defined(WOLFSSL_SHAKE128) && defined(CRYPTO_HASH_WC_SHA3_SHAKE128_EN))        
            case CRYPTO_HASH_SHA3_SHAKE128:
                ret_shakeStat_en = Crypto_Hash_Wc_ShakeInit(ptr_shakeCtx_st, CRYPTO_HASH_SHA3_SHAKE128);
                if(ret_shakeStat_en == 0)
                {
                    ret_shakeStat_en = Crypto_Hash_Wc_ShakeUpdate(ptr_shakeCtx_st, ptr_data, dataLen, CRYPTO_HASH_SHA3_SHAKE128);
                    if(ret_shakeStat_en == 0)
                    {
                        ret_shakeStat_en = Crypto_Hash_Wc_ShakeFinal(ptr_shakeCtx_st, ptr_digest, digestLen, CRYPTO_HASH_SHA3_SHAKE128);
                    }
                }
                break;
#endif /* WOLFSSL_SHAKE128 && CRYPTO_HASH_WC_SHA3_SHAKE128_EN */

#if (defined(WOLFSSL_SHAKE256) && defined(CRYPTO_HASH_WC_SHA3_SHAKE256_EN))        
            case CRYPTO_HASH_SHA3_SHAKE256:
                ret_shakeStat_en = Crypto_Hash_Wc_ShakeInit(ptr_shakeCtx_st, CRYPTO_HASH_SHA3_SHAKE256);
                if(ret_shakeStat_en == CRYPTO_HASH_SUCCESS)
                {
                    ret_shakeStat_en = Crypto_Hash_Wc_ShakeUpdate(ptr_shakeCtx_st, ptr_data, dataLen, CRYPTO_HASH_SHA3_SHAKE256);
                    if(ret_shakeStat_en == CRYPTO_HASH_SUCCESS)
                    {
                        ret_shakeStat_en = Crypto_Hash_Wc_ShakeFinal(ptr_shakeCtx_st, ptr_digest, digestLen, CRYPTO_HASH_SHA3_SHAKE256);
                    }
                }
                break;
#endif /* WOLFSSL_SHAKE256 && CRYPTO_HASH_WC_SHA3_SHAKE256_EN */  

            default:
                ret_shakeStat_en = CRYPTO_HASH_ERROR_NOTSUPPTED;
                break; 
        }
    }
    else
    {
        ret_shakeStat_en = CRYPTO_HASH_ERROR_ARG;
    }
#endif /* WOLFSSL_SHA3 */    
    return ret_shakeStat_en;
}

crypto_Hash_Status_E Crypto_Hash_Wc_ShakeInit(void *ptr_shakeCtx_st, crypto_Hash_Algo_E hashAlgo_en)
{
    crypto_Hash_Status_E ret_shakeStat_en = CRYPTO_HASH_ERROR_NOTSUPPTED;
#ifdef WOLFSSL_SHA3	     
     int wcShakeStatus = BAD_FUNC_ARG;
    if(ptr_shakeCtx_st != NULL)
    {
        switch(hashAlgo_en)
        {
#if (defined(WOLFSSL_SHAKE128) && defined(CRYPTO_HASH_WC_SHA3_SHAKE128_EN))       
            case CRYPTO_HASH_SHA3_SHAKE128:
                wcShakeStatus = wc_InitShake128((wc_Shake*)ptr_shakeCtx_st, NULL, INVALID_DEVID);
                break;
#endif /* WOLFSSL_SHAKE128 && CRYPTO_HASH_WC_SHA3_SHAKE128_EN */

#if (defined(WOLFSSL_SHAKE256) && defined(CRYPTO_HASH_WC_SHA3_SHAKE256_EN))           
            case CRYPTO_HASH_SHA3_SHAKE256:
                wcShakeStatus = wc_InitShake256((wc_Shake*)ptr_shakeCtx_st, NULL, INVALID_DEVID);
                break;
#endif /* WOLFSSL_SHAKE256 && CRYPTO_HASH_WC_SHA3_SHAKE256_EN */  

            default:
                ret_shakeStat_en = CRYPTO_HASH_ERROR_ALGO;
                break; 
        }
        
        if(ret_shakeStat_en != CRYPTO_HASH_ERROR_ALGO)
        {
            if(wcShakeStatus == 0)
            {
                ret_shakeStat_en = CRYPTO_HASH_SUCCESS;
            }
            else if (wcShakeStatus == BAD_FUNC_ARG)
            {
                ret_shakeStat_en = CRYPTO_HASH_ERROR_ARG;
            }
            else
            {
                ret_shakeStat_en = CRYPTO_HASH_ERROR_FAIL;
            }
        }
    }
    else
    {
        ret_shakeStat_en = CRYPTO_HASH_ERROR_ARG;
    }
#endif /* WOLFSSL_SHA3 */    
    return ret_shakeStat_en;
}          

crypto_Hash_Status_E Crypto_Hash_Wc_ShakeUpdate(void *ptr_shakeCtx_st, uint8_t *ptr_data, uint32_t dataLen, crypto_Hash_Algo_E hashAlgo_en)
{
    crypto_Hash_Status_E ret_shakeStat_en = CRYPTO_HASH_ERROR_NOTSUPPTED;
#ifdef WOLFSSL_SHA3	
    int wcShakeStatus = BAD_FUNC_ARG;
    if( (ptr_shakeCtx_st != NULL) && (ptr_data != NULL) && (dataLen > 0u) )
    {
        switch(hashAlgo_en)
        {
#if (defined(WOLFSSL_SHAKE128) && defined(CRYPTO_HASH_WC_SHA3_SHAKE128_EN))       
            case CRYPTO_HASH_SHA3_SHAKE128:
                //wcShakeStatus = wc_Shake128_Update((wc_Shake*)ptr_shakeCtx_st, (const byte*)ptr_data, (word32)dataLen);
                break;
#endif /* WOLFSSL_SHAKE128 && CRYPTO_HASH_WC_SHA3_SHAKE128_EN */

#if (defined(WOLFSSL_SHAKE256) && defined(CRYPTO_HASH_WC_SHA3_SHAKE256_EN))           
            case CRYPTO_HASH_SHA3_SHAKE256:
                wcShakeStatus = wc_Shake256_Update((wc_Shake*)ptr_shakeCtx_st, (const byte*)ptr_data, (word32)dataLen);
                break;
#endif /* WOLFSSL_SHAKE256 && CRYPTO_HASH_WC_SHA3_SHAKE256_EN */  

            default:
                //ret_shakeStat_en = CRYPTO_HASH_ERROR_NOTSUPPTED;
                break; 
        }

        if(wcShakeStatus == 0)
        {
            ret_shakeStat_en = CRYPTO_HASH_SUCCESS;
        }
        else if (wcShakeStatus == BAD_FUNC_ARG)
        {
            ret_shakeStat_en = CRYPTO_HASH_ERROR_ARG;
        }
        else
        {
            ret_shakeStat_en = CRYPTO_HASH_ERROR_FAIL;
        }
    }
    else
    {
        ret_shakeStat_en = CRYPTO_HASH_ERROR_ARG;
    }
#endif /* WOLFSSL_SHA3  */    
    return ret_shakeStat_en;
} 

crypto_Hash_Status_E Crypto_Hash_Wc_ShakeFinal(void *ptr_shakeCtx_st, uint8_t *ptr_digest, uint32_t digestLen, crypto_Hash_Algo_E hashAlgo_en)
{
   crypto_Hash_Status_E ret_shakeStat_en = CRYPTO_HASH_ERROR_NOTSUPPTED;
#ifdef WOLFSSL_SHA3
    int wcShakeStatus = BAD_FUNC_ARG;	
    if( (ptr_shakeCtx_st != NULL) && (ptr_digest != NULL) )
    {
        switch(hashAlgo_en)
        {
#if (defined(WOLFSSL_SHAKE128) && defined(CRYPTO_HASH_WC_SHA3_SHAKE128_EN))     
            case CRYPTO_HASH_SHA3_SHAKE128:
                //wcShakeStatus = wc_Shake128_Final((wc_Shake*)ptr_shakeCtx_st, ptr_digest, digestLen);
                break;
#endif /* WOLFSSL_SHAKE128 && CRYPTO_HASH_WC_SHA3_SHAKE128_EN */

#if (defined(WOLFSSL_SHAKE256) && defined(CRYPTO_HASH_WC_SHA3_SHAKE256_EN))           
            case CRYPTO_HASH_SHA3_SHAKE256:
                wcShakeStatus = wc_Shake256_Final((wc_Shake*)ptr_shakeCtx_st, ptr_digest, digestLen);
                break;
#endif /* WOLFSSL_SHAKE256 && CRYPTO_HASH_WC_SHA3_SHAKE256_EN */  

            default:
                ret_shakeStat_en = CRYPTO_HASH_ERROR_ALGO;
                break; 
        }
        
        if(ret_shakeStat_en != CRYPTO_HASH_ERROR_ALGO)
        {
            if(wcShakeStatus == 0)
            {
                ret_shakeStat_en = CRYPTO_HASH_SUCCESS;
            }
            else if (wcShakeStatus == BAD_FUNC_ARG)
            {
                ret_shakeStat_en = CRYPTO_HASH_ERROR_ARG;
            }
            else
            {
                ret_shakeStat_en = CRYPTO_HASH_ERROR_FAIL;
            }
        }
        else
        {
            //do nothing
        }
    }
    else
    {
        ret_shakeStat_en = CRYPTO_HASH_ERROR_ARG;
    }
#endif /* WOLFSSL_SHA3 */    
    return ret_shakeStat_en;
} 
#endif /* CRYPTO_HASH_WC_SHA3_SHAKE_EN */

#ifdef CRYPTO_HASH_WC_BLAKE2_EN
crypto_Hash_Status_E Crypto_Hash_Wc_BlakeDigest(uint8_t *ptr_data, uint32_t dataLen, 
                                                uint8_t *ptr_blakeKey, uint32_t keySize, uint8_t *ptr_digest, uint32_t digestLen, crypto_Hash_Algo_E blakeAlgorithm_en)
{   
	crypto_Hash_Status_E ret_blakeStat_en = CRYPTO_HASH_ERROR_NOTSUPPTED;   
#ifdef HAVE_BLAKE2
	if( (ptr_data != NULL) && (dataLen > 0u) && (ptr_digest != NULL) && (digestLen > 0u) )
    {
#if (defined(HAVE_BLAKE2B) && defined(CRYPTO_HASH_WC_BLAKE2B_EN))        
        if(blakeAlgorithm_en == CRYPTO_HASH_BLAKE2B)
        {
            Blake2b arr_blakeCtx_st[1];

            ret_blakeStat_en =  Crypto_Hash_Wc_BlakeInit((void *)arr_blakeCtx_st, CRYPTO_HASH_BLAKE2B, ptr_blakeKey, keySize, digestLen); 

            if(ret_blakeStat_en == CRYPTO_HASH_SUCCESS)
            {
                ret_blakeStat_en =  Crypto_Hash_Wc_BlakeUpdate((void *)arr_blakeCtx_st, ptr_data, dataLen, CRYPTO_HASH_BLAKE2B);

                if(ret_blakeStat_en == CRYPTO_HASH_SUCCESS)
                {
                  ret_blakeStat_en =  Crypto_Hash_Wc_BlakeFinal((void *)arr_blakeCtx_st, ptr_digest, digestLen, CRYPTO_HASH_BLAKE2B);  
                }  
            }
        }
#endif  /* HAVE_BLAKE2B && CRYPTO_HASH_WC_BLAKE2B_EN */
        
#if (defined(HAVE_BLAKE2S) && defined(CRYPTO_HASH_WC_BLAKE2S_EN)) 
        if(blakeAlgorithm_en == CRYPTO_HASH_BLAKE2S)
        {
            Blake2s arr_blakeCtx_st[1];

            ret_blakeStat_en =  Crypto_Hash_Wc_BlakeInit((void *)arr_blakeCtx_st, CRYPTO_HASH_BLAKE2S, ptr_blakeKey, keySize, digestLen); 

            if(ret_blakeStat_en == CRYPTO_HASH_SUCCESS)
            {
                ret_blakeStat_en =  Crypto_Hash_Wc_BlakeUpdate((void *)arr_blakeCtx_st, ptr_data, dataLen, CRYPTO_HASH_BLAKE2S);

                if(ret_blakeStat_en == CRYPTO_HASH_SUCCESS)
                {
                  ret_blakeStat_en =  Crypto_Hash_Wc_BlakeFinal((void *)arr_blakeCtx_st, ptr_digest, digestLen, CRYPTO_HASH_BLAKE2S);  
                }  
            }
        }
#endif /* HAVE_BLAKE2S && CRYPTO_HASH_WC_BLAKE2S_EN */ 
    }
    else
    {
        ret_blakeStat_en = CRYPTO_HASH_ERROR_ARG;
    }
#endif /* HAVE_BLAKE2 */    
    return ret_blakeStat_en;
}
crypto_Hash_Status_E Crypto_Hash_Wc_BlakeInit(void *ptr_blakeCtx_st, crypto_Hash_Algo_E hashAlgo_en, uint8_t *ptr_blakeKey, uint32_t keySize, uint32_t digestLen)        
{	   
	crypto_Hash_Status_E ret_blakeStat_en = CRYPTO_HASH_ERROR_NOTSUPPTED;
#ifdef HAVE_BLAKE2
    int wcBlakeStatus = BAD_FUNC_ARG;
	if(ptr_blakeCtx_st != NULL)
    {
        switch(hashAlgo_en)
        {
#if (defined(HAVE_BLAKE2B) && defined(CRYPTO_HASH_WC_BLAKE2B_EN))
            case CRYPTO_HASH_BLAKE2B:
                if( (ptr_blakeKey != NULL) && (keySize != 0u) )
                {
                    wcBlakeStatus = wc_InitBlake2b_WithKey((Blake2b*)ptr_blakeCtx_st, digestLen, ptr_blakeKey, keySize);
                }
                else
                {
                    wcBlakeStatus = wc_InitBlake2b((Blake2b*)ptr_blakeCtx_st, digestLen);
                }
                break;
#endif  /* HAVE_BLAKE2B && CRYPTO_HASH_WC_BLAKE2B_EN*/  
                
#if (defined(HAVE_BLAKE2S) && defined(CRYPTO_HASH_WC_BLAKE2S_EN)) 
            case CRYPTO_HASH_BLAKE2S:
                if( (ptr_blakeKey != NULL) && (keySize != 0u) )
                {
                   wcBlakeStatus = wc_InitBlake2s_WithKey((Blake2s*)ptr_blakeCtx_st, digestLen, ptr_blakeKey, keySize); 
                }
                else
                {
                    wcBlakeStatus = wc_InitBlake2s((Blake2s*)ptr_blakeCtx_st, digestLen); 
                }
                break;
#endif /* HAVE_BLAKE2S && CRYPTO_HASH_WC_BLAKE2S_EN*/ 

            default:
                ret_blakeStat_en = CRYPTO_HASH_ERROR_ALGO;
                break;
        }
        
        if(ret_blakeStat_en != CRYPTO_HASH_ERROR_ALGO)
        {
            if(wcBlakeStatus == 0)
            {
                 ret_blakeStat_en = CRYPTO_HASH_SUCCESS;
            }
            else if (wcBlakeStatus == BAD_FUNC_ARG)
            {
                ret_blakeStat_en = CRYPTO_HASH_ERROR_ARG;
            }
            else
            {
                ret_blakeStat_en = CRYPTO_HASH_ERROR_FAIL;
            }
        }
        else
        {
            //do nothing
        }
    }
    else
    {
        ret_blakeStat_en = CRYPTO_HASH_ERROR_ARG;
    }
#endif /* HAVE_BLAKE2 */    
    return ret_blakeStat_en;
}

crypto_Hash_Status_E Crypto_Hash_Wc_BlakeUpdate(void *ptr_blakeCtx_st, uint8_t *ptr_data, uint32_t dataLen, crypto_Hash_Algo_E hashAlgo_en)
{   
	crypto_Hash_Status_E ret_blakeStat_en = CRYPTO_HASH_ERROR_NOTSUPPTED;
#ifdef HAVE_BLAKE2
    int wcBlakeStatus = BAD_FUNC_ARG;
	if(ptr_blakeCtx_st != NULL)
    {
        switch(hashAlgo_en)
        {
#if (defined(HAVE_BLAKE2B) && defined(CRYPTO_HASH_WC_BLAKE2B_EN))
            case CRYPTO_HASH_BLAKE2B:
                wcBlakeStatus = wc_Blake2bUpdate((Blake2b*)ptr_blakeCtx_st, (const byte*)ptr_data, (word32)dataLen);
                break;
#endif  /* HAVE_BLAKE2B && CRYPTO_HASH_WC_BLAKE2B_EN*/  
                
#if (defined(HAVE_BLAKE2S) && defined(CRYPTO_HASH_WC_BLAKE2S_EN)) 
            case CRYPTO_HASH_BLAKE2S:
                
                wcBlakeStatus = wc_Blake2sUpdate((Blake2s*)ptr_blakeCtx_st, (const byte*)ptr_data, (word32)dataLen);
                break;
#endif /* HAVE_BLAKE2S && CRYPTO_HASH_WC_BLAKE2S_EN */ 

            default:
                ret_blakeStat_en = CRYPTO_HASH_ERROR_ALGO;
                break;
        }
        
        if(ret_blakeStat_en != CRYPTO_HASH_ERROR_ALGO)
        {
            if(wcBlakeStatus == 0)
            {
                 ret_blakeStat_en = CRYPTO_HASH_SUCCESS;
            }
            else if (wcBlakeStatus == BAD_FUNC_ARG)
            {
                ret_blakeStat_en = CRYPTO_HASH_ERROR_ARG;
            }
            else
            {
                ret_blakeStat_en = CRYPTO_HASH_ERROR_FAIL;
            }
        }
        else
        {
            //do nothing
        }
    }
    else
    {
        ret_blakeStat_en = CRYPTO_HASH_ERROR_ARG;
    }
#endif /* HAVE_BLAKE2 */    
    return ret_blakeStat_en;       
}

crypto_Hash_Status_E Crypto_Hash_Wc_BlakeFinal(void *ptr_blakeCtx_st, uint8_t *ptr_digest, uint32_t digestLen, crypto_Hash_Algo_E hashAlgo_en)
{   
	crypto_Hash_Status_E ret_blakeStat_en = CRYPTO_HASH_ERROR_NOTSUPPTED;
#ifdef HAVE_BLAKE2
    int wcBlakeStatus = BAD_FUNC_ARG;
	if(ptr_blakeCtx_st != NULL)
    {
        switch(hashAlgo_en)
        {
#if (defined(HAVE_BLAKE2B) && defined(CRYPTO_HASH_WC_BLAKE2B_EN))
            case CRYPTO_HASH_BLAKE2B:
                wcBlakeStatus = wc_Blake2bFinal((Blake2b*)ptr_blakeCtx_st, (byte*)ptr_digest, (word32)digestLen);
                break;
#endif  /* HAVE_BLAKE2B && CRYPTO_HASH_WC_BLAKE2B_EN */  
                
#if (defined(HAVE_BLAKE2S) && defined(CRYPTO_HASH_WC_BLAKE2S_EN)) 
            case CRYPTO_HASH_BLAKE2S:
                
                wcBlakeStatus = wc_Blake2sFinal((Blake2s*)ptr_blakeCtx_st, (byte*)ptr_digest, (word32)digestLen);
                break;
#endif /* HAVE_BLAKE2S && CRYPTO_HASH_WC_BLAKE2S_EN */ 

            default:
                ret_blakeStat_en = CRYPTO_HASH_ERROR_ALGO;
                break;
        }
        
        if(ret_blakeStat_en != CRYPTO_HASH_ERROR_ALGO)
        {
            if(wcBlakeStatus == 0)
            {
                 ret_blakeStat_en = CRYPTO_HASH_SUCCESS;
            }
            else if (wcBlakeStatus == BAD_FUNC_ARG)
            {
                ret_blakeStat_en = CRYPTO_HASH_ERROR_ARG;
            }
            else
            {
                ret_blakeStat_en = CRYPTO_HASH_ERROR_FAIL;
            }
        }
        else
        {
            //do nothing
        }
    }
    else
    {
        ret_blakeStat_en = CRYPTO_HASH_ERROR_ARG;
    }
#endif /* HAVE_BLAKE2 */    
    return ret_blakeStat_en;       
}
#endif /* CRYPTO_HASH_WC_BLAKE2_EN */