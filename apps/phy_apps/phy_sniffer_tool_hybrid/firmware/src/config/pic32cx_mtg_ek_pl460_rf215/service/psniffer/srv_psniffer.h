/*******************************************************************************
  Phy Sniffer Serialization header file.

  Company:
    Microchip Technology Inc.

  File Name:
    srv_psniffer.c

  Summary:
    Phy Sniffer serialization service used by Microchip PLC Sniffer Tool.

  Description:
    The Phy Sniffer serialization provides a service to format messages
    through serial connection in order to communicate with PLC Sniffer Tool 
    provided by Microchip.
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2021 Microchip Technology Inc. and its subsidiaries.
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

#ifndef SRV_PSNIFFER_H    // Guards against multiple inclusion
#define SRV_PSNIFFER_H


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
// Section: Macro definitions
// *****************************************************************************
// *****************************************************************************
#define PSNIFFER_TONEMAP_SIZE     3
#define PSNIFFER_SUBBANDS_SIZE    24   
#define PSNIFFER_CARRIERS_SIZE    72        
#define PSNIFFER_RS_2_BLOCKS      1
#define PSNIFFER_VERSION          0x02
#define PSNIFFER_PROFILE          0x12

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

/* PLC Phy Sniffer Tool command

  Summary:
    PLC Sniffer Commands enumeration

  Description:
    This enumeration defines the PLC commands used by PLC Phy Sniffer Tool
    provided by Microchip.
*/
typedef enum
{
  /* Receive new PLC message */
  SRV_PSNIFFER_CMD_RECEIVE_MSG = 0,
  /* Set Tone Mask request */ 
  SRV_PSNIFFER_CMD_SET_TONE_MASK,
} SRV_PSNIFFER_COMMAND;   

// *****************************************************************************
// *****************************************************************************
// Section: SRV_PSNIFFER Interface Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    SRV_PSNIFFER_COMMAND SRV_PSNIFFER_GetCommand
    (
      uint8_t* pData
    )

  Summary:
    Extracts Command field from Sniffer frame.

  Description:
    Takes Sniffer frame as parameter and extracts the Command field from the
    expected position in buffer.

  Precondition:
    None.

  Parameters:
    pData - Pointer to buffer containing Sniffer frame

  Returns:
    Command in the form of SRV_PSNIFFER_COMMAND Enum.

  Example:
    <code>
    SRV_PSNIFFER_COMMAND command;

    // Process received message from Sniffer Tool
    command = SRV_PSNIFFER_GetCommand(pData);
    </code>

  Remarks:
    None.
*/
SRV_PSNIFFER_COMMAND SRV_PSNIFFER_GetCommand(uint8_t* pData);

// *****************************************************************************
/* Function:
    size_t SRV_PSNIFFER_SerialRxMessage
    (
      uint8_t* pDataDst,
      DRV_PLC_PHY_RECEPTION_OBJ* pDataSrc
    )

  Summary:
    Serializes a received PLC frame along with its parameters.

  Description:
    This function takes an object containing a PLC frame and its related
    parameters and serializes it in a buffer for further transmission.

  Precondition:
    SRV_PSNIFFER_SetRxPayloadSymbols has to be called before to set the
    correct number of received symbols in Sniffer library.

  Parameters:
    pDataDst - Pointer to buffer where frame is serialized

    pDataSrc - Pointer to PLC Reception object containing the frame
               and parameters

  Returns:
    Number of bytes for serialized output.

  Example:
    <code>
    static void APP_PLCDataIndCb(DRV_PLC_PHY_RECEPTION_OBJ *indObj, uintptr_t context)
    {
      // Report RX Symbols
      appData.plcPIB.id = PLC_ID_RX_PAY_SYMBOLS;
      appData.plcPIB.length = 2;
      DRV_PLC_PHY_PIBGet(appData.drvPl360Handle, &appData.plcPIB);

      SRV_PSNIFFER_SetRxPayloadSymbols(*(uint16_t *)appData.plcPIB.pData);

      // Serialize received message
      length = SRV_PSNIFFER_SerialRxMessage(appData.pSerialData, indObj);
      // Send through USI
      SRV_USI_Send_Message(appData.srvUSIHandle, SRV_USI_PROT_ID_SNIFF_G3,
              appData.pSerialData, length);
    }
    </code>

  Remarks:
    None.
*/
size_t SRV_PSNIFFER_SerialRxMessage(uint8_t* pDataDst, DRV_PLC_PHY_RECEPTION_OBJ* pDataSrc);

