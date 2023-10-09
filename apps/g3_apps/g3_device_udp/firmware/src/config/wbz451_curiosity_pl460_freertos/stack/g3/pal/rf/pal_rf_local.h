/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    pal_rf_local.h

  Summary:
    RF Platform Abstraction Layer (PAL) Interface Local header file.

  Description:
    RF Platform Abstraction Layer (PAL) Interface Local header file.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
 * Copyright (C) 2023 Microchip Technology Inc. and its subsidiaries.
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

#ifndef PAL_RF_LOCAL_H
#define PAL_RF_LOCAL_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include "system/system.h"
#include "driver/driver.h"
#include "stack_config.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************

/* PAL RF Network Parameters

  Summary:
    Holds PAL RF internal data.

  Description:
    This data type defines the all data required to handle the PAL RF module.

  Remarks:
    None.
 */

typedef struct
{
    uint8_t channel;
    uint8_t channelPage;
    uint8_t ccaMode;
    PHY_CSMAMode_t csmaMode;
    bool promiscuousMode;
    uint8_t txPower;
    uint8_t pdtLevel;

} PAL_RF_NETWORK_PARAMS;

// *****************************************************************************

/* PAL RF statistics

  Summary:
    Holds PAL RF statistics data.

  Description:
    This data type holds statistics information regarding transmitted/received
    packets.

  Remarks:
    None.
 */

typedef struct
{
    uint16_t txLastPaySymbols;
    uint16_t rxLastPaySymbols;
    uint32_t txTotalPackets;
    uint32_t txTotalTxRequest;
    uint32_t txTotalCfmPackets;
    uint32_t txTotalBytes;
    uint32_t txTotalErrors;
    uint32_t txErrorBusyTX;
    uint32_t txErrorBusyRX;
    uint32_t txErrorBusyChannel;
    uint32_t txErrorLength;
    uint32_t txErrorFormat;
    uint32_t txErrorTimeout;
    uint32_t txErrorAborted;
    uint32_t txErrorTxError;
    uint32_t txErrorTrxOff;
    uint32_t txErrorPhyErrors;
    uint32_t txCfmNotHandled;
    uint32_t rxTotalPackets;
    uint32_t rxTotalBytes;
    uint32_t rxTotalErrors;
    uint32_t rxErrorFalsePositive;
    uint32_t rxErrorLength;
    uint32_t rxErrorFormat;
    uint32_t rxErrorFCS;
    uint32_t rxErrorAborted;
    uint32_t rxErrorOverride;
    uint32_t rxErrorRcvDataNotHandled;
    uint32_t phyResets;
    uint32_t phyResetsInTx;
} PAL_RF_STATISTICS;

// *****************************************************************************

/* PAL RF Data

  Summary:
    Holds PAL RF internal data.

  Description:
    This data type defines the all data required to handle the PAL RF module.

  Remarks:
    None.
 */

typedef struct
{
    DRV_HANDLE drvRfPhyHandle;

    PAL_RF_HANDLERS rfPhyHandlers;

    PAL_RF_STATUS status;
    PHY_TrxStatus_t phyStatus;

    PAL_RF_NETWORK_PARAMS network;

    PHY_FrameInfo_t txFrame;
    PHY_CSMAMode_t csmaMode;

    SYS_TIME_HANDLE txTimer;
    uint64_t txTimeIniCount;
    uint64_t txTimeEndCount;

    bool txDelayedPending;
    bool txTransmitting;
    PAL_RF_PHY_STATUS txCfmStatus;

    uint8_t rxBuffer[LARGE_BUFFER_SIZE];
    uint8_t txBuffer[LARGE_BUFFER_SIZE];

    PAL_RF_STATISTICS stats;

    uint32_t phySWVersion;
    uint8_t lastRxPktLQI;
    int8_t lastRxPktED;
    bool txContinuousMode;
    bool sleepMode;

} PAL_RF_DATA;

#endif // #ifndef PAL_RF_LOCAL_H

/*******************************************************************************
 End of File
 */