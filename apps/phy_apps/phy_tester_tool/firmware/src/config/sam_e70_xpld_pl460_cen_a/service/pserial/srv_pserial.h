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
#define PSERIAL_TONEMAP_SIZE     3
#define PSERIAL_SUBBANDS_SIZE    24   
#define PSERIAL_CARRIERS_SIZE    72        
#define PSERIAL_RS_2_BLOCKS      1

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
  SRV_PSERIAL_CMD_PHY_RECEIVE_MSG,        
  /* Noise capture request */
  SRV_PSERIAL_CMD_PHY_NOISE_REQ,         
  /* Noise capture response */ 
  SRV_PSERIAL_CMD_PHY_NOISE_RSP,          
  /* Get parameter list */
  SRV_PSERIAL_CMD_PHY_GET_CFG_LIST,       
  /* Parameter list response */
  SRV_PSERIAL_CMD_PHY_GET_CFG_LIST_RSP,   
  /* Reset PHY layer */
  SRV_PSERIAL_CMD_PHY_RESET_PHY_LAYER     
} SRV_PSERIAL_COMMAND;      
  
// *****************************************************************************
// *****************************************************************************
// Section: SRV_PSERIAL Interface Routines
// *****************************************************************************
// *****************************************************************************

SRV_PSERIAL_COMMAND SRV_PSERIAL_GetCommand(uint8_t* pData);       
void SRV_PSERIAL_ParseGetPIB(DRV_PLC_PHY_PIB_OBJ* pDataDst, uint8_t* pDataSrc);
size_t SRV_PSERIAL_SerialGetPIB(uint8_t* pDataDst, DRV_PLC_PHY_PIB_OBJ* pDataSrc);
void SRV_PSERIAL_ParseSetPIB(DRV_PLC_PHY_PIB_OBJ* pDataDst, uint8_t* pDataSrc);
size_t SRV_PSERIAL_SerialSetPIB(uint8_t* pDataDst, DRV_PLC_PHY_PIB_OBJ* pDataSrc);
void SRV_PSERIAL_ParseTxMessage(DRV_PLC_PHY_TRANSMISSION_OBJ* pDataDst, uint8_t* pDataSrc);
size_t SRV_PSERIAL_SerialRxMessage(uint8_t* pDataDst, DRV_PLC_PHY_RECEPTION_OBJ* pDataSrc);
size_t SRV_PSERIAL_SerialCfmMessage(uint8_t* pDataDst, DRV_PLC_PHY_TRANSMISSION_CFM_OBJ* pDataSrc);

#endif //SRV_PSERIAL_H
