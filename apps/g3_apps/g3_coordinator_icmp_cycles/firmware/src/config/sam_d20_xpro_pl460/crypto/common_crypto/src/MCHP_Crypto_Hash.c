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
#include "crypto/common_crypto/MCHP_Crypto_Hash_Config.h"
#include "crypto/common_crypto/MCHP_Crypto_Hash.h"
#ifdef CRYPTO_HASH_WC_ALGO_EN
#include "crypto/common_crypto/MCHP_Crypto_Hash_WolfcryptWrapper.h"
#endif /* CRYPTO_HASH_WC_ALGO_EN */
// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
//MD5 Algorithm
#ifdef CRYPTO_HASH_MD5_EN
crypto_Hash_Status_E Crypto_Hash_Md5_Digest(crypto_HandlerType_E md5Handler_en, uint8_t *ptr_data, uint32_t dataLen, uint8_t *ptr_digest, uint32_t md5SessionId)
{
   	crypto_Hash_Status_E ret_md5Stat_en = CRYPTO_HASH_ERROR_NOTSUPPTED;

    if( (ptr_data == NULL) || (dataLen == 0u) )
    {
        ret_md5Stat_en = CRYPTO_HASH_ERROR_INPUTDATA;
    }
    else if(ptr_digest == NULL)
    {      
        ret_md5Stat_en = CRYPTO_HASH_ERROR_OUTPUTDATA;
    }
    else if( (md5SessionId <= 0u) || (md5SessionId > (uint32_t)CRYPTO_HASH_SESSION_MAX) )
    {
        ret_md5Stat_en = CRYPTO_HASH_ERROR_SID;
    }
    else
    {
        switch(md5Handler_en)
        {
#ifdef CRYPTO_HASH_WC_MD5_EN            
            case CRYPTO_HANDLER_SW_WOLFCRYPT:
                ret_md5Stat_en = Crypto_Hash_Wc_Md5Digest(ptr_data, dataLen, ptr_digest);
                break;
#endif /* CRYPTO_HASH_WC_MD5_EN */
            case CRYPTO_HANDLER_HW_INTERNAL:

                break;   
                
            default:
                ret_md5Stat_en = CRYPTO_HASH_ERROR_HDLR;
                break;
        };
    }
	return ret_md5Stat_en;  
}

crypto_Hash_Status_E Crypto_Hash_Md5_Init(st_Crypto_Hash_Md5_Ctx *ptr_md5Ctx_st, crypto_HandlerType_E md5HandlerType_en, uint32_t md5SessionId)
{
	crypto_Hash_Status_E ret_md5Stat_en = CRYPTO_HASH_ERROR_NOTSUPPTED;

    if(ptr_md5Ctx_st == NULL)
    {
        ret_md5Stat_en = CRYPTO_HASH_ERROR_CTX;
    }
    else if( (md5SessionId <= 0u) || (md5SessionId > (uint32_t)CRYPTO_HASH_SESSION_MAX) )
    {
        ret_md5Stat_en = CRYPTO_HASH_ERROR_SID;
    }
    else
    {
        ptr_md5Ctx_st->md5SessionId = md5SessionId;
        ptr_md5Ctx_st->md5Handler_en = md5HandlerType_en;
        
        switch(ptr_md5Ctx_st->md5Handler_en)
        {
#ifdef CRYPTO_HASH_WC_MD5_EN            
            case CRYPTO_HANDLER_SW_WOLFCRYPT:
                ret_md5Stat_en = Crypto_Hash_Wc_Md5Init((void*)ptr_md5Ctx_st->arr_md5DataCtx);
                break;
#endif /* CRYPTO_HASH_WC_MD5_EN */
            case CRYPTO_HANDLER_HW_INTERNAL:

                break;   
                
            default:
                ret_md5Stat_en = CRYPTO_HASH_ERROR_HDLR;
                break;
        };
    }
	return ret_md5Stat_en;
}

crypto_Hash_Status_E Crypto_Hash_Md5_Update(st_Crypto_Hash_Md5_Ctx * ptr_md5Ctx_st, uint8_t *ptr_data, uint32_t dataLen)
{
	crypto_Hash_Status_E ret_md5Stat_en = CRYPTO_HASH_ERROR_NOTSUPPTED;
    
    if(ptr_md5Ctx_st == NULL)
    {
        ret_md5Stat_en = CRYPTO_HASH_ERROR_CTX;
    }
    else if( (ptr_data == NULL) || (dataLen == 0u) )
    {
        ret_md5Stat_en = CRYPTO_HASH_ERROR_INPUTDATA;
    }
    else
    {
        switch(ptr_md5Ctx_st->md5Handler_en)
        {
#ifdef CRYPTO_HASH_WC_MD5_EN            
            case CRYPTO_HANDLER_SW_WOLFCRYPT:
                ret_md5Stat_en = Crypto_Hash_Wc_Md5Update((void*)ptr_md5Ctx_st->arr_md5DataCtx, ptr_data, dataLen);
                break;
#endif /* CRYPTO_HASH_WC_MD5_EN */
            case CRYPTO_HANDLER_HW_INTERNAL:
                break;   
                
            default:
                ret_md5Stat_en = CRYPTO_HASH_ERROR_HDLR;
                break;
        };
    }
	return ret_md5Stat_en;
}

