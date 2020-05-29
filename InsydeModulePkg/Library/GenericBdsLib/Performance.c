/** @file
  GenericBdsLib

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/**
  This file include the file which can help to get the system
  performance, all the function will only include if the performance
  switch is set.

Copyright (c) 2004 - 2009, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "InternalBdsLib.h"

PERF_HEADER               mPerfHeader;
PERF_DATA                 mPerfData;
EFI_PHYSICAL_ADDRESS      mAcpiLowMemoryBase = 0x0FFFFFFFFULL;
CHAR8                     *mSkipToken[] = {"LoadImage:", "StartImage:", "DB:Support:", "DB:Start:", "\0"};


/**
  Concatenates a formatted unicode string to allocated pool.
  The caller must free the resulting buffer.

  @param  Str      Tracks the allocated pool, size in use, and amount of pool allocated.
  @param  Fmt      The format string
  @param  ...      The data will be printed.

  @return Allocated buffer with the formatted string printed in it.
          The caller must free the allocated buffer.
          The buffer allocation is not packed.

**/
CHAR16 *
EFIAPI
CatPrint (
  IN OUT POOL_PRINT   *Str,
  IN CHAR16           *Fmt,
  ...
  )
;

/**
  Convert Device Path to a Unicode string for printing.

  @param Str             The buffer holding the output string.
                         This buffer contains the length of the
                         string and the maixmum length reserved
                         for the string buffer.
  @param DevPath         The device path.

**/
CHAR16 *
FvFilePath (
  IN OUT POOL_PRINT       *Str,
  IN VOID                 *DevPath
  )
{
  MEDIA_FW_VOL_FILEPATH_DEVICE_PATH *FvFilePath;

  FvFilePath = DevPath;
  return CatPrint (Str, L"%g", &FvFilePath->FvFileName);
}

/**
  Convert Unicode characters to Ascii characters.

  @param  Dest           Destination string
  @param  Src            Source string

  @return	None
**/
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

    if(i > (PERF_TOKEN_SIZE - 2)) break;
  }

  *Dest = 0;
}

/**
  Write PEI file guid name.

  @param  Buffer          Input buffer.
  @param  Guid            File guid name.

  @return	None
**/
VOID
WritePeiDriverGuid (
  IN  CHAR8  *Buffer,
  IN  EFI_GUID  *Guid
  )
{
  UINTN   Index;

  ConvertChar16ToChar8 (Buffer, L"PEIM");

  for (Index = 0; Index < 16; ++Index) {
    *(((UINT8*) Buffer) + (Index + 5)) = *(((UINT8*) Guid) + Index);
  }
}

/**
  Write DXE Driver token.

  @param  Dest            Input buffer.
  @param  Token           Driver token.
  @param  Handle          Driver handle.

  @return	None
**/
VOID
WriteDxeDriverToken (
  IN  CHAR8   *Dest,
  IN  CHAR8   *Token,
  IN  UINT16  *Handle
  )
{
  UINTN   Index;
  CHAR8   *Ptr;

  //
  // Reserved 8 bytes for driver description
  //
  if (AsciiStrLen (Dest) > 20) {
    Ptr = Dest + 20;
    ZeroMem (Ptr, 8);
    Ptr++;
  } else {
    Ptr = Dest + AsciiStrLen (Dest) + 1;
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
  if (AsciiStrCmp (Token, "DB:Start:") == 0) {
    ConvertChar16ToChar8 (Ptr + 1, L"dbst");
  } else if (AsciiStrCmp (Token, "DB:Support:") == 0) {
    ConvertChar16ToChar8 (Ptr + 1, L"dbsu");
  } else if (AsciiStrCmp (Token, "StartImage:") == 0) {
    ConvertChar16ToChar8 (Ptr + 1, L"simg");
  } else if (AsciiStrCmp (Token, "LoadImage:") == 0) {
    ConvertChar16ToChar8 (Ptr + 1, L"limg");
  }
}

/**
  Get the short verion of PDB file name to be
  used in performance data logging.

  @param PdbFileName     The long PDB file name.
  @param GaugeString     The output string to be logged by performance logger.

**/
VOID
GetShortPdbFileName (
  IN  CONST CHAR8  *PdbFileName,
  OUT       CHAR8  *GaugeString
  )
{
  UINTN Index;
  UINTN Index1;
  UINTN StartIndex;
  UINTN EndIndex;

  if (PdbFileName == NULL) {
    AsciiStrCpy (GaugeString, " ");
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
      if (Index1 == PERF_TOKEN_LENGTH - 1) {
        break;
      }
    }

    GaugeString[Index1] = 0;
  }

  return ;
}

