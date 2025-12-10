/*******************************************************************************
  Interface definition of Metrology Driver.

  Company:
    Microchip Technology Inc.

  File Name:
    drv_metrology.h

  Summary:
    Interface definition of the Metrology Driver.

  Description:
    This file defines the interface for the Metrology driver.
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

#ifndef DRV_METROLOGY_H
#define DRV_METROLOGY_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "driver/driver.h"
#include "system/system.h"
#include "device.h"
#include "configuration.h"
#include "drv_metrology_definitions.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

#define MET_CAPTURE_BUF_SIZE     DRV_METROLOGY_CAPTURE_BUF_SIZE

// *****************************************************************************
/* Metrology Driver Result

  Summary:
    Result of a Metrology driver interface operation.

  Description:
    Identifies the result of certain Metrology driver operations.
*/

typedef enum {
    DRV_METROLOGY_SUCCESS,
    DRV_METROLOGY_ERROR
} DRV_METROLOGY_RESULT;

// *****************************************************************************
/* Metrology Driver Start mode

  Summary:
    Describes how the startup process is.

  Description:
    HARD mode implies a full reload of Core 1 application. SOFT mode does not
    any effects on Core 1 application.
*/
typedef enum {
    DRV_METROLOGY_START_SOFT,
    DRV_METROLOGY_START_HARD
} DRV_METROLOGY_START_MODE;

// *****************************************************************************
/* Metrology Driver RMS sign

  Summary:
    Describes the sign of some metrology measurements.

  Description:
    This quality only affects to some RMS measurements.
*/
typedef enum {
    RMS_SIGN_POSITIVE = 0,
    RMS_SIGN_NEGATIVE = 1,
} DRV_METROLOGY_RMS_SIGN;

// *****************************************************************************
/* Metrology Callback Function Pointer

  Summary:
    Defines the data type and function signature for the Metrology driver
    callback function.

  Description:
    The Metrology driver will call back the client's function with this signature
    when the IPC interrupt event has occurred.

  Remarks:
    None.
*/
typedef void (* DRV_METROLOGY_CALLBACK)(void);

// *****************************************************************************
/* Metrology Calibration Callback Function Pointer

  Summary:
    Defines the data type and function signature for the callback function of the
    calibration procedure.

  Description:
    The Metrology driver will call back the client's function with this signature
    when the Calibration procedure has been completed.

  Parameters:
    result  - Flag o indicate if at least one phase has been calibrated successfully.
    Otherwise none of the phases have been calibrated.

  Remarks:
    None.
*/
typedef void (* DRV_METROLOGY_CALIBRATION_CALLBACK) (bool result);

// *****************************************************************************
/* Metrology Harmonics Callback Function Pointer

  Summary:
    Defines the data type and function signature for the callback function of the
    harmonics analysis.

  Description:
    The Metrology driver will call back the client's function with this signature
    when the Harmonics analysis has been completed.

  Parameters:
    harmonicNum  - The number of the harmonic that has been analyzed. If 0, all
                    harmonics from 0 to 31 have been processed.

  Remarks:
    None.
*/
typedef void (* DRV_METROLOGY_HARMONICS_CALLBACK) (uint8_t harmonicNum);

// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: Driver System Interface Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ DRV_METROLOGY_Initialize (
        SYS_MODULE_INIT * init,
        uint32_t resetCause
    );

  Summary:
    Initializes the metrology driver according to the init parameter and the cause of the reset of the main processor.

  Description:
    This routine initializes the metrology driver making it ready for clients to open and use.
    The initialization data is specified by the init parameter.
    It is a single instance driver.

  Precondition:
    The low-level processor and board initialization must be completed before the
    system can call the initialization functions for any modules.

  Parameters:
    init  - Pointer to the init data structure containing any data necessary to initialize the driver.
    resetCause - Reset cause of the main processor.

  Returns:
    If successful, returns a valid handle to a driver instance object.
    Otherwise, it returns SYS_MODULE_OBJ_INVALID.

  Example:
    <code>
    SYS_MODULE_OBJ   sysObjDrvMet;

    extern uint8_t met_bin_start;
    extern uint8_t met_bin_end;

    DRV_METROLOGY_INIT drvMetrologyInitData = {
        .regBaseAddress = DRV_METROLOGY_REG_BASE_ADDRESS,
        .binStartAddress = (uint32_t)&met_bin_start,
        .binEndAddress = (uint32_t)&met_bin_end,

    };

    sysObjDrvMet = DRV_METROLOGY_Initialize((SYS_MODULE_INIT *)&drvMetrologyInitData, RSTC_ResetCauseGet());
    </code>

  Remarks:
    This routine must be called before any other DRV_METROLOGY routine is called.
