/** @file

  Image Handle Search By Name
  
;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corp.
;*
;******************************************************************************
*/

#include"ImageHandle_HW.h"


VOID 
NameWrite (
  IN     EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN     CHAR16                                 *FrameTitle,
  IN OUT CHAR16                                 *InputName
  )
{
  EFI_KEY_DATA    HKey;
  UINT8           ModifyPos;
  UINT8           MAXInputNum;
  CHAR16          *FrameTail[] = {
                     L"Press \"Enter\" to Finish"
                     };

  MAXInputNum = 25;
  ModifyPos = 0;
  
  CLEAN_SCREEN();
  
  BaseFrame (
    FrameTitle, 
    FrameTail[0],
    L"",
    L""
    );
  SetCursorPosColor (
    EFI_LIGHTGRAY| EFI_BACKGROUND_BLACK,
    0,
    BASE_TITLE_OFFSET
    );
  Print (L"Please Enter Name :");
  
  while(1){
    HKey = keyRead (
             InputEx
             );
    if(HKey.Key.UnicodeChar == CHAR_CARRIAGE_RETURN){
      break;
    }
    if(HKey.Key.ScanCode == 0){
			SetCursorPosColor (
        EFI_WHITE | EFI_BACKGROUND_LIGHTGRAY,
        (ModifyPos + 19),
        BASE_TITLE_OFFSET
        );
  		InputName[ModifyPos] = HKey.Key.UnicodeChar;
      Print (
        L"%c",
        InputName[ModifyPos]
        );
      ModifyPos++;
      if (ModifyPos >= MAXInputNum) {
        break;
      }
    }
  }
  SetColor(EFI_WHITE | EFI_BACKGROUND_BLACK); 
  Print(L"\n");
}

EFI_STATUS
SearchHandleByName (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN CHAR16                                 *FrameTitle,
  IN CHAR16                                 **FrameTail
  )
{
  EFI_STATUS         Status;
  UINTN              HandleCount = 0;
  EFI_HANDLE         *HandleBuffer = NULL;

  CHAR16             TargetName[25] = {'0'};
  EFI_GUID           *TargetGuid;
  
  NameWrite (
    InputEx,
    FrameTitle,
    TargetName
    );
  GetGuidFromStringName (
    TargetName,
    NULL,
    &TargetGuid
    );
  //
  // Locate Handle Buffer
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,   
                  TargetGuid,
                  NULL,
                  &HandleCount, 
                  &HandleBuffer
                  );
  
  if (EFI_ERROR (Status)) {
    CLEAN_SCREEN();
    Print (L"###LocateHandleBuffer ERROR###\n");
    return Status;
  }

  Status = SelectHandle (
             InputEx,
             FrameTitle,
             FrameTail,
             HandleCount,
             HandleBuffer,
             SearchByGuid,
             TargetGuid
             );
  if (EFI_ERROR (Status)) {
      Print (L"###SelectHandle ERROR###\n");
      return Status;
  }
  
  //
  // Free Handle Buffer
  //
  Status = gBS->FreePool (HandleBuffer);
  if (EFI_ERROR (Status)) {
      Print (L"###LocateHandleBuffer FreePool ERROR###\n");
      return Status;
  }

  return Status;
}
