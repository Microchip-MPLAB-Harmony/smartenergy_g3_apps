/***********************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    drv_mac_g3adp.h

  Summary:
    G3 ADP MAC device driver interface file

  Description:
    G3 ADP MAC Device Driver Interface.

    The G3 ADP MAC device driver provides a simple interface to manage
    the G3 ADP stack. This file defines the interface definitions
    and prototypes for the G3 ADP MAC driver.
  ***********************************************************************/

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

#ifndef DRV_MAC_G3ADP_H
#define DRV_MAC_G3ADP_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "configuration.h"
#include "driver/driver_common.h"

#include "tcpip/tcpip_mac.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Macro Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* G3 ADP MAC Driver Module Index Count

  Summary:
    Number of valid G3 ADP MAC driver index.

  Description:
    This constant identifies number of valid G3 ADP MAC driver indices.

  Remarks:
    This constant should be used in place of hard-coded numeric literals.

    This value is derived from part-specific header files defined as part of the
    peripheral libraries.
*/

#define DRV_G3ADP_MAC_INDEX_COUNT  GMAC_NUMBER_OF_MODULES

// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines - Client Level
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ DRV_G3ADP_MAC_Initialize(const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init);

  Summary:
    Initializes the G3 ADP MAC.

  Description:
    This function supports the initialization of the G3 ADP MAC.  Used by tcpip_module_manager.

  Precondition:
    None

  Parameters:
    - index     - Index of the G3 ADP MAC driver to be initialized
    - init      - Pointer to TCPIP_MAC_INIT initialization data containing: moduleInit, macControl,moduleData.
                  moduleInit and moduleData are not used in this implementation.

  Returns:
    - If successful, returns a valid handle to a module instance object. Otherwise, returns _SYS_MODULE_OBJ_INVALID_.

  Example:
    <code>
    TCPIP_MAC_INIT macInit =
    {
        { 0 }, // SYS_MODULE_INIT not currently used
        &macCtrl,
        macConfig,
    };

    pNetIf->macObjHandle = DRV_G3ADP_MAC_Initialize(pMacObj->macId, &macInit.moduleInit);

    if( pNetIf->macObjHandle == SYS_MODULE_OBJ_INVALID)
    {
        pNetIf->macObjHandle = 0;
        SYS_ERROR_PRINT(SYS_ERROR_ERROR, TCPIP_STACK_HDR_MESSAGE "%s MAC initialization failed\r\n", pMacObj->macName);
        netUpFail = 1;
        break;
    }
    </code>

  Remarks:
    This routine must be called before any other G3 ADP MAC routine is called.

*/
SYS_MODULE_OBJ DRV_G3ADP_MAC_Initialize(const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init);

// *****************************************************************************
/* Function:
    void DRV_G3ADP_MAC_Deinitialize(SYS_MODULE_OBJ object);

  Summary:
    Deinitializes the G3 ADP MAC.

  Description:
    This function supports teardown of the G3 ADP MAC (opposite of set up).  Used by tcpip_module_manager.

  Precondition:
    DRV_G3ADP_MAC_Initialize must have been called to set up the driver.

  Parameters:
    - object    - Driver object handle, returned from DRV_G3ADP_MAC_Initialize

  Returns:
     None.

  Example:
    <code>
    if(pNetIf->macObjHandle != 0)
    {
        DRV_G3ADP_MAC_Deinitialize(pNetIf->macObjHandle);
    }
    </code>

  Remarks:
    It should be called to release any resources allocated by the initialization
    and disables callback functions with the G3 ADP layer.

*/
void DRV_G3ADP_MAC_Deinitialize(SYS_MODULE_OBJ object);