*/
SYS_MODULE_OBJ DRV_METROLOGY_Initialize(SYS_MODULE_INIT * init, uint32_t resetCause);

// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ DRV_METROLOGY_Reinitialize (
        SYS_MODULE_INIT * init,
    );

  Summary:
    Reinitializes the metrology driver according to the init parameter.

  Description:
    This routine reinitializes the metrology driver making it ready for clients to open and use.
    The initialization data is specified by the init parameter.
    It is a single instance driver.

  Precondition:
    The low-level board initialization must have been completed and the
    module's initialization function must have been called before the system.

  Parameters:
    init  - Pointer to the init data structure containing any data necessary to initialize the driver.

  Returns:
    If successful, returns a valid handle to a driver instance object.
    Otherwise, it returns SYS_MODULE_OBJ_INVALID.

  Example:
    <code>
    SYS_MODULE_OBJ   sysObjDrvMet;

    extern uint8_t met_bin_start;
    extern uint8_t met_bin_end;

    DRV_METROLOGY_INIT drvMetrologyInitData = {
        .regBaseAddress = DRV_METROLOGY_REG_BASE_ADDRESS,
        .binStartAddress = (uint32_t)&met_bin_start,
        .binEndAddress = (uint32_t)&met_bin_end,

    };

    sysObjDrvMet = DRV_METROLOGY_Reinitialize((SYS_MODULE_INIT *)&drvMetrologyInitData);
    </code>

  Remarks:
    DRV_METROLOGY_Initialize routine has to be called at some point before this routine is called.
*/
SYS_MODULE_OBJ DRV_METROLOGY_Reinitialize (SYS_MODULE_INIT * init);

// *****************************************************************************
/* Function:
    DRV_METROLOGY_RESULT DRV_METROLOGY_Open (
        DRV_METROLOGY_START_MODE mode,
        DRV_METROLOGY_CONTROL * pConfiguration
    );

  Summary:
    Opens the metrology driver according to the mode parameter.

  Description:
    This routine enables the IPC peripheral and, only if a HARD start mode has been selected,
    also handles the reset and clock lines for enabling the metrology library application.
    On the other hand, SOFT mode does not any effects on metrology library application running in the second processor.

  Precondition:
    None.

  Parameters:
    mode - Metrology Driver Start mode
    pConfiguration - Pointer to CONTROL configuration. In case of NULL, the MCC default configuration will be loaded.

  Returns:
    If successful, returns DRV_METROLOGY_SUCCESS. Otherwise, it returns DRV_METROLOGY_ERROR.

  Example:
    <code>
        if (RSTC_ResetCauseGet() == RSTC_SR_RSTTYP(RSTC_SR_RSTTYP_WDT0_RST_Val))
        {
            app_metrologyData.startMode = DRV_METROLOGY_START_SOFT;
        }
        else
        {
            app_metrologyData.startMode = DRV_METROLOGY_START_HARD;
        }

        ...


        case APP_METROLOGY_STATE_INIT:
        {
            if (DRV_METROLOGY_Open(app_metrologyData.startMode) == DRV_METROLOGY_SUCCESS)
            {
                if (DRV_METROLOGY_Open(app_metrologyData.startMode,
                    NULL) == DRV_METROLOGY_SUCCESS)
                {
                    app_metrologyData.state = APP_METROLOGY_STATE_START;
                }
                else
                {
                    app_metrologyData.state = APP_METROLOGY_STATE_RUNNING;
                }
            }
            else
            {
                app_metrologyData.state = APP_METROLOGY_STATE_ERROR;
            }

            break;
        }
    </code>

  Remarks:
    None.
*/
DRV_METROLOGY_RESULT DRV_METROLOGY_Open(DRV_METROLOGY_START_MODE mode, DRV_METROLOGY_REGS_CONTROL * pConfiguration);

// *****************************************************************************
/* Function:
    DRV_METROLOGY_RESULT DRV_METROLOGY_Close (void);

  Summary:
    Closes the metrology driver.

  Description:
    This routine closes the metrology driver making it unusable.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    If successful, returns DRV_METROLOGY_SUCCESS. Otherwise, it returns DRV_METROLOGY_ERROR.

  Example:
    <code>
        app_metrologyData.state = APP_METROLOGY_STATE_INIT;
        app_metrologyData.startMode = DRV_METROLOGY_START_HARD;

        DRV_METROLOGY_Close();
        DRV_METROLOGY_Initialize(NULL, RSTC_SR_RSTTYP(RSTC_SR_RSTTYP_SOFT_RST_Val));
    </code>

  Remarks:
    None.
*/
DRV_METROLOGY_RESULT DRV_METROLOGY_Close (void);

