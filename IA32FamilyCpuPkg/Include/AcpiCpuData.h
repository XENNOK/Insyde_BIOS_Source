//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/** @file
  Definitions for CPU S3 data.

  Copyright (c) 2011 - 2012, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#ifndef _ACPI_CPU_DATA_H_
#define _ACPI_CPU_DATA_H_

typedef struct {
  BOOLEAN               APState;
  EFI_PHYSICAL_ADDRESS  StartupVector;
  EFI_PHYSICAL_ADDRESS  GdtrProfile;
  EFI_PHYSICAL_ADDRESS  IdtrProfile;
  EFI_PHYSICAL_ADDRESS  StackAddress;
  UINT32                StackSize;
  UINT32                NumberOfCpus;
  EFI_PHYSICAL_ADDRESS  MtrrTable;
  EFI_PHYSICAL_ADDRESS  PreSmmInitRegisterTable;
  EFI_PHYSICAL_ADDRESS  RegisterTable;
  EFI_PHYSICAL_ADDRESS  ApMachineCheckHandlerBase;
  UINT32                ApMachineCheckHandlerSize;
} ACPI_CPU_DATA;

#endif