crypto_Hash_Status_E Crypto_Hash_Md5_Final(st_Crypto_Hash_Md5_Ctx * ptr_md5Ctx_st, uint8_t *ptr_digest)
{
   	crypto_Hash_Status_E ret_md5Stat_en = CRYPTO_HASH_ERROR_NOTSUPPTED;
    
    if(ptr_md5Ctx_st == NULL)
    {
        ret_md5Stat_en = CRYPTO_HASH_ERROR_CTX;
    }
    else if(ptr_digest == NULL)
    {      
        ret_md5Stat_en = CRYPTO_HASH_ERROR_OUTPUTDATA;
    }
    else
    {
        switch(ptr_md5Ctx_st->md5Handler_en)
        {
#ifdef CRYPTO_HASH_WC_MD5_EN            
            case CRYPTO_HANDLER_SW_WOLFCRYPT:
                ret_md5Stat_en = Crypto_Hash_Wc_Md5Final((void*)ptr_md5Ctx_st->arr_md5DataCtx, ptr_digest);
                break;
#endif /* CRYPTO_HASH_WC_MD5_EN */
            case CRYPTO_HANDLER_HW_INTERNAL:
                break;   
                
            default:
                ret_md5Stat_en = CRYPTO_HASH_ERROR_HDLR;
                break;
        };
    }
	return ret_md5Stat_en; 
}
#endif /* CRYPTO_HASH_MD5_EN */

//RIPEMD160
#ifdef CRYPTO_HASH_RIPEMD160_EN
crypto_Hash_Status_E Crypto_Hash_Ripemd160_Digest(crypto_HandlerType_E ripedmd160Handler_en, uint8_t *ptr_data, uint32_t dataLen, uint8_t *ptr_digest, uint32_t ripemdSessionId)
{
   	crypto_Hash_Status_E ret_ripemdStat_en = CRYPTO_HASH_ERROR_NOTSUPPTED;
    
    if( (ptr_data == NULL) || (dataLen == 0u) )
    {
        ret_ripemdStat_en = CRYPTO_HASH_ERROR_INPUTDATA;
    }
    else if(ptr_digest == NULL)
    {      
        ret_ripemdStat_en = CRYPTO_HASH_ERROR_OUTPUTDATA;
    }
    else if( (ripemdSessionId <= 0u) || (ripemdSessionId > (uint32_t)CRYPTO_HASH_SESSION_MAX))
    {
        ret_ripemdStat_en = CRYPTO_HASH_ERROR_SID;
    }
    else
    {
        switch(ripedmd160Handler_en)
        {
#ifdef CRYPTO_HASH_WC_RIPEMD160_EN
            case CRYPTO_HANDLER_SW_WOLFCRYPT:
                ret_ripemdStat_en = Crypto_Hash_Wc_Ripemd160Digest(ptr_data, dataLen, ptr_digest);
                break;
#endif /* CRYPTO_HASH_WC_RIPEMD160_EN */
            case CRYPTO_HANDLER_HW_INTERNAL:

                break;

            default:
                ret_ripemdStat_en = CRYPTO_HASH_ERROR_HDLR;
                break;
        } 
    } 
	return ret_ripemdStat_en; 
    
}
//RIPEMD160
crypto_Hash_Status_E Crypto_Hash_Ripemd160_Init(st_Crypto_Hash_Ripemd160_Ctx *ptr_ripemdCtx_st, crypto_HandlerType_E ripedmd160Handler_en, uint32_t ripemdSessionId)
{
	crypto_Hash_Status_E ret_ripemdStat_en = CRYPTO_HASH_ERROR_NOTSUPPTED;
    
    if(ptr_ripemdCtx_st == NULL)
    {
        ret_ripemdStat_en = CRYPTO_HASH_ERROR_CTX;
    }
    else if( (ripemdSessionId <= 0u) || (ripemdSessionId > (uint32_t)CRYPTO_HASH_SESSION_MAX) )
    {
        ret_ripemdStat_en = CRYPTO_HASH_ERROR_SID;
    }
    else
    {
        ptr_ripemdCtx_st->ripemd160SessionId = ripemdSessionId;
        ptr_ripemdCtx_st->ripedmd160Handler_en = ripedmd160Handler_en;
        
        switch(ptr_ripemdCtx_st->ripedmd160Handler_en)
        {
#ifdef CRYPTO_HASH_WC_RIPEMD160_EN   
            case CRYPTO_HANDLER_SW_WOLFCRYPT:
                ret_ripemdStat_en = Crypto_Hash_Wc_Ripemd160Init((void*)ptr_ripemdCtx_st->arr_ripemd160DataCtx);
                break;
#endif /* CRYPTO_HASH_WC_RIPEMD160_EN */
            case CRYPTO_HANDLER_HW_INTERNAL:

                break;

            default:
                ret_ripemdStat_en = CRYPTO_HASH_ERROR_HDLR;
                break;
        }
    } 
	return ret_ripemdStat_en;
}