// *****************************************************************************
/* Function:
    DRV_METROLOGY_RESULT DRV_METROLOGY_Start (void);

  Summary:
    Starts the metrology driver.

  Description:
    This routine shoud be called until the metrology library application running in the second processor is ready to be used.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    Returns DRV_METROLOGY_SUCCESS when the metrology library application is ready to be used. Otherwise, it returns DRV_METROLOGY_ERROR.

  Example:
    <code>
        case APP_METROLOGY_STATE_START:
        {
            if (DRV_METROLOGY_Start() == DRV_METROLOGY_SUCCESS)
            {
                app_metrologyData.state = APP_METROLOGY_STATE_RUNNING;
            }
            else
            {
                app_metrologyData.state = APP_METROLOGY_STATE_ERROR;
            }

            break;
        }
    </code>

  Remarks:
    None.
*/
DRV_METROLOGY_RESULT DRV_METROLOGY_Start(void);

// *****************************************************************************
/* Function:
    DRV_METROLOGY_RESULT DRV_METROLOGY_IntegrationCallbackRegister (
        DRV_METROLOGY_CALLBACK callback
    );

  Summary:
    Allows a client to set a metrology integration event handling function for the driver to call back when a integration period has completed.

  Description:
    This function allows a client to register an integration period event handling function with the driver to call back when a integration period has completed.
    This function must be always called after DRV_METROLOGY_Initialize routine is called, since the initialization routine sets a NULL pointer to indicate no callback.

  Precondition:
    DRV_METROLOGY_Initialize routine must have been called before.

  Parameters:
    callback - Pointer to the callback function.

  Returns:
    If successful, returns DRV_METROLOGY_SUCCESS. Otherwise, it returns DRV_METROLOGY_ERROR.

  Example:
    <code>
        static void lAPP_METROLOGY_NewIntegrationCallback(void)
        {
            if (app_metrologyData.state == APP_METROLOGY_STATE_RUNNING)
            {
                app_metrologyData.integrationFlag = true;
            }
        }

        (...)

        DRV_METROLOGY_IntegrationCallbackRegister(lAPP_METROLOGY_NewIntegrationCallback);
    </code>

  Remarks:
    None.
*/
DRV_METROLOGY_RESULT DRV_METROLOGY_IntegrationCallbackRegister(DRV_METROLOGY_CALLBACK callback);

// *****************************************************************************
/* Function:
    DRV_METROLOGY_RESULT DRV_METROLOGY_CalibrationCallbackRegister (
        DRV_METROLOGY_CALIBRATION_CALLBACK callback
    );

  Summary:
    Registers a function with the metrology driver to be called back when the calibration process has completed.

  Description:
    This function allows a client to register a handling function with the driver to call back when a calibration process has completed.
    This function must be always called after DRV_METROLOGY_Initialize routine is called, since the initialization routine sets a NULL pointer to indicate no callback.

  Precondition:
    DRV_METROLOGY_Initialize routine must have been called before.

  Parameters:
    callback - Pointer to the function to be called.

  Returns:
    If successful, returns DRV_METROLOGY_SUCCESS. Otherwise, it returns DRV_METROLOGY_ERROR.

  Example:
    <code>
        static void lAPP_METROLOGY_CalibrationCallback(bool result)
        {
            if (app_metrologyData.pCalibrationCallback)
            {
                app_metrologyData.pCalibrationCallback(result);
            }

            OSAL_SEM_Post(&appMetrologyCalibrationSemID);
        }

        (...)

        DRV_METROLOGY_CalibrationCallbackRegister(lAPP_METROLOGY_CalibrationCallback);
    </code>

  Remarks:
    None.
*/
DRV_METROLOGY_RESULT DRV_METROLOGY_CalibrationCallbackRegister(DRV_METROLOGY_CALIBRATION_CALLBACK callback);

// *****************************************************************************
/* Function:
    DRV_METROLOGY_RESULT DRV_METROLOGY_HarmonicAnalysisCallbackRegister (
        DRV_METROLOGY_HARMONICS_CALLBACK callback
    );

  Summary:
    Registers a function with the metrology driver to be called back when the harmonic analysis has completed.

  Description:
    This function allows a client to register a handling function with the driver to call back when the harmonic analysis has completed.
    This function must be always called after DRV_METROLOGY_Initialize routine is called, since the initialization routine sets a NULL pointer to indicate no callback.

  Precondition:
    DRV_METROLOGY_Initialize routine must have been called before.

  Parameters:
    callback - Pointer to the function to be called.

  Returns:
    If successful, returns DRV_METROLOGY_SUCCESS. Otherwise, it returns DRV_METROLOGY_ERROR.

  Example:
    <code>
        static void lAPP_METROLOGY_HarmonicAnalysisCallback(uint8_t harmonicNum)
        {
            if (app_metrologyData.pHarmonicAnalysisCallback)
            {
                app_metrologyData.harmonicAnalysisPending = false;
                app_metrologyData.pHarmonicAnalysisCallback(harmonicNum);
            }
        }

        (...)

        DRV_METROLOGY_HarmonicAnalysisCallbackRegister(lAPP_METROLOGY_HarmonicAnalysisCallback);
    </code>

  Remarks:
    None.
*/
DRV_METROLOGY_RESULT DRV_METROLOGY_HarmonicAnalysisCallbackRegister(DRV_METROLOGY_HARMONICS_CALLBACK callback);

