/** @file

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

/** 
  This module install ACPI Boot Graphics Resource Table (BGRT).

  Copyright (c) 2011, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/

#ifndef _ACPI_BOOT_GRAPHICS_RESOURCE_TABLE_H_
#define _ACPI_BOOT_GRAPHICS_RESOURCE_TABLE_H_

#include <Uefi.h>

#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/Bmp.h>

#include <Protocol/AcpiTable.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/BootLogo.h>
#include <Protocol/ConsoleControl.h>
#include <Protocol/DriverBinding.h>
#include <Protocol/PciIo.h>


#include <Guid/EventGroup.h>
#include <Guid/EventLegacyBios.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/OemGraphicsLib.h>

typedef struct {
  EFI_GRAPHICS_OUTPUT_PROTOCOL            *Gop;
  EFI_GRAPHICS_OUTPUT_PROTOCOL_BLT        HookedBlt;
  EFI_GRAPHICS_OUTPUT_PROTOCOL_SET_MODE   HookedSetMode;
  UINT32                                  BltColor;
  UINTN                                   BltX;
  UINTN                                   BltY;
  UINTN                                   BltWidth;
  UINTN                                   BltHeight;  
} HOOKED_GOP_BLT_SET_MODE;

typedef struct {
  EFI_DRIVER_BINDING_PROTOCOL             *GopDriverBinding;         
  EFI_DRIVER_BINDING_START                HookedGopStartFun;
} HOOKED_GOP_DRV_BINDING;

#endif
