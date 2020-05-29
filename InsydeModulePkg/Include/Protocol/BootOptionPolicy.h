/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _EFI_BOOT_OPTION_POLICY_H
#define _EFI_BOOT_OPTION_POLICY_H

#include <Uefi.h>
#include <Protocol/DevicePath.h>

#define IN_FIRST                            0
#define IN_LAST                             1
#define IN_AUTO                             2

#define LEGACY_BOOT_DEV                     0x0001
#define EFI_BOOT_DEV                        0x0000

#define BOOT_POLICY_OTHER_DEVICE            0xEE

#define EFI_DP_TYPE_MASK                    0x7F
#define EFI_DP_TYPE_UNPACKED                0x80

typedef enum {
  OsCreateEfiBootOption,
  RecoveryCreateEfiBootOption,
  RemovableEfiBootOption,
  NetWorkEfiBootOption,
  ShellEfiBootOption,
  OtherEfiBootOption,
} EFI_BOOT_ORDER_PRIORITY;

//
// Global Id for BootOptio Policy
//
#define EFI_BOOT_OPTION_POLICY_PROTOCOL_GUID \
  { \
    0x0E664C9F, 0x8D6A, 0x4763, 0x8F, 0xED, 0xCB, 0x61, 0xC3, 0x76, 0x23, 0x2D \
  }

typedef struct _EFI_BOOT_OPTION_POLICY_PROTOCOL EFI_BOOT_OPTION_POLICY_PROTOCOL;

//
// Get position of new BootOption
//
typedef
EFI_STATUS
(EFIAPI *EFI_FIND_POSITION_OF_NEW_BOOT_OPTION) (
  IN     EFI_BOOT_OPTION_POLICY_PROTOCOL    *This,
  IN     EFI_DEVICE_PATH_PROTOCOL           *DevicePath,
  IN     UINT16                             *BootOrder,
  IN     UINTN                              BootOptionType,
  IN     UINTN                              BootOptionNum,
  IN     UINTN                              RegisterOptionNumber, 
  IN OUT UINTN                              *NewPosition
  );
  
typedef
EFI_STATUS
(EFIAPI *EFI_GET_EFI_BOOT_OPTION_PRIORITY) (
  IN     EFI_BOOT_OPTION_POLICY_PROTOCOL    *This,
  IN     BOOLEAN                            CreateByOS,  
  IN     EFI_DEVICE_PATH_PROTOCOL           *DevicePath,
  OUT    EFI_BOOT_ORDER_PRIORITY            *OptionPriority
  );
/*++

Routine Description:

  Use device path to get the boot priority of EFI boot option. The priority policy as below
  1. permanent OS created Boot Variables
  2. Firmware created "Widnows Boot Loader" for recovery whne OS har disk boot item is missing.
  3. Removable media with EFI spec boot \EFI\Boot\BootX64.efi or \EFI\Boot\BootIA32.efi
  4. Network load image items when EFI Network Boot is enabled.
  5. EFI Shell if found in ROM image

Arguments:

  This                  - A pointer to the EFI_BOOT_OPTION_POLICY_PROTOCOL instance.
  CreateByOS            - A Flag to indicate this boot otpion is create by OS or not.
  DevicePath            - A pointer to the EFI_DEVICE_PATH_PROTOCOL instance.
  OptionPriority        - A pointer to save boot option prioriy.

Returns:

  EFI_SUCCESS           - Get option priority successful.
  EFI_UNSUPPORTED       - Input device path isn't valid or is a BBS device path.
  EFI_INVALID_PARAMETER - Input parameter is invalid.

--*/   

//
// Protocol definition
//
struct _EFI_BOOT_OPTION_POLICY_PROTOCOL {

  EFI_FIND_POSITION_OF_NEW_BOOT_OPTION      FindPositionOfNewBootOption;
  EFI_GET_EFI_BOOT_OPTION_PRIORITY          GetEfiOptionPriority;
};

extern EFI_GUID gEfiBootOptionPolicyProtocolGuid;
#endif
