/** @file

  Performs USB Key Provisioning for AMT.

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

#include "UsbProvision.h"

VOID                            *mUsbProvData;
VOID                            *mUsbProvDataBackup;
UINTN                           mUsbProvDataSize;
BOOLEAN                         mUsbProvsionDone;
UINT8                           mUsbKeyBus;
UINT8                           mUsbKeyPort;
UINT8                           mUsbKeyDevice;
UINT8                           mUsbKeyFunction;
EFI_GUID                        mFileTypeUuid   = FILE_TYPE_UUID;
EFI_GUID                        mFileTypeUuid2x = FILE_TYPE_UUID_2X;
EFI_GUID                        mFileTypeUuid3  = FILE_TYPE_UUID_3;
EFI_GUID                        mFileTypeUuid4  = FILE_TYPE_UUID_4;

EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *mUsbProvisionFileVolume;
VOID                            *mUsbProvisionFileBuffer;
UINTN                           mUsbCurrentRecordOffset;
BOOLEAN                         mVersion2OrAbove;

/**
  Helper function called as part of the code needed
  to allocate the proper sized buffer for various
  EFI interfaces.

  @param[in] Status               Current status
  @param[in] Buffer               Current allocated buffer, or NULL
  @param[in] BufferSize           Current buffer size needed

  @retval TRUE                    if the buffer was reallocated and the caller should try the
                                  API again.
**/
BOOLEAN
GrowBuffer (
  IN OUT EFI_STATUS   *Status,
  IN OUT VOID         **Buffer,
  IN UINTN            BufferSize
  )
{
  BOOLEAN TryAgain;

  ///
  /// If this is an initial request, buffer will be null with a new buffer size
  ///
  if (!*Buffer && BufferSize) {
    *Status = EFI_BUFFER_TOO_SMALL;
  }
  ///
  /// If the status code is "buffer too small", resize the buffer
  ///
  TryAgain = FALSE;
  if (*Status == EFI_BUFFER_TOO_SMALL) {

    if (*Buffer) {
      FreePool (*Buffer);
    }

    *Buffer = AllocatePool (BufferSize);

    if (*Buffer) {
      TryAgain = TRUE;
    } else {
      *Status = EFI_OUT_OF_RESOURCES;
    }
  }
  ///
  /// If there's an error, free the buffer
  ///
  if (!TryAgain && EFI_ERROR (*Status) && *Buffer) {
    FreePool (*Buffer);
    *Buffer = NULL;
  }

  return TryAgain;
}

/**
  Function gets the file information from an open file descriptor, and stores it
  in a buffer allocated from pool.

  @param[in] FHand                A file handle

  @retval EFI_FILE_INFO           A pointer to a buffer with file information or NULL is returned
**/
EFI_FILE_INFO *
LibFileInfo (
  IN EFI_FILE_HANDLE              FHand
  )
{
  EFI_STATUS                      Status;
  EFI_FILE_INFO                   *Buffer;
  UINTN                           BufferSize;

  ///
  /// Initialize for GrowBuffer loop
  ///
  Buffer      = NULL;
  BufferSize  = SIZE_OF_EFI_FILE_INFO + 200;

  ///
  /// Call the real function
  ///
  while (GrowBuffer (&Status, (VOID **) &Buffer, BufferSize)) {
    Status = FHand->GetInfo (
                      FHand,
                      &gEfiFileInfoGuid,
                      &BufferSize,
                      Buffer
                      );
  }

  return Buffer;
}

/**
  Search device which it is match the device path.

  @param[in] DevicePath           The list of Device patch
  @param[in] DevicePathHeader     Path hoping to be found.

  @retval DevicePath              Can be matched
**/
EFI_DEVICE_PATH_PROTOCOL *
GetDevicePathTypeMatch (
  EFI_DEVICE_PATH_PROTOCOL        *DevicePath,
  EFI_DEVICE_PATH_PROTOCOL        *DevicePathHeader
  )
{
  while (!IsDevicePathEnd (DevicePath)) {
    if (DevicePath->Type == DevicePathHeader->Type && DevicePath->SubType == DevicePathHeader->SubType) {
      return DevicePath;
    } else {
      DevicePath = NextDevicePathNode (DevicePath);
    }
  }

  return NULL;
}

