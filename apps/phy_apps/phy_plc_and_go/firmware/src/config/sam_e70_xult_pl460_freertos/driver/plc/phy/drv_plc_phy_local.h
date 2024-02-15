/*******************************************************************************
  PLC Driver Local Data Structures

  Company:
    Microchip Technology Inc.

  File Name:
    drv_plc_phy_local.h

  Summary:
    PLC Driver Local Data Structures

  Description:
    Driver Local Data Structures
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

#ifndef DRV_PLC_PHY_LOCAL_H
#define DRV_PLC_PHY_LOCAL_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "system/system.h"
#include "driver/plc/phy/drv_plc_phy.h"
#include "driver/plc/phy/drv_plc_phy_definitions.h"
#include "driver/plc/phy/drv_plc_phy_comm.h"
#include "osal/osal.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* DRV_PLC_PHY Command set

  Summary:
    Enumeration listing the DRV_PLC_PHY commands.

  Description:
    This enumeration defines the commands used to interact with the DRV_PLC_PHY
    series of devices.

  Remarks:
    None
*/

typedef enum
{
    /* Write command */
    DRV_PLC_PHY_CMD_WRITE              = (1 << 10),

    /* Read command */
    DRV_PLC_PHY_CMD_READ               = (0 << 10)

} DRV_PLC_PHY_CMD;

// *****************************************************************************
/* DRV_PLC_PHY Transfer Object State

  Summary:
    Defines the status of the DRV_PLC_PHY Transfer Object.

  Description:
    This enumeration defines the status of the DRV_PLC_PHY Transfer Object.

  Remarks:
    None.
*/

typedef enum
{
    DRV_PLC_PHY_STATE_IDLE,
    DRV_PLC_PHY_STATE_TX,
    DRV_PLC_PHY_STATE_WAITING_TX_CFM,
    DRV_PLC_PHY_STATE_ERROR,
}DRV_PLC_PHY_STATE;

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
    /* Flag to indicate this object is in use  */
    bool                            inUse;

    DRV_PLC_PHY_STATE               state[1];

    /* Keep track of the number of clients that have opened this driver */
    size_t                          nClients;

    /* Maximum number of clients */
    size_t                          nClientsMax;

    /* The status of the driver */
    SYS_STATUS                      status;

    /* HAL API list that will be used by the driver to access the hardware */
    DRV_PLC_HAL_INTERFACE           *plcHal;

    /* PLC Profile */
    uint8_t                         plcProfile;

    /* Size (in Bytes) of the PLC binary file */
    uint32_t                        binSize;

    /* Address where PLC binary file is located */
    uint32_t                        binStartAddress;

    /* Secure mode */
    bool                            secure;

    /* Sleep mode */
    bool                            sleep;

    /* Application Data Confirm Callback */
    DRV_PLC_PHY_TX_CFM_CALLBACK     txCfmCallback;

    /* Application Data Indication Callback */
    DRV_PLC_PHY_DATA_IND_CALLBACK   dataIndCallback;

    /* Application Exception Callback */
    DRV_PLC_PHY_EXCEPTION_CALLBACK  exceptionCallback;

    /* Application Exception Callback */
    DRV_PLC_PHY_SLEEP_CALLBACK      sleepDisableCallback;

    /* Application Bootloader Data Callback */
    DRV_PLC_BOOT_DATA_CALLBACK      bootDataCallback;

    /* Application context for Data Confirm Callback */
    uintptr_t                       contextCfm;

    /* Application context for Data Indication Callback */
    uintptr_t                       contextInd;

    /* Application context for Exception Callback */
    uintptr_t                       contextExc;

    /* Application context for Sleep Mode Callback */
    uintptr_t                       contextSleep;

    /* Application context for Bootloader Dara Callback */
    uintptr_t                       contextBoot;

    /* Semaphore identifier. Used to suspend and resume task */
    OSAL_SEM_DECLARE(semaphoreID);

    /* Event detection flag: confirmation of the previous transmission */
    volatile bool                   evTxCfm[1];

    /* Event detection flag: parameters of new reception */
    volatile bool                   evRxPar;

    /* Event detection flag: data of new reception */
    volatile bool                   evRxDat;

    /* Event detection flag: length of the response with register content */
    volatile uint16_t               evRegRspLength;

    /* Event detection flag: reset waiting tx cfm */
    volatile bool                   evResetTxCfm;

} DRV_PLC_PHY_OBJ;


#endif //#ifndef DRV_PLC_PHY_LOCAL_H
