/*******************************************************************************
  CRC service used by PLC components Implementation

  Company:
    Microchip Technology Inc.

  File Name:
    srv_pcrc.c

  Summary:
    CRC service used by PLC components Implementation.

  Description:
    The CRC wrapper provides a simple interface to manage the CRC needs
    for PLC components. This file implements the CRC core interface routines
    for PLC.
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

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "string.h"
#include "srv_pcrc.h"

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Data
// *****************************************************************************
// *****************************************************************************

/* This is the internal SNA to use in PRIME CRC service. */
static uint8_t pCrcSna8 = 0;
static uint32_t pCrcSna32 = 0;

/* PRIME v1.4: The CRC shall be calculated over the whole BPDU, including */
/* constant CRC field */
static uint8_t pCrcConstBcn14[PCRC_CONST_BCN_PRIME_1_4_SIZE] =
    {0x00, 0x01, 0x04, 0x00};

/* Table used to get CRC32 value */
static const uint32_t pCrcTable32[256] = {
    0x00000000UL, 0x04C11DB7UL, 0x09823B6EUL, 0x0D4326D9UL,
    0x130476DCUL, 0x17C56B6BUL, 0x1A864DB2UL, 0x1E475005UL,
    0x2608EDB8UL, 0x22C9F00FUL, 0x2F8AD6D6UL, 0x2B4BCB61UL,
    0x350C9B64UL, 0x31CD86D3UL, 0x3C8EA00AUL, 0x384FBDBDUL,
    0x4C11DB70UL, 0x48D0C6C7UL, 0x4593E01EUL, 0x4152FDA9UL,
    0x5F15ADACUL, 0x5BD4B01BUL, 0x569796C2UL, 0x52568B75UL,
    0x6A1936C8UL, 0x6ED82B7FUL, 0x639B0DA6UL, 0x675A1011UL,
    0x791D4014UL, 0x7DDC5DA3UL, 0x709F7B7AUL, 0x745E66CDUL,
    0x9823B6E0UL, 0x9CE2AB57UL, 0x91A18D8EUL, 0x95609039UL,
    0x8B27C03CUL, 0x8FE6DD8BUL, 0x82A5FB52UL, 0x8664E6E5UL,
    0xBE2B5B58UL, 0xBAEA46EFUL, 0xB7A96036UL, 0xB3687D81UL,
    0xAD2F2D84UL, 0xA9EE3033UL, 0xA4AD16EAUL, 0xA06C0B5DUL,
    0xD4326D90UL, 0xD0F37027UL, 0xDDB056FEUL, 0xD9714B49UL,
    0xC7361B4CUL, 0xC3F706FBUL, 0xCEB42022UL, 0xCA753D95UL,
    0xF23A8028UL, 0xF6FB9D9FUL, 0xFBB8BB46UL, 0xFF79A6F1UL,
    0xE13EF6F4UL, 0xE5FFEB43UL, 0xE8BCCD9AUL, 0xEC7DD02DUL,
    0x34867077UL, 0x30476DC0UL, 0x3D044B19UL, 0x39C556AEUL,
    0x278206ABUL, 0x23431B1CUL, 0x2E003DC5UL, 0x2AC12072UL,
    0x128E9DCFUL, 0x164F8078UL, 0x1B0CA6A1UL, 0x1FCDBB16UL,
    0x018AEB13UL, 0x054BF6A4UL, 0x0808D07DUL, 0x0CC9CDCAUL,
    0x7897AB07UL, 0x7C56B6B0UL, 0x71159069UL, 0x75D48DDEUL,
    0x6B93DDDBUL, 0x6F52C06CUL, 0x6211E6B5UL, 0x66D0FB02UL,
    0x5E9F46BFUL, 0x5A5E5B08UL, 0x571D7DD1UL, 0x53DC6066UL,
    0x4D9B3063UL, 0x495A2DD4UL, 0x44190B0DUL, 0x40D816BAUL,
    0xACA5C697UL, 0xA864DB20UL, 0xA527FDF9UL, 0xA1E6E04EUL,
    0xBFA1B04BUL, 0xBB60ADFCUL, 0xB6238B25UL, 0xB2E29692UL,
    0x8AAD2B2FUL, 0x8E6C3698UL, 0x832F1041UL, 0x87EE0DF6UL,
    0x99A95DF3UL, 0x9D684044UL, 0x902B669DUL, 0x94EA7B2AUL,
    0xE0B41DE7UL, 0xE4750050UL, 0xE9362689UL, 0xEDF73B3EUL,
    0xF3B06B3BUL, 0xF771768CUL, 0xFA325055UL, 0xFEF34DE2UL,
    0xC6BCF05FUL, 0xC27DEDE8UL, 0xCF3ECB31UL, 0xCBFFD686UL,
    0xD5B88683UL, 0xD1799B34UL, 0xDC3ABDEDUL, 0xD8FBA05AUL,
    0x690CE0EEUL, 0x6DCDFD59UL, 0x608EDB80UL, 0x644FC637UL,
    0x7A089632UL, 0x7EC98B85UL, 0x738AAD5CUL, 0x774BB0EBUL,
    0x4F040D56UL, 0x4BC510E1UL, 0x46863638UL, 0x42472B8FUL,
    0x5C007B8AUL, 0x58C1663DUL, 0x558240E4UL, 0x51435D53UL,
    0x251D3B9EUL, 0x21DC2629UL, 0x2C9F00F0UL, 0x285E1D47UL,
    0x36194D42UL, 0x32D850F5UL, 0x3F9B762CUL, 0x3B5A6B9BUL,
    0x0315D626UL, 0x07D4CB91UL, 0x0A97ED48UL, 0x0E56F0FFUL,
    0x1011A0FAUL, 0x14D0BD4DUL, 0x19939B94UL, 0x1D528623UL,
    0xF12F560EUL, 0xF5EE4BB9UL, 0xF8AD6D60UL, 0xFC6C70D7UL,
    0xE22B20D2UL, 0xE6EA3D65UL, 0xEBA91BBCUL, 0xEF68060BUL,
    0xD727BBB6UL, 0xD3E6A601UL, 0xDEA580D8UL, 0xDA649D6FUL,
    0xC423CD6AUL, 0xC0E2D0DDUL, 0xCDA1F604UL, 0xC960EBB3UL,
    0xBD3E8D7EUL, 0xB9FF90C9UL, 0xB4BCB610UL, 0xB07DABA7UL,
    0xAE3AFBA2UL, 0xAAFBE615UL, 0xA7B8C0CCUL, 0xA379DD7BUL,
    0x9B3660C6UL, 0x9FF77D71UL, 0x92B45BA8UL, 0x9675461FUL,
    0x8832161AUL, 0x8CF30BADUL, 0x81B02D74UL, 0x857130C3UL,
    0x5D8A9099UL, 0x594B8D2EUL, 0x5408ABF7UL, 0x50C9B640UL,
    0x4E8EE645UL, 0x4A4FFBF2UL, 0x470CDD2BUL, 0x43CDC09CUL,
    0x7B827D21UL, 0x7F436096UL, 0x7200464FUL, 0x76C15BF8UL,
    0x68860BFDUL, 0x6C47164AUL, 0x61043093UL, 0x65C52D24UL,
    0x119B4BE9UL, 0x155A565EUL, 0x18197087UL, 0x1CD86D30UL,
    0x029F3D35UL, 0x065E2082UL, 0x0B1D065BUL, 0x0FDC1BECUL,
    0x3793A651UL, 0x3352BBE6UL, 0x3E119D3FUL, 0x3AD08088UL,
    0x2497D08DUL, 0x2056CD3AUL, 0x2D15EBE3UL, 0x29D4F654UL,
    0xC5A92679UL, 0xC1683BCEUL, 0xCC2B1D17UL, 0xC8EA00A0UL,
    0xD6AD50A5UL, 0xD26C4D12UL, 0xDF2F6BCBUL, 0xDBEE767CUL,
    0xE3A1CBC1UL, 0xE760D676UL, 0xEA23F0AFUL, 0xEEE2ED18UL,
    0xF0A5BD1DUL, 0xF464A0AAUL, 0xF9278673UL, 0xFDE69BC4UL,
    0x89B8FD09UL, 0x8D79E0BEUL, 0x803AC667UL, 0x84FBDBD0UL,
    0x9ABC8BD5UL, 0x9E7D9662UL, 0x933EB0BBUL, 0x97FFAD0CUL,
    0xAFB010B1UL, 0xAB710D06UL, 0xA6322BDFUL, 0xA2F33668UL,
    0xBCB4666DUL, 0xB8757BDAUL, 0xB5365D03UL, 0xB1F740B4UL,
};

