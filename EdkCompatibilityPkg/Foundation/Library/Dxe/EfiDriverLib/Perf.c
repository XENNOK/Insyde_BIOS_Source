/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/*++

Copyright (c) 2004 - 2010, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  Perf.c

Abstract:

  Support for Performance primatives.

--*/

#include "Tiano.h"
#include "EfiDriverLib.h"
#include EFI_PROTOCOL_DEFINITION (Performance)
#include EFI_PROTOCOL_DEFINITION (LoadedImage)
#include EFI_GUID_DEFINITION (Hob)
#include EFI_GUID_DEFINITION (PeiPerformanceHob)
#include "LinkedList.h"
#include "EfiHobLib.h"
#include "EfiImage.h"


EFI_GUID mPerformanceProtocolGuid  = { 0x76b6bdfa, 0x2acd, 0x4462, 0x9E, 0x3F, 0xcb, 0x58, 0xC9, 0x69, 0xd9, 0x37 };

#define DXE_PERFORMANCE_STRING_SIZE     32
typedef struct _PERFORMANCE_PROTOCOL PERFORMANCE_PROTOCOL;

typedef struct {
  EFI_PHYSICAL_ADDRESS  Handle;
  CHAR8                 Token[DXE_PERFORMANCE_STRING_SIZE];  ///< Measured token string name.
  CHAR8                 Module[DXE_PERFORMANCE_STRING_SIZE]; ///< Module string name.
  UINT64                StartTimeStamp;                      ///< Start time point.
  UINT64                EndTimeStamp;                        ///< End time point.
} GAUGE_DATA_ENTRY;

/**
 Adds a record at the end of the performance measurement log
 that records the start time of a performance measurement.
 The added record contains the Handle, Token, and Module.
 The end time of the new record is not recorded, so it is set to zero.
 If TimeStamp is not zero, then TimeStamp is used to fill in the start time in the record.
 If TimeStamp is zero, the start time in the record is filled in with the value
 read from the current time stamp.
 
 @param [in]   Handle             The pointer to environment specific context used
                                  to identify the component being measured.
 @param [in]   Token              The pointer to a Null-terminated ASCII string
                                  that identifies the component being measured.
 @param [in]   Module             The pointer to a Null-terminated ASCII string
                                  that identifies the module being measured.
 @param [in]   TimeStamp          The 64-bit time stamp.

 @retval EFI_SUCCESS             The data was read correctly from the device.
 @retval EFI_OUT_OF_RESOURCES    There are not enough resources to record the measurement.

**/
typedef
EFI_STATUS
(EFIAPI * PERFORMANCE_START_GAUGE)(
  IN CONST VOID   *Handle,  OPTIONAL
  IN CONST CHAR8  *Token,   OPTIONAL
  IN CONST CHAR8  *Module,  OPTIONAL
  IN UINT64       TimeStamp
  );

/**
 Searches the performance measurement log from the beginning of the log
 for the first matching record that contains a zero end time and fills in a valid end time.
 Searches the performance measurement log from the beginning of the log
 for the first record that matches Handle, Token, and Module, and has an end time value of zero.
 If the record can not be found then return EFI_NOT_FOUND.
 If the record is found and TimeStamp is not zero,
 then the end time in the record is filled in with the value specified by TimeStamp.
 If the record is found and TimeStamp is zero, then the end time in the matching record
 is filled in with the current time stamp value.
 
 @param [in]   Handle             The pointer to environment specific context used
                                  to identify the component being measured.
 @param [in]   Token              The pointer to a Null-terminated ASCII string
                                  that identifies the component being measured.
 @param [in]   Module             The pointer to a Null-terminated ASCII string
                                  that identifies the module being measured.
 @param [in]   TimeStamp          The 64-bit time stamp.

 @retval EFI_SUCCESS             The end of  the measurement was recorded.
 @retval EFI_NOT_FOUND           The specified measurement record could not be found.
**/
typedef
EFI_STATUS
(EFIAPI * PERFORMANCE_END_GAUGE)(
  IN CONST VOID   *Handle,  OPTIONAL
  IN CONST CHAR8  *Token,   OPTIONAL
  IN CONST CHAR8  *Module,  OPTIONAL
  IN UINT64       TimeStamp
  );

