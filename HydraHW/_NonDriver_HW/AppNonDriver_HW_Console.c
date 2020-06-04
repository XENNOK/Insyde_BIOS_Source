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

#include "AppNonDriver_HW.h"



/**

  This code Handle key event and do corresponding func in Right Frame.
  
  @param[in]  *InputEx             Key event Protocol.
  @param[in]  TotalItems           Numbers of items
  @param[in]  **ItemLists          Item lists.
  
*/
UINT8
RightSelectItems (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN UINT8                                  TotalItems,
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
  UINT8              SelectedItem;
  
  ItemsPerPage = ITEMS_PER_HUGEPAGE;
  PageNumber = 0;
  ItemPosition = 0;
  SelectedItem = NO_SELECT;

  if (TotalItems <= ItemsPerPage) {
    MaxPages = 1;
    MaxPageItemNumber =  TotalItems;
  } else if (TotalItems % ItemsPerPage == 0) {
    MaxPages = (TotalItems / ItemsPerPage);
    MaxPageItemNumber =  ITEMS_PER_HUGEPAGE;
  } else {
    MaxPages = (TotalItems / ItemsPerPage) + 1;
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
    }  else if (HKey.Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
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
	  } else if ((HKey.Key.ScanCode == 0) &&
        	     !((HKey.KeyState.KeyShiftState & EFI_LEFT_ALT_PRESSED) ||
          	     (HKey.KeyState.KeyShiftState & EFI_RIGHT_ALT_PRESSED)) &&
               (HKey.Key.UnicodeChar >= '1' && HKey.Key.UnicodeChar <= '5')
              ) {
      if (HKey.Key.UnicodeChar == '1') {
         SelectedItem = 0;
         break;
      } else if (HKey.Key.UnicodeChar == '2') {
         SelectedItem = 1;
         break;
      } else if (HKey.Key.UnicodeChar == '3') {
         SelectedItem = 2;
         break;
      } else if (HKey.Key.UnicodeChar == '4') {
         SelectedItem = 3;
         break;
      }
	  }
  }
  
    CleanFrame (
      EFI_BACKGROUND_BLACK,
      HUGE_TITLE_OFFSET,
      HUGE_TAIL_OFFSET,
      HUGE_TABLE_HORI_MAX,
      HUGE_FRAME_HORI_MAX
      );
  return SelectedItem;
}



/**

  Read key
  @param[in] *InputEx   Simple text input ex protocol

  @return    HKey       EFI_KEY_DATA
*/
EFI_KEY_DATA
keyRead (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx
  )
{
  EFI_STATUS      Status;
  UINTN           Index;
  EFI_KEY_DATA    HKey;

  
  Status = gBS->WaitForEvent (
                  1,
                  &(InputEx->WaitForKeyEx),
                  &Index
                  );
  
 if (EFI_ERROR (Status)) {
    CLEAN_SCREEN (
      VOID
      );
    
    Print (
      L"### WaitForEvent ERROR ###\n"
      );
 }
 
  Status = InputEx->ReadKeyStrokeEx (
                      InputEx,
                      &HKey
                      );
  
 if (EFI_ERROR (Status)) {
    CLEAN_SCREEN (
      VOID
      );
    Print (
      L"### ReadKeyStrokeEx ERROR ###\n"
      );
 }
 
 return HKey;
}

/**

  Set Cursor position and color
  @param[in] color   front and background colors
  @param[in] ROWS    cursor rows
  @param[in] COLS    cursor cols

*/
VOID
SetCursorPosColor (
  IN UINTN color,
  IN UINTN ROWS,
  IN UINTN COLS
  )
{
  SET_CUR_POS (
    ROWS,
    COLS
    );
  SET_COLOR (
    color
    );
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

  Check and return Mode number
  @param[in]       RequestedColumns        Requested Columns
  @param[in]       RequestedRows           Requested Rows
  @param[in][out]  *TextModeNum            Mode Number

  @return Status Code

*/
EFI_STATUS
ChkTextModeNum (
  IN UINTN     RequestedColumns,
  IN UINTN     RequestedRows,
  OUT UINTN    *TextModeNum
  )
{
  UINTN        ModeNum;
  UINTN        Columns;
  UINTN        Rows;
  EFI_STATUS   Status;

  for (ModeNum = 1; ModeNum < (UINTN) (gST->ConOut->Mode->MaxMode); ModeNum++) {
    gST->ConOut->QueryMode (
                   gST->ConOut,
                   ModeNum,
                   &Columns,
                   &Rows
                   );
    
    if ((RequestedColumns == Columns) && (RequestedRows == Rows)) {
      *TextModeNum = ModeNum;
      break;
    }
  }

  if (ModeNum == (UINTN) (gST->ConOut->Mode->MaxMode)) {
    *TextModeNum = ModeNum;
    Status = EFI_UNSUPPORTED;
  } else {
    Status = EFI_SUCCESS;
  }

  return Status;
}
