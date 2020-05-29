//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Module Name:
//;
//;   Language.c
//;
//; Abstract:
//;
//;   The language supported code for setup utility use
//;

#include "SetupUtilityLibCommon.h"

EFI_STATUS
UpdateLangItem (
  IN const EFI_HII_CONFIG_ACCESS_PROTOCOL  *This,
  IN UINT8                                 *Data
  )
/*++

Routine Description:

  According input data to update the language selectable item to suitable language

Arguments:

  This      -  Point to Form Callback protocol instance.
  Data      -  Point to input language string data

Returns:

  EFI_SUCCESS  -  Function has completed successfully.
  Other        -  Set "Lang" variable to save current language setting failed

--*/
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
  Status = gRT->SetVariable (
                  L"PlatformLang",
                  &gEfiGlobalVariableGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  EfiAsciiStrSize (&LanguageString[Index * RFC_3066_ENTRY_SIZE]),
                  &LanguageString[Index * RFC_3066_ENTRY_SIZE]
                  );
  gBS->FreePool (LanguageString);
  return Status;
}


EFI_STATUS
GetLangIndex (
  IN  CHAR8         *LangStr,
  OUT UINT8         *LangIndex
  )
/*++

Routine Description:

  Based on input string, get the index value of SCU language item.

Arguments:

  LangStr      -  Point to language string
  LangIndex    -  Point to the index value of SCU language item

Returns:

  EFI_SUCCESS            -  Get the index value of SCU language item successfully.
  EFI_INVALID_PARAMETER  -  Input parameter is NULL.
  EFI_NOT_FOUND          -  Can not get the index value with corresponding language string.
  Other                  -  Fail to get support language from database.

--*/
{
  EFI_STATUS       Status;
  UINTN            Index;
  UINT8            *SupportLangStr;
  UINTN            SupportLangNum;

  if (LangStr == NULL || LangIndex == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  
  SupportLangNum = 0;
  SupportLangStr = NULL;

  Status = SetupUtilityLibGetLangDatabase (
             &SupportLangNum,
             &SupportLangStr
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  for (Index = 0; Index < SupportLangNum; Index++) {
    if (EfiAsciiStrCmp (LangStr, &SupportLangStr[Index * RFC_3066_ENTRY_SIZE]) == 0) {
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

