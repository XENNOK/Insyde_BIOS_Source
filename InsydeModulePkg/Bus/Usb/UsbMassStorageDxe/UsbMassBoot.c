/** @file
  This file implement the command set of "USB Mass Storage Specification
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

extern UINTN mUsbMscInfo;
extern UINTN mUsbMscError;

/**

  Read an UINT32 from the buffer to avoid byte alignment problems, then 
  convert that to the little endia. The USB mass storage bootability spec
  use big endia
  
  @param  Buf                   The buffer contains the first byte of the UINT32 in big endia.

  @retval The UINT32 value read from the buffer in little endia.

**/
STATIC
UINT32
UsbBootGetUint32 (
  IN UINT8                  *Buf
  )
{
  UINT32                    Value;

  CopyMem (&Value, Buf, sizeof (UINT32));
  return USB_BOOT_SWAP32 (Value);
}

/**

  Put an UINT32 in little endia to the buffer. The data is converted to
  big endia before writing.

  @param  Buf                   The buffer to write data to
  @param  Data32                The data to write.

**/
STATIC
VOID
UsbBootPutUint32 (
  IN UINT8                  *Buf,
  IN UINT32                 Data32
  )
{
  Data32 = USB_BOOT_SWAP32 (Data32);
  CopyMem (Buf, &Data32, sizeof (UINT32));
}

/**

  Put an UINT16 in little endia to the buffer. The data is converted to
  big endia before writing.

  @param  Buf                   The buffer to write data to
  @param  Data16                The data to write

**/
STATIC
VOID
UsbBootPutUint16 (
  IN UINT8                   *Buf,
  IN UINT16                  Data16
  )
{
  Data16 = USB_BOOT_SWAP16 (Data16);
  CopyMem (Buf, &Data16, sizeof (UINT16));
}

/**

  Request sense information via sending Request Sense
  Packet Command.

  @param  UsbMass               The device to be requested sense data
    
  @retval EFI_DEVICE_ERROR      Hardware error
  @retval EFI_SUCCESS           Success

**/
EFI_STATUS
UsbBootRequestSense (
  IN USB_MASS_DEVICE          *UsbMass
  )
{ 
  USB_BOOT_REQUEST_SENSE_CMD  SenseCmd;
  USB_BOOT_REQUEST_SENSE_DATA SenseData;
  EFI_BLOCK_IO_MEDIA          *Media;
  USB_MASS_TRANSPORT_PROTOCOL *Transport;
  EFI_STATUS                  Status;
  UINT32                      CmdResult;

  Transport = UsbMass->Transport;

  //
  // Request the sense data from the device if command failed
  //
  ZeroMem (&SenseCmd, sizeof (USB_BOOT_REQUEST_SENSE_CMD));
  ZeroMem (&SenseData, sizeof (USB_BOOT_REQUEST_SENSE_DATA));
  
  SenseCmd.OpCode   = USB_BOOT_REQUEST_SENSE_OPCODE;
  SenseCmd.Lun      = USB_BOOT_LUN (UsbMass->Lun);
  SenseCmd.AllocLen = sizeof (USB_BOOT_REQUEST_SENSE_DATA);

  Status = Transport->ExecCommand (
                        UsbMass->Context,
                        &SenseCmd,
                        sizeof (USB_BOOT_REQUEST_SENSE_CMD),
                        EfiUsbDataIn,
                        &SenseData,
                        sizeof (USB_BOOT_REQUEST_SENSE_DATA),
                        USB_BOOT_GENERAL_CMD_TIMEOUT,
                        &CmdResult
                        );
  if (EFI_ERROR (Status) || CmdResult != USB_MASS_CMD_SUCCESS) {
    DEBUG ((mUsbMscError, "UsbBootRequestSense: (%r) CmdResult=0x%x\n", Status, CmdResult));
    return Status;
  }

  //
  // Interpret the sense data and update the media status if necessary.
  //
  Media = &UsbMass->BlockIoMedia;

  switch (USB_BOOT_SENSE_KEY (SenseData.SenseKey)) {
    
  case USB_BOOT_SENSE_NO_SENSE:
    Status = EFI_NO_RESPONSE;
    break;

  case USB_BOOT_SENSE_RECOVERED:
    //
    // Suppose hardware can handle this case, and recover later by itself
    //
    Status = EFI_NOT_READY;
    break;

  case USB_BOOT_SENSE_NOT_READY:
    switch (SenseData.ASC) {    
    case USB_BOOT_ASC_NO_MEDIA:
      Status              = EFI_NO_MEDIA;
      Media->MediaPresent = FALSE;
      break;
      
    case USB_BOOT_ASC_MEDIA_UPSIDE_DOWN:
      Status              = EFI_DEVICE_ERROR;
      Media->MediaPresent = FALSE;
      break;
    
    case USB_BOOT_ASC_UNKNOWN:
      //
      // Some special device will return zero ASC/ASCQ in SENSE_NOT_READY sense key
      //
      SenseData.ASCQ = USB_BOOT_ASCQ_DEVICE_BUSY;
    case USB_BOOT_ASC_NOT_READY:
      if (SenseData.ASCQ == USB_BOOT_ASCQ_IN_PROGRESS || 
          SenseData.ASCQ == USB_BOOT_ASCQ_DEVICE_BUSY) {
        //
        // Regular timeout, and need retry once more
        //
        DEBUG ((mUsbMscInfo, "UsbBootRequestSense: Not ready and need retry once more\n"));
        Status = EFI_NOT_READY; 
        //
        // Some CD/DVDRom needed extra stall to waiting for device ready
        //
        gBS->Stall(600 * USB_MASS_STALL_1_MS);
      }
    }
    break;

  case USB_BOOT_SENSE_ILLEGAL_REQUEST:
    Status = EFI_INVALID_PARAMETER;
    break;

  case USB_BOOT_SENSE_UNIT_ATTENTION:
    Status = EFI_DEVICE_ERROR;
    if (SenseData.ASC == USB_BOOT_ASC_MEDIA_CHANGE) {
      //
      // If MediaChange, reset ReadOnly and new MediId
      //
      Status          = EFI_MEDIA_CHANGED;
      Media->ReadOnly = FALSE;
      Media->MediaId++;
    }
    break;

  case USB_BOOT_SNESE_DATA_PROTECT:
    Status          = EFI_WRITE_PROTECTED;
    Media->ReadOnly = TRUE;
    break;

  default:
    Status = EFI_DEVICE_ERROR;
    break;
  }

  DEBUG ((mUsbMscInfo, "UsbBootRequestSense: (%r) with sense key %x/%x/%x\n", 
          Status,
          USB_BOOT_SENSE_KEY (SenseData.SenseKey),
          SenseData.ASC,
          SenseData.ASCQ
          ));
  
  return Status;
}

