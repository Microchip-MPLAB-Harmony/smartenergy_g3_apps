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

#ifndef METROLOGY_REGS_H
#define METROLOGY_REGS_H

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR METROLOGY REGISTERS                          */
/* ************************************************************************** */
/* MISRA C-2012 deviation block start */
/* MISRA C-2012 Rule 5.4 deviated 64 times.  Deviation record ID - H3_MISRAC_2012_R_5_4_DR_1 */

/* -------- STATE_CTRL : (MET_CONTROL Offset: 0x00) (R/W 32) Metrology state control register -------- */
#define STATE_CTRL_STATE_CTRL_Pos                     _UINT32_(0)                               /* (STATE_CTRL) State Control Position */
#define STATE_CTRL_STATE_CTRL_Msk                     (_UINT32_(0xF) << STATE_CTRL_STATE_CTRL_Pos)         /* (STATE_CTRL) State Control Mask */
#define STATE_CTRL_STATE_CTRL(value)                  (STATE_CTRL_STATE_CTRL_Msk & (_UINT32_(value) << STATE_CTRL_STATE_CTRL_Pos))
#define   STATE_CTRL_STATE_CTRL_RESET_Val               _UINT32_(0x0)                           /* (STATE_CTRL) Reset metrology module, reset ATSENSE and halt metrology filters.  */
#define   STATE_CTRL_STATE_CTRL_INIT_Val                _UINT32_(0x1)                           /* (STATE_CTRL) Input control registers are initialized and metrology can initialize itself.  */
#define   STATE_CTRL_STATE_CTRL_RUN_Val                 _UINT32_(0x2)                           /* (STATE_CTRL) Metrology filters are free to run, and outputs are updated at commanded rate.  */

/* -------- FEATURE_CTRL : (MET_CONTROL Offset: 0x04) (R/W 32) Metrology feature setting register 0 -------- */
#define FEATURE_CTRL_RZC_CHAN_SELECT_Pos             _UINT32_(0)                               /* (FEATURE_CTRL) Raw Zero-Crossing Channel Select Position */
#define FEATURE_CTRL_RZC_CHAN_SELECT_Msk             (_UINT32_(0x7) << FEATURE_CTRL_RZC_CHAN_SELECT_Pos)    /* (FEATURE_CTRL) Raw Zero-Crossing Channel Select Mask */
#define FEATURE_CTRL_RZC_CHAN_SELECT(value)          (FEATURE_CTRL_RZC_CHAN_SELECT_Msk & (_UINT32_(value) << FEATURE_CTRL_RZC_CHAN_SELECT_Pos))
#define   FEATURE_CTRL_RZC_CHAN_SELECT_DISABLED_Val    _UINT32_(0x0)                           /* (FEATURE_CTRL) Raw Zero-Crossing detection is disabled */
#define   FEATURE_CTRL_RZC_CHAN_SELECT_V1_Val          _UINT32_(0x1)                           /* (FEATURE_CTRL) Raw Zero-Crossing detection based on input voltage channel V1 */
#define   FEATURE_CTRL_RZC_CHAN_SELECT_V2_Val          _UINT32_(0x3)                           /* (FEATURE_CTRL) Raw Zero-Crossing detection based on input voltage channel V2 */
#define   FEATURE_CTRL_RZC_CHAN_SELECT_V3_Val          _UINT32_(0x5)                           /* (FEATURE_CTRL) Raw Zero-Crossing detection based on input voltage channel V3 */
#define FEATURE_CTRL_RZC_DIR_Pos                     _UINT32_(3)                               /* (FEATURE_CTRL) Raw Zero-Crossing Direction Selection Position */
#define FEATURE_CTRL_RZC_DIR_Msk                     (_UINT32_(0x1) << FEATURE_CTRL_RZC_DIR_Pos)            /* (FEATURE_CTRL) Raw Zero-Crossing Direction Selection Mask */
#define FEATURE_CTRL_RZC_DIR(value)                  (FEATURE_CTRL_RZC_DIR_Msk & (_UINT32_(value) << FEATURE_CTRL_RZC_DIR_Pos))
#define   FEATURE_CTRL_RZC_DIR_POSITIVE_Val            _UINT32_(0x0)                           /* (FEATURE_CTRL) Positive-going raw zero-crossings selected */
#define   FEATURE_CTRL_RZC_DIR_NEGATIVE_Val            _UINT32_(0x1)                           /* (FEATURE_CTRL) Negative-going raw zero-crossings selected */
#define FEATURE_CTRL_SYNCH_Pos                       _UINT32_(4)                               /* (FEATURE_CTRL) Active Voltage Channel Selection Position */
#define FEATURE_CTRL_SYNCH_Msk                       (_UINT32_(0x2) << FEATURE_CTRL_SYNCH_Pos)    /* (FEATURE_CTRL) Active Voltage Channel Selection Mask */
#define FEATURE_CTRL_SYNCH(value)                    (FEATURE_CTRL_SYNCH_Msk & (_UINT32_(value) << FEATURE_CTRL_SYNCH_Pos))
#define   FEATURE_CTRL_SYNCH_ACTIVE_PHASE_Val          _UINT32_(0x0)                           /* (FEATURE_CTRL) Measurement interval based on active phase, determined dynamically */
#define   FEATURE_CTRL_SYNCH_A_Val                     _UINT32_(0x1)                           /* (FEATURE_CTRL) Measurement interval based on phase-A */
#define   FEATURE_CTRL_SYNCH_B_Val                     _UINT32_(0x2)                           /* (FEATURE_CTRL) Measurement interval based on phase-B */
#define   FEATURE_CTRL_SYNCH_C_Val                     _UINT32_(0x3)                           /* (FEATURE_CTRL) Measurement interval based on phase-C */
#define FEATURE_CTRL_PHASE_A_EN_Pos                  _UINT32_(8)                               /* (FEATURE_CTRL) Enable Phase A Position */
#define FEATURE_CTRL_PHASE_A_EN_Msk                  (_UINT32_(0x1) << FEATURE_CTRL_PHASE_A_EN_Pos)    /* (FEATURE_CTRL) Enable Phase A Mask */
#define FEATURE_CTRL_PHASE_A_EN(value)               (FEATURE_CTRL_PHASE_A_EN_Msk & (_UINT32_(value) << FEATURE_CTRL_PHASE_A_EN_Pos))
#define   FEATURE_CTRL_PHASE_A_EN_DISABLED_Val         _UINT32_(0x0)                           /* (FEATURE_CTRL) Disable phase */
#define   FEATURE_CTRL_PHASE_A_EN_ENABLED_Val          _UINT32_(0x1)                           /* (FEATURE_CTRL) Enable phase */
#define FEATURE_CTRL_PHASE_B_EN_Pos                  _UINT32_(9)                               /* (FEATURE_CTRL) Enable Phase B Position */
#define FEATURE_CTRL_PHASE_B_EN_Msk                  (_UINT32_(0x1) << FEATURE_CTRL_PHASE_B_EN_Pos)    /* (FEATURE_CTRL) Enable Phase B Mask */
#define FEATURE_CTRL_PHASE_B_EN(value)               (FEATURE_CTRL_PHASE_B_EN_Msk & (_UINT32_(value) << FEATURE_CTRL_PHASE_B_EN_Pos))
#define   FEATURE_CTRL_PHASE_B_EN_DISABLED_Val         _UINT32_(0x0)                           /* (FEATURE_CTRL) Disable phase */
#define   FEATURE_CTRL_PHASE_B_EN_ENABLED_Val          _UINT32_(0x1)                           /* (FEATURE_CTRL) Enable phase */
#define FEATURE_CTRL_PHASE_C_EN_Pos                  _UINT32_(10)                               /* (FEATURE_CTRL) Enable Phase C Position */
#define FEATURE_CTRL_PHASE_C_EN_Msk                  (_UINT32_(0x1) << FEATURE_CTRL_PHASE_C_EN_Pos)    /* (FEATURE_CTRL) Enable Phase C Mask */
#define FEATURE_CTRL_PHASE_C_EN(value)               (FEATURE_CTRL_PHASE_C_EN_Msk & (_UINT32_(value) << FEATURE_CTRL_PHASE_C_EN_Pos))
#define   FEATURE_CTRL_PHASE_C_EN_DISABLED_Val         _UINT32_(0x0)                           /* (FEATURE_CTRL) Disable phase */
#define   FEATURE_CTRL_PHASE_C_EN_ENABLED_Val          _UINT32_(0x1)                           /* (FEATURE_CTRL) Enable phase */
#define FEATURE_CTRL_MAX_INT_SELECT_Pos              _UINT32_(12)                               /* (FEATURE_CTRL) Max Integration Period Select Position */
#define FEATURE_CTRL_MAX_INT_SELECT_Msk              (_UINT32_(0x1) << FEATURE_CTRL_MAX_INT_SELECT_Pos)    /* (FEATURE_CTRL) Max Integration Period Select Mask */
#define FEATURE_CTRL_MAX_INT_SELECT(value)           (FEATURE_CTRL_MAX_INT_SELECT_Msk & (_UINT32_(value) << FEATURE_CTRL_MAX_INT_SELECT_Pos))
#define   FEATURE_CTRL_MAX_INT_SELECT_CL1_Val           _UINT32_(0x0)                           /* (FEATURE_CTRL) Metrology DSP will integrate for an integral number of cycles closest to 1 second */
#define   FEATURE_CTRL_MAX_INT_SELECT_NGT1_Val          _UINT32_(0x1)                           /* (FEATURE_CTRL) Metrology DSP will integrate for an integral number of cycles no greater than 1 second */
#define FEATURE_CTRL_CREEP_I_EN_Pos                  _UINT32_(13)                                /* (FEATURE_CTRL) Current Creep Threshold Function Enable Position */
#define FEATURE_CTRL_CREEP_I_EN_Msk                  (_UINT32_(0x1) << FEATURE_CTRL_CREEP_I_EN_Pos)    /* (FEATURE_CTRL) Current Creep Threshold Function Enable Mask */
#define FEATURE_CTRL_CREEP_I_EN(value)               (FEATURE_CTRL_CREEP_I_EN_Msk & (_UINT32_(value) << FEATURE_CTRL_CREEP_I_EN_Pos))
#define   FEATURE_CTRL_CREEP_I_EN_DISABLED_Val         _UINT32_(0x0)                             /* (FEATURE_CTRL) Disable current creep threshold feature */
#define   FEATURE_CTRL_CREEP_I_EN_ENABLED_Val          _UINT32_(0x1)                             /* (FEATURE_CTRL) Enable current creep threshold feature */
#define FEATURE_CTRL_CREEP_Q_EN_Pos                  _UINT32_(14)                                /* (FEATURE_CTRL) Reactive Creep Threshold Function Enable Position */
#define FEATURE_CTRL_CREEP_Q_EN_Msk                  (_UINT32_(0x1) << FEATURE_CTRL_CREEP_Q_EN_Pos)    /* (FEATURE_CTRL) Reactive Creep Threshold Function Enable Mask */
#define FEATURE_CTRL_CREEP_Q_EN(value)               (FEATURE_CTRL_CREEP_Q_EN_Msk & (_UINT32_(value) << FEATURE_CTRL_CREEP_Q_EN_Pos))
#define   FEATURE_CTRL_CREEP_Q_EN_DISABLED_Val         _UINT32_(0x0)                             /* (FEATURE_CTRL) Disable Reactive creep threshold feature */
#define   FEATURE_CTRL_CREEP_Q_EN_ENABLED_Val          _UINT32_(0x1)                             /* (FEATURE_CTRL) Enable Reactive creep threshold feature */
#define FEATURE_CTRL_CREEP_P_EN_Pos                  _UINT32_(15)                                /* (FEATURE_CTRL) Active Creep Threshold Function Enable Position */
#define FEATURE_CTRL_CREEP_P_EN_Msk                  (_UINT32_(0x1) << FEATURE_CTRL_CREEP_P_EN_Pos)    /* (FEATURE_CTRL) Active Creep Threshold Function Enable Mask */
#define FEATURE_CTRL_CREEP_P_EN(value)               (FEATURE_CTRL_CREEP_P_EN_Msk & (_UINT32_(value) << FEATURE_CTRL_CREEP_P_EN_Pos))
#define   FEATURE_CTRL_CREEP_P_EN_DISABLED_Val         _UINT32_(0x0)                             /* (FEATURE_CTRL) Disable Active creep threshold feature */
#define   FEATURE_CTRL_CREEP_P_EN_ENABLED_Val          _UINT32_(0x1)                             /* (FEATURE_CTRL) Enable Active creep threshold feature */
#define FEATURE_CTRL_SWAP_B_and_C_Pos                _UINT32_(16)                              /* (FEATURE_CTRL) Swap processing channels Position */
#define FEATURE_CTRL_SWAP_B_and_C_Msk                (_UINT32_(0x1) << FEATURE_CTRL_SWAP_B_and_C_Pos)    /* (FEATURE_CTRL) Swap processing channels Mask */
#define FEATURE_CTRL_SWAP_B_and_C(value)             (FEATURE_CTRL_SWAP_B_and_C_Msk & (_UINT32_(value) << FEATURE_CTRL_SWAP_B_and_C_Pos))
#define   FEATURE_CTRL_SWAP_B_and_C_I2IB_Val           _UINT32_(0x0)                           /* (FEATURE_CTRL) DSP input mapping: I2/V2 mapped I_B/V_B; I3/V3 mapped I_C/V_C (default) */
#define   FEATURE_CTRL_SWAP_B_and_C_I2IC_Val           _UINT32_(0x1)                           /* (FEATURE_CTRL) DSP input mapping: I2/V2 mapped I_C/V_C; I3/V3 mapped I_B/V_B */
#define FEATURE_CTRL_I_MAX_RESET_Pos                 _UINT32_(17)                                /* (FEATURE_CTRL) Reset All I_x_MAX values Position */
#define FEATURE_CTRL_I_MAX_RESET_Msk                 (_UINT32_(0x1) << FEATURE_CTRL_I_MAX_RESET_Pos)    /* (FEATURE_CTRL) Reset All I_x_MAX values Mask */
#define FEATURE_CTRL_I_MAX_RESET(value)              (FEATURE_CTRL_I_MAX_RESET_Msk & (_UINT32_(value) << FEATURE_CTRL_I_MAX_RESET_Pos))
#define FEATURE_CTRL_V_MAX_RESET_Pos                 _UINT32_(18)                                /* (FEATURE_CTRL) Reset All V_x_MAX values Position */
#define FEATURE_CTRL_V_MAX_RESET_Msk                 (_UINT32_(0x1) << FEATURE_CTRL_V_MAX_RESET_Pos)    /* (FEATURE_CTRL) Reset All V_x_MAX values Mask */
#define FEATURE_CTRL_V_MAX_RESET(value)              (FEATURE_CTRL_V_MAX_RESET_Msk & (_UINT32_(value) << FEATURE_CTRL_V_MAX_RESET_Pos))
#define FEATURE_CTRL_IA_HARM_DIS_Pos                 _UINT32_(24)                                /* (FEATURE_CTRL) Disable Harmonic IA Analysis */
#define FEATURE_CTRL_IA_HARM_DIS_Msk                 (_UINT32_(0x1) << FEATURE_CTRL_IA_HARM_DIS_Pos)    /* (FEATURE_CTRL) Enable Harmonic IA Analysis Mask */
#define FEATURE_CTRL_IA_HARM_DIS(value)              (FEATURE_CTRL_IA_HARM_DIS_Msk & (_UINT32_(value) << FEATURE_CTRL_IA_HARM_DIS_Pos))
#define   FEATURE_CTRL_IA_HARM_ENABLED_Val             _UINT32_(0x0)                             /* (FEATURE_CTRL) Enable harmonic IA Analysis  */
#define   FEATURE_CTRL_IA_HARM_DISABLED_Val            _UINT32_(0x1)                             /* (FEATURE_CTRL) Disable harmonic IA Analysis  */
#define FEATURE_CTRL_VA_HARM_DIS_Pos                 _UINT32_(25)                                /* (FEATURE_CTRL) Disable Harmonic VA Analysis */
#define FEATURE_CTRL_VA_HARM_DIS_Msk                 (_UINT32_(0x1) << FEATURE_CTRL_VA_HARM_DIS_Pos)    /* (FEATURE_CTRL) Enable Harmonic VA Analysis Mask */
#define FEATURE_CTRL_VA_HARM_DIS(value)              (FEATURE_CTRL_VA_HARM_DIS_Msk & (_UINT32_(value) << FEATURE_CTRL_VA_HARM_DIS_Pos))
#define   FEATURE_CTRL_VA_HARM_ENABLED_Val             _UINT32_(0x0)                             /* (FEATURE_CTRL) Enable harmonic VA Analysis  */
#define   FEATURE_CTRL_VA_HARM_DISABLED_Val            _UINT32_(0x1)                             /* (FEATURE_CTRL) Disable harmonic VA Analysis  */
#define FEATURE_CTRL_IB_HARM_DIS_Pos                 _UINT32_(26)                                /* (FEATURE_CTRL) Disable Harmonic IB Analysis */
#define FEATURE_CTRL_IB_HARM_DIS_Msk                 (_UINT32_(0x1) << FEATURE_CTRL_IB_HARM_DIS_Pos)    /* (FEATURE_CTRL) Enable Harmonic IB Analysis Mask */
#define FEATURE_CTRL_IB_HARM_DIS(value)              (FEATURE_CTRL_IB_HARM_DIS_Msk & (_UINT32_(value) << FEATURE_CTRL_IB_HARM_DIS_Pos))
#define   FEATURE_CTRL_IB_HARM_ENABLED_Val             _UINT32_(0x0)                             /* (FEATURE_CTRL) Enable harmonic IB Analysis  */
#define   FEATURE_CTRL_IB_HARM_DISABLED_Val            _UINT32_(0x1)                             /* (FEATURE_CTRL) Disable harmonic IB Analysis  */
#define FEATURE_CTRL_VB_HARM_DIS_Pos                 _UINT32_(27)                                /* (FEATURE_CTRL) Disable Harmonic VB Analysis */
#define FEATURE_CTRL_VB_HARM_DIS_Msk                 (_UINT32_(0x1) << FEATURE_CTRL_VB_HARM_DIS_Pos)    /* (FEATURE_CTRL) Enable Harmonic VB Analysis Mask */
#define FEATURE_CTRL_VB_HARM_DIS(VBlue)              (FEATURE_CTRL_VB_HARM_DIS_Msk & (_UINT32_(Value) << FEATURE_CTRL_VB_HARM_DIS_Pos))
#define   FEATURE_CTRL_VB_HARM_ENABLED_Val             _UINT32_(0x0)                             /* (FEATURE_CTRL) Enable harmonic VB Analysis  */
#define   FEATURE_CTRL_VB_HARM_DISABLED_Val            _UINT32_(0x1)                             /* (FEATURE_CTRL) Disable harmonic VB Analysis  */
#define FEATURE_CTRL_IC_HARM_DIS_Pos                 _UINT32_(28)                                /* (FEATURE_CTRL) Disable Harmonic IC Analysis */
#define FEATURE_CTRL_IC_HARM_DIS_Msk                 (_UINT32_(0x1) << FEATURE_CTRL_IC_HARM_DIS_Pos)    /* (FEATURE_CTRL) Enable Harmonic IC Analysis Mask */
#define FEATURE_CTRL_IC_HARM_DIS(value)              (FEATURE_CTRL_IC_HARM_DIS_Msk & (_UINT32_(value) << FEATURE_CTRL_IC_HARM_DIS_Pos))
#define   FEATURE_CTRL_IC_HARM_ENABLED_Val             _UINT32_(0x0)                             /* (FEATURE_CTRL) Enable harmonic IC Analysis  */
#define   FEATURE_CTRL_IC_HARM_DISABLED_Val            _UINT32_(0x1)                             /* (FEATURE_CTRL) Disable harmonic IC Analysis  */
#define FEATURE_CTRL_VC_HARM_DIS_Pos                 _UINT32_(29)                                /* (FEATURE_CTRL) Disable Harmonic VC Analysis */
#define FEATURE_CTRL_VC_HARM_DIS_Msk                 (_UINT32_(0x1) << FEATURE_CTRL_VC_HARM_DIS_Pos)    /* (FEATURE_CTRL) Enable Harmonic VC Analysis Mask */
#define FEATURE_CTRL_VC_HARM_DIS(value)              (FEATURE_CTRL_VC_HARM_DIS_Msk & (_UINT32_(value) << FEATURE_CTRL_VC_HARM_DIS_Pos))
#define   FEATURE_CTRL_VC_HARM_ENABLED_Val             _UINT32_(0x0)                             /* (FEATURE_CTRL) Enable harmonic VC Analysis  */
#define   FEATURE_CTRL_VC_HARM_DISABLED_Val            _UINT32_(0x1)                             /* (FEATURE_CTRL) Disable harmonic VC Analysis  */
#define FEATURE_CTRL_IN_HARM_DIS_Pos                 _UINT32_(30)                                /* (FEATURE_CTRL) Disable Harmonic IN Analysis */
#define FEATURE_CTRL_IN_HARM_DIS_Msk                 (_UINT32_(0x1) << FEATURE_CTRL_IN_HARM_DIS_Pos)    /* (FEATURE_CTRL) Enable Harmonic IN Analysis Mask */
#define FEATURE_CTRL_IN_HARM_DIS(value)              (FEATURE_CTRL_IN_HARM_DIS_Msk & (_UINT32_(value) << FEATURE_CTRL_IN_HARM_DIS_Pos))
#define   FEATURE_CTRL_IN_HARM_ENABLED_Val             _UINT32_(0x0)                             /* (FEATURE_CTRL) Enable harmonic IN Analysis  */
#define   FEATURE_CTRL_IN_HARM_DISABLED_Val            _UINT32_(0x1)                             /* (FEATURE_CTRL) Disable harmonic IN Analysis  */

/* -------- HARMONIC_CTRL : (MET_CONTROL Offset: 0x08) (R/W 32) Metrology feature setting register for Harmonics -------- */
#define HARMONIC_CTRL_HARMONIC_m_REQ_Pos              _UINT32_(0)                                 /* (HARMONIC_CTRL) Request harmonic order for analysis Position */
#define HARMONIC_CTRL_HARMONIC_m_REQ_Msk              (_UINT32_(0x7FFFFFFF) << HARMONIC_CTRL_HARMONIC_m_REQ_Pos)    /* (HARMONIC_CTRL) Request harmonic order for analysis Mask */
#define HARMONIC_CTRL_HARMONIC_m_REQ(value)           (HARMONIC_CTRL_HARMONIC_m_REQ_Msk & (_UINT32_(value) << HARMONIC_CTRL_HARMONIC_m_REQ_Pos))
#define HARMONIC_CTRL_HARMONIC_EN_Pos                 _UINT32_(31)                                /* (HARMONIC_CTRL) Enable Harmonic Analysis Position */
#define HARMONIC_CTRL_HARMONIC_EN_Msk                 (_UINT32_(0x1) << HARMONIC_CTRL_HARMONIC_EN_Pos)    /* (HARMONIC_CTRL) Enable Harmonic Analysis Mask */
#define HARMONIC_CTRL_HARMONIC_EN(value)              (HARMONIC_CTRL_HARMONIC_EN_Msk & (_UINT32_(value) << HARMONIC_CTRL_HARMONIC_EN_Pos))
#define   HARMONIC_CTRL_HARMONIC_EN_DISABLED_Val        _UINT32_(0x0)                             /* (HARMONIC_CTRL) Disable harmonic analysis  */
#define   HARMONIC_CTRL_HARMONIC_EN_ENABLED_Val         _UINT32_(0x1)                             /* (HARMONIC_CTRL) Enable harmonic analysis  */

