/******************************************************************************
  DRV_PLC Hardware Abstraction Layer

  Company:
    Microchip Technology Inc.

  File Name:
    drv_plc_hal.c

  Summary:
    PLC Driver Hardware Abstraction Layer

  Description:
    This file contains the source code for the implementation of the Hardware
    Abstraction Layer.
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

// *****************************************************************************
// *****************************************************************************
// Section: Include Files
// *****************************************************************************
// *****************************************************************************
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "definitions.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data
// *****************************************************************************
// *****************************************************************************

/* SPI Header size. */
#define HAL_SPI_HEADER_SIZE      4U
/* SPI Max Msg_Data size. */
#define HAL_SPI_MSG_DATA_SIZE    512U
/* SPI Max Msg_Data size. */
#define HAL_SPI_MSG_PARAMS_SIZE  118U   /* Worst case = 118: sizeof(rx_msg_t) [G3] */
/* PDC buffer us_size. */
#define HAL_SPI_BUFFER_SIZE      (HAL_SPI_HEADER_SIZE + HAL_SPI_MSG_DATA_SIZE + HAL_SPI_MSG_PARAMS_SIZE)

/* PDC Receive buffer */
static CACHE_ALIGN uint8_t sRxSpiData[CACHE_ALIGNED_SIZE_GET(HAL_SPI_BUFFER_SIZE)];
/* PDC Transmission buffer */
static CACHE_ALIGN uint8_t sTxSpiData[CACHE_ALIGNED_SIZE_GET(HAL_SPI_BUFFER_SIZE)];

/* Static pointer to PLIB interface used to handle PLC */
static DRV_PLC_PLIB_INTERFACE *sPlcPlib;

// *****************************************************************************
// *****************************************************************************
// Section: DRV_PLC_HAL Common Interface Implementation
// *****************************************************************************
// *****************************************************************************
void DRV_PLC_HAL_Init(DRV_PLC_PLIB_INTERFACE *plcPlib)
{
    sPlcPlib = plcPlib;

    /* Clear StandBy pin */
    SYS_PORT_PinClear(sPlcPlib->stByPin);

    /* Enable LDO_EN pin */
    SYS_PORT_PinSet(sPlcPlib->ldoPin);

    /* Push NRST pin */
    SYS_PORT_PinClear(sPlcPlib->resetPin);

    /* Disable External Interrupt */
    PIO_PinInterruptDisable((PIO_PIN)sPlcPlib->extIntPin);
    /* Enable External Interrupt Source */
    SYS_INT_SourceEnable(DRV_PLC_EXT_INT_SRC);
}

void DRV_PLC_HAL_Setup(bool set16Bits)
{
    DRV_PLC_SPI_TRANSFER_SETUP spiPlibSetup;

    while(sPlcPlib->spiIsBusy()){}

    if (set16Bits)
    {
        spiPlibSetup.dataBits = DRV_PLC_SPI_DATA_BITS_16;
    }
    else
    {
        spiPlibSetup.dataBits = DRV_PLC_SPI_DATA_BITS_8;
    }

    /* Configure SPI PLIB */
    spiPlibSetup.clockFrequency = sPlcPlib->spiClockFrequency;
    spiPlibSetup.clockPhase = DRV_PLC_SPI_CLOCK_PHASE_LEADING_EDGE;
    spiPlibSetup.clockPolarity = DRV_PLC_SPI_CLOCK_POLARITY_IDLE_LOW;
    (void)sPlcPlib->spiPlibTransferSetup((uintptr_t)&spiPlibSetup, 0);

}

void DRV_PLC_HAL_Reset(void)
{
    /* Pulse of 50 us in NRST pin of PLC modem */
    SYS_PORT_PinClear(sPlcPlib->resetPin);
    DRV_PLC_HAL_Delay(50);
    SYS_PORT_PinSet(sPlcPlib->resetPin);

    /* 1.2 ms is needed after releasing NRST for the System to be up */
    DRV_PLC_HAL_Delay(1500);
}

void DRV_PLC_HAL_SetStandBy(bool enable)
{
    if (enable)
    {
        /* Enable Reset pin */
        SYS_PORT_PinClear(sPlcPlib->resetPin);

        /* Enable Stby Pin */
        SYS_PORT_PinSet(sPlcPlib->stByPin);
    }
    else
    {
        /* Disable Stby Pin */
        SYS_PORT_PinClear(sPlcPlib->stByPin);

        /* Disable Reset pin */
        SYS_PORT_PinSet(sPlcPlib->resetPin);

        /* Wait to PLC startup (700us) */
        DRV_PLC_HAL_Delay(700);
    }
}

bool DRV_PLC_HAL_GetThermalMonitor(void)
{
    if (SYS_PORT_PinRead(sPlcPlib->thMonPin))
    {
        return false;
    }
    else
    {
        return true;
    }
}

void DRV_PLC_HAL_SetTxEnable(bool enable)
{
    if (enable)
    {
        /* Set TX Enable Pin */
        SYS_PORT_PinSet(sPlcPlib->txEnablePin);
    }
    else
    {
        /* Clear TX Enable Pin */
        SYS_PORT_PinClear(sPlcPlib->txEnablePin);
    }
}

