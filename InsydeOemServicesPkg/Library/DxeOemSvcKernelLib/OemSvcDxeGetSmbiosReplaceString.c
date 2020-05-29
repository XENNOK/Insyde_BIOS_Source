/** @file
  OEM can provide a way to get replace string for OEM depend.
  It's like DMI Tool but difference.

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corp. All Rights Reserved.
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
  This service will be call by each time process SMBIOS string.
  OEM can provide a way to get replace string for OEM depend.
  
  @param[in]  Type           SMBIOS type number.
  @param[in]  FieldOffset    Field offset.
  @param[out] StrLength      String length.
  @param[out] String         String point.
  
  @retval    EFI_UNSUPPORTED       Returns unsupported by default.
  @retval    EFI_SUCCESS           N/A.
  @retval    EFI_MEDIA_CHANGED     The value of OUT parameter is changed. 
**/
EFI_STATUS
OemSvcDxeGetSmbiosReplaceString (
  IN  EFI_SMBIOS_TYPE   Type,
  IN  UINT8             FieldOffset,
  OUT UINTN            *StrLength,
  OUT CHAR8           **String
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/

  return EFI_UNSUPPORTED;
}
