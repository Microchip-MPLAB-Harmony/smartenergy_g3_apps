/*******************************************************************************
  DRV_G3_MACRT_MACRT Driver Interface Definition

  Company:
    Microchip Technology Inc.

  File Name:
    drv_g3_macrt.h

  Summary:
    MAC RT PLC Library Interface header.

  Description:
    The G3 MAC RT Driver Library provides an interface to access the
    PLC transceiver.
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

#ifndef DRV_G3_MACRT_H
#define DRV_G3_MACRT_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdio.h>
#include <stdbool.h>
#include "driver/driver.h"
#include "system/system.h"
#include "drv_g3_macrt_comm.h"
#include "drv_g3_macrt_definitions.h"
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
/* DRV_G3_MACRT Driver State

  Summary:
    Defines the status of the G3 MAC RT driver.

  Description:
    This data type defines the status of the G3 MAC RT Driver. It is used as
    output of DRV_G3_MACRT_Status.

  Remarks:
    None.
*/

typedef enum
{
    DRV_G3_MACRT_STATE_UNINITIALIZED = SYS_STATUS_UNINITIALIZED,
    DRV_G3_MACRT_STATE_BUSY = SYS_STATUS_BUSY,
    DRV_G3_MACRT_STATE_READY = SYS_STATUS_READY,
    DRV_G3_MACRT_STATE_INITIALIZED = SYS_STATUS_READY_EXTENDED + 1,
    DRV_G3_MACRT_STATE_WAITING_TX_CFM,
    DRV_G3_MACRT_STATE_SLEEP,
    DRV_G3_MACRT_STATE_ERROR = SYS_STATUS_ERROR,
    DRV_G3_MACRT_STATE_ERROR_COMM = SYS_STATUS_ERROR_EXTENDED - 1,
} DRV_G3_MACRT_STATE;

// *****************************************************************************
/* DRV_G3_MACRT Transfer Errors

  Summary:
    Defines the data type for G3 MAC RT Driver transfer errors.

  Description:
    This data type defines the G3 MAC RT Driver transfer errors. This will be
    used to indicate the error of an SPI transfer.

    It is used as parameter of the callback function defined by
    DRV_G3_MACRT_EXCEPTION_CALLBACK.

  Remarks:
    None.
*/

typedef enum
{
    /* SPI has detected an unexpected status */
    DRV_G3_MACRT_EXCEPTION_UNEXPECTED_KEY,

    /* Device has been reseted */
    DRV_G3_MACRT_EXCEPTION_RESET

} DRV_G3_MACRT_EXCEPTION;

// *****************************************************************************
/* G3 MAC RT Driver Initialization Event Handler Function Pointer

  Summary:
    Pointer to a G3 MAC RT Driver Initialization Event handler function.

  Description:
    This data type defines the required function signature for the G3 MAC RT
    driver initialization event handling callback function. A client must
    register a pointer using the callback register function whose function
    signature (parameter and return value types) match the types specified by
    this function pointer in order to receive transfer related event calls back
    from the driver.

    The parameters and return values are described here and a partial example
    implementation is provided.

  Parameters:
    initResult - Initialization result (true if success, false if error).

  Returns:
    None.

  Example:
    <code>
    void APP_MyInitEventHandler( bool initResult )
    {
        if (initResult)
        {

        }
    }
    </code>

  Remarks:
    None.

*/

typedef void ( *DRV_G3_MACRT_INIT_CALLBACK )( bool initResult );

// *****************************************************************************
/* G3 MAC RT Driver Transmission Confirm Event Handler Function Pointer

  Summary:
    Pointer to a G3 MAC RT Driver Transmission Confirm Event handler function.

  Description:
    This data type defines the required function signature for the G3 MAC RT
    driver transmission confirm event handling callback function. A client must
    register a pointer using the callback register function whose function
    signature (parameter and return value types) match the types specified by
    this function pointer in order to receive transfer related event calls back
    from the driver.

    The parameters and return values are described here and a partial example
    implementation is provided.

  Parameters:
    cfmObj - Pointer to the object containing any data necessary to identify the
             result of the last transmission.

  Returns:
    None.

  Example:
    <code>
    void APP_MyTXCfmEventHandler( MAC_RT_TX_CFM_OBJ *cfmObj )
    {
        switch(cfmObj->status)
        {
            case MAC_RT_STATUS_SUCCESS:
                break;
            case MAC_RT_STATUS_CHANNEL_ACCESS_FAILURE:
                break;
            case MAC_RT_STATUS_NO_ACK:
                break;
        }
    }
    </code>

  Remarks:
    - If the status field is MAC_RT_STATUS_SUCCESS, it means that the data was
      transferred successfully.

    - Otherwise, it means that the data was not transferred successfully.

*/

typedef void ( *DRV_G3_MACRT_TX_CFM_CALLBACK )( MAC_RT_TX_CFM_OBJ *cfmObj );

