/*******************************************************************************
  G3 ADP MAC Module (Microchip PIC32CXMT) for Microchip TCP/IP Stack
*******************************************************************************/

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

#include "drv_mac_g3adp_local.h"
#include "system/sys_time_h2_adapter.h"
#include "stack/g3/net/macg3adp/drv_mac_g3adp.h"
#include "stack/g3/adaptation/adp.h"
#include "stack/g3/adaptation/adp_api_types.h"
#include "stack/g3/adaptation/adp_shared_types.h"
#include "stack/g3/mac/mac_wrapper/mac_wrapper_defs.h"
#include "tcpip/tcpip.h"

/** D E F I N I T I O N S ****************************************************/
//PIC32CXMT internal G3 ADP MAC interface
#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_MAC_PIC32CXMT

/******************************************************************************
 * Prototypes
 ******************************************************************************/
static DRV_G3ADP_MAC_QUEUE_DATA _txDataPool[DRV_MAC_G3ADP_PACKET_TX_QUEUE_LIMIT];
static DRV_G3ADP_MAC_QUEUE_DATA _rxDataPool[DRV_MAC_G3ADP_PACKET_RX_QUEUE_LIMIT];

#if (TCPIP_STACK_MAC_DOWN_OPERATION != 0)

#endif  // (TCPIP_STACK_MAC_DOWN_OPERATION != 0)

/******************************************************************************
 * G3 ADP MAC object implementation
 ******************************************************************************/
/*static*/ const TCPIP_MAC_OBJECT DRV_G3ADP_MACObject =  
{
    .macId                                  = TCPIP_MODULE_MAC_PIC32CXMT,
    .macType                                = TCPIP_MAC_TYPE_G3ADP,    
    .macName                                = "G3ADPMAC",   
    .TCPIP_MAC_Initialize                   = DRV_G3ADP_MAC_Initialize,
#if (TCPIP_STACK_MAC_DOWN_OPERATION != 0)
    .TCPIP_MAC_Deinitialize                 = DRV_G3ADP_MAC_Deinitialize,
    .TCPIP_MAC_Reinitialize                 = DRV_G3ADP_MAC_Reinitialize, 
#else
    .TCPIP_MAC_Deinitialize                 = 0,
    .TCPIP_MAC_Reinitialize                 = 0,
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0) 
    .TCPIP_MAC_Status                       = DRV_G3ADP_MAC_Status,
    .TCPIP_MAC_Tasks                        = DRV_G3ADP_MAC_Tasks,
    .TCPIP_MAC_Open                         = DRV_G3ADP_MAC_Open,
    .TCPIP_MAC_Close                        = DRV_G3ADP_MAC_Close,
    .TCPIP_MAC_LinkCheck                    = DRV_G3ADP_MAC_LinkCheck,
    .TCPIP_MAC_RxFilterHashTableEntrySet    = DRV_G3ADP_MAC_RxFilterHashTableEntrySet,
    .TCPIP_MAC_PowerMode                    = DRV_G3ADP_MAC_PowerMode,
    .TCPIP_MAC_PacketTx                     = DRV_G3ADP_MAC_PacketTx,
    .TCPIP_MAC_PacketRx                     = DRV_G3ADP_MAC_PacketRx,
    .TCPIP_MAC_Process                      = DRV_G3ADP_MAC_Process,
    .TCPIP_MAC_StatisticsGet                = DRV_G3ADP_MAC_StatisticsGet,
    .TCPIP_MAC_ParametersGet                = DRV_G3ADP_MAC_ParametersGet,
    .TCPIP_MAC_RegisterStatisticsGet        = DRV_G3ADP_MAC_RegisterStatisticsGet,
    .TCPIP_MAC_ConfigGet                    = DRV_G3ADP_MAC_ConfigGet,
    .TCPIP_MAC_EventMaskSet                 = DRV_G3ADP_MAC_EventMaskSet,
    .TCPIP_MAC_EventAcknowledge             = DRV_G3ADP_MAC_EventAcknowledge,
    .TCPIP_MAC_EventPendingGet              = DRV_G3ADP_MAC_EventPendingGet,
};

static DRV_G3ADP_MAC_DRIVER _g3adp_mac_drv_dcpt = 
{
    &DRV_G3ADP_MACObject,
    {0}
};

/******************************************************************************
 * G3 ADP MAC local functions
 ******************************************************************************/