/**
 Retrieves a previously logged performance measurement.
 Retrieves the performance log entry from the performance log specified by LogEntryKey.
 If it stands for a valid entry, then EFI_SUCCESS is returned and
 GaugeDataEntry stores the pointer to that entry.
 
 @param [in]   LogEntryKey        The key for the previous performance measurement log entry.
                                  If 0, then the first performance measurement log entry is retrieved.
 @param [out]  GaugeDataEntry     Out parameter for the indirect pointer to the gauge data entry specified by LogEntryKey.
 
 @retval EFI_SUCCESS             The GuageDataEntry is successfully found based on LogEntryKey.
 @retval EFI_NOT_FOUND           There is no entry after the measurement referred to by LogEntryKey.
 @retval EFI_INVALID_PARAMETER   The LogEntryKey is not a valid entry, or GaugeDataEntry is NULL.

**/
typedef
EFI_STATUS
(EFIAPI * PERFORMANCE_GET_GAUGE)(
  IN  UINTN               LogEntryKey,
  OUT GAUGE_DATA_ENTRY    **GaugeDataEntry
  );

struct _PERFORMANCE_PROTOCOL {
  PERFORMANCE_START_GAUGE             StartGauge;
  PERFORMANCE_END_GAUGE               EndGauge;
  PERFORMANCE_GET_GAUGE               GetGauge;
};

PERFORMANCE_PROTOCOL  *mPerformance;

EFI_STATUS
GetTimerValue (
  OUT UINT64    *TimerValue
  );

EFI_STATUS
GetPeiPerformance (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable,
  IN UINT64               Ticker
  );

#define EFI_PERFORMANCE_DATA_SIGNATURE  EFI_SIGNATURE_32 ('P', 'E', 'D', 'A')

typedef struct {
  UINT32          Signature;
  EFI_LIST_ENTRY  Link;
  EFI_GAUGE_DATA  GaugeData;
} EFI_PERF_DATA_LIST;

#define GAUGE_DATA_FROM_LINK(_link)  \
            CR(_link, EFI_PERF_DATA_LIST, Link, EFI_PERFORMANCE_DATA_SIGNATURE)

#define GAUGE_DATA_FROM_GAUGE(_GaugeData)  \
            CR(_GaugeData, EFI_PERF_DATA_LIST, GaugeData, EFI_PERFORMANCE_DATA_SIGNATURE)

#define EFI_PERFORMANCE_SIGNATURE         EFI_SIGNATURE_32 ('P', 'E', 'R', 'F')

//
// Performance protocol instance data structure
//
typedef struct {
  UINTN                     Signature;
  EFI_HANDLE                Handle;
  EFI_PERFORMANCE_PROTOCOL  Perf;
  UINT8                     Phase;
} EFI_PERFORMANCE_INSTANCE;

//
// Performace protocol instance containing record macro
//

#define EFI_PERFORMANCE_FROM_THIS(a) \
  CR(a, EFI_PERFORMANCE_INSTANCE, Perf, EFI_PERFORMANCE_SIGNATURE)

EFI_LIST_ENTRY  mPerfDataHead = INITIALIZE_LIST_HEAD_VARIABLE(mPerfDataHead);

