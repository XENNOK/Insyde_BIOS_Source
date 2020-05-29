/** @file

  Performs PCI and Media device inventory for AMT.

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

#include "BiosExtensionLoader.h"
#include "Inventory.h"


AMT_MEDIA_FRU mAmtMediaFru;
AMT_PCI_FRU   mAmtPciFru;

/**
  Transfer each other while being front and back.

  @param[in] Data                 The address of data
  @param[in] Size                 Size of data

  @retval None
**/
VOID
SwapEntries (
  IN CHAR8                        *Data,
  IN UINT8                        Size
  )
{
  UINT16                          Index;
  CHAR8                           Temp8;

  Index = 0;
  while (Data[Index] != 0 && Data[Index + 1] != 0) {
    Temp8           = Data[Index];
    Data[Index]     = Data[Index + 1];
    Data[Index + 1] = Temp8;
    Index += 2;
    if (Index >= Size) {
      break;
    }
  }

  return ;
}

/**
  This function get the next bus range of given address space descriptors.
  It also moves the pointer backward a node, to get prepared to be called
  again.

  @param[in] Descriptors          points to current position of a serial of address space
                                  descriptors
  @param[in] MinBus               The lower range of bus number
  @param[in] MaxBus               The upper range of bus number
  @param[in] IsEnd                Meet end of the serial of descriptors

  @retval EFI_SUCCESS             The command completed successfully
**/
EFI_STATUS
PciGetNextBusRange (
  IN OUT EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR  **Descriptors,
  OUT    UINT16                             *MinBus,
  OUT    UINT16                             *MaxBus,
  OUT    BOOLEAN                            *IsEnd
  )
{
  *IsEnd = FALSE;

  ///
  /// When *Descriptors is NULL, Configuration() is not implemented, so assume
  /// range is 0~PCI_MAX_BUS
  ///
  if ((*Descriptors) == NULL) {
    *MinBus = 0;
    *MaxBus = PCI_MAX_BUS;
    return EFI_SUCCESS;
  }
  ///
  /// *Descriptors points to one or more address space descriptors, which
  /// ends with a end tagged descriptor. Examine each of the descriptors,
  /// if a bus typed one is found and its bus range covers bus, this handle
  /// is the handle we are looking for.
  ///
  if ((*Descriptors)->Desc == ACPI_END_TAG_DESCRIPTOR) {
    *IsEnd = TRUE;
  }

  while ((*Descriptors)->Desc != ACPI_END_TAG_DESCRIPTOR) {
    if ((*Descriptors)->ResType == ACPI_ADDRESS_SPACE_TYPE_BUS) {
      *MinBus = (UINT16) (*Descriptors)->AddrRangeMin;
      *MaxBus = (UINT16) (*Descriptors)->AddrRangeMax;
    }

    (*Descriptors)++;
  }

  return EFI_SUCCESS;
}

