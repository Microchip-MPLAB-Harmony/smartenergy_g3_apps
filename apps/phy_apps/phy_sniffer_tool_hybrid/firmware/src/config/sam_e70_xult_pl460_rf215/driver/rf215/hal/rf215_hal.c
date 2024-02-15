/******************************************************************************
  RF215 Driver Hardware Abstraction Layer Implementation.

  Company:
    Microchip Technology Inc.

  File Name:
    rf215_hal.c

  Summary:
    Source code for the RF215 Driver Hardware Abstraction Layer implementation.

  Description:
    The RF215 driver HAL (Hardware Abstraction Layer) manages the hardware
    peripherals used by the RF215 driver. This file contains the source code
    for the implementation of the RF215 driver HAL.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*
Copyright (C) 2024, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
#include <string.h>
#include "configuration.h"
#include "system/ports/sys_ports.h"
#include "system/dma/sys_dma.h"
#include "system/cache/sys_cache.h"
#include "driver/rf215/drv_rf215_local.h"
#include "driver/rf215/hal/rf215_hal.h"

// *****************************************************************************
// *****************************************************************************
// Section: Macro Definitions
// *****************************************************************************
// *****************************************************************************

/* Reset pin pulse width.
 * From RF215 datasheet (Table 10-3): Min tRST = 625 ns */
#define RF215_RST_PULSE_US            7U

/* SPI DMA buffer size */
#define RF215_SPI_BUF_SIZE            (RF215_SPI_CMD_SIZE + DRV_RF215_MAX_PSDU_LEN)

/* SPI transfer pool size: Maximum number of SPI transfers that can be queued */
#define RF215_SPI_TRANSFER_POOL_SIZE  20U

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Data
// *****************************************************************************
// *****************************************************************************

/* RF215 HAL object */
static RF215_HAL_OBJ rf215HalObj = {0};

/* SPI transfer pool */
static RF215_SPI_TRANSFER_OBJ halSpiTransferPool[RF215_SPI_TRANSFER_POOL_SIZE] = {0};

/* DMA buffers for SPI transmit and receive */
static CACHE_ALIGN uint8_t halSpiTxData[CACHE_ALIGNED_SIZE_GET(RF215_SPI_BUF_SIZE)];
static CACHE_ALIGN uint8_t halSpiRxData[CACHE_ALIGNED_SIZE_GET(RF215_SPI_BUF_SIZE)];

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Functions
// *****************************************************************************
// *****************************************************************************

static inline void lRF215_HAL_ExtIntDisable(void)
{
    PIO_PinInterruptDisable((PIO_PIN) DRV_RF215_EXT_INT_PIN);
    rf215HalObj.extIntDisableCount++;
}

static inline void lRF215_HAL_ExtIntEnable(void)
{
    if (rf215HalObj.extIntDisableCount > 0U)
    {
        rf215HalObj.extIntDisableCount--;
    }

    if (rf215HalObj.extIntDisableCount == 0U)
    {
        PIO_PinInterruptEnable((PIO_PIN) DRV_RF215_EXT_INT_PIN);
    }
}

static inline bool lRF215_HAL_DisableIntSources(bool* timeIntStatus, bool* plcExtIntStatus)
{
    *plcExtIntStatus = SYS_INT_SourceDisable(rf215HalObj.plcExtIntSource);
    *timeIntStatus = SYS_INT_SourceDisable(rf215HalObj.sysTimeIntSource);
    return SYS_INT_SourceDisable(rf215HalObj.dmaIntSource);
}

static inline void lRF215_HAL_RestoreIntSources(bool dmaIntStatus, bool timeIntStatus, bool plcExtIntStatus)
{
    SYS_INT_SourceRestore(rf215HalObj.dmaIntSource, dmaIntStatus);
    SYS_INT_SourceRestore(rf215HalObj.sysTimeIntSource, timeIntStatus);
    SYS_INT_SourceRestore(rf215HalObj.plcExtIntSource, plcExtIntStatus);
}

