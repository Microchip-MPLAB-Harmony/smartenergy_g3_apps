/*******************************************************************************
  DRV_PLC_PHY Driver Interface Definition

  Company:
    Microchip Technology Inc.

  File Name:
    drv_plc_phy.h

  Summary:
    PLC Library Interface header.

  Description:
    The PLC Driver Library provides a interface to access the PLC transceiver.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*
Copyright (C) 2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#ifndef DRV_PLC_PHY_H
#define DRV_PLC_PHY_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdio.h>
#include <stdbool.h>
#include "driver/driver.h"
#include "system/system.h"
#include "drv_plc_phy_comm.h"
#include "drv_plc_phy_definitions.h"
#include "driver/plc/common/drv_plc_hal.h"
#include "driver/plc/common/drv_plc_boot.h"

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
/* DRV_PLC_PHY Transfer Errors

 Summary:
    Defines the data type for PLC Driver transfer errors.

 Description:
    This will be used to indicate the error of the last SPI transfer.

 Remarks:
    None.
*/

typedef enum
{
    /* SPI has detected an unexpected status, reset is recommended */
    DRV_PLC_PHY_EXCEPTION_UNEXPECTED_KEY,

    /* SPI critical error */
    DRV_PLC_PHY_EXCEPTION_CRITICAL_ERROR,

    /* Device has been reseted by Debugging tool */
    DRV_PLC_PHY_EXCEPTION_DEBUG,

    /* Device has been reseted */
    DRV_PLC_PHY_EXCEPTION_RESET

} DRV_PLC_PHY_EXCEPTION;

// *****************************************************************************
/* PLC Driver Transmission Confirm Event Handler Function Pointer

  Summary:
    Pointer to a PLC Driver Transmission Confirm Event handler function.

  Description:
    This data type defines the required function signature for the PLC driver
    transmission confirm event handling callback function. A client must
    register a pointer using the callback register function whose function
    signature (parameter and return value types) match the types specified by
    this function pointer in order to receive transfer related event calls back
    from the driver.

    The parameters and return values are described here and a partial example
    implementation is provided.

  Parameters:
    cfmObj  - Pointer to the object containing any data necessary to identify
              the result of the last transmission.

    context - Value identifying the context of the application that registered
              the event handling function.

  Returns:
    None.

  Example:
    <code>
    void APP_MyTXCfmEventHandler( DRV_PLC_PHY_TRANSMISSION_CFM_OBJ *cfmObj,
                                   uintptr_t context )
    {
        MY_APP_DATA_STRUCT* pAppData = (MY_APP_DATA_STRUCT*) context;

        switch(cfmObj->result)
        {
            case DRV_PLC_PHY_TX_RESULT_PROCESS:
                break;
            case DRV_PLC_PHY_TX_RESULT_SUCCESS:
                break;
            case DRV_PLC_PHY_TX_RESULT_INV_LENGTH:
                break;
            case DRV_PLC_PHY_TX_RESULT_BUSY_CH:
                break;
            case DRV_PLC_PHY_TX_RESULT_BUSY_TX:
                break;
            case DRV_PLC_PHY_TX_RESULT_BUSY_RX:
                break;
            case DRV_PLC_PHY_TX_RESULT_INV_SCHEME:
                break;
            case DRV_PLC_PHY_TX_RESULT_TIMEOUT:
                break;
            case DRV_PLC_PHY_TX_RESULT_INV_TONEMAP:
                break;
            case DRV_PLC_PHY_TX_RESULT_INV_MODE:
                break;
            case DRV_PLC_PHY_TX_RESULT_NO_TX:
                break;
        }
    }
    </code>

  Remarks:
    - If the result field is DRV_PLC_PHY_TX_RESULT_SUCCESS, it means that the data was
      transferred successfully.

    - Otherwise, it means that the data was not transferred successfully.

    - The context parameter contains a handle to the client context, provided at
      the time the event handling function was registered using the
      DRV_PLC_PHY_TxCfmCallbackRegister function. This context handle value is
      passed back to the client as the "context" parameter. It can be any value
      necessary to identify the client context or instance (such as a pointer to
      the client's data) of the client that made the transfer add request.

*/

