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

/* PLC Phy Coupling Default Branch */
#define SRV_PCOUP_DEFAULT_BRANCH                 SRV_PLC_PCOUP_MAIN_BRANCH

/* Maximum value of PHY_PIB_NUM_TX_LEVELS */
/* Number of TX attenuation levels (3 dB step) suppoting automatic TX mode */
#define SRV_PCOUP_MAX_NUM_TX_LEVELS              8

/* Equalization number of coefficients (number of carriers) for Main branch */
#define SRV_PCOUP_EQU_NUM_COEF                   72

/* PLC Tx Coupling Settings for Main branch */
#define SRV_PCOUP_RMS_HIGH_TBL                   {1313, 937, 667, 477, 342, 247, 180, 131}
#define SRV_PCOUP_RMS_VLOW_TBL                   {4329, 3314, 2387, 1692, 1201, 853, 608, 432}
#define SRV_PCOUP_THRS_HIGH_TBL                  {0, 0, 0, 0, 0, 0, 0, 0, 1025, 729, 519, 372, 265, 191, 140, 101}
#define SRV_PCOUP_THRS_VLOW_TBL                  {0, 0, 0, 0, 0, 0, 0, 0, 10242, 7302, 5197, 3708, 2649, 1906, 1366, 979}
#define SRV_PCOUP_DACC_TBL                       {0x0, 0x0, 0x100, 0x100, 0x0, 0x0, \
                                                 0x4f5000ff, 0x1b1b1b1b, 0x0, 0x0, 0x6, 0x355, \
                                                 0x0, 0x1020f0, 0x355, 0x0, 0x1020ff}
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

/* Equalization number of coefficients (number of carriers) for Auxiliary branch */
#define SRV_PCOUP_AUX_EQU_NUM_COEF               36

/* PLC Tx Coupling Settings for Auxiliary branch */
#define SRV_PCOUP_AUX_RMS_HIGH_TBL               {1991, 1381, 976, 695, 495, 351, 250, 179}
#define SRV_PCOUP_AUX_RMS_VLOW_TBL               {6356, 4706, 3317, 2308, 1602, 1112, 778, 546}
#define SRV_PCOUP_AUX_THRS_HIGH_TBL              {0, 0, 0, 0, 0, 0, 0, 0, 1685, 1173, 828, 589, 419, 298, 212, 151}
#define SRV_PCOUP_AUX_THRS_VLOW_TBL              {0, 0, 0, 0, 0, 0, 0, 0, 8988, 6370, 4466, 3119, 2171, 1512, 1061, 752}
#define SRV_PCOUP_AUX_DACC_TBL                   {0x0, 0x21200000, 0x73f0000, 0x3f3f0000, 0xccc, 0x0, \
                                                 0xa20000ff, 0x14141414, 0x20200000, 0x4400, 0xfd20004, 0x3aa, \
                                                 0xf0000000, 0x1020f0, 0x3aa, 0xf0000000, 0x1020ff}
#define SRV_PCOUP_AUX_GAIN_HIGH_TBL              {142, 70, 336}
#define SRV_PCOUP_AUX_GAIN_VLOW_TBL              {474, 230, 597}
#define SRV_PCOUP_AUX_NUM_TX_LEVELS              8
#define SRV_PCOUP_AUX_LINE_DRV_CONF              8

#define SRV_PCOUP_AUX_PRED_HIGH_TBL              {0x670A, 0x660F, 0x676A, 0x6A6B, 0x6F3F, 0x7440, 0x74ED, 0x7792, 0x762D, 0x7530, 0x7938, 0x7C0A, 0x7C2A, 0x7B0E, \
                                                  0x7AF2, 0x784B, 0x7899, 0x76F9, 0x76D6, 0x769F, 0x775D, 0x70C0, 0x6EB9, 0x6F18, 0x6F1E, 0x6FA2, 0x6862, 0x67C9, \
                                                  0x68F9, 0x68A5, 0x6CA3, 0x7153, 0x7533, 0x750B, 0x7B59, 0x7FFF}
