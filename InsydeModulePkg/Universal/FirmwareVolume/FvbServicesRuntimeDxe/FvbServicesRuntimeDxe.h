/** @file
  Header file for FvbServicesRuntimeDxe driver.

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

#ifndef _FVB_SERVICES_RUNTIME_DXE_H_
#define _FVB_SERVICES_RUNTIME_DXE_H_

#include <PiSmm.h>
//
// The protocols, PPI and GUID defintions for this module
//
#include <Guid/EventGroup.h>
#include <Guid/FirmwareFileSystem2.h>
#include <Guid/SystemNvDataGuid.h>
#include <Protocol/FirmwareVolumeBlock.h>
#include <Protocol/DevicePath.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/SmmCpu.h>
#include <Protocol/SmmSwDispatch2.h>
//
// The Library classes this module consumes
//
#include <Library/UefiLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/BaseLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DevicePathLib.h>
#include <Library/FdSupportLib.h>
#include <Library/PcdLib.h>
#include <Library/ImageRelocationLib.h>
//
// BugBug: Add documentation here for data structure!!!!
//
#define FVB_PHYSICAL  0
#define FVB_VIRTUAL   1

typedef struct {
  UINTN                       FvBase[2];
  UINTN                       NumOfBlocks;
  EFI_FIRMWARE_VOLUME_HEADER  VolumeHeader;
} EFI_FW_VOL_INSTANCE;

typedef struct {
  UINT32              NumFv;
  EFI_FW_VOL_INSTANCE *FvInstance[2];
  UINT8               *FvbScratchSpace[2];
} ESAL_FWB_GLOBAL;

//
// Fvb Protocol instance data
//
#define FVB_DEVICE_FROM_THIS(a)         CR (a, EFI_FW_VOL_BLOCK_DEVICE, FwVolBlockInstance, FVB_DEVICE_SIGNATURE)
#define FVB_EXTEND_DEVICE_FROM_THIS(a)  CR (a, EFI_FW_VOL_BLOCK_DEVICE, FvbExtension, FVB_DEVICE_SIGNATURE)
#define FVB_DEVICE_SIGNATURE            SIGNATURE_32 ('F', 'V', 'B', 'N')

typedef struct {
  MEDIA_FW_VOL_DEVICE_PATH  FvDevPath;
  EFI_DEVICE_PATH_PROTOCOL  EndDevPath;
} FV_PIWG_DEVICE_PATH;

typedef struct {
  MEMMAP_DEVICE_PATH          MemMapDevPath;
  EFI_DEVICE_PATH_PROTOCOL    EndDevPath;
} FV_MEMMAP_DEVICE_PATH;

typedef struct {
  UINTN                               Signature;
  EFI_DEVICE_PATH_PROTOCOL            *DevicePath;
  UINTN                               Instance;
  EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL  FwVolBlockInstance;
} EFI_FW_VOL_BLOCK_DEVICE;


//
// Accessing FVB relative definitions
//
#define SMM_FVB_ACCESS_SIGNATURE        SIGNATURE_32 ('S', 'M', 'F', 'A')

typedef enum {
  PfatReadFvb,
  PfatWriteFvb,
  PfatEraseFvb,
} SMM_FVB_ACCESS_TYPE;

typedef
EFI_STATUS
(EFIAPI *SMI_SUB_FUNCTION) (
  VOID
  );

typedef struct {
  SMM_FVB_ACCESS_TYPE                  FunNum;
  SMI_SUB_FUNCTION                     SmiSubFunction;
} SMI_SUB_FUNCTION_MAP;

//
//  Status        : Output status after function execution.
//  Singature     : Signature for SMM_FVB_BUFFER, the signature must be SMM_FIRMWARE_VOLUME_SIGNATURE
//  AccessType    : Access type for this execution. the value should be PfatReadFvb, PfatWriteFvb, or PfatEraseFvb.
//  AccessAddress : The address which want to access in FV.
//  DataSize      : Data size by byte for access.
//  DataBuffer    : Data buffer for access. this buffer must append at the end of SMM_FVB_BUFFER.
//
typedef struct {
  EFI_STATUS                    Status;
  UINT32                        Signature;
  UINTN                         AccessAddress;
  SMM_FVB_ACCESS_TYPE           AccessType;
  UINTN                         DataSize;
//
//UINT8                         *DataBuffer;
//
} SMM_FVB_BUFFER;

/**
  Calculate correct checksum for all of backup firmware volume headers and put the
  checksum result to all of backup firmware volume headers.
**/
VOID
RecalculateFvHeaderChecksum (
  VOID
  );