// *****************************************************************************
/* Function:
    SYS_STATUS DRV_G3ADP_MAC_Status ( SYS_MODULE_OBJ object )

  Summary:
    Provides the current status of the G3 ADP MAC driver.

  Description:
    This function provides the current status of the MAC driver
    module.

  Precondition:
    The DRV_G3ADP_MAC_Initialize function must have been called before calling
    this function.

  Parameters:
    object -  Driver object handle, returned from DRV_G3ADP_MAC_Initialize

  Returns:
    - SYS_STATUS_READY  - Indicates that any previous module operation for the
                          specified module has completed
    - SYS_STATUS_BUSY   - Indicates that a previous module operation for the
                          specified module has not yet completed
    - SYS_STATUS_ERROR  - Indicates that the specified module is in an error state

  Example:
    <code>
    SYS_STATUS macStat = DRV_G3ADP_MAC_Status(pNetIf->macObjHandle);
    if(macStat < 0)
    {   // failed; kill the interface
        TCPIP_STACK_BringNetDown(&tcpip_stack_ctrl_data, pNetIf, TCPIP_STACK_ACTION_IF_DOWN, TCPIP_MAC_POWER_DOWN);
        pNetIf->Flags.bMacInitDone = true;
    }
    else if(macStat == SYS_STATUS_READY)
    {   // get the MAC address and MAC processing flags
        ...
    }
    </code>

  Remarks:
    None.
*/
SYS_STATUS DRV_G3ADP_MAC_Status ( SYS_MODULE_OBJ object );

// *****************************************************************************
/* Function:
    void DRV_G3ADP_MAC_Tasks( SYS_MODULE_OBJ object )

  Summary:
    Maintains the G3 ADP MAC driver's state machine.

  Description:
    This function is used to maintain the driver's internal state machine

  Precondition:
    The DRV_G3ADP_MAC_Initialize routine must have been called for the
    specified MAC driver instance.

  Parameters:
    - object -  Object handle for the specified driver instance (returned from
                DRV_G3ADP_MAC_Initialize)
  Returns:
    None

  Example:
    <code>
    if(pNetIf->macObjHandle != 0)
    {
        // process the underlying MAC module tasks
        DRV_G3ADP_MAC_Tasks(pNetIf->macObjHandle);
    }
    </code>

  Remarks:
    None.

*/
void DRV_G3ADP_MAC_Tasks( SYS_MODULE_OBJ object );

// *****************************************************************************
/* Function:
    DRV_HANDLE DRV_G3ADP_MAC_Open(const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT intent);

  Summary:
    Opens a client instance of the G3 ADP MAC Driver. This driver is single instance.

  Description:
    This function opens a client instance of the G3 ADP MAC Driver.
    Used by tcpip_module_manager.

  Precondition:
    DRV_G3ADP_MAC_Initialize() should have been called.

  Parameters:
    - index      - identifier for the driver instance to be opened.
    - intent     - Zero or more of the values from the enumeration
                   DRV_IO_INTENT ORed together to indicate the intended use
                   of the driver

  Return:
    - DRV_HANDLE - handle (pointer) to MAC client
    - 0 if call failed

  Example:
    <code>
    pNetIf->hIfMac = DRV_G3ADP_MAC_Open(pMacObj->macId, DRV_IO_INTENT_READWRITE);
    if(pNetIf->hIfMac == DRV_HANDLE_INVALID)
    {
        pNetIf->hIfMac = 0;
        pNetIf->macObjHandle = 0;
        SYS_ERROR_PRINT(SYS_ERROR_ERROR, TCPIP_STACK_HDR_MESSAGE "%s MAC Open failed\r\n", pMacObj->macName);
        netUpFail = 1;
        break;
    }
    </code>

  Remarks:
    The intent parameter is not used in the current implementation and is maintained only for compatibility
    with the generic driver Open function signature.
*/
DRV_HANDLE DRV_G3ADP_MAC_Open(const SYS_MODULE_INDEX index, const DRV_IO_INTENT intent);


// *****************************************************************************
/* Function:
    void DRV_G3ADP_MAC_Close( DRV_HANDLE hMac )

  Summary:
    Closes a client instance of the G3 ADP MAC Driver.

  Description:
    This function closes a client instance of the G3 ADP MAC Driver.

  Precondition:
    DRV_G3ADP_MAC_Open() should have been called.

  Parameters:
    hMac -  valid MAC handle, obtained by a call to DRV_G3ADP_MAC_Open

  Return:
    None

  Example:
    <code>
    if(pNetIf->hIfMac != 0)
    {
        DRV_G3ADP_MAC_Close(pNetIf->hIfMac);
    }

    </code>
  Remarks:
    None
*/
void DRV_G3ADP_MAC_Close( DRV_HANDLE hMac );

