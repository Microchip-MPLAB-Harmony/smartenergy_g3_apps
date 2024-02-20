/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    MCHP_Crypto_Kas_WolfcryptWrapper.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_Initialize" and "APP_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

#ifndef MCHP_CRYPTO_KAS_WOLFCRYPTWRAPPER_H
#define MCHP_CRYPTO_KAS_WOLFCRYPTWRAPPER_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "crypto/common_crypto/MCHP_Crypto_Common.h"
#include "crypto/common_crypto/MCHP_Crypto_KAS_Config.h"
// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
#ifdef CRYPTO_KAS_WC_ECDH_EN
crypto_Kas_Status_E Crypto_Kas_Wc_Ecdh_SharedSecret(uint8_t *ptr_wcPrivKey, uint32_t wcPrivKeyLen, uint8_t *ptr_wcPubKey, uint32_t wcPubKeyLen, uint8_t *ptr_wcSharedSecret,
                                                    uint32_t wcSharedSecretLen, crypto_EccCurveType_E wcEccCurveType_en);
#endif /* #ifdef CRYPTO_KAS_WC_ECDH_EN */

#endif /* MCHP_CRYPTO_KAS_WOLFCRYPTWRAPPER_H */