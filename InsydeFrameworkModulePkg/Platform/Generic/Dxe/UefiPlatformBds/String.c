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

EFI_HII_HANDLE gStringPackHandle;

EFI_GUID mBdsStringPackGuid = {
  0x7bac95d3, 0xddf, 0x42f3, 0x9e, 0x24, 0x7c, 0x64, 0x49, 0x40, 0x37, 0x9a
};

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
  EFI_SUCCESS - String support initialize success.

--*/
{
  EFI_STATUS                   Status;
  EFI_HANDLE                   DriverHandle;
  EFI_HII_PACKAGE_LIST_HEADER  *PackageList;
  EFI_HII_DATABASE_PROTOCOL    *HiiDatabase;

  Status = gBS->LocateProtocol (&gEfiHiiDatabaseProtocolGuid, NULL, &HiiDatabase);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Create driver handle used by HII database
  //
  Status = CreateHiiDriverHandle (&DriverHandle);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  PackageList = PreparePackageList (1, &mBdsStringPackGuid, &BdsStrings);
  ASSERT (PackageList != NULL);

  Status = HiiDatabase->NewPackageList (
                          HiiDatabase,
                          PackageList,
                          DriverHandle,
                          &gStringPackHandle
                          );

    UpdateBvdtToHii (gStringPackHandle);
    
  gBS->FreePool (PackageList);
  return Status;
}

CHAR16 *
GetStringById (
  IN  EFI_STRING_ID   Id
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
  CHAR16 *String;

  String = NULL;
  GetStringFromHandle (gStringPackHandle, Id, &String);

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
  UINTN                StrSize;
  CHAR16               Str[BVDT_MAX_STR_SIZE];
  EFI_STATUS           Status;

  StrSize = BVDT_MAX_STR_SIZE;
  Status = GetBvdtInfo ((BVDT_TYPE) BvdtBiosVer, &StrSize, Str);
  IfrLibSetString (HiiHandle, STRING_TOKEN(STR_MISC_BIOS_VERSION), Str);

  StrSize = BVDT_MAX_STR_SIZE;
  Status = GetBvdtInfo ((BVDT_TYPE) BvdtProductName, &StrSize, Str);
  IfrLibSetString (HiiHandle, STRING_TOKEN(STR_MISC_SYSTEM_PRODUCT_NAME), Str);

  StrSize = BVDT_MAX_STR_SIZE;
  Status = GetBvdtInfo ((BVDT_TYPE) BvdtCcbVer, &StrSize, Str);
  IfrLibSetString (HiiHandle, STRING_TOKEN(STR_CCB_VERSION), Str);
}

EFI_STATUS
GetProducerString (
  IN      EFI_GUID                  *ProducerGuid,
  IN      EFI_STRING_ID             Token,
  OUT     CHAR16                    **String
  )
/*++

Routine Description:
  Acquire the string associated with the ProducerGuid and return it.

Arguments:
  ProducerGuid - The Guid to search the HII database for
  Token - The token value of the string to extract
  String - The string that is extracted

Returns:
  EFI_SUCCESS - The function returns EFI_SUCCESS always.

--*/
{
  EFI_STATUS      Status;

  Status = GetStringFromToken (ProducerGuid, Token, String);
  if (EFI_ERROR (Status)) {
    *String = GetStringById (STRING_TOKEN (STR_MISSING_STRING));
  }

  return EFI_SUCCESS;
}