crypto_Hash_Status_E Crypto_Hash_Ripemd160_Update(st_Crypto_Hash_Ripemd160_Ctx *ptr_ripemdCtx_st, uint8_t *ptr_data, uint32_t dataLen)
{
	crypto_Hash_Status_E ret_ripemdStat_en = CRYPTO_HASH_ERROR_NOTSUPPTED;
    
    if(ptr_ripemdCtx_st == NULL)
    {
        ret_ripemdStat_en = CRYPTO_HASH_ERROR_CTX;
    }
    if( (ptr_data == NULL) || (dataLen == 0u) )
    {
        ret_ripemdStat_en = CRYPTO_HASH_ERROR_INPUTDATA;
    }
    else
    {
        switch(ptr_ripemdCtx_st->ripedmd160Handler_en)
        {
#ifdef CRYPTO_HASH_WC_RIPEMD160_EN
            case CRYPTO_HANDLER_SW_WOLFCRYPT:
                ret_ripemdStat_en = Crypto_Hash_Wc_Ripemd160Update((void*)ptr_ripemdCtx_st->arr_ripemd160DataCtx, ptr_data, dataLen);
                break;
#endif /* CRYPTO_HASH_WC_RIPEMD160_EN */
            case CRYPTO_HANDLER_HW_INTERNAL:
                break;   
                
            default:
                ret_ripemdStat_en = CRYPTO_HASH_ERROR_HDLR;
                break;
        };
    }
	return ret_ripemdStat_en;
}

crypto_Hash_Status_E Crypto_Hash_Ripemd160_Final (st_Crypto_Hash_Ripemd160_Ctx *ptr_ripemdCtx_st, uint8_t *ptr_digest)
{
   	crypto_Hash_Status_E ret_ripemdStat_en = CRYPTO_HASH_ERROR_NOTSUPPTED;
    
    if(ptr_ripemdCtx_st == NULL)
    {
        ret_ripemdStat_en = CRYPTO_HASH_ERROR_CTX;
    }
    else if(ptr_digest == NULL)
    {
         ret_ripemdStat_en = CRYPTO_HASH_ERROR_OUTPUTDATA;   
    }
    else
    {
        switch(ptr_ripemdCtx_st->ripedmd160Handler_en)
        {
#ifdef CRYPTO_HASH_WC_RIPEMD160_EN 
            case CRYPTO_HANDLER_SW_WOLFCRYPT:
                ret_ripemdStat_en = Crypto_Hash_Wc_Ripemd160Final((void*)ptr_ripemdCtx_st->arr_ripemd160DataCtx, ptr_digest);
                break;
#endif /* CRYPTO_HASH_WC_RIPEMD160_EN */
            case CRYPTO_HANDLER_HW_INTERNAL:
                break;   
                
            default:
                ret_ripemdStat_en = CRYPTO_HASH_ERROR_HDLR;
                break;
        };
    }
	return ret_ripemdStat_en; 
}
#endif /* CRYPTO_HASH_RIPEMD160_EN */