static DRV_G3ADP_MAC_QUEUE_DATA * _DRV_G3ADP_MAC_GetFreeQueueData(DRV_G3ADP_MAC_QUEUE_DATA *dataPool,
        uint16_t dataLen)
{
    DRV_G3ADP_MAC_DRIVER * pMacDrv = &_g3adp_mac_drv_dcpt;
    DRV_G3ADP_MAC_QUEUE_DATA *ptrDataPool = dataPool;
    uint8_t index;
    uint8_t poolSize = DRV_MAC_G3ADP_PACKET_TX_QUEUE_LIMIT;

    if (dataPool == _rxDataPool)
    {
        poolSize = DRV_MAC_G3ADP_PACKET_RX_QUEUE_LIMIT;
    }
    
    for (index = 0; index < poolSize; index++, ptrDataPool++)
    {
        if (ptrDataPool->inUse == false)
        {
            if (dataLen)
            {
                // Dynamically allocate MAC Packet
                ptrDataPool->pMacPacket = pMacDrv->g3AdpMacData.pktAllocF(sizeof(TCPIP_MAC_PACKET), 
                        dataLen, TCPIP_MAC_PKT_FLAG_CAST_DISABLED);

                if (ptrDataPool->pMacPacket == NULL)
                {
                    return NULL;
                }
            }
            
            ptrDataPool->inUse = true;
            return ptrDataPool;
        }
    }
    
    return NULL;
}

static DRV_G3ADP_MAC_QUEUE_DATA * _DRV_G3ADP_MAC_GetQueueDataFromMACPacket(DRV_G3ADP_MAC_QUEUE_DATA *dataPool,
        TCPIP_MAC_PACKET * pMacPacket)
{
    DRV_G3ADP_MAC_QUEUE_DATA *ptrDataPool = dataPool;
    uint8_t index;
    uint8_t poolSize = DRV_MAC_G3ADP_PACKET_TX_QUEUE_LIMIT;

    if (dataPool == _rxDataPool)
    {
        poolSize = DRV_MAC_G3ADP_PACKET_RX_QUEUE_LIMIT;
    }
    
    for (index = 0; index < poolSize; index++, ptrDataPool++)
    {
        if ((ptrDataPool->inUse == true) && (ptrDataPool->pMacPacket == pMacPacket))
        {
            return ptrDataPool;
        }
    }
    
    return NULL;
}

static bool _DRV_G3ADP_MAC_PutFreeQueueData(DRV_G3ADP_MAC_QUEUE_DATA *dataPool,
        DRV_G3ADP_MAC_QUEUE_DATA *queuedData, bool pktFreeF)
{
    DRV_G3ADP_MAC_DRIVER * pMacDrv = &_g3adp_mac_drv_dcpt;
    DRV_G3ADP_MAC_QUEUE_DATA *ptrDataPool = dataPool;
    uint8_t index;
    uint8_t poolSize = DRV_MAC_G3ADP_PACKET_TX_QUEUE_LIMIT;

    if (dataPool == _rxDataPool)
    {
        poolSize = DRV_MAC_G3ADP_PACKET_RX_QUEUE_LIMIT;
    }
    
    for (index = 0; index < poolSize; index++, ptrDataPool++)
    {
        if (ptrDataPool == queuedData)
        {
            if (pktFreeF && (pMacDrv->g3AdpMacData.pktFreeF))
            {
                // Free memory
                pMacDrv->g3AdpMacData.pktFreeF(ptrDataPool->pMacPacket);
            }
            ptrDataPool->inUse = false;
            return true;
        }
    }
    
    return false;
}

static void _DRV_G3ADP_MAC_RxMacFreePacket(TCPIP_MAC_PACKET * pMacPacket, const void * param)
{
    DRV_G3ADP_MAC_DRIVER * pMacDrv = (DRV_G3ADP_MAC_DRIVER *) param;
    
    if (pMacPacket &&  pMacPacket->pDSeg 
            &&  (pMacPacket->pDSeg->segFlags & TCPIP_MAC_SEG_FLAG_ACK_REQUIRED))
    {
        DRV_G3ADP_MAC_QUEUE_DATA * rxQueueData =_DRV_G3ADP_MAC_GetQueueDataFromMACPacket(_rxDataPool, pMacPacket);
        
        if (rxQueueData != NULL)
        {
            _DRV_G3ADP_MAC_PutFreeQueueData(_rxDataPool, rxQueueData, true);
            // Update RX statistics
            pMacDrv->g3AdpMacData.rxStat.nRxPendBuffers--;
        }
    }
}

