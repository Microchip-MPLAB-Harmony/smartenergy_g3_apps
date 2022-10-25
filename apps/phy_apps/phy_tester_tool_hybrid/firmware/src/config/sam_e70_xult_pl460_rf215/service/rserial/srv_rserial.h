/*******************************************************************************
  RF PHY Layer Serialization Header File

  Company:
    Microchip Technology Inc.

  File Name:
    srv_rserial.h

  Summary:
    RF PHY layer serialization interface header file.

  Description:
    The RF PHY layer serialization provides a service to format messages
    through serial connection in order to communicate with PHY Tester Python
    scripts provided by Microchip. This file provides the interface definition
    for the RF PHY layer serialization.
*******************************************************************************/

// DOM-IGNORE-BEGIN
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
// DOM-IGNORE-END

#ifndef SRV_RSERIAL_H    // Guards against multiple inclusion
#define SRV_RSERIAL_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

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

/* RF PHY Tester Tool Command

  Summary:
    RF PHY Tester Tool commands enumeration

  Description:
    This enumeration defines the commands used by RF PHY Tester Tool provided by
    Microchip.
*/
typedef enum
{
    /* Get data configuration request */
    SRV_RSERIAL_CMD_PHY_GET_CFG = 0,
    /* Get data configuration response */
    SRV_RSERIAL_CMD_PHY_GET_CFG_RSP,
    /* Set data configuration request */
    SRV_RSERIAL_CMD_PHY_SET_CFG,
    /* Set data configuration response */
    SRV_RSERIAL_CMD_PHY_SET_CFG_RSP,
    /* Get command request */
    SRV_RSERIAL_CMD_PHY_CMD_CFG,
    /* Get command response */
    SRV_RSERIAL_CMD_PHY_CMD_CFG_RSP,
    /* Send message data */
    SRV_RSERIAL_CMD_PHY_SEND_MSG,
    /* Send message data response */
    SRV_RSERIAL_CMD_PHY_SEND_MSG_RSP,
    /* Receive message data */
    SRV_RSERIAL_CMD_PHY_RECEIVE_MSG

} SRV_RSERIAL_COMMAND;

// *****************************************************************************
// *****************************************************************************
// Section: RF PHY Layer Serialization Interface Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    SRV_RSERIAL_COMMAND SRV_RSERIAL_GetCommand
    (
      uint8_t* pData
    )

  Summary:
    Extracts command field from serial frame.

  Description:
    Takes serial frame as parameter and extracts the command field from the
    expected position in buffer.

  Precondition:
    None.

  Parameters:
    pData - Pointer to buffer containing serial frame

  Returns:
    Command in the form of SRV_RSERIAL_COMMAND Enum.

  Example:
    <code>
    SRV_RSERIAL_COMMAND command;

    // Process received message from USI
    command = SRV_RSERIAL_GetCommand(pData);
    </code>

  Remarks:
    None.
*/

SRV_RSERIAL_COMMAND SRV_RSERIAL_GetCommand(uint8_t* pData);

// *****************************************************************************
/* Function:
    uint8_t* SRV_RSERIAL_ParsePIB (
        uint8_t* pDataSrc,
        DRV_RF215_TRX_ID* pTrxId,
        DRV_RF215_PIB_ATTRIBUTE* pPibAttr,
        uint8_t* pPibSize
    )

  Summary:
    Extracts PIB information from get/set PIB serial frame.

  Description:
    Takes a SRV_RSERIAL_CMD_PHY_GET_CFG or SRV_RSERIAL_CMD_PHY_SET_CFG serial
    frame as parameter and extracts the PIB information from the expected
    position in buffer.

  Precondition:
    None.

  Parameters:
    pDataSrc - Pointer to buffer containing serial frame
    pTrxId   - Pointer to TRX identifier (output)
    pPibAttr - Pointer to PIB attribute (output)
    pPibSize - Pointer to PIB size in bytes (output)

  Returns:
    Pointer to PIB data value to set (only for set PIB serial frame).

  Example:
    <code>
    SRV_RSERIAL_COMMAND command;
    DRV_RF215_TRX_ID trxId;
    DRV_RF215_PIB_ATTRIBUTE pibAttr;
    uint8_t pibSize;

    // Process received message from USI
    command = SRV_RSERIAL_GetCommand(pData);

    if (command == SRV_RSERIAL_CMD_PHY_GET_CFG)
    {
        SRV_RSERIAL_ParsePIB(pData, &trxId, &pibAttr, &pibSize);
        // Get PIB from RF215 Driver
    }
    </code>

  Remarks:
    None.
*/

