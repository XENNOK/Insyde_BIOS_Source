/** @file
  Provide OEM to define the updatable SMBIOS string when use the DMI tool.

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

#include <Library/SmmOemSvcKernelLib.h>

/**
  This service provides OEM to define the updatable SMBIOS string when use the DMI tool.
  OEM define updatable string by modifying the UpdateableString Table.

  @param[out]  *UpdateableStringCount    The number of UpdateableString instances.
  @param[out]  **mUpdatableStrings       Pointer to UpdateableString Table.

  @retval      EFI_UNSUPPORTED           Returns unsupported by default.
  @retval      EFI_SUCCESS               Get Updatable string success.
  @retval      EFI_MEDIA_CHANGED         The value of IN OUT parameter is changed. 
  @retval      Others                    Base on OEM design.
**/
EFI_STATUS
OemSvcSmmInstallPnpStringTable (
  OUT UINTN                                 *UpdateableStringCount,
  OUT DMI_UPDATABLE_STRING                  **mUpdatableStrings
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/
  
  return EFI_UNSUPPORTED;
}
