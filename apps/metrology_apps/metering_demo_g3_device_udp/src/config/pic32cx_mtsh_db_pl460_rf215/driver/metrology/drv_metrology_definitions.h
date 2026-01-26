/*******************************************************************************
  DRV_METROLOGY Driver Interface Definition

  Company:
    Microchip Technology Inc.

  File Name:
    drv_metrology_definitions.h

  Summary:
    Metrology Library Definitions Interface header.

  Description:
    The Metrology Library provides a interface to access the metrology data
    provided by the application running on Core 1.
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

#ifndef DRV_METROLOGY_DEFINITIONS_H
#define DRV_METROLOGY_DEFINITIONS_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdbool.h>
#include "driver/driver.h"
#include "system/system.h"
#include "drv_metrology_regs.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: External Data
// *****************************************************************************
// *****************************************************************************

/* Metrology library Binary file addressing */
extern uint8_t met_bin_start;
extern uint8_t met_bin_end;

// *****************************************************************************
// *****************************************************************************
// Section: Macro Definitions
// *****************************************************************************
// *****************************************************************************
#define DRV_METROLOGY_CHANNELS_NUMBER             5U
#define DRV_METROLOGY_AFE_SELECTION               0U
#define DRV_METROLOGY_HARMONICS_MAX_ORDER         31U

#define DRV_METROLOGY_IPC_INIT_IRQ_MSK            IPC_ISR_IRQ20_Msk
#define DRV_METROLOGY_IPC_INTEGRATION_IRQ_MSK     IPC_ISR_IRQ0_Msk
#define DRV_METROLOGY_IPC_FULLCYCLE_IRQ_MSK       IPC_ISR_IRQ4_Msk
#define DRV_METROLOGY_IPC_HALFCYCLE_IRQ_MSK       IPC_ISR_IRQ5_Msk

#define FORMAT_CONST_sQ031     31U
#define FORMAT_CONST_uQ2012    12U
#define FORMAT_CONST_sQ229     29U
#define FORMAT_CONST_uQ2440    40U
#define FORMAT_CONST_sQ2340    40U
#define FORMAT_CONST_sQ3330    30U
#define FORMAT_CONST_uQ824     24U
#define FORMAT_CONST_uQ230     30U
#define FORMAT_CONST_uQ1220    20U
#define FORMAT_CONST_sQ130     30U
#define FORMAT_CONST_uQ032     32U
#define FORMAT_CONST_uQ2210    10U
#define FORMAT_CONST_sQ940     40U
#define FORMAT_CONST_sQ1318    18U
#define FORMAT_CONST_uQ4420    20U

#define GAIN_P_K_T_Q           24U
#define GAIN_VI_Q              10U
#define DIV_GAIN               1024U /* (1 << GAIN_VI_Q) */
#define CAL_VI_Q               29U
#define CAL_PH_Q               31U
#define SAMPLING_FREQ          4000.0

#define SECS_IN_HOUR_DOUBLE    3600.0

/* Metrology Analog front End (AFE) description

  Summary:
    Describes the connection with AFE devices

  Description:
    The metrology driver has been designed to interface with several AFE devices.
*/
typedef enum {
    AFE_1xATSENSE203 = 0,
    AFE_NUM_TYPE
} DRV_METROLOGY_AFE_TYPE;

/* Metrology Driver Sensor Type

  Summary:
    Describes the sensor type.

  Description:
    The metrology driver has been designed to interface with Current Transformers, Rogowski Coils and Shunt Resistors current sensors.
*/
typedef enum {
    SENSOR_CT        = 0,
    SENSOR_SHUNT     = 1,
    SENSOR_ROGOWSKI  = 2,
    SENSOR_VRD       = 3,
    SENSOR_TEMP      = 4,
    SENSOR_NOTUSED   = 5,
    SENSOR_NUM_TYPE
} DRV_METROLOGY_SENSOR_TYPE;

