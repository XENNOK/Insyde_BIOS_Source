/** @file
  This is the SMM driver for saving and restoring the powermanagement related MSRs

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

@copyright
  Copyright (c) 2011 - 2013 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

**/
#include "PowerMgmtS3.h"
#include <CpuAccess.h>
#include <PowerMgmtDefinitions.h>
#include <Library/BaseLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/CpuPlatformLib.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/CpuPlatformPolicy.h>


///
/// SMM system table pointer
///
DXE_CPU_PLATFORM_POLICY_PROTOCOL  *mCpuPlatformPolicy;
///
/// MSR table for S3 resume
///
STATIC EFI_MSR_VALUES mMsrValues[] = {
  { MSR_IA32_PERF_CTRL,                 0, B_IA32_PERF_CTRLP_STATE_TARGET, TRUE },
  { MSR_PMG_IO_CAPTURE_BASE,            0, 0xFFFFFFFFFFFFFFFF, TRUE },
  { MSR_PMG_CST_CONFIG,                 0, 0xFFFFFFFFFFFFFFFF, TRUE },
  { MSR_MISC_PWR_MGMT,                  0, 0xFFFFFFFFFFFFFFFF, TRUE },
  { MSR_IA32_MISC_ENABLE,               0, B_CPUID_POWER_MANAGEMENT_EAX_TURBO | B_MSR_IA32_MISC_DISABLE_TURBO | B_MSR_IA32_MISC_ENABLE_MONITOR | B_MSR_IA32_MISC_ENABLE_TME | B_MSR_IA32_MISC_ENABLE_EIST, TRUE },
  { MSR_POWER_CTL,                      0, 0xFFFFFFFFFFFFFFFF, TRUE },
  { MSR_PACKAGE_POWER_LIMIT,            0, 0xFFFFFFFFFFFFFFFF, TRUE },
  { MSR_PLATFORM_POWER_LIMIT,           0, 0xFFFFFFFFFFFFFFFF, FALSE },
  { MSR_C_STATE_LATENCY_CONTROL_0,      0, 0xFFFFFFFFFFFFFFFF, TRUE },
  { MSR_C_STATE_LATENCY_CONTROL_1,      0, 0xFFFFFFFFFFFFFFFF, TRUE },
  { MSR_C_STATE_LATENCY_CONTROL_2,      0, 0xFFFFFFFFFFFFFFFF, TRUE },
  { MSR_C_STATE_LATENCY_CONTROL_3,      0, 0xFFFFFFFFFFFFFFFF, FALSE },
  { MSR_C_STATE_LATENCY_CONTROL_4,      0, 0xFFFFFFFFFFFFFFFF, FALSE },
  { MSR_C_STATE_LATENCY_CONTROL_5,      0, 0xFFFFFFFFFFFFFFFF, FALSE },
  { MSR_FLEX_RATIO,                     0, 0xFFFFFFFFFFFFFFFF, TRUE },
  { MSR_IA32_ENERGY_PERFORMANCE_BIAS,   0, 0xFFFFFFFFFFFFFFFF, TRUE },
  { MSR_CONFIG_TDP_CONTROL,             0, 0xFFFFFFFFFFFFFFFF, FALSE },
  { MSR_RFI_TUNNING,                    0, 0xFFFFFFFFFFFFFFFF, FALSE },
  { MSR_TURBO_ACTIVATION_RATIO,         0, 0xFFFFFFFFFFFFFFFF, FALSE },
  { MSR_DDR_RAPL_LIMIT,                 0, 0xFFFFFFFFFFFFFFFF, FALSE },
  { MSR_TURBO_RATIO_LIMIT,              0, 0xFFFFFFFFFFFFFFFF, TRUE }
};

