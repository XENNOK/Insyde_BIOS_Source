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
#include "Variable_HW.h"

/**

  Transfer Data to UINT32
  @param[in]   *writedata       data needs to transfet to UINT32
  @param[in]   HType            Type Byte/Word/DWord

  @return      WriteVal         transfered data

*/
UINTN
AscToIntN (
  IN CHAR16        *writedata,
  IN UINTN         HType
  )
{

  UINTN    INTVal;
  UINTN    WriteVal;
  UINTN    TypeNum;
  UINTN    Num;
  
  TypeNum = DATA_NUM (
              HType
              );
  WriteVal = 0 ;
  
  for (Num=0; Num <= TypeNum; Num++) {
    
    if (writedata[Num] >= '0' && writedata[Num] <= '9') {
      
      INTVal = (UINTN) (writedata[Num] - '0');
      
    } else if (writedata[Num] >= 'a' && writedata[Num] <= 'f') {
    
      INTVal = (UINTN) ((writedata[Num] - 'a') + 10);
      
    } else if (writedata[Num] >= 'A' && writedata[Num] <= 'F') {
    
      INTVal = (UINTN) ((writedata[Num] - 'A') + 10);
      
    }
    WriteVal += (INTVal << ( 4 * Num));
  }
   
  return WriteVal;
}