/* Metrology Driver Gain Type

  Summary:
    Gain selected for use in the ADC front-end.

  Description:
    On Current measurement channels gain for voltage is fixed to 1.
*/
typedef enum {
    GAIN_1        = 0,
    GAIN_2        = 1,
    GAIN_4        = 2,
    GAIN_8        = 3,
    GAIN_NUM_TYPE
} DRV_METROLOGY_GAIN_TYPE;

/* Metrology Channel Description

  Summary:
    Describes the channel configuration

  Description:
    - Sensor Type
    - Channel Name
    - Channel Gain

*/
typedef struct {
    char *name;
    DRV_METROLOGY_GAIN_TYPE gain;
    DRV_METROLOGY_SENSOR_TYPE sensorType;
} DRV_METROLOGY_CHANNEL;

/* Metrology Driver Dominant Voltage Selection

  Summary:
    Dominant Voltage Selection (SYNCH field in FEATURE_CTRL register).

  Description:
    These are the allowed values for SYNCH field in FEATURE_CTRL register.
    It is used for Neutral Current phase calibration, to select which is the
    voltage channel used as reference.
*/
typedef enum {
    DOMINANT_V_DYNAMIC = 0,
    DOMINANT_V_PHASE_A = 1,
    DOMINANT_V_PHASE_B = 2,
    DOMINANT_V_PHASE_C = 3,
    DOMINANT_V_NUM
} DRV_METROLOGY_DOMINANT_V_SEL;

/* Metrology Driver Calibration Mask

  Summary:
    Mask to select which metrology parameters are calibrated.

  Description:
    - magnitudeVx. Enables calibration of voltage channel Vx magnitude.
    - magnitudeIx. Enables calibration of current channel Ix magnitude.
    - anglePhaseX. Enables calibration of phase X angle (angle between IX and VX)
    - angleVAB. Enables calibration of angle between VB and VA
    - angleVAC. Enables calibration of angle between VC and VA
    - vRefPhaseN. Selects the reference voltage channel for neutral current angle calibration.
*/
typedef struct {
    unsigned int magnitudeVa : 1;
    unsigned int magnitudeVb : 1;
    unsigned int magnitudeVc : 1;
    unsigned int magnitudeVd : 1;
    unsigned int magnitudeIa : 1;
    unsigned int magnitudeIb : 1;
    unsigned int magnitudeIc : 1;
    unsigned int magnitudeIn : 1;
    unsigned int anglePhaseA : 1;
    unsigned int anglePhaseB : 1;
    unsigned int anglePhaseC : 1;
    unsigned int anglePhaseN : 1;
    unsigned int angleVAB : 1;
    unsigned int angleVAC : 1;
    unsigned int vRefPhaseN : 2;
} DRV_METROLOGY_CALIBRATION_MASK;

/* Metrology Driver Capture Channel Identifier

  Summary:
    Channel identifier used in the waveform capture process.

  Description:
    Identify the channel to be captured. Up to 7 channels of data may be captured at the same time

*/
typedef enum {
    CAPTURE_CHN_IA    = 0,
    CAPTURE_CHN_VA    = 1,
    CAPTURE_CHN_IB    = 2,
    CAPTURE_CHN_VB    = 3,
    CAPTURE_CHN_IC    = 4,
    CAPTURE_CHN_VC    = 5,
    CAPTURE_CHN_IN    = 6,
    CAPTURE_CHN_VD    = 7
} DRV_METROLOGY_CAPTURE_CHN_MASK;

