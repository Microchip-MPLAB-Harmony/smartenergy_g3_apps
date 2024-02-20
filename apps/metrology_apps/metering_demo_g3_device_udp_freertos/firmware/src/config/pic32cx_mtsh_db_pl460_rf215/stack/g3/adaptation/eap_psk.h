/*******************************************************************************
  LBP EAP-PSK Protocol Header File

  Company:
    Microchip Technology Inc.

  File Name:
    eap_psk.h

  Summary:
    LBP EAP-PSK Protocol Header File.

  Description:
    The LoWPAN Bootstrapping Protocol (LBP) provides a simple interface to
    manage the G3 boostrap process Adaptation Layer. This file provides the
    interface to manage EAP-PSK protocol.
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

#ifndef EAP_PSK_H
#define EAP_PSK_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************
#include "lbp_defs.h"

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

/* EAP message types */
/* The value takes in account the 2 reserved bits (values are left shifted by 2 bits) */
#define EAP_REQUEST                    0x04U
#define EAP_RESPONSE                   0x08U
#define EAP_SUCCESS                    0x0CU
#define EAP_FAILURE                    0x10U

/* T-subfield types */
/* 0 The first EAP-PSK message */
#define EAP_PSK_T0                     (0x00U << 6)
/* 1 The second EAP-PSK message */
#define EAP_PSK_T1                     (0x01U << 6)
/* 2 The third EAP-PSK message */
#define EAP_PSK_T2                     (0x02U << 6)
/* 3 The fourth EAP-PSK message */
#define EAP_PSK_T3                     (0x03U << 6)

/* P-Channel result field */
#define PCHANNEL_RESULT_CONTINUE       0x01U
#define PCHANNEL_RESULT_DONE_SUCCESS   0x02U
#define PCHANNEL_RESULT_DONE_FAILURE   0x03U

// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* EAP_PSK NetworkAccessIdentifier P type

  Summary:
    Network Access Identifier for Bootstrapping Peer.

  Description:
    Network Access Identifier for Bootstrapping Peer.

  Remarks:
    None.
*/
typedef struct
{
    uint8_t size;
    uint8_t value[LBP_NETWORK_ACCESS_ID_MAX_SIZE_P];

} EAP_PSK_NETWORK_ACCESS_ID_P;

// *****************************************************************************
/* EAP_PSK NetworkAccessIdentifier S type

  Summary:
    Network Access Identifier for Bootstrapping Server.

  Description:
    Network Access Identifier for Bootstrapping Server.

  Remarks:
    None.
*/
typedef struct
{
    uint8_t size;
    uint8_t value[LBP_NETWORK_ACCESS_ID_MAX_SIZE_S];

} EAP_PSK_NETWORK_ACCESS_ID_S;

// *****************************************************************************
/* The EAP_PSK key type

  Summary:
    Bootstrapping Preshared Key.

  Description:
    Bootstrapping Preshared Key.

  Remarks:
    None.
*/
typedef struct
{
    uint8_t value[16];

} EAP_PSK_KEY;

// *****************************************************************************
/* The EAP_PSK MSK key type

  Summary:
    Bootstrapping Master Session Key.

  Description:
    Bootstrapping Master Session Key.

  Remarks:
    None.
*/
typedef struct
{
    uint8_t value[64];

} EAP_PSK_MSK;

// *****************************************************************************
/* The EAP_PSK RAND type

  Summary:
    Bootstrapping Random value derived from Keys.

  Description:
    Bootstrapping Random value derived from Keys.

  Remarks:
    None.
*/
typedef struct
{
    uint8_t value[16];

} EAP_PSK_RAND;

// *****************************************************************************
/* The EAP_PSK Context

  Summary:
    Contains the information needed during the EAP-PSK.

  Description:
    Contains the information needed during the EAP-PSK Bootstrapping process.

  Remarks:
    None.
*/
typedef struct
{
    EAP_PSK_KEY kdk; /* Derivation key */
    EAP_PSK_KEY ak; /* Authentication key */
    EAP_PSK_KEY tek; /* Transient key */
    EAP_PSK_MSK msk; /* Master Session key */
    EAP_PSK_NETWORK_ACCESS_ID_S idS;
    EAP_PSK_RAND randP;
    EAP_PSK_RAND randS;
} EAP_PSK_CONTEXT;

