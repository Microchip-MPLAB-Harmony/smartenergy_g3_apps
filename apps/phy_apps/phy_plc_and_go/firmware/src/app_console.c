/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_console.c

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
#include <stdarg.h>
#include <math.h>
#include "definitions.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************
#define CTRL_S_KEY         0x13
#define BACKSPACE_KEY      0x08
#define DELETE_KEY         0x7F

/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_CONSOLE_DATA CACHE_ALIGN appConsole;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

static bool APP_CONSOLE_CheckIsPrintable(char data)
{
    if (((data >= 32) && (data <= 126)) ||
        ((data >= 128) && (data <= 254)) ||
         (data == '\t') || (data == '\n') || (data == '\r'))
    {
        return 1;
    }

    return 0;
}

static void APP_CONSOLE_ReadRestart( uint16_t numCharPending )
{
    appConsole.pNextChar = appConsole.pReceivedChar;
    appConsole.dataLength = 0;
    appConsole.numCharToReceive = numCharPending;
}

static uint8_t APP_CONSOLE_ReadSerialChar( void )
{
    if (appConsole.numCharToReceive > 0)
    {
        if (SYS_CONSOLE_Read(SYS_CONSOLE_INDEX_0, (void*)appConsole.pNextChar, 1) > 0)
        {
            /* Success */
            if ((*(appConsole.pNextChar) == '\r') || (*(appConsole.pNextChar) == '\n'))
            {
                appConsole.numCharToReceive = 0;
                return appConsole.dataLength;
            }
            
            if (*(appConsole.pNextChar) == CTRL_S_KEY)
            {
                *appConsole.pReceivedChar = CTRL_S_KEY;
                appConsole.numCharToReceive = 0;
                appConsole.dataLength = 1;
                return appConsole.dataLength;
            }
            
            if ((*(appConsole.pNextChar) == BACKSPACE_KEY) || (*(appConsole.pNextChar) == DELETE_KEY))
            {
                /* Remove character from data buffer */
                if (appConsole.dataLength > 0)
                {
                    APP_CONSOLE_Print("\b \b");
                    appConsole.dataLength--;
                    appConsole.pNextChar--;
                    return appConsole.dataLength;
                }
            }

            if (APP_CONSOLE_CheckIsPrintable(*appConsole.pNextChar))
            {
                if (appConsole.echoEnable)
                {
                    SYS_CONSOLE_Write(SYS_CONSOLE_INDEX_0, appConsole.pNextChar, 1);
                }

                appConsole.dataLength++;
                appConsole.pNextChar++;
                appConsole.numCharToReceive--;
            }
        }
    }
    
    return appConsole.dataLength;
}

static bool APP_CONSOLE_SetScheme(char *scheme)
{
    DRV_PLC_PHY_MOD_TYPE modType;
    DRV_PLC_PHY_MOD_SCHEME modScheme;
    bool result = true;
    uint8_t version;
    
    version = (uint8_t)(appPlcTx.pl360PhyVersion >> 16);

	switch (*scheme)
    {
		case '0':
			modType = MOD_TYPE_BPSK_ROBO;
			modScheme = MOD_SCHEME_DIFFERENTIAL;
            APP_CONSOLE_Print("\r\nTx Modulation: BPSK Robust Differential ");
			break;

		case '1':
			modType = MOD_TYPE_BPSK;
			modScheme = MOD_SCHEME_DIFFERENTIAL;
            APP_CONSOLE_Print("\r\nTx Modulation: BPSK Differential ");
			break;

		case '2':
			modType = MOD_TYPE_QPSK;
			modScheme = MOD_SCHEME_DIFFERENTIAL;
            APP_CONSOLE_Print("\r\nTx Modulation: QPSK Differential ");
			break;

		case '3':
			modType = MOD_TYPE_8PSK;
			modScheme = MOD_SCHEME_DIFFERENTIAL;
            if (version == 0x03)
            {
                APP_CONSOLE_Print("\r\nCoherent modulation not supported in ARIB band. Skipping configuration\r\n");
                result = false;
            }
            else
            {
                APP_CONSOLE_Print("\r\nTx Modulation: 8PSK Differential ");
            }
			break;

		case '4':
			modType = MOD_TYPE_BPSK_ROBO;
			modScheme = MOD_SCHEME_COHERENT;
            if (version == 0x03)
            {
                APP_CONSOLE_Print("\r\nCoherent modulation not supported in ARIB band. Skipping configuration\r\n");
                result = false;
            }
            else
            {
                APP_CONSOLE_Print("\r\nTx Modulation: BPSK Robust Coherent ");
            }
			break;

		case '5':
			modType = MOD_TYPE_BPSK;
			modScheme = MOD_SCHEME_COHERENT;
            if (version == 0x03)
            {
                APP_CONSOLE_Print("\r\nCoherent modulation not supported in ARIB band. Skipping configuration\r\n");
                result = false;
            }
            else
            {
                APP_CONSOLE_Print("\r\nTx Modulation: BPSK Coherent ");
            }
			break;

		case '6':
			modType = MOD_TYPE_QPSK;
			modScheme = MOD_SCHEME_COHERENT;
            if (version == 0x03)
            {
                APP_CONSOLE_Print("\r\nCoherent modulation not supported in ARIB band. Skipping configuration\r\n");
                result = false;
            }
            else
            {
                APP_CONSOLE_Print("\r\nTx Modulation: QPSK Coherent ");
            }
			break;

		case '7':
			modType = MOD_TYPE_8PSK;
			modScheme = MOD_SCHEME_COHERENT;
            if (version == 0x03)
            {
                APP_CONSOLE_Print("\r\nCoherent modulation not supported in ARIB band. Skipping configuration\r\n");
                result = false;
            }
            else
            {
                APP_CONSOLE_Print("\r\nTx Modulation: 8PSK Coherent ");
            }
			break;

		default:
			result = false;
	}
    
    if (result)
    {
        APP_PLC_SetModScheme(modType, modScheme );
    }

    return result;
}

