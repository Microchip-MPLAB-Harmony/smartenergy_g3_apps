/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    crypto_rng_wc_wrapper.c

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
#include "crypto/common_crypto/crypto_common.h"
#include "crypto/common_crypto/crypto_rng.h"
#include "crypto/wolfcrypt/crypto_rng_wc_wrapper.h"
#include "wolfssl/wolfcrypt/random.h"
#include "wolfssl/wolfcrypt/error-crypt.h"

crypto_Rng_Status_E Crypto_Rng_Wc_Prng_GenerateBlock(uint8_t* ptr_rngData, uint32_t rngLen, uint8_t* ptr_nonce, uint32_t nonceLen)
{
    crypto_Rng_Status_E ret_rngStat_en = CRYPTO_RNG_ERROR_FAIL;
    int wcStat = -1, wcFreeStat = -1;
    WC_RNG rng_st = {0};
        
    wcStat = wc_InitRngNonce(&rng_st, ptr_nonce, nonceLen);
    
    if (wcStat == 0)
    {
        wcStat = wc_RNG_GenerateBlock(&rng_st, ptr_rngData, rngLen);
    }

    wcFreeStat = wc_FreeRng(&rng_st);
    
    if (wcStat == 0 && wcFreeStat == 0)
    {
        ret_rngStat_en = CRYPTO_RNG_SUCCESS;
    }
    else if (wcStat == BAD_FUNC_ARG)
    {
        ret_rngStat_en = CRYPTO_RNG_ERROR_ARG;
    }
    else
    {
        ret_rngStat_en = CRYPTO_RNG_ERROR_FAIL;
    }
    
    return ret_rngStat_en;
}
