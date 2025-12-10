/*******************************************************************************
  Secure Project System Configuration Header

  File Name:
    wolfcrypt_config.h

  Summary:
    Build-time configuration header for the TrustZone secure system defined by 
    this project.

  Description:
    An MPLAB Project may have multiple configurations.  This file defines the
    build-time options for a single configuration.

  Remarks:
    This configuration header must not define any prototypes or data
    definitions (or include any files that do).  It only provides macro
    definitions for build-time configuration options

*******************************************************************************/

/*----------------------------------------------------------------------------
 Copyright (C) 2019-2024 Microchip Technology Inc. and its subsidiaries.

Microchip Technology Inc. and its subsidiaries.

Subject to your compliance with these terms, you may use Microchip software 
and any derivatives exclusively with Microchip products. It is your 
responsibility to comply with third party license terms applicable to your 
use of third party software (including open source software) that may 
accompany Microchip software.

THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR 
PURPOSE.

IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE 
FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN 
ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, 
THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
----------------------------------------------------------------------------*/

#ifndef WOLFCRYPT_CONFIG_H
#define WOLFCRYPT_CONFIG_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

//#include "user.h"
#include "device.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END

/*** Crypto Library Configuration ***/

//Crypto V4 Common Crypto API - WolfCrypt Library Support
#define CRYPTO_WOLFCRYPT_SUPPORT_ENABLE

//JK
#define CRYPTO_DIGISIGN_ALGO_EN
#define CRYPTO_KAS_ALGO_EN    

/*** wolfCrypt Library Configuration ***/
//configuration.h included in config.h and user_settings.h allowing these
//files to configure WolfSSL library
//--WolfSSL project defines to use these files are WOLFSSL_USER_SETTINGS and
//  HAVE_CONFIG_H

// ---------- WOLFCRYPT FUNCTIONAL CONFIGURATION START ----------

#define MICROCHIP_PIC32
#define MICROCHIP_MPLAB_HARMONY
#define MICROCHIP_MPLAB_HARMONY_3
#define HAVE_MCAPI 

#define SIZEOF_LONG_LONG 8
#define WOLFSSL_USER_IO
#define NO_WRITEV
#define NO_FILESYSTEM
#define USE_FAST_MATH  //Conflicts with WOLFSSL_SP_MATH
#define NO_INLINE 

//Math Model Defines
//Using this to declare wolfmath functions:
//mp_reverse used by fp_to_unsigned_bin (tfm.c)unctions: 
//get_digit_count,get_digit in ecc.c 
#define WOLFSSL_SP_MATH   

//Needed when WOFSSL_SP_MATH defined
//#define WOLFSSL_HAVE_SP_RSA  
//#define WOLFSSL_HAVE_SP_DH

//To prevent usage of SP functions defined by this: 
//#if defined(HAVE_FIPS_VERSION) && HAVE_FIPS_VERSION == 2 
//&& ( defined(WOLFSSL_SP_ARM32_ASM) || defined(WOLFSSL_SP_ARM64_ASM) ) 
//#define WOLFSSL_SP_NO_256   
#define WOLFSSL_SP_384   

//#define WOLFSSL_SP_ARM32_ASM   //Dependent on MCU Arch.

#define WOLFSSL_SP_MATH_ALL   //JK
#define WOLFSSL_HAVE_SP_ECC  //JK

#define WOLFSSL_SP_NO_MALLOC //JK
#define WOLFSSL_SP_SMALL     //JK
#define WOLFSSL_SP_NONBLOCK  //JK
#define WC_ECC_NONBLOCK      //JK

#define NO_PWDBASED
//#define WOLF_CRYPTO_CB     //Provide call-back support 
#define WOLFCRYPT_ONLY
#define HAVE_COMP_KEY        //JK 

#define WOLFSSL_AES_SMALL_TABLES
#define NO_MD4
#define WOLFSSL_SHA224
#define WOLFSSL_SHA384
#define WOLFSSL_SHA512
#define HAVE_SHA512
#define WOLFSSL_SHA3
#define HAVE_BLAKE2
#define WOLFSSL_RIPEMD
//#define NO_HMAC            //JK
#define WOLFSSL_AES_128
#define WOLFSSL_AES_192
#define WOLFSSL_AES_256
#define WOLFSSL_AES_DIRECT
#define HAVE_AES_DECRYPT
#define HAVE_AES_ECB
#define HAVE_AES_CBC
#define WOLFSSL_AES_COUNTER
#define WOLFSSL_AES_CFB
#define WOLFSSL_AES_OFB
#define HAVE_AESGCM
#define WOLFSSL_AES_XTS
#define HAVE_AESCCM
#define WOLFSSL_CMAC
#define WOLFSSL_DES_ECB
#define HAVE_CAMELLIA
#define HAVE_IDEA
#define HAVE_HC128
#define HAVE_RABBIT

#define HAVE_ECC
//#define HAVE_ECC_KEY_EXPORT //JK //Required for WOLFCRYPT_HAVE_ECCSI
#define HAVE_X963_KDF
#define ECC_SHAMIR
#define WOLFSSL_CUSTOM_CURVES
//#define HAVE_CURVE25519     //JK 
//#define HAVE_ED25519        //JK
#define FP_ECC
#define HAVE_ECC521           //ECC_MAX_BITS

//#define HAVE_DH
//#define WOLFSSL_HAVE_SP_DH
    
#define HAVE_ANON
#define WOLFSSL_OLD_PRIME_CHECK
#define NO_DSA
#define FP_MAX_BITS 4096
#define USE_CERT_BUFFERS_2048
//#define WOLFSSL_RSA_PUBLIC_ONLY  //JK //Conflict with WOLFSSL_HAVE_SP_DH
//#define WC_RSA_PSS               //JK
//#define WOLFSSL_STATIC_RSA       //JK
#define NO_DEV_RANDOM
#define HAVE_HASHDRBG
#define WC_NO_HARDEN
#define SINGLE_THREADED
#define NO_ERROR_STRINGS
#define NO_WOLFSSL_MEMORY

//********************************************************
//#define WOLFSSL_SHAKE128  //Manually Commented, Not supported by Wolfcrypt
#define WOLFSSL_SHAKE256 
#define HAVE_BLAKE2S    
#define HAVE_BLAKE2B   
#define HAVE_CHACHA      
#define HAVE_AES_KEYWRAP
#define HAVE_ECC_DHE   
//#define WOLFCRYPT_HAVE_ECCSI  //JK
#define WOLFSSL_AES_EAX 
#define WOLFSSL_AESGCM_STREAM
//*********************************************************
int Crypto_Rng_Wc_Prng_EntropySource(void); //User-modifiable entropy for PRNG 
int Crypto_Rng_Wc_Prng_Srand(uint8_t* output, unsigned int sz);

//*********************************************************
#define CUSTOM_RAND_GENERATE_SEED Crypto_Rng_Wc_Prng_Srand
//#define WC_NO_RNG //when using RNG
//#define WC_NO_HASHDRBG
//#define HAVE_ECC_ENCRYPT
#define NO_RSA                 //JK
#define NO_DH                  //JK
//#define NO_SIG_WRAPPER
//**********************************************************

// ---------- WOLFCRYPT CONFIGURATION END ----------

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // WOLFCRYPT_CONFIG_H
/*******************************************************************************
 End of File
*/
