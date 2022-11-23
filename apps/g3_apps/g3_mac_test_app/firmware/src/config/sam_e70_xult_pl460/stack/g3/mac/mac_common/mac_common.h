/*******************************************************************************
  G3 MAC Common Header File

  Company:
    Microchip Technology Inc.

  File Name:
    mac_common.h

  Summary:
    G3 MAC Common API Header File

  Description:
    This file contains definitions of the primitives and related types
    to be used by MAC Wrapper when accessing G3 MAC Common layer.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2022 Microchip Technology Inc. and its subsidiaries.
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

#ifndef _MAC_COMMON_H
#define _MAC_COMMON_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include "mac_common_defs.h"

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

#define MAC_PIB_MAX_VALUE_LENGTH (144)

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* MAC PIB Value Definition

   Summary:
    Defines the MAC PIB Value object.

   Description:
    The structure contains the fields which define a PIB Value,
    namely the size and the value itself.

   Remarks:
    None.
*/
typedef struct
{
    uint8_t length;
    uint8_t value[MAC_PIB_MAX_VALUE_LENGTH];
} MAC_PIB_VALUE;

// *****************************************************************************
/* MAC Common Parameter Information Base definition

   Summary:
    Lists the available objects in the MAC Parameter Information Base (PIB).

   Description:
    MAC PIB is a collection of objects that can be read/written in order to
    retrieve information and/or configure the MAC layer.

   Remarks:
    None.
*/
typedef enum
{
    MAC_COMMON_PIB_PAN_ID = 0x00000050, /* 16 bits */
    MAC_COMMON_PIB_PROMISCUOUS_MODE = 0x00000051, /* 8 bits (bool) */
    MAC_COMMON_PIB_SHORT_ADDRESS = 0x00000053, /* 16 bits */
    MAC_COMMON_PIB_KEY_TABLE = 0x00000071, /* 16 Byte entries */
    MAC_COMMON_PIB_POS_TABLE_ENTRY_TTL = 0x0000010E, /* 8 bits */
    MAC_COMMON_PIB_RC_COORD = 0x0000010F, /* 16 bits */
    MAC_COMMON_PIB_EXTENDED_ADDRESS = 0x08000001 /* 8 Byte array */
} MAC_COMMON_PIB_ATTRIBUTE;


// *****************************************************************************
// *****************************************************************************
// Section: MAC PLC Interface Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    MAC_COMMON_Init
    (
      void
    )

  Summary:
    Initializes the MAC Common module.

  Description:
    This routine initializes the MAC Common.
    No data is needed for this module initialization.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    // The following code snippet shows an example MAC Common initialization.

    MAC_COMMON_Init();

    // Initialize PLC MAC if needed
    // Initialize RF MAC if needed
    </code>

  Remarks:
    This routine must be called before any other MAC Common routine is called.
*/
void MAC_COMMON_Init(void);

// *****************************************************************************
/* Function:
    MAC_COMMON_Reset
    (
      void
    )

  Summary:
    The MAC_COMMON_Reset primitive resets the MAC Common module.

  Description:
    Reset operation initializes MAC Common PIB to their default values.

  Precondition:
    MAC_COMMON_Init primitive has to be called before.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    void globalResetRoutine(void)
    {
        // Reset G3 stack modules
        //...

        MAC_COMMON_Reset();
        
        // Reset other modules
        //...
    }
    </code>

  Remarks:
    None.
*/
void MAC_COMMON_Reset(void);

// *****************************************************************************
/* Function:
    MAC_STATUS MAC_COMMON_GetRequestSync
    (
      MAC_COMMON_PIB_ATTRIBUTE attribute,
      uint16_t index,
      MAC_PIB_VALUE *pibValue
    )

  Summary:
    The MAC_COMMON_GetRequestSync primitive gets the value of an attribute in the
    MAC layer Parameter Information Base (PIB).

  Description:
    GetRequestSync primitive is used to get the value of a PIB.
    Sync suffix indicates that result is provided upon function call return,
    in the pibValue parameter.

  Precondition:
    MAC_COMMON_Init primitive has to be called before.

  Parameters:
    attribute - Identifier of the Attribute to retrieve value

    index - Index of element in case Attribute is a table
            Otherwise index must be set to '0'

    pibValue - Pointer to MAC_PIB_VALUE object where value will be returned

  Returns:
    Result of get operation as a MAC_STATUS code.

  Example:
    <code>
    MAC_STATUS status;
    MAC_PIB_VALUE value;
    status = MAC_COMMON_GetRequestSync(MAC_COMMON_PIB_RC_COORD, 0, &value);
    if (status == MAC_STATUS_SUCCESS)
    {
        // Get value from 'value' parameter
    }
    </code>

  Remarks:
    None.
*/
MAC_STATUS MAC_COMMON_GetRequestSync(MAC_COMMON_PIB_ATTRIBUTE attribute, uint16_t index, MAC_PIB_VALUE *pibValue);

// *****************************************************************************
/* Function:
    MAC_STATUS MAC_COMMON_SetRequestSync
    (
      MAC_COMMON_PIB_ATTRIBUTE attribute,
      uint16_t index,
      const MAC_PIB_VALUE *pibValue
    )

  Summary:
    The MAC_COMMON_SetRequestSync primitive sets the value of an attribute in the
    MAC layer Parameter Information Base (PIB).

  Description:
    SetRequestSync primitive is used to set the value of a PIB.
    Sync suffix indicates that result of set operation is provided upon
    function call return, in the return status code.

  Precondition:
    MAC_COMMON_Init primitive has to be called before.

  Parameters:
    attribute - Identifier of the Attribute to provide value

    index - Index of element in case Attribute is a table
            Otherwise index must be set to '0'

    pibValue - Pointer to MAC_PIB_VALUE object where value is contained

  Returns:
    Result of set operation as a MAC_STATUS code.

  Example:
    <code>
    MAC_STATUS status;
    const MAC_PIB_VALUE value = {
        .length = 2,
        .value = {0xFF, 0xFF}
    };

    status = MAC_COMMON_SetRequestSync(MAC_COMMON_PIB_RC_COORD, 0, &value);
    if (status == MAC_STATUS_SUCCESS)
    {
        // PIB correctly set
    }
    </code>

  Remarks:
    None.
*/
MAC_STATUS MAC_COMMON_SetRequestSync(MAC_COMMON_PIB_ATTRIBUTE attribute, uint16_t index, const MAC_PIB_VALUE *pibValue);

// *****************************************************************************
/* Function:
    uint32_t MAC_COMMON_GetMsCounter
    (
      void
    )

  Summary:
    The MAC_COMMON_GetMsCounter primitive gets the value of a counter that is
    incremented every millisecond.

  Description:
    MAC layer makes use of SYS_TIME service to get the value of the millisecond
    counter in order to be able to set timeouts and perform delays.
    This function returns the current value of such counter.

  Precondition:
    MAC_COMMON_Init primitive has to be called before.

  Parameters:
    None.

  Returns:
    Value of MAC milliseconds counter.

  Example:
    <code>
    previousCounter = MAC_COMMON_GetMsCounter();

    // Perform other actions
    // ...

    newCounter = MAC_COMMON_GetMsCounter();

    if ((newCounter - previousCounter) > TIMEOUT_MS)
    {
        // Timeout elapsed
    }
    </code>

  Remarks:
    None.
*/
uint32_t MAC_COMMON_GetMsCounter(void);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // #ifndef _MAC_COMMON_H

/*******************************************************************************
 End of File
*/