void DRV_PLC_HAL_Delay(uint32_t delayUs)
{
    SYS_TIME_HANDLE tmrHandle = SYS_TIME_HANDLE_INVALID;

    if (SYS_TIME_DelayUS(delayUs, &tmrHandle) == SYS_TIME_SUCCESS)
    {
        // Wait till the delay has not expired
        while (SYS_TIME_DelayIsComplete(tmrHandle) == false){}
    }
}

void DRV_PLC_HAL_EnableInterrupts(bool enable)
{
    if (enable)
    {
        SYS_INT_SourceStatusClear(DRV_PLC_EXT_INT_SRC);
        PIO_PinInterruptEnable((PIO_PIN)sPlcPlib->extIntPin);
    }
    else
    {
        PIO_PinInterruptDisable((PIO_PIN)sPlcPlib->extIntPin);
    }
}

bool DRV_PLC_HAL_GetPinLevel(SYS_PORT_PIN pin)
{
    return (SYS_PORT_PinRead(pin));
}

void DRV_PLC_HAL_SendBootCmd(uint16_t cmd, uint32_t addr, uint32_t dataLength, uint8_t *pDataWr, uint8_t *pDataRd)
{
    uint8_t *pTxData;
    size_t size;

    while(sPlcPlib->spiIsBusy()){}

    pTxData = sTxSpiData;

    /* Build command */
    (void) memcpy(pTxData, (uint8_t *)&addr, 4);
    pTxData += 4;
    (void) memcpy(pTxData, (uint8_t *)&cmd, 2);
    pTxData += 2;
    if (dataLength > 0U)
    {
        if (dataLength > HAL_SPI_BUFFER_SIZE - 6U)
        {
            dataLength = HAL_SPI_BUFFER_SIZE - 6U;
        }

        if (pDataWr != NULL)
        {
            (void) memcpy(pTxData, pDataWr, dataLength);
        }
        else
        {
            /* Insert dummy data */
            (void) memset(pTxData, 0, dataLength);
        }
    }

    /* Get length of transaction in bytes */
    size = 6U + dataLength;

    (void) sPlcPlib->spiWriteRead(sTxSpiData, size, sRxSpiData, size);

    if ((pDataRd != NULL) && (dataLength > 0U))
    {
        while(sPlcPlib->spiIsBusy()){}

        /* Update data received */
        (void) memcpy(pDataRd, &sRxSpiData[6], dataLength);
    }
}

void DRV_PLC_HAL_SendWrRdCmd(DRV_PLC_HAL_CMD *pCmd, DRV_PLC_HAL_INFO *pInfo)
{
    uint8_t *pTxData;
    size_t cmdSize;
    uint16_t dataLength, totalLength;

    while(sPlcPlib->spiIsBusy()){}

    pTxData = sTxSpiData;

    dataLength = ((pCmd->length + 1U) >> 1) & 0x7FFFU;

    /* Protect length */
    if ((dataLength == 0U) || (pCmd->length > (HAL_SPI_MSG_DATA_SIZE + HAL_SPI_MSG_PARAMS_SIZE)))
    {
        return;
    }

    /* Join CMD and Length */
    dataLength |= pCmd->cmd;

    /* Build command */
    /* Address */
    *pTxData++ = (uint8_t)(pCmd->memId);
    *pTxData++ = (uint8_t)(pCmd->memId >> 8);
    *pTxData++ = (uint8_t)(dataLength);
    *pTxData++ = (uint8_t)(dataLength >> 8);

    if (pCmd->cmd == DRV_PLC_HAL_CMD_WR) {
        /* Fill with transmission data */
        (void) memcpy(pTxData, pCmd->pData, pCmd->length);
    } else {
        /* Fill with dummy data */
        (void) memset(pTxData, 0, pCmd->length);
    }

    pTxData += pCmd->length;

    totalLength = HAL_SPI_HEADER_SIZE + pCmd->length;
    cmdSize = totalLength;

    if ((cmdSize % 2U) > 0U) {
        *pTxData++ = 0;
        cmdSize++;
    }

    (void) sPlcPlib->spiWriteRead(sTxSpiData, cmdSize >> 1, sRxSpiData, cmdSize >> 1);

    while(sPlcPlib->spiIsBusy()){}

    if (pCmd->cmd == DRV_PLC_HAL_CMD_RD) {
        /* Update data received */
        (void) memcpy(pCmd->pData, &sRxSpiData[4], pCmd->length);
    }

    /* Get HAL info */
    pInfo->key = DRV_PLC_HAL_KEY(sRxSpiData[0], sRxSpiData[1]);
    if (pInfo->key == DRV_PLC_HAL_KEY_CORTEX)
    {
        pInfo->flags = DRV_PLC_HAL_FLAGS_CORTEX(sRxSpiData[2], sRxSpiData[3]);
    }
    else if (pInfo->key == DRV_PLC_HAL_KEY_BOOT)
    {
        pInfo->flags = DRV_PLC_HAL_FLAGS_BOOT(sRxSpiData[0], sRxSpiData[2], sRxSpiData[3]);
    }
    else
    {
        pInfo->flags = 0UL;
    }
}
