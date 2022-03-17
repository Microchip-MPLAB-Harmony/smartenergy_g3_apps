/*******************************************************************************
  PLC Driver Local G3 Data Structures

  Company:
    Microchip Technology Inc.

  File Name:
    drv_g3_macrt_local_comm.h

  Summary:
    G3 MAC RT Driver Local G3 MAC RT Data Structures

  Description:
    Driver Local G3 MAC RT Data Structures
*******************************************************************************/

//DOM-IGNORE-BEGIN
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
//DOM-IGNORE-END

#ifndef _DRV_G3_MACRT_LOCAL_COMM_H
#define _DRV_G3_MACRT_LOCAL_COMM_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "driver/plc/g3MacRt/drv_g3_macrt_comm.h"
#include "driver/plc/g3MacRt/drv_g3_macrt_local.h"

// *****************************************************************************
// *****************************************************************************
// Section: Macro Definitions
// *****************************************************************************
// ***************************************************************************** 
#define DRV_G3_MACRT_REG_CMD_RD            (0)
#define DRV_G3_MACRT_REG_CMD_WR            (1)

#define DRV_G3_MACRT_STATUS_LENGTH         12
#define DRV_G3_MACRT_DATA_MAX_SIZE         MAC_RT_DATA_MAX_SIZE
#define DRV_G3_MACRT_COMM_STATUS_SIZE      32
#define DRV_G3_MACRT_TX_CFM_SIZE           sizeof(MAC_RT_TX_CFM_OBJ)
#define DRV_G3_MACRT_RX_PAR_SIZE           sizeof(MAC_RT_RX_PARAMETERS_OBJ)
#define DRV_G3_MACRT_REG_PKT_SIZE          sizeof(MAC_RT_PIB_OBJ)

/* ! Maximum length of PHY message (G3). Worts case: G3-FCC */
#define DRV_G3_MACRT_PHY_MAX_SIZE          494

/* FLAG MASKs for set events */
#define DRV_G3_MACRT_EV_TX_CFM_FLAG_MASK                 (1<<0)
#define DRV_G3_MACRT_EV_DATA_IND_FLAG_MASK               (1<<1)
#define DRV_G3_MACRT_EV_MAC_SNF_FLAG_MASK                (1<<2)
#define DRV_G3_MACRT_EV_COMM_STATUS_FLAG_MASK            (1<<3)
#define DRV_G3_MACRT_EV_RX_PAR_IND_FLAG_MASK             (1<<4)
#define DRV_G3_MACRT_EV_REG_RSP_MASK                     (1<<5)
#define DRV_G3_MACRT_EV_PHY_SNF_FLAG_MASK                (1<<6)

// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* G3 MACRT Communication Events

  Summary:
    Object used to track communications events from G3 MACRT.

  Description:
    This object is used to keep the data necessary to communicate with G3 MACRT.

  Remarks:
    None.
*/

typedef struct {
    /* HW Timer reference */
    uint32_t timerRef;
    /* Received Data length */
    uint16_t rcvDataLength;
    /* MAC sniffer message length */
    uint16_t macSnifLength;
    /* PHY sniffer message length */
    uint16_t phySnifLength;
    /* Received Register Response length */
    uint16_t regRspLength;
    /* Flag to indicate if TX CONFIRMATION MSG event is enable */
    bool evTxCfm;
    /* Flag to indicate if DATA INDICATION MSG event is enable */
    bool evDataInd;
    /* Flag to indicate if MAC SNIFFER event is enable */
    bool evMacSniffer;
    /* Flag to indicate if COMMUNICATION STATUS event is enable */
    bool evCommStatus;
    /* Flag to indicate if RX PARAMETERS INDICATION event is enable */
    bool evRxParInd;
    /* Flag to indicate if REGISTER DATA RESPONSE event is enable */
    bool evRegRsp;
    /* Flag to indicate if PHY SNIFFER event is enable */
    bool evPhySniffer;
} DRV_G3_MACRT_EVENTS_OBJ;

/* G3 MACRT Internal Memory Map

  Summary:
    Object used to refer to data regions inside G3 MACRT.

  Description:
    This object is used to refer to the data necessary to communicate with 
    G3 MACRT.

  Remarks:
    None.
*/

typedef enum {
	STATUS_INFO_ID = 0,
	SET_COORD_ID,
	TX_REQ_ID,
	TX_CFM_ID,
	DATA_IND_ID,
	MAC_SNIF_ID,
	COMM_STATUS_ID,
	RX_PAR_IND_ID,
	REG_RSP_ID,
	PHY_SNF_ID
} DRV_G3_MACRT_MEM_ID;

/****************************** DRV_PLC Interface ***************************/

void DRV_G3_MACRT_Init(DRV_G3_MACRT_OBJ *g3MacRt);
void DRV_G3_MACRT_Task(void);

#endif //#ifndef _DRV_G3_MACRT_LOCAL_COMM_H
