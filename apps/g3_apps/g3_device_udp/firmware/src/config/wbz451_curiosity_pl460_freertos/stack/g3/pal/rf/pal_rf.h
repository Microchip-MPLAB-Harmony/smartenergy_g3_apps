/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    pal_rf.h

  Summary:
    RF Platform Abstraction Layer (PAL) Interface header file.

  Description:
    RF Platform Abstraction Layer (PAL) Interface header file. The RF PAL module
    provides a simple interface to manage the RF PHY layer.
 ******************************************************************************/

// DOM-IGNORE-BEGIN
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
// DOM-IGNORE-END

#ifndef PAL_RF_H
#define PAL_RF_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include "system/system.h"
#include "driver/driver.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

/* MISRA C-2012 deviation block start */
/* MISRA C-2012 Rule 5.2 deviated twic. Deviation record ID - H3_MISRAC_2012_R_5_2_DR_1 */

// *****************************************************************************
/* RF PAL PIB Attribute

  Summary:
    Defines the list of RF PAL PIB attributes.

  Description:
    This data type defines the list of available (PHY Information Base) PIB
    attributes in RF PAL. PIB values can be read by PAL_RF_GetRfPhyPib and
    written by PAL_RF_SetRfPhyPib. PAL_RF_GetRfPhyPibLength can be used to know
    the size of each PIB.

  Remarks:
    None.
*/
typedef enum
{
    /* RF device identifier. 16 bits */
    PAL_RF_PIB_DEVICE_ID = 0x0000,
    /* RF PHY layer firmware version number. 6 bytes */
    PAL_RF_PIB_FW_VERSION = 0x0001,
    /* RF device reset (write-only). 8 bits */
    PAL_RF_PIB_DEVICE_RESET = 0x0002,
    /* RF transceiver reset (write-only). 8 bits */
    PAL_RF_PIB_TRX_RESET = 0x0080,
    /* RF transceiver sleep. 8 bits */
    PAL_RF_PIB_TRX_SLEEP = 0x0081,
    /* RF PHY configuration. 19 bytes
     * (see "DRV_RF215_PHY_CFG_OBJ", only available for RF215) */
    PAL_RF_PIB_PHY_CONFIG = 0x0100,
    /* RF PHY band and operating mode. 16 bits (only available for RF215) */
    PAL_RF_PIB_PHY_BAND_OPERATING_MODE = 0x0101,
    /* RF channel number used for transmission and reception. 16 bits */
    PAL_RF_PIB_PHY_CHANNEL_NUM = 0x0120,
    /* RF frequency in Hz used for transmission and reception.
     * 32 bits (read-only) */
    PAL_RF_PIB_PHY_CHANNEL_FREQ_HZ = 0x0121,
    /* RF channel page used for transmission and reception. 8 bits
     * (only available for WBZ45/PIC32CX-BZ2) */
    PAL_RF_PIB_PHY_CHANNEL_PAGE = 0x0122,
    /* RF channels supported used for transmission and reception. 32 bits
     * (only available for WBZ45/PIC32CX-BZ2) */
    PAL_RF_PIB_PHY_CHANNELS_SUPPORTED = 0x0123,
    /* Duration in us of Energy Detection for CCA. 16 bits */
    PAL_RF_PIB_PHY_CCA_ED_DURATION_US = 0x0141,
    /* Threshold in dBm for CCA with Energy Detection. 8 bits */
    PAL_RF_PIB_PHY_CCA_ED_THRESHOLD_DBM = 0x0142,
    /* Duration in symbols of Energy Detection for CCA. 16 bits */
    PAL_RF_PIB_PHY_CCA_ED_DURATION_SYMBOLS = 0x0143,
    /* Threshold in dB above sensitivity for CCA with Energy Detection. 8 bits */
    PAL_RF_PIB_PHY_CCA_ED_THRESHOLD_SENSITIVITY = 0x0144,
    /* Perform a single ED measurement on current channel (dBm). 8 bits
     * (only available for WBZ45/PIC32CX-BZ2) */
    PAL_RF_PIB_PHY_CCA_ED_SAMPLE = 0x0145,
    /* Sensitivity in dBm (according to 802.15.4). 8 bits */
    PAL_RF_PIB_PHY_SENSITIVITY = 0x0150,
    /* Maximum TX power in dBm. 8 bits */
    PAL_RF_PIB_PHY_MAX_TX_POWER = 0x0151,
    /* Turnaround time in us (aTurnaroundTime in IEEE 802.15.4).
     * 16 bits (read-only) */
    PAL_RF_PIB_PHY_TURNAROUND_TIME = 0x0160,
    /* Number of payload symbols in last transmitted message. 16 bits */
    PAL_RF_PIB_PHY_TX_PAY_SYMBOLS = 0x0180,
    /* Number of payload symbols in last received message. 16 bits */
    PAL_RF_PIB_PHY_RX_PAY_SYMBOLS = 0x0181,
    /* Successfully transmitted messages count. 32 bits */
    PAL_RF_PIB_PHY_TX_TOTAL = 0x01A0,
    /* Successfully transmitted bytes count. 32 bits */
    PAL_RF_PIB_PHY_TX_TOTAL_BYTES = 0x01A1,
    /* Transmission errors count. 32 bits */
    PAL_RF_PIB_PHY_TX_ERR_TOTAL = 0x01A2,
    /* Transmission errors count due to already in transmission. 32 bits */
    PAL_RF_PIB_PHY_TX_ERR_BUSY_TX = 0x01A3,
    /* Transmission errors count due to already in reception. 32 bits */
    PAL_RF_PIB_PHY_TX_ERR_BUSY_RX = 0x01A4,
    /* Transmission errors count due to busy channel. 32 bits */
    PAL_RF_PIB_PHY_TX_ERR_BUSY_CHN = 0x01A5,
    /* Transmission errors count due to bad message length. 32 bits */
    PAL_RF_PIB_PHY_TX_ERR_BAD_LEN = 0x01A6,
    /* Transmission errors count due to bad format. 32 bits */
    PAL_RF_PIB_PHY_TX_ERR_BAD_FORMAT = 0x01A7,
    /* Transmission errors count due to timeout. 32 bits */
    PAL_RF_PIB_PHY_TX_ERR_TIMEOUT = 0x01A8,
    /* Transmission aborted count. 32 bits */
    PAL_RF_PIB_PHY_TX_ERR_ABORTED = 0x01A9,
    /* Transmission confirms not handled by upper layer count. 32 bits */
    PAL_RF_PIB_PHY_TX_CFM_NOT_HANDLED = 0x01AA,
    /* Successfully received messages count. 32 bits */
    PAL_RF_PIB_PHY_RX_TOTAL = 0x01B0,
    /* Successfully received bytes count. 32 bits */
    PAL_RF_PIB_PHY_RX_TOTAL_BYTES = 0x01B1,
    /* Reception errors count. 32 bits */
    PAL_RF_PIB_PHY_RX_ERR_TOTAL = 0x01B2,
    /* Reception false positive count. 32 bits */
    PAL_RF_PIB_PHY_RX_ERR_FALSE_POSITIVE = 0x01B3,
    /* Reception errors count due to bad message length. 32 bits */
    PAL_RF_PIB_PHY_RX_ERR_BAD_LEN = 0x01B4,
    /* Reception errors count due to bad format or bad FCS in header. 32 bits */
    PAL_RF_PIB_PHY_RX_ERR_BAD_FORMAT = 0x01B5,
    /* Reception errors count due to bad FCS in payload. 32 bits */
    PAL_RF_PIB_PHY_RX_ERR_BAD_FCS_PAY = 0x01B6,
    /* Reception aborted count. 32 bits */
    PAL_RF_PIB_PHY_RX_ERR_ABORTED = 0x01B7,
    /* Reception overrided (another message with higher signal level) count.
     * 32 bits */
    PAL_RF_PIB_PHY_RX_OVERRIDE = 0x01B8,
    /* Reception indications not handled by upper layer count. 32 bits */
    PAL_RF_PIB_PHY_RX_IND_NOT_HANDLED = 0x01B9,
    /* Reset Phy Statistics (write-only) */
    PAL_RF_PIB_PHY_STATS_RESET = 0x01C0,
    /* Set Continuous Tx Mode (write-only) */
    PAL_RF_PIB_SET_CONTINUOUS_TX_MODE = 0x01C1,
    /* Backoff period unit in us (aUnitBackoffPeriod in IEEE 802.15.4) used for
     * CSMA-CA. 16 bits (read-only) */
    PAL_RF_PIB_MAC_UNIT_BACKOFF_PERIOD = 0x0200,
    /* SUN FSK FEC enabled or disabled for transmission
     * (phyFskFecEnabled in IEEE 802.15.4; only available for RF215). 8 bits */
    PAL_RF_PIB_TX_FSK_FEC = 0x8000,
    /* SUN OFDM MCS (Modulation and coding scheme) used for transmission
     * (only available for RF215). 8 bits */
    PAL_RF_PIB_TX_OFDM_MCS = 0x8001,

} PAL_RF_PIB_ATTRIBUTE;

