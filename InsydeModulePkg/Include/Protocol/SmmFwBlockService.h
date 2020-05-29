/** @file

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

#ifndef _SMM_FW_BLOCK_SERVICE_PROTO_H_
#define _SMM_FW_BLOCK_SERVICE_PROTO_H_

#define EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL_GUID \
  { \
    0x2970687c, 0x618c, 0x4de5, 0xb8, 0xf9, 0x6c, 0x75, 0x76, 0xdc, 0xa8, 0x3d \
  }

typedef struct  _EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL;



#define SMM_FW_FLASH_MODE       0x01
#define SMM_FW_DEFAULT_MODE     0x00



/**
  Detect target flash device and copy whole flash device to output buffer.

  @param[in]  This               Pointer to EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL instance.
  @param[out] SystemTable        Buffer to saved target flash device information

  @retval EFI_SUCCESS           The flash device is recognized successfully.
  @retval EFI_NOT_FOUND         Cannot find supported flash device.
  @retval EFI_INVALID_PARAMETER Invalid function parameter.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_SMM_FWB_DETECT_DEVICE) (
  IN EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL  *This,
  OUT UINT8                             *Buffer
  );

/**
 Reads data beginning at Lba:Offset from FV and places the data in Buffer.
 The read terminates either when *NumBytes of data have been read, or when
 a block boundary is reached.  *NumBytes is updated to reflect the actual
 number of bytes read.

 @param [in]      This            Pointer to EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL instance.
 @param [in]      WriteAddress    Start LBA address which want to read.
 @param [in]      Offset          Offset in the block at which to begin read
 @param [in, out] NumBytes        IN:  The requested read size.
                                  OUT: The data size by bytes has been read
 @param [out]     Buffer          Data buffer in which to place data read.

 @retval EFI_SUCCESS              Read data successful.
 @retval EFI_INVALID_PARAMETER    Input function parameters are invalid.
 @retval EFI_UNSUPPORTED          The flash device is not supported
 @retval EFI_DEVICE_ERROR         Read data failed caused by device error.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_SMM_FWB_READ) (
  IN EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL  *This,
  IN UINTN                              WriteAddress,
  IN UINTN                              Offset,
  IN OUT UINTN                          *NumBytes,
  OUT UINT8                             *Buffer
  );

/**
 Writes data beginning at Lba:Offset from FV. The write terminates either
 when *NumBytes of data have been written, or when a block boundary is
 reached.  *NumBytes is updated to reflect the actual number of bytes
 written.

 @param [in]      This            Pointer to EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL instance.
 @param [in]      Offset          Start address to be written.
 @param [in, out] NumBytes        IN:  The requested write size.
                                  OUT: The data size by bytes has been written.
 @param [out]     Buffer          Data buffer want to write.

 @retval EFI_SUCCESS              Write data successful.
 @retval EFI_INVALID_PARAMETER    Input function parameters are invalid.
 @retval EFI_UNSUPPORTED          The flash device is not supported
 @retval EFI_DEVICE_ERROR         Write data failed caused by device error.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_SMM_FWB_WRITE) (
  IN EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL  *This,
  IN UINTN                              Offset,
  IN OUT UINTN                          *NumBytes,
  IN UINT8                              *Buffer
  );

#ifndef EFI_LBA_LIST_TERMINATOR
#define EFI_LBA_LIST_TERMINATOR 0xFFFFFFFFFFFFFFFF
#endif

/**
 The EraseBlock() function erases one or more blocks as denoted by the
 variable argument list. The entire parameter list of blocks must be verified
 prior to erasing any blocks.  If a block is requested that does not exist
 within the associated firmware volume (it has a larger index than the last
 block of the firmware volume), the EraseBlock() function must return
 EFI_INVALID_PARAMETER without modifying the contents of the firmware volume.

 @param [in]      This            Pointer to EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL instance.
 @param [in]      LbaWriteAddress Start address to be erased.
 @param [in, out] NumBytes        IN:  The requested erase size.
                                  OUT: The data size by bytes has been erased.

 @retval EFI_SUCCESS              Erase data successful.
 @retval EFI_INVALID_PARAMETER    Input function parameters are invalid.
 @retval EFI_UNSUPPORTED          The flash device is not supported
 @retval EFI_DEVICE_ERROR         Erase data failed caused by device error.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_SMM_FWB_ERASE_BLOCKS) (
  IN EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL  *This,
  IN UINTN                              LbaWriteAddress,
  IN OUT UINTN                          *NumBytes
  );

/**
 The GetFlashTable() function get flash region table from platform.
 if the descriptor is valid, the FlashTable will be filled.
 if the descriptor is invalid,the FlashTable will be filled RegionTypeEos(0xff) directly.

 @param [in]      This          Pointer to EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL instance.
 @param [in, out] FlashTable    IN:  Input buffer which want to save flash table.
                                OUT: Buffer to save flash table.

 @retval EFI_SUCCESS            Get flash table successful.
 @retval EFI_UNSUPPORTED        Flash device doesn't support this feature.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_SMM_FWB_GET_FLASH_TABLE) (
  IN EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL  *This,
  IN OUT UINT8                          *FlashTable
  );

/**
 Get Platform SPI Flash Number from SPI descriptor

 @param [in]  This          Pointer to EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL instance.
 @param [out] FlashNumber   Number of SPI flash devices returned.

 @retval EFI_SUCCESS        Successfully returns.
 @retval EFI_UNSUPPORTED    Platform is not using SPI flash rom or SPI is not in descriptor mode.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_SMM_FWB_GET_SPI_FLASH_NUMBER) (
  IN EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL  *This,
  OUT UINT8                          *FlashNumber
  );

/**
 Convert address for SPI descriptor mode

 @param [in] This          Pointer to EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL instance.
 @param[in]  MemAddress    BIOS memory mapped address.
 @param[out] SpiAddress    Flash ROM start address + BIOS address in flash ROM.

 @retval EFI_SUCCESS       Successfully returns.
 @retval EFI_UNSUPPORT     Platform is not using SPI ROM.
                           The SPI flash is not in Descriptor mode.
                           The input address does not belong to BIOS memory mapped region.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_SMM_FWB_CONVERT_SPI_ADDRESS) (
  IN  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL  *This,
  IN  UINTN                              MemAddress,
  OUT UINTN                              *SpiAddress
  );



//
// SMM RUNTIME PROTOCOL
//
struct _EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL {
  EFI_SMM_FWB_DETECT_DEVICE             DetectDevice;
  EFI_SMM_FWB_READ                      Read;
  EFI_SMM_FWB_WRITE                     Write;
  EFI_SMM_FWB_ERASE_BLOCKS              EraseBlocks;
  EFI_SMM_FWB_GET_FLASH_TABLE           GetFlashTable;
  EFI_SMM_FWB_GET_SPI_FLASH_NUMBER      GetSpiFlashNumber;
  EFI_SMM_FWB_CONVERT_SPI_ADDRESS       ConvertToSpiAddress;
  UINT8                                 *FlashMode;
};

extern EFI_GUID gEfiSmmFwBlockServiceProtocolGuid;

#endif
