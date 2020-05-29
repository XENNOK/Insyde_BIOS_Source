/**

Copyright (c) 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file
  AtPolicy.c

@brief
  TdtPlatformPolicy to check and set Tdt Platform Policy.

**/
/**
 This file contains an 'Intel Peripheral Driver' and is
 licensed for Intel CPUs and chipsets under the terms of your
 license agreement with Intel or your vendor.  This file may
 be modified by the user, subject to additional terms of the
 license agreement
**/

#include <AtPolicy.h>

DXE_AT_POLICY_PROTOCOL mAtPlatformPolicyInstance  = { 0 };


EFI_STATUS
EFIAPI
AtPolicyInitEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
)
/**

  @brief
  Entry point for the AtPlatformPolicy Driver.
  
  @param[in] ImageHandle  Image handle of this driver.
  @param[in] SystemTable  Global system service table.

  @retval EFI_SUCCESS           Initialization complete.
  @retval EFI_UNSUPPORTED       The chipset is unsupported by this driver.
  @retval EFI_OUT_OF_RESOURCES  Do not have enough resources to initialize the driver.
  @retval EFI_DEVICE_ERROR      Device error, driver exits abnormally.

**/
{
  EFI_STATUS                        Status;
  DXE_MBP_DATA_PROTOCOL             *MbpData;  
  EFI_SETUP_UTILITY_PROTOCOL        *SetupUtility;
  CHIPSET_CONFIGURATION              *SetupVariable;  



  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "\nAT Policy Entry\n"));

  Status = gBS->LocateProtocol (
                &gMeBiosPayloadDataProtocolGuid,
                NULL,
                (VOID **) &MbpData
                );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "No MBP Data Protocol available"));
    return EFI_NOT_FOUND;
  }

  if (MbpData->MeBiosPayload.FwCapsSku.FwCapabilities.Fields.IntelAT == 0) {
    return EFI_NOT_FOUND;
  }

  Status = gBS->LocateProtocol (
                    &gEfiSetupUtilityProtocolGuid, 
                    NULL, 
                    (VOID **)&SetupUtility
                    );
  
  ASSERT_EFI_ERROR (Status);
  
  SetupVariable = (CHIPSET_CONFIGURATION *)(SetupUtility->SetupNvData);
  

  ///
  /// AT DXE Policy Init
  ///
  mAtPlatformPolicyInstance.Revision                 = DXE_PLATFORM_AT_POLICY_PROTOCOL_REVISION_1;

  ///
  /// Initialzie the AT Configuration
  ///
  mAtPlatformPolicyInstance.At.AtAmBypass            = PcdGet8 ( PcdAtAtAmBypass );
  mAtPlatformPolicyInstance.At.AtEnterSuspendState   = SetupVariable->AtEnterSuspendState;
  mAtPlatformPolicyInstance.At.AtSupported           = 1;
  mAtPlatformPolicyInstance.At.AtPba                 = 1;
//[-start-120917-IB06150246-remove]//
//  mAtPlatformPolicyInstance.At.AtRecoveryAttempt     = SetupVariable->AtRecoveryAttempt;
//[-end-120917-IB06150246-remove]//
//[-start-121220-IB10820206-add]//
//[-start-130524-IB05160451-modify]//
  Status = OemSvcUpdateDxeAtPolicy (&mAtPlatformPolicyInstance);
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "DexOemSvcChipsetLib OemSvcUpdateDxeAtPolicy, Status : %r\n", Status));
//[-end-130524-IB05160451-modify]//
  if (Status == EFI_SUCCESS) {
    return Status;
  }
//[-end-121220-IB10820206-add]//

  ///
  /// Install the AT Platform Policy PROTOCOL interface
  ///
  Status = gBS->InstallMultipleProtocolInterfaces (
               &ImageHandle,
               &gDxePlatformAtPolicyGuid,
               &mAtPlatformPolicyInstance,
               NULL
             );

  return Status;
}
