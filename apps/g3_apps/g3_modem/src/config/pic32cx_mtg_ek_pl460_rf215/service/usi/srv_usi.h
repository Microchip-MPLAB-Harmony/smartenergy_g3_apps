/*******************************************************************************
  USI Service Library Interface Header File

  Company
    Microchip Technology Inc.

  File Name
    srv_usi.h

  Summary
    Unified Serial Interface (USI) service library interface.

  Description
    This file defines the interface to the USI service library. This
    library provides a function that packs the data into USI frame format and
    sends the message through serial interface to the external application.
    For serial receptions from the serial interface, the service unpacks the
    data and passes it through callback (SRV_USI_CallbackRegister) to the
    corresponding client depending on the USI protocol.

  Remarks:
    USI protocol is used by all PLC Tools provided by Microchip.
*******************************************************************************/

// DOM-IGNORE-BEGIN
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
// DOM-IGNORE-END

#ifndef SRV_USI_H    // Guards against multiple inclusion
#define SRV_USI_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "driver/driver_common.h"
#include "system/system.h"
#include "srv_usi_definitions.h"

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
/* USI Service Handle

  Summary:
    Handle to an opened instance of the USI service.

  Description:
    This handle identifies an opened instance of the USI service. It is
    returned from SRV_USI_Open and it has to be passed to all other USI
    routines (except SRV_USI_Initialize and SRV_USI_Tasks).

  Remarks:
    None.
*/

typedef uintptr_t SRV_USI_HANDLE;


// *****************************************************************************
/* Invalid USI Service Handle

 Summary:
    Definition of invalid USI handle.

 Description:
    This definition is used to indicate that an USI handle
    (SRV_USI_HANDLE) is invalid. SRV_USI_Open returns this value if:
      - the specified instance has been already opened.
      - the instance being opened is not initialized or is invalid.
      - if there is an error opening the serial interface (USB CDC instances
      only)

 Remarks:
    The handle returned from SRV_USI_Open should be checked to ensure it is
    different to SRV_USI_HANDLE_INVALID before attempting to call any other USI
    routine using the handle.
*/

#define SRV_USI_HANDLE_INVALID  (((SRV_USI_HANDLE) -1))

// *****************************************************************************
/* Function:
    typedef void ( * SRV_USI_CALLBACK ) ( uint8_t *pData, size_t length );

   Summary:
    Pointer to an USI service callback function.

   Description:
    This data type defines a pointer to an USI service callback function, thus
    defining the function signature. Clients of the USI service can register
    callback functions for a specific USI protocol with
    SRV_USI_CallbackRegister function. That function will be called back when
    a new message is received and it belongs to the specified USI protocol.

   Parameters:
    pData      - Pointer to data payload of the received USI message
    length     - Length in bytes of data payload of the received USI message


   Returns:
    None.

  Example:
    <code>
    void APP_USIPhyProtocolEventHandler(uint8_t *pData, size_t length)
    {

    }

    SRV_USI_CallbackRegister(handle, SRV_USI_PROT_ID_PHY, APP_USIPhyProtocolEventHandler);
    </code>

  Remarks:
    None.
*/

typedef void ( * SRV_USI_CALLBACK ) ( uint8_t *pData, size_t length );

// *****************************************************************************
/*  USI device descriptor function prototypes

  Summary:
    Function prototype for the device descriptor expected by the USI service.

  Description:
    Defines the function prototypes for the device descriptor that each device
    must implement in order to plug in to the USI service.

  Remarks:
    None.
*/

typedef void (*USI_READ_CALLBACK) (uint8_t *data, uint16_t length, uintptr_t context);

typedef void (*SRV_USI_INIT_FPTR) (uint32_t index, const void * const initData);

typedef DRV_HANDLE (*SRV_USI_OPEN_FPTR) (uint32_t index);

typedef void (*SRV_USI_REGISTER_READ_CALLBACK_FPTR) (uint32_t index, USI_READ_CALLBACK buf, uintptr_t context);

typedef void (*SRV_USI_WRITE_FPTR) (uint32_t index, void* buf, size_t length);

typedef void (*SRV_USI_TASK_FPTR) (uint32_t index);

typedef SRV_USI_STATUS (*SRV_USI_STATUS_FPTR) (uint32_t index);

typedef void (*SRV_USI_CLOSE) (uint32_t index);