// *****************************************************************************
// *****************************************************************************
// Section: Interface Function Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    void EAP_PSK_Initialize(EAP_PSK_KEY *pKey, EAP_PSK_CONTEXT *pPskContext)

  Summary:
    Initializes EAP-PSK module.

  Description:
    This routine initializes the PSK Context filling the Derivation Key and
    Authentication Key from a given PSK.

  Precondition:
    None.

  Parameters:
    pKey - Pointer to shared secret (PSK)
    pPskContext - Pointer to EAP-PSK context to be filled deriving PSK

  Returns:
    None.

  Example:
    <code>
    EAP_PSK_KEY psk;
    EAP_PSK_CONTEXT eapCtx;

    EAP_PSK_Initialize(&psk, &eapCtx);
    </code>

  Remarks:
    None.
*/
void EAP_PSK_Initialize(EAP_PSK_KEY *pKey, EAP_PSK_CONTEXT *pPskContext);

// *****************************************************************************
/* Function:
    void EAP_PSK_InitializeTEKMSK(
        EAP_PSK_RAND *pRandP,
        EAP_PSK_CONTEXT *pPskContext
        )

  Summary:
    Fills pending EAP-PSK context fields.

  Description:
    This routine fills the Transient Key and Master Session Key on PSK Context
    and encrypts a Random number array using the derivation key.

  Precondition:
    None.

  Parameters:
    pRandP - Pointer to Random numbers array container
    pPskContext - Pointer to EAP-PSK context to be filled

  Returns:
    None.

  Example:
    <code>
    EAP_PSK_KEY psk;
    EAP_PSK_RAND randP;
    EAP_PSK_CONTEXT eapCtx;

    EAP_PSK_Initialize(&psk, &eapCtx);
    EAP_PSK_InitializeTEKMSK(&randP, &eapCtx);
    </code>

  Remarks:
    None.
*/
void EAP_PSK_InitializeTEKMSK(
    EAP_PSK_RAND *pRandP,
    EAP_PSK_CONTEXT *pPskContext
    );

// *****************************************************************************
/* Function:
    bool EAP_PSK_DecodeMessage(
        uint16_t messageLength,
        uint8_t *pMessage,
        uint8_t *pCode,
        uint8_t *pIdentifier,
        uint8_t *pTSubfield,
        uint16_t *pEAPDataLength,
        uint8_t **pEAPData
        )

  Summary:
    Extracts header fields of an EAP-PSK frame.

  Description:
    This routine extracts Code, Identifier and TSubfield from an EAP-PSK frame
    and provides a pointer and length to EAP payload.

  Precondition:
    None.

  Parameters:
    messageLength - EAP frame length
    pMessage - Pointer to EAP frame
    pCode - Code field extracted from frame
    pIdentifier - Identifier field extracted from frame
    pTSubfield - T subfield extracted from frame
    pEAPDataLength - EAP payload length
    pEAPData - Pointer to EAP payload

  Returns:
    True if decode is successful, otherwise False.

  Example:
    <code>
    void EAP_FrameReceived(uint16_t messageLength, uint8_t *pMessage)
    {
        uint8_t code;
        uint8_t identifier;
        uint8_t tSubfield;
        uint16_t EAPDataLength;
        uint8_t *EAPData;
        bool decodeOK;

        decodeOK = EAP_PSK_DecodeMessage(messageLength, pMessage,
            &code, &identifier, &tSubfield, &EAPDataLength, &EAPData);
    }
    </code>

  Remarks:
    None.
*/
bool EAP_PSK_DecodeMessage(
    uint16_t messageLength,
    uint8_t *pMessage,
    uint8_t *pCode,
    uint8_t *pIdentifier,
    uint8_t *pTSubfield,
    uint16_t *pEAPDataLength,
    uint8_t **pEAPData
    );