/* Metrology Driver Capture Source Data

  Summary:
    Source data used in the waveform capture process.

  Description:
    - CAPTURE_SRC_16KHz captures 16 kHz data [sQ1.30] before DSP filtering
    - CAPTURE_SRC_4KHz_FBW captures 4 kHz FBW data [sQ2.29] (Full Bandwidth = fundamental + harmonics)
    - CAPTURE_SRC_4KHz_NBW captures 4 kHz NBW data [sQ2.29] (Narrow Bandwidth = fundamental only)
    - CAPTURE_SRC_8KHz_FBW captures 8 kHz FBW data [sQ2.29] (Full Bandwidth = fundamental + harmonics)

*/
typedef enum {
    CAPTURE_SRC_16KHz        = CAPTURE_CTRL_CAPTURE_SOURCE_16KHz_Val,
    CAPTURE_SRC_4KHz_FBW     = CAPTURE_CTRL_CAPTURE_SOURCE_4KHz_FBW_Val,
    CAPTURE_SRC_4KHz_NBW     = CAPTURE_CTRL_CAPTURE_SOURCE_4KHz_NBW_Val,
    CAPTURE_SRC_8KHz_FBW     = CAPTURE_CTRL_CAPTURE_SOURCE_8KHz_FBW_Val,
    CAPTURE_SRC_NUM
} DRV_METROLOGY_CAPTURE_SOURCE;

/* Metrology Driver Capture Type

  Summary:
    Identify the type of the capture process.

  Description:
    - CAPTURE_ONE_SHOT captures total CAPTURE_BUFF_SIZE of future data. Capture will stop after capture
        finishes or is disabled
    - CAPTURE_CONTINUOUS captures of waveform data in circular-buffer fashion. Capture will stop
        immediately after capture is disabled.

  Note:
    When using Continuous capture, CAPTURE_OFFSET indicates the offset of latest
    sample in capture buffer. This offset can be obtained by DRV_METROLOGY_CaptureGetOffset() function.

*/
typedef enum {
    CAPTURE_ONE_SHOT    = CAPTURE_CTRL_CAPTURE_TYPE_ONE_SHOT_Val,
    CAPTURE_CONTINUOUS  = CAPTURE_CTRL_CAPTURE_TYPE_CONTINUOS_Val,
    CAPTURE_TYPE_NUM
} DRV_METROLOGY_CAPTURE_TYPE;

/* Metrology Driver Capture State

  Summary:
    Identify the state of the capture process.

  Description:
    Waveform Capture Status.

*/
typedef enum {
    CAPTURE_DISABLED    = CAPTURE_STATUS_CAPTURE_STATE_DISABLED_Val,
    CAPTURE_ACTIVE      = CAPTURE_STATUS_CAPTURE_STATE_ACTIVE_Val,
    CAPTURE_COMPLETE    = CAPTURE_STATUS_CAPTURE_STATE_COMPLETE_Val
} DRV_METROLOGY_CAPTURE_STATE;

/* Metrology Driver Calibration References

  Summary:
    Specifies the all reference values used for the auto calibration process.

  Description:
    - vx refers to the RMS voltage applied to the voltage input where x = A,B,C,D
    - ix refers to the Rms current applied to the current input where x = A,B,C,N
    - ax refers to the Angle between the voltage and current vectors where x = A,B,C,N,VAB,VAC
    - calMask is a bitmask of the parameters to be calibrated
*/
typedef struct {
    double va;
    double ia;
    double aa;
    double vb;
    double ib;
    double ab;
    double vc;
    double ic;
    double ac;
    double in;
    double an;
    double vd;
    double avab;
    double avac;
    DRV_METROLOGY_CALIBRATION_MASK calMask;
} DRV_METROLOGY_CALIBRATION_REFS;

