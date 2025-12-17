/*******************************************************************************
  Phy layer serialization header file

  Company:
    Microchip Technology Inc.

  File Name:
    srv_pserial.h

  Summary:
    Phy layer serialization service used by Microchip PLC Tools.

  Description:
    The Phy layer serialization provides a service to format messages
    through serial connection in order to communicate with PLC Tools provided
    by Microchip.

*******************************************************************************/

// DOM-IGNORE-BEGIN
/*
Copyright (C) 2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#ifndef SRV_PSERIAL_H    // Guards against multiple inclusion
#define SRV_PSERIAL_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "system/system.h"
#include "driver/plc/phy/drv_plc_phy_comm.h"

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
#define PSERIAL_TONEMAP_SIZE     3U
#define PSERIAL_SUBBANDS_SIZE    24U
#define PSERIAL_CARRIERS_SIZE    72U
#define PSERIAL_MAX_DATA_LEN     494U

/* PLC Phy Tester Tool command

  Summary:
    PLC Commands enumeration

  Description:
    This enumeration defines the PLC commands used by PLC Phy Tester Tool
    provided by Microchip.
*/
typedef enum
{
    /* Get data configuration request */
    SRV_PSERIAL_CMD_PHY_GET_CFG = 0,
    /* Get data configuration response */
    SRV_PSERIAL_CMD_PHY_GET_CFG_RSP,
    /* Set data configuration request */
    SRV_PSERIAL_CMD_PHY_SET_CFG,
    /* Set data configuration response */
    SRV_PSERIAL_CMD_PHY_SET_CFG_RSP,
    /* Get command request */
    SRV_PSERIAL_CMD_PHY_CMD_CFG,
    /* Get command response */
    SRV_PSERIAL_CMD_PHY_CMD_CFG_RSP,
    /* Send message data */
    SRV_PSERIAL_CMD_PHY_SEND_MSG,
    /* Send message data response */
    SRV_PSERIAL_CMD_PHY_SEND_MSG_RSP,
    /* Receive message data */
    SRV_PSERIAL_CMD_PHY_RECEIVE_MSG

} SRV_PSERIAL_COMMAND;

// *****************************************************************************
// *****************************************************************************
// Section: SRV_PSERIAL Interface Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    SRV_PSERIAL_COMMAND SRV_PSERIAL_GetCommand
    (
      uint8_t* pData
    )

  Summary:
    Extracts Command field from Serial frame.

  Description:
    Takes Serial frame as parameter and extracts the Command field from the
    expected position in buffer.

  Precondition:
    None.

  Parameters:
    pData - Pointer to buffer containing Serial frame

  Returns:
    Command in the form of SRV_PSERIAL_COMMAND Enum.

  Example:
    <code>
    SRV_PSERIAL_COMMAND command;

    command = SRV_PSERIAL_GetCommand(pData);
    </code>

  Remarks:
    None.
*/
SRV_PSERIAL_COMMAND SRV_PSERIAL_GetCommand(uint8_t* pData);

// *****************************************************************************
/* Function:
    void SRV_PSERIAL_ParseGetPIB
    (
      DRV_PLC_PHY_PIB_OBJ* pDataDst,
      uint8_t* pDataSrc
    )

  Summary:
    Extracts PIB field from SRV_PSERIAL_CMD_PHY_GET_CFG Serial frame.

  Description:
    Takes a GetPIB Serial frame as parameter, extracts the PIB information
    from the expected position in buffer and fills a DRV_PLC_PHY_PIB_OBJ object.

  Precondition:
    None.

  Parameters:
    pDataDst - Pointer to a DRV_PLC_PHY_PIB_OBJ object to fill
    pDataSrc - Pointer to buffer containing Serial frame

  Returns:
    None.

  Example:
    <code>
    SRV_PSERIAL_COMMAND command;
    DRV_PLC_PHY_PIB_OBJ pibObj;

    command = SRV_PSERIAL_GetCommand(pData);
    if (command == SRV_PSERIAL_CMD_PHY_GET_CFG) {
      SRV_PSERIAL_ParseGetPIB(&pibObj, pData);
    }
    </code>

  Remarks:
    None.
*/
void SRV_PSERIAL_ParseGetPIB(DRV_PLC_PHY_PIB_OBJ* pDataDst, uint8_t* pDataSrc);