/******************************************************************************
 * G3 ADP MAC local callbacks
 ******************************************************************************/
void _DRV_G3ADP_MAC_AdpDataCfmCallback(ADP_DATA_CFM_PARAMS* pDataCfm)
{
    DRV_G3ADP_MAC_DRIVER * pMacDrv = &_g3adp_mac_drv_dcpt;
    DRV_G3ADP_MAC_QUEUE_DATA * txQueueData;
    
    if (pMacDrv->g3AdpMacData._macFlags._open)
    {
        TCPIP_MAC_PACKET * pMacPacket = (TCPIP_MAC_PACKET *)pDataCfm->nsduHandle;
        TCPIP_MAC_EVENT *event = &pMacDrv->g3AdpMacData.pendingEvents;
        ADP_RESULT result = (ADP_RESULT)pDataCfm->status;
        
        switch (result)
        {
            case G3_SUCCESS:
                pMacPacket->ackRes = TCPIP_MAC_PKT_ACK_TX_OK;
                pMacDrv->g3AdpMacData.txStat.nTxOkPackets++;
                *event |= TCPIP_MAC_EV_TX_DONE;
                break;
                
            case G3_INVALID_REQUEST:
                pMacPacket->ackRes = TCPIP_MAC_PKT_ACK_NET_DOWN;
                pMacDrv->g3AdpMacData.txStat.nTxErrorPackets++;
                *event |= TCPIP_MAC_EV_TX_BUSERR;
                break;
                
            case G3_INVALID_IPV6_FRAME:
                pMacPacket->ackRes = TCPIP_MAC_PKT_ACK_IP_REJECT_ERR;
                pMacDrv->g3AdpMacData.txStat.nTxErrorPackets++;
                *event |= TCPIP_MAC_EV_TX_BUSERR;
                break;
                
            case G3_ROUTE_ERROR:
                pMacPacket->ackRes = TCPIP_MAC_PKT_ACK_LINK_DOWN;
                pMacDrv->g3AdpMacData.txStat.nTxErrorPackets++;
                *event |= TCPIP_MAC_EV_TX_BUSERR;
                break;
                
            case G3_NO_BUFFERS:
                pMacPacket->ackRes = TCPIP_MAC_PKT_ACK_BUFFER_ERR;
                pMacDrv->g3AdpMacData.txStat.nTxErrorPackets++;
                *event |= TCPIP_MAC_EV_TX_BUSERR;
                break;
                
            default:
                pMacPacket->ackRes = TCPIP_MAC_PKT_ACK_MAC_REJECT_ERR;
                pMacDrv->g3AdpMacData.txStat.nTxErrorPackets++;
                *event |= TCPIP_MAC_EV_TX_ABORT;
                break;
            
        }
        
        pMacPacket->pktFlags &= ~TCPIP_MAC_PKT_FLAG_QUEUED;
        
        txQueueData = _DRV_G3ADP_MAC_GetQueueDataFromMACPacket(_txDataPool, pMacPacket);
        _DRV_G3ADP_MAC_PutFreeQueueData(_txDataPool, txQueueData, 0);
        // Update TX statistics
        pMacDrv->g3AdpMacData.txStat.nTxPendBuffers--;
        
        if (pMacPacket->ackFunc)
        {
            pMacPacket->ackFunc(pMacPacket, pMacPacket->ackParam);
        }
        
    }
}

