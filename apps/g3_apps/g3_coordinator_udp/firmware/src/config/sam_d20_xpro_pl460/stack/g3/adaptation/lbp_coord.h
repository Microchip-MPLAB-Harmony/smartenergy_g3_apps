/*******************************************************************************
  LBP Coordinator Header File

  Company:
    Microchip Technology Inc.

  File Name:
    lbp_coord.h

  Summary:
    LBP for Coordinator Header File.

  Description:
    The LoWPAN Bootstrapping Protocol (LBP) provides a simple interface to
    manage the G3 boostrap process Adaptation Layer. This file provides the
    interface to manage LBP process for coordinator.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2024 Microchip Technology Inc. and its subsidiaries.
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

#ifndef LBP_COORD_H
#define LBP_COORD_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************
#include "lbp_defs.h"
#include "adp_shared_types.h"

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
/* LBP Join Request Indication Event Handler Function Pointer

  Summary:
    Pointer to a Join Request Indication Event handler function.

  Description:
    This data type defines the required function signature for the LBP Join
    Request Indication event handling callback function. The LBP Join Request
    Indication primitive allows the upper layer to be notified of the intention
    of a device to Join the Network, allowing to reject it or accept it
    assigning a network short address.

    A client must register a pointer using the event handling
    function whose function signature (parameter and return value types) matches
    the types specified by this function pointer in order to receive Join
    Request Indication events back from module.

  Parameters:
    pLbdAddress      - Pointer to Extended Address of device willing to Join the
                       network

  Example:
    <code>
    App_JoinRequestIndication(uint8_t* pLbdAddress)
    {
        uint16_t assignedAddress;

        assignedAddress = 0x0001;
        LBP_ShortAddressAssign(pLbdAddress, assignedAddress);
    }
    </code>

  Remarks:
    None.
*/
typedef void (*LBP_COORD_JOIN_REQUEST_IND_CALLBACK)(uint8_t* pLbdAddress);

// *****************************************************************************
/* LBP Join Complete Indication Event Handler Function Pointer

  Summary:
    Pointer to a Join Complete Indication Event handler function.

  Description:
    This data type defines the required function signature for the LBP Join
    Complete Indication event handling callback function. The LBP Join Complete
    Indication primitive allows the upper layer to be notified of the successful
    completion of a Network Join for a certain device.

    A client must register a pointer using the event handling
    function whose function signature (parameter and return value types) matches
    the types specified by this function pointer in order to receive Join
    Complete Indication events back from module.

  Parameters:
    pLbdAddress     - Pointer to Extended Address of device that Joined the
                      network

    assignedAddress - The 16-bit address of device that Joined the network

  Example:
    <code>
    App_JoinCompleteIndication(uint8_t* pLbdAddress, uint16_t assignedAddress)
    {
        
    }
    </code>

  Remarks:
    None.
*/
typedef void (*LBP_COORD_JOIN_COMPLETE_IND_CALLBACK)(uint8_t* pLbdAddress,
    uint16_t assignedAddress);

// *****************************************************************************
/* LBP Leave Indication Event Handler Function Pointer

  Summary:
    Pointer to a Leave Indication Event handler function.

  Description:
    This data type defines the required function signature for the LBP Leave
    Indication event handling callback function. The LBP Leave
    Indication primitive allows the upper layer to be notified when a device
    leaves the network.

    A client must register a pointer using the event handling
    function whose function signature (parameter and return value types) matches
    the types specified by this function pointer in order to receive Leave
    Indication events back from module.

  Parameters:
    networkAddress - The 16-bit network address of device leaving the network

  Example:
    <code>
    App_LeaveIndication(uint16_t networkAddress)
    {
        
    }
    </code>

  Remarks:
    None.
*/
typedef void (*LBP_COORD_LEAVE_IND_CALLBACK)(uint16_t networkAddress);

