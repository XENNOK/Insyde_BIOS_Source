/** @file
  Provide stall function.

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

#include <Ppi/Stall.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>

#define ACPI_TIMER_ADDR       (FixedPcdGet16(PcdPerfPkgAcpiIoPortBaseAddress) + 0x08)
#define ACPI_TIMER_MAX_VALUE  0x1000000

/**
  Waits for at least the given number of microseconds.

  @param[in]  PeiServices           - The PEI core services table.
  @param[in]  This                  - PPI instance structure.
  @param[in]  Microseconds          - Desired length of time to wait
  
  @retval   EFI_SUCCESS           - If the desired amount of time passed.
  @retval   Others                - If error occurs while locating CpuIoPpi.

**/
EFI_STATUS
EFIAPI
StallPpiFunc (
  IN EFI_PEI_SERVICES   **PeiServices,
  IN EFI_PEI_STALL_PPI  *This,
  IN UINTN              Microseconds
  )
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