/* MISRA C-2012 deviation block end */

// *****************************************************************************
/* RF PAL PIB Result

  Summary:
    Result of a RF PAL PIB service client interface operation.

  Description:
    Identifies the result of RF PAL PIB service operations.

  Remarks:
    None.
*/
typedef enum
{
    PAL_RF_PIB_SUCCESS,
    PAL_RF_PIB_INVALID_PARAM,
    PAL_RF_PIB_INVALID_ATTR,
    PAL_RF_PIB_INVALID_HANDLE,
    PAL_RF_PIB_READ_ONLY,
    PAL_RF_PIB_WRITE_ONLY,
    PAL_RF_PIB_ERROR

} PAL_RF_PIB_RESULT;

// *****************************************************************************
/* RF PAL Handle

  Summary:
    Handle to a RF PAL instance.

  Description:
    This data type is a handle to a RF PAL instance. It can be used to access
    and control the RF PAL.

  Remarks:
    Do not rely on the underlying type as it may change in different versions
    or implementations of the RF PAL service.
*/
typedef uintptr_t PAL_RF_HANDLE;

// *****************************************************************************
/* Invalid RF PAL handle value to a RF PAL instance.

  Summary:
    Invalid handle value to a RF PAL instance.

  Description:
    Defines the invalid handle value to a RF PAL instance.

  Remarks:
    Do not rely on the actual value as it may change in different versions
    or implementations of the RF PAL service.
*/
#define PAL_RF_HANDLE_INVALID   ((PAL_RF_HANDLE) (-1))

