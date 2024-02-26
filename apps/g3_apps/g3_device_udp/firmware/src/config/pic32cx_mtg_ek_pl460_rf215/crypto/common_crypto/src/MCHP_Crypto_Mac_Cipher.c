/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    MCHP_Crypto_Mac_Cipher.c

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
#include "crypto/common_crypto/MCHP_Crypto_Mac_Config.h"
#include "crypto/common_crypto/MCHP_Crypto_Mac_Cipher.h"

#ifdef CRYPTO_MAC_WC_ALGO_EN
#include "crypto/common_crypto/MCHP_Crypto_Mac_WolfcryptWrapper.h"
#endif /* CRYPTO_MAC_WC_ALGO_EN */
// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************
#ifdef CRYPTO_MAC_AESCMAC_EN 
crypto_Mac_Status_E Crypto_Mac_AesCmac_Init(st_Crypto_Mac_Aes_ctx *ptr_aesCmacCtx_st, crypto_HandlerType_E handlerType_en, 
                                              uint8_t *ptr_key, uint32_t keyLen, uint32_t sessionID)
{
    crypto_Mac_Status_E ret_aesCmacStat_en = CRYPTO_MAC_ERROR_CIPNOTSUPPTD;
    
    if(ptr_aesCmacCtx_st == NULL)
    {
        ret_aesCmacStat_en = CRYPTO_MAC_ERROR_CTX;
    }
    else if( (ptr_key == NULL) || (keyLen < (uint32_t)CRYPTO_AESKEYSIZE_128) || (keyLen > (uint32_t)CRYPTO_AESKEYSIZE_256)  ) 
    {
       ret_aesCmacStat_en =  CRYPTO_MAC_ERROR_KEY;
    }
    else if( (sessionID <= 0u) || (sessionID > (uint32_t)CRYPTO_MAC_SESSION_MAX) )
    {
       ret_aesCmacStat_en =  CRYPTO_MAC_ERROR_SID; 
    }
    else
    {
        ptr_aesCmacCtx_st->cryptoSessionID =  sessionID;
        ptr_aesCmacCtx_st->macHandlerType_en = handlerType_en;
        ptr_aesCmacCtx_st->ptr_key = ptr_key;
        ptr_aesCmacCtx_st->mackeyLen = keyLen;
        
        switch(ptr_aesCmacCtx_st->macHandlerType_en)
        {
#ifdef CRYPTO_MAC_WC_AESCMAC_EN             
            case CRYPTO_HANDLER_SW_WOLFCRYPT:
                    ret_aesCmacStat_en = Crypto_Mac_Wc_AesCmac_Init((void*)ptr_aesCmacCtx_st->arr_macDataCtx, ptr_aesCmacCtx_st->ptr_key, ptr_aesCmacCtx_st->mackeyLen);     
                break;
#endif /* CRYPTO_MAC_WC_AESCMAC_EN */
            case CRYPTO_HANDLER_HW_INTERNAL:
                
                break;
            default:
                ret_aesCmacStat_en = CRYPTO_MAC_ERROR_HDLR;
                break;
        }
        
    }
    return ret_aesCmacStat_en;
}

crypto_Mac_Status_E Crypto_Mac_AesCmac_Cipher(st_Crypto_Mac_Aes_ctx *ptr_aesCmacCtx_st, uint8_t *ptr_inputData, uint32_t dataLen)
{
    crypto_Mac_Status_E ret_aesCmacStat_en = CRYPTO_MAC_ERROR_CIPNOTSUPPTD;
    
    if(ptr_aesCmacCtx_st == NULL)
    {
        ret_aesCmacStat_en = CRYPTO_MAC_ERROR_CTX;
    }
    else if( (ptr_inputData == NULL) || (dataLen == 0u) )
    {
        ret_aesCmacStat_en = CRYPTO_MAC_ERROR_INPUTDATA;
    }
    else
    {
        switch(ptr_aesCmacCtx_st->macHandlerType_en)
        {
#ifdef CRYPTO_MAC_WC_AESCMAC_EN           
            case CRYPTO_HANDLER_SW_WOLFCRYPT:
                ret_aesCmacStat_en = Crypto_Mac_Wc_AesCmac_Cipher((void*)ptr_aesCmacCtx_st->arr_macDataCtx, ptr_inputData, dataLen);
            break; 
#endif /* CRYPTO_MAC_WC_AESCMAC_EN */          
            case CRYPTO_HANDLER_HW_INTERNAL:

            break;
            default:
                ret_aesCmacStat_en = CRYPTO_MAC_ERROR_HDLR;
            break;
        }
    }
    return ret_aesCmacStat_en;
}

