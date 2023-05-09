/*******************************************************************************
  ADP Shared Types Header File

  Company:
    Microchip Technology Inc.

  File Name:
    adp_shared_types.h

  Summary:
    ADP Shared Types Header File

  Description:
    The ADP provides a simple interface to manage the G3 Adaptation Layer. This
    file provides shared types definition for the ADP and routing libraries.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2023 Microchip Technology Inc. and its subsidiaries.
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

#ifndef _ADP_SHARED_TYPES_H
#define _ADP_SHARED_TYPES_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************
#include "adp_api_types.h"
#include "system/time/sys_time.h"

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
/* ADP Maximum Number of Fragments Definition

   Summary:
    Defines the maximum number of fragments which can be used to receive a
    fragmented message.

   Description:
    This macro defines maximum number of fragments which can be used to receive
    a fragmented message.

   Remarks:
    None.
*/
#define ADP_MAX_NUMBER_OF_FRAGMENTS 6

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* ADP Data Common Send Event Handler Function Pointer

  Summary:
    Pointer to a Data Common Send Event handler function.

  Description:
    This data type defines the required function signature for the ADP Data
    Common Send event handling callback function.

    A client must register a pointer using the event handling function whose
    function signature (parameter and return value types) matches the types
    specified by this function pointer in order to receive Data Common Send
    events back from module.

  Parameters:
    status - Data send result

  Example:
    <code>
    App_DataSendCallback(uint8_t status)
    {
        // Check result
        if (status == G3_SUCCESS)
        {
            
        }
    }
    </code>

  Remarks:
    None.
*/
typedef void (*ADP_COMMON_DATA_SEND_CALLBACK)(uint8_t status);

// *****************************************************************************
/* ADP Data Send Parameters Definition

   Summary:
    Defines the structure of ADP Data Send parameters.

   Description:
    This data type defines the structure of ADP Data Send parameters. It used
    internally by the ADP.

   Remarks:
    The client does not need to use this data type.
*/
typedef struct
{
    ADP_ADDRESS srcDeviceAddress;
    ADP_ADDRESS dstDeviceAddress;
    ADP_COMMON_DATA_SEND_CALLBACK callback;
    uint16_t dataLength;
    uint16_t fragmentOffset;
    uint16_t lastFragmentSize;
    uint16_t datagramTag;
    uint16_t datagramSize;
    bool discoverRoute;
    uintptr_t handle;
    uint8_t maxHops;
    uint8_t dataType;
    uint8_t originalDataType;
    uint8_t qualityOfService;
    uint8_t security;
    uint8_t broadcastSeqNo;
    bool meshHeaderNeeded;
    bool multicast;
    uint8_t bufferOffset;
    uint8_t numRepairReSendAttemps;
    uint8_t mediaType;

} ADP_DATA_SEND_PARAMS;

// *****************************************************************************
/* ADP 1280-byte Data Buffer Definition

   Summary:
    Defines the structure of ADP 1280-byte Data Send parameters and buffer.

   Description:
    This data type defines the structure of ADP 1280-byte Data Send parameters
    and buffer. It used internally by the ADP library.

   Remarks:
    The client does not need to use this data type.
*/
typedef struct
{
    /* Data Send parameters */
    ADP_DATA_SEND_PARAMS sendParameters;
    
    /* 1280 + 1 extra byte needed for 6LowPAN IPv6 header (compressed or not) */
    uint8_t data[1281];

} ADP_DATA_PARAMS_BUFFER_1280;

// *****************************************************************************
/* ADP 400-byte Data Buffer Definition

   Summary:
    Defines the structure of ADP 400-byte Data Send parameters and buffer.

   Description:
    This data type defines the structure of ADP 400-byte Data Send parameters
    and buffer. It used internally by the ADP library.

   Remarks:
    The client does not need to use this data type.
*/
typedef struct
{
    /* Data Send parameters */
    ADP_DATA_SEND_PARAMS sendParameters;

    /* 400 + 1 extra byte needed for 6LowPAN IPv6 header (compressed or not) */
    uint8_t data[401];

} ADP_DATA_PARAMS_BUFFER_400;

