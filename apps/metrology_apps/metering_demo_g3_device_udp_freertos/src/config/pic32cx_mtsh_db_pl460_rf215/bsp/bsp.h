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
#define PIC32CXMTSH_DB
#define BOARD_NAME    "PIC32CXMTSH-DB"

/*** OUTPUT PIO Macros for RF215_LED_RX ***/
#define BSP_RF215_LED_RX_PIN        PIO_PIN_PC20
#define BSP_RF215_LED_RX_Get()      ((PIOC_REGS->PIO_PDSR >> 20) & 0x1)
#define BSP_RF215_LED_RX_On()       (PIOC_REGS->PIO_SODR = (1UL<<20))
#define BSP_RF215_LED_RX_Off()      (PIOC_REGS->PIO_CODR = (1UL<<20))
#define BSP_RF215_LED_RX_Toggle()   do {\
                                    PIOC_REGS->PIO_MSKR = (1<<20); \
                                    PIOC_REGS->PIO_ODSR ^= (1<<20);\
                                } while (0)

/*** OUTPUT PIO Macros for RF215_LED_TX ***/
#define BSP_RF215_LED_TX_PIN        PIO_PIN_PC21
#define BSP_RF215_LED_TX_Get()      ((PIOC_REGS->PIO_PDSR >> 21) & 0x1)
#define BSP_RF215_LED_TX_On()       (PIOC_REGS->PIO_SODR = (1UL<<21))
#define BSP_RF215_LED_TX_Off()      (PIOC_REGS->PIO_CODR = (1UL<<21))
#define BSP_RF215_LED_TX_Toggle()   do {\
                                    PIOC_REGS->PIO_MSKR = (1<<21); \
                                    PIOC_REGS->PIO_ODSR ^= (1<<21);\
                                } while (0)

/*** OUTPUT PIO Macros for RF215_RSTN ***/
#define BSP_RF215_RSTN_PIN        PIO_PIN_PB26
#define BSP_RF215_RSTN_Get()      ((PIOB_REGS->PIO_PDSR >> 26) & 0x1)
#define BSP_RF215_RSTN_On()       (PIOB_REGS->PIO_SODR = (1UL<<26))
#define BSP_RF215_RSTN_Off()      (PIOB_REGS->PIO_CODR = (1UL<<26))
#define BSP_RF215_RSTN_Toggle()   do {\
                                    PIOB_REGS->PIO_MSKR = (1<<26); \
                                    PIOB_REGS->PIO_ODSR ^= (1<<26);\
                                } while (0)


/*** INPUT PIO Macros for PL460_NTHW0 ***/
#define BSP_PL460_NTHW0_PIN                    PIO_PIN_PA2
#define BSP_PL460_NTHW0_Get()                  ((PIOA_REGS->PIO_PDSR >> 2) & 0x1)
#define BSP_PL460_NTHW0_STATE_PRESSED          0
#define BSP_PL460_NTHW0_STATE_RELEASED         1
#define BSP_PL460_NTHW0_InterruptEnable()      (PIOA_REGS->PIO_IER = (1UL<<2))
#define BSP_PL460_NTHW0_InterruptDisable()     (PIOA_REGS->PIO_IDR = (1UL<<2))

/*** INPUT PIO Macros for PL460_EXTINT ***/
#define BSP_PL460_EXTINT_PIN                    PIO_PIN_PA3
#define BSP_PL460_EXTINT_Get()                  ((PIOA_REGS->PIO_PDSR >> 3) & 0x1)
#define BSP_PL460_EXTINT_STATE_PRESSED          1
#define BSP_PL460_EXTINT_STATE_RELEASED         0
#define BSP_PL460_EXTINT_InterruptEnable()      (PIOA_REGS->PIO_IER = (1UL<<3))
#define BSP_PL460_EXTINT_InterruptDisable()     (PIOA_REGS->PIO_IDR = (1UL<<3))

/*** INPUT PIO Macros for SCRL_UP_BTN ***/
#define BSP_SCRL_UP_BTN_PIN                    PIO_PIN_PA14
#define BSP_SCRL_UP_BTN_Get()                  ((PIOA_REGS->PIO_PDSR >> 14) & 0x1)
#define BSP_SCRL_UP_BTN_STATE_PRESSED          0
#define BSP_SCRL_UP_BTN_STATE_RELEASED         1
#define BSP_SCRL_UP_BTN_InterruptEnable()      (PIOA_REGS->PIO_IER = (1UL<<14))
#define BSP_SCRL_UP_BTN_InterruptDisable()     (PIOA_REGS->PIO_IDR = (1UL<<14))

/*** INPUT PIO Macros for SCRL_DOWN_BTN ***/
#define BSP_SCRL_DOWN_BTN_PIN                    PIO_PIN_PA15
#define BSP_SCRL_DOWN_BTN_Get()                  ((PIOA_REGS->PIO_PDSR >> 15) & 0x1)
#define BSP_SCRL_DOWN_BTN_STATE_PRESSED          0
#define BSP_SCRL_DOWN_BTN_STATE_RELEASED         1
#define BSP_SCRL_DOWN_BTN_InterruptEnable()      (PIOA_REGS->PIO_IER = (1UL<<15))
#define BSP_SCRL_DOWN_BTN_InterruptDisable()     (PIOA_REGS->PIO_IDR = (1UL<<15))

/*** INPUT PIO Macros for RF215_IRQ ***/
#define BSP_RF215_IRQ_PIN                    PIO_PIN_PB25
#define BSP_RF215_IRQ_Get()                  ((PIOB_REGS->PIO_PDSR >> 25) & 0x1)
#define BSP_RF215_IRQ_STATE_PRESSED          0
#define BSP_RF215_IRQ_STATE_RELEASED         1
#define BSP_RF215_IRQ_InterruptEnable()      (PIOB_REGS->PIO_IER = (1UL<<25))
#define BSP_RF215_IRQ_InterruptDisable()     (PIOB_REGS->PIO_IDR = (1UL<<25))



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