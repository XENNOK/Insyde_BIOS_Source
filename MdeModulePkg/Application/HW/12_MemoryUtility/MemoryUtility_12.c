/** @file
  MemoryUtylity_12 C Source File

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

#include "MemoryUtility_12.h"

CONST POSITION  MainPage_ResetPosition                  = {15, 5};
CONST POSITION  AllocateTypePage_ResetPosition          = {15, 5};
CONST POSITION  MemoryTypePage_ResetPosition            = {15, 5};
CONST POSITION  MemorySettingPage_ResetPosition         = {5, 6};
CONST POSITION  MemorySettingWrite_ResetPosition        = {33, 6};
CONST POSITION  RegistPage_ResetPosition                = {5, 4};
CONST POSITION  ShowAddressAtSettingPage_ResetPosition  = {2, 3};
CONST POSITION  ShowAddressAtRegisterPage_ResetPosition = {2, 0};

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
  UINT8                 Mode;
  BRANCH_PAGE_NUM       AllBranchPage;

  ALLOCATE_TYPE         Type;
  EFI_ALLOCATE_TYPE     EfiAllocateType;
  EFI_MEMORY_TYPE       EfiMemoryType;
  UINTN                 size;
  UINT8                 *Buffer;
  UINT64                Memory;
  UINT8                 **TmpBuffer;
  UINT64                **TmpMemory;

  Mode                  = MAIN_PAGE_MODE;
  AllBranchPage         = MAIN_PAGE_NUM;
  CursorPosition.column = 0;
  CursorPosition.row    = 0;

  Type                  = 0;
  EfiAllocateType       = 0;
  EfiMemoryType         = 0;
  size                  = 0;
  Buffer                = NULL;  
  Memory                = 0;
  TmpBuffer = &Buffer;
  TmpMemory = (UINT64**)&Memory;

  //
  // initialization and into the main page
  //
  gST->ConOut->ClearScreen(gST->ConOut);
  gST->ConOut->SetCursorPosition(gST->ConOut, 0, 0);
  gST->ConOut->EnableCursor(gST->ConOut, TRUE);
  ShowMainPage(&CursorPosition);
  
  //
  // choose mode loop
  //
  while (TRUE) {
    
    //
    // to change mode
    //
    ChangeMode:

    //
    // MAIN_PAGE_MODE
    // with up, down, F1(enter), Esc key response
    //
    while (Mode == MAIN_PAGE_MODE) {
      gST->ConIn->ReadKeyStroke(gST->ConIn, &InputKey);

      if (InputKey.ScanCode == SCAN_ESC) {
        gST->ConOut->ClearScreen(gST->ConOut);
        goto Exit;
      }

      switch (InputKey.ScanCode) {

      case SCAN_UP:
        CursorPosition.row--;
        if (CursorPosition.row < MainPage_ResetPosition.row) {
          CursorPosition.row = MainPage_ResetPosition.row;
        }
        gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);    
        continue;

      case SCAN_DOWN:
        CursorPosition.row++;
        if (CursorPosition.row > (AllBranchPage + MainPage_ResetPosition.row - 1)) {
          CursorPosition.row = (UINT16)(AllBranchPage + MainPage_ResetPosition.row - 1);
        }
        gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
        continue;

      case SCAN_F1:
        Mode = BRANCH_PAGE_MODE;

        if (CursorPosition.row == MainPage_ResetPosition.row) {
          Type = PAGE;
          SetAllocateTypePageAppearance();
          AllBranchPage = ALLOCATE_TYPE_NUM;
          CursorPosition = AllocateTypePage_ResetPosition;
          gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
        } else if (CursorPosition.row == MainPage_ResetPosition.row + 1) {
          Type = POOL;
          SetMemoryTypePageAppearance();
          AllBranchPage = MEMORY_TYPE_NUM;
          CursorPosition = MemoryTypePage_ResetPosition;
          gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
        } 

        goto ChangeMode;
      }
    }

    //
    // BRANCH_PAGE_MODE
    // with up, down, F1(enter), F2(home), Esc key response
    //
    while (Mode == BRANCH_PAGE_MODE) {
      gST->ConIn->ReadKeyStroke(gST->ConIn, &InputKey);

      if (InputKey.ScanCode == SCAN_ESC) {
        gST->ConOut->ClearScreen(gST->ConOut);
        goto Exit;
      }
      
      if (AllBranchPage == ALLOCATE_TYPE_NUM) {
        switch (InputKey.ScanCode) {

        case SCAN_UP:
          CursorPosition.row--;
          if (CursorPosition.row < AllocateTypePage_ResetPosition.row) {
            CursorPosition.row = AllocateTypePage_ResetPosition.row;
          }
          gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);    
          continue;

        case SCAN_DOWN:
          CursorPosition.row++;
          if (CursorPosition.row > (AllBranchPage + AllocateTypePage_ResetPosition.row - 1)) {
            CursorPosition.row = (UINT16)(AllBranchPage + AllocateTypePage_ResetPosition.row - 1);
          }
          gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
          continue;

        case SCAN_F1:
          Mode = BRANCH_PAGE_MODE;

          if (CursorPosition.row == AllocateTypePage_ResetPosition.row) {
            SetMemoryTypePageAppearance();
            EfiAllocateType = AllocateAnyPages;
            AllBranchPage = MEMORY_TYPE_NUM;
            CursorPosition = MemoryTypePage_ResetPosition;
            gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
          } else if (CursorPosition.row == AllocateTypePage_ResetPosition.row + 1) {
            SetMemoryTypePageAppearance();
            EfiAllocateType = AllocateMaxAddress;
            AllBranchPage = MEMORY_TYPE_NUM;
            CursorPosition = MemoryTypePage_ResetPosition;
            gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
          } else if (CursorPosition.row == AllocateTypePage_ResetPosition.row + 2) {
            SetMemoryTypePageAppearance();
            EfiAllocateType = AllocateAddress;
            AllBranchPage = MEMORY_TYPE_NUM;
            CursorPosition = MemoryTypePage_ResetPosition;
            gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
          } 

          goto ChangeMode;
        }
      } else if (AllBranchPage == MEMORY_TYPE_NUM) {
        switch (InputKey.ScanCode) {

        case SCAN_UP:
          CursorPosition.row--;
          if (CursorPosition.row < MemoryTypePage_ResetPosition.row) {
            CursorPosition.row = MemoryTypePage_ResetPosition.row;
          }
          gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);    
          continue;

        case SCAN_DOWN:
          CursorPosition.row++;
          if (CursorPosition.row > (AllBranchPage + MemoryTypePage_ResetPosition.row - 1)) {
            CursorPosition.row = (UINT16)(AllBranchPage + MemoryTypePage_ResetPosition.row - 1);
          }
          gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
          continue;

        case SCAN_F1:
          if ((EfiAllocateType == AllocateAnyPages && Type == PAGE) || Type == POOL) {
            SetGetSizePageAppearance();
            Mode = GET_SIZE_MODE;
            
          } else {
            SetGetAddressPageAppearance();
            Mode = GET_ADDR_MODE;
          }

          if (CursorPosition.row == MemoryTypePage_ResetPosition.row) {
            EfiMemoryType = EfiReservedMemoryType;
          } else if (CursorPosition.row == MemoryTypePage_ResetPosition.row + 1) {
            EfiMemoryType = EfiLoaderCode;
          } else if (CursorPosition.row == MemoryTypePage_ResetPosition.row + 2) {            
            EfiMemoryType = EfiLoaderData;          
          } else if (CursorPosition.row == MemoryTypePage_ResetPosition.row + 3) {            
            EfiMemoryType = EfiBootServicesCode;            
          } else if (CursorPosition.row == MemoryTypePage_ResetPosition.row + 4) {            
            EfiMemoryType = EfiBootServicesData;            
          } else if (CursorPosition.row == MemoryTypePage_ResetPosition.row + 5) {            
            EfiMemoryType = EfiRuntimeServicesCode;            
          } else if (CursorPosition.row == MemoryTypePage_ResetPosition.row + 6) {            
            EfiMemoryType = EfiRuntimeServicesData;            
          } else if (CursorPosition.row == MemoryTypePage_ResetPosition.row + 7) {            
            EfiMemoryType = EfiConventionalMemory;          
          } else if (CursorPosition.row == MemoryTypePage_ResetPosition.row + 8) {            
            EfiMemoryType = EfiUnusableMemory;            
          } else if (CursorPosition.row == MemoryTypePage_ResetPosition.row + 9) {            
            EfiMemoryType = EfiACPIReclaimMemory;            
          } else if (CursorPosition.row == MemoryTypePage_ResetPosition.row + 10) {            
            EfiMemoryType = EfiACPIMemoryNVS;            
          } else if (CursorPosition.row == MemoryTypePage_ResetPosition.row + 11) {            
            EfiMemoryType = EfiMemoryMappedIO;            
          } else if (CursorPosition.row == MemoryTypePage_ResetPosition.row + 12) {            
            EfiMemoryType = EfiMemoryMappedIOPortSpace;            
          } else if (CursorPosition.row == MemoryTypePage_ResetPosition.row + 13) {            
            EfiMemoryType = EfiPalCode;            
          }

          CursorPosition = MemorySettingWrite_ResetPosition;
          gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);

          goto ChangeMode;
        }
      }
      switch (InputKey.ScanCode) {
      
      case SCAN_F2:
        Mode = MAIN_PAGE_MODE; 
        AllBranchPage = MAIN_PAGE_NUM;
        ShowMainPage(&CursorPosition);
        goto ChangeMode;
      }
    }

    //
    // GET_ADDR_MODE
    // with Number(0 ~ 9), Esc key response
    //
    if (Mode == GET_ADDR_MODE) {
      UINT8        WriteCount;
      UINT8        WriteNum;
      UINT16       i;                        // just for loop count
      WRITE_BUFFER WriteBuffer[16];

      WriteCount = 0;
      WriteNum   = 16;

      for (i = 0; i < WriteNum; i++) {
        WriteBuffer[i].value = 0;
        WriteBuffer[i].position = MemorySettingWrite_ResetPosition;
      }

      while (TRUE) {
        gST->ConIn->ReadKeyStroke(gST->ConIn, &InputKey);

        if (InputKey.ScanCode == SCAN_ESC) {
          gST->ConOut->ClearScreen(gST->ConOut);
          goto Exit;
        }
        
        if (WriteCount == WriteNum) {
          Mode = GET_SIZE_MODE;

          //
          // get memory value
          //
          for (WriteCount = 0; WriteCount < WriteNum; WriteCount++) {
            Memory |= WriteBuffer[WriteCount].value << ((WriteNum-WriteCount-1) * 4);
          }

          SetGetSizePageAppearance();
          CursorPosition = MemorySettingWrite_ResetPosition;
          gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);

          goto ChangeMode;
        }

        if (('a' <= InputKey.UnicodeChar && InputKey.UnicodeChar <= 'f')) {
          WriteBuffer[WriteCount].value = (UINT8)InputKey.UnicodeChar - 87;
          WriteBuffer[WriteCount].position.column = CursorPosition.column;
          WriteBuffer[WriteCount].position.row = CursorPosition.row;
          Print(L"%c", InputKey.UnicodeChar);

          CursorPosition.column = CursorPosition.column + 1;
          gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);

          WriteCount++;
          continue;

        } else if (('0' <= InputKey.UnicodeChar && InputKey.UnicodeChar <= '9')) {
          WriteBuffer[WriteCount].value = InputKey.UnicodeChar - 48;
          WriteBuffer[WriteCount].position.column = CursorPosition.column;
          WriteBuffer[WriteCount].position.row = CursorPosition.row;
          Print(L"%c", InputKey.UnicodeChar);
      
          CursorPosition.column = CursorPosition.column + 1;
          gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
          
          WriteCount++;
          continue;
        }
      }
    }

    //
    // GET_SIZE_MODE
    // with Number(0 ~ F), Esc key response
    //
    if (Mode == GET_SIZE_MODE) {
      UINT8        WriteCount;
      UINT8        WriteNum;
      UINT8        i;                       // just for loop count
      WRITE_BUFFER WriteBufferDecimal[4];

      WriteCount = 0;
      WriteNum   = 4;

      for (i = 0; i < WriteNum; i++) {
        WriteBufferDecimal[i].value = 0;
        WriteBufferDecimal[i].position = MemorySettingWrite_ResetPosition;
      }

      while (TRUE) {
        gST->ConIn->ReadKeyStroke(gST->ConIn, &InputKey);

        if (InputKey.ScanCode == SCAN_ESC) {
          gST->ConOut->ClearScreen(gST->ConOut);
          goto Exit;
        }

        if (WriteCount == WriteNum) {
          Mode = WRITE_MODE;

          //
          // get size value
          //
          for (WriteCount = 0; WriteCount < WriteNum; WriteCount++) {
            size += WriteBufferDecimal[WriteCount].value * Power((UINT64)10, (UINT64)(WriteNum-WriteCount-1));            
          }

          //
          // if size is zero operation
          //
          if (size == 0) {
            Mode = GET_SIZE_MODE;
            SetGetSizePageAppearance();
            CursorPosition = MemorySettingWrite_ResetPosition;
            gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
            goto ChangeMode;
          }

          //
          // allocate memory
          //
          if (Type == POOL) {
            Status = gBS->AllocatePool(
                                    EfiMemoryType,
                                    size,
                                    &Buffer   // (VOID**)
                                    );
            if (EFI_ERROR(Status)) {
              Mode = ERROR_MODE;
              gST->ConOut->ClearScreen(gST->ConOut);
              Print(L"%r,Please press F1.\n", Status);
              gST->ConOut->EnableCursor(gST->ConOut, FALSE);
              goto ChangeMode;
            }
          } else if (Type == PAGE) {
            Status = gBS->AllocatePages(
                                    EfiAllocateType,
                                    EfiMemoryType,
                                    size,
                                    &Memory
                                    );
            if (EFI_ERROR(Status)) {
              Mode = ERROR_MODE;
              gST->ConOut->ClearScreen(gST->ConOut);
              Print(L"%r,Please press F1.\n", Status);
              gST->ConOut->EnableCursor(gST->ConOut, FALSE);
              goto ChangeMode;
            }
          }

          SetMemorySettingPageAppearance();
          gST->ConOut->EnableCursor(gST->ConOut, FALSE);
          CursorPosition = ShowAddressAtSettingPage_ResetPosition;
          gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
          if (Type == POOL) {
            Print(L"Address:0x%.8p - 0x%.8p", Buffer, Buffer + size - 1);
          } else if (Type == PAGE) {
            Print(L"Address:0x%.8x - 0x%.8x", Memory, Memory + (size * 4096) - 1);
          }  

          goto ChangeMode;
        }
        
        if (('0' <= InputKey.UnicodeChar && InputKey.UnicodeChar <= '9')) {
          WriteBufferDecimal[WriteCount].value = InputKey.UnicodeChar - 48;
          WriteBufferDecimal[WriteCount].position.column = CursorPosition.column;
          WriteBufferDecimal[WriteCount].position.row = CursorPosition.row;
          Print(L"%c", InputKey.UnicodeChar);
    
          CursorPosition.column = CursorPosition.column + 1;
          gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
          
          WriteCount++;
          
          continue;
        }
      }
    }

    //
    // WRITE_MODE
    // with F1(set memory), F2(reset memory), F3(clear screen), F4(show register), Esc key response
    //
    if (Mode == WRITE_MODE) {
      BOOLEAN      IsWriting;          // TRUE -> is writing
      BOOLEAN      AddressOrValue;     // TRUE -> next write value ; FALSE -> next write address
      BOOLEAN      SetOrReset;         // TRUE -> is set ; FALSE -> is reset
      UINT8        LineCount;
      UINT8        WriteCount;
      UINT8        Value;              // for setting memory
      UINT16       WriteNum;
      UINT16       i;
      UINT64       Address;
      UINT64       **TmpAddress;
      WRITE_BUFFER WriteBuffer[16];

      LineCount      = 0;
      WriteCount     = 0;
      WriteNum       = 0;
      Address        = 0;
      Value          = 0;
      TmpAddress     = (UINT64**)&Address;
      IsWriting      = FALSE;
      AddressOrValue = 0;         
      SetOrReset     = 0;       
      
      for (i = 0; i < 16; i++) {
        WriteBuffer[i].value = 0;
        WriteBuffer[i].position = MemorySettingWrite_ResetPosition;
      }

      while (TRUE) {
        while (IsWriting == FALSE) {

          //
          // when address setting finish, do this judgement
          //
          if (AddressOrValue == TRUE) {  
            IsWriting = !IsWriting;
            WriteNum = 2;
            
            //
            // if larger than 13 row, clear the screen
            //
            if (LineCount == 13) {
              SetMemorySettingPageAppearance();
              gST->ConOut->EnableCursor(gST->ConOut, FALSE);
              CursorPosition = ShowAddressAtSettingPage_ResetPosition;
              gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
              if (Type == POOL) {
                Print(L"Address:0x%.8p - 0x%.8p", Buffer, Buffer + size - 1);
              } else if (Type == PAGE) {
                Print(L"Address:0x%.8x - 0x%.8x", Memory, Memory + (size * 4096) - 1);
              }  
              Mode = WRITE_MODE;
              goto ChangeMode;
            }
            
            CursorPosition = MemorySettingPage_ResetPosition;
            CursorPosition.row = CursorPosition.row + LineCount;
            gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
            Print(L"(1Byte) Set   value  with 0xXX");
            CursorPosition = MemorySettingWrite_ResetPosition;
            CursorPosition.row = CursorPosition.row + LineCount;
            gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
            gST->ConOut->EnableCursor(gST->ConOut, TRUE);

            LineCount++;
            AddressOrValue = !AddressOrValue;

            break;
          }

          gST->ConIn->ReadKeyStroke(gST->ConIn, &InputKey);

          if (InputKey.ScanCode == SCAN_ESC) {
            gBS->FreePages(
                        Memory,
                        size
                        );
            gBS->FreePool(Buffer);
            gST->ConOut->ClearScreen(gST->ConOut);
            goto Exit;
          }

          switch (InputKey.ScanCode) {
          
          //
          // set address and write value
          //
          case SCAN_F1:
            SetOrReset = 0;
            IsWriting = !IsWriting;
            
            if (LineCount == 13) {
              SetMemorySettingPageAppearance();
              gST->ConOut->EnableCursor(gST->ConOut, FALSE);
              CursorPosition = ShowAddressAtSettingPage_ResetPosition;
              gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
              if (Type == POOL) {
                Print(L"Address:0x%.8p - 0x%.8p", Buffer, Buffer + size - 1);
              } else if (Type == PAGE) {
                Print(L"Address:0x%.8x - 0x%.8x", Memory, Memory + (size * 4096) - 1);
              }  
              LineCount = 0;
            }          
            
            CursorPosition = MemorySettingPage_ResetPosition;
            CursorPosition.row = CursorPosition.row + LineCount;
            gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);

            WriteNum = 16;
            Print(L"(64bit) Set  address with 0xXXXXXXXXXXXXXXXX");  

            CursorPosition = MemorySettingWrite_ResetPosition;
            CursorPosition.row = CursorPosition.row + LineCount;
            gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
            gST->ConOut->EnableCursor(gST->ConOut, TRUE);

            LineCount++;
            AddressOrValue = !AddressOrValue;
      
            break;

          //
          // to reset
          //
          case SCAN_F2:
            SetOrReset = 1;
            IsWriting = !IsWriting;
            WriteNum = 2;
            if (LineCount == 13) {
              SetMemorySettingPageAppearance();
              gST->ConOut->EnableCursor(gST->ConOut, FALSE);
              CursorPosition = ShowAddressAtSettingPage_ResetPosition;
              gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
              if (Type == POOL) {
                Print(L"Address:0x%.8p - 0x%.8p", Buffer, Buffer + size - 1);
              } else if (Type == PAGE) {
                Print(L"Address:0x%.8x - 0x%.8x", Memory, Memory + (size * 4096) - 1);
              }  
              LineCount = 0;
            }
            CursorPosition = MemorySettingPage_ResetPosition;
            CursorPosition.row = CursorPosition.row + LineCount;
            gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
            Print(L"(1Byte) Reset memory with 0xXX");
            CursorPosition = MemorySettingWrite_ResetPosition;
            CursorPosition.row = CursorPosition.row + LineCount;
            gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
            gST->ConOut->EnableCursor(gST->ConOut, TRUE);
            LineCount++;
            break;
          //
          // clear screen
          //
          case SCAN_F3:
            SetMemorySettingPageAppearance();
            gST->ConOut->EnableCursor(gST->ConOut, FALSE);
            CursorPosition = ShowAddressAtSettingPage_ResetPosition;
            gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
            if (Type == POOL) {
              Print(L"Address:0x%.8p - 0x%.8p", Buffer, Buffer + size - 1);
            } else if (Type == PAGE) {
              Print(L"Address:0x%.8x - 0x%.8x", Memory, Memory + (size * 4096) - 1);
            }  
            LineCount = 0;
            
            continue;

          //
          // show register
          //
          case SCAN_F4:
            Mode = SHOW_MODE;

            SetRegisterPageAppearance();
            gST->ConOut->EnableCursor(gST->ConOut, FALSE);
            CursorPosition = ShowAddressAtRegisterPage_ResetPosition;
            gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
            if (Type == POOL) {
              Print(L"Address:0x%.8p", Buffer);
            } else if (Type == PAGE) {
              Print(L"Address:0x%.8x", Memory);
            }

            CursorPosition = RegistPage_ResetPosition;
            gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
            gST->ConOut->EnableCursor(gST->ConOut, FALSE);

            if (Type == POOL) {
              for (i = 0; i < 256; i++) {
                if ((i % 16) == 0) {
                  Print(L"%2.2x", *(Buffer + i));
                } else if ((i % 16) == 15) {
                  Print(L"%3.2x", *(Buffer + i));
                  CursorPosition.column = RegistPage_ResetPosition.column;
                  CursorPosition.row++;
                  gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
                } else {
                  Print(L"%3.2x", *(Buffer + i));
                }
              }
            } else if (Type == PAGE) {
              for (i = 0; i < 256; i++) {
                if ((i % 16) == 0) {
                  Print(L"%2.2x", *(UINT8*)(*TmpMemory + i));
                } else if ((i % 16) == 15) {
                  Print(L"%3.2x", *(UINT8*)(*TmpMemory + i));
                  CursorPosition.column = RegistPage_ResetPosition.column;
                  CursorPosition.row++;
                  gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
                } else {
                  Print(L"%3.2x", *(UINT8*)(*TmpMemory + i));
                }
              }
            }

            goto ChangeMode;
          }
          
        }
        
        //
        // for writing
        //
        while (IsWriting == TRUE) {
          gST->ConIn->ReadKeyStroke(gST->ConIn, &InputKey);

          if (InputKey.ScanCode == SCAN_ESC) {
            gBS->FreePages(
                        Memory,
                        size
                        );
            gBS->FreePool(Buffer);
            gST->ConOut->ClearScreen(gST->ConOut);
            goto Exit;
          }

          if (WriteCount == WriteNum) {

            if (SetOrReset == FALSE && AddressOrValue == TRUE) {

              // prepare addr
              for (WriteCount = 0; WriteCount < WriteNum; WriteCount++) {
                Address |= WriteBuffer[WriteCount].value << ((WriteNum-WriteCount-1) * 4);
              }
              
              // judge addr illigle
              if(Address < (UINT64)(*TmpBuffer) || Address > ((UINT64)(*TmpBuffer) + size -1)) {
                SetMemorySettingPageAppearance();
                gST->ConOut->EnableCursor(gST->ConOut, FALSE);
                CursorPosition = ShowAddressAtSettingPage_ResetPosition;
                gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
                if (Type == POOL) {
                  Print(L"Address:0x%.8p - 0x%.8p", Buffer, Buffer + size - 1);
                } else if (Type == PAGE) {
                  Print(L"Address:0x%.8x - 0x%.8x", Memory, Memory + (size * 4096) - 1);
                }
                CursorPosition =  MemorySettingPage_ResetPosition;
                gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
                Print(L"------Invalid range! Please set again.------");
                Print(L"------Please press F3 to clean screen.------");
                
                Mode = WRITE_MODE;

                goto ChangeMode;
              }

            } else if (SetOrReset == FALSE && AddressOrValue == FALSE) {

              // prepare value
              for (WriteCount = 0; WriteCount < WriteNum; WriteCount++) {
                Value |= WriteBuffer[WriteCount].value << ((WriteNum-WriteCount-1) * 4);
              }            

              gBS->SetMem(
                       *((UINT8**)TmpAddress),
                       1,
                       Value
                       );
            
            } else if (SetOrReset == TRUE) { 

              // prepare value
              for (WriteCount = 0; WriteCount < WriteNum; WriteCount++) {
                Value |= WriteBuffer[WriteCount].value << ((WriteNum-WriteCount-1) * 4);
              }              
              
              //
              // set memory
              //
              if (Type == POOL) {
                gBS->SetMem(
                       Buffer,
                       size,
                       Value
                       );
              } else if (Type == PAGE) {
                for (i = 0; i < 4096 * size; i++) {
                  gBS->SetMem(
                           *TmpMemory + i,
                           1,
                           Value
                           );
                }
                
              }
            }
            
            Value = 0;
            WriteCount = 0;
            for (i = 0; i < 16; i++) {
              WriteBuffer[i].value = 0;
              WriteBuffer[i].position = MemorySettingWrite_ResetPosition;
            }
            IsWriting = !IsWriting;
            break;
          }

          if (('a' <= InputKey.UnicodeChar && InputKey.UnicodeChar <= 'f')) {
            WriteBuffer[WriteCount].value = (UINT8)InputKey.UnicodeChar - 87;
            WriteBuffer[WriteCount].position.column = CursorPosition.column;
            WriteBuffer[WriteCount].position.row = CursorPosition.row;
            Print(L"%c", InputKey.UnicodeChar);

            CursorPosition.column = CursorPosition.column + 1;
            gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
            WriteCount++;
            continue;

          } else if (('0' <= InputKey.UnicodeChar && InputKey.UnicodeChar <= '9')) {
            WriteBuffer[WriteCount].value = InputKey.UnicodeChar - 48;
            WriteBuffer[WriteCount].position.column = CursorPosition.column;
            WriteBuffer[WriteCount].position.row = CursorPosition.row;
            Print(L"%c", InputKey.UnicodeChar);
            
            CursorPosition.column = CursorPosition.column + 1;
            gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
            WriteCount++;
            
            continue;
          }
        }
      }
    }
    
    //
    // SHOW_MODE
    // with up, down(turn page), F1(set memory), F2(home), Esc key response
    // 
    if (Mode == SHOW_MODE) {
      UINT64 Page;            // for turn page
      UINT64 i;

      Page = 0;
      while (TRUE)  {
        gST->ConIn->ReadKeyStroke(gST->ConIn, &InputKey);

        if (InputKey.ScanCode == SCAN_ESC) {
          gBS->FreePages(
                      Memory,
                      size
                      );
          gBS->FreePool(Buffer);
          gST->ConOut->ClearScreen(gST->ConOut);
          goto Exit;
        }

        switch (InputKey.ScanCode) {

        case SCAN_UP:
          Page--;

          SetRegisterPageAppearance();
          gST->ConOut->EnableCursor(gST->ConOut, FALSE);
          CursorPosition = ShowAddressAtRegisterPage_ResetPosition;
          gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
          if (Type == POOL) {
            Print(L"Address:0x%.8p", Buffer + Page * 256);
          } else if (Type == PAGE) {
            Print(L"Address:0x%.8x", Memory + Page * 256);
          }
            CursorPosition = RegistPage_ResetPosition;
            gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
            gST->ConOut->EnableCursor(gST->ConOut, FALSE);

            if (Type == POOL) {
              for (i = 0 + Page * 256; i < Page * 256 + 256; i++) {
                if ((i % 16) == 0) {
                  Print(L"%2.2x", *(Buffer + i));
                } else if ((i % 16) == 15) {
                  Print(L"%3.2x", *(Buffer + i));
                  CursorPosition.column = RegistPage_ResetPosition.column;
                  CursorPosition.row++;
                  gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
                } else {
                  Print(L"%3.2x", *(Buffer + i));
                }
              }
            } else if (Type == PAGE) {
              for (i = 0 + Page * 256; i < Page * 256 + 256; i++) {
                if ((i % 16) == 0) {
                  Print(L"%2.2x", *(UINT8*)(*TmpMemory + i));
                } else if ((i % 16) == 15) {
                  Print(L"%3.2x", *(UINT8*)(*TmpMemory + i));
                  CursorPosition.column = RegistPage_ResetPosition.column;
                  CursorPosition.row++;
                  gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
                } else {
                  Print(L"%3.2x", *(UINT8*)(*TmpMemory + i));
                }
              }
            }
              
          continue;

        case SCAN_DOWN:
          Page++;

          SetRegisterPageAppearance();
          gST->ConOut->EnableCursor(gST->ConOut, FALSE);
          CursorPosition = ShowAddressAtRegisterPage_ResetPosition;
          gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
          if (Type == POOL) {
            Print(L"Address:0x%.8p", Buffer + Page * 256);
          } else if (Type == PAGE) {
            Print(L"Address:0x%.8x", Memory + Page * 256);
          }
            CursorPosition = RegistPage_ResetPosition;
            gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
            gST->ConOut->EnableCursor(gST->ConOut, FALSE);

            if (Type == POOL) {
              for (i = 0 + Page * 256; i < Page * 256 + 256; i++) {
                if ((i % 16) == 0) {
                  Print(L"%2.2x", *(Buffer + i));
                } else if ((i % 16) == 15) {
                  Print(L"%3.2x", *(Buffer + i));
                  CursorPosition.column = RegistPage_ResetPosition.column;
                  CursorPosition.row++;
                  gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
                } else {
                  Print(L"%3.2x", *(Buffer + i));
                }
              }
            } else if (Type == PAGE) {
              for (i = 0 + Page * 256; i < Page * 256 + 256; i++) {
                if ((i % 16) == 0) {
                  Print(L"%2.2x", *(UINT8*)(*TmpMemory + i)); // (UINT8*)
                } else if ((i % 16) == 15) {
                  Print(L"%3.2x", *(UINT8*)(*TmpMemory + i));
                  CursorPosition.column = RegistPage_ResetPosition.column;
                  CursorPosition.row++;
                  gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
                } else {
                  Print(L"%3.2x", *(UINT8*)(*TmpMemory + i));
                }
              }
            }
          continue;

        //
        // get back write mode
        //
        case SCAN_F1:
          Mode = WRITE_MODE;
          SetMemorySettingPageAppearance();
          gST->ConOut->EnableCursor(gST->ConOut, FALSE);
          CursorPosition = ShowAddressAtSettingPage_ResetPosition;
          gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
          if (Type == POOL) {
            Print(L"Address:0x%.8p - 0x%.8p", Buffer, Buffer + size - 1);
          } else if (Type == PAGE) {
            Print(L"Address:0x%.8x - 0x%.8x", Memory, Memory + (size * 4096) - 1);
          }  
          goto ChangeMode;

        //
        // get back main page
        //
        case SCAN_F2:
          Mode = MAIN_PAGE_MODE;
          gBS->FreePages(
                      Memory,
                      size
                      );
          gBS->FreePool(Buffer); 
          AllBranchPage = MAIN_PAGE_NUM;
          size = 0;
          ShowMainPage(&CursorPosition);
          gST->ConOut->EnableCursor(gST->ConOut, TRUE);
          goto ChangeMode;
        }
      }
    }
    

    //
    // ERROR_MODE
    // with F1(home) key response
    //
    while (Mode == ERROR_MODE) {
      gST->ConIn->ReadKeyStroke(gST->ConIn, &InputKey);

      switch (InputKey.ScanCode) {
      
      case SCAN_F1:
        Mode = MAIN_PAGE_MODE;
        gBS->FreePages(
                    Memory,
                    size
                    );
        gBS->FreePool(Buffer); 
        AllBranchPage = MAIN_PAGE_NUM;
        size = 0;
        ShowMainPage(&CursorPosition);
        gST->ConOut->EnableCursor(gST->ConOut, TRUE);
        goto ChangeMode;
      }
    }
  }

  //
  // to get out
  //
  Exit:

  Status = EFI_SUCCESS;

  return Status;
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
  Print(L"|                  12-Memory_utility                 |\n");
  Print(L"|                                                    |\n");
  Print(L"|====================================================|\n");
  Print(L"|              Allocate Memory By Page               |\n");
  Print(L"|              Allocate Memory By Pool               |\n");
  Print(L"|====================================================|\n");
  Print(L"| [Arrow key] : Choose                               |\n");
  Print(L"| [F1] : Enter                                       |\n");
  Print(L"| [Esc] : Escape                                     |\n");
  Print(L"|====================================================|\n");

  return;
}

/**
  To set allocate type page appearance.

**/
VOID 
SetAllocateTypePageAppearance (
  VOID
  )
{
  gST->ConOut->ClearScreen(gST->ConOut);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);

  Print(L"|====================================================|\n");
  Print(L"|                                                    |\n");
  Print(L"|                   Allocate Type                    |\n");
  Print(L"|                                                    |\n");
  Print(L"|====================================================|\n");
  Print(L"|              Allocate Any Pages                    |\n");
  Print(L"|              Allocate Max Address                  |\n");
  Print(L"|              Allocate Address                      |\n");
  Print(L"|====================================================|\n");
  Print(L"| [Arrow key] : Choose     [Esc] : Escape            |\n");
  Print(L"| [F1] : Enter             [F2] : Home               |\n");
  Print(L"|====================================================|\n");

  return;
}

