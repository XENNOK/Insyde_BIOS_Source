/** @file
 Perform the platform memory test

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

Copyright (c) 2004 - 2012, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

*/

#include "Bds.h"
#include "String.h"
#include "OemDataHubSubClassMemory.h"

//
// BDS Platform Functions
//
EFI_STATUS
VerifyMemorySize (
  VOID
  );

EFI_STATUS
GetEfiSysMemSize (
  OUT UINT64                            *MemSize
  );

EFI_STATUS
GetSpdMemSize (
  OUT UINT64                            *MemSize
  );

VOID
FakeMemoryTestFn (
  IN EXTENDMEM_COVERAGE_LEVEL       Level,
  IN UINTN                          BaseLine,
  IN UINT64                         MemSize,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL  Foreground,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL  Background,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL  Color
  );

EFI_STATUS
MemoryTestFn (
  IN EFI_GENERIC_MEMORY_TEST_PROTOCOL  *GenMemoryTest,
  IN BOOLEAN                           RequireSoftECCInit,
  IN BOOLEAN                           DisplayProgress,
  IN UINTN                             BaseLine,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL     Foreground,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL     Background,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL     Color
  );


/**
 Check if system is in text mode or not.

 @retval TRUE       System is in text mode.
 @retval FALSE      System is not in text mode.
**/
BOOLEAN
IsTextMode (
  VOID
  )
{
  EFI_STATUS                   Status;
  KERNEL_CONFIGURATION         SystemConfiguration;
  UINT8                        *DisableQuietBoot;

  Status = GetKernelConfiguration (&SystemConfiguration);
  if (EFI_ERROR (Status) || SystemConfiguration.QuietBoot == 0) {
    return TRUE;
  }

  Status = gBS->LocateProtocol (
                  &gEndOfDisableQuietBootGuid,
                  NULL,
                  (VOID **) &DisableQuietBoot
                  );
  if (!EFI_ERROR (Status)) {
    return TRUE;
  }

  return FALSE;
}


