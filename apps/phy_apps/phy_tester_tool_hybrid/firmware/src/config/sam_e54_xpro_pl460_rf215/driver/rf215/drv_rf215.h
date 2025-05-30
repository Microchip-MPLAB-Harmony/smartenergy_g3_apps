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

#ifndef DRV_RF215_H
#define DRV_RF215_H

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
    registered with the driver. It can also be used to abort the transmission by
    DRV_RF215_TxCancel.

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
    request was not successful. It can happen due to invalid arguments, invalid
    driver handle, or lack of space in the queue.

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
    The availability of each transceiver is enabled via MCC.
*/

typedef enum
{
    RF215_TRX_ID_RF09  = 0,
    RF215_TRX_ID_RF24  = 1,

} DRV_RF215_TRX_ID;

// *****************************************************************************
/* RF215 Driver Ready Status Callback

  Summary:
    Pointer to a RF215 driver ready status callback function.

  Description:
    This data type defines the required function signature for the RF215 driver
    ready status event handling callback function. A client uses
    DRV_RF215_ReadyStatusCallbackRegister to register a pointer to a function
    which must match the signature (parameter and return value types) specified
    by this data type.

  Parameters:
    context - Value identifying the context of the client that registered the
              callback function.
    status  - SYS_STATUS_READY or SYS_STATUS_ERROR, depending on initialization
              result.

  Returns:
    None.

  Example:
    <code>
    static void APP_ReadyNotCb(uintptr_t ctxt, SYS_STATUS status)
    {
        DRV_HANDLE drvRf215Handle;

        MY_APP_OBJ myAppObj = (MY_APP_OBJ *) ctxt;

        if (status == SYS_STATUS_READY)
        {
            drvRf215Handle = DRV_RF215_Open(DRV_RF215_INDEX_0, RF215_TRX_ID_RF09);
        }
    }

    MY_APP_OBJ myAppObj;

    DRV_RF215_ReadyStatusCallbackRegister(APP_ReadyNotCb, (uintptr_t) myAppObj);
    </code>

  Remarks:
    The context parameter contains a handle to the client context, provided at
    the time the callback function was registered using
    DRV_RF215_ReadyStatusCallbackRegister. This context value is passed back to the
    client as parameter. It can be any value necessary to identify the client
    context or instance (such as a pointer to the client's data) of the client
    that registered the callback.
*/

typedef void ( *DRV_RF215_READY_STATUS_CALLBACK ) (uintptr_t context, SYS_STATUS status);