/* Table used to get CRC16 value */
static const uint16_t pCrcTable16[256] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
    0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
    0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
    0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
    0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
    0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
    0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
    0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
    0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
    0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
    0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
    0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
    0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
    0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
    0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
    0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
    0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
    0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
    0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
    0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
    0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
    0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};

/* Table used to get CRC8 value */
static const uint8_t pCrcTable8[256] = {
    0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15, 0x38,
    0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D, 0x70, 0x77,
    0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65, 0x48, 0x4F, 0x46,
    0x41, 0x54, 0x53, 0x5A, 0x5D, 0xE0, 0xE7, 0xEE, 0xE9,
    0xFC, 0xFB, 0xF2, 0xF5, 0xD8, 0xDF, 0xD6, 0xD1, 0xC4,
    0xC3, 0xCA, 0xCD, 0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B,
    0x82, 0x85, 0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA,
    0xBD, 0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2,
    0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA, 0xB7,
    0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2, 0x8F, 0x88,
    0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A, 0x27, 0x20, 0x29,
    0x2E, 0x3B, 0x3C, 0x35, 0x32, 0x1F, 0x18, 0x11, 0x16,
    0x03, 0x04, 0x0D, 0x0A, 0x57, 0x50, 0x59, 0x5E, 0x4B,
    0x4C, 0x45, 0x42, 0x6F, 0x68, 0x61, 0x66, 0x73, 0x74,
    0x7D, 0x7A, 0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B,
    0x9C, 0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
    0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC, 0xC1,
    0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4, 0x69, 0x6E,
    0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C, 0x51, 0x56, 0x5F,
    0x58, 0x4D, 0x4A, 0x43, 0x44, 0x19, 0x1E, 0x17, 0x10,
    0x05, 0x02, 0x0B, 0x0C, 0x21, 0x26, 0x2F, 0x28, 0x3D,
    0x3A, 0x33, 0x34, 0x4E, 0x49, 0x40, 0x47, 0x52, 0x55,
    0x5C, 0x5B, 0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64,
    0x63, 0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B,
    0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13, 0xAE,
    0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB, 0x96, 0x91,
    0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83, 0xDE, 0xD9, 0xD0,
    0xD7, 0xC2, 0xC5, 0xCC, 0xCB, 0xE6, 0xE1, 0xE8, 0xEF,
    0xFA, 0xFD, 0xF4, 0xF3
};