// *****************************************************************************
/* G3 MAC RT Driver Reception Event Handler Function Pointer

  Summary:
    Pointer to a G3 MAC RT Driver Reception Event handler function.

  Description:
    This data type defines the required function signature for the G3 MAC RT
    driver reception event handling callback function. A client must register a
    pointer using the callback register function whose function signature
    (parameter and return value types) match the types specified by this
    function pointer in order to receive transfer related event calls back from
    the driver.

    The parameters and return values are described here and a partial example
    implementation is provided.

  Parameters:
    pData  - Pointer to the data content.

    length - Length of the received data.

  Returns:
    None.

  Example:
    <code>
    void APP_MyRxEventHandler( uint8_t *pData, uint16_t length )
    {
        if (length > 0U)
        {
            memcpy(appData->dataRxBuffer, pData, length);
        }
    }
    </code>

*/

typedef void ( *DRV_G3_MACRT_DATA_IND_CALLBACK )( uint8_t *pData, uint16_t length );

// *****************************************************************************
/* G3 MAC RT Driver Event Handler Function Pointer to get parameters from the
   last received message

  Summary:
    Pointer to a G3 MAC RT Driver Event handler function to get parameters
    from the last received message.

  Description:
    This data type defines the required function signature for the G3 MAC RT
    driver reception parameters event handling callback function. A client must
    register a pointer using the callback register function whose function
    signature (parameter and return value types) match the types specified by
    this function pointer in order to receive transfer related event calls back
    from the driver.

    The parameters and return values are described here and a partial example
    implementation is provided.

  Parameters:
    pParameters - Pointer to the characterization parameters of the last
                  received message.

  Returns:
    None.

  Example:
    <code>
    void APP_MyRxParamsHandler( MAC_RT_RX_PARAMETERS_OBJ *pParameters )
    {
        memcpy(appData->rxParams, (uint8_t *)pParameters, sizeof(MAC_RT_RX_PARAMETERS_OBJ));
    }
    </code>

*/

typedef void ( *DRV_G3_MACRT_RX_PARAMS_IND_CALLBACK )( MAC_RT_RX_PARAMETERS_OBJ *pParameters );

// *****************************************************************************
/* G3 MAC RT Sniffer Event Handler Function Pointer

  Summary:
    Pointer to a G3 MAC RT Sniffer Reception Event handler function.

  Description:
    This data type defines the required function signature for the G3 MAC RT
    driver sniffer event handling callback function. A client must register a
    pointer using the callback register function whose function signature
    (parameter and return value types) match the types specified by this
    function pointer in order to receive transfer related event calls back from
    the driver.

    The parameters and return values are described here and a partial example
    implementation is provided.

  Parameters:
    pData  - Pointer to the data content.

    length - Length of the received data.

  Returns:
    None.

  Example:
    <code>
    void APP_MyMacRtMacSnifferHandler( uint8_t *pData, uint16_t length )
    {
        if (length > 0U)
        {
            memcpy(appData->dataMacRtMacSnifferBuffer, pData, length);
        }
    }
    </code>

*/

typedef void ( *DRV_G3_MACRT_MAC_SNIFFER_IND_CALLBACK )( uint8_t *pData, uint16_t length );

// *****************************************************************************
/* G3 MAC RT Communication Status Handler Function Pointer

  Summary:
    Pointer to a G3 MAC RT Communication Status Event handler function.

  Description:
    This data type defines the required function signature for the G3 MAC RT
    driver communication status event handling callback function. A client must
    register a pointer using the callback register function whose function
    signature (parameter and return value types) match the types specified by
    this function pointer in order to receive transfer related event calls back
    from the driver.

    The parameters and return values are described here and a partial example
    implementation is provided.

  Parameters:
    pData - Pointer to the data content.

  Returns:
    None.

  Example:
    <code>
    void APP_MyPLCMacRtCommStatusHandler( uint8_t *pData )
    {
        memcpy(appData->dataMacCommStatusBuffer, pData, MAC_RT_COMM_STATUS_SIZE);
    }
    </code>

*/

typedef void ( *DRV_G3_MACRT_COMM_STATUS_IND_CALLBACK )( uint8_t *pData );

// *****************************************************************************
/* G3 MAC RT PHY Sniffer Event Handler Function Pointer

  Summary:
    Pointer to a G3 MAC RT PHY Sniffer Reception Event handler function.

  Description:
    This data type defines the required function signature for the G3 MAC RT
    driver sniffer event handling callback function. A client must register a
    pointer using the callback register function whose function signature
    (parameter and return value types) match the types specified by this
    function pointer in order to receive transfer related event calls back from
    the driver.

    The parameters and return values are described here and a partial example
    implementation is provided.

  Parameters:
    pData  - Pointer to the data content.

    length - Length of the received data.

  Returns:
    None.

  Example:
    <code>
    void APP_MyMacRtPhySnifferHandler( uint16_t length )
    {
        if (length > 0U)
        {
            SRV_USI_Send_Message(appData.usiHandler, SRV_USI_PROT_ID_SNIFF_G3,
                (uint8_t *)&appData.phySnifferData, length);
        }
    }
    </code>

*/

typedef void ( *DRV_G3_MACRT_PHY_SNIFFER_IND_CALLBACK )( uint16_t length );

