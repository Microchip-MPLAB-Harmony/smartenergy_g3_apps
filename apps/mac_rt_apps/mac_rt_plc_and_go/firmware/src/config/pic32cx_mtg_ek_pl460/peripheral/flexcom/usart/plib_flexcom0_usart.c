/*******************************************************************************
  FLEXCOM0 USART PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_flexcom0_usart.c

  Summary:
    FLEXCOM0 USART PLIB Implementation File

  Description
    This file defines the interface to the FLEXCOM0 USART
    peripheral library. This library provides access to and control of the
    associated peripheral instance.

  Remarks:
    None.
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

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "plib_flexcom0_usart.h"
#include "interrupts.h"

#define FLEXCOM0_USART_READ_BUFFER_SIZE             128
#define FLEXCOM0_USART_READ_BUFFER_SIZE_9BIT        (128 >> 1)

#define FLEXCOM0_USART_HW_RX_FIFO_THRES                 4
#define FLEXCOM0_USART_HW_TX_FIFO_THRES                 4

/* Disable Read, Overrun, Parity and Framing error interrupts */
#define FLEXCOM0_USART_RX_INT_DISABLE()      do { \
FLEXCOM0_REGS->FLEX_US_IDR = (FLEX_US_IDR_FRAME_Msk | FLEX_US_IDR_PARE_Msk | FLEX_US_IDR_OVRE_Msk); \
FLEXCOM0_REGS->FLEX_US_FIDR = FLEX_US_FIDR_RXFTHF_Msk; \
}while(0)

/* Enable Read, Overrun, Parity and Framing error interrupts */
#define FLEXCOM0_USART_RX_INT_ENABLE()       do { \
FLEXCOM0_REGS->FLEX_US_IER = (FLEX_US_IER_FRAME_Msk | FLEX_US_IER_PARE_Msk | FLEX_US_IER_OVRE_Msk); \
FLEXCOM0_REGS->FLEX_US_FIER = FLEX_US_FIER_RXFTHF_Msk; \
}while(0)

#define FLEXCOM0_USART_TX_INT_DISABLE()      do { \
FLEXCOM0_REGS->FLEX_US_IDR = FLEX_US_IDR_TXRDY_Msk; \
FLEXCOM0_REGS->FLEX_US_FIDR = (FLEX_US_FIDR_TXFTHF_Msk); \
}while(0)

#define FLEXCOM0_USART_TX_INT_ENABLE()       do { \
FLEXCOM0_REGS->FLEX_US_IER = FLEX_US_IER_TXRDY_Msk; \
FLEXCOM0_REGS->FLEX_US_FIER = (FLEX_US_FIER_TXFTHF_Msk); \
}while(0)

static uint8_t FLEXCOM0_USART_ReadBuffer[FLEXCOM0_USART_READ_BUFFER_SIZE];

#define FLEXCOM0_USART_WRITE_BUFFER_SIZE            512
#define FLEXCOM0_USART_WRITE_BUFFER_SIZE_9BIT       (512 >> 1)

static uint8_t FLEXCOM0_USART_WriteBuffer[FLEXCOM0_USART_WRITE_BUFFER_SIZE];

// *****************************************************************************
// *****************************************************************************
// Section: FLEXCOM0 USART Ring Buffer Implementation
// *****************************************************************************
// *****************************************************************************

FLEXCOM_USART_RING_BUFFER_OBJECT flexcom0UsartObj;