// *****************************************************************************
/*  USI device descriptor

  Summary:
    Contains the API required by the USI service to abstract from the hardware
    peripheral used.

  Description:
    This structure contains pointers to functions required by the USI service
    to abstract from the hardware peripheral used. The USI service implements
    these functions for UART peripherals (see srv_usi_usart.c) and USB CDC
    Function Driver (see srv_usi_cdc.c).

  Remarks:
    The generated code will include srv_usi_usart.c and/or srv_usi_cdc.c files
    depending on the USI instance(s) MCC configuration.
*/
typedef struct
{
    SRV_USI_INIT_FPTR init;

    SRV_USI_OPEN_FPTR open;

    SRV_USI_REGISTER_READ_CALLBACK_FPTR setReadCallback;

    SRV_USI_WRITE_FPTR writeData;

    SRV_USI_TASK_FPTR task;

    SRV_USI_CLOSE close;

    SRV_USI_STATUS_FPTR status;

} SRV_USI_DEV_DESC;

// *****************************************************************************
/* USI Service Initialize structure

  Summary:
    Contains the data required to initialize an instance of the USI service.

  Description:
    This structure contains the data needed to initialize an instance of the
    USI service. It is passed as parameter (casted as SYS_MODULE_INIT) to
    SRV_USI_Initialize function.

    Field description:
      - deviceInitData: Pointer to data initialization struct for the specific
      USI instance type; UART (see srv_usi_usart.h) or USB CDC
      (see srv_usi_cdc.h)
      - consDevDesc: Pointer to SRV_USI_DEV_DESC Struct
      - deviceIndex: USI instance index for the specific USI instance type
      (UART or USB CDC)
      - pWrBuffer: Pointer to buffer to encode transmitted USI messages
      - wrBufferSize: Size in bytes of buffer to encode transmitted USI
      messages

  Remarks:
    This initialization data struct is automatically generated for each USI
    instance depending on MCC configuration (see initialization.c).
*/

typedef struct
{
    const void * const deviceInitData;

    const SRV_USI_DEV_DESC* consDevDesc;

    uint32_t deviceIndex;

    void* pWrBuffer;

    size_t wrBufferSize;

} SRV_USI_INIT;

// *****************************************************************************
// *****************************************************************************
// Section: Service Interface Functions
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ SRV_USI_Initialize(
    const SYS_MODULE_INDEX index,
    const SYS_MODULE_INIT * const init)

  Summary:
    Initializes the specified USI service instance.

  Description:
    This routine initializes the specified USI service instance, making it
    ready for clients to open and use. The initialization data is specified by
    the init parameter.

    The initialization may fail if the number of instances allocated are
    insufficient or if the specified instance is already initialized.

    The USI service instance index is independent of the peripheral instance
    it is associated with. For example, USI service instance 0 can be assigned
    to UART peripheral instance 2.

  Parameters:
    index   - Index for the instance to be initialized

    init    - Pointer to the init data structure containing any data
              necessary to initialize the service.

  Returns:
    If successful, returns a valid USI instance object. Otherwise, returns
    SYS_MODULE_OBJ_INVALID.

  Example:
    <code>
    static uint8_t CACHE_ALIGN srvUSI0ReadBuffer[SRV_USI0_RD_BUF_SIZE] = {0};
    static uint8_t CACHE_ALIGN srvUSI0WriteBuffer[SRV_USI0_WR_BUF_SIZE] = {0};

    extern const SRV_USI_DEV_DESC srvUSIUSARTDevDesc;

    const SRV_USI_USART_INTERFACE srvUsi0InitDataUART2 = {
        .readCallbackRegister = (USI_USART_PLIB_READ_CALLBACK_REG)UART2_ReadCallbackRegister,
        .readData = (USI_USART_PLIB_WRRD)UART2_Read,
        .writeData = (USI_USART_PLIB_WRRD)UART2_Write,
    };

    const USI_USART_INIT_DATA srvUsi0InitData = {
        .plib = (void*)&srvUsi0InitDataUART2,
        .pRdBuffer = (void*)srvUSI0ReadBuffer,
        .rdBufferSize = SRV_USI0_RD_BUF_SIZE,
    };

    const SRV_USI_INIT srvUSI0Init =
    {
        .deviceInitData = (const void * const)&srvUsi0InitData,
        .consDevDesc = &srvUSIUSARTDevDesc,
        .deviceIndex = 0,
        .pWrBuffer = srvUSI0WriteBuffer,
        .wrBufferSize = SRV_USI0_WR_BUF_SIZE
    };

    static uint8_t CACHE_ALIGN srvUSI1ReadBuffer[SRV_USI1_RD_BUF_SIZE] = {0};
    static uint8_t CACHE_ALIGN srvUSI1WriteBuffer[SRV_USI1_WR_BUF_SIZE] = {0};

    static uint8_t CACHE_ALIGN srvUSI1CDCReadBuffer[128] = {0};

    extern const SRV_USI_DEV_DESC srvUSICDCDevDesc;

    const USI_CDC_INIT_DATA srvUsi1InitData = {
        .cdcInstanceIndex = 0,
        .usiReadBuffer = srvUSI1ReadBuffer,
        .usiBufferSize = SRV_USI1_RD_BUF_SIZE,
        .cdcReadBuffer = srvUSI1CDCReadBuffer,
        .cdcBufferSize = 128
    };

    const SRV_USI_INIT srvUSI1Init =
    {
        .deviceInitData = (const void * const)&srvUsi1InitData,
        .consDevDesc = &srvUSICDCDevDesc,
        .deviceIndex = 0,
        .pWrBuffer = srvUSI1WriteBuffer,
        .wrBufferSize = SRV_USI1_WR_BUF_SIZE
    };

    SYS_MODULE_OBJ  objSrvUSI0;
    SYS_MODULE_OBJ  objSrvUSI1;

    objSrvUSI0 = SRV_USI_Initialize(SRV_USI_INDEX_0, (SYS_MODULE_INIT *)&srvUSI0Init);
    if (objSrvUSI0 == SYS_MODULE_OBJ_INVALID)
    {

    }

    objSrvUSI1 = SRV_USI_Initialize(SRV_USI_INDEX_1, (SYS_MODULE_INIT *)&srvUSI1Init);
    if (objSrvUSI1 == SYS_MODULE_OBJ_INVALID)
    {

    }
    </code>

  Remarks:
    This routine must be called before any other USI routine is called. This
    routine should only during system initialization.