/**
  This function gets the protocol interface from the given handle, and
  obtains its address space descriptors.

  @param[in] Handle               The PCI_ROOT_BRIDIGE_IO_PROTOCOL handle
  @param[in] IoDev                Handle used to access configuration space of PCI device
  @param[in] Descriptors          Points to the address space descriptors

  @retval EFI_SUCCESS             The command completed successfully
**/
EFI_STATUS
PciGetProtocolAndResource (
  IN  EFI_HANDLE                            Handle,
  OUT EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL       **IoDev,
  OUT EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR     **Descriptors
  )
{
  EFI_STATUS  Status;

  ///
  /// Get inferface from protocol
  ///
  Status = gBS->HandleProtocol (
                  Handle,
                  &gEfiPciRootBridgeIoProtocolGuid,
                  (VOID **) IoDev
                  );

  if (EFI_ERROR (Status)) {
    return Status;
  }
  ///
  /// Call Configuration() to get address space descriptors
  ///
  Status = (*IoDev)->Configuration (*IoDev, (VOID **) Descriptors);
  if (Status == EFI_UNSUPPORTED) {
    *Descriptors = NULL;
    return EFI_SUCCESS;

  } else {
    return Status;
  }
}
///
/// This is where we list any chipset PCI device that is NOT a FRU.
/// Platform specific PCI devices are listed elsewhere.
///
UINT64  mPciDeviceFilterOutTable[] = {
  EFI_PCI_ADDRESS(00, 00, 00, 00), ///< MCH
  EFI_PCI_ADDRESS(00, 01, 00, 00), ///< PCIe PEG
  EFI_PCI_ADDRESS(00, 02, 00, 00), ///< IGD
  EFI_PCI_ADDRESS(00, 03, 00, 00), ///< Intel High Definition Audio Controller (inside processor)
  EFI_PCI_ADDRESS(00, 30, 00, 00), ///< DMI to PCI Express Bridge
  EFI_PCI_ADDRESS(00, 31, 00, 00), ///< LPC Controller
  EFI_PCI_ADDRESS(00, 31, 02, 00), ///< Serial ATA Controller #1
  EFI_PCI_ADDRESS(00, 31, 03, 00), ///< SMBus Controller
  EFI_PCI_ADDRESS(00, 31, 05, 00), ///< Serial ATA Controller #2
  EFI_PCI_ADDRESS(00, 31, 06, 00), ///< Thermal Subsystem
  EFI_PCI_ADDRESS(00, 29, 00, 00), ///< USB EHCI Controller #1
  EFI_PCI_ADDRESS(00, 29, 01, 00), ///< USB UHCI Controller #1
  EFI_PCI_ADDRESS(00, 29, 02, 00), ///< USB UHCI Controller #2
  EFI_PCI_ADDRESS(00, 29, 03, 00), ///< USB UHCI Controller #3
  EFI_PCI_ADDRESS(00, 29, 04, 00), ///< USB UHCI Controller #4
  EFI_PCI_ADDRESS(00, 29, 07, 00), ///< USBr #1
  EFI_PCI_ADDRESS(00, 28, 00, 00), ///< PCI Express Port #1
  EFI_PCI_ADDRESS(00, 28, 01, 00), ///< PCI Express Port #2
  EFI_PCI_ADDRESS(00, 28, 02, 00), ///< PCI Express Port #3
  EFI_PCI_ADDRESS(00, 28, 03, 00), ///< PCI Express Port #4
  EFI_PCI_ADDRESS(00, 28, 04, 00), ///< PCI Express Port #5
  EFI_PCI_ADDRESS(00, 28, 05, 00), ///< PCI Express Port #6
  EFI_PCI_ADDRESS(00, 28, 06, 00), ///< PCI Express Port #7
  EFI_PCI_ADDRESS(00, 28, 07, 00), ///< PCI Express Port #8
  EFI_PCI_ADDRESS(00, 27, 00, 00), ///< Intel High Definition Audio Controller (inside chipset)
  EFI_PCI_ADDRESS(00, 26, 00, 00), ///< USB EHCI Controller #2
  EFI_PCI_ADDRESS(00, 26, 01, 00), ///< USB UHCI Controller #5
  EFI_PCI_ADDRESS(00, 26, 02, 00), ///< USB UHCI Controller #6
  EFI_PCI_ADDRESS(00, 26, 03, 00), ///< USB UHCI Controller #7
  EFI_PCI_ADDRESS(00, 26, 07, 00), ///< USBr #2
  EFI_PCI_ADDRESS(00, 25, 00, 00), ///< GbE Controller
  EFI_PCI_ADDRESS(00, 24, 00, 00), ///< Dual Channel NAND Controller
  EFI_PCI_ADDRESS(00, 23, 00, 00), ///< Virtualization Engine
  EFI_PCI_ADDRESS(00, 22, 00, 00), ///< HECI #1
  EFI_PCI_ADDRESS(00, 22, 01, 00), ///< HECI #2
  EFI_PCI_ADDRESS(00, 22, 02, 00), ///< IDER
  EFI_PCI_ADDRESS(00, 22, 03, 00), ///< KT
  EFI_PCI_ADDRESS(00, 21, 00, 00), ///< Virtualized SATA Controller
  EFI_PCI_ADDRESS(00, 20, 00, 00), ///< xHCI Controller
  MAX_ADDRESS,
};
#define MAX_FILTER_OUT_DEVICE_NUMBER  0x100

