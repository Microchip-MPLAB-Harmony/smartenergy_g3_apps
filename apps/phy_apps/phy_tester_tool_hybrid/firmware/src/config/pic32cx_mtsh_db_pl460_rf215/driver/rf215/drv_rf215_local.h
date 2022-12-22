/*******************************************************************************
  RF215 Driver Local Definitions

  Company:
    Microchip Technology Inc.

  File Name:
    drv_rf215_local.h

  Summary:
    RF215 Driver local definitions and data types.

  Description:
    This file contains the RF215 driver local definitions and data types.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2022 Microchip Technology Inc. and its subsidiaries.
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
//DOM-IGNORE-END

#ifndef _DRV_RF215_LOCAL_H
#define _DRV_RF215_LOCAL_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************
#include "system/time/sys_time.h"
#include "driver/driver.h"
#include "driver/rf215/drv_rf215.h"
#include "configuration.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* RF215 Driver Instance Object

  Summary:
    Object used to keep any data required for an instance of the RF215 driver.
*/

typedef struct
{
    /* The status of the driver */
    SYS_STATUS                      sysStatus;

    /* Ready status callback */
    DRV_RF215_READY_STATUS_CALLBACK readyStatusCallback;
    uintptr_t                       readyStatusContext;

    /* Time handle for timeout */
    SYS_TIME_HANDLE                 timeoutHandle;

    /* Token counter for handle generation */
    uint16_t                        tokenCount;

    /* RF215 IRQ status registers (external interrupt flags) */
    uint8_t                         RF09_IRQS;
    uint8_t                         RF24_IRQS;
    uint8_t                         BBC0_IRQS;
    uint8_t                         BBC1_IRQS;

    /* RF215 Part and version number registers */
    uint8_t                         RF_PN;
    uint8_t                         RF_VN;

    /* Counter of consecutive external interrupts with empty IRQS flags */
    uint8_t                         irqsEmptyCount;

    /* Wrong IRQS flags error */
    bool                            irqsErr;

    /* RF215 part/version number error */
    bool                            partNumErr;

    /* Timeout error */
    bool                            timeoutErr;

    /* RF215 Chip Reset pending (at initialization) */
    bool                            rfChipResetPending;

    /* RF215 Chip Reset flag */
    bool                            rfChipResetFlag;

    /* Ready status notified flag */
    bool                            readyStatusNotified;

} DRV_RF215_OBJ;

// *****************************************************************************
/* RF215 Driver Client Object

  Summary:
    Object used to track a single client.

  Description:
    This object is used to keep the data necessary to keep track of a single
    client.

  Remarks:
    None.
*/

typedef struct
{
    /* Receive indication callback */
    DRV_RF215_RX_IND_CALLBACK       rxIndCallback;
    uintptr_t                       rxIndContext;

    /* Transmit confirm callback */
    DRV_RF215_TX_CFM_CALLBACK       txCfmCallback;
    uintptr_t                       txCfmContext;

    /* Client handle assigned to this client object when it was opened */
    DRV_HANDLE                      clientHandle;

    /* The RF215 transceiver index associated with the client */
    uint8_t                         trxIndex;

    /* This flags indicates if the object is in use or is available */
    bool                            inUse;

} DRV_RF215_CLIENT_OBJ;

// *****************************************************************************
/* RF215 Driver TX Buffer Object

  Summary:
    Object used to track a single TX buffer.

  Description:
    This object is used to keep the data necessary to keep track of a single
    transmission buffer.

  Remarks:
    None.
*/

typedef struct
{
    /* Pointer to the client object that made the TX request */
    DRV_RF215_CLIENT_OBJ*           clientObj;

    /* TX request object containing the TX parameters */
    DRV_RF215_TX_REQUEST_OBJ        reqObj;

    /* TX confirm object containing the TX result */
    DRV_RF215_TX_CONFIRM_OBJ        cfmObj;

    /* Transmission handle assigned to this TX buffer when it was requested */
    DRV_RF215_TX_HANDLE             txHandle;

    /* Time handle assigned to this scheduled TX */
    SYS_TIME_HANDLE                 timeHandle;

    /* This flags indicates if the TX buffer object is in use or is available */
    bool                            inUse;

    /* Flag to indicate that TX confirm needs to be notified */
    bool                            cfmPending;

    /* PSDU buffer */
    uint8_t                         psdu[DRV_RF215_MAX_PSDU_LEN];

} DRV_RF215_TX_BUFFER_OBJ;

// *****************************************************************************
/* RF215 Register Base Addresses

  Summary:
    Object used to store the RF215 register base addresses (different for each
    transceiver).
*/

typedef struct
{
    uint16_t RFn;
    uint16_t BBCn;
    uint16_t frameBufBBCn;
} RF215_REG_BASE_ADDR;

// *****************************************************************************
/* RF215 Register Values (RFn_CMD)

  Summary:
    Object used to store the possible values of RF_CMD register.
*/

typedef struct
{
    uint8_t sleep;
    uint8_t trxoff;
    uint8_t txprep;
    uint8_t tx;
    uint8_t rx;
    uint8_t reset;

} RF215_REG_CMD_VALUES_OBJ;

// *****************************************************************************
/* RF215 Register Values

  Summary:
    Object used to store constant values of RF215 registers, used to write
    through SPI.
*/

typedef struct
{
    uint8_t                   RF_CLKO;
    uint8_t                   RFn_IRQM;
    uint8_t                   RFn_AUXS;
    RF215_REG_CMD_VALUES_OBJ  RFn_CMD;
    uint8_t                   BBCn_CNTC;

} RF215_REG_VALUES_OBJ;


// *****************************************************************************
// *****************************************************************************
// Section: Global Constant Data
// *****************************************************************************
// *****************************************************************************

/* RF215 register values */
extern const RF215_REG_VALUES_OBJ rf215RegValues;

/* RF215 register base addresses */
extern const RF215_REG_BASE_ADDR rf215RegBaseAddr[2];

// *****************************************************************************
// *****************************************************************************
// Section: RF215 Driver Local Functions
// *****************************************************************************
// *****************************************************************************

void DRV_RF215_ExtIntHandler(void);

void DRV_RF215_NotifyRxInd(uint8_t trxIdx, DRV_RF215_RX_INDICATION_OBJ* ind);

void DRV_RF215_AbortTxByRx(uint8_t trxIdx);

void DRV_RF215_AbortTxByPhyConfig(uint8_t trxIdx);

// *****************************************************************************
// *****************************************************************************
// Section: Macro Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Division With Rounding

  Summary:
    This macro calculates round(a/b) using integer arithmetic.
*/

#define DIV_ROUND(a, b)                            (((a) + (b >> 1)) / (b))

// *****************************************************************************
/* Division With Ceiling

  Summary:
    This macro calculates ceil(a/b) using integer arithmetic.
*/

#define DIV_CEIL(a, b)                            (((a) + (b) - 1) / (b))

// *****************************************************************************
/* RF215 Transceiver Indexes

  Summary:
    Indexes for each RF215 transceiver (Sub-GHz and/or 2.4GHz). This is used
    internally by RF215 driver to identify the TRX.
*/

#define RF215_TRX_RF09_IDX                         0
#define RF215_TRX_RF24_IDX                         1

// *****************************************************************************
/* RF215 Register Base Addresses

  Summary:
    RF215 register base addresses definition.
*/

/* Sub-1GHz radio registers base address (RF09_) */
#define RF215_BASE_ADDR_RF09                       0x0100
/* 2.4GHz radio registers base address (RF24_) */
#define RF215_BASE_ADDR_RF24                       0x0200
/* Baseband processor Core0 base address (BBC0_) */
#define RF215_BASE_ADDR_BBC0                       0x0300
/* Baseband processor Core1 base address (BBC1_) */
#define RF215_BASE_ADDR_BBC1                       0x0400
/* Baseband processor Core0 frame buffer base address (BBC0_) */
#define RF215_BASE_ADDR_FRAME_BUF_BBC0             0x2000
/* Baseband processor Core1 frame buffer base address (BBC1_) */
#define RF215_BASE_ADDR_FRAME_BUF_BBC1             0x3000

// *****************************************************************************
/* RF215 Register Addresses

  Summary:
    RF215 register addresses definition.
*/

/* Radio IRQ Status registers (RF09_ and RF24_) */
#define RF215_RF09_IRQS                            0x0000
#define RF215_RF24_IRQS                            0x0001

/* Baseband IRQ Status registers (BBC0_ and BBC1_) */
#define RF215_BBC0_IRQS                            0x0002
#define RF215_BBC1_IRQS                            0x0003

/* Common transceiver registers (RF_) */
#define RF215_RF_RST                               0x0005
#define RF215_RF_CFG                               0x0006
#define RF215_RF_CLKO                              0x0007
#define RF215_RF_BMDVC                             0x0008
#define RF215_RF_XOC                               0x0009
#define RF215_RF_IQIFC0                            0x000A
#define RF215_RF_IQIFC1                            0x000B
#define RF215_RF_IQIFC2                            0x000C
#define RF215_RF_PN                                0x000D
#define RF215_RF_VN                                0x000E

/* Sub-1GHz/2.4GHz radio registers (RFn_), depending on TRX index */
#define RF215_ADDR_RFn(offset, trxIdx)            (rf215RegBaseAddr[trxIdx].RFn + offset)
#define RF215_RFn_IRQM(trxIdx)                     RF215_ADDR_RFn(0x00, trxIdx)
#define RF215_RFn_AUXS(trxIdx)                     RF215_ADDR_RFn(0x01, trxIdx)
#define RF215_RFn_STATE(trxIdx)                    RF215_ADDR_RFn(0x02, trxIdx)
#define RF215_RFn_CMD(trxIdx)                      RF215_ADDR_RFn(0x03, trxIdx)
#define RF215_ADDR_RF09_CMD                       (0x03 + RF215_BASE_ADDR_RF09)
#define RF215_ADDR_RF24_CMD                       (0x03 + RF215_BASE_ADDR_RF24)
#define RF215_RFn_CS(trxIdx)                       RF215_ADDR_RFn(0x04, trxIdx)
#define RF215_RFn_CCF0L(trxIdx)                    RF215_ADDR_RFn(0x05, trxIdx)
#define RF215_RFn_CCF0H(trxIdx)                    RF215_ADDR_RFn(0x06, trxIdx)
#define RF215_RFn_CNL(trxIdx)                      RF215_ADDR_RFn(0x07, trxIdx)
#define RF215_RFn_CNM(trxIdx)                      RF215_ADDR_RFn(0x08, trxIdx)
#define RF215_RFn_RXBWC(trxIdx)                    RF215_ADDR_RFn(0x09, trxIdx)
#define RF215_RFn_RXDFE(trxIdx)                    RF215_ADDR_RFn(0x0A, trxIdx)
#define RF215_RFn_AGCC(trxIdx)                     RF215_ADDR_RFn(0x0B, trxIdx)
#define RF215_RFn_AGCS(trxIdx)                     RF215_ADDR_RFn(0x0C, trxIdx)
#define RF215_RFn_RSSI(trxIdx)                     RF215_ADDR_RFn(0x0D, trxIdx)
#define RF215_RFn_EDC(trxIdx)                      RF215_ADDR_RFn(0x0E, trxIdx)
#define RF215_RFn_EDD(trxIdx)                      RF215_ADDR_RFn(0x0F, trxIdx)
#define RF215_RFn_EDV(trxIdx)                      RF215_ADDR_RFn(0x10, trxIdx)
#define RF215_RFn_RNDV(trxIdx)                     RF215_ADDR_RFn(0x11, trxIdx)
#define RF215_RFn_TXCUTC(trxIdx)                   RF215_ADDR_RFn(0x12, trxIdx)
#define RF215_RFn_TXDFE(trxIdx)                    RF215_ADDR_RFn(0x13, trxIdx)
#define RF215_RFn_PAC(trxIdx)                      RF215_ADDR_RFn(0x14, trxIdx)
#define RF215_RFn_PADFE(trxIdx)                    RF215_ADDR_RFn(0x16, trxIdx)
#define RF215_RFn_PLL(trxIdx)                      RF215_ADDR_RFn(0x21, trxIdx)
#define RF215_RFn_PLLCF(trxIdx)                    RF215_ADDR_RFn(0x22, trxIdx)
#define RF215_RFn_TXCI(trxIdx)                     RF215_ADDR_RFn(0x25, trxIdx)
#define RF215_RFn_TXCQ(trxIdx)                     RF215_ADDR_RFn(0x26, trxIdx)
#define RF215_RFn_TXDACI(trxIdx)                   RF215_ADDR_RFn(0x27, trxIdx)
#define RF215_RFn_TXDACQ(trxIdx)                   RF215_ADDR_RFn(0x28, trxIdx)

