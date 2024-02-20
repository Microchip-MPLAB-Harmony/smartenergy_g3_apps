/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    MCHP_Crypto_Aead_Config.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_Initialize" and "APP_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

#ifndef MCHP_CRYPTO_AEAD_CONFIG_H
#define MCHP_CRYPTO_AEAD_CONFIG_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: System Configuration
// *****************************************************************************
// *****************************************************************************



// *****************************************************************************
// *****************************************************************************
// Section: System Service Configuration
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: Driver Configuration
// *****************************************************************************
// *****************************************************************************
#define CRYPTO_AEAD_SESSION_MAX (1)

// *****************************************************************************
// *****************************************************************************
// Section: Middleware & Other Library Configuration
// *****************************************************************************
// *****************************************************************************
//MCHP Configurations Macros
    

//*****OPERATION MODE ENABLE MACROS************

//AEAD SW/HW Mode
#define CRYPTO_AEAD_WC_ALGO_EN

//------------------------------------------------------------------------------
//AES Algorithms Operational Mode Macros
#define CRYPTO_AEAD_AESCCM_EN

#define CRYPTO_AEAD_AESEAX_EN

//****WolfcryptWrapper Algorithm Macros*************
#define CRYPTO_AEAD_WC_AESCCM_EN

#define CRYPTO_AEAD_WC_AESEAX_EN

#endif //MCHP_CRYPTO_AEAD_CONFIG_H

