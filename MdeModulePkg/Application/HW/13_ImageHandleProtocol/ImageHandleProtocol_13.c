/** @file
  ImageHandleProtocol_13 C Source File

;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "ImageHandleProtocol_13.h"

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE ImageHandle, 
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS            Status;
  EFI_INPUT_KEY         InputKey;
  POSITION              CursorPosition;
  EFI_GUID              ProtocolGuid;
  WRITE_BUFFER          WriteBuffer[50];
  CHAR16                Str[50];
  UINT32                i;

  CursorPosition.column = 0;
  CursorPosition.row    = 0;

  InitializeArray(WriteBuffer);

  gST->ConOut->ClearScreen(gST->ConOut);
  gST->ConOut->EnableCursor(gST->ConOut, FALSE);
  SetMainPageAppearance();

  while (TRUE) {
    gST->ConIn->ReadKeyStroke(gST->ConIn, &InputKey);

    if (InputKey.ScanCode == SCAN_ESC) {
      gST->ConOut->ClearScreen(gST->ConOut);
      break;
    }

    switch (InputKey.ScanCode) {

    case SCAN_F1:
      gST->ConOut->ClearScreen(gST->ConOut);
      SearchAllProtocol();
      continue;

    case SCAN_F2:
      gST->ConOut->ClearScreen(gST->ConOut);
      Print(L"Please input GUID.\n");
      InputValue(WriteBuffer, &CursorPosition);
      Print(L"\n");
      ProtocolGuid = ParseGuid(WriteBuffer);
      SearchProtocolByGuid(&ProtocolGuid);
      InitializeArray(WriteBuffer);
      continue;

    case SCAN_F3:
      gST->ConOut->ClearScreen(gST->ConOut);
      Print(L"Please input Protocol name.\n");
      InputWord(WriteBuffer, &CursorPosition);

      for (i = 0; i < 50; i++) {
        Str[i] = (CHAR16)(WriteBuffer[i].value);
      }
      Print(L"Name:%s\nGUID:", Str);
      SearchProtocolGuidByName(Str);
      InitializeArray(WriteBuffer);
      continue;

    case SCAN_DOWN:
      gST->ConOut->ClearScreen(gST->ConOut);
      SetMainPageAppearance();
      continue;
    }
  }

  Status = EFI_SUCCESS;

  return Status; 
}

/**
  To show all handles, protocol GUIDs, protocol names.

  @retval  EFI_STATUS  To check if successful or not.

**/
EFI_STATUS 
SearchAllProtocol (
  VOID
  )
{
  EFI_STATUS            Status;
  EFI_HANDLE            *HandleBuffer;
  UINT64                HandleNumber;
  UINT32                HandleCount;
  EFI_GUID              **ProtocolBuffer;
  UINT64                ProtocolNumber;
  UINT32                ProtocolCount;
  EFI_INPUT_KEY         InputKey;

  gST->ConOut->ClearScreen(gST->ConOut);
  
  Status = gBS->LocateHandleBuffer(
                  AllHandles,
                  NULL,
                  NULL,
                  &HandleNumber,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)){
    gST->ConOut->SetAttribute(gST->ConOut, EFI_YELLOW | EFI_BACKGROUND_BLACK);
    Print(L"%r\nPress down to exit.", Status);
    gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);
    return Status;
    
  } else {
    Print(L"Total:(%3d)\n", HandleNumber);
    for (HandleCount = 0; HandleCount < HandleNumber; HandleCount++) {
      Print(L"Handle:%d --> 0x%.8x\n", HandleCount, HandleBuffer[HandleCount]);

      Status = gBS->ProtocolsPerHandle(
                                    HandleBuffer[HandleCount],
                                    &ProtocolBuffer,
                                    &ProtocolNumber
                                    );
      if (EFI_ERROR (Status)){
        gST->ConOut->SetAttribute(gST->ConOut, EFI_YELLOW | EFI_BACKGROUND_BLACK);
        Print(L"%r\nPress down to exit.", Status);
        gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);
        gBS->FreePool(HandleBuffer);
        return Status;
      } else {

        for (ProtocolCount = 0; ProtocolCount < ProtocolNumber; ProtocolCount++) {

          Print(L"   Protocol: %20s --> GUID:%g\n", GetStringNameFromGuid(
                                                                    *(ProtocolBuffer + ProtocolCount),
                                                                    NULL
                                                                    ), 
                                                 ProtocolBuffer[ProtocolCount]
                                                 );          
        }
        if (HandleCount % 3 == 2) {
          gST->ConOut->SetAttribute(gST->ConOut, EFI_YELLOW | EFI_BACKGROUND_BLACK);
          Print(L"Press down to next page. Press Esc to escape.");
          gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);
          while (TRUE) {
            gST->ConIn->ReadKeyStroke(gST->ConIn, &InputKey);

            if (InputKey.ScanCode == SCAN_DOWN) {
              gST->ConOut->ClearScreen(gST->ConOut);
              Print(L"Total:(%3d)\n", HandleNumber);
              break;
            } else if (InputKey.ScanCode == SCAN_ESC) {
              gST->ConOut->ClearScreen(gST->ConOut);
              gBS->FreePool(HandleBuffer);
              SetMainPageAppearance();
              return Status;
            }
          }
        }
        if (HandleCount + 1 == HandleNumber) {
          gST->ConOut->SetAttribute(gST->ConOut, EFI_YELLOW | EFI_BACKGROUND_BLACK);
          Print(L"Press down to exit.");
          gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);
          while (TRUE) {
            gST->ConIn->ReadKeyStroke(gST->ConIn, &InputKey);

            if (InputKey.ScanCode == SCAN_DOWN) {
              gST->ConOut->ClearScreen(gST->ConOut);
              gBS->FreePool(HandleBuffer);
              SetMainPageAppearance();
              break;
            } 
          }
        }
      }
    }
  }
  return Status;
}