/* Baseband processor Core0/Core1 registers (BBCn_), depending on TRX index */
#define RF215_ADDR_BBCn(offset, trxIdx)           (rf215RegBaseAddr[trxIdx].BBCn + offset)
#define RF215_BBCn_IRQM(trxIdx)                    RF215_ADDR_BBCn(0x00, trxIdx)
#define RF215_BBCn_PC(trxIdx)                      RF215_ADDR_BBCn(0x01, trxIdx)
#define RF215_BBCn_PS(trxIdx)                      RF215_ADDR_BBCn(0x02, trxIdx)
#define RF215_BBCn_RXFLL(trxIdx)                   RF215_ADDR_BBCn(0x04, trxIdx)
#define RF215_BBCn_RXFLH(trxIdx)                   RF215_ADDR_BBCn(0x05, trxIdx)
#define RF215_BBCn_TXFLL(trxIdx)                   RF215_ADDR_BBCn(0x06, trxIdx)
#define RF215_BBCn_TXFLH(trxIdx)                   RF215_ADDR_BBCn(0x07, trxIdx)
#define RF215_BBCn_FBLL(trxIdx)                    RF215_ADDR_BBCn(0x08, trxIdx)
#define RF215_BBCn_FBLH(trxIdx)                    RF215_ADDR_BBCn(0x09, trxIdx)
#define RF215_BBCn_FBLIL(trxIdx)                   RF215_ADDR_BBCn(0x0A, trxIdx)
#define RF215_BBCn_FBLIH(trxIdx)                   RF215_ADDR_BBCn(0x0B, trxIdx)
#define RF215_BBCn_OFDMPHRTX(trxIdx)               RF215_ADDR_BBCn(0x0C, trxIdx)
#define RF215_BBCn_OFDMPHRRX(trxIdx)               RF215_ADDR_BBCn(0x0D, trxIdx)
#define RF215_BBCn_OFDMC(trxIdx)                   RF215_ADDR_BBCn(0x0E, trxIdx)
#define RF215_BBCn_OFDMSW(trxIdx)                  RF215_ADDR_BBCn(0x0F, trxIdx)
#define RF215_BBCn_OQPSKC0(trxIdx)                 RF215_ADDR_BBCn(0x10, trxIdx)
#define RF215_BBCn_OQPSKC1(trxIdx)                 RF215_ADDR_BBCn(0x11, trxIdx)
#define RF215_BBCn_OQPSKC2(trxIdx)                 RF215_ADDR_BBCn(0x12, trxIdx)
#define RF215_BBCn_OQPSKC3(trxIdx)                 RF215_ADDR_BBCn(0x13, trxIdx)
#define RF215_BBCn_OQPSKPHRTX(trxIdx)              RF215_ADDR_BBCn(0x14, trxIdx)
#define RF215_BBCn_OQPSKPHRRX(trxIdx)              RF215_ADDR_BBCn(0x15, trxIdx)
#define RF215_BBCn_AFC0(trxIdx)                    RF215_ADDR_BBCn(0x20, trxIdx)
#define RF215_BBCn_AFC1(trxIdx)                    RF215_ADDR_BBCn(0x21, trxIdx)
#define RF215_BBCn_AFFTM(trxIdx)                   RF215_ADDR_BBCn(0x22, trxIdx)
#define RF215_BBCn_AFFVM(trxIdx)                   RF215_ADDR_BBCn(0x23, trxIdx)
#define RF215_BBCn_AFS(trxIdx)                     RF215_ADDR_BBCn(0x24, trxIdx)
#define RF215_BBCn_MACEA0(trxIdx)                  RF215_ADDR_BBCn(0x25, trxIdx)
#define RF215_BBCn_MACEA1(trxIdx)                  RF215_ADDR_BBCn(0x26, trxIdx)
#define RF215_BBCn_MACEA2(trxIdx)                  RF215_ADDR_BBCn(0x27, trxIdx)
#define RF215_BBCn_MACEA3(trxIdx)                  RF215_ADDR_BBCn(0x28, trxIdx)
#define RF215_BBCn_MACEA4(trxIdx)                  RF215_ADDR_BBCn(0x29, trxIdx)
#define RF215_BBCn_MACEA5(trxIdx)                  RF215_ADDR_BBCn(0x2A, trxIdx)
#define RF215_BBCn_MACEA6(trxIdx)                  RF215_ADDR_BBCn(0x2B, trxIdx)
#define RF215_BBCn_MACEA7(trxIdx)                  RF215_ADDR_BBCn(0x2C, trxIdx)
#define RF215_BBCn_MACPID0F0(trxIdx)               RF215_ADDR_BBCn(0x2D, trxIdx)
#define RF215_BBCn_MACPID1F0(trxIdx)               RF215_ADDR_BBCn(0x2E, trxIdx)
#define RF215_BBCn_MACSHA0F0(trxIdx)               RF215_ADDR_BBCn(0x2F, trxIdx)
#define RF215_BBCn_MACSHA1F0(trxIdx)               RF215_ADDR_BBCn(0x30, trxIdx)
#define RF215_BBCn_MACPID0F1(trxIdx)               RF215_ADDR_BBCn(0x31, trxIdx)
#define RF215_BBCn_MACPID1F1(trxIdx)               RF215_ADDR_BBCn(0x32, trxIdx)
#define RF215_BBCn_MACSHA0F1(trxIdx)               RF215_ADDR_BBCn(0x33, trxIdx)
#define RF215_BBCn_MACSHA1F1(trxIdx)               RF215_ADDR_BBCn(0x34, trxIdx)
#define RF215_BBCn_MACPID0F2(trxIdx)               RF215_ADDR_BBCn(0x35, trxIdx)
#define RF215_BBCn_MACPID1F2(trxIdx)               RF215_ADDR_BBCn(0x36, trxIdx)
#define RF215_BBCn_MACSHA0F2(trxIdx)               RF215_ADDR_BBCn(0x37, trxIdx)
#define RF215_BBCn_MACSHA1F2(trxIdx)               RF215_ADDR_BBCn(0x38, trxIdx)
#define RF215_BBCn_MACPID0F3(trxIdx)               RF215_ADDR_BBCn(0x39, trxIdx)
#define RF215_BBCn_MACPID1F3(trxIdx)               RF215_ADDR_BBCn(0x3A, trxIdx)
#define RF215_BBCn_MACSHA0F3(trxIdx)               RF215_ADDR_BBCn(0x3B, trxIdx)
#define RF215_BBCn_MACSHA1F3(trxIdx)               RF215_ADDR_BBCn(0x3C, trxIdx)
#define RF215_BBCn_AMCS(trxIdx)                    RF215_ADDR_BBCn(0x40, trxIdx)
#define RF215_BBCn_AMEDT(trxIdx)                   RF215_ADDR_BBCn(0x41, trxIdx)
#define RF215_BBCn_AMAACKPD(trxIdx)                RF215_ADDR_BBCn(0x42, trxIdx)
#define RF215_BBCn_AMAACKTL(trxIdx)                RF215_ADDR_BBCn(0x43, trxIdx)
#define RF215_BBCn_AMAACKTH(trxIdx)                RF215_ADDR_BBCn(0x44, trxIdx)
#define RF215_BBCn_FSKC0(trxIdx)                   RF215_ADDR_BBCn(0x60, trxIdx)
#define RF215_BBCn_FSKC1(trxIdx)                   RF215_ADDR_BBCn(0x61, trxIdx)
#define RF215_BBCn_FSKC2(trxIdx)                   RF215_ADDR_BBCn(0x62, trxIdx)
#define RF215_BBCn_FSKC3(trxIdx)                   RF215_ADDR_BBCn(0x63, trxIdx)
#define RF215_BBCn_FSKC4(trxIdx)                   RF215_ADDR_BBCn(0x64, trxIdx)
#define RF215_BBCn_FSKPLL(trxIdx)                  RF215_ADDR_BBCn(0x65, trxIdx)
#define RF215_BBCn_FSKSFD0L(trxIdx)                RF215_ADDR_BBCn(0x66, trxIdx)
#define RF215_BBCn_FSKSFD0H(trxIdx)                RF215_ADDR_BBCn(0x67, trxIdx)
#define RF215_BBCn_FSKSFD1L(trxIdx)                RF215_ADDR_BBCn(0x68, trxIdx)
#define RF215_BBCn_FSKSFD1H(trxIdx)                RF215_ADDR_BBCn(0x69, trxIdx)
#define RF215_BBCn_FSKPHRTX(trxIdx)                RF215_ADDR_BBCn(0x6A, trxIdx)
#define RF215_BBCn_FSKPHRRX(trxIdx)                RF215_ADDR_BBCn(0x6B, trxIdx)
#define RF215_BBCn_FSKRPC(trxIdx)                  RF215_ADDR_BBCn(0x6C, trxIdx)
#define RF215_BBCn_FSKRPCONT(trxIdx)               RF215_ADDR_BBCn(0x6D, trxIdx)
#define RF215_BBCn_FSKRPCOFFT(trxIdx)              RF215_ADDR_BBCn(0x6E, trxIdx)
#define RF215_BBCn_FSKRRXFLL(trxIdx)               RF215_ADDR_BBCn(0x70, trxIdx)
#define RF215_BBCn_FSKRRXFLH(trxIdx)               RF215_ADDR_BBCn(0x71, trxIdx)
#define RF215_BBCn_FSKDM(trxIdx)                   RF215_ADDR_BBCn(0x72, trxIdx)
#define RF215_BBCn_FSKPE0(trxIdx)                  RF215_ADDR_BBCn(0x73, trxIdx)
#define RF215_BBCn_FSKPE1(trxIdx)                  RF215_ADDR_BBCn(0x74, trxIdx)
#define RF215_BBCn_FSKPE2(trxIdx)                  RF215_ADDR_BBCn(0x75, trxIdx)
#define RF215_BBCn_PMUC(trxIdx)                    RF215_ADDR_BBCn(0x80, trxIdx)
#define RF215_BBCn_PMUVAL(trxIdx)                  RF215_ADDR_BBCn(0x81, trxIdx)
#define RF215_BBCn_PMUQF(trxIdx)                   RF215_ADDR_BBCn(0x82, trxIdx)
#define RF215_BBCn_PMUI(trxIdx)                    RF215_ADDR_BBCn(0x83, trxIdx)
#define RF215_BBCn_PMUQ(trxIdx)                    RF215_ADDR_BBCn(0x84, trxIdx)
#define RF215_BBCn_CNTC(trxIdx)                    RF215_ADDR_BBCn(0x90, trxIdx)
#define RF215_BBCn_CNT0(trxIdx)                    RF215_ADDR_BBCn(0x91, trxIdx)
#define RF215_BBCn_CNT1(trxIdx)                    RF215_ADDR_BBCn(0x92, trxIdx)
#define RF215_BBCn_CNT2(trxIdx)                    RF215_ADDR_BBCn(0x93, trxIdx)
#define RF215_BBCn_CNT3(trxIdx)                    RF215_ADDR_BBCn(0x94, trxIdx)

/** Baseband Core0/Core1 frame buffer registers (BBCn_), depending on TRX index */
#define RF215_ADDR_FRAME_BUF_BBCn(offset, trxIdx) (rf215RegBaseAddr[trxIdx].frameBufBBCn + offset)
#define RF215_BBCn_FBRXS(trxIdx)                   RF215_ADDR_FRAME_BUF_BBCn(0x000, trxIdx)
#define RF215_BBCn_FBRXE(trxIdx)                   RF215_ADDR_FRAME_BUF_BBCn(0x7FE, trxIdx)
#define RF215_BBCn_FBTXS(trxIdx)                   RF215_ADDR_FRAME_BUF_BBCn(0x800, trxIdx)
#define RF215_BBCn_FBTXE(trxIdx)                   RF215_ADDR_FRAME_BUF_BBCn(0xFFE, trxIdx)

// *****************************************************************************
/* RF215 Register Bitfields

  Summary:
    RF215 register bitfields definition.
*/

/** RFn_IRQS - Radio IRQ Status; RFn_IRQM - Radio IRQ Mask */
/* Wake-up / Reset Interrupt */
#define RF215_RFn_IRQ_WAKEUP             (1 << 0)
/* Transceiver Ready Interrupt */
#define RF215_RFn_IRQ_TRXRDY             (1 << 1)
/* Energy Detection Completion Interrupt */
#define RF215_RFn_IRQ_EDC                (1 << 2)
/* Battery Low Interrupt */
#define RF215_RFn_IRQ_BATLOW             (1 << 3)
/* Transceiver Error Interrupt */
#define RF215_RFn_IRQ_TRXERR             (1 << 4)
/* I/Q IF Synchronization Failure Interrupt */
#define RF215_RFn_IRQ_IQIFSF             (1 << 5)

/** BBCn_IRQS - Baseband IRQ Status; BBCn_IRQM - Baseband IRQ Mask */
/* Receiver Frame Start Interrupt */
#define RF215_BBCn_IRQ_RXFS              (1 << 0)
/* Receiver Frame End Interrupt */
#define RF215_BBCn_IRQ_RXFE              (1 << 1)
/* Receiver Address Match Interrupt */
#define RF215_BBCn_IRQ_RXAM              (1 << 2)
/* Receiver Extended Match Interrupt */
#define RF215_BBCn_IRQ_RXEM              (1 << 3)
/* Transmitter Frame End Interrupt */
#define RF215_BBCn_IRQ_TXFE              (1 << 4)
/* AGC Hold Interrupt */
#define RF215_BBCn_IRQ_AGCH              (1 << 5)
/* AGC Release Interrupt */
#define RF215_BBCn_IRQ_AGCR              (1 << 6)
/* Frame Buffer Level Indication Interrupt */
#define RF215_BBCn_IRQ_FBLI              (1 << 7)

/** RF_CLKO - Clock Output */
/* Bit 2:0 - RF_CLKO.OS: Clock Output Selection */
#define RF215_RF_CLKO_OS_Pos             0
#define RF215_RF_CLKO_OS_Msk             (0x7 << RF215_RF_CLKO_OS_Pos)
#define RF215_RF_CLKO_OS(x)              (((x) << RF215_RF_CLKO_OS_Pos) & RF215_RF_CLKO_OS_Msk)
#define RF215_RF_CLKO_OS_OFF             RF215_RF_CLKO_OS(0)
#define RF215_RF_CLKO_OS_26MHz           RF215_RF_CLKO_OS(1) /* Default */
#define RF215_RF_CLKO_OS_32MHz           RF215_RF_CLKO_OS(2)
#define RF215_RF_CLKO_OS_16MHz           RF215_RF_CLKO_OS(3)
#define RF215_RF_CLKO_OS_8MHz            RF215_RF_CLKO_OS(4)
#define RF215_RF_CLKO_OS_4MHz            RF215_RF_CLKO_OS(5)
#define RF215_RF_CLKO_OS_2MHz            RF215_RF_CLKO_OS(6)
#define RF215_RF_CLKO_OS_1MHz            RF215_RF_CLKO_OS(7)
/* Bit 4:3 - RF_CLKO.DRV: Output Driver Strength CLKO */
#define RF215_RF_CLKO_DRV_Pos            3
#define RF215_RF_CLKO_DRV_Msk            (0x3 << RF215_RF_CLKO_DRV_Pos)
#define RF215_RF_CLKO_DRV(x)             (((x) << RF215_RF_CLKO_DRV_Pos) & RF215_RF_CLKO_DRV_Msk)
#define RF215_RF_CLKO_DRV_2mA            RF215_RF_CLKO_DRV(0)
#define RF215_RF_CLKO_DRV_4mA            RF215_RF_CLKO_DRV(1) /* Default */
#define RF215_RF_CLKO_DRV_6mA            RF215_RF_CLKO_DRV(2)
#define RF215_RF_CLKO_DRV_8mA            RF215_RF_CLKO_DRV(3)

