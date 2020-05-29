/** @file
 Unicode Collation Protocol Implementation

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

#include "UnicodeCollation.h"
#include <Protocol/FirmwareVolume2.h>
#include <Guid/H2OHiiFontFile.h>

STATIC EFI_GUID             mPackageListGuid = {0xf5f219d3, 0x7006, 0x4648, 0xac, 0x8d, 0xd6, 0x1d, 0xfb, 0x7b, 0xc6, 0xad};

#define ISO_639_2_ENTRY_SIZE    3

//
// Globals
//
EFI_UNICODE_COLLATION_PROTOCOL     UnicodeCollation = {
  StriColl,
  MetaiMatch,
  StrLwr,
  StrUpr,
  FatToStr,
  StrToFat,
  "eng"
};

CHAR8 *mUpperMap;
CHAR8 *mLowerMap;
CHAR8 *mInfoMap;

CHAR8 mOtherChars[] = {
  '0',
  '1',
  '2',
  '3',
  '4',
  '5',
  '6',
  '7',
  '8',
  '9',
  '\\',
  '.',
  '_',
  '^',
  '$',
  '~',
  '!',
  '#',
  '%',
  '&',
  '-',
  '{',
  '}',
  '(',
  ')',
  '@',
  '`',
  '\'',
  '\0'
};

EFI_STATUS
SetSupportLanguageFontsByPcd (
  VOID
  );

EFI_STATUS
CreateHiiDriverHandle (
  OUT EFI_HANDLE               *DriverHandle
  );

//
//Init Driver
//
/**
 Initializes the Unicode Collation Driver

 @param[in]   ImageHandle
 @param[in]   SystemTable

 @retval EFI_SUCCESS
 @retval EFI_OUT_OF_RESOURCES
**/
EFI_STATUS
EFIAPI
InitializeUnicodeCollation (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS  Status;
  EFI_HANDLE  Handle;
  UINTN       Index;
  UINTN       Index2;
  CHAR8       *CaseMap;


  Status = SetSupportLanguageFontsByPcd ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Initialize mapping tables for the supported languages
  //
  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  0x300,
                  (VOID **)&CaseMap
                  );

  if (EFI_ERROR (Status)) {
    return EFI_OUT_OF_RESOURCES;
  }

  ZeroMem (CaseMap, 0x300);

  mUpperMap = CaseMap + 0;
  mLowerMap = CaseMap + 0x100;
  mInfoMap  = CaseMap + 0x200;

  for (Index = 0; Index < 0x100; Index++) {

    mUpperMap[Index] = (CHAR8) Index;
    mLowerMap[Index] = (CHAR8) Index;
    mInfoMap[Index] = 0;

    if ((Index >= 'a' && Index <= 'z') || (Index >= 0xe0 && Index <= 0xf6) || (Index >= 0xf8 && Index <= 0xfe)) {

      Index2                = Index - 0x20;
      mUpperMap[Index] = (CHAR8) Index2;
      mLowerMap[Index2] = (CHAR8) Index;

      mInfoMap[Index] |= CHAR_FAT_VALID;
      mInfoMap[Index2] |= CHAR_FAT_VALID;
    }
  }

  for (Index = 0; mOtherChars[Index]; Index++) {
    Index2 = mOtherChars[Index];
    mInfoMap[Index2] |= CHAR_FAT_VALID;
  }
  //
  // Create a handle for the device
  //
  Handle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gEfiUnicodeCollation2ProtocolGuid,
                  &UnicodeCollation,
                  &gEfiUnicodeCollationProtocolGuid,
                  &UnicodeCollation,
                  NULL
                  );

  return Status;
}

/**
 Performs a case-insensitive comparison of two Null-terminated Unicode strings.

 @param[in]   This
 @param[in]   s1
 @param[in]   s2
**/
INTN
EFIAPI
StriColl (
  IN EFI_UNICODE_COLLATION_PROTOCOL   *This,
  IN CHAR16                           *s1,
  IN CHAR16                           *s2
  )
{
  while (*s1) {
    if (TO_UPPER (*s1) != TO_UPPER (*s2)) {
      break;
    }

    s1 += 1;
    s2 += 1;
  }

  return TO_UPPER (*s1) - TO_UPPER (*s2);
}

