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

#ifndef _USB_LEGACY_CONTROL_H
#define _USB_LEGACY_CONTROL_H

#include <Uefi.h>
#include <PiSmm.h>

#include <Library/DevicePathLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>

#include <Protocol/SmmUsbDispatch2.h>
#include <Protocol/SmmBase2.h>

#define  USB_LEGACY_CONTROL_PROTOCOL_GUID \
 { 0x3084d2bd, 0xf589, 0x4be1, 0x8e, 0xf0, 0x26, 0xc6, 0xd6, 0x8a, 0x1b, 0xc8 }

#define  LPC_BUS                                        0x00
#define  LPC_DEV                                        0x1f
#define  LPC_FUN                                        0x00
#define  R_ACPI_PM_BASE                                 0x40  
#define  ACPI_PM_BASE_MASK                              0xFFF8
#define  R_LUKMC                                        0x94  
#define  B_LUKMC_TRAP_60R                               0x0001
#define  B_LUKMC_TRAP_60W                               0x0002
#define  B_LUKMC_TRAP_64R                               0x0004
#define  B_LUKMC_TRAP_64W                               0x0008
#define  B_LUKMC_USBSMIEN                               0x0010
#define  B_LUKMC_TRAP_STATUS                            0x0f00
#define  N_LUKMC_TRAP_STATUS                            8     
#define  B_LUKMC_USBPIRQEN                              0x2000

#define  LPC_PCI_PMBASE                                 ((LPC_BUS << 16) | (LPC_DEV << 11) | (LPC_FUN << 8) | (R_ACPI_PM_BASE))
#define  LPC_PCI_LUKMC                                  ((LPC_BUS << 16) | (LPC_DEV << 11) | (LPC_FUN << 8) | (R_LUKMC))

#define  IRQ1                                           0x00
#define  IRQ12                                          0x01

#define  USB_LEGACY_CONTROL_SETUP_EMULATION             0
#define  USB_LEGACY_CONTROL_GET_KBC_DATA_POINTER        1
#define  USB_LEGACY_CONTROL_GET_KBC_STATUS_POINTER      2
#define  USB_LEGACY_CONTROL_GENERATE_IRQ                3

typedef
VOID
(EFIAPI *USB_LEGACY_CONTROL_SMI_HANDLER) (
  IN     UINTN                                          Event,
  IN     VOID                                           *Context
  );

typedef
EFI_STATUS
(EFIAPI *USB_LEGACY_CONTROL) (
  IN     UINTN                                          Command,
  IN     VOID                                           *Param
  );

typedef struct {
  USB_LEGACY_CONTROL                                    UsbLegacyControl;
  USB_LEGACY_CONTROL_SMI_HANDLER                        SmiHandler;
  VOID                                                  *SmiContext;
  BOOLEAN                                               InSmm;
} USB_LEGACY_CONTROL_PROTOCOL;

EFI_STATUS
EFIAPI
UsbLegacyControl (
  IN     UINTN                                          Command,
  IN     VOID                                           *Param
  );

VOID
UsbLegacyControlSmiHandler (
  IN        EFI_HANDLE                                  Handle,
  IN  CONST EFI_SMM_USB_REGISTER_CONTEXT                *Context
  );

#endif
