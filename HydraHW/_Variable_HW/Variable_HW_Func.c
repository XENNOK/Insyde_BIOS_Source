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



/**

  Data Frame.

  @param[in] *DataBuffer                         Data Buffer.

**/
EFI_STATUS
ShowDataBuf (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN UINTN                                  DataBufferSize,
  IN UINT8                                  *DataBuffer
  )
{
  UINT8           HoriPos;
  UINT8           VertPos;
  EFI_KEY_DATA    HKey;
  INT16           LargeBuff;
  BOOLEAN         BoolExit;

  LargeBuff = 0;
  BoolExit = FALSE;  

  CleanFrame (
    HUGE_TITLE_OFFSET,
    HUGE_TAIL_OFFSET,
    0,
    HUGE_FRAME_HORI_MAX
    );
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
    L"DataBufferSize = %d       ",
    DataBufferSize
    );
  SetCursorPosColor (
    EFI_WHITE | EFI_BACKGROUND_BLACK,
    HUGE_TABLE_HORI_MAX,
    (HUGE_TITLE_OFFSET + 6)
    );
  Print (
    L"Press Up/Down to show Data"
    );
  SetCursorPosColor (
    EFI_WHITE | EFI_BACKGROUND_BLACK,
    HUGE_TABLE_HORI_MAX,
    (HUGE_TITLE_OFFSET + 7)
    );
  Print (
    L"Press Ctrl+F1 to Exit   "
    );
  
  while (TRUE) {
   
    if (BoolExit) {
      break;
    }
    CleanFrame (
      HUGE_TITLE_OFFSET,
      HUGE_TAIL_OFFSET,
      0,
      HUGE_TABLE_HORI_MAX
      );
   //
   // print data frame title
   //
   SetCursorPosColor (
     EFI_BLUE | EFI_BACKGROUND_LIGHTGRAY,
     0,
     HUGE_TITLE_OFFSET
     );
   Print (L"   | 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |\n");
   //
   // print 8*16 or 16*16 data frame
   //
   for (VertPos = 0; VertPos < FRAME_MAX_VERT; VertPos++) {
     SetCursorPosColor (
       EFI_BLUE | EFI_BACKGROUND_LIGHTGRAY,
       0,
       (DATA_FRAME_OFFSET + VertPos)
       );
     Print (
       L"%02X0|",
       VertPos + (LargeBuff / 16)
       );
     for (HoriPos = 0; HoriPos < FRAME_MAX_HOZI; HoriPos++) {
      if ((((VertPos * 15) + VertPos) + HoriPos + LargeBuff) >= DataBufferSize) {
         SET_COLOR (EFI_DARKGRAY | EFI_BACKGROUND_BLACK);
         Print (L" XX");
      } else {
        if (DataBuffer[((VertPos * 15) + VertPos) + HoriPos + LargeBuff] == 0x00) {
          SET_COLOR (EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
          Print (
            L" %02X",
            DataBuffer[((VertPos * 15) + VertPos) + HoriPos + LargeBuff]
            );
        } else {
          SET_COLOR (EFI_YELLOW | EFI_BACKGROUND_BLACK);
          Print (
            L" %02X",
            DataBuffer[((VertPos * 15) + VertPos) + HoriPos + LargeBuff]
            );
        }
      }
     }
     
       SET_COLOR (EFI_BLUE | EFI_BACKGROUND_LIGHTGRAY);
       Print (L" |");
       
   }
   SET_COLOR (EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);


   HKey = keyRead (
            InputEx
            );
    if ((HKey.Key.ScanCode != 0 ) &&
        !((HKey.KeyState.KeyShiftState & EFI_LEFT_CONTROL_PRESSED) ||
          (HKey.KeyState.KeyShiftState & EFI_RIGHT_CONTROL_PRESSED))) {
         
 		  switch (HKey.Key.ScanCode) {
        
 		  case SCAN_UP:
        LargeBuff -= 256;
        if (LargeBuff < 0) {
          LargeBuff = 0;
        }
        break;
 		  case SCAN_DOWN:
        LargeBuff += 256;
        if (LargeBuff > DataBufferSize) {
          LargeBuff -= 256;
        }
        break;
 		  }
    } else if((HKey.KeyState.KeyShiftState & EFI_SHIFT_STATE_VALID) &&
      ((HKey.KeyState.KeyShiftState & EFI_LEFT_CONTROL_PRESSED) ||
       (HKey.KeyState.KeyShiftState & EFI_RIGHT_CONTROL_PRESSED)) &&
      (HKey.Key.ScanCode == SCAN_F1)) {
      BoolExit = TRUE;
      break;
    }
    
  }

}