// *****************************************************************************
/* LBP Coordinator Callback Notificatios Structure

   Summary:
    Set of event handler function pointers to receive events from LBP
    Coordinator.

   Description:
    Defines the set of callback functions that LBP Coordinator uses to generate
    events to upper layer.

   Remarks:
    In case an event is to be ignored, setting its corresponding callback
    function to NULL will lead to the event not being generated.
*/
typedef struct
{
    LBP_COORD_JOIN_REQUEST_IND_CALLBACK joinRequestIndication;
    LBP_COORD_JOIN_COMPLETE_IND_CALLBACK joinCompleteIndication;
    LBP_COORD_LEAVE_IND_CALLBACK leaveIndication;

} LBP_NOTIFICATIONS_COORD;

// *****************************************************************************
// *****************************************************************************
// Section: LBP Coordinator Interface Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    void LBP_InitCoord(bool aribBand);

  Summary:
    Initializes the Coordinator LBP module.

  Description:
    This routine initializes the Coordinator LBP data structures.

  Precondition:
    None.

  Parameters:
    aribBand - Flag to indicate if ARIB PLC band is used

  Returns:
    None.

  Example:
    <code>
    LBP_InitCoord(false);
    </code>

  Remarks:
    This routine must be called before any other Coordinator LBP API function.
*/
void LBP_InitCoord(bool aribBand);

// *****************************************************************************
/* Function:
    void LBP_UpdateLbpSlots(void);

  Summary:
    Updates the slots that control bootstrap protocol.

  Description:
    This routine updates the slots that control bootstrap protocol
    and maintains LBP State Machine.

  Precondition:
    LBP_InitCoord must have been called before.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    void APP_Tasks(void)
    {
        LBP_UpdateLbpSlots();
    }
    </code>

  Remarks:
    None.
*/
void LBP_UpdateLbpSlots(void);

// *****************************************************************************
/* Function:
    void LBP_SetNotificationsCoord(LBP_NOTIFICATIONS_COORD *pNotifications);

  Summary:
    Sets Coordinator LBP notifications.

  Description:
    This routine sets the Coordinator LBP notifications. Callback handlers for
    event notification are set in this function.

  Precondition:
    LBP_InitCoord must have been called before.

  Parameters:
    pNotifications - Structure with callbacks used to notify Coordinator LBP
                     events

  Returns:
    None.

  Example:
    <code>
    LBP_NOTIFICATIONS_COORD lbpNotifications = {
        .joinRequestIndication = appLbpJoinRequestIndication,
        .joinCompleteIndication = appLbpJoinCompleteIndication,
        .leaveIndication = appLbpLeaveIndication
    };

    LBP_SetNotificationsCoord(&lbpNotifications);
    </code>

  Remarks:
    None.
*/
void LBP_SetNotificationsCoord(LBP_NOTIFICATIONS_COORD* pNotifications);

// *****************************************************************************
/* Function:
    bool LBP_KickDevice(uint16_t shortAddress, ADP_EXTENDED_ADDRESS* pEUI64Address);

  Summary:
    Removes a device from the network.

  Description:
    This routine allows the upper layer to remove a device from the network.

  Precondition:
    LBP_InitCoord must have been called before.

  Parameters:
    shortAddress  - The network address of device to be removed

    pEUI64Address - The extended address of device to be removed

  Returns:
    True if Kick is sent to device, otherwise false.

  Example:
    <code>
    ADP_EXTENDED_ADDRESS eui64;
    uint16_t shortAddr;

    LBP_KickDevice(shortAddr, &eui64);
    </code>

  Remarks:
    None.
*/
bool LBP_KickDevice(uint16_t shortAddress, ADP_EXTENDED_ADDRESS* pEUI64Address);

