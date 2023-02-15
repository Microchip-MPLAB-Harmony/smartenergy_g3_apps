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


static va_list sArgs = {0};

/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

CACHE_ALIGN APP_CONSOLE_DATA appConsole;

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
         (data == '\t') || (data == '\n'))
    {
        return 1;
    }

    return 0;
}

static void APP_CONSOLE_ReadRestart( uint8_t numCharPending )
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

static bool APP_CONSOLE_SetAttenuationLevel(char *level)
{
	uint8_t attLevel;
    uint8_t attLevelHex;

    if (APP_CONSOLE_CharToHex(*level++, &attLevelHex))
    {
        attLevel = attLevelHex << 4;

        attLevelHex = *level;
        if (APP_CONSOLE_CharToHex(*level, &attLevelHex))
        {
            attLevel += attLevelHex;

            if ((attLevel <= 0x1F) || (attLevel == 0xFF)) {
                appPlcTx.pl360Tx.attenuation = attLevel;
                return true;
            }
        }
    }

    return false;
}

static bool APP_CONSOLE_SetScheme(char *scheme)
{
    bool result = true;

	switch (*scheme)
    {
		case '0':
			appPlcTx.pl360Tx.modType = MOD_TYPE_BPSK;
			appPlcTx.pl360Tx.modScheme = MOD_SCHEME_DIFFERENTIAL;
			break;

		case '1':
			appPlcTx.pl360Tx.modType = MOD_TYPE_QPSK;
			appPlcTx.pl360Tx.modScheme = MOD_SCHEME_DIFFERENTIAL;
			break;

		case '2':
			appPlcTx.pl360Tx.modType = MOD_TYPE_8PSK;
			appPlcTx.pl360Tx.modScheme = MOD_SCHEME_DIFFERENTIAL;
			break;

		case '3':
			appPlcTx.pl360Tx.modType = MOD_TYPE_BPSK_ROBO;
			appPlcTx.pl360Tx.modScheme = MOD_SCHEME_DIFFERENTIAL;
			break;

		case '4':
			appPlcTx.pl360Tx.modType = MOD_TYPE_BPSK;
			appPlcTx.pl360Tx.modScheme = MOD_SCHEME_COHERENT;
			break;

		case '5':
			appPlcTx.pl360Tx.modType = MOD_TYPE_QPSK;
			appPlcTx.pl360Tx.modScheme = MOD_SCHEME_COHERENT;
			break;

		case '6':
			appPlcTx.pl360Tx.modType = MOD_TYPE_8PSK;
			appPlcTx.pl360Tx.modScheme = MOD_SCHEME_COHERENT;
			break;

		case '7':
			appPlcTx.pl360Tx.modType = MOD_TYPE_BPSK_ROBO;
			appPlcTx.pl360Tx.modScheme = MOD_SCHEME_COHERENT;
			break;

		default:
			result = false;
	}

    return result;
}

static bool APP_CONSOLE_SetTransmissionPeriod(char *pTime, size_t length)
{
    uint8_t index;
    uint8_t tmpValue;
    bool result = false;

    appPlcTx.pl360Tx.time = 0;

    for(index = length - 1; index > 0; index--)
    {
        if ((*pTime >= '0') && (*pTime <= '9')) {
				tmpValue = (*pTime - 0x30);
                appPlcTx.pl360Tx.time += (uint32_t)pow(10, index) * tmpValue;
                pTime++;

                result = true;
        }
        else
        {
            result = false;
            break;
        }
    }

    return result;
}

static bool APP_CONSOLE_SetDataLength(char *pDataLength, size_t length)
{
    uint16_t dataLength = 0;
    uint8_t index;
    uint8_t tmpValue;
    bool result = false;

    appPlcTx.pl360Tx.dataLength = 0;

    for (index = length; index > 0; index--)
    {
        if ((*pDataLength >= '0') && (*pDataLength <= '9')) {
				tmpValue = (*pDataLength - 0x30);
                dataLength += (uint32_t)pow(10, index - 1) * tmpValue;
                pDataLength++;
                result = true;
        }
        else
        {
            result = false;
            break;
        }
    }
    
    if (result & (dataLength < APP_PLC_BUFFER_SIZE))
    {
        appPlcTx.pl360Tx.dataLength = dataLength;
    }

    return result;
}

