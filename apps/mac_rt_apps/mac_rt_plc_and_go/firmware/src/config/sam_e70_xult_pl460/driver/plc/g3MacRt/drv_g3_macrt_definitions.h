/*******************************************************************************
  PLC Driver Definitions Header File

  Company:
    Microchip Technology Inc.

  File Name:
    drv_g3_definitions.h

  Summary:
    PLC Driver Definitions Header File

  Description:
    This file provides implementation-specific definitions for the PLC
    driver's system interface.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*
Copyright (C) 2021, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#ifndef DRV_G3_MAC_RT_DEFINITIONS_H
#define DRV_G3_MAC_RT_DEFINITIONS_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <device.h>
#include "driver/plc/common/drv_plc_hal.h"

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

// *****************************************************************************
/* PLC Driver Initialization Data

  Summary:
    Defines the data required to initialize the PLC driver

  Description:
    This data type defines the data required to initialize or the PLC driver.

  Remarks:
    None.
*/

typedef struct
{
    /* Identifies the HAL API set to be used by the driver to access
     * peripherals. */
    DRV_PLC_HAL_INTERFACE           *plcHal;

    /* PLC Profile */
    uint8_t                         plcProfile;

    /* Start Address where PLC binary file is located */
    uint32_t                        binStartAddress;

    /* End Address where PLC binary file is located */
    uint32_t                        binEndAddress;

    /* Secure mode */
    bool                            secure;

} DRV_G3_MACRT_INIT;


//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // #ifndef DRV_G3_MAC_RT_DEFINITIONS_H

/*******************************************************************************
 End of File
*/
