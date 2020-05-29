/** @file
  Lite version of SeamlessRecoveryLib for PEIMs before memory initialization

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
  Set firmware updating in process signature, not supported if PEI memory is not initialized

  @param  None

  @returns None

**/
VOID
SetFirmwareUpdatingFlag (
  BOOLEAN FirmwareUpdatingFlag
  )
{
  //
  // not supported
  //
  return;
}
