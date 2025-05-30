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

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "device.h"
#include "interrupts.h"
#include "srv_pvddmon.h"
#include "peripheral/adc/plib_adc1.h"

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

static void lADC_WindowModeSet(ADC_WINMODE mode)
{
    ADC1_REGS->ADC_CTRLB = (ADC1_REGS->ADC_CTRLB & (uint16_t)~ADC_CTRLB_WINMODE_Msk) | ((uint16_t)mode << ADC_CTRLB_WINMODE_Pos);

    while((ADC1_REGS->ADC_SYNCBUSY & ADC_SYNCBUSY_CTRLB_Msk) == ADC_SYNCBUSY_CTRLB_Msk)
    {
        /* Wait for Synchronization */
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
    ADC1_Disable();

    /* Disable channel interrupts */
    ADC1_InterruptsDisable(ADC_STATUS_MASK);

    /* Clear all interrupt flags */
    ADC1_InterruptsClear(ADC_STATUS_MASK);
}

void SRV_PVDDMON_Start (SRV_PVDDMON_CMP_MODE cmpMode)
{
    /* Set Comparison Mode */
    if (cmpMode == SRV_PVDDMON_CMP_MODE_OUT)
    {
        srv_pvddmon_mode = SRV_PVDDMON_CMP_MODE_OUT;
        /* Set Compare Window Register */
        ADC1_ComparisonWindowSet(SRV_PVDDMON_LOW_TRESHOLD, SRV_PVDDMON_HIGH_TRESHOLD);
        lADC_WindowModeSet(ADC_WINMODE_OUTSIDE_WINLT_AND_WINUT);
    }
    else
    {
        srv_pvddmon_mode = SRV_PVDDMON_CMP_MODE_IN;
        /* Set Compare Window Register */
        ADC1_ComparisonWindowSet(SRV_PVDDMON_LOW_TRESHOLD_HYST, SRV_PVDDMON_HIGH_TRESHOLD_HYST);
        lADC_WindowModeSet(ADC_WINMODE_BETWEEN_WINLT_AND_WINUT);
    }

    /* Enable channel interrupt */
    ADC1_InterruptsEnable(ADC_STATUS_WINMON);

    /* Enable ADC channel */
    ADC1_Enable();

    /* Start ADC conversion */
    ADC1_ConversionStart();
}

void SRV_PVDDMON_Restart (SRV_PVDDMON_CMP_MODE cmpMode)
{
    /* Disable ADC channel */
    ADC1_Disable();

    /* Disable channel interrupts */
    ADC1_InterruptsDisable(ADC_STATUS_MASK);

    /* Clear all interrupt flags */
    ADC1_InterruptsClear(ADC_STATUS_MASK);

    /* Set Comparison Mode */
    if (cmpMode == SRV_PVDDMON_CMP_MODE_OUT)
    {
        srv_pvddmon_mode = SRV_PVDDMON_CMP_MODE_OUT;
        /* Set Compare Window Register */
        ADC1_ComparisonWindowSet(SRV_PVDDMON_LOW_TRESHOLD, SRV_PVDDMON_HIGH_TRESHOLD);
        lADC_WindowModeSet(ADC_WINMODE_OUTSIDE_WINLT_AND_WINUT);
    }
    else
    {
        srv_pvddmon_mode = SRV_PVDDMON_CMP_MODE_IN;
        /* Set Compare Window Register */
        ADC1_ComparisonWindowSet(SRV_PVDDMON_LOW_TRESHOLD_HYST, SRV_PVDDMON_HIGH_TRESHOLD_HYST);
        lADC_WindowModeSet(ADC_WINMODE_BETWEEN_WINLT_AND_WINUT);
    }

    /* Enable channel interrupt */
    ADC1_InterruptsEnable(ADC_STATUS_WINMON);

    /* Enable ADC channel */
    ADC1_Enable();

    /* Start ADC conversion */
    ADC1_ConversionStart();
}

void SRV_PVDDMON_CallbackRegister (SRV_PVDDMON_CALLBACK callback, uintptr_t context)
{
    /* Register ADC Callback */
    ADC1_CallbackRegister(lADC_PVDDMONCallback, context);
    ADC_CompareCallback = callback;
}

bool SRV_PVDDMON_CheckWindow(void)
{
    uint16_t adcValue;

    adcValue = ADC1_ConversionResultGet();
    while(adcValue == 0U)
    {
        adcValue = ADC1_ConversionResultGet();
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