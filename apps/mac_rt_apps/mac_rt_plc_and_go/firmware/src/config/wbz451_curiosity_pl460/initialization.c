/*******************************************************************************
  System Initialization File

  File Name:
    initialization.c

  Summary:
    This file contains source code necessary to initialize the system.

  Description:
    This file contains source code necessary to initialize the system.  It
    implements the "SYS_Initialize" function, defines the configuration bits,
    and allocates any necessary global system resources,
 *******************************************************************************/

// DOM-IGNORE-BEGIN
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
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "configuration.h"
#include "definitions.h"
#include "device.h"


// ****************************************************************************
// ****************************************************************************
// Section: Configuration Bits
// ****************************************************************************
// ****************************************************************************

/*** FUSERID ***/
#pragma config USER_ID =      0xffffU

/*** DEVCFG0 ***/
#pragma config TDOEN =      ON
#pragma config SWOEN =      ON
#pragma config TROEN =      OFF
#pragma config ADCPOVR =      HW
#pragma config ACCMP1_ALTEN =      PA1
#pragma config CPENFILT =      OFF
#pragma config RTCIN0_ALTEN =      PA3
#pragma config RTCOUT_ALTEN =      PA4
#pragma config VBCMODE =      DIRECT
#pragma config SMBUSEN0 =      OFF
#pragma config SMBUSEN1 =      OFF
#pragma config SMBUSEN2 =      OFF
#pragma config HPLUGDIS =      OFF
#pragma config SLRTEN0 =      OFF
#pragma config SLRTEN1 =      OFF
#pragma config SLRTEN2 =      OFF
#pragma config PCM =      SFR
#pragma config INT0E =      OFF
#pragma config INT0P =      NEG
#pragma config FECCCON =         OFF
#pragma config FRECCDIS =      OFF


/*** DEVCFG1 ***/
#pragma config ICESEL =      PGC1_PGD1
#pragma config TRCEN =      ON
#pragma config ZBTWKSYS =      OFF
#pragma config CMP0_OE =      OFF
#pragma config CMP1_OE =      OFF
#pragma config CLASSBDIS =      OFF
#pragma config SLRCTRL0 =      GPIO
#pragma config SLRCTRL1 =      GPIO
#pragma config SLRCTRL2 =      GPIO
#pragma config SMCLR =      NO_POR
#pragma config QSCHE_EN =      OFF
#pragma config QSPI_HSEN =      PPS
#pragma config SCOM0_HSEN =      PPS
#pragma config SCOM1_HSEN =      PPS
#pragma config SCOM2_HSEN =      PPS
#pragma config CCL_OE =      ON
#pragma config I2CDSEL0 =      OFF
#pragma config I2CDSEL1 =      OFF
#pragma config I2CDSEL2 =      OFF
#pragma config WDTPSS =      PSS1048576
#pragma config QSPIDDRM =      OFF
#pragma config CLKZBREF =      OFF

/*** DEVCFG2 ***/
#pragma config ACMP_CYCLE =      _32US
#pragma config DMTINTV =      WIN_127_128
#pragma config PMUTEST_VDD_EN =      VDD_DIV_2
#pragma config POSCMOD =      HS
#pragma config WDTRMCS =      LPRC
#pragma config SOSCSEL =      XTAL
#pragma config WAKE2SPD =      ON
#pragma config CKSWEN =      ON
#pragma config FSCMEN =      ON
#pragma config WDTPSR =      PS1048576
#pragma config WDTSPGM =      STOPS
#pragma config WINDIS =      NORMAL
#pragma config WDTEN =      OFF
#pragma config WDTWINSZ =    WINSZ_25
#pragma config DMTCNT =      DMT31
#pragma config DMTEN =      OFF


/*** DEVCFG4 ***/
#pragma config SOSCCFG =    0x0U
#pragma config RTCEVENT_SEL =      ONE_SEC
#pragma config RTCEVENT_EN =      OFF
#pragma config VBKP_1KCSEL =      _32K
#pragma config VBKP_32KCSEL =      FRC
#pragma config VBKP_DIVSEL =      DIV_31_25
#pragma config LPCLK_MOD =      DIV_1
#pragma config RTCEVTYPE =      OUT
#pragma config CPEN_DLY =      _8_LPRC
#pragma config DSZPBOREN =      OFF
#pragma config DSWDTPS =      DSPS5
#pragma config DSWDTOSC =    LPRC
#pragma config DSWDTEN =      OFF
#pragma config DSEN =    OFF
#pragma config UVREGROVR =      CONTROLLED
#pragma config RTCNTM_CSEL =      RAW