/**
  To show all handles which have required protocol.

  @param   *ProtocolGuid  The appointed protocol GUID.
  
  @retval  EFI_STATUS     To check if successful or not.

**/
EFI_STATUS
SearchProtocolByGuid (
  EFI_GUID *ProtocolGuid
  )
{
  EFI_STATUS            Status;
  EFI_HANDLE            *HandleBuffer;
  UINT64                HandleNumber;
  UINT32                HandleCount;
  EFI_GUID              **ProtocolBuffer;
  UINT64                ProtocolNumber;
  UINT32                ProtocolCount;
  EFI_INPUT_KEY         InputKey;

  gST->ConOut->ClearScreen(gST->ConOut);
  
  Status = gBS->LocateHandleBuffer(
                  ByProtocol,
                  ProtocolGuid,
                  NULL,
                  &HandleNumber,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)){
    gST->ConOut->SetAttribute(gST->ConOut, EFI_YELLOW | EFI_BACKGROUND_BLACK);
    Print(L"%r\nPress down to exit.", Status);
    gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);
    return Status;
    
  } else {
    Print(L"Total:(%3d)\n", HandleNumber);
    for (HandleCount = 0; HandleCount < HandleNumber; HandleCount++) {
      Print(L"Handle:%d --> 0x%.8x\n", HandleCount, HandleBuffer[HandleCount]);

      Status = gBS->ProtocolsPerHandle(
                                    HandleBuffer[HandleCount],
                                    &ProtocolBuffer,
                                    &ProtocolNumber
                                    );
      if (EFI_ERROR (Status)){
        gST->ConOut->SetAttribute(gST->ConOut, EFI_YELLOW | EFI_BACKGROUND_BLACK);
        Print(L"%r\nPress down to exit.", Status);
        gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);
        gBS->FreePool(HandleBuffer);
        return Status;
      } else {

        for (ProtocolCount = 0; ProtocolCount < ProtocolNumber; ProtocolCount++) {

          Print(L"   Protocol: %20s --> GUID:%g\n", GetStringNameFromGuid(
                                                                    *(ProtocolBuffer + ProtocolCount),
                                                                    NULL
                                                                    ), 
                                                 ProtocolBuffer[ProtocolCount]
                                                 );          
        }
        if (HandleCount % 3 == 2) {
          gST->ConOut->SetAttribute(gST->ConOut, EFI_YELLOW | EFI_BACKGROUND_BLACK);
          Print(L"Press down to next page. Press Esc to escape.");
          gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);
          while (TRUE) {
            gST->ConIn->ReadKeyStroke(gST->ConIn, &InputKey);

            if (InputKey.ScanCode == SCAN_DOWN) {
              gST->ConOut->ClearScreen(gST->ConOut);
              Print(L"Total:(%3d)\n", HandleNumber);
              break;
            } else if (InputKey.ScanCode == SCAN_ESC) {
              gST->ConOut->ClearScreen(gST->ConOut);
              gBS->FreePool(HandleBuffer);
              SetMainPageAppearance();
              return Status;
            }
          }
        }
        if (HandleCount + 1 == HandleNumber) {
          gST->ConOut->SetAttribute(gST->ConOut, EFI_YELLOW | EFI_BACKGROUND_BLACK);
          Print(L"Press down to exit.");
          gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);
          while (TRUE) {
            gST->ConIn->ReadKeyStroke(gST->ConIn, &InputKey);

            if (InputKey.ScanCode == SCAN_DOWN) {
              gST->ConOut->ClearScreen(gST->ConOut);
              gBS->FreePool(HandleBuffer);
              SetMainPageAppearance();
              break;
            } 
          }
        }
      }
    }
  }
  return Status;
}

