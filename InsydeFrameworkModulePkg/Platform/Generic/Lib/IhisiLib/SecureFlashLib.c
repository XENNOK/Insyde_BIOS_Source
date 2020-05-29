//;******************************************************************************
//;* Copyright (c) 1983-2014, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#include "IhisiLib.h"
#include "Efi.h"
#include "EfiImage.h"

#include EFI_GUID_DEFINITION (SecureFlashInfo)
#include EFI_PROTOCOL_DEFINITION (SmmVariable)
#include EFI_PROTOCOL_DEFINITION (TianoCompressProtocol)

#define IMAGE_BLOCK_BUFFERSIZE_FOR_COMPRESS      0x10000     // 64K

#define WINFLASH_DO_S3          0
#define WINFLASH_DO_REBOOT      1
#define WINFLASH_DO_SHUT_DOWN   2
#define WINFLASH_DO_NOTHING     3

BOOLEAN  mInPOST = TRUE;

typedef struct {
  UINT64                        ImageBlockSize;
  UINT64                        ImageBlockAddress;
} IMAGE_BLOCK;

EFI_STATUS
VerifySignedImage (
  IN UINT8                      *ImageBase
  )
/*++

Routine Description:
  Verify if the image is signed.

Arguments:
  ImageBase               The address of the image.

Returns:
  EFI_ACCESS_DENIED       -  if the image is not signed
  EFI_SUCCESS             -  if the image is signed

--*/
{
  EFI_IMAGE_DOS_HEADER                *DosHeader;
  UINT32                              PeCoffHeaderOffset;
  UINT16                              Magic;
  EFI_IMAGE_DATA_DIRECTORY            *SectionDataDir;
  EFI_IMAGE_OPTIONAL_HEADER_PTR_UNION PeHeader;


  DosHeader = (EFI_IMAGE_DOS_HEADER *)(UINTN)(ImageBase);
  if (DosHeader->e_magic == EFI_IMAGE_DOS_SIGNATURE) {
    //
    // DOS image header is present,
    // so read the PE header after the DOS image header.
    //
    PeCoffHeaderOffset = DosHeader->e_lfanew;
  } else {
    PeCoffHeaderOffset = 0;
  }
  PeHeader.Pe32 = (EFI_IMAGE_NT_HEADERS32 *) (ImageBase + PeCoffHeaderOffset);
  if (PeHeader.Pe32->Signature != EFI_IMAGE_NT_SIGNATURE) {
    //
    // It is not a valid Pe/Coff file.
    //
    return EFI_ACCESS_DENIED;
  }

  Magic = PeHeader.Pe32->OptionalHeader.Magic;
  if (Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
    //
    // Use PE32 offset.
    //
    SectionDataDir = (EFI_IMAGE_DATA_DIRECTORY *)&PeHeader.Pe32->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY];
  } else {
    //
    // Use PE32+ offset.
    //
    SectionDataDir = (EFI_IMAGE_DATA_DIRECTORY *)&PeHeader.Pe32Plus->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY];
  }

  if (SectionDataDir->Size == 0) {
    //
    // This image is not signed.
    //
    return EFI_ACCESS_DENIED;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
MergeImageBlockWithoutCompress (
  IN EFI_PHYSICAL_ADDRESS       TargetImageAddress
  )
/*++

Routine Description:
  Gather all block images and merge them into TargetImageAddress.

Arguments:
  TargetImageAddress    The address of the complete image.

Returns:
  EFI_SUCCESS             -  the function work successfully
  EFI_INVALID_PARAMETER   -  the image size is incorrect

--*/
{
  UINT32                        NumberOfImageBlk;
  UINT8                         *ImageBlkPtr;
  UINT8                         *Source;
  IMAGE_BLOCK                   *ImageBlock;
  UINTN                         Index;
  UINT64                        TotalImageSize;


  TotalImageSize = 0;
  ImageBlkPtr = (UINT8*)(UINTN)(SmmCpuSaveLocalState->ESI);
  //
  // The first DWORD is number of blocks, the second DWORD is reserved.
  //
  NumberOfImageBlk = *ImageBlkPtr;
  ImageBlock = (IMAGE_BLOCK*)(ImageBlkPtr + 8);
  if (BufferOverlapSmram ((VOID *) ImageBlkPtr, NumberOfImageBlk)) {
    return EFI_INVALID_PARAMETER;
  }

  for (Index = 0; Index < NumberOfImageBlk; Index++) {
    Source = (UINT8*)(UINTN)ImageBlock->ImageBlockAddress;
    if (BufferOverlapSmram ((VOID *) Source, (UINTN) ImageBlock->ImageBlockSize)) {
      return EFI_INVALID_PARAMETER;
    }
    EfiCommonLibCopyMem ((UINT8*)(UINTN)TargetImageAddress, Source, (UINTN)ImageBlock->ImageBlockSize);
    TotalImageSize += ImageBlock->ImageBlockSize;
    TargetImageAddress += ImageBlock->ImageBlockSize;
    ImageBlock++;
  }

  if (TotalImageSize == SmmCpuSaveLocalState->ECX) {
    return EFI_SUCCESS;
  } else {
    return EFI_INVALID_PARAMETER;
  }
}

EFI_STATUS
MergeImageBlock (
  IN OUT IMAGE_INFO             *ImageInfo
  )
/*++

Routine Description:
  Gather all block images and merge them into reserved memory.
  Compress the image data Before copying memory.

Arguments:
  ImageInfo       The information of the image.

Returns:
  EFI_SUCCESS             -  the function work successfully
  EFI_BUFFER_TOO_SMALL    -  the reserved memory is not enough
  EFI_INVALID_PARAMETER   -  the image size is incorrect
  EFI_PROTOCOL_ERROR      -  TianoCompressProtocol cannot be located

--*/
{
  UINT32                        NumberOfImageBlk;
  UINT8                         *ImageBlkPtr;
  UINT8                         *Source;
  IMAGE_BLOCK                   *ImageBlock;
  UINTN                         Index;
  EFI_STATUS                    Status;
  COMPRESS_DATA_HEADER          CompressDataHeader;
  COMPRESS_TABLE_HEADER         CompressTableHeader;
  UINT8                         *TargetImagePtr;
  UINT32                        RemainSize;
  UINTN                         Index2;
  UINT32                        MaxNumOfSplitBlock;
  TIANO_COMPRESS_PROTOCOL       *TianoCompress;


  TianoCompress = NULL;
  Status = mSmmRT->LocateProtocol (
                     &gTianoCompressProtocolGuid,
                     NULL,
                     &TianoCompress
                     );
  if (EFI_ERROR (Status)) {
    return EFI_PROTOCOL_ERROR;
  }

  Index2 = 0;
  Source = NULL;
  MaxNumOfSplitBlock = 0;

  //
  // Initialize the information of table header.
  //
  CompressTableHeader.NumOfBlock = 0;
  CompressTableHeader.TotalImageSize = 0;
  CompressTableHeader.TotalCompressedDataSize = sizeof (COMPRESS_TABLE_HEADER);

  //
  // The first data area is for COMPRESS_TABLE_HEADER.
  //
  TargetImagePtr = (UINT8*)(UINTN) ImageInfo->ImageAddress + sizeof (COMPRESS_TABLE_HEADER);

  //
  // The first DWORD is number of blocks, the second DWORD is reserved.
  //
  ImageBlkPtr = (UINT8*)(UINTN)(SmmCpuSaveLocalState->ESI);
  NumberOfImageBlk = *ImageBlkPtr;
  ImageBlock = (IMAGE_BLOCK*)(ImageBlkPtr + 8);
  if (BufferOverlapSmram ((VOID *) ImageBlkPtr, NumberOfImageBlk)) {
    return EFI_INVALID_PARAMETER;
  }

  for (Index = 0; Index < NumberOfImageBlk; Index++) {
    Source = (UINT8*)(UINTN) ImageBlock->ImageBlockAddress;
    if (BufferOverlapSmram ((VOID *) Source, (UINTN) ImageBlock->ImageBlockSize)) {
      return EFI_INVALID_PARAMETER;
    }
    RemainSize = (UINT32)(UINTN) ImageBlock->ImageBlockSize;
    //
    // Calculate the max block number of size of IMAGE_BLOCK_BUFFERSIZE_FOR_COMPRESS.(at least one)
    //
    if ((RemainSize % IMAGE_BLOCK_BUFFERSIZE_FOR_COMPRESS) == 0) {
      MaxNumOfSplitBlock = RemainSize / IMAGE_BLOCK_BUFFERSIZE_FOR_COMPRESS;
    } else {
      MaxNumOfSplitBlock = (RemainSize / IMAGE_BLOCK_BUFFERSIZE_FOR_COMPRESS) + 1;
    }

    for (Index2 = 0; Index2 < MaxNumOfSplitBlock; Index2++) {
      if (((RemainSize % IMAGE_BLOCK_BUFFERSIZE_FOR_COMPRESS) == 0) || (RemainSize >= IMAGE_BLOCK_BUFFERSIZE_FOR_COMPRESS)) {
        CompressDataHeader.SourceSize = IMAGE_BLOCK_BUFFERSIZE_FOR_COMPRESS;
      } else {
        CompressDataHeader.SourceSize = RemainSize;
      }

      CompressDataHeader.CompressedSize = CompressDataHeader.SourceSize * 2;
      //
      // We should not use the memory over reserved image size.
      //
      if (CompressTableHeader.TotalCompressedDataSize + CompressDataHeader.CompressedSize > ImageInfo->ImageSize) {
        return EFI_BUFFER_TOO_SMALL;
      }
      Status = TianoCompress->Compress (
                                Source,
                                CompressDataHeader.SourceSize,
                                TargetImagePtr + sizeof (COMPRESS_DATA_HEADER),
                                &CompressDataHeader.CompressedSize
                                );
      if (Status == EFI_BUFFER_TOO_SMALL) {
        //
        // We should not use the memory over reserved image size.
        //
        if (CompressTableHeader.TotalCompressedDataSize + CompressDataHeader.CompressedSize > ImageInfo->ImageSize) {
          return EFI_BUFFER_TOO_SMALL;
        }
        Status = TianoCompress->Compress (
                                  Source,
                                  CompressDataHeader.SourceSize,
                                  TargetImagePtr + sizeof (COMPRESS_DATA_HEADER),
                                  &CompressDataHeader.CompressedSize
                                  );
      }
      EfiCommonLibCopyMem (TargetImagePtr, &CompressDataHeader, sizeof (COMPRESS_DATA_HEADER));

      CompressTableHeader.TotalImageSize += CompressDataHeader.SourceSize;
      CompressTableHeader.TotalCompressedDataSize += CompressDataHeader.CompressedSize + sizeof (COMPRESS_DATA_HEADER);

      TargetImagePtr += CompressDataHeader.CompressedSize + sizeof (COMPRESS_DATA_HEADER);
      Source += CompressDataHeader.SourceSize;
      RemainSize -= CompressDataHeader.SourceSize;
    }

    ImageBlock++;
    CompressTableHeader.NumOfBlock += MaxNumOfSplitBlock;
  }

  ImageInfo->ImageSize = CompressTableHeader.TotalCompressedDataSize;
  //
  // Write information to the table header.
  //
  TargetImagePtr = (UINT8*)(UINTN) ImageInfo->ImageAddress;
  EfiCommonLibCopyMem (TargetImagePtr, &CompressTableHeader, sizeof (COMPRESS_TABLE_HEADER));

  if (CompressTableHeader.TotalImageSize == SmmCpuSaveLocalState->ECX) {
    return EFI_SUCCESS;
  } else {
    return EFI_INVALID_PARAMETER;
  }
}

EFI_STATUS
SecureFlashFunction (
  VOID
  )
{
  EFI_STATUS                    Status;
  EFI_SMM_VARIABLE_PROTOCOL     *SmmVariable;
  IMAGE_INFO                    ImageInfo;
  UINT8                         *ImageBase;
  IMAGE_BLOCK                   *ImageBlock;
  UINTN                         Size;
  UINT32                        TotalImageSize;

  //
  // mInPOST is a flag used for disabling IHISI write function at Runtime.
  // There is a SMI triggerd by SecureFlashDxe to modify this flag.
  //
  if ((mInPOST) && (SmmCpuSaveLocalState->ECX == SECURE_FLASH_SIGNATURE)) {
    mInPOST = FALSE;
    return EFI_SUCCESS;
  }

  Status = mSmmRT->LocateProtocol (
                     &gEfiSmmVariableProtocolGuid,
                     NULL,
                     &SmmVariable
                     );
  if (EFI_ERROR (Status)) {
    IhisiLibErrorCodeHandler (IhisiFunNotSupported);
    return Status;
  }

  //
  // The image size cannot be zero.
  //
  if (SmmCpuSaveLocalState->ECX == 0) {
    IhisiLibErrorCodeHandler (IhisiAccessProhibited);
    return EFI_INVALID_PARAMETER;
  }

  //
  // Get the address of the signed image(from ESI) and verify if it is a signed image.
  // The first DWORD is number of blocks, the second DWORD is reserved, skip them.
  // We only need the first block to verify if it is a signed image.
  //
  ImageBlock = (IMAGE_BLOCK*)(UINTN)(SmmCpuSaveLocalState->ESI + 8);
  ImageBase = (UINT8*)(UINTN)(ImageBlock->ImageBlockAddress);
  if (BufferOverlapSmram ((VOID *) ImageBlock, 1) || BufferOverlapSmram ((VOID *) ImageBase, 1)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = VerifySignedImage (ImageBase);
  if (Status == EFI_SUCCESS) {
    //
    // Get reserved memory address to save the image we need.
    //
    Size = sizeof (IMAGE_INFO);
    SmmVariable->GetVariable (
                   L"SecureFlashInfo",
                   &gSecureFlashInfoGuid,
                   NULL,
                   &Size,
                   &ImageInfo
                   );

    //
    // If the image size is smaller than the size of reserved memroy, no need to be compressed.
    //
    TotalImageSize = SmmCpuSaveLocalState->ECX;
    if (TotalImageSize <= ImageInfo.ImageSize) {
      Status = MergeImageBlockWithoutCompress (ImageInfo.ImageAddress);
      ImageInfo.ImageSize = TotalImageSize;
      ImageInfo.Compressed = FALSE;
    } else {
      Status = MergeImageBlock (&ImageInfo);
      ImageInfo.Compressed = TRUE;
    }

    if (EFI_ERROR (Status)) {
      IhisiLibErrorCodeHandler (IhisiObLenTooSmall);
      return EFI_INVALID_PARAMETER;
    }

    //
    // Set this variable to inform BIOS of doing secure flash.
    //
    ImageInfo.FlashMode = TRUE;
    SmmVariable->SetVariable (
                   L"SecureFlashInfo",
                   &gSecureFlashInfoGuid,
                   EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                   sizeof (IMAGE_INFO),
                   &ImageInfo
                   );


    //
    // return BIOS support secure flash type
    //   0: winflash should do S3
    //   1: winflash should do reboot
    //   2: winflash should do shutdown
    //   3: winflash should do nothing
    //
    SmmCpuSaveLocalState->ECX = WINFLASH_DO_REBOOT;

    IhisiLibErrorCodeHandler (IhisiSuccess);
  } else {
    IhisiLibErrorCodeHandler (IhisiInvalidParamFatalError);
  }

  return Status;
}

