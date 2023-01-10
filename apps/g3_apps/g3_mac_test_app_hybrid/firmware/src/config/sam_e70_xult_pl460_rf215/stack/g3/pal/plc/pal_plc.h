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
#include "driver/driver.h"
#include "service/pcoup/srv_pcoup.h"
#include "driver/plc/g3MacRt/drv_g3_macrt_comm.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* PAL PLC PIB Result

  Summary:
    Result of a PAL PLC PIB service client interface operation.

  Description:
    Identifies the result of certain PAL PLC PIB service operations.
*/
typedef enum {
    
    PAL_PLC_PIB_SUCCESS = MAC_RT_STATUS_SUCCESS,
    PAL_PLC_PIB_CHANNEL_ACCESS_FAILURE = MAC_RT_STATUS_CHANNEL_ACCESS_FAILURE,
    PAL_PLC_PIB_DENIED = MAC_RT_STATUS_DENIED, 
    PAL_PLC_PIB_INVALID_INDEX = MAC_RT_STATUS_INVALID_INDEX, 
    PAL_PLC_PIB_INVALID_PARAMETER = MAC_RT_STATUS_INVALID_PARAMETER, 
    PAL_PLC_PIB_NO_ACK = MAC_RT_STATUS_NO_ACK, 
    PAL_PLC_PIB_READ_ONLY = MAC_RT_STATUS_READ_ONLY, 
    PAL_PLC_PIB_TRANSACTION_OVERFLOW = MAC_RT_STATUS_TRANSACTION_OVERFLOW, 
    PAL_PLC_PIB_UNSUPPORTED_ATTRIBUTE = MAC_RT_STATUS_UNSUPPORTED_ATTRIBUTE,
            
} PAL_PLC_PIB_RESULT;

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

// *****************************************************************************
/* PAL PLC Module Status

  Summary:
    Identifies the current status/state of the PLC PLC module.

  Description:
    This enumeration identifies the current status/state of the PLC PLC module.

  Remarks:
    This enumeration is the return type for the PAL PLC status routine.
*/

typedef enum {
    PAL_PLC_STATUS_UNINITIALIZED = SYS_STATUS_UNINITIALIZED,
    PAL_PLC_STATUS_BUSY = SYS_STATUS_BUSY,
    PAL_PLC_STATUS_READY = SYS_STATUS_READY,
    PAL_PLC_STATUS_ERROR = SYS_STATUS_ERROR,
    PAL_PLC_STATUS_INVALID_OBJECT = SYS_STATUS_ERROR_EXTENDED - 1,         
} PAL_PLC_STATUS;

// *****************************************************************************
/* PAL PLC Data Indication Event Handler Function Pointer

   Summary
    Pointer to a PAL PLC Data Indication Event handler function

   Description
    This data type defines the required function signature for the PAL PLC 
    reception event handling callback function. A client must register a pointer 
    using the callback register function whose function signature (parameter 
    and return value types) match the types specified by this function pointer 
    in order to receive transfer related event calls back from the driver.

    The parameters and return values are described here and a partial example
    implementation is provided.

  Parameters:
    pData -     Pointer to the data buffer containing received data.

    length -    Length of the received data.

  Returns:
    None.

  Example:
    <code>
    static void _plcDataIndication(uint8_t *pData, uint16_t length)
    {
        // Reception handling here.
    }

    PAL_PLC_INIT palPlcInitData;

    palPlcInitData.macRtHandlers.palPlcDataIndication = _plcDataIndication;
    palPlcInitData.macRtHandlers.palPlcTxConfirm = _plcTxConfirm;
    palPlcInitData.macRtHandlers.palPlcCommStatusIndication = _plcCommStatusIndication;
    palPlcInitData.macRtHandlers.palPlcRxParamsIndication = _plcRxParamsIndication;
    // Initialize the PAL PLC module
    appPLCData.palPlcObj = PAL_PLC_Initialize(PAL_PLC_PHY_INDEX, (const SYS_MODULE_INIT *) &palPlcInitData);
    </code>

  Remarks:
    None.

*/
typedef void (*PAL_PLC_DataIndication)(uint8_t *pData, uint16_t length);