/* -------- METER_TYPE : (MET_CONTROL Offset: 0x0C) (R/W  32) Sensor type and service type settings register -------- */
#define METER_TYPE_SENSOR_TYPE_I_A_Pos             _UINT32_(0)                                 /* (METER_TYPE) Current Sensor Type on A Phase Position */
#define METER_TYPE_SENSOR_TYPE_I_A_Msk             (_UINT32_(0x3) << METER_TYPE_SENSOR_TYPE_I_A_Pos)    /* (METER_TYPE) Current Sensor Type on A Phase Mask */
#define METER_TYPE_SENSOR_TYPE_I_A(value)          (METER_TYPE_SENSOR_TYPE_I_A_Msk & (_UINT32_(value) << METER_TYPE_SENSOR_TYPE_I_A_Pos))
#define   METER_TYPE_SENSOR_TYPE_I_A_CT_Val          _UINT32_(0x0)                             /* (METER_TYPE) Current Transformer */
#define   METER_TYPE_SENSOR_TYPE_I_A_SHUNT_Val       _UINT32_(0x1)                             /* (METER_TYPE) Resistive Shunt */
#define   METER_TYPE_SENSOR_TYPE_I_A_CRC_Val         _UINT32_(0x2)                             /* (METER_TYPE) Rogowski Coil (CRC) current sensor */
#define   METER_TYPE_SENSOR_TYPE_I_A_VRD_Val         _UINT32_(0x3)                             /* (METER_TYPE) Resistive Divider (VRD) voltage sensor */
#define METER_TYPE_SENSOR_TYPE_V_A_Pos             _UINT32_(2)                                 /* (METER_TYPE) Voltage Sensor Type on A Phase Position */
#define METER_TYPE_SENSOR_TYPE_V_A_Msk             (_UINT32_(0x3) << METER_TYPE_SENSOR_TYPE_V_A_Pos)    /* (METER_TYPE) Voltage Sensor Type on A Phase Mask */
#define METER_TYPE_SENSOR_TYPE_V_A(value)          (METER_TYPE_SENSOR_TYPE_V_A_Msk & (_UINT32_(value) << METER_TYPE_SENSOR_TYPE_V_A_Pos))
#define   METER_TYPE_SENSOR_TYPE_V_A_CT_Val          _UINT32_(0x0)                             /* (METER_TYPE) Current Transformer */
#define   METER_TYPE_SENSOR_TYPE_V_A_SHUNT_Val       _UINT32_(0x1)                             /* (METER_TYPE) Resistive Shunt */
#define   METER_TYPE_SENSOR_TYPE_V_A_CRC_Val         _UINT32_(0x2)                             /* (METER_TYPE) Rogowski Coil (CRC) current sensor */
#define   METER_TYPE_SENSOR_TYPE_V_A_VRD_Val         _UINT32_(0x3)                             /* (METER_TYPE) Resistive Divider (VRD) voltage sensor */
#define METER_TYPE_SENSOR_TYPE_I_B_Pos             _UINT32_(4)                                 /* (METER_TYPE) Current Sensor Type on A Phase Position */
#define METER_TYPE_SENSOR_TYPE_I_B_Msk             (_UINT32_(0x3) << METER_TYPE_SENSOR_TYPE_I_B_Pos)    /* (METER_TYPE) Current Sensor Type on A Phase Mask */
#define METER_TYPE_SENSOR_TYPE_I_B(value)          (METER_TYPE_SENSOR_TYPE_I_B_Msk & (_UINT32_(value) << METER_TYPE_SENSOR_TYPE_I_B_Pos))
#define   METER_TYPE_SENSOR_TYPE_I_B_CT_Val          _UINT32_(0x0)                             /* (METER_TYPE) Current Transformer */
#define   METER_TYPE_SENSOR_TYPE_I_B_SHUNT_Val       _UINT32_(0x1)                             /* (METER_TYPE) Resistive Shunt */
#define   METER_TYPE_SENSOR_TYPE_I_B_CRC_Val         _UINT32_(0x2)                             /* (METER_TYPE) Rogowski Coil (CRC) current sensor */
#define   METER_TYPE_SENSOR_TYPE_I_B_VRD_Val         _UINT32_(0x3)                             /* (METER_TYPE) Resistive Divider (VRD) voltage sensor */
#define METER_TYPE_SENSOR_TYPE_V_B_Pos             _UINT32_(6)                                 /* (METER_TYPE) Voltage Sensor Type on A Phase Position */
#define METER_TYPE_SENSOR_TYPE_V_B_Msk             (_UINT32_(0x3) << METER_TYPE_SENSOR_TYPE_V_B_Pos)    /* (METER_TYPE) Voltage Sensor Type on A Phase Mask */
#define METER_TYPE_SENSOR_TYPE_V_B(value)          (METER_TYPE_SENSOR_TYPE_V_B_Msk & (_UINT32_(value) << METER_TYPE_SENSOR_TYPE_V_B_Pos))
#define   METER_TYPE_SENSOR_TYPE_V_B_CT_Val          _UINT32_(0x0)                             /* (METER_TYPE) Current Transformer */
#define   METER_TYPE_SENSOR_TYPE_V_B_SHUNT_Val       _UINT32_(0x1)                             /* (METER_TYPE) Resistive Shunt */
#define   METER_TYPE_SENSOR_TYPE_V_B_CRC_Val         _UINT32_(0x2)                             /* (METER_TYPE) Rogowski Coil (CRC) current sensor */
#define   METER_TYPE_SENSOR_TYPE_V_B_VRD_Val         _UINT32_(0x3)                             /* (METER_TYPE) Resistive Divider (VRD) voltage sensor */
#define METER_TYPE_SENSOR_TYPE_I_C_Pos             _UINT32_(8)                                 /* (METER_TYPE) Current Sensor Type on A Phase Position */
#define METER_TYPE_SENSOR_TYPE_I_C_Msk             (_UINT32_(0x3) << METER_TYPE_SENSOR_TYPE_I_C_Pos)    /* (METER_TYPE) Current Sensor Type on A Phase Mask */
#define METER_TYPE_SENSOR_TYPE_I_C(value)          (METER_TYPE_SENSOR_TYPE_I_C_Msk & (_UINT32_(value) << METER_TYPE_SENSOR_TYPE_I_C_Pos))
#define   METER_TYPE_SENSOR_TYPE_I_C_CT_Val          _UINT32_(0x0)                             /* (METER_TYPE) Current Transformer */
#define   METER_TYPE_SENSOR_TYPE_I_C_SHUNT_Val       _UINT32_(0x1)                             /* (METER_TYPE) Resistive Shunt */
#define   METER_TYPE_SENSOR_TYPE_I_C_CRC_Val         _UINT32_(0x2)                             /* (METER_TYPE) Rogowski Coil (CRC) current sensor */
#define   METER_TYPE_SENSOR_TYPE_I_C_VRD_Val         _UINT32_(0x3)                             /* (METER_TYPE) Resistive Divider (VRD) voltage sensor */
#define METER_TYPE_SENSOR_TYPE_V_C_Pos             _UINT32_(10)                                 /* (METER_TYPE) Voltage Sensor Type on A Phase Position */
#define METER_TYPE_SENSOR_TYPE_V_C_Msk             (_UINT32_(0x3) << METER_TYPE_SENSOR_TYPE_V_C_Pos)    /* (METER_TYPE) Voltage Sensor Type on A Phase Mask */
#define METER_TYPE_SENSOR_TYPE_V_C(value)          (METER_TYPE_SENSOR_TYPE_V_C_Msk & (_UINT32_(value) << METER_TYPE_SENSOR_TYPE_V_C_Pos))
#define   METER_TYPE_SENSOR_TYPE_V_C_CT_Val          _UINT32_(0x0)                             /* (METER_TYPE) Current Transformer */
#define   METER_TYPE_SENSOR_TYPE_V_C_SHUNT_Val       _UINT32_(0x1)                             /* (METER_TYPE) Resistive Shunt */
#define   METER_TYPE_SENSOR_TYPE_V_C_CRC_Val         _UINT32_(0x2)                             /* (METER_TYPE) Rogowski Coil (CRC) current sensor */
#define   METER_TYPE_SENSOR_TYPE_V_C_VRD_Val         _UINT32_(0x3)                             /* (METER_TYPE) Resistive Divider (VRD) voltage sensor */
#define METER_TYPE_SERVICE_TYPE_Pos                _UINT32_(24)                                 /* (METER_TYPE) Electrical Service Type Position */
#define METER_TYPE_SERVICE_TYPE_Msk                (_UINT32_(0xF) << METER_TYPE_SERVICE_TYPE_Pos)    /* (METER_TYPE) Electrical Service Type Mask */
#define METER_TYPE_SENSOR_TYPE(value)             (METER_TYPE_SERVICE_TYPE_Msk & (_UINT32_(value) << METER_TYPE_SERVICE_TYPE_Pos))
#define   METER_TYPE_SERVICE_TYPE_3P4WY_3E_3V3I_Val          _UINT32_(0x0)                      /* (METER_TYPE) Three-phase 4-wire Wye (3-element) */
#define   METER_TYPE_SERVICE_TYPE_3P4WD_3E_3V3I_Val          _UINT32_(0x1)                      /* (METER_TYPE) Three-phase 4-wire Delta (3-element) */
#define   METER_TYPE_SERVICE_TYPE_3P3P4WY_2p5E_2V3I_Val      _UINT32_(0x2)                      /* (METER_TYPE) Three-phase 4-wire Wye (2-½ element) */
#define   METER_TYPE_SERVICE_TYPE_3P4WD_2E_3V2I_Val          _UINT32_(0x3)                      /* (METER_TYPE) Three-phase 4-wire Delta (2-element) */
#define   METER_TYPE_SERVICE_TYPE_3P3WD_2E_2V2I_Val          _UINT32_(0x4)                      /* (METER_TYPE) Three-phase 3-wire) Delta (2-element) */
#define   METER_TYPE_SERVICE_TYPE_2P3W_2E_2V2I_Val           _UINT32_(0x5)                      /* (METER_TYPE) Two-phase 3-wire Network (2 element) */
#define   METER_TYPE_SERVICE_TYPE_1P3W_1p5E_1V2I_Val         _UINT32_(0x6)                      /* (METER_TYPE) Single-phase 3-wire (1-½ element) */
#define   METER_TYPE_SERVICE_TYPE_1P2W_1E_1V1I_Val           _UINT32_(0x7)                      /* (METER_TYPE) Single-phase 2-wire (1 element) */
#define   METER_TYPE_SERVICE_TYPE_1P3W_1E_2V1I_Val           _UINT32_(0x8)                      /* (METER_TYPE) Single-phase 3-wire (1 element) */
#define   METER_TYPE_SERVICE_TYPE_1P2W_1E_1V2I_I0_Val        _UINT32_(0x9)                      /* (METER_TYPE) I0 Single-phase 2-wire using I0 (1 element) */
#define   METER_TYPE_SERVICE_TYPE_2P3W_2E_2V2I_I0_Val        _UINT32_(0xA)                      /* (METER_TYPE) I0 Two-phase 3-wire Network using I0 (2 element) */
#define METER_TYPE_MISSING_PHASE_Pos               _UINT32_(28)                                 /* (METER_TYPE) Missing phase voltage select Position */
#define METER_TYPE_MISSING_PHASE_Msk               (_UINT32_(0x3) << METER_TYPE_MISSING_PHASE_Pos)    /* (METER_TYPE) Missing phase voltage select Mask */
#define METER_TYPE_MISSING_PHASE(value)            (METER_TYPE_MISSING_PHASE_Msk & (_UINT32_(value) << METER_TYPE_MISSING_PHASE_Pos))
#define   METER_TYPE_MISSING_PHASE_A_Val             _UINT32_(0x0)                             /* (METER_TYPE) Generate missing phase voltage for Phase_A */
#define   METER_TYPE_MISSING_PHASE_B_Val             _UINT32_(0x1)                             /* (METER_TYPE) Generate missing phase voltage for Phase_B */
#define   METER_TYPE_MISSING_PHASE_C_Val             _UINT32_(0x2)                             /* (METER_TYPE) Generate missing phase voltage for Phase_C */
#define METER_TYPE_Dto4WY_TRANSFORM_Pos            _UINT32_(31)                                 /* (METER_TYPE) Transformation to equivalent 4WY Position */
#define METER_TYPE_Dto4WY_TRANSFORM_Msk            (_UINT32_(0x1) << METER_TYPE_Dto4WY_TRANSFORM_Pos)    /* (METER_TYPE) Transformation to equivalent 4WY Mask */
#define METER_TYPE_Dto4WY_TRANSFORM(value)         (METER_TYPE_Dto4WY_TRANSFORM_Msk & (_UINT32_(value) << METER_TYPE_Dto4WY_TRANSFORM_Pos))
#define   METER_TYPE_Dto4WY_TRANSFORM_DISABLED_Val   _UINT32_(0x0)                             /* (METER_TYPE) Phase voltages remain in native service (no transformation) */
#define   METER_TYPE_Dto4WY_TRANSFORM_ENABLED_Val    _UINT32_(0x1)                             /* (METER_TYPE) Phase voltages transformed into an equivalent 4-wire Wye service */

/* -------- M : (MET_CONTROL Offset: 0x10) (R/W  32) Number of line cycles for integration period (uQ12.0) -------- */
#define M_Pos                                      _UINT32_(0)                                 /* (M) Number of cycles of the integration period Position */
#define M_Msk                                      (_UINT32_(0xFFF) << M_Pos)                  /* (M) Number of cycles of the integration period Mask */
#define M_Val(value)                               (M_Msk & (_UINT32_(value) << M_Pos))

/* -------- N_MAX : (MET_CONTROL Offset: 0x14) (R/W  32) Maximum number of samples in an integration period (uQ24.0) -------- */
#define N_MAX_Pos                                 _UINT32_(0)                                  /* (N_MAX) Maximum number of samples in an integration period Position */
#define N_MAX_Msk                                 (_UINT32_(0xFFF) << N_MAX_Pos)               /* (N_MAX) Maximum number of samples in an integration period Mask */
#define N_MAX_Val(value)                          (N_MAX_Msk & (_UINT32_(value) << N_MAX_Pos))

/* -------- PULSE0_CTRL : (MET_CONTROL Offset: 0x18) (R/W  32) Pulse 0 control: enable/disable, polarity, quantity type selection -------- */
#define PULSE0_CTRL_PC0_WIDTH_Pos                  _UINT32_(0)                                 /* (PULSE0_CTRL) Pulse Width  */
#define PULSE0_CTRL_PC0_WIDTH_Msk                  (_UINT32_(0xFFFF) << PULSE0_CTRL_PC0_WIDTH_Pos)      /* (PULSE0_CTRL) Pulse Width  */
#define PULSE0_CTRL_PC0_WIDTH(value)               (PULSE0_CTRL_PC0_WIDTH_Msk & (_UINT32_(value) << PULSE0_CTRL_PC0_WIDTH_Pos))
#define PULSE0_CTRL_PC0_TYPE_Pos                  _UINT32_(16)                                 /* (PULSE0_CTRL) Pulse Type Selection Position */
#define PULSE0_CTRL_PC0_TYPE_Msk                  (_UINT32_(0xF) << PULSE0_CTRL_PC0_TYPE_Pos)      /* (PULSE0_CTRL) Pulse Type Selection Mask */
#define PULSE0_CTRL_PC0_TYPE(value)               (PULSE0_CTRL_PC0_TYPE_Msk & (_UINT32_(value) << PULSE0_CTRL_PC0_TYPE_Pos))
#define   PULSE0_CTRL_PC0_TYPE_P_T_Val              _UINT32_(0x0)                             /* (PULSE0_CTRL) Watt-hours, total all phases, fundamental + harmonics */
#define   PULSE0_CTRL_PC0_TYPE_P_T_F_Val            _UINT32_(0x1)                             /* (PULSE0_CTRL) Watt-hours, total all phases, fundamental only */
#define   PULSE0_CTRL_PC0_TYPE_Q_T_Val              _UINT32_(0x2)                             /* (PULSE0_CTRL) Var-hours, total all phases, fundamental + harmonics */
#define   PULSE0_CTRL_PC0_TYPE_Q_T_F_Val            _UINT32_(0x3)                             /* (PULSE0_CTRL) Var-hours, total all phases, fundamental only */
#define   PULSE0_CTRL_PC0_TYPE_I_T_Val              _UINT32_(0x4)                             /* (PULSE0_CTRL) Amp-squared-hours, total all phases, fundamental + harmonics */
#define   PULSE0_CTRL_PC0_TYPE_I_T_F_Val            _UINT32_(0x5)                             /* (PULSE0_CTRL) Amp-squared-hours, total all phases, fundamental only */
#define PULSE0_CTRL_PC0_POLARITY_Pos               _UINT32_(24)                                 /* (PULSE0_CTRL) Pulse Polarity Position  */
#define PULSE0_CTRL_PC0_POLARITY_Msk               (_UINT32_(0x1) << PULSE0_CTRL_PC0_POLARITY_Pos)      /* (PULSE0_CTRL) Pulse Polarity Mask  */
#define PULSE0_CTRL_PC0_POLARITY(value)            (PULSE0_CTRL_PC0_POLARITY_Msk & (_UINT32_(value) << PULSE0_CTRL_PC0_POLARITY_Pos))
#define   PULSE0_CTRL_PC0_POLARITY_LOW_Val           _UINT32_(0x0)                             /* (PULSE0_CTRL) Output pulse is low with width set by K_WIDTH, followed by an inactive level of high */
#define   PULSE0_CTRL_PC0_POLARITY_HIGH_Val          _UINT32_(0x1)                             /* (PULSE0_CTRL) Output pulse is high with width set by K_WIDTH, followed by an inactive level of low */
#define PULSE0_CTRL_PC0_OVERRIDE_Pos               _UINT32_(25)                                 /* (PULSE0_CTRL) Pulse Override Control Position  */
#define PULSE0_CTRL_PC0_OVERRIDE_Msk               (_UINT32_(0x1) << PULSE0_CTRL_PC0_OVERRIDE_Pos)      /* (PULSE0_CTRL) Pulse Override Control Mask  */
#define PULSE0_CTRL_PC0_OVERRIDE(value)            (PULSE0_CTRL_PC0_OVERRIDE_Msk & (_UINT32_(value) << PULSE0_CTRL_PC0_OVERRIDE_Pos))
#define   PULSE0_CTRL_PC0_OVERRIDE_ENABLED_Val       _UINT32_(0x0)                             /* (PULSE0_CTRL) Pulse generation functions as normal */
#define   PULSE0_CTRL_PC0_OVERRIDE_DISABLED_Val      _UINT32_(0x1)                             /* (PULSE0_CTRL) Pulse generation is temporarily disabled */
#define PULSE0_CTRL_PC0_ACC_HOLD_Pos               _UINT32_(26)                                 /* (PULSE0_CTRL) Pulse Accumulation Hold Control Position  */
#define PULSE0_CTRL_PC0_ACC_HOLD_Msk               (_UINT32_(0x1) << PULSE0_CTRL_PC0_ACC_HOLD_Pos)      /* (PULSE0_CTRL) Pulse Accumulation Hold Control Mask  */
#define PULSE0_CTRL_PC0_ACC_HOLD(value)            (PULSE0_CTRL_PC0_ACC_HOLD_Msk & (_UINT32_(value) << PULSE0_CTRL_PC0_ACC_HOLD_Pos))
#define   PULSE0_CTRL_PC0_ACC_HOLD_DISABLED_Val      _UINT32_(0x0)                             /* (PULSE0_CTRL) Pulse accumulation registers continue to increment even if pulse generation is temporarily disabled */
#define   PULSE0_CTRL_PC0_ACC_HOLD_ENABLED_Val       _UINT32_(0x1)                             /* (PULSE0_CTRL) Pulse accumulation is held static */
#define PULSE0_CTRL_OPD_ENABLE_Pos                 _UINT32_(27)                                /* (PULSE0_CTRL) Open Drain Enable Position  */
#define PULSE0_CTRL_OPD_ENABLE_Msk                 (_UINT32_(0x1) << PULSE0_CTRL_OPD_ENABLE_Pos)   /* (PULSE0_CTRL) Open Drain Enable Mask  */
#define PULSE0_CTRL_OPD_ENABLE(value)              (PULSE0_CTRL_OPD_ENABLE_Msk & (_UINT32_(value) << PULSE0_CTRL_OPD_ENABLE_Pos))
#define   PULSE0_CTRL_OPD_ENABLE_DISABLED_Val        _UINT32_(0x0)                             /* (PULSE0_CTRL) The open drain is disabled */
#define   PULSE0_CTRL_OPD_ENABLE_ENABLED_Val         _UINT32_(0x1)                             /* (PULSE0_CTRL) The open drain is enabled */
#define PULSE0_CTRL_PC0_DETENT_Pos                 _UINT32_(28)                                /* (PULSE0_CTRL) Total Absolute Values for P and Q values Position  */
#define PULSE0_CTRL_PC0_DETENT_Msk                 (_UINT32_(0x3) << PULSE0_CTRL_PC0_DETENT_Pos)   /* (PULSE0_CTRL) Total Absolute Values for P and Q values Mask  */
#define PULSE0_CTRL_PC0_DETENT(value)              (PULSE0_CTRL_PC0_DETENT_Msk & (_UINT32_(value) << PULSE0_CTRL_PC0_DETENT_Pos))
#define   PULSE0_CTRL_PC0_DETENT_NET_Val             _UINT32_(0x0)                             /* (PULSE0_CTRL) Pulses generated based on (energy delivered ? energy generated) */
#define   PULSE0_CTRL_PC0_DETENT_ABSOLUTE_Val        _UINT32_(0x1)                             /* (PULSE0_CTRL) Pulses generated based on (energy delivered + energy generated) */
#define   PULSE0_CTRL_PC0_DETENT_DELIVERED_Val       _UINT32_(0x2)                             /* (PULSE0_CTRL) Pulses generated based on (energy delivered only) */
#define   PULSE0_CTRL_PC0_DETENT_GENERATED_Val       _UINT32_(0x3)                             /* (PULSE0_CTRL) Pulses generated based on (energy generated only) */
#define PULSE0_CTRL_PU_ENABLE_Pos                  _UINT32_(30)                                /* (PULSE0_CTRL) Pull-Up Enable Position  */
#define PULSE0_CTRL_PU_ENABLE_Msk                  (_UINT32_(0x1) << PULSE0_CTRL_PU_ENABLE_Pos)   /* (PULSE0_CTRL) Pull-Up Enable Mask  */
#define PULSE0_CTRL_PU_ENABLE(value)               (PULSE0_CTRL_PU_ENABLE_Msk & (_UINT32_(value) << PULSE0_CTRL_PU_ENABLE_Pos))
#define   PULSE0_CTRL_PU_ENABLE_DISABLED_Val         _UINT32_(0x0)                             /* (PULSE0_CTRL) Pull-Up disabled */
#define   PULSE0_CTRL_PU_ENABLE_ENABLED_Val          _UINT32_(0x1)                             /* (PULSE0_CTRL) Pull-Up enabled */
#define PULSE0_CTRL_PC0_ENABLE_Pos                 _UINT32_(31)                                /* (PULSE0_CTRL) Output Pulse Enable Position  */
#define PULSE0_CTRL_PC0_ENABLE_Msk                 (_UINT32_(0x1) << PULSE0_CTRL_PC0_ENABLE_Pos)   /* (PULSE0_CTRL) Output Pulse Enable Mask  */
#define PULSE0_CTRL_PC0_ENABLE(value)              (PULSE0_CTRL_PC0_ENABLE_Msk & (_UINT32_(value) << PULSE0_CTRL_PC0_ENABLE_Pos))
#define   PULSE0_CTRL_PC0_ENABLE_DISABLED_Val        _UINT32_(0x0)                             /* (PULSE0_CTRL) Pulse output disabled, inactive level determined by PULSE_POLARITY */
#define   PULSE0_CTRL_PC0_ENABLE_ENABLED_Val         _UINT32_(0x1)                             /* (PULSE0_CTRL) Pulse output enabled */

/* -------- PULSE1_CTRL : (MET_CONTROL Offset: 0x18) (R/W  32) Pulse 1 control: enable/disable, polarity, quantity type selection -------- */
#define PULSE1_CTRL_PC1_WIDTH_Pos                  _UINT32_(0)                                 /* (PULSE1_CTRL) Pulse Width  */
#define PULSE1_CTRL_PC1_WIDTH_Msk                  (_UINT32_(0xFFFF) << PULSE1_CTRL_PC1_WIDTH_Pos)      /* (PULSE1_CTRL) Pulse Width  */
#define PULSE1_CTRL_PC1_WIDTH(value)               (PULSE1_CTRL_PC1_WIDTH_Msk & (_UINT32_(value) << PULSE1_CTRL_PC1_WIDTH_Pos))
#define PULSE1_CTRL_PC1_TYPE_Pos                  _UINT32_(16)                                 /* (PULSE1_CTRL) Pulse Type Selection Position */
#define PULSE1_CTRL_PC1_TYPE_Msk                  (_UINT32_(0xF) << PULSE1_CTRL_PC1_TYPE_Pos)      /* (PULSE1_CTRL) Pulse Type Selection Mask */
#define PULSE1_CTRL_PC1_TYPE(value)               (PULSE1_CTRL_PC1_TYPE_Msk & (_UINT32_(value) << PULSE1_CTRL_PC1_TYPE_Pos))
#define   PULSE1_CTRL_PC1_TYPE_P_T_Val              _UINT32_(0x0)                             /* (PULSE1_CTRL) Watt-hours, total all phases, fundamental + harmonics */
#define   PULSE1_CTRL_PC1_TYPE_P_T_F_Val            _UINT32_(0x1)                             /* (PULSE1_CTRL) Watt-hours, total all phases, fundamental only */
#define   PULSE1_CTRL_PC1_TYPE_Q_T_Val              _UINT32_(0x2)                             /* (PULSE1_CTRL) Var-hours, total all phases, fundamental + harmonics */
#define   PULSE1_CTRL_PC1_TYPE_Q_T_F_Val            _UINT32_(0x3)                             /* (PULSE1_CTRL) Var-hours, total all phases, fundamental only */
#define   PULSE1_CTRL_PC1_TYPE_I_T_Val              _UINT32_(0x4)                             /* (PULSE1_CTRL) Amp-squared-hours, total all phases, fundamental + harmonics */
#define   PULSE1_CTRL_PC1_TYPE_I_T_F_Val            _UINT32_(0x5)                             /* (PULSE1_CTRL) Amp-squared-hours, total all phases, fundamental only */
#define PULSE1_CTRL_PC1_POLARITY_Pos               _UINT32_(24)                                 /* (PULSE1_CTRL) Pulse Polarity Position  */
#define PULSE1_CTRL_PC1_POLARITY_Msk               (_UINT32_(0x1) << PULSE1_CTRL_PC1_POLARITY_Pos)      /* (PULSE1_CTRL) Pulse Polarity Mask  */
#define PULSE1_CTRL_PC1_POLARITY(value)            (PULSE1_CTRL_PC1_POLARITY_Msk & (_UINT32_(value) << PULSE1_CTRL_PC1_POLARITY_Pos))
#define   PULSE1_CTRL_PC1_POLARITY_LOW_Val           _UINT32_(0x0)                             /* (PULSE1_CTRL) Output pulse is low with width set by K_WIDTH, followed by an inactive level of high */
#define   PULSE1_CTRL_PC1_POLARITY_HIGH_Val          _UINT32_(0x1)                             /* (PULSE1_CTRL) Output pulse is high with width set by K_WIDTH, followed by an inactive level of low */
#define PULSE1_CTRL_PC1_OVERRIDE_Pos               _UINT32_(25)                                 /* (PULSE1_CTRL) Pulse Override Control Position  */
#define PULSE1_CTRL_PC1_OVERRIDE_Msk               (_UINT32_(0x1) << PULSE1_CTRL_PC1_OVERRIDE_Pos)      /* (PULSE1_CTRL) Pulse Override Control Mask  */
#define PULSE1_CTRL_PC1_OVERRIDE(value)            (PULSE1_CTRL_PC1_OVERRIDE_Msk & (_UINT32_(value) << PULSE1_CTRL_PC1_OVERRIDE_Pos))
#define   PULSE1_CTRL_PC1_OVERRIDE_ENABLED_Val       _UINT32_(0x0)                             /* (PULSE1_CTRL) Pulse generation functions as normal */
#define   PULSE1_CTRL_PC1_OVERRIDE_DISABLED_Val      _UINT32_(0x1)                             /* (PULSE1_CTRL) Pulse generation is temporarily disabled */
#define PULSE1_CTRL_PC1_ACC_HOLD_Pos               _UINT32_(26)                                 /* (PULSE1_CTRL) Pulse Accumulation Hold Control Position  */
#define PULSE1_CTRL_PC1_ACC_HOLD_Msk               (_UINT32_(0x1) << PULSE1_CTRL_PC1_ACC_HOLD_Pos)      /* (PULSE1_CTRL) Pulse Accumulation Hold Control Mask  */
#define PULSE1_CTRL_PC1_ACC_HOLD(value)            (PULSE1_CTRL_PC1_ACC_HOLD_Msk & (_UINT32_(value) << PULSE1_CTRL_PC1_ACC_HOLD_Pos))
#define   PULSE1_CTRL_PC1_ACC_HOLD_DISABLED_Val      _UINT32_(0x0)                             /* (PULSE1_CTRL) Pulse accumulation registers continue to increment even if pulse generation is temporarily disabled */
#define   PULSE1_CTRL_PC1_ACC_HOLD_ENABLED_Val       _UINT32_(0x1)                             /* (PULSE1_CTRL) Pulse accumulation is held static */
#define PULSE1_CTRL_OPD_ENABLE_Pos                 _UINT32_(27)                                /* (PULSE1_CTRL) Open Drain Enable Position  */
#define PULSE1_CTRL_OPD_ENABLE_Msk                 (_UINT32_(0x1) << PULSE1_CTRL_OPD_ENABLE_Pos)   /* (PULSE1_CTRL) Open Drain Enable Mask  */
#define PULSE1_CTRL_OPD_ENABLE(value)              (PULSE1_CTRL_OPD_ENABLE_Msk & (_UINT32_(value) << PULSE1_CTRL_OPD_ENABLE_Pos))
#define   PULSE1_CTRL_OPD_ENABLE_DISABLED_Val        _UINT32_(0x0)                             /* (PULSE1_CTRL) The open drain is disabled */
#define   PULSE1_CTRL_OPD_ENABLE_ENABLED_Val         _UINT32_(0x1)                             /* (PULSE1_CTRL) The open drain is enabled */
#define PULSE1_CTRL_PC1_DETENT_Pos                 _UINT32_(28)                                /* (PULSE1_CTRL) Total Absolute Values for P and Q values Position  */
#define PULSE1_CTRL_PC1_DETENT_Msk                 (_UINT32_(0x3) << PULSE1_CTRL_PC1_DETENT_Pos)   /* (PULSE1_CTRL) Total Absolute Values for P and Q values Mask  */
#define PULSE1_CTRL_PC1_DETENT(value)              (PULSE1_CTRL_PC1_DETENT_Msk & (_UINT32_(value) << PULSE1_CTRL_PC1_DETENT_Pos))
#define   PULSE1_CTRL_PC1_DETENT_NET_Val             _UINT32_(0x0)                             /* (PULSE1_CTRL) Pulses generated based on (energy delivered ? energy generated) */
#define   PULSE1_CTRL_PC1_DETENT_ABSOLUTE_Val        _UINT32_(0x1)                             /* (PULSE1_CTRL) Pulses generated based on (energy delivered + energy generated) */
#define   PULSE1_CTRL_PC1_DETENT_DELIVERED_Val       _UINT32_(0x2)                             /* (PULSE1_CTRL) Pulses generated based on (energy delivered only) */
#define   PULSE1_CTRL_PC1_DETENT_GENERATED_Val       _UINT32_(0x3)                             /* (PULSE1_CTRL) Pulses generated based on (energy generated only) */
#define PULSE1_CTRL_PU_ENABLE_Pos                  _UINT32_(30)                                /* (PULSE1_CTRL) Pull-Up Enable Position  */
#define PULSE1_CTRL_PU_ENABLE_Msk                  (_UINT32_(0x1) << PULSE1_CTRL_PU_ENABLE_Pos)   /* (PULSE1_CTRL) Pull-Up Enable Mask  */
#define PULSE1_CTRL_PU_ENABLE(value)               (PULSE1_CTRL_PU_ENABLE_Msk & (_UINT32_(value) << PULSE1_CTRL_PU_ENABLE_Pos))
#define   PULSE1_CTRL_PU_ENABLE_DISABLED_Val         _UINT32_(0x0)                             /* (PULSE1_CTRL) Pull-Up disabled */
#define   PULSE1_CTRL_PU_ENABLE_ENABLED_Val          _UINT32_(0x1)                             /* (PULSE1_CTRL) Pull-Up enabled */
#define PULSE1_CTRL_PC1_ENABLE_Pos                 _UINT32_(31)                                /* (PULSE1_CTRL) Output Pulse Enable Position  */
#define PULSE1_CTRL_PC1_ENABLE_Msk                 (_UINT32_(0x1) << PULSE1_CTRL_PC1_ENABLE_Pos)   /* (PULSE1_CTRL) Output Pulse Enable Mask  */
#define PULSE1_CTRL_PC1_ENABLE(value)              (PULSE1_CTRL_PC1_ENABLE_Msk & (_UINT32_(value) << PULSE1_CTRL_PC1_ENABLE_Pos))
#define   PULSE1_CTRL_PC1_ENABLE_DISABLED_Val        _UINT32_(0x0)                             /* (PULSE1_CTRL) Pulse output disabled, inactive level determined by PULSE_POLARITY */
#define   PULSE1_CTRL_PC1_ENABLE_ENABLED_Val         _UINT32_(0x1)                             /* (PULSE1_CTRL) Pulse output enabled */

