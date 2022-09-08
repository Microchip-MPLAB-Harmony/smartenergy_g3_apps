/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    pal_plc.h

  Summary:
    Platform Abstraction Layer PLC (PAL PLC) Interface header file.

  Description:
    Platform Abstraction Layer PLC (PAL PLC) Interface header.
    The PAL PLC module provides a simple interface to manage the PLC PHY driver.
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

#ifndef _PAL_PLC_H
#define _PAL_PLC_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include "system/system.h"
#include "service/usi/srv_usi.h"
#include "driver/plc/g3MacRt/drv_g3_macrt_comm.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* PAL PLC Result

  Summary:
    Result of a PAL PLC service client interface operation.

  Description:
    Identifies the result of certain PAL PLC service operations.
*/
typedef enum {
    
    PAL_PLC_SUCCESS = MAC_RT_STATUS_SUCCESS,
    PAL_PLC_CHANNEL_ACCESS_FAILURE = MAC_RT_STATUS_CHANNEL_ACCESS_FAILURE,
    PAL_PLC_DENIED = MAC_RT_STATUS_DENIED, 
    PAL_PLC_INVALID_INDEX = MAC_RT_STATUS_INVALID_INDEX, 
    PAL_PLC_INVALID_PARAMETER = MAC_RT_STATUS_INVALID_PARAMETER, 
    PAL_PLC_NO_ACK = MAC_RT_STATUS_NO_ACK, 
    PAL_PLC_READ_ONLY = MAC_RT_STATUS_READ_ONLY, 
    PAL_PLC_TRANSACTION_OVERFLOW = MAC_RT_STATUS_TRANSACTION_OVERFLOW, 
    PAL_PLC_UNSUPPORTED_ATTRIBUTE = MAC_RT_STATUS_UNSUPPORTED_ATTRIBUTE,
    PAL_PLC_ERROR
            
} PAL_PLC_RESULT;

// *****************************************************************************
/* PAL PLC Handle

  Summary:
    Handle to a PAL PLC instance.

  Description:
    This data type is a handle to a PAL PLC instance.  It can be
    used to access and control PLC PHY driver.

  Remarks:
    Do not rely on the underlying type as it may change in different versions
    or implementations of the PAL PLC service.
*/

typedef uintptr_t PAL_PLC_HANDLE;

// *****************************************************************************
/* Invalid PAL PLC handle value to a PAL PLC instance.

  Summary:
    Invalid handle value to a PAL PLC instance.

  Description:
    Defines the invalid handle value to a PAL PLC instance.

  Remarks:
    Do not rely on the actual value as it may change in different versions
    or implementations of the PAL PLC service.
*/

#define PAL_PLC_HANDLE_INVALID   ((PAL_PLC_HANDLE) (-1))

typedef enum {
    PAL_PLC_STATE_IDLE,
    PAL_PLC_STATE_INIT,
    PAL_PLC_STATE_OPENING,
    PAL_PLC_STATE_READY,
    PAL_PLC_STATE_ERROR
} PAL_PLC_STATE;

typedef void (*PAL_PLC_DataIndication)(uint8_t *pData, uint16_t length);
typedef void (*PAL_PLC_CommStatusIndication)(uint8_t *pData);
typedef void (*PAL_PLC_TxConfirm)(MAC_RT_STATUS status, bool updateTimestamp);
typedef void (*PAL_PLC_RxParamsIndication)(MAC_RT_RX_PARAMETERS_OBJ *pParameters);
typedef void (*PAL_PLC_MacSnifferIndication)(uint8_t *pData, uint16_t length);

typedef struct
{
    PAL_PLC_DataIndication           palPlcDataIndication;
    PAL_PLC_CommStatusIndication     palPlcCommStatusIndication;
    PAL_PLC_TxConfirm                palPlcTxConfirm;
    PAL_PLC_RxParamsIndication       palPlcRxParamsIndication;
    PAL_PLC_MacSnifferIndication     palPlcMacSnifferIndication;
} PAL_PLC_HANDLERS;

