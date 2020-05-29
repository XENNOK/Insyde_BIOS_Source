/** @file
  SPI Access Init routines

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

#include <Uefi.h>

/**
  Detect whether the system is at EFI runtime or not

  @param  None

  @retval FALSE                 System is not at EFI runtime

**/
BOOLEAN
EFIAPI
AtRuntime (
  VOID
  )
{
  return FALSE;
}

/**
  Initialization routine for SpiAccessLib

  @param  None

  @retval EFI_SUCCESS           SpiAccessLib successfully initialized

**/
EFI_STATUS
EFIAPI
SpiAccessInit (
  VOID
  )
{
  return EFI_SUCCESS;
}

/**
  This routine uses to free the allocated resource by SpiAccessInit ().

  @retval EFI_SUCCESS    Free allocated resource successful.
  @return Others         Free allocated resource failed.
**/
EFI_STATUS
EFIAPI
SpiAccessDestroy (
  VOID
  )
{
  return EFI_SUCCESS;
}