//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#include "SetupUtilityLibCommon.h"
#include "ChipsetLib.h"


LANGUAGE_DATA_BASE  *mSuuportLangTable;

CHAR16 *
SetupUtilityLibGetTokenStringByLanguage (
  IN EFI_HII_HANDLE                             HiiHandle,
  IN STRING_REF                                 Token,
  IN CHAR8                                      *LanguageString
  )
/*++

Routine Description:

  Given a token, return the string.

Arguments:

  Token           - the string reference
  HiiHandle       - the handle the token is located
  LanguageString  - indicate what language string we want to get. if this is a
                    NULL pointer, using the current language setting to get string

Returns:

  *CHAR16 - Returns the string corresponding to the token
  NULL    - Cannot get string from Hii database

--*/
{
  CHAR16                                      *Buffer;
  UINTN                                       BufferLength;
  EFI_STATUS                                  Status;
  EFI_HII_STRING_PROTOCOL                     *HiiString;
  SETUP_UTILITY_BROWSER_DATA                  *SuBrowser;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return NULL;
  }

  HiiString = SuBrowser->HiiString;
  //
  // Set default string size assumption at no more than 256 bytes
  //
  BufferLength = 0x100;

  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  BufferLength,
                  &Buffer
                  );
  if (EFI_ERROR (Status)) {
    return NULL;
  }
  EfiZeroMem (Buffer, BufferLength);
  Status = HiiString->GetString (
                        HiiString,
                        LanguageString,
                        HiiHandle,
                        Token,
                        Buffer,
                        &BufferLength,
                        NULL
                        );
  if (EFI_ERROR (Status)) {
    if (Status == EFI_BUFFER_TOO_SMALL) {
      //
      // Free the old pool
      //
      gBS->FreePool (Buffer);

      //
      // Allocate new pool with correct value
      //
      gBS->AllocatePool (
             EfiBootServicesData,
             BufferLength,
             &Buffer
             );
      Status = HiiString->GetString (
                            HiiString,
                            LanguageString,
                            HiiHandle,
                            Token,
                            Buffer,
                            &BufferLength,
                            NULL
                            );
      if (!EFI_ERROR (Status)) {
        //
        // return searched string
        //
        return Buffer;
      }
    }
    //
    // Cannot find string, free buffer and return NULL pointer
    //
    gBS->FreePool (Buffer);
    Buffer = NULL;
    return Buffer;
  }
  //
  // return searched string
  //
  return Buffer;
}

VOID *
SetupUtilityLibGetVariableAndSize (
  IN  CHAR16              *Name,
  IN  EFI_GUID            *VendorGuid,
  OUT UINTN               *VariableSize
  )
/*++

Routine Description:

  Read the EFI variable (VendorGuid/Name) and return a dynamically allocated
  buffer, and the size of the buffer. If failure return NULL.

Arguments:

  Name       - String part of EFI variable name

  VendorGuid - GUID part of EFI variable name

  VariableSize - Returns the size of the EFI variable that was read

Returns:

  Dynamically allocated memory that contains a copy of the EFI variable.
  Caller is responsible freeing the buffer.

  NULL - Variable was not read

--*/
{
  EFI_STATUS  Status;
  UINTN       BufferSize;
  VOID        *Buffer;

  Buffer = NULL;

  //
  // Pass in a zero size buffer to find the required buffer size.
  //
  BufferSize  = 0;

  Status      = gRT->GetVariable (Name, VendorGuid, NULL, &BufferSize, Buffer);
  if (Status == EFI_BUFFER_TOO_SMALL) {
    //
    // Allocate the buffer to return
    //

    Buffer = EfiLibAllocateZeroPool (BufferSize);
    if (Buffer == NULL) {
      return NULL;
    }

    //
    // Read variable into the allocated buffer.
    //

    Status = gRT->GetVariable (Name, VendorGuid, NULL, &BufferSize, Buffer);
    if (EFI_ERROR (Status)) {
      BufferSize = 0;
      gBS->FreePool (Buffer);
      Buffer = NULL;
    }
  }

  *VariableSize = BufferSize;

  return Buffer;
}

STATIC
EFI_STATUS
InitializeSupportLanguage (
  VOID
  )