/**
  To show protocol GUID by name.

  @param   *Str        The name we input.
  
  @retval  EFI_STATUS  To check if successful or not.

**/
EFI_STATUS 
SearchProtocolGuidByName (
  CHAR16 *Str
  )
{
  EFI_STATUS            Status;
  EFI_GUID              *ProtocolBuffer;
  EFI_INPUT_KEY         InputKey;

  Status = GetGuidFromStringName(
             Str, 
             NULL, 
             &ProtocolBuffer
             );

  if (EFI_ERROR (Status)){
    Print(L"%r\n", Status);
    gST->ConOut->SetAttribute(gST->ConOut, EFI_YELLOW | EFI_BACKGROUND_BLACK);
    Print(L"Press down to exit.");
    gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);
    return Status;
  }

  Print(L"%g\n", ProtocolBuffer);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_YELLOW | EFI_BACKGROUND_BLACK);
  Print(L"Press down to exit.");
  gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);

  while (TRUE) {
    gST->ConIn->ReadKeyStroke(gST->ConIn, &InputKey);

    switch(InputKey.ScanCode){
    
    case SCAN_DOWN:
      gST->ConOut->ClearScreen(gST->ConOut);
      SetMainPageAppearance();
      return Status;
    }
  }
}

/**
  To input value, when input 32 numbers(GUID), return.

  @param  WriteBuffer        The buffer to store number.
  @param  *CursorPosition    The cursor's position.

  @retval  UINT32  The GUID number.

**/
UINT32
InputValue (
  WRITE_BUFFER  *WriteBuffer,
  POSITION      *CursorPosition
  )
{
  EFI_INPUT_KEY         InputKey;
  INT32                WriteCount;
  BOOLEAN               Flag;

  WriteCount = 0;
  Flag = 0;

  while (TRUE) {
    gST->ConIn->ReadKeyStroke(gST->ConIn, &InputKey);

    if (WriteCount == 32) {
      return WriteCount;
    }

    if (('a' <= InputKey.UnicodeChar && InputKey.UnicodeChar <= 'f')) {
      
      WriteBuffer[WriteCount].value = InputKey.UnicodeChar - 87;
      Print(L"%c", InputKey.UnicodeChar);
      WriteCount++;
      
      continue;

    } else if (('0' <= InputKey.UnicodeChar && InputKey.UnicodeChar <= '9')) {
      WriteBuffer[WriteCount].value = InputKey.UnicodeChar - 48;
      Print(L"%c", InputKey.UnicodeChar);
      WriteCount++;

      continue;
    } else if (InputKey.UnicodeChar == CHAR_BACKSPACE) {

      WriteCount--;
    
      if(WriteCount < 0) {
        WriteCount = 0;
        continue;
      } 
    
      WriteBuffer[WriteCount].value = CHAR_NULL;
      Print(L"%c", InputKey.UnicodeChar);

      continue;
    }
  }
}

