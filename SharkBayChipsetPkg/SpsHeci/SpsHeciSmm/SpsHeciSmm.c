/** @file

  HECI driver

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "SpsHeciSmm.h"
#include "SpsHeciCore.h"

#include <SpsHeciRegs.h>

///
/// Global driver data
///
HECI_INSTANCE_SMM       *mSmmHeciContext;
EFI_HANDLE              mSmmHeciHandle;

/**
  HECI driver entry point used to initialize support for the HECI SMM device.

  @param[in] ImageHandle          Standard entry point parameter.
  @param[in] SystemTable          Standard entry point parameter.

  @retval EFI_SUCCESS             Always return EFI_SUCCESS
**/
EFI_STATUS
InitializeHECI (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS                Status;
  EFI_SMM_RUNTIME_PROTOCOL  *SmmRT;

  ///
  /// Create database record and add to database
  ///
  Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    sizeof (HECI_INSTANCE_SMM),
                    (VOID **)&mSmmHeciContext
                    );
  DEBUG ( (EFI_D_INFO, "[SMM HECI] INFO: SMM Allocate Pool ==> (%r)\n", Status));
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }


  mSmmHeciContext->Handle = ImageHandle;

  ///
  /// Initialize HECI protocol pointers
  ///
  mSmmHeciContext->HeciCtlr.ResetHeci    = ResetHeciInterface;
  mSmmHeciContext->HeciCtlr.SendwACK     = HeciSendwACK;
  mSmmHeciContext->HeciCtlr.ReadMsg      = HeciReceive;
  mSmmHeciContext->HeciCtlr.SendMsg      = HeciSend;
  mSmmHeciContext->HeciCtlr.InitHeci     = HeciInitialize;
  mSmmHeciContext->HeciCtlr.ReInitHeci   = HeciReInitialize;
  mSmmHeciContext->HeciCtlr.MeResetWait  = MeResetWait;
  mSmmHeciContext->HeciCtlr.GetMeStatus  = HeciGetMeStatus;
  mSmmHeciContext->HeciCtlr.GetMeMode    = HeciGetMeMode;

  ///
  /// Initialize the HECI device
  ///
  Status = InitializeHeciPrivate ();
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ///
  /// Install the HECI interface
  ///
  mSmmHeciHandle = NULL;
  Status = gSmst->SmmInstallProtocolInterface (
                    &mSmmHeciHandle,
                    &gSpsSmmHeciProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &( mSmmHeciContext->HeciCtlr )
                    );
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mSmmHeciContext->Handle,
                  &gSpsSmmHeciProtocolGuid,
                  &mSmmHeciContext->HeciCtlr,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  //
  // For ECP compatibility, skip eroror status check
  //
  Status = gBS->LocateProtocol ( &gEfiSmmRuntimeProtocolGuid, NULL, (VOID **)&SmmRT);
  DEBUG ( (EFI_D_INFO, "[SMM HECI] INFO: Locate Protocol SmmRT ==> (%r)\n", Status));
  if (EFI_ERROR ( Status )) {
    DEBUG ( (EFI_D_ERROR, "[SMM HECI] ERROR: Locate Protocol SmmRT Failure!(%r)\n", Status));
  } else {
    Status = SmmRT->InstallProtocolInterface (
               &mSmmHeciContext->Handle,
               &gSpsSmmHeciProtocolGuid,
               EFI_NATIVE_INTERFACE,
               &( mSmmHeciContext->HeciCtlr )
               );
    DEBUG ( (EFI_D_INFO, "[SMM HECI] INFO: SMM Install Protocol ==> (%r)\n", Status));
  }

  return EFI_SUCCESS;
}
