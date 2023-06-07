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

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
/* PAR RF PHY Data information

  Summary:
    Defines the data necessary to build the serial message when receiving RF data.

  Description:
    This data type defines the data needed by RF Sniffer to be serialized.

  Remarks:
    None.
*/

typedef struct 
{
    /* Pointer to RF PHY data payload */
    uint8_t *pData;

    /* Time counter when the packet has been received/transmitted */
    uint32_t timeIniCount;

    /* Packet duration */
    uint32_t durationCount;

    /* RSSI value in dBm */
    uint16_t rssi;

    /* Number of symbols of the payload */
    uint16_t paySymbols;

    /* Payload length in bytes */
    uint16_t payloadLen;

} SRV_RSNIFFER_PHY_DATA;

// *****************************************************************************
// *****************************************************************************
// Section: RF PHY Sniffer Serialization Interface Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    SRV_RSNIFFER_SerialRxMessage ( SRV_RSNIFFER_PHY_DATA *rxData, size_t *pMsgLen )

  Summary:
    Serializes a received RF frame along with its parameters.

  Description:
    This function takes an object containing a RF frame and its related
    parameters and serializes it in a buffer for further transmission through
    serial interface.

  Precondition:
    None.

  Parameters:
    rxData     - Pointer to RF reception data containing the frame and
                 parameters
    pMsgLen    - Pointer to sniffer message length in bytes (output)

  Returns:
    Pointer to sniffer message to be sent through serial interface.

  Example:
    <code>


    </code>

  Remarks:
    None.
*/

uint8_t* SRV_RSNIFFER_SerialRxMessage ( SRV_RSNIFFER_PHY_DATA *rxData, size_t *pMsgLen );

// *****************************************************************************
/* Function:
    uint8_t* SRV_RSNIFFER_SerialCfmMessage ( SRV_RSNIFFER_PHY_DATA *txData, size_t *pMsgLen )

  Summary:
    Serializes a transmitted RF frame along with its parameters.

  Description:
    This function serializes the RF transmitted packet in a buffer for further transmission
    through serial interface.

  Precondition:
    None.

  Parameters:
    rxData     - Pointer to RF transmission data containing the frame and
                 parameters
    pMsgLen    - Pointer to sniffer message length in bytes (output)

  Returns:
    Pointer to sniffer message to be sent through serial interface.

  Example:
    <code>
    

    </code>

  Remarks:
    None.
*/

uint8_t* SRV_RSNIFFER_SerialCfmMessage ( SRV_RSNIFFER_PHY_DATA *txData, size_t *pMsgLen );

#endif //SRV_RSNIFFER_H
