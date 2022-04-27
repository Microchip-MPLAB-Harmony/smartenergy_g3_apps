/*******************************************************************************
  PLC PHY Coupling Service Implementation.

  Company:
    Microchip Technology Inc.

  File Name:
    srv_pcoup.c

  Summary:
    PLC PHY Coupling service implementation.

  Description:
    This file contains the source code for the implementation of the
    PLC PHY Coupling service. It helps to configure the PLC PHY Coupling 
    parameters through PLC Driver PIB interface.
*******************************************************************************/

//DOM-IGNORE-BEGIN
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
//DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <string.h>
#include "configuration.h"
#include "service/pcoup/srv_pcoup.h"

// *****************************************************************************
/* PLC PHY Coupling equalization data

  Summary:
    Holds the Tx equalization coefficients tables.

  Description:
    Pre-distorsion applies specific gain factor for each carrier, compensating 
    the frequency response of the external analog filter, and equalizing the 
    the transmitted signal.

  Remarks:
    Values are defined in srv_pcoup.h file. Different values for HIGH and VLOW 
    modes
 */

static const uint16_t srvPlcCoupPredistCoefHigh[SRV_PCOUP_EQU_NUM_COEF] = SRV_PCOUP_PRED_HIGH_TBL;
static const uint16_t srvPlcCoupPredistCoefVLow[SRV_PCOUP_EQU_NUM_COEF] = SRV_PCOUP_PRED_VLOW_TBL;

static const uint16_t srvPlcCoupAuxPredistCoefHigh[SRV_PCOUP_AUX_EQU_NUM_COEF] = SRV_PCOUP_AUX_PRED_HIGH_TBL;
static const uint16_t srvPlcCoupAuxPredistCoefVLow[SRV_PCOUP_AUX_EQU_NUM_COEF] = SRV_PCOUP_AUX_PRED_VLOW_TBL;

/* PLC PHY Coupling data

  Summary:
    PLC PHY Coupling data.

  Description:
    This structure(s) contains all the data required to set the PLC PHY 
    Coupling parameters, for each transmission branch.

  Remarks:
    Values are defined in srv_pcoup.h file
 */

static const SRV_PLC_PCOUP_DATA srvPlcCoup = {
  SRV_PCOUP_RMS_HIGH_TBL, SRV_PCOUP_RMS_VLOW_TBL,
  SRV_PCOUP_THRS_HIGH_TBL, SRV_PCOUP_THRS_VLOW_TBL,
  SRV_PCOUP_DACC_TBL,
  srvPlcCoupPredistCoefHigh, srvPlcCoupPredistCoefVLow,
  SRV_PCOUP_GAIN_HIGH_TBL, SRV_PCOUP_GAIN_VLOW_TBL,
  SRV_PCOUP_NUM_TX_LEVELS, SRV_PCOUP_EQU_NUM_COEF << 1,
  SRV_PCOUP_LINE_DRV_CONF
};

static const SRV_PLC_PCOUP_DATA srvPlcCoupAux = {
  SRV_PCOUP_AUX_RMS_HIGH_TBL, SRV_PCOUP_AUX_RMS_VLOW_TBL,
  SRV_PCOUP_AUX_THRS_HIGH_TBL, SRV_PCOUP_AUX_THRS_VLOW_TBL,
  SRV_PCOUP_AUX_DACC_TBL,
  srvPlcCoupAuxPredistCoefHigh, srvPlcCoupAuxPredistCoefVLow,
  SRV_PCOUP_AUX_GAIN_HIGH_TBL, SRV_PCOUP_AUX_GAIN_VLOW_TBL,
  SRV_PCOUP_AUX_NUM_TX_LEVELS, SRV_PCOUP_AUX_EQU_NUM_COEF << 1,
  SRV_PCOUP_AUX_LINE_DRV_CONF
};

// *****************************************************************************
// *****************************************************************************
// Section: File scope functions
// *****************************************************************************
// *****************************************************************************
SRV_PLC_PCOUP_DATA * SRV_PCOUP_Get_Config(SRV_PLC_PCOUP_BRANCH branch)
{
  if (branch == SRV_PLC_PCOUP_MAIN_BRANCH) 
  {
    /* PLC PHY Coupling parameters for Main transmission branch */
    return (SRV_PLC_PCOUP_DATA *)&srvPlcCoup;
  }
  else if (branch == SRV_PLC_PCOUP_AUXILIARY_BRANCH)
  {
    /* PLC PHY Coupling parameters for Auxiliary transmission branch */
    return (SRV_PLC_PCOUP_DATA *)&srvPlcCoupAux;
  }

  /* Transmission branch not recognized */
  return NULL;
}

