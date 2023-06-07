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
#define SYS_TIME_MAX_TIMERS                         (26)
#define SYS_TIME_HW_COUNTER_WIDTH                   (32)
#define SYS_TIME_HW_COUNTER_PERIOD                  (4294967295U)
#define SYS_TIME_HW_COUNTER_HALF_PERIOD             (SYS_TIME_HW_COUNTER_PERIOD>>1)
#define SYS_TIME_CPU_CLOCK_FREQUENCY                (200000000)
#define SYS_TIME_COMPARE_UPDATE_EXECUTION_CYCLES    (232)

#define SYS_CONSOLE_INDEX_0                       0





#define SYS_DEBUG_ENABLE
#define SYS_DEBUG_GLOBAL_ERROR_LEVEL       SYS_ERROR_DEBUG
#define SYS_DEBUG_BUFFER_DMA_READY
#define SYS_DEBUG_USE_CONSOLE


#define SYS_CONSOLE_DEVICE_MAX_INSTANCES   			(1U)
#define SYS_CONSOLE_UART_MAX_INSTANCES 	   			(1U)
#define SYS_CONSOLE_USB_CDC_MAX_INSTANCES 	   		(0U)
#define SYS_CONSOLE_PRINT_BUFFER_SIZE        		(512U)




// *****************************************************************************
// *****************************************************************************
// Section: Driver Configuration
// *****************************************************************************
// *****************************************************************************
/* PAL PLC Configuration Options */
#define PAL_PLC_PHY_INDEX                     0

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

/* PLC MAC RT Driver Identification */
#define DRV_G3_MACRT_INDEX                   0
#define DRV_G3_MACRT_INSTANCES_NUMBER        1
#define DRV_G3_MACRT_HOST_DESC               "PIC32CX2051MTG128"
/* RF215 Driver Configuration Options */
#define DRV_RF215_INDEX_0                     0
#define DRV_RF215_CLIENTS_NUMBER              1
#define DRV_RF215_TX_BUFFERS_NUMBER           1
#define DRV_RF215_CSR_INDEX                   0
#define DRV_RF215_EXT_INT_PIN                 SYS_PORT_PIN_PC7
#define DRV_RF215_RESET_PIN                   SYS_PORT_PIN_PD17
#define DRV_RF215_LED_TX_PIN                  SYS_PORT_PIN_PA8
#define DRV_RF215_LED_RX_PIN                  SYS_PORT_PIN_PA9
#define DRV_RF215_NUM_TRX                     1
#define DRV_RF215_FCS_LEN                     4
#define DRV_RF215_MAX_PSDU_LEN                576
#define DRV_RF215_MAX_TX_TIME_DELAY_ERROR_US  9000
#define DRV_RF215_TIME_SYNC_EXECUTION_CYCLES  180
#define DRV_RF215_TX_COMMAND_EXECUTION_CYCLES 1400

/* PAL RF Configuration Options */
#define PAL_RF_PHY_INDEX                      0



// *****************************************************************************
// *****************************************************************************
// Section: Middleware & Other Library Configuration
// *****************************************************************************
// *****************************************************************************

/* G3 stack task rate in milliseconds */
#define G3_STACK_TASK_RATE_MS            5

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

/* Adaptation Layer Identification */
#define G3_ADP_INDEX_0                   0
#define G3_ADP_INSTANCES_NUMBER          1

/* Number of buffers for Adaptation Layer */
#define G3_ADP_NUM_BUFFERS_1280          1
#define G3_ADP_NUM_BUFFERS_400           3
#define G3_ADP_NUM_BUFFERS_100           3
#define G3_ADP_PROCESS_QUEUE_SIZE        (G3_ADP_NUM_BUFFERS_1280 + G3_ADP_NUM_BUFFERS_400 + G3_ADP_NUM_BUFFERS_100)
#define G3_ADP_FRAG_TRANSFER_TABLE_SIZE  1
#define G3_ADP_FRAGMENT_SIZE             400

