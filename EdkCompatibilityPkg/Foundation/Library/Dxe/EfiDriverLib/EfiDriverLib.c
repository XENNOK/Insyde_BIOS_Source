/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/*++

Copyright (c) 2004 - 2010, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

Module Name:

  EfiDriverLib.c

Abstract:

  Light weight lib to support EFI drivers.

--*/

#include "Tiano.h"
#include "EfiDriverLib.h"

//
// Global Interface for Debug Mask Protocol
//
EFI_DEBUG_MASK_PROTOCOL *gDebugMaskInterface = NULL;

/**
 Intialize Driver Lib if it has not yet been initialized.

 @param [in]   ImageHandle      Standard EFI Image entry parameter
 @param [in]   SystemTable      Standard EFI Image entry parameter

 @return EFI_STATUS always returns EFI_SUCCESS

**/
EFI_STATUS
EfiInitializeDriverLib (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  gST = SystemTable;

  ASSERT (gST != NULL);

  gBS = gST->BootServices;
  gRT = gST->RuntimeServices;

  ASSERT (gBS != NULL);
  ASSERT (gRT != NULL);

  //
  // Get driver debug mask protocol interface
  //
#ifdef EFI_DEBUG
  gBS->HandleProtocol (
        ImageHandle,
        &gEfiDebugMaskProtocolGuid,
        (VOID *) &gDebugMaskInterface
        );
#endif
  //
  // Should be at EFI_D_INFO, but lets us know things are running
  //
  DEBUG ((EFI_D_INFO, "EfiInitializeDriverLib: Started\n"));

  return EFI_SUCCESS;
}

/**
 Tests whether a language code has format of ISO639-2.

 @param [in]   Languages        The language code to be tested.

 @retval TRUE                   Language code format is ISO 639-2.
 @retval FALSE                  Language code format is not ISO 639-2.

**/
STATIC
BOOLEAN
IsIso639LanguageCode (
  IN CHAR8                *Languages
  )
{
  UINTN  Index;

  //
  // Find out format of Languages
  //
  for (Index = 0; Languages[Index] != 0 && Languages[Index] != ';' && Languages[Index] != '-'; Index++);
  if (Languages[Index] != 0) {
    //
    // RFC4646 language code
    //
    return FALSE;
  }

  //
  // No ';' and '-', it's either ISO639-2 code (list) or single RFC4646 code
  //
  if (Index == 2) {
    //
    // Single RFC4646 language code without country code, e.g. "en"
    //
    return FALSE;
  }

  //
  // Languages in format of ISO639-2
  //
  return TRUE;
}

/**
 Compare the first language instance of two language codes, either could be a
 single language code or a language code list. This function assume Language1
 and Language2 has the same language code format, i.e. either ISO639-2 or RFC4646.

 @param [in]   Language1        The first language code to be tested.
 @param [in]   Language2        The second language code to be tested.

 @retval TRUE                   Language code match.
 @retval FALSE                  Language code mismatch.

**/
BOOLEAN
EfiLibCompareLanguage (
  IN  CHAR8               *Language1,
  IN  CHAR8               *Language2
  )
{
  UINTN Index;

  //
  // Compare first two bytes of language tag
  //
  if ((Language1[0] != Language2[0]) || (Language1[1] != Language2[1])) {
    return FALSE;
  }

  if (IsIso639LanguageCode (Language1)) {
    //
    // ISO639-2 language code, compare the third byte of language tag
    //
    return (BOOLEAN) ((Language1[2] == Language2[2]) ? TRUE : FALSE);
  }

  //
  // RFC4646 language code
  //
  for (Index = 0; Language1[Index] != 0 && Language1[Index] != ';'; Index++);
  if ((EfiAsciiStrnCmp (Language1, Language2, Index) == 0) && (Language2[Index] == 0 || Language2[Index] == ';')) {
    return TRUE;
  }

  return FALSE;
}

/**
 Step to next language code of a language code list.

 @param [in]   Languages        The language code list to traverse.

 @return Pointer to next language code or NULL terminator if it's the last one.

**/
STATIC
CHAR8 *
NextSupportedLanguage (
  IN CHAR8                *Languages
  )
{
  UINTN    Index;

  if (IsIso639LanguageCode (Languages)) {
    //
    // ISO639-2 language code
    //
    return (Languages + 3);
  }

  //
  // Search in RFC4646 language code list
  //
  for (Index = 0; Languages[Index] != 0 && Languages[Index] != ';'; Index++);
  if (Languages[Index] == ';') {
    Index++;
  }
  return (Languages + Index);
}

