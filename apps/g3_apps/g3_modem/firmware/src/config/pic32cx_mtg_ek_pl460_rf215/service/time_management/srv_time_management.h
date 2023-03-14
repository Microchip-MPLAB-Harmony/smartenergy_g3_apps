/*******************************************************************************
  Header for the Time Management in G3 stack

  Company:
    Microchip Technology Inc.

  File Name:
    time_management.h

  Summary:
    Interface definition of the Time Management in G3.

  Description:
    This file defines the interface for the time management in G3.
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

#ifndef _TIME_MANAGEMENT_H
#define _TIME_MANAGEMENT_H

#ifdef __cplusplus // Provide C++ Compatibility
 extern "C" {
#endif

// *****************************************************************************
/* Function:
    uint32_t SRV_TIME_MANAGEMENT_GetMsCounter
    (
      void
    )

  Summary:
    The SRV_TIME_MANAGEMENT_GetMsCounter primitive gets the value of a counter 
    that is incremented every millisecond.

  Description:
    This primitive makes use of SYS_TIME service to get the value of the 
    millisecond counter in order to be able to set timeouts and perform delays.
    This function returns the current value of such counter.

  Precondition:
    SYS_TIME_Initialize primitive has to be called before.

  Parameters:
    None.

  Returns:
    Value of milliseconds counter.

  Example:
    <code>
    previousCounter = SRV_TIME_MANAGEMENT_GetMsCounter();

    // Perform other actions
    // ...

    newCounter = SRV_TIME_MANAGEMENT_GetMsCounter();

    if ((newCounter - previousCounter) > TIMEOUT_MS)
    {
        // Timeout elapsed
    }
    </code>

  Remarks:
    None.
*/
uint32_t SRV_TIME_MANAGEMENT_GetMsCounter(void);

// *****************************************************************************
/* Function:
    bool SRV_TIME_MANAGEMENT_TimeIsPast
    (
      int32_t timeValue
    )

  Summary:
    Indicates whether the given time value is in the past.

  Description:
    This primitive indicates whether the given time value is in the past.

  Precondition:
    SYS_TIME_Initialize primitive has to be called before.

  Parameters:
    timeValue      - Time value in milliseconds

  Returns:
    True if the time value is in the past. 
    False if the time value is not in the past.

  Example:
    <code>
    int32_t validityTime = 5000;

    // Perform other actions
    // ...

    if (SRV_TIME_MANAGEMENT_TimeIsPast(validityTime))
    {
        // Validity ended
    }
    </code>

  Remarks:
    None.
*/
bool SRV_TIME_MANAGEMENT_TimeIsPast(int32_t timeValue);


#ifdef __cplusplus // Provide C++ Compatibility
 }
#endif

#endif /* _TIME_MANAGEMENT_H */
