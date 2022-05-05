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
#define CTRL_D_KEY         0x04
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

APP_CONSOLE_DATA appConsole;

static CACHE_ALIGN char pTransmitBuffer[CACHE_ALIGNED_SIZE_GET(SERIAL_BUFFER_SIZE)];
static CACHE_ALIGN char pReceivedBuffer[CACHE_ALIGNED_SIZE_GET(SERIAL_BUFFER_SIZE)];

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

static bool APP_CONSOLE_CharToHex(char value, uint8_t *hex)
{
    if ((value >= '0') && (value <= '9'))
    {
        *hex = value - 0x30;
    }
    else if ((value >= 'A') && (value <= 'F'))
    {
        *hex = value - 0x37;
    }
    else if ((value >= 'a') && (value <= 'f'))
    {
        *hex = value - 0x57;
    }
    else
    {
        return 0;
    }

    return 1;
}

static bool APP_CONSOLE_GetValue16(char *pData, uint16_t *pValue16)
{
    uint16_t value;
    uint8_t hexValue;
    uint8_t index;
    
    value = 0;
    for (index = 0; index < 4; index++)
    {
        if (APP_CONSOLE_CharToHex(*pData++, &hexValue))
        {
            value <<= 4;
            value += hexValue;
        }
        else
        {
            return false;
        }
    }
    
    *pValue16 = value;
    return true;
    
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
            
            if (*(appConsole.pNextChar) == CTRL_D_KEY)
            {
                *appConsole.pReceivedChar = CTRL_D_KEY;
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
    
    currentBranch = appPlc.couplingBranch;
    
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

static void APP_CONSOLE_ShowSetSourceAddressMenu( void )
{
    APP_CONSOLE_Print("\r\n--- G3 MAC Source Address Configuration Menu ---\r\n");
    APP_CONSOLE_Print("Introduce New Address (0x%04X) : 0x", 
            appPlcTx.txHeader.sourceAddress.shortAddress);
}

static bool APP_CONSOLE_SetSourceAddress(char *pData)
{
    uint16_t address;
    
    if (appConsole.dataLength != 4)
    {
        return false;
    }
    
    if (APP_CONSOLE_GetValue16(pData, &address) == false)
    {
        return false;
    }
    
    if (address == MAC_RT_SHORT_ADDRESS_BROADCAST)
    {
        /* Broadcast address is not permitted as Source address */
        return false;
    }
    
    if (address == appPlcTx.txHeader.destinationAddress.shortAddress)
    {
        /* Source and Destination addresses must be different */
        return false;
    }
    
    APP_PLC_SetSourceAddress(address);
    
    return true;
}

static void APP_CONSOLE_ShowSetDestinationAddressMenu( void )
{
    APP_CONSOLE_Print("\r\n--- G3 MAC Source Destiantion Configuration Menu ---\r\n");
    APP_CONSOLE_Print("Introduce New Address (0x%04X) : 0x", 
            appPlcTx.txHeader.destinationAddress.shortAddress);
}

static bool APP_CONSOLE_SetDestinationAddress(char *pData)
{
    uint16_t address;
    
    if (appConsole.dataLength != 4)
    {
        return false;
    }
    
    if (APP_CONSOLE_GetValue16(pData, &address) == false)
    {
        return false;
    }
    
    if ((address == MAC_RT_SHORT_ADDRESS_BROADCAST) && (appPlcTx.txHeader.frameControl.ackRequest))
    {
        /* Broadcast address is not permitted if ACK request is enabled */
        return false;
    }
    
    if (address == appPlcTx.txHeader.sourceAddress.shortAddress)
    {
        /* Source and Destination addresses must be different */
        return false;
    }
    
    APP_PLC_SetDestinationAddress(address);
    
    return true;
}

static void APP_CONSOLE_ShowSetACKMenu( void )
{
    APP_CONSOLE_Print("\r\n--- G3 MAC ACK request Configuration Menu ---\r\n");
    APP_CONSOLE_Print("Enable automatic ACK request [Y/N] : ");
}

static bool APP_CONSOLE_SetACKRequest( char *enable )
{
    bool result;

	switch (*enable)
    {
		case 'Y':
        case 'y':
            appPlcTx.txHeader.frameControl.ackRequest = true;
            result = true;
			break;

		case 'N':
        case 'n':
            appPlcTx.txHeader.frameControl.ackRequest = false;
            result = true;
			break;
           
        default:
            result = false;
	}

    return result;
}

static bool APP_CONSOLE_SetPlcBand( char *mode )
{
    bool result;

	switch (*mode)
    {
		case '0':
            if (appPlc.couplingBranch == SRV_PLC_PCOUP_AUXILIARY_BRANCH)
            {
                appPlc.couplingBranch = SRV_PLC_PCOUP_MAIN_BRANCH;
                appPlc.bin2InUse = 0;
                result = true;
            }
			break;

		case '1':
            if (appPlc.couplingBranch == SRV_PLC_PCOUP_MAIN_BRANCH)
            {
                appPlc.couplingBranch = SRV_PLC_PCOUP_AUXILIARY_BRANCH;
                appPlc.bin2InUse = 1;
                result = true;
            }
			break;
           
        default:
            result = false;
	}

    return result;
}

static void APP_CONSOLE_ShowPhyBand( uint32_t phyVersion )
{
    APP_CONSOLE_Print("G3 Tx/Rx Band: ");
    if (((phyVersion >> 16) & 0xFF) == 0x01)
    {
        /* Show PHY Band */
        APP_CONSOLE_Print("CENELEC-A band (35 - 91 kHz)\r\n");
    }
    else if (((phyVersion >> 16) & 0xFF) == 0x02)
    {
        /* Show PHY Band */
        APP_CONSOLE_Print("FCC band (154 - 488 kHz)\r\n");
    }
    else if (((phyVersion >> 16) & 0xFF) == 0x03)
    {
        /* Show PHY Band */
        APP_CONSOLE_Print("ARIB band (154 - 404 kHz)\r\n");
    }
    else if (((phyVersion >> 16) & 0xFF) == 0x04)
    {
        /* Show PHY Band */
        APP_CONSOLE_Print("CENELEC-B band (98 - 122 kHz)\r\n");
    }
    else
    {
        /* ERROR in PHY Band */
        APP_CONSOLE_Print("Find ERROR in PHY band\r\n");
    }
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
    appConsole.pTransmitChar = pTransmitBuffer;
    appConsole.pReceivedChar = pReceivedBuffer;
    appConsole.pNextChar = pReceivedBuffer;
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
                        (uint8_t)(appPlc.phyVersion >> 24), (uint8_t)(appPlc.phyVersion >> 16),
                        (uint8_t)(appPlc.phyVersion >> 8), (uint8_t)(appPlc.phyVersion));
                
                /* Set PLC Phy Tone Map Size */
                if (((appPlc.phyVersion >> 16) & 0xFF) == 0x01)
                {
                    /* Show PHY Band */
                    APP_CONSOLE_Print("(CENELEC-A band: 35 - 91 kHz)\r\n");
                }
                else if (((appPlc.phyVersion >> 16) & 0xFF) == 0x02)
                {
                    /* Show PHY Band */
                    APP_CONSOLE_Print("(FCC band: 154 - 488 kHz)\r\n");
                }
                else if (((appPlc.phyVersion >> 16) & 0xFF) == 0x03)
                {
                    /* Show PHY Band */
                    APP_CONSOLE_Print("(ARIB band: 154 - 404 kHz)\r\n");
                }
                else if (((appPlc.phyVersion >> 16) & 0xFF) == 0x04)
                {
                    /* Show PHY Band */
                    APP_CONSOLE_Print("(CENELEC-B band: 98 - 122 kHz)\r\n");
                }
                else
                {
                    /* Show PHY Band */
                    APP_CONSOLE_Print(" (Find ERROR in PHY version.)\r\n");
                }
                
                /* Show G3 MAC RT Configuration */
                APP_CONSOLE_Print("Configuring G3 MAC RT Header\r\n" \
                    "G3 MAC PAN ID: 0x%04X\r\n" \
                    "G3 MAC Source Address: 0x%04X\r\n" \
                    "G3 MAC Destination Address: 0x%04X\r\n" \
                    "G3 MAC ACK request: %u\r\n", 
                    appPlcTx.txHeader.destinationPAN, 
                    appPlcTx.txHeader.sourceAddress.shortAddress, 
                    appPlcTx.txHeader.destinationAddress.shortAddress,
                    appPlcTx.txHeader.frameControl.ackRequest);
                
                /* Show configuration options */
                APP_CONSOLE_Print("\r\nPress 'CTRL+D' to show the current configuration." \
                    "\r\nPress 'CTRL+S' to enter configuration menu." \
                    "\r\nEnter text and press 'ENTER' to trigger transmission.");
                
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
                    case CTRL_D_KEY:
                        /* Show G3 MAC RT Configuration */
                        APP_CONSOLE_Print("\n\r--- Configuration Parameters---------\r\n" \
                            "G3 MAC PAN ID: 0x%04X\r\n" \
                            "G3 MAC Source Address: 0x%04X\r\n" \
                            "G3 MAC Destination Address: 0x%04X\r\n" \
                            "G3 MAC ACK request: %u\r\n", 
                            appPlcTx.txHeader.destinationPAN, 
                            appPlcTx.txHeader.sourceAddress.shortAddress, 
                            appPlcTx.txHeader.destinationAddress.shortAddress,
                            appPlcTx.txHeader.frameControl.ackRequest);
                        
                        APP_CONSOLE_ShowPhyBand(appPlc.phyVersion);
                        appConsole.state = APP_CONSOLE_STATE_SHOW_PROMPT;
                        break;
                        
                    case CTRL_S_KEY:
                        appConsole.state = APP_CONSOLE_STATE_MENU;
                        APP_CONSOLE_ReadRestart(1);
                        APP_CONSOLE_Print("\n\r--- Configuration Menu --------------\r\n");
                        APP_CONSOLE_Print("Select parameter to configure:\r\n");
                        APP_CONSOLE_Print("\t0: Enable/Disable sleep mode\n\r");
                        APP_CONSOLE_Print("\t1: G3 MAC Source Address\n\r");
                        APP_CONSOLE_Print("\t2: G3 MAC Destination Address\n\r");
                        APP_CONSOLE_Print("\t3: G3 MAC ACK request\n\r");
                        if (appPlc.plcMultiband)
                        {
                            APP_CONSOLE_Print("\t4: Tx/Rx Band\n\r");
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
                    appConsole.state = APP_CONSOLE_STATE_SET_SRC_ADDR;
                    APP_CONSOLE_ShowSetSourceAddressMenu();
                    APP_CONSOLE_ReadRestart(4);
                }
                else if (appConsole.pReceivedChar[0] == '2')
                {
                    appConsole.state = APP_CONSOLE_STATE_SET_DST_ADDR;
                    APP_CONSOLE_ShowSetDestinationAddressMenu();
                    APP_CONSOLE_ReadRestart(4);
                }
                else if (appConsole.pReceivedChar[0] == '3')
                {
                    appConsole.state = APP_CONSOLE_STATE_SET_ACK;
                    APP_CONSOLE_ShowSetACKMenu();
                    APP_CONSOLE_ReadRestart(1);
                }
                else if (appPlc.plcMultiband && (appConsole.pReceivedChar[0] == '4'))
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
            if (appPlc.plcTxState == APP_PLC_TX_STATE_IDLE)
            {
                APP_CONSOLE_Print("\r\nTx (%u bytes): ", appConsole.dataLength);
                switch(appPlcTx.lastTxStatus)
                {
                    case MAC_RT_STATUS_SUCCESS:
                        APP_CONSOLE_Print("  MAC_RT_STATUS_SUCCESS\r\n");
                        break;
                    case MAC_RT_STATUS_CHANNEL_ACCESS_FAILURE:
                        APP_CONSOLE_Print("  MAC_RT_STATUS_CHANNEL_ACCESS_FAILURE\r\n");
                        break;
                    case MAC_RT_STATUS_DENIED:
                        APP_CONSOLE_Print("  MAC_RT_STATUS_DENIED\r\n");
                        break;
                    case MAC_RT_STATUS_INVALID_INDEX:
                        APP_CONSOLE_Print("  MAC_RT_STATUS_INVALID_INDEX\r\n");
                        break;
                    case MAC_RT_STATUS_INVALID_PARAMETER:
                        APP_CONSOLE_Print("  MAC_RT_STATUS_INVALID_PARAMETER\r\n");
                        break;
                    case MAC_RT_STATUS_NO_ACK:
                        APP_CONSOLE_Print("  MAC_RT_STATUS_NO_ACK\r\n");
                        break;
                    case MAC_RT_STATUS_READ_ONLY:
                        APP_CONSOLE_Print("  MAC_RT_STATUS_READ_ONLY\r\n");
                        break;
                    case MAC_RT_STATUS_TRANSACTION_OVERFLOW:
                        APP_CONSOLE_Print("  MAC_RT_STATUS_TRANSACTION_OVERFLOW\r\n");
                        break;
                    case MAC_RT_STATUS_UNSUPPORTED_ATTRIBUTE:
                        APP_CONSOLE_Print("  MAC_RT_STATUS_UNSUPPORTED_ATTRIBUTE\r\n");
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

        case APP_CONSOLE_STATE_SET_SRC_ADDR:
        {
            if (appConsole.numCharToReceive == 0)
            {
                if (APP_CONSOLE_SetSourceAddress(appConsole.pReceivedChar))
                {
                    APP_CONSOLE_Print("\r\nG3 MAC Source Address: 0x%04X\r\n", 
                            (unsigned int)appPlcTx.txHeader.sourceAddress.shortAddress);
                    appConsole.state = APP_CONSOLE_STATE_SHOW_PROMPT;
                }
                else
                {
                    /* Try it again */
                    APP_CONSOLE_Print("\r\nERROR: Address not permitted. Try again : 0x");
                    APP_CONSOLE_ReadRestart(4);
                }
            }
            break;
        }

        case APP_CONSOLE_STATE_SET_DST_ADDR:
        {
            if (appConsole.numCharToReceive == 0)
            {
                if (APP_CONSOLE_SetDestinationAddress(appConsole.pReceivedChar))
                {
                    APP_CONSOLE_Print("\r\nG3 MAC Destination Address: 0x%04X\r\n", 
                            (unsigned int)appPlcTx.txHeader.destinationAddress.shortAddress);
                    appConsole.state = APP_CONSOLE_STATE_SHOW_PROMPT;
                }
                else
                {
                    /* Try it again */
                    APP_CONSOLE_Print("\r\nERROR: Address not permitted. Try again : 0x");
                    APP_CONSOLE_ReadRestart(4);
                }
            }
            break;
        }

        case APP_CONSOLE_STATE_SET_ACK:
        {
            if (appConsole.numCharToReceive == 0)
            {
                if (APP_CONSOLE_SetACKRequest(appConsole.pReceivedChar))
                {
                    if (appPlcTx.txHeader.frameControl.ackRequest)
                    {
                        APP_CONSOLE_Print("\r\nACK Request has been enabled\r\n");
                    }
                    else
                    {
                        APP_CONSOLE_Print("\r\nACK Request has not been enabled." \
                                " Check the destination address. Broadcast address is not permitted. \r\n");
                    }
                    appConsole.state = APP_CONSOLE_STATE_SHOW_PROMPT;
                }
                else
                {
                    /* Try it again */
                    APP_CONSOLE_Print("\r\nERROR: Value not permitted. Try again : ");
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
                    if (appPlc.bin2InUse)
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
    uint32_t numRetries = 1000;
    
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
    len = vsnprintf(appConsole.pTransmitChar, SERIAL_BUFFER_SIZE - 1, format, args);
    va_end( args );
    
    if (len > SERIAL_BUFFER_SIZE - 1)
    {
        len = SERIAL_BUFFER_SIZE - 1;
    }
    
    appConsole.pTransmitChar[len] = '\0';
    SYS_CONSOLE_Message(SYS_CONSOLE_INDEX_0, (const char *) appConsole.pTransmitChar);
}


/*******************************************************************************
 End of File
 */
