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
#include "peripheral/port/plib_port.h"

// *****************************************************************************
// *****************************************************************************
// Section: BSP Macros
// *****************************************************************************
// *****************************************************************************
#define SAME54_XPLAINED_PRO
#define BOARD_NAME    "SAME54-XPLAINED-PRO"

/*** Macros for GPIO_PB06 output pin ***/ 
#define BSP_GPIO_PB06_PIN        PORT_PIN_PB6
#define BSP_GPIO_PB06_Get()      ((PORT_REGS->GROUP[1].PORT_IN >> 6U) & 0x01U)
#define BSP_GPIO_PB06_Set()      (PORT_REGS->GROUP[1].PORT_OUTSET = ((uint32_t)1U << 6U))
#define BSP_GPIO_PB06_Clear()    (PORT_REGS->GROUP[1].PORT_OUTCLR = ((uint32_t)1U << 6U))
#define BSP_GPIO_PB06_Toggle()   (PORT_REGS->GROUP[1].PORT_OUTTGL = ((uint32_t)1U << 6U))
#define BSP_GPIO_PB06_On()       BSP_GPIO_PB06_Set()
#define BSP_GPIO_PB06_Off()      BSP_GPIO_PB06_Clear() 

/*** Macros for PL460_NRST output pin ***/ 
#define BSP_PL460_NRST_PIN        PORT_PIN_PB8
#define BSP_PL460_NRST_Get()      ((PORT_REGS->GROUP[1].PORT_IN >> 8U) & 0x01U)
#define BSP_PL460_NRST_Set()      (PORT_REGS->GROUP[1].PORT_OUTSET = ((uint32_t)1U << 8U))
#define BSP_PL460_NRST_Clear()    (PORT_REGS->GROUP[1].PORT_OUTCLR = ((uint32_t)1U << 8U))
#define BSP_PL460_NRST_Toggle()   (PORT_REGS->GROUP[1].PORT_OUTTGL = ((uint32_t)1U << 8U))
#define BSP_PL460_NRST_On()       BSP_PL460_NRST_Set()
#define BSP_PL460_NRST_Off()      BSP_PL460_NRST_Clear() 

/*** Macros for PL460_ENABLE output pin ***/ 
#define BSP_PL460_ENABLE_PIN        PORT_PIN_PB9
#define BSP_PL460_ENABLE_Get()      ((PORT_REGS->GROUP[1].PORT_IN >> 9U) & 0x01U)
#define BSP_PL460_ENABLE_Set()      (PORT_REGS->GROUP[1].PORT_OUTSET = ((uint32_t)1U << 9U))
#define BSP_PL460_ENABLE_Clear()    (PORT_REGS->GROUP[1].PORT_OUTCLR = ((uint32_t)1U << 9U))
#define BSP_PL460_ENABLE_Toggle()   (PORT_REGS->GROUP[1].PORT_OUTTGL = ((uint32_t)1U << 9U))
#define BSP_PL460_ENABLE_On()       BSP_PL460_ENABLE_Clear()
#define BSP_PL460_ENABLE_Off()      BSP_PL460_ENABLE_Set() 

/*** Macros for EXT2_SPI_CS output pin ***/ 
#define BSP_EXT2_SPI_CS_PIN        PORT_PIN_PC6
#define BSP_EXT2_SPI_CS_Get()      ((PORT_REGS->GROUP[2].PORT_IN >> 6U) & 0x01U)
#define BSP_EXT2_SPI_CS_Set()      (PORT_REGS->GROUP[2].PORT_OUTSET = ((uint32_t)1U << 6U))
#define BSP_EXT2_SPI_CS_Clear()    (PORT_REGS->GROUP[2].PORT_OUTCLR = ((uint32_t)1U << 6U))
#define BSP_EXT2_SPI_CS_Toggle()   (PORT_REGS->GROUP[2].PORT_OUTTGL = ((uint32_t)1U << 6U))
#define BSP_EXT2_SPI_CS_On()       BSP_EXT2_SPI_CS_Clear()
#define BSP_EXT2_SPI_CS_Off()      BSP_EXT2_SPI_CS_Set() 

/*** Macros for RF215_RSTN output pin ***/ 
#define BSP_RF215_RSTN_PIN        PORT_PIN_PB14
#define BSP_RF215_RSTN_Get()      ((PORT_REGS->GROUP[1].PORT_IN >> 14U) & 0x01U)
#define BSP_RF215_RSTN_Set()      (PORT_REGS->GROUP[1].PORT_OUTSET = ((uint32_t)1U << 14U))
#define BSP_RF215_RSTN_Clear()    (PORT_REGS->GROUP[1].PORT_OUTCLR = ((uint32_t)1U << 14U))
#define BSP_RF215_RSTN_Toggle()   (PORT_REGS->GROUP[1].PORT_OUTTGL = ((uint32_t)1U << 14U))
#define BSP_RF215_RSTN_On()       BSP_RF215_RSTN_Clear()
#define BSP_RF215_RSTN_Off()      BSP_RF215_RSTN_Set() 

