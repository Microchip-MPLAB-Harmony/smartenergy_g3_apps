/*******************************************************************************
  FLEXCOM1 SPI PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_flexcom1_spi_master.c

  Summary:
    FLEXCOM1 SPI Master PLIB Implementation File.

  Description:
    This file defines the interface to the FLEXCOM SPI peripheral library.
    This library provides access to and control of the associated
    peripheral instance.

  Remarks:
    None.

*******************************************************************************/

// DOM-IGNORE-BEGIN
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
// DOM-IGNORE-END

#include "plib_flexcom1_spi_master.h"
#include "interrupts.h"

// *****************************************************************************
// *****************************************************************************
// Section: FLEXCOM1 SPI Implementation
// *****************************************************************************
// *****************************************************************************
/* Global object to save FLEXCOM SPI Exchange related data */
static FLEXCOM_SPI_OBJECT flexcom1SpiObj;

static uint8_t dummyDataBuffer[512];

static void setupDMA( void* pTransmitData, void* pReceiveData, size_t size )
{
    /* Always set up the rx channel first */
    FLEXCOM1_REGS->FLEX_RPR = (uint32_t) pReceiveData;
    FLEXCOM1_REGS->FLEX_RCR = (uint32_t) size;
    FLEXCOM1_REGS->FLEX_TPR = (uint32_t) pTransmitData;
    FLEXCOM1_REGS->FLEX_TCR = (uint32_t) size;
    FLEXCOM1_REGS->FLEX_PTCR = FLEX_PTCR_RXTEN_Msk | FLEX_PTCR_TXTEN_Msk;
    FLEXCOM1_REGS->FLEX_SPI_IER = FLEX_SPI_IER_ENDRX_Msk;
}


void FLEXCOM1_SPI_Initialize ( void )
{
    /* Set FLEXCOM SPI operating mode */
    FLEXCOM1_REGS->FLEX_MR = FLEX_MR_OPMODE_SPI;

    /* Disable and Reset the FLEXCOM SPI */
    FLEXCOM1_REGS->FLEX_SPI_CR = FLEX_SPI_CR_SPIDIS_Msk | FLEX_SPI_CR_SWRST_Msk;


    /* Enable Master mode, select clock source, select particular NPCS line for chip select and disable mode fault detection */
    FLEXCOM1_REGS->FLEX_SPI_MR = FLEX_SPI_MR_MSTR_Msk | FLEX_SPI_MR_BRSRCCLK_PERIPH_CLK | FLEX_SPI_MR_DLYBCS(0U) | FLEX_SPI_MR_PCS((uint32_t)FLEXCOM_SPI_CHIP_SELECT_NPCS0) | FLEX_SPI_MR_MODFDIS_Msk;

    /* Set up clock Polarity, data phase, Communication Width, Baud Rate */
    FLEXCOM1_REGS->FLEX_SPI_CSR[0]= FLEX_SPI_CSR_CPOL(0U) | FLEX_SPI_CSR_NCPHA(1U) | FLEX_SPI_CSR_BITS_8_BIT | FLEX_SPI_CSR_SCBR(12U) | FLEX_SPI_CSR_DLYBS(0U) | FLEX_SPI_CSR_DLYBCT(0U) | FLEX_SPI_CSR_CSAAT_Msk;




    /* Initialize global variables */
    flexcom1SpiObj.transferIsBusy = false;
    flexcom1SpiObj.callback = NULL;

    /* Enable FLEXCOM1 SPI */
    FLEXCOM1_REGS->FLEX_SPI_CR = FLEX_SPI_CR_SPIEN_Msk;
    return;
}



bool FLEXCOM1_SPI_WriteRead (void* pTransmitData, size_t txSize, void* pReceiveData, size_t rxSize)
{
    bool isRequestAccepted = false;
    uint32_t size = 0;

    /* Verify the request */
    if((((txSize > 0) && (pTransmitData != NULL)) || ((rxSize > 0) && (pReceiveData != NULL))) && (flexcom1SpiObj.transferIsBusy == false))
    {
        isRequestAccepted = true;

        flexcom1SpiObj.transferIsBusy = true;

        flexcom1SpiObj.txBuffer = pTransmitData;
        flexcom1SpiObj.rxBuffer = pReceiveData;
        flexcom1SpiObj.txCount = txSize;
        flexcom1SpiObj.rxCount = rxSize;

        if ((txSize > 0) && (rxSize > 0))
        {
            /* Find the lower value among txSize and rxSize */
            (txSize >= rxSize) ? (size = rxSize) : (size = txSize);

            /* Calculate the remaining tx/rx bytes and total bytes transferred */
            flexcom1SpiObj.rxCount -= size;
            flexcom1SpiObj.txCount -= size;
            flexcom1SpiObj.nBytesTransferred = size;

            setupDMA(pTransmitData, pReceiveData, size);
        }
        else
        {
            if (rxSize > 0)
            {
                /* txSize is 0. Need to use the dummy data buffer for transmission.
                 * Find out the max data that can be received, given the limited size of the dummy data buffer.
                 */
                (rxSize > sizeof(dummyDataBuffer)) ?
                    (size = sizeof(dummyDataBuffer)): (size = rxSize);

                /* Calculate the remaining rx bytes and total bytes transferred */
                flexcom1SpiObj.rxCount -= size;
                flexcom1SpiObj.nBytesTransferred = size;

                setupDMA(dummyDataBuffer, pReceiveData, size);
            }
            else
            {
                /* rxSize is 0. Need to use the dummy data buffer for reception.
                 * Find out the max data that can be transmitted, given the limited size of the dummy data buffer.
                 */
                (txSize > sizeof(dummyDataBuffer)) ?
                    (size = sizeof(dummyDataBuffer)): (size = txSize);

                /* Calculate the remaining tx bytes and total bytes transferred */
                flexcom1SpiObj.txCount -= size;
                flexcom1SpiObj.nBytesTransferred = size;

                setupDMA(pTransmitData, dummyDataBuffer, size);
            }
        }
    }

    return isRequestAccepted;
}

