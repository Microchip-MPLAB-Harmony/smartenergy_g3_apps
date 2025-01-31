/*******************************************************************************
  FLEXCOM5 SPI PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_flexcom5_spi_master.c

  Summary:
    FLEXCOM5 SPI Master PLIB Implementation File.

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

#include "plib_flexcom5_spi_master.h"
#include "interrupts.h"

#define FLEXCOM_SPI_TDR_8BIT_REG      (*(volatile uint8_t* const)((FLEXCOM5_BASE_ADDRESS + FLEX_SPI_TDR_REG_OFST)))

#define FLEXCOM_SPI_TDR_9BIT_REG      (*(volatile uint16_t* const)((FLEXCOM5_BASE_ADDRESS + FLEX_SPI_TDR_REG_OFST)))



#define FLEXCOM_SPI_RDR_8BIT_REG      (*(volatile uint8_t* const)((FLEXCOM5_BASE_ADDRESS + FLEX_SPI_RDR_REG_OFST)))

#define FLEXCOM_SPI_RDR_9BIT_REG      (*(volatile uint16_t* const)((FLEXCOM5_BASE_ADDRESS + FLEX_SPI_RDR_REG_OFST)))
// *****************************************************************************
// *****************************************************************************
// Section: FLEXCOM5 SPI Implementation
// *****************************************************************************
// *****************************************************************************
/* Global object to save FLEXCOM SPI Exchange related data */
volatile static FLEXCOM_SPI_OBJECT flexcom5SpiObj;

volatile static uint8_t dummyDataBuffer[512];

static void setupDMA( volatile void* pTransmitData, volatile void* pReceiveData, size_t size )
{
    /* Always set up the rx channel first */
    FLEXCOM5_REGS->FLEX_RPR = (volatile uint32_t) (volatile uint32_t*)pReceiveData;
    FLEXCOM5_REGS->FLEX_RCR = (uint32_t) size;
    FLEXCOM5_REGS->FLEX_TPR = (volatile uint32_t) (volatile uint32_t*)pTransmitData;
    FLEXCOM5_REGS->FLEX_TCR = (uint32_t) size;
    FLEXCOM5_REGS->FLEX_PTCR = FLEX_PTCR_RXTEN_Msk | FLEX_PTCR_TXTEN_Msk;
    FLEXCOM5_REGS->FLEX_SPI_IER = FLEX_SPI_IER_ENDRX_Msk;
}


void FLEXCOM5_SPI_Initialize ( void )
{
    /* Set FLEXCOM SPI operating mode */
    FLEXCOM5_REGS->FLEX_MR = FLEX_MR_OPMODE_SPI;

    /* Disable and Reset the FLEXCOM SPI */
    FLEXCOM5_REGS->FLEX_SPI_CR = FLEX_SPI_CR_SPIDIS_Msk | FLEX_SPI_CR_SWRST_Msk;


    /* Enable Master mode, select clock source, select particular NPCS line for chip select and disable mode fault detection */
    FLEXCOM5_REGS->FLEX_SPI_MR = FLEX_SPI_MR_MSTR_Msk | FLEX_SPI_MR_BRSRCCLK_PERIPH_CLK | FLEX_SPI_MR_DLYBCS(0U) | FLEX_SPI_MR_PCS((uint32_t)FLEXCOM_SPI_CHIP_SELECT_NPCS0) | FLEX_SPI_MR_MODFDIS_Msk;

    /* Set up clock Polarity, data phase, Communication Width, Baud Rate */
    FLEXCOM5_REGS->FLEX_SPI_CSR[0]= FLEX_SPI_CSR_CPOL(0U) | FLEX_SPI_CSR_NCPHA(1U) | FLEX_SPI_CSR_BITS_8_BIT | FLEX_SPI_CSR_SCBR(12U) | FLEX_SPI_CSR_DLYBS(0U) | FLEX_SPI_CSR_DLYBCT(0U) ;




    /* Initialize global variables */
    flexcom5SpiObj.transferIsBusy = false;
    flexcom5SpiObj.callback = NULL;

    /* Enable FLEXCOM5 SPI */
    FLEXCOM5_REGS->FLEX_SPI_CR = FLEX_SPI_CR_SPIEN_Msk;
    return;
}



bool FLEXCOM5_SPI_WriteRead (void* pTransmitData, size_t txSize, void* pReceiveData, size_t rxSize)
{
    bool isRequestAccepted = false;
    uint32_t size = 0;

    if (flexcom5SpiObj.transferIsBusy == false)
    {
        /* Verify the request */
        if(((txSize > 0U) && (pTransmitData != NULL)) || ((rxSize > 0U) && (pReceiveData != NULL)))
        {
            isRequestAccepted = true;

            flexcom5SpiObj.transferIsBusy = true;

            flexcom5SpiObj.txBuffer = pTransmitData;
            flexcom5SpiObj.rxBuffer = pReceiveData;
            flexcom5SpiObj.txCount = txSize;
            flexcom5SpiObj.rxCount = rxSize;

            if ((txSize > 0U) && (rxSize > 0U))
            {
                /* Find the lower value among txSize and rxSize */
                (txSize >= rxSize) ? (size = rxSize) : (size = txSize);

                /* Calculate the remaining tx/rx bytes and total bytes transferred */
                flexcom5SpiObj.rxCount -= size;
                flexcom5SpiObj.txCount -= size;
                flexcom5SpiObj.nBytesTransferred = size;

                setupDMA(pTransmitData, pReceiveData, size);
            }
            else
            {
                if (rxSize > 0U)
                {
                    /* txSize is 0. Need to use the dummy data buffer for transmission.
                     * Find out the max data that can be received, given the limited size of the dummy data buffer.
                     */
                    (rxSize > sizeof(dummyDataBuffer)) ?
                        (size = sizeof(dummyDataBuffer)): (size = rxSize);

                    /* Calculate the remaining rx bytes and total bytes transferred */
                    flexcom5SpiObj.rxCount -= size;
                    flexcom5SpiObj.nBytesTransferred = size;

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
                    flexcom5SpiObj.txCount -= size;
                    flexcom5SpiObj.nBytesTransferred = size;

                    setupDMA(pTransmitData, dummyDataBuffer, size);
                }
            }
        }
    }

    return isRequestAccepted;
}