typedef void ( *DRV_PLC_PHY_TX_CFM_CALLBACK )( DRV_PLC_PHY_TRANSMISSION_CFM_OBJ *cfmObj, uintptr_t context );

// *****************************************************************************
/* PLC Driver Reception Event Handler Function Pointer

  Summary:
    Pointer to a PLC Driver Reception Event handler function.

  Description:
    This data type defines the required function signature for the PLC driver
    reception event handling callback function. A client must register a pointer
    using the callback register function whose function signature (parameter
    and return value types) match the types specified by this function pointer
    in order to receive transfer related event calls back from the driver.

    The parameters and return values are described here and a partial example
    implementation is provided.

  Parameters:
    indObj  - Pointer to the object containing any data necessary to describe
              the new received message.

    context - Value identifying the context of the application that registered
              the event handling function.

  Returns:
    None.

  Example:
    <code>
    void APP_MyRxEventHandler( DRV_PLC_PHY_RECEPTION_OBJ *indObj,
                                   uintptr_t context )
    {
        MY_APP_DATA_STRUCT* pAppData = (MY_APP_DATA_STRUCT*) context;

        if (indObj->dataLength != 0)
        {
            memcpy(appData->pDataRx, indObj->pReceivedData, indObj->dataLength);
        }
    </code>

  Remarks:
    - Check DRV_PLC_PHY_RECEPTION_OBJ struct to identify other intereseting
      parameters about reception such as LQI, Tonemap, Modulation scheme, etc.

    - The context parameter contains a handle to the client context, provided at
      the time the event handling function was registered using the
      DRV_PLC_PHY_DataIndCallbackRegister function. This context handle value
      is passed back to the client as the "context" parameter. It can be any
      value necessary to identify the client context or instance (such as a
      pointer to the client's data) of the client that made the transfer add
      request.

*/

typedef void ( *DRV_PLC_PHY_DATA_IND_CALLBACK )( DRV_PLC_PHY_RECEPTION_OBJ *indObj, uintptr_t context );

// *****************************************************************************
/* PLC Driver Exceptions Event Handler Function Pointer

  Summary:
    Pointer to a PLC Driver Exceptions Event handler function.

  Description:
    This data type defines the required function signature for the PLC driver
    exceptions event handling callback function. A client must register a
    pointer using the callback register function whose function signature
    (parameter and return value types) match the types specified by this
    function pointer in order to receive transfer related event calls back from
    the driver.

    The parameters and return values are described here and a partial example
    implementation is provided.

  Parameters:
    exception - Value identifying the exception code which occurs during SPI
                transfer.

    context   - Value identifying the context of the application that registered
                the event handling function.

  Returns:
    None.

  Example:
    <code>
    void APP_MyExceptionEventHandler( DRV_PLC_PHY_EXCEPTION exception,
                                   uintptr_t context )
    {
        MY_APP_DATA_STRUCT* pAppData = (MY_APP_DATA_STRUCT*) context;

        switch (exceptionObj)
        {
            case DRV_PLC_PHY_EXCEPTION_UNEXPECTED_KEY:
                break;

            case DRV_PLC_PHY_EXCEPTION_CRITICAL_ERROR:
                break;

            case DRV_PLC_PHY_EXCEPTION_DEBUG:
                break;

            case DRV_PLC_PHY_EXCEPTION_RESET:
                break;

            default:
                break;
        }

        appData.plc_phy_exception = true;
    }
    </code>

  Remarks:
    - The context parameter contains a handle to the client context, provided
      at the time the event handling function was registered using the
      DRV_PLC_PHY_ExceptionCallbackRegister function. This context handle value is
      passed back to the client as the "context" parameter. It can be any value
      necessary to identify the client context or instance (such as a pointer to
      the client's data) of the client that made the transfer add request.

*/

typedef void ( *DRV_PLC_PHY_EXCEPTION_CALLBACK )( DRV_PLC_PHY_EXCEPTION exception, uintptr_t context );

