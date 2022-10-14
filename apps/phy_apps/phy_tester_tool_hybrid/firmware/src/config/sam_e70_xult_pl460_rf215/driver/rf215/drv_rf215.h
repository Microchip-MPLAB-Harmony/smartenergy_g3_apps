/*******************************************************************************
  RF215 Driver Interface Header File

  Company:
    Microchip Technology Inc.

  File Name:
    drv_rf215.h

  Summary:
    RF215 Driver Interface Header File

  Description:
    The RF215 driver provides a simple interface to manage the PHY layer of
    RF215 transceiver. This file provides the interface definition for the
    RF215 driver.
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

#ifndef _DRV_RF215_H
#define _DRV_RF215_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************
#include "system/system.h"
#include "driver/driver.h"
#include "driver/rf215/drv_rf215_definitions.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* RF215 Driver TX Handle

  Summary:
    Handle identifying the transmission request queued.

  Description:
    A transmission handle value is returned by a call to the DRV_RF215_TxRequest
    function. This handle is associated with the transmission request and it is
    returned back to the client by the transmit confirm callback function
    registered with the driver.

  Remarks:
    None
*/

typedef uintptr_t DRV_RF215_TX_HANDLE;

// *****************************************************************************
/* RF215 Driver Invalid TX Handle

  Summary:
    Definition of an invalid transmission handle.

  Description:
    This is the definition of an invalid transmission handle. An invalid
    transmission handle is returned by DRV_RF215_TxRequest function if the
    request was not successful. It can happen due to invalid arguments, busy
    channel detected, or lack of space in the queue.

  Remarks:
    None
*/

#define DRV_RF215_TX_HANDLE_INVALID ((DRV_RF215_TX_HANDLE)(-1))

// *****************************************************************************
/* RF215 Driver Transceiver Identifier

  Summary:
    Defines the available RF215 transceivers.

  Description:
    This data type defines the available RF215 transceivers (Sub-GHz and
    2.4GHz). It is used in DRV_RF215_Open to identify the transceiver to be used
    by the client.

  Remarks:
    None.
*/

typedef enum
{
    RF215_TRX_ID_RF09  = 0,
    RF215_TRX_ID_RF24  = 1,

} DRV_RF215_TRX_ID;

// *****************************************************************************
/* RF215 Driver RX Indication Callback

  Summary:
    Pointer to a RF215 driver receive indication callback function.

  Description:
    This data type defines the required function signature for the RF215 driver
    receive event handling callback function. A client uses
    DRV_RF215_RxIndCallbackRegister to register a pointer whose function
    signature (parameter and return value types) must match the signature
    specified by this data type.

    The parameters and return values are described here and a partial example
    implementation is provided.

  Parameters:
    indObj  - Pointer to the object containing the data associated to the
              receive indication.

    context - Value identifying the context of the application that registered
              the callback function.

  Returns:
    None.

  Example:
    <code>
    </code>

  Remarks:
    The context parameter contains a handle to the client context, provided at
    the time the callback function was registered using
    DRV_RF215_RxIndCallbackRegister. This context value is passed back to the
    client as parameter. It can be any value necessary to identify the client
    context or instance (such as a pointer to the client's data) of the client
    that registered the callback.
*/

typedef void ( *DRV_RF215_RX_IND_CALLBACK ) (
    DRV_RF215_RX_INDICATION_OBJ *indObj,
    uintptr_t context
);

// *****************************************************************************
/* RF215 Driver TX Confirm Callback

  Summary:
    Pointer to a RF215 driver transmit confirm callback function.

  Description:
    This data type defines the required function signature for the RF215 driver
    transmit confirm event handling callback function. A client uses
    DRV_RF215_TxCfmCallbackRegister to register a pointer whose function
    signature (parameter and return value types) must match the signature
    specified by this data type.

    The parameters and return values are described here and a partial example
    implementation is provided.

  Parameters:
    txHandle - Transmission handle returned from DRV_RF215_TxRequest.

    cfmObj   - Pointer to the object containing the data associated to the
               transmit confirm.

    context  - Value identifying the context of the application that registered
               the callback function.

  Returns:
    None.

  Example:
    <code>
    </code>

  Remarks:
    The context parameter contains a handle to the client context, provided at
    the time the callback function was registered using
    DRV_RF215_TxCfmCallbackRegister. This context value is passed back to the
    client as parameter. It can be any value necessary to identify the client
    context or instance (such as a pointer to the client's data) of the client
    that registered the callback.
*/

typedef void ( *DRV_RF215_TX_CFM_CALLBACK ) (
    DRV_RF215_TX_HANDLE txHandle,
    DRV_RF215_TX_CONFIRM_OBJ *cfmObj,
    uintptr_t context
);