// *****************************************************************************
// *****************************************************************************
// Section: File scope functions
// *****************************************************************************
// *****************************************************************************

static uint32_t lSRV_PCRC_Get32(uint8_t *pData, size_t length,
        uint32_t crcInitValue)
{
    uint32_t crc;
    uint8_t indexTable;

    crc = crcInitValue;
    while ((length--) > 0U)
    {
        indexTable = (uint8_t)(crc >> 24) ^ *pData++;
        crc = (crc << 8) ^ pCrcTable32[indexTable];
    }

    return crc;
}

static uint16_t lSRV_PCRC_Get16(uint8_t *pData, size_t length,
        uint32_t crcInitValue)
{
    uint16_t crc;
    uint8_t indexTable;

    crc = (uint16_t)crcInitValue;
    while ((length--) > 0U)
    {
        indexTable = (uint8_t)(crc >> 8);
        crc = pCrcTable16[indexTable] ^ (crc << 8) ^ (uint16_t)*pData++;
    }

    return crc;
}

static uint8_t lSRV_PCRC_Get8(uint8_t *pData, size_t length,
        uint32_t crcInitValue)
{
    uint8_t crc;

    crc = (uint8_t)crcInitValue;
    while ((length--) > 0U)
    {
        crc = pCrcTable8[crc ^ *pData++];
    }

    return crc;
}