// *****************************************************************************
/* G3 MAC RT Driver Exceptions Event Handler Function Pointer

  Summary:
    Pointer to a G3 MAC RT Driver Exceptions Event handler function.

  Description:
    This data type defines the required function signature for the G3 MAC RT
    driver exceptions event handling callback function. A client must register a
    pointer using the callback register function whose function signature
    (parameter and return value types) match the types specified by this
    function pointer in order to receive transfer related event calls back from
    the driver.

    The parameters and return values are described here and a partial example
    implementation is provided.

  Parameters:
    exception - Value identifying the exception code which occurs during SPI
                transfer.

  Returns:
    None.

  Example:
    <code>
    void APP_MyExceptionEventHandler( DRV_G3_MACRT_EXCEPTION exception )
    {
        switch (exception)
        {
            case DRV_G3_MACRT_EXCEPTION_UNEXPECTED_KEY:
                break;

            case DRV_G3_MACRT_EXCEPTION_RESET:
                break;

            default:
        }

        appData->macrt_exception = true;
    }
    </code>

*/

typedef void ( *DRV_G3_MACRT_EXCEPTION_CALLBACK )( DRV_G3_MACRT_EXCEPTION exception );


// *****************************************************************************
// *****************************************************************************
// Section: DRV MAC RT Driver System Interface Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ DRV_G3_MACRT_Initialize(
        const SYS_MODULE_INDEX index,
        const SYS_MODULE_INIT * const init
    )

  Summary:
    Initializes the G3 MAC RT instance for the specified driver index.

  Description:
    This routine initializes the G3 MAC RT driver making it ready for
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
    SYS_MODULE_OBJ   sysObjDrvMACRT;

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
        .init = (DRV_PLC_HAL_INIT)drv_plc_hal_init,
        .setup = (DRV_PLC_HAL_SETUP)drv_plc_hal_setup,
        .reset = (DRV_PLC_HAL_RESET)drv_plc_hal_reset,
        .getCd = (DRV_PLC_HAL_GET_CD)drv_plc_hal_get_cd,
        .enableExtInt = (DRV_PLC_HAL_ENABLE_EXT_INT)drv_plc_hal_enable_interrupt,
        .delay = (DRV_PLC_HAL_DELAY)drv_plc_hal_delay,
        .sendBootCmd = (DRV_PLC_HAL_SEND_BOOT_CMD)drv_plc_hal_send_boot_cmd,
        .sendWrRdCmd = (DRV_PLC_HAL_SEND_WRRD_CMD)drv_plc_hal_send_wrrd_cmd,
    };

    extern uint8_t g3_macrt_bin_start;
    extern uint8_t g3_macrt_bin_end;

    DRV_G3_MACRT_INIT drvPLCMacRtInitData = {
        .plcHal = &drvPLCHalAPI,
        .numClients = DRV_G3_MACRT_CLIENTS_NUMBER_IDX,
        .plcProfile = DRV_G3_MACRT_PLC_PROFILE,
        .binStartAddress = (uint32_t)&g3_macrt_bin_start,
        .binEndAddress = (uint32_t)&g3_macrt_bin_end,
        .secure = DRV_G3_MACRT_SECURE,
    };

    sysObjDrvMACRT = DRV_G3_MACRT_Initialize(DRV_G3_MACRT_INDEX_0, (SYS_MODULE_INIT *)&drvPLCMacRtInitData);
    PIO_PinInterruptCallbackRegister(DRV_G3_MACRT_EXT_INT_PIN, DRV_G3_MACRT_ExternalInterruptHandler, sysObjDrvMACRT);
    </code>

  Remarks:
    This routine must be called before any other DRV_G3_MACRT routine is called.
    This routine should only be called once during system initialization.
    This routine will block for hardware access.
*/

SYS_MODULE_OBJ DRV_G3_MACRT_Initialize( const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init);

// *****************************************************************************
/* Function:
    DRV_HANDLE DRV_G3_MACRT_Open
    (
        const SYS_MODULE_INDEX index,
        const DRV_PLC_BOOT_DATA_CALLBACK callback
    )

  Summary:
    Opens the specified G3 MAC RT driver instance and returns a handle to
    it.

  Description:
    This routine opens the specified G3 MAC RT driver instance and provides a
    handle that must be provided to all other client-level operations to
    identify the caller and the instance of the driver.

    This driver is a single client driver, so DRV_G3_MACRT_Open API should be
    called only once until driver is closed.

  Precondition:
    Function DRV_G3_MACRT_Initialize must have been called before calling this
    function.

  Parameters:
    index    - Identifier for the object instance to be opened.

    callback - Boot Data Callback Function Pointer. In case of use NULL,
               .binStartAddress and .binEndAddress fields must be configured
               in initialization data DRV_G3_MACRT_INIT.

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

    handle = DRV_G3_MACRT_Open(DRV_G3_MACRT_INDEX_0, NULL);
    if (handle == DRV_HANDLE_INVALID)
    {

    }
    </code>

  Remarks:
    The handle returned is valid until the DRV_G3_MACRT_Close routine is called.
    This routine will NEVER block waiting for hardware.
*/
DRV_HANDLE DRV_G3_MACRT_Open(const SYS_MODULE_INDEX index,
        const DRV_PLC_BOOT_DATA_CALLBACK callback);

