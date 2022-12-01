/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    pal_rf.h

  Summary:
    Platform Abstraction Layer RF (PAL RF) Interface header file.

  Description:
    Platform Abstraction Layer RF (PAL RF) Interface header.
    The PAL RF module provides a simple interface to manage the RF PHY driver.
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

#ifndef _PAL_RF_H
#define _PAL_RF_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include "system/system.h"
#include "driver/rf215/drv_rf215_definitions.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* PAL RF Result

  Summary:
    Result of a PAL RF service client interface operation.

  Description:
    Identifies the result of certain PAL RF service operations.
*/
typedef enum {
    
    PAL_RF_PIB_SUCCESS        = RF215_PIB_RESULT_SUCCESS,
    PAL_RF_PIB_INVALID_PARAM  = RF215_PIB_RESULT_INVALID_PARAM,
    PAL_RF_PIB_INVALID_ATTR   = RF215_PIB_RESULT_INVALID_ATTR,
    PAL_RF_PIB_INVALID_HANDLE = RF215_PIB_RESULT_INVALID_HANDLE,
    PAL_RF_PIB_READ_ONLY      = RF215_PIB_RESULT_READ_ONLY,
    PAL_RF_PIB_WRITE_ONLY     = RF215_PIB_RESULT_WRITE_ONLY,
    PAL_RF_PIB_ERROR
            
} PAL_RF_PIB_RESULT;

// *****************************************************************************
/* PAL RF Handle

  Summary:
    Handle to a PAL RF instance.

  Description:
    This data type is a handle to a PAL RF instance.  It can be
    used to access and control RF PHY driver.

  Remarks:
    Do not rely on the underlying type as it may change in different versions
    or implementations of the PAL RF service.
*/

typedef uintptr_t PAL_RF_HANDLE;

// *****************************************************************************
/* Invalid PAL RF handle value to a PAL RF instance.

  Summary:
    Invalid handle value to a PAL RF instance.

  Description:
    Defines the invalid handle value to a PAL RF instance.

  Remarks:
    Do not rely on the actual value as it may change in different versions
    or implementations of the PAL RF service.
*/

#define PAL_RF_HANDLE_INVALID   ((PAL_RF_HANDLE) (-1))

typedef enum {
	PAL_RF_PHY_SUCCESS,
	PAL_RF_PHY_CHANNEL_ACCESS_FAILURE,
	PAL_RF_PHY_BUSY_TX,
	PAL_RF_PHY_TIMEOUT,
	PAL_RF_PHY_INVALID_PARAM,
	PAL_RF_PHY_TX_CANCELLED,
	PAL_RF_PHY_TX_ERROR,
	PAL_RF_PHY_TRX_OFF,
	PAL_RF_PHY_ERROR,
} PAL_RF_PHY_STATUS;

typedef struct {
    uint32_t time;
    uint32_t txPowerAttenuation;
    bool csmaEnable;
} PAL_RF_TX_PARAMETERS;

typedef struct {
    uint32_t timeIni;
    uint32_t timeEnd;
    int8_t rssi;
    bool fcsOk;
} PAL_RF_RX_PARAMETERS;

#define PAL_RF_MAX_PIB_SIZE     (sizeof(DRV_RF215_PHY_CFG_OBJ))

typedef struct {
    uint8_t pData[PAL_RF_MAX_PIB_SIZE];
    DRV_RF215_PIB_ATTRIBUTE pib;
} PAL_RF_PIB_OBJ;

typedef void (*PAL_RF_DataIndication)(uint8_t *pData, uint16_t length, PAL_RF_RX_PARAMETERS *pParameters);
typedef void (*PAL_RF_TxConfirm)(PAL_RF_PHY_STATUS status, uint32_t timeIni, uint32_t timeEnd);

typedef struct
{
    PAL_RF_DataIndication           palRfDataIndication;
    PAL_RF_TxConfirm                palRfTxConfirm;
} PAL_RF_HANDLERS;

typedef struct
{
    PAL_RF_HANDLERS                 rfPhyHandlers;
} PAL_RF_INIT;

typedef struct  
{
    SYS_MODULE_OBJ     (*PAL_RF_Initialize)(const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init);
                     
    SYS_STATUS         (*PAL_RF_Status)(SYS_MODULE_OBJ object);
                     
    DRV_HANDLE         (*PAL_RF_Open)(const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT intent);
                     
    void               (*PAL_RF_Close)(DRV_HANDLE handle);
                     
    void               (*PAL_RF_TxRequest)(DRV_HANDLE handle, uint8_t *pData, uint16_t length, PAL_RF_TX_PARAMETERS *txParameters);
                     
    void               (*PAL_RF_Reset)(DRV_HANDLE handle);
                     
    uint64_t           (*PAL_RF_GetPhyTime)(DRV_HANDLE handle);

    PAL_RF_PIB_RESULT  (*PAL_RF_GetRfPhyPib)(PAL_RF_HANDLE handle, PAL_RF_PIB_OBJ *pibObj);

    PAL_RF_PIB_RESULT  (*PAL_RF_SetRfPhyPib)(PAL_RF_HANDLE handle, PAL_RF_PIB_OBJ *pibObj);

    uint8_t            (*PAL_RF_GetRfPhyPibLength)(PAL_RF_HANDLE handle, DRV_RF215_PIB_ATTRIBUTE pib);
                                
} PAL_RF_OBJECT_BASE;

typedef struct  
{
    DRV_HANDLE drvRfPhyHandle;
    
    SYS_STATUS status;
    
    PAL_RF_HANDLERS rfPhyHandlers;
    
    DRV_RF215_PHY_MOD_SCHEME rfPhyModScheme;
    
} PAL_RF_DATA;

// *****************************************************************************
/* The supported basic PAL RF module (PAL_RF_OBJECT_BASE).
   This object is implemented by default as part of PAL RF module.
   It can be overwritten dynamically when needed.

*/
extern const PAL_RF_OBJECT_BASE  PAL_RF_OBJECT_BASE_Default;


// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines
// *****************************************************************************
// *****************************************************************************

SYS_MODULE_OBJ PAL_RF_Initialize(const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init);

SYS_STATUS PAL_RF_Status(SYS_MODULE_OBJ object);
 
PAL_RF_HANDLE PAL_RF_Open(const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT intent);
 
void PAL_RF_Close(PAL_RF_HANDLE handle);
 
void PAL_RF_TxRequest(PAL_RF_HANDLE handle, uint8_t *pData, 
        uint16_t length, PAL_RF_TX_PARAMETERS *txParameters);
 
void PAL_RF_Reset(PAL_RF_HANDLE handle);
 
uint64_t PAL_RF_GetPhyTime(PAL_RF_HANDLE handle);

PAL_RF_PIB_RESULT PAL_RF_GetRfPhyPib(PAL_RF_HANDLE handle, PAL_RF_PIB_OBJ *pibObj);

PAL_RF_PIB_RESULT PAL_RF_SetRfPhyPib(PAL_RF_HANDLE handle, PAL_RF_PIB_OBJ *pibObj);

uint8_t PAL_RF_GetRfPhyPibLength(PAL_RF_HANDLE handle, DRV_RF215_PIB_ATTRIBUTE attribute);
 
#endif // #ifndef _PAL_RF_H
/*******************************************************************************
 End of File
*/