/** @file
  Library classes for enabling/disabling flash write access

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

#include <PiPei.h>
#include <Library/PeiServicesLib.h>
#include <Library/FdSupportLib.h>
#include <Library/DebugLib.h>
#include <Library/PeiChipsetSvcLib.h>

/**
  Enable flash device write access

  @param  FlashDevice           Pointer to the FLASH_DEVICE structure

  @retval EFI_SUCCESS           Flash device write access was successfully enabled
  @return Others                Failed to enable flash device write access

**/
EFI_STATUS
EFIAPI
FlashWriteEnable (
  IN FLASH_DEVICE               *FlashDevice
  )
{
  return PeiCsSvcEnableFdWrites (TRUE);
}

/**
  Disable flash device write access

  @param  FlashDevice           Pointer to the FLASH_DEVICE structure

  @retval EFI_SUCCESS           Flash device write access was successfully disabled
  @return Others                Failed to disable flash device write access

**/
EFI_STATUS
EFIAPI
FlashWriteDisable (
  IN FLASH_DEVICE               *FlashDevice
  )
{
  return PeiCsSvcEnableFdWrites (FALSE);
}