// *****************************************************************************
/* Function:
    DRV_METROLOGY_STATUS DRV_METROLOGY_GetStatus(void);

  Summary:
    Get the status of the metrology driver.

  Description:
    Metrology function status:
        - DRV_METROLOGY_STATUS_UNINITIALIZED: Metrology driver has not been initialized.
        - DRV_METROLOGY_STATUS_READY: Metrology driver is ready to be used.
        - DRV_METROLOGY_STATUS_HALT: Metrology driver has been initialized but not opened.
        - DRV_METROLOGY_STATUS_WAITING_IPC: Metrology driver is waiting the init IPC interrupt
        from the metrology lib as part of the opening routine.
        - DRV_METROLOGY_STATUS_INIT_DSP: IPC interrupt has been triggered indicating that DSP
        filters has been stabilized to full accuracy.
        - DRV_METROLOGY_STATUS_RUNNING: Metrology library is running and periodic data
        acquisition is performed.

    These status values are closely related to the metrology library states.
    For further information about the metrology library state diagram, refer to online documentation.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    Returns the status of the metrology driver.

  Example:
    <code>
        case APP_METROLOGY_STATE_START:
        {
            if (DRV_METROLOGY_GetStatus() == DRV_METROLOGY_STATUS_READY)
            {
                if (DRV_METROLOGY_Start() == DRV_METROLOGY_SUCCESS)
                {
                    app_metrologyData.state = APP_METROLOGY_STATE_RUNNING;
                }
                else
                {
                    app_metrologyData.state = APP_METROLOGY_STATE_ERROR;
                }
            }

            break;
        }
    </code>

  Remarks:
    None.
*/
DRV_METROLOGY_STATUS DRV_METROLOGY_GetStatus(void);

/* Function:
    SYS_MODULE_OBJ DRV_METROLOGY_Tasks (
        SYS_MODULE_OBJ object
    );

  Summary:
    Routine that performs the tasks necessary to maintain a state machine in
    the metrology driver.

  Description:
    Routine that performs the tasks necessary to maintain a state machine in
    the metrology driver.

  Precondition:
    The low-level board initialization must have been completed and
    the module's initialization function must have been called before
    the system can call the tasks routine for any module.

  Parameters:
    object - Handle to the module instance

  Returns:
    None.

  Example:
    <code>

    DRV_METROLOGY_Tasks(sysObj.drvMet);
    </code>

  Remarks:
    None.
*/
void DRV_METROLOGY_Tasks(SYS_MODULE_OBJ object);

// *****************************************************************************
/* Function:
    DRV_METROLOGY_REGS_CONTROL * DRV_METROLOGY_GetControlData (void);

  Summary:
    Get the pointer to the control registers of the metrology library application running on the second processor.

  Description:
    Control registers are acting as 32-bit metrology input control registers.
    For a detailed description of the control registers, refer to "drv_metrology_regs.h" header file.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    Pointer to the 32-bit metrology input control registers..

  Example:
    <code>
        app_metrologyData.pMetControl = DRV_METROLOGY_GetControlData();
        app_metrologyData.pMetStatus = DRV_METROLOGY_GetStatusData();
        app_metrologyData.pMetAccData = DRV_METROLOGY_GetAccData();
        app_metrologyData.pMetHarData = DRV_METROLOGY_GetHarData();
    </code>

  Remarks:
    None.
*/
DRV_METROLOGY_REGS_CONTROL * DRV_METROLOGY_GetControlData(void);

