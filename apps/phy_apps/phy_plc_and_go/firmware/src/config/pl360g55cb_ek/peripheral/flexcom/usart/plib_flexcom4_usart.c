/*******************************************************************************
  FLEXCOM4 USART PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_flexcom4_usart.c

  Summary:
    FLEXCOM4 USART PLIB Implementation File

  Description
    This file defines the interface to the FLEXCOM4 USART
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

#include "plib_flexcom4_usart.h"
#include "interrupts.h"

#define FLEXCOM4_USART_READ_BUFFER_SIZE             128
#define FLEXCOM4_USART_READ_BUFFER_SIZE_9BIT        (128 >> 1)

/* Disable Read, Overrun, Parity and Framing error interrupts */
#define FLEXCOM4_USART_RX_INT_DISABLE()      USART4_REGS->US_IDR = (US_IDR_RXRDY_Msk | US_IDR_FRAME_Msk | US_IDR_PARE_Msk | US_IDR_OVRE_Msk)
/* Enable Read, Overrun, Parity and Framing error interrupts */
#define FLEXCOM4_USART_RX_INT_ENABLE()       USART4_REGS->US_IER = (US_IER_RXRDY_Msk | US_IER_FRAME_Msk | US_IER_PARE_Msk | US_IER_OVRE_Msk)

static uint8_t FLEXCOM4_USART_ReadBuffer[FLEXCOM4_USART_READ_BUFFER_SIZE];

#define FLEXCOM4_USART_WRITE_BUFFER_SIZE            128
#define FLEXCOM4_USART_WRITE_BUFFER_SIZE_9BIT       (128 >> 1)

#define FLEXCOM4_USART_TX_INT_DISABLE()      USART4_REGS->US_IDR = US_IDR_TXRDY_Msk
#define FLEXCOM4_USART_TX_INT_ENABLE()       USART4_REGS->US_IER = US_IER_TXRDY_Msk

static uint8_t FLEXCOM4_USART_WriteBuffer[FLEXCOM4_USART_WRITE_BUFFER_SIZE];

// *****************************************************************************
// *****************************************************************************
// Section: FLEXCOM4 USART Ring Buffer Implementation
// *****************************************************************************
// *****************************************************************************

FLEXCOM_USART_RING_BUFFER_OBJECT flexcom4UsartObj;

void FLEXCOM4_USART_Initialize( void )
{
    /* Set FLEXCOM USART operating mode */
    FLEXCOM4_REGS->FLEXCOM_MR = FLEXCOM_MR_OPMODE_USART;

    /* Reset FLEXCOM4 USART */
    USART4_REGS->US_CR = (US_CR_RSTRX_Msk | US_CR_RSTTX_Msk | US_CR_RSTSTA_Msk);

    /* Enable FLEXCOM4 USART */
    USART4_REGS->US_CR = (US_CR_TXEN_Msk | US_CR_RXEN_Msk);

    /* Configure FLEXCOM4 USART mode */
    USART4_REGS->US_MR = ((US_MR_USCLKS_MCK) | US_MR_CHRL_8_BIT | US_MR_PAR_NO | US_MR_NBSTOP_1_BIT | (0 << US_MR_OVER_Pos));

    /* Configure FLEXCOM4 USART Baud Rate */
    USART4_REGS->US_BRGR = US_BRGR_CD(54);

    flexcom4UsartObj.rdCallback = NULL;
    flexcom4UsartObj.rdInIndex = 0;
    flexcom4UsartObj.rdOutIndex = 0;
    flexcom4UsartObj.isRdNotificationEnabled = false;
    flexcom4UsartObj.isRdNotifyPersistently = false;
    flexcom4UsartObj.rdThreshold = 0;
    flexcom4UsartObj.wrCallback = NULL;
    flexcom4UsartObj.wrInIndex = 0;
    flexcom4UsartObj.wrOutIndex = 0;
    flexcom4UsartObj.isWrNotificationEnabled = false;
    flexcom4UsartObj.isWrNotifyPersistently = false;
    flexcom4UsartObj.wrThreshold = 0;
    flexcom4UsartObj.errorStatus = FLEXCOM_USART_ERROR_NONE;

    if (USART4_REGS->US_MR & US_MR_MODE9_Msk)
    {
        flexcom4UsartObj.rdBufferSize = FLEXCOM4_USART_READ_BUFFER_SIZE_9BIT;
        flexcom4UsartObj.wrBufferSize = FLEXCOM4_USART_WRITE_BUFFER_SIZE_9BIT;
    }
    else
    {
        flexcom4UsartObj.rdBufferSize = FLEXCOM4_USART_READ_BUFFER_SIZE;
        flexcom4UsartObj.wrBufferSize = FLEXCOM4_USART_WRITE_BUFFER_SIZE;
    }

    FLEXCOM4_USART_RX_INT_ENABLE();
}

