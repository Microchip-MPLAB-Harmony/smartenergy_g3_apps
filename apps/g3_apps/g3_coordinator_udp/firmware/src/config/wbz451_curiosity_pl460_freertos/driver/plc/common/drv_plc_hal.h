/*******************************************************************************
  DRV_PLC Hardware Abstraction Header File

  Company:
    Microchip Technology Inc.

  File Name:
    drv_plc_hal.h

  Summary:
    PLC Driver Hardware Abstraction Layer Header File

  Description:
    The PLC Library provides a Hardware Abstraction Layer.
*******************************************************************************/

//DOM-IGNORE-BEGIN
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
//DOM-IGNORE-END

#ifndef DRV_PLC_HAL_H
#define DRV_PLC_HAL_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdio.h>
#include <stdbool.h>
#include "system/ports/sys_ports.h"
#include "system/dma/sys_dma.h"
#include "peripheral/sercom/spi_master/plib_sercom_spi_master_common.h"
#include "peripheral/eic/plib_eic.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Macro Definitions
// *****************************************************************************
// *****************************************************************************
#define DRV_PLC_HAL_CPU_CLOCK_FREQ            64000000

#define DRV_PLC_HAL_CMD_POS                   15
#define DRV_PLC_HAL_CMD_RD                    ((uint16_t)0U << DRV_PLC_HAL_CMD_POS)
#define DRV_PLC_HAL_CMD_WR                    ((uint16_t)1U << DRV_PLC_HAL_CMD_POS)

#define DRV_PLC_HAL_LEN_MASK                  0x7FFF

/* SPI Key MASK */
#define DRV_PLC_HAL_KEY_MASK                  0xFFFEU
/* SPI Key when bootloader is running in PLC transceiver */
#define DRV_PLC_HAL_KEY_BOOT                  (0x5634U & DRV_PLC_HAL_KEY_MASK)
/* SPI Key when PLC firmware is running in PLC transceiver */
#define DRV_PLC_HAL_KEY_CORTEX                (0x5a5aU & DRV_PLC_HAL_KEY_MASK)

#define DRV_PLC_HAL_KEY(b0, b1)               ((((uint16_t)(b1) << 8) + (b0)) & DRV_PLC_HAL_KEY_MASK)
#define DRV_PLC_HAL_FLAGS_BOOT(b0, b2, b3)    ((((uint32_t)(b3)) << 8) + ((uint32_t)(b2)) + (((uint32_t)(b0) & 0x01UL) << 16))
#define DRV_PLC_HAL_FLAGS_CORTEX(b2, b3)      ((((uint32_t)(b3)) << 8) + ((uint32_t)(b2)))

/* User rest flag in bootloader key*/
#define DRV_PLC_HAL_FLAG_RST_USER             0x00010000UL
/* Cortex(debugger) rest flag in bootloader key*/
#define DRV_PLC_HAL_FLAG_RST_CORTEX           0x00008000UL
/* Watch Dog flag in bootloader key */
#define DRV_PLC_HAL_FLAG_RST_WDOG             0x00004000UL
/* Power-ON reset is indicated when the three flags are 0, mask will be used to detect it*/
#define DRV_PLC_HAL_FLAG_RST_PON              0x0001C000UL

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

typedef bool (* DRV_PLC_SPI_PLIB_TRANSFER_SETUP)(uintptr_t setup, uint32_t spiSourceClock);

typedef enum
{
    DRV_PLC_SPI_CLOCK_PHASE_TRAILING_EDGE = SPI_CLOCK_PHASE_TRAILING_EDGE,
    DRV_PLC_SPI_CLOCK_PHASE_LEADING_EDGE = SPI_CLOCK_PHASE_LEADING_EDGE,

    /* Force the compiler to reserve 32-bit space for each enum value */
    DRV_PLC_SPI_CLOCK_PHASE_INVALID = SPI_CLOCK_PHASE_INVALID

} DRV_PLC_SPI_CLOCK_PHASE;

/* MISRA C-2012 deviation block start */
/* MISRA C-2012 Rule 5.2 deviated once.  Deviation record ID - H3_MISRAC_2012_R_5_2_DR_1 */

typedef enum
{
    DRV_PLC_SPI_CLOCK_POLARITY_IDLE_LOW = SPI_CLOCK_POLARITY_IDLE_LOW,
    DRV_PLC_SPI_CLOCK_POLARITY_IDLE_HIGH = SPI_CLOCK_POLARITY_IDLE_HIGH,

    /* Force the compiler to reserve 32-bit space for each enum value */
    DRV_PLC_SPI_CLOCK_POLARITY_INVALID = SPI_CLOCK_POLARITY_INVALID

} DRV_PLC_SPI_CLOCK_POLARITY;

/* MISRA C-2012 deviation block end */

typedef enum
{
    DRV_PLC_SPI_DATA_BITS_8 = SPI_DATA_BITS_8,
    DRV_PLC_SPI_DATA_BITS_9 = SPI_DATA_BITS_9,

    /* Force the compiler to reserve 32-bit space for each enum value */
    DRV_PLC_SPI_DATA_BITS_INVALID = SPI_DATA_BITS_INVALID

} DRV_PLC_SPI_DATA_BITS;

typedef struct
{
    uint32_t    clockFrequency;
    DRV_PLC_SPI_CLOCK_PHASE clockPhase;
    DRV_PLC_SPI_CLOCK_POLARITY clockPolarity;
    DRV_PLC_SPI_DATA_BITS   dataBits;

} DRV_PLC_SPI_TRANSFER_SETUP;

