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

/**

  This code Handle key event and do corresponding func.
  
  @param[in]  *InputEx             Key event Protocol.
  @param[in]  HandleCount
  @param[in]  *HandleBuffer
  @param[in]  *BaseFrametitle
  @param[in]  **BaseFrameTail      Base Frame Tail.
  
*/
EFI_STATUS
ShowProtocol (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN UINTN                                  HandleIndex,
  IN EFI_HANDLE                             *HandleBuffer,
  IN CHAR16                                 *FrameTitle,
  IN UINT8                                  SelectType,
  IN EFI_GUID                               *TargetGuid OPTIONAL
  )
{
  EFI_STATUS         Status;
  EFI_KEY_DATA       HKey;

  CHAR16             *FrameTail[] = {
                       L"F5 : Exit"
                       };

  UINTN               ProtocolBufferCount = 0;
  EFI_GUID            **ProtocolBuffer = NULL;
  
  UINTN               ProtocolIndex; 
  CHAR16*             GuidName;

  UINT8               ProtocolPosition;
  
  CLEAN_SCREEN (VOID);
  Status = gBS->ProtocolsPerHandle (
                  HandleBuffer[HandleIndex],
                  &ProtocolBuffer, 
                  &ProtocolBufferCount
                  );
  if (EFI_ERROR (Status)) {
     CLEAN_SCREEN();
     Print(L"###ProtocolsPerHandle ERROR###\n");
     return Status;
  }
  BaseFrame (
    FrameTitle, 
    FrameTail[0],
    L"",
    L""
    );
  SetCursorPosColor (
    EFI_BLUE | EFI_BACKGROUND_LIGHTGRAY,
    0,
    BASE_TITLE_OFFSET
    );
  Print (
    L"Handle %4d (%08X) :\n",
    HandleIndex,
    HandleBuffer[HandleIndex]
    );

  ProtocolPosition = 0;
  for (ProtocolIndex = 0; ProtocolIndex < ProtocolBufferCount; ProtocolIndex++) {
    SetCursorPosColor (
      EFI_LIGHTGRAY| EFI_BACKGROUND_BLACK,
      0,
      (BASE_TITLE_OFFSET + ProtocolPosition + 1)
      );
    switch (SelectType) {
    case SearchAllHandles:
      GuidName = GetStringNameFromGuid(ProtocolBuffer[ProtocolIndex],NULL);
      Print(L"%s : %g\n",GuidName,ProtocolBuffer[ProtocolIndex]);
      ProtocolPosition++;
      break;
    case SearchByGuid:
    case SearchByName:
      if ( CompareGuid (TargetGuid,ProtocolBuffer[ProtocolIndex])) {
        GuidName = GetStringNameFromGuid(ProtocolBuffer[ProtocolIndex],NULL);
        Print(L"%s : %g\n",GuidName,ProtocolBuffer[ProtocolIndex]);
        ProtocolPosition++;
      }
      break;
    }
        
    }
  
  while (TRUE) {
    HKey = keyRead (
             InputEx
             );
    if ((HKey.Key.ScanCode == SCAN_F5 ) &&
        !((HKey.KeyState.KeyShiftState & EFI_LEFT_CONTROL_PRESSED) ||
          (HKey.KeyState.KeyShiftState & EFI_RIGHT_CONTROL_PRESSED))) {
       Status = gBS->FreePool (ProtocolBuffer);
       if (EFI_ERROR (Status)) {
         CLEAN_SCREEN();
         Print(L"###ProtocolsPerHandle FreePool ERROR###\n");
         return Status;
       }
       break;
    }
  }
  return Status;
}


/**

  This code Handle key event and do corresponding func.
  
  @param[in]  *InputEx             Key event Protocol.
  @param[in]  HandleCount
  @param[in]  *HandleBuffer
  @param[in]  *BaseFrametitle
  @param[in]  **BaseFrameTail      Base Frame Tail.
  
*/
EFI_STATUS
SelectHandle (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN CHAR16                                 *BaseFrametitle,
  IN CHAR16                                 **BaseFrameTail,
  IN UINTN                                  HandleCount,
  IN EFI_HANDLE                             *HandleBuffer,
  IN UINT8                                  SelectType,
  IN EFI_GUID                               *TargetGuid OPTIONAL
  )
{
  EFI_STATUS         Status;
  EFI_KEY_DATA       HKey;
  UINT16              ItemIndex;
  UINT16              ItemCounts;
  UINT16              PageNumber;
  UINT16              ItemPosition;
  UINT16              MaxPages;
  UINT16              MaxPageItemNumber;
  UINT8              ItemsPerPage;
  UINT8              CleanLine;
  UINT8              SetSpace;
  UINTN              TotalItems;

  
  TotalItems = HandleCount;
  ItemsPerPage = ITEMS_PER_PAGE;
  PageNumber = 0;
  ItemPosition = 0;
  
  if (TotalItems % ItemsPerPage == 0) {
    MaxPages = (TotalItems / ItemsPerPage);
    MaxPageItemNumber = ITEMS_PER_PAGE;
  } else {
    MaxPages = (TotalItems / ItemsPerPage) + 1;
    MaxPageItemNumber = TotalItems % ItemsPerPage;
  }
  
  BaseFrame (
    BaseFrametitle, 
    BaseFrameTail[0],
    BaseFrameTail[1],
    BaseFrameTail[2]
    );
  SetCursorPosColor (
    EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK,
    RIGHT_DATA_FRAME_HORI_MIN,
    BASE_TITLE_OFFSET
    );
  Print (L"HandleCount %d",HandleCount);
  
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
        EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK,
        0,
        (ItemIndex + BASE_TITLE_OFFSET)
        );
      Print (
        L"  | Handle %4d (%08X)\n",
        ItemCounts+ItemIndex,
        HandleBuffer[ItemCounts+ItemIndex]
        );
      
      if (PageNumber == MaxPages - 1) {
        for (CleanLine = (ITEMS_PER_PAGE-1); CleanLine >= ItemsPerPage; CleanLine--) {
          SetCursorPosColor (
            EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK,
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
        L"->| Handle %4d (%08X)\n",
        ItemCounts+ItemPosition,
        HandleBuffer[ItemCounts+ItemPosition]
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
        
        if (ItemPosition == 0xFFFF) {
          
          PageNumber --;
          
          if (PageNumber == 0xFFFF) {
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
       
		  SET_CUR_POS (
        0,
        BASE_FRAME_OFFSET
        );
      
      Print (
        L"\n"
        );
      
			break;
		} else if (HKey.Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
		
      Status = ShowProtocol (
                 InputEx,
                 ItemCounts+ItemPosition,
                 HandleBuffer,
                 BaseFrametitle,
                 SelectType,
                 TargetGuid
                 );
      if (EFI_ERROR (Status)) {
        return Status;
      }
      
      BaseFrame (
          BaseFrametitle, 
          BaseFrameTail[0],
          BaseFrameTail[1],
          BaseFrameTail[2]
          );
      SetCursorPosColor (
        EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK,
        RIGHT_DATA_FRAME_HORI_MIN,
        BASE_TITLE_OFFSET
        );
      Print (L"HandleCount %d",HandleCount);
		}
  }

  return Status;
}

