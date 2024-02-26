/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    MCHP_Crypto_Aead_Cipher.c

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
#include "crypto/common_crypto/MCHP_Crypto_Aead_Config.h"
#include "crypto/common_crypto/MCHP_Crypto_Aead_Cipher.h"
#include "crypto/common_crypto/MCHP_Crypto_Aead_WolfcryptWrapper.h"

#include "wolfssl/wolfcrypt/error-crypt.h"

#ifndef NO_AES
#include "wolfssl/wolfcrypt/aes.h"
#endif /* NO_AES */
// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

#if (!defined(NO_AES))
#if (defined(HAVE_AESCCM))
crypto_Aead_Status_E Crypto_Aead_Wc_AesCcm_Init(void *ptr_aesCcmCtx, uint8_t *ptr_key, uint32_t keySize)
{
    crypto_Aead_Status_E ret_aesCcmStat_en = CRYPTO_AEAD_ERROR_CIPNOTSUPPTD;
    int wcAesCcmStatus = BAD_FUNC_ARG;
    
    if(ptr_aesCcmCtx != NULL)
    {
        wcAesCcmStatus = wc_AesCcmSetKey( (Aes*)ptr_aesCcmCtx, (const byte*)ptr_key, (word32)keySize);

        if(wcAesCcmStatus == 0)
        {
            ret_aesCcmStat_en = CRYPTO_AEAD_CIPHER_SUCCESS;
        }
        else if (wcAesCcmStatus == WC_KEY_SIZE_E)
        {
            ret_aesCcmStat_en = CRYPTO_AEAD_ERROR_KEY;
        }
        else if(wcAesCcmStatus == BAD_FUNC_ARG)
        {
            ret_aesCcmStat_en = CRYPTO_AEAD_ERROR_ARG;
        }
        else
        {
            ret_aesCcmStat_en  = CRYPTO_AEAD_ERROR_CIPFAIL;
        }
    }
    else
    {
        ret_aesCcmStat_en = CRYPTO_AEAD_ERROR_CTX;
    }
    return ret_aesCcmStat_en;
}

crypto_Aead_Status_E Crypto_Aead_Wc_AesCcm_Cipher(crypto_CipherOper_E cipherOper_en, void *ptr_aesCcmCtx, uint8_t *ptr_inputData, uint32_t dataLen, 
                                                    uint8_t *ptr_outData, uint8_t *ptr_nonce, uint32_t nonceLen, uint8_t *ptr_authTag,
                                                    uint32_t authTagLen, uint8_t *ptr_aad, uint32_t aadLen)
{
    crypto_Aead_Status_E ret_aesCcmStat_en = CRYPTO_AEAD_ERROR_CIPNOTSUPPTD;
    int wcAesCcmStatus = BAD_FUNC_ARG;
  
    if( (ptr_aesCcmCtx != NULL ) && (ptr_inputData != NULL) && (ptr_outData != NULL)
            &&(ptr_nonce != NULL) && (nonceLen >= 7u) && (nonceLen <= 13u)
            && (ptr_authTag != NULL) && (authTagLen >= 4u) && (authTagLen <= 16u) ) //aad may be empty as per NIST 800-38C and also AAD is optional
    {
        if( cipherOper_en == CRYPTO_CIOP_ENCRYPT)
        {
            wcAesCcmStatus = wc_AesCcmEncrypt(ptr_aesCcmCtx, ptr_outData, (const byte*)ptr_inputData, dataLen, 
                                                (const byte*)ptr_nonce, nonceLen, ptr_authTag, authTagLen, (const byte*)ptr_aad, aadLen);
        }
        else if(cipherOper_en == CRYPTO_CIOP_DECRYPT)
        {
            wcAesCcmStatus = wc_AesCcmDecrypt(ptr_aesCcmCtx, ptr_outData, (const byte*)ptr_inputData, dataLen, 
                                                (const byte*)ptr_nonce, nonceLen, (const byte*)ptr_authTag, authTagLen, (const byte*)ptr_aad, aadLen);
        }
        else
        {
            ret_aesCcmStat_en = CRYPTO_AEAD_ERROR_CIPOPER;
        }
        if(wcAesCcmStatus == 0)
        {
            ret_aesCcmStat_en = CRYPTO_AEAD_CIPHER_SUCCESS;
        }
        else if(ret_aesCcmStat_en == CRYPTO_AEAD_ERROR_CIPOPER)
        {
            //do nothing
        }
        else
        {
            if(wcAesCcmStatus == BAD_FUNC_ARG)
            {
                ret_aesCcmStat_en = CRYPTO_AEAD_ERROR_ARG;
            }
            else
            {
                ret_aesCcmStat_en  = CRYPTO_AEAD_ERROR_CIPFAIL;
            }
        }
    } //end of if of argument checking
    else
    {
        ret_aesCcmStat_en = CRYPTO_AEAD_ERROR_ARG;
    }
    return ret_aesCcmStat_en;
}

#endif /* HAVE_AESCCM */