/* -------- PULSE2_CTRL : (MET_CONTROL Offset: 0x18) (R/W  32) Pulse 2 control: enable/disable, polarity, quantity type selection -------- */
#define PULSE2_CTRL_PC2_WIDTH_Pos                  _UINT32_(0)                                 /* (PULSE2_CTRL) Pulse Width  */
#define PULSE2_CTRL_PC2_WIDTH_Msk                  (_UINT32_(0xFFFF) << PULSE2_CTRL_PC2_WIDTH_Pos)      /* (PULSE2_CTRL) Pulse Width  */
#define PULSE2_CTRL_PC2_WIDTH(value)               (PULSE2_CTRL_PC2_WIDTH_Msk & (_UINT32_(value) << PULSE2_CTRL_PC2_WIDTH_Pos))
#define PULSE2_CTRL_PC2_TYPE_Pos                  _UINT32_(16)                                 /* (PULSE2_CTRL) Pulse Type Selection Position */
#define PULSE2_CTRL_PC2_TYPE_Msk                  (_UINT32_(0xF) << PULSE2_CTRL_PC2_TYPE_Pos)      /* (PULSE2_CTRL) Pulse Type Selection Mask */
#define PULSE2_CTRL_PC2_TYPE(value)               (PULSE2_CTRL_PC2_TYPE_Msk & (_UINT32_(value) << PULSE2_CTRL_PC2_TYPE_Pos))
#define   PULSE2_CTRL_PC2_TYPE_P_T_Val              _UINT32_(0x0)                             /* (PULSE2_CTRL) Watt-hours, total all phases, fundamental + harmonics */
#define   PULSE2_CTRL_PC2_TYPE_P_T_F_Val            _UINT32_(0x1)                             /* (PULSE2_CTRL) Watt-hours, total all phases, fundamental only */
#define   PULSE2_CTRL_PC2_TYPE_Q_T_Val              _UINT32_(0x2)                             /* (PULSE2_CTRL) Var-hours, total all phases, fundamental + harmonics */
#define   PULSE2_CTRL_PC2_TYPE_Q_T_F_Val            _UINT32_(0x3)                             /* (PULSE2_CTRL) Var-hours, total all phases, fundamental only */
#define   PULSE2_CTRL_PC2_TYPE_I_T_Val              _UINT32_(0x4)                             /* (PULSE2_CTRL) Amp-squared-hours, total all phases, fundamental + harmonics */
#define   PULSE2_CTRL_PC2_TYPE_I_T_F_Val            _UINT32_(0x5)                             /* (PULSE2_CTRL) Amp-squared-hours, total all phases, fundamental only */
#define PULSE2_CTRL_PC2_POLARITY_Pos               _UINT32_(24)                                 /* (PULSE2_CTRL) Pulse Polarity Position  */
#define PULSE2_CTRL_PC2_POLARITY_Msk               (_UINT32_(0x1) << PULSE2_CTRL_PC2_POLARITY_Pos)      /* (PULSE2_CTRL) Pulse Polarity Mask  */
#define PULSE2_CTRL_PC2_POLARITY(value)            (PULSE2_CTRL_PC2_POLARITY_Msk & (_UINT32_(value) << PULSE2_CTRL_PC2_POLARITY_Pos))
#define   PULSE2_CTRL_PC2_POLARITY_LOW_Val           _UINT32_(0x0)                             /* (PULSE2_CTRL) Output pulse is low with width set by K_WIDTH, followed by an inactive level of high */
#define   PULSE2_CTRL_PC2_POLARITY_HIGH_Val          _UINT32_(0x1)                             /* (PULSE2_CTRL) Output pulse is high with width set by K_WIDTH, followed by an inactive level of low */
#define PULSE2_CTRL_PC2_OVERRIDE_Pos               _UINT32_(25)                                 /* (PULSE2_CTRL) Pulse Override Control Position  */
#define PULSE2_CTRL_PC2_OVERRIDE_Msk               (_UINT32_(0x1) << PULSE2_CTRL_PC2_OVERRIDE_Pos)      /* (PULSE2_CTRL) Pulse Override Control Mask  */
#define PULSE2_CTRL_PC2_OVERRIDE(value)            (PULSE2_CTRL_PC2_OVERRIDE_Msk & (_UINT32_(value) << PULSE2_CTRL_PC2_OVERRIDE_Pos))
#define   PULSE2_CTRL_PC2_OVERRIDE_ENABLED_Val       _UINT32_(0x0)                             /* (PULSE2_CTRL) Pulse generation functions as normal */
#define   PULSE2_CTRL_PC2_OVERRIDE_DISABLED_Val      _UINT32_(0x1)                             /* (PULSE2_CTRL) Pulse generation is temporarily disabled */
#define PULSE2_CTRL_PC2_ACC_HOLD_Pos               _UINT32_(26)                                 /* (PULSE2_CTRL) Pulse Accumulation Hold Control Position  */
#define PULSE2_CTRL_PC2_ACC_HOLD_Msk               (_UINT32_(0x1) << PULSE2_CTRL_PC2_ACC_HOLD_Pos)      /* (PULSE2_CTRL) Pulse Accumulation Hold Control Mask  */
#define PULSE2_CTRL_PC2_ACC_HOLD(value)            (PULSE2_CTRL_PC2_ACC_HOLD_Msk & (_UINT32_(value) << PULSE2_CTRL_PC2_ACC_HOLD_Pos))
#define   PULSE2_CTRL_PC2_ACC_HOLD_DISABLED_Val      _UINT32_(0x0)                             /* (PULSE2_CTRL) Pulse accumulation registers continue to increment even if pulse generation is temporarily disabled */
#define   PULSE2_CTRL_PC2_ACC_HOLD_ENABLED_Val       _UINT32_(0x1)                             /* (PULSE2_CTRL) Pulse accumulation is held static */
#define PULSE2_CTRL_OPD_ENABLE_Pos                 _UINT32_(27)                                /* (PULSE2_CTRL) Open Drain Enable Position  */
#define PULSE2_CTRL_OPD_ENABLE_Msk                 (_UINT32_(0x1) << PULSE2_CTRL_OPD_ENABLE_Pos)   /* (PULSE2_CTRL) Open Drain Enable Mask  */
#define PULSE2_CTRL_OPD_ENABLE(value)              (PULSE2_CTRL_OPD_ENABLE_Msk & (_UINT32_(value) << PULSE2_CTRL_OPD_ENABLE_Pos))
#define   PULSE2_CTRL_OPD_ENABLE_DISABLED_Val        _UINT32_(0x0)                             /* (PULSE2_CTRL) The open drain is disabled */
#define   PULSE2_CTRL_OPD_ENABLE_ENABLED_Val         _UINT32_(0x1)                             /* (PULSE2_CTRL) The open drain is enabled */
#define PULSE2_CTRL_PC2_DETENT_Pos                 _UINT32_(28)                                /* (PULSE2_CTRL) Total Absolute Values for P and Q values Position  */
#define PULSE2_CTRL_PC2_DETENT_Msk                 (_UINT32_(0x3) << PULSE2_CTRL_PC2_DETENT_Pos)   /* (PULSE2_CTRL) Total Absolute Values for P and Q values Mask  */
#define PULSE2_CTRL_PC2_DETENT(value)              (PULSE2_CTRL_PC2_DETENT_Msk & (_UINT32_(value) << PULSE2_CTRL_PC2_DETENT_Pos))
#define   PULSE2_CTRL_PC2_DETENT_NET_Val             _UINT32_(0x0)                             /* (PULSE2_CTRL) Pulses generated based on (energy delivered ? energy generated) */
#define   PULSE2_CTRL_PC2_DETENT_ABSOLUTE_Val        _UINT32_(0x1)                             /* (PULSE2_CTRL) Pulses generated based on (energy delivered + energy generated) */
#define   PULSE2_CTRL_PC2_DETENT_DELIVERED_Val       _UINT32_(0x2)                             /* (PULSE2_CTRL) Pulses generated based on (energy delivered only) */
#define   PULSE2_CTRL_PC2_DETENT_GENERATED_Val       _UINT32_(0x3)                             /* (PULSE2_CTRL) Pulses generated based on (energy generated only) */
#define PULSE2_CTRL_PU_ENABLE_Pos                  _UINT32_(30)                                /* (PULSE2_CTRL) Pull-Up Enable Position  */
#define PULSE2_CTRL_PU_ENABLE_Msk                  (_UINT32_(0x1) << PULSE2_CTRL_PU_ENABLE_Pos)   /* (PULSE2_CTRL) Pull-Up Enable Mask  */
#define PULSE2_CTRL_PU_ENABLE(value)               (PULSE2_CTRL_PU_ENABLE_Msk & (_UINT32_(value) << PULSE2_CTRL_PU_ENABLE_Pos))
#define   PULSE2_CTRL_PU_ENABLE_DISABLED_Val         _UINT32_(0x0)                             /* (PULSE2_CTRL) Pull-Up disabled */
#define   PULSE2_CTRL_PU_ENABLE_ENABLED_Val          _UINT32_(0x1)                             /* (PULSE2_CTRL) Pull-Up enabled */
#define PULSE2_CTRL_PC2_ENABLE_Pos                 _UINT32_(31)                                /* (PULSE2_CTRL) Output Pulse Enable Position  */
#define PULSE2_CTRL_PC2_ENABLE_Msk                 (_UINT32_(0x1) << PULSE2_CTRL_PC2_ENABLE_Pos)   /* (PULSE2_CTRL) Output Pulse Enable Mask  */
#define PULSE2_CTRL_PC2_ENABLE(value)              (PULSE2_CTRL_PC2_ENABLE_Msk & (_UINT32_(value) << PULSE2_CTRL_PC2_ENABLE_Pos))
#define   PULSE2_CTRL_PC2_ENABLE_DISABLED_Val        _UINT32_(0x0)                             /* (PULSE2_CTRL) Pulse output disabled, inactive level determined by PULSE_POLARITY */
#define   PULSE2_CTRL_PC2_ENABLE_ENABLED_Val         _UINT32_(0x1)                             /* (PULSE2_CTRL) Pulse output enabled */

/* -------- P_K_t : (MET_CONTROL Offset: 0x24) (R/W  32) Meter Active Power Pulse Constant (uQ8.24) -------- */
#define P_K_t_Pos                                  _UINT32_(0)                                 /* (P_K_t) Meter Active Power Pulse Constant Position */
#define P_K_t_Msk                                  (_UINT32_(0xFFFFFFFF) << P_K_t_Pos)         /* (P_K_t) Meter Active Power Pulse Constant Mask */
#define P_K_t_Val(value)                           (P_K_t_Msk & (_UINT32_(value) << P_K_t_Pos))

/* -------- Q_K_t : (MET_CONTROL Offset: 0x28) (R/W  32) Meter Reactive Power Pulse Constant (uQ8.24) -------- */
#define Q_K_t_Pos                                  _UINT32_(0)                                 /* (Q_K_t) Meter Reactive Power Pulse Constantt Position */
#define Q_K_t_Msk                                  (_UINT32_(0xFFFFFFFF) << Q_K_t_Pos)         /* (Q_K_t) Meter Reactive Power Pulse Constant Mask */
#define Q_K_t_Val(value)                           (Q_K_t_Msk & (_UINT32_(value) << Q_K_t_Pos))

/* -------- I_K_t : (MET_CONTROL Offset: 0x2C) (R/W  32) Meter Current Pulse Constant (uQ8.24) -------- */
#define I_K_t_Pos                                  _UINT32_(0)                                 /* (I_K_t) Meter Current Pulse Constant Position */
#define I_K_t_Msk                                  (_UINT32_(0xFFFFFFFF) << I_K_t_Pos)         /* (I_K_t) Meter Current Pulse Constant Mask */
#define I_K_t_Val(value)                           (I_K_t_Msk & (_UINT32_(value) << I_K_t_Pos))

/* -------- CREEP_THRESHOLD_P : (MET_CONTROL Offset: 0x30) (R/W  32) Creep Threshold for Active Energy (uQ2.30) -------- */
#define CREEP_THRESHOLD_P_Pos                      _UINT32_(0)                                 /* (CREEP_THRESHOLD_P) Creep Threshold for Active Energy Position */
#define CREEP_THRESHOLD_P_Msk                      (_UINT32_(0xFFFFFFFF) << CREEP_THRESHOLD_P_Pos)         /* (CREEP_THRESHOLD_P) Creep Threshold for Active Energy Mask */
#define CREEP_THRESHOLD_P_Val(value)               (CREEP_THRESHOLD_P_Msk & (_UINT32_(value) << CREEP_THRESHOLD_P_Pos))

/* -------- CREEP_THRESHOLD_Q : (MET_CONTROL Offset: 0x34) (R/W  32) Creep Threshold for Quadenergy (uQ2.30) -------- */
#define CREEP_THRESHOLD_Q_Pos                      _UINT32_(0)                                 /* (CREEP_THRESHOLD_Q) Creep Threshold for Quadenergy Position */
#define CREEP_THRESHOLD_Q_Msk                      (_UINT32_(0xFFFFFFFF) << CREEP_THRESHOLD_Q_Pos)         /* (CREEP_THRESHOLD_Q) Creep Threshold for Quadenergy Mask */
#define CREEP_THRESHOLD_Q_Val(value)               (CREEP_THRESHOLD_Q_Msk & (_UINT32_(value) << CREEP_THRESHOLD_Q_Pos))

/* -------- CREEP_THRESHOLD_I : (MET_CONTROL Offset: 0x38) (R/W  32) Creep Threshold of Phase Current (uQ2.30) -------- */
#define CREEP_THRESHOLD_I_Pos                      _UINT32_(0)                                 /* (CREEP_THRESHOLD_I) Creep Threshold of Phase Current Position */
#define CREEP_THRESHOLD_I_Msk                      (_UINT32_(0xFFFFFFFF) << CREEP_THRESHOLD_I_Pos)         /* (CREEP_THRESHOLD_I) Creep Threshold of Phase Current Mask */
#define CREEP_THRESHOLD_I_Val(value)               (CREEP_THRESHOLD_I_Msk & (_UINT32_(value) << CREEP_THRESHOLD_I_Pos))

/* -------- POWER_OFFSET_CTRL : (MET_CONTROL Offset: 0x3C) (R/W  32) Power offset control -------- */
#define POWER_OFFSET_CTRL_Q_OFFSET_ACC_A_Pos       _UINT32_(16)                                 /* (POWER_OFFSET_CTRL) Reactive Power Offset for Accumulator Quantities Enable Position */
#define POWER_OFFSET_CTRL_Q_OFFSET_ACC_A_Msk       (_UINT32_(0x1) << POWER_OFFSET_CTRL_Q_OFFSET_ACC_A_Pos)         /* (POWER_OFFSET_CTRL) Reactive Power Offset for Accumulator Quantities Enable Mask */
#define POWER_OFFSET_CTRL_Q_OFFSET_ACC_A(value)    (POWER_OFFSET_CTRL_Q_OFFSET_ACC_A_Msk & (_UINT32_(value) << POWER_OFFSET_CTRL_Q_OFFSET_ACC_A_Pos))
#define   POWER_OFFSET_CTRL_Q_OFFSET_ACC_A_DISABLED_Val        _UINT32_(0x0)                   /* (POWER_OFFSET_CTRL) Reactive Power Offset disabled */
#define   POWER_OFFSET_CTRL_Q_OFFSET_ACC_A_ENABLED_Val         _UINT32_(0x1)                   /* (POWER_OFFSET_CTRL) Reactive Power Offset enabled */
#define POWER_OFFSET_CTRL_Q_OFFSET_ACC_B_Pos       _UINT32_(17)                                /* (POWER_OFFSET_CTRL) Reactive Power Offset for Accumulator Quantities Enable Position */
#define POWER_OFFSET_CTRL_Q_OFFSET_ACC_B_Msk       (_UINT32_(0x1) << POWER_OFFSET_CTRL_Q_OFFSET_ACC_B_Pos)         /* (POWER_OFFSET_CTRL) Reactive Power Offset for Accumulator Quantities Enable Mask */
#define POWER_OFFSET_CTRL_Q_OFFSET_ACC_B(value)    (POWER_OFFSET_CTRL_Q_OFFSET_ACC_B_Msk & (_UINT32_(value) << POWER_OFFSET_CTRL_Q_OFFSET_ACC_B_Pos))
#define   POWER_OFFSET_CTRL_Q_OFFSET_ACC_B_DISABLED_Val        _UINT32_(0x0)                   /* (POWER_OFFSET_CTRL) Reactive Power Offset disabled */
#define   POWER_OFFSET_CTRL_Q_OFFSET_ACC_B_ENABLED_Val         _UINT32_(0x1)                   /* (POWER_OFFSET_CTRL) Reactive Power Offset enabled */
#define POWER_OFFSET_CTRL_Q_OFFSET_ACC_C_Pos       _UINT32_(18)                                /* (POWER_OFFSET_CTRL) Reactive Power Offset for Accumulator Quantities Enable Position */
#define POWER_OFFSET_CTRL_Q_OFFSET_ACC_C_Msk       (_UINT32_(0x1) << POWER_OFFSET_CTRL_Q_OFFSET_ACC_C_Pos)         /* (POWER_OFFSET_CTRL) Reactive Power Offset for Accumulator Quantities Enable Mask */
#define POWER_OFFSET_CTRL_Q_OFFSET_ACC_C(value)    (POWER_OFFSET_CTRL_Q_OFFSET_ACC_C_Msk & (_UINT32_(value) << POWER_OFFSET_CTRL_Q_OFFSET_ACC_C_Pos))
#define   POWER_OFFSET_CTRL_Q_OFFSET_ACC_C_DISABLED_Val        _UINT32_(0x0)                   /* (POWER_OFFSET_CTRL) Reactive Power Offset disabled */
#define   POWER_OFFSET_CTRL_Q_OFFSET_ACC_C_ENABLED_Val         _UINT32_(0x1)                   /* (POWER_OFFSET_CTRL) Reactive Power Offset enabled */
#define POWER_OFFSET_CTRL_P_OFFSET_ACC_A_Pos       _UINT32_(20)                                /* (POWER_OFFSET_CTRL) Active Power Offset for Accumulator Quantities Enable Position */
#define POWER_OFFSET_CTRL_P_OFFSET_ACC_A_Msk       (_UINT32_(0x1) << POWER_OFFSET_CTRL_P_OFFSET_ACC_A_Pos)         /* (POWER_OFFSET_CTRL) Active Power Offset for Accumulator Quantities Enable Mask */
#define POWER_OFFSET_CTRL_P_OFFSET_ACC_A(value)    (POWER_OFFSET_CTRL_P_OFFSET_ACC_A_Msk & (_UINT32_(value) << POWER_OFFSET_CTRL_P_OFFSET_ACC_A_Pos))
#define   POWER_OFFSET_CTRL_P_OFFSET_ACC_A_DISABLED_Val        _UINT32_(0x0)                   /* (POWER_OFFSET_CTRL) Active Power Offset disabled */
#define   POWER_OFFSET_CTRL_P_OFFSET_ACC_A_ENABLED_Val         _UINT32_(0x1)                   /* (POWER_OFFSET_CTRL) Active Power Offset enabled */
#define POWER_OFFSET_CTRL_P_OFFSET_ACC_B_Pos       _UINT32_(21)                                /* (POWER_OFFSET_CTRL) Active Power Offset for Accumulator Quantities Enable Position */
#define POWER_OFFSET_CTRL_P_OFFSET_ACC_B_Msk       (_UINT32_(0x1) << POWER_OFFSET_CTRL_P_OFFSET_ACC_B_Pos)         /* (POWER_OFFSET_CTRL) Active Power Offset for Accumulator Quantities Enable Mask */
#define POWER_OFFSET_CTRL_P_OFFSET_ACC_B(value)    (POWER_OFFSET_CTRL_P_OFFSET_ACC_B_Msk & (_UINT32_(value) << POWER_OFFSET_CTRL_P_OFFSET_ACC_B_Pos))
#define   POWER_OFFSET_CTRL_P_OFFSET_ACC_B_DISABLED_Val        _UINT32_(0x0)                   /* (POWER_OFFSET_CTRL) Active Power Offset disabled */
#define   POWER_OFFSET_CTRL_P_OFFSET_ACC_B_ENABLED_Val         _UINT32_(0x1)                   /* (POWER_OFFSET_CTRL) Active Power Offset enabled */
#define POWER_OFFSET_CTRL_P_OFFSET_ACC_C_Pos       _UINT32_(22)                                /* (POWER_OFFSET_CTRL) Active Power Offset for Accumulator Quantities Enable Position */
#define POWER_OFFSET_CTRL_P_OFFSET_ACC_C_Msk       (_UINT32_(0x1) << POWER_OFFSET_CTRL_P_OFFSET_ACC_C_Pos)         /* (POWER_OFFSET_CTRL) Active Power Offset for Accumulator Quantities Enable Mask */
#define POWER_OFFSET_CTRL_P_OFFSET_ACC_C(value)    (POWER_OFFSET_CTRL_P_OFFSET_ACC_C_Msk & (_UINT32_(value) << POWER_OFFSET_CTRL_P_OFFSET_ACC_C_Pos))
#define   POWER_OFFSET_CTRL_P_OFFSET_ACC_C_DISABLED_Val        _UINT32_(0x0)                   /* (POWER_OFFSET_CTRL) Active Power Offset disabled */
#define   POWER_OFFSET_CTRL_P_OFFSET_ACC_C_ENABLED_Val         _UINT32_(0x1)                   /* (POWER_OFFSET_CTRL) Active Power Offset enabled */
#define POWER_OFFSET_CTRL_Q_OFFSET_PUL_Pos         _UINT32_(27)                                /* (POWER_OFFSET_CTRL) Reactive Power Offset for Pulse Quantities Enable Position */
#define POWER_OFFSET_CTRL_Q_OFFSET_PUL_Msk         (_UINT32_(0x1) << POWER_OFFSET_Q_OFFSET_PUL_Pos)        /* (POWER_OFFSET_CTRL) Reactive Power Offset for Pulse Quantities Enable Mask */
#define POWER_OFFSET_CTRL_Q_OFFSET_PUL(value)      (POWER_OFFSET_Q_OFFSET_PUL_Msk & (_UINT32_(value) << POWER_OFFSET_Q_OFFSET_PUL_Pos))
#define   POWER_OFFSET_CTRL_Q_OFFSET_PUL_DISABLED_Val      _UINT32_(0x0)                            /* (POWER_OFFSET_CTRL) Reactive Power Offset for Pulse Quantities Enable disabled */
#define   POWER_OFFSET_CTRL_Q_OFFSET_PUL_ENABLED_Val       _UINT32_(0x1)                            /* (POWER_OFFSET_CTRL) Reactive Power Offset for Pulse Quantities Enable enabled */
#define POWER_OFFSET_CTRL_P_OFFSET_PUL_Pos         _UINT32_(31)                                /* (POWER_OFFSET_CTRL) Active Power Offset for Pulse Quantities Enable Position */
#define POWER_OFFSET_CTRL_P_OFFSET_PUL_Msk         (_UINT32_(0x1) << POWER_OFFSET_P_OFFSET_PUL_Pos)        /* (POWER_OFFSET_CTRL) Active Power Offset for Pulse Quantities Enable Mask */
#define POWER_OFFSET_CTRL_P_OFFSET_PUL(value)      (POWER_OFFSET_P_OFFSET_PUL_Msk & (_UINT32_(value) << POWER_OFFSET_P_OFFSET_PUL_Pos))
#define   POWER_OFFSET_CTRL_P_OFFSET_PUL_DISABLED_Val      _UINT32_(0x0)                            /* (POWER_OFFSET_CTRL) Active Power Offset for Pulse Quantities Enable disabled */
#define   POWER_OFFSET_CTRL_P_OFFSET_PUL_ENABLED_Val       _UINT32_(0x1)                            /* (POWER_OFFSET_CTRL) Active Power Offset for Pulse Quantities Enable enabled */

/* -------- POWER_OFFSET_P : (MET_CONTROL Offset: 0x40) (R/W  32) Active Power Offset (sQ1.30) -------- */
#define POWER_OFFSET_P_Pos                         _UINT32_(0)                                 /* (POWER_OFFSET_P) Active Power Offset Position */
#define POWER_OFFSET_P_Msk                         (_UINT32_(0xFFFFFFFF) << POWER_OFFSET_P_Pos)         /* (POWER_OFFSET_P) Active Power Offset Mask */
#define POWER_OFFSET_P_Val(value)                  (POWER_OFFSET_P_Msk & (_UINT32_(value) << POWER_OFFSET_P_Pos))

