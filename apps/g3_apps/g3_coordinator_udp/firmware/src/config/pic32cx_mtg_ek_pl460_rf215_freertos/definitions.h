/*******************************************************************************
  System Definitions

  File Name:
    definitions.h

  Summary:
    project system definitions.

  Description:
    This file contains the system-wide prototypes and definitions for a project.

 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
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

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "crypto/crypto.h"
#include "service/pcoup/srv_pcoup.h"
#include "stack/g3/pal/plc/pal_plc.h"
#include "driver/plc/g3MacRt/drv_g3_macrt_definitions.h"
#include "driver/plc/g3MacRt/drv_g3_macrt.h"
#include "driver/plc/g3MacRt/drv_g3_macrt_comm.h"
#include "peripheral/adc/plib_adc.h"
#include "peripheral/flexcom/spi/master/plib_flexcom3_spi_master.h"
#include "driver/rf215/drv_rf215.h"
#include "peripheral/flexcom/spi/master/plib_flexcom5_spi_master.h"
#include "stack/g3/mac/mac_common/mac_common.h"
#include "stack/g3/mac/mac_common/mac_common_defs.h"
#include "stack/g3/mac/mac_plc/mac_plc.h"
#include "stack/g3/mac/mac_plc/mac_plc_defs.h"
#include "stack/g3/mac/mac_plc/mac_plc_mib.h"
#include "stack/g3/mac/mac_rf/mac_rf.h"
#include "stack/g3/mac/mac_rf/mac_rf_defs.h"
#include "stack/g3/mac/mac_rf/mac_rf_mib.h"
#include "stack/g3/mac/mac_wrapper/mac_wrapper.h"
#include "stack/g3/mac/mac_wrapper/mac_wrapper_defs.h"
#include "stack/g3/adaptation/adp.h"
#include "stack/g3/adaptation/adp_api_types.h"
#include "stack/g3/adaptation/adp_shared_types.h"
#include "stack/g3/adaptation/lbp_defs.h"
#include "stack/g3/adaptation/lbp_coord.h"
#include "peripheral/tc/plib_tc0.h"
#include "peripheral/flexcom/usart/plib_flexcom0_usart.h"
#include "system/time/sys_time.h"
#include "service/log_report/srv_log_report.h"
#include "peripheral/trng/plib_trng.h"
#include "library/tcpip/tcpip.h"
#include "system/sys_time_h2_adapter.h"
#include "system/sys_random_h2_adapter.h"
#include "system/int/sys_int.h"
#include "system/ports/sys_ports.h"
#include "system/cache/sys_cache.h"
#include "osal/osal.h"
#include "system/debug/sys_debug.h"
#include "peripheral/dwdt/plib_dwdt.h"
#include "peripheral/clk/plib_clk.h"
#include "peripheral/rstc/plib_rstc.h"
#include "peripheral/nvic/plib_nvic.h"
#include "peripheral/cmcc/plib_cmcc.h"
#include "peripheral/pio/plib_pio.h"
#include "peripheral/supc/plib_supc.h"
#include "bsp/bsp.h"
#include "peripheral/sefc/plib_sefc0.h"
#include "peripheral/sefc/plib_sefc1.h"
#include "wolfssl/wolfcrypt/port/pic32/crypt_wolfcryptcb.h"
#include "stack/g3/pal/plc/pal_plc.h"
#include "service/pvddmon/srv_pvddmon.h"
#include "system/console/sys_console.h"
#include "system/console/src/sys_console_uart_definitions.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stack/g3/net/macg3adp/drv_mac_g3adp.h"
#include "app_g3_management.h"
#include "app_udp_responder.h"
#include "app_storage_pic32cxmt.h"
#include "app_eap_server.h"
#include "app_cycles.h"



// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END

/* Device Information */
#define DEVICE_NAME			 "PIC32CX2051MTG128"
#define DEVICE_ARCH			 "CORTEX-M4"
#define DEVICE_FAMILY		 "PIC32CX_MT"
#define DEVICE_SERIES		 "PIC32CXMTG"

/* CPU clock frequency */
#define CPU_CLOCK_FREQUENCY 200000000

// *****************************************************************************
// *****************************************************************************
// Section: System Functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* System Initialization Function

  Function:
    void SYS_Initialize( void *data )

  Summary:
    Function that initializes all modules in the system.

  Description:
    This function initializes all modules in the system, including any drivers,
    services, middleware, and applications.

  Precondition:
    None.

  Parameters:
    data            - Pointer to the data structure containing any data
                      necessary to initialize the module. This pointer may
                      be null if no data is required and default initialization
                      is to be used.

  Returns:
    None.

  Example:
    <code>
    SYS_Initialize ( NULL );

    while ( true )
    {
        SYS_Tasks ( );
    }
    </code>

  Remarks:
    This function will only be called once, after system reset.
*/

void SYS_Initialize( void *data );

// *****************************************************************************
/* System Tasks Function

Function:
    void SYS_Tasks ( void );

Summary:
    Function that performs all polled system tasks.

Description:
    This function performs all polled system tasks by calling the state machine
    "tasks" functions for all polled modules in the system, including drivers,
    services, middleware and applications.

Precondition:
    The SYS_Initialize function must have been called and completed.

Parameters:
    None.

Returns:
    None.

Example:
    <code>
    SYS_Initialize ( NULL );

    while ( true )
    {
        SYS_Tasks ( );
    }
    </code>

Remarks:
    If the module is interrupt driven, the system will call this routine from
    an interrupt context.
*/

void SYS_Tasks ( void );

// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* System Objects

Summary:
    Structure holding the system's object handles

Description:
    This structure contains the object handles for all objects in the
    MPLAB Harmony project's system configuration.

Remarks:
    These handles are returned from the "Initialize" functions for each module
    and must be passed into the "Tasks" function for each module.
*/

typedef struct
{
    SYS_MODULE_OBJ g3PalPlc;
    SYS_MODULE_OBJ  drvG3MacRt;

    SYS_MODULE_OBJ drvRf215;
    SYS_MODULE_OBJ g3MacWrapper;
    SYS_MODULE_OBJ g3Adp;

    SYS_MODULE_OBJ  sysTime;
    SYS_MODULE_OBJ  sysConsole0;


    SYS_MODULE_OBJ  tcpip;
    SYS_MODULE_OBJ  sysDebug;

    SYS_MODULE_OBJ g3PalRf;

} SYSTEM_OBJECTS;

// *****************************************************************************
// *****************************************************************************
// Section: extern declarations
// *****************************************************************************
// *****************************************************************************



extern SYSTEM_OBJECTS sysObj;

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* DEFINITIONS_H */
/*******************************************************************************
 End of File
*/

