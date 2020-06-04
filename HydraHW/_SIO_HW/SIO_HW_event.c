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

#include"SIO_HW.h"
#include"HugeHeader.h"

EFI_STATUS
DumpSIO (
  IN EFI_CPU_IO2_PROTOCOL     *CpuIo,
  IN OUT UINT8                FrameData[16][16]
  )
{
  EFI_STATUS Status;
  UINT8      CountIndex;
  UINT8      HoriIndex;
  UINT8      VertIndex;
  UINT16      DataOffset;
  UINT8      Cmd;

  DataOffset = 0;
  //
  // Open Table
  //
  Cmd = UNLOCK_CMD;
  for(CountIndex = 0; CountIndex < 2; CountIndex ++){
    Status = CpuIo->Io.Write (
                         CpuIo,
                         EfiPciWidthUint8,
                         0x2E,
                         1,
                         &Cmd
                         );
    if (EFI_ERROR (Status))
      return Status;
  }
  
  //
  //  Read data
  //
  for (HoriIndex = 0 ; HoriIndex < 16 ; HoriIndex++) {
    for (VertIndex = 0; VertIndex < 16; VertIndex++) {
      DataOffset = ((HoriIndex * 15) + HoriIndex) + VertIndex;
      if (DataOffset == LOCK_CMD) {
        continue;
      }
      Status = CpuIo->Io.Write (
                           CpuIo,
                           EfiPciWidthUint8,
                           0x2E,
                           1,
                           &DataOffset
                           );
      if (EFI_ERROR (Status)) {
        return Status;
      }
      Status = CpuIo->Io.Read (
                           CpuIo,
                           EfiPciWidthUint8,
                           0x2F,
                           1,
                           &FrameData[HoriIndex][VertIndex]
                           );
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }
  }
  
  //
  //  Close table
  //
  Cmd = LOCK_CMD;  
  Status = CpuIo->Io.Write (
                       CpuIo,
                       EfiPciWidthUint8,
                       0x2E,
                       1,
                       &Cmd
                       );
  if (EFI_ERROR (Status))
      return Status;
  
  return EFI_SUCCESS;
}

/**
  Write Data to SIO

  @param[in] *CpuIo            EFI_CPU_IO2_PROTOCOL
  @param[in] *MakeUpedData     Write to SIO Device Data   
  @param[in] DataOffset        Offset Of Data.
  @param[in] HType             Type Byte / Word / DWord.
 
  @return Status Code

**/

