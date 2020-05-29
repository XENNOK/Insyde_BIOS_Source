//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/** @file
  Include file for record cache subclass data with Smbios protocol.

  Copyright (c) 2006 - 2011, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#ifndef _CACHE_H_
#define _CACHE_H_

#include "MpCommon.h"

//
// Bit field definitions for return registers of CPUID EAX = 4
//
// EAX
#define CPU_CACHE_TYPE_MASK                0x1F
#define CPU_CACHE_LEVEL_MASK               0xE0
#define CPU_CACHE_LEVEL_SHIFT              5
// EBX
#define CPU_CACHE_LINESIZE_MASK            0xFFF
#define CPU_CACHE_PARTITIONS_MASK          0x3FF000
#define CPU_CACHE_PARTITIONS_SHIFT         12
#define CPU_CACHE_WAYS_MASK                0xFFC00000
#define CPU_CACHE_WAYS_SHIFT               22

#define CPU_CACHE_L1        1
#define CPU_CACHE_L2        2
#define CPU_CACHE_L3        3
#define CPU_CACHE_L4        4
#define CPU_CACHE_LMAX      CPU_CACHE_L4

typedef struct {
  UINT8                         CacheLevel;
  UINT8                         CacheDescriptor;
  UINT16                        CacheSizeinKB;
  CACHE_ASSOCIATIVITY_DATA      Associativity;
  CACHE_TYPE_DATA               SystemCacheType;
} CPU_CACHE_CONVERTER;


typedef struct {
  UINT16                        CacheSizeinKB;
  CACHE_ASSOCIATIVITY_DATA      Associativity;
  CACHE_TYPE_DATA               SystemCacheType;
  //
  // Can extend the structure here.
  //
} CPU_CACHE_DATA;

//
// It is defined for SMBIOS_TABLE_TYPE7.CacheConfiguration.
//
typedef struct {
  UINT16    Level           :3;
  UINT16    Socketed        :1;
  UINT16    Reserved2       :1;
  UINT16    Location        :2;
  UINT16    Enable          :1;
  UINT16    OperationalMode :2;
  UINT16    Reserved1       :6;
} CPU_CACHE_CONFIGURATION_DATA;

/**
  Add Cache Information to Type 7 SMBIOS Record.
  
  @param[in]    ProcessorNumber     Processor number of specified processor.
  @param[out]   L1CacheHandle       Pointer to the handle of the L1 Cache SMBIOS record.
  @param[out]   L2CacheHandle       Pointer to the handle of the L2 Cache SMBIOS record.
  @param[out]   L3CacheHandle       Pointer to the handle of the L3 Cache SMBIOS record.

**/
VOID
AddSmbiosCacheTypeTable (
  IN UINTN              ProcessorNumber,
  OUT EFI_SMBIOS_HANDLE *L1CacheHandle,
  OUT EFI_SMBIOS_HANDLE *L2CacheHandle,
  OUT EFI_SMBIOS_HANDLE *L3CacheHandle
  );

#endif

