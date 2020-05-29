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

Copyright (c) 2004 - 2008, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  Perf.c

Abstract:

  Support for performance primitives.

--*/

#include "Tiano.h"
#include "Pei.h"
#include "PeiLib.h"
#include "PeiHob.h"
#include "EfiCommonLib.h"

EFI_GUID mPerformanceProtocolGuid  = { 0x76b6bdfa, 0x2acd, 0x4462, 0x9E, 0x3F, 0xcb, 0x58, 0xC9, 0x69, 0xd9, 0x37 };
//
// Perfomance HOB data definitions
//
#ifndef MAX_PEI_PERF_LOG_ENTRIES
#define MAX_PEI_PERF_LOG_ENTRIES 28
#endif

//
// PEI_PERFORMANCE_STRING_SIZE must be a multiple of 8.
//
#define PEI_PERFORMANCE_STRING_SIZE     8
#define PEI_PERFORMANCE_STRING_LENGTH   (PEI_PERFORMANCE_STRING_SIZE - 1)

typedef struct {
  EFI_PHYSICAL_ADDRESS  Handle;
  CHAR8                 Token[PEI_PERFORMANCE_STRING_SIZE];   ///< Measured token string name.
  CHAR8                 Module[PEI_PERFORMANCE_STRING_SIZE];  ///< Module string name.
  UINT64                StartTimeStamp;                       ///< Start time point.
  UINT64                EndTimeStamp;                         ///< End time point.
} PEI_PERFORMANCE_LOG_ENTRY;

//
// The header must be aligned at 8 bytes.
//
typedef struct {
  UINT32                NumberOfEntries;  ///< The number of all performance log entries.
  UINT32                Reserved;
} PEI_PERFORMANCE_LOG_HEADER;

#define GET_GUID_HOB_DATA(GuidHob)      ((VOID *) (((UINT8 *) &((GuidHob)->Name)) + sizeof (EFI_GUID)))
//
// Prototype functions
//
EFI_STATUS
GetTimerValue (
  OUT UINT64    *TimerValue
  );

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
 Get the next guid hob.

 @param [in, out] HobStart      A pointer to the start hob.
 @param [in]   Guid             A pointer to a guid.

 @return Status code.
 @retval EFI_NOT_FOUND          Next Guid hob not found
 @retval EFI_SUCCESS            Next Guid hob found and data for this Guid got
 @retval EFI_INVALID_PARAMETER  invalid parameter

**/
VOID *
EFIAPI
PeiGetNextGuidHob (
  IN OUT VOID      **HobStart,
  IN     EFI_GUID  * Guid
  )
{
  EFI_STATUS            Status;
  EFI_PEI_HOB_POINTERS  GuidHob;

  GuidHob.Raw = *HobStart;
  for (Status = EFI_NOT_FOUND; EFI_ERROR (Status);) {

    GuidHob.Raw = *HobStart;
    if (END_OF_HOB_LIST (GuidHob)) {
      return NULL;
    }

    GuidHob.Raw = GetHob (EFI_HOB_TYPE_GUID_EXTENSION, *HobStart);
    if (GuidHob.Header->HobType == EFI_HOB_TYPE_GUID_EXTENSION) {
      if ( ((INT32 *)Guid)[0] == ((INT32 *)&GuidHob.Guid->Name)[0] &&
           ((INT32 *)Guid)[1] == ((INT32 *)&GuidHob.Guid->Name)[1] &&
           ((INT32 *)Guid)[2] == ((INT32 *)&GuidHob.Guid->Name)[2] &&
           ((INT32 *)Guid)[3] == ((INT32 *)&GuidHob.Guid->Name)[3] ) {
        Status  = EFI_SUCCESS;
        break;
      }
    }

    *HobStart = GET_NEXT_HOB (GuidHob);
  }

  return GuidHob.Raw;
}

PEI_PERFORMANCE_LOG_HEADER *
InternalGetPerformanceHobLog (
  EFI_PEI_SERVICES              **PeiServices
  )