// *****************************************************************************
/* PAL PLC Transmission Confirm Event Handler Function Pointer

   Summary
    Pointer to a PAL PLC Transmission Confirm Event handler function

   Description
    This data type defines the required function signature for the PAL PLC
    transmission confirm event handling callback function. A client must register 
    a pointer using the callback register function whose function signature 
    (parameter and return value types) match the types specified by this function 
    pointer in order to receive transfer related event calls back from the driver.

    The parameters and return values are described here and a partial example
    implementation is provided.

  Parameters:
    status -            MAC RT Status.

    updateTimestamp -   Flag to indicate Timestamp should be updated

  Returns:
    None.

  Example:
    <code>
    static void _plcTxConfirm(MAC_RT_STATUS status, bool updateTimestamp)
    {
        switch(status)
        {
            case MAC_RT_STATUS_SUCCESS:
                // Transmission result: already in process
                break;   
            case MAC_RT_STATUS_CHANNEL_ACCESS_FAILURE:
                // Transmission result: CSMA failure
                break;   
            case MAC_RT_STATUS_NO_ACK:
                // Transmission result: ACK failure
                break;
        }
    }

    PAL_PLC_INIT palPlcInitData;

    palPlcInitData.macRtHandlers.palPlcDataIndication = _plcDataIndication;
    palPlcInitData.macRtHandlers.palPlcTxConfirm = _plcTxConfirm;
    palPlcInitData.macRtHandlers.palPlcCommStatusIndication = _plcCommStatusIndication;
    palPlcInitData.macRtHandlers.palPlcRxParamsIndication = _plcRxParamsIndication;
    // Initialize the PAL PLC module
    appPLCData.palPlcObj = PAL_PLC_Initialize(PAL_PLC_PHY_INDEX, (const SYS_MODULE_INIT *) &palPlcInitData);
    </code>

  Remarks:
    - If the status field is MAC_RT_STATUS_SUCCESS, it means that the data was
      transferred successfully.

    - Otherwise, it means that the data was not transferred successfully.

*/
typedef void (*PAL_PLC_TxConfirm)(MAC_RT_STATUS status, bool updateTimestamp);

// *****************************************************************************
/* PAL PLC Event Handler Function Pointer to get parameters from the
   last received message

   Summary
    Pointer to a PAL PLC Event handler function to get parameters 
    from the last received message

   Description
    This data type defines the required function signature for the PAL PLC
    reception parameters event handling callback function.
    A client must register a pointer using the callback register function 
    whose function signature (parameter and return value types) match 
    the types specified by this function pointer in order to receive 
    transfer related event calls back from the driver.

    The parameters and return values are described here and a partial example
    implementation is provided.

  Parameters:
    pParameters -       Pointer to the characterization parameters of the last 
                        received message
 
  Returns:
    None.

  Example:
    <code>
    static void _plcRxParamsIndication(MAC_RT_RX_PARAMETERS_OBJ *pParameters)
    {
        // extract all parameters from received message
        memcpy(appData->rxParams, (uint8_t *)pParameters, sizeof(MAC_RT_RX_PARAMETERS_OBJ));
    }

    PAL_PLC_INIT palPlcInitData;

    palPlcInitData.macRtHandlers.palPlcDataIndication = _plcDataIndication;
    palPlcInitData.macRtHandlers.palPlcTxConfirm = _plcTxConfirm;
    palPlcInitData.macRtHandlers.palPlcCommStatusIndication = _plcCommStatusIndication;
    palPlcInitData.macRtHandlers.palPlcRxParamsIndication = _plcRxParamsIndication;
    // Initialize the PAL PLC module
    appPLCData.palPlcObj = PAL_PLC_Initialize(PAL_PLC_PHY_INDEX, (const SYS_MODULE_INIT *) &palPlcInitData);
    </code>

*/
typedef void (*PAL_PLC_RxParamsIndication)(MAC_RT_RX_PARAMETERS_OBJ *pParameters);

// *****************************************************************************
/* PAL PLC Communication Status Handler Function Pointer

   Summary
    Pointer to a PAL PLC Communication Status Event handler function

   Description
    This data type defines the required function signature for the PAL PLC
    comm status event handling callback function. A client must register a pointer 
    using the callback register function whose function signature (parameter 
    and return value types) match the types specified by this function pointer 
    in order to receive transfer related event calls back from the driver.

    The parameters and return values are described here and a partial example
    implementation is provided.

  Parameters:
    pData -             Pointer to the data content.

  Returns:
    None.

  Example:
    <code>
    static void _plcCommStatusIndication(uint8_t *pData)
    {
        // extract data from received message
        memcpy(appData->dataMacCommStatusBuffer, pData, MAC_RT_COMM_STATUS_SIZE));
    }

    PAL_PLC_INIT palPlcInitData;

    palPlcInitData.macRtHandlers.palPlcDataIndication = _plcDataIndication;
    palPlcInitData.macRtHandlers.palPlcTxConfirm = _plcTxConfirm;
    palPlcInitData.macRtHandlers.palPlcCommStatusIndication = _plcCommStatusIndication;
    palPlcInitData.macRtHandlers.palPlcRxParamsIndication = _plcRxParamsIndication;
    // Initialize the PAL PLC module
    appPLCData.palPlcObj = PAL_PLC_Initialize(PAL_PLC_PHY_INDEX, (const SYS_MODULE_INIT *) &palPlcInitData);
    </code>
*/
typedef void (*PAL_PLC_CommStatusIndication)(uint8_t *pData);