/**

  Execute the USB mass storage bootability commands. If execution
  failed, retrieve the error by REQUEST_SENSE then update the device's
  status, such as ReadyOnly.

  @param  UsbMass               The device to issue commands to
  @param  Cmd                   The command to execute
  @param  CmdLen                The length of the command
  @param  DataDir               The direction of data transfer
  @param  Data                  The buffer to hold the data
  @param  DataLen               The length of expected data
  @param  Timeout               The timeout used to transfer
    
  @retval EFI_SUCCESS           The command is excuted OK
  @retval EFI_DEVICE_ERROR      Failed to request sense
  @retval EFI_INVALID_PARAMETER The command has some invalid parameters
  @retval EFI_WRITE_PROTECTED   The device is write protected
  @retval EFI_MEDIA_CHANGED     The device media has been changed

**/
STATIC
EFI_STATUS
UsbBootExecCmd (
  IN USB_MASS_DEVICE            *UsbMass,
  IN VOID                       *Cmd,
  IN UINT8                      CmdLen,
  IN EFI_USB_DATA_DIRECTION     DataDir,
  IN VOID                       *Data,
  IN UINT32                     DataLen,
  IN UINT32                     Timeout
  )
{
  USB_MASS_TRANSPORT_PROTOCOL *Transport;
  EFI_STATUS                  Status;
  UINT32                      CmdResult;
  UINTN                       Index;
  UINTN                       Retry;

  Status    = EFI_SUCCESS;
  Transport = UsbMass->Transport;
  Retry     = USB_BOOT_COMMAND_RETRY;
  for (Index = 0; Index < Retry; Index ++) {
    //
    // Check is device been detached
    //
    if (UsbMass->UsbCore && UsbMass->UsbCore->CheckDeviceDetached(UsbMass->UsbIo) == EFI_SUCCESS) {
      return EFI_NOT_FOUND;
    }
    //
    // Some of the devices will return fail in order to tell us the current status
    // of the device, like "write protected". In such situation, try it again
    // can make it normal
    //
    Status = Transport->ExecCommand (
                          UsbMass->Context,
                          Cmd,
                          CmdLen,
                          DataDir,
                          Data,
                          DataLen,
                          Timeout,
                          &CmdResult
                          );
    //
    // ExecCommand return success and get the right CmdResult means
    // the commnad transfer is OK.
    //
    if ((CmdResult == USB_MASS_CMD_SUCCESS) && !EFI_ERROR(Status)) {
      return EFI_SUCCESS;
    } else if (Status == EFI_NOT_FOUND) {
      return Status;
    } else if (CmdResult == USB_MASS_CMD_PHASE_ERROR) {
      return EFI_NO_RESPONSE;
    }
    Status = UsbBootRequestSense (UsbMass);
    //
    // Quit if media changed
    //
    if (Status == EFI_MEDIA_CHANGED || Status == EFI_INVALID_PARAMETER || Status == EFI_NO_MEDIA) {
      break;
    }
    //
    // Given three times retry is device is going to ready
    //
    if (Status == EFI_NOT_READY) {
      Retry = USB_BOOT_COMMAND_RETRY * 3;
    }
  }
  return Status;
}

