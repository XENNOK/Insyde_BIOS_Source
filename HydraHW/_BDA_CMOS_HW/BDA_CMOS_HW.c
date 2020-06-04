/** @file

  BDA and EBDA & CMOS
  
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

#include"BDA_CMOS_HW.h"

/**

  This code Show data ASCII Code in right frame.
  
  @param[in]  MaxVert              Max Data Vert
  @param[in]  FrameData            Data of BDA / EDBA / CMOS.

*/
VOID
RightDataFrame (
  IN UINT8 MaxVert,
  IN UINT8 *FrameData
  )
{
  UINT8 BG_Index;
  UINT8 Rows;
  UINT8 Cols;

  UINT8 PrintData;

  //
  // Print right data frame title
  //
  SetCursorPosColor (
  EFI_LIGHTGRAY| EFI_BACKGROUND_BLUE,
  RIGHT_DATA_FRAME_HORI_MIN,
  BASE_TITLE_OFFSET
  );
  Print (L"        ASCII Code       \n");

  for (Rows = 0; Rows < MaxVert; Rows++) {
    for (Cols = 0; Cols <= DATA_ARRAY_COLS_MAX; Cols++) {
      //
      // print data if it is ascii from 0x21 to 0x7E
      //
      PrintData = FrameData[((Rows * 15) + Rows) + Cols];
      SetCursorPosColor (
        EFI_BLACK| EFI_BACKGROUND_LIGHTGRAY,
        (RIGHT_DATA_FRAME_HORI_MIN + Cols + 5),
        (BASE_TITLE_OFFSET + Rows + 1)
        );
      
      if ((PrintData >= '!' ) &&
          (PrintData <= '~')) {
        Print (
          L"%c",
          PrintData
          );
      } else {
        Print (L".");
      }
      
    }
  }
    
}

