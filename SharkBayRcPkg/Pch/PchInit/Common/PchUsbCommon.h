/** @file
  Header file for the PCH USB Common Driver.

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

/** 

  Header file for the PCH USB Common Driver.

@copyright
  Copyright (c) 2009 - 2012 Intel Corporation. All rights reserved
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
#ifndef _USB_COMMON_H_
#define _USB_COMMON_H_

#include <PchAccess.h>
#include <PchPlatformLib.h>
#include <PchInitCommon.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Protocol/PchPlatformPolicy.h>

#define USB_PR_CASE_0       0
#define USB_PR_CASE_1       1
#define USB_PR_CASE_2       2
#define TEN_MS_TIMEOUT      10000
#define PORT_RESET_TIMEOUT  10  ///< 100 ms timeout for xHCI port reset

typedef struct {
  UINT8   Device;
  UINT8   Function;
} USB_CONTROLLER;

/**
  Configures PCH USB controller

  @param[in] UsbConfig            The PCH Platform Policy for USB configuration
  @param[in] EhciMmioBase         Memory base address of EHCI Controller
  @param[in] XhciMmioBase         Memory base address of XHCI Controller
  @param[in] BusNumber            PCI Bus Number of the PCH device
  @param[in] RootComplexBar       RootComplexBar value of this PCH device
  @param[out] FuncDisableReg      Function Disable Register
  @param[in] Revision             The policy revision used for backward compatible check

  @retval EFI_INVALID_PARAMETER   The parameter of PchPlatformPolicy is invalid
  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
EFIAPI
CommonUsbInit (
  IN  PCH_USB_CONFIG              *UsbConfig,
  IN  UINT32                      EhciMmioBase,
  IN  UINT32                      XhciMmioBase,
  IN  UINT8                       BusNumber,
  IN  UINT32                      RootComplexBar,
  OUT UINT32                      *FuncDisableReg,
  IN  UINT8                       Revision
  );

/**
  Performs basic configuration of PCH EHCI controller.

  @param[in] UsbConfig            The PCH Platform Policy for USB configuration
  @param[in] EhciMmioBase         Memory base address of EHCI Controller
  @param[in] BusNumber            PCI Bus Number of the PCH device
  @param[in] Revision             The policy revision used for backward compatible check
  @param[in] LpcDeviceId          The device ID of LPC
  @param[in] RootComplexBar       RootComplexBar value of this PCH device

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
EFIAPI
CommonEhciHcsInit (
  IN  PCH_USB_CONFIG              *UsbConfig,
  IN  UINT32                      EhciMmioBase,
  IN  UINT8                       BusNumber,
  IN  UINT8                       Revision,
  IN  UINT16                      LpcDeviceId,
  IN  UINT32                      RootComplexBar
  );

/**
  Performs basic configuration of PCH USB3 (xHCI) controller.

  @param[in] UsbConfig            The PCH Platform Policy for USB configuration
  @param[in] XhciMmioBase         Memory base address of xHCI Controller
  @param[in] Revision             The policy revision used for backward compatible check
  @param[in] LpcDeviceId          The device ID of LPC
  @param[in] XhciPciMmBase        XHCI PCI Base Address

  @retval None
**/
VOID
CommonXhciHcInit (
  IN  PCH_USB_CONFIG              *UsbConfig,
  IN  UINT32                      XhciMmioBase,
  IN  UINT8                       Revision,
  IN  UINT16                      LpcDeviceId,
  IN  UINTN                       XhciPciMmBase
  );

/**
  Performs basic configuration of PCH USB3 (xHCI) controller.

  @param[in] UsbConfig            The PCH Platform Policy for USB configuration
  @param[in] XhciMmioBase         Memory base address of XHCI Controller
  @param[in] Revision             The policy revision used for backward compatible check
  @param[in] LpcDeviceId          The device ID of LPC
  @param[in] XhciPciMmBase        XHCI PCI Base Address
  @param[in] PciD31F0RegBase      LPC PCI Base Address

  @retval None
**/
VOID
PerformXhciEhciPortSwitchingFlow (
  IN  PCH_USB_CONFIG              *UsbConfig,
  IN  UINT32                      XhciMmioBase,
  IN  UINT8                       Revision,
  IN  UINT16                      LpcDeviceId,
  IN  UINTN                       XhciPciMmBase,
  IN  UINTN                       PciD31F0RegBase
  );

/**
  Retrieves information about number of implemented xHCI ports
  and sets appropriate port mask registers.

  @param[in]  XhciPciMmBase       XHCI PCI Base Address
  @param[out] HsPortCount         Count of High Speed Ports
  @param[out] HsUsbrPortCount     Count of USBr Port
  @param[out] SsPortCount         Count of Super Speed Ports

  @retval None
**/
VOID
GetXhciPortCountAndSetPortRoutingMask (
  IN  UINTN                       XhciPciMmBase,
  OUT UINTN                       *HsPortCount,
  OUT UINTN                       *HsUsbrPortCount,
  OUT UINTN                       *SsPortCount
  );

/**
  Setup XHCI Over-Current Mapping

  @param[in] UsbConfig            The PCH Platform Policy for USB configuration
  @param[in] XhciPciMmBase        XHCI PCI Base Address

  @retval None
**/
VOID
XhciOverCurrentMapping (
  IN  PCH_USB_CONFIG              *UsbConfig,
  IN  UINTN                       XhciPciMmBase
  );

