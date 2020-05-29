/** @file
  Called by LegacyBiosPlatform when legacy bios is going to install option Rom.
  Oem could custom what need to do in this time.

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

#include <Library/DxeOemSvcKernelLib.h>

/**
  Called by LegacyBiosPlatform when legacy bios is going to install option Rom.
  Oem could custom what need to do in this time. Or return an action code for
  LegacyBiosPlatform to do common actions. 

  @param[in, out]  DeviceHandle          Handle of device OPROM is associated with
  @param[in, out]  ForceSwitchTextMode   Initial value contains the upper level's switch text
                                         mode policy. Set to TRUE if it requires to switch to
                                         text mode.

  @retval          EFI_UNSUPPORTED       Returns unsupported by default.
  @retval          EFI_SUCCESS           The service is customized in the project.
  @retval          EFI_MEDIA_CHANGED     The value of IN OUT parameter is changed. 
  @retval          Others                Base on OEM design.
**/
EFI_STATUS 
OemSvcInstallPciRomSwitchTextMode (
  IN OUT EFI_HANDLE                            DeviceHandle,
  IN OUT BOOLEAN                               *ForceSwitchTextMode
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/
  
  return EFI_UNSUPPORTED;
}

