/** @file
 DXE IPMI Package library implement code - Set Acpi Keyboard.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/


#include <Library/DxeIpmiPackageLib.h>


/**
 This package function can set variable in GlobalNVS to disable/enable PS2 keyboard in ACPI.
 This package function will work with "Boot Option" defined in IPMI specification.
 Platform implement this function to set correct variable.

 @param[in]         EnableKeyboard      TRUE to enable keyboard, FALSE to disable keyboard.

 @retval EFI_SUCCESS                    Set Acpi Keyboard success.
 @return EFI_ERROR (Status)             Locate gEfiGlobalNvsAreaProtocolGuid Protocol error.
*/
EFI_STATUS
IpmiSetAcpiKeyboard (
  IN  BOOLEAN                           EnableKeyboard
  )
{
  //
  // Not every platform designs the variable to enable/disable PS2 keyboard
  // in ASL code. The implementation is depended on platform design.
  //
#if 0
  EFI_STATUS                            Status;
  EFI_OEM_GLOBAL_NVS_AREA               *GlobalNvsArea;
  EFI_GLOBAL_NVS_AREA_PROTOCOL          *GlobalNvsAreaProtocol;

  Status = gBS->LocateProtocol (
                  &gEfiGlobalNvsAreaProtocolGuid,
                  NULL,
                  (VOID **)&GlobalNvsAreaProtocol
                  );

  if (!EFI_ERROR (Status)) {
    GlobalNvsArea = GlobalNvsAreaProtocol->OemArea;
    GlobalNvsArea->OemKbcExistFlag = EnableKeyboard;
  }
#endif

 return EFI_SUCCESS;

}

