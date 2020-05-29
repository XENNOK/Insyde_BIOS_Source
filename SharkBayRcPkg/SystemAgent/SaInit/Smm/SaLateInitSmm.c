/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/

/**

Copyright (c) 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file
  SaLateInitSmm.c

@brief
  This SMM driver will handle SA relevant late initialization

**/

///
/// External include files do NOT need to be explicitly specified in real EDKII
/// environment
///
#if !defined(EDK_RELEASE_VERSION) || (EDK_RELEASE_VERSION < 0x00020000)
#include "SaBuildFlags.h"
#include "EdkIIGlueDxe.h"
#include "SaLateInitSmm.h"
#include "SaRegs.h"
#include "SaAccess.h"
#include "PchAccess.h"
#include "CpuIA32.h"
#include "SaPcieLib.h"
#include EFI_PROTOCOL_DEPENDENCY (SmmIoTrapDispatch)
#include EFI_GUID_DEFINITION (SaDataHob)
#endif

typedef enum {
  EnumSaSmiCallbackForMaxPayLoad,
  EnumSaSmiCallbackForSecurityLock,
  EnumSaSmiCallbackForLateInit,
  EnumSaSmiCallbackForS3resume,
  EnumSaSmiCallbackMax
} SMI_OPERATION;

UINT8                           mSaSmiCallbackPhase = EnumSaSmiCallbackForMaxPayLoad;

/**

  A SMI callback to do SA relevant late initialization

  @param[in] DispatchHandle  - The handle of this callback, obtained when registering
  @param[in] DispatchContext - Pointer to the EFI_SMM_IO_TRAP_DISPATCH_CALLBACK_CONTEXT

  @retval None

**/
VOID
EFIAPI
SaIoTrapSmiCallback (
  IN  EFI_HANDLE                                  DispatchHandle,
  IN  EFI_SMM_IO_TRAP_DISPATCH_CALLBACK_CONTEXT   *CallbackContext
  )
{

  if (mSaSmiCallbackPhase == EnumSaSmiCallbackMax) {
    return;
  }
  if (mSaSmiCallbackPhase == EnumSaSmiCallbackForMaxPayLoad) {
    SaPcieEnumCallback ();
    ///
    /// Switch to next phase
    ///
    mSaSmiCallbackPhase = EnumSaSmiCallbackForSecurityLock;
  } else if (mSaSmiCallbackPhase == EnumSaSmiCallbackForSecurityLock) {
    ///
    /// Save platform registers including IGFX BAR & COMMAND registers and PAM
    ///
    SaSaveRestorePlatform (TRUE);
    SaSecurityLock ();
    ///
    /// Switch to next phase
    ///
    mSaSmiCallbackPhase = EnumSaSmiCallbackForLateInit;
  } else if (mSaSmiCallbackPhase == EnumSaSmiCallbackForLateInit) {
    ///
    /// Expected to execute in ReadyToBoot point (after OROM)
    ///
    SaPcieConfigAfterOpRom ();
    ///
    /// Switch to next phase
    ///
    mSaSmiCallbackPhase = EnumSaSmiCallbackForS3resume;
  } else if (mSaSmiCallbackPhase == EnumSaSmiCallbackForS3resume) {
    ///
    /// Expected to execute in end of S3 resume flow
    ///
    SaS3ResumeCallback ();
  }
}

