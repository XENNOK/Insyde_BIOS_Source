/** @file

  Data Frame Handler
  
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

  Write data to pci device
  @param[in]  *HydraBridgeIo       EFI_HYDRA_BRIDGE_IO_PROTOCOL.
  @param[in]  *InputEx             Key event Protocol
  @param[in]  *HPciDev             pci devices struct
  @param[in]  DataIndex            Index of pci devices struct
  @param[in]  *data                Data write to pci devies
  @param[in]  FrameHoriPos         Frame cursor Horizontal Position
  @param[in]  FrameVertPos         Frame cursor Vertical Position
  @param[in]  HType                Type Byte/Word/DWord

  @return Status Code

*/
EFI_STATUS 
WritePCIData (
  IN EFI_HYDRA_BRIDGE_IO_PROTOCOL        *HydraBridgeIo,
  IN PCIDEV                              *HPciDev,
  IN UINTN                               DataIndex,
  IN UINT32                              *data,
  IN UINT8                               FrameHoriPos,
  IN UINT8                               FrameVertPos,
  IN UINTN                               HType
  )
{
  EFI_STATUS Status;
  UINT8      OffSet;
  UINT8      HoriPos;
  UINT8      VertPos;
  
  HoriPos = FrameHoriPos - HUGE_TABLE_HORI_MIN;
  VertPos = FrameVertPos - HUGE_TABLE_VERT_MIN;
  OffSet = 0;
  
  switch (HType) {
	case TypeByte:
     OffSet = ((HoriPos / 3) + ((VertPos * 15) + VertPos));
     
	   Status = HydraBridgeIo->HPci.Write(
                                    HydraBridgeIo,
                                    EfiPciWidthUint8,
                                    EFI_PCI_ADDRESS (
                                      HPciDev[DataIndex].BusNum,
                                      HPciDev[DataIndex].DevNum,
                                      HPciDev[DataIndex].FunNum,
                                      OffSet
                                      ),
                                    1,
                                    data
                                    );
	  break;
	case TypeWord:
     OffSet = (((HoriPos / 5) * 2) + ((VertPos * 15) + VertPos));
     
	   Status = HydraBridgeIo->HPci.Write(
                                    HydraBridgeIo,
                                    EfiPciWidthUint16,
                                    EFI_PCI_ADDRESS (
                                      HPciDev[DataIndex].BusNum,
                                      HPciDev[DataIndex].DevNum,
                                      HPciDev[DataIndex].FunNum,
                                      OffSet
                                      ),
                                    1,
                                    data
                                    );
	  break;
	case TypeDWord:
     OffSet = (((HoriPos / 8) * 4) + ((VertPos * 15) + VertPos));
     
	   Status = HydraBridgeIo->HPci.Write(
                                    HydraBridgeIo,
                                    EfiPciWidthUint32,
                                    EFI_PCI_ADDRESS (
                                    HPciDev[DataIndex].BusNum,
                                    HPciDev[DataIndex].DevNum,
                                    HPciDev[DataIndex].FunNum,
                                    OffSet
                                    ),
                                    1,
                                    data
                                    );
	  break;
	default:
		  Status = EFI_OUT_OF_RESOURCES;
		break;
	}
  return Status;
}

/**

  pow function is calculation hexadecimal to the power of MakeUpDataNum
  @param[in]  MakeUpDataNum   Make Up data numbers
  
  @return PowResult

*/
UINT32
Pow(
  IN UINT32    MakeUpDataNum
  )
{
  UINT32 PowResult;
  
  PowResult = 1;

  while (MakeUpDataNum != 0){
    PowResult *= 16;
    --MakeUpDataNum;
  }
  return PowResult;
}


