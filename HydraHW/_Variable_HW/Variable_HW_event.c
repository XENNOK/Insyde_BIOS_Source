/** @file

  Keyboard Crontroller
  
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

  This code Handle key event and do corresponding func in Right Frame.
  
  @param[in]  *InputEx             Key event Protocol.
  @param[in]  TotalItems           Numbers of items
  @param[in]  **ItemLists          Item lists.
  
*/
UINT8
RightSelectItems (
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
  UINT8              SetSpace;
  UINT8              SelectedItem;
  
  ItemsPerPage = ITEMS_PER_HUGEPAGE;
  PageNumber = 0;
  ItemPosition = 0;
  SelectedItem = NO_SELECT;

  if (TotalItems <= ItemsPerPage) {
    MaxPages = 1;
    MaxPageItemNumber = TotalItems;
  } else if (TotalItems % ItemsPerPage == 0) {
    MaxPages = (TotalItems / ItemsPerPage);
    MaxPageItemNumber = ITEMS_PER_HUGEPAGE;
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
      } else if (HKey.Key.UnicodeChar == '5') {
         SelectedItem = 4;
         break;
      }
	  }
  }
  
    CleanFrame (
      HUGE_TITLE_OFFSET,
      HUGE_TAIL_OFFSET,
      HUGE_TABLE_HORI_MAX,
      HUGE_FRAME_HORI_MAX
      );
  return SelectedItem;
}


/**

  This code Handle key event and do corresponding func.
  
  @param[in]  *InputEx             Key event Protocol.
  @param[in]  TotalItems           Numbers of items
  @param[in]  **BaseFrameTitle     Base Frame Title.
  @param[in]  **ItemLists          Item lists.
  @param[in]  **BaseFrameTail      Base Frame Tail.
  
*/
UINT8
LeftSelectItems (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN UINTN                                  TotalItems,
  IN CHAR16                                 *BaseFrametitle,
  IN CHAR16                                 **ItemLists,
  IN CHAR16                                 **BaseFrameTail
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
  UINT8              SetSpace;
  UINT8              SelectedItem;
  
  ItemsPerPage = ITEMS_PER_HUGEPAGE;
  PageNumber = 0;
  ItemPosition = 0;
  SelectedItem = NO_SELECT;

  if (TotalItems < ItemsPerPage) {
    MaxPages = 1;
    MaxPageItemNumber = TotalItems;
  } else if (TotalItems % ItemsPerPage == 0) {
    MaxPages = (TotalItems / ItemsPerPage);
    MaxPageItemNumber = ITEMS_PER_HUGEPAGE;
  } else {
    MaxPages = (TotalItems / ItemsPerPage) + 1;
    MaxPageItemNumber = TotalItems % ItemsPerPage;
  }

  //
  // Show Huge Frame
  //
  HugeFrame (
    BaseFrametitle,
    BaseFrameTail[0],
    BaseFrameTail[1],
    BaseFrameTail[2]
    );
   
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
        0,
        (ItemIndex + ITEMLIST_OFFSET)
        );

      Print (
        L"   %s",
        ItemLists[ItemCounts + ItemIndex]
        );
      
      if (PageNumber == MaxPages - 1) {
        for (CleanLine = (ITEMS_PER_HUGEPAGE-1); CleanLine >= ItemsPerPage; CleanLine--) {
          SetCursorPosColor (
            EFI_LIGHTGRAY,
            0,
            (CleanLine + ITEMLIST_OFFSET)
            );
          
            Print (
              L"                                                                ")
              ;
        }
      }
    }
    
    SetCursorPosColor (
      EFI_WHITE,
      0,
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
    }else if((HKey.KeyState.KeyShiftState & EFI_SHIFT_STATE_VALID) &&
      ((HKey.KeyState.KeyShiftState & EFI_LEFT_CONTROL_PRESSED) ||
       (HKey.KeyState.KeyShiftState & EFI_RIGHT_CONTROL_PRESSED)) &&
      (HKey.Key.ScanCode == SCAN_F1)) {

      //
      // return SelectedItem and break to Previous page
      //
		  SET_CUR_POS (
        0,
        BASE_FRAME_OFFSET
        );
      
      Print (
        L"\n"
        );
      SelectedItem = NO_SELECT;
      
			break;
		} else if (HKey.Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {

      //
      // return ItemCounts+ItemPosition
      //
		  SelectedItem = ItemCounts + ItemPosition;
      break;
		}
  }
  return SelectedItem;
}