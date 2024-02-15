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
#define MAX_FRAG_SIZE      512U

static DRV_PLC_BOOT_DATA_CALLBACK sDrvPlcBootCb = NULL;
static uintptr_t sDrvPlcBootContext;

// *****************************************************************************
// *****************************************************************************
// Section: File scope functions
// *****************************************************************************
// *****************************************************************************
static void lDRV_PLC_BOOT_Rev(uint8_t *pDataDst, uint8_t len)
{
    uint8_t pTemp[16];

    for (uint8_t idx = 0; idx < len; idx++)
    {
        pTemp[idx] = pDataDst[15U - idx];
    }

    (void) memcpy(pDataDst, pTemp, len);
}

static uint32_t lDRV_PLC_BOOT_CheckStatus(void)
{
    uint32_t regValue;

    /* Send Start Decryption */
    regValue = 0;
    sDrvPlcHalObj->sendBootCmd(DRV_PLC_BOOT_READ_BOOT_STATUS, 0, 4,
            (uint8_t *)&regValue, (uint8_t *)&regValue);

    return regValue;
}

static void lDRV_PLC_BOOT_GetSecureInfo(uint8_t *pData)
{
    /* Get Number of packets */
    sDrvPlcBootInfo.secNumPackets = ((uint16_t)*pData++) << 8;
    sDrvPlcBootInfo.secNumPackets += *pData;
    pData +=15;

    /* Get Initial Vector */
    (void) memcpy(sDrvPlcBootInfo.secIV, pData, 16);
    lDRV_PLC_BOOT_Rev(sDrvPlcBootInfo.secIV, 16);
    pData +=16;

    /* Get Signature */
    (void) memcpy(sDrvPlcBootInfo.secSN, pData, 16);
    lDRV_PLC_BOOT_Rev(sDrvPlcBootInfo.secSN, 16);
}

static void lDRV_PLC_BOOT_SetSecureInfo(void)
{
    uint32_t regValue;
    uint8_t pValue[4];

    /* Set number of packets */
    regValue = (uint32_t)sDrvPlcBootInfo.secNumPackets - 1U;
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

static void lDRV_PLC_BOOT_SartDecryption(void)
{
    uint32_t regValue;

    /* Send Start Decryption */
    regValue = 0;
    sDrvPlcHalObj->sendBootCmd(DRV_PLC_BOOT_START_DECRYPT, 0, 4,
            (uint8_t *)&regValue, NULL);

    /* Test Bootloader status : wait to AES block */
    uint32_t ul_boot_dbg = lDRV_PLC_BOOT_CheckStatus();
    while ((ul_boot_dbg & PLC_FUSES_BOOT_ST_AES_ACT) != 0U)
    {
        regValue = 0xFFFF;
        while((regValue--) > 0U){}

        ul_boot_dbg = lDRV_PLC_BOOT_CheckStatus();
    }

    /* Only for debug purposes */
//    ul_boot_dbg = lDRV_PLC_BOOT_CheckStatus();
//    if (ul_boot_dbg & PLC_FUSES_BOOT_ST_SIGN_OK) {
//        printf("SIGNATURE OK.\r\n");
//    } else {
//        printf("Error in SIGNATURE.\r\n");
//    }
}

static void lDRV_PLC_BOOT_FirmwareUploadTask(void)
{
    uint8_t *pData;
    uint32_t progAddr;
    uint16_t fragSize;
    uint8_t padding = 0;

    /* Get next address to be programmed */
    progAddr = sDrvPlcBootInfo.pDst;

    if (sDrvPlcBootCb != NULL)
    {
        /* Fragmented Bootloader from external interactions */
        uint32_t address;

        /* Call function to get the next fragment of boot data */
        sDrvPlcBootCb(&address, &fragSize, sDrvPlcBootContext);
        pData = (uint8_t *)address;

        /* Check Secure Mode */
        if ((sDrvPlcBootInfo.secure) && (sDrvPlcBootInfo.secNumPackets == 0U))
        {
            /* Catch meta-data from first fragment */
            lDRV_PLC_BOOT_GetSecureInfo(pData);
            pData += 48U;
            fragSize -= 48U;
        }

        if (fragSize > 0U)
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
        uint16_t fragSizeReal;

        /* Bootloader from internal FLASH memory */
        pData = (uint8_t *)sDrvPlcBootInfo.pSrc;

        if (sDrvPlcBootInfo.pendingLength > MAX_FRAG_SIZE)
        {
            fragSizeReal = MAX_FRAG_SIZE;
        }
        else
        {
            fragSizeReal = (uint16_t)sDrvPlcBootInfo.pendingLength;
            padding = (uint8_t)(fragSizeReal % 4U);
            if (padding > 0U)
            {
                padding = 4U - padding;
            }
        }

        fragSize = fragSizeReal + padding;

        /* Check Secure Mode */
        if ((sDrvPlcBootInfo.secure) && (sDrvPlcBootInfo.secNumPackets == 0U))
        {
            /* Catch meta-data from first fragment */
            lDRV_PLC_BOOT_GetSecureInfo(pData);
            pData += 48U;
            fragSize -= 48U;
        }

        /* Write fragment data */
        sDrvPlcHalObj->sendBootCmd(DRV_PLC_BOOT_CMD_WRITE_BUF, progAddr, fragSize, pData, NULL);

        /* Update counters */
        sDrvPlcBootInfo.pendingLength -= fragSizeReal;
        pData += fragSize;
        sDrvPlcBootInfo.pSrc = (uint32_t)pData;
    }

    /* Update counters */
    progAddr += fragSize;
    sDrvPlcBootInfo.pDst = progAddr;
}

static void lDRV_PLC_BOOT_EnableBootCmd(void)
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

static void lDRV_PLC_BOOT_DisableBootCmd(void)
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

}

