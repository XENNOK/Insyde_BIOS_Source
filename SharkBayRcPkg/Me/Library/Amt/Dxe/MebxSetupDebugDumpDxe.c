/** @file

  Dump whole ME_BIOS_EXTENSION_SETUP and serial out.

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

//[-start-120808-IB10820098-modify]//
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/Amt/Dxe/AmtPolicyLib.h>
//[-end-120808-IB10820098-modify]//

/**
  Dump ME_BIOS_EXTENSION_SETUP variable

  @param[in] MeBiosExtensionSetup   Pointer to ME_BIOS_EXTENSION_SETUP variable

  @retval None
**/
VOID
DxeMebxSetupVariableDebugDump (
  IN ME_BIOS_EXTENSION_SETUP      *MeBiosExtensionSetup OPTIONAL
  )
{
  EFI_STATUS                Status;
  UINTN                     VariableSize;
  ME_BIOS_EXTENSION_SETUP   MeBxSetup;
  ME_BIOS_EXTENSION_SETUP   *MeBxSetupPtr;

  if (MeBiosExtensionSetup == NULL) {
    Status = gRT->GetVariable (
              gEfiMeBiosExtensionSetupName,
              &gEfiMeBiosExtensionSetupGuid,
              NULL,
              &VariableSize,
              &MeBxSetup
              );
    if (EFI_ERROR(Status)) {
      return;
    }
    MeBxSetupPtr = &MeBxSetup;
  } else {
    MeBxSetupPtr = MeBiosExtensionSetup;
  }

  DEBUG ((EFI_D_INFO, "\n------------------------ MeBiosExtensionSetup Dump Begin -----------------\n"));
  DEBUG ((EFI_D_INFO, " InterfaceVersion : 0x%x\n", MeBxSetupPtr->InterfaceVersion));
  DEBUG ((EFI_D_INFO, " Flags : 0x%x\n", MeBxSetupPtr->Flags));
  DEBUG ((EFI_D_INFO, " PlatformMngSel : 0x%x\n", MeBxSetupPtr->PlatformMngSel));
  DEBUG ((EFI_D_INFO, " AmtSolIder : 0x%x\n", MeBxSetupPtr->AmtSolIder));
  DEBUG (
    (EFI_D_INFO,
    " RemoteAssistanceTriggerAvailablilty : 0x%x\n",
    MeBxSetupPtr->RemoteAssistanceTriggerAvailablilty)
    );
  DEBUG ((EFI_D_INFO, " KvmEnable : 0x%x\n", MeBxSetupPtr->KvmEnable));
  DEBUG ((EFI_D_INFO, " MebxDefaultSolIder : 0x%x\n", MeBxSetupPtr->MebxDefaultSolIder));

  DEBUG ((EFI_D_INFO, "\n------------------------ MeBiosExtensionSetup Dump End -------------------\n"));

}
