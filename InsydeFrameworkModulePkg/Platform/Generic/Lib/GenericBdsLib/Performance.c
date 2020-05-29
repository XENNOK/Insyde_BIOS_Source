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

  Performance.c

Abstract:

  This file include the file which can help to get the system
  performance, all the function will only include if the performance
  switch is set.

--*/

#include "Tiano.h"
#include "GenericBdsLib.h"
#include "EfiDriverLib.h"
#include "EfiPrintLib.h"
#ifdef FIRMWARE_PERFORMANCE
#include "EfiImage.h"
#include "Performance.h"

EFI_PHYSICAL_ADDRESS      mAcpiLowMemoryBase = 0x0FFFFFFFFULL;

CHAR16 *
CatPrint (
  IN OUT POOL_PRINT   *Str,
  IN CHAR16           *fmt,
  ...
  );

CHAR16 *
FvFilePathToString (
  IN OUT POOL_PRINT       *Str,
  IN VOID                 *DevPath
  )
{
  MEDIA_FW_VOL_FILEPATH_DEVICE_PATH *FvFilePath;

  FvFilePath = DevPath;
  return CatPrint (Str, L"%g", &FvFilePath->NameGuid);
}

STATIC
VOID
ConvertChar16ToChar8 (
  IN CHAR8      *Dest,
  IN CHAR16     *Src
  )
{
  UINT8 i = 0;

  while (*Src) {
    *Dest++ = (UINT8) (*Src++);
    i++;

    if(i > (EFI_PERF_PDBFILENAME_LENGTH - 2)) break;
  }

  *Dest = 0;
}

VOID
WritePeiDriverGuid (
  OUT CHAR8  *Buffer,
  IN  EFI_GUID  *Guid
  )
{
  UINTN   Index;

  ConvertChar16ToChar8 (Buffer, L"PEIM");

  for (Index = 0; Index < 16; ++Index) {
    *(((UINT8*) Buffer) + (Index + 5)) = *(((UINT8*) Guid) + Index);
  }
}

VOID
UpdatePostBDSToPerformanceData (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  );

VOID
WriteDxeDriverToken (
  OUT CHAR8   *Dest,
  IN  CHAR16  *Token,
  IN  UINT16  *Handle
  )
{
  UINTN   Index;
  UINT8   *Ptr;

  //
  // Reserved 8 bytes for driver description
  //
  if (EfiAsciiStrLen (Dest) > 20) {
    Ptr = Dest + 20;
    EfiZeroMem (Ptr, 8);
    Ptr++;
  } else {
    Ptr = Dest + EfiAsciiStrLen (Dest) + 1;
  }

  //
  // Write driver handle
  //
  for (Index = 0; Index < sizeof (UINT16); ++Index) {
    *Ptr++ = *(((UINT8*) Handle) + Index);
  }
  //
  // Write driver description:
  // DriverBinding:Start   - dbst
  // DriverBinding:Support - dbsu
  // StartImage            - simg
  // LoadImage             - limg
  //
  if (EfiStrCmp (Token, DRIVERBINDING_START_TOK) == 0) {
    ConvertChar16ToChar8 (Ptr + 1, L"dbst");
  } else if (EfiStrCmp (Token, DRIVERBINDING_SUPPORT_TOK) == 0) {
    ConvertChar16ToChar8 (Ptr + 1, L"dbsu");
  } else if (EfiStrCmp (Token, START_IMAGE_TOK) == 0) {
    ConvertChar16ToChar8 (Ptr + 1, L"simg");
  } else if (EfiStrCmp (Token, LOAD_IMAGE_TOK) == 0) {
    ConvertChar16ToChar8 (Ptr + 1, L"limg");
  }

}

STATIC
VOID
GetShortPdbFileName (
  CHAR8  *PdbFileName,
  CHAR8  *GaugeString
  )