// *****************************************************************************
/* RF PAL TX Handle

  Summary:
    Handle to a RF PAL transmission.

  Description:
    This data type is a handle to a RF PAL transmission. It is returned from
    PAL_RF_TxRequest and it can be used to cancel a requested transmission
    (in progress or not) using PAL_RF_TxCancel.

  Remarks:
    Do not rely on the underlying type as it may change in different versions
    or implementations of the RF PAL service.
*/
typedef uintptr_t PAL_RF_TX_HANDLE;

// *****************************************************************************
/* Invalid RF PAL TX Handle

  Summary:
    Invalid transmission handle.

  Description:
    Defines the invalid transmission handle value.

  Remarks:
    Do not rely on the actual value as it may change in different versions
    or implementations of the RF PAL service.
*/
#define PAL_RF_TX_HANDLE_INVALID ((PAL_RF_TX_HANDLE)(-1))

// *****************************************************************************
/* RF PAL Module Status

  Summary:
    Identifies the current status/state of the RF PAL module.

  Description:
    This enumeration identifies the current status/state of the RF PAL module.

  Remarks:
    This enumeration is the return type for the PAL_RF_Status routine. The
    upper layer must ensure that PAL_RF_Status returns PAL_RF_STATUS_READY
    before performing RF PAL operations.
*/
typedef enum
{
    PAL_RF_STATUS_UNINITIALIZED = SYS_STATUS_UNINITIALIZED,
    PAL_RF_STATUS_BUSY = SYS_STATUS_BUSY,
    PAL_RF_STATUS_READY = SYS_STATUS_READY,
    PAL_RF_STATUS_ERROR = SYS_STATUS_ERROR,
    PAL_RF_STATUS_INVALID_OBJECT = SYS_STATUS_ERROR_EXTENDED - 1,
} PAL_RF_STATUS;