static void lRF215_HAL_SpiTransferStart (
    RF215_SPI_TRANSFER_MODE mode,
    uint16_t regAddr,
    void* pData,
    size_t size
)
{
    size_t transferSize, txCleanCacheSize;
    uint16_t cmd;
    bool intStatus;
    uint8_t* pTxData = halSpiTxData;
    RF215_HAL_OBJ* hObj = &rf215HalObj;

    /* Build 16 bits corresponding to COMMAND
     * COMMAND[15:14] = MODE[1:0]; COMMAND[13:0] = ADDRESS[13:0] */
    cmd = regAddr | (uint16_t)mode;
    transferSize = size + RF215_SPI_CMD_SIZE;

    /* Write COMMAND to SPI transmit buffer (MSB first) */
    *pTxData++ = (uint8_t)(cmd >> 8);
    *pTxData++ = (uint8_t)cmd;

    if (mode == RF215_SPI_WRITE)
    {
        /* Copy data to SPI transmit buffer */
        (void) memcpy((void*)pTxData, pData, size);
        /* All SPI transmit data cache must be cleaned */
        txCleanCacheSize = transferSize;
    }
    else
    {
        /* Only COMMAND SPI transmit data cache needs to be cleaned */
        txCleanCacheSize = RF215_SPI_CMD_SIZE;
    }

    /* Wait if there is SPI transfer in progress */
    while(hObj->spiPlibIsBusy() == true){}
    while(SYS_DMA_ChannelIsBusy(DRV_RF215_SPI_RX_DMA_CH) == true){}

    /* Set chip select (shared SPI) */
    hObj->spiPlibSetChipSelect(DRV_RF215_SPI_CHIP_SELECT);

    /* Set 8-bit data width in DMA channels (shared SPI) */
    SYS_DMA_DataWidthSetup(DRV_RF215_SPI_TX_DMA_CH, SYS_DMA_WIDTH_8_BIT);
    SYS_DMA_DataWidthSetup(DRV_RF215_SPI_RX_DMA_CH, SYS_DMA_WIDTH_8_BIT);

    /* Configure DMA channel for SPI receive */
    (void) SYS_DMA_ChannelTransfer(DRV_RF215_SPI_RX_DMA_CH, hObj->spiRxAddr, halSpiRxData, transferSize);

    /* Clean cache to push SPI transmit buffer to the main memory */
    SYS_CACHE_CleanDCache_by_Addr(halSpiTxData, (int32_t)txCleanCacheSize);

    /* Disable all interrupts for a while to avoid delays between SPI transfer
     * and SYS_TIME counter read */
    intStatus = SYS_INT_Disable();

    /* Configure DMA channel for SPI transmit. This triggers the SPI transfer */
    (void) SYS_DMA_ChannelTransfer(DRV_RF215_SPI_TX_DMA_CH, halSpiTxData, hObj->spiTxAddr, transferSize);

    /* Read SYS_TIME counter just after SPI transfer is launched */
    hObj->sysTimeTransfer = SYS_TIME_Counter64Get();
    SYS_INT_Restore(intStatus);

    /* Set DMA transfer in progress */
    hObj->dmaTransferInProgress = true;

    if (mode == RF215_SPI_READ)
    {
        /* Invalidate SPI receive buffer (force CPU to load from main memory) */
        SYS_CACHE_InvalidateDCache_by_Addr(halSpiRxData, (int32_t)transferSize);
    }
}

static void lRF215_HAL_SpiTransfer (
    RF215_SPI_TRANSFER_MODE mode,
    uint16_t regAddr,
    void* pData,
    size_t size,
    bool fromTasks,
    RF215_SPI_TRANSFER_CALLBACK callback,
    uintptr_t context
)
{
    bool dmaIntStatus, timeIntStatus, plcExtIntStatus;
    RF215_SPI_TRANSFER_OBJ* transfer;
    RF215_SPI_TRANSFER_OBJ* transferPoolEdge;

    /* Critical region to avoid conflict in SPI transfer queue */
    dmaIntStatus = lRF215_HAL_DisableIntSources(&timeIntStatus, &plcExtIntStatus);
    lRF215_HAL_ExtIntDisable();

    /* Look for a free transfer object in the pool */
    transferPoolEdge = &halSpiTransferPool[RF215_SPI_TRANSFER_POOL_SIZE];
    for (transfer = halSpiTransferPool; transfer < transferPoolEdge; transfer++)
    {
        if (transfer->inUse == false)
        {
            /* Copy transfer parameters */
            transfer->next = NULL;
            transfer->pData = pData;
            transfer->callback = callback;
            transfer->context = context;
            transfer->size = size;
            transfer->mode = mode;
            transfer->regAddr = regAddr;
            transfer->inUse = true;
            transfer->fromTasks = fromTasks;

            if (rf215HalObj.spiQueueFirst == NULL)
            {
                /* No SPI transfers in the queue */
                rf215HalObj.spiQueueFirst = transfer;
                rf215HalObj.spiQueueLast = transfer;
            }
            else
            {
                /* Add SPI transfer to the queue */
                rf215HalObj.spiQueueLast->next = transfer;
                rf215HalObj.spiQueueLast = transfer;
            }

            break;
        }
    }

    /* External interrupt kept disabled until SPI transfer finishes */
    if (rf215HalObj.spiQueueFirst == transfer)
    {
        /* This transfer is the first in the queue so it can be started */
        if (fromTasks == false)
        {
            lRF215_HAL_SpiTransferStart(mode, regAddr, pData, size);
        }
        else
        {
            rf215HalObj.spiTransferFromTasks = true;
        }
    }

    /* Leave critical region */
    lRF215_HAL_RestoreIntSources(dmaIntStatus, timeIntStatus, plcExtIntStatus);
}

