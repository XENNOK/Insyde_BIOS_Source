/** @file
  GenericBdsLib

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/**
  BDS Lib functions which relate with create or process the boot option.

Copyright (c) 2004 - 2011, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "InternalBdsLib.h"
#include "MemoryCheck.h"
#include "String.h"
#include <Guid/ReturnFromImage.h>

#include <Guid/ConsoleOutDevice.h>
#include <Guid/Gpt.h>

BOOLEAN        mEnumBootDevice = FALSE;
LIST_ENTRY     mWindowsToGoDeviceList;
EFI_HII_HANDLE gBdsLibStringPackHandle = NULL;

//
//BDS General Uefi Boot OS table,
//that only support non-removable Media for UEFI OS
//  Windows(Win7,Win2008),
//  Redhat Linux,
//  SuSE Linux,
//  Ubuntu,
//  Android,
// Others UEFI OS could add in this table to be supported.
//
#define SECURE_BOOT_ENABLED       0x00
#define SECURE_BOOT_DISABLED      0x01
#define MAX_BIOS_ENVIRONMENT      0x02
typedef BDS_GENERAL_UEFI_BOOT_OS  BDS_GENERAL_UEFI_BOOT_ARRAY_OS[MAX_BIOS_ENVIRONMENT];
//
// {{SECURE_BOOT_ENABLED BDS_GENERAL_UEFI_BOOT_OS}, {SECURE_BOOT_DISABLED BDS_GENERAL_UEFI_BOOT_OS}}
// ...
//
BDS_GENERAL_UEFI_BOOT_ARRAY_OS mBdsGeneralUefiBootOs[] = {
  {{L"\\EFI\\Microsoft\\Boot\\bootmgfw.efi",          L"Windows Boot Manager"},{L"\\EFI\\Microsoft\\Boot\\bootmgfw.efi", L"Windows Boot Manager"}},
  {{L"\\EFI\\redhat\\grub.efi",                       L"Red Hat Linux"},       {L"\\EFI\\redhat\\grub.efi",              L"Red Hat Linux"}},
  {{L"\\EFI\\SuSE\\elilo.efi",                        L"SuSE Linux"},          {L"\\EFI\\SuSE\\elilo.efi",               L"SuSE Linux"}},
  {{UBUNTU_BOOT_SECURE_BOOT_WITH_PROCESSOR_FILE_NAME, L"ubuntu"},              {UBUNTU_BOOT_WITH_PROCESSOR_FILE_NAME,    L"ubuntu"}},
  {{L"\\EFI\\ubuntu\\shim.efi",                       L"ubuntu"},              {L"\\EFI\\ubuntu\\grub.efi",              L"ubuntu"}},
  {{L"\\EFI\\fedora\\shim.efi",                       L"Fedora"},              {L"\\EFI\\fedora\\shim.efi",              L"Fedora"}},
  {{L"\\EFI\\android\\bootx64.efi",                   L"Android"},             {L"\\EFI\\android\\bootx64.efi",          L"Android"}},
  {{L"\\EFI\\Boot\\grubx64.efi",                      L"Linpus lite"},         {L"\\EFI\\Boot\\grubx64.efi",             L"Linpus lite"}},
  };


DUMMY_BOOT_OPTION_INFO  mDummyBootOptionTable[] = {
  {DummyUsbBootOptionNum,     L"EFI USB Device"},
  {DummyCDBootOptionNum,      L"EFI DVD/CDROM"},
  {DummyNetwokrBootOptionNum, L"EFI Network"}
  };

#define SHELL_ENVIRONMENT_INTERFACE_PROTOCOL \
  { 0x47c7b221, 0xc42a, 0x11d2, 0x8e, 0x57, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b }

VOID *mShellImageCallbackReg = NULL;

VOID
EFIAPI
ShellImageCallback (
  IN EFI_EVENT                          Event,
  IN VOID                               *Context
  )
;

/**
  Create dummy boot options which use to selected by UEFI OS.

  @retval EFI_SUCCESS   Create dummy boot options successful.
  @retval Other         Any error occured while creating dummy boot options.

**/
EFI_STATUS
CreateDummyBootOptions (
  VOID
  )
;

/**
  Synchronize the contents of "BootOrder" to prevent from the contents is
  updated by SCU or UEFI OS.

  @retval EFI_SUCCESS   Synchronize data successful.
  @retval Other         Set "BootOrder" varible failed.

**/
EFI_STATUS
SyncBootOrder (
  VOID
  )
;

VOID
SignalImageReturns (
  VOID
  )
;

/**
  The constructor function register UNI strings into imageHandle.

  It will ASSERT() if that operation fails and it will always return EFI_SUCCESS.

  @param  ImageHandle   The firmware allocated handle for the EFI image.
  @param  SystemTable   A pointer to the EFI System Table.

  @retval EFI_SUCCESS   The constructor successfully added string package.
  @retval Other value   The constructor can't add string package.

**/
EFI_STATUS
EFIAPI
GenericBdsLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  gBdsLibStringPackHandle = HiiAddPackages (
                              &gBdsLibStringPackageGuid,
                              ImageHandle,
                              GenericBdsLibStrings,
                              NULL
                              );

  ASSERT (gBdsLibStringPackHandle != NULL);
  if (gBdsLibStringPackHandle == NULL) {
    return EFI_ABORTED;
  }

  UpdateBvdtToHii (gBdsLibStringPackHandle);

  return EFI_SUCCESS;
}



/**
  Unsupport the CSM Opt-out when switch disabled.

  @param  Event                The triggered event.

  @retval EFI_UNSUPPORTED

**/
EFI_STATUS
LegacyBiosDependency (
  IN EFI_EVENT            Event
  )
{
  return EFI_UNSUPPORTED;
}



/**
  Check the Device path is Win Boot Manager.

  @param  DevicePath           boot device path

  @retval TRUE                 The device path is Win Boot Manager.
  @retval FALSE                The device path is not Win Boot Manager.

**/
BOOLEAN
IsWinBootManager (
  IN  EFI_DEVICE_PATH_PROTOCOL *DevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL *TempDevicePath;
  EFI_DEVICE_PATH_PROTOCOL *FilePath;
  UINTN                    DevPathSize1;
  UINTN                    DevPathSize2;
  EFI_HANDLE               Handle;
  EFI_STATUS               Status;
  INTN                     Result;
  CHAR16                   *BootLoaderPathString = L"\\EFI\\Microsoft\\Boot\\bootmgfw.efi";

  TempDevicePath = DevicePath;

  Status = gBS->LocateDevicePath (
                  &gEfiSimpleFileSystemProtocolGuid,
                  &TempDevicePath,
                  &Handle
                  );

  FilePath = FileDevicePath (Handle, BootLoaderPathString);

  ASSERT (FilePath);
  ASSERT (DevicePath);

  DevPathSize1  = GetDevicePathSize (FilePath);
  DevPathSize2  = GetDevicePathSize (DevicePath);

  if (DevPathSize1 != DevPathSize2) {
    return FALSE;
  } else {
    Result = CompareMem (FilePath, DevicePath, DevPathSize1);
    if (Result == 0) {
      return TRUE;
    } else {
      return FALSE;
    }
  }
}


/**
  Boot the legacy system with the boot option

  @param  Option                 The legacy boot option which have BBS device path

  @retval EFI_UNSUPPORTED        There is no legacybios protocol, do not support
                                 legacy boot.
  @retval EFI_STATUS             Return the status of LegacyBios->LegacyBoot ().

**/
EFI_STATUS
BdsLibDoLegacyBoot (
  IN  BDS_COMMON_OPTION           *Option
  )
{
  EFI_STATUS                Status;
  EFI_LEGACY_BIOS_PROTOCOL  *LegacyBios;

  Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (VOID **) &LegacyBios);
  if (EFI_ERROR (Status)) {
    //
    // If no LegacyBios protocol we do not support legacy boot
    //
    return EFI_UNSUPPORTED;
  }
  //
  // Notes: if we separate the int 19, then we don't need to refresh BBS
  //
  // to set BBS Table priority
  //
  SetBbsPriority (LegacyBios, Option);
  //
  // Write boot to OS performance data for legacy boot.
  //
  WRITE_BOOT_TO_OS_PERFORMANCE_DATA;

  DEBUG ((DEBUG_INFO | DEBUG_LOAD, "Legacy Boot: %S\n", Option->Description));
  return LegacyBios->LegacyBoot (
                      LegacyBios,
                      (BBS_BBS_DEVICE_PATH *) Option->DevicePath,
                      Option->LoadOptionsSize,
                      Option->LoadOptions
                      );
}


/**
  Get messaging device path sub type

  @param  Option               The current processing boot option.
  @param  HardDriveDevicePath  EFI Device Path to boot, if it starts with a hard
                               drive device path.

  @return messaging device path sub type

**/
UINT8
BdsLibGetMessagingDevicePathSubType (
  IN  BDS_COMMON_OPTION          * Option,
  IN  HARDDRIVE_DEVICE_PATH      * HardDriveDevicePath
  )
{
  EFI_STATUS                Status;
  UINTN                     BlockIoHandleCount;
  EFI_HANDLE                *BlockIoBuffer;
  EFI_DEVICE_PATH_PROTOCOL  *BlockIoDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *NewDevicePath;
  UINTN                     Index;

  if (!((DevicePathType (&HardDriveDevicePath->Header) == MEDIA_DEVICE_PATH) &&
        (DevicePathSubType (&HardDriveDevicePath->Header) == MEDIA_HARDDRIVE_DP))) {
    //
    // If the HardDriveDevicePath does not start with a Hard Drive Device Path
    // exit.
    //
    return 0;
  }
  //
  // The boot device have already been connected
  //
  Status = gBS->LocateHandleBuffer (ByProtocol, &gEfiBlockIoProtocolGuid, NULL, &BlockIoHandleCount, &BlockIoBuffer);
  if (EFI_ERROR (Status) || BlockIoHandleCount == 0) {
    //
    // If there was an error or there are no device handles that support
    // the BLOCK_IO Protocol, then return.
    //
    return 0;
  }
  //
  // Loop through all the device handles that support the BLOCK_IO Protocol
  //
  for (Index = 0; Index < BlockIoHandleCount; Index++) {
    Status = gBS->HandleProtocol (BlockIoBuffer[Index], &gEfiDevicePathProtocolGuid, (VOID *) &BlockIoDevicePath);
    if (EFI_ERROR (Status) || BlockIoDevicePath == NULL) {
      continue;
    }
    //
    // Only do the boot, when devicepath match
    //
    if (MatchPartitionDevicePathNode (BlockIoDevicePath, HardDriveDevicePath)) {
      //
      // Combine the Block IO and Hard Drive Device path together and try
      // to boot from it.
      //
      DevicePath    = NextDevicePathNode ((EFI_DEVICE_PATH_PROTOCOL *) HardDriveDevicePath);
      NewDevicePath = AppendDevicePath (BlockIoDevicePath, DevicePath);

      while (!IsDevicePathEnd (NewDevicePath)) {
        if (DevicePathType (NewDevicePath) == MESSAGING_DEVICE_PATH) {
          return (UINT8) (DevicePathSubType (NewDevicePath));
        }

        NewDevicePath = NextDevicePathNode (NewDevicePath);
      }
    }
  }

  gBS->FreePool (BlockIoBuffer);
  return 0;
}


/**
  Internal function to check if the input boot option is a valid EFI NV Boot####.

  @param OptionToCheck  Boot option to be checked.

  @retval TRUE      This boot option matches a valid EFI NV Boot####.
  @retval FALSE     If not.

**/
BOOLEAN
IsBootOptionValidNVVarialbe (
  IN  BDS_COMMON_OPTION             *OptionToCheck
  )
{
  LIST_ENTRY        TempList;
  BDS_COMMON_OPTION *BootOption;
  BOOLEAN           Valid;
  CHAR16            OptionName[20];

  Valid = FALSE;

  InitializeListHead (&TempList);
  UnicodeSPrint (OptionName, sizeof (OptionName), L"Boot%04x", OptionToCheck->BootCurrent);

  BootOption = BdsLibVariableToOption (&TempList, OptionName);
  if (BootOption == NULL) {
    return FALSE;
  }

  //
  // If the Boot Option Number and Device Path matches, OptionToCheck matches a
  // valid EFI NV Boot####.
  //
  if ((OptionToCheck->BootCurrent == BootOption->BootCurrent) &&
      (CompareMem (OptionToCheck->DevicePath, BootOption->DevicePath, GetDevicePathSize (OptionToCheck->DevicePath)) == 0))
      {
    Valid = TRUE;
  }

  FreePool (BootOption);

  return Valid;
}

/**
  Check whether a USB device match the specified USB Class device path. This
  function follows "Load Option Processing" behavior in UEFI specification.

  @param UsbIo       USB I/O protocol associated with the USB device.
  @param UsbClass    The USB Class device path to match.

  @retval TRUE       The USB device match the USB Class device path.
  @retval FALSE      The USB device does not match the USB Class device path.

**/
BOOLEAN
BdsMatchUsbClass (
  IN EFI_USB_IO_PROTOCOL        *UsbIo,
  IN USB_CLASS_DEVICE_PATH      *UsbClass
  )
{
  EFI_STATUS                    Status;
  EFI_USB_DEVICE_DESCRIPTOR     DevDesc;
  EFI_USB_INTERFACE_DESCRIPTOR  IfDesc;
  UINT8                         DeviceClass;
  UINT8                         DeviceSubClass;
  UINT8                         DeviceProtocol;

  if ((DevicePathType (UsbClass) != MESSAGING_DEVICE_PATH) ||
      (DevicePathSubType (UsbClass) != MSG_USB_CLASS_DP)){
    return FALSE;
  }

  //
  // Check Vendor Id and Product Id.
  //
  Status = UsbIo->UsbGetDeviceDescriptor (UsbIo, &DevDesc);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  if ((UsbClass->VendorId != 0xffff) &&
      (UsbClass->VendorId != DevDesc.IdVendor)) {
    return FALSE;
  }

  if ((UsbClass->ProductId != 0xffff) &&
      (UsbClass->ProductId != DevDesc.IdProduct)) {
    return FALSE;
  }

  DeviceClass    = DevDesc.DeviceClass;
  DeviceSubClass = DevDesc.DeviceSubClass;
  DeviceProtocol = DevDesc.DeviceProtocol;
  if (DeviceClass == 0) {
    //
    // If Class in Device Descriptor is set to 0, use the Class, SubClass and
    // Protocol in Interface Descriptor instead.
    //
    Status = UsbIo->UsbGetInterfaceDescriptor (UsbIo, &IfDesc);
    if (EFI_ERROR (Status)) {
      return FALSE;
    }

    DeviceClass    = IfDesc.InterfaceClass;
    DeviceSubClass = IfDesc.InterfaceSubClass;
    DeviceProtocol = IfDesc.InterfaceProtocol;
  }

  //
  // Check Class, SubClass and Protocol.
  //
  if ((UsbClass->DeviceClass != 0xff) &&
      (UsbClass->DeviceClass != DeviceClass)) {
    return FALSE;
  }

  if ((UsbClass->DeviceSubClass != 0xff) &&
      (UsbClass->DeviceSubClass != DeviceSubClass)) {
    return FALSE;
  }

  if ((UsbClass->DeviceProtocol != 0xff) &&
      (UsbClass->DeviceProtocol != DeviceProtocol)) {
    return FALSE;
  }

  return TRUE;
}

/**
  Check whether a USB device match the specified USB WWID device path. This
  function follows "Load Option Processing" behavior in UEFI specification.

  @param UsbIo       USB I/O protocol associated with the USB device.
  @param UsbWwid     The USB WWID device path to match.

  @retval TRUE       The USB device match the USB WWID device path.
  @retval FALSE      The USB device does not match the USB WWID device path.

**/
BOOLEAN
BdsMatchUsbWwid (
  IN EFI_USB_IO_PROTOCOL        *UsbIo,
  IN USB_WWID_DEVICE_PATH       *UsbWwid
  )
{
  EFI_STATUS                   Status;
  EFI_USB_DEVICE_DESCRIPTOR    DevDesc;
  EFI_USB_INTERFACE_DESCRIPTOR IfDesc;
  UINT16                       *LangIdTable;
  UINT16                       TableSize;
  UINT16                       Index;
  CHAR16                       *CompareStr;
  UINTN                        CompareLen;
  CHAR16                       *SerialNumberStr;
  UINTN                        Length;

  if ((DevicePathType (UsbWwid) != MESSAGING_DEVICE_PATH) ||
      (DevicePathSubType (UsbWwid) != MSG_USB_WWID_DP )){
    return FALSE;
  }

  //
  // Check Vendor Id and Product Id.
  //
  Status = UsbIo->UsbGetDeviceDescriptor (UsbIo, &DevDesc);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }
  if ((DevDesc.IdVendor != UsbWwid->VendorId) ||
      (DevDesc.IdProduct != UsbWwid->ProductId)) {
    return FALSE;
  }

  //
  // Check Interface Number.
  //
  Status = UsbIo->UsbGetInterfaceDescriptor (UsbIo, &IfDesc);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }
  if (IfDesc.InterfaceNumber != UsbWwid->InterfaceNumber) {
    return FALSE;
  }

  //
  // Check Serial Number.
  //
  if (DevDesc.StrSerialNumber == 0) {
    return FALSE;
  }

  //
  // Get all supported languages.
  //
  TableSize = 0;
  LangIdTable = NULL;
  Status = UsbIo->UsbGetSupportedLanguages (UsbIo, &LangIdTable, &TableSize);
  if (EFI_ERROR (Status) || (TableSize == 0) || (LangIdTable == NULL)) {
    return FALSE;
  }

  //
  // Serial number in USB WWID device path is the last 64-or-less UTF-16 characters.
  //
  CompareStr = (CHAR16 *) (UINTN) (UsbWwid + 1);
  CompareLen = (DevicePathNodeLength (UsbWwid) - sizeof (USB_WWID_DEVICE_PATH)) / sizeof (CHAR16);
  if (CompareStr[CompareLen - 1] == L'\0') {
    CompareLen--;
  }

  //
  // Compare serial number in each supported language.
  //
  for (Index = 0; Index < TableSize / sizeof (UINT16); Index++) {
    SerialNumberStr = NULL;
    Status = UsbIo->UsbGetStringDescriptor (
                      UsbIo,
                      LangIdTable[Index],
                      DevDesc.StrSerialNumber,
                      &SerialNumberStr
                      );
    if (EFI_ERROR (Status) || (SerialNumberStr == NULL)) {
      continue;
    }

    Length = StrLen (SerialNumberStr);
    if ((Length >= CompareLen) &&
        (CompareMem (SerialNumberStr + Length - CompareLen, CompareStr, CompareLen * sizeof (CHAR16)) == 0)) {
      FreePool (SerialNumberStr);
      return TRUE;
    }

    FreePool (SerialNumberStr);
  }

  return FALSE;
}

/**
  Find a USB device path which match the specified short-form device path start
  with USB Class or USB WWID device path and load the boot file then return the
  image handle. If ParentDevicePath is NULL, this function will search in all USB
  devices of the platform. If ParentDevicePath is not NULL,this function will only
  search in its child devices.

  @param ParentDevicePath      The device path of the parent.
  @param ShortFormDevicePath   The USB Class or USB WWID device path to match.

  @return  The image Handle if find load file from specified short-form device path
           or NULL if not found.

**/
EFI_HANDLE *
BdsFindUsbDevice (
  IN EFI_DEVICE_PATH_PROTOCOL   *ParentDevicePath,
  IN EFI_DEVICE_PATH_PROTOCOL   *ShortFormDevicePath
  )
{
  EFI_STATUS                Status;
  UINTN                     UsbIoHandleCount;
  EFI_HANDLE                *UsbIoHandleBuffer;
  EFI_DEVICE_PATH_PROTOCOL  *UsbIoDevicePath;
  EFI_USB_IO_PROTOCOL       *UsbIo;
  UINTN                     Index;
  UINTN                     ParentSize;
  UINTN                     Size;
  EFI_HANDLE                ImageHandle;
  EFI_HANDLE                Handle;
  EFI_DEVICE_PATH_PROTOCOL  *FullDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *NextDevicePath;

  FullDevicePath = NULL;
  ImageHandle    = NULL;

  //
  // Get all UsbIo Handles.
  //
  UsbIoHandleCount = 0;
  UsbIoHandleBuffer = NULL;
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiUsbIoProtocolGuid,
                  NULL,
                  &UsbIoHandleCount,
                  &UsbIoHandleBuffer
                  );
  if (EFI_ERROR (Status) || (UsbIoHandleCount == 0) || (UsbIoHandleBuffer == NULL)) {
    return NULL;
  }

  ParentSize = (ParentDevicePath == NULL) ? 0 : GetDevicePathSize (ParentDevicePath);
  for (Index = 0; Index < UsbIoHandleCount; Index++) {
    //
    // Get the Usb IO interface.
    //
    Status = gBS->HandleProtocol(
                    UsbIoHandleBuffer[Index],
                    &gEfiUsbIoProtocolGuid,
                    (VOID **) &UsbIo
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }

    UsbIoDevicePath = DevicePathFromHandle (UsbIoHandleBuffer[Index]);
    if (UsbIoDevicePath == NULL) {
      continue;
    }

    if (ParentDevicePath != NULL) {
      //
      // Compare starting part of UsbIoHandle's device path with ParentDevicePath.
      //
      Size = GetDevicePathSize (UsbIoDevicePath);
      if ((Size < ParentSize) ||
          (CompareMem (UsbIoDevicePath, ParentDevicePath, ParentSize - END_DEVICE_PATH_LENGTH) != 0)) {
        continue;
      }
    }

    if (BdsMatchUsbClass (UsbIo, (USB_CLASS_DEVICE_PATH *) ShortFormDevicePath) ||
        BdsMatchUsbWwid (UsbIo, (USB_WWID_DEVICE_PATH *) ShortFormDevicePath)) {
      //
      // Try to find if there is the boot file in this DevicePath
      //
      NextDevicePath = NextDevicePathNode (ShortFormDevicePath);
      if (!IsDevicePathEnd (NextDevicePath)) {
        FullDevicePath = AppendDevicePath (UsbIoDevicePath, NextDevicePath);
        //
        // Connect the full device path, so that Simple File System protocol
        // could be installed for this USB device.
        //
        BdsLibConnectDevicePath (FullDevicePath);
        REPORT_STATUS_CODE (EFI_PROGRESS_CODE, PcdGet32 (PcdProgressCodeOsLoaderLoad));
        Status = gBS->LoadImage (
                       TRUE,
                       gImageHandle,
                       FullDevicePath,
                       NULL,
                       0,
                       &ImageHandle
                       );
        FreePool (FullDevicePath);
      } else {
        FullDevicePath = UsbIoDevicePath;
        Status = EFI_NOT_FOUND;
      }

      //
      // If we didn't find an image directly, we need to try as if it is a removable device boot option
      // and load the image according to the default boot behavior for removable device.
      //
      if (EFI_ERROR (Status)) {
        //
        // check if there is a bootable removable media could be found in this device path ,
        // and get the bootable media handle
        //
        Handle = BdsLibGetBootableHandle(UsbIoDevicePath);
        if (Handle == NULL) {
          continue;
        }
        //
        // Load the default boot file \EFI\BOOT\boot{machinename}.EFI from removable Media
        //  machinename is ia32, ia64, x64, ...
        //
        FullDevicePath = FileDevicePath (Handle, EFI_REMOVABLE_MEDIA_FILE_NAME);
        if (FullDevicePath != NULL) {
          REPORT_STATUS_CODE (EFI_PROGRESS_CODE, PcdGet32 (PcdProgressCodeOsLoaderLoad));
          Status = gBS->LoadImage (
                          TRUE,
                          gImageHandle,
                          FullDevicePath,
                          NULL,
                          0,
                          &ImageHandle
                          );
          if (EFI_ERROR (Status)) {
            //
            // The DevicePath failed, and it's not a valid
            // removable media device.
            //
            continue;
          }
        } else {
          continue;
        }
      }
      break;
    }
  }

  FreePool (UsbIoHandleBuffer);
  return ImageHandle;
}

/**
  Expand USB Class or USB WWID device path node to be full device path of a USB
  device in platform then load the boot file on this full device path and return the
  image handle.

  This function support following 4 cases:
  1) Boot Option device path starts with a USB Class or USB WWID device path,
     and there is no Media FilePath device path in the end.
     In this case, it will follow Removable Media Boot Behavior.
  2) Boot Option device path starts with a USB Class or USB WWID device path,
     and ended with Media FilePath device path.
  3) Boot Option device path starts with a full device path to a USB Host Controller,
     contains a USB Class or USB WWID device path node, while not ended with Media
     FilePath device path. In this case, it will follow Removable Media Boot Behavior.
  4) Boot Option device path starts with a full device path to a USB Host Controller,
     contains a USB Class or USB WWID device path node, and ended with Media
     FilePath device path.

  @param  DevicePath    The Boot Option device path.

  @return  The image handle of boot file, or NULL if there is no boot file found in
           the specified USB Class or USB WWID device path.

**/
EFI_HANDLE *
BdsExpandUsbShortFormDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL       *DevicePath
  )
{
  EFI_HANDLE                *ImageHandle;
  EFI_DEVICE_PATH_PROTOCOL  *TempDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *ShortFormDevicePath;

  //
  // Search for USB Class or USB WWID device path node.
  //
  ShortFormDevicePath = NULL;
  ImageHandle         = NULL;
  TempDevicePath      = DevicePath;
  while (!IsDevicePathEnd (TempDevicePath)) {
    if ((DevicePathType (TempDevicePath) == MESSAGING_DEVICE_PATH) &&
        ((DevicePathSubType (TempDevicePath) == MSG_USB_CLASS_DP) ||
         (DevicePathSubType (TempDevicePath) == MSG_USB_WWID_DP))) {
      ShortFormDevicePath = TempDevicePath;
      break;
    }
    TempDevicePath = NextDevicePathNode (TempDevicePath);
  }

  if (ShortFormDevicePath == NULL) {
    //
    // No USB Class or USB WWID device path node found, do nothing.
    //
    return NULL;
  }

  if (ShortFormDevicePath == DevicePath) {
    //
    // Boot Option device path starts with USB Class or USB WWID device path.
    //
    ImageHandle = BdsFindUsbDevice (NULL, ShortFormDevicePath);
    if (ImageHandle == NULL) {
      //
      // Failed to find a match in existing devices, connect the short form USB
      // device path and try again.
      //
      BdsLibConnectUsbDevByShortFormDP (0xff, ShortFormDevicePath);
      ImageHandle = BdsFindUsbDevice (NULL, ShortFormDevicePath);
    }
  } else {
    //
    // Boot Option device path contains USB Class or USB WWID device path node.
    //

    //
    // Prepare the parent device path for search.
    //
    TempDevicePath = DuplicateDevicePath (DevicePath);
    ASSERT (TempDevicePath != NULL);
    SetDevicePathEndNode (((UINT8 *) TempDevicePath) + ((UINTN) ShortFormDevicePath - (UINTN) DevicePath));

    //
    // The USB Host Controller device path is already in Boot Option device path
    // and USB Bus driver already support RemainingDevicePath starts with USB
    // Class or USB WWID device path, so just search in existing USB devices and
    // doesn't perform ConnectController here.
    //
    ImageHandle = BdsFindUsbDevice (TempDevicePath, ShortFormDevicePath);
    FreePool (TempDevicePath);
  }

  return ImageHandle;
}


/**
  Check the Device path is a file path pointer to UEFI OS boot option.

  @param  DevicePath           Device path

  @retval TRUE                 The device path is pointer to  UEFI OS boot opiton.
  @retval FALSE                The device path isn't pointer to  UEFI OS boot opiton.

**/
BOOLEAN
IsUefiOsFilePath (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL  *WorkingDevicePath;
  EFI_HANDLE                Handle;
  EFI_STATUS                Status;
  BOOLEAN                   IsUefiOsFilePath;
  FILEPATH_DEVICE_PATH      *FilePath;

  IsUefiOsFilePath  = FALSE;
  WorkingDevicePath = DevicePath;

  Status = gBS->LocateDevicePath (
                  &gEfiSimpleFileSystemProtocolGuid,
                  &WorkingDevicePath,
                  &Handle
                  );
  if (!EFI_ERROR (Status) &&
      DevicePathType (WorkingDevicePath) == MEDIA_DEVICE_PATH &&
      DevicePathSubType (WorkingDevicePath) == MEDIA_FILEPATH_DP) {
    //
    //  If file name isn't default removable file name, we consider this file path pointer to UEFI OS
    //
    FilePath = (FILEPATH_DEVICE_PATH *) WorkingDevicePath;
    if ((StrLen (FilePath->PathName) != StrLen (EFI_REMOVABLE_MEDIA_FILE_NAME)) ||
        (StrCmp (FilePath->PathName, EFI_REMOVABLE_MEDIA_FILE_NAME) != 0)) {
      IsUefiOsFilePath = TRUE;
    }
  }

  return IsUefiOsFilePath;
}


/**
 Check the device path belongs to PXE boot option or not.

 @param[in] DevicePath        Device path

 @retval TRUE                 The device path is for PXE boot option.
 @retval FALSE                The device path isn't for PXE boot option.
**/
BOOLEAN
IsPxeBoot (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  )
{
  BOOLEAN                      IsPxeImage;
  EFI_DEVICE_PATH_PROTOCOL     *NetworkDevicePath;

  IsPxeImage = FALSE;
  NetworkDevicePath = DevicePath;
  while (!IsDevicePathEnd (NetworkDevicePath)) {
    if (NetworkDevicePath->Type == MESSAGING_DEVICE_PATH) {
      if (NetworkDevicePath->SubType == MSG_MAC_ADDR_DP) {
        IsPxeImage = TRUE;
        break;
      }
    }
    NetworkDevicePath = NextDevicePathNode (NetworkDevicePath);
  }

  return IsPxeImage;
}


