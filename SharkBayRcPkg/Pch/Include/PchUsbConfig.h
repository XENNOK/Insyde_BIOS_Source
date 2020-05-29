/** @file
  General USB Configurate data structure and register definitions.

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
#ifndef _PCH_USB_CONFIG_H_
#define _PCH_USB_CONFIG_H_

///
/// ---------------------------- USB Config -----------------------------
///
///
/// Overcurrent pins, the values match the setting of PCH EDS, please refer to PCH EDS for more details
///
typedef enum {
  PchUsbOverCurrentPin0 = 0,
  PchUsbOverCurrentPin1,
  PchUsbOverCurrentPin2,
  PchUsbOverCurrentPin3,
  PchUsbOverCurrentPin4,
  PchUsbOverCurrentPin5,
  PchUsbOverCurrentPin6,
  PchUsbOverCurrentPin7,
  PchUsbOverCurrentPinSkip,
  PchUsbOverCurrentPinMax
} PCH_USB_OVERCURRENT_PIN;

//
// The location of the USB connectors.  This information is use to decide eye diagram tuning value for Usb 2.0 motherboard trace.
//
typedef enum {
  PchUsbPortLocationBackPanel = 0,
  PchUsbPortLocationFrontPanel,
  PchUsbPortLocationDock,
  PchUsbPortLocationMiniPciE,
  PchUsbPortLocationFlex,
  PchUsbPortLocationInternalTopology,
  PchUsbPortLocationSkip,
  PchUsbPortLocationMax
} PCH_USB_PORT_LOCATION;

typedef struct {
  UINT8   Enable            : 1;    ///< 0: Disable; 1: Enable. This would take effect while UsbPerPortCtl is enabled
  UINT8   Rsvdbits          : 7;
  UINT8   Location;                    // The location of the USB connectors. Please use the enum PCH_USB_PORT_LOCATION.
  UINT16  Usb20PortLength;             // The length of Usb Port to configure the USB transmitter, bits [16:4] represents length of Usb Port in inches using octal format and [3:0] is for the decimal Point.
  UINT8   Usb20EyeDiagramTuningParam1; // Set IOBP registers 0xE5004000 + (PortNumber * 0x100)[10:08] = {0,1,2,3,4,5,6,7}
  UINT8   Usb20EyeDiagramTuningParam2; // Set IOBP registers 0xE5004000 + (PortNumber * 0x100)[13:11] = {0,1,2,3,4,5,6,7}
} PCH_USB_PORT_SETTINGS;

typedef struct {
  UINT8   Enable            : 1;    ///< 0: Disable; 1: Enable. This would take effect while UsbPerPortCtl is enabled
  UINT8   Rsvdbits          : 7;    ///< Reserved fields for future expansion w/o protocol change;
} PCH_USB30_PORT_SETTINGS;

typedef struct {
  UINT8 Enable              : 1;    ///< 0: Disable; 1: Enable
  UINT8 Rsvdbits            : 7;
} PCH_USB20_CONTROLLER_SETTINGS;

#define PCH_XHCI_MODE_OFF         0
#define PCH_XHCI_MODE_ON          1
#define PCH_XHCI_MODE_AUTO        2
#define PCH_XHCI_MODE_SMARTAUTO   3

#define PCH_XHCI_STREAMS_OFF      0
#define PCH_XHCI_STREAMS_ON       1

#define EHCI_PRECONDITION(Device, EhciMmioBase) \
          EhciPrecondition(Device, EhciMmioBase)
#define XHCI_PRECONDITION(BusNumber, Device, Function, XhciMmioBase, USB2Ptr, HsPortCount, USB3Ptr, SsPortCount) \
          XhciPrecondition(BusNumber, Device, Function, XhciMmioBase, USB2Ptr, HsPortCount, USB3Ptr, SsPortCount)

typedef struct {
  UINT8 Mode               : 2;    ///< 0: Disable; 1: Enable, 2: Auto, 3: Smart Auto
  UINT8 PreBootSupport     : 1;    ///< 0: No xHCI driver available; 1: xHCI driver available
  UINT8 XhciStreams        : 1;    ///< OBSOLETE from Revision 2 !!! DO NOT USE !!!
  UINT8 ManualMode         : 1;    ///< 0: Disable; 1: Enable Manual Mode
  UINT8 XhciIdleL1         : 1;    ///< 0: Disable; 1: Enable
  UINT8 Btcg               : 1;    ///<.0:.Disable; 1: Enable trunk clock gating
  UINT8 Rsvdbits           : 1;
  UINT8 ManualModeUsb20PerPinRoute[LPTH_USB_MAX_PHYSICAL_PORTS]; ///< 0: EHCI; 1 :XHCI;
  UINT8 ManualModeUsb30PerPinEnable[LPTH_XHCI_MAX_USB3_PORTS];   ///< 0: Disable; 1:Enable;
} PCH_USB30_CONTROLLER_SETTINGS;

///
/// This member describes the expected configuration of the PCH USB controllers,
/// Platform modules may need to refer Setup options, schematic, BIOS specification
/// to update this field.
/// The Usb20OverCurrentPins and Usb30OverCurrentPins field must be updated by refer
/// the schematic.
///
typedef struct {
  ///
  /// This member describes whether the USB per-port controlling feature of PCH is
  /// enabled by platform modules. If enabled, the USB Port N of PCH can be
  /// enabled/disabled by setting PortSettings[N] to enabled/disabled.
  ///
  UINT8 UsbPerPortCtl       : 1;    ///< 0: Disable; 1: Enable Per-port enable control
  ///
  /// This member describes whether or not EHCI 1 USBR should be enabled.
  ///
  UINT8 Ehci1Usbr           : 1;    ///< 0: Disable; 1: Enable EHCI 1 USBR
  ///
  /// This member describes whether or not EHCI 2 USBR should be enabled.
  ///
  UINT8 Ehci2Usbr           : 1;    ///< 0: Disable; 1: Enable EHCI 2 USBR
  UINT8 RsvdBits            : 5;
  ///
  /// These members describe whether the USB2 Port N of PCH is enabled by platform modules.
  /// They would take effect while UsbPerPortCtl is enabled. Panel and Dock are used to
  /// describe the layout of USB port. Panel is only available for Desktop LPT.
  /// Dock is only available for Mobile LPT.
  ///
  PCH_USB_PORT_SETTINGS         PortSettings[LPTH_USB_MAX_PHYSICAL_PORTS];
  ///
  /// These members describe whether the USB 2.0 controller N of PCH is enabled by
  /// platform modules.
  ///
  PCH_USB20_CONTROLLER_SETTINGS Usb20Settings[PchEhciControllerMax];
  ///
  /// These members describe some settings which are related to the USB 3.0 controller.
  /// While ManualMode is set to 1, ManualModeUsb20PerPinRoute[] and ManualModeUsb30PerPinEnable[]
  /// need to be programmed properly per the platform design.
  /// While ManualModeUsb20PerPinRoute[] is set to 1, it means routes USB2 pins to the
  /// XHCI controller. ManualModeUsb30PerPinEnable[] is used to control whether Super Speed
  /// capability is enabled for a given USB3 port.
  ///
  PCH_USB30_CONTROLLER_SETTINGS Usb30Settings;
  ///
  /// These members describe the specific over current pin number of USB 2.0 Port N.
  /// It is SW's responsibility to ensure that a given port's bit map is set only for
  /// one OC pin Description. USB2 and USB3 on the same combo Port must use the same
  /// OC pin.
  ///
  PCH_USB_OVERCURRENT_PIN       Usb20OverCurrentPins[LPTH_USB_MAX_PHYSICAL_PORTS];
  ///
  /// These members describe the specific over current pin number of USB 3.0 Port N.
  /// It is SW's responsibility to ensure that a given port's bit map is set only for
  /// one OC pin Description. USB2 and USB3 on the same combo Port must use the same
  /// OC pin.
  ///
  PCH_USB_OVERCURRENT_PIN       Usb30OverCurrentPins[LPTH_XHCI_MAX_USB3_PORTS];
  ///
  /// This feature intends to reduce the necessary initialization time for USB HC
  /// and devices on root ports. It is assembled by PCHInit drivers in PEI and DXE phase.
  /// In PEI phase, the feature resets all USB HCs on PCH bus, including Intel EHCI
  /// and XHCI. After reset USB HC, continue the system initialization without waiting
  /// for the USB XHC reset ready. After running to DXE phase, the feature resets
  /// those USB devices installed on each USB HC root port in parallel, including RMH
  /// on EHCI root port 0 and any non USB3 speed devices on XHCI root port if XHCI is
  /// enabled. For USB3 protocol root port, USB3 speed devices will be advanced to
  /// enable state if link training succeeds after XHC reset.
  ///
  BOOLEAN                       UsbPrecondition;
  ///
  /// These members describe whether the USB3 Port N of PCH is enabled by platform modules.
  /// They would take effect while UsbPerPortCtl is enabled.
  ///
  PCH_USB30_PORT_SETTINGS       Port30Settings[LPTH_XHCI_MAX_USB3_PORTS];
} PCH_USB_CONFIG;

#endif
