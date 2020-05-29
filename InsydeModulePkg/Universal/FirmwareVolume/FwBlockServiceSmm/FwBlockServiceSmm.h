/** @file
  This driver provides Firmware block services interface in SMM mode

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

#ifndef _FWBLOCK_SERVICES_SMM_H_
#define _FWBLOCK_SERVICES_SMM_H_


#include <PiSmm.h>

#include <Protocol/SmmBase2.h>
#include <Protocol/SmmRuntime.h>
#include <Protocol/SmmFwBlockService.h>
#include <Protocol/SmmCommunication.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/FdSupportLib.h>


typedef struct {
  UINT32                                Signature;
  EFI_SMM_RUNTIME_PROTOCOL              *SmmRT;
  FLASH_DEVICE                          *FlashDevice;
  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL     SmmFwbService;
  UINT8                                 FlashMode;
} SMM_FW_BLOCK_SERVICE_INSTANCE;

#define SMM_FW_BLOCK_SERVICE_SIGNATURE  SIGNATURE_32 ('s', 'm', 'f', 'b')

#define INSTANCE_FROM_EFI_SMM_FW_BLOCK_SERVICE_THIS(a)  CR (a, SMM_FW_BLOCK_SERVICE_INSTANCE, SmmFwbService, SMM_FW_BLOCK_SERVICE_SIGNATURE)

#define BLOCK_SIZE       0x1000

//
// This structure is used for SMM Firmware Block Service. the collected statistics data is saved in SMRAM. It can be got from
// SMI handler. The communication buffer should be:
// EFI_SMM_COMMUNICATE_HEADER + SMM_FWB_COMMUNICATE_HEADER + payload.
//
typedef struct {
  UINTN       Function;
  EFI_STATUS  ReturnStatus;
  UINT8       Data[1];
} SMM_FWB_COMMUNICATE_HEADER;

//
// Return EFI_UNSUPPORTED directly.
//
#define SMM_FWB_FUNCTION_DETECT_DEVICE            1
//
// The payload for this function is SMM_FWB_COMMUNICATE_ACCESS_FVB.
//
#define SMM_FWB_FUNCTION_READ                     2
//
// The payload for this function is SMM_FWB_COMMUNICATE_ACCESS_FVB.
//
#define SMM_FWB_FUNCTION_WRITE                    3
//
// The payload for this function is SMM_FWB_COMMUNICATE_ACCESS_FVB.
//
#define SMM_FWB_FUNCTION_ERASE_BLOCKS             4
//
// Return EFI_UNSUPPORTED directly.
//
#define SMM_FWB_FUNCTION_GET_FLASH_TABLE          5
//
// Return EFI_UNSUPPORTED directly.
//
#define SMM_FWB_FUNCTION_GET_SPI_FLASH_NUMBER     6
//
// Return EFI_UNSUPPORTED directly.
//
#define SMM_FWB_FUNCTION_ONVERT_SPI_ADDRESS       7



typedef struct {
  UINTN       Address;
  UINTN       NumBytes;
} SMM_FWB_COMMUNICATE_ACCESS_FVB;


///
/// Size of SMM communicate header, without including the payload.
///
#define SMM_COMMUNICATE_HEADER_SIZE  (OFFSET_OF (EFI_SMM_COMMUNICATE_HEADER, Data))
///
/// Size of SMM FWB header, without including the payload.
///
#define SMM_FWB_COMMUNICATE_HEADER_SIZE  (OFFSET_OF (SMM_FWB_COMMUNICATE_HEADER, Data))
///
/// Size of SMM_FWB_COMMUNICATE_ACCESS_FVB, including header size.
///
#define SMM_FWB_COMMUNICATE_ACCESS_FVB_SIZE   (SMM_COMMUNICATE_HEADER_SIZE + SMM_FWB_COMMUNICATE_HEADER_SIZE + sizeof (SMM_FWB_COMMUNICATE_ACCESS_FVB))

/**
  Detect target flash device and copy whole flash device to output buffer.

  @param[in]  This               Pointer to EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL instance.
  @param[out] SystemTable        Buffer to saved target flash device information

  @retval EFI_SUCCESS           The flash device is recognized successfully.
  @retval EFI_NOT_FOUND         Cannot find supported flash device.
  @retval EFI_INVALID_PARAMETER Invalid function parameter.
**/
EFI_STATUS
EFIAPI
FlashFdDetectDevice (
  IN  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL *This,
  OUT UINT8                             *Buffer
  );


