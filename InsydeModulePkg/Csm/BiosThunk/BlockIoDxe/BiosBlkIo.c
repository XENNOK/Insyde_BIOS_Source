/** @file
  Provide a BIOS block Io service for RAID mode with Legacy oprom.

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/**
  EFI glue for BIOS INT 13h block devices.

  This file is coded to EDD 3.0 as defined by T13 D1386 Revision 4
  Availible on http://www.t13.org/#Project drafts
  Currently at ftp://fission.dt.wdc.com/pub/standards/x3t13/project/d1386r4.pdf

Copyright (c) 1999 - 2011, Intel Corporation. All rights reserved.<BR>

This program and the accompanying materials
are licensed and made available under the terms and conditions
of the BSD License which accompanies this distribution.  The
full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "BiosBlkIo.h"
#include <PortNumberMap.h>

//
// Global data declaration
//
//
// EFI Driver Binding Protocol Instance
//
EFI_DRIVER_BINDING_PROTOCOL gBiosBlockIoDriverBinding = {
  BiosBlockIoDriverBindingSupported,
  BiosBlockIoDriverBindingStart,
  BiosBlockIoDriverBindingStop,
  0x3,
  NULL,
  NULL
};

//
// Semaphore to control access to global variables mActiveInstances and mBufferUnder1Mb
//
EFI_LOCK                    mGlobalDataLock = EFI_INITIALIZE_LOCK_VARIABLE (TPL_APPLICATION);

//
// Number of active instances of this protocol.  This is used to allocate/free
// the shared buffer.  You must acquire the semaphore to modify.
//
UINTN                       mActiveInstances = 0;

//
// Pointer to the beginning of the buffer used for real mode thunk
// You must acquire the semaphore to modify.
//
EFI_PHYSICAL_ADDRESS        mBufferUnder1Mb = 0;

//
// Address packet is a buffer under 1 MB for all version EDD calls
//
EDD_DEVICE_ADDRESS_PACKET   *mEddBufferUnder1Mb;

//
// This is a buffer for INT 13h func 48 information
//
BIOS_LEGACY_DRIVE           *mLegacyDriverUnder1Mb;

//
// Buffer of 0xFE00 bytes for EDD 1.1 transfer must be under 1 MB
//  0xFE00 bytes is the max transfer size supported.
//
VOID                        *mEdd11Buffer;

UINT32                      mINT13Vector = 0;

UINT16                      mRaidDevCount = 0;

/**
  Driver entry point.

  @param  ImageHandle  Handle of driver image.
  @param  SystemTable  Pointer to system table.

  @retval EFI_SUCCESS  Entrypoint successfully executed.
  @retval Others       Fail to execute entrypoint.

**/
EFI_STATUS
EFIAPI
BiosBlockIoDriverEntryPoint (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS  Status;

  //
  // Install protocols
  //
  Status = EfiLibInstallDriverBindingComponentName2 (
             ImageHandle,
             SystemTable,
             &gBiosBlockIoDriverBinding,
             ImageHandle,
             &gBiosBlockIoComponentName,
             &gBiosBlockIoComponentName2
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Install Legacy BIOS GUID to mark this driver as a BIOS Thunk Driver
  //
  return gBS->InstallMultipleProtocolInterfaces (
                &ImageHandle,
                &gEfiLegacyBiosGuid,
                NULL,
                NULL
                );
}

/**
  Check whether the driver supports this device.

  @param  This                   The Udriver binding protocol.
  @param  Controller             The controller handle to check.
  @param  RemainingDevicePath    The remaining device path.

  @retval EFI_SUCCESS            The driver supports this controller.
  @retval other                  This device isn't supported.

**/
EFI_STATUS
EFIAPI
BiosBlockIoDriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   Controller,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath
  )
{
  EFI_STATUS                Status;
  EFI_PCI_IO_PROTOCOL       *PciIo;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  PCI_TYPE00                Pci;
  UINTN                     Segment;
  UINTN                     Bus;
  UINTN                     Device;
  UINTN                     Function;

  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiDevicePathProtocolGuid,
                  (VOID**) &DevicePath,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  gBS->CloseProtocol (
         Controller,
         &gEfiDevicePathProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  //
  // Open the IO Abstraction(s) needed to perform the supported test
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiPciIoProtocolGuid,
                  (VOID**) &PciIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // See if this is a PCI Controller by looking at the Command register and Class Code Register
  //
  Status = PciIo->Pci.Read (PciIo, EfiPciIoWidthUint32, 0, (sizeof (Pci) / sizeof (UINT32)), &Pci);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  Status = PciIo->GetLocation (PciIo, &Segment, &Bus, &Device, &Function);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  Status = EFI_UNSUPPORTED;

  if (Pci.Hdr.ClassCode[2] == PCI_CLASS_MASS_STORAGE) {
    Status = EFI_SUCCESS;
    if (IsOnBoardPciDevice ((UINT32)Bus, (UINT32)Device, (UINT32)Function) &&
        Pci.Hdr.ClassCode[1] != PCI_CLASS_MASS_STORAGE_RAID) {
      Status = EFI_UNSUPPORTED;
    }
  }

  return Status;
}

/**
  Starts the device with this driver.

  @param  This                   The driver binding instance.
  @param  Controller             Handle of device to bind driver to.
  @param  RemainingDevicePath    Optional parameter use to pick a specific child
                                 device to start.

  @retval EFI_SUCCESS            The controller is controlled by the driver.
  @retval Other                  This controller cannot be started.

**/
EFI_STATUS
EFIAPI
BiosBlockIoDriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   Controller,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath
  )
{
  EFI_STATUS                        Status;
  EFI_LEGACY_BIOS_PROTOCOL          *LegacyBios;
  EFI_PCI_IO_PROTOCOL               *PciIo;
  UINT8                             DiskStart;
  UINT8                             DiskEnd;
  BIOS_BLOCK_IO_DEV                 *BiosBlockIoPrivate;
  EFI_DEVICE_PATH_PROTOCOL          *PciDevPath;
  UINTN                             Flags;
  UINTN                             TmpAddress;
  BOOLEAN                           DeviceEnable;
  BBS_TABLE                         *LocalBbsTable;
  UINTN                             BbsIndex;
  UINTN                             DeviceIndex = 0;
  UINTN                             Index = 0;
  UINT8                             *Ptr;
  EFI_COMPATIBILITY16_TABLE         *Table = NULL;
  UINT8                             DeviceCount = 0;
  EFI_TO_COMPATIBILITY16_BOOT_TABLE *EfiToLegacy16BootTable;
  UINT8                             *EfiToLegacy16BootTablePtr;
  H2O_TABLE                         *H2oTable = NULL;
  PCI_TYPE00                        Pci;
  UINTN                             Segment;
  UINTN                             Bus;
  UINTN                             Device;
  UINTN                             Function;
  EFI_HANDLE                        SataHandle;

  //
  // Initialize variables
  //
  PciIo      = NULL;
  PciDevPath = NULL;
  DeviceEnable = FALSE;
  EfiToLegacy16BootTable = NULL;
  EfiToLegacy16BootTablePtr = NULL;

  //
  // See if the Legacy BIOS Protocol is available
  //
  Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (VOID**) &LegacyBios);
  if (EFI_ERROR (Status)) {
    goto Error;
  }

  //
  // Check to see if there is a legacy option ROM image associated with this PCI device
  //
  Status = LegacyBios->CheckPciRom (LegacyBios, Controller, NULL, NULL, &Flags);
  if (!(Flags & 0x01)) {
    Status = EFI_UNSUPPORTED;
    goto Error;
  }

  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiPciIoProtocolGuid,
                  (VOID**) &PciIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    goto Error;
  }

  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiDevicePathProtocolGuid,
                  (VOID**) &PciDevPath,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );

  if (EFI_ERROR (Status)) {
    goto Error;
  }
  //
  // Enable the device and make sure VGA cycles are being forwarded to this VGA device
  //
  Status = PciIo->Attributes (
                    PciIo,
                    EfiPciIoAttributeOperationEnable,
                    EFI_PCI_DEVICE_ENABLE,
                    NULL
                    );
  if (EFI_ERROR (Status)) {
    goto Error;
  }

  //
  // Search EFI table and H2O table from F segment
  //
  for (Ptr = (UINT8*) ((UINTN) 0xFE000); Ptr < (UINT8*) ((UINTN) 0x100000); Ptr += 0x10) {
    if (*(UINT32*) Ptr == SIGNATURE_32 ('I', 'F', 'E', '$')) {
      Table = (EFI_COMPATIBILITY16_TABLE*) Ptr;
    }

    if (*(UINT32*) Ptr == SIGNATURE_32 ('O', '2', 'H', '$')) {
      H2oTable = (H2O_TABLE*) Ptr;
      EfiToLegacy16BootTablePtr = (UINT8*) (UINTN) (H2oTable->BootTableSegment << 4 | H2oTable->BootTableOffset);
      EfiToLegacy16BootTable = (EFI_TO_COMPATIBILITY16_BOOT_TABLE*) EfiToLegacy16BootTablePtr;
    }

    if ((Table != NULL) && (EfiToLegacy16BootTable != NULL)) {
      break;
    }
  }

  if ((Table == NULL) || (EfiToLegacy16BootTable == NULL)) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Error;
  }

  LocalBbsTable = (BBS_TABLE*) (UINTN) EfiToLegacy16BootTable->BbsTable;

  //
  // See if this is a PCI Controller by looking at the Command register and Class Code Register
  //
  Status = PciIo->Pci.Read (PciIo, EfiPciIoWidthUint32, 0, (sizeof (Pci) / sizeof (UINT32)), &Pci);
  if (EFI_ERROR (Status)) {
    Status = EFI_UNSUPPORTED;
    goto Error;
  }

  Status = PciIo->GetLocation (PciIo, &Segment, &Bus, &Device, &Function);
  if (EFI_ERROR (Status)) {
    Status = EFI_UNSUPPORTED;
    goto Error;
  }

  //
  // Check the BBS index
  //
  for (BbsIndex = (MAX_IDE_CONTROLLER * 2) + 1; BbsIndex < MAX_BBS_ENTRIES; BbsIndex ++ ) {
    if (LocalBbsTable[BbsIndex].BootPriority != BBS_IGNORE_ENTRY) {
      if (LocalBbsTable[BbsIndex].SubClass == Pci.Hdr.ClassCode[1] &&
          LocalBbsTable[BbsIndex].Bus      == Bus &&
          LocalBbsTable[BbsIndex].Device   == Device &&
          LocalBbsTable[BbsIndex].Function == Function &&
          LocalBbsTable[BbsIndex].DeviceType == 0x02) {
        if ((LocalBbsTable[BbsIndex].AdditionalIrq13Handler > 0) && 
            ((LocalBbsTable[BbsIndex].AssignedDriveNumber & 0x80) == 0x80)) {
          //
          // The entry has already been initialized
          //
          return EFI_SUCCESS;
        }
        DeviceCount++;
        if (!DeviceIndex) {
          DeviceIndex = BbsIndex;
        }
      }
    } else {
      break;
    }
  }

  if (DeviceCount > 0) {
    DiskStart = 0x80;
    DiskEnd = DiskStart + DeviceCount;
    if (DeviceCount > 1) {
      //
      // Multiple RAID volumes are found, mark mRaidDevCount to 1 for device path LUN.
      //
      mRaidDevCount = 1;
    }
  } else {
    goto Error;
  }

  //
  // All instances share a buffer under 1MB to put real mode thunk code in
  // If it has not been allocated, then we allocate it.
  //
  if (mBufferUnder1Mb == 0) {
    //
    // Should only be here if there are no active instances
    //
    ASSERT (mActiveInstances == 0);

    //
    // Acquire the lock
    //
    EfiAcquireLock (&mGlobalDataLock);

    //
    // Allocate below 1MB
    //
    mBufferUnder1Mb = 0x00000000000FFFFF;
    Status          = gBS->AllocatePages (AllocateMaxAddress, EfiBootServicesData, BLOCK_IO_BUFFER_PAGE_SIZE, &mBufferUnder1Mb);

    //
    // Release the lock
    //
    EfiReleaseLock (&mGlobalDataLock);

    //
    // Check memory allocation success
    //
    if (EFI_ERROR (Status)) {
      //
      // In checked builds we want to assert if the allocate failed.
      //
      ASSERT_EFI_ERROR (Status);
      Status          = EFI_OUT_OF_RESOURCES;
      mBufferUnder1Mb = 0;
      goto Error;
    }

    TmpAddress = (UINTN) mBufferUnder1Mb;
    //
    // Adjusting the value to be on proper boundary
    //
    mEdd11Buffer = (VOID*) ALIGN_VARIABLE (TmpAddress);

    TmpAddress   = (UINTN) mEdd11Buffer + MAX_EDD11_XFER;
    //
    // Adjusting the value to be on proper boundary
    //
    mLegacyDriverUnder1Mb = (BIOS_LEGACY_DRIVE*) ALIGN_VARIABLE (TmpAddress);

    TmpAddress = (UINTN) mLegacyDriverUnder1Mb + sizeof (BIOS_LEGACY_DRIVE);
    //
    // Adjusting the value to be on proper boundary
    //
    mEddBufferUnder1Mb = (EDD_DEVICE_ADDRESS_PACKET*) ALIGN_VARIABLE (TmpAddress);
  }
  //
  // Allocate the private device structure for each disk
  //
  for (Index = DiskStart; Index < DiskEnd; Index++) {
    Status = gBS->AllocatePool (
                    EfiBootServicesData,
                    sizeof (BIOS_BLOCK_IO_DEV),
                    (VOID**) &BiosBlockIoPrivate
                    );
    if (EFI_ERROR (Status)) {
      goto Error;
    }
    //
    // Zero the private device structure
    //
    ZeroMem (BiosBlockIoPrivate, sizeof (BIOS_BLOCK_IO_DEV));

    //
    // Initialize the private device structure
    //
    BiosBlockIoPrivate->Signature                 = BIOS_CONSOLE_BLOCK_IO_DEV_SIGNATURE;
    BiosBlockIoPrivate->ControllerHandle          = Controller;
    BiosBlockIoPrivate->LegacyBios                = LegacyBios;
    BiosBlockIoPrivate->PciIo                     = PciIo;

    BiosBlockIoPrivate->Bios.Floppy               = FALSE;
    BiosBlockIoPrivate->Bios.Number               = (UINT8) Index;
    BiosBlockIoPrivate->Bios.Letter               = (UINT8) (Index - 0x80 + 'C');
    BiosBlockIoPrivate->BlockMedia.RemovableMedia = FALSE;
    BiosBlockIoPrivate->MbrReadError              = FALSE;

    if (!GenOpRomInt13 (BiosBlockIoPrivate, Table, LocalBbsTable, (DeviceIndex + Index - 0x80), Index, DeviceCount, EfiToLegacy16BootTable)) {
      Status = EFI_UNSUPPORTED;
      goto Error;
    }
    if (BiosInitBlockIo (BiosBlockIoPrivate)) {
      SataHandle = NULL;
      SetBiosInitBlockIoDevicePath (&Pci, PciDevPath, &BiosBlockIoPrivate->Bios, &SataHandle, &BiosBlockIoPrivate->DevicePath);
      if (AsciiStrnCmp ("RAID", BiosBlockIoPrivate->Bios.Parameters.InterfaceType, 4) == 0) {
        if (SataHandle != NULL) {
          Status = gBS->InstallMultipleProtocolInterfaces (
                          &SataHandle,
                          &gEfiBlockIoProtocolGuid,
                          &BiosBlockIoPrivate->BlockIo,
                          &gEfiDevicePathProtocolGuid,
                          BiosBlockIoPrivate->DevicePath,
                          NULL
                          );
        } else {
          //
          // Unable to find out the handle where AhciBus installs DiskInfo protocol, so try to install it again
          //
          Status = gBS->InstallMultipleProtocolInterfaces (
                          &BiosBlockIoPrivate->Handle,
                          &gEfiBlockIoProtocolGuid,
                          &BiosBlockIoPrivate->BlockIo,
                          &gEfiDevicePathProtocolGuid,
                          BiosBlockIoPrivate->DevicePath,
                          NULL
                          );
        }
      } else {
        //
        // Install the Block Io Protocol onto a new child handle
        //
        Status = gBS->InstallMultipleProtocolInterfaces (
                        &BiosBlockIoPrivate->Handle,
                        &gEfiBlockIoProtocolGuid,
                        &BiosBlockIoPrivate->BlockIo,
                        &gEfiDevicePathProtocolGuid,
                        BiosBlockIoPrivate->DevicePath,
                        NULL
                       );
      }
      if (Status == EFI_SUCCESS) {
        //
        // Open For Child Device
        //
        if (SataHandle == NULL) {
          Status = gBS->OpenProtocol (
                          Controller,
                          &gEfiPciIoProtocolGuid,
                          (VOID**) &BiosBlockIoPrivate->PciIo,
                          This->DriverBindingHandle,
                          BiosBlockIoPrivate->Handle,
                          EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER
                          );
        } else {
          Status = gBS->OpenProtocol (
                          Controller,
                          &gEfiPciIoProtocolGuid,
                          (VOID**) &BiosBlockIoPrivate->PciIo,
                          This->DriverBindingHandle,
                          SataHandle,
                          EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER
                          );
        }
      }
    } else {
      gBS->FreePool (BiosBlockIoPrivate);
      Status = EFI_UNSUPPORTED;
    }
  }

