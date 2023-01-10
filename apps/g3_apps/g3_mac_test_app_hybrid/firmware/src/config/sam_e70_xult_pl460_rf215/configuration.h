/*******************************************************************************
  System Configuration Header

  File Name:
    configuration.h

  Summary:
    Build-time configuration header for the system defined by this project.

  Description:
    An MPLAB Project may have multiple configurations.  This file defines the
    build-time options for a single configuration.

  Remarks:
    This configuration header must not define any prototypes or data
    definitions (or include any files that do).  It only provides macro
    definitions for build-time configuration options

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

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
/*  This section Includes other configuration headers necessary to completely
    define this configuration.
*/

#include "user.h"
#include "device.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: System Configuration
// *****************************************************************************
// *****************************************************************************



// *****************************************************************************
// *****************************************************************************
// Section: System Service Configuration
// *****************************************************************************
// *****************************************************************************
/* TIME System Service Configuration Options */
#define SYS_TIME_INDEX_0                            (0)
#define SYS_TIME_MAX_TIMERS                         (5)
#define SYS_TIME_HW_COUNTER_WIDTH                   (16)
#define SYS_TIME_HW_COUNTER_PERIOD                  (65535U)
#define SYS_TIME_HW_COUNTER_HALF_PERIOD             (SYS_TIME_HW_COUNTER_PERIOD>>1)
#define SYS_TIME_CPU_CLOCK_FREQUENCY                (300000000)
#define SYS_TIME_COMPARE_UPDATE_EXECUTION_CYCLES    (900)

#define SYS_CONSOLE_INDEX_0                       0

/* RX buffer size has one additional element for the empty spot needed in circular buffer */
#define SYS_CONSOLE_USB_CDC_RD_BUFFER_SIZE_IDX0    1025

/* TX buffer size has one additional element for the empty spot needed in circular buffer */
#define SYS_CONSOLE_USB_CDC_WR_BUFFER_SIZE_IDX0    1025



#define SYS_DEBUG_ENABLE
#define SYS_DEBUG_GLOBAL_ERROR_LEVEL       SYS_ERROR_DEBUG
#define SYS_DEBUG_BUFFER_DMA_READY


#define SYS_CONSOLE_DEVICE_MAX_INSTANCES   			1
#define SYS_CONSOLE_UART_MAX_INSTANCES 	   			0
#define SYS_CONSOLE_USB_CDC_MAX_INSTANCES 	   		1
#define SYS_CONSOLE_PRINT_BUFFER_SIZE        		2048

#define SYS_CONSOLE_USB_CDC_READ_WRITE_BUFFER_SIZE 	64



// *****************************************************************************
// *****************************************************************************
// Section: Driver Configuration
// *****************************************************************************
// *****************************************************************************
/* PAL PLC Configuration Options */
#define PAL_PLC_PHY_INDEX                     0

/* PLC MAC RT Configuration Options */
#define DRV_PLC_SECURE                        false
#define DRV_PLC_EXT_INT_PIO_PORT              PIO_PORT_D
#define DRV_PLC_EXT_INT_SRC                   PIOD_IRQn
#define DRV_PLC_EXT_INT_PIN                   SYS_PORT_PIN_PD28
#define DRV_PLC_RESET_PIN                     SYS_PORT_PIN_PA0
#define DRV_PLC_LDO_EN_PIN                    SYS_PORT_PIN_PC30
#define DRV_PLC_TX_ENABLE_PIN                 SYS_PORT_PIN_PA4
#define DRV_PLC_STBY_PIN                      SYS_PORT_PIN_PA3
#define DRV_PLC_THMON_PIN                     SYS_PORT_PIN_PC17
#define DRV_PLC_CSR_INDEX                     1
#define DRV_PLC_SPI_CHIP_SELECT               SPI_CHIP_SELECT_NPCS1

#define DRV_PLC_SPI_CLK                      8000000

/* PLC MAC RT Driver Identification */
#define DRV_G3_MACRT_INDEX                   0
#define DRV_G3_MACRT_INSTANCES_NUMBER        1
#define DRV_G3_MACRT_HOST_DESC               "ATSAME70Q21B"
/* RF215 Driver Configuration Options */
#define DRV_RF215_INDEX_0                     0
#define DRV_RF215_CLIENTS_NUMBER              1
#define DRV_RF215_TX_BUFFERS_NUMBER           1
#define DRV_RF215_CSR_INDEX                   3
#define DRV_RF215_SPI_CHIP_SELECT             SPI_CHIP_SELECT_NPCS3
#define DRV_RF215_SPI_TX_DMA_CH               SYS_DMA_CHANNEL_0
#define DRV_RF215_SPI_RX_DMA_CH               SYS_DMA_CHANNEL_1
#define DRV_RF215_EXT_INT_PIN                 SYS_PORT_PIN_PA2
#define DRV_RF215_RESET_PIN                   SYS_PORT_PIN_PC19
#define DRV_RF215_LED_TX_PIN                  SYS_PORT_PIN_PD11
#define DRV_RF215_LED_RX_PIN                  SYS_PORT_PIN_PA6
#define DRV_RF215_NUM_TRX                     1
#define DRV_RF215_FCS_LEN                     4
#define DRV_RF215_MAX_PSDU_LEN                576
#define DRV_RF215_MAX_TX_TIME_DELAY_ERROR_US  9000
#define DRV_RF215_TIME_SYNC_EXECUTION_CYCLES  70
#define DRV_RF215_TX_COMMAND_EXECUTION_CYCLES 660