// *****************************************************************************
/* Function:
    bool EAP_PSK_DecodeMessage1(
        uint16_t messageLength,
        uint8_t *pMessage,
        EAP_PSK_RAND *pRandS,
        EAP_PSK_NETWORK_ACCESS_ID_S *pIdS
        )

  Summary:
    Extracts fields from payload of EAP message #1 (Type 0).

  Description:
    This routine extracts the fields (RandS and IdS) from EAP payload
    of EAP message #1.

  Precondition:
    None.

  Parameters:
    messageLength - EAP payload length
    pMessage - Pointer to EAP payload
    pRandS - RandS field extracted from frame
    pIdS - IdS field extracted from frame

  Returns:
    True if decode is successful, otherwise False.

  Example:
    <code>
    void EAP_FrameReceived(uint16_t messageLength, uint8_t *pMessage)
    {
        uint8_t code;
        uint8_t identifier;
        uint8_t tSubfield;
        uint16_t EAPDataLength;
        uint8_t *EAPData;
        bool decodeOK;

        EAP_PSK_RAND randS;
        EAP_PSK_NETWORK_ACCESS_ID_S idS;

        decodeOK = EAP_PSK_DecodeMessage(messageLength, pMessage,
            &code, &identifier, &tSubfield, &EAPDataLength, &EAPData);

        if (decodeOK && (tSubfield == EAP_PSK_T0)) {
            decodeOK = EAP_PSK_DecodeMessage1(EAPDataLength, EAPData,
                &randS, &idS);
        }
    }
    </code>

  Remarks:
    None.
*/
bool EAP_PSK_DecodeMessage1(
    uint16_t messageLength,
    uint8_t *pMessage,
    EAP_PSK_RAND *pRandS,
    EAP_PSK_NETWORK_ACCESS_ID_S *pIdS
    );

// *****************************************************************************
/* Function:
    uint16_t EAP_PSK_EncodeMessage2(
        EAP_PSK_CONTEXT *pPskContext,
        uint8_t identifier,
        EAP_PSK_RAND *pRandS,
        EAP_PSK_RAND *pRandP,
        EAP_PSK_NETWORK_ACCESS_ID_S *pIdS,
        EAP_PSK_NETWORK_ACCESS_ID_P *pIdP,
        uint16_t memoryBufferLength,
        uint8_t *pMemoryBuffer
        )

  Summary:
    Encodes an EAP message #2 frame (Type 1).

  Description:
    This routine encodes the fields of an EAP message #2 into a given buffer.

  Precondition:
    None.

  Parameters:
    pPskContext - Pointer to PSK context
    identifier - EAP identifier
    pRandS - Pointer to RandS field
    pRandP - Pointer to RandP field
    pIdS - Pointer to IdS field
    pIdP - Pointer to IdP field
    memoryBufferLength - Length of buffer on which frame is encoded
    pMemoryBuffer - Pointer to buffer on which frame is encoded

  Returns:
    Encoded length, or 0 if encoding failed.

  Example:
    <code>
    void EAP_FrameReceived(uint16_t messageLength, uint8_t *pMessage)
    {
        uint8_t code;
        uint8_t identifier;
        uint8_t tSubfield;
        uint16_t EAPDataLength;
        uint8_t *EAPData;
        uint16_t txBufLen = sizeof(encodeBuffer);
        uint8_t *txBuf = &encodeBuffer[0];
        bool decodeOK;

        EAP_PSK_RAND randS;
        EAP_PSK_NETWORK_ACCESS_ID_S idS;

        decodeOK = EAP_PSK_DecodeMessage(messageLength, pMessage,
            &code, &identifier, &tSubfield, &EAPDataLength, &EAPData);

        if (decodeOK && (tSubfield == EAP_PSK_T0)) {
            decodeOK = EAP_PSK_DecodeMessage1(EAPDataLength, EAPData,
                &randS, &idS);

            if (decodeOK) {
                uint16_t encodedLen;
                EAP_PSK_RAND randP = {Peer random sequence};
                EAP_PSK_NETWORK_ACCESS_ID_S idP = {Peer identifier};

                encodedLen = EAP_PSK_EncodeMessage2(&PskContext,
                    identifier, &randS, &randP, &idS, &idP, txBufLen, txBuf);
            }
        }
    }
    </code>

  Remarks:
    None.
*/
uint16_t EAP_PSK_EncodeMessage2(
    EAP_PSK_CONTEXT *pPskContext,
    uint8_t identifier,
    EAP_PSK_RAND *pRandS,
    EAP_PSK_RAND *pRandP,
    EAP_PSK_NETWORK_ACCESS_ID_S *pIdS,
    EAP_PSK_NETWORK_ACCESS_ID_P *pIdP,
    uint16_t memoryBufferLength,
    uint8_t *pMemoryBuffer
    );

