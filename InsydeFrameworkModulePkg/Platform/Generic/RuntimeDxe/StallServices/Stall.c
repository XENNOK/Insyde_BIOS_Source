//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
--*/
/*++

Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  Stall.c

Abstract:

Revision History

--*/

#include "Stall.h"

//
// Don't use module globals after the SetVirtualAddress map is singnaled
//
UINT64  mTicksPerMicroseconds;

SAL_RETURN_REGS
EsalStallClassCommonEntry (
  IN  UINT64                                       FunctionId,
  IN  UINT64                                       Arg2,
  IN  UINT64                                       Arg3,
  IN  UINT64                                       Arg4,
  IN  UINT64                                       Arg5,
  IN  UINT64                                       Arg6,
  IN  UINT64                                       Arg7,
  IN  UINT64                                       Arg8,
  IN  SAL_EXTENDED_SAL_PROC                        ExtendedSalProc,
  IN   BOOLEAN                                     VirtualMode,
  IN  VOID                                         *TicksPerMicroseconds
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  FunctionId            - GC_TODO: add argument description
  Arg2                  - GC_TODO: add argument description
  Arg3                  - GC_TODO: add argument description
  Arg4                  - GC_TODO: add argument description
  Arg5                  - GC_TODO: add argument description
  Arg6                  - GC_TODO: add argument description
  Arg7                  - GC_TODO: add argument description
  Arg8                  - GC_TODO: add argument description
  ExtendedSalProc       - GC_TODO: add argument description
  VirtualMode           - GC_TODO: add argument description
  TicksPerMicroseconds  - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
{
  SAL_RETURN_REGS ReturnVal;
  UINT64          ItcTarget;

  ReturnVal.r9  = 0;
  ReturnVal.r10 = 0;
  ReturnVal.r11 = 0;

  switch (FunctionId) {

  case Stall:
    //
    //
    // BugBug : Since there is no guarantee that the OS won't touch ITC,
        /// we need to find another exclusive HW counter or use SW loops. In
    // the short term, we could check for the case where the ITC is update
    // by the OS while in this code to prevent infinite stall.
    //
    ItcTarget = StallReadItc () + ((*(UINT64 *) TicksPerMicroseconds) * Arg2);
    while (StallReadItc () < ItcTarget) {
    }

    ReturnVal.Status = EFI_SUCCESS;
    break;

  default:
    ReturnVal.Status = EFI_SAL_INVALID_ARGUMENT;
  }

  return ReturnVal;
}

EFI_STATUS
StallServicesEsalClassInitialize (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  ImageHandle - GC_TODO: add argument description
  SystemTable - GC_TODO: add argument description

Returns:

  EFI_SUCCESS - GC_TODO: Add description for return value

--*/
{
  EFI_CPU_ARCH_PROTOCOL *CpuArchProtocol;
  EFI_STATUS            Status;
  UINT64                CurrentTicker;
  UINT64                TimerPeriod;

  EfiInitializeRuntimeDriverLib (ImageHandle, SystemTable, NULL);

  Status = gBS->LocateProtocol (&gEfiCpuArchProtocolGuid, NULL, &CpuArchProtocol);
  ASSERT_EFI_ERROR (Status);

  //
  // Get the processor frequency in Hz.
  // BugBug : For IPF, the processor frequency could be different than
  // the ITC frequency. We need to make a PAL call to get the multiplier
  // For IA-32, ITC frequency is same as the processor frequency.
  //
  Status = CpuArchProtocol->GetTimerValue (CpuArchProtocol, 0, &CurrentTicker, &TimerPeriod);
  ASSERT_EFI_ERROR (Status);
  mTicksPerMicroseconds = DivU64x32 (1000000000, (UINTN) TimerPeriod, NULL);

  //
  //  Register the Function with Extended Sal.
  //
  RegisterEsalClass (
    &gEfiExtendedSalStallServicesProtocolGuid,
    &mTicksPerMicroseconds,
    EsalStallClassCommonEntry,
    Stall,
    NULL
    );

  return EFI_SUCCESS;
}