/* -------- POWER_OFFSET_Q : (MET_CONTROL Offset: 0x44) (R/W  32) Active Power Offset (sQ1.30) -------- */
#define POWER_OFFSET_Q_Pos                         _UINT32_(0)                                 /* (POWER_OFFSET_Q) Active Power Offset Position */
#define POWER_OFFSET_Q_Msk                         (_UINT32_(0xFFFFFFFF) << POWER_OFFSET_Q_Pos)         /* (POWER_OFFSET_Q) Active Power Offset Mask */
#define POWER_OFFSET_Q_Val(value)                  (POWER_OFFSET_Q_Msk & (_UINT32_(value) << POWER_OFFSET_Q_Pos))

/* -------- SWELL_THRESHOLD_VA : (MET_CONTROL Offset: 0x48) (R/W  32) Voltage swell threshold for each half cycle of phase-A voltage (uQ0.32) -------- */
#define SWELL_THRESHOLD_VA_Pos                     _UINT32_(0)                                 /* (SWELL_THRESHOLD_VA) Voltage swell threshold Position */
#define SWELL_THRESHOLD_VA_Msk                     (_UINT32_(0xFFFFFFFF) << SWELL_THRESHOLD_VA_Pos)         /* (SWELL_THRESHOLD_VA) Voltage swell threshold Mask */
#define SWELL_THRESHOLD_VA_Val(value)              (SWELL_THRESHOLD_VA_Msk & (_UINT32_(value) << SWELL_THRESHOLD_VA_Pos))

/* -------- SWELL_THRESHOLD_VB : (MET_CONTROL Offset: 0x4C) (R/W  32) Voltage swell threshold for each half cycle of phase-B voltage (uQ0.32) -------- */
#define SWELL_THRESHOLD_VB_Pos                     _UINT32_(0)                                 /* (SWELL_THRESHOLD_VB) Voltage swell threshold Position */
#define SWELL_THRESHOLD_VB_Msk                     (_UINT32_(0xFFFFFFFF) << SWELL_THRESHOLD_VB_Pos)         /* (SWELL_THRESHOLD_VB) Voltage swell threshold Mask */
#define SWELL_THRESHOLD_VB_Val(value)              (SWELL_THRESHOLD_VB_Msk & (_UINT32_(value) << SWELL_THRESHOLD_VB_Pos))

/* -------- SWELL_THRESHOLD_VC : (MET_CONTROL Offset: 0x50) (R/W  32) Voltage swell threshold for each half cycle of phase-C voltage (uQ0.32) -------- */
#define SWELL_THRESHOLD_VC_Pos                     _UINT32_(0)                                 /* (SWELL_THRESHOLD_VC) Voltage swell threshold Position */
#define SWELL_THRESHOLD_VC_Msk                     (_UINT32_(0xFFFFFFFF) << SWELL_THRESHOLD_VC_Pos)         /* (SWELL_THRESHOLD_VC) Voltage swell threshold Mask */
#define SWELL_THRESHOLD_VC_Val(value)              (SWELL_THRESHOLD_VC_Msk & (_UINT32_(value) << SWELL_THRESHOLD_VC_Pos))

/* -------- SAG_THRESHOLD_VA : (MET_CONTROL Offset: 0x54) (R/W  32) Voltage sag threshold for each half cycle of phase-A voltage (uQ0.32) -------- */
#define SAG_THRESHOLD_VA_Pos                     _UINT32_(0)                                 /* (SAG_THRESHOLD_VA) Voltage sag threshold Position */
#define SAG_THRESHOLD_VA_Msk                     (_UINT32_(0xFFFFFFFF) << SAG_THRESHOLD_VA_Pos)         /* (SAG_THRESHOLD_VA) Voltage sag threshold Mask */
#define SAG_THRESHOLD_VA_Val(value)              (SAG_THRESHOLD_VA_Msk & (_UINT32_(value) << SAG_THRESHOLD_VA_Pos))

/* -------- SAG_THRESHOLD_VB : (MET_CONTROL Offset: 0x58) (R/W  32) Voltage sag threshold for each half cycle of phase-B voltage (uQ0.32) -------- */
#define SAG_THRESHOLD_VB_Pos                     _UINT32_(0)                                 /* (SAG_THRESHOLD_VB) Voltage sag threshold Position */
#define SAG_THRESHOLD_VB_Msk                     (_UINT32_(0xFFFFFFFF) << SAG_THRESHOLD_VB_Pos)         /* (SAG_THRESHOLD_VB) Voltage sag threshold Mask */
#define SAG_THRESHOLD_VB_Val(value)              (SAG_THRESHOLD_VB_Msk & (_UINT32_(value) << SAG_THRESHOLD_VB_Pos))

/* -------- SAG_THRESHOLD_VC : (MET_CONTROL Offset: 0x5C) (R/W  32) Voltage sag threshold for each half cycle of phase-C voltage (uQ0.32) -------- */
#define SAG_THRESHOLD_VC_Pos                     _UINT32_(0)                                 /* (SAG_THRESHOLD_VC) Voltage sag threshold Position */
#define SAG_THRESHOLD_VC_Msk                     (_UINT32_(0xFFFFFFFF) << SAG_THRESHOLD_VC_Pos)         /* (SAG_THRESHOLD_VC) Voltage sag threshold Mask */
#define SAG_THRESHOLD_VC_Val(value)              (SAG_THRESHOLD_VC_Msk & (_UINT32_(value) << SAG_THRESHOLD_VC_Pos))

/* -------- SAG_THRESHOLD_VC : (MET_CONTROL Offset: 0x5C) (R/W  32) Voltage sag threshold for each half cycle of phase-C voltage (uQ0.32) -------- */
#define SAG_THRESHOLD_VC_Pos                     _UINT32_(0)                                 /* (SAG_THRESHOLD_VC) Voltage sag threshold Position */
#define SAG_THRESHOLD_VC_Msk                     (_UINT32_(0xFFFFFFFF) << SAG_THRESHOLD_VC_Pos)         /* (SAG_THRESHOLD_VC) Voltage sag threshold Mask */
#define SAG_THRESHOLD_VC_Val(value)              (SAG_THRESHOLD_VC_Msk & (_UINT32_(value) << SAG_THRESHOLD_VC_Pos))

/* -------- K_IA : (MET_CONTROL Offset: 0x60) (R/W  32) Current Conversion Factor (uQ22.10) -------- */
#define K_IA_Pos                                 _UINT32_(0)                                 /* (K_IA) Current Conversion Factor Position */
#define K_IA_Msk                                 (_UINT32_(0xFFFFFFFF) << K_IA_Pos)          /* (K_IA) Current Conversion Factor Mask */
#define K_IA_Val(value)                          (K_IA_Msk & (_UINT32_(value) << K_IA_Pos))

/* -------- K_VA : (MET_CONTROL Offset: 0x64) (R/W  32) Volatge Conversion Factor (uQ22.10) -------- */
#define K_VA_Pos                                 _UINT32_(0)                                 /* (K_VA) Volatge Conversion Factor Position */
#define K_VA_Msk                                 (_UINT32_(0xFFFFFFFF) << K_VA_Pos)          /* (K_VA) Volatge Conversion Factor Mask */
#define K_VA_Val(value)                          (K_VA_Msk & (_UINT32_(value) << K_VA_Pos))

/* -------- K_IB : (MET_CONTROL Offset: 0x68) (R/W  32) Current Conversion Factor (uQ22.10) -------- */
#define K_IB_Pos                                 _UINT32_(0)                                 /* (K_IB) Current Conversion Factor Position */
#define K_IB_Msk                                 (_UINT32_(0xFFFFFFFF) << K_IB_Pos)          /* (K_IB) Current Conversion Factor Mask */
#define K_IB_Val(value)                          (K_IB_Msk & (_UINT32_(value) << K_IB_Pos))

/* -------- K_VB : (MET_CONTROL Offset: 0x6C) (R/W  32) Volatge Conversion Factor (uQ22.10) -------- */
#define K_VB_Pos                                 _UINT32_(0)                                 /* (K_VB) Volatge Conversion Factor Position */
#define K_VB_Msk                                 (_UINT32_(0xFFFFFFFF) << K_VB_Pos)          /* (K_VB) Volatge Conversion Factor Mask */
#define K_VB_Val(value)                          (K_VB_Msk & (_UINT32_(value) << K_VB_Pos))

/* -------- K_IC : (MET_CONTROL Offset: 0x70) (R/W  32) Current Conversion Factor (uQ22.10) -------- */
#define K_IC_Pos                                 _UINT32_(0)                                 /* (K_IC) Current Conversion Factor Position */
#define K_IC_Msk                                 (_UINT32_(0xFFFFFFFF) << K_IC_Pos)          /* (K_IC) Current Conversion Factor Mask */
#define K_IC_Val(value)                          (K_IC_Msk & (_UINT32_(value) << K_IC_Pos))

/* -------- K_VC : (MET_CONTROL Offset: 0x74) (R/W  32) Volatge Conversion Factor (uQ22.10) -------- */
#define K_VC_Pos                                 _UINT32_(0)                                 /* (K_VC) Volatge Conversion Factor Position */
#define K_VC_Msk                                 (_UINT32_(0xFFFFFFFF) << K_VC_Pos)          /* (K_VC) Volatge Conversion Factor Mask */
#define K_VC_Val(value)                          (K_VC_Msk & (_UINT32_(value) << K_VC_Pos))

/* -------- K_IN : (MET_CONTROL Offset: 0x78) (R/W  32) Current Conversion Factor (uQ22.10) -------- */
#define K_IN_Pos                                 _UINT32_(0)                                 /* (K_IN) Current Conversion Factor Position */
#define K_IN_Msk                                 (_UINT32_(0xFFFFFFFF) << K_IN_Pos)          /* (K_IN) Current Conversion Factor Mask */
#define K_IN_Val(value)                          (K_IN_Msk & (_UINT32_(value) << K_IN_Pos))

/* -------- CAL_M_IA : (MET_CONTROL Offset: 0x7C) (R/W  32) Current magnitude calibration constant, I_A (sQ2.29) -------- */
#define CAL_M_IA_Pos                             _UINT32_(0)                                 /* (CAL_M_IA) Current Conversion Factor Position */
#define CAL_M_IA_Msk                             (_UINT32_(0xFFFFFFFF) << CAL_M_IA_Pos)      /* (CAL_M_IA) Current Conversion Factor Mask */
#define CAL_M_IA_Val(value)                      (CAL_M_IA_Msk & (_UINT32_(value) << CAL_M_IA_Pos))

/* -------- CAL_M_VA : (MET_CONTROL Offset: 0x80) (R/W  32) Voltage magnitude calibration constant, V_A (sQ2.29) -------- */
#define CAL_M_VA_Pos                             _UINT32_(0)                                 /* (CAL_M_VA) Voltage Conversion Factor Position */
#define CAL_M_VA_Msk                             (_UINT32_(0xFFFFFFFF) << CAL_M_VA_Pos)      /* (CAL_M_VA) Voltage Conversion Factor Mask */
#define CAL_M_VA_Val(value)                      (CAL_M_VA_Msk & (_UINT32_(value) << CAL_M_VA_Pos))

/* -------- CAL_M_IB : (MET_CONTROL Offset: 0x84) (R/W  32) Current magnitude calibration constant, I_B (sQ2.29) -------- */
#define CAL_M_IB_Pos                             _UINT32_(0)                                 /* (CAL_M_IB) Current Conversion Factor Position */
#define CAL_M_IB_Msk                             (_UINT32_(0xFFFFFFFF) << CAL_M_IB_Pos)      /* (CAL_M_IB) Current Conversion Factor Mask */
#define CAL_M_IB_Val(value)                      (CAL_M_IB_Msk & (_UINT32_(value) << CAL_M_IB_Pos))

/* -------- CAL_M_VB : (MET_CONTROL Offset: 0x88) (R/W  32) Voltage magnitude calibration constant, V_B (sQ2.29) -------- */
#define CAL_M_VB_Pos                             _UINT32_(0)                                 /* (CAL_M_VB) Voltage Conversion Factor Position */
#define CAL_M_VB_Msk                             (_UINT32_(0xFFFFFFFF) << CAL_M_VB_Pos)      /* (CAL_M_VB) Voltage Conversion Factor Mask */
#define CAL_M_VB_Val(value)                      (CAL_M_VB_Msk & (_UINT32_(value) << CAL_M_VB_Pos))

/* -------- CAL_M_IC : (MET_CONTROL Offset: 0x8C) (R/W  32) Current magnitude calibration constant, I_C (sQ2.29) -------- */
#define CAL_M_IC_Pos                             _UINT32_(0)                                 /* (CAL_M_IC) Current Conversion Factor Position */
#define CAL_M_IC_Msk                             (_UINT32_(0xFFFFFFFF) << CAL_M_IC_Pos)      /* (CAL_M_IC) Current Conversion Factor Mask */
#define CAL_M_IC_Val(value)                      (CAL_M_IC_Msk & (_UINT32_(value) << CAL_M_IC_Pos))

/* -------- CAL_M_VC : (MET_CONTROL Offset: 0x90) (R/W  32) Voltage magnitude calibration constant, V_C (sQ2.29) -------- */
#define CAL_M_VC_Pos                             _UINT32_(0)                                 /* (CAL_M_VC) Voltage Conversion Factor Position */
#define CAL_M_VC_Msk                             (_UINT32_(0xFFFFFFFF) << CAL_M_VC_Pos)      /* (CAL_M_VC) Voltage Conversion Factor Mask */
#define CAL_M_VC_Val(value)                      (CAL_M_VC_Msk & (_UINT32_(value) << CAL_M_VC_Pos))

/* -------- CAL_M_IN : (MET_CONTROL Offset: 0x94) (R/W  32) Current magnitude calibration constant, I_N (sQ2.29) -------- */
#define CAL_M_IN_Pos                             _UINT32_(0)                                 /* (CAL_M_IN) Current Conversion Factor Position */
#define CAL_M_IN_Msk                             (_UINT32_(0xFFFFFFFF) << CAL_M_IN_Pos)      /* (CAL_M_IN) Current Conversion Factor Mask */
#define CAL_M_IN_Val(value)                      (CAL_M_IN_Msk & (_UINT32_(value) << CAL_M_IN_Pos))

/* -------- CAL_PH_IA : (MET_CONTROL Offset: 0x98) (R/W  32) Current phase calibration constant, I_A (sQ2.29) -------- */
#define CAL_PH_IA_Pos                             _UINT32_(0)                                 /* (CAL_PH_IA) Current phase calibration constant Position */
#define CAL_PH_IA_Msk                             (_UINT32_(0xFFFFFFFF) << CAL_PH_IA_Pos)      /* (CAL_PH_IA) Current phase calibration constant Mask */
#define CAL_PH_IA_Val(value)                      (CAL_PH_IA_Msk & (_UINT32_(value) << CAL_PH_IA_Pos))

/* -------- CAL_PH_VA : (MET_CONTROL Offset: 0x9C) (R/W  32) Voltage phase calibration constant, V_A (sQ2.29) -------- */
#define CAL_PH_VA_Pos                             _UINT32_(0)                                 /* (CAL_PH_VA) Voltage phase calibration constant Position */
#define CAL_PH_VA_Msk                             (_UINT32_(0xFFFFFFFF) << CAL_PH_VA_Pos)      /* (CAL_PH_VA) Voltage phase calibration constant Mask */
#define CAL_PH_VA_Val(value)                      (CAL_PH_VA_Msk & (_UINT32_(value) << CAL_PH_VA_Pos))

/* -------- CAL_PH_IB : (MET_CONTROL Offset: 0xA0) (R/W  32) Current phase calibration constant, I_B (sQ2.29) -------- */
#define CAL_PH_IB_Pos                             _UINT32_(0)                                 /* (CAL_PH_IB) Current phase calibration constant Position */
#define CAL_PH_IB_Msk                             (_UINT32_(0xFFFFFFFF) << CAL_PH_IB_Pos)      /* (CAL_PH_IB) Current phase calibration constant Mask */
#define CAL_PH_IB_Val(value)                      (CAL_PH_IB_Msk & (_UINT32_(value) << CAL_PH_IB_Pos))

/* -------- CAL_PH_VB : (MET_CONTROL Offset: 0xA4) (R/W  32) Voltage phase calibration constant, V_B (sQ2.29) -------- */
#define CAL_PH_VB_Pos                             _UINT32_(0)                                 /* (CAL_PH_VB) Voltage phase calibration constant Position */
#define CAL_PH_VB_Msk                             (_UINT32_(0xFFFFFFFF) << CAL_PH_VB_Pos)      /* (CAL_PH_VB) Voltage phase calibration constant Mask */
#define CAL_PH_VB_Val(value)                      (CAL_PH_VB_Msk & (_UINT32_(value) << CAL_PH_VB_Pos))

/* -------- CAL_PH_IC : (MET_CONTROL Offset: 0xA8) (R/W  32) Current phase calibration constant, I_C (sQ2.29) -------- */
#define CAL_PH_IC_Pos                             _UINT32_(0)                                 /* (CAL_PH_IC) Current phase calibration constant Position */
#define CAL_PH_IC_Msk                             (_UINT32_(0xFFFFFFFF) << CAL_PH_IC_Pos)      /* (CAL_PH_IC) Current phase calibration constant Mask */
#define CAL_PH_IC_Val(value)                      (CAL_PH_IC_Msk & (_UINT32_(value) << CAL_PH_IC_Pos))

/* -------- CAL_PH_VC : (MET_CONTROL Offset: 0xAC) (R/W  32) Voltage phase calibration constant, V_C (sQ2.29) -------- */
#define CAL_PH_VC_Pos                             _UINT32_(0)                                 /* (CAL_PH_VC) Voltage phase calibration constant Position */
#define CAL_PH_VC_Msk                             (_UINT32_(0xFFFFFFFF) << CAL_PH_VC_Pos)      /* (CAL_PH_VC) Voltage phase calibration constant Mask */
#define CAL_PH_VC_Val(value)                      (CAL_PH_VC_Msk & (_UINT32_(value) << CAL_PH_VC_Pos))

/* -------- CAL_PH_IN : (MET_CONTROL Offset: 0xB0) (R/W  32) Current phase calibration constant, I_N (sQ2.29) -------- */
#define CAL_PH_IN_Pos                             _UINT32_(0)                                 /* (CAL_PH_IN) Current phase calibration constant Position */
#define CAL_PH_IN_Msk                             (_UINT32_(0xFFFFFFFF) << CAL_PH_IN_Pos)      /* (CAL_PH_IN) Current phase calibration constant Mask */
#define CAL_PH_IN_Val(value)                      (CAL_PH_IN_Msk & (_UINT32_(value) << CAL_PH_IN_Pos))

/* -------- CAPTURE_CTRL : (MET_CONTROL Offset: 0xB4) (R/W  32) Waveform capture control register -------- */
#define CAPTURE_CTRL_CAPTURE_TYPE_Pos             _UINT32_(0)                                 /* (CAPTURE_CTRL) Waveform capture control register Position */
#define CAPTURE_CTRL_CAPTURE_TYPE_Msk             (_UINT32_(0x3) << CAPTURE_CTRL_CAPTURE_TYPE_Pos)         /* (CAPTURE_CTRL) Waveform capture control register Mask */
#define CAPTURE_CTRL_CAPTURE_TYPE(value)          (CAPTURE_CTRL_CAPTURE_TYPE_Msk & (_UINT32_(value) << CAPTURE_CTRL_CAPTURE_TYPE_Pos))
#define   CAPTURE_CTRL_CAPTURE_TYPE_ONE_SHOT_Val    _UINT32_(0x0)                   /* (CAPTURE_CTRL) Capture will stop after capture finishes or is disabled */
#define   CAPTURE_CTRL_CAPTURE_TYPE_CONTINUOS_Val   _UINT32_(0x1)                   /* (CAPTURE_CTRL) Continuous capture of waveform data in circular-buffer fashion */
#define CAPTURE_CTRL_CAPTURE_SOURCE_Pos           _UINT32_(4)                                 /* (CAPTURE_CTRL) Capture Source Select Position */
#define CAPTURE_CTRL_CAPTURE_SOURCE_Msk           (_UINT32_(0x7) << CAPTURE_CTRL_CAPTURE_SOURCE_Pos)         /* (CAPTURE_CTRL) Capture Source Select Mask */
#define CAPTURE_CTRL_CAPTURE_SOURCE(value)        (CAPTURE_CTRL_CAPTURE_SOURCE_Msk & (_UINT32_(value) << CAPTURE_CTRL_CAPTURE_SOURCE_Pos))
#define   CAPTURE_CTRL_CAPTURE_SOURCE_16KHz_Val     _UINT32_(0x0)                   /* (CAPTURE_CTRL) Capture 16kHz data [sQ1.30] before DSP filtering */
#define   CAPTURE_CTRL_CAPTURE_SOURCE_4KHz_FBW_Val  _UINT32_(0x1)                   /* (CAPTURE_CTRL) Capture 4kHz FBW data [sQ2.29] (Full Bandwidth = fundamental + harmonics) */
#define   CAPTURE_CTRL_CAPTURE_SOURCE_4KHz_NBW_Val  _UINT32_(0x2)                   /* (CAPTURE_CTRL) Capture 4kHz NBW data [sQ2.29] (Narrow Bandwidth = fundamental only) */
#define CAPTURE_CTRL_CH_SEL_IA_Pos                _UINT32_(8)                                 /* (CAPTURE_CTRL) Capture Channel Select Position */
#define CAPTURE_CTRL_CH_SEL_IA_Msk                (_UINT32_(0x1) << CAPTURE_CTRL_CH_SEL_IA_Pos)         /* (CAPTURE_CTRL) Capture Channel Select Mask */
#define CAPTURE_CTRL_CH_SEL_IA(value)             (CAPTURE_CTRL_CH_SEL_IA_Msk & (_UINT32_(value) << CAPTURE_CTRL_CH_SEL_IA_Pos))
#define   CAPTURE_CTRL_CH_SEL_IA_DISABLED_Val       _UINT32_(0x0)                   /* (CAPTURE_CTRL) Waveform capture is disabled  */
#define   CAPTURE_CTRL_CH_SEL_IA_ENABLED_Val        _UINT32_(0x1)                   /* (CAPTURE_CTRL) Waveform capture is enabled */
#define CAPTURE_CTRL_CH_SEL_VA_Pos                _UINT32_(9)                                 /* (CAPTURE_CTRL) Capture Channel Select Position */
#define CAPTURE_CTRL_CH_SEL_VA_Msk                (_UINT32_(0x1) << CAPTURE_CTRL_CH_SEL_VA_Pos)         /* (CAPTURE_CTRL) Capture Channel Select Mask */
#define CAPTURE_CTRL_CH_SEL_VA(value)             (CAPTURE_CTRL_CH_SEL_VA_Msk & (_UINT32_(value) << CAPTURE_CTRL_CH_SEL_VA_Pos))
#define   CAPTURE_CTRL_CH_SEL_VA_DISABLED_Val       _UINT32_(0x0)                   /* (CAPTURE_CTRL) Waveform capture is disabled  */
#define   CAPTURE_CTRL_CH_SEL_VA_ENABLED_Val        _UINT32_(0x1)                   /* (CAPTURE_CTRL) Waveform capture is enabled */
#define CAPTURE_CTRL_CH_SEL_IB_Pos                _UINT32_(10)                                 /* (CAPTURE_CTRL) Capture Channel Select Position */
#define CAPTURE_CTRL_CH_SEL_IB_Msk                (_UINT32_(0x1) << CAPTURE_CTRL_CH_SEL_IB_Pos)         /* (CAPTURE_CTRL) Capture Channel Select Mask */
#define CAPTURE_CTRL_CH_SEL_IB(value)             (CAPTURE_CTRL_CH_SEL_IB_Msk & (_UINT32_(value) << CAPTURE_CTRL_CH_SEL_IB_Pos))
#define   CAPTURE_CTRL_CH_SEL_IB_DISABLED_Val       _UINT32_(0x0)                   /* (CAPTURE_CTRL) Waveform capture is disabled  */
#define   CAPTURE_CTRL_CH_SEL_IB_ENABLED_Val        _UINT32_(0x1)                   /* (CAPTURE_CTRL) Waveform capture is enabled */
#define CAPTURE_CTRL_CH_SEL_VB_Pos                _UINT32_(11)                                 /* (CAPTURE_CTRL) Capture Channel Select Position */
#define CAPTURE_CTRL_CH_SEL_VB_Msk                (_UINT32_(0x1) << CAPTURE_CTRL_CH_SEL_VB_Pos)         /* (CAPTURE_CTRL) Capture Channel Select Mask */
#define CAPTURE_CTRL_CH_SEL_VB(value)             (CAPTURE_CTRL_CH_SEL_VB_Msk & (_UINT32_(value) << CAPTURE_CTRL_CH_SEL_VB_Pos))
#define   CAPTURE_CTRL_CH_SEL_VB_DISABLED_Val       _UINT32_(0x0)                   /* (CAPTURE_CTRL) Waveform capture is disabled  */
#define   CAPTURE_CTRL_CH_SEL_VB_ENABLED_Val        _UINT32_(0x1)                   /* (CAPTURE_CTRL) Waveform capture is enabled */
#define CAPTURE_CTRL_CH_SEL_IC_Pos                _UINT32_(12)                                 /* (CAPTURE_CTRL) Capture Channel Select Position */
#define CAPTURE_CTRL_CH_SEL_IC_Msk                (_UINT32_(0x1) << CAPTURE_CTRL_CH_SEL_IC_Pos)         /* (CAPTURE_CTRL) Capture Channel Select Mask */
#define CAPTURE_CTRL_CH_SEL_IC(value)             (CAPTURE_CTRL_CH_SEL_IC_Msk & (_UINT32_(value) << CAPTURE_CTRL_CH_SEL_IC_Pos))
#define   CAPTURE_CTRL_CH_SEL_IC_DISABLED_Val       _UINT32_(0x0)                   /* (CAPTURE_CTRL) Waveform capture is disabled  */
#define   CAPTURE_CTRL_CH_SEL_IC_ENABLED_Val        _UINT32_(0x1)                   /* (CAPTURE_CTRL) Waveform capture is enabled */
#define CAPTURE_CTRL_CH_SEL_VC_Pos                _UINT32_(13)                                 /* (CAPTURE_CTRL) Capture Channel Select Position */
#define CAPTURE_CTRL_CH_SEL_VC_Msk                (_UINT32_(0x1) << CAPTURE_CTRL_CH_SEL_VC_Pos)         /* (CAPTURE_CTRL) Capture Channel Select Mask */
#define CAPTURE_CTRL_CH_SEL_VC(value)             (CAPTURE_CTRL_CH_SEL_VC_Msk & (_UINT32_(value) << CAPTURE_CTRL_CH_SEL_VC_Pos))
#define   CAPTURE_CTRL_CH_SEL_VC_DISABLED_Val       _UINT32_(0x0)                   /* (CAPTURE_CTRL) Waveform capture is disabled  */
#define   CAPTURE_CTRL_CH_SEL_VC_ENABLED_Val        _UINT32_(0x1)                   /* (CAPTURE_CTRL) Waveform capture is enabled */
#define CAPTURE_CTRL_CAPTURE_EN_Pos                _UINT32_(31)                                 /* (CAPTURE_CTRL) Enable waveform capture Position */
#define CAPTURE_CTRL_CAPTURE_EN_Msk                (_UINT32_(0x1) << CAPTURE_CTRL_CAPTURE_EN_Pos)         /* (CAPTURE_CTRL) Enable waveform capture Mask */
#define CAPTURE_CTRL_CAPTURE_EN(value)             (CAPTURE_CTRL_CAPTURE_EN_Msk & (_UINT32_(value) << CAPTURE_CTRL_CAPTURE_EN_Pos))
#define   CAPTURE_CTRL_CAPTURE_EN_DISABLED_Val       _UINT32_(0x0)                   /* (CAPTURE_CTRL) Waveform capture feature is disabled  */
#define   CAPTURE_CTRL_CAPTURE_EN_ENABLED_Val        _UINT32_(0x1)                   /* (CAPTURE_CTRL) Waveform capture feature is enabled */