bool FLEXCOM1_SPI_TransferSetup (FLEXCOM_SPI_TRANSFER_SETUP * setup, uint32_t spiSourceClock )
{
    uint32_t scbr;
    bool setupStatus = false;
    if ((setup != NULL) && (setup->clockFrequency != 0U))
    {
        if(spiSourceClock == 0U)
        {
            // Fetch Master Clock Frequency directly
            spiSourceClock = 100000000;
        }

        scbr = spiSourceClock/setup->clockFrequency;

        if(scbr == 0U)
        {
            scbr = 1;
        }
        else if(scbr > 255U)
        {
            scbr = 255;
        }
        else
        {
            /* Do nothing */
        }

        FLEXCOM1_REGS->FLEX_SPI_CSR[0]= (FLEXCOM1_REGS->FLEX_SPI_CSR[0] & ~(FLEX_SPI_CSR_CPOL_Msk | FLEX_SPI_CSR_NCPHA_Msk | FLEX_SPI_CSR_BITS_Msk | FLEX_SPI_CSR_SCBR_Msk)) | ((uint32_t)setup->clockPolarity | (uint32_t)setup->clockPhase | (uint32_t)setup->dataBits | FLEX_SPI_CSR_SCBR(scbr));

        setupStatus = true;
    }
    return setupStatus;
}

bool FLEXCOM1_SPI_Write(void* pTransmitData, size_t txSize)
{
    return(FLEXCOM1_SPI_WriteRead(pTransmitData, txSize, NULL, 0));
}

bool FLEXCOM1_SPI_Read(void* pReceiveData, size_t rxSize)
{
    return(FLEXCOM1_SPI_WriteRead(NULL, 0, pReceiveData, rxSize));
}

bool FLEXCOM1_SPI_IsTransmitterBusy(void)
{
    return ((FLEXCOM1_REGS->FLEX_SPI_SR & FLEX_SPI_SR_TXEMPTY_Msk) == 0U);
}

void FLEXCOM1_SPI_CallbackRegister (FLEXCOM_SPI_CALLBACK callback, uintptr_t context)
{
    flexcom1SpiObj.callback = callback;
    flexcom1SpiObj.context = context;
}

bool FLEXCOM1_SPI_IsBusy(void)
{
    return ((flexcom1SpiObj.transferIsBusy) || ((FLEXCOM1_REGS->FLEX_SPI_SR & FLEX_SPI_SR_TXEMPTY_Msk) == 0U));
}

void FLEXCOM1_InterruptHandler(void)
{
    uint32_t size;
    uint32_t index;

    /* save the status in global object before it gets cleared */
    flexcom1SpiObj.status = FLEXCOM1_REGS->FLEX_SPI_SR;

    FLEXCOM1_REGS->FLEX_PTCR = FLEX_PTCR_ERRCLR_Msk;

    if(flexcom1SpiObj.rxCount > 0)
    {
        /* txPending is 0. Need to use the dummy data buffer for transmission.
         * Find out the max data that can be received, given the limited size of the dummy data buffer.
         */
        (flexcom1SpiObj.rxCount > sizeof(dummyDataBuffer)) ?
            (size = sizeof(dummyDataBuffer)): (size = flexcom1SpiObj.rxCount);

        index = flexcom1SpiObj.nBytesTransferred;

        /* Calculate the remaining rx bytes and total bytes transferred */
        flexcom1SpiObj.rxCount -= size;
        flexcom1SpiObj.nBytesTransferred += size;

        setupDMA(dummyDataBuffer,(void *)&((uint8_t*)flexcom1SpiObj.rxBuffer)[index],size);
    }
    else if(flexcom1SpiObj.txCount > 0)
    {
        /* rxSize is 0. Need to use the dummy data buffer for reception.
         * Find out the max data that can be transmitted, given the limited size of the dummy data buffer.
         */
        (flexcom1SpiObj.txCount > sizeof(dummyDataBuffer)) ?
            (size = sizeof(dummyDataBuffer)): (size = flexcom1SpiObj.txCount);

        index = flexcom1SpiObj.nBytesTransferred;

        /* Calculate the remaining rx bytes and total bytes transferred */
        flexcom1SpiObj.txCount -= size;
        flexcom1SpiObj.nBytesTransferred += size;

        setupDMA((void *)&((uint8_t*)flexcom1SpiObj.txBuffer)[index], dummyDataBuffer, size);
    }
    else
    {
        flexcom1SpiObj.transferIsBusy = false;

        /* Set Last transfer to deassert NPCS after the last byte written in TDR has been transferred. */
        FLEXCOM1_REGS->FLEX_SPI_CR = FLEX_SPI_CR_LASTXFER_Msk;

        FLEXCOM1_REGS->FLEX_PTCR = FLEX_PTCR_RXTDIS_Msk | FLEX_PTCR_TXTDIS_Msk;
        FLEXCOM1_REGS->FLEX_SPI_IDR = FLEX_SPI_IDR_ENDRX_Msk; 

        if( flexcom1SpiObj.callback != NULL )
        {
            flexcom1SpiObj.callback(flexcom1SpiObj.context);
        }
    }
}


/*******************************************************************************
 End of File
*/

