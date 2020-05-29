/** @file

  Include the the general MRC types

@copyright
  Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
#ifndef _MRC_TYPES_H
#define _MRC_TYPES_H

//
// Data Types
//
//[-start-140610-IB05400533-modify]//
typedef UINT64              U64;
typedef UINT32              U32;
typedef UINT16              U16;
typedef UINT8               U8;
typedef INT64               S64;
typedef INT32               S32;
typedef INT16               S16;
typedef INT8                S8;
typedef BOOLEAN             MrcBool;
//[-end-140610-IB05400533-modify]//

#ifndef BOOL
#undef FALSE
#undef TRUE
typedef enum {
  FALSE = (0 == 1),
  TRUE  = (1 == 1)
} BOOL;
#endif

#ifndef NULL
#define NULL  ((void *) 0)
#endif

#ifndef IN
#define IN
#endif

#ifndef OPTIONAL
#define OPTIONAL
#endif

#ifndef OUT
#define OUT
#endif

#define UNSUPPORT 0
#define SUPPORT   1

typedef enum {
  mrcSuccess,
  mrcFail,
  mrcWrongInputParameter,
  mrcCasError,
  mrcTimingError,
  mrcSenseAmpErr,
  mrcReadMPRErr,
  mrcReadLevelingError,
  mrcWriteLevelingError,
  mrcDataTimeCentering1DErr,
  mrcWriteVoltage2DError,
  mrcReadVoltage2DError,
  mrcWrError,
  mrcDimmNotSupport,
  mrcChannelNotSupport,
  mrcPiSettingError,
  mrcDqsPiSettingError,
  mrcDeviceBusy,
  mrcFrequencyChange,
  mrcReutSequenceError,
  mrcCrcError,
  mrcFrequencyError,
  mrcDimmNotExist,
  mrcColdBootRequired,
  mrcRoundTripLatencyError,
  mrcMixedDimmSystem,
  mrcAliasDetected,
  mrcRetrain
} MrcStatus;

//
// general  macros
//
#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef ABS
#define ABS(x)  (((x) < 0) ? (-(x)) : (x))
#endif
//
//  use for ignore parames
//
// #define MRC_IGNORE_PARAM(x) ((x) = (x))
//
#if _MSC_EXTENSIONS
//
// Disable warning that make it impossible to compile at /W4
// This only works for Microsoft* tools
//
//
// Disabling bitfield type checking warnings.
//
#pragma warning (disable : 4214)
//
// Unreferenced formal parameter - We are object oriented, so we pass parameters even
//  if we don't need them.
//
#pragma warning (disable : 4100)
//
// ASSERT(FALSE) or while (TRUE) are legal constructs so supress this warning
//
#pragma warning(disable : 4127)

#endif // _MSC_EXTENSIONS
#define MRC_BIT0  0x00000001
#define MRC_BIT1  0x00000002
#define MRC_BIT2  0x00000004
#define MRC_BIT3  0x00000008
#define MRC_BIT4  0x00000010
#define MRC_BIT5  0x00000020
#define MRC_BIT6  0x00000040
#define MRC_BIT7  0x00000080
#define MRC_BIT8  0x00000100
#define MRC_BIT9  0x00000200
#define MRC_BIT10 0x00000400
#define MRC_BIT11 0x00000800
#define MRC_BIT12 0x00001000
#define MRC_BIT13 0x00002000
#define MRC_BIT14 0x00004000
#define MRC_BIT15 0x00008000
#define MRC_BIT16 0x00010000
#define MRC_BIT17 0x00020000
#define MRC_BIT18 0x00040000
#define MRC_BIT19 0x00080000
#define MRC_BIT20 0x00100000
#define MRC_BIT21 0x00200000
#define MRC_BIT22 0x00400000
#define MRC_BIT23 0x00800000
#define MRC_BIT24 0x01000000
#define MRC_BIT25 0x02000000
#define MRC_BIT26 0x04000000
#define MRC_BIT27 0x08000000
#define MRC_BIT28 0x10000000
#define MRC_BIT29 0x20000000
#define MRC_BIT30 0x40000000
#define MRC_BIT31 0x80000000

#define MRC_DEADLOOP() { volatile int __iii; __iii = 1; while (__iii); }

#ifndef ASM
#define ASM __asm
#endif

#endif
