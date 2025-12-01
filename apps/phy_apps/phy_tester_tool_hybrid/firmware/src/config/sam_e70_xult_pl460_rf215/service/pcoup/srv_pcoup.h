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
#define SRV_PCOUP_DEFAULT_BAND                   G3_CEN_A

/* Equalization number of coefficients (number of carriers) for Main branch */
#define SRV_PCOUP_EQU_NUM_COEF                   36U

/* PLC PHY Coupling parameters for Main branch */
#define SRV_PCOUP_RMS_HIGH_TBL                   {2146, 1532, 1093, 778, 554, 395, 283, 202}
#define SRV_PCOUP_RMS_VLOW_TBL                   {5838, 4515, 3272, 2333, 1657, 1175, 832, 590}
#define SRV_PCOUP_THRS_HIGH_TBL                  {0, 0, 0, 0, 0, 0, 0, 0, 1823, 1302, 929, 661, 471, 335, 240, 172}
#define SRV_PCOUP_THRS_VLOW_TBL                  {0, 0, 0, 0, 0, 0, 0, 0, 9433, 6840, 4924, 3539, 2535, 1824, 1307, 941}
#define SRV_PCOUP_DACC_TBL                       {0x0UL, 0x0UL, 0x100UL, 0x100UL, 0x0UL, 0x0UL, \
                                                 0x9d5c00ffUL, 0x14141414UL, 0x0UL, 0x0UL, 0x4UL, 0x355UL, \
                                                 0x0UL, 0x1020f0UL, 0x355UL, 0x0UL, 0x1020ffUL}
#define SRV_PCOUP_GAIN_HIGH_TBL                  {126, 60, 336}
#define SRV_PCOUP_GAIN_VLOW_TBL                  {532, 230, 597}
#define SRV_PCOUP_NUM_TX_LEVELS                  8
#define SRV_PCOUP_LINE_DRV_CONF                  5

#define SRV_PCOUP_PRED_HIGH_TBL                  {0x5620, 0x59C7, 0x5E1E, 0x6333, 0x698B, 0x6F03, 0x72CD, 0x760E, 0x7904, 0x7B57, 0x7D2C, 0x7E72, 0x7F0F, 0x7FC6, \
                                                  0x7FFF, 0x7ED1, 0x7D11, 0x7BCE, 0x7A1A, 0x777C, 0x7496, 0x720F, 0x6F8E, 0x6BE0, 0x6780, 0x6357, 0x5F5E, 0x5C0C, \
                                                  0x597B, 0x5782, 0x572D, 0x57A2, 0x5823, 0x59F2, 0x5D86, 0x6153}
#define SRV_PCOUP_PRED_VLOW_TBL                  {0x7FFF, 0x7934, 0x71F2, 0x6C8A, 0x694E, 0x6481, 0x5F97, 0x5A9E, 0x5487, 0x4F83, 0x4BF5, 0x495C, 0x4777, 0x46B8,  \
                                                  0x4634, 0x45B7, 0x45C1, 0x46FC, 0x4890, 0x4AC5, 0x4CEE, 0x4E87, 0x4F98, 0x50BC, 0x5111, 0x5244, 0x53A1, 0x55A0,  \
                                                  0x5661, 0x56EE, 0x5748, 0x5880, 0x5944, 0x5C03, 0x5F63, 0x630B}


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