void FLEXCOM0_USART_Initialize( void )
{
    /* Set FLEXCOM USART operating mode */
    FLEXCOM0_REGS->FLEX_MR = FLEX_MR_OPMODE_USART;

    /* Reset FLEXCOM0 USART */
    FLEXCOM0_REGS->FLEX_US_CR = (FLEX_US_CR_RSTRX_Msk | FLEX_US_CR_RSTTX_Msk | FLEX_US_CR_RSTSTA_Msk);

    FLEXCOM0_REGS->FLEX_US_CR = FLEX_US_CR_FIFOEN_Msk;

    FLEXCOM0_REGS->FLEX_US_FMR = FLEX_US_FMR_RXFTHRES(FLEXCOM0_USART_HW_RX_FIFO_THRES) | FLEX_US_FMR_TXFTHRES(FLEXCOM0_USART_HW_TX_FIFO_THRES);

    FLEXCOM0_REGS->FLEX_US_TTGR = 0;

    /* Enable FLEXCOM0 USART */
    FLEXCOM0_REGS->FLEX_US_CR = (FLEX_US_CR_TXEN_Msk | FLEX_US_CR_RXEN_Msk);

    /* Configure FLEXCOM0 USART mode */
    FLEXCOM0_REGS->FLEX_US_MR = ( FLEX_US_MR_USART_MODE_NORMAL | FLEX_US_MR_USCLKS_MCK | FLEX_US_MR_CHRL_8_BIT | FLEX_US_MR_PAR_NO | FLEX_US_MR_NBSTOP_1_BIT | (0 << FLEX_US_MR_OVER_Pos));

    /* Configure FLEXCOM0 USART Baud Rate */
    FLEXCOM0_REGS->FLEX_US_BRGR = FLEX_US_BRGR_CD(54) | FLEX_US_BRGR_FP(2);

    flexcom0UsartObj.rdCallback = NULL;
    flexcom0UsartObj.rdInIndex = 0;
    flexcom0UsartObj.rdOutIndex = 0;
    flexcom0UsartObj.isRdNotificationEnabled = false;
    flexcom0UsartObj.isRdNotifyPersistently = false;
    flexcom0UsartObj.rdThreshold = 0;
    flexcom0UsartObj.rdBufferSize = FLEXCOM0_USART_READ_BUFFER_SIZE;
    flexcom0UsartObj.wrCallback = NULL;
    flexcom0UsartObj.wrInIndex = 0;
    flexcom0UsartObj.wrOutIndex = 0;
    flexcom0UsartObj.isWrNotificationEnabled = false;
    flexcom0UsartObj.isWrNotifyPersistently = false;
    flexcom0UsartObj.wrThreshold = 0;
    flexcom0UsartObj.wrBufferSize = FLEXCOM0_USART_WRITE_BUFFER_SIZE;
    flexcom0UsartObj.errorStatus = FLEXCOM_USART_ERROR_NONE;
    flexcom0UsartObj.isInterruptActive = false;

    if (FLEXCOM0_REGS->FLEX_US_MR & FLEX_US_MR_MODE9_Msk)
    {
        flexcom0UsartObj.rdBufferSize = FLEXCOM0_USART_READ_BUFFER_SIZE_9BIT;
        flexcom0UsartObj.wrBufferSize = FLEXCOM0_USART_WRITE_BUFFER_SIZE_9BIT;
    }
    else
    {
        flexcom0UsartObj.rdBufferSize = FLEXCOM0_USART_READ_BUFFER_SIZE;
        flexcom0UsartObj.wrBufferSize = FLEXCOM0_USART_WRITE_BUFFER_SIZE;
    }

    FLEXCOM0_USART_RX_INT_ENABLE();
}

void static FLEXCOM0_USART_ErrorClear( void )
{
    uint16_t dummyData = 0u;

    /* Clear the error flags */
    FLEXCOM0_REGS->FLEX_US_CR = FLEX_US_CR_RSTSTA_Msk;

    /* Flush existing error bytes from the RX FIFO */
    while( FLEX_US_CSR_RXRDY_Msk == (FLEXCOM0_REGS->FLEX_US_CSR& FLEX_US_CSR_RXRDY_Msk) )
    {
        if (FLEXCOM0_REGS->FLEX_US_MR & FLEX_US_MR_MODE9_Msk)
        {
            dummyData = *((uint16_t*)&FLEXCOM0_REGS->FLEX_US_RHR) & FLEX_US_RHR_RXCHR_Msk;
        }
        else
        {
            dummyData = *((uint8_t*)&FLEXCOM0_REGS->FLEX_US_RHR);
        }
    }

    /* Ignore the warning */
    (void)dummyData;

    return;
}

FLEXCOM_USART_ERROR FLEXCOM0_USART_ErrorGet( void )
{
    FLEXCOM_USART_ERROR errors = flexcom0UsartObj.errorStatus;

    flexcom0UsartObj.errorStatus = FLEXCOM_USART_ERROR_NONE;

    /* All errors are cleared, but send the previous error state */
    return errors;
}

