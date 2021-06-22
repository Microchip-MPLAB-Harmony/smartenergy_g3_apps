/******************************************************************************
  DRV_PLC Bootloader Layer

  Company:
    Microchip Technology Inc.

  File Name:
    drv_plc_boot.c

  Summary:
    PLC Driver Bootloader Layer

  Description:
    This file contains the source code for the implementation of the bootloader
    of PLC transceiver.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
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

// *****************************************************************************
// *****************************************************************************
// Section: Include Files
// *****************************************************************************
// *****************************************************************************
#include <string.h>
#include "driver/plc/common/drv_plc_hal.h"
#include "driver/plc/common/drv_plc_boot.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data
// *****************************************************************************
// *****************************************************************************

/* This is the pointer to refer Hardware Abstraction Layer (HAL) object. */
static DRV_PLC_HAL_INTERFACE *sDrvPlcHalObj;

/* This is the driver instance object array. */
static DRV_PLC_BOOT_INFO sDrvPlcBootInfo = {0};

/* This is the maximum size of the fragments to handle the upload task of binary
 file to PLC transceiver */
#define MAX_FRAG_SIZE      512

static DRV_PLC_BOOT_DATA_CALLBACK sDrvPlcBootCb = NULL;
static uintptr_t sDrvPlcBootContext; 

// *****************************************************************************
// *****************************************************************************
// Section: File scope functions
// *****************************************************************************
// *****************************************************************************
static void _DRV_PLC_BOOT_Rev(uint8_t *pDataDst, uint8_t len)
{
    uint8_t pTemp[16];

	for (uint8_t idx = 0; idx < len; idx++) {
		pTemp[idx] = pDataDst[15 - idx];
	}

	memcpy(pDataDst, pTemp, len);
}

static uint32_t _DRV_PLC_BOOT_CheckStatus(void)
{
    uint32_t regValue;

    /* Send Start Decryption */
	regValue = 0;
    sDrvPlcHalObj->sendBootCmd(DRV_PLC_BOOT_READ_BOOT_STATUS, 0, 4, 
            (uint8_t *)&regValue, (uint8_t *)&regValue);
    
    return regValue;
}

static void _DRV_PLC_BOOT_GetSecureInfo(uint8_t *pData)
{
    /* Get Number of packets */
    sDrvPlcBootInfo.secNumPackets = ((uint16_t)*pData++) << 8;
    sDrvPlcBootInfo.secNumPackets += *pData;
    pData +=15;
    
    /* Get Initial Vector */
    memcpy(sDrvPlcBootInfo.secIV, pData, 16);
    _DRV_PLC_BOOT_Rev(sDrvPlcBootInfo.secIV, 16);
    pData +=16;
    
    /* Get Signature */
    memcpy(sDrvPlcBootInfo.secSN, pData, 16);
    _DRV_PLC_BOOT_Rev(sDrvPlcBootInfo.secSN, 16);
}

static void _DRV_PLC_BOOT_SetSecureInfo(void)
{
    uint32_t regValue;
	uint8_t pValue[4];

    /* Set number of packets */
	regValue = (uint32_t)sDrvPlcBootInfo.secNumPackets - 1;
	pValue[3] = (uint8_t)(regValue >> 24);
	pValue[2] = (uint8_t)(regValue >> 16);
	pValue[1] = (uint8_t)(regValue >> 8);
	pValue[0] = (uint8_t)(regValue);
    sDrvPlcHalObj->sendBootCmd(DRV_PLC_BOOT_SET_DEC_NUM_PKTS, 0, 4, pValue, 
            NULL);

    /* Set Init Vector */
    sDrvPlcHalObj->sendBootCmd(DRV_PLC_BOOT_SET_DEC_INIT_VECT, 0, 16, 
            sDrvPlcBootInfo.secIV, NULL);

    /* Set Signature */
    sDrvPlcHalObj->sendBootCmd(DRV_PLC_BOOT_SET_DEC_SIGN, 0, 16, 
            sDrvPlcBootInfo.secSN, NULL);
    
}