// *****************************************************************************
/* Function:
    size_t SRV_PSNIFFER_SerialCfmMessage
    (
      uint8_t* pDataDst,
      DRV_PLC_PHY_TRANSMISSION_CFM_OBJ* pCfmObj
    )

  Summary:
    Serializes a transmitted PLC frame along with its parameters.

  Description:
    This function takes a previously stored PLC transmitted frame and its
    related parameters and serializes it in a buffer for further transmission.

  Precondition:
    SRV_PSNIFFER_SetTxMessage has to be previously called to store the PLC
    transmitted frame and its parameters.
    SRV_PSNIFFER_SetTxPayloadSymbols has to be called before to set the
    correct number of transmitted symbols in Sniffer library.

  Parameters:
    pDataDst - Pointer to buffer where frame is serialized

    pCfmObj - Pointer to PLC Transmission Confirm object containing the
              transmission result

  Returns:
    Number of bytes for serialized output.

  Example:
    <code>
    static void APP_PLCDataCfmCb(DRV_PLC_PHY_TRANSMISSION_CFM_OBJ *pCfmObj)
    {
      // Report TX Symbols
      appData.plcPIB.id = PLC_ID_TX_PAY_SYMBOLS;
      appData.plcPIB.length = 2;
      DRV_PLC_PHY_PIBGet(appData.drvPl360Handle, &appData.plcPIB);

      SRV_PSNIFFER_SetTxPayloadSymbols(*(uint16_t *)appData.plcPIB.pData);

      // Serialize transmitted message
      length = SRV_PSNIFFER_SerialCfmMessage(appData.pSerialData, pCfmObj);
      // Send through USI
      SRV_USI_Send_Message(appData.srvUSIHandle, SRV_USI_PROT_ID_SNIFF_G3,
              appData.pSerialData, length);
    }
    </code>

  Remarks:
    None.
*/
size_t SRV_PSNIFFER_SerialCfmMessage(uint8_t* pDataDst, DRV_PLC_PHY_TRANSMISSION_CFM_OBJ* pCfmObj);

// *****************************************************************************
/* Function:
    void SRV_PSNIFFER_SetTxMessage
    (
      DRV_PLC_PHY_TRANSMISSION_OBJ* pTxObj
    )

  Summary:
    Gives a transmitted PLC object to Sniffer library so it is stored for
    later serialization.

  Description:
    The given transmitted PLC object contains a PLC frame and its related
    parameters, all this info is stored in sniffer library for later
    serialization when SRV_PSNIFFER_SerialCfmMessage is called.

  Precondition:
    None.

  Parameters:
    pTxObj - Pointer to a PLC transmission object to be stored

  Returns:
    None.

  Example:
    <code>
    static void APP_PLCTxFrame(DRV_PLC_PHY_TRANSMISSION_OBJ* pTxObj)
    {
      // Give transmission object to Sniffer
      SRV_PSNIFFER_SetTxMessage(pTxObj);
      // Send PLC frame through PHY Driver
      // DRV_PLC_PHY_xxx(pTxObj);
    }
    </code>

  Remarks:
    None.
*/
void SRV_PSNIFFER_SetTxMessage(DRV_PLC_PHY_TRANSMISSION_OBJ* pTxObj);

