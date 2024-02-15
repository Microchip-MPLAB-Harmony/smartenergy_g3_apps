/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    pal_plc.h

  Summary:
    PLC Platform Abstraction Layer (PAL) Interface header file.

  Description:
    PLC Platform Abstraction Layer (PAL) Interface header file. The PLC PAL
    module provides a simple interface to manage the G3-PLC MAC-RT layer.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*
Copyright (C) 2024, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
//DOM-IGNORE-END

#ifndef PAL_PLC_H
#define PAL_PLC_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include "system/system.h"
#include "driver/driver.h"
#include "driver/plc/g3MacRt/drv_g3_macrt_comm.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* PLC PAL PIB Result

  Summary:
    Result of a PLC PAL PIB service client interface operation.

  Description:
    Identifies the result of PLC PAL PIB service operations.

  Remarks:
    None.
*/
typedef enum {

    PAL_PLC_PIB_SUCCESS = MAC_RT_STATUS_SUCCESS,
    PAL_PLC_PIB_DENIED = MAC_RT_STATUS_DENIED,
    PAL_PLC_PIB_INVALID_INDEX = MAC_RT_STATUS_INVALID_INDEX,
    PAL_PLC_PIB_INVALID_PARAMETER = MAC_RT_STATUS_INVALID_PARAMETER,
    PAL_PLC_PIB_READ_ONLY = MAC_RT_STATUS_READ_ONLY,
    PAL_PLC_PIB_UNSUPPORTED_ATTRIBUTE = MAC_RT_STATUS_UNSUPPORTED_ATTRIBUTE,

} PAL_PLC_PIB_RESULT;

// *****************************************************************************
/* PLC PAL Handle

  Summary:
    Handle to a PLC PAL instance.

  Description:
    This data type is a handle to a PLC PAL instance. It can be used to access
    and control the PLC PAL.

  Remarks:
    Do not rely on the underlying type as it may change in different versions
    or implementations of the PLC PAL service.
*/
typedef uintptr_t PAL_PLC_HANDLE;

// *****************************************************************************
/* Invalid PLC PAL handle value to a PLC PAL instance.

  Summary:
    Invalid handle value to a PLC PAL instance.

  Description:
    Defines the invalid handle value to a PLC PAL instance.

  Remarks:
    Do not rely on the actual value as it may change in different versions
    or implementations of the PLC PAL service.
*/
#define PAL_PLC_HANDLE_INVALID   ((PAL_PLC_HANDLE) (-1))

// *****************************************************************************
/* PLC PAL Module Status

  Summary:
    Identifies the current status/state of the PLC PAL module.

  Description:
    This enumeration identifies the current status/state of the PLC PAL module.

  Remarks:
    This enumeration is the return type for the PAL_PLC_Status routine. The
    upper layer must ensure that PAL_PLC_Status returns PAL_PLC_STATUS_READY
    before performing PLC PAL operations.
*/
typedef enum {
    PAL_PLC_STATUS_UNINITIALIZED = SYS_STATUS_UNINITIALIZED,
    PAL_PLC_STATUS_BUSY = SYS_STATUS_BUSY,
    PAL_PLC_STATUS_READY = SYS_STATUS_READY,
    PAL_PLC_STATUS_ERROR = SYS_STATUS_ERROR,
    PAL_PLC_STATUS_INVALID_OBJECT = SYS_STATUS_ERROR_EXTENDED - 1,
} PAL_PLC_STATUS;

