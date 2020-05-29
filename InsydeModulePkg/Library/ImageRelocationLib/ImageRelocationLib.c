/** @file
  Image relocation related functions. In current design, only support relocate
  BS driver to runtime driver.

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

#include <Library/ImageRelocationLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DxeServicesLib.h>
#include <Library/DevicePathLib.h>

#include <Protocol/LoadedImage.h>

/*
  Get PE32 section header, section type is EFI_SECTION_PE32 (0x10)

  @param[in] ImageBufferSize    Image buffer size in bytes.
  @param[in] ImageBuffer        The buffer to save the contents of image.
  @param[in] Section            Output double pointer to EFI_SECTION_PE32.

  @retval EFI_SUCCESS           Get start adress of PE32 section header successful.
  @retval EFI_NOT_FOUND         Cannot find PE32 section in this input image.
*/
EFI_STATUS
GetPe32SectionHeader (
  IN    UINTN                        ImageBufferSize,
  IN    UINT8                        *ImageBuffer,
  OUT   EFI_COMMON_SECTION_HEADER    **Section
  )
{
  EFI_STATUS                  Status;
  EFI_COMMON_SECTION_HEADER   *CommonSectionHeader;
  UINT8                       *WorkingHeaderAddress;

  CommonSectionHeader = (EFI_COMMON_SECTION_HEADER *) ImageBuffer;
  //
  // Search PE32 header until out of ImageBuffer
  //
  Status = EFI_NOT_FOUND;
  while ((UINTN) (ImageBuffer + ImageBufferSize) > (UINTN) CommonSectionHeader) {
    if (CommonSectionHeader->Type == EFI_SECTION_PE32) {
      *Section = CommonSectionHeader;
      Status  = EFI_SUCCESS;
      break;
    }
    //
    // Calculate and push to next header, alignment is 4 btyes
    //
    WorkingHeaderAddress = (UINT8 *) CommonSectionHeader;
    WorkingHeaderAddress += IS_SECTION2 (CommonSectionHeader) ? SECTION2_SIZE (CommonSectionHeader) : SECTION_SIZE (CommonSectionHeader);
    CommonSectionHeader = (EFI_COMMON_SECTION_HEADER *) (((UINTN) WorkingHeaderAddress + 0x03) & (~(UINTN) 3));
  }

  return Status;
}

/**
  Retrieves the magic value from the PE/COFF header.

  @param  Pe32Header                          The buffer in which to return the PE32, PE32+, or TE header.

  @return EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC   Image is PE32.
  @return EFI_IMAGE_NT_OPTIONAL_HDR64_MAGIC   Image is PE32+.
**/
UINT16
PeCoffLoaderGetPeHeaderMagicValue (
  IN  EFI_IMAGE_OPTIONAL_HEADER_UNION  *Pe32Header
  )
{
  //
  // Reference from BasePeCoff.c, function PeCoffLoaderGetPeHeaderMagicValue
  //
  if ((Pe32Header->Pe32.FileHeader.Machine == IMAGE_FILE_MACHINE_IA64) && (Pe32Header->Pe32.OptionalHeader.Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC)) {
    return EFI_IMAGE_NT_OPTIONAL_HDR64_MAGIC;
  }
  //
  // Return the magic value from the PC/COFF Optional Header
  //
  return Pe32Header->Pe32.OptionalHeader.Magic;
}

/**
  Force change image type to EFI_IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER (0x0C)

  @param  Pe32Header                 Pe32 Header location

  @retval EFI_SUCCESS                Image type changed successful.
  @retval EFI_UNSUPPORTED            Image not supported.
**/
EFI_STATUS
UpdateSubSystemTypeToRuntimeDriver (
  IN OUT EFI_COMMON_SECTION_HEADER       *Pe32Header
  )
{
  EFI_STATUS                      Status;
  EFI_IMAGE_DOS_HEADER            *DosHeader;
  UINT32                          PeCoffHeaderOffset;
  EFI_IMAGE_OPTIONAL_HEADER_UNION *ImageHeader;
  UINT16                          MagicNumber;

  PeCoffHeaderOffset = 0;

  //
  // Chech PE32 is DOS image
  //
  DosHeader          = (EFI_IMAGE_DOS_HEADER *) Pe32Header;
  if (DosHeader->e_magic == EFI_IMAGE_DOS_SIGNATURE) {
    PeCoffHeaderOffset = DosHeader->e_lfanew;
  }

  ImageHeader = (EFI_IMAGE_OPTIONAL_HEADER_UNION *) ((UINTN)Pe32Header + PeCoffHeaderOffset);

  Status = EFI_UNSUPPORTED;
  if (ImageHeader->Te.Signature == EFI_TE_IMAGE_HEADER_SIGNATURE) {
    ImageHeader->Te.Subsystem = EFI_IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER;
    Status    = EFI_SUCCESS;

  } else if (ImageHeader->Pe32.Signature == EFI_IMAGE_NT_SIGNATURE) {
    MagicNumber = PeCoffLoaderGetPeHeaderMagicValue ((VOID *) ImageHeader);

    if (MagicNumber == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
      ImageHeader->Pe32.OptionalHeader.Subsystem = EFI_IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER;
      Status    = EFI_SUCCESS;

    } else if (MagicNumber == EFI_IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
      ImageHeader->Pe32Plus.OptionalHeader.Subsystem = EFI_IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER;
      Status    = EFI_SUCCESS;
    }
  }
  return Status;
}