/**

  Use the TEST UNIT READY command to check whether it is ready.
  If it is ready, update the parameters.

  @param  UsbMass               The device to test
  @param  Timeout               The timeout value in millisecond

  @retval EFI_SUCCESS           The device is ready and parameters are updated.
  @retval Others                Device not ready.

**/
EFI_STATUS
UsbBootIsUnitReady (
  IN USB_MASS_DEVICE            *UsbMass,
  IN UINT32                     Timeout
  )
{
  USB_BOOT_TEST_UNIT_READY_CMD  TestCmd;
  
  ZeroMem (&TestCmd, sizeof (USB_BOOT_TEST_UNIT_READY_CMD));

  TestCmd.OpCode  = USB_BOOT_TEST_UNIT_READY_OPCODE;
  TestCmd.Lun     = USB_BOOT_LUN (UsbMass->Lun);

  return UsbBootExecCmd (
           UsbMass, 
           &TestCmd, 
           sizeof (USB_BOOT_TEST_UNIT_READY_CMD), 
           EfiUsbNoData, 
           NULL, 
           0, 
           Timeout
           );
}

/**

  Use the START UNIT command to startup the device

  @param  UsbMass               The device to start unit

  @retval EFI_SUCCESS           The device is ready and parameters are updated.
  @retval Others                Device not ready.

**/
EFI_STATUS
UsbBootStartUnit (
  IN USB_MASS_DEVICE          *UsbMass
  )
{
  USB_BOOT_START_UNIT_CMD     StartUnitCmd;

  ZeroMem (&StartUnitCmd, sizeof (USB_BOOT_START_UNIT_CMD));
  StartUnitCmd.OpCode = USB_BOOT_START_UNIT_OPCODE;
  StartUnitCmd.Lun    = USB_BOOT_LUN (UsbMass->Lun);
  StartUnitCmd.Start  = 1;
  return UsbBootExecCmd (
           UsbMass,
           &StartUnitCmd,
           sizeof (USB_BOOT_START_UNIT_CMD),
           EfiUsbNoData, 
           NULL, 
           0, 
           USB_BOOT_GENERAL_CMD_TIMEOUT
           );
}

