/*******************************************************************************
  RF PHY Sniffer Serialization Header File

  Company:
    Microchip Technology Inc.

  File Name:
    srv_rsniffer.h

  Summary:
    RF PHY sniffer serialization interface header file.

  Description:
    The RF PHY sniffer serialization provides a service to format messages
    through serial connection in order to communicate with Hybrid Sniffer Tool
    provided by Microchip. This file provides the interface definition for the
    RF PHY sniffer serialization.
*******************************************************************************/

//DOM-IGNORE-BEGIN
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
//DOM-IGNORE-END

#ifndef SRV_RSNIFFER_H    // Guards against multiple inclusion
#define SRV_RSNIFFER_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include "driver/rf215/drv_rf215.h"

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

/* Hybrid PHY Sniffer Tool command

  Summary:
    Hybrid Sniffer Tool Commands enumeration.

  Description:
    This enumeration defines the commands used by the Hybrid PHY Sniffer Tool
    provided by Microchip.
*/
typedef enum
{
  /* Receive new PLC message */
  SRV_RSNIFFER_CMD_RECEIVE_MSG = 0,
  /* Set PLC Tone Mask */
  SRV_RSNIFFER_CMD_SET_PLC_TONE_MASK,
  /* Set RF Band, Operating Mode and Channel */
  SRV_RSNIFFER_CMD_SET_RF_BAND_OPM_CHANNEL
} SRV_RSNIFFER_COMMAND;

// *****************************************************************************
// *****************************************************************************
// Section: RF PHY Sniffer Serialization Interface Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    SRV_RSNIFFER_COMMAND SRV_RSNIFFER_GetCommand(uint8_t* pDataSrc)

  Summary:
    Extracts Command field from Sniffer frame.

  Description:
    Takes Sniffer frame as parameter and extracts the Command field from the
    expected position in buffer.

  Precondition:
    None.

  Parameters:
    pDataSrc - Pointer to buffer containing Sniffer frame

  Returns:
    Command in the form of SRV_RSNIFFER_COMMAND Enum.

  Example:
    <code>
    SRV_RSNIFFER_COMMAND command;

    command = SRV_RSNIFFER_GetCommand(pData);
    </code>

  Remarks:
    None.
*/
SRV_RSNIFFER_COMMAND SRV_RSNIFFER_GetCommand(uint8_t* pDataSrc);

// *****************************************************************************
/* Function:
    uint8_t* SRV_RSNIFFER_SerialRxMessage (
        DRV_RF215_RX_INDICATION_OBJ* pIndObj,
        DRV_RF215_PHY_CFG_OBJ* pPhyCfgObj,
        uint16_t paySymbols,
        size_t* pMsgLen
    )

  Summary:
    Serializes a received RF frame along with its parameters.

  Description:
    This function takes an object containing a RF frame and its related
    parameters and serializes it in a buffer for further transmission through
    serial interface.

  Precondition:
    None.

  Parameters:
    pIndObj    - Pointer to RF reception object containing the frame and
                 parameters
    pPhyCfgObj - Pointer to RF PHY configuration object
    paySymbols - Number of payload symbols in the received frame
    pMsgLen    - Pointer to sniffer message length in bytes (output)

  Returns:
    Pointer to sniffer message to be sent through serial interface.

  Example:
    <code>
    DRV_HANDLE drvRf215Handle;
    SRV_USI_HANDLE srvUSIHandle;

    static void _APP_RfRxIndCb(DRV_RF215_RX_INDICATION_OBJ* ind, uintptr_t ctxt)
    {
        DRV_RF215_PHY_CFG_OBJ rfPhyConfig;
        uint8_t* pRfSnifferData;
        size_t rfSnifferDataSize;
        uint16_t rfPayloadSymbols;

        DRV_RF215_GetPib(drvRf215Handle, RF215_PIB_PHY_RX_PAY_SYMBOLS,
                &rfPayloadSymbols);

        DRV_RF215_GetPib(drvRf215Handle, RF215_PIB_PHY_CONFIG, &rfPhyConfig);

        pRfSnifferData = SRV_RSNIFFER_SerialRxMessage(ind, &rfPhyConfig,
                rfPayloadSymbols, &rfSnifferDataSize);

        SRV_USI_Send_Message(srvUSIHandle, SRV_USI_PROT_ID_SNIF_G3,
                pRfSnifferData, rfSnifferDataSize);
    }
    </code>

  Remarks:
    None.
*/

