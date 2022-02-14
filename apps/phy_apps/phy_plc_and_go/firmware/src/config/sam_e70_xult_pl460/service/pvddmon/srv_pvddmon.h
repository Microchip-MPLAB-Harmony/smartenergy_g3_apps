/*******************************************************************************
  Interface definition of TRNG PLIB.

  Company:
    Microchip Technology Inc.

  File Name:
    srv_pvddmon.h

  Summary:
    Interface definition of the True Random Number Generator Plib (TRNG).

  Description:
    This file defines the interface for the TRNG Plib.
    It allows user to generate true random numbers.
*******************************************************************************/

/*******************************************************************************
* Copyright (C) 2021 Microchip Technology Inc. and its subsidiaries.
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

#ifndef SRV_PVDDMON_H    // Guards against multiple inclusion
#define SRV_PVDDMON_H

#include <stdint.h>
#include "device.h"
#include "interrupts.h"
#include "peripheral/afec/plib_afec_common.h"


#ifdef __cplusplus // Provide C++ Compatibility
 extern "C" {
#endif

typedef enum 
{
    SRV_PVDDMON_CMP_MODE_IN,

    SRV_PVDDMON_CMP_MODE_OUT,

} SRV_PVDDMON_CMP_MODE;

typedef void (*SRV_PVDDMON_CALLBACK)( SRV_PVDDMON_CMP_MODE cmpMode, uintptr_t context );

#define SRV_PVDDMON_HIGH_TRESHOLD              0xdb3
#define SRV_PVDDMON_LOW_TRESHOLD               0xa8a
#define SRV_PVDDMON_HIGH_TRESHOLD_HYST         0xd98
#define SRV_PVDDMON_LOW_TRESHOLD_HYST          0xaa5

/***************************** SRV PLC PVDD MONITOR API *******************************/
void SRV_PVDDMON_Initialize (void);
void SRV_PVDDMON_Start (SRV_PVDDMON_CMP_MODE cmpMode);
void SRV_PVDDMON_Restart (SRV_PVDDMON_CMP_MODE cmpMode);
void SRV_PVDDMON_RegisterCallback (SRV_PVDDMON_CALLBACK callback_fn, uintptr_t context);

#ifdef __cplusplus // Provide C++ Compatibility
 }
#endif

#endif 
