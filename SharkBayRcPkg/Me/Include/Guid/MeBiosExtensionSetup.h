/** @file

  Me Bios Extension 16 Setup Options Guid definitions

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

#ifndef _EFI_ME_BIOS_EXTENSION_SETUP_GUID_H_
#define _EFI_ME_BIOS_EXTENSION_SETUP_GUID_H_

///
/// A NV-RAM variable for storing Intel MEBX setup option settings is created if this variable is not
/// present.  Those settings are used by BIOS during POST to bypass portions of the code if the
/// Intel ME features are not available
///
#define EFI_ME_BIOS_EXTENSION_SETUP_GUID \
  { 0x1BAD711C, 0xD451, 0x4241, { 0xB1, 0xF3, 0x85, 0x37, 0x81, 0x2E, 0x0C, 0x70 }}

#define EFI_ME_BIOS_EXTENSION_SETUP_VARIABLE_NAME         (L"MeBiosExtensionSetup")
#define EFI_ME_BIOS_EXTENSION_SETUP_FORMAT_VARIABLE_NAME  ("MeBiosExtensionSetup")

extern CHAR16   gEfiMeBiosExtensionSetupName[];

extern EFI_GUID gEfiMeBiosExtensionSetupGuid;

#pragma pack(1)

///
/// A NV-RAM variable for storing Intel MEBX setup option settings is created if this variable is not
/// present.  Those settings are used by BIOS during POST to bypass portions of the code if the
/// Intel ME features are not available. The information is obtained from Intel MEBx BIOS sync data
/// structure
///
typedef struct {
  ///
  /// Sync Data Structure Version, a non-zero version indicates the variable is being initialized or valid
  ///
  UINT16  InterfaceVersion;
  UINT16  Reserved;
  ///
  /// Bit 0 - UPDATED - this structure has been updated by MEBX
  ///
  UINT32  Flags;
  ///
  /// Platform Manageability Selection
  ///   00h - Off
  ///   01h - On
  ///
  UINT8   PlatformMngSel;
  UINT8   Reserved4;
  UINT8   AmtSolIder;
  UINT8   Reserved1[2];
  ///
  /// CIRA Feature
  ///   00h - DISABLED
  ///   01h - ENABLED
  ///
  UINT8   RemoteAssistanceTriggerAvailablilty;
  ///
  /// KVM state (0 - DISABLED, 1 - ENABLED)
  ///
  UINT8   KvmEnable;
  ///
  /// 0 - On every later boot after BIOS 1st boot
  /// 1 - Mebx will not require a reboot for synchronizing SOL/IDER state, used when BIOS 1st boot only.
  ///
  UINT8   MebxDefaultSolIder;
  UINT8   Reserved2[10];
} ME_BIOS_EXTENSION_SETUP;

#define MNT_OFF     0x00
#define MNT_ON      0x01
#define LAN_ENABLE  0x01
#define SOL_ENABLE  0x01
#define IDER_ENABLE 0x02
#define KVM_ENABLE  0x01

#pragma pack()

#endif