// *****************************************************************************
/* PLC PAL Data Indication Event Handler Function Pointer

   Summary
    Pointer to a PLC PAL Data Indication event handler function.

   Description
    This data type defines the required function signature for the PLC PAL
    reception data indication event handling callback function. When
    PAL_PLC_Initialize is called, a client must register a pointer whose
    function signature (parameter and return value types) matches the types
    specified by this function pointer in order to receive transfer related
    event calls back from the PLC PAL.

    The parameters and return values are described here and a partial example
    implementation is provided.

  Parameters:
    pData -     Pointer to the data buffer containing received data.

    length -    Length of the received data in bytes.

  Returns:
    None.

  Example:
    <code>
    static void _plcDataIndication(uint8_t *pData, uint16_t length)
    {

    }

    PAL_PLC_INIT palPlcInitData;
    SYS_MODULE_OBJ palPlcObj;

    palPlcInitData.macRtBand = G3_FCC;
    palPlcInitData.macRtHandlers.palPlcDataIndication = _plcDataIndication;
    palPlcInitData.macRtHandlers.palPlcTxConfirm = _plcTxConfirm;
    palPlcInitData.macRtHandlers.palPlcCommStatusIndication = _plcCommStatusIndication;
    palPlcInitData.macRtHandlers.palPlcRxParamsIndication = _plcRxParamsIndication;
    palPlcInitData.initMIB = true;

    palPlcObj = PAL_PLC_Initialize(PAL_PLC_PHY_INDEX, (const SYS_MODULE_INIT *) &palPlcInitData);
    </code>

  Remarks:
    None.
*/
typedef void (*PAL_PLC_DataIndication)(uint8_t *pData, uint16_t length);

// *****************************************************************************
/* PLC PAL Transmission Confirm Event Handler Function Pointer

   Summary
    Pointer to a PLC PAL Transmission Confirm event handler function.

   Description
    This data type defines the required function signature for the PLC PAL
    transmission confirm event handling callback function. When
    PAL_PLC_Initialize is called, a client must register a pointer whose
    function signature (parameter and return value types) matches the types
    specified by this function pointer in order to receive transfer related
    event calls back from the PLC PAL.

    The parameters and return values are described here and a partial example
    implementation is provided.

  Parameters:
    status -            MAC RT status corresponding to the transmission result.

    updateTimestamp -   Flag to indicate if timestamp should be updated.

  Returns:
    None.

  Example:
    <code>
    static void _plcTxConfirm(MAC_RT_STATUS status, bool updateTimestamp)
    {
        switch(status)
        {
            case MAC_RT_STATUS_SUCCESS:
                break;
            case MAC_RT_STATUS_CHANNEL_ACCESS_FAILURE:
                break;
            case MAC_RT_STATUS_NO_ACK:
                break;
        }
    }

    PAL_PLC_INIT palPlcInitData;
    SYS_MODULE_OBJ palPlcObj;

    palPlcInitData.macRtBand = G3_FCC;
    palPlcInitData.macRtHandlers.palPlcDataIndication = _plcDataIndication;
    palPlcInitData.macRtHandlers.palPlcTxConfirm = _plcTxConfirm;
    palPlcInitData.macRtHandlers.palPlcCommStatusIndication = _plcCommStatusIndication;
    palPlcInitData.macRtHandlers.palPlcRxParamsIndication = _plcRxParamsIndication;
    palPlcInitData.initMIB = true;

    palPlcObj = PAL_PLC_Initialize(PAL_PLC_PHY_INDEX, (const SYS_MODULE_INIT *) &palPlcInitData);
    </code>

  Remarks:
    If the status is MAC_RT_STATUS_SUCCESS, it means that the data was
    transferred successfully. Otherwise, it means that the data was not
    transferred successfully.
*/
typedef void (*PAL_PLC_TxConfirm)(MAC_RT_STATUS status, bool updateTimestamp);

// *****************************************************************************
/* PLC PAL Event Handler Function Pointer to get parameters from the
   last received message

   Summary
    Pointer to a PLC PAL event handler function to get parameters from the last
    received message.

   Description
    This data type defines the required function signature for the PLC PAL
    reception parameters event handling callback function. When
    PAL_PLC_Initialize is called, a client must register a pointer whose
    function signature (parameter and return value types) matches the types
    specified by this function pointer in order to receive transfer related
    event calls back from the PLC PAL.

    The parameters and return values are described here and a partial example
    implementation is provided.

  Parameters:
    pParameters - Pointer to the parameters of the last received message.

  Returns:
    None.

  Example:
    <code>
    static MAC_RT_RX_PARAMETERS_OBJ rxParameters;

    static void _plcRxParamsIndication(MAC_RT_RX_PARAMETERS_OBJ *pParameters)
    {
        memcpy(rxParameters, pParameters, sizeof(MAC_RT_RX_PARAMETERS_OBJ));
    }

    PAL_PLC_INIT palPlcInitData;
    SYS_MODULE_OBJ palPlcObj;

    palPlcInitData.macRtBand = G3_FCC;
    palPlcInitData.macRtHandlers.palPlcDataIndication = _plcDataIndication;
    palPlcInitData.macRtHandlers.palPlcTxConfirm = _plcTxConfirm;
    palPlcInitData.macRtHandlers.palPlcCommStatusIndication = _plcCommStatusIndication;
    palPlcInitData.macRtHandlers.palPlcRxParamsIndication = _plcRxParamsIndication;
    palPlcInitData.initMIB = true;

    palPlcObj = PAL_PLC_Initialize(PAL_PLC_PHY_INDEX, (const SYS_MODULE_INIT *) &palPlcInitData);
    </code>

  Remarks:
    This handler function is called before the data indication callback.
*/
typedef void (*PAL_PLC_RxParamsIndication)(MAC_RT_RX_PARAMETERS_OBJ *pParameters);

