/** @file

  Hydra Console ,handle attribute and mode and base frame

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

#include"HugeHeader.h"

/**

  Base Title and tail frame.
  @param[in]  *StrVal    Title string.
  @param[in]  BaseTail1  Tail1.
  @param[in]  BaseTail2  Tail2.
  @param[in]  BaseTail3  Tail3.

*/
VOID
HugeFrame (
  IN CHAR16 *StrVal,
  IN CHAR16 *BaseTail1,
  IN CHAR16 *BaseTail2,
  IN CHAR16 *BaseTail3
  )
{
  CLEAN_SCREEN (
    VOID
    );

  SetCursorPosColor (
    EFI_BLUE | EFI_BACKGROUND_LIGHTGRAY,
    0,
    0
    );
  
  Print (
    L"===================================================================================================\n"   //99 =
    ); 
  Print (
    L"=                                                                                                 =\n"
    );
  Print (
    L"===================================================================================================\n"
    );
  
  SetCursorPosColor (
    EFI_BLACK| EFI_BACKGROUND_LIGHTGRAY,
    40,
    1
    );
  
  Print (
    L"%s",
    StrVal
    );
  
	SetCursorPosColor (
    EFI_BLUE | EFI_BACKGROUND_LIGHTGRAY,
    0,
    HUGE_TAIL_OFFSET
    ); 
  
  Print (
    L"===================================================================================================\n"
    );
  Print (
    L"=                                                                                                 =\n"
    );
  Print (
    L"=                                                                                                 =\n"
    );
  Print (
    L"=                                                                                                 =\n"
    );

  SET_CUR_POS (
    0,
    (HUGE_TAIL_OFFSET+1)
    );

  Print (
    L"%s\n%s\n%s\n",
    BaseTail1,
    BaseTail2,
    BaseTail3
    );
  Print (
    L"===================================================================================================\n"
    );
  
}





