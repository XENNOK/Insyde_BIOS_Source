//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Module Name :
//;
//;   SrIovPolicy.c
//;

#include <SrIovPolicy.h>
#include <ChipsetSetupConfig.h>

EFI_STATUS
EFIAPI
SRIOVPolicyEntry (
  IN      EFI_HANDLE              ImageHandle,
  IN      EFI_SYSTEM_TABLE        *SystemTable
  )
{
  EFI_STATUS                        Status;
  EFI_SETUP_UTILITY_PROTOCOL        *SetupUtility;
  CHIPSET_CONFIGURATION              *SetupVariable;

  Status = gBS->LocateProtocol (&gEfiSetupUtilityProtocolGuid, NULL, (VOID **)&SetupUtility);
  if (EFI_ERROR (Status)) {
    return Status;
  }  
  ASSERT_EFI_ERROR (Status);


  if (SetupUtility == NULL) {
    return EFI_ABORTED;
  }
  if (SetupUtility->SetupNvData == NULL) {
    return EFI_ABORTED;
  }
  SetupVariable = (CHIPSET_CONFIGURATION *)(SetupUtility->SetupNvData);

  PcdSetBool (PcdSrIovSupport, (BOOLEAN)SetupVariable->PCIeSRIOVSupport);
  PcdSetBool (PcdAriSupport,   (BOOLEAN)SetupVariable->PCIeAriSupport);

  return EFI_SUCCESS;
}