/* -------- CAPTURE_BUFF_SIZE : (MET_CONTROL Offset: 0xB8) (R/W  32) Waveform capture buffer size in units of 32-bits -------- */
#define CAPTURE_BUFF_SIZE_Pos                      _UINT32_(0)                                 /* (CAPTURE_BUFF_SIZE) Waveform capture buffer size Position */
#define CAPTURE_BUFF_SIZE_Msk                      (_UINT32_(0xFFFFFF) << CAPTURE_BUFF_SIZE_Pos)      /* (CAPTURE_BUFF_SIZE) Waveform capture buffer size Mask */
#define CAPTURE_BUFF_SIZE_Val(value)               (CAPTURE_BUFF_SIZE_Msk & (_UINT32_(value) << CAPTURE_BUFF_SIZE_Pos))

/* -------- CAPTURE_ADDR_SIZE : (MET_CONTROL Offset: 0xBC) (R/W  32) Waveform capture buffer address pointer -------- */
#define CAPTURE_ADDR_SIZE_Pos                      _UINT32_(0)                                 /* (CAPTURE_ADDR_SIZE) Waveform capture buffer address pointer Position */
#define CAPTURE_ADDR_SIZE_Msk                      (_UINT32_(0xFFFFFFFF) << CAPTURE_ADDR_SIZE_Pos)      /* (CAPTURE_ADDR_SIZE) Waveform capture buffer address pointer Mask */
#define CAPTURE_ADDR_SIZE_Val(value)               (CAPTURE_ADDR_SIZE_Msk & (_UINT32_(value) << CAPTURE_ADDR_SIZE_Pos))

/* -------- ATSENSE_CTRL_20_23 : (MET_CONTROL Offset: 0xCC) (R/W  32) ATSense control: channels I0,I1,V1,I2 -------- */
#define ATSENSE_CTRL_20_23_I0_ON_Pos                _UINT32_(0)                                 /* (ATSENSE_CTRL_20_23) Enable channel I0 Position */
#define ATSENSE_CTRL_20_23_I0_ON_Msk                (_UINT32_(0x1) << ATSENSE_CTRL_20_23_I0_ON_Pos)         /* (ATSENSE_CTRL_20_23) Enable channel I0 Mask */
#define ATSENSE_CTRL_20_23_I0_ON(value)             (ATSENSE_CTRL_20_23_I0_ON_Msk & (_UINT32_(value) << ATSENSE_CTRL_20_23_I0_ON_Pos))
#define   ATSENSE_CTRL_20_23_I0_ON_DISABLED_Val       _UINT32_(0x0)                   /* (ATSENSE_CTRL_20_23) Channel is disabled  */
#define   ATSENSE_CTRL_20_23_I0_ON_ENABLED_Val        _UINT32_(0x1)                   /* (ATSENSE_CTRL_20_23) Channel is enabled */
#define ATSENSE_CTRL_20_23_TEMPMEAS_Pos             _UINT32_(1)                                 /* (ATSENSE_CTRL_20_23) Channel I0/Temperature multiplex select bit Position */
#define ATSENSE_CTRL_20_23_TEMPMEAS_Msk             (_UINT32_(0x1) << ATSENSE_CTRL_20_23_TEMPMEAS_Pos)         /* (ATSENSE_CTRL_20_23) Channel I0/Temperature multiplex select bit Mask */
#define ATSENSE_CTRL_20_23_TEMPMEAS(value)          (ATSENSE_CTRL_20_23_TEMPMEAS_Msk & (_UINT32_(value) << ATSENSE_CTRL_20_23_TEMPMEAS_Pos))
#define   ATSENSE_CTRL_20_23_TEMPMEAS_CURRENT_I0_Val  _UINT32_(0x0)                   /* (ATSENSE_CTRL_20_23) Measurement of external current I0  */
#define   ATSENSE_CTRL_20_23_TEMPMEAS_TEMP_Val        _UINT32_(0x1)                   /* (ATSENSE_CTRL_20_23) Measurement of internal temperature sensor */
#define ATSENSE_CTRL_20_23_I0_GAIN_Pos              _UINT32_(4)                                 /* (ATSENSE_CTRL_20_23) Channel I0 gain Position */
#define ATSENSE_CTRL_20_23_I0_GAIN_Msk              (_UINT32_(0x3) << ATSENSE_CTRL_20_23_I0_GAIN_Pos)         /* (ATSENSE_CTRL_20_23) Channel I0 gain Mask */
#define ATSENSE_CTRL_20_23_I0_GAIN(value)           (ATSENSE_CTRL_20_23_I0_GAIN_Msk & (_UINT32_(value) << ATSENSE_CTRL_20_23_I0_GAIN_Pos))
#define   ATSENSE_CTRL_20_23_I0_GAIN_1_Val            _UINT32_(0x0)                   /* (ATSENSE_CTRL_20_23) Channel has a gain of 1  */
#define   ATSENSE_CTRL_20_23_I0_GAIN_2_Val            _UINT32_(0x1)                   /* (ATSENSE_CTRL_20_23) Channel has a gain of 2 */
#define   ATSENSE_CTRL_20_23_I0_GAIN_4_Val            _UINT32_(0x2)                   /* (ATSENSE_CTRL_20_23) Channel has a gain of 4 */
#define   ATSENSE_CTRL_20_23_I0_GAIN_8_Val            _UINT32_(0x3)                   /* (ATSENSE_CTRL_20_23) Channel has a gain of 8 */
#define ATSENSE_CTRL_20_23_I1_ON_Pos                _UINT32_(8)                                 /* (ATSENSE_CTRL_20_23) Enable channel I1 Position */
#define ATSENSE_CTRL_20_23_I1_ON_Msk                (_UINT32_(0x1) << ATSENSE_CTRL_20_23_I1_ON_Pos)         /* (ATSENSE_CTRL_20_23) Enable channel I1 Mask */
#define ATSENSE_CTRL_20_23_I1_ON(value)             (ATSENSE_CTRL_20_23_I1_ON_Msk & (_UINT32_(value) << ATSENSE_CTRL_20_23_I1_ON_Pos))
#define   ATSENSE_CTRL_20_23_I1_ON_DISABLED_Val       _UINT32_(0x0)                   /* (ATSENSE_CTRL_20_23) Channel is disabled  */
#define   ATSENSE_CTRL_20_23_I1_ON_ENABLED_Val        _UINT32_(0x1)                   /* (ATSENSE_CTRL_20_23) Channel is enabled */
#define ATSENSE_CTRL_20_23_I1_GAIN_Pos              _UINT32_(12)                                /* (ATSENSE_CTRL_20_23) Channel I1 gain Position */
#define ATSENSE_CTRL_20_23_I1_GAIN_Msk              (_UINT32_(0x3) << ATSENSE_CTRL_20_23_I1_GAIN_Pos)         /* (ATSENSE_CTRL_20_23) Channel I1 gain Mask */
#define ATSENSE_CTRL_20_23_I1_GAIN(value)           (ATSENSE_CTRL_20_23_I1_GAIN_Msk & (_UINT32_(value) << ATSENSE_CTRL_20_23_I1_GAIN_Pos))
#define   ATSENSE_CTRL_20_23_I1_GAIN_1_Val            _UINT32_(0x0)                   /* (ATSENSE_CTRL_20_23) Channel has a gain of 1  */
#define   ATSENSE_CTRL_20_23_I1_GAIN_2_Val            _UINT32_(0x1)                   /* (ATSENSE_CTRL_20_23) Channel has a gain of 2 */
#define   ATSENSE_CTRL_20_23_I1_GAIN_4_Val            _UINT32_(0x2)                   /* (ATSENSE_CTRL_20_23) Channel has a gain of 4 */
#define   ATSENSE_CTRL_20_23_I1_GAIN_8_Val            _UINT32_(0x3)                   /* (ATSENSE_CTRL_20_23) Channel has a gain of 8 */
#define ATSENSE_CTRL_20_23_V1_ON_Pos                _UINT32_(16)                                 /* (ATSENSE_CTRL_20_23) Enable channel V1 Position */
#define ATSENSE_CTRL_20_23_V1_ON_Msk                (_UINT32_(0x1) << ATSENSE_CTRL_20_23_V1_ON_Pos)         /* (ATSENSE_CTRL_20_23) Enable channel V1 Mask */
#define ATSENSE_CTRL_20_23_V1_ON(value)             (ATSENSE_CTRL_20_23_V1_ON_Msk & (_UINT32_(value) << ATSENSE_CTRL_20_23_V1_ON_Pos))
#define   ATSENSE_CTRL_20_23_V1_ON_DISABLED_Val       _UINT32_(0x0)                   /* (ATSENSE_CTRL_20_23) Channel is disabled  */
#define   ATSENSE_CTRL_20_23_V1_ON_ENABLED_Val        _UINT32_(0x1)                   /* (ATSENSE_CTRL_20_23) Channel is enabled */
#define ATSENSE_CTRL_20_23_I2_ON_Pos                _UINT32_(24)                                 /* (ATSENSE_CTRL_20_23) Enable channel I2 Position */
#define ATSENSE_CTRL_20_23_I2_ON_Msk                (_UINT32_(0x1) << ATSENSE_CTRL_20_23_I2_ON_Pos)         /* (ATSENSE_CTRL_20_23) Enable channel I2 Mask */
#define ATSENSE_CTRL_20_23_I2_ON(value)             (ATSENSE_CTRL_20_23_I2_ON_Msk & (_UINT32_(value) << ATSENSE_CTRL_20_23_I2_ON_Pos))
#define   ATSENSE_CTRL_20_23_I2_ON_DISABLED_Val       _UINT32_(0x0)                   /* (ATSENSE_CTRL_20_23) Channel is disabled  */
#define   ATSENSE_CTRL_20_23_I2_ON_ENABLED_Val        _UINT32_(0x1)                   /* (ATSENSE_CTRL_20_23) Channel is enabled */
#define ATSENSE_CTRL_20_23_I2_GAIN_Pos              _UINT32_(28)                                /* (ATSENSE_CTRL_20_23) Channel I2 gain Position */
#define ATSENSE_CTRL_20_23_I2_GAIN_Msk              (_UINT32_(0x3) << ATSENSE_CTRL_20_23_I2_GAIN_Pos)         /* (ATSENSE_CTRL_20_23) Channel I2 gain Mask */
#define ATSENSE_CTRL_20_23_I2_GAIN(value)           (ATSENSE_CTRL_20_23_I2_GAIN_Msk & (_UINT32_(value) << ATSENSE_CTRL_20_23_I2_GAIN_Pos))
#define   ATSENSE_CTRL_20_23_I2_GAIN_1_Val            _UINT32_(0x0)                   /* (ATSENSE_CTRL_20_23) Channel has a gain of 1  */
#define   ATSENSE_CTRL_20_23_I2_GAIN_2_Val            _UINT32_(0x1)                   /* (ATSENSE_CTRL_20_23) Channel has a gain of 2 */
#define   ATSENSE_CTRL_20_23_I2_GAIN_4_Val            _UINT32_(0x2)                   /* (ATSENSE_CTRL_20_23) Channel has a gain of 4 */
#define   ATSENSE_CTRL_20_23_I2_GAIN_8_Val            _UINT32_(0x3)                   /* (ATSENSE_CTRL_20_23) Channel has a gain of 8 */

/* -------- ATSENSE_CTRL_24_27 : (MET_CONTROL Offset: 0xD0) (R/W  32) ATSense control: channels V2,I3,V3,ANA -------- */
#define ATSENSE_CTRL_24_27_V2_ON_Pos                _UINT32_(0)                                 /* (ATSENSE_CTRL_24_27) Enable channel V2 Position */
#define ATSENSE_CTRL_24_27_V2_ON_Msk                (_UINT32_(0x1) << ATSENSE_CTRL_24_27_V2_ON_Pos)         /* (ATSENSE_CTRL_24_27) Enable channel V2 Mask */
#define ATSENSE_CTRL_24_27_V2_ON(value)             (ATSENSE_CTRL_24_27_V2_ON_Msk & (_UINT32_(value) << ATSENSE_CTRL_24_27_V2_ON_Pos))
#define   ATSENSE_CTRL_24_27_V2_ON_DISABLED_Val       _UINT32_(0x0)                   /* (ATSENSE_CTRL_24_27) Channel is disabled  */
#define   ATSENSE_CTRL_24_27_V2_ON_ENABLED_Val        _UINT32_(0x1)                   /* (ATSENSE_CTRL_24_27) Channel is enabled */
#define ATSENSE_CTRL_24_27_I3_ON_Pos                _UINT32_(8)                                 /* (ATSENSE_CTRL_24_27) Enable channel I3 Position */
#define ATSENSE_CTRL_24_27_I3_ON_Msk                (_UINT32_(0x1) << ATSENSE_CTRL_24_27_I3_ON_Pos)         /* (ATSENSE_CTRL_24_27) Enable channel I3 Mask */
#define ATSENSE_CTRL_24_27_I3_ON(value)             (ATSENSE_CTRL_24_27_I3_ON_Msk & (_UINT32_(value) << ATSENSE_CTRL_24_27_I3_ON_Pos))
#define   ATSENSE_CTRL_24_27_I3_ON_DISABLED_Val       _UINT32_(0x0)                   /* (ATSENSE_CTRL_24_27) Channel is disabled  */
#define   ATSENSE_CTRL_24_27_I3_ON_ENABLED_Val        _UINT32_(0x1)                   /* (ATSENSE_CTRL_24_27) Channel is enabled */
#define ATSENSE_CTRL_24_27_I3_GAIN_Pos              _UINT32_(12)                                /* (ATSENSE_CTRL_24_27) Channel I3 gain Position */
#define ATSENSE_CTRL_24_27_I3_GAIN_Msk              (_UINT32_(0x3) << ATSENSE_CTRL_24_27_I3_GAIN_Pos)         /* (ATSENSE_CTRL_24_27) Channel I3 gain Mask */
#define ATSENSE_CTRL_24_27_I3_GAIN(value)           (ATSENSE_CTRL_24_27_I3_GAIN_Msk & (_UINT32_(value) << ATSENSE_CTRL_24_27_I3_GAIN_Pos))
#define   ATSENSE_CTRL_24_27_I3_GAIN_1_Val            _UINT32_(0x0)                   /* (ATSENSE_CTRL_24_27) Channel has a gain of 1  */
#define   ATSENSE_CTRL_24_27_I3_GAIN_2_Val            _UINT32_(0x1)                   /* (ATSENSE_CTRL_24_27) Channel has a gain of 2 */
#define   ATSENSE_CTRL_24_27_I3_GAIN_4_Val            _UINT32_(0x2)                   /* (ATSENSE_CTRL_24_27) Channel has a gain of 4 */
#define   ATSENSE_CTRL_24_27_I3_GAIN_8_Val            _UINT32_(0x3)                   /* (ATSENSE_CTRL_24_27) Channel has a gain of 8 */
#define ATSENSE_CTRL_24_27_V3_ON_Pos                _UINT32_(16)                                 /* (ATSENSE_CTRL_24_27) Enable channel V3 Position */
#define ATSENSE_CTRL_24_27_V3_ON_Msk                (_UINT32_(0x1) << ATSENSE_CTRL_24_27_V3_ON_Pos)         /* (ATSENSE_CTRL_24_27) Enable channel V3 Mask */
#define ATSENSE_CTRL_24_27_V3_ON(value)             (ATSENSE_CTRL_24_27_V3_ON_Msk & (_UINT32_(value) << ATSENSE_CTRL_24_27_V3_ON_Pos))
#define   ATSENSE_CTRL_24_27_V3_ON_DISABLED_Val       _UINT32_(0x0)                   /* (ATSENSE_CTRL_24_27) Channel is disabled  */
#define   ATSENSE_CTRL_24_27_V3_ON_ENABLED_Val        _UINT32_(0x1)                   /* (ATSENSE_CTRL_24_27) Channel is enabled */
#define ATSENSE_CTRL_24_27_ONBIAS_Pos                _UINT32_(24)                                 /* (ATSENSE_CTRL_24_27) Enable bias Position */
#define ATSENSE_CTRL_24_27_ONBIAS_Msk                (_UINT32_(0x1) << ATSENSE_CTRL_24_27_ONBIAS_Pos)         /* (ATSENSE_CTRL_24_27) Enable bias Mask */
#define ATSENSE_CTRL_24_27_ONBIAS(value)             (ATSENSE_CTRL_24_27_ONBIAS_Msk & (_UINT32_(value) << ATSENSE_CTRL_24_27_ONBIAS_Pos))
#define   ATSENSE_CTRL_24_27_ONBIAS_DISABLED_Val       _UINT32_(0x0)                   /* (ATSENSE_CTRL_24_27) Bias is disabled  */
#define   ATSENSE_CTRL_24_27_ONBIAS_ENABLED_Val        _UINT32_(0x1)                   /* (ATSENSE_CTRL_24_27) Bias is enabled */
#define ATSENSE_CTRL_24_27_ONREF_Pos                _UINT32_(25)                                 /* (ATSENSE_CTRL_24_27) Enable internal reference Position */
#define ATSENSE_CTRL_24_27_ONREF_Msk                (_UINT32_(0x1) << ATSENSE_CTRL_24_27_ONREF_Pos)         /* (ATSENSE_CTRL_24_27) Enable internal reference Mask */
#define ATSENSE_CTRL_24_27_ONREF(value)             (ATSENSE_CTRL_24_27_ONREF_Msk & (_UINT32_(value) << ATSENSE_CTRL_24_27_ONREF_Pos))
#define   ATSENSE_CTRL_24_27_ONREF_DISABLED_Val       _UINT32_(0x0)                   /* (ATSENSE_CTRL_24_27) Internal reference is disabled  */
#define   ATSENSE_CTRL_24_27_ONREF_ENABLED_Val        _UINT32_(0x1)                   /* (ATSENSE_CTRL_24_27) Internal reference is enabled */
#define ATSENSE_CTRL_24_27_ONLDO_Pos                _UINT32_(26)                                 /* (ATSENSE_CTRL_24_27) Enable internal LDO Position */
#define ATSENSE_CTRL_24_27_ONLDO_Msk                (_UINT32_(0x1) << ATSENSE_CTRL_24_27_ONLDO_Pos)         /* (ATSENSE_CTRL_24_27) Enable internal LDO Mask */
#define ATSENSE_CTRL_24_27_ONLDO(value)             (ATSENSE_CTRL_24_27_ONLDO_Msk & (_UINT32_(value) << ATSENSE_CTRL_24_27_ONLDO_Pos))
#define   ATSENSE_CTRL_24_27_ONLDO_DISABLED_Val       _UINT32_(0x0)                   /* (ATSENSE_CTRL_24_27) Internal LDO is disabled  */
#define   ATSENSE_CTRL_24_27_ONLDO_ENABLED_Val        _UINT32_(0x1)                   /* (ATSENSE_CTRL_24_27) Internal LDO is enabled */

/* -------- ATSENSE_CTRL_28_2B : (MET_CONTROL Offset: 0xD4) (R/W  32) ATSense control: Registers: x28, x2A-2B, x2D -------- */
#define ATSENSE_CTRL_28_2B_OSR_Pos                  _UINT32_(0)                                 /* (ATSENSE_CTRL_28_2B) Over Sampling Ratio Position */
#define ATSENSE_CTRL_28_2B_OSR_Msk                  (_UINT32_(0x3) << ATSENSE_CTRL_28_2B_OSR_Pos)         /* (ATSENSE_CTRL_28_2B) Over Sampling Ratio Mask */
#define ATSENSE_CTRL_28_2B_OSR(value)               (ATSENSE_CTRL_28_2B_OSR_Msk & (_UINT32_(value) << ATSENSE_CTRL_28_2B_OSR_Pos))
#define   ATSENSE_CTRL_28_2B_OSR8_Val                 _UINT32_(0x0)                   /* (ATSENSE_CTRL_28_2B) System OSR = 8  */
#define   ATSENSE_CTRL_24_27_OSR16_Val                _UINT32_(0x1)                   /* (ATSENSE_CTRL_28_2B) System OSR = 16 */
#define   ATSENSE_CTRL_24_27_OSR32_Val                _UINT32_(0x2)                   /* (ATSENSE_CTRL_28_2B) System OSR = 32 */
#define   ATSENSE_CTRL_24_27_OSR64_Val                _UINT32_(0x3)                   /* (ATSENSE_CTRL_28_2B) System OSR = 64 */
#define ATSENSE_CTRL_28_2B_MSB_MODE_Pos             _UINT32_(4)                                 /* (ATSENSE_CTRL_28_2B) Mode Selection Position */
#define ATSENSE_CTRL_28_2B_MSB_MODE_Msk             (_UINT32_(0x1) << ATSENSE_CTRL_28_2B_MSB_MODE_Pos)         /* (ATSENSE_CTRL_28_2B) Mode Selection Mask */
#define ATSENSE_CTRL_28_2B_MSB_MODE(value)          (ATSENSE_CTRL_28_2B_MSB_MODE_Msk & (_UINT32_(value) << ATSENSE_CTRL_28_2B_MSB_MODE_Pos))
#define   ATSENSE_CTRL_28_2B_MSB_MODE_32BITS_Val      _UINT32_(0x0)                   /* (ATSENSE_CTRL_28_2B) 32-bits mode  */
#define   ATSENSE_CTRL_28_2B_MSB_MODE_16BITS_Val      _UINT32_(0x1)                   /* (ATSENSE_CTRL_28_2B) 16-bits mode */

/* -------- POWER_OFFSET_P_A : (MET_CONTROL Offset: 0xDC) (R/W  32) Phase-A active power offset (sQ9.40) -------- */
#define POWER_OFFSET_P_A_Pos                        _UINT32_(0)                                 /* (POWER_OFFSET_P_A) Phase-A active power offset Position */
#define POWER_OFFSET_P_A_Msk                        (_UINT32_(0xFFFFFFFF) << POWER_OFFSET_P_A_Pos)      /* (POWER_OFFSET_P_A) Phase-A active power offset Mask */
#define POWER_OFFSET_P_A_Val(value)                 (POWER_OFFSET_P_A_Msk & (_UINT32_(value) << POWER_OFFSET_P_A_Pos))

/* -------- POWER_OFFSET_P_B : (MET_CONTROL Offset: 0xE0) (R/W  32) Phase-B active power offset (sQ9.40) -------- */
#define POWER_OFFSET_P_B_Pos                        _UINT32_(0)                                 /* (POWER_OFFSET_P_B) Phase-B active power offset Position */
#define POWER_OFFSET_P_B_Msk                        (_UINT32_(0xFFFFFFFF) << POWER_OFFSET_P_B_Pos)      /* (POWER_OFFSET_P_B) Phase-B active power offset Mask */
#define POWER_OFFSET_P_B_Val(value)                 (POWER_OFFSET_P_B_Msk & (_UINT32_(value) << POWER_OFFSET_P_B_Pos))

/* -------- POWER_OFFSET_P_C : (MET_CONTROL Offset: 0xE4) (R/W  32) Phase-C active power offset (sQ9.40) -------- */
#define POWER_OFFSET_P_C_Pos                        _UINT32_(0)                                 /* (POWER_OFFSET_P_C) Phase-C active power offset Position */
#define POWER_OFFSET_P_C_Msk                        (_UINT32_(0xFFFFFFFF) << POWER_OFFSET_P_C_Pos)      /* (POWER_OFFSET_P_C) Phase-C active power offset Mask */
#define POWER_OFFSET_P_C_Val(value)                 (POWER_OFFSET_P_C_Msk & (_UINT32_(value) << POWER_OFFSET_P_C_Pos))

/* -------- POWER_OFFSET_Q_A : (MET_CONTROL Offset: 0xDC) (R/W  32) Phase-A reactive power offset (sQ9.40) -------- */
#define POWER_OFFSET_Q_A_Pos                        _UINT32_(0)                                 /* (POWER_OFFSET_Q_A) Phase-A reactive power offset Position */
#define POWER_OFFSET_Q_A_Msk                        (_UINT32_(0xFFFFFFFF) << POWER_OFFSET_Q_A_Pos)      /* (POWER_OFFSET_Q_A) Phase-A reactive power offset Mask */
#define POWER_OFFSET_Q_A_Val(value)                 (POWER_OFFSET_Q_A_Msk & (_UINT32_(value) << POWER_OFFSET_Q_A_Pos))

/* -------- POWER_OFFSET_Q_B : (MET_CONTROL Offset: 0xE0) (R/W  32) Phase-B reactive power offset (sQ9.40) -------- */
#define POWER_OFFSET_Q_B_Pos                        _UINT32_(0)                                 /* (POWER_OFFSET_Q_B) Phase-B reactive power offset Position */
#define POWER_OFFSET_Q_B_Msk                        (_UINT32_(0xFFFFFFFF) << POWER_OFFSET_Q_B_Pos)      /* (POWER_OFFSET_Q_B) Phase-B reactive power offset Mask */
#define POWER_OFFSET_Q_B_Val(value)                 (POWER_OFFSET_Q_B_Msk & (_UINT32_(value) << POWER_OFFSET_Q_B_Pos))

/* -------- POWER_OFFSET_Q_C : (MET_CONTROL Offset: 0xE4) (R/W  32) Phase-C reactive power offset (sQ9.40) -------- */
#define POWER_OFFSET_Q_C_Pos                        _UINT32_(0)                                 /* (POWER_OFFSET_Q_C) Phase-C reactive power offset Position */
#define POWER_OFFSET_Q_C_Msk                        (_UINT32_(0xFFFFFFFF) << POWER_OFFSET_Q_C_Pos)      /* (POWER_OFFSET_Q_C) Phase-C reactive power offset Mask */
#define POWER_OFFSET_Q_C_Val(value)                 (POWER_OFFSET_Q_C_Msk & (_UINT32_(value) << POWER_OFFSET_Q_C_Pos))