static void _DRV_PLC_BOOT_SartDecryption(void)
{
    uint32_t regValue;

    /* Send Start Decryption */
	regValue = 0;
    sDrvPlcHalObj->sendBootCmd(DRV_PLC_BOOT_START_DECRYPT, 0, 4, 
            (uint8_t *)&regValue, NULL);

    /* Test Bootloader status : wait to AES block */
    uint32_t ul_boot_dbg = _DRV_PLC_BOOT_CheckStatus();
	while (ul_boot_dbg & PLC_FUSES_BOOT_ST_AES_ACT) 
    {
		regValue = 0xFFFF;
        while(regValue--);
        ul_boot_dbg = _DRV_PLC_BOOT_CheckStatus();
	}
    
//    /* Only for debug purposes */
//	ul_boot_dbg = _DRV_PLC_BOOT_CheckStatus();
//	if (ul_boot_dbg & PLC_FUSES_BOOT_ST_SIGN_OK) {
//		printf("SIGNATURE OK.\r\n");
//	} else {
//		printf("Error in SIGNATURE.\r\n");
//	}
}

static void _DRV_PLC_BOOT_FirmwareUploadTask(void)
{
    uint8_t *pData;
    uint32_t progAddr;
    uint16_t fragSize;
    uint8_t padding;
    
    /* Get next address to be programmed */
    progAddr = sDrvPlcBootInfo.pDst;
    
    if (sDrvPlcBootCb)
    {
        /* Fragmented Bootloader from external interactions */
        uint32_t address;
                
        /* Call function to get the next fragment of boot data */
        sDrvPlcBootCb(&address, &fragSize, sDrvPlcBootContext);
        pData = (uint8_t *)address;
        
        /* Check Secure Mode */
        if ((sDrvPlcBootInfo.secure) && (sDrvPlcBootInfo.secNumPackets == 0))
        {
            /* Catch meta-data from first fragment */
            _DRV_PLC_BOOT_GetSecureInfo(pData);
            pData += 48;
            fragSize -= 48;
        }
        
        if (fragSize)
        {
            /* Write fragment data */
            sDrvPlcHalObj->sendBootCmd(DRV_PLC_BOOT_CMD_WRITE_BUF, progAddr, fragSize, pData, NULL);
            sDrvPlcBootInfo.pendingLength = 1;
        }
        else
        {
            /* Data Bootloader has finished */
            sDrvPlcBootInfo.pendingLength = 0;
        }
    }
    else
    {
        /* Bootloader from internal FLASH memory */
        pData = (uint8_t *)sDrvPlcBootInfo.pSrc;

        if (sDrvPlcBootInfo.pendingLength > MAX_FRAG_SIZE) {
            fragSize = MAX_FRAG_SIZE;
            padding = 0;
        } else {
            fragSize = sDrvPlcBootInfo.pendingLength;
            padding = fragSize % 4;
            fragSize += padding;
        }
        
        /* Check Secure Mode */
        if ((sDrvPlcBootInfo.secure) && (sDrvPlcBootInfo.secNumPackets == 0))
        {
            /* Catch meta-data from first fragment */
            _DRV_PLC_BOOT_GetSecureInfo(pData);
            pData += 48;
            fragSize -= 48;
        }

        /* Write fragment data */
        sDrvPlcHalObj->sendBootCmd(DRV_PLC_BOOT_CMD_WRITE_BUF, progAddr, fragSize, pData, NULL);

        /* Update counters */
        sDrvPlcBootInfo.pendingLength -= (fragSize - padding);
        pData += fragSize;
        sDrvPlcBootInfo.pSrc = (uint32_t)pData;
    }
    
    /* Update counters */
    progAddr += fragSize;
    sDrvPlcBootInfo.pDst = progAddr;
}