// *****************************************************************************
/* Function:
    bool DRV_G3ADP_MAC_LinkCheck( DRV_HANDLE hMac )

  Summary:
    Checks current link status.

  Description:
    This function checks the link status of the associated network interface.

  Precondition:
    DRV_G3ADP_MAC_Initialize must have been called to set up the driver.
    DRV_G3ADP_MAC_Open() should have been called to obtain a valid handle.

  Parameters:
    hMac - MAC client handle

  Returns:
    - true  - If the link is up
    - false - If the link is not up

  Example:
<code>
    bool linkCurr;
    bool linkStatusChanged = false;

    linkCurr = DRV_G3ADP_MAC_LinkCheck(pNetIf->hIfMac);
    if(linkPrev != linkCurr)
    {
        linkStatusChanged = true;
        linkPrev = linkCurr;

    }
</code>

  Remarks:
    None.

*/
bool DRV_G3ADP_MAC_LinkCheck( DRV_HANDLE hMac );

// *****************************************************************************
/* Function:
    bool DRV_G3ADP_MAC_PowerMode( DRV_HANDLE hMac, TCPIP_MAC_POWER_MODE pwrMode )

  Summary:
    Selects the current power mode for the MAC.

  Description:
    This function has not been implemented.

  Precondition:
    DRV_G3ADP_MAC_Initialize must have been called to set up the driver.
    DRV_G3ADP_MAC_Open() should have been called to obtain a valid handle.

  Parameters:
    - hMac      - Handle identifying the MAC driver client
    - pwrMode   - required power mode

  Returns:
    Current implementation always returns true.

  Remarks:
    None.
*/
bool DRV_G3ADP_MAC_PowerMode( DRV_HANDLE hMac, TCPIP_MAC_POWER_MODE pwrMode );

// *****************************************************************************
/* Function:
    TCPIP_MAC_RES DRV_G3ADP_MAC_RxFilterHashTableEntrySet(DRV_HANDLE hMac, const TCPIP_MAC_ADDR* DestMACAddr)

  Summary:
    Sets the current MAC hash table receive filter. Not implemented.

  Description:
    This function has not been implemented.

  Precondition:
    DRV_G3ADP_MAC_Initialize() should have been called.
    DRV_G3ADP_MAC_Open() should have been called to obtain a valid handle.

  Parameters:
    - hMac        - Handle identifying the MAC driver client
    - DestMACAddr - destination MAC address (6 bytes) to allow
                    through the Hash Table Filter.
                    If DestMACAddr is set to 00-00-00-00-00-00,
                    then the hash table will be cleared of all entries
                    and the filter will be disabled.
  Return:
    - Current implementation always returns TCPIP_MAC_RES_OK.

  Remarks:
    None.

*/
TCPIP_MAC_RES DRV_G3ADP_MAC_RxFilterHashTableEntrySet(DRV_HANDLE hMac, const TCPIP_MAC_ADDR* DestMACAddr);

// *****************************************************************************
/* Function:
    TCPIP_MAC_RES DRV_G3ADP_MAC_PacketTx(DRV_HANDLE hMac, TCPIP_MAC_PACKET * ptrPacket);

  Summary:
    MAC driver transmit function.

  Description:
    This is the MAC transmit function.
    Using this function a packet is submitted to the G3 ADP stack for transmission.

  Precondition:
    DRV_G3ADP_MAC_Initialize() should have been called.
    DRV_G3ADP_MAC_Open() should have been called to obtain a valid handle.

  Parameters:
    - hMac      - Handle identifying the MAC driver client
    - ptrPacket - Pointer to a TCPIP_MAC_PACKET that's completely formatted
                  and ready to be transmitted over the network

  Return:
    - TCPIP_MAC_RES_OK if success
    - a TCPIP_MAC_RES error value if failed

  Example:
    <code>
    TCPIP_MAC_RES res;
    TCPIP_MAC_PACKET * ptrPacket;

    // The content of the packet must be built accordingly to TCPIP_MAC_PACKET
    if(pNetIf->hIfMac != 0)
    {
        res = DRV_G3ADP_MAC_PacketTx(pNetIf->hIfMac, ptrPacket);
    }

    </code>

  Remarks:
    - The G3 ADP MAC driver supports internal queuing.
      A packet is rejected only if it's not properly formatted.
      Otherwise it will be scheduled for transmission and queued internally if needed.

    - Once the packet is scheduled for transmission the G3 ADP MAC driver will set
      the TCPIP_MAC_PKT_FLAG_QUEUED flag so that the stack is aware that this
      packet is under processing and cannot be modified.

    - Once the packet is transmitted, the TCPIP_MAC_PKT_FLAG_QUEUED will be
      cleared, the proper packet acknowledgment result (ackRes) will be
      set and the packet acknowledgment function (ackFunc) will be called.

*/
TCPIP_MAC_RES DRV_G3ADP_MAC_PacketTx(DRV_HANDLE hMac, TCPIP_MAC_PACKET * ptrPacket);

