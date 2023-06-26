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
#pragma config TCM_CONFIGURATION = 0
#pragma config SECURITY_BIT = CLEAR
#pragma config BOOT_MODE = SET




// *****************************************************************************
// *****************************************************************************
// Section: Driver Initialization Data
// *****************************************************************************
// *****************************************************************************
/* Following MISRA-C rules are deviated in the below code block */
/* MISRA C-2012 Rule 11.1 */
/* MISRA C-2012 Rule 11.3 */
/* MISRA C-2012 Rule 11.8 */
// <editor-fold defaultstate="collapsed" desc="DRV_RF215 Initialization Data">

/* RF215 Driver Initialization Data */
const DRV_RF215_INIT drvRf215InitData = {
    /* SPI Transmit Register */
    .spiTransmitAddress = (const void *)&(SPI0_REGS->SPI_TDR),

    /* SPI Receive Register */
    .spiReceiveAddress = (const void *)&(SPI0_REGS->SPI_RDR),

    /* Pointer to SPI PLIB is busy function */
    .spiPlibIsBusy = SPI0_IsTransmitterBusy,

    /* Pointer to SPI PLIB chip select function */
    .spiPlibSetChipSelect = SPI0_ChipSelectSetup,

    /* Interrupt source ID for DMA */
    .dmaIntSource = XDMAC_IRQn,

    /* Interrupt source ID for SYS_TIME */
    .sysTimeIntSource = TC0_CH0_IRQn,

    /* Interrupt source ID for PLC external interrupt */
    .plcExtIntSource = PIOD_IRQn,

    /* Initial PHY frequency band and operating mode for Sub-GHz transceiver */
    .rf09PhyBandOpmIni = SUN_FSK_BAND_863_OPM1,

    /* Initial PHY frequency channel number for Sub-GHz transceiver */
    .rf09PhyChnNumIni = 29,

    /* Initial PHY frequency band and operating mode for Sub-GHz transceiver */
    .rf24PhyBandOpmIni = SUN_FSK_BAND_2450_OPM1,

    /* Initial PHY frequency channel number for Sub-GHz transceiver */
    .rf24PhyChnNumIni = 0,

};

// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="_on_reset() critical function">
/* This routine should initialize the PL460 control pins as soon as possible */
/* after a power up reset to avoid risks on starting up PL460 device when */ 
/* pull up resistors are configured by default */
void _on_reset(void)
{
    /* Enables PIOA and PIOC */
    PMC_REGS->PMC_PCER0 = PMC_PCER0_PID10_Msk | PMC_PCER0_PID12_Msk;

    /* Enable Reset Pin */
    SYS_PORT_PinOutputEnable(DRV_PLC_RESET_PIN);
    SYS_PORT_PinClear(DRV_PLC_RESET_PIN);
    /* Disable STBY Pin */
    SYS_PORT_PinOutputEnable(DRV_PLC_STBY_PIN);
    SYS_PORT_PinClear(DRV_PLC_STBY_PIN);
    /* Disable LDO Pin */
    SYS_PORT_PinOutputEnable(DRV_PLC_LDO_EN_PIN);
    SYS_PORT_PinClear(DRV_PLC_LDO_EN_PIN);
}
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="DRV_PLC_HAL Initialization Data">
/* HAL Interface Initialization for PLC transceiver */
DRV_PLC_PLIB_INTERFACE drvPLCPlib = {

    /* SPI Transfer Setup */
    .spiPlibTransferSetup = (DRV_PLC_SPI_PLIB_TRANSFER_SETUP)SPI0_TransferSetup,

    /* SPI Is Busy */
    .spiIsBusy = SPI0_IsTransmitterBusy,

    /* SPI Set Chip Select */
    .spiSetChipSelect = SPI0_ChipSelectSetup,

    /* DMA Channel for Transmit */
    .dmaChannelTx = SYS_DMA_CHANNEL_0,

    /* DMA Channel for Receive */
    .dmaChannelRx = SYS_DMA_CHANNEL_1,

    /* SPI Transmit Register */
    .spiAddressTx = (void *)&(SPI0_REGS->SPI_TDR),

    /* SPI Receive Register */
    .spiAddressRx  = (void *)&(SPI0_REGS->SPI_RDR),

    
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
    
    /* Interrupt source ID for RF external interrupt */
    .rfExtIntSource = PIOA_IRQn,

    /* Interrupt source ID for DMA */
    .dmaIntSource = XDMAC_IRQn,

    /* Interrupt source ID for SYS_TIME */
    .sysTimeIntSource = TC0_CH0_IRQn,

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
// <editor-fold defaultstate="collapsed" desc="SRV_USI Instance 1 Initialization Data">

static uint8_t CACHE_ALIGN srvUSI1ReadBuffer[SRV_USI1_RD_BUF_SIZE] = {0};
static uint8_t CACHE_ALIGN srvUSI1WriteBuffer[SRV_USI1_WR_BUF_SIZE] = {0};

/* Declared in USI USART service implementation (srv_usi_usart.c) */
extern const SRV_USI_DEV_DESC srvUSIUSARTDevDesc;

const SRV_USI_USART_INTERFACE srvUsi1InitDataUSART1 = {
    .readCallbackRegister = (USI_USART_PLIB_READ_CALLBACK_REG)USART1_ReadCallbackRegister,
    .read = (USI_USART_PLIB_WRRD)USART1_Read,
    .write = (USI_USART_PLIB_WRRD)USART1_Write,
    .writeIsBusy = (USI_USART_PLIB_WRITE_ISBUSY)USART1_WriteIsBusy,
    .intSource = USART1_IRQn,
};

const USI_USART_INIT_DATA srvUsi1InitData = {
    .plib = (void*)&srvUsi1InitDataUSART1,
    .pRdBuffer = (void*)srvUSI1ReadBuffer,
    .rdBufferSize = SRV_USI1_RD_BUF_SIZE,
};

const SRV_USI_INIT srvUSI1Init =
{
    .deviceInitData = (const void*)&srvUsi1InitData,
    .consDevDesc = &srvUSIUSARTDevDesc,
    .deviceIndex = 0,
    .pWrBuffer = srvUSI1WriteBuffer,
    .wrBufferSize = SRV_USI1_WR_BUF_SIZE
};


// </editor-fold>
// <editor-fold defaultstate="collapsed" desc="SRV_USI Instance 0 Initialization Data">

static uint8_t CACHE_ALIGN srvUSI0ReadBuffer[SRV_USI0_RD_BUF_SIZE] = {0};
static uint8_t CACHE_ALIGN srvUSI0WriteBuffer[SRV_USI0_WR_BUF_SIZE] = {0};

static uint8_t CACHE_ALIGN srvUSI0CDCReadBuffer[128] = {0};

/* Declared in USI CDC service implementation (srv_usi_cdc.c) */
extern const SRV_USI_DEV_DESC srvUSICDCDevDesc;

const USI_CDC_INIT_DATA srvUsi0InitData = {
    .cdcInstanceIndex = 0,
    .usiReadBuffer = srvUSI0ReadBuffer,
    .usiBufferSize = SRV_USI0_RD_BUF_SIZE,
    .cdcReadBuffer = srvUSI0CDCReadBuffer,
    .cdcBufferSize = 128
};

const SRV_USI_INIT srvUSI0Init =
{
    .deviceInitData = (const void*)&srvUsi0InitData,
    .consDevDesc = &srvUSICDCDevDesc,
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
/******************************************************
 * USB Driver Initialization
 ******************************************************/
 
/*  When designing a Self-powered USB Device, the application should make sure
    that USB_DEVICE_Attach() function is called only when VBUS is actively powered.
	Therefore, the firmware needs some means to detect when the Host is powering 
	the VBUS. A 5V tolerant I/O pin can be connected to VBUS (through a resistor)
	and can be used to detect when VBUS is high or low. The application can specify
	a VBUS Detect function through the USB Driver Initialize data structure. 
	The USB device stack will periodically call this function. If the VBUS is 
	detected, the USB_DEVICE_EVENT_POWER_DETECTED event is generated. If the VBUS 
	is removed (i.e., the device is physically detached from Host), the USB stack 
	will generate the event USB_DEVICE_EVENT_POWER_REMOVED. The application should 
	call USB_DEVICE_Detach() when VBUS is removed. 
    
    The following are the steps to generate the VBUS_SENSE Function through MHC     
        1) Navigate to MHC->Tools->Pin Configuration and Configure the pin used 
		   as VBUS_SENSE. Set this pin Function as "GPIO" and set as "Input". 
		   Provide a custom name to the pin.
        2) Select the USB Driver Component in MHC Project Graph and enable the  
		   "Enable VBUS Sense" Check-box.     
        3) Specify the custom name of the VBUS SENSE pin in the "VBUS SENSE Pin Name" box.  
*/
	  
	
static DRV_USB_VBUS_LEVEL DRV_USBHSV1_VBUS_Comparator(void)
{
    DRV_USB_VBUS_LEVEL retVal = DRV_USB_VBUS_LEVEL_INVALID;
    if(true == USB_VBUS_SENSE_Get())
    {
        retVal = DRV_USB_VBUS_LEVEL_VALID;
    }
	return (retVal);

}

const DRV_USBHSV1_INIT drvUSBInit =
{
    /* Interrupt Source for USB module */
    .interruptSource = USBHS_IRQn,

    /* System module initialization */
    .moduleInit = {0},

    /* USB Controller to operate as USB Device */
    .operationMode = DRV_USBHSV1_OPMODE_DEVICE,

    /* To operate in USB Normal Mode */
	.operationSpeed = DRV_USBHSV1_DEVICE_SPEEDCONF_LOW_POWER,

    /* Identifies peripheral (PLIB-level) ID */
    .usbID = USBHS_REGS,
	
    /* Function to check for VBUS */
    .vbusComparator = DRV_USBHSV1_VBUS_Comparator
};




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


    EFC_Initialize();
  
    CLOCK_Initialize();
	PIO_Initialize();

    XDMAC_Initialize();



  

 
    TC0_CH0_TimerInitialize(); 
     
    
	BSP_Initialize();
	SPI0_Initialize();

    USART1_Initialize();

	RSWDT_REGS->RSWDT_MR = RSWDT_MR_WDDIS_Msk;	// Disable RSWDT 

	WDT_Initialize();


    AFEC1_Initialize();



    /* MISRAC 2012 deviation block start */
    /* Following MISRA-C rules deviated in this block  */
    /* MISRA C-2012 Rule 11.3 - Deviation record ID - H3_MISRAC_2012_R_11_3_DR_1 */
    /* MISRA C-2012 Rule 11.8 - Deviation record ID - H3_MISRAC_2012_R_11_8_DR_1 */

    /* Initialize RF215 Driver Instance */
    sysObj.drvRf215 = DRV_RF215_Initialize(DRV_RF215_INDEX_0, (SYS_MODULE_INIT *)&drvRf215InitData);
    /* Initialize PLC Phy Driver Instance */
    sysObj.drvPlcPhy = DRV_PLC_PHY_Initialize(DRV_PLC_PHY_INDEX, (SYS_MODULE_INIT *)&drvPlcPhyInitData);
    PIO_PinInterruptCallbackRegister((PIO_PIN)DRV_PLC_EXT_INT_PIN, DRV_PLC_PHY_ExternalInterruptHandler, sysObj.drvPlcPhy);

    /* Initialize PVDD Monitor Service */
    SRV_PVDDMON_Initialize();
    /* Initialize USI Service Instance 1 */
    sysObj.srvUSI1 = SRV_USI_Initialize(SRV_USI_INDEX_1, (SYS_MODULE_INIT *)&srvUSI1Init);
    /* Initialize USI Service Instance 0 */
    sysObj.srvUSI0 = SRV_USI_Initialize(SRV_USI_INDEX_0, (SYS_MODULE_INIT *)&srvUSI0Init);

    /* MISRA C-2012 Rule 11.3, 11.8 deviated below. Deviation record ID -  
    H3_MISRAC_2012_R_11_3_DR_1 & H3_MISRAC_2012_R_11_8_DR_1*/
        
    sysObj.sysTime = SYS_TIME_Initialize(SYS_TIME_INDEX_0, (SYS_MODULE_INIT *)&sysTimeInitData);
    
    /* MISRAC 2012 deviation block end */


    /* Initialize the USB device layer */
    sysObj.usbDevObject0 = USB_DEVICE_Initialize (USB_DEVICE_INDEX_0 , ( SYS_MODULE_INIT* ) & usbDevInitData);


	/* Initialize USB Driver */ 
    sysObj.drvUSBHSV1Object = DRV_USBHSV1_Initialize(DRV_USBHSV1_INDEX_0, (SYS_MODULE_INIT *) &drvUSBInit);	


    /* MISRAC 2012 deviation block end */
    APP_PLC_Initialize();
    APP_RF_Initialize();


    NVIC_Initialize();


    /* MISRAC 2012 deviation block end */
}

/*******************************************************************************
 End of File
*/