void _DRV_G3ADP_MAC_AdpDataIndCallback(ADP_DATA_IND_PARAMS* pDataInd)
{
    DRV_G3ADP_MAC_DRIVER * pMacDrv = &_g3adp_mac_drv_dcpt;
    
    if (pMacDrv->g3AdpMacData._macFlags._open)
    {
        DRV_G3ADP_MAC_QUEUE_DATA * rxQueueData;
        TCPIP_MAC_PACKET * pMacPacket;
        TCPIP_MAC_DATA_SEGMENT * pDSeg;
        
        // Get a Free Queue Element from the RX Queue Data Pool
        rxQueueData = (DRV_G3ADP_MAC_QUEUE_DATA *)_DRV_G3ADP_MAC_GetFreeQueueData(_rxDataPool,
                pDataInd->nsduLength);
        if (rxQueueData == NULL)
        {
            pMacDrv->g3AdpMacData.rxStat.nRxBuffNotAvailable++;
            return;
        }
        
        pMacPacket = rxQueueData->pMacPacket;
        pDSeg = pMacPacket->pDSeg;
        
        // Set pMacLayer and pNetLayer
        pMacPacket->pMacLayer = pDSeg->segLoad;
        pMacPacket->pNetLayer = pMacPacket->pMacLayer + sizeof(TCPIP_MAC_ETHERNET_HEADER);
        
        // Set Ethernet Header Type - TCPIP_ETHER_TYPE_IPV6
        TCPIP_MAC_ETHERNET_HEADER* pMacHdr = (TCPIP_MAC_ETHERNET_HEADER*)pMacPacket->pMacLayer;
        pMacHdr->Type = TCPIP_Helper_htons(0x86DDu);
        
        // Copy data payload to Net Layer Payload
        memcpy(pMacPacket->pNetLayer, pDataInd->pNsdu, pDataInd->nsduLength);
        pDSeg->segLen = pDataInd->nsduLength + sizeof(TCPIP_MAC_ETHERNET_HEADER);
        pDSeg->segFlags |= TCPIP_MAC_SEG_FLAG_ACK_REQUIRED; // allow rxMacPacketAck entry
        
        // Add timestamp
        pMacPacket->tStamp = SYS_TMR_TickCountGet();
        // just one single packet
        pMacPacket->next = NULL;
        // setup the packet acknowledgment for later use;
        pMacPacket->ackFunc = _DRV_G3ADP_MAC_RxMacFreePacket;
        pMacPacket->ackParam = pMacDrv;
        // Update Packet flags
        pMacPacket->pktFlags |= TCPIP_MAC_PKT_FLAG_QUEUED;
        pDSeg->next = NULL;
        
        // Append ADP packets to the ADP RX queue
        SRV_QUEUE_Append(&pMacDrv->g3AdpMacData.adpRxQueue, (SRV_QUEUE_ELEMENT *)rxQueueData);
        rxQueueData->inUse = true;
        
        // Update RX statistics
        pMacDrv->g3AdpMacData.rxStat.nRxSchedBuffers++;
        pMacDrv->g3AdpMacData.rxStat.nRxPendBuffers++;
        
        // Set RX triggered events: A receive packet is pending
        pMacDrv->g3AdpMacData.pendingEvents |= TCPIP_MAC_EV_RX_DONE;
        
        if (pMacDrv->g3AdpMacData.eventF)
        {
            pMacDrv->g3AdpMacData.eventF(TCPIP_MAC_EV_RX_DONE, pMacDrv->g3AdpMacData.eventParam);
        }
    }    
}

/******************************************************************************
 * G3 ADP MAC interface
 ******************************************************************************/
SYS_MODULE_OBJ DRV_G3ADP_MAC_Initialize(const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init)
{
    DRV_G3ADP_MAC_DRIVER * pMacDrv;
    const TCPIP_MAC_MODULE_CTRL* macControl = ((TCPIP_MAC_INIT*)init)->macControl;
    
	if (index != TCPIP_MODULE_MAC_PIC32CXMT)
	{   
		return SYS_MODULE_OBJ_INVALID;      // single instance
	}
    
    pMacDrv = &_g3adp_mac_drv_dcpt;
    
    if (pMacDrv->g3AdpMacData._macFlags._init != 0)
	{   // already initialized	
		return (SYS_MODULE_OBJ)pMacDrv;
	}
	    	
	if (pMacDrv->g3AdpMacData._macFlags._open != 0)
	{
		return SYS_MODULE_OBJ_INVALID;     // client already connected
	}
    
    // Init G3 Adp Mac data
    memset(&pMacDrv->g3AdpMacData, 0x0, sizeof(pMacDrv->g3AdpMacData)); 
    
    // use initialization data
    pMacDrv->g3AdpMacData.allocH = macControl->memH;
    pMacDrv->g3AdpMacData.callocF = macControl->callocF;
    pMacDrv->g3AdpMacData.freeF = macControl->freeF;
    pMacDrv->g3AdpMacData.pktAllocF = macControl->pktAllocF;
    pMacDrv->g3AdpMacData.pktFreeF = macControl->pktFreeF;
    pMacDrv->g3AdpMacData.pktAckF = macControl->pktAckF;
    
    // use events data
    pMacDrv->g3AdpMacData.eventF = macControl->eventF;
    pMacDrv->g3AdpMacData.eventParam = macControl->eventParam;
    
    pMacDrv->g3AdpMacData.controlFlags = macControl->controlFlags;
    
    // Init G3 ADP MAC Tx/Rx queues
    SRV_QUEUE_Init(&pMacDrv->g3AdpMacData.adpTxQueue, DRV_MAC_G3ADP_PACKET_TX_QUEUE_LIMIT, SRV_QUEUE_TYPE_PRIORITY);
    SRV_QUEUE_Init(&pMacDrv->g3AdpMacData.adpRxQueue, DRV_MAC_G3ADP_PACKET_RX_QUEUE_LIMIT, SRV_QUEUE_TYPE_SINGLE);
    memset(_txDataPool, 0, sizeof(_txDataPool));
    memset(_rxDataPool, 0, sizeof(_rxDataPool));

    pMacDrv->g3AdpMacData._macFlags._init = 1;
    pMacDrv->g3AdpMacData._macFlags._open = 0;
    pMacDrv->g3AdpMacData._macFlags._linkPresent = 0;
    pMacDrv->g3AdpMacData.sysStat = SYS_STATUS_BUSY;
    
    return (SYS_MODULE_OBJ)pMacDrv;

}

