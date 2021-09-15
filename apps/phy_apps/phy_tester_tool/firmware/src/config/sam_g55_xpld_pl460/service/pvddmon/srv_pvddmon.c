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

#include "device.h"
#include "interrupts.h"
#include "srv_pvddmon.h"
#include "peripheral/adc/plib_adc.h"

static SRV_PVDDMON_CMP_MODE srv_pvddmon_mode;

// *****************************************************************************
// *****************************************************************************
// Section: PLC PVDD Monitor Service Implementation
// *****************************************************************************
// *****************************************************************************
/* Objects to hold callback function and context */
static SRV_PVDDMON_CALLBACK ADC_CompareCallback = NULL;

static void _ADC_PVDDMONCallback( uint32_t status, uintptr_t context )
{
    if (status & ADC_ISR_COMPE_Msk)
    {
        if (ADC_CompareCallback)
        {
            ADC_CompareCallback(srv_pvddmon_mode, context);
        }
    }
}

/* Start PLC PVDD Monitor */
void SRV_PVDDMON_Initialize (void)
{
    ADC_CHANNEL_MASK channelMsk = (1 << 0);

    /* Disable ADC channel */
    ADC_ChannelsDisable(channelMsk);

    /* Disable channel interrupt */
    ADC_ChannelsInterruptDisable(channelMsk);
}

/* Start PLC PVDD Monitor */
void SRV_PVDDMON_Start (SRV_PVDDMON_CMP_MODE cmpMode)
{
    uint32_t emr = 0;
    ADC_CHANNEL_MASK channelMsk = (1 << 0);

    /* Set Free Run reset */
    ADC_REGS->ADC_MR |= ADC_MR_FREERUN_Msk;

    /* Set Compare Window Register */
    ADC_REGS->ADC_CWR = ADC_CWR_HIGHTHRES(SRV_PVDDMON_HIGH_TRESHOLD) | ADC_CWR_LOWTHRES(SRV_PVDDMON_LOW_TRESHOLD); 

    /* Set Comparison Mode */
    if (cmpMode == SRV_PVDDMON_CMP_MODE_OUT)
    {
        srv_pvddmon_mode = SRV_PVDDMON_CMP_MODE_OUT;
        emr |= ADC_EMR_CMPMODE_OUT;
    }
    else
    {
        srv_pvddmon_mode = SRV_PVDDMON_CMP_MODE_IN;
        emr |= ADC_EMR_CMPMODE_IN;
    }

    /* Set Comparison Selected Channel */
    emr |= ADC_EMR_CMPSEL(0);

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

/* Restart PLC PVDD Monitor */
void SRV_PVDDMON_Restart (SRV_PVDDMON_CMP_MODE cmpMode)
{
    uint32_t emr;
    ADC_CHANNEL_MASK channelMsk = (1 << 0);

    /* Disable channel COMPE interrupt */
    ADC_REGS->ADC_IDR |= ADC_IER_COMPE_Msk;

    /* Disable ADC channel */
    ADC_ChannelsDisable(channelMsk);

    /* Set Comparison Mode */
    if (cmpMode == SRV_PVDDMON_CMP_MODE_OUT)
    {
      srv_pvddmon_mode = SRV_PVDDMON_CMP_MODE_OUT;
      emr = ADC_EMR_CMPMODE_OUT;
    }
    else
    {
      srv_pvddmon_mode = SRV_PVDDMON_CMP_MODE_IN;
      emr = ADC_EMR_CMPMODE_IN;
    }
    ADC_REGS->ADC_EMR &= ~ADC_EMR_CMPMODE_Msk;
    ADC_REGS->ADC_EMR |= emr;

    while(ADC_REGS->ADC_ISR & ADC_ISR_COMPE_Msk);

    /* Comparison Restart */
    ADC_REGS->ADC_CR = 0x1U << ADC_CR_CMPRST_Pos;

    /* Enable ADC channel */
    ADC_ChannelsEnable(channelMsk);

    /* Enable Comparison Event Interrupt */
    ADC_REGS->ADC_IER |= ADC_IER_COMPE_Msk;
}

void SRV_PVDDMON_RegisterCallback (SRV_PVDDMON_CALLBACK callback_fn, uintptr_t context)
{
    /* Register ADC Callback */
    ADC_CallbackRegister(_ADC_PVDDMONCallback, context);
    ADC_CompareCallback = callback_fn;
}

bool SRV_PVDDMON_CheckComparisonInWindow(void)
{
    uint16_t adcData;
    
    adcData = (uint16_t) ((ADC_REGS->ADC_LCDR & ADC_LCDR_LDATA_Msk) >> ADC_LCDR_LDATA_Pos);
    
    if (adcData > SRV_PVDDMON_HIGH_TRESHOLD)
    {
        return false;
    }
    
    if (adcData < SRV_PVDDMON_LOW_TRESHOLD)
    {
        return false;
    }
    
    return true;
}