// *****************************************************************************
/* Function:
    size_t SRV_PSERIAL_SerialGetPIB
    (
      uint8_t* pDataDst,
      DRV_PLC_PHY_PIB_OBJ* pDataSrc
    )

  Summary:
    Serializes a response to a SRV_PSERIAL_CMD_PHY_GET_CFG command.

  Description:
    Takes a DRV_PLC_PHY_PIB_OBJ object as parameter and builds a serialized
    frame containing the Get result and (if successful) the PIB value.

  Precondition:
    None.

  Parameters:
    pDataDst - Pointer to buffer where frame will be serialized
    pDataSrc - Pointer to a DRV_PLC_PHY_PIB_OBJ object containing PIB value

  Returns:
    Length of serialized frame.

  Example:
    <code>
    SRV_PSERIAL_COMMAND command;
    DRV_PLC_PHY_PIB_OBJ pibObj;

    command = SRV_PSERIAL_GetCommand(pData);
    if (command == SRV_PSERIAL_CMD_PHY_GET_CFG) {
      SRV_PSERIAL_ParseGetPIB(&pibObj, pData);
      if (DRV_PLC_PHY_PIBGet(appData.drvPlcHandle, &pibObj))
      {
        size_t len;
        len = SRV_PSERIAL_SerialGetPIB(appData.pSerialData, &pibObj);
        SRV_USI_Send_Message(appData.srvUSIHandle, SRV_USI_PROT_ID_PHY,
                appData.pSerialData, len);
      }
    }
    </code>

  Remarks:
    None.
*/
size_t SRV_PSERIAL_SerialGetPIB(uint8_t* pDataDst, DRV_PLC_PHY_PIB_OBJ* pDataSrc);

// *****************************************************************************
/* Function:
    void SRV_PSERIAL_ParseSetPIB
    (
      DRV_PLC_PHY_PIB_OBJ* pDataDst,
      uint8_t* pDataSrc
    )

  Summary:
    Extracts PIB field from SRV_PSERIAL_CMD_PHY_SET_CFG Serial frame.

  Description:
    Takes a SetPIB Serial frame as parameter, extracts the PIB information
    from the expected position in buffer and fills a DRV_PLC_PHY_PIB_OBJ object.

  Precondition:
    None.

  Parameters:
    pDataDst - Pointer to a DRV_PLC_PHY_PIB_OBJ object to fill
    pDataSrc - Pointer to buffer containing Serial frame

  Returns:
    None.

  Example:
    <code>
    SRV_PSERIAL_COMMAND command;
    DRV_PLC_PHY_PIB_OBJ pibObj;

    command = SRV_PSERIAL_GetCommand(pData);
    if (command == SRV_PSERIAL_CMD_PHY_SET_CFG) {
      SRV_PSERIAL_ParseSetPIB(&pibObj, pData);
    }
    </code>

  Remarks:
    None.
*/
void SRV_PSERIAL_ParseSetPIB(DRV_PLC_PHY_PIB_OBJ* pDataDst, uint8_t* pDataSrc);

// *****************************************************************************
/* Function:
    size_t SRV_PSERIAL_SerialSetPIB
    (
      uint8_t* pDataDst,
      DRV_PLC_PHY_PIB_OBJ* pDataSrc
    )

  Summary:
    Serializes a response to a SRV_PSERIAL_CMD_PHY_SET_CFG command.

  Description:
    Takes a DRV_PLC_PHY_PIB_OBJ object as parameter and builds a serialized
    frame containing the Set result.

  Precondition:
    None.

  Parameters:
    pDataDst - Pointer to buffer where frame will be serialized
    pDataSrc - Pointer to a DRV_PLC_PHY_PIB_OBJ object containing PIB value

  Returns:
    Length of serialized frame.

  Example:
    <code>
    SRV_PSERIAL_COMMAND command;
    DRV_PLC_PHY_PIB_OBJ pibObj;

    command = SRV_PSERIAL_GetCommand(pData);
    if (command == SRV_PSERIAL_CMD_PHY_SET_CFG) {
      SRV_PSERIAL_ParseSetPIB(&pibObj, pData);
      if (DRV_PLC_PHY_PIBSet(appData.drvPlcHandle, &pibObj))
      {
        size_t len;
        len = SRV_PSERIAL_SerialSetPIB(appData.pSerialData, &pibObj);
        SRV_USI_Send_Message(appData.srvUSIHandle, SRV_USI_PROT_ID_PHY,
                appData.pSerialData, len);
      }
    }
    </code>

  Remarks:
    None.
*/
size_t SRV_PSERIAL_SerialSetPIB(uint8_t* pDataDst, DRV_PLC_PHY_PIB_OBJ* pDataSrc);

