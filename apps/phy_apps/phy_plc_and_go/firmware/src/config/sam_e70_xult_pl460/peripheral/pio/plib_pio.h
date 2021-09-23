/*******************************************************************************
  PIO PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_pio.h

  Summary:
    PIO PLIB Header File

  Description:
    This library provides an interface to control and interact with Parallel
    Input/Output controller (PIO) module.

*******************************************************************************/

/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
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

#ifndef PLIB_PIO_H
#define PLIB_PIO_H

#include "device.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Data types and constants
// *****************************************************************************
// *****************************************************************************


/*** Macros for PLC_LDO pin ***/
#define PLC_LDO_Set()               (PIOC_REGS->PIO_SODR = (1<<30))
#define PLC_LDO_Clear()             (PIOC_REGS->PIO_CODR = (1<<30))
#define PLC_LDO_Toggle()            (PIOC_REGS->PIO_ODSR ^= (1<<30))
#define PLC_LDO_OutputEnable()      (PIOC_REGS->PIO_OER = (1<<30))
#define PLC_LDO_InputEnable()       (PIOC_REGS->PIO_ODR = (1<<30))
#define PLC_LDO_Get()               ((PIOC_REGS->PIO_PDSR >> 30) & 0x1)
#define PLC_LDO_PIN                  PIO_PIN_PC30

/*** Macros for PLC_INT pin ***/
#define PLC_INT_Set()               (PIOD_REGS->PIO_SODR = (1<<28))
#define PLC_INT_Clear()             (PIOD_REGS->PIO_CODR = (1<<28))
#define PLC_INT_Toggle()            (PIOD_REGS->PIO_ODSR ^= (1<<28))
#define PLC_INT_OutputEnable()      (PIOD_REGS->PIO_OER = (1<<28))
#define PLC_INT_InputEnable()       (PIOD_REGS->PIO_ODR = (1<<28))
#define PLC_INT_Get()               ((PIOD_REGS->PIO_PDSR >> 28) & 0x1)
#define PLC_INT_PIN                  PIO_PIN_PD28
#define PLC_INT_InterruptEnable()   (PIOD_REGS->PIO_IER = (1<<28))
#define PLC_INT_InterruptDisable()  (PIOD_REGS->PIO_IDR = (1<<28))

/*** Macros for PLC_CD pin ***/
#define PLC_CD_Set()               (PIOA_REGS->PIO_SODR = (1<<4))
#define PLC_CD_Clear()             (PIOA_REGS->PIO_CODR = (1<<4))
#define PLC_CD_Toggle()            (PIOA_REGS->PIO_ODSR ^= (1<<4))
#define PLC_CD_OutputEnable()      (PIOA_REGS->PIO_OER = (1<<4))
#define PLC_CD_InputEnable()       (PIOA_REGS->PIO_ODR = (1<<4))
#define PLC_CD_Get()               ((PIOA_REGS->PIO_PDSR >> 4) & 0x1)
#define PLC_CD_PIN                  PIO_PIN_PA4

/*** Macros for PLC_STBY pin ***/
#define PLC_STBY_Set()               (PIOA_REGS->PIO_SODR = (1<<3))
#define PLC_STBY_Clear()             (PIOA_REGS->PIO_CODR = (1<<3))
#define PLC_STBY_Toggle()            (PIOA_REGS->PIO_ODSR ^= (1<<3))
#define PLC_STBY_OutputEnable()      (PIOA_REGS->PIO_OER = (1<<3))
#define PLC_STBY_InputEnable()       (PIOA_REGS->PIO_ODR = (1<<3))
#define PLC_STBY_Get()               ((PIOA_REGS->PIO_PDSR >> 3) & 0x1)
#define PLC_STBY_PIN                  PIO_PIN_PA3

/*** Macros for PLC_RST pin ***/
#define PLC_RST_Set()               (PIOA_REGS->PIO_SODR = (1<<0))
#define PLC_RST_Clear()             (PIOA_REGS->PIO_CODR = (1<<0))
#define PLC_RST_Toggle()            (PIOA_REGS->PIO_ODSR ^= (1<<0))
#define PLC_RST_OutputEnable()      (PIOA_REGS->PIO_OER = (1<<0))
#define PLC_RST_InputEnable()       (PIOA_REGS->PIO_ODR = (1<<0))
#define PLC_RST_Get()               ((PIOA_REGS->PIO_PDSR >> 0) & 0x1)
#define PLC_RST_PIN                  PIO_PIN_PA0

