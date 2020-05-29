//;******************************************************************************
//;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
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
//;   AmtPolicy.h
//;

#ifndef _AMT_POLICY_DXE_H_
#define _AMT_POLICY_DXE_H_

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/HobLib.h>
#include <Library/CmosLib.h>
//[-start-121219-IB10820205-add]//
#include <Library/DxeOemSvcChipsetLib.h>
//[-end-121219-IB10820205-add]//
#include <Protocol/SetupUtility.h>
#include <Protocol/MeBiosPayloadData.h>
#include <Protocol/AmtPlatformPolicy.h>
//[-start-130208-IB10820234-modify]//
#include <Guid/SetupDefaultHob.h>
//[-end-130208-IB10820234-modify]//
#include <Guid/MeBiosExtensionSetup.h>
#include <Guid/DebugMask.h>
#include <ChipsetSetupConfig.h>
#include <ChipsetCmos.h>

CHAR16  gEfiMeBiosExtensionSetupName[] = EFI_ME_BIOS_EXTENSION_SETUP_VARIABLE_NAME;

VOID
DumpAMTPolicy (
  IN      DXE_AMT_POLICY_PROTOCOL        *AMTPlatformPolicy
  );

#endif
