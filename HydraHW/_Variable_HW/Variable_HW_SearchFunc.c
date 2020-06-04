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
SearchVariables (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx
  )
{
  EFI_STATUS                Status;
  EFI_KEY_DATA              HKey;
  CHAR16                    *VariableName;
  UINTN                     NameSize;
  EFI_GUID                  VendorGuid;

  UINT8                     *DataBuffer;
  UINTN                     DataSize;
  UINT32                    Attributes;

  UINTN                     VariableCount;
  EFI_GUID                  TargetGuid;

  BOOLEAN                   BoolExit;
  CHAR16                    VariName[WRITE_NAME_SIZE]={'\0'};
  BOOLEAN                   HaveGuid;
  BOOLEAN                   HaveName;

  CHAR16                    *OldName;
  EFI_GUID                  OldGuid;
  
  BoolExit = FALSE;
  HaveGuid = FALSE;
  HaveName = FALSE;
  
  VariableCount = 0;

  //
  // show data frame address start and end
  // in right data frame.
  //
  SetCursorPosColor (
    EFI_WHITE | EFI_BACKGROUND_BLACK,
    HUGE_TABLE_HORI_MAX,
    (HUGE_TITLE_OFFSET + 5)
    );
  Print (
    L"Press Ctrl+F1 to Discard  "
    );


  SetCursorPosColor (
         EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK,
         0, 
         3
         );
  Print (L"Which Guid You wants to find? \n");
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
  Print (L"What Name You wants to find? \n");
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

  if (!(HaveName) && !(HaveGuid)){
    return EFI_NOT_READY;
  }
  
  NameSize = INIT_NAME_BUFFER_SIZE;
  VariableName = AllocateZeroPool (NameSize);
  if (VariableName == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  DataSize = INIT_DATA_BUFFER_SIZE;
  DataBuffer = AllocateZeroPool (DataSize);
  if (DataBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  OldName = AllocateZeroPool (NameSize);
  if (VariableName == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  while (TRUE) { 
    NameSize = INIT_NAME_BUFFER_SIZE;
    DataSize = INIT_DATA_BUFFER_SIZE;

    StrCpy(OldName, VariableName);
    OldGuid = VendorGuid;
    //
    // Get Next Variable 
    //
    VariableName[NameSize] = '\0';
    Status = GetNextVari (
               &NameSize,
               VariableName,
               &VendorGuid
               );
    if (Status == EFI_NOT_FOUND) {
      CleanFrame (
        HUGE_TITLE_OFFSET,
        HUGE_TAIL_OFFSET,
        0,
        HUGE_FRAME_HORI_MAX
        );
      SetCursorPosColor (EFI_WHITE | EFI_BACKGROUND_RED, 0, HUGE_TITLE_OFFSET);
      Print (L"No More Variable\n");
      Print (L"Press any key to exit\n");
      SET_COLOR(EFI_WHITE | EFI_BACKGROUND_BLACK);
      HKey = keyRead (
           InputEx
           );
      break;
    } else if (EFI_ERROR (Status)) {
      CLEAN_SCREEN();
      SET_CUR_POS(0, 0);
      Print (L"GetNextVari ERROR %r\n",Status);
      Print (L"w %-25s , %g\n",VariableName,VendorGuid);
   
      break;
    }
    
    //
    // Get Variable Data;
    //
    Status = GetVariData (
               VariableName,
               &VendorGuid,
               &DataSize,
               &Attributes,
               DataBuffer,
               InputEx
               );
    if (EFI_ERROR (Status)) {
      CLEAN_SCREEN();
      SET_CUR_POS(0, 0);
      Print (L"GetVariData ERROR %r\n",Status);
      Print (L"w %-25s , %g\n",VariableName,VendorGuid);
      break;
    }
    
    if (((HaveGuid) && !(HaveName) && (CompareGuid(&VendorGuid, &TargetGuid))) ||
        (!(HaveGuid) && (HaveName) && (StrCmp(VariableName, VariName) == 0)) ||
        ((HaveGuid) && (HaveName) && 
         (CompareGuid(&VendorGuid, &TargetGuid)) && (StrCmp(VariableName, VariName) == 0))) {
      CleanFrame (
      HUGE_TITLE_OFFSET,
      HUGE_TAIL_OFFSET,
      0,
      HUGE_FRAME_HORI_MAX
      );
      
      SetCursorPosColor (
          EFI_WHITE | EFI_BACKGROUND_BLACK,
          0,
          HUGE_TITLE_OFFSET
          );
      Print (L"           Name            Attributes                 GUIDs                \n");
      Print (L"==========================|==========|=====================================\n");
      Print (
        L"%-25s , %2s %2s %2s , %g\n",
        VariableName,
        (Attributes & 0x01) == 0?L"  ":L"NV",
        (Attributes & 0x02) == 0?L"  ":L"BS",
        (Attributes & 0x04) == 0?L"  ":L"RT",
        VendorGuid
        );
   
      Print (L"\n");
      Print (L"VariableCount %d\n",VariableCount);
      Print (L"===========================================================================\n");
      Print (L"Press Down to continue, DEL to delete, ESC to quit, F1 to show Data.\n");

      HKey = keyRead (
               InputEx
               ); 
      if (HKey.Key.ScanCode == SCAN_DELETE) {
        Status = gRT->SetVariable (VariableName, &VendorGuid,0,0,0);
        if (EFI_ERROR(Status)) {
          Print (L"Delete fail");
        } else {
          Print (L"Delete success");
          break;
        }
      } else if(HKey.Key.ScanCode == SCAN_F1){
        ShowDataBuf (
          InputEx,
          DataSize,
          DataBuffer
          );
        StrCpy(VariableName, OldName);
        VendorGuid = OldGuid;
      } else if(HKey.Key.ScanCode == SCAN_DOWN){
        
        VariableCount ++;
      }else if(HKey.Key.ScanCode == SCAN_ESC){
        BoolExit = TRUE;
      }
    }

    if (BoolExit) {
      break;
    }
  }
  return Status;
}
