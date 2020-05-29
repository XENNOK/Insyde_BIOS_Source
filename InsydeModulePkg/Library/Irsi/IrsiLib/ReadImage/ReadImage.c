/** @file
  Library Instance implementation for IRSI Read Image

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

#include <PiDxe.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/IrsiLib.h>
#include <Library/IrsiRegistrationLib.h>
#include <Library/DebugLib.h>
#include <Protocol/FvRegionInfo.h>
#include <Library/FvRegionAccessLib.h>



/**
  Irsi ReadImage function

  This routine reads image from platform with specific image GUID defined in
  ReadImageBuf

  @param ReadImageBuf           pointer to IRSI_READ_IMAGE structure

  @return EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
IrsiReadImage (
  VOID     *ReadImageBuf
  )
{
   EFI_STATUS                         Status;
   IRSI_READ_IMAGE                    *ReadImage;
   UINTN                              ImageSize;

   ReadImage = (IRSI_READ_IMAGE *)ReadImageBuf;
   ReadImage->Header.StructureSize = sizeof(IRSI_READ_IMAGE);
   
   ImageSize = (UINTN)ReadImage->ImageSize;
   Status = ReadFvRegion (
               &ReadImage->ImageTypeGuid,
               &ImageSize,
               (UINT8 *)(UINTN)ReadImage->ImageBufferPtr,
               &ReadImage->UpdateStatus
               );
   ReadImage->ImageSize = (UINT32)ImageSize;
   ReadImage->Header.ReturnStatus = IRSI_STATUS(Status);
   return Status;
}

/**
  Irsi ReadImage Initialization

  This routine is a LibraryClass constructor for IrsiReadImage, it will
  register IrsiReadImage function to the Irsi function database

  @param ImageHandle            A handle for the image that is initializing this driver
  @param SystemTable            A pointer to the EFI system table

  @retval EFI_SUCCESS           Module initialized successfully
  @retval Others                Module initialization failed

**/
EFI_STATUS
EFIAPI
IrsiReadImageInit (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                     Status;

  Status = IrsiRegisterFunction (
               &gIrsiServicesGuid,
               IRSI_READ_IMAGE_COMMAND,
               IrsiReadImage
               );

  return Status;
}