typedef struct
{
    MAC_RT_BAND                      macRtBand;
    PAL_PLC_HANDLERS                 macRtHandlers;
} PAL_PLC_INIT;

typedef struct  
{
    SYS_MODULE_OBJ   (*PAL_PLC_Initialize)(const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init);
                     
    SYS_STATUS       (*PAL_PLC_Status)(SYS_MODULE_OBJ object);
                     
    DRV_HANDLE       (*PAL_PLC_Open)(const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT intent);
                     
    void             (*PAL_PLC_Close)(DRV_HANDLE handle);
                     
    void             (*PAL_PLC_TxRequest)(DRV_HANDLE handle, uint8_t *pData, uint16_t length);
                     
    void             (*PAL_PLC_Reset)(DRV_HANDLE handle, bool resetMib);
                     
    uint32_t         (*PAL_PLC_GetPhyTime)(DRV_HANDLE handle);

    PAL_PLC_RESULT   (*PAL_PLC_GetMacRtPib)(PAL_PLC_HANDLE handle, MAC_RT_PIB_OBJ *pibObj);

    PAL_PLC_RESULT   (*PAL_PLC_SetMacRtPib)(PAL_PLC_HANDLE handle, MAC_RT_PIB_OBJ *pibObj);
                                
} PAL_PLC_OBJECT_BASE;

typedef struct
{
    MAC_RT_PHY_SNIFFER_HEADER header;
    uint8_t data[MAC_RT_PHY_DATA_MAX_SIZE];
} PAL_PLC_PHY_SNIFFER;

typedef struct  
{
    DRV_HANDLE drvG3MacRtHandle;
    
    SYS_STATUS status;
    
    PAL_PLC_STATE state;
    
    PAL_PLC_HANDLERS initHandlers;
    
    MAC_RT_BAND plcBand;
    
    SRV_PLC_PCOUP_BRANCH plcBranch;
    
    MAC_RT_PIB_OBJ plcPIB;
    
    MAC_RT_MIB_INIT_OBJ mibInitData;
    
    uint8_t statsErrorUnexpectedKey;
    
    uint8_t statsErrorReset;
    
    bool waitingTxCfm;
    
    bool restartMib;

    bool pvddMonTxEnable;

    uint8_t macSnifferData[MAC_RT_DATA_MAX_SIZE];

    PAL_PLC_PHY_SNIFFER phySnifferData;
    
    SRV_USI_HANDLE usiHandler;

} PAL_PLC_DATA;

// *****************************************************************************
/* The supported basic PAL PLC module (PAL_PLC_OBJECT_BASE).
   This object is implemented by default as part of PAL PLC module.
   It can be overwritten dynamically when needed.

*/
extern const PAL_PLC_OBJECT_BASE  PAL_PLC_OBJECT_BASE_Default;


// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines
// *****************************************************************************
// *****************************************************************************

SYS_MODULE_OBJ PAL_PLC_Initialize(const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init);

SYS_STATUS PAL_PLC_Status(SYS_MODULE_OBJ object);
 
PAL_PLC_HANDLE PAL_PLC_Open(const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT intent);
 
void PAL_PLC_Close(PAL_PLC_HANDLE handle);
 
void PAL_PLC_TxRequest(PAL_PLC_HANDLE handle, uint8_t *pData, uint16_t length);
 
void PAL_PLC_Reset(PAL_PLC_HANDLE handle, bool resetMib);
 
uint32_t PAL_PLC_GetPhyTime(PAL_PLC_HANDLE handle);

PAL_PLC_RESULT PAL_PLC_GetMacRtPib(PAL_PLC_HANDLE handle, MAC_RT_PIB_OBJ *pibObj);

PAL_PLC_RESULT PAL_PLC_SetMacRtPib(PAL_PLC_HANDLE handle, MAC_RT_PIB_OBJ *pibObj);
 
#endif // #ifndef _PAL_PLC_H
/*******************************************************************************
 End of File
*/