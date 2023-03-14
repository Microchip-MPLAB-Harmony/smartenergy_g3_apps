/*******************************************************************************
  Source for the Time Management in G3 stack

  Company:
    Microchip Technology Inc.

  File Name:
    time_management.c

  Summary:
    Interface implementation of the Time Management in G3.

  Description:
    This file implements the interface for the time management in G3.
*******************************************************************************/

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

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include "system/time/sys_time.h"

// *****************************************************************************
// *****************************************************************************
// Section: Static Data
// *****************************************************************************
// *****************************************************************************

/* Time control variables */
static uint64_t previousCounter64 = 0;
static uint32_t currentMsCounter = 0;

// *****************************************************************************
// *****************************************************************************
// Section: File scope functions
// *****************************************************************************
// *****************************************************************************

uint32_t SRV_TIME_MANAGEMENT_GetMsCounter(void)
{
    uint64_t diffCounter64, currentCounter64;
    uint32_t elapsedMs;

    // Get current timer counter    
    currentCounter64 = SYS_TIME_Counter64Get();
    // Diff with previous
    diffCounter64 = currentCounter64 - previousCounter64;
    // Diff in Ms
    elapsedMs = SYS_TIME_CountToMS((uint32_t)diffCounter64);
    // Update Ms counter
    currentMsCounter += elapsedMs;
    // Update previous counter for next computation
    previousCounter64 += SYS_TIME_MSToCount(elapsedMs);

    return currentMsCounter;
}

bool SRV_TIME_MANAGEMENT_TimeIsPast(int32_t timeValue)
{
     return (((int32_t)(SRV_TIME_MANAGEMENT_GetMsCounter()) - timeValue) > 0);
}