uint8_t* SRV_RSERIAL_ParsePIB (
    uint8_t* pDataSrc,
    DRV_RF215_TRX_ID* pTrxId,
    DRV_RF215_PIB_ATTRIBUTE* pPibAttr,
    uint8_t* pPibSize
);

// *****************************************************************************
/* Function:
    uint8_t* SRV_RSERIAL_SerialGetPIB (
        DRV_RF215_TRX_ID trxId,
        DRV_RF215_PIB_ATTRIBUTE pibAttr,
        uint8_t pibSize,
        DRV_RF215_PIB_RESULT pibResult,
        uint8_t* pPibData,
        size_t* pMsgLen
    )

  Summary:
    Serializes a response to a get PIB command.

  Description:
    Takes the result of getting a PIB from RF215 driver as parameters and builds
    a serialized frame as response to the get PIB command.

  Precondition:
    None.

  Parameters:
    trxId     - TRX identifier (Sub-1GHz, 2.4GHz)
    pibAttr   - PIB attribute
    pibSize   - PIB size in bytes
    pibResult - PIB get result
    pPibData  - Pointer to PIB data get from RF215 driver
    pMsgLen   - Pointer to sniffer message length in bytes (output)

  Returns:
    Pointer to sniffer message to be sent through serial interface.

  Example:
    <code>
    uint8_t* pSerialData;
    size_t length;
    SRV_USI_HANDLE srvUSIHandle; // returned from SRV_USI_Open
    DRV_HANDLE rf215HandleRF09, rf215HandleRF24; // returned from DRV_RF215_Open
    DRV_HANDLE rf215Handle;
    SRV_RSERIAL_COMMAND command;
    DRV_RF215_TRX_ID trxId;
    DRV_RF215_PIB_ATTRIBUTE pibAttr;
    DRV_RF215_PIB_RESULT pibResult;
    uint8_t pibSize;
    uint8_t rfDataPIBBuffer[sizeof(DRV_RF215_PHY_CFG_OBJ)];

    // Process received message from USI
    command = SRV_RSERIAL_GetCommand(pData);

    if (command == SRV_RSERIAL_CMD_PHY_GET_CFG)
    {
        SRV_RSERIAL_ParsePIB(pData, &trxId, &pibAttr, &pibSize);

        if (trxId == RF215_TRX_ID_RF09)
        {
            rf215Handle = rf215HandleRF09;
        }
        else
        {
            rf215Handle = rf215HandleRF24;
        }

        // Get PIB from RF215 Driver
        pibResult = DRV_RF215_GetPib(rf215Handle, pibAttr, rfDataPIBBuffer);
        pibSize = DRV_RF215_GetPibSize(pibAttr);

        // Serialize PIB get response and send through USI
        pSerialData = SRV_RSERIAL_SerialGetPIB(trxId, pibAttr, pibSize,
                pibResult, rfDataPIBBuffer, &length);
        SRV_USI_Send_Message(srvUSIHandle, SRV_USI_PROT_ID_PHY_RF215,
                pSerialData, length);
    }
    </code>

  Remarks:
    None.
*/

uint8_t* SRV_RSERIAL_SerialGetPIB (
    DRV_RF215_TRX_ID trxId,
    DRV_RF215_PIB_ATTRIBUTE pibAttr,
    uint8_t pibSize,
    DRV_RF215_PIB_RESULT pibResult,
    uint8_t* pPibData,
    size_t* pMsgLen
);

