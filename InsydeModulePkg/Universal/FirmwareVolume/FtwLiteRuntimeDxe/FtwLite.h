/** @file
  This is a simple fault tolerant write driver, based on PlatformFd library.
  And it only supports write BufferSize <= SpareAreaLength.

  This boot service only protocol provides fault tolerant write capability for
  block devices.  The protocol has internal non-volatile intermediate storage
  of the data and private information. It should be able to recover
  automatically from a critical fault, such as power failure.

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


#ifndef _EFI_FAULT_TOLERANT_WRITE_LITE_H_
#define _EFI_FAULT_TOLERANT_WRITE_LITE_H_

#include <PiSmm.h>

#include <Protocol/SmmBase2.h>
#include <Protocol/FirmwareVolumeBlock.h>
#include <Protocol/FaultTolerantWriteLite.h>
#include <Protocol/SmmFwBlockService.h>
#include <Protocol/SmmFtw.h>
#include <Protocol/SmmRuntime.h>
#include <Protocol/FirmwareVolume2.h>

#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/ImageRelocationLib.h>

#include <Guid/SystemNvDataGuid.h>
#include <Guid/AuthenticatedVariableFormat.h>
#include <Guid/EventGroup.h>
//
// Flash erase polarity is 1
//
#define FTW_ERASE_POLARITY  1

#define FTW_VALID_STATE     0
#define FTW_INVALID_STATE   1

#define FTW_ERASED_BYTE     ((UINT8) (255))
#define FTW_POLARITY_REVERT ((UINT8) (255))

//
// MACRO for FTW header and record
//
#define FTW_LITE_RECORD_SIZE    (sizeof (EFI_FTW_LITE_RECORD))
#define WRITE_TOTAL_SIZE        FTW_LITE_RECORD_SIZE

#define FTW_MAX_TRY_ACCESS_FVB_TIMES  0x02
#define MAX_MEMORY_NODE               0x06

#define FTW_LITE_DEVICE_SIGNATURE         SIGNATURE_32 ('F', 'T', 'W', 'L')
#define FTW_LITE_CONTEXT_FROM_THIS(a)     CR (a, EFI_FTW_LITE_DEVICE, FtwLiteInstance, FTW_LITE_DEVICE_SIGNATURE)
#define SMM_FTW_LITE_CONTEXT_FROM_THIS(a) CR (a, EFI_FTW_LITE_DEVICE, SmmFtwProtocol, FTW_LITE_DEVICE_SIGNATURE)

#define EFI_D_FTW_LITE  EFI_D_ERROR
#define EFI_D_FTW_INFO  EFI_D_INFO


#define EFI_FREE_POOL(Addr) \
{  mSmst ? mSmst->SmmFreePool((VOID*) (Addr)) : gBS->FreePool ((VOID *) (Addr)); \
   Addr = NULL;}

typedef struct {
  UINT8         WriteAllocated : 1;
  UINT8         SpareCompleted : 1;
  UINT8         WriteCompleted : 1;
  UINT8         Reserved : 5;
#define WRITE_ALLOCATED 0x1
#define SPARE_COMPLETED 0x2
#define WRITE_COMPLETED 0x4

  EFI_DEV_PATH  DevPath;
  EFI_LBA       Lba;
  UINTN         Offset;
  UINTN         NumBytes;
  //
  // UINTN           SpareAreaOffset;
  //
} EFI_FTW_LITE_RECORD;

typedef struct {
  BOOLEAN       Used;
  UINTN         BufferSize;
  VOID          *Buffer;
} EFI_FTW_LITE_BUFFER;
//
// EFI Fault tolerant protocol private data structure
//
typedef struct {
  UINTN                                   Signature;
  EFI_HANDLE                              Handle;
  EFI_FTW_LITE_PROTOCOL                   FtwLiteInstance;
  EFI_SMM_FTW_PROTOCOL                    SmmFtwProtocol;
  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL       *SmmFwbServices;
  EFI_PHYSICAL_ADDRESS                    WorkSpaceFvBaseAddr;
  EFI_PHYSICAL_ADDRESS                    SpareAreaFvBaseAddr;
  EFI_PHYSICAL_ADDRESS                    WorkBlockAddr;
  EFI_PHYSICAL_ADDRESS                    WorkSpaceAddress;
  UINTN                                   WorkSpaceLength;
  EFI_PHYSICAL_ADDRESS                    SpareAreaAddress;
  UINTN                                   SpareAreaLength;
  UINTN                                   NumberOfSpareBlock;   // Number of the blocks in spare block
  UINTN                                   SizeOfSpareBlock;     // Block size in bytes of the blocks in spare block
  EFI_FAULT_TOLERANT_WORKING_BLOCK_HEADER *FtwWorkSpaceHeader;
  EFI_FTW_LITE_RECORD                     *FtwLastRecord;
  EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL      *FtwFvBlock;          // FVB of working block
  EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL      *FtwBackupFvb;        // FVB of spare block
  EFI_LBA                                 FtwSpareLba;
  EFI_LBA                                 FtwWorkBlockLba;      // Start LBA of working block
  EFI_LBA                                 FtwWorkSpaceLba;      // Start LBA of working space
  UINTN                                   FtwWorkSpaceBase;     // Offset from LBA start addr
  UINTN                                   NumberOfFtwWorkBlock; // Number of the blocks in Ftw work block
  UINTN                                   FtwWorkSpaceSize;
  EFI_FTW_LITE_BUFFER                     *ReclaimMemory;
  UINT8                                   *FtwWorkSpace;
  //
  // Following a buffer of FtwWorkSpace[FTW_WORK_SPACE_SIZE],
  // Allocated with EFI_FTW_LITE_DEVICE.
  //
} EFI_FTW_LITE_DEVICE;

/**
  This function is the entry point of the Fault Tolerant Write driver.

  @param  ImageHandle   EFI_HANDLE: A handle for the image that is initializing this driver.
  @param  SystemTable   EFI_SYSTEM_TABLE: A pointer to the EFI system table.

  @retval EFI_SUCCESS   FTW has finished the initialization
  @retval EFI_ABORTED   FTW initialization error.

**/
EFI_STATUS
EFIAPI
InitializeFtwLite (
  IN EFI_HANDLE                 ImageHandle,
  IN EFI_SYSTEM_TABLE           *SystemTable
  );

