/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    crypto_sym_wc_wrapper.c

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

#include "crypto/common_crypto/crypto_sym_cipher.h"
#include "crypto/wolfcrypt/crypto_sym_wc_wrapper.h"
#include "wolfssl/wolfcrypt/aes.h"
#include "wolfssl/wolfcrypt/error-crypt.h"

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// ***************************************************************************** 
crypto_Sym_Status_E Crypto_Sym_Wc_Aes_Init(void *ptr_aesCtx, crypto_CipherOper_E symCipherOper_en, uint8_t *ptr_key, uint32_t keySize, uint8_t *ptr_initVect)	
{
    crypto_Sym_Status_E ret_aesStatus_en = CRYPTO_SYM_ERROR_CIPNOTSUPPTD;
    int wcAesStatus = BAD_FUNC_ARG;
    int dir = -1;
    if(ptr_aesCtx != NULL)
    {
        if(symCipherOper_en == CRYPTO_CIOP_ENCRYPT)
        {
            dir = AES_ENCRYPTION;
        }
        else if(symCipherOper_en == CRYPTO_CIOP_DECRYPT)
        {
            dir = AES_DECRYPTION;
        }
        else
        {
            ret_aesStatus_en = CRYPTO_SYM_ERROR_CIPOPER;
        }
        if(ret_aesStatus_en != CRYPTO_SYM_ERROR_CIPOPER)
        {
            wcAesStatus = wc_AesSetKey( (Aes*)ptr_aesCtx, (const byte*)ptr_key, (word32)keySize, ptr_initVect, dir);

            if(wcAesStatus == 0)
            {
                ret_aesStatus_en = CRYPTO_SYM_CIPHER_SUCCESS;
            }
            else if (wcAesStatus == WC_KEY_SIZE_E)
            {
                ret_aesStatus_en = CRYPTO_SYM_ERROR_KEY;
            }
            else if(wcAesStatus == BAD_FUNC_ARG)
            {
                ret_aesStatus_en = CRYPTO_SYM_ERROR_ARG;
            }
            else
            {
                ret_aesStatus_en  = CRYPTO_SYM_ERROR_CIPFAIL;
            }
        }
    }
    else
    {
        ret_aesStatus_en = CRYPTO_SYM_ERROR_CTX;
    }
    return ret_aesStatus_en;
}
   
crypto_Sym_Status_E Crypto_Sym_Wc_AesCTR_Init(void *ptr_aesCtx, uint8_t *ptr_key, uint32_t keySize, uint8_t *ptr_initVect)
{
	crypto_Sym_Status_E ret_aesStatus_en = CRYPTO_SYM_ERROR_CIPNOTSUPPTD;
    
    ret_aesStatus_en = Crypto_Sym_Wc_Aes_Init(ptr_aesCtx, CRYPTO_CIOP_ENCRYPT, ptr_key, keySize, ptr_initVect);
	
    return ret_aesStatus_en;
}
	
