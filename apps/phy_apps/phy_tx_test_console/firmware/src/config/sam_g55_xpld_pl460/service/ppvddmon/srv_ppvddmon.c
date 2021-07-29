/*******************************************************************************
  PLC PVDD Monitor Service Library

  Company:
    Microchip Technology Inc.

  File Name:
    srv_ppvddmon.c

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
#include "srv_ppvddmon.h"
#include "peripheral/adc/plib_adc.h"

static SRV_PVDDMON_CMP_MODE srv_pvddmon_mode;

// *****************************************************************************
// *****************************************************************************
// Section: PLC PVDD Monitor Service Implementation
// *****************************************************************************
// *****************************************************************************
/* Objects to hold callback function and context */
static SRV_PVDDMON_CALLBACK ADC_CompareCallback = NULL;

static void _ADC_PVDDMONCallback ( uint32_t status, uintptr_t context )
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
void SRV_PPVDDMON_Initialize (void)
{
    ADC_CHANNEL_MASK channelMsk = (1 << 0);

    /* Disable ADC channel */
    ADC_ChannelsDisable(channelMsk);

    /* Disable channel interrupt */
    ADC_ChannelsInterruptDisable(channelMsk);
}

/* Start PLC PVDD Monitor */
void SRV_PPVDDMON_Start (SRV_PVDDMON_CMP_MODE cmpMode)
{
    ADC_CHANNEL_MASK channelMsk = (1 << 0);

    /* Set Free Run reset */
    ADC_REGS->ADC_MR |= ADC_MR_FREERUN_Msk;

    /* Set Compare Window Register */
    ADC_REGS->ADC_CWR = ADC_CWR_HIGHTHRES(SRV_PVDDMON_HIGH_TRESHOLD) || ADC_CWR_LOWTHRES(SRV_PVDDMON_LOW_TRESHOLD); 

    /* Set Comparison Mode */
    ADC_REGS->ADC_EMR &= ~ADC_EMR_CMPMODE_Msk;
    if (cmpMode == SRV_PVDDMON_CMP_MODE_OUT)
    {
      srv_pvddmon_mode = SRV_PVDDMON_CMP_MODE_OUT;
      ADC_REGS->ADC_EMR |= ADC_EMR_CMPMODE_OUT;
    }
    else
    {
      srv_pvddmon_mode = SRV_PVDDMON_CMP_MODE_IN;
      ADC_REGS->ADC_EMR |= ADC_EMR_CMPMODE_IN;
    }

    /* Clear Comparison Type */
    ADC_REGS->ADC_EMR &= ~(1 << 2);

    /* Clear Compare All and Compare Filter fields */
    ADC_REGS->ADC_EMR &= ~(ADC_EMR_CMPALL_Msk || ADC_EMR_CMPFILTER_Msk);

    /* Set Comparison Selected Channel */
    ADC_REGS->ADC_EMR &= ~ADC_EMR_CMPSEL_Msk;
    ADC_REGS->ADC_EMR |=  ADC_EMR_CMPSEL(0);

    /* Enable Comparison Event Interrupt */
    ADC_REGS->ADC_IER |= ADC_IER_COMPE_Msk;

    /* Enable ADC channel */
    ADC_ChannelsEnable(channelMsk);

    /* Start ADC conversion */
    ADC_ConversionStart();

}

/* Start PLC PVDD Monitor */
void SRV_PPVDDMON_Restart (SRV_PVDDMON_CMP_MODE cmpMode)
{
    ADC_CHANNEL_MASK channelMsk = (1 << 0);

    /* Disable ADC channel */
    ADC_ChannelsDisable(channelMsk);

    /* Disable channel COMPE interrupt */
    ADC_REGS->ADC_IDR |= ADC_IER_COMPE_Msk;

    /* Set Comparison Mode */
    ADC_REGS->ADC_EMR &= ~ADC_EMR_CMPMODE_Msk;
    if (cmpMode == SRV_PVDDMON_CMP_MODE_OUT)
    {
      srv_pvddmon_mode = SRV_PVDDMON_CMP_MODE_OUT;
      ADC_REGS->ADC_EMR |= ADC_EMR_CMPMODE_OUT;
    }
    else
    {
      srv_pvddmon_mode = SRV_PVDDMON_CMP_MODE_IN;
      ADC_REGS->ADC_EMR |= ADC_EMR_CMPMODE_IN;
    }

    /* Enable Comparison Event Interrupt */
    ADC_REGS->ADC_IER |= ADC_IER_COMPE_Msk;

    /* Enable ADC channel */
    ADC_ChannelsEnable(channelMsk);

    /* Start ADC conversion */
    ADC_ConversionStart();

}

void SRV_PPVDDMON_RegisterCallback (SRV_PVDDMON_CALLBACK callback_fn, uintptr_t context)
{
    /* Register ADC Callback */
    ADC_CallbackRegister(_ADC_PVDDMONCallback, context);
    ADC_CompareCallback = callback_fn;
}
