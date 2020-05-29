//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#include "Tiano.h"
#include "EfiRuntimeLib.h"
#include "FdSupportLib.h"
#include "FWH_LPC.h"

EFI_STATUS
ProgramMethodOne (
  IN UINT8                      *Dest,
  IN UINT8                      *Src,
  IN UINTN                      *NumBytes,
  IN UINTN                      LbaWriteAddress
  )
/*++

Routine Description:

  Write the flash device with given address and size

Arguments:

  Dest                          Destination Offset
  Src                           Source Offset
  Count                         The size for programming
  LbaWriteAddress               Write Address

Returns:

  EFI status

--*/
{
  EFI_STATUS                    Status;
  UINTN                         Count;
  UINT8                         Toggle1;
  UINT8                         Toggle2;
  UINTN                         TimeOut;

  Status = EFI_SUCCESS;

  for (Count = 0; Count < *NumBytes; Count++, Dest++, Src++) {
    if (EfiCompareMem ((VOID *)(UINTN)Dest, (VOID *)(UINTN)Src, 1) != 0) {

      //
      // Program command
      //
      EfiSetMem ((VOID *)mMethodOne->CmdPort55, 1, 0xAA);
      MEMORY_FENCE ();

      EfiSetMem ((VOID *)mMethodOne->CmdPort2A, 1, 0x55);
      MEMORY_FENCE ();

      EfiSetMem ((VOID *)mMethodOne->CmdPort55, 1, 0xA0);
      MEMORY_FENCE ();

      //
      // Write Data to flash parts.
      //
      EfiSetMem ((VOID *)Dest, 1, *Src);
      MEMORY_FENCE ();

      Stall (STALL_TIME);

      for (TimeOut = 0; TimeOut < EFI_METHOD_ONE_TIME_OUT; TimeOut++) {
        Toggle1 = (*(UINT8*)(UINTN)(Dest));
        Stall (STALL_TIME);
        Toggle2=(*(UINT8*)(UINTN)(Dest));

        if (Toggle1 == Toggle2) {
          break;
        }
        if (TimeOut >= (EFI_METHOD_ONE_TIME_OUT - 1)) {
          *NumBytes = Count;
          Status = EFI_DEVICE_ERROR;
          break;
        }
      }

      Toggle1 = (*(UINT8*)(UINTN)(Dest));
      Toggle2 = (*(UINT8*)(UINTN)(Src));
      if (Toggle1 != Toggle2) {
        *NumBytes = Count;
        Status = EFI_DEVICE_ERROR;
        break;
      }
    }
  }

  return Status;
}

EFI_STATUS
ProgramMethodTwo (
  IN UINT8                      *Dest,
  IN UINT8                      *Src,
  IN UINTN                      *NumBytes,
  IN UINTN                      LbaWriteAddress
  )
/*++

Routine Description:

  Write the flash device with given address and size

Arguments:

  Dest                          Destination Offset
  Src                           Source Offset
  Count                         The size for programming
  LbaWriteAddress               Write Address

Returns:

  EFI status

--*/
{
  EFI_STATUS                    Status;
  UINTN                         Count;
  UINT8                         HubData;
  UINTN                         RetryTimes;

  Status = EFI_SUCCESS;

  for (Count = 0; Count < *NumBytes; Count++, Dest++, Src++) {

    EfiSetMem ((VOID *)Dest, 1, FWH_WRITE_SETUP_COMMAND);
    EfiSetMem ((VOID *)Dest, 1, *Src);
    EfiSetMem ((VOID *)Dest, 1, FWH_READ_STATUS_COMMAND);

    //
    // Device error if time out occurs
    //
    RetryTimes = 0;
    while (RetryTimes < FVB_MAX_RETRY_TIMES) {
      HubData = *(UINT8*)Dest;
      if (HubData & FWH_WRITE_STATE_STATUS) {
        break;
      }

      Stall (STALL_TIME);
      RetryTimes += STALL_TIME;
    }

    if (RetryTimes >= FVB_MAX_RETRY_TIMES) {
      *NumBytes = Count;
      Status    = EFI_DEVICE_ERROR;
      break;
    }
  }
  //
  // Clear status register
  //
  EfiSetMem ((VOID *)LbaWriteAddress, 1, FWH_CLEAR_STATUS_COMMAND);

  //
  // Issue read array command to return the FWH state machine to the
  // normal operational state
  //
  EfiSetMem ((VOID *)LbaWriteAddress, 1, FWH_READ_ARRAY_COMMAND);

  return Status;
}