/**
  Get USB device path from the list of devices

  @param[in] DevicePath           The list of Device patch

  @retval DevicePath              Can be matched
**/
USB_DEVICE_PATH *
GetUsbDevicePath (
  EFI_DEVICE_PATH_PROTOCOL        *DevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL  UsbDevicePath = { MESSAGING_DEVICE_PATH, MSG_USB_DP };
  return (USB_DEVICE_PATH *) GetDevicePathTypeMatch (DevicePath, &UsbDevicePath);
}

/**
  Get Media device path from the list of devices

  @param[in] DevicePath           The list of Device patch

  @retval DevicePath              Can be matched
**/
HARDDRIVE_DEVICE_PATH *
GetMediaDevicePath (
  EFI_DEVICE_PATH_PROTOCOL        *DevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL  MediaDevicePath = { MESSAGING_DEVICE_PATH, MEDIA_HARDDRIVE_DP };
  return (HARDDRIVE_DEVICE_PATH *) GetDevicePathTypeMatch (DevicePath, &MediaDevicePath);
}

/**
  Get PCI device path from the list of devices

  @param[in] DevicePath           The list of Device patch

  @retval DevicePath              Can be matched
**/
PCI_DEVICE_PATH *
GetPciDevicePath (
  EFI_DEVICE_PATH_PROTOCOL        *DevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL  PciDevicePath = { HARDWARE_DEVICE_PATH, HW_PCI_DP };
  return (PCI_DEVICE_PATH *) GetDevicePathTypeMatch (DevicePath, &PciDevicePath);
}

#ifndef __GNUC__
#pragma optimize("", off)
#endif

