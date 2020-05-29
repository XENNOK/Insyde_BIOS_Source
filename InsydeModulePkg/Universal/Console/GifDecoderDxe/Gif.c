/** @file
  Gif Converter application to covert Gif file to HII animation binary

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

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include "Gif.h"

STATIC
EFI_STATUS
RetrieveCompressedData (
  IN     UINT8  **GifFileImage, 
  IN     UINTN  *GifFileSize, 
  IN     UINT8  *CompressedData,
  IN OUT UINTN  *CompressedDataSize
  );

STATIC
UINT16
GetCode (
  IN  UINT8  *BufIn,
  IN  UINT8  CodeSize,
  OUT UINT32 *WhichBit
  );

STATIC
UINT32 
LZWDecoder (
  IN     UINT8                         *BufIn, 
  IN     UINTN                         BufInSize, 
  IN     BOOLEAN                       Interlaced,
  IN     EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ColorMap, 
  IN     UINT16                        Width, 
  IN     UINT16                        Height, 
  IN OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImageData
  );


/**
 Read data safely

 @param [in, out] FileImage     Pointer to file image buffer
 @param [in, out] FileSize      Pointer to file image size
 @param [out]  Buffer           Pointer to read buffer
 @param [in]   ReadSize         Size of read data

 @retval TRUE                   Read data success
 @retval FALSE                  No data to read

**/
STATIC 
BOOLEAN
ReadOk (
  IN OUT UINT8   **FileImage, 
  IN OUT UINTN  *FileSize,
  OUT    VOID    *Buffer, 
  IN     UINT32  ReadSize
  )
{
  if (*FileSize < ReadSize) {
    return FALSE; 
  }

  CopyMem (Buffer, *FileImage, ReadSize);

  *FileImage += ReadSize; 
  *FileSize  -= ReadSize; 
  return TRUE; 
}

/**
 Get data block, 1 byte block size and n byte data

 @param [in, out] GifFileImage  Pointer to file image
 @param [in, out] GifFileSize   Pointer to file image size
 @param [out]  Buffer           Pointer to read data

 @retval 0                      No data to read
 @return others  Read data size

**/
STATIC 
UINT32
GetDataBlock (
  IN OUT UINT8   **GifFileImage, 
  IN OUT UINTN   *GifFileSize, 
  OUT    UINT8   *Buffer
  )
{
  UINT8 BlockSize; 
  
  if (!ReadOk (GifFileImage, GifFileSize, &BlockSize, 1)) {
    return 0; 
  }

  if ((BlockSize != 0) && (!ReadOk(GifFileImage, GifFileSize, Buffer, BlockSize))) {
    return 0;    
  }

  return BlockSize; 
}

