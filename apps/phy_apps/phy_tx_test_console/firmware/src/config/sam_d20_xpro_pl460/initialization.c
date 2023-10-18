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
#pragma config NVMCTRL_EEPROM_SIZE = SIZE_0BYTES
#pragma config BOD33USERLEVEL = 0x7U // Enter Hexadecimal value
#pragma config BOD33_EN = ENABLED
#pragma config BOD33_ACTION = RESET

#pragma config BOD33_HYST = ENABLED
#pragma config NVMCTRL_REGION_LOCKS = 0xffffU // Enter Hexadecimal value

#pragma config WDT_ENABLE = ENABLED
#pragma config WDT_ALWAYSON = DISABLED
#pragma config WDT_PER = CYC16384

#pragma config WDT_WINDOW_0 = SET
#pragma config WDT_WINDOW_1 = 0x4U // Enter Hexadecimal value
#pragma config WDT_EWOFFSET = CYC16384
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
// <editor-fold defaultstate="collapsed" desc="DRV_PLC_PHY Initialization Data">

/* MISRA C-2012 deviation block start */
/* MISRA C-2012 Rule 8.4 deviated once. Deviation record ID - H3_MISRAC_2012_R_8_4_DR_1 */

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

/* MISRA C-2012 deviation block end */

// </editor-fold>
// <editor-fold defaultstate="collapsed" desc="DRV_MEMORY Instance 0 Initialization Data">

static uint8_t gDrvMemory0EraseBuffer[NVMCTRL_ERASE_BUFFER_SIZE] CACHE_ALIGN;

static DRV_MEMORY_CLIENT_OBJECT gDrvMemory0ClientObject[DRV_MEMORY_CLIENTS_NUMBER_IDX0];

static DRV_MEMORY_BUFFER_OBJECT gDrvMemory0BufferObject[DRV_MEMORY_BUF_Q_SIZE_IDX0];

static const DRV_MEMORY_DEVICE_INTERFACE drvMemory0DeviceAPI = {
    .Open               = DRV_NVMCTRL_Open,
    .Close              = DRV_NVMCTRL_Close,
    .Status             = DRV_NVMCTRL_Status,
    .SectorErase        = DRV_NVMCTRL_SectorErase,
    .Read               = DRV_NVMCTRL_Read,
    .PageWrite          = DRV_NVMCTRL_PageWrite,
    .EventHandlerSet    = NULL,
    .GeometryGet        = (DRV_MEMORY_DEVICE_GEOMETRY_GET)DRV_NVMCTRL_GeometryGet,
    .TransferStatusGet  = (DRV_MEMORY_DEVICE_TRANSFER_STATUS_GET)DRV_NVMCTRL_TransferStatusGet
};
static const DRV_MEMORY_INIT drvMemory0InitData =
{
    .memDevIndex                = 0,
    .memoryDevice               = &drvMemory0DeviceAPI,
    .isMemDevInterruptEnabled   = false,
    .isFsEnabled                = false,
    .ewBuffer                   = &gDrvMemory0EraseBuffer[0],
    .clientObjPool              = (uintptr_t)&gDrvMemory0ClientObject[0],
    .bufferObj                  = (uintptr_t)&gDrvMemory0BufferObject[0],
    .queueSize                  = DRV_MEMORY_BUF_Q_SIZE_IDX0,
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
// <editor-fold defaultstate="collapsed" desc="SYS_CONSOLE Instance 0 Initialization Data">


static const SYS_CONSOLE_UART_PLIB_INTERFACE sysConsole0UARTPlibAPI =
{
    .read_t = (SYS_CONSOLE_UART_PLIB_READ)SERCOM3_USART_Read,
    .readCountGet = (SYS_CONSOLE_UART_PLIB_READ_COUNT_GET)SERCOM3_USART_ReadCountGet,
    .readFreeBufferCountGet = (SYS_CONSOLE_UART_PLIB_READ_FREE_BUFFFER_COUNT_GET)SERCOM3_USART_ReadFreeBufferCountGet,
    .write_t = (SYS_CONSOLE_UART_PLIB_WRITE)SERCOM3_USART_Write,
    .writeCountGet = (SYS_CONSOLE_UART_PLIB_WRITE_COUNT_GET)SERCOM3_USART_WriteCountGet,
    .writeFreeBufferCountGet = (SYS_CONSOLE_UART_PLIB_WRITE_FREE_BUFFER_COUNT_GET)SERCOM3_USART_WriteFreeBufferCountGet,
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

    NVMCTRL_REGS->NVMCTRL_CTRLB = NVMCTRL_CTRLB_RWS(3UL);

  
    PORT_Initialize();

    CLOCK_Initialize();




    SERCOM3_USART_Initialize();

    NVMCTRL_Initialize( );

    SERCOM0_SPI_Initialize();


    ADC_Initialize();
    EIC_Initialize();

    TC0_TimerInitialize();

	BSP_Initialize();


    /* MISRAC 2012 deviation block start */
    /* Following MISRA-C rules deviated in this block  */
    /* MISRA C-2012 Rule 11.3 - Deviation record ID - H3_MISRAC_2012_R_11_3_DR_1 */
    /* MISRA C-2012 Rule 11.8 - Deviation record ID - H3_MISRAC_2012_R_11_8_DR_1 */

    /* Initialize PLC Phy Driver Instance */
    sysObj.drvPlcPhy = DRV_PLC_PHY_Initialize(DRV_PLC_PHY_INDEX, (SYS_MODULE_INIT *)&drvPlcPhyInitData);
    EIC_CallbackRegister(DRV_PLC_EXT_INT_PIN, DRV_PLC_PHY_ExternalInterruptHandler, sysObj.drvPlcPhy);


    sysObj.drvMemory0 = DRV_MEMORY_Initialize((SYS_MODULE_INDEX)DRV_MEMORY_INDEX_0, (SYS_MODULE_INIT *)&drvMemory0InitData);

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


    /* MISRAC 2012 deviation block end */
    APP_PLC_Initialize();
    APP_CONSOLE_Initialize();
    APP_NVM_Initialize();


    NVIC_Initialize();


    /* MISRAC 2012 deviation block end */
}

/*******************************************************************************
 End of File
*/