// *****************************************************************************
/* PAL PLC Handlers Data

  Summary:
    Defines the handlers required to manage the PAL PLC module.

  Description:
    This data type defines the handlers required to manage the PAL PLC module.

  Remarks:
    None.
*/
typedef struct
{
    PAL_PLC_DataIndication           palPlcDataIndication;
    PAL_PLC_CommStatusIndication     palPlcCommStatusIndication;
    PAL_PLC_TxConfirm                palPlcTxConfirm;
    PAL_PLC_RxParamsIndication       palPlcRxParamsIndication;
} PAL_PLC_HANDLERS;

// *****************************************************************************
/* PAL PLC Initialization Data

  Summary:
    Defines the data required to initialize the PAL PLC module.

  Description:
    This data type defines the data required to initialize the PAL PLC module.

  Remarks:
    None.
*/
typedef struct
{
    // G3 Phy band
    MAC_RT_BAND                      macRtBand;

    // PAL PLC Handlers
    PAL_PLC_HANDLERS                 macRtHandlers;

    // Flag to indicate if MIB data should be reinitialized with the default values
    bool                             initMIB;
} PAL_PLC_INIT;

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

    bool pvddMonTxEnable;

} PAL_PLC_DATA;

// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines
// *****************************************************************************
// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ PAL_PLC_Initialize (
        const SYS_MODULE_INDEX index,
        const SYS_MODULE_INIT * const init
    )

  Summary:
    Initializes the PAL PLC module.

  Description:
    This routine initializes the PAL PLC module, making it ready for clients to
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
    PAL_PLC_INIT palPlcInitData;

    palPlcInitData.macRtBand = G3_FCC;
    palPlcInitData.macRtHandlers.palPlcDataIndication = _plcDataIndication;
    palPlcInitData.macRtHandlers.palPlcTxConfirm = _plcTxConfirm;
    palPlcInitData.macRtHandlers.palPlcCommStatusIndication = _plcCommStatusIndication;
    palPlcInitData.macRtHandlers.palPlcRxParamsIndication = _plcRxParamsIndication;
    palPlcInitData.initMIB = true;
    // Initialize the PAL PLC module
    appPLCData.palPlcObj = PAL_PLC_Initialize(PAL_PLC_PHY_INDEX, (const SYS_MODULE_INIT *) &palPlcInitData);
    </code>

  Remarks:
    None.
*/
SYS_MODULE_OBJ PAL_PLC_Initialize(const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init);

// *****************************************************************************
/* Function:
    PAL_PLC_STATUS PAL_PLC_Status ( SYS_MODULE_OBJ object )

  Summary:
    Returns status of the specific instance of the PAL PLC module.

  Description:
    This function returns the status of the specific module instance.

  Precondition:
    The PAL_PLC_Initialize function should have been called before calling
    this function.

  Parameters:
    object    - PAL PLC object handle, returned from PAL_PLC_Initialize

  Returns:
    PAL_PLC_STATUS_READY          - Indicates that the module is initialized and is
                                  ready to accept new requests from the clients.

    PAL_PLC_STATUS_BUSY           - Indicates that the module is busy with a
                                  previous requests from the clients. However,
                                  depending on the configured queue size for
                                  transmit and receive, it may be able to queue
                                  a new request.

    PAL_PLC_STATUS_ERROR          - Indicates that the module is in an error state.
                                  Any value less than SYS_STATUS_ERROR is
                                  also an error state.

    PAL_PLC_STATUS_UNINITIALIZED  - Indicates that the module is not initialized.

  Example:
    <code>
    // Given "object" returned from PAL_PLC_Initialize

    PAL_PLC_STATUS          palPlcStatus;

    palPlcStatus = PAL_PLC_Status (object);
    if (palPlcStatus == PAL_PLC_STATUS_READY)
    {
        // PAL RF is initialized and is ready to accept client requests.
    }
    </code>

  Remarks:
    Application must ensure that the PAL_RF_Status returns PAL_PLC_STATUS_READY
    before performing PAL RF operations.
*/
PAL_PLC_STATUS PAL_PLC_Status(SYS_MODULE_OBJ object);

