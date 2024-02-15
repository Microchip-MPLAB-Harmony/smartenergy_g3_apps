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

#define PSNIFFER_CARRIERS_SIZE    72
#define PSNIFFER_VERSION          0x02
#define PSNIFFER_PROFILE          0x12

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
  /* Receive new message */
  SRV_PSNIFFER_CMD_RECEIVE_MSG = 0,
  /* Set PLC Tone Mask */
  SRV_PSNIFFER_CMD_SET_PLC_TONE_MASK,
  /* Set RF Band, Operating Mode and Channel */
  SRV_PSNIFFER_CMD_SET_RF_BAND_OPM_CHANNEL
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
      appData.plcPIB.id = PLC_ID_RX_PAY_SYMBOLS;
      appData.plcPIB.length = 2;
      DRV_PLC_PHY_PIBGet(appData.drvPlcHandle, &appData.plcPIB);

      SRV_PSNIFFER_SetRxPayloadSymbols(*(uint16_t *)appData.plcPIB.pData);

      length = SRV_PSNIFFER_SerialRxMessage(appData.pSerialData, indObj);
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
      appData.plcPIB.id = PLC_ID_TX_PAY_SYMBOLS;
      appData.plcPIB.length = 2;
      DRV_PLC_PHY_PIBGet(appData.drvPlcHandle, &appData.plcPIB);

      SRV_PSNIFFER_SetTxPayloadSymbols(*(uint16_t *)appData.plcPIB.pData);

      length = SRV_PSNIFFER_SerialCfmMessage(appData.pSerialData, pCfmObj);
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
      SRV_PSNIFFER_SetTxMessage(pTxObj);
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
      appData.plcPIB.id = PLC_ID_RX_PAY_SYMBOLS;
      appData.plcPIB.length = 2;
      DRV_PLC_PHY_PIBGet(appData.drvPlcHandle, &appData.plcPIB);

      SRV_PSNIFFER_SetRxPayloadSymbols(*(uint16_t *)appData.plcPIB.pData);

      length = SRV_PSNIFFER_SerialRxMessage(appData.pSerialData, indObj);
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
      appData.plcPIB.id = PLC_ID_TX_PAY_SYMBOLS;
      appData.plcPIB.length = 2;
      DRV_PLC_PHY_PIBGet(appData.drvPlcHandle, &appData.plcPIB);

      SRV_PSNIFFER_SetTxPayloadSymbols(*(uint16_t *)appData.plcPIB.pData);

      length = SRV_PSNIFFER_SerialCfmMessage(appData.pSerialData, pCfmObj);
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
    void SRV_PSNIFFER_ConvertToneMask(uint8_t* pDataSrc, uint8_t* pToneMaskDst)

  Summary:
    Converts a Tone Mask configuration command coming the Microchip Hybrid
    Sniffer Tool to the right format to be configured on G3-PLC PHY layer.

  Description:
    This function takes a Tone Mask configuration command with the format coming
    from the Microchip Hybrid Sniffer Tool and converts it to a format to be
    configured on G3-PLC PHY layer to be used on further transmissions and
    receptions.
    If misconfigured, no PLC frames will be seen.

  Precondition:
    None.

  Parameters:
    pDataSrc     - Pointer to the data where the command is stored (USI)
    pToneMaskDst - The Tone Mask converted to PHY format

  Returns:
    None.

  Example:
    <code>
    switch (command) {
        case SRV_PSNIFFER_CMD_SET_PLC_TONE_MASK:
        {
            SRV_PSNIFFER_ConvertToneMask(pData, appData.plcPIB.pData);

            appData.plcPIB.id = PLC_ID_TONE_MASK;
            appData.plcPIB.length = PSNIFFER_CARRIERS_SIZE;
            DRV_PLC_PHY_PIBSet(appData.drvPlcHandle, &appData.plcPIB);
            break;
        }
    }
    </code>

  Remarks:
    This function is only available in G3-PLC profile.
*/
void SRV_PSNIFFER_ConvertToneMask(uint8_t* pDataSrc, uint8_t* pToneMaskDst);


#endif //SRV_PSNIFFER_H
