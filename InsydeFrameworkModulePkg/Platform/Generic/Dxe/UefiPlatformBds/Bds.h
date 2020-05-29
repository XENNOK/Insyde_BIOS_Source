//;******************************************************************************
//;* Copyright (c) 1983-2014, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++

Copyright (c) 2004 - 2005, Intel Corporation
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  Bds.h

Abstract:

  Head file for BDS Architectural Protocol implementation

Revision History

--*/

#ifndef _BDS_H
#define _BDS_H

#include "Tiano.h"
#include "PeiHob.h"
#include "EfiDriverLib.h"
#include "SensitiveVariableLib.h"

#include EFI_ARCH_PROTOCOL_DEFINITION (Bds)

#include EFI_PROTOCOL_DEFINITION (GenericMemoryTest)
#include EFI_PROTOCOL_CONSUMER (GraphicsOutput)
#include EFI_PROTOCOL_CONSUMER (DriverHealth)
#include EFI_PROTOCOL_CONSUMER (StartOfBdsDiagnostics)

//
// Bds AP Context data
//
#define EFI_BDS_ARCH_PROTOCOL_INSTANCE_SIGNATURE  EFI_SIGNATURE_32 ('B', 'd', 's', 'A')
typedef struct {
  UINTN                     Signature;

  EFI_HANDLE                Handle;

  EFI_BDS_ARCH_PROTOCOL     Bds;

  //
  // Save the current boot mode
  //
  EFI_BOOT_MODE             BootMode;

  //
  // Set true if boot with default settings
  //
  BOOLEAN                   DefaultBoot;

  //
  // The system default timeout for choose the boot option
  //
  UINT16                    TimeoutDefault;

  //
  // Memory Test Level
  //
  EXTENDMEM_COVERAGE_LEVEL  MemoryTestLevel;

} EFI_BDS_ARCH_PROTOCOL_INSTANCE;

#define EFI_BDS_ARCH_PROTOCOL_INSTANCE_FROM_THIS(_this) \
  CR (_this, \
      EFI_BDS_ARCH_PROTOCOL_INSTANCE, \
      Bds, \
      EFI_BDS_ARCH_PROTOCOL_INSTANCE_SIGNATURE \
      )

//
// Prototypes
//
EFI_STATUS
EFIAPI
BdsInitialize (
  IN EFI_HANDLE                     ImageHandle,
  IN EFI_SYSTEM_TABLE               *SystemTable
  );

EFI_STATUS
EFIAPI
BdsEntry (
  IN  EFI_BDS_ARCH_PROTOCOL *This
  );

EFI_STATUS
ProcessCapsules (
  EFI_BOOT_MODE BootMode
  );

EFI_STATUS
BdsMemoryTest (
  EXTENDMEM_COVERAGE_LEVEL Level,
  IN UINTN                    BaseLine
  );

EFI_STATUS
PlatformBdsShowProgress (
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL TitleForeground,
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL TitleBackground,
  CHAR16                        *Title,
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL ProgressColor,
  UINTN                         Progress,
  UINTN                         PreviousValue
  );

EFI_STATUS
AutoFailoverPolicyBehavior (
  IN BOOLEAN    AutoFailover,
  IN CHAR16     *Message
  );

#endif