/**
  Function uses image handle to check this driver is runtime driver or not

  @param[in] ImageHandle   Input Image handle.

  @retval TRUE             This is a runtime driver.
  @retval FALSE            This isn't a runtime driver.
**/
BOOLEAN
IsRuntimeDriver (
  IN EFI_HANDLE       ImageHandle
  )
{
  BOOLEAN                          MatchedTypeFound;
  EFI_STATUS                       Status;
  EFI_LOADED_IMAGE_PROTOCOL        *LoadedImage;

  MatchedTypeFound = FALSE;

  Status = gBS->HandleProtocol (
                  ImageHandle,
                  &gEfiLoadedImageProtocolGuid,
                  (VOID **)&LoadedImage
                  );
  if (!EFI_ERROR (Status) && LoadedImage->ImageCodeType == EfiRuntimeServicesCode) {
    MatchedTypeFound = TRUE;
  }

  return MatchedTypeFound;
}


/**
  Relocation this driver to RuntimeService from DXE
  It will read image from FV, then LoadImage and StartImage to relocation driver

  @param[in] ParentImageHandle     Parent image to load runtime image

  @retval EFI_SUCCESS              Relocation success
  @retval others                   Failed from another driver
**/
EFI_STATUS
RelocateImageToRuntimeDriver (
  IN  EFI_HANDLE                        ParentImageHandle
  )
{
  EFI_STATUS                          Status;
  UINT8                               *ImageBuffer;
  UINTN                               ImageBufferSize;
  EFI_HANDLE                          ImageHandle;
  EFI_LOADED_IMAGE_PROTOCOL           *LoadedImage;
  EFI_GUID                            FileName;
  EFI_DEVICE_PATH_PROTOCOL            *ImageDevicePath;
  EFI_DEVICE_PATH_PROTOCOL            *CompleteFilePath;

  ImageBuffer      = NULL;
  CompleteFilePath = NULL;

  //
  // Create file path device for read file and load image
  //
  Status = gBS->HandleProtocol (
                  ParentImageHandle,
                  &gEfiLoadedImageProtocolGuid,
                  (VOID **)&LoadedImage
                  );
  if (EFI_ERROR (Status)) {
   goto Done;
  }

  FileName = ((MEDIA_FW_VOL_FILEPATH_DEVICE_PATH *) (LoadedImage->FilePath))->FvFileName;
  Status = GetSectionFromFv (
             &FileName,
             EFI_SECTION_PE32,
             0,
             (VOID **)&ImageBuffer,
             &ImageBufferSize
             );
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  Status = UpdateSubSystemTypeToRuntimeDriver ((EFI_COMMON_SECTION_HEADER *) ImageBuffer);
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  Status = gBS->HandleProtocol (
                  LoadedImage->DeviceHandle,
                  &gEfiDevicePathProtocolGuid,
                  (VOID*)&ImageDevicePath
                  );
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  CompleteFilePath = AppendDevicePath (ImageDevicePath, LoadedImage->FilePath);
  ASSERT (CompleteFilePath != NULL);

  ImageHandle = NULL;
  Status = gBS->LoadImage (
                  FALSE,
                  ParentImageHandle,
                  CompleteFilePath,
                  (VOID *) ImageBuffer,
                  ImageBufferSize,
                  &ImageHandle
                  );
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  Status = gBS->StartImage (
                  ImageHandle,
                  NULL,
                  NULL
                  );
Done:

  if (ImageBuffer != NULL) {
    FreePool (ImageBuffer);
  }

  if (CompleteFilePath != NULL) {
    FreePool (CompleteFilePath);
  }

  return Status;
}