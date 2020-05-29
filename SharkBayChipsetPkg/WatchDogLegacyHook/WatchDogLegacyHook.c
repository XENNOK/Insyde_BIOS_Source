/** @file


;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/

#include <WatchDogLegacyHook.h>
#include <Library/WatchDogLib.h>
#include <Library/PcdLib.h>
#include <Protocol/Legacy8259.h>

EFI_LEGACY_8259_SET_MODE mLegacy8259SetMode;
BOOLEAN                  mScanOprom = FALSE;

EFI_STATUS
EFIAPI
WatchDogLegacy8259SetModeHook (
  IN  EFI_LEGACY_8259_PROTOCOL  *This,
  IN  EFI_8259_MODE             Mode,
  IN  UINT16                    *Mask,
  IN  UINT16                    *EdgeLevel
  )
{
  if (Mode == Efi8259LegacyMode) {
    if (PcdGet8(PcdH2OWatchDogMask) & 0x2) { // bit 1
      WatchDogStart (OPROM_WATCH_DOG);
    }
    else {
      WatchDogStop();
    }
    mScanOprom = TRUE;
  }
  else {
    if (mScanOprom) {
      WatchDogStart (BDS_WATCH_DOG);
    }
    else {
      WatchDogStart (DXE_WATCH_DOG);
    }
    mScanOprom = FALSE;
  }

  return mLegacy8259SetMode (
           This,
           Mode,
           Mask,
           EdgeLevel
           );
}

/**
 Entrypoint of this module.

 @param[in]         ImageHandle         The firmware allocated handle for the EFI image.
 @param[in]         SystemTable         A pointer to the EFI System Table.

 @retval EFI_SUCCESS                    Install protocol success.
 @retval other                               Install protocol fail.
*/
EFI_STATUS
EFIAPI
WatchDogLegacyHookEntry (
  IN  EFI_HANDLE                        ImageHandle,
  IN  EFI_SYSTEM_TABLE                  *SystemTable
  )
{
  EFI_LEGACY_8259_PROTOCOL              *Legacy8259;
                  
  if (FeaturePcdGet (PcdH2OWatchDogSupported)) {
    //
    // Replace the Legacy8259 SetMode
    //
    gBS->LocateProtocol (
           &gEfiLegacy8259ProtocolGuid,
           NULL,
           (VOID **)&Legacy8259
           );
    
    mLegacy8259SetMode  = Legacy8259->SetMode;
    Legacy8259->SetMode = WatchDogLegacy8259SetModeHook;
  }

  return EFI_SUCCESS;
}