// *****************************************************************************
/* Function:
    DRV_METROLOGY_CONTROL * DRV_METROLOGY_GetControlByDefault (void);

  Summary:
    Get the pointer to the control registers defined by default via the Microchip Code Configurator (MCC) included in Harmony 3 framework.

  Description:
    After generating the code application via MCC, a struct data is defined as a constant. It includes all control registers obtained according to the MCC configuration established in the Harmony 3 project.
    The content of each control register is defined in "configuration.h" header file.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    Pointer to the control registers defined as the default configuration.

  Example:
    <code>
      const DRV_METROLOGY_CONTROL gDrvMetControlDefault =
        {
            STATE_CTRL_STATE_CTRL_RESET_Val,
            (uint32_t)(DRV_METROLOGY_CONF_FCTRL),
            (uint32_t)(DRV_METROLOGY_CONF_HARMONIC_CTRL),
            (uint32_t)(DRV_METROLOGY_CONF_MT),
            (uint32_t)(0x00000000),
            (uint32_t)(0x00001130),
            (uint32_t)(DRV_METROLOGY_CONF_PULSE0_CTRL),
            (uint32_t)(DRV_METROLOGY_CONF_PULSE1_CTRL),
            (uint32_t)(DRV_METROLOGY_CONF_PULSE2_CTRL),
            (uint32_t)(DRV_METROLOGY_CONF_PKT),
            (uint32_t)(DRV_METROLOGY_CONF_PKT),
            (uint32_t)(DRV_METROLOGY_CONF_PKT),
            (uint32_t)(DRV_METROLOGY_CONF_CREEP_P),
            (uint32_t)(DRV_METROLOGY_CONF_CREEP_Q),
            (uint32_t)(DRV_METROLOGY_CONF_CREEP_I),
            (uint32_t)(0x00000000),
            (uint32_t)(0x00000000),
            (uint32_t)(0x00000000),
            (uint32_t)(DRV_METROLOGY_CONF_SWELL),
            (uint32_t)(DRV_METROLOGY_CONF_SWELL),
            (uint32_t)(DRV_METROLOGY_CONF_SWELL),
            (uint32_t)(DRV_METROLOGY_CONF_SAG),
            (uint32_t)(DRV_METROLOGY_CONF_SAG),
            (uint32_t)(DRV_METROLOGY_CONF_SAG),
            (uint32_t)(DRV_METROLOGY_CONF_KI),
            (uint32_t)(DRV_METROLOGY_CONF_KV),
            (uint32_t)(DRV_METROLOGY_CONF_KI),
            (uint32_t)(DRV_METROLOGY_CONF_KV),
            (uint32_t)(DRV_METROLOGY_CONF_KI),
            (uint32_t)(DRV_METROLOGY_CONF_KV),
            (uint32_t)(DRV_METROLOGY_CONF_KI),
            (uint32_t)(0x20000000),
            (uint32_t)(0x20000000),
            (uint32_t)(0x20000000),
            (uint32_t)(0x20000000),
            (uint32_t)(0x20000000),
            (uint32_t)(0x20000000),
            (uint32_t)(0x20000000),
            (uint32_t)(0x00000000),
            (uint32_t)(0x00000000),
            (uint32_t)(0x00000000),
            (uint32_t)(0x00000000),
            (uint32_t)(0x00000000),
            (uint32_t)(0x00000000),
            (uint32_t)(0x00000000),
            (uint32_t)(DRV_METROLOGY_CONF_WAVEFORM),
            (uint32_t)(DRV_METROLOGY_CAPTURE_BUF_SIZE),
            (uint32_t)(0x00000000),
            (uint32_t)(0x00000000),
            (uint32_t)(0x00000000),
            (uint32_t)(DRV_METROLOGY_CONF_ATS2023),
            (uint32_t)(DRV_METROLOGY_CONF_ATS2427),
            (uint32_t)(0x00000003),
            (uint32_t)(0x00000000),
            (uint32_t)(0x00000000),
            (uint32_t)(0x00000000),
            (uint32_t)(0x00000000),
            (uint32_t)(0x00000000),
            (uint32_t)(0x00000000),
            (uint32_t)(0x00000000)
        };

        (...)

        DRV_METROLOGY_CONTROL *pSrc;

        pSrc = DRV_METROLOGY_GetControlByDefault();

    </code>

  Remarks:
    None.
*/
DRV_METROLOGY_REGS_CONTROL * DRV_METROLOGY_GetControlByDefault(void);

// *****************************************************************************
/* Function:
    DRV_METROLOGY_REGS_STATUS * DRV_METROLOGY_GetStatusData (void);

  Summary:
    Get the pointer to the status registers of the metrology library application running on the second processor.

  Description:
    The Metrology library generates primary 32-bit output measurement quantities for each measurement interval.
    For a detailed description of the status registers, refer to "drv_metrology_regs.h" header file.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    Pointer to the 32-bit metrology output measurement quantities for each measurement interval.

  Example:
    <code>
        app_metrologyData.pMetControl = DRV_METROLOGY_GetControlData();
        app_metrologyData.pMetStatus = DRV_METROLOGY_GetStatusData();
        app_metrologyData.pMetAccData = DRV_METROLOGY_GetAccData();
        app_metrologyData.pMetHarData = DRV_METROLOGY_GetHarData();
    </code>

  Remarks:
    None.
*/
DRV_METROLOGY_REGS_STATUS * DRV_METROLOGY_GetStatusData(void);

