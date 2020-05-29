//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _SMM_FTW_PROTO_H_
#define _SMM_FTW_PROTO_H_

EFI_FORWARD_DECLARATION(EFI_SMM_FTW_PROTOCOL);

#define EFI_SMM_FTW_PROTOCOL_GUID \
{ 0x83a108ca, 0x4641, 0x451a, 0x92, 0x1d, 0x9a, 0x42, 0x2f, 0xe4, 0x6b, 0x1b }


typedef
EFI_STATUS
(EFIAPI * EFI_SMM_FTW_WRITE) (
  IN EFI_SMM_FTW_PROTOCOL              *This,
  IN EFI_PHYSICAL_ADDRESS              BaseAddress,
  IN EFI_LBA                           Lba,
  IN UINTN                             Offset,
  IN UINTN                             *NumBytes,
  IN VOID                              *Buffer
  );
/*++

Routine Description:

  Starts a target block update. This records information about the write 
  in fault tolerant storage and will complete the write in a recoverable 
  manner, ensuring at all times that either the original contents or 
  the modified contents are available.

Arguments:

  This             - Calling context
  BaseAddress      - The Firmware Volume BaseAddress of the target block.
  Lba              - The logical block address of the target block.  
  Offset           - The offset within the target block to place the data.
  Length           - The number of bytes to write to the target block.
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

//
// Protocol declaration
//
typedef struct _EFI_SMM_FTW_PROTOCOL {
  EFI_SMM_FTW_WRITE               Write;
} EFI_SMM_FTW_PROTOCOL;

extern EFI_GUID gEfiSmmFtwProtocolGuid;

#endif
