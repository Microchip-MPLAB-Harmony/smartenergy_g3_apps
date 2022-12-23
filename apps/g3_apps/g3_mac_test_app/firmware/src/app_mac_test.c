/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_mac_test.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <string.h>
#include "app_mac_test.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

#define SENDER
//#define RECEIVER

/* Comment/Uncommnent to perform tests on PLC/RF MAC */
#define MAC_TEST_RF

#define MAC_TEST_IOT

#ifdef MAC_TEST_IOT

#   define MAC_GR1_001
//#   define MAC_GR1_004
//#   define MAC_GR1_005
//#   define MAC_GR1_006

//#   define MAC_GR2_001
//#   define MAC_GR2_002
//#   define MAC_GR2_003

//#   define MAC_GR3_001
//#   define MAC_GR3_002
//#   define MAC_GR3_003
//#   define MAC_GR3_005
//#   define MAC_GR3_006
//#   define MAC_GR3_007
//#   define MAC_GR3_008
//#   define MAC_GR3_009
//#   define MAC_GR3_010
//#   define MAC_GR3_011
//#   define MAC_GR3_012

#endif /* MAC_TEST_IOT */

#define DELAY_BETWEEN_DATA_REQUESTS   500

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_MAC_TEST_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_MAC_TEST_DATA app_mac_testData;