// *****************************************************************************
/* Function:
    void DRV_G3_MACRT_Close( const DRV_HANDLE handle )

  Summary:
    Closes opened-instance of the G3 MAC RT driver.

  Description:
    This routine closes opened-instance of the G3 MAC RT driver, invalidating
    the handle. A new transfer of binary file should be performed by calling
    DRV_G3_MACRT_Initialize routine before the caller may use the driver again.

  Precondition:
    DRV_G3_MACRT_Open must have been called to obtain a valid opened device
    handle.

  Parameters:
    handle - A valid open-instance handle, returned from the driver's open
             routine.

  Returns:
    None.

  Example:
    <code>
    DRV_G3_MACRT_Close(handle);
    </code>

  Remarks:
    None.
*/
void DRV_G3_MACRT_Close(const DRV_HANDLE handle);

// *****************************************************************************
/* Function:
    void DRV_G3_MACRT_TxRequest( const DRV_HANDLE handle, uint8_t *pData, uint16_t length )

  Summary:
    Allows a client to transmit G3 MAC data through Power Line (PLC).

  Description:
    This routine sends a new data message through PLC using the G3 MAC RT
    driver. The message must have a valid IEEE 802.15.4 MAC format with its
    corresponding MAC header.

  Precondition:
    DRV_G3_MACRT_Open must have been called to obtain a valid opened device
    handle.

  Parameters:
    handle - A valid open-instance handle, returned from the driver's open
             routine.

    pData  - Pointer to the data to transmit.

    length - Length of the data to transmit in bytes.


  Returns:
    None.

  Example:
    <code>
    _setupTransmissionParameters();

    DRV_G3_MACRT_TxRequest(handle, appData.pData, appData.length);
    </code>

  Remarks:
    None.
*/
void DRV_G3_MACRT_TxRequest(const DRV_HANDLE handle, uint8_t *pData, uint16_t length);

// *****************************************************************************
/* Function:
    MAC_RT_STATUS DRV_G3_MACRT_PIBGet( const DRV_HANDLE handle, MAC_RT_PIB_OBJ *pibObj )

  Summary:
    Allows a client to get information from PLC transceiver about G3 MAC RT
    and PHY information base (PIB).

  Description:
    This routine gets G3 MAC RT and PHY data information from the PLC
    transceiver.

  Precondition:
    DRV_G3_MACRT_Open must have been called to obtain a valid opened device
    handle. MAC_RT_PIB_OBJ must be configured before getting information.

  Parameters:
    handle - A valid open-instance handle, returned from the driver's open
             routine.

    pibObj - Pointer to the PIB object to get.

  Returns:
    MAC_RT_STATUS Enum value containing the result of get operation.

  Example:
    <code>
    MAC_RT_PIB_OBJ pibObj;
    uint32_t phyVersion;
    uint8_t macRtVersion[6];

    pibObj.pib = MAC_RT_PIB_MANUF_PHY_PARAM;
    pibObj.index = PHY_PARAM_VERSION;
    pibObj.pData = &phyVersion;
    pibObj.length = 4;

    DRV_G3_MACRT_PIBGet(handle, &pibObj);

    pibObj.pib = MAC_RT_PIB_MANUF_MAC_RT_INTERNAL_VERSION;
    pibObj.index = 0;
    pibObj.pData = &macRtVersion;
    pibObj.length = 6;

    DRV_G3_MACRT_PIBGet(handle, &pibObj);
    </code>

  Remarks:
    None.
*/
MAC_RT_STATUS DRV_G3_MACRT_PIBGet(const DRV_HANDLE handle, MAC_RT_PIB_OBJ *pibObj);

// *****************************************************************************
/* Function:
    MAC_RT_STATUS DRV_G3_MACRT_PIBSet( const DRV_HANDLE handle, MAC_RT_PIB_OBJ *pibObj )

  Summary:
    Allows a client to set information to PLC transceiver on G3 MAC RT and PHY
    information base (PIB).

  Description:
    This routine sets G3 MAC RT and PHY data information to the PLC transceiver.

  Precondition:
    DRV_G3_MACRT_Open must have been called to obtain a valid opened device
    handle. MAC_RT_PIB_OBJ must be configured before setting information.

  Parameters:
    handle - A valid open-instance handle, returned from the driver's open
             routine.

    pibObj - Pointer to the PIB object to set.

  Returns:
    MAC_RT_STATUS Enum value containing the result of set operation.

  Example:
    <code>
    MAC_RT_PIB_OBJ pibObj;
    uint8_t autoMode
    uint8_t impedance;
    uint8_t forcedRobo;

    autoMode = 0;

    impedance = VLO_STATE;

    pibObj.pib = MAC_RT_PIB_MANUF_PHY_PARAM;
    pibObj.index = PHY_PARAM_CFG_AUTODETECT_BRANCH;
    pibObj.pData = &autoMode;
    pibObj.length = 1;
    DRV_G3_MACRT_PIBSet(handle, &pibObj);

    pibObj.pib = MAC_RT_PIB_MANUF_PHY_PARAM;
    pibObj.index = PHY_PARAM_CFG_IMPEDANCE;
    pibObj.pData = &impedance;
    pibObj.length = 1;
    DRV_G3_MACRT_PIBSet(handle, &pibObj);

    forcedRobo = 1;

    pibObj.pib = MAC_RT_PIB_MANUF_FORCED_MOD_TYPE;
    pibObj.index = 0;
    pibObj.pData = &forcedRobo;
    pibObj.length = 1;
    DRV_G3_MACRT_PIBSet(handle, &pibObj);
    </code>

  Remarks:
    None.
*/
MAC_RT_STATUS DRV_G3_MACRT_PIBSet(const DRV_HANDLE handle, MAC_RT_PIB_OBJ *pibObj);

