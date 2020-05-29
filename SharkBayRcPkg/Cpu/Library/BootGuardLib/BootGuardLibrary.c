/** @file
  BootGuardLibrary implementation.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
**/

#include <Uefi.h>
#include <CpuRegs.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/CpuPlatformLib.h>

/**
  Determine if Anchor Cove is supported

  @retval TRUE  - Processor is Anchor Cove capable.
  @retval FALSE - Processor is not Anchor Cove capable.

**/
BOOLEAN
IsBootGuardSupported (
  VOID
  )
{
  UINT64          BootGuardBootStatus;
  UINT32          BootGuardAcmStatus;
  UINT64          BootGuardCapability;
  CPU_STEPPING    CpuSteppingId;

  //
  // Return unsupported if processor is not ULT sku
  //
  if(GetCpuSku() != EnumCpuUlt) {
    return FALSE;
  }

  BootGuardBootStatus = *(UINT64 *) (UINTN) (TXT_PUBLIC_BASE + R_CPU_BOOT_GUARD_BOOTSTATUS);
  BootGuardAcmStatus  = *(UINT32 *) (UINTN) (TXT_PUBLIC_BASE + R_CPU_BOOT_GUARD_ACM_STATUS);
  BootGuardCapability = AsmReadMsr64 (MSR_BOOT_GUARD_SACM_INFO) & BIT32;
  CpuSteppingId = GetCpuStepping();

  if (CpuSteppingId >= EnumHswUltC0) {
    if (BootGuardCapability != 0) {
      DEBUG ((EFI_D_ERROR, "Processor supports Boot Guard.\n"));
      return TRUE;
    } else {
      DEBUG ((EFI_D_ERROR, "Processor does not support Boot Guard.\n"));
      return FALSE;
    }
  } else {
    if(((BootGuardBootStatus & BIT62) == 0) &&                            // Check for ACM not found in FIT
       ((BootGuardAcmStatus & B_BOOT_GUARD_ACM_ERRORCODE_MASK) == 0) &&   // Check for ACM failed to load/run successfully
       ((BootGuardAcmStatus & BIT31) == 0)) {                             // Check if ACM Entered
      DEBUG ((EFI_D_ERROR, "Processor does not support Boot Guard.\n"));
      return FALSE;
    } else {
      DEBUG ((EFI_D_ERROR, "Processor supports Boot Guard.\n"));
      return TRUE;
    }
  }
}

/**
  Stop PBE timer if system is in Boot Guard boot

  @retval EFI_SUCCESS        - Stop PBE timer
  @retval EFI_UNSUPPORTED    - Not in Boot Guard boot mode.
**/
EFI_STATUS
StopPbeTimer (
  VOID
  )
{
  UINT64   BootGuardBootStatus;
  UINT64   BootGuardOperationMode;

  if (IsBootGuardSupported()) {
    BootGuardBootStatus = (*(UINT64 *) (UINTN) (TXT_PUBLIC_BASE + R_CPU_BOOT_GUARD_BOOTSTATUS) & (BIT63|BIT62));
    BootGuardOperationMode = AsmReadMsr64 (MSR_BOOT_GUARD_SACM_INFO);

    //
    // Stop PBET if Verified/Measured/NEM bit is set in MSR 0x13A or
    // Boot Guard fails to launch or fails to execute successfully for avoiding brick platform
    //

    if (BootGuardBootStatus == V_CPU_BOOT_GUARD_LOAD_ACM_SUCCESS) {
      if (BootGuardOperationMode == 0) {
        DEBUG ((EFI_D_ERROR, "Platform in Legacy boot mode.\n"));
        return EFI_UNSUPPORTED;
      } else {
        DEBUG ((EFI_D_ERROR, "Platform in Boot Guard Boot mode.\n"));
      }
    } else {
      DEBUG ((EFI_D_ERROR, "Boot Guard ACM launch failed or ACM execution failed.\n"));
    }

    DEBUG ((EFI_D_ERROR, "Disable PBET\n"));
    AsmWriteMsr64 (MSR_BC_PBEC, B_STOP_PBET);
  } else {
    DEBUG ((EFI_D_ERROR, "Boot Guard is not supported.\n"));
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}

