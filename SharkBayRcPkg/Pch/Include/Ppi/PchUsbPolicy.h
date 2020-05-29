/**
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement

@copyright
  Copyright (c) 2009 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  PchUsbPolicy.h

@brief
  PCH Usb policy PPI produced by a platform driver specifying
  various expected PCH Usb settings. This PPI is consumed by the
  PCH PEI drivers.

**/
#ifndef _PCH_USB_POLICY_H_
#define _PCH_USB_POLICY_H_

///
/// PCH Usb policy provided by platform for PEI phase
///
///
/// EDK and EDKII have different GUID formats
///
//[-start-120911-IB10820121-modify]//
//#if !defined(EDK_RELEASE_VERSION) || (EDK_RELEASE_VERSION < 0x00020000)
#include <PchAccess.h>
#include <Ppi/PchPlatformPolicy.h>
//#endif

#define PCH_USB_POLICY_PPI_GUID \
  { \
    0x887acae1, 0x6a8c, 0x4eee, \
    { \
      0x97, 0xd, 0x91, 0x12, 0xda, 0x63, 0xbc, 0xf1 \
    } \
  }
//#endif
//[-end-120911-IB10820121-modify]//
extern EFI_GUID                     gPchUsbPolicyPpiGuid;

typedef struct _PCH_USB_POLICY_PPI  PCH_USB_POLICY_PPI;

///
/// PPI revision number
/// Any backwards compatible changes to this PPI will result in an update in the revision number
/// Major changes will require publication of a new PPI
///
/// Revision 1: Original version
///
#define PCH_USB_POLICY_PPI_REVISION_1 1
///
/// Revision 2: Add ManualMode, ManualModeUsb20PerPinRoute and ManualModeUsb30PerPinEnable
///             to PCH_USB30_CONTROLLER_SETTINGS
///             Deprecated XhciStreams of PCH_USB30_CONTROLLER_SETTINGS
///
#define PCH_USB_POLICY_PPI_REVISION_2 2
///
/// Revision 3: Add UsbPrecondition in UsbConfig
///
#define PCH_USB_POLICY_PPI_REVISION_3 3
///
/// Revision 4: Add XhciIdleL1 to PCH_USB30_CONTROLLER_SETTINGS
///
#define PCH_USB_POLICY_PPI_REVISION_4 4

//
// Generic definitions for device enabling/disabling used by PCH code.
//
#define PCH_DEVICE_ENABLE   1
#define PCH_DEVICE_DISABLE  0

#define EHCI_MODE           1

///
/// PCH Usb policy PPI produced by a platform driver specifying various expected
/// PCH Usb settings that would be used by PCH_INIT_PPI.UsbInit () and USB PEI module.
/// This PPI needs to be installed before calling PCH_INIT_PPI.UsbInit ().
///
struct _PCH_USB_POLICY_PPI {
  ///
  /// This member specifies the revision of the PEI PCH USB Policy PPI.
  /// This field is used to indicate backwards compatible changes to the protocol.
  /// Platform code that produces this PPI must fill with the correct revision value
  /// for the PCH reference code to correctly interpret the content of the PPI fields.
  ///
  UINT8           Revision;
  ///
  /// This member describes USB controller's related configuration.
  ///
  PCH_USB_CONFIG  *UsbConfig;
  ///
  /// This member decides which USB controller needs to be initialed and allocated
  /// resource in Pei Phase. It will be referred by USB PEI module.
  /// For RMH enabled, please set this field to EHCI_MODE.
  ///
  UINT8           Mode;
  ///
  /// This member describes EHCI memory base address. USB PEI module will refer to
  /// this field to program memory base address of each EHCI controllers.
  ///
  UINTN           EhciMemBaseAddr;
  ///
  /// This member describes EHCI memory length. USB PEI module will check this field
  /// to determine if the memory resource is less than the required. Each EHCI controller
  /// requires 0x400 memory space.
  ///
  UINT32          EhciMemLength;
  ///
  /// This member describes XHCI memory base address. USB PEI module will refer to
  /// this field to program memory base address of the XHCI controller.
  ///
  UINTN           XhciMemBaseAddr;
};

#endif