// *****************************************************************************
/* PLC Driver Sleep Mode Disable Event Handler Function Pointer

  Summary:
    Notifies when SLeep Mode is disabled and PLC driver is able to be used
    again.

  Description:
    This data type defines the required function signature for the PLC driver
    sleep mode disable event handling callback function. A client must register
    a pointer using the callback register function whose function signature
    (parameter and return value types) match the types specified by this
    function pointer in order to receive transfer related event calls back from
    the driver.

    The parameters and return values are described here and a partial example
    implementation is provided.

  Parameters:
    context - Value identifying the context of the application that registered
              the event handling function.

  Returns:
    None.

  Example:
    <code>
    void APP_MySleepDisableEventHandler( uintptr_t context )
    {
        MY_APP_DATA_STRUCT* appData = (MY_APP_DATA_STRUCT*) context;

        appData.plc_sleep = false;
    }
    </code>

  Remarks:
    - The context parameter contains the a handle to the client context,
      provided at the time the event handling function was registered using the
      DRV_PLC_PHY_SleepDisableCallbackRegister function. This context handle
      value is passed back to the client as the "context" parameter. It can be
      any value necessary to identify the client context or instance (such as a
      pointer to the client's data) of the client that made the transfer add
      request.

*/

typedef void ( *DRV_PLC_PHY_SLEEP_CALLBACK )( uintptr_t context );

// *****************************************************************************
// *****************************************************************************
// Section: DRV_PLC_PHY Driver System Interface Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ DRV_PLC_PHY_Initialize(
        const SYS_MODULE_INDEX index,
        const SYS_MODULE_INIT * const init
    )

  Summary:
    Initializes the PLC instance for the specified driver index.

  Description:
    This routine initializes the PLC transceiver driver making it ready for
    clients to open and use. The initialization data is specified by the init
    parameter. It is a single instance driver, so this API should be called
    only once.

  Precondition:
    None.

  Parameters:
    index - Identifier for the instance to be initialized (single instance
            allowed).

    init  - Pointer to the init data structure containing any data necessary to
            initialize the driver.

  Returns:
    If successful, returns a valid handle to a driver instance object.
    Otherwise, returns SYS_MODULE_OBJ_INVALID.

  Example:
    <code>
    SYS_MODULE_OBJ   sysObjDrvPLC;

    DRV_PLC_PLIB_INTERFACE drvPLCPlib = {
        .spiPlibTransferSetup = (DRV_PLC_SPI_PLIB_TRANSFER_SETUP)SPI0_TransferSetup,
        .dmaChannelTx = SYS_DMA_CHANNEL_1,
        .dmaChannelRx  = SYS_DMA_CHANNEL_0,
        .spiAddressTx =  (void *)&(SPI0_REGS->SPI_TDR),
        .spiAddressRx  = (void *)&(SPI0_REGS->SPI_RDR),
        .spiClockFrequency = DRV_PLC_SPI_CLK,
        .ldoPin = DRV_PLC_LDO_EN_PIN,
        .resetPin = DRV_PLC_RESET_PIN,
        .extIntPin = DRV_PLC_EXT_INT_PIN,
        .txEnablePin = DRV_PLC_TX_ENABLE_PIN,
        .stByPin = DRV_PLC_STBY_PIN,
        .thMonPin = DRV_PLC_THMON_PIN,
    };

    DRV_PLC_HAL_INTERFACE drvPLCHalAPI = {
        .plcPlib = &drvPLCPlib,
        .init = (DRV_PLC_HAL_INIT)DRV_PLC_HAL_Init,
        .setup = (DRV_PLC_HAL_SETUP)DRV_PLC_HAL_Setup,
        .reset = (DRV_PLC_HAL_RESET)DRV_PLC_HAL_Reset,
        .setStandBy = (DRV_PLC_HAL_SET_STBY)DRV_PLC_HAL_SetStandBy,
        .getThermalMonitor = (DRV_PLC_HAL_GET_THMON)DRV_PLC_HAL_GetThermalMonitor,
        .setTxEnable = (DRV_PLC_HAL_SET_TXENABLE)DRV_PLC_HAL_SetTxEnable,
        .enableExtInt = (DRV_PLC_HAL_ENABLE_EXT_INT)DRV_PLC_HAL_EnableInterrupts,
        .delay = (DRV_PLC_HAL_DELAY)DRV_PLC_HAL_Delay,
        .sendBootCmd = (DRV_PLC_HAL_SEND_BOOT_CMD)DRV_PLC_HAL_SendBootCmd,
        .sendWrRdCmd = (DRV_PLC_HAL_SEND_WRRD_CMD)DRV_PLC_HAL_SendWrRdCmd,
    };

    extern uint8_t plc_phy_bin_start;
    extern uint8_t plc_phy_bin_end;

    DRV_PLC_PHY_INIT drvPlcPhyInitData = {
        .plcHal = &drvPLCHalAPI,
        .numClients = DRV_PLC_PHY_CLIENTS_NUMBER_IDX,
        .plcProfile = DRV_PLC_PHY_PROFILE,
        .binStartAddress = (uint32_t)&plc_phy_bin_start,
        .binEndAddress = (uint32_t)&plc_phy_bin_end,
        .secure = DRV_PLC_SECURE,
    };

    sysObjDrvPLC0 = DRV_PLC_PHY_Initialize(DRV_PLC_PHY_INDEX_0, (SYS_MODULE_INIT *)&drvPLC0InitData);
    PIO_PinInterruptCallbackRegister(DRV_PLC_EXT_INT_PIN, DRV_PLC_PHY_ExternalInterruptHandler, sysObjDrvPLC0);
    </code>

  Remarks:
    This routine must be called before any other DRV_PLC_PHY routine is called.
    This routine should only be called once during system initialization.
    This routine will block for hardware access.