/**

 @param        PdbFileName
 @param        GaugeString


**/
STATIC
VOID
GetShortPdbFileName (
  CHAR8  *PdbFileName,
  CHAR8  *GaugeString
  )
{
  UINTN Index;
  UINTN Index1;
  UINTN StartIndex;
  UINTN EndIndex;

  if (PdbFileName == NULL) {
    EfiAsciiStrCpy (GaugeString, (CHAR8 *)" ");
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

/**
 Located PDB path name in PE image

 @param        ImageBase        base of PE to search

 @return   Pointer into image at offset of PDB file name if PDB file name is found,
           Otherwise a pointer to an empty string.

**/
STATIC
CHAR8 *
GetPdbPath (
  VOID *ImageBase
  )
{
  CHAR8                           *PdbPath;
  UINT32                          DirCount;
  EFI_IMAGE_DOS_HEADER            *DosHdr;
  EFI_IMAGE_NT_HEADERS            *NtHdr;
  UINT16                          Magic;
  EFI_IMAGE_OPTIONAL_HEADER32     *OptionalHdr32;
  EFI_IMAGE_OPTIONAL_HEADER64     *OptionalHdr64;
  EFI_IMAGE_DATA_DIRECTORY        *DirectoryEntry;
  EFI_IMAGE_DEBUG_DIRECTORY_ENTRY *DebugEntry;
  VOID                            *CodeViewEntryPointer;

  CodeViewEntryPointer  = NULL;
  PdbPath               = NULL;
  DosHdr                = ImageBase;
  if (DosHdr && DosHdr->e_magic == EFI_IMAGE_DOS_SIGNATURE) {
    NtHdr           = (EFI_IMAGE_NT_HEADERS *) ((UINT8 *) DosHdr + DosHdr->e_lfanew);
    //
    // NOTE: We use Machine to identify PE32/PE32+, instead of Magic.
    //       It is for backward-compatibility consideration, because
    //       some system will generate PE32+ image with PE32 Magic.
    //
    if (NtHdr->FileHeader.Machine == EFI_IMAGE_MACHINE_IA32) {
      Magic = EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC;
    } else if (NtHdr->FileHeader.Machine == EFI_IMAGE_MACHINE_IA64) {
      Magic = EFI_IMAGE_NT_OPTIONAL_HDR64_MAGIC;
    } else if (NtHdr->FileHeader.Machine == EFI_IMAGE_MACHINE_X64) {
      Magic = EFI_IMAGE_NT_OPTIONAL_HDR64_MAGIC;
    } else {
      Magic = NtHdr->OptionalHeader.Magic;
    }
    if (Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
      OptionalHdr32 = (VOID *) &NtHdr->OptionalHeader;
      DirectoryEntry  = (EFI_IMAGE_DATA_DIRECTORY *) &(OptionalHdr32->DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_DEBUG]);
    } else {
      OptionalHdr64 = (VOID *) &NtHdr->OptionalHeader;
      DirectoryEntry  = (EFI_IMAGE_DATA_DIRECTORY *) &(OptionalHdr64->DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_DEBUG]);
    }

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

  EfiAsciiStrCpy (GaugeString, (CHAR8 *)" ");

  //
  // Get handle name from image protocol
  //
  Status = gBS->HandleProtocol (
                  Handle,
                  &gEfiLoadedImageProtocolGuid,
                  (VOID**)&Image
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
                    (VOID**)&Image
                    );
  }

  PdbFileName = GetPdbPath (Image->ImageBase);

  if (PdbFileName != NULL) {
    GetShortPdbFileName (PdbFileName, GaugeString);
  }

  return ;
}

/**
 Create a EFI_PERF_DATA_LIST data node.

 @param [in]   Handle           Handle of gauge data
 @param [in]   Token            Token of gauge data
 @param [in]   Host             Host of gauge data

 @return Pointer to a data node created.

**/
EFI_PERF_DATA_LIST *
CreateDataNode (
  IN EFI_HANDLE       Handle,
  IN UINT16           *Token,
  IN UINT16           *Host
  )
{
  EFI_PERF_DATA_LIST  *Node;

  //
  // Al\ a new image structure
  //
  Node = EfiLibAllocateZeroPool (sizeof (EFI_PERF_DATA_LIST));
  if (Node != NULL) {

    Node->Signature         = EFI_PERFORMANCE_DATA_SIGNATURE;

    Node->GaugeData.Handle  = Handle;

    if (Token != NULL) {
      EfiStrCpy ((Node->GaugeData).Token, Token);
    }

    if (Host != NULL) {
      EfiStrCpy ((Node->GaugeData).Host, Host);
    }

    if (Handle != NULL) {
      GetNameFromHandle (Handle, Node->GaugeData.PdbFileName);
    }
  }

  return Node;
}


