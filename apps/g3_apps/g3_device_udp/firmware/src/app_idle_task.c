/*******************************************************************************
 System Idle Task File

  File Name:
    app_idle_task.c

  Summary:
    This file contains source code necessary for FreeRTOS idle task

  Description:

  Remarks:
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

#include "definitions.h"
void app_idle_task( void )
{
    uint8_t PDS_Items_Pending = PDS_GetPendingItemsCount();
    bool RF_Cal_Needed = RF_NeedCal(); // device_support library API

    if (PDS_Items_Pending || RF_Cal_Needed)
    {
        if (1) // TODO: Modify to evaluate to true only if application is idle
        {
            if (PDS_Items_Pending)
            {
                PDS_StoreItemTaskHandler();
            }
            else if (RF_Cal_Needed)
            {
                RF_Timer_Cal(WSS_ENABLE_NONE);
            }
        }
    }
}




/*-----------------------------------------------------------*/
/*******************************************************************************
 End of File
 */