*/

SYS_MODULE_OBJ DRV_PLC_PHY_Initialize( const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init);

// *****************************************************************************
/* Function:
    DRV_HANDLE DRV_PLC_PHY_Open
    (
        const SYS_MODULE_INDEX index,
        const DRV_PLC_BOOT_DATA_CALLBACK callback
    )

  Summary:
    Opens the specified PLC driver instance and returns a handle to it.

  Description:
    This routine opens the specified PLC driver instance and provides a handle
    that must be provided to all other client-level operations to identify the
    caller and the instance of the driver.

    This driver is a single client driver, so DRV_PLC_PHY_Open API should be
    called only once until driver is closed.

  Precondition:
    Function DRV_PLC_PHY_Initialize must have been called before calling this
    function.

  Parameters:
    index    - Identifier for the object instance to be opened.

    callback - Boot Data Callback Function Pointer. In case of use NULL,
               .binStartAddress and .binEndAddress fields must be configured in
               initialization data DRV_PLC_PHY_INIT.

  Returns:
    If successful, the routine returns a valid open-instance handle (a number
    identifying both the caller and the module instance).

    If an error occurs, the return value is DRV_HANDLE_INVALID. Error can occur
    - if the  driver has been already opened once and in use.
    - if the driver peripheral instance being opened is not initialized or is
      invalid.

  Example:
    <code>
    DRV_HANDLE handle;

    handle = DRV_PLC_PHY_Open(DRV_PLC_PHY_INDEX_0, NULL);
    if (handle == DRV_HANDLE_INVALID)
    {

    }
    </code>

  Remarks:
    The handle returned is valid until the DRV_PLC_PHY_Close routine is called.
    This routine will NEVER block waiting for hardware.
*/
DRV_HANDLE DRV_PLC_PHY_Open(const SYS_MODULE_INDEX index,
        const DRV_PLC_BOOT_DATA_CALLBACK callback);

// *****************************************************************************
/* Function:
    void DRV_PLC_PHY_Close( const DRV_Handle handle )

  Summary:
    Closes opened-instance of the PLC driver.

  Description:
    This routine closes opened-instance of the PLC driver, invalidating the
    handle. A new handle must be obtained by calling DRV_PLC_PHY_Open
    before the caller may use the driver again.

  Precondition:
    DRV_PLC_PHY_Open must have been called to obtain a valid opened device
    handle.

  Parameters:
    handle - A valid open-instance handle, returned from the driver's open
             routine.

  Returns:
    None.

  Example:
    <code>
    DRV_PLC_PHY_Close(handle);
    </code>

  Remarks:
    None.
*/
void DRV_PLC_PHY_Close(const DRV_HANDLE handle);