// *****************************************************************************

// *****************************************************************************
/* RF PAL PHY TX Results

  Summary:
    Defines the possible RF PAL PHY transmission results.

  Description:
    This data type defines the list of possible RF PAL PHY transmission results,
    reported via PAL_RF_TxConfirm callback.

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
/* RF PAL TX Request Parameters

  Summary:
    Defines the parameters needed to request a transmission.

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
    uint8_t txPowerAttenuation;

    /* CSMA enable flag */
    bool csmaEnable;

} PAL_RF_TX_PARAMETERS;

// *****************************************************************************
/* RF PAL RX Indication Data

  Summary:
    Defines the data reported in RF PAL RX indication.

  Description:
    This data type defines the data reported in the RF PAL receive indication
    via PAL_RF_DataIndication callback.

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
/* RF PAL PIB data

  Summary:
    Defines the data used when requesting a RF PAL PIB attribute get/set.

  Description:
    This data type defines the data used when requesting a RF PAL PIB attribute
    get/set, using PAL_RF_GetRfPhyPib/PAL_RF_SetRfPhyPib.

  Remarks:
    PAL_RF_MAX_PIB_SIZE is the size in bytes of the largest PIB attribute.
*/

#define PAL_RF_MAX_PIB_SIZE     (32U)

typedef struct
{
    /* Data buffer where data is read/stored */
    uint8_t pData[PAL_RF_MAX_PIB_SIZE];

    /* RF Driver PIB Attribute to request */
    PAL_RF_PIB_ATTRIBUTE pib;

} PAL_RF_PIB_OBJ;

// *****************************************************************************
/* RF PAL RX Indication Callback

  Summary:
    Pointer to a RF PAL module receive indication callback function.

  Description:
    This data type defines the required function signature for the RF PAL
    receive indication event handling callback function. When
    PAL_RF_Initialize is called, a client must register a pointer whose
    function signature (parameter and return value types) matches the types
    specified by this function pointer in order to receive transfer related
    event calls back from the RF PAL.

  Parameters:
    pData       - Pointer to the buffer containing the data associated to the
                  receive indication.
    length      - PSDU length in bytes (including FCS).
    pParameters - Pointer to the parameters associated to the receive
                  indication.

  Returns:
    None.

  Example:
    <code>
    static void _rfDataIndication(uint8_t *pData, uint16_t length, PAL_RF_RX_PARAMETERS *pParameters)
    {

    }

    PAL_RF_INIT palRfInitData;
    SYS_MODULE_OBJ palRfObj;

    palRfInitData.rfPhyHandlers.palRfDataIndication = _rfDataIndication;
    palRfInitData.rfPhyHandlers.palRfTxConfirm = _rfTxConfirm;

    palRfObj = PAL_RF_Initialize(PAL_RF_PHY_INDEX, (const SYS_MODULE_INIT *) &palRfInitData);
    </code>

  Remarks:
    None.
*/
typedef void (*PAL_RF_DataIndication)(uint8_t *pData, uint16_t length, PAL_RF_RX_PARAMETERS *pParameters);

// *****************************************************************************
/* RF PAL TX Confirm Callback

  Summary:
    Pointer to a RF PAL module transmit confirm callback function.

  Description:
    This data type defines the required function signature for the RF PAL
    transmit confirm event handling callback function. When PAL_RF_Initialize
    is called, a client must register a pointer whose function signature
    (parameter and return value types) matches the types specified by this
    function pointer in order to receive transfer related event calls back from
    the RF PAL.

  Parameters:
    status      - Transmission result (see PAL_RF_PHY_STATUS).
    timeIni     - TX time (PPDU start), referred to system 64-bit time counter.
    timeEnd     - TX time (PPDU end), referred to system 64-bit time counter.

  Returns:
    None.

  Example:
    <code>
    static void _rfTxConfirm(PAL_RF_PHY_STATUS status, uint64_t timeIni, uint64_t timeEnd)
    {

    }

    PAL_RF_INIT palRfInitData;
    SYS_MODULE_OBJ palRfObj;

    palRfInitData.rfPhyHandlers.palRfDataIndication = _rfDataIndication;
    palRfInitData.rfPhyHandlers.palRfTxConfirm = _rfTxConfirm;

    palRfObj = PAL_RF_Initialize(PAL_RF_PHY_INDEX, (const SYS_MODULE_INIT *) &palRfInitData);
    </code>

  Remarks:
    None.
*/
typedef void (*PAL_RF_TxConfirm)(PAL_RF_PHY_STATUS status, uint64_t timeIni, uint64_t timeEnd);

