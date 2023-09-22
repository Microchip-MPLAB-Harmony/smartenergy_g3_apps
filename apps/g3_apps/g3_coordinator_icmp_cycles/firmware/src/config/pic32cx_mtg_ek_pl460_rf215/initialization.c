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
/* Following MISRA-C rules are deviated in the below code block */
/* MISRA C-2012 Rule 11.1 */
/* MISRA C-2012 Rule 11.3 */
/* MISRA C-2012 Rule 11.8 */

// <editor-fold defaultstate="collapsed" desc="_on_reset() critical function">


/* MISRA C-2012 deviation block start */
/* MISRA C-2012 Rule 8.4 deviated once. Deviation record ID - H3_MISRAC_2012_R_8_4_DR_1 */
/* MISRA C-2012 Rule 21.2 deviated once. Deviation record ID - H3_MISRAC_2012_R_21_2_DR_1 */

/* This routine must initialize the PL460 control pins as soon as possible */
/* after a power up reset to avoid risks on starting up PL460 device when */
/* pull up resistors are configured by default */
void _on_reset(void)
{
    PMC_REGS->PMC_PCR = PMC_PCR_CMD_Msk | PMC_PCR_EN_Msk | PMC_PCR_PID(ID_PIOA);
    while((PMC_REGS->PMC_CSR0 & PMC_CSR0_PID17_Msk) == 0U)
    {
        /* Wait for clock to be initialized */
    }
    /* Disable STBY Pin */
    SYS_PORT_PinOutputEnable(SYS_PORT_PIN_PA0);
    SYS_PORT_PinClear(SYS_PORT_PIN_PA0);
}

/* MISRA C-2012 deviation block end */

// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="DRV_PLC_HAL Initialization Data">
/* HAL Interface Initialization for PLC transceiver */
static DRV_PLC_PLIB_INTERFACE drvPLCPlib = {

    /* SPI Transfer Setup */
    .spiPlibTransferSetup = (DRV_PLC_SPI_PLIB_TRANSFER_SETUP)FLEXCOM5_SPI_TransferSetup,

    /* SPI Is Busy */
    .spiIsBusy = FLEXCOM5_SPI_IsTransmitterBusy,

    /* SPI Write/Read */
    .spiWriteRead = FLEXCOM5_SPI_WriteRead,


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

    /* PLC External Interrupt Pin */
    .thMonPin = DRV_PLC_THMON_PIN,

};

