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
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
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
#define PSNIFFER_TONEMAP_SIZE     1
#define PSNIFFER_SUBBANDS_SIZE    6   
#define PSNIFFER_CARRIERS_SIZE    36        
#define PSNIFFER_RS_2_BLOCKS      0
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
  /* Set PLC Channel */    
  SRV_PSNIFFER_CMD_SET_CHANNEL,           
} SRV_PSNIFFER_COMMAND;      

// *****************************************************************************
// *****************************************************************************
// Section: SRV_PSNIFFER Interface Routines
// *****************************************************************************
// *****************************************************************************

SRV_PSNIFFER_COMMAND SRV_PSNIFFER_GetCommand(uint8_t* pData);
size_t SRV_PSNIFFER_SerialRxMessage(uint8_t* pDataDst, DRV_PLC_PHY_RECEPTION_OBJ* pDataSrc);
size_t SRV_PSNIFFER_SerialCfmMessage(uint8_t* pDataDst, DRV_PLC_PHY_TRANSMISSION_CFM_OBJ* pDataCfm);
void SRV_PSNIFFER_SetTxMessage(DRV_PLC_PHY_TRANSMISSION_OBJ* pDataDst);
void SRV_PSNIFFER_SetRxPayloadSymbols(uint16_t payloadSym);
void SRV_PSNIFFER_SetTxPayloadSymbols(uint16_t payloadSym);
void SRV_PSNIFFER_ConvertToneMask(uint8_t* pToneMaskDst, uint8_t* pToneMaskSrc);

#endif //SRV_PSNIFFER_H
