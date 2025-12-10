/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    crypto_sym_cipher.c

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
#include "crypto/common_crypto/crypto_common.h"
#include "crypto/common_crypto/crypto_sym_cipher.h"
#include "crypto/wolfcrypt/crypto_sym_wc_wrapper.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

#define CRYPTO_SYM_SESSION_MAX (1) 

// *****************************************************************************
// *****************************************************************************
// Section: Function Definitions
// *****************************************************************************
// *****************************************************************************

crypto_Sym_Status_E Crypto_Sym_Aes_Init(st_Crypto_Sym_BlockCtx *ptr_aesCtx_st, crypto_HandlerType_E handlerType_en, crypto_CipherOper_E cipherOpType_en, 
                                                crypto_Sym_OpModes_E opMode_en, uint8_t *ptr_key, uint32_t keyLen, uint8_t *ptr_initVect, uint32_t sessionID)
{
    crypto_Sym_Status_E ret_aesStatus_en = CRYPTO_SYM_ERROR_CIPNOTSUPPTD;
    
    if(ptr_aesCtx_st == NULL)
    {
        ret_aesStatus_en = CRYPTO_SYM_ERROR_CTX;
    }
    else if( (cipherOpType_en <= CRYPTO_CIOP_INVALID) || (cipherOpType_en >= CRYPTO_CIOP_MAX) )
    {
       ret_aesStatus_en =  CRYPTO_SYM_ERROR_CIPOPER; 
    }
    else if( (opMode_en <= CRYPTO_SYM_OPMODE_INVALID) || (opMode_en >= CRYPTO_SYM_OPMODE_MAX) )
    {
        ret_aesStatus_en = CRYPTO_SYM_ERROR_OPMODE;
    }         
    else if(
            ( (ptr_key == NULL) || (keyLen < (uint32_t)CRYPTO_AESKEYSIZE_128) || (keyLen > (uint32_t)CRYPTO_AESKEYSIZE_256) )  ) //key length check other than XTS mode 
    {
       ret_aesStatus_en =  CRYPTO_SYM_ERROR_KEY;
    }
    else if( (sessionID <= 0u) || (sessionID > (uint32_t)CRYPTO_SYM_SESSION_MAX) )
    {
       ret_aesStatus_en =  CRYPTO_SYM_ERROR_SID; 
    }
    else if( ptr_initVect == NULL
            && (opMode_en != CRYPTO_SYM_OPMODE_ECB)
            )
    {
        ret_aesStatus_en = CRYPTO_SYM_ERROR_IV;
    }
    else
    {
        ptr_aesCtx_st->cryptoSessionID =  sessionID;
        ptr_aesCtx_st->symHandlerType_en = handlerType_en;
        ptr_aesCtx_st->ptr_initVect = ptr_initVect;
        ptr_aesCtx_st->ptr_key = ptr_key;
        ptr_aesCtx_st->symAlgoMode_en = opMode_en;
        ptr_aesCtx_st->symKeySize = keyLen;
        ptr_aesCtx_st->symCipherOper_en = cipherOpType_en;
        
        switch(ptr_aesCtx_st->symHandlerType_en)
        {
            case CRYPTO_HANDLER_SW_WOLFCRYPT:
				if(ptr_aesCtx_st->symAlgoMode_en == CRYPTO_SYM_OPMODE_CTR)
                {                   
                    ret_aesStatus_en = Crypto_Sym_Wc_AesCTR_Init((void*)ptr_aesCtx_st->arr_symDataCtx,ptr_aesCtx_st->ptr_key, ptr_aesCtx_st->symKeySize, ptr_aesCtx_st->ptr_initVect);
                }
				else
                {
                    ret_aesStatus_en = Crypto_Sym_Wc_Aes_Init((void*)ptr_aesCtx_st->arr_symDataCtx,ptr_aesCtx_st->symCipherOper_en, 
                                                  ptr_aesCtx_st->ptr_key, ptr_aesCtx_st->symKeySize, ptr_aesCtx_st->ptr_initVect);

				}
                break;                
            default:
                ret_aesStatus_en = CRYPTO_SYM_ERROR_HDLR;
                break;
        }
        
    }
    return ret_aesStatus_en;
}
	
crypto_Sym_Status_E Crypto_Sym_Aes_Cipher(st_Crypto_Sym_BlockCtx *ptr_aesCtx_st, uint8_t *ptr_inputData, uint32_t dataLen, uint8_t *ptr_outData)
{
    crypto_Sym_Status_E ret_aesStatus_en = CRYPTO_SYM_ERROR_CIPNOTSUPPTD;
    
    if(ptr_aesCtx_st == NULL)
    {
        ret_aesStatus_en = CRYPTO_SYM_ERROR_CTX;
    }
    else if( (ptr_inputData == NULL) || (dataLen == 0u) )
    {
        ret_aesStatus_en = CRYPTO_SYM_ERROR_INPUTDATA; 
    }
    else if(ptr_outData == NULL)
    {
        ret_aesStatus_en = CRYPTO_SYM_ERROR_OUTPUTDATA;
    }
    else
    {
        switch(ptr_aesCtx_st->symHandlerType_en)
        {
            case CRYPTO_HANDLER_SW_WOLFCRYPT:
                if(ptr_aesCtx_st->symCipherOper_en == CRYPTO_CIOP_ENCRYPT)
                {
                    ret_aesStatus_en = Crypto_Sym_Wc_Aes_Encrypt(ptr_aesCtx_st->arr_symDataCtx, ptr_aesCtx_st->symAlgoMode_en, ptr_inputData, dataLen, ptr_outData);
                }
                else if(ptr_aesCtx_st->symCipherOper_en == CRYPTO_CIOP_DECRYPT)
                {
                    ret_aesStatus_en = Crypto_Sym_Wc_Aes_Decrypt(ptr_aesCtx_st->arr_symDataCtx, ptr_aesCtx_st->symAlgoMode_en, ptr_inputData, dataLen, ptr_outData);
                }
                else
                {
                    ret_aesStatus_en = CRYPTO_SYM_ERROR_CIPOPER;
                }
                break;
            default:
                ret_aesStatus_en = CRYPTO_SYM_ERROR_HDLR;
                break;
        }
    }
    return ret_aesStatus_en; 
}

