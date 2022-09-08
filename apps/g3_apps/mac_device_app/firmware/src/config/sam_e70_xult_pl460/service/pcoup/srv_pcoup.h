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
#define SRV_PCOUP_EQU_NUM_COEF                   72

/* PLC PHY Coupling parameters for Main branch */
#define SRV_PCOUP_RMS_HIGH_TBL                   {1064, 763, 549, 394, 283, 204, 148, 108}
#define SRV_PCOUP_RMS_VLOW_TBL                   {3614, 2775, 2009, 1431, 1019, 725, 516, 367}
#define SRV_PCOUP_THRS_HIGH_TBL                  {0, 0, 0, 0, 0, 0, 0, 0, 929, 668, 480, 345, 247, 179, 129, 94}
#define SRV_PCOUP_THRS_VLOW_TBL                  {0, 0, 0, 0, 0, 0, 0, 0, 9668, 6931, 4955, 3538, 2520, 1793, 1276, 909}
#define SRV_PCOUP_DACC_TBL                       {0x0, 0x0, 0x100, 0x100, 0x0, 0x0, \
                                                 0x4f5000ff, 0x1b1b1b1b, 0x0, 0x0, 0x6, 0x355, \
                                                 0x0, 0x1020f0, 0x355, 0x0, 0x1020ff}
#define SRV_PCOUP_GAIN_HIGH_TBL                  {105, 50, 256}
#define SRV_PCOUP_GAIN_VLOW_TBL                  {364, 180, 408}
#define SRV_PCOUP_NUM_TX_LEVELS                  8
#define SRV_PCOUP_LINE_DRV_CONF                  7

#define SRV_PCOUP_PRED_HIGH_TBL                  {0x6FFD, 0x6AD0, 0x65CF, 0x6073, 0x5AF7, 0x5618, 0x5158, 0x4CA7, 0x4869, 0x44EC, 0x4222, 0x3FD7, 0x3E4E, 0x3DB9, \
                                                  0x3DC3, 0x3E05, 0x3E97, 0x3F8B, 0x407B, 0x4130, 0x41D1, 0x4285, 0x4330, 0x4379, 0x4394, 0x43C5, 0x4407, 0x43FA, \
                                                  0x43C6, 0x43B2, 0x43C5, 0x43B2, 0x435D, 0x4359, 0x43AD, 0x43FB, 0x4437, 0x44CD, 0x45EC, 0x46C7, 0x47D3, 0x48F6, \
                                                  0x4ABD, 0x4C07, 0x4D9C, 0x4F0B, 0x5125, 0x52CE, 0x5479, 0x564A, 0x5844, 0x5A45, 0x5BE9, 0x5DAC, 0x5F88, 0x617E, \
                                                  0x62F0, 0x64A8, 0x66AA, 0x68AB, 0x6A56, 0x6BCB, 0x6DC4, 0x6F84, 0x70F2, 0x7268, 0x747B, 0x7660, 0x77D4, 0x79E1, \
                                                  0x7CD8, 0x7FFF}
#define SRV_PCOUP_PRED_VLOW_TBL                  {0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, \
                                                  0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, \
                                                  0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, \
                                                  0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, \
                                                  0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, \
                                                  0x7FFF, 0x7FFF}  

/* Equalization number of coefficients (number of carriers) for Auxiliary branch */
#define SRV_PCOUP_AUX_EQU_NUM_COEF               36

/* PLC PHY Coupling parameters for Auxiliary branch */
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