/**
 Perform the memory test base on the memory test intensive level,
 and update the memory resource.

 @param[in] Level         The memory test intensive level.
 @param[in] BaseLine      Row value of base line

 @retval EFI_STATUS       Success test all the system memory and update the memory resource
**/
EFI_STATUS
EFIAPI
BdsMemoryTest (
  IN EXTENDMEM_COVERAGE_LEVEL Level,
  IN UINTN                    BaseLine
  )
{
  EFI_STATUS                        Status;
  BOOLEAN                           RequireSoftECCInit;
  EFI_GENERIC_MEMORY_TEST_PROTOCOL  *GenMemoryTest;
  UINT64                            TotalMemorySize;
  UINT64                            PreviousValue;
  CHAR16                            *StrTotalMemory;
  CHAR16                            *Pos;
  CHAR16                            *TmpStr;
  BOOLEAN                           IsFirstBoot;
  BOOLEAN                           DisplayProgress;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL     Foreground;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL     Background;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL     Color;
  BOOLEAN                           QuietBootEnabled;

  BdsLibGetQuietBootState (&QuietBootEnabled);
  DisplayProgress = TRUE;
  if (Level == IGNORE || (BdsLibGetBootType () == EFI_BOOT_TYPE && QuietBootEnabled)) {
    DisplayProgress = FALSE;
  }

  TotalMemorySize   = 0;
  PreviousValue     = 0;

  SetMem (&Foreground, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL), 0xff);
  SetMem (&Background, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL), 0x0);
  SetMem (&Color     , sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL), 0xff);

  RequireSoftECCInit = FALSE;

  gST->ConOut->SetAttribute (gST->ConOut, EFI_WHITE);
  gST->ConOut->EnableCursor (gST->ConOut, FALSE);

  Status = gBS->LocateProtocol (
                  &gEfiGenericMemTestProtocolGuid,
                  NULL,
                  (VOID **) &GenMemoryTest
                  );
  if (EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }

  Status = GenMemoryTest->MemoryTestInit (
                            GenMemoryTest,
                            Level,
                            &RequireSoftECCInit
                            );
  if (Status == EFI_NO_MEDIA) {
    //
    // The PEI codes also have the relevant memory test code to check the memory,
    // it can select to test some range of the memory or all of them. If PEI code
    // checks all the memory, this BDS memory test will has no not-test memory to
    // do the test, and then the status of EFI_NO_MEDIA will be returned by
    // "MemoryTestInit". So it does not need to test memory again, just return.
    //
    if (DisplayProgress) {
      Status = GetEfiSysMemSize (&TotalMemorySize);
      if (!EFI_ERROR (Status)) {
        FakeMemoryTestFn (Level, BaseLine, TotalMemorySize, Foreground, Background, Color);
      }
    }
  } else {
    Status = MemoryTestFn (
               GenMemoryTest,
               RequireSoftECCInit,
               DisplayProgress,
               BaseLine,
               Foreground,
               Background,
               Color
               );
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  //
  // Check the memory size between memory map(E820) and SPD
  //
  Status = VerifyMemorySize ();
  ASSERT_EFI_ERROR (Status);

  if (DisplayProgress) {
    Pos = AllocatePool (128);
    if (Pos == NULL) {
      return EFI_SUCCESS;
    }

    StrTotalMemory    = Pos;
    GetSpdMemSize (&TotalMemorySize);
    UnicodeValueToString (StrTotalMemory, COMMA_TYPE, TotalMemorySize, 0);
    if (StrTotalMemory[0] == L',') {
      StrTotalMemory++;
    }

    TmpStr = GetStringById (STRING_TOKEN (STR_MEM_TEST_COMPLETED));
    if (TmpStr != NULL) {
      StrCat (StrTotalMemory, TmpStr);
      FreePool (TmpStr);
    }

    if (IsTextMode ()) {
      gST->ConOut->SetAttribute (gST->ConOut, EFI_WHITE);
      gST->ConOut->SetCursorPosition (gST->ConOut, 0, (BaseLine + 2));
      gST->ConOut->EnableCursor (gST->ConOut, FALSE);
      gST->ConOut->OutputString (gST->ConOut, StrTotalMemory);
      gST->ConOut->SetCursorPosition (gST->ConOut, 0, (BaseLine + 3));
    } else {
      BdsLibShowProgress (
        Foreground,
        Background,
        StrTotalMemory,
        Color,
        100,
        (UINTN) PreviousValue
        );
    }
    FreePool (Pos);
  }

  //
  // Use a DynamicHii type pcd to save the boot status, which is used to
  // control configuration mode, such as FULL/MINIMAL/NO_CHANGES configuration.
  //
  IsFirstBoot = PcdGetBool(PcdBootState);
  if (IsFirstBoot) {
    PcdSetBool(PcdBootState, FALSE);
  }

  return EFI_SUCCESS;
}