Error:
  if ((EFI_ERROR (Status)) || (DeviceCount == 0)) {
    if (PciIo != NULL) {
      if (DeviceEnable) {
        PciIo->Attributes (
                 PciIo,
                 EfiPciIoAttributeOperationDisable,
                 EFI_PCI_DEVICE_ENABLE,
                 NULL
                 );
      }
      gBS->CloseProtocol (
             Controller,
             &gEfiPciIoProtocolGuid,
             This->DriverBindingHandle,
             Controller
             );
      if (PciDevPath != NULL) {
        gBS->CloseProtocol (
               Controller,
               &gEfiDevicePathProtocolGuid,
               This->DriverBindingHandle,
               Controller
               );
      }
      if (mBufferUnder1Mb != 0 && mActiveInstances == 0) {
        gBS->FreePages (mBufferUnder1Mb, BLOCK_IO_BUFFER_PAGE_SIZE);

        //
        // Clear the buffer back to 0
        //
        EfiAcquireLock (&mGlobalDataLock);
        mBufferUnder1Mb = 0;
        EfiReleaseLock (&mGlobalDataLock);
      }
    }
  } else {
    //
    // Successfully installed, so increment the number of active instances
    //
    EfiAcquireLock (&mGlobalDataLock);
    mActiveInstances++;
    EfiReleaseLock (&mGlobalDataLock);
  }

  return Status;
}