static void lRF215_HAL_SpiTransferFinished(RF215_SPI_TRANSFER_OBJ* transfer)
{
    uint64_t callbackTime;
    uintptr_t callbackContext;
    void* callbackData;
    RF215_SPI_TRANSFER_OBJ* next;
    RF215_SPI_TRANSFER_CALLBACK callback;

    if (transfer->mode == RF215_SPI_READ)
    {
        /* Copy SPI received data to buffer from upper layer */
        (void) memcpy(transfer->pData, (void*)&halSpiRxData[RF215_SPI_CMD_SIZE], transfer->size);
    }

    /* Copy needed data to local variables */
    callback = transfer->callback;
    callbackContext = transfer->context;
    callbackData = transfer->pData;
    callbackTime = rf215HalObj.sysTimeTransfer;

    /* The transfer object can now be freed */
    transfer->inUse = false;
    rf215HalObj.dmaTransferInProgress = false;

    /* Check next transfer */
    next = transfer->next;
    if (next != NULL)
    {
        /* Move queue start to next transfer */
        rf215HalObj.spiQueueFirst = next;
    }
    else
    {
        /* No more SPI transfers in the queue */
        rf215HalObj.spiQueueFirst = NULL;
        rf215HalObj.spiQueueLast = NULL;
    }

    if (next != NULL)
    {
        /* Start next SPI transfer */
        if (next->fromTasks == false)
        {
            lRF215_HAL_SpiTransferStart(next->mode, next->regAddr, next->pData, next->size);
        }
        else
        {
            rf215HalObj.spiTransferFromTasks = true;
        }
    }

    if (callback != NULL)
    {
        /* Notify upper layer via callback */
        callback(callbackContext, callbackData, callbackTime);
    }

    /* External interrupt can now be enabled */
    lRF215_HAL_ExtIntEnable();
}

static void lRF215_HAL_SpiDmaHandler(SYS_DMA_TRANSFER_EVENT ev, uintptr_t ctxt)
{
    bool dmaIntStatus, timeIntStatus, plcExtIntStatus;
    RF215_SPI_TRANSFER_OBJ* transfer = rf215HalObj.spiQueueFirst;
    SYS_DMA_CHANNEL dmaChannel = (SYS_DMA_CHANNEL) ctxt;
    bool restartTransfer = false;

    if (transfer == NULL)
    {
        /* Empty SPI transfer queue, probably because of RF215_HAL_Reset */
        return;
    }

    /* Critical region to avoid conflicts */
    dmaIntStatus = lRF215_HAL_DisableIntSources(&timeIntStatus, &plcExtIntStatus);
    lRF215_HAL_ExtIntDisable();

    if ((rf215HalObj.dmaTransferInProgress == false) || (rf215HalObj.spiPlibIsBusy() == true))
    {
        /* DMA transfer not in progress or still in progress (shared SPI) */
        lRF215_HAL_ExtIntEnable();
        lRF215_HAL_RestoreIntSources(dmaIntStatus, timeIntStatus, plcExtIntStatus);
        return;
    }

    if (ev == SYS_DMA_TRANSFER_ERROR)
    {
        if (dmaChannel == DRV_RF215_SPI_TX_DMA_CH)
        {
            /* Set DMA error flag */
            rf215HalObj.dmaTxError = true;
        }
        else /* DRV_RF215_SPI_RX_DMA_CH */
        {
            /* Restart SPI transfer */
            restartTransfer = true;
        }
    }
    else /* SYS_DMA_TRANSFER_COMPLETE */
    {
        if (dmaChannel == DRV_RF215_SPI_TX_DMA_CH)
        {
            /* Clear DMA error flag */
            rf215HalObj.dmaTxError = false;
        }
        else /* DRV_RF215_SPI_RX_DMA_CH */
        {
            if (rf215HalObj.dmaTxError == true)
            {
                /* Restart SPI transfer */
                restartTransfer = true;
            }
            else
            {
                /* SPI transfer finished successfully */
                lRF215_HAL_SpiTransferFinished(transfer);
            }
        }
    }

    if (restartTransfer == true)
    {
        if (transfer->fromTasks == false)
        {
            lRF215_HAL_SpiTransferStart(transfer->mode, transfer->regAddr, transfer->pData, transfer->size);
        }
        else
        {
            rf215HalObj.spiTransferFromTasks = true;
        }
    }

    /* Leave critical region */
    lRF215_HAL_ExtIntEnable();
    lRF215_HAL_RestoreIntSources(dmaIntStatus, timeIntStatus, plcExtIntStatus);
}