/**

  This code Make up User Type Value when it's incomplete.
  
  @param[in]  *ModifyVal          User type value
  @param[in]  DataNum             Un-typed data number
  @param[in]  ModifyBytes         How many Byte you wants to type.

  
  @return UINT32                  Make Up complete data
*/
EFI_STATUS
GuidData (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN CHAR16             *ModifyVal,
  IN INT8               DataNum,
  IN UINTN              ModifyBytes,
  IN UINTN              OrgData,
  IN OUT EFI_GUID       *TargetGUID
  )
{
  INT8               MakeUpDataNum;
  
  UINTN              MakeUpedDataH;
  UINTN              MakeUpedDataL;

  UINTN              MakeUpVal;
  CHAR16             HighGuid[16]={'0'};
  CHAR16             LowGuid[16]={'0'};
  UINT8              DataIndex;
  
  MakeUpedDataH = 0;
  MakeUpedDataL = 0;
  MakeUpVal = 0;
  
  MakeUpDataNum = DataNum + 1;

  if (DataNum < 0) {
    
    for (DataIndex = 0; DataIndex < 32; DataIndex++)
    if (DataIndex < 16) {
      LowGuid[DataIndex] = ModifyVal[DataIndex];
    } else {
      HighGuid[DataIndex - 16] = ModifyVal[DataIndex];
    }


    MakeUpedDataH = AscToIntN (
                      HighGuid,
                      8
                      );
    MakeUpedDataL = AscToIntN (
                      LowGuid,
                      8
                      );
    TargetGUID->Data1 = (UINT32) (MakeUpedDataH >> 32);
    TargetGUID->Data2 = (UINT16) (MakeUpedDataH >> 16);
    TargetGUID->Data3 = (UINT16) MakeUpedDataH;
    TargetGUID->Data4[0] = (UINT8) (MakeUpedDataL >> 56);
    TargetGUID->Data4[1] = (UINT8) (MakeUpedDataL >> 48);
    TargetGUID->Data4[2] = (UINT8) (MakeUpedDataL >> 40);
    TargetGUID->Data4[3] = (UINT8) (MakeUpedDataL >> 32);
    TargetGUID->Data4[4] = (UINT8) (MakeUpedDataL >> 24);
    TargetGUID->Data4[5] = (UINT8) (MakeUpedDataL >> 16);
    TargetGUID->Data4[6] = (UINT8) (MakeUpedDataL >> 8);
    TargetGUID->Data4[7] = (UINT8) MakeUpedDataL;

    return EFI_SUCCESS;
  } else {

   return EFI_INVALID_PARAMETER;
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
EFI_STATUS
GuidWrite (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN UINT8                                  TopStart,
  IN UINT8                                  LeftStart,
  IN UINTN                                  ModifyBytes,
  IN UINTN                                  OrgData,
  IN OUT EFI_GUID                           *TargetGUID
  )
{
  EFI_STATUS         Status;
  EFI_KEY_DATA       HKey;
  UINT8              ModifyPos;
  INT8               DataNum;
  CHAR16             ModifyVal[32] = {'0'};
  
  ModifyPos = 0;
  DataNum = QUANTITY_PER_TYPE(ModifyBytes);

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
            EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK,
            LeftStart, 
            TopStart
            );
          Print (L"________-____-____-____-____________");
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
        if ((ModifyPos == 8)  ||
            (ModifyPos == 13) ||
            (ModifyPos == 18) ||
            (ModifyPos == 23) ) {
          ModifyPos++;
        }
  			
  	}else if (HKey.Key.UnicodeChar == CHAR_BACKSPACE) {
      if (DataNum == QUANTITY_PER_TYPE(ModifyBytes)) {
        continue;
      }
      
      DataNum++;
      ModifyPos--;
      if ((ModifyPos == 8)  ||
            (ModifyPos == 13) ||
            (ModifyPos == 18) ||
            (ModifyPos == 23) ) {
          ModifyPos--;
        }
      ModifyVal[DataNum] = '0';
      
      SetCursorPosColor (
        EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK,
        LeftStart+ModifyPos, 
        TopStart
        );
      Print (L"_");
  	
    }else if (HKey.Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
  	  Status = GuidData (
                 InputEx,
                 ModifyVal,
                 DataNum,
                 ModifyBytes,
                 OrgData,
                 TargetGUID
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
      Status = EFI_NOT_READY;
      break;
		}
  }
  
  return Status;
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
EFI_STATUS
VarNameWrite (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN UINT8                                  TopStart,
  IN UINT8                                  LeftStart,
  IN OUT CHAR16                             *VariName
  )
{
  EFI_KEY_DATA       HKey;
  UINT8              ModifyPos;

  ModifyPos = 0;

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
        (HKey.Key.UnicodeChar >= 0x21 && HKey.Key.UnicodeChar <= 0x7E))
    {   
        if (ModifyPos >= WRITE_NAME_SIZE) {
          SetCursorPosColor (
            EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK,
            LeftStart, 
            TopStart
            );
          Print (L"______________________________");
          
          ModifyPos = 0;
          
          memset (
            (char *) VariName,
            0,
            sizeof(VariName)
            );
  			}
        
        
				SetCursorPosColor (
          EFI_WHITE | EFI_BACKGROUND_BLUE,
          (LeftStart + ModifyPos),
          TopStart
          );
        
  			VariName[ModifyPos] = HKey.Key.UnicodeChar;
        
  			Print (
          L"%c",
          VariName[ModifyPos]
          );
        
				ModifyPos++;
        
  			
  	}else if (HKey.Key.UnicodeChar == CHAR_BACKSPACE) {
      if (ModifyPos == 0) {
        continue;
      }
      

      ModifyPos--;
      VariName[ModifyPos] = '\0';
      
      SetCursorPosColor (
        EFI_DARKGRAY | EFI_BACKGROUND_BLACK,
        LeftStart+ModifyPos, 
        TopStart
        );
      Print (L"_");
  	
    }else if (HKey.Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
      if (ModifyPos == 0) {
        return EFI_INVALID_PARAMETER;
      }
      break;
  	} else if ((HKey.KeyState.KeyShiftState & EFI_SHIFT_STATE_VALID) &&
  	  ((HKey.KeyState.KeyShiftState & EFI_LEFT_CONTROL_PRESSED) ||
  	   (HKey.KeyState.KeyShiftState & EFI_RIGHT_CONTROL_PRESSED)) &&
  	  (HKey.Key.ScanCode == SCAN_F1)) {
  	  memset (
        (char *) VariName,
        0,
        sizeof(VariName)
        );
      return EFI_NOT_READY;
		}
  }
  return EFI_SUCCESS;
}