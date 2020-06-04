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

#include "HugeHeader.h"
#include "SIO_HW.h"

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
		case T_Byte:
		  CalculationOffset = ((HoriPosition/3) + ((VertPosition*15)+VertPosition));
      DisplayOffset = CalculationOffset;
		break;
		case T_Word:
		  CalculationOffset = (((HoriPosition/5)*2) + ((VertPosition*15)+VertPosition));
      DisplayOffset = CalculationOffset + 1;
		break;
		case T_DWord:
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
  IN UINT8              FrameData[16][16],
  IN CHAR16             *ModifyVal,
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
      HPos = (HoriPos - HUGE_TABLE_HORI_MIN) / 3;
      VPos = (VertPos - HUGE_TABLE_VERT_MIN);
      break;
  	case T_Word:
      HPos = ((HoriPos - HUGE_TABLE_HORI_MIN) / 5) * 2;
      VPos = (VertPos - HUGE_TABLE_VERT_MIN);
      break;
    case T_DWord:
      HPos = ((HoriPos- HUGE_TABLE_HORI_MIN) / 8) * 4;
      VPos = (VertPos - HUGE_TABLE_VERT_MIN);
      break;
    }
    
    for (TypeVal = (INT8) HType - 1; TypeVal >= 0; TypeVal--) {

      OrgData += ((FrameData[VPos][HPos+TypeVal]) << (8*TypeVal));
      
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
	case T_Byte:
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
	case T_Word:
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
	case T_DWord:
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

  Show Data Frame Table
  @param[in]  *HPciDev             pci devices struct
  @param[in]  HType                Type Byte/Word/DWord
  @param[in]  AscOrData            boolean, show ascii value or human read data

*/
VOID
ShowHugeDataFrame (
  IN UINTN     HType,
  UINT8        FrameData[16][16]
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
	case T_Byte:
    Print(
      L"   | 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |\n"
      ); 
	  break;
	case T_Word:
	  Print(
      L"   | 0100 0302 0504 0706 0908 0B0A 0D0C 0F0E         |\n"
      );
	  break;
	case T_DWord:
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
        if (FrameData[DataVert][DataHori+TypeVal] != 0x00) {
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
          FrameData[DataVert][DataHori+TypeVal]
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
  
  SET_COLOR (
    EFI_LIGHTGRAY|EFI_BACKGROUND_BLACK
    );  //Reset Color

}

