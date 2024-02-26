/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    MCHP_Crypto_RNG_WolfcryptWrapper.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_Initialize" and "APP_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/
#ifndef MCHP_CRYPTO_RNG_WOLFCRYPTWRAPPER_H
#define MCHP_CRYPTO_RNG_WOLFCRYPTWRAPPER_H

#ifdef CRYPTO_RNG_WC_PRNG_EN
crypto_Rng_Status_E Crypto_Rng_Wc_Prng_GenerateBlock(uint8_t* ptr_rngData, uint32_t rngLen, uint8_t* ptr_nonce, uint32_t nonceLen);
#endif /* CRYPTO_RNG_WC_PRNG_EN */     

#endif /* MCHP_CRYPTO_RNG_WOLFCRYPTWRAPPER_H */