/**
  Setup EHCI Over-Current Mapping

  @param[in] UsbConfig            The PCH Platform Policy for USB configuration
  @param[in] Ehci1PciMmBase       EHCI 1 PCI Base Address
  @param[in] Ehci2PciMmBase       EHCI 2 PCI Base Address

  @retval None
**/
VOID
EhciOverCurrentMapping (
  IN  PCH_USB_CONFIG              *UsbConfig,
  IN  UINTN                       Ehci1PciMmBase,
  IN  UINTN                       Ehci2PciMmBase
  );

/**
  Program Ehci Port Disable Override

  @param[in] UsbConfig            The PCH Platform Policy for USB configuration
  @param[in] Ehci1PciMmBase       EHCI 1 PCI Base Address
  @param[in] Ehci2PciMmBase       EHCI 2 PCI Base Address

  @retval None
**/
VOID
EhciPortDisableOverride (
  IN  PCH_USB_CONFIG              *UsbConfig,
  IN  UINTN                       Ehci1PciMmBase,
  IN  UINTN                       Ehci2PciMmBase
  );

/**
  Program Xhci Port Disable Override

  @param[in] UsbConfig            The PCH Platform Policy for USB configuration
  @param[in] XhciPciMmBase        XHCI PCI Base Address
  @param[in] Revision             Platform policy revision

  @retval None
**/
VOID
XhciPortDisableOverride (
  IN  PCH_USB_CONFIG              *UsbConfig,
  IN  UINTN                       XhciPciMmBase,
  IN  UINT8                       Revision
  );

/**
  Enable EHCI USBR device

  @param[in] EhciPciMmBase        Ehci PCI Base Address

  @retval None
**/
VOID
EhciUsbrEnable (
  IN  UINTN                       EhciPciMmBase
  );

/**
  Program and enable XHCI Memory Space

  @param[in] UsbConfig            The PCH Platform Policy for USB configuration
  @param[in] XhciMmioBase         Memory base address of XHCI Controller
  @param[in] XhciPciMmBase        XHCI PCI Base Address

  @retval None
**/
VOID
XhciMemorySpaceOpen (
  IN  PCH_USB_CONFIG              *UsbConfig,
  IN  UINT32                      XhciMmioBase,
  IN  UINTN                       XhciPciMmBase
  );

/**
  Clear and disable XHCI Memory Space

  @param[in] UsbConfig            The PCH Platform Policy for USB configuration
  @param[in] XhciMmioBase         Memory base address of XHCI Controller
  @param[in] XhciPciMmBase        XHCI PCI Base Address

  @retval None
**/
VOID
XhciMemorySpaceClose (
  IN  PCH_USB_CONFIG              *UsbConfig,
  IN  UINT32                      XhciMmioBase,
  IN  UINTN                       XhciPciMmBase
  );

/**
  Tune the USB 2.0 high-speed signals quality.

  @param[in] UsbConfig            The PCH Platform Policy for USB configuration
  @param[in] LpcDeviceId          The device ID of LPC
  @param[in] RootComplexBar       RootComplexBar value of this PCH device

  @retval None
**/
VOID
Usb2PortLengthProgramming (
  IN  PCH_USB_CONFIG              *UsbConfig,
  IN  UINT16                      LpcDeviceId,
  IN  UINT32                      RootComplexBar
  );

/**
  Initialization USB Clock Gating registers

  @param[in] PchPlatformPolicy    The PCH Platform Policy
  @param[in] RootComplexBar       RootComplexBar value of this PCH device

  @retval None
**/
VOID
ConfigureUsbClockGating (
  IN     DXE_PCH_PLATFORM_POLICY_PROTOCOL    *PchPlatformPolicy,
  IN     UINT32                               RootComplexBar
  );

/**
  Initialization EHCI Clock Gating registers

  @param[in] PchPlatformPolicy    The PCH Platform Policy
  @param[in] RootComplexBar       RootComplexBar value of this PCH device

  @retval None
**/
VOID
ConfigureEhciClockGating (
  IN     DXE_PCH_PLATFORM_POLICY_PROTOCOL    *PchPlatformPolicy,
  IN     UINT32                               RootComplexBar
  );

/**
  Initialization XHCI Clock Gating registers

  @param[in] PchPlatformPolicy    The PCH Platform Policy
  @param[in] RootComplexBar       RootComplexBar value of this PCH device

  @retval None
**/
VOID
ConfigureXhciClockGating (
  IN     DXE_PCH_PLATFORM_POLICY_PROTOCOL    *PchPlatformPolicy,
  IN     UINT32                               RootComplexBar
  );

#ifdef USB_PRECONDITION_ENABLE_FLAG
/**
  Perform USB precondition on EHCI, it is the HC on USB HC in PEI phase;
  it is the root port reset on installed USB device in DXE phase

  @param[in]  Device              The device number of the EHCI
  @param[in]  EhciMmioBase        Memory base address of EHCI Controller

  @retval     None
**/
VOID
EhciPrecondition (
  IN  UINT8                       Device,
  IN  UINT32                      EhciMmioBase
  );

/**
  Perform USB precondition on XHCI, it is the HC on USB HC in PEI phase;
  it is the root port reset on installed USB device in DXE phase

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
  );
#endif  // USB_PRECONDITION_ENABLE_FLAG

#endif