// *****************************************************************************
/* Function:
    uint8_t* SRV_RSERIAL_SerialSetPIB (
        DRV_RF215_TRX_ID trxId,
        DRV_RF215_PIB_ATTRIBUTE pibAttr,
        uint8_t pibSize,
        DRV_RF215_PIB_RESULT pibResult,
        size_t* pMsgLen
    )

  Summary:
    Serializes a response to a set PIB command.

  Description:
    Takes the result of setting a PIB in RF215 driver as parameters and builds a
    serialized frame as response to the set PIB command.

  Precondition:
    None.

  Parameters:
    trxId     - TRX identifier (Sub-1GHz, 2.4GHz)
    pibAttr   - PIB attribute
    pibSize   - PIB size in bytes
    pibResult - PIB set result
    pMsgLen   - Pointer to sniffer message length in bytes (output)

  Returns:
    Pointer to sniffer message to be sent through serial interface.

  Example:
    <code>
    uint8_t* pSerialData;
    size_t length;
    SRV_USI_HANDLE srvUSIHandle; // returned from SRV_USI_Open
    DRV_HANDLE rf215HandleRF09, rf215HandleRF24; // returned from DRV_RF215_Open
    DRV_HANDLE rf215Handle;
    SRV_RSERIAL_COMMAND command;
    DRV_RF215_TRX_ID trxId;
    DRV_RF215_PIB_ATTRIBUTE pibAttr;
    DRV_RF215_PIB_RESULT pibResult;
    uint8_t pibSize;

    // Process received message from USI
    command = SRV_RSERIAL_GetCommand(pData);

    if (command == SRV_RSERIAL_CMD_PHY_SET_CFG)
    {
        uint8_t *pPibValue;

        pPibValue = SRV_RSERIAL_ParsePIB(pData, &trxId, &pibAttr, &pibSize);

        if (trxId == RF215_TRX_ID_RF09)
        {
            rf215Handle = rf215HandleRF09;
        }
        else
        {
            rf215Handle = rf215HandleRF24;
        }

        // Set PIB to RF215 driver
        pibResult = DRV_RF215_SetPib(rf215Handle, pibAttr, pPibValue);
        pibSize = DRV_RF215_GetPibSize(pibAttr);

        // Serialize PIB set response and send through USI
        pSerialData = SRV_RSERIAL_SerialSetPIB(trxId, pibAttr, pibSize,
                pibResult, &length);
        SRV_USI_Send_Message(srvUSIHandle, SRV_USI_PROT_ID_PHY_RF215,
                pSerialData, length);
    }
    </code>

  Remarks:
    None.
*/

uint8_t* SRV_RSERIAL_SerialSetPIB (
    DRV_RF215_TRX_ID trxId,
    DRV_RF215_PIB_ATTRIBUTE pibAttr,
    uint8_t pibSize,
    DRV_RF215_PIB_RESULT pibResult,
    size_t* pMsgLen
);

// *****************************************************************************
/* Function:
    DRV_RF215_TRX_ID SRV_RSERIAL_ParseTxMessageTrxId(uint8_t* pDataSrc)

  Summary:
    Extracts the TRX identifier from SRV_RSERIAL_CMD_PHY_SEND_MSG
    serial frame.

  Description:
    Takes a SRV_RSERIAL_CMD_PHY_SEND_MSG serial frame as parameter and extracts
    the TRX identifier (Sub-1GHz, 2.4GHz).

  Precondition:
    None.

  Parameters:
    pDataSrc - Pointer to buffer containing serial frame

  Returns:
    TRX identifier (Sub-1GHz, 2.4GHz).

  Example:
    <code>
    SRV_RSERIAL_COMMAND command;
    DRV_RF215_TRX_ID trxId;

    // Process received message from USI
    command = SRV_RSERIAL_GetCommand(pData);

    if (command == SRV_RSERIAL_CMD_PHY_SEND_MSG)
    {
        // Parse TRX identifier from USI
        trxId = SRV_RSERIAL_ParseTxMessageTrxId(pData);
    }
    </code>

  Remarks:
    None.
*/

DRV_RF215_TRX_ID SRV_RSERIAL_ParseTxMessageTrxId(uint8_t* pDataSrc);

