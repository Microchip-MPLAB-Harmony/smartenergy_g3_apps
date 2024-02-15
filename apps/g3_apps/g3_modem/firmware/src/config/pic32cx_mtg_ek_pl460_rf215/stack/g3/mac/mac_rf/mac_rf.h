/*******************************************************************************
  G3 MAC RF Header File

  Company:
    Microchip Technology Inc.

  File Name:
    mac_rf.h

  Summary:
    G3 MAC RF API Header File

  Description:
    This file contains definitions of the primitives and related types
    to be used by MAC Wrapper when accessing G3 MAC RF layer.
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

#ifndef MAC_RF_H
#define MAC_RF_H

#include "../mac_common/mac_common.h"
#include "mac_rf_mib.h"

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
/* MAC RF Callback Handlers Structure

   Summary:
    Set of Event Handler function pointers to receive events from MAC RF.

   Description:
    Defines the set of callback functions that MAC RF uses to generate
    events to upper layer.

   Remarks:
    In case an event is to be ignored, setting its corresponding callback
    function to NULL will lead to the event not being generated.
*/
typedef struct
{
    /* Callbacks */
    MAC_DataConfirm macRfDataConfirm;
    MAC_DataIndication macRfDataIndication;
    MAC_ResetConfirm macRfResetConfirm;
    MAC_BeaconNotifyIndication macRfBeaconNotifyIndication;
    MAC_ScanConfirm macRfScanConfirm;
    MAC_StartConfirm macRfStartConfirm;
    MAC_CommStatusIndication macRfCommStatusIndication;
    MAC_SnifferIndication macRfMacSnifferIndication;
} MAC_RF_HANDLERS;

// *****************************************************************************
/* MAC RF Init Structure

   Summary:
    Initialization Data for MAC RF to be provided on Init routine.

   Description:
    Defines the set of callback functions that MAC RF uses to generate
    events to upper layer and a pointer to the MAC Tables structure.

   Remarks:
    In case an event is to be ignored, setting its corresponding callback
    function to NULL will lead to the event not being generated.
*/
typedef struct
{
    /* Callbacks */
    MAC_RF_HANDLERS macRfHandlers;
    /* Pointer to MAC Tables */
    MAC_RF_TABLES *macRfTables;
    /* RF PAL index from configuration */
    uint8_t palRfIndex;
} MAC_RF_INIT;

// *****************************************************************************
/* MAC RF State Machine Definition

  Summary:
    Defines the states of the MAC RF State Machine.

  Description:
    None.

  Remarks:
    None.
*/
typedef enum
{
    MAC_RF_STATE_IDLE,
    MAC_RF_STATE_TX,
    MAC_RF_STATE_WAITING_TX_CFM,
    MAC_RF_STATE_ERROR,
}MAC_RF_STATE;

// *****************************************************************************
/* MAC RF Data Structure

   Summary:
    Object used to keep any data required for an instance of the module.

   Description:
    Contains status of module state machine, runtime variables, pointer to
    Mac Tables and callback definitions.

   Remarks:
    None.
*/
typedef struct
{
    /* Flag to indicate this object is in use  */
    bool inUse;
    /* State of the MAC RF module */
    MAC_RF_STATE state;
    /* Callbacks */
    MAC_RF_HANDLERS macRfHandlers;
    /* Pointer to MAC Tables */
    MAC_RF_TABLES *macRfTables;
} MAC_RF_DATA;


// *****************************************************************************
// *****************************************************************************
// Section: MAC RF Interface Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    void MAC_RF_Init
    (
      MAC_RF_INIT *init
    )

  Summary:
    Initializes the MAC RF module.

  Description:
    This routine initializes the MAC RF.
    Callback handlers for event notification are set in this function.
    A Pointer to MAC RF Tables is also set here so MAC library can use them.

  Precondition:
    None.

  Parameters:
    init  - Pointer to the init data structure containing any data necessary to
            initialize the module.

  Returns:
    If successful, returns a valid module instance object.
    Otherwise, returns SYS_MODULE_OBJ_INVALID.

  Example:
    <code>
    MAC_RF_INIT macRfInit = {
        .macRfHandlers.macRfDataConfirm = appDataConfirm,
        .macRfHandlers.macRfDataIndication = appDataIndication,
        .macRfHandlers.macRfResetConfirm = appResetConfirm,
        .macRfHandlers.macRfBeaconNotifyIndication = appBeaconIndication,
        .macRfHandlers.macRfScanConfirm = appScanConfirm,
        .macRfHandlers.macRfStartConfirm = NULL,
        .macRfHandlers.macRfCommStatusIndication = appCommStatus,
        .macRfHandlers.macRfMacSnifferIndication = NULL,
        .macRfTables = &tables,
        .palRfIndex = 0,
    };

    MAC_RF_Init(&macRfInit);
    </code>

  Remarks:
    This routine must be called before any other MAC RF routine is called.