//SHA-1, SHA-2 and SHA-3 except SHAKE
#if (defined(CRYPTO_HASH_SHA1_EN) || defined(CRYPTO_HASH_SHA2_EN) || defined(CRYPTO_HASH_SHA3_EN))
crypto_Hash_Status_E Crypto_Hash_Sha_Digest(crypto_HandlerType_E shaHandler_en, uint8_t *ptr_data, uint32_t dataLen, 
                                                uint8_t *ptr_digest, crypto_Hash_Algo_E shaAlgorithm_en,  uint32_t shaSessionId)
{
 	crypto_Hash_Status_E ret_shaStat_en = CRYPTO_HASH_ERROR_NOTSUPPTED;
    
    if( (ptr_data == NULL) || (dataLen == 0u) )
    {
        ret_shaStat_en = CRYPTO_HASH_ERROR_INPUTDATA;
    }
    else if(ptr_digest == NULL)
    {      
        ret_shaStat_en = CRYPTO_HASH_ERROR_OUTPUTDATA;
    }
    else if( (shaAlgorithm_en <= CRYPTO_HASH_INVALID) || (shaAlgorithm_en >= CRYPTO_HASH_MAX))
    {
        ret_shaStat_en = CRYPTO_HASH_ERROR_ALGO;
    }
    else if( (shaSessionId <= 0u) || (shaSessionId > (uint32_t)CRYPTO_HASH_SESSION_MAX) )
    {
        ret_shaStat_en = CRYPTO_HASH_ERROR_SID;
    }
	else
    {
        switch(shaHandler_en)
        {
#if (defined(CRYPTO_HASH_WC_SHA1_EN) || defined(CRYPTO_HASH_WC_SHA2_EN) || defined(CRYPTO_HASH_WC_SHA3_EN))           
            case CRYPTO_HANDLER_SW_WOLFCRYPT:
                ret_shaStat_en = Crypto_Hash_Wc_ShaDigest(ptr_data, dataLen, ptr_digest, shaAlgorithm_en);
                break;
#endif /* CRYPTO_HASH_WC_SHA1_EN || CRYPTO_HASH_WC_SHA2_EN || CRYPTO_HASH_WC_SHA3_EN */
            case CRYPTO_HANDLER_HW_INTERNAL:
                break;

            default:
                ret_shaStat_en = CRYPTO_HASH_ERROR_HDLR;
                break;
        }
    }
	return ret_shaStat_en;
}

crypto_Hash_Status_E Crypto_Hash_Sha_Init(st_Crypto_Hash_Sha_Ctx *ptr_shaCtx_st, crypto_Hash_Algo_E shaAlgorithm_en, crypto_HandlerType_E shaHandler_en, uint32_t shaSessionId)
{
 	crypto_Hash_Status_E ret_shaStat_en = CRYPTO_HASH_ERROR_NOTSUPPTED;
    
    if(ptr_shaCtx_st == NULL)
    {
        ret_shaStat_en = CRYPTO_HASH_ERROR_CTX;
    }
    else if( (shaAlgorithm_en <= CRYPTO_HASH_INVALID) || (shaAlgorithm_en >= CRYPTO_HASH_MAX))
    {
        ret_shaStat_en = CRYPTO_HASH_ERROR_ALGO;
    }
    else if( (shaSessionId <= 0u) || (shaSessionId > (uint32_t)CRYPTO_HASH_SESSION_MAX) )
    {
        ret_shaStat_en = CRYPTO_HASH_ERROR_SID;
    }
	else
    {
        ptr_shaCtx_st->shaSessionId = shaSessionId;
        ptr_shaCtx_st->shaAlgo_en = shaAlgorithm_en;
        ptr_shaCtx_st->shaHandler_en = shaHandler_en;

        switch(ptr_shaCtx_st->shaHandler_en)
        {
#if (defined(CRYPTO_HASH_WC_SHA1_EN) || defined(CRYPTO_HASH_WC_SHA2_EN) || defined(CRYPTO_HASH_WC_SHA3_EN))   
            case CRYPTO_HANDLER_SW_WOLFCRYPT:
                ret_shaStat_en = Crypto_Hash_Wc_ShaInit((void*)ptr_shaCtx_st->arr_shaDataCtx, ptr_shaCtx_st->shaAlgo_en);
                break;
#endif /* CRYPTO_HASH_WC_SHA1_EN || CRYPTO_HASH_WC_SHA2_EN || CRYPTO_HASH_WC_SHA3_EN */
            case CRYPTO_HANDLER_HW_INTERNAL:
                break;

            default:
                ret_shaStat_en = CRYPTO_HASH_ERROR_HDLR;
                break;
        }
    }
	return ret_shaStat_en;
}

