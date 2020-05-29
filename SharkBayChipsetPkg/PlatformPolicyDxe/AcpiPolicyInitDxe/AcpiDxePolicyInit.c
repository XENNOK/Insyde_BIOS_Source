//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//; ModuleName:
//;
//;   AcpiDxePolicyInit.c
//;
//;------------------------------------------------------------------------------
//;
//; Abstract:
//;
//;   This file is a wrapper for ACPI Platform Policy driver.
//;   Get Setup Value to initilize Intel DXE Platform Policy.
//;

#include <AcpiDxePolicyInit.h>

ACPI_PLATFORM_POLICY_PROTOCOL mAcpiPlatformPolicyProtocol;
EFI_ACPI_INFO_PROTOCOL      mAcpiInfoProtocol;

EFI_STATUS
EFIAPI
AcpiDxePolicyInitEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
/**
 
@brief
 
  Initialize ACPI DXE Platform Policy
  
  @param[in] ImageHandle       Image handle of this driver.
  @param[in] SystemTable       Global system service table.

  @retval EFI_SUCCESS           Initialization complete.
  @exception EFI_UNSUPPORTED       The chipset is unsupported by this driver.
  @retval EFI_OUT_OF_RESOURCES  Do not have enough resources to initialize the driver.
  @retval EFI_DEVICE_ERROR      Device error, driver exits abnormally.

**/
{
  EFI_STATUS                   Status;
  EFI_SETUP_UTILITY_PROTOCOL   *SetupUtility;
  CHIPSET_CONFIGURATION         *SetupVariable;
  
  Status = gBS->LocateProtocol (&gEfiSetupUtilityProtocolGuid, NULL, (VOID **)&SetupUtility);
  ASSERT_EFI_ERROR (Status);
  
  SetupVariable = (CHIPSET_CONFIGURATION *) (SetupUtility->SetupNvData);
  ///
  /// Initialize the EFI Driver Library
  ///
  ZeroMem (&mAcpiPlatformPolicyProtocol, sizeof (ACPI_PLATFORM_POLICY_PROTOCOL));
  mAcpiPlatformPolicyProtocol.Revision              = ACPI_PLATFORM_POLICY_PROTOCOL_REVISION_1;
  mAcpiPlatformPolicyProtocol.BoardId               = 0;

  mAcpiPlatformPolicyProtocol.EnableDptf            = SetupVariable->EnableDptf;
  mAcpiPlatformPolicyProtocol.EnableCppc            = SetupVariable->EnableCppc;
  mAcpiPlatformPolicyProtocol.EnableCppcPlatformSCI = SetupVariable->EnableCppcPlatformSCI;

//[-start-121219-IB10820205-modify]//
//[-start-130524-IB05160451-modify]//
  Status = OemSvcUpdateAcpiPlatformPolicy (&mAcpiPlatformPolicyProtocol);
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "DexOemSvcChipsetLib OemSvcUpdateAcpiPlatformPolicy, Status : %r\n", Status));
//[-end-130524-IB05160451-modify]//
  if (Status != EFI_SUCCESS) {
    Status = gBS->InstallMultipleProtocolInterfaces (
                    &ImageHandle,
                    &gAcpiPlatformPolicyProtocolGuid,
                    &mAcpiPlatformPolicyProtocol,
                    NULL
                    );
    ASSERT_EFI_ERROR (Status);
  }
//[-end-121219-IB10820205-modify]//
  ///
  /// Install ACPI_INFO_PROTOCOL
  ///
  ZeroMem (&mAcpiInfoProtocol, sizeof (ACPI_PLATFORM_POLICY_PROTOCOL));
  mAcpiInfoProtocol.Revision   = ACPI_INFO_PROTOCOL_REVISION_1;
  ///
  /// RCVersion[32:0] is the release number.
  /// For example: 
  /// Acpi Framework 0.5.0 should be 00 05 00 00 (0x00050000)
  ///
  mAcpiInfoProtocol.RCVersion  = ACPI_RC_VERSION;

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gEfiAcpiInfoProtocolGuid,
                  &mAcpiInfoProtocol,
                  NULL
                  );

  return Status;
}
