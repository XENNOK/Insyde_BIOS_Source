/** @file
  EFI 2.0 PEIM to initialize the cache and load the BSP microcode

@copyright
  Copyright (c) 1999 - 2013 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement

**/

#include "CpuInitPeim.h"
#include <Library/DebugLib.h>
#include <Library/BootGuardLibrary.h>
#include <MeAccess.h>
#include <HeciRegs.h>
#include <CpuRegs.h>

VOID
BootGuardInit (
  IN EFI_PEI_SERVICES             **PeiServices,
  IN PEI_CPU_PLATFORM_POLICY_PPI  *CpuPlatformPolicyPpi
  )
/**

  Perform the platform spefific initializations.

  @param[in] PeiServices          - Indirect reference to the PEI Services Table.
  @param[in] CpuPlatformPolicyPpi - Platform Policy PPI

**/
{
  UINT32  MsrValue;
  UINT32  MeFwSts4;
  UINT32  BootGuardAcmStatus;

  if (CpuPlatformPolicyPpi->SecurityConfig->BootGuardConfig == NULL) {
    return;
  }

  ///
  /// Check if System Supports Boot Guard
  ///
  if( IsBootGuardSupported() ) {
    CpuPlatformPolicyPpi->SecurityConfig->BootGuardConfig->BootGuardSupport = TRUE;

    BootGuardAcmStatus  = *(UINT32 *) (UINTN) (TXT_PUBLIC_BASE + R_CPU_BOOT_GUARD_ACM_STATUS);
    DEBUG ((EFI_D_INFO, "Boot Guard ACM Status = %x\n", BootGuardAcmStatus));

    ///
    /// Check Bit 12 in ME FWSTS4 to check if TPM_DISCONNECT_ALL bit is set
    /// or ENF Shutdown path is taken by ME FW.
    /// Also Identify any failures in ACM
    ///
    MeFwSts4 = HeciPciRead32(R_ME_HFS_4);
    DEBUG ((EFI_D_INFO, "ME FW STS 4 = %x\n", MeFwSts4));
    if((MeFwSts4 & (B_TPM_DISCONNECT | B_BOOT_GUARD_ENF_MASK)) || (BootGuardAcmStatus & B_BOOT_GUARD_ACM_ERRORCODE_MASK)) {
      DEBUG ((EFI_D_INFO, "All TPM's on Platform are Disconnected\n"));
      CpuPlatformPolicyPpi->SecurityConfig->BootGuardConfig->DisconnectAllTpms = TRUE;
    }
    CpuPlatformPolicyPpi->SecurityConfig->BootGuardConfig->BypassTpmInit = FALSE;
    CpuPlatformPolicyPpi->SecurityConfig->BootGuardConfig->MeasuredBoot = FALSE;

    if(MeFwSts4 & BIT10) {
      DEBUG ((EFI_D_INFO, "Sx Resume Type Identified - TPM Event Log not required for ACM Measurements\n"));
      CpuPlatformPolicyPpi->SecurityConfig->BootGuardConfig->ByPassTpmEventLog = TRUE;
    }
    ///
    /// Check bit 0 of BOOT_GUARD_SACM_INFO MSR if system is in Boot Guard boot mode
    ///
    MsrValue = (UINT32) AsmReadMsr64 (MSR_BOOT_GUARD_SACM_INFO);
    DEBUG ((EFI_D_INFO, "MSR_BOOT_GUARD_SACM_INFO MSR = %x\n", MsrValue));
    if ( (MsrValue & B_NEM_INIT) == 0 ) {
      DEBUG ((EFI_D_INFO, "NEM is not initiated by Boot Guard ACM\n"));
    }
    if (MsrValue & B_MEASURED) {
      CpuPlatformPolicyPpi->SecurityConfig->BootGuardConfig->MeasuredBoot = TRUE;
      ///
      /// if measured bit is set, BIOS needs to bypass startup command
      ///
      if (MsrValue & B_TPM_SUCCESS) {
        CpuPlatformPolicyPpi->SecurityConfig->BootGuardConfig->BypassTpmInit = TRUE;
      }
      ///
      /// Read present TPM type
      ///
      CpuPlatformPolicyPpi->SecurityConfig->BootGuardConfig->TpmType = (TPM_TYPE) ( (MsrValue & V_TPM_PRESENT_MASK) >> 1 );
      DEBUG ((EFI_D_INFO, "TPM Type is %x\n", CpuPlatformPolicyPpi->SecurityConfig->BootGuardConfig->TpmType));
    }
  } else {
    CpuPlatformPolicyPpi->SecurityConfig->BootGuardConfig->BootGuardSupport = FALSE;
  }

  return;
}
