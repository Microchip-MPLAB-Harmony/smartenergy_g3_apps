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
/*******************************************************************************
* Copyright (C) 2022 Microchip Technology Inc. and its subsidiaries.
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

#define SRV_PCOUP_EQU_NUM_COEF                   36

/* PLC PHY Coupling parameters for Main branch */
#define SRV_PCOUP_RMS_HIGH_TBL                   {2226, 1586, 1132, 805, 573, 408, 290, 206}
#define SRV_PCOUP_RMS_VLOW_TBL                   {5920, 4604, 3331, 2374, 1686, 1193, 846, 599}
#define SRV_PCOUP_THRS_HIGH_TBL                  {0, 0, 0, 0, 0, 0, 0, 0, 1884, 1341, 955, 677, 483, 341, 243, 173}
#define SRV_PCOUP_THRS_VLOW_TBL                  {0, 0, 0, 0, 0, 0, 0, 0, 9551, 6881, 4936, 3541, 2532, 1805, 1290, 922}
#define SRV_PCOUP_DACC_TBL                       {0x0, 0x0, 0x100, 0x100, 0x0, 0x0, \
                                                 0x9d5c00ff, 0x14141414, 0x0, 0x0, 0x4, 0x355, \
                                                 0x0, 0x1020f0, 0x355, 0x0, 0x1020ff}
#define SRV_PCOUP_GAIN_HIGH_TBL                  {126, 60, 336}
#define SRV_PCOUP_GAIN_VLOW_TBL                  {532, 230, 597}
#define SRV_PCOUP_NUM_TX_LEVELS                  8
#define SRV_PCOUP_LINE_DRV_CONF                  5

#define SRV_PCOUP_PRED_HIGH_TBL                  {0x5620, 0x59C7, 0x5E1E, 0x6333, 0x698B, 0x6F03, 0x72CD, 0x760E, 0x7904, 0x7B57, 0x7D2C, 0x7E72, 0x7F0F, 0x7FC6, \
                                                  0x7FFF, 0x7ED1, 0x7D11, 0x7BCE, 0x7A1A, 0x777C, 0x7496, 0x720F, 0x6F8E, 0x6BE0, 0x6780, 0x6357, 0x5F5E, 0x5C0C, \
                                                  0x597B, 0x5782, 0x572D, 0x57A2, 0x5823, 0x59F2, 0x5D86, 0x6153}
#define SRV_PCOUP_PRED_VLOW_TBL                  {0x7FFF, 0x7F81, 0x7E57, 0x7C6F, 0x7A35, 0x771F, 0x730B, 0x6E99, 0x6A40, 0x6654, 0x62C6, 0x5F77, 0x5CE6, 0x5B68, \
                                                  0x5A7B, 0x5A08, 0x5A66, 0x5BAD, 0x5D58, 0x5F29, 0x6109, 0x6338, 0x6539, 0x6686, 0x672E, 0x67D2, 0x686D, 0x68D2, \
                                                  0x68F6, 0x6927, 0x6995, 0x6989, 0x68C3, 0x68D1, 0x69AA, 0x6AC3}


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
    // 'handle', returned from DRV_G3_MACRT_Open
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
        // G3 CEN-A band
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