/**
  To set memory type page appearance.

**/
VOID 
SetMemoryTypePageAppearance (
  VOID
  )
{
  gST->ConOut->ClearScreen(gST->ConOut);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);

  Print(L"|====================================================|\n");
  Print(L"|                                                    |\n");
  Print(L"|                    Memory Type                     |\n");
  Print(L"|                                                    |\n");
  Print(L"|====================================================|\n");
  Print(L"|              Reserved Memory                       |\n");
  Print(L"|              Loader Code                           |\n");
  Print(L"|              Loader Data                           |\n");
  Print(L"|              Boot Service Code                     |\n");
  Print(L"|              Boot Service Data                     |\n");
  Print(L"|              Runtime Service Code                  |\n");
  Print(L"|              Runtime Service Data                  |\n");
  Print(L"|              Conventional Memory                   |\n");
  Print(L"|              Unusable Memory                       |\n");
  Print(L"|              ACPI Reclaim Memory                   |\n");
  Print(L"|              ACPI NVS Memory                       |\n");
  Print(L"|              Memory Mapped IO                      |\n");
  Print(L"|              Memory Mapped IO Port Space           |\n");
  Print(L"|              PAL Code                              |\n");
  Print(L"|====================================================|\n");
  Print(L"| [Arrow key] : Choose     [Esc] : Escape            |\n");
  Print(L"| [F1] : Enter             [F2] : Home               |\n");
  Print(L"|====================================================|\n");

  return;
}

