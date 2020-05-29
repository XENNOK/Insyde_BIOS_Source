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

#include <MePolicyPei.h>

EFI_STATUS
MePolicyPeiEntry (
  IN      EFI_FFS_FILE_HEADER        *FfsHeader,
  IN      CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                        Status;
  EFI_PEI_PPI_DESCRIPTOR            *MEPlatformPolicyPpiDesc;
  EFI_PEI_PPI_DESCRIPTOR            *MEPlatformMeHookPpiDesc;
  PEI_ME_PLATFORM_POLICY_PPI        *MEPlatformPolicyPpi;
  PLATFORM_ME_HOOK_PPI              *MEPlatformMeHookPpi;
//[-start-121011-IB06150251-add]//
//[-start-130710-IB05160465-modify]//
  VOID                              *SystemConfiguration;
//[-end-130710-IB05160465-modify]//
  EFI_PEI_READ_ONLY_VARIABLE2_PPI   *Variable;
  UINTN                             VariableSize;
  EFI_GUID                          SystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;
//[-end-121011-IB06150251-add]//

  MEPlatformPolicyPpiDesc = NULL;
  MEPlatformPolicyPpi     = NULL;
//[-start-121011-IB06150251-add]//
  Variable                = NULL;
  VariableSize            = 0;
//[-end-121011-IB06150251-add]//

  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "\nME Policy PEI Entry\n"));

//[-start-121011-IB06150251-add]//
  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **)&Variable
             );
  ASSERT_EFI_ERROR (Status);


//[-start-130710-IB05160465-modify]//
  VariableSize = PcdGet32 (PcdSetupConfigSize);
  SystemConfiguration = AllocateZeroPool (VariableSize);
//[-start-140625-IB05080432-add]//
  if (SystemConfiguration == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-140625-IB05080432-add]//

  Status = Variable->GetVariable (
             Variable,
             L"Setup",
             &SystemConfigurationGuid,
             NULL,
             &VariableSize,
             SystemConfiguration
             );
//[-start-140625-IB05080432-modify]//
  if (EFI_ERROR(Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }
//[-end-140625-IB05080432-modify]//
//[-end-130710-IB05160465-modify]//
  
//[-end-121011-IB06150251-add]//
  //
  // Allocate memory for ME Policy PPI and Descriptor
  //
  MEPlatformPolicyPpi = (PEI_ME_PLATFORM_POLICY_PPI *)AllocateZeroPool (sizeof (PEI_ME_PLATFORM_POLICY_PPI));
  ASSERT (MEPlatformPolicyPpi != NULL);
//[-start-130104-IB10870064-add]//
  if (MEPlatformPolicyPpi == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130104-IB10870064-add]//

  MEPlatformPolicyPpiDesc = (EFI_PEI_PPI_DESCRIPTOR *)AllocateZeroPool (sizeof (EFI_PEI_PPI_DESCRIPTOR));
  ASSERT (MEPlatformPolicyPpiDesc != NULL);
//[-start-130104-IB10870064-add]//
  if (MEPlatformPolicyPpiDesc == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130104-IB10870064-add]//

  MEPlatformMeHookPpi = (PLATFORM_ME_HOOK_PPI *)AllocateZeroPool (sizeof (PLATFORM_ME_HOOK_PPI));
  ASSERT (MEPlatformMeHookPpi != NULL);
//[-start-130104-IB10870064-add]//
  if (MEPlatformMeHookPpi == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130104-IB10870064-add]//

  MEPlatformMeHookPpiDesc = (EFI_PEI_PPI_DESCRIPTOR *)AllocateZeroPool (sizeof (EFI_PEI_PPI_DESCRIPTOR));
  ASSERT (MEPlatformMeHookPpiDesc != NULL);
//[-start-130104-IB10870064-add]//
  if (MEPlatformMeHookPpiDesc == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130104-IB10870064-add]//
  //
  // Update REVISION number
  //
  MEPlatformPolicyPpi->Revision = PEI_ME_PLATFORM_POLICY_PPI_REVISION_2;
  
//[-start-121011-IB06150251-add]//
//[-start-130710-IB05160465-modify]//
  MEPlatformPolicyPpi->FTpmSwitch = ((CHIPSET_CONFIGURATION *)SystemConfiguration)->FTpmSwitch;
//[-end-130710-IB05160465-modify]//
//[-end-121011-IB06150251-add]//
//[-start-121224-IB10870062-modify]//
  //
  // Update ME Policy PPI Descriptor
  //
  MEPlatformPolicyPpiDesc->Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  MEPlatformPolicyPpiDesc->Guid  = &gPeiMePlatformPolicyPpiGuid;
  MEPlatformPolicyPpiDesc->Ppi   = MEPlatformPolicyPpi;
//[-start-130524-IB05160451-modify]//
  Status = OemSvcUpdatePeiMePlatformPolicy (MEPlatformPolicyPpi, MEPlatformPolicyPpiDesc);
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "PeiOemSvcChipsetLib OemSvcUpdatePeiMePlatformPolicy, Status : %r\n", Status));
//[-end-130524-IB05160451-modify]//
  if (Status != EFI_SUCCESS) {  
    DEBUG ((EFI_D_ERROR | EFI_D_INFO, "Installing ME Policy ( PEI ).\n"));
    //
    // Install ME PEI Platform Policy PPI
    //
    Status = PeiServicesInstallPpi (MEPlatformPolicyPpiDesc);
    ASSERT_EFI_ERROR (Status);
  }