static bool lDRV_PLC_BOOT_CheckFirmware(void)
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

static void lDRV_PLC_BOOT_RestartProcess(void)
{
    sDrvPlcBootInfo.pendingLength = sDrvPlcBootInfo.binSize;
    sDrvPlcBootInfo.pSrc = sDrvPlcBootInfo.binStartAddress;
    sDrvPlcBootInfo.pDst = DRV_PLC_BOOT_PROGRAM_ADDR;
    sDrvPlcBootInfo.secNumPackets = 0;

    lDRV_PLC_BOOT_EnableBootCmd();

    sDrvPlcBootInfo.status = DRV_PLC_BOOT_STATUS_PROCESING;
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
    if (pBootInfo->bootDataCallback != NULL)
    {
        sDrvPlcBootInfo.bootDataCallback = pBootInfo->bootDataCallback;
        sDrvPlcBootInfo.contextBoot = pBootInfo->contextBoot;
    }

    lDRV_PLC_BOOT_EnableBootCmd();

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
        lDRV_PLC_BOOT_FirmwareUploadTask();
        if (sDrvPlcBootInfo.pendingLength == 0U)
        {
            /* Check Secure Mode */
            if (sDrvPlcBootInfo.secure)
            {
                lDRV_PLC_BOOT_SetSecureInfo();
                lDRV_PLC_BOOT_SartDecryption();
            }
            /* Complete firmware upload */
            sDrvPlcBootInfo.status = DRV_PLC_BOOT_STATUS_SWITCHING;
        }
    }
    else if (sDrvPlcBootInfo.status == DRV_PLC_BOOT_STATUS_SWITCHING)
    {
        uint32_t counter = 0;

        sDrvPlcBootInfo.status = DRV_PLC_BOOT_STATUS_STARTINGUP;

        lDRV_PLC_BOOT_DisableBootCmd();
        while(sDrvPlcHalObj->getPinLevel(sDrvPlcHalObj->plcPlib->extIntPio) == false)
        {
            counter++;
            if (counter > 0x1FFU)
            {
                sDrvPlcBootInfo.status = DRV_PLC_BOOT_STATUS_ERROR;
                break;
            }
        }
    }
    else if (sDrvPlcBootInfo.status == DRV_PLC_BOOT_STATUS_STARTINGUP)
    {
        if (sDrvPlcHalObj->getPinLevel(sDrvPlcHalObj->plcPlib->extIntPio) == false)
        {
            sDrvPlcBootInfo.validationCounter = 50;
            sDrvPlcBootInfo.status = DRV_PLC_BOOT_STATUS_VALIDATING;
        }
    }
    else
    {
        if (sDrvPlcBootInfo.status == DRV_PLC_BOOT_STATUS_VALIDATING)
        {
            /* Check firmware */
            if (lDRV_PLC_BOOT_CheckFirmware())
            {
                /* Update boot status */
                sDrvPlcBootInfo.status = DRV_PLC_BOOT_STATUS_READY;
            }
            else
            {
                if ((sDrvPlcBootInfo.validationCounter--) > 0U)
                {
                    sDrvPlcHalObj->delay(200);
                }
                else
                {
                    sDrvPlcBootInfo.status = DRV_PLC_BOOT_STATUS_ERROR;
                }
            }
        }
    }
}

void DRV_PLC_BOOT_Restart(DRV_PLC_BOOT_RESTART_MODE mode)
{
    if (mode == DRV_PLC_BOOT_RESTART_SOFT)
    {
        /* Configure 8 bits transfer */
        sDrvPlcHalObj->setup(false);

        /* Disable Bootloader */
        sDrvPlcHalObj->sendBootCmd(DRV_PLC_BOOT_CMD_DIS_SPI_CLK_CTRL, 0, 0, NULL, NULL);

        /* Wait to PLC startup */
        sDrvPlcHalObj->delay(200);
    }
    else if (mode == DRV_PLC_BOOT_RESTART_HARD)
    {
        /* Restart Boot process */
        lDRV_PLC_BOOT_RestartProcess();
    }
    else /* (mode == DRV_PLC_BOOT_RESTART_SLEEP) */
    {
        /* Enable Boot Command Mode */
        lDRV_PLC_BOOT_EnableBootCmd();
        lDRV_PLC_BOOT_DisableBootCmd();
        sDrvPlcBootInfo.status = DRV_PLC_BOOT_STATUS_VALIDATING;
    }
}