#if (TCPIP_STACK_MAC_DOWN_OPERATION != 0)
void DRV_G3ADP_MAC_Deinitialize(SYS_MODULE_OBJ object)
{
    // This is the function that deinitializes the MAC. 
    // It is called by the stack as a result of one interface going down.
    DRV_G3ADP_MAC_DRIVER * pMacDrv = &_g3adp_mac_drv_dcpt;
    
    if (object == (SYS_MODULE_OBJ)pMacDrv)
    {
        if(pMacDrv->g3AdpMacData._macFlags._init != 0)
        {
            ADP_DATA_NOTIFICATIONS adpDataNot;
                
            // Clear ADP Data Notifications
            adpDataNot.dataConfirm = NULL;
            adpDataNot.dataIndication = NULL;
            ADP_SetDataNotifications(&adpDataNot);

            pMacDrv->g3AdpMacData.sysStat = SYS_STATUS_UNINITIALIZED;
            pMacDrv->g3AdpMacData._macFlags.val = 0;
        }
    }
}

void DRV_G3ADP_MAC_Reinitialize(SYS_MODULE_OBJ object, const SYS_MODULE_INIT * const init)
{
    // not supported
}
#endif  // (TCPIP_STACK_MAC_DOWN_OPERATION != 0)

SYS_STATUS DRV_G3ADP_MAC_Status (SYS_MODULE_OBJ object)
{
    DRV_G3ADP_MAC_DRIVER * pMacDrv = &_g3adp_mac_drv_dcpt;
    
    if (object == (SYS_MODULE_OBJ)pMacDrv)
    {
        if(pMacDrv->g3AdpMacData._macFlags._init != 0)
        {
            return pMacDrv->g3AdpMacData.sysStat;
        }
    }
    
	return SYS_STATUS_ERROR;
}

void DRV_G3ADP_MAC_Tasks(SYS_MODULE_OBJ object)
{
    DRV_G3ADP_MAC_DRIVER * pMacDrv = &_g3adp_mac_drv_dcpt;
    
    if ((object != (SYS_MODULE_OBJ)pMacDrv) || (pMacDrv->g3AdpMacData._macFlags._init == 0))
	{   // nothing to do
		return;
	}
        
    // Check ADP status to update link information
    if (pMacDrv->g3AdpMacData.sysStat == SYS_STATUS_BUSY)
    {
        if (ADP_Status() == ADP_STATUS_LBP_CONNECTED)
        {
            pMacDrv->g3AdpMacData._macFlags._linkPresent = 1;
            pMacDrv->g3AdpMacData.sysStat = SYS_STATUS_READY;        
        }
    }
}

size_t DRV_G3ADP_MAC_ConfigGet(DRV_HANDLE hMac, void* configBuff, size_t buffSize, size_t* pConfigSize) 
{	
    // not needed
    if (pConfigSize)
    {
        *pConfigSize = 0;
    }
    
    return 0;
}

