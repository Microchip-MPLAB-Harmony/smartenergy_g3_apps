/*******************************************************************************
  Header for the pseudo-random numbers generation service

  Company:
    Microchip Technology Inc.

  File Name:
    srv_random.h

  Summary:
    Interface definition for the pseudo-random numbers generation service.

  Description:
    This file defines the interface for the pseudo-random numbers generation
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

#ifndef SRV_RANDOM_H
#define SRV_RANDOM_H

#ifdef __cplusplus // Provide C++ Compatibility
 extern "C" {
#endif

//******************************************************************************
/* Function:
    uint8_t SRV_RANDOM_Get8bits(void)

  Summary:
    Returns a random value of 8 bits.

  Description:
    This function returns a random value of 8 bits.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    8-bit random value.

  Example:
    <code>
    uint8_t rndNum = SRV_RANDOM_Get8bits();
    </code>

  Remarks:
    None.
*/

uint8_t SRV_RANDOM_Get8bits(void);

//******************************************************************************
/* Function:
    uint16_t SRV_RANDOM_Get16bits(void)

  Summary:
    Returns a random value of 16 bits.

  Description:
    This function returns a random value of 16 bits.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    16-bit random value.

  Example:
    <code>
    uint16_t rndNum = SRV_RANDOM_Get16bits();
    </code>

  Remarks:
    None.
*/

uint16_t SRV_RANDOM_Get16bits(void);

//******************************************************************************
/* Function:
    uint16_t SRV_RANDOM_Get16bitsInRange(uint16_t min, uint16_t max)

  Summary:
    Returns a random value of 16 bits within the given range.

  Description:
    This function returns a random value of 16 bits within the given range.

  Precondition:
    None.

  Parameters:
    min       - Minimum value of the range
    max       - Maximum value of the range

  Returns:
    16-bit random value.

  Example:
    <code>
    uint16_t rndNum = SRV_RANDOM_Get16bits(0, 1500);
    </code>

  Remarks:
    None.
*/

uint16_t SRV_RANDOM_Get16bitsInRange(uint16_t min, uint16_t max);

//******************************************************************************
/* Function:
    uint16_t SRV_RANDOM_Get32bits(void)

  Summary:
    Returns a random value of 32 bits.

  Description:
    This function returns a random value of 32 bits.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    16-bit random value.

  Example:
    <code>
    uint32_t rndNum = SRV_RANDOM_GET32bits();
    </code>

  Remarks:
    None.
*/

uint32_t SRV_RANDOM_Get32bits(void);

//******************************************************************************
/* Function:
    uint32_t SRV_RANDOM_Get32bitsInRange(uint32_t min, uint32_t max)

  Summary:
    Returns a random value of 32 bits within the given range.

  Description:
    This function returns a random value of 32 bits within the given range.

  Precondition:
    None.

  Parameters:
    min       - Minimum value of the range
    max       - Maximum value of the range

  Returns:
    32-bit random value.

  Example:
    <code>
    uint32_t rndNum = SRV_RANDOM_Get32bits(0, 0xFFFFF);
    </code>

  Remarks:
    None.
*/

uint32_t SRV_RANDOM_Get32bitsInRange(uint32_t min, uint32_t max);

//******************************************************************************
/* Function:
    void SRV_RANDOM_Get128bits(uint8_t *rndValue)

  Summary:
    Returns a random value of 128 bits.

  Description:
    This function returns a random value of 128 bits.

  Precondition:
    None.

  Parameters:
    rndValue     - Pointer to an array to return the generated value

  Returns:
    None.

  Example:
    <code>
    uint8_t rndNum[16];
    SRV_RANDOM_GET128bits(rndNum);
    </code>

  Remarks:
    None.
*/

void SRV_RANDOM_Get128bits(uint8_t *rndValue);

#ifdef __cplusplus // Provide C++ Compatibility
 }
#endif

#endif /* SRV_RANDOM_H */
