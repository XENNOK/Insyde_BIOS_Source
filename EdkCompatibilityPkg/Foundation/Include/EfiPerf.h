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

Copyright (c) 2004, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

 EfiPerf.h

Abstract:
 EfiPerf.h provides performance primitive for the DXE and Shell phase


--*/

#ifndef _EFI_PERF_H_
#define _EFI_PERF_H_

#include EFI_PROTOCOL_DEFINITION (Performance)

/**
 TODO: Add function description

 @param [in]   ImageHandle      TODO: add argument description
 @param [in]   SystemTable      TODO: add argument description
 @param [in]   Ticker           TODO: add argument description

 @return TODO: add return values

**/
EFI_STATUS
EFIAPI
InitializePerformanceInfrastructure (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable,
  IN UINT64             Ticker
  );

/**
 TODO: Add function description

 @param [in]   Handle           TODO: add argument description
 @param [in]   Token            TODO: add argument description
 @param [in]   Host             TODO: add argument description
 @param [in]   Ticker           TODO: add argument description

 @return TODO: add return values

**/
EFI_STATUS
EFIAPI
EndMeasure (
  IN EFI_HANDLE       Handle,
  IN UINT16           *Token,
  IN UINT16           *Host,
  IN UINT64           Ticker
  );

/**
 TODO: Add function description

 @param [in]   Handle           TODO: add argument description
 @param [in]   Token            TODO: add argument description
 @param [in]   Host             TODO: add argument description
 @param [in]   Ticker           TODO: add argument description

 @return TODO: add return values

**/
EFI_STATUS
EFIAPI
StartMeasure (
  IN EFI_HANDLE       Handle,
  IN UINT16           *Token,
  IN UINT16           *Host,
  IN UINT64           Ticker
  );

/**
 TODO: Add function description

 @param [in]   Handle           TODO: add argument description
 @param [in]   Token            TODO: add argument description
 @param [in]   Host             TODO: add argument description
 @param [in]   HandleNew        TODO: add argument description
 @param [in]   TokenNew         TODO: add argument description
 @param [in]   HostNew          TODO: add argument description

 @return TODO: add return values

**/
EFI_STATUS
EFIAPI
UpdateMeasure (
  IN EFI_HANDLE      Handle,
  IN UINT16          *Token,
  IN UINT16          *Host,
  IN EFI_HANDLE      HandleNew,
  IN UINT16          *TokenNew,
  IN UINT16          *HostNew
  );

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
EFI_STATUS
EFIAPI
StartPerformanceMeasurement (
  IN CONST VOID   *Handle,  OPTIONAL
  IN UINT16       *Token,   OPTIONAL
  IN UINT16       *Module,  OPTIONAL
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
EFI_STATUS
EFIAPI
EndPerformanceMeasurement (
  IN CONST VOID   *Handle,  OPTIONAL
  IN UINT16       *Token,   OPTIONAL
  IN UINT16       *Module,  OPTIONAL
  IN UINT64       TimeStamp
  );

/**
 Retrieves a previously logged performance measurement.
 Retrieves the performance log entry from the performance log specified by LogEntryKey.
 If it stands for a valid entry, then EFI_SUCCESS is returned and
 GaugeDataEntry stores the pointer to that entry.
 
 @param [in]   LogEntryKey        The key for the previous performance measurement log entry.
                                  If 0, then the first performance measurement log entry is retrieved.
 @param [out]  Handle
 @param [out]  Token
 @param [out]  Module
 @param [out]  StartTimeStamp
 @param [out]  EndTimeStamp

 @retval EFI_SUCCESS             The GuageDataEntry is successfully found based on LogEntryKey.
 @retval EFI_NOT_FOUND           There is no entry after the measurement referred to by LogEntryKey.
 @retval EFI_INVALID_PARAMETER   The LogEntryKey is not a valid entry, or GaugeDataEntry is NULL.
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
  );

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
  );

#ifdef EFI_DXE_PERFORMANCE
#define PERF_ENABLE(handle, table, ticker)      InitializePerformanceInfrastructure (handle, table, ticker)
#define PERF_START(Handle, Token, Module, TimeStamp)                  \
  do {                                                                \
    if (PerformanceMeasurementEnabled ()) {                           \
      StartPerformanceMeasurement (Handle, Token, Module, TimeStamp); \
    }                                                                 \
  } while (FALSE)
#define PERF_END(Handle, Token, Module, TimeStamp)                    \
  do {                                                                \
    if (PerformanceMeasurementEnabled ()) {                           \
      EndPerformanceMeasurement (Handle, Token, Module, TimeStamp);   \
    }                                                                 \
  } while (FALSE)
#define PERF_UPDATE(handle, token, host, handlenew, tokennew, hostnew) \
  UpdateMeasure (handle, \
                 token, \
                 host, \
                 handlenew, \
                 tokennew, \
                 hostnew \
      )
#define PERF_CODE(code) code
#else
#define PERF_ENABLE(handle, table, ticker)
#define PERF_START(handle, token, host, ticker)
#define PERF_END(handle, token, host, ticker)
#define PERF_UPDATE(handle, token, host, handlenew, tokennew, hostnew)
#define PERF_CODE(code)
#endif

#endif
