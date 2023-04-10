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
/*****************************************************************************
 Copyright (C) 2023 Microchip Technology Inc. and its subsidiaries.

Microchip Technology Inc. and its subsidiaries.

Subject to your compliance with these terms, you may use Microchip software 
and any derivatives exclusively with Microchip products. It is your 
responsibility to comply with third party license terms applicable to your 
use of third party software (including open source software) that may 
accompany Microchip software.

THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR 
PURPOSE.

IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE 
FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN 
ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, 
THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*****************************************************************************/

//DOM-IGNORE-END

#ifndef _DRV_G3ADP_MAC_LOCAL_H
#define _DRV_G3ADP_MAC_LOCAL_H

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


// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
/* TCPIP Stack Event Descriptor

  Summary:

  Description:

  Remarks:
    None
*/
typedef struct
{
    TCPIP_MAC_EVENT         _TcpEnabledEvents;          // group enabled notification events
    TCPIP_MAC_EVENT         _TcpPendingEvents;          // group notification events that are set, waiting to be re-acknowledged
    TCPIP_MAC_EventF        _TcpNotifyFnc;              // group notification handler
    const void*             _TcpNotifyParam;            // notification parameter
} DRV_G3ADP_MAC_EVENT_DCPT;   // event descriptor per group

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

// *****************************************************************************
/* G3 ADP MAC driver data.

  Summary: Data for G3 ADP MAC driver structure.

  Description: All the data related to G3 ADP MAC driver
*/
typedef struct {	
	
    SYS_STATUS                      sysStat;      // driver status
    
    TCPIP_MAC_EVENT                 enabledEvents; 
    TCPIP_MAC_EVENT                 pendingEvents; 
    
	TCPIP_MAC_RX_STATISTICS         rxStat;       // run time statistics
	TCPIP_MAC_TX_STATISTICS         txStat;       // run time statistics

	// general stuff
	const void*                     allocH ;      // allocation handle
	TCPIP_MAC_HEAP_CallocF          callocF;      // allocation functions
	TCPIP_MAC_HEAP_FreeF            freeF;
    
	// packet allocation functions
	TCPIP_MAC_PKT_AllocF            pktAllocF;
	TCPIP_MAC_PKT_FreeF             pktFreeF;
	TCPIP_MAC_PKT_AckF              pktAckF;
	
	// synchronization
	TCPIP_MAC_SynchReqF             synchF;
    // Event reporting. */
    TCPIP_MAC_EventF                eventF;
    // Parameter to be used when the event function is called
    const void*                     eventParam;  
    
    uint16_t                        controlFlags;
    
    ADP_DATA_NOTIFICATIONS          adpDataNotifications;    
    
    bool                            adpFreeBuffers;
    
    SRV_QUEUE                       adpTxQueue;
    
    SRV_QUEUE                       adpRxQueue;
    
    union
	{
		uint8_t        val;
		struct
		{
			uint8_t    _init               : 1;    // the corresponding MAC is initialized
			uint8_t    _open               : 1;    // the corresponding MAC is opened
			uint8_t    _linkPresent        : 1;    // link connection to the ADP Network properly detected : on/off
		};
	}_macFlags;	
    
} DRV_G3ADP_MAC_INSTANCE;


// *****************************************************************************
/* G3 ADP MAC driver structure.

  Summary: Driver Apis and Data for G3 ADP MAC driver.

  Description:

*/
typedef struct
{
	const TCPIP_MAC_OBJECT* pObj;		    // Pointer to G3 ADP MAC object
	DRV_G3ADP_MAC_INSTANCE  g3AdpMacData;   // G3 ADP MAC data
} DRV_G3ADP_MAC_DRIVER;

// *****************************************************************************
/* Structure for PIC32C GMAC Hash Calculation

  Summary:

  Description:
    Structure supports hash value calculation and setting in PIC32C GMAC register

  Remarks:
*/
typedef struct
{
    uint64_t hash_value;            // 64- bit Hash value to Set
    const TCPIP_MAC_ADDR* DestMACAddr;    // MAC address pointer for Hash value calculation
    uint8_t calculate_hash;        // Selects hash calculation or direct hash value
                                    // Clear to 0 for direct loading of hash value to register
                                    // Set to 1for Calculating the Hash from Mac address and load

}DRV_G3ADP_MAC_HASH; 

typedef union
  {
      uint8_t index;
      struct __attribute__((packed))
      {
          uint8_t b0:1;
          uint8_t b1:1;
          uint8_t b2:1;
          uint8_t b3:1;
          uint8_t b4:1;
          uint8_t b5:1;
          uint8_t b6:1;
          uint8_t b7:1;
      } bits;
  }DRV_G3ADP_MAC_HASH_INDEX;  

  typedef union
  {
      uint8_t addr_val;
      struct __attribute__((packed))
      {
          uint8_t b0:1;
          uint8_t b1:1;
          uint8_t b2:1;
          uint8_t b3:1;
          uint8_t b4:1;
          uint8_t b5:1;
          uint8_t b6:1;
          uint8_t b7:1;
      } bits;
  }DRV_G3ADP_MAC_MAC_ADDR;
#endif //#ifndef _DRV_G3ADP_MAC_LOCAL_H

/*******************************************************************************
 End of File
*/