/**
 Check the memory size between memory map(E820) and SPD

 @retval EFI_SUCCESS     Check memory finish.
 @retval Other           Error return value from get memory function
**/
EFI_STATUS
VerifyMemorySize (
  VOID
  )
{
  EFI_STATUS                            Status;
  UINT64                                MemoryMapSize;
  UINT64                                SPDMemorySize;
  EFI_HANDLE                            Handle;

  Handle = NULL;
  SPDMemorySize = 0;
  MemoryMapSize = 0;

  Status = GetEfiSysMemSize (&MemoryMapSize);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = GetSpdMemSize (&SPDMemorySize);
  if (EFI_ERROR (Status)) {
    return Status;
  }

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


/**
 Get SPD memory size from SMBIOS

 @param[out] MemSize               SPD memory size.

 @retval EFI_SUCCESS               Get system memory size success.
 @retval EFI_INVALID_PARAMETER     Output parameter is NULL.
 @retval Other                     Locate SMBIOS protocol fail.
**/
EFI_STATUS
GetSpdMemSize (
  OUT UINT64                            *MemSize
  )
{
  EFI_STATUS                            Status;
  EFI_SMBIOS_PROTOCOL                   *Smbios;
  EFI_SMBIOS_HANDLE                     SmbiosHandle;
  EFI_SMBIOS_TABLE_HEADER               *Record;
  SMBIOS_TABLE_TYPE17                   *Type17Record;
  UINT64                                Size;
  UINT64                                SPDMemorySize;

  if (MemSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->LocateProtocol (
                  &gEfiSmbiosProtocolGuid,
                  NULL,
                  (VOID **) &Smbios
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Size          = 0;
  SPDMemorySize = 0;
  SmbiosHandle  = SMBIOS_HANDLE_PI_RESERVED;

  do {
    Status = Smbios->GetNext (Smbios, &SmbiosHandle, NULL, &Record, NULL);
    if (EFI_ERROR(Status)) {
      break;
    }

    if (Record->Type == EFI_SMBIOS_TYPE_MEMORY_DEVICE) {
      Type17Record = (SMBIOS_TABLE_TYPE17 *) Record;

      if (Type17Record->Size == 0 || Type17Record->Size == 0xFFFF) {
        continue;
      }

      if (Type17Record->Size < 0x8000) {
        Size = MultU64x32 (Type17Record->Size, MEGABYTE);
      } else {
        Size = MultU64x32 (Type17Record->Size - 0x8000, KILOBYTE);
      }
      SPDMemorySize += Size;
    }
  } while (Status == EFI_SUCCESS);

  *MemSize = SPDMemorySize;

  return EFI_SUCCESS;
}


/**
 Get EFI system memory size.

 @param[out] MemSize     EFI system memory size.

 @retval EFI_SUCCESS               Get system memory size success.
 @retval EFI_INVALID_PARAMETER     Output parameter is NULL.
 @retval Other                     Get system memory map fail.
**/
EFI_STATUS
GetEfiSysMemSize (
  OUT UINT64                            *MemSize
  )
{
  EFI_STATUS                            Status;
  EFI_MEMORY_DESCRIPTOR                 *EfiMemoryMap;
  EFI_MEMORY_DESCRIPTOR                 *EfiMemoryMapEnd;
  EFI_MEMORY_DESCRIPTOR                 *EfiEntry;
  UINTN                                 EfiMemoryMapSize;
  UINTN                                 EfiMapKey;
  UINTN                                 EfiDescriptorSize;
  UINT32                                EfiDescriptorVersion;
  EFI_PEI_HOB_POINTERS                  Hob;
  EFI_HOB_RESOURCE_DESCRIPTOR           *ResourceHob;
  UINT64                                MemoryBlockLength;
  UINT64                                MemoryMapSize;

  if (MemSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }

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
    Status = gBS->AllocatePool (EfiBootServicesData, EfiMemoryMapSize, (VOID **) &EfiMemoryMap);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    Status = gBS->GetMemoryMap (
                    &EfiMemoryMapSize,
                    EfiMemoryMap,
                    &EfiMapKey,
                    &EfiDescriptorSize,
                    &EfiDescriptorVersion
                    );
    if (EFI_ERROR (Status)) {
      gBS->FreePool (EfiMemoryMap);
      EfiMemoryMap = NULL;
    }
  }

  if (EfiMemoryMap == NULL) {
    return EFI_ABORTED;
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
        MemoryMapSize += MemoryBlockLength;
      }
    }
    EfiEntry = NEXT_MEMORY_DESCRIPTOR (EfiEntry, EfiDescriptorSize);
  }

  //
  // Search the reserved memory map
  //
  Hob.Raw = GetHobList ();
  if (Hob.Raw != NULL) {
    for (; !END_OF_HOB_LIST (Hob); Hob.Raw = GET_NEXT_HOB (Hob)) {
      if (GET_HOB_TYPE (Hob) == EFI_HOB_TYPE_RESOURCE_DESCRIPTOR) {
        ResourceHob = Hob.ResourceDescriptor;

        if (ResourceHob->ResourceType == EFI_RESOURCE_MEMORY_RESERVED) {
          if ((ResourceHob->PhysicalStart + ResourceHob->ResourceLength) > 0x100000) {
            MemoryMapSize += ResourceHob->ResourceLength;
          }
        }
      }
    }
  }

  *MemSize = MemoryMapSize;
  gBS->FreePool (EfiMemoryMap);

  return EFI_SUCCESS;
}