// *****************************************************************************
/* RF PAL Handlers Data

  Summary:
    Defines the handlers required to manage the RF PAL module.

  Description:
    This data type defines the handlers required to manage the RF PAL module.

  Remarks:
    None.
*/
typedef struct
{
    PAL_RF_DataIndication palRfDataIndication;
    PAL_RF_TxConfirm palRfTxConfirm;
} PAL_RF_HANDLERS;

// *****************************************************************************
/* RF PAL Initialization Data

  Summary:
    Defines the data required to initialize the RF PAL module.

  Description:
    This data type defines the data required to initialize the RF PAL module,
    using PAL_RF_Initialize.

  Remarks:
    None.
*/
typedef struct
{
    PAL_RF_HANDLERS rfPhyHandlers;
} PAL_RF_INIT;

// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ PAL_RF_Initialize (
        const SYS_MODULE_INDEX index,
        const SYS_MODULE_INIT * const init
    )

  Summary:
    Initializes the RF PAL module.

  Description:
    This routine initializes the RF PAL module, making it ready for clients to
    use it. The initialization data is specified by the init parameter. It is a
    single instance module, so this function should be called only once (unless
    PAL_RF_Deinitialize is called).

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
    SYS_MODULE_OBJ palRfObj;

    palRfInitData.rfPhyHandlers.palRfDataIndication = _rfDataIndication;
    palRfInitData.rfPhyHandlers.palRfTxConfirm = _rfTxConfirm;

    palRfObj = PAL_RF_Initialize( PAL_RF_PHY_INDEX, (const SYS_MODULE_INIT *) &palRfInitData );
    </code>

  Remarks:
    This routine must be called before any other RF PAL routine is called.
*/
SYS_MODULE_OBJ PAL_RF_Initialize(const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init);

// *****************************************************************************
/* Function:
    PAL_RF_STATUS PAL_RF_Status ( SYS_MODULE_OBJ object )

  Summary:
    Returns status of the specific instance of the RF PAL module.

  Description:
    This function returns the status of the specific RF PAL module instance.

  Precondition:
    The PAL_RF_Initialize function should have been called before calling
    this function.

  Parameters:
    object    - RF PAL object handle, returned from PAL_RF_Initialize.

  Returns:
    PAL_RF_STATUS_READY - Indicates that the module is initialized and is
    ready to accept new requests from the client.

    PAL_RF_STATUS_BUSY - Indicates that the module is busy with a previous
    initialization request from the client.

    PAL_RF_STATUS_ERROR - Indicates that the module is in an error state.
    Any value lower than SYS_STATUS_ERROR is also an error state.

    PAL_RF_STATUS_UNINITIALIZED - Indicates that the module is not initialized.

  Example:
    <code>
    PAL_RF_STATUS palRfStatus;

    palRfStatus = PAL_RF_Status (object);
    if (palRfStatus == PAL_RF_STATUS_READY)
    {

    }
    </code>

  Remarks:
    The upper layer must ensure that PAL_RF_Status returns PAL_RF_STATUS_READY
    before performing RF PAL operations.
*/
PAL_RF_STATUS PAL_RF_Status(SYS_MODULE_OBJ object);

// *****************************************************************************
/* Function:
    PAL_RF_HANDLE PAL_RF_HandleGet(const SYS_MODULE_INDEX index)

  Summary:
    Returns a handle to the requested RF PAL instance.

  Description:
    This function returns a handle to the requested RF PAL instance.

  Preconditions:
    The PAL_RF_Initialize function should have been called before calling
    this function.

  Parameters:
    index - Index of the RF PAL instance. Only one instance (index 0)
            supported.

  Returns:
    Handle (PAL_RF_HANDLE type) to the requested RF PAL instance.
    PAL_RF_HANDLE_INVALID is returned if the index parameter is invalid.

  Example:
    <code>
    PAL_RF_HANDLE myPalRfHandle;
    myPalRfHandle = PAL_RF_HandleGet(PAL_RF_INDEX_0);

    if (myPalRfHandle != PAL_RF_HANDLE_INVALID)
    {

    }
    </code>

  Remarks:
    The handle returned by this function is needed to call other functions of
    the RF PAL module.
*/
PAL_RF_HANDLE PAL_RF_HandleGet(const SYS_MODULE_INDEX index);

