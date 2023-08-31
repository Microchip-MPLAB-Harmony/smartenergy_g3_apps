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
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "device.h"
#include "interrupts.h"
#include "srv_pvddmon.h"
#include "peripheral/afec/plib_afec1.h"

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Data
// *****************************************************************************
// *****************************************************************************

static SRV_PVDDMON_CMP_MODE srv_pvddmon_mode;
static SRV_PVDDMON_CALLBACK AFEC1_CompareCallback = NULL;


// *****************************************************************************
// *****************************************************************************
// Section: File Scope Functions
// *****************************************************************************
// *****************************************************************************

static void lAFEC1_PVDDMONCallback( uint32_t status, uintptr_t context )
{
    if ((status & AFEC_ISR_COMPE_Msk) != 0U)
    {
        if (AFEC1_CompareCallback != NULL)
        {
            AFEC1_CompareCallback(srv_pvddmon_mode, context);
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
    AFEC_CHANNEL_MASK channelMsk = AFEC_CH6_MASK;

    /* Disable AFEC1 channel */
    AFEC1_ChannelsDisable(channelMsk);

    /* Disable channel interrupt */
    AFEC1_ChannelsInterruptDisable((AFEC_INTERRUPT_MASK)channelMsk);
}

void SRV_PVDDMON_Start (SRV_PVDDMON_CMP_MODE cmpMode)
{
    uint32_t emr = 0;
    AFEC_CHANNEL_MASK channelMsk = AFEC_CH6_MASK;

    /* Set Free Run reset */
    AFEC1_REGS->AFEC_MR |= AFEC_MR_FREERUN_Msk;

    /* Set Comparison Mode */
    if (cmpMode == SRV_PVDDMON_CMP_MODE_OUT)
    {
        srv_pvddmon_mode = SRV_PVDDMON_CMP_MODE_OUT;
        emr |= AFEC_EMR_CMPMODE_OUT;
        /* Set Compare Window Register */
        AFEC1_REGS->AFEC_CWR = AFEC_CWR_HIGHTHRES(SRV_PVDDMON_HIGH_TRESHOLD) | AFEC_CWR_LOWTHRES(SRV_PVDDMON_LOW_TRESHOLD);
    }
    else
    {
        srv_pvddmon_mode = SRV_PVDDMON_CMP_MODE_IN;
        emr |= AFEC_EMR_CMPMODE_IN;
        /* Set Compare Window Register */
        AFEC1_REGS->AFEC_CWR = AFEC_CWR_HIGHTHRES(SRV_PVDDMON_HIGH_TRESHOLD_HYST) | AFEC_CWR_LOWTHRES(SRV_PVDDMON_LOW_TRESHOLD_HYST); 
    }

    /* Set Comparison Selected Channel */
    emr |= AFEC_EMR_CMPSEL(6);

    /* Set Filter */
    emr |= AFEC_EMR_CMPFILTER(3);
    AFEC1_REGS->AFEC_EMR = emr;

    /* Enable Comparison Event Interrupt */
    AFEC1_REGS->AFEC_IER |= AFEC_IER_COMPE_Msk;

    /* Enable AFEC1 channel */
    AFEC1_ChannelsEnable(channelMsk);

    /* Start AFEC1 conversion */
    AFEC1_ConversionStart();
}

void SRV_PVDDMON_Restart (SRV_PVDDMON_CMP_MODE cmpMode)
{
    uint32_t emr;
    AFEC_CHANNEL_MASK channelMsk = AFEC_CH6_MASK;

    /* Disable channel COMPE interrupt */
    AFEC1_REGS->AFEC_IDR |= AFEC_IER_COMPE_Msk;

    /* Disable AFEC1 channel */
    AFEC1_ChannelsDisable(channelMsk);

    /* Set Comparison Mode */
    if (cmpMode == SRV_PVDDMON_CMP_MODE_OUT)
    {
        srv_pvddmon_mode = SRV_PVDDMON_CMP_MODE_OUT;
        emr = AFEC_EMR_CMPMODE_OUT;
        /* Set Compare Window Register */
        AFEC1_REGS->AFEC_CWR = AFEC_CWR_HIGHTHRES(SRV_PVDDMON_HIGH_TRESHOLD) | AFEC_CWR_LOWTHRES(SRV_PVDDMON_LOW_TRESHOLD);
    }
    else
    {
        srv_pvddmon_mode = SRV_PVDDMON_CMP_MODE_IN;
        emr = AFEC_EMR_CMPMODE_IN;
        /* Set Compare Window Register */
        AFEC1_REGS->AFEC_CWR = AFEC_CWR_HIGHTHRES(SRV_PVDDMON_HIGH_TRESHOLD_HYST) | AFEC_CWR_LOWTHRES(SRV_PVDDMON_LOW_TRESHOLD_HYST); 
    }
    AFEC1_REGS->AFEC_EMR &= ~AFEC_EMR_CMPMODE_Msk;
    AFEC1_REGS->AFEC_EMR |= emr;

    while((AFEC1_REGS->AFEC_ISR & AFEC_ISR_COMPE_Msk) != 0U){}

    /* Enable AFEC1 channel */
    AFEC1_ChannelsEnable(channelMsk);

    /* Enable Comparison Event Interrupt */
    AFEC1_REGS->AFEC_IER |= AFEC_IER_COMPE_Msk;
}

void SRV_PVDDMON_CallbackRegister (SRV_PVDDMON_CALLBACK callback, uintptr_t context)
{
    /* Register AFEC1 Callback */
    AFEC1_CallbackRegister(lAFEC1_PVDDMONCallback, context);
    AFEC1_CompareCallback = callback;
}

bool SRV_PVDDMON_CheckWindow(void)
{
    uint32_t adcValue;
    
    adcValue = AFEC1_ChannelResultGet(AFEC_CH6);
    while(adcValue == 0U)
    {
        adcValue = AFEC1_ChannelResultGet(AFEC_CH6);
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