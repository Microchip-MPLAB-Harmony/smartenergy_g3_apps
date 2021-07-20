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

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <string.h>
#include "app_plc.h"
#include "definitions.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************
#define DRV_PLC_PHY_INDEX_0   0

#define div_round(a, b)      (((a) + (b >> 1)) / (b))

extern uint8_t plc_phy_bin_start;
extern uint8_t plc_phy_bin_end;
extern uint8_t plc_phy_bin2_start;
extern uint8_t plc_phy_bin2_end;

/* PLC Driver Initialization Data (initialization.c) */
extern DRV_PLC_PHY_INIT drvPlcPhyInitData;

// *****************************************************************************
/* Pointer to PLC Coupling configuration data

  Summary:
    Holds PLC configuration data

  Description:
    This structure holds the PLC coupling configuration data.

  Remarks:
    Parameters are defined in srv_pcoup.h file
 */

SRV_PLC_PCOUP *appPLCCoupling;

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

APP_PLC_DATA CACHE_ALIGN appPlc;
APP_PLC_DATA_TX CACHE_ALIGN appPlcTx;

static uint8_t appPlcStaticNotching[NUM_CARRIERS_CENELEC_A] = APP_PLC_TONE_MASK_STATIC_NOTCHING_EXAMPLE;

static void APP_PLC_SetCouplingConfiguration ( SRV_PLC_PCOUP_BRANCH branch )
{
    appPLCCoupling = SRV_PCOUP_Get_Config(branch);

    appPlc.plcPIB.id = PLC_ID_IC_DRIVER_CFG;
    appPlc.plcPIB.length = 1;
    *appPlc.plcPIB.pData = appPLCCoupling->lineDrvConf;
    DRV_PLC_PHY_PIBSet(appPlc.drvPl360Handle, &appPlc.plcPIB);

    appPlc.plcPIB.id = PLC_ID_DACC_TABLE_CFG;
    appPlc.plcPIB.length = sizeof(appPLCCoupling->daccTable);
    memcpy(appPlc.plcPIB.pData, (uint8_t *)appPLCCoupling->daccTable,
            appPlc.plcPIB.length);
    DRV_PLC_PHY_PIBSet(appPlc.drvPl360Handle, &appPlc.plcPIB);

    appPlc.plcPIB.id = PLC_ID_NUM_TX_LEVELS;
    appPlc.plcPIB.length = 1;
    *appPlc.plcPIB.pData = appPLCCoupling->numTxLevels;
    DRV_PLC_PHY_PIBSet(appPlc.drvPl360Handle, &appPlc.plcPIB);

    appPlc.plcPIB.id = PLC_ID_MAX_RMS_TABLE_HI;
    appPlc.plcPIB.length = sizeof(appPLCCoupling->rmsHigh);
    memcpy(appPlc.plcPIB.pData, (uint8_t *)appPLCCoupling->rmsHigh,
            appPlc.plcPIB.length);
    DRV_PLC_PHY_PIBSet(appPlc.drvPl360Handle, &appPlc.plcPIB);

    appPlc.plcPIB.id = PLC_ID_MAX_RMS_TABLE_VLO;
    appPlc.plcPIB.length = sizeof(appPLCCoupling->rmsVLow);
    memcpy(appPlc.plcPIB.pData, (uint8_t *)appPLCCoupling->rmsVLow,
            appPlc.plcPIB.length);
    DRV_PLC_PHY_PIBSet(appPlc.drvPl360Handle, &appPlc.plcPIB);

    appPlc.plcPIB.id = PLC_ID_THRESHOLDS_TABLE_HI;
    appPlc.plcPIB.length = sizeof(appPLCCoupling->thrsHigh);
    memcpy(appPlc.plcPIB.pData, (uint8_t *)appPLCCoupling->thrsHigh,
            appPlc.plcPIB.length);
    DRV_PLC_PHY_PIBSet(appPlc.drvPl360Handle, &appPlc.plcPIB);

    appPlc.plcPIB.id = PLC_ID_THRESHOLDS_TABLE_VLO;
    appPlc.plcPIB.length = sizeof(appPLCCoupling->thrsVLow);
    memcpy(appPlc.plcPIB.pData, (uint8_t *)appPLCCoupling->thrsVLow,
            appPlc.plcPIB.length);
    DRV_PLC_PHY_PIBSet(appPlc.drvPl360Handle, &appPlc.plcPIB);

    appPlc.plcPIB.id = PLC_ID_GAIN_TABLE_HI;
    appPlc.plcPIB.length = sizeof(appPLCCoupling->gainHigh);
    memcpy(appPlc.plcPIB.pData, (uint8_t *)appPLCCoupling->gainHigh,
            appPlc.plcPIB.length);
    DRV_PLC_PHY_PIBSet(appPlc.drvPl360Handle, &appPlc.plcPIB);

    appPlc.plcPIB.id = PLC_ID_GAIN_TABLE_VLO;
    appPlc.plcPIB.length = sizeof(appPLCCoupling->gainVLow);
    memcpy(appPlc.plcPIB.pData, (uint8_t *)appPLCCoupling->gainVLow,
            appPlc.plcPIB.length);
    DRV_PLC_PHY_PIBSet(appPlc.drvPl360Handle, &appPlc.plcPIB);

    appPlc.plcPIB.id = PLC_ID_PREDIST_COEF_TABLE_HI;
    appPlc.plcPIB.length = appPLCCoupling->equSize;
    memcpy(appPlc.plcPIB.pData, (uint8_t *)appPLCCoupling->equHigh,
            appPlc.plcPIB.length);
    DRV_PLC_PHY_PIBSet(appPlc.drvPl360Handle, &appPlc.plcPIB);

    appPlc.plcPIB.id = PLC_ID_PREDIST_COEF_TABLE_VLO;
    /* Not use size of array. It depends on PHY band in use */
    appPlc.plcPIB.length = appPLCCoupling->equSize;
    memcpy(appPlc.plcPIB.pData, (uint8_t *)appPLCCoupling->equVlow,
            appPlc.plcPIB.length);
    DRV_PLC_PHY_PIBSet(appPlc.drvPl360Handle, &appPlc.plcPIB);
}