// *****************************************************************************
/* Function:
    void SRV_PSNIFFER_SetRxPayloadSymbols
    (
      uint16_t payloadSym
    )

  Summary:
    Sets the number of received symbols on Sniffer library for further use.

  Description:
    Number of received symbols is set on Sniffer library so that info can be
    later serialized along with other reception parameters.

  Precondition:
    None.

  Parameters:
    payloadSym - Number of symbols

  Returns:
    None.

  Example:
    <code>
    static void APP_PLCDataIndCb(DRV_PLC_PHY_RECEPTION_OBJ *indObj, uintptr_t context)
    {
      // Report RX Symbols
      appData.plcPIB.id = PLC_ID_RX_PAY_SYMBOLS;
      appData.plcPIB.length = 2;
      DRV_PLC_PHY_PIBGet(appData.drvPl360Handle, &appData.plcPIB);

      SRV_PSNIFFER_SetRxPayloadSymbols(*(uint16_t *)appData.plcPIB.pData);

      // Serialize received message
      length = SRV_PSNIFFER_SerialRxMessage(appData.pSerialData, indObj);
      // Send through USI
      SRV_USI_Send_Message(appData.srvUSIHandle, SRV_USI_PROT_ID_SNIFF_G3,
              appData.pSerialData, length);
    }
    </code>

  Remarks:
    None.
*/
void SRV_PSNIFFER_SetRxPayloadSymbols(uint16_t payloadSym);

// *****************************************************************************
/* Function:
    void SRV_PSNIFFER_SetTxPayloadSymbols
    (
      uint16_t payloadSym
    )

  Summary:
    Sets the number of transmitted symbols on Sniffer library for further use.

  Description:
    Number of transmitted symbols is set on Sniffer library so that info can be
    later serialized along with other transmission parameters.

  Precondition:
    None.

  Parameters:
    payloadSym - Number of symbols

  Returns:
    None.

  Example:
    <code>
    static void APP_PLCDataCfmCb(DRV_PLC_PHY_TRANSMISSION_CFM_OBJ *pCfmObj)
    {
      // Report TX Symbols
      appData.plcPIB.id = PLC_ID_TX_PAY_SYMBOLS;
      appData.plcPIB.length = 2;
      DRV_PLC_PHY_PIBGet(appData.drvPl360Handle, &appData.plcPIB);

      SRV_PSNIFFER_SetTxPayloadSymbols(*(uint16_t *)appData.plcPIB.pData);

      // Serialize transmitted message
      length = SRV_PSNIFFER_SerialCfmMessage(appData.pSerialData, pCfmObj);
      // Send through USI
      SRV_USI_Send_Message(appData.srvUSIHandle, SRV_USI_PROT_ID_SNIFF_G3,
              appData.pSerialData, length);
    }
    </code>

  Remarks:
    None.
*/
void SRV_PSNIFFER_SetTxPayloadSymbols(uint16_t payloadSym);


// *****************************************************************************
/* Function:
    void SRV_PSNIFFER_ConvertToneMask
    (
      uint8_t* pToneMaskDst,
      uint8_t* pToneMaskSrc
    )

  Summary:
    Converts a Tone Mask configuration coming from an external tool to the
    right format to be configured on PLC PHY layer.

  Description:
    This function takes a Tone Mask configuration with the format coming from
    MCHP PLC Sniffer Tool and converts it to a format to be configured on PLC PHY
    layer to be used on further transmissions and receptions.
    If misconfigured, no PLC frames will be seen.

  Precondition:
    None.

  Parameters:
    pToneMaskDst - The Tone Mask converted to PHY format
    pToneMaskSrc - The Tone Mask with format coming from external tool

  Returns:
    None.

  Example:
    <code>
	  switch (command) {
        case SRV_PSNIFFER_CMD_SET_TONE_MASK:
        {
            // Convert ToneMask from Sniffer Tool to PLC phy layer
            SRV_PSNIFFER_ConvertToneMask(appData.plcPIB.pData, pData + 1);

            // Send data to PLC
            appData.plcPIB.id = PLC_ID_TONE_MASK;
            appData.plcPIB.length = PSNIFFER_CARRIERS_SIZE;
            DRV_PLC_PHY_PIBSet(appData.drvPl360Handle, &appData.plcPIB);
        }
        break;

        // ...
    }
    </code>

  Remarks:
    This function is only available in G3 PLC profile.
*/
void SRV_PSNIFFER_ConvertToneMask(uint8_t* pToneMaskDst, uint8_t* pToneMaskSrc);


#endif //SRV_PSNIFFER_H