bool SRV_PCOUP_Set_Config(DRV_HANDLE handle, SRV_PLC_PCOUP_BRANCH branch)
{
  SRV_PLC_PCOUP_DATA *pCoupValues;
  MAC_RT_PIB_OBJ pibObj;
  MAC_RT_STATUS result;

  /* Get PLC PHY Coupling parameters for the desired transmission branch */
  pCoupValues = SRV_PCOUP_Get_Config(branch);

  if (pCoupValues == NULL)
  {
    /* Transmission branch not recognized */
    return false;
  }

  /* Set PLC PHY Coupling parameters */
  pibObj.pib = MAC_RT_PIB_MANUF_PHY_PARAM;
  pibObj.index = PHY_PIB_PLC_IC_DRIVER_CFG;
  pibObj.length = 1;
  pibObj.pData[0] = pCoupValues->lineDrvConf;
  result = DRV_G3_MACRT_PIBSet(handle, &pibObj);

  pibObj.index = PHY_PIB_NUM_TX_LEVELS;
  pibObj.pData[0] = pCoupValues->numTxLevels;
  result |= DRV_G3_MACRT_PIBSet(handle, &pibObj);

  pibObj.index = PHY_PIB_DACC_TABLE_CFG;
  pibObj.length = sizeof(pCoupValues->daccTable);
  memcpy(pibObj.pData, pCoupValues->daccTable, pibObj.length);
  result |= DRV_G3_MACRT_PIBSet(handle, &pibObj);  

  pibObj.index = PHY_PIB_MAX_RMS_TABLE_HI;
  pibObj.length = sizeof(pCoupValues->rmsHigh);
  memcpy(pibObj.pData, pCoupValues->rmsHigh, pibObj.length);
  result |= DRV_G3_MACRT_PIBSet(handle, &pibObj);

  pibObj.index = PHY_PIB_MAX_RMS_TABLE_VLO;
  memcpy(pibObj.pData, pCoupValues->rmsVLow, pibObj.length);
  result |= DRV_G3_MACRT_PIBSet(handle, &pibObj);

  pibObj.index = PHY_PIB_THRESHOLDS_TABLE_HI;
  pibObj.length = sizeof(pCoupValues->thrsHigh);
  memcpy(pibObj.pData, pCoupValues->thrsHigh, pibObj.length);
  result |= DRV_G3_MACRT_PIBSet(handle, &pibObj);

  pibObj.index = PHY_PIB_THRESHOLDS_TABLE_VLO;
  memcpy(pibObj.pData, pCoupValues->thrsVLow, pibObj.length);
  result |= DRV_G3_MACRT_PIBSet(handle, &pibObj);

  pibObj.index = PHY_PIB_GAIN_TABLE_HI;
  pibObj.length = sizeof(pCoupValues->gainHigh);
  memcpy(pibObj.pData, pCoupValues->gainHigh, pibObj.length);
  result |= DRV_G3_MACRT_PIBSet(handle, &pibObj);

  pibObj.index = PHY_PIB_GAIN_TABLE_VLO;
  memcpy(pibObj.pData, pCoupValues->gainVLow, pibObj.length);
  result |= DRV_G3_MACRT_PIBSet(handle, &pibObj);

  pibObj.index = PHY_PIB_PREDIST_COEF_TABLE_HI;
  pibObj.length = pCoupValues->equSize;
  memcpy(pibObj.pData, pCoupValues->equHigh, pibObj.length);
  result |= DRV_G3_MACRT_PIBSet(handle, &pibObj);

  pibObj.index = PHY_PIB_PREDIST_COEF_TABLE_VLO;
  memcpy(pibObj.pData, pCoupValues->equVlow, pibObj.length);
  result |= DRV_G3_MACRT_PIBSet(handle, &pibObj);

  return (bool)(result == MAC_RT_STATUS_SUCCESS);
}

SRV_PLC_PCOUP_BRANCH SRV_PCOUP_Get_Default_Branch( void )
{
  return SRV_PCOUP_DEFAULT_BRANCH;
}

uint8_t SRV_PCOUP_Get_Phy_Band(SRV_PLC_PCOUP_BRANCH branch)
{
  if (branch == SRV_PLC_PCOUP_MAIN_BRANCH) 
  {
    /* PHY band for Main transmission branch */
    return G3_FCC;
  }
  else if (branch == SRV_PLC_PCOUP_AUXILIARY_BRANCH)
  {
    /* PHY band for Main Auxiliary branch */
    return G3_CEN_A;
  }

  /* Transmission branch not recognized */
  return G3_INVALID;
}
