/*******************************************************************************
  G3 ADP MAC Driver Local Data Structures

  Company:
    Microchip Technology Inc.

  File Name:
    drv_mac_g3adp_local.h

  Summary:
    G3 ADP MAC driver local declarations and definitions.

  Description:
    This file contains the G3 ADP MAC driver's local declarations and definitions.
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

#ifndef DRV_G3ADP_MAC_LOCAL_H
#define DRV_G3ADP_MAC_LOCAL_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "system_config.h"
#include "system/system.h"
#include "system/int/sys_int.h"
#include "system/time/sys_time.h"
#include "tcpip/tcpip_mac_object.h"

#include "service/queue/srv_queue.h"

#include "stack/g3/adaptation/adp.h"
#include "stack/g3/adaptation/adp_api_types.h"
#include "stack/g3/adaptation/adp_shared_types.h"


#define DRV_MAC_G3ADP_PACKET_TX_QUEUE_LIMIT         5
#define DRV_MAC_G3ADP_PACKET_RX_QUEUE_LIMIT         2


// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* G3 ADP MAC Queue Element definition

  Summary:

  Description:

  Remarks:
    None
*/
typedef struct
{
    SRV_QUEUE_ELEMENT       queueElement;      // queue element
    TCPIP_MAC_PACKET *      pMacPacket;        // pointer to the TCPIP MAC packet
    bool                    inUse;             // Flag to indicate if the element is being used in a queue
} DRV_G3ADP_MAC_QUEUE_DATA;

/* MISRA C-2012 deviation block start */
/* MISRA C-2012 Rule 6.1 deviated 3 times.  Deviation record ID - H3_MISRAC_2012_R_6_1_DR_1 */

// *****************************************************************************
/* G3 ADP MAC driver data.

  Summary: Data for G3 ADP MAC driver structure.

  Description: All the data related to G3 ADP MAC driver
*/
typedef struct {

    SYS_STATUS                      sysStat;      // driver status

    TCPIP_MAC_EVENT                 pendingEvents;

    TCPIP_MAC_RX_STATISTICS         rxStat;       // run time statistics
    TCPIP_MAC_TX_STATISTICS         txStat;       // run time statistics

    // packet allocation functions
    TCPIP_MAC_PKT_AllocF            pktAllocF;
    TCPIP_MAC_PKT_FreeF             pktFreeF;

    // Event reporting. */
    TCPIP_MAC_EventF                eventF;
    // Parameter to be used when the event function is called
    const void*                     eventParam;

    SRV_QUEUE                       adpTxQueue;

    SRV_QUEUE                       adpRxQueue;

    union
    {
        uint8_t        val;
        struct
        {
            uint8_t    init               : 1;    // the corresponding MAC is initialized
            uint8_t    open               : 1;    // the corresponding MAC is opened
            uint8_t    linkPresent        : 1;    // link connection to the ADP Network properly detected : on/off
        };
    }macFlags;

} DRV_G3ADP_MAC_INSTANCE;

/* MISRA C-2012 deviation block end */

// *****************************************************************************
/* G3 ADP MAC driver structure.

  Summary: Driver Apis and Data for G3 ADP MAC driver.

  Description:

*/
typedef struct
{
    const TCPIP_MAC_OBJECT* pObj;           // Pointer to G3 ADP MAC object
    DRV_G3ADP_MAC_INSTANCE  g3AdpMacData;   // G3 ADP MAC data
} DRV_G3ADP_MAC_DRIVER;

#endif //#ifndef DRV_G3ADP_MAC_LOCAL_H

/*******************************************************************************
 End of File
*/

