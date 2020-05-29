/** @file
  Header file for Flash Device Support Library Class

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

#ifndef _FD_SUPPORT_LIB_H_
#define _FD_SUPPORT_LIB_H_


#define FLASH_SIZE_128K     0x01
#define FLASH_SIZE_256K     0x02
#define FLASH_SIZE_512K     0x03
#define FLASH_SIZE_1024K    0x04
#define FLASH_SIZE_2048K    0x05
#define FLASH_SIZE_4096K    0x06
#define FLASH_SIZE_8192K    0x07
#define FLASH_SIZE_16384K   0x08
#define MAX_STRING          0x20

#define FD_INDEX_INVALID    0xff

#define ACCESS_NOT_AVAILABLE            0
#define ACCESS_AVAILABLE                1
#define BIOS_REGION_READ_ACCESS         0
#define BIOS_REGION_WRITE_ACCESS        8
#define BIOS_MASTER_READ_ACCESS_GRANT   16
#define BIOS_MASTER_WRITE_ACCESS_GRANT  24

#define FW_DEFAULT_MODE                 0
#define FW_FLASH_MODE                   1

typedef enum {
  LpcFlashType1,
  LpcFlashType2,
  LpcFlashType3,
  SpiFlashType
} FLASH_DEVICE_TYPE;

typedef enum {
  DescRegionType,
  BiosRegionType,
  MeRegionType,
  GbeRegionType,
  PlatformDataRegionType,
  MaxFlashRegionType,
  EndOfRegionType  = 0xff,
} FLASH_REGION_TYPE1;

typedef enum {
  DESC_REGION              = 0,
  BIOS_REGION,
  ME_REGION,
  GBE_REGION,
  PLATFORM_DATA_REGION,
  MAX_FLASH_REGION,
  ME_SUB_REGION_IGNITION   = 0x20,
  REGION_TYPE_OF_EOS       = 0xff,
} FLASH_REGION_TYPE;

#define NUMBER_OF_SUB_REGION    1
#define REGION_EOS              1
#define TOTAL_FLASH_REGION      ( MAX_FLASH_REGION + NUMBER_OF_SUB_REGION + REGION_EOS )

#pragma pack(1)
typedef struct {
  UINT8        Type;
  UINT32       Offset;
  UINT32       Size;
  UINT8        Access;
} FLASH_REGION;

typedef struct {
  UINT16      BlockSize;   //Unit: 1 = 256 bytes
  UINT16      Mutiple;     //flash how many times
  UINT16      EOS;         //end of struct, this word must be 0xffff
} FD_BLOCK_MAP;
#pragma pack()

typedef struct  {
  UINT32                        Id;
  UINT32                        ExtId;
  UINT8                         Size;
  CHAR8                         VendorName[MAX_STRING];
  CHAR8                         DeviceName[MAX_STRING];
  FD_BLOCK_MAP                  BlockMap;
} FD_INFO;

typedef struct {
  UINTN                         IdAddress;
  UINTN                         CmdPort55;
  UINTN                         CmdPort2A;
} LPC_FLASH_DEVICE_INFO;


typedef struct  {
   FLASH_DEVICE_TYPE            DeviceType;
   FD_INFO                      DeviceInfo;
   VOID                         *TypeSpecificInfo;
} FLASH_DEVICE;

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
  );

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
  );

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
  );

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
  );

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
  );

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
  IN  UINTN                     EraseBlockSize
  );

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
  );


/**
  Get the erase block size of the active flash device

  @return    the block size

**/
UINT32
EFIAPI
GetFlashBlockSize (
  VOID
  );

/**
  Initialize Flash mode pointer for library to use.

  @param[in] FlashMode          FlashMode pointer.

  @retval EFI_SUCCESS           Initialize flash mode successful.
**/
EFI_STATUS
EFIAPI
InitFlashMode (
  IN  UINT8     *FlashMode
  );

/**
  Get current flash mode.

  @retval FW_DEFAULT_MODE      Current mode is default mode.
  @retval FW_FLASH_MODE        Current mode is flash mode.
**/
UINT8
GetFlashMode (
  VOID
  );

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
  );

/**
  Get flash number from SPI Descriptor.

  @param[out] FlashNumber       Number of SPI flash devices returned.

  @retval EFI_SUCCESS           Function successfully returned.
  @retval EFI_UNSUPPORTED       The SPI flash is not in Descriptor mode.
**/
EFI_STATUS
GetPlatformSpiFlashNumber (
  OUT   UINT8    *FlashNumber
  );


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
  );

#endif
