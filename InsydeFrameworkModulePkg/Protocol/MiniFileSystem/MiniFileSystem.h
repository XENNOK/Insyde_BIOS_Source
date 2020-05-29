//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _MINI_FILE_SYSTEM_PROTOCOL_
#define _MINI_FILE_SYSTEM_PROTOCOL_

#define MINI_FILE_SYSTEM_PROTOCOL_GUID  \
  { \
    0x5ac2c5d1, 0x99bb, 0x46ca, 0xaa, 0x41, 0xce, 0x2e, 0xb2, 0x2b, 0xe1, 0x67 \
  }

EFI_FORWARD_DECLARATION (MINI_FILE_SYSTEM_PROTOCOL);

#ifndef SEEK_SET
    #define SEEK_SET    0
#endif

#ifndef SEEK_CUR
    #define SEEK_CUR    1
#endif

#ifndef SEEK_END
    #define SEEK_END    2
#endif

typedef
EFI_STATUS
(EFIAPI *MINI_FILE_SYSTEM_OPEN) (
  IN MINI_FILE_SYSTEM_PROTOCOL    *This,
  IN UINT16                       *FileName,
  IN UINTN                        MatchIndex,
  OUT VOID                        **FileHandle
  );

typedef
EFI_STATUS
(EFIAPI *MINI_FILE_SYSTEM_CLOSE) (
  IN MINI_FILE_SYSTEM_PROTOCOL    *This,
  IN VOID                         *FileHandle
  );

typedef
EFI_STATUS
(EFIAPI *MINI_FILE_SYSTEM_READ) (
  IN MINI_FILE_SYSTEM_PROTOCOL    *This,
  IN VOID                         *FileHandle,
  IN OUT UINTN                    *BufferSize,
  OUT VOID                        *Buffer
  );

typedef
EFI_STATUS
(EFIAPI *MINI_FILE_SYSTEM_SEEK) (
  IN MINI_FILE_SYSTEM_PROTOCOL    *This,
  IN VOID                         *FileHandle,
  IN OUT INT64                    *Position,
  IN UINTN                        SeekType
  );

typedef
EFI_STATUS
(EFIAPI *MINI_FILE_SYSTEM_GETSIZE) (
  IN MINI_FILE_SYSTEM_PROTOCOL    *This,
  IN VOID                         *FileHandle,
  OUT INT64                       *Size
  );

typedef
EFI_STATUS
(EFIAPI *MINI_FILE_SYSTEM_GETNEXTFILENAME) (
  IN MINI_FILE_SYSTEM_PROTOCOL    *This,
  IN VOID                         *FileHandle,
  IN UINT16                       *PathName,
  IN UINTN                        BufferLen
  );

//
// MINI_FILE_SYSTEM PROTOCOL
//
typedef struct _MINI_FILE_SYSTEM_PROTOCOL {
  MINI_FILE_SYSTEM_OPEN               Open;
  MINI_FILE_SYSTEM_CLOSE              Close;
  MINI_FILE_SYSTEM_READ               Read;
  MINI_FILE_SYSTEM_SEEK               Seek;
  MINI_FILE_SYSTEM_GETSIZE            GetSize;
  MINI_FILE_SYSTEM_GETNEXTFILENAME    GetNextFileName;
} MINI_FILE_SYSTEM_PROTOCOL;

extern EFI_GUID               gMiniFileSystemProtocolGuid;

#endif