/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/

#include <Library/AcpiPlatformCommon.h>

/** 
 Locate the first instance of FV protocol that contains the ACPI table
 storage file.
 
 @param[out]      Return pointer to the first instance of Fv protocol which
                         contains the ACPI table.
 
 @retval   EFI_SUCCESS                   The function completed successfully.
 @retval   EFI_NOT_FOUND              The protocol could not be located.
 @retval   EFI_OUT_OF_RESOURCES  There are not enough resources to find the protocol.

*/
EFI_STATUS
LocateSupportFvProtocol (
  OUT    VOID                          **Instance
  )
{
  EFI_STATUS              Status;
  EFI_HANDLE              *HandleBuffer;
  UINTN                   NumberOfHandles;
  EFI_FV_FILETYPE         FileType;
  UINT32                  FvStatus;
  EFI_FV_FILE_ATTRIBUTES  Attributes;
  UINTN                   Size;
  UINTN                   Index;

  //
  // Locate protocol.
  //
  Status = gBS->LocateHandleBuffer (
    ByProtocol,
    &gEfiFirmwareVolume2ProtocolGuid,
    NULL,
    &NumberOfHandles,
    &HandleBuffer
    );
  if (EFI_ERROR (Status)) {
    //
    // Defined errors at this time are not found and out of resources.
    //
    return Status;
  }

  //
  // Looking for FV with ACPI storage file
  //

  for (Index = 0; Index < NumberOfHandles; Index ++) {
    //
    // Get the protocol on this handle
    // This should not fail because of LocateHandleBuffer
    //
    Status = gBS->HandleProtocol (
      HandleBuffer[Index],
      &gEfiFirmwareVolume2ProtocolGuid,
      Instance
      );
    ASSERT_EFI_ERROR (Status);


    //
    // See if it has the ACPI storage file
    //
    Size = 0;
    FvStatus = 0;
    Status = ((EFI_FIRMWARE_VOLUME2_PROTOCOL*) (*Instance))->ReadFile (
      *Instance,
      &gEfiAcpiTableStorageGuid,
      NULL,
      &Size,
      &FileType,
      &Attributes,
      &FvStatus
      );

    //
    // If we found it, then we are done
    //
    if (Status == EFI_SUCCESS) {
      break;
    }
  }

  //
  // Our exit status is determined by the success of the previous operations
  // If the protocol was found, Instance already points to it.
  //

  //
  // Free any allocated buffers
  //
  gBS->FreePool (HandleBuffer);

  return Status;
}

EFI_STATUS
GetAcpiCommonTables (
  IN  EFI_FIRMWARE_VOLUME2_PROTOCOL  *FwVol,
  IN  INTN                           Instance,
  OUT EFI_ACPI_COMMON_HEADER         **CommonTablePtr
  )
{
  UINTN                         Size;
  UINT32                        FvStatus;
  EFI_STATUS                    Status;
  
  Size = 0;
  Status = FwVol->ReadSection (
      FwVol,
      &gEfiAcpiTableStorageGuid,
      EFI_SECTION_RAW,
      Instance,
      CommonTablePtr,
      &Size,
      &FvStatus
      );

  if (Status == EFI_BUFFER_TOO_SMALL) {
    Status = FwVol->ReadSection (
        FwVol,
        &gEfiAcpiTableStorageGuid,
        EFI_SECTION_RAW,
        Instance,
        CommonTablePtr,
        &Size,
        &FvStatus
        );
  }
  
  return Status;
}
    
EFI_STATUS
PublishAcpiTables (
  IN EFI_ACPI_SUPPORT_PROTOCOL   *AcpiSupport,
  IN UINT32                      AcpiVersion
  )
{
  EFI_STATUS                     Status;
  
  Status = AcpiSupport->PublishTables (AcpiSupport, AcpiVersion);
  ASSERT_EFI_ERROR (Status);
 
  return EFI_SUCCESS;
}
