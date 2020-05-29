//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++

Copyright (c) 2004 - 2006, Intel Corporation
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  MemoryTest.c

Abstract:

  Perform the platform memory test

--*/

#include "bds.h"
#include "GenericBdsLib.h"
#include "EfiPrintLib.h"
#include "String.h"

#include EFI_GUID_DEFINITION (BootState)
#include EFI_PROTOCOL_DEFINITION (DataHub)
#include EFI_PROTOCOL_DEFINITION (MemoryErrorEvent)
#include EFI_PROTOCOL_DEFINITION (SimpleTextInputEx)
#include EFI_GUID_DEFINITION (DataHubRecords)
#include EFI_GUID_DEFINITION (Hob)
#include "OemDataHubSubClassMemory.h"

EFI_STATUS
VerifyMemorySize (
  );

//
// BDS Platform Functions
//
EFI_STATUS
PlatformBdsShowProgress (
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL TitleForeground,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL TitleBackground,
  IN CHAR16                        *Title,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL ProgressColor,
  IN UINTN                         Progress,
  IN UINTN                         PreviousValue
  )
/*++

Routine Description:

  Show progress bar with title above it. It only works in Graphics mode.

Arguments:

  TitleForeground - Foreground color for Title.
  TitleBackground - Background color for Title.
  Title           - Title above progress bar.
  ProgressColor   - Progress bar color.
  Progress        - Progress (0-100)

Returns:

  EFI_STATUS      - Success update the progress bar

--*/
{
  EFI_STATUS                     Status;
  EFI_GRAPHICS_OUTPUT_PROTOCOL   *GraphicsOutput;
  EFI_UGA_DRAW_PROTOCOL          *UgaDraw;
  UINT32                         SizeOfX;
  UINT32                         SizeOfY;
  UINT32                         ColorDepth;
  UINT32                         RefreshRate;
  CHAR16                         *PrintStr;
  CHAR16                         *TmpStr;
  CHAR16                         StrPercent[16];
  UINTN                          PosY;


  if (Progress > 100) {
    return EFI_INVALID_PARAMETER;
  }

  UgaDraw = NULL;
  Status = gBS->HandleProtocol (
                  gST->ConsoleOutHandle,
                  &gEfiGraphicsOutputProtocolGuid,
                  &GraphicsOutput
                  );
  if (EFI_ERROR (Status)) {
    GraphicsOutput = NULL;

    Status = gBS->HandleProtocol (
                    gST->ConsoleOutHandle,
                    &gEfiUgaDrawProtocolGuid,
                    &UgaDraw
                    );
    if (EFI_ERROR (Status)) {
      return EFI_UNSUPPORTED;
    }
  }

  if (GraphicsOutput != NULL) {
    SizeOfX = GraphicsOutput->Mode->Info->HorizontalResolution;
    SizeOfY = GraphicsOutput->Mode->Info->VerticalResolution;
  } else {
    Status = UgaDraw->GetMode (
                        UgaDraw,
                        &SizeOfX,
                        &SizeOfY,
                        &ColorDepth,
                        &RefreshRate
                        );
    if (EFI_ERROR (Status)) {
      return EFI_UNSUPPORTED;
    }
  }

  //
  // Remove Progress bar relative code, because Progress bar covers logo
  //
  PosY        = SizeOfY * 48 / 50;

  PrintXY (
    (SizeOfX - EfiStrLen (Title) * GLYPH_WIDTH) / 2,
    PosY - GLYPH_HEIGHT - 1,
    &TitleForeground,
    &TitleBackground,
    Title
    );

  //
  // Show Memory test progress percentage
  //
  TmpStr = GetStringById (STRING_TOKEN (STR_MEMORY_TEST_PERCENT));
  if (TmpStr != NULL) {
    EfiValueToString (StrPercent, Progress, 0, 0);
    EfiStrCat (StrPercent, L"%");
    PrintStr = EfiLibAllocateZeroPool (EfiStrSize (StrPercent) + EfiStrSize (TmpStr));
    EfiCopyMem (PrintStr, StrPercent, EfiStrSize (StrPercent));
    EfiStrCat (PrintStr, TmpStr);
    PrintXY (
      (SizeOfX - EfiStrLen (PrintStr) * GLYPH_WIDTH) / 2,
      PosY - 1,
      &TitleForeground,
      &TitleBackground,
      PrintStr
      );
    gBS->FreePool (TmpStr);
    gBS->FreePool (PrintStr);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
BdsMemoryTest (
  IN EXTENDMEM_COVERAGE_LEVEL Level,
  IN UINTN                    BaseLine
  )
/*++

Routine Description:

  Perform the memory test base on the memory test intensive level,
  and update the memory resource.

Arguments:

  Level  - The memory test intensive level.

Returns:

  EFI_STATUS      - Success test all the system memory and update
                    the memory resource

--*/
{
  EFI_STATUS                        Status;
  EFI_STATUS                        InitStatus;
  EFI_STATUS                        KeyStatus;
  EFI_STATUS                        ReturnStatus;
  BOOLEAN                           RequireSoftECCInit;
  EFI_GENERIC_MEMORY_TEST_PROTOCOL  *GenMemoryTest;
  UINT64                            TestedMemorySize;
  UINT64                            TotalMemorySize;
  UINTN                             TestPercent;
  UINT64                            PreviousValue;
  BOOLEAN                           ErrorOut;
  BOOLEAN                           TestAbort;
  EFI_INPUT_KEY                     Key;
  CHAR16                            StrPercent[16];
  CHAR16                            *StrTotalMemory;
  CHAR16                            *Pos;
  CHAR16                            *TmpStr;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL     Foreground;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL     Background;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL     Color;
  UINT8                             Value;
  UINTN                             DataSize;
  UINT32                            Attributes;
  UINT8                             QuickBootSetting;
#ifdef ENABLE_CONSOLE_EX
  EFI_KEY_DATA                       KeyData;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *SimpleTextInEx = NULL;
#endif
  //
  // Check the memory size between memory map(E820) and SPD
  //
  Status = VerifyMemorySize ();
  ASSERT_EFI_ERROR (Status);
  DataSize = sizeof (QuickBootSetting);
  Status = gRT->GetVariable (
           L"QuickBoot",
           &gEfiGenericVariableGuid,
           NULL,
           &DataSize,
           &QuickBootSetting
           );
  //
  // Only need do memory test when QuickBoot variable exists and QuickBoot is disabled
  //
  if (!(!EFI_ERROR (Status) && (QuickBootSetting == 0))) {
    return EFI_SUCCESS;
  }

  ReturnStatus = EFI_SUCCESS;
  EfiZeroMem (&Key, sizeof (EFI_INPUT_KEY));

  Pos = EfiLibAllocatePool (128);

  if (Pos == NULL) {
    return ReturnStatus;
  }

  StrTotalMemory    = Pos;

  TestedMemorySize  = 0;
  TotalMemorySize   = 0;
  PreviousValue     = 0;
  ErrorOut          = FALSE;
  TestAbort         = FALSE;

  EfiSetMem (&Foreground, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL), 0xff);
  EfiSetMem (&Background, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL), 0x0);
  EfiSetMem (&Color, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL), 0xff);

  RequireSoftECCInit = FALSE;

  gST->ConOut->SetAttribute (gST->ConOut, EFI_WHITE);
  gST->ConOut->EnableCursor (gST->ConOut, FALSE);

  Status = gBS->LocateProtocol (
                  &gEfiGenericMemTestProtocolGuid,
                  NULL,
                  &GenMemoryTest
                  );
  if (EFI_ERROR (Status)) {
    gBS->FreePool (Pos);
    return EFI_SUCCESS;
  }

  InitStatus = GenMemoryTest->MemoryTestInit (
                                GenMemoryTest,
                                Level,
                                &RequireSoftECCInit
                                );
  if (InitStatus == EFI_NO_MEDIA) {
    //
    // The PEI codes also have the relevant memory test code to check the memory,
    // it can select to test some range of the memory or all of them. If PEI code
    // checks all the memory, this BDS memory test will has no not-test memory to
    // do the test, and then the status of EFI_NO_MEDIA will be returned by
    // "MemoryTestInit". So it does not need to test memory again, just return.
    //
    gBS->FreePool (Pos);
    return EFI_SUCCESS;
  }
  gST->ConOut->SetCursorPosition (gST->ConOut, 0, (BaseLine + 2));
  TmpStr = GetStringById (STRING_TOKEN (STR_DEL_TO_SKIP_MEM_TEST));

  if (TmpStr != NULL) {
    gST->ConOut->OutputString (gST->ConOut, TmpStr);
    gBS->FreePool (TmpStr);
  }
