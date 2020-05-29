//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/*++

Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  FaultTolerantWrite.h

Abstract:

  Fault Tolerant Write protocol as defined in the Tiano Fault Tolerant Write
  specification.

  This boot service only protocol provides fault tolerant write capability for
  block devices.  The protocol provides for non-volatile intermediate storage
  of the data and private information a caller would need to recover from a
  critical fault, such as power failure.

--*/

#ifndef _FW_FAULT_TOLERANT_WRITE_PROTOCOL_H_
#define _FW_FAULT_TOLERANT_WRITE_PROTOCOL_H_

#include "EfiFirmwareVolumeHeader.h"
#include EFI_PROTOCOL_CONSUMER (FirmwareVolumeBlock)

#define EFI_FAULT_TOLERANT_WRITE_PROTOCOL_GUID \
  { \
    0x3ebd9e82, 0x2c78, 0x4de6, 0x97, 0x86, 0x8d, 0x4b, 0xfc, 0xb7, 0xc8, 0x81 \
  }

//
// Forward reference for pure ANSI compatability
//
EFI_FORWARD_DECLARATION (EFI_FAULT_TOLERANT_WRITE_PROTOCOL);

//
// Protocol API definitions
//
//
// Forward declaration of protocol data structure
//
typedef struct _EFI_FAULT_TOLERANT_WRITE_PROTOCOL EFI_FAULT_TOLERANT_WRITE_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *EFI_FAULT_TOLERANT_WRITE_GET_MAX_BLOCK_SIZE) (
  IN EFI_FAULT_TOLERANT_WRITE_PROTOCOL    * This,
  OUT UINTN                               *BlockSize
  );

/*++

Routine Description:

  Query the largest block that may be updated in a fault tolerant manner.

Arguments:

  This       - Indicates a pointer to the calling context.
  BlockSize  - A pointer to a caller allocated UINTN that is updated to
               indicate the size of the largest block that can be updated.

Returns:

  EFI_SUCCESS   - The function completed successfully
  EFI_ABORTED   - The function could not complete successfully.

--*/
typedef
EFI_STATUS
(EFIAPI *EFI_FAULT_TOLERANT_WRITE_ALLOCATE) (
  IN EFI_FAULT_TOLERANT_WRITE_PROTOCOL    * This,
  IN EFI_GUID                             * CallerId,
  IN UINTN                                PrivateDataSize,
  IN UINTN                                NumberOfWrites
  );

/*++

Routine Description:

  Allocates space for the protocol to maintain information about writes.
  Since writes must be completed in a fault tolerant manner and multiple
  updates will require more resources to be successful, this function
  enables the protocol to ensure that enough space exists to track
  information about the upcoming writes.

Arguments:

  This             - Indicates a pointer to the calling context.
  CallerId         - The GUID identifying the write.
  PrivateDataSize  - The size of the caller's private data
                     that must be recorded for each write.
  NumberOfWrites   - The number of fault tolerant block writes
                     that will need to occur.

Returns:

  EFI_SUCCESS        - The function completed successfully
  EFI_ABORTED        - The function could not complete successfully.
  EFI_ACCESS_DENIED  - All allocated writes have not been completed.
                       All writes must be completed or aborted before
                       another fault tolerant write can occur.

--*/
typedef
EFI_STATUS
(EFIAPI *EFI_FAULT_TOLERANT_WRITE_WRITE) (
  IN EFI_FAULT_TOLERANT_WRITE_PROTOCOL     * This,
  IN EFI_LBA                               Lba,
  IN UINTN                                 Offset,
  IN UINTN                                 Length,
  IN VOID                                  *PrivateData,
  IN EFI_HANDLE                            FvbHandle,
  IN VOID                                  *Buffer
  );