DRV_HANDLE DRV_G3ADP_MAC_Open(const SYS_MODULE_INDEX index, const DRV_IO_INTENT intent) 
{
    DRV_G3ADP_MAC_DRIVER * pMacDrv;
    DRV_HANDLE hMac = DRV_HANDLE_INVALID;
    
	if (index == TCPIP_MODULE_MAC_PIC32CXMT)
	{   // only one client for now
        pMacDrv = &_g3adp_mac_drv_dcpt;
        if (pMacDrv->g3AdpMacData._macFlags._init == 1)
        {
            if (pMacDrv->g3AdpMacData._macFlags._open == 0)
            {
                pMacDrv->g3AdpMacData._macFlags._open = 1;
                hMac = (DRV_HANDLE)pMacDrv;
            }
        }
    }
    
    return hMac;
}


void DRV_G3ADP_MAC_Close( DRV_HANDLE hMac ) 
{
    DRV_G3ADP_MAC_DRIVER * pMacDrv = &_g3adp_mac_drv_dcpt;
    
    if (hMac == (DRV_HANDLE)pMacDrv)
    {
        if(pMacDrv->g3AdpMacData._macFlags._init == 1)	
        {
            pMacDrv->g3AdpMacData._macFlags._open = 0;
        }
    }
}

TCPIP_MAC_RES DRV_G3ADP_MAC_PacketTx(DRV_HANDLE hMac, TCPIP_MAC_PACKET * ptrPacket)  
{
    DRV_G3ADP_MAC_DRIVER * pMacDrv = &_g3adp_mac_drv_dcpt;
    
    if (hMac != (DRV_HANDLE)pMacDrv)
    {
        return TCPIP_MAC_RES_OP_ERR;
    }
    
    if (pMacDrv->g3AdpMacData._macFlags._open == 0)
    {
        return TCPIP_MAC_RES_INIT_FAIL;
    }
    
    // new packet for transmission
	while (ptrPacket)
	{
        DRV_G3ADP_MAC_QUEUE_DATA * txQueueData;

        // Get a Free Queue Element from the TX Queue Data Pool
        txQueueData = (DRV_G3ADP_MAC_QUEUE_DATA *)_DRV_G3ADP_MAC_GetFreeQueueData(_txDataPool, 0);
        if (txQueueData == NULL)
        {
            pMacDrv->g3AdpMacData.txStat.nTxQueueFull++;
            return TCPIP_MAC_RES_QUEUE_TX_FULL;
        }

        txQueueData->pMacPacket = ptrPacket;

        // Append ADP packets to the ADP TX queue
        SRV_QUEUE_Append_With_Priority(&pMacDrv->g3AdpMacData.adpTxQueue, (ptrPacket->pktPriority > 0), 
                (SRV_QUEUE_ELEMENT *)txQueueData);

        ptrPacket->pktFlags |= TCPIP_MAC_PKT_FLAG_QUEUED;
        pMacDrv->g3AdpMacData.txStat.nTxPendBuffers++;

        if (pMacDrv->g3AdpMacData.eventF)
        {
            pMacDrv->g3AdpMacData.eventF(TCPIP_MAC_EV_TX_DONE, pMacDrv->g3AdpMacData.eventParam);
        }

        ptrPacket = ptrPacket->next;
    }            
    
	return TCPIP_MAC_RES_OK;
}

TCPIP_MAC_PACKET* DRV_G3ADP_MAC_PacketRx (DRV_HANDLE hMac, TCPIP_MAC_RES* pRes, TCPIP_MAC_PACKET_RX_STAT* pPktStat)  
{
    DRV_G3ADP_MAC_DRIVER * pMacDrv = &_g3adp_mac_drv_dcpt;
	TCPIP_MAC_PACKET * pRxPkt = NULL;
    DRV_G3ADP_MAC_QUEUE_DATA * rxQueueData;
    TCPIP_MAC_RES mRes = TCPIP_MAC_RES_OK;
    
    if (hMac != (DRV_HANDLE)pMacDrv)
    {
        return 0;
    }
    
    rxQueueData = (DRV_G3ADP_MAC_QUEUE_DATA *)SRV_QUEUE_Read_Or_Remove(&pMacDrv->g3AdpMacData.adpRxQueue, 
            SRV_QUEUE_MODE_REMOVE, SRV_QUEUE_POSITION_HEAD);
    if (rxQueueData == NULL)
    {
        return 0;
    }
    else
    {
        // Update RX statistics
        pMacDrv->g3AdpMacData.rxStat.nRxSchedBuffers--;
        
        pRxPkt = rxQueueData->pMacPacket;
    
        if (pRxPkt == NULL)
        {
            mRes = TCPIP_MAC_RES_INTERNAL_ERR;
            pMacDrv->g3AdpMacData.rxStat.nRxErrorPackets++;
        }
    }
    
    if (pRes)
    {
		*pRes = mRes;
	}
    
    // Update RX statistics
    pMacDrv->g3AdpMacData.rxStat.nRxOkPackets++;
    
    return pRxPkt;
}

