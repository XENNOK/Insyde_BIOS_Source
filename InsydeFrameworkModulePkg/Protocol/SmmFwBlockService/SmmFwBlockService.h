//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _SMM_FW_BLOCK_SERVICE_PROTO_H_
#define _SMM_FW_BLOCK_SERVICE_PROTO_H_

#include "Tiano.h"

#define EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL_GUID \
  { \
    0x2970687c, 0x618c, 0x4de5, 0xb8, 0xf9, 0x6c, 0x75, 0x76, 0xdc, 0xa8, 0x3d \
  }

EFI_FORWARD_DECLARATION (EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL);
#define SMM_FW_FLASH_MODE       0x01
#define SMM_FW_DEFAULT_MODE     0x00

typedef
EFI_STATUS
(EFIAPI *EFI_SMM_FWB_DETECT_DEVICE) (
  IN EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL  *This,
  OUT UINT8                             *Buffer
  )
;
typedef
EFI_STATUS
(EFIAPI *EFI_SMM_FWB_READ) (
  IN EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL  *This,
  IN UINTN                              WriteAddress,
  IN UINTN                              Offset,
  IN OUT UINTN                          *NumBytes,
  OUT UINT8                             *Buffer
  )
/*++

Routine Description:
  Reads data beginning at Lba:Offset from FV and places the data in Buffer.
  The read terminates either when *NumBytes of data have been read, or when
  a block boundary is reached.  *NumBytes is updated to reflect the actual
  number of bytes read.

Arguments:
  This - Calling context
  Lba - Block in which to begin read
  Offset - Offset in the block at which to begin read
  NumBytes - At input, indicates the requested read size.  At output, indicates
    the actual number of bytes read.
  Buffer - Data buffer in which to place data read.

Returns:
  EFI_INVALID_PARAMETER
  EFI_NOT_FOUND
  EFI_DEVICE_ERROR
  EFI_SUCCESS

--*/
;

typedef
EFI_STATUS
(EFIAPI *EFI_SMM_FWB_WRITE) (
  IN EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL  *This,
  IN UINTN                              Offset,
  IN OUT UINTN                          *NumBytes,
  IN UINT8                              *Buffer
  )
/*++

Routine Description:

  Writes data beginning at Lba:Offset from FV. The write terminates either
  when *NumBytes of data have been written, or when a block boundary is
  reached.  *NumBytes is updated to reflect the actual number of bytes
  written.

Arguments:
  This - Calling context
  Lba - Block in which to begin write
  Offset - Offset in the block at which to begin write
  NumBytes - At input, indicates the requested write size.  At output, indicates
    the actual number of bytes written.
  Buffer - Buffer containing source data for the write.

Returns:
  EFI_INVALID_PARAMETER
  EFI_NOT_FOUND
  EFI_DEVICE_ERROR
  EFI_SUCCESS

--*/
;

#define EFI_LBA_LIST_TERMINATOR 0xFFFFFFFFFFFFFFFF

typedef
EFI_STATUS
(EFIAPI *EFI_SMM_FWB_ERASE_BLOCKS) (
  IN EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL  *This,
  IN UINTN                              LbaWriteAddress,
  IN OUT UINTN                          *NumBytes
  )
/*++

Routine Description:
  The EraseBlock() function erases one or more blocks as denoted by the
variable argument list. The entire parameter list of blocks must be verified
prior to erasing any blocks.  If a block is requested that does not exist
within the associated firmware volume (it has a larger index than the last
block of the firmware volume), the EraseBlock() function must return
EFI_INVALID_PARAMETER without modifying the contents of the firmware volume.

Arguments:
    This - Calling context
    ...  - Starting LBA followed by Number of Lba to erase. a -1 to terminate
           the list.

Returns:
    EFI_INVALID_PARAMETER
    EFI_DEVICE_ERROR
    EFI_SUCCESS
    EFI_ACCESS_DENIED

--*/
;

typedef
EFI_STATUS
(EFIAPI *EFI_SMM_FWB_GET_FLASH_TABLE) (
  IN EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL  *This,
  IN OUT UINT8                          *FlashTable
  )
/*++

Routine Description:

  The GetFlashTable() function get flash region table from platform.
  if the descriptor is valid, the FlashTable will be filled.
  if the descriptor is invalid,the FlashTable will be filled RegionTypeEos(0xff) directly.

Arguments:
    This        - Calling context
    FlashTable  - IN: the input buffer address
                  OUT:the flash region table from platform

Returns:

  EFI_SUCCESS           - Successfully returns

--*/
;

typedef
EFI_STATUS
(EFIAPI *EFI_SMM_FWB_GET_SPI_FLASH_NUMBER) (
  IN EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL  *This,
  OUT UINT8                          *FlashNumber
  )
/*++

Routine Description:
  Get Platform SPI Flash Number from SPI descriptor

Arguments:

  *FlashNumber           - the flash number

Returns:
  EFI_SUCCESS           - Successfully returns
  EFI_UNSUPPORTED       - Platform is not using SPI flash rom
                          SPI is not in descriptor mode

--*/
;

typedef
EFI_STATUS
(EFIAPI *EFI_SMM_FWB_CONVERT_SPI_ADDRESS) (
  IN  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL  *This,
  IN  UINTN                              MemAddress,
  OUT UINTN                              *SpiAddress
  )
/*++
Routine Description:

  Convert address for SPI descriptor mode

Arguments:

  Source      - The source address
  Destination - The destination address
  
Returns:

  EFI_SUCCESS           - Successfully returns
  EFI_UNSUPPORT         - Platform is not using SPI ROM
                        - The SPI flash is not in Descriptor mode
                        - The input address does not belong to BIOS memory mapped region

--*/
;

//
// SMM RUNTIME PROTOCOL
//
typedef struct _EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL {
  EFI_SMM_FWB_DETECT_DEVICE             DetectDevice;
  EFI_SMM_FWB_READ                      Read;
  EFI_SMM_FWB_WRITE                     Write;
  EFI_SMM_FWB_ERASE_BLOCKS              EraseBlocks;
  EFI_SMM_FWB_GET_FLASH_TABLE           GetFlashTable;
  EFI_SMM_FWB_GET_SPI_FLASH_NUMBER      GetSpiFlashNumber;
  EFI_SMM_FWB_CONVERT_SPI_ADDRESS       ConvertToSpiAddress;
  UINT8                                 *FlashMode;
} EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL;

extern EFI_GUID gEfiSmmFwBlockServiceProtocolGuid;

#endif
