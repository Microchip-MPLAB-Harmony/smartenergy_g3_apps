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
//DOM-IGNORE-END

#ifndef DRV_G3_MACRT_LOCAL_COMM_H
#define DRV_G3_MACRT_LOCAL_COMM_H


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
#define DRV_G3_MACRT_REG_CMD_RD            0U
#define DRV_G3_MACRT_REG_CMD_WR            1U

#define DRV_G3_MACRT_STATUS_LENGTH         12U
#define DRV_G3_MACRT_DATA_MAX_SIZE         MAC_RT_DATA_MAX_SIZE
#define DRV_G3_MACRT_COMM_STATUS_SIZE      32U
#define DRV_G3_MACRT_TX_CFM_SIZE           sizeof(MAC_RT_TX_CFM_OBJ)
#define DRV_G3_MACRT_RX_PAR_SIZE           sizeof(MAC_RT_RX_PARAMETERS_OBJ)
#define DRV_G3_MACRT_REG_PKT_SIZE          sizeof(MAC_RT_PIB_OBJ)

/* FLAG MASKs for set events */
#define DRV_G3_MACRT_EV_TX_CFM_FLAG_MASK                 (1U << 0)
#define DRV_G3_MACRT_EV_DATA_IND_FLAG_MASK               (1U << 1)
#define DRV_G3_MACRT_EV_MAC_SNF_FLAG_MASK                (1U << 2)
#define DRV_G3_MACRT_EV_COMM_STATUS_FLAG_MASK            (1U << 3)
#define DRV_G3_MACRT_EV_RX_PAR_IND_FLAG_MASK             (1U << 4)
#define DRV_G3_MACRT_EV_REG_RSP_MASK                     (1U << 5)
#define DRV_G3_MACRT_EV_PHY_SNF_FLAG_MASK                (1U << 6)

// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* G3 MAC RT Communication Events

  Summary:
    Object used to track communications events from G3 MAC RT.

  Description:
    This object is used to keep the data necessary to communicate with G3
    MAC RT.

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

// *****************************************************************************
/* G3 MAC RT Internal Memory Map

  Summary:
    Data type used to refer to data regions inside G3 MAC RT.

  Description:
    This data type is used to refer to the data necessary to communicate with
    G3 MAC RT.

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

#endif //#ifndef DRV_G3_MACRT_LOCAL_COMM_H
