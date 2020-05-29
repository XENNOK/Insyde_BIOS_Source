/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/*++
  This file contains an 'Intel Peripheral Driver' and uniquely  
  identified as "Intel Reference Module" and is                 
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/

/*++

Copyright (c)  1999 - 2010 Intel Corporation. All rights reserved
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


#ifndef _ACPI_VARIABLE_SET_H_
#define _ACPI_VARIABLE_SET_H_

#define ACPI_VARIABLE_SET_GUID \
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
  UINT64                SystemMemoryLengthBelow4GB;
  UINT64                SystemMemoryLengthAbove4GB;
  ACPI_CPU_DATA         AcpiCpuData;
} ACPI_VARIABLE_SET;

extern EFI_GUID gAcpiVariableSetGuid;

#endif