/*** FBCFG0 ***/
#pragma config BINFOVALID =      VALID
#pragma config PCSCMODE =      SINGLE

/*** FCPN0 ***/
#pragma config CP =      DISABLED





// *****************************************************************************
// *****************************************************************************
// Section: Driver Initialization Data
// *****************************************************************************
// *****************************************************************************
/* Following MISRA-C rules are deviated in the below code block */
/* MISRA C-2012 Rule 11.1 */
/* MISRA C-2012 Rule 11.3 */
/* MISRA C-2012 Rule 11.8 */

// <editor-fold defaultstate="collapsed" desc="_on_reset() critical function">
/* This routine should initialize the PL460 control pins as soon as possible */
/* after a power up reset to avoid risks on starting up PL460 device when */
/* pull up resistors are configured by default */
void _on_reset(void)
{

    /* Enable Reset Pin */
    SYS_PORT_PinOutputEnable(DRV_PLC_RESET_PIN);
    SYS_PORT_PinClear(DRV_PLC_RESET_PIN);
    /* Disable LDO Pin */
    SYS_PORT_PinOutputEnable(DRV_PLC_LDO_EN_PIN);
    SYS_PORT_PinClear(DRV_PLC_LDO_EN_PIN);
}
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="DRV_PLC_HAL Initialization Data">
/* HAL Interface Initialization for PLC transceiver */
DRV_PLC_PLIB_INTERFACE drvPLCPlib = {

    /* SPI Transfer Setup */
    .spiPlibTransferSetup = (DRV_PLC_SPI_PLIB_TRANSFER_SETUP)SERCOM0_SPI_TransferSetup,

    /* DMA Channel for Transmit */
    .dmaChannelTx = SYS_DMA_CHANNEL_0,

    /* DMA Channel for Receive */
    .dmaChannelRx = SYS_DMA_CHANNEL_1,

    /* SPI Transmit Register */
    .spiAddressTx = (void *)&(SERCOM0_REGS->SPIM.SERCOM_DATA),

    /* SPI Receive Register */
    .spiAddressRx  = (void *)&(SERCOM0_REGS->SPIM.SERCOM_DATA),

    /* SPI Chip select pin */
    .spiCSPin = DRV_PLC_SPI_CS_PIN,

    /* SPI clock frequency */
    .spiClockFrequency = DRV_PLC_SPI_CLK,

    /* PLC LDO Enable Pin */
    .ldoPin = DRV_PLC_LDO_EN_PIN,

    /* PLC Reset Pin */
    .resetPin = DRV_PLC_RESET_PIN,

    /* PLC External Interrupt Pin */
    .extIntPin = DRV_PLC_EXT_INT_PIN,

    /* PLC External Interrupt Pio */
    .extIntPio = DRV_PLC_EXT_INT_PIO,

    /* PLC TX Enable Pin */
    .txEnablePin = DRV_PLC_TX_ENABLE_PIN,

};