/*** Macros for PLC_THMON pin ***/
#define PLC_THMON_Set()               (PIOC_REGS->PIO_SODR = (1<<17))
#define PLC_THMON_Clear()             (PIOC_REGS->PIO_CODR = (1<<17))
#define PLC_THMON_Toggle()            (PIOC_REGS->PIO_ODSR ^= (1<<17))
#define PLC_THMON_OutputEnable()      (PIOC_REGS->PIO_OER = (1<<17))
#define PLC_THMON_InputEnable()       (PIOC_REGS->PIO_ODR = (1<<17))
#define PLC_THMON_Get()               ((PIOC_REGS->PIO_PDSR >> 17) & 0x1)
#define PLC_THMON_PIN                  PIO_PIN_PC17

/*** Macros for USB_VBUS_SENSE pin ***/
#define USB_VBUS_SENSE_Set()               (PIOB_REGS->PIO_SODR = (1<<8))
#define USB_VBUS_SENSE_Clear()             (PIOB_REGS->PIO_CODR = (1<<8))
#define USB_VBUS_SENSE_Toggle()            (PIOB_REGS->PIO_ODSR ^= (1<<8))
#define USB_VBUS_SENSE_OutputEnable()      (PIOB_REGS->PIO_OER = (1<<8))
#define USB_VBUS_SENSE_InputEnable()       (PIOB_REGS->PIO_ODR = (1<<8))
#define USB_VBUS_SENSE_Get()               ((PIOB_REGS->PIO_PDSR >> 8) & 0x1)
#define USB_VBUS_SENSE_PIN                  PIO_PIN_PB8


// *****************************************************************************
/* PIO Port

  Summary:
    Identifies the available PIO Ports.

  Description:
    This enumeration identifies the available PIO Ports.

  Remarks:
    The caller should not rely on the specific numbers assigned to any of
    these values as they may change from one processor to the next.

    Not all ports are available on all devices.  Refer to the specific
    device data sheet to determine which ports are supported.
*/

typedef enum
{
    PIO_PORT_A = PIOA_BASE_ADDRESS,
    PIO_PORT_B = PIOB_BASE_ADDRESS,
    PIO_PORT_C = PIOC_BASE_ADDRESS,
    PIO_PORT_D = PIOD_BASE_ADDRESS,
    PIO_PORT_E = PIOE_BASE_ADDRESS
} PIO_PORT;

// *****************************************************************************
/* PIO Port Pins

  Summary:
    Identifies the available PIO port pins.

  Description:
    This enumeration identifies the available PIO port pins.

  Remarks:
    The caller should not rely on the specific numbers assigned to any of
    these values as they may change from one processor to the next.

    Not all pins are available on all devices.  Refer to the specific
    device data sheet to determine which pins are supported.
*/

