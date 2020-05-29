/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

@copyright
  Copyright (c) 2004 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  CpuBaseLib.h

@brief
**/
#ifndef _CPU_BASE_LIB_H
#define _CPU_BASE_LIB_H

#include "CpuRegs.h"
#include "CpuDataStruct.h"

///
/// Combine f(FamilyId), m(Model), s(SteppingId) to a single 32 bit number
///
#define EfiMakeCpuVersion(f, m, s)  (((UINT32) (f) << 16) | ((UINT32) (m) << 8) | ((UINT32) (s)))

#if defined (__GNUC__)
#define IA32API                     EFIAPI
#else
#define IA32API                     __cdecl
#endif

/**
  Halt the Cpu
**/
VOID
IA32API
EfiHalt (
  VOID
  );

/**
  Write back and invalidate the Cpu cache
**/
VOID
IA32API
EfiWbinvd (
  VOID
  );

/**
  Invalidate the Cpu cache
**/
VOID
IA32API
EfiInvd (
  VOID
  );

/**
  Get the Cpu info by excute the CPUID instruction

  @param[in] RegisterInEax -The input value to put into register EAX
  @param[in] Regs          -The Output value
**/
VOID
IA32API
EfiCpuid (
  IN UINT32              RegisterInEax,
  OUT EFI_CPUID_REGISTER *Regs
  );

/**
  When RegisterInEax != 4, the functionality is the same as EfiCpuid.
  When RegisterInEax == 4, the function return the deterministic cache
  parameters by excuting the CPUID instruction

  @param[in] RegisterInEax - The input value to put into register EAX
  @param[in] CacheLevel    - The deterministic cache level
  @param[in] Regs          - The Output value
**/
VOID
IA32API
EfiCpuidExt (
  IN UINT32              RegisterInEax,
  IN UINT32              CacheLevel,
  OUT EFI_CPUID_REGISTER *Regs
  );

/**
  Read Cpu MSR

  @param[in] Index - The index value to select the register

  @retval UINT64 - the read data
**/
UINT64
IA32API
EfiReadMsr (
  IN UINT32 Index
  );

/**
  Write Cpu MSR

  @param[in] Index - The index value to select the register
  @param[in] Value - The value to write to the selected register
**/
VOID
IA32API
EfiWriteMsr (
  IN UINT32 Index,
  IN UINT64 Value
  );

/**
  Read Time stamp

  @retval Return the read data
**/
UINT64
IA32API
EfiReadTsc (
  VOID
  );

/**
  Writing back and invalidate the cache,then diable it
**/
VOID
IA32API
EfiDisableCache (
  VOID
  );

/**
  Invalidate the cache,then Enable it
**/
VOID
IA32API
EfiEnableCache (
  VOID
  );

/**
  Get Eflags

  @retval Return the Eflags value
**/
UINT32
IA32API
EfiGetEflags (
  VOID
  );

/**
  Disable interrupt
**/
VOID
IA32API
EfiDisableInterrupts (
  VOID
  );
/**
  Enable interrupt
**/
VOID
IA32API
EfiEnableInterrupts (
  VOID
  );

/**
  Extract CPU detail version infomation

  @param[in] FamilyId   - FamilyId, including ExtendedFamilyId
  @param[in] Model      - Model, including ExtendedModel
  @param[in] SteppingId - SteppingId
  @param[in] Processor  - Processor
**/
VOID
IA32API
EfiCpuVersion (
  IN UINT16           *FamilyId,
  OPTIONAL
  IN UINT8            *Model,
  OPTIONAL
  IN UINT8            *SteppingId,
  OPTIONAL
  IN UINT8 *Processor OPTIONAL
  );

#endif
