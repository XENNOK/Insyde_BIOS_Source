/** @file

  PCI Utility searching PCI Decives, show and Modify PCI Devices Data

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

#include"PCI_Utility_HW.h"

/**

  This code Make up User Type Value when it's incomplete.
  @param[in]  *HPciDev            pci devices struct
  @param[in]  *ModifyVal          User type value
  @param[in]  DataIndex           Index of pci devices struct
  @param[in]  DataNum             Un-typed data number
  @param[in]  HType               Type Byte/Word/DWord
  @param[in]  VertPos             Vertical Position of data frame
  @param[in]  HoriPos             Horizontal Position of data frame
  
  @return PciData             Make Up complete data
*/
UINT32
MakeUpData (
  IN PCIDEV             *HPciDev,
  IN CHAR16             *ModifyVal,
  IN UINTN              DataIndex,
  IN INT8               DataNum,
  IN UINTN              HType,
  IN UINTN              VertPos,
  IN UINTN              HoriPos
  )
{
  INT8               TypeVal;
  UINTN              VPos;
  UINTN              HPos;
  INT8               MakeUpDataNum;
  
  CHAR16             Data[8];
  CHAR16             *DataStr = {'0'};
  UINT32             PciData;
  
  UINT32             OrgData;
  UINT32             MakeUpVal;

  PciData = 0;
  OrgData = 0;
  MakeUpVal = 0;
  
  MakeUpDataNum = DataNum+1;

  PciData = AscToInt (
              ModifyVal,
              HType
              );

  if (DataNum == DATA_NUM (HType)) {
    
    return PciData;
    
  } else {
  
    switch (HType) {
      
    case T_Byte:
      HPos = (HoriPos - DATA_TABLE_HORI_MIN) / 3;
      VPos = (VertPos - DATA_TABLE_VERT_MIN);
      break;
  	case T_Word:
      HPos = ((HoriPos - DATA_TABLE_HORI_MIN) / 5) * 2;
      VPos = (VertPos - DATA_TABLE_VERT_MIN);
      break;
    case T_DWord:
      HPos = ((HoriPos- DATA_TABLE_HORI_MIN) / 8) * 4;
      VPos = (VertPos - DATA_TABLE_VERT_MIN);
      break;
    }
    
    for (TypeVal = (INT8) HType - 1; TypeVal >= 0; TypeVal--) {

      OrgData += ((HPciDev[DataIndex].PcidevData[VPos][HPos+TypeVal]) << (8*TypeVal));
      
	  }
    MakeUpVal = OrgData % (Pow (MakeUpDataNum));
    PciData = PciData + MakeUpVal;

    return PciData;
  }
}

