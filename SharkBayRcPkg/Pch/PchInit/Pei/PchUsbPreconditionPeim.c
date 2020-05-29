/** @file

  PCH USB precondition feature support in PEI phase

@copyright
  Copyright (c) 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
#include "PchInitPeim.h"

#ifdef USB_PRECONDITION_ENABLE_FLAG

extern USB_CONTROLLER EhciControllersMap[];

/**
  Perform USB precondition on EHCI, it is the HC on USB HC in PEI phase

  @param[in]  Device              The device number of the EHCI
  @param[in]  EhciMmioBase        Memory base address of EHCI Controller

  @retval     None
**/
VOID
EhciPrecondition (
  IN  UINT8                       Device,
  IN  UINT32                      EhciMmioBase
  )
{
  UINTN                     HcResetTimeout;

  HcResetTimeout = 0;
  while ((HcResetTimeout < 200) &&
    (MmioRead32 (EhciMmioBase + R_PCH_EHCI_USB2CMD) & B_PCH_EHCI_USB2CMD_HCRESET)) {
    PchPmTimerStall (100);
    HcResetTimeout++;
  }

  if ((MmioRead32 (EhciMmioBase + R_PCH_EHCI_USB2CMD) & B_PCH_EHCI_USB2CMD_HCRESET) == 0) {
    //
    // Route all ports to this EHCI
    //
    MmioWrite32 ((EhciMmioBase + R_PCH_EHCI_CONFIGFLAG), BIT0);
  }
}

/**
  Perform USB precondition on XHCI, it is the HC on USB HC in PEI phase

  @param[in]  BusNumber           The Bus number of the XHCI
  @param[in]  Device              The device number of the XHCI
  @param[in]  Function            The function number of the XHCI
  @param[in]  XhciMmioBase        Memory base address of XHCI Controller
  @param[in]  XhciUSB2Ptr         Pointer to USB2 protocol port register
  @param[in]  HsPortCount         The number of USB2 protocol port supported by this XHCI

  @retval     None
**/
VOID
XhciPrecondition (
  IN  UINT8                       BusNumber,
  IN  UINT8                       Device,
  IN  UINT8                       Function,
  IN  UINT32                      XhciMmioBase,
  IN  UINTN                       *XhciUSB2Ptr,
  IN  UINTN                       HsPortCount,
  IN  UINTN                       *XhciUSB3Ptr,
  IN  UINTN                       SsPortCount
  )
{
  UINT32                    Data32;
  UINTN                     HcHaltTimeout;

  //
  // Set the XHC to halt state before reset
  //
  HcHaltTimeout = 0;
  if (!(MmioRead32 (XhciMmioBase + R_PCH_XHCI_USBSTS) & BIT0)) {
    Data32 = MmioRead32 (XhciMmioBase + R_PCH_XHCI_USBCMD);
    MmioWrite32 ((XhciMmioBase + R_PCH_XHCI_USBCMD), (Data32 &~B_PCH_XHCI_USBCMD_RS));
    while ((HcHaltTimeout < 200) &&
           (!(MmioRead32 (XhciMmioBase + R_PCH_XHCI_USBSTS) & BIT0))) {
      PchPmTimerStall (100);
      HcHaltTimeout++;
    }
  }

  if (MmioRead32 (XhciMmioBase + R_PCH_XHCI_USBSTS) & BIT0) {
    MmioOr16 ((XhciMmioBase + R_PCH_XHCI_USBCMD), BIT1);
  }
}

#endif  // USB_PRECONDITION_ENABLE_FLAG
