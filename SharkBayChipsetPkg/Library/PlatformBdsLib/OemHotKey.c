/** @file
  PlatformBdsLib

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

#include "OemHotKey.h"
//[-start-140512-IB10300110-add]//
#include <Library/WatchDogLib.h>
//[-end-140512-IB10300110-add]//

//[-start-140321-IB10300105-add]//
EFI_STATUS
BiosFlashUpdate (
  VOID
  );
//[-end-140321-IB10300105-add]//

/**
  Platform Oem HotKey Callback Function

  @param  Selection       HotKey Selection
  @param  Timeout         
  @param  BootMode        
  @param  NoBootDevices   

  @retval EFI_SUCCESS   

**/
EFI_STATUS
OemHotKeyCallback (
  IN UINT16                                    Selection,
  IN UINT16                                    Timeout,
  IN EFI_BOOT_MODE                             BootMode,
  IN BOOLEAN                                   NoBootDevices
  )
{
  EFI_STATUS                            Status;
  LIST_ENTRY                            BdsBootOptionList;

  if (Selection == FRONT_PAGE_HOT_KEY && BdsLibIsWin8FastBootActive ()) {
    BdsLibConnectUsbHID ();
    BdsLibConnectAll ();
    if (BdsLibGetBootType () != LEGACY_BOOT_TYPE) {
      BdsLibRestoreBootOrderFromPhysicalBootOrder ();
    }
  }

  Status = EFI_SUCCESS;
  Status = BdsLibStartSetupUtility (TRUE);

//[-start-140512-IB10300110-add]//
  if (Selection != NO_OPERATION) {
    WatchDogStop ();
  }
//[-end-140512-IB10300110-add]//

  switch (Selection) {

//[-start-140325-IB13080003-add]//
  case SETUP_HOT_KEY:
    //
    // Display SetupUtility
    //
    BdsLibStartSetupUtility (FALSE);

    if (NoBootDevices) { 
      PlatformBdsNoBootDevice (); 
    }

    break;

  case DEVICE_MANAGER_HOT_KEY:
    //
    // Display the Device Manager
    //
    do {
      gCallbackKey = CallDeviceManager ();
    } while (gCallbackKey == FRONT_PAGE_KEY_DEVICE_MANAGER);

    if (NoBootDevices) { 
      PlatformBdsNoBootDevice (); 
    }
    break;

  case BOOT_MANAGER_HOT_KEY:
    if (NoBootDevices) { 
      PlatformBdsNoBootDevice (); 
    }
    //
    // User chose to run the Boot Manager
    //
    CallBootManager ();
    break;

  case BOOT_MAINTAIN_HOT_KEY:
    if (BdsLibGetBootType () != LEGACY_BOOT_TYPE) {
      //
      // Display the Boot Maintenance Manager
      //
      BdsStartBootMaint ();
    } else {
      return EFI_UNSUPPORTED;
    }
    break;

  case SECURE_BOOT_HOT_KEY:
    if (BdsLibGetBootType () != LEGACY_BOOT_TYPE) {
      //
      // Display SetupUtility
      //
      CallSecureBootMgr ();
    } else {
      return EFI_UNSUPPORTED;
    }
    break;
    
//[-end-140325-IB13080003-add]//
  case FRONT_PAGE_HOT_KEY:
    if (FeaturePcdGet(PcdFrontPageSupported)) {
      Timeout = 0xFFFF;
      InitializeListHead (&BdsBootOptionList);
      BdsLibEnumerateAllBootOption (TRUE, &BdsBootOptionList);
      PlatformBdsEnterFrontPage (Timeout, TRUE);
      if (NoBootDevices) {
        PlatformBdsNoBootDevice ();
      }
      break;
    }

//[-start-130204-IB10820231-modify]//
  case MEBX_HOT_KEY:
    if (FeaturePcdGet (PcdMe5MbSupported)) {
      DisableQuietBoot ();
      gST->ConOut->EnableCursor (gST->ConOut, TRUE);
      BdsLibStartSetupUtility (TRUE);
      InvokeMebxHotKey ();
    }
    break;

  case REMOTE_ASST_HOT_KEY:
    if (FeaturePcdGet (PcdMe5MbSupported)) {
      DisableQuietBoot ();
      gST->ConOut->EnableCursor (gST->ConOut, TRUE);
      BdsLibStartSetupUtility (TRUE);
      InvokeRemoteAsstHotKey ();
    }
    break;
//[-end-130204-IB10820231-modify]//
//[-start-140321-IB10300105-add]//
  case FLASH_BOX_HOT_KEY:
    if (FeaturePcdGet (PcdHotKeyFlashSupported)) {
      DisableQuietBoot ();
      gST->ConOut->EnableCursor (gST->ConOut, TRUE);
      BiosFlashUpdate ();
    }
    break;
//[-end-140321-IB10300105-add]//

  case NO_OPERATION:
    //
    //Haven't any boot device and user doesn't press any key, show the no bootable error string
    //
    if (NoBootDevices && (BootMode != BOOT_IN_RECOVERY_MODE)) {
      PlatformBdsNoBootDevice ();
    }

    break;
  }
  
  return EFI_SUCCESS;
}  