/**
 Converts all the Unicode characters in a Null-terminated Unicode string
 to lower case Unicode characters.

 @param[in]      This             A pointer to the EFI_UNICODE_COLLATION_PROTOCOL instance.
 @param[in, out] Str

 @retval 0    s1 is equivalent to s2.
 @return > 0  s1 is lexically greater than s2.
 @return < 0  s1 is lexically less than s2.
**/
VOID
EFIAPI
StrLwr (
  IN EFI_UNICODE_COLLATION_PROTOCOL   *This,
  IN OUT CHAR16                       *Str
  )
{
  while (*Str) {
    *Str = TO_LOWER (*Str);
    Str += 1;
  }
}

/**
 Converts all the Unicode characters in a Null-terminated
 Unicode string to upper case Unicode characters.

 @param[in]      This
 @param[in, out] Str

 @retval None
**/
VOID
EFIAPI
StrUpr (
  IN EFI_UNICODE_COLLATION_PROTOCOL   *This,
  IN OUT CHAR16                       *Str
  )
{
  while (*Str) {
    *Str = TO_UPPER (*Str);
    Str += 1;
  }
}

/**
 Performs a case-insensitive comparison between a Null-terminated
 Unicode pattern string and a Null-terminated Unicode string.
 The pattern string can use the '?' wildcard to match any character,
 and the '*' wildcard to match any sub-string.

 @param[in] This             A pointer to the EFI_UNICODE_COLLATION_PROTOCOL instance.
 @param[in] String           A pointer to a Null-terminated Unicode string.
 @param[in] Pattern          A pointer to a Null-terminated Unicode pattern string.

 @retval TRUE                Pattern was found in String.
 @retval FALSE               Pattern was not found in String.
**/
BOOLEAN
EFIAPI
MetaiMatch (
  IN EFI_UNICODE_COLLATION_PROTOCOL   *This,
  IN CHAR16                           *String,
  IN CHAR16                           *Pattern
  )
{
  CHAR16  CharC;
  CHAR16  CharP;
  CHAR16  Index3;

  for (;;) {
    CharP = *Pattern;
    Pattern += 1;

    switch (CharP) {

    case 0:
      //
      // End of pattern.  If end of string, TRUE match
      //
      if (*String) {
        return FALSE;
      } else {
        return TRUE;
      }

    case '*':
      //
      // Match zero or more chars
      //
      while (*String) {
        if (MetaiMatch (This, String, Pattern)) {
          return TRUE;
        }

        String += 1;
      }

      return MetaiMatch (This, String, Pattern);

    case '?':
      //
      // Match any one char
      //
      if (!*String) {
        return FALSE;
      }

      String += 1;
      break;

    case '[':
      //
      // Match char set
      //
      CharC = *String;
      if (!CharC) {
        //
        // syntax problem
        //
        return FALSE;
      }

      Index3  = 0;
      CharP   = *Pattern++;
      while (CharP) {
        if (CharP == ']') {
          return FALSE;
        }

        //
        // if range of chars,
        //
        if (CharP == '-') {
          //
          // get high range
          //
          CharP = *Pattern;
          if (CharP == 0 || CharP == ']') {
            //
            // syntax problem
            //
            return FALSE;
          }

          //
          // if in range,
          //
          if (TO_UPPER (CharC) >= TO_UPPER (Index3) && TO_UPPER (CharC) <= TO_UPPER (CharP)) {
            //
            // if in range, it's a match
            //
            break;
          }
        }

        Index3 = CharP;
        //
        // if char matches
        //
        if (TO_UPPER (CharC) == TO_UPPER (CharP)) {
          //
          // if char matches
          //
          break;
        }

        CharP = *Pattern++;
      }
      //
      // skip to end of match char set
      //
      while (CharP && CharP != ']') {
        CharP = *Pattern;
        Pattern += 1;
      }

      String += 1;
      break;

    default:
      CharC = *String;
      if (TO_UPPER (CharC) != TO_UPPER (CharP)) {
        return FALSE;
      }

      String += 1;
      break;
    }
  }
}