// *****************************************************************************
/* Function:
    void DRV_PLC_PHY_TxRequest( DRV_Handle handle, DRV_PLC_PHY_TRANSMISSION_OBJ *transmitObj )

  Summary:
    Allows a client to transmit data through Power Line (PLC).

  Description:
    This routine sends a new data message through PLC using the PLC driver.

  Precondition:
    DRV_PLC_PHY_Open must have been called to obtain a valid opened device
    handle.
    DRV_PLC_PHY_TRANSMISSION_OBJ must be configured before requesting the
    transmission.

  Parameters:
    handle      - A valid open-instance handle, returned from the driver's open
                  routine.
    transmitObj - Pointer to the transmission parameters and data.

  Returns:
    None.

  Example:
    <code>
    DRV_PLC_PHY_TRANSMISSION_OBJ transmitObj;

    _setupTransmissionParameters();

    memcpy(transmitObj.data, txBuf, dataLen);
    transmitObj.length = dataLen;

    DRV_PLC_PHY_TxRequest(handle, &transmitObj);
    </code>

  Remarks:
    None.
*/
void DRV_PLC_PHY_TxRequest(const DRV_HANDLE handle, DRV_PLC_PHY_TRANSMISSION_OBJ *transmitObj);

// *****************************************************************************
/* Function:
    void DRV_PLC_PHY_PIBGet( const DRV_HANDLE handle, DRV_PLC_PHY_PIB_OBJ *pibObj )

  Summary:
    Allows a client to get information from PLC transceiver about PLC
    information base (PIB).

  Description:
    This routine gets PLC data information from the PLC transceiver.

  Precondition:
    DRV_PLC_PHY_Open must have been called to obtain a valid opened device
    handle.
    DRV_PLC_PHY_PIB_OBJ must be configured before getting information.

  Parameters:
    handle - A valid open-instance handle, returned from the driver's open
             routine.
    pibObj - Pointer to the PIB object to get.

  Returns:
    None.

  Example:
    <code>
    DRV_PLC_PHY_PIB_OBJ pibObj;
    uint32_t version;

    pibObj.pData = &version;
    pibObj.length = 4;
    pibObj.id = PLC_ID_VERSION_NUM;

    DRV_PLC_PHY_PIBGet(handle, &pibObj);
    </code>

  Remarks:
    None.
*/
bool DRV_PLC_PHY_PIBGet(const DRV_HANDLE handle, DRV_PLC_PHY_PIB_OBJ *pibObj);

// *****************************************************************************
/* Function:
    void DRV_PLC_PHY_PIBSet( const DRV_HANDLE handle, DRV_PLC_PHY_PIB_OBJ *pibObj )

  Summary:
    Allows a client to set information to PLC transceiver about PLC information
    base (PIB).

  Description:
    This routine sets PLC data information to the PLC transceiver.

  Precondition:
    DRV_PLC_PHY_Open must have been called to obtain a valid opened device
    handle.
    DRV_PLC_PHY_PIB_OBJ must be configured before setting information.

  Parameters:
    handle - A valid open-instance handle, returned from the driver's open
             routine.
    pibObj - Pointer to the PIB object to set.

  Returns:
    None.

  Example:
    <code>
    DRV_PLC_PHY_PIB_OBJ pibObj;
    uint8_t autoMode
    uint8_t impedance;

    autoMode = 0;

    impedance = VLO_STATE;

    pibObj.pData = &autoMode;
    pibObj.length = 1;
    pibObj.id = PLC_ID_CFG_AUTODETECT_IMPEDANCE;
    DRV_PLC_PHY_PIBSet(handle, &pibObj);

    pibObj.pData = &impedance;
    pibObj.length = 1;
    pibObj.id = PLC_ID_CFG_IMPEDANCE;
    DRV_PLC_PHY_PIBSet(handle, &pibObj);
    </code>

  Remarks:
    None.
*/
bool DRV_PLC_PHY_PIBSet(const DRV_HANDLE handle, DRV_PLC_PHY_PIB_OBJ *pibObj);