static void FLEXCOM0_USART_BaudCalculate(uint32_t srcClkFreq, uint32_t reqBaud, uint8_t overSamp, uint32_t* cd, uint32_t* fp, uint32_t* baudError)
{
    uint32_t actualBaud = 0;

    *cd = srcClkFreq / (reqBaud * 8 * (2 - overSamp));

    if (*cd > 0)
    {
        *fp = ((srcClkFreq / (reqBaud * (2 - overSamp))) - ((*cd) * 8));
        actualBaud = (srcClkFreq / (((*cd) * 8) + (*fp))) / (2 - overSamp);
        *baudError = ((100 * actualBaud)/reqBaud) - 100;
    }
}

bool FLEXCOM0_USART_SerialSetup( FLEXCOM_USART_SERIAL_SETUP *setup, uint32_t srcClkFreq )
{
    uint32_t baud = 0;
    uint32_t overSampVal = 0;
    uint32_t usartMode;
    uint32_t cd0, fp0, cd1, fp1, baudError0, baudError1;
    bool status = false;

    cd0 = fp0 = cd1 = fp1 = baudError0 = baudError1 = 0;

    if (setup != NULL)
    {
        baud = setup->baudRate;

        if(srcClkFreq == 0)
        {
            srcClkFreq = FLEXCOM0_USART_FrequencyGet();
        }

        /* Calculate baud register values for 8x/16x oversampling values */

        FLEXCOM0_USART_BaudCalculate(srcClkFreq, baud, 0, &cd0, &fp0, &baudError0);
        FLEXCOM0_USART_BaudCalculate(srcClkFreq, baud, 1, &cd1, &fp1, &baudError1);

        if ( !(cd0 > 0 && cd0 <= 65535) && !(cd1 > 0 && cd1 <= 65535) )
        {
            /* Requested baud cannot be generated with current clock settings */
            return status;
        }

        if ( (cd0 > 0 && cd0 <= 65535) && (cd1 > 0 && cd1 <= 65535) )
        {
            /* Requested baud can be generated with both 8x and 16x oversampling. Select the one with less % error. */
            if (baudError1 < baudError0)
            {
                cd0 = cd1;
                fp0 = fp1;
                overSampVal = (1 << FLEX_US_MR_OVER_Pos) & FLEX_US_MR_OVER_Msk;
            }
        }
        else
        {
            /* Requested baud can be generated with either with 8x oversampling or with 16x oversampling. Select valid one. */
            if (cd1 > 0 && cd1 <= 65535)
            {
                cd0 = cd1;
                fp0 = fp1;
                overSampVal = (1 << FLEX_US_MR_OVER_Pos) & FLEX_US_MR_OVER_Msk;
            }
        }

        /* Configure FLEXCOM0 USART mode */
        usartMode = FLEXCOM0_REGS->FLEX_US_MR;
        usartMode &= ~(FLEX_US_MR_CHRL_Msk | FLEX_US_MR_MODE9_Msk | FLEX_US_MR_PAR_Msk | FLEX_US_MR_NBSTOP_Msk | FLEX_US_MR_OVER_Msk);
        FLEXCOM0_REGS->FLEX_US_MR = usartMode | ((uint32_t)setup->dataWidth | (uint32_t)setup->parity | (uint32_t)setup->stopBits | overSampVal);

        /* Configure FLEXCOM0 USART Baud Rate */
        FLEXCOM0_REGS->FLEX_US_BRGR = FLEX_US_BRGR_CD(cd0) | FLEX_US_BRGR_FP(fp0);

        if (FLEXCOM0_REGS->FLEX_US_MR & FLEX_US_MR_MODE9_Msk)
        {
            flexcom0UsartObj.rdBufferSize = FLEXCOM0_USART_READ_BUFFER_SIZE_9BIT;
            flexcom0UsartObj.wrBufferSize = FLEXCOM0_USART_WRITE_BUFFER_SIZE_9BIT;
        }
        else
        {
            flexcom0UsartObj.rdBufferSize = FLEXCOM0_USART_READ_BUFFER_SIZE;
            flexcom0UsartObj.wrBufferSize = FLEXCOM0_USART_WRITE_BUFFER_SIZE;
        }

        status = true;
    }

    return status;
}

