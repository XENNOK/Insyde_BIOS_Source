//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/*++

Copyright (c)  1999 - 2006 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

    AcpiVariable.h

Abstract:

    GUIDs used for ACPI variables.

--*/

#ifndef _ACPI_VARIABLE_H_
#define _ACPI_VARIABLE_H_

#define EFI_ACPI_VARIABLE_GUID \
  { \
    0xc020489e, 0x6db2, 0x4ef2, 0x9a, 0xa5, 0xca, 0x6, 0xfc, 0x11, 0xd3, 0x6a \
  }

#define ACPI_GLOBAL_VARIABLE  L"AcpiGlobalVariable"

//
// The following structure combine all ACPI related variables into one in order
// to boost performance
//
#pragma pack(1)
typedef struct {
  UINT16  Limit;
  UINTN               Base;
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

typedef struct {
  //
  // Acpi Related variables
  //
  EFI_PHYSICAL_ADDRESS  AcpiReservedMemoryBase;
  UINT32                AcpiReservedMemorySize;
  EFI_PHYSICAL_ADDRESS  S3ReservedLowMemoryBase;
  EFI_PHYSICAL_ADDRESS  AcpiBootScriptTable;
  EFI_PHYSICAL_ADDRESS  RuntimeScriptTableBase;
  EFI_PHYSICAL_ADDRESS  AcpiFacsTable;
  UINT64                SystemMemoryLength;
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