/**
 Get gif logical screen, include gif image information and color map

 @param [in, out] FileImage     Pointer to file image
 @param [in, out] FileSize      Pointer to file image size
 @param [out]  GifScreen        Gif image information
 @param [out]  GlobalColorMap   Global color map for gif imgae

 @retval EFI_SUCCESS            read success
 @retval EFI_UNSUPPORTED        format error

**/
EFI_STATUS 
GifDecoderGetLogicalScreen (
  IN OUT UINT8                         **FileImage, 
  IN OUT UINTN                         *FileSize,
  OUT    GIF_LOGICAL_SCREEN_DESCRIPTOR *GifScreen,
  OUT    EFI_GRAPHICS_OUTPUT_BLT_PIXEL *GlobalColorMap
  )
{
  UINT8      *ColorPtr; 
  GIF_HEADER GifHeader; 
  UINT32     MapSize; 
  UINT16     Index;


  ASSERT (FileImage != NULL && *FileImage != NULL && FileSize != NULL);
  ASSERT (GifScreen != NULL && GlobalColorMap != NULL);

  //
  // Gif Header
  // 
  if ((!ReadOk (FileImage, FileSize, &GifHeader, sizeof(GIF_HEADER))) || 
      (CompareMem (&GifHeader, "GIF87a", sizeof (GIF_HEADER)) != 0 &&
       CompareMem (&GifHeader, "GIF89a", sizeof (GIF_HEADER)) != 0)) {
    return EFI_UNSUPPORTED;
  }

  //
  // Gif Logical Screen Descriptor 
  // 
  if (!ReadOk (FileImage, FileSize, GifScreen, sizeof (GIF_LOGICAL_SCREEN_DESCRIPTOR))) {
    return EFI_UNSUPPORTED;
  }

  //
  // Gif Global Color Map 
  // 
  MapSize = 1 << (GifScreen->PackedFields.SizeOfGlobalColorTable + 1);
  
  if (GifScreen->PackedFields.GlobalColorTableFlag) {
    if (*FileSize < (UINT32)(MapSize * 3)) {
      return EFI_UNSUPPORTED;
    }
    ColorPtr = *FileImage; 
    
    for (Index = 0; Index < MapSize; ++Index) {      
      GlobalColorMap[Index].Red   = *ColorPtr; 
      GlobalColorMap[Index].Green = *(ColorPtr + 1); 
      GlobalColorMap[Index].Blue  = *(ColorPtr + 2); 
      GlobalColorMap[Index].Reserved = 0; 
      
      ColorPtr += 3; 
    }
    *FileImage += MapSize * 3;
    *FileSize  -= MapSize * 3;     
  } else {
    for (Index = 0; Index < MapSize; ++Index) {
      GlobalColorMap[Index].Red   = (UINT8)Index;
      GlobalColorMap[Index].Green = (UINT8)Index;
      GlobalColorMap[Index].Blue  = (UINT8)Index;
      GlobalColorMap[Index].Reserved = 0; 
    }      
  }  
 
  return EFI_SUCCESS; 
}

/**
 Get single image description (offset x, y), local color map,
 and graphic control (delay time, and transparent)


 @retval EFI_SUCCESS            read success, has one signle image
 @retval EFI_NOT_FOUND          no image to read
 @retval EFI_UNSUPPORTED        format error

**/
EFI_STATUS 
GifDecoderGetImageDesc (
  IN      UINT8                           **GifFileImage, 
  IN      UINTN                           *GifFileSize, 
  OUT     GIF_IMAGE_DESCRIPTOR            *ImageDesc, 
  OUT     EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *LocalColorMap,
  OUT     GIF_GRAPHIC_CONTROL_EXTENSION   *GraphicControl
  )
  
{
  UINT8      Ch; 
  UINT8      Label;
  UINT8      Buf[256]; 
  UINT32     Count; 
  UINT8      *ColorPtr; 
  UINT8      *ImagePtr; 
  UINTN      FileSize; 
  UINT32     MapSize; 
  UINT32     Index; 

  ASSERT (GifFileImage != NULL);

  ImagePtr = *GifFileImage; 
  FileSize = *GifFileSize; 

  if (FileSize == 0) {
    return EFI_NOT_FOUND; 
  }

  Ch = *ImagePtr;   
  while (Ch != GIF_TRAILER) {

    if (Ch == IMAGE_SEPARATOR) {
      if (!ReadOk (&ImagePtr, &FileSize, ImageDesc, sizeof (GIF_IMAGE_DESCRIPTOR))) {
        return EFI_UNSUPPORTED;
      }
      if (ImageDesc->PackedFields.LocalColorTableFlag) {
        MapSize = 1 << (ImageDesc->PackedFields.SizeOfLocalColorTable + 1);
        if (FileSize < (UINT32)(MapSize * 3)) {
          return EFI_UNSUPPORTED;
        }
        ColorPtr = ImagePtr;         
        for (Index = 0; Index < MapSize; ++Index) {      
          LocalColorMap[Index].Red   = *ColorPtr; 
          LocalColorMap[Index].Green = *(ColorPtr + 1); 
          LocalColorMap[Index].Blue  = *(ColorPtr + 2); 
          LocalColorMap[Index].Reserved = 0;          
          ColorPtr += 3; 
        }
        ImagePtr += MapSize * 3;
        FileSize -= MapSize * 3;     
      } 
      *GifFileImage = ImagePtr; 
      *GifFileSize  = FileSize; 
      return EFI_SUCCESS; 
    } else if (Ch == EXTENSION_INTRODUCER) {
      if (FileSize < 2) {
        return EFI_UNSUPPORTED;
      }

      Label = *(ImagePtr + 1);
      ImagePtr += 2;
      FileSize -= 2;       
      if (Label == GRAPHIC_CONTROL_LABEL) {
        Count = GetDataBlock (&ImagePtr, &FileSize, Buf);        
        if (Count != 4) {
          return EFI_UNSUPPORTED;
        } else {
          CopyMem (&GraphicControl->PackedFields, Buf, 4);
          while (GetDataBlock (&ImagePtr, &FileSize, Buf) != 0); 
        }
      } else {
        while (GetDataBlock (&ImagePtr, &FileSize, Buf) != 0); 
      }
    } else {
      ImagePtr++; 
      FileSize--;
    }
    
    if (FileSize == 0) {
      return EFI_UNSUPPORTED;    
    } else {
      Ch = *ImagePtr;
    } 
  }  

  *GifFileImage = ImagePtr; 
  *GifFileSize  = FileSize; 
  return EFI_NOT_FOUND; 
}

