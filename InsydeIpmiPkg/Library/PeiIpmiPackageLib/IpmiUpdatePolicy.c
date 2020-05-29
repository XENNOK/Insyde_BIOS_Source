/** @file
 PEI IPMI Package library implement code - Update Policy.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/


#include <Library/PeiIpmiPackageLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PcdLib.h>

#include <Ppi/ReadOnlyVariable2.h>

#include <SetupConfig.h>


/**
 Update IPMI policy according to the "Setup" variable SYSTEM_CONFIGURATION struct data.

 @param[in]         PeiServices         A pointer to EFI_PEI_SERVICES struct pointer.

 @retval EFI_SUCCESS                    Update Policy success.
 @return EFI_ERROR (Status)             Locate gEfiPeiReadOnlyVariable2PpiGuid or GetVariable error.
 @retval EFI_UNSUPPORTED                Platform does not implement this function when IPMI Config in Device Manager.
*/
EFI_STATUS
IpmiUpdatePolicy (
  IN CONST EFI_PEI_SERVICES             **PeiServices
  )
{
  EFI_STATUS                            Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI       *VariablePpi;
  UINTN                                 Size;
  SYSTEM_CONFIGURATION                  SetupConfig;

  Status = (*PeiServices)->LocatePpi (PeiServices, &gEfiPeiReadOnlyVariable2PpiGuid, 0, NULL, (VOID **)&VariablePpi);
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  if (FeaturePcdGet (PcdIpmiConfigInDeviceManager) == FALSE) {

    Size = sizeof (SYSTEM_CONFIGURATION);
    Status = VariablePpi->GetVariable (
                            VariablePpi,
                            SETUP_VARIABLE_NAME,
                            &gSystemConfigurationGuid,
                            NULL,
                            &Size,
                            &SetupConfig
                            );
    if (EFI_ERROR (Status)) {
      return EFI_NOT_FOUND;
    }

    if (SetupConfig.SetupVariableInvalid == 0) {
      PcdSet8 (PcdIpmiEnable, SetupConfig.IpmiEnable);
      PcdSet8 (PcdBmcWarmupTime, SetupConfig.BmcWarmupTime);
      PcdSet8 (PcdBmcWdtEnable, SetupConfig.BmcWdtEnable);
      PcdSet8 (PcdBmcWdtAction, SetupConfig.BmcWdtAction);
      PcdSet8 (PcdBmcWdtTimeout, SetupConfig.BmcWdtTimeout);
      PcdSet8 (PcdIpmiSpmiEnable, SetupConfig.IpmiSpmiEnable);
      PcdSet8 (PcdIpmiSetBiosVersion, SetupConfig.IpmiSetBiosVersion);
      PcdSet8 (PcdIpmiBootOption, SetupConfig.IpmiBootOption);
    }

  } else {

    //
    // There is no need to update policy when IPMI Config in Device Manager.
    //

    return EFI_UNSUPPORTED;

  }

  return EFI_SUCCESS;

}

