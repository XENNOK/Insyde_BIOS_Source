/** @file
  The implementation of USB mass storage class device driver.
  The command set supported is "USB Mass Storage Specification
  for Bootability".

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

#include "UsbMassImpl.h"

#include <PostCode.h>

UINTN mUsbMscInfo  = EFI_D_INFO;
UINTN mUsbMscError = EFI_D_ERROR;

STATIC EFI_GUID                         mUsbMassTransportProtocolGuid = USB_MASS_TRANSPORT_PROTOCOL_GUID;

/**

  Usb Get Maximum LUNs for multiple LUN device

  @param  UsbIo                 EFI_USB_IO_PROTOCOL
  @param  NumOfLun              Pointer to MaximumLuns for return

  @retval EFI_INVALID_PARAMETER Parameter is error
  @retval EFI_SUCCESS           Success
  @retval EFI_UNSUPPORTED       Device has no multiple LUN supported

**/  
EFI_STATUS
UsbMassGetMaximumLuns (
  IN  EFI_USB_IO_PROTOCOL     *UsbIo,
  IN  UINT8                   InterfaceNumber,
  OUT UINT8                   *MaximumLuns
  )
{
  EFI_USB_DEVICE_REQUEST  DevReq;
  UINT32                  RetStatus;

  if (UsbIo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem (&DevReq, sizeof (EFI_USB_DEVICE_REQUEST));

  DevReq.RequestType  = USB_DEV_SET_MAX_LUN_REQ_TYPE;
  DevReq.Request      = USB_DEV_GET_MAX_LUN;
  DevReq.Index        = InterfaceNumber;
  DevReq.Length       = 1;

  return UsbIo->UsbControlTransfer (
                  UsbIo,
                  &DevReq,
                  EfiUsbDataIn,
                  1000,
                  MaximumLuns,
                  1,
                  &RetStatus
                  );
}

/**

  Retrieve the media parameters such as disk gemotric for the 
  device's BLOCK IO protocol.

  @param  UsbMass               The USB mass storage device

  @retval EFI_SUCCESS           The media parameters is updated successfully.
  @retval Others                Failed to get the media parameters.

**/
EFI_STATUS
UsbMassInitMedia (
  IN USB_MASS_DEVICE          *UsbMass
  )
{
  EFI_BLOCK_IO_MEDIA          *Media;
  EFI_STATUS                  Status;
  UINTN                       Index;
  UINTN                       Retry;
  BOOLEAN                     InquirySuccessed;
  BOOLEAN                     MediaPresent;
  
  Status           = EFI_SUCCESS;
  InquirySuccessed = FALSE;
  MediaPresent     = TRUE;
  Media            = &UsbMass->BlockIoMedia;
  //
  // Initialize the MediaPrsent/ReadOnly and others to the default.
  // We are not forced to get it right at this time, check UEFI2.0
  // spec for more information:
  //
  // MediaPresent: This field shows the media present status as
  //               of the most recent ReadBlocks or WriteBlocks call.
  //
  // ReadOnly    : This field shows the read-only status as of the
  //               recent WriteBlocks call.
  //
  // but remember to update MediaId/MediaPresent/ReadOnly status
  // after ReadBlocks and WriteBlocks
  //
  Media->MediaPresent     = FALSE;
  Media->LogicalPartition = FALSE;
  Media->ReadOnly         = FALSE;
  Media->WriteCaching     = FALSE;
  Media->IoAlign          = 0;
  //
  // Use the loop to make the device get ready(if available)
  //
  for (Index = 0; Index < USB_BOOT_INIT_MEDIA_RETRY; Index++) {
    //
    // Make Inquiry as first UFI command after bunch of control transfer commands
    //
    if (!InquirySuccessed) {
      Status = UsbBootInquiry (UsbMass);
      if (!EFI_ERROR (Status)) {
        InquirySuccessed = TRUE;
      } else {
        DEBUG ((mUsbMscError, "UsbBootGetParams: UsbBootInquiry (%r)\n", Status));
        if (Status == EFI_NOT_FOUND || Status == EFI_NO_RESPONSE) {
          //
          // Device been detached or no response
          //
          break;
        }
      }
    }
    //
    // Use Test Unit to detect the device ready
    //
    Status = UsbBootIsUnitReady (UsbMass, USB_BOOT_TEST_UNIT_TIMEOUT);
    if (EFI_ERROR (Status)) {
      if (Status == EFI_NOT_FOUND || Status == EFI_NO_RESPONSE) {
        //
        // Device been detached or no response
        //
        break;
      } else if (Status == EFI_NO_MEDIA) {
        //
        // Clear MediaPresent to ignore unnecessary ReadCapacity
        //
        MediaPresent = FALSE;
      } else {
        gBS->Stall (USB_BOOT_UNIT_READY_STALL * (Index + 1));
        continue;
      }
    }
    //
    // Don't use the Removable bit in inquirydata to test whether the media 
    // is removable because many flash disks wrongly set this bit.
    //
    if ((UsbMass->Pdt == USB_PDT_CDROM) || (UsbMass->Pdt == USB_PDT_OPTICAL)) {
      //
      // CD-Rom or Optical device
      //
      UsbMass->OpticalStorage = 1;
      //
      // Default value 2048 Bytes, in case no media present at first time
      //
      Media->BlockSize        = 0x0800;
    } else {
      //
      // Specific defect device workaround
      //
      if (CompareMem(UsbMass->InquiryData.ProductID, "USB CYBER DISK", 14) == 0) {
        //
        // Waiting for media ready within 2 seconds
        //
        for (Retry = 0; Retry < 20; Retry ++) {
          Status = UsbBootIsUnitReady (UsbMass, USB_BOOT_GENERAL_CMD_TIMEOUT);
          if (Status != EFI_NO_MEDIA) break;
          gBS->Stall (100 * USB_MASS_STALL_1_MS);
        }
      }
      if (Media->RemovableMedia && 
          CompareMem(UsbMass->InquiryData.VendorID, "IOMEGA", 6) == 0 && 
          CompareMem(UsbMass->InquiryData.ProductID, "ZIP", 3) == 0) {
        //
        // Use START_UNIT command if it is IOMEGA ZIP
        //
        Status = UsbBootStartUnit (UsbMass);
        if (EFI_ERROR (Status)) {
          break;
        }
      }
    }
    if (MediaPresent) {
      //
      // Use ReadCapacity to setup media related parameters 
      //
      Status = UsbBootReadCapacity (UsbMass);
    }
    if (Status != EFI_MEDIA_CHANGED &&
        Status != EFI_NOT_READY &&
        Status != EFI_TIMEOUT &&
        InquirySuccessed) {
      break;
    }
  }
  return Status;
}

/**

  Reset the block device. ExtendedVerification is ignored for this.

  @param  This                  The BLOCK IO protocol
  @param  ExtendedVerification  Whether to execute extended verfication.

  @retval EFI_SUCCESS           The device is successfully resetted.
  @retval Others                Failed to reset the device.

**/
EFI_STATUS
EFIAPI
UsbMassReset (
  IN EFI_BLOCK_IO_PROTOCOL    *This,
  IN BOOLEAN                  ExtendedVerification
  )
{
  USB_MASS_DEVICE *UsbMass;
  EFI_STATUS      Status;
  //
  // RaiseTPL to preventing the background thread releases the UsbMass through hot-plug
  //
  EFI_TPL OldTPL = gBS->RaiseTPL (TPL_NOTIFY);

  UsbMass = USB_MASS_DEVICE_FROM_BLOCKIO (This);
  Status = UsbMass->Transport->Reset (UsbMass->Context, ExtendedVerification);
  //
  // RestoreTPL back to original TPL
  //
  gBS->RestoreTPL(OldTPL);
  return Status;
}

/**

  Read some blocks of data from the block device.

  @param  This                  The Block IO protocol 
  @param  MediaId               The media's ID of the device for current request
  @param  Lba                   The start block number
  @param  BufferSize            The size of buffer to read data in
  @param  Buffer                The buffer to read data to

  @retval EFI_SUCCESS           The data is successfully read
  @retval EFI_NO_MEDIA          Media isn't present
  @retval EFI_MEDIA_CHANGED     The device media has been changed, that is, MediaId changed
  @retval EFI_INVALID_PARAMETER Some parameters are invalid, such as Buffer is NULL.
  @retval EFI_BAD_BUFFER_SIZE   The buffer size isn't a multiple of media's block size, 
                                or overflow the last block number.
**/
EFI_STATUS
EFIAPI
UsbMassReadBlocks (
  IN EFI_BLOCK_IO_PROTOCOL    *This,
  IN UINT32                   MediaId,
  IN EFI_LBA                  Lba,
  IN UINTN                    BufferSize,
  OUT VOID                    *Buffer
  )
{
  USB_MASS_DEVICE     *UsbMass;
  EFI_BLOCK_IO_MEDIA  *Media;
  EFI_STATUS          Status;
  UINTN               TotalBlock;
  //
  // RaiseTPL to preventing the background thread releases the UsbMass through hot-plug
  //
  EFI_TPL OldTPL = gBS->RaiseTPL (TPL_NOTIFY);

  UsbMass = USB_MASS_DEVICE_FROM_BLOCKIO (This);
  Media   = &UsbMass->BlockIoMedia;

  //
  // Validate MediaId 
  //
  if(MediaId != Media->MediaId) {
    Status = EFI_MEDIA_CHANGED;
    goto ON_EXIT;
  }

  if (BufferSize == 0) {
    Status = EFI_SUCCESS;
    goto ON_EXIT;
  }
  //
  // Validate the parameters
  //
  if (Buffer == NULL) {
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }
  //
  // If it is a remoable media, such as CD-Rom or Usb-Floppy,
  // if, need to detect the media before each rw, while Usb-Flash
  // needn't. However, it's hard to identify Usb-Floppy between 
  // Usb-Flash by now, so detect media every time.
  // 
  Status = UsbBootDetectMedia (UsbMass);
  if (EFI_ERROR (Status)) {
    DEBUG ((mUsbMscError, "UsbMassReadBlocks: UsbBootDetectMedia (%r)\n", Status));
    goto ON_EXIT;
  } 
  
  //
  // Make sure BlockSize and LBA is consistent with BufferSize
  //
  if ((BufferSize % Media->BlockSize) != 0) {
    Status = EFI_BAD_BUFFER_SIZE;
    goto ON_EXIT;
  }

  TotalBlock = BufferSize / Media->BlockSize;

  if (Lba + TotalBlock - 1 > Media->LastBlock) {
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }
  
  Status = UsbBootReadBlocks (UsbMass, (UINT32) Lba, TotalBlock, Buffer);
  if (EFI_ERROR (Status) && Status != EFI_INVALID_PARAMETER) {
    DEBUG ((mUsbMscError, "UsbMassReadBlocks: UsbBootReadBlocks (%r) -> Reset\n", Status));
    UsbMassReset (This, TRUE);
  }
ON_EXIT:
  //
  // RestoreTPL back to original TPL
  //
  gBS->RestoreTPL(OldTPL);
  return Status;
}

/**

  Write some blocks of data to the block device.

  @param  This                  The Block IO protocol 
  @param  MediaId               The media's ID of the device for current request
  @param  Lba                   The start block number
  @param  BufferSize            The size of buffer to write data to
  @param  Buffer                The buffer to write data to

  @retval EFI_SUCCESS           The data is successfully written
  @retval EFI_NO_MEDIA          Media isn't present
  @retval EFI_MEDIA_CHANGED     The device media has been changed, that is, MediaId changed
  @retval EFI_INVALID_PARAMETER Some parameters are invalid, such as Buffer is NULL.
  @retval EFI_BAD_BUFFER_SIZE   The buffer size isn't a multiple of media's block size, 

**/
EFI_STATUS
EFIAPI
UsbMassWriteBlocks (
  IN EFI_BLOCK_IO_PROTOCOL    *This,
  IN UINT32                   MediaId,
  IN EFI_LBA                  Lba,
  IN UINTN                    BufferSize,
  IN VOID                     *Buffer
  )
{
  USB_MASS_DEVICE     *UsbMass;
  EFI_BLOCK_IO_MEDIA  *Media;
  EFI_STATUS          Status;
  UINTN               TotalBlock;
  //
  // RaiseTPL to preventing the background thread releases the UsbMass through hot-plug
  //
  EFI_TPL OldTPL = gBS->RaiseTPL (TPL_NOTIFY);

  UsbMass = USB_MASS_DEVICE_FROM_BLOCKIO (This);
  Media   = &UsbMass->BlockIoMedia;

  //
  // First, validate the parameters
  //
  if ((Buffer == NULL) || (BufferSize == 0)) {
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }
  
  //
  // If it is a remoable media, such as CD-Rom or Usb-Floppy,
  // if, need to detect the media before each rw, while Usb-Flash
  // needn't. However, it's hard to identify Usb-Floppy between 
  // Usb-Flash by now, so detect media every time.
  // 
  Status = UsbBootDetectMedia (UsbMass);
  if (EFI_ERROR (Status)) {
    DEBUG ((mUsbMscError, "UsbMassWriteBlocks: UsbBootDetectMedia (%r)\n", Status));
    goto ON_EXIT;
  }
  
  //
  // Make sure BlockSize and LBA is consistent with BufferSize
  //
  if ((BufferSize % Media->BlockSize) != 0) {
    Status = EFI_BAD_BUFFER_SIZE;
    goto ON_EXIT;
  }

  TotalBlock = BufferSize / Media->BlockSize;

  if (Lba + TotalBlock - 1 > Media->LastBlock) {
    Status = EFI_BAD_BUFFER_SIZE;
    goto ON_EXIT;
  }
  
  //
  // Try to write the data even the device is marked as ReadOnly,
  // and clear the status should the write succeed.
  //
  Status = UsbBootWriteBlocks (UsbMass, (UINT32) Lba, TotalBlock, Buffer);
  if (EFI_ERROR (Status)) {
    DEBUG ((mUsbMscError, "UsbMassWriteBlocks: UsbBootWriteBlocks (%r) -> Reset\n", Status));
    UsbMassReset (This, TRUE);
  }
ON_EXIT:
  //
  // RestoreTPL back to original TPL
  //
  gBS->RestoreTPL(OldTPL);
  return Status;
}

/**

  Flush the cached writes to disks. USB mass storage device doesn't
  support write cache, so return EFI_SUCCESS directly.

  @param  This                  The BLOCK IO protocol

  @retval EFI_SUCCESS           Always returns success

**/
EFI_STATUS
EFIAPI
UsbMassFlushBlocks (
  IN EFI_BLOCK_IO_PROTOCOL  *This
  )
{
  return EFI_SUCCESS;
}

/**

  Provides inquiry information for the controller type.
  This function is used to get inquiry data.  Data format
  of Identify data is defined by the Interface GUID.

  @param  This                  Pointer to the EFI_DISK_INFO_PROTOCOL instance.
  @param  InquiryData           Pointer to a buffer for the inquiry data.
  @param  InquiryDataSize       Pointer to the value for the inquiry data size.

  @retval EFI_SUCCESS           The command was accepted without any errors.
  @retval EFI_NOT_FOUND         Device does not support this data class 
  @retval EFI_DEVICE_ERROR      Error reading InquiryData from device 
  @retval EFI_BUFFER_TOO_SMALL  InquiryDataSize not big enough 

**/
EFI_STATUS
EFIAPI
UsbMassInquiry (
  IN     EFI_DISK_INFO_PROTOCOL   *This,
  IN OUT VOID                     *InquiryData,
  IN OUT UINT32                   *InquiryDataSize
  )
{
  EFI_STATUS        Status;
  USB_MASS_DEVICE   *UsbMass;

  UsbMass  = USB_MASS_DEVICE_FROM_DISK_INFO (This);

  Status = EFI_BUFFER_TOO_SMALL;
  if (*InquiryDataSize >= sizeof (USB_BOOT_INQUIRY_DATA)) {
    Status = EFI_SUCCESS;
    CopyMem (InquiryData, &UsbMass->InquiryData, sizeof (USB_BOOT_INQUIRY_DATA));
  }
  *InquiryDataSize = sizeof (USB_BOOT_INQUIRY_DATA);
  return Status;
}

/**

  Provides identify information for the controller type.
  This function is used to get identify data.  Data format
  of Identify data is defined by the Interface GUID.

  @param  This                  Pointer to the EFI_DISK_INFO_PROTOCOL instance.
  @param  IdentifyData          Pointer to a buffer for the identify data.
  @param  IdentifyDataSize      Pointer to the value for the identify data size.

  @retval EFI_SUCCESS           The command was accepted without any errors.
  @retval EFI_NOT_FOUND         Device does not support this data class 
  @retval EFI_DEVICE_ERROR      Error reading IdentifyData from device 
  @retval EFI_BUFFER_TOO_SMALL  IdentifyDataSize not big enough 

**/
EFI_STATUS
EFIAPI
UsbMassIdentify (
  IN     EFI_DISK_INFO_PROTOCOL   *This,
  IN OUT VOID                     *IdentifyData,
  IN OUT UINT32                   *IdentifyDataSize
  )
{
  return EFI_NOT_FOUND;
}

/**

  Provides sense data information for the controller type.
  This function is used to get sense data. 
  Data format of Sense data is defined by the Interface GUID.

  @param  This                  Pointer to the EFI_DISK_INFO_PROTOCOL instance.
  @param  SenseData             Pointer to the SenseData.
  @param  SenseDataSize         Size of SenseData in bytes.
  @param  SenseDataNumber       Pointer to the value for the sense data size.

  @retval EFI_SUCCESS           The command was accepted without any errors.
  @retval EFI_NOT_FOUND         Device does not support this data class.
  @retval EFI_DEVICE_ERROR      Error reading SenseData from device.
  @retval EFI_BUFFER_TOO_SMALL  SenseDataSize not big enough.

**/
EFI_STATUS
EFIAPI
UsbMassSenseData (
  IN     EFI_DISK_INFO_PROTOCOL   *This,
  IN OUT VOID                     *SenseData,
  IN OUT UINT32                   *SenseDataSize,
  OUT    UINT8                    *SenseDataNumber
  )
{
  return EFI_NOT_FOUND;
}

/**

  This function is used to get controller information.

  @param  This                  Pointer to the EFI_DISK_INFO_PROTOCOL instance. 
  @param  IdeChannel            Pointer to the Ide Channel number.  Primary or secondary.
  @param  IdeDevice             Pointer to the Ide Device number.  Master or slave.

  @retval EFI_SUCCESS           IdeChannel and IdeDevice are valid.
  @retval EFI_UNSUPPORTED       This is not an IDE device.

**/
EFI_STATUS
EFIAPI
UsbMassWhichIde (
  IN  EFI_DISK_INFO_PROTOCOL   *This,
  OUT UINT32                   *IdeChannel,
  OUT UINT32                   *IdeDevice
  )
{
  return EFI_UNSUPPORTED;
}

/**

  Check whether the controller is a supported USB mass storage.

  @param  This                  The USB mass driver's driver binding.
  @param  Controller            The device to test against.
  @param  RemainingDevicePath   The remaining device path

  @retval EFI_SUCCESS           This device is a supported USB mass storage.
  @retval EFI_UNSUPPORTED       The device isn't supported
  @retval Others                Some error happened.

**/
EFI_STATUS
EFIAPI
UsbMassDriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   Controller,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath
  )
{
  EFI_USB_IO_PROTOCOL           *UsbIo;
  EFI_USB_INTERFACE_DESCRIPTOR  Interface;
  EFI_STATUS                    Status;
  USB_MASS_TRANSPORT_PROTOCOL   *Transport;
  UINTN                         Index;
  UINTN                         HandleCount;
  EFI_HANDLE                    *HandleBuffer;
  EFI_DEV_PATH_PTR              Node;
  EFI_DEVICE_PATH_PROTOCOL      *DevicePath;
  EFI_USB_CORE_PROTOCOL         *UsbCore;
  EFI_USB_DEVICE_DESCRIPTOR     DeviceDescriptor;
  //
  // Check Device Path
  //
  if (RemainingDevicePath != NULL) {
    Node.DevPath = RemainingDevicePath;
    if ((Node.DevPath->Type != MESSAGING_DEVICE_PATH ||
         Node.DevPath->SubType != MSG_DEVICE_LOGICAL_UNIT_DP ||
         DevicePathNodeLength(Node.DevPath) != sizeof(DEVICE_LOGICAL_UNIT_DEVICE_PATH)) &&
        (Node.DevPath->Type != MEDIA_DEVICE_PATH ||
         Node.DevPath->SubType != MEDIA_HARDDRIVE_DP ||
         DevicePathNodeLength(Node.DevPath) != sizeof(HARDDRIVE_DEVICE_PATH))) {
      return EFI_UNSUPPORTED;
    }
  }
  //
  // Check whether the controlelr support USB_IO
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiUsbIoProtocolGuid,
                  (VOID **)&UsbIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Get the device path for CheckIgnoredDevice
  //
  Status = gBS->HandleProtocol (
                  Controller,
                  &gEfiDevicePathProtocolGuid,
                  (VOID **)&DevicePath
                  );
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }
  //
  // Setup the data for UsbLegacy
  //
  Status = gBS->LocateProtocol (
                  &gEfiUsbCoreProtocolGuid,
                  NULL,
                  (VOID **)&UsbCore
                  );
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }
#if (KEEP_USBIO_FOR_IGNORED_DEVICE == 1)
  //
  // Filter out the USB devices which in the UsbIgnoreDevice list
  //
  Status = UsbCore->CheckIgnoredDevice(DevicePath, UsbIo);
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }
#endif
  //
  // Get the interface to check the USB class and find a transport
  // protocol handler.
  //
  Status = UsbIo->UsbGetInterfaceDescriptor (UsbIo, &Interface);
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }

  Status = EFI_UNSUPPORTED;

  if (Interface.InterfaceClass != USB_MASS_STORE_CLASS && 
      Interface.InterfaceClass != USB_VENDOR_SPECIFIC_CLASS) {
    goto ON_EXIT;
  }
  //
  // Get VID and PID
  //
  Status = UsbIo->UsbGetDeviceDescriptor (UsbIo, &DeviceDescriptor);
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }
  //
  // Checks vendor-specific devices
  //
  if (Interface.InterfaceClass == USB_VENDOR_SPECIFIC_CLASS) {
    Status = UsbVendorSpecificDevice(
               UsbIo,
               DeviceDescriptor.IdVendor,
               DeviceDescriptor.IdProduct,
               &Interface.InterfaceProtocol
               );
    if (EFI_ERROR (Status)) {
      goto ON_EXIT;
    }
  }
  //
  // Locates UsbMassTransport Protocol
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &mUsbMassTransportProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR(Status)) {
    goto ON_EXIT;
  }
  Status = EFI_UNSUPPORTED;
  for (Index = 0; Index < HandleCount; Index++) {
    if (EFI_ERROR (gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &mUsbMassTransportProtocolGuid,
                    (VOID **)&Transport
                    ))) {
      continue;
    }
    if (Interface.InterfaceProtocol == Transport->Protocol) {
      Status = EFI_SUCCESS;
      break;
    }
  }
  gBS->FreePool (HandleBuffer);
  