static void APP_CONSOLE_ShowSetSchemeMenu( void )
{
    uint8_t schemeMenu = 0xFF;
    uint8_t index;
    
    APP_CONSOLE_Print("\r\n--- Tx Modulation Configuration Menu ---\r\n");
    APP_CONSOLE_Print("Select Modulation:\r\n");
    
    if (appPlcTx.pl360Tx.modScheme == MOD_SCHEME_DIFFERENTIAL)
    {
        if (appPlcTx.pl360Tx.modType == MOD_TYPE_BPSK_ROBO)
        {
            schemeMenu = 0;
        }
        else if (appPlcTx.pl360Tx.modType == MOD_TYPE_BPSK)
        {
            schemeMenu = 1;
        }
        else if (appPlcTx.pl360Tx.modType == MOD_TYPE_QPSK)
        {
            schemeMenu = 2;
        }
        else if (appPlcTx.pl360Tx.modType == MOD_TYPE_8PSK)
        {
            schemeMenu = 3;
        }
    }
    else
    {   /* MOD_SCHEME_COHERENT */
        if (appPlcTx.pl360Tx.modType == MOD_TYPE_BPSK_ROBO)
        {
            schemeMenu = 4;
        }
        else if (appPlcTx.pl360Tx.modType == MOD_TYPE_BPSK)
        {
            schemeMenu = 5;
        }
        else if (appPlcTx.pl360Tx.modType == MOD_TYPE_QPSK)
        {
            schemeMenu = 6;
        }
        else if (appPlcTx.pl360Tx.modType == MOD_TYPE_8PSK)
        {
            schemeMenu = 7;
        }
    }
    
    for (index = 0; index < 8; index++)
    {
        if (index == schemeMenu)
        {
            APP_CONSOLE_Print("->\t");
        }
        else
        {
            APP_CONSOLE_Print("\t");
        }
        
        switch (index)
        {
            case 0:
                APP_CONSOLE_Print("0: BPSK Robust Differential ... 5.5 kbit/s\r\n");
                break;

            case 1:
                APP_CONSOLE_Print("1: BPSK Differential .......... 20.1 kbit/s\r\n");
                break;

            case 2:
                APP_CONSOLE_Print("2: QPSK Differential .......... 34.5 kbit/s\r\n");
                break;

            case 3:
                APP_CONSOLE_Print("3: 8PSK Differential .......... 44.6 kbit/s\r\n");
                break;

            case 4:
                APP_CONSOLE_Print("4: BPSK Robust Coherent ....... 5 kbit/s\r\n");
                break;

            case 5:
                APP_CONSOLE_Print("5: BPSK Coherent .............. 18.5 kbit/s\r\n");
                break;

            case 6:
                APP_CONSOLE_Print("6: QPSK Coherent .............. 31.3 kbit/s\r\n");
                break;

            case 7:
                APP_CONSOLE_Print("7: 8PSK Coherent .............. 41.2 kbit/s\r\n");
                break;
        }
    }
    APP_CONSOLE_Print(MENU_CMD_PROMPT);
}

