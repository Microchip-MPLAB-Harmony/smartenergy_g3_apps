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

#include "stdint.h"
#include <string.h>
#include "definitions.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************
#define DRV_G3_MACRT_INDEX_0   0

#define div_round(a, b)      (((a) + (b >> 1)) / (b))

extern uint8_t g3_mac_rt_bin_start;
extern uint8_t g3_mac_rt_bin_end;
extern uint8_t g3_mac_rt_bin2_start;
extern uint8_t g3_mac_rt_bin2_end;

/* G3 MAC RT Driver Initialization Data (initialization.c) */
extern DRV_G3_MACRT_INIT drvG3MacRtInitData;

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

APP_PLC_DATA appPlc;
APP_PLC_DATA_TX appPlcTx;

static CACHE_ALIGN uint8_t appPlcTxFrameBuffer[CACHE_ALIGNED_SIZE_GET(MAC_RT_DATA_MAX_SIZE)];
static CACHE_ALIGN uint8_t appPlcRxFrameBuffer[CACHE_ALIGNED_SIZE_GET(MAC_RT_DATA_MAX_SIZE)];

static void APP_PLC_SetInitialConfiguration ( void )
{
    /* Apply PLC coupling configuration */
    SRV_PCOUP_Set_Config(appPlc.drvPl360Handle, appPlc.couplingBranch);

    /* Force Transmission to VLO mode by default in order to maximize signal level in any case */
    /* Disable auto-detect mode */
    appPlc.plcPIB.pib = MAC_RT_PIB_MANUF_PHY_PARAM;
    appPlc.plcPIB.index = PHY_PIB_CFG_AUTODETECT_IMPEDANCE;
    appPlc.plcPIB.length = 1;
    appPlc.plcPIB.pData[0] = 0;
    DRV_G3_MACRT_PIBSet(appPlc.drvPl360Handle, &appPlc.plcPIB);

    /* Set VLO mode */
    appPlc.plcPIB.index = PHY_PIB_CFG_IMPEDANCE;
    appPlc.plcPIB.length = 1;
    appPlc.plcPIB.pData[0] = 2;
    DRV_G3_MACRT_PIBSet(appPlc.drvPl360Handle, &appPlc.plcPIB);

    /* Get PLC PHY version */
    appPlc.plcPIB.index = PHY_PIB_VERSION_NUM;
    appPlc.plcPIB.length = 4;
    DRV_G3_MACRT_PIBGet(appPlc.drvPl360Handle, &appPlc.plcPIB);
    memcpy((uint8_t *)&appPlc.phyVersion, appPlc.plcPIB.pData, 4);
    
    /* Fill MAC RT Header */
    appPlcTx.txHeader.frameControl.frameType = MAC_RT_FRAME_TYPE_DATA;
    appPlcTx.txHeader.frameControl.securityEnabled = 0;
    appPlcTx.txHeader.frameControl.framePending = 0;
    appPlcTx.txHeader.frameControl.ackRequest = 0; // No en Broadcast
    appPlcTx.txHeader.frameControl.frameVersion = 1;
    appPlcTx.txHeader.sequenceNumber = 0;
    
    /* Set PAN_ID */
    APP_PLC_SetPANID(CONF_PAN_ID);
    
    /* Set Addresses */
    APP_PLC_SetDestinationAddress(MAC_RT_SHORT_ADDRESS_BROADCAST);
    APP_PLC_SetSourceAddress((uint16_t)TRNG_ReadData());
}

static uint8_t APP_PLC_BuildMacRTHeader ( uint8_t *pFrame, MAC_RT_HEADER *pHeader )
{
    uint8_t *pData;
    
    pData = pFrame;
    
    /* Copy Frame Control and Sequence number */
    memcpy(pData, pHeader, 3);
    pData += 3;
    /* Build Header to use MAC_RT_SHORT_ADDRESS mode */
    /* Destination PAN ID */
    *pData++ = (uint8_t)(pHeader->destinationPAN);
    *pData++ = (uint8_t)(pHeader->destinationPAN >> 8);
    /* Destination address */
    *pData++ = (uint8_t)(pHeader->destinationAddress.shortAddress);
    *pData++ = (uint8_t)(pHeader->destinationAddress.shortAddress >> 8);
    /* panIdCompression = 1 -> No Source PAN ID */
    /* Source Address */
    *pData++ = (uint8_t)(pHeader->sourceAddress.shortAddress);
    *pData++ = (uint8_t)(pHeader->sourceAddress.shortAddress >> 8);
    
    /* Return Header length */
    return (uint8_t)(pData - pFrame);
}

