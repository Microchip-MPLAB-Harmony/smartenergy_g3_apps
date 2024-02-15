/*******************************************************************************
  LBP Device Header File

  Company:
    Microchip Technology Inc.

  File Name:
    lbp_dev.h

  Summary:
    LBP for Device Header File.

  Description:
    The LoWPAN Bootstrapping Protocol (LBP) provides a simple interface to
    manage the G3 boostrap process Adaptation Layer. This file provides the
    interface to manage LBP process for device.
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

#ifndef LBP_DEV_H
#define LBP_DEV_H

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
/* LBP ADP Network Join Confirm Parameters

   Summary:
    Defines the parameters for the ADP Network Join Confirm event handler
    function.

   Description:
    The structure contains the fields reported by the ADP Network Join Confirm
    event handler function.

   Remarks:
    None.
*/
typedef struct
{
    /* The 16-bit network address that was allocated to the device */
    uint16_t networkAddress;

    /* The 16-bit address of the PAN of which the device is now a member */
    uint16_t panId;

    /* The status of the request */
    uint8_t status;

} LBP_ADP_NETWORK_JOIN_CFM_PARAMS;

// *****************************************************************************
/* LBP ADP Network Join Confirm Event Handler Function Pointer

  Summary:
    Pointer to a Network Join Confirm Event handler function.

  Description:
    This data type defines the required function signature for the ADP Network
    Join Confirm event handling callback function. The ADP Network Join Confirm
    primitive allows the upper layer to be notified of the completion of an ADP
    Network Join Request.

    A client must register a pointer using the event handling
    function whose function signature (parameter and return value types) matches
    the types specified by this function pointer in order to receive ADP Network
    Join Confirm events back from module.

  Parameters:
    pNetworkJoinCfm - Pointer to structure containing parameters related to confirm

  Example:
    <code>
    App_LbpJoinConfirm(LBP_ADP_NETWORK_JOIN_CFM_PARAMS *params)
    {
        if (params->status == G3_SUCCESS)
        {

        }
    }
    </code>

  Remarks:
    None.
*/
typedef void (*LBP_ADP_NETWORK_JOIN_CFM_CALLBACK)(
    LBP_ADP_NETWORK_JOIN_CFM_PARAMS* pNetworkJoinCfm);

// *****************************************************************************
/* LBP ADP Network Leave Indication Event Handler Function Pointer

  Summary:
    Pointer to a Network Leave Indication Event handler function.

  Description:
    This data type defines the required function signature for the ADP Network
    Leave Indication event handling callback function. The ADP Network Leave
    Indication primitive is is generated by the LBP layer of a non-coordinator
    device to inform the upper layer that it has been unregistered from the
    network by the coordinator.

    A client must register a pointer using the event handling function whose
    function signature (parameter and return value types) matches the types
    specified by this function pointer in order to receive ADP Network Leave
    Indication events back from module.

  Parameters:
    None.

  Example:
    <code>
    App_LbpLeaveIndication(void)
    {

    }
    </code>

  Remarks:
    None.
*/
typedef void (*LBP_ADP_NETWORK_LEAVE_IND_CALLBACK)(void);

// *****************************************************************************
/* LBP ADP Network Leave Confirm Event Handler Function Pointer

  Summary:
    Pointer to a Network Leave Confirm Event handler function.

  Description:
    This data type defines the required function signature for the ADP Network
    Leave Confirm event handling callback function. The ADP Network Leave
    Confirm primitive allows the upper layer to be notified of the completion of
    an ADP Network Leave Request.

    A client must register a pointer using the event handling function whose
    function signature (parameter and return value types) matches the types
    specified by this function pointer in order to receive ADP Network Leave
    Confirm events back from module.

  Parameters:
    status - The status of the request

  Example:
    <code>
    App_LbpLeaveConfirm(uint8_t status)
    {
        if (status == G3_SUCCESS)
        {

        }
    }
    </code>

  Remarks:
    None.
*/
typedef void (*LBP_ADP_NETWORK_LEAVE_CFM_CALLBACK)(uint8_t status);

// *****************************************************************************
/* LBP Device Callback Notificatios Structure

   Summary:
    Set of event handler function pointers to receive events from LBP
    Device.

   Description:
    Defines the set of callback functions that LBP Device uses to generate
    events to upper layer.

   Remarks:
    In case an event is to be ignored, setting its corresponding callback
    function to NULL will lead to the event not being generated.
*/
typedef struct {
    LBP_ADP_NETWORK_JOIN_CFM_CALLBACK adpNetworkJoinConfirm;
    LBP_ADP_NETWORK_LEAVE_CFM_CALLBACK adpNetworkLeaveConfirm;
    LBP_ADP_NETWORK_LEAVE_IND_CALLBACK adpNetworkLeaveIndication;

} LBP_NOTIFICATIONS_DEV;

// *****************************************************************************
/* Function:
    void LBP_InitDev(void);

  Summary:
    Restarts the Device LBP module.

  Description:
    This routine restarts the Device LBP module.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    ADP_Open(ADP_BAND_CENELEC_A);
    LBP_InitDev();
    </code>

  Remarks:
    This routine must be called before any other Device LBP API function.
*/
void LBP_InitDev(void);