#ifdef ENABLE_CONSOLE_EX
  gBS->HandleProtocol (
                 gST->ConsoleInHandle,
                 &gEfiSimpleTextInputExProtocolGuid,
                 &SimpleTextInEx
                 );
#endif

  do {
    Status = GenMemoryTest->PerformMemoryTest (
                              GenMemoryTest,
                              &TestedMemorySize,
                              &TotalMemorySize,
                              &ErrorOut,
                              TestAbort
                              );
    if (ErrorOut && (Status == EFI_DEVICE_ERROR)) {
      TmpStr = GetStringById (STRING_TOKEN (STR_SYSTEM_MEM_ERROR));
      if (TmpStr != NULL) {
        PrintXY (10, 10, NULL, NULL, TmpStr);
        gST->ConOut->SetCursorPosition (gST->ConOut, 0, (BaseLine + 4));
        gST->ConOut->OutputString (gST->ConOut, TmpStr);
        gBS->FreePool (TmpStr);
      }

      ASSERT (0);
    }

    TestPercent = (UINTN) DivU64x32 (
                            DivU64x32 (MultU64x32 (TestedMemorySize, 100), 16, NULL),
                            (UINTN)DivU64x32 (TotalMemorySize, 16, NULL),
                            NULL
                            );
    if (TestPercent != PreviousValue) {
      EfiValueToString (StrPercent, TestPercent, 0, 0);
      gST->ConOut->SetCursorPosition (gST->ConOut, 0, BaseLine);
      TmpStr = GetStringById (STRING_TOKEN (STR_MEMORY_TEST_PERCENT));
      if (TmpStr != NULL) {
        BdsLibOutputStrings (gST->ConOut, StrPercent, TmpStr, NULL);
        gBS->FreePool (TmpStr);
      }

      TmpStr = GetStringById (STRING_TOKEN (STR_PERFORM_MEM_TEST));
      if (TmpStr != NULL) {
        PlatformBdsShowProgress (
          Foreground,
          Background,
          TmpStr,
          Color,
          TestPercent,
          (UINTN) PreviousValue
          );
        gBS->FreePool (TmpStr);
      }
    }

    PreviousValue = TestPercent;
#ifdef ENABLE_CONSOLE_EX
  if (SimpleTextInEx != NULL) {
    KeyStatus = SimpleTextInEx->ReadKeyStrokeEx (SimpleTextInEx, &KeyData);
    Key = KeyData.Key;
  } else {
#endif
    KeyStatus     = gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);
#ifdef ENABLE_CONSOLE_EX
  }