/** RF_PN - Device Part Number */
#define RF215_RF_PN_AT86RF215            0x34
#define RF215_RF_PN_AT86RF215Q           0x35
/* Errata #9: The RF215M device part number is 0x34 instead of 0x36 */
#define RF215_RF_PN_AT86RF215M           0x36

/** RF_VN - Device Version Number */
#define RF215_RF_VN_V1                   0x01
#define RF215_RF_VN_V3                   0x03

/** RFn_AUXS - Transceiver Auxiliary Settings */
/* Bit 1:0 - AUXS.PAVC: Power Amplifier Voltage Control */
#define RF215_RFn_AUXS_PAVC_Pos          0
#define RF215_RFn_AUXS_PAVC_Msk          (0x3 << RF215_RFn_AUXS_PAVC_Pos)
#define RF215_RFn_AUXS_PAVC(x)           (((x) << RF215_RFn_AUXS_PAVC_Pos) & RF215_RFn_AUXS_PAVC_Msk)
#define RF215_RFn_AUXS_PAVC_2_0V         RF215_RFn_AUXS_PAVC(0)
#define RF215_RFn_AUXS_PAVC_2_2V         RF215_RFn_AUXS_PAVC(1)
#define RF215_RFn_AUXS_PAVC_2_4V         RF215_RFn_AUXS_PAVC(2) /* Default */
/* Bit 2 - AUXS.AVS: Analog Voltage Status */
#define RF215_RFn_AUXS_AVS               (1 << 2)
/* Bit 3 - AUXS.AVEN: Analog Voltage Enable */
#define RF215_RFn_AUXS_AVEN_Pos          3
#define RF215_RFn_AUXS_AVEN_Msk          (0x1 << RF215_RFn_AUXS_AVEN_Pos)
#define RF215_RFn_AUXS_AVEN(x)           (((x) << RF215_RFn_AUXS_AVEN_Pos) & RF215_RFn_AUXS_AVEN_Msk)
#define RF215_RFn_AUXS_AVEN_DIS          RF215_RFn_AUXS_AVEN(0) /* Default */
#define RF215_RFn_AUXS_AVEN_EN           RF215_RFn_AUXS_AVEN(1)
/* Bit 4 - AUXS.AVEXT: Analog Voltage External Driven */
#define RF215_RFn_AUXS_AVEXT_Pos         4
#define RF215_RFn_AUXS_AVEXT_Msk         (0x1 << RF215_RFn_AUXS_AVEXT_Pos)
#define RF215_RFn_AUXS_AVEXT(x)          (((x) << RF215_RFn_AUXS_AVEXT_Pos) & RF215_RFn_AUXS_AVEXT_Msk)
#define RF215_RFn_AUXS_AVEXT_INT         RF215_RFn_AUXS_AVEXT(0) /* Default */
#define RF215_RFn_AUXS_AVEXT_EXT         RF215_RFn_AUXS_AVEXT(1)
/* Bit 6:5 - AUXS.AGCMAP: AGC Map */
#define RF215_RFn_AUXS_AGCMAP_Pos        5
#define RF215_RFn_AUXS_AGCMAP_Msk        (0x3 << RF215_RFn_AUXS_AGCMAP_Pos)
#define RF215_RFn_AUXS_AGCMAP(x)         (((x) << RF215_RFn_AUXS_AGCMAP_Pos) & RF215_RFn_AUXS_AGCMAP_Msk)
#define RF215_RFn_AUXS_AGCMAP_INT        RF215_RFn_AUXS_AGCMAP(0) /* Default; Internal AGC, no external LNA used */
#define RF215_RFn_AUXS_AGCMAP_EXT_9dB    RF215_RFn_AUXS_AGCMAP(1) /* AGC back-off for external LNA of about 9dB gain */
#define RF215_RFn_AUXS_AGCMAP_EXT_12dB   RF215_RFn_AUXS_AGCMAP(2) /* AGC back-off for external LNA of about 12dB gain */
/* Bit 7 - AUXS.EXTLNABYP: External LNA Bypass Availability */
#define RF215_RFn_AUXS_EXTLNABYP_Pos     7
#define RF215_RFn_AUXS_EXTLNABYP_Msk     (0x1 << RF215_RFn_AUXS_EXTLNABYP_Pos)
#define RF215_RFn_AUXS_EXTLNABYP(x)      (((x) << RF215_RFn_AUXS_EXTLNABYP_Pos) & RF215_RFn_AUXS_EXTLNABYP_Msk)
#define RF215_RFn_AUXS_EXTLNABYP_DIS     RF215_RFn_AUXS_EXTLNABYP(0) /* Default; Bypass of external LNA not available */
#define RF215_RFn_AUXS_EXTLNABYP_EN      RF215_RFn_AUXS_EXTLNABYP(1) /* Bypass of external LNA available */

/** RFn_STATE - Transceiver State */
/* TRXOFF (Transceiver off, SPI active) */
#define RF215_RFn_STATE_RF_TRXOFF        0x2
/* TXPREP (Transmit preparation) */
#define RF215_RFn_STATE_RF_TXPREP        0x3
/* TX (Transmit) */
#define RF215_RFn_STATE_RF_TX            0x4
/* RX (Receive) */
#define RF215_RFn_STATE_RF_RX            0x5
/* TRANSITION (State transition in progress) */
#define RF215_RFn_STATE_RF_TRANSITION    0x6
/* RESET (Transceiver is in state RESET or SLEEP) */
#define RF215_RFn_STATE_RF_RESET         0x7

/** RFn_CMD - Transceiver Command */
/* NO OPERATION */
#define RF215_RFn_CMD_RF_NOP             0x0
/* SLEEP */
#define RF215_RFn_CMD_RF_SLEEP           0x1
/* TRXOFF (Transceiver off, SPI active) */
#define RF215_RFn_CMD_RF_TRXOFF          0x2
/* TXPREP (Transmit preparation) */
#define RF215_RFn_CMD_RF_TXPREP          0x3
/* TX (Transmit) */
#define RF215_RFn_CMD_RF_TX              0x4
/* RX (Receive) */
#define RF215_RFn_CMD_RF_RX              0x5
/* RESET (transceiver reset, the transceiver state will automatically end up in state TRXOFF) */
#define RF215_RFn_CMD_RF_RESET           0x7

/** RFn_CNM - Channel Mode and Channel Number High Bit */
/* Bit 0 - CNM.CNH: Channel Number CN[8] */
#define RF215_RFn_CNM_CNH_Pos            0
#define RF215_RFn_CNM_CNH_Msk            (0x1 << RF215_RFn_CNM_CNH_Pos)
#define RF215_RFn_CNM_CNH(x)             (((x) << RF215_RFn_CNM_CNH_Pos) & RF215_RFn_CNM_CNH_Msk)
/* Bit 7:6 - CNM.CM: Channel Setting Mode */
#define RF215_RFn_CNM_CM_Pos             6
#define RF215_RFn_CNM_CM_Msk             (0x3 << RF215_RFn_CNM_CM_Pos)
#define RF215_RFn_CNM_CM(x)              (((x) << RF215_RFn_CNM_CM_Pos) & RF215_RFn_CNM_CM_Msk)
#define RF215_RFn_CNM_CM_IEEE            RF215_RFn_CNM_CM(0) /* IEEE compliant channel scheme; f=(CCF0+CN*CS)*25kHz+foffset; (foffset = 0Hz for sub-1GHz transceiver; foffset = 1.5GHz for 2.4GHz transceiver) */
#define RF215_RFn_CNM_CM_FINE_389        RF215_RFn_CNM_CM(1) /* Fine resolution (389.5-510.0)MHz with 99.182Hz channel stepping */
#define RF215_RFn_CNM_CM_FINE_779        RF215_RFn_CNM_CM(2) /* Fine resolution (779-1020)MHz with 198.364Hz channel stepping */
#define RF215_RFn_CNM_CM_FINE_2400       RF215_RFn_CNM_CM(3) /* Fine resolution (2400-2483.5)MHz with 396.728Hz channel stepping */

/** RFn_RXBWC - Receiver Filter Bandwidth Control */
/* RXBWC.BW: Receiver Bandwidth */
#define RF215_RFn_RXBWC_BW_Pos           0
#define RF215_RFn_RXBWC_BW_Msk           (0xF << RF215_RFn_RXBWC_BW_Pos)
#define RF215_RFn_RXBWC_BW(x)            (((x) << RF215_RFn_RXBWC_BW_Pos) & RF215_RFn_RXBWC_BW_Msk)
#define RF215_RFn_RXBWC_BW160_IF250kHz   RF215_RFn_RXBWC_BW(0)
#define RF215_RFn_RXBWC_BW200_IF250kHz   RF215_RFn_RXBWC_BW(1)
#define RF215_RFn_RXBWC_BW250_IF250kHz   RF215_RFn_RXBWC_BW(2)
#define RF215_RFn_RXBWC_BW320_IF500kHz   RF215_RFn_RXBWC_BW(3)
#define RF215_RFn_RXBWC_BW400_IF500kHz   RF215_RFn_RXBWC_BW(4)
#define RF215_RFn_RXBWC_BW500_IF500kHz   RF215_RFn_RXBWC_BW(5)
#define RF215_RFn_RXBWC_BW630_IF1000kHz  RF215_RFn_RXBWC_BW(6)
#define RF215_RFn_RXBWC_BW800_IF1000kHz  RF215_RFn_RXBWC_BW(7)
#define RF215_RFn_RXBWC_BW1000_IF1000kHz RF215_RFn_RXBWC_BW(8)
#define RF215_RFn_RXBWC_BW1250_IF2000kHz RF215_RFn_RXBWC_BW(9)
#define RF215_RFn_RXBWC_BW1600_IF2000kHz RF215_RFn_RXBWC_BW(10)
#define RF215_RFn_RXBWC_BW2000_IF2000kHz RF215_RFn_RXBWC_BW(11) /* Default */
/* Bit 4 - RXBWC.IFS: IF Shift */
#define RF215_RFn_RXBWC_IFS              (1 << 4)
/* Bit 5 - RXBWC.IFI: IF Inversion */
#define RF215_RFn_RXBWC_IFI              (1 << 5)

/** RFn_RXDFE - Receiver Digital Frontend */
/* Bit 3:0 - RXDFE.SR: RX Sample Rate */
#define RF215_RFn_RXDFE_SR_Pos          0
#define RF215_RFn_RXDFE_SR_Msk          (0xF << RF215_RFn_RXDFE_SR_Pos)
#define RF215_RFn_RXDFE_SR(x)           (((x) << RF215_RFn_RXDFE_SR_Pos) & RF215_RFn_RXDFE_SR_Msk)
#define RF215_RFn_RXDFE_SR_4000kHz      RF215_RFn_RXDFE_SR(1) /* Default */
#define RF215_RFn_RXDFE_SR_2000kHz      RF215_RFn_RXDFE_SR(2)
#define RF215_RFn_RXDFE_SR_1333kHz      RF215_RFn_RXDFE_SR(3)
#define RF215_RFn_RXDFE_SR_1000kHz      RF215_RFn_RXDFE_SR(4)
#define RF215_RFn_RXDFE_SR_800kHz       RF215_RFn_RXDFE_SR(5)
#define RF215_RFn_RXDFE_SR_667kHz       RF215_RFn_RXDFE_SR(6)
#define RF215_RFn_RXDFE_SR_500kHz       RF215_RFn_RXDFE_SR(8)
#define RF215_RFn_RXDFE_SR_400kHz       RF215_RFn_RXDFE_SR(10)
/* Bit 7:5 - RXDFE.RCUT: RX filter relative cut-off frequency */
#define RF215_RFn_RXDFE_RCUT_Pos        5
#define RF215_RFn_RXDFE_RCUT_Msk        (0x7 << RF215_RFn_RXDFE_RCUT_Pos)
#define RF215_RFn_RXDFE_RCUT(x)         (((x) << RF215_RFn_RXDFE_RCUT_Pos) & RF215_RFn_RXDFE_RCUT_Msk)
#define RF215_RFn_RXDFE_RCUT_0_25       RF215_RFn_RXDFE_RCUT(0) /* fCUT=0.25 *fS/2; Default */
#define RF215_RFn_RXDFE_RCUT_0_375      RF215_RFn_RXDFE_RCUT(1) /* fCUT=0.375 *fS/2 */
#define RF215_RFn_RXDFE_RCUT_0_5        RF215_RFn_RXDFE_RCUT(2) /* fCUT=0.5 *fS/2 */
#define RF215_RFn_RXDFE_RCUT_0_75       RF215_RFn_RXDFE_RCUT(3) /* fCUT=0.75 *fS/2 */
#define RF215_RFn_RXDFE_RCUT_1_00       RF215_RFn_RXDFE_RCUT(4) /* fCUT=1.00 *fS/2 */

