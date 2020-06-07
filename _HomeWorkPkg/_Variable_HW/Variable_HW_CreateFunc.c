/** @file

  Variable Homework
  
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


EFI_STATUS
VariDataWrite (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN OUT UINTN                              *VarSize,
  IN OUT UINT8                              *DataBuffer,
  IN INT8                                   Hori,
  IN INT8                                   Vert
  )
{ 
  EFI_KEY_DATA     HKey;
	INT8             ModifyPos;
  UINT8            ByteCount;
  CHAR16           ByteInput[2] = {'\0'};

//  UINT8            *DataBuffer;
  UINT8            BufferPosition;
  ModifyPos = 0;
  ByteCount = 1;
  BufferPosition = 0;
//  DataBuffer = AllocateZeroPool (*VarSize);
//  if (DataBuffer == NULL) {
//    return EFI_OUT_OF_RESOURCES;
//  }
  
  
	while(TRUE){
	  HKey = keyRead(InputEx);
    if (HKey.Key.UnicodeChar == CHAR_CARRIAGE_RETURN){
      *VarSize = BufferPosition;
      break;
     }
  	if((HKey.Key.ScanCode == 0) 
        && ((HKey.Key.UnicodeChar >= '0' && HKey.Key.UnicodeChar <= '9') 
        || (HKey.Key.UnicodeChar >= 'a' && HKey.Key.UnicodeChar <= 'f')
        || (HKey.Key.UnicodeChar >= 'A' && HKey.Key.UnicodeChar <= 'F'))
        ){
          ByteInput[ByteCount] = HKey.Key.UnicodeChar;
          
					SetCursorPosColor(EFI_WHITE | EFI_BACKGROUND_BLUE, Hori + ModifyPos, Vert);
          
          Print (L"%c",ByteInput[ByteCount]);
          
          if (ByteCount == 0) {
            DataBuffer[BufferPosition] = (UINT8) AscToIntN(ByteInput, 1);
            BufferPosition++;
            ByteCount = 1;
          } else {
            ByteCount --;
          }
          ModifyPos++;
          
  	} else if (HKey.Key.UnicodeChar == CHAR_BACKSPACE) {
      
  	}
	}
 
  return EFI_SUCCESS;
}

VOID
BoolAttrShow (
  IN BOOLEAN   BoolAttr,
  IN UINT8     itemposition,
  IN UINT32    *AttrVal,
  IN UINT8     Hori,
  IN UINT8     Vert
  )
{

  if (BoolAttr) {
    SetCursorPosColor (
      EFI_RED | EFI_BACKGROUND_LIGHTGRAY,
      Hori,
      itemposition + Vert
      );
    Print(L" V ");
    switch (itemposition) {
    case 0:
      *AttrVal += 0x01;
      break;
    case 1:
      *AttrVal += 0x02;
      break;
    case 2:
      *AttrVal += 0x04;
      break;
    }
  }else{
    SetCursorPosColor (
      EFI_LIGHTGRAY,
      Hori,
      itemposition + Vert
      );
    Print(L"   ");
    if (*AttrVal != 0) {
      switch (itemposition) {
      case 0:
        *AttrVal -= 0x01;
        break;
      case 1:
        *AttrVal -= 0x02;
        break;
      case 2:
        *AttrVal -= 0x04;
        break;
      }
    }
  }
}

UINT32
SelectAttrVal (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN UINT8                                  Hori,
  IN UINT8                                  Vert
  )
{
  EFI_KEY_DATA HKey;
  UINT8        ItemQuantity;
  UINT8        ItemCounts;
  UINT8        itemposition;
  
  CHAR16       *AttrsItem[3] = {
                 L"NV",
                 L"BS",
                 L"RT"
                 };
  UINT32       AttrVal;
  
  BOOLEAN      BOOL_NV;
  BOOLEAN      BOOL_BS;
  BOOLEAN      BOOL_RT;

  BOOL_NV      = FALSE;
  BOOL_BS      = FALSE;
  BOOL_RT      = FALSE;
  AttrVal      = 0;
  itemposition = 0;
  ItemQuantity = 3;
 
  SetCurPos(Hori, Vert);
  while (TRUE) {
    for (ItemCounts = 0; ItemCounts < ItemQuantity; ItemCounts++) {
      SetCursorPosColor (
        EFI_LIGHTGRAY, 
        0,
        (ItemCounts + Vert)
        );
      Print (
        L"   | %s |\n",
        AttrsItem[ItemCounts]
        );
    }
    SetCursorPosColor (
      EFI_WHITE,
      0,
      (itemposition + Vert)
      );
    Print (
      L"-> | %s |",
      AttrsItem[itemposition]
      );

    HKey = keyRead(InputEx);
    if(HKey.Key.ScanCode != 0){
		  switch(HKey.Key.ScanCode){
			  case SCAN_UP:
          itemposition--;
          if (itemposition == 0xFF) {
            itemposition=ItemQuantity-1;
          }
			    break;
			  case SCAN_DOWN:
          itemposition++;
          if (itemposition >= ItemQuantity) {
            itemposition=0;
          }				  
			    break;
         case SCAN_RIGHT:
          switch(itemposition){
          case 0:
            BOOL_NV = !BOOL_NV;
            BoolAttrShow (
              BOOL_NV,
              itemposition,
              &AttrVal,
              10,
              Vert
              );
            break;
          case 1:
            BOOL_BS = !BOOL_BS;
            BoolAttrShow (
              BOOL_BS,
              itemposition,
              &AttrVal,
              10,
              Vert
              );
            break;
          case 2:
            BOOL_RT = !BOOL_RT;
            BoolAttrShow (
              BOOL_RT,
              itemposition,
              &AttrVal,
              10,
              Vert
              );
            break;
          }
          break;
		  }
    } else if (HKey.Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
		  SetCurPos(0, Vert + 3);
			break;
		}
  }
return AttrVal;
}



EFI_STATUS
CreateVariables (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx
  )
{

  EFI_STATUS                Status;
  EFI_KEY_DATA              HKey;
  EFI_GUID                  TargetGuid;

  CHAR16                    VariName[WRITE_NAME_SIZE]={'\0'};
  BOOLEAN                   HaveGuid;
  BOOLEAN                   HaveName;
  UINT32                    AttrVal;

  UINTN                     VarSize;
  UINT8                     *DataBuf = 0;
  UINTN                     BufIndex;

  VarSize = INIT_DATA_BUFFER_SIZE;
  DataBuf = AllocateZeroPool (VarSize);
  if (DataBuf == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  AttrVal = 0;
  HaveGuid = FALSE;
  HaveName = FALSE;

  SetCursorPosColor (
    EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK,
    0, 
    3
    );
  Print (L"Create your Guid? \n");
  Print (L"________-____-____-____-____________");
  Status = GuidWrite (
             InputEx,
             4,
             0,
             16,
             0,
             &TargetGuid
             );
  if (Status == EFI_INVALID_PARAMETER) {
    SetCursorPosColor (EFI_WHITE | EFI_BACKGROUND_BLACK, 0, 3);
    Print (L"Parameter ERROR %r\n",Status);
    return Status;
  } else if (Status == EFI_NOT_READY) {
    HaveGuid = FALSE;
  } else {
    HaveGuid = TRUE;
  }
  
  SetCursorPosColor (
    EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK,
    0, 
    5
    );
  Print (L"Create your Name? \n");
  Print (L"______________________________");
  Status = VarNameWrite (
             InputEx,
             6,
             0,
             VariName
             );
  if (Status == EFI_INVALID_PARAMETER) {
    SetCursorPosColor (EFI_WHITE | EFI_BACKGROUND_BLACK, 0, 3);
    Print (L"NameParameter ERROR %r\n",Status);
     return Status;
  } else if (Status == EFI_NOT_READY) {
    HaveName = FALSE;
  } else {
    HaveName = TRUE;
  }

  if (!(HaveName) || !(HaveGuid)){
    return EFI_NOT_READY;
  }
  SetCursorPosColor (
    EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK,
    0, 
    9
    );
  Print (L"Attributes:\n");
  Print (L"Press right to Select/Unselect, Enter to Confirm\n");
  AttrVal = SelectAttrVal (
              InputEx,
              0,
              11
              );
  SetCursorPosColor (
    EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK,
    0, 
    14
    );
  Print (L"Enter Your Data:\n");
  VariDataWrite (
    InputEx,
    &VarSize,
    DataBuf,
    0,
    15
    );
  
Print(L"Name : %s\n",VariName);
Print(L"GUID : %g\n",TargetGuid);
Print(L"Attribute Value : 0x%02X\n",AttrVal);
Print(L"VaribleSize : %d\n",VarSize);
keyRead(InputEx);
ShowDataBuf (
  InputEx,
  VarSize,
  DataBuf
  );

keyRead(InputEx);

Status = gRT->SetVariable(
                VariName,
                &TargetGuid,
                AttrVal,
                VarSize,
                DataBuf
                );
}