/**
 Converts an 8.3 FAT file name using an OEM character set
 to a Null-terminated Unicode string.
 BUGBUG: Function has to expand DBCS FAT chars, currently not.

 @param[in]   This
 @param[in]   FatSize
 @param[in]   Fat
 @param[out]  String
**/
VOID
EFIAPI
FatToStr (
  IN EFI_UNICODE_COLLATION_PROTOCOL   *This,
  IN UINTN                            FatSize,
  IN CHAR8                            *Fat,
  OUT CHAR16                          *String
  )
{
  //
  // No DBCS issues, just expand and add null terminate to end of string
  //
  while (*Fat && FatSize) {
    *String = *Fat;
    String += 1;
    Fat += 1;
    FatSize -= 1;
  }

  *String = 0;
}

/**
 Converts a Null-terminated Unicode string to legal characters
 in a FAT filename using an OEM character set.
 Functions has to crunch string to a fat string. Replacing
 any chars that can't be represented in the fat name.

 @param[in]   This
 @param[in]   String
 @param[in]   FatSize
 @param[out]  Fat

 @retval TRUE
 @retval FALSE
**/
BOOLEAN
EFIAPI
StrToFat (
  IN EFI_UNICODE_COLLATION_PROTOCOL   *This,
  IN CHAR16                           *String,
  IN UINTN                            FatSize,
  OUT CHAR8                           *Fat
  )
{
  BOOLEAN SpecialCharExist;

  SpecialCharExist = FALSE;
  while (*String && FatSize) {
    //
    // Skip '.' or ' ' when making a fat name
    //
    if (*String != '.' && *String != ' ') {
      //
      // If this is a valid fat char, move it.
      // Otherwise, move a '_' and flag the fact that the name needs an Lfn
      //
      if (*String < 0x100 && (mInfoMap[*String] & CHAR_FAT_VALID)) {
        *Fat = mUpperMap[*String];
      } else {
        *Fat              = '_';
        SpecialCharExist  = TRUE;
      }

      Fat += 1;
      FatSize -= 1;
    }

    String += 1;
  }
  //
  // Do not terminate that fat string
  //
  return SpecialCharExist;
}

/**
  Initalize LangCoes from PCD

  @param[in]   LangCodesSettingRequired
**/
VOID
InitializeLanguage (
  BOOLEAN LangCodesSettingRequired
  )
{
  EFI_STATUS  Status;
  UINTN       Size;
  CHAR8       *Lang;
  CHAR8       LangCode[ISO_639_2_ENTRY_SIZE + 1];
  CHAR8       *LangCodes;
  CHAR8       *PlatformLang;
  CHAR8       *PlatformLangCodes;
  UINTN       Index;
  BOOLEAN     Invalid;

  LangCodes = (CHAR8 *)PcdGetPtr (PcdUefiVariableDefaultLangCodes);
  if (LangCodesSettingRequired) {
    if (!FeaturePcdGet (PcdUefiVariableDefaultLangDeprecate)) {
      //
      // UEFI 2.1 depricated this variable so we support turning it off
      //
      Status =CommonSetVariable (
                      L"LangCodes",
                      &gEfiGlobalVariableGuid,
                      EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                      AsciiStrSize (LangCodes),
                      LangCodes
                      );
    }


    PlatformLangCodes = (CHAR8 *)PcdGetPtr (PcdUefiVariableDefaultPlatformLangCodes);
    Status = CommonSetVariable (
               L"PlatformLangCodes",
               &gEfiGlobalVariableGuid,
               EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
               AsciiStrSize (PlatformLangCodes),
               PlatformLangCodes
               );
  }

  if (!FeaturePcdGet (PcdUefiVariableDefaultLangDeprecate)) {
    //
    // UEFI 2.1 depricated this variable so we support turning it off
    //
    //
    // Find current LangCode from Lang NV Variable
    //
    Size = ISO_639_2_ENTRY_SIZE + 1;
    Status =CommonGetVariable (
              L"Lang",
              &gEfiGlobalVariableGuid,
              &Size,
              &LangCode
              );
    if (!EFI_ERROR (Status)) {
      Status = EFI_NOT_FOUND;

      for (Index = 0; LangCodes[Index] != 0; Index += ISO_639_2_ENTRY_SIZE) {
        if (CompareMem (&LangCodes[Index], LangCode, ISO_639_2_ENTRY_SIZE) == 0) {
          Status = EFI_SUCCESS;
          break;
        }
      }
    }

    //
    // If we cannot get language code from Lang variable, or LangCode cannot be found from language table,
    // set the mDefaultLangCode to Lang variable.
    //
    if (EFI_ERROR (Status)) {
      Lang = (CHAR8 *)PcdGetPtr (PcdUefiVariableDefaultLang);
      Status = CommonSetVariable (
                 L"Lang",
                 &gEfiGlobalVariableGuid,
                 EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                 ISO_639_2_ENTRY_SIZE + 1,
                 Lang
                 );
    }
  }

  Invalid = FALSE;
  PlatformLang = CommonGetVariableData (L"PlatformLang", &gEfiGlobalVariableGuid);
  if (PlatformLang != NULL) {
    //
    // Check Current PlatformLang value against PlatformLangCode.
    // Need a library that is TBD Set Invalid based on state of PlatformLang.
    //
    FreePool (PlatformLang);
  } else {
    // No valid variable is set
    Invalid = TRUE;
  }

  if (Invalid) {
    PlatformLang = (CHAR8 *)PcdGetPtr (PcdUefiVariableDefaultPlatformLang);
    Status = CommonSetVariable (
               L"PlatformLang",
               &gEfiGlobalVariableGuid,
               EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
               AsciiStrSize (PlatformLang),
               PlatformLang
               );
  }
}