static bool APP_CONSOLE_SetDataMode(char *mode)
{
    size_t length;
    uint8_t* pData;
    uint32_t dataValue;
    bool result = true;

    length = appPlcTx.pl360Tx.dataLength;
    pData = appPlcTx.pl360Tx.pTransmitData;

	switch (*mode)
    {
		case '0':
            /* Random mode */
            length >>= 2;
            length++;
            while(length--)
            {
                dataValue = TRNG_ReadData();
                *pData++ = (uint8_t)dataValue;
                *pData++ = (uint8_t)(dataValue >> 8);
                *pData++ = (uint8_t)(dataValue >> 16);
                *pData++ = (uint8_t)(dataValue >> 24);
            }
			break;

		case '1':
            /* Fixed mode */
            dataValue = 0x30;
			while(length--)
            {
                *pData++ = (uint8_t)dataValue++;
                if (dataValue == 0x3A) {
                    dataValue = 0x30;
                }
            }
			break;

		default:
			result = false;
	}

    return result;
}

static bool APP_CONSOLE_SetToneMap(char *toneMap, size_t length)
{
    uint8_t* pToneMapTx;
    uint8_t newToneMap[TONE_MAP_SIZE_MAX] = {0};
    uint8_t index;
    bool result = true;
    
    if (length != (appPlcTx.toneMapSize << 1))
    {
        return false;
    }
    
    pToneMapTx = newToneMap;
    for(index = 0; index < length; index++, toneMap++)
    {
        if (*toneMap >= 0x30 && *toneMap < 0x40) {
            /* 0 to 9 */
            if (index % 2) {
                *pToneMapTx++ += *toneMap - 0x30;
            } else {
                *pToneMapTx = (*toneMap - 0x30) << 4;
            }
        } else if (*toneMap >= 0x41 && *toneMap < 0x47) {
            /* A to F */
            if (index % 2) {
                *pToneMapTx++ += *toneMap - 0x37;
            } else {
                *pToneMapTx = (*toneMap - 0x37) << 4;
            }
        } else if (*toneMap >= 0x61 && *toneMap < 0x67) {
            /* a to f */
            if (index % 2) {
                *pToneMapTx++ += *toneMap - 0x57;
            } else {
                *pToneMapTx = (*toneMap - 0x57) << 4;
            }
        } else {
            result = false;
            break;
        }
    }
    
    if (result)
    {
        /* Validate New Tone Map */
        if (((appPlcTx.pl360PhyVersion >> 16) & 0xFF) == 0x01)
        {
            /* CENA(01 - 3F) */
            if (newToneMap[0] > 0x3F)
            {
                result = false;
            }
        }
        else if (((appPlcTx.pl360PhyVersion >> 16) & 0xFF) == 0x04)
        {
            /* CENB(01 - 0F) */
            if (newToneMap[0] > 0x0F)
            {
                result = false;
            }
        }
    }
    
    if (result)
    {
        /* Update Tone Map */
        memcpy(appPlcTx.pl360Tx.toneMap, newToneMap, TONE_MAP_SIZE_MAX);
    }

    return result;
}

static bool APP_CONSOLE_SetBranchMode(char *mode)
{
    bool result = true;

	switch (*mode)
    {
		case '0':
			appPlcTx.txAuto = 1;
			appPlcTx.txImpedance = HI_STATE;
			break;

		case '1':
			appPlcTx.txAuto = 0;
			appPlcTx.txImpedance = HI_STATE;
			break;

		case '2':
			appPlcTx.txAuto = 0;
			appPlcTx.txImpedance = VLO_STATE;
			break;

		default:
			result = false;
	}

    return result;
}

static bool APP_CONSOLE_SetPlcBand(char *mode)
{
    bool result = true;

	switch (*mode)
    {
		case '0':
			appPlcTx.bin2InUse = 0;
			break;

		case '1':
			appPlcTx.bin2InUse = 1;
			break;

		default:
			result = false;
	}

    return result;
}

