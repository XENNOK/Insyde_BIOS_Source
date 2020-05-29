/** @file
  Implementation file for Watchdog Timer functionality

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corporation. All Rights Reserved.
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

@copyright
  Copyright (c) 2010 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  WdtDxe.c 

@brief
  Implementation file for Watchdog Timer functionality

**/
#include <Protocol/PchReset.h>
#include <WdtCommon.h>
#include <Protocol/Wdt.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Guid/HobList.h>

VOID
EFIAPI
WdtRunBeforeOsBoot (
  IN EFI_EVENT Event,
  IN VOID      *Context
  );

EFI_STATUS
EFIAPI
WdtPchResetCallback (
  IN     PCH_RESET_TYPE           PchResetType
  );

#define TIMEOUT_AFTER_POST_MULTIPLIER 16
#define MINIMUM_TIMEOUT_AT_S4_EXIT    600 ///< 10 minutes
EFI_HANDLE                  mImageHandle;
WDT_PROTOCOL                mWdtProtocol = {
  WdtReloadAndStart,
  WdtCheckStatus,
  WdtDisable,
  WdtAllowKnownReset,
  IsWdtRequired,
  IsWdtEnabled
};

PCH_RESET_CALLBACK_PROTOCOL mPchResetCallbackProtocol = { WdtPchResetCallback };

/**
  Installs WDT protocol.
  Registers a function to be executed just before booting to OS.

  @param[in] ImageHandle          Image handle for this driver image
  @param[in] SystemTable          Pointer to the EFI System Table

  @retval EFI_SUCCESS             WDT DXE driver initialization completed successfully
**/
EFI_STATUS
EFIAPI
WdtDxeEntryPoint (
  IN  EFI_HANDLE       ImageHandle,
  IN  EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS  Status;
  EFI_EVENT   Event;

  DEBUG ((EFI_D_INFO, "(Wdt) Entry Point to WdtDxe\n"));

  mImageHandle = ImageHandle;

  Status = gBS->CreateEvent (
                  EVT_SIGNAL_EXIT_BOOT_SERVICES,
                  TPL_CALLBACK,
                  WdtRunBeforeOsBoot,
                  NULL,
                  &Event
                  );
  ASSERT_EFI_ERROR (Status);

  Status = EfiCreateEventLegacyBootEx (
            TPL_CALLBACK,
            WdtRunBeforeOsBoot,
            NULL,
            &Event
            );
  ASSERT_EFI_ERROR (Status);

  DEBUG ((EFI_D_INFO, "(Wdt) WDT event registration; Status = %r\n", Status));

  Status = gBS->InstallProtocolInterface (
                  &ImageHandle,
                  &gWdtProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mWdtProtocol
                  );
  ASSERT_EFI_ERROR (Status);

  Status = gBS->InstallProtocolInterface (
                  &ImageHandle,
                  &gPchResetCallbackProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mPchResetCallbackProtocol
                  );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}

/**
  Turns on watchdog timer just before booting to OS, if an OS application requested that.
  Clears request status.
  Uninstalls Wdt protocol to prevent other modules from interfering with actions described above.

  @param[in] Event                useless here, but required in functions invoked by events
  @param[in] Context              useless here, but required in functions invoked by events

  @retval    None
**/
VOID
EFIAPI
WdtRunBeforeOsBoot (
  IN  EFI_EVENT Event,
  IN  VOID      *Context
  )
{
  UINT32                ReloadValue;
  UINT32                Readback;
  EFI_STATUS            Status;
  EFI_BOOT_MODE         BootMode;
  EFI_PEI_HOB_POINTERS  HobList;
  WDT_HOB               *WdtHob;

  gBS->CloseEvent (Event);

  DEBUG ((EFI_D_INFO, "(Wdt) RunWdtBeforeOsBoot\n"));
  Status = gBS->UninstallProtocolInterface (
                  mImageHandle,
                  &gWdtProtocolGuid,
                  &mWdtProtocol
                  );

  Status = gBS->UninstallProtocolInterface (
                  mImageHandle,
                  &gPchResetCallbackProtocolGuid,
                  &mPchResetCallbackProtocol
                  );
  ///
  /// check boot type, there are different flows for S4/S5
  ///
  EfiGetSystemConfigurationTable (&gEfiHobListGuid, (VOID **) &HobList.Raw);
  if (HobList.Header->HobType != EFI_HOB_TYPE_HANDOFF) {
    DEBUG ((EFI_D_ERROR, "(Wdt) Handoff Hob missing!\n"));
    return;
  }

  BootMode  = HobList.HandoffInformationTable->BootMode;

  WdtHob    = GetFirstGuidHob (&mWdtHobGuid);
  if (WdtHob == NULL) {
    return;
  }

  Readback    = IoRead32 (WdtGetAddress ());
  ReloadValue = TIMEOUT_AFTER_POST_MULTIPLIER * ((Readback & B_PCH_OC_WDT_CTL_AFTER_POST) >> 16);

  if (BootMode == BOOT_ON_S4_RESUME) {
    ///
    /// S4 resume: if WDT was enabled before S0->S4 transition,
    /// then WDT must be turned on even though TimeoutValueAfterPost == 0
    /// unlike in S5->S0 flow, ToVaP is not set to zero after being consumed
    ///
    if (WdtHob->Active == 1) {
      if (ReloadValue != 0) {
        WdtReloadAndStart (ReloadValue);
      } else {
        WdtReloadAndStart (MINIMUM_TIMEOUT_AT_S4_EXIT);
      }
    } else {
      WdtDisable ();
    }
  } else if (ReloadValue != 0) {
    ///
    /// start WDT with TimeoutValueAfterPost and clear that value from register
    ///
    Readback &= ~(B_PCH_OC_WDT_CTL_AFTER_POST);
    IoWrite32 (WdtGetAddress (), Readback);
    WdtReloadAndStart (ReloadValue);
  } else {
    WdtDisable ();
  }

  return;
}

/**
  WDT call back function for Pch Reset.

  @param[in] PchResetType         Pch Reset Types which includes PowerCycle, Globalreset.

  @retval EFI_SUCCESS             The function completed successfully
  @retval Others                  All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
EFIAPI
WdtPchResetCallback (
  IN     PCH_RESET_TYPE           PchResetType
  )
{
  WdtAllowKnownReset ();
  return EFI_SUCCESS;
}