/* Metrology Driver Calibration Data

  Summary:
    Specifies all data internally needed for the auto calibration process.

  Description:
    - references. Calibration references. Client must be set the references before starting the calibration process.
    - numPreIntegrationPeriods. Number of integration periods to wait before starting the calibration process. It is set internally to 2.
    - numIntegrationPeriods. Number of integration periods needed to complete the calibration process. It is set internally to 4.
    - running. Flag used to check if the calibration process was completed.
    - result. Flag used to check if the calibration process has been successful.
    - Rest of the values are internally used to perform the calibration process.
*/
typedef struct {
    uint64_t dspAccIa;
    uint64_t dspAccIb;
    uint64_t dspAccIc;
    uint64_t dspAccIn;
    uint64_t dspAccUa;
    uint64_t dspAccUb;
    uint64_t dspAccUc;
    uint64_t dspAccUd;
    uint64_t dspAccUaf;
    uint64_t dspAccUbf;
    uint64_t dspAccUcf;
    uint64_t dspAccUdf;
    uint64_t dspAccUabf;
    uint64_t dspAccUcaf;
    int64_t  dspAccPa;
    int64_t  dspAccPb;
    int64_t  dspAccPc;
    int64_t  dspAccPn;
    int64_t  dspAccQa;
    int64_t  dspAccQb;
    int64_t  dspAccQc;
    int64_t  dspAccQn;
    uint32_t featureCtrlBackup;
    DRV_METROLOGY_CALIBRATION_REFS references;
    uint8_t numPreIntegrationPeriods;
    uint8_t numIntegrationPeriods;
    bool  running;
    bool  result;
} DRV_METROLOGY_CALIBRATION;

/* Metrology Driver Harmonic Data

  Summary:
    Identifies the result of the Harmonic Analysis process.

  Description:
    - Irms_X_m. RMS current value obtained as the result of last the harmonic analysis regarding channel X.
    - Irms_N_m. RMS current value obtained as the result of last the harmonic analysis regarding neutral channel.
    - Vrms_X_m. RMS voltage value obtained as the result of last the harmonic analysis regarding channel X.
*/
typedef struct {
    double Irms_A_m;
    double Irms_B_m;
    double Irms_C_m;
    double Irms_N_m;
    double Vrms_A_m;
    double Vrms_B_m;
    double Vrms_C_m;
} DRV_METROLOGY_HARMONICS_RMS;

/* Metrology Harmonic Analysis Data

  Summary:
    Internal data used to perform an harmonic analysis

  Description:
    - pHarmonicAnalysisResponse. Pointer to store the result of the Harmonic Analysis.
    - harmonicBitmap: Store the harmonics to be analyzed.
    - integrationPeriods: Indicate the number of integration periods that must be waited until get the response
    - running: Flag to indicate that harmonic analysis is in process.
*/
typedef struct {
    DRV_METROLOGY_HARMONICS_RMS * pHarmonicAnalysisResponse;
    uint32_t harmonicBitmap;
    uint8_t integrationPeriods;
    bool  running;
    bool holdRegs;
} DRV_METROLOGY_HARMONIC_ANALYSIS;

#define DRV_METROLOGY_SYN_CONTROL_KEY  0xA5

/* Metrology Synthesizer Control

  Summary:
    Data descriptor used to configure the synthesizer function

  Description:
    - numSamples: Number of samples to be transfered.
    - channel: Indicates the channel number to which the samples belong to.
    - key: Security key. It must be set to DRV_METROLOGY_SYNTHESIZER_CONTROL_KEY.

  Remarks:
    Any other Key value aborts the data transfer of the corresponding channel.
*/

typedef struct
{
    unsigned int numSamples : 19;
    unsigned int channel : 5;
    unsigned int key : 8;
} DRV_METROLOGY_SYN_CONTROL;

/* Metrology Synthesizer Descriptor

  Summary:
    Data descriptor used to configure the synthesizer function

  Description:
    - control: Synthesizer control values. See DRV_METROLOGY_SYNTHESIZER_CONTROL.
    - *pData: Pointer to the data buffer with the waveform samples.
    - *next: Pointer to the next descriptor address. If NULL, the descriptor is the last one to be performed.
    - *prev: Pointer to the previous descriptor address. If NULL, the descriptor is the first one to be performed.
*/
typedef struct DRV_METROLOGY_SYN_DESCRIPTOR{
    DRV_METROLOGY_SYN_CONTROL control;
    uint32_t *pData;
    struct DRV_METROLOGY_SYN_DESCRIPTOR *next;
    struct DRV_METROLOGY_SYN_DESCRIPTOR *prev;
} DRV_METROLOGY_SYN_DESCRIPTOR;