// *****************************************************************************
// *****************************************************************************
// Section: RF215 Driver System Interface Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ DRV_RF215_Initialize (
        const SYS_MODULE_INDEX index,
        const SYS_MODULE_INIT * const init
    )

  Summary:
    Initializes the RF215 driver.

  Description:
    This routine initializes the RF215 driver, making it ready for clients to
    open and use it. The initialization data is specified by the init parameter.
    It is a single instance driver, so this function should be called only once.

  Precondition:
    None.

  Parameters:
    index - Identifier for the instance to be initialized. Only one instance
            (index 0) supported.

    init  - Pointer to the initialization data structure containing any data
            necessary to initialize the driver.

  Returns:
    If successful, returns a valid handle to a driver instance object.
    Otherwise, returns SYS_MODULE_OBJ_INVALID.

  Example:
    <code>
    </code>

  Remarks:
    This routine must be called before any other RF215 Driver routine.
    This routine should only be called once during system initialization.
*/

SYS_MODULE_OBJ DRV_RF215_Initialize (
    const SYS_MODULE_INDEX index,
    const SYS_MODULE_INIT * const init
);

// *****************************************************************************
/* Function:
    SYS_STATUS DRV_RF215_Status( SYS_MODULE_OBJ object )

  Summary:
    Gets the current status of the RF215 driver module.

  Description:
    This routine provides the current status of the RF215 driver module.

  Precondition:
    Function DRV_RF215_Initialize should have been called before calling this
    function.

  Parameters:
    object - Driver object handle, returned from the DRV_RF215_Initialize
             routine

  Returns:
    SYS_STATUS_READY         - Indicates that the RF215 driver module
                               initialization has completed successfully
    SYS_STATUS_BUSY          - Indicates that the RF215 driver module
                               initialization has not yet completed
    SYS_STATUS_ERROR         - Indicates that the RF215 driver module is in an
                               error state
    SYS_STATUS_UNINITIALIZED - Indicates that the RF215 driver module is not
                               initialized

  Example:
    <code>
    SYS_MODULE_OBJ object;  // Returned from DRV_RF215_Initialize
    SYS_STATUS rfPhyStatus;

    rfPhyStatus = DRV_RF215_Status(object);
    if (rfPhyStatus == SYS_STATUS_READY)
    {
        // This means now the driver can be opened using DRV_RF215_Open routine
    }
    </code>

  Remarks:
    The RF215 driver can be opened only when its status is SYS_STATUS_READY.
*/

SYS_STATUS DRV_RF215_Status( SYS_MODULE_OBJ object );

// *****************************************************************************
/* Function:
    void DRV_RF215_Tasks( SYS_MODULE_OBJ object )

  Summary:
    Maintains the RF215 driver's state machine.

  Description:
    This function is used to maintain the RF215 driver's internal state
    machine.

  Precondition:
    The DRV_RF215_Initialize routine must have been called to obtain a valid
    system object.

  Parameters:
    object - Object handle for the driver instance (returned from
             DRV_RF215_Initialize)
  Returns:
    None
  Example:
    <code>
    // object returned from DRV_RF215_Initialize

    while (true)
    {
        DRV_RF215_Tasks (object);

        // Do other tasks
    }
    </code>

  Remarks:
    This function is normally not called directly by an application. It is
    called by the system's Tasks routine (SYS_Tasks)
*/

void DRV_RF215_Tasks( SYS_MODULE_OBJ object );

// *****************************************************************************
/* Function:
    DRV_HANDLE DRV_RF215_Open (
        const SYS_MODULE_INDEX index,
        const DRV_RF215_TRX_ID trxID
    )

  Summary:
    Opens the specified RF215 driver instance and returns a handle to it.

  Description:
    This routine opens the specified RF215 driver instance and provides a handle
    that must be provided to all other client-level operations to identify the
    caller and the instance of the driver. The trxID parameter defines the RF215
    transceiver (Sub-GHz or 2.4GHz) used by the client.

  Precondition:
    Function DRV_RF215_Initialize must have been called before calling this
    function.

  Parameters:
    index - Identifier for the object instance to be opened. Only one instance
            (index 0) supported.

    trxID - Identifier of the RF215 transceiver (Sub-GHz or 2.4GHz) used by the
            client.

  Returns:
    If successful, the routine returns a valid open-instance handle (a number
    identifying both the caller and the module instance).

    If an error occurs, the return value is DRV_HANDLE_INVALID. Error can occur
    - if the number of client objects allocated via DRV_RF215_CLIENTS_NUMBER is
      insufficient.
    - if the specified instance index is not supported.
    - if the specified RF215 transceiver is not supported or disabled in MCC
      configuration.
    - if the driver instance being opened is not ready to be opened (not
      initialized, initialization in progress or in error state because of wrong
      peripheral/hardware configuration).
    - if the driver is not ready to be opened, typically when the initialize
      routine has not completed execution.

  Example:
    <code>
    DRV_HANDLE handle;

    handle = DRV_RF215_Open(DRV_RF215_INDEX_0, RF215_TRX_ID_RF09);
    if (handle != DRV_HANDLE_INVALID)
    {
        // Driver opened successfully
    }
    </code>

  Remarks:
    - The handle returned is valid until the DRV_RF215_Close routine is called.
    - This routine will never block waiting for hardware.
*/

