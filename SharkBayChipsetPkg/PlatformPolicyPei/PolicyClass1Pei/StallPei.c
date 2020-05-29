//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Module Name :
//;
//;   StallPEI.c
//;

#include "StallPei.h"

static EFI_PEI_STALL_PPI            mStallPpi = {
  PEI_STALL_RESOLUTION,
  StallPpiFunc
};

static EFI_PEI_PPI_DESCRIPTOR   mPpiList[1] = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiStallPpiGuid,
  &mStallPpi
};

EFI_STATUS
StallPeiEntry (
  IN      EFI_PEI_FILE_HANDLE         FileHandle,
  IN      CONST EFI_PEI_SERVICES      **PeiServices
  )
{
  EFI_STATUS                        Status;

  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "\nStall PEI Entry\n"));

  Status = (**PeiServices).InstallPpi (PeiServices, &mPpiList[0]);
  ASSERT_EFI_ERROR (Status);

  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "Stall PEI Exit\n"));

  return Status;
}

EFI_STATUS
EFIAPI
StallPpiFunc (
  IN CONST EFI_PEI_SERVICES           **PeiServices,
  IN CONST EFI_PEI_STALL_PPI          *This,
  IN UINTN                            Microseconds
  )
/*++

Routine Description:

  Waits for at least the given number of microseconds.

Arguments:

  PeiServices           - The PEI core services table.
  This                  - PPI instance structure.
  Microseconds          - Desired length of time to wait

Returns:

  EFI_SUCCESS           - If the desired amount of time passed.
  Others                - If error occurs while locating CpuIoPpi.

--*/
{
  UINTN   Ticks;
  UINTN   Counts;
  UINT32  CurrentTick;
  UINT32  OriginalTick;
  UINT32  RemainingTick;

  if (Microseconds == 0) {
    return EFI_SUCCESS;
  }

  OriginalTick  = IoRead32 ( ACPI_TIMER_ADDR ) & 0x00FFFFFF;

  CurrentTick   = OriginalTick;

  //
  // The timer frequency is 3.579545 MHz, so 1 ms corresponds 3.58 clocks
  //
  Ticks = Microseconds * 358 / 100 + OriginalTick + 1;
  //
  // The loops needed by timer overflow
  //
  Counts = Ticks / ACPI_TIMER_MAX_VALUE;
  //
  // remaining clocks within one loop
  //
  RemainingTick = Ticks % ACPI_TIMER_MAX_VALUE;
  //
  // not intend to use TMROF_STS bit of register PM1_STS, because this adds extra
  // one I/O operation, and maybe generate SMI
  //
  while (Counts != 0) {
    CurrentTick = IoRead32 ( ACPI_TIMER_ADDR ) & 0x00FFFFFF;

    if (CurrentTick <= OriginalTick) {
      Counts--;
    }

    OriginalTick = CurrentTick;
  }

  while ((RemainingTick > CurrentTick) && (OriginalTick <= CurrentTick)) {
    OriginalTick  = CurrentTick;
    CurrentTick   = (IoRead32 ( ACPI_TIMER_ADDR ) & 0x00FFFFFF);
  }

  return EFI_SUCCESS;
}

