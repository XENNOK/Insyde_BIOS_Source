/** @file

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/CommonEcLib.h>

#define ACPI_TIMER_ADDR       (PcdGet16(PcdPerfPkgAcpiIoPortBaseAddress) + 0x08)
#define ACPI_TIMER_MAX_VALUE  0x1000000
#define KBC_TIME_OUT          0x10000

EFI_STATUS
Stall ( 
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
  //
  // Don't use CpuIO PPI for IO port access here, it will result 915
  // platform recovery fail when using the floppy,because the CpuIO PPI is
  // located at the flash.Use the ASM file to replace it.
  //
  OriginalTick  = IoRead32 (ACPI_TIMER_ADDR) & 0x00FFFFFF;

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
  RemainingTick = (UINT32) Ticks % ACPI_TIMER_MAX_VALUE;
  //
  // not intend to use TMROF_STS bit of register PM1_STS, because this adds extra
  // one I/O operation, and maybe generate SMI
  //
  while (Counts != 0) {
    CurrentTick = IoRead32 (ACPI_TIMER_ADDR) & 0x00FFFFFF;

    if (CurrentTick <= OriginalTick) {
      Counts--;
    }

    OriginalTick = CurrentTick;
  }

  while ((RemainingTick > CurrentTick) && (OriginalTick <= CurrentTick)) {
    OriginalTick  = CurrentTick;
    CurrentTick   = (UINT32) IoRead32 (ACPI_TIMER_ADDR) & 0x00FFFFFF;
  }

  return EFI_SUCCESS;
}

/**
  Wait for output buffer full

  @param[in]  CommandState - the Io to read.

  @retval     EFI_SUCCESS - input buffer full.
  
**/
EFI_STATUS
WaitKbcObf (
  IN UINT16              CommandState
)
{
  UINT8                 KbdCmdState;
  UINTN                 Index;
  KbdCmdState = 0;

  for (Index = 0; Index < KBC_TIME_OUT; Index++) {
    KbdCmdState = IoRead8 (CommandState);
    if (KbdCmdState & KEY_OBF) {
      return EFI_SUCCESS;
    } else{
      Stall(15);
    }
  }

  return EFI_DEVICE_ERROR;
}

/**
  Wait for input buffer empty

  @param[in]    CommandState - the Io to read.

  @retval       EFI_SUCCESS - input buffer full.

  
**/
EFI_STATUS
WaitKbcIbe (
  IN UINT16              CommandState
  )
{
  UINT8                 KbdCmdState;
  UINTN                 Index;
  KbdCmdState = 0;
  
  for (Index = 0; Index < KBC_TIME_OUT; Index++) {
    KbdCmdState = IoRead8 (CommandState);
    if (!(KbdCmdState & KEY_IBF)) {
      return EFI_SUCCESS;
    } else{
      Stall(15);
    }
  }

  return EFI_DEVICE_ERROR;
}

/**
  Write data to Kbc data port

  @param[in]  CommandState - the Io to write.
  @param[in]  Data - The data which want write to Kbc data port

  @retval     EFI_SUCCESS - Write data to Kbc port successfully
  
**/
EFI_STATUS
WriteKbc (
  IN UINT16             CommandState,
  IN UINT8              Data
  )
{
  IoWrite8(CommandState, Data);
  return EFI_SUCCESS;
}

/**
  Read data from kbc data port

  @param[in] CommandState - the Io to read.
  @param[in] Data - IN OUT :The data which read from Kbc data port

  @retval EFI_SUCCESS - Read data from Kbc port successfully.
  @retval EFI_INVALID_PARAMETER - The input parameter is invalid.
  
**/
EFI_STATUS
ReadKbc (
  IN UINT16                 CommandState,
  IN OUT UINT8              *Data
  )
{
  if (Data == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  *Data = IoRead8 (CommandState);
  return EFI_SUCCESS;
}

