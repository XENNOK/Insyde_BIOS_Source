/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <PiDxe.h>

#include <Protocol/AcpiTable.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/AcpiSystemDescriptionTable.h>
#include <Protocol/SetupUtility.h>
#include <Protocol/PublishAcpiTableDone.h>

#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>

#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/SLP2_0.h>

#include <AcpiHeaderDefaultValue.h>
#include <Library/KernelConfigLib.h>
#include <Library/DxeChipsetSvcLib.h>

EFI_STATUS
PlatformUpdateTables (
  IN OUT EFI_ACPI_COMMON_HEADER       **Table,
  IN KERNEL_CONFIGURATION             *SetupVariable
  );

EFI_STATUS
MadtUpdateCommon (
  IN OUT EFI_ACPI_COMMON_HEADER       *Table,
  IN KERNEL_CONFIGURATION             *SetupVariable
  );

EFI_STATUS
FacpUpdateCommon (
  IN OUT EFI_ACPI_COMMON_HEADER       *Table,
  IN KERNEL_CONFIGURATION             *SetupVariable
  );

EFI_STATUS
FacsUpdateCommon (
  IN OUT EFI_ACPI_COMMON_HEADER       *Table,
  IN KERNEL_CONFIGURATION             *SetupVariable
  );

EFI_STATUS
SpcrUpdateCommon (
  IN OUT EFI_ACPI_COMMON_HEADER       *Table,
  IN KERNEL_CONFIGURATION             *SetupVariable
  );

VOID
DsdtUpdateCommon (
  IN OUT EFI_ACPI_COMMON_HEADER       *Table
  );

EFI_STATUS
SlicUpdateCommon (
  IN OUT EFI_ACPI_COMMON_HEADER       *Table
  );

EFI_HANDLE  mAcpiPlatformHandle = NULL;

/**
  Locate the first instance of a protocol.  If the protocol requested is an
  FV protocol, then it will return the first FV that contains the ACPI table
  storage file.

  @param  Instance      Return pointer to the first instance of the protocol

  @return EFI_SUCCESS           The function completed successfully.
  @return EFI_NOT_FOUND         The protocol could not be located.
  @return EFI_OUT_OF_RESOURCES  There are not enough resources to find the protocol.

**/
EFI_STATUS
LocateFvInstanceWithTables (
  OUT EFI_FIRMWARE_VOLUME2_PROTOCOL **Instance
  )
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    *HandleBuffer;
  UINTN                         NumberOfHandles;
  EFI_FV_FILETYPE               FileType;
  UINT32                        FvStatus;
  EFI_FV_FILE_ATTRIBUTES        Attributes;
  UINTN                         Size;
  UINTN                         Index;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *FvInstance;

  FvStatus = 0;

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

  for (Index = 0; Index < NumberOfHandles; Index++) {
    //
    // Get the protocol on this handle
    // This should not fail because of LocateHandleBuffer
    //
    Status = gBS->HandleProtocol (
                     HandleBuffer[Index],
                     &gEfiFirmwareVolume2ProtocolGuid,
                     (VOID**) &FvInstance
                     );
    ASSERT_EFI_ERROR (Status);

    //
    // See if it has the ACPI storage file
    //
    Status = FvInstance->ReadFile (
                           FvInstance,
                           (EFI_GUID*)PcdGetPtr (PcdAcpiTableStorageFile),
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
      *Instance = FvInstance;
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


/**
  This function calculates and updates an UINT8 checksum.

  @param  Buffer          Pointer to buffer to checksum
  @param  Size            Number of bytes to checksum

**/
VOID
AcpiPlatformChecksum (
  IN UINT8      *Buffer,
  IN UINTN      Size
  )
{
  UINTN ChecksumOffset;

  ChecksumOffset = OFFSET_OF (EFI_ACPI_DESCRIPTION_HEADER, Checksum);

  //
  // Set checksum to 0 first
  //
  Buffer[ChecksumOffset] = 0;

  //
  // Update checksum value
  //
  Buffer[ChecksumOffset] = CalculateCheckSum8 (Buffer, Size);
}


/**
  Entrypoint of Acpi Platform driver.

  @param  ImageHandle Handle for this drivers loaded image protocol.
  @param  SystemTable EFI system table.

  @return EFI_SUCCESS The driver installed without error.
  @return EFI_ABORTED the driver encountered an error and could not complete installation of the ACPI tables.

**/
EFI_STATUS
EFIAPI
AcpiPlatformEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                     Status;
  EFI_ACPI_TABLE_PROTOCOL        *AcpiTableProtocol;
  EFI_FIRMWARE_VOLUME2_PROTOCOL  *FwVol;
  INTN                           Instance;
  EFI_ACPI_COMMON_HEADER         *CurrentTable;
  UINTN                          TableHandle;
  UINT32                         FvStatus;
  UINTN                          TableSize;
  UINTN                          Size;
  KERNEL_CONFIGURATION                   KernelConfiguration;
  EFI_STATUS                             TableStatus;

  Instance     = 0;
  CurrentTable = NULL;
  TableHandle  = 0;

  //
  // Find the AcpiTable protocol
  //
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID**)&AcpiTableProtocol);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return EFI_ABORTED;
  }

  //
  // Locate the firmware volume protocol
  //
  Status = LocateFvInstanceWithTables (&FwVol);
  if (EFI_ERROR (Status)) {
    return EFI_ABORTED;
  }
  
  Status = GetKernelConfiguration (&KernelConfiguration);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }
  
  //
  // Read tables from the storage file.
  //
  while (Status == EFI_SUCCESS) {
    Status = FwVol->ReadSection (
                      FwVol,
                      (EFI_GUID*)PcdGetPtr (PcdAcpiTableStorageFile),
                      EFI_SECTION_RAW,
                      Instance,
                      (VOID**)&CurrentTable,
                      &Size,
                      &FvStatus
                      );
    if (!EFI_ERROR (Status)) {
      //
      // Perform any table specific updates.
      //
      TableStatus = PlatformUpdateTables (&CurrentTable, &KernelConfiguration);
      if (TableStatus == EFI_SUCCESS) {
        //
        // Add the table
        //
        TableHandle = 0;
        TableSize = ((EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable)->Length;
        ASSERT (Size >= TableSize);

        //
        // Checksum ACPI table
        //
        AcpiPlatformChecksum ((UINT8*)CurrentTable, TableSize);

        //
        // Install ACPI table
        //
        Status = AcpiTableProtocol->InstallAcpiTable (
                                      AcpiTableProtocol,
                                      CurrentTable,
                                      TableSize,
                                      &TableHandle
                                      );
      }
      
      if (EFI_ERROR (TableStatus)) {
        DEBUG ((EFI_D_ERROR, "\nWARNING: Table signature=0x%x was removed!!\n", CurrentTable->Signature));
      }
      //
      // Free memory allocated by ReadSection
      //
      gBS->FreePool (CurrentTable);
      
    }
    //
    // Increment the instance
    //
    Instance++;
    CurrentTable = NULL;

  }
  
  //
  // Install Publish ACPI Tables Done Protocol for notification and dependency
  //
  Status = gBS->InstallProtocolInterface (
                  &mAcpiPlatformHandle,
                  &gPublishAcpiTableDoneProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  NULL
                  );
  
  return EFI_SUCCESS;
}