/* This routine is only called from ISR. Hence do not disable/enable USART interrupts. */
static bool FLEXCOM0_USART_TxPullByte(uint16_t* pWrByte)
{
    bool isSuccess = false;
    uint32_t wrOutIndex = flexcom0UsartObj.wrOutIndex;
    uint32_t wrInIndex = flexcom0UsartObj.wrInIndex;

    if (wrOutIndex != wrInIndex)
    {
        if (FLEXCOM0_REGS->FLEX_US_MR & FLEX_US_MR_MODE9_Msk)
        {
            *pWrByte = ((uint16_t*)&FLEXCOM0_USART_WriteBuffer)[wrOutIndex++];
        }
        else
        {
            *pWrByte = FLEXCOM0_USART_WriteBuffer[wrOutIndex++];
        }

        if (wrOutIndex >= flexcom0UsartObj.wrBufferSize)
        {
            wrOutIndex = 0;
        }

        flexcom0UsartObj.wrOutIndex = wrOutIndex;

        isSuccess = true;
    }

    return isSuccess;
}

static inline bool FLEXCOM0_USART_TxPushByte(uint16_t wrByte)
{
    uint32_t tempInIndex;
    uint32_t wrOutIndex;
    uint32_t wrInIndex;
    bool isSuccess = false;

    /* Take a snapshot of indices to avoid creation of critical section */
    wrOutIndex = flexcom0UsartObj.wrOutIndex;
    wrInIndex = flexcom0UsartObj.wrInIndex;

    tempInIndex = wrInIndex + 1;

    if (tempInIndex >= flexcom0UsartObj.wrBufferSize)
    {
        tempInIndex = 0;
    }
    if (tempInIndex != wrOutIndex)
    {
        if (FLEXCOM0_REGS->FLEX_US_MR & FLEX_US_MR_MODE9_Msk)
        {
            ((uint16_t*)&FLEXCOM0_USART_WriteBuffer)[wrInIndex] = wrByte;
        }
        else
        {
            FLEXCOM0_USART_WriteBuffer[wrInIndex] = (uint8_t)wrByte;
        }

        flexcom0UsartObj.wrInIndex = tempInIndex;
        isSuccess = true;
    }
    else
    {
        /* Queue is full. Report Error. */
    }

    return isSuccess;
}

/* This routine is only called from ISR. Hence do not disable/enable USART interrupts. */
static void FLEXCOM0_USART_WriteNotificationSend(void)
{
    uint32_t nFreeWrBufferCount;

    if (flexcom0UsartObj.isWrNotificationEnabled == true)
    {
        nFreeWrBufferCount = FLEXCOM0_USART_WriteFreeBufferCountGet();

        if(flexcom0UsartObj.wrCallback != NULL)
        {
            if (flexcom0UsartObj.isWrNotifyPersistently == true)
            {
                if (nFreeWrBufferCount >= flexcom0UsartObj.wrThreshold)
                {
                    flexcom0UsartObj.wrCallback(FLEXCOM_USART_EVENT_WRITE_THRESHOLD_REACHED, flexcom0UsartObj.wrContext);
                }
            }
            else
            {
                if (nFreeWrBufferCount == flexcom0UsartObj.wrThreshold)
                {
                    flexcom0UsartObj.wrCallback(FLEXCOM_USART_EVENT_WRITE_THRESHOLD_REACHED, flexcom0UsartObj.wrContext);
                }
            }
        }
    }
}

static size_t FLEXCOM0_USART_WritePendingBytesGet(void)
{
    size_t nPendingTxBytes;

    /* Take a snapshot of indices to avoid creation of critical section */

    uint32_t wrOutIndex = flexcom0UsartObj.wrOutIndex;
    uint32_t wrInIndex = flexcom0UsartObj.wrInIndex;

    if ( wrInIndex >=  wrOutIndex)
    {
        nPendingTxBytes =  wrInIndex -  wrOutIndex;
    }
    else
    {
        nPendingTxBytes =  (flexcom0UsartObj.wrBufferSize -  wrOutIndex) + wrInIndex;
    }

    return nPendingTxBytes;
}

