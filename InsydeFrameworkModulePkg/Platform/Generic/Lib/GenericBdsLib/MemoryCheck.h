//;******************************************************************************
//;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++


Module Name:

  MemoryCheck.h

Abstract:

  Header file for check memory consistency realted functions

--*/
#ifndef _MEMORY_CHECK_H_
#define _MEMORY_CHECK_H_
#include "GenericBdsLib.h"

typedef struct {
  UINTN                     Signature;
  EFI_LIST_ENTRY            Link;
  EFI_PHYSICAL_ADDRESS      MemStart;
  EFI_PHYSICAL_ADDRESS      MemEnd;
} SKIP_MEMORY_RANGE;

#pragma pack (1)
typedef struct {
  EFI_PHYSICAL_ADDRESS      MemStart;
  EFI_PHYSICAL_ADDRESS      MemEnd;
} MEMORY_RANGE;
#pragma pack ()

#define SKIP_MEMORY_RANGE_SIGNATURE EFI_SIGNATURE_32 ('S', 'M', 'R', 'S')
#define SKIP_MEMORY_RANGE_FROM_THIS(a)               CR(a, SKIP_MEMORY_RANGE, Link, SKIP_MEMORY_RANGE_SIGNATURE)
#define MAX_ADJUST_MEMORY_TIMES                      3

#ifdef MEMORY_MAP_CONSISTENCY_CHECK
VOID
CheckRtAndBsMemUsage (
  VOID
  )
/*++

Routine Description:

  Check the usage size of each runtime and boot services memory type.
  If the usage size exceeds the default size, adjust memory type information automatically.
  And then reset system.

Arguments:

Returns:

--*/
;
#endif

#ifdef EFI_DEBUG
VOID
DumpMemoryMap (
  VOID
  )
/*++

Routine Description:

  Dump memory information if EFI_DEBUG is enabled.

Arguments:

Returns:

--*/
;
#endif
#endif
