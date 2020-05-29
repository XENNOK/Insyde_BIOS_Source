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

#include <AzaliaPolicyPei.h>


EFI_STATUS
AzaliaPolicyPeiEntry (
  IN      EFI_PEI_FILE_HANDLE         FileHandle,
  IN      CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                        Status;
  VOID                              *SystemConfiguration;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI   *Variable;
  UINTN                             VariableSize;
  EFI_GUID                          SystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;
  EFI_PEI_PPI_DESCRIPTOR            *PchAzaliaPolicyPpiDesc;
  AZALIA_POLICY                     *PchAzaliaPolicyPpi;
  UINT8                             Index;
//[-start-121004-IB10820133-add]//
  UINT8                             CurrentAzaliaDockingAttachStatus;
//[-end-121004-IB10820133-add]//

  Variable                = NULL;
  VariableSize            = 0;
  PchAzaliaPolicyPpiDesc  = NULL;
  PchAzaliaPolicyPpi      = NULL;
  Index                   = 0;
//[-start-121004-IB10820133-add]//
  CurrentAzaliaDockingAttachStatus = 0;
//[-end-121004-IB10820133-add]//

  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "\nAzalia Policy PEI Entry\n"));

  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **)&Variable
             );
  ASSERT_EFI_ERROR (Status);

//[-start-130709-IB05160465-modify]//
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
  ASSERT_EFI_ERROR (Status);
//[-start-140625-IB05080432-add]//
  if (EFI_ERROR(Status)) {
    return Status;
  }
//[-end-140625-IB05080432-add]//
//[-end-130709-IB05160465-modify]//

  //
  // Allocate memory for Azalia Policy PPI and Descriptor
  //
  PchAzaliaPolicyPpiDesc = (EFI_PEI_PPI_DESCRIPTOR *)AllocateZeroPool (sizeof (EFI_PEI_PPI_DESCRIPTOR));
  ASSERT (PchAzaliaPolicyPpiDesc != NULL);
//[-start-130104-IB10870064-add]//
  if (PchAzaliaPolicyPpiDesc == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130104-IB10870064-add]//

  PchAzaliaPolicyPpi = (AZALIA_POLICY *)AllocateZeroPool (sizeof (AZALIA_POLICY));
  ASSERT (PchAzaliaPolicyPpi != NULL);
//[-start-130104-IB10870064-add]//
  if (PchAzaliaPolicyPpi == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130104-IB10870064-add]//

  //
  // Setting Azalia policy
  //
//[-start-130710-IB05160465-modify]//
  PchAzaliaPolicyPpi->Pme                = ((CHIPSET_CONFIGURATION *)SystemConfiguration)->AzaliaPme;   // 0 : Disable ; 1 : Enable
//[-start-120619-IB10540010-modify]//
  PchAzaliaPolicyPpi->DS                 = ((CHIPSET_CONFIGURATION *)SystemConfiguration)->AzaliaDockSupport;   // 0 : Docking is not supported ; 1 : Docking is supported
//[-end-120619-IB10540010-modify]//
//[-end-130710-IB05160465-modify]//
//[-start-121109-IB10820156-modify]//
  PchAzaliaPolicyPpi->DA                 = PcdGet8 (PcdPchAzaliaPolicyPpiDockAttached);   // 0 : Docking is not attached ; 1 : Docking is attached
//[-start-121224-IB10870061-remove]//
//  Status = PchAzaliaPolicyPpiDa (&CurrentAzaliaDockingAttachStatus);
//  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "PeiOemSvcChipsetLib PchAzaliaPolicyPpiDa, Status : %r\n", Status));
//  if (Status == EFI_MEDIA_CHANGED) {
//    PchAzaliaPolicyPpi->DA               = CurrentAzaliaDockingAttachStatus;
//  }
//[-end-121224-IB10870061-remove]//
//[-end-121109-IB10820156-modify]//
//[-start-120606-IB04770232-remove]//
//   PchAzaliaPolicyPpi->HdmiCodec          = SystemConfiguration.AzaliaHdmi;
//[-end-120606-IB04770232-remove]//
  PchAzaliaPolicyPpi->ResetWaitTimer     = PcdGet16 (PcdPchAzaliaPolicyPpiResetWaitTimer);

  //
  // Update Azalia Policy PPI Descriptor
  //
  PchAzaliaPolicyPpiDesc->Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  PchAzaliaPolicyPpiDesc->Guid  = &gAzaliaPolicyGuid;
  PchAzaliaPolicyPpiDesc->Ppi   = PchAzaliaPolicyPpi;

//[-start-121224-IB10870061-add]//
//[-start-130524-IB05160451-modify]//
  Status = OemSvcUpdateAzaliaPolicy (PchAzaliaPolicyPpi, PchAzaliaPolicyPpiDesc);
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "PeiOemSvcChipsetLib OemSvcUpdateAzaliaPolicy, Status : %r\n", Status));
//[-end-130524-IB05160451-modify]//
  if (Status != EFI_SUCCESS) {
    DEBUG ((EFI_D_ERROR | EFI_D_INFO, "Installing Azalia Policy ( PEI ).\n"));
    //
    // Install SA PEI Platform Policy PPI
    //
    Status = PeiServicesInstallPpi (PchAzaliaPolicyPpiDesc);
    ASSERT_EFI_ERROR (Status);
  }
//[-end-121224-IB10870061-add]//

  //
  // Dump policy
  //
  DumpAzaliaPolicyPEI (PeiServices, PchAzaliaPolicyPpi);

  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "Azalia Policy PEI Exit\n"));

  return Status;
}

VOID
DumpAzaliaPolicyPEI (
  IN      CONST EFI_PEI_SERVICES        **PeiServices,
  IN      AZALIA_POLICY                 *PchAzaliaPolicyPpi
  )
{

  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "\n"));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "AzaliaPlatformPolicyPpi ( Address : 0x%x )\n", PchAzaliaPolicyPpi));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-Pme                            : %x\n", PchAzaliaPolicyPpi->Pme));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-DS                             : %x\n", PchAzaliaPolicyPpi->DS));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-DA                             : %x\n", PchAzaliaPolicyPpi->DA));
//[-start-120606-IB04770232-remove]//
//   DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |-HdmiCodec                      : %x\n", PchAzaliaPolicyPpi->HdmiCodec ) );
//[-end-120606-IB04770232-remove]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-Rsvdbits                       : %x\n", PchAzaliaPolicyPpi->Rsvdbits));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-AzaliaVerbTableNum             : %x\n", PchAzaliaPolicyPpi->AzaliaVerbTableNum));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-AzaliaVerbTable                : %x\n", PchAzaliaPolicyPpi->AzaliaVerbTable));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-ResetWaitTimer                 : %x\n", PchAzaliaPolicyPpi->ResetWaitTimer));
  return;
}
