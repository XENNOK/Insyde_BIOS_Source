/** @file

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

#ifndef _PERFORMANCE_MEASURE_PROTOCOL_H_
#define _PERFORMANCE_MEASURE_PROTOCOL_H_

#define EFI_PERFORMANCE_MEASURE_PROTOCOL_GUID \
  {0xe911342e, 0x7b6b, 0x4257, 0x83, 0xfb, 0x4f, 0x0f, 0x41, 0xd9, 0x31, 0xdd }

typedef struct _EFI_PERFORMANCE_MEASURE_PROTOCOL EFI_PERFORMANCE_MEASURE_PROTOCOL;

typedef enum {
  MeasureInMilliSec,
  MeasureInMicroSec,
  MeasureInNanoSec,
  MeasureMax
} PERFORMANCE_MEASURE_UINT;

typedef
VOID
(EFIAPI *EFI_PERFORMANCE_MEASURE_PROTOCOL_START) (
  IN UINTN           RecordIndex,
  IN BOOLEAN         ValidFlag
  );

typedef
UINT64
(EFIAPI *EFI_PERFORMANCE_MEASURE_PROTOCOL_END) (
  IN UINTN           RecordIndex,
  IN BOOLEAN         ValidFlag,
  IN CHAR16          *StartString,
  IN CHAR16          *EndString
  );


typedef
UINT64
(EFIAPI *EFI_PERFORMANCE_MEASURE_PROTOCOL_TOTAL) (
  IN UINTN           RecordIndex,
  IN BOOLEAN         ValidFlag,
  IN BOOLEAN         ClearRecord,
  IN CHAR16          *StartString,
  IN CHAR16          *EndString
  );

typedef
EFI_STATUS
(EFIAPI *EFI_PERFORMANCE_MEASURE_PROTOCOL_CHANGE_UNIT) (
  IN PERFORMANCE_MEASURE_UINT                      MeasureUnit
  );

struct _EFI_PERFORMANCE_MEASURE_PROTOCOL {
  BOOLEAN                                          IsPrintEnable;
  BOOLEAN                                          PrintTickDifference;
  EFI_PERFORMANCE_MEASURE_PROTOCOL_START           Start;
  EFI_PERFORMANCE_MEASURE_PROTOCOL_END             End;
  EFI_PERFORMANCE_MEASURE_PROTOCOL_TOTAL           Total;
  EFI_PERFORMANCE_MEASURE_PROTOCOL_CHANGE_UNIT     ChangeUnit;
};

extern EFI_GUID gPerformanceMeasureProtocolGuid;

#endif