/** RFn_AGCC - Receiver AGC Control 0 */
/* Bit 0 - AGCC.EN: AGC Enable */
#define RF215_RFn_AGCC_EN                (1 << 0)
/* Bit 1 - AGCC.FRZC: AGC Freeze Control */
#define RF215_RFn_AGCC_FRZC              (1 << 1)
/* Bit 2 - AGCC.FRZS: AGC Freeze Status */
#define RF215_RFn_AGCC_FRZS              (1 << 2)
/* Bit 3 - AGCC.RST: AGC Reset */
#define RF215_RFn_AGCC_RST               (1 << 3)
/* Bit 5:4 - AGCC.AVGS: AGC Average Time in Number of Samples */
#define RF215_RFn_AGCC_AVGS_Pos          4
#define RF215_RFn_AGCC_AVGS_Msk          (0x3 << RF215_RFn_AGCC_AVGS_Pos)
#define RF215_RFn_AGCC_AVGS(x)           (((x) << RF215_RFn_AGCC_AVGS_Pos) & RF215_RFn_AGCC_AVGS_Msk)
#define RF215_RFn_AGCC_AVGS_8SAMP        RF215_RFn_AGCC_AVGS(0) /* Default */
#define RF215_RFn_AGCC_AVGS_16SAMP       RF215_RFn_AGCC_AVGS(1)
#define RF215_RFn_AGCC_AVGS_32SAMP       RF215_RFn_AGCC_AVGS(2)
#define RF215_RFn_AGCC_AVGS_64SAMP       RF215_RFn_AGCC_AVGS(3)
/* Bit 6 - AGCC.AGCI: AGC Input */
#define RF215_RFn_AGCC_AGCI_Pos          6
#define RF215_RFn_AGCC_AGCI_Msk          (0x1 << RF215_RFn_AGCC_AGCI_Pos)
#define RF215_RFn_AGCC_AGCI(x)           (((x) << RF215_RFn_AGCC_AGCI_Pos) & RF215_RFn_AGCC_AGCI_Msk)
#define RF215_RFn_AGCC_AGCI_x0           RF215_RFn_AGCC_AGCI(0) /* Default */
#define RF215_RFn_AGCC_AGCI_x1           RF215_RFn_AGCC_AGCI(1)
/* Bit 7 - ?? */
#define RF215_RFn_AGCC_RSV               (1 << 7)

/** RFn_AGCS - Receiver AGCG */
/* Bit 4:0 - AGCS.GCW: RX Gain Control Word */
#define RF215_RFn_AGCS_GCW_Pos          0
#define RF215_RFn_AGCS_GCW_Msk          (0x1F << RF215_RFn_AGCS_GCW_Pos)
#define RF215_RFn_AGCS_GCW(x)           (((x) << RF215_RFn_AGCS_GCW_Pos) & RF215_RFn_AGCS_GCW_Msk)
/* Bit 7:5 - AGCS.TGT: AGC Target Level */
#define RF215_RFn_AGCS_TGT_Pos          5
#define RF215_RFn_AGCS_TGT_Msk          (0x7 << RF215_RFn_AGCS_TGT_Pos)
#define RF215_RFn_AGCS_TGT(x)           (((x) << RF215_RFn_AGCS_TGT_Pos) & RF215_RFn_AGCS_TGT_Msk)
#define RF215_RFn_AGCS_TGT_21dB         RF215_RFn_AGCS_TGT(0)
#define RF215_RFn_AGCS_TGT_24dB         RF215_RFn_AGCS_TGT(1)
#define RF215_RFn_AGCS_TGT_27dB         RF215_RFn_AGCS_TGT(2)
#define RF215_RFn_AGCS_TGT_30dB         RF215_RFn_AGCS_TGT(3) /* Default */
#define RF215_RFn_AGCS_TGT_33dB         RF215_RFn_AGCS_TGT(4)
#define RF215_RFn_AGCS_TGT_36dB         RF215_RFn_AGCS_TGT(5)
#define RF215_RFn_AGCS_TGT_39dB         RF215_RFn_AGCS_TGT(6)
#define RF215_RFn_AGCS_TGT_42dB         RF215_RFn_AGCS_TGT(7)

/** RFn_EDC - Energy Detection Configuration */
/* Bit 1:0 - EDC.EDM: Energy Detection Mode */
#define RF215_RFn_EDC_EDM_Pos           0
#define RF215_RFn_EDC_EDM_Msk           (0x3 << RF215_RFn_EDC_EDM_Pos)
#define RF215_RFn_EDC_EDM(x)            (((x) << RF215_RFn_EDC_EDM_Pos) & RF215_RFn_EDC_EDM_Msk)
#define RF215_RFn_EDC_EDM_AUTO          RF215_RFn_EDC_EDM(0) /* Default */
#define RF215_RFn_EDC_EDM_SINGLE        RF215_RFn_EDC_EDM(1)
#define RF215_RFn_EDC_EDM_CONT          RF215_RFn_EDC_EDM(2)
#define RF215_RFn_EDC_EDM_OFF           RF215_RFn_EDC_EDM(3)

/** RFn_EDD - Receiver Energy Detection Averaging Duration */
/* Bit 1:0 - EDD.DTB: Receiver energy detection average duration time basis */
#define RF215_RFn_EDD_DTB_Pos           0
#define RF215_RFn_EDD_DTB_Msk           (0x3 << RF215_RFn_EDD_DTB_Pos)
#define RF215_RFn_EDD_DTB(x)            (((x) << RF215_RFn_EDD_DTB_Pos) & RF215_RFn_EDD_DTB_Msk)
#define RF215_RFn_EDD_DTB_2us           RF215_RFn_EDD_DTB(0)
#define RF215_RFn_EDD_DTB_8us           RF215_RFn_EDD_DTB(1) /* Default */
#define RF215_RFn_EDD_DTB_32us          RF215_RFn_EDD_DTB(2)
#define RF215_RFn_EDD_DTB_128us         RF215_RFn_EDD_DTB(3)
/* Bit 7:2 - EDD.DF: Receiver energy detection duration factor */
#define RF215_RFn_EDD_DF_Pos            2
#define RF215_RFn_EDD_DF_Msk            (0x3F << RF215_RFn_EDD_DF_Pos)
#define RF215_RFn_EDD_DF(x)             (((x) << RF215_RFn_EDD_DF_Pos) & RF215_RFn_EDD_DF_Msk)

/** RFn_TXCUTC - Transmitter Filter Cutoff Control and PA Ramp Time */
/* Bit 3:0 - TXCUTC.LPFCUT: Transmitter cut-off frequency */
#define RF215_RFn_TXCUTC_LPFCUT_Pos      0
#define RF215_RFn_TXCUTC_LPFCUT_Msk      (0xF << RF215_RFn_TXCUTC_LPFCUT_Pos)
#define RF215_RFn_TXCUTC_LPFCUT(x)       (((x) << RF215_RFn_TXCUTC_LPFCUT_Pos) & RF215_RFn_TXCUTC_LPFCUT_Msk)
#define RF215_RFn_TXCUTC_LPFCUT_80kHz    RF215_RFn_TXCUTC_LPFCUT(0)
#define RF215_RFn_TXCUTC_LPFCUT_100kHz   RF215_RFn_TXCUTC_LPFCUT(1)
#define RF215_RFn_TXCUTC_LPFCUT_125kHz   RF215_RFn_TXCUTC_LPFCUT(2)
#define RF215_RFn_TXCUTC_LPFCUT_160kHz   RF215_RFn_TXCUTC_LPFCUT(3)
#define RF215_RFn_TXCUTC_LPFCUT_200kHz   RF215_RFn_TXCUTC_LPFCUT(4)
#define RF215_RFn_TXCUTC_LPFCUT_250kHz   RF215_RFn_TXCUTC_LPFCUT(5)
#define RF215_RFn_TXCUTC_LPFCUT_315kHz   RF215_RFn_TXCUTC_LPFCUT(6)
#define RF215_RFn_TXCUTC_LPFCUT_400kHz   RF215_RFn_TXCUTC_LPFCUT(7)
#define RF215_RFn_TXCUTC_LPFCUT_500kHz   RF215_RFn_TXCUTC_LPFCUT(8) /* Default */
#define RF215_RFn_TXCUTC_LPFCUT_625kHz   RF215_RFn_TXCUTC_LPFCUT(9)
#define RF215_RFn_TXCUTC_LPFCUT_800kHz   RF215_RFn_TXCUTC_LPFCUT(10)
#define RF215_RFn_TXCUTC_LPFCUT_1000kHz  RF215_RFn_TXCUTC_LPFCUT(11)
/* Bit 7:6 - TXCUTC.PARAMP: Power Amplifier Ramp Time */
#define RF215_RFn_TXCUTC_PARAMP_Pos      6
#define RF215_RFn_TXCUTC_PARAMP_Msk      (0x3 << RF215_RFn_TXCUTC_PARAMP_Pos)
#define RF215_RFn_TXCUTC_PARAMP(x)       (((x) << RF215_RFn_TXCUTC_PARAMP_Pos) & RF215_RFn_TXCUTC_PARAMP_Msk)
#define RF215_RFn_TXCUTC_PARAMP_4us      RF215_RFn_TXCUTC_PARAMP(0) /* Default */
#define RF215_RFn_TXCUTC_PARAMP_8us      RF215_RFn_TXCUTC_PARAMP(1)
#define RF215_RFn_TXCUTC_PARAMP_16us     RF215_RFn_TXCUTC_PARAMP(2)
#define RF215_RFn_TXCUTC_PARAMP_32us     RF215_RFn_TXCUTC_PARAMP(3)

/** RFn_TXDFE - Transmitter TX Digital Frontend */
/* Bit 3:0 - TXDFE.SR: TX Sample Rate */
#define RF215_RFn_TXDFE_SR_Pos           0
#define RF215_RFn_TXDFE_SR_Msk           (0xF << RF215_RFn_TXDFE_SR_Pos)
#define RF215_RFn_TXDFE_SR(x)            (((x) << RF215_RFn_TXDFE_SR_Pos) & RF215_RFn_TXDFE_SR_Msk)
#define RF215_RFn_TXDFE_SR_4000kHz       RF215_RFn_TXDFE_SR(1) /* Default */
#define RF215_RFn_TXDFE_SR_2000kHz       RF215_RFn_TXDFE_SR(2)
#define RF215_RFn_TXDFE_SR_1333kHz       RF215_RFn_TXDFE_SR(3)
#define RF215_RFn_TXDFE_SR_1000kHz       RF215_RFn_TXDFE_SR(4)
#define RF215_RFn_TXDFE_SR_800kHz        RF215_RFn_TXDFE_SR(5)
#define RF215_RFn_TXDFE_SR_667kHz        RF215_RFn_TXDFE_SR(6)
#define RF215_RFn_TXDFE_SR_500kHz        RF215_RFn_TXDFE_SR(8)
#define RF215_RFn_TXDFE_SR_400kHz        RF215_RFn_TXDFE_SR(10)
/* Bit 4 - TXDFE.DM: Direct Modulation */
#define RF215_RFn_TXDFE_DM_Pos           4
#define RF215_RFn_TXDFE_DM_Msk           (0x1 << RF215_RFn_TXDFE_DM_Pos)
#define RF215_RFn_TXDFE_DM(x)            (((x) << RF215_RFn_TXDFE_DM_Pos) & RF215_RFn_TXDFE_DM_Msk)
#define RF215_RFn_TXDFE_DM_DIS           RF215_RFn_TXDFE_DM(0) /* Default */
#define RF215_RFn_TXDFE_DM_EN            RF215_RFn_TXDFE_DM(1)
/* Bit 7:5 - TXDFE.RCUT: TX filter relative to the cut-off frequency */
#define RF215_RFn_TXDFE_RCUT_Pos         5
#define RF215_RFn_TXDFE_RCUT_Msk         (0x7 << RF215_RFn_TXDFE_RCUT_Pos)
#define RF215_RFn_TXDFE_RCUT(x)          (((x) << RF215_RFn_TXDFE_RCUT_Pos) & RF215_RFn_TXDFE_RCUT_Msk)
#define RF215_RFn_TXDFE_RCUT_0_25        RF215_RFn_TXDFE_RCUT(0) /* fCUT=0.25 *fS/2; Default */
#define RF215_RFn_TXDFE_RCUT_0_375       RF215_RFn_TXDFE_RCUT(1) /* fCUT=0.375 *fS/2 */
#define RF215_RFn_TXDFE_RCUT_0_5         RF215_RFn_TXDFE_RCUT(2) /* fCUT=0.5 *fS/2 */
#define RF215_RFn_TXDFE_RCUT_0_75        RF215_RFn_TXDFE_RCUT(3) /* fCUT=0.75 *fS/2 */
#define RF215_RFn_TXDFE_RCUT_1_00        RF215_RFn_TXDFE_RCUT(4) /* fCUT=1.00 *fS/2 */

