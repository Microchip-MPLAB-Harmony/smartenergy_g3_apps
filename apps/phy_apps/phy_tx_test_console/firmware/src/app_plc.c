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

/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_plc.c

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
#define DRV_PLC_PHY_INDEX_0   0
#define APP_PLC_CONFIG_KEY  0xA55A

extern uint8_t plc_phy_bin_start;
extern uint8_t plc_phy_bin_end;
extern uint8_t plc_phy_bin2_start;
extern uint8_t plc_phy_bin2_end;

/* PLC Driver Initialization Data (initialization.c) */
extern DRV_PLC_PHY_INIT drvPlcPhyInitData;

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

CACHE_ALIGN APP_PLC_DATA appPlc;
CACHE_ALIGN APP_PLC_DATA_TX appPlcTx;

static CACHE_ALIGN uint8_t appPlcPibDataBuffer[CACHE_ALIGNED_SIZE_GET(APP_PLC_PIB_BUFFER_SIZE)];
static CACHE_ALIGN uint8_t appPlcTxDataBuffer[CACHE_ALIGNED_SIZE_GET(APP_PLC_BUFFER_SIZE)];

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************
void APP_PLC_Timer1_Callback (uintptr_t context)
{
    appPlc.tmr1Expired = true;
}

void APP_PLC_Timer2_Callback (uintptr_t context)
{
    appPlc.tmr2Expired = true;
}

static void APP_PLC_ExceptionCb(DRV_PLC_PHY_EXCEPTION exceptionObj, uintptr_t context )
{
    /* Avoid warning */
    (void)context;

    /* Update PLC TX Status */
    appPlc.plcTxState = APP_PLC_TX_STATE_IDLE;
    /* Restore TX configuration */
    appPlc.state = APP_PLC_STATE_READ_CONFIG;
}

static void APP_PLC_DataCfmCb(DRV_PLC_PHY_TRANSMISSION_CFM_OBJ *cfmObj, uintptr_t context )
{
    /* Avoid warning */
    (void)context;

    /* Update PLC TX Status */
    appPlc.plcTxState = APP_PLC_TX_STATE_IDLE;

    /* Handle result of transmission : Show it through Console */
    switch(cfmObj->result)
    {
        case DRV_PLC_PHY_TX_RESULT_PROCESS:
            APP_CONSOLE_Print("...DRV_PLC_PHY_TX_RESULT_PROCESS\r\n");
            break;
        case DRV_PLC_PHY_TX_RESULT_SUCCESS:
            APP_CONSOLE_Print("...DRV_PLC_PHY_TX_RESULT_SUCCESS\r\n");
            break;
        case DRV_PLC_PHY_TX_RESULT_INV_LENGTH:
            APP_CONSOLE_Print("...DRV_PLC_PHY_TX_RESULT_INV_LENGTH\r\n");
            break;
        case DRV_PLC_PHY_TX_RESULT_BUSY_CH:
            APP_CONSOLE_Print("...DRV_PLC_PHY_TX_RESULT_BUSY_CH\r\n");
            break;
        case DRV_PLC_PHY_TX_RESULT_BUSY_TX:
            APP_CONSOLE_Print("...DRV_PLC_PHY_TX_RESULT_BUSY_TX\r\n");
            break;
        case DRV_PLC_PHY_TX_RESULT_BUSY_RX:
            APP_CONSOLE_Print("...DRV_PLC_PHY_TX_RESULT_BUSY_RX\r\n");
            break;
        case DRV_PLC_PHY_TX_RESULT_INV_SCHEME:
            APP_CONSOLE_Print("...DRV_PLC_PHY_TX_RESULT_INV_SCHEME\r\n");
            break;
        case DRV_PLC_PHY_TX_RESULT_TIMEOUT:
            APP_CONSOLE_Print("...DRV_PLC_PHY_TX_RESULT_TIMEOUT\r\n");
            break;
        case DRV_PLC_PHY_TX_RESULT_INV_TONEMAP:
            APP_CONSOLE_Print("...DRV_PLC_PHY_TX_RESULT_INV_TONEMAP\r\n");
            break;
        case DRV_PLC_PHY_TX_RESULT_INV_MODTYPE:
            APP_CONSOLE_Print("...DRV_PLC_PHY_TX_RESULT_INV_MODTYPE\r\n");
            break;
        case DRV_PLC_PHY_TX_RESULT_INV_DT:
            APP_CONSOLE_Print("...DRV_PLC_PHY_TX_RESULT_INV_DT\r\n");
            break;
        case DRV_PLC_PHY_TX_CANCELLED:
            APP_CONSOLE_Print("...DRV_PLC_PHY_TX_CANCELLED\r\n");
            break;
        case DRV_PLC_PHY_TX_RESULT_HIGH_TEMP_120:
            APP_CONSOLE_Print("...DRV_PLC_PHY_TX_RESULT_HIGH_TEMP_120\r\n");
            break;
        case DRV_PLC_PHY_TX_RESULT_HIGH_TEMP_110:
            APP_CONSOLE_Print("...DRV_PLC_PHY_TX_RESULT_HIGH_TEMP_110\r\n");
            break;
        case DRV_PLC_PHY_TX_RESULT_NO_TX:
            APP_CONSOLE_Print("...DRV_PLC_PHY_TX_RESULT_NO_TX\r\n");
            break;
    }
}