bool DRV_G3ADP_MAC_LinkCheck(DRV_HANDLE hMac) 
{
    DRV_G3ADP_MAC_DRIVER * pMacDrv = &_g3adp_mac_drv_dcpt;
    
    if (hMac != (DRV_HANDLE)pMacDrv)
    {
        return false;
    }
    
    // Get ADP status and check if LBP has been completed
    if (ADP_Status() == ADP_STATUS_LBP_CONNECTED)
    {
        pMacDrv->g3AdpMacData._macFlags._linkPresent = 1;
        return true;
    }
    else
    {
        pMacDrv->g3AdpMacData._macFlags._linkPresent = 0;
        return false;
    }
}

TCPIP_MAC_RES DRV_G3ADP_MAC_RxFilterHashTableEntrySet(DRV_HANDLE hMac, const TCPIP_MAC_ADDR* DestMACAddr) 
{ 
    // not supported
    return TCPIP_MAC_RES_OK;
}

bool DRV_G3ADP_MAC_PowerMode(DRV_HANDLE hMac, TCPIP_MAC_POWER_MODE pwrMode) 
{   
    // not supported
    return true;
}

TCPIP_MAC_RES DRV_G3ADP_MAC_Process(DRV_HANDLE hMac)  
{
    DRV_G3ADP_MAC_DRIVER * pMacDrv = &_g3adp_mac_drv_dcpt;
    
    if (hMac != (DRV_HANDLE)pMacDrv)
    {
        return TCPIP_MAC_RES_OP_ERR;
    }
    
    if (pMacDrv->g3AdpMacData._macFlags._open == 0)
    {
        return TCPIP_MAC_RES_OP_ERR;
    }
    
    // G3 ADP TX Process
    DRV_G3ADP_MAC_QUEUE_DATA * txQueueData = (DRV_G3ADP_MAC_QUEUE_DATA *)SRV_QUEUE_Read_Or_Remove(&pMacDrv->g3AdpMacData.adpTxQueue,
        SRV_QUEUE_MODE_REMOVE, SRV_QUEUE_POSITION_HEAD);
    while (txQueueData != NULL)
    {
        // Send packets to G3 ADP
        TCPIP_MAC_PACKET * ptrPacket = txQueueData->pMacPacket;
        if (ptrPacket->pNetLayer)
        {
            IPV6_HEADER *ipv6Pkt = (IPV6_HEADER *)ptrPacket->pNetLayer;
            uint16_t length = sizeof(IPV6_HEADER) + TCPIP_Helper_htons(ipv6Pkt->PayloadLength);
            ADP_DataRequest(length, (const uint8_t *)ipv6Pkt, (uint32_t)ptrPacket, true, (ptrPacket->pktPriority > 0));
        }

        // Get next Queued Data
        txQueueData = (DRV_G3ADP_MAC_QUEUE_DATA *)SRV_QUEUE_Read_Or_Remove(&pMacDrv->g3AdpMacData.adpTxQueue,
            SRV_QUEUE_MODE_REMOVE, SRV_QUEUE_POSITION_HEAD);
    }
    
	return TCPIP_MAC_RES_OK;
}

TCPIP_MAC_RES DRV_G3ADP_MAC_StatisticsGet(DRV_HANDLE hMac, TCPIP_MAC_RX_STATISTICS* pRxStatistics, TCPIP_MAC_TX_STATISTICS* pTxStatistics) 
{
	DRV_G3ADP_MAC_DRIVER * pMacDrv = &_g3adp_mac_drv_dcpt;
    
    if (hMac != (DRV_HANDLE)pMacDrv)
    {
        return TCPIP_MAC_RES_OP_ERR;
    }
    
	if (pRxStatistics)
	{
		*pRxStatistics = pMacDrv->g3AdpMacData.rxStat;
	}
    
	if (pTxStatistics)
	{
		*pTxStatistics = pMacDrv->g3AdpMacData.txStat;
	}

	return TCPIP_MAC_RES_OK;

}