/**
  Stop the device handled by this driver.

  @param  This                   The driver binding protocol.
  @param  Controller             The controller to release.
  @param  NumberOfChildren       The number of handles in ChildHandleBuffer.
  @param  ChildHandleBuffer      The array of child handle.

  @retval EFI_SUCCESS            The device was stopped.
  @retval EFI_DEVICE_ERROR       The device could not be stopped due to a device error.
  @retval Others                 Fail to uninstall protocols attached on the device.

**/
EFI_STATUS
EFIAPI
BiosBlockIoDriverBindingStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL     *This,
  IN  EFI_HANDLE                      Controller,
  IN  UINTN                           NumberOfChildren,
  IN  EFI_HANDLE                      *ChildHandleBuffer
  )
{
  EFI_STATUS            Status;
  BOOLEAN               AllChildrenStopped;
  EFI_BLOCK_IO_PROTOCOL *BlockIo;
  BIOS_BLOCK_IO_DEV     *BiosBlockIoPrivate;
  UINTN                 Index;

  //
  // Decrement the number of active instances
  //
  if (mActiveInstances != 0) {
    //
    // Add a check since the stop function will be called 2 times for each handle
    //
    EfiAcquireLock (&mGlobalDataLock);
    mActiveInstances--;
    EfiReleaseLock (&mGlobalDataLock);
  }

  if ((mActiveInstances == 0) && (mBufferUnder1Mb != 0)) {
    //
    // Free our global buffer
    //
    Status = gBS->FreePages (mBufferUnder1Mb, BLOCK_IO_BUFFER_PAGE_SIZE);
    ASSERT_EFI_ERROR (Status);

    EfiAcquireLock (&mGlobalDataLock);
    mBufferUnder1Mb = 0;
    EfiReleaseLock (&mGlobalDataLock);
  }

  AllChildrenStopped = TRUE;

  for (Index = 0; Index < NumberOfChildren; Index++) {
    Status = gBS->OpenProtocol (
                    ChildHandleBuffer[Index],
                    &gEfiBlockIoProtocolGuid,
                    (VOID **) &BlockIo,
                    This->DriverBindingHandle,
                    Controller,
                    EFI_OPEN_PROTOCOL_GET_PROTOCOL
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    BiosBlockIoPrivate = BIOS_BLOCK_IO_FROM_THIS (BlockIo);

    //
    // Release PCI I/O and Block IO Protocols on the clild handle.
    //
    Status = gBS->UninstallMultipleProtocolInterfaces (
                    ChildHandleBuffer[Index],
                    &gEfiBlockIoProtocolGuid,
                    &BiosBlockIoPrivate->BlockIo,
                    &gEfiDevicePathProtocolGuid,
                    BiosBlockIoPrivate->DevicePath,
                    NULL
                    );
    if (EFI_ERROR (Status)) {
      AllChildrenStopped = FALSE;
    }
    //
    // Shutdown the hardware
    //
    BiosBlockIoPrivate->PciIo->Attributes (
                                 BiosBlockIoPrivate->PciIo,
                                 EfiPciIoAttributeOperationDisable,
                                 EFI_PCI_DEVICE_ENABLE,
                                 NULL
                                 );

    gBS->CloseProtocol (
           Controller,
           &gEfiPciIoProtocolGuid,
           This->DriverBindingHandle,
           ChildHandleBuffer[Index]
           );

    gBS->FreePool (BiosBlockIoPrivate);
  }

  if (!AllChildrenStopped) {
    return EFI_DEVICE_ERROR;
  }

  Status = gBS->CloseProtocol (
                  Controller,
                  &gEfiDevicePathProtocolGuid,
                  This->DriverBindingHandle,
                  Controller
                  );

  Status = gBS->CloseProtocol (
                  Controller,
                  &gEfiPciIoProtocolGuid,
                  This->DriverBindingHandle,
                  Controller
                  );

  return EFI_SUCCESS;
}

/**
  Build device path for device.

  @param  Pci                    PCI header instance.
  @param  BaseDevicePath         Base device path.
  @param  Drive                  Legacy drive.
  @param  Handle                 DiskInfo Handle in which the same device path is installed.
  @param  DevicePath             Device path for output.

**/
VOID
SetBiosInitBlockIoDevicePath (
  IN  PCI_TYPE00                *Pci,
  IN  EFI_DEVICE_PATH_PROTOCOL  *BaseDevicePath,
  IN  BIOS_LEGACY_DRIVE         *Drive,
  OUT EFI_HANDLE                *Handle,
  OUT EFI_DEVICE_PATH_PROTOCOL  **DevicePath
  )
{

  EFI_STATUS                  Status;
  BLOCKIO_VENDOR_DEVICE_PATH  VendorNode;
  UINT8                       PortMap;
  UINT8                       PortNumber;

  Status = EFI_UNSUPPORTED;
  PortMap = 0;
  PortNumber = 0;

  //
  // BugBug: Check for memory leaks!
  //
  if (Drive->EddVersion == EDD_VERSION_30) {
    //
    // EDD 3.0 case.
    //
    Status = BuildEdd30DevicePath (Pci, BaseDevicePath, Drive, Handle, DevicePath);
  }

  if (EFI_ERROR (Status)) {
    ZeroMem (&VendorNode, sizeof (VendorNode));
    VendorNode.DevicePath.Header.Type     = HARDWARE_DEVICE_PATH;
    VendorNode.DevicePath.Header.SubType  = HW_VENDOR_DP;
    SetDevicePathNodeLength (&VendorNode.DevicePath.Header, sizeof (VendorNode));
    CopyMem (&VendorNode.DevicePath.Guid, &gBlockIoVendorGuid, sizeof (EFI_GUID));
    VendorNode.LegacyDriveLetter          = Drive->Number;

    if ((Pci->Hdr.VendorId == 0x8086) && (Pci->Hdr.ClassCode[1] == PCI_CLASS_MASS_STORAGE_RAID)) {
      //
      // Intel RAID OpRom ONLY.
      // If BIT0 set, the SATA port number of this device is 0, it BIT1 set port number is 1 and so on.
      //
      PortMap = Drive->Parameters.DevicePath.Atapi.Reserved[2];
      for (PortNumber = 0xFF; PortMap != 0; PortNumber++, PortMap >>= 1);
      VendorNode.LegacyDriveLetter = PortNumber;
    }

    *DevicePath = AppendDevicePathNode (BaseDevicePath, &VendorNode.DevicePath.Header);
  }

}

/**
  Build device path for EDD 3.0.

  @param  Pci                    Pci header instance.
  @param  BaseDevicePath         Base device path.
  @param  Drive                  Legacy drive.
  @param  Handle                 DiskInfo Handle in which the same device path is installed.
  @param  DevicePath             Device path for output.

  @retval EFI_SUCCESS            The device path is built successfully.
  @retval EFI_UNSUPPORTED        It is failed to built device path.

**/
EFI_STATUS
BuildEdd30DevicePath (
  IN  PCI_TYPE00                *Pci,
  IN  EFI_DEVICE_PATH_PROTOCOL  *BaseDevicePath,
  IN  BIOS_LEGACY_DRIVE         *Drive,
  OUT EFI_HANDLE                *Handle,
  OUT EFI_DEVICE_PATH_PROTOCOL  **DevicePath
  )
{

  EFI_DEV_PATH                      Node;
  UINT32                            Controller;
  BIOS_BLOCK_SATA_DEVICE_PATH       SataDevice;
  UINT8                             PortMap;
  UINT8                             PortNumber;
  EFI_STATUS                        Status;

  Controller = (UINT32) Drive->Parameters.InterfacePath.Pci.Controller;

  ZeroMem (&Node, sizeof (Node));
  if ((AsciiStrnCmp ("ATAPI", Drive->Parameters.InterfaceType, 5) == 0) ||
      (AsciiStrnCmp ("ATA", Drive->Parameters.InterfaceType, 3) == 0)
      ) {
    //
    // ATA or ATAPI drive found
    //
    Node.Atapi.Header.Type    = MESSAGING_DEVICE_PATH;
    Node.Atapi.Header.SubType = MSG_ATAPI_DP;
    SetDevicePathNodeLength (&Node.Atapi.Header, sizeof (ATAPI_DEVICE_PATH));
    Node.Atapi.SlaveMaster      = Drive->Parameters.DevicePath.Atapi.Master;
    Node.Atapi.Lun              = Drive->Parameters.DevicePath.Atapi.Lun;
    Node.Atapi.PrimarySecondary = (UINT8) Controller;
  } else {
    //
    // Not an ATA/ATAPI drive
    //
    if (Controller != 0) {
      ZeroMem (&Node, sizeof (Node));
      Node.Controller.Header.Type      = HARDWARE_DEVICE_PATH;
      Node.Controller.Header.SubType   = HW_CONTROLLER_DP;
      SetDevicePathNodeLength (&Node.Controller.Header, sizeof (CONTROLLER_DEVICE_PATH));
      Node.Controller.ControllerNumber = Controller;
      *DevicePath                      = AppendDevicePathNode (*DevicePath, &Node.DevPath);
    }

    ZeroMem (&Node, sizeof (Node));
    ZeroMem (&SataDevice, sizeof (SataDevice));
    if (AsciiStrnCmp ("SCSI", Drive->Parameters.InterfaceType, 4) == 0) {
      //
      // SCSI drive
      //
      Node.Scsi.Header.Type     = MESSAGING_DEVICE_PATH;
      Node.Scsi.Header.SubType  = MSG_SCSI_DP;
      SetDevicePathNodeLength (&Node.Scsi.Header, sizeof (SCSI_DEVICE_PATH));

      //
      // Lun is miss aligned in both EDD and Device Path data structures.
      //  thus we do a byte copy, to prevent alignment traps on IA-64.
      //
      CopyMem (&Node.Scsi.Lun, &Drive->Parameters.DevicePath.Scsi.Lun, sizeof (UINT16));
      Node.Scsi.Pun = Drive->Parameters.DevicePath.Scsi.Pun;

    } else if (AsciiStrnCmp ("USB", Drive->Parameters.InterfaceType, 3) == 0) {
      //
      // USB drive
      //
      Node.Usb.Header.Type    = MESSAGING_DEVICE_PATH;
      Node.Usb.Header.SubType = MSG_USB_DP;
      SetDevicePathNodeLength (&Node.Usb.Header, sizeof (USB_DEVICE_PATH));
      Node.Usb.ParentPortNumber = (UINT8) Drive->Parameters.DevicePath.Usb.Reserved;

    } else if (AsciiStrnCmp ("1394", Drive->Parameters.InterfaceType, 4) == 0) {
      //
      // 1394 drive
      //
      Node.F1394.Header.Type    = MESSAGING_DEVICE_PATH;
      Node.F1394.Header.SubType = MSG_1394_DP;
      SetDevicePathNodeLength (&Node.F1394.Header, sizeof (F1394_DEVICE_PATH));
      Node.F1394.Guid = Drive->Parameters.DevicePath.FireWire.Guid;

    } else if (AsciiStrnCmp ("FIBRE", Drive->Parameters.InterfaceType, 5) == 0) {
      //
      // Fibre drive
      //
      Node.FibreChannel.Header.Type     = MESSAGING_DEVICE_PATH;
      Node.FibreChannel.Header.SubType  = MSG_FIBRECHANNEL_DP;
      SetDevicePathNodeLength (&Node.FibreChannel.Header, sizeof (FIBRECHANNEL_DEVICE_PATH));
      Node.FibreChannel.WWN = Drive->Parameters.DevicePath.FibreChannel.Wwn;
      Node.FibreChannel.Lun = Drive->Parameters.DevicePath.FibreChannel.Lun;
    } else if (AsciiStrnCmp ("I2O", Drive->Parameters.InterfaceType, 3) == 0) {
      Node.I2O.Header.Type     = MESSAGING_DEVICE_PATH;
      Node.I2O.Header.SubType  = MSG_I2O_DP;
      SetDevicePathNodeLength (&Node.Sata.Header, sizeof (I2O_DEVICE_PATH));
      Node.I2O.Tid = (UINT32)Drive->Parameters.DevicePath.I2O.IdentityTag;

    } else if (AsciiStrnCmp ("SATA", Drive->Parameters.InterfaceType, 4) == 0) {
      SataDevice.Header.Type               = MESSAGING_DEVICE_PATH;
      SataDevice.Header.SubType            = MSG_SATA_DP;
      SetDevicePathNodeLength (&SataDevice.Header, sizeof (SATA_DEVICE_PATH));
      SataDevice.HBAPortNumber             = (UINT8)Drive->Parameters.DevicePath.Sata.PortNumber;
      CopyMem (&Node.Sata, &SataDevice, sizeof (SATA_DEVICE_PATH));
    } else if (AsciiStrnCmp ("SAS", Drive->Parameters.InterfaceType, 3) == 0) {
      Node.Sas.Header.Type     = MESSAGING_DEVICE_PATH;
      Node.Sas.Header.SubType  = 0x16; //MSG_SAS_DP
      SetDevicePathNodeLength (&Node.Sata.Header, sizeof (SAS_DEVICE_PATH));
      Node.Sas.SasAddress = Drive->Parameters.DevicePath.SAS.SASAddress;
      Node.Sas.Lun = Drive->Parameters.DevicePath.SAS.Lun;
    } else if (AsciiStrnCmp ("RAID", Drive->Parameters.InterfaceType, 4) == 0) {
      SataDevice.Header.Type     = MESSAGING_DEVICE_PATH;
      SataDevice.Header.SubType  = MSG_SATA_DP;
      SetDevicePathNodeLength (&SataDevice.Header, sizeof (SATA_DEVICE_PATH));
      if ((Pci->Hdr.VendorId == 0x8086) && (Pci->Hdr.ClassCode[1] == PCI_CLASS_MASS_STORAGE_RAID)) {
        //
        // Intel RAID OpRom ONLY.
        // If BIT0 set, the SATA port number of this device is 0, it BIT1 set port number is 1 and so on.
        //
        if (mRaidDevCount <= 1) {
          //
          // Case of single RAID volume or of 1st in multiple RAID volume.
          // Set device path to search for the DiskInfo protocol handle installed by AhciBus..
          // If BIT0 set, the SATA port number of this device is 0, it BIT1 set port number is 1 and so on.
          // Try to find out the handle in which AhciBus installs DiskInfo protocol
          //
          PortMap = Drive->Parameters.DevicePath.Atapi.Reserved[2];
          for (PortNumber = 0xFF; PortMap != 0; PortNumber++, PortMap >>= 1);
          SataDevice.HBAPortNumber = PortNumber;
        } else {
          //
          // Set device path as previous one to search for the DiskInfo handle.
          //
          SataDevice.HBAPortNumber = Drive->Parameters.DevicePath.Atapi.Reserved[2];
          SataDevice.PortMultiplierPortNumber = SATA_HBA_DIRECT_CONNECT_FLAG;
          SataDevice.Lun = (mRaidDevCount - 1);
        }
        //
        // Try to locate device path by DiskInfo protocol to avoid install the same device path error.
        //
        CopyMem (&Node.Sata, &SataDevice, sizeof (SATA_DEVICE_PATH));
        Status = GetDiskInfoHandle (&Node.Sata, Handle);
        if (Status == EFI_SUCCESS) {
          //
          // Set device path
          //
          if (mRaidDevCount <= 1) {
            SataDevice.HBAPortNumber = Drive->Parameters.DevicePath.Atapi.Reserved[2];
            SataDevice.PortMultiplierPortNumber = SATA_HBA_DIRECT_CONNECT_FLAG;
            SataDevice.Lun = mRaidDevCount;
            if (mRaidDevCount == 1) {
              //
              // Increment for next RAID volume
              mRaidDevCount++;
            }
          } else {
            SataDevice.Lun++;
          }
        }
      } else {
        SataDevice.HBAPortNumber = (UINT8) Drive->Parameters.DevicePath.RAID.RAIDArrayNumber;
      }
      CopyMem (&Node.Sata, &SataDevice, sizeof (SATA_DEVICE_PATH));
    }
  }

  if (Node.DevPath.Type == 0) {
    return EFI_UNSUPPORTED;
  }

  *DevicePath = AppendDevicePathNode (BaseDevicePath, &Node.DevPath);

  return EFI_SUCCESS;
}


/**
  Generate the OpRom Int13h vector.

  @param  BiosBlockIoDev          Instance of block I/O device.
  @param  Table                   Legacy drive.
  @param  LocalBbsTable           Pointer to Bbs table.
  @param  BbsIndex                Bbs index of device.
  @param  HDDNumber               Hdd physical number.
  @param  DeviceCount             Device amount
  @param  EfiToLegacy16BootTable  Pointer to Boot Table

  @retval TRUE                    Generate the OpRom Int13h succeeds.
  @retval FALSE                   Generate the OpRom Int13h  fails.
**/
STATIC
BOOLEAN
GenOpRomInt13 (
  IN     BIOS_BLOCK_IO_DEV                  *BiosBlockIoDev,
  IN     EFI_COMPATIBILITY16_TABLE          *Table,
  IN     BBS_TABLE                          *LocalBbsTable,
  IN     UINTN                              BbsIndex,
  IN     UINTN                              HDDNumber,
  IN     UINT8                              DeviceCount,
  IN OUT EFI_TO_COMPATIBILITY16_BOOT_TABLE  *EfiToLegacy16BootTable
  )
{

  EFI_STATUS                   Status;
  EFI_IA32_REGISTER_SET        Regs;
  UINT32                       *INT13Vecter;
  UINT32                       OldINT13;
  EFI_LEGACY_REGION2_PROTOCOL  *LegacyRegion;
  UINTN                        BcvHddNumber;
  UINT8                        NextBbsIndex;

  BcvHddNumber = 0;
  NextBbsIndex = 0;
  Status = gBS->LocateProtocol (&gEfiLegacyRegion2ProtocolGuid, NULL, (VOID **)&LegacyRegion);
  if (EFI_ERROR(Status)) {
    return FALSE;
  }

  Status = LegacyRegion->UnLock (LegacyRegion, 0xC0000, 0x40000, NULL);
  if (EFI_ERROR(Status)) {
    return FALSE;
  }

  INT13Vecter = (UINT32*) (UINTN) 0x4c;
  OldINT13 = *(INT13Vecter);

  if (LocalBbsTable[BbsIndex].AdditionalIrq13Handler) {
    //
    // Legacy Rom INT13
    //
    BiosBlockIoDev->Bios.INT13Vector = LocalBbsTable[BbsIndex].AdditionalIrq13Handler;
  } else {
    //
    // Gen Rom INT13 througth BcvHandle
    //
    ZeroMem (&Regs, sizeof (EFI_IA32_REGISTER_SET));
    Regs.X.AX = 0xffff;
    Regs.X.BX = 0xffff;
    Regs.X.DX = 0xffff;
    Regs.X.ES = Table->PnPInstallationCheckSegment;
    Regs.X.DI = Table->PnPInstallationCheckOffset;
    BiosBlockIoDev->LegacyBios->FarCall86 (
                                  BiosBlockIoDev->LegacyBios,
                                  LocalBbsTable[BbsIndex].BootHandlerSegment,
                                  LocalBbsTable[BbsIndex].BootHandlerOffset,
                                  &Regs,
                                  NULL,
                                  0
                                  );
    //
    // Store the first Int13 function point produced by the OpROM.
    // If following generated Int13 function point is equal to the legacy Int13
    // then use the saved Int13 function point.
    // The sample is the AMD RAID OpROM only generate one Int13 function point
    // and only modify the Int13 vector at the first time.
    //
    if (OldINT13 != *(INT13Vecter)) {
      mINT13Vector = *(INT13Vecter);
    } else {
      if (mINT13Vector != 0) {
        *(INT13Vecter) = mINT13Vector;
      }
    }
    BiosBlockIoDev->Bios.INT13Vector = *(INT13Vecter);
    LocalBbsTable[BbsIndex].AdditionalIrq13Handler = *(INT13Vecter);
    *(INT13Vecter) = OldINT13;
  }
  if (BDA(HARDDISK_NUMBER) <= DeviceCount) {
    LocalBbsTable[BbsIndex].AssignedDriveNumber = (UINT8)((HDDNumber & 0xf0) | (BDA(HARDDISK_NUMBER) - 1));
    BiosBlockIoDev->Bios.Number                 = (UINT8)((HDDNumber & 0xf0) | (BDA(HARDDISK_NUMBER) - 1));
  } else {
    //
    // More than one HDD is under one BBS index
    //
    BcvHddNumber = BDA(HARDDISK_NUMBER);
    LocalBbsTable[BbsIndex].AssignedDriveNumber = (UINT8)(HDDNumber & 0xf0);
    BiosBlockIoDev->Bios.Number                 = (UINT8)(HDDNumber & 0xf0);
    do {
      NextBbsIndex++;
      if (LocalBbsTable[(BbsIndex+ NextBbsIndex)].BootPriority == BBS_IGNORE_ENTRY) {
        //
        // Create/duplicate additional BBS entry for CSM.
        //
        LocalBbsTable[(BbsIndex+ NextBbsIndex)].AssignedDriveNumber      = LocalBbsTable[BbsIndex].AssignedDriveNumber + NextBbsIndex;
        LocalBbsTable[(BbsIndex+ NextBbsIndex)].BootPriority             = BBS_DO_NOT_BOOT_FROM;
        LocalBbsTable[(BbsIndex+ NextBbsIndex)].DeviceType               = LocalBbsTable[BbsIndex].DeviceType;
        LocalBbsTable[(BbsIndex+ NextBbsIndex)].Bus                      = LocalBbsTable[BbsIndex].Bus;
        LocalBbsTable[(BbsIndex+ NextBbsIndex)].Device                   = LocalBbsTable[BbsIndex].Device;
        LocalBbsTable[(BbsIndex+ NextBbsIndex)].Function                 = LocalBbsTable[BbsIndex].Function;
        LocalBbsTable[(BbsIndex+ NextBbsIndex)].StatusFlags.OldPosition  = LocalBbsTable[BbsIndex].StatusFlags.OldPosition;
        LocalBbsTable[(BbsIndex+ NextBbsIndex)].StatusFlags.Reserved1    = LocalBbsTable[BbsIndex].StatusFlags.Reserved1;
        LocalBbsTable[(BbsIndex+ NextBbsIndex)].StatusFlags.Enabled      = LocalBbsTable[BbsIndex].StatusFlags.Enabled;
        LocalBbsTable[(BbsIndex+ NextBbsIndex)].StatusFlags.Failed       = LocalBbsTable[BbsIndex].StatusFlags.Failed;
        LocalBbsTable[(BbsIndex+ NextBbsIndex)].StatusFlags.MediaPresent = LocalBbsTable[BbsIndex].StatusFlags.MediaPresent;
        LocalBbsTable[(BbsIndex+ NextBbsIndex)].StatusFlags.Reserved2    = LocalBbsTable[BbsIndex].StatusFlags.Reserved2;
        LocalBbsTable[(BbsIndex+ NextBbsIndex)].Class                    = LocalBbsTable[BbsIndex].Class;
        LocalBbsTable[(BbsIndex+ NextBbsIndex)].SubClass                 = LocalBbsTable[BbsIndex].SubClass ;
        LocalBbsTable[(BbsIndex+ NextBbsIndex)].DescStringOffset         = LocalBbsTable[BbsIndex].DescStringOffset ;
        LocalBbsTable[(BbsIndex+ NextBbsIndex)].DescStringSegment        = LocalBbsTable[BbsIndex].DescStringSegment;
        LocalBbsTable[(BbsIndex+ NextBbsIndex)].MfgStringOffset          = LocalBbsTable[BbsIndex].MfgStringOffset;
        LocalBbsTable[(BbsIndex+ NextBbsIndex)].MfgStringSegment         = LocalBbsTable[BbsIndex].MfgStringSegment;
        LocalBbsTable[(BbsIndex+ NextBbsIndex)].BootHandlerSegment       = LocalBbsTable[BbsIndex].BootHandlerSegment;
        LocalBbsTable[(BbsIndex+ NextBbsIndex)].BootHandlerOffset        = LocalBbsTable[BbsIndex].BootHandlerOffset;
      }
    } while ((LocalBbsTable[(BbsIndex+ NextBbsIndex)].BootPriority != BBS_IGNORE_ENTRY) &&
             ((LocalBbsTable[(BbsIndex+ NextBbsIndex)].AssignedDriveNumber & 0xF) < (BcvHddNumber - 1)));
    NextBbsIndex++;
    EfiToLegacy16BootTable->NumberBbsEntries = (UINT32) (BbsIndex+ NextBbsIndex);
  }
  Status = LegacyRegion->Lock (LegacyRegion, 0xC0000, 0x40000, NULL);
  if (EFI_ERROR(Status)) {
    return FALSE;
  }

  return TRUE;
}

/**
  According the Bus, Device, Function to check this controller is in Port Number Map table or not.
  If yes, then this is a on board PCI device.

  @param  Bus                    Pci Bus number.
  @param  Device                 Pci Device number.
  @param  Function               Pci Function number.

  @retval TRUE                   Onboard Pci device.
  @retval FALSE                  Not Onboard Pci device.
**/
BOOLEAN
IsOnBoardPciDevice (
  IN     UINT32                              Bus,
  IN     UINT32                              Device,
  IN     UINT32                              Function
  )
{

  UINTN                         Index;
  PORT_NUMBER_MAP               *PortMappingTable;   //retrieved from Pcd
  PORT_NUMBER_MAP               EndEntry;
  UINTN                         NoPorts;

  PortMappingTable = NULL;
  ZeroMem (&EndEntry, sizeof (PORT_NUMBER_MAP));

  PortMappingTable = (PORT_NUMBER_MAP *)PcdGetPtr (PcdPortNumberMapTable);

  if (PortMappingTable == NULL) {
    return FALSE;
  }

  NoPorts = 0;
  while (CompareMem (&EndEntry, &PortMappingTable[NoPorts], sizeof (PORT_NUMBER_MAP)) != 0) {
    NoPorts++;
  }

  if (NoPorts == 0) {
    return FALSE;
  }

  for (Index = 0; Index < NoPorts; Index++) {
    if ((PortMappingTable[Index].Bus == Bus) &&
        (PortMappingTable[Index].Device == Device) &&
        (PortMappingTable[Index].Function == Function)) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
  For RAID interface type, check if the same device path protocol has been installed in handle
  where DiskInfo is installed. If yes, return the handle and uninstall device path protocol installed by AhciBus.

  @param  DiskIofoDevPath       Device path to search for DiskInfo protocol handle .
  @param  Handle                DiskInfo protocol handle in which the same device path is installed.

  @retval EFI_SUCCESS           The same device path is installed, and BlockIo protocol is also successsfully
                                installed.
  @retval EFI_NOT_FOUND         The same device path is not found in DiskInfo handle, try to install device path.
**/
STATIC
EFI_STATUS
GetDiskInfoHandle (
  IN  SATA_DEVICE_PATH          *DiskIofoDevPath,
  OUT EFI_HANDLE                *Handle
  )
{
  UINTN                             HandleCount;
  EFI_HANDLE                        *HandleBuffer;
  UINTN                             HandleIndex;
  EFI_DEVICE_PATH_PROTOCOL          *DiskInfoDevicePath;
  EFI_DEVICE_PATH_PROTOCOL          *DiskIofoDevicePathNode;
  EFI_STATUS                        Status;
  BOOLEAN                           Match;

  HandleCount   = 0;
  HandleBuffer  = NULL;
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiDiskInfoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (Status == EFI_SUCCESS) {
    for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++) {
      Status = gBS->HandleProtocol (
                      HandleBuffer[HandleIndex],
                      &gEfiDevicePathProtocolGuid,
                      (VOID **)&DiskInfoDevicePath
                      );
      if (Status == EFI_SUCCESS) {
        DiskIofoDevicePathNode = DiskInfoDevicePath;
        Match = FALSE;
        while (!IsDevicePathEnd (DiskIofoDevicePathNode)) {
          if ((DevicePathType (DiskIofoDevicePathNode) == MESSAGING_DEVICE_PATH) &&
              (DevicePathSubType (DiskIofoDevicePathNode) == MSG_SATA_DP)) {
            if (CompareMem (
                  DiskIofoDevicePathNode,
                  (EFI_DEVICE_PATH_PROTOCOL*) DiskIofoDevPath,
                  DevicePathNodeLength (DiskIofoDevicePathNode)
                  ) == 0) {
              Match = TRUE;
              break;
            }
          }
          DiskIofoDevicePathNode = NextDevicePathNode (DiskIofoDevicePathNode);
        }
        if (Match) {
          //
          // Handle is found.
          //
          if (DiskIofoDevPath->Lun == 0) {
            //
            // Uninstall device path protocol installed by AhciBus and return the handle.
            //
            Status = gBS->UninstallProtocolInterface (
                            HandleBuffer[HandleIndex],
                            &gEfiDevicePathProtocolGuid,
                            DiskInfoDevicePath
                            );
            if (Status == EFI_SUCCESS) {
              *Handle = HandleBuffer[HandleIndex];
            }
            return Status;
          } else {
            //
            // Device path protocol has been installed by 1st RAID volume,
            // return NULL handle to install device path protocol in another handle.
            //
            return EFI_SUCCESS;
          }
        }
      }
    }
  }
  return EFI_NOT_FOUND;
}