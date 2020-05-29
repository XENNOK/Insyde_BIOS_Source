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
//;   MePolicy.h
//;

#ifndef _ME_POLICY_DXE_H_
#define _ME_POLICY_DXE_H_

#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
//[-start-121220-IB10820206-add]//
#include <Library/DxeOemSvcChipsetLib.h>
//[-end-121220-IB10820206-add]//
#include <Protocol/SetupUtility.h>
#include <Protocol/MePlatformPolicy.h>
#include <Protocol/PlatformMeHook.h>
#include <Protocol/Heci.h>
#include <Protocol/MeBiosPayloadData.h>
#include <ChipsetSetupConfig.h>
#include <Guid/MeInfo.h>

//[-start-121003-IB10820132-remove]//
//#define THERMAL_CONFIG_DIMM_NUMBER                  2
//#define THERMAL_CONFIG_SMBUS_ADDRESS_NO_DIMM        0x00
//#define THERMAL_CONFIG_SMBUS_ADDRESS_DIMM0          0x30
//#define THERMAL_CONFIG_SMBUS_ADDRESS_DIMM1          0x34
//[-end-121003-IB10820132-remove]//

VOID
MEReportError (
  IN      ME_ERROR_MSG_ID        MsgId
  );

VOID
DumpMEPolicy (
  IN      DXE_ME_POLICY_PROTOCOL        *MEPlatformPolicy
  );

EFI_STATUS
EFIAPI
PreGlobalResetHook (
  VOID
  );
#endif