// *****************************************************************************
/* Function:
    bool EAP_PSK_DecodeMessage3(
        uint16_t messageLength,
        uint8_t *pMessage,
        EAP_PSK_CONTEXT *pPskContext,
        uint16_t headerLength,
        uint8_t *pHeader,
        EAP_PSK_RAND *pRandS,
        uint32_t *pNonce,
        uint8_t *pPChannelResult,
        uint16_t *pPChannelDataLength,
        uint8_t **pPChannelData)

  Summary:
    Extracts fields from payload of EAP message #3 (Type 2).

  Description:
    This routine extracts the fields (RandS, Nonce and PChannel Result)
    from EAP payload of EAP message #3 and returns a pointer and lengh to
    Protected Channel data.

  Precondition:
    None.

  Parameters:
    messageLength - EAP payload length
    pMessage - Pointer to EAP payload
    pPskContext - Pointer to PSK context
    headerLength - Length of the header field
    pHeader - Pointer to header field, used to compute the Authentication Tag
    pRandS - RandS field extracted from frame
    pNonce - Nonce field extracted from frame
    pPChannelResult - Protected Channel result
    pPChannelDataLength - Protected Channel length
    pPChannelData - Pointer to Protected Channel data

  Returns:
    True if decode is successful, otherwise False.

  Example:
    <code>
    void EAP_FrameReceived(uint16_t messageLength, uint8_t *pMessage)
    {
        uint8_t code;
        uint8_t identifier;
        uint8_t tSubfield;
        uint16_t EAPDataLength;
        uint8_t *EAPData;
        bool decodeOK;

        EAP_PSK_RAND randS;
        uint16_t headerLength = HEADER_LEN;
        uint8_t *header = &headerBuf[0];
        uint32_t nonce;
        uint8_t channelRes;
        uint16_t chnDataLength;
        uint8_t *chnData;

        decodeOK = EAP_PSK_DecodeMessage(messageLength, pMessage,
            &code, &identifier, &tSubfield, &EAPDataLength, &EAPData);

        if (decodeOK && (tSubfield == EAP_PSK_T2)) {
            decodeOK = EAP_PSK_DecodeMessage3(EAPDataLength, EAPData,
                &PskContext, headerLength, header, &randS, &nonce,
                &channelRes, &chnDataLength, &chnData);
        }
    }
    </code>

  Remarks:
    None.
*/
bool EAP_PSK_DecodeMessage3(
    uint16_t messageLength,
    uint8_t *pMessage,
    EAP_PSK_CONTEXT *pPskContext,
    uint16_t headerLength,
    uint8_t *pHeader,
    EAP_PSK_RAND *pRandS,
    uint32_t *pNonce,
    uint8_t *pPChannelResult,
    uint16_t *pPChannelDataLength,
    uint8_t **pPChannelData
    );

// *****************************************************************************
/* Function:
    uint16_t EAP_PSK_EncodeMessage4(
        EAP_PSK_CONTEXT *pPskContext,
        uint8_t identifier,
        EAP_PSK_RAND *pRandS,
        uint32_t nonce,
        uint8_t PChannelResult,
        uint16_t PChannelDataLength,
        uint8_t *pPChannelData,
        uint16_t memoryBufferLength,
        uint8_t *pMemoryBuffer)

  Summary:
    Encodes an EAP message #4 frame (Type 3).

  Description:
    This routine encodes the fields of an EAP message #4 into a given buffer.

  Precondition:
    None.

  Parameters:
    pPskContext - Pointer to PSK context
    identifier - EAP identifier
    pRandS - Pointer to RandS field
    nonce - Nonce field
    PChannelResult - Protected Channel result
    PChannelDataLength - Protected Channel length
    PChannelData - Pointer to Protected Channel data
    memoryBufferLength - Length of buffer on which frame is encoded
    pMemoryBuffer - Pointer to buffer on which frame is encoded

  Returns:
    Encoded length, or 0 if encoding failed.

  Example:
    <code>
    void EAP_FrameReceived(uint16_t messageLength, uint8_t *pMessage)
    {
        uint8_t code;
        uint8_t identifier;
        uint8_t tSubfield;
        uint16_t EAPDataLength;
        uint8_t *EAPData;
        uint16_t txBufLen = sizeof(encodeBuffer);
        uint8_t *txBuf = &encodeBuffer[0];
        bool decodeOK;

        EAP_PSK_RAND randS;
        uint16_t headerLength = HEADER_LEN;
        uint8_t *header = &headerBuf[0];
        uint32_t nonce;
        uint8_t channelRes;
        uint16_t chnDataLength;
        uint8_t *chnData;

        decodeOK = EAP_PSK_DecodeMessage(messageLength, pMessage,
            &code, &identifier, &tSubfield, &EAPDataLength, &EAPData);

        if (decodeOK && (tSubfield == EAP_PSK_T2)) {
            decodeOK = EAP_PSK_DecodeMessage3(EAPDataLength, EAPData,
                &PskContext, headerLength, header, &randS, &nonce,
                &channelRes, &chnDataLength, &chnData);

            if (decodeOK) {
                uint16_t encodedLen;

                encodedLen = EAP_PSK_EncodeMessage4(&PskContext,
                    identifier, &randS, nonce, channelRes,
                    chnDataLength, chnData, txBufLen, txBuf);
            }
        }
    }
    </code>

  Remarks:
    None.
*/
uint16_t EAP_PSK_EncodeMessage4(
    EAP_PSK_CONTEXT *pPskContext,
    uint8_t identifier,
    EAP_PSK_RAND *pRandS,
    uint32_t nonce,
    uint8_t PChannelResult,
    uint16_t PChannelDataLength,
    uint8_t *pPChannelData,
    uint16_t memoryBufferLength,
    uint8_t *pMemoryBuffer
    );

