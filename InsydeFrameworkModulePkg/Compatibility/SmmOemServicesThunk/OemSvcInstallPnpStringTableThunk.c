/** @file
  Provide OEM to define the updatable SMBIOS string when use the DMI tool.

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "SmmOemServicesThunk.h"
#include <Library/SmmOemSvcKernelLib.h>

/**
  This service provides OEM to define the updatable SMBIOS string when use the DMI tool.
  OEM define updatable string by modifying the UpdateableString Table.

  @param  *UpdateableStringCount    The number of UpdateableString instances.
  @param  **mUpdatableStrings       Pointer to UpdateableString Table.

  @retval EFI_SUCCESS               Get Updatable string success.
  @retval Others                    Base on OEM design.
**/
EFI_STATUS
OemSvcInstallPnpStringTableThunk (
  IN SMM_OEM_SERVICES_PROTOCOL          *OemServices,
  IN  UINTN                             NumOfArgs,
  ...
  )
{
  VA_LIST                               Marker;
  UINTN                                 *UpdateableStringCount;
  DMI_UPDATABLE_STRING                  **mUpdatableStrings;
  EFI_STATUS                            Status;

  VA_START (Marker, NumOfArgs);
  UpdateableStringCount = VA_ARG (Marker, UINTN *);
  mUpdatableStrings     = VA_ARG (Marker, DMI_UPDATABLE_STRING **);
  VA_END (Marker);

  //
  // make a call to SmmOemSvcKernelLib internally
  //
  Status = OemSvcSmmInstallPnpStringTable (
             UpdateableStringCount,
             mUpdatableStrings
             );

  return Status;
}