DRV_HANDLE DRV_RF215_Open (
    const SYS_MODULE_INDEX index,
    const DRV_RF215_TRX_ID trxID
);

// *****************************************************************************
/* Function:
    void DRV_RF215_Close( DRV_Handle drvHandle )

  Summary:
    Closes an opened-instance of the RF215 driver.

  Description:
    This routine closes an opened-instance of the RF215 driver, invalidating the
    handle. A new handle must be obtained by calling DRV_RF215_Open before the
    caller may use the driver again.

  Precondition:
    DRV_RF215_Open must have been called to obtain a valid opened driver handle.

  Parameters:
    drvHandle - A valid open-instance handle, returned from the driver's open
                routine.

  Returns:
    None.

  Example:
    <code>
    // 'drvHandle', returned from the DRV_RF215_Open

    DRV_RF215_Close(drvHandle);

    </code>

  Remarks:
    None.
*/

void DRV_RF215_Close( const DRV_HANDLE drvHandle);

// *****************************************************************************
/* Function:
    void DRV_RF215_RxIndCallbackRegister (
        DRV_HANDLE drvHandle,
        const DRV_RF215_RX_IND_CALLBACK callback,
        uintptr_t context
    )

  Summary:
    Allows a client to set an event handling function for the driver to call
    back when a new PPDU is received.

  Description:
    This function allows a client to register a RF215 receive indication event
    handling function for the driver to call back when a new PPDU is received.

    The callback should be registered immediately after opening the driver. The
    callback once set, persists until the client closes the driver or sets
    another callback (which could be a "NULL" pointer to indicate no callback).

  Precondition:
    DRV_RF215_Open must have been called to obtain a valid opened driver handle.

  Parameters:
    drvHandle - A valid open-instance handle, returned from the driver's open
                routine.

    callback  - Pointer to the callback function.

    context   - The value of parameter will be passed back to the client
                unchanged, when the callback function is called.

  Returns:
    None.

  Example:
    <code>
    </code>

  Remarks:
    None.
*/

void DRV_RF215_RxIndCallbackRegister (
    DRV_HANDLE drvHandle,
    const DRV_RF215_RX_IND_CALLBACK callback,
    uintptr_t context
);

// *****************************************************************************
/* Function:
    void DRV_RF215_TxCfmCallbackRegister (
        DRV_HANDLE drvHandle,
        const DRV_RF215_TX_CFM_CALLBACK callback,
        uintptr_t context
    )

  Summary:
    Allows a client to set an event handling function for the driver to call
    back when a transmission request has finished.

  Description:
    This function allows a client to register a RF215 transmit confirm event
    handling function for the driver to call back when a transmission request
    has finished, successfully or not.

    The callback should be registered immediately after opening the driver. The
    callback once set, persists until the client closes the driver or sets
    another callback (which could be a "NULL" pointer to indicate no callback).

  Precondition:
    DRV_RF215_Open must have been called to obtain a valid opened driver handle.

  Parameters:
    drvHandle - A valid open-instance handle, returned from the driver's open
                routine.

    callback  - Pointer to the callback function.

    context   - The value of parameter will be passed back to the client
                unchanged, when the callback function is called.

  Returns:
    None.

  Example:
    <code>
    </code>

  Remarks:
    None.
*/

void DRV_RF215_TxCfmCallbackRegister (
    DRV_HANDLE drvHandle,
    const DRV_RF215_TX_CFM_CALLBACK callback,
    uintptr_t context
);

// *****************************************************************************
/* Function:
    DRV_RF215_TX_HANDLE DRV_RF215_TxRequest (
        DRV_HANDLE drvHandle,
        DRV_RF215_TX_REQUEST_OBJ* reqObj,
        DRV_RF215_TX_RESULT* result
    )

  Summary:
    Allows a client to request data transmission through RF.

  Description:
    This routine allows a client to request a new data transmission through RF
    using the RF215 device.

  Precondition:
    DRV_RF215_Open must have been called to obtain a valid opened driver handle.

  Parameters:
    drvHandle - A valid open-instance handle, returned from the driver's open
                routine.
    reqObj    - Pointer to the object containing TX request parameters.
    result    - Pointer to result of the TX request.

  Returns:
    If successful, the routine returns a valid scheduled-transmission handle (a
    number identifying the transmission request). Once the transmission finishes
    (successfully or not), it is notified via TX confirm callback;

    If an error occurs, the return value is DRV_RF215_TX_HANDLE_INVALID. In this
    case the error type will be reported via result parameter. Error can occur
    - if the number of TX buffer objects allocated via
      DRV_RF215_TX_BUFFERS_NUMBER is insufficient.
    - if the specified driver handle is not valid.
    - if transmission request parameters are not valid.

  Example:
    <code>
    </code>

  Remarks:
    The handle returned is valid until the TX confirm routine is called back.
    This routine will never block waiting for hardware.
*/