/**
 Fake memory test function which only show memory test progress information.

 @param[in] Level           The memory test intensive level.
 @param[in] BaseLine        Row value of base line
 @param[in] MemSize         Memory size.
 @param[in] Foreground      Foreground color for Title.
 @param[in] Background      Background color for Title.
 @param[in] Color           Progress bar color.
**/
VOID
FakeMemoryTestFn (
  IN EXTENDMEM_COVERAGE_LEVEL       Level,
  IN UINTN                          BaseLine,
  IN UINT64                         MemSize,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL  Foreground,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL  Background,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL  Color
  )
{
  CHAR16                            StrPercent[80];
  CHAR16                            *TmpStr;
  UINTN                             TestPercent;
  UINT64                            PreviousValue;
  UINTN                             TestBlockSize;
  UINTN                             TestBlockNum;
  UINTN                             Index;

  PreviousValue = 0;

  if (Level == EXTENSIVE) {
    TestBlockSize = 0x40;
  } else {
    TestBlockSize = 0x200000;
  }
  TestBlockNum = (UINTN) DivU64x32 (MemSize, (UINT32) TestBlockSize);

  for (Index = 0; Index <= TestBlockNum; Index++) {
    TestPercent = (UINTN) DivU64x32 (MultU64x32 (Index, 100), (UINT32) TestBlockNum);

    if (TestPercent == PreviousValue) {
      continue;
    }

    if (IsTextMode ()) {
      UnicodeValueToString (StrPercent, 0, TestPercent, 0);
      gST->ConOut->SetCursorPosition (gST->ConOut, 0, BaseLine);
      TmpStr = GetStringById (STRING_TOKEN (STR_MEMORY_TEST_PERCENT));
      if (TmpStr != NULL) {
        //
        // TmpStr size is 64, StrPercent is reserved to 16.
        //
        BdsLibOutputStrings (gST->ConOut, StrPercent, TmpStr, NULL);
        FreePool (TmpStr);
      }
    } else {
      TmpStr = GetStringById (STRING_TOKEN (STR_PERFORM_MEM_TEST));
      if (TmpStr != NULL) {
        BdsLibShowProgress (
          Foreground,
          Background,
          TmpStr,
          Color,
          TestPercent,
          (UINTN) PreviousValue
          );
        FreePool (TmpStr);
      }
    }
    PreviousValue = TestPercent;
  }
}

