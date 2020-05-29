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

#ifndef _H2O_FLASH_LIB_H_
#define _H2O_FLASH_LIB_H_

#include <Uefi.h>
#include <Guid/H2OFlashDevice.h>

typedef enum {
  LpcFlashDeviceType1,
  LpcFlashDeviceType2,
  LpcFlashDeviceType3,
  SpiFlashDeviceType
} H2O_FLASH_DEVICE_TYPE;

#pragma pack(1)

typedef struct _H2O_FLASH_DEVICE_BLOCK {
  UINT16 Size;            ///< size of each block, in 256 byte chunks.
  UINT16 Count;           ///< number of blocks.
} H2O_FLASH_DEVICE_BLOCK;

typedef struct _H2O_FLASH_DEVICE {
  UINT32 Size;            ///< Size of structure.
  VOID   *DeviceTypeData;
  UINT8  DeviceType;      ///< Flash Device Type.
  UINT8  Reserved[3];
  UINT32 Id;              ///< Flash Device Identifier.
  UINT32 ExtId;
  H2O_FLASH_DEVICE_BLOCK BlockMap[1];
} H2O_FLASH_DEVICE;


//
// Format of this structure must match the last part of H2O_FLASH_DEVICE.
//
typedef struct _H2O_PCD_FLASH_DEVICE {
  UINT8  DeviceType;      ///< Flash Device Type.
  UINT8  Reserved[3];     ///< Reserved. Must be 0.

  UINT32 Id;              ///< Flash Device Identifier.
  UINT32 ExtId;
//H2O_FLASH_DEVICE_BLOCK BlockMap[1];
} H2O_PCD_FLASH_DEVICE;
#pragma pack()

/**
  Get target flash device.

  @param[in] AccessMethod       Optional pointer to a GUID that specifies the access method
                                for this flash operation. If NULL, it indicates that the
                                memory-mapped access method (H2O_FLASH_ACCESS_METHOD_MEMORY_MAPPED_GUID)
                                should be used.
  @param[in] PhysDest           Physical address of the first byte of the flash device
  @param[out] FlashDevice       Double ointer to the target flash device structure.

  @retval EFI_SUCCESS           The flash device is recognized succesfully.
  @retval EFI_NOT_FOUND         Cannot find supported flash device.
  @retval EFI_INVALID_PARAMETER Invalid function parameter.
**/
EFI_STATUS
EFIAPI
H2OFlashGetDevice (
  IN CONST EFI_GUID                 *AccessMethod OPTIONAL,
  IN EFI_PHYSICAL_ADDRESS           PhysDest,
  OUT H2O_FLASH_DEVICE              **FlashDevice
  );

/**
  H2OFlashProgram() Library Class function for FdSupportLib

  @param[in] AccessMethod       Optional pointer to a GUID that specifies the access method
                                for this flash operation. If NULL, it indicates that the
                                memory-mapped access method (H2O_FLASH_ACCESS_METHOD_MEMORY_MAPPED_GUID)
                                should be used.
  @param LogicalDest            Pointer to the first byte of the flash device region to be programmed.
                                During boot, this is the same as PhysDest.
                                During runtime, this may differ when the operating system
                                remaps the logical addresses of the flash device.
  @param Src                    Pointer that specifies the buffer containing the bytes to be programmed.
  @param NumBytes               On entry, pointer to the size of the source buffer specified by Src, in bytes.
                                On exit, points to the actual number of bytes programmed.
  @param LbaWriteAddress        Physical address of the first byte of the flash device region to be programmed.
                                During boot, this is the same as Dest.
                                During runtime, it may be different since the operating system
                                may have remapped the flash device.

  @retval EFI_SUCCESS           Data successfully read from flash device
  @retval EFI_UNSUPPORTED       The flash device is not supported
  @retval EFI_DEVICE_ERROR      Failed to erase the blocks

**/
EFI_STATUS
EFIAPI
H2OFlashProgram (
  IN CONST EFI_GUID             *AccessMethod OPTIONAL,
  IN UINT8                      *LogicalDest,
  IN UINT8                      *Src,
  IN UINT64                     *NumBytes,
  IN EFI_PHYSICAL_ADDRESS       PhysDest
  );

/**
  H2OFlashRead() Library Class function for FdSupportLib

  @param Dest                   Pointer to the buffer which will hold the data read from the flash device.
  @param LogicSrc               Pointer to the logical first byte of the flash device to be read.
  @param Count                  Unsigned integer that specifies the number of bytes to be copied.

  @retval EFI_SUCCESS           Data successfully read from flash device
  @retval EFI_UNSUPPORTED       The flash device is not supported
  @retval EFI_DEVICE_ERROR      Failed to erase the blocks

**/
EFI_STATUS
EFIAPI
H2OFlashRead (
  IN CONST EFI_GUID             *AccessMethod OPTIONAL,
  IN UINT8                      *Dest,
  IN UINT8                      *LogicalSrc,
  IN UINT64                     Size
  );

/**
  H2OFlashErase() Library Class function for FdSupportLib

  @param[in] AccessMethod       Optional pointer to a GUID that specifies the access method
                                for this flash operation. If NULL, it indicates that the
                                memory-mapped access method (H2O_FLASH_ACCESS_METHOD_MEMORY_MAPPED_GUID)
                                should be used.
  @param  PhysDest              Physical address of the first byte of the flash device to be erased
  @param  EraseSize             Unsigned integer that specifies the number of bytes to be erased.

  @retval EFI_SUCCESS           Flash device successfully detected
  @retval EFI_UNSUPPORTED       The flash device is not supported
  @retval EFI_DEVICE_ERROR      Failed to erase the blocks

**/
EFI_STATUS
EFIAPI
H2OFlashErase (
  IN CONST EFI_GUID             *AccessMethod OPTIONAL,
  IN  EFI_PHYSICAL_ADDRESS      PhysDest,
  IN  UINT64                    EraseSize
  );


#endif
