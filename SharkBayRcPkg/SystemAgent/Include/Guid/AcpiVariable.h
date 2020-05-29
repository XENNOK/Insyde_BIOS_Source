/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

@copyright
  Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
    AcpiVariable.h

@brief
    GUIDs used for ACPI variables.

**/
#ifndef _ACPI_VARIABLE_H_
#define _ACPI_VARIABLE_H_

///
/// RC Override: sync from R8HswClientPkg\Guid\AcpiVariable\AcpiVariable.h
///
#define EFI_ACPI_VARIABLE_GUID \
  { \
    0xc020489e, 0x6db2, 0x4ef2, 0x9a, 0xa5, 0xca, 0x6, 0xfc, 0x11, 0xd3, 0x6a \
  }

#define ACPI_GLOBAL_VARIABLE  L"AcpiGlobalVariable"

///
/// The following structure combine all ACPI related variables into one in order
/// to boost performance
///
#pragma pack(1)
typedef struct {
  UINT16  Limit;
  UINTN   Base;
} PSEUDO_DESCRIPTOR;
#pragma pack()

typedef struct {
  BOOLEAN               APState;
  BOOLEAN               S3BootPath;
  EFI_PHYSICAL_ADDRESS  WakeUpBuffer;
  EFI_PHYSICAL_ADDRESS  GdtrProfile;
  EFI_PHYSICAL_ADDRESS  IdtrProfile;
  EFI_PHYSICAL_ADDRESS  CpuPrivateData;
  EFI_PHYSICAL_ADDRESS  StackAddress;
  EFI_PHYSICAL_ADDRESS  MicrocodePointerBuffer;
  EFI_PHYSICAL_ADDRESS  SmramBase;
  EFI_PHYSICAL_ADDRESS  SmmStartImageBase;
  UINT32                SmmStartImageSize;
  UINT32                NumberOfCpus;
  UINT32                ApInitDone;
} ACPI_CPU_DATA;

typedef
EFI_STATUS
(EFIAPI *EFI_FUNC_CHECK) (
  IN UINT8                        SupportFlag,
  IN OUT UINT8                    *Feature
);
typedef struct {
  UINT16  Index;
  UINT64  Value;
  EFI_FUNC_CHECK    FuncCheck; 
  UINT8             SetupValue; 
} EFI_MSR_VALUES;

//
// Acpi Related variables
//
typedef struct {
  EFI_PHYSICAL_ADDRESS  AcpiReservedMemoryBase;
  UINT32                AcpiReservedMemorySize;
  EFI_PHYSICAL_ADDRESS  S3ReservedLowMemoryBase;
  EFI_PHYSICAL_ADDRESS  AcpiBootScriptTable;
  EFI_PHYSICAL_ADDRESS  RuntimeScriptTableBase;
  EFI_PHYSICAL_ADDRESS  AcpiFacsTable;
  UINT64                SystemMemoryLengthBelow4GB;
  UINT64                SystemMemoryLengthAbove4GB;
  ACPI_CPU_DATA         AcpiCpuData;
  //
  // VGA OPROM to support Video Re-POST for Linux S3
  //
  EFI_PHYSICAL_ADDRESS  VideoOpromAddress;
  UINT32                VideoOpromSize;

  EFI_PHYSICAL_ADDRESS  MsrTableAddress; 
  BOOLEAN                 ResetRequired; 
  UINT32                   NumofThreading;
  UINT32                   CountThreading;
} ACPI_VARIABLE_SET;

extern EFI_GUID gEfiAcpiVariableGuid;

#endif
