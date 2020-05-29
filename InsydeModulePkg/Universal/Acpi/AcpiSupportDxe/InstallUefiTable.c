/** @file
  Install UEFI ACPI table which contains BVDT(BIOS Version Date and Time) data.

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

#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <AcpiHeaderDefaultValue.h>
#include "InstallUefiTable.h"

EFI_ACPI_UEFI_TABLE mUefiTable = {
  {
    {
      UEFI_ACPI_TABLE_SIGNATURE,
      sizeof (EFI_ACPI_UEFI_TABLE),
      1,
      //
      // Checksum will be updated at runtime
      //
      0x00,

      EFI_ACPI_OEM_ID,
      EFI_ACPI_OEM_TABLE_ID,
      EFI_ACPI_OEM_REVISION,
      EFI_ACPI_CREATOR_ID,
      EFI_ACPI_CREATOR_REVISION
    },
    UEFI_ACPI_TABLE_INSYDE_BIOS_GUID,
    0
  },
    
  //
  // BVDT Data fields
  //
  {
    0
  }
};

/**
 Install UEFI ACPI table which contains BVDT data.

 @param [in]   AcpiSupport      Instance of the protocol.
 @param [in]   AcpiVersion      ACPI version to set.

 @retval EFI_SUCCESS            Install successfully.
 @retval EFI_INVALID_PARAMETER  Invalid input parameter.
 @return others                 Fail to install UEFI ACPI table.

**/
EFI_STATUS
InstallUefiTable (
  IN EFI_ACPI_SUPPORT_PROTOCOL   *AcpiSupport,
  IN UINT32                      AcpiVersion
  )
{
  EFI_STATUS                        Status;
  UINTN                             TableHandle;

  if (AcpiSupport == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  mUefiTable.UefiAcpiDataTable.DataOffset = (UINT16) ((UINTN) mUefiTable.Data - (UINTN) &mUefiTable);
  CopyMem (
    mUefiTable.Data,
    (VOID *) (UINTN)BVDT_BASE_ADDRESS,
    (UINTN) BVDT_SIZE
    );
  
  TableHandle = 0;
  Status = AcpiSupport->SetAcpiTable (
                          AcpiSupport,
                          &mUefiTable,
                          TRUE,
                          AcpiVersion,
                          &TableHandle
                          );
  if (!EFI_ERROR (Status)) {
    Status = AcpiSupport->PublishTables (AcpiSupport, AcpiVersion);
  }
  
  ASSERT_EFI_ERROR (Status);
  return Status;
}