// *****************************************************************************
/* Function:
    bool SRV_RSERIAL_ParseTxMessage (
        uint8_t* pDataSrc,
        DRV_RF215_PHY_CFG_OBJ* pPhyCfgObj,
        DRV_RF215_TX_REQUEST_OBJ* pDataDst,
        DRV_RF215_TX_HANDLE* pTxHandleCancel
    )

  Summary:
    Extracts a RF frame to be transmitted from SRV_RSERIAL_CMD_PHY_SEND_MSG
    serial frame.

  Description:
    Takes a SRV_RSERIAL_CMD_PHY_SEND_MSG serial frame as parameter, extracts the
    RF frame and its related transmission information and fills a
    DRV_RF215_TX_REQUEST_OBJ object.

  Precondition:
    None.

  Parameters:
    pDataSrc        - Pointer to buffer containing serial frame
    pPhyCfgObj      - Pointer to RF PHY configuration object
    pDataDst        - Pointer to a DRV_RF215_TX_REQUEST_OBJ object to fill
                      (output)
    pTxHandleCancel - Pointer to TX handle to cancel (output)

  Returns:
    Boolean indicating if it is a TX cancel request.

  Example:
    <code>
    SRV_RSERIAL_COMMAND command;
    DRV_RF215_TRX_ID trxId;
    DRV_HANDLE rf215HandleRF09, rf215HandleRF24; // returned from DRV_RF215_Open
    DRV_HANDLE rf215Handle;
    DRV_RF215_PHY_CFG_OBJ phyCfg;
    DRV_RF215_TX_REQUEST_OBJ txReq;
    DRV_RF215_TX_HANDLE txHandle;
    bool txCancel;

    // Process received message from USI
    command = SRV_RSERIAL_GetCommand(pData);

    if (command == SRV_RSERIAL_CMD_PHY_SEND_MSG)
    {
        // Parse TRX identifier from USI
        trxId = SRV_RSERIAL_ParseTxMessageTrxId(pData);

        if (trxId == RF215_TRX_ID_RF09)
        {
            rf215Handle = rf215HandleRF09;
        }
        else
        {
            rf215Handle = rf215HandleRF24;
        }

        // Get RF PHY configuration
        DRV_RF215_GetPib(rf215Handle, RF215_PIB_PHY_CONFIG, &phyCfg);

        // Parse TX request data from USI
        txCancel = SRV_RSERIAL_ParseTxMessage(pData, &phyCfg, &txReq,
                &txHandle);

        if (txCancel == false)
        {
            DRV_RF215_TX_RESULT txResult;

            // Send Message through RF
            txHandle = DRV_RF215_TxRequest(rf215Handle, &txReq, &txResult);
            SRV_RSERIAL_SetTxHandle(txHandle);
        }
        else
        {
            // Cancel TX request
            DRV_RF215_TxCancel(rf215Handle, txHandle);
        }
    }
    </code>

  Remarks:
    None.
*/

bool SRV_RSERIAL_ParseTxMessage (
    uint8_t* pDataSrc,
    DRV_RF215_PHY_CFG_OBJ* pPhyCfgObj,
    DRV_RF215_TX_REQUEST_OBJ* pDataDst,
    DRV_RF215_TX_HANDLE* pTxHandleCancel
);

// *****************************************************************************
/* Function:
    void SRV_RSERIAL_SetTxHandle(DRV_RF215_TX_HANDLE txHandle)

  Summary:
    Sets the TX handle for the last TX request.

  Description:
    Sets the TX handle returned from DRV_RF215_TxRequest for the last TX
    request.

  Precondition:
    DRV_RF215_TxRequest must have been called to obtain a valid TX handle.

  Parameters:
    txHandle - TX handle returned from DRV_RF215_TxRequest.

  Returns:
    None.

  Example:
    <code>
    SRV_RSERIAL_COMMAND command;
    DRV_RF215_TRX_ID trxId;
    DRV_HANDLE rf215HandleRF09, rf215HandleRF24; // returned from DRV_RF215_Open
    DRV_HANDLE rf215Handle;
    DRV_RF215_PHY_CFG_OBJ phyCfg;
    DRV_RF215_TX_REQUEST_OBJ txReq;
    DRV_RF215_TX_HANDLE txHandle;
    bool txCancel;

    // Process received message from USI
    command = SRV_RSERIAL_GetCommand(pData);

    if (command == SRV_RSERIAL_CMD_PHY_SEND_MSG)
    {
        // Parse TRX identifier from USI
        trxId = SRV_RSERIAL_ParseTxMessageTrxId(pData);

        if (trxId == RF215_TRX_ID_RF09)
        {
            rf215Handle = rf215HandleRF09;
        }
        else
        {
            rf215Handle = rf215HandleRF24;
        }

        // Get RF PHY configuration
        DRV_RF215_GetPib(rf215Handle, RF215_PIB_PHY_CONFIG, &phyCfg);

        // Parse TX request data from USI
        txCancel = SRV_RSERIAL_ParseTxMessage(pData, &phyCfg, &txReq,
                &txHandle);

        if (txCancel == false)
        {
            DRV_RF215_TX_RESULT txResult;

            // Send Message through RF
            txHandle = DRV_RF215_TxRequest(rf215Handle, &txReq, &txResult);
            SRV_RSERIAL_SetTxHandle(txHandle);
        }
        else
        {
            // Cancel TX request
            DRV_RF215_TxCancel(rf215Handle, txHandle);
        }
    }
    </code>

  Remarks:
    None.
*/

void SRV_RSERIAL_SetTxHandle(DRV_RF215_TX_HANDLE txHandle);