static void _DRV_PLC_BOOT_EnableBootCmd(void)
{
    uint32_t reg_value;
    uint8_t cmd_value[4];
    
    /* Reset PLC transceiver */
    sDrvPlcHalObj->reset();
    
    /* Configure 8 bits transfer */
    sDrvPlcHalObj->setup(false);
    
    /* Enable Write operation in bootloader */
    cmd_value[3] = (uint8_t)(DRV_PLC_BOOT_WRITE_KEY >> 24);
    cmd_value[2] = (uint8_t)(DRV_PLC_BOOT_WRITE_KEY >> 16);
    cmd_value[1] = (uint8_t)(DRV_PLC_BOOT_WRITE_KEY >> 8);
    cmd_value[0] = (uint8_t)(DRV_PLC_BOOT_WRITE_KEY);
    sDrvPlcHalObj->sendBootCmd(DRV_PLC_BOOT_CMD_ENABLE_WRITE, 0, 4, cmd_value, NULL);

    cmd_value[3] = (uint8_t)(DRV_PLC_BOOT_WRITE_KEY >> 8);
    cmd_value[2] = (uint8_t)(DRV_PLC_BOOT_WRITE_KEY);
    cmd_value[1] = (uint8_t)(DRV_PLC_BOOT_WRITE_KEY >> 24);
    cmd_value[0] = (uint8_t)(DRV_PLC_BOOT_WRITE_KEY >> 16);
    sDrvPlcHalObj->sendBootCmd(DRV_PLC_BOOT_CMD_ENABLE_WRITE, 0, 4, cmd_value, NULL);

    /* System configuration */
    reg_value = PLC_MISCR_CPUWAIT | PLC_MISCR_PPM_CALIB_OFF | PLC_MISCR_MEM_96_96_CFG | PLC_MISCR_EN_ACCESS_ERROR | PLC_MISCR_SET_GPIO_12_ZC;
    cmd_value[3] = (uint8_t)(reg_value >> 24);
    cmd_value[2] = (uint8_t)(reg_value >> 16);
    cmd_value[1] = (uint8_t)(reg_value >> 8);
    cmd_value[0] = (uint8_t)(reg_value);
    sDrvPlcHalObj->sendBootCmd(DRV_PLC_BOOT_CMD_WRITE_WORD, PLC_MISCR, 4, cmd_value, NULL);
}

static void _DRV_PLC_BOOT_DisableBootCmd(void)
{
    uint32_t reg_value;
    uint8_t cmd_value[4];
    
    /* Disable CPU Wait */
    reg_value = PLC_MISCR_PPM_CALIB_OFF | PLC_MISCR_MEM_96_96_CFG | PLC_MISCR_EN_ACCESS_ERROR | PLC_MISCR_SET_GPIO_12_ZC;
    cmd_value[3] = (uint8_t)(reg_value >> 24);
    cmd_value[2] = (uint8_t)(reg_value >> 16);
    cmd_value[1] = (uint8_t)(reg_value >> 8);
    cmd_value[0] = (uint8_t)(reg_value);
    sDrvPlcHalObj->sendBootCmd(DRV_PLC_BOOT_CMD_WRITE_WORD, PLC_MISCR, 4, cmd_value, NULL);

    /* Disable Bootloader */
    sDrvPlcHalObj->sendBootCmd(DRV_PLC_BOOT_CMD_DIS_SPI_CLK_CTRL, 0, 0, NULL, NULL);
    
    /* Configure 16 bits transfer */
    sDrvPlcHalObj->setup(true);
    
    /* Wait to PLC startup (2ms) */
    sDrvPlcHalObj->delay(2000);
}

static bool _DRV_PLC_BOOT_CheckFirmware(void)
{
    DRV_PLC_HAL_CMD halCmd;
    DRV_PLC_HAL_INFO halInfo;
    uint8_t status[8];
    
    halCmd.cmd = DRV_PLC_HAL_CMD_RD;
    halCmd.memId = 0;
    halCmd.length = 8;
    halCmd.pData = status;    
    
    sDrvPlcHalObj->sendWrRdCmd(&halCmd, &halInfo);

    if (halInfo.key == DRV_PLC_HAL_KEY_CORTEX)
    {
        return true;
    }
    
    return false;
}

// *****************************************************************************
// *****************************************************************************
// Section: DRV_PLC_BOOT Common Interface Implementation
// *****************************************************************************
// *****************************************************************************

