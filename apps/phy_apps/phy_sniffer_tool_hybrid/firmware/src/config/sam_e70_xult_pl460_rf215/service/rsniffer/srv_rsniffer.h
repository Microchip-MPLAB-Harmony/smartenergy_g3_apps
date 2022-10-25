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
// Section: RF PHY Sniffer Serialization Interface Routines
// *****************************************************************************
// *****************************************************************************

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
    DRV_HANDLE drvRf215Handle; // returned from DRV_RF215_Open
    SRV_USI_HANDLE srvUSIHandle; // returned from SRV_USI_Open

    static void _APP_RfRxIndCb(DRV_RF215_RX_INDICATION_OBJ* ind, uintptr_t ctxt)
    {
        DRV_RF215_PHY_CFG_OBJ rfPhyConfig;
        uint8_t* pRfSnifferData;
        size_t rfSnifferDataSize;
        uint16_t rfPayloadSymbols;

        // Get payload symbols in the received message
        DRV_RF215_GetPib(drvRf215Handle, RF215_PIB_PHY_RX_PAY_SYMBOLS,
                &rfPayloadSymbols);

        // Get RF PHY configuration
        DRV_RF215_GetPib(drvRf215Handle, RF215_PIB_PHY_CONFIG, &rfPhyConfig);

        // Serialize received RF message
        pRfSnifferData = SRV_RSNIFFER_SerialRxMessage(ind, &rfPhyConfig,
                rfPayloadSymbols, &rfSnifferDataSize);

        // Send through USI
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
    txHandle - TX handle returned from DRV_RF215_TxRequest

  Returns:
    None.

  Example:
    <code>
    DRV_HANDLE drvRf215Handle // returned from DRV_RF215_Open
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
    txReqObj.time = 0;
    txReqObj.psdu = psduTx;

    txReqHandle = DRV_RF215_TxRequest(drvRf215Handle, &txReqObj, &txReqResult);
    if (txReqHandle != DRV_RF215_TX_HANDLE_INVALID)
    {
        SRV_RSNIFFER_SetTxMessage(&txReqObj, txReqHandle);
    }
    </code>

  Remarks:
    None.
*/

void SRV_RSNIFFER_SetTxMessage (
    DRV_RF215_TX_REQUEST_OBJ* pReqObj,
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
    DRV_HANDLE drvRf215Handle; // returned from DRV_RF215_Open
    SRV_USI_HANDLE srvUSIHandle; // returned from SRV_USI_Open

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

        // Get payload symbols in the received message
        DRV_RF215_GetPib(drvRf215Handle, RF215_PIB_PHY_TX_PAY_SYMBOLS,
                &rfPayloadSymbols);

        // Get RF PHY configuration
        DRV_RF215_GetPib(drvRf215Handle, RF215_PIB_PHY_CONFIG, &rfPhyConfig);

        // Serialize received RF message
        pRfSnifferData = SRV_RSNIFFER_SerialCfmMessage(cfmObj, txHandle,
                &rfPhyConfig, rfPayloadSymbols, &rfSnifferDataSize);

        // Send through USI
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

#endif //SRV_RSNIFFER_H
