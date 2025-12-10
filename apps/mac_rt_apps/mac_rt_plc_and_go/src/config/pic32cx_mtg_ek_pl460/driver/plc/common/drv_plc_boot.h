/*******************************************************************************
  DRV_PLC Bootloader Interface Header File

  Company:
    Microchip Technology Inc.

  File Name:
    drv_plc_boot.h

  Summary:
    PLC Driver Bootloader Interface Header File

  Description:
    The PLC bootloader provides a simple interface to handle the upload task
    of the firmware on PLC transceiver. This file provides the interface definition
    for the PLC bootloader driver.
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

#ifndef DRV_PLC_BOOT_H
#define DRV_PLC_BOOT_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************
#include <stdio.h>
#include <stdbool.h>
#include "driver/plc/common/drv_plc_hal.h"

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

#define DRV_PLC_BOOT_CMD_DIS_SPI_CTRL      0xA55AU

#define DRV_PLC_BOOT_CMD_DIS_SPI_CLK_CTRL  0xA66AU

#define DRV_PLC_BOOT_CMD_ENABLE_WRITE      0xDE05U

#define DRV_PLC_BOOT_PROGRAM_ADDR          0x00000000UL

#define DRV_PLC_BOOT_WRITE_KEY             0x5345ACBAUL

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* PLC Driver Boot Data Callback Function Pointer

   Summary
    Pointer to a PLC Driver Boot Data Callback Function

   Description
    This data type defines the required function signature for the PLC driver
    boot data callback function. A client must register a pointer
    using the PLC open function whose function signature (parameter
    and return value types) match the types specified by this function pointer
    in order to be notified when more data will be necessary while bootloader
    process.

    The parameters and return values are described here and a partial example
    implementation is provided.

  Parameters:
    address -           Address where new block of the binary data image is
                        ready to be transfered.

    length -            Length of the new block to be transfered. Maximum size
                        is 634 bytes. If length is set to 0, it means that
                        binary data image transfer has finished and PLC transceiver
                        is ready to start up.

    context -           Value identifying the context of the application that
                        registered the event handling function. In this case,
                        this value is fixed to the SYS_MODULE_INDEX passed in
                        open function.

  Returns:
    None.

  Example:
    <code>
    #define APP_BOOT_DATA_FRAG_SIZE    512

    static uint32_t gBootDataNextAddr = 0x00450000;
    static uint16_t gBootDataPending = 64000;

    static void APP_PLCBootDataCb(uint32_t *address, uint16_t *length, uintptr_t context)
    {
        (void)context;

        if (gBootDataPending)
        {
            *address = gBootDataNextAddr;

            gBootDataNextAddr += APP_BOOT_DATA_FRAG_SIZE;

            if (gBootDataPending > APP_BOOT_DATA_FRAG_SIZE)
            {
                *length = APP_BOOT_DATA_FRAG_SIZE;
                gBootDataPending -= APP_BOOT_DATA_FRAG_SIZE;
            }
            else
            {
                *length = APP_BOOT_DATA_FRAG_SIZE - gBootDataPending;
                gBootDataPending = 0;
            }
        }
        else
        {
            *length = 0;
        }
    }

    DRV_HANDLE handle;

    handle = DRV_PLC_MACRT_Open(DRV_PLC_MACRT_INDEX_0, APP_PLCBootDataCb);
    if (handle == DRV_HANDLE_INVALID)
    {

    }
    </code>

  Remarks:
    - The context parameter contains the SYS_MODULE_INDEX provided at the time
      the open function was called. This SYS_MODULE_INDEX value is passed back
      to the client as the "context" parameter.

    - Length parameter is used to notify that all binary image has been transfered
      to PLC Driver and it should be ready to start up. This is indicated using
      length = 0;

    - The event handler function executes in task context of the peripheral.
      Hence it is recommended of the application to not perform process
      intensive or blocking operations with in this function.

*/

typedef void ( *DRV_PLC_BOOT_DATA_CALLBACK )( uint32_t *address,
        uint16_t *length, uintptr_t context );

// *****************************************************************************
/* DRV_PLC_BOOT Command set

  Summary:
    Enumeration listing the DRV_PLC_BOOT commands.

  Description:
    This enumeration defines the commands used to interact with the DRV_PLC_BOOT
    of PLC transceiver.

  Remarks:
    None
*/

