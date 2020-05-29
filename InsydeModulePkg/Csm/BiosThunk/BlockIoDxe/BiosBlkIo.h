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

Copyright (c) 1999 - 2011, Intel Corporation. All rights reserved.<BR>

This program and the accompanying materials
are licensed and made available under the terms and conditions
of the BSD License which accompanies this distribution.  The
full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _BIOS_BLOCK_IO_H_
#define _BIOS_BLOCK_IO_H_

#include <Uefi.h>

#include <Protocol/BlockIo.h>
#include <Protocol/PciIo.h>
#include <Protocol/LegacyBios.h>
#include <Protocol/LegacyRegion2.h>
#include <Protocol/DevicePath.h>
#include <Protocol/DiskInfo.h>
#include <Guid/LegacyBios.h>
#include <Guid/BlockIoVendor.h>

#include <Library/UefiDriverEntryPoint.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/DevicePathLib.h>
#include <Library/MemoryAllocationLib.h>

#include <IndustryStandard/Pci.h>
#include <IndustryStandard/Pci22.h>

#include "Edd.h"
//
// Global Variables
//
extern EFI_COMPONENT_NAME_PROTOCOL   gBiosBlockIoComponentName;
extern EFI_COMPONENT_NAME2_PROTOCOL  gBiosBlockIoComponentName2;

//
// Define the I2O class code
//
#define PCI_BASE_CLASS_INTELLIGENT  0x0e
#define PCI_SUB_CLASS_INTELLIGENT   0x00

#define INT13_VECTER                0x4c
#define BDA(a)                      *(UINT16*)((UINTN)0x400 +(a))
#define HARDDISK_NUMBER             0x75
//
// Number of pages needed for our buffer under 1MB
//
#define BLOCK_IO_BUFFER_PAGE_SIZE (((sizeof (EDD_DEVICE_ADDRESS_PACKET) + sizeof (BIOS_LEGACY_DRIVE) + MAX_EDD11_XFER) / EFI_PAGE_SIZE) + 1 \
        )

#define MAX_BBS_ENTRIES             0x100

//
// H2O table
//
typedef struct {                                         //   CSM16 H2O table
  UINT32                      H2OSingnature;             //   '$H2O'
  UINT16                      SwSmiPort;                 //   s/w smi port to be filled by CSM32
  UINT16                      UsbBufferAddr;             //   buf segment @ to be filled by UsbLegacy
  UINT16                      O1394BufferAddr;           //   buf segment @ to be filled by legacy1394
  UINT16                      BootTableOffset;           //   EfiToLegacy16BootTable addr
  UINT16                      BootTableSegment;          //   EfiToLegacy16BootTable addr
  UINT8                       UsbUhciCtrl;               //   Usb Uhci Ctrl Number from csm32
  UINT8                       UsbEhciCtrl;               //   Usb Ehci Ctrl Number from csm32
  UINT16                      CSM16OemSwitch;            //   Switch CSM16 function by OEM service
  UINT16                      SataINT13;                 //   (0:Normal<INT13 support by CSM16>, 1:Legacy Ahci OpRom, 2.Seamless Ahci/RAID(ODD only), 4.Seamless IDE)
  UINT16                      DebugPort;                 //   Debug Port number
  UINT8                       PCI30Support;              //   PCI 3.0 support for EFI and CSM16
  UINT8                       E820SupportExtAttributes;  //   Support E820 new structure (bit 0:CSM16 support , bit 1:EFI support)
  UINT16                      AcpiTimer;                 //   ACPI Timer Address
  UINT16                      Reserve2[3];               //   Reserve
} H2O_TABLE;


//
// Driver Binding Protocol functions
//

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
  );


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
  );

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
  IN  EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN  EFI_HANDLE                   Controller,
  IN  UINTN                        NumberOfChildren,
  IN  EFI_HANDLE                   *ChildHandleBuffer
  );

//
// Other internal functions
//

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
  );

/**
  Initialize block I/O device instance

  @param  Dev   Instance of block I/O device instance

  @retval TRUE  Initialization succeeds.
  @retval FALSE Initialization fails.

**/
BOOLEAN
BiosInitBlockIo (
  IN  BIOS_BLOCK_IO_DEV     *Dev
  );

