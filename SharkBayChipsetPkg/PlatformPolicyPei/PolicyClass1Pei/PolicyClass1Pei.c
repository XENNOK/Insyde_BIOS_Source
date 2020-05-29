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

#include <PolicyClass1Pei.h>
//[-start-121126-IB03780468-add]//
#include <Ppi/Wdt.h>
//[-end-121126-IB03780468-add]//

//[-start-121120-IB03780473-add]//
//
// RecoveryPchUsbPolicyPEI
//
static EFI_PEI_NOTIFY_DESCRIPTOR       mRecoveryPchUsbPolicyNotifyDesc[] = {
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_DISPATCH | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiPeiMasterBootModePpiGuid,
    RecoveryPchUsbPolicyCallBack
  }
};

//
// SaPolicyPEI
//
static EFI_PEI_NOTIFY_DESCRIPTOR       mSaPolicyNotifyDesc[] = {
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_DISPATCH | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gPeiSmbusPolicyPpiGuid,
    SaPolicyCallBack
  }
};
//[-end-121120-IB03780473-add]//
//[-start-121126-IB03780468-add]//
//
// CpuPolicyPEI
//
static EFI_PEI_NOTIFY_DESCRIPTOR       mCpuPolicyNotifyDesc[] = {
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_DISPATCH | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gWdtPpiGuid,
    CpuPolicyCallBack
  }
};
//[-end-121126-IB03780468-add]//

EFI_STATUS
EFIAPI
PEIPolicyClass1Entry (
  IN      EFI_PEI_FILE_HANDLE         FileHandle,
  IN      CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS        Status;
//[-start-121120-IB03780473-remove]//
////[-start-120712-IB03780454-add]//
//  EFI_BOOT_MODE     BootMode;
////[-end-120712-IB03780454-add]//
//[-end-121120-IB03780473-remove]//

  Status = EFI_SUCCESS;

  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "\nPEI Policy Class 1 Entry\n"));

  Status = MePolicyPeiEntry (FileHandle, PeiServices);
  ASSERT_EFI_ERROR (Status);

//[-start-121120-IB03780473-add]//
//[-start-130125-IB11410046-modify]//
  if (FeaturePcdGet (PcdMe5MbSupported)) {
    Status = AmtPolicyPeiEntry (FileHandle, PeiServices);
    ASSERT_EFI_ERROR (Status);
  }
//[-end-130125-IB11410046-modify]//

  Status = AzaliaPolicyPeiEntry (FileHandle, PeiServices);
  ASSERT_EFI_ERROR (Status);

  Status = PchPolicyPeiEntry (FileHandle, PeiServices);
  ASSERT_EFI_ERROR (Status);

  Status = StallPeiEntry (FileHandle, PeiServices);
  ASSERT_EFI_ERROR (Status);

//[-start-121126-IB03780468-remove]//
//  Status = CpuPolicyPeiEntry (FileHandle, PeiServices);
//  ASSERT_EFI_ERROR (Status);
//[-end-121126-IB03780468-remove]//
//[-end-121120-IB03780473-add]//

//[-start-121120-IB03780473-remove]//
////[-start-120712-IB03780454-add]//
//  Status = PeiServicesGetBootMode (&BootMode);
//  ASSERT_EFI_ERROR (Status);
////[-end-120712-IB03780454-add]//
//[-end-121120-IB03780473-remove]//

//[-start-121126-IB03780468-modify]//
//[-start-121120-IB03780473-modify]//
//[-start-140804-IB14090001-modify]//
  if (FeaturePcdGet (PcdH2OUsbPeiSupported)) {
    Status = PeiServicesNotifyPpi (mRecoveryPchUsbPolicyNotifyDesc);
    ASSERT_EFI_ERROR (Status);
  }
//[-end-140804-IB14090001-modify]//
  
  Status = PeiServicesNotifyPpi (mSaPolicyNotifyDesc);
  ASSERT_EFI_ERROR (Status);

  Status = PeiServicesNotifyPpi (mCpuPolicyNotifyDesc);
  ASSERT_EFI_ERROR (Status);
//[-end-121120-IB03780473-modify]//
//[-end-121126-IB03780468-modify]//

//[-start-120528-IB07250113-remove]//
//#ifdef SWITCHABLE_GRAPHICS_SUPPORT
//  Status = SgPeiPolicyInitEntryPoint ( FileHandle, PeiServices );
//  ASSERT_EFI_ERROR ( Status );
//#endif
//[-end-120528-IB07250113-remove]//

  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "PEI Policy Class 1 Exit\n"));

  return Status;
}
