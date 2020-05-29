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
//;   IccPolicy.h
//;

#ifndef _ICC_POLICY_DXE_H_
#define _ICC_POLICY_DXE_H_

#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/IoLib.h>
#include <Library/CmosLib.h>
//[-start-121220-IB10820206-add]//
#include <Library/DxeOemSvcChipsetLib.h>
//[-end-121220-IB10820206-add]//
#include <Protocol/SetupUtility.h>
#include <Protocol/ICCPlatformPolicy.h>
#include <ChipsetCmos.h>
#include <ChipsetSetupConfig.h>
#include <PchAccess.h>
//[-start-121005-IB05330380-add]//
#include <Protocol/PlatformInfo.h>
//[-end-121005-IB05330380-add]//

EFI_STATUS
EFIAPI
ICCSetBackupProfile (
     OUT  UINT8        *ICCBackupProfile
  );

VOID
DumpICCPolicy (
  IN      DXE_PLATFORM_ICC_POLICY_PROTOCOL        *ICCPlatformPolicy
  );

#endif
