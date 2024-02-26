/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    MCHP_Crypto_DigSign.c

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
#include "crypto/common_crypto/MCHP_Crypto_DigSign.h"
#include "crypto/common_crypto/MCHP_Crypto_Common.h"
#include "crypto/common_crypto/MCHP_Crypto_DigSign_Config.h"

#ifdef CRYPTO_DIGISIGN_WC_ALGO_EN 
#include "crypto/common_crypto/MCHP_Crypto_DigSign_WolfcryptWrapper.h"
#endif /* CRYPTO_DIGISIGN_WC_ALGO_EN */ 


//const st_Crypto_EccCurveData EccCurveData_st[CRYPTO_ECC_CURVE_MAX] =  {  
//                                                                            {CRYPTO_ECC_CURVE_SECP256R1,        32, 64},
//                                                                            {CRYPTO_ECC_CURVE_SECP384R1,        48, 96},
//                                                                            {CRYPTO_ECC_CURVE_SECP256K1,        32, 64},
//                                                                            {CRYPTO_ECC_CURVE_BRAINPOOLP256R1,  32, 64},
//                                                                            {CRYPTO_ECC_CURVE_BRAINPOOLP384R1,  48, 96},
//                                                                        };


#ifdef CRYPTO_DIGISIGN_ECDSA_EN
crypto_DigiSign_Status_E Crypto_DigiSign_Ecdsa_Sign(crypto_HandlerType_E ecdsaHandlerType_en, uint8_t *ptr_inputHash, uint32_t hashLen, uint8_t *ptr_outSig, 
                                                    uint32_t sigLen, uint8_t *ptr_privKey, uint32_t privKeyLen, crypto_EccCurveType_E eccCurveType_En, uint32_t ecdsaSessionId)
{
    crypto_DigiSign_Status_E ret_ecdsaStat_en = CRYPTO_DIGISIGN_ERROR_ALGONOTSUPPTD;
    
    if( (ptr_inputHash == NULL) || (hashLen == 0u) )
    {
        ret_ecdsaStat_en = CRYPTO_DIGISIGN_ERROR_INPUTHASH;
    }
    else if( (ptr_outSig == NULL) || (sigLen == 0u) )
    {
         ret_ecdsaStat_en = CRYPTO_DIGISIGN_ERROR_SIGNATURE;
    }
    else if( (eccCurveType_En <= CRYPTO_ECC_CURVE_INVALID) || (eccCurveType_En >= CRYPTO_ECC_CURVE_MAX) )
    {
         ret_ecdsaStat_en = CRYPTO_DIGISIGN_ERROR_FAIL;
    }
    else if(ptr_privKey == NULL) 
    {
        ret_ecdsaStat_en = CRYPTO_DIGISIGN_ERROR_PRIVKEY;
    }
    else if(privKeyLen <= 0u)
//    else if( (privKeyLen <= 0) || (privKeyLen != (EccCurveData_st[eccCurveType_En].curvePrivKeyLen)) )
//    //else if( (privKeyLen <= 0) || (privKeyLen != (arr_EccCurveWcMap2[eccCurveType_En][1])) )
//    //else if( (privKeyLen <= 0) || (privKeyLen > CRYPTO_ECC_MAX_KEY_LENGTH) )
    {
         ret_ecdsaStat_en = CRYPTO_DIGISIGN_ERROR_PRIVKEY;
    }
    else if((ecdsaSessionId <= 0u) || (ecdsaSessionId > (uint32_t)CRYPTO_DIGISIGN_SESSION_MAX) )
    {
        ret_ecdsaStat_en = CRYPTO_DIGISIGN_ERROR_SID;
    }
    else
    {
        switch(ecdsaHandlerType_en)
        {
#ifdef CRYPTO_DIGISIGN_WC_ECDSA_EN           
            case CRYPTO_HANDLER_SW_WOLFCRYPT:
                ret_ecdsaStat_en = Crypto_DigiSign_Wc_Ecdsa_Sign(ptr_inputHash, hashLen, ptr_outSig, sigLen, ptr_privKey, privKeyLen, eccCurveType_En);
            break; 
#endif /* CRYPTO_DIGISIGN_WC_ECDSA_EN */           
            case CRYPTO_HANDLER_HW_INTERNAL:

            break;
            default:
                ret_ecdsaStat_en = CRYPTO_DIGISIGN_ERROR_HDLR;
            break;
        }
    }

    return ret_ecdsaStat_en;
}