/* HAL Interface Initialization for PLC transceiver */
DRV_PLC_HAL_INTERFACE drvPLCHalAPI = {

    /* PLC PLIB */
    .plcPlib = &drvPLCPlib,

    /* PLC HAL init */
    .init = (DRV_PLC_HAL_INIT)DRV_PLC_HAL_Init,

    /* PLC HAL setup */
    .setup = (DRV_PLC_HAL_SETUP)DRV_PLC_HAL_Setup,

    /* PLC transceiver reset */
    .reset = (DRV_PLC_HAL_RESET)DRV_PLC_HAL_Reset,

    /* PLC Set TX Enable Pin */
    .setTxEnable = (DRV_PLC_HAL_SET_TXENABLE)DRV_PLC_HAL_SetTxEnable,

    /* PLC HAL Enable/Disable external interrupt */
    .enableExtInt = (DRV_PLC_HAL_ENABLE_EXT_INT)DRV_PLC_HAL_EnableInterrupts,

    /* PLC HAL Enable/Disable external interrupt */
    .getPinLevel = (DRV_PLC_HAL_GET_PIN_LEVEL)DRV_PLC_HAL_GetPinLevel,

    /* PLC HAL delay function */
    .delay = (DRV_PLC_HAL_DELAY)DRV_PLC_HAL_Delay,

    /* PLC HAL Transfer Bootloader Command */
    .sendBootCmd = (DRV_PLC_HAL_SEND_BOOT_CMD)DRV_PLC_HAL_SendBootCmd,

    /* PLC HAL Transfer Write/Read Command */
    .sendWrRdCmd = (DRV_PLC_HAL_SEND_WRRD_CMD)DRV_PLC_HAL_SendWrRdCmd,
};

// </editor-fold>
// <editor-fold defaultstate="collapsed" desc="DRV_G3_MACRT Initialization Data">

/* PLC MAC RT Binary file addressing */
extern uint8_t g3_mac_rt_bin_start;
extern uint8_t g3_mac_rt_bin_end;
extern uint8_t g3_mac_rt_bin2_start;
extern uint8_t g3_mac_rt_bin2_end;

/* G3 MAC RT Driver Initialization Data */
DRV_G3_MACRT_INIT drvG3MacRtInitData = {

    /* SPI PLIB API interface*/
    .plcHal = &drvPLCHalAPI,

    /* PLC MAC RT Binary start address */
    .binStartAddress = (uint32_t)&g3_mac_rt_bin_start,

    /* PLC MAC RT Binary end address */
    .binEndAddress = (uint32_t)&g3_mac_rt_bin_end,

    /* Secure Mode */
    .secure = DRV_PLC_SECURE,

};

// </editor-fold>



// *****************************************************************************
// *****************************************************************************
// Section: System Data
// *****************************************************************************
// *****************************************************************************
/* Structure to hold the object handles for the modules in the system. */
SYSTEM_OBJECTS sysObj;

// *****************************************************************************
// *****************************************************************************
// Section: Library/Stack Initialization Data
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: System Initialization
// *****************************************************************************
// *****************************************************************************
// <editor-fold defaultstate="collapsed" desc="SYS_TIME Initialization Data">

static const SYS_TIME_PLIB_INTERFACE sysTimePlibAPI = {
    .timerCallbackSet = (SYS_TIME_PLIB_CALLBACK_REGISTER)TC0_TimerCallbackRegister,
    .timerStart = (SYS_TIME_PLIB_START)TC0_TimerStart,
    .timerStop = (SYS_TIME_PLIB_STOP)TC0_TimerStop,
    .timerFrequencyGet = (SYS_TIME_PLIB_FREQUENCY_GET)TC0_TimerFrequencyGet,
    .timerPeriodSet = (SYS_TIME_PLIB_PERIOD_SET)TC0_Timer16bitPeriodSet,
    .timerCompareSet = (SYS_TIME_PLIB_COMPARE_SET)TC0_Timer16bitCompareSet,
    .timerCounterGet = (SYS_TIME_PLIB_COUNTER_GET)TC0_Timer16bitCounterGet,
};

static const SYS_TIME_INIT sysTimeInitData =
{
    .timePlib = &sysTimePlibAPI,
    .hwTimerIntNum = TC0_IRQn,
};

// </editor-fold>
// <editor-fold defaultstate="collapsed" desc="SYS_CONSOLE Instance 0 Initialization Data">


static const SYS_CONSOLE_UART_PLIB_INTERFACE sysConsole0UARTPlibAPI =
{
    .read_t = (SYS_CONSOLE_UART_PLIB_READ)SERCOM1_USART_Read,
    .readCountGet = (SYS_CONSOLE_UART_PLIB_READ_COUNT_GET)SERCOM1_USART_ReadCountGet,
    .readFreeBufferCountGet = (SYS_CONSOLE_UART_PLIB_READ_FREE_BUFFFER_COUNT_GET)SERCOM1_USART_ReadFreeBufferCountGet,
    .write_t = (SYS_CONSOLE_UART_PLIB_WRITE)SERCOM1_USART_Write,
    .writeCountGet = (SYS_CONSOLE_UART_PLIB_WRITE_COUNT_GET)SERCOM1_USART_WriteCountGet,
    .writeFreeBufferCountGet = (SYS_CONSOLE_UART_PLIB_WRITE_FREE_BUFFER_COUNT_GET)SERCOM1_USART_WriteFreeBufferCountGet,
};