typedef enum
{
    /* Write Word (32 bits) */
    DRV_PLC_BOOT_CMD_WRITE_WORD         = 0x0000,

    /* Write buffer */
    DRV_PLC_BOOT_CMD_WRITE_BUF          = 0x0001,

    /* Read buffer */
    DRV_PLC_BOOT_CMD_READ_BUF           = 0x0002,

    /* Read Word (32 bits) */
    DRV_PLC_BOOT_CMD_READ_WORD          = 0x0003,

    /* Set number of decryption packets */
    DRV_PLC_BOOT_SET_DEC_NUM_PKTS       = 0x0004,

    /* Set decryption initial vector */
    DRV_PLC_BOOT_SET_DEC_INIT_VECT      = 0x0005,

    /* Set decryption signature */
    DRV_PLC_BOOT_SET_DEC_SIGN           = 0x0006,

    /* Set 128 bits fuses value */
    DRV_PLC_BOOT_SET_128_FUSES_VALUE    = 0x0007,

    /* Write buffer register to tamper value at KEY_ENC_FUSES */
    DRV_PLC_BOOT_SET_ENC_FUSES          = 0x0008,

    /* Write buffer register to tamper value at KEY_TAG_FUSES */
    DRV_PLC_BOOT_SET_TAG_FUSES          = 0x0009,

    /* Write buffer register to tamper value at CONTROL_FUSES */
    DRV_PLC_BOOT_SET_CTRL_FUSES         = 0x000B,

    /* Blown desired fuses */
    DRV_PLC_BOOT_BLOWN_FUSES            = 0x000C,

    /* Read KEY_ENC_FUSES to tamper register */
    DRV_PLC_BOOT_GET_ENC_FUSES          = 0x000D,

    /* Read KEY_TAG_FUSES to tamper register */
    DRV_PLC_BOOT_GET_TAG_FUSES          = 0x000E,

    /* Read CONTROL_FUSES to tamper register */
    DRV_PLC_BOOT_GET_CTRL_FUSES         = 0x0010,

    /* Read tamper register */
    DRV_PLC_BOOT_READ_TAMPER_REG        = 0x0011,

    /* Read bootloader status */
    DRV_PLC_BOOT_READ_BOOT_STATUS       = 0x0012,

    /* Start Decryption */
    DRV_PLC_BOOT_START_DECRYPT          = 0x0013,

    /* Stop Window */
    DRV_PLC_BOOT_STOP_WINDOW            = 0x0014

} DRV_PLC_BOOT_CMD;

typedef enum
{
    DRV_PLC_BOOT_STATUS_IDLE = 0,
    DRV_PLC_BOOT_STATUS_PROCESING,
    DRV_PLC_BOOT_STATUS_SWITCHING,
    DRV_PLC_BOOT_STATUS_STARTINGUP,
    DRV_PLC_BOOT_STATUS_VALIDATING,
    DRV_PLC_BOOT_STATUS_READY,
    DRV_PLC_BOOT_STATUS_ERROR,
} DRV_PLC_BOOT_STATUS;

typedef enum
{
    DRV_PLC_BOOT_RESTART_SOFT = 0,
    DRV_PLC_BOOT_RESTART_HARD,
    DRV_PLC_BOOT_RESTART_SLEEP,
} DRV_PLC_BOOT_RESTART_MODE;

typedef struct
{
    uint32_t                   binSize;
    uint32_t                   binStartAddress;
    bool                       secure;
    DRV_PLC_BOOT_STATUS        status;
    uint32_t                   pendingLength;
    uint32_t                   pSrc;
    uint32_t                   pDst;
    uint16_t                   secNumPackets;
    uint8_t                    secIV[16];
    uint8_t                    secSN[16];
    DRV_PLC_BOOT_DATA_CALLBACK bootDataCallback;
    uintptr_t                  contextBoot;
    uint8_t                    validationCounter;
} DRV_PLC_BOOT_INFO;

// *****************************************************************************
// *****************************************************************************
// Section: DRV_PLC_BOOT Software API definition
// *****************************************************************************
// *****************************************************************************

/* -------- Register Definition --------------------------------------------- */
#define PLC_MISCR                               (0x400E1800U) /**< (MISC) Miscelaneous Register */
#define PLC_RSTR                                (0x400E1804U) /**< (RSTR) Reset Register */
#define PLC_SR                                  (0x400E1808U) /**< (SR) Status Register */

/* -------- PLC_MISCR : Miscelaneous Register ----------------------------- */
#define PLC_MISCR_CPUWAIT                       (0x1UL << 0)   /**< (PLC_MISCR) Cortex M7 Hold */
#define PLC_MISCR_PPM_CALIB_ON                  (0x1UL << 8)   /**< (PLC_MISCR) PPM Calibration On */
#define PLC_MISCR_PPM_CALIB_OFF                 (0x0UL << 8)   /**< (PLC_MISCR) PPM Calibration Off */
#define PLC_MISCR_MEM_128_64_CFG                (0x0UL << 16)  /**< (PLC_MISCR) Memory configuration: 128kB ITCM - 64kB DTCM */
#define PLC_MISCR_MEM_96_96_CFG                 (0x1UL << 16)  /**< (PLC_MISCR) Memory configuration: 96kB ITCM - 96kB DTCM */
#define PLC_MISCR_EN_ACCESS_ERROR               (0x1UL << 24)  /**< (PLC_MISCR) Access Errors from CM7 enable */
#define PLC_MISCR_SET_GPIO_12_ZC                (0x0UL << 25)  /**< (PLC_MISCR) Change GPIO ZeroCross: ZC by GPIO_12 */
#define PLC_MISCR_SET_GPIO_2_ZC                 (0x1UL << 25)  /**< (PLC_MISCR) Change GPIO ZeroCross: ZC by GPIO_2 */
#define PLC_MISCR_SIGN_FAIL                     (0x1UL << 26)  /**< (PLC_MISCR) Check fail in Signature check */