/**

  This code Handle key event and do corresponding func
  @param[in]  *PciRootBridgeIo     PciRootBridegIo Protocol
  @param[in]  *InputEx             Key event Protocol
  @param[in]  *HPciDev             pci devices struct
  @param[in]  DataIndex            Index of pci devices struct

*/
VOID
DataFrameHandler (
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *PciRootBridgeIo,
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN PCIDEV                                 *HPciDev,
  IN UINTN                                  DataIndex
  )
{
  EFI_STATUS         Status;
  EFI_KEY_DATA       HKey;
  UINTN              HType;
  UINTN              OrgType;
  UINTN              VertPos;
  UINTN              HoriPos;

  INT8               DataNum;
  INT8               ModifyPos;
  CHAR16             ModifyVal[8] = {'0'};
  UINT32             PciData;
  BOOLEAN            ModifyFlag;
  BOOLEAN            OrgAscOrData;
  BOOLEAN            AscOrData;
  CHAR16             *FramePageTail[] = { 
                        L" Ctrl+Left : Exit | F1 : ReFresh   | Enter: Confirm Modify",
                        L" Left / Right Button: Select Item  | Up / Down Botton: Select Previous",
                        L" F2 : Change BYTE / WORD / DWORD   | F3 : Change Right Frame"
                        };

  HType = T_Byte;  //Default Type
  OrgType  = NO_TYPE;
  OrgAscOrData = FALSE;
  
  AscOrData  =  FALSE;
  ModifyFlag = FALSE;
  DataNum = DATA_NUM (
              HType
              );
  
  ModifyPos = 0;
  VertPos = DATA_TABLE_VERT_MIN;
  HoriPos = DATA_TABLE_HORI_MIN;
  
  CLEAN_SCREEN (
    VOID
    );
  
  DataBaseFrame (
    HPciDev[DataIndex].BusNum,
    HPciDev[DataIndex].DevNum,
    HPciDev[DataIndex].FunNum,
    FramePageTail[0],
    FramePageTail[1],
    FramePageTail[2]
    );
  
  ShowDataFrame (
    HPciDev[DataIndex],
    HType,
    AscOrData
    );
  
  EN_CURSOR (
    TRUE
    );
  
  while (1) {
    
    if ((OrgType != HType) ||
        (OrgAscOrData != AscOrData)) {
      ShowDataFrame (
        HPciDev[DataIndex],
        HType,
        AscOrData
        );
      
      DataNum = DATA_NUM (
                  HType
                  );
      ModifyPos = 0;
      OrgType = HType;
      OrgAscOrData = AscOrData;
      VertPos = DATA_TABLE_VERT_MIN;
      HoriPos = DATA_TABLE_HORI_MIN;
    }
    
    SET_CUR_POS (
      HoriPos,
      VertPos
      );

    HKey = keyRead (
             InputEx
             );
    
    if ((HKey.Key.ScanCode != 0 ) && 
        !((HKey.KeyState.KeyShiftState & EFI_LEFT_CONTROL_PRESSED) ||
          (HKey.KeyState.KeyShiftState & EFI_RIGHT_CONTROL_PRESSED))) {
      
      if (ModifyFlag) {
        ShowDataFrame (
          HPciDev[DataIndex],
          HType,
          AscOrData
          );
        
        DataNum = DATA_NUM (
                    HType
                    );
        
        ModifyPos = 0;
        ModifyFlag = FALSE;
      }
      
		  switch (HKey.Key.ScanCode) {
        
			case SCAN_UP:
        VertPos--;
        
        if (VertPos < DATA_TABLE_OFFSET_VERT) {
          VertPos = DATA_TABLE_VERT_MAX;
        }
        break;
			case SCAN_DOWN:
        VertPos++;
        
        if (VertPos > DATA_TABLE_VERT_MAX) {
          VertPos = DATA_TABLE_OFFSET_VERT;
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
        HType *=2;
        if (HType > T_DWord) {
          HType = T_Byte;
        }
        break;
      case SCAN_F3:
        AscOrData = !AscOrData;
        break;
      case SCAN_F1:
         DumpPCIData (
          PciRootBridgeIo,
          HPciDev,
          DataIndex
          );

        ShowDataFrame (
          HPciDev[DataIndex],
          HType,
          AscOrData
          );
        break;
      }
    }else if ((HKey.Key.ScanCode == 0) &&
      ((HKey.Key.UnicodeChar >= '0' && HKey.Key.UnicodeChar <= '9') ||
      (HKey.Key.UnicodeChar >= 'a' && HKey.Key.UnicodeChar <= 'f') ||
      (HKey.Key.UnicodeChar >= 'A' && HKey.Key.UnicodeChar <= 'F'))
      )
    {   
        if (DataNum < 0) {
          ShowDataFrame (
            HPciDev[DataIndex],
            HType,
            AscOrData
            );
        
          memset (
            (char *) ModifyVal,
            0,
            sizeof(ModifyVal)
            );
  				DataNum = DATA_NUM (
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
        
        PciData = MakeUpData (
                    HPciDev,
                    ModifyVal,
                    DataIndex,
                    DataNum,
                    HType,
                    VertPos,
                    HoriPos
                    );
        
        Status = WritePCIData (
                   PciRootBridgeIo,
                   HPciDev,
                   DataIndex,
                   &PciData,
                   HoriPos,
                   VertPos,
                   HType
                   );
        
        if (EFI_ERROR (Status)) {
          
   		    SetCursorPosColor (
            EFI_BLACK | EFI_BACKGROUND_LIGHTGRAY,
            41,
            0
            );
         
          Print (
            L"WritePCIDataError"
            );
        }
         
        DumpPCIData (
          PciRootBridgeIo,
          HPciDev,
          DataIndex
          );

        ShowDataFrame (
          HPciDev[DataIndex],
          HType,
          AscOrData
          );
  	  }
      
      DataNum = DATA_NUM (
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
  	  (HKey.Key.ScanCode == SCAN_LEFT)) {
       break;
		}
  }
  
  SET_COLOR (
    EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK
    );
}

/**

  This code Handle key event and do corresponding func.
  @param[in]  *PciRootBridgeIo     PciRootBridegIo Protocol.
  @param[in]  *InputEx             Key event Protocol.
  @param[in]  *HPciDev             pci devices struct.
  @param[in]  DataIndex            Index of pci devices struct.
  @param[in]  TotalItems           Numbers of Pci devices.
  @param[in]  **BaseFrameTail      Base Frame Tail.
  
*/
VOID
SelectItems (
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *PciRootBridgeIo,
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN PCIDEV                                 *HPciDev,
  IN UINTN                                  TotalItems,
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
        L"   BUS=%03d, DEV=%03d, FUN=%02d\n",
        HPciDev[ItemCounts+ItemIndex].BusNum,
        HPciDev[ItemCounts+ItemIndex].DevNum,
        HPciDev[ItemCounts+ItemIndex].FunNum
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
      L"-> BUS=%03d, DEV=%03d, FUN=%02d\n",
      HPciDev[ItemCounts+ItemPosition].BusNum,
      HPciDev[ItemCounts+ItemPosition].DevNum,
      HPciDev[ItemCounts+ItemPosition].FunNum
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
       
		  SET_CUR_POS (
        0,
        BASE_FRAME_OFFSET
        );
      
      Print (
        L"\n"
        );
      
			break;
		}else if (HKey.Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
		
		  DataFrameHandler (
        PciRootBridgeIo,
        InputEx,
        HPciDev,
        (ItemCounts + ItemPosition)
        );
      
      BaseFrame (
        L"EFI PCI Utility",
        BaseFrameTail[0],
        BaseFrameTail[1],
        BaseFrameTail[2]
        );
		}
  }
}

/**
  PCI_Utility Entry Point.

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
  
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *PciRootBridgeIo;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx = NULL;
  UINT16                                 TotalDevNum;
  PCIDEV                                 HPciDev[100];
  PCIDEV                                 *PciDevPtr;
  CHAR16                                 *BaseFrameTail[] = { 
                                            L"Ctrl+F1 : Exit | Enter : Select Item,",
                                            L"Up Botton: Select Previous Item",
                                            L"Down Botton: Select Next Item"
                                            };
  CHAR16                                 **BaseFrameTailPtr;
  
  PciDevPtr = HPciDev;
  BaseFrameTailPtr = BaseFrameTail;
  
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
    Print (
      L"SetMode ERROR\n"
      );
  }
  
  Status = gBS->LocateProtocol (
                  &gEfiPciRootBridgeIoProtocolGuid,
                  NULL,
                  &PciRootBridgeIo
                  );
  
	if (EFI_ERROR (Status)) {
    Print (
      L"LocateProtocol ERROR \n"
      );
    
		return Status;		
	}
  
  Status = gBS->LocateProtocol (
                  &gEfiSimpleTextInputExProtocolGuid,
                  NULL,
                  (VOID **) &InputEx
                  );
  
  
  TotalDevNum = GrepPCI0Devices (
                  PciRootBridgeIo,
                  PciDevPtr
                  ); 
  
  BaseFrame (
    L"EFI PCI Utility",
    BaseFrameTail[0],
    BaseFrameTail[1],
    BaseFrameTail[2]
    );
  
  SelectItems (
    PciRootBridgeIo,
    InputEx,
    PciDevPtr,
    TotalDevNum,
    BaseFrameTailPtr
    );
  
  CLEAN_SCREEN (
    VOID
    ); 

  Status = gST->ConOut->SetMode (
                          gST->ConOut,
                          OringinalMode
                          );
  
  if (EFI_ERROR (Status)) {
    Print (
      L"SetMode ERROR\n"
      );
  }
  
  return EFI_SUCCESS;
}
