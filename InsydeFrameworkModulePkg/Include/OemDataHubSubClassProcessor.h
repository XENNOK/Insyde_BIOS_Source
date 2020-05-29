//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
#ifndef _OEMDATAHUB_SUBCLASS_PROCESSOR_H_
#define _OEMDATAHUB_SUBCLASS_PROCESSOR_H_
#include EFI_GUID_DEFINITION (DataHubRecords)
#include "Numbers.h"

typedef struct {
  UINT16  Reserved                :1;
  UINT16  Unknown                 :1;
  UINT16  Capable64Bit            :1;
  //
  // SMBIOS 2.7+
  //
  UINT16  MultiCore               :1;
  UINT16  HardwareThread          :1;
  UINT16  ExecuteProtection       :1;
  UINT16  EnhancedVirtualization  :1;
  UINT16  PowerPerformanceControl :1;
  UINT16  Reserved2               :8;
} EFI_PROCESSOR_CHARACTERISTICS_DATA_PLUS;

typedef enum {
  EfiProcessorFamilyIntelCeleronM                   = 0x14, //The definition same with EfiProcessorFamilyM1Reserved2 in DataHubSubClassProcessor.h.
  EfiProcessorFamilyIntelPentium4HT                 = 0x15, //The definition same with EfiProcessorFamilyM1Reserved3 in DataHubSubClassProcessor.h.
  //
  // SMBIOS 2.7+
  //
  EfiProcessorFamilyAmdTurionIIUltraDualCoreMobileM = 0x38,
  EfiProcessorFamilyAmdTurionIIDualCoreMobileM      = 0x39,
  EfiProcessorFamilyAmdAthlonIIDualCoreM            = 0x3A,
  EfiProcessorFamilyAmdOpteron6100Series            = 0x3B,
  EfiProcessorFamilyAmdOpteron4100Series            = 0x3C,

  //
  // SMBIOS 2.7.1+
  //
  EfiProcessorFamilyAmdOpteron6200Series            = 0x3D,
  EfiProcessorFamilyAmdOpteron4200Series            = 0x3E,
  //
  // SMBIOS 2.7.1+
  //
  EfiProcessorFamilyAmdCSeries                      = 0x46,
  EfiProcessorFamilyAmdESeries                      = 0x47,
  EfiProcessorFamilyAmdSSeries                      = 0x48,
  EfiProcessorFamilyAmdGSeries                      = 0x49,
  //
  // SMBIOS 2.7+
  //
  EfiProcessorFamilyIntelCoreI5                     = 0xCD,
  EfiProcessorFamilyIntelCoreI3                     = 0xCE,
  EfiProcessorFamilyViaNano                         = 0xD9,
  EfiProcessorFamilyMultiCoreIntelXeon3400Series    = 0xE0,
  EfiProcessorFamilyAmdPhenomII                     = 0xEC,
  EfiProcessorFamilyAmdAthlonII                     = 0xED,
  EfiProcessorFamilySixCoreAmdOpteron               = 0xEE,
  EfiProcessorFamilyAmdSempronM                     = 0xEF,
} EFI_PROCESSOR_FAMILY_DATA_PLUS;

typedef enum {
  //
  // SMBIOS 2.7+
  //
  EfiProcessorSocketG34                             = 0x1A,
  EfiProcessorSocketAM3                             = 0x1B,
  EfiProcessorSocketC32                             = 0x1C,
  EfiProcessorSocketLGA1156                         = 0x1D,
  EfiProcessorSocketLGA1567                         = 0x1E,
  EfiProcessorSocketPGA988A                         = 0x1F,
  EfiProcessorSocketBGA1288                         = 0x20,
  EfiProcessorSocketrPGA988B                        = 0x21,
  EfiProcessorSocketBGA1023                         = 0x22,
  EfiProcessorSocketLGA1224                         = 0x23,
  EfiProcessorSocketLGA1155                         = 0x24,
  EfiProcessorSocketLGA1356                         = 0x25,
  EfiProcessorSocketLGA2011                         = 0x26,
  //
  // SMBIOS 2.7.1+
  //
  EfiProcessorSocketFS1                             = 0x27,
  EfiProcessorSocketFS2                             = 0x28,
  EfiProcessorSocketFM1                             = 0x29,
  EfiProcessorSocketFM2                             = 0x2A
} EFI_PROCESSOR_SOCKET_TYPE_DATA_PLUS;

typedef union {
  EFI_CPU_VARIABLE_RECORD                 EfiCpuVariableRecord;
  EFI_PROCESSOR_CHARACTERISTICS_DATA_PLUS ProcessorCharacteristics;
} EFI_CPU_VARIABLE_RECORD_PLUS;

typedef struct {
  EFI_SUBCLASS_TYPE1_HEADER      DataRecordHeader;
  EFI_CPU_VARIABLE_RECORD_PLUS   VariableRecord;
} EFI_CPU_DATA_RECORD_PLUS;
#endif
