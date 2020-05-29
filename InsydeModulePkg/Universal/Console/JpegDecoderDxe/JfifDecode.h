/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

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

  JfifDecode.h

Abstract:
  This code decode the JFIF image and get the useful information.

--*/

#ifndef _JFIF_DECODE_H_
#define _JFIF_DECODE_H_

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/JpegDecoder.h>

typedef struct _EFI_JPEG_SAMPLE_DATA {
  UINT8                       Hi;
  UINT8                       Vi;
  UINT8                       Blocks;
  UINT8                       QuanTable;
} EFI_JPEG_SAMPLE_DATA;

typedef struct _EFI_JPEG_SOF0_DATA {
  UINT8                       *Ptr;
  UINT8                       Components;
  UINT16                      Width;
  UINT16                      Height;
  EFI_JPEG_SAMPLE_DATA        Samples[3];
} EFI_JPEG_SOF0_DATA;

typedef struct _EFI_JPEG_HUFFMAN_TABLE {
  UINT8   *Ptr;
  UINT8   Number;           //Huffman table number, [0,3]
  UINT8   Type;             //Table class: 0 = DC table, 1 = AC table
  UINT16  MinCode[17];      //Table with minimum value of Huffman code for each layer of the Huffman tree
  UINT16  MaxCode[17];      //Table with maximum value of Huffman code for each layer of the Huffman tree (-1, if none)
  UINT8   Codes[17];        //Table with the number of codes in each layer
  UINT8   FirstCode[17];    //Table with index of 1st symbol of each layer in Huffmanval[]
  UINT8   HuffmanVal[256];  //the length of coding value, or the layers of the Huffman tree
} EFI_JPEG_HUFFMAN_TABLE;

typedef struct EFI_JPEG_DECODER_BLOCK_DATA {
  UINT8                       DcAcHTIndex;
  UINT8                       QTIndex;
  UINT8                       HiViIndex;
  INT16                       *DcValPtr;
} EFI_JPEG_DECODER_BLOCK_DATA;

typedef struct EFI_JPEG_DECODER_DATA {
  UINT8                       *ImagePtr;
  UINT8                       CurByte;
  UINT8                       BitPos;
  UINT8                       Blocks;
  INT16                       DcVal[3];
  EFI_JPEG_DECODER_BLOCK_DATA BlocksInMcu[10];
} EFI_JPEG_DECODER_DATA;


typedef struct EFI_JPEG_JFIF_DATA {
  UINT8                       *SoiPtr;
  UINT8                       *App0Ptr;
  UINT8                       *SosPtr;
  UINT8                       *ComPtr;
  UINT8                       *DnlPtr;
  UINT16                      McuRestart;
  UINT8                       *DqtPtr[4];
  UINT8                       HTMaxIndex;
  UINT8                       QTMaxIndex;
  EFI_JPEG_SOF0_DATA          Sof0Data;
  EFI_JPEG_HUFFMAN_TABLE      HuffTable[8];
} EFI_JPEG_JFIF_DATA;

EFI_JPEG_DECODER_STATUS
GetSof0Data (
  IN     UINT8                        *ImagePtr
  );

EFI_JPEG_DECODER_STATUS
GetHuffmanTable (
  IN     UINT8                        *ImagePtr
  );

EFI_JPEG_DECODER_STATUS
InitJfifData (
  IN     UINT8                         *ImageData,
  IN     UINTN                         ImageDataSize
  );

VOID
InitDecoderData (
  );

extern EFI_JPEG_JFIF_DATA      mJfifData;
extern EFI_JPEG_DECODER_DATA   mDecoderData;
#endif
