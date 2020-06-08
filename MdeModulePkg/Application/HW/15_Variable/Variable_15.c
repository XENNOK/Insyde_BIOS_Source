/** @file
  Variable_15 C Source File

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

#include "Variable_15.h"

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE ImageHandle, 
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS      Status;
  EFI_INPUT_KEY   InputKey;

  gST->ConOut->ClearScreen(gST->ConOut);
  ShowMainPage();

  while (TRUE) {
    gST->ConIn->ReadKeyStroke(gST->ConIn, &InputKey);

    if (InputKey.ScanCode == SCAN_ESC) {
      gST->ConOut->ClearScreen(gST->ConOut);
      break;
    }

    switch (InputKey.ScanCode) {

    case SCAN_F1:
      gST->ConOut->ClearScreen(gST->ConOut);
      SearchAllVariable();
      PressStop();
      ShowMainPage();
      continue;

    case SCAN_F2:
      gST->ConOut->ClearScreen(gST->ConOut);
      SearchVariableByName();
      PressStop();
      ShowMainPage();
      continue;

    case SCAN_F3:
      gST->ConOut->ClearScreen(gST->ConOut);
      SearchVariableByGuid();
      PressStop();
      ShowMainPage();
      continue;

    case SCAN_F4:
      gST->ConOut->ClearScreen(gST->ConOut);
      SetCreateVariable();
      PressStop();
      ShowMainPage();
      continue;
    
    case SCAN_F5:
      gST->ConOut->ClearScreen(gST->ConOut);
      DeleteVariable();
      PressStop();
      ShowMainPage();
      continue;
    }
  }

  Status = EFI_SUCCESS;

  return Status; 
}

EFI_STATUS
SearchAllVariable (
  VOID
  )
{
  EFI_STATUS      Status;
  EFI_INPUT_KEY   InputKey;
  UINT32          Count;
  UINT32          i;

  UINT64          VariableSize;
  CHAR16          *VariableName;
  EFI_GUID        VendorGuid;

  UINT64          OldNameSize;
  CHAR16          *OldName;
    
  UINT32          Attributes;
  UINT64          DataSize;
  UINT8           *Data;

  UINT64          OldDataSize;
  UINT8           *OldData;

  VariableSize = 2;
  VariableName = AllocateZeroPool(VariableSize);

  Attributes = 0;
  DataSize = 0;
  Data = AllocateZeroPool(DataSize);

  Count = 1;
  while (TRUE) {
    OldNameSize = VariableSize;
    OldName = VariableName;
    OldDataSize = DataSize;
    OldData = Data;

    Status = gRT->GetNextVariableName(
                                 &VariableSize,
                                 VariableName,
                                 &VendorGuid 
                                 );
    if (EFI_ERROR(Status) && Status != EFI_BUFFER_TOO_SMALL) {
      Print(L"%r\n", Status);
      Print(L"Press any key to back main page.\n");
      break;
    }

    if (Status == EFI_BUFFER_TOO_SMALL) {
      VariableName = ReallocatePool(OldNameSize, VariableSize, NULL);
      InitializeArray_16(VariableName, VariableSize);
      gBS->CopyMem(VariableName, OldName, OldNameSize);
      Status = gRT->GetNextVariableName(
                                 &VariableSize,
                                 VariableName,
                                 &VendorGuid 
                                 );
      if (EFI_ERROR(Status)) {
        Print(L"%r\n", Status);
        break;
      }

      Status = gRT->GetVariable(
                             VariableName,
                             &VendorGuid,
                             &Attributes,
                             &DataSize,
                             (VOID*)Data
                             );
      if (EFI_ERROR(Status) && Status != EFI_BUFFER_TOO_SMALL) {
        Print(L"%r\n", Status);
        break;
      } 
      
      if (Status == EFI_BUFFER_TOO_SMALL) {
        Data = ReallocatePool(OldDataSize, DataSize, NULL);
        InitializeArray_8(Data, DataSize);
        gBS->CopyMem(Data, OldData, OldDataSize);
        Status = gRT->GetVariable(
                             VariableName,
                             &VendorGuid,
                             &Attributes,
                             &DataSize,
                             (VOID*)Data
                             );
        if (EFI_ERROR(Status)) {
          Print(L"%r\n", Status);
          break;
        }
        Print(L"No. %d\n", Count);
        Print(L"Variable Name : %s\n", VariableName);
        Print(L"Variable GUID : %g\n", VendorGuid);
        Print(L"Attributes    : %d\n", Attributes);
        Print(L"Data Size     : %d\n", DataSize);
        Print(L"Data :\n");
        for (i = 0; i < DataSize; i++) {
          Print(L"%3.2x", *(Data + i));
          if ((i % 16) == 15 || i == DataSize - 1) Print(L"\n");
        }
        gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);
        Print(L"[Down]:Next   [Esc]:Escape\n");
        gST->ConOut->SetAttribute(gST->ConOut, EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
        ShowAttributeTable();

      } else {
        Print(L"No. %d\n", Count);
        Print(L"Variable Name : %s\n", VariableName);
        Print(L"Variable GUID : %g\n", VendorGuid);
        Print(L"Attributes    : %d\n", Attributes);
        Print(L"Data Size     : %d\n", DataSize);
        Print(L"Data :\n");
        for (i = 0; i < DataSize; i++) {
          Print(L"%3.2x", *(Data + i));
          if ((i % 16) == 15 || i == DataSize - 1) Print(L"\n");
        }
        gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);
        Print(L"[Down]:Next   [Esc]:Escape\n");
        gST->ConOut->SetAttribute(gST->ConOut, EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
        ShowAttributeTable();
      }

    } else {
      Status = gRT->GetVariable(
                             VariableName,
                             &VendorGuid,
                             &Attributes,
                             &DataSize,
                             (VOID*)Data
                             );
      if (EFI_ERROR(Status) && Status != EFI_BUFFER_TOO_SMALL) {
        Print(L"%r\n", Status);
        break;
      } 
      
      if (Status == EFI_BUFFER_TOO_SMALL) {
        Data = ReallocatePool(OldDataSize, DataSize, NULL);
        InitializeArray_8(Data, DataSize);
        gBS->CopyMem(Data, OldData, OldDataSize);
        Status = gRT->GetVariable(
                             VariableName,
                             &VendorGuid,
                             &Attributes,
                             &DataSize,
                             (VOID*)Data
                             );
        if (EFI_ERROR(Status)) {
          Print(L"%r\n", Status);
          break;
        }
        Print(L"No. %d\n", Count);
        Print(L"Variable Name : %s\n", VariableName);
        Print(L"Variable GUID : %g\n", VendorGuid);
        Print(L"Attributes    : %d\n", Attributes);
        Print(L"Data Size     : %d\n", DataSize);
        Print(L"Data :\n");
        for (i = 0; i < DataSize; i++) {
          Print(L"%3.2x", *(Data + i));
          if ((i % 16) == 15 || i == DataSize - 1) Print(L"\n");
        }
        gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);
        Print(L"[Down]:Next   [Esc]:Escape\n");
        gST->ConOut->SetAttribute(gST->ConOut, EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
        ShowAttributeTable();
  
      } else {
        Print(L"No. %d\n", Count);
        Print(L"Variable Name : %s\n", VariableName);
        Print(L"Variable GUID : %g\n", VendorGuid);
        Print(L"Attributes    : %d\n", Attributes);
        Print(L"Data Size     : %d\n", DataSize);
        Print(L"Data :\n");
        for (i = 0; i < DataSize; i++) {
          Print(L"%3.2x", *(Data + i));
          if ((i % 16) == 15 || i == DataSize - 1) Print(L"\n");
        }
        gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);
        Print(L"[Down]:Next   [Esc]:Escape\n");
        gST->ConOut->SetAttribute(gST->ConOut, EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
        ShowAttributeTable();
      }
    }

    while (TRUE) {
      gST->ConIn->ReadKeyStroke(gST->ConIn, &InputKey);

      if (InputKey.ScanCode == SCAN_ESC) {
        gST->ConOut->ClearScreen(gST->ConOut);
        Print(L"Press any key to back main page.\n");
        goto AllExit;
      }

      if (InputKey.ScanCode == SCAN_DOWN) {
        gST->ConOut->ClearScreen(gST->ConOut);
        Count++;
        break;
      }
    }
  }   


  AllExit:

  FreePool(VariableName);
  FreePool(Data);

  Status = EFI_SUCCESS;

  return Status;
}

EFI_STATUS
SearchVariableByName (
  VOID
  )
{
  EFI_STATUS      Status;
  EFI_INPUT_KEY   InputKey;
  UINT32          Count;
  UINT32          i;

  UINT64          VariableSize;
  CHAR16          *VariableName;
  EFI_GUID        VendorGuid;

  UINT64          OldNameSize;
  CHAR16          *OldName;
    
  UINT32          Attributes;
  UINT64          DataSize;
  UINT8           *Data;

  UINT64          OldDataSize;
  UINT8           *OldData;

  CHAR16          *SearchName;

  VariableSize = 2;
  VariableName = AllocateZeroPool(VariableSize);

  Attributes = 0;
  DataSize = 0;
  Data = AllocateZeroPool(DataSize);

  Print(L"Please input search name.\n");
  SearchName = InputWord();

  gST->ConOut->ClearScreen(gST->ConOut);

  Count = 1;
  while (TRUE) {
    OldNameSize = VariableSize;
    OldName = VariableName;
    OldDataSize = DataSize;
    OldData = Data;

    Status = gRT->GetNextVariableName(
                                 &VariableSize,
                                 VariableName,
                                 &VendorGuid 
                                 );
    if (EFI_ERROR(Status) && Status != EFI_BUFFER_TOO_SMALL) {
      Print(L"%r\n", Status);
      Print(L"Press any key to back main page.\n");
      break;
    }

    if (Status == EFI_BUFFER_TOO_SMALL) {
      VariableName = ReallocatePool(OldNameSize, VariableSize, NULL);
      InitializeArray_16(VariableName, VariableSize);
      gBS->CopyMem(VariableName, OldName, OldNameSize);
      Status = gRT->GetNextVariableName(
                                 &VariableSize,
                                 VariableName,
                                 &VendorGuid 
                                 );
      if (EFI_ERROR(Status)) {
        Print(L"%r\n", Status);
        break;
      }

      Status = gRT->GetVariable(
                             VariableName,
                             &VendorGuid,
                             &Attributes,
                             &DataSize,
                             (VOID*)Data
                             );
      if (EFI_ERROR(Status) && Status != EFI_BUFFER_TOO_SMALL) {
        Print(L"%r\n", Status);
        break;
      } 
      
      if (Status == EFI_BUFFER_TOO_SMALL) {
        Data = ReallocatePool(OldDataSize, DataSize, NULL);
        InitializeArray_8(Data, DataSize);
        gBS->CopyMem(Data, OldData, OldDataSize);
        Status = gRT->GetVariable(
                             VariableName,
                             &VendorGuid,
                             &Attributes,
                             &DataSize,
                             (VOID*)Data
                             );
        if (EFI_ERROR(Status)) {
          Print(L"%r\n", Status);
          break;
        }
        if (StrCmp(SearchName, VariableName) == EFI_SUCCESS) {
          Print(L"No. %d\n", Count);
          Print(L"Variable Name : %s\n", VariableName);
          Print(L"Variable GUID : %g\n", VendorGuid);
          Print(L"Attributes    : %d\n", Attributes);
          Print(L"Data Size     : %d\n", DataSize);
          Print(L"Data :\n");
          for (i = 0; i < DataSize; i++) {
            Print(L"%3.2x", *(Data + i));
            if ((i % 16) == 15 || i == DataSize - 1) Print(L"\n");
          }
          gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);
          Print(L"[Down]:Next   [Esc]:Escape\n");
          gST->ConOut->SetAttribute(gST->ConOut, EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
          ShowAttributeTable();
        }

      } else {
        if (StrCmp(SearchName, VariableName) == EFI_SUCCESS) {
          Print(L"No. %d\n", Count);
          Print(L"Variable Name : %s\n", VariableName);
          Print(L"Variable GUID : %g\n", VendorGuid);
          Print(L"Attributes    : %d\n", Attributes);
          Print(L"Data Size     : %d\n", DataSize);
          Print(L"Data :\n");
          for (i = 0; i < DataSize; i++) {
            Print(L"%3.2x", *(Data + i));
            if ((i % 16) == 15 || i == DataSize - 1) Print(L"\n");
          }
          gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);
          Print(L"[Down]:Next   [Esc]:Escape\n");
          gST->ConOut->SetAttribute(gST->ConOut, EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
          ShowAttributeTable();
        }
      }

    } else {
      Status = gRT->GetVariable(
                             VariableName,
                             &VendorGuid,
                             &Attributes,
                             &DataSize,
                             (VOID*)Data
                             );
      if (EFI_ERROR(Status) && Status != EFI_BUFFER_TOO_SMALL) {
        Print(L"%r\n", Status);
        break;
      } 
      
      if (Status == EFI_BUFFER_TOO_SMALL) {
        Data = ReallocatePool(OldDataSize, DataSize, NULL);
        InitializeArray_8(Data, DataSize);
        gBS->CopyMem(Data, OldData, OldDataSize);
        Status = gRT->GetVariable(
                             VariableName,
                             &VendorGuid,
                             &Attributes,
                             &DataSize,
                             (VOID*)Data
                             );
        if (EFI_ERROR(Status)) {
          Print(L"%r\n", Status);
          break;
        }
        if (StrCmp(SearchName, VariableName) == EFI_SUCCESS) {
          Print(L"No. %d\n", Count);
          Print(L"Variable Name : %s\n", VariableName);
          Print(L"Variable GUID : %g\n", VendorGuid);
          Print(L"Attributes    : %d\n", Attributes);
          Print(L"Data Size     : %d\n", DataSize);
          Print(L"Data :\n");
          for (i = 0; i < DataSize; i++) {
            Print(L"%3.2x", *(Data + i));
            if ((i % 16) == 15 || i == DataSize - 1) Print(L"\n");
          }
          gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);
          Print(L"[Down]:Next   [Esc]:Escape\n");
          gST->ConOut->SetAttribute(gST->ConOut, EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
          ShowAttributeTable();
        }
  
      } else {
        if (StrCmp(SearchName, VariableName) == EFI_SUCCESS) {
          Print(L"No. %d\n", Count);
          Print(L"Variable Name : %s\n", VariableName);
          Print(L"Variable GUID : %g\n", VendorGuid);
          Print(L"Attributes    : %d\n", Attributes);
          Print(L"Data Size     : %d\n", DataSize);
          Print(L"Data :\n");
          for (i = 0; i < DataSize; i++) {
            Print(L"%3.2x", *(Data + i));
            if ((i % 16) == 15 || i == DataSize - 1) Print(L"\n");
          }
          gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);
          Print(L"[Down]:Next   [Esc]:Escape\n");
          gST->ConOut->SetAttribute(gST->ConOut, EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
          ShowAttributeTable();
        }
      }
    }
    
    while (StrCmp(SearchName, VariableName) == EFI_SUCCESS) {
      gST->ConIn->ReadKeyStroke(gST->ConIn, &InputKey);

      if (InputKey.ScanCode == SCAN_ESC) {
        gST->ConOut->ClearScreen(gST->ConOut);
        Print(L"Press any key to back main page.\n");
        goto NameExit;
      }

      if (InputKey.ScanCode == SCAN_DOWN) {
        gST->ConOut->ClearScreen(gST->ConOut);
        Count++;
        break;
      }
    }
  }   


  NameExit:

  FreePool(VariableName);
  FreePool(Data);
  Status = EFI_SUCCESS;

  return Status;
}

EFI_STATUS
SearchVariableByGuid (
  VOID
  )
{
  EFI_STATUS      Status;
  EFI_INPUT_KEY   InputKey;
  UINT32          Count;
  UINT32          i;

  UINT64          VariableSize;
  CHAR16          *VariableName;
  EFI_GUID        VendorGuid;

  UINT64          OldNameSize;
  CHAR16          *OldName;
    
  UINT32          Attributes;
  UINT64          DataSize;
  UINT8           *Data;

  UINT64          OldDataSize;
  UINT8           *OldData;

  UINT64          Buffer[32];
  EFI_GUID        SearchGuid;

  VariableSize = 2;
  VariableName = AllocateZeroPool(VariableSize);

  Attributes = 0;
  DataSize = 0;
  Data = AllocateZeroPool(DataSize);
  
  Print(L"Please input search GUID.\n");
  SearchGuid = ParseGuid(InputGuid(Buffer));

  gST->ConOut->ClearScreen(gST->ConOut);

  Count = 1;
  while (TRUE) {
    OldNameSize = VariableSize;
    OldName = VariableName;
    OldDataSize = DataSize;
    OldData = Data;

    Status = gRT->GetNextVariableName(
                                 &VariableSize,
                                 VariableName,
                                 &VendorGuid 
                                 );
    if (EFI_ERROR(Status) && Status != EFI_BUFFER_TOO_SMALL) {
      Print(L"%r\n", Status);
      Print(L"Press any key to back main page.\n");
      break;
    }

    if (Status == EFI_BUFFER_TOO_SMALL) {
      VariableName = ReallocatePool(OldNameSize, VariableSize, NULL);
      InitializeArray_16(VariableName, VariableSize);
      gBS->CopyMem(VariableName, OldName, OldNameSize);
      Status = gRT->GetNextVariableName(
                                 &VariableSize,
                                 VariableName,
                                 &VendorGuid 
                                 );
      if (EFI_ERROR(Status)) {
        Print(L"%r\n", Status);
        break;
      }

      Status = gRT->GetVariable(
                             VariableName,
                             &VendorGuid,
                             &Attributes,
                             &DataSize,
                             (VOID*)Data
                             );
      if (EFI_ERROR(Status) && Status != EFI_BUFFER_TOO_SMALL) {
        Print(L"%r\n", Status);
        break;
      } 
      
      if (Status == EFI_BUFFER_TOO_SMALL) {
        Data = ReallocatePool(OldDataSize, DataSize, NULL);
        InitializeArray_8(Data, DataSize);
        gBS->CopyMem(Data, OldData, OldDataSize);
        Status = gRT->GetVariable(
                             VariableName,
                             &VendorGuid,
                             &Attributes,
                             &DataSize,
                             (VOID*)Data
                             );
        if (EFI_ERROR(Status)) {
          Print(L"%r\n", Status);
          break;
        }
        if (CompareMem(&SearchGuid, &VendorGuid, sizeof(EFI_GUID)) == EFI_SUCCESS) {
          Print(L"No. %d\n", Count);
          Print(L"Variable Name : %s\n", VariableName);
          Print(L"Variable GUID : %g\n", VendorGuid);
          Print(L"Attributes    : %d\n", Attributes);
          Print(L"Data Size     : %d\n", DataSize);
          Print(L"Data :\n");
          for (i = 0; i < DataSize; i++) {
            Print(L"%3.2x", *(Data + i));
            if ((i % 16) == 15 || i == DataSize - 1) Print(L"\n");
          }
          gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);
          Print(L"[Down]:Next   [Esc]:Escape\n");
          gST->ConOut->SetAttribute(gST->ConOut, EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
          ShowAttributeTable();
        }

      } else {
        if (CompareMem(&SearchGuid, &VendorGuid, sizeof(EFI_GUID)) == EFI_SUCCESS) {
          Print(L"No. %d\n", Count);
          Print(L"Variable Name : %s\n", VariableName);
          Print(L"Variable GUID : %g\n", VendorGuid);
          Print(L"Attributes    : %d\n", Attributes);
          Print(L"Data Size     : %d\n", DataSize);
          Print(L"Data :\n");
          for (i = 0; i < DataSize; i++) {
            Print(L"%3.2x", *(Data + i));
            if ((i % 16) == 15 || i == DataSize - 1) Print(L"\n");
          }
          gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);
          Print(L"[Down]:Next   [Esc]:Escape\n");
          gST->ConOut->SetAttribute(gST->ConOut, EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
          ShowAttributeTable();
        }
      }

    } else {
      Status = gRT->GetVariable(
                             VariableName,
                             &VendorGuid,
                             &Attributes,
                             &DataSize,
                             (VOID*)Data
                             );
      if (EFI_ERROR(Status) && Status != EFI_BUFFER_TOO_SMALL) {
        Print(L"%r\n", Status);
        break;
      } 
      
      if (Status == EFI_BUFFER_TOO_SMALL) {
        Data = ReallocatePool(OldDataSize, DataSize, NULL);
        InitializeArray_8(Data, DataSize);
        gBS->CopyMem(Data, OldData, OldDataSize);
        Status = gRT->GetVariable(
                             VariableName,
                             &VendorGuid,
                             &Attributes,
                             &DataSize,
                             (VOID*)Data
                             );
        if (EFI_ERROR(Status)) {
          Print(L"%r\n", Status);
          break;
        }
        if (CompareMem(&SearchGuid, &VendorGuid, sizeof(EFI_GUID)) == EFI_SUCCESS) {
          Print(L"No. %d\n", Count);
          Print(L"Variable Name : %s\n", VariableName);
          Print(L"Variable GUID : %g\n", VendorGuid);
          Print(L"Attributes    : %d\n", Attributes);
          Print(L"Data Size     : %d\n", DataSize);
          Print(L"Data :\n");
          for (i = 0; i < DataSize; i++) {
            Print(L"%3.2x", *(Data + i));
            if ((i % 16) == 15 || i == DataSize - 1) Print(L"\n");
          }
          gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);
          Print(L"[Down]:Next   [Esc]:Escape\n");
          gST->ConOut->SetAttribute(gST->ConOut, EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
          ShowAttributeTable();
        }
  
      } else {
        if (CompareMem(&SearchGuid, &VendorGuid, sizeof(EFI_GUID)) == EFI_SUCCESS) {
          Print(L"No. %d\n", Count);
          Print(L"Variable Name : %s\n", VariableName);
          Print(L"Variable GUID : %g\n", VendorGuid);
          Print(L"Attributes    : %d\n", Attributes);
          Print(L"Data Size     : %d\n", DataSize);
          Print(L"Data :\n");
          for (i = 0; i < DataSize; i++) {
            Print(L"%3.2x", *(Data + i));
            if ((i % 16) == 15 || i == DataSize - 1) Print(L"\n");
          }
          gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);
          Print(L"[Down]:Next   [Esc]:Escape\n");
          gST->ConOut->SetAttribute(gST->ConOut, EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
          ShowAttributeTable();
        }
      }
    }
    
    while (CompareMem(&SearchGuid, &VendorGuid, sizeof(EFI_GUID)) == EFI_SUCCESS) {
      gST->ConIn->ReadKeyStroke(gST->ConIn, &InputKey);

      if (InputKey.ScanCode == SCAN_ESC) {
        gST->ConOut->ClearScreen(gST->ConOut);
        Print(L"Press any key to back main page.\n");
        goto GuidExit;
      }

      if (InputKey.ScanCode == SCAN_DOWN) {
        gST->ConOut->ClearScreen(gST->ConOut);
        Count++;
        break;
      }
    }
  }   


  GuidExit:

  FreePool(VariableName);
  FreePool(Data);
  Status = EFI_SUCCESS;

  return Status;
}

EFI_STATUS
SetCreateVariable (
  VOID
  )
{
  EFI_STATUS      Status;
  CHAR16          *VariableName;
  EFI_GUID        VendorGuid;
  UINT32          Attributes;
  UINT64          DataSize;
  UINT8           *Data;
  UINT32 i;

  UINT64          GuidBuffer[32];

  gST->ConOut->ClearScreen(gST->ConOut);

  Print(L"Please input name.\n");
  VariableName = InputWord();

  Print(L"\nPlease input GUID.\n");
  VendorGuid = ParseGuid(InputGuid(GuidBuffer));

  Print(L"\nPlease input attributes.\n");
  Attributes = (UINT32)InputValueD();

  Print(L"\nPlease input data size.\n");
  DataSize = InputValueD();

  Print(L"\nPlease input data in byte.\n");
  Data = AllocateZeroPool(DataSize);
  for (i = 0; i < DataSize; i++) {
    Print(L" ");
    *(Data + (UINT8)i) = (UINT8)InputValueH();
    if ((i % 16) == 15) Print(L"\n");
  }

  Status = gRT->SetVariable(
                         VariableName,
                         &VendorGuid,
                         Attributes,
                         DataSize,
                         Data
                         );
  if (EFI_ERROR(Status)) {
    Print(L"%r\n", Status);
    return Status;

  } else {
    Print(L"\nSet done.\n");
    Print(L"Press any key to back main page.\n");
    return Status;
  }  
}

EFI_STATUS
DeleteVariable (
  VOID
  )
{
  EFI_STATUS      Status;

  CHAR16          *VariableName;
  EFI_GUID        VendorGuid;

  UINT32          Attributes;
  UINT64          DataSize;
  UINT8           *Data;

  UINT64          GuidBuffer[32];

  Attributes = 0;
  DataSize = 0;
  Data = NULL;

  gST->ConOut->ClearScreen(gST->ConOut);

  Print(L"Please input name.\n");
  VariableName = InputWord();

  Print(L"\nPlease input GUID.\n");
  VendorGuid = ParseGuid(InputGuid(GuidBuffer));

  Status = gRT->SetVariable(
                         VariableName,
                         &VendorGuid,
                         Attributes,
                         DataSize,
                         Data
                         );
  if (EFI_ERROR(Status)) {
    Print(L"%r\n", Status);
    return Status;

  } else {
    Print(L"\nDelete done.\n");
    Print(L"Press any key to back main page.\n");
    return Status;
  } 
}

VOID
ShowMainPage (
  VOID
  )
{
  gST->ConOut->ClearScreen(gST->ConOut);
  gST->ConOut->SetCursorPosition(gST->ConOut, 0, 0);
  gST->ConOut->EnableCursor(gST->ConOut, FALSE);

  Print(L"15-Variable\n\n");  
  Print(L"[F1].  Search all variable.\n");
  Print(L"[F2].  Search variable by name.\n");
  Print(L"[F3].  Search variable by GUID.\n");
  Print(L"[F4].  Set/Create variable.\n");
  Print(L"[F5].  Delete variable.\n");
  Print(L"[Esc]. Escape\n");

  return;
}

VOID
PressStop (
  VOID
  )
{
  EFI_INPUT_KEY   InputKey;

  while (TRUE) {
    gST->ConIn->ReadKeyStroke(gST->ConIn, &InputKey);
    if(InputKey.UnicodeChar != CHAR_NULL || InputKey.ScanCode != SCAN_NULL) {
      return;
    }
  }
}

CHAR16* 
InputWord (
  VOID
  )
{
  EFI_INPUT_KEY    InputKey;
  INT32            Count;           // The amount of we input.
  UINT32           AllocateSize;
  UINT32           i;               // For loop count.
  CHAR16           *Buffer;
  CHAR16           *OldBuffer;

  Count = 0;
  AllocateSize = WORD_INITIAL_SIZE;
  Buffer = NULL;
  OldBuffer = NULL;

  Buffer = AllocatePool(AllocateSize);
  InitializeArray_16(Buffer, AllocateSize);

  while (TRUE) {
    gST->ConIn->ReadKeyStroke(gST->ConIn, &InputKey);

    if (Count >= (INT32)AllocateSize / 2) {
      OldBuffer = Buffer;
      Buffer = ReallocatePool(AllocateSize, AllocateSize + WORD_SIZE_INCREMENT, NULL);
      AllocateSize += WORD_SIZE_INCREMENT;
      InitializeArray_16(Buffer, AllocateSize);
      for (i = 0; i < (AllocateSize - WORD_SIZE_INCREMENT) / 2; i++) {
        Buffer[i] = OldBuffer[i];
      }
    }

    switch(InputKey.UnicodeChar){
    case CHAR_NULL:
      continue;
    
    case CHAR_CARRIAGE_RETURN:
      *(Buffer + AllocateSize) = 0;
      FreePool(OldBuffer);
      return Buffer;
    
    case CHAR_BACKSPACE:
      Count--;
      if (Count < 0) {
        Count = 0;
      }
      Buffer[Count] = CHAR_NULL;
      Print(L"%c", InputKey.UnicodeChar); 
      
      continue;

    default:
      Buffer[Count] = InputKey.UnicodeChar;
      Print(L"%c", InputKey.UnicodeChar);
      Count++;
      continue;
    }
  }
}

UINT64
InputValueD (
  VOID
  )
{
  EFI_INPUT_KEY    InputKey;
  INT64            Count;           // The amount of we input.
  UINT64           AllocateSize;
  UINT64           i;               // For loop count.
  CHAR8           *Buffer;
  CHAR8           *OldBuffer;

  UINT64          ReturnValue;

  Count = 0;
  AllocateSize = VALUE_INITIAL_SIZE;
  // AllocateSize = 0;
  Buffer = NULL;
  OldBuffer = NULL;

  ReturnValue = 0;

  Buffer = AllocatePool(AllocateSize);
  InitializeArray_8(Buffer, AllocateSize);

  while (TRUE) {
    gST->ConIn->ReadKeyStroke(gST->ConIn, &InputKey);

    if (Count >= (INT64)AllocateSize) {
      OldBuffer = Buffer;
      Buffer = ReallocatePool(AllocateSize, AllocateSize + VALUE_SIZE_INCREMENT, NULL);
      AllocateSize += VALUE_SIZE_INCREMENT;
      InitializeArray_8(Buffer, AllocateSize);
      for (i = 0; i < (AllocateSize - VALUE_SIZE_INCREMENT); i++) {
        Buffer[i] = OldBuffer[i];
      }
    }

    if (('0' <= InputKey.UnicodeChar && InputKey.UnicodeChar <= '9')) {
      Buffer[Count] = (CHAR8)InputKey.UnicodeChar - 48;
      Print(L"%d", Buffer[Count]);
      Count++;
      continue;

    } else if (InputKey.UnicodeChar == CHAR_BACKSPACE) {
      Count--;
      if (Count < 0) {
        Count = 0;
      }
      Buffer[Count] = CHAR_NULL;
      Print(L"%c", InputKey.UnicodeChar);
      continue;

    } else if (InputKey.UnicodeChar == CHAR_CARRIAGE_RETURN) {
      for (i = 0; i < (UINT64)Count; i++) {
        ReturnValue += Buffer[i] * Power((UINT64)10, (UINT64)(Count-i-1));            
      }
      
      FreePool(Buffer);
      FreePool(OldBuffer);
      return ReturnValue;
    }
  }
}

UINT64
InputValueH (
  VOID
  )
{
  EFI_INPUT_KEY    InputKey;
  INT64            Count;           // The amount of we input.
  UINT64           AllocateSize;
  UINT64           i;               // For loop count.
  CHAR8           *Buffer;
  CHAR8           *OldBuffer;

  UINT64          ReturnValue;

  Count = 0;
  AllocateSize = VALUE_INITIAL_SIZE;
  Buffer = NULL;
  OldBuffer = NULL;

  ReturnValue = 0;

  Buffer = AllocatePool(AllocateSize);
  InitializeArray_8(Buffer, AllocateSize);

  while (TRUE) {
    gST->ConIn->ReadKeyStroke(gST->ConIn, &InputKey);

    if (Count >= (INT64)AllocateSize) {
      OldBuffer = Buffer;
      Buffer = ReallocatePool(AllocateSize, AllocateSize + VALUE_SIZE_INCREMENT, NULL);
      AllocateSize += VALUE_SIZE_INCREMENT;
      InitializeArray_8(Buffer, AllocateSize);
      for (i = 0; i < (AllocateSize - VALUE_SIZE_INCREMENT); i++) {
        Buffer[i] = OldBuffer[i];
      }
    }

    if (('0' <= InputKey.UnicodeChar && InputKey.UnicodeChar <= '9')) {
      Buffer[Count] = (CHAR8)InputKey.UnicodeChar - 48;
      Print(L"%x", Buffer[Count]);
      Count++;
      continue;

    } else if (('a' <= InputKey.UnicodeChar && InputKey.UnicodeChar <= 'f')) {
      Buffer[Count] = (CHAR8)InputKey.UnicodeChar - 87;
      Print(L"%x", Buffer[Count]);
      Count++;
      continue;

    } else if (InputKey.UnicodeChar == CHAR_BACKSPACE) {
      Count--;
      if (Count < 0) {
        Count = 0;
      }
      Buffer[Count] = CHAR_NULL;
      Print(L"%c", InputKey.UnicodeChar);
      continue;

    } else if (InputKey.UnicodeChar == CHAR_CARRIAGE_RETURN) {
      for (i = 0; i < (UINT64)Count; i++) {
        ReturnValue += Buffer[i] * Power((UINT64)0x10, (UINT64)(Count-i-1));            
      }
      
      FreePool(Buffer);
      FreePool(OldBuffer);
      return ReturnValue;
    }
  }
}

UINT64*
InputGuid (
  UINT64  *Buffer
  )
{
  EFI_INPUT_KEY        InputKey;
  INT32                WriteCount;

  WriteCount = 0;

  while (TRUE) {
    gST->ConIn->ReadKeyStroke(gST->ConIn, &InputKey);

    if (WriteCount == 32) {
      return Buffer;
    }

    if (('a' <= InputKey.UnicodeChar && InputKey.UnicodeChar <= 'f')) {
      
      Buffer[WriteCount] = InputKey.UnicodeChar - 87;
      Print(L"%c", InputKey.UnicodeChar);
      WriteCount++;
      
      continue;

    } else if (('0' <= InputKey.UnicodeChar && InputKey.UnicodeChar <= '9')) {
      Buffer[WriteCount] = InputKey.UnicodeChar - 48;
      Print(L"%c", InputKey.UnicodeChar);
      WriteCount++;

      continue;
    } else if (InputKey.UnicodeChar == CHAR_BACKSPACE) {

      WriteCount--;
    
      if(WriteCount < 0) {
        WriteCount = 0;
      } 
    
      Buffer[WriteCount] = CHAR_NULL;
      Print(L"%c", InputKey.UnicodeChar);

      continue;
    }
  }
}

EFI_GUID 
ParseGuid (
  UINT64 *Buffer
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
    TestGuid.Data1 |= Buffer[i] << ((8-i-1) * 4);
  }

  for (i = 8; i < 12; i++) {
    TestGuid.Data2 |= Buffer[i] << ((12-i-1) * 4);
  }

  for (i = 12; i < 16; i++) {
    TestGuid.Data3 |= Buffer[i] << ((16-i-1) * 4);
  }

  for (j = 0; j < 8; j++) {
    for (i = 16 + 2*j; i < 18 + 2*j; i++) {
      TestGuid.Data4[j] |= Buffer[i] << (((18+2*j)-i-1) * 4);
    }
  }

  return TestGuid;
}

VOID
InitializeArray_8(
  CHAR8 *Array,
  UINT64 ArraySize
  )
{
  UINT32  i;  // For loop count.

  for (i = 0; i < ArraySize; i++) {
    *(Array + i) = 0;
  }

  return;
}

VOID
InitializeArray_16(
  CHAR16 *Array,
  UINT64 ArraySize
  )
{
  UINT32  i;  // For loop count.

  for (i = 0; i < ArraySize; i++) {
    *(Array + i) = 0;
  }

  return;
}

UINT64 
Power (
  UINT64 Base,
  UINT64 Index
  )
{  
  if (Index == 0) {
    return 1;
  } else {
    return (Base * Power(Base, Index-1));
  }
}

VOID
ShowAttributeTable (
  VOID
  )
{
  gST->ConOut->SetAttribute(gST->ConOut, EFI_YELLOW | EFI_BACKGROUND_BLACK);
  Print(L"\nAttribute Table\n");
  gST->ConOut->SetAttribute(gST->ConOut, EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
  Print(L"NON_VOLATILE                           0x00000001\n");
  Print(L"BOOTSERVICE_ACCESS                     0x00000002\n");
  Print(L"RUNTIME_ACCESS                         0x00000004\n");
  Print(L"HARDWARE_ERROR_RECORD                  0x00000008\n");
  Print(L"AUTHENTICATED_WRITE_ACCESS             0x00000010\n");
  Print(L"TIME_BASED_AUTHENTICATED_WRITE_ACCESS  0x00000020\n");
  Print(L"APPEND_WRITE                           0x00000040\n");
  Print(L"ENHANCED_AUTHENTICATED_ACCESS          0x00000080\n");

  return;
}