uint8_t* SRV_RSNIFFER_SerialRxMessage (
    DRV_RF215_RX_INDICATION_OBJ* pIndObj,
    DRV_RF215_PHY_CFG_OBJ* pPhyCfgObj,
    uint16_t paySymbols,
    size_t* pMsgLen
);

// *****************************************************************************
/* Function:
    void SRV_RSNIFFER_SetTxMessage (
        DRV_RF215_TX_REQUEST_OBJ* pReqObj,
        DRV_RF215_PHY_CFG_OBJ* pPhyCfgObj,
        DRV_RF215_TX_HANDLE txHandle
    )

  Summary:
    Gives a transmitted RF TX request object to sniffer library so it is stored
    for later serialization.

  Description:
    The given RF TX request contains a RF frame and its related parameters. This
    information is stored in sniffer library for later serialization when
    SRV_RSNIFFER_SerialCfmMessage is called.

  Precondition:
    None.

  Parameters:
    pReqObj  - Pointer to the RF TX request object
    pPhyCfgObj - Pointer to RF PHY configuration object
    txHandle - TX handle returned from DRV_RF215_TxRequest

  Returns:
    None.

  Example:
    <code>
    DRV_HANDLE drvRf215Handle;
    DRV_RF215_PHY_CFG_OBJ rfPhyConfig;
    DRV_RF215_TX_REQUEST_OBJ txReqObj;
    DRV_RF215_TX_RESULT txReqResult;
    DRV_RF215_TX_HANDLE txReqHandle;
    uint8_t psduTx[DRV_RF215_MAX_PSDU_LEN];

    txReqObj.cancelByRx = false;
    txReqObj.ccaMode = PHY_CCA_MODE_3;
    txReqObj.modScheme = FSK_FEC_OFF;
    txReqObj.txPwrAtt = 0;
    txReqObj.psduLen = DRV_RF215_MAX_PSDU_LEN;
    txReqObj.timeMode = TX_TIME_RELATIVE;
    txReqObj.timeCount = 0;
    txReqObj.psdu = psduTx;

    txReqHandle = DRV_RF215_TxRequest(drvRf215Handle, &txReqObj, &txReqResult);
    if (txReqHandle != DRV_RF215_TX_HANDLE_INVALID)
    {
        SRV_RSNIFFER_SetTxMessage(&txReqObj, &rfPhyConfig, txReqHandle);
    }
    </code>

  Remarks:
    None.
*/

void SRV_RSNIFFER_SetTxMessage (
    DRV_RF215_TX_REQUEST_OBJ* pReqObj,
    DRV_RF215_PHY_CFG_OBJ* pPhyCfgObj,
    DRV_RF215_TX_HANDLE txHandle
);