EFI_STATUS
GetNextVari (
  IN OUT UINTN      *NameSize,
  IN OUT CHAR16     *VariableName,
  IN OUT EFI_GUID   *VendorGuid
  )
{
  EFI_STATUS                Status;
  EFI_KEY_DATA              HKey;
  CHAR16                    *OldName;
  UINTN                     OldNameBufferSize;
  
  Status = gRT->GetNextVariableName (
                  &NameSize,
                  VariableName,
                  VendorGuid
                  );
 if (Status == EFI_BUFFER_TOO_SMALL) {
  
   OldName           = VariableName;
   OldNameBufferSize = NameSize;

   NameSize = NameSize > (INIT_NAME_BUFFER_SIZE * 2) ? NameSize : (INIT_NAME_BUFFER_SIZE * 2);
   VariableName = AllocateZeroPool (NameSize);
   if (VariableName == NULL) {
       Status = EFI_OUT_OF_RESOURCES;
       FreePool (OldName);
       return Status;
   }

   CopyMem (
     VariableName,
     OldName,
     OldNameBufferSize
     );
   FreePool (OldName);

   Status = gRT->GetNextVariableName (
                   &NameSize,
                   VariableName, 
                   VendorGuid
                   );
 }
 if (Status == EFI_NOT_FOUND) {
   return Status;
 }

  return Status;
}


EFI_STATUS
GetVariData (
  IN CHAR16                                 *VariableName,
  IN EFI_GUID                               *VendorGuid,
  IN OUT UINTN                              *DataBufferSize,
  IN OUT UINT32                             *Attributes,
  IN OUT UINT8                              *DataBuffer,
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx
  )
{
  EFI_STATUS Status;
  EFI_KEY_DATA HKey;

  Status = gRT->GetVariable (
                  VariableName,
                  VendorGuid,
                  Attributes,
                  DataBufferSize,
                  DataBuffer
                  );
   
  if (Status == EFI_BUFFER_TOO_SMALL) {
    Print(L"EFI_BUFFER_TOO_SMALL\n");
    return Status;
  }

  return Status;
}

EFI_STATUS
ListAllVariables (
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
  VariableCount = 0;
  
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

//
// Get Next Variable 
//
  VariableName[NameSize] = '\0';
  Status = GetNextVari (
             &NameSize,
             VariableName,
             &VendorGuid
             );
  if (EFI_ERROR (Status)) {
    CLEAN_SCREEN();
    SetCursorPosColor (EFI_WHITE | EFI_BACKGROUND_BLACK, 0, 0);
    Print (L"GetNextVari ERROR %r\n",Status);
    Print (L"%-25s , %g\n",VariableName,VendorGuid);
    return Status;
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
    SetCursorPosColor (EFI_WHITE | EFI_BACKGROUND_BLACK, 0, 0);
    Print (L"GetVariData ERROR %r\n",Status);
    Print (L"%-25s , %g\n",VariableName,VendorGuid);
    return Status;
  }

 while (TRUE) { 
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
   } else if(HKey.Key.ScanCode == SCAN_DOWN){
     NameSize = INIT_NAME_BUFFER_SIZE;
     DataSize = INIT_DATA_BUFFER_SIZE;
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

       return Status;
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
       return Status;
     }
     
     VariableCount ++;
   } else if(HKey.Key.ScanCode == SCAN_F1){
     ShowDataBuf (
       InputEx,
       DataSize,
       DataBuffer
       );
     
   } else if(HKey.Key.ScanCode == SCAN_ESC){
     break;
   }
 }
  return Status;
}