#endif
    if (Key.ScanCode == SCAN_DELETE) {
      if (!RequireSoftECCInit) {
        TmpStr = GetStringById (STRING_TOKEN (STR_PERFORM_MEM_TEST));
        if (TmpStr != NULL) {
          PlatformBdsShowProgress (
            Foreground,
            Background,
            TmpStr,
            Color,
            100,
            (UINTN) PreviousValue
            );
          gBS->FreePool (TmpStr);
        }

        gST->ConOut->SetCursorPosition (gST->ConOut, 0, BaseLine);
        gST->ConOut->OutputString (gST->ConOut, L"100");
        TmpStr = GetStringById (STRING_TOKEN (STR_MEMORY_TEST_PERCENT));
        gST->ConOut->OutputString (gST->ConOut, TmpStr);
        Status = GenMemoryTest->Finished (GenMemoryTest);
        goto Done;
      }

      TestAbort = TRUE;
    }
  } while (Status != EFI_NOT_FOUND);

  Status = GenMemoryTest->Finished (GenMemoryTest);

Done:
  EfiValueToString (StrTotalMemory, TotalMemorySize, COMMA_TYPE, 0);
  if (StrTotalMemory[0] == L',') {
    StrTotalMemory++;
  }

  TmpStr = GetStringById (STRING_TOKEN (STR_MEM_TEST_COMPLETED));
  if (TmpStr != NULL) {
    EfiStrCat (StrTotalMemory, TmpStr);
    gBS->FreePool (TmpStr);
  }

  gST->ConOut->SetAttribute (gST->ConOut, EFI_WHITE);
  gST->ConOut->SetCursorPosition (gST->ConOut, 0, (BaseLine + 2));
  gST->ConOut->EnableCursor (gST->ConOut, FALSE);
  gST->ConOut->OutputString (gST->ConOut, StrTotalMemory);
  gST->ConOut->SetCursorPosition (gST->ConOut, 0, (BaseLine + 3));
  PlatformBdsShowProgress (
    Foreground,
    Background,
    StrTotalMemory,
    Color,
    100,
    (UINTN) PreviousValue
    );

  gBS->FreePool (Pos);

  DataSize = sizeof (Value);
  Status = gRT->GetVariable (
                  L"BootState",
                  &gEfiBootStateGuid,
                  &Attributes,
                  &DataSize,
                  &Value
                  );

  if (EFI_ERROR (Status)) {
    Value = 1;
    gRT->SetVariable (
          L"BootState",
          &gEfiBootStateGuid,
          EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
          sizeof (Value),
          &Value
          );
  }

  return ReturnStatus;
}
EFI_STATUS
VerifyMemorySize (
  )
