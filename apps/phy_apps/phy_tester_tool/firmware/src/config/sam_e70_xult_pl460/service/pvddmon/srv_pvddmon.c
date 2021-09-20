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
#include "peripheral/afec/plib_afec1.h"

static SRV_PVDDMON_CMP_MODE srv_pvddmon_mode;

// *****************************************************************************
// *****************************************************************************
// Section: PLC PVDD Monitor Service Implementation
// *****************************************************************************
// *****************************************************************************
/* Objects to hold callback function and context */
static SRV_PVDDMON_CALLBACK AFEC1_CompareCallback = NULL;

static void _AFEC1_PVDDMONCallback( uint32_t status, uintptr_t context )
{
    if (status & AFEC_ISR_COMPE_Msk)
    {
        if (AFEC1_CompareCallback)
        {
            AFEC1_CompareCallback(srv_pvddmon_mode, context);
        }
    }
}

/* Start PLC PVDD Monitor */
void SRV_PVDDMON_Initialize (void)
{
    AFEC_CHANNEL_MASK channelMsk = (1 << 6);

    /* Disable AFEC1 channel */
    AFEC1_ChannelsDisable(channelMsk);

    /* Disable channel interrupt */
    AFEC1_ChannelsInterruptDisable(channelMsk);
}

/* Start PLC PVDD Monitor */
void SRV_PVDDMON_Start (SRV_PVDDMON_CMP_MODE cmpMode)
{
    uint32_t emr = 0;
    AFEC_CHANNEL_MASK channelMsk = (1 << 6);

    /* Set Free Run reset */
    AFEC1_REGS->AFEC_MR |= AFEC_MR_FREERUN_Msk;

    /* Set Compare Window Register */
    AFEC1_REGS->AFEC_CWR = AFEC_CWR_HIGHTHRES(SRV_PVDDMON_HIGH_TRESHOLD) | AFEC_CWR_LOWTHRES(SRV_PVDDMON_LOW_TRESHOLD); 

    /* Set Comparison Mode */
    if (cmpMode == SRV_PVDDMON_CMP_MODE_OUT)
    {
        srv_pvddmon_mode = SRV_PVDDMON_CMP_MODE_OUT;
        emr |= AFEC_EMR_CMPMODE_OUT;
    }
    else
    {
        srv_pvddmon_mode = SRV_PVDDMON_CMP_MODE_IN;
        emr |= AFEC_EMR_CMPMODE_IN;
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

/* Restart PLC PVDD Monitor */
void SRV_PVDDMON_Restart (SRV_PVDDMON_CMP_MODE cmpMode)
{
    uint32_t emr;
    AFEC_CHANNEL_MASK channelMsk = (1 << 6);

    /* Disable channel COMPE interrupt */
    AFEC1_REGS->AFEC_IDR |= AFEC_IER_COMPE_Msk;

    /* Disable AFEC1 channel */
    AFEC1_ChannelsDisable(channelMsk);

    /* Set Comparison Mode */
    if (cmpMode == SRV_PVDDMON_CMP_MODE_OUT)
    {
      srv_pvddmon_mode = SRV_PVDDMON_CMP_MODE_OUT;
      emr = AFEC_EMR_CMPMODE_OUT;
    }
    else
    {
      srv_pvddmon_mode = SRV_PVDDMON_CMP_MODE_IN;
      emr = AFEC_EMR_CMPMODE_IN;
    }
    AFEC1_REGS->AFEC_EMR &= ~AFEC_EMR_CMPMODE_Msk;
    AFEC1_REGS->AFEC_EMR |= emr;

    while(AFEC1_REGS->AFEC_ISR & AFEC_ISR_COMPE_Msk);

    /* Enable AFEC1 channel */
    AFEC1_ChannelsEnable(channelMsk);

    /* Enable Comparison Event Interrupt */
    AFEC1_REGS->AFEC_IER |= AFEC_IER_COMPE_Msk;
}

void SRV_PVDDMON_RegisterCallback (SRV_PVDDMON_CALLBACK callback_fn, uintptr_t context)
{
    /* Register AFEC1 Callback */
    AFEC1_CallbackRegister(_AFEC1_PVDDMONCallback, context);
    AFEC1_CompareCallback = callback_fn;
}

bool SRV_PVDDMON_CheckComparisonInWindow(void)
{
    uint16_t adcData;
    
    adcData = (uint16_t) ((AFEC1_REGS->AFEC_LCDR & AFEC_LCDR_LDATA_Msk) >> AFEC_LCDR_LDATA_Pos);
    
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
