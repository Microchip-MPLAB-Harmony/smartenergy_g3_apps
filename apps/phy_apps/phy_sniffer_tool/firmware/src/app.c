/*
Copyright (C) 2022, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 ******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <string.h>
#include "definitions.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

CACHE_ALIGN APP_DATA appData;

static CACHE_ALIGN uint8_t pPLCDataPIBBuffer[CACHE_ALIGNED_SIZE_GET(APP_PLC_PIB_BUFFER_SIZE)];
static CACHE_ALIGN uint8_t pSerialDataBuffer[CACHE_ALIGNED_SIZE_GET(APP_SERIAL_DATA_BUFFER_SIZE)];

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

static void APP_Timer1_Callback (uintptr_t context)
{
    appData.tmr1Expired = true;
}

static void APP_Timer2_Callback (uintptr_t context)
{
    appData.tmr2Expired = true;
}

static void APP_PLCDataIndCb(DRV_PLC_PHY_RECEPTION_OBJ *indObj, uintptr_t context)
{
    /* Avoid warning */
    (void)context;

    /* Send Received PLC message through USI */
	if (indObj->dataLength) {
        size_t length;

        /* Turn on indication LED and start timer to turn it off */
        SYS_TIME_TimerDestroy(appData.tmr2Handle);
        USER_PLC_IND_LED_On();
        appData.tmr2Handle = SYS_TIME_CallbackRegisterMS(APP_Timer2_Callback, 0,
                LED_BLINK_PLC_MSG_MS, SYS_TIME_SINGLE);

        /* Report RX Symbols */
        appData.plcPIB.id = PLC_ID_RX_PAY_SYMBOLS;
        appData.plcPIB.length = 2;
        DRV_PLC_PHY_PIBGet(appData.drvPlcHandle, &appData.plcPIB);

        SRV_PSNIFFER_SetRxPayloadSymbols(*(uint16_t *)appData.plcPIB.pData);

        /* Add received message */
        length = SRV_PSNIFFER_SerialRxMessage(appData.pSerialData, indObj);
        /* Send through USI */
        SRV_USI_Send_Message(appData.srvUSIHandle, SRV_USI_PROT_ID_SNIFF_G3,
                appData.pSerialData, length);
    }
}

void APP_USIPhyProtocolEventHandler(uint8_t *pData, size_t length)
{
    SRV_PSNIFFER_COMMAND command;

    /* Protection for invalid length */
    if (!length)
    {
        return;
    }

    /* Process received command */
    command = SRV_PSNIFFER_GetCommand(pData);

    switch (command)
    {
        case SRV_PSNIFFER_CMD_SET_PLC_TONE_MASK:
        {
            /* Convert ToneMask from Sniffer Tool to PLC PHY layer */
            SRV_PSNIFFER_ConvertToneMask(pData, appData.plcPIB.pData);

            /* Send data to PLC device */
            appData.plcPIB.id = PLC_ID_TONE_MASK;
            appData.plcPIB.length = PSNIFFER_CARRIERS_SIZE;
            DRV_PLC_PHY_PIBSet(appData.drvPlcHandle, &appData.plcPIB);
            break;
        }

        default:
            break;
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize(void)

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize(void)
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_IDLE;

    /* Init Timer handler */
    appData.tmr1Handle = SYS_TIME_HANDLE_INVALID;
    appData.tmr2Handle = SYS_TIME_HANDLE_INVALID;
    appData.tmr1Expired = false;
    appData.tmr2Expired = false;

    appData.state = APP_STATE_INIT;

    /* Initialize PLC objects */
    appData.plcPIB.pData = pPLCDataPIBBuffer;
    appData.pSerialData = pSerialDataBuffer;
}


/******************************************************************************
  Function:
    void APP_Tasks(void)

  Remarks:
    See prototype in app.h.
 */
void APP_Tasks(void)
{
    /* Update Watchdog */
    CLEAR_WATCHDOG();

    /* Signalling */
    if (appData.tmr1Expired)
    {
        appData.tmr1Expired = false;
        USER_BLINK_LED_Toggle();
    }

    if (appData.tmr2Expired)
    {
        appData.tmr2Expired = false;
        USER_PLC_IND_LED_Off();
    }

    /* Check the application's current state. */
    switch(appData.state)
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
            /* Open PLC driver : Start uploading process */
            appData.drvPlcHandle = DRV_PLC_PHY_Open(DRV_PLC_PHY_INDEX, NULL);

            if (appData.drvPlcHandle != DRV_HANDLE_INVALID)
            {
                /* Set Application to next state */
                appData.state = APP_STATE_REGISTER;
            }
            else
            {
                /* Set Application to ERROR state */
                appData.state = APP_STATE_ERROR;
            }
            break;
        }

        /* Waiting to PLC transceiver be opened and register callback functions */
        case APP_STATE_REGISTER:
        {
            /* Check PLC transceiver */
            if (DRV_PLC_PHY_Status(DRV_PLC_PHY_INDEX) == SYS_STATUS_READY)
            {
                /* Register PLC callback */
                DRV_PLC_PHY_DataIndCallbackRegister(appData.drvPlcHandle,
                        APP_PLCDataIndCb, DRV_PLC_PHY_INDEX);

                /* Open USI Service */
                appData.srvUSIHandle = SRV_USI_Open(SRV_USI_INDEX_0);

                if (appData.srvUSIHandle != DRV_HANDLE_INVALID)
                {
                    /* Set Application to next state */
                    appData.state = APP_STATE_CONFIG_USI;
                }
                else
                {
                    /* Set Application to ERROR state */
                    appData.state = APP_STATE_ERROR;
                }
            }
            break;
        }

        case APP_STATE_CONFIG_USI:
        {
            if (SRV_USI_Status(appData.srvUSIHandle) == SRV_USI_STATUS_CONFIGURED)
            {
                /* Register USI callback */
                SRV_USI_CallbackRegister(appData.srvUSIHandle,
                        SRV_USI_PROT_ID_SNIFF_G3, APP_USIPhyProtocolEventHandler);

                if (appData.tmr1Handle == SYS_TIME_HANDLE_INVALID)
                {
                    /* Register Timer Callback */
                    appData.tmr1Handle = SYS_TIME_CallbackRegisterMS(
                            APP_Timer1_Callback, 0, LED_BLINK_RATE_MS,
                            SYS_TIME_PERIODIC);
                }
                else
                {
                    SYS_TIME_TimerStart(appData.tmr1Handle);
                }

                /* Set Application to next state */
                appData.state = APP_STATE_READY;
            }
            break;
        }

        case APP_STATE_READY:
        {
            /* Check USI status in case of USI device has been reset */
            if (SRV_USI_Status(appData.srvUSIHandle) == SRV_USI_STATUS_NOT_CONFIGURED)
            {
                /* Set Application to next state */
                appData.state = APP_STATE_CONFIG_USI;
                SYS_TIME_TimerStop(appData.tmr1Handle);
                /* Disable Blink Led */
                USER_BLINK_LED_Off();
            }
            break;
        }

        case APP_STATE_ERROR:
        {
            /* Handle error in application's state machine */
            break;
        }

        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}


/*******************************************************************************
 End of File
 */
