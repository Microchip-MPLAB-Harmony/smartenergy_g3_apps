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
// <editor-fold defaultstate="collapsed" desc="DRV_SST26 Initialization Data">

const DRV_SST26_PLIB_INTERFACE drvSST26PlibAPI = {
    .CommandWrite   = QSPI_CommandWrite,
    .RegisterRead   = QSPI_RegisterRead,
    .RegisterWrite  = QSPI_RegisterWrite,
    .MemoryRead     = QSPI_MemoryRead,
    .MemoryWrite    = QSPI_MemoryWrite
};

const DRV_SST26_INIT drvSST26InitData =
{
    .sst26Plib      = &drvSST26PlibAPI,
};
// </editor-fold>

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

    /* PLC HAL delay function */
    .delay = (DRV_PLC_HAL_DELAY)DRV_PLC_HAL_Delay,

    /* PLC HAL Transfer Bootloader Command */
    .sendBootCmd = (DRV_PLC_HAL_SEND_BOOT_CMD)DRV_PLC_HAL_SendBootCmd,

    /* PLC HAL Transfer Write/Read Command */
    .sendWrRdCmd = (DRV_PLC_HAL_SEND_WRRD_CMD)DRV_PLC_HAL_SendWrRdCmd,
};

// </editor-fold>
// <editor-fold defaultstate="collapsed" desc="DRV_PLC_PHY Initialization Data">

/* PLC Binary file addressing */
extern uint8_t plc_phy_bin_start;
extern uint8_t plc_phy_bin_end;
extern uint8_t plc_phy_bin2_start;
extern uint8_t plc_phy_bin2_end;

/* PLC Driver Initialization Data */
DRV_PLC_PHY_INIT drvPlcPhyInitData = {

    /* SPI PLIB API  interface*/
    .plcHal = &drvPLCHalAPI,

    /* PLC PHY Number of clients */
    .numClients = DRV_PLC_PHY_CLIENTS_NUMBER_IDX,  

    /* PLC PHY profile */
    .plcProfile = DRV_PLC_PHY_PROFILE,
 
    /* PLC Binary start address */
    .binStartAddress = (uint32_t)&plc_phy_bin_start,
    
    /* PLC Binary end address */
    .binEndAddress = (uint32_t)&plc_phy_bin_end,

    /* Secure Mode */
    .secure = DRV_PLC_SECURE,
    
};

// </editor-fold>
// <editor-fold defaultstate="collapsed" desc="DRV_MEMORY Instance 0 Initialization Data">

static uint8_t gDrvMemory0EraseBuffer[DRV_SST26_ERASE_BUFFER_SIZE] CACHE_ALIGN;

static DRV_MEMORY_CLIENT_OBJECT gDrvMemory0ClientObject[DRV_MEMORY_CLIENTS_NUMBER_IDX0];

static DRV_MEMORY_BUFFER_OBJECT gDrvMemory0BufferObject[DRV_MEMORY_BUFFER_QUEUE_SIZE_IDX0];

const DRV_MEMORY_DEVICE_INTERFACE drvMemory0DeviceAPI = {
    .Open               = DRV_SST26_Open,
    .Close              = DRV_SST26_Close,
    .Status             = DRV_SST26_Status,
    .SectorErase        = DRV_SST26_SectorErase,
    .Read               = DRV_SST26_Read,
    .PageWrite          = DRV_SST26_PageWrite,
    .EventHandlerSet    = NULL,
    .GeometryGet        = (DRV_MEMORY_DEVICE_GEOMETRY_GET)DRV_SST26_GeometryGet,
    .TransferStatusGet  = (DRV_MEMORY_DEVICE_TRANSFER_STATUS_GET)DRV_SST26_TransferStatusGet
};

const DRV_MEMORY_INIT drvMemory0InitData =
{
    .memDevIndex                = DRV_SST26_INDEX,
    .memoryDevice               = &drvMemory0DeviceAPI,
    .isMemDevInterruptEnabled   = false,
    .isFsEnabled                = false,
    .ewBuffer                   = &gDrvMemory0EraseBuffer[0],
    .clientObjPool              = (uintptr_t)&gDrvMemory0ClientObject[0],
    .bufferObj                  = (uintptr_t)&gDrvMemory0BufferObject[0],
    .queueSize                  = DRV_MEMORY_BUFFER_QUEUE_SIZE_IDX0,
    .nClientsMax                = DRV_MEMORY_CLIENTS_NUMBER_IDX0
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
    .read = (SYS_CONSOLE_UART_PLIB_READ)FLEXCOM0_USART_Read,
	.readCountGet = (SYS_CONSOLE_UART_PLIB_READ_COUNT_GET)FLEXCOM0_USART_ReadCountGet,
	.readFreeBufferCountGet = (SYS_CONSOLE_UART_PLIB_READ_FREE_BUFFFER_COUNT_GET)FLEXCOM0_USART_ReadFreeBufferCountGet,
    .write = (SYS_CONSOLE_UART_PLIB_WRITE)FLEXCOM0_USART_Write,
	.writeCountGet = (SYS_CONSOLE_UART_PLIB_WRITE_COUNT_GET)FLEXCOM0_USART_WriteCountGet,
	.writeFreeBufferCountGet = (SYS_CONSOLE_UART_PLIB_WRITE_FREE_BUFFER_COUNT_GET)FLEXCOM0_USART_WriteFreeBufferCountGet,
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




	BSP_Initialize();
    ADC_Initialize();
    FLEXCOM5_SPI_Initialize();

 
    TC0_CH0_TimerInitialize(); 
     
    
    FLEXCOM0_USART_Initialize();

    QSPI_Initialize();

    sysObj.drvSST26 = DRV_SST26_Initialize((SYS_MODULE_INDEX)DRV_SST26_INDEX, (SYS_MODULE_INIT *)&drvSST26InitData);

    /* Initialize PLC Phy Driver Instance */
    sysObj.drvPlcPhy = DRV_PLC_PHY_Initialize(DRV_PLC_PHY_INDEX, (SYS_MODULE_INIT *)&drvPlcPhyInitData);
    PIO_PinInterruptCallbackRegister((PIO_PIN)DRV_PLC_EXT_INT_PIN, DRV_PLC_PHY_ExternalInterruptHandler, sysObj.drvPlcPhy);

    sysObj.drvMemory0 = DRV_MEMORY_Initialize((SYS_MODULE_INDEX)DRV_MEMORY_INDEX_0, (SYS_MODULE_INIT *)&drvMemory0InitData);

    /* Initialize PVDD Monitor Service */
    SRV_PVDDMON_Initialize();

    sysObj.sysTime = SYS_TIME_Initialize(SYS_TIME_INDEX_0, (SYS_MODULE_INIT *)&sysTimeInitData);
    sysObj.sysConsole0 = SYS_CONSOLE_Initialize(SYS_CONSOLE_INDEX_0, (SYS_MODULE_INIT *)&sysConsole0Init);



    APP_PLC_Initialize();
    APP_CONSOLE_Initialize();
    APP_NVM_Initialize();


    NVIC_Initialize();

    /* MISRAC 2012 deviation block end */
}


/*******************************************************************************
 End of File
*/