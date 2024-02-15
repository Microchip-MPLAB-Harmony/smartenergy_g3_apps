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
#include "driver/plc/g3MacRt/drv_g3_macrt.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

/* Default branch of the PLC transmission coupling */
#define SRV_PCOUP_DEFAULT_BRANCH                 SRV_PLC_PCOUP_MAIN_BRANCH

/* Equalization number of coefficients (number of carriers) for Main branch */
#define SRV_PCOUP_EQU_NUM_COEF                   72U

/* PLC PHY Coupling parameters for Main branch */
#define SRV_PCOUP_RMS_HIGH_TBL                   {1313, 937, 667, 477, 342, 247, 180, 131}
#define SRV_PCOUP_RMS_VLOW_TBL                   {4329, 3314, 2387, 1692, 1201, 853, 608, 432}
#define SRV_PCOUP_THRS_HIGH_TBL                  {0, 0, 0, 0, 0, 0, 0, 0, 1025, 729, 519, 372, 265, 191, 140, 101}
#define SRV_PCOUP_THRS_VLOW_TBL                  {0, 0, 0, 0, 0, 0, 0, 0, 10242, 7302, 5197, 3708, 2649, 1906, 1366, 979}
#define SRV_PCOUP_DACC_TBL                       {0x0UL, 0x0UL, 0x100UL, 0x100UL, 0x0UL, 0x0UL, \
                                                 0x4f5000ffUL, 0x1b1b1b1bUL, 0x0UL, 0x0UL, 0x6UL, 0x355UL, \
                                                 0x0UL, 0x1020f0UL, 0x355UL, 0x0UL, 0x1020ffUL}
#define SRV_PCOUP_GAIN_HIGH_TBL                  {49, 20, 256}
#define SRV_PCOUP_GAIN_VLOW_TBL                  {364, 180, 408}
#define SRV_PCOUP_NUM_TX_LEVELS                  8
#define SRV_PCOUP_LINE_DRV_CONF                  5

#define SRV_PCOUP_PRED_HIGH_TBL                  {0x7399, 0x6D5B, 0x6982, 0x671E, 0x6699, 0x6730, 0x6875, 0x6975, 0x6AE7, 0x6CE3, 0x6EF9, 0x70A7, 0x7276, 0x74B0, \
                                                  0x76BF, 0x77FE, 0x7905, 0x7A70, 0x7BC9, 0x7C88, 0x7D0A, 0x7DF6, 0x7EDF, 0x7F32, 0x7EF1, 0x7F6D, 0x7FFB, 0x7FFF, \
                                                  0x7F96, 0x7F76, 0x7F9D, 0x7EF8, 0x7E1B, 0x7D55, 0x7D2F, 0x7C3C, 0x7B39, 0x7A6C, 0x79CE, 0x790C, 0x779B, 0x76A4, \
                                                  0x7560, 0x7498, 0x72B8, 0x7185, 0x7049, 0x6F5D, 0x6DA6, 0x6C38, 0x6B46, 0x6A5E, 0x6940, 0x6855, 0x6802, 0x678A, \
                                                  0x6676, 0x6567, 0x654C, 0x6546, 0x651F, 0x65CD, 0x673D, 0x6876, 0x69C8, 0x6AD5, 0x6C7A, 0x6E1D, 0x6F4E, 0x70B3, \
                                                  0x72F9, 0x74E0}
#define SRV_PCOUP_PRED_VLOW_TBL                  {0x7FEC, 0x7D9A, 0x7BBA, 0x7987, 0x7752, 0x75E3, 0x7429, 0x71CE, 0x6FA1, 0x6E0A, 0x6C89, 0x6A9E, 0x68D7, 0x67BA, \
                                                  0x66CC, 0x655C, 0x63F4, 0x6318, 0x626F, 0x6186, 0x6093, 0x602C, 0x604E, 0x6022, 0x5F9A, 0x5FB6, 0x602F, 0x6049, \
                                                  0x6024, 0x608F, 0x615F, 0x61D9, 0x61E3, 0x6265, 0x6372, 0x6414, 0x6464, 0x6519, 0x6647, 0x672B, 0x679F, 0x6834, \
                                                  0x6959, 0x6A44, 0x6A93, 0x6B1F, 0x6C52, 0x6D4F, 0x6D98, 0x6E0E, 0x6F43, 0x7047, 0x70A5, 0x7136, 0x7258, 0x732C, \
                                                  0x7348, 0x7371, 0x7453, 0x7566, 0x75C8, 0x764F, 0x77A2, 0x78F2, 0x7929, 0x7990, 0x7AB0, 0x7B90, 0x7B35, 0x7C1E, \
                                                  0x7DE6, 0x7FFF}


// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
/* PLC PHY Coupling Branch definitions

 Summary:
    List of possible transmission branches.

 Description:
    This type defines the possible values of PLC transmission coupling branches.

 Remarks:
    None.
*/

