/** @file

 CPU ID Homework
  
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

#include"CPU_HW.h"

/**
  Handle CPU HW Frame.
  
  @param[in] InputEx        EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL.
                             
  @param[in] CpuIo          EFI_CPU_IO2_PROTOCOL.
 
  @return Status Code
*/
EFI_STATUS
CPUFrameHandler (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN EFI_CPU_IO2_PROTOCOL                   *CpuIo
  )
{
  
  CHAR16                                 *BaseFrameTitle[] = {
                                           L"CPU ID / MSR"
                                           };
  CHAR16                                 *BaseFrameTail[] = { 
                                           L" Enter : Select Item / Confirm Modify   |  Ctrl+F1     : Exit",
                                           L" Left / Right Button: Select Item       |  0~9 & r & w : Select Std Func",
                                           L" Up / Down Botton: Select Previous      |  Alt + 0~9   : Select Ext Func"
                                           };
  CHAR16                                 *ItemLists[] = {
                                           L" Standard Function [0]  ",L" Standard Function [1]  ",
                                           L" Standard Function [2]  ",L" Standard Function [3]  ",
                                           L" Standard Function [4]  ",L" Standard Function [5]  ",
                                           L" Standard Function [6]  ",L" Standard Function [7]  ",
                                           L" Standard Function [8]  ",L" Standard Function [9]  ",
                                           L" Standard Function [A]  ",L" Standard Function [B]  ",
                                           L" Standard Function [C]  ",L" Standard Function [D]  ",
                                           L" Extended Function [0]  ",L" Extended Function [1]  ",
                                           L" Extended Function [2]  ",L" Extended Function [3]  ",
                                           L" Extended Function [4]  ",L" Extended Function [5]  ",
                                           L" Extended Function [6]  ",L" Extended Function [7]  ",
                                           L" Extended Function [8]  ",L" Read MSR               ",
                                           L" Write MSR              "
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
    SetCursorPosColor (
      EFI_BACKGROUND_BLACK,
      HUGE_TABLE_HORI_MAX,
      ITEMLIST_OFFSET
      );
    Print (L"                                              \n");
    SetCursorPosColor (
      EFI_BACKGROUND_BLACK,
      HUGE_TABLE_HORI_MAX,
      (ITEMLIST_OFFSET + 1)
      );
    Print (L"                                              ");
    
    CleanLeftFrame (
      HUGE_TITLE_OFFSET,
      HUGE_TAIL_OFFSET,
      HUGE_TABLE_HORI_MAX,
      HUGE_FRAME_HORI_MAX
      );
    
    SelectedItem = RightSelectItems (
                     InputEx,
                     TOTAL_ITEMS,
                     ItemListsPtr
                     );
    if (SelectedItem == NO_SELECT) {
      break;;
    }
    
	  if (SelectedItem < StandardFunc) {
      ShowStandardFunc (
        SelectedItem,
        InputEx
        );
	  } else if (SelectedItem < ExtendedFunc) {
      ShowExtendedFunc (
        (SelectedItem - StandardFunc),
        InputEx
        );
	  } else if (SelectedItem <= ReadWiteMSR) {
      switch (SelectedItem) {
        case 0x17:
          ReadMsr (
            InputEx
            );
          break;
        case 0x18:
          WriteMSR (
            InputEx
            );
          break;
      }      
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
  
  EFI_CPU_IO2_PROTOCOL                   *CpuIo;
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
  // Locate CpuIo2 Protocol
  //
  Status = gBS->LocateProtocol(
                  &gEfiCpuIo2ProtocolGuid,
                  NULL,
                  &CpuIo
                  );
  if (EFI_ERROR (Status)) {
    Print(
      L"LocateProtocol gEfiCpuIo2ProtocolGuid Fail : %2d\n",
      Status
      );
    
    return Status;
  }
    
  //
  // Keyboard Controller Handler
  //
  CPUFrameHandler (
    InputEx,
    CpuIo
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
