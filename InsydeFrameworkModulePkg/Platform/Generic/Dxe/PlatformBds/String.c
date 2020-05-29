//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++

Copyright (c) 2004 - 2005, Intel Corporation
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  string.c

Abstract:

  String support

--*/

#include "Bds.h"
#include "Language.h"
#include "String.h"

extern UINT8  BdsStrings[];

EFI_GUID      gBdsStringPackGuid = { 0x7bac95d3, 0xddf, 0x42f3, 0x9e, 0x24, 0x7c, 0x64, 0x49, 0x40, 0x37, 0x9a };

EFI_STATUS
InitializeStringSupport (
  VOID
  )
/*++

Routine Description:

  Initialize HII global accessor for string support

Arguments:
  None

Returns:
  String from ID.

--*/
{
  EFI_STATUS        Status;
  EFI_HII_PACKAGES  *PackageList;
  //
  // There should only ever be one HII protocol
  //
  Status = gBS->LocateProtocol (
                  &gEfiHiiProtocolGuid,
                  NULL,
                  &Hii
                  );
  if (!EFI_ERROR (Status)) {
    PackageList = PreparePackages (1, &gBdsStringPackGuid, BdsStrings);
    Status      = Hii->NewPack (Hii, PackageList, &gStringPackHandle);
    UpdateBvdtToHii (gStringPackHandle);
    gBS->FreePool (PackageList);
  }

  return Status;
}

CHAR16 *
GetStringById (
  IN  STRING_REF   Id
  )
/*++

Routine Description:

  Get string by string id from HII Interface

Arguments:

  Id       - String ID.

Returns:

  CHAR16 * - String from ID.
  NULL     - If error occurs.

--*/
{
  CHAR16      *String;
  UINTN       StringLength;
  EFI_STATUS  Status;

  //
  // Set default string size assumption at no more than 256 bytes
  //
  StringLength  = 0x100;

  String        = EfiLibAllocateZeroPool (StringLength);
  if (String == NULL) {
    //
    // If this happens, we are oh-so-dead, but return a NULL in any case.
    //
    return NULL;
  }
  //
  // Get the current string for the current Language
  //
  Status = Hii->GetString (Hii, gStringPackHandle, Id, TRUE, NULL, &StringLength, String);
  if (EFI_ERROR (Status)) {
    if (Status == EFI_BUFFER_TOO_SMALL) {
      //
      // Free the old pool
      //
      gBS->FreePool (String);

      //
      // Allocate new pool with correct value
      //
      String = EfiLibAllocatePool (StringLength);
      ASSERT (String != NULL);

      Status = Hii->GetString (Hii, gStringPackHandle, Id, TRUE, NULL, &StringLength, String);
      if (!EFI_ERROR (Status)) {
        return String;
      }
    }

    return NULL;
  }

  return String;
}

VOID
UpdateBvdtToHii (
  IN EFI_HII_HANDLE     HiiHandle
  )
/*++

Routine Description:

  Update all BVDT data (which are from .fd file) into HiiDatabase.

Arguments:

  HiiHandle - target EFI_HII_HANDLE instance

Returns:

  VOID

--*/
{
  EFI_HII_PROTOCOL     *Hii;
  EFI_STRING           Lang;
  STRING_REF           TokenToUpdate;
  UINTN                StrSize;
  CHAR16               Str[BVDT_MAX_STR_SIZE];
  EFI_STATUS           Status;

  Status = gBS->LocateProtocol (
                  &gEfiHiiProtocolGuid,
                  NULL,
                  &Hii
                  );
  if (EFI_ERROR (Status)) {
    return;
  }

  STRING_TOKEN(STR_MISC_BIOS_VERSION);
  STRING_TOKEN(STR_MISC_SYSTEM_PRODUCT_NAME);
  STRING_TOKEN(STR_CCB_VERSION);    
  
  Hii->GetPrimaryLanguages (Hii, HiiHandle, &Lang);
  
  TokenToUpdate = (STRING_REF) STR_MISC_BIOS_VERSION;
  StrSize = BVDT_MAX_STR_SIZE;
  Status = GetBvdtInfo ((BVDT_TYPE) BvdtBiosVer, &StrSize, Str);
  if (!EFI_ERROR(Status)) {
    Hii->NewString (Hii, Lang, HiiHandle, &TokenToUpdate, Str);
  }
  
  TokenToUpdate = (STRING_REF) STR_MISC_SYSTEM_PRODUCT_NAME;
  StrSize = BVDT_MAX_STR_SIZE;
  Status = GetBvdtInfo ((BVDT_TYPE) BvdtProductName, &StrSize, Str);
  if (!EFI_ERROR(Status)) {
    Hii->NewString (Hii, Lang, HiiHandle, &TokenToUpdate, Str);
  }
  
  TokenToUpdate = (STRING_REF) STR_CCB_VERSION;
  StrSize = BVDT_MAX_STR_SIZE;
  Status = GetBvdtInfo ((BVDT_TYPE) BvdtCcbVer, &StrSize, Str);
  if (!EFI_ERROR(Status)) {
    Hii->NewString (Hii, Lang, HiiHandle, &TokenToUpdate, Str);
  }  
}
