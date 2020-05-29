/** @file
  This DXE driver configures and supports Dptf.

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

@copyright
  Copyright (c) 2013 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

**/
#include "DptfDxe.h"

ACPI_PLATFORM_POLICY_PROTOCOL              *mAcpiPlatformPolicyProtocol;

//EFI_RUNTIME_SERVICES          *gRT;

///
/// Driver entry point
///
//EFI_DRIVER_ENTRY_POINT (InitializeDptf)

/**

  Dptf DXE support.

  @param[in] ImageHandle   - Pointer to the loaded image protocol for this driver
  @param[in] SystemTable   - Pointer to the EFI System Table

  @retval EFI_SUCCESS - The driver installed/initialized correctly.

**/
EFI_STATUS
EFIAPI
InitializeDptf (
  IN EFI_HANDLE               ImageHandle,
  IN EFI_SYSTEM_TABLE         *SystemTable
  )
{
  EFI_STATUS                  Status;

  //gRT = SystemTable->RuntimeServices;

  ///
  /// Initialize the EFI Runtime Library
  ///
  //DxeInitializeDriverLib (ImageHandle, SystemTable);

  ///
  /// S3 boot script
  ///
  //INITIALIZE_SCRIPT (ImageHandle, SystemTable);

  ///
  /// Locate platform configuration information and copy it to a global variable.
  ///
  Status = gBS->LocateProtocol (
                  &gAcpiPlatformPolicyProtocolGuid,
                  NULL,
                  (VOID**) &mAcpiPlatformPolicyProtocol
                  );
  if (EFI_ERROR(Status)) {
    DEBUG ((EFI_D_ERROR, "No ACPI Platform Policy Protocol available."));
  } else {
    DEBUG ((EFI_D_INFO, "ACPI Platform Policy Protocol is loaded."));
  }
  ASSERT_EFI_ERROR(Status);

  ///
  /// Check if Dptf is enabled and load the ACPI SSDT.
  ///
  if (mAcpiPlatformPolicyProtocol->EnableDptf == 1) {
    ///
    /// Load the SSDT ACPI Tables.
    ///
    switch (mAcpiPlatformPolicyProtocol->BoardId) {
    case 0x30: // BoardId for Ffrd
      LoadAcpiTablesFfrd ();
      break;
    default:
      LoadAcpiTables ();
    }
  }

  return EFI_SUCCESS;
}

VOID
LoadAcpiTables(
  VOID
  )
/**
@brief
  This procedure loads the ACPI SSDT tables.

  @param[in] None

  @retval None

**/
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    *HandleBuffer;
  UINTN                         NumberOfHandles;
  UINTN                         Index;
  EFI_FIRMWARE_VOLUME_PROTOCOL  *FwVol;
  EFI_ACPI_TABLE_PROTOCOL       *AcpiTable;
  INTN                          Instance;
  EFI_ACPI_COMMON_HEADER        *Table;
  UINTN                         Size;
  EFI_FV_FILETYPE               FileType;
  EFI_FV_FILE_ATTRIBUTES        Attributes;
  UINT32                        FvStatus;
  EFI_ACPI_DESCRIPTION_HEADER   *TableHeader;
  UINTN                         TableHandle;
  EFI_ACPI_TABLE_VERSION        Version;
  BOOLEAN                       LoadTable;

  FwVol = NULL;
  Table = NULL;

  ///
  /// Locate FV protocol.
  ///
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolumeProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Look for FV with ACPI storage file
  ///
  for (Index = 0; Index < NumberOfHandles; Index++) {

    ///
    /// Get the protocol on this handle
    /// This should not fail because of LocateHandleBuffer
    ///
    Status = gBS->HandleProtocol (
                  HandleBuffer[Index],
                  &gEfiFirmwareVolumeProtocolGuid,
                  (VOID **) &FwVol
                  );
    ASSERT_EFI_ERROR (Status);

    if ((Status == EFI_SUCCESS) && (FwVol != NULL)) {
      ///
      /// See if it has the ACPI storage file
      ///
      Size      = 0;
      FvStatus  = 0;
      Status = FwVol->ReadFile (
                      FwVol,
                      &gDptfAcpiTableStorageGuid,
                      NULL,
                      &Size,
                      &FileType,
                      &Attributes,
                      &FvStatus
                      );

      ///
      /// If we found it, then we are done
      ///
      if (Status == EFI_SUCCESS) {
        break;
      }
    }
  }

  ///
  /// Our exit status is determined by the success of the previous operations
  /// If the protocol was found, Instance already points to it.
  ///
  /// Free any allocated buffers
  ///
  FreePool (HandleBuffer);

  ///
  /// Sanity check that we found our data file
  ///
  ASSERT (FwVol);
  if (FwVol == NULL) {
    return;
  }

  ///
  /// By default, a table belongs in all ACPI table versions published.
  ///
  Version = EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0;

  ///
  /// Find the Table protocol
  ///
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **) &AcpiTable);

  ///
  /// Read tables from the storage file.
  ///
  Instance = 0;

  while (Status == EFI_SUCCESS) {
    ///
    /// Read the ACPI tables
    ///
    Status = FwVol->ReadSection (
                          FwVol,
                          &gDptfAcpiTableStorageGuid,
                          EFI_SECTION_RAW,
                          Instance,
                          (VOID **) &Table,
                          &Size,
                          &FvStatus
                          );

    if (!EFI_ERROR (Status)) {

      LoadTable = FALSE;
      TableHeader = (EFI_ACPI_DESCRIPTION_HEADER *) Table;

      switch (((EFI_ACPI_DESCRIPTION_HEADER*) TableHeader)->OemTableId) {

      case SIGNATURE_64 ('D', 'p', 't', 'f', 'T', 'a', 'b', 'l'):
        ///
        /// This is Dptf SSDT. Dptf should be enabled if we reach here so load the table.
        ///
        LoadTable = TRUE;
        DEBUG ((EFI_D_INFO, "Found Dptf SSDT signature."));
        break;

      default:
        break;
      }

      ///
      /// Add the table
      ///
      if (LoadTable) {
        TableHandle = 0;
        Status = AcpiTable->InstallAcpiTable (
                                  AcpiTable,
                                  Table,
                                  Table->Length,
                                  &TableHandle
                                  );
      }

      ///
      /// Increment the instance
      ///
      Instance++;
      Table = NULL;
    }
  }
}