/**
  AMT only need to know removable PCI device information.

  @param[in] None

  @retval EFI_SUCCESS             mAmtPciFru will be update.
  @retval EFI_OUT_OF_RESOURCES    System on-board device list is larger than
                                  MAX_FILTER_OUT_DEVICE_NUMBER supported.
**/
EFI_STATUS
BuildPciFru (
  VOID
  )
{
  EFI_STATUS                        Status;
  EFI_HANDLE                        *HandleBuffer;
  UINTN                             NumberOfHandles;
  UINTN                             Index;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL   *IoDev;
  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *Descriptors;
  UINT16                            MinBus;
  UINT16                            MaxBus;
  BOOLEAN                           IsEnd;
  UINT16                            Bus;
  UINT16                            Device;
  UINT16                            Func;
  UINT64                            Address;
  PCI_COMMON_HEADER                 PciHeader;
  PCI_CONFIG_SPACE                  ConfigSpace;
  BOOLEAN                           ValidDevice;
  UINT8                             CurrentFruDev;
  UINTN                             FilterOutIndex;
  UINT64                            *PciDeviceFilterOutTable;
  UINT64                            *PlatformPciDeviceFilterOutTable;

  CurrentFruDev = 0;
  ValidDevice   = FALSE;

  ///
  /// Get PCI Device Filter Out table about on-board device list from AMT Platform Policy
  /// Only need to report removeable device to AMT
  ///
  PlatformPciDeviceFilterOutTable = (UINT64 *) (UINTN) AmtPciDeviceFilterOutTable ();

  ///
  /// System on-board device list
  ///
  if (sizeof (mPciDeviceFilterOutTable) / sizeof (UINT64) > MAX_FILTER_OUT_DEVICE_NUMBER) {
    return EFI_OUT_OF_RESOURCES;
  }

  PciDeviceFilterOutTable = mPciDeviceFilterOutTable;

  ///
  /// Get all instances of PciRootBridgeIo
  ///
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciRootBridgeIoProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );

  if (EFI_ERROR (Status)) {
    return Status;
  }
  ///
  /// For each handle, which decides a segment and a bus number range,
  /// enumerate all devices on it.
  ///
  for (Index = 0; Index < NumberOfHandles; Index++) {
    Status = PciGetProtocolAndResource (
              HandleBuffer[Index],
              &IoDev,
              &Descriptors
              );
    if (EFI_ERROR (Status)) {
      FreePool (HandleBuffer);
      return Status;
    }
    ///
    /// No document say it's impossible for a RootBridgeIo protocol handle
    /// to have more than one address space descriptors, so find out every
    /// bus range and for each of them do device enumeration.
    ///
    while (TRUE) {
      Status = PciGetNextBusRange (&Descriptors, &MinBus, &MaxBus, &IsEnd);

      if (EFI_ERROR (Status)) {
        FreePool (HandleBuffer);
        return Status;
      }

      if (IsEnd) {
        break;
      }

      for (Bus = MinBus; Bus <= MaxBus; Bus++) {
        ///
        /// For each devices, enumerate all functions it contains
        ///
        for (Device = 0; Device <= PCI_MAX_DEVICE; Device++) {
          ///
          /// For each function, read its configuration space and print summary
          ///
          for (Func = 0; Func <= PCI_MAX_FUNC; Func++) {
            Address = EFI_PCI_ADDRESS (Bus, Device, Func, 0);
            ///
            /// Read the vendor information
            ///
            IoDev->Pci.Read (
                        IoDev,
                        EfiPciWidthUint16,
                        Address,
                        1,
                        &PciHeader.VendorId
                        );

            ///
            /// If VendorId = 0xffff, there does not exist a device at this
            /// location. For each device, if there is any function on it,
            /// there must be 1 function at Function 0. So if Func = 0, there
            /// will be no more functions in the same device, so we can break
            /// loop to deal with the next device.
            ///
            if (PciHeader.VendorId == 0xffff && Func == 0) {
              break;
            }

            if (PciHeader.VendorId != 0xffff) {
              IoDev->Pci.Read (
                          IoDev,
                          EfiPciWidthUint32,
                          Address,
                          sizeof (PciHeader) / sizeof (UINT32),
                          &PciHeader
                          );

              ValidDevice = TRUE;

              ///
              /// Check if any onboard system devices which need to be filter-out and do not report to AMT
              ///
              if (ValidDevice == TRUE) {
                for (FilterOutIndex = 0; FilterOutIndex < (sizeof (mPciDeviceFilterOutTable) / sizeof (UINT64)); FilterOutIndex++) {
                  if (PciDeviceFilterOutTable[FilterOutIndex] == Address) {
                    ///
                    /// Match found so ignore it.
                    ///
                    ValidDevice = FALSE;
                  }

                  if (PciDeviceFilterOutTable[FilterOutIndex] == Address ||
                      PciDeviceFilterOutTable[FilterOutIndex] == MAX_ADDRESS
                      ) {
                    ///
                    /// Match or end of list.
                    ///
                    break;
                  }
                }
              }
              ///
              /// Check if any Platform specific onboard devices which need to be filter-out and do not report to AMT
              ///
              if (ValidDevice == TRUE) {
                if (PlatformPciDeviceFilterOutTable != NULL) {
                  for (FilterOutIndex = 0; FilterOutIndex < MAX_FILTER_OUT_DEVICE_NUMBER; FilterOutIndex++) {
                    if (PlatformPciDeviceFilterOutTable[FilterOutIndex] == Address) {
                      ///
                      /// Match found so ignore it.
                      ///
                      ValidDevice = FALSE;
                    }

                    if (PlatformPciDeviceFilterOutTable[FilterOutIndex] == Address ||
                        PlatformPciDeviceFilterOutTable[FilterOutIndex] == MAX_ADDRESS
                        ) {
                      ///
                      /// Match or end of list.
                      ///
                      break;
                    }
                  }
                }
              }
              ///
              /// Filter-out bridge
              ///
              if (ValidDevice == TRUE) {
                switch (PciHeader.ClassCode[2]) {
                case PCI_CLASS_BRIDGE:
                  ValidDevice = FALSE;
                  break;
                }
              }

              if (ValidDevice == TRUE) {
                mAmtPciFru.PciDevInfo[CurrentFruDev].Vid  = PciHeader.VendorId;
                mAmtPciFru.PciDevInfo[CurrentFruDev].Did  = PciHeader.DeviceId;
                mAmtPciFru.PciDevInfo[CurrentFruDev].Rid  = PciHeader.RevisionId;
                mAmtPciFru.PciDevInfo[CurrentFruDev].ClassCode = ((UINT32) PciHeader.ClassCode[0]) | ((UINT32) PciHeader.ClassCode[1] << 0x8) | ((UINT32) PciHeader.ClassCode[2] << 0x10);

                Status = IoDev->Pci.Read (
                                      IoDev,
                                      EfiPciWidthUint8,
                                      Address,
                                      sizeof (ConfigSpace),
                                      &ConfigSpace
                                      );

                mAmtPciFru.PciDevInfo[CurrentFruDev].SubSysVid  = ConfigSpace.NonCommon.Device.SubVendorId;
                mAmtPciFru.PciDevInfo[CurrentFruDev].SubSysDid  = ConfigSpace.NonCommon.Device.SubSystemId;
                mAmtPciFru.PciDevInfo[CurrentFruDev].BusDevFcn  = (UINT16) (Bus << 0x08 | Device << 0x03 | Func);

                if (CurrentFruDev >= PCI_DEVICE_MAX_NUM - 1) {
                  Status = EFI_DEVICE_ERROR;
                  goto Done;
                }

                CurrentFruDev++;
              }
              ///
              /// If this is not a multi-function device, we can leave the loop
              /// to deal with the next device.
              ///
              if (Func == 0 && ((PciHeader.HeaderType & HEADER_TYPE_MULTI_FUNCTION) == 0x00)) {
                break;
              }
            }
          }
        }
      }
      ///
      /// If Descriptor is NULL, Configuration() returns EFI_UNSUPPRORED,
      /// we assume the bus range is 0~PCI_MAX_BUS. After enumerated all
      /// devices on all bus, we can leave loop.
      ///
      if (Descriptors == NULL) {
        break;
      }
    }
  }

  mAmtPciFru.PciDevicesHeader.VersionInfo = DEVICES_LIST_VERSION;
  mAmtPciFru.PciDevicesHeader.DevCount    = CurrentFruDev;

