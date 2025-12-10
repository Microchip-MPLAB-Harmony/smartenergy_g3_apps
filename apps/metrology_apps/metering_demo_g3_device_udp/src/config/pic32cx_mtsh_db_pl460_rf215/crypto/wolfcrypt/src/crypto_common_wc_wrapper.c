/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    crypto_common_wc_wrapper.c

  Summary:
    This file contains the Common code for the Wolfcrypt Library application.

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
#include <stdlib.h>
#include <time.h>

#include "crypto/wolfcrypt/crypto_common_wc_wrapper.h"

__attribute__((weak)) int Crypto_Rng_Wc_Prng_EntropySource(void)
{
  return (int) rand();
}

__attribute__((weak)) int Crypto_Rng_Wc_Prng_Srand(uint8_t* output, unsigned int sz)
{   
    unsigned int i;
    for (i = 0; i < sz; i++)
    {
        int randVal = Crypto_Rng_Wc_Prng_EntropySource() % 256;
        output[i] = (uint8_t)randVal;
    }
    
    return 0;
} 