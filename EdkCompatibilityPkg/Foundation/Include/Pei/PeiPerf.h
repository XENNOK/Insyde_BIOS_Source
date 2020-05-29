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

 PeiPerf.h

Abstract:

 PeiPerf.h provides performance primitives for PEI modules


--*/

#ifndef _PEI_PERF_H_
#define _PEI_PERF_H_

/**
 Creates a record for the beginning of a performance measurement.
 Creates a record that contains the Handle, Token, and Module.
 If TimeStamp is not zero, then TimeStamp is added to the record as the start time.
 If TimeStamp is zero, then this function reads the current time stamp
 and adds that time stamp value to the record as the start time.
 
 @param        PeiServices
 @param [in]   Handle       Pointer to environment specific context used
                            to identify the component being measured.
 @param [in]   Token        Pointer to a Null-terminated ASCII string
                            that identifies the component being measured.
 @param [in]   Module       Pointer to a Null-terminated ASCII string
                            that identifies the module being measured.
 @param [in]   TimeStamp    64-bit time stamp.

 @retval RETURN_SUCCESS          The start of the measurement was recorded.
 @retval RETURN_OUT_OF_RESOURCES There are not enough resources to record the measurement.
**/
EFI_STATUS
EFIAPI
PeiStartPerformanceMeasurement (
  EFI_PEI_SERVICES              **PeiServices,
  IN CONST VOID                 *Handle,  OPTIONAL
  IN UINT16                     *Token,   OPTIONAL
  IN UINT16                     *Module,  OPTIONAL
  IN UINT64                     TimeStamp
  );

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
  );

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
  IN  UINTN                     LogEntryKey,
  OUT CONST VOID                **Handle,
  OUT CONST CHAR8               **Token,
  OUT CONST CHAR8               **Module,
  OUT UINT64                    *StartTimeStamp,
  OUT UINT64                    *EndTimeStamp
  );
#ifdef EFI_PEI_PERFORMANCE
#define PEI_PERF_START(Ps, Token, FileHeader, Value)  PeiStartPerformanceMeasurement (Ps, NULL, Token, NULL, Value)
#define PEI_PERF_END(Ps, Token, FileHeader, Value)    PeiEndPerformanceMeasurement (Ps, NULL, Token, NULL, Value)
#else
#define PEI_PERF_START(Ps, Token, FileHeader, Value)
#define PEI_PERF_END(Ps, Token, FileHeader, Value)
#endif

#endif
