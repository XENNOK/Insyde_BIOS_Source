/** @file
  This driver provides IHISI interface in SMM mode

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "FetsOemHook.h"
//[-start-130125-IB10820224-remove]//
#if 0
/**
  Get EC part information hook function.

  @retval EFI_SUCCESS    Successfully returns.
**/
EFI_STATUS
FetsGetPartInfoHook (
  VOID
  )
{
  //
  // Return Function Not Supported
  //
  IhisiLibErrorCodeHandler (IhisiFunNotSupported);

  return EFI_SUCCESS;
}
#endif
//[-end-130125-IB10820224-remove]//
/**
  Hook function before flashing EC part.

  @param[in, out] FlashingDataBuffer Double pointer to data buffer.
  @param[in, out] SizeToFlash        Data size by bytes want to flash.
  @param[in, out] DestBlockNo        Dsstination block number.

  @retval EFI_SUCCESS                Successfully returns.
**/
EFI_STATUS
FetsDoBeforeFlashing (
  IN OUT  UINT8        **FlashingDataBuffer,
  IN OUT  UINTN        *SizeToFlash,
  IN OUT  UINT8        *DestBlockNo
  )
{
//[-start-130125-IB10820224-add]//
  EFI_STATUS         Status;

//[-start-130524-IB05160451-modify]//
  Status = OemSvcIhisiS20HookFetsDoBeforeFlashing (FlashingDataBuffer, SizeToFlash, DestBlockNo);
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "SmmOemSvcChipsetLib OemSvcIhisiS20HookFetsDoBeforeFlashing, Status : %r\n", Status));
//[-end-130524-IB05160451-modify]//
  if (Status == EFI_SUCCESS) {
    return EFI_SUCCESS;
  }
//[-end-130125-IB10820224-modify]//
  FetsEcIdle (TRUE);

  return EFI_SUCCESS;
}

/**
  Hook function after flashing EC part.

  @param[in] ActionAfterFlashing Input action flag.

  @retval EFI_SUCCESS            Successfully returns.
**/
EFI_STATUS
FetsDoAfterFlashing (
  IN      UINT8        ActionAfterFlashing
  )
{
//[-start-130125-IB10820224-modify]//
  EFI_STATUS        Status;

//[-start-130524-IB05160451-modify]//
  Status = OemSvcIhisiS20HookFetsDoAfterFlashing (ActionAfterFlashing);
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "SmmOemSvcChipsetLib OemSvcIhisiS20HookFetsDoAfterFlashing, Status : %r\n", Status));
//[-end-130524-IB05160451-modify]//
  if (Status == EFI_SUCCESS) {
    return EFI_SUCCESS;
  }
//[-end-130125-IB10820224-modify]//
  switch (ActionAfterFlashing) {
    case EcFlashDosReboot:
      FetsEcIdle (FALSE);

      FetsReboot ();
      break;

    case EcFlashDoshutdown:
      FetsEcIdle (FALSE);

      FetsShutdown ();
      break;

    case EcFlashOSShutdown:
    case EcFlashOSReboot:
      FetsEcIdle (FALSE);
      break;

    case EcFlashDoNothing:
      FetsEcIdle (FALSE);
      break;

    case EcFlashContinueToFlash:
    default:
      break;
  }

  return EFI_SUCCESS;
}

/**
  Fets reset system function.

  @retval EFI_SUCCESS   Successfully returns.
**/
EFI_STATUS
FetsReboot (
  VOID
  )
{
  UINT8        Buffer;

  Buffer = 0;

//  FetsEcIdle ( FALSE );

  Buffer = V_RST_CNT_FULLRESET;
  gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT8, RST_CNT, 1, &Buffer );
  return EFI_SUCCESS;
}

/**
  Fets shutdown function.

  @retval EFI_SUCCESS   Successfully returns.
**/
EFI_STATUS
FetsShutdown (
  VOID
  )
{
  FbtsShutDown ();

  return EFI_SUCCESS;
}

/**
  Fets EC idle function

  @retval EFI_SUCCESS   Successfully returns.
**/
EFI_STATUS
FetsEcIdle (
  IN      BOOLEAN        Idle
  )
{
//[-start-130125-IB10820224-modify]//
  EFI_STATUS        Status;

//[-start-130524-IB05160451-modify]//
  Status = OemSvcIhisiS20HookFetsEcIdle (Idle);
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "SmmOemSvcChipsetLib OemSvcIhisiS20HookFetsEcIdle , Status : %r\n", Status));
//[-end-130524-IB05160451-modify]//
//[-end-130125-IB10820224-modify]//
  return EFI_SUCCESS;
}

/**
  Function uses to flash EC part.

  @retval EFI_SUCCESS   Successfully returns.
**/
EFI_STATUS
FetsEcFlash (
  IN      UINT8        *FlashingDataBuffer,
  IN      UINTN        SizeToFlash,
  IN      UINT8        DestBlockNo
  )
{
//[-start-130125-IB10820224-modify]//
  EFI_STATUS        Status;

  Status = OemSvcIhisiS20HookFetsEcFlash  (FlashingDataBuffer, SizeToFlash, DestBlockNo);
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "SmmOemSvcChipsetLib OemSvcIhisiS20HookFetsEcFlash , Status : %r\n", Status));
//[-end-130125-IB10820224-modify]//
  return EFI_SUCCESS;
}
