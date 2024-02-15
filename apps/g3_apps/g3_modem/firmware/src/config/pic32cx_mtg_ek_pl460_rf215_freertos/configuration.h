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
#define SYS_TIME_MAX_TIMERS                         (25)
#define SYS_TIME_HW_COUNTER_WIDTH                   (32)
#define SYS_TIME_HW_COUNTER_PERIOD                  (4294967295U)
#define SYS_TIME_HW_COUNTER_HALF_PERIOD             (SYS_TIME_HW_COUNTER_PERIOD>>1)
#define SYS_TIME_CPU_CLOCK_FREQUENCY                (200000000)
#define SYS_TIME_COMPARE_UPDATE_EXECUTION_CYCLES    (232)



// *****************************************************************************
// *****************************************************************************
// Section: Driver Configuration
// *****************************************************************************
// *****************************************************************************
/* PAL PLC Configuration Options */
#define PAL_PLC_PHY_INDEX                     0U


/* PLC MAC RT Configuration Options */
#define DRV_PLC_SECURE                        false
#define DRV_PLC_EXT_INT_PIO_PORT              PIO_PORT_A
#define DRV_PLC_EXT_INT_SRC                   PIOA_IRQn
#define DRV_PLC_EXT_INT_PIO                   SYS_PORT_PIN_PA2
#define DRV_PLC_EXT_INT_PIN                   SYS_PORT_PIN_PA2
#define DRV_PLC_RESET_PIN                     SYS_PORT_PIN_PD15
#define DRV_PLC_LDO_EN_PIN                    SYS_PORT_PIN_PD19
#define DRV_PLC_TX_ENABLE_PIN                 SYS_PORT_PIN_PA1
#define DRV_PLC_THMON_PIN                     SYS_PORT_PIN_PB15
#define DRV_PLC_CSR_INDEX                     0
#define DRV_PLC_SPI_CLK                       8000000

/* PLC MAC RT Driver RTOS Configuration Options */
#define DRV_PLC_RTOS_STACK_SIZE               320
#define DRV_PLC_RTOS_TASK_PRIORITY            1

/* PLC MAC RT Driver Identification */
#define DRV_G3_MACRT_INDEX                   0U
#define DRV_G3_MACRT_INSTANCES_NUMBER        1U
#define DRV_G3_MACRT_HOST_DESC               "PIC32CX2051MTG128"
/* RF215 Driver Configuration Options */
#define DRV_RF215_INDEX_0                     0U
#define DRV_RF215_CLIENTS_NUMBER              1U
#define DRV_RF215_TX_BUFFERS_NUMBER           1U
#define DRV_RF215_RTOS_STACK_SIZE             448
#define DRV_RF215_RTOS_TASK_PRIORITY          1U
#define DRV_RF215_EXT_INT_PIN                 SYS_PORT_PIN_PC7
#define DRV_RF215_RESET_PIN                   SYS_PORT_PIN_PA25
#define DRV_RF215_LED_TX_PIN                  SYS_PORT_PIN_PA8
#define DRV_RF215_LED_RX_PIN                  SYS_PORT_PIN_PA9
#define DRV_RF215_NUM_TRX                     1U
#define DRV_RF215_FCS_LEN                     4U
#define DRV_RF215_MAX_PSDU_LEN                576U
#define DRV_RF215_MAX_TX_TIME_DELAY_ERROR_US  9000U
#define DRV_RF215_TIME_SYNC_EXECUTION_CYCLES  180U
#define DRV_RF215_TX_COMMAND_EXECUTION_CYCLES 1400U


/* USI Service Common Configuration Options */
#define SRV_USI_INSTANCES_NUMBER              1U
#define SRV_USI_USART_CONNECTIONS             1U
#define SRV_USI_CDC_CONNECTIONS               0U
#define SRV_USI_MSG_POOL_SIZE                 5U
/* PAL RF Configuration Options */
#define PAL_RF_PHY_INDEX                      0U



/* USI Service Instance 0 Configuration Options */
#define SRV_USI_INDEX_0                       0
#define SRV_USI0_RD_BUF_SIZE                  1024
#define SRV_USI0_WR_BUF_SIZE                  1024
#define SRV_USI0_RTOS_STACK_SIZE               256
#define SRV_USI0_RTOS_TASK_PRIORITY            1



// *****************************************************************************
// *****************************************************************************
// Section: Middleware & Other Library Configuration
// *****************************************************************************
// *****************************************************************************

/* G3 stack RTOS configuration */
#define G3_STACK_RTOS_STACK_SIZE         896U
#define G3_STACK_RTOS_TASK_PRIORITY      1U
#define G3_STACK_RTOS_TASK_DELAY_MS      5U

/* MAC COMMON Identification */
#define G3_MAC_COMMON_INDEX_0            0U
#define G3_MAC_COMMON_INSTANCES_NUMBER   1U

/* MAC PLC Identification */
#define G3_MAC_PLC_INDEX_0               0U
#define G3_MAC_PLC_INSTANCES_NUMBER      1U

/* MAC RF Identification */
#define G3_MAC_RF_INDEX_0                0U
#define G3_MAC_RF_INSTANCES_NUMBER       1U

/* MAC Wrapper Identification */
#define G3_MAC_WRP_INDEX_0               0U
#define G3_MAC_WRP_INSTANCES_NUMBER      1U

#define G3_MAC_WRP_SERIAL_USI_INDEX      0U

/* Adaptation Layer Identification */
#define G3_ADP_INDEX_0                   0U
#define G3_ADP_INSTANCES_NUMBER          1U

/* Number of buffers for Adaptation Layer */
#define G3_ADP_NUM_BUFFERS_1280          1U
#define G3_ADP_NUM_BUFFERS_400           3U
#define G3_ADP_NUM_BUFFERS_100           3U
#define G3_ADP_PROCESS_QUEUE_SIZE        (G3_ADP_NUM_BUFFERS_1280 + G3_ADP_NUM_BUFFERS_400 + G3_ADP_NUM_BUFFERS_100)
#define G3_ADP_FRAG_TRANSFER_TABLE_SIZE  1U
#define G3_ADP_FRAGMENT_SIZE             400U

#define G3_ADP_ROUTING_TABLE_SIZE        150U
#define G3_ADP_BLACKLIST_TABLE_SIZE      20U
#define G3_ADP_ROUTING_SET_SIZE          30U
#define G3_ADP_DESTINATION_ADDR_SET_SIZE 1U

/* Table sizes for Routing (LOADNG) */
#define LOADNG_PENDING_RREQ_TABLE_SIZE   6U
#define LOADNG_RREP_GEN_TABLE_SIZE       3U
#define LOADNG_RREQ_FORWARD_TABLE_SIZE   5U
#define LOADNG_DISCOVER_ROUTE_TABLE_SIZE 3U

/* ADP Serialization Identification */
#define G3_ADP_SERIAL_INDEX_0            0U
#define G3_ADP_SERIAL_INSTANCES_NUMBER   1U
#define G3_ADP_SERIAL_USI_INDEX          0U


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
#define WC_NO_HARDEN
#define SINGLE_THREADED
#define NO_SIG_WRAPPER
#define NO_ERROR_STRINGS
#define NO_WOLFSSL_MEMORY
// ---------- FUNCTIONAL CONFIGURATION END ----------



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
