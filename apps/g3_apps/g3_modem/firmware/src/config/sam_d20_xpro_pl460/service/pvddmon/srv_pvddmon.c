/*******************************************************************************
  PLC PVDD Monitor Service Library

  Company:
    Microchip Technology Inc.

  File Name:
    srv_pvddmon.c (U2204)

  Summary:
    PLC PVDD Monitor Service File

  Description:
    None

*******************************************************************************/

/*******************************************************************************
* Copyright (C) 2024 Microchip Technology Inc. and its subsidiaries.
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
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "device.h"
#include "interrupts.h"
#include "srv_pvddmon.h"
#include "peripheral/adc/plib_adc.h"

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Data
// *****************************************************************************
// *****************************************************************************

static SRV_PVDDMON_CMP_MODE srv_pvddmon_mode;
static SRV_PVDDMON_CALLBACK ADC_CompareCallback = NULL;

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Functions
// *****************************************************************************
// *****************************************************************************

static void lADC_PVDDMONCallback( ADC_STATUS status, uintptr_t context )
{
    if ((status & ADC_STATUS_WINMON) != 0U)
    {
        if (ADC_CompareCallback != NULL)
        {
            ADC_CompareCallback(srv_pvddmon_mode, context);
        }
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: PLC PVDD Monitor Service Interface Implementation
// *****************************************************************************
// *****************************************************************************

void SRV_PVDDMON_Initialize (void)
{
    /* Disable ADC channel */
    ADC_Disable();

    /* Disable channel interrupts */
    ADC_InterruptsDisable(ADC_STATUS_MASK);
    
    /* Clear all interrupt flags */
    ADC_InterruptsClear(ADC_STATUS_MASK);
}

void SRV_PVDDMON_Start (SRV_PVDDMON_CMP_MODE cmpMode)
{
    /* Set Comparison Mode */
    if (cmpMode == SRV_PVDDMON_CMP_MODE_OUT)
    {
        srv_pvddmon_mode = SRV_PVDDMON_CMP_MODE_OUT;
        /* Set Compare Window Register */
        ADC_ComparisonWindowSet(SRV_PVDDMON_LOW_TRESHOLD, SRV_PVDDMON_HIGH_TRESHOLD);
        ADC_WindowModeSet(ADC_WINMODE_OUTSIDE_WINLT_AND_WINUT);
    }
    else
    {
        srv_pvddmon_mode = SRV_PVDDMON_CMP_MODE_IN;
        /* Set Compare Window Register */
        ADC_ComparisonWindowSet(SRV_PVDDMON_LOW_TRESHOLD_HYST, SRV_PVDDMON_HIGH_TRESHOLD_HYST);
        ADC_WindowModeSet(ADC_WINMODE_BETWEEN_WINLT_AND_WINUT);
    }

    /* Enable channel interrupt */
    ADC_InterruptsEnable(ADC_STATUS_WINMON);

    /* Enable ADC channel */
    ADC_Enable();

    /* Start ADC conversion */
    ADC_ConversionStart();
}

void SRV_PVDDMON_Restart (SRV_PVDDMON_CMP_MODE cmpMode)
{
    /* Disable ADC channel */
    ADC_Disable();

    /* Disable channel interrupts */
    ADC_InterruptsDisable(ADC_STATUS_MASK);
    
    /* Clear all interrupt flags */
    ADC_InterruptsClear(ADC_STATUS_MASK);
    
    /* Set Comparison Mode */
    if (cmpMode == SRV_PVDDMON_CMP_MODE_OUT)
    {
        srv_pvddmon_mode = SRV_PVDDMON_CMP_MODE_OUT;
        /* Set Compare Window Register */
        ADC_ComparisonWindowSet(SRV_PVDDMON_LOW_TRESHOLD, SRV_PVDDMON_HIGH_TRESHOLD);
        ADC_WindowModeSet(ADC_WINMODE_OUTSIDE_WINLT_AND_WINUT);
    }
    else
    {
        srv_pvddmon_mode = SRV_PVDDMON_CMP_MODE_IN;
        /* Set Compare Window Register */
        ADC_ComparisonWindowSet(SRV_PVDDMON_LOW_TRESHOLD_HYST, SRV_PVDDMON_HIGH_TRESHOLD_HYST);
        ADC_WindowModeSet(ADC_WINMODE_BETWEEN_WINLT_AND_WINUT);
    }

    /* Enable channel interrupt */
    ADC_InterruptsEnable(ADC_STATUS_WINMON);

    /* Enable ADC channel */
    ADC_Enable();

    /* Start ADC conversion */
    ADC_ConversionStart();
}

void SRV_PVDDMON_CallbackRegister (SRV_PVDDMON_CALLBACK callback, uintptr_t context)
{
    /* Register ADC Callback */
    ADC_CallbackRegister(lADC_PVDDMONCallback, context);
    ADC_CompareCallback = callback;
}

bool SRV_PVDDMON_CheckWindow(void)
{
    uint32_t adcValue;
    
    adcValue = ADC_ConversionResultGet();
    while(adcValue == 0U)
    {
        adcValue = ADC_ConversionResultGet();
    }
    
    if ((adcValue <= SRV_PVDDMON_HIGH_TRESHOLD) && (adcValue >= SRV_PVDDMON_LOW_TRESHOLD))
    {
        return true;
    }
    else
    {
        return false;
    }
}