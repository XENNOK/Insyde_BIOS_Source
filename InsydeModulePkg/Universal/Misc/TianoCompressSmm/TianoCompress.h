/** @file

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/** @file
  Internal include file for Tiano Decompress Libary.

  Copyright (c) 2006, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

  Module Name:  TianoDecompress.h

**/

#ifndef __TIANO_COMPRESS_H__
#define __TIANO_COMPRESS_H__

#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>

#define BITBUFSIZ 32
#define MAXMATCH  256
#define THRESHOLD 3
#define CODE_BIT  16
#define BAD_TABLE - 1

typedef INT32 NODE;

//
// C: Char&Len Set; P: Position Set; T: exTra Set
//
#define NC      (0xff + MAXMATCH + 2 - THRESHOLD)
#define CBIT    9
#define MAXPBIT 5
#define TBIT    5
#define MAXNP   ((1U << MAXPBIT) - 1)
#define NT      (CODE_BIT + 3)
#if NT > MAXNP
#define NPT NT
#else
#define NPT MAXNP
#endif

typedef struct {
  UINT8   *mSrcBase;  // Starting address of compressed data
  UINT8   *mDstBase;  // Starting address of decompressed data
  UINT32  mOutBuf;
  UINT32  mInBuf;

  UINT16  mBitCount;
  UINT32  mBitBuf;
  UINT32  mSubBitBuf;
  UINT16  mBlockSize;
  UINT32  mCompSize;
  UINT32  mOrigSize;

  UINT16  mBadTableFlag;

  UINT16  mLeft[2 * NC - 1];
  UINT16  mRight[2 * NC - 1];
  UINT8   mCLen[NC];
  UINT8   mPTLen[NPT];
  UINT16  mCTable[4096];
  UINT16  mPTTable[256];

  //
  // The length of the field 'Position Set Code Length Array Size' in Block Header.
  // For EFI 1.1 de/compression algorithm, mPBit = 4
  // For Tiano de/compression algorithm, mPBit = 5
  //
  UINT8   mPBit;
} SCRATCH_DATA;

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

//
// Function Prototypes
//
EFI_STATUS
EFIAPI
TianoCompress (
  IN      UINT8   *SrcBuffer,
  IN      UINT32  SrcSize,
  IN      UINT8   *DstBuffer,
  IN OUT  UINT32  *DstSize
  );

EFI_STATUS
GetFileContents (
  IN char    *InputFileName,
  OUT UINT8   *FileBuffer,
  OUT UINT32  *BufferLength
  );

STATIC
VOID
PutDword(
  IN UINT32 Data
  );

#if 0
STATIC
EFI_STATUS
AllocateMemory (
  VOID
  );

STATIC
VOID
FreeMemory (
  VOID
  );
#endif

STATIC
VOID
InitSlide (
  VOID
  );

STATIC
NODE
Child (
  IN NODE   NodeQ,
  IN UINT8  CharC
  );

STATIC
VOID
MakeChild (
  IN NODE  NodeQ,
  IN UINT8 CharC,
  IN NODE  NodeR
  );

STATIC
VOID
Split (
  IN NODE Old
  );

STATIC
VOID
InsertNode (
  VOID
  );

STATIC
VOID
DeleteNode (
  VOID
  );

STATIC
VOID
GetNextMatch (
  VOID
  );

STATIC
EFI_STATUS
Encode (
  VOID
  );

STATIC
VOID
CountTFreq (
  VOID
  );

STATIC
VOID
WritePTLen (
  IN INT32 Number,
  IN INT32 nbit,
  IN INT32 Special
  );

STATIC
VOID
WriteCLen (
  VOID
  );

STATIC
VOID
EncodeC (
  IN INT32 Value
  );

STATIC
VOID
EncodeP (
  IN UINT32 Value
  );

STATIC
VOID
SendBlock (
  VOID
  );

STATIC
VOID
Output (
  IN UINT32 c,
  IN UINT32 p
  );

STATIC
VOID
HufEncodeStart (
  VOID
  );

STATIC
VOID
HufEncodeEnd (
  VOID
  );

STATIC
VOID
MakeCrcTable (
  VOID
  );


STATIC
VOID
PutBits (
  IN INT32  Number,
  IN UINT32 Value
  );

STATIC
INT32
FreadCrc (
  OUT UINT8 *Pointer,
  IN  INT32 Number
  );

STATIC
VOID
InitPutBits (
  VOID
  );

STATIC
VOID
CountLen (
  IN INT32 Index
  );

STATIC
VOID
MakeLen (
  IN INT32 Root
  );

STATIC
VOID
DownHeap (
  IN INT32 Index
  );

STATIC
VOID
MakeCode (
  IN  INT32       Number,
  IN  UINT8 Len[  ],
  OUT UINT16 Code[]
  );

STATIC
INT32
MakeTree (
  IN  INT32            NParm,
  IN  UINT16  FreqParm[],
  OUT UINT8   LenParm[ ],
  OUT UINT16  CodeParm[]
  );

#endif