/**
 Search gauge node list to find one node with matched handle, token, host and Guid name.

 @param [in]   Handle           Handle to match
 @param [in]   Token            Token to match
 @param [in]   Host             Host to match
 @param [in]   GuidName         Guid name to match
 @param [in]   PrevGauge        Start node, start from list head if NULL

 @return Return pointer to the node found, NULL if not found.

**/
EFI_PERF_DATA_LIST *
GetDataNode (
  IN EFI_HANDLE        Handle,
  IN UINT16            *Token,
  IN UINT16            *Host,
  IN EFI_GUID          *GuidName,
  IN EFI_GAUGE_DATA    *PrevGauge
  )
{
  EFI_PERF_DATA_LIST  *Node;
  EFI_PERF_DATA_LIST  *Temp;
  EFI_PERF_DATA_LIST  *Temp2;
  EFI_LIST_ENTRY      *CurrentLink;
  EFI_GUID            NullGuid = EFI_NULL_GUID;

  Node      = NULL;
  Temp      = NULL;
  Temp2     = NULL;

  if (PrevGauge == NULL) {
    CurrentLink = mPerfDataHead.ForwardLink;
  } else {
    Temp2       = GAUGE_DATA_FROM_GAUGE (PrevGauge);
    CurrentLink = (Temp2->Link).ForwardLink;
  }

  while (CurrentLink && CurrentLink != &mPerfDataHead) {
    Node = GAUGE_DATA_FROM_LINK (CurrentLink);

    if (Handle == 0 && Token == NULL && Host == NULL && GuidName == NULL) {
      return Node;
    }

    if (Handle != (Node->GaugeData).Handle) {
      CurrentLink = CurrentLink->ForwardLink;
      continue;
    }

    if (GuidName == NULL && !EfiCompareGuid (&((Node->GaugeData).GuidName), &NullGuid)) {
      CurrentLink = CurrentLink->ForwardLink;
      continue;
    }

    if (GuidName && !EfiCompareGuid (&((Node->GaugeData).GuidName), GuidName)) {
      CurrentLink = CurrentLink->ForwardLink;
      continue;
    }

    if (Token == NULL && EfiStrCmp (Node->GaugeData.Token, L"")) {
      CurrentLink = CurrentLink->ForwardLink;
      continue;
    }

    if (Token && EfiStrCmp (Node->GaugeData.Token, Token)) {
      CurrentLink = CurrentLink->ForwardLink;
      continue;
    }

    if (Host == NULL && EfiStrCmp (Node->GaugeData.Host, L"")) {
      CurrentLink = CurrentLink->ForwardLink;
      continue;
    }

    if (Host && EfiStrCmp (Node->GaugeData.Host, Host)) {
      CurrentLink = CurrentLink->ForwardLink;
      continue;
    }

    Temp = Node;
    break;
  }

  return Temp;
}


/**
 Create a guage data node and initialized it.

 @param [in]   This             Calling context
 @param [in]   Handle           Handle of gauge data
 @param [in]   Token            Token of gauge data
 @param [in]   Host             Host of gauge data
 @param [in]   Ticker           Set gauge data's StartTick. If 0, StartTick is current timer.

 @retval EFI_SUCCESS            Successfully create and initialized a guage data node.
 @retval EFI_OUT_OF_RESOURCES   No enough resource to create a guage data node.

**/
EFI_STATUS
EFIAPI
StartGauge (
  IN EFI_PERFORMANCE_PROTOCOL         *This,
  IN EFI_HANDLE                       Handle,
  IN UINT16                           *Token,
  IN UINT16                           *Host,
  IN UINT64                           Ticker
  )
{
  EFI_PERFORMANCE_INSTANCE  *PerfInstance;
  EFI_PERF_DATA_LIST        *Node;
  UINT64                    TimerValue;

  TimerValue    = 0;
  PerfInstance  = EFI_PERFORMANCE_FROM_THIS (This);

  Node          = CreateDataNode (Handle, Token, Host);
  if (!Node) {
    return EFI_OUT_OF_RESOURCES;
  }

  if (Ticker != 0) {
    TimerValue = Ticker;
  } else {
    GetTimerValue (&TimerValue);
  }

  Node->GaugeData.StartTick = TimerValue;

  if (!EfiStrCmp (Token, DXE_TOK)) {
    PerfInstance->Phase = DXE_PHASE;
  }

  if (!EfiStrCmp (Token, SHELL_TOK)) {
    PerfInstance->Phase = SHELL_PHASE;
  }

  Node->GaugeData.Phase = PerfInstance->Phase;

  InsertTailList (&mPerfDataHead, &(Node->Link));

  return EFI_SUCCESS;
}


/**
 End all unfinished gauge data node that match specified handle, token and host.

 @param [in]   This             Calling context
 @param [in]   Handle           Handle to stop
 @param [in]   Token            Token to stop
 @param [in]   Host             Host to stop
 @param [in]   Ticker           End tick, if 0 then get current timer

 @retval EFI_NOT_FOUND          Node not found
 @retval EFI_SUCCESS            Gauge data node successfully ended.

**/
EFI_STATUS
EFIAPI
EndGauge (
  IN EFI_PERFORMANCE_PROTOCOL         *This,
  IN EFI_HANDLE                       Handle,
  IN UINT16                           *Token,
  IN UINT16                           *Host,
  IN UINT64                           Ticker
  )
{
  EFI_PERFORMANCE_INSTANCE  *PerfInstance;
  EFI_PERF_DATA_LIST        *Node;
  UINT64                    TimerValue;

  TimerValue    = 0;
  PerfInstance  = EFI_PERFORMANCE_FROM_THIS (This);

  Node          = GetDataNode (Handle, Token, Host, NULL, NULL);
  if (!Node) {
    return EFI_NOT_FOUND;
  }

  while (Node->GaugeData.EndTick != 0) {
    Node = GetDataNode (Handle, Token, Host, NULL, &(Node->GaugeData));
    if (!Node) {
      return EFI_NOT_FOUND;
    }
  }

  if (Ticker != 0) {
    TimerValue = Ticker;
  } else {
    GetTimerValue (&TimerValue);
  }

  Node->GaugeData.EndTick = TimerValue;

  return EFI_SUCCESS;
}


