/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    pal_plc.h

  Summary:
    PLC Platform Abstraction Layer (PAL) Interface Local header file.

  Description:
    PLC Platform Abstraction Layer (PAL) Interface Local header file.
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

#ifndef PAL_PLC_LOCAL_H
#define PAL_PLC_LOCAL_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include "stack/g3/pal/plc/pal_plc.h"
#include "service/pcoup/srv_pcoup.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* PAL PLC Data

  Summary:
    Holds PAL PLC internal data.

  Description:
    This data type defines the all data required to handle the PAL PLC module.

  Remarks:
    None.
*/
typedef struct  
{
    DRV_HANDLE drvG3MacRtHandle;

    PAL_PLC_STATUS status;

    PAL_PLC_HANDLERS initHandlers;

    MAC_RT_BAND plcBand;

    SRV_PLC_PCOUP_BRANCH plcBranch;

    MAC_RT_PIB_OBJ plcPIB;

    MAC_RT_MIB_INIT_OBJ mibInitData;

    uint8_t statsErrorUnexpectedKey;

    uint8_t statsErrorReset;

    bool waitingTxCfm;

    bool restartMib;

    bool coordinator;

} PAL_PLC_DATA;

#endif // #ifndef PAL_PLC_LOCAL_H
/*******************************************************************************
 End of File
*/