/**
  Starts a target block update. This function will record data about write in fault tolerant
  storage and will complete the write in a recoverable manner, ensuring at all times that
  either the original contents or the modified contents are available.

  @param  This                  Calling context.
  @param  FvbHandle             The handle of FVB protocol that provides services for reading,
                                writing, and erasing the target block.
  @param  Lba                   The logical block address of the target block.
  @param  Offset                The offset within the target block to place the data.
  @param  NumBytes              The number of bytes to write to the target block.
  @param  Buffer                The data to write.

  @retval EFI_SUCCESS           The function completed successfully.
  @retval EFI_BAD_BUFFER_SIZE   The write would span a target block, which is not a valid action.
  @retval EFI_ACCESS_DENIED     No writes have been allocated.
  @retval EFI_NOT_FOUND         Cannot find FVB by handle.
  @retval EFI_OUT_OF_RESOURCES  Cannot allocate memory.
  @retval EFI_ABORTED           The function could not complete successfully.

**/
EFI_STATUS
EFIAPI
FtwLiteWrite (
  IN EFI_FTW_LITE_PROTOCOL                 *This,
  IN EFI_HANDLE                            FvbHandle,
  IN EFI_LBA                               Lba,
  IN UINTN                                 Offset,
  IN UINTN                                 *NumBytes,
  IN VOID                                  *Buffer
  );

/**
  Write a record with fault tolerant mannaer. Since the content has already backuped in
  spare block, the write is guaranteed to be completed with fault tolerant manner.

  @param  FtwLiteDevice      The private data of FTW_LITE driver.
  @param  Fvb                The FVB protocol that provides services for reading, writing,
                             and erasing the target block.
  @param  FvBaseAddr         Frimware volume base address.

  @retval EFI_SUCCESS        The function completed successfully.
  @retval EFI_ABORTED        The function could not complete successfully.

**/
EFI_STATUS
FtwWriteRecord (
  IN EFI_FTW_LITE_DEVICE                   *FtwLiteDevice,
  IN EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL    *Fvb,
  IN EFI_PHYSICAL_ADDRESS                  FvBaseAddr
  );

