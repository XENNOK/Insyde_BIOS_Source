/** @file
  Header file for enabling/disabling Flash Device write access

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

#ifndef _FLASH_WRITE_ENABLE_LIB_H_
#define _FLASH_WRITE_ENABLE_LIB_H_

/**
  Enable flash device write access

  @param  FlashDevice           Pointer to the FLASH_DEVICE structure

  @retval EFI_SUCCESS           Flash device write access was successfully enabled
  @return Others                Failed to enable flash device write access

**/
EFI_STATUS
EFIAPI
FlashWriteEnable (
  FLASH_DEVICE        *FlashDevice
  );

/**
  Disable flash device write access

  @param  FlashDevice           Pointer to the FLASH_DEVICE structure

  @retval EFI_SUCCESS           Flash device write access was successfully disabled
  @return Others                Failed to disable flash device write access

**/
EFI_STATUS
EFIAPI
FlashWriteDisable (
  FLASH_DEVICE        *FlashDevice
  );

#endif
