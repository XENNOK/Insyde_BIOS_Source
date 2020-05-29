//;******************************************************************************
//;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
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
/** @file
  Internal include file for Tiano Compress Libary.

  Copyright (c) 2006, Intel Corporation. All rights reserved.
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/


#ifndef _TIANO_COMPRESS_PROTOCOL_H_
#define _TIANO_COMPRESS_PROTOCOL_H_

#include "Tiano.h"


#define TIANO_COMPRESS_PROTOCOL_GUID \
  { \
    0x0E1C2F09, 0xA27D, 0x47E8, 0xB4, 0xA0, 0x15, 0xB2, 0x29, 0x59, 0xFA, 0xA0  \
  }



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


EFI_FORWARD_DECLARATION (TIANO_COMPRESS_PROTOCOL);

typedef
EFI_STATUS
(EFIAPI *TIANO_COMPRESS) (
  IN      UINT8                 *SrcBuffer,
  IN      UINT32                SrcSize,
  IN      UINT8                 *DstBuffer,
  IN OUT  UINT32                *DstSize
  );


typedef struct _TIANO_COMPRESS_PROTOCOL{
  TIANO_COMPRESS                Compress;
} TIANO_COMPRESS_PROTOCOL;

extern EFI_GUID gTianoCompressProtocolGuid;

#endif
