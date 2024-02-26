/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    MCHP_Crypto_RNG.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_Initialize" and "APP_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/
#ifndef MCHP_CRYPTO_RNG_H
#define MCHP_CRYPTO_RNG_H

#include "MCHP_Crypto_Common.h"
#include "MCHP_Crypto_RNG_Config.h"

typedef enum
{
    CRYPTO_RNG_ERROR_NOTSUPPTED = -127,
    CRYPTO_RNG_ERROR_SEED       = -126,
    CRYPTO_RNG_ERROR_NONCE      = -125,
    CRYPTO_RNG_ERROR_ARG        = -124,
    CRYPTO_RNG_ERROR_SID        = -123,
    CRYPTO_RNG_ERROR_HDLR       = -122,
    CRYPTO_RNG_ERROR_FAIL       = -121,
    CRYPTO_RNG_SUCCESS = 0,   
}crypto_Rng_Status_E;


crypto_Rng_Status_E Crypto_Rng_Prng_Generate(crypto_HandlerType_E rngHandlerType_en, uint8_t* ptr_rngData, uint32_t rngLen, uint8_t* ptr_nonce, uint32_t nonceLen, uint32_t sessionID);

#endif /* MCHP_CRYPTO_RNG_H */
