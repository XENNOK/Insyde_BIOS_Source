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


#include <AmtPolicyPei.h>


EFI_STATUS
AmtPolicyPeiEntry (
  IN EFI_PEI_FILE_HANDLE             FileHandle,
  IN      CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                         Status;
//[-start-130709-IB05160465-modify]//
  VOID                               *SystemConfiguration;
//[-end-130709-IB05160465-modify]//
  EFI_PEI_READ_ONLY_VARIABLE2_PPI    *Variable;
  UINTN                              VariableSize;
  EFI_GUID                           SystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;
  EFI_PEI_PPI_DESCRIPTOR             *AMTPlatformPolicyPpiDesc;
  PEI_AMT_PLATFORM_POLICY_PPI        *AMTPlatformPolicyPpi;
  UINT8                              ManageabilityMode;

  Variable                 = NULL;
  VariableSize             = 0;
  AMTPlatformPolicyPpiDesc = NULL;
  AMTPlatformPolicyPpi     = NULL;
  ManageabilityMode        = 0;

  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "\nAMT Policy PEI Entry\n"));

  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **)&Variable
             );
  ASSERT_EFI_ERROR (Status);

//[-start-130709-IB05160465-modify]//
  VariableSize = PcdGet32 (PcdSetupConfigSize);
  SystemConfiguration = AllocatePool (VariableSize);
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
  ASSERT_EFI_ERROR (Status);
//[-start-140625-IB05080432-add]//
  if (EFI_ERROR(Status)) {
    return Status;
  }
//[-end-140625-IB05080432-add]//
//[-end-130709-IB05160465-modify]//

  //
  // Allocate memory for AMT Policy PPI and Descriptor
  //
  AMTPlatformPolicyPpi = (PEI_AMT_PLATFORM_POLICY_PPI *)AllocateZeroPool (sizeof (PEI_AMT_PLATFORM_POLICY_PPI));
  ASSERT (AMTPlatformPolicyPpi != NULL);
//[-start-130104-IB10870064-add]//
  if (AMTPlatformPolicyPpi == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130104-IB10870064-add]//
  AMTPlatformPolicyPpiDesc = (EFI_PEI_PPI_DESCRIPTOR *)AllocateZeroPool (sizeof (EFI_PEI_PPI_DESCRIPTOR));
  ASSERT (AMTPlatformPolicyPpiDesc != NULL);
//[-start-130104-IB10870064-add]//
  if (AMTPlatformPolicyPpiDesc == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130104-IB10870064-add]//
  
  //
  // Update REVISION number
  //
//[-start-120917-IB06150246-modify]//
  AMTPlatformPolicyPpi->Revision          = PEI_AMT_PLATFORM_POLICY_PPI_REVISION_5;
//[-end-120917-IB06150246-modify]//

  //
  // Initialize Platform Configuration
  //
//[-start-130709-IB05160465-modify]//
  AMTPlatformPolicyPpi->iAmtEnabled       = ((CHIPSET_CONFIGURATION *)SystemConfiguration)->Amt;
  AMTPlatformPolicyPpi->WatchDog          = ((CHIPSET_CONFIGURATION *)SystemConfiguration)->WatchDog;
  AMTPlatformPolicyPpi->AsfEnabled        = ((CHIPSET_CONFIGURATION *)SystemConfiguration)->Asf;
  AMTPlatformPolicyPpi->ManageabilityMode = PcdGet8 (PcdAMTPlatformPolicyPpiManageabilityMode);
  AMTPlatformPolicyPpi->WatchDogTimerOs   = ((CHIPSET_CONFIGURATION *)SystemConfiguration)->WatchDogTimerOs;
  AMTPlatformPolicyPpi->WatchDogTimerBios = ((CHIPSET_CONFIGURATION *)SystemConfiguration)->WatchDogTimerBios;
  AMTPlatformPolicyPpi->FWProgress        = ((CHIPSET_CONFIGURATION *)SystemConfiguration)->FWProgress;
//[-end-130709-IB05160465-modify]//

  //
  // Update AMT Policy PPI Descriptor
  //
  AMTPlatformPolicyPpiDesc->Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  AMTPlatformPolicyPpiDesc->Guid  = &gPeiAmtPlatformPolicyPpiGuid;
  AMTPlatformPolicyPpiDesc->Ppi   = AMTPlatformPolicyPpi;

//[-start-121224-IB10870061-add]//
//[-start-130524-IB05160451-modify]//
  Status = OemSvcUpdatePeiAmtPlatformPolicy (AMTPlatformPolicyPpi, AMTPlatformPolicyPpiDesc);
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "PeiOemSvcChipsetLib OemSvcUpdatePeiAmtPlatformPolicy, Status : %r\n", Status));
//[-end-130524-IB05160451-modify]//
  if (Status != EFI_SUCCESS) {  
    DEBUG ((EFI_D_ERROR | EFI_D_INFO, "Installing AMT Policy ( PEI ).\n"));
    //
    // Install AMT PEI Platform Policy PPI
    //
    Status = PeiServicesInstallPpi (AMTPlatformPolicyPpiDesc);
    ASSERT_EFI_ERROR (Status);
  }
//[-end-121224-IB10870061-add]//

  //
  // Dump policy
  //
  DumpAMTPolicyPEI (PeiServices, AMTPlatformPolicyPpi);

  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "AMT Policy PEI Exit\n"));

//[-start-130709-IB05160465-add]//
  FreePool (SystemConfiguration);
//[-end-130709-IB05160465-add]//

  return Status;
}

VOID
DumpAMTPolicyPEI (
  IN      CONST EFI_PEI_SERVICES             **PeiServices,
  IN      PEI_AMT_PLATFORM_POLICY_PPI        *AMTPlatformPolicyPpi
  )
{
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "\n"));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "AMTPlatformPolicyPpi ( Address : 0x%x )\n", AMTPlatformPolicyPpi));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-Revision          : %x\n", AMTPlatformPolicyPpi->Revision));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-iAmtEnabled       : %x\n", AMTPlatformPolicyPpi->iAmtEnabled));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-WatchDog          : %x\n", AMTPlatformPolicyPpi->WatchDog));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-Reserved          : %x\n", AMTPlatformPolicyPpi->Reserved));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-AsfEnabled        : %x\n", AMTPlatformPolicyPpi->AsfEnabled));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-ManageabilityMode : %x\n", AMTPlatformPolicyPpi->ManageabilityMode));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-WatchDogTimerOs   : %x\n", AMTPlatformPolicyPpi->WatchDogTimerOs));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-WatchDogTimerBios : %x\n", AMTPlatformPolicyPpi->WatchDogTimerBios));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-FWProgress        : %x\n", AMTPlatformPolicyPpi->FWProgress));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "\n"));

  return;
}
