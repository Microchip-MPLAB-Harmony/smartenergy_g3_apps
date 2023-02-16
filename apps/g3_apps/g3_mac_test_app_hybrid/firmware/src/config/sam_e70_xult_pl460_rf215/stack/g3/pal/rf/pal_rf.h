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
#include "driver/driver.h"
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
typedef enum 
{
    
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

// *****************************************************************************
/* PAL RF TX Handle

  Summary:
    Transmission handle to a PAL RF instance.

  Description:
    This data type is a handle to a PAL RF transmission.  It can be
    used to cancel of the transmission in progress.

  Remarks:
    Do not rely on the underlying type as it may change in different versions
    or implementations of the PAL RF service.
*/

typedef uintptr_t PAL_RF_TX_HANDLE;

// *****************************************************************************
/* Invalid PAL RF TX Handle value to a PAL RF instance.

  Summary:
    Invalid transmission handle value to a PAL RF instance.

  Description:
    Defines the invalid transmission handle value to a PAL RF instance.

  Remarks:
    Do not rely on the actual value as it may change in different versions
    or implementations of the PAL RF service.
*/

#define PAL_RF_TX_HANDLE_INVALID   ((PAL_RF_HANDLE) (-1))

// *****************************************************************************
/* PAL RF Module Status

  Summary:
    Identifies the current status/state of the PLC RF module.

  Description:
    This enumeration identifies the current status/state of the PLC RF module.

  Remarks:
    This enumeration is the return type for the PAL RF status routine.
*/

typedef enum 
{
    PAL_RF_STATUS_UNINITIALIZED = SYS_STATUS_UNINITIALIZED,
    PAL_RF_STATUS_BUSY = SYS_STATUS_BUSY,
    PAL_RF_STATUS_READY = SYS_STATUS_READY,
    PAL_RF_STATUS_ERROR = SYS_STATUS_ERROR,
    PAL_RF_STATUS_DEINITIALIZED = SYS_STATUS_READY_EXTENDED + 1,
} PAL_RF_STATUS;

// *****************************************************************************
/* PAL RF PHY TX Results

  Summary:
    Defines the possible PAL RF PHY transmission results.

  Description:
    This data type defines the list of possible PAL RF PHY transmission results.

  Remarks:
    None.
*/

typedef enum 
{
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

// *****************************************************************************
/* PAR RF TX Request Parameters

  Summary:
    Defines the parameters used in PAL RF transmissions.

  Description:
    This data type defines the parameters needed to request a transmission,
    using the PAL_RF_TxRequest function.

  Remarks:
    None.
*/

typedef struct 
{
    /* TX time (PPDU start), referred to system 64-bit time counter */
    uint64_t timeCount;

    /* Transmitter power attenuation in dB (max 31 dB) */
    uint32_t txPowerAttenuation;

    /* CSMA enable flag */
    bool csmaEnable;

} PAL_RF_TX_PARAMETERS;

// *****************************************************************************
/* PAR RF RX Indication Data

  Summary:
    Defines the data reported in PAR RF RX indication.

  Description:
    This data type defines the data reported in the PAR RF receive indication via 
    PAL_RF_DataIndication callback.

  Remarks:
    None.
*/

typedef struct 
{
    /* RX time (PPDU start), referred to system 64-bit time counter */
    uint64_t timeIniCount;
    
    /* RX time (PPDU end), referred to system 64-bit time counter */
    uint64_t timeEndCount;

    /* RSSI in dBm */
    int8_t rssi;

    /* Correct FCS flag */
    bool fcsOk;

} PAL_RF_RX_PARAMETERS;

// *****************************************************************************
/* PAR RF PIB data

  Summary:
    Defines the data used when requesting a PIB attribute.

  Description:
    This data type defines the data used in a PIB request.

  Remarks:
    PAL_RF_MAX_PIB_SIZE is the size of the largest PIB attibute (DRV_RF215_PHY_CFG_OBJ).
*/

#define PAL_RF_MAX_PIB_SIZE     (sizeof(DRV_RF215_PHY_CFG_OBJ))

typedef struct 
{
    /* Data buffer wher data is read/stored */
    uint8_t pData[PAL_RF_MAX_PIB_SIZE];

    /* RF215 Driver PIB Attribute to request */
    DRV_RF215_PIB_ATTRIBUTE pib;

} PAL_RF_PIB_OBJ;

// *****************************************************************************
/* PAL RF RX Indication Callback

  Summary:
    Pointer to a PAL RF module receive indication callback function.

  Description:
    This data type defines the required function signature for the PAL RF driver
    receive event handling callback function. A client uses
    PAL_RF_Initialize function to register a pointer to a function which
    must match the signature (parameter and return value types) specified by
    this data type.

  Parameters:
    pData       - Pointer to the buffer containing the data associated to the
                  receive indication.
    lengh       - PSDU length in bytes (including FCS)
    pParameters - Pointer to the parameters associated to the receive indication

  Returns:
    None.

  Example:
    <code>
    static void _rfDataIndication(uint8_t *pData, uint16_t length, PAL_RF_RX_PARAMETERS *pParameters)
    {
        // Reception handling here.
    }

    PAL_RF_INIT palRfInitData;

    palRfInitData.rfPhyHandlers.palRfDataIndication = _rfDataIndication;
    palRfInitData.rfPhyHandlers.palRfTxConfirm = _rfTxConfirm;
    // Initialize the PAL RF module
    appRFData.palRfObj = PAL_RF_Initialize(PAL_RF_PHY_INDEX, (const SYS_MODULE_INIT *) &palRfInitData);
    </code>

  Remarks:
    None.
*/

typedef void (*PAL_RF_DataIndication)(uint8_t *pData, uint16_t length, PAL_RF_RX_PARAMETERS *pParameters);

// *****************************************************************************
/* PAL RF TX Confirm Callback

  Summary:
    Pointer to a PAL RF module transmit confirm callback function.

  Description:
    This data type defines the required function signature for the PAL RF driver
    transmit confirm event handling callback function. A client uses
    PAL_RF_Initialize function to register a pointer to a function which
    must match the signature (parameter and return value types) specified by
    this data type.

  Parameters:
    status      - Transmission result
    timeIni     - TX time (PPDU start), referred to system 64-bit time counter
    timeEnd     - TX time (PPDU end), referred to system 64-bit time counter

  Returns:
    None.

  Example:
    <code>
    static void _rfTxConfirm(PAL_RF_PHY_STATUS status, uint64_t timeIni, uint64_t timeEnd)
    {
        // Tx Confirm handling here.
    }

    PAL_RF_INIT palRfInitData;

    palRfInitData.rfPhyHandlers.palRfDataIndication = _rfDataIndication;
    palRfInitData.rfPhyHandlers.palRfTxConfirm = _rfTxConfirm;
    // Initialize the PAL RF module
    appRFData.palRfObj = PAL_RF_Initialize(PAL_RF_PHY_INDEX, (const SYS_MODULE_INIT *) &palRfInitData);
    </code>

  Remarks:
    None.
*/

typedef void (*PAL_RF_TxConfirm)(PAL_RF_PHY_STATUS status, uint64_t timeIni, uint64_t timeEnd);

// *****************************************************************************
/* PAL RF Handlers Data

  Summary:
    Defines the handlers required to manage the PAL RF module.

  Description:
    This data type defines the handlers required to manage the PAL RF module.

  Remarks:
    None.
*/

typedef struct
{
    PAL_RF_DataIndication           palRfDataIndication;
    PAL_RF_TxConfirm                palRfTxConfirm;   
} PAL_RF_HANDLERS;

// *****************************************************************************
/* PAL RF Initialization Data

  Summary:
    Defines the data required to initialize the PAL RF module.

  Description:
    This data type defines the data required to initialize the PAL RF module.

  Remarks:
    None.
*/

typedef struct
{
    PAL_RF_HANDLERS                 rfPhyHandlers;
} PAL_RF_INIT;

// *****************************************************************************
/* PAL RF Data

  Summary:
    Holds PAL RF internal data.

  Description:
    This data type defines the all data required to handle the PAL RF module.

  Remarks:
    None.
*/

typedef struct  
{
    DRV_HANDLE drvRfPhyHandle;
    
    PAL_RF_STATUS status;
    
    PAL_RF_HANDLERS rfPhyHandlers;
    
    DRV_RF215_PHY_MOD_SCHEME rfPhyModScheme;

} PAL_RF_DATA;

// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines
// *****************************************************************************
// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ PAL_RF_Initialize (
        const SYS_MODULE_INDEX index,
        const SYS_MODULE_INIT * const init
    )

  Summary:
    Initializes the PAL RF module.

  Description:
    This routine initializes the PAL RF module, making it ready for clients to
    open and use it. The initialization data is specified by the init parameter.
    It is a single instance module, so this function should be called only once.

  Precondition:
    None.

  Parameters:
    index - Identifier for the instance to be initialized. Only one instance
            (index 0) supported.
    init  - Pointer to the initialization data structure containing the data
            necessary to initialize the module.

  Returns:
    If successful, returns a valid handle to a module instance object.
    Otherwise, returns SYS_MODULE_OBJ_INVALID.

  Example:
    <code>
    PAL_RF_INIT palRfInitData;
    
    palRfInitData.rfPhyHandlers.palRfDataIndication = _rfDataIndication;
    palRfInitData.rfPhyHandlers.palRfTxConfirm = _rfTxConfirm;

    appRFData.palRfObj = PAL_RF_Initialize( PAL_PLC_PHY_INDEX, (const SYS_MODULE_INIT *) &palRfInitData );
    </code>

  Remarks:
    This routine must be called before any other PAL RF module routine and should
    only be called once during initialization of the application.
*/
SYS_MODULE_OBJ PAL_RF_Initialize(const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init);

// *****************************************************************************
/* Function:
    PAL_RF_STATUS PAL_RF_Status ( SYS_MODULE_OBJ object )

  Summary:
    Returns status of the specific instance of the PAL RF module.

  Description:
    This function returns the status of the specific module instance.

  Precondition:
    The PAL_RF_Initialize function should have been called before calling
    this function.

  Parameters:
    object    - PAL RF object handle, returned from PAL_RF_Initialize

  Returns:
    PAL_RF_STATUS_READY          - Indicates that the module is initialized and is
                                  ready to accept new requests from the clients.

    PAL_RF_STATUS_BUSY           - Indicates that the module is busy with a
                                  previous requests from the clients. However,
                                  depending on the configured queue size for
                                  transmit and receive, it may be able to queue
                                  a new request.

    PAL_RF_STATUS_ERROR          - Indicates that the module is in an error state.
                                  Any value less than SYS_STATUS_ERROR is
                                  also an error state.

    PAL_RF_STATUS_UNINITIALIZED  - Indicates that the module is not initialized.

    PAL_RF_STATUS_DEINITIALIZED  - Indicates that the module has been deinitialized. 
                                
  Example:
    <code>
    // Given "object" returned from PAL_RF_Initialize

    PAL_RF_STATUS          palRfStatus;

    palRfStatus = PAL_RF_Status (object);
    if (palRfStatus == PAL_RF_STATUS_READY)
    {
        // PAL RF is initialized and is ready to accept client requests.
    }
    </code>

  Remarks:
    Application must ensure that the PAL_RF_Status returns PAL_RF_STATUS_READY
    before performing PAL RF operations.
*/
PAL_RF_STATUS PAL_RF_Status(SYS_MODULE_OBJ object);

// *****************************************************************************
/* Function:
    PAL_RF_HANDLE PAL_RF_HandleGet(const SYS_MODULE_INDEX index)

  Summary:
    Returns a handle to the requested PAL RF instance

  Description:
    This function returns a handle to the requested PAL RF instance.

  Preconditions:
    The PAL_RF_Initialize function should have been called before calling
    this function.

  Parameters:
    index       - index of the PAL RF instance

  Returns:
    PAL_RF_HANDLE - Handle to the requested PAL RF instance

  Example:
    <code>
    PAL_RF_HANDLE myPalRfHandle;
    myPalRfHandle = PAL_RF_HandleGet(PAL_RF_INDEX_0);

    if (myPalRfHandle != PAL_RF_HANDLE_INVALID)
    {
        // Found a valid handle to the PAL RF instance

        // Send some data via PAL RF
        PAL_RF_TxRequest(myPalRfHandle, pData, size, &txParam);
    }
    </code>

  Remarks:
    None.
*/
 
PAL_RF_HANDLE PAL_RF_HandleGet(const SYS_MODULE_INDEX index);
                                                        
// *****************************************************************************
/* Function:
    void PAL_RF_Deinitialize(SYS_MODULE_OBJ object)

  Summary:
    De-initializes the specified instance of the PAL RF module.

  Description:
    This function deinitializes the specified instance of the PAL RF module,
    disabling its operation and invalidates all of the internal data.

  Precondition:
    Function PAL_RF_Initialize must have been called before calling this
    routine and a valid SYS_MODULE_OBJ must have been returned.

  Parameters:
    object          - PAL RF module object handle, returned by PAL_RF_Initialize

  Returns:
    None.

  Example:
    <code>
    // This code example shows how the PAL RF can be deinitialized. 
    // It is assumed the PAL RF module was already initialized.

    SYS_MODULE_OBJ palRfobj;

    PAL_RF_Deinitialize(palRfobj); 

    </code>

  Remarks:
    Once the Initialize operation has been called, the deinitialize operation
    must be called before the Initialize operation can be called again.
*/
 
void PAL_RF_Deinitialize(SYS_MODULE_OBJ object);

// *****************************************************************************
/* Function:
    PAL_RF_TX_HANDLE PAL_RF_TxRequest(PAL_RF_HANDLE handle, uint8_t *pData, 
        uint16_t length, PAL_RF_TX_PARAMETERS *txParameters)

  Summary:
    Allows a client to transmit data through RF device.

  Description:
    This routine sends a new data message through RF using the PAL RF module.

  Precondition:
    PAL_RF_HandleGet must have been called to obtain a valid PAL RF handle.

  Parameters:
    handle -       A valid handle, returned from the handle get routine.

    pData -        Pointer to the data to transmit.

    length -       Length of the data to transmit in bytes.

    txParameters - Pointer to parameters of the transmission.

  Returns:
    PAL_RF_TX_HANDLE - Handle of the current transmission.

  Example:
    <code>
    // 'palRfHandle', returned from the PAL_RF_HandleGet
    
    // Local function implemented in the user application
    _setupTransmissionParameters();

    PAL_RF_TxRequest(palRfHandle, appData.pData, appData.length, &appData.txParams);

    </code>

  Remarks:
    None.
*/
 
PAL_RF_TX_HANDLE PAL_RF_TxRequest(PAL_RF_HANDLE handle, uint8_t *pData, 
        uint16_t length, PAL_RF_TX_PARAMETERS *txParameters);

// *****************************************************************************
/* Function:
    void PAL_RF_TxCancel(PAL_RF_HANDLE handle, PAL_RF_TX_HANDLE txHandle)

  Summary:
    Allows a client to cancel a previously requested transmission.

  Description:
    This routine allows a client to cancel a previously requested transmission.

  Precondition:
    PAL_RF_TxRequest must have been called to obtain a valid TX handle.

  Parameters:
    handle - A valid PAL RF handle, returned from the handle get routine.
    txHandle  - A valid transmission handle returned from PAL_RF_TxRequest.

  Returns:
    None.

  Example:
    <code>
    PAL_RF_HANDLE palRfHandle; // returned from PAL_RF_HandleGet
    PAL_RF_TX_HANDLE txReqHandle; // returned from PAL_RF_TxRequest

    PAL_RF_TxCancel(palRfHandle, txReqHandle);
    </code>

  Remarks:
    If transmission has already started it will be aborted before completion.
*/

void PAL_RF_TxCancel(PAL_RF_HANDLE handle, PAL_RF_TX_HANDLE txHandle);

// *****************************************************************************
/* Function:
    void PAL_RF_Reset(PAL_RF_HANDLE handle)

  Summary:
    Allows a client to reset PAL RF module.

  Description:
    This routine performs a reset of the RF device and clear the RF PHY statistics.

  Precondition:
    PAL_RF_HandleGet must have been called to obtain a valid PAL RF handle.

  Parameters:
    handle -       A valid handle, returned from the handle get routine

  Returns:
    None.

  Example:
    <code>
    // 'palRfHandle', returned from the PAL_RF_HandleGet
    
    PAL_RF_Reset(palRfHandle);

    </code>

  Remarks:
    None.
*/
 
void PAL_RF_Reset(PAL_RF_HANDLE handle);

// *****************************************************************************
/* Function:
    PAL_RF_PIB_RESULT PAL_RF_GetRfPhyPib (PAL_RF_HANDLE handle, PAL_RF_PIB_OBJ *pibObj)

  Summary:
    Gets value of PIB attribute.

  Description:
    This routine allows a client to get the value of a PIB (PHY Information
    Base) attribute of the RF device.

  Precondition:
    PAL_RF_HandleGet must have been called to obtain a valid PAL RF handle.

  Parameters:
    handle  - A valid handle, returned from the handle get routine.
    pibObj  - Pointer to PIB object to indicate what PIB is read. PIB object includes a 
              data buffer to store the read value.

  Returns:
    Result of getting the PIB (see PAL_RF_PIB_RESULT).

  Example:
    <code>
    PAL_RF_HANDLE palRfHandle; // returned from PAL_RF_HandleGet

    appRFData.pibObj.pib = RF215_PIB_FW_VERSION;
            
    if (PAL_RF_GetRfPhyPib(palRfHandle, &appRFData.pibObj) == PAL_RF_PIB_SUCCESS)
    {
        uint8_t size = PAL_RF_GetRfPhyPibLength(palRfHandle, RF215_PIB_FW_VERSION);
        memcpy(&appRFData.fwVersion, appRFData.pibObj.pData, size);
        appRFData.state = APP_RF_STATE_GET_PHY_CONFIG;
    }
    </code>

  Remarks:
    If dual-band is used, the PIB values are different for each RF transceiver.
*/

PAL_RF_PIB_RESULT PAL_RF_GetRfPhyPib(PAL_RF_HANDLE handle, PAL_RF_PIB_OBJ *pibObj);

// *****************************************************************************
/* Function:
    PAL_RF_PIB_RESULT PAL_RF_SetRfPhyPib (PAL_RF_HANDLE handle, PAL_RF_PIB_OBJ *pibObj)

  Summary:
    Sets value of PIB attribute.

  Description:
    This routine allows a client to set the value of a PIB (PHY Information
    Base) attribute of the RF device.

  Precondition:
    PAL_RF_HandleGet must have been called to obtain a valid PAL RF handle.

  Parameters:
    handle  - A valid handle, returned from the handle get routine.
    pibObj  - Pointer to PIB object to indicate what PIB is write. PIB object includes a 
              data buffer to write the new value.

  Returns:
    Result of getting the PIB (see PAL_RF_PIB_RESULT).

  Example:
    <code>
    PAL_RF_HANDLE palRfHandle; // returned from PAL_RF_HandleGet

    appRFData.pibObj.pib = RF215_PIB_PHY_CHANNEL_NUM;
    appRFData.pibObj.pData[0] = 1;
            
    if (PAL_RF_SetRfPhyPib(palRfHandle, &appRFData.pibObj) == PAL_RF_PIB_SUCCESS)
    {
        // PIB set successful
    }
    </code>

  Remarks:
    If dual-band is used, the PIB values are different for each RF transceiver.
*/

PAL_RF_PIB_RESULT PAL_RF_SetRfPhyPib(PAL_RF_HANDLE handle, PAL_RF_PIB_OBJ *pibObj);

// *****************************************************************************
/* Function:
    uint8_t PAL_RF_GetRfPhyPibLength(PAL_RF_HANDLE handle, DRV_RF215_PIB_ATTRIBUTE attribute)

  Summary:
    Gets size of PIB attribute.

  Description:
    This routine allows to get the size in bytes of a PIB (PHY Information Base)
    attribute of the RF PHY device.

  Precondition:
    None.

  Parameters:
    attribute - A valid PIB attribute (see DRV_RF215_PIB_ATTRIBUTE).

  Returns:
    Size of PIB attribute in bytes. 0 if invalid PIB attribute.

  Example:
    <code>
    PAL_RF_HANDLE palRfHandle; // returned from PAL_RF_HandleGet
    uint8_t pibSize;

    pibSize = PAL_RF_GetRfPhyPibLength(palRfHandle, RF215_PIB_PHY_CONFIG);
    </code>

  Remarks:
    None.
*/

uint8_t PAL_RF_GetRfPhyPibLength(PAL_RF_HANDLE handle, DRV_RF215_PIB_ATTRIBUTE attribute);
 
#endif // #ifndef _PAL_RF_H
/*******************************************************************************
 End of File
*/