ON_EXIT:
  gBS->CloseProtocol (
        Controller,
        &gEfiUsbIoProtocolGuid,
        This->DriverBindingHandle,
        Controller
        );

  return Status;
}

/**

  Start the USB mass storage device on the controller. It will 
  install a BLOCK_IO protocol on the device if everything is OK.

  @param  This                  The USB mass storage driver binding.
  @param  Controller            The USB mass storage device to start on
  @param  RemainingDevicePath   The remaining device path.

  @retval EFI_SUCCESS           The driver has started on the device.
  @retval EFI_OUT_OF_RESOURCES  Failed to allocate memory
  @retval Others                Failed to start the driver on the device.

**/
EFI_STATUS
EFIAPI
UsbMassDriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   Controller,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath
  )
{
  EFI_USB_IO_PROTOCOL           *UsbIo;
  EFI_USB_INTERFACE_DESCRIPTOR  Interface;
  USB_MASS_DEVICE               *UsbMass;
  USB_MASS_TRANSPORT_PROTOCOL   *Transport;
  VOID                          *Context;
  EFI_STATUS                    Status;
  UINTN                         Index;
  UINTN                         HandleCount;
  EFI_HANDLE                    *HandleBuffer;
  EFI_USB_CORE_PROTOCOL         *UsbCore;
  USB_DEVICE                    UsbDevice;
  EFI_USB_DEVICE_DESCRIPTOR     DeviceDescriptor;
  EFI_DEVICE_PATH_PROTOCOL      *DevicePath;
  UINT8                         MaximumLuns;
  UINT8                         NewDevicePath[sizeof(DEVICE_LOGICAL_UNIT_DEVICE_PATH) + END_DEVICE_PATH_LENGTH];
  DEVICE_LOGICAL_UNIT_DEVICE_PATH *LunDevicePath;
  EFI_TPL                       OldTPL;
  EFI_USB_LEGACY_PLATFORM_PROTOCOL *LegacyBiosPlatform;
  USB_LEGACY_MODIFIERS          UsbLegacyPolicy;
  EFI_DEVICE_PATH_PROTOCOL      *TmpDevicePath;
  EFI_HANDLE                    Handle;
  EFI_PCI_IO_PROTOCOL           *PciIo;
  UINT16                        HcLocation;
  UINTN                         Seg;
  UINTN                         Bus;
  UINTN                         Dev;
  UINTN                         Fun;

  POST_CODE (BDS_CONNECT_USB_DEVICE);

  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiUsbIoProtocolGuid,
                  (VOID **)&UsbIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );

  if (EFI_ERROR (Status)) {
    return Status;
  }
  UsbMass = NULL;
  OldTPL  = 0;
  //
  // Locate the UsbCore for UsbLegacy's InsertUsbDevice
  //
  UsbCore = NULL;
  gBS->LocateProtocol (
         &gEfiUsbCoreProtocolGuid,
         NULL,
         (VOID **)&UsbCore
         );
  if (UsbCore && UsbCore->CheckDeviceDetached(UsbIo) == EFI_SUCCESS) {
    //
    // Device detached
    //
    goto ON_ERROR;
  }
  //
  // Initialize the transport protocols
  //
  Status = UsbIo->UsbGetInterfaceDescriptor (UsbIo, &Interface);
  if (EFI_ERROR (Status)) {
    DEBUG ((mUsbMscError, "USBMassDriverBindingStart: UsbIo->UsbGetInterfaceDescriptor (%r)\n", Status));
    goto ON_ERROR;
  }
  //
  // Get VID and PID
  //
  Status = UsbIo->UsbGetDeviceDescriptor (UsbIo, &DeviceDescriptor);
  if (EFI_ERROR (Status)) {
    goto ON_ERROR;
  }
  //
  // Checks vendor-specific devices
  //
  if (Interface.InterfaceClass == USB_VENDOR_SPECIFIC_CLASS) {
    Status = UsbVendorSpecificDevice(
               UsbIo,
               DeviceDescriptor.IdVendor,
               DeviceDescriptor.IdProduct,
               &Interface.InterfaceProtocol
               );
    if (EFI_ERROR (Status)) {
      goto ON_ERROR;
    }
  }
  //
  // Locates UsbMassTransport Protocol
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &mUsbMassTransportProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR(Status)) {
    goto ON_ERROR;
  }
  Status    = EFI_UNSUPPORTED;
  Transport = NULL;
  for (Index = 0; Index < HandleCount; Index++) {
    if (EFI_ERROR (gBS->HandleProtocol (
                          HandleBuffer[Index],
                          &mUsbMassTransportProtocolGuid,
                          (VOID **)&Transport
                          ))) {
      continue;
    }
    if (Interface.InterfaceProtocol == Transport->Protocol) {
      Status = EFI_SUCCESS;
      break;
    }
  }
  gBS->FreePool (HandleBuffer);

  if (EFI_ERROR (Status) || Transport == NULL) {
    DEBUG ((mUsbMscError, "USBMassDriverBindingStart: Transport->Init (%r)\n", Status));
    goto ON_ERROR;
  }
  //
  // Get the device path for new handle
  //
  DevicePath = NULL;
  Status = gBS->HandleProtocol (
                  Controller,
                  &gEfiDevicePathProtocolGuid,
                  (VOID **)&DevicePath
                  );
  if (EFI_ERROR (Status)) {
    goto ON_ERROR;
  }
  //
  // Get UsbPlatformPolicy to get the Multi-LUN support policy
  // Clear UsbLegacyPolicy first (GetUsbPlatformOptions will only capable to "set", can't "clear")
  //
  ZeroMem(&UsbLegacyPolicy, sizeof(USB_LEGACY_MODIFIERS));
  Status = gBS->LocateProtocol (
                  &gEfiUsbLegacyPlatformProtocolGuid,
                  NULL,
                  (VOID **)&LegacyBiosPlatform
                  );
  if (Status == EFI_SUCCESS) {
    //
    // Get the Usb Legacy Policy
    //
    LegacyBiosPlatform->GetUsbPlatformOptions(
                          LegacyBiosPlatform,
                          &UsbLegacyPolicy
                          );
  }
  //
  // Get the HC location for legacy BBS
  //
  HcLocation    = 0;
  TmpDevicePath = DevicePath;
  Status = gBS->LocateDevicePath (
                  &gEfiUsb3HcProtocolGuid,
                  &TmpDevicePath,
                  &Handle
                  );
  if (Status == EFI_SUCCESS) {
    Status = gBS->HandleProtocol (
                    Handle,
                    &gEfiPciIoProtocolGuid,
                    (VOID **)&PciIo
                    );
    if (Status == EFI_SUCCESS) {
      PciIo->GetLocation (
               PciIo,
               &Seg,
               &Bus,
               &Dev,
               &Fun
               );
      HcLocation = (UINT16)((Bus << 8) | (Dev << 3) | Fun);
    }
  }
  //
  // Stall 0.2(BOT)/1.2(CBI) second before TestUnit to waiting for spin-up
  //
  gBS->Stall (((Transport->Protocol == USB_MASS_STORE_BOT) ? 200 : 1200) * USB_MASS_STALL_1_MS);
  //
  // RaiseTPL to preventing the background thread releases the UsbMass through hot-plug
  //
  OldTPL = gBS->RaiseTPL (TPL_NOTIFY);
  //
  // Get maximum LUNs number for Multi-LUN device
  //
  MaximumLuns = 0;
  if (Transport->Protocol == USB_MASS_STORE_BOT && !UsbLegacyPolicy.UsbSupportSingleLunOnly) {
    //
    // Only BOT devices support LUN
    //
    UsbMassGetMaximumLuns (UsbIo, Interface.InterfaceNumber, &MaximumLuns);
  }
  for (Index = 0; Index <= MaximumLuns; Index ++) {
    //
    // Initial Transport instance
    //
    Context = NULL;
    Status = Transport->Init (UsbIo, Controller, &Context);
    //
    // Allocate the instance for each BlockIo
    //
    Status = gBS->AllocatePool (
                    EfiBootServicesData,
                    sizeof (USB_MASS_DEVICE),
                    (VOID **) &UsbMass
                    );
    if (EFI_ERROR (Status)) {
      goto ON_ERROR;
    }
    ZeroMem (UsbMass, sizeof (USB_MASS_DEVICE));
    UsbMass->Signature            = USB_MASS_SIGNATURE;
    UsbMass->Controller           = Controller;
    UsbMass->UsbIo                = UsbIo;
    UsbMass->BlockIo.Revision     = EFI_BLOCK_IO_PROTOCOL_REVISION;
    UsbMass->BlockIo.Media        = &UsbMass->BlockIoMedia;
    UsbMass->BlockIo.Reset        = UsbMassReset;
    UsbMass->BlockIo.ReadBlocks   = UsbMassReadBlocks;
    UsbMass->BlockIo.WriteBlocks  = UsbMassWriteBlocks;
    UsbMass->BlockIo.FlushBlocks  = UsbMassFlushBlocks;
    CopyMem (&UsbMass->DiskInfo.Interface, &gEfiDiskInfoUsbInterfaceGuid, sizeof (EFI_GUID));
    UsbMass->DiskInfo.Inquiry     = UsbMassInquiry;
    UsbMass->DiskInfo.Identify    = UsbMassIdentify;
    UsbMass->DiskInfo.SenseData   = UsbMassSenseData;
    UsbMass->DiskInfo.WhichIde    = UsbMassWhichIde;
    UsbMass->OpticalStorage       = 0;
    UsbMass->Transport            = Transport;
    UsbMass->Context              = Context;
    UsbMass->Lun                  = (UINT8)Index;
    UsbMass->DevicePath           = NULL;
    UsbMass->UsbCore              = UsbCore;
    //
    // Get the storage's parameters, such as last block number.
    // then install the BLOCK_IO
    //
    Status = UsbMassInitMedia (UsbMass);
    if (!EFI_ERROR (Status)) {
      if ((UsbMass->Pdt != USB_PDT_DIRECT_ACCESS) && 
           (UsbMass->Pdt != USB_PDT_CDROM) &&
           (UsbMass->Pdt != USB_PDT_OPTICAL) && 
           (UsbMass->Pdt != USB_PDT_SIMPLE_DIRECT)) {
        DEBUG ((mUsbMscError, "USBMassDriverBindingStart: Found an unsupported peripheral type[%d]\n", UsbMass->Pdt));
        if (MaximumLuns == 0) goto ON_ERROR; else continue;
      }
    } else if (Status != EFI_NO_MEDIA){
      DEBUG ((mUsbMscError, "USBMassDriverBindingStart: UsbMassInitMedia (%r)\n", Status));
      if (Status == EFI_NOT_FOUND) {
        //
        // Device has detached or no response, quit immediately
        //
        Status = EFI_DEVICE_ERROR;
        goto ON_ERROR;
      }
      if (Status == EFI_NO_RESPONSE) {
        //
        // Serious error occurred, skip the protocol generating and 
        // block the UsbIo to preventing further unnecessary retry
        //
        continue;
      }
      if (MaximumLuns == 0) goto ON_ERROR; else continue;
    }
    //
    // Install BlockIo. For Multi-LUN device should
    // create new handle with MSG_DEVICE_LOGICAL_UNIT_DP device path appended
    //
    if (MaximumLuns == 0) {
      //
      // Install BlockIo protocol for Single-LUN device
      //
      Status = gBS->InstallMultipleProtocolInterfaces (
                      &Controller,
                      &gEfiBlockIoProtocolGuid,
                      &UsbMass->BlockIo,
                      &gEfiDiskInfoProtocolGuid,
                      &UsbMass->DiskInfo,
                      NULL
                      );
    } else {
      //
      // Install child BlockIo protocol for Multi-LUN device. Create new handle with 
      // MSG_DEVICE_LOGICAL_UNIT_DP device path appended
      //
      LunDevicePath                 = (DEVICE_LOGICAL_UNIT_DEVICE_PATH*)NewDevicePath;
      LunDevicePath->Header.Type    = MESSAGING_DEVICE_PATH;
      LunDevicePath->Header.SubType = MSG_DEVICE_LOGICAL_UNIT_DP;
      SetDevicePathNodeLength (&LunDevicePath->Header, sizeof (DEVICE_LOGICAL_UNIT_DEVICE_PATH));
      LunDevicePath->Lun            = (UINT8)Index;
      SetDevicePathEndNode ((EFI_DEVICE_PATH_PROTOCOL*)(LunDevicePath + 1));
      UsbMass->DevicePath           = AppendDevicePath(DevicePath, &LunDevicePath->Header);
      //
      // Create new handle
      //
      UsbMass->Controller = NULL;
      Status = gBS->InstallMultipleProtocolInterfaces (
                      &UsbMass->Controller,
                      &gEfiDevicePathProtocolGuid,
                      UsbMass->DevicePath,
                      &gEfiBlockIoProtocolGuid,
                      &UsbMass->BlockIo,
                      &gEfiDiskInfoProtocolGuid,
                      &UsbMass->DiskInfo,
                      NULL
                      );
      if (!EFI_ERROR (Status)) {
      	Status = gBS->OpenProtocol (
                        Controller,
                        &gEfiUsbIoProtocolGuid,
                        (VOID **)&UsbIo,
                        This->DriverBindingHandle,
                        UsbMass->Controller,
                        EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER
                        );
      }
    }
    if (EFI_ERROR (Status)) {
      goto ON_ERROR;
    }
    //
    // Setup the data for UsbLegacy
    //
    if (UsbMass->UsbCore) {
      //
      // Registers a UsbMassStorage Device for UsbLegacy
      //
      UsbDevice.UsbMassStorage.Type            = USB_CORE_USB_MASS_STORAGE;
      UsbDevice.UsbMassStorage.UsbIo           = UsbIo;
      UsbDevice.UsbMassStorage.Transport       = (USB_MASS_TRANSPORT_PROTOCOL*)UsbMass->Context;
      UsbDevice.UsbMassStorage.Media           = &UsbMass->BlockIoMedia;
      UsbDevice.UsbMassStorage.MediumType      = &UsbMass->MediumType;
      UsbDevice.UsbMassStorage.Manufacturer    = UsbMass->InquiryData.VendorID;
      UsbDevice.UsbMassStorage.Product         = UsbMass->InquiryData.ProductID;
      UsbDevice.UsbMassStorage.SerialNumber    = UsbMass->InquiryData.ProductRevision;
      UsbDevice.UsbMassStorage.VendorID        = DeviceDescriptor.IdVendor;
      UsbDevice.UsbMassStorage.ProductID       = DeviceDescriptor.IdProduct;
      UsbDevice.UsbMassStorage.HcLocation      = HcLocation;
      UsbDevice.UsbMassStorage.Lun             = UsbMass->Lun;
      UsbDevice.UsbMassStorage.InterfaceNumber = Interface.InterfaceNumber;
      UsbDevice.UsbMassStorage.HotPlugged      = 0;
      Status = UsbMass->UsbCore->InsertUsbDevice(
                                   &UsbDevice
                                   );
      if (Status == EFI_ALREADY_STARTED) {
        //
        // InsertUsbDevice may return EFI_ALREADY_STARTED because
        // this device had recorded into BBS. Finish the transport
        // allocated by previous Init
        //
        ((USB_MASS_TRANSPORT_PROTOCOL*)UsbMass->Context)->Fini(UsbMass->Context);
        //
        // Use of the stock context in recorded device list
        //
        UsbMass->Context = UsbDevice.UsbMassStorage.Transport;
      }
    }
  }
  Status = EFI_SUCCESS;
  goto ON_EXIT;