crypto_Hash_Status_E Crypto_Hash_Sha_Update(st_Crypto_Hash_Sha_Ctx *ptr_shaCtx_st, uint8_t *ptr_data, uint32_t dataLen)
{
	crypto_Hash_Status_E ret_shaStat_en = CRYPTO_HASH_ERROR_NOTSUPPTED;
    
    if(ptr_shaCtx_st == NULL)
    {
        ret_shaStat_en = CRYPTO_HASH_ERROR_CTX;
    }
    if( (ptr_data == NULL) || (dataLen == 0u) )
    {
        ret_shaStat_en = CRYPTO_HASH_ERROR_INPUTDATA;
    }
    else
    {
        switch(ptr_shaCtx_st->shaHandler_en)
        {
#if (defined(CRYPTO_HASH_WC_SHA1_EN) || defined(CRYPTO_HASH_WC_SHA2_EN) || defined(CRYPTO_HASH_WC_SHA3_EN))       
            case CRYPTO_HANDLER_SW_WOLFCRYPT:
                ret_shaStat_en = Crypto_Hash_Wc_ShaUpdate((void*)ptr_shaCtx_st->arr_shaDataCtx, ptr_data, dataLen, ptr_shaCtx_st->shaAlgo_en);
                break;
#endif /* CRYPTO_HASH_WC_SHA1_EN || CRYPTO_HASH_WC_SHA2_EN || CRYPTO_HASH_WC_SHA3_EN */
            case CRYPTO_HANDLER_HW_INTERNAL:
                break;

            default:
                ret_shaStat_en = CRYPTO_HASH_ERROR_HDLR;
                break;
        }
    }
	return ret_shaStat_en;
}

crypto_Hash_Status_E Crypto_Hash_Sha_Final(st_Crypto_Hash_Sha_Ctx *ptr_shaCtx_st, uint8_t *ptr_digest)
{
	crypto_Hash_Status_E ret_shaStat_en = CRYPTO_HASH_ERROR_NOTSUPPTED;
    
    if(ptr_shaCtx_st == NULL)
    {
        ret_shaStat_en = CRYPTO_HASH_ERROR_CTX;
    }
    else if(ptr_digest == NULL)
    {
        ret_shaStat_en = CRYPTO_HASH_ERROR_OUTPUTDATA;
    }
	else
    {
        switch(ptr_shaCtx_st->shaHandler_en)
        {
#if (defined(CRYPTO_HASH_WC_SHA1_EN) || defined(CRYPTO_HASH_WC_SHA2_EN) || defined(CRYPTO_HASH_WC_SHA3_EN))             
            case CRYPTO_HANDLER_SW_WOLFCRYPT:
                ret_shaStat_en = Crypto_Hash_Wc_ShaFinal((void*)ptr_shaCtx_st->arr_shaDataCtx, ptr_digest, ptr_shaCtx_st->shaAlgo_en);
                break;
#endif /* CRYPTO_HASH_WC_SHA1_EN || CRYPTO_HASH_WC_SHA2_EN || CRYPTO_HASH_WC_SHA3_EN */
            case CRYPTO_HANDLER_HW_INTERNAL:
                break;

            default:
                ret_shaStat_en = CRYPTO_HASH_ERROR_HDLR;
                break;
        }
    }
	return ret_shaStat_en;
}
#endif /* CRYPTO_HASH_SHA1_EN || CRYPTO_HASH_SHA2_EN || CRYPTO_HASH_SHA3_EN */


//SHAKE
#ifdef CRYPTO_HASH_SHA3_SHAKE_EN
crypto_Hash_Status_E Crypto_Hash_Shake_Digest(crypto_HandlerType_E shakeHandlerType_en, crypto_Hash_Algo_E shakeAlgorithm_en, 
                                                    uint8_t *ptr_data, uint32_t dataLen, uint8_t *ptr_digest, uint32_t digestLen, uint32_t shakeSessionId)
{
    crypto_Hash_Status_E ret_shakeStat_en = CRYPTO_HASH_ERROR_NOTSUPPTED;
    
    if( (shakeAlgorithm_en <= CRYPTO_HASH_INVALID) || (shakeAlgorithm_en >= CRYPTO_HASH_MAX))
    {
        ret_shakeStat_en = CRYPTO_HASH_ERROR_ALGO;
    }   
    else if( (ptr_data == NULL) || (dataLen == 0u) )
    {
        ret_shakeStat_en = CRYPTO_HASH_ERROR_INPUTDATA;
    }
    else if( (ptr_digest == NULL) || (digestLen == 0u) )
    {      
        ret_shakeStat_en = CRYPTO_HASH_ERROR_OUTPUTDATA;
    }
    else if( (shakeSessionId <= 0u) || (shakeSessionId > (uint32_t)CRYPTO_HASH_SESSION_MAX) )
    {
        ret_shakeStat_en = CRYPTO_HASH_ERROR_SID;
    }
	else
    {
        switch(shakeHandlerType_en)
        {
#ifdef CRYPTO_HASH_WC_SHA3_SHAKE_EN            
            case CRYPTO_HANDLER_SW_WOLFCRYPT:
                ret_shakeStat_en = Crypto_Hash_Wc_ShakeDigest(ptr_data, dataLen, ptr_digest, digestLen, shakeAlgorithm_en);
                break;
#endif /* CRYPTO_HASH_WC_SHA3_SHAKE_EN */
            case CRYPTO_HANDLER_HW_INTERNAL:
                break;

            default:
                ret_shakeStat_en = CRYPTO_HASH_ERROR_HDLR;
                break;
        }
    }
	return ret_shakeStat_en;   
}

