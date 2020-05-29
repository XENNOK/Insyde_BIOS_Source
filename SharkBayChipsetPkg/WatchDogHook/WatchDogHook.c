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


#include "WatchDogHook.h"
#include <Library/WatchDogLib.h>
#include <Library/PcdLib.h>
#include <Protocol/WatchDogHookProtocol.h>

BOOT_SERVICE_CONNECT_CONTROLLER_HOOKS mConnectControllerHook;
WATCH_DOG_HOOK_PROTOCOL               mWatchDogHook = {
  OemWatchDogHook
  };

EFI_STATUS
ConnectControllerWatchDog (
  IN  EFI_HANDLE                ControllerHandle,
  IN  EFI_HANDLE                *DriverImageHandle    OPTIONAL,
  IN  EFI_DEVICE_PATH_PROTOCOL  *RemainingDevicePath  OPTIONAL,
  IN  BOOLEAN                   Recursive
  )
{
  EFI_STATUS  Status;
  WatchDogStart (BDS_CONNECT_WATCH_DOG);
  Status = mConnectControllerHook (ControllerHandle, DriverImageHandle, RemainingDevicePath, Recursive);
  return Status;
}

EFI_STATUS
OemWatchDogHook (
  VOID
)
{
  WatchDogStop ();
  return EFI_SUCCESS;
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
WatchDogHookEntry (
  IN  EFI_HANDLE                        ImageHandle,
  IN  EFI_SYSTEM_TABLE                  *SystemTable
  )
{
  EFI_HANDLE                            Handle;  
  EFI_STATUS                            Status;
                  
  if (FeaturePcdGet (PcdH2OWatchDogSupported)) {
    if (gBS == NULL) {
      return EFI_SUCCESS;
    }

    mConnectControllerHook = gBS->ConnectController;
    gBS->ConnectController = ConnectControllerWatchDog;
  }


  //
  // Install Watch Dog protocol for Oem hook function
  //
  Status = gBS->InstallProtocolInterface (
                                      &Handle,
                                      &gWatchDogHookProtocolGuid,
                                      EFI_NATIVE_INTERFACE,
                                      &mWatchDogHook
                                      );
  
 
  return Status;
}


