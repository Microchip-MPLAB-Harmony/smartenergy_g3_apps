/*******************************************************************************
  PLC Driver Local G3 Data Structures

  Company:
    Microchip Technology Inc.

  File Name:
    drv_plc_phy_local_comm.h

  Summary:
    PLC Driver Local G3 Data Structures

  Description:
    Driver Local G3 Data Structures
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2023 Microchip Technology Inc. and its subsidiaries.
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

#ifndef DRV_PLC_PHY_LOCAL_COMM_H
#define DRV_PLC_PHY_LOCAL_COMM_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "driver/plc/phy/drv_plc_phy_comm.h"

// *****************************************************************************
// *****************************************************************************
// Section: Macro Definitions
// *****************************************************************************
// ***************************************************************************** 

#define PLC_STATUS_LENGTH     8U
#define PLC_DATA_PKT_SIZE     512U
#define PLC_TX_PAR_SIZE       sizeof(DRV_PLC_PHY_TRANSMISSION_OBJ)
#define PLC_RX_PAR_SIZE       sizeof(DRV_PLC_PHY_RECEPTION_OBJ)
#define PLC_CMF_PKT_SIZE      sizeof(DRV_PLC_PHY_TRANSMISSION_CFM_OBJ)
#define PLC_REG_PKT_SIZE      PLC_DATA_PKT_SIZE

/* Number of transmission buffers */
#define NUM_TX_BUFFERS                           1U

/* FLAG MASKs for set events */
#define DRV_PLC_PHY_EV_FLAG_TX_CFM_MASK            0x0001U
#define DRV_PLC_PHY_EV_FLAG_RX_DAT_MASK            0x0002U
#define DRV_PLC_PHY_EV_FLAG_REG_MASK               0x0008U
#define DRV_PLC_PHY_EV_FLAG_RX_PAR_MASK            0x0010U

/* FLAG MASKs for register events */
#define DRV_PLC_PHY_REG_LEN_MASK                   0x1FFU
#define DRV_PLC_PHY_REG_ID_MASK                    0xF000U
#define DRV_PLC_PHY_REG_OFFSET_MASK                0x0FFFU
#define DRV_PLC_PHY_REG_ADC_MASK                   0x1000U
#define DRV_PLC_PHY_REG_DAC_MASK                   0x2000U
#define DRV_PLC_PHY_REG_MASK                       0x4000U
#define DRV_PLC_PHY_FUSES_MASK                     0x8000U
#define DRV_PLC_PHY_REG_ADC_BASE                   0x40000000UL
#define DRV_PLC_PHY_REG_DAC_BASE                   0x40004000UL
#define DRV_PLC_PHY_REG_BASE                       0x80000000UL
#define DRV_PLC_PHY_FUSES_BASE                     0x400E1800UL

// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* PLC Communication Events

  Summary:
    Object used to track communications events from PLC.

  Description:
    This object is used to keep the data necessary to communicate with PLC.

  Remarks:
    None.
*/

typedef struct {
    /* HW Timer reference */
    uint32_t timerRef;
    /* Received Data length */
    uint16_t rcvDataLength;
    /* Received Register Response length */
    uint16_t regRspLength;
    /* Flag to indicate if CONFIRMATION MSG event is enable */
    bool evCfm;
    /* Flag to indicate if DATA INDICATION MSG event is enable */
    bool evRxDat;
    /* Flag to indicate if QPAR INDICATION MSG event is enable */
    bool evRxPar;
    /* Flag to indicate if REGISTER DATA RESPONSE event is enable */
    bool evReg;
} DRV_PLC_PHY_EVENTS_OBJ;

/* PLC Internal Memory Map

  Summary:
    Object used to refer to data regions inside PLC.

  Description:
    This object is used to refer to the data necessary to communicate with 
    PLC.

  Remarks:
    None.
*/

typedef enum {
    STATUS_ID = 0,
    TX_PAR_ID,
    TX_DAT_ID,
    TX_CFM_ID,
    RX_PAR_ID,
    RX_DAT_ID,
    REG_INFO_ID,
    IDS,
} DRV_PLC_PHY_MEM_ID;

/****************************** DRV_PLC_PHY Interface ***************************/

void DRV_PLC_PHY_Init(DRV_PLC_PHY_OBJ *plcPhyObj);
void DRV_PLC_PHY_Task(void);

#endif //#ifndef DRV_PLC_PHY_LOCAL_COMM_H
