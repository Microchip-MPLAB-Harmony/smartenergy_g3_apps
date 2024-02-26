/*******************************************************************************
  Source for the pseudo-random numbers generation service

  Company:
    Microchip Technology Inc.

  File Name:
    srv_random.c

  Summary:
    Interface implementation for the pseudo-random numbers generation service.

  Description:
    This file implements the interface for the pseudo-random numbers generation
    service.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*
Copyright (C) 2024, Microchip Technology Inc., and its subsidiaries. All rights reserved.

The software and documentation is provided by microchip and its contributors
"as is" and any express, implied or statutory warranties, including, but not
limited to, the implied warranties of merchantability, fitness for a particular
purpose and non-infringement of third party intellectual property rights are
disclaimed to the fullest extent permitted by law. In no event shall microchip
or its contributors be liable for any direct, indirect, incidental, special,
exemplary, or consequential damages (including, but not limited to, procurement
of substitute goods or services; loss of use, data, or profits; or business
interruption) however caused and on any theory of liability, whether in contract,
strict liability, or tort (including negligence or otherwise) arising in any way
out of the use of the software and documentation, even if advised of the
possibility of such damage.

Except as expressly permitted hereunder and subject to the applicable license terms
for any third-party software incorporated in the software and any applicable open
source software license terms, no license or other rights, whether express or
implied, are granted under any patent or other intellectual property rights of
Microchip or any third party.
*/
//DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include "definitions.h"
#include <stdlib.h>
#include "crypto/common_crypto/MCHP_Crypto_RNG.h"
#include "srv_random.h"

// *****************************************************************************
// *****************************************************************************
// Section: Random Service Interface Implementation
// *****************************************************************************
// *****************************************************************************

uint8_t SRV_RANDOM_Get8bits(void)
{
    uint8_t retValue = 0;

    crypto_Rng_Status_E status;
    uint32_t seed;
    uint8_t randBuf[1];

    // Generate random number from Crypto
    status = Crypto_Rng_Prng_Generate(CRYPTO_HANDLER_SW_WOLFCRYPT,
        randBuf, 1, NULL, 0, 1);

    if (status == CRYPTO_RNG_SUCCESS) {
        retValue = randBuf[0];
    }
    else {
        seed = SYS_TIME_CounterGet();
        srand(seed);
        retValue = (uint8_t)rand();
    }

    return retValue;
}

uint16_t SRV_RANDOM_Get16bits(void)
{
    uint16_t retValue = 0;

    crypto_Rng_Status_E status;
    uint32_t seed;
    uint8_t randBuf[2];

    // Generate random number from Crypto
    status = Crypto_Rng_Prng_Generate(CRYPTO_HANDLER_SW_WOLFCRYPT,
        randBuf, 2, NULL, 0, 1);

    if (status == CRYPTO_RNG_SUCCESS) {
        retValue = (randBuf[1] << 8) + randBuf[0];
    }
    else {
        seed = SYS_TIME_CounterGet();
        srand(seed);
        retValue = (uint16_t)rand();
    }

    return retValue;
}

uint16_t  SRV_RANDOM_Get16bitsInRange(uint16_t min, uint16_t max)
{
    uint16_t localMin = min;

    if (max < min)
    {
        localMin = max;
        max = min;
    }

    return (SRV_RANDOM_Get16bits() % (max - localMin + 1U) + localMin);
}

uint32_t SRV_RANDOM_Get32bits(void)
{
    uint32_t retValue = 0;

    crypto_Rng_Status_E status;
    uint32_t seed;
    uint8_t randBuf[4];

    // Generate random number from Crypto
    status = Crypto_Rng_Prng_Generate(CRYPTO_HANDLER_SW_WOLFCRYPT,
        randBuf, 4, NULL, 0, 1);

    if (status == CRYPTO_RNG_SUCCESS) {
        retValue = (randBuf[3] << 24) + (randBuf[2] << 16) +
            (randBuf[1] << 8) + randBuf[0];
    }
    else {
        seed = SYS_TIME_CounterGet();
        srand(seed);
        retValue = rand();
    }

    return retValue;
}

uint32_t SRV_RANDOM_Get32bitsInRange(uint32_t min, uint32_t max)
{
    uint32_t localMin = min;

    if (max < min)
    {
        localMin = max;
        max = min;
    }

    return (SRV_RANDOM_Get32bits() % (max - localMin + 1U) + localMin);
}

void SRV_RANDOM_Get128bits(uint8_t *rndValue)
{
    crypto_Rng_Status_E status;
    uint32_t seed;
    uint32_t randNum;
    uint8_t n;

    // Generate random number from Crypto
    status = Crypto_Rng_Prng_Generate(CRYPTO_HANDLER_SW_WOLFCRYPT,
        rndValue, 16, NULL, 0, 1);

    if (status != CRYPTO_RNG_SUCCESS) {
        seed = SYS_TIME_CounterGet();
        srand(seed);

        for (n = 0; n < 4; n ++)
        {
            randNum = rand();

            *rndValue++ = (uint8_t)(randNum >> 24);
            *rndValue++ = (uint8_t)(randNum >> 16);
            *rndValue++ = (uint8_t)(randNum >> 8);
            *rndValue++ = (uint8_t)randNum;
        }
    }
}