/* Metrology Driver AFE Events

  Summary:
    Identifies all events related to metrology library.

  Description:
    - pXDir. Identifies the sign of the active power in channel X. "1" means a negative value, "0" is a positive value.
    - qXDir. Identifies the sign of the reactive power in channel X. "1" means a negative value, "0" is a positive value.
    - sagX. Voltage Sag Detected Flag for Channel X. "1" means that voltage sag is detected.
    - swellX. Voltage Swell Detected Flag for Channel X. "1" means that voltage Swell is detected.
    - creepX. Channel X Current or Power Creep Detected Flag. "1" means that Creep is detected.
    - phActiveX. Voltage Active Detected Flag for Channel X. "1" means that voltage Active is detected.
*/
typedef struct {
    unsigned int paDir : 1;
    unsigned int pbDir : 1;
    unsigned int pcDir : 1;
    unsigned int ptDir : 1;
    unsigned int qaDir : 1;
    unsigned int qbDir : 1;
    unsigned int qcDir : 1;
    unsigned int qtDir : 1;
    unsigned int pafDir : 1;
    unsigned int pbfDir : 1;
    unsigned int pcfDir : 1;
    unsigned int ptfDir : 1;
    unsigned int qafDir : 1;
    unsigned int qbfDir : 1;
    unsigned int qcfDir : 1;
    unsigned int qtfDir : 1;
    unsigned int swellA : 1;
    unsigned int swellB : 1;
    unsigned int swellC : 1;
    unsigned int sagA : 1;
    unsigned int sagB : 1;
    unsigned int sagC : 1;
    unsigned int creepP : 1;
    unsigned int creepPA : 1;
    unsigned int creepPB : 1;
    unsigned int creepPC : 1;
    unsigned int creepQ : 1;
    unsigned int creepQA : 1;
    unsigned int creepQB : 1;
    unsigned int creepQC : 1;
    unsigned int creepIA : 1;
    unsigned int creepIB : 1;
    unsigned int creepIC : 1;
    unsigned int creepS : 1;
    unsigned int phActiveA : 1;
    unsigned int phActiveB : 1;
    unsigned int phActiveC : 1;
    unsigned int reserved : 1;
} DRV_METROLOGY_AFE_EVENTS;

typedef union {
    DRV_METROLOGY_AFE_EVENTS afeEvents;
    uint64_t afeEventsMask;
} DRV_METROLOGY_AFE_EVENTS_UNION;

/* Metrology Driver Measurements type

  Summary:
    Identifies all types of measurements.

  Description:
    Values are calculated both including all harmonics and fundamental only (F added at the end of magnitude name), where:
        - U = Voltage RMS value
        - I = Current RMS value
        - P = Active power value
        - Q = Reactive power value
        - S = Aparent power value
        - FREQ = Frequency of the line voltage fundamental harmonic component determined by the Metrology library using the dominant phase
        - ANGLE = Angle between the voltage and current vectors
*/
typedef enum {
    MEASURE_UA_RMS = 0,
    MEASURE_UB_RMS,
    MEASURE_UC_RMS,
    MEASURE_UD_RMS,
    MEASURE_IA_RMS,
    MEASURE_IB_RMS,
    MEASURE_IC_RMS,
    MEASURE_INI_RMS,
    MEASURE_INM_RMS,
    MEASURE_INMI_RMS,
    MEASURE_PT,
    MEASURE_PA,
    MEASURE_PB,
    MEASURE_PC,
    MEASURE_QT,
    MEASURE_QA,
    MEASURE_QB,
    MEASURE_QC,
    MEASURE_ST,
    MEASURE_SA,
    MEASURE_SB,
    MEASURE_SC,
    MEASURE_UAF_RMS,
    MEASURE_UBF_RMS,
    MEASURE_UCF_RMS,
    MEASURE_UDF_RMS,
    MEASURE_IAF_RMS,
    MEASURE_IBF_RMS,
    MEASURE_ICF_RMS,
    MEASURE_IMNF_RMS,
    MEASURE_PTF,
    MEASURE_PAF,
    MEASURE_PBF,
    MEASURE_PCF,
    MEASURE_QTF,
    MEASURE_QAF,
    MEASURE_QBF,
    MEASURE_QCF,
    MEASURE_STF,
    MEASURE_SAF,
    MEASURE_SBF,
    MEASURE_SCF,
    MEASURE_FREQ,
    MEASURE_FREQA,
    MEASURE_FREQB,
    MEASURE_FREQC,
    MEASURE_ANGLEA,
    MEASURE_ANGLEB,
    MEASURE_ANGLEC,
    MEASURE_ANGLEN,
    MEASURE_ANGLEVAB,
    MEASURE_ANGLEVAC,
    MEASURE_TYPE_NUM
} DRV_METROLOGY_MEASURE_TYPE;