/*++

Routine Description:

  According the priority of langdef in UNI file to add the supported language code
  to supported language database.

Arguments:

  None

Returns:

  EFI_SUCEESS           - Initialize supported language database successful

--*/
{
  UINT8                         *LanguageString;
  UINT8                         *Language;
  UINT8                         Lang[RFC_3066_ENTRY_SIZE];
  UINT8                         *SuuportedLanguage;
  UINT8                         *TempSuuportedLanguage;
  UINTN                         SupportedLangCnt;
  UINTN                         SavedLang;
  UINTN                         Index;
  UINTN                         BufferSize;
  SETUP_UTILITY_BROWSER_DATA    *SuBrowser;
  EFI_STATUS                    Status;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SuuportedLanguage = SetupUtilityLibGetVariableAndSize (L"PlatformLangCodes", &gEfiGlobalVariableGuid, &BufferSize);
  SupportedLangCnt = 1;
  for (Index = 0; SuuportedLanguage[Index] != 0; Index++) {
    if (SuuportedLanguage[Index] == ';') {
      SupportedLangCnt++;
      SuuportedLanguage[Index] = 0;
    }
  }
  mSuuportLangTable = EfiLibAllocateZeroPool (SupportedLangCnt * RFC_3066_ENTRY_SIZE + sizeof (UINTN));
  LanguageString = GetSupportedLanguages (SuBrowser->SUCInfo->MapTable[ExitHiiHandle].HiiHandle);
  Language = LanguageString;

  SavedLang = 0;
  while (*Language != 0) {
    GetNextLanguage (&Language, Lang);
    TempSuuportedLanguage = SuuportedLanguage;
    for (Index = 0; Index < SupportedLangCnt; Index++) {
      if (!EfiAsciiStrCmp (Lang, TempSuuportedLanguage)) {
        EfiAsciiStrCpy (&mSuuportLangTable->LangString[SavedLang * RFC_3066_ENTRY_SIZE], Lang);
        SavedLang++;
        break;
      }
      TempSuuportedLanguage += EfiAsciiStrSize (TempSuuportedLanguage);
    }
  }
  mSuuportLangTable->LangNum = SavedLang;
  gBS->FreePool(LanguageString);
  gBS->FreePool(SuuportedLanguage);
  return EFI_SUCCESS;

}


EFI_STATUS
SetupUtilityLibGetLangDatabase (
  OUT UINTN            *LangNumber,
  OUT UINT8            **LanguageString
  )
