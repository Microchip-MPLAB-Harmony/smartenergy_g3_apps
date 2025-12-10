/*******************************************************************************
  DRV_METROLOGY Driver Local Data Structures

  Company:
    Microchip Technology Inc.

  File Name:
    drv_metrology_local.h

  Summary:
    Metrology Driver Local Data Structures.

  Description:
    The Metrology Library uses local data structures for internal management.
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

#ifndef DRV_METROLOGY_LOCAL_H
#define DRV_METROLOGY_LOCAL_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdbool.h>
#include "driver/driver.h"
#include "system/system.h"
#include "drv_metrology_definitions.h"

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
/* Metrology Driver Instance Object

  Summary:
    Object used to keep any data required for an instance of the metrology driver.

  Description:
    None.

  Remarks:
    None.
*/

typedef struct
{
    /* Flag to indicate this object is in use */
    bool                                          inUse;

    /* The status of the driver */
    volatile DRV_METROLOGY_STATUS                 status;

    /* Flag to indicate that a new IPC interrupt has been triggered */
    volatile bool                                 ipcInterruptFlag;

    /* Flag to indicate that a new integration period has been completed */
    volatile bool                                 integrationFlag;

    /* Size (in Bytes) of the PLC binary file */
    uint32_t                                      binSize;

    /* Address where PLC binary file is located */
    uint32_t                                      binStartAddress;

    /* Metrology Control interface */
    MET_REGISTERS *                               metRegisters;

    /* Metrology Accumulated Output Data */
    DRV_METROLOGY_REGS_ACCUMULATORS               metAccData;

    /* Metrology Harmonic Analysis Output Data */
    DRV_METROLOGY_REGS_HARMONICS                  metHarData;

    /* Metrology Analog Front End Data */
    DRV_METROLOGY_AFE_DATA                        metAFEData;

    /* Metrology Calibration interface */
    DRV_METROLOGY_CALIBRATION                     calibrationData;

    /* Harmonic Analysis Data */
    DRV_METROLOGY_HARMONIC_ANALYSIS               harmonicAnalysisData;

    /* IPC metrology lib integration Callback */
    DRV_METROLOGY_CALLBACK                        integrationCallback;

    /* Calibration Process Callback */
    DRV_METROLOGY_CALIBRATION_CALLBACK            calibrationCallback;

    /* Harmonic Analysis Callback */
    DRV_METROLOGY_HARMONICS_CALLBACK              harmonicAnalysisCallback;

} DRV_METROLOGY_OBJ;

#ifdef __cplusplus
}
#endif

#endif // #ifndef DRV_METROLOGY_LOCAL_H
/*******************************************************************************
 End of File
*/
