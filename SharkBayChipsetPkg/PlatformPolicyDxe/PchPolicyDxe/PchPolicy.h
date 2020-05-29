//;******************************************************************************
//;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _PCH_POLICY_DXE_H_
#define _PCH_POLICY_DXE_H_
#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/HobLib.h>
//[-start-121031-IB10820146-add]//
#include <Library/IoLib.h>
//[-end-121031-IB10820146-add]//
#include <PchAccess.h>
#include <Library/PchPlatformLib.h>
//[-start-121220-IB10820206-add]//
#include <Library/DxeOemSvcChipsetLib.h>
//[-end-121220-IB10820206-add]//
#include <Protocol/PchPlatformPolicy.h>
#include <Protocol/SetupUtility.h>
#include <Protocol/GlobalNvsArea.h>
#include <Guid/PlatformHardwareSwitch.h>
#include <Guid/HobList.h>
#include <ChipsetSetupConfig.h>
//[-start-121126-IB06460466-add]//
//[-start-130812-IB06720232-remove]//
//#include <OemEcLib.h>
//[-end-130812-IB06720232-remove]//
//[-end-121126-IB06460466-add]//
//[-start-121126-IB06460466-add]//
#include <Protocol/PlatformInfo.h>
//[-end-121126-IB06460466-add]//
//[-start-130307-IB03780481-add]//
#include <OemThunderbolt.h>
//[-end-130307-IB03780481-add]//

//
// Data
// BUGBUG: These constants are defined in multiple places.
//
#define PLATFORM_NUM_SMBUS_RSVD_ADDRESSES 4

#define SMBUS_ADDR_CH_A_1 0xA0
#define SMBUS_ADDR_CH_A_2 0xA2
#define SMBUS_ADDR_CH_B_1 0xA4
#define SMBUS_ADDR_CH_B_2 0xA6

#define PCH_USB_EHCI1 0
#define PCH_USB_EHCI2 1

#define PCH_DEVICE_DEFAULT  0

EFI_STATUS
SetPcieConfig (
  IN PLATFORM_HARDWARE_SWITCH          *PlatformHardwareSwitch,
  IN CHIPSET_CONFIGURATION              *SystemConfiguration
);

VOID
DumpPCHPolicy (
  IN DXE_PCH_PLATFORM_POLICY_PROTOCOL        *PchPlatformPolicy
);

VOID
PCHPolicyUpdateACPI (
  VOID
  );

VOID
EFIAPI
PCHPolicyUpdateACPICallback (
  IN      EFI_EVENT        Event,
  IN      VOID             *Context
  );

#endif