// *****************************************************************************
/* Function:
    void DRV_PLC_PHY_TxCfmCallbackRegister(
        const DRV_HANDLE handle,
        const DRV_PLC_PHY_TX_CFM_CALLBACK callback,
        const uintptr_t context
    );

  Summary:
    Allows a client to set a data confirm event handling function for the driver
    to call back when the requested transmission has finished.

  Description:
    This function allows a client to register a PLC data confirm event handling
    function for the driver to call back when a data confirmation PLC event
    occurs.

    The event handler should be set before the client submits any transmission
    requests that could generate events. The callback once set, persists until
    the client closes the driver or sets another callback (which could be a
    "NULL" pointer to indicate no callback).

  Precondition:
    DRV_PLC_PHY_Open must have been called to obtain a valid opened device
    handle.

  Parameters:
    handle   - A valid open-instance handle, returned from the driver's open
               routine.

    callback - Pointer to the callback function.

    context - The value of this parameter will be passed back to the client
              unchanged, when the callback function is called.

  Returns:
    None.

  Example:
    <code>
    void APP_PLC_Data_Cfm_callback(DRV_PLC_PHY_DATA_CFM_OBJ *cfmObj, uintptr_t context)
    {
        MY_APP_OBJ* myAppObj = (MY_APP_OBJ *) context;

        if (cfmObj->result == DRV_PLC_PHY_TX_RESULT_PROCESS)
        {

        }
        else
        {

        }
    }

    MY_APP_OBJ myAppObj;

    DRV_PLC_PHY_TRANSMISSION_OBJ myTransmissionObj;

    DRV_PLC_PHY_TxCfmCallbackRegister( myHandle, APP_PLC_Data_Cfm_callback, (uintptr_t)&myAppObj );

    DRV_PLC_PHY_TxRequest(myHandle, myTransmissionObj);
    </code>

*/

void DRV_PLC_PHY_TxCfmCallbackRegister(
    const DRV_HANDLE handle,
    const DRV_PLC_PHY_TX_CFM_CALLBACK callback,
    const uintptr_t context
);

// *****************************************************************************
/* Function:
    void DRV_PLC_PHY_DataIndCallbackRegister(
        const DRV_HANDLE handle,
        const DRV_PLC_PHY_DATA_IND_CALLBACK callback,
        const uintptr_t context
    );

  Summary:
    Allows a client to set a data indication event handling function for the
    driver to call back when a data reception has finished.

  Description:
    This function allows a client to register a PLC data indication event
    handling function for the driver to call back when a data reception PLC
    event occurs.

    Before this callback is set, any received frame by the PLC transceiver will
    not be notified. The callback once set, persists until the client closes the
    driver or sets another callback (which could be a "NULL" pointer to indicate
    no callback).

  Precondition:
    DRV_PLC_PHY_Open must have been called to obtain a valid opened device
    handle.

  Parameters:
    handle   - A valid open-instance handle, returned from the driver's open
               routine.

    callback - Pointer to the callback function.

    context - The value of this parameter will be passed back to the client
              unchanged, when the callback function is called.

  Returns:
    None.

  Example:
    <code>
    void APP_PLC_Data_Ind_callback(DRV_PLC_PHY_DATA_IND_OBJ *indObj, uintptr_t context)
    {
        MY_APP_OBJ* myAppObj = (MY_APP_OBJ *) context;

    }

    MY_APP_OBJ myAppObj;

    DRV_PLC_PHY_DataIndCallbackRegister( myHandle, APP_PLC_Data_Ind_callback, (uintptr_t)&myAppObj );
    </code>

*/

void DRV_PLC_PHY_DataIndCallbackRegister(
    const DRV_HANDLE handle,
    const DRV_PLC_PHY_DATA_IND_CALLBACK callback,
    const uintptr_t context
);

// *****************************************************************************
/* Function:
    void DRV_PLC_PHY_ExceptionCallbackRegister(
        const DRV_HANDLE handle,
        const DRV_PLC_PHY_EXCEPTION_CALLBACK callback,
        const uintptr_t context
    );

  Summary:
    Allows a client to set an exception event handling function for the driver
    to call back when some error occurs through PLC transceiver communication.

  Description:
    This function allows a client to register a PLC exception event handling
    function for the driver to call back when a communication SPI error occurs.

    The event handler should be set before using the PLC transceiver in order
    to capture error events. The callback once set, persists until the client
    closes the driver or sets another callback (which could be a "NULL" pointer
    to indicate no callback).

  Precondition:
    DRV_PLC_PHY_Open must have been called to obtain a valid opened device
    handle.

  Parameters:
    handle   - A valid open-instance handle, returned from the driver's open
               routine.

    callback - Pointer to the callback function.

    context - The value of this parameter will be passed back to the client
              unchanged, when the callback function is called.

  Returns:
    None.

  Example:
    <code>
    void APP_PLC_Exception_callback(DRV_PLC_PHY_EXCEPTION_OBJ *exceptionObj, uintptr_t context)
    {
        MY_APP_OBJ* myAppObj = (MY_APP_OBJ *) context;

    }

    MY_APP_OBJ myAppObj;

    DRV_PLC_PHY_ExceptionCallbackRegister( myHandle, APP_PLC_Exception_callback, (uintptr_t)&myAppObj );
    </code>

*/

