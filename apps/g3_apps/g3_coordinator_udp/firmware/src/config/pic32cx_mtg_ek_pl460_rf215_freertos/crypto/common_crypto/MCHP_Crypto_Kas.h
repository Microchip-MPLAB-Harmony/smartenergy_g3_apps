/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    MCHP_Crypto_Kas.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_Initialize" and "APP_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

#ifndef MCHP_CRYPTO_KAS_H
#define MCHP_CRYPTO_KAS_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "MCHP_Crypto_Common.h"
#include "MCHP_Crypto_Kas_Config.h"
// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
typedef enum
{
    CRYPTO_KAS_ERROR_ALGONOTSUPPTD = -127,
    CRYPTO_KAS_ERROR_PUBKEY = -126,
    CRYPTO_KAS_ERROR_PRIVKEY = -125,
    CRYPTO_KAS_ERROR_HDLR = -124,
    CRYPTO_KAS_ERROR_SID = -123,  ////session ID Error
    CRYPTO_KAS_ERROR_CURVE = -122,
    CRYPTO_KAS_ERROR_ARG = -121,
    CRYPTO_KAS_ERROR_FAIL = -120,
    CRYPTO_KAS_SUCCESS = 0,        
}crypto_Kas_Status_E;

#ifdef CRYPTO_KAS_ECDH_EN
crypto_Kas_Status_E Crypto_Kas_Ecdh_SharedSecret(crypto_HandlerType_E ecdhHandlerType_en, uint8_t *ptr_privKey, uint32_t privKeyLen, uint8_t *ptr_pubKey, uint32_t pubKeyLen,
                                                    uint8_t *ptr_sharedSecret, uint32_t sharedSecretLen, crypto_EccCurveType_E eccCurveType_en, uint32_t ecdhSessionId);
#endif /* CRYPTO_KAS_ECDH_EN */

#endif /* MCHP_CRYPTO_KAS_H */