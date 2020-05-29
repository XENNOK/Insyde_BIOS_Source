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

Copyright (c)  1999 - 2002 Intel Corporation.
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the CPL License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/cpl1.0.php

THE PROGRAM IS DISTRIBUTED UNDER THE CPL LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  CustomizedDecompress.h

Abstract:

  Header file for decompression routine

--*/

#ifndef _LZMA_DECOMPRESS_H_
#define _LZMA_DECOMPRESS_H_

#define LZMA_BASE_SIZE 1846
#define LZMA_LIT_SIZE 768

#ifdef _LZMA_PROB32
#define CProb UINT32
#else
#define CProb unsigned short
#endif

#define IN
#define OUT
#define VOID    void
#define EFIAPI  __cdecl
#define NULL    ((VOID *) 0)

#ifdef EFIX64
typedef unsigned __int64 UINTN;
typedef __int64 INTN;
#else
typedef unsigned __int32 UINTN;
typedef __int32 INTN;
#endif

typedef unsigned char UINT8;
typedef unsigned __int32 UINT32;
typedef __int32 INT32;
typedef UINTN EFI_STATUS;

#define EFI_MAX_BIT           0x80000000
#define EFIERR(a)             (EFI_MAX_BIT | (a))
#define EFI_SUCCESS           0
#define EFI_INVALID_PARAMETER EFIERR (2)
#define EFI_ERROR(a)          (((INTN) (a)) < 0)

#pragma warning(disable : 4100)

typedef struct _EFI_CUSTOMIZED_DECOMPRESS_PROTOCOL EFI_CUSTOMIZED_DECOMPRESS_PROTOCOL;

typedef struct {
  UINT32  Lc;
  UINT32  Lp;
  UINT32  Pb;
  UINT32  DictionarySize;
  UINT32  InternalSize;
  UINT8   Properties[5];
} LZMAPARAMETER;

#ifdef _LZMA_OUT_READ
INTN LzmaDecoderInit(
    unsigned char *buffer, UINT32 bufferSize,
    INTN lc, INTN lp, INTN pb,
    unsigned char *dictionary, UINT32 dictionarySize,
  #ifdef _LZMA_IN_CB
    ILzmaInCallback *inCallback
  #else
    unsigned char *inStream, UINT32 inSize
  #endif
);
#endif

INTN LzmaDecode(
    unsigned char *buffer,
  #ifndef _LZMA_OUT_READ
    UINT32 bufferSize,
    INTN lc, INTN lp, INTN pb,
  #ifdef _LZMA_IN_CB
    ILzmaInCallback *inCallback,
  #else
    unsigned char *inStream, UINT32 inSize,
  #endif
  #endif
    unsigned char *outStream, UINT32 outSize,
    UINT32 *outSizeProcessed);

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

  GC_TODO: Add function description

Arguments:

  This        - GC_TODO: add argument description
  Source      - GC_TODO: add argument description
  SrcSize     - GC_TODO: add argument description
  DstSize     - GC_TODO: add argument description
  ScratchSize - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

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

  GC_TODO: Add function description

Arguments:

  This        - GC_TODO: add argument description
  Source      - GC_TODO: add argument description
  SrcSize     - GC_TODO: add argument description
  Destination - GC_TODO: add argument description
  DstSize     - GC_TODO: add argument description
  Scratch     - GC_TODO: add argument description
  ScratchSize - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

typedef
EFI_STATUS
(EFIAPI *EFI_CUSTOMIZED_DECOMPRESS_GET_INFO) (
  IN EFI_CUSTOMIZED_DECOMPRESS_PROTOCOL       * This,
  IN   VOID                                   *Source,
  IN   UINT32                                 SourceSize,
  OUT  UINT32                                 *DestinationSize,
  OUT  UINT32                                 *ScratchSize
  );

