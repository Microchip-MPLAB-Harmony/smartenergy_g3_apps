/*******************************************************************************
  FLEXCOM7 USART PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_flexcom7_usart.c

  Summary:
    FLEXCOM7 USART PLIB Implementation File

  Description
    This file defines the interface to the FLEXCOM7 USART
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

#include "plib_flexcom7_usart.h"
#include "interrupts.h"

#define FLEXCOM7_USART_READ_BUFFER_SIZE             512
#define FLEXCOM7_USART_READ_BUFFER_SIZE_9BIT        (512 >> 1)

/* Disable Read, Overrun, Parity and Framing error interrupts */
#define FLEXCOM7_USART_RX_INT_DISABLE()      USART7_REGS->US_IDR = (US_IDR_RXRDY_Msk | US_IDR_FRAME_Msk | US_IDR_PARE_Msk | US_IDR_OVRE_Msk)
/* Enable Read, Overrun, Parity and Framing error interrupts */
#define FLEXCOM7_USART_RX_INT_ENABLE()       USART7_REGS->US_IER = (US_IER_RXRDY_Msk | US_IER_FRAME_Msk | US_IER_PARE_Msk | US_IER_OVRE_Msk)

static uint8_t FLEXCOM7_USART_ReadBuffer[FLEXCOM7_USART_READ_BUFFER_SIZE];

#define FLEXCOM7_USART_WRITE_BUFFER_SIZE            512
#define FLEXCOM7_USART_WRITE_BUFFER_SIZE_9BIT       (512 >> 1)

#define FLEXCOM7_USART_TX_INT_DISABLE()      USART7_REGS->US_IDR = US_IDR_TXRDY_Msk
#define FLEXCOM7_USART_TX_INT_ENABLE()       USART7_REGS->US_IER = US_IER_TXRDY_Msk

static uint8_t FLEXCOM7_USART_WriteBuffer[FLEXCOM7_USART_WRITE_BUFFER_SIZE];

// *****************************************************************************
// *****************************************************************************
// Section: FLEXCOM7 USART Ring Buffer Implementation
// *****************************************************************************
// *****************************************************************************

FLEXCOM_USART_RING_BUFFER_OBJECT flexcom7UsartObj;

void FLEXCOM7_USART_Initialize( void )
{
    /* Set FLEXCOM USART operating mode */
    FLEXCOM7_REGS->FLEXCOM_MR = FLEXCOM_MR_OPMODE_USART;

    /* Reset FLEXCOM7 USART */
    USART7_REGS->US_CR = (US_CR_RSTRX_Msk | US_CR_RSTTX_Msk | US_CR_RSTSTA_Msk);

    /* Enable FLEXCOM7 USART */
    USART7_REGS->US_CR = (US_CR_TXEN_Msk | US_CR_RXEN_Msk);

    /* Configure FLEXCOM7 USART mode */
    USART7_REGS->US_MR = ((US_MR_USCLKS_MCK) | US_MR_CHRL_8_BIT | US_MR_PAR_NO | US_MR_NBSTOP_1_BIT | (0 << US_MR_OVER_Pos));

    /* Configure FLEXCOM7 USART Baud Rate */
    USART7_REGS->US_BRGR = US_BRGR_CD(54);

    flexcom7UsartObj.rdCallback = NULL;
    flexcom7UsartObj.rdInIndex = 0;
    flexcom7UsartObj.rdOutIndex = 0;
    flexcom7UsartObj.isRdNotificationEnabled = false;
    flexcom7UsartObj.isRdNotifyPersistently = false;
    flexcom7UsartObj.rdThreshold = 0;
    flexcom7UsartObj.wrCallback = NULL;
    flexcom7UsartObj.wrInIndex = 0;
    flexcom7UsartObj.wrOutIndex = 0;
    flexcom7UsartObj.isWrNotificationEnabled = false;
    flexcom7UsartObj.isWrNotifyPersistently = false;
    flexcom7UsartObj.wrThreshold = 0;
    flexcom7UsartObj.errorStatus = FLEXCOM_USART_ERROR_NONE;

    if (USART7_REGS->US_MR & US_MR_MODE9_Msk)
    {
        flexcom7UsartObj.rdBufferSize = FLEXCOM7_USART_READ_BUFFER_SIZE_9BIT;
        flexcom7UsartObj.wrBufferSize = FLEXCOM7_USART_WRITE_BUFFER_SIZE_9BIT;
    }
    else
    {
        flexcom7UsartObj.rdBufferSize = FLEXCOM7_USART_READ_BUFFER_SIZE;
        flexcom7UsartObj.wrBufferSize = FLEXCOM7_USART_WRITE_BUFFER_SIZE;
    }

    FLEXCOM7_USART_RX_INT_ENABLE();
}

