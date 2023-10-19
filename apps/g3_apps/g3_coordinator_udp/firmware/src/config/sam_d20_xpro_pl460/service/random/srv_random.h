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
/*******************************************************************************
* Copyright (C) 2023 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/
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