/**

  This code Show data frame.
  
  @param[in]  IoType               BDA / EDBA / CMOS.
  @param[in]  FrameData            Data of BDA / EDBA / CMOS.

*/
VOID
ShowData (
  IN UINT8 IoType,
  IN UINT8 *FrameData
  )
{

  UINT8 HoriPos;
  UINT8 VertPos;
  UINT8 MaxVert;
  UINT8 PrintData;

  PrintData = 0;

  //
  // print data frame title
  //
  SetCursorPosColor (
    EFI_BLUE | EFI_BACKGROUND_LIGHTGRAY,
    0,
    BASE_TITLE_OFFSET
    );
  Print (L"   | 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |\n"); 
  
  //
  // switch max vert value
  // if BDA / EBDA -> Max vert = 16
  // if CMOS       -> Nax vert = 8
  //
  if (IoType == T_CMOS) {
    MaxVert = CMOS_MAX_VERT;
  } else {
    MaxVert = BDA_MAX_VERT;
  }

  //
  // print 8*16 or 16*16 data frame
  //
  for (VertPos = 0; VertPos < MaxVert; VertPos++) {
    SetCursorPosColor (
      EFI_BLUE | EFI_BACKGROUND_LIGHTGRAY,
      0,
      (DATA_FRAME_OFFSET + VertPos)
      );
    Print (
      L"%X0 |",
      VertPos
      );
    for (HoriPos = 0; HoriPos < FRAME_MAX_HOZI; HoriPos++) {
      PrintData = FrameData[((VertPos * 15) + VertPos) + HoriPos];

      //
      // Change front color,
      // if 00 -> DARKGRAY,
      // if ff -> LIGHTGRAY,
      // else     YELLOW
      //
      if (PrintData == 0x00) {
        SET_COLOR (EFI_DARKGRAY | EFI_BACKGROUND_BLACK);
      } else if (PrintData == 0xFF) {
        SET_COLOR (EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
      } else {
        SET_COLOR (EFI_YELLOW | EFI_BACKGROUND_BLACK);
      }
      Print (
        L" %02X",
        PrintData
        );
    }
    
      SET_COLOR (EFI_BLUE | EFI_BACKGROUND_LIGHTGRAY);
      Print (L" |                         ");
      
  }

  //
  // Show Right data frame 
  //
  RightDataFrame (
    MaxVert,
    FrameData
    );
  
  //
  // If Type is CMOS
  // Make up frame to 16*16 in "XX"
  //
  if (IoType == T_CMOS) {
    for (VertPos = CMOS_MAX_VERT; VertPos < FRAME_MAX_VERT; VertPos++) {
      SetCursorPosColor (
        EFI_BLUE | EFI_BACKGROUND_LIGHTGRAY,
        0,
        (DATA_FRAME_OFFSET + VertPos)
        );
      Print (
        L"%X0 |",
        VertPos
        );
      
      for (HoriPos = 0; HoriPos < FRAME_MAX_HOZI; HoriPos++) {
        SET_COLOR (EFI_DARKGRAY | EFI_BACKGROUND_BLACK);
        Print (L" XX");
      }
      SET_COLOR (EFI_BLUE | EFI_BACKGROUND_LIGHTGRAY);
      Print (L" |                         ");
    }
  }

 
}

/**

  This code dump data and saved in array
  
  @param[in]       *CpuIo               EFI_CPU_IO2_PROTOCOL.
  @param[in]       IoType               BDA / EDBA / CMOS.
  @param[in][out]  FrameData            Data of BDA / EDBA / CMOS.

  @return Status Code
*/
EFI_STATUS
DumpData (
  IN EFI_CPU_IO2_PROTOCOL                   *CpuIo,
  IN UINT8                                  IoType,
  IN OUT UINT8                              *FrameData
  )
{
  EFI_STATUS Status;
  UINTN      EBDAAdd;
  UINT16     DataCount;

  //
  // Switch BDA / EBDA / CMOS Type and dump data to array
  //
  switch (IoType) {
  case T_BDA:
    Status = CpuIo->Mem.Read (
                          CpuIo,
                          EfiCpuIoWidthUint8,
                          BDA_PORT,
                          256,
                          FrameData
                          );
    if (EFI_ERROR (Status)) {
      SET_CUR_POS (
        0,
        1
        );
       Print (
        L"CpuIo->Mem.Read BDA_Data Fail : %2d\n",
        Status
        );
       return Status;
    }
    break;
  case T_EBDA:
    Status = CpuIo->Mem.Read (
                          CpuIo,
                          EfiCpuIoWidthUint16,
                          EBDA_PORT,
                          1,
                          &EBDAAdd
                          );
    if (EFI_ERROR (Status)) {
      Print (
        L"CpuIo->Mem.Read EBDA_Add Fail : %2d\n",
        Status
        );
      return Status;
    }
    Status = CpuIo->Mem.Read (
                          CpuIo,
                          EfiCpuIoWidthUint8,
                          EBDAAdd << 4,
                          256,
                          FrameData
                          );
    if (EFI_ERROR (Status)) {
       Print (
         L"CpuIo->Mem.Read EBDA_Data Fail : %2d\n",
         Status
         );
       return Status;
    }
    break;
  case T_CMOS:
    for (DataCount = 0; DataCount <= CMOS_MAX_BYTE; DataCount++) {
      Status = CpuIo->Io.Write (
                           CpuIo,
                           EfiCpuIoWidthUint8,
                           CMOS_CMD,
                           1,
                           &DataCount
                           );
      if (EFI_ERROR (Status)) {
        Print (
          L"CpuIo->Io.Write Fail : %2d ,%d\n",
          Status,
          DataCount
          );
        break;
      }
      Status = CpuIo->Io.Read (
                           CpuIo,
                           EfiCpuIoWidthUint8,
                           CMOS_DATA,
                           1,
                           &FrameData[DataCount]
                           );
      if (EFI_ERROR (Status)) {
        Print (
          L"CpuIo->Io.Read Fail : %2d\n",
          Status
          );
        break;
      }
    }
    break;
  }
  return Status;
}

/**

  This code Handle key event and timer event,
  it will dump and show data frame. 
  
  @param[in]  *CpuIo               EFI_CPU_IO2_PROTOCOL.
  @param[in]  *InputEx             Key event Protocol.
  @param[in]  IoType               BDA / EDBA / CMOS.
  
*/
VOID
FrameHandler (
  IN EFI_CPU_IO2_PROTOCOL                   *CpuIo,
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN UINT8                                  IoType
  )
{
  EFI_STATUS         Status;
  UINTN              EventIndex = 0;
  EFI_EVENT          Events[2] = {0};   
  
  EFI_KEY_DATA       HKey;
  UINTN              DataVert;
  
  CHAR16             *Title[] = {
                       L"BDA",
                       L"EBDA",
                       L"CMOS"
                       };
  CHAR16             *Tail[] = {
                       L" Ctrl+Left : Exit"
                       };

  UINT8              FrameData[256] = {0};
  
  CLEAN_SCREEN (VOID);

  //
  // Show BDA / EBDA / CMOS title and data frame
  //
  BaseFrame (
    Title[IoType],
    Tail[0],
    "",
    ""
    );
  
  Status = DumpData (
             CpuIo,
             IoType,
             FrameData
             );
  if (EFI_ERROR (Status)) {
    CLEAN_SCREEN (VOID);
    Print (L"### DumpData ERROR ###\n");
  }
  
  ShowData (
    IoType,
    FrameData
    );
      
  //
  // Create key event and timer event.
  // Refresh data per half second,
  // if key event equals ctrl + left close event.
  //
  Events[0] = gST->ConIn->WaitForKey;
  
  Status = gBS->CreateEvent(
                  EVT_TIMER,
                  TPL_CALLBACK,
                  (EFI_EVENT_NOTIFY) NULL,
                  (VOID*) NULL,
                  &Events[1]
                  );
  
  Status = gBS->SetTimer(
                  Events[1],
                  TimerPeriodic ,
                  (5 * 1000 * 1000)
                  );  

  while (1) {
    Status = gBS->WaitForEvent (
                    2,
                    Events,
                    &EventIndex
                    );
    
    if (EFI_SUCCESS == Status) {
      if (EventIndex == KEY_EVENT) {
        Status = InputEx->ReadKeyStrokeEx (
                            InputEx,
                            &HKey
                            );
  
        if (EFI_ERROR (Status)) {
           CLEAN_SCREEN (VOID);
           Print (L"### ReadKeyStrokeEx ERROR ###\n");
        }
        
        if((HKey.KeyState.KeyShiftState & EFI_SHIFT_STATE_VALID) &&
            ((HKey.KeyState.KeyShiftState & EFI_LEFT_CONTROL_PRESSED) ||
             (HKey.KeyState.KeyShiftState & EFI_RIGHT_CONTROL_PRESSED)) &&
            (HKey.Key.ScanCode == SCAN_LEFT)) {

          Status = gBS->CloseEvent(Events);
          
          if (EFI_ERROR (Status)) {
             CLEAN_SCREEN (VOID);
             Print (L"### CloseEvent ERROR ###\n");
          }
          break;
        }
      } else if (EventIndex == TIMER_EVENT) {
      //
      // reset frame data buffer,
      // re-dump and show data.
      //
        memset (
          (char *) FrameData,
          0,
          sizeof(FrameData)
          );
        
        Status = DumpData (
                   CpuIo,
                   IoType,
                   FrameData
                   );
        if (EFI_ERROR (Status)) {
          CLEAN_SCREEN (VOID);
          Print (L"### DumpData ERROR ###\n");
          break;
        }
      
        ShowData (
          IoType,
          FrameData
          );
      }
    }
  }

  //
  // Set front and background color to default.
  //
  SET_COLOR (EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
  
}

/**

  This code Handle key event and do corresponding func.
  
  @param[in]  *CpuIo               EFI_CPU_IO2_PROTOCOL.
  @param[in]  *InputEx             Key event Protocol.
  @param[in]  **BaseFrameTail      Base Frame Tail.
  
*/
VOID
SelectItems (
  IN EFI_CPU_IO2_PROTOCOL                   *CpuIo,
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN CHAR16                                 **BaseFrameTail
  )
{
  EFI_KEY_DATA       HKey;
  UINT8              ItemPosition;

  ItemPosition = 0;
  
   while (1) {
     //
     // Print Select Items and "->"
     //
     SetCursorPosColor (
       EFI_LIGHTGRAY,
       0, 
       BASE_TITLE_OFFSET
       );
     
 	   Print (L"   | BDA\n");
  	 Print (L"   | EBDA\n");
  	 Print (L"   | CMOS\n");
     
 		 EN_CURSOR (FALSE);
     SetCursorPosColor (
       EFI_WHITE,
       0,
       (ItemPosition+BASE_TITLE_OFFSET)
       );
     Print (L"-> | ");

     //
     // Read Key Event
     //
     HKey = keyRead (InputEx);

     if ((HKey.Key.ScanCode != 0 ) && 
         !((HKey.KeyState.KeyShiftState & EFI_LEFT_CONTROL_PRESSED) ||
           (HKey.KeyState.KeyShiftState & EFI_RIGHT_CONTROL_PRESSED))) {
       //
       //  key event up & down to change select item and record item position
       //
       switch (HKey.Key.ScanCode) {
       case SCAN_UP:
         ItemPosition--;
         if (ItemPosition == 0xFF) {
           ItemPosition = TOTAL_NUM-1;
         }
         break;
       case SCAN_DOWN:
         ItemPosition++;
         if (ItemPosition >= TOTAL_NUM) {
           ItemPosition = 0;
         }				  
         break;
       }
     } else if ((HKey.KeyState.KeyShiftState & EFI_SHIFT_STATE_VALID) &&
       ((HKey.KeyState.KeyShiftState & EFI_LEFT_CONTROL_PRESSED) ||
        (HKey.KeyState.KeyShiftState & EFI_RIGHT_CONTROL_PRESSED)) &&
       (HKey.Key.ScanCode == SCAN_F1)) {
        //
        // if key equal ctrl + F1 break this while loop
        //
   	   SET_CUR_POS (
         0,
         BASE_FRAME_OFFSET
         );
        
       Print (L"\n");
       
  		 break;
  	 } else if (HKey.Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
       //
       // Handle Frame and show data
       //
       FrameHandler (
         CpuIo,
         InputEx,
         ItemPosition
         );

       //
       // Back to base utility frame
       //
       BaseFrame (
         L"BDA / CMOS Utility",
         BaseFrameTail[0],
         BaseFrameTail[1],
         BaseFrameTail[2]
         );
  	 }
  }
}

/**
  BDA CMOS Utility Entry Point.

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
  
  EFI_CPU_IO2_PROTOCOL                   *CpuIo;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx = NULL;
  CHAR16                                 *BaseFrameTail[] = { 
                                            L"Ctrl+F1 : Exit | Enter : Select Item,",
                                            L"Up Botton: Select Previous Item",
                                            L"Down Botton: Select Next Item"
                                            };
  CHAR16                                 **BaseFrameTailPtr;


  BaseFrameTailPtr = BaseFrameTail;
  
  //
  // Check Console Mode, save orignal mode and Set specification Mode
  //
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
    Print (L"SetMode ERROR\n");
  }

  //
  // Locate Simple Text Input Protocol
  //
  Status = gBS->LocateProtocol (
                  &gEfiSimpleTextInputExProtocolGuid,
                  NULL,
                  (VOID **) &InputEx
                  );

  if (EFI_ERROR (Status)) {
    Print(
      L"LocateProtocol gEfiSimpleTextInputExProtocolGuid Fail : %2d\n",
      Status
      );
    
    return Status;
  }

  //
  // Locate CpuIo2 Protocol
  //
  Status = gBS->LocateProtocol(
                  &gEfiCpuIo2ProtocolGuid,
                  NULL,
                  &CpuIo
                  );
  if (EFI_ERROR (Status)) {
    Print(
      L"LocateProtocol gEfiCpuIo2ProtocolGuid Fail : %2d\n",
      Status
      );
    
    return Status;
  }

  //
  // Show Base Frame
  //
  BaseFrame (
    L"BDA / CMOS Utility",
    BaseFrameTail[0],
    BaseFrameTail[1],
    BaseFrameTail[2]
    );

  //
  // Select BDA / EBDA / CMOS
  //
  SelectItems (
    CpuIo,
    InputEx,
    BaseFrameTailPtr
    );

  //
  // Reset to Orginal Mode
  //
  CLEAN_SCREEN (VOID); 

  Status = gST->ConOut->SetMode (
                          gST->ConOut,
                          OringinalMode
                          );
  
  if (EFI_ERROR (Status)) {
    Print (L"SetMode ERROR\n");
  }
  
  return EFI_SUCCESS;
}
