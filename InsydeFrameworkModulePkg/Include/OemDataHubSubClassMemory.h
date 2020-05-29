//;******************************************************************************
//;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _OEMDATAHUB_SUBCLASS_MEMORY_H_
#define _OEMDATAHUB_SUBCLASS_MEMORY_H_
#include EFI_GUID_DEFINITION (DataHubRecords)
#include "Numbers.h"
//
// SMBIOS Type 16
//
//
// The "Maximum Capacity"(0x07) field limitaion, in kilobytes, for SMBIOS Type 16
//
#define MEMORY_CAPACITY_SIZE_KB_2TB                        0x80000000

typedef struct {
  EFI_MEMORY_ARRAY_LOCATION_DATA        MemoryArrayLocationData;
  EFI_INTER_LINK_DATA                   MemoryDeviceErrorInfoLink;
  //
  // SMBIOS 2.7+
  //
  UINT64                                ExtendedMaximumMemoryCapacity;
} EFI_MEMORY_ARRAY_LOCATION_DATA_PLUS;

//
//SMBIOS Type 17
//
#define EFI_MEMORY_ARRAY_LINK_PLUS_RECORD_NUMBER           0x00000003

//
// The "Size"(0x0c) field limitaion, in kilobytes, for SMBIOS Type 17.
//
#define MEMORY_DEVICE_SIZE_MB_32GB                          0x8000

typedef enum {
  //
  // Type Detail:
  // ..
  // Bit13 Registered (Buffered)     --2.7+
  // Bit14 Unbuffered (Unregistered) --2.7+
  // Bit15 Reserved, set to 0.
  //
  EfiMemoryRegistered = BIT0,
  EfiMemoryUnbuffered = BIT1
} EFI_MEMORY_DEVICE_TYPE_DETAIL;

typedef struct {
  EFI_MEMORY_ARRAY_LINK       MemoryArrayLink;
  //
  // SMBIOS 2.7+
  //
  UINT16                      ConfiguredMemoryClockSpeed;
  EFI_INTER_LINK_DATA         MemoryDeviceErrorInfoLink;
  //
  // SMBIOS 2.8+
  //
  UINT16                      MinimumVoltage;
  UINT16                      MaximumVoltage;
  UINT16                      ConfiguredVoltage;
} EFI_MEMORY_ARRAY_LINK_PLUS;

//
// SMBIOS Type 6
//
#define EFI_MEMORY_MODULE_INFORMATION_DATA_RECORD_NUMBER       0x00000024

typedef struct {
  UINT16          Other          :1;
  UINT16          Unknown        :1;
  UINT16          Standard       :1;
  UINT16          FastPageMode   :1;
  UINT16          EDO            :1;
  UINT16          Parity         :1;
  UINT16          ECC            :1;
  UINT16          SIMM           :1;
  UINT16          DIMM           :1;
  UINT16          BurstEDO       :1;
  UINT16          SDRAM          :1;
  UINT16	        Reserved       :5;
} EFI_MEMORY_MODULE_TYPE;

typedef struct {
  STRING_REF                            SocketDesignation;
  UINT8                                 BankConnections;
  UINT8                                 CurrentSpeed;
  EFI_MEMORY_MODULE_TYPE                CurrentMemoryType;
  UINT8                                 InstalledSize;
  UINT8                                 EnabledSize;
  UINT8                                 ErrorStatus;
} EFI_MEMORY_MODULE_INFORMATION_DATA;

typedef union _EFI_MEMORY_SUBCLASS_RECORDS_PLUS {
  EFI_MEMORY_ARRAY_LOCATION_DATA_PLUS           ArrayLocationData;      // SMBIOS Type 16
  EFI_MEMORY_ARRAY_LINK_PLUS                    ArrayLink;              // SMBIOS Type 17
  EFI_MEMORY_MODULE_INFORMATION_DATA            ModuleInformation;
  EFI_MEMORY_CONTROLLER_INFORMATION_DATA        MemoryControllerInfo;
} EFI_MEMORY_SUBCLASS_RECORDS_PLUS;

typedef struct {
  EFI_SUBCLASS_TYPE1_HEADER             Header;
  EFI_MEMORY_SUBCLASS_RECORDS_PLUS      Record;
} EFI_MEMORY_SUBCLASS_DRIVER_DATA_PLUS;
#endif