/**
  Read provisioning file and checking if header is valid.

  @param[in] FileHandle           File handle will be checking.

  @retval None
**/
VOID
DoProvision (
  EFI_FILE_HANDLE                 FileHandle
  )
{
  EFI_STATUS          Status;
  EFI_FILE_INFO       *FileInfo;
  UINTN               FileInfoSize;
  FILE_HEADER_RECORD  *FileHeaderRecord;
  DATA_RECORD_HEADER  *DataRecordHeader;
  UINT16              DataRecordEntryCount;
  UINTN               FileHeaderSize;
  UINTN               DataRecordSize;
  DATA_RECORD_ENTRY   *DataRecordEntry;
  UINTN               ConsumedDataRecordSize;
  UINTN               UsbProvisionFileSize;
#ifdef EFI_DEBUG
  UINT16              Padding;
#endif

  FileInfoSize          = 0;
  FileInfo              = NULL;
  FileHeaderRecord      = NULL;
  DataRecordEntryCount  = 0;
  DataRecordSize        = 0;
  DataRecordHeader      = NULL;

  FileInfo              = LibFileInfo (FileHandle);
  if (FileInfo == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Done;
  }

  mUsbProvisionFileBuffer = AllocatePool ((UINTN) FileInfo->FileSize);
  if (mUsbProvisionFileBuffer != NULL) {
    UsbProvisionFileSize  = (UINTN) FileInfo->FileSize;

    Status                = FileHandle->Read (FileHandle, &UsbProvisionFileSize, mUsbProvisionFileBuffer);
    if (EFI_ERROR (Status)) {
      goto Done;
    }
  } else {
    Status = EFI_OUT_OF_RESOURCES;
    goto Done;
  }
  ///
  /// Check if Valid File header record
  ///
  FileHeaderRecord = (FILE_HEADER_RECORD *) mUsbProvisionFileBuffer;
  if (CompareGuid ((EFI_GUID *) &(FileHeaderRecord->FileTypeUUID), &mFileTypeUuid) == TRUE) {
    if (FileHeaderRecord->MajorVersion == MAJOR_VERSION) {
      if (FileHeaderRecord->MinorVersion != MINOR_VERSION) {
        return ;
      }
    } else {
      return ;
    }
  } else if (CompareGuid ((EFI_GUID *) &(FileHeaderRecord->FileTypeUUID), &mFileTypeUuid2x) == TRUE) {
    if (FileHeaderRecord->MajorVersion == MAJOR_VERSION_20) {
      if ((FileHeaderRecord->MinorVersion != MINOR_VERSION_20) && (FileHeaderRecord->MinorVersion != MINOR_VERSION_21)) {
        return ;
      }
    } else {
      return ;
    }
  } else if (CompareGuid ((EFI_GUID *) &(FileHeaderRecord->FileTypeUUID), &mFileTypeUuid3) == TRUE) {
    if (FileHeaderRecord->MajorVersion == MAJOR_VERSION_30) {
      if (FileHeaderRecord->MinorVersion != MINOR_VERSION_30) {
        return ;
      }
    } else {
      return ;
    }
  } else if (CompareGuid ((EFI_GUID *) &(FileHeaderRecord->FileTypeUUID), &mFileTypeUuid4) == TRUE) {
    if (FileHeaderRecord->MajorVersion == MAJOR_VERSION_40) {
      if (FileHeaderRecord->MinorVersion != MINOR_VERSION_40) {
        return ;
      }
    } else {
      return ;
    }

  } else {
    return ;
  }

  if (FileHeaderRecord->MajorVersion >= MAJOR_VERSION_20) {
    mVersion2OrAbove = TRUE;
  } else {
    mVersion2OrAbove = FALSE;
  }
  ///
  /// Check if all records are consumed.
  ///
  if (FileHeaderRecord->DataRecordsConsumed == FileHeaderRecord->DataRecordCount) {
    return ;
  }
  ///
  /// Calculate Data Record size
  ///
  DataRecordSize = (FileHeaderRecord->DataRecordChunkCount) * CHUNK_SIZE;

  ///
  /// Calculate Consumed Data Record size
  ///
  ConsumedDataRecordSize = (FileHeaderRecord->DataRecordsConsumed) * DataRecordSize;

  ///
  /// Calculate File Header Size
  ///
  FileHeaderSize = (FileHeaderRecord->RecordChunkCount) * CHUNK_SIZE;

  ///
  /// Calculate Current Data Record Header Offset
  ///
  mUsbCurrentRecordOffset = FileHeaderSize + ConsumedDataRecordSize;

  ///
  /// Get current Data Record Header pointer
  ///
  DataRecordHeader = (DATA_RECORD_HEADER *) ((UINTN) mUsbProvisionFileBuffer + mUsbCurrentRecordOffset);

  ///
  /// Check if this is a data record
  ///
  if (DataRecordHeader->RecordTypeIdentifier == DATA_RECORD_TYPE_DATA_RECORD) {
    ///
    /// Check the if valid flag is set
    ///
    if ((DataRecordHeader->RecordFlags) & DATA_RECORD_FLAG_VALID) {
      ///
      /// Found a valid Data Record
      ///
      mUsbProvsionDone = TRUE;

      ///
      /// fetch the first entry
      ///
      DataRecordEntry = (DATA_RECORD_ENTRY *) ((UINTN) DataRecordHeader + DataRecordHeader->RecordHeaderByteCount);

#ifdef EFI_DEBUG
      DataRecordEntryCount = 0;
      do {
        switch (DataRecordEntry->ModuleIdentifier) {
        case MODULE_IDENTIFIER_INVALID:
          goto Done;
          break;

        case MODULE_IDENTIFIER_ME_KERNEL:
          DEBUG ((EFI_D_ERROR, "MODULE_IDENTIFIER_ME_KERNEL\n"));
          switch (DataRecordEntry->VariableIdentifier) {
          case VARIABLE_IDENTIFIER_ME_KERNEL_INVALID:
            goto Done;

          case VARIABLE_IDENTIFIER_ME_KERNEL_CURRENT_MEBX_PASSWORD:
            DataRecordEntryCount++;
            DEBUG ((EFI_D_ERROR, "VARIABLE_IDENTIFIER_ME_KERNEL_CURRENT_MEBX_PASSWORD\n"));
            break;

          case VARIABLE_IDENTIFIER_ME_KERNEL_NEW_MEBX_PASSWORD:
            DataRecordEntryCount++;
            DEBUG ((EFI_D_ERROR, "VARIABLE_IDENTIFIER_ME_KERNEL_NEW_MEBX_PASSWORD\n"));
            break;
          } // end of switch VariableIdentifier
          break;

        case MODULE_IDENTIFIER_INTEL_AMT_CM:
          DEBUG ((EFI_D_ERROR, "MODULE_IDENTIFIER_ME_KERNEL\n"));
          switch (DataRecordEntry->VariableIdentifier) {
          case VARIABLE_IDENTIFIER_INTEL_AMT_CM_INVALID:
            goto Done;

          case VARIABLE_IDENTIFIER_INTEL_AMT_CM_PID:
            DataRecordEntryCount++;
            DEBUG ((EFI_D_ERROR, "VARIABLE_IDENTIFIER_INTEL_AMT_CM_PID\n"));
            break;

          case VARIABLE_IDENTIFIER_INTEL_AMT_CM_PPS:
            DataRecordEntryCount++;
            DEBUG ((EFI_D_ERROR, "VARIABLE_IDENTIFIER_INTEL_AMT_CM_PPS\n"));
            break;
          } // end of switch VariableIdentifier
          break;
        } // End of switch ModuleIdentifier
        ///
        /// Move to the next entry
        ///
        ///
        /// Calculate the padding required
        ///
        Padding = (4 - (DataRecordEntry->VariableLenght % 4)) % 4;

        DataRecordEntry = (DATA_RECORD_ENTRY *) ((UINTN) DataRecordEntry + 8 + DataRecordEntry->VariableLenght + Padding);
      } while ((UINTN) DataRecordEntry < ((UINTN) DataRecordHeader + DataRecordSize));
#endif // end of EFI_DEBUG
    } // end of if RecordFlags
  } // end of if RecordTypeIdentifier

Done:
  if (mUsbProvsionDone) {
    DEBUG ((EFI_D_ERROR, "Found Total %x Entries\n", DataRecordEntryCount));
    DEBUG ((EFI_D_ERROR, "Found Consumed %x Entries\n", FileHeaderRecord->DataRecordsConsumed));
    mUsbProvData      = (VOID *) DataRecordHeader;
    mUsbProvDataSize  = DataRecordSize;
    DEBUG ((EFI_D_ERROR, "Total Size is %x\n", mUsbProvDataSize));
    mUsbProvDataBackup = AllocateZeroPool ((UINTN) DataRecordSize);
  }

  return ;
}