/**

  This code Handle key event and do corresponding func in Right Frame.
  
  @param[in]  *InputEx             Key event Protocol.
  @param[in]  TotalItems           Numbers of items
  @param[in]  **ItemLists          Item lists.
  
*/
UINT8
HugeRightSelectItems (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN UINTN                                  TotalItems,
  IN CHAR16                                 **ItemLists
  )
{
  EFI_KEY_DATA       HKey;
  UINT8              ItemIndex;
  UINT8              ItemCounts;
  UINT8              PageNumber;
  UINT8              ItemPosition;
  UINT8              MaxPages;
  UINT8              MaxPageItemNumber;
  UINT8              ItemsPerPage;
  UINT8              CleanLine;
  // UINT8              SetSpace;
  UINT8              SelectedItem;
  
  ItemsPerPage = ITEMS_PER_HUGEPAGE;
  PageNumber = 0;
  ItemPosition = 0;
  SelectedItem = NO_SELECT;
  
  if (TotalItems % ItemsPerPage == 0) {
    MaxPages = (UINT8)(TotalItems / ItemsPerPage);
    MaxPageItemNumber = ITEMS_PER_HUGEPAGE;
  } else {
    MaxPages = (UINT8)(TotalItems / ItemsPerPage) + 1;
    MaxPageItemNumber = TotalItems % ItemsPerPage;
  }

   
  while (1) {
    
    EN_CURSOR (
      FALSE
      );
    
    ItemCounts = PageNumber * ITEMS_PER_HUGEPAGE;
    
    if (PageNumber == (MaxPages  -1)) {
      ItemsPerPage = MaxPageItemNumber;
    } else {
      ItemsPerPage = ITEMS_PER_HUGEPAGE;
    }
    
    for (ItemIndex = 0; ItemIndex < ItemsPerPage; ItemIndex++) {
      
      SetCursorPosColor (
        EFI_LIGHTGRAY,
        HUGE_TABLE_HORI_MAX,
        (ItemIndex + ITEMLIST_OFFSET)
        );

      Print (
        L"   %s",
        ItemLists[ItemCounts + ItemIndex]
        );
      
      if (PageNumber == MaxPages - 1) {
        for (CleanLine = (ITEMS_PER_HUGEPAGE-1); CleanLine >= ItemsPerPage; CleanLine--) {
          SetCursorPosColor (
            EFI_LIGHTGRAY ,
            HUGE_TABLE_HORI_MAX,
            (CleanLine + ITEMLIST_OFFSET)
            );
            Print (L"                         ");
        }
      }
    }
    
    SetCursorPosColor (
      EFI_WHITE,
      HUGE_TABLE_HORI_MAX,
      (ItemPosition + ITEMLIST_OFFSET)
      );

    Print (
      L"-> %s\n",
      ItemLists[ItemCounts + ItemPosition]
      );

    HKey = keyRead (
             InputEx
             );
    
    if ((HKey.Key.ScanCode != 0 ) &&
        !((HKey.KeyState.KeyShiftState & EFI_LEFT_CONTROL_PRESSED) ||
          (HKey.KeyState.KeyShiftState & EFI_RIGHT_CONTROL_PRESSED))) {
         
		  switch (HKey.Key.ScanCode) {
        
			case SCAN_UP:
        ItemPosition--;
        
        if (ItemPosition == 0xFF) {
          
          PageNumber --;
          
          if (PageNumber == 0xFF) {
            PageNumber = MaxPages-1;
            ItemPosition = MaxPageItemNumber-1;
          }else{
            ItemPosition = ITEMS_PER_HUGEPAGE-1;
          }
        }
			  break;
			case SCAN_DOWN:
        ItemPosition++;
        
        if (ItemPosition >= ItemsPerPage) {
          ItemPosition=0;
          PageNumber++;
          
          if (PageNumber >= MaxPages) {
            PageNumber = 0;
          }
        }				  
			  break;

		  }
    } else if (HKey.Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {

      //
      // return ItemCounts+ItemPosition
      //
		  SelectedItem = ItemCounts + ItemPosition;
      break;
		} else if ((HKey.KeyState.KeyShiftState & EFI_SHIFT_STATE_VALID) &&
  	  ((HKey.KeyState.KeyShiftState & EFI_LEFT_CONTROL_PRESSED) ||
  	   (HKey.KeyState.KeyShiftState & EFI_RIGHT_CONTROL_PRESSED)) &&
  	  (HKey.Key.ScanCode == SCAN_F1)) {
  	   SelectedItem = NO_SELECT;
       break;
		}
  }
  
  for (CleanLine = 0; CleanLine < ITEMS_PER_HUGEPAGE; CleanLine++) {
    SetCursorPosColor (
      EFI_LIGHTGRAY,
      HUGE_TABLE_HORI_MAX,
      (CleanLine + ITEMLIST_OFFSET)
      );
      Print (L"                                             ");
  }
  return SelectedItem;
}


/**
  Clean frame.
  
  @param[in] TopStart       Position from top.
  @param[in] TailEnd        End Position.
  @param[in] LeftStart      Position from left.
  @param[in] RightEnd        End Position.
  
*/
VOID
CleanFrame (
  UINT8    BackGroundColor,
  UINT8    TopStart,
  UINT8    TailEnd,
  UINT8    LeftStart,
  UINT8    RightEnd
  )
{
  UINT8 CleanLine;
  UINT8 LineIndex;

  for (LineIndex = TopStart; LineIndex < TailEnd; LineIndex++) {
    SetCursorPosColor (
      BackGroundColor,
      LeftStart,
      LineIndex
      );

    for (CleanLine = LeftStart; CleanLine < RightEnd ; CleanLine++) {
      Print (L" ");
    }
  }
}


/**

  Show Data Frame Table
  @param[in]  *HPciDev             pci devices struct
  @param[in]  HType                Type Byte/Word/DWord
  @param[in]  AscOrData            boolean, show ascii value or human read data

*/
VOID
ShowHugeDataFrame (
  IN UINTN     HType,
  UINT8        *FrameData
  )
{
  UINTN   DataVert;
  UINTN   DataHori;
	INT8    TypeVal;

  SetCursorPosColor (
    EFI_BLUE | EFI_BACKGROUND_LIGHTGRAY,
    HUGE_FRAME_VERTTITLE_OFFSET,
    HUGE_FRAME_HORITITLE_OFFSET
    );
  
	switch (HType) {
	case TypeByte:
    Print(
      L"   | 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |\n"
      ); 
	  break;
	case TypeWord:
	  Print(
      L"   | 0100 0302 0504 0706 0908 0B0A 0D0C 0F0E         |\n"
      );
	  break;
	case TypeDWord:
	  Print(
      L"   | 03020100 07060504 0B0A0908 0F0E0D0C             |\n"
      );
	  break;
	}
  
	for (DataVert = 0; DataVert < 16; DataVert++) {
    
    SetCursorPosColor (
      EFI_BLUE | EFI_BACKGROUND_LIGHTGRAY,
      HUGE_FRAME_VERTTITLE_OFFSET,
      (HUGE_FRAME_HORI_OFFSET + DataVert)
      );
    
    Print(
      L"%X0 |",
      DataVert
      );
    
    SetCursorPosColor (
      EFI_BRIGHT,
      HUGE_FRAME_VERT_OFFSET,
      (HUGE_FRAME_HORI_OFFSET + DataVert)
      );
    
    Print (
      L"                                                "   //Clean Line
      ); 
    
    SET_CUR_POS (
      HUGE_FRAME_VERT_OFFSET,
      (HUGE_FRAME_HORI_OFFSET + DataVert)
      );
    
    for (DataHori = 0 ; DataHori < 16 ; DataHori += HType) {
      Print (
        L" "
        );
      
			for (TypeVal = (INT8) (HType-1); TypeVal >= 0; TypeVal--) {
        if (FrameData[ (DataVert * 15) + DataHori + TypeVal] != 0x00) {
          SET_COLOR (
            EFI_LIGHTGRAY
            );
        } else {
          SET_COLOR (
            EFI_BRIGHT
            );
        }
        Print (
          L"%2X",
          FrameData[(DataVert * 15) + DataHori + TypeVal]
          );
			}
    }
    
    SetCursorPosColor (
      EFI_BLUE | EFI_BACKGROUND_LIGHTGRAY,
      52,
      (HUGE_FRAME_HORI_OFFSET + DataVert)
      );
    
    Print (
      L" |\n"
      );
  }
}