static void APP_PLC_DataIndCb( DRV_PLC_PHY_RECEPTION_OBJ *indObj, uintptr_t context )
{
    /* Avoid warning */
    (void)context;
    
    if (indObj->dataLength)
    {
        /* Turn on indication LED and start timer to turn it off */
        SYS_TIME_TimerDestroy(appPlc.tmr2Handle);
        USER_PLC_IND_LED_On();
        appPlc.tmr2Handle = SYS_TIME_CallbackRegisterMS(APP_PLC_Timer2_Callback, 0, LED_PLC_RX_MSG_RATE_MS, SYS_TIME_SINGLE);
    }
}

static void APP_PLC_PVDDMonitorCb( SRV_PVDDMON_CMP_MODE cmpMode, uintptr_t context )
{
    (void)context;
    
    if (cmpMode == SRV_PVDDMON_CMP_MODE_OUT)
    {
        /* PLC Transmission is not permitted */
        DRV_PLC_PHY_EnableTX(appPlc.drvPl360Handle, false);
        appPlc.pvddMonTxEnable = false;
        /* Restart PVDD Monitor to check when VDD is within the comparison window */
        SRV_PVDDMON_Restart(SRV_PVDDMON_CMP_MODE_IN);
    }
    else
    {
        /* PLC Transmission is permitted again */
        DRV_PLC_PHY_EnableTX(appPlc.drvPl360Handle, true);
        appPlc.pvddMonTxEnable = true;
        /* Restart PVDD Monitor to check when VDD is out of the comparison window */
        SRV_PVDDMON_Restart(SRV_PVDDMON_CMP_MODE_OUT);
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_PLC_Initialize(void)

  Remarks:
    See prototype in app_plc.h.
 */
void APP_PLC_Initialize ( void )
{
    /* IDLE state is used to signal when application is started */
    appPlc.state = APP_PLC_STATE_IDLE;

    /* Init PLC PIB buffer */
    appPlc.plcPIB.pData = appPlcPibDataBuffer;

    /* Init PLC TX Buffer */
    appPlcTx.pl360Tx.pTransmitData = appPlcTxDataBuffer;
    
    /* Set PLC Multiband flag */
    if (SRV_PCOUP_Get_Config(SRV_PLC_PCOUP_AUXILIARY_BRANCH) == NULL) {
        appPlc.plcMultiband = false;
    } else {
        appPlc.plcMultiband = true;
    }
    
    /* Init Timer handler */
    appPlc.tmr1Handle = SYS_TIME_HANDLE_INVALID;
    appPlc.tmr2Handle = SYS_TIME_HANDLE_INVALID;
    appPlc.tmr1Expired = false;
    appPlc.tmr2Expired = false;
    
    /* Init signalling */
    appPlc.signalResetCounter = LED_RESET_BLINK_COUNTER;
    
    /* Set PVDD Monitor tracking data */
    appPlc.pvddMonTxEnable = true;
    
    /* Init PLC TX status */
    appPlc.plcTxState = APP_PLC_TX_STATE_IDLE;
    
}

/******************************************************************************
  Function:
    void APP_PLC_Tasks ( void )

  Remarks:
    See prototype in app_plc.h.
 */

void APP_PLC_Tasks ( void )
{    
    /* Signalling: LED Toggle */
    if (appPlc.tmr1Expired)
    {
        appPlc.tmr1Expired = false;
        USER_BLINK_LED_Toggle();
        
        if (appPlc.signalResetCounter)
        {
            appPlc.signalResetCounter--;
        }
    }
    
    /* Signalling: PLC RX */
    if (appPlc.tmr2Expired)
    {
        appPlc.tmr2Expired = false;
        USER_PLC_IND_LED_Off();
    }

    /* Check the application's current state. */
    switch ( appPlc.state )
    {
        case APP_PLC_STATE_IDLE:
        {
            /* Signalling when the application is starting */
            if (appPlc.signalResetCounter)
            {
                if (appPlc.tmr1Handle == SYS_TIME_HANDLE_INVALID)
                {
                    /* Init Timer to handle blinking led */
                    appPlc.tmr1Handle = SYS_TIME_CallbackRegisterMS(APP_PLC_Timer1_Callback, 0, LED_RESET_BLINK_RATE_MS, SYS_TIME_PERIODIC);
                }
            }
            else
            {
                SYS_TIME_TimerDestroy(appPlc.tmr1Handle);
                appPlc.tmr1Handle = SYS_TIME_HANDLE_INVALID;
                
                /* Read configuration from NVM memory */
                appPlc.state = APP_PLC_STATE_READ_CONFIG;
            }
            break;
        }
        
        case APP_PLC_STATE_READ_CONFIG:
        {
            if (appNvm.state == APP_NVM_STATE_CMD_WAIT)
            {
                appNvm.pData = (uint8_t*)&appPlcTx;
                appNvm.dataLength = sizeof(appPlcTx);
                appNvm.state = APP_NVM_STATE_READ_MEMORY;

                appPlc.state = APP_PLC_STATE_CHECK_CONFIG;
            }
            break;
        }

        case APP_PLC_STATE_CHECK_CONFIG:
        {
            if (appNvm.state == APP_NVM_STATE_CMD_WAIT)
            {
                if (appPlcTx.configKey != APP_PLC_CONFIG_KEY)
                {
                    uint8_t index;
                    uint8_t* pData;

                    /* Set configuration by default */
                    appPlcTx.configKey = APP_PLC_CONFIG_KEY;
                    appPlcTx.pl360PhyVersion = 0;
                    appPlcTx.txImpedance = HI_STATE;
                    appPlcTx.txAuto = 1;
                    appPlcTx.pl360Tx.time = 1000000;
                    appPlcTx.pl360Tx.attenuation = 0;
                    appPlcTx.pl360Tx.modScheme = MOD_SCHEME_DIFFERENTIAL;
                    appPlcTx.pl360Tx.modType = MOD_TYPE_BPSK;
                    appPlcTx.pl360Tx.delimiterType = DT_SOF_NO_RESP;
                    appPlcTx.pl360Tx.mode = TX_MODE_RELATIVE;
                    appPlcTx.pl360Tx.rs2Blocks = 0;
                    appPlcTx.pl360Tx.pdc = 0;
                    appPlcTx.pl360Tx.dataLength = 64;
                    appPlcTx.pl360Tx.pTransmitData = appPlcTxDataBuffer;
                    pData = appPlcTx.pl360Tx.pTransmitData;
                    for(index = 0; index < appPlcTx.pl360Tx.dataLength; index++)
                    {
                        *pData++ = index;
                    }

                    /* CEN A */
                    appPlcTx.toneMapSize = 1;
                    appPlcTx.pl360Tx.toneMap[0] = 0x3F;

                    memset(appPlcTx.pl360Tx.preemphasis, 0, sizeof(appPlcTx.pl360Tx.preemphasis));

                    /* Clear Transmission flag */
                    appPlcTx.inTx = false;

                    /* Select PLC binary by default */
                    appPlcTx.bin2InUse = false;
                }

                /* Initialize PLC driver */
                appPlc.state = APP_PLC_STATE_INIT;
            }
            break;
        }

        case APP_PLC_STATE_WRITE_CONFIG:
        {
            if (appNvm.state == APP_NVM_STATE_CMD_WAIT)
            {
                appNvm.pData = (uint8_t*)&appPlcTx;
                appNvm.dataLength = sizeof(appPlcTx);
                appNvm.state = APP_NVM_STATE_WRITE_MEMORY;

                appPlc.state = APP_PLC_STATE_WAIT_CONFIG;
            }
            break;
        }

        case APP_PLC_STATE_WAIT_CONFIG:
        {
            if (appNvm.state == APP_NVM_STATE_CMD_WAIT)
            {
                if (appPlcTx.inTx)
                {
                    appPlc.state = APP_PLC_STATE_TX;
                }
                else
                {
                    appPlc.state = APP_PLC_STATE_WAITING;
                }
            }
            break;
        }

        case APP_PLC_STATE_INIT:
        {
            SYS_STATUS drvPlcStatus = DRV_PLC_PHY_Status(DRV_PLC_PHY_INDEX);

            /* Set Coupling branch by default */
            appPlcTx.couplingBranch = SRV_PLC_PCOUP_MAIN_BRANCH;
            
            /* Select PLC Binary file for multi-band solution */
            if (appPlc.plcMultiband == true)
            {
                if (appPlcTx.bin2InUse == true)
                {
                    drvPlcPhyInitData.binStartAddress = (uint32_t)&plc_phy_bin2_start;
                    drvPlcPhyInitData.binEndAddress = (uint32_t)&plc_phy_bin2_end;
                    /* Set Coupling Auxiliary branch */
                    appPlcTx.couplingBranch = SRV_PLC_PCOUP_AUXILIARY_BRANCH;
                }
                else
                {
                    drvPlcPhyInitData.binStartAddress = (uint32_t)&plc_phy_bin_start;
                    drvPlcPhyInitData.binEndAddress = (uint32_t)&plc_phy_bin_end;
                }

                if (drvPlcStatus == SYS_STATUS_UNINITIALIZED)
                {
                    /* Initialize PLC Driver Instance */
                    sysObj.drvPlcPhy = DRV_PLC_PHY_Initialize(DRV_PLC_PHY_INDEX, (SYS_MODULE_INIT *)&drvPlcPhyInitData);
                    /* Register Callback function to handle PLC interruption */
                    PIO_PinInterruptCallbackRegister(DRV_PLC_EXT_INT_PIN, DRV_PLC_PHY_ExternalInterruptHandler, sysObj.drvPlcPhy);
                }
            }
            
            /* Open PLC driver */
            appPlc.drvPl360Handle = DRV_PLC_PHY_Open(DRV_PLC_PHY_INDEX_0, NULL);

            if (appPlc.drvPl360Handle != DRV_HANDLE_INVALID)
            {
                if ((appPlc.plcMultiband == true) && (appPlcTx.bin2InUse == true) && (drvPlcStatus == SYS_STATUS_BUSY))
                {
                    /* Close PLC Driver */
                    DRV_PLC_PHY_Close(appPlc.drvPl360Handle);

                    /* Initialize PLC Driver Instance */
                    sysObj.drvPlcPhy = DRV_PLC_PHY_Initialize(DRV_PLC_PHY_INDEX, (SYS_MODULE_INIT *)&drvPlcPhyInitData);
                    /* Register Callback function to handle PLC interruption */
                    PIO_PinInterruptCallbackRegister(DRV_PLC_EXT_INT_PIN, DRV_PLC_PHY_ExternalInterruptHandler, sysObj.drvPlcPhy);

                    /* Open PLC driver again */
                    appPlc.drvPl360Handle = DRV_PLC_PHY_Open(DRV_PLC_PHY_INDEX_0, NULL);
                }

                appPlc.state = APP_PLC_STATE_OPEN;
            }
            else
            {
                appPlc.state = APP_PLC_STATE_ERROR;
            }
            break;
        }

        case APP_PLC_STATE_OPEN:
        {
            /* Check PLC transceiver */
            if (DRV_PLC_PHY_Status(DRV_PLC_PHY_INDEX_0) == SYS_STATUS_READY)
            {
                DRV_PLC_PHY_PIB_OBJ pibObj;
                uint32_t version;

                /* Configure PLC callbacks */
                DRV_PLC_PHY_ExceptionCallbackRegister(appPlc.drvPl360Handle, APP_PLC_ExceptionCb, DRV_PLC_PHY_INDEX_0);
                DRV_PLC_PHY_TxCfmCallbackRegister(appPlc.drvPl360Handle, APP_PLC_DataCfmCb, DRV_PLC_PHY_INDEX_0);
                DRV_PLC_PHY_DataIndCallbackRegister(appPlc.drvPl360Handle, APP_PLC_DataIndCb, DRV_PLC_PHY_INDEX_0);
                
                /* Apply PLC coupling configuration */
                SRV_PCOUP_Set_Config(appPlc.drvPl360Handle, appPlcTx.couplingBranch);
                
                /* Disable TX Enable at the beginning */
                DRV_PLC_PHY_EnableTX(appPlc.drvPl360Handle, false);
                appPlc.pvddMonTxEnable = false;
                /* Enable PLC PVDD Monitor Service */
                SRV_PVDDMON_CallbackRegister(APP_PLC_PVDDMonitorCb, 0);
                SRV_PVDDMON_Start(SRV_PVDDMON_CMP_MODE_IN);
                
                /* Init Timer to handle blinking led */
                appPlc.tmr1Handle = SYS_TIME_CallbackRegisterMS(APP_PLC_Timer1_Callback, 0, LED_BLINK_RATE_MS, SYS_TIME_PERIODIC);
                
                /* Get PLC PHY version */
                pibObj.id = PLC_ID_VERSION_NUM;
                pibObj.length = 4;
                pibObj.pData = (uint8_t *)&version;
                DRV_PLC_PHY_PIBGet(appPlc.drvPl360Handle, &pibObj);

                if (version == appPlcTx.pl360PhyVersion)
                {
                    if (appPlcTx.inTx)
                    {
                        /* Previous Transmission state */
                        appPlc.state = APP_PLC_STATE_TX;
                    }
                    else
                    {
                        /* Nothing To Do */
                        appPlc.state = APP_PLC_STATE_WAITING;
                    }
                }
                else
                {
                    appPlcTx.pl360PhyVersion = version;

                    /* Adjust ToneMap Info */
                    switch ((uint8_t)(appPlcTx.pl360PhyVersion >> 16))
                    {
                        case 1:
                            /* CEN A */
                            appPlcTx.toneMapSize = TONE_MAP_SIZE_CENELEC;
                            appPlcTx.pl360Tx.toneMap[0] = 0x3F;
                            break;

                        case 2:
                            /* FCC */
                            appPlcTx.toneMapSize = TONE_MAP_SIZE_FCC;
                            appPlcTx.pl360Tx.toneMap[0] = 0xFF;
                            appPlcTx.pl360Tx.toneMap[1] = 0xFF;
                            appPlcTx.pl360Tx.toneMap[2] = 0xFF;
                            break;

                        case 3:
                            /* ARIB */
                            appPlcTx.toneMapSize = TONE_MAP_SIZE_FCC;
                            appPlcTx.pl360Tx.toneMap[0] = 0x03;
                            appPlcTx.pl360Tx.toneMap[1] = 0xFF;
                            appPlcTx.pl360Tx.toneMap[2] = 0xFF;
                            break;

                        case 4:
                            /* CEN B */
                            appPlcTx.toneMapSize = TONE_MAP_SIZE_CENELEC;
                            appPlcTx.pl360Tx.toneMap[0] = 0x0F;
                            break;
                    }

                    /* Store configuration in NVM memory */
                    appPlc.state = APP_PLC_STATE_WRITE_CONFIG;
                }
            }
            break;
        }

        case APP_PLC_STATE_WAITING:
        {
            break;
        }

        case APP_PLC_STATE_TX:
        {
            if (!appPlcTx.inTx)
            {
                DRV_PLC_PHY_PIB_OBJ pibObj;

                /* Apply TX configuration */
                /* Set Autodetect Mode */
                pibObj.id = PLC_ID_CFG_AUTODETECT_IMPEDANCE;
                pibObj.length = 1;
                pibObj.pData = (uint8_t *)&appPlcTx.txAuto;
                DRV_PLC_PHY_PIBSet(appPlc.drvPl360Handle, &pibObj);
                /* Set Impedance Mode */
                pibObj.id = PLC_ID_CFG_IMPEDANCE;
                pibObj.length = 1;
                pibObj.pData = (uint8_t *)&appPlcTx.txImpedance;
                DRV_PLC_PHY_PIBSet(appPlc.drvPl360Handle, &pibObj);

                /* Set Transmission Mode */
                appPlcTx.pl360Tx.mode = TX_MODE_RELATIVE;

                /* Set Transmission flag */
                appPlcTx.inTx = true;

                /* Store TX configuration */
                appPlc.state = APP_PLC_STATE_WRITE_CONFIG;
            }
            else
            {
                if (appPlc.plcTxState == APP_PLC_TX_STATE_IDLE)
                {
                    if (appPlc.pvddMonTxEnable)
                    {
                        appPlc.plcTxState = APP_PLC_TX_STATE_WAIT_TX_CFM;
                        /* Send PLC message */
                        DRV_PLC_PHY_TxRequest(appPlc.drvPl360Handle, &appPlcTx.pl360Tx);
                    }
                    else
                    {
                        DRV_PLC_PHY_TRANSMISSION_CFM_OBJ cfmData;
                        
                        cfmData.time = 0;
                        cfmData.rmsCalc = 0;
                        cfmData.result = DRV_PLC_PHY_TX_RESULT_NO_TX;
                        APP_PLC_DataCfmCb(&cfmData, 0);
                    }
                }
            }

            break;
        }

        case APP_PLC_STATE_STOP_TX:
        {
            /* Clear Transmission flag */
            appPlcTx.inTx = false;

            /* Store TX configuration */
            appPlc.state = APP_PLC_STATE_WRITE_CONFIG;

            /* Cancel last transmission */
            if (appPlc.plcTxState == APP_PLC_TX_STATE_WAIT_TX_CFM)
            {
                /* Send PLC Cancel message */
                appPlcTx.pl360Tx.mode = TX_MODE_CANCEL | TX_MODE_RELATIVE;
                DRV_PLC_PHY_TxRequest(appPlc.drvPl360Handle, &appPlcTx.pl360Tx);
            }
            break;
        }

        case APP_PLC_STATE_SET_BAND:
        {
            if (!appPlc.plcMultiband)
            {
                /* PLC Multi-band is not supported */
                appPlc.state = APP_PLC_STATE_WAITING;
                break;
            }

            /* Clear Transmission flags */
            appPlcTx.inTx = false;

            /* Close PLC Driver */
            DRV_PLC_PHY_Close(appPlc.drvPl360Handle);
            
            /* Destroy Blink Timer */
            SYS_TIME_TimerDestroy(appPlc.tmr1Handle);
            appPlc.tmr1Handle = SYS_TIME_HANDLE_INVALID;

            /* Restart PLC Driver */
            appPlc.state = APP_PLC_STATE_INIT;
            appPlc.plcTxState = APP_PLC_TX_STATE_IDLE;
            break;
        }

        /* The default state should never be executed. */
        default:
        {
            break;
        }
    }
}


/*******************************************************************************
 End of File
 */
