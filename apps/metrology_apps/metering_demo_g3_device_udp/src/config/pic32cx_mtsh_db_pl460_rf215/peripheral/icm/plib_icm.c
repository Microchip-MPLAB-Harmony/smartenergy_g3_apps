/*******************************************************************************
  ICM Peripheral Library

  Company:
    Microchip Technology Inc.

  File Name:
    plib_icm.c

  Summary:
    Integrity Check Monitor Plib (ICM) Source File

  Description:
    None

*******************************************************************************/

/*******************************************************************************
* Copyright (C) 2025 Microchip Technology Inc. and its subsidiaries.
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

#include <stdbool.h>
#include <string.h>
#include "interrupts.h"
#include "plib_icm.h"
#include "device.h"

#define ICM_UIHVAL_WORDS_SHA1      (5U)
#define ICM_UIHVAL_WORDS_SHA256    (8U)
#define ICM_BBC_MAX                (15U)
#define ICM_BLOCK_SIZE             (64U)

static ICM_CALLBACK icmCallbackList[ICM_INTERRUPT_MAX] = {0U};

static ICM_REGION_DESCRIPTOR icmDefaultRegDescriptor[ICM_REGION_NUM] __ALIGNED(64) = 
{
    {0U, {0x10003e4}, 0U, NULL},
};

void ICM_Initialize ( void )
{
    ICM_Reset();

    // Set ICM configuration
    ICM_REGS->ICM_CFG = 0x1010000;

    // Set Default Descriptors
    ICM_REGS->ICM_DSCR = (uint32_t)&icmDefaultRegDescriptor[0];
}

void ICM_Reset ( void )
{
    ICM_REGS->ICM_CTRL = ICM_CTRL_SWRST_Msk;
}

uint32_t ICM_GetStatus(void)
{
    return ICM_REGS->ICM_SR;
}

void ICM_Enable ( void )
{
    NVIC_ClearPendingIRQ(ICM_IRQn);
    NVIC_EnableIRQ(ICM_IRQn);
    ICM_REGS->ICM_CTRL = ICM_CTRL_ENABLE_Msk;
}

void ICM_Disable ( void )
{
    NVIC_DisableIRQ(ICM_IRQn);
    ICM_REGS->ICM_CTRL = ICM_CTRL_DISABLE_Msk;

    while ((ICM_REGS->ICM_SR & ICM_SR_ENABLE_Msk) != 0U)
    {
        /* Wait for disable */
    }
}

void ICM_GetConfiguration (ICM_CONFIG *config)
{
    uint32_t regValue = ICM_REGS->ICM_CFG;
    uint32_t tmpValue = 0;

    config->disableWriteBack = (bool)((regValue & ICM_CFG_WBDIS_Msk) >> ICM_CFG_WBDIS_Pos);
    config->disableEndOfMonitoring = (bool)((regValue & ICM_CFG_EOMDIS_Msk) >> ICM_CFG_EOMDIS_Pos);
    config->disableSecondaryList = (bool)((regValue & ICM_CFG_SLBDIS_Msk) >> ICM_CFG_SLBDIS_Pos);
    config->burdenControl = (uint8_t)((regValue & ICM_CFG_BBC_Msk) >> ICM_CFG_BBC_Pos);
    config->monitorMode = (bool)((regValue & ICM_CFG_ASCD_Msk) >> ICM_CFG_ASCD_Pos);
    config->dualInputBuffer = (bool)((regValue & ICM_CFG_DUALBUFF_Msk) >> ICM_CFG_DUALBUFF_Pos);
    config->userHash = (bool)((regValue & ICM_CFG_UIHASH_Msk) >> ICM_CFG_UIHASH_Pos);
    tmpValue = (regValue & ICM_CFG_UALGO_Msk) >> ICM_CFG_UALGO_Pos;
    config->userAlgo = (ICM_ALGO)tmpValue;
    tmpValue = (regValue & ICM_CFG_HAPROT_Msk) >> ICM_CFG_HAPROT_Pos;
    config->hashAccess = (ICM_ACCESS)tmpValue;
    tmpValue = (regValue & ICM_CFG_DAPROT_Msk) >> ICM_CFG_DAPROT_Pos;
    config->descriptorAccess = (ICM_ACCESS)tmpValue;
}

