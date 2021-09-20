/*******************************************************************************
  PLC Coupling Service Library Interface Header File

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
    values for the customer�s hardware implementation. Please refer to the 
    PL360 Host Controller document for more details about the available 
    configuration values and their purpose. 

  Remarks:
    This provides the required information to be included on PLC PHY projects 
    for PL360/PL460 in order to apply the custom calibration.
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2021 Microchip Technology Inc. and its subsidiaries.
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
#include "system/system.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

/* PLC Phy Coupling Default Branch */
#define SRV_PCOUP_DEFAULT_BRANCH                 SRV_PLC_PCOUP_MAIN_BRANCH

/* PLC Phy Coupling Configuration Options */
#define SRV_PCOUP_MAX_NUM_TX_LEVELS              8

/* Equalization number of coefficients (number of carriers) */
#define SRV_PCOUP_EQU_NUM_COEF                   72

#define SRV_PCOUP_RMS_HIGH_TBL                   {5656, 4174, 2877, 1987, 1413, 1020, 736, 532}
#define SRV_PCOUP_RMS_VLOW_TBL                   {5656, 4174, 2877, 1987, 1413, 1020, 736, 532}
#define SRV_PCOUP_THRS_HIGH_TBL                  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
#define SRV_PCOUP_THRS_VLOW_TBL                  {0, 0, 0, 0, 0, 0, 0, 0, 8153, 5718, 4007, 2871, 2080, 1506, 1083, 778}
#define SRV_PCOUP_DACC_TBL                       {0x0, 0x21200000, 0x73f0000, 0x3f3f0000, 0xccc, 0x0, \
                                                 0x2a3000ff, 0x1b1b1b1b, 0x10100000, 0x1100, 0x4380006, 0x3aa, \
                                                 0xf0000000, 0x1020ff, 0x3aa, 0xf0000000, 0x1020ff}
#define SRV_PCOUP_GAIN_HIGH_TBL                  {364, 50, 256}
#define SRV_PCOUP_GAIN_VLOW_TBL                  {364, 50, 408}

#define SRV_PCOUP_LINE_DRV_CONF                  0

#define SRV_PCOUP_PRED_HIGH_TBL                  {0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, \
                                                  0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, \
                                                  0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, \
                                                  0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, \
                                                  0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, \
                                                  0x7FFF, 0x7FFF}
#define SRV_PCOUP_PRED_VLOW_TBL                  {0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, \
                                                  0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, \
                                                  0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, \
                                                  0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, \
                                                  0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, \
                                                  0x7FFF, 0x7FFF}  

/* PLC Phy Coupling Configuration Options */
#define SRV_PCOUP_AUX_MAX_NUM_TX_LEVELS          8

/* Equalization number of coefficients (number of carriers) for Auxiliary branch */
#define SRV_PCOUP_AUX_EQU_NUM_COEF               36

#define SRV_PCOUP_AUX_RMS_HIGH_TBL               {1991, 1381, 976, 695, 495, 351, 250, 179}
#define SRV_PCOUP_AUX_RMS_VLOW_TBL               {6356, 4706, 3317, 2308, 1602, 1112, 778, 546}
#define SRV_PCOUP_AUX_THRS_HIGH_TBL              {0, 0, 0, 0, 0, 0, 0, 0, 1685, 1173, 828, 589, 419, 298, 212, 151}
#define SRV_PCOUP_AUX_THRS_VLOW_TBL              {0, 0, 0, 0, 0, 0, 0, 0, 8988, 6370, 4466, 3119, 2171, 1512, 1061, 752}
#define SRV_PCOUP_AUX_DACC_TBL                   {0x0, 0x2120, 0x73f, 0x3f3f, 0x333, 0x0, \
                                                 0xa20000ff, 0x14141414, 0x2020, 0x44, 0xfd20004, 0x355, \
                                                 0xf000000, 0x1020f0, 0x355, 0xf000000, 0x1020ff}
