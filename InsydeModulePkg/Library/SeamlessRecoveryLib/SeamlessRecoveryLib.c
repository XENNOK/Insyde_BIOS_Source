/** @file
   Seamless Recovery support library for Windows UEFI Firmware Update Platform

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
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/FdSupportLib.h>

/**
  Check whether it is in firmware failure recovery mode

  @param  None

  @retval TRUE      The system is in firmware failure recovery mode
          FALSE     The system is not in firmware failure recovery mode

**/
BOOLEAN
IsFirmwareFailureRecovery (
  VOID
  )
{
  EFI_GUID *RecoverySig;

  RecoverySig = (EFI_GUID *)(UINTN)PcdGet32(PcdFlashNvStorageFtwSpareBase);
  if ( CompareGuid( PcdGetPtr(PcdSeamlessRecoverySignature), RecoverySig) ) {
    return TRUE;
  }
  return FALSE;
}

/**
  Set firmware updating in process signature

  @param  None

  @returns None

**/
VOID
SetFirmwareUpdatingFlag (
  BOOLEAN FirmwareUpdatingFlag
  )
{
  EFI_STATUS  Status;
  UINTN       Size;

  if ( (!FirmwareUpdatingFlag) && (!IsFirmwareFailureRecovery()) ) {
    return;
  }

  Status = FlashErase(PcdGet32(PcdFlashNvStorageFtwSpareBase), GetFlashBlockSize());
  if (EFI_ERROR(Status)) {
    ASSERT_EFI_ERROR(Status);
    return;
  }
  if (FirmwareUpdatingFlag) {
    Size = sizeof(EFI_GUID);
    FlashProgram (
      (UINT8 *)(UINTN)PcdGet32(PcdFlashNvStorageFtwSpareBase),
      PcdGetPtr(PcdSeamlessRecoverySignature),
      &Size, PcdGet32(PcdFlashNvStorageFtwSpareBase)
      );
  }
}