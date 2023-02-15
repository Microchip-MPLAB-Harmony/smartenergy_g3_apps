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
#define SYS_TIME_HW_COUNTER_WIDTH                   (32)
#define SYS_TIME_HW_COUNTER_PERIOD                  (4294967295U)
#define SYS_TIME_HW_COUNTER_HALF_PERIOD             (SYS_TIME_HW_COUNTER_PERIOD>>1)
#define SYS_TIME_CPU_CLOCK_FREQUENCY                (200000000)
#define SYS_TIME_COMPARE_UPDATE_EXECUTION_CYCLES    (188)



// *****************************************************************************
// *****************************************************************************
// Section: Driver Configuration
// *****************************************************************************
// *****************************************************************************
/* RF215 Driver Configuration Options */
#define DRV_RF215_INDEX_0                     0
#define DRV_RF215_CLIENTS_NUMBER              1
#define DRV_RF215_TX_BUFFERS_NUMBER           1
#define DRV_RF215_CSR_INDEX                   0
#define DRV_RF215_EXT_INT_PIN                 SYS_PORT_PIN_PC7
#define DRV_RF215_RESET_PIN                   SYS_PORT_PIN_PD17
#define DRV_RF215_LED_RX_PIN                  SYS_PORT_PIN_PA9
#define DRV_RF215_NUM_TRX                     1
#define DRV_RF215_FCS_LEN                     4
#define DRV_RF215_MAX_PSDU_LEN                576
#define DRV_RF215_MAX_TX_TIME_DELAY_ERROR_US  9000
#define DRV_RF215_TIME_SYNC_EXECUTION_CYCLES  180
#define DRV_RF215_TX_COMMAND_EXECUTION_CYCLES 1400

/* USI Service Common Configuration Options */
#define SRV_USI_INSTANCES_NUMBER              1
#define SRV_USI_USART_CONNECTIONS             1
#define SRV_USI_CDC_CONNECTIONS               0
#define SRV_USI_MSG_POOL_SIZE                 5
/* PLC PHY Driver Configuration Options */
#define DRV_PLC_SECURE                        false
#define DRV_PLC_EXT_INT_PIO_PORT              PIO_PORT_A
#define DRV_PLC_EXT_INT_SRC                   PIOA_IRQn
#define DRV_PLC_EXT_INT_PIN                   SYS_PORT_PIN_PA2
#define DRV_PLC_RESET_PIN                     SYS_PORT_PIN_PD15
#define DRV_PLC_LDO_EN_PIN                    SYS_PORT_PIN_PD19
#define DRV_PLC_TX_ENABLE_PIN                 SYS_PORT_PIN_PA1
#define DRV_PLC_CSR_INDEX                     0

#define DRV_PLC_SPI_CLK                       8000000
#define DRV_PLC_PHY_INSTANCES_NUMBER          1
#define DRV_PLC_PHY_INDEX                     0
#define DRV_PLC_PHY_CLIENTS_NUMBER_IDX        1

/* PLC Driver Identification */
#define DRV_PLC_PHY_PROFILE                   2
#define DRV_PLC_PHY_NUM_CARRIERS              NUM_CARRIERS_FCC
#define DRV_PLC_PHY_HOST_PRODUCT              0x3601
#define DRV_PLC_PHY_HOST_VERSION              0x36010300
#define DRV_PLC_PHY_HOST_PHY                  0x36020103
#define DRV_PLC_PHY_HOST_DESC                 "PIC32CX2051MTG128"
#define DRV_PLC_PHY_HOST_MODEL                3
#define DRV_PLC_PHY_HOST_BAND                 DRV_PLC_PHY_PROFILE
/* USI Service Instance 0 Configuration Options */
#define SRV_USI_INDEX_0                       0
#define SRV_USI0_RD_BUF_SIZE                  1024
#define SRV_USI0_WR_BUF_SIZE                  1024



// *****************************************************************************
// *****************************************************************************
// Section: Middleware & Other Library Configuration
// *****************************************************************************
// *****************************************************************************


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