// *****************************************************************************
/* ADP 100-byte Data Buffer Definition

   Summary:
    Defines the structure of ADP 400-byte Data Send parameters and buffer.

   Description:
    This data type defines the structure of ADP 400-byte Data Send parameters
    and buffer. It used internally by the ADP library.

   Remarks:
    The client does not need to use this data type.
*/
typedef struct
{
    /* Data Send parameters */
    ADP_DATA_SEND_PARAMS sendParameters;

    /* 100 + 1 extra byte needed for 6LowPAN IPv6 header (compressed or not) */
    uint8_t data[101];

} ADP_DATA_PARAMS_BUFFER_100;

// *****************************************************************************
/* ADP Process Queue Entry Definition

   Summary:
    Defines the structure of ADP Process Queue entry.

   Description:
    This data type defines the structure of ADP Process Queue entry. It used
    internally by the ADP library.

   Remarks:
    The client does not need to use this data type.
*/
typedef struct _ADP_PROCESS_QUEUE_ENTRY
{
    /* Pointer to the next object of the queue */
    struct _ADP_PROCESS_QUEUE_ENTRY* next;

    /* Pointer to data send parameters */
    ADP_DATA_SEND_PARAMS* pSendParameters;

    /* Pointer to data buffer */
    uint8_t* pData;

    /* Send delay time in milliseconds */
    int32_t delayTime;

    /* Absolute time in milliseconds when the entry expires */
    int32_t validTime;

    /* Fragmentation time handle */
    SYS_TIME_HANDLE fragTimeHandle;

    /* Data size in bytes */
    uint16_t dataSize;

    /* Entry processing flag */
    bool processing;

    /* Delayed flag */
    bool delayed;

    /* Fragmentation time expired flag */
    bool fragTimeExpired;

} ADP_PROCESS_QUEUE_ENTRY;

// *****************************************************************************
/* ADP 6LowPAN Fragment Information Definition

   Summary:
    Defines the structure of ADP 6LowPAN Fragment Information.

   Description:
    This data type defines the structure of ADP 6LowPAN Fragment Information.
    It used internally by the ADP library.

   Remarks:
    The client does not need to use this data type.
*/
typedef struct
{
    uint16_t offset;
    uint16_t size;

} ADP_LOWPAN_FRAGMENT_INFO;

// *****************************************************************************
/* ADP 6LowPAN Fragmented Data Definition

   Summary:
    Defines the structure of ADP 6LowPAN Fragmented Data.

   Description:
    This data type defines the structure of ADP 6LowPAN Fragmented Data.
    It used internally by the ADP library.

   Remarks:
    The client does not need to use this data type.
*/
typedef struct
{
    /* Fragments information */
    ADP_LOWPAN_FRAGMENT_INFO fragments[ADP_MAX_NUMBER_OF_FRAGMENTS];

    /* Absolute time in milliseconds when the entry expires */
    int32_t validTime;

    /* Datagram origin address */
    uint16_t datagramOrigin;

    /* Datagram tag */
    uint16_t datagramTag;

    /* Datagram size in bytes */
    uint16_t datagramSize;

    /* 1280 max IPv6 packet + 1 byte IPv6 6LowPAN header */
    uint8_t data[1281];

    /* Flag to indicate if header was compressed */
    bool wasCompressed;
    
} ADP_LOWPAN_FRAGMENTED_DATA;

// *****************************************************************************
/* ADP Data Send Parameters Definition

   Summary:
    Defines the structure of ADP Group Master Key.

   Description:
    This data type defines the structure of ADP Group Master Key (GMK).

   Remarks:
    None.
*/
typedef struct
{
    /* The id of the key */
    uint8_t keyId;

    /* The key value */
    uint8_t key[16];

} ADP_GROUP_MASTER_KEY;

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // #ifndef _ADP_SHARED_TYPES_H
