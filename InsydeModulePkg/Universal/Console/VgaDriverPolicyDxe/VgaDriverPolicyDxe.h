/** @file
  Header file for Vga Driver Policy Driver's Data Structures

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _VGA_DRIVER_POLICY_H_
#define _VGA_DRIVER_POLICY_H_

#include <Protocol/PciIo.h>
#include <Protocol/DevicePath.h>
#include <Protocol/ComponentName.h>
#include <Protocol/ComponentName2.h>
#include <Protocol/DriverBinding.h>

#include <IndustryStandard/Pci22.h>

#include <Guid/DebugMask.h>

#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/DevicePathLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/KernelConfigLib.h>
#include <Library/DebugLib.h>
#include <Library/VariableLib.h>

#include <KernelSetupConfig.h>

#define VGA_DRIVER_POLICY_NAME L"Vga Driver Policy"
//
// It is the volatile variable which store the Primary Vga contoller devicepath and will be Updated by Platform code.
// Provide Vga policy Driver to stop the non-Primary Vga controller.
// The Data type will be the EFI_DEVICE_PATH_PROTOCOL *ActiveVgaDev.
//
#define ACTIVE_VGA_VAR_NAME    L"ActiveVgaDev"

//
// Common language code definition
//
#define LANGUAGE_CODE_ENGLISH_ISO639    "eng"
#define LANGUAGE_CODE_ENGLISH_RFC4646   "en-US"

EFI_STATUS
EFIAPI
VgaDriverPolicyBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL  * This,
  IN EFI_HANDLE                   Controller,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath OPTIONAL
  );

EFI_STATUS
EFIAPI
VgaDriverPolicyBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL  * This,
  IN EFI_HANDLE                   Controller,
  IN EFI_DEVICE_PATH_PROTOCOL     * RemainingDevicePath OPTIONAL
  );

EFI_STATUS
EFIAPI
VgaDriverPolicyBindingStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN  EFI_HANDLE                     Controller,
  IN  UINTN                          NumberOfChildren,
  IN  EFI_HANDLE                     *ChildHandleBuffer
  );

VOID *
VgaDriverPolicyGetVariable (
  IN  CHAR16                              *Name
  );

BOOLEAN
VgaDriverPolicyMatchDevicePaths (
  IN  EFI_DEVICE_PATH_PROTOCOL            *Multi,
  IN  EFI_DEVICE_PATH_PROTOCOL            *Single
  );

//
// Global Variables
//
extern EFI_DRIVER_BINDING_PROTOCOL      gVgaDriverPolicyDriverBinding;
extern EFI_COMPONENT_NAME_PROTOCOL      gVgaDriverPolicyComponentName;
extern EFI_COMPONENT_NAME2_PROTOCOL     gVgaDriverPolicyComponentName2;
#endif