// *****************************************************************************
/* Function:
    void LBP_Rekey(uint16_t shortAddress, ADP_EXTENDED_ADDRESS *pEUI64Address,
        bool distribute);

  Summary:
    Starts a rekey process for a device in the network.

  Description:
    This routine allows the upper layer to start a rekey process for a device in
    the network.

  Precondition:
    LBP_InitCoord must have been called before.

  Parameters:
    shortAddress  - The network address of device to be rekeyed

    pEUI64Address - The extended address of device to be rekeyed

    distribute    - True in distribution phase, false in activation phase

  Returns:
    None.

  Example:
    <code>
    ADP_EXTENDED_ADDRESS eui64;
    uint16_t shortAddr;

    LBP_Rekey(shortAddr, &eui64, true);
    </code>

  Remarks:
    None.
*/
void LBP_Rekey(uint16_t shortAddress, ADP_EXTENDED_ADDRESS *pEUI64Address,
    bool distribute);

// *****************************************************************************
/* Function:
    void LBP_SetRekeyPhase(bool rekeyStart);

  Summary:
    Starts or stops a rekey phase in LBP module.

  Description:
    This routine allows the upper layer to start or stop a rekey phase in LBP
    module.

  Precondition:
    LBP_InitCoord must have been called before.

  Parameters:
    rekeyStart - Indicates whether rekey phase starts or finishes

  Returns:
    None.

  Example:
    <code>
    LBP_SetRekeyPhase(true);
    </code>

  Remarks:
    None.
*/
void LBP_SetRekeyPhase(bool rekeyStart);

// *****************************************************************************
/* Function:
    void LBP_ActivateNewKey(void);

  Summary:
    Activates the new GMK after a rekeying process.

  Description:
    This routine allows the upper layer to activate the new GMK after a rekeying
    process.

  Precondition:
    LBP_InitCoord must have been called before.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    LBP_ActivateNewKey();
    </code>

  Remarks:
    None.
*/
void LBP_ActivateNewKey(void);

// *****************************************************************************
/* Function:
    void LBP_SetParamCoord(uint32_t attributeId, uint16_t attributeIndex,
        uint8_t attributeLen, const uint8_t *pAttributeValue,
        LBP_SET_PARAM_CONFIRM *pSetConfirm);

  Summary:
    Sets the value of a parameter in LBP IB.

  Description:
    This routine allows the upper layer to set the value of a parameter in LBP
    IB.

  Precondition:
    LBP_InitCoord must have been called before.

  Parameters:
    attributeId     - LBP attribute identifier to set

    attributeIndex  - Index to set when attribute is a list

    attributeLen    - Length of value to set

    pAttributeValue - Pointer to array containing value

    pSetConfirm     - Pointer to result information of the set operation

  Returns:
    None.

  Example:
    <code>
    uint8_t gmk[16];
    LBP_SET_PARAM_CONFIRM setConfirm;

    LBP_SetParamCoord(LBP_IB_GMK, 0, 16, &gmk, &setConfirm);

    if (setConfirm.status == LBP_STATUS_OK)
    {

    }
    </code>

  Remarks:
    None.
*/
void LBP_SetParamCoord(uint32_t attributeId, uint16_t attributeIndex,
    uint8_t attributeLen, const uint8_t *pAttributeValue,
    LBP_SET_PARAM_CONFIRM *pSetConfirm);

// *****************************************************************************
/* Function:
    void LBP_ShortAddressAssign(uint8_t *pExtAddress, uint16_t assignedAddress);

  Summary:
    Assigns a short address to a device willing to join.

  Description:
    This primitive allows the upper layer to assign a short address to a device
    willing to join.

  Precondition:
    LBP_InitCoord must have been called before.

  Parameters:
    pExtAddress     - Pointer to the extended address of device willing to join

    assignedAddress - The network address to be assigned. Set to 0xFFFF to
                      reject device.

  Returns:
    None.

  Example:
    <code>
    App_JoinRequestIndication(uint8_t* pLbdAddress)
    {
        uint16_t assignedAddress;

        assignedAddress = 0x0001;
        LBP_ShortAddressAssign(pLbdAddress, assignedAddress);
    }
    </code>

  Remarks:
    None.
*/
void LBP_ShortAddressAssign(uint8_t *pExtAddress, uint16_t assignedAddress);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // #ifndef LBP_COORD_H