/**
 Get gauge.

 @param [in]   This             A pointer to the EFI_PERFORMANCE_PROTOCOL.
 @param [in]   Handle           A pointer of a efi handle.
 @param [in]   Token            A pointer to the token.
 @param [in]   Host             A pointer to the host.
 @param [in]   PrevGauge        A pointer to the EFI_GAUGE_DATA structure.

 @return Status code.

**/
EFI_GAUGE_DATA *
EFIAPI
GetGauge (
  IN EFI_PERFORMANCE_PROTOCOL         *This,
  IN EFI_HANDLE                       Handle,
  IN UINT16                           *Token,
  IN UINT16                           *Host,
  IN EFI_GAUGE_DATA                   *PrevGauge
  )
{
  EFI_PERFORMANCE_INSTANCE  *PerfInstance;
  EFI_PERF_DATA_LIST        *Node;

  PerfInstance  = EFI_PERFORMANCE_FROM_THIS (This);

  Node          = GetDataNode (Handle, Token, Host, NULL, PrevGauge);
  if (Node != NULL) {
    return &(Node->GaugeData);
  } else {
    return NULL;
  }
}

//
// Driver entry point
//
/**
 Install gEfiPerformanceProtocolGuid protocol and transfer PEI performance to gauge data nodes.

 @param [in]   ImageHandle      Standard driver entry point parameter
 @param [in]   SystemTable      Standard driver entry point parameter
 @param [in]   Ticker           End tick for PEI performance

 @retval EFI_OUT_OF_RESOURCES   No enough buffer to allocate
 @retval EFI_SUCCESS            Protocol installed.

**/
EFI_STATUS
EFIAPI
InitializePerformanceInfrastructure (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable,
  IN UINT64               Ticker
  )
{
  EFI_STATUS                Status;
  EFI_PERFORMANCE_INSTANCE  *PerfInstance;

  //
  // Allocate a new image structure
  //
  PerfInstance = EfiLibAllocateZeroPool (sizeof (EFI_PERFORMANCE_INSTANCE));
  if (PerfInstance == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  PerfInstance->Signature       = EFI_PERFORMANCE_SIGNATURE;
  PerfInstance->Perf.StartGauge = StartGauge;
  PerfInstance->Perf.EndGauge   = EndGauge;
  PerfInstance->Perf.GetGauge   = GetGauge;

  //
  // Install the protocol interfaces
  //
  Status = gBS->InstallProtocolInterface (
                  &PerfInstance->Handle,
                  &gEfiPerformanceProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &PerfInstance->Perf
                  );

  if (!EFI_ERROR (Status)) {
    GetPeiPerformance (ImageHandle, SystemTable, Ticker);
  }

  return EFI_SUCCESS;
}


/**
 Start to gauge on a specified handle, token and host, with Ticker as start tick.

 @param        Handle           Handle to measure
 @param [in]   Token            Token to measure
 @param [in]   Host             Host to measure
 @param [in]   Ticker           Ticker as start tick

 @return Status code.

**/
EFI_STATUS
EFIAPI
StartMeasure (
  EFI_HANDLE          Handle,
  IN UINT16           *Token,
  IN UINT16           *Host,
  IN UINT64           Ticker
  )
{
  EFI_STATUS                Status;
  EFI_PERFORMANCE_PROTOCOL  *Perf;

  Status = gBS->LocateProtocol (&gEfiPerformanceProtocolGuid, NULL, (VOID **) &Perf);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return Perf->StartGauge (Perf, Handle, Token, Host, Ticker);

}


/**
 End gauging on a specified handle, token and host, with Ticker as end tick.

 @param        Handle           Handle to stop
 @param [in]   Token            Token to stop
 @param [in]   Host             Host to stop
 @param [in]   Ticker           Ticker as end tick

 @return Status code.

**/
EFI_STATUS
EFIAPI
EndMeasure (
  EFI_HANDLE          Handle,
  IN UINT16           *Token,
  IN UINT16           *Host,
  IN UINT64           Ticker
  )
{
  EFI_STATUS                Status;
  EFI_PERFORMANCE_PROTOCOL  *Perf;

  Status = gBS->LocateProtocol (&gEfiPerformanceProtocolGuid, NULL, (VOID **) &Perf);
  if (Status != EFI_SUCCESS) {
    return Status;
  }

  return (Perf->EndGauge( Perf, Handle, Token, Host, Ticker)) ;
}


/**
 Update measure.

 @param        Handle           A pointer of an efi handle.
 @param [in]   Token            A pointer to the token.
 @param [in]   Host             A pointer to the host.
 @param        HandleNew        A pointer of an new efi handle.
 @param [in]   TokenNew         A pointer to the new token.
 @param [in]   HostNew          A pointer to the new host.

 @return Status code.
 @retval EFI_NOT_FOUND          The speicified gauge data node not found.
 @retval EFI_SUCCESS            Update successfully.

**/
EFI_STATUS
EFIAPI
UpdateMeasure (
  EFI_HANDLE         Handle,
  IN UINT16          *Token,
  IN UINT16          *Host,
  EFI_HANDLE         HandleNew,
  IN UINT16          *TokenNew,
  IN UINT16          *HostNew
  )
{
  EFI_STATUS                Status;
  EFI_GAUGE_DATA            *GaugeData;
  EFI_PERFORMANCE_PROTOCOL  *Perf;

  Status = gBS->LocateProtocol (&gEfiPerformanceProtocolGuid, NULL, (VOID **) &Perf);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  GaugeData = Perf->GetGauge (Perf, Handle, Token, Host, NULL);
  if (!GaugeData) {
    return EFI_NOT_FOUND;
  }

  GaugeData->Handle = HandleNew;
  if (HostNew != NULL) {
    EfiStrCpy (GaugeData->Host, HostNew);
  } else {
    EfiStrCpy (GaugeData->Host, L"");
  }

  if (TokenNew != NULL) {
    EfiStrCpy (GaugeData->Token, TokenNew);
  } else {
    EfiStrCpy (GaugeData->Token, L"");
  }

  return EFI_SUCCESS;
}


/**
 Transfer PEI performance data to gauge data node.

 @param [in]   ImageHandle      Standard entry point parameter
 @param [in]   SystemTable      Standard entry point parameter
 @param [in]   Ticker           Start tick

 @retval EFI_OUT_OF_RESOURCES   No enough resource to create data node.
 @retval EFI_SUCCESS            Transfer done successfully.

**/
EFI_STATUS
GetPeiPerformance (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable,
  IN UINT64               Ticker
  )
{
  EFI_STATUS                        Status;
  VOID                              *HobList;
  EFI_HOB_GUID_DATA_PERFORMANCE_LOG *LogHob;
  PEI_PERFORMANCE_MEASURE_LOG_ENTRY *LogEntry;
  UINT32                            Index;
  EFI_PERF_DATA_LIST                *Node;
  UINT64                            TimerValue;

  Node = CreateDataNode (0, PEI_TOK, NULL);
  if (!Node) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Initialize 'LogHob' to NULL before usage.
  //
  LogHob = NULL;

  if (Ticker != 0) {
    TimerValue = Ticker;
  } else {
    GetTimerValue (&TimerValue);
  }
  (Node->GaugeData).EndTick = TimerValue;

  InsertTailList (&mPerfDataHead, &(Node->Link));

  EfiLibGetSystemConfigurationTable (&gEfiHobListGuid, &HobList);
  do {
    Status = GetNextGuidHob (&HobList, &gEfiPeiPerformanceHobGuid, (VOID **) &LogHob, NULL);
    if (EFI_ERROR (Status) || (LogHob == NULL)) {
      //
      // Failed to get HOB for ProtocolGuid.
      //
      break;
    }

    for (Index = 0; Index < LogHob->NumberOfEntries; Index++) {
      LogEntry  = &(LogHob->Log[Index]);
      Node      = CreateDataNode (0, LogEntry->DescriptionString, NULL);
      if (!Node) {
        return EFI_OUT_OF_RESOURCES;
      }
      (Node->GaugeData).StartTick = LogEntry->StartTimeCount;

      EfiCopyMem (&(Node->GaugeData.GuidName), &LogEntry->Name, sizeof (EFI_GUID));

      InsertTailList (&mPerfDataHead, &(Node->Link));

      (Node->GaugeData).EndTick = LogEntry->StopTimeCount;
    }
  } while (!EFI_ERROR (Status));

  return EFI_SUCCESS;
}

STATIC
VOID
ConvertChar16ToChar8 (
  IN CHAR8      *Dest,
  IN CHAR16     *Src
  )
{
  while (*Src) {
    *Dest++ = (UINT8) (*Src++);
  }

  *Dest = 0;
}

/**
 Creates a record for the beginning of a performance measurement.
 Creates a record that contains the Handle, Token, and Module.
 If TimeStamp is not zero, then TimeStamp is added to the record as the start time.
 If TimeStamp is zero, then this function reads the current time stamp
 and adds that time stamp value to the record as the start time.
 
 @param [in]   Handle             The pointer to environment specific context used
                                  to identify the component being measured.
 @param [in]   Token              The pointer to a Null-terminated ASCII string
                                  that identifies the component being measured.
 @param [in]   Module             The pointer to a Null-terminated ASCII string
                                  that identifies the module being measured.
 @param [in]   TimeStamp          64-bit time stamp.

 @retval RETURN_SUCCESS          The start of the measurement was recorded.
 @retval RETURN_OUT_OF_RESOURCES There are not enough resources to record the measurement.
 @retval RETURN_DEVICE_ERROR     A device error reading the time stamp.
**/
EFI_STATUS
EFIAPI
StartPerformanceMeasurement (
  IN CONST VOID   *Handle,  OPTIONAL
  IN UINT16       *Token,   OPTIONAL
  IN UINT16       *Module,  OPTIONAL
  IN UINT64       TimeStamp
  )
{
  CHAR8    TokenBuf[DXE_PERFORMANCE_STRING_SIZE];  ///< Measured token string name.
  CHAR8    ModuleBuf[DXE_PERFORMANCE_STRING_SIZE]; ///< Module string name.

  EfiZeroMem (TokenBuf, sizeof (CHAR8) * DXE_PERFORMANCE_STRING_SIZE);
  EfiZeroMem (ModuleBuf, sizeof (CHAR8) * DXE_PERFORMANCE_STRING_SIZE);
  if  (Token != NULL) {
    ConvertChar16ToChar8 (TokenBuf, Token);
  }
  if  (Module != NULL) {
    ConvertChar16ToChar8 (ModuleBuf, Module);
  }
  return mPerformance->StartGauge (Handle, (CONST CHAR8*)TokenBuf, (CONST CHAR8*)ModuleBuf, TimeStamp);
}

/**
 Fills in the end time of a performance measurement.
 Looks up the record that matches Handle, Token, and Module.
 If the record can not be found then return RETURN_NOT_FOUND.
 If the record is found and TimeStamp is not zero,
 then TimeStamp is added to the record as the end time.
 If the record is found and TimeStamp is zero, then this function reads
 the current time stamp and adds that time stamp value to the record as the end time.
 If this function is called multiple times for the same record, then the end time is overwritten.

 @param [in]   Handle             The pointer to environment specific context used
                                  to identify the component being measured.
 @param [in]   Token              The pointer to a Null-terminated ASCII string
                                  that identifies the component being measured.
 @param [in]   Module             The pointer to a Null-terminated ASCII string
                                  that identifies the module being measured.
 @param [in]   TimeStamp          64-bit time stamp.

 @retval RETURN_SUCCESS          The end of the measurement was recorded.
 @retval RETURN_NOT_FOUND        The specified measurement record could not be found.
 @retval RETURN_DEVICE_ERROR     A device error reading the time stamp.

**/
EFI_STATUS
EFIAPI
EndPerformanceMeasurement (
  IN CONST VOID   *Handle,  OPTIONAL
  IN UINT16       *Token,   OPTIONAL
  IN UINT16       *Module,  OPTIONAL
  IN UINT64       TimeStamp
  )
{
  CHAR8    TokenBuf[DXE_PERFORMANCE_STRING_SIZE];  ///< Measured token string name.
  CHAR8    ModuleBuf[DXE_PERFORMANCE_STRING_SIZE]; ///< Module string name.

  EfiZeroMem (TokenBuf, sizeof (CHAR8) * DXE_PERFORMANCE_STRING_SIZE);
  EfiZeroMem (ModuleBuf, sizeof (CHAR8) * DXE_PERFORMANCE_STRING_SIZE);
  if  (Token != NULL) {
    ConvertChar16ToChar8 (TokenBuf, Token);
  }
  if  (Module != NULL) {
    ConvertChar16ToChar8 (ModuleBuf, Module);
  }
  return mPerformance->EndGauge (Handle, (CONST CHAR8*)TokenBuf, (CONST CHAR8*)ModuleBuf, TimeStamp);
}

/**
 Attempts to retrieve a performance measurement log entry from the performance measurement log.
 Attempts to retrieve the performance log entry specified by LogEntryKey.  If LogEntryKey is
 zero on entry, then an attempt is made to retrieve the first entry from the performance log,
 and the key for the second entry in the log is returned.  If the performance log is empty,
 then no entry is retrieved and zero is returned.  If LogEntryKey is not zero, then the performance
 log entry associated with LogEntryKey is retrieved, and the key for the next entry in the log is
 returned.  If LogEntryKey is the key for the last entry in the log, then the last log entry is
 retrieved and an implementation specific non-zero key value that specifies the end of the performance
 log is returned.  If LogEntryKey is equal this implementation specific non-zero key value, then no entry
 is retrieved and zero is returned.  In the cases where a performance log entry can be returned,
 the log entry is returned in Handle, Token, Module, StartTimeStamp, and EndTimeStamp.
 If LogEntryKey is not a valid log entry key for the performance measurement log, then ASSERT().
 If Handle is NULL, then ASSERT().
 If Token is NULL, then ASSERT().
 If Module is NULL, then ASSERT().
 If StartTimeStamp is NULL, then ASSERT().
 If EndTimeStamp is NULL, then ASSERT().
 
 @param [in]   LogEntryKey        On entry, the key of the performance measurement log entry to retrieve.
                                  0, then the first performance measurement log entry is retrieved.
                                  On exit, the key of the next performance lof entry entry.
 @param [out]  Handle             The pointer to environment specific context used to identify the component
                                  being measured.
 @param [out]  Token              The pointer to a Null-terminated ASCII string that identifies the component
                                  being measured.
 @param [out]  Module             The pointer to a Null-terminated ASCII string that identifies the module
                                  being measured.
 @param [out]  StartTimeStamp     The pointer to the 64-bit time stamp that was recorded when the measurement
                                  was started.
 @param [out]  EndTimeStamp       The pointer to the 64-bit time stamp that was recorded when the measurement
                                  was ended.
 
 @return The key for the next performance log entry (in general case).

**/
UINTN
EFIAPI
GetPerformanceMeasurement (
  IN  UINTN       LogEntryKey,
  OUT CONST VOID  **Handle,
  OUT CONST CHAR8 **Token,
  OUT CONST CHAR8 **Module,
  OUT UINT64      *StartTimeStamp,
  OUT UINT64      *EndTimeStamp
  )
{
  EFI_STATUS        Status;
  GAUGE_DATA_ENTRY  *GaugeData;

  GaugeData = NULL;

  ASSERT (Handle != NULL);
  ASSERT (Token != NULL);
  ASSERT (Module != NULL);
  ASSERT (StartTimeStamp != NULL);
  ASSERT (EndTimeStamp != NULL);

  Status = mPerformance->GetGauge (LogEntryKey++, &GaugeData);

  //
  // Make sure that LogEntryKey is a valid log entry key,
  //
  ASSERT (Status != EFI_INVALID_PARAMETER);

  if (EFI_ERROR (Status)) {
    //
    // The LogEntryKey is the last entry (equals to the total entry number).
    //
    return 0;
  }

  ASSERT (GaugeData != NULL);

  *Handle         = (VOID *) (UINTN) GaugeData->Handle;
  *Token          = GaugeData->Token;
  *Module         = GaugeData->Module;
  *StartTimeStamp = GaugeData->StartTimeStamp;
  *EndTimeStamp   = GaugeData->EndTimeStamp;

  return LogEntryKey;
}

/**
 Returns TRUE if the performance measurement macros are enabled.
 This function returns TRUE if the PERFORMANCE_LIBRARY_PROPERTY_MEASUREMENT_ENABLED bit of
 PcdPerformanceLibraryPropertyMask is set.  Otherwise FALSE is returned.
 
 @param None

 @retval TRUE                    The PERFORMANCE_LIBRARY_PROPERTY_MEASUREMENT_ENABLED bit of
                                 PcdPerformanceLibraryPropertyMask is set.
 @retval FALSE                   The PERFORMANCE_LIBRARY_PROPERTY_MEASUREMENT_ENABLED bit of
                                 PcdPerformanceLibraryPropertyMask is clear.

**/
BOOLEAN
EFIAPI
PerformanceMeasurementEnabled (
  VOID
  )
{
  EFI_STATUS            Status;

  Status = gBS->LocateProtocol (&mPerformanceProtocolGuid, NULL, (VOID **) &mPerformance);
  if (!EFI_ERROR (Status)) {
    return TRUE;
  } else {
    return FALSE;
  }
}