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

#include"KBC_HW.h"


/**

  This code handle time out event.
  
*/
EFI_STATUS 
HelfSecTimeOut (
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
                    (5 * 1000 * 1000)
                    );
    Status = gBS->WaitForEvent (
                    2,
                    Events,
                    &EventIndex
                    );
    Status = gBS->CloseEvent(Events);
    return Status;
}

VOID
Blinking (
  IN EFI_CPU_IO2_PROTOCOL                   *CpuIo
)
{
  UINT8      LEDIndex;
  UINT8      KbcAdd;
  
  KbcAdd = STATINDIC_CMD;
  for (LEDIndex = 0; LEDIndex < 8; LEDIndex++) {

     KBC_Event (
       CpuIo,
       KBC_KB_CMD,
       KBC_KB_DATA,
       &KbcAdd,
       &LEDIndex
       );

     HelfSecTimeOut ();
  }
}

/**

  This code handle time out event.
  
*/
EFI_STATUS 
BlinkLEDs (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN EFI_CPU_IO2_PROTOCOL                   *CpuIo
  )
{
  EFI_STATUS      Status;
  EFI_KEY_DATA    HKey;
  UINTN           EventIndex = 0;
  EFI_EVENT       Events[2] = {0}; 
  UINT8           KbcAdd;
   UINT8          LEDReset;
   
  KbcAdd = STATINDIC_CMD;
  LEDReset = 0;
  SetCursorPosColor (
    EFI_WHITE | EFI_BACKGROUND_CYAN ,
    RIGHT_DATA_FRAME_HORI_MIN,
    (BASE_TITLE_OFFSET + 3)
    );
  Print (L"Press Ctrl + F1 to Quit");

  SET_COLOR (
    EFI_LIGHTGRAY| EFI_BACKGROUND_BLACK
    );
    
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
                  TimerPeriodic,
                  (30 * 1000 * 1000)
                  );
  Blinking (CpuIo);
  while (TRUE) {
    Status = gBS->WaitForEvent (
                    2,
                    Events,
                    &EventIndex
                    );
    if (EFI_SUCCESS == Status)
    {
        if (EventIndex == 0){          
           Status = InputEx->ReadKeyStrokeEx (
                               InputEx,
                               &HKey
                               );
           
          if (EFI_ERROR (Status)) {
             CLEAN_SCREEN ();
             Print (L"### ReadKeyStrokeEx ERROR ###\n");
          }
  
          if ((HKey.KeyState.KeyShiftState & EFI_SHIFT_STATE_VALID) &&
              ((HKey.KeyState.KeyShiftState & EFI_LEFT_CONTROL_PRESSED) ||
               (HKey.KeyState.KeyShiftState & EFI_RIGHT_CONTROL_PRESSED)) &&
              (HKey.Key.ScanCode == SCAN_F1)) {
            KBC_Event (
              CpuIo,
              KBC_KB_CMD,
              KBC_KB_DATA,
              &KbcAdd,
              &LEDReset
              );
            Status = gBS->CloseEvent(Events);
            break;
          }
        } else if(EventIndex == 1){
          Blinking (CpuIo);
        }
    }
  }
  
  return Status;
}

VOID
IsLightOn (
  IN BOOLEAN                                BoolScrollLockLED,
  IN BOOLEAN                                BoolNumLockLED,
  IN BOOLEAN                                BoolCapsLockLED
)
{
  if (BoolScrollLockLED) {
    SetCursorPosColor (
      EFI_WHITE | EFI_BACKGROUND_CYAN ,
      RIGHT_DATA_FRAME_HORI_MIN,
      BASE_TITLE_OFFSET
      );
    Print (L"LIGHT ON ");
  } else {
    SetCursorPosColor (
      EFI_LIGHTGRAY| EFI_BACKGROUND_BLACK,
      RIGHT_DATA_FRAME_HORI_MIN,
      BASE_TITLE_OFFSET
      );
    Print (L"         ");
  }

  if (BoolNumLockLED) {
    SetCursorPosColor (
      EFI_WHITE | EFI_BACKGROUND_CYAN ,
      RIGHT_DATA_FRAME_HORI_MIN,
      (BASE_TITLE_OFFSET + 1)
      );
    Print (L"LIGHT ON ");
  } else {
    SetCursorPosColor (
      EFI_LIGHTGRAY| EFI_BACKGROUND_BLACK,
      RIGHT_DATA_FRAME_HORI_MIN,
      (BASE_TITLE_OFFSET + 1)
      );
    Print (L"         ");
  }

  if (BoolCapsLockLED) {
    SetCursorPosColor (
      EFI_WHITE | EFI_BACKGROUND_CYAN ,
      RIGHT_DATA_FRAME_HORI_MIN,
      (BASE_TITLE_OFFSET + 2)
      );
    Print (L"LIGHT ON ");
  } else {
    SetCursorPosColor (
      EFI_LIGHTGRAY| EFI_BACKGROUND_BLACK,
      RIGHT_DATA_FRAME_HORI_MIN,
      (BASE_TITLE_OFFSET + 2)
      );
    Print (L"         ");
  }

  SET_COLOR (
    EFI_LIGHTGRAY| EFI_BACKGROUND_BLACK
    );

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
SelectItems (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN UINTN                                  TotalItems,
  IN CHAR16                                 *BaseFrametitle,
  IN CHAR16                                 **ItemLists,
  IN CHAR16                                 **BaseFrameTail,
  IN BOOLEAN                                BoolScrollLockLED,
  IN BOOLEAN                                BoolNumLockLED,
  IN BOOLEAN                                BoolCapsLockLED
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
   IsLightOn (
     BoolScrollLockLED,
     BoolNumLockLED,
     BoolCapsLockLED
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