/**
  Set support language font by pcd and install font from H2O_HII_FONT_FILE_GUID,
  if H2O_HII_FONT is not found, then will collect Font database to install font.

  @retval EFI_SUCCESS         Install HII font success.
  @retval EFI_NOT_FOUND       Doesn't found H2O_HII_FONT or FontDatabase.
**/
EFI_STATUS
SetSupportLanguageFontsByPcd (
  VOID
  )
{
  EFI_STATUS                    Status;
  UINTN                         FvProtocolCount;
  EFI_HANDLE                    *FvHandles;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *Fv;
  UINTN                         Index;
  UINT32                        AuthenticationStatus;
  UINTN                         ImageSize;
  UINT8                         *ImageData;
  UINTN                         PackageLength;
  UINT8                         *Package;
  EFI_HANDLE                    DriverHandle;
  EFI_HII_HANDLE                HiiHandle;



  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolume2ProtocolGuid,
                  NULL,
                  &FvProtocolCount,
                  &FvHandles
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ImageData = NULL;
  ImageSize = 0;

  for (Index = 0; Index < FvProtocolCount; Index++) {
    Status = gBS->HandleProtocol (
                    FvHandles[Index],
                    &gEfiFirmwareVolume2ProtocolGuid,
                    (VOID **) &Fv
                    );

    Status = Fv->ReadSection (
                  Fv,
                  &gH2OHiiFontFileGuid,
                  EFI_SECTION_RAW,
                  0,
                  (VOID **)&ImageData,
                  &ImageSize,
                  &AuthenticationStatus
                  );
    if (!EFI_ERROR (Status)) {
      break;
    }
  }

  if (EFI_ERROR (Status)) {
    return SetSupportLanguageFonts ();
  }

  InitializeLanguage (TRUE);

  PackageLength = 4 + ImageSize;
  Package = (UINT8 *)AllocateZeroPool (PackageLength);

  CopyMem (Package, &PackageLength, 4);
  CopyMem (Package + 4, ImageData, ImageSize);

  CreateHiiDriverHandle (&DriverHandle);
  HiiHandle = HiiAddPackages (&mPackageListGuid, DriverHandle, Package, NULL);
  ASSERT(HiiHandle != NULL);

  FreePool (ImageData);
  FreePool (Package);

  return EFI_SUCCESS;

}