#define G3_ADP_ROUTING_TABLE_SIZE        150
#define G3_ADP_BLACKLIST_TABLE_SIZE      20
#define G3_ADP_ROUTING_SET_SIZE          30
#define G3_ADP_DESTINATION_ADDR_SET_SIZE 1

/* Table sizes for Routing (LOADNG) */
#define LOADNG_PENDING_RREQ_TABLE_SIZE   6
#define LOADNG_RREP_GEN_TABLE_SIZE       3
#define LOADNG_RREQ_FORWARD_TABLE_SIZE   5
#define LOADNG_DISCOVER_ROUTE_TABLE_SIZE 3



#define TCPIP_STACK_USE_ICMPV6_SERVER


#define TCPIP_IPV6_NDP_MAX_RTR_SOLICITATION_DELAY 	1
#define TCPIP_IPV6_NDP_RTR_SOLICITATION_INTERVAL 	4
#define TCPIP_IPV6_NDP_MAX_RTR_SOLICITATIONS 		1
#define TCPIP_IPV6_NDP_MAX_MULTICAST_SOLICIT 		1
#define TCPIP_IPV6_NDP_MAX_UNICAST_SOLICIT 			3
#define TCPIP_IPV6_NDP_MAX_ANYCAST_DELAY_TIME 		1
#define TCPIP_IPV6_NDP_MAX_NEIGHBOR_ADVERTISEMENT 	1
#define TCPIP_IPV6_NDP_REACHABLE_TIME 				30
#define TCPIP_IPV6_NDP_RETRANS_TIMER 				1
#define TCPIP_IPV6_NDP_DELAY_FIRST_PROBE_TIME 		5
#define TCPIP_IPV6_NDP_VALID_LIFETIME_TWO_HOURS 	(60 * 60 * 2)
#define TCPIP_IPV6_MTU_INCREASE_TIMEOUT 			600
#define TCPIP_IPV6_NDP_TASK_TIMER_RATE 				32


/* Network Configuration Index 0 */
#define TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX0 "G3ADPMAC"

#define TCPIP_NETWORK_DEFAULT_HOST_NAME_IDX0              "MCHPBOARD_PLC"
#define TCPIP_NETWORK_DEFAULT_MAC_ADDR_IDX0               0

#define TCPIP_NETWORK_DEFAULT_IP_ADDRESS_IDX0         "0.0.0.0"
#define TCPIP_NETWORK_DEFAULT_IP_MASK_IDX0            "255.255.255.0"
#define TCPIP_NETWORK_DEFAULT_GATEWAY_IDX0            "0.0.0.0"
#define TCPIP_NETWORK_DEFAULT_DNS_IDX0                "0.0.0.0"
#define TCPIP_NETWORK_DEFAULT_SECOND_DNS_IDX0         "0.0.0.0"
#define TCPIP_NETWORK_DEFAULT_POWER_MODE_IDX0         "full"
#define TCPIP_NETWORK_DEFAULT_INTERFACE_FLAGS_IDX0            \
                                                    TCPIP_NETWORK_CONFIG_MULTICAST_ON |\
                                                    TCPIP_NETWORK_CONFIG_IP_STATIC
                                                    
#define TCPIP_NETWORK_DEFAULT_MAC_DRIVER_IDX0         DRV_G3ADP_MACObject