// *****************************************************************************
/* Function:
    uint8_t* SRV_RSNIFFER_SerialCfmMessage (
        DRV_RF215_TX_CONFIRM_OBJ* pCfmObj,
        DRV_RF215_TX_HANDLE txHandle,
        DRV_RF215_PHY_CFG_OBJ* pPhyCfgObj,
        uint16_t paySymbols,
        size_t* pMsgLen
    )

  Summary:
    Serializes a transmitted RF frame along with its parameters.

  Description:
    This function takes a previously stored RF transmitted frame and its
    related parameters and serializes it in a buffer for further transmission
    through serial interface.

  Precondition:
    SRV_RSNIFFER_SetTxMessage has to be previously called to store the RF
    transmitted frame and its parameters.

  Parameters:
    pCfmObj    - Pointer to RF TX confirm object
    txHandle   - TX handle given in TX confirm callback
    pPhyCfgObj - Pointer to RF PHY configuration object
    paySymbols - Number of payload symbols in the transmitted frame
    pMsgLen    - Pointer to sniffer message length in bytes (output)

  Returns:
    Pointer to sniffer message to be sent through serial interface.

  Example:
    <code>
    DRV_HANDLE drvRf215Handle;
    SRV_USI_HANDLE srvUSIHandle;

    static void _APP_RF_TxCfmCb (
        DRV_RF215_TX_HANDLE txHandle,
        DRV_RF215_TX_CONFIRM_OBJ *cfmObj,
        uintptr_t ctxt
    )
    {
        DRV_RF215_PHY_CFG_OBJ rfPhyConfig;
        uint8_t* pRfSnifferData;
        size_t rfSnifferDataSize;
        uint16_t rfPayloadSymbols;

        DRV_RF215_GetPib(drvRf215Handle, RF215_PIB_PHY_TX_PAY_SYMBOLS,
                &rfPayloadSymbols);

        DRV_RF215_GetPib(drvRf215Handle, RF215_PIB_PHY_CONFIG, &rfPhyConfig);

        pRfSnifferData = SRV_RSNIFFER_SerialCfmMessage(cfmObj, txHandle,
                &rfPhyConfig, rfPayloadSymbols, &rfSnifferDataSize);

        SRV_USI_Send_Message(srvUSIHandle, SRV_USI_PROT_ID_SNIF_G3,
                pRfSnifferData, rfSnifferDataSize);
    }
    </code>

  Remarks:
    None.
*/

uint8_t* SRV_RSNIFFER_SerialCfmMessage (
    DRV_RF215_TX_CONFIRM_OBJ* pCfmObj,
    DRV_RF215_TX_HANDLE txHandle,
    DRV_RF215_PHY_CFG_OBJ* pPhyCfgObj,
    uint16_t paySymbols,
    size_t* pMsgLen
);

// *****************************************************************************
/* Function:
    void SRV_RSNIFFER_ParseConfigCommand (
        uint8_t* pDataSrc,
        uint16_t* pBandOpMode,
        uint16_t* pChannel
    )

  Summary:
    Parses a RF configuration command coming from the Microchip Hybrid Sniffer
    Tool.

  Description:
    This function takes a RF configuration command with the format coming from
    the Microchip Hybrid Sniffer Tool and extracts the Band, Operating Mode
    and Channel parameters to be configured in the RF PHY layer.
    If misconfigured, no RF frames will be seen.

  Precondition:
    None.

  Parameters:
    pDataSrc    - Pointer to the data where the command is stored (USI)
    pBandOpMode - Pointer to store the parsed Band and Operating Mode parameter
                  (RF215_PIB_PHY_BAND_OPERATING_MODE)
    pChannel    - Pointer to store the parsed Channel parameter
                  (RF215_PIB_PHY_CHANNEL_NUM)

  Returns:
    None.

  Example:
    <code>
    switch (command) {
        case SRV_RSNIFFER_CMD_SET_RF_BAND_OPM_CHANNEL:
        {
            uint16_t rfBandOpMode, rfChannel;
            SRV_RSNIFFER_ParseConfigCommand(pData, &rfBandOpMode, &rfChannel);

            DRV_RF215_SetPib(appData.drvRf215Handle, RF215_PIB_PHY_BAND_OPERATING_MODE, &rfBandOpMode);
            DRV_RF215_SetPib(appData.drvRf215Handle, RF215_PIB_PHY_CHANNEL_NUM, &rfChannel);
            break;
        }
    }
    </code>

  Remarks:
    None.
*/
void SRV_RSNIFFER_ParseConfigCommand (
    uint8_t* pDataSrc,
    uint16_t* pBandOpMode,
    uint16_t* pChannel
);

#endif //SRV_RSNIFFER_H