/*++

Routine Description:

Arguments:

Returns:

--*/
{
  UINTN Index;
  UINTN Index1;
  UINTN StartIndex;
  UINTN EndIndex;

  if (PdbFileName == NULL) {
    EfiAsciiStrCpy (GaugeString, " ");
  } else {
    StartIndex = 0;
    for (EndIndex = 0; PdbFileName[EndIndex] != 0; EndIndex++)
      ;

    for (Index = 0; PdbFileName[Index] != 0; Index++) {
      if (PdbFileName[Index] == '\\') {
        StartIndex = Index + 1;
      }

      if (PdbFileName[Index] == '.') {
        EndIndex = Index;
      }
    }

    Index1 = 0;
    for (Index = StartIndex; Index < EndIndex; Index++) {
      GaugeString[Index1] = PdbFileName[Index];
      Index1++;
      if (Index1 == EFI_PERF_PDBFILENAME_LENGTH - 1) {
        break;
      }
    }

    GaugeString[Index1] = 0;
  }

  return ;
}

STATIC
CHAR8 *
GetPdbPath (
  VOID *ImageBase
  )
/*++

Routine Description:

  Located PDB path name in PE image

Arguments:

  ImageBase - base of PE to search

Returns:

  Pointer into image at offset of PDB file name if PDB file name is found,
  Otherwise a pointer to an empty string.

--*/
{
  CHAR8                           *PdbPath;
  UINT32                          DirCount;
  EFI_IMAGE_DOS_HEADER            *DosHdr;
  EFI_IMAGE_NT_HEADERS            *NtHdr;
  EFI_IMAGE_OPTIONAL_HEADER       *OptionalHdr;
  EFI_IMAGE_DATA_DIRECTORY        *DirectoryEntry;
  EFI_IMAGE_DEBUG_DIRECTORY_ENTRY *DebugEntry;
  VOID                            *CodeViewEntryPointer;

  CodeViewEntryPointer  = NULL;
  PdbPath               = NULL;
  DosHdr                = ImageBase;

  if (DosHdr->e_magic == EFI_IMAGE_DOS_SIGNATURE) {
    NtHdr           = (EFI_IMAGE_NT_HEADERS *) ((UINT8 *) DosHdr + DosHdr->e_lfanew);
    OptionalHdr     = (VOID *) &NtHdr->OptionalHeader;
    DirectoryEntry  = (EFI_IMAGE_DATA_DIRECTORY *) &(OptionalHdr->DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_DEBUG]);
    if (DirectoryEntry->VirtualAddress != 0) {
      for (DirCount = 0;
           (DirCount < DirectoryEntry->Size / sizeof (EFI_IMAGE_DEBUG_DIRECTORY_ENTRY)) && CodeViewEntryPointer == NULL;
           DirCount++
          ) {
        DebugEntry = (EFI_IMAGE_DEBUG_DIRECTORY_ENTRY *) (DirectoryEntry->VirtualAddress + (UINTN) ImageBase + DirCount * sizeof (EFI_IMAGE_DEBUG_DIRECTORY_ENTRY));
        if (DebugEntry->Type == EFI_IMAGE_DEBUG_TYPE_CODEVIEW) {
          CodeViewEntryPointer = (VOID *) ((UINTN) DebugEntry->RVA + (UINTN) ImageBase);
          switch (*(UINT32 *) CodeViewEntryPointer) {
          case CODEVIEW_SIGNATURE_NB10:
            PdbPath = (CHAR8 *) CodeViewEntryPointer + sizeof (EFI_IMAGE_DEBUG_CODEVIEW_NB10_ENTRY);
            break;

          case CODEVIEW_SIGNATURE_RSDS:
            PdbPath = (CHAR8 *) CodeViewEntryPointer + sizeof (EFI_IMAGE_DEBUG_CODEVIEW_RSDS_ENTRY);
            break;

          default:
            break;
          }
        }
      }
    }
  }

  return PdbPath;
}