/* MAC parameter values */
#if defined(SENDER)
static uint8_t extAddress[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
static uint16_t shortAddress = 0x002A;
#   if defined(MAC_GR3_005)
static uint16_t destAddress = 0x0089;
#   elif defined(MAC_GR3_007) || defined(MAC_GR3_010)
static uint16_t destAddress = 0xFFFF;
#   elif defined(MAC_GR3_008)
static uint16_t destAddress = 0xFFFF;
static uint16_t destPanId = 0xFFFF;
#   else
static uint16_t destAddress = 0x010C;
#   endif
#   if defined(MAC_GR3_006)
static uint8_t destExtAddress[8] = {0xAE, 0xAD, 0xAE, 0xAD, 0xAE, 0xAD, 0xAE, 0xAD};
#   else
static uint8_t destExtAddress[8] = {0x81, 0x72, 0x63, 0x54, 0x45, 0x36, 0x27, 0x18};
#   endif
#   if defined(MAC_GR1_001)
    static uint8_t dsnRF = 0x5E;
#   elif defined(MAC_GR1_004)
    static uint8_t dsnRF = 0x29;
#   elif defined(MAC_GR1_005) || defined(MAC_GR1_006)
    static uint8_t dsnRF = 0x6A;
#   elif defined(MAC_GR2_001) || defined(MAC_GR2_003) || defined(MAC_GR3_011) || defined(MAC_GR3_012)
    static uint8_t dsnRF = 0x29;
    static uint32_t frameCounterRF = 0x00000000;
#   elif defined(MAC_GR2_002)
    static uint8_t dsnRF = 0x2A;
    static uint32_t frameCounterRF = 0xA0125123;
#   elif defined(MAC_GR3_010)
    static uint8_t dsnRF = 0x55;
    static uint16_t dutyCyclePeriod = 60;
    static uint16_t dutyCycleLimit = 15;
    static uint8_t framesToSend = 20;
#   else
    static uint8_t dsnRF = 0x55;
#   endif /* MAC_GRx_xxx */
#elif defined(RECEIVER)
static uint8_t extAddress[8] = {0x81, 0x72, 0x63, 0x54, 0x45, 0x36, 0x27, 0x18};
static uint16_t shortAddress = 0x010C;
static uint16_t destAddress = 0x002A;
static uint8_t destExtAddress[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
#   if defined(MAC_GR2_001) || defined(MAC_GR2_002) || defined(MAC_GR2_003) || defined(MAC_GR3_011) || defined(MAC_GR3_012)
    static uint8_t dsnRF = 0x55;
    static uint32_t frameCounterRF = 0x00000000;
#   elif defined(MAC_GR3_010)
    static uint8_t dsnRF = 0x55;
    static uint8_t framesToReceive = 20;
#   else
    static uint8_t dsnRF = 0x55;
    static uint32_t frameCounterRF = 0x00000000;
#   endif /* MAC_GRx_xxx */
#else
#error "Neither SENDER nor RECEIVER defined"
#endif

#if defined(MAC_GR1_001)
    static uint8_t msdu[50] = {0x41, 0x60, 0x00, 0x00, 0x00, 0x00, 0x09, 0x3A, 0x01, 0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
            0x00, 0x78, 0x1D, 0x00, 0xFF, 0xFE, 0x00, 0x00, 0x2A, 0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x1D, 
            0x00, 0xFF, 0xFE, 0x00, 0x01, 0x0C, 0x80, 0x00, 0x8D, 0x41, 0x01, 0x02, 0x05, 0x06, 0x00};
    static uint16_t msduLength = 50;
    static uint8_t msduHandle = 0;
    static MAC_WRP_ADDRESS_MODE srcAddressMode = MAC_WRP_ADDRESS_MODE_SHORT;
    static MAC_WRP_ADDRESS_MODE destAddressMode = MAC_WRP_ADDRESS_MODE_SHORT;
    static uint8_t reqACK = 0;
    static MAC_WRP_SECURITY_LEVEL secLevel = MAC_WRP_SECURITY_LEVEL_NONE;
    static uint8_t keyIndex = 0;
    static MAC_WRP_QUALITY_OF_SERVICE qos = MAC_WRP_QUALITY_OF_SERVICE_NORMAL_PRIORITY;
    static uint8_t gmk[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16};
#elif defined(MAC_GR1_004)
    static uint8_t msdu[400] = {0x41, 0x60, 0x00, 0x00, 0x00, 0x01, 0x67, 0x3A, 0x01, 0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 
            0x00, 0x00, 0x78, 0x1D, 0x00, 0xFF, 0xFE, 0x00, 0x00, 0x2A, 0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
            0x78, 0x1D, 0x00, 0xFF, 0xFE, 0x00, 0x01, 0x0C, 0x80, 0x00, 0xF2, 0xF9, 0x01, 0x02, 0x05, 0x06, 0x00, 0x01, 
            0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 
            0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 
            0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 
            0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 
            0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 
            0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 
            0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F, 
            0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 0x90, 0x91, 
            0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, 0xA0, 0xA1, 0xA2, 0xA3, 
            0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 
            0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 
            0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 
            0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, 0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 
            0xEC, 0xED, 0xEE, 0xEF, 0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 
            0xFE, 0xFF, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 
            0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 
            0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 
            0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 
            0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 
            0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E};
    static uint16_t msduLength = 400;
    static uint8_t msduHandle = 0;
    static MAC_WRP_ADDRESS_MODE srcAddressMode = MAC_WRP_ADDRESS_MODE_SHORT;
    static MAC_WRP_ADDRESS_MODE destAddressMode = MAC_WRP_ADDRESS_MODE_SHORT;
    static uint8_t reqACK = 1;
    static MAC_WRP_SECURITY_LEVEL secLevel = MAC_WRP_SECURITY_LEVEL_NONE;
    static uint8_t keyIndex = 0;
    static MAC_WRP_QUALITY_OF_SERVICE qos = MAC_WRP_QUALITY_OF_SERVICE_NORMAL_PRIORITY;
    static uint8_t gmk[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16};
#elif defined(MAC_GR1_005) || defined(MAC_GR3_001) || defined(MAC_GR3_009)
    static uint8_t msdu[50] = {0x41, 0x60, 0x00, 0x00, 0x00, 0x00, 0x09, 0x3A, 0x01, 0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
            0x00, 0x78, 0x1D, 0x00, 0xFF, 0xFE, 0x00, 0x00, 0x2A, 0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x1D, 
            0x00, 0xFF, 0xFE, 0x00, 0x01, 0x0C, 0x80, 0x00, 0x8D, 0x41, 0x01, 0x02, 0x05, 0x06, 0x00};
    static uint16_t msduLength = 50;
    static uint8_t msduHandle = 0;
    static MAC_WRP_ADDRESS_MODE srcAddressMode = MAC_WRP_ADDRESS_MODE_SHORT;
    static MAC_WRP_ADDRESS_MODE destAddressMode = MAC_WRP_ADDRESS_MODE_SHORT;
    static uint8_t reqACK = 1;
    static MAC_WRP_SECURITY_LEVEL secLevel = MAC_WRP_SECURITY_LEVEL_NONE;
    static uint8_t keyIndex = 0;
    static MAC_WRP_QUALITY_OF_SERVICE qos = MAC_WRP_QUALITY_OF_SERVICE_NORMAL_PRIORITY;
    static uint8_t gmk[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16};
#   ifdef SENDER
#       ifdef MAC_TEST_RF
        static MAC_WRP_POS_ENTRY_RF posEntryRF = {
            0x010c, /* Short Address */
            0xFF, /* Forward LQI */
            0xFF, /* Reverse LQI */
            0, /* Duty Cycle */
            26, /* Forward Tx Power Offset */
            0, /* Reverse Tx Power Offset */
            7500, /* POS Valid Time (125*60) */
        };
#       else
        static MAC_WRP_POS_ENTRY posEntry = {
            0x010c, /* Short Address */
            0xF0, /* LQI */
            7500, /* POS Valid Time (125*60) */
        };
#       endif
#   else
#       ifdef MAC_TEST_RF
        static MAC_WRP_POS_ENTRY_RF posEntryRF = {
            0x002A, /* Short Address */
            0xFF, /* Forward LQI */
            0xFF, /* Reverse LQI */
            0, /* Duty Cycle */
            23, /* Forward Tx Power Offset */
            0, /* Reverse Tx Power Offset */
            7500, /* POS Valid Time (125*60) */
        };
#       else
        static MAC_WRP_POS_ENTRY posEntry = {
            0x010c, /* Short Address */
            0xF0, /* LQI */
            7500, /* POS Valid Time (125*60) */
        };
#       endif
#   endif
#elif defined(MAC_GR3_005)
    static uint8_t msdu[50] = {0x41, 0x60, 0x00, 0x00, 0x00, 0x00, 0x09, 0x3A, 0x01, 0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
            0x00, 0x78, 0x1D, 0x00, 0xFF, 0xFE, 0x00, 0x00, 0x2A, 0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x1D, 
            0x00, 0xFF, 0xFE, 0x00, 0x00, 0x89, 0x80, 0x00, 0x8D, 0xC4, 0x01, 0x02, 0x05, 0x06, 0x00};
    static uint16_t msduLength = 50;
    static uint8_t msduHandle = 0;
    static MAC_WRP_ADDRESS_MODE srcAddressMode = MAC_WRP_ADDRESS_MODE_SHORT;
    static MAC_WRP_ADDRESS_MODE destAddressMode = MAC_WRP_ADDRESS_MODE_SHORT;
    static uint8_t reqACK = 1;
    static MAC_WRP_SECURITY_LEVEL secLevel = MAC_WRP_SECURITY_LEVEL_NONE;
    static uint8_t keyIndex = 0;
    static MAC_WRP_QUALITY_OF_SERVICE qos = MAC_WRP_QUALITY_OF_SERVICE_NORMAL_PRIORITY;
    static uint8_t gmk[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16};
#elif defined(MAC_GR3_006)
    static uint8_t msdu[50] = {0x41, 0x60, 0x00, 0x00, 0x00, 0x00, 0x09, 0x3A, 0x01, 0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
            0x00, 0x78, 0x1D, 0x00, 0xFF, 0xFE, 0x00, 0x00, 0x2A, 0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xAE, 0xAD, 
            0xAE, 0xAD, 0xAE, 0xAD, 0xAE, 0xAD, 0x80, 0x00, 0x4A, 0xB4, 0x01, 0x02, 0x05, 0x06, 0x00};
    static uint16_t msduLength = 50;
    static uint8_t msduHandle = 0;
    static MAC_WRP_ADDRESS_MODE srcAddressMode = MAC_WRP_ADDRESS_MODE_SHORT;
    static MAC_WRP_ADDRESS_MODE destAddressMode = MAC_WRP_ADDRESS_MODE_EXTENDED;
    static uint8_t reqACK = 1;
    static MAC_WRP_SECURITY_LEVEL secLevel = MAC_WRP_SECURITY_LEVEL_NONE;
    static uint8_t keyIndex = 0;
    static MAC_WRP_QUALITY_OF_SERVICE qos = MAC_WRP_QUALITY_OF_SERVICE_NORMAL_PRIORITY;
    static uint8_t gmk[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16};
#elif defined(MAC_GR3_007)
    static uint8_t msdu[50] = {0x41, 0x60, 0x00, 0x00, 0x00, 0x00, 0x09, 0x3A, 0x01, 0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
            0x00, 0x78, 0x1D, 0x00, 0xFF, 0xFE, 0x00, 0x00, 0x2A, 0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x1D, 
            0x00, 0xFF, 0xFE, 0x00, 0xFF, 0xFF, 0x80, 0x00, 0x8E, 0x4D, 0x01, 0x02, 0x05, 0x06, 0x00};
    static uint16_t msduLength = 50;
    static uint8_t msduHandle = 0;
    static MAC_WRP_ADDRESS_MODE srcAddressMode = MAC_WRP_ADDRESS_MODE_SHORT;
    static MAC_WRP_ADDRESS_MODE destAddressMode = MAC_WRP_ADDRESS_MODE_SHORT;
    static uint8_t reqACK = 0;
    static MAC_WRP_SECURITY_LEVEL secLevel = MAC_WRP_SECURITY_LEVEL_NONE;
    static uint8_t keyIndex = 0;
    static MAC_WRP_QUALITY_OF_SERVICE qos = MAC_WRP_QUALITY_OF_SERVICE_NORMAL_PRIORITY;
    static uint8_t gmk[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16};
#elif defined(MAC_GR3_008)
    static uint8_t msdu[50] = {0x41, 0x60, 0x00, 0x00, 0x00, 0x00, 0x09, 0x3A, 0x01, 0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
            0x00, 0xFF, 0xFF, 0x00, 0xFF, 0xFE, 0x00, 0x00, 0x2A, 0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 
            0x00, 0xFF, 0xFE, 0x00, 0xFF, 0xFF, 0x80, 0x00, 0x7E, 0x88, 0x01, 0x02, 0x05, 0x06, 0x00};
    static uint16_t msduLength = 50;
    static uint8_t msduHandle = 0;
    static MAC_WRP_ADDRESS_MODE srcAddressMode = MAC_WRP_ADDRESS_MODE_SHORT;
    static MAC_WRP_ADDRESS_MODE destAddressMode = MAC_WRP_ADDRESS_MODE_SHORT;
    static uint8_t reqACK = 0;
    static MAC_WRP_SECURITY_LEVEL secLevel = MAC_WRP_SECURITY_LEVEL_NONE;
    static uint8_t keyIndex = 0;
    static MAC_WRP_QUALITY_OF_SERVICE qos = MAC_WRP_QUALITY_OF_SERVICE_NORMAL_PRIORITY;
    static uint8_t gmk[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16};
#elif defined(MAC_GR1_006) || defined(MAC_GR3_002) || defined(MAC_GR3_003)
    static uint16_t scanDuration = 15;
    static uint8_t msdu[4] = {0x11, 0x22, 0x33, 0x44};
    static uint16_t msduLength = 4;
    static uint8_t msduHandle = 0;
    static MAC_WRP_ADDRESS_MODE srcAddressMode = MAC_WRP_ADDRESS_MODE_SHORT;
    static MAC_WRP_ADDRESS_MODE destAddressMode = MAC_WRP_ADDRESS_MODE_SHORT;
    static uint8_t reqACK = 1;
    static MAC_WRP_SECURITY_LEVEL secLevel = MAC_WRP_SECURITY_LEVEL_NONE;
    static uint8_t keyIndex = 0;
    static MAC_WRP_QUALITY_OF_SERVICE qos = MAC_WRP_QUALITY_OF_SERVICE_NORMAL_PRIORITY;
    static uint8_t gmk[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16};
    #if defined(MAC_GR3_003) && defined(RECEIVER)
    static uint16_t rcCoord = 0x1234;
    #endif
#elif defined(MAC_GR2_001) || defined(MAC_GR3_011) || defined(MAC_GR3_012)
    static uint8_t msdu[50] = {0x41, 0x60, 0x00, 0x00, 0x00, 0x00, 0x09, 0x3A, 0x01, 0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
            0x00, 0x78, 0x1D, 0x00, 0xFF, 0xFE, 0x00, 0x00, 0x2A, 0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x1D, 
            0x00, 0xFF, 0xFE, 0x00, 0x01, 0x0C, 0x80, 0x00, 0x8D, 0x41, 0x01, 0x02, 0x05, 0x06, 0x00};
    static uint16_t msduLength = 50;
    static uint8_t msduHandle = 0;
    static MAC_WRP_ADDRESS_MODE srcAddressMode = MAC_WRP_ADDRESS_MODE_SHORT;
    static MAC_WRP_ADDRESS_MODE destAddressMode = MAC_WRP_ADDRESS_MODE_SHORT;
    static uint8_t reqACK = 1;
    static MAC_WRP_SECURITY_LEVEL secLevel = MAC_WRP_SECURITY_LEVEL_ENC_MIC_32;
    static uint8_t keyIndex = 1;
    static MAC_WRP_QUALITY_OF_SERVICE qos = MAC_WRP_QUALITY_OF_SERVICE_NORMAL_PRIORITY;
    static uint8_t gmk[16] = {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11};
#   if defined (MAC_GR3_011) && defined(RECEIVER)
    static uint8_t gmkDifferent[16] = {0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22};
#   endif
#elif defined(MAC_GR2_002)
    static uint8_t msdu[50] = {0x41, 0x60, 0x00, 0x00, 0x00, 0x00, 0x09, 0x3A, 0x01, 0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
            0x00, 0x78, 0x1D, 0x00, 0xFF, 0xFE, 0x00, 0x00, 0x2A, 0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x1D, 
            0x00, 0xFF, 0xFE, 0x00, 0x01, 0x0C, 0x80, 0x00, 0x8D, 0x41, 0x01, 0x02, 0x05, 0x06, 0x00};
    static uint16_t msduLength = 50;
    static uint8_t msduHandle = 0;
    static MAC_WRP_ADDRESS_MODE srcAddressMode = MAC_WRP_ADDRESS_MODE_SHORT;
    static MAC_WRP_ADDRESS_MODE destAddressMode = MAC_WRP_ADDRESS_MODE_SHORT;
    static uint8_t reqACK = 1;
    static MAC_WRP_SECURITY_LEVEL secLevel = MAC_WRP_SECURITY_LEVEL_ENC_MIC_32;
    static uint8_t keyIndex = 0;
    static MAC_WRP_QUALITY_OF_SERVICE qos = MAC_WRP_QUALITY_OF_SERVICE_NORMAL_PRIORITY;
    static uint8_t gmk[16] = {0xAB, 0x10, 0x34, 0x11, 0x45, 0x11, 0x1B, 0xC3, 0xC1, 0x2D, 0xE8, 0xFF, 0x11, 0x14, 0x22, 0x04};
#elif defined(MAC_GR2_003)
    static uint8_t msdu[400] = {0x41, 0x60, 0x00, 0x00, 0x00, 0x01, 0x67, 0x3A, 0x01, 0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 
            0x00, 0x00, 0x78, 0x1D, 0x00, 0xFF, 0xFE, 0x00, 0x00, 0x2A, 0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
            0x78, 0x1D, 0x00, 0xFF, 0xFE, 0x00, 0x01, 0x0C, 0x80, 0x00, 0xF2, 0xF9, 0x01, 0x02, 0x05, 0x06, 0x00, 0x01, 
            0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 
            0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 
            0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 
            0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 
            0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 
            0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 
            0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F, 
            0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 0x90, 0x91, 
            0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, 0xA0, 0xA1, 0xA2, 0xA3, 
            0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 
            0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 
            0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 
            0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, 0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 
            0xEC, 0xED, 0xEE, 0xEF, 0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 
            0xFE, 0xFF, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 
            0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 
            0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 
            0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 
            0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 
            0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E};
    static uint16_t msduLength = 400;
    static uint8_t msduHandle = 0;
    static MAC_WRP_ADDRESS_MODE srcAddressMode = MAC_WRP_ADDRESS_MODE_SHORT;
    static MAC_WRP_ADDRESS_MODE destAddressMode = MAC_WRP_ADDRESS_MODE_SHORT;
    static uint8_t reqACK = 1;
    static MAC_WRP_SECURITY_LEVEL secLevel = MAC_WRP_SECURITY_LEVEL_ENC_MIC_32;
    static uint8_t keyIndex = 1;
    static MAC_WRP_QUALITY_OF_SERVICE qos = MAC_WRP_QUALITY_OF_SERVICE_NORMAL_PRIORITY;
    static uint8_t gmk[16] = {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11};
#elif defined(MAC_GR3_010)
    static uint8_t msdu[400] = {0x41, 0x60, 0x00, 0x00, 0x00, 0x01, 0x67, 0x3A, 0x01, 0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 
            0x00, 0x00, 0x78, 0x1D, 0x00, 0xFF, 0xFE, 0x00, 0x00, 0x2A, 0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
            0x78, 0x1D, 0x00, 0xFF, 0xFE, 0x00, 0xFF, 0xFF, 0x80, 0x00, 0xF4, 0x05, 0x01, 0x02, 0x05, 0x06, 0x00, 0x01, 
            0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 
            0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 
            0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 
            0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 
            0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 
            0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 
            0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F, 
            0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 0x90, 0x91, 
            0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, 0xA0, 0xA1, 0xA2, 0xA3, 
            0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 
            0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 
            0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 
            0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, 0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 
            0xEC, 0xED, 0xEE, 0xEF, 0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 
            0xFE, 0xFF, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 
            0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 
            0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 
            0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 
            0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 
            0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E};
    static uint16_t msduLength = 400;
    static uint8_t msduHandle = 0;
    static MAC_WRP_ADDRESS_MODE srcAddressMode = MAC_WRP_ADDRESS_MODE_SHORT;
    static MAC_WRP_ADDRESS_MODE destAddressMode = MAC_WRP_ADDRESS_MODE_SHORT;
    static uint8_t reqACK = 0;
    static MAC_WRP_SECURITY_LEVEL secLevel = MAC_WRP_SECURITY_LEVEL_NONE;
    static uint8_t keyIndex = 0;
    static MAC_WRP_QUALITY_OF_SERVICE qos = MAC_WRP_QUALITY_OF_SERVICE_NORMAL_PRIORITY;
    static uint8_t gmk[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16};
#else
static uint8_t msdu[4] = {0x11, 0x22, 0x33, 0x44};
static uint16_t msduLength = 4;
#endif

static uint16_t panId = 0x781D;

/* Initial delay control */
static SYS_TIME_HANDLE tmr;
#define APP_INITIAL_DELAY_MS   2000

/* Console handle */
SYS_CONSOLE_HANDLE myConsoleHandle;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

static void APP_MAC_TEST_DataConfirm(MAC_WRP_DATA_CONFIRM_PARAMS *dcParams)
{
    if ((app_mac_testData.state == APP_MAC_TEST_STATE_WAIT_FOR_DATA_CONFIRM_RF) || (app_mac_testData.state == APP_MAC_TEST_STATE_WAIT_FOR_DATA_CONFIRM)) {
        if ((dcParams->mediaType == MAC_WRP_MEDIA_TYPE_CONF_RF) || (dcParams->mediaType == MAC_WRP_MEDIA_TYPE_CONF_PLC_AS_BACKUP)) {
            /* RF request */
            SYS_CONSOLE_Print(myConsoleHandle, "\r\n\r\n-- Mac Data Confirm RF --\r\n");
        }
        else {
            /* PLC request */
            SYS_CONSOLE_Print(myConsoleHandle, "\r\n\r\n-- Mac Data Confirm PLC --\r\n");
        }
        SYS_CONSOLE_Print(myConsoleHandle, "-- Msdu Handle: %u --\r\n", dcParams->msduHandle);
        SYS_CONSOLE_Print(myConsoleHandle, "-- Status: 0x%02X --\r\n", dcParams->status);

        if (dcParams->status == MAC_WRP_STATUS_SUCCESS) {
#ifdef MAC_TEST_IOT
            /* Finish test */
            if ((dcParams->mediaType == MAC_WRP_MEDIA_TYPE_CONF_RF) || (dcParams->mediaType == MAC_WRP_MEDIA_TYPE_CONF_PLC_AS_BACKUP)) {
                app_mac_testData.state = APP_MAC_TEST_STATE_STOP_RF;
            }
            else {
                app_mac_testData.state = APP_MAC_TEST_STATE_STOP;
            }
#   ifdef SENDER
#       if defined(MAC_GR1_001) || defined(MAC_GR1_004) \
                || defined(MAC_GR2_001) || defined(MAC_GR2_002) || defined(MAC_GR2_003) \
                || defined(MAC_GR3_001) || defined(MAC_GR3_007) || defined(MAC_GR3_008)
            SYS_CONSOLE_Print(myConsoleHandle, "\r\n\r\n-- Frame transmitted successfully --\r\n");
            SYS_CONSOLE_Print(myConsoleHandle, "-- Test OK --\r\n");
#       endif
#       if defined(MAC_GR3_005) || defined(MAC_GR3_006) || defined(MAC_GR3_011) || defined(MAC_GR3_012)
            SYS_CONSOLE_Print(myConsoleHandle, "\r\n\r\n-- ACK received when not expected --\r\n");
            SYS_CONSOLE_Print(myConsoleHandle, "-- Test FAILED --\r\n");
#       endif
#       if (defined(MAC_GR3_011) || defined(MAC_GR3_012))
            #ifdef MAC_TEST_RF
            SYS_CONSOLE_Print(myConsoleHandle, "\r\n\r\n-- ACK received when not expected --\r\n");
            SYS_CONSOLE_Print(myConsoleHandle, "-- Test FAILED --\r\n");
            #else
            SYS_CONSOLE_Print(myConsoleHandle, "\r\n\r\n-- Frame transmitted successfully --\r\n");
            SYS_CONSOLE_Print(myConsoleHandle, "-- Test OK --\r\n");
            #endif
#       endif
#       if defined(MAC_GR3_009) || defined(MAC_GR1_005)
            SYS_CONSOLE_Print(myConsoleHandle, "\r\n\r\n-- Frame transmitted successfully --\r\n");
            SYS_CONSOLE_Print(myConsoleHandle, "\r\n-- Retrieving POS Table --\r\n");
            if ((dcParams->mediaType == MAC_WRP_MEDIA_TYPE_CONF_RF) || (dcParams->mediaType == MAC_WRP_MEDIA_TYPE_CONF_PLC_AS_BACKUP)) {
                app_mac_testData.state = APP_MAC_TEST_STATE_GET_POS_TABLE_RF;
            }
            else {
                app_mac_testData.state = APP_MAC_TEST_STATE_GET_POS_TABLE;
            }
#       endif
#       if defined(MAC_GR3_010)
            SYS_CONSOLE_Print(myConsoleHandle, "\r\n\r\n-- Frame transmitted successfully. Frames Left: %u --\r\n", framesToSend - 1);
            if (--framesToSend == 0) {
                SYS_CONSOLE_Print(myConsoleHandle, "-- Test finished --\r\n");
            }
            else {
                SYS_TIME_DelayMS(DELAY_BETWEEN_DATA_REQUESTS, &tmr);
                if ((dcParams->mediaType == MAC_WRP_MEDIA_TYPE_CONF_RF) || (dcParams->mediaType == MAC_WRP_MEDIA_TYPE_CONF_PLC_AS_BACKUP)) {
                    app_mac_testData.state = APP_MAC_TEST_STATE_DATA_REQUEST_RF;
                }
                else {
                    app_mac_testData.state = APP_MAC_TEST_STATE_DATA_REQUEST;
                }
                SYS_CONSOLE_Print(myConsoleHandle, "-- Next Data Request in %u milliseconds... --\r\n", DELAY_BETWEEN_DATA_REQUESTS);
            }
#       endif
#   endif
#else
            /* Go to next step */
            if ((dcParams->mediaType == MAC_WRP_MEDIA_TYPE_CONF_RF) || (dcParams->mediaType == MAC_WRP_MEDIA_TYPE_CONF_PLC_AS_BACKUP)) {
                app_mac_testData.state = APP_MAC_TEST_STATE_WAIT_FOR_DATA_INDICATION_RF;
            }
            else {
                app_mac_testData.state = APP_MAC_TEST_STATE_WAIT_FOR_DATA_INDICATION;
            }
#endif /* MAC_TEST_IOT */
        }
        else {
            /* Retry */
            SYS_TIME_DelayMS(DELAY_BETWEEN_DATA_REQUESTS, &tmr);
            if ((dcParams->mediaType == MAC_WRP_MEDIA_TYPE_CONF_RF) || (dcParams->mediaType == MAC_WRP_MEDIA_TYPE_CONF_PLC_AS_BACKUP)) {
                app_mac_testData.state = APP_MAC_TEST_STATE_DATA_REQUEST_RF;
            }
            else {
                app_mac_testData.state = APP_MAC_TEST_STATE_DATA_REQUEST;
            }
#   ifdef SENDER
#       if defined(MAC_GR1_001) || defined(MAC_GR1_004) || defined(MAC_GR1_005) \
                || defined(MAC_GR2_001) || defined(MAC_GR2_002) || defined(MAC_GR2_003) \
                || defined(MAC_GR3_001) || defined(MAC_GR3_007) || defined(MAC_GR3_008) \
                || defined(MAC_GR3_009) || defined(MAC_GR3_010)
            SYS_CONSOLE_Print(myConsoleHandle, "\r\n\r\n-- Frame transmission failed with result 0x%02X --\r\n", dcParams->status);
            SYS_CONSOLE_Print(myConsoleHandle, "-- Test FAILED. Retrying in %u milliseconds... --\r\n", DELAY_BETWEEN_DATA_REQUESTS);
#       endif
#       if defined(MAC_GR3_005) || defined(MAC_GR3_006) || defined(MAC_GR3_011) || defined(MAC_GR3_012)
            if ((dcParams->mediaType == MAC_WRP_MEDIA_TYPE_CONF_RF) || (dcParams->mediaType == MAC_WRP_MEDIA_TYPE_CONF_PLC_AS_BACKUP)) {
                app_mac_testData.state = APP_MAC_TEST_STATE_STOP_RF;
            }
            else {
                app_mac_testData.state = APP_MAC_TEST_STATE_STOP;
            }
            if (dcParams->status == MAC_WRP_STATUS_NO_ACK) {
                SYS_CONSOLE_Print(myConsoleHandle, "\r\n\r\n-- Confirm with result NO ACK --\r\n");
                SYS_CONSOLE_Print(myConsoleHandle, "-- Test OK --\r\n");
            }
            else {
                SYS_CONSOLE_Print(myConsoleHandle, "\r\n\r\n-- Frame transmission failed with result othet than NO ACK (0x%02X) --\r\n", dcParams->status);
                SYS_CONSOLE_Print(myConsoleHandle, "-- Test FAILED --\r\n");
            }
#       endif
#   endif
        }
    }
}

static void APP_MAC_TEST_DataIndication(MAC_WRP_DATA_INDICATION_PARAMS *diParams)
{
    if ((diParams->mediaType == MAC_WRP_MEDIA_TYPE_IND_RF) || (diParams->mediaType == MAC_WRP_MEDIA_TYPE_IND_PLC)) {
        bool bFrameForMe;
        uint16_t i;

        bFrameForMe = ((diParams->destAddress.addressMode == MAC_WRP_ADDRESS_MODE_SHORT) && (diParams->destAddress.shortAddress == shortAddress))
                || ((diParams->destAddress.addressMode == MAC_WRP_ADDRESS_MODE_SHORT) && (diParams->destAddress.shortAddress == MAC_WRP_SHORT_ADDRESS_BROADCAST))
                || ((diParams->destAddress.addressMode == MAC_WRP_ADDRESS_MODE_EXTENDED) && (memcmp(diParams->destAddress.extendedAddress.address, extAddress, sizeof(extAddress)) == 0));

        if (diParams->mediaType == MAC_WRP_MEDIA_TYPE_IND_RF) {
            SYS_CONSOLE_Print(myConsoleHandle, "\r\n\r\n-- Mac Data Indication RF --\r\n");
        }
        else {
            SYS_CONSOLE_Print(myConsoleHandle, "\r\n\r\n-- Mac Data Indication PLC --\r\n");
        }
        SYS_CONSOLE_Print(myConsoleHandle, "-- Src PanID: 0x%04X --\r\n", diParams->srcPanId);
        if (diParams->srcAddress.addressMode == MAC_WRP_ADDRESS_MODE_SHORT) {
            SYS_CONSOLE_Print(myConsoleHandle, "-- Src Address Mode: Short Address --\r\n");
            SYS_CONSOLE_Print(myConsoleHandle, "-- Src Address: 0x%04X --\r\n", diParams->srcAddress.shortAddress);
        }
        else {
            SYS_CONSOLE_Print(myConsoleHandle, "-- Src Address Mode: Extended Address --\r\n");
            SYS_CONSOLE_Print(myConsoleHandle, "-- Src Address: ");
            for (i = 0; i < sizeof(diParams->srcAddress.extendedAddress); i++) {
                SYS_CONSOLE_Print(myConsoleHandle, "%02X", diParams->srcAddress.extendedAddress.address[i]);
            }
            SYS_CONSOLE_Print(myConsoleHandle, "\r\n");
        }
        SYS_CONSOLE_Print(myConsoleHandle, "-- Dest PanID: 0x%04X --\r\n", diParams->destPanId);
        if (diParams->destAddress.addressMode == MAC_WRP_ADDRESS_MODE_SHORT) {
            SYS_CONSOLE_Print(myConsoleHandle, "-- Dest Address Mode: Short Address --\r\n");
            SYS_CONSOLE_Print(myConsoleHandle, "-- Dest Address: 0x%04X --\r\n", diParams->destAddress.shortAddress);
        }
        else {
            SYS_CONSOLE_Print(myConsoleHandle, "-- Dest Address Mode: Extended Address --\r\n");
            SYS_CONSOLE_Print(myConsoleHandle, "-- Dest Address: ");
            for (i = 0; i < sizeof(diParams->destAddress.extendedAddress); i++) {
                SYS_CONSOLE_Print(myConsoleHandle, "%02X", diParams->destAddress.extendedAddress.address[i]);
            }
            SYS_CONSOLE_Print(myConsoleHandle, "\r\n");
        }
        SYS_CONSOLE_Print(myConsoleHandle, "-- MSDU Length: %u --\r\n", diParams->msduLength);
        SYS_CONSOLE_Print(myConsoleHandle, "-- MSDU: ");
        for (i = 0; i < diParams->msduLength; i++) {
            SYS_CONSOLE_Print(myConsoleHandle, "%02X", diParams->msdu[i]);
        }
        SYS_CONSOLE_Print(myConsoleHandle, "\r\n");
        SYS_CONSOLE_Print(myConsoleHandle, "-- LQI: 0x%02X --\r\n", diParams->linkQuality);
        SYS_CONSOLE_Print(myConsoleHandle, "-- DSN: 0x%02X --\r\n", diParams->dsn);
        SYS_CONSOLE_Print(myConsoleHandle, "-- Security Level: %u --\r\n", diParams->securityLevel);
        SYS_CONSOLE_Print(myConsoleHandle, "-- Key Index: %u --\r\n", diParams->keyIndex);
        SYS_CONSOLE_Print(myConsoleHandle, "-- QoS: %u --\r\n", diParams->qualityOfService);

        if (bFrameForMe) {
    #ifdef MAC_TEST_IOT
            /* Finish test */
            if (diParams->mediaType == MAC_WRP_MEDIA_TYPE_IND_RF) {
                app_mac_testData.state = APP_MAC_TEST_STATE_STOP_RF;
            }
            else {
                app_mac_testData.state = APP_MAC_TEST_STATE_STOP;
            }
    #   ifdef RECEIVER
    #       if defined(MAC_GR1_001) || defined(MAC_GR1_004) \
                || defined(MAC_GR2_001) || defined(MAC_GR2_002) || defined(MAC_GR2_003) \
                || defined(MAC_GR3_001) || defined(MAC_GR3_007) || defined(MAC_GR3_008)
            /* Check test result */
            if (memcmp(msdu, diParams->msdu, msduLength) == 0) {
                /* Frame as expected */
                SYS_CONSOLE_Print(myConsoleHandle, "\r\n\r\n-- Frame received as expected --\r\n");
                SYS_CONSOLE_Print(myConsoleHandle, "-- Test OK --\r\n");
            }
            else {
                /* Frame does not match */
                SYS_CONSOLE_Print(myConsoleHandle, "\r\n\r\n-- Rx Frame does not match --\r\n");
                SYS_CONSOLE_Print(myConsoleHandle, "-- Received: ");
                for (i = 0; i < diParams->msduLength; i++) {
                    SYS_CONSOLE_Print(myConsoleHandle, "%02X", diParams->msdu[i]);
                }
                SYS_CONSOLE_Print(myConsoleHandle, "\r\n-- Expected: ");
                for (i = 0; i < msduLength; i++) {
                    SYS_CONSOLE_Print(myConsoleHandle, "%02X", msdu[i]);
                }
                SYS_CONSOLE_Print(myConsoleHandle, "\r\n-- Test FAILED --\r\n");
            }
    #       endif
    #       if defined(MAC_GR3_005) || defined(MAC_GR3_006)
            SYS_CONSOLE_Print(myConsoleHandle, "\r\n\r\n-- Frame received when not expected on this test --\r\n");
            SYS_CONSOLE_Print(myConsoleHandle, "-- Test FAILED --\r\n");
    #       endif
    #       if defined(MAC_GR3_009) || defined(MAC_GR1_005)
            /* Check test result */
            if (memcmp(msdu, diParams->msdu, msduLength) == 0) {
                /* Frame as expected */
                SYS_CONSOLE_Print(myConsoleHandle, "\r\n\r\n-- Frame received as expected --\r\n");
                SYS_CONSOLE_Print(myConsoleHandle, "\r\n-- Retrieving POS Table --\r\n");
                if (diParams->mediaType == MAC_WRP_MEDIA_TYPE_IND_RF) {
                    app_mac_testData.state = APP_MAC_TEST_STATE_GET_POS_TABLE_RF;
                }
                else {
                    app_mac_testData.state = APP_MAC_TEST_STATE_GET_POS_TABLE;
                }
            }
            else {
                /* Frame does not match */
                SYS_CONSOLE_Print(myConsoleHandle, "\r\n\r\n-- Rx Frame does not match --\r\n");
                SYS_CONSOLE_Print(myConsoleHandle, "-- Received: ");
                for (i = 0; i < diParams->msduLength; i++) {
                    SYS_CONSOLE_Print(myConsoleHandle, "%02X", diParams->msdu[i]);
                }
                SYS_CONSOLE_Print(myConsoleHandle, "\r\n-- Expected: ");
                for (i = 0; i < msduLength; i++) {
                    SYS_CONSOLE_Print(myConsoleHandle, "%02X", msdu[i]);
                }
                SYS_CONSOLE_Print(myConsoleHandle, "\r\n-- Test FAILED --\r\n");
            }
    #       endif
    #       if defined(MAC_GR3_010)
            if (diParams->mediaType == MAC_WRP_MEDIA_TYPE_IND_RF) {
                framesToReceive--;
                SYS_CONSOLE_Print(myConsoleHandle, "\r\n\r\n-- Frame received successfully. Frames Left: %u --\r\n", framesToReceive);
                if (framesToReceive == 0) {
                    SYS_CONSOLE_Print(myConsoleHandle, "-- Test finished --\r\n");
                    SYS_CONSOLE_Print(myConsoleHandle, "\r\n-- Retrieving POS Table --\r\n");
                    app_mac_testData.state = APP_MAC_TEST_STATE_GET_POS_TABLE_RF;
                }
                else {
                    app_mac_testData.state = APP_MAC_TEST_STATE_WAIT_FOR_DATA_INDICATION_RF;
                }
            }
    #       endif
    #   endif
    #else
            SYS_TIME_DelayMS(DELAY_BETWEEN_DATA_REQUESTS, &tmr);
            /* Go to next step */
            app_mac_testData.state = APP_MAC_TEST_STATE_DATA_REQUEST_RF;
    #endif /* MAC_TEST_IOT */
        }
    }
}

static void APP_MAC_TEST_ResetConfirm(MAC_WRP_RESET_CONFIRM_PARAMS *rcParams)
{
    /* MAC Reset Confirm */
    (void)rcParams;
}

static void APP_MAC_TEST_BeaconIndication(MAC_WRP_BEACON_NOTIFY_INDICATION_PARAMS *bcnParams)
{
    if (bcnParams->panDescriptor.mediaType == MAC_WRP_MEDIA_TYPE_IND_RF) {
        /* print received beacon */
        SYS_CONSOLE_Print(myConsoleHandle, "\r\n\r\n-- Beacon received on RF: --\r\n");
        SYS_CONSOLE_Print(myConsoleHandle, "-- PanID: 0x%04X, LBA: 0x%04X, RCCoord: 0x%04X, LQI: 0x%02X --\r\n", 
                bcnParams->panDescriptor.panId, bcnParams->panDescriptor.lbaAddress, 
                bcnParams->panDescriptor.rcCoord, bcnParams->panDescriptor.linkQuality);
        /* Go to next step */
        app_mac_testData.state = APP_MAC_TEST_STATE_WAIT_FOR_SCAN_CONFIRM_RF;
    }
    else {
        /* print received beacon */
        SYS_CONSOLE_Print(myConsoleHandle, "\r\n\r\n-- Beacon received on PLC: --\r\n");
        SYS_CONSOLE_Print(myConsoleHandle, "-- PanID: 0x%04X, LBA: 0x%04X, RCCoord: 0x%04X, LQI: 0x%02X --\r\n", 
                bcnParams->panDescriptor.panId, bcnParams->panDescriptor.lbaAddress, 
                bcnParams->panDescriptor.rcCoord, bcnParams->panDescriptor.linkQuality);
        /* Go to next step */
        app_mac_testData.state = APP_MAC_TEST_STATE_WAIT_FOR_SCAN_CONFIRM_RF;
    }
}

static void APP_MAC_TEST_ScanConfirm(MAC_WRP_SCAN_CONFIRM_PARAMS *scParams)
{
    MAC_WRP_PIB_VALUE pibValue;

    if (app_mac_testData.state == APP_MAC_TEST_STATE_WAIT_FOR_SCAN_CONFIRM) {
#if defined(MAC_GR1_006) || defined(MAC_GR3_002) || defined(MAC_GR3_003)
        (void)pibValue;
        /* Go to next step */
        app_mac_testData.state = APP_MAC_TEST_STATE_STOP;
        if (scParams->status == MAC_WRP_STATUS_SUCCESS) {
            SYS_CONSOLE_Print(myConsoleHandle, "\r\n\r\n-- Scan Confirm with result Success --\r\n");
            SYS_CONSOLE_Print(myConsoleHandle, "-- Test OK --\r\n");
        }
        else {
            SYS_CONSOLE_Print(myConsoleHandle, "\r\n\r\n-- Scan Confirm failed with result 0x%02X --\r\n", scParams->status);
            SYS_CONSOLE_Print(myConsoleHandle, "-- Test FAILED --\r\n");
        }
#else
        (void)scParams;
        /* Set back PAN Id, as it has been reset on Scan Request */
        pibValue.length = sizeof(panId);
        memcpy(pibValue.value, &panId, sizeof(panId));
        MAC_WRP_SetRequestSync(app_mac_testData.macWrpHandle, MAC_WRP_PIB_PAN_ID, 0, &pibValue);
        /* Go to next step */
        SYS_TIME_DelayMS(DELAY_BETWEEN_DATA_REQUESTS, &tmr);
        app_mac_testData.state = APP_MAC_TEST_STATE_DATA_REQUEST;
#endif
    }
    else if (app_mac_testData.state == APP_MAC_TEST_STATE_WAIT_FOR_SCAN_CONFIRM_RF) {
#if defined(MAC_GR1_006) || defined(MAC_GR3_002) || defined(MAC_GR3_003)
        /* Go to next step */
        app_mac_testData.state = APP_MAC_TEST_STATE_STOP_RF;
        if (scParams->status == MAC_WRP_STATUS_SUCCESS) {
            SYS_CONSOLE_Print(myConsoleHandle, "\r\n\r\n-- Scan Confirm with result Success --\r\n");
            SYS_CONSOLE_Print(myConsoleHandle, "-- Test OK --\r\n");
        }
        else {
            SYS_CONSOLE_Print(myConsoleHandle, "\r\n\r\n-- Scan Confirm failed with result 0x%02X --\r\n", scParams->status);
            SYS_CONSOLE_Print(myConsoleHandle, "-- Test FAILED --\r\n");
        }
#else
        (void)scParams;
        /* Set back PAN Id, as it has been reset on Scan Request */
        pibValue.length = sizeof(panId);
        memcpy(pibValue.value, &panId, sizeof(panId));
        MAC_WRP_SetRequestSync(app_mac_testData.macWrpHandle, MAC_WRP_PIB_PAN_ID, 0, &pibValue);
        /* Go to next step */
        SYS_TIME_DelayMS(DELAY_BETWEEN_DATA_REQUESTS, &tmr);
        app_mac_testData.state = APP_MAC_TEST_STATE_DATA_REQUEST_RF;
#endif
    }
}

static void APP_MAC_TEST_StartConfirm(MAC_WRP_START_CONFIRM_PARAMS *scParams)
{
    /* MAC Start Confirm */
    (void)scParams;
}

static void APP_MAC_TEST_CommStatusIndication(MAC_WRP_COMM_STATUS_INDICATION_PARAMS *csParams)
{
#if defined(MAC_GR3_011)
    if (csParams->status == MAC_WRP_STATUS_SECURITY_ERROR) {
        SYS_CONSOLE_Print(myConsoleHandle, "\r\n\r\n-- Comm Status Indication with result SECURITY ERROR --\r\n");
        SYS_CONSOLE_Print(myConsoleHandle, "-- Test OK --\r\n");
    }
    else{
        SYS_CONSOLE_Print(myConsoleHandle, "\r\n\r\n-- Comm Status Indication with result 0x%02X --\r\n", csParams->status);
        SYS_CONSOLE_Print(myConsoleHandle, "-- Test FAILED --\r\n");
    }
#elif defined(MAC_GR3_012)
    if (csParams->status == MAC_WRP_STATUS_UNAVAILABLE_KEY) {
        SYS_CONSOLE_Print(myConsoleHandle, "\r\n\r\n-- Comm Status Indication with result UNAVAILABLE KEY --\r\n");
        SYS_CONSOLE_Print(myConsoleHandle, "-- Test OK --\r\n");
    }
    else{
        SYS_CONSOLE_Print(myConsoleHandle, "\r\n\r\n-- Comm Status Indication with result 0x%02X --\r\n", csParams->status);
        SYS_CONSOLE_Print(myConsoleHandle, "-- Test FAILED --\r\n");
    }
#else
    (void)csParams;
#endif
}

static void GetAndPrintPOSTableRF(void)
{
    MAC_WRP_PIB_VALUE pibValue;
    MAC_WRP_POS_ENTRY_RF entry;
    
    /* Get POS Table entry 0 */
    if (MAC_WRP_GetRequestSync(app_mac_testData.macWrpHandle, MAC_WRP_PIB_POS_TABLE_RF, 0, &pibValue) == MAC_WRP_STATUS_SUCCESS) {
        /* Print entry */
        memcpy(&entry, pibValue.value, pibValue.length);
        SYS_CONSOLE_Print(myConsoleHandle, "-- POS Table entry 0 --\r\n");
        SYS_CONSOLE_Print(myConsoleHandle, "-- Short Address: 0x%04X\r\n", entry.shortAddress);
        SYS_CONSOLE_Print(myConsoleHandle, "-- Forward LQI: 0x%02X\r\n", entry.forwardLqi);
        SYS_CONSOLE_Print(myConsoleHandle, "-- Reverse LQI: 0x%02X\r\n", entry.reverseLqi);
        SYS_CONSOLE_Print(myConsoleHandle, "-- Duty Cyle: %u\r\n", entry.dutyCycle);
        SYS_CONSOLE_Print(myConsoleHandle, "-- Forward Tx Power Offset: %u\r\n", entry.forwardTxPowerOffset);
        SYS_CONSOLE_Print(myConsoleHandle, "-- Reverse Tx Power Offset: %u\r\n", entry.reverseTxPowerOffset);
        SYS_CONSOLE_Print(myConsoleHandle, "-- Valid Time: %u\r\n\r\n", (entry.posValidTime + 59) / 60); /* Seconds to minutes */
    }
    else {
        SYS_CONSOLE_Print(myConsoleHandle, "-- Could not retrieve POS Table --\r\n");
    }

    /* Finish test */
    app_mac_testData.state = APP_MAC_TEST_STATE_STOP_RF;
}

static void GetAndPrintPOSTable(void)
{
    MAC_WRP_PIB_VALUE pibValue;
    MAC_WRP_POS_ENTRY entry;
    
    /* Get POS Table entry 0 */
    if (MAC_WRP_GetRequestSync(app_mac_testData.macWrpHandle, MAC_WRP_PIB_POS_TABLE, 0, &pibValue) == MAC_WRP_STATUS_SUCCESS) {
        /* Print entry */
        memcpy(&entry, pibValue.value, pibValue.length);
        SYS_CONSOLE_Print(myConsoleHandle, "-- POS Table entry 0 --\r\n");
        SYS_CONSOLE_Print(myConsoleHandle, "-- Short Address: 0x%04X\r\n", entry.shortAddress);
        SYS_CONSOLE_Print(myConsoleHandle, "-- LQI: 0x%02X\r\n", entry.lqi);
        SYS_CONSOLE_Print(myConsoleHandle, "-- Valid Time: %u\r\n\r\n", (entry.posValidTime + 59) / 60); /* Seconds to minutes */
    }
    else {
        SYS_CONSOLE_Print(myConsoleHandle, "-- Could not retrieve POS Table --\r\n");
    }

    /* Finish test */
    app_mac_testData.state = APP_MAC_TEST_STATE_STOP;
}


// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

static void APP_MAC_TEST_SetParams(void)
{
    MAC_WRP_PIB_ATTRIBUTE eAttribute;
    uint16_t u16Index;
    MAC_WRP_PIB_VALUE pibValue;
    MAC_WRP_STATUS eStatus;
#ifndef MAC_TEST_IOT
    uint16_t duty_cycle = 3600; /* 100% */
    uint8_t retries = 0;
#endif
    
#ifndef MAC_TEST_IOT
    /* Only disable retries and max duty cycle */
    eAttribute = MAC_WRP_PIB_MAX_FRAME_RETRIES_RF;
    u16Index = 0;
    pibValue.length = sizeof(retries);
    memcpy(pibValue.value, &retries, sizeof(retries));
    eStatus = MAC_WRP_SetRequestSync(app_mac_testData.macWrpHandle, eAttribute, u16Index, &pibValue);
    eAttribute = MAC_WRP_PIB_DUTY_CYCLE_LIMIT_RF;
    u16Index = 0;
    pibValue.length = sizeof(duty_cycle);
    memcpy(pibValue.value, &retries, sizeof(duty_cycle));
    eStatus = MAC_WRP_SetRequestSync(app_mac_testData.macWrpHandle, eAttribute, u16Index, &pibValue);
    return;
#endif

#if defined(MAC_GR3_002) || defined(MAC_GR3_003)
    /* Set IBs */
    eAttribute = MAC_WRP_PIB_MANUF_EXTENDED_ADDRESS;
    u16Index = 0;
    pibValue.length = sizeof(destExtAddress);
    memcpy(pibValue.value, destExtAddress, sizeof(destExtAddress));
    eStatus = MAC_WRP_SetRequestSync(app_mac_testData.macWrpHandle, eAttribute, u16Index, &pibValue);
#if defined(MAC_GR3_002) && defined(RECEIVER)
    /* Short Address */
    uint16_t coordAddress = 0;
    eAttribute = MAC_WRP_PIB_SHORT_ADDRESS;
    u16Index = 0;
    pibValue.length = sizeof(coordAddress);
    memcpy(pibValue.value, &coordAddress, sizeof(coordAddress));
    eStatus = MAC_WRP_SetRequestSync(app_mac_testData.macWrpHandle, eAttribute, u16Index, &pibValue);
    /* Start Coordinator */
    struct TMacWrpStartRequest startReq;
    startReq.panId = panId;
    MacWrapperMlmeStartRequest(&startReq);
#endif
#if defined(MAC_GR3_003) && defined(RECEIVER)
    /* PAN Id */
    eAttribute = MAC_WRP_PIB_PAN_ID;
    u16Index = 0;
    pibValue.length = sizeof(panId);
    memcpy(pibValue.value, &panId, sizeof(panId));
    eStatus = MAC_WRP_SetRequestSync(app_mac_testData.macWrpHandle, eAttribute, u16Index, &pibValue);
    /* Short Address */
    eAttribute = MAC_WRP_PIB_SHORT_ADDRESS;
    u16Index = 0;
    pibValue.length = sizeof(destAddress);
    memcpy(pibValue.value, &destAddress, sizeof(destAddress));
    eStatus = MAC_WRP_SetRequestSync(app_mac_testData.macWrpHandle, eAttribute, u16Index, &pibValue);
    /* RC Coord */
    eAttribute = MAC_WRP_PIB_RC_COORD;
    u16Index = 0;
    pibValue.length = sizeof(rcCoord);
    memcpy(pibValue.value, &rcCoord, sizeof(rcCoord));
    eStatus = MAC_WRP_SetRequestSync(app_mac_testData.macWrpHandle, eAttribute, u16Index, &pibValue);
#endif
    /* No more config needed */
    //return;
#else

    /* Set IBs */
    /* Extended Address */
    eAttribute = MAC_WRP_PIB_MANUF_EXTENDED_ADDRESS;
    u16Index = 0;
    pibValue.length = sizeof(extAddress);
    memcpy(pibValue.value, extAddress, sizeof(extAddress));
    eStatus = MAC_WRP_SetRequestSync(app_mac_testData.macWrpHandle, eAttribute, u16Index, &pibValue);
#if defined (MAC_GR3_011) && defined(RECEIVER)
    /* Key Table */
    eAttribute = MAC_WRP_PIB_KEY_TABLE;
    u16Index = keyIndex;
    pibValue.length = sizeof(gmkDifferent);
    memcpy(pibValue.value, gmkDifferent, sizeof(gmkDifferent));
    eStatus = MAC_WRP_SetRequestSync(app_mac_testData.macWrpHandle, eAttribute, u16Index, &pibValue);
#elif defined (MAC_GR3_012) && defined(RECEIVER)
    /* No Key set */
#else
    /* Key Table */
    eAttribute = MAC_WRP_PIB_KEY_TABLE;
    u16Index = keyIndex;
    pibValue.length = sizeof(gmk);
    memcpy(pibValue.value, gmk, sizeof(gmk));
    eStatus = MAC_WRP_SetRequestSync(app_mac_testData.macWrpHandle, eAttribute, u16Index, &pibValue);
#endif
    /* PAN Id */
    eAttribute = MAC_WRP_PIB_PAN_ID;
    u16Index = 0;
    pibValue.length = sizeof(panId);
    memcpy(pibValue.value, &panId, sizeof(panId));
    eStatus = MAC_WRP_SetRequestSync(app_mac_testData.macWrpHandle, eAttribute, u16Index, &pibValue);
    /* Short Address */
    eAttribute = MAC_WRP_PIB_SHORT_ADDRESS;
    u16Index = 0;
    pibValue.length = sizeof(shortAddress);
    memcpy(pibValue.value, &shortAddress, sizeof(shortAddress));
    eStatus = MAC_WRP_SetRequestSync(app_mac_testData.macWrpHandle, eAttribute, u16Index, &pibValue);
    /* DSN RF */
    #ifdef MAC_TEST_RF
    eAttribute = MAC_WRP_PIB_DSN_RF;
    #else
    eAttribute = MAC_WRP_PIB_DSN;
    #endif
    u16Index = 0;
    pibValue.length = sizeof(dsnRF);
    memcpy(pibValue.value, &dsnRF, sizeof(dsnRF));
    eStatus = MAC_WRP_SetRequestSync(app_mac_testData.macWrpHandle, eAttribute, u16Index, &pibValue);
#if defined(MAC_GR2_001) || defined(MAC_GR2_002) || defined(MAC_GR2_003) || defined(MAC_GR3_011) || defined(MAC_GR3_012)
    /* Frame Counter RF */
    #ifdef MAC_TEST_RF
    eAttribute = MAC_WRP_PIB_FRAME_COUNTER_RF;
    #else
    eAttribute = MAC_WRP_PIB_FRAME_COUNTER;
    #endif
    u16Index = 0;
    pibValue.length = sizeof(frameCounterRF);
    memcpy(pibValue.value, &frameCounterRF, sizeof(frameCounterRF));
    eStatus = MAC_WRP_SetRequestSync(app_mac_testData.macWrpHandle, eAttribute, u16Index, &pibValue);
#endif
#if defined(MAC_GR1_005)
    /* POS Entry RF */
    #ifdef MAC_TEST_RF
    eAttribute = MAC_WRP_PIB_POS_TABLE_RF;
    u16Index = 0;
    pibValue.length = sizeof(posEntryRF);
    memcpy(pibValue.value, &posEntryRF, sizeof(posEntryRF));
    eStatus = MAC_WRP_SetRequestSync(app_mac_testData.macWrpHandle, eAttribute, u16Index, &pibValue);
    #else
    eAttribute = MAC_WRP_PIB_POS_TABLE;
    u16Index = 0;
    pibValue.length = sizeof(posEntry);
    memcpy(pibValue.value, &posEntry, sizeof(posEntry));
    eStatus = MAC_WRP_SetRequestSync(app_mac_testData.macWrpHandle, eAttribute, u16Index, &pibValue);
    #endif
#endif
#if defined(MAC_GR3_010) && defined(SENDER)
    #ifdef MAC_TEST_RF
    /* Duty Cyle period */
    eAttribute = MAC_WRP_PIB_DUTY_CYCLE_PERIOD_RF;
    u16Index = 0;
    pibValue.length = sizeof(dutyCyclePeriod);
    memcpy(pibValue.value, &dutyCyclePeriod, sizeof(dutyCyclePeriod));
    eStatus = MAC_WRP_SetRequestSync(app_mac_testData.macWrpHandle, eAttribute, u16Index, &pibValue);
    /* Duty Cyle limit */
    eAttribute = MAC_WRP_PIB_DUTY_CYCLE_LIMIT_RF;
    u16Index = 0;
    pibValue.length = sizeof(dutyCycleLimit);
    memcpy(pibValue.value, &dutyCycleLimit, sizeof(dutyCycleLimit));
    eStatus = MAC_WRP_SetRequestSync(app_mac_testData.macWrpHandle, eAttribute, u16Index, &pibValue);
    #endif
#endif
#endif //defined(MAC_GR3_002) || defined(MAC_GR3_003)

    if (eStatus == MAC_WRP_STATUS_SUCCESS) {
#ifndef MAC_TEST_IOT
        /* Do not scan, just send the frames */
        SYS_TIME_DelayMS(DELAY_BETWEEN_DATA_REQUESTS, &tmr);
        app_mac_testData.state = APP_MAC_TEST_STATE_DATA_REQUEST_RF;
        return;
#endif
        /* Go to next step */
#if defined(SENDER)
#   if defined(MAC_GR1_001) || defined(MAC_GR1_004) || defined(MAC_GR1_005) \
        || defined(MAC_GR2_001) || defined(MAC_GR2_002) || defined(MAC_GR2_003) \
        || defined(MAC_GR3_001) || defined(MAC_GR3_005) || defined(MAC_GR3_006) \
        || defined(MAC_GR3_007) || defined(MAC_GR3_008) || defined(MAC_GR3_009) \
        || defined(MAC_GR3_010) || defined(MAC_GR3_011) || defined(MAC_GR3_012)
    SYS_TIME_DelayMS(DELAY_BETWEEN_DATA_REQUESTS, &tmr);
    #ifdef MAC_TEST_RF
        app_mac_testData.state = APP_MAC_TEST_STATE_DATA_REQUEST_RF;
    #else
        app_mac_testData.state = APP_MAC_TEST_STATE_DATA_REQUEST;
    #endif
#   elif defined(MAC_GR1_006) || defined(MAC_GR3_002) || defined(MAC_GR3_003)
        
    #ifdef MAC_TEST_RF
        app_mac_testData.state = APP_MAC_TEST_STATE_SCAN_REQUEST_RF;
    #else
        app_mac_testData.state = APP_MAC_TEST_STATE_SCAN_REQUEST;
    #endif
#   else
    #ifdef MAC_TEST_RF
        app_mac_testData.state = APP_MAC_TEST_STATE_GET_PARAMS_RF;
    #else
        app_mac_testData.state = APP_MAC_TEST_STATE_GET_PARAMS;
    #endif
#   endif
#else
    #ifdef MAC_TEST_RF
        app_mac_testData.state = APP_MAC_TEST_STATE_GET_PARAMS_RF;
    #else
        app_mac_testData.state = APP_MAC_TEST_STATE_GET_PARAMS;
    #endif
#endif
    }
    else {
        /* Go to Set Params again */
    #ifdef MAC_TEST_RF
        app_mac_testData.state = APP_MAC_TEST_STATE_SET_PARAMS_RF;
    #else
        app_mac_testData.state = APP_MAC_TEST_STATE_SET_PARAMS;
    #endif
    }
    (void)gmk;
    (void)dsnRF;
}

static void APP_MAC_TEST_GetParams(void)
{
    MAC_WRP_PIB_VALUE pibValue;
    uint16_t value;

    /* PAN Id */
    if (MAC_WRP_GetRequestSync(app_mac_testData.macWrpHandle, MAC_WRP_PIB_PAN_ID , 0, &pibValue) != MAC_WRP_STATUS_SUCCESS) {
        /* Get parameter again */
#ifdef MAC_TEST_RF
        app_mac_testData.state = APP_MAC_TEST_STATE_GET_PARAMS_RF;
#else
        app_mac_testData.state = APP_MAC_TEST_STATE_GET_PARAMS;
#endif
    }
    else {
        /* Check PanId */
        memcpy(&value, pibValue.value, sizeof(uint16_t));
        if ((value == panId) && (pibValue.length == sizeof(uint16_t))) {
            /* OK */
            /* Short Address */
            if (MAC_WRP_GetRequestSync(app_mac_testData.macWrpHandle, MAC_WRP_PIB_SHORT_ADDRESS , 0, &pibValue) != MAC_WRP_STATUS_SUCCESS) {
                /* Get parameter again */
#ifdef MAC_TEST_RF
                app_mac_testData.state = APP_MAC_TEST_STATE_GET_PARAMS_RF;
#else
                app_mac_testData.state = APP_MAC_TEST_STATE_GET_PARAMS;
#endif
            }
            else {
                memcpy(&value, pibValue.value, sizeof(uint16_t));
                if ((value == shortAddress) && (pibValue.length == sizeof(uint16_t))) {
                    /* OK. Go to next step */
#ifdef MAC_TEST_RF
    #ifdef SENDER
                    app_mac_testData.state = APP_MAC_TEST_STATE_SCAN_REQUEST_RF;
    #else
                    app_mac_testData.state = APP_MAC_TEST_STATE_WAIT_FOR_DATA_INDICATION_RF;
    #endif
#else
    #ifdef SENDER
                    app_mac_testData.state = APP_MAC_TEST_STATE_SCAN_REQUEST;
    #else
                    app_mac_testData.state = APP_MAC_TEST_STATE_WAIT_FOR_DATA_INDICATION;
    #endif
#endif
                }
                else {
                    /* Get parameter again */
#ifdef MAC_TEST_RF
                    app_mac_testData.state = APP_MAC_TEST_STATE_GET_PARAMS_RF;
#else
                    app_mac_testData.state = APP_MAC_TEST_STATE_GET_PARAMS;
#endif
                }
            }
        }
        else {
            /* Get parameter again */
#ifdef MAC_TEST_RF
            app_mac_testData.state = APP_MAC_TEST_STATE_GET_PARAMS_RF;
#else
            app_mac_testData.state = APP_MAC_TEST_STATE_GET_PARAMS;
#endif
        }
    }
}

static void APP_MAC_TEST_ScanRequest(void)
{
    MAC_WRP_SCAN_REQUEST_PARAMS scanReq;
    
#if defined(MAC_GR1_006) || defined(MAC_GR3_002) || defined(MAC_GR3_003)
    scanReq.scanDuration = scanDuration;
#else
    scanReq.scanDuration = 20;
#endif
    SYS_CONSOLE_Print(myConsoleHandle, "\r\n\r\n-- Scan Request with duration %u seconds --\r\n", scanReq.scanDuration);

    MAC_WRP_ScanRequest(app_mac_testData.macWrpHandle, &scanReq);
}

static void APP_MAC_TEST_DataRequest(void)
{
    MAC_WRP_DATA_REQUEST_PARAMS dataReq;

    /* Set params and call request */
    dataReq.destAddress.addressMode = destAddressMode;
    if (destAddressMode == MAC_WRP_ADDRESS_MODE_SHORT) {
        dataReq.destAddress.shortAddress = destAddress;
    }
    else {
        memcpy(dataReq.destAddress.extendedAddress.address, destExtAddress, sizeof(destExtAddress));
    }
#ifdef MAC_TEST_RF
    dataReq.mediaType = MAC_WRP_MEDIA_TYPE_REQ_RF_NO_BACKUP;
#else
    dataReq.mediaType = MAC_WRP_MEDIA_TYPE_REQ_PLC_NO_BACKUP;
#endif
    dataReq.qualityOfService = qos;
    dataReq.securityLevel = secLevel;
    dataReq.srcAddressMode = srcAddressMode;
#if defined(MAC_GR3_008) && defined(SENDER)
    dataReq.destPanId = destPanId;
#else
    dataReq.destPanId = panId;
#endif
    dataReq.keyIndex = keyIndex;
#ifdef MAC_TEST_IOT
    dataReq.msduHandle = msduHandle;
#else
    dataReq.msduHandle = msduHandle++;
#endif
    dataReq.txOptions = reqACK;
    dataReq.msduLength = msduLength;
    dataReq.msdu = msdu;

    MAC_WRP_DataRequest(app_mac_testData.macWrpHandle, &dataReq);
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_MAC_TEST_Initialize ( void )

  Remarks:
    See prototype in app_mac_test.h.
 */

void APP_MAC_TEST_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    app_mac_testData.state = APP_MAC_TEST_STATE_INIT;

    /* Get Console Handle */
    myConsoleHandle = SYS_CONSOLE_HandleGet(SYS_CONSOLE_INDEX_0);
}


/******************************************************************************
  Function:
    void APP_MAC_TEST_Tasks ( void )

  Remarks:
    See prototype in app_mac_test.h.
 */

void APP_MAC_TEST_Tasks(void)
{
    /* Clear watchdog */
    WDT_Clear();
    
    /* Check the application's current state. */
    switch (app_mac_testData.state)
    {
        /* Application's initial state. */
        case APP_MAC_TEST_STATE_INIT:
        {
            app_mac_testData.state = APP_MAC_TEST_STATE_INITIALIZE_MAC;

            /* Open MAC Wrapper instance */
            app_mac_testData.macWrpHandle = MAC_WRP_Open(G3_MAC_WRP_INDEX_0);

            /* Set MAC Wrapper Init data */
            app_mac_testData.macWrpInit.plcBand = MAC_WRP_BAND_CENELEC_A;
            app_mac_testData.macWrpInit.macWrpHandlers.dataConfirmCallback = APP_MAC_TEST_DataConfirm;
            app_mac_testData.macWrpInit.macWrpHandlers.dataIndicationCallback = APP_MAC_TEST_DataIndication;
            app_mac_testData.macWrpInit.macWrpHandlers.resetConfirmCallback = APP_MAC_TEST_ResetConfirm;
            app_mac_testData.macWrpInit.macWrpHandlers.beaconNotifyIndicationCallback = APP_MAC_TEST_BeaconIndication;
            app_mac_testData.macWrpInit.macWrpHandlers.scanConfirmCallback = APP_MAC_TEST_ScanConfirm;
            app_mac_testData.macWrpInit.macWrpHandlers.startConfirmCallback = APP_MAC_TEST_StartConfirm;
            app_mac_testData.macWrpInit.macWrpHandlers.commStatusIndicationCallback = APP_MAC_TEST_CommStatusIndication;
            app_mac_testData.macWrpInit.macWrpHandlers.snifferIndicationCallback = NULL;
            break;
        }

        case APP_MAC_TEST_STATE_INITIALIZE_MAC:
        {
            /* Initialize MAC layer */
            MAC_WRP_Init(app_mac_testData.macWrpHandle, &app_mac_testData.macWrpInit);
            /* Go to next state */
            app_mac_testData.state = APP_MAC_TEST_STATE_WAIT_MAC_READY;
            break;
        }

        case APP_MAC_TEST_STATE_WAIT_MAC_READY:
        {
            /* Check MAC Status */
            if (MAC_WRP_Status() == SYS_STATUS_READY) {
                /* Set timer and go to next state */
                SYS_TIME_DelayMS(APP_INITIAL_DELAY_MS, &tmr);
                app_mac_testData.state = APP_MAC_TEST_STATE_INITIAL_DELAY;
            }
            break;
        }

        case APP_MAC_TEST_STATE_INITIAL_DELAY:
        {
            /* Check delay expired */
            if (SYS_TIME_DelayIsComplete(tmr)) {
                /* Go to next state */
                #ifdef MAC_TEST_RF
                    app_mac_testData.state = APP_MAC_TEST_STATE_SET_PARAMS_RF;
                #else
                    app_mac_testData.state = APP_MAC_TEST_STATE_SET_PARAMS;
                #endif
            }
            break;
        }

        case APP_MAC_TEST_STATE_SET_PARAMS:
        {
            /* Go to next test step */
            app_mac_testData.state = APP_MAC_TEST_STATE_WAIT_FOR_SET_CONFIRM;
            APP_MAC_TEST_SetParams();
            break;
        }

        case APP_MAC_TEST_STATE_WAIT_FOR_SET_CONFIRM:
        {
            /* Do nothing. State will change on confirm function */
            break;
        }

        case APP_MAC_TEST_STATE_GET_PARAMS:
        {
            /* Go to next test step */
            app_mac_testData.state = APP_MAC_TEST_STATE_WAIT_FOR_GET_CONFIRM;
            APP_MAC_TEST_GetParams();
            break;
        }

        case APP_MAC_TEST_STATE_WAIT_FOR_GET_CONFIRM:
        {
            /* Do nothing. State will change on confirm function */
            break;
        }

        case APP_MAC_TEST_STATE_SCAN_REQUEST:
        {
            /* Go to next state */
            app_mac_testData.state = APP_MAC_TEST_STATE_WAIT_FOR_BCN_INDICATION;
            APP_MAC_TEST_ScanRequest();
            break;
        }

        case APP_MAC_TEST_STATE_WAIT_FOR_BCN_INDICATION:
        {
            /* Do nothing. State will change on indication function */
            break;
        }

        case APP_MAC_TEST_STATE_WAIT_FOR_SCAN_CONFIRM:
        {
            /* Do nothing. State will change on confirm function */
            break;
        }

        case APP_MAC_TEST_STATE_DATA_REQUEST:
        {
            if (SYS_TIME_DelayIsComplete(tmr)) {
                /* Go to next state */
                app_mac_testData.state = APP_MAC_TEST_STATE_WAIT_FOR_DATA_CONFIRM;
                APP_MAC_TEST_DataRequest();
            }
            break;
        }

        case APP_MAC_TEST_STATE_WAIT_FOR_DATA_CONFIRM:
        {
            /* Do nothing. State will change on confirm function */
            break;
        }

        case APP_MAC_TEST_STATE_WAIT_FOR_DATA_INDICATION:
        {
            /* Do nothing. State will change on indication function */
            break;
        }

        case APP_MAC_TEST_STATE_GET_POS_TABLE:
        {
            /* Go to next state */
            app_mac_testData.state = APP_MAC_TEST_STATE_STOP;
            GetAndPrintPOSTable();
            break;
        }

        case APP_MAC_TEST_STATE_STOP:
        {
            /* Do nothing */
            break;
        }

        case APP_MAC_TEST_STATE_ERROR:
        {
            /* Do nothing */
            break;
        }

        case APP_MAC_TEST_STATE_SET_PARAMS_RF:
        {
            /* Go to next test step */
            app_mac_testData.state = APP_MAC_TEST_STATE_WAIT_FOR_SET_CONFIRM_RF;
            APP_MAC_TEST_SetParams();
            break;
        }

        case APP_MAC_TEST_STATE_WAIT_FOR_SET_CONFIRM_RF:
        {
            /* Do nothing. State will change on confirm function */
            break;
        }

        case APP_MAC_TEST_STATE_GET_PARAMS_RF:
        {
            /* Go to next test step */
            app_mac_testData.state = APP_MAC_TEST_STATE_WAIT_FOR_GET_CONFIRM_RF;
            APP_MAC_TEST_GetParams();
            break;
        }

        case APP_MAC_TEST_STATE_WAIT_FOR_GET_CONFIRM_RF:
        {
            /* Do nothing. State will change on confirm function */
            break;
        }

        case APP_MAC_TEST_STATE_SCAN_REQUEST_RF:
        {
            /* Go to next state */
            app_mac_testData.state = APP_MAC_TEST_STATE_WAIT_FOR_BCN_INDICATION_RF;
            APP_MAC_TEST_ScanRequest();
            break;
        }

        case APP_MAC_TEST_STATE_WAIT_FOR_BCN_INDICATION_RF:
        {
            /* Do nothing. State will change on indication function */
            break;
        }

        case APP_MAC_TEST_STATE_WAIT_FOR_SCAN_CONFIRM_RF:
        {
            /* Do nothing. State will change on confirm function */
            break;
        }

        case APP_MAC_TEST_STATE_DATA_REQUEST_RF:
        {
            if (SYS_TIME_DelayIsComplete(tmr)) {
                /* Go to next state */
                app_mac_testData.state = APP_MAC_TEST_STATE_WAIT_FOR_DATA_CONFIRM_RF;
                APP_MAC_TEST_DataRequest();
            }
            break;
        }

        case APP_MAC_TEST_STATE_WAIT_FOR_DATA_CONFIRM_RF:
        {
            /* Do nothing. State will change on confirm function */
            break;
        }

        case APP_MAC_TEST_STATE_WAIT_FOR_DATA_INDICATION_RF:
        {
            /* Do nothing. State will change on indication function */
            break;
        }

        case APP_MAC_TEST_STATE_GET_POS_TABLE_RF:
        {
            /* Go to next state */
            app_mac_testData.state = APP_MAC_TEST_STATE_STOP_RF;
            GetAndPrintPOSTableRF();
            break;
        }

        case APP_MAC_TEST_STATE_STOP_RF:
        {
            /* Do nothing */
            break;
        }

        case APP_MAC_TEST_STATE_ERROR_RF:
        {
            /* Do nothing */
            break;
        }

        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}


/*******************************************************************************
 End of File
 */
