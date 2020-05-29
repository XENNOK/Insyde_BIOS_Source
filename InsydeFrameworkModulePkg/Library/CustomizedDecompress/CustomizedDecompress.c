//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
--*/
/*++

Copyright (c)  1999 - 2002 Intel Corporation.
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the CPL License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/cpl1.0.php

THE PROGRAM IS DISTRIBUTED UNDER THE CPL LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  CustomizedDecompress.c

Abstract:

  Implementation file for decompression routine

--*/

#include "CustomizedDecompress.h"

EFI_CUSTOMIZED_DECOMPRESS_PROTOCOL  mCustomizedDecompress = {
  CustomizedGetInfo,
  CustomizedDecompress
};

EFI_STATUS
InstallCustomizedDecompress (
  EFI_CUSTOMIZED_DECOMPRESS_PROTOCOL  **This
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  This  - GC_TODO: add argument description

Returns:

  EFI_SUCCESS - GC_TODO: Add description for return value

--*/
{
  *This = &mCustomizedDecompress;
  return EFI_SUCCESS;
}

EFI_STATUS
ParseLzma (
  IN      VOID          *Source,
  IN      UINT32        SrcSize,
  OUT     UINT32        *DstSize,
  OUT     UINT32        *ScratchSize,
  OUT     LZMAPARAMETER *Param
  )
/*++

Routine Description:

  The implementation of LZMA header parsing.

Arguments:

  Source      - The source buffer containing the compressed data.
  SrcSize     - The size of source buffer
  DstSize     - The size of destination buffer.
  ScratchSize - The size of scratch buffer.
  Param       - The parameter used by LZMA.

Returns:

  EFI_SUCCESS           - The size of destination buffer and the size of scratch buffer are successull retrieved.
  EFI_INVALID_PARAMETER - The source data is corrupted

--*/
{
  UINT8   Prop0;
  UINT32  Lc;
  UINT32  Lp;
  UINT32  Pb;
  UINT8   *SrcPtr;
  UINTN   Index;
  UINT32  OutSize;
  UINT32  InternalSize;
  UINT32  DictionarySize;

  if (Source == NULL || DstSize == NULL || ScratchSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (SrcSize < 13) {
    return EFI_INVALID_PARAMETER;
  }

  SrcPtr          = (UINT8 *) Source;

  Prop0           = *SrcPtr;
  DictionarySize  = 0;

  if (Param != NULL) {
    for (Index = 0; Index < 5; Index++) {
      Param->Properties[Index] = *SrcPtr++;
    }
  } else {
    SrcPtr += 5;
  }

  OutSize = 0;
  for (Index = 0; Index < 4; Index++) {
    OutSize += ((UINT32) (*SrcPtr++) << (Index * 8));
  }

  if (OutSize == 0xFFFFFFFF) {
    return EFI_INVALID_PARAMETER;
  }

  for (Index = 0; Index < 4; Index++) {
    if (*SrcPtr++ != 0) {
      return EFI_INVALID_PARAMETER;
    }
  }

  if (Prop0 >= (9 * 5 * 5)) {
    return EFI_INVALID_PARAMETER;
  }

  for (Pb = 0; Prop0 >= (9 * 5); Pb++, Prop0 -= (9 * 5))
    ;
  for (Lp = 0; Prop0 >= 9; Lp++, Prop0 -= 9)
    ;
  Lc            = Prop0;

  InternalSize  = (LZMA_BASE_SIZE + (LZMA_LIT_SIZE << (Lc + Lp))) * sizeof (CProb);

#ifdef _LZMA_OUT_READ
  InternalSize += 100;
#endif

  DictionarySize = 0;

#ifdef _LZMA_OUT_READ
  SrcPtr          = (UINT8 *) Source;
  for (Index = 0; Index < 4; Index++) {
    DictionarySize += (UINT32) (SrcPtr[1 + Index]) << (Index * 8);
  }
#endif

  if (Param != NULL) {
    Param->Lc             = Lc;
    Param->Lp             = Lp;
    Param->Pb             = Pb;

    Param->InternalSize   = InternalSize;
    Param->DictionarySize = DictionarySize;
  }

  *DstSize = OutSize;

  //
  // Request longer space to allow align
  //
  *ScratchSize = InternalSize + DictionarySize + 32;

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
CustomizedGetInfo (
  IN EFI_CUSTOMIZED_DECOMPRESS_PROTOCOL     *This,
  IN      VOID                              *Source,
  IN      UINT32                            SrcSize,
  OUT     UINT32                            *DstSize,
  OUT     UINT32                            *ScratchSize
  )
/*++

Routine Description:

  The implementation of LZMA GetInfo().

Arguments:

  Source      - The source buffer containing the compressed data.
  SrcSize     - The size of source buffer
  DstSize     - The size of destination buffer.
  ScratchSize - The size of scratch buffer.

Returns:

  EFI_SUCCESS           - The size of destination buffer and the size of scratch buffer are successull retrieved.
  EFI_INVALID_PARAMETER - The source data is corrupted

--*/
// GC_TODO:    This - add argument and description to function comment
{
  return ParseLzma (
          Source,
          SrcSize,
          DstSize,
          ScratchSize,
          NULL
          );
}

#ifdef _LZMA_IN_CB
typedef struct _CBuffer {
  ILzmaInCallback InCallback;
  UINT8           *Buffer;
  UINT32          Size;
} CBuffer;

INT32
LzmaReadCompressed (
  VOID   *Object,
  UINT8  **Buffer,
  UINT32 *Size
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  Object  - GC_TODO: add argument description
  Buffer  - GC_TODO: add argument description
  Size    - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
{
  CBUFFER *Bo;
  Bo      = (CBuffer *) Object;
  *Size   = Bo->Size; /* You can specify any available size here */
  *Buffer = Bo->Buffer;
  Bo->Buffer += *Size;
  Bo->Size -= *Size;
  return LZMA_RESULT_OK;
}
#endif

EFI_STATUS
EFIAPI
CustomizedDecompress (
  IN EFI_CUSTOMIZED_DECOMPRESS_PROTOCOL     *This,
  IN      VOID                              *Source,
  IN      UINT32                            SrcSize,
  IN OUT  VOID                              *Destination,
  IN      UINT32                            DstSize,
  IN OUT  VOID                              *Scratch,
  IN      UINT32                            ScratchSize
  )
/*++

Routine Description:

  The implementation of LZMA Decompress().

Arguments:

  This        - The protocol instance pointer
  Source      - The source buffer containing the compressed data.
  SrcSize     - The size of source buffer
  Destination - The destination buffer to store the decompressed data
  DstSize     - The size of destination buffer.
  Scratch     - The buffer used internally by the decompress routine. This  buffer is needed to store intermediate data.
  ScratchSize - The size of scratch buffer.

Returns:

  EFI_SUCCESS           - Decompression is successfull
  EFI_INVALID_PARAMETER - The source data is corrupted

--*/
{
#ifdef _LZMA_IN_CB
  CBuffer       Bo;
#endif
  EFI_STATUS    Status;
  INT32         LzmaStatus;
  LZMAPARAMETER Param;
  UINT8         *SrcPtr;
  UINT32        CompressedSize;
  UINT32        OutSize;
  UINT32        RequiredScratchSize;
  UINT32        OutSizeProcessed;

  Status = ParseLzma (
            Source,
            SrcSize,
            &OutSize,
            &RequiredScratchSize,
            &Param
            );
  if (EFI_ERROR (Status)) {
    return EFI_INVALID_PARAMETER;
  }

  if (RequiredScratchSize > ScratchSize) {
    return EFI_INVALID_PARAMETER;
  }

  if (OutSize > DstSize) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Skip the LZMA param field
  //
  CompressedSize  = SrcSize - 13;
  SrcPtr          = (UINT8 *) Source + 13;

#ifdef _LZMA_IN_CB
  Bo.InCallback.Read  = LzmaReadCompressed;
  Bo.Buffer           = (unsigned char *) SrcPtr;
  Bo.Size             = CompressedSize;
#endif

#ifdef _LZMA_OUT_READ
  {
    UINT32  NowPos;
    UINT8   *Dictionary;

    Dictionary = ((Param.InternalSize + (UINT8 *) Scratch) + 16) -
      ((UINTN)(Param.InternalSize + (UINT8 *) Scratch) & 0x0F);

      LzmaDecoderInit (
                                                        (UINT8 *) Scratch,
                                                        Param.InternalSize,
                                                        Param.Lc,
                                                        Param.Lp,
                                                        Param.Pb,
                                                        Dictionary,
                                                        Param.DictionarySize,
#ifdef _LZMA_IN_CB
                                                        & Bo.InCallback
#else
                                                        (UINT8 *) SrcPtr,
                                                        CompressedSize
#endif
                                                        );
    LzmaStatus = EFI_INVALID_PARAMETER;
    for (NowPos = 0; NowPos < OutSize;) {
      UINT32  BlockSize;
      UINT32  KBlockSize;

      KBlockSize  = 0x10000;
      BlockSize   = OutSize - NowPos;
      if (BlockSize > KBlockSize) {
        BlockSize = KBlockSize;
      }

      LzmaStatus = LzmaDecode (
                    (UINT8 *) Scratch,
                    ((UINT8 *) Destination) + NowPos,
                    BlockSize,
                    &OutSizeProcessed
                    );
      if (LzmaStatus != 0) {
        return EFI_INVALID_PARAMETER;
      }

      if (OutSizeProcessed == 0) {
        OutSize = NowPos;
        break;
      }

      NowPos += OutSizeProcessed;
    }
  }

#else
  LzmaStatus = LzmaDecode (
                (UINT8 *) Scratch,
                ScratchSize,
                Param.Lc,
                Param.Lp,
                Param.Pb,
#ifdef _LZMA_IN_CB
                & Bo.InCallback,
#else
                (UINT8 *) SrcPtr,
                CompressedSize,
#endif
                (UINT8 *) Destination,
                OutSize,
                &OutSizeProcessed
                );

  OutSize = OutSizeProcessed;
#endif

  if (LzmaStatus != 0) {
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}
