/** @file
  This function provides an interface to modify USB legacy options.

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

#include <Library/DxeOemSvcChipsetLib.h>
#if 0 // Sample Implementation
#include <Protocol/UsbLegacyPlatform.h>
#endif
//[-start-121128-IB05280020-modify]//
/**
 This function provides an interface to modify USB legacy options.

 @param[in out]     *UsbLegacyModifiers On entry, points to USB_LEGACY_MODIFIERS instance.
                                        On exit , points to updated USB_LEGACY_MODIFIERS instance.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
*/
EFI_STATUS
OemSvcSetUsbLegacyPlatformOptions (
  IN OUT  USB_LEGACY_MODIFIERS                *UsbLegacyModifiers
  )
//[-end-121128-IB05280020-modify]//
{
/*++
  Todo:
  Add project specific code in here.
--*/
#if 0 // Sample Implementation
  EFI_USB_LEGACY_PLATFORM_PROTOCOL    *LegacyBiosPlatform;
//
// Sets the UsbIgnoreDevicesPtr as a bunch of device paths to indicate which
// (1)Usb controller(s) (2)Usb port(s) (3)Usb device(s) you want to ignore
//
// 1. To ignore Usb controller : Insert a PCI_DEVICE_PATH to UsbIgnoreDevicesPtr 
//    and set appropriate value into "Function" and "Device" fields
//
// 2. To ignore Usb port : Insert a USB_DEVICE_PATH to UsbIgnoreDevicesPtr 
//    and set appropriate value into ParentPortNumber
//
// 3. To ignore Usb device : Insert a USB_CLASS_DEVICE_PATH to UsbIgnoreDevicesPtr 
//    and set appropriate value into "VendorId" and "ProductId" field
//
// 4. Every device path MUST end as a END_DEVICE_PATH in order to distinguish each other
//
// 5. Append 0(UINT32) after end of whole device paths to indicate the end of whole device paths
//
// Turn following code on to enable this feature
//
  //
  // The example of add a couple of Usb devices to ignore
  //
  static struct {
    USB_CLASS_DEVICE_PATH    UsbClass;
    EFI_DEVICE_PATH_PROTOCOL EndDP1;

    PCI_DEVICE_PATH          Pci;
    EFI_DEVICE_PATH_PROTOCOL EndDP2;

    USB_DEVICE_PATH          Usb;
    EFI_DEVICE_PATH_PROTOCOL EndDP3;

    USB_DEVICE_PATH          Usb1;
    USB_CLASS_DEVICE_PATH    UsbClass1;
    EFI_DEVICE_PATH_PROTOCOL EndDP4;

    PCI_DEVICE_PATH          Pci1;
    USB_DEVICE_PATH          Usb2;
    EFI_DEVICE_PATH_PROTOCOL EndDP5;

    PCI_DEVICE_PATH          Pci2;
    USB_DEVICE_PATH          Usb3;
    USB_CLASS_DEVICE_PATH    UsbClass2;
    EFI_DEVICE_PATH_PROTOCOL EndDP6;

    UINT32                   Zero;
  } UsbUnsupportDevicePaths = {
    //
    // Example 1 : Ignore device which VID = 0x0461 and PID = 0x4d03
    //
    { { MESSAGING_DEVICE_PATH, MSG_USB_CLASS_DP, sizeof(USB_CLASS_DEVICE_PATH), 0 }, 0x0461, 0x4d03, 0, 0, 0 },
    { END_DEVICE_PATH_TYPE, END_ENTIRE_DEVICE_PATH_SUBTYPE, sizeof(EFI_DEVICE_PATH_PROTOCOL), 0 },
    //
    // Example 2 : Ignore whole devices attached in Dev = 0x1d and Fun = 0x07 controller
    //
    { { HARDWARE_DEVICE_PATH, HW_PCI_DP, sizeof(PCI_DEVICE_PATH), 0 }, 0x07, 0x1d },
    { END_DEVICE_PATH_TYPE, END_ENTIRE_DEVICE_PATH_SUBTYPE, sizeof(EFI_DEVICE_PATH_PROTOCOL), 0 },
    //
    // Example 3 : Ignore whole devices plugged in each controller's port 2
    //
    { { MESSAGING_DEVICE_PATH, MSG_USB_DP, sizeof(USB_DEVICE_PATH), 0 }, 0x02, 0 },
    { END_DEVICE_PATH_TYPE, END_ENTIRE_DEVICE_PATH_SUBTYPE, sizeof(EFI_DEVICE_PATH_PROTOCOL), 0 },
    //
    // Example 4 : Ignore device which VID = 0x0461 and PID = 0x4d03 and plugged in each controller's port 2
    //
    { { MESSAGING_DEVICE_PATH, MSG_USB_DP, sizeof(USB_DEVICE_PATH), 0 }, 0x02, 0 },
    { { MESSAGING_DEVICE_PATH, MSG_USB_CLASS_DP, sizeof(USB_CLASS_DEVICE_PATH), 0 }, 0x0461, 0x4d03, 0, 0, 0 },
    { END_DEVICE_PATH_TYPE, END_ENTIRE_DEVICE_PATH_SUBTYPE, sizeof(EFI_DEVICE_PATH_PROTOCOL), 0 },
    //
    // Example 5 : Ignore whole device plugged in Dev = 0x1d and Fun = 0x07 controller's port 2
    //
    { { HARDWARE_DEVICE_PATH, HW_PCI_DP, sizeof(PCI_DEVICE_PATH), 0 }, 0x07, 0x1d },
    { { MESSAGING_DEVICE_PATH, MSG_USB_DP, sizeof(USB_DEVICE_PATH), 0 }, 0x02, 0 },
    { END_DEVICE_PATH_TYPE, END_ENTIRE_DEVICE_PATH_SUBTYPE, sizeof(EFI_DEVICE_PATH_PROTOCOL), 0 },
    //
    // Example 6 : Ignore device which VID = 0x0461 and PID = 0x4d03 and plugged in Dev = 0x1d and Fun = 0x07 controller's port 2
    //
    { { HARDWARE_DEVICE_PATH, HW_PCI_DP, sizeof(PCI_DEVICE_PATH), 0 }, 0x07, 0x1d },
    { { MESSAGING_DEVICE_PATH, MSG_USB_DP, sizeof(USB_DEVICE_PATH), 0 }, 0x02, 0 },
    { { MESSAGING_DEVICE_PATH, MSG_USB_CLASS_DP, sizeof(USB_CLASS_DEVICE_PATH), 0 }, 0x0461, 0x4d03, 0, 0, 0 },
    { END_DEVICE_PATH_TYPE, END_ENTIRE_DEVICE_PATH_SUBTYPE, sizeof(EFI_DEVICE_PATH_PROTOCOL), 0 },
    //
    // End of device paths
    //
    0
  };
  Status = gBS->LocateProtocol (
                  &gEfiUsbLegacyPlatformProtocolGuid,
                  NULL,
                  (VOID **)&LegacyBiosPlatform
                  );

  UsbLegacyModifiers->UsbIgnoreDevicesPtr = (UINT32)(UINTN)&UsbUnsupportDevicePaths;
#endif
  return EFI_UNSUPPORTED;
}