/* -------- VERSION : (MET_STATUS Offset: 0x00) (R/W  32) Metrology Version -------- */
#define VERSION_FIRMWARE_REV_Pos                    _UINT32_(0)                                 /* (VERSION) Firmware Revision Position */
#define VERSION_FIRMWARE_REV_Msk                    (_UINT32_(0xFF) << VERSION_FIRMWARE_REV_Pos)      /* (VERSION) Firmware Revision Mask */
#define VERSION_FIRMWARE_REV(value)                 (VERSION_FIRMWARE_REV_Msk & (_UINT32_(value) << VERSION_FIRMWARE_REV_Pos))
#define VERSION_FIRMWARE_MINOR_REV_Pos              _UINT32_(8)                                 /* (VERSION) Minor Firmware Revision Position */
#define VERSION_FIRMWARE_MINOR_REV_Msk              (_UINT32_(0xFF) << VERSION_FIRMWARE_MINOR_REV_Pos)      /* (VERSION) Minor Firmware Revision Mask */
#define VERSION_FIRMWARE_MINOR_REV(value)           (VERSION_FIRMWARE_MINOR_REV_Msk & (_UINT32_(value) << VERSION_FIRMWARE_MINOR_REV_Pos))
#define VERSION_FIRMWARE_MAJOR_REV_Pos              _UINT32_(8)                                 /* (VERSION) Major  Firmware Revision Position */
#define VERSION_FIRMWARE_MAJOR_REV_Msk              (_UINT32_(0xFF) << VERSION_FIRMWARE_MAJOR_REV_Pos)      /* (VERSION) Major  Firmware Revision Mask */
#define VERSION_FIRMWARE_MAJOR_REV(value)           (VERSION_FIRMWARE_MAJOR_REV_Msk & (_UINT32_(value) << VERSION_FIRMWARE_MAJOR_REV_Pos))

/* -------- STATUS : (MET_STATUS Offset: 0x04) (R/W  32) Metrology status -------- */
#define STATUS_STATUS_Pos                           _UINT32_(0)                       /* (STATUS) Metrology status Position */
#define STATUS_STATUS_Msk                           (_UINT32_(0x0F) << STATUS_STATUS_Pos)      /* (STATUS) Metrology status Mask */
#define STATUS_STATUS(value)                        (STATUS_STATUS_Msk & (_UINT32_(value) << STATUS_STATUS_Pos))
#define   STATUS_STATUS_HALT                        _UINT32_(0x0)                   /* (STATUS) Metrology module is halted */
#define   STATUS_STATUS_RESET                       _UINT32_(0x1)                   /* (STATUS) Resetting metrology module */
#define   STATUS_STATUS_INIT_DSP                    _UINT32_(0x2)                   /* (STATUS) Initializing DSP filters */
#define   STATUS_STATUS_DSP_READY                   _UINT32_(0x3)                   /* (STATUS) DSP filters have been initialized */
#define   STATUS_STATUS_INIT_ATSENSE                _UINT32_(0x4)                   /* (STATUS) Initializing ATSENSE */
#define   STATUS_STATUS_ATSENSE_READY               _UINT32_(0x5)                   /* (STATUS) ATSENSE has been initialized */
#define   STATUS_STATUS_READY                       _UINT32_(0x6)                   /* (STATUS) ATSENSE temperature configuration & calibration data has been copied into output registers */
#define   STATUS_STATUS_DSP_SETTLING                _UINT32_(0x7)                   /* (STATUS) Waiting for DSP filters to stabilize to full accuracy */
#define   STATUS_STATUS_DSP_RUNNING                 _UINT32_(0x8)                   /* (STATUS) DSP filters have stabilized to full accuracy */

/* -------- STATUS : (MET_STATUS Offset: 0x08) (R/W  32) Metrology state flags -------- */
#define STATUS_STATE_FLAG_PH_A_ACTIVE_Pos           _UINT32_(0)                       /* (STATUS) Phase A active Position */
#define STATUS_STATE_FLAG_PH_A_ACTIVE_Msk           (_UINT32_(0x01) << STATUS_STATE_FLAG_PH_A_ACTIVE_Pos)      /* (STATUS) Phase A active Mask */
#define STATUS_STATE_FLAG_PH_A_ACTIVE(value)        (STATUS_STATE_FLAG_PH_A_ACTIVE_Msk & (_UINT32_(value) << STATUS_STATE_FLAG_PH_A_ACTIVE_Pos))
#define   STATUS_STATE_FLAG_PH_A_ACTIVE_INACTIVE_Val  _UINT32_(0x0)                   /* (STATUS) Voltage phase A is inactive  */
#define   STATUS_STATE_FLAG_PH_A_ACTIVE_ACTIVE_Val    _UINT32_(0x1)                   /* (STATUS) Voltage phase A is active */
#define STATUS_STATE_FLAG_PH_B_ACTIVE_Pos           _UINT32_(1)                       /* (STATUS) Phase A active Position */
#define STATUS_STATE_FLAG_PH_B_ACTIVE_Msk           (_UINT32_(0x01) << STATUS_STATE_FLAG_PH_B_ACTIVE_Pos)      /* (STATUS) Phase B active Mask */
#define STATUS_STATE_FLAG_PH_B_ACTIVE(value)        (STATUS_STATE_FLAG_PH_B_ACTIVE_Msk & (_UINT32_(value) << STATUS_STATE_FLAG_PH_B_ACTIVE_Pos))
#define   STATUS_STATE_FLAG_PH_B_ACTIVE_INACTIVE_Val  _UINT32_(0x0)                   /* (STATUS) Voltage phase B is inactive  */
#define   STATUS_STATE_FLAG_PH_B_ACTIVE_ACTIVE_Val    _UINT32_(0x1)                   /* (STATUS) Voltage phase B is active */
#define STATUS_STATE_FLAG_PH_C_ACTIVE_Pos           _UINT32_(2)                       /* (STATUS) Phase A active Position */
#define STATUS_STATE_FLAG_PH_C_ACTIVE_Msk           (_UINT32_(0x01) << STATUS_STATE_FLAG_PH_C_ACTIVE_Pos)      /* (STATUS) Phase C active Mask */
#define STATUS_STATE_FLAG_PH_C_ACTIVE(value)        (STATUS_STATE_FLAG_PH_C_ACTIVE_Msk & (_UINT32_(value) << STATUS_STATE_FLAG_PH_C_ACTIVE_Pos))
#define   STATUS_STATE_FLAG_PH_C_ACTIVE_INACTIVE_Val  _UINT32_(0x0)                   /* (STATUS) Voltage phase C is inactive  */
#define   STATUS_STATE_FLAG_PH_C_ACTIVE_ACTIVE_Val    _UINT32_(0x1)                   /* (STATUS) Voltage phase C is active */
#define STATUS_STATE_FLAG_TIMING_Vx_Pos             _UINT32_(3)                       /* (STATUS) Active Voltage Channel Position */
#define STATUS_STATE_FLAG_TIMING_Vx_Msk             (_UINT32_(0x03) << STATUS_STATE_FLAG_TIMING_Vx_Pos)      /* (STATUS) Active Voltage Channel Mask */
#define STATUS_STATE_FLAG_TIMING_Vx(value)          (STATUS_STATE_FLAG_TIMING_Vx_Msk & (_UINT32_(value) << STATUS_STATE_FLAG_TIMING_Vx_Pos))
#define   STATUS_STATE_FLAG_TIMING_Vx_A_Val           _UINT32_(0x0)                   /* (STATUS) Voltage phase VA is used for timing extraction purposes  */
#define   STATUS_STATE_FLAG_TIMING_Vx_B_Val           _UINT32_(0x1)                   /* (STATUS) Voltage phase VB is used for timing extraction purposes */
#define   STATUS_STATE_FLAG_TIMING_Vx_C_Val           _UINT32_(0x2)                   /* (STATUS) Voltage phase VC is used for timing extraction purposes */
#define STATUS_STATE_FLAG_FREQ_LOCKED_Pos           _UINT32_(5)                       /* (STATUS) Frequency Locked Flag Position */
#define STATUS_STATE_FLAG_FREQ_LOCKED_Msk           (_UINT32_(0x01) << STATUS_STATE_FLAG_FREQ_LOCKED_Pos)      /* (STATUS) Frequency Locked Flag Mask */
#define STATUS_STATE_FLAG_FREQ_LOCKED(value)        (STATUS_STATE_FLAG_FREQ_LOCKED_Msk & (_UINT32_(value) << STATUS_STATE_FLAG_FREQ_LOCKED_Pos))
#define   STATUS_STATE_FLAG_FREQ_LOCKED_DISABLED_Val   _UINT32_(0x0)                   /* (STATUS) Line frequency is not determined  */
#define   STATUS_STATE_FLAG_FREQ_LOCKED_ENABLED_Val    _UINT32_(0x1)                   /* (STATUS) Line frequency has been determined */
#define STATUS_STATE_FLAG_ATSENSE_FAIL_Pos           _UINT32_(7)                       /* (STATUS) ATSENSE state Position */
#define STATUS_STATE_FLAG_ATSENSE_FAIL_Msk           (_UINT32_(0x01) << STATUS_STATE_FLAG_ATSENSE_FAIL_Pos)      /* (STATUS) ATSENSE state Mask */
#define STATUS_STATE_FLAG_ATSENSE_FAIL(value)        (STATUS_STATE_FLAG_ATSENSE_FAIL_Msk & (_UINT32_(value) << STATUS_STATE_FLAG_ATSENSE_FAIL_Pos))
#define   STATUS_STATE_FLAG_ATSENSE_FAIL_DISABLED_Val   _UINT32_(0x0)                   /* (STATUS) ATSENSE initialization is successful  */
#define   STATUS_STATE_FLAG_ATSENSE_FAIL_ENABLED_Val    _UINT32_(0x1)                   /* (STATUS) ATSENSE initialization is failed */
#define STATUS_STATE_FLAG_HOST_ID_FAIL_Pos           _UINT32_(15)                       /* (STATUS) Host Identification Failure Indication Position */
#define STATUS_STATE_FLAG_HOST_ID_FAIL_Msk           (_UINT32_(0x01) << STATUS_STATE_FLAG_HOST_ID_FAIL_Pos)      /* (STATUS) Host Identification Failure Indication Mask */
#define STATUS_STATE_FLAG_HOST_ID_FAIL(value)        (STATUS_STATE_FLAG_HOST_ID_FAIL_Msk & (_UINT32_(value) << STATUS_STATE_FLAG_HOST_ID_FAIL_Pos))
#define   STATUS_STATE_FLAG_HOST_ID_FAIL_SUCCESS_Val   _UINT32_(0x0)                   /* (STATUS) Proper PIC32CXMTx part successfully identified  */
#define   STATUS_STATE_FLAG_HOST_ID_FAIL_FAIL_Val      _UINT32_(0x1)                   /* (STATUS) Failed to identify proper PIC32CXMTx part */
#define STATUS_STATE_FLAG_CREEP_DET_IA_Pos           _UINT32_(16)                       /* (STATUS) Phase A Current Creep Detected Flag Position */
#define STATUS_STATE_FLAG_CREEP_DET_IA_Msk           (_UINT32_(0x01) << STATUS_STATE_FLAG_CREEP_DET_IA_Pos)      /* (STATUS) Phase A Current Creep Detected Flag Mask */
#define STATUS_STATE_FLAG_CREEP_DET_IA(value)        (STATUS_STATE_FLAG_CREEP_DET_IA_Msk & (_UINT32_(value) << STATUS_STATE_FLAG_CREEP_DET_IA_Pos))
#define   STATUS_STATE_FLAG_CREEP_DET_IA_NO_DETECT_Val   _UINT32_(0x0)                   /* (STATUS) Channel A current creep isn?t detected in the current full line cycle  */
#define   STATUS_STATE_FLAG_CREEP_DET_IA_DETECT_Val      _UINT32_(0x1)                   /* (STATUS) Channel A current creep is detected in the current full line cycle */
#define STATUS_STATE_FLAG_CREEP_DET_IB_Pos           _UINT32_(17)                       /* (STATUS) Phase B Current Creep Detected Flag Position */
#define STATUS_STATE_FLAG_CREEP_DET_IB_Msk           (_UINT32_(0x01) << STATUS_STATE_FLAG_CREEP_DET_IB_Pos)      /* (STATUS) Phase B Current Creep Detected Flag Mask */
#define STATUS_STATE_FLAG_CREEP_DET_IB(value)        (STATUS_STATE_FLAG_CREEP_DET_IB_Msk & (_UINT32_(value) << STATUS_STATE_FLAG_CREEP_DET_IB_Pos))
#define   STATUS_STATE_FLAG_CREEP_DET_IB_NO_DETECT_Val   _UINT32_(0x0)                   /* (STATUS) Channel B current creep isn?t detected in the current full line cycle  */
#define   STATUS_STATE_FLAG_CREEP_DET_IB_DETECT_Val      _UINT32_(0x1)                   /* (STATUS) Channel B current creep is detected in the current full line cycle */
#define STATUS_STATE_FLAG_CREEP_DET_IC_Pos           _UINT32_(18)                       /* (STATUS) Phase C Current Creep Detected Flag Position */
#define STATUS_STATE_FLAG_CREEP_DET_IC_Msk           (_UINT32_(0x01) << STATUS_STATE_FLAG_CREEP_DET_IC_Pos)      /* (STATUS) Phase C Current Creep Detected Flag Mask */
#define STATUS_STATE_FLAG_CREEP_DET_IC(value)        (STATUS_STATE_FLAG_CREEP_DET_IC_Msk & (_UINT32_(value) << STATUS_STATE_FLAG_CREEP_DET_IC_Pos))
#define   STATUS_STATE_FLAG_CREEP_DET_IC_NO_DETECT_Val   _UINT32_(0x0)                   /* (STATUS) Channel C current creep isn?t detected in the current full line cycle  */
#define   STATUS_STATE_FLAG_CREEP_DET_IC_DETECT_Val      _UINT32_(0x1)                   /* (STATUS) Channel C current creep is detected in the current full line cycle */
#define STATUS_STATE_FLAG_CREEP_DET_Q_Pos            _UINT32_(19)                       /* (STATUS) Total Reactive Power Creep Detected Flag Position */
#define STATUS_STATE_FLAG_CREEP_DET_Q_Msk            (_UINT32_(0x01) << STATUS_STATE_FLAG_CREEP_DET_Q_Pos)      /* (STATUS) Total Reactive Power Creep Detected Flag Mask */
#define STATUS_STATE_FLAG_CREEP_DET_Q(value)         (STATUS_STATE_FLAG_CREEP_DET_Q_Msk & (_UINT32_(value) << STATUS_STATE_FLAG_CREEP_DET_Q_Pos))
#define   STATUS_STATE_FLAG_CREEP_DET_Q_NO_DETECT_Val   _UINT32_(0x0)                   /* (STATUS) Reactive power creep isn?t detected in the current full line cycle  */
#define   STATUS_STATE_FLAG_CREEP_DET_Q_DETECT_Val      _UINT32_(0x1)                   /* (STATUS) Reactive power creep is detected in the current full line cycle */
#define STATUS_STATE_FLAG_CREEP_DET_P_Pos            _UINT32_(20)                       /* (STATUS) Total Active Power Creep Detected Flag Position */
#define STATUS_STATE_FLAG_CREEP_DET_P_Msk            (_UINT32_(0x01) << STATUS_STATE_FLAG_CREEP_DET_P_Pos)      /* (STATUS) Total Active Power Creep Detected Flag Mask */
#define STATUS_STATE_FLAG_CREEP_DET_P(value)         (STATUS_STATE_FLAG_CREEP_DET_P_Msk & (_UINT32_(value) << STATUS_STATE_FLAG_CREEP_DET_P_Pos))
#define   STATUS_STATE_FLAG_CREEP_DET_P_NO_DETECT_Val   _UINT32_(0x0)                   /* (STATUS) Active power creep isn?t detected in the current full line cycle  */
#define   STATUS_STATE_FLAG_CREEP_DET_P_DETECT_Val      _UINT32_(0x1)                   /* (STATUS) Active power creep is detected in the current full line cycle */
#define STATUS_STATE_FLAG_ARCH_DETECT_FAIL_Pos            _UINT32_(21)                       /* (STATUS) Architecture Detect Failure Position */
#define STATUS_STATE_FLAG_ARCH_DETECT_FAIL_Msk            (_UINT32_(0x01) << STATUS_STATE_FLAG_ARCH_DETECT_FAIL_Pos)      /* (STATUS) Architecture Detect Failure Mask */
#define STATUS_STATE_FLAG_ARCH_DETECT_FAIL(value)         (STATUS_STATE_FLAG_ARCH_DETECT_FAIL_Msk & (_UINT32_(value) << STATUS_STATE_FLAG_ARCH_DETECT_FAIL_Pos))
#define   STATUS_STATE_FLAG_ARCH_DETECT_FAIL_NO_FAIL_Val     _UINT32_(0x0)                   /* (STATUS) A proper device (?SH? or ?C?) was detected  */
#define   STATUS_STATE_FLAG_ARCH_DETECT_FAIL_FAIL_Val        _UINT32_(0x1)                   /* (STATUS) An improper device (not ?SH? nor ?C?) was detected */
#define STATUS_STATE_FLAG_PC_OUT_OF_RANGE_Pos            _UINT32_(22)                       /* (STATUS) Phase Corrector Out of Range Position */
#define STATUS_STATE_FLAG_PC_OUT_OF_RANGE_Msk            (_UINT32_(0x01) << STATUS_STATE_FLAG_PC_OUT_OF_RANGE_Pos)      /* (STATUS) Phase Corrector Out of Range Mask */
#define STATUS_STATE_FLAG_PC_OUT_OF_RANGE(value)         (STATUS_STATE_FLAG_PC_OUT_OF_RANGE_Msk & (_UINT32_(value) << STATUS_STATE_FLAG_PC_OUT_OF_RANGE_Pos))
#define   STATUS_STATE_FLAG_PC_OUT_OF_RANGE_IN_Val         _UINT32_(0x0)                   /* (STATUS) Requested phase correction values in range  */
#define   STATUS_STATE_FLAG_PC_OUT_OF_RANGE_OUT_Val        _UINT32_(0x1)                   /* (STATUS) Requested phase correction values out of range */
#define STATUS_STATE_FLAG_SAG_DET_VA_Pos                 _UINT32_(24)                       /* (STATUS) Voltage Sag Detected Flag for Channel A Position */
#define STATUS_STATE_FLAG_SAG_DET_VA_Msk                 (_UINT32_(0x01) << STATUS_STATE_FLAG_SAG_DET_VA_Pos)      /* (STATUS) Voltage Sag Detected Flag for Channel A Mask */
#define STATUS_STATE_FLAG_SAG_DET_VA(value)              (STATUS_STATE_FLAG_SAG_DET_VA_Msk & (_UINT32_(value) << STATUS_STATE_FLAG_SAG_DET_VA_Pos))
#define   STATUS_STATE_FLAG_SAG_DET_VA_NO_DETECTED_Val     _UINT32_(0x0)                   /* (STATUS) Voltage sag isn?t detected in the current half line cycle  */
#define   STATUS_STATE_FLAG_SAG_DET_VA_DETECTED_Val        _UINT32_(0x1)                   /* (STATUS) Voltage sag is detected in the current half line cycle */
#define STATUS_STATE_FLAG_SAG_DET_VB_Pos                 _UINT32_(25)                       /* (STATUS) Voltage Sag Detected Flag for Channel B Position */
#define STATUS_STATE_FLAG_SAG_DET_VB_Msk                 (_UINT32_(0x01) << STATUS_STATE_FLAG_SAG_DET_VB_Pos)      /* (STATUS) Voltage Sag Detected Flag for Channel B Mask */
#define STATUS_STATE_FLAG_SAG_DET_VB(value)              (STATUS_STATE_FLAG_SAG_DET_VB_Msk & (_UINT32_(value) << STATUS_STATE_FLAG_SAG_DET_VB_Pos))
#define   STATUS_STATE_FLAG_SAG_DET_VB_NO_DETECTED_Val     _UINT32_(0x0)                   /* (STATUS) Voltage sag isn?t detected in the current half line cycle  */
#define   STATUS_STATE_FLAG_SAG_DET_VB_DETECTED_Val        _UINT32_(0x1)                   /* (STATUS) Voltage sag is detected in the current half line cycle */
#define STATUS_STATE_FLAG_SAG_DET_VC_Pos                 _UINT32_(26)                       /* (STATUS) Voltage Sag Detected Flag for Channel C Position */
#define STATUS_STATE_FLAG_SAG_DET_VC_Msk                 (_UINT32_(0x01) << STATUS_STATE_FLAG_SAG_DET_VC_Pos)      /* (STATUS) Voltage Sag Detected Flag for Channel C Mask */
#define STATUS_STATE_FLAG_SAG_DET_VC(value)              (STATUS_STATE_FLAG_SAG_DET_VC_Msk & (_UINT32_(value) << STATUS_STATE_FLAG_SAG_DET_VC_Pos))
#define   STATUS_STATE_FLAG_SAG_DET_VC_NO_DETECTED_Val     _UINT32_(0x0)                   /* (STATUS) Voltage sag isn?t detected in the current half line cycle  */
#define   STATUS_STATE_FLAG_SAG_DET_VC_DETECTED_Val        _UINT32_(0x1)                   /* (STATUS) Voltage sag is detected in the current half line cycle */
#define STATUS_STATE_FLAG_SWELL_DET_VA_Pos                 _UINT32_(28)                       /* (STATUS) Voltage Swell Detected Flag for Channel A Position */
#define STATUS_STATE_FLAG_SWELL_DET_VA_Msk                 (_UINT32_(0x01) << STATUS_STATE_FLAG_SWELL_DET_VA_Pos)      /* (STATUS) Voltage Swell Detected Flag for Channel A Mask */
#define STATUS_STATE_FLAG_SWELL_DET_VA(value)              (STATUS_STATE_FLAG_SWELL_DET_VA_Msk & (_UINT32_(value) << STATUS_STATE_FLAG_SWELL_DET_VA_Pos))
#define   STATUS_STATE_FLAG_SWELL_DET_VA_NO_DETECTED_Val     _UINT32_(0x0)                   /* (STATUS) Voltage swell isn?t detected in the current half line cycle  */
#define   STATUS_STATE_FLAG_SWELL_DET_VA_DETECTED_Val        _UINT32_(0x1)                   /* (STATUS) Voltage swell is detected in the current half line cycle */
#define STATUS_STATE_FLAG_SWELL_DET_VB_Pos                 _UINT32_(29)                       /* (STATUS) Voltage Swell Detected Flag for Channel B Position */
#define STATUS_STATE_FLAG_SWELL_DET_VB_Msk                 (_UINT32_(0x01) << STATUS_STATE_FLAG_SWELL_DET_VB_Pos)      /* (STATUS) Voltage Swell Detected Flag for Channel B Mask */
#define STATUS_STATE_FLAG_SWELL_DET_VB(value)              (STATUS_STATE_FLAG_SWELL_DET_VB_Msk & (_UINT32_(value) << STATUS_STATE_FLAG_SWELL_DET_VB_Pos))
#define   STATUS_STATE_FLAG_SWELL_DET_VB_NO_DETECTED_Val     _UINT32_(0x0)                   /* (STATUS) Voltage swell isn?t detected in the current half line cycle  */
#define   STATUS_STATE_FLAG_SWELL_DET_VB_DETECTED_Val        _UINT32_(0x1)                   /* (STATUS) Voltage swell is detected in the current half line cycle */
#define STATUS_STATE_FLAG_SWELL_DET_VC_Pos                 _UINT32_(30)                       /* (STATUS) Voltage Swell Detected Flag for Channel C Position */
#define STATUS_STATE_FLAG_SWELL_DET_VC_Msk                 (_UINT32_(0x01) << STATUS_STATE_FLAG_SWELL_DET_VC_Pos)      /* (STATUS) Voltage Swell Detected Flag for Channel C Mask */
#define STATUS_STATE_FLAG_SWELL_DET_VC(value)              (STATUS_STATE_FLAG_SWELL_DET_VC_Msk & (_UINT32_(value) << STATUS_STATE_FLAG_SWELL_DET_VC_Pos))
#define   STATUS_STATE_FLAG_SWELL_DET_VC_NO_DETECTED_Val     _UINT32_(0x0)                   /* (STATUS) Voltage swell isn?t detected in the current half line cycle  */
#define   STATUS_STATE_FLAG_SWELL_DET_VC_DETECTED_Val        _UINT32_(0x1)                   /* (STATUS) Voltage swell is detected in the current half line cycle */

/* -------- CAPTURE_STATUS : (MET_STATUS Offset: 0x0C) (R/W  32) Waveform Capture Function Status -------- */
#define CAPTURE_STATUS_CAPTURE_OFFSET_Pos                  _UINT32_(0)                       /* (CAPTURE_STATUS) Offset within the buffer Position */
#define CAPTURE_STATUS_CAPTURE_OFFSET_Msk                  (_UINT32_(0xFFFFFF) << CAPTURE_STATUS_CAPTURE_OFFSET_Pos)      /* (CAPTURE_STATUS) Offset within the buffer Mask */
#define CAPTURE_STATUS_CAPTURE_OFFSET(value)               (CAPTURE_STATUS_CAPTURE_OFFSET_Msk & (_UINT32_(value) << CAPTURE_STATUS_CAPTURE_OFFSET_Pos))
#define CAPTURE_STATUS_CAPTURE_STATE_Pos                   _UINT32_(24)                       /* (CAPTURE_STATUS) Waveform Capture Status Position */
#define CAPTURE_STATUS_CAPTURE_STATE_Msk                   (_UINT32_(0xF) << CAPTURE_STATUS_CAPTURE_STATE_Pos)      /* (CAPTURE_STATUS) Waveform Capture Status Mask */
#define CAPTURE_STATUS_CAPTURE_STATE(value)                (CAPTURE_STATUS_CAPTURE_STATE_Msk & (_UINT32_(value) << CAPTURE_STATUS_CAPTURE_STATE_Pos))
#define   CAPTURE_STATUS_CAPTURE_STATE_DISABLED_Val          _UINT32_(0x0)                   /* (CAPTURE_STATUS) Disabled  */
#define   CAPTURE_STATUS_CAPTURE_STATE_ACTIVE_Val            _UINT32_(0x1)                   /* (CAPTURE_STATUS) Capture in progress  */
#define   CAPTURE_STATUS_CAPTURE_STATE_COMPLETE_Val          _UINT32_(0x2)                   /* (CAPTURE_STATUS) Capture completed  */
#define CAPTURE_STATUS_INVALID_ADDR_Pos                    _UINT32_(28)                       /* (CAPTURE_STATUS) Invalid Capture Address Position */
#define CAPTURE_STATUS_INVALID_ADDR_Msk                    (_UINT32_(0x1) << CAPTURE_STATUS_INVALID_ADDR_Pos)      /* (CAPTURE_STATUS) Invalid Capture Address Mask */
#define CAPTURE_STATUS_INVALID_ADDR(value)                 (CAPTURE_STATUS_INVALID_ADDR_Msk & (_UINT32_(value) << CAPTURE_STATUS_INVALID_ADDR_Pos))
#define   CAPTURE_STATUS_INVALID_ADDR_WITHIN_Val             _UINT32_(0x0)                   /* (CAPTURE_STATUS) Capture address within valid range   */
#define   CAPTURE_STATUS_INVALID_ADDR_OUTSIDE_Val            _UINT32_(0x1)                   /* (CAPTURE_STATUS) Capture address outside valid range  */
#define CAPTURE_STATUS_CAPTURE_WRAP_Pos                    _UINT32_(31)                       /* (CAPTURE_STATUS) Capture Buffer Wrapped Position */
#define CAPTURE_STATUS_CAPTURE_WRAP_Msk                    (_UINT32_(0x1) << CAPTURE_STATUS_CAPTURE_WRAP_Pos)      /* (STATUS) Capture Buffer Wrapped Mask */
#define CAPTURE_STATUS_CAPTURE_WRAP(value)                 (CAPTURE_STATUS_CAPTURE_WRAP_Msk & (_UINT32_(value) << CAPTURE_STATUS_CAPTURE_WRAP_Pos))
#define   CAPTURE_STATUS_CAPTURE_WRAP_NOT_WRAPPED_Val        _UINT32_(0x0)                   /* (CAPTURE_STATUS) Buffer has not wrapped  */
#define   CAPTURE_STATUS_CAPTURE_WRAP_WRAPPED_Val            _UINT32_(0x1)                   /* (CAPTURE_STATUS) Buffer has wrapped at least once */

