/** @file

  Image Handle
  
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

EFI_STATUS
ImageHandleHandler (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx
  )
{
  EFI_STATUS      Status;
  EFI_KEY_DATA    HKey;

  CHAR16          *JobItemSelect[] = {
                    L"Search All Handle?",
                    L"Search Specific Handle by Protocol Guid?",
                    L"Search Specific Handle by Protocol Name?"
                    };
  CHAR16          *BaseFrameTitle[] = {
                    L"Image Handle",
                    L"Search All Handles",
                    L"Search By GUID",
                    L"Search By Name"
                    };
  CHAR16          *BaseFrameTail[] = { 
                    L"Ctrl+F1 : Exit | Enter : Select Item,",
                    L"Up Botton: Select Previous Item",
                    L"Down Botton: Select Next Item"
                    };
  CHAR16          **JobItemSelectPtr;
  CHAR16          **BaseFrameTitlePtr;
  CHAR16          **BaseFrameTailPtr;

  UINT8           SelectedItem;

  JobItemSelectPtr = JobItemSelect;
  BaseFrameTitlePtr = BaseFrameTitle;
  BaseFrameTailPtr = BaseFrameTail;

  while (TRUE) {
    SelectedItem = SelectItems (
                     InputEx,
                     3,
                     BaseFrameTitle[0],
                     JobItemSelectPtr,
                     BaseFrameTailPtr
                     );
    
    CLEAN_SCREEN();
    if (SelectedItem == NO_SELECT) {
      break;
    }
    switch (SelectedItem) {
    case SearchAllHandles:
      Status = ListAllHandle (
                 InputEx,
                 BaseFrameTitle[1],
                 BaseFrameTailPtr
                 );
      if (EFI_ERROR (Status)) {
        return Status;
      }
      break;
    case SearchByGuid:
      Status = SearchHandleByGuid (
                 InputEx,
                 BaseFrameTitle[2],
                 BaseFrameTailPtr
                 );
      if (EFI_ERROR (Status)) {
        return Status;
      }
      break;
    case SearchByName:
      Status = SearchHandleByName (
                 InputEx,
                 BaseFrameTitle[3],
                 BaseFrameTailPtr
                 );
      break;
    }
  }
  return Status;
}


/**
  Image Handle Entry Point.

  @param[in] ImageHnadle        While the driver image loaded be the ImageLoader(), an image handle is assigned to this
                                driver binary, all activities of the driver is tied to this ImageHandle.
  @param[in] SystemTable        A pointer to the system table, for all BS(Boot Services) and RT(Runtime Services).
 
  @return Status Code

**/
EFI_STATUS
EFIAPI
H_BasePages (
  IN EFI_HANDLE       ImageHandle, 
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS                             Status;

  UINTN                                  TextModeNum;
  UINTN                                  Columns;
  UINTN                                  Rows;
  INT32                                  OringinalMode;
  UINTN                                  OrgColumns;
  UINTN                                  OrgRows;
  
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx = NULL;
  
  //
  // Check Console Mode, save orignal mode and Set specification Mode
  //
  OringinalMode = gST->ConOut->Mode->Mode;
  
  ChkTextModeNum (
    DEF_COLS,
    DEF_ROWS,
    &TextModeNum
    );
  
  Status = gST->ConOut->SetMode (
                          gST->ConOut,
                          TextModeNum
                          );
  if (EFI_ERROR (Status)) {
    Print (L"SetMode ERROR\n");
  }

  //
  // Locate Simple Text Input Protocol
  //
  Status = gBS->LocateProtocol (
                  &gEfiSimpleTextInputExProtocolGuid,
                  NULL,
                  (VOID **) &InputEx
                  );

  if (EFI_ERROR (Status)) {
    Print(
      L"LocateProtocol gEfiSimpleTextInputExProtocolGuid Fail : %2d\n",
      Status
      );
    
    return Status;
  }

  

  //
  //Select func
  //
  Status = ImageHandleHandler (
             InputEx
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Reset to Orginal Mode
  //
  CLEAN_SCREEN (VOID); 

  Status = gST->ConOut->SetMode (
                          gST->ConOut,
                          OringinalMode
                          );
  
  if (EFI_ERROR (Status)) {
    Print (L"SetMode ERROR\n");
  }

  return EFI_SUCCESS;
}