static uint8_t APP_PLC_GetMacRTHeaderInfo ( uint8_t *pFrame )
{
    uint8_t *pData;
    uint16_t address;
    
    /* Frame Struct :
     * Frame Control(2b) + Seq Num(1b) + Dest PAN ID(2b) + Dest Addr(2b) + Src Addr(2b) */
    
    pData = pFrame;
    pData += 2;
    
    pData++; /* Sequence number */
    
    pData += 2; /* PAN ID */
    
    /* Destination address */
    address = *pData++;
    address += (uint16_t)*pData++ << 8;
    APP_CONSOLE_Print("Destination Address: 0x%04X ", address);

    /* panIdCompression = 1 -> No Source PAN ID */
    
    /* Source address */
    address = *pData++;
    address += (uint16_t)*pData++ << 8;
    APP_CONSOLE_Print(" - Source Address: 0x%04X\r\n", address);

    /* Return Header length */
    return (uint8_t)(pData - pFrame);
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************
static void APP_PLC_Timer1_Callback (uintptr_t context)
{
    appPlc.tmr1Expired = true;
}

static void APP_PLC_Timer2_Callback (uintptr_t context)
{
    appPlc.tmr2Expired = true;
}

static void APP_PLC_G3MACRTInitCallback(bool initResult)
{
    if (initResult == true)
    {
        /* Apply PLC initial configuration */
        APP_PLC_SetInitialConfiguration();
    }
    else
    {
        /* Error in G3 MAC RT initialization process */
        appPlc.state = APP_PLC_STATE_ERROR;
    }
}

static void APP_PLC_PVDDMonitorCallback( SRV_PVDDMON_CMP_MODE cmpMode, uintptr_t context )
{
    (void)context;
    
    if (cmpMode == SRV_PVDDMON_CMP_MODE_OUT)
    {
        /* PLC Transmission is not permitted */
        DRV_G3_MACRT_EnableTX(appPlc.drvPl360Handle, false);
        appPlc.pvddMonTxEnable = false;
        /* Restart PVDD Monitor to check when VDD is within the comparison window */
        SRV_PVDDMON_Restart(SRV_PVDDMON_CMP_MODE_IN);
    }
    else
    {
        /* PLC Transmission is permitted again */
        DRV_G3_MACRT_EnableTX(appPlc.drvPl360Handle, true);
        appPlc.pvddMonTxEnable = true;
        /* Restart PVDD Monitor to check when VDD is out of the comparison window */
        SRV_PVDDMON_Restart(SRV_PVDDMON_CMP_MODE_OUT);
    }
}

static void APP_PLC_SleepModeDisableCallback( void )
{
    /* Apply PLC initial configuration */
    APP_PLC_SetInitialConfiguration();

    /* Set PLC state */
    appPlc.state = APP_PLC_STATE_WAITING;
}

static void APP_PLC_ExceptionCallback( DRV_G3_MACRT_EXCEPTION exceptionObj )
{
    /* Avoid warning */
    (void)exceptionObj;

    /* Update PLC TX Status */
    appPlc.plcTxState = APP_PLC_TX_STATE_IDLE;
    /* Restart PLC task */
    appPlc.state = APP_PLC_STATE_IDLE;
}

static void APP_PLC_DataCfmCallback( MAC_RT_TX_CFM_OBJ *cfmObj )
{
    /* Update PLC TX Status */
    appPlc.plcTxState = APP_PLC_TX_STATE_IDLE;
    
    /* Capture TX result of the last transmission */
    appPlcTx.lastTxStatus = cfmObj->status;
}

static void APP_PLC_DataIndCallback( uint8_t *pData, uint16_t length )
{
    uint8_t *pFrame;
    uint8_t headerLength;
    
    /* Turn on indication LED and start timer to turn it off */
    SYS_TIME_TimerDestroy(appPlc.tmr2Handle);
    USER_PLC_IND_LED_On();
    appPlc.tmr2Handle = SYS_TIME_CallbackRegisterMS(APP_PLC_Timer2_Callback, 0, LED_PLC_RX_MSG_RATE_MS, SYS_TIME_SINGLE);

    APP_CONSOLE_Print("\rRx (");
    /* Show Modulation of received frame */
    if (appPlcTx.rxParams.modScheme == MAC_RT_MOD_SCHEME_DIFFERENTIAL)
    {
        if (appPlcTx.rxParams.modType == MAC_RT_MOD_ROBUST)
        {
            APP_CONSOLE_Print("BPSK Robust Differential, ");
        }
        else if (appPlcTx.rxParams.modType == MAC_RT_MOD_BPSK)
        {
            APP_CONSOLE_Print("BPSK Differential, ");
        }
        else if (appPlcTx.rxParams.modType == MAC_RT_MOD_QPSK)
        {
            APP_CONSOLE_Print("QPSK Differential, ");
        }
        else if (appPlcTx.rxParams.modType == MAC_RT_MOD_8PSK)
        {
            APP_CONSOLE_Print("8PSK Differential, ");
        }
    }
    else
    {   /* MOD_SCHEME_COHERENT */
        if (appPlcTx.rxParams.modType == MAC_RT_MOD_ROBUST)
        {
            APP_CONSOLE_Print("BPSK Robust Coherent, ");
        }
        else if (appPlcTx.rxParams.modType == MAC_RT_MOD_BPSK)
        {
            APP_CONSOLE_Print("BPSK Coherent, ");
        }
        else if (appPlcTx.rxParams.modType == MAC_RT_MOD_QPSK)
        {
            APP_CONSOLE_Print("QPSK Coherent, ");
        }
        else if (appPlcTx.rxParams.modType == MAC_RT_MOD_8PSK)
        {
            APP_CONSOLE_Print("8PSK Coherent, ");
        }
    }
    /* Show LQI (Link Quality Indicator). It is in quarters of dB and 10-dB 
     * offset: SNR(dB) = (LQI - 40) / 4 */
    APP_CONSOLE_Print("LQI %ddB): ", div_round((int16_t)appPlcTx.rxParams.pduLinkQuality - 40, 4));
    
    /* Extract MAC RT Header */
    pFrame = pData;
    headerLength = APP_PLC_GetMacRTHeaderInfo(pFrame);
    pFrame += headerLength;
    
    /* Show Payload */
    APP_CONSOLE_Print("%.*s", length - headerLength, pFrame);
    
    APP_CONSOLE_Print(MENU_CMD_PROMPT);
}

static void APP_PLC_RxParamsIndCallback( MAC_RT_RX_PARAMETERS_OBJ *pParameters )
{
    appPlcTx.rxParams.highPriority = pParameters->highPriority;
    appPlcTx.rxParams.pduLinkQuality = pParameters->pduLinkQuality;
    appPlcTx.rxParams.phaseDifferential = pParameters->phaseDifferential;
    appPlcTx.rxParams.modType = pParameters->modType;
    appPlcTx.rxParams.modScheme = pParameters->modScheme;
    
    memcpy(&appPlcTx.rxParams.toneMap, &pParameters->toneMap, sizeof(MAC_RT_TONE_MAP));
    
    memcpy(&appPlcTx.rxParams.toneMapRsp, &pParameters->toneMapRsp, sizeof(MAC_RT_TONE_MAP_RSP_DATA));
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
    /* Init PLC data buffers */
    appPlcTx.pTxFrame = appPlcTxFrameBuffer;
    appPlcTx.pRxFrame = appPlcRxFrameBuffer;
    
    /* Set PLC state */
    appPlc.state = APP_PLC_STATE_IDLE;
    
    /* Set PVDD Monitor tracking data */
    appPlc.pvddMonTxEnable = true;
    
    /* Init PLC TX status */
    appPlc.plcTxState = APP_PLC_TX_STATE_IDLE;

    /* Init Timer handler */
    appPlc.tmr1Handle = SYS_TIME_HANDLE_INVALID;
    appPlc.tmr2Handle = SYS_TIME_HANDLE_INVALID;
    appPlc.tmr1Expired = false;
    appPlc.tmr2Expired = false;
    
}

/******************************************************************************
  Function:
    void APP_PLC_Tasks ( void )

  Remarks:
    See prototype in app_plc.h.
 */

void APP_PLC_Tasks ( void )
{   
    /* Signalling */
    if (appPlc.tmr1Expired)
    {
        appPlc.tmr1Expired = false;
        USER_BLINK_LED_Toggle();
    }
    
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
            /* Set Static Notching capability (example only valid for FCC band */
            /* Caution: Example provided only for FCC band */
            appPlc.staticNotchingEnable = APP_PLC_STATIC_NOTCHING_ENABLE;

            /* Set PLC Multi-band / Coupling Branch flag */
            if (SRV_PCOUP_Get_Config(SRV_PLC_PCOUP_AUXILIARY_BRANCH) == NULL) {
                /* Auxiliary branch is not configured. Single branch */
                appPlc.plcMultiband = false;
                appPlc.bin2InUse = false;
            } else {
                /* Dual branch */
                appPlc.plcMultiband = true;
                appPlc.couplingBranch = SRV_PCOUP_Get_Default_Branch();
                if (appPlc.couplingBranch == SRV_PLC_PCOUP_MAIN_BRANCH)
                {
                    appPlc.bin2InUse = false;
                }
                else
                {
                    appPlc.bin2InUse = true;
                }
            }

            /* Initialize PLC driver */
            appPlc.state = APP_PLC_STATE_INIT;
        }
        break;

        case APP_PLC_STATE_INIT:
        {
            DRV_G3_MACRT_STATE drvG3MacRtStatus = DRV_G3_MACRT_Status(DRV_G3_MACRT_INDEX);
            
            /* Select PLC Binary file for multi-band solution */
            if (appPlc.plcMultiband && (drvG3MacRtStatus == DRV_G3_MACRT_STATE_UNINITIALIZED))
            {
                if (appPlc.bin2InUse)
                {
                    drvG3MacRtInitData.binStartAddress = (uint32_t)&g3_mac_rt_bin2_start;
                    drvG3MacRtInitData.binEndAddress = (uint32_t)&g3_mac_rt_bin2_end;
                    /* Set Coupling Auxiliary branch */
                    appPlc.couplingBranch = SRV_PLC_PCOUP_AUXILIARY_BRANCH;
                }
                else
                {
                    drvG3MacRtInitData.binStartAddress = (uint32_t)&g3_mac_rt_bin_start;
                    drvG3MacRtInitData.binEndAddress = (uint32_t)&g3_mac_rt_bin_end;
                }

                /* Initialize PLC Driver Instance */
                sysObj.drvG3MacRt = DRV_G3_MACRT_Initialize(DRV_G3_MACRT_INDEX, (SYS_MODULE_INIT *)&drvG3MacRtInitData);
                /* Register Callback function to handle PLC interruption */
                PIO_PinInterruptCallbackRegister(DRV_PLC_EXT_INT_PIN, DRV_G3_MACRT_ExternalInterruptHandler, sysObj.drvG3MacRt);
            }
            
            /* Set G3 MAC RT initialization callback */
            DRV_G3_MACRT_InitCallbackRegister(DRV_G3_MACRT_INDEX_0, APP_PLC_G3MACRTInitCallback);
            
            /* Open PLC driver */
            appPlc.drvPl360Handle = DRV_G3_MACRT_Open(DRV_G3_MACRT_INDEX_0, NULL);

            if (appPlc.drvPl360Handle != DRV_HANDLE_INVALID)
            {
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
            if (DRV_G3_MACRT_Status(DRV_G3_MACRT_INDEX_0) == DRV_G3_MACRT_STATE_READY)
            {
                /* Configure PLC callbacks */
                DRV_G3_MACRT_ExceptionCallbackRegister(appPlc.drvPl360Handle, APP_PLC_ExceptionCallback);
                DRV_G3_MACRT_TxCfmCallbackRegister(appPlc.drvPl360Handle, APP_PLC_DataCfmCallback);
                DRV_G3_MACRT_DataIndCallbackRegister(appPlc.drvPl360Handle, APP_PLC_DataIndCallback);
                DRV_G3_MACRT_RxParamsIndCallbackRegister(appPlc.drvPl360Handle, APP_PLC_RxParamsIndCallback);
                DRV_G3_MACRT_SleepIndCallbackRegister(appPlc.drvPl360Handle, APP_PLC_SleepModeDisableCallback);
                
                /* Enable PLC Transmission */
                DRV_G3_MACRT_EnableTX(appPlc.drvPl360Handle, true);                        
                
                /* Enable PLC PVDD Monitor Service */
                SRV_PVDDMON_CallbackRegister(APP_PLC_PVDDMonitorCallback, 0);
                SRV_PVDDMON_Start(SRV_PVDDMON_CMP_MODE_OUT);
            
                /* Init Timer to handle blinking led */
                appPlc.tmr1Handle = SYS_TIME_CallbackRegisterMS(APP_PLC_Timer1_Callback, 0, LED_BLINK_RATE_MS, SYS_TIME_PERIODIC);
                
                /* Set PLC state */
                appPlc.state = APP_PLC_STATE_WAITING;
            }
        }
        break;

        case APP_PLC_STATE_WAITING:
        {
            break;
        }

        case APP_PLC_STATE_WAITING_TX_CFM:
        {
            if (appPlc.plcTxState != APP_PLC_TX_STATE_WAIT_TX_CFM)
            {
                 appPlc.state = APP_PLC_STATE_WAITING;
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

            /* Close PLC Driver */
            DRV_G3_MACRT_Close(appPlc.drvPl360Handle);

            /* Restart PLC Driver */
            appPlc.state = APP_PLC_STATE_INIT;
            appPlc.plcTxState = APP_PLC_TX_STATE_IDLE;
            SYS_TIME_TimerDestroy(appPlc.tmr1Handle);
            break;
        }

        /* The default state should never be executed. */
        default:
        {
            /* Handle error in application's state machine. */
            break;
        }
    }
}

bool APP_PLC_SendData ( uint8_t* pData, uint16_t length )
{
    if (appPlc.state == APP_PLC_STATE_WAITING)
    {
        if (appPlc.pvddMonTxEnable)
        {
            uint8_t *pFrame;
            uint8_t headerLen;
            
            if (length > MAC_RT_MAX_PAYLOAD_SIZE) {
                return false;
            }
            
            appPlc.plcTxState = APP_PLC_TX_STATE_WAIT_TX_CFM;
            
            pFrame = appPlcTx.pTxFrame;
            
            /* Build MAC RT Frame */
            headerLen = APP_PLC_BuildMacRTHeader(pFrame, &appPlcTx.txHeader);
            pFrame += headerLen;
            
            /* Fill Payload */
            memcpy(pFrame, pData, length);
            pFrame += length;

            /* Send MAC RT Frame */
            DRV_G3_MACRT_TxRequest(appPlc.drvPl360Handle, appPlcTx.pTxFrame, 
                    pFrame - appPlcTx.pTxFrame);

            /* Set PLC state */
            if (appPlc.plcTxState == APP_PLC_TX_STATE_WAIT_TX_CFM)
            {
                appPlc.state = APP_PLC_STATE_WAITING_TX_CFM;
                return true;
            }
        }
    }
    
    return false;
}

bool APP_PLC_SetSleepMode ( bool enable )
{
    bool sleepIsEnabled = (appPlc.state == APP_PLC_STATE_SLEEP);
    
    if (sleepIsEnabled != enable)
    {
        DRV_G3_MACRT_Sleep(appPlc.drvPl360Handle, enable);
        if (enable)
        {
            appPlc.prevState = appPlc.state;
            appPlc.state = APP_PLC_STATE_SLEEP;
        }
        else
        {
            appPlc.state = appPlc.prevState;
        }
        
        return true;
    }
    
    return false;
}

void APP_PLC_SetSourceAddress ( uint16_t address )
{
    /* Set Short Address in TX header */
    appPlcTx.txHeader.frameControl.srcAddressingMode = MAC_RT_SHORT_ADDRESS;
    appPlcTx.txHeader.sourceAddress.addressMode = MAC_RT_SHORT_ADDRESS;
    appPlcTx.txHeader.sourceAddress.shortAddress = address;
    
    /* Set Short Address in G3 MAC RT device */
    appPlc.plcPIB.pib = MAC_RT_PIB_SHORT_ADDRESS;
    appPlc.plcPIB.index = 0;
    appPlc.plcPIB.length = 2;
    memcpy(appPlc.plcPIB.pData, (uint8_t *)&address, 2);
    DRV_G3_MACRT_PIBSet(appPlc.drvPl360Handle, &appPlc.plcPIB);
}

void APP_PLC_SetDestinationAddress ( uint16_t address )
{
    /* Set Short Address in TX header */
    appPlcTx.txHeader.frameControl.destAddressingMode = MAC_RT_SHORT_ADDRESS;
    appPlcTx.txHeader.destinationAddress.addressMode = MAC_RT_SHORT_ADDRESS;
    appPlcTx.txHeader.destinationAddress.shortAddress = address;
}

void APP_PLC_SetPANID ( uint16_t panid )
{
    /* Set Short Address in TX header */
    appPlcTx.txHeader.frameControl.panIdCompression = 1;
    appPlcTx.txHeader.destinationPAN = panid;
    appPlcTx.txHeader.sourcePAN = panid;
    
    /* Set Short Address in G3 MAC RT device */
    appPlc.plcPIB.pib = MAC_RT_PIB_PAN_ID;
    appPlc.plcPIB.index = 0;
    appPlc.plcPIB.length = 2;
    memcpy(appPlc.plcPIB.pData, (uint8_t *)&panid, 2);
    DRV_G3_MACRT_PIBSet(appPlc.drvPl360Handle, &appPlc.plcPIB);
}

/*******************************************************************************
 End of File
 */