/*** Macros for LED0 output pin ***/ 
#define BSP_LED0_PIN        PORT_PIN_PC18
#define BSP_LED0_Get()      ((PORT_REGS->GROUP[2].PORT_IN >> 18U) & 0x01U)
#define BSP_LED0_Set()      (PORT_REGS->GROUP[2].PORT_OUTSET = ((uint32_t)1U << 18U))
#define BSP_LED0_Clear()    (PORT_REGS->GROUP[2].PORT_OUTCLR = ((uint32_t)1U << 18U))
#define BSP_LED0_Toggle()   (PORT_REGS->GROUP[2].PORT_OUTTGL = ((uint32_t)1U << 18U))
#define BSP_LED0_On()       BSP_LED0_Clear()
#define BSP_LED0_Off()      BSP_LED0_Set() 

/*** Macros for PL460_STBY output pin ***/ 
#define BSP_PL460_STBY_PIN        PORT_PIN_PA22
#define BSP_PL460_STBY_Get()      ((PORT_REGS->GROUP[0].PORT_IN >> 22U) & 0x01U)
#define BSP_PL460_STBY_Set()      (PORT_REGS->GROUP[0].PORT_OUTSET = ((uint32_t)1U << 22U))
#define BSP_PL460_STBY_Clear()    (PORT_REGS->GROUP[0].PORT_OUTCLR = ((uint32_t)1U << 22U))
#define BSP_PL460_STBY_Toggle()   (PORT_REGS->GROUP[0].PORT_OUTTGL = ((uint32_t)1U << 22U))
#define BSP_PL460_STBY_On()       BSP_PL460_STBY_Set()
#define BSP_PL460_STBY_Off()      BSP_PL460_STBY_Clear() 

/*** Macros for PL460_TXEN output pin ***/ 
#define BSP_PL460_TXEN_PIN        PORT_PIN_PA23
#define BSP_PL460_TXEN_Get()      ((PORT_REGS->GROUP[0].PORT_IN >> 23U) & 0x01U)
#define BSP_PL460_TXEN_Set()      (PORT_REGS->GROUP[0].PORT_OUTSET = ((uint32_t)1U << 23U))
#define BSP_PL460_TXEN_Clear()    (PORT_REGS->GROUP[0].PORT_OUTCLR = ((uint32_t)1U << 23U))
#define BSP_PL460_TXEN_Toggle()   (PORT_REGS->GROUP[0].PORT_OUTTGL = ((uint32_t)1U << 23U))
#define BSP_PL460_TXEN_On()       BSP_PL460_TXEN_Set()
#define BSP_PL460_TXEN_Off()      BSP_PL460_TXEN_Clear() 

/*** Macros for EXT1_SPI_CS output pin ***/ 
#define BSP_EXT1_SPI_CS_PIN        PORT_PIN_PB28
#define BSP_EXT1_SPI_CS_Get()      ((PORT_REGS->GROUP[1].PORT_IN >> 28U) & 0x01U)
#define BSP_EXT1_SPI_CS_Set()      (PORT_REGS->GROUP[1].PORT_OUTSET = ((uint32_t)1U << 28U))
#define BSP_EXT1_SPI_CS_Clear()    (PORT_REGS->GROUP[1].PORT_OUTCLR = ((uint32_t)1U << 28U))
#define BSP_EXT1_SPI_CS_Toggle()   (PORT_REGS->GROUP[1].PORT_OUTTGL = ((uint32_t)1U << 28U))
#define BSP_EXT1_SPI_CS_On()       BSP_EXT1_SPI_CS_Clear()
#define BSP_EXT1_SPI_CS_Off()      BSP_EXT1_SPI_CS_Set() 

/*** Macros for GPIO_PB01 output pin ***/ 
#define BSP_GPIO_PB01_PIN        PORT_PIN_PB1
#define BSP_GPIO_PB01_Get()      ((PORT_REGS->GROUP[1].PORT_IN >> 1U) & 0x01U)
#define BSP_GPIO_PB01_Set()      (PORT_REGS->GROUP[1].PORT_OUTSET = ((uint32_t)1U << 1U))
#define BSP_GPIO_PB01_Clear()    (PORT_REGS->GROUP[1].PORT_OUTCLR = ((uint32_t)1U << 1U))
#define BSP_GPIO_PB01_Toggle()   (PORT_REGS->GROUP[1].PORT_OUTTGL = ((uint32_t)1U << 1U))
#define BSP_GPIO_PB01_On()       BSP_GPIO_PB01_Set()
#define BSP_GPIO_PB01_Off()      BSP_GPIO_PB01_Clear() 


/*** Macros for USB_VBUS_SENSE input pin ***/ 
#define BSP_USB_VBUS_SENSE_PIN                    PORT_PIN_PC0
#define BSP_USB_VBUS_SENSE_Get()                  ((PORT_REGS->GROUP[2].PORT_IN >> 0U) & 0x01U)
#define BSP_USB_VBUS_SENSE_STATE_PRESSED          1
#define BSP_USB_VBUS_SENSE_STATE_RELEASED         0
/*** Macros for PL460_NTHW0 input pin ***/ 
#define BSP_PL460_NTHW0_PIN                    PORT_PIN_PA27
#define BSP_PL460_NTHW0_Get()                  ((PORT_REGS->GROUP[0].PORT_IN >> 27U) & 0x01U)
#define BSP_PL460_NTHW0_STATE_PRESSED          1
#define BSP_PL460_NTHW0_STATE_RELEASED         0


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