/**
  This function will update any runtime platform specific information.
  This currently includes:
    Setting OEM table values, ID, table ID, creator ID and creator revision.
    Enabling the proper processor entries in the APIC tables
  It also indicates with which ACPI table version the table belongs.

  @param  Table     The table to update
  
  @return EFI_SUCCESS           The function completed successfully.

**/
EFI_STATUS
PlatformUpdateTables (
  IN OUT EFI_ACPI_COMMON_HEADER       **Table,
  IN KERNEL_CONFIGURATION             *SetupVariable
  )
{
  EFI_STATUS                              Status;
  EFI_ACPI_DESCRIPTION_HEADER             *TableHeader;

  Status = EFI_SUCCESS;
  
  //
  // This will be accurate except for FACS structure
  //
  TableHeader = (EFI_ACPI_DESCRIPTION_HEADER*) (*Table);
  
  //
  // Common update code for all platform
  //
  switch ((*Table)->Signature) {
  case EFI_ACPI_3_0_MULTIPLE_APIC_DESCRIPTION_TABLE_SIGNATURE:
    Status = MadtUpdateCommon ((*Table), SetupVariable);
    break;
  case EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE:
    Status = FacpUpdateCommon ((*Table), SetupVariable);
    break;
  case EFI_ACPI_SLIC_TABLE_SIGNATURE:
    Status = SlicUpdateCommon ((*Table));
    break;
  case EFI_ACPI_3_0_FIRMWARE_ACPI_CONTROL_STRUCTURE_SIGNATURE:
    Status = FacsUpdateCommon ((*Table), SetupVariable);
    break;
  case EFI_ACPI_3_0_SERIAL_PORT_CONSOLE_REDIRECTION_TABLE_SIGNATURE:
    Status = SpcrUpdateCommon ((*Table), SetupVariable);
    break;
  case EFI_ACPI_3_0_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE:
    DsdtUpdateCommon ((*Table));
    break;
  }
  
  //
  // Chipset Specific table update
  //
  DxeCsSvcUpdateAcpiTable (TableHeader, &Status);
  
  //
  // Update the OEM and creator information for every table except FACS.
  //
  if ((*Table)->Signature != EFI_ACPI_3_0_FIRMWARE_ACPI_CONTROL_STRUCTURE_SIGNATURE) {
    *(UINT32*) (((EFI_ACPI_DESCRIPTION_HEADER*)(*Table))->OemId) = (UINT32) SIGNATURE_32('I','N','S','Y');
    *(UINT16*) (((EFI_ACPI_DESCRIPTION_HEADER*)(*Table))->OemId + 4) = (UINT16) SIGNATURE_16('D','E');
    //
    // Update the OEM Table ID
    //
    TableHeader->OemTableId             = EFI_ACPI_OEM_TABLE_ID;
    //
    // Update creator information
    //
    TableHeader->CreatorId              = EFI_ACPI_CREATOR_ID;
    TableHeader->CreatorRevision        = EFI_ACPI_CREATOR_REVISION;
  }
  return Status;
}