typedef enum
{
    /* Main Transmission Branch */
    SRV_PLC_PCOUP_MAIN_BRANCH,

    /* Auxiliary Transmission Branch */
    SRV_PLC_PCOUP_AUXILIARY_BRANCH,

} SRV_PLC_PCOUP_BRANCH;

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
    SRV_PLC_PCOUP_DATA * SRV_PCOUP_Get_Config(SRV_PLC_PCOUP_BRANCH branch)

  Summary:
    Get the PLC PHY Coupling parameters for the specified transmission branch.

  Description:
    This function allows to get the PLC PHY Coupling parameters for the
    specified transmission branch. These parameters can be sent to the PLC
    device through PLC Driver PIB interface (DRV_G3_MACRT_PIBSet).

  Precondition:
    None.

  Parameters:
    branch          - Transmission branch for which the parameters are requested

  Returns:
    - Pointer PLC PHY Coupling parameters
      - if branch parameter is valid
    - NULL
      - if branch parameter is not valid

  Example:
    <code>
    SRV_PLC_PCOUP_DATA *pCoupValues;

    pCoupValues = SRV_PCOUP_Get_Config(SRV_PLC_PCOUP_MAIN_BRANCH);
    </code>

  Remarks:
    If SRV_PCOUP_Set_Config is used to set the PLC PHY Coupling parameters,
    this function is not needed.
  ***************************************************************************/

SRV_PLC_PCOUP_DATA * SRV_PCOUP_Get_Config(SRV_PLC_PCOUP_BRANCH branch);

/***************************************************************************
  Function:
    bool SRV_PCOUP_Set_Config(DRV_HANDLE handle, SRV_PLC_PCOUP_BRANCH branch);

  Summary:
    Set the PLC PHY Coupling parameters for the specified transmission branch.

  Description:
    This function allows to set the PLC PHY Coupling parameters for the
    specified transmission branch, using the PLC Driver PIB
    interface (DRV_G3_MACRT_PIBSet).

  Precondition:
    DRV_G3_MACRT_Open must have been called to obtain a valid
    opened device handle.

  Parameters:
    handle  - A valid instance handle, returned from DRV_G3_MACRT_Open
    branch  - Transmission branch for which the parameters will be set

  Returns:
    - true
      - Successful configuration
    - false
      - if branch parameter is not valid
      - if there is an error when calling DRV_G3_MACRT_PIBSet

  Example:
    <code>
    bool result;

    result = SRV_PCOUP_Set_Config(handle, SRV_PLC_PCOUP_MAIN_BRANCH);
    </code>

  Remarks:
    None.
  ***************************************************************************/

bool SRV_PCOUP_Set_Config(DRV_HANDLE handle, SRV_PLC_PCOUP_BRANCH branch);

/***************************************************************************
  Function:
    SRV_PLC_PCOUP_BRANCH SRV_PCOUP_Get_Default_Branch( void )

  Summary:
    Get the default branch of the PLC transmission coupling.

  Description:
    This function allows to get the tranmission branch used by default.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    Default transmission branch.

  Example:
    <code>
    SRV_PLC_PCOUP_BRANCH plcDefaultBranch;

    plcDefaultBranch = SRV_PCOUP_Get_Default_Branch();
    SRV_PCOUP_Set_Config(plcDefaultBranch);
    </code>

  Remarks:
    None.
  ***************************************************************************/

SRV_PLC_PCOUP_BRANCH SRV_PCOUP_Get_Default_Branch( void );

/***************************************************************************
  Function:
    uint8_t SRV_PCOUP_Get_Phy_Band(SRV_PLC_PCOUP_BRANCH branch)

  Summary:
    Get the G3-PLC PHY band associated to the specified transmission branch.

  Description:
    This function allows to get the G3-PLC PHY band associated to the
    specified transmission branch.

  Precondition:
    None.

  Parameters:
    branch         - Transmission branch from which the PHY band is requested

  Returns:
    G3-PLC PHY band associated to the specified transmission branch
    (see drv_g3_macrt_comm.h):
    - 0: G3_CEN_A
    - 1: G3_CEN_B
    - 2: G3_FCC
    - 3: G3_ARIB
    - 0xFF: G3_INVALID (if transmission branch is not valid)

  Example:
    <code>
    phyBand = SRV_PCOUP_Get_Phy_Band(SRV_PLC_PCOUP_MAIN_BRANCH);

    if (phyBand == G3_CEN_A)
    {

    }
    </code>

  Remarks:
    None.
  ***************************************************************************/

uint8_t SRV_PCOUP_Get_Phy_Band(SRV_PLC_PCOUP_BRANCH branch);

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif
// DOM-IGNORE-END

#endif //SRV_PCOUP_H