// *****************************************************************************
/* Function:
    PAL_PLC_HANDLE PAL_PLC_HandleGet(const SYS_MODULE_INDEX index)

  Summary:
    Returns a handle to the requested PAL PLC instance

  Description:
    This function returns a handle to the requested PAL PLC instance.

  Preconditions:
    The PAL_PLC_Initialize function should have been called before calling
    this function.

  Parameters:
    index       - index of the PAL PLC instance

  Returns:
    PAL_RF_HANDLE - Handle to the requested PAL PLC instance

  Example:
    <code>
    PAL_PLC_HANDLE myPalPlcHandle;
    myPalPlcHandle = PAL_PLC_HandleGet(PAL_PLC_INDEX_0);

    if (myPalRfHandle != PAL_PLC_HANDLE_INVALID)
    {
        // Found a valid handle to the PAL PLC instance
    }
    </code>

  Remarks:
    None.
*/
PAL_PLC_HANDLE PAL_PLC_HandleGet(const SYS_MODULE_INDEX index);

// *****************************************************************************
/* Function:
    void PAL_PLC_Deinitialize(SYS_MODULE_OBJ object)

  Summary:
    De-initializes the specified instance of the PAL PLC module.

  Description:
    This function deinitializes the specified instance of the PAL PLC module,
    disabling its operation and invalidates all of the internal data.

  Precondition:
    Function PAL_PLC_Initialize must have been called before calling this
    routine and a valid SYS_MODULE_OBJ must have been returned.

  Parameters:
    object          - PAL RF module object handle, returned by PAL_PLC_Initialize

  Returns:
    None.

  Example:
    <code>
    // This code example shows how the PAL PLC can be deinitialized. 
    // It is assumed the PAL PLC module was already initialized.

    SYS_MODULE_OBJ palPlcobj;

    PAL_PLC_Deinitialize(palPlcobj); 

    </code>

  Remarks:
    Once the Initialize operation has been called, the deinitialize operation
    must be called before the Initialize operation can be called again.
*/
void PAL_PLC_Deinitialize(SYS_MODULE_OBJ object);
 
// *****************************************************************************
/* Function:
    void PAL_PLC_TxRequest(PAL_PLC_HANDLE handle, uint8_t *pData, uint16_t length)

  Summary:
    Allows a client to transmit data through PLC device.

  Description:
    This routine sends a new data message through PLC using the PAL PLC module.

  Precondition:
    PAL_PLC_HandleGet must have been called to obtain a valid PAL PLC handle.

  Parameters:
    handle -       A valid handle, returned from the handle get routine.

    pData -        Pointer to the data to transmit.

    length -       Length of the data to transmit in bytes.

  Returns:
    None.

  Example:
    <code>
    // 'palPlcHandle', returned from the PAL_PLC_HandleGet
    
    // Local function implemented in the user application
    _setupTransmissionParameters();

    PAL_PLC_TxRequest(palPlcHandle, appData.pData, appData.length);

    </code>

  Remarks:
    None.
*/
void PAL_PLC_TxRequest(PAL_PLC_HANDLE handle, uint8_t *pData, uint16_t length);
 
// *****************************************************************************
/* Function:
    void PAL_PLC_Reset(PAL_PLC_HANDLE handle)

  Summary:
    Allows a client to reset PAL PLC module.

  Description:
    This routine performs a reset of the PLC device.

  Precondition:
    PAL_PLC_HandleGet must have been called to obtain a valid PAL PLC handle.

  Parameters:
    handle -       A valid handle, returned from the handle get routine

  Returns:
    None.

  Example:
    <code>
    // 'palPlcHandle', returned from the PAL_PLC_HandleGet
    
    PAL_PLC_Reset(palPlcHandle);

    </code>

  Remarks:
    None.
*/
void PAL_PLC_Reset(PAL_PLC_HANDLE handle, bool resetMib);
 