// *****************************************************************************
/* Function:
    void PAL_RF_Deinitialize(SYS_MODULE_OBJ object)

  Summary:
    De-initializes the specified instance of the RF PAL module.

  Description:
    This function de-initializes the specified instance of the RF PAL module,
    disabling its operation and invalidates all of the internal data.

  Precondition:
    Function PAL_RF_Initialize must have been called before calling this
    routine and a valid SYS_MODULE_OBJ must have been returned.

  Parameters:
    object       - RF PAL module object handle, returned by PAL_RF_Initialize.

  Returns:
    None.

  Example:
    <code>
    SYS_MODULE_OBJ palRfobj;

    PAL_RF_Deinitialize(palRfobj);
    </code>

  Remarks:
    Once PAL_RF_Initialize has been called, PAL_RF_Deinitialize must be
    called before PAL_RF_Initialize can be called again.
*/
void PAL_RF_Deinitialize(SYS_MODULE_OBJ object);

//***************************************************************************
/* Function:
    void PAL_RF_Tasks( void )

  Summary:
    Maintains the RF PAL's state machine.

  Description:
    This function is used to maintain the RF PAL's internal state machine.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    while (true)
    {
        PAL_RF_Tasks();
    }
    </code>

  Remarks:
    - This function is normally not called directly by an application. It is
      called by the system's Tasks routine (SYS_Tasks).
    - This function will never block or access any resources that may cause
      it to block.
*/
void PAL_RF_Tasks( void );

// *****************************************************************************
/* Function:
    PAL_RF_TX_HANDLE PAL_RF_TxRequest(PAL_RF_HANDLE handle, uint8_t *pData,
        uint16_t length, PAL_RF_TX_PARAMETERS *txParameters)

  Summary:
    Allows a client to transmit data through RF device.

  Description:
    This routine sends a new data message through RF using the RF PAL module.

  Precondition:
    PAL_RF_HandleGet must have been called to obtain a valid RF PAL handle.

  Parameters:
    handle -       A valid handle, returned from PAL_RF_HandleGet.

    pData -        Pointer to the data to transmit.

    length -       Length of the data to transmit in bytes.

    txParameters - Pointer to parameters of the transmission.

  Returns:
    Handle (PAL_RF_TX_HANDLE type) to the requested transmission.
    PAL_RF_TX_HANDLE_INVALID is returned if there was an error in the
    transmission request.

  Example:
    <code>
    uint8_t txData[128];
    PAL_RF_TX_PARAMETERS txParameters;
    uint16_t length = 128;

    txParameters.timeCount = SYS_TIME_Counter64Get() + SYS_TIME_USToCount(5000);
    txParameters.txPowerAttenuation = 0;
    txParameters.csmaEnable = true;

    PAL_RF_TxRequest(palRfHandle, txData, length, &txParameters);
    </ code>

  Remarks:
    The handle returned by this function is needed to cancel the transmission
    with PAL_RF_TxCancel.
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
    PAL_RF_HandleGet must have been called to obtain a valid RF PAL handle.
    PAL_RF_TxRequest must have been called to obtain a valid transmission
    handle.

  Parameters:
    handle   - A valid handle, returned from PAL_RF_HandleGet.
    txHandle - A valid transmission handle, returned from PAL_RF_TxRequest.

  Returns:
    None.

  Example:
    <code>
    PAL_RF_HANDLE palRfHandle;
    PAL_RF_TX_HANDLE txReqHandle;

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
    Allows a client to reset the RF PAL module.

  Description:
    This routine performs a reset of the RF device and clears the RF PHY
    statistics.

  Precondition:
    PAL_RF_HandleGet must have been called to obtain a valid RF PAL handle.

  Parameters:
    handle -       A valid handle, returned from PAL_RF_HandleGet.

  Returns:
    None.

  Example:
    <code>
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
    Gets value of RF PHY PIB attribute.

  Description:
    This routine allows a client to get the value of a RF PHY Information Base
    (PIB) attribute from the RF PHY layer.

  Precondition:
    PAL_RF_HandleGet must have been called to obtain a valid RF PAL handle.

  Parameters:
    handle - A valid handle, returned from PAL_RF_HandleGet.
    pibObj - Pointer to PIB object to indicate the PIB attribute to read. PIB
             object includes a data buffer where the read value is stored.

  Returns:
    Result of getting the PIB (see PAL_RF_PIB_RESULT).

  Example:
    <code>
    PAL_RF_HANDLE palRfHandle;
    PAL_RF_PIB_OBJ pibObj;

    pibObj.pib = PAL_RF_PIB_PHY_FW_VERSION;

    if (PAL_RF_GetRfPhyPib(palRfHandle, &pibObj) == PAL_RF_PIB_SUCCESS)
    {
        uint8_t size = PAL_RF_GetRfPhyPibLength(palRfHandle, PAL_RF_PIB_PHY_FW_VERSION);
    }
    </code>

  Remarks:
    None.
*/
PAL_RF_PIB_RESULT PAL_RF_GetRfPhyPib(PAL_RF_HANDLE handle, PAL_RF_PIB_OBJ *pibObj);

