/*******************************************************************************
  PLC PVDD Monitor Service Library

  Company:
    Microchip Technology Inc.

  File Name:
    srv_pvddmon.c

  Summary:
    PLC PVDD Monitor Service File

  Description:
    None

*******************************************************************************/

/*
Copyright (C) 2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

static void lADC_PVDDMONCallback( uint32_t status, uint32_t eocStatus, uintptr_t context )
{
    /* Avoid warning */
    (void)eocStatus;

    if ((status & ADC_ISR_COMPE_Msk) != 0U)
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
    ADC_CHANNEL_MASK channelMsk = ADC_CH4_MASK;

    /* Disable ADC channel */
    ADC_ChannelsDisable(channelMsk);

    /* Disable channel interrupt */
    ADC_ChannelsInterruptDisable((ADC_INTERRUPT_EOC_MASK)channelMsk);
}

void SRV_PVDDMON_Start (SRV_PVDDMON_CMP_MODE cmpMode)
{
    uint32_t emr = 0;
    ADC_CHANNEL_MASK channelMsk = ADC_CH4_MASK;

    /* Set Free Run reset */
    ADC_REGS->ADC_TRGR |= ADC_TRGR_TRGMOD_CONTINUOUS;

    /* Set Comparison Mode */
    if (cmpMode == SRV_PVDDMON_CMP_MODE_OUT)
    {
        srv_pvddmon_mode = SRV_PVDDMON_CMP_MODE_OUT;
        emr |= ADC_EMR_CMPMODE_OUT;
        /* Set Compare Window Register */
        ADC_REGS->ADC_CWR = ADC_CWR_HIGHTHRES(SRV_PVDDMON_HIGH_TRESHOLD) | ADC_CWR_LOWTHRES(SRV_PVDDMON_LOW_TRESHOLD);
    }
    else
    {
        srv_pvddmon_mode = SRV_PVDDMON_CMP_MODE_IN;
        emr |= ADC_EMR_CMPMODE_IN;
        /* Set Compare Window Register */
        ADC_REGS->ADC_CWR = ADC_CWR_HIGHTHRES(SRV_PVDDMON_HIGH_TRESHOLD_HYST) | ADC_CWR_LOWTHRES(SRV_PVDDMON_LOW_TRESHOLD_HYST);
    }

    /* Set Comparison Selected Channel */
    emr |= ADC_EMR_CMPSEL(4);

    /* Set Compare Type */
    emr |= ADC_EMR_CMPTYPE_Msk;

    /* Set Filter */
    emr |= ADC_EMR_CMPFILTER(3);
    ADC_REGS->ADC_EMR = emr;

    /* Enable Comparison Event Interrupt */
    ADC_REGS->ADC_IER |= ADC_IER_COMPE_Msk;

    /* Enable ADC channel */
    ADC_ChannelsEnable(channelMsk);

    /* Comparison Restart */
    ADC_REGS->ADC_CR = 0x1U << ADC_CR_CMPRST_Pos;

    /* Start ADC conversion */
    ADC_ConversionStart();
}

void SRV_PVDDMON_Restart (SRV_PVDDMON_CMP_MODE cmpMode)
{
    uint32_t emr;
    ADC_CHANNEL_MASK channelMsk = ADC_CH4_MASK;

    /* Disable channel COMPE interrupt */
    ADC_REGS->ADC_IDR |= ADC_IER_COMPE_Msk;

    /* Disable ADC channel */
    ADC_ChannelsDisable(channelMsk);

    /* Set Comparison Mode */
    if (cmpMode == SRV_PVDDMON_CMP_MODE_OUT)
    {
        srv_pvddmon_mode = SRV_PVDDMON_CMP_MODE_OUT;
        emr = ADC_EMR_CMPMODE_OUT;
        /* Set Compare Window Register */
        ADC_REGS->ADC_CWR = ADC_CWR_HIGHTHRES(SRV_PVDDMON_HIGH_TRESHOLD) | ADC_CWR_LOWTHRES(SRV_PVDDMON_LOW_TRESHOLD);
    }
    else
    {
        srv_pvddmon_mode = SRV_PVDDMON_CMP_MODE_IN;
        emr = ADC_EMR_CMPMODE_IN;
        /* Set Compare Window Register */
        ADC_REGS->ADC_CWR = ADC_CWR_HIGHTHRES(SRV_PVDDMON_HIGH_TRESHOLD_HYST) | ADC_CWR_LOWTHRES(SRV_PVDDMON_LOW_TRESHOLD_HYST);
    }
    ADC_REGS->ADC_EMR &= ~ADC_EMR_CMPMODE_Msk;
    ADC_REGS->ADC_EMR |= emr;

    while((ADC_REGS->ADC_ISR & ADC_ISR_COMPE_Msk) != 0U){}

    /* Comparison Restart */
    ADC_REGS->ADC_CR = 0x1U << ADC_CR_CMPRST_Pos;

    /* Enable ADC channel */
    ADC_ChannelsEnable(channelMsk);

    /* Enable Comparison Event Interrupt */
    ADC_REGS->ADC_IER |= ADC_IER_COMPE_Msk;
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

    adcValue = ADC_ChannelResultGet(ADC_CH4);
    while(adcValue == 0U)
    {
        adcValue = ADC_ChannelResultGet(ADC_CH4);
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