/**
  Save processor MSR runtime settings for S3.

  @retval EFI_SUCCESS   Processor MSR setting is saved.
**/
STATIC
EFI_STATUS
S3SaveMsr (
  VOID
  )
{
  UINT32             Index;
  EFI_CPUID_REGISTER Cpuid06 = { 0, 0, 0, 0 };
  UINT64             MsrValue;

  for (Index = 0; Index < sizeof (mMsrValues) / sizeof (EFI_MSR_VALUES); Index++) {
    DEBUG ((EFI_D_INFO, " MSR Number: %x\n", mMsrValues[Index].Index));
    if (mMsrValues[Index].Index == MSR_IA32_ENERGY_PERFORMANCE_BIAS) {
      ///
      /// MSR_IA32_ENERGY_PERFORMANCE_BIAS (1B0h) is accessible only if CPUID(6), ECX[3] = 1 to indicate feature availability.
      ///
      AsmCpuid (CPUID_FUNCTION_6, &Cpuid06.RegEax, &Cpuid06.RegEbx, &Cpuid06.RegEcx, &Cpuid06.RegEdx);
      if (!(Cpuid06.RegEcx & B_CPUID_POWER_MANAGEMENT_ECX_ENERGY_EFFICIENT_POLICY_SUPPORT)) {
        mMsrValues[Index].RestoreFlag = FALSE;
        continue;
      }
    }
    ///
    /// Check for HSW specific MSRs
    ///
    MsrValue = AsmReadMsr64 (MSR_PLATFORM_INFO);
    ///
    ///
    /// Check PLATFORM_INFO MSR[34:33] > 0 before accessing the MSR_CONFIG_TDP_CONTROL
    ///
    if ((mMsrValues[Index].Index == MSR_CONFIG_TDP_CONTROL) &&
        ((RShiftU64 (MsrValue, N_MSR_PLATFORM_INFO_CONFIG_TDP_NUM_LEVELS_OFFSET) & 0x03))
        ) {
      mMsrValues[Index].RestoreFlag = TRUE;
    }
    ///
    /// MSR_TURBO_ACTIVATION_RATIO, MSR_DDR_RAPL_LIMIT, MSR_RFI_TUNNING are supported only on HSW A0 or above.
    ///
    if (mMsrValues[Index].Index == MSR_TURBO_ACTIVATION_RATIO) {
      mMsrValues[Index].RestoreFlag = TRUE;
    }
    if (mMsrValues[Index].Index == MSR_DDR_RAPL_LIMIT) {
      mMsrValues[Index].RestoreFlag = TRUE;
    }
    ///
    /// Check PLATFORM_INFO MSR[25] == 1 before accessing the MSR_RFI_TUNNING
    ///
    if (mMsrValues[Index].Index == MSR_RFI_TUNNING) {
      if ((mCpuPlatformPolicy->PowerMgmtConfig->RfiFreqTunningOffset != AUTO) && (MsrValue & B_FIVR_RFI_TUNING_AVAIL)) {
        mMsrValues[Index].RestoreFlag = TRUE;
      }
    }

    if(GetCpuSku()== EnumCpuUlt) {
      if ((mMsrValues[Index].Index == MSR_C_STATE_LATENCY_CONTROL_3) ||
          (mMsrValues[Index].Index == MSR_C_STATE_LATENCY_CONTROL_4)||
          (mMsrValues[Index].Index == MSR_C_STATE_LATENCY_CONTROL_5)){
        mMsrValues[Index].RestoreFlag = TRUE;
      }
    }

    ///
    /// PL3 is supported on HSW ULT C0 & HSW  C0 and later 
    ///
    if (mMsrValues[Index].Index == MSR_PLATFORM_POWER_LIMIT) {
      if (((GetCpuFamily() == EnumCpuHsw) && (GetCpuStepping() >= EnumHswC0))
         || ((GetCpuFamily() == EnumCpuHswUlt) && (GetCpuStepping() >= EnumHswUltC0))) {
        mMsrValues[Index].RestoreFlag = TRUE;
      }
    }

    if (mMsrValues[Index].RestoreFlag == TRUE) {
      mMsrValues[Index].Value = AsmReadMsr64 (mMsrValues[Index].Index);
      DEBUG ((EFI_D_INFO, " MSR Number %x read Done \n", mMsrValues[Index].Index));
    }
  }

  return EFI_SUCCESS;
}

/**
  Restore processor MSR runtime settings for S3.

  @param[in] DispatchHandle  - The handle of this callback, obtained when registering
  @param[in] DispatchContex  - Pointer to the EFI_SMM_SW_DISPATCH_CONTEXT

  @retval EFI_SUCCESS   Processor MSR setting is restored.
**/
STATIC
EFI_STATUS
EFIAPI
S3RestoreMsr (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID  *Context         OPTIONAL,
  IN OUT VOID    *CommBuffer      OPTIONAL,
  IN OUT UINTN   *CommBufferSize  OPTIONAL
  )
{
  ///
  /// Restore MSR's on all logical processors.
  ///
  RunOnAllLogicalProcessors (ApSafeRestoreMsr, NULL);
  return EFI_SUCCESS;
}

/**
  Runs the specified procedure on all logical processors, passing in the
  parameter buffer to the procedure.

  @param[in] Procedure     The function to be run.
  @param[in] Buffer        Pointer to a parameter buffer.

  @retval EFI_SUCCESS
**/
STATIC
EFI_STATUS
RunOnAllLogicalProcessors (
  IN OUT EFI_AP_PROCEDURE Procedure,
  IN OUT VOID             *Buffer
  )
{
  UINTN      Index;
  EFI_STATUS Status;
  ///
  /// Run the procedure on all logical processors.
  ///
  (*Procedure) (Buffer);
  for (Index = 1; Index < gSmst->NumberOfCpus; Index++) {
    Status = EFI_NOT_READY;
    while (Status != EFI_SUCCESS) {
      Status = gSmst->SmmStartupThisAp (Procedure, Index, Buffer);
      if (Status != EFI_SUCCESS) {
        ///
        /// SmmStartupThisAp might return failure if AP is busy executing some other code. Let's wait for sometime and try again.
        ///
        PchPmTimerStall (PPM_WAIT_PERIOD);
      }
    }
  }

  return EFI_SUCCESS;
}