// *****************************************************************************
/* PLC PAL Communication Status Handler Function Pointer

   Summary
    Pointer to a PLC PAL Communication Status event handler function.

   Description
    This data type defines the required function signature for the PLC PAL
    communication status event handling callback function. When
    PAL_PLC_Initialize is called, a client must register a pointer whose
    function signature (parameter and return value types) matches the types
    specified by this function pointer in order to receive transfer related
    event calls back from the PLC PAL.

    The parameters and return values are described here and a partial example
    implementation is provided.

  Parameters:
    pData -             Pointer to the data content.

  Returns:
    None.

  Example:
    <code>
    static uint8_t macCommStatusBuffer[MAC_RT_FULL_HEADER_SIZE];

    static void _plcCommStatusIndication(uint8_t *pData)
    {
        memcpy(macCommStatusBuffer, pData, MAC_RT_FULL_HEADER_SIZE);
    }

    PAL_PLC_INIT palPlcInitData;
    SYS_MODULE_OBJ palPlcObj;

    palPlcInitData.macRtBand = G3_FCC;
    palPlcInitData.macRtHandlers.palPlcDataIndication = _plcDataIndication;
    palPlcInitData.macRtHandlers.palPlcTxConfirm = _plcTxConfirm;
    palPlcInitData.macRtHandlers.palPlcCommStatusIndication = _plcCommStatusIndication;
    palPlcInitData.macRtHandlers.palPlcRxParamsIndication = _plcRxParamsIndication;
    palPlcInitData.initMIB = true;

    palPlcObj = PAL_PLC_Initialize(PAL_PLC_PHY_INDEX, (const SYS_MODULE_INIT *) &palPlcInitData);
    </code>

  Remarks:
    None.
*/
typedef void (*PAL_PLC_CommStatusIndication)(uint8_t *pData);

// *****************************************************************************
/* PLC PAL G3 MAC Sniffer Event Handler Function Pointer

   Summary
    Pointer to a PLC PAL G3 MAC Sniffer Reception Event handler function.

   Description
    This data type defines the required function signature for the PLC PAL G3
    MAC sniffer event handling callback function. When PAL_PLC_Initialize is
    called, a client must register a pointer whose function signature (parameter
    and return value types) matches the types specified by this function pointer
    in order to receive transfer related event calls back from the PLC PAL.

    The parameters and return values are described here and a partial example
    implementation is provided.

  Parameters:
    pData -             Pointer to the data content.

    length -            Length of the received data in bytes.

  Returns:
    None.

  Example:
    <code>
    static void _plcMACSnifferIndication( uint8_t *pData, uint16_t length )
    {

    }

    PAL_PLC_INIT palPlcInitData;
    SYS_MODULE_OBJ palPlcObj;

    palPlcInitData.macRtBand = G3_FCC;
    palPlcInitData.macRtHandlers.palPlcDataIndication = _plcDataIndication;
    palPlcInitData.macRtHandlers.palPlcTxConfirm = _plcTxConfirm;
    palPlcInitData.macRtHandlers.palPlcCommStatusIndication = _plcCommStatusIndication;
    palPlcInitData.macRtHandlers.palPlcRxParamsIndication = _plcRxParamsIndication;
    palPlcInitData.macRtHandlers.palPlcMacSnifferIndication = _plcMACSnifferIndication;
    palPlcInitData.initMIB = true;

    palPlcObj = PAL_PLC_Initialize(PAL_PLC_PHY_INDEX, (const SYS_MODULE_INIT *) &palPlcInitData);
    </code>

  Remarks:
    None.
*/
typedef void (*PAL_PLC_MacSnifferIndication)(uint8_t *pData, uint16_t length);