Done:
  FreePool (HandleBuffer);

  return Status;
}

/**
  Collect all media devices.

  @param[in] None

  @retval None
**/
VOID
EFIAPI
BuildMediaList (
  VOID
  )
{
  EFI_STATUS                Status;
  UINTN                     HandleCount;
  EFI_HANDLE                *HandleBuffer;
  UINTN                     IderHandleCount;
  EFI_HANDLE                *IderHandleBuffer;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePathNode;
  PCI_DEVICE_PATH           *PciDevicePath;
  PCI_DEVICE_PATH           *IderPciDevicePath;
  ACPI_HID_DEVICE_PATH      *AcpiHidDevicePath;
  UINTN                     Index;
  EFI_DISK_INFO_PROTOCOL    *DiskInfo;
  EFI_BLOCK_IO_PROTOCOL     *BlkIo;
  UINT32                    IdeChannel;
  UINT32                    IdeDevice;
  EFI_IDENTIFY_DATA         *IdentifyDriveInfo;
  UINT8                     Index1;
  UINT32                    BufferSize;
  UINT64                    DriveSize;
  UINT8                     MediaDeviceCount;

  MediaDeviceCount  = 0;
  PciDevicePath     = NULL;
  Status            = EFI_SUCCESS;
  IderHandleCount   = 0;
  IderPciDevicePath = NULL;

  ///
  /// Look for IDER PCI Device Path
  ///
  gBS->LocateHandleBuffer (
        ByProtocol,
        &gEfiIderControllerDriverProtocolGuid,
        NULL,
        &IderHandleCount,
        &IderHandleBuffer
        );

  if (IderHandleCount != 0) {
    Status = gBS->HandleProtocol (
                    IderHandleBuffer[0],
                    &gEfiDevicePathProtocolGuid,
                    (VOID *) &DevicePath
                    );
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR (Status)) {
      return ;
    }
    ///
    /// First traverse the device path and look for IDER PCI Device Path
    ///
    DevicePathNode = DevicePath;
    while (!IsDevicePathEnd (DevicePathNode)) {
      if ((DevicePathType (DevicePathNode) == HARDWARE_DEVICE_PATH) &&
          (DevicePathSubType (DevicePathNode) == HW_PCI_DP)
          ) {
        IderPciDevicePath = (PCI_DEVICE_PATH *) DevicePathNode;
        break;
      }

      DevicePathNode = NextDevicePathNode (DevicePathNode);
    }

    FreePool (IderHandleBuffer);
    if (IderPciDevicePath == NULL) {
      DEBUG ((EFI_D_ERROR, " No Ider Pci device \n"));
      return;
    }

  }
  ///
  /// Locate all media devices connected.
  /// We look for the Block I/O protocol to be attached to the device.
  ///
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiBlockIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status) || HandleCount == 0) {
    return ;
  }

  DEBUG ((EFI_D_ERROR, "HandleCount=%X\n", HandleCount));
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiBlockIoProtocolGuid,
                    (VOID **) &BlkIo
                    );
    ASSERT_EFI_ERROR (Status);

    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiDevicePathProtocolGuid,
                    (VOID *) &DevicePath
                    );
    ASSERT_EFI_ERROR (Status);

    ///
    /// We want to remove any Block I/O instances that refer to Logical partitions.
    /// A Block I/O instance is added to the raw device and any partition discovered on
    /// the media. This prevents duplications in our table.
    ///
    if (BlkIo->Media->LogicalPartition) {
      continue;
    }
    ///
    /// First traverse the device path and look for our PCI Device Path Type
    ///
    DevicePathNode  = DevicePath;
    PciDevicePath   = NULL;
    while (!IsDevicePathEnd (DevicePathNode)) {
      if ((DevicePathType (DevicePathNode) == HARDWARE_DEVICE_PATH) &&
          (DevicePathSubType (DevicePathNode) == HW_PCI_DP)
          ) {
        PciDevicePath = (PCI_DEVICE_PATH *) DevicePathNode;
        break;
      }

      DevicePathNode = NextDevicePathNode (DevicePathNode);
    }

    if (PciDevicePath == NULL) {
      continue;
    }
    ///
    /// Skip IDE-R
    ///
    if (IderPciDevicePath != NULL) {
      if (PciDevicePath->Function == IderPciDevicePath->Function && PciDevicePath->Device == IderPciDevicePath->Device) {
        continue;
      }
    }

    mAmtMediaFru.MediaDevInfo[MediaDeviceCount].StructSize  = sizeof (MEBX_FRU_MEDIA_DEVICES);
    mAmtMediaFru.MediaDevInfo[MediaDeviceCount].Rsvd[0]     = 0;

    ///
    /// Next take a look at the Function field of the current device path node to determine if it is
    /// a floppy, or ATA/ATAPI device.
    ///
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiDiskInfoProtocolGuid,
                    (VOID **) &DiskInfo
                    );