size_t FLEXCOM0_USART_WriteCountGet(void)
{
    size_t nPendingTxBytes;

    nPendingTxBytes = FLEXCOM0_USART_WritePendingBytesGet();

    return nPendingTxBytes;
}

size_t FLEXCOM0_USART_Write(uint8_t* pWrBuffer, const size_t size )
{
    size_t nBytesWritten  = 0;

    while (nBytesWritten < size)
    {
        if (FLEXCOM0_REGS->FLEX_US_MR & FLEX_US_MR_MODE9_Msk)
        {
            if (FLEXCOM0_USART_TxPushByte(((uint16_t*)pWrBuffer)[nBytesWritten]) == true)
            {
                nBytesWritten++;
            }
            else
            {
                /* Queue is full, exit the loop */
                break;
            }
        }
        else
        {
            if (FLEXCOM0_USART_TxPushByte(pWrBuffer[nBytesWritten]) == true)
            {
                nBytesWritten++;
            }
            else
            {
                /* Queue is full, exit the loop */
                break;
            }
        }
    }

    /* Check if any data is pending for transmission */
    if (FLEXCOM0_USART_WritePendingBytesGet() > 0)
    {
        /* Enable TX interrupt as data is pending for transmission */
        FLEXCOM0_USART_TX_INT_ENABLE();
    }

    return nBytesWritten;
}

size_t FLEXCOM0_USART_WriteFreeBufferCountGet(void)
{
    return (flexcom0UsartObj.wrBufferSize - 1) - FLEXCOM0_USART_WriteCountGet();
}

size_t FLEXCOM0_USART_WriteBufferSizeGet(void)
{
    return (flexcom0UsartObj.wrBufferSize - 1);
}

bool FLEXCOM0_USART_TransmitComplete(void)
{
    bool status = false;

    if (FLEXCOM0_REGS->FLEX_US_CSR & FLEX_US_CSR_TXEMPTY_Msk)
    {
        status = true;
    }

    return status;
}

bool FLEXCOM0_USART_WriteNotificationEnable(bool isEnabled, bool isPersistent)
{
    bool previousStatus = flexcom0UsartObj.isWrNotificationEnabled;

    flexcom0UsartObj.isWrNotificationEnabled = isEnabled;

    flexcom0UsartObj.isWrNotifyPersistently = isPersistent;

    return previousStatus;
}

void FLEXCOM0_USART_WriteThresholdSet(uint32_t nBytesThreshold)
{
    if (nBytesThreshold > 0)
    {
        flexcom0UsartObj.wrThreshold = nBytesThreshold;
    }
}

void FLEXCOM0_USART_WriteCallbackRegister( FLEXCOM_USART_RING_BUFFER_CALLBACK callback, uintptr_t context)
{
    flexcom0UsartObj.wrCallback = callback;

    flexcom0UsartObj.wrContext = context;
}

/* This routine is only called from ISR. Hence do not disable/enable USART interrupts. */
static inline bool FLEXCOM0_USART_RxPushByte(uint16_t rdByte)
{
    uint32_t tempInIndex;
    bool isSuccess = false;

    tempInIndex = flexcom0UsartObj.rdInIndex + 1;

    if (tempInIndex >= flexcom0UsartObj.rdBufferSize)
    {
        tempInIndex = 0;
    }

    if (tempInIndex == flexcom0UsartObj.rdOutIndex)
    {
        /* Queue is full - Report it to the application. Application gets a chance to free up space by reading data out from the RX ring buffer */
        if(flexcom0UsartObj.rdCallback != NULL)
        {
            flexcom0UsartObj.rdCallback(FLEXCOM_USART_EVENT_READ_BUFFER_FULL, flexcom0UsartObj.rdContext);

            /* Read the indices again in case application has freed up space in RX ring buffer */
            tempInIndex = flexcom0UsartObj.rdInIndex + 1;

            if (tempInIndex >= flexcom0UsartObj.rdBufferSize)
            {
                tempInIndex = 0;
            }
        }
    }

    /* Attempt to push the data into the ring buffer */
    if (tempInIndex != flexcom0UsartObj.rdOutIndex)
    {
        if (FLEXCOM0_REGS->FLEX_US_MR & FLEX_US_MR_MODE9_Msk)
        {
            ((uint16_t*)&FLEXCOM0_USART_ReadBuffer)[flexcom0UsartObj.rdInIndex] = rdByte;
        }
        else
        {
            FLEXCOM0_USART_ReadBuffer[flexcom0UsartObj.rdInIndex] = (uint8_t)rdByte;
        }

        flexcom0UsartObj.rdInIndex = tempInIndex;
        isSuccess = true;
    }
    else
    {
        /* Queue is full. Data will be lost. */
    }

    return isSuccess;
}

