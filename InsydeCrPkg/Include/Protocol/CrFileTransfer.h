/** @file
  Console Redirection FileTransfer Protocol definition.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
#ifndef _CR_FILE_TRANSFER_PROTOCOL_H_
#define _CR_FILE_TRANSFER_PROTOCOL_H_


#define CR_FILE_TRANSFER_PROTOCOL_GUID \
  { \
    0x2f33369a, 0x8f47, 0x4f90, 0xb4, 0x1a, 0xe9, 0x10, 0xa6, 0x60, 0xdf, 0x7d \
  }

typedef struct _CR_FILE_TRANSFER_PROTOCOL  CR_FILE_TRANSFER_PROTOCOL;


typedef struct _FT_INFO {
  EFI_HANDLE    TermHandle;
  UINT8         TransMode;
  UINT8         FlowControl;
  UINT8         *DataBuffer;
  UINTN         DataSize;
} FT_INFO;


typedef 
EFI_STATUS
(EFIAPI *FT_TRANSFER_PROCESS) (
  IN  CR_FILE_TRANSFER_PROTOCOL    *This,
  IN  FT_INFO                      *FTInfo
  );


typedef
EFI_STATUS
(EFIAPI *FT_SEND_FORM) (
  IN  CR_FILE_TRANSFER_PROTOCOL    *This
  );


typedef struct _CR_FILE_TRANSFER_PROTOCOL {
  FT_SEND_FORM           SendForm;
  FT_TRANSFER_PROCESS    TransferProcess;
  UINT8                  *BufferPtr;
  UINTN                  DataSize;
} CR_FILE_TRANSFER_PROTOCOL;


extern EFI_GUID gCrFileTransferProtocolGuid;


#endif // _CR_FILE_TRANSFER_PROTOCOL_H_