/**

  Transfer Data to UINT32
  @param[in]   *writedata       data needs to transfet to UINT32
  @param[in]   HType            Type Byte/Word/DWord

  @return      WriteVal         transfered data

*/
UINT32 
AscToInt(
  IN CHAR16        *writedata,
  IN UINTN         HType
  )
{
  UINT8    INTVal[8]= {0};
  UINT32   WriteVal;
  UINTN    TypeNum;
  UINT8    Num;
  
  TypeNum = DATA_NUM (
              HType
              );
  WriteVal = 0 ;
  
  for (Num=0; Num <= TypeNum; Num++) {
    
    if (writedata[Num] >= '0' && writedata[Num] <= '9') {
      
      INTVal[Num] = (UINT8) (writedata[Num] - '0');
      
    }else if (writedata[Num] >= 'a' && writedata[Num] <= 'f') {
    
      INTVal[Num] = (UINT8) ((writedata[Num] - 'a') + 10);
      
    }else if (writedata[Num] >= 'A' && writedata[Num] <= 'F') {
    
      INTVal[Num] = (UINT8) ((writedata[Num] - 'A') + 10);
      
    }
		WriteVal += (INTVal[Num] << ( 4 * Num));
  }
  
  return WriteVal;
}

/*
 Show Offset.

 @param[in]  HoriPos  Horizantal Position
 @param[in]  VertPos  Vertical   Position
 @param[in]  HType    Byte / Word /Dword

 @return Offset Value
*/

