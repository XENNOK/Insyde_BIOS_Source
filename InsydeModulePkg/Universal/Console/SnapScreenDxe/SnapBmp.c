/** @file
  Serial driver for standard UARTS on an ISA bus.

Copyright (c) 2006 - 2010, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

//#include "Efi.h"
//#include "EfiDriverLib.h"
#include <Uefi.h>

#include "SnapBmp.h"
#include "SnapLib.h"
#include <Guid/FileInfo.h>
#include <Protocol/SimpleFileSystem.h>

//#include EFI_PROTOCOL_DEFINITION (FileInfo)
//#include EFI_PROTOCOL_DEFINITION (SimpleFileSystem)

BMP_IMAGE_HEADER  mBmpImageHeaderTemplate = {
  'B',    // CharB
  'M',    // CharM
  0,      // Size will be updated at runtime
  {0, 0}, // Reserved
  sizeof (BMP_IMAGE_HEADER), // ImageOffset
  sizeof (BMP_IMAGE_HEADER) - EFI_FIELD_OFFSET (BMP_IMAGE_HEADER, HeaderSize), // HeaderSize
  0,      // PixelWidth will be updated at runtime
  0,      // PixelHeight will be updated at runtime
  1,      // Planes
  24,     // BitPerPixel
  0,      // CompressionType
  0,      // ImageSize will be updated at runtime
  0,      // XPixelsPerMeter
  0,      // YPixelsPerMeter
  0,      // NumberOfColors
  0       // ImportantColors
};

EFI_STATUS
ObtainBmpFileNameByTime (
  CHAR16    *FileName,
  UINTN     Size
  )
{
  EFI_STATUS    Status;
  EFI_TIME      EfiTime;
  CHAR16        Buffer[5];

  if (FileName == NULL)
    return EFI_INVALID_PARAMETER;

  if (Size < BMP_TIME_FILE_NAME_SIZE)
    return EFI_BUFFER_TOO_SMALL;
    
  Status = gRT->GetTime (&EfiTime, NULL);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  
  ValueToString (EfiTime.Year, 4, Buffer, VTS_LEAD_0);
  StrCpy (FileName, Buffer);
  
  ValueToString (EfiTime.Month, 2, Buffer, VTS_LEAD_0);
  StrCat (FileName, Buffer);

  ValueToString (EfiTime.Day, 2, Buffer, VTS_LEAD_0);
  StrCat (FileName, Buffer);

  ValueToString (EfiTime.Hour, 2, Buffer, VTS_LEAD_0);
  StrCat (FileName, Buffer);

  ValueToString (EfiTime.Minute, 2, Buffer, VTS_LEAD_0);
  StrCat (FileName, Buffer);

  ValueToString (EfiTime.Second, 2, Buffer, VTS_LEAD_0);
  StrCat (FileName, Buffer);

  StrCat (FileName, L".BMP");

  return EFI_SUCCESS;
}

EFI_STATUS
BmpCreate (
  EFI_HANDLE    FsHandle,
  CHAR16        *FileName,
  BMP_FILE      **ABmpFile
  )
{
  EFI_STATUS           Status;
  BMP_FILE             *BmpFile;

  // Test SimpleFileSystem Handle
  Status = gBS->OpenProtocol (
                              FsHandle, 
                              &gEfiSimpleFileSystemProtocolGuid, 
                              NULL, 
                              NULL,
                              NULL,
                              EFI_OPEN_PROTOCOL_TEST_PROTOCOL
                             );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  // allocate memory for BMP_FILE intance
  BmpFile  = AllocateZeroPool (sizeof(BMP_FILE) + StrSize(FileName));
  if (BmpFile == NULL)
    return EFI_OUT_OF_RESOURCES;

  // initialize object members
  BmpFile->FsHandle = FsHandle;
  BmpFile->FileName = (CHAR16 *)((UINT8 *)BmpFile + sizeof (BMP_FILE));
  StrCpy(BmpFile->FileName, FileName);

  // return BMP_FILE object 
  *ABmpFile = BmpFile;
  
  return EFI_SUCCESS;
}

EFI_STATUS
BmpBltBufferToImage (
  BMP_FILE                        *Bmp,
  UINTN                           Width,
  UINTN                           Height,
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *BltBuffer,
  UINTN                           BufferWidth
  )
{
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *BltPixel;
  UINTN                           PaddingSize;
  UINTN                           BmpSize;
  UINT8                           *BmpBuffer;
  UINT8                           *Image;
  UINTN                           Col;
  UINTN                           Row;

  if (Bmp->BmpHeader != NULL)
    gBS->FreePool (Bmp->BmpHeader);
    
  //
  // Allocate memory for BMP file.
  //
  PaddingSize = Width & 0x3;
  BmpSize = (Width * 3 + PaddingSize) * Height + sizeof (BMP_IMAGE_HEADER);
  BmpBuffer = AllocateZeroPool (BmpSize);
  if (BmpBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  mBmpImageHeaderTemplate.Size = (UINT32) BmpSize;
  mBmpImageHeaderTemplate.ImageSize = (UINT32) BmpSize - sizeof (BMP_IMAGE_HEADER);
  mBmpImageHeaderTemplate.PixelWidth = (UINT32) Width;
  mBmpImageHeaderTemplate.PixelHeight = (UINT32) Height;
  CopyMem (BmpBuffer, &mBmpImageHeaderTemplate, sizeof (BMP_IMAGE_HEADER));

  //
  // Convert BLT buffer to BMP file.
  //
  Image = BmpBuffer + sizeof (BMP_IMAGE_HEADER);
  for (Row = 0; Row < Height; Row++) {
    BltPixel = &BltBuffer[(Height - Row - 1) * BufferWidth];

    for (Col = 0; Col < Width; Col++) {
      *Image++ = BltPixel->Blue;
      *Image++ = BltPixel->Green;
      *Image++ = BltPixel->Red;
      BltPixel++;
    }

    //
    // Padding for 4 byte alignment.
    //
    Image += PaddingSize;
  }

  // set Bmp Object member
  Bmp->BmpHeader = (BMP_IMAGE_HEADER *) BmpBuffer;
  Bmp->BmpImage = BmpBuffer + sizeof (BMP_IMAGE_HEADER);
  
  return EFI_SUCCESS;
}

EFI_STATUS
BmpScreenToImage (
  BMP_FILE      *Bmp,
  UINTN         X,
  UINTN         Y,
  UINTN         Width,
  UINTN         Height
  )
{
  EFI_STATUS                      Status;
  EFI_HANDLE                      VgaHandle;
  UINTN                           BltSize;
  EFI_GRAPHICS_OUTPUT_PROTOCOL    *Gop = NULL;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *BltBuffer = NULL;
  EFI_TPL                         OriginalTPL;
  UINTN                           BufferWidth;

  //
  // get the VGA GOP protocol
  //
  Status = GetActiveVgaHandle (&VgaHandle);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  
  Status = gBS->HandleProtocol (VgaHandle, &gEfiGraphicsOutputProtocolGuid, (VOID **)&Gop);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  if (Width == MAX_VGA_MODE_WIDTH)
    Width = Gop->Mode->Info->HorizontalResolution;

  if (Height == MAX_VGA_MODE_HEIGHT)
    Height = Gop->Mode->Info->VerticalResolution;


  if (Gop->Mode->Info->PixelFormat == PixelBlueGreenRedReserved8BitPerColor
      && Gop->Mode->FrameBufferBase != 0
      && Gop->Mode->FrameBufferSize >= (Gop->Mode->Info->PixelsPerScanLine * Height * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL))) {
    BufferWidth = Gop->Mode->Info->PixelsPerScanLine;
  } else {
    BufferWidth = Width;
  }

  //
  //  Allocate memory for Blt Buffer and Get Video Image
  //
  BltSize = BufferWidth * Height * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL);
  BltBuffer = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)AllocatePool (BltSize);
  if (BltBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Capture full screen image
  //

  // RaiseTPL is needed, Mouse cursor use timer event for refresh cursor. 
  OriginalTPL = gBS->RaiseTPL (TPL_NOTIFY);

  if (Gop->Mode->Info->PixelFormat == PixelBlueGreenRedReserved8BitPerColor
      && Gop->Mode->FrameBufferBase != 0
      && Gop->Mode->FrameBufferSize >= BltSize) {
    CopyMem (BltBuffer, (UINT8 *)(UINTN)Gop->Mode->FrameBufferBase, BltSize);
  } 
  
  gBS->RestoreTPL (OriginalTPL);
  Status = BmpBltBufferToImage (Bmp, Width, Height, BltBuffer, BufferWidth);

  gBS->FreePool (BltBuffer);

    
  return Status;
}

EFI_STATUS
BmpSaveToFile (
  BMP_FILE      *Bmp
  )
{
  EFI_STATUS                        Status;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL   *Sfs;
  EFI_FILE                          *RootFs = NULL;
  EFI_FILE                          *FileHandle = NULL;
  UINTN                             BmpSize;
  UINT8                             Buffer[MAX_FILE_INFO_SIZE];
  UINTN                             BufferSize = MAX_FILE_INFO_SIZE;
  EFI_FILE_INFO                     *FileInfo = (EFI_FILE_INFO *) Buffer;

  // Check the BMP image is already
  if (Bmp->BmpHeader == NULL) {
    return EFI_UNSUPPORTED;
  }
  
  //
  // Locate the SimpleFileSystem protocol for save image to file
  //
  Status = gBS->HandleProtocol (Bmp->FsHandle, &gEfiSimpleFileSystemProtocolGuid, (VOID **)&Sfs);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  // Caculate BMP file size
  BmpSize = Bmp->BmpHeader->ImageSize + sizeof(BMP_IMAGE_HEADER);
  
  //
  // Save image to .BMP file
  //
  Status = Sfs->OpenVolume (Sfs, &RootFs);
  if (EFI_ERROR(Status)) {
    goto EXIT;
  }
  
  Status = RootFs->Open (RootFs, &FileHandle, Bmp->FileName, EFI_FILE_MODE_CREATE | EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE, 0);
  if (EFI_ERROR(Status)) {
    Status = EFI_NO_MEDIA;
    goto EXIT;
  }
  
  Status = FileHandle->Write (FileHandle, &BmpSize, Bmp->BmpHeader);
  if (EFI_ERROR(Status)) {
    Status = EFI_NO_MEDIA;
    goto EXIT;
  }
  
  // When open a exist file the file size may be large than BMP file, So make sure file size is BMP file size. 
  FileHandle->GetInfo (FileHandle, &gEfiFileInfoGuid,  &BufferSize, FileInfo);
  BufferSize = FileInfo->Size;
  FileInfo->PhysicalSize = BmpSize;
  FileHandle->SetInfo (FileHandle, &gEfiFileInfoGuid, BufferSize, FileInfo);

EXIT:

  // Close file
  if (FileHandle != NULL)
    FileHandle->Close (FileHandle);

  if (RootFs != NULL)
    RootFs->Close (RootFs);
  
  return Status;
}

EFI_STATUS
BmpDestroy (
  BMP_FILE      *Bmp
  )
{
  if (Bmp == NULL)
    return EFI_SUCCESS;
    
  if (Bmp->BmpHeader != NULL)
    gBS->FreePool (Bmp->BmpHeader);

  gBS->FreePool (Bmp);
  
  return EFI_SUCCESS;
}