crypto_Hash_Status_E Crypto_Hash_Shake_Init(st_Crypto_Hash_Shake_Ctx* ptr_shakeCtx_st, crypto_Hash_Algo_E shakeAlgorithm_en, 
                                                crypto_HandlerType_E shakeHandlerType_en, uint32_t digestLen, uint32_t shakeSessionId)
{
  	crypto_Hash_Status_E ret_shakeStat_en = CRYPTO_HASH_ERROR_NOTSUPPTED;
    
    if(ptr_shakeCtx_st == NULL)
    {
        ret_shakeStat_en = CRYPTO_HASH_ERROR_CTX;
    }
    if( (shakeAlgorithm_en <= CRYPTO_HASH_INVALID) || (shakeAlgorithm_en >= CRYPTO_HASH_MAX))
    {
        ret_shakeStat_en = CRYPTO_HASH_ERROR_ALGO;
    }   
    else if(digestLen == 0u)
    {      
        ret_shakeStat_en = CRYPTO_HASH_ERROR_ARG;
    }
    else if( (shakeSessionId <= 0u) || (shakeSessionId > (uint32_t)CRYPTO_HASH_SESSION_MAX) )
    {
        ret_shakeStat_en = CRYPTO_HASH_ERROR_SID;
    }
    else
    {
        ptr_shakeCtx_st->shakeSessionId = shakeSessionId;
        ptr_shakeCtx_st->shakeAlgo_en = shakeAlgorithm_en;
        ptr_shakeCtx_st->shakeHandler_en = shakeHandlerType_en;
        ptr_shakeCtx_st->digestLen = digestLen;

        switch(ptr_shakeCtx_st->shakeHandler_en)
        {
#ifdef CRYPTO_HASH_WC_SHAKE_EN            
            case CRYPTO_HANDLER_SW_WOLFCRYPT:
                ret_shakeStat_en = Crypto_Hash_Wc_ShakeInit((void*)ptr_shakeCtx_st->arr_shakeDataCtx, ptr_shakeCtx_st->shakeAlgo_en);
                break;
#endif /* CRYPTO_HASH_WC_SHAKE_EN */
            case CRYPTO_HANDLER_HW_INTERNAL:
                break;

            default:
                ret_shakeStat_en = CRYPTO_HASH_ERROR_HDLR;
                break;
        }
    }
	return ret_shakeStat_en;   
}

crypto_Hash_Status_E Crypto_Hash_Shake_Update(st_Crypto_Hash_Shake_Ctx* ptr_shakeCtx_st, uint8_t *ptr_data, uint32_t dataLen)
{
  	crypto_Hash_Status_E ret_shakeStat_en = CRYPTO_HASH_ERROR_NOTSUPPTED;
    
    if(ptr_shakeCtx_st == NULL)
    {
        ret_shakeStat_en = CRYPTO_HASH_ERROR_CTX;
    }
    else if( (ptr_data == NULL) || (dataLen == 0u) )
    {
        ret_shakeStat_en = CRYPTO_HASH_ERROR_INPUTDATA;
    }        
	else
    {
        switch(ptr_shakeCtx_st->shakeHandler_en)
        {
#ifdef CRYPTO_HASH_WC_SHAKE_EN            
            case CRYPTO_HANDLER_SW_WOLFCRYPT:
                ret_shakeStat_en = Crypto_Hash_Wc_ShakeUpdate((void*)ptr_shakeCtx_st->arr_shakeDataCtx, ptr_data, dataLen, ptr_shakeCtx_st->shakeAlgo_en);
                break;
#endif /* CRYPTO_HASH_WC_SHAKE_EN */
            case CRYPTO_HANDLER_HW_INTERNAL:
                break;

            default:
                ret_shakeStat_en = CRYPTO_HASH_ERROR_HDLR;
                break;
        }
    }
	return ret_shakeStat_en;   
}