UINT8 
ShowOffset (
  UINT8 HoriPos,
  UINT8 VertPos,
  UINT8 HType
  )
{
	UINT8 CalculationOffset;
  UINT8 DisplayOffset;
  UINT8 HoriPosition;
  UINT8 VertPosition;
  
  HoriPosition = HoriPos-HUGE_TABLE_HORI_MIN;
  VertPosition = VertPos-HUGE_TABLE_VERT_MIN;
  
	switch(HType){
		case TypeByte:
		  CalculationOffset = ((HoriPosition/3) + ((VertPosition*15)+VertPosition));
      DisplayOffset = CalculationOffset;
		break;
		case TypeWord:
		  CalculationOffset = (((HoriPosition/5)*2) + ((VertPosition*15)+VertPosition));
      DisplayOffset = CalculationOffset + 1;
		break;
		case TypeDWord:
		  CalculationOffset = (((HoriPosition/8)*4) + ((VertPosition*15)+VertPosition));
      DisplayOffset = CalculationOffset + 3;
		break;
		default:
		  CalculationOffset = 0;
      DisplayOffset = CalculationOffset;
		break;
	}

  SetCursorPosColor (
    EFI_BLACK | EFI_BACKGROUND_LIGHTGRAY,
    HUGE_FRAME_VERTTITLE_OFFSET,
    HUGE_FRAME_HORITITLE_OFFSET
    );
  Print (
    L"%02X",
    DisplayOffset
    );
	SetCursorPosColor (
    EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK,
    HoriPos,
    VertPos
    );
  return CalculationOffset;
}

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
  
  UINT32             PciData;
  
  UINT32             OrgData;
  UINT32             MakeUpVal;

  PciData = 0;
  OrgData = 0;
  MakeUpVal = 0;
  VPos = 0;
  HPos = 0;
  MakeUpDataNum = DataNum+1;

  PciData = AscToInt (
              ModifyVal,
              HType
              );

  if (DataNum == DATA_NUM (HType)) {
    
    return PciData;
    
  } else {
  
    switch (HType) {
      
    case TypeByte:
      HPos = (HoriPos - HUGE_TABLE_HORI_MIN) / 3;
      VPos = (VertPos - HUGE_TABLE_VERT_MIN);
      break;
  	case TypeWord:
      HPos = ((HoriPos - HUGE_TABLE_HORI_MIN) / 5) * 2;
      VPos = (VertPos - HUGE_TABLE_VERT_MIN);
      break;
    case TypeDWord:
      HPos = ((HoriPos- HUGE_TABLE_HORI_MIN) / 8) * 4;
      VPos = (VertPos - HUGE_TABLE_VERT_MIN);
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

  Frame Cursor Horizontal Move
  @param[in]        ScanCode        key event
  @param[in] [out]  *HoriPos        Position of cursor
  @param[in]        HType           Type Byte/Word/DWord

*/
VOID 
HugeCurHorizontalMove (
  IN UINT16             ScanCode,
  IN OUT INT8           *HoriPos,
  IN UINTN              HType
  )
{
	switch (HType) {
	case TypeByte:
    if (ScanCode == SCAN_LEFT) {
			*HoriPos -= 3;
      
			if (*HoriPos < HUGE_TABLE_HORI_MIN) {
				*HoriPos = (HUGE_TABLE_HORI_BYTE_MAX-1);
			}
		} else if (ScanCode == SCAN_RIGHT) {
			*HoriPos += 3;
      
			if (*HoriPos > HUGE_TABLE_HORI_BYTE_MAX) {
				*HoriPos = HUGE_TABLE_HORI_MIN;
			}
		}
	  break;
	case TypeWord:
	  if (ScanCode == SCAN_LEFT) {
			*HoriPos -= 5;
      
			if (*HoriPos < HUGE_TABLE_HORI_MIN) {
				*HoriPos = (HUGE_TABLE_HORI_WORD_MAX-4);
			}
		} else if (ScanCode == SCAN_RIGHT) {
			*HoriPos += 5;
      
			if (*HoriPos > HUGE_TABLE_HORI_WORD_MAX) {
				*HoriPos = HUGE_TABLE_HORI_MIN;
			}
		}
	  break;
	case TypeDWord:
	 if (ScanCode == SCAN_LEFT) {
			*HoriPos -= 9;
      
			if (*HoriPos < HUGE_TABLE_HORI_MIN) {
				*HoriPos = (HUGE_TABLE_HORI_DWORD_MAX-8);
			}
		} else if (ScanCode == SCAN_RIGHT) {
			*HoriPos += 9;
      
			if (*HoriPos > HUGE_TABLE_HORI_DWORD_MAX) {
				*HoriPos = HUGE_TABLE_HORI_MIN;
			}
		}
	  break;
	}
}


/**

  Right Data Frame, show frame table ascii value or human read data
  @param[in]  *HPciDev             pci devices struct
  @param[in]  AscOrData            boolean, show ascii value or human read data
  
*/
VOID RightDataFrame (
  IN PCIDEV    HPciDev,
  IN BOOLEAN   AscOrData
  )
{

  UINT8 ROWS;
  UINT8 COLS;

  CleanFrame (
    EFI_BACKGROUND_LIGHTGRAY,
    DATA_FRAME_OFFSET,
    (DATA_FRAME_OFFSET + 17),
    HUGE_TABLE_HORI_MAX,
    HUGE_FRAME_HORI_MAX
    );

  CleanFrame (
    EFI_BACKGROUND_BLUE,
    HUGE_TITLE_OFFSET,
    (HUGE_TITLE_OFFSET + 1),
    HUGE_TABLE_HORI_MAX,
    HUGE_FRAME_HORI_MAX
    );
  SetCursorPosColor (
    EFI_LIGHTGRAY| EFI_BACKGROUND_BLUE,
    HUGE_TABLE_HORI_MAX,
    HUGE_TITLE_OFFSET
    );
  
  if (AscOrData) {

    Print (
      L"        ASCII Code       \n"
      );

    for (ROWS = 0; ROWS <= DATA_ARRAY_ROW_MAX; ROWS++) {
      for (COLS = 0; COLS <= DATA_ARRAY_COLS_MAX; COLS++) {
        SetCursorPosColor (
          EFI_BLACK| EFI_BACKGROUND_LIGHTGRAY,
          (HUGE_TABLE_HORI_MAX+COLS+5),
          (HUGE_TITLE_OFFSET+ROWS+1)
          );
        
        if (((HPciDev.PcidevData[ROWS][COLS] >= 0x21 ) &&
           (HPciDev.PcidevData[ROWS][COLS] <= 0x7E))) {
          Print (
            L"%c",
            HPciDev.PcidevData[ROWS][COLS]
            );
        } else {
          Print (
            L"."
            );
        }
        
      }
    }
    
  } else {
    Print (
      L"    PCI Data  Summary    \n"
      );
    
    SetCursorPosColor (
      EFI_BLACK| EFI_BACKGROUND_LIGHTGRAY,
      HUGE_TABLE_HORI_MAX,
      (HUGE_TITLE_OFFSET + 1)
      );
    Print (
      L" VendorID  :     %04X\n",
      HPciDev.Pci.Hdr.VendorId
      );
    
    SET_CUR_POS (
      HUGE_TABLE_HORI_MAX,
      (HUGE_TITLE_OFFSET + 2)
      );
    Print (
      L" DeviceID  :     %04X\n",
      HPciDev.Pci.Hdr.DeviceId
      );
  
    SET_CUR_POS (
      HUGE_TABLE_HORI_MAX,
      (HUGE_TITLE_OFFSET + 3)
      );
    Print (
      L" RevisionID:     %02X\n",
      HPciDev.Pci.Hdr.RevisionID
      );
    
    SET_CUR_POS (
      HUGE_TABLE_HORI_MAX,
      (HUGE_TITLE_OFFSET + 4)
      );
    Print (
      L" HeaderType:     %02X\n",
      HPciDev.Pci.Hdr.HeaderType
      );
  
    SET_CUR_POS (
      HUGE_TABLE_HORI_MAX,
      (HUGE_TITLE_OFFSET + 5)
      );
    Print (
      L" ClassCode :     %02X%02X%02X\n",
      HPciDev.Pci.Hdr.ClassCode[2],
      HPciDev.Pci.Hdr.ClassCode[1],
      HPciDev.Pci.Hdr.ClassCode[0]
      );
    
    SET_CUR_POS (
      HUGE_TABLE_HORI_MAX,
      (HUGE_TITLE_OFFSET + 6)
      );
    Print (
      L" SubSystemID :   %04X\n",
      HPciDev.Pci.Device.SubsystemID
      );
    
    SET_CUR_POS (
      HUGE_TABLE_HORI_MAX,
      (HUGE_TITLE_OFFSET + 7)
      );
    Print (
      L" SubSystemVID:   %04X\n",
      HPciDev.Pci.Device.SubsystemVendorID
      );
    
    SET_CUR_POS (
      HUGE_TABLE_HORI_MAX,
      (HUGE_TITLE_OFFSET + 8)
      );
    Print (
      L" Interrupt Pin:  %02X\n",
      HPciDev.Pci.Device.InterruptPin
      );
    
    SET_CUR_POS (
      HUGE_TABLE_HORI_MAX,
      (HUGE_TITLE_OFFSET + 9)
      );
    Print (
      L" Interrupt Line: %02X\n",
      HPciDev.Pci.Device.InterruptLine
      );

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
  IN PCIDEV    HPciDev,
  IN BOOLEAN   AscOrData
  )
{
  UINTN   DataVert;
  UINTN   DataHori;
	INT8    TypeVal;

  SetCursorPosColor (
    EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE,
    HUGE_FRAME_VERTTITLE_OFFSET,
    HUGE_TITLE_OFFSET
    );
  Print (L"   |                 PCI Data Table                  |\n");
  
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
    
    CleanFrame (
      EFI_BACKGROUND_BLACK,
      (UINT8)(HUGE_FRAME_HORI_OFFSET + DataVert),
      (UINT8)(HUGE_FRAME_HORI_OFFSET + DataVert +1),
      HUGE_FRAME_VERT_OFFSET,
      HUGE_TABLE_HORI_MAX
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
        if (HPciDev.PcidevData[DataVert][DataHori+TypeVal] != 0x00) {
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
          HPciDev.PcidevData[DataVert][DataHori+TypeVal]
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

  RightDataFrame (
    HPciDev,
    AscOrData
    );
    
  SET_COLOR (
    EFI_LIGHTGRAY|EFI_BACKGROUND_BLACK
    );  //Reset Color

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
  IN EFI_HYDRA_BRIDGE_IO_PROTOCOL           *HydraBridgeIo,
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN PCIDEV                                 *HPciDev,
  IN UINTN                                  DataIndex
  )
{
  EFI_STATUS         Status;
  EFI_KEY_DATA       HKey;
  UINT8              HType;
  UINTN              OrgType;
  UINT8              VertPos;
  UINT8              HoriPos;

  INT8               DataNum;
  INT8               ModifyPos;
  CHAR16             ModifyVal[8] = {'0'};
  UINT32             PciData;
  BOOLEAN            ModifyFlag;
  BOOLEAN            OrgAscOrData;
  BOOLEAN            AscOrData;


  HType = TypeByte;  //Default Type
  OrgType  = NO_TYPE;
  OrgAscOrData = FALSE;
  
  AscOrData  =  FALSE;
  ModifyFlag = FALSE;
  
  DataNum = DATA_NUM (
              HType
              );
  
  ModifyPos = 0;
  VertPos = HUGE_TABLE_VERT_MIN;
  HoriPos = HUGE_TABLE_HORI_MIN;
  
  CleanFrame (
    EFI_BACKGROUND_BLACK,
    HUGE_TITLE_OFFSET,
    HUGE_TAIL_OFFSET,
    0,
    HUGE_FRAME_HORI_MAX
    );
  
  ShowHugeDataFrame (
    HType,
    HPciDev[DataIndex],
    AscOrData
    );
  
  EN_CURSOR (
    TRUE
    );
  
  while (1) {
    
    if ((OrgType != HType) ||
        (OrgAscOrData != AscOrData)) {
      ShowHugeDataFrame (
        HType,
        HPciDev[DataIndex],
        AscOrData
        );
      
      DataNum = DATA_NUM (
                  HType
                  );
      ModifyPos = 0;
      OrgType = HType;
      OrgAscOrData = AscOrData;
      VertPos = HUGE_TABLE_VERT_MIN;
      HoriPos = HUGE_TABLE_HORI_MIN;
    }

   ShowOffset (
     HoriPos,
     VertPos,
     HType
     );
    
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
        ShowHugeDataFrame (
          HType,
          HPciDev[DataIndex],
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
        HugeCurHorizontalMove (
          HKey.Key.ScanCode,
          &HoriPos,
          HType
          );
        break;
      case SCAN_F2:
        HType *=2;
        if (HType > TypeDWord) {
          HType = TypeByte;
        }
        break;
      case SCAN_F3:
        AscOrData = !AscOrData;
        break;
      case SCAN_F1:
        HydraBridgeIo->HPci.Dump (
                              HPciDev,
                              DataIndex
                              );

        ShowHugeDataFrame (
          HType,
          HPciDev[DataIndex],
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
          ShowHugeDataFrame (
            HType,
            HPciDev[DataIndex],
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
                   HydraBridgeIo,
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
         
        HydraBridgeIo->HPci.Dump (
                              HPciDev,
                              DataIndex
                              );

        ShowHugeDataFrame (
          HType,
          HPciDev[DataIndex],
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
  	  (HKey.Key.ScanCode == SCAN_F1)) {
       break;
		}
  }
  
  SET_COLOR (
    EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK
    );
  CleanFrame (
    EFI_BACKGROUND_BLACK,
    HUGE_TITLE_OFFSET,
    HUGE_TAIL_OFFSET,
    0,
    HUGE_FRAME_HORI_MAX
    );
}