// *****************************************************************************
/* Function:
    uint16_t EAP_PSK_EncodeMessage1(
        uint8_t identifier,
        const EAP_PSK_RAND *pRandS,
        const EAP_PSK_NETWORK_ACCESS_ID_S *pIdS,
        uint16_t memoryBufferLength,
        uint8_t *pMemoryBuffer
        )

  Summary:
    Encodes an EAP message #1 frame (Type 0).

  Description:
    This routine encodes the fields of an EAP message #1 into a given buffer.

  Precondition:
    None.

  Parameters:
    identifier - EAP identifier
    pRandS - Pointer to RandS field
    pIdS - Pointer to IdS field
    memoryBufferLength - Length of buffer on which frame is encoded
    pMemoryBuffer - Pointer to buffer on which frame is encoded

  Returns:
    Encoded length, or 0 if encoding failed.

  Example:
    <code>
    void NodeJoining(void)
    {
        uint8_t identifier;
        EAP_PSK_RAND randS;
        EAP_PSK_NETWORK_ACCESS_ID_S idS;
        uint16_t txBufLen = sizeof(encodeBuffer);
        uint8_t *txBuf = &encodeBuffer[0];

        encodedLen = EAP_PSK_EncodeMessage1(identifier, &randS, &idS, txBufLen, txBuf);
    }
    </code>

  Remarks:
    None.
*/
uint16_t EAP_PSK_EncodeMessage1(
    uint8_t identifier,
    const EAP_PSK_RAND *pRandS,
    const EAP_PSK_NETWORK_ACCESS_ID_S *pIdS,
    uint16_t memoryBufferLength,
    uint8_t *pMemoryBuffer
    );

// *****************************************************************************
/* Function:
    bool EAP_PSK_DecodeMessage2(
        bool aribBand,
        uint16_t messageLength,
        uint8_t *pMessage,
        EAP_PSK_CONTEXT *pPskContext,
        EAP_PSK_NETWORK_ACCESS_ID_S *pIdS,
        EAP_PSK_RAND *pRandS,
        EAP_PSK_RAND *pRandP
        )

  Summary:
    Extracts fields from payload of EAP message #2 (Type 1).

  Description:
    This routine extracts the fields (IdS, RandS, and RandP)
    from EAP payload of EAP message #2.

  Precondition:
    None.

  Parameters:
    aribBand - Flag indicating whether ARIB band is used (fields size change)
    messageLength - EAP payload length
    pMessage - Pointer to EAP payload
    pPskContext - Pointer to PSK context
    pIdS - IdS field extracted from frame
    pRandS - RandS field extracted from frame
    pRandP - RandP field extracted from frame

  Returns:
    True if decode is successful, otherwise False.

  Example:
    <code>
    void EAP_FrameReceived(uint16_t messageLength, uint8_t *pMessage)
    {
        uint8_t code;
        uint8_t identifier;
        uint8_t tSubfield;
        uint16_t EAPDataLength;
        uint8_t *EAPData;
        bool decodeOK;

        bool arib;
        EAP_PSK_NETWORK_ACCESS_ID_S idS;
        EAP_PSK_RAND randS;
        EAP_PSK_RAND randP;
        uint16_t headerLength = HEADER_LEN;
        uint8_t *header = &headerBuf[0];
        uint32_t nonce;
        uint8_t channelRes;
        uint16_t chnDataLength;
        uint8_t *chnData;

        decodeOK = EAP_PSK_DecodeMessage(messageLength, pMessage,
            &code, &identifier, &tSubfield, &EAPDataLength, &EAPData);

        if (decodeOK && (tSubfield == EAP_PSK_T1)) {
            decodeOK = EAP_PSK_DecodeMessage2(arib, EAPDataLength, EAPData,
                &PskContext, &idS, &randS, &randP);
        }
    }
    </code>

  Remarks:
    None.
*/
bool EAP_PSK_DecodeMessage2(
    bool aribBand,
    uint16_t messageLength,
    uint8_t *pMessage,
    EAP_PSK_CONTEXT *pPskContext,
    EAP_PSK_NETWORK_ACCESS_ID_S *pIdS,
    EAP_PSK_RAND *pRandS,
    EAP_PSK_RAND *pRandP
    );