static void APP_PLC_SetInitialConfiguration ( void )
{
    DRV_PLC_PHY_PIB_OBJ pibObj;
    uint8_t plcCrcEnable;
    bool applyStaticNotching = false;
                
    /* Apply PLC coupling configuration */
    APP_PLC_SetCouplingConfiguration(appPlcTx.couplingBranch);

    /* Force Transmission to VLO mode by default in order to maximize signal level in anycase */
    /* Disable autodetect mode */
    appPlcTx.txAuto = 0;
    pibObj.id = PLC_ID_CFG_AUTODETECT_IMPEDANCE;
    pibObj.length = 1;
    pibObj.pData = (uint8_t *)&appPlcTx.txAuto;
    DRV_PLC_PHY_PIBSet(appPlc.drvPl360Handle, &pibObj);

    /* Set VLO mode */
    appPlcTx.txImpedance = 2;
    pibObj.id = PLC_ID_CFG_IMPEDANCE;
    pibObj.length = 1;
    pibObj.pData = (uint8_t *)&appPlcTx.txImpedance;
    DRV_PLC_PHY_PIBSet(appPlc.drvPl360Handle, &pibObj);

    /* Get PLC PHY version */
    pibObj.id = PLC_ID_VERSION_NUM;
    pibObj.length = 4;
    pibObj.pData = (uint8_t *)&appPlcTx.pl360PhyVersion;
    DRV_PLC_PHY_PIBGet(appPlc.drvPl360Handle, &pibObj);

    /* Adjust ToneMap Info */
    switch ((uint8_t)(appPlcTx.pl360PhyVersion >> 16))
    {
        case 1:
            /* CEN A */
            appPlcTx.toneMapSize = TONE_MAP_SIZE_CENELEC;
            appPlcTx.pl360Tx.toneMap[0] = 0x3F;
            if (appPlc.staticNotchingEnable)
            {
                /* Caution: Example provided only for CEN-A band */
                applyStaticNotching = true;
            }
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
            appPlcTx.toneMapSize = TONE_MAP_SIZE_ARIB;
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

    /* Enable CRC calculation for transmission and reception in PLC PHY layer */
    /* In Transmission, 16-bit CRC is computed and added to data payload by PHY layer in PLC device */
    /* In Reception, CRC is checked by PHY layer in PLC device and the result is reported in uc_crc_ok field in rx_msg_t structure */
    /* The CRC format is the same that uses the G3-PLC stack, which is described in the IEEE 802.15.4 standard. */
    plcCrcEnable = 1;
    pibObj.id = PLC_ID_CRC_TX_RX_CAPABILITY;
    pibObj.length = 1;
    pibObj.pData = (uint8_t *)&plcCrcEnable;
    DRV_PLC_PHY_PIBSet(appPlc.drvPl360Handle, &pibObj);

    if (applyStaticNotching)
    {
        /* Caution: Example provided only for CEN-A band */
        pibObj.id = PLC_ID_TONE_MASK;
        pibObj.length = NUM_CARRIERS_CENELEC_A;
        pibObj.pData = (uint8_t *)&appPlcStaticNotching;
        DRV_PLC_PHY_PIBSet(appPlc.drvPl360Handle, &pibObj);
    }

    /* Get maximum data length allowed with configured Tx Parameters */
    APP_PLC_SetModScheme(appPlcTx.pl360Tx.modType, appPlcTx.pl360Tx.modScheme);
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

static void APP_PLC_SleepModeDisableCb( uintptr_t context )
{
    /* Avoid warning */
    (void)context;

    /* Apply PLC initial configuration */
    APP_PLC_SetInitialConfiguration();

    /* Set PLC state */
    appPlc.state = APP_PLC_STATE_WAITING;
}

static void APP_PLC_ExceptionCb(DRV_PLC_PHY_EXCEPTION exceptionObj, uintptr_t context )
{
    /* Avoid warning */
    (void)context;
    (void)exceptionObj;

    /* Clear App flag */
    appPlc.waitingTxCfm = false;
    /* Restart PLC task */
    appPlc.state = APP_PLC_STATE_IDLE;
}

static void APP_PLC_DataCfmCb(DRV_PLC_PHY_TRANSMISSION_CFM_OBJ *cfmObj, uintptr_t context )
{
    /* Avoid warning */
    (void)context;

    /* Update App flags */
    appPlc.waitingTxCfm = false;
    appPlc.lastTxResult = cfmObj->result;

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
        case DRV_PLC_PHY_TX_RESULT_NO_TX:
            APP_CONSOLE_Print("...DRV_PLC_PHY_TX_RESULT_NO_TX\r\n");
            break;
    }
}

static void APP_PLC_DataIndCb( DRV_PLC_PHY_RECEPTION_OBJ *indObj, uintptr_t context )
{
    /* Avoid warning */
    (void)context;
    
    if (indObj->crcOk == 1)
    {
        uint16_t us_len;
        
        us_len = (uint16_t)indObj->pReceivedData[0] << 8;
        us_len += (uint16_t)indObj->pReceivedData[1];
        
        if (us_len > (indObj->dataLength))
        {
            /* Length error: length in message content should never be more than total data length from PHY */
            APP_CONSOLE_Print("\rRx ERROR: length error\r\n");
        }
        else
        {
            APP_CONSOLE_Print("\rRx (");
            /* Show Modulation of received frame */
            if (indObj->modScheme == MOD_SCHEME_DIFFERENTIAL)
            {
                if (indObj->modType == MOD_TYPE_BPSK_ROBO)
                {
                    APP_CONSOLE_Print("BPSK Robust Differential, ");
                }
                else if (indObj->modType == MOD_TYPE_BPSK)
                {
                    APP_CONSOLE_Print("BPSK Differential, ");
                }
                else if (indObj->modType == MOD_TYPE_QPSK)
                {
                    APP_CONSOLE_Print("QPSK Differential, ");
                }
                else if (indObj->modType == MOD_TYPE_8PSK)
                {
                    APP_CONSOLE_Print("8PSK Differential, ");
                }
            }
            else
            {   /* MOD_SCHEME_COHERENT */
                if (indObj->modType == MOD_TYPE_BPSK_ROBO)
                {
                    APP_CONSOLE_Print("BPSK Robust Coherent, ");
                }
                else if (indObj->modType == MOD_TYPE_BPSK)
                {
                    APP_CONSOLE_Print("BPSK Coherent, ");
                }
                else if (indObj->modType == MOD_TYPE_QPSK)
                {
                    APP_CONSOLE_Print("QPSK Coherent, ");
                }
                else if (indObj->modType == MOD_TYPE_8PSK)
                {
                    APP_CONSOLE_Print("8PSK Coherent, ");
                }
            }
            /* Show RSSI (Received Signal Strength Indicator) in dBuV */
            APP_CONSOLE_Print("RSSI %udBuV, ", indObj->rssi);
            /* Show LQI (Link Quality Indicator). It is in quarters of dB and 10-dB offset: SNR(dB) = (LQI - 40) / 4 */
            APP_CONSOLE_Print("LQI %ddB): ", div_round((int16_t)indObj->lqi - 40, 4));
            APP_CONSOLE_Print("%.*s", us_len, indObj->pReceivedData + 2);
        }
    }
    else
    {
        APP_CONSOLE_Print("\rRx ERROR: CRC error\r\n");
    }
    
    APP_CONSOLE_Print(MENU_CMD_PROMPT);
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
    /* Init flags of PLC transmission */
    appPlc.waitingTxCfm = false;

    /* Init PLC PIB buffer */
    appPlc.plcPIB.pData = appPlc.pPLCDataPIB;

    /* Init PLC objects */
    appPlcTx.pl360Tx.pTransmitData = appPlcTx.pDataTx;
    
    /* Set PLC state */
    appPlc.state = APP_PLC_STATE_IDLE;
}

/******************************************************************************
  Function:
    void APP_PLC_Tasks ( void )

  Remarks:
    See prototype in app_plc.h.
 */

void APP_PLC_Tasks ( void )
{
    /* Check the application's current state. */
    switch ( appPlc.state )
    {
        case APP_PLC_STATE_IDLE:
        {
            /* Set Static Notching capability (example only valid for CEN-A band */
            /* Caution: Example provided only for CEN-A band */
            appPlc.staticNotchingEnable = APP_PLC_STATIC_NOTCHING_ENABLE;

            /* Set configuration by default */
            appPlcTx.pl360Tx.time = 0;
            appPlcTx.pl360Tx.attenuation = 0;
            appPlcTx.pl360Tx.modScheme = MOD_SCHEME_DIFFERENTIAL;
            appPlcTx.pl360Tx.modType = MOD_TYPE_BPSK;
            appPlcTx.pl360Tx.delimiterType = DT_SOF_NO_RESP;
            appPlcTx.pl360Tx.mode = TX_MODE_FORCED | TX_MODE_RELATIVE;
            /* Set 1 Reed-Solomon block. In this example it cannot be configured dynamically. To test 2 Reed-Solomon blocks change 0 by 1 (Only for FCC). */
            appPlcTx.pl360Tx.rs2Blocks = 0;
            appPlcTx.pl360Tx.pdc = 0;
            appPlcTx.pl360Tx.pTransmitData = appPlcTx.pDataTx;
            appPlcTx.pl360Tx.dataLength = 0;

            memset(appPlcTx.pl360Tx.preemphasis, 0, sizeof(appPlcTx.pl360Tx.preemphasis));
            
            /* Set PLC Multiband / Couling Branch flag */
            if (SRV_PCOUP_Get_Config(SRV_PLC_PCOUP_AUXILIARY_BRANCH) == SYS_STATUS_UNINITIALIZED) {
                /* Auxiliary branch is not configured. Single branch */
                appPlc.plcMultiband = false;
                appPlcTx.bin2InUse = false;
            } else {
                /* Dual branch */
                appPlc.plcMultiband = true;
                appPlcTx.couplingBranch = SRV_PCOUP_Get_Default_Branch();
                if (appPlcTx.couplingBranch == SRV_PLC_PCOUP_MAIN_BRANCH)
                {
                    appPlcTx.bin2InUse = false;
                }
                else
                {
                    appPlcTx.bin2InUse = true;
                }
            }

            /* Initialize PLC driver */
            appPlc.state = APP_PLC_STATE_INIT;
        }
        break;

        case APP_PLC_STATE_INIT:
        {
            /* Select PLC Binary file for multi-band solution */
            if (appPlc.plcMultiband)
            {
                if (appPlcTx.bin2InUse)
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

                /* Initialize PLC Driver Instance */
                sysObj.drvPlcPhy = DRV_PLC_PHY_Initialize(DRV_PLC_PHY_INDEX, (SYS_MODULE_INIT *)&drvPlcPhyInitData);
                /* Register Callback function to handle PLC interruption */
                PIO_PinInterruptCallbackRegister(DRV_PLC_EXT_INT_PIN, DRV_PLC_PHY_ExternalInterruptHandler, sysObj.drvPlcPhy);
            }
            
            /* Open PLC driver */
            appPlc.drvPl360Handle = DRV_PLC_PHY_Open(DRV_PLC_PHY_INDEX_0, NULL);

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
            if (DRV_PLC_PHY_Status(DRV_PLC_PHY_INDEX_0) == SYS_STATUS_READY)
            {

                /* Configure PLC callbacks */
                DRV_PLC_PHY_ExceptionCallbackRegister(appPlc.drvPl360Handle, APP_PLC_ExceptionCb, DRV_PLC_PHY_INDEX_0);
                DRV_PLC_PHY_DataCfmCallbackRegister(appPlc.drvPl360Handle, APP_PLC_DataCfmCb, DRV_PLC_PHY_INDEX_0);
                DRV_PLC_PHY_DataIndCallbackRegister(appPlc.drvPl360Handle, APP_PLC_DataIndCb, DRV_PLC_PHY_INDEX_0);
                DRV_PLC_PHY_SleepDisableCallbackRegister(appPlc.drvPl360Handle, APP_PLC_SleepModeDisableCb, DRV_PLC_PHY_INDEX_0);
                
                /* Apply PLC initial configuration */
                APP_PLC_SetInitialConfiguration();
                
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
            if (appPlc.waitingTxCfm == false)
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

            /* Clear Transmission flags */
            appPlc.waitingTxCfm = false;

            /* Close PLC Driver */
            DRV_PLC_PHY_Close(appPlc.drvPl360Handle);

            /* Restart PLC Driver */
            appPlc.state = APP_PLC_STATE_INIT;
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
  Function:
    bool APP_PLC_Initialize(void)

  Remarks:
    See prototype in app_plc.h.
 */
bool APP_PLC_SendData ( uint8_t* pData, uint16_t length )
{
    if (appPlc.state == APP_PLC_STATE_WAITING)
    {
            
        if ((length > 0) && (length <= APP_PLC_BUFFER_SIZE))
        {
            /* Fill 2 first bytes with data length */
            /* Physical Layer may add padding bytes in order to complete symbols with data */
            /* It is needed to include real data length in the message because otherwise at reception is not possible to know if there is padding or not */
            appPlcTx.pDataTx[0] = length >> 8;
            appPlcTx.pDataTx[1] = length & 0xFF;

            /* Set data length in Tx Parameters structure */
            /* It should be equal or less than Maximum Data Length (see _get_max_psdu_len) */
            /* Otherwise DRV_PLC_PHY_TX_RESULT_INV_LENGTH will be reported in Tx Confirm */
            appPlcTx.pl360Tx.dataLength = length + 2;
            memcpy(appPlcTx.pDataTx + 2, pData, length);
            
            appPlc.waitingTxCfm = true;

            DRV_PLC_PHY_Send(appPlc.drvPl360Handle, &appPlcTx.pl360Tx);

            /* Set PLC state */
            if (appPlc.waitingTxCfm)
            {
                appPlc.state = APP_PLC_STATE_WAITING_TX_CFM;
                return true;
            }
        }
    }
    
    return false;
}

void APP_PLC_SetModScheme ( DRV_PLC_PHY_MOD_TYPE modType, DRV_PLC_PHY_MOD_SCHEME modScheme )
{
    DRV_PLC_PHY_MAX_PSDU_LEN_PARAMS parameters;
    
    if (appPlc.state == APP_PLC_STATE_WAITING)
    {
        appPlcTx.pl360Tx.modScheme = modScheme;
        appPlcTx.pl360Tx.modType = modType;
        
        parameters.modScheme = modScheme;
        parameters.modType = modType;
        parameters.rs2Blocks = appPlcTx.pl360Tx.rs2Blocks;
        memset(parameters.toneMap, 0, TONE_MAP_SIZE_MAX);
        memcpy(parameters.toneMap, appPlcTx.pl360Tx.toneMap, appPlcTx.toneMapSize);

        /* Set parameters for MAX_PSDU_LEN computation in PLC device */
        appPlc.plcPIB.id = PLC_ID_MAX_PSDU_LEN_PARAMS;
        appPlc.plcPIB.length = sizeof(DRV_PLC_PHY_MAX_PSDU_LEN_PARAMS);
        memcpy(appPlc.plcPIB.pData, (uint8_t *)&parameters, appPlc.plcPIB.length); 
        DRV_PLC_PHY_PIBSet(appPlc.drvPl360Handle, &appPlc.plcPIB);

        /* Get MAX_PSDU_LEN from PL360 device */
        appPlc.plcPIB.id = PLC_ID_MAX_PSDU_LEN;
        appPlc.plcPIB.length = 2;
        memcpy(appPlc.plcPIB.pData, (uint8_t *)&appPlcTx.maxPsduLen, 2); 
        DRV_PLC_PHY_PIBGet(appPlc.drvPl360Handle, &appPlc.plcPIB);
    }
}

bool APP_PLC_SetSleepMode ( bool enable )
{
    bool sleepIsEnabled = (appPlc.state == APP_PLC_STATE_SLEEP);
    
    if (sleepIsEnabled != enable)
    {
        DRV_PLC_PHY_Sleep(appPlc.drvPl360Handle, enable);
        if (enable)
        {
            appPlc.state = APP_PLC_STATE_SLEEP;
        }
        
        return true;
    }
    
    return false;
}

/*******************************************************************************
 End of File
 */