crypto_Sym_Status_E Crypto_Sym_Wc_Aes_Encrypt(void *ptr_aesCtx, crypto_Sym_OpModes_E symAlgoMode_en, uint8_t *ptr_inputData, uint32_t dataLen, uint8_t *ptr_outData)
{
    crypto_Sym_Status_E ret_aesStatus_en = CRYPTO_SYM_ERROR_CIPNOTSUPPTD;
    int wcAesStatus = BAD_FUNC_ARG;
  
    if(ptr_aesCtx != NULL)
    {
        switch(symAlgoMode_en)
        {
            case CRYPTO_SYM_OPMODE_ECB:
                wcAesStatus = wc_AesEcbEncrypt((Aes*)ptr_aesCtx, (byte*)ptr_outData, (const byte*)ptr_inputData, (word32)dataLen);
                break;
            case CRYPTO_SYM_OPMODE_CTR:
                wcAesStatus = wc_AesCtrEncrypt((Aes*)ptr_aesCtx, (byte*)ptr_outData, (const byte*)ptr_inputData, (word32)dataLen);
                break;
            default:
                ret_aesStatus_en = CRYPTO_SYM_ERROR_OPMODE;
                break;
        } //end of switch
            
        if(wcAesStatus == 0)
        {
            ret_aesStatus_en = CRYPTO_SYM_CIPHER_SUCCESS;
        }
        else if(ret_aesStatus_en != CRYPTO_SYM_ERROR_OPMODE)
        {
            if(wcAesStatus == BAD_FUNC_ARG)
            {
                ret_aesStatus_en = CRYPTO_SYM_ERROR_ARG;
            }
            else
            {
                ret_aesStatus_en  = CRYPTO_SYM_ERROR_CIPFAIL;
            }
        }
        else
        {
            //do nothing
        }
    } //end of if of argument checking
    else
    {
        ret_aesStatus_en = CRYPTO_SYM_ERROR_CTX;
    }
    return ret_aesStatus_en;
}

crypto_Sym_Status_E Crypto_Sym_Wc_Aes_Decrypt(void *ptr_aesCtx, crypto_Sym_OpModes_E symAlgoMode_en, uint8_t *ptr_inputData, uint32_t dataLen, uint8_t *ptr_outData)
{
    crypto_Sym_Status_E ret_aesStatus_En = CRYPTO_SYM_ERROR_CIPNOTSUPPTD;
    int wcAesStatus = BAD_FUNC_ARG;
  
    if(ptr_inputData != NULL)
    {
        switch(symAlgoMode_en)
        {
            case CRYPTO_SYM_OPMODE_ECB:
                wcAesStatus = wc_AesEcbDecrypt( (Aes*)ptr_aesCtx, (byte*)ptr_outData, (const byte*)ptr_inputData, (word32)dataLen);
                break;
            case CRYPTO_SYM_OPMODE_CTR:
                wcAesStatus = wc_AesCtrEncrypt((Aes*)ptr_aesCtx, (byte*)ptr_outData, (const byte*)ptr_inputData, (word32)dataLen);
                break;
            default:
                ret_aesStatus_En = CRYPTO_SYM_ERROR_OPMODE;
                break;
        } //end of switch
            
        if(wcAesStatus == 0)
        {
            ret_aesStatus_En = CRYPTO_SYM_CIPHER_SUCCESS;
        }
        else if(ret_aesStatus_En != CRYPTO_SYM_ERROR_OPMODE)
        {
            if(wcAesStatus == BAD_FUNC_ARG)
            {
                ret_aesStatus_En = CRYPTO_SYM_ERROR_ARG;
            }
            else
            {
                ret_aesStatus_En  = CRYPTO_SYM_ERROR_CIPFAIL;
            }
        }
        else
        {
            //do nothing
        }
    } //end of if of argument checking
    
    return ret_aesStatus_En;
}
	