/** RFn_PAC - Transmitter Power Amplifier Control */
/* Bit 4:0 - PAC.TXPWR: Transmitter Output Power */
#define RF215_RFn_PAC_TXPWR_Pos          0
#define RF215_RFn_PAC_TXPWR_Msk          (0x1F << RF215_RFn_PAC_TXPWR_Pos)
#define RF215_RFn_PAC_TXPWR(x)           (((x) << RF215_RFn_PAC_TXPWR_Pos) & RF215_RFn_PAC_TXPWR_Msk)
#define RF215_RFn_PAC_TXPWR_MAX          RF215_RFn_PAC_TXPWR(0x1F) /* Maximum output power; Default */
/* Bit 6:5 - PAC.PACUR: Power Amplifier Current Control */
#define RF215_RFn_PAC_PACUR_Pos          5
#define RF215_RFn_PAC_PACUR_Msk          (0x3 << RF215_RFn_PAC_PACUR_Pos)
#define RF215_RFn_PAC_PACUR(x)           (((x) << RF215_RFn_PAC_PACUR_Pos) & RF215_RFn_PAC_PACUR_Msk)
#define RF215_RFn_PAC_PACUR_22mA         RF215_RFn_PAC_PACUR(0) /* Power amplifier current reduction by about 22mA (3dB reduction of max. small signal gain) */
#define RF215_RFn_PAC_PACUR_18mA         RF215_RFn_PAC_PACUR(1) /* Power amplifier current reduction by about 18mA (2dB reduction of max. small signal gain) */
#define RF215_RFn_PAC_PACUR_11mA         RF215_RFn_PAC_PACUR(2) /* Power amplifier current reduction by about 11mA (1dB reduction of max. small signal gain) */
#define RF215_RFn_PAC_PACUR_0mA          RF215_RFn_PAC_PACUR(3) /* No power amplifier current reduction (max. transmit small signal gain); Default */

/** RFn_PLL - Transceiver PLL */
/* Bit 1 - PLL.LS: PLL Lock Status */
#define RF215_RFn_PLL_LS                 (1 << 1)
/* Bit 5:4 - PLL.LBW: Loop Bandwidth */
#define RF215_RFn_PLL_LBW_Pos            4
#define RF215_RFn_PLL_LBW_Msk            (0x3 << RF215_RFn_PLL_LBW_Pos)
#define RF215_RFn_PLL_LBW(x)             (((x) << RF215_RFn_PLL_LBW_Pos) & RF215_RFn_PLL_LBW_Msk)
#define RF215_RFn_PLL_LBW_DEF            RF215_RFn_PLL_LBW(0) /* Default */
#define RF215_RFn_PLL_LBW_SMALLER        RF215_RFn_PLL_LBW(1) /* 15% smaller PLL loopbandwidth */
#define RF215_RFn_PLL_LBW_LARGER         RF215_RFn_PLL_LBW(2) /* 15% larger PLL loopbandwidth */

/** BBCn_PC - PHY Control */
/* Bit 1:0 - PC.PT: PHY Type */
#define RF215_BBCn_PC_PT_Pos             0
#define RF215_BBCn_PC_PT_Msk             (0x3 << RF215_BBCn_PC_PT_Pos)
#define RF215_BBCn_PC_PT(x)              (((x) << RF215_BBCn_PC_PT_Pos) & RF215_BBCn_PC_PT_Msk)
#define RF215_BBCn_PC_PT_OFF             RF215_BBCn_PC_PT(0) /* Default */
#define RF215_BBCn_PC_PT_FSK             RF215_BBCn_PC_PT(1)
#define RF215_BBCn_PC_PT_OFDM            RF215_BBCn_PC_PT(2)
#define RF215_BBCn_PC_PT_OQPSK           RF215_BBCn_PC_PT(3)
/* Bit 2 - PC.BBEN: Baseband Enable */
#define RF215_BBCn_PC_BBEN_Pos           2
#define RF215_BBCn_PC_BBEN_Msk           (0x1 << RF215_BBCn_PC_BBEN_Pos)
#define RF215_BBCn_PC_BBEN(x)            (((x) << RF215_BBCn_PC_BBEN_Pos) & RF215_BBCn_PC_BBEN_Msk)
#define RF215_BBCn_PC_BBEN_OFF           RF215_BBCn_PC_BBEN(0) /* Baseband is not enabled (switched off) */
#define RF215_BBCn_PC_BBEN_ON            RF215_BBCn_PC_BBEN(1) /* Baseband is enabled (switched on); Default */
/* Bit 3 - PC.FCST: Frame Check Sequence Type */
#define RF215_BBCn_PC_FCST_Pos           3
#define RF215_BBCn_PC_FCST_Msk           (0x1 << RF215_BBCn_PC_FCST_Pos)
#define RF215_BBCn_PC_FCST(x)            (((x) << RF215_BBCn_PC_FCST_Pos) & RF215_BBCn_PC_FCST_Msk)
#define RF215_BBCn_PC_FCST_32            RF215_BBCn_PC_FCST(0) /* FCS type 32-bit; Default */
#define RF215_BBCn_PC_FCST_16            RF215_BBCn_PC_FCST(1) /* FCS type 16-bit */
/* Bit 4 - PC.TXAFCS: Transmitter Auto Frame Check Sequence */
#define RF215_BBCn_PC_TXAFCS_Pos         4
#define RF215_BBCn_PC_TXAFCS_Msk         (0x1 << RF215_BBCn_PC_TXAFCS_Pos)
#define RF215_BBCn_PC_TXAFCS(x)          (((x) << RF215_BBCn_PC_TXAFCS_Pos) & RF215_BBCn_PC_TXAFCS_Msk)
#define RF215_BBCn_PC_TXAFCS_DIS         RF215_BBCn_PC_TXAFCS(0) /* FCS not calculated */
#define RF215_BBCn_PC_TXAFCS_EN          RF215_BBCn_PC_TXAFCS(1) /* FCS autonomously calculated; Default */
/* Bit 5 - PC.FCSOK: Frame Check Sequence OK */
#define RF215_BBCn_PC_FCSOK              (1 << 5)
/* Bit 6 - PC.FCSFE: Frame Check Sequence Filter Enable */
#define RF215_BBCn_PC_FCSFE_Pos          6
#define RF215_BBCn_PC_FCSFE_Msk          (0x1 << RF215_BBCn_PC_FCSFE_Pos)
#define RF215_BBCn_PC_FCSFE(x)           (((x) << RF215_BBCn_PC_FCSFE_Pos) & RF215_BBCn_PC_FCSFE_Msk)
#define RF215_BBCn_PC_FCSFE_DIS          RF215_BBCn_PC_FCSFE(0) /* FCS filter disabled */
#define RF215_BBCn_PC_FCSFE_EN           RF215_BBCn_PC_FCSFE(1) /* FCS filter enabled; Default */
/* Bit 7 - PC.CTX: Continuous Transmit */
#define RF215_BBCn_PC_CTX_Pos            7
#define RF215_BBCn_PC_CTX_Msk            (0x1 << RF215_BBCn_PC_CTX_Pos)
#define RF215_BBCn_PC_CTX(x)             (((x) << RF215_BBCn_PC_CTX_Pos) & RF215_BBCn_PC_CTX_Msk)
#define RF215_BBCn_PC_CTX_DIS            RF215_BBCn_PC_CTX(0) /* Continuous transmission disabled; Default */
#define RF215_BBCn_PC_CTX_EN             RF215_BBCn_PC_CTX(1) /* Continuous transmission enabled */

/** BBCn_PS - PHY Status and Settings */
/* Bit 0 - PS.TXUR: TX Underrun */
#define RF215_BBCn_PS_TXUR               (1 << 0)

/** BBCn_RXFLH - RX Frame Length High Byte */
/* Bit 2:0 - RXFLH.RXFLH: RX Frame Length High Byte */
#define RF215_BBCn_RXFLH_RXFLH_Pos       0
#define RF215_BBCn_RXFLH_RXFLH_Msk       (0x7 << RF215_BBCn_RXFLH_RXFLH_Pos)
#define RF215_BBCn_RXFLH_RXFLH(x)        (((x) << RF215_BBCn_RXFLH_RXFLH_Pos) & RF215_BBCn_RXFLH_RXFLH_Msk)

/** BBCn_TXFLH - TX Frame Length High Byte */
/* Bit 2:0 - TXFLH.TXFLH: TX Frame Length High Byte */
#define RF215_BBCn_TXFLH_TXFLH_Pos       0
#define RF215_BBCn_TXFLH_TXFLH_Msk       (0x7 << RF215_BBCn_TXFLH_TXFLH_Pos)
#define RF215_BBCn_TXFLH_TXFLH(x)        (((x) << RF215_BBCn_TXFLH_TXFLH_Pos) & RF215_BBCn_TXFLH_TXFLH_Msk)

/** BBCn_FBLH - Frame Buffer Level High Byte */
/* Bit 2:0 - FBLH.FBLH: RX Frame Length High Byte */
#define RF215_BBCn_FBLH_FBLH_Pos         0
#define RF215_BBCn_FBLH_FBLH_Msk         (0x7 << RF215_BBCn_FBLH_FBLH_Pos)
#define RF215_BBCn_FBLH_FBLH(x)          (((x) << RF215_BBCn_FBLH_FBLH_Pos) & RF215_BBCn_FBLH_FBLH_Msk)

/** BBCn_FBLIH - Frame Buffer Level Interrupt Value High Byte */
/* Bit 2:0 - FBLIH.FBLIH: RX Frame Length High Byte */
#define RF215_BBCn_FBLIH_FBLIH_Pos       0
#define RF215_BBCn_FBLIH_FBLIH_Msk       (0x7 << RF215_BBCn_FBLIH_FBLIH_Pos)
#define RF215_BBCn_FBLIH_FBLIH(x)        (((x) << RF215_BBCn_FBLIH_FBLIH_Pos) & RF215_BBCn_FBLIH_FBLIH_Msk)

/** BBCn_OFDMPHRTX - MR-OFDM Transmitter PHY Header Configuration */
/* Bit 2:0 - OFDMPHRTX.MCS: Modulation and Coding Scheme */
#define RF215_BBCn_OFDMPHRTX_MCS_Pos     0
#define RF215_BBCn_OFDMPHRTX_MCS_Msk     (0x7 << RF215_BBCn_OFDMPHRTX_MCS_Pos)
#define RF215_BBCn_OFDMPHRTX_MCS(x)      (((x) << RF215_BBCn_OFDMPHRTX_MCS_Pos) & RF215_BBCn_OFDMPHRTX_MCS_Msk)
#define RF215_BBCn_OFDMPHRTX_MCS_0       RF215_BBCn_OFDMPHRTX_MCS(0) /* Default */
#define RF215_BBCn_OFDMPHRTX_MCS_1       RF215_BBCn_OFDMPHRTX_MCS(1)
#define RF215_BBCn_OFDMPHRTX_MCS_2       RF215_BBCn_OFDMPHRTX_MCS(2)
#define RF215_BBCn_OFDMPHRTX_MCS_3       RF215_BBCn_OFDMPHRTX_MCS(3)
#define RF215_BBCn_OFDMPHRTX_MCS_4       RF215_BBCn_OFDMPHRTX_MCS(4)
#define RF215_BBCn_OFDMPHRTX_MCS_5       RF215_BBCn_OFDMPHRTX_MCS(5)
#define RF215_BBCn_OFDMPHRTX_MCS_6       RF215_BBCn_OFDMPHRTX_MCS(6)
/* Bit 4 - OFDMPHRTX.RB5: Reserved PHR Bit 5 */
#define RF215_BBCn_OFDMPHRTX_RB5_Pos     4
#define RF215_BBCn_OFDMPHRTX_RB5_Msk     (0x1 << RF215_BBCn_OFDMPHRTX_RB5_Pos)
#define RF215_BBCn_OFDMPHRTX_RB5(x)      (((x) << RF215_BBCn_OFDMPHRTX_RB5_Pos) & RF215_BBCn_OFDMPHRTX_RB5_Msk)
/* Bit 5 - OFDMPHRTX.RB17: Reserved PHR Bit 17 */
#define RF215_BBCn_OFDMPHRTX_RB17_Pos    5
#define RF215_BBCn_OFDMPHRTX_RB17_Msk    (0x1 << RF215_BBCn_OFDMPHRTX_RB17_Pos)
#define RF215_BBCn_OFDMPHRTX_RB17(x)     (((x) << RF215_BBCn_OFDMPHRTX_RB17_Pos) & RF215_BBCn_OFDMPHRTX_RB17_Msk)
/* Bit 6 - OFDMPHRTX.RB18: Reserved PHR Bit 18 */
#define RF215_BBCn_OFDMPHRTX_RB18_Pos    6
#define RF215_BBCn_OFDMPHRTX_RB18_Msk    (0x1 << RF215_BBCn_OFDMPHRTX_RB18_Pos)
#define RF215_BBCn_OFDMPHRTX_RB18(x)     (((x) << RF215_BBCn_OFDMPHRTX_RB18_Pos) & RF215_BBCn_OFDMPHRTX_RB18_Msk)
/* Bit 7 - OFDMPHRTX.RB21: Reserved PHR Bit 21 */
#define RF215_BBCn_OFDMPHRTX_RB21_Pos    7
#define RF215_BBCn_OFDMPHRTX_RB21_Msk    (0x1 << RF215_BBCn_OFDMPHRTX_RB21_Pos)
#define RF215_BBCn_OFDMPHRTX_RB21(x)     (((x) << RF215_BBCn_OFDMPHRTX_RB21_Pos) & RF215_BBCn_OFDMPHRTX_RB21_Msk)