void ICM_SetConfiguration (ICM_CONFIG *config)
{
    uint32_t regValue;

    regValue = ICM_CFG_WBDIS(config->disableWriteBack);
    regValue |= ICM_CFG_EOMDIS(config->disableEndOfMonitoring);
    regValue |= ICM_CFG_SLBDIS(config->disableSecondaryList);
    regValue |= ICM_CFG_BBC(config->burdenControl);
    regValue |= ICM_CFG_ASCD(config->monitorMode);
    regValue |= ICM_CFG_DUALBUFF(config->dualInputBuffer);
    regValue |= ICM_CFG_UIHASH(config->userHash);
    regValue |= ICM_CFG_UALGO(config->userAlgo);
    regValue |= ICM_CFG_HAPROT(config->hashAccess);
    regValue |= ICM_CFG_DAPROT(config->descriptorAccess);

    ICM_REGS->ICM_CFG = regValue;
}

void ICM_SetMonitorMode ( bool enable , uint8_t bbc)
{
    if (bbc > ICM_BBC_MAX)
    {
        bbc = ICM_BBC_MAX;
    }

    if (enable == true)
    {
        ICM_REGS->ICM_CFG &= ~ICM_CFG_BBC_Msk;
        ICM_REGS->ICM_CFG |= (ICM_CFG_ASCD_Msk | ICM_CFG_BBC(bbc));
    }
    else
    {
        ICM_REGS->ICM_CFG &= ~(ICM_CFG_ASCD_Msk | ICM_CFG_BBC_Msk);
    }
}

void ICM_EnableRegionMonitor ( ICM_REGION_ID regionId )
{
  if (regionId < ICM_REGION_NUM)
  {
    ICM_REGS->ICM_CTRL = ICM_CTRL_RMEN((1UL << (uint8_t)regionId));
  }
}

void ICM_DisableRegionMonitor ( ICM_REGION_ID regionId )
{
  if (regionId < ICM_REGION_NUM)
  {
    ICM_REGS->ICM_CTRL = ICM_CTRL_RMDIS((1UL << (uint8_t)regionId));
  }
}

ICM_REGION_DESCRIPTOR * ICM_GetRegionDescriptor(ICM_REGION_ID regionId)
{
    if (regionId < ICM_REGION_NUM)
    {
        return &icmDefaultRegDescriptor[regionId];
    }
    else
    {
        return NULL;
    }
}

void ICM_SetRegionDescriptor(ICM_REGION_ID regionId, ICM_REGION_DESCRIPTOR * pRegionDescriptor)
{
    if (regionId < ICM_REGION_NUM)
    {
        (void)memcpy(&icmDefaultRegDescriptor[(uint8_t)regionId], pRegionDescriptor, sizeof(ICM_REGION_DESCRIPTOR));
    }
}

void ICM_SetRegionDescriptorData(ICM_REGION_ID regionId, uint32_t * pData, size_t bytes)
{
    if ((regionId < ICM_REGION_NUM) && (bytes > 0U))
    {
        ICM_REGION_DESCRIPTOR *pRegionDescriptor = &icmDefaultRegDescriptor[regionId];
        uint32_t txferSize;

        pRegionDescriptor->startAddress = (uint32_t)pData;
        
        // ICM performs a transfer of (TRSIZE + 1) blocks of 512 bits (64 bytes)
        txferSize = (bytes >> 6U); 
        if ((bytes % ICM_BLOCK_SIZE) != 0U)
        {
            txferSize++;
        }

        pRegionDescriptor->transferSize = txferSize - 1U;
    }
}