/**
  Detect target flash device and copy whole flash device to output buffer for ECP driver.

  @param[in]  This               Pointer to EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL instance.
  @param[out] SystemTable        Buffer to saved target flash device information

  @retval EFI_SUCCESS           The flash device is recognized successfully.
  @retval EFI_NOT_FOUND         Cannot find supported flash device.
  @retval EFI_INVALID_PARAMETER Invalid function parameter.
**/
EFI_STATUS
EFIAPI
EcpFlashFdDetectDevice (
  IN  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL *This,
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
EFI_STATUS
EFIAPI
FlashFdRead (
  IN  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL *This,
  IN  UINTN                             WriteAddress,
  IN  UINTN                             Offset,
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
EFI_STATUS
EFIAPI
FlashFdWrite (
  IN  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL *This,
  IN  UINTN                             WriteAddress,
  IN OUT UINTN                          *NumBytes,
  IN  UINT8                             *Buffer
  );

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
EFI_STATUS
EFIAPI
FlashFdEraseBlocks (
  IN  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL *This,
  IN  UINTN                             WriteAddress,
  IN  UINTN                             *NumBytes
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
EFI_STATUS
EFIAPI
FlashFdGetFlashTable (
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
EFI_STATUS
EFIAPI
FlashFdGetSpiFlashNumber (
  IN  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL  *This,
  OUT UINT8                              *FlashNumber
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
EFI_STATUS
EFIAPI
FlashFdConvertToSpiAddress (
  IN  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL  *This,
  IN  UINTN                              MemAddress,
  OUT UINTN                              *SpiAddress
  );

/**
  Detect target flash device and copy whole flash device to output buffer.

  This function always returns EFI_UNSUPPORTED.

  @param[in]  This               Pointer to EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL instance.
  @param[out] SystemTable        Buffer to saved target flash device information

  @retval EFI_UNSUPPORTED        Always return EFI_UNSUPPORTED.
**/
EFI_STATUS
EFIAPI
BsFlashFdDetectDevice (
  IN  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL *This,
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
EFI_STATUS
EFIAPI
BsFlashFdRead (
  IN  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL *This,
  IN  UINTN                             WriteAddress,
  IN  UINTN                             Offset,
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
EFI_STATUS
EFIAPI
BsFlashFdWrite (
  IN  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL *This,
  IN  UINTN                             WriteAddress,
  IN OUT UINTN                          *NumBytes,
  IN  UINT8                             *Buffer
  );

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
EFI_STATUS
EFIAPI
BsFlashFdEraseBlocks (
  IN  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL *This,
  IN  UINTN                             WriteAddress,
  IN  UINTN                             *NumBytes
  );

/**
 The GetFlashTable() function get flash region table from platform.
 if the descriptor is valid, the FlashTable will be filled.
 if the descriptor is invalid,the FlashTable will be filled RegionTypeEos(0xff) directly.

 This function always returns EFI_UNSUPPORTED.

 @param [in]      This          Pointer to EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL instance.
 @param [in, out] FlashTable    IN:  Input buffer which want to save flash table.
                                OUT: Buffer to save flash table.

 @retval EFI_UNSUPPORTED        Always return EFI_UNSUPPORTED.
**/
EFI_STATUS
EFIAPI
BsFlashFdGetFlashTable (
  IN EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL  *This,
  IN OUT UINT8                          *FlashTable
  );

/**
 Get Platform SPI Flash Number from SPI descriptor

 This function always returns EFI_UNSUPPORTED.

 @param [in]  This          Pointer to EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL instance.
 @param [out] FlashNumber   Number of SPI flash devices returned.

 @retval EFI_UNSUPPORT      Always return EFI_UNSUPPORTED.
**/
EFI_STATUS
EFIAPI
BsFlashFdGetSpiFlashNumber (
  IN  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL  *This,
  OUT UINT8                              *FlashNumber
  );

/**
 Convert address for SPI descriptor mode

 This function always returns EFI_UNSUPPORTED.

 @param [in] This          Pointer to EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL instance.
 @param[in]  MemAddress    BIOS memory mapped address.
 @param[out] SpiAddress    Flash ROM start address + BIOS address in flash ROM.

 @retval EFI_UNSUPPORT     Always return EFI_UNSUPPORTED.
 **/
EFI_STATUS
EFIAPI
BsFlashFdConvertToSpiAddress (
  IN  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL  *This,
  IN  UINTN                              MemAddress,
  OUT UINTN                              *SpiAddress
  );

#endif
