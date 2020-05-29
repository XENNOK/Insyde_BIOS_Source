/** @file
  When user enable PxeToLan.
  Control network stack behavior via callback.

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _NATIVE_DRIVER_LOCKER_H
#define _NATIVE_DRIVER_LOCKER_H

#include <IndustryStandard/Pci22.h>
#include <Guid/NetworkEventGroup.h>
#include <Protocol/PciIo.h>
#include <Protocol/LegacyBiosPlatform.h>
#include <Protocol/NetworkLocker.h>
#include <KernelSetupConfig.h>
#include "PxeOptionRomHeader.h"

//
// for Boot Type
//
#define SCU_DUAL_BOOT_TYPE                 0
#define SCU_LEGACY_BOOT_TYPE               1
#define SCU_UEFI_BOOT_TYPE                 2

//
// for SCU Pxe Boot to LAN
//
#define SCU_PXE_BOOT_TO_LAN_ENABLED        1
#define SCU_PXE_BOOT_TO_LAN_DISABLED       0

#define UEFI_NETWORK_BOOT_OPTION_MAX_CHAR  60
#define UEFI_NETWORK_MAC_ADDRESS_LENGTH    32

//
// for PcdH2ONetworkSupported
//
#define UEFI_NETWORK_BOOT_OPTION_IPV4      0
#define UEFI_NETWORK_BOOT_OPTION_IPV6      1
#define UEFI_NETWORK_BOOT_OPTION_BOTH      2
#define UEFI_NETWORK_BOOT_OPTION_LEGACY    3
#define UEFI_NETWORK_BOOT_OPTION_DHCP      4
#define UEFI_NETWORK_BOOT_OPTION_MAX       8

//
// String Length : "VLAN(XXXX)"
//   Max length 10 + 1 null string end
//
#define UEFI_NETWORK_VLAN_STRING_LENGTH    11

typedef struct _NETWORK_LOCKER_INSTANCE {
  EFI_HANDLE                           Handle;
  EFI_LEGACY_BIOS_PLATFORM_PROTOCOL    *LegacyBiosPlatform;
  EFI_LEGACY_BIOS_PLATFORM_HOOKS       LegacyBiosPlatformHooks;
  EFI_EVENT                            LegacyBiosPlatformProtocolNotifyEvent;
  UINT8                                BootType;
  BOOLEAN                              NetworkStackEnable;
  UINT8                                NetworkStackStatus;
  NETWORK_LOCKER_PROTOCOL              NetworkLockerProtocol;
} NETWORK_LOCKER_INSTANCE;

#endif //_NATIVE_DRIVER_LOCKER_H
