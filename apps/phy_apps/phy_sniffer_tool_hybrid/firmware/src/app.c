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
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2022 Microchip Technology Inc. and its subsidiaries.
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
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "app.h"

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

APP_DATA appData;
    
static uint8_t plcDataPibBuffer[APP_PLC_PIB_BUFFER_SIZE];
static uint8_t plcSnifferDataBuffer[APP_PLC_SNIFFER_BUFFER_SIZE];

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

static void _APP_TimeExpired(uintptr_t context)
{
    *((bool *) context) = true;
}

static void _APP_PlcDataIndCb(DRV_PLC_PHY_RECEPTION_OBJ *indObj, uintptr_t ctxt)
{
    size_t length;
    
    /* Avoid warning */
    (void) ctxt;

    /* Start Timer: LED blinking for each received message */
    USER_PLC_IND_LED_On();
    SYS_TIME_TimerDestroy(appData.tmr2Handle);
    appData.tmr2Expired = false;
    appData.tmr2Handle = SYS_TIME_CallbackRegisterMS(_APP_TimeExpired,
            (uintptr_t) &appData.tmr2Expired, LED_BLINK_PLC_MSG_MS, SYS_TIME_SINGLE);

    /* Report RX Symbols */
    appData.plcPIB.id = PLC_ID_RX_PAY_SYMBOLS;
    appData.plcPIB.length = 2;
    DRV_PLC_PHY_PIBGet(appData.drvPlcHandle, &appData.plcPIB);
    SRV_PSNIFFER_SetRxPayloadSymbols(*(uint16_t *)appData.plcPIB.pData);

    /* Serialize received PLC message */
    length = SRV_PSNIFFER_SerialRxMessage(plcSnifferDataBuffer, indObj);
    
    /* Send through USI */
    SRV_USI_Send_Message(appData.srvUSIHandle, SRV_USI_PROT_ID_SNIFF_G3,
            plcSnifferDataBuffer, length);
}

static void _APP_RfRxIndCb(DRV_RF215_RX_INDICATION_OBJ* indObj, uintptr_t ctxt)
{
    DRV_RF215_PHY_CFG_OBJ rfPhyCfg;
    uint8_t* pRfSnifferData;
    size_t rfSnifferDataSize;
    uint16_t rfPayloadSymbols;
    
    /* Avoid warning */
    (void) ctxt;
    
    /* Get payload symbols in the received message */
    DRV_RF215_GetPib(appData.drvRf215Handle, RF215_PIB_PHY_RX_PAY_SYMBOLS, &rfPayloadSymbols);
    
    /* Get PHY configuration */
    DRV_RF215_GetPib(appData.drvRf215Handle, RF215_PIB_PHY_CONFIG, &rfPhyCfg);
    
    /* Serialize received RF message */
    pRfSnifferData = SRV_RSNIFFER_SerialRxMessage(indObj, &rfPhyCfg,
            rfPayloadSymbols, &rfSnifferDataSize);
    
    /* Send through USI */
    SRV_USI_Send_Message(appData.srvUSIHandle, SRV_USI_PROT_ID_SNIFF_G3,
            pRfSnifferData, rfSnifferDataSize);
}