/* PAL RF Configuration Options */
#define PAL_RF_PHY_INDEX                     0



// *****************************************************************************
// *****************************************************************************
// Section: Middleware & Other Library Configuration
// *****************************************************************************
// *****************************************************************************
/* Number of Endpoints used */
#define DRV_USBHSV1_ENDPOINTS_NUMBER                        4

/* The USB Device Layer will not initialize the USB Driver */
#define USB_DEVICE_DRIVER_INITIALIZE_EXPLICIT

/* Maximum device layer instances */
#define USB_DEVICE_INSTANCES_NUMBER                         1

/* EP0 size in bytes */
#define USB_DEVICE_EP0_BUFFER_SIZE                          64


/* Maximum instances of CDC function driver */
#define USB_DEVICE_CDC_INSTANCES_NUMBER                     1


/* CDC Transfer Queue Size for both read and
   write. Applicable to all instances of the
   function driver */
#define USB_DEVICE_CDC_QUEUE_DEPTH_COMBINED                 3


/*** wolfCrypt Library Configuration ***/
#define MICROCHIP_PIC32
#define MICROCHIP_MPLAB_HARMONY
#define MICROCHIP_MPLAB_HARMONY_3
#define HAVE_MCAPI
#define SIZEOF_LONG_LONG 8
#define WOLFSSL_USER_IO
#define NO_WRITEV
#define NO_FILESYSTEM
#define USE_FAST_MATH
#define NO_PWDBASED
#define HAVE_MCAPI
#define WOLF_CRYPTO_CB  // provide call-back support
#define WOLFCRYPT_ONLY
// ---------- FUNCTIONAL CONFIGURATION START ----------
#define WOLFSSL_AES_SMALL_TABLES
#define NO_MD4
#define WOLFSSL_SHA224
#define WOLFSSL_AES_128
#define WOLFSSL_AES_192
#define WOLFSSL_AES_256
#define WOLFSSL_AES_DIRECT
#define HAVE_AES_DECRYPT
#define HAVE_AES_ECB
#define HAVE_AES_CBC
#define WOLFSSL_AES_COUNTER
#define WOLFSSL_AES_OFB
#define HAVE_AESGCM
#define HAVE_AESCCM
#define WOLFSSL_CMAC
#define NO_RC4
#define NO_HC128
#define NO_RABBIT
#define HAVE_ECC
#define NO_DH
#define NO_DSA
#define FP_MAX_BITS 4096
#define USE_CERT_BUFFERS_2048
#define NO_DEV_RANDOM
#define HAVE_HASHDRBG
#define TFM_TIMING_RESISTANT
#define ECC_TIMING_RESISTANT
#define WC_RSA_BLINDING
#define SINGLE_THREADED
#define NO_SIG_WRAPPER
#define NO_ERROR_STRINGS
#define NO_WOLFSSL_MEMORY
// ---------- FUNCTIONAL CONFIGURATION END ----------

/*** USB Driver Configuration ***/

/* Maximum USB driver instances */
#define DRV_USBHSV1_INSTANCES_NUMBER                        1

/* Interrupt mode enabled */
#define DRV_USBHSV1_INTERRUPT_MODE                          true

/* Enables Device Support */
#define DRV_USBHSV1_DEVICE_SUPPORT                          true
	
/* Disable Host Support */
#define DRV_USBHSV1_HOST_SUPPORT                            false

/* Alignment for buffers that are submitted to USB Driver*/ 
#define USB_ALIGN  CACHE_ALIGN


/* MAC COMMON Identification */
#define G3_MAC_COMMON_INDEX_0            0
#define G3_MAC_COMMON_INSTANCES_NUMBER   1

/* MAC PLC Identification */
#define G3_MAC_PLC_INDEX_0               0
#define G3_MAC_PLC_INSTANCES_NUMBER      1

/* MAC RF Identification */
#define G3_MAC_RF_INDEX_0                0
#define G3_MAC_RF_INSTANCES_NUMBER       1

/* MAC Wrapper Identification */
#define G3_MAC_WRP_INDEX_0               0
#define G3_MAC_WRP_INSTANCES_NUMBER      1



// *****************************************************************************
// *****************************************************************************
// Section: Application Configuration
// *****************************************************************************
// *****************************************************************************


//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // CONFIGURATION_H
/*******************************************************************************
 End of File
*/