ON_ERROR:
  if (UsbMass) {
    gBS->FreePool (UsbMass);
  }
  if (Status != EFI_DEVICE_ERROR) {
    gBS->CloseProtocol (
           Controller,
           &gEfiUsbIoProtocolGuid,
           This->DriverBindingHandle,
           Controller
           );
  }
ON_EXIT:
  //
  // RestoreTPL back to original TPL
  //
  if (OldTPL) gBS->RestoreTPL(OldTPL);
  return Status;
}

/**

  Stop controlling the device.

  @param  This                  The USB mass storage driver binding
  @param  Controller            The device controller controlled by the driver.
  @param  NumberOfChildren      The number of children of this device
  @param  ChildHandleBuffer     The buffer of children handle.

  @retval EFI_SUCCESS           The driver stopped from controlling the device.
  @retval Others                Failed to stop the driver 
  
**/
EFI_STATUS
EFIAPI
UsbMassDriverBindingStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL *This,
  IN  EFI_HANDLE                  Controller,
  IN  UINTN                       NumberOfChildren,
  IN  EFI_HANDLE                  *ChildHandleBuffer
  )
{
  EFI_STATUS            Status;
  USB_MASS_DEVICE       *UsbMass;
  EFI_BLOCK_IO_PROTOCOL *BlockIo;
  EFI_TPL               OldTPL;
  UINTN                 Index;

  //
  // First, get our context back from the BLOCK_IO
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiBlockIoProtocolGuid,
                  (VOID **)&BlockIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  //
  // RaiseTPL to preventing the background thread releases the UsbMass through hot-plug
  //
  OldTPL = gBS->RaiseTPL (TPL_NOTIFY);
  if (!EFI_ERROR (Status)) {
    //
    // For Single-LUN device, the BlockIo and UsbIo are installed in single handle
    //
    UsbMass = USB_MASS_DEVICE_FROM_BLOCKIO (BlockIo);
    //
    // Uninstall Block I/O protocol from the device handle,
    // then call the transport protocol to stop itself.
    //
    Status = gBS->UninstallMultipleProtocolInterfaces (
                    Controller,
                    &gEfiBlockIoProtocolGuid,
                    &UsbMass->BlockIo,
                    &gEfiDiskInfoProtocolGuid,
                    &UsbMass->DiskInfo,
                    NULL
                    );
    if (EFI_ERROR (Status)) {
      goto ON_EXIT;
    }
    if (UsbMass->UsbCore) {
      //
      // Unregisters a UsbMassStorage Device
      //
      Status = UsbMass->UsbCore->RemoveUsbDevice(UsbMass->UsbIo);
      if (!EFI_ERROR (Status)) {
        //
        // RemoveUsbDevice may return EFI_ALREADY_STARTED because
        // this device had recorded into BBS. Don't finish the transport
        // protocol because it may attached again(through hot-plug)
        //
        UsbMass->Transport->Fini (UsbMass->Context);
      }
    }
    gBS->FreePool (UsbMass);
  }
  //
  // Uninstall child handles for Multi-LUN device
  //
  for (Index = 0; Index < NumberOfChildren; Index ++) { 
    //
    // Uninstall child BlockIo protocol
    //
    Status = gBS->OpenProtocol (
                    ChildHandleBuffer[Index],
                    &gEfiBlockIoProtocolGuid,
                    (VOID **)&BlockIo,
                    This->DriverBindingHandle,
                    Controller,
                    EFI_OPEN_PROTOCOL_GET_PROTOCOL
                    );
    if (!EFI_ERROR (Status)) {
      UsbMass = USB_MASS_DEVICE_FROM_BLOCKIO (BlockIo);
      gBS->CloseProtocol (
             Controller,
             &gEfiUsbIoProtocolGuid,
             This->DriverBindingHandle,
             ChildHandleBuffer[Index]
             );
      Status = gBS->UninstallMultipleProtocolInterfaces (
                      ChildHandleBuffer[Index],
                      &gEfiDevicePathProtocolGuid,
                      UsbMass->DevicePath,
                      &gEfiBlockIoProtocolGuid,
                      &UsbMass->BlockIo,
                      &gEfiDiskInfoProtocolGuid,
                      &UsbMass->DiskInfo,
                      NULL
                      );
      if (EFI_ERROR (Status)) {
        goto ON_EXIT;
      }
      if (UsbMass->UsbCore) {
        //
        // Unregisters a UsbMassStorage Device
        //
        Status = UsbMass->UsbCore->RemoveUsbDevice(UsbMass->UsbIo);
        if (!EFI_ERROR (Status)) {
          //
          // RemoveUsbDevice may return EFI_ALREADY_STARTED because
          // this device had recorded into BBS. Don't finish the transport
          // protocol because it may attached again(through hot-plug)
          //
          UsbMass->Transport->Fini (UsbMass->Context);
        }
      }
      if (UsbMass->DeviceNameTable) {
        FreeUnicodeStringTable (UsbMass->DeviceNameTable);
      }
      gBS->FreePool (UsbMass->DevicePath);
      gBS->FreePool (UsbMass);
    }
  }
  gBS->CloseProtocol (
         Controller,
         &gEfiUsbIoProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );
  Status = EFI_SUCCESS;
ON_EXIT:
  //
  // RestoreTPL back to original TPL
  //
  gBS->RestoreTPL(OldTPL);
  return Status;
}

EFI_DRIVER_BINDING_PROTOCOL gUsbMassDriverBinding = {
  UsbMassDriverBindingSupported,
  UsbMassDriverBindingStart,
  UsbMassDriverBindingStop,
  0x11,
  NULL,
  NULL
};

/**

  The entry point for the driver, which will install the driver binding and 
  component name protocol

  @param  ImageHandle           The image handle of this driver
  @param  SystemTable           The system table

  @retval EFI_SUCCESS           the protocols are installed OK
  @retval Others                Failed to install protocols.

**/
EFI_STATUS
EFIAPI
UsbMassStorageEntryPoint (
  IN EFI_HANDLE               ImageHandle,
  IN EFI_SYSTEM_TABLE         *SystemTable
  )
{
  EFI_STATUS  Status;
  //
  // Install driver binding protocol
  //
  Status = EfiLibInstallDriverBindingComponentName2 (
               ImageHandle,
               SystemTable,
               &gUsbMassDriverBinding,
               ImageHandle,
               &gUsbMassStorageComponentName,
               &gUsbMassStorageComponentName2
               );

  return Status;
}