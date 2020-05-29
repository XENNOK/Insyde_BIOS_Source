/** @file
  This is the main DXE file for TXT.  It represents an abstract outline of the
  steps required during DXE for enabling TXT.  Each individual step is further
  abstracted behind a function call interface.  This is intended to minimize 
  the need to modify this file when porting TXT to future platforms. 
  
;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
/**
  This file contains an 'Intel Peripheral Driver' and uniquely  
  identified as "Intel Reference Module" and is                 
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
**/
/**

Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


@file:

  TxtDxe.c

@brief:

This is the main DXE file for TXT.  It represents an abstract outline of the
steps required during DXE for enabling TXT.  Each individual step is further
abstracted behind a function call interface.  This is intended to minimize 
the need to modify this file when porting TXT to future platforms.  

**/

#include <Txt.h>
#include "TxtDxeLib.h"
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiLib.h>

TXT_DXE_LIB_CONTEXT mTxtDxeCtx;

/**

  This function gets registered as a callback to run the SCHECK function
  from the TXT BIOS ACM as a result of Boot Events.

  @param[in] Event     - A pointer to the Event that triggered the callback.
  @param[in] Context   - A pointer to private data registered with the callback function.

  @retval EFI_SUCCESS   - Always.

**/
EFI_STATUS
EFIAPI
ScheckCallback (
  IN  EFI_EVENT                Event,
  IN  VOID                     *Context
  )

{
  DEBUG ((EFI_D_INFO, "TXTDXE::Running of DoScheck\n"));

  DoScheck (&mTxtDxeCtx);
  ///
  /// Closed the event to avoid call twice when launch shell
  ///
  gBS->CloseEvent (Event);

  return EFI_SUCCESS;
}

/**

  This is the entry point to the TXT DXE Driver.  This routine checks to see if
  the platform should be configured for TXT and if so, configures the platform
  by reserving and initializing TXT Configuration Space and TXT Device Memory and
  registering a callback to run SCHECK from the TXT BIOS ACM prior to boot.

  If the platform should not be configured for TXT, this routine checks the
  establishment bit in the TPM and resets it if it is asserted.

  @param[in] ImageHandle       - A handle for this module
  @param[in] SystemTable       - A pointer to the EFI System Table

  @retval EFI_SUCCESS   - If TXT initialization succeed
  @retval EFI_UNLOAD_IMAGE   - If TXT criterias are not met

**/
EFI_STATUS
EFIAPI
DriverEntry (
  IN  EFI_HANDLE               ImageHandle,
  IN  EFI_SYSTEM_TABLE         *SystemTable
  )

{
  EFI_STATUS  Status;
  EFI_EVENT   ReadyToBootEvent;
  ///
  /// Initialize the platform specific code
  ///
  Status = InitializeTxtDxeLib (ImageHandle, SystemTable, &mTxtDxeCtx);
  ///
  /// If failure - assume TXT is not enabled
  ///
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "TXTDXE::InitializeTxtDxeLib failed.... Unloading\n"));
    return EFI_UNLOAD_IMAGE;
  }
  ///
  /// If ESTS.TXTRESET bit is set, skip all other functions since
  /// attempt to execute GETSEC will hang system. Skipping allows to
  /// boot to OS and let MLE assess situation.
  ///
  if (IsTxtResetSet (&mTxtDxeCtx)) {
    DEBUG ((EFI_D_ERROR, "TXTDXE::TXT_RESET bit is set.... Unloading\n"));
    return EFI_UNLOAD_IMAGE;
  }
  ///
  /// If TXT is enabled, configure platform appropriately.
  /// Code assumes that if TXT is enabled by CPU driver than all checks
  /// are passed, i.e. TPM is present, CPU and CS are TXT capable.
  ///
  ///
  /// Add to check CPU TXT capable in case CPU drivers do not check additional requirements
  ///
  if ((mTxtDxeCtx.TxtInfoData->ChipsetIsTxtCapable) && IsTxtProcessor () && IsTxtEnabled (&mTxtDxeCtx)) {
    DEBUG ((EFI_D_INFO, "TXTDXE::TXT Enabled\n"));
    ///
    /// Mark TXT Config Space as System Reserved in Memory Map
    ///
    ReserveTxtConfigSpace (&mTxtDxeCtx);

    ///
    /// Allocate and Initialize TXT Device Memory
    ///
    Status = SetupTxtDeviceMemory (&mTxtDxeCtx);
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "TXTDXE::SetupTxtDeviceMemory failed.... Unloading\n"));
      return EFI_UNLOAD_IMAGE;
    }
    ///
    /// Create callback to run SCHECK on a Legacy Boot event
    ///
    Status = EfiCreateEventReadyToBootEx (
              TPL_CALLBACK,
              ScheckCallback,
              NULL,
              &ReadyToBootEvent
              );
    ASSERT_EFI_ERROR (Status);

  } else {
    ///
    /// TXT is not enabled, so make sure TPM Establishment
    /// bit is de-asserted
    ///
    DEBUG ((EFI_D_INFO, "TXTDXE::TXT Disabled\n"));

    if (IsTxtEstablished (&mTxtDxeCtx)) {
      ///
      /// We can invoke BIOS ACM function only if CS and CPU are TXT
      /// capable
      ///
      if ((mTxtDxeCtx.TxtInfoData->ChipsetIsTxtCapable) &&
          IsTxtProcessor () &&
          !(mTxtDxeCtx.TxtInfoData->Flags & TPM_INIT_FAILED)
          ) {
        DEBUG ((EFI_D_INFO, "TXTDXE::Resetting TPM Establishment bit\n"));
        ResetTpmEstBit (&mTxtDxeCtx);
      }
    }
    ///
    /// Reset AUX
    ///
    Status = ResetTpmAux (&mTxtDxeCtx);
    ASSERT_EFI_ERROR (Status);
  }

  return EFI_SUCCESS;
}
