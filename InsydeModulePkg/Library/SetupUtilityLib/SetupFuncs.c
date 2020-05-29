/** @file
  Setup utility library related functions

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "SetupUtilityLibCommon.h"


LANGUAGE_DATA_BASE  *mSuuportLangTable;
STATIC CONST CHAR16 mConfigHdrTemplate[] = L"GUID=00000000000000000000000000000000&NAME=0000&PATH=00";
/**
  Get next language from language code list (with separator ';').

  If LangCode is NULL, then ASSERT.
  If Lang is NULL, then ASSERT.

  @param  LangCode               On input: point to first language in the list.
                                 On output: point to next language in the list, or
                                 NULL if no more language in the list.
  @param  Lang                   The first language in the list.

**/
VOID
EFIAPI
SetupUtilityLibGetNextLanguage (
  IN OUT CHAR8      **LangCode,
  OUT CHAR8         *Lang
  )
{
  UINTN  Index;
  CHAR8  *StringPtr;

  ASSERT (LangCode != NULL);
  ASSERT (*LangCode != NULL);
  ASSERT (Lang != NULL);

  Index = 0;
  StringPtr = *LangCode;
  while (StringPtr[Index] != 0 && StringPtr[Index] != ';') {
    Index++;
  }

  CopyMem (Lang, StringPtr, Index);
  Lang[Index] = 0;

  if (StringPtr[Index] == ';') {
    Index++;
  }
  *LangCode = StringPtr + Index;
}

/**
  Given a token, return the string.

  @param  HiiHandle              The handle the token is located
  @param  Token                  The string reference
  @param  LanguageString         indicate what language string we want to get. if this is a
                                 NULL pointer, using the current language setting to get string

  @retval  Pointer to the string corresponding to the token or NULL if
           it cannot get string from Hii database

**/
CHAR16 *
SetupUtilityLibGetTokenStringByLanguage (
  IN EFI_HII_HANDLE                             HiiHandle,
  IN STRING_REF                                 Token,
  IN CHAR8                                      *LanguageString
  )
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
                  (VOID **) &Buffer
                  );
  if (EFI_ERROR (Status)) {
    return NULL;
  }
  ZeroMem (Buffer, BufferLength);
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
             (VOID **) &Buffer
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


/**
  According the priority of langdef in UNI file to add the supported language code
  to supported language database.

  @retval EFI_SUCCESS    Initialize supported language database successful
  @retval Other          Get setup utility browser data fail

**/
STATIC
EFI_STATUS
InitializeSupportLanguage (
  VOID
  )
{
  CHAR8                         *LanguageString;
  CHAR8                         *Language;
  CHAR8                         Lang[RFC_3066_ENTRY_SIZE];
  CHAR8                         *SuuportedLanguage;
  CHAR8                         *TempSuuportedLanguage;
  UINTN                         SupportedLangCnt;
  UINTN                         SavedLang;
  UINTN                         Index;
  SETUP_UTILITY_BROWSER_DATA    *SuBrowser;
  EFI_STATUS                    Status;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SuuportedLanguage = CommonGetVariableData (L"PlatformLangCodes", &gEfiGlobalVariableGuid);
  if (SuuportedLanguage == NULL) {
    return EFI_NOT_FOUND;
  }
  SupportedLangCnt = 1;
  for (Index = 0; SuuportedLanguage[Index] != 0; Index++) {
    if (SuuportedLanguage[Index] == ';') {
      SupportedLangCnt++;
      SuuportedLanguage[Index] = 0;
    }
  }
  mSuuportLangTable = AllocateZeroPool (SupportedLangCnt * RFC_3066_ENTRY_SIZE + sizeof (UINTN));
  if (mSuuportLangTable == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  LanguageString = HiiGetSupportedLanguages (SuBrowser->SUCInfo->MapTable[ExitHiiHandle].HiiHandle);
  if (LanguageString == NULL) {
    return EFI_NOT_FOUND;
  }
  Language = LanguageString;

  SavedLang = 0;
  while (*Language != 0) {
    SetupUtilityLibGetNextLanguage (&Language, Lang);
    TempSuuportedLanguage = SuuportedLanguage;
    for (Index = 0; Index < SupportedLangCnt; Index++) {
      if (!AsciiStrCmp (Lang, TempSuuportedLanguage)) {
        AsciiStrCpy ((CHAR8 *) &mSuuportLangTable->LangString[SavedLang * RFC_3066_ENTRY_SIZE], Lang);
        SavedLang++;
        break;
      }
      TempSuuportedLanguage += AsciiStrSize (TempSuuportedLanguage);
    }
  }
  mSuuportLangTable->LangNum = SavedLang;
  gBS->FreePool(LanguageString);
  gBS->FreePool(SuuportedLanguage);
  return EFI_SUCCESS;

}

/**
  Get supported language database. This funciton will return supported language number
  and language string

  @param  LangNumber             Pointer to supported language number
  @param  LanguageString         A double pointer to save the start of supported language string

  @retval EFI_SUCCESS            Initialize supported language database successful
  @retval EFI_INVALID_PARAMETER  Input parameter is invalid.

**/
EFI_STATUS
SetupUtilityLibGetLangDatabase (
  OUT UINTN            *LangNumber,
  OUT UINT8            **LanguageString
  )
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
  *LanguageString = AllocateZeroPool (TotalSize);
  CopyMem (*LanguageString, mSuuportLangTable->LangString, TotalSize);

  return EFI_SUCCESS;
}