typedef enum
{
    PIO_PIN_PA0 = 0,
    PIO_PIN_PA1 = 1,
    PIO_PIN_PA2 = 2,
    PIO_PIN_PA3 = 3,
    PIO_PIN_PA4 = 4,
    PIO_PIN_PA5 = 5,
    PIO_PIN_PA6 = 6,
    PIO_PIN_PA7 = 7,
    PIO_PIN_PA8 = 8,
    PIO_PIN_PA9 = 9,
    PIO_PIN_PA10 = 10,
    PIO_PIN_PA11 = 11,
    PIO_PIN_PA12 = 12,
    PIO_PIN_PA13 = 13,
    PIO_PIN_PA14 = 14,
    PIO_PIN_PA15 = 15,
    PIO_PIN_PA16 = 16,
    PIO_PIN_PA17 = 17,
    PIO_PIN_PA18 = 18,
    PIO_PIN_PA19 = 19,
    PIO_PIN_PA20 = 20,
    PIO_PIN_PA21 = 21,
    PIO_PIN_PA22 = 22,
    PIO_PIN_PA23 = 23,
    PIO_PIN_PA24 = 24,
    PIO_PIN_PA25 = 25,
    PIO_PIN_PA26 = 26,
    PIO_PIN_PA27 = 27,
    PIO_PIN_PA28 = 28,
    PIO_PIN_PA29 = 29,
    PIO_PIN_PA30 = 30,
    PIO_PIN_PA31 = 31,
    PIO_PIN_PB0 = 32,
    PIO_PIN_PB1 = 33,
    PIO_PIN_PB2 = 34,
    PIO_PIN_PB3 = 35,
    PIO_PIN_PB4 = 36,
    PIO_PIN_PB5 = 37,
    PIO_PIN_PB6 = 38,
    PIO_PIN_PB7 = 39,
    PIO_PIN_PB8 = 40,
    PIO_PIN_PB9 = 41,
    PIO_PIN_PB12 = 44,
    PIO_PIN_PB13 = 45,
    PIO_PIN_PC0 = 64,
    PIO_PIN_PC1 = 65,
    PIO_PIN_PC2 = 66,
    PIO_PIN_PC3 = 67,
    PIO_PIN_PC4 = 68,
    PIO_PIN_PC5 = 69,
    PIO_PIN_PC6 = 70,
    PIO_PIN_PC7 = 71,
    PIO_PIN_PC8 = 72,
    PIO_PIN_PC9 = 73,
    PIO_PIN_PC10 = 74,
    PIO_PIN_PC11 = 75,
    PIO_PIN_PC12 = 76,
    PIO_PIN_PC13 = 77,
    PIO_PIN_PC14 = 78,
    PIO_PIN_PC15 = 79,
    PIO_PIN_PC16 = 80,
    PIO_PIN_PC17 = 81,
    PIO_PIN_PC18 = 82,
    PIO_PIN_PC19 = 83,
    PIO_PIN_PC20 = 84,
    PIO_PIN_PC21 = 85,
    PIO_PIN_PC22 = 86,
    PIO_PIN_PC23 = 87,
    PIO_PIN_PC24 = 88,
    PIO_PIN_PC25 = 89,
    PIO_PIN_PC26 = 90,
    PIO_PIN_PC27 = 91,
    PIO_PIN_PC28 = 92,
    PIO_PIN_PC29 = 93,
    PIO_PIN_PC30 = 94,
    PIO_PIN_PC31 = 95,
    PIO_PIN_PD0 = 96,
    PIO_PIN_PD1 = 97,
    PIO_PIN_PD2 = 98,
    PIO_PIN_PD3 = 99,
    PIO_PIN_PD4 = 100,
    PIO_PIN_PD5 = 101,
    PIO_PIN_PD6 = 102,
    PIO_PIN_PD7 = 103,
    PIO_PIN_PD8 = 104,
    PIO_PIN_PD9 = 105,
    PIO_PIN_PD10 = 106,
    PIO_PIN_PD11 = 107,
    PIO_PIN_PD12 = 108,
    PIO_PIN_PD13 = 109,
    PIO_PIN_PD14 = 110,
    PIO_PIN_PD15 = 111,
    PIO_PIN_PD16 = 112,
    PIO_PIN_PD17 = 113,
    PIO_PIN_PD18 = 114,
    PIO_PIN_PD19 = 115,
    PIO_PIN_PD20 = 116,
    PIO_PIN_PD21 = 117,
    PIO_PIN_PD22 = 118,
    PIO_PIN_PD23 = 119,
    PIO_PIN_PD24 = 120,
    PIO_PIN_PD25 = 121,
    PIO_PIN_PD26 = 122,
    PIO_PIN_PD27 = 123,
    PIO_PIN_PD28 = 124,
    PIO_PIN_PD29 = 125,
    PIO_PIN_PD30 = 126,
    PIO_PIN_PD31 = 127,
    PIO_PIN_PE0 = 128,
    PIO_PIN_PE1 = 129,
    PIO_PIN_PE2 = 130,
    PIO_PIN_PE3 = 131,
    PIO_PIN_PE4 = 132,
    PIO_PIN_PE5 = 133,

    /* This element should not be used in any of the PIO APIs.
       It will be used by other modules or application to denote that none of the PIO Pin is used */
    PIO_PIN_NONE = -1

} PIO_PIN;

typedef  void (*PIO_PIN_CALLBACK) ( PIO_PIN pin, uintptr_t context);

void PIO_Initialize(void);