/** BBCn_OFDMPHRRX - MR-OFDM Receiver PHY Header Status */
/* Bit 2:0 - OFDMPHRRX.MCS: Modulation and Coding Scheme */
#define RF215_BBCn_OFDMPHRRX_MCS_Pos     0
#define RF215_BBCn_OFDMPHRRX_MCS_Msk     (0x7 << RF215_BBCn_OFDMPHRRX_MCS_Pos)
#define RF215_BBCn_OFDMPHRRX_MCS(x)      (((x) << RF215_BBCn_OFDMPHRRX_MCS_Pos) & RF215_BBCn_OFDMPHRRX_MCS_Msk)
#define RF215_BBCn_OFDMPHRRX_MCS_0       RF215_BBCn_OFDMPHRRX_MCS(0) /* Default */
#define RF215_BBCn_OFDMPHRRX_MCS_1       RF215_BBCn_OFDMPHRRX_MCS(1)
#define RF215_BBCn_OFDMPHRRX_MCS_2       RF215_BBCn_OFDMPHRRX_MCS(2)
#define RF215_BBCn_OFDMPHRRX_MCS_3       RF215_BBCn_OFDMPHRRX_MCS(3)
#define RF215_BBCn_OFDMPHRRX_MCS_4       RF215_BBCn_OFDMPHRRX_MCS(4)
#define RF215_BBCn_OFDMPHRRX_MCS_5       RF215_BBCn_OFDMPHRRX_MCS(5)
#define RF215_BBCn_OFDMPHRRX_MCS_6       RF215_BBCn_OFDMPHRRX_MCS(6)
/* Bit 3 - OFDMPHRRX.SPC: RX Spurious Compensation */
#define RF215_BBCn_OFDMPHRRX_SPC_Pos     3
#define RF215_BBCn_OFDMPHRRX_SPC_Msk     (0x1 << RF215_BBCn_OFDMPHRRX_SPC_Pos)
#define RF215_BBCn_OFDMPHRRX_SPC(x)      (((x) << RF215_BBCn_OFDMPHRRX_SPC_Pos) & RF215_BBCn_OFDMPHRRX_SPC_Msk)
#define RF215_BBCn_OFDMPHRRX_SPC_DIS     RF215_BBCn_OFDMPHRRX_SPC(0) /* Default */
#define RF215_BBCn_OFDMPHRRX_SPC_EN      RF215_BBCn_OFDMPHRRX_SPC(1) /* This is recommended if the receive channel is a multiple of 26MHz or 32MHz */
/* Bit 4 - OFDMPHRRX.RB5: Reserved PHR Bit 5 */
#define RF215_BBCn_OFDMPHRRX_RB5_Pos     4
#define RF215_BBCn_OFDMPHRRX_RB5_Msk     (0x1 << RF215_BBCn_OFDMPHRRX_RB5_Pos)
#define RF215_BBCn_OFDMPHRRX_RB5(x)      (((x) << RF215_BBCn_OFDMPHRRX_RB5_Pos) & RF215_BBCn_OFDMPHRRX_RB5_Msk)
/* Bit 5 - OFDMPHRRX.RB17: Reserved PHR Bit 17 */
#define RF215_BBCn_OFDMPHRRX_RB17_Pos    5
#define RF215_BBCn_OFDMPHRRX_RB17_Msk    (0x1 << RF215_BBCn_OFDMPHRRX_RB17_Pos)
#define RF215_BBCn_OFDMPHRRX_RB17(x)     (((x) << RF215_BBCn_OFDMPHRRX_RB17_Pos) & RF215_BBCn_OFDMPHRRX_RB17_Msk)
/* Bit 6 - OFDMPHRRX.RB18: Reserved PHR Bit 18 */
#define RF215_BBCn_OFDMPHRRX_RB18_Pos    6
#define RF215_BBCn_OFDMPHRRX_RB18_Msk    (0x1 << RF215_BBCn_OFDMPHRRX_RB18_Pos)
#define RF215_BBCn_OFDMPHRRX_RB18(x)     (((x) << RF215_BBCn_OFDMPHRRX_RB18_Pos) & RF215_BBCn_OFDMPHRRX_RB18_Msk)
/* Bit 7 - OFDMPHRRX.RB21: Reserved PHR Bit 21 */
#define RF215_BBCn_OFDMPHRRX_RB21_Pos    7
#define RF215_BBCn_OFDMPHRRX_RB21_Msk    (0x1 << RF215_BBCn_OFDMPHRRX_RB21_Pos)
#define RF215_BBCn_OFDMPHRRX_RB21(x)     (((x) << RF215_BBCn_OFDMPHRRX_RB21_Pos) & RF215_BBCn_OFDMPHRRX_RB21_Msk)

/** BBCn_OFDMC - MR-OFDM PHY Configuration */
/* Bit 1:0 - OFDMC.OPT: MR-OFDM Bandwidth Option */
#define RF215_BBCn_OFDMC_OPT_Pos         0
#define RF215_BBCn_OFDMC_OPT_Msk         (0x3 << RF215_BBCn_OFDMC_OPT_Pos)
#define RF215_BBCn_OFDMC_OPT(x)          (((x) << RF215_BBCn_OFDMC_OPT_Pos) & RF215_BBCn_OFDMC_OPT_Msk)
#define RF215_BBCn_OFDMC_OPT_1           RF215_BBCn_OFDMC_OPT(0) /* Default */
#define RF215_BBCn_OFDMC_OPT_2           RF215_BBCn_OFDMC_OPT(1)
#define RF215_BBCn_OFDMC_OPT_3           RF215_BBCn_OFDMC_OPT(2)
#define RF215_BBCn_OFDMC_OPT_4           RF215_BBCn_OFDMC_OPT(3)
/* Bit 2 - OFDMC.POI: PIB Attribute phyOFDMInterleaving */
#define RF215_BBCn_OFDMC_POI_Pos         2
#define RF215_BBCn_OFDMC_POI_Msk         (0x1 << RF215_BBCn_OFDMC_POI_Pos)
#define RF215_BBCn_OFDMC_POI(x)          (((x) << RF215_BBCn_OFDMC_POI_Pos) & RF215_BBCn_OFDMC_POI_Msk)
/* Bit 3 - OFDMC.LFO: Reception with Low Frequency Offset */
#define RF215_BBCn_OFDMC_LFO_Pos         3
#define RF215_BBCn_OFDMC_LFO_Msk         (0x1 << RF215_BBCn_OFDMC_LFO_Pos)
#define RF215_BBCn_OFDMC_LFO(x)          (((x) << RF215_BBCn_OFDMC_LFO_Pos) & RF215_BBCn_OFDMC_LFO_Msk)
#define RF215_BBCn_OFDMC_LFO_DIS         RF215_BBCn_OFDMC_LFO(0) /* Default */
#define RF215_BBCn_OFDMC_LFO_EN          RF215_BBCn_OFDMC_LFO(1)
/* Bit 5:4 - OFDMC.SSTX: Transmitter Scrambler Seed Configuration */
#define RF215_BBCn_OFDMC_SSTX_Pos        4
#define RF215_BBCn_OFDMC_SSTX_Msk        (0x3 << RF215_BBCn_OFDMC_SSTX_Pos)
#define RF215_BBCn_OFDMC_SSTX(x)         (((x) << RF215_BBCn_OFDMC_SSTX_Pos) & RF215_BBCn_OFDMC_SSTX_Msk)
/* Bit 7:6 - OFDMC.SSRX: Receiver Scrambler Seed Status */
#define RF215_BBCn_OFDMC_SSRX_Pos        6
#define RF215_BBCn_OFDMC_SSRX_Msk        (0x3 << RF215_BBCn_OFDMC_SSRX_Pos)
#define RF215_BBCn_OFDMC_SSRX(x)         (((x) << RF215_BBCn_OFDMC_SSRX_Pos) & RF215_BBCn_OFDMC_SSRX_Msk)

/** BBCn_OFDMSW - OFDM Switches */
/* Bit 4 - OFDMSW.RXO: Receiver Override */
#define RF215_BBCn_OFDMSW_RXO_Pos         4
#define RF215_BBCn_OFDMSW_RXO_Msk         (0x1 << RF215_BBCn_OFDMSW_RXO_Pos)
#define RF215_BBCn_OFDMSW_RXO(x)          (((x) << RF215_BBCn_OFDMSW_RXO_Pos) & RF215_BBCn_OFDMSW_RXO_Msk)
#define RF215_BBCn_OFDMSW_RXO_OFF         RF215_BBCn_OFDMSW_RXO(0) /* Receiver override disabled */
#define RF215_BBCn_OFDMSW_RXO_12dB        RF215_BBCn_OFDMSW_RXO(1) /* Receiver restarted by > 12dB stronger frame; Default */
/* Bit 7:5 - OFDMSW.PDT: Preamble Detection Threshold */
#define RF215_BBCn_OFDMSW_PDT_Pos         5
#define RF215_BBCn_OFDMSW_PDT_Msk         (0x7 << RF215_BBCn_OFDMSW_PDT_Pos)
#define RF215_BBCn_OFDMSW_PDT(x)          (((x) << RF215_BBCn_OFDMSW_PDT_Pos) & RF215_BBCn_OFDMSW_PDT_Msk)

/** BBCn_AMCS - Auto Mode Configuration and Status */
/* Bit 0 - AMCS.TX2RX: Transmit and Switch to Receive */
#define RF215_BBCn_AMCS_TX2RX            (1 << 0)
/* Bit 1 - AMCS.CCATX: CCA Measurement and automatic Transmit */
#define RF215_BBCn_AMCS_CCATX            (1 << 1)
/* Bit 2 - AMCS.CCAED: CCA Energy Detection Result */
#define RF215_BBCn_AMCS_CCAED            (1 << 2)
/* Bit 3 - AMCS.AACK: Auto Acknowledgement */
#define RF215_BBCn_AMCS_AACK             (1 << 3)
/* Bit 4 - AMCS.AACKS: Auto Acknowledgement Source */
#define RF215_BBCn_AMCS_AACKS            (1 << 4)
/* Bit 5 - AMCS.AACKDR: Auto Acknowledgement Data Rate */
#define RF215_BBCn_AMCS_AACKDR           (1 << 5)
/* Bit 6 - AMCS.AACKFA: Auto Acknowledgement FCS Adaption */
#define RF215_BBCn_AMCS_AACKFA           (1 << 6)
/* Bit 7 - AMCS.AACKFT: Auto Acknowledgement Frame Transmit */
#define RF215_BBCn_AMCS_AACKFT           (1 << 7)

/** BBCn_FSKC0 - FSK Configuration Byte 0 */
/* Bit 0 - FSKC0.MORD: FSK Modulation Order */
#define RF215_BBCn_FSKC0_MORD_Pos        0
#define RF215_BBCn_FSKC0_MORD_Msk        (0x1 << RF215_BBCn_FSKC0_MORD_Pos)
#define RF215_BBCn_FSKC0_MORD(x)         (((x) << RF215_BBCn_FSKC0_MORD_Pos) & RF215_BBCn_FSKC0_MORD_Msk)
#define RF215_BBCn_FSKC0_MORD_2FSK       RF215_BBCn_FSKC0_MORD(0) /* Default */
#define RF215_BBCn_FSKC0_MORD_4FSK       RF215_BBCn_FSKC0_MORD(1)
/* Bit 3:1 - FSKC0.MIDX: Modulation Index */
#define RF215_BBCn_FSKC0_MIDX_Pos        1
#define RF215_BBCn_FSKC0_MIDX_Msk        (0x7 << RF215_BBCn_FSKC0_MIDX_Pos)
#define RF215_BBCn_FSKC0_MIDX(x)         (((x) << RF215_BBCn_FSKC0_MIDX_Pos) & RF215_BBCn_FSKC0_MIDX_Msk)
#define RF215_BBCn_FSKC0_MIDX_0_375      RF215_BBCn_FSKC0_MIDX(0) /* Modulation index = 0.375 */
#define RF215_BBCn_FSKC0_MIDX_0_5        RF215_BBCn_FSKC0_MIDX(1) /* Modulation index = 0.5 */
#define RF215_BBCn_FSKC0_MIDX_0_75       RF215_BBCn_FSKC0_MIDX(2) /* Modulation index = 0.75 */
#define RF215_BBCn_FSKC0_MIDX_1_0        RF215_BBCn_FSKC0_MIDX(3) /* Modulation index = 1.0; Default */
#define RF215_BBCn_FSKC0_MIDX_1_25       RF215_BBCn_FSKC0_MIDX(4) /* Modulation index = 1.25 */
#define RF215_BBCn_FSKC0_MIDX_1_5        RF215_BBCn_FSKC0_MIDX(5) /* Modulation index = 1.5 */
#define RF215_BBCn_FSKC0_MIDX_1_75       RF215_BBCn_FSKC0_MIDX(6) /* Modulation index = 1.75 */
#define RF215_BBCn_FSKC0_MIDX_2_0        RF215_BBCn_FSKC0_MIDX(7) /* Modulation index = 2.0 */
/* Bit 5:4 - FSKC0.MIDXS: Modulation Index Scale */
#define RF215_BBCn_FSKC0_MIDXS_Pos       4
#define RF215_BBCn_FSKC0_MIDXS_Msk       (0x3 << RF215_BBCn_FSKC0_MIDXS_Pos)
#define RF215_BBCn_FSKC0_MIDXS(x)        (((x) << RF215_BBCn_FSKC0_MIDXS_Pos) & RF215_BBCn_FSKC0_MIDXS_Msk)
#define RF215_BBCn_FSKC0_MIDXS_0_875     RF215_BBCn_FSKC0_MIDXS(0) /* s = 1.0 - 1/8 */
#define RF215_BBCn_FSKC0_MIDXS_1_0       RF215_BBCn_FSKC0_MIDXS(1) /* s = 1.0; Default */
#define RF215_BBCn_FSKC0_MIDXS_1_125     RF215_BBCn_FSKC0_MIDXS(2) /* s = 1.0 + 1/8 */
#define RF215_BBCn_FSKC0_MIDXS_1_25      RF215_BBCn_FSKC0_MIDXS(3) /* s = 1.0 + 1/4 */
/* Bit 7:6 - FSKC0.BT: FSK Bandwidth Time Product */
#define RF215_BBCn_FSKC0_BT_Pos          6
#define RF215_BBCn_FSKC0_BT_Msk          (0x3 << RF215_BBCn_FSKC0_BT_Pos)
#define RF215_BBCn_FSKC0_BT(x)           (((x) << RF215_BBCn_FSKC0_BT_Pos) & RF215_BBCn_FSKC0_BT_Msk)
#define RF215_BBCn_FSKC0_BT_0_5          RF215_BBCn_FSKC0_BT(0) /* BT = 0.5 */
#define RF215_BBCn_FSKC0_BT_1_0          RF215_BBCn_FSKC0_BT(1) /* BT = 1.0 */
#define RF215_BBCn_FSKC0_BT_1_5          RF215_BBCn_FSKC0_BT(2) /* BT = 1.5 */
#define RF215_BBCn_FSKC0_BT_2_0          RF215_BBCn_FSKC0_BT(3) /* BT = 2.0; Default */