// *****************************************************************************
/* PLC PAL Handlers Data

  Summary:
    Defines the handlers required to manage the PLC PAL module.

  Description:
    This data type defines the handlers required to manage the PLC PAL module.

  Remarks:
    None.
*/
typedef struct
{
    PAL_PLC_DataIndication           palPlcDataIndication;
    PAL_PLC_CommStatusIndication     palPlcCommStatusIndication;
    PAL_PLC_TxConfirm                palPlcTxConfirm;
    PAL_PLC_RxParamsIndication       palPlcRxParamsIndication;
    PAL_PLC_MacSnifferIndication     palPlcMacSnifferIndication;
} PAL_PLC_HANDLERS;

// *****************************************************************************
/* PLC PAL Initialization Data

  Summary:
    Defines the data required to initialize the PLC PAL module.

  Description:
    This data type defines the data required to initialize the PLC PAL module.

  Remarks:
    None.
*/
typedef struct
{
    // PLC PAL Handlers
    PAL_PLC_HANDLERS                 macRtHandlers;

    // G3 Phy band
    MAC_RT_BAND                      macRtBand;

    // Flag to indicate if MIB data should be reinitialized with the default values
    bool                             initMIB;
} PAL_PLC_INIT;

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
    Initializes the PLC PAL module.

  Description:
    This routine initializes the PLC PAL module, making it ready for clients to
    use it. The initialization data is specified by the init parameter. It is a
    single instance module, so this function should be called only once (unless
    PAL_PLC_Deinitialize is called).

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
    SYS_MODULE_OBJ palPlcObj;

    palPlcInitData.macRtBand = G3_FCC;
    palPlcInitData.macRtHandlers.palPlcDataIndication = _plcDataIndication;
    palPlcInitData.macRtHandlers.palPlcTxConfirm = _plcTxConfirm;
    palPlcInitData.macRtHandlers.palPlcCommStatusIndication = _plcCommStatusIndication;
    palPlcInitData.macRtHandlers.palPlcRxParamsIndication = _plcRxParamsIndication;
    palPlcInitData.initMIB = true;

    palPlcObj = PAL_PLC_Initialize(PAL_PLC_PHY_INDEX, (const SYS_MODULE_INIT *) &palPlcInitData);
    </code>

  Remarks:
    This routine must be called before any other PLC PAL routine is called.
*/
SYS_MODULE_OBJ PAL_PLC_Initialize(const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init);

// *****************************************************************************
/* Function:
    PAL_PLC_STATUS PAL_PLC_Status ( SYS_MODULE_OBJ object )

  Summary:
    Returns status of the specific instance of the PLC PAL module.

  Description:
    This function returns the status of the specific PLC PAL module instance.

  Precondition:
    The PAL_PLC_Initialize function should have been called before calling
    this function.

  Parameters:
    object    - PLC PAL object handle, returned from PAL_PLC_Initialize.

  Returns:
    PAL_PLC_STATUS_READY - Indicates that the module is initialized and is
    ready to accept new requests from the client.

    PAL_PLC_STATUS_BUSY - Indicates that the module is busy with a previous
    initialization request from the client.

    PAL_PLC_STATUS_ERROR - Indicates that the module is in an error state.
    Any value lower than SYS_STATUS_ERROR is also an error state.

    PAL_PLC_STATUS_UNINITIALIZED - Indicates that the module is not initialized.

  Example:
    <code>
    PAL_PLC_STATUS palPlcStatus;

    palPlcStatus = PAL_PLC_Status (object);
    if (palPlcStatus == PAL_PLC_STATUS_READY)
    {

    }
    </code>

  Remarks:
    The upper layer must ensure that PAL_PLC_Status returns PAL_PLC_STATUS_READY
    before performing PLC PAL operations.
*/
PAL_PLC_STATUS PAL_PLC_Status(SYS_MODULE_OBJ object);

