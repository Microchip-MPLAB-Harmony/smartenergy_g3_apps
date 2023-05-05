/*******************************************************************************
  LBP Definitions Header File

  Company:
    Microchip Technology Inc.

  File Name:
    lbp_defs.h

  Summary:
    LBP definitions Header File.

  Description:
    The LoWPAN Bootstrapping Protocol (LBP) provides a simple interface to
    manage the G3 boostrap process Adaptation Layer. This file provides types
    definition for the LBP.
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

#ifndef _LBP_DEFS_H
#define _LBP_DEFS_H

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
/* S Network Access Identifier Size Definition

   Summary:
    Defines the size in bytes of the S network access identifier for CENELEC and
    FCC bands.

   Description:
    This macro defines the size in bytes size of the S network access identifier
    for CENELEC and FCC bands.

   Remarks:
    None.
*/
#define LBP_NETWORK_ACCESS_ID_SIZE_S_CENELEC_FCC 8

// *****************************************************************************
/* P Network Access Identifier Size Definition

   Summary:
    Defines the size in bytes of the P network access identifier for CENELEC and
    FCC bands.

   Description:
    This macro defines the size in bytes size of the P network access identifier
    for CENELEC and FCC bands.

   Remarks:
    None.
*/
#define LBP_NETWORK_ACCESS_ID_SIZE_P_CENELEC_FCC 8

// *****************************************************************************
/* S Network Access Identifier Size Definition

   Summary:
    Defines the size in bytes of the S network access identifier for ARIB band.

   Description:
    This macro defines the size in bytes size of the S network access identifier
    for ARIB band.

   Remarks:
    None.
*/
#define LBP_NETWORK_ACCESS_ID_SIZE_S_ARIB        34

// *****************************************************************************
/* P Network Access Identifier Size Definition

   Summary:
    Defines the size in bytes of the S network access identifier for ARIB band.

   Description:
    This macro defines the size in bytes size of the S network access identifier
    for ARIB band.

   Remarks:
    None.
*/
#define LBP_NETWORK_ACCESS_ID_SIZE_P_ARIB        36

// *****************************************************************************
/* S Network Access Identifier Maximum Size Definition

   Summary:
    Defines the maximum size in bytes of the S network access identifier.

   Description:
    This macro defines the maximum size in bytes size of the S network access
    identifier.

   Remarks:
    None.
*/
#define LBP_NETWORK_ACCESS_ID_MAX_SIZE_S         LBP_NETWORK_ACCESS_ID_SIZE_S_ARIB

// *****************************************************************************
/* P Network Access Identifier Maximum Size Definition

   Summary:
    Defines the maximum size in bytes of the P network access identifier.

   Description:
    This macro defines the maximum size in bytes size of the P network access
    identifier.

   Remarks:
    None.
*/
#define LBP_NETWORK_ACCESS_ID_MAX_SIZE_P         LBP_NETWORK_ACCESS_ID_SIZE_P_ARIB

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* LBP Status Definition

   Summary:
    Identifies the list of errors returned by the LBP.

   Description:
    This enumeration identifies the list of errors returned by the LBP.

   Remarks:
    None.
*/
typedef enum
{
    LBP_STATUS_OK = 0,
    LBP_STATUS_NOK,
    LBP_STATUS_UNSUPPORTED_PARAMETER,
    LBP_STATUS_INVALID_INDEX,
    LBP_STATUS_INVALID_LENGTH,
    LBP_STATUS_INVALID_VALUE

} LBP_STATUS;

// *****************************************************************************
/* LBP PIB Attributes Definition

   Summary:
    Identifies the available LBP PIB attributes.

   Description:
    This enumeration identifies the list of available parameter information base
    (PIB) attributes for the LBP.

   Remarks:
    None.
*/
typedef enum
{
    LBP_IB_IDS         = 0x00000000,
    LBP_IB_IDP         = 0x00000001,
    LBP_IB_PSK         = 0x00000002,
    LBP_IB_GMK         = 0x00000003,
    LBP_IB_REKEY_GMK   = 0x00000004,
    LBP_IB_RANDP       = 0x00000005,
    LBP_IB_MSG_TIMEOUT = 0x00000006

} LBP_ATTRIBUTE;

// *****************************************************************************
/* LBP Set Confirm Parameters

   Summary:
    Defines the parameters for the result of setting a LBP parameter.

   Description:
    The structure contains the the result of setting a LBP parameter.

   Remarks:
    None.
*/
typedef struct
{
    /* The identifier of the IB attribute to set */
    uint32_t attributeId;

    /* The index within the table of the specified IB attribute */
    uint16_t attributeIndex;

    /* The status of the set request */
    LBP_STATUS status;

} LBP_SET_PARAM_CONFIRM;

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // #ifndef _LBP_DEFS_H