/**
 Translate a unicode string to a specified language if supported.

 @param [in]   Language         The name of language to translate to
 @param [in]   SupportedLanguages  Supported languages set
 @param [in]   UnicodeStringTable  Pointer of one item in translation dictionary
 @param [out]  UnicodeString    The translated string

 @retval EFI_INVALID_PARAMETER  Invalid parameter
 @retval EFI_UNSUPPORTED        System not supported this language or this string translation
 @retval EFI_SUCCESS            String successfully translated

**/
EFI_STATUS
EfiLibLookupUnicodeString (
  IN  CHAR8                     *Language,
  IN  CHAR8                     *SupportedLanguages,
  IN  EFI_UNICODE_STRING_TABLE  *UnicodeStringTable,
  OUT CHAR16                    **UnicodeString
  )
{
  //
  // Make sure the parameters are valid
  //
  if (Language == NULL || UnicodeString == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // If there are no supported languages, or the Unicode String Table is empty, then the
  // Unicode String specified by Language is not supported by this Unicode String Table
  //
  if (SupportedLanguages == NULL || UnicodeStringTable == NULL) {
    return EFI_UNSUPPORTED;
  }

  //
  // Make sure Language is in the set of Supported Languages
  //
  while (*SupportedLanguages != 0) {
    if (EfiLibCompareLanguage (Language, SupportedLanguages)) {

      //
      // Search the Unicode String Table for the matching Language specifier
      //
      while (UnicodeStringTable->Language != NULL) {
        if (EfiLibCompareLanguage (Language, UnicodeStringTable->Language)) {

          //
          // A matching string was found, so return it
          //
          *UnicodeString = UnicodeStringTable->UnicodeString;
          return EFI_SUCCESS;
        }

        UnicodeStringTable++;
      }

      return EFI_UNSUPPORTED;
    }

    SupportedLanguages = NextSupportedLanguage (SupportedLanguages);
  }

  return EFI_UNSUPPORTED;
}

/**
 Add an translation to the dictionary if this language if supported.

 @param [in]   Language         The name of language to translate to
 @param [in]   SupportedLanguages  Supported languages set
 @param [in, out] UnicodeStringTable  Translation dictionary
 @param [in]   UnicodeString    The corresponding string for the language to be translated to

 @retval EFI_INVALID_PARAMETER  Invalid parameter
 @retval EFI_UNSUPPORTED        System not supported this language
 @retval EFI_ALREADY_STARTED    Already has a translation item of this language
 @retval EFI_OUT_OF_RESOURCES   No enough buffer to be allocated
 @retval EFI_SUCCESS            String successfully translated

**/
EFI_STATUS
EfiLibAddUnicodeString (
  IN      CHAR8                     *Language,
  IN      CHAR8                     *SupportedLanguages,
  IN OUT  EFI_UNICODE_STRING_TABLE  **UnicodeStringTable,
  IN      CHAR16                    *UnicodeString
  )
{
  UINTN                     NumberOfEntries;
  EFI_UNICODE_STRING_TABLE  *OldUnicodeStringTable;
  EFI_UNICODE_STRING_TABLE  *NewUnicodeStringTable;
  UINTN                     UnicodeStringLength;

  //
  // Make sure the parameter are valid
  //
  if (Language == NULL || UnicodeString == NULL || UnicodeStringTable == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // If there are no supported languages, then a Unicode String can not be added
  //
  if (SupportedLanguages == NULL) {
    return EFI_UNSUPPORTED;
  }

  //
  // If the Unicode String is empty, then a Unicode String can not be added
  //
  if (UnicodeString[0] == 0) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Make sure Language is a member of SupportedLanguages
  //
  while (*SupportedLanguages != 0) {
    if (EfiLibCompareLanguage (Language, SupportedLanguages)) {

      //
      // Determine the size of the Unicode String Table by looking for a NULL Language entry
      //
      NumberOfEntries = 0;
      if (*UnicodeStringTable != NULL) {
        OldUnicodeStringTable = *UnicodeStringTable;
        while (OldUnicodeStringTable->Language != NULL) {
          if (EfiLibCompareLanguage (Language, OldUnicodeStringTable->Language)) {
            return EFI_ALREADY_STARTED;
          }

          OldUnicodeStringTable++;
          NumberOfEntries++;
        }
      }

      //
      // Allocate space for a new Unicode String Table.  It must hold the current number of
      // entries, plus 1 entry for the new Unicode String, plus 1 entry for the end of table
      // marker
      //
      NewUnicodeStringTable = EfiLibAllocatePool ((NumberOfEntries + 2) * sizeof (EFI_UNICODE_STRING_TABLE));
      if (NewUnicodeStringTable == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }

      //
      // If the current Unicode String Table contains any entries, then copy them to the
      // newly allocated Unicode String Table.
      //
      if (*UnicodeStringTable != NULL) {
        EfiCopyMem (
          NewUnicodeStringTable,
          *UnicodeStringTable,
          NumberOfEntries * sizeof (EFI_UNICODE_STRING_TABLE)
          );
      }

      //
      // Allocate space for a copy of the Language specifier
      //
      NewUnicodeStringTable[NumberOfEntries].Language = EfiLibAllocateCopyPool (EfiAsciiStrSize (Language), Language);
      if (NewUnicodeStringTable[NumberOfEntries].Language == NULL) {
        gBS->FreePool (NewUnicodeStringTable);
        return EFI_OUT_OF_RESOURCES;
      }

      //
      // Compute the length of the Unicode String
      //
      for (UnicodeStringLength = 0; UnicodeString[UnicodeStringLength] != 0; UnicodeStringLength++)
        ;

      //
      // Allocate space for a copy of the Unicode String
      //
      NewUnicodeStringTable[NumberOfEntries].UnicodeString = EfiLibAllocateCopyPool (
                                                              (UnicodeStringLength + 1) * sizeof (CHAR16),
                                                              UnicodeString
                                                              );
      if (NewUnicodeStringTable[NumberOfEntries].UnicodeString == NULL) {
        gBS->FreePool (NewUnicodeStringTable[NumberOfEntries].Language);
        gBS->FreePool (NewUnicodeStringTable);
        return EFI_OUT_OF_RESOURCES;
      }

      //
      // Mark the end of the Unicode String Table
      //
      NewUnicodeStringTable[NumberOfEntries + 1].Language       = NULL;
      NewUnicodeStringTable[NumberOfEntries + 1].UnicodeString  = NULL;

      //
      // Free the old Unicode String Table
      //
      if (*UnicodeStringTable != NULL) {
        gBS->FreePool (*UnicodeStringTable);
      }

      //
      // Point UnicodeStringTable at the newly allocated Unicode String Table
      //
      *UnicodeStringTable = NewUnicodeStringTable;

      return EFI_SUCCESS;
    }

    SupportedLanguages = NextSupportedLanguage (SupportedLanguages);
  }

  return EFI_UNSUPPORTED;
}

/**
 Free a string table.

 @param [in, out] UnicodeStringTable  The string table to be freed.

 @retval EFI_SUCCESS            The table successfully freed.

**/
EFI_STATUS
EfiLibFreeUnicodeStringTable (
  IN OUT  EFI_UNICODE_STRING_TABLE  *UnicodeStringTable
  )
{
  UINTN Index;

  //
  // If the Unicode String Table is NULL, then it is already freed
  //
  if (UnicodeStringTable == NULL) {
    return EFI_SUCCESS;
  }

  //
  // Loop through the Unicode String Table until we reach the end of table marker
  //
  for (Index = 0; UnicodeStringTable[Index].Language != NULL; Index++) {

    //
    // Free the Language string from the Unicode String Table
    //
    gBS->FreePool (UnicodeStringTable[Index].Language);

    //
    // Free the Unicode String from the Unicode String Table
    //
    if (UnicodeStringTable[Index].UnicodeString != NULL) {
      gBS->FreePool (UnicodeStringTable[Index].UnicodeString);
    }
  }

  //
  // Free the Unicode String Table itself
  //
  gBS->FreePool (UnicodeStringTable);

  return EFI_SUCCESS;
}