// *****************************************************************************
/* Function:
    uint32_t DRV_G3_MACRT_GetTimerReference( const DRV_HANDLE handle )

  Summary:
    Gets the internal timer reference from PLC transceiver in microseconds.

  Description:
    PLC transceiver has an internal 32-bit counter which is used as internal
    time reference for all time calculations in PHY and MAC-RT layers. This
    counter is internally configured to be increased each microsecond. This
    function allows to use this counter as an accurate time reference for upper
    layers.

  Precondition:
    DRV_G3_MACRT_Open must have been called to obtain a valid opened device
    handle.

  Parameters:
    handle - A valid open-instance handle, returned from the driver's open
             routine.

  Returns:
    Internal timer reference from PLC transceiver in microseconds.

  Example:
    <code>
    uint32_t macRtTimerRef;

    macRtTimerRef = DRV_G3_MACRT_GetTimerReference(handle);

    </code>

  Remarks:
    This function requires an SPI transaction to retrieve time from
    PLC transceiver, so potential jitter in different SPI accesses
    may impact accuracy.
*/
uint32_t DRV_G3_MACRT_GetTimerReference(const DRV_HANDLE handle);

// *****************************************************************************
/* Function:
    void DRV_G3_MACRT_InitCallbackRegister(
        const SYS_MODULE_INDEX index,
        const DRV_G3_MACRT_INIT_CALLBACK callback
    );

  Summary:
    Allows a client to set a G3 MAC RT initialization event handling function
    for the driver to call back when the PLC binary file has been loaded into
    the PLC transceiver.

  Description:
    This function allows a client to register a G3 MAC RT initialization event
    handling function for the driver to call back when the loading of the PLC
    binary file has finished.

    This routine should be called before calling DRV_G3_MACRT_Open() function.

    The callback once set, persists until the client closes the driver or sets
    another callback (which could be a "NULL" pointer to indicate no callback).

  Precondition:
    Function DRV_G3_MACRT_Initialize must have been called before calling this
    function.


  Parameters:
    index    - Identifier for the object instance to be opened.

    callback - Pointer to the callback function.

  Returns:
    None.

  Example:
    <code>
    void APP_PLC_Init_callback(bool initResult)
    {
        if (initResult == True)
        {

        }
        else
        {

        }
    }

    DRV_HANDLE handle;

    DRV_G3_MACRT_initCallbackRegister( DRV_G3_MACRT_INDEX_0, APP_PLC_Init_callback );

    handle = DRV_G3_MACRT_Open(DRV_G3_MACRT_INDEX_0, NULL);
    if (handle == DRV_HANDLE_INVALID)
    {

    }
    </code>

*/

void DRV_G3_MACRT_InitCallbackRegister(
    const SYS_MODULE_INDEX index,
    const DRV_G3_MACRT_INIT_CALLBACK callback
);

// *****************************************************************************
/* Function:
    void DRV_G3_MACRT_DataCfmCallbackRegister(
        const DRV_HANDLE handle,
        const DRV_G3_MACRT_TX_CFM_CALLBACK callback
    );

  Summary:
    Allows a client to set a G3 MAC RT data confirm event handling function for
    the driver to call back when the requested transmission has finished.

  Description:
    This function allows a client to register a G3 MAC RT data confirm event
    handling function for the driver to call back when a data confirmation PLC
    event occurs.

    The event handler should be set before the client submits any transmission
    requests that could generate events. The callback once set, persists until
    the client closes the driver or sets another callback (which could be a
    "NULL" pointer to indicate no callback).

  Precondition:
    DRV_G3_MACRT_Open must have been called to obtain a valid opened device
    handle.

  Parameters:
    handle   - A valid open-instance handle, returned from the driver's open
               routine.

    callback - Pointer to the callback function.

  Returns:
    None.

  Example:
    <code>
    void APP_PLC_Tx_Cfm_callback(MAC_RT_TX_CFM_OBJ *cfmObj)
    {
        if (cfmObj->result == MAC_RT_STATUS_SUCCESS)
        {

        }
        else
        {

        }
    }

    MY_APP_OBJ myAppObj;

    DRV_G3_MACRT_TxCfmCallbackRegister( myAppObj.myHandle, APP_PLC_Tx_Cfm_callback );

    DRV_G3_MACRT_TxRequest(myAppObj.myHandle, myAppObj.pData, myAppObj.dataLength);
    </code>

*/

void DRV_G3_MACRT_TxCfmCallbackRegister(
    const DRV_HANDLE handle,
    const DRV_G3_MACRT_TX_CFM_CALLBACK callback
);

