/*******************************************************************************
  Queue Service Local Data Structures

  Company:
    Microchip Technology Inc.

  File Name:
    srv_queue_local.h

  Summary:
    Queue Service Local Data Structures

  Description:
    Queue Service Local Data Structures
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

#ifndef SRV_QUEUE_LOCAL_H
#define SRV_QUEUE_LOCAL_H

// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************

/* Queue Service Errors (reserved 200 - 299) */
#define QUEUE_FULL_INSERT_END               200UL
#define QUEUE_FULL_INSERT_BEFORE            201UL
#define QUEUE_FULL_INSERT_AFTER             202UL
#define QUEUE_FULL_INSERT_FIRST             203UL
#define QUEUE_EMPTY_REMOVE_HEAD             204UL
#define QUEUE_EMPTY_READ_REMOVE             205UL
#define QUEUE_FULL_APPEND                   206UL
#define QUEUE_APPEND_BAD_ELEMENT            207UL
#define QUEUE_BAD_ELEMENT                   208UL
#define QUEUE_BAD_TAIL                      209UL
#define QUEUE_APPEND_PRIO_SINGLE            210UL
#define QUEUE_NOT_INIT_NULL_SIZE            211UL
#define QUEUE_FIRST_BAD_INIT                212UL
#define QUEUE_LAST_NOT_TAIL                 213UL
#define QUEUE_TOO_BIG                       214UL
#define QUEUE_BAD_NEXT_ELEMENT              215UL
#define QUEUE_WRONG_CHAIN                   216UL
#define QUEUE_APPEND_AGAIN                  217UL
#define QUEUE_APPEND_AGAIN_ONE_ELEMENT      218UL

#endif //#ifndef SRV_QUEUE_LOCAL_H