static uint8_t lSRV_PCRC_Get8ForUsi(uint8_t *pData, size_t length,
        uint32_t crcInitValue)
{
    uint8_t crc;

    crc = (uint8_t)crcInitValue;
    while ((length--) > 0U)
    {
        crc = crc ^ *pData++;
    }

    return crc;
}

// *****************************************************************************
// *****************************************************************************
// Section: CRC Service Interface Implementation
// *****************************************************************************
// *****************************************************************************

uint32_t SRV_PCRC_GetValue(uint8_t *pData, size_t length,
        PCRC_HEADER_TYPE hdrType, PCRC_CRC_TYPE crcType, uint32_t initValue)
{
    uint32_t crc32Val;

    switch(hdrType)
    {
        case PCRC_HT_GENERIC:
            switch (crcType)
            {
                case PCRC_CRC8:
                    crc32Val = (uint32_t)lSRV_PCRC_Get8(pData, length, initValue);
                    break;

                case PCRC_CRC16:
                    crc32Val = (uint32_t)lSRV_PCRC_Get16(pData, length, initValue);
                    break;

                case PCRC_CRC32:
                    crc32Val = (uint32_t)lSRV_PCRC_Get32(pData, length, initValue);
                    break;

                default:
                    crc32Val = PCRC_INVALID;
                    break;
            }
            break;

        case PCRC_HT_PRIME_GENERIC:
            switch (crcType)
            {
                case PCRC_CRC8:
                    crc32Val = (uint32_t)lSRV_PCRC_Get8(pData, length, pCrcSna8);
                    break;

                case PCRC_CRC16:
                    crc32Val = (uint32_t)lSRV_PCRC_Get16(pData, length, 0);
                    break;

                case PCRC_CRC32:
                    crc32Val = (uint32_t)lSRV_PCRC_Get32(pData, length, pCrcSna32);
                    break;

                default:
                    crc32Val = PCRC_INVALID;
                    break;
            }
            break;

        case PCRC_HT_PRIME_BEACON14:
            crc32Val = (uint32_t)lSRV_PCRC_Get32(pData, length, 0);
            crc32Val = (uint32_t)lSRV_PCRC_Get32(pCrcConstBcn14,
                                                 PCRC_CONST_BCN_PRIME_1_4_SIZE,
                                                 crc32Val);
            break;

        case PCRC_HT_USI:
            switch(crcType)
            {
                case PCRC_CRC8:
                    crc32Val = (uint32_t)lSRV_PCRC_Get8ForUsi(pData, length, initValue);
                    break;

                case PCRC_CRC16:
                    crc32Val = (uint32_t)lSRV_PCRC_Get16(pData, length, initValue);
                    break;

                case PCRC_CRC32:
                    crc32Val = (uint32_t)lSRV_PCRC_Get32(pData, length, initValue);
                    break;

                default:
                    crc32Val = PCRC_INVALID;
                    break;
            }
            break;

        default:
            crc32Val = PCRC_INVALID;
            break;
    }

    return crc32Val;

}

void SRV_PCRC_ConfigureSNA (uint8_t* sna)
{
    uint8_t pCrcSna[PCRC_SNA_SIZE] = {0};

    (void) memcpy(pCrcSna, sna, PCRC_SNA_SIZE);

    pCrcSna8  = (uint8_t)lSRV_PCRC_Get8(pCrcSna, PCRC_SNA_SIZE, 0);
	pCrcSna32 = (uint32_t)lSRV_PCRC_Get32(pCrcSna, PCRC_SNA_SIZE, 0);
}


/*******************************************************************************
 End of File
*/