/**
  Get SETUP_UTILITY_BROWSER_DATA instance from gEfiSetupUtilityBrowserProtocolGuid

  @param  SuBrowser              Double pointer point to SETUP_UTILITY_BROWSER_DATA instance

  @retval EFI_SUCCESS            Get SETUP_UTILITY_BROWSER_DATA instance successful
  @retval Other                  Cannot locate gEfiSetupUtilityBrowserProtocolGuid

**/
EFI_STATUS
GetSetupUtilityBrowserData (
  OUT SETUP_UTILITY_BROWSER_DATA    **SuBrowser
  )
{
  EFI_STATUS                                Status;
  EFI_SETUP_UTILITY_BROWSER_PROTOCOL        *Interface;


  Status = gBS->LocateProtocol (
               &gEfiSetupUtilityBrowserProtocolGuid,
               NULL,
               (VOID **) &Interface
               );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  *SuBrowser = EFI_SETUP_UTILITY_BROWSER_FROM_THIS (Interface);

  return Status;;
}

/**
 Update string token of specific legacy boot device type.

 @param[in] HiiHandle           Specific HII handle for Boot menu
 @param[in] SUCInfo             Pointer to SETUP_UTILITY_CONFIGURATION instance
 @param[in] BootDevType         Legacy boot device type
**/
VOID
UpdateLegacyNormalToken (
  IN EFI_HII_HANDLE                 HiiHandle,
  IN SETUP_UTILITY_CONFIGURATION    *SUCInfo,
  IN UINT16                         BootDevType
  )
{
  STRING_REF                        Token;
  STRING_PTR                        *String;
  UINTN                             Index;
  UINTN                             DeviceTypeCount;

  Token  = SUCInfo->LegacyNormalTokenRecord[BootDevType];
  String = (STRING_PTR *) mBbsTypeTable[BootDevType].StringPtr;
  DeviceTypeCount = mBbsTypeTable[BootDevType].DeviceTypeCount;

  for (Index = 0; Index < DeviceTypeCount; Index++, Token++) {
    HiiSetString (HiiHandle, Token, String[Index].pString, NULL);
  }
}

