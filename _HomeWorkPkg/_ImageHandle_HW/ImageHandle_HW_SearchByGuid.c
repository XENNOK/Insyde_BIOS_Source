/** @file

  Image Handle Search Handle By GUID
  
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

  This code convert user type char to UINT32.
  
  @param[in]  *InputEx             Key event Protocol.
  @param[in]  TypeByte             Type bytes.
  @param[in]  Hori                 Horizontal Position.
  @param[in]  Vert                 Vertical Position
  
*/
UINT32
GuidWrite (
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
	CHAR16             ModifyVal[16]={'0'};

  
	DataNum = DATA_NUM (TypeByte);
	ModifyPos = 0;
  ASCIIVal = 0;
  
	while (TRUE) {
      
    if (DataNum < 0) {
      break;
    }
    
	  HKey = keyRead (
             InputEx
             );
   
    if ((HKey.Key.ScanCode == 0) &&
        ((HKey.Key.UnicodeChar >= '0' && HKey.Key.UnicodeChar <= '9') ||
         (HKey.Key.UnicodeChar >= 'a' && HKey.Key.UnicodeChar <= 'f') ||
         (HKey.Key.UnicodeChar >= 'A' && HKey.Key.UnicodeChar <= 'F'))) {
    
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
      
    	}
  }
  ASCIIVal = AscToInt (
      ModifyVal,
      TypeByte
      );


  SET_COLOR (EFI_WHITE | EFI_BACKGROUND_BLACK); 
  
  return ASCIIVal;
}


EFI_STATUS
SearchHandleByGuid (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN CHAR16                                 *FrameTitle,
  IN CHAR16                                 **FrameTail
  )
{

  EFI_STATUS          Status;
  EFI_KEY_DATA        HKey;
  
  UINTN               HandleIndex;

  UINTN              HandleCount = 0;
  EFI_HANDLE         *HandleBuffer = NULL;
  
  EFI_GUID            HGuid;
  
  
  CLEAN_SCREEN();
  BaseFrame (
    FrameTitle, 
    FrameTail[0],
    FrameTail[1],
    FrameTail[2]
    );
  SetCursorPosColor (
    EFI_BLUE | EFI_BACKGROUND_LIGHTGRAY,
    0,
    BASE_TITLE_OFFSET
    );
  Print(L"SearchHandleByGuid :\n");
  
  SetCursorPosColor (
    EFI_LIGHTGRAY| EFI_BACKGROUND_BLACK,
    0,
    (BASE_TITLE_OFFSET + 1)
    );
  Print(L"XXXXXXXX - XXXX - XXXX - XXXX - XXXXXXXXXXXX\n");
  
  while(1){
    HGuid.Data1 = (UINT32)GuidWrite(InputEx,4,0,(BASE_TITLE_OFFSET +1));
    HGuid.Data2 = (UINT16)GuidWrite(InputEx,2,11,(BASE_TITLE_OFFSET +1));
    HGuid.Data3 = (UINT16)GuidWrite(InputEx,2,18,(BASE_TITLE_OFFSET +1));
    HGuid.Data4[0] = (UINT8)GuidWrite(InputEx,1,25,(BASE_TITLE_OFFSET +1));
    HGuid.Data4[1] = (UINT8)GuidWrite(InputEx,1,27,(BASE_TITLE_OFFSET +1));
    HGuid.Data4[2] = (UINT8)GuidWrite(InputEx,1,32,(BASE_TITLE_OFFSET +1));
    HGuid.Data4[3] = (UINT8)GuidWrite(InputEx,1,34,(BASE_TITLE_OFFSET +1));
    HGuid.Data4[4] = (UINT8)GuidWrite(InputEx,1,36,(BASE_TITLE_OFFSET +1));
    HGuid.Data4[5] = (UINT8)GuidWrite(InputEx,1,38,(BASE_TITLE_OFFSET +1));
    HGuid.Data4[6] = (UINT8)GuidWrite(InputEx,1,40,(BASE_TITLE_OFFSET +1));
    HGuid.Data4[7] = (UINT8)GuidWrite(InputEx,1,42,(BASE_TITLE_OFFSET +1));
    
    SetCursorPosColor (
    EFI_LIGHTGRAY| EFI_BACKGROUND_BLACK,
    0,
    (BASE_TITLE_OFFSET + 2)
    );
    Print(L"Press Enter to Confirm, other key to re-enter GUID\n");
    HKey = keyRead (
               InputEx
               );
    
    SetCursorPosColor (
    EFI_LIGHTGRAY| EFI_BACKGROUND_BLACK,
    0,
    (BASE_TITLE_OFFSET + 2)
    );
    Print(L"                                                  \n");
    if(HKey.Key.UnicodeChar == CHAR_CARRIAGE_RETURN){
      break;
    }
  }

  //
  // Locate Handle Buffer
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,   
                  &HGuid,
                  NULL,
                  &HandleCount, 
                  &HandleBuffer
                  );
  
  if (EFI_ERROR (Status)) {
    CLEAN_SCREEN();
    Print (L"###LocateHandleBuffer ERROR###\n");
    return Status;
  }

  Status = SelectHandle (
             InputEx,
             FrameTitle,
             FrameTail,
             HandleCount,
             HandleBuffer,
             SearchByGuid,
             &HGuid
             );
  if (EFI_ERROR (Status)) {
      Print (L"###SelectHandle ERROR###\n");
      return Status;
  }
  
  //
  // Free Handle Buffer
  //
  Status = gBS->FreePool (HandleBuffer);
  if (EFI_ERROR (Status)) {
      Print (L"###LocateHandleBuffer FreePool ERROR###\n");
      return Status;
  }

return Status;
}