void DRV_PLC_PHY_ExceptionCallbackRegister(
    const DRV_HANDLE handle,
    const DRV_PLC_PHY_EXCEPTION_CALLBACK callback,
    const uintptr_t context
);

// *****************************************************************************
/* Function:
    void DRV_PLC_PHY_SleepDisableCallbackRegister(
        const DRV_HANDLE handle,
        const DRV_PLC_PHY_SLEEP_CALLBACK callback,
        const uintptr_t context
    );

  Summary:
    Allows a client to set a sleep mode disable event handling function for the
    driver to call back when the PLC driver is active again.

  Description:
    This function allows a client to register a PLC sleep mode disable event
    handling function for the driver to call back when sleep mode is disabled
    and PLC driver has been restarted.

    The event handler should be set before the client submits a wake-up request
    using the DRV_PLC_PHY_Sleep function. The callback once set, persists until
    the client closes the driver or sets another callback (which could be a
    "NULL" pointer to indicate no callback).

  Precondition:
    DRV_PLC_PHY_Open must have been called to obtain a valid opened device
    handle.

  Parameters:
    handle   - A valid open-instance handle, returned from the driver's open
               routine.

    callback - Pointer to the callback function.

    context - The value of this parameter will be passed back to the client
              unchanged, when the callback function is called.

  Returns:
    None.

  Example:
    <code>
    void APP_PLC_SleepModeDisableCb( uintptr_t context )
    {
        MY_APP_OBJ* myAppObj = (MY_APP_OBJ *) context;

    }

    MY_APP_OBJ myAppObj;

    DRV_PLC_PHY_SleepDisableCallbackRegister( myHandle, APP_PLC_SleepModeDisableCb, (uintptr_t)&myAppObj );
    </code>

*/

void DRV_PLC_PHY_SleepDisableCallbackRegister(
    const DRV_HANDLE handle,
    const DRV_PLC_PHY_SLEEP_CALLBACK callback,
    const uintptr_t context
);

// *****************************************************************************
/* Function:
    void DRV_PLC_PHY_ExternalInterruptHandler(
        const PIO_PIN pin,
        const uintptr_t context
    );

  Summary:
    Allows application to register callback for PLC Interrupt pin.

  Description:
    This function allows a client to register a callback function to handle
    PLC interrupt.

  Precondition:
    DRV_PLC_PHY_Initialize must have been called before calling this function.

  Parameters:
    pin     - PIO pin where interrupt source is connected.

    context - Pointer to parameters to be passed to Handler function.

  Returns:
    None.

  Remarks:
    See plib_pio.h for more details.

  Example:
    <code>
    sysObj.drvPLC = DRV_PLC_PHY_Initialize(DRV_PLC_PHY_INDEX, (SYS_MODULE_INIT *)&drvPlcPhyInitData);
    PIO_PinInterruptCallbackRegister(DRV_PLC_EXT_INT_PIN, DRV_PLC_PHY_ExternalInterruptHandler, sysObj.drvPLC);
    </code>

*/

void DRV_PLC_PHY_ExternalInterruptHandler( PIO_PIN pin, uintptr_t context );