void static FLEXCOM7_USART_ErrorClear( void )
{
    uint8_t dummyData = 0u;

    USART7_REGS->US_CR = US_CR_RSTSTA_Msk;

    /* Flush existing error bytes from the RX FIFO */
    while (USART7_REGS->US_CSR& US_CSR_RXRDY_Msk)
    {
        dummyData = (USART7_REGS->US_RHR & US_RHR_RXCHR_Msk);
    }

    /* Ignore the warning */
    (void)dummyData;
}

FLEXCOM_USART_ERROR FLEXCOM7_USART_ErrorGet( void )
{
    FLEXCOM_USART_ERROR errors = flexcom7UsartObj.errorStatus;

    flexcom7UsartObj.errorStatus = FLEXCOM_USART_ERROR_NONE;

    /* All errors are cleared, but send the previous error state */
    return errors;
}

bool FLEXCOM7_USART_SerialSetup( FLEXCOM_USART_SERIAL_SETUP *setup, uint32_t srcClkFreq )
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
            srcClkFreq = FLEXCOM7_USART_FrequencyGet();
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

        /* Configure FLEXCOM7 USART mode */
        usartMode = USART7_REGS->US_MR;
        usartMode &= ~(US_MR_CHRL_Msk | US_MR_MODE9_Msk | US_MR_PAR_Msk | US_MR_NBSTOP_Msk | US_MR_OVER_Msk);
        USART7_REGS->US_MR = usartMode | ((uint32_t)setup->dataWidth | (uint32_t)setup->parity | (uint32_t)setup->stopBits | overSampVal);

        /* Configure FLEXCOM7 USART Baud Rate */
        USART7_REGS->US_BRGR = US_BRGR_CD(brgVal);

        if (USART7_REGS->US_MR & US_MR_MODE9_Msk)
        {
            flexcom7UsartObj.rdBufferSize = FLEXCOM7_USART_READ_BUFFER_SIZE_9BIT;
            flexcom7UsartObj.wrBufferSize = FLEXCOM7_USART_WRITE_BUFFER_SIZE_9BIT;
        }
        else
        {
            flexcom7UsartObj.rdBufferSize = FLEXCOM7_USART_READ_BUFFER_SIZE;
            flexcom7UsartObj.wrBufferSize = FLEXCOM7_USART_WRITE_BUFFER_SIZE;
        }

        status = true;
    }

    return status;
}

/* This routine is only called from ISR. Hence do not disable/enable USART interrupts. */
static bool FLEXCOM7_USART_TxPullByte(uint16_t* pWrByte)
{
    bool isSuccess = false;
    uint32_t wrOutIndex = flexcom7UsartObj.wrOutIndex;
    uint32_t wrInIndex = flexcom7UsartObj.wrInIndex;

    if (wrOutIndex != wrInIndex)
    {
        if (USART7_REGS->US_MR & US_MR_MODE9_Msk)
        {
            *pWrByte = ((uint16_t*)&FLEXCOM7_USART_WriteBuffer)[wrOutIndex++];
        }
        else
        {
            *pWrByte = FLEXCOM7_USART_WriteBuffer[wrOutIndex++];
        }


        if (wrOutIndex >= flexcom7UsartObj.wrBufferSize)
        {
            wrOutIndex = 0;
        }

        flexcom7UsartObj.wrOutIndex = wrOutIndex;

        isSuccess = true;
    }

    return isSuccess;
}

static inline bool FLEXCOM7_USART_TxPushByte(uint16_t wrByte)
{
    uint32_t tempInIndex;
    uint32_t wrOutIndex = flexcom7UsartObj.wrOutIndex;
    uint32_t wrInIndex = flexcom7UsartObj.wrInIndex;

    bool isSuccess = false;

    tempInIndex = wrInIndex + 1;

    if (tempInIndex >= flexcom7UsartObj.wrBufferSize)
    {
        tempInIndex = 0;
    }
    if (tempInIndex != wrOutIndex)
    {
        if (USART7_REGS->US_MR & US_MR_MODE9_Msk)
        {
            ((uint16_t*)&FLEXCOM7_USART_WriteBuffer)[wrInIndex] = wrByte;
        }
        else
        {
            FLEXCOM7_USART_WriteBuffer[wrInIndex] = (uint8_t)wrByte;
        }

        flexcom7UsartObj.wrInIndex = tempInIndex;

        isSuccess = true;
    }
    else
    {
        /* Queue is full. Report Error. */
    }

    return isSuccess;
}