// *****************************************************************************
/* Function:
    PAL_PLC_HANDLE PAL_PLC_HandleGet(const SYS_MODULE_INDEX index)

  Summary:
    Returns a handle to the requested PLC PAL instance.

  Description:
    This function returns a handle to the requested PLC PAL instance.

  Preconditions:
    The PAL_PLC_Initialize function should have been called before calling
    this function.

  Parameters:
    index - Index of the PLC PAL instance. Only one instance (index 0)
            supported.

  Returns:
    Handle (PAL_PLC_HANDLE type) to the requested PLC PAL instance.
    PAL_PLC_HANDLE_INVALID is returned if the index parameter is invalid.

  Example:
    <code>
    PAL_PLC_HANDLE myPalPlcHandle;
    myPalPlcHandle = PAL_PLC_HandleGet(PAL_PLC_INDEX_0);

    if (myPalPlcHandle != PAL_PLC_HANDLE_INVALID)
    {

    }
    </code>

  Remarks:
    The handle returned by this function is needed to call other functions of
    the PLC PAL module.
*/
PAL_PLC_HANDLE PAL_PLC_HandleGet(const SYS_MODULE_INDEX index);

// *****************************************************************************
/* Function:
    void PAL_PLC_Deinitialize(SYS_MODULE_OBJ object)

  Summary:
    De-initializes the specified instance of the PLC PAL module.

  Description:
    This function de-initializes the specified instance of the PLC PAL module,
    disabling its operation and invalidates all of the internal data.

  Precondition:
    Function PAL_PLC_Initialize must have been called before calling this
    routine and a valid SYS_MODULE_OBJ must have been returned.

  Parameters:
    object       - PLC PAL module object handle, returned by PAL_PLC_Initialize.

  Returns:
    None.

  Example:
    <code>
    SYS_MODULE_OBJ palPlcobj;

    PAL_PLC_Deinitialize(palPlcobj);
    </code>

  Remarks:
    Once PAL_PLC_Initialize has been called, PAL_PLC_Deinitialize must be
    called before PAL_PLC_Initialize can be called again.
*/
void PAL_PLC_Deinitialize(SYS_MODULE_OBJ object);

// *****************************************************************************
/* Function:
    void PAL_PLC_TxRequest(PAL_PLC_HANDLE handle, uint8_t *pData, uint16_t length)

  Summary:
    Allows a client to transmit data through PLC device.

  Description:
    This routine sends a new data message through PLC using the PLC PAL module.
    The message must have a valid IEEE 802.15.4 MAC format with its
    corresponding MAC header.

  Precondition:
    PAL_PLC_HandleGet must have been called to obtain a valid PLC PAL handle.

  Parameters:
    handle -       A valid handle, returned from PAL_PLC_HandleGet.

    pData -        Pointer to the data to transmit.

    length -       Length of the data to transmit in bytes.

  Returns:
    None.

  Example:
    <code>
    MAC_RT_HEADER macHeader;
    uint16_t totalLength;
    uint16_t headerLength;
    uint16_t payloadLength = 100;
    uint8_t txData[MAC_RT_DATA_MAX_SIZE];
    uint8_t payloadData[MAC_RT_MAX_PAYLOAD_SIZE];

    headerLength = _buildMacHeader(txData, &macHeader);

    memcpy(&txData[headerLength], payloadData, payloadLength);
    totalLength = headerLength + payloadLength;

    PAL_PLC_TxRequest(palPlcHandle, txData, totalLength);
    </code>

  Remarks:
    None.
*/
void PAL_PLC_TxRequest(PAL_PLC_HANDLE handle, uint8_t *pData, uint16_t length);

// *****************************************************************************
/* Function:
    void PAL_PLC_Reset(PAL_PLC_HANDLE handle, bool resetMib)

  Summary:
    Allows a client to reset the PLC PAL module.

  Description:
    This routine performs a reset of the PLC device.

  Precondition:
    PAL_PLC_HandleGet must have been called to obtain a valid PLC PAL handle.

  Parameters:
    handle -       A valid handle, returned from PAL_PLC_HandleGet.

    resetMib -     Flag to indicate if MIBs has to been reset too.

  Returns:
    None.

  Example:
    <code>
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
    Gets the internal timer reference from PLC transceiver in microseconds.

  Description:
    The PLC transceiver has an internal 32-bit counter which is used as time
    reference for all time calculations in the PLC PHY layer. This counter is
    internally configured to be increased each microsecond. This function allows
    to use this counter as an accurate time reference for upper layers.

  Precondition:
    PAL_PLC_HandleGet must have been called to obtain a valid PLC PAL handle.

  Parameters:
    handle -    A valid handle, returned from PAL_PLC_HandleGet.

  Returns:
    Internal timer reference from PLC transceiver in microseconds.

  Example:
    <code>
    uint32_t plcTimerRef;

    plcTimerRef = PAL_PLC_GetPhyTime(palPlcHandle);
    </code>

  Remarks:
    This function requires an SPI transaction to retrieve the time from the PLC
    transceiver, so potential jitter in different SPI accesses may impact the
    accuracy.
*/
uint32_t PAL_PLC_GetPhyTime(PAL_PLC_HANDLE handle);