/*+
  Gets PEI the GUID HOB for PEI performance.

  This internal function searches for the GUID HOB for PEI performance.
  If that GUID HOB is not found, it will build a new one.
  It returns the data area of that GUID HOB to record performance log.

  @param  Handle                  Pointer to environment specific context used
                                  to identify the component being measured.
  @param  Token                   Pointer to a Null-terminated ASCII string
                                  that identifies the component being measured.
  @param  Module                  Pointer to a Null-terminated ASCII string
                                  that identifies the module being measured.

  @retval The index of log entry in the array.

--*/
{
  EFI_STATUS                  Status;
  VOID                        *HobList;
  EFI_HOB_GUID_TYPE           *GuidHob = NULL;
  PEI_PERFORMANCE_LOG_HEADER  *PeiPerformanceLog;
  UINTN                       PeiPerformanceLogSize;

  //
  // Get the Hob table pointer
  //
  Status = (*PeiServices)->GetHobList (
                            PeiServices,
                            (VOID **) &HobList
                            );
  if (!EFI_ERROR(Status)) {
    GuidHob = PeiGetNextGuidHob (&HobList, &mPerformanceProtocolGuid);
  }

  if (GuidHob != NULL) {
    //
    // PEI Performance HOB was found, then return the existing one.
    //
    PeiPerformanceLog = GET_GUID_HOB_DATA (GuidHob);
  } else {
    //
    // PEI Performance HOB was not found, then build one.
    //
    PeiPerformanceLogSize = sizeof (PEI_PERFORMANCE_LOG_HEADER) +
                            sizeof (PEI_PERFORMANCE_LOG_ENTRY) * MAX_PEI_PERF_LOG_ENTRIES;
    PeiBuildHobGuid (PeiServices,&mPerformanceProtocolGuid, PeiPerformanceLogSize, (VOID **)&GuidHob);
    PeiPerformanceLog     = (PEI_PERFORMANCE_LOG_HEADER *)(GuidHob+1);
    EfiCommonLibZeroMem (PeiPerformanceLog, PeiPerformanceLogSize);
  }

  return PeiPerformanceLog;
}

/**
 Searches in the log array with keyword Handle, Token and Module.
 This internal function searches for the log entry in the log array.
 If there is an entry that exactly matches the given key word triple
 and its end time stamp is zero, then the index of that log entry is returned;
 otherwise, the the number of log entries in the array is returned.
 
 @param [in]   PeiPerformanceLog  Pointer to the data structure containing PEI
                                  performance data.
 @param [in]   Handle             Pointer to environment specific context used
                                  to identify the component being measured.
 @param [in]   Token              Pointer to a Null-terminated ASCII string
                                  that identifies the component being measured.
 @param [in]   Module             Pointer to a Null-terminated ASCII string
                                  that identifies the module being measured.

 @retval The index of log entry in the array.
**/
UINT32
InternalSearchForLogEntry (
  IN PEI_PERFORMANCE_LOG_HEADER *PeiPerformanceLog,
  IN CONST VOID                 *Handle,  OPTIONAL
  IN CHAR8                      *Token,   OPTIONAL
  IN CHAR8                      *Module   OPTIONAL
  )
{
  UINT32                    Index;
  UINT32                    NumberOfEntries;
  PEI_PERFORMANCE_LOG_ENTRY *LogEntryArray;


  if (Token == NULL) {
    Token = "";
  }
  if (Module == NULL) {
    Module = "";
  }
  NumberOfEntries = PeiPerformanceLog->NumberOfEntries;
  LogEntryArray   = (PEI_PERFORMANCE_LOG_ENTRY *) (PeiPerformanceLog + 1);

  for (Index = 0; Index < NumberOfEntries; Index++) {
    if ((LogEntryArray[Index].Handle == (EFI_PHYSICAL_ADDRESS) (UINTN) Handle) &&
         EfiAsciiStrnCmp (LogEntryArray[Index].Token, Token, PEI_PERFORMANCE_STRING_LENGTH) == 0 &&
         EfiAsciiStrnCmp (LogEntryArray[Index].Module, Module, PEI_PERFORMANCE_STRING_LENGTH) == 0 &&
         LogEntryArray[Index].EndTimeStamp == 0
       ) {
      break;
    }
  }
  return Index;
}