// *****************************************************************************
/* RF215 Driver RX Indication Callback

  Summary:
    Pointer to a RF215 driver receive indication callback function.

  Description:
    This data type defines the required function signature for the RF215 driver
    receive event handling callback function. A client uses
    DRV_RF215_RxIndCallbackRegister to register a pointer to a function which
    must match the signature (parameter and return value types) specified by
    this data type.

  Parameters:
    indObj  - Pointer to the object containing the data associated to the
              receive indication (see DRV_RF215_RX_INDICATION_OBJ).
    context - Value identifying the context of the client that registered the
              callback function.

  Returns:
    None.

  Example:
    <code>
    static void APP_RxIndCb(DRV_RF215_RX_INDICATION_OBJ* indObj, uintptr_t ctxt)
    {
        MY_APP_OBJ myAppObj = (MY_APP_OBJ *) ctxt;
    }

    DRV_HANDLE drvRf215Handle;
    MY_APP_OBJ myAppObj;

    DRV_RF215_RxIndCallbackRegister(drvRf215Handle, APP_RxIndCb,
        (uintptr_t) myAppObj);
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
    DRV_RF215_TxCfmCallbackRegister to register a pointer to a function which
    must match the signature (parameter and return value types) specified by
    this data type.

  Parameters:
    txHandle - Transmission handle returned from DRV_RF215_TxRequest.
    cfmObj   - Pointer to the object containing the data associated to the
               transmit confirm (see DRV_RF215_TX_CONFIRM_OBJ).
    context  - Value identifying the context of the application that registered
               the callback function.

  Returns:
    None.

  Example:
    <code>
    static void _APP_RF_TxCfmCb (
        DRV_RF215_TX_HANDLE txHandle,
        DRV_RF215_TX_CONFIRM_OBJ *cfmObj,
        uintptr_t ctxt
    )
    {
        MY_APP_OBJ myAppObj = (MY_APP_OBJ *) ctxt;
    }

    DRV_HANDLE drvRf215Handle;
    MY_APP_OBJ myAppObj;

    DRV_RF215_TxCfmCallbackRegister(drvRf215Handle, _APP_RF_TxCfmCb,
        (uintptr_t) myAppObj);
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
    init  - Pointer to the initialization data structure containing the data
            necessary to initialize the driver.

  Returns:
    If successful, returns a valid handle to a driver instance object.
    Otherwise, returns SYS_MODULE_OBJ_INVALID.

  Example:
    <code>
    SYS_MODULE_OBJ sysObjDrvRf215;

    const DRV_RF215_INIT drvRf215InitData = {
        .spiTransmitAddress = (const void *)&(SPI0_REGS->SPI_TDR),

        .spiReceiveAddress = (const void *)&(SPI0_REGS->SPI_RDR),

        .dmaIntSource = XDMAC_IRQn,

        .sysTimeIntSource = TC0_CH0_IRQn,

        .rf09PhyBandOpmIni = SUN_FSK_BAND_863_OPM1,

        .rf09PhyChnNumIni = 0,

    };

    sysObjDrvRf215 = DRV_RF215_Initialize(DRV_RF215_INDEX_0, (SYS_MODULE_INIT *)&drvRf215InitData);
    </code>

  Remarks:
    This routine must be called before any other RF215 Driver routine and should
    only be called once during system initialization.
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
    The DRV_RF215_Initialize routine must have been called to obtain a valid
    system object.

  Parameters:
    object - Driver object handle, returned from the DRV_RF215_Initialize
             routine.

  Returns:
    SYS_STATUS_READY         - The RF215 driver module initialization has
                               completed successfully.
    SYS_STATUS_BUSY          - The RF215 driver module initialization has not
                               completed yet.
    SYS_STATUS_ERROR         - The RF215 driver module is in an error state.
    SYS_STATUS_UNINITIALIZED - The RF215 driver module is not initialized.

  Example:
    <code>
    SYS_MODULE_OBJ sysObjDrvRf215;
    SYS_STATUS drvRf215Status;

    drvRf215Status = DRV_RF215_Status(sysObjDrvRf215);
    if (drvRf215Status == SYS_STATUS_READY)
    {

    }
    </code>

  Remarks:
    The RF215 driver can be opened (DRV_RF215_Open) only when its status is
    SYS_STATUS_READY.
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
    object - Driver object handle, returned from the DRV_RF215_Initialize
             routine.

  Returns:
    None

  Example:
    <code>
    SYS_MODULE_OBJ sysObjDrvRf215;

    while (true)
    {
        DRV_RF215_Tasks (sysObjDrvRf215);
    }
    </code>

  Remarks:
    This function is normally not called directly by an application. It is
    called by the system's tasks routine (SYS_Tasks).
*/

void DRV_RF215_Tasks( SYS_MODULE_OBJ object );

// *****************************************************************************
/* Function:
    void DRV_RF215_ReadyStatusCallbackRegister (
        const SYS_MODULE_INDEX index,
        const DRV_RF215_READY_STATUS_CALLBACK callback,
        uintptr_t context
    )

  Summary:
    Allows a client to set an event handling function for the driver to call
    back when it is ready to be opened.

  Description:
    This function allows a client to register a RF215 ready notification event
    handling function for the driver to call back when the driver is ready to be
    opened.

  Precondition:
    DRV_RF215_Initialize must have been called before.

  Parameters:
    index     - Identifier for the object instance to be opened. Only one
                instance (index 0) supported.
    callback  - Pointer to the callback function.
    context   - The value of this parameter will be passed back to the client
                unchanged, when the callback function is called.

  Returns:
    None.

  Example:
    <code>
    static void APP_ReadyNotCb(uintptr_t ctxt, SYS_STATUS status)
    {
        DRV_HANDLE drvRf215Handle;

        MY_APP_OBJ myAppObj = (MY_APP_OBJ *) ctxt;


        if (status == SYS_STATUS_READY)
        {
            drvRf215Handle = DRV_RF215_Open(DRV_RF215_INDEX_0, RF215_TRX_ID_RF09);
        }
    }

    MY_APP_OBJ myAppObj;

    DRV_RF215_ReadyStatusCallbackRegister(APP_ReadyNotCb, (uintptr_t) myAppObj);
    </code>

  Remarks:
    None.
*/