/**
 Decompress LZW data to graphics output pixel format image data

 @param [in]   GifFileImage     Pointer to file imag
 @param [in]   GifFileSize      Pointer to file image size
 @param [in]   ImageDesc        Gif image description
 @param [in]   GlobalColorMap   Global color map
 @param [in]   LocalColorMap    Local color map
 @param [in]   GraphicControl
 @param [out]  ImageData        Graphics Output Pixel format image data
 @param [in]   ImageSize
 @param [in]   CompressedData   Temporary buffer for fragmentary image data block
 @param [in, out] CompressedDataSize  Temporary buffer size

 @retval EFI_SUCCESS            read success, has one signle image
 @retval EFI_BUFFER_TOO_SMALL   buffer too small
 @retval EFI_UNSUPPORTED        format error

**/
EFI_STATUS
GifDecoderGetImageData (
  IN     UINT8                           **GifFileImage, 
  IN     UINTN                           *GifFileSize, 
  IN     GIF_IMAGE_DESCRIPTOR            *ImageDesc, 
  IN     EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *GlobalColorMap, 
  IN     EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *LocalColorMap,
  IN     GIF_GRAPHIC_CONTROL_EXTENSION   *GraphicControl, 
  OUT    EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *ImageData, 
  IN     UINT32                          ImageSize,
  IN     UINT8                           *CompressedData,
  IN OUT UINTN                           *CompressedDataSize
  )
{
  EFI_STATUS                    Status; 
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ColorMap; 
  UINT8                         *ImagePtr;
  UINTN                         FileSize; 
  
  //
  // allocate compressed data 
  //
  ImagePtr = *GifFileImage; 
  FileSize = *GifFileSize; 
  
  Status = RetrieveCompressedData (
             &ImagePtr, 
             &FileSize,
             CompressedData, 
             CompressedDataSize
             ); 
  
  if (EFI_ERROR (Status)) {
    return Status; 
  }

  *GifFileImage = ImagePtr; 
  *GifFileSize  = FileSize; 

  if (ImageDesc->PackedFields.LocalColorTableFlag) {
    ColorMap = LocalColorMap; 
  } else {
    ColorMap = GlobalColorMap; 
  }
  
  LZWDecoder (
    CompressedData,
    *CompressedDataSize, 
    ImageDesc->PackedFields.InterlaceFlag, 
    ColorMap, 
    ImageDesc->ImageWidth,
    ImageDesc->ImageHeight, 
    ImageData
    );

  return EFI_SUCCESS; 
}