// *****************************************************************************
/* Function:
    void LBP_SetNotificationsDev(LBP_NOTIFICATIONS_DEV *pNotifications);

  Summary:
    Sets Device LBP notifications.

  Description:
    This routine sets the Device LBP notifications. Callback handlers for event
    notification are set in this function.

  Precondition:
    LBP_InitDev must have been called before.

  Parameters:
    pNotifications - Structure with callbacks used to notify Device LBP
                     events

  Returns:
    None.

  Example:
    <code>
    LBP_NOTIFICATIONS_DEV lbpNotifications = {
        .adpNetworkJoinConfirm = appLbpJoinConfirm,
        .adpNetworkLeaveConfirm = appLbpLeaveConfirm,
        .adpNetworkLeaveIndication = appLbpLeaveIndication
    };

    LBP_SetNotificationsDev(&lbpNotifications);
    </code>

  Remarks:
    None.
*/
void LBP_SetNotificationsDev(LBP_NOTIFICATIONS_DEV *pNotifications);

// *****************************************************************************
/* Function:
    void LBP_TasksDev(void);

Summary:
    Maintains LBP Device state sachine.

  Description:
    This function maintains the LoWPAN Bootstrapping Protocol state machine in
    Device mode.

  Precondition:
    LBP_InitDev must have been called before.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    void APP_Tasks(void)
    {
        LBP_TasksDev();
    }
    </code>

  Remarks:
    None.
*/
void LBP_TasksDev(void);

// *****************************************************************************
/* Function:
    void LBP_SetParamDev(uint32_t attributeId, uint16_t attributeIndex,
        uint8_t attributeLen, const uint8_t *pAttributeValue,
        LBP_SET_PARAM_CONFIRM *pSetConfirm);

  Summary:
    Sets the value of a parameter in LBP IB.

  Description:
    This routine allows the upper layer to set the value of a parameter in LBP
    IB.

  Precondition:
    LBP_InitDev must have been called before.

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
    uint8_t psk[16];
    LBP_SET_PARAM_CONFIRM setConfirm;

    LBP_SetParamDev(LBP_IB_PSK, 0, 16, &psk, &setConfirm);

    if (setConfirm.status == LBP_STATUS_OK)
    {

    }
    </code>

  Remarks:
    None.
*/
void LBP_SetParamDev(uint32_t attributeId, uint16_t attributeIndex,
    uint8_t attributeLen, const uint8_t *pAttributeValue,
    LBP_SET_PARAM_CONFIRM *pSetConfirm);

// *****************************************************************************
/* Function:
    void LBP_ForceRegister(ADP_EXTENDED_ADDRESS *pEUI64Address,
        uint16_t shortAddress, uint16_t panId, ADP_GROUP_MASTER_KEY *pGMK);

  Summary:
    Forces the device register in the network without going through the
    bootstrap process.

  Description:
    This routine is used for testing purposes to force the device register in
    the network without going through the bootstrap process.

  Precondition:
    LBP_InitDev must have been called before.

  Parameters:
    pEUI64Address - Pointer to EUI64 address of the node

    shortAddress  - The 16-bit network address to be set

    panId         - The 16-bit PAN Id to be set

    pGMK          - Pointer to Group Master Key to set

  Returns:
    None.

  Example:
    <code>
    ADP_EXTENDED_ADDRESS eui64;
    ADP_GROUP_MASTER_KEY gmk;

    LBP_ForceRegister(&eui64, 0x0001, 0x1234, &gmk);
    </code>

  Remarks:
    None.
*/
void LBP_ForceRegister(ADP_EXTENDED_ADDRESS *pEUI64Address,
    uint16_t shortAddress, uint16_t panId, ADP_GROUP_MASTER_KEY *pGMK);

// *****************************************************************************
/* Function:
    void LBP_AdpNetworkJoinRequest(uint16_t panId, uint16_t lbaAddress,
        uint8_t mediaType);

  Summary:
    This primitive allows the upper layer to join an existing network.

  Description:
    The LBP Join Request primitive allows the upper layer to join an existing
    network.

    Result is provided in the corresponding LBP Join Confirm callback.

  Precondition:
    LBP_InitDev must have been called before.

  Parameters:
    panId      - The 16-bit PAN identifier of the network to join

    lbaAddress - The 16-bit short address of the device acting as a 6LowPAN
                 bootstrap agent (relay)

    mediaType  - The Media Type to use for frame exchange with LBA. Only used in
                 Hybrid Profile.

  Returns:
    None.

  Example:
    <code>
    App_DiscoveryIndication(ADP_PAN_DESCRIPTOR *panDescriptor)
    {
        uint8_t mediaType = MAC_WRP_MEDIA_TYPE_REQ_PLC_BACKUP_RF;

        if (panDescriptor-> == MAC_WRP_MEDIA_TYPE_IND_RF)
        {
            mediaType = MAC_WRP_MEDIA_TYPE_REQ_PLC_BACKUP_PLC;
        }

        LBP_AdpNetworkJoinRequest(panDescriptor->panId, panDescriptor->panId,
            mediaType);
    }
    </code>

  Remarks:
    None.
*/
void LBP_AdpNetworkJoinRequest(uint16_t panId, uint16_t lbaAddress,
    uint8_t mediaType);

// *****************************************************************************
/* Function:
    void LBP_AdpNetworkLeaveRequest(void);

  Summary:
    This primitive allows to remove itself from the network.

  Description:
    The LBP Leave Request primitive allows a non-coordinator device to remove
    itself from the network.

    Result is provided in the corresponding LBP Leave Confirm callback.

  Precondition:
    LBP_InitDev must have been called before.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    LBP_AdpNetworkLeaveRequest();
    </code>

  Remarks:
    None.
*/
void LBP_AdpNetworkLeaveRequest(void);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // #ifndef LBP_DEV_H
