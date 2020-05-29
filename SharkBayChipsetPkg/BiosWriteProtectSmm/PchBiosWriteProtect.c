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

/**
  This file contains a 'Sample Driver' and is licensed as such  
  under the terms of your license agreement with Intel or your  
  vendor.  This file may be modified by the user, subject to    
  the additional terms of the license agreement 
**/
/**

Copyright (c) 2011 - 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file 
  PchBiosWriteProtect.c

  @brief 
  PCH BIOS Write Protect Driver.

**/
//[-start-121113-IB10820161-modify]//
#include <PchAccess.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
///
/// Driver Dependency Protocols
///
#include <Protocol/PchPlatformPolicy.h>
#include <Protocol/SmmIchnDispatch.h>
#include <Protocol/SmmIoTrapDispatch.h>
//[-end-121113-IB10820161-modify]//

///
/// Global variables
///
EFI_SMM_ICHN_DISPATCH_PROTOCOL    *mIchnDispatch;
EFI_SMM_IO_TRAP_DISPATCH_PROTOCOL *mPchIoTrap;
UINTN                             mPciD31F0RegBase;

static
VOID
PchBiosWpCallback (
  IN  EFI_HANDLE                              DispatchHandle,
  IN  EFI_SMM_ICHN_DISPATCH_CONTEXT           *DispatchContext
  )
/**

  @brief 
  This hardware SMI handler will be run every time the BIOS Write Enable bit is set.

  @param[in] DispatchHandle       Not used
  @param[in] DispatchContext      Not used

  @retval None

**/
{
  ///
  /// Disable BIOSWE bit to protect BIOS
  ///
  MmioAnd8 ((UINTN) (mPciD31F0RegBase + R_PCH_LPC_BIOS_CNTL), (UINT8) ~B_PCH_LPC_BIOS_CNTL_BIOSWE);
}

VOID
PchBiosLockIoTrapCallback (
  IN  EFI_HANDLE                                DispatchHandle,
  IN  EFI_SMM_IO_TRAP_DISPATCH_CALLBACK_CONTEXT *CallbackContext
  )
/**

  @brief 
  Register an IchnBiosWp callback function to handle TCO BIOSWR SMI
  SMM_BWP and BLE bits will be set here

  @param[in] DispatchHandle       Not used
  @param[in] CallbackContext      Information about the IO trap that occurred

  @retval None

**/
{
  EFI_STATUS                    Status;
  EFI_SMM_ICHN_DISPATCH_CONTEXT IchnContext;
  EFI_HANDLE                    IchnHandle;

  ///
  /// Do not run the callback function if it is not Write cycle trapped or the wrtie data
  /// is not PCH_BWP_SIGNATURE,
  ///
  if ((CallbackContext->Type != WriteTrap) || (CallbackContext->WriteData != PCH_BWP_SIGNATURE)) {
    return;
  }

  if (mIchnDispatch == NULL) {
    return;
  }

  IchnHandle = NULL;

  ///
  /// Set SMM_BWP bit before registering IchnBiosWp
  ///
  MmioOr8 ((UINTN) (mPciD31F0RegBase + R_PCH_LPC_BIOS_CNTL), (UINT8) B_PCH_LPC_BIOS_CNTL_SMM_BWP);

  ///
  /// Register an IchnBiosWp callback function to handle TCO BIOSWR SMI
  ///
  IchnContext.Type = IchnBiosWp;
  Status = mIchnDispatch->Register (
                            mIchnDispatch,
                            PchBiosWpCallback,
                            &IchnContext,
                            &IchnHandle
                            );
  ASSERT_EFI_ERROR (Status);
}

EFI_STATUS
EFIAPI
InstallPchBiosWriteProtect (
  IN EFI_HANDLE            ImageHandle,
  IN EFI_SYSTEM_TABLE      *SystemTable
  )
/**

  @brief 
  Entry point for Pch Bios Write Protect driver.
  
  @param[in] ImageHandle          Image handle of this driver.
  @param[in] SystemTable          Global system service table.

  @retval EFI_SUCCESS             Initialization complete.

**/
{
  EFI_STATUS                                Status;
  DXE_PCH_PLATFORM_POLICY_PROTOCOL          *PchPlatformPolicy;
  EFI_HANDLE                                PchIoTrapHandle;
  EFI_SMM_IO_TRAP_DISPATCH_REGISTER_CONTEXT PchIoTrapContext;

  ///
  /// Locate PCH Platform Policy protocol
  ///
  Status = gBS->LocateProtocol (&gDxePchPlatformPolicyProtocolGuid, NULL, (VOID **)&PchPlatformPolicy);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR | EFI_D_INFO, "Failed to locate PCH Policy protocol.\n"));
    return Status;
  }

  if (PchPlatformPolicy->LockDownConfig->BiosLock == PCH_DEVICE_ENABLE) {
    mPciD31F0RegBase = MmPciAddress (
                        0,
                        0,
                        PCI_DEVICE_NUMBER_PCH_LPC,
                        PCI_FUNCTION_NUMBER_PCH_LPC,
                        0
                        );

    ///
    /// Get the ICHn protocol
    ///
    mIchnDispatch = NULL;
    Status        = gBS->LocateProtocol (&gEfiSmmIchnDispatchProtocolGuid, NULL, (VOID **)&mIchnDispatch);
    ASSERT_EFI_ERROR (Status);

    ///
    /// Locate the PCH IO TRAP Dispatch protocol
    ///
    PchIoTrapHandle = NULL;
    Status = gBS->LocateProtocol (&gEfiSmmIoTrapDispatchProtocolGuid, NULL, (VOID **)&mPchIoTrap);
    ASSERT_EFI_ERROR (Status);

    ///
    /// Register BIOS Lock IO Trap SMI handler
    ///
    PchIoTrapContext.Type         = WriteTrap;
    PchIoTrapContext.Length       = 4;
    PchIoTrapContext.Address      = PchPlatformPolicy->LockDownConfig->PchBiosLockIoTrapAddress;
    PchIoTrapContext.Context      = NULL;
    PchIoTrapContext.MergeDisable = FALSE;
    Status = mPchIoTrap->Register (
                          mPchIoTrap,
                          PchBiosLockIoTrapCallback,
                          &PchIoTrapContext,
                          &PchIoTrapHandle
                          );
    ASSERT_EFI_ERROR (Status);

    DEBUG ((EFI_D_ERROR, "PchBiosLockIoTrapAddress = 0x%x\n", PchIoTrapContext.Address));

    if ((PchPlatformPolicy->LockDownConfig->PchBiosLockIoTrapAddress == 0) &&
        (PchIoTrapContext.Address == 0)) {
      DEBUG ((EFI_D_ERROR | EFI_D_INFO, "Invalid PchIoTrapContext.Address!!!\n"));
      ASSERT (FALSE);
    } else {
      if ((PchPlatformPolicy->LockDownConfig->PchBiosLockIoTrapAddress != 0) &&
          (PchPlatformPolicy->LockDownConfig->PchBiosLockIoTrapAddress != PchIoTrapContext.Address)) {
        DEBUG ((EFI_D_ERROR | EFI_D_INFO, "Invalid PchIoTrapContext.Address!!!\n"));
        ASSERT (FALSE);
      } else {
        PchPlatformPolicy->LockDownConfig->PchBiosLockIoTrapAddress = PchIoTrapContext.Address;
      }
    }
  }

  return EFI_SUCCESS;
}
