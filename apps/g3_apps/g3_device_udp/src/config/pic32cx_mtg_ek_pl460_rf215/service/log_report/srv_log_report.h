/*******************************************************************************
  Header for log report service

  Company:
    Microchip Technology Inc.

  File Name:
    srv_log_report.h

  Summary:
    Interface definition for the log report service.

  Description:
    This file defines the interface for the log report service.
    Debug messages and log information is printed on the console.
    If a display is available, debug code errors will be shown.
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

#ifndef SRV_LOG_REPORT_H
#define SRV_LOG_REPORT_H

#ifdef __cplusplus // Provide C++ Compatibility
 extern "C" {
#endif

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Log level enumeration

   Summary:
    Log message priority levels.

   Description:
    This enumeration maps the log levels to the supported system error
    message priority values.

   Remarks:
    Used to add the right individual message priority before reporting through
    SYS_DEBUG.
*/

typedef enum
{
    /* Errors that have the potential to cause a system crash. */
    SRV_LOG_REPORT_FATAL = 0,

    /* Errors that have the potential to cause incorrect behavior. */
    SRV_LOG_REPORT_ERROR = 1,

    /* Warnings about potentially unexpected behavior or side effects. */
    SRV_LOG_REPORT_WARNING = 2,

    /* Information helpful to understanding potential errors and warnings. */
    SRV_LOG_REPORT_INFO = 3,

    /* Verbose information helpful during debugging and testing. */
    SRV_LOG_REPORT_DEBUG = 4

} SRV_LOG_REPORT_LEVEL;


// *****************************************************************************
/* Error/warning code enumeration

   Summary:
    Error/warning codes.

   Description:
    This enumeration lists the error and warning codes.

   Remarks:
    None.
*/

