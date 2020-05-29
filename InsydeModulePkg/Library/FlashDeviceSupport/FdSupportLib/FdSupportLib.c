/** @file
  FdSupportLib Library Instance implementation

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/FdSupportLib.h>
#include <Library/SpiAccessLib.h>
#include <Library/FlashWriteEnableLib.h>
#include <Library/IrsiRegistrationLib.h>
#include "LpcFlashSupport.h"

FLASH_DEVICE                   *mFlashDevice = NULL;
UINT8                          *mFlashMode;
/**
  Set target flash device

  @param FlashDevice            Pointer to the target flash device structure

  @retval EFI_SUCCESS           The flash device is recognized succesfully
  @retval EFI_INVALID_PARAMETER Invalid function parameter

**/
EFI_STATUS
EFIAPI
SetFlashDevice (
  FLASH_DEVICE                  *FlashDevice
  )
{
  if (FlashDevice == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  mFlashDevice = FlashDevice;
  return EFI_SUCCESS;
}

/**
  Get target flash device.

  @param[out] FlashDevice       Double ointer to the target flash device structure.

  @retval EFI_SUCCESS           The flash device is recognized succesfully.
  @retval EFI_NOT_FOUND         Cannot find supported flash device.
  @retval EFI_INVALID_PARAMETER Invalid function parameter.
**/
EFI_STATUS
EFIAPI
GetFlashDevice (
  FLASH_DEVICE                  **FlashDevice
  )
{
  if (FlashDevice == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (mFlashDevice == NULL) {
    return EFI_NOT_FOUND;
  }

  *FlashDevice = mFlashDevice;
  return EFI_SUCCESS;
}


/**
  FlashRecognize() Library Class function for FdSupportLib

  @param None

  @retval EFI_SUCCESS           The flash device is recognized succesfully
  @retval EFI_UNSUPPORTED       The flash device is not supported

**/
EFI_STATUS
EFIAPI
FlashRecognize (
  VOID
  )
{
  EFI_STATUS                    Status;

  ASSERT (mFlashDevice != NULL);
  switch (mFlashDevice->DeviceType) {
    case LpcFlashType1:
      Status = LpcFlashType1Recognize(mFlashDevice);
      break;
    case LpcFlashType2:
      Status = LpcFlashType2Recognize(mFlashDevice);
      break;
    case SpiFlashType:
      Status = SpiRecognize(mFlashDevice);
      break;
    default:
      Status = EFI_UNSUPPORTED;
      break;
  }

  return Status;
}

/**
  FlashProgram() Library Class function for FdSupportLib

  @param Dest                   The destination buffer
  @param Src                    Number of blocks to be erased
  @param NumBytes               Pointer to a UINTN. At entry, *NumBytes contains
                                the total size of the buffer. At exit, *NumBytes
                                contains the total number of bytes read.
  @param LbaWriteAddress        Target address to be programmed

  @retval EFI_SUCCESS           Data successfully read from flash device
  @retval EFI_UNSUPPORTED       The flash device is not supported
  @retval EFI_DEVICE_ERROR      Failed to erase the blocks

**/
EFI_STATUS
EFIAPI
FlashProgram (
  IN UINT8                      *Dest,
  IN UINT8                      *Src,
  IN UINTN                      *NumBytes,
  IN UINTN                      LbaWriteAddress
  )
{
  EFI_STATUS                    Status;

  ASSERT (mFlashDevice != NULL);
  Status = FlashWriteEnable (mFlashDevice);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  switch (mFlashDevice->DeviceType) {
    case LpcFlashType1:
      Status = LpcFlashType1Program (
                  mFlashDevice,
                  Dest,
                  Src,
                  NumBytes,
                  LbaWriteAddress
                  );
      break;
    case LpcFlashType2:
      Status = LpcFlashType2Program (
                  mFlashDevice,
                  Dest,
                  Src,
                  NumBytes,
                  LbaWriteAddress
                  );
      break;
    case SpiFlashType:
      Status = SpiProgram (
                  mFlashDevice,
                  Dest,
                  Src,
                  NumBytes,
                  LbaWriteAddress
                  );
      break;
    default:
      Status = EFI_UNSUPPORTED;
      break;
  }
  FlashWriteDisable (mFlashDevice);

  return Status;
}

/**
  FlashRead() Library Class function for FdSupportLib

  @param Dest                   Destination buffer address to be copied to
  @param Src                    Source address of the flash device to be read from
  @param Count                  Number of bytes to be copied

  @retval EFI_SUCCESS           Data successfully read from flash device
  @retval EFI_UNSUPPORTED       The flash device is not supported
  @retval EFI_DEVICE_ERROR      Failed to erase the blocks

**/
EFI_STATUS
EFIAPI
FlashRead (
  IN UINT8                      *Dest,
  IN UINT8                      *Src,
  IN UINTN                      Count
  )
{
  ASSERT (mFlashDevice != NULL);

  if (mFlashDevice->DeviceType == SpiFlashType) {
    return SpiRead(mFlashDevice, Dest, Src, Count);
  } else if (mFlashDevice->DeviceType == LpcFlashType1 || mFlashDevice->DeviceType == LpcFlashType2) {
    CopyMem (Dest, Src, Count);
    return EFI_SUCCESS;
  }
  return EFI_UNSUPPORTED;
}

/**
  FlashErase() Library Class function for FdSupportLib

  @param LbaWriteAddress        Starting LBA address of the flash device to be erased
  @param  EraseSize             Number of bytes to be erased

  @retval EFI_SUCCESS           Flash device successfully detected
  @retval EFI_UNSUPPORTED       The flash device is not supported
  @retval EFI_DEVICE_ERROR      Failed to erase the blocks

**/
EFI_STATUS
EFIAPI
FlashErase (
  IN  UINTN                     LbaWriteAddress,
  IN  UINTN                     EraseSize
  )
{
  EFI_STATUS                    Status;

  ASSERT (mFlashDevice != NULL);

  Status = FlashWriteEnable (mFlashDevice);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  switch (mFlashDevice->DeviceType) {
    case LpcFlashType1:
      Status = LpcFlashType1Erase(mFlashDevice, LbaWriteAddress, EraseSize);
      break;
    case LpcFlashType2:
      Status = LpcFlashType2Erase(mFlashDevice, LbaWriteAddress, EraseSize);
      break;
    case SpiFlashType:
      Status = SpiErase(mFlashDevice, LbaWriteAddress, EraseSize);
      break;
    default:
      Status = EFI_UNSUPPORTED;
      break;
  }
  FlashWriteDisable (mFlashDevice);
  return Status;
}

/**
  Flash device detection

  @param FlashDevice            Double pointer to FLASH_DEVICE structure
  @param DeviceCount            Total number of supported flash devices

  @retval EFI_SUCCESS           Flash device successfully detected
  @retval EFI_NOT_FOUND         No supported flash device found

**/
EFI_STATUS
EFIAPI
DetectFlashDevice (
  FLASH_DEVICE  **SupportedDevices,
  UINTN         DeviceCount
  )
{
  EFI_STATUS    Status;
  UINTN         Index;

  for (Index = 0; Index < DeviceCount; Index++ ) {
    SetFlashDevice (SupportedDevices[Index]);
    Status = FlashRecognize();
    if (Status == EFI_SUCCESS) {
      IrsiAddVirtualPointer ((VOID **)&mFlashDevice);
      return EFI_SUCCESS;
    }
  }
  return EFI_NOT_FOUND;
}

/**
  Get the erase block size of the active flash device

  @return    the block size

**/
UINT32
EFIAPI
GetFlashBlockSize (
  VOID
  )
{
   return (UINT32)mFlashDevice->DeviceInfo.BlockMap.BlockSize * 0x100;
}


/**
  Initialize Flash mode pointer for library to use.

  @param[in] FlashMode          FlashMode pointer.

  @retval EFI_SUCCESS           Initialize flash mode successful.
**/
EFI_STATUS
EFIAPI
InitFlashMode (
  IN  UINT8     *FlashMode
  )
{
  mFlashMode = FlashMode;
  return EFI_SUCCESS;
}

/**
  Get current flash mode.

  @retval FW_DEFAULT_MODE      Current mode is default mode.
  @retval FW_FLASH_MODE        Current mode is flash mode.
**/
UINT8
GetFlashMode (
  VOID
  )
{
  return mFlashMode == NULL ? FW_DEFAULT_MODE : *mFlashMode;
}

/**
  Get flash table from platform.

  @param[in, out] DataBuffer    IN: the input buffer address.
                                OUT:the flash region table from rom file.

  @retval EFI_SUCCESS           Get flash table successful.
  @retval EFI_UNSUPPORTED       Flash device doesn't support this feature.
**/
EFI_STATUS
GetPlatformFlashTable (
  IN OUT   UINT8    *DataBuffer
  )
{
  if (mFlashDevice->DeviceType == SpiFlashType) {
    return GetSpiPlatformFlashTable (DataBuffer);
  }
  return EFI_UNSUPPORTED;
}

/**
  Get flash number from SPI Descriptor.

  @param[out] FlashNumber       Number of SPI flash devices returned.

  @retval EFI_SUCCESS           Function successfully returned.
  @retval EFI_UNSUPPORTED       The SPI flash is not in Descriptor mode.
**/
EFI_STATUS
GetPlatformSpiFlashNumber (
  OUT   UINT8    *FlashNumber
  )
{
  if (mFlashDevice->DeviceType == SpiFlashType) {
    return GetSpiFlashNumber (FlashNumber);
  }
  return EFI_UNSUPPORTED;
}

/**
  Offset the BIOS address from top of 4G memory address to correct BIOS region
  described in SPI Descriptor.

  @param[in] MemoryAddress      BIOS memory mapped address.
  @param[out] BiosRegionAddress Flash ROM start address + BIOS address in flash ROM.

  @retval EFI_SUCCESS           Function successfully returned.
  @retval EFI_UNSUPPORTED       The SPI flash is not in Descriptor mode.
**/
EFI_STATUS
ConvertToSpiAddress (
  IN  UINTN       MemoryAddress,
  OUT UINTN      *BiosRegionAddress
  )
{
  if (mFlashDevice->DeviceType == SpiFlashType) {
    return MemoryToBiosRegionAddress(MemoryAddress, BiosRegionAddress);
  }
  return EFI_UNSUPPORTED;
}

/**
  FdSupportLib Library Class Constructor

  @param  None

  @retval EFI_SUCCESS:   Module initialized successfully
  @retval Others     :   Module initialization failed

**/
EFI_STATUS
EFIAPI
FdSupportLibInit (
  VOID
  )
{
  IrsiAddVirtualPointer ((VOID **) &mFlashDevice);
  return EFI_SUCCESS;
}

/**
  FdSupportLib Library Class destructor

  @retval EFI_SUCCESS The library shutdown successfully.
**/
EFI_STATUS
EFIAPI
FdSupportLibDestruct (
  VOID
  )
{
  IrsiRemoveVirtualPointer ((VOID **) &mFlashDevice);
  return EFI_SUCCESS;
}