static void APP_CONSOLE_ShowConfiguration(void)
{
    uint8_t index;

    APP_CONSOLE_Print("\n\r-- Configuration Info --------------\r\n");
    APP_CONSOLE_Print("-I- PHY Version: 0x%08X\n\r", (unsigned int)appPlcTx.pl360PhyVersion);

    if (appPlcTx.pl360Tx.attenuation == 0xFF)
    {
        APP_CONSOLE_Print("-I- TX Attenuation: 0xFF (no signal)\n\r");
    }
    else
    {
        APP_CONSOLE_Print("-I- TX Attenuation: 0x%02X\n\r", (unsigned int)appPlcTx.pl360Tx.attenuation);
    }

    switch (appPlcTx.pl360Tx.modType)
    {
        case MOD_TYPE_BPSK:
            if (appPlcTx.pl360Tx.modScheme)
            {
                APP_CONSOLE_Print("-I- Modulation Scheme: Coherent BPSK \n\r");
            }
            else
            {
                APP_CONSOLE_Print("-I- Modulation Scheme: Differential BPSK \n\r");
            }
		break;

        case MOD_TYPE_QPSK:
            if (appPlcTx.pl360Tx.modScheme)
            {
                APP_CONSOLE_Print("-I- Modulation Scheme: Coherent QPSK \n\r");
            }
            else
            {
                APP_CONSOLE_Print("-I- Modulation Scheme: Differential QPSK \n\r");
            }
            break;

        case MOD_TYPE_8PSK:
            if (appPlcTx.pl360Tx.modScheme)
            {
                APP_CONSOLE_Print("-I- Modulation Scheme: Coherent 8PSK \n\r");
            }
            else
            {
                APP_CONSOLE_Print("-I- Modulation Scheme: Differential 8PSK \n\r");
            }
            break;

        case MOD_TYPE_BPSK_ROBO:
            if (appPlcTx.pl360Tx.modScheme)
            {
                APP_CONSOLE_Print("-I- Modulation Scheme: Coherent Robust\n\r");
            }
            else
            {
                APP_CONSOLE_Print("-I- Modulation Scheme: Differential Robust\n\r");
            }
            break;

        default:
            APP_CONSOLE_Print("-I- Modulation Scheme: ERROR CFG\n\r");
	}

    APP_CONSOLE_Print("-I- Tone Map: ");
	for (index = 0; index < appPlcTx.toneMapSize; index++) {
		APP_CONSOLE_Print("%02X", appPlcTx.pl360Tx.toneMap[index]);
	}
	APP_CONSOLE_Print("\r\n");

    if (appPlcTx.txAuto)
    {
		APP_CONSOLE_Print("-I- Branch Mode : Autodetect - ");
	}
    else
    {
		APP_CONSOLE_Print("-I- Branch Mode : Fixed - ");
	}

	if (appPlcTx.txImpedance == HI_STATE)
    {
		APP_CONSOLE_Print("High Impedance \r\n");
	}
    else
    {
		APP_CONSOLE_Print("Very Low Impedance \r\n");
	}

	APP_CONSOLE_Print("-I- Time Period: %u\n\r", (unsigned int)appPlcTx.pl360Tx.time);
	APP_CONSOLE_Print("-I- Data Len: %u\n\r", (unsigned int)appPlcTx.pl360Tx.dataLength);

	if (appPlcTx.pl360Tx.pTransmitData[0] == 0x30)
    {
		APP_CONSOLE_Print("-I- Fixed Data\r\n");
	}
    else
    {
		APP_CONSOLE_Print("-I- Random Data\r\n");
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
    /* Place the App state machine in its initial state. */
    appConsole.state = APP_CONSOLE_STATE_IDLE;

    /* Init Timer handler */
    appConsole.tmr1Handle = SYS_TIME_HANDLE_INVALID;

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
    CLEAR_WATCHDOG();
    
    /* Read console port */
    APP_CONSOLE_ReadSerialChar();

    /* Check the application's current state. */
    switch ( appConsole.state )
    {
        /* Application's initial state. */
        case APP_CONSOLE_STATE_INIT:
        {
            appConsole.state = APP_CONSOLE_STATE_WAIT_PLC;

            /* Show App Header */
            APP_CONSOLE_Print(STRING_HEADER);

            break;
        }

        case APP_CONSOLE_STATE_WAIT_PLC:
        {
            /* Wait for PLC transceiver initialization */
            if (appPlc.state == APP_PLC_STATE_WAITING)
            {
                /* Set PLC Phy Tone Map Size */
                if (((appPlcTx.pl360PhyVersion >> 16) & 0xFF) == 0x01)
                {
                    /* CENA(01 - 3F) */
                    appPlcTx.toneMapSize = 1;
                }
                else if (((appPlcTx.pl360PhyVersion >> 16) & 0xFF) == 0x02)
                {
                    /* FCC(000001 - FFFFFF) */
                    appPlcTx.toneMapSize = 3;
                }
                else if (((appPlcTx.pl360PhyVersion >> 16) & 0xFF) == 0x04)
                {
                    /* CENB(01 - 0F) */
                    appPlcTx.toneMapSize = 1;
                }
                /* Show Console menu */
                appConsole.state = APP_CONSOLE_STATE_SHOW_MENU;
            }
            else if (appPlc.state == APP_PLC_STATE_TX)
            {
                /* Set TX state */
                appConsole.state = APP_CONSOLE_STATE_TX;
            }
            break;
        }

        case APP_CONSOLE_STATE_SHOW_MENU:
        {
            /* Show console interface */
            APP_CONSOLE_Print(MENU_HEADER);
            
            /* Show console prompt */
            APP_CONSOLE_Print(MENU_PROMPT);

            /* Waiting Console command */
            appConsole.state = APP_CONSOLE_STATE_CONSOLE;
            APP_CONSOLE_ReadRestart(1);

            break;
        }

        case APP_CONSOLE_STATE_CONSOLE:
        {
            if (appConsole.numCharToReceive == 0)
            {
                switch(*appConsole.pReceivedChar)
                {
                    case '0':
                        appConsole.state = APP_CONSOLE_STATE_SET_ATT_LEVEL;
                        APP_CONSOLE_Print("\r\nEnter attenuation level using 2 digits [00..FF][use FF for signal 0] : ");
                        APP_CONSOLE_ReadRestart(2);
                        break;

                    case '1':
                        appConsole.state = APP_CONSOLE_STATE_SET_SCHEME;
                        APP_CONSOLE_Print(MENU_SCHEME);
                        APP_CONSOLE_ReadRestart(1);
                        break;

                    case '2':
                        APP_CONSOLE_Print("\r\nEnter transmission period in us. (max. 10 digits and value min 2100 us): ");
                        appConsole.state = APP_CONSOLE_STATE_SET_TIME_PERIOD;
                        APP_CONSOLE_ReadRestart(10);
                        break;

                    case '3':
                        APP_CONSOLE_Print("\r\nEnter length of data to transmit in bytes (max. 512 bytes): ");
                        appConsole.state = APP_CONSOLE_STATE_SET_DATA_LEN;
                        APP_CONSOLE_ReadRestart(3);
                        break;

                    case '4':
                        if (((appPlcTx.pl360PhyVersion >> 16) & 0xFF) == 0x01)
                        {
                            APP_CONSOLE_Print("\r\nEnter enter value for tone map. CENA(01 - 3F) : ");
                            appPlcTx.toneMapSize = 1;
                            APP_CONSOLE_ReadRestart(2);
                        }
                        else if (((appPlcTx.pl360PhyVersion >> 16) & 0xFF) == 0x02)
                        {
                            APP_CONSOLE_Print("\r\nEnter enter value for tone map. FCC(000001 - FFFFFF) : ");
                            appPlcTx.toneMapSize = 3;
                            APP_CONSOLE_ReadRestart(6);
                        }
                        else if (((appPlcTx.pl360PhyVersion >> 16) & 0xFF) == 0x04)
                        {
                            APP_CONSOLE_Print("\r\nEnter enter value for tone map. CENB(01 - 0F) : ");
                            appPlcTx.toneMapSize = 1;
                            APP_CONSOLE_ReadRestart(2);
                        }

                        appConsole.state = APP_CONSOLE_STATE_SET_TONE_MAP;
                        break;

                    case '5':
                        APP_CONSOLE_Print(MENU_BRANCH_MODE);
                        appConsole.state = APP_CONSOLE_STATE_SET_BRANCH_MODE;
                        APP_CONSOLE_ReadRestart(1);
                        break;

                    case '6':
                        if (appPlc.plcMultiband)
                        {
                            uint8_t phyBand;
                            
                            APP_CONSOLE_Print("\n\r-- Select PLC band  --------------\r\n");
                            phyBand = SRV_PCOUP_Get_Phy_Band(SRV_PLC_PCOUP_MAIN_BRANCH);
                            switch(phyBand)
                            {
                                case G3_CEN_A:
                                    APP_CONSOLE_Print("0: MAIN BRANCH [CEN-A]\n\r");
                                    break;
                                    
                                case G3_CEN_B:
                                    APP_CONSOLE_Print("0: MAIN BRANCH [CEN-B]\n\r");
                                    break;
                                    
                                case G3_FCC:
                                    APP_CONSOLE_Print("0: MAIN BRANCH [FCC]\n\r");
                                    break;
                                    
                            }
                            
                            phyBand = SRV_PCOUP_Get_Phy_Band(SRV_PLC_PCOUP_AUXILIARY_BRANCH);
                            switch(phyBand)
                            {
                                case G3_CEN_A:
                                    APP_CONSOLE_Print("1: AUXILIARY BRANCH [CEN-A]\n\r");
                                    break;
                                    
                                case G3_CEN_B:
                                    APP_CONSOLE_Print("1: AUXILIARY BRANCH [CEN-B]\n\r");
                                    break;
                                    
                                case G3_FCC:
                                    APP_CONSOLE_Print("1: AUXILIARY BRANCH [FCC]\n\r");
                                    break;
                                    
                            }
                            
                            appConsole.state = APP_CONSOLE_STATE_SET_PLC_BAND;
                            APP_CONSOLE_ReadRestart(1);
                        }
                        else
                        {
                            APP_CONSOLE_Print("\r\nMulti-band option is not supported.\r\n");
                            APP_CONSOLE_ReadRestart(1);
                        }
                        break;

                    case 'v':
                    case 'V':
                        appConsole.state = APP_CONSOLE_STATE_VIEW_CONFIG;
                        break;

                    case 'e':
                    case 'E':
                        APP_CONSOLE_Print("\r\nStart transmission, type 'x' to cancel.\r\n");
                        appPlc.state = APP_PLC_STATE_TX;
                        appConsole.state = APP_CONSOLE_STATE_TX;
                        APP_CONSOLE_ReadRestart(1);
                        break;

                    case 'c':
                    case 'C':
                        if (appConsole.echoEnable)
                        {
                            appConsole.echoEnable = false;
                            APP_CONSOLE_Print("\r\nConsole ECHO disabled.\r\n");
                        }
                        else
                        {
                            appConsole.echoEnable = true;
                            APP_CONSOLE_Print("\r\nConsole ECHO enabled.\r\n");
                        }
                        
                        appConsole.state = APP_CONSOLE_STATE_SHOW_MENU;
                        break;

                    default:
                        /* Discard character */
                        appConsole.state = APP_CONSOLE_STATE_ERROR;
                        break;

                }
            }

            break;
        }

        case APP_CONSOLE_STATE_SET_ATT_LEVEL:
        {
            if (appConsole.numCharToReceive == 0)
            {
                if (APP_CONSOLE_SetAttenuationLevel(appConsole.pReceivedChar))
                {
                    APP_CONSOLE_Print("\r\nSet Attenuation level = 0x%02x\r\n",
                            (unsigned int)appPlcTx.pl360Tx.attenuation);
                    appConsole.state = APP_CONSOLE_STATE_SHOW_MENU;
                }
                else
                {
                    /* Try it again */
                    APP_CONSOLE_Print("\r\nERROR: Attenuation level not permitted. Try again : ");
                    APP_CONSOLE_ReadRestart(2);
                }
            }
            break;
        }

        case APP_CONSOLE_STATE_SET_SCHEME:
        {
            if (appConsole.numCharToReceive == 0)
            {
                if (APP_CONSOLE_SetScheme(appConsole.pReceivedChar))
                {
                    APP_CONSOLE_Print("\r\nSet Type: %u, Scheme: %u \r\n",
                            (unsigned int)appPlcTx.pl360Tx.modType,
                            (unsigned int)appPlcTx.pl360Tx.modScheme);
                    appConsole.state = APP_CONSOLE_STATE_SHOW_MENU;
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

        case APP_CONSOLE_STATE_SET_TIME_PERIOD:
        {
            if (appConsole.numCharToReceive == 0)
            {
                if (APP_CONSOLE_SetTransmissionPeriod(appConsole.pReceivedChar, appConsole.dataLength))
                {
                    APP_CONSOLE_Print("\r\nSet Time Period = %u us.\r\n",
                            (unsigned int)appPlcTx.pl360Tx.time);
                    appConsole.state = APP_CONSOLE_STATE_SHOW_MENU;
                }
                else
                {
                    /* Try it again */
                    APP_CONSOLE_Print("\r\nERROR: Time Period not defined. Try again : ");
                    APP_CONSOLE_ReadRestart(10);
                }
                break;
            }
        }

        case APP_CONSOLE_STATE_SET_DATA_LEN:
        {
            if (appConsole.numCharToReceive == 0)
            {
                if (APP_CONSOLE_SetDataLength(appConsole.pReceivedChar, appConsole.dataLength))
                {
                    APP_CONSOLE_Print("\r\nSet Data Length = %u bytes\r\n",
                            (unsigned int)appPlcTx.pl360Tx.dataLength);

                    /* Set Data content */
                    APP_CONSOLE_Print(MENU_DATA_MODE);
                    appConsole.state = APP_CONSOLE_STATE_SET_DATA;
                    APP_CONSOLE_ReadRestart(1);
                }
                else
                {
                    /* Try it again */
                    APP_CONSOLE_Print("\r\nERROR: Data length is not permitted. Try again : ");
                    APP_CONSOLE_ReadRestart(3);
                }
            }
            break;
        }

        case APP_CONSOLE_STATE_SET_DATA:
        {
            if (appConsole.numCharToReceive == 0)
            {
                if (APP_CONSOLE_SetDataMode(appConsole.pReceivedChar))
                {
                    APP_CONSOLE_Print("\r\nSet Data mode successfully\r\n");
                    appConsole.state = APP_CONSOLE_STATE_SHOW_MENU;
                }
                else
                {
                    /* Try it again */
                    APP_CONSOLE_Print("\r\nERROR: Data Mode not permitted. Try again : ");
                    APP_CONSOLE_ReadRestart(1);
                }
            }
            break;
        }

        case APP_CONSOLE_STATE_SET_TONE_MAP:
        {
            if (appConsole.numCharToReceive == 0)
            {
                if (APP_CONSOLE_SetToneMap(appConsole.pReceivedChar, appConsole.dataLength))
                {
                    if (appPlcTx.toneMapSize == 3)
                    {
                        APP_CONSOLE_Print("\r\nSet ToneMap: %02x%02x%02x \r\n",
                            (unsigned int)appPlcTx.pl360Tx.toneMap[0],
                            (unsigned int)appPlcTx.pl360Tx.toneMap[1],
                            (unsigned int)appPlcTx.pl360Tx.toneMap[2]);
                    }
                    else
                    {
                        APP_CONSOLE_Print("\r\nSet ToneMap: %02X \r\n",
                            (unsigned int)appPlcTx.pl360Tx.toneMap[0]);
                    }
                    appConsole.state = APP_CONSOLE_STATE_SHOW_MENU;
                }
                else
                {
                    /* Try it again */
                    APP_CONSOLE_Print("\r\nERROR: ToneMap not permitted. Try again : ");
                    APP_CONSOLE_ReadRestart(appPlcTx.toneMapSize << 1);
                }
            }
            break;
        }

        case APP_CONSOLE_STATE_SET_BRANCH_MODE:
        {
            if (appConsole.numCharToReceive == 0)
            {
                if (APP_CONSOLE_SetBranchMode(appConsole.pReceivedChar))
                {
                    APP_CONSOLE_Print("\r\nSet Auto: %u, Branch: %u \r\n",
                            (unsigned int)appPlcTx.txAuto,
                            (unsigned int)appPlcTx.txImpedance);
                    appConsole.state = APP_CONSOLE_STATE_SHOW_MENU;
                }
                else
                {
                    /* Try it again */
                    APP_CONSOLE_Print("\r\nERROR: Branch Mode not permitted. Try again : ");
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
                    appConsole.state = APP_CONSOLE_STATE_SHOW_MENU;
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

        case APP_CONSOLE_STATE_VIEW_CONFIG:
        {
            APP_CONSOLE_ShowConfiguration();
            appConsole.state = APP_CONSOLE_STATE_SHOW_MENU;
            break;
        }

        case APP_CONSOLE_STATE_TX:
        {
            if (appConsole.numCharToReceive == 0)
            {
                if ((appConsole.pReceivedChar[0] == 'x') || (appConsole.pReceivedChar[0] == 'X'))
                {
                    APP_CONSOLE_Print("\r\nCancel transmission\r\n");
                    appPlc.state = APP_PLC_STATE_STOP_TX;
                    appConsole.state = APP_CONSOLE_STATE_SHOW_MENU;
                }
                else
                {
                    APP_CONSOLE_ReadRestart(1);
                }
            }
            break;
        }

        case APP_CONSOLE_STATE_ERROR:
        {
            APP_CONSOLE_Print("\r\nERROR: Unknown received character\r\n");
            appConsole.state = APP_CONSOLE_STATE_SHOW_MENU;
            break;
        }

        /* The default state should never be executed. */
        default:
        {
            break;
        }
    }
}

void APP_CONSOLE_Print(const char *format, ...)
{
    size_t len = 0;
    uint32_t numRetries = 10000;
    
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
            
            /* Refresh WDG */
            CLEAR_WATCHDOG();
        }
        else
        {
            return;
        }
    }

    va_start( sArgs, format );
    len = vsnprintf(appConsole.pTransmitChar, SERIAL_BUFFER_SIZE - 1, format, sArgs);
    va_end( sArgs );
    
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