// *****************************************************************************
/* Function:
    uint16_t EAP_PSK_EncodeMessage3(
        EAP_PSK_CONTEXT *pPskContext,
        uint8_t identifier,
        EAP_PSK_RAND *pRandS,
        EAP_PSK_RAND *pRandP,
        EAP_PSK_NETWORK_ACCESS_ID_S *pIdS,
        uint32_t nonce,
        uint8_t PChannelResult,
        uint16_t PChannelDataLength,
        uint8_t *pPChannelData,
        uint16_t memoryBufferLength,
        uint8_t *pMemoryBuffer
        )

  Summary:
    Encodes an EAP message #3 frame (Type 2).

  Description:
    This routine encodes the fields of an EAP message #3 into a given buffer.

  Precondition:
    None.

  Parameters:
    pPskContext - Pointer to PSK context
    identifier - EAP identifier
    pRandS - Pointer to RandS field
    pRandP - Pointer to RandP field
    pIdS - Pointer to IdS field
    nonce - Nonce field
    PChannelResult - Protected Channel result
    PChannelDataLength - Protected Channel length
    PChannelData - Pointer to Protected Channel data
    memoryBufferLength - Length of buffer on which frame is encoded
    pMemoryBuffer - Pointer to buffer on which frame is encoded

  Returns:
    Encoded length, or 0 if encoding failed.

  Example:
    <code>
    void EAP_FrameReceived(uint16_t messageLength, uint8_t *pMessage)
    {
        uint8_t code;
        uint8_t identifier;
        uint8_t tSubfield;
        uint16_t EAPDataLength;
        uint8_t *EAPData;
        uint16_t txBufLen = sizeof(encodeBuffer);
        uint8_t *txBuf = &encodeBuffer[0];
        bool decodeOK;

        EAP_PSK_RAND randS;
        uint16_t headerLength = HEADER_LEN;
        uint8_t *header = &headerBuf[0];
        uint32_t nonce;
        uint8_t channelRes;
        uint16_t chnDataLength;
        uint8_t *chnData;

        decodeOK = EAP_PSK_DecodeMessage(messageLength, pMessage,
            &code, &identifier, &tSubfield, &EAPDataLength, &EAPData);

        if (decodeOK && (tSubfield == EAP_PSK_T2)) {
            decodeOK = EAP_PSK_DecodeMessage3(EAPDataLength, EAPData,
                &PskContext, headerLength, header, &randS, &nonce,
                &channelRes, &chnDataLength, &chnData);

            if (decodeOK) {
                uint16_t encodedLen;

                encodedLen = EAP_PSK_EncodeMessage4(&PskContext,
                    identifier, &randS, nonce, channelRes,
                    chnDataLength, chnData, txBufLen, txBuf);
            }
        }
    }
    void EAP_FrameReceived(uint16_t messageLength, uint8_t *pMessage)
    {
        uint8_t code;
        uint8_t identifier;
        uint8_t tSubfield;
        uint16_t EAPDataLength;
        uint8_t *EAPData;
        uint16_t txBufLen = sizeof(encodeBuffer);
        uint8_t *txBuf = &encodeBuffer[0];
        bool decodeOK;

        bool arib;
        EAP_PSK_NETWORK_ACCESS_ID_S idS;
        EAP_PSK_RAND randS;
        EAP_PSK_RAND randP;
        uint32_t nonce;
        uint8_t channelRes;
        uint16_t chnDataLength;
        uint8_t *chnData;

        decodeOK = EAP_PSK_DecodeMessage(messageLength, pMessage,
            &code, &identifier, &tSubfield, &EAPDataLength, &EAPData);

        if (decodeOK && (tSubfield == EAP_PSK_T1)) {
            decodeOK = EAP_PSK_DecodeMessage2(arib, EAPDataLength, EAPData,
                &PskContext, &idS, &randS, &randP);

            if (decodeOK) {
                uint16_t encodedLen;

                encodedLen = EAP_PSK_EncodeMessage3(&PskContext,
                    identifier, &randS, &randP, &idS, nonce, channelRes,
                    chnDataLength, chnData, txBufLen, txBuf);
            }
        }
    }
    </code>

  Remarks:
    None.
*/
uint16_t EAP_PSK_EncodeMessage3(
    EAP_PSK_CONTEXT *pPskContext,
    uint8_t identifier,
    EAP_PSK_RAND *pRandS,
    EAP_PSK_RAND *pRandP,
    EAP_PSK_NETWORK_ACCESS_ID_S *pIdS,
    uint32_t nonce,
    uint8_t PChannelResult,
    uint16_t PChannelDataLength,
    uint8_t *pPChannelData,
    uint16_t memoryBufferLength,
    uint8_t *pMemoryBuffer
    );

