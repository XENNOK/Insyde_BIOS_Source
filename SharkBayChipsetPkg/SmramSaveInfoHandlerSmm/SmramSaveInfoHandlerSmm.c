/** @file

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
/** @file
  A helper driver to save information to SMRAM after SMRR is enabled.

  This driver is for ECP platforms.

@copyright
  Copyright (c) 2011 - 2013 Intel Corporation. All rights reserved.
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/

///
/// External include files do NOT need to be explicitly specified in real EDKII
/// environment
///
#include "SmramSaveInfoHandlerSmm.h"

#define SMM_FROM_CPU_DRIVER_SAVE_INFO 0x81

UINT8   mSmiDataRegister;
BOOLEAN mLocked = FALSE;

/**
  Dispatch function for a Software SMI handler.

  @param  DispatchHandle        The handle of this dispatch function.
  @param  DispatchContext       The pointer to the dispatch function's context.
                                The SwSmiInputValue field is filled in
                                by the software dispatch driver prior to
                                invoking this dispatch function.
                                The dispatch function will only be called
                                for input values for which it is registered.
**/
VOID
EFIAPI
SmramSaveInfoHandler (
  IN EFI_HANDLE                  DispatchHandle,
  IN  EFI_SMM_SW_REGISTER_CONTEXT *DispatchContext
  )
{
  EFI_STATUS                Status;
  UINT64                    VarData[3];
  EFI_PHYSICAL_ADDRESS      SmramDataAddress;
  UINTN                     VarSize;

  if (!mLocked && IoRead8 (mSmiDataRegister) == SMM_FROM_CPU_DRIVER_SAVE_INFO) {
    VarSize = sizeof (VarData);
    Status = gRT->GetVariable (
                    SMRAM_CPU_DATA_VARIABLE,
                    &gSmramCpuDataVariableGuid,
                    NULL,
                    &VarSize,
                    VarData
                    );
    if (!EFI_ERROR (Status) && VarSize == sizeof (VarData)) {
       Status = gSmst->SmmAllocatePages (
                  AllocateAnyPages,
                  EfiRuntimeServicesData,
                  EFI_SIZE_TO_PAGES (VarData[2]),
                  &SmramDataAddress
                  );
        ASSERT_EFI_ERROR (Status);
       DEBUG ((DEBUG_INFO, "CPU SMRAM NVS Data - %x\n", SmramDataAddress));
       DEBUG ((DEBUG_INFO, "CPU SMRAM NVS Data size - %x\n", VarData[2]));
       VarData[0] = (UINT64)SmramDataAddress;
      CopyMem (
              (VOID *) (UINTN) (VarData[0]),
              (VOID *) (UINTN) (VarData[1]),
              (UINTN) (VarData[2])
              );
	     Status = gRT->SetVariable (
                  SMRAM_CPU_DATA_VARIABLE,
                  &gSmramCpuDataVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS,
                  VarSize,
                  &VarData
                  );
    }

    mLocked = TRUE;
  }
}

/**
  Entry point function of this driver.

  @param[in] ImageHandle  The firmware allocated handle for the EFI image.
  @param[in] SystemTable  A pointer to the EFI System Table.

  @retval EFI_SUCCESS     The entry point is executed successfully.
  @retval other           Some error occurs when executing this entry point.
**/
EFI_STATUS
EFIAPI
SmramSaveInfoHandlerSmmMain (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS                       Status;
  EFI_SMM_SW_DISPATCH2_PROTOCOL    *SmmSwDispatch;
  EFI_SMM_SW_REGISTER_CONTEXT      SmmSwDispatchContext;
  EFI_HANDLE                       DispatchHandle;
  DXE_CPU_PLATFORM_POLICY_PROTOCOL *CpuPlatformPolicy;

  CpuPlatformPolicy = NULL;
  ///
  /// Get SMI data register
  ///
  mSmiDataRegister = R_PCH_APM_STS;

  ///
  /// Register software SMI handler
  ///
  Status = gSmst->SmmLocateProtocol (
                  &gEfiSmmSwDispatch2ProtocolGuid,
                  NULL,
                  (VOID **) &SmmSwDispatch
                  );
  ASSERT_EFI_ERROR (Status);

  Status = gBS->LocateProtocol (
                  &gDxeCpuPlatformPolicyProtocolGuid,
                  NULL,
                  (VOID **) &CpuPlatformPolicy
                  );
  ASSERT_EFI_ERROR (Status);

  SmmSwDispatchContext.SwSmiInputValue = CpuPlatformPolicy->CpuConfig->SmmbaseSwSmiNumber;

  Status = SmmSwDispatch->Register (
                            SmmSwDispatch,
                            (EFI_SMM_HANDLER_ENTRY_POINT2)&SmramSaveInfoHandler,
                            &SmmSwDispatchContext,
                            &DispatchHandle
                            );
  ASSERT_EFI_ERROR (Status);
  return Status;
}