/**
 Creates a record for the beginning of a performance measurement.
 Creates a record that contains the Handle, Token, and Module.
 If TimeStamp is not zero, then TimeStamp is added to the record as the start time.
 If TimeStamp is zero, then this function reads the current time stamp
 and adds that time stamp value to the record as the start time.
 
 @param        PeiServices
 @param [in]   Handle             Pointer to environment specific context used
                                  to identify the component being measured.
 @param [in]   Token              Pointer to a Null-terminated ASCII string
                                  that identifies the component being measured.
 @param [in]   Module             Pointer to a Null-terminated ASCII string
                                  that identifies the module being measured.
 @param [in]   TimeStamp          64-bit time stamp.

 @retval RETURN_SUCCESS          The start of the measurement was recorded.
 @retval RETURN_OUT_OF_RESOURCES There are not enough resources to record the measurement.
**/
EFI_STATUS
EFIAPI
PeiStartPerformanceMeasurement (
  EFI_PEI_SERVICES              **PeiServices,
  IN CONST VOID                 *Handle,  OPTIONAL
  IN UINT16       *Token,   OPTIONAL
  IN UINT16       *Module,  OPTIONAL
  IN UINT64                     TimeStamp
  )
{
  PEI_PERFORMANCE_LOG_HEADER  *PeiPerformanceLog;
  PEI_PERFORMANCE_LOG_ENTRY   *LogEntryArray;
  UINT32                      Index;
  CHAR8                       TokenBuf[PEI_PERFORMANCE_STRING_SIZE];  ///< Measured token string name.
  CHAR8                       ModuleBuf[PEI_PERFORMANCE_STRING_SIZE]; ///< Module string name.

  EfiCommonLibZeroMem (TokenBuf, sizeof (CHAR8) * PEI_PERFORMANCE_STRING_SIZE);
  EfiCommonLibZeroMem (ModuleBuf, sizeof (CHAR8) * PEI_PERFORMANCE_STRING_SIZE);
  if  (Token != NULL) {
    ConvertChar16ToChar8 (TokenBuf, Token);
  }
  if  (Module != NULL) {
    ConvertChar16ToChar8 (ModuleBuf, Module);
  }

  PeiPerformanceLog = InternalGetPerformanceHobLog (PeiServices);

  if (PeiPerformanceLog->NumberOfEntries >= MAX_PEI_PERF_LOG_ENTRIES) {
    return EFI_OUT_OF_RESOURCES;
  }
  Index                       = PeiPerformanceLog->NumberOfEntries++;
  LogEntryArray               = (PEI_PERFORMANCE_LOG_ENTRY *) (PeiPerformanceLog + 1);
  LogEntryArray[Index].Handle = (EFI_PHYSICAL_ADDRESS) (UINTN) Handle;

  if (Token != NULL) {
    EfiAsciiStrnCpy (LogEntryArray[Index].Token, TokenBuf, PEI_PERFORMANCE_STRING_LENGTH);
  }
  if (Module != NULL) {
    EfiAsciiStrnCpy (LogEntryArray[Index].Module, ModuleBuf, PEI_PERFORMANCE_STRING_LENGTH);
  }

  if (TimeStamp == 0) {
    GetTimerValue (&TimeStamp);
  }
  LogEntryArray[Index].StartTimeStamp = TimeStamp;

  return EFI_SUCCESS;
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

 @param        PeiServices
 @param [in]   Handle            Pointer to environment specific context used
                                 to identify the component being measured.
 @param [in]   Token             Pointer to a Null-terminated ASCII string
                                 that identifies the component being measured.
 @param [in]   Module            Pointer to a Null-terminated ASCII string
                                 that identifies the module being measured.
 @param [in]   TimeStamp         64-bit time stamp.

 @retval RETURN_SUCCESS          The end of  the measurement was recorded.
 @retval RETURN_NOT_FOUND        The specified measurement record could not be found.

**/
EFI_STATUS
EFIAPI
PeiEndPerformanceMeasurement (
  EFI_PEI_SERVICES              **PeiServices,
  IN CONST VOID                 *Handle,  OPTIONAL
  IN UINT16                     *Token,   OPTIONAL
  IN UINT16                     *Module,  OPTIONAL
  IN UINT64                     TimeStamp
  )
{
  PEI_PERFORMANCE_LOG_HEADER  *PeiPerformanceLog;
  PEI_PERFORMANCE_LOG_ENTRY   *LogEntryArray;
  UINT32                      Index;
  CHAR8                       TokenBuf[PEI_PERFORMANCE_STRING_SIZE];  ///< Measured token string name.
  CHAR8                       ModuleBuf[PEI_PERFORMANCE_STRING_SIZE]; ///< Module string name.

  if (TimeStamp == 0) {
    GetTimerValue (&TimeStamp);
  }

  EfiCommonLibZeroMem (TokenBuf, sizeof (CHAR8) * PEI_PERFORMANCE_STRING_SIZE);
  EfiCommonLibZeroMem (ModuleBuf, sizeof (CHAR8) * PEI_PERFORMANCE_STRING_SIZE);
  if  (Token != NULL) {
    ConvertChar16ToChar8 (TokenBuf, Token);
  }
  if  (Module != NULL) {
    ConvertChar16ToChar8 (ModuleBuf, Module);
  }
  PeiPerformanceLog = InternalGetPerformanceHobLog (PeiServices);
  Index             = InternalSearchForLogEntry (PeiPerformanceLog, Handle, TokenBuf, ModuleBuf);
  if (Index >= PeiPerformanceLog->NumberOfEntries) {
    return EFI_NOT_FOUND;
  }
  LogEntryArray     = (PEI_PERFORMANCE_LOG_ENTRY *) (PeiPerformanceLog + 1);
  LogEntryArray[Index].EndTimeStamp = TimeStamp;

  return EFI_SUCCESS;
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
 
 @param        PeiServices
 @param [in]   LogEntryKey        On entry, the key of the performance measurement log entry to retrieve.
                                  0, then the first performance measurement log entry is retrieved.
                                  On exit, the key of the next performance lof entry entry.
 @param [out]  Handle             Pointer to environment specific context used to identify the component
                                  being measured.
 @param [out]  Token              Pointer to a Null-terminated ASCII string that identifies the component
                                  being measured.
 @param [out]  Module             Pointer to a Null-terminated ASCII string that identifies the module
                                  being measured.
 @param [out]  StartTimeStamp     Pointer to the 64-bit time stamp that was recorded when the measurement
                                  was started.
 @param [out]  EndTimeStamp       Pointer to the 64-bit time stamp that was recorded when the measurement
                                  was ended.

 @return The key for the next performance log entry (in general case).
 
**/
UINTN
EFIAPI
PeiGetPerformanceMeasurement (
EFI_PEI_SERVICES              **PeiServices,
  IN  UINTN                   LogEntryKey,
  OUT CONST VOID              **Handle,
  OUT CONST CHAR8             **Token,
  OUT CONST CHAR8             **Module,
  OUT UINT64                  *StartTimeStamp,
  OUT UINT64                  *EndTimeStamp
  )
{
  PEI_PERFORMANCE_LOG_HEADER  *PeiPerformanceLog;
  PEI_PERFORMANCE_LOG_ENTRY   *CurrentLogEntry;
  PEI_PERFORMANCE_LOG_ENTRY   *LogEntryArray;
  UINTN                       NumberOfEntries;

  ASSERT (Handle != NULL);
  ASSERT (Token != NULL);
  ASSERT (Module != NULL);
  ASSERT (StartTimeStamp != NULL);
  ASSERT (EndTimeStamp != NULL);

  PeiPerformanceLog = InternalGetPerformanceHobLog (PeiServices);

  NumberOfEntries   = (UINTN) (PeiPerformanceLog->NumberOfEntries);
  LogEntryArray     = (PEI_PERFORMANCE_LOG_ENTRY *) (PeiPerformanceLog + 1);
  //
  // Make sure that LogEntryKey is a valid log entry key.
  //
  ASSERT (LogEntryKey <= NumberOfEntries);

  if (LogEntryKey == NumberOfEntries) {
    return 0;
  }

  CurrentLogEntry = &(LogEntryArray[LogEntryKey++]);

  *Handle         = (VOID *) (UINTN) (CurrentLogEntry->Handle);
  *Token          = CurrentLogEntry->Token;
  *Module         = CurrentLogEntry->Module;
  *StartTimeStamp = CurrentLogEntry->StartTimeStamp;
  *EndTimeStamp   = CurrentLogEntry->EndTimeStamp;

  return LogEntryKey;
}