// *****************************************************************************
/* Function:
    bool EAP_PSK_DecodeMessage4(
        uint16_t messageLength,
        uint8_t *pMessage,
        EAP_PSK_CONTEXT *pPskContext,
        uint16_t headerLength,
        uint8_t *pHeader,
        EAP_PSK_RAND *pRandS,
        uint32_t *pNonce,
        uint8_t *pPChannelResult,
        uint16_t *pPChannelDataLength,
        uint8_t **pPChannelData
        )

  Summary:
    Extracts fields from payload of EAP message #4 (Type 3).

  Description:
    This routine extracts the fields (RandS, Nonce and PChannel Result)
    from EAP payload of EAP message #4 and returns a pointer and lengh to
    Protected Channel data.

  Precondition:
    None.

  Parameters:
    messageLength - EAP payload length
    pMessage - Pointer to EAP payload
    pPskContext - Pointer to PSK context
    headerLength - Length of the header field
    pHeader - Pointer to header field, used to compute the Authentication Tag
    pRandS - RandS field extracted from frame
    pNonce - Nonce field extracted from frame
    pPChannelResult - Protected Channel result
    pPChannelDataLength - Protected Channel length
    pPChannelData - Pointer to Protected Channel data

  Returns:
    True if decode is successful, otherwise False.

  Example:
    <code>
    void EAP_FrameReceived(uint16_t messageLength, uint8_t *pMessage)
    {
        uint8_t code;
        uint8_t identifier;
        uint8_t tSubfield;
        uint16_t EAPDataLength;
        uint8_t *EAPData;
        bool decodeOK;

        EAP_PSK_RAND randS;
        uint16_t headerLength = HEADER_LEN;
        uint8_t *header = &headerBuf[0];
        uint32_t nonce;
        uint8_t channelRes;
        uint16_t chnDataLength;
        uint8_t *chnData;

        decodeOK = EAP_PSK_DecodeMessage(messageLength, pMessage,
            &code, &identifier, &tSubfield, &EAPDataLength, &EAPData);

        if (decodeOK && (tSubfield == EAP_PSK_T3)) {
            decodeOK = EAP_PSK_DecodeMessage4(EAPDataLength, EAPData,
                &PskContext, headerLength, header, &randS, &nonce,
                &channelRes, &chnDataLength, &chnData);
        }
    }
    </code>

  Remarks:
    None.
*/
bool EAP_PSK_DecodeMessage4(
    uint16_t messageLength,
    uint8_t *pMessage,
    EAP_PSK_CONTEXT *pPskContext,
    uint16_t headerLength,
    uint8_t *pHeader,
    EAP_PSK_RAND *pRandS,
    uint32_t *pNonce,
    uint8_t *pPChannelResult,
    uint16_t *pPChannelDataLength,
    uint8_t **pPChannelData
    );