TCPIP_MAC_RES DRV_G3ADP_MAC_RegisterStatisticsGet(DRV_HANDLE hMac, TCPIP_MAC_STATISTICS_REG_ENTRY* pRegEntries, int nEntries, int* pHwEntries)  
{
    // not supported
    return TCPIP_MAC_RES_OP_ERR;
}

TCPIP_MAC_RES DRV_G3ADP_MAC_ParametersGet(DRV_HANDLE hMac, TCPIP_MAC_PARAMETERS* pMacParams)
{
	DRV_G3ADP_MAC_DRIVER * pMacDrv = &_g3adp_mac_drv_dcpt;
    
    if (hMac != (DRV_HANDLE)pMacDrv)
    {
        return TCPIP_MAC_RES_OP_ERR;
    }

	if (pMacDrv->g3AdpMacData.sysStat == SYS_STATUS_READY)
	{
		if (pMacParams)
		{		
            ADP_MAC_GET_CFM_PARAMS getConfirm;   
            ADP_DATA_NOTIFICATIONS adpDataNot;

            // Set ADP Data Notifications
            adpDataNot.dataConfirm = _DRV_G3ADP_MAC_AdpDataCfmCallback;
            adpDataNot.dataIndication = _DRV_G3ADP_MAC_AdpDataIndCallback;
            ADP_SetDataNotifications(&adpDataNot);
                
            // Get MAC address from ADP Extended Address
            ADP_MacGetRequestSync(MAC_WRP_PIB_MANUF_EXTENDED_ADDRESS, 0, &getConfirm);
            memcpy(&pMacParams->ifPhyAddress, &getConfirm.attributeValue[2], 
                    sizeof(pMacParams->ifPhyAddress));
            
			pMacParams->processFlags = (TCPIP_MAC_PROCESS_FLAG_RX | TCPIP_MAC_PROCESS_FLAG_TX);
			pMacParams->macType = TCPIP_MAC_TYPE_G3ADP;
			pMacParams->linkMtu = TCPIP_MAC_LINK_MTU_G3ADP;
            pMacParams->checksumOffloadRx = TCPIP_MAC_CHECKSUM_NONE;
            pMacParams->checksumOffloadTx = TCPIP_MAC_CHECKSUM_NONE;
            pMacParams->macTxPrioNum = 2;
            pMacParams->macRxPrioNum = 1;
		}

		return TCPIP_MAC_RES_OK;
	}

	return TCPIP_MAC_RES_IS_BUSY;
}

bool DRV_G3ADP_MAC_EventMaskSet(DRV_HANDLE hMac, TCPIP_MAC_EVENT macEvMask, bool enable) 
{
	DRV_G3ADP_MAC_DRIVER * pMacDrv = &_g3adp_mac_drv_dcpt;
    
    if (hMac != (DRV_HANDLE)pMacDrv)
    {
        return false;
    }

	if (enable)
	{
        pMacDrv->g3AdpMacData.enabledEvents |= macEvMask;
    }
    else
    {
        pMacDrv->g3AdpMacData.enabledEvents &= ~macEvMask;
        pMacDrv->g3AdpMacData.pendingEvents &= ~macEvMask;
    }
        
	return true;
}

bool DRV_G3ADP_MAC_EventAcknowledge(DRV_HANDLE hMac, TCPIP_MAC_EVENT tcpAckEv) 
{
	DRV_G3ADP_MAC_DRIVER * pMacDrv = &_g3adp_mac_drv_dcpt;
    
    if (hMac != (DRV_HANDLE)pMacDrv)
    {
        return false;
    }

    if (pMacDrv->g3AdpMacData.pendingEvents & tcpAckEv)
    {
        pMacDrv->g3AdpMacData.pendingEvents &= ~tcpAckEv;
        return true;
    }

    return false;
}

TCPIP_MAC_EVENT DRV_G3ADP_MAC_EventPendingGet(DRV_HANDLE hMac) 
{
	DRV_G3ADP_MAC_DRIVER * pMacDrv = &_g3adp_mac_drv_dcpt;
    
    if (hMac != (DRV_HANDLE)pMacDrv)
    {
        return TCPIP_MAC_EV_NONE;
    }

    return pMacDrv->g3AdpMacData.pendingEvents;
}