/** BBCn_FSKC1 - FSK Configuration Byte 1 */
/* Bit 3:0 - FSKC1.SRATE: MR-FSK Symbol Rate */
#define RF215_BBCn_FSKC1_SRATE_Pos       0
#define RF215_BBCn_FSKC1_SRATE_Msk       (0xF << RF215_BBCn_FSKC1_SRATE_Pos)
#define RF215_BBCn_FSKC1_SRATE(x)        (((x) << RF215_BBCn_FSKC1_SRATE_Pos) & RF215_BBCn_FSKC1_SRATE_Msk)
#define RF215_BBCn_FSKC1_SRATE_50kHz     RF215_BBCn_FSKC1_SRATE(0) /* Default */
#define RF215_BBCn_FSKC1_SRATE_100kHz    RF215_BBCn_FSKC1_SRATE(1)
#define RF215_BBCn_FSKC1_SRATE_150kHz    RF215_BBCn_FSKC1_SRATE(2)
#define RF215_BBCn_FSKC1_SRATE_200kHz    RF215_BBCn_FSKC1_SRATE(3)
#define RF215_BBCn_FSKC1_SRATE_300kHz    RF215_BBCn_FSKC1_SRATE(4)
#define RF215_BBCn_FSKC1_SRATE_400kHz    RF215_BBCn_FSKC1_SRATE(5)
/* Bit 5 - FSKC1.FI: Frequency Inversion */
#define RF215_BBCn_FSKC1_IFI_Pos         5
#define RF215_BBCn_FSKC1_IFI_Msk         (0x1 << RF215_BBCn_FSKC1_IFI_Pos)
#define RF215_BBCn_FSKC1_IFI(x)          (((x) << RF215_BBCn_FSKC1_IFI_Pos) & RF215_BBCn_FSKC1_IFI_Msk)
#define RF215_BBCn_FSKC1_IFI_DIS         RF215_BBCn_FSKC1_IFI(0) /* Default */
#define RF215_BBCn_FSKC1_IFI_EN          RF215_BBCn_FSKC1_IFI(1) /* The sign of the FSK deviation frequency is inverted */
/* Bit 7:6 - FSKC1.FSKPLH: FSK Preamble Length High Byte */
#define RF215_BBCn_FSKC1_FSKPLH_Pos      6
#define RF215_BBCn_FSKC1_FSKPLH_Msk      (0x3 << RF215_BBCn_FSKC1_FSKPLH_Pos)
#define RF215_BBCn_FSKC1_FSKPLH(x)       (((x) << RF215_BBCn_FSKC1_FSKPLH_Pos) & RF215_BBCn_FSKC1_FSKPLH_Msk)

/** BBCn_FSKC2 - FSK Configuration Byte 2 */
/* Bit 0 - FSKC2.FECIE: FEC Interleaving Enable */
#define RF215_BBCn_FSKC2_FECIE_Pos       0
#define RF215_BBCn_FSKC2_FECIE_Msk       (0x1 << RF215_BBCn_FSKC2_FECIE_Pos)
#define RF215_BBCn_FSKC2_FECIE(x)        (((x) << RF215_BBCn_FSKC2_FECIE_Pos) & RF215_BBCn_FSKC2_FECIE_Msk)
#define RF215_BBCn_FSKC2_FECIE_DIS       RF215_BBCn_FSKC2_FECIE(0) /* Interleaving disabled */
#define RF215_BBCn_FSKC2_FECIE_EN        RF215_BBCn_FSKC2_FECIE(1) /* Interleaving enabled; Default */
/* Bit 1 - FSKC2.FECS: FEC Scheme */
#define RF215_BBCn_FSKC2_FECS_Pos        1
#define RF215_BBCn_FSKC2_FECS_Msk        (0x1 << RF215_BBCn_FSKC2_FECS_Pos)
#define RF215_BBCn_FSKC2_FECS(x)         (((x) << RF215_BBCn_FSKC2_FECS_Pos) & RF215_BBCn_FSKC2_FECS_Msk)
#define RF215_BBCn_FSKC2_FECS_NRNSC      RF215_BBCn_FSKC2_FECS(0) /* Default */
#define RF215_BBCn_FSKC2_FECS_RSC        RF215_BBCn_FSKC2_FECS(1)
/* Bit 2 - FSKC2.PRI: Preamble Inversion */
#define RF215_BBCn_FSKC2_PRI_Pos         2
#define RF215_BBCn_FSKC2_PRI_Msk         (0x1 << RF215_BBCn_FSKC2_PRI_Pos)
#define RF215_BBCn_FSKC2_PRI(x)          (((x) << RF215_BBCn_FSKC2_PRI_Pos) & RF215_BBCn_FSKC2_PRI_Msk)
#define RF215_BBCn_FSKC2_PRI_DIS         RF215_BBCn_FSKC2_PRI(0) /* Default */
#define RF215_BBCn_FSKC2_PRI_EN          RF215_BBCn_FSKC2_PRI(1) /* The sign of the FSK deviation frequency belonging to the preamble part is inverted */
/* Bit 3 - FSKC2.MSE: Mode Switch Enable */
#define RF215_BBCn_FSKC2_MSE_Pos         3
#define RF215_BBCn_FSKC2_MSE_Msk         (0x1 << RF215_BBCn_FSKC2_MSE_Pos)
#define RF215_BBCn_FSKC2_MSE(x)          (((x) << RF215_BBCn_FSKC2_MSE_Pos) & RF215_BBCn_FSKC2_MSE_Msk)
#define RF215_BBCn_FSKC2_MSE_DIS         RF215_BBCn_FSKC2_MSE(0) /* Mode Switch disabled; Default */
#define RF215_BBCn_FSKC2_MSE_EN          RF215_BBCn_FSKC2_MSE(1) /* Mode Switch enabled */
/* Bit 4 - FSKC2.RXPTO: Receiver Preamble Time Out */
#define RF215_BBCn_FSKC2_RXPTO_Pos       4
#define RF215_BBCn_FSKC2_RXPTO_Msk       (0x1 << RF215_BBCn_FSKC2_RXPTO_Pos)
#define RF215_BBCn_FSKC2_RXPTO(x)        (((x) << RF215_BBCn_FSKC2_RXPTO_Pos) & RF215_BBCn_FSKC2_RXPTO_Msk)
#define RF215_BBCn_FSKC2_RXPTO_DIS       RF215_BBCn_FSKC2_RXPTO(0) /* Receiver preamble timeout disabled; Default */
#define RF215_BBCn_FSKC2_RXPTO_EN        RF215_BBCn_FSKC2_RXPTO(1) /* Receiver preamble timeout enabled */
/* Bit 6:5 - FSKC2.RXO: Receiver Override */
#define RF215_BBCn_FSKC2_RXO_Pos         5
#define RF215_BBCn_FSKC2_RXO_Msk         (0x3 << RF215_BBCn_FSKC2_RXO_Pos)
#define RF215_BBCn_FSKC2_RXO(x)          (((x) << RF215_BBCn_FSKC2_RXO_Pos) & RF215_BBCn_FSKC2_RXO_Msk)
#define RF215_BBCn_FSKC2_RXO_6dB         RF215_BBCn_FSKC2_RXO(0) /* Receiver restarted by > 6dB stronger frame */
#define RF215_BBCn_FSKC2_RXO_12dB        RF215_BBCn_FSKC2_RXO(1) /* Receiver restarted by > 12dB stronger frame */
#define RF215_BBCn_FSKC2_RXO_18dB        RF215_BBCn_FSKC2_RXO(2) /* Receiver restarted by > 18dB stronger frame; Default */
#define RF215_BBCn_FSKC2_RXO_OFF         RF215_BBCn_FSKC2_RXO(3) /* Receiver override disabled */
/* Bit 7 - FSKC2.PDTM: Preamble Detection Mode */
#define RF215_BBCn_FSKC2_PDTM_Pos        7
#define RF215_BBCn_FSKC2_PDTM_Msk        (0x1 << RF215_BBCn_FSKC2_PDTM_Pos)
#define RF215_BBCn_FSKC2_PDTM(x)         (((x) << RF215_BBCn_FSKC2_PDTM_Pos) & RF215_BBCn_FSKC2_PDTM_Msk)
#define RF215_BBCn_FSKC2_PDTM_NORSSI     RF215_BBCn_FSKC2_PDTM(0) /* Preamble detection does not take RSSI values into account; Default */
#define RF215_BBCn_FSKC2_PDTM_RSSI       RF215_BBCn_FSKC2_PDTM(1) /* Preamble detection takes RSSI values into account */

/** BBCn_FSKC3 - FSK Configuration Byte 3*/
/* Bit 3:0 - FSKC3.PDT: Preamble Detection Threshold */
#define RF215_BBCn_FSKC3_PDT_Pos         0
#define RF215_BBCn_FSKC3_PDT_Msk         (0xF << RF215_BBCn_FSKC3_PDT_Pos)
#define RF215_BBCn_FSKC3_PDT(x)          (((x) << RF215_BBCn_FSKC3_PDT_Pos) & RF215_BBCn_FSKC3_PDT_Msk)
/* Bit 7:4 - FSKC3.SFDT: SFD Detection Threshold */
#define RF215_BBCn_FSKC3_SFDT_Pos        4
#define RF215_BBCn_FSKC3_SFDT_Msk        (0xF << RF215_BBCn_FSKC3_SFDT_Pos)
#define RF215_BBCn_FSKC3_SFDT(x)         (((x) << RF215_BBCn_FSKC3_SFDT_Pos) & RF215_BBCn_FSKC3_SFDT_Msk)

/** BBCn_FSKC4 - FSK Configuration Byte 4 */
/* Bit 1:0 - FSKC4.CSFD0: Configuration of PPDU with SFD0 */
#define RF215_BBCn_FSKC4_CSFD0_Pos       0
#define RF215_BBCn_FSKC4_CSFD0_Msk       (0x3 << RF215_BBCn_FSKC4_CSFD0_Pos)
#define RF215_BBCn_FSKC4_CSFD0(x)        (((x) << RF215_BBCn_FSKC4_CSFD0_Pos) & RF215_BBCn_FSKC4_CSFD0_Msk)
#define RF215_BBCn_FSKC4_CSFD0_U_IEEE    RF215_BBCn_FSKC4_CSFD0(0) /* Uncoded IEEE mode; Default */
#define RF215_BBCn_FSKC4_CSFD0_U_RAW     RF215_BBCn_FSKC4_CSFD0(1) /* Uncoded RAW mode */
#define RF215_BBCn_FSKC4_CSFD0_C_IEEE    RF215_BBCn_FSKC4_CSFD0(2) /* Coded IEEE mode */
#define RF215_BBCn_FSKC4_CSFD0_C_RAW     RF215_BBCn_FSKC4_CSFD0(3) /* Coded RAW mode */
/* Bit 3:2 - FSKC4.CSFD1: Configuration of PPDU with SFD1 */
#define RF215_BBCn_FSKC4_CSFD1_Pos       2
#define RF215_BBCn_FSKC4_CSFD1_Msk       (0x3 << RF215_BBCn_FSKC4_CSFD1_Pos)
#define RF215_BBCn_FSKC4_CSFD1(x)        (((x) << RF215_BBCn_FSKC4_CSFD1_Pos) & RF215_BBCn_FSKC4_CSFD1_Msk)
#define RF215_BBCn_FSKC4_CSFD1_U_IEEE    RF215_BBCn_FSKC4_CSFD1(0) /* Uncoded IEEE mode */
#define RF215_BBCn_FSKC4_CSFD1_U_RAW     RF215_BBCn_FSKC4_CSFD1(1) /* Uncoded RAW mode */
#define RF215_BBCn_FSKC4_CSFD1_C_IEEE    RF215_BBCn_FSKC4_CSFD1(2) /* Coded IEEE mode; Default */
#define RF215_BBCn_FSKC4_CSFD1_C_RAW     RF215_BBCn_FSKC4_CSFD1(3) /* Coded RAW mode */
/* Bit 4 - FSKC4.RAWRBIT: RAW mode Reversal Bit */
#define RF215_BBCn_FSKC4_RAWRBIT_Pos     4
#define RF215_BBCn_FSKC4_RAWRBIT_Msk     (0x1 << RF215_BBCn_FSKC4_RAWRBIT_Pos)
#define RF215_BBCn_FSKC4_RAWRBIT(x)      (((x) << RF215_BBCn_FSKC4_RAWRBIT_Pos) & RF215_BBCn_FSKC4_RAWRBIT_Msk)
#define RF215_BBCn_FSKC4_RAWRBIT_LSB     RF215_BBCn_FSKC4_RAWRBIT(0) /* The order is LSB first */
#define RF215_BBCn_FSKC4_RAWRBIT_MSB     RF215_BBCn_FSKC4_RAWRBIT(1) /* The order is MSB first; Default */
/* Bit 5 - FSKC4.SFD32: SFD 32Bit */
#define RF215_BBCn_FSKC4_SFD32_Pos       5
#define RF215_BBCn_FSKC4_SFD32_Msk       (0x1 << RF215_BBCn_FSKC4_SFD32_Pos)
#define RF215_BBCn_FSKC4_SFD32(x)        (((x) << RF215_BBCn_FSKC4_SFD32_Pos) & RF215_BBCn_FSKC4_SFD32_Msk)
#define RF215_BBCn_FSKC4_SFD32_DUAL      RF215_BBCn_FSKC4_SFD32(0) /* Search for two 16-bit SFD; Default */
#define RF215_BBCn_FSKC4_SFD32_SINGLE    RF215_BBCn_FSKC4_SFD32(1) /* Search for a single 32-bit SFD */
/* Bit 6 - FSKC4.SFDQ: SFD Quantization */
#define RF215_BBCn_FSKC4_SFDQ_Pos        6
#define RF215_BBCn_FSKC4_SFDQ_Msk        (0x1 << RF215_BBCn_FSKC4_SFDQ_Pos)
#define RF215_BBCn_FSKC4_SFDQ(x)         (((x) << RF215_BBCn_FSKC4_SFDQ_Pos) & RF215_BBCn_FSKC4_SFDQ_Msk)
#define RF215_BBCn_FSKC4_SFDQ_SOFT       RF215_BBCn_FSKC4_SFDQ(0) /* The FSK receiver applies a soft decisions at the bit positions with regard to SFD search; Default */
#define RF215_BBCn_FSKC4_SFDQ_HARD       RF215_BBCn_FSKC4_SFDQ(1) /* The FSK receiver applies a hard decisions at the bit positions with regard to SFD search */