EFI_STATUS
SetSupportLanguageFonts (
  VOID
  )
{
  EFI_STATUS                            Status;
  EFI_HII_HANDLE                        HiiHandle;
  UINTN                                 NarrowFontSize;
  EFI_HII_SIMPLE_FONT_PACKAGE_HDR       *SimplifiedFont;
  UINTN                                 PackageLength;
  UINT8                                 *Package;
  UINT8                                 *Location;
  EFI_HII_DATABASE_PROTOCOL             *HiiDatabase;
  EFI_HANDLE                            DriverHandle;
  UINTN                                 TotalStringSize;
  EFI_HANDLE                            *HandleBuffer;
  UINTN                                 HandleCount;
  EFI_FONT_DATABASE_PROTOCOL            *Fonts;
  UINTN                                 Index1;
  CHAR8                                 *LangBuffer;

  LangBuffer = NULL;
  Fonts = NULL;
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFontDatabaseProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );

  if (EFI_ERROR (Status) || HandleCount == 0) {
    return EFI_NOT_FOUND;
  }

  Status = gBS->LocateProtocol (
                  &gEfiHiiDatabaseProtocolGuid,
                  NULL,
                  (VOID **)&HiiDatabase
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  TotalStringSize = 0;

  for (Index1 = 0; Index1 < HandleCount; Index1++) {
    Status = gBS->HandleProtocol(
                    HandleBuffer[Index1],
                    &gEfiFontDatabaseProtocolGuid,
                    (VOID **)&Fonts
                    );
    if (EFI_ERROR (Status)) {
      return EFI_LOAD_ERROR;
    }
    NarrowFontSize = (Fonts->NumOfNarrowGlyph * sizeof (EFI_NARROW_GLYPH)) + (Fonts->NumOfWideGlyph * sizeof (EFI_WIDE_GLYPH));

    //
    // Add 4 bytes to the header for entire length for PreparePackageList use only.
    // Looks ugly. Might be updated when font tool is ready.
    //
    PackageLength   = sizeof (EFI_HII_SIMPLE_FONT_PACKAGE_HDR) + NarrowFontSize + 4;
    Package = (UINT8 *)AllocateZeroPool (PackageLength);
    if (Package == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    CopyMem (Package, &PackageLength, 4);
    SimplifiedFont = (EFI_HII_SIMPLE_FONT_PACKAGE_HDR*) (Package + 4);
    SimplifiedFont->Header.Length        = (UINT32) (PackageLength - 4);
    SimplifiedFont->Header.Type          = EFI_HII_PACKAGE_SIMPLE_FONTS;
    SimplifiedFont->NumberOfNarrowGlyphs = Fonts->NumOfNarrowGlyph;
    SimplifiedFont->NumberOfWideGlyphs = Fonts->NumOfWideGlyph;

    Location = (UINT8 *) (&SimplifiedFont->NumberOfWideGlyphs + 1);
    CopyMem(
      Location,
      Fonts->FontNarrowGlyph,
      (Fonts->NumOfNarrowGlyph * sizeof (EFI_NARROW_GLYPH))
      );

    //
    //Copy mChineseFontWideGlyphData[]
    //
    CopyMem(
      Location + (Fonts->NumOfNarrowGlyph * sizeof (EFI_NARROW_GLYPH)),
      Fonts->FontsWideGlyph,
      (Fonts->NumOfWideGlyph * sizeof (EFI_WIDE_GLYPH))
      );
    //
    // Add this simplified font package to a package list then install it.
    //

    CreateHiiDriverHandle (&DriverHandle);
    HiiHandle = HiiAddPackages (&mPackageListGuid, DriverHandle, Package, NULL);
    ASSERT(HiiHandle != NULL);
    gBS->FreePool (Package);
    TotalStringSize += AsciiStrSize ((CHAR8 *)Fonts->SupportedLanguages);
  }
  LangBuffer = (CHAR8 *)AllocateZeroPool (TotalStringSize);
  ASSERT (LangBuffer != NULL);
  if (LangBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  for (Index1 = 0; Index1 < HandleCount; Index1++) {
    Status = gBS->HandleProtocol(
                    HandleBuffer[Index1],
                    &gEfiFontDatabaseProtocolGuid,
                    (VOID **)&Fonts
                    );
    AsciiStrCat (LangBuffer, (CHAR8 *)Fonts->SupportedLanguages);
    if (Index1 != HandleCount - 1) {
      AsciiStrCat (LangBuffer, ";");
    }
  }

  Status = CommonSetVariable (
             L"PlatformLangCodes",
             &gEfiGlobalVariableGuid,
             EFI_VARIABLE_BOOTSERVICE_ACCESS |
             EFI_VARIABLE_RUNTIME_ACCESS,
             TotalStringSize,
             LangBuffer
             );

  gBS->FreePool (LangBuffer);
  return Status;
}