#ifndef __GNUC__
#pragma optimize("", on)
#endif

/**
  Remove Consumed Data Record in USB Key Provisioning Data File

  @param[in] None.

  @retval None
**/
VOID
UsbConsumedDataRecordRemove (
  VOID
  )
{
  EFI_STATUS          Status;
  EFI_FILE_HANDLE     UsbRootFileHandle;
  EFI_FILE_HANDLE     UsbProvisionFileHandle;
  FILE_HEADER_RECORD  *FileHeaderRecord;
  UINTN               FileSize;
  DATA_RECORD_HEADER  *DataRecordHeader;

  if (mUsbProvsionDone) {
    DataRecordHeader  = (DATA_RECORD_HEADER *) mUsbProvData;
    FileHeaderRecord  = (FILE_HEADER_RECORD *) mUsbProvisionFileBuffer;

    ///
    /// If version 2 or above and DONT_CONSUME_RECORDS is set, don't remove the record
    ///
    if (mVersion2OrAbove && ((FileHeaderRecord->FileFlags) & FILE_FLAGS_DONT_CONSUME_RECORDS)) {
      return ;
    }
    ///
    /// check if file is present in the root dir
    ///
    Status = mUsbProvisionFileVolume->OpenVolume (mUsbProvisionFileVolume, &UsbRootFileHandle);
    ASSERT_EFI_ERROR (Status);

    Status = UsbRootFileHandle->Open (
                                  UsbRootFileHandle,
                                  &UsbProvisionFileHandle,
                                  USB_PROVISIONING_DATA_FILE_NAME,
                                  EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE,
                                  0
                                  );
    if (!EFI_ERROR (Status)) {
      (FileHeaderRecord->DataRecordsConsumed)++;
      ///
      /// Zero-out data record body and set valid flag to invalid
      ///
      ZeroMem (
        (VOID *) ((UINTN) mUsbProvData + DataRecordHeader->RecordHeaderByteCount),
        mUsbProvDataSize - DataRecordHeader->RecordHeaderByteCount
        );
      DataRecordHeader->RecordFlags &= ~DATA_RECORD_FLAG_VALID;

      UsbProvisionFileHandle->SetPosition (
                                UsbProvisionFileHandle,
                                (UINT64) USB_DATA_RECORD_CONSUMED_OFFSET
                                );

      FileSize = sizeof (FileHeaderRecord->DataRecordsConsumed);
      UsbProvisionFileHandle->Write (
                                UsbProvisionFileHandle,
                                &FileSize,
                                &FileHeaderRecord->DataRecordsConsumed
                                );

      UsbProvisionFileHandle->SetPosition (
                                UsbProvisionFileHandle,
                                (UINT64) mUsbCurrentRecordOffset
                                );
      FileSize = mUsbProvDataSize;
      UsbProvisionFileHandle->Write (
                                UsbProvisionFileHandle,
                                &FileSize,
                                mUsbProvData
                                );
      UsbProvisionFileHandle->Close (UsbProvisionFileHandle);
    }

    UsbRootFileHandle->Close (UsbRootFileHandle);
  }

  return ;
}

