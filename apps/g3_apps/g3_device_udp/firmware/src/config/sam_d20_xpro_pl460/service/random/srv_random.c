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
#include "wolfssl/wolfcrypt/random.h"
#include "srv_random.h"

// *****************************************************************************
// *****************************************************************************
// Section: Random Service Interface Implementation
// *****************************************************************************
// *****************************************************************************

uint8_t SRV_RANDOM_Get8bits(void)
{
    uint8_t retValue = 0;

    WC_RNG rngCtx;

    (void) wc_InitRng(&rngCtx);
    (void) wc_RNG_GenerateByte(&rngCtx, (byte*)&retValue);
    (void) wc_FreeRng(&rngCtx);

    return retValue;
}

uint16_t SRV_RANDOM_Get16bits(void)
{
    uint16_t retValue = 0;

    WC_RNG rngCtx;

    (void) wc_InitRng(&rngCtx);
    (void) wc_RNG_GenerateBlock(&rngCtx, (byte*)&retValue, 2);
    (void) wc_FreeRng(&rngCtx);

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

    WC_RNG rngCtx;

    (void) wc_InitRng(&rngCtx);
    (void) wc_RNG_GenerateBlock(&rngCtx, (byte*)&retValue, 4);
    (void) wc_FreeRng(&rngCtx);

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
    WC_RNG rngCtx;

    (void) wc_InitRng(&rngCtx);
    (void) wc_RNG_GenerateBlock(&rngCtx, (byte*)rndValue, 16);
    (void) wc_FreeRng(&rngCtx);
}
