/** @file

Data Frame functions  

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

#include "HydraLibInternal.h"
#include <Library/HydraLib.h>

/**

  Write data to pci device
  @param[in]  *PciRootBridgeIo     PciRootBridegIo Protocol
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
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL     *PciRootBridgeIo,
  IN PCIDEV                              *HPciDev,
  IN UINT8                               DataIndex,
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
  
  HoriPos = FrameHoriPos - DATA_TABLE_HORI_MIN;
  VertPos = FrameVertPos - DATA_TABLE_VERT_MIN;
  OffSet = 0;
  
  switch (HType) {
	case T_Byte:
     OffSet = ((HoriPos / 3) + ((VertPos * 15) + VertPos));
     
	   Status = PciRootBridgeIo->Pci.Write (
                                 PciRootBridgeIo,
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
	case T_Word:
     OffSet = (((HoriPos / 5) * 2) + ((VertPos * 15) + VertPos));
     
	   Status = PciRootBridgeIo->Pci.Write (
                                 PciRootBridgeIo,
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
	case T_DWord:
     OffSet = (((HoriPos / 8) * 4) + ((VertPos * 15) + VertPos));
     
	   Status = PciRootBridgeIo->Pci.Write (
                                 PciRootBridgeIo,
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

/**

  Frame Cursor Horizontal Move
  @param[in]        ScanCode        key event
  @param[in] [out]  *HoriPos        Position of cursor
  @param[in]        HType           Type Byte/Word/DWord

*/
VOID 
CurHorizontalMove (
  IN UINT16             ScanCode,
  IN OUT INT8           *HoriPos,
  IN UINTN              HType
  )
{
	switch (HType) {
	case T_Byte:
    if (ScanCode == SCAN_LEFT) {
			*HoriPos -= 3;
      
			if (*HoriPos < DATA_TABLE_HORI_MIN) {
				*HoriPos = (DATA_TABLE_HORI_BYTE_MAX-1);
			}
		} else if (ScanCode == SCAN_RIGHT) {
			*HoriPos += 3;
      
			if (*HoriPos > DATA_TABLE_HORI_BYTE_MAX) {
				*HoriPos = DATA_TABLE_HORI_MIN;
			}
		}
	  break;
	case T_Word:
	  if (ScanCode == SCAN_LEFT) {
			*HoriPos -= 5;
      
			if (*HoriPos < DATA_TABLE_HORI_MIN) {
				*HoriPos = (DATA_TABLE_HORI_WORD_MAX-4);
			}
		} else if (ScanCode == SCAN_RIGHT) {
			*HoriPos += 5;
      
			if (*HoriPos > DATA_TABLE_HORI_WORD_MAX) {
				*HoriPos = DATA_TABLE_HORI_MIN;
			}
		}
	  break;
	case T_DWord:
	 if (ScanCode == SCAN_LEFT) {
			*HoriPos -= 9;
      
			if (*HoriPos < DATA_TABLE_HORI_MIN) {
				*HoriPos = (DATA_TABLE_HORI_DWORD_MAX-8);
			}
		} else if (ScanCode == SCAN_RIGHT) {
			*HoriPos += 9;
      
			if (*HoriPos > DATA_TABLE_HORI_DWORD_MAX) {
				*HoriPos = DATA_TABLE_HORI_MIN;
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
  UINT8 BG_Index;

  UINT8 ROWS;
  UINT8 COLS;
  
  for (BG_Index = 0; BG_Index < 17; BG_Index++) {

    SetCursorPosColor (
      EFI_BLUE | EFI_BACKGROUND_LIGHTGRAY,
      RIGHT_DATA_FRAME_HORI_MIN,
      (BASE_TITLE_OFFSET + BG_Index)
      );
    
    Print (
      L"                         " //25
      ); 
  }
  
  SetCursorPosColor (
    EFI_LIGHTGRAY| EFI_BACKGROUND_BLUE,
    RIGHT_DATA_FRAME_HORI_MIN,
    BASE_TITLE_OFFSET
    );
  
  if (AscOrData) {

    Print (
      L"        ASCII Code       \n"
      );

    for (ROWS = 0; ROWS <= DATA_ARRAY_ROW_MAX; ROWS++) {
      for (COLS = 0; COLS <= DATA_ARRAY_COLS_MAX; COLS++) {
        SetCursorPosColor (
          EFI_BLACK| EFI_BACKGROUND_LIGHTGRAY,
          (RIGHT_DATA_FRAME_HORI_MIN+COLS+5),
          (BASE_TITLE_OFFSET+ROWS+1)
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
      RIGHT_DATA_FRAME_HORI_MIN,
      (BASE_TITLE_OFFSET + 1)
      );
    Print (
      L" VendorID  :     %04X\n",
      HPciDev.Pci.Hdr.VendorId
      );
    
    SET_CUR_POS (
      RIGHT_DATA_FRAME_HORI_MIN,
      (BASE_TITLE_OFFSET + 2)
      );
    Print (
      L" DeviceID  :     %04X\n",
      HPciDev.Pci.Hdr.DeviceId
      );
  
    SET_CUR_POS (
      RIGHT_DATA_FRAME_HORI_MIN,
      (BASE_TITLE_OFFSET + 3)
      );
    Print (
      L" RevisionID:     %02X\n",
      HPciDev.Pci.Hdr.RevisionID
      );
    
    SET_CUR_POS (
      RIGHT_DATA_FRAME_HORI_MIN,
      (BASE_TITLE_OFFSET + 4)
      );
    Print (
      L" HeaderType:     %02X\n",
      HPciDev.Pci.Hdr.HeaderType
      );
  
    SET_CUR_POS (
      RIGHT_DATA_FRAME_HORI_MIN,
      (BASE_TITLE_OFFSET + 5)
      );
    Print (
      L" ClassCode :     %02X%02X%02X\n",
      HPciDev.Pci.Hdr.ClassCode[2],
      HPciDev.Pci.Hdr.ClassCode[1],
      HPciDev.Pci.Hdr.ClassCode[0]
      );
    
    SET_CUR_POS (
      RIGHT_DATA_FRAME_HORI_MIN,
      (BASE_TITLE_OFFSET + 6)
      );
    Print (
      L" SubSystemID :   %04X\n",
      HPciDev.Pci.Device.SubsystemID
      );
    
    SET_CUR_POS (
      RIGHT_DATA_FRAME_HORI_MIN,
      (BASE_TITLE_OFFSET + 7)
      );
    Print (
      L" SubSystemVID:   %04X\n",
      HPciDev.Pci.Device.SubsystemVendorID
      );
    
    SET_CUR_POS (
      RIGHT_DATA_FRAME_HORI_MIN,
      (BASE_TITLE_OFFSET + 8)
      );
    Print (
      L" Interrupt Pin:  %02X\n",
      HPciDev.Pci.Device.InterruptPin
      );
    
    SET_CUR_POS (
      RIGHT_DATA_FRAME_HORI_MIN,
      (BASE_TITLE_OFFSET + 9)
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
ShowDataFrame (
  IN PCIDEV    HPciDev,
  IN UINTN     HType,
  IN BOOLEAN   AscOrData
  )
{
  UINTN   DataVert;
  UINTN   DataHori;
	INT8    TypeVal;

  SetCursorPosColor (
    EFI_BLUE | EFI_BACKGROUND_LIGHTGRAY,
    0,
    BASE_TITLE_OFFSET
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
      0,
      (DATA_FRAME_OFFSET + DataVert)
      );
    
    Print(
      L"%X0 |",
      DataVert
      );
    
    SetCursorPosColor (
      EFI_BRIGHT,
      DATA_TABLE_OFFSET_HORI,
      (DATA_TABLE_OFFSET_VERT + DataVert)
      );
    
    Print (
      L"                                                "   //Clean Line
      ); 
    
    SET_CUR_POS (
      DATA_TABLE_OFFSET_HORI,
      (DATA_TABLE_OFFSET_VERT + DataVert)
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
      (DATA_FRAME_OFFSET + DataVert)
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

  Data table base title and tail Frame.
  @param[in]  Bus   PCI devices Bus.
  @param[in]  Dev   PCI devices Device.
  @param[in]  Fun   PCI devices Function.
  @param[in]  Tail1 Frame Tail.
  @param[in]  Tail2 Frame Tail.
  @param[in]  Tail3 Frame Tail.
  
*/
VOID
DataBaseFrame (
  IN UINT16  Bus,
  IN UINT8   Dev,
  IN UINT8   Fun,
  IN CHAR16  *Tail1,
  IN CHAR16  *Tail2,
  IN CHAR16  *Tail3
  )
{
  CLEAN_SCREEN (
    VOID
    );
  
  SET_COLOR (
    EFI_BLUE | EFI_BACKGROUND_LIGHTGRAY
    );
  
  SET_CUR_POS (
    0,
    0
    );
  
  Print (
    L"===============================================================================\n"  //79 =
    );  
  Print (
    L"=                                                                             =\n"
    );
  Print (
    L"===============================================================================\n"
    );
  
  SetCursorPosColor (
    EFI_BLACK| EFI_BACKGROUND_LIGHTGRAY,
    30,
    1
    );
  
  Print (
    L"BUS=%03d, DEV=%03d, FUN=%02d\n",
    Bus,
    Dev,
    Fun
    );
  
	SetCursorPosColor (
    EFI_BLUE | EFI_BACKGROUND_LIGHTGRAY,
    0,
    BASE_TAIL_OFFSET
    );  
  
  Print (
    L"===============================================================================\n"
    );
  Print (
    L"                                                                               \n"
    );
  Print (
    L"                                                                               \n"
    );
  Print (
    L"                                                                               \n"
    );
  
  SET_CUR_POS (
    0,
    BASE_TAIL_OFFSET+1
    );
  Print (
    L"%s\n%s\n%s\n",
    Tail1,
    Tail2,
    Tail3
    );
  Print (
    L"==============================================================================="
    );
  
}