// *****************************************************************************
/* Function:
    TCPIP_MAC_PACKET* DRV_G3ADP_MAC_PacketRx (DRV_HANDLE hMac, TCPIP_MAC_RES* pRes, TCPIP_MAC_PACKET_RX_STAT* pPktStat);

  Summary:
    This is the G3 ADP MAC receive function.

  Description:
    This function will return a packet if such a pending packet exists.

    Additional information about the packet is available by providing the pRes and
    pPktStat fields.

  Precondition:
    DRV_G3ADP_MAC_Initialize() should have been called.
    DRV_G3ADP_MAC_Open() should have been called to obtain a valid handle.

  Parameters:
    - hMac        - Handle identifying the MAC driver client
    - pRes        - optional pointer to an address that will receive an additional
                    result associated with the operation.
                    Can be 0 if not needed.
    - pPktStat    - optional pointer to an address where to copy the received packet status.
                    Can be 0 if not needed.


  Returns:
    - a valid pointer to an available RX packet
    - 0 if no packet pending/available

  Example:
<code>
    TCPIP_MAC_PACKET* pRxPkt;

    // get all the new MAC packets
    while((pRxPkt = DRV_G3ADP_MAC_PacketRx(pNetIf->hIfMac, 0, 0)) != 0)
    {
        // process or queue RX packet
    }
</code>

  Remarks:
    - Once a pending packet is available in the MAC driver internal RX queues
      this function will dequeue the packet and hand it over to the
      G3 ADP MAC driver's client - i.e., the TCPIP stack - for further processing.

    - The G3 ADP MAC driver dequeues and return to the caller just one single packet.
      That is the packets are not chained.

    - The packet buffers are allocated by the G3 ADP MAC driver itself,
      Once the higher level layers in the stack are done with processing the RX packet,
      they have to call the corresponding packet acknowledgment function
      that tells the MAC driver that it can resume control of that packet.

    - Once the stack modules are done processing the RX packets and the acknowledge function is called
      the MAC driver will reuse the RX packets.

    - The G3 ADP MAC driver may use the DRV_G3ADP_MAC_Process() for obtaining new RX packets if needed.

*/
TCPIP_MAC_PACKET* DRV_G3ADP_MAC_PacketRx (DRV_HANDLE hMac, TCPIP_MAC_RES* pRes, TCPIP_MAC_PACKET_RX_STAT* pPktStat);

// *****************************************************************************
/* Function:
    TCPIP_MAC_RES DRV_G3ADP_MAC_Process(DRV_HANDLE hMac);

  Summary:
    MAC periodic processing function.

  Description:
    This is a function that allows for internal processing by the MAC
    driver. It is meant for processing that cannot be done from within ISR.

    Normally this function will be called in response to an TX and/or RX
    event signaled by the driver. This is specified by the MAC driver at
    initialization time using TCPIP_MAC_MODULE_CTRL.

  Precondition:
    DRV_G3ADP_MAC_Initialize() should have been called.
    DRV_G3ADP_MAC_Open() should have been called to obtain a valid handle.

  Parameters:
    hMac -  MAC client handle

  Return:
    - TCPIP_MAC_RES_OK if all processing went on OK
    - a TCPIP_MAC_RES error code if processing failed for some reason

  Example:
    <code>
    if(pNetIf->Flags.bMacProcessOnEvent != 0)
    {   // MAC internal processing
        DRV_G3ADP_MAC_Process(pNetIf->hIfMac);
    }
    </code>

  Remarks:
    - Upper layers may use the DRV_G3ADP_MAC_Process() for processing its pending TX queues

*/
TCPIP_MAC_RES DRV_G3ADP_MAC_Process(DRV_HANDLE hMac);