/**
  To Erase one block. The size is FTW_BLOCK_SIZE.

  @param  FtwLiteDevice  Calling context
  @param  FvBlock        FVB Protocol interface
  @param  FvBaseAddr     The Firmware Volume BaseAddress of target block.
  @param  Lba            Lba of the firmware block

  @retval EFI_SUCCESS    Block LBA is Erased successfully.
  @retval EFI_ABORTED    Error occurs.

**/
EFI_STATUS
FtwEraseBlock (
  IN EFI_FTW_LITE_DEVICE              *FtwLiteDevice,
  EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL  *FvBlock,
  IN EFI_PHYSICAL_ADDRESS             FvBaseAddr,
  EFI_LBA                             Lba
  );

/**
  Function uses to erase whole spare block.

  @param  FtwLiteDevice  Calling context

  @retval EFI_SUCCESS    Whole spare block is Erased successfully.
  @retval EFI_ABORTED    Error occurs.

**/
EFI_STATUS
FtwEraseSpareBlock (
  IN EFI_FTW_LITE_DEVICE   *FtwLiteDevice
  );

/**
  Write target block and check the data which write to target block is correct.

  @param  FtwLiteDevice          Calling context.
  @param  FvBlock                FVB Protocol interface to access target block.
  @param  FvBaseAddr             The Firmware Volume BaseAddress of target block.
  @param  StartLba               Lba of the target block.
  @param  WriteBuffer            Write Buffer.
  @param  WriteLength            The size of write data.
  @param  MaxTryWriteTimes       The maximum trying to write times.

  @retval EFI_SUCCESS            Write data to working block success.
  @retval EFI_OUT_OF_RESOURCE    Allocate memory error.
  @retval EFI_ABORTED            Erase, write, or read error.
  @retval EFI_DEVICE_ERROR       Cannont write data to spare block.
  @retval EFI_INVALID_PARAMETER  Any input value is invalid.

**/
EFI_STATUS
WriteTargetBlock (
  IN EFI_FTW_LITE_DEVICE                     *FtwLiteDevice,
  IN EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL      *FvBlock,
  IN EFI_PHYSICAL_ADDRESS                    FvBaseAddr,
  IN EFI_LBA                                 StartLba,
  IN UINT8                                   *WriteBuffer,
  IN UINTN                                   WriteLength,
  IN UINTN                                   MaxTryWriteTimes
  );

/**
  Write spare block and check the data which write to spare block is correct.

  @param  FtwLiteDevice          Calling context.
  @param  WriteBuffer            Write Buffer.
  @param  WriteLength            The size of write data.
  @param  TryWriteTimes          The maximum trying to write times.

  @retval EFI_SUCCESS            Write data to spare block success.
  @retval EFI_OUT_OF_RESOURCE    The input value is invalid.
  @retval EFI_ABORTED            Write data to spare block failed.

**/
EFI_STATUS
FtwWriteSpareBlock (
  IN EFI_FTW_LITE_DEVICE   *FtwLiteDevice,
  IN UINT8                 *WriteBuffer,
  IN UINTN                 WriteLength,
  IN UINTN                 MaxTryWriteTimes
  );

/**
  Write working block and check the data which write to working block is correct.

  @param  FtwLiteDevice          Calling context.
  @param  WriteBuffer            Write Buffer.
  @param  WriteLength            The size of write data.
  @param  TryWriteTimes          The maximum trying to write times.

  @retval EFI_SUCCESS            Write data to working block success.
  @retval EFI_OUT_OF_RESOURCE    Allocate memory error.
  @retval EFI_ABORTED            Erase, write, or read error.
  @retval EFI_DEVICE_ERROR       Cannont write data to working block.
  @retval EFI_INVALID_PARAMETER  Any input value is invalid

**/
EFI_STATUS
FtwWriteWorkingBlock (
  IN EFI_FTW_LITE_DEVICE   *FtwLiteDevice,
  IN UINT8                 *WriteBuffer,
  IN UINTN                 WriteLength,
  IN UINTN                 MaxTryWriteTimes
  );

