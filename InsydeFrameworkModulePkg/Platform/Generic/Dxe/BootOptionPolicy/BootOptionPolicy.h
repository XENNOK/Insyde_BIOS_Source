//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _BOOT_OPTION_POLICY_DRIVER_H_
#define _BOOT_OPTION_POLICY_DRIVER_H_

#include "Tiano.h"
#include "EfiDriverLib.h"
#include "SetupConfig.h"
#include "EfiPrintLib.h"
#include "Pci22.h"
#include EFI_GUID_DEFINITION     (GlobalVariable)
#include EFI_GUID_DEFINITION     (EfiShell)
#include EFI_PROTOCOL_DEFINITION (BootOptionPolicy)
#include EFI_PROTOCOL_DEFINITION (EfiSetupUtility)
#include EFI_PROTOCOL_DEFINITION (LegacyBios)
#include EFI_PROTOCOL_DEFINITION (SimpleFileSystem)
#include EFI_PROTOCOL_DEFINITION (BlockIo)

#define MIN_ALIGNMENT_SIZE  sizeof(UINTN)
#define ALIGN_SIZE(a) ((a % MIN_ALIGNMENT_SIZE) ? MIN_ALIGNMENT_SIZE - (a % MIN_ALIGNMENT_SIZE) : 0)


#define EFI_BOOT_OPTION_POLICY_SIGNATURE    EFI_SIGNATURE_32('B','O','P','o')

typedef struct {
  UINTN                                     Signature;
  EFI_HANDLE                                Handle;

  UINTN                                     NewPositionPolicy;
  UINTN                                     BootNormalPriority;
  UINTN                                     LegacyNormalMenuType;
  UINT8                                     BootTypeOrder[MAX_BOOT_ORDER_NUMBER];
  EFI_BOOT_OPTION_POLICY_PROTOCOL           BootOptionPolicy;
} BOOT_OPTION_POLICY_DATA;

#define EFI_BOOT_OPTION_POLICY_FROM_THIS(a) CR (a, BOOT_OPTION_POLICY_DATA, BootOptionPolicy, EFI_BOOT_OPTION_POLICY_SIGNATURE)

EFI_STATUS
FindPositionOfNewBootOption (
  IN     EFI_BOOT_OPTION_POLICY_PROTOCOL    * This,
  IN     EFI_DEVICE_PATH_PROTOCOL           *DevicePath,
  IN     UINT16                             *BootOrder,
  IN     UINTN                              BootOptionType,
  IN     UINTN                              BootOptionNum,
  IN     UINTN                              RegisterOptionNumber,
  IN OUT UINTN                              *NewPosition
);

EFI_STATUS
GetEfiOptionPriority (
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

VOID *
GetVariableAndSize (
  IN  CHAR16              *Name,
  IN  EFI_GUID            *VendorGuid,
  OUT UINTN               *VariableSize
);

#endif
