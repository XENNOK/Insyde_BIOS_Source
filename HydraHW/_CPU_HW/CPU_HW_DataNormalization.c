/** @file

  A dedicated function to Normalization Data

******************************************************************************
* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/
#include "CPU_HW.h"



/**

  This code Make up User Type Value when it's incomplete.
  
  @param[in]  *ModifyVal          User type value
  @param[in]  DataNum             Un-typed data number
  @param[in]  ModifyBytes         How many Byte you wants to type.

  
  @return UINT32                  Make Up complete data
*/
UINT32
MakeUpData (
  IN CHAR16             *ModifyVal,
  IN INT8               DataNum,
  IN UINTN              ModifyBytes,
  IN UINT32             OrgData
  )
{
  INT8               MakeUpDataNum;
  
  CHAR16             Data[8];
  CHAR16             *DataStr = {'0'};
  UINT32             MakeUpedData;

  UINT32             MakeUpVal;

  MakeUpedData = 0;
  MakeUpVal = 0;
  
  MakeUpDataNum = DataNum+1;

  MakeUpedData = AscToInt (
                   ModifyVal,
                   ModifyBytes
                   );
  
  if (DataNum == QUANTITY_PER_TYPE(ModifyBytes)) {
    
    return MakeUpedData;
    
  } else {

    MakeUpVal = OrgData % (Pow (MakeUpDataNum));
    MakeUpedData = MakeUpedData + MakeUpVal;

    return MakeUpedData;
  }
}

/**
  User write data.
  
  @param[in] InputEx        EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL.
  @param[in] TopStart       Position from top.
  @param[in] LeftStart      Position from left.
  @param[in] ModifyBytes    How many Byte you wants to type.
  @param[in] OrgData        Orignal data.
 
  @return UINTN             User write data
*/
UINTN
DataWrite (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN UINT8                                  TopStart,
  IN UINT8                                  LeftStart,
  IN UINT8                                  ModifyBytes,
  IN UINT32                                 OrgData
  )
{
  EFI_KEY_DATA       HKey;
  UINT8              ModifyPos;
  INT8               DataNum;
  CHAR16             ModifyVal[8] = {'0'};
  UINT32             MakeUpedData;
  
  ModifyPos = 0;
  DataNum = QUANTITY_PER_TYPE(ModifyBytes);
  MakeUpedData = 0;

  EN_CURSOR (
    TRUE
    );
  while (TRUE) {

    SET_CUR_POS (
      LeftStart+ModifyPos, 
      TopStart
      );
    
    HKey = keyRead (
             InputEx
             ); 
    if ((HKey.Key.ScanCode == 0) &&
      ((HKey.Key.UnicodeChar >= '0' && HKey.Key.UnicodeChar <= '9') ||
      (HKey.Key.UnicodeChar >= 'a' && HKey.Key.UnicodeChar <= 'f') ||
      (HKey.Key.UnicodeChar >= 'A' && HKey.Key.UnicodeChar <= 'F'))
      )
    {   
        if (DataNum < 0) {
          SetCursorPosColor (
            EFI_DARKGRAY | EFI_BACKGROUND_BLACK,
            LeftStart, 
            TopStart
            );
          Print (L"________");
          
          ModifyPos = 0;
          DataNum = QUANTITY_PER_TYPE(ModifyBytes);
          
          memset (
            (char *) ModifyVal,
            0,
            sizeof(ModifyVal)
            );
  			}
        
        
				SetCursorPosColor (
          EFI_WHITE | EFI_BACKGROUND_BLUE,
          (LeftStart + ModifyPos),
          TopStart
          );
        
  			ModifyVal[DataNum] = HKey.Key.UnicodeChar;
        
  			Print (
          L"%c",
          ModifyVal[DataNum]
          );
        
  			DataNum--;
				ModifyPos++;
        
  			
  	}else if (HKey.Key.UnicodeChar == CHAR_BACKSPACE) {
      if (DataNum == QUANTITY_PER_TYPE(ModifyBytes)) {
        continue;
      }
      
      DataNum++;
      ModifyPos--;
      ModifyVal[DataNum] = '0';
      
      SetCursorPosColor (
        EFI_DARKGRAY | EFI_BACKGROUND_BLACK,
        LeftStart+ModifyPos, 
        TopStart
        );
      Print (L"_");
  	
    }else if (HKey.Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
  	  MakeUpedData = MakeUpData (
                       ModifyVal,
                       DataNum,
                       ModifyBytes,
                       OrgData
                       );
        
      ModifyPos = 0;
      DataNum = QUANTITY_PER_TYPE(ModifyBytes);
      
      memset (
        (char *) ModifyVal,
        0,
        sizeof(ModifyVal)
        );
      break;
  	} else if ((HKey.KeyState.KeyShiftState & EFI_SHIFT_STATE_VALID) &&
  	  ((HKey.KeyState.KeyShiftState & EFI_LEFT_CONTROL_PRESSED) ||
  	   (HKey.KeyState.KeyShiftState & EFI_RIGHT_CONTROL_PRESSED)) &&
  	  (HKey.Key.ScanCode == SCAN_F1)) {
  	  MakeUpedData = NO_INPUT;
      break;
		}
  }
  
  return MakeUpedData;
}