*/
void MAC_RF_Init(MAC_RF_INIT *init);

// *****************************************************************************
/* Function:
    void MAC_RF_Tasks
    (
      void
    )

  Summary:
    Maintains MAC RF State Machine.

  Description:
    MAC RF State Machine controls MAC layer duties, such as transmitting and
    receiving frames, managing RF medium access or ensure link reliability.

  Precondition:
    MAC_RF_Init routine must have been called before.

  Parameters:
    object - Identifier for the object instance

  Returns:
    None.

  Example:
    <code>
    MAC_RF_Init(&macRfInit);

    while (true)
    {
        MAC_RF_Tasks();
    }
    </code>

  Remarks:
    None.
*/
void MAC_RF_Tasks(void);

// *****************************************************************************
/* Function:
    SYS_STATUS MAC_RF_Status
    (
      void
    )

  Summary:
    The MAC_RF_Status primitive retrieves the Status of RF MAC.

  Description:
    This primitive is intended to be called before using RF MAC layer
    to ensure it is ready to be used.

  Precondition:
    MAC_RF_Init routine must have been called before.

  Parameters:
    None

  Returns:
    Status of RF MAC layer as a SYS_STATUS code.

  Example:
    <code>
    SYS_STATUS status;
    status = MAC_RF_Status();
    if (status == SYS_STATUS_READY)
    {

    }
    </code>

  Remarks:
    None.
*/
SYS_STATUS MAC_RF_Status(void);

// *****************************************************************************
/* Function:
    void MAC_RF_DataRequest
    (
      MAC_DATA_REQUEST_PARAMS *drParams
    )

  Summary:
    The MAC_RF_DataRequest primitive requests the transfer of a PDU
    to another device or multiple devices.

  Description:
    DataRequest primitive is used to transfer data to other nodes in the G3
    Network. Result is provided in the corresponding Confirm callback.

  Precondition:
    MAC_RF_Init routine must have been called before.

  Parameters:
    drParams - Pointer to structure containing required parameters for request

  Returns:
    None.

  Example:
    <code>
    uint8_t txBuffer[MAC_PDU_SIZE];

    MAC_DATA_REQUEST_PARAMS params = {
        .srcAddressMode = MAC_ADDRESS_MODE_SHORT,
        .destPanId = 0x1234,
        .destAddress = 0x0002,
        .msduLength = 20,
        .msdu = &txBuffer[0],
        .msduHandle = appHandle++,
        .txOptions = MAC_TX_OPTION_ACK,
        .securityLevel = MAC_SECURITY_LEVEL_ENC_MIC_32,
        .keyIndex = 0,
        .qualityOfService = MAC_QUALITY_OF_SERVICE_NORMAL_PRIORITY,
    };

    MAC_RF_DataRequest(&params);
    </code>

  Remarks:
    None.
*/
void MAC_RF_DataRequest(MAC_DATA_REQUEST_PARAMS *drParams);

// *****************************************************************************
/* Function:
    MAC_STATUS MAC_RF_GetRequestSync
    (
      MAC_RF_PIB_ATTRIBUTE attribute,
      uint16_t index,
      MAC_PIB_VALUE *pibValue
    )

  Summary:
    The MAC_RF_GetRequestSync primitive gets the value of an attribute in the
    MAC layer Parameter Information Base (PIB).

  Description:
    GetRequestSync primitive is used to get the value of a PIB.
    Sync suffix indicates that result is provided upon function call return,
    in the pibValue parameter.

  Precondition:
    MAC_RF_Init routine must have been called before.

  Parameters:
    attribute - Identifier of the Attribute to retrieve value

    index - Index of element in case Attribute is a table
            Otherwise index must be set to '0'

    pibValue - Pointer to MAC_PIB_VALUE object where value will be returned

  Returns:
    Result of get operation as a MAC_STATUS code.

  Example:
    <code>
    MAC_STATUS status;
    MAC_PIB_VALUE value;
    status = MAC_RF_GetRequestSync(MAC_PIB_MAX_FRAME_RETRIES_RF, 0, &value);
    if (status == MAC_STATUS_SUCCESS)
    {

    }
    </code>

  Remarks:
    None.
*/
MAC_STATUS MAC_RF_GetRequestSync(MAC_RF_PIB_ATTRIBUTE attribute,
    uint16_t index, MAC_PIB_VALUE *pibValue);