// *****************************************************************************
// *****************************************************************************
// Section: PIO Functions which operates on multiple pins of a port
// *****************************************************************************
// *****************************************************************************

uint32_t PIO_PortRead(PIO_PORT port);

void PIO_PortWrite(PIO_PORT port, uint32_t mask, uint32_t value);

uint32_t PIO_PortLatchRead ( PIO_PORT port );

void PIO_PortSet(PIO_PORT port, uint32_t mask);

void PIO_PortClear(PIO_PORT port, uint32_t mask);

void PIO_PortToggle(PIO_PORT port, uint32_t mask);

void PIO_PortInputEnable(PIO_PORT port, uint32_t mask);

void PIO_PortOutputEnable(PIO_PORT port, uint32_t mask);

void PIO_PortInterruptEnable(PIO_PORT port, uint32_t mask);

void PIO_PortInterruptDisable(PIO_PORT port, uint32_t mask);

// *****************************************************************************
// *****************************************************************************
// Section: Local Data types and Prototypes
// *****************************************************************************
// *****************************************************************************

typedef struct {

    /* target pin */
    PIO_PIN                 pin;

    /* Callback for event on target pin*/
    PIO_PIN_CALLBACK        callback;

    /* Callback Context */
    uintptr_t               context;

} PIO_PIN_CALLBACK_OBJ;

// *****************************************************************************
// *****************************************************************************
// Section: PIO Functions which operates on one pin at a time
// *****************************************************************************
// *****************************************************************************

static inline void PIO_PinWrite(PIO_PIN pin, bool value)
{
    PIO_PortWrite((PIO_PORT)(PIOA_BASE_ADDRESS + (0x200 * (pin>>5))), (uint32_t)(0x1) << (pin & 0x1f), (uint32_t)(value) << (pin & 0x1f));
}

static inline bool PIO_PinRead(PIO_PIN pin)
{
    return (bool)((PIO_PortRead((PIO_PORT)(PIOA_BASE_ADDRESS + (0x200 * (pin>>5)))) >> (pin & 0x1F)) & 0x1);
}

static inline bool PIO_PinLatchRead(PIO_PIN pin)
{
    return (bool)((PIO_PortLatchRead((PIO_PORT)(PIOA_BASE_ADDRESS + (0x200 * (pin>>5)))) >> (pin & 0x1F)) & 0x1);
}

static inline void PIO_PinToggle(PIO_PIN pin)
{
    PIO_PortToggle((PIO_PORT)(PIOA_BASE_ADDRESS + (0x200 * (pin>>5))), 0x1 << (pin & 0x1F));
}

static inline void PIO_PinSet(PIO_PIN pin)
{
    PIO_PortSet((PIO_PORT)(PIOA_BASE_ADDRESS + (0x200 * (pin>>5))), 0x1 << (pin & 0x1F));
}

static inline void PIO_PinClear(PIO_PIN pin)
{
    PIO_PortClear((PIO_PORT)(PIOA_BASE_ADDRESS + (0x200 * (pin>>5))), 0x1 << (pin & 0x1F));
}

static inline void PIO_PinInputEnable(PIO_PIN pin)
{
    PIO_PortInputEnable((PIO_PORT)(PIOA_BASE_ADDRESS + (0x200 * (pin>>5))), 0x1 << (pin & 0x1F));
}

static inline void PIO_PinOutputEnable(PIO_PIN pin)
{
    PIO_PortOutputEnable((PIO_PORT)(PIOA_BASE_ADDRESS + (0x200 * (pin>>5))), 0x1 << (pin & 0x1F));
}

static inline void PIO_PinInterruptEnable(PIO_PIN pin)
{
    PIO_PortInterruptEnable((PIO_PORT)(PIOA_BASE_ADDRESS + (0x200 * (pin>>5))), 0x1 << (pin & 0x1F));
}

static inline void PIO_PinInterruptDisable(PIO_PIN pin)
{
    PIO_PortInterruptDisable((PIO_PORT)(PIOA_BASE_ADDRESS + (0x200 * (pin>>5))), 0x1 << (pin & 0x1F));
}

bool PIO_PinInterruptCallbackRegister(
    PIO_PIN pin,
    const   PIO_PIN_CALLBACK callBack,
    uintptr_t context
);

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif
// DOM-IGNORE-END
#endif // PLIB_PIO_H