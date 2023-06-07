/*******************************************************************************
  LBP Protocol Encode/Decode Header File

  Company:
    Microchip Technology Inc.

  File Name:
    lbp_encode_decode.h

  Summary:
    LBP Protocol Encode/Decode Header File.

  Description:
    This file contains the functions to Encode and Decode the LBP frames.
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

#ifndef _LBP_H
#define _LBP_H

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

/* LPB message types */
/* Fields T and Code are both considered */
/* The LBD requests joining a PAN and provides the necessary authentication material. */
#define LBP_JOINING                             0x01
/* Authentication succeeded with delivery of device specific information (DSI) to the LBD */
#define LBP_ACCEPTED                            0x09
/* Authentication in progress. PAN specific information (PSI) may be delivered to the LBD */
#define LBP_CHALLENGE                           0x0A
/* Authentication failed */
#define LBP_DECLINE                             0x0B
/* KICK frame is used by any device to inform the coordinator that it left the PAN. */
#define LBP_KICK_FROM_LBD                       0x04
/* KICK frame is used by a PAN coordinator to force a device to lose its MAC address */
#define LBP_KICK_TO_LBD                         0x0C

/* Parameter values */
/* Already encoded on 1 byte (M field and last bit included) */
#define LBP_CONF_PARAM_SHORT_ADDR               0x1D
#define LBP_CONF_PARAM_GMK                      0x27
#define LBP_CONF_PARAM_GMK_ACTIVATION           0x2B
#define LBP_CONF_PARAM_GMK_REMOVAL              0x2F
#define LBP_CONF_PARAM_RESULT                   0x31

/* Parameter result values */
#define LBP_RESULT_PARAMETER_SUCCESS            0x00
#define LBP_RESULT_MISSING_REQUIRED_PARAMETER   0x01
#define LBP_RESULT_INVALID_PARAMETER_VALUE      0x02
#define LBP_RESULT_UNKNOWN_PARAMETER_ID         0x03

/* Mask to track the presence of mandatory parameters */
#define LBP_CONF_PARAM_SHORT_ADDR_MASK          0x01
#define LBP_CONF_PARAM_GMK_MASK                 0x02
#define LBP_CONF_PARAM_GMK_ACTIVATION_MASK      0x04
#define LBP_CONF_PARAM_GMK_REMOVAL_MASK         0x08
#define LBP_CONF_PARAM_RESULT_MASK              0x10

// *****************************************************************************
// *****************************************************************************
// Section: Interface Function Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    uint16_t LBP_EncodeKickFromLBDRequest(
        const ADP_EXTENDED_ADDRESS *pEUI64Address,
        uint16_t bufferLength,
        uint8_t *pMessageBuffer)

  Summary:
    Encodes Kick from LBD.

  Description:
    This routine Encodes an LBP KICK frame sent from LBD to LBS.

  Precondition:
    None.

  Parameters:
    pEUI64Address - Pointer to 64bits address of LBD
    bufferLength - Length of buffer where frame will be encoded
    pMessageBuffer - Pointer to buffer where frame will be encoded

  Returns:
    Encoded length.

  Example:
    <code>
    uint16_t encodedLen;
    ADP_EXTENDED_ADDRESS myAddress;
    uint8_t encodeBuffer[100];

    void NetworkLeave(void)
    {
        encodedLen = LBP_EncodeKickFromLBDRequest(
            &myAddress, sizeof(encodeBuffer), &encodeBuffer);

        SendLbpFrame(encodedLen, &encodeBuffer);
    }
    </code>

  Remarks:
    None.
*/
uint16_t LBP_EncodeKickFromLBDRequest(
    const ADP_EXTENDED_ADDRESS *pEUI64Address,
    uint16_t bufferLength,
    uint8_t *pMessageBuffer);

// *****************************************************************************
/* Function:
    uint16_t LBP_EncodeKickToLBD(
        const ADP_EXTENDED_ADDRESS *pEUI64Address,
        uint16_t bufferLength,
        uint8_t *pMessageBuffer)

  Summary:
    Encodes Kick to LBD.

  Description:
    This routine Encodes an LBP KICK frame sent from LBS to LBD.

  Precondition:
    None.

  Parameters:
    pEUI64Address - Pointer to 64bits address of LBD
    bufferLength - Length of buffer where frame will be encoded
    pMessageBuffer - Pointer to buffer where frame will be encoded

  Returns:
    Encoded length.

  Example:
    <code>
    uint16_t encodedLen;
    uint8_t encodeBuffer[100];

    void KickDevice(ADP_EXTENDED_ADDRESS *deviceAddress)
    {
        encodedLen = LBP_EncodeKickFromLBDRequest(
            deviceAddress, sizeof(encodeBuffer), &encodeBuffer);

        SendLbpFrame(encodedLen, &encodeBuffer);
    }
    </code>

  Remarks:
    None.
*/
uint16_t LBP_EncodeKickToLBD(
    const ADP_EXTENDED_ADDRESS *pEUI64Address,
    uint16_t bufferLength,
    uint8_t *pMessageBuffer);

