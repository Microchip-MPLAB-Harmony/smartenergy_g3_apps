/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    MCHP_Crypto_RNG.c

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
#include "crypto/common_crypto/MCHP_Crypto_RNG.h"
#include "crypto/common_crypto/MCHP_Crypto_RNG_Config.h"

#ifdef CRYPTO_RNG_WC_ALGO_EN
#include "crypto/common_crypto/MCHP_Crypto_RNG_WolfcryptWrapper.h"
#endif /* CRYPTO_RNG_WC_ALGO_EN */

crypto_Rng_Status_E Crypto_Rng_Prng_Generate(crypto_HandlerType_E rngHandlerType_en, uint8_t* ptr_rngData, uint32_t rngLen, uint8_t* ptr_nonce, uint32_t nonceLen, uint32_t sessionID)
{
    crypto_Rng_Status_E ret_rngStat_en = CRYPTO_RNG_ERROR_NOTSUPPTED;
    
    if(ptr_rngData == NULL || rngLen <= 0u)
    {
        ret_rngStat_en = CRYPTO_RNG_ERROR_ARG;
    }
    else if( ((ptr_nonce == NULL) && (nonceLen  > 0u))
             || ((ptr_nonce != NULL) && (nonceLen <= 0u)) )
    {
        ret_rngStat_en = CRYPTO_RNG_ERROR_NONCE;
    }
    else if( (sessionID <= 0u) || (sessionID > (uint32_t)CRYPTO_RNG_SESSION_MAX))
    {
        ret_rngStat_en = CRYPTO_RNG_ERROR_SID;
    }
    else
    {
        switch(rngHandlerType_en)
        {
#ifdef CRYPTO_RNG_WC_PRNG_EN           
            case CRYPTO_HANDLER_SW_WOLFCRYPT:
                ret_rngStat_en = Crypto_Rng_Wc_Prng_GenerateBlock(ptr_rngData, rngLen, ptr_nonce, nonceLen);
            break; 
#endif /* CRYPTO_RNG_WC_PRNG_EN */           
            case CRYPTO_HANDLER_HW_INTERNAL:

            break;
            default:
                ret_rngStat_en = CRYPTO_RNG_ERROR_HDLR;
            break;
        }
    }
        
    return ret_rngStat_en;
}