/* -------- INTERVAL_NUM : (MET_STATUS Offset: 0x10) (R/W  32) Measurement Interval Number -------- */
#define INTERVAL_NUM_HALFCYCLE_COUNT_Pos                   _UINT32_(0)                       /* (INTERVAL_NUM) Subinterval number Position */
#define INTERVAL_NUM_HALFCYCLE_COUNT_Msk                   (_UINT32_(0xFFFF) << INTERVAL_NUM_HALFCYCLE_COUNT_Pos)      /* (INTERVAL_NUM) Subinterval number Mask */
#define INTERVAL_NUM_HALFCYCLE_COUNT(value)                (INTERVAL_NUM_HALFCYCLE_COUNT_Msk & (_UINT32_(value) << INTERVAL_NUM_HALFCYCLE_COUNT_Pos))
#define INTERVAL_NUM_INTERVAL_NUM_Pos                      _UINT32_(16)                       /* (INTERVAL_NUM) Interval number Position */
#define INTERVAL_NUM_INTERVAL_NUM_Msk                      (_UINT32_(0xFFFF) << INTERVAL_NUM_INTERVAL_NUM_Pos)      /* (INTERVAL_NUM) Interval number Mask */
#define INTERVAL_NUM_INTERVAL_NUM(value)                   (INTERVAL_NUM_INTERVAL_NUM_Msk & (_UINT32_(value) << INTERVAL_NUM_INTERVAL_NUM_Pos))

/* -------- N : (MET_STATUS Offset: 0x14) (R/W  32) Number of Samples in the Last Measurement Interval (uQ16.0) -------- */
#define N_Pos                            _UINT32_(0)                       /* (N) Number of Samples in the Last Measurement Interval Position */
#define N_Msk                            (_UINT32_(0xFFFF) << N_Pos)      /* (N) Number of Samples in the Last Measurement Interval Mask */
#define N_Val(value)                     (N_Msk & (_UINT32_(value) << N_Pos))

/* -------- PH_OFFSET : (MET_STATUS Offset: 0x18) (R/W  32) Phase Correction Filter Offset (sQ0.31) -------- */
#define PH_OFFSET_Pos                    _UINT32_(0)                       /* (PH_OFFSET) Phase Correction Filter Offset Position */
#define PH_OFFSET_Msk                    (_UINT32_(0xFFFFFFFF) << N_Pos)      /* (PH_OFFSET) Phase Correction Filter Offset Mask */
#define PH_OFFSET_Val(value)             (PH_OFFSET_Msk & (_UINT32_(value) << PH_OFFSET_Pos))

/* -------- FREQ : (MET_STATUS Offset: 0x1C) (R/W  32) Active Line Voltage Fundamental Harmonic Frequency (uQ20.12) -------- */
#define FREQ_Pos                         _UINT32_(0)                       /* (FREQ) Active Line Voltage Fundamental Harmonic Frequency Position */
#define FREQ_Msk                         (_UINT32_(0xFFFFFFFF) << N_Pos)      /* (FREQ) Active Line Voltage Fundamental Harmonic Frequency Mask */
#define FREQ_Val(value)                  (FREQ_Msk & (_UINT32_(value) << FREQ_Pos))

/* -------- FREQ_VA : (MET_STATUS Offset: 0x20) (R/W  32) Frequency of fundamental harmonic, VA (uQ20.12) -------- */
#define FREQ_VA_Pos                      _UINT32_(0)                       /* (FREQ_VA) Frequency of fundamental harmonic, VA Position */
#define FREQ_VA_Msk                      (_UINT32_(0xFFFFFFFF) << N_Pos)      /* (FREQ_VA) Frequency of fundamental harmonic, VA Mask */
#define FREQ_VA_Val(value)               (FREQ_VA_Msk & (_UINT32_(value) << FREQ_VA_Pos))

/* -------- FREQ_VB : (MET_STATUS Offset: 0x24) (R/W  32) Frequency of fundamental harmonic, VB (uQ20.12) -------- */
#define FREQ_VB_Pos                      _UINT32_(0)                       /* (FREQ_VB) Frequency of fundamental harmonic, VB Position */
#define FREQ_VB_Msk                      (_UINT32_(0xFFFFFFFF) << N_Pos)      /* (FREQ_VB) Frequency of fundamental harmonic, VB Mask */
#define FREQ_VB_Val(value)               (FREQ_VB_Msk & (_UINT32_(value) << FREQ_VB_Pos))

/* -------- FREQ_VC : (MET_STATUS Offset: 0x28) (R/W  32) Frequency of fundamental harmonic, VC (uQ20.12) -------- */
#define FREQ_VC_Pos                      _UINT32_(0)                       /* (FREQ_VC) Frequency of fundamental harmonic, VC Position */
#define FREQ_VC_Msk                      (_UINT32_(0xFFFFFFFF) << N_Pos)      /* (FREQ_VC) Frequency of fundamental harmonic, VC Mask */
#define FREQ_VC_Val(value)               (FREQ_VC_Msk & (_UINT32_(value) << FREQ_VC_Pos))

/* -------- TEMPERATURE : (MET_STATUS Offset: 0x30) (R/W  32) Sensor Temperature (sQ23.8) -------- */
#define TEMPERATURE_Pos                  _UINT32_(0)                       /* (TEMPERATURE) Sensor Temperature Position */
#define TEMPERATURE_Msk                  (_UINT32_(0xFFFFFFFF) << N_Pos)      /* (TEMPERATURE) Sensor Temperature Mask */
#define TEMPERATURE_Val(value)           (TEMPERATURE_Msk & (_UINT32_(value) << TEMPERATURE_Pos))

/* -------- I_A_MAX : (MET_STATUS Offset: 0x34) (R/W  32) Phase A Current Peak (sQ2.29) -------- */
#define I_A_MAX_Pos                      _UINT32_(0)                       /* (I_A_MAX) Phase A Current Peak Position */
#define I_A_MAX_Msk                      (_UINT32_(0xFFFFFFFF) << N_Pos)      /* (I_A_MAX) Phase A Current Peak Mask */
#define I_A_MAX_Val(value)               (I_A_MAX_Msk & (_UINT32_(value) << I_A_MAX_Pos))

/* -------- I_B_MAX : (MET_STATUS Offset: 0x38) (R/W  32) Phase B Current Peak (sQ2.29) -------- */
#define I_B_MAX_Pos                      _UINT32_(0)                       /* (I_B_MAX) Phase B Current Peak Position */
#define I_B_MAX_Msk                      (_UINT32_(0xFFFFFFFF) << N_Pos)      /* (I_B_MAX) Phase B Current Peak Mask */
#define I_B_MAX_Val(value)               (I_B_MAX_Msk & (_UINT32_(value) << I_B_MAX_Pos))

/* -------- I_C_MAX : (MET_STATUS Offset: 0x3C) (R/W  32) Phase C Current Peak (sQ2.29) -------- */
#define I_C_MAX_Pos                      _UINT32_(0)                       /* (I_C_MAX) Phase C Current Peak Position */
#define I_C_MAX_Msk                      (_UINT32_(0xFFFFFFFF) << N_Pos)      /* (I_C_MAX) Phase C Current Peak Mask */
#define I_C_MAX_Val(value)               (I_C_MAX_Msk & (_UINT32_(value) << I_C_MAX_Pos))

/* -------- I_Ni_MAX : (MET_STATUS Offset: 0x40) (R/W  32) Neutral Current Peak (sQ16.15) -------- */
#define I_Ni_MAX_Pos                     _UINT32_(0)                       /* (I_Ni_MAX) Neutral Current Peak Position */
#define I_Ni_MAX_Msk                     (_UINT32_(0xFFFFFFFF) << N_Pos)      /* (I_Ni_MAX) Neutral Current Peak Mask */
#define I_Ni_MAX_Val(value)              (I_Ni_MAX_Msk & (_UINT32_(value) << I_Ni_MAX_Pos))

/* -------- I_Nm_MAX : (MET_STATUS Offset: 0x44) (R/W  32) Neutral Current Peak (sQ16.15) -------- */
#define I_Nm_MAX_Pos                     _UINT32_(0)                       /* (I_Nm_MAX) Neutral Current Peak Position */
#define I_Nm_MAX_Msk                     (_UINT32_(0xFFFFFFFF) << N_Pos)      /* (I_Nm_MAX) Neutral Current Peak Mask */
#define I_Nm_MAX_Val(value)              (I_Nm_MAX_Msk & (_UINT32_(value) << I_Nm_MAX_Pos))

/* -------- V_A_MAX : (MET_STATUS Offset: 0x48) (R/W  32) Voltage A Peak (sQ2.29) -------- */
#define V_A_MAX_Pos                      _UINT32_(0)                       /* V_A_MAX) Voltage A Peak Position */
#define V_A_MAX_Msk                      (_UINT32_(0xFFFFFFFF) << N_Pos)      /* (V_A_MAX) Voltage A Peak Mask */
#define V_A_MAX_Val(value)               (V_A_MAX_Msk & (_UINT32_(value) << V_A_MAX_Pos))

/* -------- V_B_MAX : (MET_STATUS Offset: 0x4C) (R/W  32) Voltage B Peak (sQ2.29) -------- */
#define V_B_MAX_Pos                      _UINT32_(0)                       /* (V_B_MAX) Voltage B Peak Position */
#define V_B_MAX_Msk                      (_UINT32_(0xFFFFFFFF) << N_Pos)      /* (V_B_MAX) Voltage B Peak Mask */
#define V_B_MAX_Val(value)               (V_B_MAX_Msk & (_UINT32_(value) << V_B_MAX_Pos))

/* -------- V_C_MAX : (MET_STATUS Offset: 0x50) (R/W  32) Voltage C Peak (sQ2.29) -------- */
#define V_C_MAX_Pos                      _UINT32_(0)                       /* (V_C_MAX) Voltage C Peak Position */
#define V_C_MAX_Msk                      (_UINT32_(0xFFFFFFFF) << N_Pos)      /* (V_C_MAX) Voltage C Peak Mask */
#define V_C_MAX_Val(value)               (V_C_MAX_Msk & (_UINT32_(value) << V_C_MAX_Pos))

/* -------- FEATURES : (MET_STATUS Offset: 0x54) (R/W  32) Compiled feature indicator -------- */
#define FEATURES_DEBUG_MODES_Pos                    _UINT32_(0)
#define FEATURES_DEBUG_MODES_Msk                    (_UINT32_(0x1) << FEATURES_DEBUG_MODES_Pos)
#define FEATURES_DEBUG_MODES(value)                 (FEATURES_DEBUG_MODES_Msk & (_UINT32_(value) << FEATURES_DEBUG_MODES_Pos))
#define FEATURES_ATSENSE_HALF_CLK_Pos               _UINT32_(1)
#define FEATURES_ATSENSE_HALF_CLK_Msk               (_UINT32_(0x1) << FEATURES_ATSENSE_HALF_CLK_Pos)
#define FEATURES_ATSENSE_HALF_CLK(value)            (FEATURES_ATSENSE_HALF_CLK_Msk & (_UINT32_(value) << FEATURES_ATSENSE_HALF_CLK_Pos))
#define FEATURES_ATSENSE_SYNTHESIZE_Pos             _UINT32_(2)
#define FEATURES_ATSENSE_SYNTHESIZE_Msk             (_UINT32_(0x1) << FEATURES_ATSENSE_SYNTHESIZE_Pos)
#define FEATURES_ATSENSE_SYNTHESIZE(value)          (FEATURES_ATSENSE_SYNTHESIZE_Msk & (_UINT32_(value) << FEATURES_ATSENSE_SYNTHESIZE_Pos))
#define FEATURES_DUPLICATE_V1_Pos                   _UINT32_(3)
#define FEATURES_DUPLICATE_V1_Msk                   (_UINT32_(0x1) << FEATURES_DUPLICATE_V1_Pos)
#define FEATURES_DUPLICATE_V1(value)                (FEATURES_DUPLICATE_V1_Msk & (_UINT32_(value) << FEATURES_DUPLICATE_V1_Pos))
#define FEATURES_FIXED_VREF_Pos                     _UINT32_(4)
#define FEATURES_FIXED_VREF_Msk                     (_UINT32_(0x1) << FEATURES_FIXED_VREF_Pos)
#define FEATURES_FIXED_VREF(value)                  (FEATURES_FIXED_VREF_Msk & (_UINT32_(value) << FEATURES_FIXED_VREF_Pos))
#define FEATURES_DEBUG_CONST_PWM_PD18_Pos           _UINT32_(5)
#define FEATURES_DEBUG_CONST_PWM_PD18_Msk           (_UINT32_(0x1) << FEATURES_DEBUG_CONST_PWM_PD18_Pos)
#define FEATURES_DEBUG_CONST_PWM_PD18(value)        (FEATURES_DEBUG_CONST_PWM_PD18_Msk & (_UINT32_(value) << FEATURES_DEBUG_CONST_PWM_PD18_Pos))
#define FEATURES_DEBUG_DAC_PD19_Pos                 _UINT32_(6)
#define FEATURES_DEBUG_DAC_PD19_Msk                 (_UINT32_(0x1) << FEATURES_DEBUG_DAC_PD19_Pos)
#define FEATURES_DEBUG_DAC_PD19(value)              (FEATURES_DEBUG_DAC_PD19_Msk & (_UINT32_(value) << FEATURES_DEBUG_DAC_PD19_Pos))
#define FEATURES_DEBUG_FLEX_PC0_PC22_Pos            _UINT32_(7)
#define FEATURES_DEBUG_FLEX_PC0_PC22_Msk            (_UINT32_(0x1) << FEATURES_DEBUG_FLEX_PC0_PC22_Pos)
#define FEATURES_DEBUG_FLEX_PC0_PC22(value)         (FEATURES_DEBUG_FLEX_PC0_PC22_Msk & (_UINT32_(value) << FEATURES_DEBUG_FLEX_PC0_PC22_Pos))
#define FEATURES_HALF_COPROC_CLK_Pos                _UINT32_(8)
#define FEATURES_HALF_COPROC_CLK_Msk                (_UINT32_(0x1) << FEATURES_HALF_COPROC_CLK_Pos)
#define FEATURES_HALF_COPROC_CLK(value)             (FEATURES_HALF_COPROC_CLK_Msk & (_UINT32_(value) << FEATURES_HALF_COPROC_CLK_Pos))
#define FEATURES_I_N_MUXING_Pos                     _UINT32_(9)
#define FEATURES_I_N_MUXING_Msk                     (_UINT32_(0x1) << FEATURES_I_N_MUXING_Pos)
#define FEATURES_I_N_MUXING(value)                  (FEATURES_I_N_MUXING_Msk & (_UINT32_(value) << FEATURES_I_N_MUXING_Pos))
#define FEATURES_PQ_OFFSET_X_Pos                     _UINT32_(10)
#define FEATURES_PQ_OFFSET_X_Msk                    (_UINT32_(0x1) << FEATURES_PQ_OFFSET_X_Pos)
#define FEATURES_PQ_OFFSET_X(value)                 (FEATURES_PQ_OFFSET_X_Msk & (_UINT32_(value) << FEATURES_PQ_OFFSET_X_Pos))
#define FEATURES_RZC_DETECT_Pos                     _UINT32_(11)
#define FEATURES_RZC_DETECT_Msk                     (_UINT32_(0x1) << FEATURES_RZC_DETECT_Pos)
#define FEATURES_RZC_DETECT(value)                  (FEATURES_RZC_DETECT_Msk & (_UINT32_(value) << FEATURES_RZC_DETECT_Pos))
#define FEATURES_CORE_CLK_SPEED_Pos                 _UINT32_(12)
#define FEATURES_CORE_CLK_SPEED_Msk                 (_UINT32_(0xF) << FEATURES_CORE_CLK_SPEED_Pos)
#define FEATURES_CORE_CLK_SPEED(value)              (FEATURES_CORE_CLK_SPEED_Msk & (_UINT32_(value) << FEATURES_CORE_CLK_SPEED_Pos))
#define   FEATURES_CORE_CLK_SPEED_UNDEFINED_Val       _UINT32_(0x0)                   /* (FEATURES) Compiled to be used with Core-0 clock = undefined  */
#define   FEATURES_CORE_CLK_SPEED_102_Val             _UINT32_(0x1)                   /* (FEATURES) Compiled to be used with Core-0 clock = 102.400MHz */
#define   FEATURES_CORE_CLK_SPEED_106_Val             _UINT32_(0x2)                   /* (FEATURES) Compiled to be used with Core-0 clock = 106.496MHz */
#define   FEATURES_CORE_CLK_SPEED_110_Val             _UINT32_(0x3)                   /* (FEATURES) Compiled to be used with Core-0 clock = 110.592MHz */
#define   FEATURES_CORE_CLK_SPEED_114_Val             _UINT32_(0x4)                   /* (FEATURES) Compiled to be used with Core-0 clock = 114.688MHz */
#define   FEATURES_CORE_CLK_SPEED_118_Val             _UINT32_(0x5)                   /* (FEATURES) Compiled to be used with Core-0 clock = 118.784MHz */
#define   FEATURES_CORE_CLK_SPEED_240_Val             _UINT32_(0x6)                   /* (FEATURES) Compiled to be used with Core-0 clock = 240.000MHz */
#define   FEATURES_CORE_CLK_SPEED_237_Val             _UINT32_(0x7)                   /* (FEATURES) Compiled to be used with Core-0 clock = 237.568MHz */
#define FEATURES_DFT_ENABLED_Pos                    _UINT32_(16)
#define FEATURES_DFT_ENABLED_Msk                    (_UINT32_(0x1) << FEATURES_DFT_ENABLED_Pos)
#define FEATURES_DFT_ENABLED(value)                 (FEATURES_DFT_ENABLED_Msk & (_UINT32_(value) << FEATURES_DFT_ENABLED_Pos))
#define FEATURES_CREEP_Pos                          _UINT32_(17)
#define FEATURES_CREEP_Msk                          (_UINT32_(0x1) << FEATURES_CREEP_Pos)
#define FEATURES_CREEP(value)                       (FEATURES_CREEP_Msk & (_UINT32_(value) << FEATURES_CREEP_Pos))
#define FEATURES_CAPTURE_Pos                        _UINT32_(18)
#define FEATURES_CAPTURE_Msk                        (_UINT32_(0x1) << FEATURES_CAPTURE_Pos)
#define FEATURES_CAPTURE(value)                     (FEATURES_CAPTURE_Msk & (_UINT32_(value) << FEATURES_CAPTURE_Pos))
#define FEATURES_ROGOWSKI_DC_REMOVE_Pos             _UINT32_(19)
#define FEATURES_ROGOWSKI_DC_REMOVE_Msk             (_UINT32_(0x1) << FEATURES_ROGOWSKI_DC_REMOVE_Pos)
#define FEATURES_ROGOWSKI_DC_REMOVE(value)          (FEATURES_ROGOWSKI_DC_REMOVE_Msk & (_UINT32_(value) << FEATURES_ROGOWSKI_DC_REMOVE_Pos))
#define FEATURES_NUM_PULSES_Pos                     _UINT32_(20)
#define FEATURES_NUM_PULSES_Msk                     (_UINT32_(0x3) << FEATURES_NUM_PULSES_Pos)
#define FEATURES_NUM_PULSES(value)                  (FEATURES_NUM_PULSES_Msk & (_UINT32_(value) << FEATURES_NUM_PULSES_Pos))
#define FEATURES_ATSENSE_LOC_Pos                    _UINT32_(22)
#define FEATURES_ATSENSE_LOC_Msk                    (_UINT32_(0x1) << FEATURES_ATSENSE_LOC_Pos)
#define FEATURES_ATSensE_LOC(value)                 (FEATURES_ATSENSE_LOC_Msk & (_UINT32_(value) << FEATURES_ATSENSE_LOC_Pos))
#define FEATURES_MODIFY_CHIPID_Pos                  _UINT32_(23)
#define FEATURES_MODIFY_CHIPID_Msk                  (_UINT32_(0x1) << FEATURES_MODIFY_CHIPID_Pos)
#define FEATURES_MODIFY_CHIPID(value)               (FEATURES_MODIFY_CHIPID_Msk & (_UINT32_(value) << FEATURES_MODIFY_CHIPID_Pos))
#define FEATURES_Fs_SAMPLE_RATE_Pos                 _UINT32_(24)
#define FEATURES_Fs_SAMPLE_RATE_Msk                 (_UINT32_(0x1) << FEATURES_Fs_SAMPLE_RATE_Pos)
#define FEATURES_Fs_SAMPLE_RATE(value)              (FEATURES_Fs_SAMPLE_RATE_Msk & (_UINT32_(value) << FEATURES_Fs_SAMPLE_RATE_Pos))
#define   FEATURES_Fs_SAMPLE_RATE_4MHz_Val            _UINT32_(0x0)                   /* (FEATURES) Baseband sample rate FS = 4.000KHz  */
#define   FEATURES_Fs_SAMPLE_RATE_8MHz_Val            _UINT32_(0x1)                   /* (FEATURES) Baseband sample rate FS = 8.000KHz  */
#define FEATURES_I_Ni_SOURCE_Pos                    _UINT32_(25)
#define FEATURES_I_Ni_SOURCE_Msk                    (_UINT32_(0x1) << FEATURES_I_Ni_SOURCE_Pos)
#define FEATURES_I_Ni_SOURCE(value)                 (FEATURES_I_Ni_SOURCE_Msk & (_UINT32_(value) << FEATURES_I_Ni_SOURCE_Pos))
#define FEATURES_RCZ_OUT_Pos                        _UINT32_(26)
#define FEATURES_RCZ_OUT_Msk                        (_UINT32_(0x1) << FEATURES_RCZ_OUT_Pos)
#define FEATURES_RCZ_OUT(value)                     (FEATURES_RCZ_OUT_Msk & (_UINT32_(value) << FEATURES_RCZ_OUT_Pos))
#define FEATURES_DEBUG_ENABLE_VREF_TEST_DATA_Pos    _UINT32_(27)
#define FEATURES_DEBUG_ENABLE_VREF_TEST_DATA_Msk    (_UINT32_(0x1) << FEATURES_DEBUG_ENABLE_VREF_TEST_DATA_Pos)
#define FEATURES_DEBUG_ENABLE_VREF_TEST_DATA(value) (FEATURES_DEBUG_ENABLE_VREF_TEST_DATA_Msk & (_UINT32_(value) << FEATURES_DEBUG_ENABLE_VREF_TEST_DATA_Pos))
#define FEATURES_METER_TYPE_10_Pos                  _UINT32_(28)
#define FEATURES_METER_TYPE_10_Msk                  (_UINT32_(0x1) << FEATURES_METER_TYPE_10_Pos)
#define FEATURES_METER_TYPE_10(value)               (FEATURES_METER_TYPE_10_Msk & (_UINT32_(value) << FEATURES_METER_TYPE_10_Pos))
#define FEATURES_METER_TYPE_09_Pos                  _UINT32_(29)
#define FEATURES_METER_TYPE_09_Msk                  (_UINT32_(0x1) << FEATURES_METER_TYPE_09_Pos)
#define FEATURES_METER_TYPE_09(value)               (FEATURES_METER_TYPE_09_Msk & (_UINT32_(value) << FEATURES_METER_TYPE_09_Pos))
#define FEATURES_SINGLEPHASE_Pos                    _UINT32_(30)
#define FEATURES_SINGLEPHASE_Msk                    (_UINT32_(0x1) << FEATURES_SINGLEPHASE_Pos)
#define FEATURES_SINGLEPHASE(value)                 (FEATURES_SINGLEPHASE_Msk & (_UINT32_(value) << FEATURES_SINGLEPHASE_Pos))
#define FEATURES_POLYPHASE_Pos                      _UINT32_(31)
#define FEATURES_POLYPHASE_Msk                      (_UINT32_(0x1) << FEATURES_POLYPHASE_Pos)
#define FEATURES_POLYPHASE(value)                   (FEATURES_POLYPHASE_Msk & (_UINT32_(value) << FEATURES_POLYPHASE_Pos))

/* -------- ZC_N_VA : (MET_STATUS Offset: 0x78) (R/W  32) Zero-crossing sample number of phase-A voltage (uQ20.12) -------- */
#define ZC_N_VA_Pos                    _UINT32_(0)
#define ZC_N_VA_Msk                    (_UINT32_(0xFFFFFF) << ZC_N_VA_Pos)
#define ZC_N_VA_Val(value)             (ZC_N_VA_Msk & (_UINT32_(value) << ZC_N_VA_Pos))

/* -------- ZC_N_VB : (MET_STATUS Offset: 0x7C) (R/W  32) Zero-crossing sample number of phase-B voltage (uQ20.12) -------- */
#define ZC_N_VB_Pos                    _UINT32_(0)
#define ZC_N_VB_Msk                    (_UINT32_(0xFFFFFF) << ZC_N_VB_Pos)
#define ZC_N_VB_Val(value)             (ZC_N_VB_Msk & (_UINT32_(value) << ZC_N_VB_Pos))

/* -------- ZC_N_VC : (MET_STATUS Offset: 0x80) (R/W  32) Zero-crossing sample number of phase-C voltage (uQ20.12) -------- */
#define ZC_N_VC_Pos                    _UINT32_(0)
#define ZC_N_VC_Msk                    (_UINT32_(0xFFFFFF) << ZC_N_VC_Pos)
#define ZC_N_VC_Val(value)             (ZC_N_VC_Msk & (_UINT32_(value) << ZC_N_VC_Pos))

/* -------- ATSENSE_CAL_41_44 : (MET_STATUS Offset: 0x84) (R/W  32) ATSENSE Calibration Trim Data, Registers: 0x41 ? 0x44 -------- */
#define ATSENSE_CAL_41_44_REF_TL_11_8_Pos                   _UINT32_(0)
#define ATSENSE_CAL_41_44_REF_TL_11_8_Msk                   (_UINT32_(0xF) << ATSENSE_CAL_41_44_REF_TL_11_8_Pos)
#define ATSENSE_CAL_41_44_REF_TL_11_8(value)                (ATSENSE_CAL_41_44_REF_TL_11_8_Msk & (_UINT32_(value) << ATSENSE_CAL_41_44_REF_TL_11_8_Pos))
#define ATSENSE_CAL_41_44_REF_TL_0_7_Pos                    _UINT32_(8)
#define ATSENSE_CAL_41_44_REF_TL_0_7_Msk                    (_UINT32_(0xFF) << ATSENSE_CAL_41_44_REF_TL_0_7_Pos)
#define ATSENSE_CAL_41_44_REF_TL_0_7(value)                 (ATSENSE_CAL_41_44_REF_TL_0_7_Msk & (_UINT32_(value) << ATSENSE_CAL_41_44_REF_TL_0_7_Pos))
#define ATSENSE_CAL_41_44_TEMP_TL_11_8_Pos                  _UINT32_(16)
#define ATSENSE_CAL_41_44_TEMP_TL_11_8_Msk                  (_UINT32_(0xF) << ATSENSE_CAL_41_44_TEMP_TL_11_8_Pos)
#define ATSENSE_CAL_41_44_TEMP_TL_11_8(value)               (ATSENSE_CAL_41_44_TEMP_TL_11_8_Msk & (_UINT32_(value) << ATSENSE_CAL_41_44_TEMP_TL_11_8_Pos))
#define ATSENSE_CAL_41_44_TEMP_TL_0_7_Pos                   _UINT32_(24)
#define ATSENSE_CAL_41_44_TEMP_TL_0_7_Msk                   (_UINT32_(0xFF) << ATSENSE_CAL_41_44_TEMP_TL_0_7_Pos)
#define ATSENSE_CAL_41_44_TEMP_TL_0_7(value)                (ATSENSE_CAL_41_44_TEMP_TL_0_7_Msk & (_UINT32_(value) << ATSENSE_CAL_41_44_TEMP_TL_0_7_Pos))