/**
  Restore Consumed Data Record in USB Key Provisioning Data File

  @param[in] None.

  @retval None
**/
VOID
UsbConsumedDataRecordRestore (
  VOID
  )
{
  EFI_STATUS          Status;
  EFI_FILE_HANDLE     UsbRootFileHandle;
  EFI_FILE_HANDLE     UsbProvisionFileHandle;
  FILE_HEADER_RECORD  *FileHeaderRecord;
  UINTN               FileSize;
  DATA_RECORD_HEADER  *DataRecordHeader;

  if (mUsbProvsionDone) {
    DataRecordHeader  = (DATA_RECORD_HEADER *) mUsbProvDataBackup;
    FileHeaderRecord  = (FILE_HEADER_RECORD *) mUsbProvisionFileBuffer;

    ///
    /// If version 2 or above and DONT_CONSUME_RECORDS is set, don't need to restore the record
    /// If DataRecordHeader->RecordFlags, DATA_RECORD_FLAG_VALID is not set, don't need to restore the record
    ///
    if ((mVersion2OrAbove && ((FileHeaderRecord->FileFlags) & FILE_FLAGS_DONT_CONSUME_RECORDS)) ||
        (((DataRecordHeader->RecordFlags) & DATA_RECORD_FLAG_VALID) == 0)
        ) {
      FreePool (mUsbProvisionFileBuffer);
      FreePool (mUsbProvDataBackup);
      return ;
    }
    ///
    /// check if file is present in the root dir
    ///
    Status = mUsbProvisionFileVolume->OpenVolume (mUsbProvisionFileVolume, &UsbRootFileHandle);
    ASSERT_EFI_ERROR (Status);

    Status = UsbRootFileHandle->Open (
                                  UsbRootFileHandle,
                                  &UsbProvisionFileHandle,
                                  USB_PROVISIONING_DATA_FILE_NAME,
                                  EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE,
                                  0
                                  );
    if (!EFI_ERROR (Status)) {
      ///
      /// Restore DataRecordsConsumed number
      ///
      (FileHeaderRecord->DataRecordsConsumed)--;
      UsbProvisionFileHandle->SetPosition (
                                UsbProvisionFileHandle,
                                (UINT64) USB_DATA_RECORD_CONSUMED_OFFSET
                                );

      FileSize = sizeof (FileHeaderRecord->DataRecordsConsumed);
      UsbProvisionFileHandle->Write (
                                UsbProvisionFileHandle,
                                &FileSize,
                                &FileHeaderRecord->DataRecordsConsumed
                                );

      UsbProvisionFileHandle->SetPosition (
                                UsbProvisionFileHandle,
                                (UINT64) mUsbCurrentRecordOffset
                                );
      FileSize = mUsbProvDataSize;
      UsbProvisionFileHandle->Write (
                                UsbProvisionFileHandle,
                                &FileSize,
                                mUsbProvDataBackup
                                );
      UsbProvisionFileHandle->Close (UsbProvisionFileHandle);
    }

    UsbRootFileHandle->Close (UsbRootFileHandle);
    FreePool (mUsbProvisionFileBuffer);
    FreePool (mUsbProvDataBackup);
  }

  return ;
}