crypto_Hash_Status_E Crypto_Hash_Shake_Final(st_Crypto_Hash_Shake_Ctx* ptr_shakeCtx_st, uint8_t *ptr_digest)
{
  	crypto_Hash_Status_E ret_shakeStat_en = CRYPTO_HASH_ERROR_NOTSUPPTED;
    
    if(ptr_shakeCtx_st == NULL)
    {
        ret_shakeStat_en = CRYPTO_HASH_ERROR_CTX;
    }
    else if(ptr_digest == NULL)
    {      
        ret_shakeStat_en = CRYPTO_HASH_ERROR_OUTPUTDATA;
    }
	else
    {
        switch(ptr_shakeCtx_st->shakeHandler_en)
        {
#ifdef CRYPTO_HASH_WC_SHAKE_EN            
            case CRYPTO_HANDLER_SW_WOLFCRYPT:
                ret_shakeStat_en = Crypto_Hash_Wc_ShakeFinal((void*)ptr_shakeCtx_st->arr_shakeDataCtx, ptr_digest, ptr_shakeCtx_st->digestLen, ptr_shakeCtx_st->shakeAlgo_en);
                break;
#endif /* CRYPTO_HASH_WC_SHAKE_EN */
                
            case CRYPTO_HANDLER_HW_INTERNAL:
                break;

            default:
                ret_shakeStat_en = CRYPTO_HASH_ERROR_HDLR;
                break;
        }
    }
	return ret_shakeStat_en;   
}
#endif /* CRYPTO_HASH_SHA3_SHAKE_EN */


//BLAKE
#ifdef CRYPTO_HASH_WC_BLAKE2_EN
crypto_Hash_Status_E Crypto_Hash_Blake_Digest(crypto_HandlerType_E blakeHandlerType_en, crypto_Hash_Algo_E blakeAlgorithm_en, uint8_t *ptr_data, uint32_t dataLen, 
                                                uint8_t *ptr_blakeKey, uint32_t keySize, uint8_t *ptr_digest, uint32_t digestLen, uint32_t blakeSessionId)
{
    crypto_Hash_Status_E ret_blakeStat_en = CRYPTO_HASH_ERROR_NOTSUPPTED;
    
    if( (blakeAlgorithm_en <= CRYPTO_HASH_INVALID) || (blakeAlgorithm_en >= CRYPTO_HASH_MAX))
    {
        ret_blakeStat_en = CRYPTO_HASH_ERROR_ALGO;
    }   
    else if( (ptr_data == NULL) || (dataLen == 0u) )
    {
        ret_blakeStat_en = CRYPTO_HASH_ERROR_INPUTDATA;
    }
    else if( ((ptr_blakeKey != NULL) && (keySize == 0u)) 
                || ((ptr_blakeKey == NULL) && (keySize > 0u)) )
    {
        ret_blakeStat_en = CRYPTO_HASH_ERROR_KEY;
    }
    else if( (ptr_digest == NULL) || (digestLen == 0u ) )
    {      
        ret_blakeStat_en = CRYPTO_HASH_ERROR_OUTPUTDATA;
    }
    else if( (blakeSessionId <= 0u) || (blakeSessionId > (uint32_t)CRYPTO_HASH_SESSION_MAX) )
    {
        ret_blakeStat_en = CRYPTO_HASH_ERROR_SID;
    }
    else
    {
        switch(blakeHandlerType_en)
        {
#ifdef CRYPTO_HASH_WC_BLAKE2_EN           
            case CRYPTO_HANDLER_SW_WOLFCRYPT:
                ret_blakeStat_en = Crypto_Hash_Wc_BlakeDigest(ptr_data, dataLen, ptr_blakeKey, keySize, ptr_digest, digestLen, blakeAlgorithm_en);
                break;
#endif /* CRYPTO_HASH_WC_BLAKE2_EN */
            case CRYPTO_HANDLER_HW_INTERNAL:
                break;

            default:
                ret_blakeStat_en = CRYPTO_HASH_ERROR_HDLR;
                break;
        }
    }
    return ret_blakeStat_en;    
}
crypto_Hash_Status_E Crypto_Hash_Blake_Init(st_Crypto_Hash_Blake_Ctx* ptr_blakeCtx_st, crypto_Hash_Algo_E blakeAlgorithm_en, uint8_t *ptr_blakeKey,
                                                uint32_t keySize, uint32_t digestSize, crypto_HandlerType_E blakeHandlerType_en, uint32_t blakeSessionId)
{
    crypto_Hash_Status_E ret_blakeStat_en = CRYPTO_HASH_ERROR_NOTSUPPTED;
    
    if(ptr_blakeCtx_st == NULL)
    {
        ret_blakeStat_en = CRYPTO_HASH_ERROR_CTX;
    }
    if( (blakeAlgorithm_en <= CRYPTO_HASH_INVALID) || (blakeAlgorithm_en >= CRYPTO_HASH_MAX))
    {
        ret_blakeStat_en = CRYPTO_HASH_ERROR_ALGO;
    }   
    else if( ((ptr_blakeKey == NULL) && (keySize > 0u) ) 
            || ( (ptr_blakeKey != NULL) &&(keySize == 0u) ) )
    {
        ret_blakeStat_en = CRYPTO_HASH_ERROR_KEY;
    }
    else if(digestSize == 0u)
    {      
        ret_blakeStat_en = CRYPTO_HASH_ERROR_ARG;
    }
    else if( (blakeSessionId <= 0u) || (blakeSessionId > (uint32_t)CRYPTO_HASH_SESSION_MAX) )
    {
        ret_blakeStat_en = CRYPTO_HASH_ERROR_SID;
    }
	else 
    {
        ptr_blakeCtx_st->blakeSessionId = blakeSessionId;
        ptr_blakeCtx_st->blakeAlgo_en = blakeAlgorithm_en;
        ptr_blakeCtx_st->blakeHandler_en = blakeHandlerType_en;
        ptr_blakeCtx_st->digestLen = digestSize;
        ptr_blakeCtx_st->ptr_key = ptr_blakeKey;
        ptr_blakeCtx_st->keyLen = keySize;

        switch(ptr_blakeCtx_st->blakeHandler_en)
        {
#ifdef CRYPTO_HASH_WC_BLAKE2_EN    
            case CRYPTO_HANDLER_SW_WOLFCRYPT:
                ret_blakeStat_en = Crypto_Hash_Wc_BlakeInit((void*)ptr_blakeCtx_st->arr_blakeDataCtx, ptr_blakeCtx_st->blakeAlgo_en, ptr_blakeKey, keySize, digestSize);
                break;
#endif /* CRYPTO_HASH_WC_BLAKE2_EN */
            case CRYPTO_HANDLER_HW_INTERNAL:
                break;

            default:
                ret_blakeStat_en = CRYPTO_HASH_ERROR_HDLR;
                break;
        }
    }
    return ret_blakeStat_en;    
}