*/

SYS_MODULE_OBJ SRV_USI_Initialize(
    const SYS_MODULE_INDEX index,
    const SYS_MODULE_INIT * const init);

// *****************************************************************************
/* Function:
    SRV_USI_HANDLE SRV_USI_Open( const SYS_MODULE_INDEX index )

  Summary:
    Opens the specified USI service instance and returns a handle to it.

  Description:
    This routine opens the specified USI service instance and provides a handle
    that must be provided to all other client-level operations to identify the
    caller and the instance of the service.

  Precondition:
    Function SRV_USI_Initialize must have been called before calling this
    function.

  Parameters:
    index     - Index for the instance to be opened

  Returns:
    If successful, the routine returns a valid open-instance handle (SRV_USI_HANDLE Typedef).

    If an error occurs, the return value is SRV_USI_HANDLE_INVALID. Error can occur
      - if the specified instance has been already opened.
      - if the instance being opened is not initialized or is invalid.
      - if there is an error opening the serial interface (USB CDC instances
      only)

  Example:
    <code>
    SRV_USI_HANDLE handle;

    handle = SRV_USI_Open(SRV_USI_INDEX_0);
    if (handle == SRV_USI_HANDLE_INVALID)
    {

    }
    </code>

  Remarks:
    The handle returned is valid until SRV_USI_Close is called.
*/

SRV_USI_HANDLE SRV_USI_Open( const SYS_MODULE_INDEX index );

// *****************************************************************************
/* Function:
    void SRV_USI_Close( SRV_USI_HANDLE handle )

  Summary:
    Closes an opened-instance of the USI service.

  Description:
    This routine closes an opened-instance of the USI service, invalidating
    the handle.

    A new handle must be obtained by calling SRV_USI_Open before the caller
    uses the service again.

  Precondition:
    SRV_USI_Open must have been called to obtain a valid opened service handle.

  Parameters:
    handle - A valid open-instance handle, returned from SRV_USI_Open

  Returns:
    None.

  Example:
    <code>
    SRV_USI_Close(handle);
    </code>

  Remarks:
    None.
*/

void SRV_USI_Close( SRV_USI_HANDLE handle );