/**
  Read BufferSize bytes from Lba into Buffer.

  @param  This       Indicates a pointer to the calling context.
  @param  MediaId    Id of the media, changes every time the media is replaced.
  @param  Lba        The starting Logical Block Address to read from
  @param  BufferSize Size of Buffer, must be a multiple of device block size.
  @param  Buffer     A pointer to the destination buffer for the data. The caller is
                     responsible for either having implicit or explicit ownership of the buffer.

  @retval EFI_SUCCESS           The data was read correctly from the device.
  @retval EFI_DEVICE_ERROR      The device reported an error while performing the read.
  @retval EFI_NO_MEDIA          There is no media in the device.
  @retval EFI_MEDIA_CHANGED     The MediaId does not matched the current device.
  @retval EFI_BAD_BUFFER_SIZE   The Buffer was not a multiple of the block size of the device.
  @retval EFI_INVALID_PARAMETER The read request contains LBAs that are not valid,
                                or the buffer is not on proper alignment.

**/
EFI_STATUS
EFIAPI
Edd30BiosReadBlocks (
  IN  EFI_BLOCK_IO_PROTOCOL *This,
  IN  UINT32                MediaId,
  IN  EFI_LBA               Lba,
  IN  UINTN                 BufferSize,
  OUT VOID                  *Buffer
  );

/**
  Write BufferSize bytes from Lba into Buffer.

  @param  This       Indicates a pointer to the calling context.
  @param  MediaId    The media ID that the write request is for.
  @param  Lba        The starting logical block address to be written. The caller is
                     responsible for writing to only legitimate locations.
  @param  BufferSize Size of Buffer, must be a multiple of device block size.
  @param  Buffer     A pointer to the source buffer for the data.

  @retval EFI_SUCCESS           The data was written correctly to the device.
  @retval EFI_WRITE_PROTECTED   The device can not be written to.
  @retval EFI_DEVICE_ERROR      The device reported an error while performing the write.
  @retval EFI_NO_MEDIA          There is no media in the device.
  @retval EFI_MEDIA_CHNAGED     The MediaId does not matched the current device.
  @retval EFI_BAD_BUFFER_SIZE   The Buffer was not a multiple of the block size of the device.
  @retval EFI_INVALID_PARAMETER The write request contains LBAs that are not valid,
                                or the buffer is not on proper alignment.

**/
EFI_STATUS
EFIAPI
Edd30BiosWriteBlocks (
  IN  EFI_BLOCK_IO_PROTOCOL  *This,
  IN  UINT32                 MediaId,
  IN  EFI_LBA                Lba,
  IN  UINTN                  BufferSize,
  OUT VOID                   *Buffer
  );

/**
  Flush the Block Device.

  @param  This              Indicates a pointer to the calling context.

  @retval EFI_SUCCESS       All outstanding data was written to the device
  @retval EFI_DEVICE_ERROR  The device reported an error while writting back the data
  @retval EFI_NO_MEDIA      There is no media in the device.

**/
EFI_STATUS
EFIAPI
BiosBlockIoFlushBlocks (
  IN  EFI_BLOCK_IO_PROTOCOL  *This
  );

/**
  Reset the Block Device.

  @param  This                 Indicates a pointer to the calling context.
  @param  ExtendedVerification Driver may perform diagnostics on reset.

  @retval EFI_SUCCESS          The device was reset.
  @retval EFI_DEVICE_ERROR     The device is not functioning properly and could
                               not be reset.

**/
EFI_STATUS
EFIAPI
BiosBlockIoReset (
  IN  EFI_BLOCK_IO_PROTOCOL  *This,
  IN  BOOLEAN                ExtendedVerification
  );

/**
  Read BufferSize bytes from Lba into Buffer.

  @param  This       Indicates a pointer to the calling context.
  @param  MediaId    Id of the media, changes every time the media is replaced.
  @param  Lba        The starting Logical Block Address to read from
  @param  BufferSize Size of Buffer, must be a multiple of device block size.
  @param  Buffer     A pointer to the destination buffer for the data. The caller is
                     responsible for either having implicit or explicit ownership of the buffer.

  @retval EFI_SUCCESS           The data was read correctly from the device.
  @retval EFI_DEVICE_ERROR      The device reported an error while performing the read.
  @retval EFI_NO_MEDIA          There is no media in the device.
  @retval EFI_MEDIA_CHANGED     The MediaId does not matched the current device.
  @retval EFI_BAD_BUFFER_SIZE   The Buffer was not a multiple of the block size of the device.
  @retval EFI_INVALID_PARAMETER The read request contains LBAs that are not valid,
                                or the buffer is not on proper alignment.

**/
EFI_STATUS
EFIAPI
Edd11BiosReadBlocks (
  IN  EFI_BLOCK_IO_PROTOCOL *This,
  IN  UINT32                MediaId,
  IN  EFI_LBA               Lba,
  IN  UINTN                 BufferSize,
  OUT VOID                  *Buffer
  );

