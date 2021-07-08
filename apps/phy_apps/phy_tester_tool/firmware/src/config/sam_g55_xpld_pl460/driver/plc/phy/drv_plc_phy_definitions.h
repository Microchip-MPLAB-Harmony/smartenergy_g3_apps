/*******************************************************************************
  PLC Driver Definitions Header File

  Company:
    Microchip Technology Inc.

  File Name:
    drv_plc_phy_definitions.h

  Summary:
    PLC Driver Definitions Header File

  Description:
    This file provides implementation-specific definitions for the PLC
    driver's system interface.
*******************************************************************************/

//DOM-IGNORE-BEGIN
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
//DOM-IGNORE-END

#ifndef DRV_PLC_PHY_DEFINITIONS_H
#define DRV_PLC_PHY_DEFINITIONS_H

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
    DRV_PLC_HAL_INTERFACE          *plcHal;

    /* Number of clients */
    size_t                          numClients;

    /* PLC Profile */
    uint8_t                         plcProfile;

    /* Start Address where PLC binary file is located */
    uint32_t                        binStartAddress;

    /* End Address where PLC binary file is located */
    uint32_t                        binEndAddress;

    /* Secure mode */
    bool                            secure;

} DRV_PLC_PHY_INIT;


//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // #ifndef DRV_PLC_PHY_DEFINITIONS_H

/*******************************************************************************
 End of File
*/