DRV_RF215_TX_HANDLE DRV_RF215_TxRequest (
    DRV_HANDLE drvHandle,
    DRV_RF215_TX_REQUEST_OBJ* reqObj,
    DRV_RF215_TX_RESULT* result
);

// *****************************************************************************
/* Function:
    void DRV_RF215_TxCancel(DRV_HANDLE drvHandle, DRV_RF215_TX_HANDLE txHandle)

  Summary:
    Allows a client to cancel a previously programmed transmission.

  Description:
    This routine allows a client to cancel a previously programmed transmission.

  Precondition:
    DRV_RF215_Open must have been called to obtain a valid opened driver handle.
    DRV_RF215_TxRequest must have been called to obtain a valid TX handle.

  Parameters:
    drvHandle - A valid open-instance handle, returned from the driver's open
                routine.
    txHandle  - A valid transmission handle returned from DRV_RF215_TxRequest.

  Returns:
    None.

  Example:
    <code>
    </code>

  Remarks:
    None.
*/

void DRV_RF215_TxCancel(DRV_HANDLE drvHandle, DRV_RF215_TX_HANDLE txHandle);

// *****************************************************************************
/* Function:
    uint8_t DRV_RF215_GetPibSize(DRV_RF215_PIB_ATTRIBUTE attr)

  Summary:
    Get size of PIB attribute.

  Description:
    This routine allows to get the size in bytes of a PIB attribute of RF215
    Driver.

  Precondition:
    None.

  Parameters:
    attr - A valid RF215 Driver PIB attribute (see DRV_RF215_PIB_ATTRIBUTE).

  Returns:
    Size of PIB attribute in bytes. 0 if invalid PIB attribute.

  Example:
    <code>
    </code>

  Remarks:
    None.
*/

uint8_t DRV_RF215_GetPibSize(DRV_RF215_PIB_ATTRIBUTE attr);

// *****************************************************************************
/* Function:
    DRV_RF215_PIB_RESULT DRV_RF215_GetPib (
        DRV_HANDLE drvHandle,
        DRV_RF215_PIB_ATTRIBUTE attr,
        void* value
    )

  Summary:
    Get value of PIB attribute.

  Description:
    This routine allows a client to get the value of a PIB attribute of RF215
    Driver.

  Precondition:
    DRV_RF215_Open must have been called to obtain a valid opened driver handle.

  Parameters:
    drvHandle - A valid open-instance handle, returned from the driver's open
                routine. 
    attr      - A valid RF215 Driver PIB attribute
                (see DRV_RF215_PIB_ATTRIBUTE).
    value     - Pointer to store PIB value. The allocated memory must be same or
                equal of PIB size (see DRV_RF215_GetPibSize). 

  Returns:
    Result of PIB get.

  Example:
    <code>
    </code>

  Remarks:
    None.
*/

DRV_RF215_PIB_RESULT DRV_RF215_GetPib (
    DRV_HANDLE drvHandle,
    DRV_RF215_PIB_ATTRIBUTE attr,
    void* value
);

// *****************************************************************************
/* Function:
    DRV_RF215_PIB_RESULT DRV_RF215_SetPib (
        DRV_HANDLE drvHandle,
        DRV_RF215_PIB_ATTRIBUTE attr,
        void* value
    )

  Summary:
    Set value of PIB attribute.

  Description:
    This routine allows a client to set the value of a PIB attribute of RF215
    Driver.

  Precondition:
    DRV_RF215_Open must have been called to obtain a valid opened driver handle.

  Parameters:
    drvHandle - A valid open-instance handle, returned from the driver's open
                routine. 
    attr      - A valid RF215 Driver PIB attribute
                (see DRV_RF215_PIB_ATTRIBUTE).
    value     - Pointer to PIB value to set. The allocated memory must be same
                or equal of PIB size (see DRV_RF215_GetPibSize). 

  Returns:
    Result of PIB set.

  Example:
    <code>
    </code>

  Remarks:
    None.
*/

DRV_RF215_PIB_RESULT DRV_RF215_SetPib (
    DRV_HANDLE drvHandle,
    DRV_RF215_PIB_ATTRIBUTE attr,
    void* value
);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // #ifndef _DRV_RF215_H