/**
  According to device path and image handle to boot.

  @param  ImageHandle
  @param  Option               The boot option need to be processed
  @param  DevicePath           The device path which describe where to load the boot image or the legcy BBS device path to boot the legacy OS
  @param  ExitDataSize         Returned directly from gBS->StartImage ()
  @param  ExitData             Returned directly from gBS->StartImage ()

  @retval EFI_SUCCESS          Boot from recovery boot option successful
  @retval Other                Some errors occured during boot process

**/
EFI_STATUS
BootFromImage (
  IN  EFI_HANDLE                    ImageHandle,
  IN  BDS_COMMON_OPTION             *Option,
  IN  EFI_DEVICE_PATH_PROTOCOL      *DevicePath,
  OUT UINTN                         *ExitDataSize,
  OUT CHAR16                        **ExitData OPTIONAL
  )
{
  EFI_STATUS                        Status;
  EFI_LOADED_IMAGE_PROTOCOL         *ImageInfo;
  EFI_CONSOLE_CONTROL_SCREEN_MODE   ScreenMode;
  EFI_EVENT                         ShellImageEvent;
  EFI_GUID                          ShellEnvProtocol = SHELL_ENVIRONMENT_INTERFACE_PROTOCOL;

  ScreenMode = EfiConsoleControlScreenMaxValue;

  Status = gBS->HandleProtocol (ImageHandle, &gEfiLoadedImageProtocolGuid, (VOID **) &ImageInfo);
  ASSERT_EFI_ERROR (Status);

  if (Option->LoadOptionsSize != 0) {
    ImageInfo->LoadOptionsSize  = Option->LoadOptionsSize;
    ImageInfo->LoadOptions      = Option->LoadOptions;
  }

  //
  // Set a monitor to its native resolution when boot to UEFI Windows and update Bgrt if needed
  //
  //
  // Check Image is PXE image, DEVICE PATH will carry Message MAC device information
  //
  if (IsPxeBoot (DevicePath)) {
    //
    // If is PXE image, perpare to TEXT mode via DisableQuietBoot
    //
    DisableQuietBoot ();
  }

  //
  // Register Event for Shell Image
  //
  if (mShellImageCallbackReg == NULL) {
    Status = gBS->CreateEvent (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    ShellImageCallback,
                    NULL,
                    &ShellImageEvent
                    );
    if (!EFI_ERROR (Status)) {
      Status = gBS->RegisterProtocolNotify (
                      &ShellEnvProtocol,
                      ShellImageEvent,
                      &mShellImageCallbackReg
                      );
    }
  }

  //
  // Before calling the image, enable the Watchdog Timer for the 5 Minute period
  //
  gBS->SetWatchdogTimer (5 * 60, 0x0000, 0x00, NULL);

  //
  // PostCode = 0xFB, UEFI Boot Start Image
  //
  POST_CODE (POST_BDS_START_IMAGE);

  PERF_END (0, "PostBDS", NULL, 0);
  WRITE_BOOT_TO_OS_PERFORMANCE_DATA;
  if (FeaturePcdGet (PcdAutoCreateDummyBootOption)) {
    BdsLibChangeToVirtualBootOrder ();
  }

  DEBUG ((EFI_D_INFO | EFI_D_LOAD, "Image Start...\n"));

  //
  // Report status code for OS Loader StartImage.
  //
  REPORT_STATUS_CODE (EFI_PROGRESS_CODE, PcdGet32 (PcdProgressCodeOsLoaderStart));

  Status = gBS->StartImage (ImageHandle, ExitDataSize, ExitData);
  DEBUG ((EFI_D_INFO | EFI_D_LOAD, "Image Return Status = %r\n", Status));

  //
  // Clear the Watchdog Timer after the image returns
  //
  gBS->SetWatchdogTimer (0x0000, 0x0000, 0x0000, NULL);
  if (FeaturePcdGet (PcdAutoCreateDummyBootOption)) {
    SyncBootOrder ();
    BdsLibRestoreBootOrderFromPhysicalBootOrder ();
  }
  return Status;
}


/**
  According to device path  to boot from reocvery boot option.

  @param  Option               The boot option need to be processed
  @param  DevicePath           The device path which describe where to load the boot image or the legcy BBS device path to boot the legacy OS
  @param  ExitDataSize         Returned directly from gBS->StartImage ()
  @param  ExitData             Returned directly from gBS->StartImage ()

  @retval EFI_SUCCESS          Boot from recovery boot option successful
  @retval Other                Some errors occured during boot process

**/
EFI_STATUS
BootRecoveryOption (
  IN  BDS_COMMON_OPTION             *Option,
  IN  EFI_DEVICE_PATH_PROTOCOL      *DevicePath,
  OUT UINTN                         *ExitDataSize,
  OUT CHAR16                        **ExitData OPTIONAL
  )
{
  EFI_DEVICE_PATH_PROTOCOL  *WorkingDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *FilePath;
  EFI_HANDLE                Handle;
  EFI_STATUS                Status;
  EFI_HANDLE                ImageHandle;

  if (!IsUefiOsFilePath (DevicePath)) {
    return EFI_UNSUPPORTED;
  }

  WorkingDevicePath = DevicePath;
  Status = gBS->LocateDevicePath (
                  &gEfiSimpleFileSystemProtocolGuid,
                  &WorkingDevicePath,
                  &Handle
                  );
  if (!EFI_ERROR (Status)) {
    FilePath = FileDevicePath (Handle, EFI_REMOVABLE_MEDIA_FILE_NAME);
    if (FilePath != NULL) {
      Status = gBS->LoadImage (
                      TRUE,
                      gImageHandle,
                      FilePath,
                      NULL,
                      0,
                      &ImageHandle
                      );
      if (!EFI_ERROR (Status)) {
        Status = BootFromImage (ImageHandle, Option, FilePath, ExitDataSize, ExitData);
      }

      gBS->FreePool (FilePath);
    }
  }

  return Status;
}