void static FLEXCOM4_USART_ErrorClear( void )
{
    uint8_t dummyData = 0u;

    USART4_REGS->US_CR = US_CR_RSTSTA_Msk;

    /* Flush existing error bytes from the RX FIFO */
    while (USART4_REGS->US_CSR& US_CSR_RXRDY_Msk)
    {
        dummyData = (USART4_REGS->US_RHR & US_RHR_RXCHR_Msk);
    }

    /* Ignore the warning */
    (void)dummyData;
}

FLEXCOM_USART_ERROR FLEXCOM4_USART_ErrorGet( void )
{
    FLEXCOM_USART_ERROR errors = flexcom4UsartObj.errorStatus;

    flexcom4UsartObj.errorStatus = FLEXCOM_USART_ERROR_NONE;

    /* All errors are cleared, but send the previous error state */
    return errors;
}

bool FLEXCOM4_USART_SerialSetup( FLEXCOM_USART_SERIAL_SETUP *setup, uint32_t srcClkFreq )
{
    uint32_t baud = 0;
    uint32_t brgVal = 0;
    uint32_t overSampVal = 0;
    uint32_t usartMode;
    bool status = false;

    if (setup != NULL)
    {
        baud = setup->baudRate;

        if(srcClkFreq == 0)
        {
            srcClkFreq = FLEXCOM4_USART_FrequencyGet();
        }

        /* Calculate BRG value */
        if (srcClkFreq >= (16 * baud))
        {
            brgVal = (srcClkFreq / (16 * baud));
        }
        else if (srcClkFreq >= (8 * baud))
        {
            brgVal = (srcClkFreq / (8 * baud));
            overSampVal = (1 << US_MR_OVER_Pos) & US_MR_OVER_Msk;
        }
        else
        {
            /* The input clock source - srcClkFreq, is too low to generate the desired baud */
            return status;
        }

        if (brgVal > 65535)
        {
            /* The requested baud is so low that the ratio of srcClkFreq to baud exceeds the 16-bit register value of CD register */
            return status;
        }

        /* Configure FLEXCOM4 USART mode */
        usartMode = USART4_REGS->US_MR;
        usartMode &= ~(US_MR_CHRL_Msk | US_MR_MODE9_Msk | US_MR_PAR_Msk | US_MR_NBSTOP_Msk | US_MR_OVER_Msk);
        USART4_REGS->US_MR = usartMode | ((uint32_t)setup->dataWidth | (uint32_t)setup->parity | (uint32_t)setup->stopBits | overSampVal);

        /* Configure FLEXCOM4 USART Baud Rate */
        USART4_REGS->US_BRGR = US_BRGR_CD(brgVal);

        if (USART4_REGS->US_MR & US_MR_MODE9_Msk)
        {
            flexcom4UsartObj.rdBufferSize = FLEXCOM4_USART_READ_BUFFER_SIZE_9BIT;
            flexcom4UsartObj.wrBufferSize = FLEXCOM4_USART_WRITE_BUFFER_SIZE_9BIT;
        }
        else
        {
            flexcom4UsartObj.rdBufferSize = FLEXCOM4_USART_READ_BUFFER_SIZE;
            flexcom4UsartObj.wrBufferSize = FLEXCOM4_USART_WRITE_BUFFER_SIZE;
        }

        status = true;
    }

    return status;
}