/**
  Update all of device relative stirngs

  @param  HiiHandle              Specific HII handle for Boot menu
  @param  SetupVariable          Pointer to SYSTEM_CONFIGURATION instance

  @retval EFI_SUCCESS            Update device relative strings successful
  @retval Other                  Update device relative strings failed

**/
EFI_STATUS
SetupUtilityLibUpdateDeviceString (
  IN EFI_HII_HANDLE                         HiiHandle,
  IN KERNEL_CONFIGURATION                   *KernelConfig
  )
{
  EFI_STATUS                                Status;
  UINTN                                     Index;
  STRING_REF                                Token;
  UINT16                                    AdvBootDevCount;
  UINT16                                    LegacyAdvBootDevCount;
  UINT16                                    EfiBootDevCount;
  STRING_PTR                                *UpdateAdvBootString;
  STRING_PTR                                *UpdateLegacyAdvBootString;
  STRING_PTR                                *UpdateEfiBootString;
  STRING_PTR                                *UpdateBootTypeOrderString;
  UINT16                                    TableTypeCount;
  UINT16                                    *LegacyBootDevType;
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
             KernelConfig,
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
  LegacyBootDevType     = SUCInfo->LegacyBootDevType;
  LegacyAdvBootDevCount = SUCInfo->LegacyBootDeviceNum;
  EfiBootDevCount       = SUCInfo->EfiBootDeviceNum;
  AdvBootDevCount       = SUCInfo->AdvBootDeviceNum;

  BootOptionStrAppendDeviceName (EfiBootDevCount, UpdateEfiBootString);
  if (TableTypeCount != 0) {
    KernelConfig->NoBootTypeOrder = 1;
  }
  Token = SUCInfo->BootTypeTokenRecord;
  if (UpdateBootTypeOrderString != NULL) {
    for (Index = 0; Index < TableTypeCount; Index++, Token++) {
      HiiSetString (HiiHandle, Token, UpdateBootTypeOrderString[Index].pString, NULL);
    }
  }

  if (LegacyAdvBootDevCount != 0) {
    KernelConfig->NoBootTypeOrder = 1;
  }
  Token = SUCInfo->LegacyAdvanceTokenRecord;
  if (UpdateLegacyAdvBootString != NULL) {
    for (Index = 0; Index < LegacyAdvBootDevCount; Index++, Token++) {
      HiiSetString (HiiHandle, Token, UpdateLegacyAdvBootString[Index].pString, NULL);
    }
  }

  Token = SUCInfo->EfiTokenRecord;
  if (UpdateEfiBootString != NULL) {
    for (Index = 0; Index < EfiBootDevCount; Index++, Token++) {
      HiiSetString (HiiHandle, Token, UpdateEfiBootString[Index].pString, NULL);
    }
  }

  if (AdvBootDevCount != 0) {
    KernelConfig->NoAdvBootDev = 1;
  }
  Token = SUCInfo->AdvanceTokenRecord;
  if (Token != 0 && UpdateAdvBootString != NULL) {
    for (Index = 0; Index < AdvBootDevCount; Index++, Token++) {
      HiiSetString (HiiHandle, Token, UpdateAdvBootString[Index].pString, NULL);
    }
  }

  for(Index = 0; Index < TableTypeCount; Index++) {
    if (mBbsTypeTable[LegacyBootDevType[Index]].DeviceTypeCount != 0) {
      mBootConfig.NoBootDevs[LegacyBootDevType[Index]] = 1;

      UpdateLegacyNormalToken (
        HiiHandle,
        SUCInfo,
        LegacyBootDevType[Index]
        );
    }
  }

  if (UpdateBootTypeOrderString != NULL) {
    for (Index = 0; Index < MAX_BOOT_ORDER_NUMBER; Index++) {
      if (UpdateBootTypeOrderString[Index].pString != NULL) {
        gBS->FreePool (UpdateBootTypeOrderString[Index].pString);
      }
    }
  }
  if (UpdateBootTypeOrderString != NULL) {
    gBS->FreePool (UpdateBootTypeOrderString);
  }
  if (UpdateLegacyAdvBootString != NULL) {
    gBS->FreePool (UpdateLegacyAdvBootString);
  }
  if (UpdateEfiBootString != NULL) {
    gBS->FreePool (UpdateEfiBootString);
  }

  if (UpdateAdvBootString != NULL) {
    for (Index = 0; Index < SuBrowser->SUCInfo->AdvBootDeviceNum; Index++) {
      if (UpdateAdvBootString[Index].BbsEntry != NULL) {
        gBS->FreePool (UpdateAdvBootString[Index].BbsEntry);
      }
      if (UpdateAdvBootString[Index].pString != NULL) {
        gBS->FreePool (UpdateAdvBootString[Index].pString);
      }
    }
    gBS->FreePool (UpdateAdvBootString);
  }
  return EFI_SUCCESS;
}

/**
  According platform setting to configure setup variable

  @param  VariableGuid           An optional field to indicate the target variable GUID name to use.
  @param  VariableName           An optional field to indicate the target human-readable variable name.
  @param  BufferSize             On input: Length in bytes of buffer to hold retrived data.
                                 On output:
                                   If return EFI_BUFFER_TOO_SMALL, containg length of buffer desired.
  @param  Buffer                 Buffer to hold retrived data.
  @param  RetrieveData           TRUE : Get setup variable from broswer
                                 FALSE: Set current setuputility setting to browser

  @retval EFI_SUCCESS            Setup variable configuration successful
  @retval EFI_ABORTED            Setup variable configuration failed

**/
EFI_STATUS
SetupVariableConfig (
  IN EFI_GUID        *VariableGuid, OPTIONAL
  IN CHAR16          *VariableName, OPTIONAL
  IN UINTN           BufferSize,
  IN UINT8           *Buffer,
  IN BOOLEAN         RetrieveData
  )
{
  BOOLEAN            Success;

  if (RetrieveData) {
    Success = HiiGetBrowserData (VariableGuid, VariableName, BufferSize, Buffer);
  } else {
    Success = HiiSetBrowserData (VariableGuid, VariableName, BufferSize, Buffer, NULL);
  }
  if (!Success) {
    DEBUG ((EFI_D_ERROR, "SetupVariableConfig %s variable (%g, %s) fail\n", RetrieveData ? L"Get" : L"Set", VariableGuid, VariableName));
  }
  ASSERT (Success);

  return Success ? EFI_SUCCESS : EFI_ABORTED;
}