/**
  Get firmware block by address.

  @param  FvBlockHandle   Address specified the block.
  @param  FvBlock         The block caller wanted.

  @retval EFI_SUCCESS     Get Fvb protocol successful.
  @retval EFI_NOT_FOUND   Block not found

**/
EFI_STATUS
GetFvbByAddress (
  IN  EFI_PHYSICAL_ADDRESS               Address,
  OUT EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL **FvBlock
  );

/**
  According physical address or LBA to check this address is whether
  located in working block.

  @param  FtwLiteDevice   Calling context.
  @param  FvBlock         Fvb protocol instance.
  @param  Lba             input LBA for proteced mode use.
  @param  Address         input target address for SMM mode use.

  @retval TRUE            This address or LBA is located in working block
  @retval FALSE           This address or LBA isn't located in working block

**/
BOOLEAN
IsInWorkingBlock (
  EFI_FTW_LITE_DEVICE                 *FtwLiteDevice,
  EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL  *FvBlock,
  EFI_LBA                             Lba,
  EFI_PHYSICAL_ADDRESS                Address
  );


/**
  Copy the content of spare block to a target block. Size is FTW_BLOCK_SIZE. Spare block
  is accessed by FTW backup FVB protocol interface. LBA is FtwLiteDevice->FtwSpareLba.
  Target block is accessed by FvBlock protocol interface. LBA is Lba.

  @param  FtwLiteDevice   The private data of FTW_LITE driver.
  @param  FvBlock         FVB Protocol interface to access target block.
  @param  FvBaseAddr      Frimware volume base address.
  @param  Lba             Lba of the target block.

  @retval EFI_SUCCESS            Spare block content is copied to target block.
  @retval EFI_INVALID_PARAMETER  Input parameter error.
  @retval EFI_OUT_OF_RESOURCES   Allocate memory error.
  @retval EFI_ABORTED            The function could not complete successfully.

**/
EFI_STATUS
FlushSpareBlockToTargetBlock (
  EFI_FTW_LITE_DEVICE                 *FtwLiteDevice,
  EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL  *FvBlock,
  IN EFI_PHYSICAL_ADDRESS             FvBaseAddr,
  EFI_LBA                             Lba
  );

/**
  Copy the content of spare block to working block. Size is FTW_BLOCK_SIZE. Spare block is
  accessed by FTW backup FVB protocol interface. LBA is FtwLiteDevice->FtwSpareLba. Working
  block is accessed by FTW working FVB protocol interface. LBA is FtwLiteDevice->FtwWorkBlockLba.

  @param  FtwLiteDevice          The private data of FTW_LITE driver.

  @retval EFI_SUCCESS            Spare block content is copied to target block.
  @retval EFI_OUT_OF_RESOURCES   Allocate memory error.
  @retval EFI_ABORTED            The function could not complete successfully.

Notes:
    Since the working block header is important when FTW initializes, the state of the operation
    should be handled carefully. The Crc value is calculated without STATE element.

**/
EFI_STATUS
FlushSpareBlockToWorkingBlock (
  EFI_FTW_LITE_DEVICE                 *FtwLiteDevice
  );

/**
  Copy the content of spare block to a boot block. Size is FTW_BLOCK_SIZE. Spare block is accessed
  by FTW backup FVB protocol interface. LBA is FtwLiteDevice->FtwSpareLba. Boot block is accessed
  by BootFvb protocol interface. LBA is 0.

  @param  FtwLiteDevice          The private data of FTW_LITE driver.

  @retval EFI_SUCCESS            Spare block content is copied to boot block.
  @retval EFI_INVALID_PARAMETER  Input parameter error.
  @retval EFI_OUT_OF_RESOURCES   Allocate memory error.
  @retval EFI_ABORTED            The function could not complete successfully.

**/
EFI_STATUS
FlushSpareBlockToBootBlock (
  EFI_FTW_LITE_DEVICE                 *FtwLiteDevice
  );