crypto_Hash_Status_E Crypto_Hash_Blake_Update(st_Crypto_Hash_Blake_Ctx * ptr_blakeCtx_st, uint8_t *ptr_data, uint32_t dataLen)
{
    crypto_Hash_Status_E ret_blakeStat_en = CRYPTO_HASH_ERROR_NOTSUPPTED;
    
    if(ptr_blakeCtx_st == NULL)
    {
        ret_blakeStat_en = CRYPTO_HASH_ERROR_CTX;
    }
    else if( (ptr_data == NULL) || (dataLen == 0u) )
    {
        ret_blakeStat_en = CRYPTO_HASH_ERROR_INPUTDATA;
    }
	else
    {
        switch(ptr_blakeCtx_st->blakeHandler_en)
        {
#ifdef CRYPTO_HASH_WC_BLAKE2_EN           
            case CRYPTO_HANDLER_SW_WOLFCRYPT:
                ret_blakeStat_en = Crypto_Hash_Wc_BlakeUpdate((void*)ptr_blakeCtx_st->arr_blakeDataCtx, ptr_data, dataLen, ptr_blakeCtx_st->blakeAlgo_en);
                break;
#endif /* CRYPTO_HASH_WC_BLAKE2_EN */
            case CRYPTO_HANDLER_HW_INTERNAL:
                break;

            default:
                ret_blakeStat_en = CRYPTO_HASH_ERROR_HDLR;
                break;
        }
    }
    return ret_blakeStat_en;    
}

crypto_Hash_Status_E Crypto_Hash_Blake_Final(st_Crypto_Hash_Blake_Ctx * ptr_blakeCtx_st, uint8_t *ptr_digest)
{
    crypto_Hash_Status_E ret_blakeStat_en = CRYPTO_HASH_ERROR_NOTSUPPTED;
    
    if(ptr_blakeCtx_st == NULL)
    {
        ret_blakeStat_en = CRYPTO_HASH_ERROR_CTX;
    }
    else if(ptr_digest == NULL)
    {
        ret_blakeStat_en = CRYPTO_HASH_ERROR_OUTPUTDATA;
    }
	else
    {
        switch(ptr_blakeCtx_st->blakeHandler_en)
        {
#ifdef CRYPTO_HASH_WC_BLAKE2_EN         
            case CRYPTO_HANDLER_SW_WOLFCRYPT:
                ret_blakeStat_en = Crypto_Hash_Wc_BlakeFinal((void*)ptr_blakeCtx_st->arr_blakeDataCtx, ptr_digest, ptr_blakeCtx_st->digestLen, ptr_blakeCtx_st->blakeAlgo_en);
                break;
#endif /* CRYPTO_HASH_WC_BLAKE2_EN */
            case CRYPTO_HANDLER_HW_INTERNAL:
                break;

            default:
                ret_blakeStat_en = CRYPTO_HASH_ERROR_HDLR;
                break;
        }
    }
    return ret_blakeStat_en;    
}
#endif /* CRYPTO_HASH_WC_BLAKE2_EN */