// *****************************************************************************
/* Function:
    PAL_PLC_PIB_RESULT PAL_PLC_GetMacRtPib(PAL_PLC_HANDLE handle, MAC_RT_PIB_OBJ *pibObj)

  Summary:
    Gets value of G3 MAC-RT or G3 PHY PIB attribute.

  Description:
    This routine allows a client to get information from PLC transceiver about
    MAC-RT and PHY information base (PIB).

  Precondition:
    PAL_PLC_HandleGet must have been called to obtain a valid PLC PAL handle.

  Parameters:
    handle  - A valid handle, returned from PAL_PLC_HandleGet.
    pibObj  - Pointer to PIB object to indicate the PIB to read. PIB object
              includes a data buffer to store the read value.

  Returns:
    Result of getting the PIB (see PAL_PLC_PIB_RESULT).

  Example:
    <code>
    MAC_RT_PIB_OBJ pibObj;
    uint32_t phyVersion;

    pibObj.pib = MAC_RT_PIB_MANUF_PHY_PARAM;
    pibObj.index = PHY_PIB_VERSION_NUM;
    pibObj.length = 4;

    if (PAL_PLC_GetMacRtPib(palPlcHandle, &pibObj) == PAL_PLC_PIB_SUCCESS)
    {
        phyVersion = *(uint32_t *)appPlcData.pibObj.pData;
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
    Sets value of G3 MAC-RT or G3 PHY PIB attribute.

  Description:
    This routine allows a client to set information to PLC transceiver on MAC-RT
    and PHY information base (PIB).

  Precondition:
    PAL_PLC_HandleGet must have been called to obtain a valid PLC PAL handle.

  Parameters:
    handle  - A valid handle, returned from PAL_PLC_HandleGet.
    pibObj  - Pointer to PIB object to indicate the PIB to write. PIB object
              includes a data buffer to write the new value.

  Returns:
    Result of setting the PIB (see PAL_PLC_PIB_RESULT).

  Example:
    <code>
    MAC_RT_PIB_OBJ pibObj;

    pibObj.pib = MAC_RT_PIB_PAN_ID;
    pibObj.index = 0;
    pibObj.length = 2;
    pibObj.pData[0] = 0x12;
    pibObj.pData[1] = 0x34;

    if (PAL_PLC_SetMacRtPib(palPlcHandle, &pibObj) == PAL_PLC_PIB_SUCCESS)
    {

    }
    </code>

  Remarks:
    None.
*/
PAL_PLC_PIB_RESULT PAL_PLC_SetMacRtPib(PAL_PLC_HANDLE handle, MAC_RT_PIB_OBJ *pibObj);

// *****************************************************************************
/* Function:
    void PAL_PLC_SetCoordinator(PAL_PLC_HANDLE handle)

  Summary:
    Allows a client to configure the G3-PLC MAC-RT driver as coordinator.

  Description:
    This routine configures the G3-PLC MAC-RT driver as coordinator.

  Precondition:
    PAL_PLC_HandleGet must have been called to obtain a valid PLC PAL handle.

  Parameters:
    handle  - A valid handle, returned from PAL_PLC_HandleGet.

  Returns:
    None.

  Example:
    <code>
    PAL_PLC_SetCoordinator(palPlcHandle);
    </code>

  Remarks:
    None.
*/
void PAL_PLC_SetCoordinator(PAL_PLC_HANDLE handle);

#endif // #ifndef PAL_PLC_H
/*******************************************************************************
 End of File
*/