// *****************************************************************************
/* Function:
    void DRV_G3_MACRT_DataIndCallbackRegister(
        const DRV_HANDLE handle,
        const DRV_G3_MACRT_DATA_IND_CALLBACK callback
    );

  Summary:
    Allows a client to set a G3 MAC RT data indication event handling function
    for the driver to call back when a data reception has finished.

  Description:
    This function allows a client to register a G3 MAC RT data indication
    event handling function for the driver to call back when a data reception
    PLC event occurs.

    Before this callback is set, any received frame by the PLC transceiver will
    not be notified. The callback once set, persists until the client closes the
    driver or sets another callback (which could be a "NULL" pointer to indicate
    no callback).

  Precondition:
    DRV_G3_MACRT_Open must have been called to obtain a valid opened device
    handle.

  Parameters:
    handle   - A valid open-instance handle, returned from the driver's open
               routine.

    callback - Pointer to the callback function.

  Returns:
    None.

  Example:
    <code>

    void APP_PLC_Data_Ind_callback(uint8_t *pData, uint16_t length)
    {
        if (length > 0U)
        {
            memcpy(myAppObj->pDataRcv, pData, length);
            myAppObj->lengthDataRcv = length;
        }

    }

    MY_APP_OBJ myAppObj;

    DRV_G3_MACRT_DataIndCallbackRegister( myAppObj.myHandle, APP_PLC_Data_Ind_callback );
    </code>

*/

void DRV_G3_MACRT_DataIndCallbackRegister(
    const DRV_HANDLE handle,
    const DRV_G3_MACRT_DATA_IND_CALLBACK callback
);

// *****************************************************************************
/* Function:
    void DRV_G3_MACRT_RxParamsIndCallbackRegister(
        const DRV_HANDLE handle,
        const DRV_G3_MACRT_RX_PARAMS_IND_CALLBACK callback
    );

  Summary:
    Allows a client to set a G3 MAC RT reception parameters event handling
    function for the driver to call back when a new data message is received.

  Description:
    This function allows a client to register a G3 MAC RT Rx parameters
    indication event handling function for the driver to call back when a new
    data message is received.

    This callback has to be set to get the reception parameters related to a
    received frame, otherwise such information will be unknown. The callback
    once set, persists until the client closes the driver or sets another
    callback (which could be a "NULL" pointer to indicate no callback).

  Precondition:
    DRV_G3_MACRT_Open must have been called to obtain a valid opened device
    handle.

  Parameters:
    handle   - A valid open-instance handle, returned from the driver's open
               routine.

    callback - Pointer to the callback function.

  Returns:
    None.

  Example:
    <code>

    void APP_PLC_Params_Ind_callback(MAC_RT_RX_PARAMETERS_OBJ *pParameters)
    {

    }

    MY_APP_OBJ myAppObj;

    DRV_G3_MACRT_RxParamsIndCallbackRegister( myAppObj.myHandle, APP_PLC_Params_Ind_callback );
    </code>

*/

void DRV_G3_MACRT_RxParamsIndCallbackRegister(
    const DRV_HANDLE handle,
    const DRV_G3_MACRT_RX_PARAMS_IND_CALLBACK callback
);

// *****************************************************************************
/* Function:
    void DRV_G3_MACRT_MacSnifferCallbackRegister(
        const DRV_HANDLE handle,
        const DRV_G3_MACRT_MAC_SNIFFER_CALLBACK callback,
        uint8_t* pDataBuffer
    );

  Summary:
    Allows enabling G3-PLC MAC Sniffer capabilities and set the data buffer in
    which the content of the sniffer packet will be stored when a new PLC
    message is received.

  Description:
    This function allows a client to register a G3-PLC MAC Sniffer event
    handling function for the driver to call back when either a data reception
    event or confirmation of the last transmission event occurs.

    A MAC Sniffer will receive any frame in the PLC medium, regardless of
    addressing or frame type.

    The callback once set, persists until the client closes the driver or sets
    another callback (which could be a "NULL" pointer to indicate no callback).

  Precondition:
    DRV_G3_MACRT_Open must have been called to obtain a valid opened device
    handle.

  Parameters:
    handle      - A valid open-instance handle, returned from the driver's
                  open routine.

    callback    - Pointer to the callback function.

    pDataBuffer - Pointer to buffer where sniffer message is stored. Data buffer
                  should be defined as MAC_RT_DATA_MAX_SIZE to avoid memory
                  overlaps.

  Returns:
    None.

  Example:
    <code>

    void APP_G3_MAC_RT_Mac_Sniffer_callback(uint8_t *pData, uint16_t length)
    {
        if (length)
        {

        }
    }

    MY_APP_OBJ myAppObj;

    DRV_G3_MACRT_MacSnifferCallbackRegister( myAppObj.myHandle, APP_G3_MAC_RT_Mac_Sniffer_callback,
            myAppObj.pMacSnfBuffer);
    </code>

*/

void DRV_G3_MACRT_MacSnifferCallbackRegister(
    const DRV_HANDLE handle,
    const DRV_G3_MACRT_MAC_SNIFFER_IND_CALLBACK callback,
    uint8_t* pDataBuffer
);

