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
    values for the customer´s hardware implementation. Please refer to the 
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
#define SRV_PCOUP_DEFAULT_BRANCH                 SRV_PLC_PCOUP_AUXILIARY_BRANCH

/* PLC Phy Coupling Configuration Options */
#define SRV_PCOUP_MAX_NUM_TX_LEVELS              8

/* Equalization number of coefficients (number of carriers) */
#define SRV_PCOUP_EQU_NUM_COEF                   36

#define SRV_PCOUP_RMS_HIGH_TBL                   {2226, 1586, 1132, 805, 573, 408, 290, 206}
#define SRV_PCOUP_RMS_VLOW_TBL                   {5920, 4604, 3331, 2374, 1686, 1193, 846, 599}
#define SRV_PCOUP_THRS_HIGH_TBL                  {0, 0, 0, 0, 0, 0, 0, 0, 1884, 1341, 955, 677, 483, 341, 243, 173}
#define SRV_PCOUP_THRS_VLOW_TBL                  {0, 0, 0, 0, 0, 0, 0, 0, 9551, 6881, 4936, 3541, 2532, 1805, 1290, 922}
#define SRV_PCOUP_DACC_TBL                       {0x0, 0x0, 0x100, 0x100, 0x0, 0x0, \
                                                 0x9d5c00ff, 0x14141414, 0x0, 0x0, 0x4, 0x355, \
                                                 0x0, 0x1020f0, 0x355, 0x0, 0x1020ff}
#define SRV_PCOUP_GAIN_HIGH_TBL                  {126, 60, 336}
#define SRV_PCOUP_GAIN_VLOW_TBL                  {532, 230, 597}

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