// *****************************************************************************
/* Function:
    SRV_USI_STATUS SRV_USI_Status( SRV_USI_HANDLE handle )

  Summary:
    Returns the current status of the USI service.

  Description:
    This routine provides the current status of the USI service instance.

  Precondition:
    SRV_USI_Open must have been called to obtain a valid opened service handle.

  Parameters:
    handle - A valid open-instance handle, returned from SRV_USI_Open

  Returns:
    SRV_USI_STATUS_UNINITIALIZED - Indicates that the USI instance is not initialized.

    SRV_USI_STATUS_NOT_CONFIGURED - Indicates that the USI instance is not configured.

    SRV_USI_STATUS_CONFIGURED - Indicates that the USI instance is properly configured

    SRV_USI_STATUS_ERROR - Indicates that there is an error with the handler

  Example:
    <code>
    if (SRV_USI_Status (handle) == SRV_USI_STATUS_CONFIGURED)
    {

    }
    </code>

  Remarks:
    After opening an USI instance, before any other operation, the client must
    use this function to check the instance is ready.
  */

SRV_USI_STATUS SRV_USI_Status( SRV_USI_HANDLE handle );

// *****************************************************************************
/* Function:
  void SRV_USI_CallbackRegister(
    SRV_USI_HANDLE handle,
    SRV_USI_PROTOCOL_ID protocol,
    SRV_USI_CALLBACK callback)

  Summary:
    Registers a function to be called back when a new message is received and
    it belongs to the specified USI protocol.

  Description:
    This function allows a client to register an event handling function to be
    called back when a new message is received and it belongs to the specified
    USI protocol.

    One USI instance can have different callbacks for different USI protocols.

    The callback once set, persists until the client closes the USI service
    instance or sets another callback (which could be a NULL pointer to
    indicate no callback).

  Precondition:
    SRV_USI_Open must have been called to obtain a valid opened service handle.

  Parameters:
    handle      - A valid open-instance handle, returned from SRV_USI_Open
    protocol    - Identifier of the protocol for which callback function will
                  be registered
    callback    - Pointer to the callback function

  Returns:
    None.

  Example:
    <code>
    void APP_USIPhyProtocolEventHandler(uint8_t *pData, size_t length)
    {

    }

    SRV_USI_CallbackRegister(handle, SRV_USI_PROT_ID_PHY, APP_USIPhyProtocolEventHandler);
    </code>
*/

void SRV_USI_CallbackRegister(
    SRV_USI_HANDLE handle,
    SRV_USI_PROTOCOL_ID protocol,
    SRV_USI_CALLBACK callback);

// *****************************************************************************
/* Function:
    void SRV_USI_Tasks( SYS_MODULE_OBJ object )

  Summary:
    Maintains the USI's state machine.

  Description:
    This function is used to maintain the USI's internal state machine and
    generate callback functions.

  Precondition:
    SRV_USI_Initialize must have been called for the specified USI service
    instance.

  Parameters:
    object - Object returned from SRV_USI_Initialize

  Returns:
    None

  Example:
    <code>
    while (true)
    {
        SRV_USI_Tasks(SRV_USI_INDEX_0);
    }
    </code>

  Remarks:
    This function is normally not called directly by an application. It is
    called by the system's Tasks routine (SYS_Tasks).

    This function will never block or access any resources that may cause it to
    block.
  */

void SRV_USI_Tasks( SYS_MODULE_OBJ object );

// *****************************************************************************
/* Function:
      size_t SRV_USI_Send_Message( SRV_USI_HANDLE handle,
        SRV_USI_PROTOCOL_ID protocol, uint8_t *data, size_t length )

  Summary:
    Sends a message through serial interface (USI).

  Description:
    This function is used to send a message through USI. The message will be
    formated depending on the specified Protocol and will be sent using the
    serial interface associated to the corresponding USI instance.

  Precondition:
    SRV_USI_Open must have been called to obtain a valid opened service handle.

  Parameters:
    handle      - A valid open-instance handle, returned from SRV_USI_Open
    protocol    - Identifier of the protocol for the message to send
    data        - Pointer to the data to send
    length      - Length of the data to send in bytes

  Returns:
    Number of bytes sent.

  Example:
    <code>
    uint8_t pData[] = "Message to send through USI";

    SRV_USI_Send_Message(handle, SRV_USI_PROT_ID_PHY, pData, sizeof(pData));
    </code>

  Remarks:
    None.
  */

size_t SRV_USI_Send_Message( SRV_USI_HANDLE handle,
        SRV_USI_PROTOCOL_ID protocol, uint8_t *data, size_t length );

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif
// DOM-IGNORE-END

#endif //SRV_USI_H