/**
  Get the name from the Driver handle, which can be a handle with
  EFI_LOADED_IMAGE_PROTOCOL or EFI_DRIVER_BINDING_PROTOCOL installed.
  This name can be used in performance data logging.

  @param Handle          Driver handle.
  @param GaugeString     The output string to be logged by performance logger.

**/
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

  AsciiStrCpy (GaugeString, " ");
  ZeroMem (&Str, sizeof (Str));
  Tok = NULL;
  //
  // Get handle name from image protocol
  //
  Status = gBS->HandleProtocol (
                  Handle,
                  &gEfiLoadedImageProtocolGuid,
                  (VOID **) &Image
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
                    (VOID **) &Image
                    );
  }

  PdbFileName = PeCoffLoaderGetPdbPointer (Image->ImageBase);

  if (PdbFileName != NULL) {
    GetShortPdbFileName (PdbFileName, GaugeString);
  } else {
    Tok = FvFilePath (&Str,Image->FilePath);
    if (Tok != NULL) {
      ConvertChar16ToChar8 (GaugeString, Tok);
      FreePool (Tok);
    }
  }

  return ;
}

/**

  Allocates a block of memory and writes performance data of booting into it.
  OS can processing these record.

**/
VOID
WriteBootToOsPerformanceData (
  VOID
  )
{
  EFI_STATUS                Status;
  UINT32                    LimitCount;
  EFI_HANDLE                *Handles;
  UINTN                     NoHandles;
  CHAR8                     GaugeString[PERF_TOKEN_LENGTH];
  UINT8                     *Ptr;
  UINT32                    Index;
  UINT64                    Ticker;
  UINT64                    Freq;
  UINT32                    Duration;
  UINTN                     LogEntryKey;
  CONST VOID                *Handle;
  CONST CHAR8               *Token;
  CONST CHAR8               *Module;
  CONST CHAR8               *DriverToken;
  UINT64                    StartTicker;
  UINT64                    EndTicker;
  UINT64                    StartValue;
  UINT64                    EndValue;
  BOOLEAN                   CountUp;
  UINTN                     EntryIndex;
  UINTN                     NumPerfEntries;
  //
  // List of flags indicating PerfEntry contains DXE handle
  //
  BOOLEAN                   *PerfEntriesAsDxeHandle;
  UINTN                     VarSize;

  //
  // Retrieve time stamp count as early as possible
  //
  Ticker  = GetPerformanceCounter ();

  Freq    = GetPerformanceCounterProperties (&StartValue, &EndValue);

  Freq    = DivU64x32 (Freq, 1000);

  ZeroMem (&mPerfHeader, sizeof (PERF_HEADER));
  mPerfHeader.CpuFreq = Freq;

  //
  // Record BDS raw performance data
  //
  if (EndValue >= StartValue) {
    mPerfHeader.BDSRaw = Ticker - StartValue;
    CountUp            = TRUE;
  } else {
    mPerfHeader.BDSRaw = StartValue - Ticker;
    CountUp            = FALSE;
  }

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
	  FreePool (Handles);
      return;
    }
  }


  Ptr        = (UINT8 *) ((UINT32) mAcpiLowMemoryBase + sizeof (PERF_HEADER));
  LimitCount = (UINT32) (PERF_DATA_MAX_LENGTH - sizeof (PERF_HEADER)) / sizeof (PERF_DATA);

  NumPerfEntries = 0;
  LogEntryKey    = 0;
  while ((LogEntryKey = GetPerformanceMeasurement (
                          LogEntryKey,
                          &Handle,
                          &Token,
                          &Module,
                          &StartTicker,
                          &EndTicker)) != 0) {
    NumPerfEntries++;
  }
  PerfEntriesAsDxeHandle = AllocateZeroPool (NumPerfEntries * sizeof (BOOLEAN));
  ASSERT (PerfEntriesAsDxeHandle != NULL);
  if (PerfEntriesAsDxeHandle == NULL) {
    return;
  }

  //
  // Get DXE drivers performance
  //
  for (Index = 0; Index < NoHandles; Index++) {
    LogEntryKey = 0;
    EntryIndex  = 0;
    Token       = NULL;
    DriverToken = NULL;
    while ((LogEntryKey = GetPerformanceMeasurement (
                            LogEntryKey,
                            &Handle,
                            &Token,
                            &Module,
                            &StartTicker,
                            &EndTicker)) != 0) {
      Ticker = 0;
      if (Handle == Handles[Index] && !PerfEntriesAsDxeHandle[EntryIndex]) {
        PerfEntriesAsDxeHandle[EntryIndex] = TRUE;
        DriverToken = Token;
      }
      EntryIndex++;
      if ((Handle == Handles[Index]) && (EndTicker != 0) && DriverToken != NULL) {
        if (StartTicker == 1) {
          StartTicker = StartValue;
        }
        if (EndTicker == 1) {
          EndTicker = StartValue;
        }
        Ticker += CountUp ? (EndTicker - StartTicker) : (StartTicker - EndTicker);

        Duration = (UINT32) DivU64x32 (Ticker, (UINT32) Freq);

        if (Duration > 0) {
          ZeroMem (&mPerfData, sizeof (PERF_DATA));
          ZeroMem (GaugeString, PERF_TOKEN_LENGTH);
          GetNameFromHandle (Handles[Index], GaugeString);

          AsciiStrCpy (mPerfData.Token, GaugeString);
          WriteDxeDriverToken  (mPerfData.Token, (CHAR8 *) DriverToken, (UINT16 *)&Index);
          mPerfData.Duration = Duration;

          CopyMem (Ptr, &mPerfData, sizeof (PERF_DATA));
          Ptr += sizeof (PERF_DATA);

          mPerfHeader.Count++;
          if (mPerfHeader.Count == LimitCount) {
            goto Done;
          }
        }
      }
    }
  }

  //
  // Get inserted performance data
  //
  LogEntryKey = 0;
  EntryIndex  = 0;
  while ((LogEntryKey = GetPerformanceMeasurement (
                          LogEntryKey,
                          &Handle,
                          &Token,
                          &Module,
                          &StartTicker,
                          &EndTicker)) != 0) {
    Index = 0;
    while (AsciiStrCmp (mSkipToken[Index], "\0") != 0) {
      if (AsciiStrCmp (Token, mSkipToken[Index]) == 0) {
        EndTicker = 0;
        break;
      }
      Index++;
    }

    if (!PerfEntriesAsDxeHandle[EntryIndex] && EndTicker != 0) {

      ZeroMem (&mPerfData, sizeof (PERF_DATA));

      if (Handle != NULL){
        WritePeiDriverGuid (mPerfData.Token, (VOID*)Handle);
      } else {
        AsciiStrnCpy (mPerfData.Token, Token, PERF_TOKEN_LENGTH);
      }
      if (StartTicker == 1) {
        StartTicker = StartValue;
      }
      if (EndTicker == 1) {
        EndTicker = StartValue;
      }
      Ticker = CountUp ? (EndTicker - StartTicker) : (StartTicker - EndTicker);

      mPerfData.Duration = (UINT32) DivU64x32 (Ticker, (UINT32) Freq);

      CopyMem (Ptr, &mPerfData, sizeof (PERF_DATA));
      Ptr += sizeof (PERF_DATA);

      mPerfHeader.Count++;
      if (mPerfHeader.Count == LimitCount) {
        goto Done;
      }
    }
    EntryIndex++;
  }

Done:

  FreePool (Handles);
  FreePool (PerfEntriesAsDxeHandle);

  mPerfHeader.Signiture = PERFORMANCE_SIGNATURE;

  //
  // Put performance data to Reserved memory
  //
  CopyMem (
    (UINTN *) (UINTN) mAcpiLowMemoryBase,
    &mPerfHeader,
    sizeof (PERF_HEADER)
    );

  return ;
}