/**

  Initializes the SA SMM handler

  @param[in] ImageHandle - The image handle of Wake On Lan driver
  @param[in] SystemTable - The standard EFI system table

  @retval EFI_SUCCESS    - SA SMM handler was installed or not necessary
  @retval EFI_NOT_FOUND  - DxePlatformSaPolicy not found

**/
EFI_STATUS
SaLateInitSmmEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                                Status;
  EFI_SMM_IO_TRAP_DISPATCH_PROTOCOL         *PchIoTrap;
  EFI_HANDLE                                PchIoTrapHandle;
  EFI_SMM_IO_TRAP_DISPATCH_REGISTER_CONTEXT PchIoTrapContext;
  SA_DATA_HOB                               *SaDataHob;
  DXE_PLATFORM_SA_POLICY_PROTOCOL           *DxePlatformSaPolicy;
  BOOLEAN                                   InitPcieAspmAfterOprom;
  EFI_PHYSICAL_ADDRESS                      IotrapAddress;

  DEBUG ((EFI_D_INFO, "SaLateInitSmmEntryPoint()\n"));

  SaDataHob = NULL;
  SaDataHob = (SA_DATA_HOB *)GetFirstGuidHob (&gSaDataHobGuid);
  Status = EFI_NOT_FOUND;
  if ((SaDataHob != NULL)) {
    ///
    /// Locate the PCH Trap dispatch protocol
    ///
    Status = gBS->LocateProtocol (&gEfiSmmIoTrapDispatchProtocolGuid, NULL, (VOID **) &PchIoTrap);
    ASSERT_EFI_ERROR (Status);
    if ((Status == EFI_SUCCESS) && (SaDataHob->SaIotrapSmiAddress != 0)) {
      ///
      /// Allocate 16 byte range from GCD for this IO trap address
      ///
      IotrapAddress = SaDataHob->SaIotrapSmiAddress;
      DEBUG ((EFI_D_INFO, "Iotrap address=%X\n", SaDataHob->SaIotrapSmiAddress));
      Status = gDS->AllocateIoSpace (
                      EfiGcdAllocateAddress,
                      EfiGcdIoTypeIo,
                      0,
                      0x10,
                      &IotrapAddress,
                      ImageHandle,
                      NULL
                      );
      ASSERT_EFI_ERROR (Status);
      if (Status == EFI_SUCCESS) {
        PchIoTrapContext.Type     = ReadWriteTrap;
        PchIoTrapContext.Length   = 4;
        PchIoTrapContext.Address  = SaDataHob->SaIotrapSmiAddress;
        PchIoTrapContext.Context  = NULL;
        PchIoTrapContext.MergeDisable = FALSE;
        Status = PchIoTrap->Register (
                              PchIoTrap,
                              SaIoTrapSmiCallback,
                              &PchIoTrapContext,
                              &PchIoTrapHandle
                              );
        ASSERT_EFI_ERROR (Status);
        if (Status == EFI_SUCCESS) {
          InitPcieAspmAfterOprom = SaDataHob->InitPcieAspmAfterOprom;
#if SA_PCIE_ASPM_IN_DXE == 0
          ///
          /// There is no DXE ASPM code so always executes SMM code
          ///
          InitPcieAspmAfterOprom = 1;
#endif
          if (InitPcieAspmAfterOprom == 1) {
            ///
            /// Initialize module global variables - Stepping ID and Platform Policy for runtime SMI handler
            /// Get the platform setup policy.
            ///
            Status = gBS->LocateProtocol (&gDxePlatformSaPolicyGuid, NULL, (VOID **) &DxePlatformSaPolicy);
            ASSERT_EFI_ERROR (Status);
            if (DxePlatformSaPolicy != NULL) {
              SaPcieInitPolicy (DxePlatformSaPolicy);
            }
          } else {
            ///
            /// InitPcieAspmAfterOprom was not available or disabled, make this SMI handler directly return.
            ///
            mSaSmiCallbackPhase = EnumSaSmiCallbackMax;
          }
        }
      }
    }
  }

  ///
  /// For security consideration, if this SMM driver was compiled/executed, the IOTRAP SMI handler must be registered successfully.
  /// If not, hang system here
  ///
  if (Status != EFI_SUCCESS) {
    DEBUG ((EFI_D_ERROR, "Failed to register SaIotrapSmiCallback! System halt!\n"));
    EFI_DEADLOOP ();
  }

  return EFI_SUCCESS;
}
