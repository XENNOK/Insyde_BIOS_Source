/** @file
  Called by LegacyBiosPlatform when legacy bios is going to install option Rom.
  Oem could custom what need to do in this time.

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

#include "DxeOemServicesThunk.h"
#include <Library/DxeOemSvcKernelLib.h>

/**
  Called by LegacyBiosPlatform when legacy bios is going to install option Rom.
  Oem could custom what need to do in this time. Or return an action code for
  LegacyBiosPlatform to do common actions. 

  @param  DeviceHandle          Handle of device OPROM is associated with
  @param  ForceSwitchTextMode   Initial value contains the upper level's switch text
                                mode policy. Set to TRUE if it requires to switch to
                                text mode.

  @retval EFI_SUCCESS           Always returns success.
  @retval Others                Base on OEM design.
**/
EFI_STATUS 
OemSvcInstallPciRomSwitchTextModeThunk (
  IN OEM_SERVICES_PROTOCOL              *OemServices,
  IN UINTN                              NumOfArgs,
  ...
  )
{
  VA_LIST                               Marker;
  EFI_HANDLE                            DeviceHandle;
  BOOLEAN                               *ForceSwitchTextMode;
  EFI_STATUS                            Status;

  VA_START (Marker, NumOfArgs);

  DeviceHandle          = VA_ARG (Marker, EFI_HANDLE);
  ForceSwitchTextMode   = VA_ARG (Marker, BOOLEAN*);

  VA_END (Marker);
  
  //
  // make a call to DxeOemSvcKernelLib internally
  //
  Status = OemSvcInstallPciRomSwitchTextMode (
             DeviceHandle,
             ForceSwitchTextMode
             );

  return Status;
}