static const SYS_CONSOLE_UART_INIT_DATA sysConsole0UARTInitData =
{
    .uartPLIB = &sysConsole0UARTPlibAPI,
};

static const SYS_CONSOLE_INIT sysConsole0Init =
{
    .deviceInitData = (const void*)&sysConsole0UARTInitData,
    .consDevDesc = &sysConsoleUARTDevDesc,
    .deviceIndex = 0,
};



// </editor-fold>




// *****************************************************************************
// *****************************************************************************
// Section: Local initialization functions
// *****************************************************************************
// *****************************************************************************

/* MISRAC 2012 deviation block end */

/*******************************************************************************
  Function:
    void SYS_Initialize ( void *data )

  Summary:
    Initializes the board, services, drivers, application and other modules.

  Remarks:
 */

void SYS_Initialize ( void* data )
{

    /* MISRAC 2012 deviation block start */
    /* MISRA C-2012 Rule 2.2 deviated in this file.  Deviation record ID -  H3_MISRAC_2012_R_2_2_DR_1 */


    CLK_Initialize();
    /* Configure Prefetch, Wait States */
    PCHE_REGS->PCHE_CHECON = (PCHE_REGS->PCHE_CHECON & (~(PCHE_CHECON_PFMWS_Msk | PCHE_CHECON_ADRWS_Msk | PCHE_CHECON_PREFEN_Msk)))
                                    | (PCHE_CHECON_PFMWS(1) | PCHE_CHECON_PREFEN(1));



	GPIO_Initialize();

    SERCOM1_USART_Initialize();

    SERCOM0_SPI_Initialize();

    EVSYS_Initialize();

    DMAC_Initialize();

    EIC_Initialize();

    TC0_TimerInitialize();

	TRNG_Initialize();

	BSP_Initialize();


    /* MISRAC 2012 deviation block start */
    /* Following MISRA-C rules deviated in this block  */
    /* MISRA C-2012 Rule 11.3 - Deviation record ID - H3_MISRAC_2012_R_11_3_DR_1 */
    /* MISRA C-2012 Rule 11.8 - Deviation record ID - H3_MISRAC_2012_R_11_8_DR_1 */

    /* Initialize G3 MAC RT Driver Instance */
    sysObj.drvG3MacRt = DRV_G3_MACRT_Initialize(DRV_G3_MACRT_INDEX, (SYS_MODULE_INIT *)&drvG3MacRtInitData);
    EIC_CallbackRegister(DRV_PLC_EXT_INT_PIN, DRV_G3_MACRT_ExternalInterruptHandler, sysObj.drvG3MacRt);


    /* MISRA C-2012 Rule 11.3, 11.8 deviated below. Deviation record ID -
    H3_MISRAC_2012_R_11_3_DR_1 & H3_MISRAC_2012_R_11_8_DR_1*/

    sysObj.sysTime = SYS_TIME_Initialize(SYS_TIME_INDEX_0, (SYS_MODULE_INIT *)&sysTimeInitData);

    /* MISRAC 2012 deviation block end */
    /* MISRA C-2012 Rule 11.3, 11.8 deviated below. Deviation record ID -
     H3_MISRAC_2012_R_11_3_DR_1 & H3_MISRAC_2012_R_11_8_DR_1*/
        sysObj.sysConsole0 = SYS_CONSOLE_Initialize(SYS_CONSOLE_INDEX_0, (SYS_MODULE_INIT *)&sysConsole0Init);
   /* MISRAC 2012 deviation block end */


    /* MISRAC 2012 deviation block end */
    APP_PLC_WBZ451_Initialize();
    APP_CONSOLE_Initialize();


    NVIC_Initialize();


    /* MISRAC 2012 deviation block end */
}

/*******************************************************************************
 End of File
*/
