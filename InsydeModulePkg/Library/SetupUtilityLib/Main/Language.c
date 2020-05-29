/** @file
  The language supported code for setup utility use

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "SetupUtilityLibCommon.h"

/**
  According input data to update the language selectable item to suitable language

  @param This                    Point to Form Callback protocol instance.
  @param Data                    Point to input language string data

  @retval EFI_SUCCESS            Function has completed successfully.
  @return Other                  Set "PlatformLang" variable to save current language setting failed

**/
EFI_STATUS
UpdateLangItem (
  IN const EFI_HII_CONFIG_ACCESS_PROTOCOL  *This,
  IN UINT8                                 *Data
  )
{
  EFI_STATUS       Status;
  UINTN            Index;
  UINT8            *LanguageString;
  UINTN            LangNum;

  LanguageString = NULL;

  SetupUtilityLibGetLangDatabase (
    &LangNum,
    &LanguageString
    );

  if (Data != NULL) {
    Index = *Data;
  } else {
    Index = 0;
  }

  Status = CommonSetVariable (
             L"PlatformLang",
             &gEfiGlobalVariableGuid,
             EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
             AsciiStrSize (((CHAR8 *)&LanguageString[Index * RFC_3066_ENTRY_SIZE])),
             &LanguageString[Index * RFC_3066_ENTRY_SIZE]
             );

  gBS->FreePool (LanguageString);
  return Status;
}


/**
  Based on input string, get the index value of SCU language item.

  @param[in]  LangStr            Point to language string
  @param[out] LangIndex          Point to the index value of SCU language item

  @retval EFI_SUCCESS            Get the index value of SCU language item successfully.
  @retval EFI_INVALID_PARAMETER  Input parameter is NULL.
  @retval EFI_NOT_FOUND          Can not get the index value with corresponding language string.
  @return Other                  Fail to get support language from database.
**/
EFI_STATUS
GetLangIndex (
  IN  CHAR8         *LangStr,
  OUT UINT8         *LangIndex
  )
{
  EFI_STATUS       Status;
  UINTN            Index;
  CHAR8            *SupportLangStr;
  UINTN            SupportLangNum;

  if (LangStr == NULL || LangIndex == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  SupportLangNum = 0;
  SupportLangStr = NULL;

  Status = SetupUtilityLibGetLangDatabase (
             &SupportLangNum,
             (UINT8 **) &SupportLangStr
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  for (Index = 0; Index < SupportLangNum; Index++) {
    if (AsciiStrCmp (LangStr, &SupportLangStr[Index * RFC_3066_ENTRY_SIZE]) == 0) {
      *LangIndex = (UINT8) Index;
      break;
    }
  }

  if (Index == SupportLangNum) {
    Status = EFI_NOT_FOUND;
  }

  if (SupportLangStr != NULL) {
    gBS->FreePool (SupportLangStr);
  }

  return Status;
}