/**
 Convert fragmentary image data block to a continuous block

 @param [in]   GifFileImage     Pointer to file imag
 @param [in]   GifFileSize      Pointer to file image size
 @param [in]   CompressedData   Temporary buffer for fragmentary image data block
 @param [in, out] CompressedDataSize  Temporary buffer size

 @retval EFI_SUCCESS            read success, has one signle image
 @retval EFI_BUFFER_TOO_SMALL   buffer too small
 @retval EFI_UNSUPPORTED        format error

**/
EFI_STATUS
RetrieveCompressedData (
  IN     UINT8  **GifFileImage, 
  IN     UINTN  *GifFileSize, 
  IN     UINT8  *CompressedData,
  IN OUT UINTN  *CompressedDataSize
  )
  
{
  UINT8  LzwCodeSize;
  UINT8  BlockLength;
  UINTN  TotalSize; 
  UINTN  AvailableSize; 
  
  UINT8  Buf[256]; 
  UINT8  *ImagePtr; 
  UINTN  FileSize; 

  ImagePtr = *GifFileImage; 
  FileSize = *GifFileSize; 
  AvailableSize = *CompressedDataSize;
    
  //
  // calculate lzwcodesize compressed data size, then copy data 
  //
  if (!ReadOk (&ImagePtr, &FileSize, &LzwCodeSize, 1)) {
    return EFI_UNSUPPORTED;
  }
  TotalSize = 1;  

  if (AvailableSize >= 1) {
    *CompressedData = LzwCodeSize; 
    AvailableSize -= 1;
  }  

  for (;;) { 
    if (!ReadOk (&ImagePtr, &FileSize, &BlockLength, 1)) {
      return EFI_UNSUPPORTED; 
    }
    
    if ((BlockLength == 0) || (AvailableSize < BlockLength)) {
      break; 
    }     

    if (!ReadOk (&ImagePtr, &FileSize, CompressedData + TotalSize, BlockLength)) {
      return EFI_UNSUPPORTED;
    }
    AvailableSize -= BlockLength; 
    TotalSize     += BlockLength; 
  }


  if (BlockLength != 0) {
    do {
      if (!ReadOk (&ImagePtr, &FileSize, Buf, BlockLength)) {
        return EFI_UNSUPPORTED;
      }
      TotalSize += BlockLength; 
      
      if (!ReadOk (&ImagePtr, &FileSize, &BlockLength, 1)) {
        return EFI_UNSUPPORTED;
      }
    } while (BlockLength != 0); 
  } 

  if (*CompressedDataSize < TotalSize) {
    *CompressedDataSize = TotalSize; 
    return EFI_BUFFER_TOO_SMALL; 
  }

  *CompressedDataSize = TotalSize; 
  *GifFileImage = ImagePtr; 
  *GifFileSize  = FileSize;     
  return EFI_SUCCESS; 
}



/**
 Get code data by code size

 @param [in]   BufIn            Buffer to read
 @param [in]   CodeSize         Size (bits) of Read
 @param [out]  WhichBit         Record already read bits

 @retval Data                   read data

**/
STATIC
UINT16
GetCode (
  IN  UINT8  *BufIn,
  IN  UINT8  CodeSize,
  OUT UINT32 *WhichBit
  )
{
  UINT32 Bit;
  UINT32 Data;

  Bit = *WhichBit;

  Data = *(UINT32 *) (BufIn + Bit / 8);
  Data >>= (Bit & 7);
  Data = Data & ((1 << CodeSize) - 1);

  *WhichBit += CodeSize;

  return (UINT16)Data;
}

