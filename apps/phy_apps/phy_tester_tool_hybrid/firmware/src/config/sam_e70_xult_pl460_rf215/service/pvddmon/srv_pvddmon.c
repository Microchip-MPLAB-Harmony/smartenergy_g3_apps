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