// *****************************************************************************
/* Function:
    uint8_t* SRV_RSERIAL_SerialRxMessage (
        DRV_RF215_RX_INDICATION_OBJ* pIndObj,
        DRV_RF215_TRX_ID trxId,
        DRV_RF215_PHY_CFG_OBJ* pPhyCfgObj,
        size_t* pMsgLen
    )

  Summary:
    Serializes a received RF frame and its related information.

  Description:
    Takes a DRV_RF215_RX_INDICATION_OBJ object as parameter and builds a
    serialized frame containing the RF frame and its related reception
    parameters.

  Precondition:
    None.

  Parameters:
    pIndObj    - Pointer to RF Reception object containing the frame and
                 parameters
    trxId      - TRX identifier (Sub-1GHz, 2.4GHz)
    pPhyCfgObj - Pointer to RF PHY configuration object
    pMsgLen    - Pointer to sniffer message length in bytes (output)

  Returns:
    Pointer to sniffer message to be sent through serial interface.

  Example:
    <code>
    DRV_HANDLE rf215Handle; // Returned from DRV_RF215_Open
    SRV_USI_HANDLE srvUSIHandle; // returned from SRV_USI_Open

    void _APP_RF_RxIndCb(DRV_RF215_RX_INDICATION_OBJ* indObj, uintptr_t ctxt)
    {
        DRV_RF215_PHY_CFG_OBJ phyCfg;
        uint8_t* pSerialData;
        size_t length;

        // Get RF PHY configuration
        DRV_RF215_GetPib(rf215Handle, RF215_PIB_PHY_CONFIG, &phyCfg);

        // Serialize received message and send through USI
        pSerialData = SRV_RSERIAL_SerialRxMessage(indObj, RF215_TRX_ID_RF09,
                &phyCfg, &length);
        SRV_USI_Send_Message(srvUSIHandle, SRV_USI_PROT_ID_PHY_RF215,
                pSerialData, length);
    }
    </code>

  Remarks:
    None.
*/

uint8_t* SRV_RSERIAL_SerialRxMessage (
    DRV_RF215_RX_INDICATION_OBJ* pIndObj,
    DRV_RF215_TRX_ID trxId,
    DRV_RF215_PHY_CFG_OBJ* pPhyCfgObj,
    size_t* pMsgLen
);

// *****************************************************************************
/* Function:
    uint8_t* SRV_RSERIAL_SerialCfmMessage (
        DRV_RF215_TX_CONFIRM_OBJ* pCfmObj,
        DRV_RF215_TRX_ID trxId,
        DRV_RF215_TX_HANDLE txHandle,
        size_t* pMsgLen
    )

  Summary:
    Serializes the result of a RF transmitted frame.

  Description:
    Takes a DRV_RF215_TX_CONFIRM_OBJ object as parameter, and builds a
    serialized frame containing the RF transmission result and parameters.

  Precondition:
    DRV_RF215_TxRequest and SRV_RSERIAL_SetTxHandle must have been called
    before.

  Parameters:
    pCfmObj  - Pointer to RF transmit confirm object containing the transmission
               result
    trxId    - TRX identifier (Sub-1GHz, 2.4GHz)
    txHandle - Transmission handle corresponding to transmit confirm
    pMsgLen  - Pointer to sniffer message length in bytes (output)

  Returns:
    Pointer to sniffer message to be sent through serial interface.

  Example:
    <code>
    DRV_HANDLE rf215Handle; // Returned from DRV_RF215_Open
    SRV_USI_HANDLE srvUSIHandle; // returned from SRV_USI_Open

    void _APP_RF_TxCfmCb (
        DRV_RF215_TX_HANDLE txHandle,
        DRV_RF215_TX_CONFIRM_OBJ *cfmObj,
        uintptr_t ctxt
    )
    {
        uint8_t* pSerialData;
        size_t length;

        // Serialize confirm and send through USI
        pSerialData = SRV_RSERIAL_SerialCfmMessage(cfmObj, RF215_TRX_ID_RF09,
                txHandle, &length);
        SRV_USI_Send_Message(srvUSIHandle, SRV_USI_PROT_ID_PHY_RF215,
                pSerialData, length);
    }
    </code>

  Remarks:
    None.
*/

uint8_t* SRV_RSERIAL_SerialCfmMessage (
    DRV_RF215_TX_CONFIRM_OBJ* pCfmObj,
    DRV_RF215_TRX_ID trxId,
    DRV_RF215_TX_HANDLE txHandle,
    size_t* pMsgLen
);

#endif //SRV_RSERIAL_H
