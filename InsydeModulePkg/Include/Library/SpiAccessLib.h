/** @file
  Header file for SPI Flash Device Support Library Class

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

#ifndef _SPI_ACCESS_LIB_H_
#define _SPI_ACCESS_LIB_H_

#include <Uefi.h>
#include <Library/FdSupportLib.h>

//
// Type of operations
//
#define NAR                             0x00
#define NAW                             0x01
#define ADR                             0x02
#define ADW                             0x03

#define ID_COMMON_SPI_DEVICE            0x12345678

#pragma pack(1)
//
// Flash config block definition for SPI
//
typedef struct
{
  UINT8   OpCodeMenu[8];
  UINT16  OpType;
  UINT8   PrefixMenu[2];
  UINT32  GlobalProtect : 1;
  UINT32  BlockProtect : 1;
  UINT32  BlockProtectDataRequired : 1;
  UINT32  ProgramGranularity : 1;
  UINT32  FlashIDSize : 3;
  UINT32  MinBytesPerOp : 3;
  UINT32  NVStatusBit : 1;
  UINT32  Reserve : 21;
  UINT8   GlobalProtectCode;
  UINT8   GlobalUnprotectCode;
  UINT8   BlockProtectCode;
  UINT8   BlockUnprotectCode;
  UINT32  DeviceSize;
  UINT32  BlockEraseSize;
  UINT32  BlockProtectSize;
} SPI_CONFIG_BLOCK;
#pragma pack()

/**
  Detect and Initialize SPI flash part OpCode and other parameter through PCH

  @param FlashDevice            pointer to FLASH_DEVICE structure

  @retval EFI_SUCCESS           The SPI device was successfully recognized
  @retval EFI_UNSUPPORTED       The flash device is not supported by this function
  @retval EFI_DEVICE_ERROR      Failed to Recoginize the SPI device

**/
EFI_STATUS
EFIAPI
SpiRecognize (
  IN FLASH_DEVICE               *FlashDevice
  );

/**
  Write the SPI flash device with given address and size through PCH

  @param FlashDevice            pointer to FLASH_DEVICE structure
  @param FlashAddress           Destination Offset
  @param SrcAddress             Source Offset
  @param SPIBufferSize          The size for programming
  @param LbaWriteAddress        Write Address

  @retval EFI_SUCCESS           The SPI device was successfully recognized
  @retval EFI_UNSUPPORTED       The flash device is not supported by this function
  @retval EFI_DEVICE_ERROR      Failed to erase the target address

**/
EFI_STATUS
EFIAPI
SpiProgram (
  IN FLASH_DEVICE               *FlashDevice,
  IN UINT8                      *Dest,
  IN UINT8                      *Src,
  IN UINTN                      *NumBytes,
  IN UINTN                      LbaWriteAddress
  );

/**
  Read the SPI flash device with given address and size through PCH

  @param FlashDevice            pointer to FLASH_DEVICE structure
  @param DstAddress             Destination buffer address
  @param FlashAddress           The flash device address to be read
  @param BufferSize             The size to be read

  @retval EFI_SUCCESS           The SPI device was successfully recognized
  @retval EFI_UNSUPPORTED       The flash device is not supported by this function
  @retval EFI_DEVICE_ERROR      Failed to erase the target address

**/
EFI_STATUS
EFIAPI
SpiRead (
  IN FLASH_DEVICE               *FlashDevice,
  IN UINT8                      *Dest,
  IN UINT8                      *Src,
  IN UINTN                      Count
  );

/**
  Erase the SPI flash device from LbaWriteAddress through PCH

  @param FlashDevice            pointer to FLASH_DEVICE structure
  @param FlashAddress           Target address to be erased
  @param Size                   The size in bytes to be erased

  @retval EFI_SUCCESS           The SPI device was successfully recognized
  @retval EFI_UNSUPPORTED       The flash device is not supported by this function
  @retval EFI_DEVICE_ERROR      Failed to erase the target address

**/
EFI_STATUS
EFIAPI
SpiErase (
  IN  FLASH_DEVICE              *FlashDevice,
  IN  UINTN                     LbaWriteAddress,
  IN  UINTN                     EraseBlockSize
  );

/**
  Get flash number from SPI Descriptor.

  @param[out] FlashNumber       Number of SPI flash devices returned.

  @retval EFI_SUCCESS           Function successfully returned.
  @retval EFI_UNSUPPORTED       The SPI flash is not in Descriptor mode.
**/
EFI_STATUS
GetSpiFlashNumber (
  OUT   UINT8    *FlashNumber
  );

/**
  Offset the BIOS address from top of 4G memory address to correct BIOS region
  described in SPI Descriptor.

  @param[in]  MemoryAddress     BIOS memory mapped address.
  @param[out] BiosRegionAddress Flash ROM start address + BIOS address in flash ROM.

  @retval EFI_SUCCESS           Function successfully returned.
  @retval EFI_UNSUPPORTED       The SPI flash is not in Descriptor mode.
**/
EFI_STATUS
MemoryToBiosRegionAddress (
  IN UINTN       MemoryAddress,
  OUT UINTN      *BiosRegionAddress
  );

/**
  Get flash table from platform.

  @param[in, out] DataBuffer    IN: the input buffer address.
                                OUT:the flash region table from rom file.

  @retval EFI_SUCCESS           Function successfully returned.
**/
EFI_STATUS
GetSpiPlatformFlashTable (
  IN OUT   UINT8    *DataBuffer
  );

#endif