/*++

Routine Description:

  Check the memory size between memory map(E820) and SPD

Arguments:

  None

Returns:

  EFI_SUCCESS   - Check memory finish.

  Other         - Error return value from get memory function

--*/
{
  EFI_STATUS                            Status;
  EFI_MEMORY_DESCRIPTOR                 *EfiMemoryMap;
  EFI_MEMORY_DESCRIPTOR                 *EfiMemoryMapEnd;
  EFI_MEMORY_DESCRIPTOR                 *EfiEntry;
  UINTN                                 EfiMemoryMapSize;
  UINTN                                 EfiMapKey;
  UINTN                                 EfiDescriptorSize;
  UINT32                                EfiDescriptorVersion;
  VOID                                  *HobList;
  EFI_PEI_HOB_POINTERS                  Hob;
  EFI_HOB_RESOURCE_DESCRIPTOR           *ResourceHob;
  UINT64                                MemoryBlockLength;
  UINT64                                MemoryMapSize = 0;
  UINT64                                MonotonicCount;
  EFI_DATA_HUB_PROTOCOL                 *DataHub;
  EFI_DATA_RECORD_HEADER                *Record;
  EFI_SUBCLASS_TYPE1_HEADER             *DataHeader;
  UINT64                                SPDMemorySize = 0;
  EFI_MEMORY_SUBCLASS_DRIVER_DATA_PLUS  *MemorySubClassDataPlus;
  EFI_HANDLE                            Handle;

  Handle = NULL;
  //
  // Get the EFI memory map.
  //
  EfiMemoryMapSize  = 0;
  EfiMemoryMap      = NULL;
  Status = gBS->GetMemoryMap (
                  &EfiMemoryMapSize,
                  EfiMemoryMap,
                  &EfiMapKey,
                  &EfiDescriptorSize,
                  &EfiDescriptorVersion
                  );
  while (Status == EFI_BUFFER_TOO_SMALL) {

    Status = gBS->AllocatePool (EfiBootServicesData, EfiMemoryMapSize, &EfiMemoryMap);
    ASSERT_EFI_ERROR (Status);
    Status = gBS->GetMemoryMap (
                    &EfiMemoryMapSize,
                    EfiMemoryMap,
                    &EfiMapKey,
                    &EfiDescriptorSize,
                    &EfiDescriptorVersion
                    );
    if (EFI_ERROR (Status)) {
      gBS->FreePool (EfiMemoryMap);
      return Status;
    }
  }


  //
  // Calculate the system memory size from EFI memory map and resourceHob
  //
  EfiEntry        = EfiMemoryMap;
  EfiMemoryMapEnd = (EFI_MEMORY_DESCRIPTOR *) ((UINT8 *) EfiMemoryMap + EfiMemoryMapSize);
  //
  // Default memory is 1MB, due to some platform doesn't save A0000~100000h in memory map or Hob.
  //
  MemoryMapSize   = 0x100000;

  while (EfiEntry < EfiMemoryMapEnd) {
    MemoryBlockLength = (UINT64) (LShiftU64 (EfiEntry->NumberOfPages, 12));
    if ((EfiEntry->PhysicalStart + MemoryBlockLength) > 0x100000) {
      if ((EfiEntry->Type != EfiMemoryMappedIO) &&
          (EfiEntry->Type != EfiMemoryMappedIOPortSpace)) {
           MemoryMapSize = MemoryMapSize + MemoryBlockLength;
      }
    }
    EfiEntry = NEXT_MEMORY_DESCRIPTOR (EfiEntry, EfiDescriptorSize);
  }


  // Search the reserved memory map
  //
  Status = EfiLibGetSystemConfigurationTable (&gEfiHobListGuid, &HobList);
  if (!EFI_ERROR (Status)) {
    for (Hob.Raw = HobList; !END_OF_HOB_LIST (Hob); Hob.Raw = GET_NEXT_HOB (Hob)) {
      if (GET_HOB_TYPE (Hob) == EFI_HOB_TYPE_RESOURCE_DESCRIPTOR) {
        ResourceHob = Hob.ResourceDescriptor;

        if (ResourceHob->ResourceType == EFI_RESOURCE_MEMORY_RESERVED) {
          if ((ResourceHob->PhysicalStart + ResourceHob->ResourceLength) > 0x100000) {
            MemoryMapSize = MemoryMapSize + ResourceHob->ResourceLength;
          }
        }
      }
    }
  }
  gBS->FreePool (EfiMemoryMap);


  //
  // Get SPD memory size from Hub
  //
  Status = gBS->LocateProtocol (
                  &gEfiDataHubProtocolGuid,
                  NULL,
                  &DataHub
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  MonotonicCount  = 0;
  Record = NULL;

  do {
    Status = DataHub->GetNextRecord (DataHub, &MonotonicCount, NULL, &Record);
    if (Record->DataRecordClass == EFI_DATA_RECORD_CLASS_DATA) {
      DataHeader  = (EFI_SUBCLASS_TYPE1_HEADER *)(Record + 1);

      //
      // SPD total Memory Size
      //
      if (EfiCompareGuid (&Record->DataRecordGuid, &gEfiMemorySubClassGuid)) {
        MemorySubClassDataPlus = (EFI_MEMORY_SUBCLASS_DRIVER_DATA_PLUS *) (Record + 1);
        if (MemorySubClassDataPlus->Header.RecordType == EFI_MEMORY_ARRAY_LINK_PLUS_RECORD_NUMBER) {
          SPDMemorySize = SPDMemorySize + MemorySubClassDataPlus->Record.ArrayLink.MemoryArrayLink.MemoryDeviceSize;
          //
          // Follow SMBIOS to support 32GB only.
          //
          SPDMemorySize &= 0x7ffffffff;
        }
      }

    }

  } while (!EFI_ERROR(Status) && (MonotonicCount != 0));


  //
  // Compare memory size between memory map size and SPD size
  //
  if (SPDMemorySize != MemoryMapSize) {

    //
    // Install gMemoryErrorEventGuid protocol when memory information is not consistent with Memory SPD
    //
    Status = gBS->InstallProtocolInterface (
                    &Handle,
                    &gMemoryErrorEventGuid,
                    EFI_NATIVE_INTERFACE,
                    NULL
                    );
  }
  return EFI_SUCCESS;
}