/* This routine is only called from ISR. Hence do not disable/enable USART interrupts. */
static bool FLEXCOM4_USART_TxPullByte(uint16_t* pWrByte)
{
    bool isSuccess = false;
    uint32_t wrOutIndex = flexcom4UsartObj.wrOutIndex;
    uint32_t wrInIndex = flexcom4UsartObj.wrInIndex;

    if (wrOutIndex != wrInIndex)
    {
        if (USART4_REGS->US_MR & US_MR_MODE9_Msk)
        {
            *pWrByte = ((uint16_t*)&FLEXCOM4_USART_WriteBuffer)[wrOutIndex++];
        }
        else
        {
            *pWrByte = FLEXCOM4_USART_WriteBuffer[wrOutIndex++];
        }


        if (wrOutIndex >= flexcom4UsartObj.wrBufferSize)
        {
            wrOutIndex = 0;
        }

        flexcom4UsartObj.wrOutIndex = wrOutIndex;

        isSuccess = true;
    }

    return isSuccess;
}

static inline bool FLEXCOM4_USART_TxPushByte(uint16_t wrByte)
{
    uint32_t tempInIndex;
    uint32_t wrOutIndex = flexcom4UsartObj.wrOutIndex;
    uint32_t wrInIndex = flexcom4UsartObj.wrInIndex;

    bool isSuccess = false;

    tempInIndex = wrInIndex + 1;

    if (tempInIndex >= flexcom4UsartObj.wrBufferSize)
    {
        tempInIndex = 0;
    }
    if (tempInIndex != wrOutIndex)
    {
        if (USART4_REGS->US_MR & US_MR_MODE9_Msk)
        {
            ((uint16_t*)&FLEXCOM4_USART_WriteBuffer)[wrInIndex] = wrByte;
        }
        else
        {
            FLEXCOM4_USART_WriteBuffer[wrInIndex] = (uint8_t)wrByte;
        }

        flexcom4UsartObj.wrInIndex = tempInIndex;

        isSuccess = true;
    }
    else
    {
        /* Queue is full. Report Error. */
    }

    return isSuccess;
}

/* This routine is only called from ISR. Hence do not disable/enable USART interrupts. */
static void FLEXCOM4_USART_WriteNotificationSend(void)
{
    uint32_t nFreeWrBufferCount;

    if (flexcom4UsartObj.isWrNotificationEnabled == true)
    {
        nFreeWrBufferCount = FLEXCOM4_USART_WriteFreeBufferCountGet();

        if(flexcom4UsartObj.wrCallback != NULL)
        {
            if (flexcom4UsartObj.isWrNotifyPersistently == true)
            {
                if (nFreeWrBufferCount >= flexcom4UsartObj.wrThreshold)
                {
                    flexcom4UsartObj.wrCallback(FLEXCOM_USART_EVENT_WRITE_THRESHOLD_REACHED, flexcom4UsartObj.wrContext);
                }
            }
            else
            {
                if (nFreeWrBufferCount == flexcom4UsartObj.wrThreshold)
                {
                    flexcom4UsartObj.wrCallback(FLEXCOM_USART_EVENT_WRITE_THRESHOLD_REACHED, flexcom4UsartObj.wrContext);
                }
            }
        }
    }
}