/* This routine is only called from ISR. Hence do not disable/enable USART interrupts. */
static void FLEXCOM0_USART_ReadNotificationSend(void)
{
    uint32_t nUnreadBytesAvailable;

    if (flexcom0UsartObj.isRdNotificationEnabled == true)
    {
        nUnreadBytesAvailable = FLEXCOM0_USART_ReadCountGet();

        if(flexcom0UsartObj.rdCallback != NULL)
        {
            if (flexcom0UsartObj.isRdNotifyPersistently == true)
            {
                if (nUnreadBytesAvailable >= flexcom0UsartObj.rdThreshold)
                {
                    flexcom0UsartObj.rdCallback(FLEXCOM_USART_EVENT_READ_THRESHOLD_REACHED, flexcom0UsartObj.rdContext);
                }
            }
            else
            {
                if (nUnreadBytesAvailable == flexcom0UsartObj.rdThreshold)
                {
                    flexcom0UsartObj.rdCallback(FLEXCOM_USART_EVENT_READ_THRESHOLD_REACHED, flexcom0UsartObj.rdContext);
                }
            }
        }
    }
}

size_t FLEXCOM0_USART_Read(uint8_t* pRdBuffer, const size_t size)
{
    size_t nBytesRead = 0;
    uint32_t rdOutIndex = 0;
    uint32_t rdInIndex = 0;

    if ((flexcom0UsartObj.isInterruptActive == false) && (FLEXCOM0_REGS->FLEX_US_CSR & FLEX_US_CSR_RXRDY_Msk))
    {
        FLEXCOM0_REGS->FLEX_US_IER = FLEX_US_IER_RXRDY_Msk;

        while (FLEXCOM0_REGS->FLEX_US_CSR & FLEX_US_CSR_RXRDY_Msk);

        FLEXCOM0_REGS->FLEX_US_IDR = FLEX_US_IDR_RXRDY_Msk;
    }

    /* Take a snapshot of indices to avoid creation of critical section */
    rdOutIndex = flexcom0UsartObj.rdOutIndex;
    rdInIndex = flexcom0UsartObj.rdInIndex;

    while (nBytesRead < size)
    {
        if (rdOutIndex != rdInIndex)
        {
            if (FLEXCOM0_REGS->FLEX_US_MR & FLEX_US_MR_MODE9_Msk)
            {
                ((uint16_t*)pRdBuffer)[nBytesRead++] = ((uint16_t*)&FLEXCOM0_USART_ReadBuffer)[rdOutIndex++];
            }
            else
            {
                pRdBuffer[nBytesRead++] = FLEXCOM0_USART_ReadBuffer[rdOutIndex++];
            }

            if (rdOutIndex >= flexcom0UsartObj.rdBufferSize)
            {
                rdOutIndex = 0;
            }
        }
        else
        {
            /* No more data available in the RX buffer */
            break;
        }
    }

    flexcom0UsartObj.rdOutIndex = rdOutIndex;

    return nBytesRead;
}