/* -------- ATSENSE_CAL_45_48 : (MET_STATUS Offset: 0x88) (R/W  32) ATSENSE Calibration Trim Data, Registers: 0x45 ? 0x48 -------- */
#define ATSENSE_CAL_45_48_REF_TH_11_8_Pos                   _UINT32_(0)
#define ATSENSE_CAL_45_48_REF_TH_11_8_Msk                   (_UINT32_(0xF) << ATSENSE_CAL_45_48_REF_TH_11_8_Pos)
#define ATSENSE_CAL_45_48_REF_TH_11_8(value)                (ATSENSE_CAL_45_48_REF_TH_11_8_Msk & (_UINT32_(value) << ATSENSE_CAL_45_48_REF_TH_11_8_Pos))
#define ATSENSE_CAL_45_48_REF_TH_0_7_Pos                    _UINT32_(8)
#define ATSENSE_CAL_45_48_REF_TH_0_7_Msk                    (_UINT32_(0xFF) << ATSENSE_CAL_45_48_REF_TH_0_7_Pos)
#define ATSENSE_CAL_45_48_REF_TH_0_7(value)                 (ATSENSE_CAL_45_48_REF_TH_0_7_Msk & (_UINT32_(value) << ATSENSE_CAL_45_48_REF_TH_0_7_Pos))
#define ATSENSE_CAL_45_48_TEMP_TH_11_8_Pos                  _UINT32_(16)
#define ATSENSE_CAL_45_48_TEMP_TH_11_8_Msk                  (_UINT32_(0xF) << ATSENSE_CAL_45_48_TEMP_TH_11_8_Pos)
#define ATSENSE_CAL_45_48_TEMP_TH_11_8(value)               (ATSENSE_CAL_45_48_TEMP_TH_11_8_Msk & (_UINT32_(value) << ATSENSE_CAL_45_48_TEMP_TH_11_8_Pos))
#define ATSENSE_CAL_45_48_TEMP_TH_0_7_Pos                   _UINT32_(24)
#define ATSENSE_CAL_45_48_TEMP_TH_0_7_Msk                   (_UINT32_(0xFF) << ATSENSE_CAL_45_48_TEMP_TH_0_7_Pos)
#define ATSENSE_CAL_45_48_TEMP_TH_0_7(value)                (ATSENSE_CAL_45_48_TEMP_TH_0_7_Msk & (_UINT32_(value) << ATSENSE_CAL_45_48_TEMP_TH_0_7_Pos))

/* MISRA C-2012 deviation block end */

/** \brief Metrology Harmonic Analysis Output register API structure */
typedef struct
{
    __O  uint32_t I_A_m_R[31];            /**< Offset: 0x00 (R/  32) The real part of DFT result for current, phase-A, m-th harmonics (sQ13.18) */
    __O  uint32_t V_A_m_R[31];            /**< Offset: 0x7C (R/  32) The real part of DFT result for voltage, phase-A, m-th harmonics (sQ13.18) */
    __O  uint32_t I_B_m_R[31];            /**< Offset: 0xF8 (R/  32) The real part of DFT result for current, phase-B, m-th harmonics (sQ13.18) */
    __O  uint32_t V_B_m_R[31];            /**< Offset: 0x174 (R/  32) The real part of DFT result for voltage, phase-B, m-th harmonics (sQ13.18) */
    __O  uint32_t I_C_m_R[31];            /**< Offset: 0x1F0 (R/  32) The real part of DFT result for current, phase-C, m-th harmonics (sQ13.18) */
    __O  uint32_t V_C_m_R[31];            /**< Offset: 0x26C (R/  32) The real part of DFT result for voltage, phase-C, m-th harmonics (sQ13.18) */
    __O  uint32_t I_N_m_R[31];            /**< Offset: 0x2E8 (R/  32) The real part of DFT result for voltage, phase-N, m-th harmonics (sQ13.18) */
    __O  uint32_t I_A_m_I[31];            /**< Offset: 0x364 (R/  32) The imaginary part of DFT result for current, phase-A, m-th harmonics (sQ13.18) */
    __O  uint32_t V_A_m_I[31];            /**< Offset: 0x3E0 (R/  32) The imaginary part of DFT result for voltage, phase-A, m-th harmonics (sQ13.18) */
    __O  uint32_t I_B_m_I[31];            /**< Offset: 0x45C (R/  32) The imaginary part of DFT result for current, phase-B, m-th harmonics (sQ13.18) */
    __O  uint32_t V_B_m_I[31];            /**< Offset: 0x4D8 (R/  32) The imaginary part of DFT result for voltage, phase-B, m-th harmonics (sQ13.18) */
    __O  uint32_t I_C_m_I[31];            /**< Offset: 0x554 (R/  32) The imaginary part of DFT result for current, phase-C, m-th harmonics (sQ13.18) */
    __O  uint32_t V_C_m_I[31];            /**< Offset: 0x5D0 (R/  32) The imaginary part of DFT result for voltage, phase-C, m-th harmonics (sQ13.18) */
    __O  uint32_t I_N_m_I[31];            /**< Offset: 0x64C (R/  32) The imaginary part of DFT result for voltage, phase-N, m-th harmonics (sQ13.18) */
} DRV_METROLOGY_REGS_HARMONICS;

/** \brief Metrology Accumulated Output register API structure */
typedef struct
{
    __O  uint64_t I_A;               /**< Offset: 0x08  (R/  32) Amp-squared-samples, phase-A, fundamental + harmonics (uQ24.40) */
    __O  uint64_t I_B;               /**< Offset: 0x10  (R/  32) Amp-squared-samples, phase-B, fundamental + harmonics (uQ24.40) */
    __O  uint64_t I_C;               /**< Offset: 0x18  (R/  32) Amp-squared-samples, phase-C, fundamental + harmonics (uQ24.40) */
    __O  uint64_t I_Ni;              /**< Offset: 0x20  (R/  32) Amp-squared-samples, phase-N inputed, fundamental + harmonics (uQ44.20) */
    __O  uint64_t I_Nm;              /**< Offset: 0x28  (R/  32) Amp-squared-samples, phase-N measured, fundamental + harmonics (uQ44.20) */
    __O  uint64_t I_A_F;             /**< Offset: 0x30  (R/  32) Amp-squared-samples, phase-A, fundamental only (uQ24.40) */
    __O  uint64_t I_B_F;             /**< Offset: 0x38  (R/  32) Amp-squared-samples, phase-B, fundamental only (uQ24.40) */
    __O  uint64_t I_C_F;             /**< Offset: 0x40  (R/  32) Amp-squared-samples, phase-C, fundamental only (uQ24.40) */
    __O  uint64_t I_Nmi;             /**< Offset: 0x48  (R/  32) Amp-squared-samples, phase-N, fundamental only (uQ44.20) */
    __O  uint64_t I_Nm_F;            /**< Offset: 0x50  (R/  32) Amp-squared-samples, phase-N, fundamental only (uQ24.40) */
    __O  uint64_t Reserved1[5];
    __O  int64_t  P_A;               /**< Offset: 0x78  (R/  32) Watt-samples, phase-A, fundamental + harmonics (sQ23.40) */
    __O  int64_t  P_B;               /**< Offset: 0x80  (R/  32) Watt-samples, phase-B, fundamental + harmonics (sQ23.40) */
    __O  int64_t  P_C;               /**< Offset: 0x88  (R/  32) Watt-samples, phase-C, fundamental + harmonics (sQ23.40) */
    __O  int64_t  P_A_F;             /**< Offset: 0x90  (R/  32) Watt-samples, phase-A, fundamental only (sQ23.40) */
    __O  int64_t  P_B_F;             /**< Offset: 0x98  (R/  32) Watt-samples, phase-B, fundamental only (sQ23.40) */
    __O  int64_t  P_C_F;             /**< Offset: 0xA0  (R/  32) Watt-samples, phase-C, fundamental only (sQ23.40) */
    __O  int64_t  P_N;               /**< Offset: 0xA8  (R/  32) Watt-samples, phase-N, fundamental only (sQ23.40) */
    __O  int64_t  P_N_F;             /**< Offset: 0xB0  (R/  32) Watt-samples, phase-N, fundamental only (sQ23.40) */
    __O  uint64_t Reserved2;
    __O  int64_t  Q_A;               /**< Offset: 0xC0  (R/  32) VAR-samples, phase-A, fundamental + harmonics (sQ23.40) */
    __O  int64_t  Q_B;               /**< Offset: 0xC8  (R/  32) VAR-samples, phase-B, fundamental + harmonics (sQ23.40) */
    __O  int64_t  Q_C;               /**< Offset: 0xD0  (R/  32) VAR-samples, phase-C, fundamental + harmonics (sQ23.40) */
    __O  int64_t  Q_A_F;             /**< Offset: 0xD8  (R/  32) VAR-samples, phase-A, fundamental only (sQ23.40) */
    __O  int64_t  Q_B_F;             /**< Offset: 0xE0  (R/  32) VAR-samples, phase-B, fundamental only (sQ23.40) */
    __O  int64_t  Q_C_F;             /**< Offset: 0xE8  (R/  32) VAR-samples, phase-C, fundamental only (sQ23.40) */
    __O  int64_t  Q_N;               /**< Offset: 0xF0  (R/  32) VAR-samples, phase-N, fundamental only (sQ23.40) */
    __O  int64_t  Q_N_F;             /**< Offset: 0xF8  (R/  32) VAR-samples, phase-N, fundamental only (sQ23.40) */
    __O  uint64_t Reserved3;
    __O  uint64_t V_A;               /**< Offset: 0x108 (R/  32) Volt-square-samples, phase-A, fundamental + harmonics (sQ24.40) */
    __O  uint64_t V_B;               /**< Offset: 0x110 (R/  32) Volt-square-samples, phase-B, fundamental + harmonics (sQ24.40) */
    __O  uint64_t V_C;               /**< Offset: 0x118 (R/  32) Volt-square-samples, phase-C, fundamental + harmonics (sQ24.40) */
    __O  uint64_t Reserved4;
    __O  uint64_t V_A_F;             /**< Offset: 0x128 (R/  32) Volt-square-samples, phase-A, fundamental only (uQ24.40) */
    __O  uint64_t V_B_F;             /**< Offset: 0x130 (R/  32) Volt-square-samples, phase-B, fundamental only (uQ24.40 */
    __O  uint64_t V_C_F;             /**< Offset: 0x138 (R/  32) Volt-square-samples, phase-C, fundamental only (uQ24.40 */
    __O  uint64_t Reserved5;
    __O  uint64_t V_AB;              /**< Offset: 0x148 (R/  32) Volt-square-samples, VA - VB, fundamental + harmonics (uQ24.40 */
    __O  uint64_t V_BC;              /**< Offset: 0x150 (R/  32) Volt-square-samples, VB - VC, fundamental + harmonics (uQ24.40 */
    __O  uint64_t V_CA;              /**< Offset: 0x158 (R/  32) Volt-square-samples, VC - VA, fundamental + harmonics (uQ24.40 */
    __O  uint64_t V_AB_F;            /**< Offset: 0x160 (R/  32) Volt-square-samples, VA - VB, fundamental only (uQ24.40 */
    __O  uint64_t V_BC_F;            /**< Offset: 0x168 (R/  32) Volt-square-samples, VB - VC, fundamental only (uQ24.40 */
    __O  uint64_t V_CA_F;            /**< Offset: 0x170 (R/  32) Volt-square-samples, VC - VA, fundamental only (uQ24.40 */
    __O  uint64_t Reserved6[3];
    __O  int64_t  ACC_T0;            /**< Offset: 0x190 (R/  32) Total accumulated quantities specified by PC0_TYPE (sQ33.30) */
    __O  int64_t  ACC_T1;            /**< Offset: 0x198 (R/  32) Total accumulated quantities specified by PC1_TYPE (sQ33.30) */
    __O  int64_t  ACC_T2;            /**< Offset: 0x1A0 (R/  32) Total accumulated quantities specified by PC2_TYPE (sQ33.30) */
    __O  uint64_t Reserved7[2];
} DRV_METROLOGY_REGS_ACCUMULATORS;

/** \brief Metrology Status register API structure */
typedef struct
{
    __O  uint32_t VERSION;                 /**< Offset: 0x00 (R/  32) Metrology firmware version */
    __O  uint32_t STATUS;                  /**< Offset: 0x04 (R/  32) Metrology status register */
    __O  uint32_t STATE_FLAG;              /**< Offset: 0x08 (R/  32) Metrology state flags register */
    __O  uint32_t CAPTURE_STATUS;          /**< Offset: 0x0C (R/  32) Waveform Capture Function Status */
    __O  uint32_t INTERVAL_NUM;            /**< Offset: 0x10 (R/  32) Interval number */
    __O  uint32_t N;                       /**< Offset: 0x14 (R/  32) Number of Samples in the Last Measurement Interval (uQ16.0) */
    __O  uint32_t PH_OFFSET;               /**< Offset: 0x18 (R/  32) Phase Correction Filter Offset (sQ0.31) */
    __O  uint32_t FREQ;                    /**< Offset: 0x1C (R/  32) Active Line Voltage Fundamental Harmonic Frequency (uQ20.12) */
    __O  uint32_t FREQ_VA;                 /**< Offset: 0x20 (R/  32) Frequency of fundamental harmonic, VA (uQ20.12) */
    __O  uint32_t FREQ_VB;                 /**< Offset: 0x24 (R/  32) Frequency of fundamental harmonic, VB (uQ20.12) */
    __O  uint32_t FREQ_VC;                 /**< Offset: 0x28 (R/  32) Frequency of fundamental harmonic, VC (uQ20.12) */
    __O  uint32_t Reserved1;
    __O  uint32_t TEMPERATURE;             /**< Offset: 0x30 (R/  32) Sensor Temperature (sQ23.8) */
    __O  uint32_t I_A_MAX;                 /**< Offset: 0x34 (R/  32) Phase A Current Peak (sQ2.29) */
    __O  uint32_t I_B_MAX;                 /**< Offset: 0x38 (R/  32) Phase B Current Peak (sQ2.29) */
    __O  uint32_t I_C_MAX;                 /**< Offset: 0x3C (R/  32) Phase C Current Peak (sQ2.29) */
    __O  uint32_t I_Ni_MAX;                /**< Offset: 0x40 (R/  32) Neutral Current Peak (sQ16.15) */
    __O  uint32_t I_Nm_MAX;                /**< Offset: 0x44 (R/  32) Neutral Current Peak (sQ16.15) */
    __O  uint32_t V_A_MAX;                 /**< Offset: 0x48 (R/  32) Max phase-A measured current during measurement interval (sQ2.29)*/
    __O  uint32_t V_B_MAX;                 /**< Offset: 0x4C (R/  32) Max phase-B measured current during measurement interval (sQ2.29)*/
    __O  uint32_t V_C_MAX;                 /**< Offset: 0x50 (R/  32) Max phase-C measured current during measurement interval (sQ2.29)*/
    __O  uint32_t FEATURES;                /**< Offset: 0x54 (R/  32) Compiled feature indicator */
    __O  uint32_t Reserved2[3];
    __O  uint32_t PULSE0_COUNTER;          /**< Offset: 0x64 (R/  32) Counter of Pulse 0 */
    __O  uint32_t PULSE1_COUNTER;          /**< Offset: 0x68 (R/  32) Counter of Pulse 1 */
    __O  uint32_t PULSE2_COUNTER;          /**< Offset: 0x6C (R/  32) Counter of Pulse 2 */
    __O  uint32_t Reserved3[2];
    __O  uint32_t ZC_N_VA;                 /**< Offset: 0x78 (R/  32) Zero-crossing sample number of phase-A voltage (uQ20.12) */
    __O  uint32_t ZC_N_VB;                 /**< Offset: 0x7C (R/  32) Zero-crossing sample number of phase-B voltage (uQ20.12) */
    __O  uint32_t ZC_N_VC;                 /**< Offset: 0x80 (R/  32) Zero-crossing sample number of phase-C voltage (uQ20.12) */
    __O  uint32_t ATSENSE_CAL_41_44;       /**< Offset: 0x84 (R/  32) ATSENSE Calibration Trim Data, Registers: 0x41 ? 0x44 */
    __O  uint32_t ATSENSE_CAL_45_48;       /**< Offset: 0x88 (R/  32) ATSENSE Calibration Trim Data, Registers: 0x45 ? 0x48 */
} DRV_METROLOGY_REGS_STATUS;

/** \brief Metrology Control register API structure */
typedef struct
{
   __IO  uint32_t STATE_CTRL;              /**< Offset: 0x00 (R/W  32) Metrology state control register */
   __IO  uint32_t FEATURE_CTRL;            /**< Offset: 0x04 (R/W  32) Metrology feature setting register */
   __IO  uint32_t HARMONIC_CTRL;           /**< Offset: 0x08 (R/W  32) Metrology feature setting register for Harmonics */
   __IO  uint32_t METER_TYPE;              /**< Offset: 0x0C (R/W  32) Sensor type and service type settings register */
   __IO  uint32_t M;                       /**< Offset: 0x10 (R/W  32) Number of line cycles for integration period (uQ12.0) */
   __IO  uint32_t N_MAX;                   /**< Offset: 0x14 (R/W  32) Maximum number of samples in an integration period (uQ24.0) */
   __IO  uint32_t PULSE0_CTRL;             /**< Offset: 0x18 (R/W  32) Pulse 0 control: enable/disable, polarity, quantity type selection */
   __IO  uint32_t PULSE1_CTRL;             /**< Offset: 0x1C (R/W  32) Pulse 1 control: enable/disable, polarity, quantity type selection */
   __IO  uint32_t PULSE2_CTRL;             /**< Offset: 0x20 (R/W  32) Pulse 2 control: enable/disable, polarity, quantity type selection */
   __IO  uint32_t P_K_t;                   /**< Offset: 0x24 (R/W  32) Meter Active Power Pulse Constant (uQ8.24) */
   __IO  uint32_t Q_K_t;                   /**< Offset: 0x28 (R/W  32) Meter Reactive Power Pulse Constant (uQ8.24) */
   __IO  uint32_t I_K_t;                   /**< Offset: 0x2C (R/W  32) Meter Current Pulse Constant (uQ8.24) */
   __IO  uint32_t CREEP_THRESHOLD_P;       /**< Offset: 0x30 (R/W  32) Creep Threshold for Active Energy (uQ2.30) */
   __IO  uint32_t CREEP_THRESHOLD_Q;       /**< Offset: 0x34 (R/W  32) Creep Threshold for Quadergy (uQ2.30) */
   __IO  uint32_t CREEP_THRESHOLD_I;       /**< Offset: 0x38 (R/W  32) Creep Threshold of Phase Current (uQ2.30) */
   __IO  uint32_t POWER_OFFSET_CTRL;       /**< Offset: 0x3C (R/W  32) Power offset control */
   __IO  uint32_t POWER_OFFSET_P;          /**< Offset: 0x40 (R/W  32) Active Power Offset (sQ1.30) */
   __IO  uint32_t POWER_OFFSET_Q;          /**< Offset: 0x44 (R/W  32) Reactive Power Offset (sQ1.30) */
   __IO  uint32_t SWELL_THRESHOLD_VA;      /**< Offset: 0x48 (R/W  32) Voltage swell threshold for each half cycle of phase-A voltage (uQ0.32) */
   __IO  uint32_t SWELL_THRESHOLD_VB;      /**< Offset: 0x4C (R/W  32) Voltage swell threshold for each half cycle of phase-B voltage (uQ0.32) */
   __IO  uint32_t SWELL_THRESHOLD_VC;      /**< Offset: 0x50 (R/W  32) Voltage swell threshold for each half cycle of phase-C voltage (uQ0.32) */
   __IO  uint32_t SAG_THRESHOLD_VA;        /**< Offset: 0x54 (R/W  32) Voltage sag threshold for each half cycle of phase-A voltage (uQ0.32) */
   __IO  uint32_t SAG_THRESHOLD_VB;        /**< Offset: 0x58 (R/W  32) Voltage sag threshold for each half cycle of phase-B voltage (uQ0.32) */
   __IO  uint32_t SAG_THRESHOLD_VC;        /**< Offset: 0x5C (R/W  32) Voltage sag threshold for each half cycle of phase-B voltage (uQ0.32) */
   __IO  uint32_t K_IA;                    /**< Offset: 0x60 (R/W  32) ADC input current conversion factor = I_A(rms)/Vadc(rms) (uQ22.10) */
   __IO  uint32_t K_VA;                    /**< Offset: 0x64 (R/W  32) ADC input voltage conversion factor = V_A(rms)/Vadc(rms) (uQ22.10) */
   __IO  uint32_t K_IB;                    /**< Offset: 0x68 (R/W  32) ADC input current conversion factor = I_B(rms)/Vadc(rms) (uQ22.10) */
   __IO  uint32_t K_VB;                    /**< Offset: 0x6C (R/W  32) ADC input voltage conversion factor = V_B(rms)/Vadc(rms) (uQ22.10) */
   __IO  uint32_t K_IC;                    /**< Offset: 0x70 (R/W  32) ADC input current conversion factor = I_C(rms)/Vadc(rms) (uQ22.10) */
   __IO  uint32_t K_VC;                    /**< Offset: 0x74 (R/W  32) ADC input voltage conversion factor = V_C(rms)/Vadc(rms) (uQ22.10) */
   __IO  uint32_t K_IN;                    /**< Offset: 0x78 (R/W  32) ADC input current conversion factor = I_N(rms)/Vadc(rms) (uQ22.10) */
   __IO  uint32_t CAL_M_IA;                /**< Offset: 0x7C (R/W  32) Current magnitude calibration constant, I_A  (sQ2.29) */
   __IO  uint32_t CAL_M_VA;                /**< Offset: 0x80 (R/W  32) Voltage magnitude calibration constant, V_A  (sQ2.29) */
   __IO  uint32_t CAL_M_IB;                /**< Offset: 0x84 (R/W  32) Current magnitude calibration constant, I_B  (sQ2.29) */
   __IO  uint32_t CAL_M_VB;                /**< Offset: 0x88 (R/W  32) Voltage magnitude calibration constant, V_B  (sQ2.29) */
   __IO  uint32_t CAL_M_IC;                /**< Offset: 0x8C (R/W  32) Current magnitude calibration constant, I_C  (sQ2.29) */
   __IO  uint32_t CAL_M_VC;                /**< Offset: 0x90 (R/W  32) Voltage magnitude calibration constant, V_C  (sQ2.29) */
   __IO  uint32_t CAL_M_IN;                /**< Offset: 0x94 (R/W  32) Current magnitude calibration constant, I_N  (sQ2.29) */
   __IO  uint32_t CAL_PH_IA;               /**< Offset: 0x98 (R/W  32) Current phase calibration constant, I_A (sQ0.31) */
   __IO  uint32_t CAL_PH_VA;               /**< Offset: 0x9C (R/W  32) Voltage phase calibration constant, V_A (sQ0.31) */
   __IO  uint32_t CAL_PH_IB;               /**< Offset: 0xA0 (R/W  32) Current phase calibration constant, I_B (sQ0.31) */
   __IO  uint32_t CAL_PH_VB;               /**< Offset: 0xA4 (R/W  32) Voltage phase calibration constant, V_B (sQ0.31) */
   __IO  uint32_t CAL_PH_IC;               /**< Offset: 0xA8 (R/W  32) Current phase calibration constant, I_C (sQ0.31) */
   __IO  uint32_t CAL_PH_VC;               /**< Offset: 0xAC (R/W  32) Voltage phase calibration constant, V_C (sQ0.31) */
   __IO  uint32_t CAL_PH_IN;               /**< Offset: 0xB0 (R/W  32) Current phase calibration constant, I_N (sQ0.31) */
   __IO  uint32_t CAPTURE_CTRL;            /**< Offset: 0xB4 (R/W  32) Waveform capture control register */
   __IO  uint32_t CAPTURE_BUFF_SIZE;       /**< Offset: 0xB8 (R/W  32) Waveform capture buffer size in units of 32-bits */
   __IO  uint32_t CAPTURE_ADDR;            /**< Offset: 0xBC (R/W  32) Waveform capture buffer address pointer */
   __IO  uint32_t Reserved1;
   __IO  uint32_t Reserved2;
   __IO  uint32_t Reserved3;
   __IO  uint32_t ATSENSE_CTRL_20_23;      /**< Offset: 0xCC (R/W  32) ATSense control: channels I0,I1,V1,I2 */
   __IO  uint32_t ATSENSE_CTRL_24_27;      /**< Offset: 0xD0 (R/W  32) ATSense control: channels V2,I3,V3,ANA */
   __IO  uint32_t ATSENSE_CTRL_28_2B;      /**< Offset: 0xD4 (R/W  32) ATSense control: Registers: x28, x2A-2B, x2D */
   __IO  uint32_t Reserved4;
   __IO  uint32_t POWER_OFFSET_P_A;        /**< Offset: 0xDC (R/W  32) Phase-A active power offset (sQ9.40) */
   __IO  uint32_t POWER_OFFSET_P_B;        /**< Offset: 0xE0 (R/W  32) Phase-B active power offset (sQ9.40) */
   __IO  uint32_t POWER_OFFSET_P_C;        /**< Offset: 0xE4 (R/W  32) Phase-C active power offset (sQ9.40) */
   __IO  uint32_t POWER_OFFSET_Q_A;        /**< Offset: 0xE8 (R/W  32) Phase-A reactive power offset (sQ9.40) */
   __IO  uint32_t POWER_OFFSET_Q_B;        /**< Offset: 0xEC (R/W  32) Phase-B reactive power offset (sQ9.40) */
   __IO  uint32_t POWER_OFFSET_Q_C;        /**< Offset: 0xF0 (R/W  32) Phase-C reactive power offset (sQ9.40) */
} DRV_METROLOGY_REGS_CONTROL;

/** \brief METROLOGY register API structure */
typedef struct
{
    DRV_METROLOGY_REGS_CONTROL       MET_CONTROL;         /**< Offset: 0x0000 (R/W  32) Metrology Control Registers */
    DRV_METROLOGY_REGS_STATUS        MET_STATUS;          /**< Offset: 0x00F4 (R/   32) Metrology Status Registers */
    DRV_METROLOGY_REGS_ACCUMULATORS  MET_ACCUMULATORS;    /**< Offset: 0x0180 (R/   32) Metrology Accumulated Output Registers */
    DRV_METROLOGY_REGS_HARMONICS     MET_HARMONICS;       /**< Offset: 0x0338 (R/   32) Metrology Harmonic Analysis Output Registers */
} MET_REGISTERS;



#endif /* METROLOGY_REGS_H */
