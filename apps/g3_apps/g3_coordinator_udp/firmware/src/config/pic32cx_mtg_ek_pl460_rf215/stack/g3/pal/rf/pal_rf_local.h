/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    pal_rf.h

  Summary:
    RF Platform Abstraction Layer (PAL) Interface Local header file.

  Description:
    RF Platform Abstraction Layer (PAL) Interface Local header file.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*
Copyright (C) 2024, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#ifndef PAL_RF_LOCAL_H
#define PAL_RF_LOCAL_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include "system/system.h"
#include "driver/driver.h"
#include "driver/rf215/drv_rf215_definitions.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

/* PAL RF Data

  Summary:
    Holds PAL RF internal data.

  Description:
    This data type defines the all data required to handle the PAL RF module.

  Remarks:
    None.
*/

typedef struct
{
    DRV_HANDLE drvRfPhyHandle;

    PAL_RF_STATUS status;

    PAL_RF_HANDLERS rfPhyHandlers;

    DRV_RF215_PHY_MOD_SCHEME rfPhyModScheme;

    DRV_RF215_PHY_MOD_SCHEME rfPhyModSchemeFsk;

    DRV_RF215_PHY_MOD_SCHEME rfPhyModSchemeOfdm;

    DRV_RF215_PHY_CFG_OBJ rfPhyConfig;

} PAL_RF_DATA;

#endif // #ifndef PAL_RF_LOCAL_H
/*******************************************************************************
 End of File
*/