// *****************************************************************************
/* Function:
    DRV_METROLOGY_ACCUMULATORS * DRV_METROLOGY_GetAccData (void);

  Summary:
    Get the pointer to the accumulator registers of the metrology library application running on the second processor.

  Description:
    Accumulator registers are acting as 64-bit metrology output accumulator registers.
    All values are integrated at an equivalent sampling rate of 4.000KHz. The Metrology library shall
    generate the following primary output measurement quantities for each measurement interval, per-phase quantities
    with each phase designated by the subscript '_x' (where x = [A, B, or C] for voltage channels and x = [A, B, C or N]
    for current channels)
    For a detailed description of the control registers, refer to "drv_metrology_regs.h" header file.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    Pointer to the 32-bit metrology input control registers.

  Example:
    <code>
        app_metrologyData.pMetControl = DRV_METROLOGY_GetControlData();
        app_metrologyData.pMetStatus = DRV_METROLOGY_GetStatusData();
        app_metrologyData.pMetAccData = DRV_METROLOGY_GetAccData();
        app_metrologyData.pMetHarData = DRV_METROLOGY_GetHarData();
    </code>

  Remarks:
    None.
*/
DRV_METROLOGY_REGS_ACCUMULATORS * DRV_METROLOGY_GetAccData(void);

// *****************************************************************************
/* Function:
    DRV_METROLOGY_HARMONICS * DRV_METROLOGY_GetHarData (void);

  Summary:
    Get the pointer to the harmonic analysis registers of the metrology library application running on the second processor.

  Description:
    Harmonic analysis registers are acting as 32-bit metrology harmonic analysis registers.
    The Metrology module shall generate the following primary output measurement quantities for each measurement interval,
    per-phase quantities with each phase designated by the subscript '_x' (where x = [A, B, or C] for voltage channels
    and x = [A, B, C] for current channels).
    Internal scaling allows accurate harmonic analysis for integration periods of up to 16 seconds for up to full-scale magnitude input waveforms.
    For a detailed description of the harmonic analysis registers, refer to "drv_metrology_regs.h" header file.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    Pointer to the 32-bit metrology harmonic analysis registers.

  Example:
    <code>
        app_metrologyData.pMetControl = DRV_METROLOGY_GetControlData();
        app_metrologyData.pMetStatus = DRV_METROLOGY_GetStatusData();
        app_metrologyData.pMetAccData = DRV_METROLOGY_GetAccData();
        app_metrologyData.pMetHarData = DRV_METROLOGY_GetHarData();
    </code>

  Remarks:
    None.
*/
DRV_METROLOGY_REGS_HARMONICS * DRV_METROLOGY_GetHarData(void);

// *****************************************************************************
/* Function:
    void DRV_METROLOGY_SetControl (DRV_METROLOGY_REGS_CONTROL * pControl);

  Summary:
    Establishes the content of all control registers at once.

  Description:
    Overwrite the value of the every control register in the shared memory at once.

  Precondition:
    None.

  Parameters:
    pControl - Pointer to data of the new control registers to overwrite the old ones.

  Returns:
    None.

  Example:
    <code>
        void APP_METROLOGY_SetControlByDefault(void)
       {
           DRV_METROLOGY_REGS_CONTROL *pSrc;

           pSrc = DRV_METROLOGY_GetControlByDefault();
           DRV_METROLOGY_SetControl(pSrc);
       }
    </code>

  Remarks:
    None.
*/
void DRV_METROLOGY_SetControl(DRV_METROLOGY_REGS_CONTROL * pControl);

// *****************************************************************************
/* Function:
    uint32_t DRV_METROLOGY_GetEnergyValue(bool restartEnergy);

  Summary:
    Gets the active energy value.

  Description:
    Gets the active energy value. Energy value should be previously updated by the DRV_METROLOGY_UpdateMeasurements() routine.

  Precondition:
    None.

  Parameters:
    restartEnergy - Flag to indicate if the energy value should be restarted or accumulated to the previous calculated value.

  Returns:
    The active energy value obtained according to the last call to DRV_METROLOGY_UpdateMeasurements() routine..

  Example:
    <code>
        DRV_METROLOGY_UpdateMeasurements();

        app_metrologyData.queueFree = uxQueueSpacesAvailable(appEnergyQueueID);
        if (app_metrologyData.queueFree)
        {
            newMetrologyData.energy = DRV_METROLOGY_GetEnergyValue(true);
            newMetrologyData.Pt = DRV_METROLOGY_GetRMSValue(RMS_PT);
            xQueueSend(appEnergyQueueID, &newMetrologyData, (TickType_t) 0);
        }
        else
        {
            SYS_CMD_MESSAGE("ENERGY Queue is FULL!!!\n\r");
        }
    </code>

  Remarks:
    None.
*/
uint32_t DRV_METROLOGY_GetEnergyValue(bool restartEnergy);