// *****************************************************************************
/* Function:
    MAC_STATUS MAC_RF_SetRequestSync
    (
      MAC_RF_PIB_ATTRIBUTE attribute,
      uint16_t index,
      const MAC_PIB_VALUE *pibValue
    )

  Summary:
    The MAC_RF_SetRequestSync primitive sets the value of an attribute in the
    MAC layer Parameter Information Base (PIB).

  Description:
    SetRequestSync primitive is used to set the value of a PIB.
    Sync suffix indicates that result of set operation is provided upon
    function call return, in the return status code.

  Precondition:
    MAC_RF_Init routine must have been called before.

  Parameters:
    attribute - Identifier of the Attribute to provide value

    index - Index of element in case Attribute is a table
            Otherwise index must be set to '0'

    pibValue - Pointer to MAC_PIB_VALUE object where value is contained

  Returns:
    Result of set operation as a MAC_STATUS code.

  Example:
    <code>
    MAC_STATUS status;
    const MAC_PIB_VALUE value = {
        .length = 1,
        .value = {6}
    };

    status = MAC_RF_SetRequestSync(MAC_PIB_MAX_FRAME_RETRIES_RF, 0, &value);
    if (status == MAC_STATUS_SUCCESS)
    {

    }
    </code>

  Remarks:
    None.
*/
MAC_STATUS MAC_RF_SetRequestSync(MAC_RF_PIB_ATTRIBUTE attribute,
    uint16_t index, const MAC_PIB_VALUE *pibValue);

// *****************************************************************************
/* Function:
    void MAC_RF_ResetRequest
    (
      MAC_RESET_REQUEST_PARAMS *rstParams
    )

  Summary:
    The MAC_RF_ResetRequest primitive resets the MAC RF module.

  Description:
    Reset operation initializes MAC RF State Machine and PIB to their
    default values. Result is provided in the corresponding Confirm callback.

  Precondition:
    MAC_RF_Init routine must have been called before.

  Parameters:
    rstParams - Pointer to structure containing required parameters for request

  Returns:
    None.

  Example:
    <code>
    MAC_RESET_REQUEST_PARAMS params = {
        .setDefaultPib = true
    };

    MAC_RF_ResetRequest(&params);
    </code>

  Remarks:
    None.
*/
void MAC_RF_ResetRequest(MAC_RESET_REQUEST_PARAMS *rstParams);

// *****************************************************************************
/* Function:
    void MAC_RF_ScanRequest
    (
      MAC_SCAN_REQUEST_PARAMS *scanParams
    )

  Summary:
    The MAC_RF_ScanRequest primitive sets MAC layer in Network Scan mode.

  Description:
    Scan operation asks MAC layer to send a Beacon Request frame and wait
    for incoming Beacon frames.
    During the Scan period, Beacons received will be notified by means of
    MAC_BeaconNotifyIndication callback.
    Result is provided in the corresponding Confirm callback.

  Precondition:
    MAC_RF_Init routine must have been called before.

  Parameters:
    scanParams - Pointer to structure containing required parameters for request

  Returns:
    None.

  Example:
    <code>
    MAC_SCAN_REQUEST_PARAMS params = {
        .scanDuration = 15
    };

    MAC_RF_ScanRequest(&params);
    </code>

  Remarks:
    None.
*/
void MAC_RF_ScanRequest(MAC_SCAN_REQUEST_PARAMS *scanParams);

// *****************************************************************************
/* Function:
    void MAC_RF_StartRequest
    (
      MAC_START_REQUEST_PARAMS *startParams
    )

  Summary:
    The MAC_RF_StartRequest primitive starts a G3 Network and sets the device
    as the PAN Coordinator.

  Description:
    Start operation asks MAC layer to start a G3 Network, turning the device
    into the PAN Coordinator of such Network, and setting the PAN Identifier.
    Result is provided in the corresponding Confirm callback.

  Precondition:
    MAC_RF_Init routine must have been called before.

  Parameters:
    startParams - Pointer to structure containing required parameters for request

  Returns:
    None.

  Example:
    <code>
    MAC_START_REQUEST_PARAMS params = {
        .panId = 0x1234
    };

    MAC_RF_StartRequest(&params);
    </code>

  Remarks:
    None.
*/
void MAC_RF_StartRequest(MAC_START_REQUEST_PARAMS *srParams);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // #ifndef MAC_RF_H

/*******************************************************************************
 End of File
*/
