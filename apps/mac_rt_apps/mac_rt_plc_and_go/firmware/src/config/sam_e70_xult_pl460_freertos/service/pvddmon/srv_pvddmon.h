/*******************************************************************************
  Interface definition of PLC PVDD Monitor service.

  Company:
    Microchip Technology Inc.

  File Name:
    srv_pvddmon.h

  Summary:
    Interface definition of the PLC PVDD Monitor service.

  Description:
    This file defines the interface for the PLC PVDD Monitor service.
*******************************************************************************/

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

#ifndef SRV_PVDDMON_H    // Guards against multiple inclusion
#define SRV_PVDDMON_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include "device.h"
#include "interrupts.h"
#include "peripheral/afec/plib_afec_common.h"

#ifdef __cplusplus // Provide C++ Compatibility
 extern "C" {
#endif

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* List of PVDD Monitor comparison modes

 Summary:
    Defines two comparison modes.

 Description:
    This will be used to set the comparison mode.

*/
typedef enum
{
    /* Notify when PVDD level enters the comparison window */
    SRV_PVDDMON_CMP_MODE_IN,

    /* Notify when PVDD level leaves the comparison window */
    SRV_PVDDMON_CMP_MODE_OUT,

} SRV_PVDDMON_CMP_MODE;

// *****************************************************************************
/* PVDD Monitor Event Handler Function Pointer

   Summary
    Pointer to a PVDD Monitor Event handler function

   Description
    This data type defines the required function signature for the PVDD Monitor Event
    handling callback function. A client must register a pointer using the callback
    register function whose function signature match the types specified by this
    function pointer in order to receive a notification related the comparison event.

  Parameters:
    cmpMode - Comparison mode.

    context - Pointer to parameters to be passed to Handler function.

  Returns:
    None.

  Example:
    <code>
    static void APP_PLC_PVDDMonitorCb( SRV_PVDDMON_CMP_MODE cmpMode, uintptr_t context )
    {
        (void)context;

        if (cmpMode == SRV_PVDDMON_CMP_MODE_OUT)
        {
            DRV_PLC_PHY_EnableTX(appPlc.drvPlcHandle, false);
            appPlc.pvddMonTxEnable = false;
            SRV_PVDDMON_Restart(SRV_PVDDMON_CMP_MODE_IN);
        }
        else
        {
            DRV_PLC_PHY_EnableTX(appPlc.drvPlcHandle, true);
            appPlc.pvddMonTxEnable = true;
            SRV_PVDDMON_Restart(SRV_PVDDMON_CMP_MODE_OUT);
        }
    }
    </code>

  Remarks:
    - See SRV_PVDDMON_CMP_MODE for more details.

*/
typedef void (*SRV_PVDDMON_CALLBACK)( SRV_PVDDMON_CMP_MODE cmpMode, uintptr_t context );

/* High and Low threshold ADC values */
#define SRV_PVDDMON_HIGH_TRESHOLD              0xdb2U
#define SRV_PVDDMON_LOW_TRESHOLD               0xa88U
#define SRV_PVDDMON_HIGH_TRESHOLD_HYST         0xd97U
#define SRV_PVDDMON_LOW_TRESHOLD_HYST          0xaa3U

// *****************************************************************************
// *****************************************************************************
// Section: PLC PVDD Monitor Service Interface Definition
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    void SRV_PVDDMON_Initialize ( void )

  Summary:
    Initializes the PVDD Monitor service.

  Description:
    This routine initializes the PVDD Monitor service, making it ready for
    clients to start and use it.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    SRV_PVDDMON_Initialize();
    </code>

  Remarks:
    None
*/
void SRV_PVDDMON_Initialize (void);

// *****************************************************************************
/* Function:
    void SRV_PVDDMON_Start
    (
        SRV_PVDDMON_CMP_MODE cmpMode
    )

  Summary:
    Start the PVDD Monitor service.

  Description:
    This routine starts the PVDD Monitor service, configuring the ADC plib and
    setting the comparison mode.

  Precondition:
    Function SRV_PVDDMON_Initialize must have been called before calling this function.
    PVDD Monitor event handler must have been set before calling this function to
    be able to receive the PVDD Monitor events.

  Parameters:
    cmpMode - .

  Returns:
    None.

  Example:
    <code>
    SRV_PVDDMON_CallbackRegister(APP_PLC_PVDDMonitorCb, 0);
    SRV_PVDDMON_Start(SRV_PVDDMON_CMP_MODE_OUT);
    </code>

  Remarks:
    None
*/
void SRV_PVDDMON_Start (SRV_PVDDMON_CMP_MODE cmpMode);

// *****************************************************************************
/* Function:
    void SRV_PVDDMON_Restart
    (
        SRV_PVDDMON_CMP_MODE cmpMode
    )

  Summary:
    Restart the PVDD Monitor service.

  Description:
    This routine restarts the PVDD Monitor service, and allows clients to modify
    the comparison mode in runtime. This routine is commonly used in the PVDD Monitor
    event handler to switch the comparison mode once ADC has been previously configured
    and is running.

  Precondition:
    Function SRV_PVDDMON_Start must have been called before calling this function.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    static void APP_PLC_PVDDMonitorCb( SRV_PVDDMON_CMP_MODE cmpMode, uintptr_t context )
    {
        (void)context;

        if (cmpMode == SRV_PVDDMON_CMP_MODE_OUT)
        {
            DRV_PLC_PHY_EnableTX(appPlc.drvPlcHandle, false);
            appPlc.pvddMonTxEnable = false;
            SRV_PVDDMON_Restart(SRV_PVDDMON_CMP_MODE_IN);
        }
        else
        {
            DRV_PLC_PHY_EnableTX(appPlc.drvPlcHandle, true);
            appPlc.pvddMonTxEnable = true;
            SRV_PVDDMON_Restart(SRV_PVDDMON_CMP_MODE_OUT);
        }
    }
    </code>

  Remarks:
    None
*/
void SRV_PVDDMON_Restart (SRV_PVDDMON_CMP_MODE cmpMode);

// *****************************************************************************
/* Function:
    void SRV_PVDDMON_CallbackRegister
    (
        SRV_PVDDMON_CALLBACK callback,
        uintptr_t context
    )

  Summary:
    Allows a client to identify a PVDD Monitor event handling function to call
    back when the PVDD level is in/out comparison window.

  Description:
    The callback once set, persists until the client closes the sets another
    callback (which could be a "NULL" pointer to indicate no callback).

  Precondition:
    None.

  Parameters:
    callback - Pointer to the callback function.

    context - The value of parameter will be passed back to the client unchanged,
    when the callback function is called.

  Returns:
    None.

  Example:
    <code>
    SRV_PVDDMON_CallbackRegister(APP_PLC_PVDDMonitorCb, 0);
    SRV_PVDDMON_Start(SRV_PVDDMON_CMP_MODE_OUT);
    </code>

  Remarks:
    None
*/
void SRV_PVDDMON_CallbackRegister (SRV_PVDDMON_CALLBACK callback, uintptr_t context);

// *****************************************************************************
/* Function:
    bool SRV_PVDDMON_CheckWindow(void)

  Summary:
    Allows a client to check if the current ADC value is between correct threshold
    levels to be able to transmit through PLC.

  Description:
    Allows a client to check if the current ADC value is between correct threshold
    levels to be able to transmit through PLC. Threshold levels are obtained from
    the MCC.

  Precondition:
    Function SRV_PVDDMON_Start must have been called before calling this function.

  Parameters:
    None.

  Returns:
    True if the current ADC level is correct. Otherwise, false.

  Example:
    <code>
    if (SRV_PVDDMON_CheckWindow())
    {
        DRV_PLC_PHY_EnableTX(appData.drvPlcHandle, true);

        appData.pvddMonTxEnable = true;
    }
    else
    {
        DRV_PLC_PHY_EnableTX(appData.drvPlcHandle, false);

        appData.pvddMonTxEnable = false;
    }
    </code>

  Remarks:
    None
*/
bool SRV_PVDDMON_CheckWindow(void);

#ifdef __cplusplus // Provide C++ Compatibility
 }
#endif

#endif