/** BBCn_FSKPHRTX - FSK PHR TX Information */
/* Bit 0 - FSKPHRTX.RB1: Reserved PHR Bit 1 */
#define RF215_BBCn_FSKPHRTX_RB1_Pos      0
#define RF215_BBCn_FSKPHRTX_RB1_Msk      (0x1 << RF215_BBCn_FSKPHRTX_RB1_Pos)
#define RF215_BBCn_FSKPHRTX_RB1(x)       (((x) << RF215_BBCn_FSKPHRTX_RB1_Pos) & RF215_BBCn_FSKPHRTX_RB1_Msk)
/* Bit 1 - FSKPHRTX.RB2: Reserved PHR Bit 2 */
#define RF215_BBCn_FSKPHRTX_RB2_Pos      1
#define RF215_BBCn_FSKPHRTX_RB2_Msk      (0x1 << RF215_BBCn_FSKPHRTX_RB2_Pos)
#define RF215_BBCn_FSKPHRTX_RB2(x)       (((x) << RF215_BBCn_FSKPHRTX_RB2_Pos) & RF215_BBCn_FSKPHRTX_RB2_Msk)
/* Bit 2 - FSKPHRTX.DW: Data Whitening */
#define RF215_BBCn_FSKPHRTX_DW_Pos       2
#define RF215_BBCn_FSKPHRTX_DW_Msk       (0x1 << RF215_BBCn_FSKPHRTX_DW_Pos)
#define RF215_BBCn_FSKPHRTX_DW(x)        (((x) << RF215_BBCn_FSKPHRTX_DW_Pos) & RF215_BBCn_FSKPHRTX_DW_Msk)
#define RF215_BBCn_FSKPHRTX_DW_DIS       RF215_BBCn_FSKPHRTX_DW(0) /* PSDU data whitening disabled */
#define RF215_BBCn_FSKPHRTX_DW_EN        RF215_BBCn_FSKPHRTX_DW(1) /* PSDU data whitening enabled; Default */
/* Bit 3 - FSKPHRTX.SFD: SFD type */
#define RF215_BBCn_FSKPHRTX_SFD_Pos      3
#define RF215_BBCn_FSKPHRTX_SFD_Msk      (0x1 << RF215_BBCn_FSKPHRTX_SFD_Pos)
#define RF215_BBCn_FSKPHRTX_SFD(x)       (((x) << RF215_BBCn_FSKPHRTX_SFD_Pos) & RF215_BBCn_FSKPHRTX_SFD_Msk)
#define RF215_BBCn_FSKPHRTX_SFD_0        RF215_BBCn_FSKPHRTX_SFD(0) /* SFD0 is used; Default */
#define RF215_BBCn_FSKPHRTX_SFD_1        RF215_BBCn_FSKPHRTX_SFD(1) /* SFD1 is used*/

/** BBCn_FSKPHRRX - FSK PHR RX Information */
/* Bit 0 - FSKPHRRX.RB1: Reserved PHR Bit 1 */
#define RF215_BBCn_FSKPHRRX_RB1_Pos      0
#define RF215_BBCn_FSKPHRRX_RB1_Msk      (0x1 << RF215_BBCn_FSKPHRRX_RB1_Pos)
#define RF215_BBCn_FSKPHRRX_RB1(x)       (((x) << RF215_BBCn_FSKPHRRX_RB1_Pos) & RF215_BBCn_FSKPHRRX_RB1_Msk)
/* Bit 1 - FSKPHRRX.RB2: Reserved PHR Bit 2 */
#define RF215_BBCn_FSKPHRRX_RB2_Pos      1
#define RF215_BBCn_FSKPHRRX_RB2_Msk      (0x1 << RF215_BBCn_FSKPHRRX_RB2_Pos)
#define RF215_BBCn_FSKPHRRX_RB2(x)       (((x) << RF215_BBCn_FSKPHRRX_RB2_Pos) & RF215_BBCn_FSKPHRRX_RB2_Msk)
/* Bit 2 - FSKPHRRX.DW: Data Whitening */
#define RF215_BBCn_FSKPHRRX_DW_Pos       2
#define RF215_BBCn_FSKPHRRX_DW_Msk       (0x1 << RF215_BBCn_FSKPHRRX_DW_Pos)
#define RF215_BBCn_FSKPHRRX_DW(x)        (((x) << RF215_BBCn_FSKPHRRX_DW_Pos) & RF215_BBCn_FSKPHRRX_DW_Msk)
#define RF215_BBCn_FSKPHRRX_DW_DIS       RF215_BBCn_FSKPHRRX_DW(0) /* PSDU data whitening disabled */
#define RF215_BBCn_FSKPHRRX_DW_EN        RF215_BBCn_FSKPHRRX_DW(1) /* PSDU data whitening enabled */
/* Bit 3 - FSKPHRRX.SFD: SFD type */
#define RF215_BBCn_FSKPHRRX_SFD_Pos      3
#define RF215_BBCn_FSKPHRRX_SFD_Msk      (0x1 << RF215_BBCn_FSKPHRRX_SFD_Pos)
#define RF215_BBCn_FSKPHRRX_SFD(x)       (((x) << RF215_BBCn_FSKPHRRX_SFD_Pos) & RF215_BBCn_FSKPHRRX_SFD_Msk)
#define RF215_BBCn_FSKPHRRX_SFD_0        RF215_BBCn_FSKPHRRX_SFD(0) /* SFD0 detected */
#define RF215_BBCn_FSKPHRRX_SFD_1        RF215_BBCn_FSKPHRRX_SFD(1) /* SFD1 detected */
/* Bit 6 - FSKPHRRX.MS: Mode Switch */
#define RF215_BBCn_FSKPHRRX_MS           (1 << 6)
/* Bit 7 - FSKPHRRX.FCST: Frame Check Sequence Type */
#define RF215_BBCn_FSKPHRRX_FCST_Pos     7
#define RF215_BBCn_FSKPHRRX_FCST_Msk     (0x1 << RF215_BBCn_FSKPHRRX_FCST_Pos)
#define RF215_BBCn_FSKPHRRX_FCST(x)      (((x) << RF215_BBCn_FSKPHRRX_FCST_Pos) & RF215_BBCn_FSKPHRRX_FCST_Msk)
#define RF215_BBCn_FSKPHRRX_FCST_32      RF215_BBCn_FSKPHRRX_FCST(0) /* FCS type 32-bit */
#define RF215_BBCn_FSKPHRRX_FCST_16      RF215_BBCn_FSKPHRRX_FCST(1) /* FCS type 16-bit */

/** BBCn_FSKDM - FSK Direct Modulation */
/* Bit 0 - FSKDM.EN: FSK Direct Modulation Enable */
#define RF215_BBCn_FSKDM_EN              (1 << 0)
/* Bit 1 - FSKDM.PE: FSK Preemphasis */
#define RF215_BBCn_FSKDM_PE              (1 << 1)

/** BBCn_CNTC - Counter Configuration */
/* Bit 0 - CNTC.EN: Enable */
#define RF215_BBCn_CNTC_EN               (1 << 0)
/* Bit 1 - CNTC.RSTRXS: Reset at RX Start Event */
#define RF215_BBCn_CNTC_RSTRXS           (1 << 1)
/* Bit 2 - CNTC.RSTTXS: Reset at TX Start Event */
#define RF215_BBCn_CNTC_RSTTXS           (1 << 2)
/* Bit 3 - CNTC.CAPRXS: Capture of Counter Values at RX Start Event */
#define RF215_BBCn_CNTC_CAPRXS           (1 << 3)
/* Bit 4 - CNTC.CAPTXS: Capture of Counter Values at TX Start Event */
#define RF215_BBCn_CNTC_CAPTXS           (1 << 4)

/*** RF215 register reset values definition ***/
#define RF215_RFn_CS_Rst                 (0x08)
#define RF215_RFn_CCF0L_Rst              (0xF8)
#define RF215_RFn_CCF0H_Rst              (0x8C)
#define RF215_RFn_CNL_Rst                (0x00)
#define RF215_RFn_CNM_Rst                (RF215_RFn_CNM_CM_IEEE)
#define RF215_RFn_RXBWC_Rst              (RF215_RFn_RXBWC_BW2000_IF2000kHz)
#define RF215_RFn_RXDFE_Rst              (RF215_RFn_RXDFE_SR_4000kHz)
#define RF215_RFn_AGCC_Rst               (RF215_RFn_AGCC_EN | RF215_RFn_AGCC_RSV)
#define RF215_RFn_AGCS_Rst               (RF215_RFn_AGCS_GCW(23) | RF215_RFn_AGCS_TGT_30dB)
#define RF215_RFn_RSSI_Rst               (0x7F)
#define RF215_RFn_EDC_Rst                (RF215_RFn_EDC_EDM_AUTO)
#define RF215_RFn_EDD_Rst                (RF215_RFn_EDD_DTB_8us | RF215_RFn_EDD_DF(16))
#define RF215_RFn_EDV_Rst                (0x7F)
#define RF215_RFn_RNDV_Rst               (0x00)
#define RF215_RFn_TXCUTC_Rst             (RF215_RFn_TXCUTC_LPFCUT_500kHz)
#define RF215_RFn_TXDFE_Rst              (RF215_RFn_TXDFE_SR_4000kHz)
#define RF215_RFn_PAC_Rst                (RF215_RFn_PAC_TXPWR_MAX | RF215_RFn_PAC_PACUR_0mA)
#define RF215_BBCn_IRQM_Rst              (0x00)
#define RF215_BBCn_PC_Rst                (RF215_BBCn_PC_BBEN_ON | RF215_BBCn_PC_TXAFCS_EN | RF215_BBCn_PC_FCSFE_EN)
#define RF215_BBCn_TXFLL_Rst             (0)
#define RF215_BBCn_TXFLH_Rst             (0)
#define RF215_BBCn_FBLIL_Rst             (0x7F)
#define RF215_BBCn_FBLIH_Rst             (RF215_BBCn_FBLIH_FBLIH_Msk)
#define RF215_BBCn_OFDMPHRTX_Rst         (RF215_BBCn_OFDMPHRTX_MCS_0)
#define RF215_BBCn_OFDMPHRRX_Rst         (RF215_BBCn_OFDMPHRRX_SPC_DIS)
#define RF215_BBCn_OFDMC_Rst             (RF215_BBCn_OFDMC_OPT_1)
#define RF215_BBCn_OFDMSW_Rst            (RF215_BBCn_OFDMSW_RXO_12dB | RF215_BBCn_OFDMSW_PDT(3))
#define RF215_BBCn_AMCS_Rst              (0x00)
#define RF215_BBCn_AMEDT_Rst             (0xB5)
#define RF215_BBCn_FSKC0_Rst             (RF215_BBCn_FSKC0_MIDX_1_0 | RF215_BBCn_FSKC0_MIDXS_1_0 | RF215_BBCn_FSKC0_BT_2_0)
#define RF215_BBCn_FSKC1_Rst             (RF215_BBCn_FSKC1_SRATE_50kHz)
#define RF215_BBCn_FSKC2_Rst             (RF215_BBCn_FSKC2_FECIE_EN | RF215_BBCn_FSKC2_RXO_18dB)
#define RF215_BBCn_FSKC3_Rst             (RF215_BBCn_FSKC3_PDT(5) | RF215_BBCn_FSKC3_SFDT(8))
#define RF215_BBCn_FSKC4_Rst             (RF215_BBCn_FSKC4_CSFD0_U_IEEE | RF215_BBCn_FSKC4_CSFD1_C_IEEE | RF215_BBCn_FSKC4_RAWRBIT_MSB)
#define RF215_BBCn_FSKPLL_Rst            (0x08)
#define RF215_BBCn_FSKSFD0L_Rst          (0x09)
#define RF215_BBCn_FSKSFD0H_Rst          (0x72)
#define RF215_BBCn_FSKSFD1L_Rst          (0xF6)
#define RF215_BBCn_FSKSFD1H_Rst          (0x72)
#define RF215_BBCn_FSKPHRTX_Rst          (RF215_BBCn_FSKPHRTX_DW_EN)
#define RF215_BBCn_FSKPHRRX_Rst          (0x00)
#define RF215_BBCn_FSKRRXFLL_Rst         (0xFF)
#define RF215_BBCn_FSKRRXFLH_Rst         (0x07)
#define RF215_BBCn_FSKDM_Rst             (0x00)
#define RF215_BBCn_FSKPE0_Rst            (0x00)
#define RF215_BBCn_FSKPE1_Rst            (0x00)
#define RF215_BBCn_FSKPE2_Rst            (0x00)

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // #ifndef _DRV_RF215_LOCAL_H