#define SRV_PCOUP_AUX_PRED_VLOW_TBL              {0x7FFF, 0x7DB1, 0x7CE6, 0x7B36, 0x772F, 0x7472, 0x70AA, 0x6BC2, 0x682D, 0x6618, 0x6384, 0x6210, 0x61D7, 0x6244, \
                                                  0x6269, 0x63A8, 0x6528, 0x65CC, 0x67F6, 0x693B, 0x6B13, 0x6C29, 0x6D43, 0x6E26, 0x6D70, 0x6C94, 0x6BB5, 0x6AC9, \
                                                  0x6A5F, 0x6B65, 0x6B8C, 0x6A62, 0x6CEC, 0x6D5A, 0x6F9D, 0x6FD3}  

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
/* PLC PHY Coupling Branch definitions

 Summary:
    Defines two independent transmission branches.

 Description:
    This will be used to indicate the branch to work with.

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
/* PLC PHY Tx Coupling Settings Data

  Summary:
    Defines the data required to set the PLC PHY Coupling parameters.

  Description:
    This data type defines the data required to set the PLC Tx Coupling PHY 
    parameters.

  Remarks:
    Tx equalization coefficients tables are not stored in the struct, just 
    pointers to them.
*/

typedef struct
{  
    /* Target RMS values in HIGH mode for dynamic Tx gain */
    uint32_t                         rmsHigh[SRV_PCOUP_MAX_NUM_TX_LEVELS];

    /* Target RMS values in VLOW mode for dynamic Tx gain */
    uint32_t                         rmsVLow[SRV_PCOUP_MAX_NUM_TX_LEVELS];

    /* Threshold RMS values in HIGH mode for dynamic Tx mode */
    uint32_t                         thrsHigh[SRV_PCOUP_MAX_NUM_TX_LEVELS << 1];

    /* Threshold RMS values in VLOW mode for dynamic Tx mode */
    uint32_t                         thrsVLow[SRV_PCOUP_MAX_NUM_TX_LEVELS << 1];

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

} SRV_PLC_PCOUP;

// *****************************************************************************
// *****************************************************************************
// Section: Service Interface Functions
// *****************************************************************************
// *****************************************************************************

/***************************************************************************
  Function:
    SRV_PLC_PCOUP * SRV_PCOUP_Get_Config(SRV_PLC_PCOUP_BRANCH branch)
    
  Summary:
    Get the PLC Tx Coupling PHY parameters for the desired transmission branch.

  Description:
    This function is used to get the PLC Tx Coupling PHY parameters for the 
    desired transmission branch. These parameters should be sent to the PLC 
    device through PLC Driver PIB interface.

  Precondition:
    None.

  Parameters:
    branch          - Transmission branch for which the parameters are requested

  Returns:
    NULL - Indicates an error in the PLC PHY Coupling settings.

    Pointer to PLC Tx Coupling PHY parameters.

  Example:
    <code>
    SRV_PLC_PCOUP *pCoupValues;

    pCoupValues = SRV_PCOUP_Get_Config(SRV_PLC_PCOUP_MAIN_BRANCH);
    </code>

  Remarks:
    None.
  ***************************************************************************/

SRV_PLC_PCOUP * SRV_PCOUP_Get_Config(SRV_PLC_PCOUP_BRANCH branch);

/***************************************************************************
  Function:
    bool SRV_PCOUP_Set_Config(DRV_HANDLE handle, SRV_PLC_PCOUP_BRANCH branch);
    
  Summary:
    Set the PLC Tx Coupling PHY parameters for the desired transmission branch.

  Description:
    This function is used to set the PLC Tx Coupling PHY parameters for the 
    desired transmission branch, using the PLC Driver PIB interface.

  Precondition:
    DRV_PLC_PHY_Open must have been called to obtain a valid driver handle.

  Parameters:
    handle  - PLC driver handle used to set PIB parameters
    branch  - Transmission branch for which the parameters will be set

  Returns:
    true    - Successful configuration
    false   - Failed configuration

  Example:
    <code>
    // 'handle', returned from the DRV_PLC_PHY_Open
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
    Get the default PLC PHY Tx Coupling Branch.

  Description:
    This function is used to get the default tranmission branch of the 
    PLC PHY Coupling.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    SRV_PLC_PCOUP_BRANCH - Indicates the tranmission branch by default.

  Example:
    <code>
    SRV_PLC_PCOUP_BRANCH plcDefaultBranch;
    SRV_PLC_PCOUP *pCoupValues;

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
    Get the PHY band of the branch that is selected.

  Description:
    This function is used to get the PHY band linked to the selected branch 
    of the PLC PHY Coupling.

  Precondition:
    None.

  Parameters:
    branch             - Tx branch from which the PHY band is requested

  Returns:
    PHY band linked to the coupling branch. See "drv_plc_phy_comm.h" .
    [G3_CEN_A, G3_CEN_B, G3_FCC, G3_ARIB, G3_INVALID]

  Example:
    <code>
    uint8_t pCoupPhyBand;

    pCoupPhyBand = SRV_PCOUP_Get_Phy_Band(SRV_PLC_PCOUP_MAIN_BRANCH);

    if (pCoupPhyBand == G3_CEN_A) {
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
