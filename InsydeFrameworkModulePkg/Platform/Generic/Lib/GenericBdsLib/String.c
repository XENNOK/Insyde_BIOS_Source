//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++

Module Name:

  String.c

Abstract:

  String support for GenericBdsLib

--*/

#include "GenericBdsLib.h"
#include "GenericBdsLibStrDefs.h"

extern UINT8               GenericBdsLibStrings[];
EFI_GUID                   mBdsLibStringPackGuid = {
                             0x3b4d9b23, 0x95ac, 0x44f6, 0x9f, 0xcd, 0xe, 0x95, 0x94, 0x58, 0x6c, 0x72
                             };
EFI_HII_HANDLE             mBdsLibStringPackHandle;
EFI_HII_STRING_PROTOCOL    *mBdsLibHiiString;
BOOLEAN                    mInitializedStringSupport;


EFI_STATUS
BdsLibInitializeStringSupport (
  VOID
  )
/*++

Routine Description:

  Initialize HII string protocol and package list.

Arguments:

  None

Returns:

  EFI_SUCCESS    - String support initialize success.
  Other          - Locate protocol fail or new package list fail

--*/
{
  EFI_STATUS                   Status;
  EFI_HANDLE                   DriverHandle;
  EFI_HII_PACKAGE_LIST_HEADER  *PackageList;
  EFI_HII_DATABASE_PROTOCOL    *HiiDatabase;

  Status = gBS->LocateProtocol (&gEfiHiiStringProtocolGuid, NULL, &mBdsLibHiiString);
  if (EFI_ERROR (Status)) {
    return Status;
  }

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

  PackageList = PreparePackageList (1, &mBdsLibStringPackGuid, &GenericBdsLibStrings);
  ASSERT (PackageList != NULL);

  Status = HiiDatabase->NewPackageList (
                          HiiDatabase,
                          PackageList,
                          DriverHandle,
                          &mBdsLibStringPackHandle
                          );    
  gBS->FreePool (PackageList);

  mInitializedStringSupport = TRUE;

  return Status;
}


CHAR16 *
BdsLibGetStringById (
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

  if (!mInitializedStringSupport) {
    BdsLibInitializeStringSupport ();
  }

  String = NULL;
  GetStringFromHandle (mBdsLibStringPackHandle, Id, &String);

  return String;
}


STATIC
EFI_STATUS
GetTargetStrToken (
  IN  BOOT_DEVICE_TYPE         BootDevType,
  OUT EFI_STRING_ID            *StrToken
  )
/*++

Routine Description:

  Get string ID of boot device type.

Arguments:

  BootDevType    - Requested boot device type.
  StrToken       - The String ID.

Returns:

  EFI_INVALID_PARAMETER   - Invalid boot device type or StrToken is NULL.
  EFI_NOT_FOUND           - String ID not found.
  EFI_SUCCESS             - Get string ID success.

--*/
{
  EFI_STATUS                   Status;

  if (BootDevType >= BOOT_DEVICE_TYPE_MAX || StrToken == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = EFI_SUCCESS;

  switch (BootDevType) {

  case BOOT_DEVICE_TYPE_FLOPPY:
    *StrToken =  STRING_TOKEN (STR_DESCRIPTION_FLOPPY);
    break;

  case BOOT_DEVICE_TYPE_CD_DVD:
    *StrToken =  STRING_TOKEN (STR_DESCRIPTION_CD_DVD);
    break;

  case BOOT_DEVICE_TYPE_USB:
    *StrToken =  STRING_TOKEN (STR_DESCRIPTION_USB);
    break;

  case BOOT_DEVICE_TYPE_SCSI:
    *StrToken =  STRING_TOKEN (STR_DESCRIPTION_SCSI);
    break;

  case BOOT_DEVICE_TYPE_MISC:
    *StrToken =  STRING_TOKEN (STR_DESCRIPTION_MISC);
    break;

  case BOOT_DEVICE_TYPE_HDD:
    *StrToken =  STRING_TOKEN (STR_DESCRIPTION_HARDDRIVE);
    break;

  case BOOT_DEVICE_TYPE_NETWORK:
    *StrToken =  STRING_TOKEN (STR_DESCRIPTION_NETWORK);
    break;

  case BOOT_DEVICE_TYPE_SHELL:
    *StrToken =  STRING_TOKEN (STR_DESCRIPTION_SHELL);
    break;

  default:
    Status = EFI_NOT_FOUND;
    break;
  }

  return Status;
}


EFI_STATUS
BdsLibUpdateBootDevTypeStr (
  IN  BOOT_DEVICE_TYPE         BootDevType,
  IN  EFI_HII_HANDLE           StrPackHandle,
  IN  EFI_STRING_ID            StrToken
  )
/*++

Routine Description:

  Update the string of boot device type.

Arguments:

  BootDevType    - Boot device type which will be updated.
  StrPackHandle  - The HII handle of package list.
  StrToken       - The String ID.

Returns:

  EFI_INVALID_PARAMETER   - Invalid boot device type.
  EFI_ABORTED             - Hii string protocol not found.
  EFI_NOT_FOUND           - Not found for string token of boot device type or supported language.
  EFI_SUCCESS             - Update the string of boot device type success.
  Other                   - Update the string of boot device type fail.

--*/
{
  EFI_STATUS                   Status;
  EFI_STRING_ID                TargetToken;
  CHAR8                        *Languages;
  CHAR8                        *LangStrings;
  CHAR8                        Lang[RFC_3066_ENTRY_SIZE];
  CHAR16                       *String;
  UINTN                        StringSize;

  if (!mInitializedStringSupport) {
    BdsLibInitializeStringSupport ();
  }

  if (BootDevType >= BOOT_DEVICE_TYPE_MAX) {
    return EFI_INVALID_PARAMETER;
  }

  if (mBdsLibHiiString == NULL) {
    return EFI_ABORTED;
  }

  Status = GetTargetStrToken (BootDevType, &TargetToken);
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  Languages = GetSupportedLanguages (StrPackHandle);
  if (Languages == NULL) {
    return EFI_NOT_FOUND;
  }

  String = NULL;
  LangStrings = Languages;
  while (*LangStrings != 0) {
    GetNextLanguage (&LangStrings, Lang);

    StringSize = 0;
    Status = mBdsLibHiiString->GetString (
                                 mBdsLibHiiString,
                                 Lang,
                                 StrPackHandle,
                                 StrToken,
                                 String,
                                 &StringSize,
                                 NULL
                                 );
    if (Status != EFI_BUFFER_TOO_SMALL) {
      continue;
    }
    
    String = EfiLibAllocatePool (StringSize);
    Status = mBdsLibHiiString->GetString (
                                 mBdsLibHiiString,
                                 Lang,
                                 StrPackHandle,
                                 StrToken,
                                 String,
                                 &StringSize,
                                 NULL
                                 );
    if (!EFI_ERROR (Status) && EfiStrCmp (String, L"")) {
      Status = mBdsLibHiiString->SetString (
                                   mBdsLibHiiString,
                                   mBdsLibStringPackHandle,
                                   TargetToken,
                                   Lang,
                                   String,
                                   NULL
                                   );
      ASSERT_EFI_ERROR (Status);
    }
    
    gBS->FreePool (String);
  }

  gBS->FreePool (Languages);

  return Status;
}