/**
  Update a bit of state on a block device. The location of the bit is calculated by the
  (Lba, Offset, bit). Here bit is determined by the the name of a certain bit.

  @param  FtwLiteDevice    The private data of FTW_LITE driver.
  @param  FvBlock          FVB Protocol interface to access SrcBlock and DestBlock.
  @param  Lba              Lba of a block.
  @param  Offset           Offset on the Lba.
  @param  Address          The Address of FvState.
  @param  NewBit           New value that will override the old value if it can be change.

  @retval EFI_SUCCESS      A state bit has been updated successfully
  @retval EFI_ABORTED      Read block fail.
  @return Others           Access block device error.

**/
EFI_STATUS
FtwUpdateFvState (
  IN EFI_FTW_LITE_DEVICE                 *FtwLiteDevice,
  IN EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL  *FvBlock,
  IN EFI_LBA                             Lba,
  IN UINTN                               Offset,
  IN UINTN                               Address,
  IN UINT8                               NewBit
  );

/**
  Get the last Write record pointer. The last record is the record whose 'complete' state hasn't
  been set. After all, this header may be a EMPTY header entry for next Allocate.

  @param  FtwLiteDevice    Private data of this driver.
  @param  FtwLastRecord    Pointer to retrieve the last write record.

  @retval EFI_SUCCESS      Get the last write record successfully.
  @retval EFI_ABORTED      The FTW work space is damaged.

**/
EFI_STATUS
FtwGetLastRecord (
  IN  EFI_FTW_LITE_DEVICE  *FtwLiteDevice,
  OUT EFI_FTW_LITE_RECORD  **FtwLastRecord
  );

/**
  Check whether a flash buffer is erased.

  @param  Polarity    All 1 or all 0.
  @param  Buffer      Buffer to check.
  @param  BufferSize  Size of the buffer.

  @retval TRUE        This is an erased buffer.
  @retval FALSE       This isn't an erased buffer.

**/
BOOLEAN
IsErasedFlashBuffer (
  IN BOOLEAN         Polarity,
  IN UINT8           *Buffer,
  IN UINTN           BufferSize
  );

/**
  Initialize a work space when there is no work space.

  @param  WorkingHeader    Pointer of working block header.

  @retval EFI_SUCCESS      The function completed successfully.
  @retval EFI_ABORTED      The function could not complete successfully.

**/
EFI_STATUS
InitWorkSpaceHeader (
  IN EFI_FAULT_TOLERANT_WORKING_BLOCK_HEADER *WorkingHeader
  );

/**
  Read from working block to refresh the work space in memory.

  @param  FtwLiteDevice    Point to private data of FTW driver.

  @retval EFI_SUCCESS      The function completed successfully.
  @retval EFI_ABORTED      The function could not complete successfully.

**/
EFI_STATUS
WorkSpaceRefresh (
  IN EFI_FTW_LITE_DEVICE  *FtwLiteDevice
  );

/**
  Check to see if it is a valid work space.

  @param  WorkingHeader    Pointer of working block header.

  @retval EFI_SUCCESS      The function completed successfully.
  @retval EFI_ABORTED      The function could not complete successfully.

**/
BOOLEAN
IsValidWorkSpace (
  IN EFI_FAULT_TOLERANT_WORKING_BLOCK_HEADER *WorkingHeader
  );


/**
  Reclaim the work space. Get rid of all the completed write records and write records in
  the Fault Tolerant work space.

  @param  FtwLiteDevice         Point to private data of FTW driver.
  @param  FtwSpaceBuffer        Buffer to contain the reclaimed clean data.
  @param  BufferSize            Size of the FtwSpaceBuffer.

  @retval EFI_SUCCESS           The function completed successfully.
  @retval EFI_BUFFER_TOO_SMALL  The FtwSpaceBuffer is too small.
  @retval EFI_ABORTED           The function could not complete successfully.

**/
EFI_STATUS
CleanupWorkSpace (
  IN EFI_FTW_LITE_DEVICE  *FtwLiteDevice,
  IN OUT UINT8            *BlockBuffer,
  IN UINTN                BufferSize
  );


/**
  Reclaim the work space on the working block.

  @param  FtwLiteDevice         Point to private data of FTW driver.

  @retval EFI_SUCCESS           The function completed successfully.
  @retval EFI_OUT_OF_RESOURCES  Allocate memory error.
  @retval EFI_ABORTED           The function could not complete successfully.

**/
EFI_STATUS
FtwReclaimWorkSpace (
  IN EFI_FTW_LITE_DEVICE  *FtwLiteDevice
  );