// *****************************************************************************
/* Function:
    PAL_RF_PIB_RESULT PAL_RF_SetRfPhyPib (PAL_RF_HANDLE handle, PAL_RF_PIB_OBJ *pibObj)

  Summary:
    Sets value of RF PHY PIB attribute.

  Description:
    This routine allows a client to set the value of a RF PHY Information Base
    (PIB) attribute in the RF PHY layer.

  Precondition:
    PAL_RF_HandleGet must have been called to obtain a valid RF PAL handle.

  Parameters:
    handle - A valid handle, returned from PAL_RF_HandleGet.
    pibObj - Pointer to PIB object to indicate the PIB attribute to write. PIB
             object includes a data buffer with the new value to write.

  Returns:
    Result of setting the PIB (see PAL_RF_PIB_RESULT).

  Example:
    <code>
    PAL_RF_HANDLE palRfHandle;
    PAL_RF_PIB_OBJ pibObj;

    pibObj.pib = PAL_RF_PIB_PHY_CHANNEL_NUM;
    pibObj.pData[0] = 1;

    if (PAL_RF_SetRfPhyPib(palRfHandle, &pibObj) == PAL_RF_PIB_SUCCESS)
    {

    }
    </code>

  Remarks:
    None.
*/
PAL_RF_PIB_RESULT PAL_RF_SetRfPhyPib(PAL_RF_HANDLE handle, PAL_RF_PIB_OBJ *pibObj);

// *****************************************************************************
/* Function:
    uint8_t PAL_RF_GetRfPhyPibLength(PAL_RF_HANDLE handle, PAL_RF_PIB_ATTRIBUTE attribute)

  Summary:
    Gets size of PIB attribute.

  Description:
    This routine allows to get the size in bytes of a RF PHY Information Base
    (PIB) attribute of the RF PHY layer.

  Precondition:
    PAL_RF_HandleGet must have been called to obtain a valid RF PAL handle.

  Parameters:
    handle    - A valid handle, returned from PAL_RF_HandleGet.
    attribute - A valid PIB attribute (see PAL_RF_PIB_ATTRIBUTE).

  Returns:
    Size of PIB attribute in bytes. 0 if invalid PIB attribute.

  Example:
    <code>
    PAL_RF_HANDLE palRfHandle;
    uint8_t pibSize;

    pibSize = PAL_RF_GetRfPhyPibLength(palRfHandle, PAL_RF_PIB_PHY_CONFIG);
    </code>

  Remarks:
    None.
*/
uint8_t PAL_RF_GetRfPhyPibLength(PAL_RF_HANDLE handle, PAL_RF_PIB_ATTRIBUTE attribute);

#endif // #ifndef PAL_RF_H

/*******************************************************************************
 End of File
*/