void APP_USIPhyProtocolEventHandler(uint8_t *pData, size_t length)
{
    /* Message received from PLC Tool - USART */
	uint8_t command;

	/* Protection for invalid us_length */
	if (!length)
    {
		return;
	}

	/* Process received message */
	command = SRV_PSNIFFER_GetCommand(pData);

	switch (command) {
        case SRV_PSNIFFER_CMD_SET_TONE_MASK:
        {
            /* Convert ToneMask from Sniffer Tool to PLC phy layer */
            SRV_PSNIFFER_ConvertToneMask(appData.plcPIB.pData, pData + 1);

            /* Send data to PLC */
            appData.plcPIB.id = PLC_ID_TONE_MASK;
            appData.plcPIB.length = PSNIFFER_CARRIERS_SIZE;
            DRV_PLC_PHY_PIBSet(appData.drvPlcHandle, &appData.plcPIB);

        }
        break;

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
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;

    /* Initialize Timer handles */
    appData.tmr1Handle = SYS_TIME_HANDLE_INVALID;
    appData.tmr2Handle = SYS_TIME_HANDLE_INVALID;
    appData.tmr1Expired = false;
    appData.tmr2Expired = false;
    
    /* Initialize driver handles */
    appData.drvRf215Handle = DRV_HANDLE_INVALID;
    appData.drvPlcHandle = DRV_HANDLE_INVALID;
    appData.srvUSIHandle = DRV_HANDLE_INVALID;

    /* Initialize PLC objects */
    appData.plcPIB.pData = plcDataPibBuffer;
}

/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{
    /* Update Watchdog */
    WDT_Clear();
    
    /* LED signaling */
    if (appData.tmr1Expired == true)
    {
        appData.tmr1Expired = false;
        USER_BLINK_LED_Toggle();
    }
    
    if (appData.tmr2Expired == true)
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
            /* Open PLC driver: Start uploading process */
            appData.drvPlcHandle = DRV_PLC_PHY_Open(DRV_PLC_PHY_INDEX, NULL);

            if (appData.drvPlcHandle != DRV_HANDLE_INVALID)
            {
                /* Set Application to next state */
                appData.state = APP_STATE_REGISTER_PLC;
            }
            else
            {
                /* PLC driver unavailable: Try to open RF215 driver */
                appData.state = APP_STATE_REGISTER_RF;
            }
            break;
        }

        /* Waiting to PLC driver be opened and register callback functions */
        case APP_STATE_REGISTER_PLC:
        {
            /* Check PLC transceiver */
            SYS_STATUS plcStatus = DRV_PLC_PHY_Status(DRV_PLC_PHY_INDEX);
            if (plcStatus == SYS_STATUS_READY)
            {
                /* Register PLC driver callback */
                DRV_PLC_PHY_DataIndCallbackRegister(appData.drvPlcHandle,
                        _APP_PlcDataIndCb, DRV_PLC_PHY_INDEX);

                /* PLC driver opened successfully. Try to open RF215 driver. */
                appData.state = APP_STATE_REGISTER_RF;                
            }
            else if (plcStatus == SYS_STATUS_ERROR)
            {
                /* PLC driver unavailable. Try to open RF215 driver. */
                appData.drvPlcHandle = DRV_HANDLE_INVALID;
                appData.state = APP_STATE_REGISTER_RF;
            }
            
            break;
        }
        
        /* Waiting to RF215 driver be opened and register callback functions */
        case APP_STATE_REGISTER_RF:
        {
            /* Check status of RF215 driver */
            SYS_STATUS rf215Status = DRV_RF215_Status(sysObj.drvRf215);
            if (rf215Status == SYS_STATUS_READY)
            {
                /* RF215 driver is ready to be opened */
                appData.drvRf215Handle = DRV_RF215_Open(DRV_RF215_INDEX_0, RF215_TRX_ID_RF09);
                if (appData.drvRf215Handle != DRV_HANDLE_INVALID)
                {
                    /* Register RF215 driver callback */
                    DRV_RF215_RxIndCallbackRegister(appData.drvRf215Handle, _APP_RfRxIndCb, 0);
                }
            }
            
            if ((rf215Status == SYS_STATUS_READY) || (rf215Status == SYS_STATUS_ERROR))
            {
                if ((appData.drvPlcHandle == DRV_HANDLE_INVALID) && (appData.drvRf215Handle == DRV_HANDLE_INVALID))
                {
                    /* Set Application to ERROR state */
                    appData.state = APP_STATE_ERROR;
                }
                else
                {
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
                            _APP_TimeExpired, (uintptr_t) &appData.tmr1Expired,
                            LED_BLINK_RATE_MS, SYS_TIME_PERIODIC);
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