/**
  Read all data from whole work block.

  @param  FtwLiteDevice          Point to private data of FTW driver.
  @param  Buffer                 Buffer to save work block data.

  @retval EFI_SUCCESS            The function completed successfully.
  @retval EFI_ABORTED            The function could not complete successfully.
  @retval EFI_INVALID_PARAMETER  Some input parameter is invalid.

**/
EFI_STATUS
FtwLiteReadWorkBlock (
  IN  EFI_FTW_LITE_DEVICE                 *FtwLiteDevice,
  OUT UINT8                               *Buffer
  );

/**
  Read all data from whole work block.

  @param  FtwLiteDevice          Point to private data of FTW driver.
  @param  Buffer                 Buffer to save work block data.

  @retval EFI_SUCCESS            The function completed successfully.
  @retval EFI_ABORTED            The function could not complete successfully.
  @retval EFI_INVALID_PARAMETER  Some input parameter is invalid.

**/
EFI_STATUS
FtwLiteReadSpareBlock (
  IN  EFI_FTW_LITE_DEVICE                     *FtwLiteDevice,
  OUT UINT8                                   *Buffer
  );

/**
  Initialize CRC32 table.

**/
VOID
RuntimeDriverInitializeCrc32Table (
  VOID
  );

/**
  Calculate CRC32 for target data.

  @param  Data                   The target data.
  @param  DataSize               The target data size.
  @param  CrcOut                 The CRC32 for target data.

  @retval EFI_SUCCESS            The CRC32 for target data is calculated successfully.
  @retval EFI_INVALID_PARAMETER  Some parameter is not valid, so the CRC32 is not calculated.

**/
EFI_STATUS
EFIAPI
RuntimeDriverCalculateCrc32 (
  IN  VOID    *Data,
  IN  UINTN   DataSize,
  OUT UINT32  *CrcOut
  );

/**

  This function uses to allocate runtimeservicesdata memory in protected mode before before exit
  boot services event.


  Size - The size of buffer to allocate

Returns:

  NULL    - Buffer unsuccessfully allocated.

  Other   - Buffer successfully allocated.

**/
VOID *
AllocateZeroRuntimeDataBuffer (
  IN UINTN                  Size
  );

/**
  According to system mode to allocate pool. Allocate pool from pre-allocate RuntimeServicesData in protect mode.
  Allocate EfiRuntimeServicesData (actually this memory is located in the range of SMM RAM)
  in SMM mode.

  @param  FtwLiteDevice          Point to private data of FTW driver.
  @param  Size                   The size of buffer to allocate.

  @retval NULL                   Allocate buffer failed.
  @return EFI_INVALID_PARAMETER  Start address of allocated buffer.

**/
VOID *
FtwAllocateZeroBuffer (
  IN EFI_FTW_LITE_DEVICE    *FtwLiteDevice,
  IN UINTN                  Size
  );

/**
  This function uses to free memory which allocated by FtwAllocateZeroBuffer ()

  @param  FtwLiteDevice          Point to private data of FTW driver.
  @param  Buffer                 [in]:Double pointer saved the pointer of buffer address which want to free
                                 [out]:Double pointer Saved NULL pointer to indicate allocated memory is freed.

  @retval EFI_SUCCESS            Memory return to system
  @return EFI_INVALID_PARAMETER  Buffer was invalid.

**/
EFI_STATUS
FtwFreePool (
  IN     EFI_FTW_LITE_DEVICE    *FtwLiteDevice,
  IN OUT VOID                   **Buffer
  );

/**
  Check this input handle is whether a NV store FVB handle.

  @param[in] Handle    Input EFI_HANDLE instance

  @retval TRUE         This is NV storage FVB handle.
  @retval FALSE        This isn't NV storage FVB handle.
**/
BOOLEAN
IsNvStorageHandle (
  EFI_HANDLE      Handle
  );

extern  EFI_SMM_SYSTEM_TABLE2          *mSmst;

#ifndef MDEPKG_NDEBUG
extern UINTN mFtwLiteInfo;
extern UINTN mFtwLiteError;
#endif

#endif