/**

  Inquiry Command requests that information regrarding parameters of
  the Device be sent to the Host.

  @param  UsbMass               The device to inquiry.

  @retval EFI_SUCCESS           The device is ready and parameters are updated.
  @retval Others                Device not ready.

**/
EFI_STATUS
UsbBootInquiry (
  IN USB_MASS_DEVICE            *UsbMass
  )
{
  USB_BOOT_INQUIRY_CMD        InquiryCmd;
  USB_BOOT_INQUIRY_DATA       *InquiryData;
  EFI_BLOCK_IO_MEDIA          *Media;
  EFI_STATUS                  Status;
  UINTN                       Index;
  UINTN                       Index1;
  UINT8                       *ManufactureString;
  CHAR16                      Unicode[32];
  UINTN                       SpaceChar;

  Media       = &UsbMass->BlockIoMedia;
  InquiryData = &UsbMass->InquiryData;

  //
  // Use the Inquiry command to get the RemovableMedia setting.
  //
  ZeroMem (&InquiryCmd, sizeof (USB_BOOT_INQUIRY_CMD));
  ZeroMem (InquiryData, sizeof (USB_BOOT_INQUIRY_DATA));

  InquiryCmd.OpCode   = USB_BOOT_INQUIRY_OPCODE;
  InquiryCmd.Lun      = USB_BOOT_LUN (UsbMass->Lun);
  InquiryCmd.AllocLen = sizeof (USB_BOOT_INQUIRY_DATA);

  Status = UsbBootExecCmd (
             UsbMass,
             &InquiryCmd,
             sizeof (USB_BOOT_INQUIRY_CMD),
             EfiUsbDataIn,
             InquiryData,
             sizeof (USB_BOOT_INQUIRY_DATA),
             USB_BOOT_INQUIRY_CMD_TIMEOUT
             );
  if (EFI_ERROR (Status) && Status != EFI_NO_MEDIA) {
    //
    // The error of EFI_NO_MEDIA excluded due to some devices capable to
    // response Inquiry data with media not present sense code to indicate
    // this device is not ready for media access
    //
    return Status;
  }

  UsbMass->Pdt          = USB_BOOT_PDT (InquiryData->Pdt);
  Media->RemovableMedia = USB_BOOT_REMOVABLE (InquiryData->Removable);
  //
  // Default value 512 Bytes, in case no media present at first time
  //
  Media->BlockSize      = 0x0200;
  //
  // String checking to preventing empty or illegal string reported by device
  //
  for (Index = 0, SpaceChar = 0, ManufactureString = InquiryData->VendorID; Index < 24; Index ++) {
    if (ManufactureString[Index] == ' ') SpaceChar ++;
    if (ManufactureString[Index] == 0 && Index > 0) continue;
    if (ManufactureString[Index] < 0x20 || ManufactureString[Index] > 0x7E) break;
  }
  if (SpaceChar == 24 || Index < 24) {
    //
    // Set the default name to the device
    //
    CopyMem (InquiryData->VendorID, "Generic USB Storage     ", 24);
  }
  //
  // Convert to unicode for UEFI controller name
  //
  for (Index = 0, Index1 = 0; Index < 8; Index ++) {
    Unicode[Index1 ++] = InquiryData->VendorID[Index];
  }
  for (Index = 0; Index < 16; Index ++) {
    Unicode[Index1 ++] = InquiryData->ProductID[Index];
  }
  Unicode[Index1 ++] = 0;
  AddUnicodeString2 (
    LANGUAGE_CODE_ENGLISH_ISO639,
    gUsbMassStorageComponentName.SupportedLanguages,
    &UsbMass->DeviceNameTable,
    Unicode,
    TRUE
    );
  AddUnicodeString2 (
    LANGUAGE_CODE_ENGLISH_RFC4646,
    gUsbMassStorageComponentName2.SupportedLanguages,
    &UsbMass->DeviceNameTable,
    Unicode,
    FALSE
    );
  return Status;
}

/**

  Get the capacity of the USB mass storage media, including
  the presentation, block size, and last block number. This
  function is used to get the disk parameters at the start if
  it is a non-removable media or to detect the media if it is
  removable. 

  @param  UsbMass               The device to retireve disk gemotric.

  @retval EFI_SUCCESS           The disk gemotric is successfully retrieved.
  @retval EFI_DEVICE_ERROR      Something is inconsistent with the disk gemotric.

**/
EFI_STATUS
UsbBootReadCapacity (
  IN USB_MASS_DEVICE          *UsbMass
  )
{
  USB_BOOT_READ_CAPACITY_CMD  CapacityCmd;
  USB_BOOT_READ_CAPACITY_DATA CapacityData;
  EFI_BLOCK_IO_MEDIA          *Media;
  EFI_STATUS                  Status;
  UINT32                      BlockSize;

  Media   = &UsbMass->BlockIoMedia;
  //
  // Use the READ CAPACITY command to get the block length and last blockno
  //
  ZeroMem (&CapacityCmd, sizeof (USB_BOOT_READ_CAPACITY_CMD));
  ZeroMem (&CapacityData, sizeof (USB_BOOT_READ_CAPACITY_DATA));

  CapacityCmd.OpCode = USB_BOOT_READ_CAPACITY_OPCODE;
  CapacityCmd.Lun    = USB_BOOT_LUN (UsbMass->Lun);

  Status = UsbBootExecCmd (
             UsbMass,
             &CapacityCmd,
             sizeof (USB_BOOT_READ_CAPACITY_CMD),
             EfiUsbDataIn,
             &CapacityData,
             sizeof (USB_BOOT_READ_CAPACITY_DATA),
             USB_BOOT_GENERAL_CMD_TIMEOUT
             );
  if (EFI_ERROR (Status)) { 
    return Status;
  }
  BlockSize = UsbBootGetUint32 (CapacityData.BlockLen);
  //
  // Usb ZIP drive may get zero block size when motor spin up
  //
  if (BlockSize != 0) {
    Media->MediaPresent = TRUE;
    Media->LastBlock    = UsbBootGetUint32 (CapacityData.LastLba);
    Media->BlockSize    = BlockSize;
    //
    // Setup the MediumType for FDD
    //
    switch(Media->LastBlock) {
      case 0xb3f:
        UsbMass->MediumType = FDD_MEDIUM_TYPE_1440K;
        break;
      case 0x59f:
        UsbMass->MediumType = FDD_MEDIUM_TYPE_720K;
        break;
    }
  }  
  DEBUG ((mUsbMscInfo, "UsbBootReadCapacity Success LBA=%d BlockSize=%d\n", 
    Media->LastBlock, Media->BlockSize));
  
  return EFI_SUCCESS;
}