size_t FLEXCOM0_USART_ReadCountGet(void)
{
    size_t nUnreadBytesAvailable;
    uint32_t rdOutIndex;
    uint32_t rdInIndex;

    if ((flexcom0UsartObj.isInterruptActive == false) && (FLEXCOM0_REGS->FLEX_US_CSR & FLEX_US_CSR_RXRDY_Msk))
    {
        FLEXCOM0_REGS->FLEX_US_IER = FLEX_US_IER_RXRDY_Msk;

        while (FLEXCOM0_REGS->FLEX_US_CSR & FLEX_US_CSR_RXRDY_Msk);

        FLEXCOM0_REGS->FLEX_US_IDR = FLEX_US_IDR_RXRDY_Msk;
    }

    /* Take a snapshot of indices to avoid creation of critical section */
    rdOutIndex = flexcom0UsartObj.rdOutIndex;
    rdInIndex = flexcom0UsartObj.rdInIndex;

    if ( rdInIndex >=  rdOutIndex)
    {
        nUnreadBytesAvailable =  rdInIndex -  rdOutIndex;
    }
    else
    {
        nUnreadBytesAvailable =  (flexcom0UsartObj.rdBufferSize -  rdOutIndex) + rdInIndex;
    }

    return nUnreadBytesAvailable;
}

size_t FLEXCOM0_USART_ReadFreeBufferCountGet(void)
{
    return (flexcom0UsartObj.rdBufferSize - 1) - FLEXCOM0_USART_ReadCountGet();
}

size_t FLEXCOM0_USART_ReadBufferSizeGet(void)
{
    return (flexcom0UsartObj.rdBufferSize - 1);
}

static void FLEXCOM0_USART_UpdateRXFIFOThreshold(void)
{
    size_t nUnreadBytesAvailable = 0;
    size_t nBytesPendingForRDThreshold = 0;

    if (flexcom0UsartObj.isRdNotificationEnabled == true)
    {
        nUnreadBytesAvailable = FLEXCOM0_USART_ReadCountGet();

        if (flexcom0UsartObj.rdThreshold > nUnreadBytesAvailable)
        {
            /* Don't have RX threshold number of bytes in the receive buffer.
             * Setup the hardware FIFO threshold level based on number of bytes remaining to reach the rdThreshold level */

            nBytesPendingForRDThreshold = flexcom0UsartObj.rdThreshold - nUnreadBytesAvailable;

            if (nBytesPendingForRDThreshold > FLEXCOM0_USART_HW_RX_FIFO_THRES)
            {
                nBytesPendingForRDThreshold = FLEXCOM0_USART_HW_RX_FIFO_THRES;
            }

            /* Set the new threshold level for hardware RX FIFO */
            FLEXCOM0_REGS->FLEX_US_FMR = (FLEXCOM0_REGS->FLEX_US_FMR & ~FLEX_US_FMR_RXFTHRES_Msk) | FLEX_US_FMR_RXFTHRES(nBytesPendingForRDThreshold);
        }
        else
        {
            /* Set the threshold to generate interrupt when the HW RX FIFO has RX threshold number of bytes */
            FLEXCOM0_REGS->FLEX_US_FMR = (FLEXCOM0_REGS->FLEX_US_FMR & ~FLEX_US_FMR_RXFTHRES_Msk) | FLEX_US_FMR_RXFTHRES(FLEXCOM0_USART_HW_RX_FIFO_THRES);
        }
    }
}

bool FLEXCOM0_USART_ReadNotificationEnable(bool isEnabled, bool isPersistent)
{
    bool previousStatus = flexcom0UsartObj.isRdNotificationEnabled;

    flexcom0UsartObj.isRdNotificationEnabled = isEnabled;

    flexcom0UsartObj.isRdNotifyPersistently = isPersistent;

    FLEXCOM0_USART_UpdateRXFIFOThreshold();

    return previousStatus;
}

void FLEXCOM0_USART_ReadThresholdSet(uint32_t nBytesThreshold)
{
    if (nBytesThreshold > 0)
    {
        flexcom0UsartObj.rdThreshold = nBytesThreshold;

        FLEXCOM0_USART_UpdateRXFIFOThreshold();
    }
}

void FLEXCOM0_USART_ReadCallbackRegister( FLEXCOM_USART_RING_BUFFER_CALLBACK callback, uintptr_t context)
{
    flexcom0UsartObj.rdCallback = callback;

    flexcom0UsartObj.rdContext = context;
}

