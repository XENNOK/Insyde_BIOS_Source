/** @file

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
#ifndef _FILE_SELECT_UI_H_
#define _FILE_SELECT_UI_H_

//
// File Select UI GUID
//
#define FILE_SELECT_UI_PROTOCOL_GUID \
  { 0xC9C42895, 0x1596, 0x4ed8, { 0x96, 0x8B, 0xAC, 0x4C, 0xC0, 0x59, 0x1A, 0xE7 } }

typedef struct _FILE_SELECT_UI_PROTOCOL FILE_SELECT_UI_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *FILE_OPERATION_SAVE_WITH_UI) (
  VOID                       *BufferPtr,
  UINTN                      *BufferSize,
  CHAR16                     *FileName
  );

typedef
EFI_STATUS
(EFIAPI *FILE_OPERATION_LOAD_WITH_UI) (
  VOID                       **BufferPtr,
  UINTN                      *BufferSize
  );

//
// File Select UI Protocol
//
typedef struct _FILE_SELECT_UI_PROTOCOL {
  FILE_OPERATION_SAVE_WITH_UI       SaveFileOperation;
  FILE_OPERATION_LOAD_WITH_UI       LoadFileOperation;
};


//
// Extern the GUID for protocol users.
//
extern EFI_GUID gFileSelectUIProtocolGuid;

#endif //_FILE_SELECT_UI_H_