typedef enum
{
    /* PRIME CL_432 Errors */
    CL_432_BAD_HANDLER_RECEIVED         =   100,
    CL_432_UNKNOWN_HANDLER_IN_CONFIRM   =   101,

    /* PRIME DB Errors */
    DB_GET_PARAM_BAD_ID                 =   200,
    DB_GET_PARAM_BAD_SIZE               =   201,
    DB_SET_PARAM_BAD_ID                 =   202,
    DB_SET_PARAM_BAD_SIZE               =   203,
    DB_GET_ELEM_BAD_ID                  =   204,
    DB_GET_ELEM_BAD_SIZE                =   205,
    DB_GET_ELEM_BAD_INDEX               =   206,
    DB_SET_ELEM_BAD_ID                  =   207,
    DB_SET_ELEM_BAD_SIZE                =   208,
    DB_SET_ELEM_BAD_INDEX               =   209,

    /* PRIME DISPATCHER Errors */
    DIS_ERROR_BUFF_ADDRESS_SEND         =   300,
    DIS_EMPTY_ELEM_INSERT               =   301,
    DIS_MODULE_NOT_REGISTERED           =   302,
    DIS_ADD_MSG_ALREADY_ADDED           =   303,
    DIS_MSG_GEN_ERROR_MSG_DEL           =   304,
    DIS_TRY_SEND_MSG_DEL                =   305,    
    DIS_NULL_BUFF_QUEUE                 =   306,

    /* PRIME NOTIFICATIONS Errors */
    NOT_NOT_VALID_SIGNAL                =   400,
    NOT_TABLE_FULL                      =   401,
    NOT_TOO_MANY_EVENTS                 =   402,
    NOT_WRONG_POINTER                   =   403,

    /* PRIME BMM Errors */
    BMM_ALLOC_BUFF_BIGGER_THAN_AVAILABLE=   500,
    BMM_ALLOC_NO_FREE_BUFF              =   501,
    BMM_FREE_FREE_BUFF                  =   502,  
    BMM_FREE_ERROR                      =   503,
    BMM_BUFFER_INTEGRITY_BROKEN         =   504,

    /* Queue Service Errors */
    QUEUE_FULL_INSERT_END               =   600,
    QUEUE_FULL_INSERT_BEFORE            =   601,
    QUEUE_FULL_INSERT_AFTER             =   602,
    QUEUE_FULL_INSERT_FIRST             =   603,
    QUEUE_EMPTY_REMOVE_HEAD             =   604,
    QUEUE_EMPTY_READ_REMOVE             =   605,
    QUEUE_FULL_APPEND                   =   606,
    QUEUE_APPEND_BAD_ELEMENT            =   607,
    QUEUE_BAD_ELEMENT                   =   608,
    QUEUE_BAD_TAIL                      =   609,
    QUEUE_APPEND_PRIO_SINGLE            =   610,
    QUEUE_NOT_INIT_NULL_SIZE            =   611,
    QUEUE_FIRST_BAD_INIT                =   612,
    QUEUE_LAST_NOT_TAIL                 =   613,
    QUEUE_TOO_BIG                       =   614,
    QUEUE_BAD_NEXT_ELEMENT              =   615,
    QUEUE_WRONG_CHAIN                   =   616,
    QUEUE_APPEND_AGAIN                  =   617,
    QUEUE_APPEND_AGAIN_ONE_ELEMENT      =   618,

    /* PRIME TMM Errors */
    TMM_NO_FREE_TIMER                   =   700,
    TMM_QUEUE_EMPTY_REMOVE              =   701,
    TMM_START_NULL_CB_FUNCTION          =   702,
    TMM_START_INVALID_TIMER_VALUE       =   703,
    TMM_START_NO_TIME                   =   704,

    /* PRIME BCN Errors */
    BCN_BAD_VALUES_ADJUSTING            =   800,
    BCN_PROGRAMMED_PAST                 =   801,
    BCN_NO_LSID                         =   802,

    /* PRIME IQ errors */
    IQ_PRO_BAD_QUEUE                    =   900,
    IQ_PRO_START_PRO                    =   901,
    IQ_PRO_CONT_PRO                     =   902,
    IQ_PRO_END_PRO                      =   903,
    IQ_PRO_BUSY_START_PRO               =   904,
    IQ_PRO_SERVICE_BAD_STATE            =   905,
    IQ_PRO_MAX_TIME_PROMOTING           =   906,
    IQ_NETWORK_NODE_DISCONNECTED_STATE  =   907,
    IQ_SN_STATE_NOT_ALLOWED             =   908,

    /* PRIME BSI Errors */
    BSI_ERROR                           =   1000,

    /* PRIME PRO Errors */
    PRO_START_PRO                       =   1100,
    PRO_END_PRO_OK                      =   1101,
    PRO_END_PRO_BAD                     =   1102,
    PRO_START_DEM                       =   1103,
    PRO_END_NODE_DISCONNECTED           =   1104,
    PRO_FIRST_DEM                       =   1105,
    PRO_NO_VBS_FREE                     =   1106,
    PRO_SWITCH_INFO_NOT_FOUND           =   1107,

    /* PRIME PHY Wrapper Errors */
    PHY_WRP_CRC_WRONG_HEADER_TYPE       =   1200,
    PHY_WRP_WRONG_LEN                   =   1201,
    PHY_WRP_WRONG_MSG_TYPE              =   1202,
    PHY_WRP_MUL_LCID_NOT_FOUND          =   1203,

    /* PRIME PNPDU Errors */
    PNU_ENHANCED_NOT_AVAILABLE          =   1300,

    /* PRIME SWI Errors */
    SWI_INVALID_CHANNEL                 =   1400,
    
    /* PRIME Management Plane Errors */
    MNGP_FU_ST_MACHINE_CONFIRM_TIMEOUT  =   1500,
    MNGP_FU_ST_MACHINE_WRONG_BUFFER     =   1501,
    MNGP_FU_NODE_MNG_CONFIRM_TIMEOUT    =   1502,
    
    /* PRIME DATA Errors */
    DATA_TX_EXTRA_QUEUE_FULL            =   1600,
    DATA_CONN_CLOSED_INVALID_HANDLER    =   1601,

    /* PRIME CON Errors */
    CON_UNICAST_NO_COMMON_ANCESTOR      =   1700,
    CON_UNICAST_BAD_RESPONSE            =   1701,
    CON_MULTICAST_SW_INCREASE_OVERFLOW  =   1702,
    CON_MULTICAST_SW_DECREASE_OVERFLOW  =   1703,    
    CON_MULTICAST_SW_SONS_INCREASE_OVERFLOW  =   1704,
    CON_MULTICAST_SW_SONS_DECREASE_OVERFLOW  =   1705,  
    CON_MULTICAST_NO_CONNECTIONS        =   1706,
    
    /* PRIME RM Errors */
    RM_NO_MODULATION_ASSIGNED           =   1800,
    
    /* PRIME ALV Errors */
    ALV_BAD_VALUES_RECEIVED             =   1900,
    
    /* USI Service Errors */
    USI_BAD_LENGTH                      =   9001,
    USI_BAD_PROTOCOL                    =   9002,
    USI_BAD_CRC                         =   9003,
    USI_INVALID_LENGTH                  =   9004,
    USI_ERROR_ESCAPE                    =   9005,

   /* PRIME PHY Layer Errors */
    PHY_LAYER_PLC_NOT_AVAILABLE         =   9100,
    PHY_LAYER_RF_NOT_AVAILABLE          =   9101,
    PAL_PLC_TIMER_SYNC_ERROR            =   9102,

    /* PRIME Critical System Error */
    CRITICAL_ERROR_PRIME_NO_PHY         =   9997,
    CRITICAL_ERROR_PRIME_NO_PLC         =   9998,
    CRITICAL_ERROR                      =   9999


} SRV_LOG_REPORT_CODE;

#define SRV_LOG_REPORT_Message_With_Code(logLevel, code, info, ...)
#define SRV_LOG_REPORT_Message(logLevel, info, ...)
#define SRV_LOG_REPORT_Buffer(logLevel, buffer, bufferLength, info, ...)

#ifdef __cplusplus // Provide C++ Compatibility
 }
#endif

#endif /* SRV_LOG_REPORT_H */
