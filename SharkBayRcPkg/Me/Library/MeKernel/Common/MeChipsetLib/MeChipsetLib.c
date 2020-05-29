/** @file

  Me Chipset Lib implementation.

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

//[-start-120808-IB10820098-modify]//
#include <Uefi.h>
#include <Library/IoLib.h>
#include <MeAccess.h>
//[-end-120808-IB10820098-modify]//
//[-start-120905-IB10820116-modify]//
#include <PchPlatformLib.h>
//[-end-120905-IB10820116-modify]//

/**
  Enable/Disable Me devices

  @param[in] WhichDevice          Select of Me device
  @param[in] DeviceFuncCtrl       Function control

  @retval None
**/
VOID
MeDeviceControl (
  IN  ME_DEVICE                   WhichDevice,
  IN  ME_DEVICE_FUNC_CTRL         DeviceFuncCtrl
  )
{
  switch (WhichDevice) {
    case HECI1:
    case HECI2:
    case IDER:
    case SOL:
      if (DeviceFuncCtrl) {
        Mmio16And (PCH_RCRB_BASE, R_PCH_RCRB_FD2, ~(BIT0 << WhichDevice));
      } else {
        Mmio16Or (PCH_RCRB_BASE, R_PCH_RCRB_FD2, BIT0 << WhichDevice);
      }
      Mmio16 (PCH_RCRB_BASE, R_PCH_RCRB_FD2);
      break;

    case USBR1:
      if (DeviceFuncCtrl) {
        MmioOr16 (
          MmPciAddress (0,
          DEFAULT_PCI_BUS_NUMBER_PCH,
          PCI_DEVICE_NUMBER_PCH_USB,
          PCI_FUNCTION_NUMBER_PCH_EHCI,
          0x7A),
          (UINT16) (0x100)
          );
      } else {
        MmioAnd16 (
          MmPciAddress (0,
          DEFAULT_PCI_BUS_NUMBER_PCH,
          PCI_DEVICE_NUMBER_PCH_USB,
          PCI_FUNCTION_NUMBER_PCH_EHCI,
          0x7A),
          (UINT16) (~0x100)
          );
      }
      break;

    case USBR2:
      if (GetPchSeries() == PchH) {
        if (DeviceFuncCtrl) {
          MmioOr16 (
            MmPciAddress (0,
            DEFAULT_PCI_BUS_NUMBER_PCH,
            PCI_DEVICE_NUMBER_PCH_USB_EXT,
            PCI_FUNCTION_NUMBER_PCH_EHCI2,
            0x7A),
            (UINT16) (0x100)
            );
        } else {
          MmioAnd16 (
            MmPciAddress (0,
            DEFAULT_PCI_BUS_NUMBER_PCH,
            PCI_DEVICE_NUMBER_PCH_USB_EXT,
            PCI_FUNCTION_NUMBER_PCH_EHCI2,
            0x7A),
            (UINT16) (~0x100)
            );
        }
      }
      break;

    ///
    /// Function Disable SUS well lockdown
    ///
    case FDSWL:
      if (DeviceFuncCtrl) {
        Mmio16Or (PCH_RCRB_BASE, R_PCH_RCRB_FDSW, (UINT16) B_PCH_RCRB_FDSW_FDSWL);
      } else {
        Mmio16And (PCH_RCRB_BASE, R_PCH_RCRB_FDSW, (UINT16)~(B_PCH_RCRB_FDSW_FDSWL));
      }
      Mmio16 (PCH_RCRB_BASE, R_PCH_RCRB_FDSW);
      break;
  }
}