/* Metrology Driver AFE calculated data

  Summary:
    Identifies the data calculated from the metrology AFE measurements.

  Description:
    - energy. Active energy calculated value.
    - afeEvents. AFE events data.
    - measure[MEASURE_TYPE_NUM]. Measure calculated values.
*/
typedef struct {
    DRV_METROLOGY_AFE_EVENTS afeEvents;
    float energy;
    float measure[MEASURE_TYPE_NUM];
    float cycleMeasure[MEASURE_TYPE_NUM];
} DRV_METROLOGY_AFE_DATA;

/* METROLOGY Driver Status

  Summary:
    Defines the status of the DRV_METROLOGY driver.

  Description:
    This enumeration defines the status of the DRV_METROLOGY Driver:
        - DRV_METROLOGY_STATUS_UNINITIALIZED: Metrology driver has not been initialized.
        - DRV_METROLOGY_STATUS_READY: Metrology driver is ready to be used.
        - DRV_METROLOGY_STATUS_HALT: Metrology driver has been initialized but not opened.
        - DRV_METROLOGY_STATUS_WAITING_IPC: Metrology driver is waiting the init IPC interrupt
        from the metrology lib as part of the opening routine.
        - DRV_METROLOGY_STATUS_INIT_DSP: IPC interrupt has been triggered indicating that DSP
        filters has been stabilized to full accuracy.
        - DRV_METROLOGY_STATUS_RUNNING: Metrology library is running and periodic data
        acquisition is performed.

  Remarks:
    None.
*/

typedef enum
{
    DRV_METROLOGY_STATUS_UNINITIALIZED = SYS_STATUS_UNINITIALIZED,
    DRV_METROLOGY_STATUS_BUSY = SYS_STATUS_BUSY,
    DRV_METROLOGY_STATUS_READY = SYS_STATUS_READY,
    DRV_METROLOGY_STATUS_HALT = SYS_STATUS_READY_EXTENDED + 1U,
    DRV_METROLOGY_STATUS_WAITING_IPC = SYS_STATUS_READY_EXTENDED + 2U,
    DRV_METROLOGY_STATUS_INIT_DSP = SYS_STATUS_READY_EXTENDED + 3U,
    DRV_METROLOGY_STATUS_RUNNING = SYS_STATUS_READY_EXTENDED + 4U,
    DRV_METROLOGY_STATUS_ERROR = SYS_STATUS_ERROR,
} DRV_METROLOGY_STATUS;

/* Metrology Driver Initialization Data

  Summary:
    Defines the data required to initialize the Metrology driver

  Description:
    - regBaseAddress. Base Address for Metrology registers.
    - binStartAddress. Start Address where Metrology library application file is located.
    - binEndAddress. End Address where Metrology library application binary file is located.

  Remarks:
    None.
*/
typedef struct {
    uint32_t regBaseAddress;
    uint32_t binStartAddress;
    uint32_t binEndAddress;
} DRV_METROLOGY_INIT;

#ifdef __cplusplus
}
#endif

#endif // #ifndef DRV_METROLOGY_DEFINITIONS_H
/*******************************************************************************
 End of File
*/