void DRV_RF215_ReadyStatusCallbackRegister (
    const SYS_MODULE_INDEX index,
    const DRV_RF215_READY_STATUS_CALLBACK callback,
    uintptr_t context
);

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
    Function DRV_RF215_Initialize must have been called and function
    DRV_RF215_Status should have returned SYS_STATUS_READY before calling this
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
    - if the number of clients allocated via MCC is insufficient.
    - if the specified instance index is not supported.
    - if the specified RF215 transceiver is not supported or disabled via MCC.
    - if the driver instance being opened is not ready to be opened (not
      initialized, initialization in progress or in error state because of wrong
      peripheral/hardware configuration).

  Example:
    <code>
    SYS_MODULE_OBJ sysObjDrvRf215;
    SYS_STATUS drvRf215Status;
    DRV_HANDLE drvRf215Handle;

    drvRf215Status = DRV_RF215_Status(sysObjDrvRf215);
    if (drvRf215Status == SYS_STATUS_READY)
    {
        drvRf215Handle = DRV_RF215_Open(DRV_RF215_INDEX_0, RF215_TRX_ID_RF09);
        if (drvRf215Handle != DRV_HANDLE_INVALID)
        {

        }
    }
    </code>

  Remarks:
    The handle returned is valid until the DRV_RF215_Close routine is called.
*/

DRV_HANDLE DRV_RF215_Open (
    const SYS_MODULE_INDEX index,
    const DRV_RF215_TRX_ID trxID
);

// *****************************************************************************
/* Function:
    void DRV_RF215_Close( const DRV_HANDLE drvHandle )

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
    DRV_RF215_Close(drvHandle);
    </code>

  Remarks:
    None.
*/

void DRV_RF215_Close( const DRV_HANDLE drvHandle );

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

    The callback should be registered immediately after opening the driver.
    Before this callback is set, any PPDU received by the transceiver will not
    be notified.

  Precondition:
    DRV_RF215_Open must have been called to obtain a valid opened driver handle.

  Parameters:
    drvHandle - A valid open-instance handle, returned from the driver's open
                routine.
    callback  - Pointer to the callback function.
    context   - The value of this parameter will be passed back to the client
                unchanged, when the callback function is called.

  Returns:
    None.

  Example:
    <code>
    static void APP_RxIndCb(DRV_RF215_RX_INDICATION_OBJ* indObj, uintptr_t ctxt)
    {
        MY_APP_OBJ myAppObj = (MY_APP_OBJ *) ctxt;
    }

    SYS_MODULE_OBJ sysObjDrvRf215;
    SYS_STATUS drvRf215Status;
    DRV_HANDLE drvRf215Handle;
    MY_APP_OBJ myAppObj;

    drvRf215Status = DRV_RF215_Status(sysObjDrvRf215);
    if (drvRf215Status == SYS_STATUS_READY)
    {
        drvRf215Handle = DRV_RF215_Open(DRV_RF215_INDEX_0, RF215_TRX_ID_RF09);
        if (drvRf215Handle != DRV_HANDLE_INVALID)
        {
            DRV_RF215_RxIndCallbackRegister(drvRf215Handle, APP_RxIndCb,
                (uintptr_t) myAppObj);
        }
    }
    </code>

  Remarks:
    The callback once set, persists until the client closes the driver or sets
    another callback (which could be a "NULL" pointer to indicate no callback).
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

    The callback should be registered immediately after opening the driver.
    Before this callback is set, any transmission confirmation will not be
    notified.

  Precondition:
    DRV_RF215_Open must have been called to obtain a valid opened driver handle.

  Parameters:
    drvHandle - A valid open-instance handle, returned from the driver's open
                routine.
    callback  - Pointer to the callback function.
    context   - The value of this parameter will be passed back to the client
                unchanged, when the callback function is called.

  Returns:
    None.

  Example:
    <code>
    static void _APP_RF_TxCfmCb (
        DRV_RF215_TX_HANDLE txHandle,
        DRV_RF215_TX_CONFIRM_OBJ *cfmObj,
        uintptr_t ctxt
    )
    {
        MY_APP_OBJ myAppObj = (MY_APP_OBJ *) ctxt;
    }

    SYS_MODULE_OBJ sysObjDrvRf215;
    SYS_STATUS drvRf215Status;
    DRV_HANDLE drvRf215Handle;
    MY_APP_OBJ myAppObj;

    drvRf215Status = DRV_RF215_Status(sysObjDrvRf215);
    if (drvRf215Status == SYS_STATUS_READY)
    {
        drvRf215Handle = DRV_RF215_Open(DRV_RF215_INDEX_0, RF215_TRX_ID_RF09);
        if (drvRf215Handle != DRV_HANDLE_INVALID)
        {
            DRV_RF215_TxCfmCallbackRegister(drvRf215Handle, _APP_RF_TxCfmCb,
                (uintptr_t) myAppObj);
        }
    }
    </code>

  Remarks:
    The callback once set, persists until the client closes the driver or sets
    another callback (which could be a "NULL" pointer to indicate no callback).
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
    (successfully or not), it is notified via TX confirm callback.

    If an error occurs in the TX request, the return value is
    DRV_RF215_TX_HANDLE_INVALID. In this case the error type will be reported
    by the result parameter. Error can occur
    - if the number of TX buffer objects allocated via MCC is insufficient.
    - if the specified driver handle is not valid.
    - if transmission request parameters are not valid.

  Example:
    <code>
    DRV_HANDLE drvRf215Handle;
    DRV_RF215_TX_REQUEST_OBJ txReqObj;
    DRV_RF215_TX_RESULT txReqResult;
    DRV_RF215_TX_HANDLE txReqHandle;
    uint8_t psduTx[DRV_RF215_MAX_PSDU_LEN];

    txReqObj.cancelByRx = false;
    txReqObj.ccaMode = PHY_CCA_MODE_3;
    txReqObj.modScheme = FSK_FEC_OFF;
    txReqObj.txPwrAtt = 0;
    txReqObj.psduLen = DRV_RF215_MAX_PSDU_LEN;
    txReqObj.timeMode = TX_TIME_RELATIVE;
    txReqObj.timeCount = 0;
    txReqObj.psdu = psduTx;

    txReqHandle = DRV_RF215_TxRequest(drvRf215Handle, &txReqObj, &txReqResult);
    if (txReqHandle != DRV_RF215_TX_HANDLE_INVALID)
    {

    }
    </code>

  Remarks:
    The handle returned is valid until the TX confirm routine is called back or
    canceled by DRV_RF215_TxCancel function.
    This is a non-blocking routine.
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
    Allows a client to cancel a previously requested transmission.

  Description:
    This routine allows a client to cancel a previously requested transmission.

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
    DRV_HANDLE drvRf215Handle;
    DRV_RF215_TX_HANDLE txReqHandle;

    DRV_RF215_TxCancel(drvRf215Handle, txReqHandle);
    </code>

  Remarks:
    If transmission has already started it will be aborted before completion.