// *****************************************************************************
/* Function:
    TCPIP_MAC_RES DRV_G3ADP_MAC_StatisticsGet(DRV_HANDLE hMac, TCPIP_MAC_RX_STATISTICS* pRxStatistics, TCPIP_MAC_TX_STATISTICS* pTxStatistics);

  Summary:
    Gets the current G3 ADP MAC statistics.

  Description:
    This function will get the current value of the statistic counters
    maintained by the G3 ADP MAC driver.


  Precondition:
   DRV_G3ADP_MAC_Initialize() should have been called.
   DRV_G3ADP_MAC_Open() should have been called to obtain a valid handle.

  Parameters:
    - hMac          - Handle identifying the MAC driver client

    - pRxStatistics - pointer to a TCPIP_MAC_RX_STATISTICS that will receive the current
                      RX statistics counters
                      Can be NULL if not needed

    - pTxStatistics - pointer to a TCPIP_MAC_TX_STATISTICS that will receive the current
                      TX statistics counters
                      Can be NULL if not needed

  Returns:
    - TCPIP_MAC_RES_OK if all processing went on OK.
    - TCPIP_MAC_RES_OP_ERR error code if function not supported by the driver.

  Example:
    <code>
    TCPIP_MAC_RX_STATISTICS rxStatistics;
    TCPIP_MAC_TX_STATISTICS txStatistics;

    TCPIP_MAC_RES res = DRV_G3ADP_MAC_StatisticsGet(pNetIf->hIfMac, &rxStatistics, &txStatistics);
    </code>

  Remarks:
    - The reported values are info only and change dynamically.

*/
TCPIP_MAC_RES DRV_G3ADP_MAC_StatisticsGet(DRV_HANDLE hMac, TCPIP_MAC_RX_STATISTICS* pRxStatistics, TCPIP_MAC_TX_STATISTICS* pTxStatistics);

// *****************************************************************************
/* MAC Parameter Get function
    TCPIP_MAC_RES DRV_G3ADP_MAC_ParametersGet(DRV_HANDLE hMac, TCPIP_MAC_PARAMETERS* pMacParams);

  Summary:
    G3 ADP MAC parameter get function.

  Description:
    This function returns the run time parameters of the G3 ADP MAC driver.

  Precondition:
   DRV_G3ADP_MAC_Initialize() should have been called.
   DRV_G3ADP_MAC_Open() should have been called to obtain a valid handle.

  Parameters:
    - hMac        - Handle identifying the G3 ADPMAC driver client
    - pMacParams  - Address to store the MAC parameters

  Returns:
    - TCPIP_MAC_RES_OK if pMacParams updated properly
    - a TCPIP_MAC_RES error code if processing failed for some reason

  Example:
    <code>
    SYS_STATUS macStat = DRV_G3ADP_MAC_Status(pNetIf->macObjHandle);
    if(macStat == SYS_STATUS_READY)
    {   // get the MAC address and MAC processing flags
        // set the default MTU; MAC driver will override if needed
        TCPIP_MAC_PARAMETERS macParams = {{{0}}};

        macParams.linkMtu = TCPIP_MAC_LINK_MTU_DEFAULT;
        DRV_G3ADP_MAC_ParametersGet(pNetIf->hIfMac, &macParams);
        memcpy(pNetIf->netMACAddr.v, macParams.ifPhyAddress.v, sizeof(pNetIf->netMACAddr));
        pNetIf->Flags.bMacProcessOnEvent = macParams.processFlags != TCPIP_MAC_PROCESS_FLAG_NONE;
        pNetIf->linkMtu = macParams.linkMtu;

        // enable the interface
        pNetIf->Flags.bInterfaceEnabled = true;
        pNetIf->Flags.bMacInitialize = false;
        pNetIf->Flags.bMacInitDone = true;
    }
    </code>

  Remarks:
    None.

*/
TCPIP_MAC_RES DRV_G3ADP_MAC_ParametersGet(DRV_HANDLE hMac, TCPIP_MAC_PARAMETERS* pMacParams);

