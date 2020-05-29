/** @file

  This code makes the BIOS Data structure available via standard ACPI mechanisms.

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "BdatAccessHandler.h"

STATIC EFI_ACPI_TABLE_PROTOCOL  *mAcpiTable;

/**
  Entry point of the Bdat Access Handler.

  @param[in] ImageHandle   EFI_HANDLE: A handle for the image that is initializing this driver
  @param[in] SystemTable   EFI_SYSTEM_TABLE: A pointer to the EFI system table

  @retval EFI_SUCCESS:              Driver initialized successfully
  @exception EFI_UNSUPPORTED:          A needed driver was not located
  @retval EFI_OUT_OF_RESOURCES:     Could not allocate needed resources

**/
EFI_STATUS
EFIAPI
BdatAccessHandler (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS                      Status;
  VOID                            *HobList;
  DXE_PLATFORM_SA_POLICY_PROTOCOL *DxePlatformSaPolicy;

  ///
  /// Check if the BDAT ACPI table support is enabled in Setup, if not then exit.
  /// Get the platform setup policy.
  ///
  Status = gBS->LocateProtocol (&gDxePlatformSaPolicyGuid, NULL, (VOID **) &DxePlatformSaPolicy);
  ASSERT_EFI_ERROR (Status);

  ///
  /// Get the start of the HOBs.
  ///
  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, (VOID **)&HobList);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }
  ///
  /// Locate ACPI table protocol
  ///
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL,(VOID **) &mAcpiTable);
  ASSERT_EFI_ERROR (Status);
  ASSERT (mAcpiTable != NULL);
  if (EFI_ERROR (Status)) {
    return EFI_OUT_OF_RESOURCES;
  }
  BdatRmtHandler (DxePlatformSaPolicy, HobList, mAcpiTable);
  ///
  /// ASSERT_EFI_ERROR (Status);
  ///
  return Status;
}
