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
// Section: File scope functions
// *****************************************************************************
// *****************************************************************************

static void __attribute__((optimize("O0"))) lDRV_PLC_HAL_memcpyREV16 (void * pDst, void * pSrc, size_t size)
{
    /* R0=pDst; R1=pSrc; R2=size */
    __asm volatile (
        "PUSH {R4}\n"
        "MOV R4, #0\n"

    "COPY_DATA_LOOP:\n"
        "CMP R2, R4\n"
        "BLS COPY_DATA_END\n"
        "LDR R3, [R1, R4]\n"
        "REV16 R3, R3\n"
        "STR R3, [R0, R4]\n"
        "ADD R4, R4, #4\n"
        "B COPY_DATA_LOOP\n"
    "COPY_DATA_END:\n"
        "POP {R4}\n"
    );
}

// *****************************************************************************
// *****************************************************************************
// Section: DRV_PLC_HAL Common Interface Implementation
// *****************************************************************************
// *****************************************************************************
void DRV_PLC_HAL_Init(DRV_PLC_PLIB_INTERFACE *plcPlib)
{
    sPlcPlib = plcPlib;   
    
    /* Disable External Interrupt */
    EIC_InterruptDisable(sPlcPlib->extIntPin);
    /* Enable External Interrupt Source */
    SYS_INT_SourceEnable(DRV_PLC_EXT_INT_SRC);
}

void DRV_PLC_HAL_Setup(bool set16Bits)
{
    DRV_PLC_SPI_TRANSFER_SETUP spiPlibSetup;

    (void)set16Bits;

    while(sPlcPlib->spiIsBusy()){}
        
    spiPlibSetup.dataBits = DRV_PLC_SPI_DATA_BITS_8;
    
    /* Configure SPI PLIB */
    spiPlibSetup.clockFrequency = sPlcPlib->spiClockFrequency;
    spiPlibSetup.clockPhase = DRV_PLC_SPI_CLOCK_PHASE_LEADING_EDGE;
    spiPlibSetup.clockPolarity = DRV_PLC_SPI_CLOCK_POLARITY_IDLE_LOW;
    (void) sPlcPlib->spiPlibTransferSetup((uintptr_t)&spiPlibSetup, 0);
    
}

void DRV_PLC_HAL_Reset(void)
{
    /* Disable LDO pin */
    SYS_PORT_PinClear(sPlcPlib->ldoPin);

    /* Enable Reset Pin */
    SYS_PORT_PinClear(sPlcPlib->resetPin);

    /* Wait to PLC startup (50us) */
    DRV_PLC_HAL_Delay(50);

    /* Enable LDO pin */
    SYS_PORT_PinSet(sPlcPlib->ldoPin);

    /* Disable Reset pin */
    SYS_PORT_PinSet(sPlcPlib->resetPin);

    /* Wait to PLC startup (1000us) */
    DRV_PLC_HAL_Delay(1000);
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
        EIC_InterruptEnable(sPlcPlib->extIntPin);
    }
    else
    {
        EIC_InterruptDisable(sPlcPlib->extIntPin);
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

    /* Assert CS pin */
    SYS_PORT_PinClear(sPlcPlib->spiCSPin);

    (void) sPlcPlib->spiWriteRead(sTxSpiData, size, sRxSpiData, size);

    while(sPlcPlib->spiIsBusy()){}

    /* Deassert CS pin */
    SYS_PORT_PinSet(sPlcPlib->spiCSPin);   

    if ((pDataRd != NULL) && (dataLength > 0U))
    {
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
    *pTxData++ = (uint8_t)(pCmd->memId >> 8);
    *pTxData++ = (uint8_t)(pCmd->memId);
    *pTxData++ = (uint8_t)(dataLength >> 8);
    *pTxData++ = (uint8_t)(dataLength);

    if (pCmd->cmd == DRV_PLC_HAL_CMD_WR) {
        /* Fill with transmission data */
        lDRV_PLC_HAL_memcpyREV16(pTxData, pCmd->pData, pCmd->length);
    } else {
        /* Fill with dummy data */
        (void) memset(pTxData, 0, pCmd->length);
    }

    pTxData += pCmd->length;

    totalLength = HAL_SPI_HEADER_SIZE + pCmd->length;
    cmdSize = totalLength;
    
    if ((cmdSize % 2U) > 0U) {
        cmdSize++;
    }
       
    /* Assert CS pin */
    SYS_PORT_PinClear(sPlcPlib->spiCSPin);

    (void) sPlcPlib->spiWriteRead(sTxSpiData, cmdSize, sRxSpiData, cmdSize);

    while(sPlcPlib->spiIsBusy()){}

    /* Deassert CS pin */
    SYS_PORT_PinSet(sPlcPlib->spiCSPin);

    if (pCmd->cmd == DRV_PLC_HAL_CMD_RD) {
        /* Update data received */
        lDRV_PLC_HAL_memcpyREV16(pCmd->pData, &sRxSpiData[4], pCmd->length);
    }
    
    /* Get HAL info */
    pInfo->key = DRV_PLC_HAL_KEY(sRxSpiData[1], sRxSpiData[0]);
    if (pInfo->key == DRV_PLC_HAL_KEY_CORTEX)
    {
        pInfo->flags = DRV_PLC_HAL_FLAGS_CORTEX(sRxSpiData[3], sRxSpiData[2]);
    } 
    else if (pInfo->key == DRV_PLC_HAL_KEY_BOOT)
    {
        pInfo->flags = DRV_PLC_HAL_FLAGS_BOOT(sRxSpiData[1], sRxSpiData[3], sRxSpiData[2]);
    } 
    else 
    {
        pInfo->flags = 0UL;
    }
}