uint32_t ICM_GetTransferSize(size_t bytes)
{
    uint32_t txferSize;

    // ICM performs a transfer of (TRSIZE + 1) blocks of 512 bits (64 bytes)
    txferSize = (bytes >> 6U); 
    if ((bytes % ICM_BLOCK_SIZE) != 0U)
    {
        txferSize++;
    }

    if (txferSize != 0U)
    {
        return (txferSize - 1U);
    }
    else
    {
        return 0U;
    }
}

void ICM_SetHashAreaAddress(uint32_t address)
{
    ICM_REGS->ICM_HASH = address;
}

void ICM_SetUserInitialHashValue(uint32_t *pValue)
{
    ICM_REGS->ICM_UIHVAL[0] = *pValue++;
    ICM_REGS->ICM_UIHVAL[1] = *pValue++;
    ICM_REGS->ICM_UIHVAL[2] = *pValue++;
    ICM_REGS->ICM_UIHVAL[3] = *pValue++;
    ICM_REGS->ICM_UIHVAL[4] = *pValue++;

    if ((ICM_REGS->ICM_CFG & ICM_CFG_UALGO_Msk) != 0U)
    {
        ICM_REGS->ICM_UIHVAL[5] = *pValue++;
        ICM_REGS->ICM_UIHVAL[6] = *pValue++;
        ICM_REGS->ICM_UIHVAL[7] = *pValue;
    }
    else
    {
        ICM_REGS->ICM_UIHVAL[5] = 0;
        ICM_REGS->ICM_UIHVAL[6] = 0;
        ICM_REGS->ICM_UIHVAL[7] = 0;
    }
}

void ICM_EnableInterrupt ( ICM_INTERRUPT_SOURCE source, ICM_REGION_ID regionId )
{
    uint32_t regValue = 0;

    if (regionId < ICM_REGION_NUM)
    {
        if (source == ICM_INTERRUPT_URAD)
        {
            regValue = ICM_IER_URAD_Msk;
        }
        else
        {
            regValue = (1UL << (uint32_t)regionId) << ((uint32_t)source << 2U);
        }

        ICM_REGS->ICM_IER = regValue;
    }
}

void ICM_DisableInterrupt ( ICM_INTERRUPT_SOURCE source, ICM_REGION_ID regionId )
{
    uint32_t regValue = 0;

    if (regionId < ICM_REGION_NUM)
    {
        if (source == ICM_INTERRUPT_URAD)
        {
            regValue = ICM_IER_URAD_Msk;
        }
        else
        {
            regValue = (1UL << (uint32_t)regionId) << ((uint32_t)source << 2U);
        }

        ICM_REGS->ICM_IDR = regValue;
    }
}

uint32_t ICM_GetIStatus(void)
{
    return ICM_REGS->ICM_ISR;
}

void ICM_CallbackRegister(ICM_INTERRUPT_SOURCE source, ICM_CALLBACK callback)
{
    icmCallbackList[source] = callback;
}

void __attribute__((used)) ICM_InterruptHandler(void)
{
    volatile uint32_t status;
    uint8_t intIndex;
    uint8_t regionIndex;
    uint8_t intStatus;
    
    status = ICM_REGS->ICM_IMR & ICM_REGS->ICM_ISR;

    if (status != 0U)
    {
        for (intIndex = (uint8_t)ICM_INTERRUPT_RHC; intIndex < (uint8_t)ICM_INTERRUPT_MAX; intIndex++)
        {
            intStatus = (uint8_t)(status & 0x0FU);
            if ((icmCallbackList[intIndex] != NULL) && (intStatus != 0U))
            {
                for (regionIndex = 0; regionIndex < (uint8_t)ICM_REGION_NUM; regionIndex++)
                {
                    if ((intStatus & 0x01U) != 0U)
                    {
                        icmCallbackList[intIndex](regionIndex);
                    }
                    intStatus = (uint8_t)(intStatus >> 1U);
                }
            }
            status = status >> 4U;
        }
    }
}