/**
  Get backup firmware volume header contents according to physical base address.

  @param[in]  BaseAddress  Firmware volume start address.
  @param[out] FvbInfo      Double pointer to output firmware volume header.

  @retval EFI_SUCCESS      Get firmware volume contents successful.
  @retval EFI_NOT_FOUND    Cannot find corresponding firmware volume header.
**/
EFI_STATUS
GetFvbInfo (
  IN  UINT64                            FvLength,
  OUT EFI_FIRMWARE_VOLUME_HEADER        **FvbInfo
  );

//
// Protocol APIs
//
/**
  Retrieves Volume attributes.  No polarity translations are done.

  @param[in]  This        Calling context.
  @param[out] Attributes  Output buffer which contains attributes

  @retval EFI_SUCCESS     Successfully returns.
**/
EFI_STATUS
EFIAPI
FvbProtocolGetAttributes (
  IN CONST EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL           *This,
  OUT EFI_FVB_ATTRIBUTES_2                              *Attributes
  );

/**
  Sets Volume attributes. No polarity translations are done.

  @param [in]     This        Calling context.
  @param [in,out] Attributes  Output buffer which contains attributes.

  @retval EFI_SUCCESS         Successfully returns.
**/
EFI_STATUS
EFIAPI
FvbProtocolSetAttributes (
  IN CONST EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL           *This,
  IN OUT EFI_FVB_ATTRIBUTES_2                           *Attributes
  );

/**
  Retrieves the physical address of the device.

  @param [in]  This     Calling context.
  @param [out] Address  Output buffer containing the address.

  @retval EFI_SUCCESS   Successfully returns.
**/
EFI_STATUS
EFIAPI
FvbProtocolGetPhysicalAddress (
  IN CONST EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL           *This,
  OUT EFI_PHYSICAL_ADDRESS                              *Address
  );

/**
  Retrieve the size of a logical block.

  @param [in]   This         Calling context.
  @param [in]   Lba          Indicates which block to return the size for.
  @param [out]  BlockSize    A pointer to a caller allocated UINTN in which
                             the size of the block is returned.
  @param [out]  NumOfBlocks  a pointer to a caller allocated UINTN in which the
                             number of consecutive blocks starting with Lba is
                             returned. All blocks in this range have a size of
                             BlockSize.

  @retval EFI_SUCCESS        The firmware volume was read successfully and
                             contents are in Buffer.
**/
EFI_STATUS
EFIAPI
FvbProtocolGetBlockSize (
  IN CONST EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL           *This,
  IN CONST EFI_LBA                                      Lba,
  OUT UINTN                                             *BlockSize,
  OUT UINTN                                             *NumOfBlocks
  );