/**
  To set size getting page appearance.

**/
VOID 
SetGetSizePageAppearance (
  VOID
  )
{
  gST->ConOut->ClearScreen(gST->ConOut);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);

  Print(L"|====================================================|\n");
  Print(L"|                                                    |\n");
  Print(L"|                     Get Size                       |\n");
  Print(L"|                                                    |\n");
  Print(L"|====================================================|\n");
  Print(L"|              Please set size.                      |\n");
  Print(L"|              Set  size  with   XXXX                |\n");
  Print(L"|                                                    |\n");
  Print(L"|====================================================|\n");
  Print(L"| [Num]:Number                                       |\n");
  Print(L"| [Esc]:Escape                                       |\n");
  Print(L"|====================================================|\n");

  return;
}

/**
  To set address getting page appearance.

**/
VOID 
SetGetAddressPageAppearance (
  VOID
  )
{
  gST->ConOut->ClearScreen(gST->ConOut);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);

  Print(L"|====================================================|\n");
  Print(L"|                                                    |\n");
  Print(L"|                     Get Address                    |\n");
  Print(L"|                                                    |\n");
  Print(L"|====================================================|\n");
  Print(L"|            Please set address.                     |\n");
  Print(L"|            Set  address with 0xXXXXXXXXXXXXXXXX    |\n");
  Print(L"|                                                    |\n");
  Print(L"|====================================================|\n");
  Print(L"| [Num]:Number                                       |\n");
  Print(L"| [Esc]:Escape                                       |\n");
  Print(L"|====================================================|\n");

  return;
}

