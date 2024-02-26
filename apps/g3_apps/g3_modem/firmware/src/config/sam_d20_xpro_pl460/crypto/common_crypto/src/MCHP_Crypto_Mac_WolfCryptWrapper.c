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
#include "crypto/common_crypto/MCHP_Crypto_Mac_WolfcryptWrapper.h"

#include "wolfssl/wolfcrypt/error-crypt.h"

#if !defined(NO_AES) && defined(WOLFSSL_CMAC)
#include "wolfssl/wolfcrypt/cmac.h"
#endif /* !NO_AES && WOLFSSL_CMAC */
// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************



#if !defined(NO_AES) && defined(WOLFSSL_CMAC)

crypto_Mac_Status_E Crypto_Mac_Wc_AesCmac_Init(void *ptr_aesCmacCtx, uint8_t *ptr_key, uint32_t keySize)
{
    crypto_Mac_Status_E ret_aesStat_en = CRYPTO_MAC_ERROR_CIPNOTSUPPTD;
    int wcAesStatus = BAD_FUNC_ARG;
    
    if(ptr_aesCmacCtx != NULL)
    {
        wcAesStatus =  wc_InitCmac( (Cmac*) ptr_aesCmacCtx, (const byte*)ptr_key, (word32)keySize, (int)WC_CMAC_AES, NULL);
        
        if(wcAesStatus == 0)
        {
            ret_aesStat_en = CRYPTO_MAC_CIPHER_SUCCESS;
        }
        else if(wcAesStatus == BAD_FUNC_ARG)
        {
            ret_aesStat_en = CRYPTO_MAC_ERROR_ARG;
        }
        else
        {
            ret_aesStat_en  = CRYPTO_MAC_ERROR_CIPFAIL;
        }
    }
    else
    {
        ret_aesStat_en = CRYPTO_MAC_ERROR_CTX;
    }
    return ret_aesStat_en;
}

crypto_Mac_Status_E Crypto_Mac_Wc_AesCmac_Cipher(void *ptr_aesCmacCtx, uint8_t *ptr_inputData, uint32_t dataLen)
{
    crypto_Mac_Status_E ret_aesStat_en = CRYPTO_MAC_ERROR_CIPNOTSUPPTD;
    int wcAesStatus = BAD_FUNC_ARG;
  
    if( (ptr_aesCmacCtx != NULL ) && (ptr_inputData != NULL) )
    {
        wcAesStatus = wc_CmacUpdate(ptr_aesCmacCtx, (const byte*)ptr_inputData, (word32)dataLen);
        
        if(wcAesStatus == 0)
        {
            ret_aesStat_en = CRYPTO_MAC_CIPHER_SUCCESS;
        }
        else if(wcAesStatus == BAD_FUNC_ARG)
        {
            ret_aesStat_en = CRYPTO_MAC_ERROR_ARG;
        }
        else
        {
            ret_aesStat_en  = CRYPTO_MAC_ERROR_CIPFAIL;
        }
    } //end of if of argument checking
    else
    {
        ret_aesStat_en = CRYPTO_MAC_ERROR_ARG;
    }
    return ret_aesStat_en;
}

crypto_Mac_Status_E Crypto_Mac_Wc_AesCmac_Final(void *ptr_aesCmacCtx, uint8_t *ptr_outMac, uint32_t macLen)
{
    crypto_Mac_Status_E ret_aesStat_en = CRYPTO_MAC_ERROR_CIPNOTSUPPTD;
    int wcAesStatus = BAD_FUNC_ARG;
    word32 wcMacLen = macLen;
    if( (ptr_aesCmacCtx != NULL ) && (ptr_outMac != NULL) )
    {
        wcAesStatus = wc_CmacFinal( (Cmac*)ptr_aesCmacCtx, (byte*)ptr_outMac, &wcMacLen);
        if(wcAesStatus == 0)
        {
            ret_aesStat_en = CRYPTO_MAC_CIPHER_SUCCESS;
        }
        else if(wcAesStatus == BAD_FUNC_ARG)
        {
            ret_aesStat_en = CRYPTO_MAC_ERROR_ARG;
        }
        else
        {
            ret_aesStat_en  = CRYPTO_MAC_ERROR_CIPFAIL;
        }
    } //end of if of argument checking
    else
    {
        ret_aesStat_en = CRYPTO_MAC_ERROR_ARG;
    }
    return ret_aesStat_en;
}

crypto_Mac_Status_E Crypto_Mac_Wc_AesCmac_Direct(uint8_t *ptr_inputData, uint32_t inuptLen, uint8_t *ptr_outMac, uint32_t macLen, uint8_t *ptr_key, uint32_t keyLen)
{
    crypto_Mac_Status_E ret_aesStat_en = CRYPTO_MAC_ERROR_CIPNOTSUPPTD;
    int wcAesStatus = BAD_FUNC_ARG;
    word32 wcMacLen = macLen;
    if( (ptr_inputData != NULL ) && (ptr_outMac != NULL) )
    {
        wcAesStatus = wc_AesCmacGenerate( (byte*)ptr_outMac, &wcMacLen, (const byte*)ptr_inputData, inuptLen, (const byte*)ptr_key, keyLen);
        
        if(wcAesStatus == 0)
        {
            ret_aesStat_en = CRYPTO_MAC_CIPHER_SUCCESS;
        }
        else if(wcAesStatus == BAD_FUNC_ARG)
        {
            ret_aesStat_en = CRYPTO_MAC_ERROR_ARG;
        }
        else
        {
            ret_aesStat_en  = CRYPTO_MAC_ERROR_CIPFAIL;
        }
    } //end of if of argument checking
    else
    {
        ret_aesStat_en = CRYPTO_MAC_ERROR_ARG;
    }
    return ret_aesStat_en;  
}
    
#endif /* !NO_AES && WOLFSSL_CMAC */

// *****************************************************************************