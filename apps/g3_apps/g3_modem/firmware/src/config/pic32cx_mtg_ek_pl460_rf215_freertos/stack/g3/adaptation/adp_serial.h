/*******************************************************************************
  G3 ADP Serialization Header File

  Company:
    Microchip Technology Inc.

  File Name:
    adp_serial.h

  Summary:
    G3 ADP Serialization API Header File.

  Description:
    The G3 ADP Serialization allows to serialize the ADP and LBP API through
    USI interface in order to run the application on an external device. This
    file contains definitions of the API of G3 ADP Serialization.
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

#ifndef _ADP_SERIAL_H
#define _ADP_SERIAL_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************
#include "adp.h"

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
/* ADP Serial Set EUI64 and Non-volatile Data Event Handler Function Pointer

  Summary:
    Pointer to a Set EUI64 and Non-volatile Data Event handler function.

  Description:
    This data type defines the required function signature for the ADP Serial
    Set EUI64 and Non-volatile Data event handling callback function. The ADP
    Serial Set EUI64 and non-volatile Data primitive allows the next higher
    layer to be notified when EUI64 (Extended Address) and non-volatile data
    should be set, after initializing ADP.

    A client must register a pointer using the event handling function whose
    function signature (parameter and return value types) matches the types
    specified by this function pointer in order to receive Set EUI64 and
    Non-volatile Data events back from module.

  Parameters:
    pEUI64              - Pointer to store the EUI64 (Extended Address)
    pNonVolatileDataInd - Pointer to store non-volatile data

  Example:
    <code>
    App_SetEUI64NonVolatileData (
        ADP_EXTENDED_ADDRESS* pEUI64,
        ADP_NON_VOLATILE_DATA_IND_PARAMS* pNonVolatileData
    )
    {
        ADP_EXTENDED_ADDRESS eui64;
        ADP_NON_VOLATILE_DATA_IND_PARAMS nonVolatileData;

        // Get Extended Address. Must be unique for each device.
        // Load non-volatile data

        memcpy(pEUI64->value, eui64.value, 8);
        memcpy(pNonVolatileData, &nonVolatileData, sizeof(nonVolatileData));
    }
    </code>

  Remarks:
    None.
*/
typedef void (*ADP_SERIAL_SET_EUI64_NON_VOLATILE_DATA_CALLBACK) (
    ADP_EXTENDED_ADDRESS* pEUI64,
    ADP_NON_VOLATILE_DATA_IND_PARAMS* pNonVolatileData
);

// *****************************************************************************
/* ADP Serial Callback Notifications Structure

   Summary:
    Set of event handler function pointers to receive events from ADP Serial.

   Description:
    Defines the set of callback functions that ADP Serial uses to generate
    events to upper layer.

   Remarks:
    In case an event is to be ignored, setting its corresponding callback
    function to NULL will lead to the event not being generated.
*/
typedef struct
{
    ADP_SERIAL_SET_EUI64_NON_VOLATILE_DATA_CALLBACK setEUI64NonVolatileData;
    ADP_NON_VOLATILE_DATA_IND_CALLBACK              nonVolatileDataIndication;
} ADP_SERIAL_NOTIFICATIONS;

// *****************************************************************************
// *****************************************************************************
// Section: ADP Serial Interface Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ ADP_SERIAL_Initialize(const SYS_MODULE_INDEX index);

  Summary:
    Initializes the ADP Serialization module for the specified index.

  Description:
    This routine initializes the ADP Serialization module for the specified
    index.

  Precondition:
    None.

  Parameters:
    index - Identifier for the instance to be initialized (single instance
            allowed)

  Returns:
    If successful, returns a valid module instance object.
    Otherwise, returns SYS_MODULE_OBJ_INVALID.

  Example:
    <code>
    SYS_MODULE_OBJ sysObjAdpSerial;

    sysObjAdpSerial = ADP_SERIAL_Initialize(G3_ADP_SERIAL_INDEX_0);
    if (sysObjAdpSerial == SYS_MODULE_OBJ_INVALID)
    {
        // Handle error
    }
    </code>

  Remarks:
    This routine must be called before any other ADP Serialization routine is
    called.
*/
SYS_MODULE_OBJ ADP_SERIAL_Initialize(const SYS_MODULE_INDEX index);

// *****************************************************************************
/* Function:
    void ADP_SERIAL_Tasks(SYS_MODULE_OBJ object);

  Summary:
    Maintains ADP Serialization State Machine.

  Description:
    This routine maintains ADP Serialization State Machine.

  Precondition:
    The ADP_SERIAL_Initialize routine must have been called to obtain a valid
    system object.

  Parameters:
    object - System object handle, returned from the ADP_SERIAL_Initialize
             routine.

  Returns:
    None.

  Example:
    <code>
    SYS_MODULE_OBJ sysObjAdpSerial; // Returned from ADP_SERIAL_Initialize

    while (true)
    {
        ADP_SERIAL_Tasks(sysObjAdpSerial);

        // Do other tasks
    }
    </code>

  Remarks:
    This function is normally not called directly by an application. It is
    called by the system's tasks routine (SYS_Tasks).
*/
void ADP_SERIAL_Tasks(SYS_MODULE_OBJ object);

// *****************************************************************************
/* Function:
    void ADP_SERIAL_SetNotifications(ADP_SERIAL_NOTIFICATIONS* pNotifications);

  Summary:
    Sets ADP Serial notifications.

  Description:
    This routine sets the ADP Serial notifications. Callback handlers for event
    notification are set in this function.

  Precondition:
    None.

  Parameters:
    pNotifications - Structure with callbacks used to notify ADP Serial specific
                     events

  Returns:
    None.

  Example:
    <code>
    ADP_SERIAL_NOTIFICATIONS adpSerialNotifications = {
        .setEUI64 = appSetEUI64,
        .nonVolatileDataIndication = appNonVolatileDataIndication
    };

    ADP_SERIAL_SetNotifications(&adpSerialNotifications);
    </code>

  Remarks:
    None.
*/
void ADP_SERIAL_SetNotifications(ADP_SERIAL_NOTIFICATIONS* pNotifications);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // #ifndef _ADP_SERIAL_H