// *****************************************************************************
/* PLC Driver PLIB Interface Data

  Summary:
    Defines the data required to initialize the PLC driver PLIB Interface.

  Description:
    This data type defines the data required to initialize the PLC driver
    PLIB Interface.

  Remarks:
    None.
*/

typedef struct
{
    /* PLC SPI PLIB Transfer Setup */
    DRV_PLC_SPI_PLIB_TRANSFER_SETUP        spiPlibTransferSetup;

    /* SPI transmit DMA channel. */
    SYS_DMA_CHANNEL                        dmaChannelTx;

    /* SPI receive DMA channel. */
    SYS_DMA_CHANNEL                        dmaChannelRx;

    /* SPI transmit register address used for DMA operation. */
    void                                   *spiAddressTx;

    /* SPI receive register address used for DMA operation. */
    void                                   *spiAddressRx;

    /* SPI Chip select pin */
    SYS_PORT_PIN                           spiCSPin;

    /* SPI clock frequency */
    uint32_t                               spiClockFrequency;

    /* PLC LDO enable pin */
    SYS_PORT_PIN                           ldoPin;

    /* PLC reset pin */
    SYS_PORT_PIN                           resetPin;

    EIC_PIN                                extIntPin;

    /* PLC external interrupt pio */
    SYS_PORT_PIN                           extIntPio;

    /* PLC Tx Enable pin */
    SYS_PORT_PIN                           txEnablePin;

} DRV_PLC_PLIB_INTERFACE;

// *****************************************************************************

typedef void (* DRV_PLC_HAL_INIT)(DRV_PLC_PLIB_INTERFACE *plcPlib);

typedef void (* DRV_PLC_HAL_SETUP)(bool set16Bits);

typedef void (* DRV_PLC_HAL_RESET)(void);

typedef void (* DRV_PLC_HAL_SET_TXENABLE)(bool enable);

typedef void (* DRV_PLC_HAL_ENABLE_EXT_INT)(bool enable);

typedef bool (* DRV_PLC_HAL_GET_PIN_LEVEL)(SYS_PORT_PIN pin);

typedef void (* DRV_PLC_HAL_DELAY)(uint32_t delay);

typedef void (* DRV_PLC_HAL_SEND_BOOT_CMD)(uint16_t cmd, uint32_t addr, uint32_t dataLength, void *pDataWr, void *pDataRd);

typedef void (* DRV_PLC_HAL_SEND_WRRD_CMD)(void *pCmd, void *pInfo);

// *****************************************************************************
/* PLC Driver HAL Interface Data

  Summary:
    Defines the data required to initialize the PLC driver HAL Interface.

  Description:
    This data type defines the data required to initialize the PLC driver
    HAL Interface.

  Remarks:
    None.
*/

typedef struct
{
    /* PLC PLIB Interface */
    DRV_PLC_PLIB_INTERFACE                   *plcPlib;

    /* PLC HAL init */
    DRV_PLC_HAL_INIT                         init;

    /* PLC HAL setup */
    DRV_PLC_HAL_SETUP                        setup;

    /* PLC HAL reset device */
    DRV_PLC_HAL_RESET                        reset;

    /* PLC HAL Set Tx Enable pin */
    DRV_PLC_HAL_SET_TXENABLE                 setTxEnable;

    /* PLC HAL Enable/Disable external interrupt */
    DRV_PLC_HAL_ENABLE_EXT_INT               enableExtInt;

    /* PLC HAL Get Pin level */
    DRV_PLC_HAL_GET_PIN_LEVEL                getPinLevel;

    /* PLC HAL delay function */
    DRV_PLC_HAL_DELAY                        delay;

    /* PLC HAL Transfer Bootloader Command */
    DRV_PLC_HAL_SEND_BOOT_CMD                sendBootCmd;

    /* PLC HAL Transfer Write/Read Command */
    DRV_PLC_HAL_SEND_WRRD_CMD                sendWrRdCmd;

} DRV_PLC_HAL_INTERFACE;

// *****************************************************************************
/* DRV_PLC_HAL Command object

  Summary:
    Object used to handle any SPI transfer.

  Description:
    None.

  Remarks:
    None.
*/

typedef struct
{
    uint8_t *pData;
    uint16_t length;
    uint16_t memId;
    uint16_t cmd;
}DRV_PLC_HAL_CMD;

// *****************************************************************************
/* DRV_PLC_HAL header Information

  Summary:
    Object used to keep track of a SPI transfer with PLC transceiver.

  Description:
    None.

  Remarks:
    None.
*/

typedef struct
{
    uint32_t flags;
    uint16_t key;
}DRV_PLC_HAL_INFO;

// *****************************************************************************
// *****************************************************************************
// Section: DRV_PLC_HAL Common Interface Implementation
// *****************************************************************************
// *****************************************************************************

void DRV_PLC_HAL_Init(DRV_PLC_PLIB_INTERFACE *plcPlib);
void DRV_PLC_HAL_Reset(void);
void DRV_PLC_HAL_Setup(bool set16Bits);
void DRV_PLC_HAL_SetTxEnable(bool enable);
void DRV_PLC_HAL_EnableInterrupts(bool enable);
bool DRV_PLC_HAL_GetPinLevel(SYS_PORT_PIN pin);
void DRV_PLC_HAL_Delay(uint32_t delayUs);
void DRV_PLC_HAL_SendBootCmd(uint16_t cmd, uint32_t addr, uint32_t dataLength, uint8_t *pDataWr, uint8_t *pDataRd);
void DRV_PLC_HAL_SendWrRdCmd(DRV_PLC_HAL_CMD *pCmd, DRV_PLC_HAL_INFO *pInfo);

#ifdef __cplusplus
}
#endif

#endif // #ifndef DRV_PLC_HAL_H
/*******************************************************************************
 End of File
*/
