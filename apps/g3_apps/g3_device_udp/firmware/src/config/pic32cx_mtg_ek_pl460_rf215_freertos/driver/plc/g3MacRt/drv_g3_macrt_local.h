/*******************************************************************************
  G3 MAC RT Driver Local Data Structures

  Company:
    Microchip Technology Inc.

  File Name:
    drv_g3_macrt_local.h

  Summary:
    G3 MAC RT Driver Local Data Structures

  Description:
    G3 MAC Real Time Driver Local Data Structures
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

#ifndef DRV_G3_MACRT_LOCAL_H
#define DRV_G3_MACRT_LOCAL_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "system/system.h"
#include "driver/plc/g3MacRt/drv_g3_macrt.h"
#include "driver/plc/g3MacRt/drv_g3_macrt_definitions.h"
#include "driver/plc/g3MacRt/drv_g3_macrt_comm.h"
#include "driver/plc/common/drv_plc_boot.h"
#include "osal/osal.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* DRV_G3_MACRT Command set

  Summary:
    Enumeration listing the DRV_G3_MACRT commands.

  Description:
    This enumeration defines the commands used to interact with the DRV_G3_MACRT
    series of devices.

  Remarks:
    None
*/

typedef enum
{
    /* Write command */
    DRV_G3_MACRT_CMD_WRITE              = (1 << 10),

    /* Read command */
    DRV_G3_MACRT_CMD_READ               = (0 << 10)

} DRV_G3_MACRT_CMD;

// *****************************************************************************
/* PLC Driver Instance Object

  Summary:
    Object used to keep any data required for an instance of the PLC driver.

  Description:
    None.

  Remarks:
    None.
*/

typedef struct
{
    /* State of the MAC RT driver */
    DRV_G3_MACRT_STATE                        state;

    /* HAL API list that will be used by the driver to access the hardware */
    DRV_PLC_HAL_INTERFACE                     *plcHal;

    /* PLC Profile */
    uint8_t                                   plcProfile;

    /* Size (in Bytes) of the PLC binary file */
    uint32_t                                  binSize;

    /* Address where PLC binary file is located */
    uint32_t                                  binStartAddress;

    /* Secure mode */
    bool                                      secure;

    /* PLC Initialization Confirm Callback */
    DRV_G3_MACRT_INIT_CALLBACK                initCallback;

    /* Application Bootloader Data Callback */
    DRV_PLC_BOOT_DATA_CALLBACK                bootDataCallback;

    /* Application Transmission Confirm Callback */
    DRV_G3_MACRT_TX_CFM_CALLBACK              txCfmCallback;

    /* Application Data Indication Callback */
    DRV_G3_MACRT_DATA_IND_CALLBACK            dataIndCallback;

    /* Application RX Parameters Indication Callback */
    DRV_G3_MACRT_RX_PARAMS_IND_CALLBACK       rxParamsIndCallback;

    /* Application MAC RT Sniffer Indication Callback */
    DRV_G3_MACRT_MAC_SNIFFER_IND_CALLBACK     macSnifferIndCallback;

    /* Application Comm Status Indication Callback */
    DRV_G3_MACRT_COMM_STATUS_IND_CALLBACK     commStatusIndCallback;

    /* Application PLC PHY Sniffer Callback */
    DRV_G3_MACRT_PHY_SNIFFER_IND_CALLBACK     phySnifferIndCallback;

    /* Application Exception Callback */
    DRV_G3_MACRT_EXCEPTION_CALLBACK           exceptionCallback;

    /* Event detection flag: data of new reception */
    volatile uint16_t                         evDataIndLength[2];

    /* Event detection flag: length of the response with register content */
    volatile uint16_t                         evRegRspLength;

    /* Event detection flag: MAC Sniffer */
    volatile uint16_t                         evMacSnifLength;

    /* Event detection flag: PHY Sniffer */
    volatile uint16_t                         evPhySnifLength;

    /* Event detection flag: reset waiting tx cfm */
    volatile bool                             evResetTxCfm;

    /* Event detection flag: tx cfm */
    volatile bool                             evTxCfm;

    /* Event detection flag: Comm Status */
    volatile bool                             evCommStatus;

    /* Event detection flag: RX parameters */
    volatile bool                             evRxParams[2];

    /* Indices to control in which buffer to receive data and parameters */
    uint8_t                                   evDataIndLengthIndex;
    uint8_t                                   evRxParamsIndex;

    /* Pointer to MAC Sniffer Data Buffer */
    uint8_t                                   *pMacDataSniffer;

    /* Pointer to PHY Sniffer Data Buffer */
    uint8_t                                   *pPhyDataSniffer;

    /* Semaphore identifier. Used to suspend and resume task */
    OSAL_SEM_DECLARE(semaphoreID);

} DRV_G3_MACRT_OBJ;


#endif //#ifndef DRV_G3_MACRT_LOCAL_H
