/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    MCHP_Crypto_DigSign_WolfcryptWrapper.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_Initialize" and "APP_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

#ifndef MCHP_CRYPTO_DIGSIGN_WOLFCRYPTWRAPPER_H
#define MCHP_CRYPTO_DIGSIGN_WOLFCRYPTWRAPPER_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#ifdef CRYPTO_DIGISIGN_WC_ECDSA_EN

crypto_DigiSign_Status_E Crypto_DigiSign_Wc_Ecdsa_Sign(uint8_t *ptr_wcInputHash, uint32_t wcHashLen, uint8_t *ptr_wcSig, uint32_t wcSigLen, uint8_t *ptr_wcPrivKey, 
                                                       uint32_t wcPrivKeyLen, crypto_EccCurveType_E wcEccCurveType_en);

crypto_DigiSign_Status_E Crypto_DigiSign_Wc_Ecdsa_Verify(uint8_t *ptr_wcInputHash, uint32_t wcHashLen, uint8_t *ptr_wcInputSig, uint32_t wcSigLen, uint8_t *ptr_wcPubKey, uint32_t wcPubKeyLen, 
                                                        int8_t *ptr_wcHashVerifyStat, crypto_EccCurveType_E wcEccCurveType_en);
#endif /* CRYPTO_DIGISIGN_WC_ECDSA_EN */




#endif /* MCHP_CRYPTO_DIGSIGN_WOLFCRYPTWRAPPER_H */