crypto_Sym_Status_E Crypto_Sym_Aes_EncryptDirect(crypto_HandlerType_E handlerType_en, crypto_Sym_OpModes_E opMode_en, uint8_t *ptr_inputData, 
                                                        uint32_t dataLen, uint8_t *ptr_outData, uint8_t *ptr_key, uint32_t keyLen, uint8_t *ptr_initVect, uint32_t sessionID)
{
    crypto_Sym_Status_E ret_aesStatus_en = CRYPTO_SYM_ERROR_CIPNOTSUPPTD;
    
    if( (ptr_inputData == NULL) || (dataLen == 0u) )
    {
        ret_aesStatus_en = CRYPTO_SYM_ERROR_INPUTDATA;
    }
    else if(ptr_outData == NULL)
    {
        ret_aesStatus_en = CRYPTO_SYM_ERROR_OUTPUTDATA;
    }
    else if( (opMode_en <= CRYPTO_SYM_OPMODE_INVALID) || (opMode_en >= CRYPTO_SYM_OPMODE_MAX) )
    {
        ret_aesStatus_en = CRYPTO_SYM_ERROR_OPMODE;
    }   
    else if(ptr_key == NULL)
	{
		ret_aesStatus_en =  CRYPTO_SYM_ERROR_KEY;
	}	
	else if(
            ((keyLen < (uint32_t)CRYPTO_AESKEYSIZE_128) || (keyLen > (uint32_t)CRYPTO_AESKEYSIZE_256) ) ) //key length check other than XTS mode 
    {
       ret_aesStatus_en =  CRYPTO_SYM_ERROR_KEY;
    }
    else if( (sessionID <= 0u ) || (sessionID > (uint32_t)CRYPTO_SYM_SESSION_MAX) )
    {
       ret_aesStatus_en =  CRYPTO_SYM_ERROR_SID; 
    }
    else if( (ptr_initVect == NULL) 
            && (opMode_en != CRYPTO_SYM_OPMODE_ECB) 
            )
    {
        ret_aesStatus_en = CRYPTO_SYM_ERROR_IV;
    }
    else
    {
        switch(handlerType_en)
        {
            case CRYPTO_HANDLER_SW_WOLFCRYPT:
                ret_aesStatus_en = Crypto_Sym_Wc_Aes_EncryptDirect(opMode_en, ptr_inputData, dataLen, ptr_outData, ptr_key, keyLen, ptr_initVect);
                break;
            default:
                ret_aesStatus_en = CRYPTO_SYM_ERROR_HDLR;
                break;
        }
    }
    return ret_aesStatus_en;    
}

crypto_Sym_Status_E Crypto_Sym_Aes_DecryptDirect(crypto_HandlerType_E handlerType_en, crypto_Sym_OpModes_E opMode_en, uint8_t *ptr_inputData, 
                                                        uint32_t dataLen, uint8_t *ptr_outData, uint8_t *ptr_key, uint32_t keyLen, uint8_t *ptr_initVect, uint32_t sessionID)
{
    crypto_Sym_Status_E ret_aesStatus_en = CRYPTO_SYM_ERROR_CIPNOTSUPPTD;
    
    if( (ptr_inputData == NULL) || (dataLen == 0u) )
    {
        ret_aesStatus_en = CRYPTO_SYM_ERROR_INPUTDATA;
    }
    else if(ptr_outData == NULL)
    {
        ret_aesStatus_en = CRYPTO_SYM_ERROR_OUTPUTDATA;
    }
    else if( (opMode_en <= CRYPTO_SYM_OPMODE_INVALID) || (opMode_en >= CRYPTO_SYM_OPMODE_MAX) )
    {
        ret_aesStatus_en = CRYPTO_SYM_ERROR_OPMODE;
    }
    else if(
            ( (ptr_key == NULL) || (keyLen < (uint32_t)CRYPTO_AESKEYSIZE_128) || (keyLen > (uint32_t)CRYPTO_AESKEYSIZE_256) )  ) //key length check other than XTS mode 
    {
       ret_aesStatus_en =  CRYPTO_SYM_ERROR_KEY;
    }
    else if( (sessionID <= 0u) || (sessionID > (uint32_t)CRYPTO_SYM_SESSION_MAX) )
    {
       ret_aesStatus_en =  CRYPTO_SYM_ERROR_SID; 
    }
    else if( (ptr_initVect == NULL)
            && (opMode_en != CRYPTO_SYM_OPMODE_ECB)
            )
    {
        ret_aesStatus_en = CRYPTO_SYM_ERROR_IV;
    }
    else
    {
        switch(handlerType_en)
        {
            case CRYPTO_HANDLER_SW_WOLFCRYPT:
                ret_aesStatus_en = Crypto_Sym_Wc_Aes_DecryptDirect(opMode_en, ptr_inputData, dataLen, ptr_outData, ptr_key, keyLen, ptr_initVect);
                break;
                
            default:
                ret_aesStatus_en = CRYPTO_SYM_ERROR_HDLR;
                break;
        }
    }
    return ret_aesStatus_en;    
}