static size_t FLEXCOM4_USART_WritePendingBytesGet(void)
{
    size_t nPendingTxBytes;

    /* Take a snapshot of indices to avoid creation of critical section */
    uint32_t wrOutIndex = flexcom4UsartObj.wrOutIndex;
    uint32_t wrInIndex = flexcom4UsartObj.wrInIndex;

    if ( wrInIndex >=  wrOutIndex)
    {
        nPendingTxBytes =  wrInIndex - wrOutIndex;
    }
    else
    {
        nPendingTxBytes =  (flexcom4UsartObj.wrBufferSize -  wrOutIndex) + wrInIndex;
    }

    return nPendingTxBytes;
}

size_t FLEXCOM4_USART_WriteCountGet(void)
{
    size_t nPendingTxBytes;

    nPendingTxBytes = FLEXCOM4_USART_WritePendingBytesGet();

    return nPendingTxBytes;
}

size_t FLEXCOM4_USART_Write(uint8_t* pWrBuffer, const size_t size )
{
    size_t nBytesWritten  = 0;

    while (nBytesWritten < size)
    {
        if (USART4_REGS->US_MR & US_MR_MODE9_Msk)
        {
            if (FLEXCOM4_USART_TxPushByte(((uint16_t*)pWrBuffer)[nBytesWritten]) == true)
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
            if (FLEXCOM4_USART_TxPushByte(pWrBuffer[nBytesWritten]) == true)
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
    if (FLEXCOM4_USART_WritePendingBytesGet() > 0)
    {
        /* Enable TX interrupt as data is pending for transmission */
        FLEXCOM4_USART_TX_INT_ENABLE();
    }

    return nBytesWritten;
}

size_t FLEXCOM4_USART_WriteFreeBufferCountGet(void)
{
    return (flexcom4UsartObj.wrBufferSize - 1) - FLEXCOM4_USART_WriteCountGet();
}

size_t FLEXCOM4_USART_WriteBufferSizeGet(void)
{
    return (flexcom4UsartObj.wrBufferSize - 1);
}

bool FLEXCOM4_USART_WriteNotificationEnable(bool isEnabled, bool isPersistent)
{
    bool previousStatus = flexcom4UsartObj.isWrNotificationEnabled;

    flexcom4UsartObj.isWrNotificationEnabled = isEnabled;

    flexcom4UsartObj.isWrNotifyPersistently = isPersistent;

    return previousStatus;
}

void FLEXCOM4_USART_WriteThresholdSet(uint32_t nBytesThreshold)
{
    if (nBytesThreshold > 0)
    {
        flexcom4UsartObj.wrThreshold = nBytesThreshold;
    }
}

void FLEXCOM4_USART_WriteCallbackRegister( FLEXCOM_USART_RING_BUFFER_CALLBACK callback, uintptr_t context)
{
    flexcom4UsartObj.wrCallback = callback;

    flexcom4UsartObj.wrContext = context;
}

/* This routine is only called from ISR. Hence do not disable/enable USART interrupts. */
static inline bool FLEXCOM4_USART_RxPushByte(uint16_t rdByte)
{
    uint32_t tempInIndex;
    bool isSuccess = false;

    tempInIndex = flexcom4UsartObj.rdInIndex + 1;

    if (tempInIndex >= flexcom4UsartObj.rdBufferSize)
    {
        tempInIndex = 0;
    }

    if (tempInIndex == flexcom4UsartObj.rdOutIndex)
    {
        /* Queue is full - Report it to the application. Application gets a chance to free up space by reading data out from the RX ring buffer */
        if(flexcom4UsartObj.rdCallback != NULL)
        {
            flexcom4UsartObj.rdCallback(FLEXCOM_USART_EVENT_READ_BUFFER_FULL, flexcom4UsartObj.rdContext);

            /* Read the indices again in case application has freed up space in RX ring buffer */
            tempInIndex = flexcom4UsartObj.rdInIndex + 1;

            if (tempInIndex >= flexcom4UsartObj.rdBufferSize)
            {
                tempInIndex = 0;
            }
        }
    }

    /* Attempt to push the data into the ring buffer */
    if (tempInIndex != flexcom4UsartObj.rdOutIndex)
    {
        if (USART4_REGS->US_MR & US_MR_MODE9_Msk)
        {
            ((uint16_t*)&FLEXCOM4_USART_ReadBuffer)[flexcom4UsartObj.rdInIndex] = rdByte;
        }
        else
        {
            FLEXCOM4_USART_ReadBuffer[flexcom4UsartObj.rdInIndex] = (uint8_t)rdByte;
        }

        flexcom4UsartObj.rdInIndex = tempInIndex;
        isSuccess = true;
    }
    else
    {
        /* Queue is full. Data will be lost. */
    }

    return isSuccess;
}

/* This routine is only called from ISR. Hence do not disable/enable USART interrupts. */
static void FLEXCOM4_USART_ReadNotificationSend(void)
{
    uint32_t nUnreadBytesAvailable;

    if (flexcom4UsartObj.isRdNotificationEnabled == true)
    {
        nUnreadBytesAvailable = FLEXCOM4_USART_ReadCountGet();

        if(flexcom4UsartObj.rdCallback != NULL)
        {
            if (flexcom4UsartObj.isRdNotifyPersistently == true)
            {
                if (nUnreadBytesAvailable >= flexcom4UsartObj.rdThreshold)
                {
                    flexcom4UsartObj.rdCallback(FLEXCOM_USART_EVENT_READ_THRESHOLD_REACHED, flexcom4UsartObj.rdContext);
                }
            }
            else
            {
                if (nUnreadBytesAvailable == flexcom4UsartObj.rdThreshold)
                {
                    flexcom4UsartObj.rdCallback(FLEXCOM_USART_EVENT_READ_THRESHOLD_REACHED, flexcom4UsartObj.rdContext);
                }
            }
        }
    }
}

size_t FLEXCOM4_USART_Read(uint8_t* pRdBuffer, const size_t size)
{
    size_t nBytesRead = 0;
    uint32_t rdOutIndex = flexcom4UsartObj.rdOutIndex;
    uint32_t rdInIndex = flexcom4UsartObj.rdInIndex;

    while (nBytesRead < size)
    {
        if (rdOutIndex != rdInIndex)
        {
            if (USART4_REGS->US_MR & US_MR_MODE9_Msk)
            {
                ((uint16_t*)pRdBuffer)[nBytesRead++] = ((uint16_t*)&FLEXCOM4_USART_ReadBuffer)[rdOutIndex++];
            }
            else
            {
                pRdBuffer[nBytesRead++] = FLEXCOM4_USART_ReadBuffer[rdOutIndex++];
            }


            if (rdOutIndex >= flexcom4UsartObj.rdBufferSize)
            {
                rdOutIndex = 0;
            }
        }
        else
        {
            break;
        }
    }

    flexcom4UsartObj.rdOutIndex = rdOutIndex;

    return nBytesRead;
}

size_t FLEXCOM4_USART_ReadCountGet(void)
{
    size_t nUnreadBytesAvailable;
    uint32_t rdOutIndex;
    uint32_t rdInIndex;

    /* Take a snapshot of indices to avoid creation of critical section */
    rdOutIndex = flexcom4UsartObj.rdOutIndex;
    rdInIndex = flexcom4UsartObj.rdInIndex;

    if ( rdInIndex >= rdOutIndex)
    {
        nUnreadBytesAvailable =  rdInIndex - rdOutIndex;
    }
    else
    {
        nUnreadBytesAvailable =  (flexcom4UsartObj.rdBufferSize -  rdOutIndex) + rdInIndex;
    }

    return nUnreadBytesAvailable;
}

size_t FLEXCOM4_USART_ReadFreeBufferCountGet(void)
{
    return (flexcom4UsartObj.rdBufferSize - 1) - FLEXCOM4_USART_ReadCountGet();
}

size_t FLEXCOM4_USART_ReadBufferSizeGet(void)
{
    return (flexcom4UsartObj.rdBufferSize - 1);
}

bool FLEXCOM4_USART_ReadNotificationEnable(bool isEnabled, bool isPersistent)
{
    bool previousStatus = flexcom4UsartObj.isRdNotificationEnabled;

    flexcom4UsartObj.isRdNotificationEnabled = isEnabled;

    flexcom4UsartObj.isRdNotifyPersistently = isPersistent;

    return previousStatus;
}

void FLEXCOM4_USART_ReadThresholdSet(uint32_t nBytesThreshold)
{
    if (nBytesThreshold > 0)
    {
        flexcom4UsartObj.rdThreshold = nBytesThreshold;
    }
}

void FLEXCOM4_USART_ReadCallbackRegister( FLEXCOM_USART_RING_BUFFER_CALLBACK callback, uintptr_t context)
{
    flexcom4UsartObj.rdCallback = callback;

    flexcom4UsartObj.rdContext = context;
}

void static FLEXCOM4_USART_ISR_RX_Handler( void )
{
    uint16_t rdData = 0;

    /* Keep reading until there is a character availabe in the RX FIFO */
    while(USART4_REGS->US_CSR & US_CSR_RXRDY_Msk)
    {
        rdData = USART4_REGS->US_RHR & US_RHR_RXCHR_Msk;

        if (FLEXCOM4_USART_RxPushByte( rdData ) == true)
        {
            FLEXCOM4_USART_ReadNotificationSend();
        }
        else
        {
            /* UART RX buffer is full */
        }
    }
}

void static FLEXCOM4_USART_ISR_TX_Handler( void )
{
    uint16_t wrByte;

    /* Keep writing to the TX FIFO as long as there is space */
    while (USART4_REGS->US_CSR & US_CSR_TXRDY_Msk)
    {
        if (FLEXCOM4_USART_TxPullByte(&wrByte) == true)
        {
            if (USART4_REGS->US_MR & US_MR_MODE9_Msk)
            {
                USART4_REGS->US_THR = wrByte & US_THR_TXCHR_Msk;
            }
            else
            {
                USART4_REGS->US_THR = (uint8_t)wrByte;
            }

            /* Send notification */
            FLEXCOM4_USART_WriteNotificationSend();
        }
        else
        {
            /* Nothing to transmit. Disable the data register empty interrupt. */
            FLEXCOM4_USART_TX_INT_DISABLE();
            break;
        }
    }
}

void FLEXCOM4_InterruptHandler( void )
{
    /* Channel status */
    uint32_t channelStatus = USART4_REGS->US_CSR;

    /* Error status */
    uint32_t errorStatus = (channelStatus & (US_CSR_OVRE_Msk | US_CSR_FRAME_Msk | US_CSR_PARE_Msk));

    if(errorStatus != 0)
    {
        /* Save the error so that it can be reported when application calls the FLEXCOM4_USART_ErrorGet() API */
        flexcom4UsartObj.errorStatus = (FLEXCOM_USART_ERROR)errorStatus;

        /* Clear the error flags and flush out the error bytes */
        FLEXCOM4_USART_ErrorClear();

        /* USART errors are normally associated with the receiver, hence calling receiver context */
        if( flexcom4UsartObj.rdCallback != NULL )
        {
            flexcom4UsartObj.rdCallback(FLEXCOM_USART_EVENT_READ_ERROR, flexcom4UsartObj.rdContext);
        }
    }

    /* Receiver status */
    if (channelStatus & US_CSR_RXRDY_Msk)
    {
        FLEXCOM4_USART_ISR_RX_Handler();
    }

    /* Transmitter status */
    if ( (channelStatus & US_CSR_TXRDY_Msk) && (USART4_REGS->US_IMR & US_IMR_TXRDY_Msk) )
    {
        FLEXCOM4_USART_ISR_TX_Handler();
    }
}
