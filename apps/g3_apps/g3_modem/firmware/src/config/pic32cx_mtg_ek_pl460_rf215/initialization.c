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
#pragma config SECURITY_BIT = CLEAR
#pragma config BOOT_MODE = 0x3
#pragma config PLANE_SELECTION = CLEAR
#pragma config ERASE_FUNCTION_LOCK = 0x0




// *****************************************************************************
// *****************************************************************************
// Section: Driver Initialization Data
// *****************************************************************************
// *****************************************************************************
// <editor-fold defaultstate="collapsed" desc="DRV_PLC_HAL Initialization Data">

/* HAL Interface Initialization for PLC transceiver */
DRV_PLC_PLIB_INTERFACE drvPLCPlib = {

    /* SPI Transfer Setup */
    .spiPlibTransferSetup = (DRV_PLC_SPI_PLIB_TRANSFER_SETUP)FLEXCOM5_SPI_TransferSetup,

    /* SPI Is Busy */
    .spiIsBusy = FLEXCOM5_SPI_IsTransmitterBusy,

    /* SPI Write/Read */
    .spiWriteRead = FLEXCOM5_SPI_WriteRead,

    /* SPI CSR register address. */
    .spiCSR  = (void *)&(FLEXCOM5_REGS->FLEX_SPI_CSR[DRV_PLC_CSR_INDEX]),
    
    /* SPI clock frequency */
    .spiClockFrequency = DRV_PLC_SPI_CLK,
    
    /* PLC LDO Enable Pin */
    .ldoPin = DRV_PLC_LDO_EN_PIN, 
    
    /* PLC Reset Pin */
    .resetPin = DRV_PLC_RESET_PIN,
       
    /* PLC External Interrupt Pin */
    .extIntPin = DRV_PLC_EXT_INT_PIN,

    /* PLC TX Enable Pin */
    .txEnablePin = DRV_PLC_TX_ENABLE_PIN,
    
    /* PLC External Interrupt Pin */
    .thMonPin = DRV_PLC_THMON_PIN,
    
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

    /* PLC Get Thermal Monitor value */
    .getThermalMonitor = (DRV_PLC_HAL_GET_THMON)DRV_PLC_HAL_GetThermalMonitor,
    
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
// <editor-fold defaultstate="collapsed" desc="DRV_RF215 Initialization Data">

/* RF215 Driver Initialization Data */
const DRV_RF215_INIT drvRf215InitData = {
    /* SPI chip select register address used for SPI configuration */
    .spiCSRegAddress = (uint32_t *)&(FLEXCOM3_REGS->FLEX_SPI_CSR[DRV_RF215_CSR_INDEX]),

    /* Pointer to SPI PLIB is busy function */
    .spiPlibIsBusy = FLEXCOM3_SPI_IsTransmitterBusy,

    /* Pointer to SPI Write and Read function */
    .spiPlibWriteRead = FLEXCOM3_SPI_WriteRead,

    /* Pointer to SPI Register Callback function */
    .spiPlibSetCallback = FLEXCOM3_SPI_CallbackRegister,

    /* Interrupt source ID for DMA */
    .dmaIntSource = FLEXCOM3_IRQn,

    /* Interrupt source ID for SYS_TIME */
    .sysTimeIntSource = TC0_CH0_IRQn,

    /* Interrupt source ID for PLC external interrupt */
    .plcExtIntSource = PIOA_IRQn,

    /* Initial PHY frequency band and operating mode for Sub-GHz transceiver */
    .rf09PhyBandOpmIni = SUN_FSK_BAND_863_OPM1,

    /* Initial PHY frequency channel number for Sub-GHz transceiver */
    .rf09PhyChnNumIni = 0,

};

// </editor-fold>
// <editor-fold defaultstate="collapsed" desc="SRV_USI Instance 0 Initialization Data">

static uint8_t CACHE_ALIGN srvUSI0ReadBuffer[SRV_USI0_RD_BUF_SIZE] = {0};
static uint8_t CACHE_ALIGN srvUSI0WriteBuffer[SRV_USI0_WR_BUF_SIZE] = {0};

/* Declared in USI USART service implementation (srv_usi_usart.c) */
extern const SRV_USI_DEV_DESC srvUSIUSARTDevDesc;

const SRV_USI_USART_INTERFACE srvUsi0InitDataFLEXCOM0 = {
    .readCallbackRegister = (USI_USART_PLIB_READ_CALLBACK_REG)FLEXCOM0_USART_ReadCallbackRegister,
    .read = (USI_USART_PLIB_WRRD)FLEXCOM0_USART_Read,
    .write = (USI_USART_PLIB_WRRD)FLEXCOM0_USART_Write,
    .writeIsBusy = (USI_USART_PLIB_WRITE_ISBUSY)FLEXCOM0_USART_WriteIsBusy,
    .intSource = FLEXCOM0_IRQn,
};

const USI_USART_INIT_DATA srvUsi0InitData = {
    .plib = (void*)&srvUsi0InitDataFLEXCOM0,
    .pRdBuffer = (void*)srvUSI0ReadBuffer,
    .rdBufferSize = SRV_USI0_RD_BUF_SIZE,
};

const SRV_USI_INIT srvUSI0Init =
{
    .deviceInitData = (const void*)&srvUsi0InitData,
    .consDevDesc = &srvUSIUSARTDevDesc,
    .deviceIndex = 0,
    .pWrBuffer = srvUSI0WriteBuffer,
    .wrBufferSize = SRV_USI0_WR_BUF_SIZE
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
// <editor-fold defaultstate="collapsed" desc="G3 MAC Wrapper Initialization Data">

/* G3 MAC Wrapper Initialization Data */
SYS_MODULE_INIT g3MacWraperInitData = {
    /* Init data not used, set default field */
    .value = 0,
};

// </editor-fold>
// <editor-fold defaultstate="collapsed" desc="G3 ADP Initialization Data">
/* G3 ADP Buffers and Queues */
ADP_DATA_PARAMS_BUFFER_1280 g3Adp1280Buffers[G3_ADP_NUM_BUFFERS_1280];
ADP_DATA_PARAMS_BUFFER_400 g3Adp400Buffers[G3_ADP_NUM_BUFFERS_400];
ADP_DATA_PARAMS_BUFFER_100 g3Adp100Buffers[G3_ADP_NUM_BUFFERS_100];
ADP_PROCESS_QUEUE_ENTRY g3AdpProcessQueueEntries[G3_ADP_PROCESS_QUEUE_SIZE];
ADP_LOWPAN_FRAGMENTED_DATA g3AdpFragmentedTransferTable[G3_ADP_FRAG_TRANSFER_TABLE_SIZE];

/* G3 ADP Initialization Data */
ADP_INIT g3AdpInitData = {
    /* Pointer to start of 1280-byte buffers */
    .pBuffers1280 = &g3Adp1280Buffers,

    /* Pointer to start of 400-byte buffers */
    .pBuffers400 = &g3Adp400Buffers,

    /* Pointer to start of 100-byte buffers */
    .pBuffers100 = &g3Adp100Buffers,

    /* Pointer to start of process queue entries */
    .pProcessQueueEntries = &g3AdpProcessQueueEntries,

    /* Pointer to start of fragmented transfer entries */
    .pFragmentedTransferEntries = &g3AdpFragmentedTransferTable,

    /* Number of 1280-byte buffers */
    .numBuffers1280 = G3_ADP_NUM_BUFFERS_1280,

    /* Number of 400-byte buffers */
    .numBuffers400 = G3_ADP_NUM_BUFFERS_400,

    /* Number of 100-byte buffers */
    .numBuffers100 = G3_ADP_NUM_BUFFERS_100,

    /* Number of process queue entries */
    .numProcessQueueEntries = G3_ADP_PROCESS_QUEUE_SIZE,

    /* Number of fragmented transfer entries */
    .numFragmentedTransferEntries = G3_ADP_FRAG_TRANSFER_TABLE_SIZE
};

// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="ADP Serialization Initialization Data">

/* G3 ADP Serialization Initialization Data */
SYS_MODULE_INIT g3AdpSerialInitData = {
    /* Init data not used, set default field */
    .value = 0,
};

// </editor-fold>



// *****************************************************************************
// *****************************************************************************
// Section: System Initialization
// *****************************************************************************
// *****************************************************************************
// <editor-fold defaultstate="collapsed" desc="SYS_TIME Initialization Data">

const SYS_TIME_PLIB_INTERFACE sysTimePlibAPI = {
    .timerCallbackSet = (SYS_TIME_PLIB_CALLBACK_REGISTER)TC0_CH0_TimerCallbackRegister,
    .timerStart = (SYS_TIME_PLIB_START)TC0_CH0_TimerStart,
    .timerStop = (SYS_TIME_PLIB_STOP)TC0_CH0_TimerStop ,
    .timerFrequencyGet = (SYS_TIME_PLIB_FREQUENCY_GET)TC0_CH0_TimerFrequencyGet,
    .timerPeriodSet = (SYS_TIME_PLIB_PERIOD_SET)TC0_CH0_TimerPeriodSet,
    .timerCompareSet = (SYS_TIME_PLIB_COMPARE_SET)TC0_CH0_TimerCompareSet,
    .timerCounterGet = (SYS_TIME_PLIB_COUNTER_GET)TC0_CH0_TimerCounterGet,
};

const SYS_TIME_INIT sysTimeInitData =
{
    .timePlib = &sysTimePlibAPI,
    .hwTimerIntNum = TC0_CH0_IRQn,
};

// </editor-fold>
// <editor-fold defaultstate="collapsed" desc="SYS_CONSOLE Instance 0 Initialization Data">


/* Declared in console device implementation (sys_console_uart.c) */
extern const SYS_CONSOLE_DEV_DESC sysConsoleUARTDevDesc;

const SYS_CONSOLE_UART_PLIB_INTERFACE sysConsole0UARTPlibAPI =
{
    .read = (SYS_CONSOLE_UART_PLIB_READ)FLEXCOM7_USART_Read,
	.readCountGet = (SYS_CONSOLE_UART_PLIB_READ_COUNT_GET)FLEXCOM7_USART_ReadCountGet,
	.readFreeBufferCountGet = (SYS_CONSOLE_UART_PLIB_READ_FREE_BUFFFER_COUNT_GET)FLEXCOM7_USART_ReadFreeBufferCountGet,
    .write = (SYS_CONSOLE_UART_PLIB_WRITE)FLEXCOM7_USART_Write,
	.writeCountGet = (SYS_CONSOLE_UART_PLIB_WRITE_COUNT_GET)FLEXCOM7_USART_WriteCountGet,
	.writeFreeBufferCountGet = (SYS_CONSOLE_UART_PLIB_WRITE_FREE_BUFFER_COUNT_GET)FLEXCOM7_USART_WriteFreeBufferCountGet,
};

const SYS_CONSOLE_UART_INIT_DATA sysConsole0UARTInitData =
{
    .uartPLIB = &sysConsole0UARTPlibAPI,    
};

const SYS_CONSOLE_INIT sysConsole0Init =
{
    .deviceInitData = (const void*)&sysConsole0UARTInitData,
    .consDevDesc = &sysConsoleUARTDevDesc,
    .deviceIndex = 0,
};



// </editor-fold>


const SYS_DEBUG_INIT debugInit =
{
    .moduleInit = {0},
    .errorLevel = SYS_DEBUG_GLOBAL_ERROR_LEVEL,
    .consoleIndex = 0,
};





// *****************************************************************************
// *****************************************************************************
// Section: Local initialization functions
// *****************************************************************************
// *****************************************************************************



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


    SEFC0_Initialize();

    SEFC1_Initialize();
  
    DWDT_Initialize();
    CLK_Initialize();
    RSTC_Initialize();

    PIO_Initialize();
    SUPC_Initialize();




    FLEXCOM7_USART_Initialize();

    FLEXCOM3_SPI_Initialize();

    ADC_Initialize();
    FLEXCOM5_SPI_Initialize();

 
    TC0_CH0_TimerInitialize(); 
     
    
    FLEXCOM0_USART_Initialize();

	BSP_Initialize();

    /* Initialize G3 MAC RT Driver Instance */
    sysObj.drvG3MacRt = DRV_G3_MACRT_Initialize(DRV_G3_MACRT_INDEX, (SYS_MODULE_INIT *)&drvG3MacRtInitData);
    PIO_PinInterruptCallbackRegister((PIO_PIN)DRV_PLC_EXT_INT_PIN, DRV_G3_MACRT_ExternalInterruptHandler, sysObj.drvG3MacRt);

    /* Initialize RF215 Driver Instance */
    sysObj.drvRf215 = DRV_RF215_Initialize(DRV_RF215_INDEX_0, (SYS_MODULE_INIT *)&drvRf215InitData);
    /* Initialize PVDD Monitor Service */
    SRV_PVDDMON_Initialize();
    /* Initialize USI Service Instance 0 */
    sysObj.srvUSI0 = SRV_USI_Initialize(SRV_USI_INDEX_0, (SYS_MODULE_INIT *)&srvUSI0Init);

    sysObj.sysTime = SYS_TIME_Initialize(SYS_TIME_INDEX_0, (SYS_MODULE_INIT *)&sysTimeInitData);
    sysObj.sysConsole0 = SYS_CONSOLE_Initialize(SYS_CONSOLE_INDEX_0, (SYS_MODULE_INIT *)&sysConsole0Init);

    sysObj.sysDebug = SYS_DEBUG_Initialize(SYS_DEBUG_INDEX_0, (SYS_MODULE_INIT*)&debugInit);



    CRYPT_WCCB_Initialize();

    /* Initialize G3 MAC Wrapper Instance */
    sysObj.g3MacWrapper = MAC_WRP_Initialize(G3_MAC_WRP_INDEX_0, &g3MacWraperInitData);

    /* Initialize G3 ADP Instance */
    sysObj.g3Adp = ADP_Initialize(G3_ADP_INDEX_0, (SYS_MODULE_INIT *)&g3AdpInitData);

    /* Initialize G3 ADP Serialization Instance */
    sysObj.g3AdpSerial = ADP_SERIAL_Initialize(G3_ADP_SERIAL_INDEX_0, &g3AdpSerialInitData);


    APP_Initialize();


    NVIC_Initialize();

    /* MISRAC 2012 deviation block end */
}


/*******************************************************************************
 End of File
*/