/*++

Routine Description:

  Get supported language database. This funciton will return supported language number
  and language string

Arguments:

  LangNumber      - Pointer to supported language number
  LanguageString  - A double pointer to save the start of supported language string

Returns:

  EFI_SUCEESS           - Initialize supported language database successful

--*/
{
  UINTN       TotalSize;
  EFI_STATUS  Status;

  if (LangNumber == NULL || LanguageString == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  if (mSuuportLangTable == NULL) {
    Status = InitializeSupportLanguage ();
    if (EFI_ERROR(Status)) {
      return Status;
    }
  }
  *LangNumber = mSuuportLangTable->LangNum;
  TotalSize = *LangNumber * RFC_3066_ENTRY_SIZE;
  *LanguageString = EfiLibAllocateZeroPool (TotalSize);
  EfiCopyMem (*LanguageString, mSuuportLangTable->LangString, TotalSize);

  return EFI_SUCCESS;
}

EFI_STATUS
GetSetupUtilityBrowserData (
  OUT SETUP_UTILITY_BROWSER_DATA    **SuBrowser
  )
/*++

Routine Description:

  Get SETUP_UTILITY_BROWSER_DATA instance from gEfiSetupUtilityBrowserProtocolGuid

Arguments:

  SuBrowser       - double pointer point to SETUP_UTILITY_BROWSER_DATA instance

Returns:

  EFI_SUCCESS      - Get SETUP_UTILITY_BROWSER_DATA instance successful
  Other            - Cannot locate gEfiSetupUtilityBrowserProtocolGuid

--*/
{
  EFI_STATUS                                Status;
  EFI_SETUP_UTILITY_BROWSER_PROTOCOL        *Interface;


  Status = gBS->LocateProtocol (
               &gEfiSetupUtilityBrowserProtocolGuid,
               NULL,
               &Interface
               );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  *SuBrowser = EFI_SETUP_UTILITY_BROWSER_FROM_THIS (Interface);

  return Status;;
}
EFI_STATUS
SetupUtilityLibUpdateDeviceString (
  IN EFI_HII_HANDLE                         HiiHandle,
  IN SYSTEM_CONFIGURATION                   *SetupVariable
  )
/*++

Routine Description:

  Update all of device relative stirngs

Arguments:

  HiiHandle       - Specific HII handle for Boot menu
  SetupVariable   - Pointer to SYSTEM_CONFIGURATION instance

Returns:

  EFI_SUCCESS    - Update device relative strings successful
  Other          - Update device relative strings failed

--*/
{
  EFI_STATUS                                Status;
  UINTN                                     Index;
  UINTN                                     Index1;
  STRING_REF                                Token;
  UINT16                                    AdvBootDevCount;
  UINT16                                    LegacyAdvBootDevCount;
  UINT16                                    EfiBootDevCount;
  STRING_PTR                                *UpdateAdvBootString;
  STRING_PTR                                *UpdateLegacyAdvBootString;
  STRING_PTR                                *UpdateEfiBootString;
  STRING_PTR                                *UpdateBootTypeOrderString;
  STRING_PTR                                *TempString;
  UINT16                                    TableTypeCount;
  UINT16                                    *TempBootType;
  SETUP_UTILITY_CONFIGURATION               *SUCInfo;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (SuBrowser->SUCInfo->AdvBootDeviceNum == 0) {
    return EFI_SUCCESS;
  }

  Status = GetBbsName (
             HiiHandle,
             SetupVariable,
             &UpdateBootTypeOrderString,
             &UpdateAdvBootString,
             &UpdateLegacyAdvBootString,
             &UpdateEfiBootString
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SUCInfo               = SuBrowser->SUCInfo;
  TableTypeCount        = SUCInfo->LegacyBootDevTypeCount;
  TempBootType          = SUCInfo->LegacyBootDevType;
  LegacyAdvBootDevCount = SUCInfo->LegacyBootDeviceNum;
  EfiBootDevCount       = SUCInfo->EfiBootDeviceNum;
  AdvBootDevCount       = SUCInfo->AdvBootDeviceNum;

  BootOptionStrAppendDeviceName (EfiBootDevCount, UpdateEfiBootString);
  if (TableTypeCount != 0) {
    SetupVariable->NoBootTypeOrder = 1;
  }
  Token = SUCInfo->BootTypeTokenRecord;
  for (Index = 0; Index < TableTypeCount; Index++, Token++) {
    IfrLibSetString (HiiHandle, Token, UpdateBootTypeOrderString[Index].pString);
  }

  if (LegacyAdvBootDevCount != 0) {
    SetupVariable->NoBootTypeOrder = 1;
  }
  Token = SUCInfo->LegacyAdvanceTokenRecord;
  for (Index = 0; Index < LegacyAdvBootDevCount; Index++, Token++) {
    IfrLibSetString (HiiHandle, Token, UpdateLegacyAdvBootString[Index].pString);
  }

  Token = SUCInfo->EfiTokenRecord;
  for (Index = 0; Index < EfiBootDevCount; Index++, Token++) {
    IfrLibSetString (HiiHandle, Token, UpdateEfiBootString[Index].pString);
  }

  if (AdvBootDevCount != 0) {
    SetupVariable->NoAdvBootDev = 1;
  }
  Token = SUCInfo->AdvanceTokenRecord;
  if (Token != 0) {
    for (Index = 0; Index < AdvBootDevCount; Index++, Token++) {
      IfrLibSetString (HiiHandle, Token, UpdateAdvBootString[Index].pString);
    }
  }

  for(Index = 0; Index < TableTypeCount; Index++) {
    if (mBbsTypeTable[TempBootType[Index]].DeviceTypeCount != 0) {
#if MAX_BOOT_ORDER_NUMBER == 16
        mBootConfig.NoBootDevs[TempBootType[Index]]= 1;
#else
        SetupVariable->NoBootDevs[TempBootType[Index]]= 1;
#endif
    }
    Token = SUCInfo->LegacyNormalTokenRecord[TempBootType[Index]];
    TempString = (STRING_PTR *) mBbsTypeTable[TempBootType[Index]].StringPtr;
    for (Index1 = 0; Index1 < (UINT16) mBbsTypeTable[TempBootType[Index]].DeviceTypeCount; Index1++, Token++) {
      IfrLibSetString (HiiHandle, Token, TempString[Index1].pString);
    }
  }
  for (Index = 0; Index < MAX_BOOT_ORDER_NUMBER; Index++) {
    if (UpdateBootTypeOrderString[Index].pString != NULL) {
      gBS->FreePool (UpdateBootTypeOrderString[Index].pString);
    }
  }
  gBS->FreePool (UpdateBootTypeOrderString);
  gBS->FreePool (UpdateLegacyAdvBootString);
  gBS->FreePool (UpdateEfiBootString);
  for (Index = 0; Index < SuBrowser->SUCInfo->AdvBootDeviceNum; Index++) {
    if (UpdateAdvBootString[Index].BbsEntry != NULL) {
      gBS->FreePool (UpdateAdvBootString[Index].BbsEntry);
    }
    if (UpdateAdvBootString[Index].pString != NULL) {
      gBS->FreePool (UpdateAdvBootString[Index].pString);
    }
  }
  gBS->FreePool (UpdateAdvBootString);

  return EFI_SUCCESS;
}

EFI_STATUS
SetupVariableConfig (
  IN EFI_GUID        *VariableGuid, OPTIONAL
  IN CHAR16          *VariableName, OPTIONAL
  IN UINTN           *BufferSize,
  IN UINT8           *Buffer,
  IN BOOLEAN         RetrieveData
  )
/*++

Routine Description:

  According platform setting to configure setup variable

Arguments:

  VariableGuid  - An optional field to indicate the target variable GUID name to use.
  VariableName  - An optional field to indicate the target human-readable variable name.
  BufferSize    - On input: Length in bytes of buffer to hold retrived data.
                  On output:
                    If return EFI_BUFFER_TOO_SMALL, containg length of buffer desired.
  Buffer        - Buffer to hold retrived data.
  RetrieveData - True : Get setup variable from broswer
                 FALSE: Set current setuputility setting to browser

Returns:

  EFI_SUCCESS   - Setup variable configuration successful
  Other         - Setup variable configuration failed

--*/
{
  EFI_STATUS                       Status;

  if (RetrieveData) {
    Status = GetBrowserData (VariableGuid, VariableName, BufferSize, Buffer);
  } else {
    Status = SetBrowserData (VariableGuid, VariableName, *BufferSize, Buffer, NULL);
  }
  return Status;
}

EFI_STATUS
SetupUtilityLibAsciiToUnicode (
  IN    CHAR8     *AsciiString,
  IN    CHAR16    *UnicodeString,
  IN    UINTN     Length
  )
/*++

Routine Description:

  Convert ASCII string to Unicode string in fixed length.

Arguments:

  AsciiStr        - Input ASCII string
  UnicodeStr      - Output Unicode string
  AsciiStrLen     - The string length of ASCII

Returns:

--*/
{
  UINT8           Index;

  for (Index = 0; Index < Length; Index++) {
    UnicodeString[Index] = (CHAR16) AsciiString[Index];
  }

  return EFI_SUCCESS;
}

EFI_STATUS
SetupUtilityLibUpdateAtaString(
  IN      EFI_IDENTIFY_DATA                 *IdentifyDriveInfo,
  IN      UINTN                             BufSize,
  IN OUT  CHAR16                            **BootString
  )
/*++

Routine Description:
  Function to update the ATA strings into Model Name -- Size

Arguments:

Returns:
  Will return model name and size (or ATAPI if non-ATA)
--*/
{
  CHAR8                                     *TempString;
  UINT16                                    Index;
  CHAR8                                     Temp8;

  TempString = EfiLibAllocateZeroPool (0x100);

  EfiCopyMem (
    TempString,
    IdentifyDriveInfo->AtapiData.ModelName,
    sizeof(IdentifyDriveInfo->AtapiData.ModelName)
    );

  //
  // Swap the IDE string since Identify Drive format is inverted
  //
  Index = 0;
  while (TempString[Index] != 0 && TempString[Index+1] != 0) {
    Temp8 = TempString[Index];
    TempString[Index] = TempString[Index+1];
    TempString[Index+1] = Temp8;
    Index +=2;
  }
  SetupUtilityLibAsciiToUnicode (TempString, *BootString, (UINTN) Index);
  if (TempString != NULL) {
    gBS->FreePool(TempString);
  }

  return EFI_SUCCESS;
}

BOOLEAN
SetupUtilityLibIsLangCodeSupport (
  IN CHAR8                        *LangCode
  )
/*++

Routine Description:

  Check if language code is support in system or not.

Arguments:

  LangCode - Pointer to the language code

Returns:

  TRUE     - Language code is support in system.
  FALSE    - Language code is not support in system.

--*/
{
  UINTN                         BufferSize;
  UINT8                         *SuuportedLanguage;
  UINT8                         *LangStrings;
  BOOLEAN                       Support;
  CHAR8                         Lang[RFC_3066_ENTRY_SIZE];

  if (LangCode == NULL) {
    return FALSE;
  }

  SuuportedLanguage = SetupUtilityLibGetVariableAndSize (L"PlatformLangCodes", &gEfiGlobalVariableGuid, &BufferSize);
  if (SuuportedLanguage == NULL) {
    return FALSE;
  }

  Support = FALSE;
  LangStrings = SuuportedLanguage;
  while (*LangStrings != 0) {
    GetNextLanguage (&LangStrings, Lang);

    if (EfiAsciiStrCmp (Lang, LangCode) == 0) {
      Support = TRUE;
      break;
    }
  }

  gBS->FreePool (SuuportedLanguage);

  return Support;
}