/* This routine is only called from ISR. Hence do not disable/enable USART interrupts. */
static void FLEXCOM7_USART_WriteNotificationSend(void)
{
    uint32_t nFreeWrBufferCount;

    if (flexcom7UsartObj.isWrNotificationEnabled == true)
    {
        nFreeWrBufferCount = FLEXCOM7_USART_WriteFreeBufferCountGet();

        if(flexcom7UsartObj.wrCallback != NULL)
        {
            if (flexcom7UsartObj.isWrNotifyPersistently == true)
            {
                if (nFreeWrBufferCount >= flexcom7UsartObj.wrThreshold)
                {
                    flexcom7UsartObj.wrCallback(FLEXCOM_USART_EVENT_WRITE_THRESHOLD_REACHED, flexcom7UsartObj.wrContext);
                }
            }
            else
            {
                if (nFreeWrBufferCount == flexcom7UsartObj.wrThreshold)
                {
                    flexcom7UsartObj.wrCallback(FLEXCOM_USART_EVENT_WRITE_THRESHOLD_REACHED, flexcom7UsartObj.wrContext);
                }
            }
        }
    }
}

static size_t FLEXCOM7_USART_WritePendingBytesGet(void)
{
    size_t nPendingTxBytes;

    /* Take a snapshot of indices to avoid creation of critical section */
    uint32_t wrOutIndex = flexcom7UsartObj.wrOutIndex;
    uint32_t wrInIndex = flexcom7UsartObj.wrInIndex;

    if ( wrInIndex >=  wrOutIndex)
    {
        nPendingTxBytes =  wrInIndex - wrOutIndex;
    }
    else
    {
        nPendingTxBytes =  (flexcom7UsartObj.wrBufferSize -  wrOutIndex) + wrInIndex;
    }

    return nPendingTxBytes;
}

size_t FLEXCOM7_USART_WriteCountGet(void)
{
    size_t nPendingTxBytes;

    nPendingTxBytes = FLEXCOM7_USART_WritePendingBytesGet();

    return nPendingTxBytes;
}