/**
  Write BufferSize bytes from Lba into Buffer.

  @param  This       Indicates a pointer to the calling context.
  @param  MediaId    The media ID that the write request is for.
  @param  Lba        The starting logical block address to be written. The caller is
                     responsible for writing to only legitimate locations.
  @param  BufferSize Size of Buffer, must be a multiple of device block size.
  @param  Buffer     A pointer to the source buffer for the data.

  @retval EFI_SUCCESS           The data was written correctly to the device.
  @retval EFI_WRITE_PROTECTED   The device can not be written to.
  @retval EFI_DEVICE_ERROR      The device reported an error while performing the write.
  @retval EFI_NO_MEDIA          There is no media in the device.
  @retval EFI_MEDIA_CHNAGED     The MediaId does not matched the current device.
  @retval EFI_BAD_BUFFER_SIZE   The Buffer was not a multiple of the block size of the device.
  @retval EFI_INVALID_PARAMETER The write request contains LBAs that are not valid,
                                or the buffer is not on proper alignment.

**/
EFI_STATUS
EFIAPI
Edd11BiosWriteBlocks (
  IN  EFI_BLOCK_IO_PROTOCOL *This,
  IN  UINT32                MediaId,
  IN  EFI_LBA               Lba,
  IN  UINTN                 BufferSize,
  OUT VOID                  *Buffer
  );

/**
  Read BufferSize bytes from Lba into Buffer.

  @param  This       Indicates a pointer to the calling context.
  @param  MediaId    Id of the media, changes every time the media is replaced.
  @param  Lba        The starting Logical Block Address to read from
  @param  BufferSize Size of Buffer, must be a multiple of device block size.
  @param  Buffer     A pointer to the destination buffer for the data. The caller is
                     responsible for either having implicit or explicit ownership of the buffer.

  @retval EFI_SUCCESS           The data was read correctly from the device.
  @retval EFI_DEVICE_ERROR      The device reported an error while performing the read.
  @retval EFI_NO_MEDIA          There is no media in the device.
  @retval EFI_MEDIA_CHANGED     The MediaId does not matched the current device.
  @retval EFI_BAD_BUFFER_SIZE   The Buffer was not a multiple of the block size of the device.
  @retval EFI_INVALID_PARAMETER The read request contains LBAs that are not valid,
                                or the buffer is not on proper alignment.

**/
EFI_STATUS
EFIAPI
BiosReadLegacyDrive (
  IN  EFI_BLOCK_IO_PROTOCOL *This,
  IN  UINT32                MediaId,
  IN  EFI_LBA               Lba,
  IN  UINTN                 BufferSize,
  OUT VOID                  *Buffer
  );

/**
  Write BufferSize bytes from Lba into Buffer.

  @param  This       Indicates a pointer to the calling context.
  @param  MediaId    The media ID that the write request is for.
  @param  Lba        The starting logical block address to be written. The caller is
                     responsible for writing to only legitimate locations.
  @param  BufferSize Size of Buffer, must be a multiple of device block size.
  @param  Buffer     A pointer to the source buffer for the data.

  @retval EFI_SUCCESS           The data was written correctly to the device.
  @retval EFI_WRITE_PROTECTED   The device can not be written to.
  @retval EFI_DEVICE_ERROR      The device reported an error while performing the write.
  @retval EFI_NO_MEDIA          There is no media in the device.
  @retval EFI_MEDIA_CHNAGED     The MediaId does not matched the current device.
  @retval EFI_BAD_BUFFER_SIZE   The Buffer was not a multiple of the block size of the device.
  @retval EFI_INVALID_PARAMETER The write request contains LBAs that are not valid,
                                or the buffer is not on proper alignment.

**/
EFI_STATUS
EFIAPI
BiosWriteLegacyDrive (
  IN  EFI_BLOCK_IO_PROTOCOL *This,
  IN  UINT32                MediaId,
  IN  EFI_LBA               Lba,
  IN  UINTN                 BufferSize,
  OUT VOID                  *Buffer
  );

/**
  Gets parameters of block I/O device.

  @param  BiosBlockIoDev Instance of block I/O device.
  @param  Drive          Legacy drive.

  @return  Result of device parameter retrieval.

**/
UINTN
Int13GetDeviceParameters (
  IN BIOS_BLOCK_IO_DEV    *BiosBlockIoDev,
  IN BIOS_LEGACY_DRIVE    *Drive
  );

/**
  Extension of INT13 call.

  @param  BiosBlockIoDev Instance of block I/O device.
  @param  Drive          Legacy drive.

  @return  Result of this extension.

**/
UINTN
Int13Extensions (
  IN BIOS_BLOCK_IO_DEV    *BiosBlockIoDev,
  IN BIOS_LEGACY_DRIVE    *Drive
  );

/**
  Gets parameters of legacy drive.

  @param  BiosBlockIoDev Instance of block I/O device.
  @param  Drive          Legacy drive.

  @return  Result of drive parameter retrieval.

**/
UINTN
GetDriveParameters (
  IN BIOS_BLOCK_IO_DEV    *BiosBlockIoDev,
  IN  BIOS_LEGACY_DRIVE   *Drive
  );

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
  );

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
  );

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
  IN UINT32                     Bus,
  IN UINT32                     Device,
  IN UINT32                     Function
  );

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
  );
#endif