/**
  To set memory setting page appearance.

**/
VOID 
SetMemorySettingPageAppearance (
  VOID
  )
{
  gST->ConOut->ClearScreen(gST->ConOut);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);

  Print(L"|====================================================|\n");
  Print(L"|                                                    |\n");
  Print(L"|                  Memory Setting                    |\n");
  Print(L"|                                                    |\n");
  Print(L"|====================================================|\n");
  Print(L"|           Please press button to modify.           |\n");
  Print(L"|                                                    |\n");
  Print(L"|                                                    |\n");
  Print(L"|                                                    |\n");
  Print(L"|                                                    |\n");
  Print(L"|                                                    |\n");
  Print(L"|                                                    |\n");
  Print(L"|                                                    |\n");
  Print(L"|                                                    |\n");
  Print(L"|                                                    |\n");
  Print(L"|                                                    |\n");
  Print(L"|                                                    |\n");
  Print(L"|                                                    |\n");
  Print(L"|                                                    |\n");
  Print(L"|====================================================|\n");
  Print(L"| [F1]:Set       [F2]:Reset     [F3]:Clean Screen    |\n");
  Print(L"| [F4]:Show      [Esc]:Escape   [Num]:Number         |\n");
  Print(L"|====================================================|\n");

  return;
}

/**
  To set register page appearance.

**/
VOID 
SetRegisterPageAppearance (
  VOID
  )
{
  gST->ConOut->ClearScreen(gST->ConOut);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);

  Print(L"|                                                    |\n");
  Print(L"|====================================================|\n");
  Print(L"|  | 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |\n");
  Print(L"|====================================================|\n");
  Print(L"|00|                                                 |\n");
  Print(L"|10|                                                 |\n");
  Print(L"|20|                                                 |\n");
  Print(L"|30|                                                 |\n");
  Print(L"|40|                                                 |\n");
  Print(L"|50|                                                 |\n");
  Print(L"|60|                                                 |\n");
  Print(L"|70|                                                 |\n");
  Print(L"|80|                                                 |\n");
  Print(L"|90|                                                 |\n");
  Print(L"|A0|                                                 |\n");
  Print(L"|B0|                                                 |\n");
  Print(L"|C0|                                                 |\n");
  Print(L"|D0|                                                 |\n");
  Print(L"|E0|                                                 |\n");
  Print(L"|F0|                                                 |\n");
  Print(L"|====================================================|\n");
  Print(L"| [F1]:Modify            [F2]:Home                   |\n");
  Print(L"| [Arrow key]:Choose     [Esc]:Escape                |\n");
  Print(L"|====================================================|\n");

  return;
}

/**
  To show main page and set cursor position.

  @param  *CursorPosition    The cursor's position.

**/
VOID 
ShowMainPage (
  POSITION *CursorPosition
  )
{
  SetMainPageAppearance();
  *CursorPosition = MainPage_ResetPosition;
  gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition->column, CursorPosition->row);
  gST->ConOut->EnableCursor(gST->ConOut, TRUE);

  return;
}

/**
  To calculate exponential function.

  @param  Base      Base number.
  @param  Index     Index number.

  @retval UINT64    The value of Base to the power of Index.

**/
UINT64 Power (
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