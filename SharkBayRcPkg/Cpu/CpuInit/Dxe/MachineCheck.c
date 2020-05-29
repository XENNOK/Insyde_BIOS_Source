/** @file
  Machine check register initialization

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

#include "MachineCheck.h"
#include "Features.h"
#include <FrameworkDxe.h>
#include <Protocol/CpuPlatformPolicy.h>
#include <Library/BaseLib.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Guid/HobList.h>
#include <Guid/PoweronHob.h>

extern DXE_CPU_PLATFORM_POLICY_PROTOCOL *mPlatformCpu;

/**
  Initialize all the Machine-Check registers.

  @param[in] Buffer      - Pointer to private data. Not Used.
  @param[in] MchkEnable  - Enable or disable Mchk.
**/
VOID
InitializeMachineCheckRegisters (
  IN VOID    *Buffer,
  IN BOOLEAN MchkEnable
  )
{
  EFI_CPUID_REGISTER            CpuidRegisters;
  CPU_FEATURE                   Feature;
  CPU_IA32_MCG_CAP_LOW_REGISTER *MCGCap;
  UINT64                        MCGCapValue;
  UINT8                         Count;
  UINT8                         Index;
  UINT8                         StartIndex;
  UINT64                        Value;

  if (!MchkEnable) {
    ///
    /// Do not enable MCHK
    ///
    return;
  }

  AsmCpuid (
          CPUID_VERSION_INFO,
          &CpuidRegisters.RegEax,
          &CpuidRegisters.RegEbx,
          &CpuidRegisters.RegEcx,
          &CpuidRegisters.RegEdx
          );
  *(UINT32 *) (&Feature) = CpuidRegisters.RegEdx;

  if (Feature.MCE && Feature.MCA) {

    MCGCapValue = AsmReadMsr64 (IA32_MCG_CAP);
    MCGCap      = (CPU_IA32_MCG_CAP_LOW_REGISTER *) &MCGCapValue;

    Count       = (UINT8) MCGCap->Count;

    StartIndex  = 0;
    for (Index = StartIndex; Index < Count; Index++) {
      Value = (UINT64) -1;
      AsmWriteMsr64WithScript (IA32_MC0_CTL + Index * 4, Value);
    }
    for (Index = StartIndex; Index < Count; Index++) {
      if (Index <= 4) {
        ///
        /// Clean MC0-MC4 Status when system is cold reset, but no boot script.  S3 is treated as warm reset.
        ///
        if (mPlatformCpu->CpuConfig->IsColdReset == CPU_FEATURE_ENABLE) {
          AsmWriteMsr64 (IA32_MC0_STATUS + Index * 4, 0);
        }
        continue;
      }
      AsmWriteMsr64WithScript (IA32_MC0_STATUS + Index * 4, 0);
    }
    EnableMce ();
  }

  return;
}

/**
  Enable MCE feature for current CPU.

  @param[in] MchkEnable  - Enable Mchk or not.
**/
VOID
InitializeMce (
  IN BOOLEAN MchkEnable
  )
{
  EFI_CPUID_REGISTER CpuidRegisters;
  CPU_FEATURE        Feature;

  if (!MchkEnable) {
    ///
    /// Do not enable MCHK
    ///
    return;
  }

  AsmCpuid (
          CPUID_VERSION_INFO,
          &CpuidRegisters.RegEax,
          &CpuidRegisters.RegEbx,
          &CpuidRegisters.RegEcx,
          &CpuidRegisters.RegEdx
          );
  *(UINT32 *) (&Feature) = CpuidRegisters.RegEdx;

  if (Feature.MCE && Feature.MCA) {
    EnableMce ();
  }

  return;
}
