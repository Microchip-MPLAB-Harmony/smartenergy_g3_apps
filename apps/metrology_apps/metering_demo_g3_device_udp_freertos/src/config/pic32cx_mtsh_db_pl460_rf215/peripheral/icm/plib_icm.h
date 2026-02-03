/*******************************************************************************
  Interface definition of ICM PLIB.

  Company:
    Microchip Technology Inc.

  File Name:
    plib_icm.h

  Summary:
    Interface definition of the Integrity Check Monitor Plib (ICM).

  Description:
    This file defines the interface for the ICM Plib.
*******************************************************************************/

/*******************************************************************************
* Copyright (C) 2025 Microchip Technology Inc. and its subsidiaries.
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

#ifndef PLIB_ICM_H    // Guards against multiple inclusion
#define PLIB_ICM_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus // Provide C++ Compatibility
    extern "C" {
#endif


// *****************************************************************************
// *****************************************************************************
// Section: Interface
// *****************************************************************************
// *****************************************************************************

typedef enum {
	ICM_REGION_0 = 0U,
	ICM_REGION_1,
	ICM_REGION_2,
	ICM_REGION_3,
	ICM_REGION_MAX
}ICM_REGION_ID;
#define ICM_REGION_NUM       (ICM_REGION_1)

typedef enum {
	ICM_INTERRUPT_RHC = 0U,
	ICM_INTERRUPT_RDM,
	ICM_INTERRUPT_RBE,
	ICM_INTERRUPT_RWC,
	ICM_INTERRUPT_REC,
	ICM_INTERRUPT_RSU,
	ICM_INTERRUPT_URAD,
	ICM_INTERRUPT_MAX
}ICM_INTERRUPT_SOURCE;

typedef enum {
	ICM_SHA_1 = 0U,
	ICM_SHA_224 = 4U,
	ICM_SHA_256 = 1U,
}ICM_ALGO;

typedef enum {
	ICM_USER_ACCESS = 0U,
	ICM_PRIVILEDGE_ACCESS,
}ICM_ACCESS;

typedef struct ICM_SECONDARY_LIST
{
	uint32_t startAddress;
	uint32_t reserved;
	uint32_t transferSize;
	struct ICM_SECONDARY_LIST *nextAddress;
}ICM_SECONDARY_LIST;

typedef struct 
{
	unsigned int compareMode : 1;
	unsigned int wrap : 1;
	unsigned int endMonitor : 1;
	unsigned int reserved1 : 1;
	unsigned int regHashIntDis : 1;
	unsigned int mismatchIntDis : 1;
	unsigned int busErrIntDis : 1;
	unsigned int wrapConIntDis : 1;
	unsigned int endBitConIntDis : 1;
	unsigned int statusUpdConIntDis : 1;
	unsigned int procDelay : 1;
	unsigned int reserved2 : 1;
	unsigned int algo : 3;
	unsigned int reserved3 : 9;
	unsigned int memBusProtection : 6;
	unsigned int reserved4 : 2;
}ICM_REGION_BITFIELD;

typedef union {
	uint32_t value;
	ICM_REGION_BITFIELD bitfield;
}ICM_REGION_CONFIG;

typedef struct 
{
	uint32_t startAddress;
	ICM_REGION_CONFIG config;
	uint32_t transferSize;
	ICM_SECONDARY_LIST *secondaryList;
}ICM_REGION_DESCRIPTOR;

typedef struct {
	ICM_ACCESS descriptorAccess;
	ICM_ACCESS hashAccess;
	ICM_ALGO userAlgo;
	bool userHash;
	bool dualInputBuffer;
	bool monitorMode;
	uint8_t burdenControl;
	bool disableSecondaryList;
	bool disableEndOfMonitoring;
	bool disableWriteBack;
}ICM_CONFIG;

typedef void (*ICM_CALLBACK)(ICM_REGION_ID regionId);

/***************************** ICM API *******************************/
void ICM_Initialize (void);
void ICM_Reset (void);
uint32_t ICM_GetStatus(void);
void ICM_Enable (void);
void ICM_Disable (void);
void ICM_GetConfiguration (ICM_CONFIG *config);
void ICM_SetConfiguration (ICM_CONFIG *config);
void ICM_SetMonitorMode ( bool enable , uint8_t bbc);
void ICM_EnableRegionMonitor ( ICM_REGION_ID regionId );
void ICM_DisableRegionMonitor ( ICM_REGION_ID regionId );
ICM_REGION_DESCRIPTOR * ICM_GetRegionDescriptor(ICM_REGION_ID regionId);
void ICM_SetRegionDescriptor (ICM_REGION_ID regionId, ICM_REGION_DESCRIPTOR * pRegionDescriptor);
void ICM_SetRegionDescriptorData (ICM_REGION_ID regionId, uint32_t * pData, size_t bytes);
uint32_t ICM_GetTransferSize (size_t bytes);
void ICM_SetHashAreaAddress (uint32_t address);
void ICM_SetUserInitialHashValue (uint32_t *pValue);
void ICM_EnableInterrupt (ICM_INTERRUPT_SOURCE source, ICM_REGION_ID regionId);
void ICM_DisableInterrupt (ICM_INTERRUPT_SOURCE source, ICM_REGION_ID regionId);
uint32_t ICM_GetIStatus (void);
void ICM_CallbackRegister (ICM_INTERRUPT_SOURCE source, ICM_CALLBACK callback);

#ifdef __cplusplus // Provide C++ Compatibility
 }
#endif

#endif
