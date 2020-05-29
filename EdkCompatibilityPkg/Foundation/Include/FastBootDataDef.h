/*++

Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:     

Module Name:

  FastBootDataDef.h
  
Abstract:

  This file defines Fast Boot exception types and categories defined by
  the Intel Fast Boot Specification.

--*/

#ifndef _FAST_BOOT_DATA_DEF_H_
#define _FAST_BOOT_DATA_DEF_H_

typedef enum {
  NoException = 0,
  ExceptionType1,
  ExceptionType2,
  ExceptionType3A,
  ExceptionType3B,
  ExceptionTypeMax
} FAST_BOOT_EXCEPTION_TYPE;

typedef enum {
  NoExceptionCategory = 0,
  FastBootDisabled,
  FastBootOverridden,
  FastBootBreakByUser,
  FirstBoot,
  BootFailure,
  FirmwareUpdate,
  HardwareChanged,
  SetupConfigurationChanged,
  ConsoleDeviceChanged,
  BootDeviceChanged,
  ContentLost,
  PowerFailure,
  HardwareError,
  SpecialBoot,
  AnyOfOtherCategories,
  ExceptionCategoryMax  
} FAST_BOOT_EXCEPTION_CATEGORY;

#endif