static void APP_CONSOLE_ShowSetSleepMenu( void )
{
    APP_CONSOLE_Print("\r\n--- Enable/Disable Sleep Configuration Menu ---\r\n");
    APP_CONSOLE_Print("Select Sleep Mode:\r\n");
    
    if (appPlc.state != APP_PLC_STATE_SLEEP)
    {
        APP_CONSOLE_Print("->");
    }
    APP_CONSOLE_Print("\t0: Sleep mode off\r\n");

    if (appPlc.state == APP_PLC_STATE_SLEEP)
    {
        APP_CONSOLE_Print("->");
    }
    APP_CONSOLE_Print("\t1: Sleep mode on\r\n");
    APP_CONSOLE_Print(MENU_CMD_PROMPT);
}

static void APP_CONSOLE_ShowMultibandMenu( void )
{
    SRV_PLC_PCOUP_BRANCH currentBranch;
    SRV_PLC_PCOUP_BRANCH index;
    uint8_t band;
    
    APP_CONSOLE_Print("\r\n--- Tx/Rx Coupling Band Configuration Menu ---\r\n");
    APP_CONSOLE_Print("Select PLC Coupling branch:\r\n");
    
    currentBranch = appPlcTx.couplingBranch;
    
    for (index = 0; index < 2; index++)
    {
        if (index == currentBranch)
        {
            APP_CONSOLE_Print("->\t");
        }
        else
        {
            APP_CONSOLE_Print("\t");
        }
        
        if (index == SRV_PLC_PCOUP_MAIN_BRANCH)
        {
            APP_CONSOLE_Print("0: Main Branch ");
        }
        else
        {
            APP_CONSOLE_Print("1: Auxiliary ");
        }
        
        band = SRV_PCOUP_Get_Phy_Band(index);
        switch (band)
        {
            case G3_CEN_A:
                APP_CONSOLE_Print("(CENELEC-A band: 35 - 91 KHz)\r\n");
                break;
                
            case G3_CEN_B:
                APP_CONSOLE_Print("(CENELEC-B band: 98 - 122 kHz)\r\n");
                break;
                
            case G3_FCC:
                APP_CONSOLE_Print("(FCC band: 154 - 488 KHz)\r\n");
                break;
                
            case G3_ARIB:
                APP_CONSOLE_Print("(ARIB band: 154 - 404 KHz)\r\n");
                break;
                
        }
    }
        
    APP_CONSOLE_Print(MENU_CMD_PROMPT);
}