// *************************************************************************
/* Function:
    SYS_STATUS DRV_PLC_PHY_Status( const SYS_MODULE_INDEX index )

  Summary:
    Gets the current status of the PLC driver module.

  Description:
    This routine provides the current status of the PLC driver module.

  Preconditions:
    Function DRV_PLC_PHY_Initialize should have been called before calling
    this function.

  Parameters:
    index - Identifier for the instance used to initialize driver.

  Returns:
    SYS_STATUS_READY: Indicates that the driver is ready and accept
    requests for new operations.

    SYS_STATUS_UNINITIALIZED: Indicates the driver is not initialized.

    SYS_STATUS_ERROR: Indicates the driver is not initialized correctly.

    SYS_STATUS_BUSY: Indicates the driver is initializing.

  Example:
    <code>
    SYS_STATUS status;

    status = DRV_PLC_PHY_Status(DRV_PLC_PHY_INDEX);
    </code>

  Remarks:
    None.
*/

SYS_STATUS DRV_PLC_PHY_Status( const SYS_MODULE_INDEX index );

/***************************************************************************
  Function:
    void DRV_PLC_PHY_Tasks( SYS_MODULE_OBJ object )

  Summary:
    Maintains the driver's state machine.

  Description:
    This function is used to maintain the PLC driver's internal state
    machine.

  Precondition:
    The DRV_PLC_PHY_Initialize routine must have been called to obtain a
    valid system object handle.

  Parameters:
    object - Object handle for the specified driver instance (returned from
             DRV_PLC_PHY_Initialize)
  Returns:
    None.

  Example:
    <code>
    SYS_MODULE_OBJ      object;

    while (true)
    {
        DRV_PLC_PHY_Tasks (object);
    }
    </code>

  Remarks:
    - This function is normally not called directly by an application. It is
      called by the system's Tasks routine (SYS_Tasks).
    - This function will never block or access any resources that may cause
      it to block.
*/

void DRV_PLC_PHY_Tasks( SYS_MODULE_OBJ object );

/***************************************************************************
  Function:
       void DRV_PLC_PHY_Sleep( const DRV_HANDLE handle, bool enable )

  Summary:
    Allows to manage the PLC Sleep mode.

  Description:
    This function allows to manage the PLC Sleep mode. Before going to sleep,
    it disables PLC interrupts, and will leave them disabled upon return to
    normal operation. In sleep mode, the PLC transceiver consumes as little
    power as possible. If there is any PLC transmission in progress, it will
    be silently discarded before sleep mode is entered. To exit from sleep
    mode it is not needed to load the PLC binary again.

  Precondition:
    DRV_PLC_PHY_Open must have been called to obtain a valid opened device
    handle.

  Parameters:
    handle - A valid open-instance handle, returned from the driver's open
             routine.
    enable - Set true to enter in sleep mode. Set false to exit from sleep
             mode.

  Returns:
    None.

  Example:
    <code>
    DRV_HANDLE handle;

    while (true)
    {
      if (sleep_condition)
      {
          DRV_PLC_PHY_Sleep (handle, true);
      }
    }
    </code>

  Remarks:
    This function will never block or access any resources that may cause
    it to block.
*/

void DRV_PLC_PHY_Sleep( const DRV_HANDLE handle, bool enable );

/***************************************************************************
  Function:
    void DRV_PLC_PHY_EnableTX( const DRV_HANDLE handle, bool enable )

  Summary:
    Enables/Disables PLC transmission.

  Description:
    This function allows a client to enable or disable the PLC tranmission.
    If there is any transmission on going, it will be cancelled and notified
    through the TX confirmation callback.

  Precondition:
    DRV_PLC_PHY_Open must have been called to obtain a valid opened device
    handle.

  Parameters:
    handle - A valid open-instance handle, returned from the driver's open
             routine.

    enable - Set true to enable PLC tranmission. Set false to disable it.

  Returns:
    None.

  Example:
    <code>
    DRV_HANDLE handle;

    while (true)
    {
        ...
        if (cancel_tx)
        {
            DRV_PLC_PHY_EnableTX (handle, false);
        }
        else
        {
            DRV_PLC_PHY_EnableTX (handle, true);
        }
        ...
    }
    </code>

  ***************************************************************************/

void DRV_PLC_PHY_EnableTX( const DRV_HANDLE handle, bool enable );

#ifdef __cplusplus
}
#endif

#include "driver/plc/phy/drv_plc_phy_local.h"

#endif // #ifndef DRV_PLC_PHY_H
/*******************************************************************************
 End of File
*/