VOID
LoadAcpiTablesFfrd(
  VOID
  )
/**
@brief
  This procedure loads the ACPI SSDT tables.

  @param[in] None

  @retval None

**/
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    *HandleBuffer;
  UINTN                         NumberOfHandles;
  UINTN                         Index;
  EFI_FIRMWARE_VOLUME_PROTOCOL  *FwVol;
  EFI_ACPI_TABLE_PROTOCOL       *AcpiTable;
  INTN                          Instance;
  EFI_ACPI_COMMON_HEADER        *Table;
  UINTN                         Size;
  EFI_FV_FILETYPE               FileType;
  EFI_FV_FILE_ATTRIBUTES        Attributes;
  UINT32                        FvStatus;
  EFI_ACPI_DESCRIPTION_HEADER   *TableHeader;
  UINTN                         TableHandle;
  EFI_ACPI_TABLE_VERSION        Version;
  BOOLEAN                       LoadTable;

  FwVol = NULL;
  Table = NULL;

  ///
  /// Locate FV protocol.
  ///
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolumeProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Look for FV with ACPI storage file
  ///
  for (Index = 0; Index < NumberOfHandles; Index++) {

    ///
    /// Get the protocol on this handle
    /// This should not fail because of LocateHandleBuffer
    ///
    Status = gBS->HandleProtocol (
                  HandleBuffer[Index],
                  &gEfiFirmwareVolumeProtocolGuid,
                  (VOID **) &FwVol
                  );
    ASSERT_EFI_ERROR (Status);

    if ((Status == EFI_SUCCESS) && (FwVol != NULL)) {
      ///
      /// See if it has the ACPI storage file
      ///
      Size      = 0;
      FvStatus  = 0;
      Status = FwVol->ReadFile (
                      FwVol,
                      &gDptfAcpiTableStorageGuidFfrd,
                      NULL,
                      &Size,
                      &FileType,
                      &Attributes,
                      &FvStatus
                      );

      ///
      /// If we found it, then we are done
      ///
      if (Status == EFI_SUCCESS) {
        break;
      }
    }
  }

  ///
  /// Our exit status is determined by the success of the previous operations
  /// If the protocol was found, Instance already points to it.
  ///
  /// Free any allocated buffers
  ///
  FreePool (HandleBuffer);

  ///
  /// Sanity check that we found our data file
  ///
  ASSERT (FwVol);
  if (FwVol == NULL) {
    return;
  }

  ///
  /// By default, a table belongs in all ACPI table versions published.
  ///
  Version = EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0;

  ///
  /// Find the Table protocol
  ///
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **) &AcpiTable);

  ///
  /// Read tables from the storage file.
  ///
  Instance = 0;

  while (Status == EFI_SUCCESS) {
    ///
    /// Read the ACPI tables
    ///
    Status = FwVol->ReadSection (
                          FwVol,
                          &gDptfAcpiTableStorageGuidFfrd,
                          EFI_SECTION_RAW,
                          Instance,
                          (VOID **) &Table,
                          &Size,
                          &FvStatus
                          );

    if (!EFI_ERROR (Status)) {

      LoadTable = FALSE;
      TableHeader = (EFI_ACPI_DESCRIPTION_HEADER *) Table;

      switch (((EFI_ACPI_DESCRIPTION_HEADER*) TableHeader)->OemTableId) {

      case SIGNATURE_64 ('D', 'p', 't', 'f', 'F', 'f', 'r', 'd'):
        ///
        /// This is Dptf SSDT. Dptf should be enabled if we reach here so load the table.
        ///
        LoadTable = TRUE;
        DEBUG ((EFI_D_INFO, "Found Dptf SSDT signature."));
        break;

      default:
        break;
      }

      ///
      /// Add the table
      ///
      if (LoadTable) {
        TableHandle = 0;
        Status = AcpiTable->InstallAcpiTable (
                                  AcpiTable,
                                  Table,
                                  Table->Length,
                                  &TableHandle
                                  );
      }

      ///
      /// Increment the instance
      ///
      Instance++;
      Table = NULL;
    }
  }
}