// *****************************************************************************
/* Function:
    TCPIP_MAC_RES DRV_G3ADP_MAC_RegisterStatisticsGet(DRV_HANDLE hMac, TCPIP_MAC_STATISTICS_REG_ENTRY* pRegEntries, int nEntries, int* pHwEntries);

  Summary:
    Gets the current MAC hardware statistics registers.

  Description:
    This function will get the current value of the statistic registers
    of the associated MAC controller.


  Precondition:
   DRV_G3ADP_MAC_Initialize() should have been called.
   DRV_G3ADP_MAC_Open() should have been called to obtain a valid handle.

  Parameters:
    - hMac          - Handle identifying the MAC driver client

    - pRegEntries   - pointer to an array of TCPIP_MAC_STATISTICS_REG_ENTRY that will receive the current
                      hardware statistics registers
                      Can be NULL if not needed

    - nEntries      - number of TCPIP_MAC_STATISTICS_REG_ENTRY in the array.

    - pHwEntries    - address to store the number of hardware supported registers by the associated MAC
                      Can be NULL if not needed

  Returns:
    - TCPIP_MAC_RES_OK if all processing went on OK.
    - TCPIP_MAC_RES_OP_ERR error code if function not supported by the driver.

  Example:
    <code>
    </code>

  Remarks:
    - The reported values are info only and change dynamically.

*/
TCPIP_MAC_RES DRV_G3ADP_MAC_RegisterStatisticsGet(DRV_HANDLE hMac, TCPIP_MAC_STATISTICS_REG_ENTRY* pRegEntries, int nEntries, int* pHwEntries);

// *****************************************************************************
/* Function:
    size_t DRV_G3ADP_MAC_ConfigGet(DRV_HANDLE hMac, void* configBuff, size_t buffSize, size_t* pConfigSize);

  Summary:
    G3 ADP MAC driver does not required any additional configuration.

  Description:
    This function is not necessary, it only satisfies the compatibility with tcpip manager's API.

  Precondition:
   DRV_G3ADP_MAC_Initialize() should have been called.
   DRV_G3ADP_MAC_Open() should have been called to obtain a valid handle.

  Parameters:
    - hMac          - Handle identifying the MAC driver client

    - configBuff    - pointer to a buffer to store the configuration
                      Can be NULL if not needed

    - buffSize      - size of the supplied buffer

    - pConfigSize   - address to store the number of bytes needed for the storage of the MAC configuration.
                      Can be NULL if not needed

  Returns:
    - Number of bytes copied into the supplied storage buffer. This implementation returns 0 bytes.

  Remarks:
    - None

*/
size_t DRV_G3ADP_MAC_ConfigGet(DRV_HANDLE hMac, void* configBuff, size_t buffSize, size_t* pConfigSize);

// *****************************************************************************
/* Function:
    bool DRV_G3ADP_MAC_EventMaskSet(DRV_HANDLE hMac, TCPIP_MAC_EVENT macEvMask, bool enable);

  Summary:
    Enables/disables the G3 ADP MAC events.

  Description:
    This is a function that enables or disables the events to be reported
    to the MAC client (TCP/IP stack).

     All events that are to be enabled will be added to the notification process.
     All events that are to be disabled will be removed from the notification process.
     The stack has to catch the events that are notified and process them.
     After that the stack should call DRV_G3ADP_MAC_EventAcknowledge()
     so that the events can be re-enable

     The stack should process at least the following transfer events:
        - TCPIP_MAC_EV_RX_PKTPEND
        - TCPIP_MAC_EV_RX_DONE
        - TCPIP_MAC_EV_TX_DONE

  Precondition:
    DRV_G3ADP_MAC_Initialize() should have been called.
    DRV_G3ADP_MAC_Open() should have been called to obtain a valid handle.

  Parameters:
    - hMac      - Handle identifying the MAC driver client
    - macEvMask - events the user of the stack wants to add/delete for
                  notification
    - enable    - if true, the events will be enabled, else disabled

  Return:
    always true, operation succeeded.

  Example:
    <code>
        DRV_G3ADP_MAC_EventMaskSet( hMac, TCPIP_MAC_EV_RX_OVFLOW | TCPIP_MAC_EV_RX_BUFNA, true );
    </code>

  Remarks:
    - The event notification system enables the user of the TCP/IP stack to
      call into the stack for processing only when there are relevant events
      rather than being forced to periodically call from within a loop.
*/
bool DRV_G3ADP_MAC_EventMaskSet(DRV_HANDLE hMac, TCPIP_MAC_EVENT macEvMask, bool enable);

