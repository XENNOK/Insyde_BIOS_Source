/** @file

  Memory Utility Event
  
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


/**

  This code Handle key event and do corresponding func.
  
  @param[in]  *InputEx             Key event Protocol.
  @param[in]  TotalItems           Numbers of items
  @param[in]  **BaseFrameTitle     Base Frame Title.
  @param[in]  **ItemLists          Item lists.
  @param[in]  **BaseFrameTail      Base Frame Tail.
  
*/
UINT8
SelectItems (
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
  
  ItemsPerPage = ITEMS_PER_PAGE;
  PageNumber = 0;
  ItemPosition = 0;
  SelectedItem = NO_SELECT;
  
  if (TotalItems % ItemsPerPage == 0) {
    MaxPages = (TotalItems / ItemsPerPage);
    MaxPageItemNumber = ITEMS_PER_PAGE;
  } else {
    MaxPages = (TotalItems / ItemsPerPage) + 1;
    MaxPageItemNumber = TotalItems % ItemsPerPage;
  }

  //
  // Show Base Frame
  //
  BaseFrame (
    BaseFrametitle,
    BaseFrameTail[0],
    BaseFrameTail[1],
    BaseFrameTail[2]
    );
  
  while (1) {
    
    EN_CURSOR (
      FALSE
      );
    
    ItemCounts = PageNumber * ITEMS_PER_PAGE;
    
    if (PageNumber == (MaxPages  -1)) {
      ItemsPerPage = MaxPageItemNumber;
    } else {
      ItemsPerPage = ITEMS_PER_PAGE;
    }
    
    for (ItemIndex = 0; ItemIndex < ItemsPerPage; ItemIndex++) {
      
      SetCursorPosColor (
        EFI_LIGHTGRAY,
        0,
        (ItemIndex + BASE_TITLE_OFFSET)
        );

      Print (
        L"   %s",
        ItemLists[ItemCounts + ItemIndex]
        );
      
      if (PageNumber == MaxPages - 1) {
        for (CleanLine = (ITEMS_PER_PAGE-1); CleanLine >= ItemsPerPage; CleanLine--) {
          SetCursorPosColor (
            EFI_LIGHTGRAY,
            0,
            (CleanLine + BASE_TITLE_OFFSET)
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
      (ItemPosition + BASE_TITLE_OFFSET)
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
            ItemPosition = ITEMS_PER_PAGE-1;
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

/**

  This code convert user type char to UINT32.
  
  @param[in]  *InputEx             Key event Protocol.
  @param[in]  TypeByte             Type bytes.
  @param[in]  Hori                 Horizontal Position.
  @param[in]  Vert                 Vertical Position
  
*/
UINT32
HDataWrite (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN UINT8                                  TypeByte,
  IN UINT8                                  Hori,
  IN UINT8                                  Vert
  )
{ 
  EFI_KEY_DATA       HKey;
  INT8               DataNum;
	INT8               ModifyPos;
  UINT32             ASCIIVal;
	CHAR16             ModifyVal[8]={'0'};
  BOOLEAN            ModifyFlag;
  
	DataNum = DATA_NUM (TypeByte);
	ModifyPos = 0;
  ASCIIVal = 0;
  
	while (TRUE) {
	  HKey = keyRead (
             InputEx
             );
   
  if ((HKey.Key.ScanCode == 0) &&
      ((HKey.Key.UnicodeChar >= '0' && HKey.Key.UnicodeChar <= '9') ||
       (HKey.Key.UnicodeChar >= 'a' && HKey.Key.UnicodeChar <= 'f') ||
       (HKey.Key.UnicodeChar >= 'A' && HKey.Key.UnicodeChar <= 'F'))) {

    if (DataNum < 0) {
  		DataNum = DATA_NUM (
                  TypeByte
                  );
			ModifyPos = 0;
    }

    ModifyFlag = TRUE;
    
		SetCursorPosColor (
      EFI_WHITE | EFI_BACKGROUND_BLUE,
      (Hori + ModifyPos),
      Vert
      );
  	ModifyVal[DataNum] = HKey.Key.UnicodeChar;
  	Print (
      L"%c",
      ModifyVal[DataNum]
      );
    
  	DataNum--;
		ModifyPos++;
    
    
  	} else if (HKey.Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
      if (ModifyFlag) {
         ModifyFlag = FALSE;
         ASCIIVal = AscToInt (
               ModifyVal,
               TypeByte
               );

         memset (
           (char *) ModifyVal,
           0,
           sizeof(ModifyVal)
           );
      }
      break;
   }
	}
  
  SET_COLOR (EFI_WHITE | EFI_BACKGROUND_BLACK); 
  
  return ASCIIVal;
}

/**

  This code handle time out event.
  
*/
EFI_STATUS 
TimeOut (
  VOID
  )
{
    EFI_STATUS Status;
    UINTN      EventIndex = 0;
    EFI_EVENT  Events[2] = {0}; 
    
    Events[0] = gST->ConIn->WaitForKey;
    Status = gBS->CreateEvent (
                    EVT_TIMER,
                    TPL_CALLBACK,
                    (EFI_EVENT_NOTIFY) NULL,
                    (VOID*) NULL,
                    &Events[1]
                    );
    Status = gBS->SetTimer (
                    Events[1],
                    TimerRelative,
                    (30 * 1000 * 1000)
                    );
    Status = gBS->WaitForEvent (
                    2,
                    Events,
                    &EventIndex
                    );
    if (EFI_SUCCESS == Status)
    {
        if(EventIndex == 0){
            
        }else if(EventIndex == 1){

        }
    }
    Status = gBS->CloseEvent(Events);
    return Status;
}