void static FLEXCOM0_USART_ISR_RX_Handler( void )
{
    uint16_t rdData = 0;

    /* Keep reading until there is a character availabe in the RX FIFO */
    while(FLEX_US_CSR_RXRDY_Msk == (FLEXCOM0_REGS->FLEX_US_CSR & FLEX_US_CSR_RXRDY_Msk))
    {
        if (FLEXCOM0_REGS->FLEX_US_MR & FLEX_US_MR_MODE9_Msk)
        {
            rdData = *((uint16_t*)&FLEXCOM0_REGS->FLEX_US_RHR) & FLEX_US_RHR_RXCHR_Msk;
        }
        else
        {
            rdData = *((uint8_t*)&FLEXCOM0_REGS->FLEX_US_RHR);
        }

        if (FLEXCOM0_USART_RxPushByte( rdData ) == true)
        {
            FLEXCOM0_USART_ReadNotificationSend();
        }
        else
        {
            /* UART RX buffer is full */
        }
    }

    FLEXCOM0_USART_UpdateRXFIFOThreshold();

}

void static FLEXCOM0_USART_ISR_TX_Handler( void )
{
    uint16_t wrByte;

    /* Keep writing to the TX FIFO as long as there is space */
    while(FLEX_US_CSR_TXRDY_Msk == (FLEXCOM0_REGS->FLEX_US_CSR & FLEX_US_CSR_TXRDY_Msk))
    {
        if (FLEXCOM0_USART_TxPullByte(&wrByte) == true)
        {
            if (FLEXCOM0_REGS->FLEX_US_MR & FLEX_US_MR_MODE9_Msk)
            {
                *((uint16_t*)&FLEXCOM0_REGS->FLEX_US_THR) = wrByte & FLEX_US_THR_TXCHR_Msk;
            }
            else
            {
                *((uint8_t*)&FLEXCOM0_REGS->FLEX_US_THR) = (uint8_t)wrByte;
            }

            /* Send notification */
            FLEXCOM0_USART_WriteNotificationSend();
        }
        else
        {
            /* Nothing to transmit. Disable the data register empty/fifo Threshold interrupt. */
            FLEXCOM0_USART_TX_INT_DISABLE();
            break;
        }
    }

    /* At this point, either FIFO is completly full or all bytes are transmitted (copied in FIFO). If FIFO is full, then threshold interrupt
    *  will be generated. If all bytes are transmitted then interrupts are disabled as interrupt generation is not needed in ring buffer mode
    */

    /* Always disable the TXRDY interrupt. It is only used to start off transmission. */
    FLEXCOM0_REGS->FLEX_US_IDR = FLEX_US_IDR_TXRDY_Msk;
}

void FLEXCOM0_InterruptHandler( void )
{
    /* Channel status */
    uint32_t channelStatus = FLEXCOM0_REGS->FLEX_US_CSR;

    /* Error status */
    uint32_t errorStatus = (channelStatus & (FLEX_US_CSR_OVRE_Msk | FLEX_US_CSR_FRAME_Msk | FLEX_US_CSR_PARE_Msk));

    flexcom0UsartObj.isInterruptActive = true;

    if(errorStatus != 0)
    {
        /* Save the error so that it can be reported when application calls the FLEXCOM0_USART_ErrorGet() API */
        flexcom0UsartObj.errorStatus = (FLEXCOM_USART_ERROR)errorStatus;

        /* Clear the error flags and flush out the error bytes */
        FLEXCOM0_USART_ErrorClear();

        /* USART errors are normally associated with the receiver, hence calling receiver context */
        if( flexcom0UsartObj.rdCallback != NULL )
        {
            flexcom0UsartObj.rdCallback(FLEXCOM_USART_EVENT_READ_ERROR, flexcom0UsartObj.rdContext);
        }
    }

    /* Clear FIFO related interrupt flags */
    FLEXCOM0_REGS->FLEX_US_CR = FLEX_US_CR_RSTSTA_Msk;

    FLEXCOM0_USART_ISR_RX_Handler();

    FLEXCOM0_USART_ISR_TX_Handler();

    flexcom0UsartObj.isInterruptActive = false;
}