size_t FLEXCOM7_USART_Write(uint8_t* pWrBuffer, const size_t size )
{
    size_t nBytesWritten  = 0;

    while (nBytesWritten < size)
    {
        if (USART7_REGS->US_MR & US_MR_MODE9_Msk)
        {
            if (FLEXCOM7_USART_TxPushByte(((uint16_t*)pWrBuffer)[nBytesWritten]) == true)
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
            if (FLEXCOM7_USART_TxPushByte(pWrBuffer[nBytesWritten]) == true)
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
    if (FLEXCOM7_USART_WritePendingBytesGet() > 0)
    {
        /* Enable TX interrupt as data is pending for transmission */
        FLEXCOM7_USART_TX_INT_ENABLE();
    }

    return nBytesWritten;
}

size_t FLEXCOM7_USART_WriteFreeBufferCountGet(void)
{
    return (flexcom7UsartObj.wrBufferSize - 1) - FLEXCOM7_USART_WriteCountGet();
}

size_t FLEXCOM7_USART_WriteBufferSizeGet(void)
{
    return (flexcom7UsartObj.wrBufferSize - 1);
}

bool FLEXCOM7_USART_WriteNotificationEnable(bool isEnabled, bool isPersistent)
{
    bool previousStatus = flexcom7UsartObj.isWrNotificationEnabled;

    flexcom7UsartObj.isWrNotificationEnabled = isEnabled;

    flexcom7UsartObj.isWrNotifyPersistently = isPersistent;

    return previousStatus;
}

void FLEXCOM7_USART_WriteThresholdSet(uint32_t nBytesThreshold)
{
    if (nBytesThreshold > 0)
    {
        flexcom7UsartObj.wrThreshold = nBytesThreshold;
    }
}

void FLEXCOM7_USART_WriteCallbackRegister( FLEXCOM_USART_RING_BUFFER_CALLBACK callback, uintptr_t context)
{
    flexcom7UsartObj.wrCallback = callback;

    flexcom7UsartObj.wrContext = context;
}

/* This routine is only called from ISR. Hence do not disable/enable USART interrupts. */
static inline bool FLEXCOM7_USART_RxPushByte(uint16_t rdByte)
{
    uint32_t tempInIndex;
    bool isSuccess = false;

    tempInIndex = flexcom7UsartObj.rdInIndex + 1;

    if (tempInIndex >= flexcom7UsartObj.rdBufferSize)
    {
        tempInIndex = 0;
    }

    if (tempInIndex == flexcom7UsartObj.rdOutIndex)
    {
        /* Queue is full - Report it to the application. Application gets a chance to free up space by reading data out from the RX ring buffer */
        if(flexcom7UsartObj.rdCallback != NULL)
        {
            flexcom7UsartObj.rdCallback(FLEXCOM_USART_EVENT_READ_BUFFER_FULL, flexcom7UsartObj.rdContext);

            /* Read the indices again in case application has freed up space in RX ring buffer */
            tempInIndex = flexcom7UsartObj.rdInIndex + 1;

            if (tempInIndex >= flexcom7UsartObj.rdBufferSize)
            {
                tempInIndex = 0;
            }
        }
    }

    /* Attempt to push the data into the ring buffer */
    if (tempInIndex != flexcom7UsartObj.rdOutIndex)
    {
        if (USART7_REGS->US_MR & US_MR_MODE9_Msk)
        {
            ((uint16_t*)&FLEXCOM7_USART_ReadBuffer)[flexcom7UsartObj.rdInIndex] = rdByte;
        }
        else
        {
            FLEXCOM7_USART_ReadBuffer[flexcom7UsartObj.rdInIndex] = (uint8_t)rdByte;
        }

        flexcom7UsartObj.rdInIndex = tempInIndex;
        isSuccess = true;
    }
    else
    {
        /* Queue is full. Data will be lost. */
    }

    return isSuccess;
}

/* This routine is only called from ISR. Hence do not disable/enable USART interrupts. */
static void FLEXCOM7_USART_ReadNotificationSend(void)
{
    uint32_t nUnreadBytesAvailable;

    if (flexcom7UsartObj.isRdNotificationEnabled == true)
    {
        nUnreadBytesAvailable = FLEXCOM7_USART_ReadCountGet();

        if(flexcom7UsartObj.rdCallback != NULL)
        {
            if (flexcom7UsartObj.isRdNotifyPersistently == true)
            {
                if (nUnreadBytesAvailable >= flexcom7UsartObj.rdThreshold)
                {
                    flexcom7UsartObj.rdCallback(FLEXCOM_USART_EVENT_READ_THRESHOLD_REACHED, flexcom7UsartObj.rdContext);
                }
            }
            else
            {
                if (nUnreadBytesAvailable == flexcom7UsartObj.rdThreshold)
                {
                    flexcom7UsartObj.rdCallback(FLEXCOM_USART_EVENT_READ_THRESHOLD_REACHED, flexcom7UsartObj.rdContext);
                }
            }
        }
    }
}

size_t FLEXCOM7_USART_Read(uint8_t* pRdBuffer, const size_t size)
{
    size_t nBytesRead = 0;
    uint32_t rdOutIndex = flexcom7UsartObj.rdOutIndex;
    uint32_t rdInIndex = flexcom7UsartObj.rdInIndex;

    while (nBytesRead < size)
    {
        if (rdOutIndex != rdInIndex)
        {
            if (USART7_REGS->US_MR & US_MR_MODE9_Msk)
            {
                ((uint16_t*)pRdBuffer)[nBytesRead++] = ((uint16_t*)&FLEXCOM7_USART_ReadBuffer)[rdOutIndex++];
            }
            else
            {
                pRdBuffer[nBytesRead++] = FLEXCOM7_USART_ReadBuffer[rdOutIndex++];
            }


            if (rdOutIndex >= flexcom7UsartObj.rdBufferSize)
            {
                rdOutIndex = 0;
            }
        }
        else
        {
            break;
        }
    }

    flexcom7UsartObj.rdOutIndex = rdOutIndex;

    return nBytesRead;
}

size_t FLEXCOM7_USART_ReadCountGet(void)
{
    size_t nUnreadBytesAvailable;
    uint32_t rdOutIndex;
    uint32_t rdInIndex;

    /* Take a snapshot of indices to avoid creation of critical section */
    rdOutIndex = flexcom7UsartObj.rdOutIndex;
    rdInIndex = flexcom7UsartObj.rdInIndex;

    if ( rdInIndex >= rdOutIndex)
    {
        nUnreadBytesAvailable =  rdInIndex - rdOutIndex;
    }
    else
    {
        nUnreadBytesAvailable =  (flexcom7UsartObj.rdBufferSize -  rdOutIndex) + rdInIndex;
    }

    return nUnreadBytesAvailable;
}

size_t FLEXCOM7_USART_ReadFreeBufferCountGet(void)
{
    return (flexcom7UsartObj.rdBufferSize - 1) - FLEXCOM7_USART_ReadCountGet();
}

size_t FLEXCOM7_USART_ReadBufferSizeGet(void)
{
    return (flexcom7UsartObj.rdBufferSize - 1);
}

bool FLEXCOM7_USART_ReadNotificationEnable(bool isEnabled, bool isPersistent)
{
    bool previousStatus = flexcom7UsartObj.isRdNotificationEnabled;

    flexcom7UsartObj.isRdNotificationEnabled = isEnabled;

    flexcom7UsartObj.isRdNotifyPersistently = isPersistent;

    return previousStatus;
}

void FLEXCOM7_USART_ReadThresholdSet(uint32_t nBytesThreshold)
{
    if (nBytesThreshold > 0)
    {
        flexcom7UsartObj.rdThreshold = nBytesThreshold;
    }
}

void FLEXCOM7_USART_ReadCallbackRegister( FLEXCOM_USART_RING_BUFFER_CALLBACK callback, uintptr_t context)
{
    flexcom7UsartObj.rdCallback = callback;

    flexcom7UsartObj.rdContext = context;
}

void static FLEXCOM7_USART_ISR_RX_Handler( void )
{
    uint16_t rdData = 0;

    /* Keep reading until there is a character availabe in the RX FIFO */
    while(USART7_REGS->US_CSR & US_CSR_RXRDY_Msk)
    {
        rdData = USART7_REGS->US_RHR & US_RHR_RXCHR_Msk;

        if (FLEXCOM7_USART_RxPushByte( rdData ) == true)
        {
            FLEXCOM7_USART_ReadNotificationSend();
        }
        else
        {
            /* UART RX buffer is full */
        }
    }
}

void static FLEXCOM7_USART_ISR_TX_Handler( void )
{
    uint16_t wrByte;

    /* Keep writing to the TX FIFO as long as there is space */
    while (USART7_REGS->US_CSR & US_CSR_TXRDY_Msk)
    {
        if (FLEXCOM7_USART_TxPullByte(&wrByte) == true)
        {
            if (USART7_REGS->US_MR & US_MR_MODE9_Msk)
            {
                USART7_REGS->US_THR = wrByte & US_THR_TXCHR_Msk;
            }
            else
            {
                USART7_REGS->US_THR = (uint8_t)wrByte;
            }

            /* Send notification */
            FLEXCOM7_USART_WriteNotificationSend();
        }
        else
        {
            /* Nothing to transmit. Disable the data register empty interrupt. */
            FLEXCOM7_USART_TX_INT_DISABLE();
            break;
        }
    }
}

void FLEXCOM7_InterruptHandler( void )
{
    /* Channel status */
    uint32_t channelStatus = USART7_REGS->US_CSR;

    /* Error status */
    uint32_t errorStatus = (channelStatus & (US_CSR_OVRE_Msk | US_CSR_FRAME_Msk | US_CSR_PARE_Msk));

    if(errorStatus != 0)
    {
        /* Save the error so that it can be reported when application calls the FLEXCOM7_USART_ErrorGet() API */
        flexcom7UsartObj.errorStatus = (FLEXCOM_USART_ERROR)errorStatus;

        /* Clear the error flags and flush out the error bytes */
        FLEXCOM7_USART_ErrorClear();

        /* USART errors are normally associated with the receiver, hence calling receiver context */
        if( flexcom7UsartObj.rdCallback != NULL )
        {
            flexcom7UsartObj.rdCallback(FLEXCOM_USART_EVENT_READ_ERROR, flexcom7UsartObj.rdContext);
        }
    }

    /* Receiver status */
    if (channelStatus & US_CSR_RXRDY_Msk)
    {
        FLEXCOM7_USART_ISR_RX_Handler();
    }

    /* Transmitter status */
    if ( (channelStatus & US_CSR_TXRDY_Msk) && (USART7_REGS->US_IMR & US_IMR_TXRDY_Msk) )
    {
        FLEXCOM7_USART_ISR_TX_Handler();
    }
}
