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
  IN EFI_FORM_CALLBACK_PROTOCOL         *This,
  IN UINT8                              *Data
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
  UINT8                                 Lang[3];
  UINT16                                *LanguageString;
  UINT16                                UnicodeLang[3];
  UINTN                                 BufferSize;
  UINTN                                 Index;
  EFI_STATUS                            Status;

  LanguageString = NULL;

  SetupUtilityLibGetLangDatabase (
    &BufferSize,
    &LanguageString
    );

  //
  // Based on the Data->Data->Data value, we can determine
  // which language was chosen by the user
  //
  if (Data != NULL) {
    Index = (*Data) * 3;
  } else {
    Index = 0;
  }

  //
  // The Language (in Unicode format) the user chose
  //
  EfiCopyMem (UnicodeLang, &LanguageString[Index], 6);

  //
  // Convert Unicode to ASCII (Since the ISO standard assumes ASCII equivalent abbreviations
  // we can be safe in converting this Unicode stream to ASCII without any loss in meaning.
  //

  BufferSize = 3;
  for (Index = 0; Index < BufferSize; Index++) {
    Lang[Index] = (CHAR8) UnicodeLang[Index];
  }

  Status = gRT->SetVariable (
                  L"Lang",
                  &gEfiGlobalVariableGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  BufferSize,
                  Lang
                  );

  gBS->FreePool (LanguageString);

  return Status;
}