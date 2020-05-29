/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
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

  JpegDecoder.c

Abstract:

  This code supports a the private implementation
  of the JPEG Decoder protocol
--*/

#include "JpegDecoder.h"
#include "McuDecode.h"

JPEG_DECODER_INSTANCE   mPrivateData;
extern EFI_JPEG_DECODER_DATA   mDecoderData;




/**
 Decodes a JFIF image into a UGA formatted image,
 and returns the decoded image, image¡¯s width and image¡¯s height

 @param [in]   This             Protocol instance structure
 @param [in]   ImageData        The data of the JFIF image, which will be decoded
 @param [in]   ImageDataSize    The size in bytes of ImageData
 @param [out]  DecodedData      The decoded data, this output parameter contains
                                a newly allocated memory space, and it is the
                                caller¡¯s responsibility to free this memory buffer.
 @param [out]  DecodedDataSize  The size in bytes of DecodedData
 @param [out]  Height           The height of the image¡¯s displaying
 @param [out]  Width            The width of the image¡¯s displaying
 @param [out]  DecoderStatus    The status of the decoding progress, defined in
                                \Protocol\JpegDecoder\JpegDecoder.h.

 @retval EFI_SUCCESS            The JFIF image is decoded successfully.
 @retval EFI_INVALID_PARAMETER  Either one of ImageData, Width, Height, DecodedDataSize
                                and DecoderStatus is NULL, or ImageDataSize is zero.
 @retval EFI_OUT_OF_RESOURCES   The memory for DecodedData could not be allocated.
 @retval EFI_UNSUPPORTED        The JFIF image can not be decoded, and the detail error info
                                will be returned by the output parameter ¡°DecoderStatus¡±.

**/
EFI_STATUS
EFIAPI
JpegDecoderDecodeImage (
  IN     EFI_JPEG_DECODER_PROTOCOL     *This,
  IN     UINT8                         *ImageData,
  IN     UINTN                         ImageDataSize,
     OUT UINT8                         **DecodedData,
     OUT UINTN                         *DecodedDataSize,
     OUT UINTN                         *Height,
     OUT UINTN                         *Width,
     OUT EFI_JPEG_DECODER_STATUS       *DecoderStatus
  )
{
  EFI_STATUS            Status;
  BOOLEAN               IntervalFlag;
  BOOLEAN               IsEnd;
  UINT16                McuCount;
  INT16                 McuSrcBuff[10*64];
  INT16                 McuDstBuff[12*64];
  UINT16                CurHPixel;
  UINT16                CurVLine;
  UINT64                BltBufferSize;

  if (ImageData == NULL
       || ImageDataSize == 0
       || DecodedDataSize == NULL
       || Height == NULL
       || Width == NULL
       || DecoderStatus == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  *DecoderStatus = InitJfifData (ImageData, ImageDataSize);
  if (*DecoderStatus != EFI_JPEG_DECODE_SUCCESS) {
    return EFI_UNSUPPORTED;
  }

  *Width = mJfifData.Sof0Data.Width;
  *Height = mJfifData.Sof0Data.Height;
  BltBufferSize = mJfifData.Sof0Data.Width * mJfifData.Sof0Data.Height * sizeof (EFI_UGA_PIXEL);
  if (BltBufferSize >= 0x100000000LL) {
    return EFI_UNSUPPORTED;
  }
  *DecodedDataSize = (UINTN) BltBufferSize;
  Status = gBS->AllocatePool(EfiBootServicesData, *DecodedDataSize, (VOID **)DecodedData);
  if (EFI_ERROR(Status)) {
    return EFI_OUT_OF_RESOURCES;
  }
  ZeroMem (*DecodedData, *DecodedDataSize);
  CurHPixel = 0;
  CurVLine = 0;
  IntervalFlag = FALSE;

  IsEnd = FALSE;
  McuCount = 0;
  InitDecoderData();
  while (!IsEnd) {
    SetMem(&McuDstBuff, sizeof(McuDstBuff), 0);
    if (McuRetrieve((INT16 *)&McuSrcBuff, IntervalFlag, &IsEnd, DecoderStatus) != EFI_SUCCESS) {
      break;
    }
    McuCount++;
    if ((mJfifData.McuRestart)&&(McuCount % mJfifData.McuRestart==0)) {
      IntervalFlag = TRUE;
    } else {
      IntervalFlag = FALSE;
    }
    McuDecode ((INT16 *)&McuSrcBuff, (INT16 *)&McuDstBuff);
    StoreMcuToUgaBlt ((INT16 *)&McuDstBuff, DecodedData, &CurHPixel, &CurVLine);
  }

  return EFI_SUCCESS;
}


/**
 Get a special Marker info of the JFIF image

 @param [in]   This             Protocol instance structure
 @param [in]   Start            The start of the JFIF image,
                                or the output Next parameter from a previous call.
 @param [in]   End              The end of the JFIF image.
 @param [in, out] MarkerType    The type of the marker in the JFIF image
 @param [out]  MarkerData       The pointer of the marker specified by the special MarkerType in the JFIF image.
 @param [out]  DataSize         The size in bytes of MarkerData (with the marker bytes and length bytes).
 @param [out]  Next             The next pointer following the "MarkerType" marker,
                                it is next marker pointer, or the compressed data pointer after SOS marker.

 @retval EFI_SUCCESS            The marker¡¯s information is retrieved sucessfully.
 @retval EFI_INVALID_PARAMETER  Either one of Start, End and DataSize is NULL,
                                or End is less than Start.
 @retval EFI_NOT_FOUND          The marker can not be found in the JFIF image.

**/
EFI_STATUS
EFIAPI
JpegDecoderGetMarkerData (
  IN     EFI_JPEG_DECODER_PROTOCOL     *This,
  IN     UINT8                         *Start,
  IN     UINT8                         *End,
  IN OUT EFI_JPEG_MARKER_TYPE          *MarkerType,
     OUT UINT8                         **MarkerData,
     OUT UINT32                        *DataSize,
     OUT UINT8                         **Next  OPTIONAL
  )
{
  UINT8                 *ImagePtr;
  UINT16                i;

  i = 0;
  ImagePtr = Start;
  if (Start == NULL
       || End == NULL
       || End < Start
       || DataSize == NULL) {

    return EFI_INVALID_PARAMETER;
  }

  while (ImagePtr < End) {
    //
    // The ImagePtr is not a Marker's pointer
    //
    if (*ImagePtr != 0xFF || (*ImagePtr == 0xFF && *(ImagePtr + 1) == 0x00)) {
      ImagePtr++;
      continue;
    }

    if (*(ImagePtr + 2) == 0xFF) {
      //
      // The ImagePtr is a marker's pointer, and this marker does not have data info
      //
      *DataSize = 2;
    } else {
      *DataSize = (*(ImagePtr + 2) << 8) + *(ImagePtr + 3) + 2;
    }
    if (*MarkerType == JPEG_ANY) {
      //
      //Find the first marker following the Start pointer which has data info.
      //
      *MarkerType = *(ImagePtr + 1);
      *MarkerData = ImagePtr;
      if (Next != NULL) {
        *Next = ImagePtr + *DataSize;
      }

      return EFI_SUCCESS;
    } else if ( *(ImagePtr + 1) == *MarkerType) {
      //
      // The ImagePtr is a marker's pointer, and this marker have info
      //
      *MarkerData = ImagePtr;
      if (Next != NULL) {
        *Next = ImagePtr + *DataSize;
      }

      return EFI_SUCCESS;
    } else {
      ImagePtr += *DataSize;
    }
  }
  *MarkerData = NULL;
  *DataSize = 0;
  if (Next != NULL) {
    *Next = NULL;
  }

  return EFI_NOT_FOUND;
}



/**
 Install Driver to produce JPEG Decoder protocol.

 @param [in]   ImageHandle
 @param [in]   SystemTable

 @retval EFI_SUCCESS            JPEG Decoder protocol installed
 @return Other        No protocol installed, unload driver.

**/
EFI_STATUS
EFIAPI
JpegDecoderInstall (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS              Status;

  //
  // Initialize Private Data required by this protocol
  //
  mPrivateData.Signature = JPEG_DECODER_INSTANCE_SIGNATURE;
  mPrivateData.JpegDecoder.DecodeImage = JpegDecoderDecodeImage;
  mPrivateData.JpegDecoder.GetMarkerData = JpegDecoderGetMarkerData;

  mPrivateData.Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &mPrivateData.Handle,
                  &gEfiJpegDecoderProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mPrivateData.JpegDecoder
                  );
  return Status;
}