// *****************************************************************************
/* Function:
    bool DRV_G3ADP_MAC_EventAcknowledge(DRV_HANDLE hMac, TCPIP_MAC_EVENT tcpAckEv);

  Summary:
    Acknowledges and re-enables processed events.

  Description:
    This function acknowledges and re-enables processed events. Multiple
    events can be ORed together as they are processed together. The events
    acknowledged by this function should be the events that have been
    retrieved from the stack by calling DRV_G3ADP_MAC_EventPendingGet()
    or have been passed to the stack by the driver using the registered notification handler
    and have been processed and have to be re-enabled.

  Precondition:
    DRV_G3ADP_MAC_Initialize() should have been called.
    DRV_G3ADP_MAC_Open() should have been called to obtain a valid handle.

  Parameters:
    - hMac      - Handle identifying the MAC driver client
    - tcpAckEv  - the events that the user processed and need to be re-enabled

  Return:
    - true if events acknowledged
    - false if no events to be acknowledged

  Example:
    <code>
    TCPIP_MAC_EVENT activeEvents;

    // Get Pending events
    activeEvents = DRV_G3ADP_MAC_EventPendingGet(pNetIf->hIfMac);
    // Process events
    (...)
    // Acknowledge events
    DRV_G3ADP_MAC_EventAcknowledge( hMac, activeEvents );
    </code>

  Remarks:
    - All events should be acknowledged, in order to be re-enabled.

    - Some events are fatal errors and should not be acknowledged
      ( TCPIP_MAC_EV_RX_BUSERR, TCPIP_MAC_EV_TX_BUSERR).
      Driver/stack re-initialization is needed under such circumstances.

    - Some events are just system/application behavior and they are intended
      only as simple info (TCPIP_MAC_EV_RX_OVFLOW, TCPIP_MAC_EV_RX_BUFNA,
      TCPIP_MAC_EV_TX_ABORT, TCPIP_MAC_EV_RX_ACT).

    - The events are persistent. They shouldn't be re-enabled unless they
      have been processed and the condition that generated them was removed.
      Re-enabling them immediately without proper processing will have
      dramatic effects on system performance.
*/
bool DRV_G3ADP_MAC_EventAcknowledge(DRV_HANDLE hMac, TCPIP_MAC_EVENT tcpAckEv);

// *****************************************************************************
/* Function:
    TCPIP_MAC_EVENT DRV_G3ADP_MAC_EventPendingGet(DRV_HANDLE hMac)

  Summary:
    Returns the currently pending events.

  Description:
    This function returns the currently pending G3 ADP MAC events.
    Multiple events will be ORed together as they accumulate.
    The stack should perform processing whenever a transmission related
    event (TCPIP_MAC_EV_RX_PKTPEND, TCPIP_MAC_EV_TX_DONE) is present.
    The other, non critical events, may not be managed by the stack and passed to an
    user. They will have to be eventually acknowledged if re-enabling is
    needed.

  Precondition:
    DRV_G3ADP_MAC_Initialize() should have been called.
    DRV_G3ADP_MAC_Open() should have been called to obtain a valid handle.

  Parameters:
    - hMac -  Handle identifying the MAC driver client

  Return:
    The currently stack pending events.

  Example:
    <code>
    TCPIP_MAC_EVENT currEvents = DRV_G3ADP_MAC_EventPendingGet( hMac);
    </code>

  Remarks:
    - This is the preferred method to get the current pending G3 ADP MAC events.
      The stack maintains a proper image of the events from their occurrence to
      their acknowledgment.

    - Even with a notification handler in place it's better to use this
      function to get the current pending events rather than using the
      events passed by the notification handler which could be stale.

    - The events are persistent. They shouldn't be re-enabled unless they
      have been processed and the condition that generated them was removed.
      Re-enabling them immediately without proper processing will have
      dramatic effects on system performance.

    - The returned value is just a momentary value. The pending events can
      change any time.
*/
TCPIP_MAC_EVENT DRV_G3ADP_MAC_EventPendingGet(DRV_HANDLE hMac);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // #ifndef DRV_MAC_G3ADP_H

/*******************************************************************************
 End of File
*/