// *****************************************************************************
/* Function:
    uint32_t DRV_METROLOGY_GetRMSValue(DRV_METROLOGY_RMS_TYPE type);

  Summary:
    Gets the last RMS value of the selected measurement type.

  Description:
    RMS values should be previously updated by the DRV_METROLOGY_UpdateMeasurements() routine.
    For further information about RMS types, refer to DRV_METROLOGY_RMS_TYPE definition.

  Precondition:
    None.

  Parameters:
    type - Indicate what type of measurement is obtained.

  Returns:
    The RMS value of the selected type.

  Example:
    <code>
        DRV_METROLOGY_UpdateMeasurements();

        app_metrologyData.queueFree = uxQueueSpacesAvailable(appEnergyQueueID);
        if (app_metrologyData.queueFree)
        {
            newMetrologyData.energy = DRV_METROLOGY_GetEnergyValue(true);
            newMetrologyData.Pt = DRV_METROLOGY_GetRMSValue(RMS_PT);
            xQueueSend(appEnergyQueueID, &newMetrologyData, (TickType_t) 0);
        }
        else
        {
            SYS_CMD_MESSAGE("ENERGY Queue is FULL!!!\n\r");
        }
    </code>

  Remarks:
    None.
*/
uint32_t DRV_METROLOGY_GetRMSValue(DRV_METROLOGY_RMS_TYPE type);

// *****************************************************************************
/* Function:
    DRV_METROLOGY_RMS_SIGN DRV_METROLOGY_GetRMSSign(DRV_METROLOGY_RMS_TYPE type);

  Summary:
    Gets the sign of the last RMS value of the selected measurement type.

  Description:
    RMS sign values should be previously updated by the DRV_METROLOGY_UpdateMeasurements() routine.
    For further information about RMS types, refer to DRV_METROLOGY_RMS_TYPE definition.

  Precondition:
    None.

  Parameters:
    type - Indicate what type of measurement is obtained.

  Returns:
    The RMS sign of the last RMS value. Positive sign is identified as RMS_SIGN_POSITIVE (0), negative sign as RMS_SIGN_NEGATIVE (1).

  Example:
    <code>

    </code>

  Remarks:
    None.
*/
DRV_METROLOGY_RMS_SIGN DRV_METROLOGY_GetRMSSign(DRV_METROLOGY_RMS_TYPE type);

// *****************************************************************************
/* Function:
    void DRV_METROLOGY_SetConfiguration(DRV_METROLOGY_CONFIGURATION * config);

  Summary:
    Set metrology configuration.

  Description:
    This routine is used to configure the metrology library according to the hardware
    in the input networks, as well as the programmable gain of the internal amplifiers.
    This function has to be called before the DRV_METROLOGY_StartCalibration() routine,
    in order to ensure that the proper configuration has been applied before calibrating the system.
    If the programmable gain of the internal amplifiers changes, then the metrology library must be
    reinitialized in order to apply the changes.
    The metrology driver code receives the parameters and writes the proper metrology control registers.

  Precondition:
    None.

  Parameters:
    config - Pointer to configuration data to be used by the metrology library.

  Returns:
    None.

  Example:
    <code>
        DRV_METROLOGY_CONFIGURATION newConf;

        newConf.mc = 800;
        newConf.st = SENSOR_CT;
        newConf.freq = 50;
        newConf.gain = GAIN_1;
        newConf.tr = 1000;
        newConf.rl = 3.24;
        newConf.ku = 1651;

        DRV_METROLOGY_SetConfiguration(&newConf);
    </code>

  Remarks:
    None.
*/
void DRV_METROLOGY_SetConfiguration(DRV_METROLOGY_CONFIGURATION * config);