static bool APP_CONSOLE_SetPlcBand(char *mode)
{
    bool result = false;

	switch (*mode)
    {
		case '0':
            if (appPlcTx.couplingBranch == SRV_PLC_PCOUP_AUXILIARY_BRANCH)
            {
                appPlcTx.couplingBranch = SRV_PLC_PCOUP_MAIN_BRANCH;
                appPlcTx.bin2InUse = 0;
                result = true;
            }
			break;

		case '1':
            if (appPlcTx.couplingBranch == SRV_PLC_PCOUP_MAIN_BRANCH)
            {
                appPlcTx.couplingBranch = SRV_PLC_PCOUP_AUXILIARY_BRANCH;
                appPlcTx.bin2InUse = 1;
                result = true;
            }
			break;
	}

    return result;
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_CONSOLE_Initialize ( void )

  Remarks:
    See prototype in app_console.h.
 */

void APP_CONSOLE_Initialize ( void )
{
    /* Update state machine */
    appConsole.state = APP_CONSOLE_STATE_INIT;

    /* Init Reception data */
    appConsole.pNextChar = appConsole.pReceivedChar;
    appConsole.dataLength = 0;
    appConsole.numCharToReceive = 0;
    
    /* Set ECHO ON by default */
    appConsole.echoEnable = true;

}

/******************************************************************************
  Function:
    void APP_CONSOLE_Tasks ( void )

  Remarks:
    See prototype in app_console.h.
 */

void APP_CONSOLE_Tasks ( void )
{
    /* Refresh WDG */
    WDT_Clear();
    
    /* Read console port */
    APP_CONSOLE_ReadSerialChar();

    /* Check the application's current state. */
    switch ( appConsole.state )
    {
        /* Application's initial state. */
        case APP_CONSOLE_STATE_INIT:
        {
            char data;
            
            /* Wait for any serial data to start Console application */
            if (SYS_CONSOLE_Read(SYS_CONSOLE_INDEX_0, &data, 1) > 0)
            {
                appConsole.state = APP_CONSOLE_STATE_WAIT_PLC;
            }
            break;
        }

        case APP_CONSOLE_STATE_WAIT_PLC:
        {
            /* Wait for PLC transceiver initialization */
            if (appPlc.state == APP_PLC_STATE_WAITING)
            {
                /* Show App Header */
                APP_CONSOLE_Print(STRING_HEADER);
            
                /* Show PHY version */
                APP_CONSOLE_Print("PL360 binary loaded correctly\r\nPHY version: %02x.%02x.%02x.%02x", 
                        (uint8_t)(appPlcTx.pl360PhyVersion >> 24), (uint8_t)(appPlcTx.pl360PhyVersion >> 16),
                        (uint8_t)(appPlcTx.pl360PhyVersion >> 8), (uint8_t)(appPlcTx.pl360PhyVersion));
                
                /* Set PLC Phy Tone Map Size */
                if (((appPlcTx.pl360PhyVersion >> 16) & 0xFF) == 0x01)
                {
                    /* Show PHY Band */
                    APP_CONSOLE_Print("(CENELEC-A band: 35 - 91 kHz)\r\n");
                }
                else if (((appPlcTx.pl360PhyVersion >> 16) & 0xFF) == 0x02)
                {
                    /* Show PHY Band */
                    APP_CONSOLE_Print("(FCC band: 154 - 488 kHz)\r\n");
                }
                else if (((appPlcTx.pl360PhyVersion >> 16) & 0xFF) == 0x03)
                {
                    /* Show PHY Band */
                    APP_CONSOLE_Print("(ARIB band: 154 - 404 kHz)\r\n");
                }
                else if (((appPlcTx.pl360PhyVersion >> 16) & 0xFF) == 0x04)
                {
                    /* Show PHY Band */
                    APP_CONSOLE_Print("(CENELEC-B band: 98 - 122 kHz)\r\n");
                }
                
                APP_CONSOLE_Print("\r\nPress 'CTRL+S' to enter configuration menu. " \
                    "Enter text and press 'ENTER' to trigger transmission\r\n>>> ");
                
                /* Set Console state */
                appConsole.state = APP_CONSOLE_STATE_SHOW_PROMPT;
            }
            break;
        }

        case APP_CONSOLE_STATE_SHOW_PROMPT:
        {
            /* Show console interface */
            APP_CONSOLE_Print(MENU_CMD_PROMPT);

            /* Waiting Console command */
            APP_CONSOLE_ReadRestart(SERIAL_BUFFER_SIZE);
            appConsole.state = APP_CONSOLE_STATE_CONSOLE;
            

            break;
        }

        case APP_CONSOLE_STATE_CONSOLE:
        {
            if (appConsole.numCharToReceive == 0)
            {
                switch(*appConsole.pReceivedChar)
                {
                    case CTRL_S_KEY:
                        appConsole.state = APP_CONSOLE_STATE_MENU;
                        APP_CONSOLE_ReadRestart(1);
                        APP_CONSOLE_Print("\n\r-- Configuration Menu --------------\r\n");
                        APP_CONSOLE_Print("Select parameter to configure:\r\n");
                        APP_CONSOLE_Print("\t0: Enable/Disable sleep mode\n\r");
                        APP_CONSOLE_Print("\t1: Tx Modulation\n\r");
                        if (appPlc.plcMultiband)
                        {
                            APP_CONSOLE_Print("\t2: Tx/Rx Coupling Band\n\r");
                        }
                        break;
                        
                    default:
                        if (appConsole.dataLength)
                        {
                            /* Transmit PLC message */
                            if (APP_PLC_SendData((uint8_t *)appConsole.pReceivedChar, appConsole.dataLength))
                            {
                                appConsole.state = APP_CONSOLE_STATE_WAIT_PLC_TX_CFM;
                            }
                            else
                            {
                                if (appPlc.pvddMonTxEnable == false)
                                {
                                    APP_CONSOLE_Print("\n\rTransmission is not available. PVDD Monitor out of thresholds.\r\n");
                                }
                                else
                                {
                                    APP_CONSOLE_Print("\n\rTransmission is not available in Sleep Mode\r\n");
                                }
                                appConsole.state = APP_CONSOLE_STATE_SHOW_PROMPT;
                            }
                        }
                        else
                        {
                            appConsole.state = APP_CONSOLE_STATE_SHOW_PROMPT;
                        }
                }
            }
        }
        break;
        
        case APP_CONSOLE_STATE_MENU:
        {
            if (appConsole.numCharToReceive == 0)
            {
                if (appConsole.pReceivedChar[0] == '0')
                {
                    appConsole.state = APP_CONSOLE_STATE_SET_SLEEP;
                    APP_CONSOLE_ShowSetSleepMenu();
                    APP_CONSOLE_ReadRestart(1);
                }
                else if (appConsole.pReceivedChar[0] == '1')
                {
                    appConsole.state = APP_CONSOLE_STATE_SET_SCHEME;
                    APP_CONSOLE_ShowSetSchemeMenu();
                    APP_CONSOLE_ReadRestart(1);
                }
                else if (appPlc.plcMultiband && (appConsole.pReceivedChar[0] == '2'))
                {
                    appConsole.state = APP_CONSOLE_STATE_SET_PLC_BAND;
                    APP_CONSOLE_ShowMultibandMenu();
                    APP_CONSOLE_ReadRestart(1);
                }
                else
                {
                    /* Try it again */
                    APP_CONSOLE_Print("\r\nERROR: Menu option is not permitted. Try again : ");
                    APP_CONSOLE_ReadRestart(1);
                }
            }
        }
        break;
            

        case APP_CONSOLE_STATE_WAIT_PLC_TX_CFM:
        {
            if (appPlc.plcTxState == APP_PLC_TX_STATE_WAIT_TX_CFM)
            {
                APP_CONSOLE_Print("\r\nTx (%u bytes): ", appConsole.dataLength);
                switch(appPlc.lastTxResult)
                {
                    case DRV_PLC_PHY_TX_RESULT_PROCESS:
                        APP_CONSOLE_Print("  TX_RESULT_PROCESS\r\n");
                        break;
                    case DRV_PLC_PHY_TX_RESULT_SUCCESS:
                        APP_CONSOLE_Print("  TX_RESULT_SUCCESS\r\n");
                        break;
                    case DRV_PLC_PHY_TX_RESULT_INV_LENGTH:
                        APP_CONSOLE_Print("  TX_RESULT_INV_LENGTH\r\n");
                        break;
                    case DRV_PLC_PHY_TX_RESULT_BUSY_CH:
                        APP_CONSOLE_Print("  TX_RESULT_BUSY_CH\r\n");
                        break;
                    case DRV_PLC_PHY_TX_RESULT_BUSY_TX:
                        APP_CONSOLE_Print("  TX_RESULT_BUSY_TX\r\n");
                        break;
                    case DRV_PLC_PHY_TX_RESULT_BUSY_RX:
                        APP_CONSOLE_Print("  TX_RESULT_BUSY_RX\r\n");
                        break;
                    case DRV_PLC_PHY_TX_RESULT_INV_SCHEME:
                        APP_CONSOLE_Print("  TX_RESULT_INV_SCHEME\r\n");
                        break;
                    case DRV_PLC_PHY_TX_RESULT_TIMEOUT:
                        APP_CONSOLE_Print("  TX_RESULT_TIMEOUT\r\n");
                        break;
                    case DRV_PLC_PHY_TX_RESULT_INV_TONEMAP:
                        APP_CONSOLE_Print("  TX_RESULT_INV_TONEMAP\r\n");
                        break;
                    case DRV_PLC_PHY_TX_RESULT_INV_MODTYPE:
                        APP_CONSOLE_Print("  TX_RESULT_INV_MODTYPE\r\n");
                        break;
                    case DRV_PLC_PHY_TX_RESULT_INV_DT:
                        APP_CONSOLE_Print("  TX_RESULT_INV_DT\r\n");
                        break;
                    case DRV_PLC_PHY_TX_CANCELLED:
                        APP_CONSOLE_Print("  DRV_PLC_PHY_TX_CANCELLED\r\n");
                        break;
                    case DRV_PLC_PHY_TX_RESULT_HIGH_TEMP_120:
                        APP_CONSOLE_Print("...DRV_PLC_PHY_TX_RESULT_HIGH_TEMP_120\r\n");
                        break;
                    case DRV_PLC_PHY_TX_RESULT_HIGH_TEMP_110:
                        APP_CONSOLE_Print("...DRV_PLC_PHY_TX_RESULT_HIGH_TEMP_110\r\n");
                        break;
                    case DRV_PLC_PHY_TX_RESULT_NO_TX:
                        APP_CONSOLE_Print("  TX_RESULT_NO_TX\r\n");
                        break;
                }
                
                appConsole.state = APP_CONSOLE_STATE_SHOW_PROMPT;
            }
        }
        break;

        case APP_CONSOLE_STATE_SET_SLEEP:
        {
            if (appConsole.numCharToReceive == 0)
            {
                if (appConsole.pReceivedChar[0] == '1')
                {
                    if (APP_PLC_SetSleepMode(true))
                    {
                        APP_CONSOLE_Print("\r\nPLC Sleep mode enabled\r\n");
                    }
                    else
                    {
                        APP_CONSOLE_Print("\r\nPLC Sleep mode is already enabled\r\n");
                    }
                }
                else
                {
                    if (APP_PLC_SetSleepMode(false))
                    {
                        APP_CONSOLE_Print("\r\nPLC Sleep mode disabled\r\n");
                    }
                    else
                    {
                        APP_CONSOLE_Print("\r\nPLC Sleep mode is already disabled\r\n");
                    }
                }
                appConsole.state = APP_CONSOLE_STATE_SHOW_PROMPT;
            }
            break;
        }

        case APP_CONSOLE_STATE_SET_SCHEME:
        {
            if (appConsole.numCharToReceive == 0)
            {
                if (APP_CONSOLE_SetScheme(appConsole.pReceivedChar))
                {
                    APP_CONSOLE_Print("(Max data length = %u bytes)\r\n", (unsigned int)appPlcTx.maxPsduLen);
                    appConsole.state = APP_CONSOLE_STATE_SHOW_PROMPT;
                }
                else
                {
                    /* Try it again */
                    APP_CONSOLE_Print("\r\nERROR: Scheme not permitted. Try again : ");
                    APP_CONSOLE_ReadRestart(1);
                }
            }
            break;
        }
        
        case APP_CONSOLE_STATE_SET_PLC_BAND:
        {
            if (appConsole.numCharToReceive == 0)
            {
                if (APP_CONSOLE_SetPlcBand(appConsole.pReceivedChar))
                {
                    if (appPlcTx.bin2InUse)
                    {
                        APP_CONSOLE_Print("\r\nSet Auxiliary branch\r\n");
                    }
                    else
                    {
                        APP_CONSOLE_Print("\r\nSet Main branch\r\n");
                    }

                    appPlc.state = APP_PLC_STATE_SET_BAND;
                    appConsole.state = APP_CONSOLE_STATE_SHOW_PROMPT;
                }
                else
                {
                    APP_CONSOLE_Print("\r\nPLC Band has not changed\r\n");
                    appConsole.state = APP_CONSOLE_STATE_SHOW_PROMPT;
                }
            }
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

void APP_CONSOLE_Print(const char *format, ...)
{
    size_t len = 0;
    va_list args = {0};
    uint32_t numRetries = 100;
    
    if (appConsole.state == APP_CONSOLE_STATE_INIT)
    {
        return;
    }

    while(SYS_CONSOLE_WriteCountGet(SYS_CONSOLE_INDEX_0))
    {
        if (numRetries--)
        {
            /* Maintain Console service */
            SYS_CONSOLE_Tasks(SYS_CONSOLE_INDEX_0);
        }
        else
        {
            return;
        }
    }

    va_start( args, format );
    len = vsnprintf(appConsole.pTrasmitChar, SERIAL_BUFFER_SIZE - 1, format, args);
    va_end( args );

    if (len > 0 && len < SERIAL_BUFFER_SIZE - 1)
    {
        appConsole.pTrasmitChar[len] = '\0';

        SYS_CONSOLE_Write(SYS_CONSOLE_INDEX_0, appConsole.pTrasmitChar, len);
    }
}


/*******************************************************************************
 End of File
 */
