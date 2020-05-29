/** @file
  Common definitions for LPC Flash Type devices

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

#ifndef _LPC_FLASH_TYPE_SUPPORT_H_
#define _LPC_FLASH_TYPE_SUPPORT_H_


//
// Common Define
//
#define FLASH_BLOCK_SIZE                        0x10000
#define FLASH_SECTOR_SIZE                       0x1000

//
// Define Method One flash layout
//
#define LPC_FLASH_TYPE1_MAGIC_55                 0xFFF05555
#define LPC_FLASH_TYPE1_MAGIC_2A                 0xFFF02AAA
#define LPC_FLASH_TYPE1_ID_ADDRESS1              0xFFBC0000
#define LPC_FLASH_TYPE1_ID_ADDRESS2              0xFFF00000
#define LPC_FLASH_TYPE1_SOFT_ID_EXIT             0xF0
#define LPC_FLASH_TYPE1_DQ6                      0x40
#define LPC_FLASH_TYPE1_TIME_OUT                 50000
#define LPC_FLASH_TYPE1_DQ6                      0x40
#define LPC_FLASH_TYPE1_PROG_BYTE_CMD 	         0xA0
#define LPC_FLASH_TYPE1_BLOCK_CMD                0x50
#define LPC_FLASH_TYPE1_SECTOR_CMD               0x30

//
// Define Method Two flash layout
//
#define LPC_FLASH_TYPE2_BLOCK_ERASE_SETUP_CMD    0x20
#define LPC_FLASH_TYPE2_BLOCK_ERASE_CONFIRM_CMD  0xD0
#define LPC_FLASH_TYPE2_READ_STATUS_CMD          0x70
#define LPC_FLASH_TYPE2_ID_CMD                   0x90
#define LPC_FLASH_TYPE2_CLEAR_STATUS_CMD         0x50
#define LPC_FLASH_TYPE2_READ_ARRAY_CMD           0xFF
#define LPC_FLASH_TYPE2_WRITE_SETUP_CMD          0x40
#define LPC_FLASH_TYPE2_WRITE_STATE_STATUS       (1 << 7)


#define FVB_MAX_RETRY_TIMES                      10000000
#define STALL_TIME                               0x02

#define ACCESS_NOT_AVAILABLE                     0
#define ACCESS_AVAILABLE                         1
#define BIOS_REGION_READ_ACCESS                  0
#define BIOS_REGION_WRITE_ACCESS                 8
#define BIOS_MASTER_READ_ACCESS_GRANT            16
#define BIOS_MASTER_WRITE_ACCESS_GRANT           24


/**
  Flash device Recognize() fuction for LPC Flash Type 1 devices

  @param FlashDevice            pointer to FLASH_DEVICE structure

  @retval EFI_SUCCESS           Flash device successfully recognized
  @retval EFI_UNSUPPORTED       The flash device is not supported

**/
EFI_STATUS
EFIAPI
LpcFlashType1Recognize (
  IN FLASH_DEVICE               *FlashDevice
  );

/**
  Flash device Program() fuction for LPC Flash Type 1 devices

  @param FlashDevice            Pointer to FLASH_DEVICE structure
  @param Dest                   Target address to be programmed
  @param Src                    Souce buffer
  @param NumBytes               Total number of bytes to be programmed
  @param LbaWriteAddress        not used

  @retval EFI_SUCCESS           Data successfully written to flash device
  @retval EFI_UNSUPPORTED       The flash device is not supported
  @retval EFI_DEVICE_ERROR      Failed to write the Flash ROM

**/
EFI_STATUS
EFIAPI
LpcFlashType1Program (
  IN FLASH_DEVICE               *FlashDevice,
  IN UINT8                      *Dest,
  IN UINT8                      *Src,
  IN UINTN                      *NumBytes,
  IN UINTN                      LbaWriteAddress
  );

/**
  Flash device Erase() fuction for LPC Flash Type 1 devices

  @param FlashDevice             pointer to FLASH_DEVICE structure
  @param LbaWriteAddress         Starting address to be erased
  @param EraseSize               Total size in bytes to be erased

  @retval EFI_SUCCESS            Flash ROM erased successfully
  @retval EFI_UNSUPPORTED        The flash device is not supported
  @retval EFI_INVALID_PARAMETER  Invalid function parameters

**/
EFI_STATUS
EFIAPI
LpcFlashType1Erase (
  IN FLASH_DEVICE               *FlashDevice,
  IN  UINTN                     LbaWriteAddress,
  IN  UINTN                     EraseBlockSize
  );


/**
  Flash device Recognize() fuction for LPC Flash Type 2 devices

  @param FlashDevice            pointer to FLASH_DEVICE structure

  @retval EFI_SUCCESS           Flash device successfully recognized
  @retval EFI_UNSUPPORTED       The flash device is not supported

**/
EFI_STATUS
EFIAPI
LpcFlashType2Recognize (
  IN FLASH_DEVICE               *FlashDevice
  );

/**
  Flash device Program() fuction for LPC Flash Type 2 devices

  @param FlashDevice            pointer to FLASH_DEVICE structure
  @param Dest                   Target address to be programmed
  @param Src                    Souce buffer
  @param NumBytes               Total number of bytes to be programmed
  @param LbaWriteAddress        Flash device write address

  @retval EFI_SUCCESS           Data successfully written to flash device
  @retval EFI_UNSUPPORTED       The flash device is not supported
  @retval EFI_DEVICE_ERROR      Failed to write the Flash ROM

**/
EFI_STATUS
EFIAPI
LpcFlashType2Program (
  IN FLASH_DEVICE               *FlashDevice,
  IN UINT8                      *Dest,
  IN UINT8                      *Src,
  IN UINTN                      *NumBytes,
  IN UINTN                      LbaWriteAddress
  );

/**
  Flash device Erase() fuction for LPC Flash Type 2 devices

  @param FlashDevice             pointer to FLASH_DEVICE structure
  @param LbaWriteAddress         Starting address to be erased
  @param EraseSize               Total size in bytes to be erased

  @retval EFI_SUCCESS            Flash ROM erased successfully
  @retval EFI_UNSUPPORTED        The flash device is not supported
  @retval EFI_INVALID_PARAMETER  Invalid function parameters

**/
EFI_STATUS
EFIAPI
LpcFlashType2Erase (
  IN FLASH_DEVICE               *FlashDevice,
  IN  UINTN                     LbaWriteAddress,
  IN  UINTN                     EraseBlockSize
  );

  

#endif