/*++

Routine Description:

  Starts a target block update. This records information about the write
  in fault tolerant storage and will complete the write in a recoverable
  manner, ensuring at all times that either the original contents or
  the modified contents are available.

Arguments:

  This             - Calling context
  Lba              - The logical block address of the target block.
  Offset           - The offset within the target block to place the data.
  Length           - The number of bytes to write to the target block.
  PrivateData      - A pointer to private data that the caller requires to
                     complete any pending writes in the event of a fault.
  FvBlockHandle    - The handle of FVB protocol that provides services for
                     reading, writing, and erasing the target block.
  Buffer           - The data to write.

Returns:

  EFI_SUCCESS          - The function completed successfully
  EFI_ABORTED          - The function could not complete successfully.
  EFI_BAD_BUFFER_SIZE  - The write would span a block boundary,
                         which is not a valid action.
  EFI_ACCESS_DENIED    - No writes have been allocated.
  EFI_NOT_READY        - The last write has not been completed.
                         Restart () must be called to complete it.

--*/
typedef
EFI_STATUS
(EFIAPI *EFI_FAULT_TOLERANT_WRITE_RESTART) (
  IN EFI_FAULT_TOLERANT_WRITE_PROTOCOL     * This,
  IN EFI_HANDLE                            FvbHandle
  );

/*++

Routine Description:

  Restarts a previously interrupted write. The caller must provide the
  block protocol needed to complete the interrupted write.

Arguments:

  This                - Calling context.
  FvBlockProtocol     - The handle of FVB protocol that provides services for
                        reading, writing, and erasing the target block.

Returns:

  EFI_SUCCESS         - The function completed successfully
  EFI_ABORTED         - The function could not complete successfully.
  EFI_ACCESS_DENIED   - No pending writes exist.

--*/
typedef
EFI_STATUS
(EFIAPI *EFI_FAULT_TOLERANT_WRITE_ABORT) (
  IN EFI_FAULT_TOLERANT_WRITE_PROTOCOL     * This
  );

/*++

Routine Description:

  Aborts all previous allocated writes.

Arguments:

  This - Calling context

Returns:

  EFI_SUCCESS      - The function completed successfully
  EFI_ABORTED      - The function could not complete successfully.
  EFI_NOT_FOUND    - No allocated writes exist.

--*/
typedef
EFI_STATUS
(EFIAPI *EFI_FAULT_TOLERANT_WRITE_GET_LAST_WRITE) (
  IN EFI_FAULT_TOLERANT_WRITE_PROTOCOL     * This,
  OUT EFI_GUID                             * CallerId,
  OUT EFI_LBA                              *Lba,
  OUT UINTN                                *Offset,
  OUT UINTN                                *Length,
  IN OUT UINTN                             *PrivateDataSize,
  OUT VOID                                 *PrivateData,
  OUT BOOLEAN                              *Complete
  );

/*++

Routine Description:

  Starts a target block update. This records information about the write
  in fault tolerant storage and will complete the write in a recoverable
  manner, ensuring at all times that either the original contents or
  the modified contents are available.

Arguments:

  This             - Indicates a pointer to the calling context.
  CallerId         - The GUID identifying the last write.
  Lba              - The logical block address of the last write.
  Offset           - The offset within the block of the last write.
  Length           - The length of the last write.
  PrivateDataSize  - On input, the size of the PrivateData buffer.
                     On output, the size of the private data stored
                     for this write.
  PrivateData      - A pointer to a buffer. The function will copy
                     PrivateDataSize bytes from the private data
                     stored for this write.
  Complete         - A Boolean value with TRUE indicating
                     that the write was completed.

Returns:

  EFI_SUCCESS     - The function completed successfully
  EFI_ABORTED     - The function could not complete successfully.
  EFI_NOT_FOUND   - No allocated writes exist.

--*/

//
// Protocol declaration
//
typedef struct _EFI_FAULT_TOLERANT_WRITE_PROTOCOL {
  EFI_FAULT_TOLERANT_WRITE_GET_MAX_BLOCK_SIZE GetMaxBlockSize;
  EFI_FAULT_TOLERANT_WRITE_ALLOCATE           Allocate;
  EFI_FAULT_TOLERANT_WRITE_WRITE              Write;
  EFI_FAULT_TOLERANT_WRITE_RESTART            Restart;
  EFI_FAULT_TOLERANT_WRITE_ABORT              Abort;
  EFI_FAULT_TOLERANT_WRITE_GET_LAST_WRITE     GetLastWrite;
} EFI_FAULT_TOLERANT_WRITE_PROTOCOL;

extern EFI_GUID gEfiFaultTolerantWriteProtocolGuid;

#endif
