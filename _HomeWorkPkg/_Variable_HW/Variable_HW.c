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

/**
  Handle CPU HW Frame.
  
  @param[in] InputEx        EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL.
                             
  @param[in] CpuIo          EFI_CPU_IO2_PROTOCOL.
 
  @return Status Code
*/
EFI_STATUS
VariFrameHandler (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx
  )
{
  EFI_STATUS                             Status;
  CHAR16                                 *BaseFrameTitle[] = {
                                           L"Variable"
                                           };
  CHAR16                                 *BaseFrameTail[] = { 
                                            L" Enter : Select Item / Confirm Modify   |  Ctrl+F1     : Exit",
                                            L" Left / Right Button: Select Item       |  ",
                                            L" Up / Down Botton: Select Previous      |  "
                                            };
  CHAR16                                 *ItemLists[] = {
                                             L"List All Variable?",
                                             L"Search Specific Variable by Guid / Name?",
                                             L"Create New Variable?",
                                             L"Get HOB Data"
                                           };
  
  CHAR16                                 **ItemListsPtr;
  UINT8                                  SelectedItem;
  UINT8                                  DefaultOffset;



  ItemListsPtr = ItemLists;
  SelectedItem = NO_SELECT;
  
  //
  // Show Huge Frame
  //
  HugeFrame (
    BaseFrameTitle[0],
    BaseFrameTail[0],
    BaseFrameTail[1],
    BaseFrameTail[2]
    );

  while (TRUE) {
    CleanFrame (
      HUGE_TITLE_OFFSET,
      HUGE_TAIL_OFFSET,
      0,
      HUGE_FRAME_HORI_MAX
      );
    
    SelectedItem = RightSelectItems (
                     InputEx,
                     TOTAL_ITEMS,
                     ItemListsPtr
                     );
    if (SelectedItem == NO_SELECT) {
      break;
    }
    
	 switch (SelectedItem) {
   case ListVari:
     ListAllVariables (
       InputEx
       );
     break;
   case SearchVariByGuidName:
     SearchVariables (
       InputEx
       );
     break;
   case CreateNew:
     CreateVariables (
       InputEx
       );
     break;
   case GetHobs:
     ListHydraHobs (
       InputEx
       ); 
   break;
	 }
  }
}

/**
  Keyboard Controller Entry Point.

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

  UINTN    TextModeNum;
  UINTN    Columns;
  UINTN    Rows;
  INT32    OringinalMode;
  UINTN    OrgColumns;
  UINTN    OrgRows;
  
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
  // Keyboard Controller Handler
  //
  VariFrameHandler (
    InputEx
    );


  //
  // Reset to Orginal Mode
  //
  EN_CURSOR(TRUE);
  SET_COLOR(EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
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
