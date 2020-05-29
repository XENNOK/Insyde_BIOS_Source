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

#ifndef _XHCI_SMI_DISPATCHER_H
#define _XHCI_SMI_DISPATCHER_H

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/DevicePathLib.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/SmmUsbDispatch2.h>

//
// NEC USB 3.0 WDM driver has a special behavior, it will trigger
// a XHCI hand-off to BIOS event before S3/S4 suspend, this action
// will make BIOS XHCI driver issues a HCRESET command(in order
// to re-enumerate the device) and caused USB mouse wakeup failed.
// Set SKIP_HAND_OFF_TO_BIOS_EVENT to 1 to make the BIOS XHCI driver
// ignore the XHCI hand-off to BIOS event to workaround the issue.
// But platform owner needed to know this workaround will caused
// whole devices plugged in USB 3.0 port doesn't work on other 
// OS(Linux) after XHCI hand-off to BIOS because BIOS XHCI driver
// unable to own the XHCI anymore
//
#define  SKIP_HAND_OFF_TO_BIOS_EVENT    (mXhciPcdSetting & 0x01)
//
// For NEC/TI USB 3.0, please set to 0 because it will be high
// active(pull high when SMI occurred). For other USB 3.0 please
// set to 1 due to low active(this is standard)
//
#define  INVERSE_GPIO_SIGNAL            ((mXhciPcdSetting & 0x02) >> 1)
//
// The GPIO pin for XHCI SMIB.
// Please based on platform setting to change the value
//
#define  GPIO_PIN_FOR_XHCI_SMIB         ((mXhciPcdSetting & 0xff00) >> 8)
//
// The PCI location of the bridge which plugged XHCI device.
// Please based on platform setting to change the bridge position
//
#define  BRIDGE_DEV_FOR_XHCI            ((mXhciPcdSetting & 0xff000000) >> 24)
#define  BRIDGE_FUN_FOR_XHCI            ((mXhciPcdSetting & 0x00ff0000) >> 16)
//
// The PCI location of the XHCI behind bridge
//
#define  XHCI_DEV                       0x00
#define  XHCI_FUN                       0x00
//
// The LPC related register used to locate GPIO base
//
#define  LPC_BUS                        0x00
#define  LPC_DEV                        0x1f
#define  LPC_FUN                        0x00
#define    R_ACPI_PM_BASE               0x40
#define    ACPI_PM_BASE_MASK            0xFFF8
#define    R_ACPI_GPIO_BASE             0x48
#define    ACPI_GPIO_BASE_MASK          0xFFF8
#define    R_ACPI_GPIO_ROUT             0xB8
//
// The GPIO related registers
//
#define  ALT_GP_SMI_EN                  0x38
#define  ALT_GP_SMI_STS                 0x3A

#define  GP_USE_SEL                     0x00
#define  GP_IO_SEL                      0x04
#define  GP_INV_SEL                     0x2C

#define  LPC_PCI_PMBASE                 PCI_LIB_ADDRESS (LPC_BUS, LPC_DEV, LPC_FUN, R_ACPI_PM_BASE)
#define  LPC_PCI_GPIOBASE               PCI_LIB_ADDRESS (LPC_BUS, LPC_DEV, LPC_FUN, R_ACPI_GPIO_BASE)
#define  LPC_PCI_GPIO_ROUT              PCI_LIB_ADDRESS (LPC_BUS, LPC_DEV, LPC_FUN, R_ACPI_GPIO_ROUT)

#define  DATABASE_RECORD_SIGNATURE      SIGNATURE_32 ('X', 'H', 'C', 'R')

typedef struct _DATABASE_RECORD DATABASE_RECORD;

typedef struct _DATABASE_RECORD {
  UINT32                                        Signature;
  DATABASE_RECORD                               *Next;
  EFI_SMM_HANDLER_ENTRY_POINT2                  DispatchFunction;
  EFI_SMM_USB_REGISTER_CONTEXT                  RegisterContext;
};

EFI_STATUS
XhciSmiRegister (
  IN  CONST EFI_SMM_USB_DISPATCH2_PROTOCOL      *This,
  IN        EFI_SMM_HANDLER_ENTRY_POINT2        DispatchFunction,
  IN  CONST EFI_SMM_USB_REGISTER_CONTEXT        *RegisterContext,
  OUT       EFI_HANDLE                          *DispatchHandle
  );

EFI_STATUS
XhciSmiUnregister (
  IN CONST EFI_SMM_USB_DISPATCH2_PROTOCOL       *This,
  IN       EFI_HANDLE                           DispatchHandle
  );

EFI_STATUS
EFIAPI
XhciSmmCoreDispatcher (
  IN       EFI_HANDLE                           DispatchHandle,
  IN CONST VOID                                 *Context         OPTIONAL,
  IN OUT   VOID                                 *CommBuffer      OPTIONAL,
  IN OUT   UINTN                                *CommBufferSize  OPTIONAL
  );
  
#endif