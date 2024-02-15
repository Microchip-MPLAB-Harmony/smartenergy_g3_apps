/*******************************************************************************
  USI Service Definitions Header File

  Company:
    Microchip Technology Inc.

  File Name:
    srv_usi_definitions.h

  Summary:
    USI Service Definitions Header File

  Description:
    This file provides implementation-specific definitions for the USI
    service's system interface.
*******************************************************************************/

//DOM-IGNORE-BEGIN
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
//DOM-IGNORE-END

#ifndef SRV_USI_DEFINITIONS_H
#define SRV_USI_DEFINITIONS_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

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

/* MISRA C-2012 deviation block start */
/* MISRA C-2012 Rule 5.2 deviated twice.  Deviation record ID - H3_MISRAC_2012_R_5_2_DR_1 */

// *****************************************************************************
/* USI Service Serial Protocol Identifiers

  Summary:
    List of available USI protocols.

  Description:
    This data type defines the identifier required to specify the protocol to
    use by USI service.

  Remarks:
    None.
*/

typedef enum
{
    /* PRIME Manager generic */
    SRV_USI_PROT_ID_MNGP_PRIME                = 0x00,

    /* PRIME Manager: GETQRY */
    SRV_USI_PROT_ID_MNGP_PRIME_GETQRY         = 0x00,

    /* PRIME Manager: GETRSP */
    SRV_USI_PROT_ID_MNGP_PRIME_GETRSP         = 0x01,

    /* PRIME Manager: SET */
    SRV_USI_PROT_ID_MNGP_PRIME_SET            = 0x02,

    /* PRIME Manager: RESET */
    SRV_USI_PROT_ID_MNGP_PRIME_RESET          = 0x03,

    /* PRIME Manager: REBOOT */
    SRV_USI_PROT_ID_MNGP_PRIME_REBOOT         = 0x04,

    /* PRIME Manager: Firmware Upgrade */
    SRV_USI_PROT_ID_MNGP_PRIME_FU             = 0x05,

    /* PRIME Manager: GETQRY enhanced */
    SRV_USI_PROT_ID_MNGP_PRIME_GETQRY_EN      = 0x06,

    /* PRIME Manager: GETRSP enhanced */
    SRV_USI_PROT_ID_MNGP_PRIME_GETRSP_EN      = 0x07,

    /* PRIME Sniffer */
    SRV_USI_PROT_ID_SNIF_PRIME                = 0x13,

    /* PRIME PHY Serial */
    SRV_USI_PROT_ID_PHY_SERIAL_PRIME          = 0x1F,

    /* Physical Layer  */
    SRV_USI_PROT_ID_PHY                       = 0x22,

    /* G3 Sniffer  */
    SRV_USI_PROT_ID_SNIFF_G3                  = 0x23,

    /* G3 MAC layer  */
    SRV_USI_PROT_ID_MAC_G3                    = 0x24,

    /* G3 ADP layer  */
    SRV_USI_PROT_ID_ADP_G3                    = 0x25,

    /* G3 Coordinator  */
    SRV_USI_PROT_ID_COORD_G3                  = 0x26,

    /* MicroPLC Physical Layer  */
    SRV_USI_PROT_ID_PHY_MICROPLC              = 0x27,

    /* RF215 Physical Layer */
    SRV_USI_PROT_ID_PHY_RF215                 = 0x28,

    /* PRIME API  */
    SRV_USI_PROT_ID_PRIME_API                 = 0x30,

    /* Invalid protocol  */
    SRV_USI_PROT_ID_INVALID                   = 0xFF

} SRV_USI_PROTOCOL_ID;

/* MISRA C-2012 deviation block end */

// *****************************************************************************
/* USI Service Serial Protocol Identifiers
  Summary:
    List of possible values of USI status.

  Description:
    This type defines the possible return values for SRV_USI_Status function.

  Remarks:
    None.
*/

typedef enum
{
    SRV_USI_STATUS_UNINITIALIZED,

    SRV_USI_STATUS_NOT_CONFIGURED,

    SRV_USI_STATUS_CONFIGURED,

    SRV_USI_STATUS_ERROR

} SRV_USI_STATUS;

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // #ifndef SRV_USI_DEFINITIONS_H