//[-end-121224-IB10870062-modify]//

  //
  // Install ME PEI Platform Me Hook PPI
  //
  MEPlatformMeHookPpi->Revision  = PLATFORM_ME_HOOK_PPI_REVISION;
//[-start-121004-IB10820133-modify]//
  MEPlatformMeHookPpi->PreGlobalReset = PreGlobalResetPeiHook;
//[-end-121004-IB10820133-modify]//
//[-start-121224-IB10870062-modify]//
  MEPlatformMeHookPpiDesc->Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  MEPlatformMeHookPpiDesc->Guid  = &gPlatformMeHookPpiGuid;
  MEPlatformMeHookPpiDesc->Ppi   = MEPlatformMeHookPpi;
//[-start-130524-IB05160451-modify]//
  Status = OemSvcUpdatePlatformMeHook (MEPlatformMeHookPpi, MEPlatformMeHookPpiDesc);
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "PeiOemSvcChipsetLib OemSvcUpdatePlatformMeHook, Status : %r\n", Status));
//[-end-130524-IB05160451-modify]//
  if (Status != EFI_SUCCESS) {  
    Status = PeiServicesInstallPpi (MEPlatformMeHookPpiDesc);
    ASSERT_EFI_ERROR (Status);
  }
//[-end-121224-IB10870062-modify]//

  //
  // Dump policy
  //
  DumpMEPolicyPEI (PeiServices, MEPlatformPolicyPpi);

  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "ME Policy PEI Exit\n"));

  return Status;
}

VOID
DumpMEPolicyPEI (
  IN      CONST EFI_PEI_SERVICES            **PeiServices,
  IN      PEI_ME_PLATFORM_POLICY_PPI        *MEPlatformPolicyPpi
  )
{
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "\n"));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "MEPlatformPolicyPpi ( Address : 0x%x )\n", MEPlatformPolicyPpi));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-Revision  : %x\n", MEPlatformPolicyPpi->Revision));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-Reserved1 : %x\n", MEPlatformPolicyPpi->Reserved1));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-Reserved2 : %x\n", MEPlatformPolicyPpi->Reserved2));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-Reserved  : %x\n", MEPlatformPolicyPpi->Reserved));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "\n"));

  return;
}

//[-start-121109-IB10820156-modify]//
EFI_STATUS
PreGlobalResetPeiHook (
  IN      CONST EFI_PEI_SERVICES       **PeiServices,
  IN      PLATFORM_ME_HOOK_PPI         * This
  )
{
//[-start-121219-IB10870058-remove]//
//  EFI_STATUS                        Status;
//
//  Status = PlatformMeHookPpiPreGlobalReset();
//  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "PeiOemSvcChipsetLib PlatformMeHookPpiPreGlobalReset, Status : %r\n", Status));
//[-end-121219-IB10870058-remove]//
  return EFI_SUCCESS;
}
//[-end-121109-IB10820156-modify]//