crypto_Mac_Status_E Crypto_Mac_AesCmac_Final(st_Crypto_Mac_Aes_ctx *ptr_aesCmacCtx_st, uint8_t *ptr_outMac, uint32_t macLen)
{
    crypto_Mac_Status_E ret_aesCmacStat_en = CRYPTO_MAC_ERROR_CIPNOTSUPPTD;
    if(ptr_aesCmacCtx_st == NULL)
    {
        ret_aesCmacStat_en = CRYPTO_MAC_ERROR_CTX;
    }
    else if(ptr_outMac == NULL || macLen == 0u)
    {
        ret_aesCmacStat_en = CRYPTO_MAC_ERROR_MACDATA;
    }
    else
    {
        switch(ptr_aesCmacCtx_st->macHandlerType_en)
        {
#ifdef CRYPTO_MAC_WC_AESCMAC_EN           
            case CRYPTO_HANDLER_SW_WOLFCRYPT:
                ret_aesCmacStat_en = Crypto_Mac_Wc_AesCmac_Final((void*)ptr_aesCmacCtx_st->arr_macDataCtx, ptr_outMac, macLen);
            break; 
#endif /* CRYPTO_MAC_WC_AESCMAC_EN */           
            case CRYPTO_HANDLER_HW_INTERNAL:

            break;
            default:
                ret_aesCmacStat_en = CRYPTO_MAC_ERROR_HDLR;
            break;
        }
    }
    return ret_aesCmacStat_en;
}

crypto_Mac_Status_E Crypto_Mac_AesCmac_Direct(crypto_HandlerType_E macHandlerType_en, uint8_t *ptr_inputData, uint32_t dataLen, 
                                                uint8_t *ptr_outMac, uint32_t macLen, uint8_t *ptr_key, uint32_t keyLen, uint32_t sessionID)
{
    crypto_Mac_Status_E ret_aesCmacStat_en = CRYPTO_MAC_ERROR_CIPNOTSUPPTD;
    if( (ptr_inputData == NULL) || (dataLen == 0u) )
    {
        ret_aesCmacStat_en = CRYPTO_MAC_ERROR_INPUTDATA;
    }
    else if( (ptr_outMac == NULL) || (macLen == 0u) )
    {
        ret_aesCmacStat_en = CRYPTO_MAC_ERROR_MACDATA;
    }
    else if( (ptr_key == NULL) || (keyLen < (uint32_t)CRYPTO_AESKEYSIZE_128) || (keyLen > (uint32_t)CRYPTO_AESKEYSIZE_256)  ) 
    {
       ret_aesCmacStat_en =  CRYPTO_MAC_ERROR_KEY;
    }
    else if( (sessionID <= 0u) || (sessionID > (uint32_t)CRYPTO_MAC_SESSION_MAX) )
    {
       ret_aesCmacStat_en =  CRYPTO_MAC_ERROR_SID; 
    }
    else
    {
        switch(macHandlerType_en)
        {
#ifdef CRYPTO_MAC_WC_AESCMAC_EN           
            case CRYPTO_HANDLER_SW_WOLFCRYPT:
                ret_aesCmacStat_en = Crypto_Mac_Wc_AesCmac_Direct(ptr_inputData, dataLen, ptr_outMac, macLen, ptr_key, keyLen);
            break; 
#endif /* CRYPTO_MAC_WC_AESCMAC_EN */           
            case CRYPTO_HANDLER_HW_INTERNAL:

            break;
            default:
                ret_aesCmacStat_en = CRYPTO_MAC_ERROR_HDLR;
            break;
        }
    }
    return ret_aesCmacStat_en;
}
#endif /* #ifdef CRYPTO_MAC_AESCMAC_EN */
// *****************************************************************************