/**

  Detect whether the removable media is present and whether it has changed.
  The Non-removable media doesn't need it.

  @param  UsbMass               The device to retireve disk gemotric.

  @retval EFI_SUCCESS           The disk gemotric is successfully retrieved.
  @retval EFI_DEVICE_ERROR      Something is inconsistent with the disk gemotric.

**/
EFI_STATUS
UsbBootDetectMedia (
  IN  USB_MASS_DEVICE       *UsbMass
  )
{   
  EFI_BLOCK_IO_MEDIA        OldMedia;
  EFI_BLOCK_IO_MEDIA        *Media;
  EFI_STATUS                Status;
  
  Media    = &UsbMass->BlockIoMedia;
  CopyMem (&OldMedia, Media, sizeof (EFI_BLOCK_IO_MEDIA));
  //
  // First test whether the device is ready and get status
  // If media changed or ready, need read the device's capacity
  //
  Status = UsbBootIsUnitReady (UsbMass, USB_BOOT_GENERAL_CMD_TIMEOUT);
  if (Status == EFI_MEDIA_CHANGED) {
    DEBUG ((mUsbMscInfo, "UsbBootDetectMedia: Need Read Capacity\n"));
    Status = UsbBootReadCapacity (UsbMass);
  }
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  //
  // Detect whether it is necessary to reinstall the BlockIO
  //
  if ((Media->MediaId != OldMedia.MediaId) || 
      (Media->MediaPresent != OldMedia.MediaPresent) || 
      (Media->ReadOnly != OldMedia.ReadOnly) || 
      (Media->BlockSize != OldMedia.BlockSize) || 
      (Media->LastBlock != OldMedia.LastBlock)) {
    DEBUG ((mUsbMscInfo, "UsbBootDetectMedia: Need reinstall BlockIoProtocol\n"));
    Media->MediaId++;
    gBS->ReinstallProtocolInterface (
           UsbMass->Controller,
           &gEfiBlockIoProtocolGuid,
           &UsbMass->BlockIo,
           &UsbMass->BlockIo
           );
    //
    // Check whether media present or media changed or write protected
    //
    if (Media->MediaPresent == FALSE) {
      Status = EFI_NO_MEDIA;
    } 
    if (Media->MediaId != OldMedia.MediaId) {
      Status = EFI_MEDIA_CHANGED;
    } 
    if (Media->ReadOnly != OldMedia.ReadOnly) {
      Status = EFI_WRITE_PROTECTED;
    }
  }
  
  return Status;
}

