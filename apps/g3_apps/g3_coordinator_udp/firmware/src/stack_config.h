/*******************************************************************************
  Stack Config Header

  File Name:
    stack_config.h

  Summary:
    This file contains the application specific definitions for
    Resource management

  Description:
    These are application-specific resources which are used in the example
    application of the coordinator in addition to the underlaying stack.
*******************************************************************************/

// DOM-IGNORE-BEGIN
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
// DOM-IGNORE-END

/* Prevent double inclusion */
#ifndef STACK_CONFIG_H
#define STACK_CONFIG_H

/* === INCLUDES ============================================================ */
#include "phy.h"

/* === EXTERNALS =========================================================== */

/* === MACROS ============================================================== */
/* LARGE_BUFFER_SIZE

   Summary:
    This macro hold the Large buffer size value used in PHY library
   Description:
    The following macros hold the size of a large buffer.
    Additional octets for the length of the frame, the LQI
    and the ED value are required.
    Size of PHY_FrameInfo_t + max number of payload octets +
    1 octet LQI  + 1 octet ED value + 1 octet Length field.
   Remarks:
    None
 */

#ifndef LARGE_BUFFER_SIZE
#define LARGE_BUFFER_SIZE                   (((sizeof(PHY_FrameInfo_t) + \
	aMaxPHYPacketSize + \
	LENGTH_FIELD_LEN + LQI_LEN + ED_VAL_LEN) / 4U + 1U) * 4U)
#endif

// *****************************************************************************
/* SMALL_BUFFER_SIZE

   Summary:
    This macro hold the small buffer size value
   Description:
    The following macros hold the size of a small buffer.
    Additional octets for the length of the frame, the LQI
    and the ED value are required.
    Size of PHY_FrameInfo_t + max number of mac management frame len +
    1 octet LQI  + 1 octet ED value + 1 octet Length field.
   Remarks:
    None
 */

#ifndef SMALL_BUFFER_SIZE
#define SMALL_BUFFER_SIZE                   (((sizeof(PHY_FrameInfo_t) + \
	MAX_MGMT_FRAME_LENGTH +	\
	LENGTH_FIELD_LEN + LQI_LEN + ED_VAL_LEN) / 4U + 1U) * 4U)
#endif

/*
 * Configuration of Large and Small Buffers Used in Radio Layer
 */
#define NUMBER_OF_LARGE_PHY_BUFS          (3U)
#define NUMBER_OF_SMALL_PHY_BUFS          (3U)

/* === TYPES =============================================================== */


/* === PROTOTYPES ========================================================== */

#endif /* STACK_CONFIG_H */
