/*******************************************************************************
  WDT Peripheral Library

  Company:
    Microchip Technology Inc.

  File Name:
    plib_wdt.c

  Summary:
    WDT Source File

  Description:
    None

*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
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

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "device.h"
#include "plib_wdt.h"

// *****************************************************************************
// *****************************************************************************
// Section: WDT Implementation
// *****************************************************************************
// *****************************************************************************

void WDT_Enable( void )
{
    /* ON = 1 */
    WDT_REGS->WDT_WDTCONSET = WDT_WDTCON_ON_Msk;
}

void WDT_Disable( void )
{
    /* ON = 0 */
    WDT_REGS->WDT_WDTCONCLR = WDT_WDTCON_ON_Msk;
}

bool WDT_IsEnabled( void )
{
    return((WDT_REGS->WDT_WDTCON & WDT_WDTCON_ON_Msk) == WDT_WDTCON_ON_Msk ? true : false);
}

void WDT_WindowEnable( void )
{
    /* WDTWINEN = 1 */
    WDT_REGS->WDT_WDTCONSET = WDT_WDTCON_WDTWINEN_Msk;
}

void WDT_WindowDisable( void )
{
    /* WDTWINEN = 0 */
    WDT_REGS->WDT_WDTCONCLR = WDT_WDTCON_WDTWINEN_Msk;
}

bool WDT_IsWindowEnabled( void )
{
    return((WDT_REGS->WDT_WDTCON & WDT_WDTCON_WDTWINEN_Msk) == WDT_WDTCON_WDTWINEN_Msk ? true : false);
}

void WDT_Clear( void )
{
    /* Writing specific value to only upper 16 bits of WDTCON register clears WDT counter */
    /* Only write to the upper 16 bits of the register when clearing. */
    /* WDTCLRKEY = 0x5743 */
    const uint32_t WDT_CLR_REG_ADDRESS = (WDT_BASE_ADDRESS + WDT_WDTCON_REG_OFST + 2U);
    *((volatile uint16_t *)WDT_CLR_REG_ADDRESS) = 0x5743U;
}