crypto_DigiSign_Status_E Crypto_DigiSign_Ecdsa_Verify(crypto_HandlerType_E ecdsaHandlerType_en, uint8_t *ptr_inputHash, uint32_t hashLen, uint8_t *ptr_inputSig, uint32_t sigLen, 
                                                    uint8_t *ptr_pubKey, uint32_t pubKeyLen, int8_t *ptr_hashVerifyStat, crypto_EccCurveType_E eccCurveType_En, uint32_t ecdsaSessionId)
{
    crypto_DigiSign_Status_E ret_ecdsaStat_en = CRYPTO_DIGISIGN_ERROR_ALGONOTSUPPTD;
    
    if( (ptr_inputHash == NULL) || (hashLen == 0u) )
    {
        ret_ecdsaStat_en = CRYPTO_DIGISIGN_ERROR_INPUTHASH;
    }
    else if( (ptr_inputSig == NULL) || (sigLen == 0u) )
    {
         ret_ecdsaStat_en = CRYPTO_DIGISIGN_ERROR_SIGNATURE;
    }
    else if( (eccCurveType_En <= CRYPTO_ECC_CURVE_INVALID) || (eccCurveType_En >= CRYPTO_ECC_CURVE_MAX) )
    {
         ret_ecdsaStat_en = CRYPTO_DIGISIGN_ERROR_CURVE;
    }
    else if( (ptr_pubKey == NULL) || (pubKeyLen <= 0u) )
    {
        ret_ecdsaStat_en = CRYPTO_DIGISIGN_ERROR_PUBKEY;
    }
    //Check the Key compression Type, 0x04 for uncompressed, 0x02 for Even compressed and 0x03 for Odd compressed
    else if( !( (ptr_pubKey[0] == 0x04u) || ((ptr_pubKey[0] == 0x02u) || (ptr_pubKey[0] == 0x03u)) ) )
    {
        ret_ecdsaStat_en = CRYPTO_DIGISIGN_ERROR_PUBKEYCOMPRESS;
    }
//    else if( ( (ptr_pubKey[0] == 0x04) && (pubKeyLen != (EccCurveData_st[eccCurveType_En].curveMaxPubKeyLen + 1) ) ) //when Pub key is uncompressed
//             || ( (ptr_pubKey[0] != 0x04) && (pubKeyLen != ((EccCurveData_st[eccCurveType_En].curveMaxPubKeyLen/2) + 1)) ) ) //when Pub key is even or Odd compressed 
//    {
//         ret_ecdsaStat_en = CRYPTO_DIGISIGN_ERROR_PUBKEYLEN;
//    }
    else if((ecdsaSessionId <= 0u) || (ecdsaSessionId > (uint32_t)CRYPTO_DIGISIGN_SESSION_MAX) )
    {
        ret_ecdsaStat_en = CRYPTO_DIGISIGN_ERROR_SID;
    }
    else
    {
        switch(ecdsaHandlerType_en)
        {
#ifdef CRYPTO_DIGISIGN_WC_ECDSA_EN           
            case CRYPTO_HANDLER_SW_WOLFCRYPT:
                ret_ecdsaStat_en = Crypto_DigiSign_Wc_Ecdsa_Verify(ptr_inputHash, hashLen, ptr_inputSig, sigLen, ptr_pubKey, pubKeyLen, ptr_hashVerifyStat, eccCurveType_En);
            break; 
#endif /* CRYPTO_DIGISIGN_WC_ECDSA_EN */           
            case CRYPTO_HANDLER_HW_INTERNAL:

            break;
            default:
                ret_ecdsaStat_en = CRYPTO_DIGISIGN_ERROR_HDLR;
            break;
        }
    }

    return ret_ecdsaStat_en;
}
#endif /* CRYPTO_DIGISIGN_ECDSA_EN */