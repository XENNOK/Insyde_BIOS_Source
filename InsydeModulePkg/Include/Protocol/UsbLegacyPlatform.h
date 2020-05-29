/** @file
  Protocol used to get the platform policy on USB legacy operation.

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

#ifndef _USB_LEGACY_PLATFORM_H_
#define _USB_LEGACY_PLATFORM_H_

#include <Uefi.h>

#define EFI_USB_LEGACY_PLATFORM_PROTOCOL_GUID \
  {0x13f40f6e, 0x50c1, 0x4b73, 0xb1, 0xe2, 0x6e, 0x72, 0xd2, 0x1c, 0xb0, 0x4a}

typedef struct _EFI_USB_LEGACY_PLATFORM_PROTOCOL EFI_USB_LEGACY_PLATFORM_PROTOCOL;

typedef struct {
    UINT16            ShiftKey     :1;
    UINT16            AltKey       :1;
    UINT16            CtrlKey      :1;
    UINT16            Reserved     :13;
} KEY_ATTRIBUTE;

typedef struct {
    UINT8            ScanCode;
    KEY_ATTRIBUTE    Keyattribute;
} KEY_ELEMENT;

// UsbLegacyEnable
//    0b = Disabled
//    1b = Enabled
//
// UsbBoot
//    00b = Not supported
//    01b = Disabled
//    10b = Enabled
//    11b = Reserved
//
// UsbZip  
//    (No longer used due to always supported in USB stack)
//
// Usb Zip Emulation
//    (No longer used due to auto detected in USB stack)
//
// UsbFixedDiskWpBootSector
//    00b = Not supported
//    01b = Disabled
//    10b = Enabled
//    11b = Reserved
//
// UsbEhciSupport
//    0b = USB 2.0 unsupported
//    1b = USB 2.0 supported
//
// UsbSupportSingleLunOnly
//    0b = Support USB multiple logical unit mass storage device
//    1b = Support USB single logical unit mass storage device only
//
// UsbUefiOnly
//    0b = Support USB keyboard/mouse/storage under both of UEFI and Legacy
//    1b = Support USB keyboard/mouse/storage under UEFI only
//
// UsbIgnoreDevicesPtr
//    Device path list for ignored USB HC/port/devices
//

typedef struct {
    UINT32            UsbLegacyEnable          :1;
    UINT32            UsbBoot                  :2;
    UINT32            UsbZip                   :2;
    UINT32            UsbZipEmulation          :2;
    UINT32            UsbFixedDiskWpBootSector :2;
    UINT32            UsbEhciSupport           :1;
    UINT32            UsbSupportSingleLunOnly  :1;
    UINT32            UsbUefiOnly              :1;
    UINT32            Reserved                 :20;
    UINT32            UsbIgnoreDevicesPtr;
} USB_LEGACY_MODIFIERS;

/**

  Get SETUP/platform options for USB Legacy operation modification.

  @param  This                  Protocol instance pointer.
  @param  UsbLegacyModifiers    List of keys to monitor from. This includes both
                                PS2 and USB keyboards.

  @retval EFI_SUCCESS           Modifiers exist.
  @retval EFI_NOT_FOUND         Modifiers not not exist.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_GET_USB_PLATFORM_OPTIONS) (
  IN   EFI_USB_LEGACY_PLATFORM_PROTOCOL *This,
  OUT  USB_LEGACY_MODIFIERS           *UsbLegacyModifiers
  );

/**

  Return a list of keys to monitor for.

  @param  This                  Protocol instance pointer.
  @param  KeyList               List of keys to monitor from. This includes both
                                USB & PS2 keyboard inputs.
  @param  KeyListSize           Size of KeyList in bytes

  @retval EFI_SUCCESS           Keys are to be monitored.
  @retval EFI_NOT_FOUND         No keys are to be monitored.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_GET_PLATFORM_MONITOR_KEY_OPTIONS) (
  IN   EFI_USB_LEGACY_PLATFORM_PROTOCOL *This,
  OUT  KEY_ELEMENT                    **KeyList,
  OUT  UINTN                          *KeyListSize
  );

struct _EFI_USB_LEGACY_PLATFORM_PROTOCOL{
  EFI_GET_USB_PLATFORM_OPTIONS           GetUsbPlatformOptions;
  EFI_GET_PLATFORM_MONITOR_KEY_OPTIONS   GetPlatformMonitorKeyOptions;
};

extern EFI_GUID gEfiUsbLegacyPlatformProtocolGuid;

#endif
