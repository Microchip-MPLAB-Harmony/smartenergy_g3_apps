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
//DOM-IGNORE-END

#ifndef MAC_COMMON_H
#define MAC_COMMON_H

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

#define MAC_PIB_MAX_VALUE_LENGTH (144U)

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
    MAC_COMMON_PIB_POS_RECENT_ENTRY_THRESHOLD = 0x00000121, /* 8 bits */
    MAC_COMMON_PIB_EXTENDED_ADDRESS = 0x08000001 /* 8 Byte array */
} MAC_COMMON_PIB_ATTRIBUTE;


// *****************************************************************************
// *****************************************************************************
// Section: MAC Common Interface Routines
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
    MAC_COMMON_Init();
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
        MAC_COMMON_Reset();
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
    The MAC_COMMON_GetMsCounter primitive gets the value of a counter
    that is incremented every millisecond.

  Description:
    This primitive makes use of SYS_TIME service to get the value of the
    millisecond counter in order to be able to set timeouts and perform delays.
    This function returns the current value of such counter.

  Precondition:
    SYS_TIME_Initialize primitive has to be called before.

  Parameters:
    None.

  Returns:
    Value of milliseconds counter.

  Example:
    <code>
    previousCounter = MAC_COMMON_GetMsCounter();

    newCounter = MAC_COMMON_GetMsCounter();

    if ((newCounter - previousCounter) > TIMEOUT_MS)
    {

    }
    </code>

  Remarks:
    None.
*/
uint32_t MAC_COMMON_GetMsCounter(void);

// *****************************************************************************
/* Function:
    bool MAC_COMMON_TimeIsPast
    (
      int32_t timeValue
    )

  Summary:
    Indicates whether the given time value is in the past.

  Description:
    This primitive indicates whether the given time value is in the past.

  Precondition:
    SYS_TIME_Initialize primitive has to be called before.

  Parameters:
    timeValue      - Time value in milliseconds

  Returns:
    True if the time value is in the past.
    False if the time value is not in the past.

  Example:
    <code>
    int32_t validityTime = 5000;

    if (MAC_COMMON_TimeIsPast(validityTime))
    {

    }
    </code>

  Remarks:
    None.
*/
bool MAC_COMMON_TimeIsPast(int32_t timeValue);

// *****************************************************************************
/* Function:
    uint32_t MAC_COMMON_GetSecondsCounter
    (
      void
    )

  Summary:
    The MAC_COMMON_GetSecondsCounter primitive gets the value of a counter
    that is incremented every second.

  Description:
    A seconds counter is provided in order to be able to set timeouts
    and perform delays.
    This function returns the current value of such counter.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    Value of seconds counter.

  Example:
    <code>
    previousCounter = MAC_COMMON_GetSecondsCounter();

    newCounter = MAC_COMMON_GetSecondsCounter();

    if ((newCounter - previousCounter) > TIMEOUT_SECONDS)
    {

    }
    </code>

  Remarks:
    None.
*/
uint32_t MAC_COMMON_GetSecondsCounter(void);

// *****************************************************************************
/* Function:
    bool MAC_COMMON_TimeIsPastSeconds
    (
      int32_t timeValue
    )

  Summary:
    Indicates whether the given time value is in the past.

  Description:
    This primitive indicates whether the given time value is in the past.

  Precondition:
    None.

  Parameters:
    timeValue      - Time value in seconds

  Returns:
    True if the time value is in the past.
    False if the time value is not in the past.

  Example:
    <code>
    int32_t validityTime = 10;

    if (MAC_COMMON_TimeIsPastSeconds(validityTime))
    {

    }
    </code>

  Remarks:
    None.
*/
bool MAC_COMMON_TimeIsPastSeconds(int32_t timeValue);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // #ifndef MAC_COMMON_H

/*******************************************************************************
 End of File
*/