// *****************************************************************************
/* Function:
    uint16_t LBP_EncodeJoiningRequest(
        const ADP_EXTENDED_ADDRESS *pEUI64Address,
        uint8_t mediaType,
        uint8_t disableBackupMedium,
        uint16_t lbpDataLength,
        uint16_t bufferLength,
        uint8_t *pMessageBuffer)

  Summary:
    Encodes a JOINING frame from LBD.

  Description:
    This routine Encodes an LBP JOINING frame sent from LBD to LBS.

  Precondition:
    None.

  Parameters:
    pEUI64Address - Pointer to 64bits address of LBD
    mediaType - Medium on which to frame is to be sent
    disableBackupMedium - Enable/Disable backup medium usage
    lbpDataLength - LBP data length already present in encode buffer
    bufferLength - Length of buffer where frame will be encoded
    pMessageBuffer - Pointer to buffer where frame will be encoded

  Returns:
    Encoded length.

  Example:
    <code>
    uint16_t encodedLen;
    uint8_t mediaType;
    uint8_t disableBackupMedium;
    ADP_EXTENDED_ADDRESS myAddress;
    uint8_t encodeBuffer[100];

    void SendJoin(void)
    {
        encodedLen = EAP_PSK_Encode(...);

        encodedLen = LBP_EncodeJoiningRequest(
            &myAddress, mediaType, disableBackupMedium,
            encodedLen, sizeof(encodeBuffer), &encodeBuffer);

        SendLbpFrame(encodedLen, &encodeBuffer);
    }
    </code>

  Remarks:
    None.
*/
uint16_t LBP_EncodeJoiningRequest(
    const ADP_EXTENDED_ADDRESS *pEUI64Address,
    uint8_t mediaType,
    uint8_t disableBackupMedium,
    uint16_t lbpDataLength,
    uint16_t bufferLength,
    uint8_t *pMessageBuffer);

// *****************************************************************************
/* Function:
    uint16_t LBP_EncodeChallengeRequest(
        const ADP_EXTENDED_ADDRESS *pEUI64Address,
        uint8_t mediaType,
        uint8_t disableBackupMedium,
        uint16_t lbpDataLength,
        uint16_t bufferLength,
        uint8_t *pMessageBuffer)

  Summary:
    Encodes a CHALLENGE frame to LBD.

  Description:
    This routine Encodes an LBP CHALLENGE frame sent from LBS to LBD.

  Precondition:
    None.

  Parameters:
    pEUI64Address - Pointer to 64bits address of LBD
    mediaType - Medium on which to frame is to be sent
    disableBackupMedium - Enable/Disable backup medium usage
    lbpDataLength - LBP data length already present in encode buffer
    bufferLength - Length of buffer where frame will be encoded
    pMessageBuffer - Pointer to buffer where frame will be encoded

  Returns:
    Encoded length.

  Example:
    <code>
    uint16_t encodedLen;
    uint8_t mediaType;
    uint8_t disableBackupMedium;
    uint8_t encodeBuffer[100];

    void SendChallenge(ADP_EXTENDED_ADDRESS *devAddress)
    {
        encodedLen = EAP_PSK_Encode(...);

        encodedLen = LBP_EncodeJoiningRequest(
            devAddress, mediaType, disableBackupMedium,
            encodedLen, sizeof(encodeBuffer), &encodeBuffer);

        SendLbpFrame(encodedLen, &encodeBuffer);
    }
    </code>

  Remarks:
    None.
*/
uint16_t LBP_EncodeChallengeRequest(
    const ADP_EXTENDED_ADDRESS *pEUI64Address,
    uint8_t mediaType,
    uint8_t disableBackupMedium,
    uint16_t lbpDataLength,
    uint16_t bufferLength,
    uint8_t *pMessageBuffer);