STATIC
VOID
GetNameFromHandle (
  IN  EFI_HANDLE     Handle,
  OUT CHAR8          *GaugeString
  )
{
  EFI_STATUS                  Status;
  EFI_LOADED_IMAGE_PROTOCOL   *Image;
  CHAR8                       *PdbFileName;
  EFI_DRIVER_BINDING_PROTOCOL *DriverBinding;
  CHAR16                      *Tok;
  POOL_PRINT                  Str;

  EfiAsciiStrCpy (GaugeString, " ");
  EfiZeroMem (&Str, sizeof (Str));
  Tok = NULL;
  //
  // Get handle name from image protocol
  //
  Status = gBS->HandleProtocol (
                  Handle,
                  &gEfiLoadedImageProtocolGuid,
                  &Image
                  );

  if (EFI_ERROR (Status)) {
    Status = gBS->OpenProtocol (
                    Handle,
                    &gEfiDriverBindingProtocolGuid,
                    (VOID **) &DriverBinding,
                    NULL,
                    NULL,
                    EFI_OPEN_PROTOCOL_GET_PROTOCOL
                    );
    if (EFI_ERROR (Status)) {
      return ;
    }
    //
    // Get handle name from image protocol
    //
    Status = gBS->HandleProtocol (
                    DriverBinding->ImageHandle,
                    &gEfiLoadedImageProtocolGuid,
                    &Image
                    );
  }

  PdbFileName = GetPdbPath (Image->ImageBase);
  if (PdbFileName != NULL) {
    GetShortPdbFileName (PdbFileName, GaugeString);
  } else {
    Tok = FvFilePathToString (&Str,Image->FilePath);
    ConvertChar16ToChar8 (GaugeString, Tok);
    gBS->FreePool (Tok);
  }
  return ;
}

VOID
WriteBootToOsPerformanceData (
  VOID
  )
