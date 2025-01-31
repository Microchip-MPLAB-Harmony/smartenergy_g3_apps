/*******************************************************************************
  Board Support Package Header File.

  Company:
    Microchip Technology Inc.

  File Name:
    bsp.h

  Summary:
    Board Support Package Header File 

  Description:
    This file contains constants, macros, type definitions and function
    declarations 
*******************************************************************************/

// DOM-IGNORE-BEGIN
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
// DOM-IGNORE-END

#ifndef BSP_H
#define BSP_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "device.h"
#include "peripheral/pio/plib_pio.h"

// *****************************************************************************
// *****************************************************************************
// Section: BSP Macros
// *****************************************************************************
// *****************************************************************************
#define SAME70_XPLAINED_ULTRA
#define BOARD_NAME    "SAME70-XPLAINED-ULTRA"

/*** OUTPUT PIO Macros for PL460_ENABLE ***/
#define BSP_PL460_ENABLE_PIN        PIO_PIN_PC30
#define BSP_PL460_ENABLE_Get()      ((PIOC_REGS->PIO_PDSR >> 30) & 0x1)
#define BSP_PL460_ENABLE_On()       (PIOC_REGS->PIO_CODR = (1UL<<30))
#define BSP_PL460_ENABLE_Off()      (PIOC_REGS->PIO_SODR = (1UL<<30))
#define BSP_PL460_ENABLE_Toggle()   (PIOC_REGS->PIO_ODSR ^= (1UL<<30))

/*** OUTPUT PIO Macros for LED1 ***/
#define BSP_LED1_PIN        PIO_PIN_PA5
#define BSP_LED1_Get()      ((PIOA_REGS->PIO_PDSR >> 5) & 0x1)
#define BSP_LED1_On()       (PIOA_REGS->PIO_CODR = (1UL<<5))
#define BSP_LED1_Off()      (PIOA_REGS->PIO_SODR = (1UL<<5))
#define BSP_LED1_Toggle()   (PIOA_REGS->PIO_ODSR ^= (1UL<<5))

/*** OUTPUT PIO Macros for PL460_TXEN ***/
#define BSP_PL460_TXEN_PIN        PIO_PIN_PA4
#define BSP_PL460_TXEN_Get()      ((PIOA_REGS->PIO_PDSR >> 4) & 0x1)
#define BSP_PL460_TXEN_On()       (PIOA_REGS->PIO_SODR = (1UL<<4))
#define BSP_PL460_TXEN_Off()      (PIOA_REGS->PIO_CODR = (1UL<<4))
#define BSP_PL460_TXEN_Toggle()   (PIOA_REGS->PIO_ODSR ^= (1UL<<4))

/*** OUTPUT PIO Macros for PL460_STBY ***/
#define BSP_PL460_STBY_PIN        PIO_PIN_PA3
#define BSP_PL460_STBY_Get()      ((PIOA_REGS->PIO_PDSR >> 3) & 0x1)
#define BSP_PL460_STBY_On()       (PIOA_REGS->PIO_SODR = (1UL<<3))
#define BSP_PL460_STBY_Off()      (PIOA_REGS->PIO_CODR = (1UL<<3))
#define BSP_PL460_STBY_Toggle()   (PIOA_REGS->PIO_ODSR ^= (1UL<<3))

/*** OUTPUT PIO Macros for PL460_NRST ***/
#define BSP_PL460_NRST_PIN        PIO_PIN_PA0
#define BSP_PL460_NRST_Get()      ((PIOA_REGS->PIO_PDSR >> 0) & 0x1)
#define BSP_PL460_NRST_On()       (PIOA_REGS->PIO_SODR = (1UL<<0))
#define BSP_PL460_NRST_Off()      (PIOA_REGS->PIO_CODR = (1UL<<0))
#define BSP_PL460_NRST_Toggle()   (PIOA_REGS->PIO_ODSR ^= (1UL<<0))


/*** INPUT PIO Macros for PL460_EXTINT ***/
#define BSP_PL460_EXTINT_PIN                    PIO_PIN_PD28
#define BSP_PL460_EXTINT_Get()                  ((PIOD_REGS->PIO_PDSR >> 28) & 0x1)
#define BSP_PL460_EXTINT_STATE_PRESSED          1
#define BSP_PL460_EXTINT_STATE_RELEASED         0
#define BSP_PL460_EXTINT_InterruptEnable()      (PIOD_REGS->PIO_IER = (1UL<<28))
#define BSP_PL460_EXTINT_InterruptDisable()     (PIOD_REGS->PIO_IDR = (1UL<<28))

/*** INPUT PIO Macros for PL460_NTHW0 ***/
#define BSP_PL460_NTHW0_PIN                    PIO_PIN_PC17
#define BSP_PL460_NTHW0_Get()                  ((PIOC_REGS->PIO_PDSR >> 17) & 0x1)
#define BSP_PL460_NTHW0_STATE_PRESSED          0
#define BSP_PL460_NTHW0_STATE_RELEASED         1
#define BSP_PL460_NTHW0_InterruptEnable()      (PIOC_REGS->PIO_IER = (1UL<<17))
#define BSP_PL460_NTHW0_InterruptDisable()     (PIOC_REGS->PIO_IDR = (1UL<<17))

/*** INPUT PIO Macros for USB_VBUS_SENSE ***/
#define BSP_USB_VBUS_SENSE_PIN                    PIO_PIN_PB8
#define BSP_USB_VBUS_SENSE_Get()                  ((PIOB_REGS->PIO_PDSR >> 8) & 0x1)
#define BSP_USB_VBUS_SENSE_STATE_PRESSED          1
#define BSP_USB_VBUS_SENSE_STATE_RELEASED         0
#define BSP_USB_VBUS_SENSE_InterruptEnable()      (PIOB_REGS->PIO_IER = (1UL<<8))
#define BSP_USB_VBUS_SENSE_InterruptDisable()     (PIOB_REGS->PIO_IDR = (1UL<<8))



// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    void BSP_Initialize(void)

  Summary:
    Performs the necessary actions to initialize a board

  Description:
    This function initializes the LED and Switch ports on the board.  This
    function must be called by the user before using any APIs present on this
    BSP.

  Precondition:
    None.

  Parameters:
    None

  Returns:
    None.

  Example:
    <code>
    BSP_Initialize();
    </code>

  Remarks:
    None
*/

void BSP_Initialize(void);

#endif // BSP_H

/*******************************************************************************
 End of File
*/