// *****************************************************************************
/* Function:
    void SRV_PSERIAL_ParseTxMessage
    (
      DRV_PLC_PHY_TRANSMISSION_OBJ* pDataDst,
      uint8_t* pDataSrc
    )

  Summary:
    Extracts a PLC frame to be transmitted from SRV_PSERIAL_CMD_PHY_SEND_MSG
    Serial frame.

  Description:
    Takes a SendMsg Serial frame as parameter, extracts the PLC frame
    and its related transmission information
    and fills a DRV_PLC_PHY_TRANSMISSION_OBJ object.

  Precondition:
    None.

  Parameters:
    pDataDst - Pointer to a DRV_PLC_PHY_TRANSMISSION_OBJ object to fill
    pDataSrc - Pointer to buffer containing Serial frame

  Returns:
    None.

  Example:
    <code>
    SRV_PSERIAL_COMMAND command;
    DRV_PLC_PHY_TRANSMISSION_OBJ plcTxObj;

    command = SRV_PSERIAL_GetCommand(pData);
    if (command == SRV_PSERIAL_CMD_PHY_SEND_MSG) {
      SRV_PSERIAL_ParseTxMessage(&plcTxObj, pData);
      DRV_PLC_PHY_Send(appData.drvPlcHandle, &plcTxObj);
    }
    </code>

  Remarks:
    None.
*/
void SRV_PSERIAL_ParseTxMessage(DRV_PLC_PHY_TRANSMISSION_OBJ* pDataDst, uint8_t* pDataSrc);

// *****************************************************************************
/* Function:
    size_t SRV_PSERIAL_SerialRxMessage
    (
      uint8_t* pDataDst,
      DRV_PLC_PHY_RECEPTION_OBJ* pDataSrc
    )

  Summary:
    Serializes a received PLC frame and its related information.

  Description:
    Takes a DRV_PLC_PHY_RECEPTION_OBJ object as parameter and builds a serialized
    frame containing the PLC frame and its related reception parameters.

  Precondition:
    None.

  Parameters:
    pDataDst - Pointer to buffer where frame will be serialized
    pDataSrc - Pointer to a DRV_PLC_PHY_RECEPTION_OBJ object containing the PLC frame

  Returns:
    Length of serialized frame.

  Example:
    <code>
    static void APP_PLCDataIndCb(DRV_PLC_PHY_RECEPTION_OBJ *indObj, uintptr_t context)
    {
      if (indObj->dataLength)
      {
          size_t length;

          length = SRV_PSERIAL_SerialRxMessage(appData.pSerialData, indObj);
          SRV_USI_Send_Message(appData.srvUSIHandle, SRV_USI_PROT_ID_PHY,
                  appData.pSerialData, length);
      }
    }
    </code>

  Remarks:
    None.
*/
size_t SRV_PSERIAL_SerialRxMessage(uint8_t* pDataDst, DRV_PLC_PHY_RECEPTION_OBJ* pDataSrc);

// *****************************************************************************
/* Function:
    size_t SRV_PSERIAL_SerialCfmMessage
    (
      uint8_t* pDataDst,
      DRV_PLC_PHY_TRANSMISSION_CFM_OBJ* pDataSrc
    )

  Summary:
    Serializes the result of a PLC transmitted frame.

  Description:
    Takes a DRV_PLC_PHY_TRANSMISSION_CFM_OBJ object as parameter, and builds
    a serialized frame containing the PLC transmission result and parameters.

  Precondition:
    None.

  Parameters:
    pDataDst - Pointer to buffer where frame will be serialized
    pDataSrc - Pointer to a DRV_PLC_PHY_TRANSMISSION_CFM_OBJ object containing
               the PLC transmission result and parameters

  Returns:
    Length of serialized frame.

  Example:
    <code>
    static void APP_PLCDataCfmCb(DRV_PLC_PHY_TRANSMISSION_CFM_OBJ *cfmObj, uintptr_t context)
    {
      size_t length;

      length = SRV_PSERIAL_SerialCfmMessage(appData.pSerialData, cfmObj);
      SRV_USI_Send_Message(appData.srvUSIHandle, SRV_USI_PROT_ID_PHY,
              appData.pSerialData, length);
    }
    </code>

  Remarks:
    None.
*/
size_t SRV_PSERIAL_SerialCfmMessage(uint8_t* pDataDst, DRV_PLC_PHY_TRANSMISSION_CFM_OBJ* pDataSrc);

#endif //SRV_PSERIAL_H