#if (defined(WOLFSSL_AES_EAX))
crypto_Aead_Status_E Crypto_Aead_Wc_AesEax_Init(void *ptr_aesEaxCtx, uint8_t *ptr_key, uint32_t keySize, 
                                                uint8_t *ptr_nonce, uint32_t nonceLen, uint8_t *ptr_aad, uint32_t aadLen)
{
    crypto_Aead_Status_E ret_aesEaxStat_en = CRYPTO_AEAD_ERROR_CIPNOTSUPPTD;
    int wcAesStatus = BAD_FUNC_ARG;
    if(ptr_aesEaxCtx != NULL)
    {
        wcAesStatus = wc_AesEaxInit((AesEax*) ptr_aesEaxCtx, (const byte*) ptr_key, (word32) ptr_key,
                                     (const byte*) ptr_nonce, (word32) nonceLen, (const byte*) ptr_aad, (word32) aadLen);
        if(wcAesStatus == 0)
        {
            ret_aesEaxStat_en = CRYPTO_AEAD_CIPHER_SUCCESS;
        }
        else if (wcAesStatus == WC_KEY_SIZE_E)
        {
            ret_aesEaxStat_en = CRYPTO_AEAD_ERROR_KEY;
        }
        else if(wcAesStatus == BAD_FUNC_ARG)
        {
            ret_aesEaxStat_en = CRYPTO_AEAD_ERROR_ARG;
        }
        else
        {
            ret_aesEaxStat_en  = CRYPTO_AEAD_ERROR_CIPFAIL;
        }
    }
    else
    {
        ret_aesEaxStat_en = CRYPTO_AEAD_ERROR_CTX;
    }
    return ret_aesEaxStat_en;
}

crypto_Aead_Status_E Crypto_Aead_Wc_AesEax_Cipher(crypto_CipherOper_E cipherOper_en, void *ptr_aesEaxCtx, uint8_t *ptr_inputData, uint32_t dataLen, 
                                                    uint8_t *ptr_outData, uint8_t *ptr_aad, uint32_t aadLen)
{
    crypto_Aead_Status_E ret_aesEaxStat_en = CRYPTO_AEAD_ERROR_CIPNOTSUPPTD;
    int wcAesStatus = BAD_FUNC_ARG;
    
    if( (ptr_aesEaxCtx != NULL ) && (ptr_inputData != NULL) && (ptr_outData != NULL) && (dataLen > 0u) )
    {
        if( cipherOper_en == CRYPTO_CIOP_ENCRYPT)
        {
            wcAesStatus = wc_AesEaxEncryptUpdate((AesEax *) ptr_aesEaxCtx, ptr_outData, (const byte *) ptr_inputData, dataLen, 
                                        (const byte*) ptr_aad, (word32) aadLen);
        }
        else if(cipherOper_en == CRYPTO_CIOP_DECRYPT)
        {
            wcAesStatus = wc_AesEaxDecryptUpdate((AesEax *) ptr_aesEaxCtx, ptr_outData, (const byte *) ptr_inputData, dataLen, 
                                        (const byte*) ptr_aad, (word32) aadLen);
        }
        else
        {
            ret_aesEaxStat_en = CRYPTO_AEAD_ERROR_CIPOPER;
        }

        if(wcAesStatus == 0)
        {
            ret_aesEaxStat_en = CRYPTO_AEAD_CIPHER_SUCCESS;
        }
        else if(ret_aesEaxStat_en == CRYPTO_AEAD_ERROR_CIPOPER)
        {
            //do nothing
        }
        else
        {
            if(wcAesStatus == BAD_FUNC_ARG)
            {
                ret_aesEaxStat_en = CRYPTO_AEAD_ERROR_ARG;
            }
            else
            {
                ret_aesEaxStat_en  = CRYPTO_AEAD_ERROR_CIPFAIL;
            }
        }
    }
    else
    {
        ret_aesEaxStat_en = CRYPTO_AEAD_ERROR_ARG;
    }
    return ret_aesEaxStat_en;
}        
    
crypto_Aead_Status_E Crypto_Aead_Wc_AesEax_Final(crypto_CipherOper_E cipherOper_en, void *ptr_aesEaxCtx, uint8_t *ptr_authTag, uint32_t authTagLen)
{
    crypto_Aead_Status_E ret_aesEaxStat_en = CRYPTO_AEAD_ERROR_CIPNOTSUPPTD;
    int wcAesStatus = BAD_FUNC_ARG;
    if( (ptr_aesEaxCtx != NULL) && (authTagLen <= (uint32_t)AES_BLOCK_SIZE) )
    {
        if( cipherOper_en == CRYPTO_CIOP_ENCRYPT)
        {
            wcAesStatus = wc_AesEaxEncryptFinal((AesEax *) ptr_aesEaxCtx, ptr_authTag, authTagLen);
        }
        else if(cipherOper_en == CRYPTO_CIOP_DECRYPT)
        {
            wcAesStatus = wc_AesEaxDecryptFinal((AesEax *) ptr_aesEaxCtx, ptr_authTag, authTagLen);
        }
        else
        {
            ret_aesEaxStat_en = CRYPTO_AEAD_ERROR_CIPOPER;
        }

        if(wcAesStatus == 0)
        {
            ret_aesEaxStat_en = CRYPTO_AEAD_CIPHER_SUCCESS;
        }
        else if(ret_aesEaxStat_en == CRYPTO_AEAD_ERROR_CIPOPER)
        {
           //do nothing 
        }
        else
        {
            if(wcAesStatus == BAD_FUNC_ARG)
            {
                ret_aesEaxStat_en = CRYPTO_AEAD_ERROR_ARG;
            }
            else
            {
                ret_aesEaxStat_en  = CRYPTO_AEAD_ERROR_CIPFAIL;
            }
        }
    }
    else
    {
        ret_aesEaxStat_en = CRYPTO_AEAD_ERROR_ARG;
    }
    return ret_aesEaxStat_en;
}