/**
 Perform memory test.

 @param[in] GenMemoryTest          Memory test function instance.
 @param[in] RequireSoftECCInit     Indicate if the memory need software ECC init.
 @param[in] DisplayProgress        Flag to decide to show progress or not.
 @param[in] BaseLine               Row value of base line
 @param[in] Foreground             Foreground color for Title.
 @param[in] Background             Background color for Title.
 @param[in] Color                  Progress bar color.

 @retval EFI_SUCCESS               Success to perform memory test.
 @retval EFI_INVALID_PARAMETER     Memory test funtion pointer is NULL.
 @retval Other                     Memory test function return error status.
**/
EFI_STATUS
MemoryTestFn (
  IN EFI_GENERIC_MEMORY_TEST_PROTOCOL  *GenMemoryTest,
  IN BOOLEAN                           RequireSoftECCInit,
  IN BOOLEAN                           DisplayProgress,
  IN UINTN                             BaseLine,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL     Foreground,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL     Background,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL     Color
  )
{
  EFI_STATUS                        Status;
  EFI_STATUS                        KeyStatus;
  CHAR16                            *TmpStr;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *SimpleTextInEx;
  UINT64                            TestedMemorySize;
  UINT64                            TotalMemorySize;
  UINTN                             TestPercent;
  UINT64                            PreviousValue;
  BOOLEAN                           ErrorOut;
  BOOLEAN                           TestAbort;
  UINT32                            TempData;
  CHAR16                            StrPercent[80];
  EFI_INPUT_KEY                     Key;
  EFI_KEY_DATA                      KeyData;

  if (GenMemoryTest == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  TestedMemorySize  = 0;
  TotalMemorySize   = 0;
  PreviousValue     = 0;
  ErrorOut          = FALSE;
  TestAbort         = FALSE;
  SimpleTextInEx    = NULL;

  if (DisplayProgress) {
    if (IsTextMode ()) {
      gST->ConOut->SetCursorPosition (gST->ConOut, 0, (BaseLine + 2));
      TmpStr = GetStringById (STRING_TOKEN (STR_DEL_TO_SKIP_MEM_TEST));
      if (TmpStr != NULL) {
        gST->ConOut->OutputString (gST->ConOut, TmpStr);
        FreePool (TmpStr);
      }
    }

    ZeroMem (&Key, sizeof (EFI_INPUT_KEY));
    gBS->HandleProtocol (
           gST->ConsoleInHandle,
           &gEfiSimpleTextInputExProtocolGuid,
           (VOID **) &SimpleTextInEx
           );
  }

  do {
    Status = GenMemoryTest->PerformMemoryTest (
                              GenMemoryTest,
                              &TestedMemorySize,
                              &TotalMemorySize,
                              &ErrorOut,
                              TestAbort
                              );
    if (ErrorOut && (Status == EFI_DEVICE_ERROR)) {
      if (IsTextMode ()) {
        TmpStr = GetStringById (STRING_TOKEN (STR_SYSTEM_MEM_ERROR));
        if (TmpStr != NULL) {
          PrintXY (10, 10, NULL, NULL, TmpStr);
          gST->ConOut->SetCursorPosition (gST->ConOut, 0, (BaseLine + 4));
          gST->ConOut->OutputString (gST->ConOut, TmpStr);
          FreePool (TmpStr);
        }
      }

      ASSERT (0);
    }

    if (!DisplayProgress) {
      continue;
    }

    TempData = (UINT32) DivU64x32 (TotalMemorySize, 16);
    TestPercent = (UINTN) DivU64x32 (
                            DivU64x32 (MultU64x32 (TestedMemorySize, 100), 16),
                            TempData
                            );
    if (TestPercent != PreviousValue) {
      if (IsTextMode ()) {
        UnicodeValueToString (StrPercent, 0, TestPercent, 0);
        gST->ConOut->SetCursorPosition (gST->ConOut, 0, BaseLine);
        TmpStr = GetStringById (STRING_TOKEN (STR_MEMORY_TEST_PERCENT));
        if (TmpStr != NULL) {
          //
          // TmpStr size is 64, StrPercent is reserved to 16.
          //
          BdsLibOutputStrings (gST->ConOut, StrPercent, TmpStr, NULL);
          FreePool (TmpStr);
        }
      } else {
        TmpStr = GetStringById (STRING_TOKEN (STR_PERFORM_MEM_TEST));
        if (TmpStr != NULL) {
          BdsLibShowProgress (
            Foreground,
            Background,
            TmpStr,
            Color,
            TestPercent,
            (UINTN) PreviousValue
            );
          FreePool (TmpStr);
        }
      }
    }
    PreviousValue = TestPercent;

    if (SimpleTextInEx != NULL) {
      KeyStatus = SimpleTextInEx->ReadKeyStrokeEx (SimpleTextInEx, &KeyData);
      Key = KeyData.Key;
    } else {
      KeyStatus = gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);
    }

    if (!EFI_ERROR (KeyStatus) && (Key.ScanCode == SCAN_DELETE)) {
      if (!RequireSoftECCInit) {
        if (IsTextMode ()) {
          gST->ConOut->SetCursorPosition (gST->ConOut, 0, BaseLine);
          gST->ConOut->OutputString (gST->ConOut, L"100");
          TmpStr = GetStringById (STRING_TOKEN (STR_MEMORY_TEST_PERCENT));
          gST->ConOut->OutputString (gST->ConOut, TmpStr);
          FreePool (TmpStr);
        } else {
          TmpStr = GetStringById (STRING_TOKEN (STR_PERFORM_MEM_TEST));
          if (TmpStr != NULL) {
            BdsLibShowProgress (
              Foreground,
              Background,
              TmpStr,
              Color,
              100,
              (UINTN) PreviousValue
              );
            FreePool (TmpStr);
          }
        }
        break;
      }

      TestAbort = TRUE;
    }
  } while (Status != EFI_NOT_FOUND);

  Status = GenMemoryTest->Finished (GenMemoryTest);

  return Status;
}

