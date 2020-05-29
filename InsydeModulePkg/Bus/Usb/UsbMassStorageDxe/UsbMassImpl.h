/** @file
  The implementation of USB mass storage class device driver.

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

#ifndef _EFI_USBMASS_IMPL_H_
#define _EFI_USBMASS_IMPL_H_

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DevicePathLib.h>
#include <Library/DebugLib.h>
#include <Protocol/DriverBinding.h>
#include <Protocol/DevicePath.h>
#include <Protocol/UsbCore.h>
#include <Protocol/DiskInfo.h>
#include <Protocol/UsbLegacyPlatform.h>
#include "UsbMassBoot.h"
#include "UsbVendorSpec.h"

#define IMAGE_DRIVER_NAME               L"Usb Mass Storage Driver"
#define USB_MASS_SIGNATURE              SIGNATURE_32 ('U', 's', 'b', 'K')

struct _USB_MASS_DEVICE {
  UINT32                      Signature;
  EFI_HANDLE                  Controller;
  EFI_USB_IO_PROTOCOL         *UsbIo;
  EFI_BLOCK_IO_PROTOCOL       BlockIo;
  EFI_BLOCK_IO_MEDIA          BlockIoMedia;
  EFI_DISK_INFO_PROTOCOL      DiskInfo;
  UINT8                       OpticalStorage : 1;
  UINT8                       ScsiInterface : 1;
  UINT8                       Reserved : 6;
  UINT8                       Lun;          // Logical Unit Number
  UINT8                       Pdt;          // Peripheral Device Type
  UINT8                       MediumType;   // MediumType for FDD 720/1.44
  USB_MASS_TRANSPORT_PROTOCOL *Transport;   // USB mass storage transport protocol
  VOID                        *Context;     // Opaque storage for mass transport
  USB_BOOT_INQUIRY_DATA       InquiryData;
  EFI_UNICODE_STRING_TABLE    *DeviceNameTable;
  EFI_DEVICE_PATH_PROTOCOL    *DevicePath;
  EFI_USB_CORE_PROTOCOL       *UsbCore;
};

#define USB_MASS_DEVICE_FROM_BLOCKIO(a) \
        CR (a, USB_MASS_DEVICE, BlockIo, USB_MASS_SIGNATURE)

#define USB_MASS_DEVICE_FROM_DISK_INFO(a) \
        CR (a, USB_MASS_DEVICE, DiskInfo, USB_MASS_SIGNATURE)

extern EFI_DRIVER_BINDING_PROTOCOL  gUsbMassDriverBinding;
extern EFI_COMPONENT_NAME_PROTOCOL  gUsbMassStorageComponentName;
extern EFI_COMPONENT_NAME2_PROTOCOL gUsbMassStorageComponentName2;

#endif