crypto_Aead_Status_E Crypto_Aead_Wc_AesEax_AddAadData(void *ptr_aesEaxCtx, uint8_t *ptr_aad, uint32_t aadLen)
{
    crypto_Aead_Status_E ret_aesEaxStat_en = CRYPTO_AEAD_ERROR_CIPNOTSUPPTD;
    int wcAesStatus = BAD_FUNC_ARG;
    if(ptr_aesEaxCtx != NULL)
    {
        wcAesStatus = wc_AesEaxAuthDataUpdate((AesEax *)ptr_aesEaxCtx, (const byte *)ptr_aad, aadLen);
        if(wcAesStatus == 0)
        {
            ret_aesEaxStat_en = CRYPTO_AEAD_CIPHER_SUCCESS;
        }
        else if(ret_aesEaxStat_en == CRYPTO_AEAD_ERROR_CIPOPER)
        {
            //Do nothing    
        }
        else
        {
            if(wcAesStatus == BAD_FUNC_ARG)
            {
                ret_aesEaxStat_en = CRYPTO_AEAD_ERROR_ARG;
            }
            else
            {
                ret_aesEaxStat_en  = CRYPTO_AEAD_ERROR_CIPFAIL;
            }
        }
    }
    else
    {
        ret_aesEaxStat_en = CRYPTO_AEAD_ERROR_ARG;
    }
    return ret_aesEaxStat_en;
}

crypto_Aead_Status_E Crypto_Aead_Wc_AesEax_EncDecAuthDirect(crypto_CipherOper_E cipherOper_en, uint8_t *ptr_inputData, uint32_t dataLen, 
                                                            uint8_t *ptr_outData, uint8_t *ptr_key, uint32_t keySize, uint8_t *ptr_nonce, 
                                                            uint32_t nonceLen, uint8_t *ptr_aad, uint32_t aadLen, uint8_t *ptr_authTag, uint32_t authTagLen)
{
    crypto_Aead_Status_E ret_aesEaxStat_en = CRYPTO_AEAD_ERROR_CIPNOTSUPPTD;
    int wcAesEaxStatus = BAD_FUNC_ARG;
    
    if( cipherOper_en == CRYPTO_CIOP_ENCRYPT)
    {
        wcAesEaxStatus = wc_AesEaxEncryptAuth((const byte*)ptr_key, keySize, ptr_outData, (const byte*)ptr_inputData, dataLen,
                                       (const byte*) ptr_nonce, nonceLen, ptr_authTag, authTagLen, ptr_aad, aadLen);
    }
    else if(cipherOper_en == CRYPTO_CIOP_DECRYPT)
    {
        wcAesEaxStatus = wc_AesEaxDecryptAuth((const byte*)ptr_key, keySize, ptr_outData, (const byte*)ptr_inputData, dataLen,
                                       (const byte*) ptr_nonce, nonceLen, ptr_authTag, authTagLen, ptr_aad, aadLen);
    }
    else
    {
        ret_aesEaxStat_en = CRYPTO_AEAD_ERROR_CIPOPER;
    }

    if(wcAesEaxStatus == 0)
    {
        ret_aesEaxStat_en = CRYPTO_AEAD_CIPHER_SUCCESS;
    }
    else if(ret_aesEaxStat_en == CRYPTO_AEAD_ERROR_CIPOPER)
    {
        //do nothing
    }
    else
    {
        if(wcAesEaxStatus == BAD_FUNC_ARG)
        {
            ret_aesEaxStat_en = CRYPTO_AEAD_ERROR_ARG;
        }
        else if(wcAesEaxStatus == AES_EAX_AUTH_E)
        {
            ret_aesEaxStat_en = CRYPTO_AEAD_ERROR_AUTHFAIL;
        }
        else
        {
            ret_aesEaxStat_en  = CRYPTO_AEAD_ERROR_CIPFAIL;
        }
    }
    
    return ret_aesEaxStat_en;
}        
#endif /* WOLFSSL_AES_EAX */

#endif /* !NO_AES  */
// *****************************************************************************