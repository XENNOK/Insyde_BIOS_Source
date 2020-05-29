/** @file
  Entry and initial functions for BootOptionPolicy driver

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

#ifndef _BOOT_OPTION_POLICY_DRIVER_H_
#define _BOOT_OPTION_POLICY_DRIVER_H_

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <FrameworkDxe.h>

#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/DevicePathLib.h>
#include <Library/KernelConfigLib.h>
#include <Library/VariableLib.h>

#include <IndustryStandard/Pci22.h>

#include <Guid/GlobalVariable.h>

#include <Protocol/DevicePath.h>
#include <Protocol/BootOptionPolicy.h>
#include <Protocol/SetupUtility.h>
#include <Protocol/LegacyBios.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/BlockIo.h>


#define MIN_ALIGNMENT_SIZE  sizeof(UINTN)
#define ALIGN_SIZE(a) ((a % MIN_ALIGNMENT_SIZE) ? MIN_ALIGNMENT_SIZE - (a % MIN_ALIGNMENT_SIZE) : 0)

#define EFI_BOOT_OPTION_POLICY_SIGNATURE    SIGNATURE_32('B','O','P','o')

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
EFIAPI
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
EFIAPI
GetEfiOptionPriority (
  IN     EFI_BOOT_OPTION_POLICY_PROTOCOL    *This,
  IN     BOOLEAN                            CreateByOS,
  IN     EFI_DEVICE_PATH_PROTOCOL           *DevicePath,
  OUT    EFI_BOOT_ORDER_PRIORITY            *OptionPriority
  );


#endif