/* HAL Interface Initialization for PLC transceiver */
static DRV_PLC_HAL_INTERFACE drvPLCHalAPI = {

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

/* MISRA C-2012 deviation block start */
/* MISRA C-2012 Rule 8.4 deviated once. Deviation record ID - H3_MISRAC_2012_R_8_4_DR_1 */

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

/* MISRA C-2012 deviation block end */

// </editor-fold>
// <editor-fold defaultstate="collapsed" desc="DRV_RF215 Initialization Data">

/* RF215 Driver Initialization Data */
static const DRV_RF215_INIT drvRf215InitData = {
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
    .rf09PhyChnNumIni = 29,

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
// <editor-fold defaultstate="collapsed" desc="G3 ADP Initialization Data">
/* G3 ADP Buffers and Queues */
static ADP_DATA_PARAMS_BUFFER_1280 g3Adp1280Buffers[G3_ADP_NUM_BUFFERS_1280];
static ADP_DATA_PARAMS_BUFFER_400 g3Adp400Buffers[G3_ADP_NUM_BUFFERS_400];
static ADP_DATA_PARAMS_BUFFER_100 g3Adp100Buffers[G3_ADP_NUM_BUFFERS_100];
static ADP_PROCESS_QUEUE_ENTRY g3AdpProcessQueueEntries[G3_ADP_PROCESS_QUEUE_SIZE];
static ADP_LOWPAN_FRAGMENTED_DATA g3AdpFragmentedTransferTable[G3_ADP_FRAG_TRANSFER_TABLE_SIZE];

/* G3 ADP Initialization Data */
static ADP_INIT g3AdpInitData = {
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

    /* ADP fragmentation size */
    .fragmentSize = G3_ADP_FRAGMENT_SIZE,

    /* Number of 1280-byte buffers */
    .numBuffers1280 = G3_ADP_NUM_BUFFERS_1280,

    /* Number of 400-byte buffers */
    .numBuffers400 = G3_ADP_NUM_BUFFERS_400,

    /* Number of 100-byte buffers */
    .numBuffers100 = G3_ADP_NUM_BUFFERS_100,

    /* Number of process queue entries */
    .numProcessQueueEntries = G3_ADP_PROCESS_QUEUE_SIZE,

    /* Number of fragmented transfer entries */
    .numFragmentedTransferEntries = G3_ADP_FRAG_TRANSFER_TABLE_SIZE,

    /* ADP task rate in milliseconds */
    .taskRateMs = G3_STACK_TASK_RATE_MS

};

// </editor-fold>


// <editor-fold defaultstate="collapsed" desc="TCP/IP Stack Initialization Data">
// *****************************************************************************
// *****************************************************************************
// Section: TCPIP Data
// *****************************************************************************
// *****************************************************************************

























/*** IPv6 Initialization Data ***/
const TCPIP_IPV6_MODULE_CONFIG  tcpipIPv6InitData =
{
    .rxfragmentBufSize      = TCPIP_IPV6_RX_FRAGMENTED_BUFFER_SIZE,
    .fragmentPktRxTimeout   = TCPIP_IPV6_FRAGMENT_PKT_TIMEOUT,
};








TCPIP_STACK_HEAP_INTERNAL_CONFIG tcpipHeapConfig =
{
    .heapType = TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP,
    .heapFlags = TCPIP_STACK_HEAP_USE_FLAGS,
    .heapUsage = TCPIP_STACK_HEAP_USAGE_CONFIG,
    .malloc_fnc = TCPIP_STACK_MALLOC_FUNC,
    .free_fnc = TCPIP_STACK_FREE_FUNC,
    .heapSize = TCPIP_STACK_DRAM_SIZE,
};


const TCPIP_NETWORK_CONFIG __attribute__((unused))  TCPIP_HOSTS_CONFIGURATION[] =
{
    /*** Network Configuration Index 0 ***/
    {
        .interface = TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX0,
        .hostName = TCPIP_NETWORK_DEFAULT_HOST_NAME_IDX0,
        .macAddr = TCPIP_NETWORK_DEFAULT_MAC_ADDR_IDX0,
        .ipAddr = TCPIP_NETWORK_DEFAULT_IP_ADDRESS_IDX0,
        .ipMask = TCPIP_NETWORK_DEFAULT_IP_MASK_IDX0,
        .gateway = TCPIP_NETWORK_DEFAULT_GATEWAY_IDX0,
        .priDNS = TCPIP_NETWORK_DEFAULT_DNS_IDX0,
        .secondDNS = TCPIP_NETWORK_DEFAULT_SECOND_DNS_IDX0,
        .powerMode = TCPIP_NETWORK_DEFAULT_POWER_MODE_IDX0,
        .startFlags = TCPIP_NETWORK_DEFAULT_INTERFACE_FLAGS_IDX0,
        .pMacObject = &TCPIP_NETWORK_DEFAULT_MAC_DRIVER_IDX0,
    },
};

const size_t TCPIP_HOSTS_CONFIGURATION_SIZE = sizeof (TCPIP_HOSTS_CONFIGURATION) / sizeof (*TCPIP_HOSTS_CONFIGURATION);

const TCPIP_STACK_MODULE_CONFIG TCPIP_STACK_MODULE_CONFIG_TBL [] =
{


    {TCPIP_MODULE_IPV6,             &tcpipIPv6InitData},            // TCPIP_MODULE_IPV6
    {TCPIP_MODULE_ICMPV6,           0},                             // TCPIP_MODULE_ICMPV6
    {TCPIP_MODULE_NDP,              0},                             // TCPIP_MODULE_NDP

    { TCPIP_MODULE_MANAGER,         &tcpipHeapConfig },             // TCPIP_MODULE_MANAGER

// MAC modules

    {TCPIP_MODULE_MAC_G3ADP,        0},                             // TCPIP_MODULE_MAC_G3ADP
};

const size_t TCPIP_STACK_MODULE_CONFIG_TBL_SIZE = sizeof (TCPIP_STACK_MODULE_CONFIG_TBL) / sizeof (*TCPIP_STACK_MODULE_CONFIG_TBL);
/*********************************************************************
 * Function:        SYS_MODULE_OBJ TCPIP_STACK_Init()
 *
 * PreCondition:    None
 *
 * Input:
 *
 * Output:          valid system module object if Stack and its componets are initialized
 *                  SYS_MODULE_OBJ_INVALID otherwise
 *
 * Overview:        The function starts the initialization of the stack.
 *                  If an error occurs, the SYS_ERROR() is called
 *                  and the function de-initialize itself and will return false.
 *
 * Side Effects:    None
 *
 * Note:            This function must be called before any of the
 *                  stack or its component routines are used.
 *
 ********************************************************************/


SYS_MODULE_OBJ TCPIP_STACK_Init(void)
{
    TCPIP_STACK_INIT    tcpipInit;

    tcpipInit.pNetConf = TCPIP_HOSTS_CONFIGURATION;
    tcpipInit.nNets = TCPIP_HOSTS_CONFIGURATION_SIZE;
    tcpipInit.pModConfig = TCPIP_STACK_MODULE_CONFIG_TBL;
    tcpipInit.nModules = TCPIP_STACK_MODULE_CONFIG_TBL_SIZE;
    tcpipInit.initCback = 0;

    return TCPIP_STACK_Initialize(0, &tcpipInit.moduleInit);
}
// </editor-fold>



// *****************************************************************************
// *****************************************************************************
// Section: System Initialization
// *****************************************************************************
// *****************************************************************************
// <editor-fold defaultstate="collapsed" desc="SYS_TIME Initialization Data">

static const SYS_TIME_PLIB_INTERFACE sysTimePlibAPI = {
    .timerCallbackSet = (SYS_TIME_PLIB_CALLBACK_REGISTER)TC0_CH0_TimerCallbackRegister,
    .timerStart = (SYS_TIME_PLIB_START)TC0_CH0_TimerStart,
    .timerStop = (SYS_TIME_PLIB_STOP)TC0_CH0_TimerStop ,
    .timerFrequencyGet = (SYS_TIME_PLIB_FREQUENCY_GET)TC0_CH0_TimerFrequencyGet,
    .timerPeriodSet = (SYS_TIME_PLIB_PERIOD_SET)TC0_CH0_TimerPeriodSet,
    .timerCompareSet = (SYS_TIME_PLIB_COMPARE_SET)TC0_CH0_TimerCompareSet,
    .timerCounterGet = (SYS_TIME_PLIB_COUNTER_GET)TC0_CH0_TimerCounterGet,
};

static const SYS_TIME_INIT sysTimeInitData =
{
    .timePlib = &sysTimePlibAPI,
    .hwTimerIntNum = TC0_CH0_IRQn,
};

// </editor-fold>
// <editor-fold defaultstate="collapsed" desc="SYS_CONSOLE Instance 0 Initialization Data">


static const SYS_CONSOLE_UART_PLIB_INTERFACE sysConsole0UARTPlibAPI =
{
    .read_t = (SYS_CONSOLE_UART_PLIB_READ)FLEXCOM0_USART_Read,
    .readCountGet = (SYS_CONSOLE_UART_PLIB_READ_COUNT_GET)FLEXCOM0_USART_ReadCountGet,
    .readFreeBufferCountGet = (SYS_CONSOLE_UART_PLIB_READ_FREE_BUFFFER_COUNT_GET)FLEXCOM0_USART_ReadFreeBufferCountGet,
    .write_t = (SYS_CONSOLE_UART_PLIB_WRITE)FLEXCOM0_USART_Write,
    .writeCountGet = (SYS_CONSOLE_UART_PLIB_WRITE_COUNT_GET)FLEXCOM0_USART_WriteCountGet,
    .writeFreeBufferCountGet = (SYS_CONSOLE_UART_PLIB_WRITE_FREE_BUFFER_COUNT_GET)FLEXCOM0_USART_WriteFreeBufferCountGet,
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


static const SYS_DEBUG_INIT debugInit =
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


    SEFC0_Initialize();

    SEFC1_Initialize();

    DWDT_Initialize();
    CLK_Initialize();
    RSTC_Initialize();

    PIO_Initialize();
    SUPC_Initialize();




    FLEXCOM3_SPI_Initialize();

    ADC_Initialize();
    FLEXCOM5_SPI_Initialize();


    TC0_CH0_TimerInitialize();


    FLEXCOM0_USART_Initialize();

	BSP_Initialize();


    /* MISRAC 2012 deviation block start */
    /* Following MISRA-C rules deviated in this block  */
    /* MISRA C-2012 Rule 11.3 - Deviation record ID - H3_MISRAC_2012_R_11_3_DR_1 */
    /* MISRA C-2012 Rule 11.8 - Deviation record ID - H3_MISRAC_2012_R_11_8_DR_1 */

    /* Initialize G3 MAC RT Driver Instance */
    sysObj.drvG3MacRt = DRV_G3_MACRT_Initialize(DRV_G3_MACRT_INDEX, (SYS_MODULE_INIT *)&drvG3MacRtInitData);
    (void) PIO_PinInterruptCallbackRegister((PIO_PIN)DRV_PLC_EXT_INT_PIN, DRV_G3_MACRT_ExternalInterruptHandler, sysObj.drvG3MacRt);

    /* Initialize RF215 Driver Instance */
    sysObj.drvRf215 = DRV_RF215_Initialize(DRV_RF215_INDEX_0, (SYS_MODULE_INIT *)&drvRf215InitData);
    /* Initialize PVDD Monitor Service */
    SRV_PVDDMON_Initialize();

    /* MISRA C-2012 Rule 11.3, 11.8 deviated below. Deviation record ID -
    H3_MISRAC_2012_R_11_3_DR_1 & H3_MISRAC_2012_R_11_8_DR_1*/

    sysObj.sysTime = SYS_TIME_Initialize(SYS_TIME_INDEX_0, (SYS_MODULE_INIT *)&sysTimeInitData);

    /* MISRAC 2012 deviation block end */
    /* MISRA C-2012 Rule 11.3, 11.8 deviated below. Deviation record ID -
     H3_MISRAC_2012_R_11_3_DR_1 & H3_MISRAC_2012_R_11_8_DR_1*/
        sysObj.sysConsole0 = SYS_CONSOLE_Initialize(SYS_CONSOLE_INDEX_0, (SYS_MODULE_INIT *)&sysConsole0Init);
   /* MISRAC 2012 deviation block end */
    /* MISRA C-2012 Rule 11.3, 11.8 deviated below. Deviation record ID -
     H3_MISRAC_2012_R_11_3_DR_1 & H3_MISRAC_2012_R_11_8_DR_1*/

    sysObj.sysDebug = SYS_DEBUG_Initialize(SYS_DEBUG_INDEX_0, (SYS_MODULE_INIT*)&debugInit);

    /* MISRAC 2012 deviation block end */


    /* Initialize G3 MAC Wrapper Instance */
    sysObj.g3MacWrapper = MAC_WRP_Initialize(G3_MAC_WRP_INDEX_0);

    /* Initialize G3 ADP Instance */
    sysObj.g3Adp = ADP_Initialize(G3_ADP_INDEX_0, (SYS_MODULE_INIT *)&g3AdpInitData);


   /* TCPIP Stack Initialization */
   sysObj.tcpip = TCPIP_STACK_Init();
   SYS_ASSERT(sysObj.tcpip != SYS_MODULE_OBJ_INVALID, "TCPIP_STACK_Init Failed" );


    CRYPT_WCCB_Initialize();

    /* MISRAC 2012 deviation block end */
    APP_G3_MANAGEMENT_Initialize();
    APP_EAP_SERVER_Initialize();
    APP_STORAGE_PIC32CXMT_Initialize();
    APP_CYCLES_Initialize();
    APP_TCPIP_MANAGEMENT_Initialize();


    NVIC_Initialize();


    /* MISRAC 2012 deviation block end */
}

/*******************************************************************************
 End of File
*/