crypto_Sym_Status_E Crypto_Sym_Wc_Aes_EncryptDirect(crypto_Sym_OpModes_E symAlgoMode_en, uint8_t *ptr_inputData, uint32_t dataLen, uint8_t *ptr_outData,
                                                        uint8_t *ptr_key, uint32_t keySize, uint8_t *ptr_initVect)
{
    crypto_Sym_Status_E ret_aesStat_en = CRYPTO_SYM_ERROR_CIPNOTSUPPTD;    
   
    int wcAesStatus = BAD_FUNC_ARG;
    {
        Aes aesCtx[1];
        wcAesStatus = wc_AesSetKey(aesCtx, (const byte*)ptr_key, (word32)keySize, (const byte*)ptr_initVect, AES_ENCRYPTION);
        if(wcAesStatus == 0)
        {
            switch(symAlgoMode_en)
            {
                case CRYPTO_SYM_OPMODE_ECB:
                    wcAesStatus = wc_AesEcbEncrypt(aesCtx, (byte*)ptr_outData, (const byte*)ptr_inputData, (word32)dataLen);
                    break;
                case CRYPTO_SYM_OPMODE_CTR:
                    wcAesStatus = wc_AesCtrEncrypt(aesCtx, (byte*)ptr_outData, (const byte*)ptr_inputData, (word32)dataLen);
                    break;
                default:
                    ret_aesStat_en = CRYPTO_SYM_ERROR_OPMODE;
                    break;
            } //end of switch
        }
    }
    if(wcAesStatus == 0)
    {
        ret_aesStat_en = CRYPTO_SYM_CIPHER_SUCCESS;
    }
    else if(ret_aesStat_en == CRYPTO_SYM_ERROR_OPMODE)
    {
        //do nothing
    }
    else
    {
        if(wcAesStatus == BAD_FUNC_ARG)
        {
            ret_aesStat_en = CRYPTO_SYM_ERROR_ARG;
        }
        else
        {
            ret_aesStat_en  = CRYPTO_SYM_ERROR_CIPFAIL;
        }
    }
    return ret_aesStat_en;
}
        
crypto_Sym_Status_E Crypto_Sym_Wc_Aes_DecryptDirect(crypto_Sym_OpModes_E symAlgoMode_en, uint8_t *ptr_inputData, uint32_t dataLen, uint8_t *ptr_outData,
                                                        uint8_t *ptr_key, uint32_t keySize, uint8_t *ptr_initVect)
{
    crypto_Sym_Status_E ret_aesStat_en = CRYPTO_SYM_ERROR_CIPNOTSUPPTD;    
  
    int wcAesStatus = BAD_FUNC_ARG;
    if( (ptr_inputData != NULL) && (dataLen > 0u) && (ptr_outData != NULL) && (ptr_key != NULL) && (keySize > 0u) )
    {
        {
            Aes aesCtx[1];
			if(symAlgoMode_en == CRYPTO_SYM_OPMODE_CTR)
			{
            	wcAesStatus = wc_AesSetKey(aesCtx, (const byte*)ptr_key, (word32)keySize, (const byte*)ptr_initVect, AES_ENCRYPTION);
			}
			else
			{
				wcAesStatus = wc_AesSetKey(aesCtx, (const byte*)ptr_key, (word32)keySize, (const byte*)ptr_initVect, AES_DECRYPTION);
			}
			
            if(wcAesStatus == 0)
            {
                switch(symAlgoMode_en)
                {
                    case CRYPTO_SYM_OPMODE_ECB:
                        wcAesStatus = wc_AesEcbDecrypt(aesCtx, (byte*)ptr_outData, (const byte*)ptr_inputData, (word32)dataLen);
                        break;
                    case CRYPTO_SYM_OPMODE_CTR:
                        wcAesStatus = wc_AesCtrEncrypt(aesCtx, (byte*)ptr_outData, (const byte*)ptr_inputData, (word32)dataLen);
                        break;
                    default:
                        ret_aesStat_en = CRYPTO_SYM_ERROR_OPMODE;
                        break;
                } //end of switch
            }
        }
        if(wcAesStatus == 0)
        {
            ret_aesStat_en = CRYPTO_SYM_CIPHER_SUCCESS;
        }
        else if(ret_aesStat_en != CRYPTO_SYM_ERROR_OPMODE)
        {
            if(wcAesStatus == BAD_FUNC_ARG)
            {
                ret_aesStat_en = CRYPTO_SYM_ERROR_ARG;
            }
            else
            {
                ret_aesStat_en  = CRYPTO_SYM_ERROR_CIPFAIL;
            }
        }
        else
        {
         //do nothing   
        }
    } //end of if of argument checking
    else
    {
        ret_aesStat_en = CRYPTO_SYM_ERROR_ARG;
    }
    return ret_aesStat_en;
}