*/

void DRV_RF215_TxCancel(DRV_HANDLE drvHandle, DRV_RF215_TX_HANDLE txHandle);

// *****************************************************************************
/* Function:
    uint8_t DRV_RF215_GetPibSize(DRV_RF215_PIB_ATTRIBUTE attr)

  Summary:
    Gets size of PIB attribute.

  Description:
    This routine allows to get the size in bytes of a PIB (PHY Information Base)
    attribute of the RF215 Driver.

  Precondition:
    None.

  Parameters:
    attr - A valid RF215 Driver PIB attribute (see DRV_RF215_PIB_ATTRIBUTE).

  Returns:
    Size of PIB attribute in bytes. 0 if invalid PIB attribute.

  Example:
    <code>
    uint8_t pibSize;

    pibSize = DRV_RF215_GetPibSize(RF215_PIB_PHY_BAND_OPERATING_MODE);
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
    Gets value of PIB attribute.

  Description:
    This routine allows a client to get the value of a PIB (PHY Information
    Base) attribute of the RF215 Driver.

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
    Result of getting the PIB (see DRV_RF215_PIB_RESULT).

  Example:
    <code>
    DRV_HANDLE drvRf215Handle;
    DRV_RF215_PHY_BAND_OPM phyBandOpm;
    DRV_RF215_PIB_RESULT pibResult;

    pibResult = DRV_RF215_GetPib(drvRf215Handle,
        RF215_PIB_PHY_BAND_OPERATING_MODE, &phyBandOpm);

    if (pibResult == RF215_PIB_RESULT_SUCCESS)
    {

    }
    </code>

  Remarks:
    If dual-band is used, the PIB values are different for each transceiver.
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
    Sets value of PIB attribute.

  Description:
    This routine allows a client to set the value of a PIB (PHY Information
    Base) attribute of RF215 Driver.

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
    Result of setting the PIB (see DRV_RF215_PIB_RESULT).

  Example:
    <code>
    DRV_HANDLE drvRf215Handle;
    DRV_RF215_PHY_BAND_OPM phyBandOpm = SUN_FSK_BAND_863_OPM1;
    DRV_RF215_PIB_RESULT pibResult;

    pibResult = DRV_RF215_SetPib(drvRf215Handle,
        RF215_PIB_PHY_BAND_OPERATING_MODE, &phyBandOpm);

    if (pibResult == RF215_PIB_RESULT_SUCCESS)
    {

    }
    </code>

  Remarks:
    If dual-band is used, the PIB values are different for each transceiver.
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

#endif // #ifndef DRV_RF215_H
