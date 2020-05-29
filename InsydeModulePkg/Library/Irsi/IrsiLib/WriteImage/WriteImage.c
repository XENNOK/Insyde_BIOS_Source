/** @file
  Library Instance implementation for IRSI Write Image

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

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/DebugLib.h>
#include <Guid/EventGroup.h>
#include <Library/IrsiRegistrationLib.h>
#include <Library/IrsiLib.h>
#include <Protocol/FvRegionInfo.h>
#include <Library/FvRegionAccessLib.h>
  
  
/**
  Irsi WriteImage function

  This routine write image to platform with specific capsule GUID defined in
  WriteImageBuf

  @param WriteImageBuf        pointer to IRSI_WRITE_IMAGE structure
  @param WriteImageSize       the size of WriteImageBuf, normally it is equal to
                              sizeof(IRSI_WRITE_IMAGE)

  @return EFI_UNSUPPORTED       Not supported yet

**/
EFI_STATUS
EFIAPI
IrsiWriteImage (
  VOID     *WriteImageBuf
  )
{
   EFI_STATUS                         Status;
   IRSI_WRITE_IMAGE                   *WriteImage;
   UINTN                              ImageSize;

   WriteImage = (IRSI_WRITE_IMAGE *)WriteImageBuf;
   WriteImage->Header.StructureSize = sizeof(IRSI_WRITE_IMAGE);

   ImageSize = (UINTN)WriteImage->ImageSize;
   Status = WriteFvRegion (
               &WriteImage->ImageTypeGuid,
               ImageSize,
               (UINT8 *)(UINTN)WriteImage->ImageBufferPtr,
               &WriteImage->UpdateStatus
               );
   WriteImage->ImageSize = (UINT32)ImageSize;
   WriteImage->Header.ReturnStatus = IRSI_STATUS(Status);
   return Status;
}


/**
  Irsi WriteImage Initialization

  This routine is a LibraryClass constructor for IrsiWriteImage, it will
  register IrsiWriteImage function to the Irsi function database

  @param ImageHandle            A handle for the image that is initializing this driver
  @param SystemTable            A pointer to the EFI system table

  @retval EFI_SUCCESS           Module initialized successfully
  @retval Others                Module initialization failed

**/
EFI_STATUS
EFIAPI
IrsiWriteImageInit (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                     Status;

  Status = IrsiRegisterFunction (
               &gIrsiServicesGuid,
               IRSI_WRITE_IMAGE_COMMAND,
               IrsiWriteImage
               );

  return Status;
}