/*++

Routine Description:

  Allocates a block of memory and writes performance data of booting to OS into it.

Arguments:

  None

Returns:

  None

--*/
{
  EFI_STATUS                Status;
  EFI_CPU_ARCH_PROTOCOL     *Cpu;
  EFI_PERFORMANCE_PROTOCOL  *DrvPerf;
  UINT32                    LimitCount;
  EFI_PERF_HEADER           mPerfHeader;
  EFI_PERF_DATA             mPerfData;
  EFI_GAUGE_DATA            *DumpData;
  EFI_HANDLE                *Handles;
  UINTN                     NoHandles;
  CHAR8                     GaugeString[EFI_PERF_PDBFILENAME_LENGTH];
  CHAR8                     *PdbFileName;
  EFI_EVENT                 LegacyBootEvent;
  UINT8                     *Ptr;
  UINT32                    mIndex;
  UINT64                    Ticker;
  UINT64                    Freq;
  UINT32                    Duration;
  UINT64                    CurrentTicker;
  UINT64                    TimerPeriod;
  UINTN                     VarSize;

  //
  // Retrive time stamp count as early as possilbe
  //
  Ticker = EfiReadTsc ();

  if (mAcpiLowMemoryBase == 0x0FFFFFFFF) {
    VarSize = sizeof (EFI_PHYSICAL_ADDRESS);
    Status = gRT->GetVariable (
                    L"PerfDataMemAddr",
                    &gEfiGenericVariableGuid,
                    NULL,
                    &VarSize,
                    &mAcpiLowMemoryBase
                    );
    if (EFI_ERROR (Status)) {
      //
      // Fail to get the variable, return.
      //
      return;
    }
  }
  Ptr                   = (UINT8 *) ((UINT32) mAcpiLowMemoryBase + sizeof (EFI_PERF_HEADER));
  LimitCount            = (UINT32) (PERF_DATA_MAX_LENGTH - sizeof (EFI_PERF_HEADER)) / sizeof (EFI_PERF_DATA);

  //
  // Get performance architecture protocol
  //
  Status = gBS->LocateProtocol (
                  &gEfiPerformanceProtocolGuid,
                  NULL,
                  &DrvPerf
                  );
  if (EFI_ERROR (Status)) {
    return ;
  }
  //
  // Initialize performance data structure
  //
  EfiZeroMem (&mPerfHeader, sizeof (EFI_PERF_HEADER));

  //
  // Get CPU frequency
  //
  Status = gBS->LocateProtocol (
                  &gEfiCpuArchProtocolGuid,
                  NULL,
                  &Cpu
                  );
  if (EFI_ERROR (Status)) {
    return ;
  }
  //
  // Get Cpu Frequency
  //
  Status = Cpu->GetTimerValue (Cpu, 0, &(CurrentTicker), &TimerPeriod);
  if (EFI_ERROR (Status)) {
    return ;
  }

  Freq                = DivU64x32 (1000000000000, (UINTN) TimerPeriod, NULL);

  mPerfHeader.CpuFreq = Freq;
  LegacyBootEvent = NULL;
  //
  // Record BDS raw performance data
  //
  mPerfHeader.BDSRaw = Ticker;

  //
  // Put Detailed performance data into memory
  //
  Handles = NULL;
  Status = gBS->LocateHandleBuffer (
                  AllHandles,
                  NULL,
                  NULL,
                  &NoHandles,
                  &Handles
                  );
  if (EFI_ERROR (Status)) {
    return ;
  }
  //
  // Get DXE drivers performance
  //
  for (mIndex = 0; mIndex < NoHandles; mIndex++) {
    PdbFileName = NULL;
    DumpData = DrvPerf->GetGauge (
                          DrvPerf,    // Context
                          NULL,       // Handle
                          NULL,       // Token
                          NULL,       // Host
                          NULL        // PrecGauge
                          );
    while (DumpData) {
      Ticker = 0;
      if ((DumpData->Handle == Handles[mIndex])) {
        PdbFileName = &(DumpData->PdbFileName[0]);
        if ((DumpData->StartTick < DumpData->EndTick)) {
          Ticker += (DumpData->EndTick - DumpData->StartTick);
        }
      }
    Duration = (UINT32) DivU64x32 (
                          Ticker,
                          (UINT32) Freq,
                          NULL
                          );

    if (Duration > 0) {
      EfiZeroMem (&mPerfData, sizeof (EFI_PERF_DATA));
      if ((*PdbFileName == ' ') || (*PdbFileName == 0)) {
        EfiZeroMem (GaugeString, EFI_PERF_PDBFILENAME_LENGTH);
        GetNameFromHandle (Handles[mIndex], GaugeString);
        EfiAsciiStrnCpy (PdbFileName, GaugeString, EfiAsciiStrLen(GaugeString));
      }
      if (PdbFileName != NULL) {
        EfiAsciiStrCpy (mPerfData.Token, PdbFileName);
        WriteDxeDriverToken ((UINT8 *) mPerfData.Token, DumpData->Token, (UINT16 *)&mIndex);
      }
      mPerfData.Duration = Duration;

      EfiCopyMem (Ptr, &mPerfData, sizeof (EFI_PERF_DATA));
      Ptr += sizeof (EFI_PERF_DATA);

      mPerfHeader.Count++;
      if (mPerfHeader.Count == LimitCount) {
        goto Done;
      }
    }
      DumpData = DrvPerf->GetGauge (
                        DrvPerf,  // Context
                        NULL,     // Handle
                        NULL,     // Token
                        NULL,     // Host
                        DumpData  // PrecGauge
                        );
    }
  }

  gBS->FreePool (Handles);

  //
  // Get inserted performance data
  //
  DumpData = DrvPerf->GetGauge (
                        DrvPerf,      // Context
                        NULL,         // Handle
                        NULL,         // Token
                        NULL,         // Host
                        NULL          // PrecGauge
                        );
  while (DumpData) {
    if ((DumpData->Handle) || (DumpData->StartTick > DumpData->EndTick)) {
      DumpData = DrvPerf->GetGauge (
                            DrvPerf,  // Context
                            NULL,     // Handle
                            NULL,     // Token
                            NULL,     // Host
                            DumpData  // PrecGauge
                            );
      continue;
    }

    EfiZeroMem (&mPerfData, sizeof (EFI_PERF_DATA));
    if (EfiStrCmp (DumpData->Token, L"PEIM") == 0) {
      WritePeiDriverGuid ((UINT8 *) mPerfData.Token, &DumpData->GuidName);
    } else {
      ConvertChar16ToChar8 ((UINT8 *) mPerfData.Token, DumpData->Token);
    }
    mPerfData.Duration = (UINT32) DivU64x32 (
                                    DumpData->EndTick - DumpData->StartTick,
                                    (UINT32) Freq,
                                    NULL
                                    );

    EfiCopyMem (Ptr, &mPerfData, sizeof (EFI_PERF_DATA));
    Ptr += sizeof (EFI_PERF_DATA);

    mPerfHeader.Count++;
    if (mPerfHeader.Count == LimitCount) {
      goto Done;
    }

    DumpData = DrvPerf->GetGauge (
                          DrvPerf,    // Context
                          NULL,       // Handle
                          NULL,       // Token
                          NULL,       // Host
                          DumpData    // PrecGauge
                          );
  }
  Status = EfiCreateEventLegacyBoot (
             EFI_TPL_CALLBACK,
             UpdatePostBDSToPerformanceData,
             NULL,
             &LegacyBootEvent
             );

  if (EFI_ERROR (Status)) {
    gBS->CloseEvent (LegacyBootEvent);
  }
Done:

  mPerfHeader.Signiture = 0x66726550;

  //
  // Put performance data to memory
  //
  EfiCopyMem (
    (UINTN *) (UINTN) mAcpiLowMemoryBase,
    &mPerfHeader,
    sizeof (EFI_PERF_HEADER)
    );


  return ;
}