/**

  Read some blocks from the device.

  @param  UsbMass               The USB mass storage device to read from
  @param  Lba                   The start block number
  @param  TotalBlock            Total block number to read
  @param  Buffer                The buffer to read to

  @retval EFI_SUCCESS           Data are read into the buffer
  @retval Others                Failed to read all the data

**/
EFI_STATUS
UsbBootReadBlocks (
  IN  USB_MASS_DEVICE       *UsbMass,
  IN  UINT32                Lba,
  IN  UINTN                 TotalBlock,
  OUT UINT8                 *Buffer
  )
{
  USB_BOOT_READ10_CMD       ReadCmd;
  EFI_STATUS                Status;
  UINT16                    Count;
  UINT32                    BlockSize;
  UINT32                    ByteSize;
  UINT32                    Timeout;

  BlockSize = UsbMass->BlockIoMedia.BlockSize;
  Status    = EFI_SUCCESS;

  while (TotalBlock > 0) {
    //
    // Split the total blocks into smaller pieces to ease the pressure
    // on the device. We must split the total block because the READ10
    // command only has 16 bit transfer length (in the unit of block).
    //
    Count     = (UINT16)(((TotalBlock * BlockSize) < (USB_BOOT_IO_BLOCKS * 0x200)) ? TotalBlock : ((USB_BOOT_IO_BLOCKS * 0x200) / BlockSize));
    ByteSize  = (UINT32)Count * BlockSize;

    Timeout = (UINT32)Count * USB_BOOT_GENERAL_BLOCK_TIMEOUT;
    //
    // Optical device need longer timeout than other device
    //
    if (UsbMass->OpticalStorage == 1) {
      Timeout += USB_BOOT_OPTICAL_BLOCK_TIMEOUT;
    }
    //
    // Adds Timeout a base value
    //
    Timeout += USB_MASS_STALL_1_S;
    //
    // Fill in the command then execute 
    //
    ZeroMem (&ReadCmd, sizeof (USB_BOOT_READ10_CMD));
    
    ReadCmd.OpCode  = USB_BOOT_READ10_OPCODE;
    ReadCmd.Lun     = USB_BOOT_LUN (UsbMass->Lun);
    UsbBootPutUint32 (ReadCmd.Lba, Lba);
    UsbBootPutUint16 (ReadCmd.TransferLen, Count);
    
    Status = UsbBootExecCmd (
               UsbMass,
               &ReadCmd,
               sizeof (USB_BOOT_READ10_CMD),
               EfiUsbDataIn,
               Buffer,
               ByteSize,
               Timeout
               );
    if (EFI_ERROR (Status)) {   
      return Status;
    }

    Lba        += Count;
    Buffer     += Count * BlockSize;
    TotalBlock -= Count;
  }

  return Status;
}

/**

  Write some blocks to the device.

  @param  UsbMass               The USB mass storage device to write to
  @param  Lba                   The start block number
  @param  TotalBlock            Total block number to write
  @param  Buffer                The buffer to write to

  @retval EFI_SUCCESS           Data are written into the buffer
  @retval Others                Failed to write all the data

**/
EFI_STATUS
UsbBootWriteBlocks (
  IN  USB_MASS_DEVICE         *UsbMass,
  IN  UINT32                  Lba,
  IN  UINTN                   TotalBlock,
  OUT UINT8                   *Buffer
  )
{
  USB_BOOT_WRITE10_CMD  WriteCmd;
  EFI_STATUS            Status;
  UINT16                Count;
  UINT32                BlockSize;
  UINT32                ByteSize;
  UINT32                Timeout;

  BlockSize = UsbMass->BlockIoMedia.BlockSize;
  Status    = EFI_SUCCESS;

  while (TotalBlock > 0) {
    //
    // Split the total blocks into smaller pieces to ease the pressure
    // on the device. We must split the total block because the WRITE10
    // command only has 16 bit transfer length (in the unit of block).
    //
    Count     = (UINT16)(((TotalBlock * BlockSize) < (USB_BOOT_IO_BLOCKS * 0x200)) ? TotalBlock : ((USB_BOOT_IO_BLOCKS * 0x200) / BlockSize));
    ByteSize  = (UINT32)Count * BlockSize;

    Timeout = (UINT32)Count * USB_BOOT_GENERAL_BLOCK_TIMEOUT;
    //
    // Optical device need longer timeout than other device
    //
    if (UsbMass->OpticalStorage == 1) {
      Timeout += USB_BOOT_OPTICAL_BLOCK_TIMEOUT;
    }
    //
    // Adds Timeout a base value
    //
    Timeout += USB_MASS_STALL_1_S;
    //
    // Fill in the write10 command block
    //
    ZeroMem (&WriteCmd, sizeof (USB_BOOT_WRITE10_CMD));

    WriteCmd.OpCode = USB_BOOT_WRITE10_OPCODE;
    WriteCmd.Lun    = USB_BOOT_LUN (UsbMass->Lun);
    UsbBootPutUint32 (WriteCmd.Lba, Lba);
    UsbBootPutUint16 (WriteCmd.TransferLen, Count);

    Status = UsbBootExecCmd (
               UsbMass,
               &WriteCmd,
               sizeof (USB_BOOT_WRITE10_CMD),
               EfiUsbDataOut,
               Buffer,
               ByteSize,
               Timeout
               );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    Lba        += Count;
    Buffer     += Count * BlockSize;
    TotalBlock -= Count;
  }

  return Status;
}
