/** @file
  Performance measure protocol driver related functions

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "PerfDxeCommon.h"
#include <Library/BaseMemoryLib.h>

UINT64   mStartTick[PERF_INFO_NUM];
UINT64   mTotalDiffTick[PERF_INFO_NUM];
UINT32   mCount[PERF_INFO_NUM];
UINT32   mUnitDivisor;
CHAR16   *mUnitStrArray[] = {L"MilliSec", L"MicroSec", L"NanooSec"};
CHAR16   *mUnitStr;

EFI_PERFORMANCE_MEASURE_PROTOCOL   mPerfMeasure;

UINT64
EFIAPI
InternalGetTimeInPicoSecond (
  IN UINT64                        Ticks
  )
{
  STATIC UINT64                    Frequency = 0;
  UINT64                           NanoSeconds;
  UINT64                           Remainder;
  INTN                             Shift;

  if (Frequency == 0) {
    Frequency = GetPerformanceCounterProperties (NULL, NULL);
  }

  //
  //          Ticks
  // Time = --------- x 1,000,000,000
  //        Frequency
  //
  NanoSeconds = MultU64x32 (DivU64x64Remainder (Ticks, Frequency, &Remainder), 1000000000u);

  //
  // Ensure (Remainder * 1,000,000,000) will not overflow 64-bit.
  // Since 2^29 < 1,000,000,000 = 0x3B9ACA00 < 2^30, Remainder should < 2^(64-30) = 2^34,
  // i.e. highest bit set in Remainder should <= 33.
  //
  Shift = MAX (0, HighBitSet64 (Remainder) - 33);
  Remainder = RShiftU64 (Remainder, (UINTN) Shift);
  Frequency = RShiftU64 (Frequency, (UINTN) Shift);
  NanoSeconds += DivU64x64Remainder (MultU64x32 (Remainder, 1000000000u), Frequency, NULL);

  return NanoSeconds;
}

VOID
EFIAPI
PerfMeasureStart (
  IN UINTN                         RecordIndex,
  IN BOOLEAN                       ValidFlag
  )
{
  ASSERT (RecordIndex < PERF_INFO_NUM);

  if (!ValidFlag) {
    return;
  }

  mStartTick[RecordIndex] = GetPerformanceCounter();
}


UINT64
EFIAPI
PerfMeasureEnd (
  IN UINTN                         RecordIndex,
  IN BOOLEAN                       ValidFlag,
  IN CHAR16                        *StartString,
  IN CHAR16                        *EndString
  )
{
  UINT64                           EndTick;
  UINT64                           DiffTick;
  UINT64                           DiffTime;

  ASSERT (RecordIndex < PERF_INFO_NUM);

  if (!ValidFlag) {
    return 0;
  }

  EndTick  = GetPerformanceCounter();
  DiffTick = EndTick - mStartTick[RecordIndex];
  mTotalDiffTick[RecordIndex] += DiffTick;
  mCount[RecordIndex]++;

  if (mPerfMeasure.PrintTickDifference) {
    if (mPerfMeasure.IsPrintEnable) {
      DEBUG ((EFI_D_ERROR, "%s, Ticks = %08ld %s", StartString, DiffTick, EndString));
    }
    return DiffTick;
  } else {
    DiffTime = DivU64x32 (InternalGetTimeInPicoSecond (DiffTick), mUnitDivisor);
    if (mPerfMeasure.IsPrintEnable) {
      DEBUG ((EFI_D_ERROR, "%s, %s = %08ld %s", StartString, mUnitStr, DiffTime, EndString));
    }
    return DiffTime;
  }
}

UINT64
EFIAPI
PerfMeasureTotal (
  IN UINTN                         RecordIndex,
  IN BOOLEAN                       ValidFlag,
  IN BOOLEAN                       ClearRecord,
  IN CHAR16                        *StartString,
  IN CHAR16                        *EndString
  )
{
  UINT64                           Difference;

  if (!ValidFlag) {
    return 0;
  }

  if (mPerfMeasure.PrintTickDifference) {
    Difference = mTotalDiffTick[RecordIndex];
    if (mPerfMeasure.IsPrintEnable) {
      DEBUG ((EFI_D_ERROR, "%s, Ticks = %08ld %s", StartString, Difference, EndString));
    }
  } else {
    Difference = DivU64x32 (InternalGetTimeInPicoSecond (mTotalDiffTick[RecordIndex]), mUnitDivisor);
    if (mPerfMeasure.IsPrintEnable) {
      if (mCount[RecordIndex] == 0) {
        DEBUG ((EFI_D_ERROR, "%s, %s = %08ld %s", StartString, mUnitStr, Difference, EndString));
      } else {
        DEBUG ((EFI_D_ERROR, "%s, %s = %08ld (Count = %d, Avg = %d)\n", StartString, mUnitStr, Difference, mCount[RecordIndex], DivU64x32 (Difference, mCount[RecordIndex])));
      }
    }
  }

  if (ClearRecord == TRUE) {
    mTotalDiffTick[RecordIndex] = 0;
    mCount[RecordIndex] = 0;
  }

  return Difference;
}

EFI_STATUS
EFIAPI
PerfMeasureChangeUnit (
  IN PERFORMANCE_MEASURE_UINT      MeasureUnit
  )
{
  ASSERT (MeasureUnit < MeasureMax);

  if (MeasureUnit >= MeasureMax) {
    return EFI_INVALID_PARAMETER;
  }

  switch (MeasureUnit) {

  case MeasureInMilliSec:
    mUnitStr     = mUnitStrArray[0];
    mUnitDivisor = 1000000000;
    break;

  case MeasureInMicroSec:
    mUnitStr     = mUnitStrArray[1];
    mUnitDivisor = 1000000;
    break;

  case MeasureInNanoSec:
    mUnitStr     = mUnitStrArray[2];
    mUnitDivisor = 1000;
    break;

  default:
    ASSERT (FALSE);
    break;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
InitializePerfDxe (
  IN EFI_HANDLE                    ImageHandle,
  IN EFI_SYSTEM_TABLE              *SystemTable
  )
{
  EFI_STATUS                       Status;

  ZeroMem (mStartTick, sizeof (mStartTick));
  ZeroMem (mTotalDiffTick, sizeof (mTotalDiffTick));

  mPerfMeasure.IsPrintEnable       = TRUE;
  mPerfMeasure.PrintTickDifference = FALSE;
  mPerfMeasure.Start               = PerfMeasureStart;
  mPerfMeasure.End                 = PerfMeasureEnd;
  mPerfMeasure.Total               = PerfMeasureTotal;
  mPerfMeasure.ChangeUnit          = PerfMeasureChangeUnit;

  PerfMeasureChangeUnit (MeasureInMicroSec);

  Status = gBS->InstallProtocolInterface (
                  &ImageHandle,
                  &gPerformanceMeasureProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mPerfMeasure
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