/**
  Convert ASCII string to Unicode string in fixed length.

  @param  AsciiString            Input ASCII string
  @param  UnicodeString          Output Unicode string
  @param  Length                 The string length of ASCII

  @retval EFI_SUCCESS            Convert ASCII to Unicode string successfully

**/
EFI_STATUS
SetupUtilityLibAsciiToUnicode (
  IN    CHAR8     *AsciiString,
  IN    CHAR16    *UnicodeString,
  IN    UINTN     Length
  )
{
  UINT8           Index;

  for (Index = 0; Index < Length; Index++) {
    UnicodeString[Index] = (CHAR16) AsciiString[Index];
  }

  return EFI_SUCCESS;
}

/**
  Function to update the ATA strings into Model Name -- Size

  @param  IdentifyDriveInfo      Data of ATA or ATAPI device
  @param  BufSize                Output buffer size
  @param  BootString             Buffer to contain output string

  @retval EFI_SUCCESS            Update ATA string successfully

**/
EFI_STATUS
SetupUtilityLibUpdateAtaString(
  IN      EFI_IDENTIFY_DATA                 *IdentifyDriveInfo,
  IN      UINTN                             BufSize,
  IN OUT  CHAR16                            **BootString
  )
{
  CHAR8                                     *TempString;
  UINT16                                    Index;
  CHAR8                                     Temp8;

  TempString = AllocateZeroPool (0x100);
  if (TempString == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  CopyMem (
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
  gBS->FreePool(TempString);

  return EFI_SUCCESS;
}


/**
 Check if language code is support in system or not.

 @param[in] LangCode            Pointer to the language code

 @retval TRUE                   Language code is support in system.
 @retval FALSE                  Language code is not support in system.
**/
BOOLEAN
SetupUtilityLibIsLangCodeSupport (
  IN CHAR8                        *LangCode
  )
{
  CHAR8                         *SuuportedLanguage;
  CHAR8                         *LangStrings;
  BOOLEAN                       Support;
  CHAR8                         Lang[RFC_3066_ENTRY_SIZE];

  if (LangCode == NULL) {
    return FALSE;
  }

  SuuportedLanguage = CommonGetVariableData (L"PlatformLangCodes", &gEfiGlobalVariableGuid);
  if (SuuportedLanguage == NULL) {
    return FALSE;
  }

  Support = FALSE;
  LangStrings = SuuportedLanguage;
  while (*LangStrings != 0) {
    SetupUtilityLibGetNextLanguage (&LangStrings, Lang);

    if (AsciiStrCmp (Lang, LangCode) == 0) {
      Support = TRUE;
      break;
    }
  }

  gBS->FreePool (SuuportedLanguage);

  return Support;
}

/**
 Get variable store size in bytes from HII package

 @param[in]  Package             HII package data
 @param[in]  FormsetGuid         Formset GUID
 @param[in]  VarStoreGuid        Variable store GUID
 @param[in]  VarStoreName        Variable store name
 @param[out] VarStoreSize        Variable store size

 @retval EFI_SUCCESS             Get size successfully
 @retval EFI_INVALID_PARAMETER   Package or FormsetGuid is NULL or it is not form package
 @retval EFI_NOT_FOUND           Form package or variable store is not found
**/
STATIC
EFI_STATUS
GetVarStoreSizeFromPackage (
  IN  UINT8                     *Package,
  IN  EFI_GUID                  *FormsetGuid,
  IN  EFI_GUID                  *VarStoreGuid,
  IN  CHAR8                     *VarStoreName,
  OUT UINT16                    *VarStoreSize
  )
{
  EFI_HII_PACKAGE_HEADER       *PackageHeader;
  UINT32                       Offset;
  EFI_IFR_OP_HEADER            *OpCodeHdr;
  BOOLEAN                      IsCurrentFormset;
  EFI_IFR_VARSTORE             *VarStore;

  if (Package == NULL || FormsetGuid == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  PackageHeader = (EFI_HII_PACKAGE_HEADER *) Package;
  if (PackageHeader->Type != EFI_HII_PACKAGE_FORMS) {
    return EFI_INVALID_PARAMETER;
  }

  IsCurrentFormset = FALSE;
  Offset = sizeof (EFI_HII_PACKAGE_HEADER);
  while (Offset < PackageHeader->Length) {
    OpCodeHdr = (EFI_IFR_OP_HEADER *) (Package + Offset);

    if (OpCodeHdr->OpCode == EFI_IFR_FORM_SET_OP) {
      if (CompareGuid ((EFI_GUID *) (VOID *) (&((EFI_IFR_FORM_SET *) OpCodeHdr)->Guid), FormsetGuid)) {
        IsCurrentFormset = TRUE;
      } else {
        IsCurrentFormset = FALSE;
      }
    }

    if (IsCurrentFormset && OpCodeHdr->OpCode == EFI_IFR_VARSTORE_OP) {
      VarStore = (EFI_IFR_VARSTORE *) OpCodeHdr;

      if (CompareGuid ((EFI_GUID *) (VOID *) (&VarStore->Guid), VarStoreGuid) &&
          AsciiStrCmp ((CHAR8 *) VarStore->Name, VarStoreName) == 0) {
        *VarStoreSize = VarStore->Size;
        return EFI_SUCCESS;
      }
    }

    Offset += OpCodeHdr->Length;
  }

  return EFI_NOT_FOUND;
}

/**
 Get variable store size in bytes

 @param[in] HiiHandle           An EFI_HII_HANDLE that corresponds to the desired package list in the HII database
 @param[in] FormsetGuid         Formset GUID
 @param[in] VarStoreGuid        Variable store GUID
 @param[in] VarStoreName        Variable store name

 @return The size of variable store in bytes
**/
UINT16
GetVarStoreSize (
  IN EFI_HII_HANDLE            HiiHandle,
  IN EFI_GUID                  *FormsetGuid,
  IN EFI_GUID                  *VarStoreGuid,
  IN CHAR8                     *VarStoreName
  )
{
  EFI_STATUS                   Status;
  EFI_HII_DATABASE_PROTOCOL    *HiiDatabase;
  UINTN                        HiiPackageListSize;
  EFI_HII_PACKAGE_LIST_HEADER  *HiiPackageList;
  UINT8                        *Package;
  EFI_HII_PACKAGE_HEADER       *PackageHeader;
  UINT32                       Offset;
  UINT32                       PackageListLength;
  UINT16                       VarStoreSize;

  VarStoreSize = 0;

  if (HiiHandle == NULL || FormsetGuid == NULL || VarStoreGuid == NULL || VarStoreName == NULL) {
    return VarStoreSize;
  }

  Status = gBS->LocateProtocol (&gEfiHiiDatabaseProtocolGuid, NULL, (VOID **) &HiiDatabase);
  if (EFI_ERROR (Status)) {
    return VarStoreSize;
  }

  //
  // Get HII package list
  //
  HiiPackageList     = NULL;
  HiiPackageListSize = 0;
  Status = HiiDatabase->ExportPackageLists (HiiDatabase, HiiHandle, &HiiPackageListSize, HiiPackageList);
  if (Status != EFI_BUFFER_TOO_SMALL) {
    return VarStoreSize;
  }
  HiiPackageList = AllocateZeroPool (HiiPackageListSize);
  if (HiiPackageList == NULL) {
    return VarStoreSize;
  }
  Status = HiiDatabase->ExportPackageLists (HiiDatabase, HiiHandle, &HiiPackageListSize, HiiPackageList);
  if (EFI_ERROR (Status)) {
    FreePool (HiiPackageList);
    return VarStoreSize;
  }

  //
  // In HII package list, find the variable store size which match the formset GUID, variable store GUID and name.
  //
  Offset            = sizeof (EFI_HII_PACKAGE_LIST_HEADER);
  PackageListLength = ReadUnaligned32 (&HiiPackageList->PackageLength);
  while (Offset < PackageListLength) {
    Package       = (UINT8 *) HiiPackageList + Offset;
    PackageHeader = (EFI_HII_PACKAGE_HEADER *) Package;

    if (PackageHeader->Type == EFI_HII_PACKAGE_FORMS) {
      Status = GetVarStoreSizeFromPackage (Package, FormsetGuid, VarStoreGuid, VarStoreName, &VarStoreSize);
      if (!EFI_ERROR (Status)) {
        break;
      }
    }

    Offset += PackageHeader->Length;
  }

  FreePool (HiiPackageList);

  return VarStoreSize;
}