EFI_STATUS
WriteSIO (
  EFI_CPU_IO2_PROTOCOL *CpuIo,
  UINT32               *MakeUpedData,
  UINT8                DataOffset,
  UINT8                HType
  )
{
  EFI_STATUS Status;
  UINT8      Cmd;
  UINT8      CountIndex;

  //
  // Open Table
  //
  Cmd = UNLOCK_CMD;
  for (CountIndex = 0; CountIndex < 2; CountIndex++) {
    Status = CpuIo->Io.Write (
                         CpuIo,
                         EfiPciWidthUint8,
                         0x2E,
                         1,
                         &Cmd
                         );
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }
  //
  //  Write data
  //
  Status = CpuIo->Io.Write (
                         CpuIo,
                         EfiPciWidthUint8,
                         0x2E,
                         1,
                         &DataOffset
                         );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = CpuIo->Io.Write (
                       CpuIo,
                       EfiPciWidthUint8,
                       0x2F,
                       1,
                       MakeUpedData
                       );

  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  //  Close table
  //
  Cmd = LOCK_CMD;  
  Status = CpuIo->Io.Write (
                       CpuIo,
                       EfiPciWidthUint8,
                       0x2E,
                       1,
                       &Cmd
                       );
  if (EFI_ERROR (Status)) {
      return Status;
  }

  return EFI_SUCCESS;
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
  UINT8              SetSpace;
  UINT8              SelectedItem;
  
  ItemsPerPage = ITEMS_PER_HUGEPAGE;
  PageNumber = 0;
  ItemPosition = 0;
  SelectedItem = NO_SELECT;
  
  if (TotalItems % ItemsPerPage == 0) {
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
    } else if (HKey.Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {

      //
      // return ItemCounts+ItemPosition
      //
		  SelectedItem = ItemCounts + ItemPosition;
      if ((SelectedItem == 0) ||
          (SelectedItem == 4) ||
          (SelectedItem == 15)) {
          SelectedItem = RESERVED;
      }
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

  This code Handle key event and do corresponding func.
  
  @param[in]  *InputEx             Key event Protocol.
  @param[in]  TotalItems           Numbers of items
  @param[in]  **BaseFrameTitle     Base Frame Title.
  @param[in]  **ItemLists          Item lists.
  @param[in]  **BaseFrameTail      Base Frame Tail.
  
*/
UINT8
HugeSelectItems (
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


EFI_STATUS
DataFrameHandler (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN EFI_CPU_IO2_PROTOCOL                   *CpuIo
  )
{

  EFI_KEY_DATA       HKey;
  UINT8              HType;

  UINTN              OrgType;
  UINTN              VertPos;
  UINTN              HoriPos;

  INT8               DataNum;
  INT8               ModifyPos;
  CHAR16             ModifyVal[8] = {'0'};
  UINT32             MakeUpedData;
  BOOLEAN            ModifyFlag;
  UINT8              DataOffset;

  UINT8              SelectedItem;
  UINT8              FrameData[16][16] = {0};

  
  HType = TypeByte;
  OrgType  = NO_TYPE;
  
  ModifyFlag = FALSE;
  DataNum = QUANTITY_PER_TYPE (
              HType
              );
  ModifyPos = 0;
  VertPos = HUGE_TABLE_VERT_MIN;
  HoriPos = HUGE_TABLE_HORI_MIN;

  EN_CURSOR (TRUE);

  DumpSIO (
    CpuIo,
    FrameData
    );
  
  ShowHugeDataFrame (
      HType,
      FrameData
      );
  
  while (TRUE) {
    if (OrgType != HType) {
      ShowHugeDataFrame (
        HType,
        FrameData
        );
      
      DataNum = QUANTITY_PER_TYPE (
                  HType
                  );
      ModifyPos = 0;
      OrgType = HType;
      VertPos = HUGE_TABLE_VERT_MIN;
      HoriPos = HUGE_TABLE_HORI_MIN;
    }

    SET_CUR_POS (
      HoriPos,
      VertPos
      );
    DataOffset = ShowOffset (
                   HoriPos,
                   VertPos,
                   HType
                   );
    
    HKey = keyRead (
             InputEx
             );
    if ((HKey.Key.ScanCode != 0 ) && 
        !((HKey.KeyState.KeyShiftState & EFI_LEFT_CONTROL_PRESSED) ||
          (HKey.KeyState.KeyShiftState & EFI_RIGHT_CONTROL_PRESSED))) {
      
      if (ModifyFlag) {
        ShowHugeDataFrame (
          HType,
          FrameData
          );
        
        DataNum = QUANTITY_PER_TYPE (
                    HType
                    );
        
        ModifyPos = 0;
        ModifyFlag = FALSE;
      }
      
		  switch (HKey.Key.ScanCode) {
        
			case SCAN_UP:
        VertPos--;
        
        if (VertPos < HUGE_TABLE_VERT_MIN) {
          VertPos = HUGE_TABLE_VERT_MAX;
        }
        break;
			case SCAN_DOWN:
        VertPos++;
        
        if (VertPos > HUGE_TABLE_VERT_MAX) {
          VertPos = HUGE_TABLE_VERT_MIN;
        }
        break;
			case SCAN_LEFT:
			case SCAN_RIGHT:
        CurHorizontalMove (
          HKey.Key.ScanCode,
          &HoriPos,
          HType
          );
        break;
      case SCAN_F2:
        HType *= 2;
        if (HType > T_DWord) {
          HType = T_Byte;
        }
        break;
      }
    }else if ((HKey.Key.ScanCode == 0) &&
      ((HKey.Key.UnicodeChar >= '0' && HKey.Key.UnicodeChar <= '9') ||
      (HKey.Key.UnicodeChar >= 'a' && HKey.Key.UnicodeChar <= 'f') ||
      (HKey.Key.UnicodeChar >= 'A' && HKey.Key.UnicodeChar <= 'F'))
      )
    {   
      if (DataNum < 0) {
          ShowHugeDataFrame (
            HType,
            FrameData
            );
        
          memset (
            (char *) ModifyVal,
            0,
            sizeof(ModifyVal)
            );
  				DataNum = QUANTITY_PER_TYPE (
                      HType
                      );
					ModifyPos = 0;
  			}
        
        ModifyFlag = TRUE;
        
				SetCursorPosColor (
          EFI_WHITE | EFI_BACKGROUND_BLUE,
          (HoriPos + ModifyPos),
          VertPos
          );
        
  			ModifyVal[DataNum] = HKey.Key.UnicodeChar;
        
  			Print (
          L"%c",
          ModifyVal[DataNum]
          );
        
  			DataNum--;
				ModifyPos++;
        
  			
  	}else if (HKey.Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
      if (ModifyFlag) {

        ModifyFlag = FALSE;
        
        MakeUpedData = MakeUpData (
                         FrameData,
                         ModifyVal,
                         DataNum,
                         HType,
                         VertPos,
                         HoriPos
                         );

        WriteSIO (
          CpuIo,
          &MakeUpedData,
          DataOffset,
          HType
          );

        DumpSIO (
          CpuIo,
          FrameData
          );
        
        ShowHugeDataFrame (
          HType,
          FrameData
          );
      }

      DataNum = QUANTITY_PER_TYPE (
                  HType
                  );
      
      ModifyPos = 0;
      
      memset (
        (char *) ModifyVal,
        0,
        sizeof(ModifyVal)
        );
      
  	}else if ((HKey.KeyState.KeyShiftState & EFI_SHIFT_STATE_VALID) &&
  	  ((HKey.KeyState.KeyShiftState & EFI_LEFT_CONTROL_PRESSED) ||
  	   (HKey.KeyState.KeyShiftState & EFI_RIGHT_CONTROL_PRESSED)) &&
  	  (HKey.Key.ScanCode == SCAN_F1)) {
       break;
		}
  }
  
  SET_COLOR (
    EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK
    );

}