// *****************************************************************************
/* Function:
    uint16_t LBP_EncodeAcceptedRequest(
        const ADP_EXTENDED_ADDRESS *pEUI64Address,
        uint8_t mediaType,
        uint8_t disableBackupMedium,
        uint16_t lbpDataLength,
        uint16_t bufferLength,
        uint8_t *pMessageBuffer)

  Summary:
    Encodes an ACCEPTED frame to LBD.

  Description:
    This routine Encodes an LBP ACCEPTED frame sent from LBS to LBD.

  Precondition:
    None.

  Parameters:
    pEUI64Address - Pointer to 64bits address of LBD
    mediaType - Medium on which to frame is to be sent
    disableBackupMedium - Enable/Disable backup medium usage
    lbpDataLength - LBP data length already present in encode buffer
    bufferLength - Length of buffer where frame will be encoded
    pMessageBuffer - Pointer to buffer where frame will be encoded

  Returns:
    Encoded length.

  Example:
    <code>
    uint16_t encodedLen;
    uint8_t mediaType;
    uint8_t disableBackupMedium;
    uint8_t encodeBuffer[100];

    void SendAccepted(ADP_EXTENDED_ADDRESS *devAddress)
    {
        encodedLen = EAP_PSK_Encode(...);

        encodedLen = LBP_EncodeAcceptedRequest(
            devAddress, mediaType, disableBackupMedium,
            encodedLen, sizeof(encodeBuffer), &encodeBuffer);

        SendLbpFrame(encodedLen, &encodeBuffer);
    }
    </code>

  Remarks:
    None.
*/
uint16_t LBP_EncodeAcceptedRequest(
    const ADP_EXTENDED_ADDRESS *pEUI64Address,
    uint8_t mediaType,
    uint8_t disableBackupMedium,
    uint16_t lbpDataLength,
    uint16_t bufferLength,
    uint8_t *pMessageBuffer);

// *****************************************************************************
/* Function:
    uint16_t LBP_EncodeDecline(
        const ADP_EXTENDED_ADDRESS *pEUI64Address,
        uint8_t mediaType,
        uint8_t disableBackupMedium,
        uint8_t EAPIdentifier,
        uint16_t bufferLength,
        uint8_t *pMessageBuffer)

  Summary:
    Encodes a DECLINE frame to LBD.

  Description:
    This routine Encodes an LBP DECLINE frame sent from LBS to LBD.

  Precondition:
    None.

  Parameters:
    pEUI64Address - Pointer to 64bits address of LBD
    mediaType - Medium on which to frame is to be sent
    disableBackupMedium - Enable/Disable backup medium usage
    EAPIdentifier - EAP Identifier to use in EAP Failure encoding
    bufferLength - Length of buffer where frame will be encoded
    pMessageBuffer - Pointer to buffer where frame will be encoded

  Returns:
    Encoded length.

  Example:
    <code>
    uint16_t encodedLen;
    uint8_t mediaType;
    uint8_t disableBackupMedium;
    uint8_t identifier;
    uint8_t encodeBuffer[100];

    void SendDecline(ADP_EXTENDED_ADDRESS *devAddress)
    {
        encodedLen = LBP_EncodeDecline(
            devAddress, mediaType, disableBackupMedium,
            identifier, sizeof(encodeBuffer), &encodeBuffer);

        SendLbpFrame(encodedLen, &encodeBuffer);
    }
    </code>

  Remarks:
    None.
*/
uint16_t LBP_EncodeDecline(
    const ADP_EXTENDED_ADDRESS *pEUI64Address,
    uint8_t mediaType,
    uint8_t disableBackupMedium,
    uint8_t EAPIdentifier,
    uint16_t bufferLength,
    uint8_t *pMessageBuffer);

// *****************************************************************************
/* Function:
    bool LBP_DecodeMessage(
        uint16_t bufferLength,
        uint8_t *pMessageBuffer,
        uint8_t *pMessageType,
        ADP_EXTENDED_ADDRESS *pEUI64Address,
        uint16_t *pLbpDataLength,
        uint8_t **pLbpData)

  Summary:
    Decodes an LBP frame.

  Description:
    This routine decodes an LBP frame and extracts its contents.

  Precondition:
    None.

  Parameters:
    bufferLength - Length of buffer where frame is contained
    pMessageBuffer - Pointer to buffer where frame is contained
    pMessageType - Message Type extracted from LBP frame
    pEUI64Address - 64bits address of LBD, extracted from LBP frame
    pLbpDataLength - LBP data length in decoded frame
    pLbpData - Pointer to LBP data in decoded frame

  Returns:
    True if frame is correctly decoded, otherwise False.

  Example:
    <code>
    uint16_t decodedLen;
    uint8_t lbpType;
    ADP_EXTENDED_ADDRESS devAddress;
    uint8_t *decodeBuffer;

    void LbpFrameReceived(uint16_t frameLen, uint8_t *frame)
    {
        bool decodeResult;

        decodeResult = LBP_DecodeMessage(
            frameLen, frame, &lbpType,
            &devAddress, &decodedLen,
            &decodeBuffer);

        if (decodeResult)
        {
            // Decoding success, handle frame
        }
    }
    </code>

  Remarks:
    None.
*/
bool LBP_DecodeMessage(
    uint16_t bufferLength,
    uint8_t *pMessageBuffer,
    uint8_t *pMessageType,
    ADP_EXTENDED_ADDRESS *pEUI64Address,
    uint16_t *pLbpDataLength,
    uint8_t **pLbpData);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // #ifndef _LBP_H
