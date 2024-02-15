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

#ifndef LBP_DEFS_H
#define LBP_DEFS_H

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
#define LBP_NETWORK_ACCESS_ID_SIZE_S_CENELEC_FCC 8U

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
#define LBP_NETWORK_ACCESS_ID_SIZE_P_CENELEC_FCC 8U

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
#define LBP_NETWORK_ACCESS_ID_SIZE_S_ARIB        34U

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
#define LBP_NETWORK_ACCESS_ID_SIZE_P_ARIB        36U

/* MISRA C-2012 deviation block start */
/* MISRA C-2012 Rule 5.4 deviated once.  Deviation record ID - H3_MISRAC_2012_R_5_4_DR_1 */

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

/* MISRA C-2012 deviation block end */

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

#endif // #ifndef LBP_DEFS_H