/* -------- PLC_RSTR : Reset Register ------------------------------------- */
#define PLC_RSTR_EN_PROC_RESET                  (0x1UL << 0)   /**< (PLC_RSTR) Enable Processor Reset */
#define PLC_RSTR_EN_PER_RESET                   (0x1UL << 1)   /**< (PLC_RSTR) Enable Peripheral Reset */
#define PLC_RSTR_EN_PERSLCK_RESET               (0x1UL << 2)   /**< (PLC_RSTR) Enable Peripheral SLCK Reset */
#define PLC_RSTR_EN_LOCK_PLL                    (0x1UL << 3)   /**< (PLC_RSTR) Enable Lock PLL */
#define PLC_RSTR_VALUE_CONST_RST                (0x24UL << 16) /**< (PLC_RSTR) Number of cycles on active reset */

/* -------- PLC_SR : Status Register -------------------------------------- */
#define PLC_SR_WDT_RESET                        (0x1UL << 0)   /**< (PLC_RSTR) Watchdog Reset */
#define PLC_SR_CM7_RESET                        (0x1UL << 1)   /**< (PLC_RSTR) Cortex-M7 Reset */
#define PLC_SR_USR_RESET                        (0x1UL << 2)   /**< (PLC_RSTR) User Reset */

/* -------- PLC_Fuses : Fuses Control-------------------------------------- */
#define PLC_FUSES_ENCRNOTPLAIN                  (0x1UL << 0)   /**< (PLC_Fuses) Set to enable secure mode */
#define PLC_FUSES_READ_AES_KEY                  (0x1UL << 1)   /**< (PLC_Fuses) Set to disable AES key fuses reading.*/
#define PLC_FUSES_WRITE_AES_KEY                 (0x1UL << 2)   /**< (PLC_Fuses) Set to disable AES key fuses writing.*/
#define PLC_FUSES_READ_CONTROL                  (0x1UL << 5)   /**< (PLC_Fuses) Set to disable fuses control reading. */
#define PLC_FUSES_WRITE_CONTROL                 (0x1UL << 6)   /**< (PLC_Fuses) Set to disable fuses control writing. */
#define PLC_FUSES_READ_RAM                      (0x1UL << 7)   /**< (PLC_Fuses) Set to disable ram reading. */
#define PLC_FUSES_KEY_MCHP_RDY                  (0x1UL << 9)   /**< (PLC_Fuses) Set to disable bootloader in master mode */
#define PLC_FUSES_SIG_IV_NB                     (0x1UL << 10)  /**< (PLC_Fuses) Set to force IV + NB in Signature */
#define PLC_FUSES_DISABLE_DBG                   (0x1UL << 16)  /**< (PLC_Fuses) Set to disable Debug access */
#define PLC_FUSES_DBG_MSSC                      (0x1UL << 17)  /**< (PLC_Fuses) Set to Debug access depending on MSSC register */

/* -------- PLC_Boot_status : Boot status --------------------------------- */
#define PLC_FUSES_BOOT_ST_FUSES_ACT             (0x1UL << 0)   /**< (PLC_Boot_status) Fuses Blown active */
#define PLC_FUSES_BOOT_ST_AES_ACT               (0x1UL << 1)   /**< (PLC_Boot_status) AES active */
#define PLC_FUSES_BOOT_ST_SIGN_OK               (0x1UL << 2)   /**< (PLC_Boot_status) Signature OK */

// *****************************************************************************
// *****************************************************************************
// Section: DRV_PLC_BOOT Common Interface Implementation
// *****************************************************************************
// *****************************************************************************

void DRV_PLC_BOOT_Start( DRV_PLC_BOOT_INFO *pBootInfo, DRV_PLC_HAL_INTERFACE *pHal );
void DRV_PLC_BOOT_Tasks( void );
DRV_PLC_BOOT_STATUS DRV_PLC_BOOT_Status( void );
void DRV_PLC_BOOT_Restart(DRV_PLC_BOOT_RESTART_MODE mode);

/* Provide C++ Compatibility */
#ifdef __cplusplus

    }

#endif

#endif // #ifndef DRV_PLC_BOOT_H
/*******************************************************************************
 End of File
*/