// *****************************************************************************
/* Function:
    void DRV_METROLOGY_GetEventsData(DRV_METROLOGY_AFE_EVENTS * events);

  Summary:
    Gets the last metrology AFE events data.

  Description:
    Events data should be previously updated by the DRV_METROLOGY_UpdateMeasurements() routine.
    For further information about the event data, refer to DRV_METROLOGY_AFE_EVENTS definition.

  Precondition:
    None.

  Parameters:
    events - Pointer to the data where the events related information will be written.

  Returns:
    None.

  Example:
    <code>
        DRV_METROLOGY_UpdateMeasurements();

        app_metrologyData.queueFree = uxQueueSpacesAvailable(appEventsQueueID);
        if (app_metrologyData.queueFree)
        {
            RTC_TimeGet(&newEvent.eventTime);
            DRV_METROLOGY_GetEventsData(&newEvent.eventFlags);
            xQueueSend(appEventsQueueID, &newEvent, (TickType_t) 0);
        }
        else
        {
            SYS_CMD_MESSAGE("EVENTS Queue is FULL!!!\n\r");
        }
    </code>

  Remarks:
    None.
*/
void DRV_METROLOGY_GetEventsData(DRV_METROLOGY_AFE_EVENTS * events);

// *****************************************************************************
/* Function:
    DRV_METROLOGY_CALIBRATION_REFS * DRV_METROLOGY_GetCalibrationReferences (void);

  Summary:
    Get the pointer to the calibration references used in the internal
    calibration process of the metrology library.

  Description:
    These internal calibration values are used to obtain the RMS values correctly.
    They also includes the result of the calibration process to be checked by the main application.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    Pointer to the internal calibration data.

  Example:
    <code>
        void APP_METROLOGY_StartCalibration(APP_METROLOGY_CALIBRATION * calibration)
        {
            DRV_METROLOGY_CALIBRATION_REFS * pCalibrationRefs;

            pCalibrationRefs = DRV_METROLOGY_GetCalibrationReferences();
            pCalibrationRefs->aimIA = calibration->aimIA;
            pCalibrationRefs->aimVA = calibration->aimVA;
            pCalibrationRefs->angleA = calibration->angleA;
            pCalibrationRefs->aimIB = calibration->aimIB;
            pCalibrationRefs->aimVB = calibration->aimVB;
            pCalibrationRefs->angleB = calibration->angleB;
            pCalibrationRefs->aimIC = calibration->aimIC;
            pCalibrationRefs->aimVC = calibration->aimVC;
            pCalibrationRefs->angleC = calibration->angleC;
            pCalibrationRefs->lineId = calibration->lineId;

            app_metrologyData.state = APP_METROLOGY_STATE_CHECK_CALIBRATION;
            DRV_METROLOGY_StartCalibration();
        }
    </code>

  Remarks:
    None.
*/
DRV_METROLOGY_CALIBRATION_REFS * DRV_METROLOGY_GetCalibrationReferences(void);

// *****************************************************************************
/* Function:
    void APP_METROLOGY_StartCalibration(APP_METROLOGY_CALIBRATION * calibration);

  Summary:
    Starts internal calibration process.

  Description:
    This routine is used to automatically calibrate the board.
    It requires accurate and stable voltage and current sources and loads, which can be
    provided by a meter test bench like the WECO (or equivalent equipment).
    This routine must be called after a configuration routine (APP_METROLOGY_SetConfiguration),
    in order to ensure that the proper configuration has been applied.

  Precondition:
    None.

  Parameters:
    calibration - Pointer to calibration data to be used by the metrology library.

  Returns:
    None.

  Example:
    <code>
        APP_METROLOGY_CALIBRATION newCal;

        newCal.aimVA = 220.00;
        newCal.aimIA = 5.000;
        newCal.angleA = 60.00;

        APP_METROLOGY_StartCalibration(&newCal);
    </code>

  Remarks:
    None.
*/
void DRV_METROLOGY_StartCalibration(void);

// *****************************************************************************
/* Function:
    void DRV_METROLOGY_StartHarmonicAnalysis(
        uint8_t harmonicNum,
        DRV_METROLOGY_HARMONIC *pHarmonicResponse
    );

  Summary:
    Starts the harmonic Analysis process.

  Description:
    This routine configures the metrology library to enable the harmonics computation
    and to calculate the data corresponding to the harmonic number n.
    Then, the metrology driver receives the data from the metrology library and
    computes the RMS currents and voltages of the selected harmonic.

  Precondition:
    None.

  Parameters:
    harmonicNum - Harmonic number. If 0, all harmonics from 0 to 31 will be processed.
    pHarmonicResponse - Pointer to the harmonic analysis struct data to store the harmonic data result.

  Returns:
    None.

  Example:
    <code>
        DRV_METROLOGY_HARMONIC harmonicAnalysisResponse;

        DRV_METROLOGY_StartHarmonicAnalysis(3, &harmonicAnalysisResponse);
    </code>

  Remarks:
    None.
*/
void DRV_METROLOGY_StartHarmonicAnalysis(uint8_t harmonicNum, DRV_METROLOGY_HARMONICS_RMS *pHarmonicResponse);

#ifdef __cplusplus
 }
#endif

#endif // #ifndef DRV_METROLOGY_H
/*******************************************************************************
 End of File
*/
