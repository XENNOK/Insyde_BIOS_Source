/** @file

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

#include <OemAcpiPlatform.h>

EFI_ACPI_SUPPORT_PROTOCOL                 *mAcpiSupport = NULL;
CHIPSET_CONFIGURATION                      *mSetupNvData = NULL;

//[-end-120308-IB10820020-add]//

/**

  OEM ACPI Platform driver installation function.

  @param     ImageHandle     EFI_HANDLE
  @param     SystemTable     EFI_SYSTEM_TABLE pointer

  @retval    EFI_SUCCESS    The driver installed without error.
             EFI_ABORTED    The driver encountered an error and could not complete installation of
                            the ACPI tables.

**/
EFI_STATUS
EFIAPI
InstallOemAcpiPlatform (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                                Status;
  EFI_SETUP_UTILITY_PROTOCOL                *EfiSetupUtility;

  Status = gBS->LocateProtocol (&gEfiSetupUtilityProtocolGuid, NULL, (VOID **)&EfiSetupUtility);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  mSetupNvData = (CHIPSET_CONFIGURATION*) EfiSetupUtility->SetupNvData;

  //
  // Locate ACPI support protocol
  //
  Status = gBS->LocateProtocol (&gEfiAcpiSupportProtocolGuid, NULL, (VOID **)&mAcpiSupport);
  ASSERT_EFI_ERROR (Status);
//[-start-121107-IB10820153-modify]//
//[-start-121219-IB10820204-remove]//
//  Status = UpdateOemGlobalNvs ();
//  DEBUG ( ( EFI_D_ERROR | EFI_D_INFO, "DxeOemSvcChipsetLib UpdateOemGlobalNvs, Status : %r\n", Status ) );
//[-end-121219-IB10820204-remove]//
//[-end-121107-IB10820153-modify]//

  Status = OemUpdateOemTableID ();
  if (EFI_ERROR(Status)) {
    DEBUG ( ( EFI_D_ERROR, "OEM Update OEM Table ID failed, Status : %r\n", Status ) );
  }
  return Status;
}
