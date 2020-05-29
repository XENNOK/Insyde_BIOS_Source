/** @file
  This code supports a the private implementation
  of the BMP Decoder protocol

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

#include "BmpDecoder.h"

BMP_DECODER_INSTANCE    mPrivateData = {
  BMP_DECODER_INSTANCE_SIGNATURE,
  NULL,
  { BmpDecoderDecodeImage }
};

#define SIZE_4G     0x100000000
//
// BMP compression type.
//
#define BMP_UNCOMPRESSION_TYPE 0
#define BMP_RLE8BIT_TYPE       1
#define BMP_RLE4BIT_TYPE       2

//
// RLE Mode. 
//
#define ENCODE_MODE_END_OF_LINE   0x00
#define ENCODE_MODE_END_OF_BITMAP 0x01
#define ENCODE_MODE_DELTA         0x02
#define ABSOLUTE_MODE_FLAG        0x00

EFI_STATUS
RLE4ToBlt (
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer,   // Target data
  UINT8                         *RLE4Image,   // source data
  BMP_COLOR_MAP                 *BmpColorMap, // Palette
  BMP_IMAGE_HEADER              *BmpHeader    //
  ) 
{
  UINT8         FirstByte;
  UINT8         SecondByte;
  UINTN         Index;
  UINTN         Height;
  UINTN         Width;
  BOOLEAN       EndOfLine;
  BOOLEAN       EndOfBMP;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *Blt;

  EndOfLine = FALSE;
  EndOfBMP = FALSE;
  Height = 0;

  while (Height <= BmpHeader->PixelHeight && EndOfBMP == FALSE) {  
        
    Blt = &BltBuffer[(BmpHeader->PixelHeight - Height - 1) * BmpHeader->PixelWidth];
    Width = 0;
    EndOfLine = FALSE;
        
    while (Width <= BmpHeader->PixelWidth && EndOfLine == FALSE) {

      // 
      // Get a WORD each time
      //
      FirstByte = *RLE4Image;
      RLE4Image++;
      SecondByte = *RLE4Image;
      RLE4Image++;

      if (FirstByte > ABSOLUTE_MODE_FLAG) {
        UINT8 Nibble[2];
        Nibble[0] = (SecondByte & 0x0F);
        Nibble[1] = (SecondByte & 0xF0) >>4;

        for(Index=0; Index<FirstByte; Index++, Width++, Blt++) {
          //
          // The left-most pixel is in the more significant nibble.
          //
          Blt->Red    = BmpColorMap[Nibble[1-Index%2]].Red;
          Blt->Green  = BmpColorMap[Nibble[1-Index%2]].Green;
          Blt->Blue   = BmpColorMap[Nibble[1-Index%2]].Blue;
        }
      }
      else { // FirstByte is zero...
        switch (SecondByte) {
          case ENCODE_MODE_END_OF_LINE:
            EndOfLine = TRUE;      
            break;
            
          case ENCODE_MODE_END_OF_BITMAP:
            EndOfBMP = TRUE;  
            EndOfLine = TRUE; 
            break;

          case ENCODE_MODE_DELTA:
            FirstByte = *RLE4Image;
            RLE4Image++;
        	  SecondByte= *RLE4Image;
            RLE4Image++;
            Width = Width + FirstByte;
            Height = Height + SecondByte;
            Blt = &BltBuffer[((BmpHeader->PixelHeight - Height - 1) * BmpHeader->PixelWidth) + (Width)];          
            break;

          default:
            //
            // Absolute mode. (This block has no compression)
            //
            {              
              UINT16  ByteToBeRead = (((SecondByte-1)/4)+1)*2;
              
              for (Index = 0; Index < ByteToBeRead; Index++) {
                UINT8 NibbleHigh = ((*RLE4Image) & 0xF0) >> 4;
                UINT8 NibbleLow  = (*RLE4Image & 0x0F);
                RLE4Image++;
                
                if(SecondByte != 0) {
                  Blt->Red    = BmpColorMap[NibbleHigh].Red;
                  Blt->Green  = BmpColorMap[NibbleHigh].Green;
                  Blt->Blue   = BmpColorMap[NibbleHigh].Blue;
                  Width++;
                  Blt++;
                  SecondByte--;
                }

                if(SecondByte != 0) {
                  Blt->Red    = BmpColorMap[NibbleLow].Red;
                  Blt->Green  = BmpColorMap[NibbleLow].Green;
                  Blt->Blue   = BmpColorMap[NibbleLow].Blue;
                  Width++;
                  Blt++;
                  SecondByte--;
                }
              }
            }
            break;
        }
      }
    }
    Height++;
  }
  return EFI_SUCCESS;
}

EFI_STATUS
RLE8ToBlt (
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer,
  UINT8                         *RLE8Image,
  BMP_COLOR_MAP                 *BmpColorMap,
  BMP_IMAGE_HEADER              *BmpHeader 
  ) 
{
  UINT8         FirstByte;
  UINT8         SecondByte;
  UINT8         EachValue;
  UINTN         Index;
  UINTN         Height;
  UINTN         Width;
  BOOLEAN       EndOfLine;
  BOOLEAN       EndOfBMP;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *Blt;

  EndOfLine = FALSE;
  EndOfBMP = FALSE;
  Height = 0;

  while (Height <= BmpHeader->PixelHeight && EndOfBMP == FALSE) {  
        
    Blt = &BltBuffer[(BmpHeader->PixelHeight - Height - 1) * BmpHeader->PixelWidth];
    Width = 0;
    EndOfLine = FALSE;
        
    while (Width <= BmpHeader->PixelWidth && EndOfLine == FALSE) {
        
      FirstByte = *RLE8Image;
      RLE8Image++;
      SecondByte = *RLE8Image;
      RLE8Image++;

      if (FirstByte > ABSOLUTE_MODE_FLAG) {
        for (Index = 0; Index < FirstByte; Index++, Width++, Blt++) { 
          Blt->Red    = BmpColorMap[SecondByte].Red;
          Blt->Green  = BmpColorMap[SecondByte].Green;
          Blt->Blue   = BmpColorMap[SecondByte].Blue;
        }
      }
      else {
        switch (SecondByte) {
      
        case ENCODE_MODE_END_OF_LINE:
          EndOfLine = TRUE;      
        break;

        case ENCODE_MODE_END_OF_BITMAP:
          EndOfBMP = TRUE;  
          EndOfLine = TRUE; 
        break;

        case ENCODE_MODE_DELTA:
          FirstByte = *RLE8Image;
          RLE8Image++;
	  SecondByte= *RLE8Image;
          RLE8Image++;
          Width = Width + FirstByte;
          Height = Height + SecondByte;
          Blt = &BltBuffer[((BmpHeader->PixelHeight - Height - 1) * BmpHeader->PixelWidth) + (Width)];          
        break;

        default:                    
          //
          // Absolute mode.
          //
	  for (Index = 0; Index < SecondByte; Index++, Width++, Blt++) {
            EachValue = *RLE8Image;
            RLE8Image++;          
            Blt->Red    = BmpColorMap[EachValue].Red;
            Blt->Green  = BmpColorMap[EachValue].Green;
            Blt->Blue   = BmpColorMap[EachValue].Blue; 	                
	  }
          //
          // align on word boundary
          //
          if (Index % 2 == 1)
	    RLE8Image++;
        break;
        }
      }
    }
    Height++;
  }
  return EFI_SUCCESS;
} 

/**
 Convert a *.BMP graphics image to a GOP/UGA blt buffer. If a NULL Blt buffer
 is passed in a GopBlt buffer will be allocated by this routine. If a GopBlt
 buffer is passed in it will be used if it is big enough.

 @param [in]   This
 @param [in]   ImageData
 @param [in]   ImageDataSize
 @param [out]  DecodedData
 @param [out]  DecodedDataSize
 @param [out]  Height
 @param [out]  Width

 @retval EFI_SUCCESS            GopBlt and GopBltSize are returned.
 @retval EFI_UNSUPPORTED        BmpImage is not a valid *.BMP image
 @retval EFI_BUFFER_TOO_SMALL   The passed in GopBlt buffer is not big enough.
                                GopBltSize will contain the required size.
 @retval EFI_OUT_OF_RESOURCES   No enough buffer to allocate

**/
EFI_STATUS
EFIAPI
BmpDecoderDecodeImage (
  IN     EFI_BMP_DECODER_PROTOCOL     *This,
  IN     UINT8                         *ImageData,
  IN     UINTN                         ImageDataSize,
     OUT UINT8                         **DecodedData,
     OUT UINTN                         *DecodedDataSize,
     OUT UINTN                         *Height,
     OUT UINTN                         *Width
  )
{
  UINT8                         *Image;
  UINT8                         *ImageHeader;
  BMP_IMAGE_HEADER              *BmpHeader;
  BMP_COLOR_MAP                 *BmpColorMap;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *Blt;
  UINT64                        BltBufferSize;
  UINTN                         Index;
  UINTN                         HeightIndex;
  UINTN                         WidthIndex;
  UINTN                         ImageIndex;
  BOOLEAN                       IsAllocated;

  BmpHeader = (BMP_IMAGE_HEADER *) ImageData;
  if (BmpHeader->CharB != 'B' || BmpHeader->CharM != 'M') {
    return EFI_UNSUPPORTED;
  }

  if (   BmpHeader->CompressionType != BMP_UNCOMPRESSION_TYPE 
      && BmpHeader->CompressionType != BMP_RLE8BIT_TYPE
      && BmpHeader->CompressionType != BMP_RLE4BIT_TYPE ) {
    return EFI_UNSUPPORTED;
  }

  //
  // Calculate Color Map offset in the image.
  //
  Image       = ImageData;
  BmpColorMap = (BMP_COLOR_MAP *) (Image + sizeof (BMP_IMAGE_HEADER));

  //
  // Calculate graphics image data address in the image
  //
  Image         = ((UINT8 *) ImageData) + BmpHeader->ImageOffset;
  ImageHeader   = Image;

  BltBufferSize = BmpHeader->PixelWidth * BmpHeader->PixelHeight * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL);
  if (BltBufferSize >= SIZE_4G) {
     //
     // The buffer size extends the limitation
     //
     return EFI_UNSUPPORTED;
  }

  IsAllocated   = FALSE;
  if (*DecodedData == NULL) {
    *DecodedDataSize = (UINTN) BltBufferSize;
    *DecodedData     = AllocatePool (*DecodedDataSize);
    IsAllocated = TRUE;
    if (*DecodedData == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
  } else {
    if (*DecodedDataSize < (UINTN) BltBufferSize) {
      *DecodedDataSize = (UINTN) BltBufferSize;

      return EFI_BUFFER_TOO_SMALL;
    }
  }

  *Width   = BmpHeader->PixelWidth;
  *Height  = BmpHeader->PixelHeight;
  //
  // Convert image from BMP to Blt buffer format
  //
  BltBuffer = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) *DecodedData;
  //
  // RLE8 decode.
  //
  if (BmpHeader->CompressionType == BMP_RLE8BIT_TYPE) {
    gBS->SetMem (BltBuffer, (UINTN)BltBufferSize, 0x00);
    RLE8ToBlt (BltBuffer, Image, BmpColorMap, BmpHeader);
    return EFI_SUCCESS;
  }
  //
  // RLE4 decode.
  //
  if(BmpHeader->CompressionType == BMP_RLE4BIT_TYPE) {
    gBS->SetMem (BltBuffer, (UINTN)BltBufferSize, 0x00);
    RLE4ToBlt (BltBuffer, Image, BmpColorMap, BmpHeader);
    return EFI_SUCCESS;    
  }
  //
  // BMP decode.
  //
  for (HeightIndex = 0; HeightIndex < BmpHeader->PixelHeight; HeightIndex++) {
    Blt = &BltBuffer[(BmpHeader->PixelHeight - HeightIndex - 1) * BmpHeader->PixelWidth];
    for (WidthIndex = 0; WidthIndex < BmpHeader->PixelWidth; WidthIndex++, Image++, Blt++) {
      switch (BmpHeader->BitPerPixel) {

      case 1:
        //
        // Convert 1bit BMP to 24-bit color
        //
        for (Index = 0; Index < 8 && WidthIndex < BmpHeader->PixelWidth; Index++) {
          Blt->Red    = BmpColorMap[((*Image) >> (7 - Index)) & 0x1].Red;
          Blt->Green  = BmpColorMap[((*Image) >> (7 - Index)) & 0x1].Green;
          Blt->Blue   = BmpColorMap[((*Image) >> (7 - Index)) & 0x1].Blue;
          Blt++;
          WidthIndex++;
        }

        Blt --;
        WidthIndex --;
        break;

      case 4:
        //
        // Convert BMP Palette to 24-bit color
        //
        Index       = (*Image) >> 4;
        Blt->Red    = BmpColorMap[Index].Red;
        Blt->Green  = BmpColorMap[Index].Green;
        Blt->Blue   = BmpColorMap[Index].Blue;
        if (WidthIndex < (BmpHeader->PixelWidth - 1)) {
          Blt++;
          WidthIndex++;
          Index       = (*Image) & 0x0f;
          Blt->Red    = BmpColorMap[Index].Red;
          Blt->Green  = BmpColorMap[Index].Green;
          Blt->Blue   = BmpColorMap[Index].Blue;
        }
        break;

      case 8:
        //
        // Convert BMP Palette to 24-bit color
        //
        Blt->Red    = BmpColorMap[*Image].Red;
        Blt->Green  = BmpColorMap[*Image].Green;
        Blt->Blue   = BmpColorMap[*Image].Blue;
        break;

      case 24:
        Blt->Blue   = *Image++;
        Blt->Green  = *Image++;
        Blt->Red    = *Image;
        break;

      case 32:
        Blt->Blue   = *Image++;
        Blt->Green  = *Image++;
        Blt->Red    = *Image++;
        break;

      default:
        if (IsAllocated) {
          gBS->FreePool (*DecodedData);
          *DecodedData = NULL;
        }
        return EFI_UNSUPPORTED;
        break;
      };

    }

    ImageIndex = (UINTN) (Image - ImageHeader);
    if ((ImageIndex % 4) != 0) {
      //
      // Bmp Image starts each row on a 32-bit boundary!
      //
      Image = Image + (4 - (ImageIndex % 4));
    }
  }

  return EFI_SUCCESS;
}

/**
 Install Driver to produce PCX Decoder protocol.

 @param [in]   ImageHandle
 @param [in]   SystemTable

 @retval EFI_SUCCESS            PCX Decoder protocol installed
 @return Other        No protocol installed, unload driver.

**/
EFI_STATUS
EFIAPI
BmpDecoderInstall (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS              Status;

  Status = gBS->InstallProtocolInterface (
                  &mPrivateData.Handle,
                  &gEfiBmpDecoderProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mPrivateData.BmpDecoder
                  );
  return Status;
}