bool FLEXCOM5_SPI_TransferSetup (FLEXCOM_SPI_TRANSFER_SETUP * setup, uint32_t spiSourceClock )
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

        FLEXCOM5_REGS->FLEX_SPI_CSR[0]= (FLEXCOM5_REGS->FLEX_SPI_CSR[0] & ~(FLEX_SPI_CSR_CPOL_Msk | FLEX_SPI_CSR_NCPHA_Msk | FLEX_SPI_CSR_BITS_Msk | FLEX_SPI_CSR_SCBR_Msk)) | ((uint32_t)setup->clockPolarity | (uint32_t)setup->clockPhase | (uint32_t)setup->dataBits | FLEX_SPI_CSR_SCBR(scbr));

        setupStatus = true;
    }
    return setupStatus;
}

bool FLEXCOM5_SPI_Write(void* pTransmitData, size_t txSize)
{
    return(FLEXCOM5_SPI_WriteRead(pTransmitData, txSize, NULL, 0));
}

bool FLEXCOM5_SPI_Read(void* pReceiveData, size_t rxSize)
{
    return(FLEXCOM5_SPI_WriteRead(NULL, 0, pReceiveData, rxSize));
}

bool FLEXCOM5_SPI_IsTransmitterBusy(void)
{
    return ((FLEXCOM5_REGS->FLEX_SPI_SR & FLEX_SPI_SR_TXEMPTY_Msk) == 0U);
}

void FLEXCOM5_SPI_CallbackRegister (FLEXCOM_SPI_CALLBACK callback, uintptr_t context)
{
    flexcom5SpiObj.callback = callback;
    flexcom5SpiObj.context = context;
}

bool FLEXCOM5_SPI_IsBusy(void)
{
    bool transferIsBusy = flexcom5SpiObj.transferIsBusy;

    return (((FLEXCOM5_REGS->FLEX_SPI_SR & FLEX_SPI_SR_TXEMPTY_Msk) == 0U) || (transferIsBusy));
}

void __attribute__((used)) FLEXCOM5_InterruptHandler(void)
{
    uint32_t size;
    uint32_t index;
    uintptr_t context = flexcom5SpiObj.context;

    /* save the status in global object before it gets cleared */
    flexcom5SpiObj.status = FLEXCOM5_REGS->FLEX_SPI_SR;

    FLEXCOM5_REGS->FLEX_PTCR = FLEX_PTCR_ERRCLR_Msk;

    if(flexcom5SpiObj.rxCount > 0U)
    {
        /* txPending is 0. Need to use the dummy data buffer for transmission.
         * Find out the max data that can be received, given the limited size of the dummy data buffer.
         */
        (flexcom5SpiObj.rxCount > sizeof(dummyDataBuffer)) ?
            (size = sizeof(dummyDataBuffer)): (size = flexcom5SpiObj.rxCount);

        index = flexcom5SpiObj.nBytesTransferred;

        /* Calculate the remaining rx bytes and total bytes transferred */
        flexcom5SpiObj.rxCount -= size;
        flexcom5SpiObj.nBytesTransferred += size;

        setupDMA(dummyDataBuffer,&((uint8_t*)flexcom5SpiObj.rxBuffer)[index],size);
    }
    else if(flexcom5SpiObj.txCount > 0U)
    {
        /* rxSize is 0. Need to use the dummy data buffer for reception.
         * Find out the max data that can be transmitted, given the limited size of the dummy data buffer.
         */
        (flexcom5SpiObj.txCount > sizeof(dummyDataBuffer)) ?
            (size = sizeof(dummyDataBuffer)): (size = flexcom5SpiObj.txCount);

        index = flexcom5SpiObj.nBytesTransferred;

        /* Calculate the remaining rx bytes and total bytes transferred */
        flexcom5SpiObj.txCount -= size;
        flexcom5SpiObj.nBytesTransferred += size;

        setupDMA(&((uint8_t*)flexcom5SpiObj.txBuffer)[index], dummyDataBuffer, size);
    }
    else
    {
        flexcom5SpiObj.transferIsBusy = false;

        /* Set Last transfer to deassert NPCS after the last byte written in TDR has been transferred. */
        FLEXCOM5_REGS->FLEX_SPI_CR = FLEX_SPI_CR_LASTXFER_Msk;

        FLEXCOM5_REGS->FLEX_PTCR = FLEX_PTCR_RXTDIS_Msk | FLEX_PTCR_TXTDIS_Msk;
        FLEXCOM5_REGS->FLEX_SPI_IDR = FLEX_SPI_IDR_ENDRX_Msk;

        if( flexcom5SpiObj.callback != NULL )
        {
            flexcom5SpiObj.callback(context);
        }
    }
}


/*******************************************************************************
 End of File
*/

