/*******************************************************************************
  ADC Peripheral Library Interface Source File

  Company
    Microchip Technology Inc.

  File Name
    plib_adc.c

  Summary
    ADC peripheral library source.

  Description
    This file implements the ADC peripheral library.

*******************************************************************************/

// DOM-IGNORE-BEGIN
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
// DOM-IGNORE-END
#include "device.h"
#include "plib_adc.h"
#include "interrupts.h"

#define ADC_SEQ1_CHANNEL_NUM (8U)


// *****************************************************************************
// *****************************************************************************
// Section: ADC Implementation
// *****************************************************************************
// *****************************************************************************
/* Object to hold callback function and context */
volatile static ADC_CALLBACK_OBJECT ADC_CallbackObj;

/* Initialize ADC peripheral */
void ADC_Initialize(void)
{
    /* Software reset */
    ADC_REGS->ADC_CR = ADC_CR_SWRST_Msk;

    /* Prescaler and different time settings as per CLOCK section  */
    ADC_REGS->ADC_MR =  ADC_MR_ALWAYS_Msk |  ADC_MR_PRESCAL(4U) | ADC_MR_TRACKTIM(14U) | ADC_MR_STARTUP_SUT512 |
        ADC_MR_TRANSFER(2U) | ADC_MR_ANACH_ALLOWED;

    /* Resolution and Sign mode of result */
    ADC_REGS->ADC_EMR = ADC_EMR_OSR_NO_AVERAGE  | ADC_EMR_SIGNMODE_SE_UNSG_DF_SIGN | ADC_EMR_TAG_Msk;

    /* Trigger mode */
    ADC_REGS->ADC_TRGR = ADC_TRGR_TRGMOD_CONTINUOUS;

    /* Automatic Window Comparison */
    ADC_REGS->ADC_EMR |= ADC_EMR_CMPMODE_LOW | ADC_EMR_CMPTYPE_FLAG_ONLY | ADC_EMR_CMPSEL(ADC_CH0);
    ADC_REGS->ADC_CWR = ADC_CWR_LOWTHRES(0U) | ADC_CWR_HIGHTHRES(0U);
    ADC_REGS->ADC_IER = ADC_IER_COMPE_Msk;

    /* Enable channel */
    ADC_REGS->ADC_CHER = ADC_CHER_CH4_Msk;
}

/* Enable ADC channels */
void ADC_ChannelsEnable (ADC_CHANNEL_MASK channelsMask)
{
    ADC_REGS->ADC_CHER = (uint32_t)channelsMask;
}

/* Disable ADC channels */
void ADC_ChannelsDisable (ADC_CHANNEL_MASK channelsMask)
{
    ADC_REGS->ADC_CHDR = (uint32_t)channelsMask;
}

/* Enable channel end of conversion interrupt */
void ADC_ChannelsInterruptEnable (ADC_INTERRUPT_EOC_MASK channelsInterruptMask)
{
    ADC_REGS->ADC_EOC_IER = (uint32_t)channelsInterruptMask;
}

/* Disable channel end of conversion interrupt */
void ADC_ChannelsInterruptDisable (ADC_INTERRUPT_EOC_MASK channelsInterruptMask)
{
    ADC_REGS->ADC_EOC_IDR = (uint32_t)channelsInterruptMask;
}

/* Enable interrupt */
void ADC_InterruptEnable (ADC_INTERRUPT_MASK interruptMask)
{
    ADC_REGS->ADC_IER = (uint32_t)interruptMask;
}

/* Disable interrupt */
void ADC_InterruptDisable (ADC_INTERRUPT_MASK interruptMask)
{
    ADC_REGS->ADC_IDR = (uint32_t)interruptMask;
}

/* Get interrupt status */
bool ADC_InterruptStatusGet(ADC_INTERRUPT_MASK interruptMask)
{
    return ((ADC_REGS->ADC_ISR & (uint32_t)interruptMask) != 0U);
}

/* Start the conversion with software trigger */
void ADC_ConversionStart(void)
{
    ADC_REGS->ADC_CR = ADC_CR_START_Msk;
}

/* Check if conversion result is available */
bool ADC_ChannelResultIsReady(ADC_CHANNEL_NUM channel)
{
    return (((ADC_REGS->ADC_EOC_ISR >> channel) & 0x1U) != 0U);
}

/* Read the conversion result */
uint16_t ADC_ChannelResultGet(ADC_CHANNEL_NUM channel)
{
    return (uint16_t)ADC_REGS->ADC_CDR[channel];
}

/* Configure the user defined conversion sequence */
void ADC_ConversionSequenceSet(ADC_CHANNEL_NUM *channelList, uint8_t numChannel)
{
    uint8_t channelIndex;
    ADC_REGS->ADC_SEQR1 = 0U;

    if (numChannel < 8U)
    {
        for (channelIndex = 0U; channelIndex < numChannel; channelIndex++)
        {
            ADC_REGS->ADC_SEQR1 |= channelList[channelIndex] << (channelIndex * 4U);
        }
    }
}

/* Sets Low threshold and High threshold in comparison window */
void ADC_ComparisonWindowSet(uint16_t lowThreshold, uint16_t highThreshold)
{
    ADC_REGS->ADC_CWR = ADC_CWR_LOWTHRES((uint32_t)lowThreshold) | ADC_CWR_HIGHTHRES((uint32_t)highThreshold);
}

/* Check if Comparison event result is available */
bool ADC_ComparisonEventResultIsReady(void)
{
    return (((ADC_REGS->ADC_ISR >> ADC_ISR_COMPE_Pos) & 0x1U) != 0U);
}

/* Restart the comparison function */
void ADC_ComparisonRestart(void)
{
    ADC_REGS->ADC_CR = ADC_CR_CMPRST_Msk;
}

/* Low power - Enable Sleep mode */
void ADC_SleepModeEnable(void)
{
    ADC_REGS->ADC_MR |= ADC_MR_SLEEP_Msk;
}

/* Low power - Disable Sleep mode */
void ADC_SleepModeDisable(void)
{
    ADC_REGS->ADC_MR &= ~(ADC_MR_SLEEP_Msk);
}

/* Low power - Enable Fast wake up mode */
void ADC_FastWakeupEnable(void)
{
    ADC_REGS->ADC_MR |= ADC_MR_FWUP_Msk;
}

/* Low power - Disable Fast wake up mode */
void ADC_FastWakeupDisable(void)
{
    ADC_REGS->ADC_MR &= ~(ADC_MR_FWUP_Msk);
}

/* Register the callback function */
void ADC_CallbackRegister(ADC_CALLBACK callback, uintptr_t context)
{
    ADC_CallbackObj.callback_fn = callback;
    ADC_CallbackObj.context = context;
}

/* Interrupt Handler */
void __attribute__((used)) ADC_InterruptHandler(void)
{
    uint32_t interruptStatus = ADC_REGS->ADC_ISR;
    uint32_t eocInterruptStatus = ADC_REGS->ADC_EOC_ISR;

    /* Additional temporary variable used to prevent MISRA violations (Rule 13.x) */
    uintptr_t context = ADC_CallbackObj.context;

    if (ADC_CallbackObj.callback_fn != NULL)
    {
        ADC_CallbackObj.callback_fn(interruptStatus, eocInterruptStatus, context);
    }
}
