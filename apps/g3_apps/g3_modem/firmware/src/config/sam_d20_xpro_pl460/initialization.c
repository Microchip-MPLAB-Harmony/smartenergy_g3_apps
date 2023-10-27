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
#pragma config NVMCTRL_BOOTPROT = SIZE_0BYTES
#pragma config NVMCTRL_EEPROM_SIZE = SIZE_256BYTES
#pragma config BOD33USERLEVEL = 0x6U // Enter Hexadecimal value
#pragma config BOD33_EN = ENABLED
#pragma config BOD33_ACTION = INT

#pragma config BOD33_HYST = ENABLED
#pragma config NVMCTRL_REGION_LOCKS = 0xffffU // Enter Hexadecimal value

#pragma config WDT_ENABLE = ENABLED
#pragma config WDT_ALWAYSON = DISABLED
#pragma config WDT_PER = CYC16384

#pragma config WDT_WINDOW_0 = SET
#pragma config WDT_WINDOW_1 = 0x4U // Enter Hexadecimal value
#pragma config WDT_EWOFFSET = CYC8192
#pragma config WDT_WEN = DISABLED




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
    /* Disable STBY Pin */
    SYS_PORT_PinOutputEnable(SYS_PORT_PIN_PA08);
    SYS_PORT_PinClear(SYS_PORT_PIN_PA08);
    /* Enable Reset Pin */
    SYS_PORT_PinOutputEnable(DRV_PLC_RESET_PIN);
    SYS_PORT_PinClear(DRV_PLC_RESET_PIN);
    /* Disable LDO Pin */
    SYS_PORT_PinOutputEnable(DRV_PLC_LDO_EN_PIN);
    SYS_PORT_PinClear(DRV_PLC_LDO_EN_PIN);
}

/* MISRA C-2012 deviation block end */

// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="DRV_PLC_HAL Initialization Data">
/* HAL Interface Initialization for PLC transceiver */
static DRV_PLC_PLIB_INTERFACE drvPLCPlib = {

    /* SPI Transfer Setup */
    .spiPlibTransferSetup = (DRV_PLC_SPI_PLIB_TRANSFER_SETUP)SERCOM0_SPI_TransferSetup,

    /* SPI Is Busy */
    .spiIsBusy = SERCOM0_SPI_IsBusy,

    /* SPI Write/Read */
    .spiWriteRead = SERCOM0_SPI_WriteRead,

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
    
    /* PLC StandBy Pin */
    .stByPin = DRV_PLC_STBY_PIN,
    
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

    /* PLC Set StandBy Mode */
    .setStandBy = (DRV_PLC_HAL_SET_STBY)DRV_PLC_HAL_SetStandBy,
    
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
// <editor-fold defaultstate="collapsed" desc="SRV_USI Instance 0 Initialization Data">

static uint8_t CACHE_ALIGN srvUSI0ReadBuffer[SRV_USI0_RD_BUF_SIZE] = {0};
static uint8_t CACHE_ALIGN srvUSI0WriteBuffer[SRV_USI0_WR_BUF_SIZE] = {0};


static const SRV_USI_USART_INTERFACE srvUsi0InitDataSERCOM3 = {
    .readCallbackRegister = (USI_USART_PLIB_READ_CALLBACK_REG)SERCOM3_USART_ReadCallbackRegister,
    .readData = (USI_USART_PLIB_WRRD)SERCOM3_USART_Read,
    .writeData = (USI_USART_PLIB_WRRD)SERCOM3_USART_Write,
    .writeIsBusy = (USI_USART_PLIB_WRITE_ISBUSY)SERCOM3_USART_WriteIsBusy,
    .intSource = SERCOM3_IRQn,
};

static const USI_USART_INIT_DATA srvUsi0InitData = {
    .plib = (void*)&srvUsi0InitDataSERCOM3,
    .pRdBuffer = (void*)srvUSI0ReadBuffer,
    .rdBufferSize = SRV_USI0_RD_BUF_SIZE,
};

/* srvUSIUSARTDevDesc declared in USI USART service implementation (srv_usi_usart.c) */

static const SRV_USI_INIT srvUSI0Init =
{
    .deviceInitData = (const void * const)&srvUsi0InitData,
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
    .timerPeriodSet = (SYS_TIME_PLIB_PERIOD_SET)TC0_Timer32bitPeriodSet,
    .timerCompareSet = (SYS_TIME_PLIB_COMPARE_SET)TC0_Timer32bitCompareSet,
    .timerCounterGet = (SYS_TIME_PLIB_COUNTER_GET)TC0_Timer32bitCounterGet,
};

static const SYS_TIME_INIT sysTimeInitData =
{
    .timePlib = &sysTimePlibAPI,
    .hwTimerIntNum = TC0_IRQn,
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

    NVMCTRL_REGS->NVMCTRL_CTRLB = NVMCTRL_CTRLB_RWS(3UL);

  
    PORT_Initialize();

    CLOCK_Initialize();




    NVMCTRL_Initialize( );

    ADC_Initialize();
    TC0_TimerInitialize();

	BSP_Initialize();
    SERCOM3_USART_Initialize();

    SERCOM0_SPI_Initialize();


    EIC_Initialize();



    /* MISRAC 2012 deviation block start */
    /* Following MISRA-C rules deviated in this block  */
    /* MISRA C-2012 Rule 11.3 - Deviation record ID - H3_MISRAC_2012_R_11_3_DR_1 */
    /* MISRA C-2012 Rule 11.8 - Deviation record ID - H3_MISRAC_2012_R_11_8_DR_1 */

    /* Initialize G3 MAC RT Driver Instance */
    sysObj.drvG3MacRt = DRV_G3_MACRT_Initialize(DRV_G3_MACRT_INDEX, (SYS_MODULE_INIT *)&drvG3MacRtInitData);
    EIC_CallbackRegister(DRV_PLC_EXT_INT_PIN, DRV_G3_MACRT_ExternalInterruptHandler, sysObj.drvG3MacRt);

    /* Initialize PVDD Monitor Service */
    SRV_PVDDMON_Initialize();
    /* Initialize USI Service Instance 0 */
    sysObj.srvUSI0 = SRV_USI_Initialize(SRV_USI_INDEX_0, (SYS_MODULE_INIT *)&srvUSI0Init);

    /* MISRA C-2012 Rule 11.3, 11.8 deviated below. Deviation record ID -  
    H3_MISRAC_2012_R_11_3_DR_1 & H3_MISRAC_2012_R_11_8_DR_1*/
        
    sysObj.sysTime = SYS_TIME_Initialize(SYS_TIME_INDEX_0, (SYS_MODULE_INIT *)&sysTimeInitData);
    
    /* MISRAC 2012 deviation block end */


    /* Initialize G3 MAC Wrapper Instance */
    sysObj.g3MacWrapper = MAC_WRP_Initialize(G3_MAC_WRP_INDEX_0);

    /* Initialize G3 ADP Instance */
    sysObj.g3Adp = ADP_Initialize(G3_ADP_INDEX_0, (SYS_MODULE_INIT *)&g3AdpInitData);

    /* Initialize G3 ADP Serialization Instance */
    sysObj.g3AdpSerial = ADP_SERIAL_Initialize(G3_ADP_SERIAL_INDEX_0);

    CRYPT_WCCB_Initialize();

    /* MISRAC 2012 deviation block end */
    APP_SAMD20_Initialize();


    NVIC_Initialize();


    /* MISRAC 2012 deviation block end */
}

/*******************************************************************************
 End of File
*/
