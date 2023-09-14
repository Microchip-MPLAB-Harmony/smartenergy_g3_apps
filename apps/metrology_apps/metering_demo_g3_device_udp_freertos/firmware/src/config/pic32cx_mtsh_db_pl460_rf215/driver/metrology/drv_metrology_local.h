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
#include "osal/osal.h"

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

    /* Define a semaphore to attend the Metrology IPC interrupts */
    OSAL_SEM_DECLARE(semaphoreID);

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