static void lRF215_HAL_ExtIntHandler(PIO_PIN pin, uintptr_t context)
{
    /* Check if external interrupt is still active */
    if (SYS_PORT_PinRead((SYS_PORT_PIN)pin) == true)
    {
        DRV_RF215_ExtIntHandler();
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: RF215 Driver HAL Interface Implementation
// *****************************************************************************
// *****************************************************************************

void RF215_HAL_Initialize(const DRV_RF215_INIT * const init)
{
    /* Store interrupt sources */
    rf215HalObj.sysTimeIntSource = init->sysTimeIntSource;
    rf215HalObj.dmaIntSource = init->dmaIntSource;
    rf215HalObj.plcExtIntSource = init->plcExtIntSource;

    /* Store pointers to SPI PLIB functions */
    rf215HalObj.spiPlibIsBusy = init->spiPlibIsBusy;
    rf215HalObj.spiPlibSetChipSelect = init->spiPlibSetChipSelect;

    /* Store addresses of SPI transmit and receive registers */
    rf215HalObj.spiTxAddr = init->spiTransmitAddress;
    rf215HalObj.spiRxAddr = init->spiReceiveAddress;

    /* MISRA C-2012 deviation block start */
    /* MISRA C-2012 Rule 11.1 deviated twice. Deviation record ID - H3_MISRAC_2012_R_11_1_DR_1 */

    /* Register callback for SPI Transmit and Receive DMA */
    SYS_DMA_ChannelCallbackRegister(DRV_RF215_SPI_RX_DMA_CH,
            lRF215_HAL_SpiDmaHandler, (uintptr_t) DRV_RF215_SPI_RX_DMA_CH);
    SYS_DMA_ChannelCallbackRegister(DRV_RF215_SPI_TX_DMA_CH,
            lRF215_HAL_SpiDmaHandler, (uintptr_t) DRV_RF215_SPI_TX_DMA_CH);

    /* MISRA C-2012 deviation block end */

    /* Register callback for external interrupt pin */
    (void) PIO_PinInterruptCallbackRegister((PIO_PIN) DRV_RF215_EXT_INT_PIN,
            lRF215_HAL_ExtIntHandler, 0);

    /* Pin interrupt is disabled at initialization */
    rf215HalObj.extIntDisableCount = 1;
    rf215HalObj.firstReset = true;

    /* Zero initialization */
    rf215HalObj.spiQueueFirst = NULL;
    rf215HalObj.spiQueueLast = NULL;
    rf215HalObj.spiTransferFromTasks = false;
    rf215HalObj.dmaTxError = false;
    rf215HalObj.dmaTransferInProgress = false;
    rf215HalObj.ledRxOnCount = 0;
    for (uint8_t idx = 0; idx < RF215_SPI_TRANSFER_POOL_SIZE; idx++)
    {
        halSpiTransferPool[idx].inUse = false;
    }
}

void RF215_HAL_Deinitialize(void)
{
    bool dmaIntStatus, timeIntStatus, plcExtIntStatus;

    /* Critical region to avoid conflict in SPI transfer queue */
    dmaIntStatus = lRF215_HAL_DisableIntSources(&timeIntStatus, &plcExtIntStatus);
    lRF215_HAL_ExtIntDisable();

    /* Push reset pin */
    SYS_PORT_PinClear(DRV_RF215_RESET_PIN);

    /* Clear SPI transfer pool */
    rf215HalObj.spiQueueFirst = NULL;
    rf215HalObj.spiQueueLast = NULL;
    for (uint8_t idx = 0; idx < RF215_SPI_TRANSFER_POOL_SIZE; idx++)
    {
        halSpiTransferPool[idx].inUse = false;
    }

    /* Leave critical region. External interrupt disabled */
    lRF215_HAL_RestoreIntSources(dmaIntStatus, timeIntStatus, plcExtIntStatus);
}

void RF215_HAL_Reset(void)
{
    SYS_TIME_HANDLE timeHandle;
    bool dmaIntStatus, timeIntStatus, plcExtIntStatus;

    /* Critical region to avoid interrupts during reset */
    dmaIntStatus = lRF215_HAL_DisableIntSources(&timeIntStatus, &plcExtIntStatus);
    if (rf215HalObj.firstReset == false)
    {
        lRF215_HAL_ExtIntDisable();
    }

    if (rf215HalObj.spiQueueFirst != NULL)
    {
        /* Abort SPI transfer in progress */
        SYS_DMA_ChannelDisable(DRV_RF215_SPI_TX_DMA_CH);
        SYS_DMA_ChannelDisable(DRV_RF215_SPI_RX_DMA_CH);
    }

    /* Push reset pin */
    SYS_PORT_PinClear(DRV_RF215_RESET_PIN);

    /* Clear SPI transfer pool. Pending SPI transfers aborted */
    rf215HalObj.spiQueueFirst = NULL;
    rf215HalObj.spiQueueLast = NULL;
    rf215HalObj.spiTransferFromTasks = false;
    rf215HalObj.firstReset = false;
    for (uint8_t idx = 0; idx < RF215_SPI_TRANSFER_POOL_SIZE; idx++)
    {
        halSpiTransferPool[idx].inUse = false;
    }

    /* Perform reset pulse delay (SYS_TIME interrupt has to be enabled) */
    SYS_INT_SourceEnable(rf215HalObj.sysTimeIntSource);
    if (SYS_TIME_DelayUS(RF215_RST_PULSE_US, &timeHandle) == SYS_TIME_SUCCESS)
    {
        while (SYS_TIME_DelayIsComplete(timeHandle) == false){}
    }

    /* Disable again SYS_TIME interrupt */
    (void) SYS_INT_SourceDisable(rf215HalObj.sysTimeIntSource);

    /* Release reset pin and enable/restore interrupts */
    SYS_PORT_PinSet(DRV_RF215_RESET_PIN);
    lRF215_HAL_ExtIntEnable();
    lRF215_HAL_RestoreIntSources(dmaIntStatus, timeIntStatus, plcExtIntStatus);
}

void RF215_HAL_Tasks(void)
{

    if (rf215HalObj.spiTransferFromTasks == true)
    {
        RF215_SPI_TRANSFER_OBJ* transfer = rf215HalObj.spiQueueFirst;

        if (transfer != NULL)
        {
            bool dmaIntStatus, timeIntStatus, plcExtIntStatus;

            /* Critical region to avoid new SPI transfers */
            dmaIntStatus = lRF215_HAL_DisableIntSources(&timeIntStatus, &plcExtIntStatus);

            /* Start SPI transfer from tasks */
            lRF215_HAL_SpiTransferStart(transfer->mode, transfer->regAddr, transfer->pData, transfer->size);

            /* Leave critical region */
            lRF215_HAL_RestoreIntSources(dmaIntStatus, timeIntStatus, plcExtIntStatus);
        }

        rf215HalObj.spiTransferFromTasks = false;
    }
}

bool RF215_HAL_SpiLock(void)
{
    /* Disable interrupts to avoid new SPI transfers */
    RF215_HAL_OBJ* hObj = &rf215HalObj;
    hObj->dmaIntStatus = lRF215_HAL_DisableIntSources(&hObj->sysTimeIntStatus, &hObj->plcExtIntStatus);
    lRF215_HAL_ExtIntDisable();

    if (hObj->spiQueueFirst == NULL)
    {
        /* Check SPI status */
        return !hObj->spiPlibIsBusy();
    }
    else
    {
        /* SPI is busy */
        return false;
    }
}

void RF215_HAL_SpiUnlock(void)
{
    /* Restore interrupts */
    RF215_HAL_OBJ* hObj = &rf215HalObj;
    lRF215_HAL_ExtIntEnable();
    lRF215_HAL_RestoreIntSources(hObj->dmaIntStatus, hObj->sysTimeIntStatus, hObj->plcExtIntStatus);
}

void RF215_HAL_EnterCritical(void)
{
    bool intStatus = SYS_INT_Disable();

    /* Critical region: Disable interrupts to avoid conflicts
     * External interrupt not disabled because it just makes one SPI transfer
     * and there is no other static variable update */
    RF215_HAL_OBJ* hObj = &rf215HalObj;
    hObj->dmaIntStatus = lRF215_HAL_DisableIntSources(&hObj->sysTimeIntStatus, &hObj->plcExtIntStatus);

    SYS_INT_Restore(intStatus);
}

void RF215_HAL_LeaveCritical(void)
{
    bool intStatus = SYS_INT_Disable();

    /* Leave critical region: Restore interrupts */
    RF215_HAL_OBJ* hObj = &rf215HalObj;
    lRF215_HAL_RestoreIntSources(hObj->dmaIntStatus, hObj->sysTimeIntStatus, hObj->plcExtIntStatus);

    SYS_INT_Restore(intStatus);
}

void RF215_HAL_SpiRead (
    uint16_t addr,
    void* pData,
    size_t size,
    RF215_SPI_TRANSFER_CALLBACK cb,
    uintptr_t context
)
{
    lRF215_HAL_SpiTransfer(RF215_SPI_READ, addr, pData, size, false, cb, context);
}

void RF215_HAL_SpiReadFromTasks (
    uint16_t addr,
    void* pData,
    size_t size,
    RF215_SPI_TRANSFER_CALLBACK cb,
    uintptr_t context
)
{
    lRF215_HAL_SpiTransfer(RF215_SPI_READ, addr, pData, size, true, cb, context);
}

void RF215_HAL_SpiWrite (
    uint16_t addr,
    void* pData,
    size_t size
)
{
    lRF215_HAL_SpiTransfer(RF215_SPI_WRITE, addr, pData, size, false, NULL, 0);
}

void RF215_HAL_SpiWriteUpdate (
    uint16_t addr,
    uint8_t* pDataNew,
    uint8_t* pDataOld,
    size_t size
)
{
    uint8_t* pDataWrite = pDataOld;
    size_t sizeWrite = 0U;
    size_t sizeSame = 0U;
    uint16_t addrWrite = addr;

    for (uint8_t idx = 0U; idx < size; idx++)
    {
        if (pDataNew[idx] != pDataOld[idx])
        {
            /* Different byte, needs to be written. Update static array to store
             * register values */
            pDataOld[idx] = pDataNew[idx];

            /* Split the SPI transfer only if it is worth: 3 consecutive bytes
             * not updated (2 SPI header bytes) */
            if (sizeSame > RF215_SPI_CMD_SIZE)
            {
                RF215_HAL_SpiWrite(addrWrite, pDataWrite, sizeWrite);
                sizeWrite = 0U;
                sizeSame = 0U;
            }

            /* Check start of SPI transfer block */
            if (sizeWrite == 0U)
            {
                addrWrite = addr + idx;
                pDataWrite = &pDataOld[idx];
            }

            /* Update write counters */
            sizeWrite += (sizeSame + 1U);
            sizeSame = 0U;
        }
        else
        {
            if (sizeWrite != 0U)
            {
                /* Same byte and SPI transfer block started */
                sizeSame += 1U;
            }
        }
    }

    if (sizeWrite != 0U)
    {
        /* Send the last SPI transfer */
        RF215_HAL_SpiWrite(addrWrite, pDataWrite, sizeWrite);
    }
}

size_t RF215_HAL_GetSpiQueueSize(void)
{
    size_t queueSize = 0;
    RF215_SPI_TRANSFER_OBJ* transfer = rf215HalObj.spiQueueFirst;

    while (transfer != NULL)
    {
        queueSize += transfer->size + RF215_SPI_CMD_SIZE;
        transfer = transfer->next;
    }

    return queueSize;
}

void RF215_HAL_LedRx(bool on)
{
    if (on == true)
    {
        SYS_PORT_PinSet(DRV_RF215_LED_RX_PIN);
        rf215HalObj.ledRxOnCount++;
    }
    else
    {
        if (rf215HalObj.ledRxOnCount > 0U)
        {
            rf215HalObj.ledRxOnCount--;
        }

        if (rf215HalObj.ledRxOnCount == 0U)
        {
            SYS_PORT_PinClear(DRV_RF215_LED_RX_PIN);
        }
    }
}