/**
  This function will restore MSR settings.

  This function must be MP safe.

  @param[in] Buffer        Unused

  @retval EFI_SUCCESS   MSR restored
**/
VOID
EFIAPI
ApSafeRestoreMsr (
  IN OUT VOID *Buffer
  )
{
  UINT32 Index;
  UINT64 MsrValue;

  for (Index = 0; Index < sizeof (mMsrValues) / sizeof (EFI_MSR_VALUES); Index++) {
    ///
    /// Check RestoreFlag and skip restoring the MSR if it is set to FALSE
    ///
    if (mMsrValues[Index].RestoreFlag == FALSE) {
      DEBUG ((EFI_D_INFO, "Skipping MSR : %x as RestoreFalg is set to FALSE \n", mMsrValues[Index].Index));
      continue;
    }
    ///
    /// Check for Lock bits before programming
    ///
    MsrValue = AsmReadMsr64 (mMsrValues[Index].Index);
    if ((mMsrValues[Index].Index == MSR_CONFIG_TDP_CONTROL) && (MsrValue & CONFIG_TDP_CONTROL_LOCK)) {
      continue;
    }

    if ((mMsrValues[Index].Index == MSR_TURBO_ACTIVATION_RATIO) && (MsrValue & MSR_TURBO_ACTIVATION_RATIO_LOCK)) {
      continue;
    }

    if ((mMsrValues[Index].Index == MSR_PACKAGE_POWER_LIMIT) && (MsrValue & B_POWER_LIMIT_LOCK)) {
      continue;
    }

    if ((mMsrValues[Index].Index == MSR_PLATFORM_POWER_LIMIT) && (MsrValue & B_POWER_LIMIT_LOCK)) {
      continue;
    }

    if ((mMsrValues[Index].Index == MSR_DDR_RAPL_LIMIT) && (MsrValue & B_POWER_LIMIT_LOCK)) {
      continue;
    }

    MsrValue = AsmReadMsr64 (mMsrValues[Index].Index);
    MsrValue &= ~mMsrValues[Index].BitMask;
    MsrValue |= (mMsrValues[Index].Value & mMsrValues[Index].BitMask);
    AsmWriteMsr64 (mMsrValues[Index].Index, MsrValue);
  }

  return;
}

/**
  Initialize the S3 power management Handler.

  @param[in] ImageHandle   - Pointer to the loaded image protocol for this driver
  @param[in] SystemTable   - Pointer to the EFI System Table

  @retval EFI_SUCCESS   The driver installes/initialized correctly.
**/
EFI_STATUS
EFIAPI
PowerMgmtS3SmmEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{

  EFI_STATUS                        Status;
  EFI_SMM_SW_REGISTER_CONTEXT       SwContext;
  EFI_SMM_SW_DISPATCH2_PROTOCOL     *SwDispatch2;
  EFI_HANDLE                        SwHandle;

  SwHandle = 0;
  DEBUG ((EFI_D_INFO, " PpmS3SmmEntryPoint Started : \n"));

  ///
  /// Locate platform configuration information
  ///
  Status = gBS->LocateProtocol (&gDxeCpuPlatformPolicyProtocolGuid, NULL, (VOID **) &mCpuPlatformPolicy);
  ASSERT_EFI_ERROR (Status);

  ///
  /// Locate the ICH SMM SW dispatch protocol
  ///
  Status = gSmst->SmmLocateProtocol (&gEfiSmmSwDispatch2ProtocolGuid, NULL, (VOID **) &SwDispatch2);
  ASSERT_EFI_ERROR (Status);

  ///
  /// Register ACPI S3 MSR restore handler
  ///
  SwContext.SwSmiInputValue = mCpuPlatformPolicy->PowerMgmtConfig->S3RestoreMsrSwSmiNumber;

  Status = SwDispatch2->Register (
                          SwDispatch2,
                          S3RestoreMsr,
                          &SwContext,
                          &SwHandle
                          );
  ASSERT_EFI_ERROR (Status);

  ///
  ///  Save MSRs for S3 Resume.
  ///
  DEBUG ((EFI_D_INFO, " Saving Processor MSR for S3 Resume \n"));

  S3SaveMsr ();

  return EFI_SUCCESS;
}
