/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    MCHP_Crypto_Kas.c

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

#include "crypto/common_crypto/MCHP_Crypto_Common.h"
#include "crypto/common_crypto/MCHP_Crypto_Kas.h"
#include "crypto/common_crypto/MCHP_Crypto_Kas_Config.h"

#ifdef CRYPTO_KAS_WC_ALGO_EN
#include "crypto/common_crypto/MCHP_Crypto_Kas_WolfcryptWrapper.h"
#endif /* CRYPTO_KAS_WC_ALGO_EN */


#ifdef CRYPTO_KAS_ECDH_EN
crypto_Kas_Status_E Crypto_Kas_Ecdh_SharedSecret(crypto_HandlerType_E ecdhHandlerType_en, uint8_t *ptr_privKey, uint32_t privKeyLen, uint8_t *ptr_pubKey, uint32_t pubKeyLen,
                                                    uint8_t *ptr_sharedSecret, uint32_t sharedSecretLen, crypto_EccCurveType_E eccCurveType_en, uint32_t ecdhSessionId)
{
    crypto_Kas_Status_E ret_ecdhStat_en = CRYPTO_KAS_ERROR_ALGONOTSUPPTD;
    
    if( (ptr_pubKey == NULL) || (pubKeyLen == 0u) )
    {
        ret_ecdhStat_en = CRYPTO_KAS_ERROR_PUBKEY;
    }
    else if( (ptr_privKey == NULL) || (privKeyLen <= 0u) || (privKeyLen > (uint32_t)CRYPTO_ECC_MAX_KEY_LENGTH) )
    {
         ret_ecdhStat_en = CRYPTO_KAS_ERROR_PRIVKEY;
    }
    else if( (eccCurveType_en <= CRYPTO_ECC_CURVE_INVALID) || (eccCurveType_en >= CRYPTO_ECC_CURVE_MAX) )
    {
         ret_ecdhStat_en = CRYPTO_KAS_ERROR_CURVE;
    }
    else if((ecdhSessionId <= 0u) || (ecdhSessionId > (uint32_t)CRYPTO_KAS_SESSION_MAX) )
    {
        ret_ecdhStat_en = CRYPTO_KAS_ERROR_SID;
    }
    else
    {
        switch(ecdhHandlerType_en)
        {            
#ifdef CRYPTO_KAS_WC_ECDH_EN           
            case CRYPTO_HANDLER_SW_WOLFCRYPT:
                ret_ecdhStat_en = Crypto_Kas_Wc_Ecdh_SharedSecret(ptr_privKey, privKeyLen, ptr_pubKey, pubKeyLen, ptr_sharedSecret,
                                                                    sharedSecretLen, eccCurveType_en);
            break; 
#endif /* CRYPTO_KAS_WC_ECDH_EN */           
            case CRYPTO_HANDLER_HW_INTERNAL:

            break;
            default:
                ret_ecdhStat_en = CRYPTO_KAS_ERROR_HDLR;
            break;
        }
    }
    
    return ret_ecdhStat_en;
}
#endif /* CRYPTO_KAS_ECDH_EN */
