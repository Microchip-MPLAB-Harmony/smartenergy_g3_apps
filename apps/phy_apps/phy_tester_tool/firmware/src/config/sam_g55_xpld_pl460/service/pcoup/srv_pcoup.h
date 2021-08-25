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
#define SRV_PCOUP_DEFAULT_BRANCH                 SRV_PLC_PCOUP_MAIN_BRANCH

/* PLC Phy Coupling Configuration Options */
#define SRV_PCOUP_MAX_NUM_TX_LEVELS              8

/* Equalization number of coefficients (number of carriers) */
#define SRV_PCOUP_EQU_NUM_COEF                   16

#define SRV_PCOUP_RMS_HIGH_TBL                   {1126, 800, 568, 404, 287, 204, 146, 104}
#define SRV_PCOUP_RMS_VLOW_TBL                   {3900, 2805, 1996, 1418, 1007, 715, 509, 362}
#define SRV_PCOUP_THRS_HIGH_TBL                  {0, 0, 0, 0, 0, 0, 0, 0, 998, 709, 504, 358, 254, 182, 129, 93}
#define SRV_PCOUP_THRS_VLOW_TBL                  {0, 0, 0, 0, 0, 0, 0, 0, 3900, 2805, 1996, 1418, 1007, 715, 509, 362}
#define SRV_PCOUP_DACC_TBL                       {0x0, 0x0, 0x100, 0x100, 0x0, 0x0, \
                                                 0x9c1800ff, 0x19191919, 0x0, 0x0, 0x4, 0x355, \
                                                 0x0, 0x1020f0, 0x355, 0x0, 0x1020ff}
#define SRV_PCOUP_GAIN_HIGH_TBL                  {221, 110, 496}
#define SRV_PCOUP_GAIN_VLOW_TBL                  {786, 350, 883}

#define SRV_PCOUP_LINE_DRV_CONF                  5

#define SRV_PCOUP_PRED_HIGH_TBL                  {0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, \
                                                  0x7FFF, 0x7FFF}
#define SRV_PCOUP_PRED_VLOW_TBL                  {0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, \
                                                  0x7FFF, 0x7FFF}  


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
