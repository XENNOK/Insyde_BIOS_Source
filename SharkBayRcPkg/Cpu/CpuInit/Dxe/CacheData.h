/** @file
   Header file for CPU Data File

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
**/

#ifndef _CACHE_DATA_H_
#define _CACHE_DATA_H_

#include "CpuInitDxe.h"

///
/// Platform-specific definitions
///
#define CPU_CACHE_DATA_MAXIMUM_LENGTH 0x100

typedef union {
  EFI_CACHE_DATA_RECORD *DataRecord;
  UINT8                 *Raw;
} CPU_CACHE_DATA_RECORD_BUFFER;

typedef struct {
  UINT8                         CacheLevel;
  UINT8                         CacheDescriptor;
  UINT16                        CacheSizeinKB;
  EFI_CACHE_ASSOCIATIVITY_DATA  Associativity;
  EFI_CACHE_TYPE_DATA           Type;
} EFI_CACHE_CONVERTER;

/**

  This function gets called with the processor number and will log all cache data to data hub
  pertaining to this processor.

  @param[in] CpuNumber - Processor Number
  @param[in] CacheInformation - Cache information get from cpuid instruction

  @retval EFI_OUT_OF_RESOURCES - Failed to allocate required POOL for record buffer.
  @retval EFI_SUCCESS - successful to update cache data

**/
EFI_STATUS
InitializeCacheData (
  IN  UINTN                            CpuNumber,
  IN  EFI_CPUID_REGISTER               *CacheInformation
  );

/**
  Log cache data into data hub

  @param[in] DataHub - Pointer to the DataHub protocol that will be updated
  @param[in] Buffer  - Data buffer which will be updated into DataHub
  @param[in] Size    - The size of data buffer

  @retval EFI_STATUS - status code for logging data into dat hub
**/
EFI_STATUS
LogCacheData (
  EFI_DATA_HUB_PROTOCOL      *DataHub,
  UINT8                      *Buffer,
  UINT32                     Size
  );

/**
  Log CPU data into data hub

  @param[in] DataHub - point to data hub that will be updated
  @param[in] Buffer  - the buffer which will be updated into data hub
  @param[in] Size    - size of the buffer

  @retval EFI_STATUS - status returned when updating Data hub
**/
EFI_STATUS
LogCpuData (
  EFI_DATA_HUB_PROTOCOL      *DataHub,
  UINT8                      *Buffer,
  UINT32                     Size
  );

#endif