/*++

Routine Description:

  The GetInfo() function retrieves the size of the uncompressed buffer
  and the temporary scratch buffer required to decompress the buffer
  specified by Source and SourceSize.  If the size of the uncompressed
  buffer or the size of the scratch buffer cannot be determined from
  the compressed data specified by Source and SourceData, then
  EFI_INVALID_PARAMETER is returned.  Otherwise, the size of the uncompressed
  buffer is returned in DestinationSize, the size of the scratch buffer is
  returned in ScratchSize, and EFI_SUCCESS is returned.

  The GetInfo() function does not have scratch buffer available to perform
  a thorough checking of the validity of the source data. It just retrieves
  the 'Original Size' field from the beginning bytes of the source data and
  output it as DestinationSize.  And ScratchSize is specific to the decompression
  implementation.

Arguments:

  This            - The protocol instance pointer
  Source          - The source buffer containing the compressed data.
  SourceSize      - The size, in bytes, of source buffer.
  DestinationSize - A pointer to the size, in bytes, of the uncompressed buffer
                    that will be generated when the compressed buffer specified
                    by Source and SourceSize is decompressed.
  ScratchSize     - A pointer to the size, in bytes, of the scratch buffer that
                    is required to decompress the compressed buffer specified by
                    Source and SourceSize.

Returns:
  EFI_SUCCESS     - The size of the uncompressed data was returned in DestinationSize
                    and the size of the scratch buffer was returned in ScratchSize.
  EFI_INVALID_PARAMETER - The size of the uncompressed data or the size of the scratch
                  buffer cannot be determined from the compressed data specified by
                  Source and SourceData.

--*/
typedef
EFI_STATUS
(EFIAPI *EFI_CUSTOMIZED_DECOMPRESS_DECOMPRESS) (
  IN EFI_CUSTOMIZED_DECOMPRESS_PROTOCOL         * This,
  IN     VOID                                   *Source,
  IN     UINT32                                 SourceSize,
  IN OUT VOID                                   *Destination,
  IN     UINT32                                 DestinationSize,
  IN OUT VOID                                   *Scratch,
  IN     UINT32                                 ScratchSize
  );

/*++

Routine Description:

  The Decompress() function extracts decompressed data to its original form.

  This protocol is designed so that the decompression algorithm can be
  implemented without using any memory services.  As a result, the
  Decompress() function is not allowed to call AllocatePool() or
  AllocatePages() in its implementation.  It is the caller's responsibility
  to allocate and free the Destination and Scratch buffers.

  If the compressed source data specified by Source and SourceSize is
  sucessfully decompressed into Destination, then EFI_SUCCESS is returned.
  If the compressed source data specified by Source and SourceSize is not in
  a valid compressed data format, then EFI_INVALID_PARAMETER is returned.

Arguments:

  This            - The protocol instance pointer
  Source          - The source buffer containing the compressed data.
  SourceSize      - The size of source data.
  Destination     - On output, the destination buffer that contains
                    the uncompressed data.
  DestinationSize - The size of destination buffer. The size of destination
                    buffer needed is obtained from GetInfo().
  Scratch         - A temporary scratch buffer that is used to perform the
                    decompression.
  ScratchSize     - The size of scratch buffer. The size of scratch buffer needed
                    is obtained from GetInfo().

Returns:

  EFI_SUCCESS     - Decompression completed successfully, and the uncompressed
                    buffer is returned in Destination.
  EFI_INVALID_PARAMETER
                  - The source buffer specified by Source and SourceSize is
                    corrupted (not in a valid compressed format).

--*/
typedef struct _EFI_CUSTOMIZED_DECOMPRESS_PROTOCOL {
  EFI_CUSTOMIZED_DECOMPRESS_GET_INFO    GetInfo;
  EFI_CUSTOMIZED_DECOMPRESS_DECOMPRESS  Decompress;
} EFI_CUSTOMIZED_DECOMPRESS_PROTOCOL;

#endif