#define SRV_PCOUP_AUX_GAIN_HIGH_TBL              {142, 70, 336}
#define SRV_PCOUP_AUX_GAIN_VLOW_TBL              {474, 230, 597}

#define SRV_PCOUP_AUX_LINE_DRV_CONF              0

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
/* PLC Phy Coupling Branch definitions

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
/* PLC Phy Coupling Interface Data

  Summary:
    Defines the data required to initialize the PLC PHY Coupling Interface.

  Description:
    This data type defines the data required to initialize the PLC PHY Coupling
    Interface.

  Remarks:
    None.
*/

typedef struct
{  
    /* Target RMS high values for each Transmission */
    uint32_t                               rmsHigh[SRV_PCOUP_MAX_NUM_TX_LEVELS];
    
    /* Target RMS very low values for each Transmission */
    uint32_t                               rmsVLow[SRV_PCOUP_MAX_NUM_TX_LEVELS];
    
    /* Table of thresholds to automatically update Tx Mode from HIGH mode */
    uint32_t                               thrsHigh[SRV_PCOUP_MAX_NUM_TX_LEVELS << 1];
    
    /* Table of thresholds to automatically update Tx Mode from VLOW mode */
    uint32_t                               thrsVLow[SRV_PCOUP_MAX_NUM_TX_LEVELS << 1];
    
    /* Configuration values of DACC peripheral according to hardware configuration */
    uint32_t                               daccTable[17];
    
    /* Table of gain values for HIGH Tx Mode [HIGH_INI, HIGH_MIN, HIGH_MAX] */
    uint16_t                               gainHigh[3];
    
    /* Table of gain values for VLOW Tx Mode [VLOW_INI, VLOW_MIN, VLOW_MAX] */
    uint16_t                               gainVLow[3];
    
    /* Number of Tx attenuation levels (3 dB steps) for normal transmission behavior */
    uint8_t                                numTxLevels;
    
    /* Size of Equalization Coefficients table in Tx mode in bytes. */
    uint8_t                                equSize;
    
    /* Configuration of the embedded PLC Line Driver */
    uint8_t                                lineDrvConf;
    
    /* Equalization Coefficients table in HIGH Tx mode. There is one coefficient for each carrier in the used band */
    uint16_t                               equHigh[SRV_PCOUP_EQU_NUM_COEF];
    
    /* Equalization Coefficients table in VLOW Tx mode. There is one coefficient for each carrier in the used band */
    uint16_t                               equVlow[SRV_PCOUP_EQU_NUM_COEF];

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
    Get the proper parameters to configure the PLC PHY Coupling according to
    the customer values.

  Description:
    This function is used to Get the proper parameters to configure the 
    PLC PHY Coupling. This parameters should be sent to the PLC device through
    PIB interface.

  Precondition:
    None.

  Parameters:
    pCoupValues        - Pointer to PLC PHY Coupling parameters.
    branch             - Branch from which the parameters are obtained

  Returns:
    SYS_STATUS_UNINITIALIZED - Indicates that has been an error in the coupling configuration.

    Pointer to PLC PHY Coupling parameters to be used.

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
    SRV_PLC_PCOUP_BRANCH SRV_PCOUP_Get_Default_Branch( void )
    
  Summary:
    Get the transmission branch that is configured by default.

  Description:
    This function is used to Get the default tranmission branch of the 
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
    pCoupValues = SRV_PCOUP_Get_Config(plcDefaultBranch);
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
    This function is used to Get the PHY band linked to the selected branch 
    of the PLC PHY Coupling.

  Precondition:
    None.

  Parameters:
    branch             - Branch from which the phy band is obtained

  Returns:
    PHY band linked to the coupling branch. See "drv_plc_phy_comm.h" .
    [G3_CEN_A, G3_CEN_B, G3_FCC, G3_ARIB, G3_INVALID]

  Example:
    <code>
    uint8_t pCoupPhyBand;

    pCoupPhyBand = SRV_PCOUP_Get_Config(SRV_PLC_PCOUP_MAIN_BRANCH);

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