/**
  To input word, when input Enter, get string and return.

  @param   WriteBuffer      The buffer to store character.
  @param   *CursorPosition  The cursor's position.

  @retval  UINT32           The string length.

**/
UINT32 
InputWord (
  WRITE_BUFFER *WriteBuffer, 
  POSITION *CursorPosition
  )
{
  EFI_INPUT_KEY         InputKey;
  INT32                WriteCount;

  WriteCount = 0;

  while (TRUE) {
    gST->ConIn->ReadKeyStroke(gST->ConIn, &InputKey);

    if (WriteCount >= 50) {
      return WriteCount;
    }

    switch(InputKey.UnicodeChar){
    case CHAR_NULL:
      continue;
    
    case CHAR_CARRIAGE_RETURN:
      gST->ConOut->ClearScreen(gST->ConOut);
      return WriteCount;
    
    case CHAR_BACKSPACE:
      WriteCount--;
      if (WriteCount < 0) {
        WriteCount = 0;
      }
      WriteBuffer[WriteCount].value = CHAR_NULL;
      Print(L"%c", InputKey.UnicodeChar); 
      
      continue;

    default:
      WriteBuffer[WriteCount].value = InputKey.UnicodeChar;
      Print(L"%c", InputKey.UnicodeChar);
      WriteCount++;
      continue;
    }
  }
}

/**
  Parse the number returned from InputValue(), and return a Guid.

  @param   WriteBuffer  The buffer to store number.

  @retval  EFI_GUID     The GUID structure.

**/
EFI_GUID 
ParseGuid (
  WRITE_BUFFER *WriteBuffer
  )
{
  EFI_GUID TestGuid;
  UINT32   i;
  UINT32   j;
  
  TestGuid.Data1 = 0;
  TestGuid.Data2 = 0;
  TestGuid.Data3 = 0;
  

  for (i = 0; i < 8; i++) {
    TestGuid.Data4[i] = 0;
  }

  for (i = 0; i < 8; i++) {
    TestGuid.Data1 |= WriteBuffer[i].value << ((8-i-1) * 4);
  }

  for (i = 8; i < 12; i++) {
    TestGuid.Data2 |= WriteBuffer[i].value << ((12-i-1) * 4);
  }

  for (i = 12; i < 16; i++) {
    TestGuid.Data3 |= WriteBuffer[i].value << ((16-i-1) * 4);
  }

  for (j = 0; j < 8; j++) {
    for (i = 16 + 2*j; i < 18 + 2*j; i++) {
      TestGuid.Data4[j] |= WriteBuffer[i].value << (((18+2*j)-i-1) * 4);
    }
  }

  return TestGuid;
}

/**
  To set main page appearance.

**/
VOID 
SetMainPageAppearance (
  VOID
  )
{
  gST->ConOut->ClearScreen(gST->ConOut);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);

  Print(L"|====================================================|\n");
  Print(L"|                                                    |\n");
  Print(L"|              13-Image/Handle/Protocol              |\n");
  Print(L"|                                                    |\n");
  Print(L"|====================================================|\n");
  Print(L"|                                                    |\n");
  Print(L"| [F1] : Search all Handle and Protocol              |\n");
  Print(L"| [F2] : Search Protocol name by GUID                |\n");
  Print(L"| [F3] : Search GUID by Protocol name                |\n");
  Print(L"| [Esc] : Escape                                     |\n");
  Print(L"|====================================================|\n");

  return;
}

/**
  To initialize WriteBuffer.

**/
VOID 
InitializeArray(
  WRITE_BUFFER *WriteBuffer
  )
{
  UINT32 i;

  for (i = 0; i < 50; i++) {
    WriteBuffer[i].value = 0;
    WriteBuffer[i].position.column = 0;
    WriteBuffer[i].position.row = 0;
  }
}