// *****************************************************************************
/* Function:
    void DRV_G3_MACRT_CommStatusCallbackRegister(
        const DRV_HANDLE handle,
        const DRV_G3_MACRT_COMM_STATUS_IND_CALLBACK callback
    );

  Summary:
    Allows a client to set a G3-PLC MAC Communication Status indication event
    handling function for the driver to call back when a Communication Status
    event has to be reported.

  Description:
    This function allows a client to register a G3-PLC MAC Communication Status
    indication event handling function for the driver to call back when a
    Communication Status event occurs. Communication Status events are defined
    in IEEE 802.15.4 standard.

    The callback once set, persists until the client closes the driver or sets
    another callback (which could be a "NULL" pointer to indicate no callback).

  Precondition:
    DRV_G3_MACRT_Open must have been called to obtain a valid opened device
    handle.

  Parameters:
    handle   - A valid open-instance handle, returned from the driver's open routine.

    callback - Pointer to the callback function.

  Returns:
    None.

  Example:
    <code>

    void APP_G3_MAC_RT_Comm_Status_callback(uint8_t *pData)
    {

    }

    MY_APP_OBJ myAppObj;

    DRV_G3_MACRT_CommStatusCallbackRegister( myAppObj.myHandle, APP_G3_MAC_RT_Comm_Status_callback );
    </code>

*/

void DRV_G3_MACRT_CommStatusCallbackRegister(
    const DRV_HANDLE handle,
    const DRV_G3_MACRT_COMM_STATUS_IND_CALLBACK callback
);

// *****************************************************************************
/* Function:
    void DRV_G3_MACRT_PhySnifferCallbackRegister(
        const DRV_HANDLE handle,
        const DRV_G3_MACRT_PHY_SNIFFER_CALLBACK callback,
        uint8_t* pDataBuffer
    );

  Summary:
    Allows to enable G3-PLC PHY Sniffer capabilities and set the data buffer in
    which the content of the sniffer packet will be stored when a new PLC
    message is received.

  Description:
    This function allows a client to register a G3-PLC PHY Sniffer event
    handling function for the driver to call back when either a data reception
    event or confirmation of the last transmission event occurs.

    The content of the sniffer data is compliant with Microchip Hybrid Sniffer
    tool.

    The callback once set, persists until the client closes the driver or sets
    another callback (which could be a "NULL" pointer to indicate no callback).

  Precondition:
    DRV_G3_MACRT_Open must have been called to obtain a valid opened device
    handle.

  Parameters:
    handle      - A valid open-instance handle, returned from the driver's
                  open routine.

    callback    - Pointer to the callback function.

    pDataBuffer - Pointer to buffer where sniffer message is stored. Data buffer
                  should be defined as sizeof(MAC_RT_PHY_SNIFFER_HEADER) +
                  MAC_RT_PHY_DATA_MAX_SIZE to avoid memory overlaps.

  Returns:
    None.

  Example:
    <code>

    void APP_G3_MAC_RT_Phy_Sniffer_callback(uint8_t *pData, uint16_t length)
    {
        if (length)
        {

        }
    }

    MY_APP_OBJ myAppObj;

    DRV_G3_MACRT_PhySnifferCallbackRegister( myAppObj.myHandle, APP_G3_MAC_RT_Phy_Sniffer_callback,
            myAppObj.pPhySnfBuffer);
    </code>

*/

void DRV_G3_MACRT_PhySnifferCallbackRegister(
    const DRV_HANDLE handle,
    const DRV_G3_MACRT_PHY_SNIFFER_IND_CALLBACK callback,
    uint8_t* pDataBuffer
);

// *****************************************************************************
/* Function:
    void DRV_G3_MACRT_ExceptionCallbackRegister(
        const DRV_HANDLE handle,
        const DRV_G3_MACRT_EXCEPTION_CALLBACK callback
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
    DRV_G3_MACRT_Open must have been called to obtain a valid opened device
    handle.

  Parameters:
    handle   - A valid open-instance handle, returned from the driver's open
               routine.

    callback - Pointer to the callback function.

  Returns:
    None.

  Example:
    <code>

    void APP_PLC_Exception_callback(DRV_G3_MACRT_EXCEPTION_OBJ *exceptionObj)
    {

    }

    MY_APP_OBJ myAppObj;

    DRV_G3_MACRT_ExceptionCallbackRegister( myAppObj.myHandle, APP_PLC_Exception_callback );
    </code>

*/

void DRV_G3_MACRT_ExceptionCallbackRegister(
    const DRV_HANDLE handle,
    const DRV_G3_MACRT_EXCEPTION_CALLBACK callback
);

// *****************************************************************************
/* Function:
    void DRV_G3_MACRT_ExternalInterruptHandler(
        const uintptr_t context
    );

  Summary:
    Allows application to register callback for PLC Interrupt pin.

  Description:
    This function allows a client to register a callback function to handle
    G3 MAC RT interrupt.

  Precondition:
    Function DRV_G3_MACRT_Initialize must have been called before calling this
    function.

  Parameters:
    context - Pointer to parameters to be passed to Handler function.

  Returns:
    None.

  Remarks:
    None.

  Example:
    <code>
    sysObj.drvPLCMacRt = DRV_G3_MACRT_Initialize(DRV_G3_MACRT_INDEX, (SYS_MODULE_INIT *)&drvPLCMacRtInitData);
    EIC_CallbackRegister(DRV_G3_MACRT_EXT_INT_PIN, DRV_G3_MACRT_ExternalInterruptHandler, sysObj.drvPLCMacRt);
    </code>

*/

