/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    MCHP_Crypto_Mac_Cipher.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_Initialize" and "APP_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

#ifndef MCHP_CRYPTO_MAC_CIPHER_H
#define MCHP_CRYPTO_MAC_CIPHER_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "MCHP_Crypto_Common.h"
#include "MCHP_Crypto_Mac_Config.h"

typedef enum
{
    CRYPTO_MAC_ERROR_CIPNOTSUPPTD = -127,
    CRYPTO_MAC_ERROR_CTX = -126,
    CRYPTO_MAC_ERROR_KEY = -125,
    CRYPTO_MAC_ERROR_HDLR = -124,
    CRYPTO_MAC_ERROR_INPUTDATA = -123,
    CRYPTO_MAC_ERROR_MACDATA = -122,        
    CRYPTO_MAC_ERROR_CIPOPER = -121,
    CRYPTO_MAC_ERROR_SID = -120,  ////session ID Error
    CRYPTO_MAC_ERROR_ARG = -119,
    CRYPTO_MAC_ERROR_CIPFAIL = -118,
    CRYPTO_MAC_CIPHER_SUCCESS = 0,        
}crypto_Mac_Status_E;

// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************
typedef struct
{
    uint32_t cryptoSessionID;
    crypto_HandlerType_E macHandlerType_en;
    uint8_t *ptr_key;
    uint32_t mackeyLen;
    uint8_t arr_macDataCtx[70];
}st_Crypto_Mac_Aes_ctx;
// *****************************************************************************
#ifdef CRYPTO_MAC_AESCMAC_EN 
crypto_Mac_Status_E Crypto_Mac_AesCmac_Init(st_Crypto_Mac_Aes_ctx *ptr_aesCmacCtx_st, crypto_HandlerType_E handlerType_en, 
                                              uint8_t *ptr_key, uint32_t keyLen, uint32_t sessionID);
crypto_Mac_Status_E Crypto_Mac_AesCmac_Cipher(st_Crypto_Mac_Aes_ctx *ptr_aesCmacCtx_st, uint8_t *ptr_inputData, uint32_t dataLen);

crypto_Mac_Status_E Crypto_Mac_AesCmac_Final(st_Crypto_Mac_Aes_ctx *ptr_aesCmacCtx_st, uint8_t *ptr_outMac, uint32_t macLen);

crypto_Mac_Status_E Crypto_Mac_AesCmac_Direct(crypto_HandlerType_E macHandlerType_en, uint8_t *ptr_inputData, uint32_t dataLen, 
                                                uint8_t *ptr_outMac, uint32_t macLen, uint8_t *ptr_key, uint32_t keyLen, uint32_t sessionID);
#endif /* CRYPTO_MAC_AESCMAC_EN */


#endif /* MCHP_CRYPTO_MAC_CIPHER_H */