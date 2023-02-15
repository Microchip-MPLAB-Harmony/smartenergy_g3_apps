/*******************************************************************************
Interface definition of SEFC0 PLIB.

Company:
Microchip Technology Inc.

File Name:
plib_sefc0.h

Summary:
Interface definition of SEFC0 Plib.

Description:
This file defines the interface for the SEFC0 Plib.
It allows user to Program, Erase and lock the on-chip FLASH memory.
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2022 Microchip Technology Inc. and its subsidiaries.
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

#include <string.h>
#include "device.h"
#include "plib_sefc0.h"
#include "interrupts.h"

static uint32_t sefc_status = 0;


void SEFC0_Initialize(void)
{
    SEFC0_REGS->SEFC_EEFC_FMR = SEFC_EEFC_FMR_FWS(7U) | SEFC_EEFC_FMR_CLOE_Msk | SEFC_EEFC_FMR_ALWAYS1_Msk;
}

bool SEFC0_Read( uint32_t *data, uint32_t length, uint32_t address )
{
    uint32_t *pAddress = (uint32_t*)address;
    (void)memcpy(data, pAddress, length);
    return true;
}

bool SEFC0_SectorErase( uint32_t address )
{
    uint16_t page_number;

    /* Calculate the Page number to be passed for FARG register */
    page_number = (uint16_t)((address - IFLASH0_ADDR) / IFLASH0_PAGE_SIZE);

    /* Issue the FLASH erase operation */
    SEFC0_REGS->SEFC_EEFC_FCR = (SEFC_EEFC_FCR_FCMD_EPA | SEFC_EEFC_FCR_FARG((uint32_t)page_number | 0x2U) | SEFC_EEFC_FCR_FKEY_PASSWD);

    sefc_status = 0;


    return true;
}

bool SEFC0_PageBufferWrite( uint32_t *data, const uint32_t address)
{
    uint16_t page_number;

    /* Calculate the Page number to be passed for FARG register */
    page_number = (uint16_t)((address - IFLASH0_ADDR) / IFLASH0_PAGE_SIZE);

    for (uint32_t i = 0; i < IFLASH0_PAGE_SIZE; i += 4U)
    {
        *((uint32_t *)( IFLASH0_ADDR + ( page_number * IFLASH0_PAGE_SIZE ) + i )) = *data ;
        data++;
    }

    __DSB();
    __ISB();

    return true;
}

bool SEFC0_PageBufferCommit( const uint32_t address)
{
    uint16_t page_number;

    /* Calculate the Page number to be passed for FARG register */
    page_number = (uint16_t)((address - IFLASH0_ADDR) / IFLASH0_PAGE_SIZE);

    __DSB();
    __ISB();

    /* Issue the FLASH write operation*/
    SEFC0_REGS->SEFC_EEFC_FCR = (SEFC_EEFC_FCR_FCMD_WP | SEFC_EEFC_FCR_FARG((uint32_t)page_number) | SEFC_EEFC_FCR_FKEY_PASSWD);

    sefc_status = 0;


    return true;
}

bool SEFC0_PageWrite( uint32_t *data, uint32_t address )
{
    uint16_t page_number;

    /* Calculate the Page number to be passed for FARG register */
    page_number = (uint16_t)((address - IFLASH0_ADDR) / IFLASH0_PAGE_SIZE);

    for (uint32_t i = 0; i < IFLASH0_PAGE_SIZE; i += 4U)
    {
        *((uint32_t *)( IFLASH0_ADDR + ( page_number * IFLASH0_PAGE_SIZE ) + i )) = *data;
        data++;
    }

    __DSB();
    __ISB();

    /* Issue the FLASH write operation*/
    SEFC0_REGS->SEFC_EEFC_FCR = (SEFC_EEFC_FCR_FCMD_WP | SEFC_EEFC_FCR_FARG((uint32_t)page_number) | SEFC_EEFC_FCR_FKEY_PASSWD);

    sefc_status = 0;


    return true;
}

bool SEFC0_QuadWordWrite( uint32_t *data, uint32_t address )
{
    uint16_t page_number;

    /* Calculate the Page number to be passed for FARG register */
    page_number = (uint16_t)((address - IFLASH0_ADDR) / IFLASH0_PAGE_SIZE);

    for (uint32_t i = 0; i < 16U; i += 4U)
    {
        *((uint32_t *)(( address ) + i )) = *data;
        data++;
    }
    /* Issue the FLASH write operation */
    SEFC0_REGS->SEFC_EEFC_FCR = (SEFC_EEFC_FCR_FCMD_WP | SEFC_EEFC_FCR_FARG((uint32_t)page_number) | SEFC_EEFC_FCR_FKEY_PASSWD);

    sefc_status = 0;


    return true;
}

void SEFC0_RegionLock(uint32_t address)
{
    uint16_t page_number;

    /*Calculate the Page number to be passed for FARG register*/
    page_number = (uint16_t)((address - IFLASH0_ADDR) / IFLASH0_PAGE_SIZE);
    SEFC0_REGS->SEFC_EEFC_FCR = (SEFC_EEFC_FCR_FCMD_SLB | SEFC_EEFC_FCR_FARG((uint32_t)page_number) | SEFC_EEFC_FCR_FKEY_PASSWD);

    sefc_status = 0;

}

void SEFC0_RegionUnlock(uint32_t address)
{
    uint16_t page_number;

    /*Calculate the Page number to be passed for FARG register*/
    page_number = (uint16_t)((address - IFLASH0_ADDR) / IFLASH0_PAGE_SIZE);
    SEFC0_REGS->SEFC_EEFC_FCR = (SEFC_EEFC_FCR_FCMD_CLB | SEFC_EEFC_FCR_FARG((uint32_t)page_number) | SEFC_EEFC_FCR_FKEY_PASSWD);

    sefc_status = 0;

}

bool SEFC0_IsBusy(void)
{
    sefc_status |= SEFC0_REGS->SEFC_EEFC_FSR;
    return (bool)((sefc_status & SEFC_EEFC_FSR_FRDY_Msk) == 0U);
}

SEFC_ERROR SEFC0_ErrorGet( void )
{
    sefc_status |= SEFC0_REGS->SEFC_EEFC_FSR;
    return (SEFC_ERROR)sefc_status;
}