/**
  Reads data beginning at Lba:Offset from FV. The Read terminates either
  when *NumBytes of data have been read, or when a block boundary is
  reached.  *NumBytes is updated to reflect the actual number of bytes
  written. The write opertion does not include erase. This routine will
  attempt to write only the specified bytes. If the writes do not stick,
  it will return an error.

  @param[in]      This           Calling context.
  @param[in]      Lba            Block in which to begin Read.
  @param[in]      Offset         Offset in the block at which to begin Read.
  @param[in, out] NumBytes       On input, indicates the requested write size. On
                                 output, indicates the actual number of bytes Read.
  @param[in]      Buffer         Buffer containing source data for the Read.

  @retval EFI_SUCCESS            The firmware volume was read successfully and
                                 contents are in Buffer.
  @retval EFI_BAD_BUFFER_SIZE    Read attempted across a LBA boundary. On output,
                                 NumBytes contains the total number of bytes returned
                                 in Buffer.
  @retval EFI_ACCESS_DENIED      The firmware volume is in the ReadDisabled state.
  @retval EFI_DEVICE_ERROR       The block device is not functioning correctly and
                                 could not be read.
  @retval EFI_INVALID_PARAMETER  NumBytes or Buffer are NULL.
**/
EFI_STATUS
EFIAPI
FvbProtocolRead (
  IN CONST EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL           *This,
  IN CONST EFI_LBA                                      Lba,
  IN CONST UINTN                                        Offset,
  IN OUT UINTN                                          *NumBytes,
  IN UINT8                                              *Buffer
  );

/**
  Writes data beginning at Lba:Offset from FV. The write terminates either
  when *NumBytes of data have been written, or when a block boundary is
  reached.  *NumBytes is updated to reflect the actual number of bytes
  written. The write opertion does not include erase. This routine will
  attempt to write only the specified bytes. If the writes do not stick,
  it will return an error.

  @param[in]      This           Calling context.
  @param[in]      Lba            Block in which to begin write.
  @param[in]      Offset         Offset in the block at which to begin write.
  @param[in, out] NumBytes       On input, indicates the requested write size. On
                                 output, indicates the actual number of bytes written.
  @param[in]      Buffer         Buffer containing source data for the write.

  @retval EFI_SUCCESS            The firmware volume was written successfully.
  @retval EFI_BAD_BUFFER_SIZE    Write attempted across a LBA boundary. On output,
                                 NumBytes contains the total number of bytes
                                 actually written.
  @retval EFI_ACCESS_DENIED      The firmware volume is in the WriteDisabled state.
  @retval EFI_DEVICE_ERROR       The block device is not functioning correctly and
                                 could not be written.
  @retval EFI_INVALID_PARAMETER  NumBytes or Buffer are NULL.
**/
EFI_STATUS
EFIAPI
FvbProtocolWrite (
  IN CONST EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL           *This,
  IN       EFI_LBA                                      Lba,
  IN       UINTN                                        Offset,
  IN OUT   UINTN                                        *NumBytes,
  IN       UINT8                                        *Buffer
  );

/**
  The EraseBlock() function erases one or more blocks as denoted by the
  variable argument list. The entire parameter list of blocks must be verified
  prior to erasing any blocks.  If a block is requested that does not exist
  within the associated firmware volume (it has a larger index than the last
  block of the firmware volume), the EraseBlock() function must return
  EFI_INVALID_PARAMETER without modifying the contents of the firmware volume.

  @retval EFI_SUCCESS            The erase request was successfully completed
  @retval EFI_ACCESS_DENIED      The firmware volume is in the WriteDisabled state
  @retval EFI_DEVICE_ERROR       The block device is not functioning correctly and
                                 could not be written. Firmware device may have been
                                 partially erased.
**/
EFI_STATUS
EFIAPI
FvbProtocolEraseBlocks (
  IN CONST EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL    *This,
  ...
  );

extern ESAL_FWB_GLOBAL         *mFvbModuleGlobal;
extern EFI_SMM_SYSTEM_TABLE2   *mSmst;
extern BOOLEAN                 mFvbAccessThroughSmi;
extern SMM_FVB_BUFFER          *mSmmFvbBuffer;
extern SMM_FVB_BUFFER          *mSmmPhyFvbBuffer;

extern EFI_GUID                gFvbAccessThroughSmiGuid;
#endif
