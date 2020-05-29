/** @file
  Tiano Compress Protocol

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

#ifndef _TIANO_COMPRESS_PROTOCOL_H_
#define _TIANO_COMPRESS_PROTOCOL_H_

#include <Uefi.h>

#define TIANO_COMPRESS_PROTOCOL_GUID \
  { 0x0E1C2F09, 0xA27D, 0x47E8, 0xB4, 0xA0, 0x15, 0xB2, 0x29, 0x59, 0xFA, 0xA0 }

typedef struct _TIANO_COMPRESS_PROTOCOL TIANO_COMPRESS_PROTOCOL;

#pragma pack(1)
typedef struct {
  UINT32                        SourceSize;
  UINT32                        CompressedSize;
} COMPRESS_DATA_HEADER;

typedef struct {
  UINT32                        NumOfBlock;
  UINT32                        TotalImageSize;
  UINT32                        TotalCompressedDataSize;
} COMPRESS_TABLE_HEADER;
#pragma pack()

typedef
EFI_STATUS
(EFIAPI *TIANO_COMPRESS) (
  IN      UINT8   *SrcBuffer,
  IN      UINT32  SrcSize,
  IN      UINT8   *DstBuffer,
  IN OUT  UINT32  *DstSize
  );

struct _TIANO_COMPRESS_PROTOCOL {
  TIANO_COMPRESS                Compress;
};

extern EFI_GUID gTianoCompressProtocolGuid;


#endif
