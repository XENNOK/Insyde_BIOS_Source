/** @file
  H2O Debug Library

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

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/DebugComm.h>


static H2O_DEBUG_COMM_PROTOCOL    *mDebugComm = NULL;

/**
  Locate the H2ODebug Protocol.

  @return   Function pointer to the H2ODebug protocol service.
            NULL is returned if no H2ODebug protocol service is available.

**/
VOID
InternalGetH2ODebugProtocol (
  VOID
  )
{
  EFI_STATUS              Status;

  if (mDebugComm != NULL) {
    return;
  }

  //
  // Check gBS just in case H2ODebug is called before gBS is initialized.
  //
  if (gBS != NULL && gBS->LocateProtocol != NULL) {
    Status = gBS->LocateProtocol (&gH2ODebugCommProtocolGuid, NULL, (VOID**) &mDebugComm);
    if (EFI_ERROR (Status)) {
      mDebugComm = NULL;
    }
  }
}

VOID
DDTPrint (
  IN CHAR8                        *Format,
  ...
  )
{
  VA_LIST                       Args;

  InternalGetH2ODebugProtocol ();
  if (mDebugComm != NULL) {
    VA_START (Args, Format);
    mDebugComm->DDTPrint(mDebugComm, Format, Args);
    VA_END (Args);
  }
}