/**
  Check USB Key Provisioning Data

  @param[in] None.

  @retval None
**/
VOID
UsbKeyProvisioning (
  VOID
  )
{
  EFI_STATUS                      Status;
  UINTN                           NumberFileSystemHandles;
  EFI_HANDLE                      *FileSystemHandles;
  UINTN                           Index;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *PciRootBridgeIo;
  EFI_DEVICE_PATH_PROTOCOL        *DevicePath;
  USB_DEVICE_PATH                 *UsbDevicePath;
  PCI_DEVICE_PATH                 *PciDevicePath;
  PCI_DEVICE_PATH                 *ParentPciDevicePath;
  PCI_DEVICE_PATH                 *NextPciDevicePath;
  EFI_DEVICE_PATH_PROTOCOL        *NextDevicePath;
  EFI_FILE_HANDLE                 UsbRootFileHandle;
  EFI_FILE_HANDLE                 UsbProvisionFileHandle;
  UINT8                           UsbBusNumber;
  UINT64                          PciAddress;

  ///
  /// This flag will be set only when some meaningfull data is read from USB key
  ///
  mUsbProvsionDone = FALSE;

  ///
  /// Parse Fixed Disk Devices.
  ///
  gBS->LocateHandleBuffer (
        ByProtocol,
        &gEfiSimpleFileSystemProtocolGuid,
        NULL,
        &NumberFileSystemHandles,
        &FileSystemHandles
        );

  for (Index = 0; Index < NumberFileSystemHandles; Index++) {
    DevicePath = DevicePathFromHandle (FileSystemHandles[Index]);
    if (DevicePath == NULL) {
      continue;
    }

    UsbDevicePath = GetUsbDevicePath (DevicePath);
    if (UsbDevicePath == NULL) {
      continue;
    }

    Status = gBS->HandleProtocol (
                    FileSystemHandles[Index],
                    &gEfiSimpleFileSystemProtocolGuid,
                    (VOID **) &mUsbProvisionFileVolume
                    );
    ASSERT_EFI_ERROR (Status);

    ///
    /// check if file is present in the root dir
    ///
    Status = mUsbProvisionFileVolume->OpenVolume (mUsbProvisionFileVolume, &UsbRootFileHandle);
    ASSERT_EFI_ERROR (Status);

    Status = UsbRootFileHandle->Open (
                                  UsbRootFileHandle,
                                  &UsbProvisionFileHandle,
                                  USB_PROVISIONING_DATA_FILE_NAME,
                                  EFI_FILE_MODE_READ,
                                  0
                                  );

    if (!EFI_ERROR (Status)) {
      ///
      /// USB Key Provisioning Data File is exist.
      ///
      ///
      /// Locate PCI IO protocol for PCI registers read
      ///
      Status = gBS->LocateProtocol (
                      &gEfiPciRootBridgeIoProtocolGuid,
                      NULL,
                      (VOID **) &PciRootBridgeIo
                      );
      ASSERT_EFI_ERROR (Status);

      ///
      /// Get bus number by analysis Device Path and PCI Register - PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET
      ///
      PciDevicePath = GetPciDevicePath (DevicePath);
      if (PciDevicePath == NULL) {
        break;
      }

      ParentPciDevicePath = NULL;
      UsbBusNumber        = 0;
      while (TRUE) {
        NextDevicePath    = (EFI_DEVICE_PATH_PROTOCOL *) PciDevicePath;
        NextDevicePath    = NextDevicePathNode (NextDevicePath);
        NextPciDevicePath = GetPciDevicePath (NextDevicePath);
        if (NextPciDevicePath == NULL) {
          break;
        }
        ///
        /// If found next PCI Device Path, current Device Path is a P2P bridge
        ///
        ParentPciDevicePath = PciDevicePath;
        PciDevicePath       = NextPciDevicePath;
        ///
        /// Read Bus number
        ///
        PciAddress = EFI_PCI_ADDRESS (
                      UsbBusNumber,
                      ParentPciDevicePath->Device,
                      ParentPciDevicePath->Function,
                      PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET
                      );
        Status = PciRootBridgeIo->Pci.Read (
                                        PciRootBridgeIo,
                                        EfiPciWidthUint8,
                                        PciAddress,
                                        1,
                                        &UsbBusNumber
                                        );
        ASSERT_EFI_ERROR (Status);
      }
      ///
      /// Need a way to get this information
      ///
      mUsbKeyPort     = UsbDevicePath->InterfaceNumber;
      mUsbKeyBus      = UsbBusNumber;
      mUsbKeyDevice   = PciDevicePath->Device;
      mUsbKeyFunction = PciDevicePath->Function;
      DoProvision (UsbProvisionFileHandle);
      UsbProvisionFileHandle->Close (UsbProvisionFileHandle);
    }

    UsbRootFileHandle->Close (UsbRootFileHandle);
    ///
    /// Break if found a valid provisioning file
    ///
    if (mUsbProvsionDone == TRUE) {
      break;
    }
  } // End of For NumberFileSystem Loop

  if (NumberFileSystemHandles != 0) {
    FreePool (FileSystemHandles);
  }
}
