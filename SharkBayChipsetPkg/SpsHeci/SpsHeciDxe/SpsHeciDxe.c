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

#include "SpsHeciDxe.h"
#include "SpsHeciCore.h"

#include <SpsHeciRegs.h>

///
/// Global driver data
///
HECI_INSTANCE           *mDxeHeciContext;
EFI_HANDLE              mDxeHeciHandle;

/**
  HECI driver entry point used to initialize support for the HECI DXE device.

  @param[in] ImageHandle          Standard entry point parameter.
  @param[in] SystemTable          Standard entry point parameter.

  @retval EFI_SUCCESS             Always return EFI_SUCCESS
**/
EFI_STATUS
InitializeHECI (
  IN EFI_HANDLE                   ImageHandle,
  IN EFI_SYSTEM_TABLE             *SystemTable
  )
{
  EFI_STATUS                      Status;
  BOOLEAN                         HeciInitializeError;

  mDxeHeciHandle      = ImageHandle;
  HeciInitializeError = FALSE;
  mDxeHeciContext     = AllocateZeroPool (sizeof (HECI_INSTANCE));
  ///
  /// Initialize HECI protocol pointers
  ///
  if (mDxeHeciContext != NULL) {
    mDxeHeciContext->HeciCtlr.ResetHeci    = ResetHeciInterface;
    mDxeHeciContext->HeciCtlr.SendwACK     = HeciSendwACK;
    mDxeHeciContext->HeciCtlr.ReadMsg      = HeciReceive;
    mDxeHeciContext->HeciCtlr.SendMsg      = HeciSend;
    mDxeHeciContext->HeciCtlr.InitHeci     = HeciInitialize;
    mDxeHeciContext->HeciCtlr.ReInitHeci   = HeciReInitialize;
    mDxeHeciContext->HeciCtlr.MeResetWait  = MeResetWait;
    mDxeHeciContext->HeciCtlr.GetMeStatus  = HeciGetMeStatus;
    mDxeHeciContext->HeciCtlr.GetMeMode    = HeciGetMeMode;
  }

  ///
  /// Initialize the HECI device
  ///
  Status = InitializeHeciPrivate ();
  if ((EFI_ERROR (Status)) || (mDxeHeciContext == NULL)) {
    HeciInitializeError = TRUE;
  }

  if (HeciInitializeError) {
    ///
    /// Don't install on ERR
    ///
    if (Status != EFI_NOT_READY) {
      DEBUG ((EFI_D_ERROR, "HECI not initialized - Removing devices from PCI space!\n"));
      SpsDisableAllMEDevices ();
      ///
      /// Store the current value of DEVEN for S3 resume path
      ///
    }
    SpsDeviceStatusSave();
    return EFI_SUCCESS;
  }
  ///
  /// Install the HECI interface
  ///
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mDxeHeciContext->Handle,
                  &gSpsDxeHeciProtocolGuid,
                  &mDxeHeciContext->HeciCtlr,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }


  return Status;
}
