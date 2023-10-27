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
* Copyright (C) 2023 Microchip Technology Inc. and its subsidiaries.
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


// *****************************************************************************
// *****************************************************************************
// Section: PLC PHY Coupling Service Interface Implementation
// *****************************************************************************
// *****************************************************************************

SRV_PLC_PCOUP_DATA * SRV_PCOUP_Get_Config(SRV_PLC_PCOUP_BRANCH branch)
{
    /* MISRA C-2012 deviation block start */
    /* MISRA C-2012 Rule 11.8 deviated once. Deviation record ID - H3_MISRAC_2012_R_11_8_DR_1 */

    if (branch == SRV_PLC_PCOUP_MAIN_BRANCH) 
    {
        /* PLC PHY Coupling parameters for Main transmission branch */
        return (SRV_PLC_PCOUP_DATA *)&srvPlcCoup;
    }

    /* MISRA C-2012 deviation block end */

    /* Transmission branch not recognized */
    return NULL;
}

bool SRV_PCOUP_Set_Config(DRV_HANDLE handle, SRV_PLC_PCOUP_BRANCH branch)
{
    SRV_PLC_PCOUP_DATA *pCoupValues;
    bool result, resultOut;
    MAC_RT_PIB_OBJ pibObj;

    /* Get PLC PHY Coupling parameters for the desired transmission branch */
    pCoupValues = SRV_PCOUP_Get_Config(branch);

    if (pCoupValues == NULL)
    {
        /* Transmission branch not recognized */
        return false;
    }

    /* Set PLC PHY Coupling parameters */
    pibObj.pib = MAC_RT_PIB_MANUF_PHY_PARAM;
    pibObj.index = (uint16_t)PHY_PIB_PLC_IC_DRIVER_CFG;
    pibObj.length = 1;
    pibObj.pData[0] = pCoupValues->lineDrvConf;
    result = (DRV_G3_MACRT_PIBSet(handle, &pibObj) == MAC_RT_STATUS_SUCCESS);

    pibObj.index = (uint16_t)PHY_PIB_NUM_TX_LEVELS;
    pibObj.pData[0] = pCoupValues->numTxLevels;
    resultOut = (DRV_G3_MACRT_PIBSet(handle, &pibObj) == MAC_RT_STATUS_SUCCESS);
    result = result && resultOut;

    pibObj.index = (uint16_t)PHY_PIB_DACC_TABLE_CFG;
    pibObj.length = (uint8_t)sizeof(pCoupValues->daccTable);
    (void) memcpy(pibObj.pData, (uint8_t *)pCoupValues->daccTable, pibObj.length);
    resultOut = (DRV_G3_MACRT_PIBSet(handle, &pibObj) == MAC_RT_STATUS_SUCCESS);
    result = result && resultOut;

    pibObj.index = (uint16_t)PHY_PIB_MAX_RMS_TABLE_HI;
    pibObj.length = (uint8_t)sizeof(pCoupValues->rmsHigh);
    (void) memcpy(pibObj.pData, (uint8_t *)pCoupValues->rmsHigh, pibObj.length);
    resultOut = (DRV_G3_MACRT_PIBSet(handle, &pibObj) == MAC_RT_STATUS_SUCCESS);
    result = result && resultOut;

    pibObj.index = (uint16_t)PHY_PIB_MAX_RMS_TABLE_VLO;
    (void) memcpy(pibObj.pData, (uint8_t *)pCoupValues->rmsVLow, pibObj.length);
    resultOut = (DRV_G3_MACRT_PIBSet(handle, &pibObj) == MAC_RT_STATUS_SUCCESS);
    result = result && resultOut;

    pibObj.index = (uint16_t)PHY_PIB_THRESHOLDS_TABLE_HI;
    pibObj.length = (uint8_t)sizeof(pCoupValues->thrsHigh);
    (void) memcpy(pibObj.pData, (uint8_t *)pCoupValues->thrsHigh, pibObj.length);
    resultOut = (DRV_G3_MACRT_PIBSet(handle, &pibObj) == MAC_RT_STATUS_SUCCESS);
    result = result && resultOut;

    pibObj.index = (uint16_t)PHY_PIB_THRESHOLDS_TABLE_VLO;
    (void) memcpy(pibObj.pData, (uint8_t *)pCoupValues->thrsVLow, pibObj.length);
    resultOut = (DRV_G3_MACRT_PIBSet(handle, &pibObj) == MAC_RT_STATUS_SUCCESS);
    result = result && resultOut;

    pibObj.index = (uint16_t)PHY_PIB_GAIN_TABLE_HI;
    pibObj.length = (uint8_t)sizeof(pCoupValues->gainHigh);
    (void) memcpy(pibObj.pData, (uint8_t *)pCoupValues->gainHigh, pibObj.length);
    resultOut = (DRV_G3_MACRT_PIBSet(handle, &pibObj) == MAC_RT_STATUS_SUCCESS);
    result = result && resultOut;

    pibObj.index = (uint16_t)PHY_PIB_GAIN_TABLE_VLO;
    (void) memcpy(pibObj.pData, (uint8_t *)pCoupValues->gainVLow, pibObj.length);
    resultOut = (DRV_G3_MACRT_PIBSet(handle, &pibObj) == MAC_RT_STATUS_SUCCESS);
    result = result && resultOut;

    /* MISRA C-2012 deviation block start */
    /* MISRA C-2012 Rule 11.8 deviated twice. Deviation record ID - H3_MISRAC_2012_R_11_8_DR_1 */

    pibObj.index = (uint16_t)PHY_PIB_PREDIST_COEF_TABLE_HI;
    pibObj.length = pCoupValues->equSize;
    (void) memcpy(pibObj.pData, (uint8_t *)pCoupValues->equHigh, pibObj.length);
    resultOut = (DRV_G3_MACRT_PIBSet(handle, &pibObj) == MAC_RT_STATUS_SUCCESS);
    result = result && resultOut;

    pibObj.index = (uint16_t)PHY_PIB_PREDIST_COEF_TABLE_VLO;
    (void) memcpy(pibObj.pData, (uint8_t *)pCoupValues->equVlow, pibObj.length);
    resultOut = (DRV_G3_MACRT_PIBSet(handle, &pibObj) == MAC_RT_STATUS_SUCCESS);
    result = result && resultOut;

    /* MISRA C-2012 deviation block end */

    return result;
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
       return (uint8_t)G3_FCC;
   }

    /* Transmission branch not recognized */
    return (uint8_t)G3_INVALID;
}