/**
  Process the boot option follow the UEFI specification and
  special treat the legacy boot option with BBS_DEVICE_PATH.

  @param  Option                 The boot option need to be processed
  @param  DevicePath             The device path which describe where to load the
                                 boot image or the legacy BBS device path to boot
                                 the legacy OS
  @param  ExitDataSize           The size of exit data.
  @param  ExitData               Data returned when Boot image failed.

  @retval EFI_SUCCESS            Boot from the input boot option successfully.
  @retval EFI_NOT_FOUND          If the Device Path is not found in the system

**/
EFI_STATUS
EFIAPI
BdsLibBootViaBootOption (
  IN  BDS_COMMON_OPTION             *Option,
  IN  EFI_DEVICE_PATH_PROTOCOL      *DevicePath,
  OUT UINTN                         *ExitDataSize,
  OUT CHAR16                        **ExitData OPTIONAL
  )
{
  EFI_STATUS                Status;
  EFI_HANDLE                Handle;
  EFI_HANDLE                ImageHandle;
  EFI_DEVICE_PATH_PROTOCOL  *TempDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *FilePath;
  EFI_DEVICE_PATH_PROTOCOL  *WorkingDevicePath;
  EFI_ACPI_S3_SAVE_PROTOCOL *AcpiS3Save;
  EFI_BLOCK_IO_PROTOCOL     *BlkIo;
  VOID                      *Buffer;
  BDS_COMMON_OPTION         *OptionTemp;
  LIST_ENTRY                *LinkTemp;
  EFI_STATUS                LocateDevicePathStatus;
  EFI_STATUS                RecoverBootStatus;
  UINT16                            BootNext;
  EFI_TREE_MEASURE_OS_LOADER_AUTHORITY_PROTOCOL   *MeasureOsLoaderAuthorityProtocol;

  //
  // PostCode = 0x2C, Shadow Misc Option ROM
  //
  POST_CODE (BDS_EFI64_SHADOW_ALL_LEGACY_ROM);

  //
  // All the driver options should have been processed since now boot will be performed.
  //
  PERF_END (0, BDS_TOK, NULL, 0);

  PERF_START (0, "PostBDS", NULL, 0);

  if (Option == NULL || DevicePath == NULL || ExitDataSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *ExitDataSize = 0;
  if (ExitData != NULL) {
    *ExitData = NULL;
  }

  //
  // Before boot to device, always clean BootNext variable.
  //
  gRT->SetVariable (
         L"BootNext",
         &gEfiGlobalVariableGuid,
         EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
         0,
         &BootNext
         );
  //
  // Notes: put EFI64 ROM Shadow Solution
  //
  EFI64_SHADOW_ALL_LEGACY_ROM ();

  //
  // If it's Device Path that starts with a hard drive path, append it with the front part to compose a
  // full device path
  //
  WorkingDevicePath = NULL;
  if ((DevicePathType (DevicePath) == MEDIA_DEVICE_PATH) &&
      (DevicePathSubType (DevicePath) == MEDIA_HARDDRIVE_DP)) {
    WorkingDevicePath = BdsExpandPartitionPartialDevicePathToFull (
                          (HARDDRIVE_DEVICE_PATH *)DevicePath
                          );
    if (WorkingDevicePath != NULL) {
      DevicePath = WorkingDevicePath;
    }
  }

  //
  // Signal the EVT_SIGNAL_READY_TO_BOOT event
  //
  //
  // PostCode = 0x2E, Last Chipset initial before boot to OS
  //
  POST_CODE (BDS_READY_TO_BOOT_EVENT);
  EfiSignalEventReadyToBoot();

  //
  // S3Save should be executed after EventReadyToBoot
  //
  Status = gBS->LocateProtocol (&gEfiAcpiS3SaveProtocolGuid, NULL, (VOID **) &AcpiS3Save);
  if (!EFI_ERROR (Status)) {
    AcpiS3Save->S3Save (AcpiS3Save, NULL);
  }

  //
  // Set Boot Current
  //
  if (IsBootOptionValidNVVarialbe (Option)) {
    //
    // For a temporary boot (i.e. a boot by selected a EFI Shell using "Boot From File"), Boot Current is actually not valid.
    // In this case, "BootCurrent" is not created.
    // Only create the BootCurrent variable when it points to a valid Boot#### variable.
    //
    gRT->SetVariable (
          L"BootCurrent",
          &gEfiGlobalVariableGuid,
          EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
          sizeof (UINT16),
          &Option->BootCurrent
          );
  }

  ASSERT (Option->DevicePath != NULL);
  if ((DevicePathType (Option->DevicePath) == BBS_DEVICE_PATH) &&
      (DevicePathSubType (Option->DevicePath) == BBS_BBS_DP)
     ) {
    //
    // Check to see if we should legacy BOOT. If yes then do the legacy boot
    //
    if (FeaturePcdGet (PcdAutoCreateDummyBootOption) && BdsLibIsBootOrderHookEnabled ()) {
      BdsLibRestoreBootOrderFromPhysicalBootOrder ();
    }
    //
    // PostCode = 0x2F, Start to boot Legacy OS
    //
    POST_CODE (BDS_GO_LEGACY_BOOT);
    return BdsLibDoLegacyBoot (Option);
  }
  //
  // Drop the TPL level from EFI_TPL_DRIVER to EFI_TPL_APPLICATION
  //
  //
  // PostCode = 0x30, Start to boot UEFI OS
  //
  POST_CODE (BDS_GO_UEFI_BOOT);

  gBS->RestoreTPL (TPL_APPLICATION);

#ifndef MDEPKG_NDEBUG
  DumpMemoryMap();
#endif
  if (FeaturePcdGet (PcdMemoryMapConsistencyCheck)) {
    CheckRtAndBsMemUsage ();
  }

  DEBUG_CODE_BEGIN();

  if (Option->Description == NULL) {
    DEBUG ((DEBUG_INFO | DEBUG_LOAD, "Booting from unknown device path\n"));
  } else {
    DEBUG ((DEBUG_INFO | DEBUG_LOAD, "Booting %S\n", Option->Description));
  }

  DEBUG_CODE_END();

  EnableOptimalTextMode ();

  //
  // Report status code for OS Loader LoadImage.
  //
  REPORT_STATUS_CODE (EFI_PROGRESS_CODE, PcdGet32 (PcdProgressCodeOsLoaderLoad));

  if (IsPxeBoot (DevicePath)) {
    DisableQuietBoot ();
  }

  Status = gBS->LocateProtocol (
                &gEfiTrEEMeasureOsLoaderAuthorityProtocolGuid,
                NULL,
                (VOID**)&MeasureOsLoaderAuthorityProtocol
                );
  if (!EFI_ERROR(Status)) {
    MeasureOsLoaderAuthorityProtocol->OsLoader = TRUE;
  }

  Status = gBS->LoadImage (
                  TRUE,
                  gImageHandle,
                  DevicePath,
                  NULL,
                  0,
                  &ImageHandle
                  );
  //
  // If we didn't find an image, we may need to load the default boot behavior for the device.
  //
  if (EFI_ERROR (Status)) {
    //
    // Find a Simple File System protocol on the device path.
    // If the remaining device path is set to end then no Files are being specified, so try the removable media file name.
    //
    TempDevicePath = DevicePath;
    LocateDevicePathStatus = gBS->LocateDevicePath (
                                    &gEfiSimpleFileSystemProtocolGuid,
                                    &TempDevicePath,
                                    &Handle
                                    );
    if (!EFI_ERROR (LocateDevicePathStatus) && IsDevicePathEnd (TempDevicePath)) {
      FilePath = FileDevicePath (Handle, EFI_REMOVABLE_MEDIA_FILE_NAME);
      if (FilePath) {
        //
        // Issue a dummy read to the device to check for media change.
        // When the removable media is changed, any Block IO read/write will cause the BlockIo protocol be reinstalled and EFI_MEDIA_CHANGED is returned.
        // After the Block IO protocol is reinstalled, subsequent Block IO read/write will success.
        //
        Status = gBS->HandleProtocol (
                        Handle,
                        &gEfiBlockIoProtocolGuid,
                        (VOID **) &BlkIo
                        );
        if (!EFI_ERROR (Status)) {
          Buffer = AllocatePool (BlkIo->Media->BlockSize);
          if (Buffer != NULL) {
            BlkIo->ReadBlocks (
                     BlkIo,
                     BlkIo->Media->MediaId,
                     0,
                     BlkIo->Media->BlockSize,
                     Buffer
                     );
            gBS->FreePool (Buffer);
          }
        }

        Status = gBS->LoadImage (
                        TRUE,
                        gImageHandle,
                        FilePath,
                        NULL,
                        0,
                        &ImageHandle
                        );
        gBS->FreePool (FilePath);
        if (EFI_ERROR (Status)) {
          //
          // The DevicePath failed, and it's not a valid removable media device.
          //
          goto Done;
        }
      }
    }
  }

  if (EFI_ERROR (Status)) {
    //
    // If the DevicePath is USB class, we will boot the next USB EFI Boot Option
    //
    if ((DevicePathType (Option->DevicePath) == MESSAGING_DEVICE_PATH) &&
        (DevicePathSubType (Option->DevicePath) == MSG_USB_CLASS_DP)) {
      //
      // Try Windows To Go devices in the list until one device return success
      //
      Status = EFI_NOT_FOUND;
      LinkTemp = mWindowsToGoDeviceList.ForwardLink;
      while (LinkTemp != &mWindowsToGoDeviceList) {
        OptionTemp = CR (LinkTemp, BDS_COMMON_OPTION, Link, BDS_LOAD_OPTION_SIGNATURE);
        Status = BdsLibBootViaBootOption (OptionTemp, OptionTemp->DevicePath, ExitDataSize, ExitData);
        if (!EFI_ERROR (Status)) {
          break;
        }

        LinkTemp = LinkTemp->ForwardLink;
      }
    }

    //
    // It there is any error from the Boot attempt exit now.
    //
    goto Done;
  }

  BitlockerVirtualKeyboardSupport (DevicePath);

  //
  // Provide the image with it's load options
  //
  Status = BootFromImage (ImageHandle, Option, DevicePath, ExitDataSize, ExitData);

Done:
  if (EFI_ERROR (Status) && IsUefiOsFilePath (DevicePath)) {
    //
    // Try to boot from recovery boot option and only update status to EFI_SUCCESS,
    // if boot from this recovery file path is successful.
    //
    RecoverBootStatus = BootRecoveryOption (Option, DevicePath, ExitDataSize, ExitData);
    if (!EFI_ERROR (RecoverBootStatus)) {
      Status = EFI_SUCCESS;
    }
  }

  if (WorkingDevicePath != NULL) {
    gBS->FreePool (WorkingDevicePath);
  }

  //
  // Clear Boot Current
  //
  gRT->SetVariable (
        L"BootCurrent",
        &gEfiGlobalVariableGuid,
        EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
        0,
        &Option->BootCurrent
        );
  //
  // Clear Boot Option
  //
  mEnumBootDevice = FALSE;

  //
  // Signal BIOS after the image returns
  //
  SignalImageReturns ();

  //
  // Raise the TPL level back to EFI_TPL_DRIVER
  //
  gBS->RaiseTPL (TPL_DRIVER);

  return Status;
}


/**
  Check to see if a hard ware device path was passed in. If it was then search
  all the block IO devices for the passed in hard drive device path.

  @param  Option               The current processing boot option.
  @param  HardDriveDevicePath  EFI Device Path to boot, if it starts with a hard
                               drive device path.
  @param  LoadOptionsSize      Passed into gBS->StartImage () via the loaded image protocol.
  @param  LoadOptions          Passed into gBS->StartImage () via the loaded image protocol.
  @param  ExitDataSize         returned directly from gBS->StartImage ()
  @param  ExitData             returned directly from gBS->StartImage ()

  @retval EFI_SUCCESS          Status from gBS->StartImage (),
                               or BootByDiskSignatureAndPartition ()
  @retval EFI_NOT_FOUND        If the Device Path is not found in the system

**/
EFI_STATUS
BdsBootByDiskSignatureAndPartition (
  IN  BDS_COMMON_OPTION          * Option,
  IN  HARDDRIVE_DEVICE_PATH      * HardDriveDevicePath,
  IN  UINT32                     LoadOptionsSize,
  IN  VOID                       *LoadOptions,
  OUT UINTN                      *ExitDataSize,
  OUT CHAR16                     **ExitData OPTIONAL
  )
{
  EFI_STATUS                Status;
  UINTN                     BlockIoHandleCount;
  EFI_HANDLE                *BlockIoBuffer;
  EFI_DEVICE_PATH_PROTOCOL  *BlockIoDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *NewDevicePath;
  UINTN                     Index;
  EFI_STATUS                HandleProtocolStatus;

  *ExitDataSize = 0;
  *ExitData     = NULL;

  if (!((DevicePathType (&HardDriveDevicePath->Header) == MEDIA_DEVICE_PATH) &&
        (DevicePathSubType (&HardDriveDevicePath->Header) == MEDIA_HARDDRIVE_DP))) {
    //
    // If the HardDriveDevicePath does not start with a Hard Drive Device Path
    // exit.
    //
    return EFI_NOT_FOUND;
  }
  //
  // The boot device have already been connected
  //
  Status = gBS->LocateHandleBuffer (ByProtocol, &gEfiBlockIoProtocolGuid, NULL, &BlockIoHandleCount, &BlockIoBuffer);
  if (EFI_ERROR (Status) || BlockIoHandleCount == 0) {
    //
    // If there was an error or there are no device handles that support
    // the BLOCK_IO Protocol, then return.
    //
    return EFI_NOT_FOUND;
  }
  //
  // Loop through all the device handles that support the BLOCK_IO Protocol
  //
  Status = EFI_NOT_FOUND;
  for (Index = 0; Index < BlockIoHandleCount; Index++) {
    HandleProtocolStatus = gBS->HandleProtocol (BlockIoBuffer[Index], &gEfiDevicePathProtocolGuid, (VOID *) &BlockIoDevicePath);
    if (EFI_ERROR (HandleProtocolStatus) || BlockIoDevicePath == NULL) {
      continue;
    }
    //
    // Only do the boot, when devicepath match
    //
    if (MatchPartitionDevicePathNode (BlockIoDevicePath, HardDriveDevicePath)) {
      //
      // Combine the Block IO and Hard Drive Device path together and try
      // to boot from it.
      //
      DevicePath    = NextDevicePathNode ((EFI_DEVICE_PATH_PROTOCOL *) HardDriveDevicePath);
      NewDevicePath = AppendDevicePath (BlockIoDevicePath, DevicePath);

      //
      // Recursive boot with new device path
      //
      Status = BdsLibBootViaBootOption (Option, NewDevicePath, ExitDataSize, ExitData);
      //
      // No matter MBR or GPT, the partition sigunature is unique, so we should break after booting from
      // boot option.
      //
      break;
    }
  }

  gBS->FreePool (BlockIoBuffer);
  return Status;
}


/**
  Expand a device path that starts with a hard drive media device path node to be a
  full device path that includes the full hardware path to the device. We need
  to do this so it can be booted. As an optimization the front match (the part point
  to the partition node. E.g. ACPI() /PCI()/ATA()/Partition() ) is saved in a variable
  so a connect all is not required on every boot. All successful history device path
  which point to partition node (the front part) will be saved.

  @param  HardDriveDevicePath    EFI Device Path to boot, if it starts with a hard
                                 drive media device path.
  @return A Pointer to the full device path or NULL if a valid Hard Drive devic path
          cannot be found.

**/
EFI_DEVICE_PATH_PROTOCOL *
EFIAPI
BdsExpandPartitionPartialDevicePathToFull (
  IN  HARDDRIVE_DEVICE_PATH      *HardDriveDevicePath
  )
{
  EFI_STATUS                Status;
  UINTN                     BlockIoHandleCount;
  EFI_HANDLE                *BlockIoBuffer;
  EFI_DEVICE_PATH_PROTOCOL  *FullDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *BlockIoDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  UINTN                     Index;
  UINTN                     InstanceNum;
  EFI_DEVICE_PATH_PROTOCOL  *CachedDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *TempNewDevicePath;
  UINTN                     CachedDevicePathSize;
  BOOLEAN                   DeviceExist;
  BOOLEAN                   NeedAdjust;
  EFI_DEVICE_PATH_PROTOCOL  *Instance;
  UINTN                     Size;

  if (!((DevicePathType (&HardDriveDevicePath->Header) == MEDIA_DEVICE_PATH) &&
        (DevicePathSubType (&HardDriveDevicePath->Header) == MEDIA_HARDDRIVE_DP))) {
    return NULL;
  }

  FullDevicePath = NULL;
  CachedDevicePath = NULL;
  //
  // Check if there is prestore HD_BOOT_DEVICE_PATH_VARIABLE_NAME variable.
  // If exist, search the front path which point to partition node in the variable instants.
  // If fail to find or HD_BOOT_DEVICE_PATH_VARIABLE_NAME not exist, reconnect all and search in all system
  //
  if (FeaturePcdGet (PcdHdBootDevPathVarSupported)) {
    CachedDevicePath = BdsLibGetVariableAndSize (
                        HD_BOOT_DEVICE_PATH_VARIABLE_NAME,
                        &gHdBootDevicePathVariablGuid,
                        &CachedDevicePathSize
                        );

    if (CachedDevicePath != NULL) {
      TempNewDevicePath = CachedDevicePath;
      DeviceExist = FALSE;
      NeedAdjust = FALSE;
      do {
        //
        // Check every instance of the variable
        // First, check whether the instance contain the partition node, which is needed for distinguishing  multi
        // partial partition boot option. Second, check whether the instance could be connected.
        //
        Instance  = GetNextDevicePathInstance (&TempNewDevicePath, &Size);
        if (MatchPartitionDevicePathNode (Instance, HardDriveDevicePath)) {
          //
          // Connect the device path instance, the device path point to hard drive media device path node
          // e.g. ACPI() /PCI()/ATA()/Partition()
          //
          Status = BdsLibConnectDevicePath (Instance);
          if (!EFI_ERROR (Status)) {
            DeviceExist = TRUE;
            break;
          }
        }
        //
        // Come here means the first instance is not matched
        //
        NeedAdjust = TRUE;
        FreePool(Instance);
      } while (TempNewDevicePath != NULL);

      if (DeviceExist) {
        //
        // Find the matched device path.
        // Append the file path information from the boot option and return the fully expanded device path.
        //
        DevicePath     = NextDevicePathNode ((EFI_DEVICE_PATH_PROTOCOL *) HardDriveDevicePath);
        FullDevicePath = AppendDevicePath (Instance, DevicePath);

        //
        // Adjust the HD_BOOT_DEVICE_PATH_VARIABLE_NAME instances sequence if the matched one is not first one.
        //
        if (NeedAdjust) {
          //
          // First delete the matched instance.
          //
          TempNewDevicePath = CachedDevicePath;
          CachedDevicePath  = BdsLibDelPartMatchInstance (CachedDevicePath, Instance );
          FreePool (TempNewDevicePath);

          //
          // Second, append the remaining path after the matched instance
          //
          TempNewDevicePath = CachedDevicePath;
          CachedDevicePath = AppendDevicePathInstance (Instance, CachedDevicePath );
          FreePool (TempNewDevicePath);
          //
          // Save the matching Device Path so we don't need to do a connect all next time
          //
          Status = gRT->SetVariable (
                          HD_BOOT_DEVICE_PATH_VARIABLE_NAME,
                          &gHdBootDevicePathVariablGuid,
                          EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                          GetDevicePathSize (CachedDevicePath),
                          CachedDevicePath
                          );
        }

        FreePool (Instance);
        FreePool (CachedDevicePath);
        return FullDevicePath;
      }
    }

    //
    // If we get here we fail to find or HD_BOOT_DEVICE_PATH_VARIABLE_NAME not exist, and now we need
    // to search all devices in the system for a matched partition
    //
    BdsLibConnectAllDriversToAllControllers ();
  }

  Status = gBS->LocateHandleBuffer (ByProtocol, &gEfiBlockIoProtocolGuid, NULL, &BlockIoHandleCount, &BlockIoBuffer);
  if (EFI_ERROR (Status) || BlockIoHandleCount == 0 || BlockIoBuffer == NULL) {
    //
    // If there was an error or there are no device handles that support
    // the BLOCK_IO Protocol, then return.
    //
    return NULL;
  }
  //
  // Loop through all the device handles that support the BLOCK_IO Protocol
  //
  for (Index = 0; Index < BlockIoHandleCount; Index++) {

    Status = gBS->HandleProtocol (BlockIoBuffer[Index], &gEfiDevicePathProtocolGuid, (VOID *) &BlockIoDevicePath);
    if (EFI_ERROR (Status) || BlockIoDevicePath == NULL) {
      continue;
    }

    if (MatchPartitionDevicePathNode (BlockIoDevicePath, HardDriveDevicePath)) {
      //
      // Find the matched partition device path
      //
      DevicePath    = NextDevicePathNode ((EFI_DEVICE_PATH_PROTOCOL *) HardDriveDevicePath);
      FullDevicePath = AppendDevicePath (BlockIoDevicePath, DevicePath);

      if (FeaturePcdGet (PcdHdBootDevPathVarSupported)) {
        //
        // Save the matched partition device path in HD_BOOT_DEVICE_PATH_VARIABLE_NAME variable
        //
        if (CachedDevicePath != NULL) {
          //
          // Save the matched partition device path as first instance of HD_BOOT_DEVICE_PATH_VARIABLE_NAME variable
          //
          if (BdsLibMatchDevicePaths (CachedDevicePath, BlockIoDevicePath)) {
            TempNewDevicePath = CachedDevicePath;
            CachedDevicePath = BdsLibDelPartMatchInstance (CachedDevicePath, BlockIoDevicePath);
            FreePool(TempNewDevicePath);

            TempNewDevicePath = CachedDevicePath;
            CachedDevicePath = AppendDevicePathInstance (BlockIoDevicePath, CachedDevicePath);
            if (TempNewDevicePath != NULL) {
              FreePool(TempNewDevicePath);
            }
          } else {
            TempNewDevicePath = CachedDevicePath;
            CachedDevicePath = AppendDevicePathInstance (BlockIoDevicePath, CachedDevicePath);
            FreePool(TempNewDevicePath);
          }
          //
          // Here limit the device path instance number to 12, which is max number for a system support 3 IDE controller
          // If the user try to boot many OS in different HDs or partitions, in theory,
          // the HD_BOOT_DEVICE_PATH_VARIABLE_NAME variable maybe become larger and larger.
          //
          InstanceNum = 0;
          ASSERT (CachedDevicePath != NULL);
          if (CachedDevicePath == NULL) {
            break;
          }
          TempNewDevicePath = CachedDevicePath;
          while (!IsDevicePathEnd (TempNewDevicePath)) {
            TempNewDevicePath = NextDevicePathNode (TempNewDevicePath);
            //
            // Parse one instance
            //
            while (!IsDevicePathEndType (TempNewDevicePath)) {
              TempNewDevicePath = NextDevicePathNode (TempNewDevicePath);
            }
            InstanceNum++;
            //
            // If the CachedDevicePath variable contain too much instance, only remain 12 instances.
            //
            if (InstanceNum >= 12) {
              SetDevicePathEndNode (TempNewDevicePath);
              break;
            }
          }
        } else {
          CachedDevicePath = DuplicateDevicePath (BlockIoDevicePath);
        }

        //
        // Save the matching Device Path so we don't need to do a connect all next time
        //
        Status = gRT->SetVariable (
                        HD_BOOT_DEVICE_PATH_VARIABLE_NAME,
                        &gHdBootDevicePathVariablGuid,
                        EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                        GetDevicePathSize (CachedDevicePath),
                        CachedDevicePath
                        );
      }
      break;
    }
  }

  if (CachedDevicePath != NULL) {
    FreePool (CachedDevicePath);
  }
  if (BlockIoBuffer != NULL) {
    FreePool (BlockIoBuffer);
  }
  return FullDevicePath;
}


/**
  Check whether there is a instance in BlockIoDevicePath, which contain multi device path
  instances, has the same partition node with HardDriveDevicePath device path

  @param  BlockIoDevicePath      Multi device path instances which need to check
  @param  HardDriveDevicePath    A device path which starts with a hard drive media
                                 device path.

  @retval TRUE                   There is a matched device path instance.
  @retval FALSE                  There is no matched device path instance.

**/
BOOLEAN
EFIAPI
MatchPartitionDevicePathNode (
  IN  EFI_DEVICE_PATH_PROTOCOL   *BlockIoDevicePath,
  IN  HARDDRIVE_DEVICE_PATH      *HardDriveDevicePath
  )
{
  HARDDRIVE_DEVICE_PATH     *TmpHdPath;
  HARDDRIVE_DEVICE_PATH     *TempPath;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  BOOLEAN                   Match;
  EFI_DEVICE_PATH_PROTOCOL  *BlockIoHdDevicePathNode;

  if ((BlockIoDevicePath == NULL) || (HardDriveDevicePath == NULL)) {
    return FALSE;
  }

  //
  // Make PreviousDevicePath == the device path node before the end node
  //
  DevicePath              = BlockIoDevicePath;
  BlockIoHdDevicePathNode = NULL;

  //
  // find the partition device path node
  //
  while (!IsDevicePathEnd (DevicePath)) {
    if ((DevicePathType (DevicePath) == MEDIA_DEVICE_PATH) &&
        (DevicePathSubType (DevicePath) == MEDIA_HARDDRIVE_DP)
        ) {
      BlockIoHdDevicePathNode = DevicePath;
      break;
    }

    DevicePath = NextDevicePathNode (DevicePath);
  }

  if (BlockIoHdDevicePathNode == NULL) {
    return FALSE;
  }
  //
  // See if the harddrive device path in blockio matches the orig Hard Drive Node
  //
  TmpHdPath = (HARDDRIVE_DEVICE_PATH *) BlockIoHdDevicePathNode;
  TempPath  = (HARDDRIVE_DEVICE_PATH *) BdsLibUnpackDevicePath ((EFI_DEVICE_PATH_PROTOCOL *) HardDriveDevicePath);
  Match = FALSE;

  //
  // Check for the match
  //
  if ((TempPath != NULL) &&
      (TmpHdPath->MBRType == TempPath->MBRType) &&
      (TmpHdPath->SignatureType == TempPath->SignatureType)) {
    switch (TmpHdPath->SignatureType) {
    case SIGNATURE_TYPE_GUID:
      Match = CompareGuid ((EFI_GUID *)TmpHdPath->Signature, (EFI_GUID *)TempPath->Signature);
      break;
    case SIGNATURE_TYPE_MBR:
      Match = (BOOLEAN)(*((UINT32 *)(&(TmpHdPath->Signature[0]))) == *(UINT32 *)(&(TempPath->Signature[0])));
      break;
    default:
      Match = FALSE;
      break;
    }
  }

  if (TempPath != NULL) {
    gBS->FreePool (TempPath);
  }

  return Match;
}

/**
  Delete the boot option that content was duplicated.

**/
VOID
BdsLibDeleteRedundantOption (
  )
{
  UINT16                    *BootOrder;
  UINT16                    *TmpBootOrder;
  UINTN                     BootOrderSize;
  UINTN                     SearchIndex;
  UINTN                     Index;
  UINTN                     BootOptionCnt;
  UINT16                    BootIndex[BOOT_OPTION_MAX_CHAR];
  UINT8                     **BootOptionPool;
  UINTN                     *BootOptionSizePool;
  INTN                      CompareResult;
  BOOLEAN                   BootOrderChanged;

  BootOrder          = NULL;
  TmpBootOrder       = NULL;
  BootOptionPool     = NULL;
  BootOptionSizePool = NULL;
  BootOrderChanged   = FALSE;
  BootOptionCnt      = 0;
  BootOrderSize      = 0;
  //
  // Get all boot option
  //
  BootOrder = BdsLibGetVariableAndSize (
                L"BootOrder",
                &gEfiGlobalVariableGuid,
                &BootOrderSize
                );
  TmpBootOrder = AllocateZeroPool (BootOrderSize);
  if (NULL == BootOrder || TmpBootOrder == NULL) {
    goto EXIT;
  }
  CopyMem (TmpBootOrder, BootOrder, BootOrderSize);
  BootOptionCnt      = BootOrderSize / sizeof (UINT16);
  if (BootOptionCnt == 1) {
    goto EXIT;
  }

  BootOptionSizePool = AllocateZeroPool (BootOptionCnt * sizeof (UINTN));
  BootOptionPool     = AllocateZeroPool (BootOptionCnt * sizeof (UINT8 *));
  if (BootOptionSizePool == NULL || BootOptionPool == NULL) {
    goto EXIT;
  }
  Index = 0;
  while (Index < BootOptionCnt) {
    UnicodeSPrint (BootIndex, sizeof (BootIndex), L"Boot%04x", BootOrder[Index]);
    BootOptionPool[Index] = BdsLibGetVariableAndSize (
                              BootIndex,
                              &gEfiGlobalVariableGuid,
                              (BootOptionSizePool + Index)
                              );
    if (NULL == BootOptionPool[Index]) {
      goto EXIT;
    }
    Index++;
  }

  //
  // Scan and delete redundant option
  //
  for (Index = 0; Index < BootOptionCnt; Index++) {
    if (BootOptionSizePool[Index] == 0) {
      continue;
    }
    for (SearchIndex = Index + 1; SearchIndex < BootOptionCnt; SearchIndex++) {
      if ((BootOptionSizePool[Index] != 0) && (BootOptionSizePool[Index] == BootOptionSizePool[SearchIndex])) {
        CompareResult = CompareMem (
                          BootOptionPool[Index],
                          BootOptionPool[SearchIndex],
                          BootOptionSizePool[Index]
                          );
        if (CompareResult == 0) {
          BootOrderChanged = TRUE;
          BdsLibDeleteBootOption (TmpBootOrder[SearchIndex], BootOrder, &BootOrderSize);
          BootOptionSizePool[SearchIndex] = 0;
        }
      }
    }
  }

  //
  // Restore BootOrder if need.
  //
  if (BootOrderChanged) {
    gRT->SetVariable (
           L"BootOrder",
           &gEfiGlobalVariableGuid,
           EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
           BootOrderSize,
           BootOrder
           );
  }

EXIT:
  if (TmpBootOrder != NULL) {
    gBS->FreePool (TmpBootOrder);
  }
  if (BootOptionSizePool != NULL) {
    gBS->FreePool (BootOptionSizePool);
  }
  if (BootOptionPool != NULL) {
    for (Index = 0; Index < BootOptionCnt; Index++) {
      if (BootOptionPool[Index] != NULL) {
        gBS->FreePool (BootOptionPool[Index]);
      }
    }
    gBS->FreePool (BootOptionPool);
  }
  if (BootOrder != NULL) {
    gBS->FreePool (BootOrder);
  }
}

/**
  Delete the boot option associated with the handle passed in.

  @param  Handle                 The handle which present the device path to create
                                 boot option

  @retval EFI_SUCCESS            Delete the boot option success
  @retval EFI_NOT_FOUND          If the Device Path is not found in the system
  @retval EFI_OUT_OF_RESOURCES   Lack of memory resource
  @retval Other                  Error return value from SetVariable()

**/
EFI_STATUS
BdsLibDeleteOptionFromHandle (
  IN  EFI_HANDLE                 Handle
  )
{
  UINT16                    *BootOrder;
  UINT8                     *BootOptionVar;
  UINTN                     BootOrderSize;
  UINTN                     BootOptionSize;
  EFI_STATUS                Status;
  UINTN                     Index;
  UINT16                    BootOption[BOOT_OPTION_MAX_CHAR];
  UINTN                     DevicePathSize;
  UINTN                     OptionDevicePathSize;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *OptionDevicePath;
  UINT8                     *TempPtr;

  Status        = EFI_SUCCESS;
  BootOrder     = NULL;
  BootOrderSize = 0;

  //
  // Check "BootOrder" variable, if no, means there is no any boot order.
  //
  BootOrder = BdsLibGetVariableAndSize (
                L"BootOrder",
                &gEfiGlobalVariableGuid,
                &BootOrderSize
                );
  if (BootOrder == NULL) {
    return EFI_NOT_FOUND;
  }

  //
  // Convert device handle to device path protocol instance
  //
  DevicePath = DevicePathFromHandle (Handle);
  if (DevicePath == NULL) {
    return EFI_NOT_FOUND;
  }
  DevicePathSize = GetDevicePathSize (DevicePath);

  //
  // Loop all boot order variable and find the matching device path
  //
  Index = 0;
  while (Index < BootOrderSize / sizeof (UINT16)) {
    UnicodeSPrint (BootOption, sizeof (BootOption), L"Boot%04x", BootOrder[Index]);
    BootOptionVar = BdsLibGetVariableAndSize (
                      BootOption,
                      &gEfiGlobalVariableGuid,
                      &BootOptionSize
                      );

    if (BootOptionVar == NULL) {
      FreePool (BootOrder);
      return EFI_OUT_OF_RESOURCES;
    }

    TempPtr = BootOptionVar;
    TempPtr += sizeof (UINT32) + sizeof (UINT16);
    TempPtr += StrSize ((CHAR16 *) TempPtr);
    OptionDevicePath = (EFI_DEVICE_PATH_PROTOCOL *) TempPtr;
    OptionDevicePathSize = GetDevicePathSize (OptionDevicePath);

    //
    // Check whether the device path match
    //
    if ((OptionDevicePathSize == DevicePathSize) &&
        (CompareMem (DevicePath, OptionDevicePath, DevicePathSize) == 0)) {
      BdsLibDeleteBootOption (BootOrder[Index], BootOrder, &BootOrderSize);
      FreePool (BootOptionVar);
      break;
    }

    FreePool (BootOptionVar);
    Index++;
  }

  //
  // Adjust number of boot option for "BootOrder" variable.
  //
  Status = gRT->SetVariable (
                  L"BootOrder",
                  &gEfiGlobalVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  BootOrderSize,
                  BootOrder
                  );

  FreePool (BootOrder);

  return Status;
}


/**
  Compare device path to check if this boot device is already exist in BootOrder.

  @param  DevicePath           Device path of the query boot device

  @retval TRUE                 The device is already in BootOrder variable.
  @retval FALSE                The device is not in BootOrder variable.

**/
BOOLEAN
IsAlreadyInBootOrder (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  )
{
  UINTN                     DevicePathSize;
  UINT16                    *BootOrder;
  UINTN                     BootOrderSize;
  UINTN                     BootDevNum;
  UINTN                     Index;
  UINT16                    BootOptionName[20];
  UINTN                     VariableSize;
  UINT8                     *BootOption;
  UINT8                     *WorkingPtr;
  EFI_DEVICE_PATH_PROTOCOL  *BootOptionDevicePath;
  UINTN                     BootOptionDevicePathSize;

  DevicePathSize = GetDevicePathSize (DevicePath);

  BootOrder = BdsLibGetVariableAndSize (
                L"BootOrder",
                &gEfiGlobalVariableGuid,
                &BootOrderSize
                );
  if (BootOrder == NULL) {
    return FALSE;
  }

  BootDevNum = BootOrderSize / sizeof (UINT16);
  for (Index = 0; Index < BootDevNum; Index++) {
    UnicodeSPrint (BootOptionName, sizeof (BootOptionName), L"Boot%04x", BootOrder[Index]);
    BootOption = BdsLibGetVariableAndSize (
                   BootOptionName,
                   &gEfiGlobalVariableGuid,
                   &VariableSize
                   );
    if (BootOption == NULL) {
      continue;
    }

    //
    // Find device path in Bootxxxx variable
    //
    WorkingPtr = BootOption;
    WorkingPtr += sizeof (UINT32);
    WorkingPtr += sizeof (UINT16);
    WorkingPtr += (UINTN) StrSize ((UINT16 *) WorkingPtr);
    BootOptionDevicePath     = (EFI_DEVICE_PATH_PROTOCOL *) WorkingPtr;
    BootOptionDevicePathSize = GetDevicePathSize (BootOptionDevicePath);

    if (DevicePathSize == BootOptionDevicePathSize &&
        CompareMem (DevicePath, BootOptionDevicePath, BootOptionDevicePathSize) == 0) {
      gBS->FreePool (BootOption);
      gBS->FreePool (BootOrder);
      return TRUE;
    }

    gBS->FreePool (BootOption);
  }

  gBS->FreePool (BootOrder);

  return FALSE;
}

/**
  Get the hash value of shell file

  @param  Handle               FV handle value
  @param  HashValue            Output hash value of file

  @retval EFI_SUCCESS          Get hash value of file successfully.
  @retval EFI_ABORTED          Read file fail.
  @retval Other                HandleProtocol or AllocatePage or CalculateCrc32 fail.

**/
EFI_STATUS
GetShellFileHashValue (
  IN  EFI_HANDLE                  Handle,
  OUT UINT32                      *HashValue
  )
{
  EFI_STATUS                      Status;
  EFI_FIRMWARE_VOLUME2_PROTOCOL   *Fv;
  UINT8                           *File;
  UINTN                           Size;
  EFI_FV_FILETYPE                 Type;
  EFI_FV_FILE_ATTRIBUTES          Attributes;
  UINT32                          AuthenticationStatus;
  UINTN                           PageNum;

  PageNum = 0;

  Status = gBS->HandleProtocol (
                  Handle,
                  &gEfiFirmwareVolume2ProtocolGuid,
                  (VOID **) &Fv
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  File = NULL;
  Status = Fv->ReadFile (
                 Fv,
                 PcdGetPtr(PcdShellFile),
                 (VOID **) &File,
                 &Size,
                 &Type,
                 &Attributes,
                 &AuthenticationStatus
                 );
  if (Status == EFI_WARN_BUFFER_TOO_SMALL || Status == EFI_BUFFER_TOO_SMALL) {
    PageNum = EFI_SIZE_TO_PAGES(Size);
    Status = gBS->AllocatePages (
                    AllocateMaxAddress,
                    EfiBootServicesData,
                    PageNum,
                    (EFI_PHYSICAL_ADDRESS *) &File
                    );
    if (EFI_ERROR(Status)) {
      return Status;
    }

    Size = EFI_PAGES_TO_SIZE(PageNum);
    Status = Fv->ReadFile (
                   Fv,
                   PcdGetPtr(PcdShellFile),
                   (VOID **) &File,
                   &Size,
                   &Type,
                   &Attributes,
                   &AuthenticationStatus
                   );
    if (Status != EFI_SUCCESS) {
      gBS->FreePages ((EFI_PHYSICAL_ADDRESS) (UINTN) File, PageNum);
      return EFI_ABORTED;
    }
  }

  if (Status != EFI_SUCCESS) {
    return EFI_ABORTED;
  }

  Status = gBS->CalculateCrc32 (File, Size, HashValue);

  if (PageNum != 0) {
    gBS->FreePages ((EFI_PHYSICAL_ADDRESS) (UINTN) File, PageNum);
  } else {
    gBS->FreePool (File);
  }

  return Status;
}


/**
  Remove console out variable

  @param  OptionName           Boot option string
  @param  Description          Description of boot option
  @param  BootOptionHashValue  Hase value of shell boot option

  @retval EFI_SUCCESS          Success to update shell device path in BootXXXX variable.
  @retval EFI_NOT_FOUND        Can not find the corresponding device path by hash value.
  @retval Other                Update variable fail.

**/
EFI_STATUS
UpdateShellDevicePath (
  IN CHAR16                      *OptionName,
  IN CHAR16                      *Description,
  IN UINT32                      BootOptionHashValue
  )
{
  EFI_STATUS                               Status;
  UINTN                                    Index;
  UINTN                                    FvHandleCount;
  EFI_HANDLE                               *FvHandleBuffer;
  EFI_DEVICE_PATH_PROTOCOL                 *DevicePath;
  MEDIA_FW_VOL_FILEPATH_DEVICE_PATH        ShellNode;
  UINT8                                    OptionalData[SHELL_OPTIONAL_DATA_SIZE];
  UINT32                                   HashValue;

  Status = EFI_NOT_FOUND;
  FvHandleCount = 0;
  FvHandleBuffer = NULL;

  gBS->LocateHandleBuffer (
         ByProtocol,
         &gEfiFirmwareVolume2ProtocolGuid,
         NULL,
         &FvHandleCount,
         &FvHandleBuffer
         );
  for (Index = 0; Index < FvHandleCount; Index++) {
    Status = GetShellFileHashValue (FvHandleBuffer[Index], &HashValue);
    if (EFI_ERROR (Status) || (BootOptionHashValue != HashValue)) {
      continue;
    }

    DevicePath = DevicePathFromHandle (FvHandleBuffer[Index]);
    EfiInitializeFwVolDevicepathNode (&ShellNode, PcdGetPtr(PcdShellFile));
    DevicePath = AppendDevicePathNode (DevicePath, (EFI_DEVICE_PATH_PROTOCOL *) &ShellNode);

    if (IsAlreadyInBootOrder (DevicePath)) {
      gBS->FreePool (DevicePath);
      continue;
    }

    gBS->CopyMem (OptionalData    , "RC"      , 2);
    gBS->CopyMem (&OptionalData[2], &HashValue, 4);

    Status = BdsLibUpdateOptionVar (
               OptionName,
               DevicePath,
               Description,
               OptionalData,
               SHELL_OPTIONAL_DATA_SIZE
               );
    gBS->FreePool (DevicePath);
    if (!EFI_ERROR (Status)) {
      break;
    }
  }

  if (FvHandleCount != 0) {
    gBS->FreePool (FvHandleBuffer);
  }

  if (Index == FvHandleCount) {
    return EFI_NOT_FOUND;
  }

  return Status;
}


/**
  Check the Bootxxxx number of device is EFI device or legacy device.

  @param  NumberOfDevice      The device number in BootOrder variable (ex: Boot0001,
                              then NumberOfDevice is 1)
  @retval TRUE                The device is EFI device.
  @retval FALSE               The device is legacy device.

**/
STATIC
BOOLEAN
IsEfiDevice (
  IN     UINT16    NumberOfDevice
  )
{
  UINT16                    BootOptionName[20];
  UINTN                     BootOptionSize;
  UINT8                     *BootOption;
  UINT8                     *WorkingPtr;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  BOOLEAN                   IsEfiDevice;

  UnicodeSPrint (
    BootOptionName,
    sizeof (BootOptionName),
    L"Boot%04x",
    NumberOfDevice
    );
  BootOption = BdsLibGetVariableAndSize (
                 BootOptionName,
                 &gEfiGlobalVariableGuid,
                 &BootOptionSize
                 );
  ASSERT (BootOption != NULL);
  if (BootOption == NULL) {
    return FALSE;
  }

  //
  // Find device path in Bootxxxx variable
  //
  WorkingPtr = BootOption;
  WorkingPtr += sizeof (UINT32);
  WorkingPtr += sizeof (UINT16);
  WorkingPtr += (UINTN) StrSize ((UINT16 *) WorkingPtr);
  DevicePath = (EFI_DEVICE_PATH_PROTOCOL *) WorkingPtr;

  IsEfiDevice = TRUE;
  if ((BBS_DEVICE_PATH == DevicePath->Type) && (BBS_BBS_DP == DevicePath->SubType)) {
    IsEfiDevice = FALSE;
  }

  return IsEfiDevice;
}


/**
  Check the iput device path is wheter a USB device path or not.

  @param  DevicePath           Pointer to device path instance

  @retval TRUE                 It is a USB device path.
  @retval FALSE                It isn't a USB device path.

**/
BOOLEAN
IsUsbDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  )
{
  BOOLEAN                      IsEfiUsb;
  EFI_DEVICE_PATH_PROTOCOL     *FullDevicePath;
  EFI_DEVICE_PATH_PROTOCOL     *WorkingDevicePath;


  if (DevicePath == NULL) {
    return FALSE;
  }

  IsEfiUsb = FALSE;
  FullDevicePath = BdsExpandPartitionPartialDevicePathToFull ((HARDDRIVE_DEVICE_PATH *) DevicePath);
  WorkingDevicePath = (FullDevicePath == NULL) ? DevicePath : FullDevicePath;
  while (!IsDevicePathEnd (WorkingDevicePath)) {
    if (DevicePathType (WorkingDevicePath) == MESSAGING_DEVICE_PATH && DevicePathSubType (WorkingDevicePath) == MSG_USB_DP) {
      IsEfiUsb = TRUE;
      break;
    }
    WorkingDevicePath = NextDevicePathNode (WorkingDevicePath);
  }
  if (FullDevicePath != NULL) {
    gBS->FreePool (FullDevicePath);
  }

  return IsEfiUsb;
}


/**
  According to input parameter to adjust boot order to EFI device first or
  legacy device first.

  @param  EfiDeviceFirst       TRUE : Indicate EFI device first.
                               FALSE: Indicate legacy device first.
  @param  DeviceCount          Total device count
  @param  BootOrder            Pointer to BootOrder.

  @retval EFI_SUCCESS           Adjust boot order successful.
  @retval EFI_INVALID_PARAMETER Input parameter is invalid.

**/
EFI_STATUS
AdjustBootOrder (
  IN      BOOLEAN      EfiDeviceFirst,
  IN      UINTN        DeviceCount,
  IN OUT  UINT16       *BootOrder
  )
{
  UINTN               BootOrderIndex;
  UINT16              *EfiDevicOrder;
  UINTN               EfiDeviceIndex;
  UINT16              *LegacyDeviceOrder;
  UINTN               LegacyDeviceIndex;

  if (DeviceCount == 0 || BootOrder == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  EfiDevicOrder = AllocateZeroPool (DeviceCount * sizeof (CHAR16));
  if (EfiDevicOrder == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  LegacyDeviceOrder = AllocateZeroPool (DeviceCount * sizeof (CHAR16));
  if (LegacyDeviceOrder == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  EfiDeviceIndex = 0;
  LegacyDeviceIndex = 0;

  //
  // According boot type (EFI or legacy) to put boot order respective buffer
  //
  for (BootOrderIndex = 0; BootOrderIndex < DeviceCount; BootOrderIndex++) {
    if (IsEfiDevice (BootOrder[BootOrderIndex])) {
      EfiDevicOrder[EfiDeviceIndex++] = BootOrder[BootOrderIndex];
    } else {
      LegacyDeviceOrder[LegacyDeviceIndex++] = BootOrder[BootOrderIndex];
    }
  }

  //
  // Adjust boot order depend on EFI device first or legacy device first
  //
  if (EfiDeviceFirst) {
    CopyMem (BootOrder, EfiDevicOrder, EfiDeviceIndex * sizeof (CHAR16));
    CopyMem (&BootOrder[EfiDeviceIndex], LegacyDeviceOrder, LegacyDeviceIndex * sizeof (CHAR16));
  } else {
    CopyMem (BootOrder, LegacyDeviceOrder, LegacyDeviceIndex * sizeof (CHAR16));
    CopyMem (&BootOrder[LegacyDeviceIndex], EfiDevicOrder, EfiDeviceIndex * sizeof (CHAR16));
  }

  gBS->FreePool (EfiDevicOrder);
  gBS->FreePool (LegacyDeviceOrder);
  return EFI_SUCCESS;
}


/**
  Get PCI device path from the list of device path

  @param  DevicePath           The list of Device patch

  @retval DevicePath           PCI device path
  @retval                      No PCI device path

**/
PCI_DEVICE_PATH *
GetPciDevicePath (
  EFI_DEVICE_PATH_PROTOCOL *DevicePath
  )
{
  if (DevicePath == NULL) {
    return NULL;
  }

  while (!IsDevicePathEnd (DevicePath)) {
    if (DevicePath->Type == HARDWARE_DEVICE_PATH && DevicePath->SubType == HW_PCI_DP) {
      return (PCI_DEVICE_PATH *) DevicePath;
    } else {
      DevicePath = NextDevicePathNode (DevicePath);
    }
  }

  return NULL;
}


/**
  Based on the input oprom storage device table, check if the device path belongs to oprom storage device.

  @param  DevicePath           The list of device patch
  @param  OpromStorageDev      Pointer of array which contains oprom storage device information
  @param  OpromStorageDevCount Number of oprom storage device

  @retval TRUE                 Device path belongs to oprom storage device.
  @retval FALSE                otherwise

**/
BOOLEAN
IsOpromStorageDev (
  IN EFI_DEVICE_PATH_PROTOCOL      *DevicePath,
  IN OPROM_STORAGE_DEVICE_INFO     *OpromStorageDev,
  IN UINTN                         OpromStorageDevCount
  )
{
  EFI_STATUS                         Status;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL    *PciRootBridgeIo;
  PCI_DEVICE_PATH                    *PciDevicePath;
  PCI_DEVICE_PATH                    *ParentPciDevicePath;
  PCI_DEVICE_PATH                    *NextPciDevicePath;
  EFI_DEVICE_PATH_PROTOCOL           *NextDevicePath;
  UINT8                              DevicePathBusNumber;
  UINT64                             PciAddress;
  UINTN                              Index;

  if (DevicePath == NULL || OpromStorageDev == NULL || OpromStorageDevCount == 0) {
    return FALSE;
  }

  Status = gBS->LocateProtocol (
                  &gEfiPciRootBridgeIoProtocolGuid,
                  NULL,
                  (VOID **) &PciRootBridgeIo
                  );
  if (EFI_ERROR(Status)) {
    return FALSE;
  }

  PciDevicePath = GetPciDevicePath (DevicePath);
  if (PciDevicePath == NULL) {
    return FALSE;
  }

  //
  // Get bus, device and function number form device path.
  //
  ParentPciDevicePath = NULL;
  DevicePathBusNumber = 0;
  while (TRUE) {
    NextDevicePath    = (EFI_DEVICE_PATH_PROTOCOL *) PciDevicePath;
    NextDevicePath    = NextDevicePathNode (NextDevicePath);
    NextPciDevicePath = GetPciDevicePath (NextDevicePath);
    if (NextPciDevicePath == NULL) {
      break;
    }
    //
    // If found next PCI Device Path, current Device Path is a P2P bridge
    //
    ParentPciDevicePath = PciDevicePath;
    PciDevicePath       = NextPciDevicePath;

    PciAddress = EFI_PCI_ADDRESS (
                   DevicePathBusNumber,
                   ParentPciDevicePath->Device,
                   ParentPciDevicePath->Function,
                   PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET
                   );

    Status = PciRootBridgeIo->Pci.Read (
                                    PciRootBridgeIo,
                                    EfiPciWidthUint8,
                                    PciAddress,
                                    1,
                                    &DevicePathBusNumber
                                    );
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
  }

  for (Index = 0; Index < OpromStorageDevCount; Index++) {
    if (DevicePathBusNumber     == OpromStorageDev[Index].Bus &&
        PciDevicePath->Device   == OpromStorageDev[Index].Device &&
        PciDevicePath->Function == OpromStorageDev[Index].Function) {
      return TRUE;
    }
  }

  return FALSE;
}


/**
  Collect option ROM storage device information.

  @param  OpromStorageDev      Pointer of array which contains oprom storage device information
  @param  OpromStorageDevCount Number of oprom storage device

  @retval EFI_SUCCESS             Get Oprom storage device information successfully.
  @retval EFI_INVALID_PARAMETER   Input parameter is NULL.
  @retval Other                   LocateHandleBuffer or AllocatePool fail.

**/
EFI_STATUS
BdsLibGetOpromStorageDevInfo (
  OUT OPROM_STORAGE_DEVICE_INFO **OpromStorageDev,
  OUT UINTN                     *OpromStorageDevCount
  )
{
  EFI_STATUS                       Status;
  UINTN                            NumberPciIoHandles;
  EFI_HANDLE                       *PciIoHandles;
  EFI_PCI_IO_PROTOCOL              *PciIo;
  UINTN                            Segment;
  UINT8                            Class;
  UINTN                            Index;
  EFI_PCI_IO_PROTOCOL              *OpromStorageDevTable[MAX_OPTION_ROM_STORAGE_DEVICE];

  if (OpromStorageDev == NULL || OpromStorageDevCount == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *OpromStorageDev = NULL;
  *OpromStorageDevCount = 0;
  NumberPciIoHandles = 0;
  PciIoHandles = NULL;

  //
  // Find all storage devices which are controlled by option ROM.
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &NumberPciIoHandles,
                  &PciIoHandles
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  for (Index = 0; Index < NumberPciIoHandles; Index++) {
    Status = gBS->HandleProtocol (PciIoHandles[Index], &gEfiPciIoProtocolGuid, (VOID **) &PciIo);
    if (EFI_ERROR (Status) || PciIo->RomSize == 0) {
      continue;
    }

    Status = PciIo->Pci.Read (
                          PciIo,
                          EfiPciWidthUint8,
                          PCI_CLASSCODE_OFFSET + 2,
                          1,
                          &Class
                          );
    if (EFI_ERROR (Status) || Class != PCI_CLASS_MASS_STORAGE) {
      continue;
    }

    if (*OpromStorageDevCount == MAX_OPTION_ROM_STORAGE_DEVICE) {
      DEBUG((EFI_D_ERROR, "WARNING: Unable to get all option ROM storage device information!\n"));
      break;
    }

    OpromStorageDevTable[*OpromStorageDevCount] = PciIo;
    (*OpromStorageDevCount)++;
  }

  if (NumberPciIoHandles) {
    gBS->FreePool (PciIoHandles);
  }

  //
  // From the table of oprom storage device, get info one by one.
  //
  if (*OpromStorageDevCount) {
    Status = gBS->AllocatePool (
                    EfiBootServicesData,
                    *OpromStorageDevCount * sizeof (OPROM_STORAGE_DEVICE_INFO),
                    (VOID **) OpromStorageDev
                    );
    if (EFI_ERROR (Status)) {
      *OpromStorageDevCount = 0;
      return Status;
    }

    for (Index = 0; Index < *OpromStorageDevCount; Index++) {
      PciIo = OpromStorageDevTable[Index];
      PciIo->GetLocation (
               PciIo,
               &Segment,
               &((*OpromStorageDev)[Index].Bus),
               &((*OpromStorageDev)[Index].Device),
               &((*OpromStorageDev)[Index].Function)
               );
    }
  }

  return EFI_SUCCESS;
}

/**
  According input boot order index to delete BIOS created boot option which partition device path is same
  as input boot option. If input boot option is created by BIOS, this function will delete input boot option
  directly.

  @param[in]      BootOrderIndex      Input boot order index.
  @param[in]      PositionPolicy      The new created boot order policy.
  @param[in, out] BootOrder           Boot order list.
  @param[in, out] BootOrderSize       Boot order size by byte.
  @param[in]      BiosCreatedBootNum  The boot option number which is created by BIOS.

  @retval EFI_SUCCESS                 BIOS created boot option is deleted and the delete boot order index is returned.
  @retval EFI_INVALID_PARAMETER       BootOrder is NULL or BiosCreatedIndex is NULL or corresponding Boot#### is incorrect.
  @retval EFI_NOT_FOUND               Cannot find BIOS created boot option.
  @retval EFI_OUT_OF_RESOURCES        Lack of memory resource.
**/
EFI_STATUS
DeleteBiosCreateOption (
  IN      UINTN       BootOrderIndex,
  IN      UINT16      PositionPolicy,
  IN OUT  UINT16      *BootOrder,
  IN OUT  UINTN       *BootOrderSize,
  OUT     UINT16       *BiosCreatedBootNum
  )
{
  UINT16                    BootOption[BOOT_OPTION_MAX_CHAR];
  UINT8                     *BootOptionVar;
  UINTN                     VariableSize;
  UINT16                    BootOrderNum;
  UINT16                    Index;
  EFI_DEVICE_PATH_PROTOCOL  *OptionDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *WorkingDevicePath;
  BOOLEAN                   BiosCreatedFound;

  if (BootOrder == NULL || BootOrderSize == NULL || BiosCreatedBootNum == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  UnicodeSPrint (BootOption, sizeof (BootOption), L"Boot%04x", BootOrder[BootOrderIndex]);
  BootOptionVar = BdsLibGetVariableAndSize (
                    BootOption,
                    &gEfiGlobalVariableGuid,
                    &VariableSize
                    );
  if (BootOptionVar == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  BiosCreatedFound = FALSE;
  OptionDevicePath = NULL;
  if (BdsLibIsBiosCreatedOption (BootOptionVar, VariableSize)) {
    gBS->FreePool (BootOptionVar);
    *BiosCreatedBootNum = BootOrder[BootOrderIndex];
    BdsLibDeleteBootOption (BootOrder[BootOrderIndex], BootOrder, BootOrderSize);
    BiosCreatedFound = TRUE;
  } else {
    gBS->FreePool (BootOptionVar);
    OptionDevicePath = BdsLibGetDevicePathFromBootOption (BootOrder[BootOrderIndex]);
    if (OptionDevicePath == NULL) {
      return EFI_INVALID_PARAMETER;
    }
    BootOrderNum = (UINT16) (*BootOrderSize / sizeof (UINT16));
    for (Index = 0; Index < BootOrderNum; Index++) {
      if (Index == BootOrderIndex) {
        continue;
      }
      UnicodeSPrint (BootOption, sizeof (BootOption), L"Boot%04x", BootOrder[Index]);
      BootOptionVar = BdsLibGetVariableAndSize (
                        BootOption,
                        &gEfiGlobalVariableGuid,
                        &VariableSize
                        );
      if (BootOptionVar == NULL || !BdsLibIsBiosCreatedOption (BootOptionVar, VariableSize)) {
        if (BootOptionVar != NULL) {
          gBS->FreePool (BootOptionVar);
        }
        continue;
      }

      WorkingDevicePath = BdsLibGetDevicePathFromBootOption (BootOrder[Index]);
      if (WorkingDevicePath == NULL) {
         gBS->FreePool (BootOptionVar);
         continue;
      }
      if (MatchPartitionDevicePathNode (OptionDevicePath, (HARDDRIVE_DEVICE_PATH *) WorkingDevicePath) &&
          BdsLibMatchFilePathDevicePathNode (OptionDevicePath, WorkingDevicePath)) {
        BiosCreatedFound = TRUE;
        *BiosCreatedBootNum = BootOrder[Index];
      }

      gBS->FreePool (BootOptionVar);
      gBS->FreePool (WorkingDevicePath);
      if (BiosCreatedFound) {
        UnicodeSPrint (BootOption, sizeof (BootOption), L"Boot%04x", BootOrder[Index]);
        gRT->SetVariable (
               BootOption,
               &gEfiGlobalVariableGuid,
               EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
               0,
               NULL
               );
        if (PositionPolicy == IN_AUTO) {
          //
          // Keep OS created EFI Boot option prioiry, if NewPositionPolicy is auto
          //
          CopyMem (
            &BootOrder[Index],
            BootOrder + Index + 1,
            *BootOrderSize - (Index + 1) * sizeof (UINT16)
            );
        } else {
          //
          // Keep recovery EFI Boot option prioiry, if NewPositionPolicy isn't auto
          //
          BootOrder[Index] = BootOrder[BootOrderIndex];
          CopyMem (
            &BootOrder[BootOrderIndex],
            BootOrder + BootOrderIndex + 1,
            *BootOrderSize - (BootOrderIndex + 1) * sizeof (UINT16)
            );
        }
         *BootOrderSize -= sizeof (UINT16);
        break;
      }
    }
  }

  if (OptionDevicePath != NULL) {
    gBS->FreePool (OptionDevicePath);
  }

  return BiosCreatedFound ? EFI_SUCCESS : EFI_NOT_FOUND;
}



/**
  Delete all invalid EFI boot options.

  @retval EFI_SUCCESS            Delete all invalid boot option success
  @retval EFI_NOT_FOUND          Variable "BootOrder" is not found
  @retval EFI_OUT_OF_RESOURCES   Lack of memory resource
  @retval Other                  Error return value from SetVariable()

**/
EFI_STATUS
BdsDeleteAllInvalidEfiBootOption (
  OUT BOOLEAN               *HaveUefiOs,
  OUT BOOLEAN               *WindowsToGoBootVarExist
  )
{
  UINT16                        *BootOrder;
  UINT8                         *BootOptionVar;
  UINTN                         BootOrderSize;
  UINTN                         BootOptionSize;
  EFI_STATUS                    Status;
  UINTN                         Index;
  UINT16                        BootOption[BOOT_OPTION_MAX_CHAR];
  EFI_DEVICE_PATH_PROTOCOL      *TempDevicePath;
  EFI_DEVICE_PATH_PROTOCOL      *LastDeviceNode;
  EFI_DEVICE_PATH_PROTOCOL      *OptionDevicePath;
  UINT8                         *TempPtr;
  CHAR16                        *Description;
  EFI_HANDLE                    Handle;
  BOOLEAN                       NeedDelete;
  EFI_DEVICE_PATH_PROTOCOL      *NewDevicePath;
  UINT16                        BiosCreatedBootNum;
  KERNEL_CONFIGURATION          SystemConfiguration;
  EFI_STATUS                    SystemConfigStatus;
  EFI_FILE_HANDLE               FileHandle;
  UINTN                         Size;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *Fv;
  EFI_FV_FILETYPE               Type;
  EFI_FV_FILE_ATTRIBUTES        Attributes;
  UINT32                        AuthenticationStatus;
  OPROM_STORAGE_DEVICE_INFO     *OpromStorageDev;
  UINTN                         OpromStorageDevCount;
  UINT8                         *DisableOpromStorageDevBoot;
  BOOLEAN                       UefiOsFound;
  UINTN                         VariableSize;
  UINTN                         OptionDevicePathSize;
  UINT8                         *OptionalData;
  UINT32                        HashValue;
  BOOLEAN                       IsIPv4BootOption;
  BOOLEAN                       IsIPv6BootOption;

  Status        = EFI_SUCCESS;
  BootOrder     = NULL;
  BootOrderSize = 0;
  UefiOsFound         = FALSE;
  *WindowsToGoBootVarExist = FALSE;

  BootOrder = BdsLibGetVariableAndSize (
                L"BootOrder",
                &gEfiGlobalVariableGuid,
                &BootOrderSize
                );
  if (NULL == BootOrder) {
    *HaveUefiOs = UefiOsFound;
    return EFI_NOT_FOUND;
  }

  OpromStorageDev = NULL;
  OpromStorageDevCount = 0;
  DisableOpromStorageDevBoot = BdsLibGetVariableAndSize (
                                 L"DisableOpromStorageDevBoot",
                                 &gEfiGenericVariableGuid,
                                 &Size
                                 );
  if (DisableOpromStorageDevBoot != NULL) {
    BdsLibGetOpromStorageDevInfo (&OpromStorageDev, &OpromStorageDevCount);
  }

  SystemConfigStatus = GetKernelConfiguration (&SystemConfiguration);

  Index = 0;
  while (Index < BootOrderSize / sizeof (UINT16)) {
    if (FeaturePcdGet (PcdAutoCreateDummyBootOption) && BdsLibIsDummyBootOption (BootOrder[Index])) {
      Index++;
      continue;
    }
    UnicodeSPrint (BootOption, sizeof (BootOption), L"Boot%04x", BootOrder[Index]);
    BootOptionVar = BdsLibGetVariableAndSize (
                      BootOption,
                      &gEfiGlobalVariableGuid,
                      &VariableSize
                      );
    if (NULL == BootOptionVar) {
      //
      //If the Boot Device is not exit, we should dynamically adjust the BootOrder
      //
      BdsLibUpdateInvalidBootOrder (&BootOrder, Index, &BootOrderSize);
      if (BootOrder == NULL) {
        break;
      }
      continue;
    }

    if (!ValidateOption (BootOptionVar, VariableSize)) {
      BdsLibDeleteBootOption (BootOrder[Index], BootOrder, &BootOrderSize);
      gBS->FreePool (BootOptionVar);
      continue;
    }

    TempPtr = BootOptionVar;
    TempPtr += sizeof (UINT32) + sizeof (UINT16);
    Description = (CHAR16 *) TempPtr;
    TempPtr += StrSize ((CHAR16 *) TempPtr);
    OptionDevicePath = (EFI_DEVICE_PATH_PROTOCOL *) TempPtr;
    OptionDevicePathSize = GetDevicePathSize (OptionDevicePath);
    BootOptionSize = (UINTN) (TempPtr - BootOptionVar) + OptionDevicePathSize;
    OptionalData = BootOptionVar + BootOptionSize;

    //
    // Skip legacy boot option (BBS boot device)
    //
    if ((DevicePathType (OptionDevicePath) == BBS_DEVICE_PATH) &&
        (DevicePathSubType (OptionDevicePath) == BBS_BBS_DP)) {
      gBS->FreePool (BootOptionVar);
      Index++;
      continue;
    }

    //
    // SCU disable EFI Boot
    //
    if (BdsLibGetBootType () == LEGACY_BOOT_TYPE) {
      BdsLibDeleteBootOption (BootOrder[Index], BootOrder, &BootOrderSize);
      gBS->FreePool (BootOptionVar);
      continue;
    }

    //
    // Skip USB class boot option
    //
    if ((DevicePathType (OptionDevicePath) == MESSAGING_DEVICE_PATH) &&
        (DevicePathSubType (OptionDevicePath) == MSG_USB_CLASS_DP)) {
      *WindowsToGoBootVarExist = TRUE;
      gBS->FreePool (BootOptionVar);
      Index++;
      continue;
    }

    //
    // If system doesn't support USB boot, delete all of USB EFI boot options.
    //
    if (!EFI_ERROR (SystemConfigStatus) && SystemConfiguration.UsbBoot != 0 && IsUsbDevicePath (OptionDevicePath)) {
      BdsLibDeleteBootOption (BootOrder[Index], BootOrder, &BootOrderSize);
      gBS->FreePool (BootOptionVar);
      continue;
    }

    TempDevicePath = OptionDevicePath;
    LastDeviceNode = OptionDevicePath;
    while (!IsDevicePathEnd (TempDevicePath)) {
      LastDeviceNode = TempDevicePath;
      TempDevicePath = NextDevicePathNode (TempDevicePath);
    }

    //
    // Skip boot option for internal Shell if read file successfully.
    //
    if (EfiGetNameGuidFromFwVolDevicePathNode ((MEDIA_FW_VOL_FILEPATH_DEVICE_PATH *) LastDeviceNode) != NULL) {
      TempDevicePath = OptionDevicePath;
      Status = gBS->LocateDevicePath (
                      &gEfiFirmwareVolume2ProtocolGuid,
                      &TempDevicePath,
                      &Handle
                      );
      if (!EFI_ERROR (Status)) {
        gBS->HandleProtocol (
               Handle,
               &gEfiFirmwareVolume2ProtocolGuid,
               (VOID **) &Fv
               );
        Status = Fv->ReadFile (
                       Fv,
                       PcdGetPtr(PcdShellFile),
                       NULL,
                       &Size,
                       &Type,
                       &Attributes,
                       &AuthenticationStatus
                       );
        if (!EFI_ERROR (Status)) {
          gBS->FreePool (BootOptionVar);
          Index++;
          continue;
        }
      }

      if (VariableSize - BootOptionSize == SHELL_OPTIONAL_DATA_SIZE) {
        gBS->CopyMem (&HashValue, &OptionalData[2], 4);
        Status = UpdateShellDevicePath ((CHAR16 *) BootOption, Description, HashValue);
        if (!EFI_ERROR (Status)) {
          gBS->FreePool (BootOptionVar);
          Index++;
          continue;
        }
      }

      BdsLibDeleteBootOption (BootOrder[Index], BootOrder, &BootOrderSize);
      gBS->FreePool (BootOptionVar);
      continue;
    }

    NeedDelete = TRUE;
    //
    // Check if it's a valid UEFI OS boot option
    //
    if ((DevicePathType (LastDeviceNode) == MEDIA_DEVICE_PATH) &&
        (DevicePathSubType (LastDeviceNode) == MEDIA_FILEPATH_DP)) {
      NewDevicePath = BdsExpandPartitionPartialDevicePathToFull (
                        (HARDDRIVE_DEVICE_PATH *)OptionDevicePath
                        );
      if (NewDevicePath == NULL) {
        NewDevicePath = OptionDevicePath;
      }

      if (OpromStorageDevCount != 0 &&
          IsOpromStorageDev (NewDevicePath, OpromStorageDev, OpromStorageDevCount)) {
        BdsLibDeleteBootOption (BootOrder[Index], BootOrder, &BootOrderSize);
        gBS->FreePool (BootOptionVar);
        if (NewDevicePath != OptionDevicePath) {
          gBS->FreePool (NewDevicePath);
        }
        continue;
      }

      Status = BdsLibOpenFileFromDevicePath (
                 NewDevicePath,
                 EFI_FILE_MODE_READ,
                 0,
                 &FileHandle
                 );

      if (NewDevicePath != OptionDevicePath) {
        gBS->FreePool (NewDevicePath);
      }

      if (Status == EFI_SUCCESS) {
        NeedDelete = FALSE;
        UefiOsFound = TRUE;
        FileHandle->Close (FileHandle);
      }
      gBS->FreePool (BootOptionVar);
      if (NeedDelete) {
        //
        // Only delete BIOS created boot option, doesn't delete OS created boot option, just don't display and boot these
        // boot options.
        //
        BdsLibDeleteBootOption (BootOrder[Index], BootOrder, &BootOrderSize);
        continue;
      }
      if (!NeedDelete && !BdsLibIsBiosCreatedOption (BootOptionVar, VariableSize)) {
        Status = DeleteBiosCreateOption (Index, SystemConfiguration.NewPositionPolicy, BootOrder, &BootOrderSize, &BiosCreatedBootNum);
        if (!EFI_ERROR (Status)) {
          if (ReadCmos8 (LastBootDevice) == (UINT8) BiosCreatedBootNum) {
            WriteCmos8 (LastBootDevice, (UINT8) BootOrder[Index]);
          }
          continue;
        }
      }

      Index++;
      continue;
    }

    //
    // Check if it's a valid boot option for removable media
    //
    TempDevicePath = OptionDevicePath;
    Status = gBS->LocateDevicePath (
                    &gEfiSimpleFileSystemProtocolGuid,
                    &TempDevicePath,
                    &Handle
                    );
    if (!EFI_ERROR (Status) &&
        (OpromStorageDevCount == 0 ||
         !IsOpromStorageDev (OptionDevicePath, OpromStorageDev, OpromStorageDevCount))) {
      NeedDelete = FALSE;
    }

    //
    // Check if it's a valid boot option for network boot device
    //
    //
    // if PxeBootToLan is disable, delete all network boot option
    //
    if (!EFI_ERROR(SystemConfigStatus) && SystemConfiguration.PxeBootToLan == 1) {
      TempDevicePath = OptionDevicePath;
      Status = gBS->LocateDevicePath (
                      &gEfiLoadFileProtocolGuid,
                      &TempDevicePath,
                      &Handle
                      );
      //
      // Focus on removable media and network boot device
      //
      if (!EFI_ERROR (Status)) {
        if (!FeaturePcdGet (PcdH2ONetworkSupported)) {
          NeedDelete = FALSE;
        } else {
          IsIPv4BootOption = FALSE;
          IsIPv6BootOption = FALSE;

          //
          // Scan Build Option device path, if is IPv4 or IPv6 will trigger boolean
          //
          TempDevicePath = OptionDevicePath;
          while (!IsDevicePathEnd (TempDevicePath)) {
            if (TempDevicePath->Type == MESSAGING_DEVICE_PATH) {
              if (TempDevicePath->SubType == MSG_IPv4_DP) {
                IsIPv4BootOption = TRUE;
              }
              else if (TempDevicePath->SubType == MSG_IPv6_DP) {
                IsIPv6BootOption = TRUE;
              }
            }
            TempDevicePath = NextDevicePathNode (TempDevicePath);
          }

          //
          // Detect user set Network is IPv4/IPv6/Both
          // Will delete delete not selected item
          //
          switch (SystemConfiguration.NetworkProtocol) {
          case UEFI_NETWORK_BOOT_OPTION_IPV4:
            if (IsIPv4BootOption) {
              NeedDelete = FALSE;
            }
            break;

          case UEFI_NETWORK_BOOT_OPTION_IPV6:
            if (IsIPv6BootOption) {
              NeedDelete = FALSE;
            }
            break;

          case UEFI_NETWORK_BOOT_OPTION_BOTH:
            NeedDelete = FALSE;
          }

          //
          // If Boot option is vaild, will not delete
          // Include Unknown LoadFile build option
          //
          if (!IsIPv4BootOption & !IsIPv6BootOption) {
            NeedDelete = FALSE;
          }
        }
      }
    }

    if (NeedDelete) {
      BdsLibDeleteBootOption (BootOrder[Index], BootOrder, &BootOrderSize);
      gBS->FreePool (BootOptionVar);
      continue;
    }

    gBS->FreePool (BootOptionVar);
    Index++;
  }

  if (!EFI_ERROR (SystemConfigStatus)) {
    AdjustBootOrder (
      !((BOOLEAN) SystemConfiguration.BootNormalPriority),
      BootOrderSize / sizeof (UINT16),
      BootOrder
      );
  }

  Status = gRT->SetVariable (
                  L"BootOrder",
                  &gEfiGlobalVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  BootOrderSize,
                  BootOrder
                  );

  if (BootOrder != NULL) {
    gBS->FreePool (BootOrder);
  }
  *HaveUefiOs = UefiOsFound;

  if (DisableOpromStorageDevBoot != NULL) {
    gBS->FreePool (DisableOpromStorageDevBoot);

    if (OpromStorageDevCount) {
      gBS->FreePool (OpromStorageDev);
    }
  }

  return Status;
}


/**
  Add a BDS_COMMON_OPTION to portabl boot option list

  @param  DevicePath           A poitner to device path instance.
  @param  BdsBootOptionList    The header of the link list which indexed all current boot options

  @retval EFI_SUCCESS          Finished all the boot device enumerate and create
                               the boot option base on that boot device

**/
EFI_STATUS
RegisterPortalbeBootOption (
  IN EFI_DEVICE_PATH_PROTOCOL    *DevicePath,
  IN OUT LIST_ENTRY              *BdsBootOptionList
 )
{
  BDS_COMMON_OPTION         *Option;

  Option = AllocateZeroPool (sizeof (BDS_COMMON_OPTION));
  if (Option == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Option->Signature   = BDS_LOAD_OPTION_SIGNATURE;
  Option->DevicePath  = AllocateZeroPool (GetDevicePathSize (DevicePath));
  CopyMem (Option->DevicePath, DevicePath, GetDevicePathSize (DevicePath));
  Option->Attribute   = 0;
  Option->Description = NULL;
  Option->LoadOptions = NULL;
  Option->LoadOptionsSize = 0;
  InsertTailList (BdsBootOptionList, &Option->Link);

  return EFI_SUCCESS;
}

/**
  Check if the input device path contains the query type and subtype or not.

  @param  DevicePath           Pointer to device path instance
  @param  QueryType            Query type of device path
  @param  QuerySubType         Query subtype of device path

  @retval TRUE                 Remove variable was success.
  @retval FALSE                DevicePath doesn't contain the query type and subtype.

**/
BOOLEAN
CheckDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath,
  IN UINT8                     QueryType,
  IN UINT8                     QuerySubType
  )
{
  BOOLEAN                      Match;
  EFI_DEVICE_PATH_PROTOCOL     *FullDevicePath;
  EFI_DEVICE_PATH_PROTOCOL     *WorkingDevicePath;

  Match = FALSE;

  if (DevicePath == NULL) {
    return FALSE;
  }

  FullDevicePath = BdsExpandPartitionPartialDevicePathToFull ((HARDDRIVE_DEVICE_PATH *) DevicePath);
  WorkingDevicePath = (FullDevicePath == NULL) ? DevicePath : FullDevicePath;

  while (!IsDevicePathEnd (WorkingDevicePath)) {
    if (DevicePathType (WorkingDevicePath) == QueryType &&
        DevicePathSubType (WorkingDevicePath) == QuerySubType) {
      Match = TRUE;
      break;
    }

    WorkingDevicePath = NextDevicePathNode (WorkingDevicePath);
  }

  if (FullDevicePath != NULL) {
    gBS->FreePool (FullDevicePath);
  }

  return Match;
}


/**
  Check if it has the default removable file or not.

  @param  Handle        A pointer to a device handle.

  @retval TRUE          It has the default removable file.
  @retval FALSE         It does not has the default removable file.

**/
BOOLEAN
HaveDefaultRemovableFile (
  IN   EFI_HANDLE         Handle
  )
{
  EFI_DEVICE_PATH_PROTOCOL      *FilePath;
  EFI_FILE_HANDLE               FileHandle;
  EFI_STATUS                    Status;

  FilePath = FileDevicePath (Handle, EFI_REMOVABLE_MEDIA_FILE_NAME);
  Status = BdsLibOpenFileFromDevicePath (
             FilePath,
             EFI_FILE_MODE_READ,
             0,
             &FileHandle
             );
  gBS->FreePool (FilePath);
  if (Status == EFI_SUCCESS) {
    FileHandle->Close (FileHandle);
    return TRUE;
  }

  return FALSE;
}


/**
 Check if it is a generic UEFI bootable OS or not.

 @param[in] Handle             A pointer to a device handle.

 @retval TRUE                  This is a generic UEFI Bootable OS.
 @retval FALSE                 This is not a generic UEFI Bootable OS.
**/
BOOLEAN
IsGenericUefiBootOs (
  IN  EFI_HANDLE                Handle
  )
{
  UINTN                         MaxUefiOs;
  UINTN                         Index;
  UINTN                         EnvType;
  EFI_DEVICE_PATH_PROTOCOL      *FilePath;
  EFI_FILE_HANDLE               FileHandle;
  EFI_STATUS                    Status;

  MaxUefiOs = sizeof (mBdsGeneralUefiBootOs) / sizeof (BDS_GENERAL_UEFI_BOOT_ARRAY_OS);

  for(Index = 0; Index < MaxUefiOs; Index++) {
    for (EnvType = 0; EnvType < MAX_BIOS_ENVIRONMENT; EnvType++) {
      if (mBdsGeneralUefiBootOs[Index][EnvType].FilePathString != NULL) {
        FilePath = FileDevicePath (Handle, mBdsGeneralUefiBootOs[Index][EnvType].FilePathString);
        Status = BdsLibOpenFileFromDevicePath (
                   FilePath,
                   EFI_FILE_MODE_READ,
                   0,
                   &FileHandle
                   );
        gBS->FreePool (FilePath);

        if (Status == EFI_SUCCESS) {
          FileHandle->Close (FileHandle);
          return TRUE;
        }
      }
    }
  }

  return FALSE;
}


/**
 Create generic UEFI bootable OS.

 @param[in] Handle             A pointer to a device handle.
 @param[in] BdsBootOptionList  The header of the link list which indexed all current boot options

 @retval EFI_SUCCESS           Create generic UEFI OS success
 @retval Other                 Register new boot option fail
**/
EFI_STATUS
CreateGenericUefiBootOs (
  IN  EFI_HANDLE                Handle,
  IN  LIST_ENTRY                *BdsBootOptionList
  )
{
  UINTN                         MaxUefiOs;
  UINTN                         Index;
  UINTN                         EnvType;
  EFI_DEVICE_PATH_PROTOCOL      *FilePath;
  EFI_FILE_HANDLE               FileHandle;
  EFI_STATUS                    Status;
  EFI_STATUS                    RegisterStatus;
  UINT8                         *TempPtr;
  EFI_DEVICE_PATH_PROTOCOL      *TempFilePath;

  RegisterStatus = EFI_SUCCESS;
  MaxUefiOs = sizeof (mBdsGeneralUefiBootOs) / sizeof (BDS_GENERAL_UEFI_BOOT_ARRAY_OS);

  for(Index = 0; Index < MaxUefiOs; Index++) {
    for (EnvType = 0; EnvType < MAX_BIOS_ENVIRONMENT; EnvType++) {
      if (mBdsGeneralUefiBootOs[Index][EnvType].FilePathString == NULL) {
        continue;
      }
      FilePath = FileDevicePath (Handle, mBdsGeneralUefiBootOs[Index][EnvType].FilePathString);
      if (FilePath == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      Status = BdsLibOpenFileFromDevicePath (
                 FilePath,
                 EFI_FILE_MODE_READ,
                 0,
                 &FileHandle
                 );
      if (Status == EFI_SUCCESS) {
        //
        // If open generic OS loader file success, register a new boot option.
        //
        FileHandle->Close (FileHandle);

        TempPtr      = (UINT8 *) FilePath;
        TempFilePath = FilePath;

        while (!IsDevicePathEnd (TempFilePath) &&
               (TempFilePath->Type != MEDIA_DEVICE_PATH || TempFilePath->SubType != MEDIA_HARDDRIVE_DP)) {
          TempPtr += DevicePathNodeLength (TempFilePath);
          TempFilePath = (EFI_DEVICE_PATH_PROTOCOL *) TempPtr;
        }
        if (IsDevicePathEnd (TempFilePath)) {
          gBS->FreePool (FilePath);
          continue;
        }

        Status = BdsLibRegisterNewOption (
                   BdsBootOptionList,
                   TempFilePath,
                   mBdsGeneralUefiBootOs[Index][EnvType].Description,
                   L"BootOrder",
                   (UINT8 *) "RC",
                   2
                   );
        if (EFI_ERROR(Status)) {
          ASSERT_EFI_ERROR (Status);
          RegisterStatus = Status;
        }
        gBS->FreePool (FilePath);
        break;
      }
      gBS->FreePool (FilePath);
    }
  }

  return RegisterStatus;
}

/**
 Check if the target file is signed image or not

 @param[in] Handle             A pointer to a device handle.
 @param[in] FileName           Pointer to file name

 @retval TRUE                  This is a signed image
 @retval FALSE                 The file isn't exist, the image is not supported or this isn't a signed image

**/
BOOLEAN
IsSignedImage (
  IN EFI_HANDLE                         Handle,
  IN CHAR16                             *FileName
  )
{
  EFI_STATUS                            Status;
  EFI_IMAGE_DOS_HEADER                  DosHeader;
  EFI_IMAGE_OPTIONAL_HEADER_PTR_UNION   Hdr;
  EFI_IMAGE_OPTIONAL_HEADER_UNION       HdrData;
  UINT16                                Magic;
  EFI_IMAGE_DATA_DIRECTORY              *SecDataDir;


  Hdr.Union = &HdrData;
  Status = BdsLibGetImageHeader (
             Handle,
             FileName,
             &DosHeader,
             Hdr
             );
  if (EFI_ERROR (Status) || !EFI_IMAGE_MACHINE_TYPE_SUPPORTED (Hdr.Pe32->FileHeader.Machine)) {
    return FALSE;
  }

  if (Hdr.Pe32->FileHeader.Machine == EFI_IMAGE_MACHINE_IA64 && Hdr.Pe32->OptionalHeader.Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
    Magic = EFI_IMAGE_NT_OPTIONAL_HDR64_MAGIC;
  } else {
    Magic = Hdr.Pe32->OptionalHeader.Magic;
  }

  if (Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
    //
    // Use PE32 offset.
    //
    SecDataDir = (EFI_IMAGE_DATA_DIRECTORY *) &Hdr.Pe32->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY];
  } else {
    //
    // Use PE32+ offset.
    //
    SecDataDir = (EFI_IMAGE_DATA_DIRECTORY *) &Hdr.Pe32Plus->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY];
  }

  if (SecDataDir->Size != 0) {
    return TRUE;
  }

  return FALSE;
}

/**
  This function uses device handle to check the EFI boot option is Windows To Go device or not

  @param  Handle        A pointer to a device handle.

  @retval TRUE          This is a Windows To Go device
  @retval FALSE         This isn't a Windows To Go device

**/
BOOLEAN
IsWindowsToGo (
  IN   EFI_HANDLE         Handle
  )
{
  EFI_DEVICE_PATH_PROTOCOL    *DevicePath;
  UINTN                       DevicePathType;

  DevicePath     = DevicePathFromHandle (Handle);
  DevicePathType = BdsLibGetBootTypeFromDevicePath (DevicePath);

  if (DevicePathType == BDS_EFI_MESSAGE_USB_DEVICE_BOOT &&
      IsSignedImage (Handle, EFI_REMOVABLE_MEDIA_FILE_NAME)) {
    return TRUE;
  }

  return FALSE;
}

/**
  Delete boot option which device path and input device path is matched

  @param  DevicePath    Pointer to device path instance

  @retval EFI_SUCCESS   Finished all the boot device enumerate and create
                        the boot option base on that boot device
  @retval Other

**/
EFI_STATUS
DeleteRecoveryOption (
  IN EFI_DEVICE_PATH_PROTOCOL      *DevicePath
  )
{
  UINT16                    *BootOrder;
  UINT8                     *BootOptionVar;
  UINTN                     BootOrderSize;
  UINTN                     BootOptionSize;
  EFI_STATUS                Status;
  UINTN                     Index;
  UINT16                    BootOption[BOOT_OPTION_MAX_CHAR];
  EFI_DEVICE_PATH_PROTOCOL  *OptionDevicePath;
  UINT8                     *TempPtr;
  CHAR16                    *Description;
  UINTN                     DevicePathLen;

  Status        = EFI_SUCCESS;
  BootOrder     = NULL;
  BootOrderSize = 0;

  BootOrder = BdsLibGetVariableAndSize (
                L"BootOrder",
                &gEfiGlobalVariableGuid,
                &BootOrderSize
                );
  if (NULL == BootOrder) {
    return EFI_NOT_FOUND;
  }

  Index = 0;
  while (Index < BootOrderSize / sizeof (UINT16)) {
    UnicodeSPrint (BootOption, sizeof (BootOption), L"Boot%04x", BootOrder[Index]);
    BootOptionVar = BdsLibGetVariableAndSize (
                      BootOption,
                      &gEfiGlobalVariableGuid,
                      &BootOptionSize
                      );
    if (NULL == BootOptionVar) {
      //
      //If the Boot Device is not exit, we should dynamically adjust the BootOrder
      //
      BdsLibUpdateInvalidBootOrder (&BootOrder, Index, &BootOrderSize);
      if (BootOrder == NULL) {
        break;
      }
      continue;
    }

    TempPtr = BootOptionVar;
    TempPtr += sizeof (UINT32);
    DevicePathLen = (UINTN) (*((UINT16 *) TempPtr));
    TempPtr += sizeof (UINT16);
    Description = (CHAR16 *) TempPtr;
    TempPtr += StrSize ((CHAR16 *) TempPtr);
    OptionDevicePath = (EFI_DEVICE_PATH_PROTOCOL *) TempPtr;

    if (DevicePathLen == GetDevicePathSize (DevicePath) && CompareMem (OptionDevicePath, DevicePath, DevicePathLen) == 0) {
      BdsLibDeleteBootOption (BootOrder[Index], BootOrder, &BootOrderSize);
      gBS->FreePool (BootOptionVar);
      continue;
    }
    gBS->FreePool (BootOptionVar);
    Index++;
  }

  Status = gRT->SetVariable (
                  L"BootOrder",
                  &gEfiGlobalVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  BootOrderSize,
                  BootOrder
                  );

  if (BootOrder != NULL) {
    gBS->FreePool (BootOrder);
  }
  return Status;
}

/*++

Routine Description:

  Enumernate all Windows To Go devices and return device number.

Arguments:

  DeviceNum      - Pointer to device number

Returns:

  EFI_SUCCESS    - Enumernate all Windows To Go devices success
  EFI_NOT_FOUND  - Windows To Go device is not found

--*/
EFI_STATUS
EnumerateAllWindowsToGoDevice (
  OUT UINT16                    *DeviceNum
  )
{
  UINTN                         Index;
  UINTN                         NumberFileSystemHandles;
  EFI_HANDLE                    *FileSystemHandles;
  EFI_DEVICE_PATH_PROTOCOL      *FilePath;
  UINT16                        Count;

  Count                   = 0;
  NumberFileSystemHandles = 0;

  gBS->LocateHandleBuffer (
         ByProtocol,
         &gEfiSimpleFileSystemProtocolGuid,
         NULL,
         &NumberFileSystemHandles,
         &FileSystemHandles
         );
  for (Index = 0; Index < NumberFileSystemHandles; Index++) {
    if (IsWindowsToGo (FileSystemHandles[Index])) {
      //
      // Add to Windows To Go list if this is USB Entry for Windows To Go
      //
      FilePath = FileDevicePath (FileSystemHandles[Index], EFI_REMOVABLE_MEDIA_FILE_NAME);
      RegisterPortalbeBootOption (FilePath, &mWindowsToGoDeviceList);
      gBS->FreePool (FilePath);
      Count++;
    }
  }

  if (NumberFileSystemHandles != 0) {
    gBS->FreePool (FileSystemHandles);
  }

  if (Count == 0) {
    return EFI_NOT_FOUND;
  }

  *DeviceNum = Count;

  return EFI_SUCCESS;
}

/**
  Set Windows To Go variable

  @param[in] DeviceNum    Windows To Go device number

  @retval EFI_SUCCESS           Set Windows To Go device variable success
  @retval EFI_NOT_FOUND         Windows To Go device is not found
  @retval EFI_OUT_OF_RESOURCES  Allocate memory fail
**/
EFI_STATUS
SetWindowsToGoVariable (
  IN UINT16                     DeviceNum
  )
{
  EFI_STATUS                    Status;
  BDS_COMMON_OPTION             *Option;
  UINT8                         *Ptr;
  UINT8                         *VarData;
  UINTN                         Size;
  UINTN                         DevPathSize;

  if (IsListEmpty (&mWindowsToGoDeviceList)) {
    return EFI_NOT_FOUND;
  }

  Option      = CR (mWindowsToGoDeviceList.ForwardLink, BDS_COMMON_OPTION, Link, BDS_LOAD_OPTION_SIGNATURE);
  DevPathSize = GetDevicePathSize (Option->DevicePath);
  Size        = DevPathSize + sizeof(UINT16);
  VarData     = AllocatePool (Size);
  if (VarData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Ptr = VarData;
  CopyMem (Ptr, Option->DevicePath, DevPathSize);
  Ptr += DevPathSize;
  CopyMem (Ptr, &DeviceNum, sizeof(UINT16));
  Status = gRT->SetVariable (
                  L"WindowsToGo",
                  &gEfiGenericVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS,
                  Size,
                  VarData
                  );
  FreePool (VarData);

  return Status;
}

/**
  Enumernate all Windows To Go devices and update them to list.

**/
VOID
UpdateWindowsToGoList (
  VOID
  )
{
  EFI_STATUS                    Status;
  UINT16                        DeviceNum;

  Status = EnumerateAllWindowsToGoDevice (&DeviceNum);
  if (!EFI_ERROR (Status)) {
    SetWindowsToGoVariable (DeviceNum);
  }
}

/**
  According to handle to check this handle is whether contains EFI system partition.

  @param[in]  Handle      The handle for the protocol interface that is being opened.

  @retval     TRUE        This handle contains EFI system partition.
  @retval     FALSE       This handle doesn't contain EFI system partition.
**/
STATIC
BOOLEAN
IsEfiSystemPartition (
  IN EFI_HANDLE       Handle
  )
{
  EFI_STATUS      Status;

  Status = gBS->OpenProtocol (
                  Handle,
                  &gEfiPartTypeSystemPartGuid,
                  NULL,
                  NULL,
                  NULL,
                  EFI_OPEN_PROTOCOL_TEST_PROTOCOL
                  );
  return Status == EFI_SUCCESS ? TRUE : FALSE;
}

/**
  For EFI boot option, BDS separate them as six types:
  1. Network - The boot option points to the SimpleNetworkProtocol device.
               Bds will try to automatically create this type boot option when enumerate.
  2. Shell   - The boot option points to internal flash shell.
               Bds will try to automatically create this type boot option when enumerate.
  3. Removable BlockIo      - The boot option only points to the removable media
                              device, like USB flash disk, DVD, Floppy etc.
                              These device should contain a *removable* blockIo
                              protocol in their device handle.
                              Bds will try to automatically create this type boot option
                              when enumerate.
  4. Fixed BlockIo          - The boot option only points to a Fixed blockIo device,
                              like HardDisk.
                              These device should contain a *fixed* blockIo
                              protocol in their device handle.
                              BDS will skip fixed blockIo devices, and NOT
                              automatically create boot option for them. But BDS
                              will help to delete those fixed blockIo boot option,
                              whose description rule conflict with other auto-created
                              boot options.
  5. Non-BlockIo Simplefile - The boot option points to a device whose handle
                              has SimpleFileSystem Protocol, but has no blockio
                              protocol. These devices do not offer blockIo
                              protocol, but BDS still can get the
                              \EFI\BOOT\boot{machinename}.EFI by SimpleFileSystem
                              Protocol.
  6. File    - The boot option points to a file. These boot options are usually
               created by user manually or OS loader. BDS will not delete or modify
               these boot options.

  This function will enumerate all possible boot device in the system, and
  automatically create boot options for Network, Shell, Removable BlockIo,
  and Non-BlockIo Simplefile devices.
  It will only execute once of every boot.

  @param  BdsBootOptionList      The header of the link list which indexed all
                                 current boot options

  @retval EFI_SUCCESS            Finished all the boot device enumerate and create
                                 the boot option base on that boot device

  @retval EFI_OUT_OF_RESOURCES   Failed to enumerate the boot device and create the boot option list
**/
EFI_STATUS
BdsLibEnumerateAllBootOption (
  IN     BOOLEAN             ForceEnumerateAll,
  IN OUT LIST_ENTRY          *BdsBootOptionList
  )
{
  EFI_STATUS                    Status;
  UINTN                         NumberFileSystemHandles;
  EFI_HANDLE                    *FileSystemHandles;
  EFI_BLOCK_IO_PROTOCOL         *BlkIo;
  UINTN                         Index;
  UINTN                         NumberLoadFileHandles;
  EFI_HANDLE                    *LoadFileHandles;
  VOID                          *ProtocolInstance;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *Fv;
  UINTN                         FvHandleCount;
  EFI_HANDLE                    *FvHandleBuffer;
  EFI_FV_FILETYPE               Type;
  UINTN                         Size;
  EFI_FV_FILE_ATTRIBUTES        Attributes;
  UINT32                        AuthenticationStatus;
  UINT16                        FloppyNumber;
  UINT16                        CdromNumber;
  UINT16                        UsbNumber;
  UINT16                        ScsiNumber;
  UINT16                        IScsiNumber;
  UINT16                        MiscNumber;
  UINT16                        HddNumber;
  UINT16                        InternalShellNumber;
  EFI_DEVICE_PATH_PROTOCOL      *DevicePath;
  UINTN                         DevicePathType;
  CHAR16                        Buffer[128];
  KERNEL_CONFIGURATION          SystemConfiguration;
  EFI_STATUS                    SystemConfigStatus;
  OPROM_STORAGE_DEVICE_INFO     *OpromStorageDev;
  UINTN                         OpromStorageDevCount;
  UINT8                         *DisableOpromStorageDevBoot;
  BOOLEAN                       UefiOsFound;
  BOOLEAN                       WindowsToGoBootVarExist;
  STATIC BOOLEAN                Win8FastBootEnum = TRUE;
  STATIC BOOLEAN                DeletedInvalidBootOption = FALSE;
  HARDWARE_BOOT_DEVICE_INFO     *HwBootDeviceInfo;
  UINTN                         HwBootDeviceCount;
  UINTN                         HwNum;
  BOOLEAN                       IsCreated;
  CHAR8                         *PlatLang;
  CHAR8                         *LastLang;
  STATIC BOOLEAN                EnableBootOrderHook = FALSE;


  //
  // PostCode = 27, Get boot device information
  //
  POST_CODE (BDS_ENUMERATE_ALL_BOOT_OPTION);

  FloppyNumber        = 0;
  HddNumber           = 0;
  CdromNumber         = 0;
  UsbNumber           = 0;
  ScsiNumber          = 0;
  IScsiNumber         = 0;
  MiscNumber          = 0;
  InternalShellNumber = 0;
  PlatLang = NULL;
  LastLang = NULL;
  ZeroMem (Buffer, sizeof (Buffer));

  if (!DeletedInvalidBootOption) {
    BdsLibDeleteInvalidBootOptions ();
    DeletedInvalidBootOption = TRUE;
  }

  //
  // If the boot device enumerate happened, just get the boot device from the boot order variable
  //
  if (mEnumBootDevice && !ForceEnumerateAll) {
    LastLang = GetVariable (LAST_ENUM_LANGUAGE_VARIABLE_NAME, &gLastEnumLangGuid);
    PlatLang = GetEfiGlobalVariable (L"PlatformLang");
    ASSERT (PlatLang != NULL);
    if (PlatLang == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    if ((LastLang != NULL) && (AsciiStrCmp (LastLang, PlatLang) == 0)) {
      Status = BdsLibBuildOptionFromVar (BdsBootOptionList, L"BootOrder");
      FreePool (LastLang);
      FreePool (PlatLang);
      return Status;
    } else {
      Status = gRT->SetVariable (
                      LAST_ENUM_LANGUAGE_VARIABLE_NAME,
                      &gLastEnumLangGuid,
                      EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                      AsciiStrSize (PlatLang),
                      PlatLang
                      );
      ASSERT_EFI_ERROR (Status);

      if (LastLang != NULL) {
        FreePool (LastLang);
      }
      FreePool (PlatLang);
    }
  }

  //
  // Notes:
  // this dirty code is to get the legacy boot option from the BBS table and create to variable as the EFI boot option,
  // it should be removed after the CSM can provide legacy boot option directly
  //
  if (BdsLibGetBootType () != EFI_BOOT_TYPE) {
    REFRESH_LEGACY_BOOT_OPTIONS;

    if (BdsLibGetBootType () == LEGACY_BOOT_TYPE) {
      BdsLibRemovedBootOption (FALSE);
    }
  } else {
    BdsLibRemovedBootOption (TRUE);
  }

  if (FeaturePcdGet (PcdAutoCreateDummyBootOption) && BdsLibGetBootType () != LEGACY_BOOT_TYPE) {
    CreateDummyBootOptions ();
    SyncBootOrder ();
  }

  BdsLibDeleteRedundantOption ();

  if (BdsLibIsWin8FastBootActive () && Win8FastBootEnum) {
    //
    // First enumernate is for Win 8 Fast Boot, just skip it for first time.
    //
    Win8FastBootEnum = FALSE;
    return EFI_SUCCESS;
  }

  if (BdsLibGetBootType () != LEGACY_BOOT_TYPE && !EnableBootOrderHook) {
    BdsLibEnableBootOrderHook ();
    EnableBootOrderHook = TRUE;
  }


  //
  // Delete invalid boot option
  //
  BdsDeleteAllInvalidEfiBootOption (&UefiOsFound, &WindowsToGoBootVarExist);

  //
  // SCU disable EFI Boot
  //
  if (BdsLibGetBootType () == LEGACY_BOOT_TYPE) {
    BdsLibBuildOptionFromVar (BdsBootOptionList, L"BootOrder");
    mEnumBootDevice = TRUE;
    return EFI_SUCCESS;
  }

  InitializeListHead (&mWindowsToGoDeviceList);
  if (WindowsToGoBootVarExist) {
    UpdateWindowsToGoList ();
  }

  SystemConfigStatus = GetKernelConfiguration (&SystemConfiguration);
  ASSERT_EFI_ERROR (SystemConfigStatus);

  OpromStorageDev = NULL;
  OpromStorageDevCount = 0;
  DisableOpromStorageDevBoot = BdsLibGetVariableAndSize (
                                 L"DisableOpromStorageDevBoot",
                                 &gEfiGenericVariableGuid,
                                 &Size
                                 );
  if (DisableOpromStorageDevBoot != NULL) {
    BdsLibGetOpromStorageDevInfo (&OpromStorageDev, &OpromStorageDevCount);
  }

  HwBootDeviceCount = 0;
  BdsLibGetAllHwBootDeviceInfo (&HwBootDeviceCount, &HwBootDeviceInfo);

  //
  // Parse removable media
  //
  gBS->LocateHandleBuffer (
         ByProtocol,
         &gEfiSimpleFileSystemProtocolGuid,
         NULL,
         &NumberFileSystemHandles,
         &FileSystemHandles
         );
  for (Index = 0; Index < NumberFileSystemHandles; Index++) {
    //
    // If need, skip the option ROM storage device.
    //
    if (OpromStorageDevCount != 0) {
      Status = gBS->HandleProtocol (
                      FileSystemHandles[Index],
                      &gEfiDevicePathProtocolGuid,
                      (VOID *) &DevicePath
                      );
      if (!EFI_ERROR (Status) &&
          IsOpromStorageDev (DevicePath, OpromStorageDev, OpromStorageDevCount)) {
        continue;
      }
    }

    //
    // If system doesn't support USB boot, needn't add USB EFI boot option
    //
    if (!EFI_ERROR (SystemConfigStatus) && SystemConfiguration.UsbBoot != 0 &&
        IsUsbDevicePath (DevicePathFromHandle (FileSystemHandles[Index]))) {
      continue;
    }

    Status = gBS->HandleProtocol (
                    FileSystemHandles[Index],
                    &gEfiBlockIoProtocolGuid,
                    (VOID **) &BlkIo
                    );
    if (!EFI_ERROR (Status)) {
      DevicePath     = DevicePathFromHandle (FileSystemHandles[Index]);
      DevicePathType = BdsLibGetBootTypeFromDevicePath (DevicePath);
      //
      // If the file system handle supports a BlkIo protocol, skip the removable media devices
      //
      if (!BlkIo->Media->RemovableMedia &&
          DevicePathType != BDS_EFI_MESSAGE_USB_DEVICE_BOOT &&
          CheckDevicePath (DevicePath, MEDIA_DEVICE_PATH, MEDIA_HARDDRIVE_DP)) {
        if (!IsEfiSystemPartition (FileSystemHandles[Index])) {
          continue;
        }

        //
        // If find UEFI OS option in unremovable device, delete exist recovery boot option.
        // If cannot find UEFI OS option, try to find default EFI boot option as recovery boot option.
        //
        if (IsGenericUefiBootOs (FileSystemHandles[Index])) {
          CreateGenericUefiBootOs (FileSystemHandles[Index], BdsBootOptionList);
          DeleteRecoveryOption (DevicePath);
          continue;
        }
      }
    }

    if (WindowsToGoBootVarExist && IsWindowsToGo (FileSystemHandles[Index])) {
      BdsLibDeleteOptionFromHandle (FileSystemHandles[Index]);
      continue;
    }

    //
    // Do the removable Media thing. \EFI\BOOT\boot{machinename}.EFI
    //  machinename is ia32, ia64, x64, ...
    //
    if (!HaveDefaultRemovableFile (FileSystemHandles[Index])) {
      //
      // No such file or the file is not a EFI application, delete this boot option
      //
      BdsLibDeleteOptionFromHandle (FileSystemHandles[Index]);
    } else {
      DevicePath     = DevicePathFromHandle (FileSystemHandles[Index]);
      DevicePathType = BdsLibGetBootTypeFromDevicePath (DevicePath);

      IsCreated = FALSE;

      //
      // Get hardware device name for current boot option.
      //
      for (HwNum = 0; HwNum < HwBootDeviceCount; HwNum++) {
        if (BdsLibCompareBlockIoDevicePath (HwBootDeviceInfo[HwNum].BlockIoDevicePath, DevicePath)) {
          break;
        }
      }

      switch (DevicePathType) {

      case BDS_EFI_ACPI_FLOPPY_BOOT:
        if (FloppyNumber == 0) {
          UnicodeSPrint (Buffer, sizeof (Buffer), L"%s", BdsLibGetStringById (STRING_TOKEN (STR_DESCRIPTION_FLOPPY)));
        } else {
          UnicodeSPrint (Buffer, sizeof (Buffer), L"%s %d", BdsLibGetStringById (STRING_TOKEN (STR_DESCRIPTION_FLOPPY)), FloppyNumber);
        }
        FloppyNumber++;
        break;

      //
      // Assume a removable SATA device should be the DVD/CD device
      //
      case BDS_EFI_MESSAGE_SATA_BOOT:
      case BDS_EFI_MESSAGE_ATAPI_BOOT:
      case BDS_EFI_MEDIA_CDROM_BOOT:
        if (CdromNumber == 0) {
          UnicodeSPrint (Buffer, sizeof (Buffer), L"%s", BdsLibGetStringById (STRING_TOKEN (STR_DESCRIPTION_CD_DVD)));
        } else {
          UnicodeSPrint (Buffer, sizeof (Buffer), L"%s %d", BdsLibGetStringById (STRING_TOKEN (STR_DESCRIPTION_CD_DVD)), CdromNumber);
        }
        CdromNumber++;
        break;

      case BDS_EFI_MESSAGE_USB_DEVICE_BOOT:
        if (UsbNumber == 0) {
          UnicodeSPrint (Buffer, sizeof (Buffer), L"%s", BdsLibGetStringById (STRING_TOKEN (STR_DESCRIPTION_USB)));
        } else {
          UnicodeSPrint (Buffer, sizeof (Buffer), L"%s %d", BdsLibGetStringById (STRING_TOKEN (STR_DESCRIPTION_USB)), UsbNumber);
        }
        UsbNumber++;
        break;

      case BDS_EFI_MESSAGE_SCSI_BOOT:
        if (ScsiNumber == 0) {
          UnicodeSPrint (Buffer, sizeof (Buffer), L"%s", BdsLibGetStringById (STRING_TOKEN (STR_DESCRIPTION_SCSI)));
        } else {
          UnicodeSPrint (Buffer, sizeof (Buffer), L"%s %d", BdsLibGetStringById (STRING_TOKEN (STR_DESCRIPTION_SCSI)), ScsiNumber);
        }
        ScsiNumber++;
        break;

      case BDS_EFI_MESSAGE_ISCSI_BOOT:
        if (IScsiNumber == 0) {
          UnicodeSPrint (Buffer, sizeof (Buffer), L"%s", BdsLibGetStringById (STRING_TOKEN (STR_DESCRIPTION_ISCSI)));
        } else {
          UnicodeSPrint (Buffer, sizeof (Buffer), L"%s %d", BdsLibGetStringById (STRING_TOKEN (STR_DESCRIPTION_ISCSI)), IScsiNumber);
        }
        IScsiNumber++;
        break;

      case BDS_EFI_MESSAGE_MISC_BOOT:
        if (MiscNumber == 0) {
          UnicodeSPrint (Buffer, sizeof (Buffer), L"%s", BdsLibGetStringById (STRING_TOKEN (STR_DESCRIPTION_MISC)));
        } else {
          UnicodeSPrint (Buffer, sizeof (Buffer), L"%s %d", BdsLibGetStringById (STRING_TOKEN (STR_DESCRIPTION_MISC)), MiscNumber);
        }
        MiscNumber++;
        break;

      case BDS_EFI_MEDIA_HD_BOOT:
        if (HddNumber == 0) {
          UnicodeSPrint (Buffer, sizeof (Buffer), L"%s", BdsLibGetStringById (STRING_TOKEN (STR_DESCRIPTION_HDD)));
        } else {
          UnicodeSPrint (Buffer, sizeof (Buffer), L"%s %d", BdsLibGetStringById (STRING_TOKEN (STR_DESCRIPTION_HDD)), HddNumber);
        }
        HddNumber++;
        break;

      default:
        BdsLibBuildOptionFromHandle (FileSystemHandles[Index], BdsBootOptionList, NULL);
        IsCreated = TRUE;
        break;
      }

      if (!IsCreated) {
        if (HwNum == HwBootDeviceCount) {
          BdsLibBuildOptionFromHandle (FileSystemHandles[Index], BdsBootOptionList, Buffer);
        } else {
          UnicodeSPrint (Buffer, sizeof(Buffer), L"%s (%s)", Buffer, HwBootDeviceInfo[HwNum].HwDeviceName);
          BdsLibBuildOptionFromHandle (FileSystemHandles[Index], BdsBootOptionList, Buffer);
        }
      }
    }
  }

  if (HwBootDeviceCount != 0) {
    for (Index = 0; Index < HwBootDeviceCount; Index++) {
      gBS->FreePool (HwBootDeviceInfo[Index].HwDeviceName);
    }
    gBS->FreePool (HwBootDeviceInfo);
  }

  if (NumberFileSystemHandles) {
    gBS->FreePool (FileSystemHandles);
  }

  if (DisableOpromStorageDevBoot != NULL) {
    gBS->FreePool (DisableOpromStorageDevBoot);

    if (OpromStorageDevCount) {
      gBS->FreePool (OpromStorageDev);
    }
  }

  //
  // Parse Network Boot Device
  //
  if(!EFI_ERROR (SystemConfigStatus) && SystemConfiguration.PxeBootToLan != 0) {
    if (FeaturePcdGet (PcdH2ONetworkSupported)) {
      BuildNetworkBootOption (BdsBootOptionList);
    } else {
      gBS->LocateHandleBuffer (
            ByProtocol,
            &gEfiSimpleNetworkProtocolGuid,
            NULL,
            &NumberLoadFileHandles,
            &LoadFileHandles
            );
      for (Index = 0; Index < NumberLoadFileHandles; Index++) {
        Status = gBS->HandleProtocol (
                        LoadFileHandles[Index],
                        &gEfiLoadFileProtocolGuid,
                        (VOID **) &ProtocolInstance
                        );
        if (EFI_ERROR (Status)) {
          continue;
        }

        if (Index == 0) {
          UnicodeSPrint (Buffer, sizeof (Buffer), L"%s", BdsLibGetStringById (STRING_TOKEN (STR_DESCRIPTION_NETWORK)));
        } else {
          UnicodeSPrint (Buffer, sizeof (Buffer), L"%s %d", BdsLibGetStringById (STRING_TOKEN (STR_DESCRIPTION_NETWORK)), Index);
        }
        BdsLibBuildOptionFromHandle (LoadFileHandles[Index], BdsBootOptionList, Buffer);
      }

      if (NumberLoadFileHandles) {
        gBS->FreePool (LoadFileHandles);
      }
    }
  }

  //
  // Check if we have on flash shell
  //
  gBS->LocateHandleBuffer (
        ByProtocol,
        &gEfiFirmwareVolume2ProtocolGuid,
        NULL,
        &FvHandleCount,
        &FvHandleBuffer
        );
  for (Index = 0; Index < FvHandleCount; Index++) {
    gBS->HandleProtocol (
          FvHandleBuffer[Index],
          &gEfiFirmwareVolume2ProtocolGuid,
          (VOID **) &Fv
          );
    Status = Fv->ReadFile (
                  Fv,
                  PcdGetPtr(PcdShellFile),
                  NULL,
                  &Size,
                  &Type,
                  &Attributes,
                  &AuthenticationStatus
                  );
    if (EFI_ERROR (Status)) {
      //
      // Skip if no shell file in the FV
      //
      continue;
    }
    //
    // Build the shell boot option
    //
    if (InternalShellNumber == 0) {
      UnicodeSPrint (Buffer, sizeof (Buffer), L"Internal EFI Shell");
    } else {
      UnicodeSPrint (Buffer, sizeof (Buffer), L"Internal EFI Shell %d", InternalShellNumber);
    }
    InternalShellNumber++;
    BdsLibBuildOptionFromShell (FvHandleBuffer[Index], BdsBootOptionList, Buffer);
  }

  if (FvHandleCount) {
    gBS->FreePool (FvHandleBuffer);
  }

  //
  // Make sure every boot only have one time boot device enumerate
  //
  BdsLibBuildOptionFromVar (BdsBootOptionList, L"BootOrder");
  if (SystemConfiguration.Win8FastBoot == 0 &&
      BdsLibGetBootType () == EFI_BOOT_TYPE) {
    UpdateTargetHddVariable ();
  }
  if (FeaturePcdGet (PcdAutoCreateDummyBootOption)) {
    BdsLibSyncPhysicalBootOrder ();
  }
  mEnumBootDevice = TRUE;

  return EFI_SUCCESS;
}

/**
  Build the boot option with the handle parsed in

  @param  Handle                 The handle which present the device path to create
                                 boot option
  @param  BdsBootOptionList      The header of the link list which indexed all
                                 current boot options
  @param  String                 The description of the boot option.

**/
VOID
EFIAPI
BdsLibBuildOptionFromHandle (
  IN  EFI_HANDLE                 Handle,
  IN  LIST_ENTRY                 *BdsBootOptionList,
  IN  CHAR16                     *String
  )
{
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  CHAR16                    *TempString;

  DevicePath = DevicePathFromHandle (Handle);
  if (String == NULL) {
    TempString  = DevicePathToStr (DevicePath);
  } else {
    TempString = String;
  }

  //
  // Create and register new boot option
  //
  BdsLibRegisterNewOption (BdsBootOptionList, DevicePath, TempString, L"BootOrder", (UINT8 *) "RC", 2);
}


/**
  Build the on flash shell boot option with the handle parsed in.

  @param  Handle                 The handle which present the device path to create
                                 on flash shell boot option
  @param  BdsBootOptionList      The header of the link list which indexed all
                                 current boot options

**/
VOID
EFIAPI
BdsLibBuildOptionFromShell (
  IN EFI_HANDLE                  Handle,
  IN OUT LIST_ENTRY              *BdsBootOptionList,
  IN CHAR16                      *Description
  )
{
  EFI_DEVICE_PATH_PROTOCOL          *DevicePath;
  MEDIA_FW_VOL_FILEPATH_DEVICE_PATH ShellNode;
  EFI_STATUS                        Status;
  UINT32                            HashValue;
  UINT8                             OptionalData[SHELL_OPTIONAL_DATA_SIZE];


  DevicePath = DevicePathFromHandle (Handle);

  //
  // Build the shell device path
  //
  EfiInitializeFwVolDevicepathNode (&ShellNode, PcdGetPtr(PcdShellFile));
  DevicePath = AppendDevicePathNode (DevicePath, (EFI_DEVICE_PATH_PROTOCOL *) &ShellNode);

  if (IsAlreadyInBootOrder (DevicePath)) {
    gBS->FreePool (DevicePath);
    return;
  }

  Status = GetShellFileHashValue (Handle, &HashValue);
  if (EFI_ERROR(Status)) {
    BdsLibRegisterNewOption (BdsBootOptionList, DevicePath, Description, L"BootOrder", (UINT8 *) "RC", 2);
    gBS->FreePool (DevicePath);
    return;
  }

  gBS->CopyMem (OptionalData    , "RC"                , 2);
  gBS->CopyMem (&OptionalData[2], (UINT8 *) &HashValue, 4);

  //
  // Create and register the shell boot option
  //
  BdsLibRegisterNewOption (BdsBootOptionList, DevicePath, Description, L"BootOrder", OptionalData, SHELL_OPTIONAL_DATA_SIZE);

  gBS->FreePool (DevicePath);
}

/**
  Boot from the UEFI spec defined "BootNext" variable.

**/
EFI_STATUS
EFIAPI
BdsLibBootNext (
  VOID
  )
{
  UINT16            *BootNext;
  UINTN             BootNextSize;
  CHAR16            Buffer[20];
  BDS_COMMON_OPTION *BootOption;
  LIST_ENTRY        TempList;
  UINTN             ExitDataSize;
  CHAR16            *ExitData;
  EFI_STATUS        Status;

  //
  // Init the boot option name buffer and temp link list
  //
  InitializeListHead (&TempList);
  ZeroMem (Buffer, sizeof (Buffer));

  BootNext = BdsLibGetVariableAndSize (
              L"BootNext",
              &gEfiGlobalVariableGuid,
              &BootNextSize
              );

  //
  // Clear the boot next variable first
  //
  Status = EFI_NOT_FOUND;
  if (BootNext != NULL) {
    gRT->SetVariable (
          L"BootNext",
          &gEfiGlobalVariableGuid,
          EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
          0,
          BootNext
          );

    //
    // Start to build the boot option and try to boot
    //
    UnicodeSPrint (Buffer, sizeof (Buffer), L"Boot%04x", *BootNext);
    gBS->FreePool (BootNext);

    BootOption = BdsLibVariableToOption (&TempList, Buffer);
    ASSERT (BootOption != NULL);
    if (BootOption == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    BdsLibConnectDevicePath (BootOption->DevicePath);
    Status = BdsLibBootViaBootOption (BootOption, BootOption->DevicePath, &ExitDataSize, &ExitData);
  }

  return Status;
}

EFI_STATUS
SetBbsPriority (
  IN  EFI_LEGACY_BIOS_PROTOCOL          *LegacyBios,
  IN  BDS_COMMON_OPTION                 *Option
  )
{
  BBS_TABLE                     *LocalBbsTable;
  UINT16                        BbsIndex;
  UINT16                        BootOption[10];
  UINTN                         BootOptionSize;
  UINT8                         *Ptr, *BootOptionVar;
  UINT16                        *OptionOrder;
  UINTN                         OptionOrderSize;
  UINT16                        PriorityIndex;
  BOOLEAN                       Flag;  // TRUE for Option->BootCurrent is the highest priority
  UINT16                        DevPathSize;
  CHAR16                        *BootDesc;

  //
  // Read the BootOrder variable.
  //
  OptionOrder = BdsLibGetVariableAndSize (
                  L"BootOrder",
                  &gEfiGlobalVariableGuid,
                  &OptionOrderSize
                  );
  if (OptionOrder == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Flag = FALSE;

  //
  // Set BBS priority according OptionOrder variable
  //
  for (PriorityIndex = 0; PriorityIndex < OptionOrderSize / sizeof (UINT16); PriorityIndex++) {
    UnicodeSPrint (BootOption, sizeof (BootOption), L"Boot%04x", OptionOrder[PriorityIndex]);

    BootOptionVar = BdsLibGetVariableAndSize (
                      BootOption,
                      &gEfiGlobalVariableGuid,
                      &BootOptionSize
                      );
    if (BootOptionVar == NULL) {
      continue;
    }
    //
    // Skip the native boot options(EFI shell...)
    //
    Ptr = BootOptionVar + sizeof(UINT32) + sizeof(UINT16) + StrSize ((CHAR16 *)(BootOptionVar + 6));
    if (*Ptr != BBS_DEVICE_PATH) {
      continue;
    }

    Ptr = BootOptionVar;
    Ptr += sizeof (UINT32);

    DevPathSize = *((UINT16 *) Ptr);
    Ptr += sizeof (UINT16);

    BootDesc = (CHAR16*) Ptr;
    Ptr += StrSize (BootDesc);

    Ptr += DevPathSize;
    Ptr += sizeof (BBS_TABLE);

    BbsIndex = *(UINT16 *)Ptr;

    LegacyBios->GetBbsInfo (LegacyBios, NULL, NULL, NULL, &LocalBbsTable);

    LocalBbsTable[BbsIndex].BootPriority = (UINT16)PriorityIndex;

    //
    // Pull Option->BootCurrent up to the highest priority
    //
    if (!Flag) {
      if (Option->BootCurrent == OptionOrder[PriorityIndex]) {
        LocalBbsTable[BbsIndex].BootPriority = 0;
        Flag = TRUE;
      } else {
        LocalBbsTable[BbsIndex].BootPriority++;
      }
    }
  }

  return EFI_SUCCESS;
}


/**
  Return the bootable media handle.
  First, check the device is connected
  Second, check whether the device path point to a device which support SimpleFileSystemProtocol,
  Third, detect the the default boot file in the Media, and return the removable Media handle.

  @param  DevicePath  Device Path to a  bootable device

  @return  The bootable media handle. If the media on the DevicePath is not bootable, NULL will return.

**/
EFI_HANDLE
EFIAPI
BdsLibGetBootableHandle (
  IN  EFI_DEVICE_PATH_PROTOCOL      *DevicePath
  )
{
  EFI_STATUS                      Status;
  EFI_TPL                         OldTpl;
  EFI_DEVICE_PATH_PROTOCOL        *UpdatedDevicePath;
  EFI_DEVICE_PATH_PROTOCOL        *DupDevicePath;
  EFI_HANDLE                      Handle;
  EFI_BLOCK_IO_PROTOCOL           *BlockIo;
  VOID                            *Buffer;
  EFI_DEVICE_PATH_PROTOCOL        *TempDevicePath;
  UINTN                           Size;
  UINTN                           TempSize;
  EFI_HANDLE                      ReturnHandle;
  EFI_HANDLE                      *SimpleFileSystemHandles;

  UINTN                           NumberSimpleFileSystemHandles;
  UINTN                           Index;
  EFI_IMAGE_DOS_HEADER            DosHeader;
  EFI_IMAGE_OPTIONAL_HEADER_UNION       HdrData;
  EFI_IMAGE_OPTIONAL_HEADER_PTR_UNION   Hdr;

  UpdatedDevicePath = DevicePath;

  //
  // Enter to critical section to protect the acquired BlockIo instance
  // from getting released due to the USB mass storage hotplug event
  //
  OldTpl = gBS->RaiseTPL (TPL_CALLBACK);

  //
  // Check whether the device is connected
  //
  Status = gBS->LocateDevicePath (&gEfiBlockIoProtocolGuid, &UpdatedDevicePath, &Handle);
  if (EFI_ERROR (Status)) {
    //
    // Skip the case that the boot option point to a simple file protocol which does not consume block Io protocol,
    //
    Status = gBS->LocateDevicePath (&gEfiSimpleFileSystemProtocolGuid, &UpdatedDevicePath, &Handle);
    if (EFI_ERROR (Status)) {
      //
      // Fail to find the proper BlockIo and simple file protocol, maybe because device not present,  we need to connect it firstly
      //
      UpdatedDevicePath = DevicePath;
      Status            = gBS->LocateDevicePath (&gEfiDevicePathProtocolGuid, &UpdatedDevicePath, &Handle);
      gBS->ConnectController (Handle, NULL, NULL, TRUE);
    }
  } else {
    //
    // For removable device boot option, its contained device path only point to the removable device handle,
    // should make sure all its children handles (its child partion or media handles) are created and connected.
    //
    gBS->ConnectController (Handle, NULL, NULL, TRUE);
    //
    // Get BlockIo protocol and check removable attribute
    //
    Status = gBS->HandleProtocol (Handle, &gEfiBlockIoProtocolGuid, (VOID **)&BlockIo);
    ASSERT_EFI_ERROR (Status);

    //
    // Issue a dummy read to the device to check for media change.
    // When the removable media is changed, any Block IO read/write will
    // cause the BlockIo protocol be reinstalled and EFI_MEDIA_CHANGED is
    // returned. After the Block IO protocol is reinstalled, subsequent
    // Block IO read/write will success.
    //
    Buffer = AllocatePool (BlockIo->Media->BlockSize);
    if (Buffer != NULL) {
      BlockIo->ReadBlocks (
               BlockIo,
               BlockIo->Media->MediaId,
               0,
               BlockIo->Media->BlockSize,
               Buffer
               );
      FreePool(Buffer);
    }
  }

  //
  // Detect the the default boot file from removable Media
  //

  //
  // If fail to get bootable handle specified by a USB boot option, the BDS should try to find other bootable device in the same USB bus
  // Try to locate the USB node device path first, if fail then use its previous PCI node to search
  //
  DupDevicePath = DuplicateDevicePath (DevicePath);
  ASSERT (DupDevicePath != NULL);

  UpdatedDevicePath = DupDevicePath;
  Status = gBS->LocateDevicePath (&gEfiDevicePathProtocolGuid, &UpdatedDevicePath, &Handle);
  //
  // if the resulting device path point to a usb node, and the usb node is a dummy node, should only let device path only point to the previous Pci node
  // Acpi()/Pci()/Usb() --> Acpi()/Pci()
  //
  if ((DevicePathType (UpdatedDevicePath) == MESSAGING_DEVICE_PATH) &&
      (DevicePathSubType (UpdatedDevicePath) == MSG_USB_DP)) {
    //
    // Remove the usb node, let the device path only point to PCI node
    //
    SetDevicePathEndNode (UpdatedDevicePath);
    UpdatedDevicePath = DupDevicePath;
  } else {
    UpdatedDevicePath = DevicePath;
  }

  //
  // Get the device path size of boot option
  //
  Size = GetDevicePathSize(UpdatedDevicePath) - sizeof (EFI_DEVICE_PATH_PROTOCOL); // minus the end node
  ReturnHandle = NULL;
  gBS->LocateHandleBuffer (
      ByProtocol,
      &gEfiSimpleFileSystemProtocolGuid,
      NULL,
      &NumberSimpleFileSystemHandles,
      &SimpleFileSystemHandles
      );
  for (Index = 0; Index < NumberSimpleFileSystemHandles; Index++) {
    //
    // Get the device path size of SimpleFileSystem handle
    //
    TempDevicePath = DevicePathFromHandle (SimpleFileSystemHandles[Index]);
    TempSize = GetDevicePathSize (TempDevicePath)- sizeof (EFI_DEVICE_PATH_PROTOCOL); // minus the end node
    //
    // Check whether the device path of boot option is part of the  SimpleFileSystem handle's device path
    //
    if (Size <= TempSize && CompareMem (TempDevicePath, UpdatedDevicePath, Size)==0) {
      //
      // Load the default boot file \EFI\BOOT\boot{machinename}.EFI from removable Media
      //  machinename is ia32, ia64, x64, ...
      //
      Hdr.Union = &HdrData;
      Status = BdsLibGetImageHeader (
                 SimpleFileSystemHandles[Index],
                 EFI_REMOVABLE_MEDIA_FILE_NAME,
                 &DosHeader,
                 Hdr
                 );
      if (!EFI_ERROR (Status) &&
        EFI_IMAGE_MACHINE_TYPE_SUPPORTED (Hdr.Pe32->FileHeader.Machine) &&
        Hdr.Pe32->OptionalHeader.Subsystem == EFI_IMAGE_SUBSYSTEM_EFI_APPLICATION) {
        ReturnHandle = SimpleFileSystemHandles[Index];
        break;
      }
    }
  }

  FreePool(DupDevicePath);

  if (SimpleFileSystemHandles != NULL) {
    FreePool(SimpleFileSystemHandles);
  }

  gBS->RestoreTPL (OldTpl);

  return ReturnHandle;
}

/**
  Check to see if the network cable is plugged in. If the DevicePath is not
  connected it will be connected.

  @param  DevicePath             Device Path to check

  @retval TRUE                   DevicePath points to an Network that is connected
  @retval FALSE                  DevicePath does not point to a bootable network

**/
BOOLEAN
BdsLibNetworkBootWithMediaPresent (
  IN  EFI_DEVICE_PATH_PROTOCOL      *DevicePath
  )
{
  EFI_STATUS                      Status;
  EFI_DEVICE_PATH_PROTOCOL        *UpdatedDevicePath;
  EFI_HANDLE                      Handle;
  EFI_SIMPLE_NETWORK_PROTOCOL     *Snp;
  BOOLEAN                         MediaPresent;
  UINT32                          InterruptStatus;

  MediaPresent = FALSE;

  UpdatedDevicePath = DevicePath;
  //
  // Locate Load File Protocol for PXE boot option first
  //
  Status = gBS->LocateDevicePath (&gEfiLoadFileProtocolGuid, &UpdatedDevicePath, &Handle);
  if (EFI_ERROR (Status)) {
    //
    // Device not present so see if we need to connect it
    //
    Status = BdsLibConnectDevicePath (DevicePath);
    if (!EFI_ERROR (Status)) {
      //
      // This one should work after we did the connect
      //
      Status = gBS->LocateDevicePath (&gEfiLoadFileProtocolGuid, &UpdatedDevicePath, &Handle);
    }
  }

  if (!EFI_ERROR (Status)) {
    Status = gBS->HandleProtocol (Handle, &gEfiSimpleNetworkProtocolGuid, (VOID **)&Snp);
    if (EFI_ERROR (Status)) {
      //
      // Failed to open SNP from this handle, try to get SNP from parent handle
      //
      UpdatedDevicePath = DevicePathFromHandle (Handle);
      if (UpdatedDevicePath != NULL) {
        Status = gBS->LocateDevicePath (&gEfiSimpleNetworkProtocolGuid, &UpdatedDevicePath, &Handle);
        if (!EFI_ERROR (Status)) {
          //
          // SNP handle found, get SNP from it
          //
          Status = gBS->HandleProtocol (Handle, &gEfiSimpleNetworkProtocolGuid, (VOID **) &Snp);
        }
      }
    }

    if (!EFI_ERROR (Status)) {
      if (Snp->Mode->MediaPresentSupported) {
        if (Snp->Mode->State == EfiSimpleNetworkInitialized) {
          //
          // Invoke Snp->GetStatus() to refresh the media status
          //
          Snp->GetStatus (Snp, &InterruptStatus, NULL);

          //
          // In case some one else is using the SNP check to see if it's connected
          //
          MediaPresent = Snp->Mode->MediaPresent;
        } else {
          //
          // No one is using SNP so we need to Start and Initialize so
          // MediaPresent will be valid.
          //
          Status = Snp->Start (Snp);
          if (!EFI_ERROR (Status)) {
            Status = Snp->Initialize (Snp, 0, 0);
            if (!EFI_ERROR (Status)) {
              MediaPresent = Snp->Mode->MediaPresent;
              Snp->Shutdown (Snp);
            }
            Snp->Stop (Snp);
          }
        }
      } else {
        MediaPresent = TRUE;
      }
    }
  }

  return MediaPresent;
}


/**
  Check whether the Device path in a boot option point to a valid bootable device,
  And if CheckMedia is true, check the device is ready to boot now.

  @param  DevPath     the Device path in a boot option
  @param  CheckMedia  if true, check the device is ready to boot now.

  @retval TRUE        the Device path  is valid
  @retval FALSE       the Device path  is invalid .

**/
BOOLEAN
EFIAPI
BdsLibIsValidEFIBootOptDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL     *DevPath,
  IN BOOLEAN                      CheckMedia
  )
{
  return BdsLibIsValidEFIBootOptDevicePathExt (DevPath, CheckMedia, NULL);
}

/**
  Check whether the Device path in a boot option point to a valid bootable device,
  And if CheckMedia is true, check the device is ready to boot now.
  If Description is not NULL and the device path point to a fixed BlockIo
  device, check the description whether conflict with other auto-created
  boot options.

  @param  DevPath     the Device path in a boot option
  @param  CheckMedia  if true, check the device is ready to boot now.
  @param  Description the description in a boot option

  @retval TRUE        the Device path  is valid
  @retval FALSE       the Device path  is invalid .

**/
BOOLEAN
EFIAPI
BdsLibIsValidEFIBootOptDevicePathExt (
  IN EFI_DEVICE_PATH_PROTOCOL     *DevPath,
  IN BOOLEAN                      CheckMedia,
  IN CHAR16                       *Description
  )
{
  EFI_STATUS                Status;
  EFI_HANDLE                Handle;
  EFI_DEVICE_PATH_PROTOCOL  *TempDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *LastDeviceNode;
  EFI_BLOCK_IO_PROTOCOL     *BlockIo;

  TempDevicePath = DevPath;
  LastDeviceNode = DevPath;

  //
  // Check if it's a valid boot option for network boot device.
  // Check if there is EfiLoadFileProtocol installed.
  // If yes, that means there is a boot option for network.
  //
  Status = gBS->LocateDevicePath (
                  &gEfiLoadFileProtocolGuid,
                  &TempDevicePath,
                  &Handle
                  );
  if (EFI_ERROR (Status)) {
    //
    // Device not present so see if we need to connect it
    //
    TempDevicePath = DevPath;
    BdsLibConnectDevicePath (TempDevicePath);
    Status = gBS->LocateDevicePath (
                    &gEfiLoadFileProtocolGuid,
                    &TempDevicePath,
                    &Handle
                    );
  }

  if (!EFI_ERROR (Status)) {
    if (!IsDevicePathEnd (TempDevicePath)) {
      //
      // LoadFile protocol is not installed on handle with exactly the same DevPath
      //
      return FALSE;
    }

    if (CheckMedia) {
      //
      // Test if it is ready to boot now
      //
      if (BdsLibNetworkBootWithMediaPresent(DevPath)) {
        return TRUE;
      }
    } else {
      return TRUE;
    }
  }

  //
  // If the boot option point to a file, it is a valid EFI boot option,
  // and assume it is ready to boot now
  //
  while (!IsDevicePathEnd (TempDevicePath)) {
    //
    // If there is USB Class or USB WWID device path node, treat it as valid EFI
    // Boot Option. BdsExpandUsbShortFormDevicePath () will be used to expand it
    // to full device path.
    //
    if ((DevicePathType (TempDevicePath) == MESSAGING_DEVICE_PATH) &&
        ((DevicePathSubType (TempDevicePath) == MSG_USB_CLASS_DP) ||
         (DevicePathSubType (TempDevicePath) == MSG_USB_WWID_DP))) {
      return TRUE;
    }

    LastDeviceNode = TempDevicePath;
    TempDevicePath = NextDevicePathNode (TempDevicePath);
  }
  if ((DevicePathType (LastDeviceNode) == MEDIA_DEVICE_PATH) &&
    (DevicePathSubType (LastDeviceNode) == MEDIA_FILEPATH_DP)) {
    return TRUE;
  }

  //
  // Check if it's a valid boot option for internal FV application
  //
  if (EfiGetNameGuidFromFwVolDevicePathNode ((MEDIA_FW_VOL_FILEPATH_DEVICE_PATH *) LastDeviceNode) != NULL) {
    //
    // If the boot option point to internal FV application, make sure it is valid
    //
    TempDevicePath = DevPath;
    Status = BdsLibUpdateFvFileDevicePath (
               &TempDevicePath,
               EfiGetNameGuidFromFwVolDevicePathNode ((MEDIA_FW_VOL_FILEPATH_DEVICE_PATH *) LastDeviceNode)
               );
    if (Status == EFI_ALREADY_STARTED) {
      return TRUE;
    } else {
      if (Status == EFI_SUCCESS) {
        FreePool (TempDevicePath);
      }
      return FALSE;
    }
  }

  //
  // If the boot option point to a blockIO device:
  //    if it is a removable blockIo device, it is valid.
  //    if it is a fixed blockIo device, check its description confliction.
  //
  TempDevicePath = DevPath;
  Status = gBS->LocateDevicePath (&gEfiBlockIoProtocolGuid, &TempDevicePath, &Handle);
  if (EFI_ERROR (Status)) {
    //
    // Device not present so see if we need to connect it
    //
    Status = BdsLibConnectDevicePath (DevPath);
    if (!EFI_ERROR (Status)) {
      //
      // Try again to get the Block Io protocol after we did the connect
      //
      TempDevicePath = DevPath;
      Status = gBS->LocateDevicePath (&gEfiBlockIoProtocolGuid, &TempDevicePath, &Handle);
    }
  }

  if (!EFI_ERROR (Status)) {
    Status = gBS->HandleProtocol (Handle, &gEfiBlockIoProtocolGuid, (VOID **)&BlockIo);
    if (!EFI_ERROR (Status)) {
      if (CheckMedia) {
        //
        // Test if it is ready to boot now
        //
        if (BdsLibGetBootableHandle (DevPath) != NULL) {
          return TRUE;
        }
      } else {
        return TRUE;
      }
    }
  } else {
    //
    // if the boot option point to a simple file protocol which does not consume block Io protocol, it is also a valid EFI boot option,
    //
    Status = gBS->LocateDevicePath (&gEfiSimpleFileSystemProtocolGuid, &TempDevicePath, &Handle);
    if (!EFI_ERROR (Status)) {
      if (CheckMedia) {
        //
        // Test if it is ready to boot now
        //
        if (BdsLibGetBootableHandle (DevPath) != NULL) {
          return TRUE;
        }
      } else {
        return TRUE;
      }
    }
  }
  return FALSE;
}


/**
  According to a file guild, check a Fv file device path is valid. If it is invalid,
  try to return the valid device path.
  FV address maybe changes for memory layout adjust from time to time, use this function
  could promise the Fv file device path is right.

  @param  DevicePath             on input, the Fv file device path need to check on
                                 output, the updated valid Fv file device path
  @param  FileGuid               the Fv file guild

  @retval EFI_INVALID_PARAMETER  the input DevicePath or FileGuid is invalid
                                 parameter
  @retval EFI_UNSUPPORTED        the input DevicePath does not contain Fv file
                                 guild at all
  @retval EFI_ALREADY_STARTED    the input DevicePath has pointed to Fv file, it is
                                 valid
  @retval EFI_SUCCESS            has successfully updated the invalid DevicePath,
                                 and return the updated device path in DevicePath

**/
EFI_STATUS
EFIAPI
BdsLibUpdateFvFileDevicePath (
  IN  OUT EFI_DEVICE_PATH_PROTOCOL      ** DevicePath,
  IN  EFI_GUID                          *FileGuid
  )
{
  EFI_DEVICE_PATH_PROTOCOL      *TempDevicePath;
  EFI_DEVICE_PATH_PROTOCOL      *LastDeviceNode;
  EFI_STATUS                    Status;
  EFI_GUID                      *GuidPoint;
  UINTN                         Index;
  UINTN                         FvHandleCount;
  EFI_HANDLE                    *FvHandleBuffer;
  EFI_FV_FILETYPE               Type;
  UINTN                         Size;
  EFI_FV_FILE_ATTRIBUTES        Attributes;
  UINT32                        AuthenticationStatus;
  BOOLEAN                       FindFvFile;
  EFI_LOADED_IMAGE_PROTOCOL     *LoadedImage;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *Fv;
  MEDIA_FW_VOL_FILEPATH_DEVICE_PATH FvFileNode;
  EFI_HANDLE                    FoundFvHandle;
  EFI_DEVICE_PATH_PROTOCOL      *NewDevicePath;

  if ((DevicePath == NULL) || (*DevicePath == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  if (FileGuid == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check whether the device path point to the default the input Fv file
  //
  TempDevicePath = *DevicePath;
  LastDeviceNode = TempDevicePath;
  while (!IsDevicePathEnd (TempDevicePath)) {
     LastDeviceNode = TempDevicePath;
     TempDevicePath = NextDevicePathNode (TempDevicePath);
  }
  GuidPoint = EfiGetNameGuidFromFwVolDevicePathNode (
                (MEDIA_FW_VOL_FILEPATH_DEVICE_PATH *) LastDeviceNode
                );
  if (GuidPoint == NULL) {
    //
    // if this option does not points to a Fv file, just return EFI_UNSUPPORTED
    //
    return EFI_UNSUPPORTED;
  }
  if (!CompareGuid (GuidPoint, FileGuid)) {
    //
    // If the Fv file is not the input file guid, just return EFI_UNSUPPORTED
    //
    return EFI_UNSUPPORTED;
  }

  //
  // Check whether the input Fv file device path is valid
  //
  TempDevicePath = *DevicePath;
  FoundFvHandle = NULL;
  Status = gBS->LocateDevicePath (
                  &gEfiFirmwareVolume2ProtocolGuid,
                  &TempDevicePath,
                  &FoundFvHandle
                  );
  if (!EFI_ERROR (Status)) {
    Status = gBS->HandleProtocol (
                    FoundFvHandle,
                    &gEfiFirmwareVolume2ProtocolGuid,
                    (VOID **) &Fv
                    );
    if (!EFI_ERROR (Status)) {
      //
      // Set FV ReadFile Buffer as NULL, only need to check whether input Fv file exist there
      //
      Status = Fv->ReadFile (
                    Fv,
                    FileGuid,
                    NULL,
                    &Size,
                    &Type,
                    &Attributes,
                    &AuthenticationStatus
                    );
      if (!EFI_ERROR (Status)) {
        return EFI_ALREADY_STARTED;
      }
    }
  }

  //
  // Look for the input wanted FV file in current FV
  // First, try to look for in Bds own FV. Bds and input wanted FV file usually are in the same FV
  //
  FindFvFile = FALSE;
  FoundFvHandle = NULL;
  Status = gBS->HandleProtocol (
             gImageHandle,
             &gEfiLoadedImageProtocolGuid,
             (VOID **) &LoadedImage
             );
  if (!EFI_ERROR (Status)) {
    Status = gBS->HandleProtocol (
                    LoadedImage->DeviceHandle,
                    &gEfiFirmwareVolume2ProtocolGuid,
                    (VOID **) &Fv
                    );
    if (!EFI_ERROR (Status)) {
      Status = Fv->ReadFile (
                    Fv,
                    FileGuid,
                    NULL,
                    &Size,
                    &Type,
                    &Attributes,
                    &AuthenticationStatus
                    );
      if (!EFI_ERROR (Status)) {
        FindFvFile = TRUE;
        FoundFvHandle = LoadedImage->DeviceHandle;
      }
    }
  }
  //
  // Second, if fail to find, try to enumerate all FV
  //
  if (!FindFvFile) {
    FvHandleBuffer = NULL;
    gBS->LocateHandleBuffer (
          ByProtocol,
          &gEfiFirmwareVolume2ProtocolGuid,
          NULL,
          &FvHandleCount,
          &FvHandleBuffer
          );
    for (Index = 0; Index < FvHandleCount; Index++) {
      gBS->HandleProtocol (
            FvHandleBuffer[Index],
            &gEfiFirmwareVolume2ProtocolGuid,
            (VOID **) &Fv
            );

      Status = Fv->ReadFile (
                    Fv,
                    FileGuid,
                    NULL,
                    &Size,
                    &Type,
                    &Attributes,
                    &AuthenticationStatus
                    );
      if (EFI_ERROR (Status)) {
        //
        // Skip if input Fv file not in the FV
        //
        continue;
      }
      FindFvFile = TRUE;
      FoundFvHandle = FvHandleBuffer[Index];
      break;
    }

    if (FvHandleBuffer != NULL) {
      FreePool (FvHandleBuffer);
    }
  }

  if (FindFvFile) {
    //
    // Build the shell device path
    //
    NewDevicePath = DevicePathFromHandle (FoundFvHandle);
    EfiInitializeFwVolDevicepathNode (&FvFileNode, FileGuid);
    NewDevicePath = AppendDevicePathNode (NewDevicePath, (EFI_DEVICE_PATH_PROTOCOL *) &FvFileNode);
    *DevicePath = NewDevicePath;
    return EFI_SUCCESS;
  } else {
    return EFI_NOT_FOUND;
  }
}



/**
  According Input value to determine remove legacy boot device or EFI
  boot device.

  @param  RemovedLegacy     TRUE: Remove all of legacy boot devices
                            FALSE: Remove all of EFI boot devices

  @retval EFI_SUCCESS            Remove boot devices successful.
  @retval EFI_NOT_FOUND          Cannot find any boot device.
  @retval EFI_OUT_OF_RESOURCES   Boot Order and Boot option does't sychronization

**/
EFI_STATUS
BdsLibRemovedBootOption (
  IN  BOOLEAN                           RemovedLegacy
  )
{
  UINT16                    *BootOrder;
  UINT8                     *BootOptionVar;
  UINTN                     BootOrderSize;
  UINTN                     BootOptionSize;
  EFI_STATUS                Status;
  UINTN                     Index;
  UINTN                     Index2;
  UINT16                    BootOption[BOOT_OPTION_MAX_CHAR];

  BOOLEAN                   IsLegacyBootOption;
  BBS_TABLE                 *BbsEntry;
  UINT16                    BbsIndex;

  Status        = EFI_SUCCESS;
  BootOrder     = NULL;
  BootOrderSize = 0;

  //
  // delete physical and virtual boot order variable if system is in legacy mode.
  //
  if (FeaturePcdGet (PcdAutoCreateDummyBootOption) && !RemovedLegacy) {
    Status = gRT->SetVariable (
                    PHYSICAL_BOOT_ORDER_NAME,
                    &gEfiGenericVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    0,
                    NULL
                    );
  }

  BootOrder = BdsLibGetVariableAndSize (
                L"BootOrder",
                &gEfiGlobalVariableGuid,
                &BootOrderSize
                );
  if (NULL == BootOrder) {
    return EFI_NOT_FOUND;
  }

  Index = 0;
  while (Index < BootOrderSize / sizeof (UINT16)) {
    UnicodeSPrint (BootOption, sizeof (BootOption), L"Boot%04x", BootOrder[Index]);
    BootOptionVar = BdsLibGetVariableAndSize (
                      BootOption,
                      &gEfiGlobalVariableGuid,
                      &BootOptionSize
                      );
    if (NULL == BootOptionVar) {
      gBS->FreePool (BootOrder);
      return EFI_OUT_OF_RESOURCES;
    }

    IsLegacyBootOption = BdsLibIsLegacyBootOption (BootOptionVar, &BbsEntry, &BbsIndex);

    if (RemovedLegacy && !IsLegacyBootOption) {
      gBS->FreePool (BootOptionVar);
      Index++;
      continue;
    }

    if (!RemovedLegacy && IsLegacyBootOption) {
      gBS->FreePool (BootOptionVar);
      Index++;
      continue;
    }
    //
    // Delete this invalid boot option "Boot####"
    //
    Status = gRT->SetVariable (
                    BootOption,
                    &gEfiGlobalVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    0,
                    NULL
                    );
    //
    // Mark this boot option in boot order as deleted
    //
    BootOrder[Index] = 0xffff;

    gBS->FreePool (BootOptionVar);
    Index++;
  }

  //
  // Adjust boot order array
  //
  Index2 = 0;
  for (Index = 0; Index < BootOrderSize / sizeof (UINT16); Index++) {
    if (BootOrder[Index] != 0xffff) {
      BootOrder[Index2] = BootOrder[Index];
      Index2 ++;
    }
  }
  Status = gRT->SetVariable (
                  L"BootOrder",
                  &gEfiGlobalVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  Index2 * sizeof (UINT16),
                  BootOrder
                  );

  gBS->FreePool (BootOrder);

  return Status;
}


/**
  For a bootable Device path, return its boot type

  @param  DevicePath    The bootable device Path to check

  @return UINT32 Boot type :
    //
    // If the device path contains any media deviec path node, it is media boot type
    // For the floppy node, handle it as media node
    //
    BDS_EFI_MEDIA_HD_BOOT
    BDS_EFI_MEDIA_CDROM_BOOT
    BDS_EFI_ACPI_FLOPPY_BOOT
    //
    // If the device path not contains any media deviec path node,  and
    // its last device path node point to a message device path node, it is
    // a message boot type
    //
    BDS_EFI_MESSAGE_ATAPI_BOOT
    BDS_EFI_MESSAGE_SCSI_BOOT
    BDS_EFI_MESSAGE_USB_DEVICE_BOOT
    BDS_EFI_MESSAGE_MISC_BOOT
    //
    // Legacy boot type
    //
    BDS_LEGACY_BBS_BOOT
    //
    // If a EFI Removable BlockIO device path not point to a media and message devie,
    // it is unsupported
    //
    BDS_EFI_UNSUPPORT

**/
UINT32
BdsLibGetBootTypeFromDevicePath (
  IN  EFI_DEVICE_PATH_PROTOCOL     *DevicePath
  )
{
  ACPI_HID_DEVICE_PATH          *Acpi;
  EFI_DEVICE_PATH_PROTOCOL      *TempDevicePath;
  EFI_DEVICE_PATH_PROTOCOL      *LastDeviceNode;
  UINT32                        BootType;

  if (NULL == DevicePath) {
    return BDS_EFI_UNSUPPORT;
  }

  TempDevicePath = DevicePath;

  while (!IsDevicePathEndType (TempDevicePath)) {
    switch (DevicePathType (TempDevicePath)) {

    case BBS_DEVICE_PATH:
       return BDS_LEGACY_BBS_BOOT;

    case MEDIA_DEVICE_PATH:
      if (DevicePathSubType (TempDevicePath) == MEDIA_HARDDRIVE_DP) {
        return BDS_EFI_MEDIA_HD_BOOT;
      } else if (DevicePathSubType (TempDevicePath) == MEDIA_CDROM_DP) {
        return BDS_EFI_MEDIA_CDROM_BOOT;
      } else if (DevicePathSubType (TempDevicePath) == MEDIA_PIWG_FW_FILE_DP) {
        return BDS_EFI_MEDIA_FV_FILEPATH_BOOT;
      }
      break;

    case ACPI_DEVICE_PATH:
      Acpi = (ACPI_HID_DEVICE_PATH *) TempDevicePath;
      if (EISA_ID_TO_NUM (Acpi->HID) == 0x0604) {
        return BDS_EFI_ACPI_FLOPPY_BOOT;
      }
      break;

    case MESSAGING_DEVICE_PATH:

      //
      // check message device path is USB device first.
      //
      if (DevicePathSubType(TempDevicePath) == MSG_USB_DP) {
        return BDS_EFI_MESSAGE_USB_DEVICE_BOOT;
      }

      //
      // check message device path is ISCSI device.
      //
      if (DevicePathSubType(TempDevicePath) == MSG_ISCSI_DP) {
        return BDS_EFI_MESSAGE_ISCSI_BOOT;
      }

      //
      // Get the last device path node
      //
      LastDeviceNode = NextDevicePathNode (TempDevicePath);

      if (DevicePathSubType(LastDeviceNode) == MSG_DEVICE_LOGICAL_UNIT_DP) {
        //
        // if the next node type is Device Logical Unit, which specify the Logical Unit Number (LUN),
        // skit it
        //
          LastDeviceNode = NextDevicePathNode (LastDeviceNode);
      }

      //
      // if the device path not only point to driver device, it is not a messaging device path,
      //
      if (!IsDevicePathEndType (LastDeviceNode)) {
        break;
      }

      switch (DevicePathSubType (TempDevicePath)) {
      case MSG_ATAPI_DP:
        BootType = BDS_EFI_MESSAGE_ATAPI_BOOT;
        break;

      case MSG_USB_DP:
        BootType = BDS_EFI_MESSAGE_USB_DEVICE_BOOT;
        break;

      case MSG_SCSI_DP:
        BootType = BDS_EFI_MESSAGE_SCSI_BOOT;
        break;

      case MSG_SATA_DP:
        BootType = BDS_EFI_MESSAGE_SATA_BOOT;
        break;

      case MSG_MAC_ADDR_DP:
      case MSG_VLAN_DP:
      case MSG_IPv4_DP:
      case MSG_IPv6_DP:
        BootType = BDS_EFI_MESSAGE_MAC_BOOT;
        break;

      default:
        BootType = BDS_EFI_MESSAGE_MISC_BOOT;
        break;
      }
      return BootType;

    default:
      break;
    }
    TempDevicePath = NextDevicePathNode (TempDevicePath);
  }

  return BDS_EFI_UNSUPPORT;
}

/**
  Check the MAC address is empty

  @param  MacAddr

  @retval TRUE
  @retval FALSE

**/
BOOLEAN
MacEmpty (
  IN     UINT8                  *MacAddr
  )
{
  UINTN       Index;
  BOOLEAN     Result;
  UINT8       TempValue;

  Result    = FALSE;
  TempValue = 0;

  for (Index = 0; Index < UEFI_NETWORK_MAC_ADDRESS_LENGTH; Index++ ) {
    TempValue = TempValue + (UINT8)(MacAddr[Index]);
  }
  if (TempValue == 0) {
    Result = TRUE;
  }
  return Result;
}

/**
  Check two MAC address is equal

  @param  MacAddr1
  @param  MacAddr2

  @retval TRUE
  @retval FALSE

**/
BOOLEAN
MacCompare (
  IN     UINT8                  *MacAddr1,
  IN     UINT8                  *MacAddr2
  )
{
  UINTN       Index;
  BOOLEAN     Result;

  Result = TRUE;

  for (Index = 0; Index < UEFI_NETWORK_MAC_ADDRESS_LENGTH; Index++ ) {
    if ((UINT8)(MacAddr1[Index]) != (UINT8)(MacAddr2[Index])) {
      Result = FALSE;
      break;
    }
  }
  return Result;
}


/**
  Transfer Mac address to device index
  It will compare device and mac address list
  If not found and not over UEFI_NETWORK_BOOT_OPTION_MAX value
  It will insert into mac address

  @param  DeviceMacAddr    Your Devices MAC Address
  @param  MacAddressList   Mac Address list buffer
  @param  DeviceId         Result

  @retval EFI_SUCCESS     Found
  @retval EFI_NOT_FOUND   Not found or buffer full, check with UEFI_NETWORK_BOOT_OPTION_MAX
                          Id = UEFI_NETWORK_BOOT_OPTION_MAX

**/
EFI_STATUS
MacToIndex (
  IN     EFI_MAC_ADDRESS        *DeviceMacAddr,
  IN OUT EFI_MAC_ADDRESS        *MacAddressList,
  OUT    UINTN                  *DeviceId
  )
{
  BOOLEAN      Found;
  UINTN        Index;

  Found = FALSE;
  for (Index = 0; Index < UEFI_NETWORK_BOOT_OPTION_MAX; Index++) {
    if (MacEmpty ((VOID *) &(MacAddressList[Index]))) {
      CopyMem (&(MacAddressList[Index]), DeviceMacAddr, sizeof (EFI_MAC_ADDRESS));
    }
    if (MacCompare ((VOID *) &(MacAddressList[Index]),  (VOID *)(DeviceMacAddr))) {
      *DeviceId = Index;
      return EFI_SUCCESS;
    }
  }
  *DeviceId = Index;
  return EFI_NOT_FOUND;
}

/**
  Build Boot option for UEFI PXE.
  The boot option will follow style.
    "EFI Network (Index) for IPv(4/6) (MAC Address)"

  @param  BdsBootOptionList    Your Devices MAC Address

  @retval EFI_SUCCESS     Boot option build success
  @retval Other           Memory allocate failed.
                          PXE boot option not found.
                          System configuration not found.

**/
EFI_STATUS
BuildNetworkBootOption (
  IN     LIST_ENTRY  *BdsBootOptionList
  )
{
  EFI_STATUS                    Status;
  UINTN                         Index;
  KERNEL_CONFIGURATION          SystemConfiguration;
  EFI_DEVICE_PATH_PROTOCOL      *NetworkDevicePath;
  EFI_DEVICE_PATH_PROTOCOL      *TempDevicePath;
  UINTN                         NumberLoadFileHandles;
  EFI_HANDLE                    *LoadFileHandles;
  CHAR16                        Buffer[UEFI_NETWORK_BOOT_OPTION_MAX_CHAR];
  MAC_ADDR_DEVICE_PATH          *MAC;
  UINTN                         DeviceId;
  EFI_MAC_ADDRESS               *MacAddressList = NULL;
  BOOLEAN                       NeedBuild;
  UINT8                         IpType;
  CHAR16                        VlanString[UEFI_NETWORK_VLAN_STRING_LENGTH];
  UINT16                        VlanID;
  BOOLEAN                       IsIPv4BootOption = FALSE;
  BOOLEAN                       IsIPv6BootOption = FALSE;

  NetworkDevicePath   = NULL;
  TempDevicePath      = NULL;
  MacAddressList      = NULL;
  LoadFileHandles     = NULL;

  Status = GetKernelConfiguration (&SystemConfiguration);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // If Pxe disabled then do not build Network boot variables
  //
  if (SystemConfiguration.PxeBootToLan == 0) {
    return EFI_SUCCESS;
  }

  ZeroMem (Buffer, sizeof (UEFI_NETWORK_BOOT_OPTION_MAX_CHAR) * sizeof (CHAR16));

  MacAddressList = AllocateZeroPool (sizeof (EFI_MAC_ADDRESS) * UEFI_NETWORK_BOOT_OPTION_MAX);
  if (MacAddressList == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ERROR_HANDLE;
  }

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiLoadFileProtocolGuid,
                  NULL,
                  &NumberLoadFileHandles,
                  &LoadFileHandles
                  );
  if (EFI_ERROR (Status)) {
    goto ERROR_HANDLE;
  }

  for (Index = 0; Index < NumberLoadFileHandles; Index++) {
    NeedBuild = FALSE;
    //
    // VlanID is Range 0 ~ 4094, If VlanID == 0xFFFF means not Vlan device
    //
    VlanID              = 0xFFFF;
    MAC                 = NULL;
    IsIPv4BootOption    = FALSE;
    IsIPv6BootOption    = FALSE;
    IpType              = 0;

    Status = gBS->HandleProtocol (
                    LoadFileHandles[Index],
                    &gEfiDevicePathProtocolGuid,
                    (VOID **) &TempDevicePath
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }

    //
    // Scan DevicePath include MAC and IPv4/IPv6
    //
    NetworkDevicePath = TempDevicePath;
    while (!IsDevicePathEnd (TempDevicePath)) {
      if (TempDevicePath->Type == MESSAGING_DEVICE_PATH) {
        if (TempDevicePath->SubType == MSG_MAC_ADDR_DP) {
          MAC = (MAC_ADDR_DEVICE_PATH *)TempDevicePath;
        } else if (TempDevicePath->SubType == MSG_VLAN_DP) {
          VlanID = ((VLAN_DEVICE_PATH *)TempDevicePath)->VlanId;
        } else if (TempDevicePath->SubType == MSG_IPv4_DP) {
          IsIPv4BootOption = TRUE;
        } else if (TempDevicePath->SubType == MSG_IPv6_DP) {
          IsIPv6BootOption = TRUE;
        }
      }
      TempDevicePath = NextDevicePathNode (TempDevicePath);
    }

    //
    // IPv4 and IPv6 in one DevicePath is InValid
    //
    if (IsIPv4BootOption && IsIPv6BootOption) {
      continue;
    }

    //
    // If MAC has not found, Item cannot display
    //
    if (MAC == NULL) {
      continue;
    }

    //
    // Build Policy:
    //   Device Path is:
    //     MAC\IPv4    : "EFI Network X for IPv4 (MAC Address)"
    //     MAC\IPv6    : "EFI Network X for IPv6 (MAC Address)"
    //     MAC         : "EFI Network X for IPv4 (MAC Address)"
    //     Other       : "Others: (Device Path)"
    //
    if (MAC != NULL && (!IsIPv4BootOption && !IsIPv6BootOption)) {
      IsIPv4BootOption = TRUE;
    }

    switch (SystemConfiguration.NetworkProtocol) {
      case UEFI_NETWORK_BOOT_OPTION_IPV4:
      case UEFI_NETWORK_BOOT_OPTION_BOTH:
      //
      // Only IPv4 Device
      //
        if (IsIPv4BootOption && !IsIPv6BootOption) {
          if (MAC != NULL) {
            IpType    = 4;
            NeedBuild = TRUE;
            break;
          }
        }
        //
        // Break when option is Only IPv4
        //
        if (SystemConfiguration.NetworkProtocol == UEFI_NETWORK_BOOT_OPTION_IPV4) {
          break;
        }
      case UEFI_NETWORK_BOOT_OPTION_IPV6:
      //
      // Only IPv6 Device
      //
        if (!IsIPv4BootOption && IsIPv6BootOption) {
          if (MAC != NULL) {
            IpType    = 6;
            NeedBuild = TRUE;
          }
        }
        break;
      case UEFI_NETWORK_BOOT_OPTION_NONE:
      default:
        break;
    }

    //
    // Build when DevicePath check is vaild
    // But if DevicePath include IPv4 and IPv6 is Invaild, will not build it
    //
    if (!IsIPv4BootOption && !IsIPv6BootOption) {
      NeedBuild = TRUE;
    }

    if (NeedBuild) {
      if (IsIPv4BootOption || IsIPv6BootOption) {
        MacToIndex (&(MAC->MacAddress), MacAddressList, &DeviceId);
        UnicodeSPrint (
          Buffer,
          sizeof (Buffer),
          L"EFI Network %d for IPv%d (%02x-%02x-%02x-%02x-%02x-%02x) ",  //        NetworkOptionString,
          DeviceId,
          IpType,
          MAC->MacAddress.Addr[0],
          MAC->MacAddress.Addr[1],
          MAC->MacAddress.Addr[2],
          MAC->MacAddress.Addr[3],
          MAC->MacAddress.Addr[4],
          MAC->MacAddress.Addr[5]
          );
        if (VlanID != 0xFFFF) {
          UnicodeSPrint (
            VlanString,
            sizeof (VlanString),
            L"VLAN(%d)",
            VlanID
            );
          StrCat (Buffer, VlanString);
        }
        BdsLibBuildOptionFromHandle (LoadFileHandles[Index], BdsBootOptionList, Buffer);
      } else {
        //
        // Build unknown load file option
        //
        StrCat (Buffer, L"Others: ");
        StrCat (Buffer, DevicePathToStr (NetworkDevicePath));
        BdsLibBuildOptionFromHandle (LoadFileHandles[Index], BdsBootOptionList, Buffer);
      }
    }
  }

ERROR_HANDLE:
  if (LoadFileHandles) {
    gBS->FreePool (LoadFileHandles);
  }
  if (MacAddressList) {
    gBS->FreePool (MacAddressList);
  }

  return Status;
}

/**
  According to device path to open file .

  @param  DevicePath    Pointer to EFI_DEVICE_PATH_PROTOCOL
  @param  OpenMode      The mode to open the file. The only valid combinations that the
                        file may be opened with are: Read, Read/Write, or Create/Read/
                        Write. See ¡§Related Definitions¡¨ below.
  @param  Attributes    Only valid for EFI_FILE_MODE_CREATE, in which case these
                        are the attribute bits for the newly created file. See ¡§Related
                        Definitions¡¨ below.
  @param  NewHandle     A pointer to the location to return the opened handle for the new
                        file. See the type EFI_FILE_PROTOCOL description.

  @retval EFI_SUCCESS            The file was opened.
  @retval EFI_NOT_FOUND          The specified file could not be found on the device.
  @retval EFI_NO_MEDIA           The device has no medium.
  @retval EFI_MEDIA_CHANGED      The device has a different medium in it or the medium is no
                                 longer supported.
  @retval EFI_DEVICE_ERROR       The device reported an error.
  @retval EFI_VOLUME_CORRUPTED   The file system structures are corrupted.
  @retval EFI_WRITE_PROTECTED    An attempt was made to create a file, or open a file for write
                                 when the media is write-protected.
  @retval EFI_ACCESS_DENIED      The service denied access to the file.
  @retval EFI_OUT_OF_RESOURCES   Not enough resources were available to open the file.
  @retval EFI_VOLUME_FULL        The volume is full.

**/
EFI_STATUS
BdsLibOpenFileFromDevicePath (
  IN  EFI_DEVICE_PATH_PROTOCOL   *DevicePath,
  IN  UINT64                     OpenMode,
  IN  UINT64                     Attributes,
  OUT EFI_FILE_HANDLE            *NewHandle
  )
{
  EFI_STATUS                        Status;
  EFI_HANDLE                        DeviceHandle;
  FILEPATH_DEVICE_PATH              *FilePathNode;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL   *Volume;
  EFI_FILE_HANDLE                   FileHandle;
  EFI_FILE_HANDLE                   LastHandle;
  FILEPATH_DEVICE_PATH              *OriginalFilePathNode;

  FilePathNode = (FILEPATH_DEVICE_PATH *) DevicePath;
  Volume       = NULL;
  *NewHandle   = NULL;

  Status = gBS->LocateDevicePath (
                  &gEfiSimpleFileSystemProtocolGuid,
                  (EFI_DEVICE_PATH_PROTOCOL **) &FilePathNode,
                  &DeviceHandle
                  );
  if (!EFI_ERROR (Status)) {
    Status = gBS->HandleProtocol (DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (VOID **) &Volume);
  }

  if (EFI_ERROR (Status) || Volume == NULL) {
    return EFI_NOT_FOUND;
  }

  Status = Volume->OpenVolume (Volume, &FileHandle);
  if (!EFI_ERROR (Status)) {
    //
    // Duplicate the device path to avoid the access to unaligned device path node.
    // Because the device path consists of one or more FILE PATH MEDIA DEVICE PATH
    // nodes, It assures the fields in device path nodes are 2 byte aligned.
    //
    FilePathNode = (FILEPATH_DEVICE_PATH *) DuplicateDevicePath (
                                              (EFI_DEVICE_PATH_PROTOCOL *)(UINTN)FilePathNode
                                              );
    if (FilePathNode == NULL) {
      FileHandle->Close (FileHandle);
      Status = EFI_OUT_OF_RESOURCES;
    } else {
      OriginalFilePathNode = FilePathNode;
      //
      // Parse each MEDIA_FILEPATH_DP node. There may be more than one, since the
      //  directory information and filename can be seperate. The goal is to inch
      //  our way down each device path node and close the previous node
      //
      while (!IsDevicePathEnd (&FilePathNode->Header)) {
        if (DevicePathType (&FilePathNode->Header) != MEDIA_DEVICE_PATH ||
            DevicePathSubType (&FilePathNode->Header) != MEDIA_FILEPATH_DP) {
          Status = EFI_UNSUPPORTED;
        }

        if (EFI_ERROR (Status)) {
          //
          // Exit loop on Error
          //
          break;
        }

        LastHandle = FileHandle;
        FileHandle = NULL;
        Status = LastHandle->Open (
                               LastHandle,
                               &FileHandle,
                               FilePathNode->PathName,
                               OpenMode,
                               Attributes
                               );

        //
        // Close the previous node
        //
        LastHandle->Close (LastHandle);
        FilePathNode = (FILEPATH_DEVICE_PATH *) NextDevicePathNode (&FilePathNode->Header);
      }
      //
      // Free the allocated memory pool
      //
      gBS->FreePool (OriginalFilePathNode);
    }
  }

  if (Status == EFI_SUCCESS) {
    *NewHandle = FileHandle;
  }

  return Status;
}


/**
  Set a panel to its naive resolution.

  @param  PrimaryVgaHandle         A pointer to primary VGA handle

**/
VOID
SetNativeResolution (
  IN EFI_HANDLE                         *PrimaryVgaHandle
  )
{
  EFI_EDID_DISCOVERED_PROTOCOL           *EdidDiscovered;
  EFI_GRAPHICS_OUTPUT_PROTOCOL           *GraphicsOutput;
  UINT32                                 NativeResolutionX;
  UINT32                                 NativeResolutionY;
  UINTN                                  SizeOfInfo;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION   *Info;
  UINT32                                 ModeNumber;
  EFI_STATUS                             Status;
  UINTN                                  HandleCount;
  EFI_HANDLE                             *HandleBuffer;
  UINTN                                  HandleIndex;
  OEM_LOGO_RESOLUTION_DEFINITION         *OemLogoResolutionPtr;
  OEM_LOGO_RESOLUTION_DEFINITION         LogoResolution;


  if (PrimaryVgaHandle == NULL) {
    Status = gBS->LocateHandleBuffer (
                    ByProtocol,
                    &gEfiEdidDiscoveredProtocolGuid,
                    NULL,
                    &HandleCount,
                    &HandleBuffer
                    );
    if (!EFI_ERROR (Status)) {
      for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++) {
        SetNativeResolution (HandleBuffer[HandleIndex]);
      }
      gBS->FreePool (HandleBuffer);
    }
  } else {
    Status = gBS->HandleProtocol (
                    PrimaryVgaHandle,
                    &gEfiGraphicsOutputProtocolGuid,
                    (VOID **) &GraphicsOutput
                    );
    if (EFI_ERROR (Status)) {
      return;
    }

    OemLogoResolutionPtr = (OEM_LOGO_RESOLUTION_DEFINITION *)PcdGetPtr (PcdDefaultLogoResolution);

    Status = gBS->HandleProtocol (
                    PrimaryVgaHandle,
                    &gEfiEdidDiscoveredProtocolGuid,
                    (VOID **) &EdidDiscovered
                    );
    if (EFI_ERROR (Status)) {
      return;
    }

    Status = GetResolutionByEdid (EdidDiscovered, &NativeResolutionX, &NativeResolutionY);
    if (EFI_ERROR (Status)) {
      return;
    }

    LogoResolution.LogoResolutionX = NativeResolutionX;
    LogoResolution.LogoResolutionY = NativeResolutionY;
    Status = CheckModeSupported (
               GraphicsOutput,
               NativeResolutionX,
               NativeResolutionY,
               &ModeNumber
               );
    if (!EFI_ERROR(Status)) {
      OemLogoResolutionPtr->LogoResolutionX = LogoResolution.LogoResolutionX;
      OemLogoResolutionPtr->LogoResolutionY = LogoResolution.LogoResolutionY;
    }


    //
    // OemServices
    //
    Status = OemSvcLogoResolution (&OemLogoResolutionPtr);

    Status = GraphicsOutput->QueryMode (
                               GraphicsOutput,
                               GraphicsOutput->Mode->Mode,
                               &SizeOfInfo,
                               &Info
                               );
    if (!EFI_ERROR (Status)) {
      if (Info->HorizontalResolution != OemLogoResolutionPtr->LogoResolutionX ||
          Info->VerticalResolution != OemLogoResolutionPtr->LogoResolutionY) {
        Status = CheckModeSupported (
                   GraphicsOutput,
                   OemLogoResolutionPtr->LogoResolutionX,
                   OemLogoResolutionPtr->LogoResolutionY,
                   &ModeNumber
                   );
        if (EFI_ERROR (Status)) {
          Status = CheckModeSupported (
                     GraphicsOutput,
                     DEFAULT_HORIZONTAL_RESOLUTION,
                     DEFAULT_VERTICAL_RESOLUTION,
                     &ModeNumber
                     );
          if (!EFI_ERROR (Status)) {
            Status = GraphicsOutput->SetMode (GraphicsOutput, ModeNumber);
          }
        } else {
          Status = GraphicsOutput->SetMode (GraphicsOutput, ModeNumber);
        }
      }
      gBS->FreePool (Info);
    }
  }
}

/**
  Update Bgrt information and retrive image information.

  @param  PrimaryVgaHandle   A pointer to primary vga handle
  @param  BltBuffer          A pointer to Blt Buffer which contains image information
  @param  DestinationX       A pointer to a calculated offset X which will be set into Bgrt
  @param  DestinationY       A pointer to a calculated offset Y which will be set into Bgrt
  @param  Width              A pointer to a image size width which will be set into Bgrt
  @param  Height             A pointer to a image size height which will be set into Bgrt

  @retval EFI_SUCCESS   The function runs correctly.

**/
EFI_STATUS
BgrtUpdateImageInfo (
  IN  EFI_HANDLE                        *PrimaryVgaHandle,
  OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL     **BltBuffer,
  OUT UINTN                             *DestinationX,
  OUT UINTN                             *DestinationY,
  OUT UINTN                             *Width,
  OUT UINTN                             *Height
  )
{
  EFI_STATUS                                    Status;
  UINT8                                         *Blt;
  UINT8                                         *ImageData;
  UINTN                                         ImageSize;
  UINTN                                         ResolutionX;
  UINTN                                         ResolutionY;
  UINTN                                         ImageWidth;
  UINTN                                         ImageHeight;
  EFI_BADGING_SUPPORT_FORMAT                    BadgingImageFormat;
  EFI_GRAPHICS_OUTPUT_PROTOCOL                  *GraphicsOutput;
  UINT32                                        ModeNumber;
  OEM_LOGO_RESOLUTION_DEFINITION                *OemLogoResolutionPtr;
  OEM_LOGO_RESOLUTION_DEFINITION                LogoResolution;
  EFI_EDID_DISCOVERED_PROTOCOL                  *EdidDiscovered;
  UINT32                                        NativeResolutionX;
  UINT32                                        NativeResolutionY;

  Blt                = NULL;
  ImageData          = NULL;
  ImageSize          = 0;
  ImageWidth         = 0;
  ImageHeight        = 0;
  ModeNumber         = 0;
  BadgingImageFormat = EfiBadgingSupportFormatUnknown;
  ResolutionX = 0;
  ResolutionY = 0;

  Status = BgrtGetImageByBadging (&ImageData, &ImageSize, &BadgingImageFormat);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = gBS->HandleProtocol (
                  PrimaryVgaHandle,
                  &gEfiGraphicsOutputProtocolGuid,
                  (VOID **) &GraphicsOutput
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = gBS->HandleProtocol (
                  PrimaryVgaHandle,
                  &gEfiEdidDiscoveredProtocolGuid,
                  (VOID **) &EdidDiscovered
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  OemLogoResolutionPtr = (OEM_LOGO_RESOLUTION_DEFINITION *)PcdGetPtr (PcdDefaultLogoResolution);

  Status = GetResolutionByEdid (EdidDiscovered, &NativeResolutionX, &NativeResolutionY);
  if (EFI_ERROR (Status)) {
    return  Status;
  }

  LogoResolution.LogoResolutionX = NativeResolutionX;
  LogoResolution.LogoResolutionY = NativeResolutionY;
  Status = CheckModeSupported (
             GraphicsOutput,
             NativeResolutionX,
             NativeResolutionY,
             &ModeNumber
             );
  if (!EFI_ERROR(Status)) {
    OemLogoResolutionPtr->LogoResolutionX = LogoResolution.LogoResolutionX;
    OemLogoResolutionPtr->LogoResolutionY = LogoResolution.LogoResolutionY;
  }


  //
  // OemServices
  //
  Status = OemSvcLogoResolution (
             &OemLogoResolutionPtr
             );

  Status = CheckModeSupported (
                     GraphicsOutput,
                     OemLogoResolutionPtr->LogoResolutionX,
                     OemLogoResolutionPtr->LogoResolutionY,
                     &ModeNumber
                     );
  if (EFI_ERROR (Status)) {
    Status = CheckModeSupported (
               GraphicsOutput,
               DEFAULT_HORIZONTAL_RESOLUTION,
               DEFAULT_VERTICAL_RESOLUTION,
               &ModeNumber
               );
    if (!EFI_ERROR (Status)) {
      ResolutionX = DEFAULT_HORIZONTAL_RESOLUTION;
      ResolutionY = DEFAULT_VERTICAL_RESOLUTION;
    }
  } else {
    ResolutionX = OemLogoResolutionPtr->LogoResolutionX;
    ResolutionY = OemLogoResolutionPtr->LogoResolutionY;
  }

  Status = BgrtDecodeImageToBlt (
             ImageData,
             ImageSize,
             BadgingImageFormat,
             &Blt,
             &ImageWidth,
             &ImageHeight
             );

  if (!EFI_ERROR(Status)) {
    *BltBuffer = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL*)Blt;
    *Width     = ImageWidth;
    *Height    = ImageHeight;
  }

  BgrtGetLogoCoordinate (
    ResolutionX,
    ResolutionY,
    ImageWidth,
    ImageHeight,
    DestinationX,
    DestinationY
    );

  if (ImageData != NULL) {
    gBS->FreePool (ImageData);
  }

  return Status;
}

/**
  Create dummy boot options which use to selected by UEFI OS.

  @retval EFI_SUCCESS   Create dummy boot options successful.
  @retval Other         Any error occured while creating dummy boot options .

**/
EFI_STATUS
CreateDummyBootOptions (
  VOID
  )
{
  UINT16                        BootOption[BOOT_OPTION_MAX_CHAR];
  UINTN                         VariableSize;
  UINT8                         *BootOptionVar;
  EFI_STATUS                    Status;
  EFI_DEVICE_PATH_PROTOCOL      DevicePath;
  UINTN                         TableCnt;
  UINTN                         Index;

  TableCnt = sizeof (mDummyBootOptionTable) / sizeof (DUMMY_BOOT_OPTION_INFO);
  Status = EFI_SUCCESS;
  for (Index = 0; Index < TableCnt; Index++) {
    ASSERT (BdsLibIsDummyBootOption (mDummyBootOptionTable[Index].BootOptionNum));
    UnicodeSPrint (BootOption, sizeof (BootOption), L"Boot%04x", mDummyBootOptionTable[Index].BootOptionNum);
    BootOptionVar = BdsLibGetVariableAndSize (
                      BootOption,
                      &gEfiGlobalVariableGuid,
                      &VariableSize
                      );
    if (BootOptionVar != NULL) {
      gBS->FreePool (BootOptionVar);
    } else {
      SetDevicePathEndNode (&DevicePath);
      Status = BdsLibUpdateOptionVar (
                 BootOption,
                 &DevicePath,
                 mDummyBootOptionTable[Index].Description,
                 (UINT8 *) "RC",
                 2
                 );
      ASSERT_EFI_ERROR (Status);
    }
  }
  return Status;
}


/**
 Parse the input boot order and return first adjacent boot options which belongs to specific dummy boot option.
 If the first option of boot order does not belong to specific dummy boot option, return not found.

 @param[in]  DummyOptionNum    Dummy boot option number.
 @param[in]  BootOrder         Pointer to the boot order.
 @param[in]  BootOrderNum      The number of boot options in the boot order.
 @param[out] OptionNum         Pointer to the number of boot options returned in the output option list.

 @return The list of boot options which are the first adjacent boot options in boot order and belongs to specific
         dummy boot option or NULL if not found.
**/
UINT16 *
GetFirstAdjacentOptionsByDummyOptionNum (
  IN  DUMMY_BOOT_OPTION_NUM   DummyOptionNum,
  IN  UINT16                  *BootOrder,
  IN  UINTN                   BootOrderNum,
  OUT UINTN                   *OptionNum
  )
{
  UINTN                      Index;

  for (Index = 0; Index < BootOrderNum; Index++) {
    if (BdsLibGetDummyBootOptionNum (BootOrder[Index]) != DummyOptionNum) {
      break;
    }
  }

  *OptionNum  = Index;
  return AllocateCopyPool (Index * sizeof(UINT16), BootOrder);
}

/**
 Parse the input boot order and return all boot options which belong to specific dummy boot option.

 @param[in]  DummyOptionNum    Dummy boot option number.
 @param[in]  BootOrder         Pointer to the boot order.
 @param[in]  BootOrderNum      The number of boot options in the boot order.
 @param[out] OptionNum         Pointer to the number of boot options returned in the output option list.

 @return The list of boot options which belong to specific dummy boot option or NULL if not found.
**/
UINT16 *
GetAllOptionsByDummyOptionNum (
  IN  DUMMY_BOOT_OPTION_NUM    DummyOptionNum,
  IN  UINT16                   *BootOrder,
  IN  UINTN                    BootOrderNum,
  OUT UINTN                    *OptionNum
  )
{
  UINTN                      Index;
  UINTN                      Count;
  UINT16                     *OptionList;

  Count = 0;
  OptionList = AllocatePool (BootOrderNum * sizeof(UINT16));
  if (OptionList == NULL) {
    return OptionList;
  }

  for (Index = 0; Index < BootOrderNum; Index++) {
    if (BdsLibGetDummyBootOptionNum (BootOrder[Index]) == DummyOptionNum) {
      OptionList[Count] = BootOrder[Index];
      Count++;
    }
  }

  *OptionNum  = Count;

  if (Count == 0) {
    if (OptionList != NULL) {
      gBS->FreePool (OptionList);
      OptionList = NULL;
    }
  }

  return OptionList;
}

/**
 Add boot option(s) into the input boot order.

 @param[in]      OptionList     The list of boot options.
 @param[in]      OptionNum      The number of boot options in the option list.
 @param[in]      BootOrderSize  The buffer size of boot order.
 @param[in, out] BootOrder      Pointer to the boot order.
 @param[in, out] BootOrderNum   Pointer to the number of boot options in the boot order.

 @retval EFI_SUCCESS            Success to add boot options into input boot order.
 @retval EFI_INVALID_PARAMETER  Input parameter is NULL.
 @retval EFI_OUT_OF_RESOURCES   The buffer size of boot order is not enough.
**/
EFI_STATUS
AddOptionsToBootOrder (
  IN     UINT16              *OptionList,
  IN     UINTN               OptionNum,
  IN     UINTN               BootOrderSize,
  IN OUT UINT16              *BootOrder,
  IN OUT UINTN               *BootOrderNum
  )
{
  if (OptionList == NULL || BootOrder == NULL || BootOrderNum == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (BootOrderSize < (*BootOrderNum + OptionNum) * sizeof(UINT16)) {
    return EFI_OUT_OF_RESOURCES;
  }

  CopyMem (&BootOrder[*BootOrderNum], OptionList, OptionNum * sizeof(UINT16));
  (*BootOrderNum) += OptionNum;

  return EFI_SUCCESS;
}

/**
 Remove boot option(s) in the input boot order.

 @param[in]      OptionList     The list of boot options.
 @param[in]      OptionNum      The number of boot options in the option list.
 @param[in, out] BootOrder      Pointer to the boot order.
 @param[in, out] BootOrderNum   Pointer to the number of boot options in the boot order.

 @retval EFI_SUCCESS            Success to remove boot options into input boot order.
 @retval EFI_INVALID_PARAMETER  Input parameter is NULL.
**/
EFI_STATUS
RemoveOptionsInBootOrder (
  IN     UINT16              *OptionList,
  IN     UINTN               OptionNum,
  IN OUT UINT16              *BootOrder,
  IN OUT UINTN               *BootOrderNum
  )
{
  UINTN       OptionIndex;
  UINTN       Index;


  if (OptionList == NULL || BootOrder == NULL || BootOrderNum == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  for (OptionIndex = 0; OptionIndex < OptionNum; OptionIndex++) {
    for (Index = 0; Index < *BootOrderNum; Index++) {
      if (BootOrder[Index] == OptionList[OptionIndex]) {
        CopyMem (&BootOrder[Index], &BootOrder[Index + 1], (*BootOrderNum - Index - 1) * sizeof (UINT16));
        (*BootOrderNum)--;
        break;
      }
    }
  }

  return EFI_SUCCESS;
}

/**
  Synchronize the contents of "BootOrder" to prevent from the contents is
  updated by SCU or UEFI OS.

  @retval EFI_SUCCESS   Synchronize data successful.
  @retval Other         Set "BootOrder" varible failed.

**/
EFI_STATUS
SyncBootOrder (
  VOID
  )
{
  UINT16          *BootOrder;
  UINTN           BootOrderSize;
  UINT16          *PhysicalBootOrder;
  UINTN           PhysicalBootOrderSize;
  UINT16          *WorkingBootOrder;
  UINTN           WorkingBootOrderNum;
  EFI_STATUS      Status;
  BOOLEAN         IsPhysicalBootOrder;
  UINTN           Index;
  UINTN           BootOrderNum;
  UINTN           PhysicalBootOrderNum;
  UINTN           WorkingBootOrderSize;
  UINT16          *OptionList;
  UINTN           OptionNum;

  if (!FeaturePcdGet (PcdAutoCreateDummyBootOption)) {
    return EFI_SUCCESS;
  }
  if (BdsLibIsBootOrderHookEnabled ()) {
    return EFI_SUCCESS;
  }


  BootOrder = BdsLibGetVariableAndSize (
                L"BootOrder",
                &gEfiGlobalVariableGuid,
                &BootOrderSize
                );
  if (BootOrder == NULL) {
    return EFI_SUCCESS;
  }
  BootOrderNum = BootOrderSize / sizeof (UINT16);

  Status = BdsLibGetBootOrderType (BootOrder, BootOrderNum, &IsPhysicalBootOrder);
  if (!EFI_ERROR (Status) && IsPhysicalBootOrder) {
    Status = gRT->SetVariable (
                    PHYSICAL_BOOT_ORDER_NAME,
                    &gEfiGenericVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    BootOrderSize,
                    BootOrder
                    );
  } else if (!EFI_ERROR (Status) && !IsPhysicalBootOrder) {
    PhysicalBootOrder = BdsLibGetVariableAndSize (
                          PHYSICAL_BOOT_ORDER_NAME,
                          &gEfiGenericVariableGuid,
                          &PhysicalBootOrderSize
                          );
    PhysicalBootOrderNum = PhysicalBootOrderSize / sizeof (UINT16);

    WorkingBootOrderSize = PhysicalBootOrderSize + BootOrderSize;
    WorkingBootOrder = AllocateZeroPool (WorkingBootOrderSize);
    WorkingBootOrderNum = 0;

    //
    // Based on the priority of BootOrder variable with dummy boot options,
    // sync the priority of physical boot order into working boot order.
    //
    for (Index = 0; Index < BootOrderNum; Index++) {
      if (BdsLibIsDummyBootOption (BootOrder[Index])) {
        if (PhysicalBootOrder == NULL || PhysicalBootOrderNum == 0) {
          continue;
        }

        if (BdsLibGetDummyBootOptionNum (PhysicalBootOrder[0]) == BootOrder[Index]) {
          OptionList = GetFirstAdjacentOptionsByDummyOptionNum (
                         BootOrder[Index],
                         PhysicalBootOrder,
                         PhysicalBootOrderNum,
                         &OptionNum
                         );
        } else {
          OptionList = GetAllOptionsByDummyOptionNum (
                         BootOrder[Index],
                         PhysicalBootOrder,
                         PhysicalBootOrderNum,
                         &OptionNum
                         );
        }

        if (OptionList != NULL) {
          AddOptionsToBootOrder (
            OptionList,
            OptionNum,
            WorkingBootOrderSize,
            WorkingBootOrder,
            &WorkingBootOrderNum
            );
          RemoveOptionsInBootOrder (
            OptionList,
            OptionNum,
            PhysicalBootOrder,
            &PhysicalBootOrderNum
            );
          gBS->FreePool (OptionList);
        }
      } else {
        AddOptionsToBootOrder (
          &BootOrder[Index],
          1,
          WorkingBootOrderSize,
          WorkingBootOrder,
          &WorkingBootOrderNum
          );
        RemoveOptionsInBootOrder (
          &BootOrder[Index],
          1,
          PhysicalBootOrder,
          &PhysicalBootOrderNum
          );
      }
    }

    CopyMem (&WorkingBootOrder[WorkingBootOrderNum], PhysicalBootOrder, PhysicalBootOrderNum * sizeof(UINT16));
    WorkingBootOrderNum += PhysicalBootOrderNum;
    Status = gRT->SetVariable (
                    PHYSICAL_BOOT_ORDER_NAME,
                    &gEfiGenericVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    WorkingBootOrderNum * sizeof (UINT16),
                    WorkingBootOrder
                    );
    if (PhysicalBootOrder != NULL) {
      gBS->FreePool (PhysicalBootOrder);
    }
    if (WorkingBootOrder != NULL) {
      gBS->FreePool (WorkingBootOrder);
    }
  }

  if (BootOrder != NULL) {
    gBS->FreePool (BootOrder);
  }

  return Status;
}

VOID
EnableOptimalTextMode (
  VOID
  )
{
  EFI_STATUS                                Status;
  OEM_LOGO_RESOLUTION_DEFINITION            *OemLogoResolutionPtr;
  OEM_LOGO_RESOLUTION_DEFINITION            LogoResolution;
  EFI_GRAPHICS_OUTPUT_PROTOCOL              *GraphicsOutput;
  UINT32                                    SizeOfX;
  UINT32                                    SizeOfY;
  UINTN                                     RequestedRows;
  UINTN                                     RequestedColumns;
  UINTN                                     TextModeNum;
  UINT32                                    Index;
  EFI_HANDLE                                SinglePhyGopHandle;
  EFI_EDID_DISCOVERED_PROTOCOL              *EdidDiscovered;
  UINT32                                    NativeResolutionX;
  UINT32                                    NativeResolutionY;
  UINTN                                     BestResolution;
  UINTN                                     SizeOfInfo;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION      *Info;
  UINT32                                    GopModeNum;
  UINTN                                     ModeNum;
  UINTN                                     Columns;
  UINTN                                     Rows;
  UINTN                                     OptimalTxtMode;
  EFI_DEVICE_PATH_PROTOCOL                  *ConOutDevicePath;

  //
  //init local vars
  //
  Index = 0;
  SinglePhyGopHandle = NULL;
  OemLogoResolutionPtr = NULL;
  SizeOfInfo = 0;
  Info = NULL;
  BestResolution = 0;
  RequestedColumns  = 0;
  RequestedRows = 0;
  SizeOfX = 0;
  SizeOfY = 0;
  GraphicsOutput = NULL;
  OptimalTxtMode = 0;
  TextModeNum = gST->ConOut->Mode->MaxMode;
  ConOutDevicePath = NULL;

  ConOutDevicePath = GetAllActiveConOutDevPath ();
  if (ConOutDevicePath == NULL) {
    //
    // no monitor is attached
    //
    return ;
  }

  if (FoundTextBasedConsole ()) {
    //
    //if any Console deveice is text-based display,
    //based on text mode intersection, find optimal text mode
    //
    for (ModeNum = 0; ModeNum < (UINTN)(gST->ConOut->Mode->MaxMode); ModeNum++) {
      Status =  gST->ConOut->QueryMode (gST->ConOut, ModeNum, &Columns, &Rows);
      if (!EFI_ERROR (Status)) {
        if ((Columns * Rows) > OptimalTxtMode) {
          OptimalTxtMode = Columns * Rows;
          TextModeNum = (UINTN)ModeNum;
        }
      }
    }
    if (TextModeNum == (UINTN)(gST->ConOut->Mode->MaxMode)) {
      Status = EFI_UNSUPPORTED;
    } else {
      Status = EFI_SUCCESS;
    }
    goto Done;
  }

  OemLogoResolutionPtr = (OEM_LOGO_RESOLUTION_DEFINITION *)PcdGetPtr (PcdDefaultLogoResolution);
  if (IsSinglePhysicalGop (&SinglePhyGopHandle)) {
   gBS->HandleProtocol (
      SinglePhyGopHandle,
      &gEfiGraphicsOutputProtocolGuid,
      (VOID**) &GraphicsOutput
      );
    Status = gBS->HandleProtocol (
                    SinglePhyGopHandle,
                    &gEfiEdidDiscoveredProtocolGuid,
                    (VOID **) &EdidDiscovered
                    );
    if (!EFI_ERROR (Status)) {
    Status = GetResolutionByEdid (EdidDiscovered, &NativeResolutionX, &NativeResolutionY);
      if (!EFI_ERROR (Status)) {
        LogoResolution.LogoResolutionX = NativeResolutionX;
        LogoResolution.LogoResolutionY = NativeResolutionY;
        Status = CheckModeSupported (
                   GraphicsOutput,
                   NativeResolutionX,
                   NativeResolutionY,
                   &GopModeNum
                   );
        if (!EFI_ERROR (Status)) {
          OemLogoResolutionPtr = &LogoResolution;
        }
      }
    } else {
      //
      // find optimal resoltion from gop modes to set for possible combination of two video output devices
      // attached to a gop device
      //
      GetComboVideoOptimalResolution (GraphicsOutput, &SizeOfX, &SizeOfY);
      LogoResolution.LogoResolutionX = SizeOfX;
      LogoResolution.LogoResolutionY = SizeOfY;
      OemLogoResolutionPtr = &LogoResolution;
    }

    OemSvcLogoResolution (&OemLogoResolutionPtr);
    SizeOfX = OemLogoResolutionPtr->LogoResolutionX;
    SizeOfY = OemLogoResolutionPtr->LogoResolutionY;

  } else {
    Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiGraphicsOutputProtocolGuid, (VOID**)&GraphicsOutput);
    if (!EFI_ERROR (Status)) {
      for (Index = 0;Index < GraphicsOutput->Mode->MaxMode; Index++) {
        //
        //find best resolution from virtual gop
        //
        GraphicsOutput->QueryMode (GraphicsOutput, Index, &SizeOfInfo, &Info);
        if (((Info->HorizontalResolution) * (Info->VerticalResolution)) > BestResolution) {
          BestResolution =  (Info->HorizontalResolution) * (Info->VerticalResolution);
          LogoResolution.LogoResolutionX = Info->HorizontalResolution;
          LogoResolution.LogoResolutionY = Info->VerticalResolution;
          OemLogoResolutionPtr = &LogoResolution;
        }
        gBS->FreePool (Info);
      }

      OemSvcLogoResolution (&OemLogoResolutionPtr);
      SizeOfX = OemLogoResolutionPtr->LogoResolutionX;
      SizeOfY = OemLogoResolutionPtr->LogoResolutionY;

    }
  }

  RequestedColumns = SizeOfX / EFI_GLYPH_WIDTH;
  RequestedRows = SizeOfY / EFI_GLYPH_HEIGHT;
  Status = ChkTextModeNum (RequestedColumns, RequestedRows, &TextModeNum);

Done:
  if (!EFI_ERROR (Status)) {
    if ((UINTN)(gST->ConOut->Mode->Mode) != TextModeNum) {
      Status = gST->ConOut->SetMode (gST->ConOut, TextModeNum);
    } else {
      //
      // make sure text dimension's conrresponding resolution is set.
      //
      if (GraphicsOutput != NULL) {    
        if ((SizeOfX != GraphicsOutput->Mode->Info->HorizontalResolution) || 
            (SizeOfY != GraphicsOutput->Mode->Info->VerticalResolution)) {
          CheckModeSupported (
            GraphicsOutput,
            SizeOfX,
            SizeOfY,
            &GopModeNum
            );
          GraphicsOutput->SetMode (GraphicsOutput, GopModeNum);
        }
      }
    }
  } else {
    Status = gST->ConOut->SetMode (gST->ConOut, 0);
  }

}


BOOLEAN
IsSinglePhysicalGop (
  EFI_HANDLE  *SinglePhyGop
  )
{
  EFI_DEVICE_PATH_PROTOCOL              *GopDevicePath;
  EFI_STATUS                             Status;
  UINTN                                  HandleCount;
  EFI_HANDLE                             *HandleBuffer;
  UINTN                                  Index;
  UINTN                                  NumberOfPhysicalGop;

  NumberOfPhysicalGop = 0;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiGraphicsOutputProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiDevicePathProtocolGuid,
                    (VOID*)&GopDevicePath
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }
    *SinglePhyGop = HandleBuffer[Index];
    NumberOfPhysicalGop++;
  }
  gBS->FreePool (HandleBuffer);

  if (NumberOfPhysicalGop == 1) {
    return TRUE;
  } else {
    return FALSE;
  }
}

EFI_STATUS
ChkTextModeNum (
  IN UINTN     RequestedColumns,
  IN UINTN     RequestedRows,
  OUT UINTN    *TextModeNum
  )
{
  UINTN        ModeNum;
  UINTN        Columns;
  UINTN        Rows;
  EFI_STATUS   Status;

  for (ModeNum = 0; ModeNum < (UINTN)(gST->ConOut->Mode->MaxMode); ModeNum++) {
    gST->ConOut->QueryMode (gST->ConOut, ModeNum, &Columns, &Rows);
    if ((RequestedColumns == Columns) && (RequestedRows == Rows)) {
      *TextModeNum = ModeNum;
      break;
    }
  }

  if (ModeNum == (UINTN)(gST->ConOut->Mode->MaxMode)) {
    *TextModeNum = ModeNum;
    Status = EFI_UNSUPPORTED;
  } else {
    Status = EFI_SUCCESS;
  }

  return Status;
}

VOID *
GetAllActiveConOutDevPath (
  VOID
  )
{
  UINTN                      NoHandles;
  EFI_HANDLE                 *HandleBuf;
  EFI_DEVICE_PATH_PROTOCOL   *DevPath;
  EFI_DEVICE_PATH_PROTOCOL   *ConsoleDevPath;
  EFI_DEVICE_PATH_PROTOCOL   *TempDevPath;
  UINT8                      Index;
  EFI_STATUS                 Status;

  //
  //init local
  //
  NoHandles = 0;
  HandleBuf = NULL;
  DevPath = NULL;
  ConsoleDevPath = NULL;
  TempDevPath = NULL;

  Status =  gBS->LocateHandleBuffer (
              ByProtocol,
              &gEfiConsoleOutDeviceGuid,
              NULL,
              &NoHandles,
              &HandleBuf
              );
  if (EFI_ERROR (Status)) {
    return NULL;
  }

  for (Index = 0; Index < NoHandles; Index++) {
    Status =  gBS->HandleProtocol (
                HandleBuf [Index],
                &gEfiDevicePathProtocolGuid,
                (VOID **)&DevPath
                );
    if (EFI_ERROR (Status) || (DevPath == NULL)) {
      continue;
    }

    TempDevPath = ConsoleDevPath;
    ConsoleDevPath = AppendDevicePathInstance (ConsoleDevPath, DevPath);
    if (TempDevPath != NULL) {
      FreePool (TempDevPath);
    }
  }

  FreePool (HandleBuf);
  return ConsoleDevPath;
}

BOOLEAN
FoundTextBasedConsole (
  VOID
  )
{
  EFI_DEVICE_PATH_PROTOCOL         *RemainingDevicePath;
  EFI_DEVICE_PATH_PROTOCOL         *NextDevPathInstance;
  EFI_STATUS                       Status;
  EFI_HANDLE                       Handle;
  EFI_GRAPHICS_OUTPUT_PROTOCOL     *Gop;
  BOOLEAN                          FoundTextBasedCon;
  UINTN                            Size;
  EFI_DEVICE_PATH_PROTOCOL         *ConOutDevPathInstance;

  //
  //init local
  //
  RemainingDevicePath = NULL;
  Gop = NULL;
  FoundTextBasedCon = FALSE;
  NextDevPathInstance = NULL;
  ConOutDevPathInstance = NULL;

  RemainingDevicePath = GetAllActiveConOutDevPath ();
  do {
    //
    //find all output console handles
    //
    NextDevPathInstance = GetNextDevicePathInstance (&RemainingDevicePath, &Size);
    if (NextDevPathInstance != NULL) {
      ConOutDevPathInstance = NextDevPathInstance;
      gBS->LocateDevicePath (&gEfiDevicePathProtocolGuid, &ConOutDevPathInstance, &Handle);
      Status = gBS->HandleProtocol (
                      Handle,
                      &gEfiGraphicsOutputProtocolGuid,
                      (VOID**)&Gop
                      );
       if (EFI_ERROR (Status)) {
         //
         //found text-based console
         //
         FoundTextBasedCon = TRUE;
         FreePool (NextDevPathInstance);
         break;
       }
       FreePool (NextDevPathInstance);
    }
  } while (RemainingDevicePath != NULL);

  return FoundTextBasedCon;
}

VOID
GetComboVideoOptimalResolution (
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL  *GraphicsOutput,
  OUT UINT32                        *XResolution,
  OUT UINT32                        *YResoulution
  )
{
  UINT32                               ModeNumber;
  EFI_STATUS                           Status;
  UINTN                                SizeOfInfo;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Info;
  UINT32                               MaxMode;
  UINTN                                MaxResolution;
  UINTN                                TempResolution;

  Status  = EFI_SUCCESS;
  MaxMode = GraphicsOutput->Mode->MaxMode;
  MaxResolution = 0;
  TempResolution = 0;

  for (ModeNumber = 0; ModeNumber < MaxMode; ModeNumber++) {
    Status = GraphicsOutput->QueryMode (
                       GraphicsOutput,
                       ModeNumber,
                       &SizeOfInfo,
                       &Info
                       );
    if (!EFI_ERROR (Status)) {
      TempResolution = (Info->HorizontalResolution) * (Info->VerticalResolution);
      if (TempResolution > MaxResolution) {
        MaxResolution = TempResolution;
        *XResolution = Info->HorizontalResolution;
        *YResoulution = Info->VerticalResolution;
      }
      gBS->FreePool (Info);
    }
  }
}

VOID
EFIAPI
ShellImageCallback (
  IN EFI_EVENT                          Event,
  IN VOID                               *Context
  )
{
  gST->ConOut->ClearScreen(gST->ConOut);
}

VOID
SignalImageReturns (
  VOID
  )
/*++

Routine Description:

  Signal BIOS after the image returns

Arguments:

  None.

Returns:

  None.

--*/

{
  EFI_HANDLE                        SignalHandle;
  EFI_HANDLE                        *HandleBuffer;
  UINTN                             NumberOfHandles;
  EFI_STATUS                        Status;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gReturnFromImageGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    SignalHandle = NULL;
    Status = gBS->InstallProtocolInterface (
                    &SignalHandle,
                    &gReturnFromImageGuid,
                    EFI_NATIVE_INTERFACE,
                    NULL
                    );
  } else {
    Status = gBS->ReinstallProtocolInterface (
                    HandleBuffer[0],
                    &gReturnFromImageGuid,
                    NULL,
                    NULL
                    );
    gBS->FreePool (HandleBuffer);
  }
}

