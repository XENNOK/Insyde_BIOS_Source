/** @file
  Install ACPI MSDM Table For OA 3.0

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
#include <PiDxe.h>
#include <Protocol/AcpiTable.h>
#include "AcpiHeaderDefaultValue.h"
#include <Library/BaseMemoryLib.h>
#include <IndustryStandard/Oa3_0.h>

#include <Guid/EventGroup.h>

#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DxeOemSvcKernelLib.h>

static  EFI_GUID                gMsdmAddressGuid = MEMORY_ADDRESS_FOR_MSDM_GUID;

EFI_ACPI_MICROSOFT_DIGITAL_MARKER_TABLE    mMsdmTable = {
  {
    EFI_ACPI_MSDM_TABLE_SIGNATURE,
    sizeof (EFI_ACPI_MICROSOFT_DIGITAL_MARKER_TABLE),
    EFI_ACPI_MSDM_TABLE_REVISION,

    //
    // Checksum will be updated at runtime
    //
    0x00,

    //
    // It is expected that these values will be programmed at runtime
    //
    EFI_ACPI_OEM_ID,                    //'I', 'N', 'S', 'Y', 'D', 'E'
    EFI_ACPI_OEM_TABLE_ID,              //'C','a','l','p','e','l','l','a'

    EFI_ACPI_OEM_REVISION,
    EFI_ACPI_CREATOR_ID,                //'M','S','D','M'
    EFI_ACPI_CREATOR_REVISION           //1000000,// ASL compiler revision number
  },

    //
    // Msdm Data fields
    //
  {
    0
  }
};


/**
  Allocate memory space for MSDM ACPI table

  @param None

  @retval EFI_SUCCESS               Memory allocated successfully
  @return others                    Failed to allocate memory for MSDM ACPI table

**/
EFI_STATUS
AllocateMemoryForMsdm (
  VOID
  )
{
  EFI_STATUS                                Status;
  EFI_PHYSICAL_ADDRESS                      MsdmAddress;
  UINTN                                     PagesNum;
  UINT8                                     *MsdmAddressData;
  EFI_ACPI_MICROSOFT_DIGITAL_MARKER_TABLE   *MsdmTable;

  MsdmAddress = 0xFFFFFFFF;
  PagesNum = EFI_SIZE_TO_PAGES (sizeof (mMsdmTable));
  Status = gBS->AllocatePages(
                  AllocateMaxAddress,
                  EfiACPIReclaimMemory,
                  PagesNum,
                  &MsdmAddress
                  );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  MsdmTable = (EFI_ACPI_MICROSOFT_DIGITAL_MARKER_TABLE *)(UINTN)MsdmAddress;
  CopyMem (MsdmTable, &mMsdmTable, sizeof (mMsdmTable));

  gBS->AllocatePool (EfiBootServicesData, sizeof (MsdmAddress), (VOID **)&MsdmAddressData);
  CopyMem (MsdmAddressData, &MsdmAddress, sizeof (MsdmAddress));

  Status = gRT->SetVariable (
                  L"MsdmAddress",
                  &gMsdmAddressGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  sizeof (MsdmAddress),
                  MsdmAddressData
                  );

  gBS->FreePool (MsdmAddressData);

  if (EFI_ERROR(Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

/**
  The InstallMsdm Driver Entry Point.

  The function is the driver Entry point which will produce MSDM ACPI table.

  @param ImageHandle   A handle for the image that is initializing this driver
  @param SystemTable   A pointer to the EFI system table

  @retval EFI_SUCCESS              Driver initialized successfully
  @retval EFI_ABORTED              Failed to locate EFI_ACPI_TABLE_PROTOCOL
  @return others                   Failed to install driver

**/
EFI_STATUS
EFIAPI
InstallMsdm (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  UINTN                                      TableHandle;
  EFI_STATUS                                 Status;
  EFI_ACPI_TABLE_PROTOCOL                    *AcpiTable;
  EFI_ACPI_MSDM_DATA_STRUCTURE               *MsdmDataInRom;
  EFI_ACPI_MICROSOFT_DIGITAL_MARKER_TABLE    *MsdmPointer;


  Status = gBS->LocateProtocol (
                  &gEfiAcpiTableProtocolGuid,
                  NULL,
                  (VOID**)&AcpiTable
                  );
  if (EFI_ERROR (Status)) {
    return EFI_ABORTED;
  }
  MsdmPointer = &mMsdmTable;
  if (*(UINT32*)MsdmPointer != EFI_ACPI_MSDM_TABLE_SIGNATURE) {
    return EFI_UNSUPPORTED;
  }

  //
  // OemServices
  //
  MsdmDataInRom = (VOID *)((UINTN)PcdGet32 (PcdFlashNvStorageMsdmDataBase));
  CopyMem (&MsdmPointer->MsdmData, MsdmDataInRom, sizeof (EFI_ACPI_MSDM_DATA_STRUCTURE));

  Status = OemSvcGetOa30MsdmData (
             &MsdmPointer->MsdmData
             );
  if (EFI_ERROR (Status)) {
    if ((MsdmPointer->MsdmData.MsdmVersion    != 0xFFFFFFFF) &&
        (MsdmPointer->MsdmData.MdsmDataType   != 0xFFFFFFFF) &&
        (MsdmPointer->MsdmData.MsdmDataLength != 0xFFFFFFFF)) {
      TableHandle = 0;
      Status = AcpiTable->InstallAcpiTable (
                            AcpiTable,
                            &mMsdmTable,
                            sizeof (EFI_ACPI_MICROSOFT_DIGITAL_MARKER_TABLE),
                            &TableHandle
                            );
      return Status;
    }
  }

  Status = AllocateMemoryForMsdm ();

  return Status;
}