/*** IPv6 Configuration ***/
#define TCPIP_IPV6_DEFAULT_ALLOCATION_BLOCK_SIZE 		64
#define TCPIP_IPV6_MINIMUM_LINK_MTU 					1280
#define TCPIP_IPV6_DEFAULT_LINK_MTU 					1280
#define TCPIP_IPV6_DEFAULT_CUR_HOP_LIMIT 				64
#define TCPIP_IPV6_DEFAULT_BASE_REACHABLE_TIME 			2000000
#define TCPIP_IPV6_DEFAULT_RETRANSMIT_TIME 				1000
#define TCPIP_IPV6_QUEUE_NEIGHBOR_PACKET_LIMIT 			1
#define TCPIP_IPV6_NEIGHBOR_CACHE_ENTRY_STALE_TIMEOUT 	600
#define TCPIP_IPV6_QUEUE_MCAST_PACKET_LIMIT 			4
#define TCPIP_IPV6_QUEUED_MCAST_PACKET_TIMEOUT 			10
#define TCPIP_IPV6_TASK_PROCESS_RATE 					1000
#define TCPIP_IPV6_INIT_TASK_PROCESS_RATE 				32
#define TCPIP_IPV6_ULA_GENERATE_ENABLE 					false
#define TCPIP_IPV6_ULA_NTP_ACCESS_TMO 					12000
#define TCPIP_IPV6_ULA_NTP_VALID_WINDOW 				1000
#define TCPIP_IPV6_FRAGMENT_PKT_TIMEOUT 				60
#define TCPIP_IPV6_RX_FRAGMENTED_BUFFER_SIZE 			1280
#define TCPIP_IPV6_EXTERN_PACKET_PROCESS   false


/*** TCPIP Heap Configuration ***/
#define TCPIP_STACK_USE_INTERNAL_HEAP
#define TCPIP_STACK_DRAM_SIZE                       16384
#define TCPIP_STACK_DRAM_RUN_LIMIT                  2048

#define TCPIP_STACK_MALLOC_FUNC                     malloc

#define TCPIP_STACK_CALLOC_FUNC                     calloc

#define TCPIP_STACK_FREE_FUNC                       free



#define TCPIP_STACK_HEAP_USE_FLAGS                   TCPIP_STACK_HEAP_FLAG_ALLOC_UNCACHED

#define TCPIP_STACK_HEAP_USAGE_CONFIG                TCPIP_STACK_HEAP_USE_DEFAULT

#define TCPIP_STACK_SUPPORTED_HEAPS                  1




// *****************************************************************************
// *****************************************************************************
// Section: TCPIP Stack Configuration
// *****************************************************************************
// *****************************************************************************

#define TCPIP_STACK_USE_IPV6
#define TCPIP_STACK_USE_UDP

#define TCPIP_STACK_TICK_RATE		        		5
#define TCPIP_STACK_SECURE_PORT_ENTRIES             10
#define TCPIP_STACK_LINK_RATE		        		333

#define TCPIP_STACK_ALIAS_INTERFACE_SUPPORT   false

#define TCPIP_PACKET_LOG_ENABLE     0

/* TCP/IP stack event notification */
#define TCPIP_STACK_USE_EVENT_NOTIFICATION
#define TCPIP_STACK_USER_NOTIFICATION   false
#define TCPIP_STACK_DOWN_OPERATION   false
#define TCPIP_STACK_IF_UP_DOWN_OPERATION   false
#define TCPIP_STACK_MAC_DOWN_OPERATION   true
#define TCPIP_STACK_INTERFACE_CHANGE_SIGNALING   false
#define TCPIP_STACK_CONFIGURATION_SAVE_RESTORE   true
#define TCPIP_STACK_EXTERN_PACKET_PROCESS   false
#define TCPIP_STACK_RUN_TIME_INIT   false

#define TCPIP_STACK_INTMAC_COUNT           1






/*** UDP Configuration ***/
#define TCPIP_UDP_MAX_SOCKETS		                	2
#define TCPIP_UDP_SOCKET_DEFAULT_TX_SIZE		    	1200
#define TCPIP_UDP_SOCKET_DEFAULT_TX_QUEUE_LIMIT    	 	3
#define TCPIP_UDP_SOCKET_DEFAULT_RX_QUEUE_LIMIT			3
#define TCPIP_UDP_USE_POOL_BUFFERS   false
#define TCPIP_UDP_USE_TX_CHECKSUM             			true
#define TCPIP_UDP_USE_RX_CHECKSUM             			true
#define TCPIP_UDP_COMMANDS   false
#define TCPIP_UDP_EXTERN_PACKET_PROCESS   false



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

#define TCPIP_STACK_NETWORK_INTERAFCE_COUNT  	1







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
