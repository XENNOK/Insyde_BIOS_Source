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

#ifndef _KEYBOARD_INIT_H_
#define _KEYBOARD_INIT_H_

#include <Uefi.h>
#include <PiPei.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/TimerLib.h>
#include <Ppi/MasterBootMode.h>
#include <Ppi/EndOfPeiPhase.h>
#include <Guid/KbcInitialized.h>

#define KBC_TIME_OUT                      0x10000
#define KEY_OBF                           1
#define KEY_IBF                           2
#define KEY_DATA                          0x60
#define KEY_CMD_STATE                     0x64
#define KBC_READ_CMD_BYTE                 0x20
#define KBC_WRITE_CMD_BYTE                0x60
//
// Defines for stall
//
#define PEI_STALL_RESOLUTION      1
#define ACPI_TIMER_ADDR       (FixedPcdGet16(PcdPerfPkgAcpiIoPortBaseAddress) + 0x08)
#define ACPI_TIMER_MAX_VALUE  0x1000000 // The timer is 24 bit overflow

EFI_STATUS
EFIAPI
BootModePpiNotifyCallback (
  IN EFI_PEI_SERVICES                     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR            *NotifyDescriptor,
  IN VOID                                 *Ppi
  );

EFI_STATUS
EFIAPI
EndOfPeiPpiNotifyCallback (
  IN EFI_PEI_SERVICES                     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR            *NotifyDescriptor,
  IN VOID                                 *Ppi
  );

#endif


