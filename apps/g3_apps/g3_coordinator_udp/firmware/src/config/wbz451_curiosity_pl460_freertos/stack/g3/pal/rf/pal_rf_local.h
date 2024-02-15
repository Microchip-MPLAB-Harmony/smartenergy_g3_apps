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
/*
Copyright (C) 2024, Microchip Technology Inc., and its subsidiaries. All rights reserved.

The software and documentation is provided by microchip and its contributors
"as is" and any express, implied or statutory warranties, including, but not
limited to, the implied warranties of merchantability, fitness for a particular
purpose and non-infringement of third party intellectual property rights are
disclaimed to the fullest extent permitted by law. In no event shall microchip
or its contributors be liable for any direct, indirect, incidental, special,
exemplary, or consequential damages (including, but not limited to, procurement
of substitute goods or services; loss of use, data, or profits; or business
interruption) however caused and on any theory of liability, whether in contract,
strict liability, or tort (including negligence or otherwise) arising in any way
out of the use of the software and documentation, even if advised of the
possibility of such damage.

Except as expressly permitted hereunder and subject to the applicable license terms
for any third-party software incorporated in the software and any applicable open
source software license terms, no license or other rights, whether express or
implied, are granted under any patent or other intellectual property rights of
Microchip or any third party.
*/
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