// *****************************************************************************
/* Function:
    uint16_t EAP_PSK_EncodeEAPSuccess(
        uint8_t identifier,
        uint16_t memoryBufferLength,
        uint8_t *pMemoryBuffer
        )

  Summary:
    Encodes an EAP Header carrying EAP_SUCCESS code.

  Description:
    This routine encodes the header fields of an EAP message to provide
    a EAP_SUCCESS result into a given buffer.

  Precondition:
    None.

  Parameters:
    identifier - EAP identifier
    memoryBufferLength - Length of buffer on which frame is encoded
    pMemoryBuffer - Pointer to buffer on which frame is encoded

  Returns:
    Encoded length, or 0 if encoding failed.

  Example:
    <code>
    void EAP_Success(void)
    {
        uint8_t identifier;
        uint16_t txBufLen = sizeof(encodeBuffer);
        uint8_t *txBuf = &encodeBuffer[0];
        uint16_t encodedLen;

        encodedLen = EAP_PSK_EncodeEAPSuccess(identifier, txBufLen, txBuf);
    }
    </code>

  Remarks:
    None.
*/
uint16_t EAP_PSK_EncodeEAPSuccess(
    uint8_t identifier,
    uint16_t memoryBufferLength,
    uint8_t *pMemoryBuffer
    );

// *****************************************************************************
/* Function:
    uint16_t EAP_PSK_EncodeEAPFailure(
        uint8_t identifier,
        uint16_t memoryBufferLength,
        uint8_t *pMemoryBuffer
        )

  Summary:
    Encodes an EAP Header carrying EAP_FAILURE code.

  Description:
    This routine encodes the header fields of an EAP message to provide
    a EAP_FAILURE result into a given buffer.

  Precondition:
    None.

  Parameters:
    identifier - EAP identifier
    memoryBufferLength - Length of buffer on which frame is encoded
    pMemoryBuffer - Pointer to buffer on which frame is encoded

  Returns:
    Encoded length, or 0 if encoding failed.

  Example:
    <code>
    void EAP_Failure(void)
    {
        uint8_t identifier;
        uint16_t txBufLen = sizeof(encodeBuffer);
        uint8_t *txBuf = &encodeBuffer[0];
        uint16_t encodedLen;

        encodedLen = EAP_PSK_EncodeEAPFailure(identifier, txBufLen, txBuf);
    }
    </code>

  Remarks:
    None.
*/
uint16_t EAP_PSK_EncodeEAPFailure(
    uint8_t identifier,
    uint16_t memoryBufferLength,
    uint8_t *pMemoryBuffer
    );

// *****************************************************************************
/* Function:
    uint16_t EAP_PSK_EncodeGMKActivation(
        uint8_t *pPChannelData,
        uint16_t memoryBufferLength,
        uint8_t *pMemoryBuffer
        )

  Summary:
    Encodes an EAP Header carrying PChannelData.

  Description:
    This routine encodes the header fields of an EAP message to provide
    a commnad contained in PChannelData into a given buffer.

  Precondition:
    None.

  Parameters:
    pPChannelData - Pointer to data to copy in EAP frame
    memoryBufferLength - Length of buffer on which frame is encoded
    pMemoryBuffer - Pointer to buffer on which frame is encoded

  Returns:
    Encoded length, or 0 if encoding failed.

  Example:
    <code>
    void EAP_ActivateGMK(void)
    {
        uint8_t pdata[3];
        uint8_t u8NewKeyIndex = 1;
        uint16_t txBufLen = sizeof(encodeBuffer);
        uint8_t *txBuf = &encodeBuffer[0];
        uint16_t encodedLen;

        pdata[0] = LBP_CONF_PARAM_GMK_ACTIVATION;
        pdata[1] = 0x01;
        pdata[2] = u8NewKeyIndex;

        encodedLen = EAP_PSK_EncodeGMKActivation(
            pdata, txBufLen, txBuf);
    }
    </code>

  Remarks:
    None.
*/
uint16_t EAP_PSK_EncodeGMKActivation(
    uint8_t *pPChannelData,
    uint16_t memoryBufferLength,
    uint8_t *pMemoryBuffer
    );

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // #ifndef EAP_PSK_H