void DRV_G3_MACRT_ExternalInterruptHandler( uintptr_t context );

// *************************************************************************
/* Function:
    DRV_G3_MACRT_STATE DRV_G3_MACRT_Status( const SYS_MODULE_INDEX index )

  Summary:
    Gets the current status of the G3 MAC RT driver module.

  Description:
    This routine provides the current status of the G3 MAC RT driver module.

  Preconditions:
    Function DRV_G3_MACRT_Initialize should have been called before calling
    this function.

  Parameters:
    index - Identifier for the instance used to initialize driver.

  Returns:
    DRV_G3_MACRT_STATE_READY: Indicates that the driver is ready to accept
    requests for new operations.

    DRV_G3_MACRT_STATE_UNINITIALIZED: Indicates the driver is not
    initialized.

    DRV_G3_MACRT_STATE_ERROR: Indicates the driver is not initialized
    correctly.

    DRV_G3_MACRT_STATE_BUSY: Indicates the driver is initializing.

    DRV_G3_MACRT_STATE_WAITING_TX_CFM: Indicates the driver is waiting a
    confirmation of the last transmission.

  Example:
    <code>
    DRV_G3_MACRT_STATE state;
    state = DRV_G3_MACRT_Status(DRV_G3_MACRT_INDEX);
    </code>

  Remarks:
    None.
*/

DRV_G3_MACRT_STATE DRV_G3_MACRT_Status( const SYS_MODULE_INDEX index );

//***************************************************************************
/* Function:
    void DRV_G3_MACRT_Tasks( SYS_MODULE_OBJ object )

  Summary:
    Maintains the driver's state machine.

  Description:
    This function is used to maintain the G3 MAC RT driver's internal state
    machine.

  Precondition:
    The DRV_G3_MACRT_Initialize routine must have been called for the
    specified G3 MAC RT driver instance.

  Parameters:
    object - Object handle for the specified driver instance (returned from
             DRV_G3_MACRT_Initialize)
  Returns:
    None
  Example:
    <code>
    SYS_MODULE_OBJ      object;

    while (true)
    {
        DRV_G3_MACRT_Tasks (object);
    }
    </code>

  Remarks:
    - This function is normally not called directly by an application. It is
      called by the system's Tasks routine (SYS_Tasks).
    - This function will never block or access any resources that may cause
      it to block.
*/

void DRV_G3_MACRT_Tasks( SYS_MODULE_OBJ object );

//***************************************************************************
/* Function:
    void DRV_G3_MACRT_EnableTX( const DRV_HANDLE handle, bool enable )

  Summary:
    Enables/Disables PLC transmission.

  Description:
    This function allows a client to enable or disable the PLC tranmission.
    If there is any transmission on going, it will be cancelled and notified
    through the TX confirmation callback.

  Precondition:
    DRV_G3_MACRT_Open must have been called to obtain a valid opened device
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
            DRV_G3_MACRT_EnableTX (handle, false);
        }
        else
        {
            DRV_G3_MACRT_EnableTX (handle, true);
        }
        ...
    }
    </code>

  */

void DRV_G3_MACRT_EnableTX( const DRV_HANDLE handle, bool enable );

//***************************************************************************
/*
  Function:
    void DRV_G3_MACRT_SetCoordinator( const DRV_HANDLE handle )

  Summary:
    Enables G3-PLC coordinator capabilities.

  Description:
    This function allows a client to enable G3-PLC coordinator capabilities.

  Precondition:
    DRV_G3_MACRT_Open must have been called to obtain a valid opened device
    handle.

  Parameters:
    handle - A valid open-instance handle, returned from the driver's open
             routine.

  Returns:
    None.

  Example:
    <code>
    DRV_HANDLE drvHandler;

    DRV_G3_MACRT_SetCoordinator(drvHandler);

    </code>

*/

void DRV_G3_MACRT_SetCoordinator(const DRV_HANDLE handle);

//***************************************************************************
/*
  Function:
    void DRV_G3_MACRT_EnablePhySniffer( const DRV_HANDLE handle )

  Summary:
    Enables G3-PLC PHY Sniffer capabilities.

  Description:
    This function allows a client to enable G3-PLC PHY Sniffer capabilities.

  Precondition:
    DRV_G3_MACRT_Open must have been called to obtain a valid opened device
    handle.

  Parameters:
    handle - A valid open-instance handle, returned from the driver's open
             routine.

  Returns:
    None.

  Example:
    <code>
    DRV_HANDLE drvHandler;

    DRV_G3_MACRT_EnablePhySniffer(drvHandler);
    </code>

*/

void DRV_G3_MACRT_EnablePhySniffer(const DRV_HANDLE handle);

#ifdef __cplusplus
}
#endif

#include "driver/plc/g3MacRt/drv_g3_macrt_local.h"

#endif // #ifndef DRV_G3_MACRT_H
/*******************************************************************************
 End of File
*/