VOID
UpdatePostBDSToPerformanceData (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{

  EFI_PERFORMANCE_PROTOCOL  *DrvPerf;
  EFI_PERF_HEADER           *mPerfHeader;
  EFI_PERF_DATA             *mPerfData;
  EFI_GAUGE_DATA            *DumpData;
  EFI_PHYSICAL_ADDRESS      mAcpiLowMemoryBase;
  UINT32                    VarAttr;
  UINTN                     DataSize;
  EFI_STATUS                Status;

  PERF_END (0, L"PostBDS", NULL, 0);
  Status = gBS->LocateProtocol (
                &gEfiPerformanceProtocolGuid,
                NULL,
                &DrvPerf
                );

  if (EFI_ERROR (Status)) {
    return ;
  }

  VarAttr = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS;
  DataSize = sizeof (UINT32);

  Status = gRT->GetVariable (
           L"PerfDataMemAddr",
           &gEfiGenericVariableGuid,
           &VarAttr,
           &DataSize,
           &mAcpiLowMemoryBase
           );

  if (EFI_ERROR (Status)) {
    return ;
  }
  mPerfHeader = (EFI_PERF_HEADER *) (UINTN) mAcpiLowMemoryBase;
  mPerfData = (EFI_PERF_DATA *) (UINTN) (mAcpiLowMemoryBase + sizeof (EFI_PERF_HEADER) + (mPerfHeader->Count * sizeof (EFI_PERF_DATA)));
  EfiZeroMem (mPerfData, sizeof (EFI_PERF_DATA));
  DumpData = DrvPerf->GetGauge (
                        DrvPerf,    // Context
                        NULL,       // Handle
                        L"PostBDS",  // Token
                        NULL,       // Host
                        NULL        // PrecGauge
                        );
  ConvertChar16ToChar8 ((UINT8 *) mPerfData->Token, DumpData->Token);
  mPerfData->Duration = (UINT32) DivU64x32 (
                                  DumpData->EndTick - DumpData->StartTick,
                                  (UINT32) mPerfHeader->CpuFreq,
                                  NULL
                                  );
  mPerfHeader->Count++;

  return;
}
#endif