/**
 LZW decoder

 @param [in]   BufIn            Pointer to compressed data
 @param [in]   BufInSize        Pointer to compressed data size
 @param [in]   Interlaced       Data store is interlaced
 @param [in]   ColorMap         Image color map
 @param [in]   Width            Image width
 @param [in]   Height           Image Height
 @param [in, out] Blt           Output image data

 @return ReadBits    Read image bits

**/
STATIC
UINT32 
LZWDecoder (
  IN     UINT8                         *BufIn, 
  IN     UINTN                         BufInSize, 
  IN     BOOLEAN                       Interlaced,
  IN     EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ColorMap, 
  IN     UINT16                        Width, 
  IN     UINT16                        Height, 
  IN OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL *Blt
  )
{
  UINT16 PrevCode; 
  UINT16 Code; 
  UINT16 ClearCode; 
  UINT16 EndCode; 
  UINT16 TmpCode; 
  
  UINT8  InitCodeSize; 
  UINT8  CodeSize; 

  UINT16 FirstIndex; 
  UINT16 EntryIndex; 

  UINT32 ReadBits; 
  INTN   OutIndex; 
  UINT16 Index; 

  INT16  StackIndex; 

  UINT16 X;
  UINT16 Y; 
  
  

  InitCodeSize = *BufIn; 
  BufIn++;   
  CodeSize   = InitCodeSize + 1; 
  ClearCode  = (1 << InitCodeSize);
  EndCode    = ClearCode + 1; 
  FirstIndex = EndCode   + 1; 
  EntryIndex = FirstIndex; 

  ReadBits = 0; 
  OutIndex = 0; 
  PrevCode = NONCODE; 

  X = 0;
  Y = 0; 
  
  //
  // init lzw table 
  //
  for (Index = 0; Index < ClearCode; ++Index) {

    mLzwTable[Index].Prefix = NONCODE; 
    mLzwTable[Index].Suffix = (UINT8)Index;     
  }

  for (;;) {
    
    if (((ReadBits + CodeSize) / 8) > BufInSize) {

      //
      // no data
      //
      //Print (L"nodata");
      break; 
    } else {
      Code = GetCode (BufIn, CodeSize, &ReadBits);     
    }
        
    if (Code == EndCode) {

      break;         
    } 

    if (Code == ClearCode) {

      CodeSize   = InitCodeSize + 1; 
      EntryIndex = FirstIndex;
      PrevCode   = ClearCode;  
    } else {    
    
      if (Y == Height) {
        //Print (L"bad code");
        break;    
      }      
    
      StackIndex = -1;       
      if (Code < EntryIndex) {


        //
        // code in table 
        // convert [code] to string, and output [code]
        // add new entry [prevcode][first char of code] 
        // 
        TmpCode = Code;        
        while (TmpCode != NONCODE) {
          mOutStack[++StackIndex] = mLzwTable[TmpCode].Suffix;
          TmpCode = mLzwTable[TmpCode].Prefix;     
        } 
      } else {
      
        if (PrevCode == ClearCode) {

          //Print (L"bad code"); 
          Code = ClearCode; 
          continue; 
        }
        //
        // code isn't in table          
        // convert [prevcode] to string, and output [prevcode][first char of prevcode]
        // add new entry [prevcode][first char of prevcode] 
        //            
        StackIndex++; 
        TmpCode = PrevCode; 
        while (TmpCode != NONCODE) {
          mOutStack[++StackIndex] = mLzwTable[TmpCode].Suffix;
          TmpCode = mLzwTable[TmpCode].Prefix;       
        }    
        mOutStack[0] = mOutStack[StackIndex];        
      }      
      
      if (PrevCode != ClearCode) {
        mLzwTable[EntryIndex].Prefix = PrevCode; 
        mLzwTable[EntryIndex].Suffix = mOutStack[StackIndex]; 
        EntryIndex++; 
      }
      //
      // output outstack data 
      //     
      while (StackIndex >= 0) {
        Blt[X + Y * Width] = ColorMap[mOutStack[StackIndex--]];
        X++; 
        if (X == Width) {
          
          X = 0; 
          if (Interlaced) {
            if ((Y & 7) == 0) { Y += 8; if (Y >= Height) Y = 4; } 
            else if ((Y & 3) == 0) { Y += 8; if (Y >= Height) Y = 2; } 
            else if ((Y & 1) == 0) { Y += 4; if (Y >= Height) Y = 1; } 
            else Y += 2;              
          } else {
            Y++; 
          }
        }        
        if (Y == Height) {
          break;         
        }       
      }       
    }

    if (EntryIndex >= (1 << CodeSize)) {

      if (CodeSize < 12) {

        CodeSize++; 
      } else {
        //Print (L"bad code"); 
        Code = ClearCode; 
        continue; 
      }      
    }        
    PrevCode = Code;        
  }    
  return ReadBits;
} 