// *****************************************************************************
/* Function:
    uint32_t PAL_PLC_GetPhyTime(PAL_PLC_HANDLE handle)

  Summary:
    Gets the internal timer reference from PLC transceiver in microseconds

  Description:
    PLC transceiver has an internal 32 bits counter which is used as internal
    time reference for all time calculations in PLC PHY layer. This counter is 
    internally configured to be increased each microsecond. This function allows 
    using this counter as an accurate time reference to upper layers.
    
  Precondition:
    PAL_PLC_HandleGet must have been called to obtain a valid PAL PLC handle.

  Parameters:
    handle -    A valid handle, returned from the handle get routine

  Returns:
    Internal timer reference from PLC transceiver in microseconds.

  Example:
    <code>
    // 'palPlcHandle', returned from the PAL_PLC_HandleGet
    uint32_t plcTimerRef;

    plcTimerRef = PAL_PLC_GetPhyTime(palPlcHandle);

    </code>

  Remarks:
    This function requires an SPI transaction to retrieve time from
    PLC transceiver, so potential jitter in different SPI accesses
    may impact accuracy.
*/
uint32_t PAL_PLC_GetPhyTime(PAL_PLC_HANDLE handle);

// *****************************************************************************
/* Function:
    PAL_PLC_PIB_RESULT PAL_PLC_GetMacRtPib(PAL_PLC_HANDLE handle, MAC_RT_PIB_OBJ *pibObj)

  Summary:
    Gets value of G3 MAC or G3 PHY PIB attribute.

  Description:
    This routine allows a client to get the value of a PIB (PHY Information
    Base) attribute of the PLC device.

  Precondition:
    PAL_PLC_HandleGet must have been called to obtain a valid PAL PLC handle.

  Parameters:
    handle  - A valid handle, returned from the handle get routine.
    pibObj  - Pointer to PIB object to indicate the PIB to read. PIB object includes a 
              data buffer to store the read value.

  Returns:
    Result of getting the PIB (see PAL_PLC_PIB_RESULT).

  Example:
    <code>
    PAL_PLC_HANDLE palPlcHandle; // returned from PAL_PLC_HandleGet

    appPlcData.pibObj.pib = MAC_RT_PIB_MANUF_PHY_PARAM;
    appPlcData.pibObj.index = PHY_PIB_VERSION_NUM;
    appPlcData.pibObj.length = 4;
            
    if (PAL_PLC_GetMacRtPib(palPlcHandle, &appPlcData.pibObj) == PAL_PLC_PIB_SUCCESS)
    {
        appPlcData.phyVersion = *(uint32_t *)appPlcData.pibObj.pData;
    }
    </code>

  Remarks:
    None.
*/
PAL_PLC_PIB_RESULT PAL_PLC_GetMacRtPib(PAL_PLC_HANDLE handle, MAC_RT_PIB_OBJ *pibObj);

// *****************************************************************************
/* Function:
    PAL_PLC_PIB_RESULT PAL_PLC_SetMacRtPib(PAL_PLC_HANDLE handle, MAC_RT_PIB_OBJ *pibObj)

  Summary:
    Sets value of G3 MAC or G3 PHY PIB attribute.

  Description:
    This routine allows a client to set the value of a PIB (PHY Information
    Base) attribute of the PLC device.

  Precondition:
    PAL_PLC_HandleGet must have been called to obtain a valid PAL PLC handle.

  Parameters:
    handle  - A valid handle, returned from the handle get routine.
    pibObj  - Pointer to PIB object to indicate the PIB to write. PIB object includes a 
              data buffer to write the new value.

  Returns:
    Result of getting the PIB (see PAL_PLC_PIB_RESULT).

  Example:
    <code>
    PAL_PLC_HANDLE palPlcHandle; // returned from PAL_PLC_HandleGet

    appPlcData.pibObj.pib = MAC_RT_PIB_PAN_ID;
    appPlcData.pibObj.index = 0;
    appPlcData.pibObj.length = 2;
    appPlcData.pibObj.pData[0] = 0x12;
    appPlcData.pibObj.pData[1] = 0x34;
    
    if (PAL_PLC_SetMacRtPib(palPlcHandle, &appPlcData.pibObj) == PAL_PLC_PIB_SUCCESS)
    {
        // PIB has been written successfully
    }
    </code>

  Remarks:
    None.
*/
PAL_PLC_PIB_RESULT PAL_PLC_SetMacRtPib(PAL_PLC_HANDLE handle, MAC_RT_PIB_OBJ *pibObj);
 
#endif // #ifndef _PAL_PLC_H
/*******************************************************************************
 End of File
*/