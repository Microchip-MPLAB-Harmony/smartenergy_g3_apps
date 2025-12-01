/*******************************************************************************
  PLC PHY Coupling Service Library Interface Header File

  Company
    Microchip Technology Inc.

  File Name
    srv_pcoup.h

  Summary
    PLC PHY Coupling service library interface.

  Description
    The Microchip G3-PLC and PRIME implementations include default PHY layer
    configuration values optimized for the Evaluation Kits. With the help of
    the PHY Calibration Tool it is possible to obtain the optimal configuration
    values for the customer's hardware implementation. Refer to the online
    documentation for more details about the available configuration values and
    their purpose.

  Remarks:
    This service provides the required information to be included on PLC
    projects for PL360/PL460 in order to apply the custom calibration.
*******************************************************************************/

// DOM-IGNORE-BEGIN
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
// DOM-IGNORE-END

#ifndef SRV_PCOUP_H    // Guards against multiple inclusion
#define SRV_PCOUP_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "driver/plc/phy/drv_plc_phy.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

/* Default G3-PLC PHY band of the PLC transmission coupling */
#define SRV_PCOUP_DEFAULT_BAND                   G3_FCC

/* Equalization number of coefficients (number of carriers) for Main branch */
#define SRV_PCOUP_EQU_NUM_COEF                   72U

/* PLC PHY Coupling parameters for Main branch */
#define SRV_PCOUP_RMS_HIGH_TBL                   {1201, 850, 602, 427, 304, 217, 155, 112}
#define SRV_PCOUP_RMS_VLOW_TBL                   {4617, 3509, 2512, 1769, 1247, 882, 624, 443}
#define SRV_PCOUP_THRS_HIGH_TBL                  {0, 0, 0, 0, 0, 0, 0, 0, 1020, 721, 510, 362, 257, 183, 131, 95}
#define SRV_PCOUP_THRS_VLOW_TBL                  {0, 0, 0, 0, 0, 0, 0, 0, 10327, 7247, 5074, 3562, 2510, 1776, 1261, 896}
#define SRV_PCOUP_DACC_TBL                       {0x0UL, 0x0UL, 0x100UL, 0x100UL, 0x0UL, 0x0UL, \
                                                 0x4f5000ffUL, 0x1b1b1b1bUL, 0x0UL, 0x0UL, 0x6UL, 0x355UL, \
                                                 0x0UL, 0x1020f0UL, 0x355UL, 0x0UL, 0x1020ffUL}
#define SRV_PCOUP_GAIN_HIGH_TBL                  {49, 20, 256}
#define SRV_PCOUP_GAIN_VLOW_TBL                  {364, 180, 408}
#define SRV_PCOUP_NUM_TX_LEVELS                  8
#define SRV_PCOUP_LINE_DRV_CONF                  5

#define SRV_PCOUP_PRED_HIGH_TBL                  {0x7FFF, 0x7519, 0x6C54, 0x6476, 0x5E1A, 0x58E0, 0x5539, 0x51FC, 0x4FFA, 0x4EFA, 0x4F08, 0x4F36, 0x5006, 0x5182,  \
                                                  0x536B, 0x5503, 0x5674, 0x586A, 0x5A42, 0x5BB6, 0x5CC7, 0x5E5C, 0x5FEB, 0x610F, 0x61EF, 0x6356, 0x64E5, 0x65C1,  \
                                                  0x6664, 0x6721, 0x6881, 0x6886, 0x6924, 0x693D, 0x6A8F, 0x6A3C, 0x6AC5, 0x6A68, 0x6B60, 0x6B17, 0x6B3C, 0x6AEF,  \
                                                  0x6B6A, 0x6B46, 0x6AE5, 0x6A38, 0x6A61, 0x69FF, 0x6976, 0x6844, 0x681A, 0x67A6, 0x66B6, 0x6609, 0x65B8, 0x653D,  \
                                                  0x63FE, 0x62A1, 0x61C5, 0x614F, 0x6029, 0x5F81, 0x5FA3, 0x5F3D, 0x5EB8, 0x5E23, 0x5DC9, 0x5D56, 0x5CD2, 0x5CC2,  \
                                                  0x5D78, 0x5E49}
#define SRV_PCOUP_PRED_VLOW_TBL                  {0x7FFF, 0x7666, 0x6ED8, 0x6939, 0x650D, 0x6178, 0x5F4B, 0x5D03, 0x5B7B, 0x593E, 0x5784, 0x5465, 0x515B, 0x4E98,  \
                                                  0x4CDB, 0x4B46, 0x49F2, 0x495C, 0x48D0, 0x47B9, 0x459C, 0x44E1, 0x449D, 0x4487, 0x448A, 0x452A, 0x45B4, 0x462B,  \
                                                  0x45DA, 0x45CE, 0x4699, 0x46F5, 0x46B9, 0x47D9, 0x488E, 0x495C, 0x497C, 0x4AAF, 0x4AD6, 0x4BA0, 0x4B14, 0x4B7E,  \
                                                  0x4BA5, 0x4C44, 0x4C2A, 0x4DEF, 0x4DE4, 0x4E0F, 0x4DC3, 0x4DAB, 0x4D8B, 0x4EA7, 0x4F17, 0x4E98, 0x4F3C, 0x4EF0,  \
                                                  0x4EFA, 0x4EEB, 0x4FC6, 0x5109, 0x51A2, 0x51FC, 0x52D0, 0x535B, 0x52E4, 0x530D, 0x534B, 0x539D, 0x5468, 0x5515,  \
                                                  0x5577, 0x5629}


// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* PLC PHY Coupling data

  Summary:
    PLC PHY Coupling data.

  Description:
    This structure contains all the data required to set the PLC PHY Coupling
    parameters, for a specific transmission branch (associated to a G3-PLC
    PHY band).

  Remarks:
    Equalization coefficients are not stored in the structure, just pointers to
    arrays were they are actually stored. This allows to use the same type for
    different G3-PLC PHY bands.
*/

typedef struct
{
    /* Target RMS values in HIGH mode for dynamic Tx gain */
    uint32_t                         rmsHigh[8];

    /* Target RMS values in VLOW mode for dynamic Tx gain */
    uint32_t                         rmsVLow[8];

    /* Threshold RMS values in HIGH mode for dynamic Tx mode */
    uint32_t                         thrsHigh[16];

    /* Threshold RMS values in VLOW mode for dynamic Tx mode */
    uint32_t                         thrsVLow[16];

    /* Values for configuration of PLC DACC peripheral, according to hardware
       coupling design and PLC device (PL360/PL460) */
    uint32_t                         daccTable[17];

    /* Pointer to Tx equalization coefficients table in HIGH mode.
       There is one coefficient for each carrier in the used band */
    const uint16_t *                 equHigh;

    /* Pointer to Tx equalization coefficients table in VLOW mode.
       There is one coefficient for each carrier in the used band */
    const uint16_t *                 equVlow;

    /* Tx gain values for HIGH mode [HIGH_INI, HIGH_MIN, HIGH_MAX] */
    uint16_t                         gainHigh[3];

    /* Tx gain values for VLOW mode [VLOW_INI, VLOW_MIN, VLOW_MAX] */
    uint16_t                         gainVLow[3];

    /* Number of Tx attenuation levels (3 dB step) suppoting dynamic Tx mode */
    uint8_t                          numTxLevels;

    /* Size of Tx equalization coefficients table in bytes */
    uint8_t                          equSize;

    /* Configuration of the PLC Tx Line Driver, according to hardware coupling
       design and PLC device (PL360/PL460) */
    uint8_t                          lineDrvConf;

} SRV_PLC_PCOUP_DATA;

// *****************************************************************************
// *****************************************************************************
// Section: Service Interface Functions
// *****************************************************************************
// *****************************************************************************

/***************************************************************************
  Function:
    SRV_PLC_PCOUP_DATA * SRV_PCOUP_Get_Config(uint8_t phyBand)

  Summary:
    Get the PLC PHY Coupling parameters for the specified G3-PLC PHY band.

  Description:
    This function allows to get the PLC PHY Coupling parameters for the
    specified G3-PLC PHY band. These parameters can be sent to the PLC
    device through PLC Driver PIB interface (DRV_PLC_PHY_PIBSet).

  Precondition:
    None.

  Parameters:
    phyBand - G3-PLC PHY band for which the parameters are requested

  Returns:
    - Pointer PLC PHY Coupling parameters
      - if phyBand parameter is valid
    - NULL
      - if phyBand parameter is not valid

  Example:
    <code>
    SRV_PLC_PCOUP_DATA *pCoupValues;

    pCoupValues = SRV_PCOUP_Get_Config(G3_FCC);
    </code>

  Remarks:
    If SRV_PCOUP_Set_Config is used to set the PLC PHY Coupling parameters,
    this function is not needed.
  ***************************************************************************/

SRV_PLC_PCOUP_DATA * SRV_PCOUP_Get_Config(uint8_t phyBand);

/***************************************************************************
  Function:
    bool SRV_PCOUP_Set_Config(DRV_HANDLE handle, uint8_t phyBand);

  Summary:
    Set the PLC PHY Coupling parameters for the specified G3-PLC PHY band.

  Description:
    This function allows to set the PLC PHY Coupling parameters for the
    specified G3-PLC PHY band, using the PLC Driver PIB interface
    (DRV_PLC_PHY_PIBSet).

  Precondition:
    DRV_PLC_PHY_Open must have been called to obtain a valid
    opened device handle.

  Parameters:
    handle  - A valid instance handle, returned from DRV_PLC_PHY_Open
    phyBand - G3-PLC PHY band for which the parameters are requested

  Returns:
    - true
      - Successful configuration
    - false
      - if phyBand parameter is not valid
      - if there is an error when calling DRV_PLC_PHY_PIBSet

  Example:
    <code>
    bool result;

    result = SRV_PCOUP_Set_Config(handle, G3_FCC);
    </code>

  Remarks:
    None.
  ***************************************************************************/

bool SRV_PCOUP_Set_Config(DRV_HANDLE handle, uint8_t phyBand);

/***************************************************************************
  Function:
    uint8_t SRV_PCOUP_Get_Default_Phy_Band( void )

  Summary:
    Get the default G3-PLC PHY band.

  Description:
    This function allows to get the G3-PLC PHY band used by default.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    Default G3-PLC PHY band.

  Example:
    <code>
    uint8_t plcPhyBand;

    plcPhyBand = SRV_PCOUP_Get_Default_Phy_Band();
    SRV_PCOUP_Set_Config(plcPhyBand);
    </code>

  Remarks:
    None.
  ***************************************************************************/

uint8_t SRV_PCOUP_Get_Default_Phy_Band( void );

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif
// DOM-IGNORE-END

#endif //SRV_PCOUP_H
