/** @file

  Variable Homework
  
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

#include"Variable_HW.h"

EFI_STATUS
ListHydraHobs (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx
  )
{
  EFI_KEY_DATA       HKey;
  HOB_LIST           *Hobs;

  Hobs = GetFirstGuidHob(&gEfiHydraVariGuid);
  if (Hobs == NULL) {
    SetCursorPosColor (
      EFI_WHITE | EFI_BACKGROUND_RED,
      0, 
      3
      );
    Print (L"HOB NOT FOUND !!\n");
    Print (L"Press any key to EXIT\n");
    SET_COLOR (
      EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK
      );
    HKey = keyRead (
           InputEx
           );
    return EFI_NOT_FOUND;
  }
  SetCursorPosColor (
    EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK,
    0, 
    3
    );
  Print(L"GUID :\n");
  SetCursorPosColor (
    EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK,
    0, 
    4
    );
  Print(L"%g\n",Hobs->Guid); 
  Print (L"Press any key to show Data\n");
  HKey = keyRead (
           InputEx
           );

  ShowDataBuf (
    InputEx,
    Hobs->Size,
    Hobs->Data
    );
  return EFI_SUCCESS;
}