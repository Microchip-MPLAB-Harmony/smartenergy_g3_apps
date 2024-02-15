/***********************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    drv_mac_g3adp.c

  Summary:
    G3 ADP MAC device driver source file.

  Description:
    G3 ADP MAC Device Driver Implementation.

    The G3 ADP MAC device driver provides a simple interface to manage
    the G3 ADP stack. This file defines the interface definitions
    and prototypes for the G3 ADP MAC driver.
  ***********************************************************************/

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

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include "drv_mac_g3adp_local.h"
#include "system/sys_time_h2_adapter.h"
#include "stack/g3/net/macg3adp/drv_mac_g3adp.h"
#include "stack/g3/adaptation/adp.h"
#include "stack/g3/adaptation/adp_api_types.h"
#include "stack/g3/adaptation/adp_shared_types.h"
#include "stack/g3/mac/mac_wrapper/mac_wrapper_defs.h"
#include "tcpip/tcpip.h"

/******************************************************************************
 * G3 ADP MAC object implementation
 ******************************************************************************/
/*static*/ const TCPIP_MAC_OBJECT DRV_G3ADP_MACObject =
{
    .macId                                  = (uint16_t)TCPIP_MODULE_MAC_G3ADP,
    .macType                                = (uint8_t)TCPIP_MAC_TYPE_G3ADP,
    .macName                                = "G3ADPMAC",
    .TCPIP_MAC_Initialize                   = DRV_G3ADP_MAC_Initialize,
#if (TCPIP_STACK_MAC_DOWN_OPERATION != 0)
    .TCPIP_MAC_Deinitialize                 = DRV_G3ADP_MAC_Deinitialize,
    .TCPIP_MAC_Reinitialize                 = NULL,
#else
    .TCPIP_MAC_Deinitialize                 = NULL,
    .TCPIP_MAC_Reinitialize                 = NULL,
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

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Data
// *****************************************************************************
// *****************************************************************************

static DRV_G3ADP_MAC_QUEUE_DATA g3adp_mac_txDataPool[DRV_MAC_G3ADP_PACKET_TX_QUEUE_LIMIT];
static DRV_G3ADP_MAC_QUEUE_DATA g3adp_mac_rxDataPool[DRV_MAC_G3ADP_PACKET_RX_QUEUE_LIMIT];

static DRV_G3ADP_MAC_DRIVER g3adp_mac_drv_dcpt =
    {
        &DRV_G3ADP_MACObject,
        {
            .sysStat = SYS_STATUS_UNINITIALIZED,
            .macFlags.val = 0,
        }
};

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Functions
// *****************************************************************************
// *****************************************************************************

static DRV_G3ADP_MAC_QUEUE_DATA * lDRV_G3ADP_MAC_GetFreeQueueData(DRV_G3ADP_MAC_QUEUE_DATA *dataPool,
        uint16_t dataLen)
{
    DRV_G3ADP_MAC_DRIVER * pMacDrv = &g3adp_mac_drv_dcpt;
    DRV_G3ADP_MAC_QUEUE_DATA *ptrDataPool = dataPool;
    uint8_t index;
    uint8_t poolSize = DRV_MAC_G3ADP_PACKET_TX_QUEUE_LIMIT;

    if (dataPool == g3adp_mac_rxDataPool)
    {
        poolSize = DRV_MAC_G3ADP_PACKET_RX_QUEUE_LIMIT;
    }

    for (index = 0U; index < poolSize; index++)
    {
        if (ptrDataPool->inUse == false)
        {
            if (dataLen > 0U)
            {
                if ((dataPool == g3adp_mac_rxDataPool) && (pMacDrv->g3AdpMacData.pktAllocF != NULL))
                {
                    // Dynamically allocate MAC Packet
                    ptrDataPool->pMacPacket = pMacDrv->g3AdpMacData.pktAllocF(sizeof(TCPIP_MAC_PACKET),
                            dataLen, TCPIP_MAC_PKT_FLAG_CAST_DISABLED);

                    if (ptrDataPool->pMacPacket == NULL)
                    {
                        return NULL;
                    }
                }
            }

            ptrDataPool->inUse = true;
            return ptrDataPool;
        }

        ptrDataPool++;
    }

    return NULL;
}

static DRV_G3ADP_MAC_QUEUE_DATA * lDRV_G3ADP_MAC_GetQueueDataFromMACPacket(DRV_G3ADP_MAC_QUEUE_DATA *dataPool,
        TCPIP_MAC_PACKET * pMacPacket)
{
    DRV_G3ADP_MAC_QUEUE_DATA *ptrDataPool = dataPool;
    uint8_t index;
    uint8_t poolSize = DRV_MAC_G3ADP_PACKET_TX_QUEUE_LIMIT;

    if (dataPool == g3adp_mac_rxDataPool)
    {
        poolSize = DRV_MAC_G3ADP_PACKET_RX_QUEUE_LIMIT;
    }

    for (index = 0U; index < poolSize; index++)
    {
        if ((ptrDataPool->inUse == true) && (ptrDataPool->pMacPacket == pMacPacket))
        {
            return ptrDataPool;
        }

        ptrDataPool++;
    }

    return NULL;
}

static void lDRV_G3ADP_MAC_PutFreeQueueData(DRV_G3ADP_MAC_QUEUE_DATA *dataPool,
        DRV_G3ADP_MAC_QUEUE_DATA *queuedData)
{
    DRV_G3ADP_MAC_DRIVER * pMacDrv = &g3adp_mac_drv_dcpt;
    DRV_G3ADP_MAC_QUEUE_DATA *ptrDataPool = dataPool;
    uint8_t index;
    uint8_t poolSize = DRV_MAC_G3ADP_PACKET_TX_QUEUE_LIMIT;

    if (dataPool == g3adp_mac_rxDataPool)
    {
        poolSize = DRV_MAC_G3ADP_PACKET_RX_QUEUE_LIMIT;
    }

    for (index = 0U; index < poolSize; index++)
    {
        if (ptrDataPool == queuedData)
        {
            if ((dataPool == g3adp_mac_rxDataPool) && (pMacDrv->g3AdpMacData.pktFreeF != NULL))
            {
                // Free memory
                pMacDrv->g3AdpMacData.pktFreeF(ptrDataPool->pMacPacket);
            }

            ptrDataPool->inUse = false;
            break;
        }

        ptrDataPool++;
    }
}

static void lDRV_G3ADP_MAC_RxMacFreePacket(TCPIP_MAC_PACKET * pMacPacket, const void * param)
{
    /* MISRA C-2012 deviation block start */
    /* MISRA C-2012 Rule 11.8 deviated once. Deviation record ID - H3_MISRAC_2012_R_11_8_DR_1 */
    DRV_G3ADP_MAC_DRIVER * pMacDrv = (DRV_G3ADP_MAC_DRIVER *) param;
    /* MISRA C-2012 deviation block end */

    if ((pMacPacket != NULL) &&  (pMacPacket->pDSeg != NULL)
            &&  ((pMacPacket->pDSeg->segFlags & (uint16_t)TCPIP_MAC_SEG_FLAG_ACK_REQUIRED) != 0U))
    {
        DRV_G3ADP_MAC_QUEUE_DATA * rxQueueData =lDRV_G3ADP_MAC_GetQueueDataFromMACPacket(g3adp_mac_rxDataPool, pMacPacket);

        if (rxQueueData != NULL)
        {
            lDRV_G3ADP_MAC_PutFreeQueueData(g3adp_mac_rxDataPool, rxQueueData);
            // Update RX statistics
            pMacDrv->g3AdpMacData.rxStat.nRxPendBuffers--;
        }
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: G3 ADP MAC Local Callbacks
// *****************************************************************************
// *****************************************************************************

static void lDRV_G3ADP_MAC_AdpDataCfmCallback(ADP_DATA_CFM_PARAMS* pDataCfm)
{
    DRV_G3ADP_MAC_DRIVER * pMacDrv = &g3adp_mac_drv_dcpt;
    DRV_G3ADP_MAC_QUEUE_DATA * txQueueData;

    if (pMacDrv->g3AdpMacData.macFlags.open != 0U)
    {
        TCPIP_MAC_PACKET * pMacPacket = (TCPIP_MAC_PACKET *)pDataCfm->nsduHandle;
        TCPIP_MAC_EVENT *event = &pMacDrv->g3AdpMacData.pendingEvents;
        uint16_t eventValue = (uint16_t)*event;
        ADP_RESULT result = (ADP_RESULT)pDataCfm->status;

        switch (result)
        {
            case G3_SUCCESS:
                pMacPacket->ackRes = (int8_t)TCPIP_MAC_PKT_ACK_TX_OK;
                pMacDrv->g3AdpMacData.txStat.nTxOkPackets++;
                eventValue |= (uint16_t)TCPIP_MAC_EV_TX_DONE;
                break;

            case G3_INVALID_REQUEST:
                pMacPacket->ackRes = (int8_t)TCPIP_MAC_PKT_ACK_NET_DOWN;
                pMacDrv->g3AdpMacData.txStat.nTxErrorPackets++;
                eventValue |= (uint16_t)TCPIP_MAC_EV_TX_BUSERR;
                break;

            case G3_INVALID_IPV6_FRAME:
                pMacPacket->ackRes = (int8_t)TCPIP_MAC_PKT_ACK_IP_REJECT_ERR;
                pMacDrv->g3AdpMacData.txStat.nTxErrorPackets++;
                eventValue |= (uint16_t)TCPIP_MAC_EV_TX_BUSERR;
                break;

            case G3_ROUTE_ERROR:
                pMacPacket->ackRes = (int8_t)TCPIP_MAC_PKT_ACK_LINK_DOWN;
                pMacDrv->g3AdpMacData.txStat.nTxErrorPackets++;
                eventValue |= (uint16_t)TCPIP_MAC_EV_TX_BUSERR;
                break;

            case G3_NO_BUFFERS:
                pMacPacket->ackRes = (int8_t)TCPIP_MAC_PKT_ACK_BUFFER_ERR;
                pMacDrv->g3AdpMacData.txStat.nTxErrorPackets++;
                eventValue |= (uint16_t)TCPIP_MAC_EV_TX_BUSERR;
                break;

            default:
                pMacPacket->ackRes = (int8_t)TCPIP_MAC_PKT_ACK_MAC_REJECT_ERR;
                pMacDrv->g3AdpMacData.txStat.nTxErrorPackets++;
                eventValue |= (uint16_t)TCPIP_MAC_EV_TX_ABORT;
                break;

        }

        *event = (TCPIP_MAC_EVENT)eventValue;
        pMacPacket->pktFlags &= ~((uint16_t)TCPIP_MAC_PKT_FLAG_QUEUED);

        txQueueData = lDRV_G3ADP_MAC_GetQueueDataFromMACPacket(g3adp_mac_txDataPool, pMacPacket);
        lDRV_G3ADP_MAC_PutFreeQueueData(g3adp_mac_txDataPool, txQueueData);
        // Update TX statistics
        pMacDrv->g3AdpMacData.txStat.nTxPendBuffers--;

        if (pMacPacket->ackFunc != NULL)
        {
            pMacPacket->ackFunc(pMacPacket, pMacPacket->ackParam);
        }

    }
}

static void lDRV_G3ADP_MAC_AdpDataIndCallback(ADP_DATA_IND_PARAMS* pDataInd)
{
    DRV_G3ADP_MAC_DRIVER * pMacDrv = &g3adp_mac_drv_dcpt;

    if (pMacDrv->g3AdpMacData.macFlags.open != 0U)
    {
        DRV_G3ADP_MAC_QUEUE_DATA * rxQueueData;
        TCPIP_MAC_PACKET * pMacPacket;
        TCPIP_MAC_DATA_SEGMENT * pDSeg;
        uint16_t pendingEvents;

        // Get a Free Queue Element from the RX Queue Data Pool
        rxQueueData = (DRV_G3ADP_MAC_QUEUE_DATA *)lDRV_G3ADP_MAC_GetFreeQueueData(g3adp_mac_rxDataPool,
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
        TCPIP_MAC_ETHERNET_HEADER* pMacHdr = (void*)pMacPacket->pMacLayer;
        pMacHdr->Type = TCPIP_Helper_htons(0x86DDu);

        // Copy data payload to Net Layer Payload
        (void) memcpy(pMacPacket->pNetLayer, pDataInd->pNsdu, pDataInd->nsduLength);
        pDSeg->segLen = pDataInd->nsduLength + (uint16_t)sizeof(TCPIP_MAC_ETHERNET_HEADER);
        pDSeg->segFlags |= (uint16_t)TCPIP_MAC_SEG_FLAG_ACK_REQUIRED; // allow rxMacPacketAck entry

        // Add timestamp
        pMacPacket->tStamp = SYS_TMR_TickCountGet();
        // just one single packet
        pMacPacket->next = NULL;
        // setup the packet acknowledgment for later use;
        pMacPacket->ackFunc = lDRV_G3ADP_MAC_RxMacFreePacket;
        pMacPacket->ackParam = pMacDrv;
        // Update Packet flags
        pMacPacket->pktFlags |= (uint16_t)TCPIP_MAC_PKT_FLAG_QUEUED;
        pDSeg->next = NULL;

        // Append ADP packets to the ADP RX queue
        SRV_QUEUE_Append(&pMacDrv->g3AdpMacData.adpRxQueue, &rxQueueData->queueElement);
        rxQueueData->inUse = true;

        // Update RX statistics
        pMacDrv->g3AdpMacData.rxStat.nRxSchedBuffers++;
        pMacDrv->g3AdpMacData.rxStat.nRxPendBuffers++;

        // Set RX triggered events: A receive packet is pending
        pendingEvents = (uint16_t)pMacDrv->g3AdpMacData.pendingEvents | (uint16_t)TCPIP_MAC_EV_RX_DONE;
        pMacDrv->g3AdpMacData.pendingEvents = (TCPIP_MAC_EVENT)pendingEvents;

        if (pMacDrv->g3AdpMacData.eventF != NULL)
        {
            pMacDrv->g3AdpMacData.eventF(TCPIP_MAC_EV_RX_DONE, pMacDrv->g3AdpMacData.eventParam);
        }
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: G3 ADP MAC interface
// *****************************************************************************
// *****************************************************************************

SYS_MODULE_OBJ DRV_G3ADP_MAC_Initialize(const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init)
{
    DRV_G3ADP_MAC_DRIVER * pMacDrv;
    const TCPIP_MAC_INIT * const tcpipMacInit = (const void * const)init;
    const TCPIP_MAC_MODULE_CTRL* macControl = tcpipMacInit->macControl;

    if (index != (SYS_MODULE_INDEX)TCPIP_MODULE_MAC_G3ADP)
    {
        return SYS_MODULE_OBJ_INVALID;      // single instance
    }

    pMacDrv = &g3adp_mac_drv_dcpt;

    if (pMacDrv->g3AdpMacData.macFlags.init != 0U)
    {   // already initialized
        return (SYS_MODULE_OBJ)pMacDrv;
    }

    if (pMacDrv->g3AdpMacData.macFlags.open != 0U)
    {
        return SYS_MODULE_OBJ_INVALID;     // client already connected
    }

    // Init G3 Adp Mac data
    (void) memset(&pMacDrv->g3AdpMacData, 0x0, sizeof(pMacDrv->g3AdpMacData));

    // use initialization data
    pMacDrv->g3AdpMacData.pktAllocF = macControl->pktAllocF;
    pMacDrv->g3AdpMacData.pktFreeF = macControl->pktFreeF;

    // use events data
    pMacDrv->g3AdpMacData.eventF = macControl->eventF;
    pMacDrv->g3AdpMacData.eventParam = macControl->eventParam;

    // Init G3 ADP MAC Tx/Rx queues
    SRV_QUEUE_Init(&pMacDrv->g3AdpMacData.adpTxQueue, DRV_MAC_G3ADP_PACKET_TX_QUEUE_LIMIT, SRV_QUEUE_TYPE_PRIORITY);
    SRV_QUEUE_Init(&pMacDrv->g3AdpMacData.adpRxQueue, DRV_MAC_G3ADP_PACKET_RX_QUEUE_LIMIT, SRV_QUEUE_TYPE_SINGLE);
    (void) memset(g3adp_mac_txDataPool, 0, sizeof(g3adp_mac_txDataPool));
    (void) memset(g3adp_mac_rxDataPool, 0, sizeof(g3adp_mac_rxDataPool));

    pMacDrv->g3AdpMacData.macFlags.init = 1U;
    pMacDrv->g3AdpMacData.macFlags.open = 0U;
    pMacDrv->g3AdpMacData.macFlags.linkPresent = 0U;
    pMacDrv->g3AdpMacData.sysStat = SYS_STATUS_BUSY;

    return (SYS_MODULE_OBJ)pMacDrv;

}

#if (TCPIP_STACK_MAC_DOWN_OPERATION != 0)
void DRV_G3ADP_MAC_Deinitialize(SYS_MODULE_OBJ object)
{
    // This is the function that deinitializes the MAC.
    // It is called by the stack as a result of one interface going down.
    DRV_G3ADP_MAC_DRIVER * pMacDrv = &g3adp_mac_drv_dcpt;

    if (object == (SYS_MODULE_OBJ)pMacDrv)
    {
        if(pMacDrv->g3AdpMacData.macFlags.init != 0U)
        {
            ADP_DATA_NOTIFICATIONS adpDataNot;

            // Clear ADP Data Notifications
            adpDataNot.dataConfirm = NULL;
            adpDataNot.dataIndication = NULL;
            ADP_SetDataNotifications(&adpDataNot);

            pMacDrv->g3AdpMacData.sysStat = SYS_STATUS_UNINITIALIZED;
            pMacDrv->g3AdpMacData.macFlags.val = 0U;
        }
    }
}
#endif  // (TCPIP_STACK_MAC_DOWN_OPERATION != 0)

SYS_STATUS DRV_G3ADP_MAC_Status (SYS_MODULE_OBJ object)
{
    DRV_G3ADP_MAC_DRIVER * pMacDrv = &g3adp_mac_drv_dcpt;

    if (object == (SYS_MODULE_OBJ)pMacDrv)
    {
        if(pMacDrv->g3AdpMacData.macFlags.init != 0U)
        {
            return pMacDrv->g3AdpMacData.sysStat;
        }
    }

    return SYS_STATUS_ERROR;
}

void DRV_G3ADP_MAC_Tasks(SYS_MODULE_OBJ object)
{
    DRV_G3ADP_MAC_DRIVER * pMacDrv = &g3adp_mac_drv_dcpt;

    if ((object != (SYS_MODULE_OBJ)pMacDrv) || (pMacDrv->g3AdpMacData.macFlags.init == 0U))
    {   // nothing to do
        return;
    }

    // Check ADP status to update link information
    if (pMacDrv->g3AdpMacData.sysStat == SYS_STATUS_BUSY)
    {
        if (ADP_Status() == ADP_STATUS_LBP_CONNECTED)
        {
            pMacDrv->g3AdpMacData.macFlags.linkPresent = 1U;
            pMacDrv->g3AdpMacData.sysStat = SYS_STATUS_READY;
        }
    }
}

size_t DRV_G3ADP_MAC_ConfigGet(DRV_HANDLE hMac, void* configBuff, size_t buffSize, size_t* pConfigSize)
{
    // not needed
    if (pConfigSize != NULL)
    {
        *pConfigSize = 0;
    }

    return 0;
}

DRV_HANDLE DRV_G3ADP_MAC_Open(const SYS_MODULE_INDEX index, const DRV_IO_INTENT intent)
{
    DRV_G3ADP_MAC_DRIVER * pMacDrv;
    DRV_HANDLE hMac = DRV_HANDLE_INVALID;

    if (index == (SYS_MODULE_INDEX)TCPIP_MODULE_MAC_G3ADP)
    {   // only one client for now
        pMacDrv = &g3adp_mac_drv_dcpt;
        if (pMacDrv->g3AdpMacData.macFlags.init == 1U)
        {
            if (pMacDrv->g3AdpMacData.macFlags.open == 0U)
            {
                pMacDrv->g3AdpMacData.macFlags.open = 1U;
                hMac = (DRV_HANDLE)pMacDrv;
            }
        }
    }

    return hMac;
}


void DRV_G3ADP_MAC_Close( DRV_HANDLE hMac )
{
    DRV_G3ADP_MAC_DRIVER * pMacDrv = &g3adp_mac_drv_dcpt;

    if (hMac == (DRV_HANDLE)pMacDrv)
    {
        if(pMacDrv->g3AdpMacData.macFlags.init == 1U)
        {
            pMacDrv->g3AdpMacData.macFlags.open = 0U;
        }
    }
}

TCPIP_MAC_RES DRV_G3ADP_MAC_PacketTx(DRV_HANDLE hMac, TCPIP_MAC_PACKET * ptrPacket)
{
    DRV_G3ADP_MAC_DRIVER * pMacDrv = &g3adp_mac_drv_dcpt;

    if (hMac != (DRV_HANDLE)pMacDrv)
    {
        return TCPIP_MAC_RES_OP_ERR;
    }

    if (pMacDrv->g3AdpMacData.macFlags.open == 0U)
    {
        return TCPIP_MAC_RES_INIT_FAIL;
    }

    // new packet for transmission
    while (ptrPacket != NULL)
    {
        DRV_G3ADP_MAC_QUEUE_DATA * txQueueData;

        // Get a Free Queue Element from the TX Queue Data Pool
        txQueueData = (DRV_G3ADP_MAC_QUEUE_DATA *)lDRV_G3ADP_MAC_GetFreeQueueData(g3adp_mac_txDataPool, 0U);
        if (txQueueData == NULL)
        {
            pMacDrv->g3AdpMacData.txStat.nTxQueueFull++;
            return TCPIP_MAC_RES_QUEUE_TX_FULL;
        }

        txQueueData->pMacPacket = ptrPacket;

        // Append ADP packets to the ADP TX queue
        SRV_QUEUE_Append_With_Priority(&pMacDrv->g3AdpMacData.adpTxQueue, 0xFFU - (uint32_t)ptrPacket->pktPriority,
                                       &txQueueData->queueElement);

        ptrPacket->pktFlags |= (uint16_t)TCPIP_MAC_PKT_FLAG_QUEUED;
        pMacDrv->g3AdpMacData.txStat.nTxPendBuffers++;

        if (pMacDrv->g3AdpMacData.eventF != NULL)
        {
            pMacDrv->g3AdpMacData.eventF(TCPIP_MAC_EV_TX_DONE, pMacDrv->g3AdpMacData.eventParam);
        }

        ptrPacket = ptrPacket->next;
    }

    return TCPIP_MAC_RES_OK;
}

TCPIP_MAC_PACKET* DRV_G3ADP_MAC_PacketRx (DRV_HANDLE hMac, TCPIP_MAC_RES* pRes, TCPIP_MAC_PACKET_RX_STAT* pPktStat)
{
    DRV_G3ADP_MAC_DRIVER * pMacDrv = &g3adp_mac_drv_dcpt;
    TCPIP_MAC_PACKET * pRxPkt = NULL;
    DRV_G3ADP_MAC_QUEUE_DATA * rxQueueData;
    TCPIP_MAC_RES mRes = TCPIP_MAC_RES_OK;

    if (hMac != (DRV_HANDLE)pMacDrv)
    {
        return NULL;
    }

    rxQueueData = (void *)SRV_QUEUE_Read_Or_Remove(&pMacDrv->g3AdpMacData.adpRxQueue,
            SRV_QUEUE_MODE_REMOVE, SRV_QUEUE_POSITION_HEAD);
    if (rxQueueData == NULL)
    {
        return NULL;
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

    if (pRes != NULL)
    {
        *pRes = mRes;
    }

    // Update RX statistics
    pMacDrv->g3AdpMacData.rxStat.nRxOkPackets++;

    return pRxPkt;
}

bool DRV_G3ADP_MAC_LinkCheck(DRV_HANDLE hMac)
{
    DRV_G3ADP_MAC_DRIVER * pMacDrv = &g3adp_mac_drv_dcpt;

    if (hMac != (DRV_HANDLE)pMacDrv)
    {
        return false;
    }

    // Get ADP status and check if LBP has been completed
    if (ADP_Status() == ADP_STATUS_LBP_CONNECTED)
    {
        pMacDrv->g3AdpMacData.macFlags.linkPresent = 1U;
        return true;
    }
    else
    {
        pMacDrv->g3AdpMacData.macFlags.linkPresent = 0U;
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
    DRV_G3ADP_MAC_DRIVER * pMacDrv = &g3adp_mac_drv_dcpt;

    if (hMac != (DRV_HANDLE)pMacDrv)
    {
        return TCPIP_MAC_RES_OP_ERR;
    }

    if (pMacDrv->g3AdpMacData.macFlags.open == 0U)
    {
        return TCPIP_MAC_RES_OP_ERR;
    }

    // G3 ADP TX Process
    DRV_G3ADP_MAC_QUEUE_DATA * txQueueData = (void *)SRV_QUEUE_Read_Or_Remove(&pMacDrv->g3AdpMacData.adpTxQueue,
        SRV_QUEUE_MODE_REMOVE, SRV_QUEUE_POSITION_HEAD);
    while (txQueueData != NULL)
    {
        // Send packets to G3 ADP
        TCPIP_MAC_PACKET * ptrPacket = txQueueData->pMacPacket;
        if (ptrPacket->pNetLayer != NULL)
        {
            IPV6_HEADER *ipv6Pkt = (void *)ptrPacket->pNetLayer;
            uint16_t length = (uint16_t)sizeof(IPV6_HEADER) + TCPIP_Helper_htons(ipv6Pkt->PayloadLength);
            ADP_DataRequest(length, (const uint8_t *)ipv6Pkt, (uintptr_t)ptrPacket, true, (uint8_t)(ptrPacket->pktPriority > 0U));
        }

        // Get next Queued Data
        txQueueData = (void *)SRV_QUEUE_Read_Or_Remove(&pMacDrv->g3AdpMacData.adpTxQueue,
            SRV_QUEUE_MODE_REMOVE, SRV_QUEUE_POSITION_HEAD);
    }

    return TCPIP_MAC_RES_OK;
}

TCPIP_MAC_RES DRV_G3ADP_MAC_StatisticsGet(DRV_HANDLE hMac, TCPIP_MAC_RX_STATISTICS* pRxStatistics, TCPIP_MAC_TX_STATISTICS* pTxStatistics)
{
    DRV_G3ADP_MAC_DRIVER * pMacDrv = &g3adp_mac_drv_dcpt;

    if (hMac != (DRV_HANDLE)pMacDrv)
    {
        return TCPIP_MAC_RES_OP_ERR;
    }

    if (pRxStatistics != NULL)
    {
        *pRxStatistics = pMacDrv->g3AdpMacData.rxStat;
    }

    if (pTxStatistics != NULL)
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
    DRV_G3ADP_MAC_DRIVER * pMacDrv = &g3adp_mac_drv_dcpt;

    if (hMac != (DRV_HANDLE)pMacDrv)
    {
        return TCPIP_MAC_RES_OP_ERR;
    }

    if (pMacDrv->g3AdpMacData.sysStat == SYS_STATUS_READY)
    {
        if (pMacParams != NULL)
        {
            ADP_MAC_GET_CFM_PARAMS getConfirm;
            ADP_DATA_NOTIFICATIONS adpDataNot;
            uint16_t processFlags;

            // Set ADP Data Notifications
            adpDataNot.dataConfirm = lDRV_G3ADP_MAC_AdpDataCfmCallback;
            adpDataNot.dataIndication = lDRV_G3ADP_MAC_AdpDataIndCallback;
            ADP_SetDataNotifications(&adpDataNot);

            // Get MAC address from ADP Extended Address
            ADP_MacGetRequestSync((uint32_t)MAC_WRP_PIB_MANUF_EXTENDED_ADDRESS, 0U, &getConfirm);
            (void) memcpy(&pMacParams->ifPhyAddress.v, &getConfirm.attributeValue[2],
                    sizeof(pMacParams->ifPhyAddress));

            processFlags = (uint16_t)TCPIP_MAC_PROCESS_FLAG_RX | (uint16_t)TCPIP_MAC_PROCESS_FLAG_TX;
            pMacParams->processFlags = (TCPIP_MAC_PROCESS_FLAGS)processFlags;
            pMacParams->macType = TCPIP_MAC_TYPE_G3ADP;
            pMacParams->linkMtu = TCPIP_MAC_LINK_MTU_G3ADP;
            pMacParams->checksumOffloadRx = TCPIP_MAC_CHECKSUM_NONE;
            pMacParams->checksumOffloadTx = TCPIP_MAC_CHECKSUM_NONE;
            pMacParams->macTxPrioNum = 2U;
            pMacParams->macRxPrioNum = 1U;
        }

        return TCPIP_MAC_RES_OK;
    }

    return TCPIP_MAC_RES_IS_BUSY;
}

bool DRV_G3ADP_MAC_EventMaskSet(DRV_HANDLE hMac, TCPIP_MAC_EVENT macEvMask, bool enable)
{
    DRV_G3ADP_MAC_DRIVER * pMacDrv = &g3adp_mac_drv_dcpt;

    if (hMac != (DRV_HANDLE)pMacDrv)
    {
        return false;
    }

    if (!enable)
    {
        uint16_t pendingEvents = (uint16_t)pMacDrv->g3AdpMacData.pendingEvents & ~((uint16_t)macEvMask);
        pMacDrv->g3AdpMacData.pendingEvents = (TCPIP_MAC_EVENT)pendingEvents;
    }

    return true;
}

bool DRV_G3ADP_MAC_EventAcknowledge(DRV_HANDLE hMac, TCPIP_MAC_EVENT tcpAckEv)
{
    DRV_G3ADP_MAC_DRIVER * pMacDrv = &g3adp_mac_drv_dcpt;

    if (hMac != (DRV_HANDLE)pMacDrv)
    {
        return false;
    }

    if (((uint16_t)pMacDrv->g3AdpMacData.pendingEvents & (uint16_t)tcpAckEv) != 0U)
    {
        uint16_t pendingEvents = (uint16_t)pMacDrv->g3AdpMacData.pendingEvents & ~((uint16_t)tcpAckEv);
        pMacDrv->g3AdpMacData.pendingEvents = (TCPIP_MAC_EVENT)pendingEvents;
        return true;
    }

    return false;
}

TCPIP_MAC_EVENT DRV_G3ADP_MAC_EventPendingGet(DRV_HANDLE hMac)
{
    DRV_G3ADP_MAC_DRIVER * pMacDrv = &g3adp_mac_drv_dcpt;

    if (hMac != (DRV_HANDLE)pMacDrv)
    {
        return TCPIP_MAC_EV_NONE;
    }

    return pMacDrv->g3AdpMacData.pendingEvents;
}