//[-start-120727-IB10370012-add]//
    //
    // Treat DiskInfo Protocols other than SATA Interface as not found.
    //
    if ( !( CompareGuid ( &DiskInfo->Interface, &gEfiDiskInfoIdeInterfaceGuid ) ||
            CompareGuid ( &DiskInfo->Interface, &gEfiDiskInfoAhciInterfaceGuid )
            )
         ) {
      Status = EFI_NOT_FOUND;
    }
//[-end-120727-IB10370012-add]/
    if (EFI_ERROR (Status)) {
      ///
      /// No DiskInfo available, check message field
      /// Currently USB does not have a DiskInfo available
      ///
      DevicePathNode = NextDevicePathNode (DevicePathNode);
      while (!IsDevicePathEnd (DevicePathNode)) {
        if ((DevicePathType (DevicePathNode) == MESSAGING_DEVICE_PATH)) {
          switch (DevicePathSubType (DevicePathNode)) {
          case MSG_USB_DP:
            ///
            /// USB Mass Storage Device
            ///
            mAmtMediaFru.MediaDevInfo[MediaDeviceCount].Interface     = MEBX_MEDIA_IN_ATA;
            mAmtMediaFru.MediaDevInfo[MediaDeviceCount].DevType       = MEBX_MEDIA_DT_HDD;
            mAmtMediaFru.MediaDevInfo[MediaDeviceCount].SerialNo[0]   = 0;
            mAmtMediaFru.MediaDevInfo[MediaDeviceCount].VersionNo[0]  = 0;
            AsciiStrCpy ((CHAR8 *) mAmtMediaFru.MediaDevInfo[MediaDeviceCount].ModelNo, "USB");
            mAmtMediaFru.MediaDevInfo[MediaDeviceCount].MaxMediaSize = MultU64x32 (
                                                                        BlkIo->Media->LastBlock + 1,
                                                                        BlkIo->Media->BlockSize
                                                                        );
            mAmtMediaFru.MediaDevInfo[MediaDeviceCount].SupportedCmdSets[0] = 0;
            break;

          case MSG_ATAPI_DP:
            ///
            /// Should be SATA if no DiskInfo Protocol available
            ///
            mAmtMediaFru.MediaDevInfo[MediaDeviceCount].Interface     = MEBX_MEDIA_IN_SATA;
            mAmtMediaFru.MediaDevInfo[MediaDeviceCount].DevType       = MEBX_MEDIA_DT_HDD;
            mAmtMediaFru.MediaDevInfo[MediaDeviceCount].SerialNo[0]   = 0;
            mAmtMediaFru.MediaDevInfo[MediaDeviceCount].VersionNo[0]  = 0;
            AsciiStrCpy ((CHAR8 *) mAmtMediaFru.MediaDevInfo[MediaDeviceCount].ModelNo, "Native SATA");
            mAmtMediaFru.MediaDevInfo[MediaDeviceCount].MaxMediaSize = MultU64x32 (
                                                                        BlkIo->Media->LastBlock + 1,
                                                                        BlkIo->Media->BlockSize
                                                                        );
            mAmtMediaFru.MediaDevInfo[MediaDeviceCount].SupportedCmdSets[0] = 0;
            break;

          default:
            mAmtMediaFru.MediaDevInfo[MediaDeviceCount].Interface     = MEBX_MEDIA_IN_ATA;
            mAmtMediaFru.MediaDevInfo[MediaDeviceCount].DevType       = MEBX_MEDIA_DT_MO;
            mAmtMediaFru.MediaDevInfo[MediaDeviceCount].SerialNo[0]   = 0;
            mAmtMediaFru.MediaDevInfo[MediaDeviceCount].VersionNo[0]  = 0;
            AsciiStrCpy ((CHAR8 *) mAmtMediaFru.MediaDevInfo[MediaDeviceCount].ModelNo, "Unknown");
            mAmtMediaFru.MediaDevInfo[MediaDeviceCount].MaxMediaSize = MultU64x32 (
                                                                        BlkIo->Media->LastBlock + 1,
                                                                        BlkIo->Media->BlockSize
                                                                        );
            mAmtMediaFru.MediaDevInfo[MediaDeviceCount].SupportedCmdSets[0] = 0;
            break;
          }
          ///
          /// End of Switch
          ///
        } else if ((DevicePathType (DevicePathNode) == ACPI_DEVICE_PATH)) {
          AcpiHidDevicePath = (ACPI_HID_DEVICE_PATH *) DevicePathNode;
          if (EISA_ID_TO_NUM (AcpiHidDevicePath->HID) == 0x0604) {
            ///
            /// Floppy
            ///
            mAmtMediaFru.MediaDevInfo[MediaDeviceCount].Interface     = MEBX_MEDIA_IN_ATA;
            mAmtMediaFru.MediaDevInfo[MediaDeviceCount].DevType       = MEBX_MEDIA_DT_FDD;
            mAmtMediaFru.MediaDevInfo[MediaDeviceCount].SerialNo[0]   = 0;
            mAmtMediaFru.MediaDevInfo[MediaDeviceCount].VersionNo[0]  = 0;
            AsciiStrCpy ((CHAR8 *) mAmtMediaFru.MediaDevInfo[MediaDeviceCount].ModelNo, "Floppy");
            mAmtMediaFru.MediaDevInfo[MediaDeviceCount].MaxMediaSize = MultU64x32 (
                                                                        BlkIo->Media->LastBlock + 1,
                                                                        BlkIo->Media->BlockSize
                                                                        );
            mAmtMediaFru.MediaDevInfo[MediaDeviceCount].SupportedCmdSets[0] = 0;
          }
        }

        DevicePathNode = NextDevicePathNode (DevicePathNode);
      }
    } else {
      Status = DiskInfo->WhichIde (
                          DiskInfo,
                          &IdeChannel,
                          &IdeDevice
                          );
      if (Status == EFI_UNSUPPORTED) {
        continue;
      }

      BufferSize        = sizeof (EFI_IDENTIFY_DATA);
      IdentifyDriveInfo = AllocatePool (BufferSize);
      ASSERT (IdentifyDriveInfo != NULL);
      if (IdentifyDriveInfo == NULL) {
        return ;
      }

      Status = DiskInfo->Identify (
                          DiskInfo,
                          IdentifyDriveInfo,
                          &BufferSize
                          );
      ASSERT_EFI_ERROR (Status);

      if (PciDevicePath->Function == 1) {
        ///
        /// PATA Device
        ///
        mAmtMediaFru.MediaDevInfo[MediaDeviceCount].Interface = MEBX_MEDIA_IN_ATA;
      } else if (PciDevicePath->Function == 2) {
        mAmtMediaFru.MediaDevInfo[MediaDeviceCount].Interface = MEBX_MEDIA_IN_SATA;
      }
      ///
      /// Reverse the bytes for proper ordering of characters
      ///
      SwapEntries ((CHAR8 *) &IdentifyDriveInfo->AtaData.ModelName, sizeof (IdentifyDriveInfo->AtaData.ModelName));
      SwapEntries ((CHAR8 *) &IdentifyDriveInfo->AtaData.SerialNo, sizeof (IdentifyDriveInfo->AtaData.SerialNo));

      CopyMem (
        mAmtMediaFru.MediaDevInfo[MediaDeviceCount].ModelNo,
        IdentifyDriveInfo->AtaData.ModelName,
        sizeof (mAmtMediaFru.MediaDevInfo[MediaDeviceCount].ModelNo)
        );

      ///
      /// For HardDisk append the size. Otherwise display atapi
      ///
      if ((IdentifyDriveInfo->AtaData.config & 0x8000) == 00) {
        ///
        /// 48 bit address feature set is supported, get maximum capacity
        ///
        if ((IdentifyDriveInfo->AtaData.command_set_supported_83 & 0x0400) == 0) {
          DriveSize =
            (
              (IdentifyDriveInfo->AtaData.user_addressable_sectors_hi << 16) +
              IdentifyDriveInfo->AtaData.user_addressable_sectors_lo
            );
        } else {
          DriveSize = IdentifyDriveInfo->AtaData.maximum_lba_for_48bit_addressing[0];
          for (Index1 = 1; Index1 < 4; Index1++) {
            ///
            /// Lower byte goes first: word[100] is the lowest word, word[103] is highest
            ///
            DriveSize |= LShiftU64 (
                           IdentifyDriveInfo->AtaData.maximum_lba_for_48bit_addressing[Index1],
                           16 * Index1
                           );
          }
        }

        DriveSize = MultU64x32 (DriveSize, 512);
        CopyMem (
          mAmtMediaFru.MediaDevInfo[MediaDeviceCount].SerialNo,
          IdentifyDriveInfo->AtaData.SerialNo,
          sizeof (mAmtMediaFru.MediaDevInfo[MediaDeviceCount].SerialNo)
          );
        CopyMem (
          mAmtMediaFru.MediaDevInfo[MediaDeviceCount].VersionNo,
          IdentifyDriveInfo->AtaData.FirmwareVer,
          sizeof (mAmtMediaFru.MediaDevInfo[MediaDeviceCount].VersionNo)
          );
        mAmtMediaFru.MediaDevInfo[MediaDeviceCount].SupportedCmdSets[0] = IdentifyDriveInfo->AtaData.command_set_supported_82;
        mAmtMediaFru.MediaDevInfo[MediaDeviceCount].SupportedCmdSets[1] = IdentifyDriveInfo->AtaData.command_set_supported_83;
        mAmtMediaFru.MediaDevInfo[MediaDeviceCount].SupportedCmdSets[2] = IdentifyDriveInfo->AtaData.command_set_feature_extn;
        mAmtMediaFru.MediaDevInfo[MediaDeviceCount].MaxMediaSize  = DriveSize;
        mAmtMediaFru.MediaDevInfo[MediaDeviceCount].DevType       = MEBX_MEDIA_DT_HDD;
      } else {
        mAmtMediaFru.MediaDevInfo[MediaDeviceCount].Interface = MEBX_MEDIA_IN_ATAPI;
        mAmtMediaFru.MediaDevInfo[MediaDeviceCount].MaxMediaSize = MultU64x32 (
                                                                    BlkIo->Media->LastBlock + 1,
                                                                    BlkIo->Media->BlockSize
                                                                    );
        CopyMem (
          mAmtMediaFru.MediaDevInfo[MediaDeviceCount].SerialNo,
          IdentifyDriveInfo->AtapiData.SerialNo,
          sizeof (mAmtMediaFru.MediaDevInfo[MediaDeviceCount].SerialNo)
          );
        CopyMem (
          mAmtMediaFru.MediaDevInfo[MediaDeviceCount].VersionNo,
          IdentifyDriveInfo->AtapiData.FirmwareVer,
          sizeof (mAmtMediaFru.MediaDevInfo[MediaDeviceCount].VersionNo)
          );
        mAmtMediaFru.MediaDevInfo[MediaDeviceCount].SupportedCmdSets[0] = IdentifyDriveInfo->AtapiData.cmd_set_support_82;
        mAmtMediaFru.MediaDevInfo[MediaDeviceCount].SupportedCmdSets[1] = IdentifyDriveInfo->AtapiData.cmd_set_support_83;
        mAmtMediaFru.MediaDevInfo[MediaDeviceCount].SupportedCmdSets[2] = IdentifyDriveInfo->AtapiData.cmd_feature_support;
        if (BlkIo->Media->RemovableMedia) {
          mAmtMediaFru.MediaDevInfo[MediaDeviceCount].DevType = MEBX_MEDIA_DT_DVD;
        }
      }
    }

    DEBUG ((EFI_D_ERROR, "%x DevType=%x, \n", MediaDeviceCount, mAmtMediaFru.MediaDevInfo[MediaDeviceCount].DevType));
    DEBUG ((EFI_D_ERROR, "Interface=%x, \n", mAmtMediaFru.MediaDevInfo[MediaDeviceCount].Interface));
    DEBUG ((EFI_D_ERROR, "MaxMediaSize=%x, \n", mAmtMediaFru.MediaDevInfo[MediaDeviceCount].MaxMediaSize));
    DEBUG ((EFI_D_ERROR, "SupportedCmdSets=%x, \n", mAmtMediaFru.MediaDevInfo[MediaDeviceCount].SupportedCmdSets));

    MediaDeviceCount++;
    if (MediaDeviceCount >= MEDIA_DEVICE_MAX_NUM) {
      break;
    }
  } /// end of handlebuffer blockio
  mAmtMediaFru.MediaDevicesHeader.VersionInfo = DEVICES_LIST_VERSION;
  mAmtMediaFru.MediaDevicesHeader.DevCount    = MediaDeviceCount;

  if (HandleBuffer != NULL) {
    FreePool (HandleBuffer);
  }
}