void DRV_PLC_BOOT_Start(DRV_PLC_BOOT_INFO *pBootInfo, DRV_PLC_HAL_INTERFACE *pHal)
{  
    sDrvPlcHalObj = pHal;
    
    sDrvPlcBootInfo.binSize = pBootInfo->binSize;
    sDrvPlcBootInfo.binStartAddress = pBootInfo->binStartAddress;
    sDrvPlcBootInfo.pendingLength = pBootInfo->binSize;
    sDrvPlcBootInfo.pSrc = pBootInfo->binStartAddress;  
    sDrvPlcBootInfo.secure = pBootInfo->secure;
    sDrvPlcBootInfo.pDst = DRV_PLC_BOOT_PROGRAM_ADDR;
    sDrvPlcBootInfo.secNumPackets = 0;
    
    /* Set Bootloader data callback to handle boot by external fragments */
    if (pBootInfo->bootDataCallback)
    {
        sDrvPlcBootInfo.bootDataCallback = pBootInfo->bootDataCallback;
        sDrvPlcBootInfo.contextBoot = pBootInfo->contextBoot;
    }

    _DRV_PLC_BOOT_EnableBootCmd();
    
    sDrvPlcBootInfo.status = DRV_PLC_BOOT_STATUS_PROCESING;
}

DRV_PLC_BOOT_STATUS DRV_PLC_BOOT_Status( void )
{
    return sDrvPlcBootInfo.status;
}

void DRV_PLC_BOOT_Tasks( void )
{
    if (sDrvPlcBootInfo.status == DRV_PLC_BOOT_STATUS_PROCESING)
    {
        _DRV_PLC_BOOT_FirmwareUploadTask();
        if (sDrvPlcBootInfo.pendingLength == 0)
        {
            /* Check Secure Mode */
            if (sDrvPlcBootInfo.secure)
            {   
                _DRV_PLC_BOOT_SetSecureInfo();
                _DRV_PLC_BOOT_SartDecryption();
            }
            /* Complete firmware upload */
            sDrvPlcBootInfo.status = DRV_PLC_BOOT_STATUS_SWITCHING;
        }
    }
    else if (sDrvPlcBootInfo.status == DRV_PLC_BOOT_STATUS_SWITCHING)
    {
        _DRV_PLC_BOOT_DisableBootCmd();
        sDrvPlcBootInfo.status = DRV_PLC_BOOT_STATUS_VALIDATING;
    }
    else if (sDrvPlcBootInfo.status == DRV_PLC_BOOT_STATUS_VALIDATING)
    {
        
        /* Check firmware */
        if (_DRV_PLC_BOOT_CheckFirmware())
        {
            /* Update boot status */
            sDrvPlcBootInfo.status = DRV_PLC_BOOT_STATUS_READY;
        }
        else
        {
            /* Update boot status */
            sDrvPlcBootInfo.status = DRV_PLC_BOOT_STATUS_ERROR;
        }
    }
}

void DRV_PLC_BOOT_Restart(bool update)
{
    if (!update)
    {
        /* Configure 8 bits transfer */
        sDrvPlcHalObj->setup(false);
        
        /* Disable Bootloader */
        sDrvPlcHalObj->sendBootCmd(DRV_PLC_BOOT_CMD_DIS_SPI_CLK_CTRL, 0, 0, NULL, NULL);

        /* Configure 16 bits transfer */
        sDrvPlcHalObj->setup(true);

        /* Wait to PLC startup (2ms) */
        sDrvPlcHalObj->delay(2000);
    }
    else
    {
        /* Restore initial boot parameters */
        sDrvPlcBootInfo.pendingLength = sDrvPlcBootInfo.binSize;
        sDrvPlcBootInfo.pSrc = sDrvPlcBootInfo.binStartAddress;
        sDrvPlcBootInfo.secNumPackets = 0;
        
        /* Enable Boot Command Mode */
        _DRV_PLC_BOOT_EnableBootCmd();
        
        sDrvPlcBootInfo.status = DRV_PLC_BOOT_STATUS_PROCESING;
        
        _DRV_PLC_BOOT_FirmwareUploadTask();
    }
}
