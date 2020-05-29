/** @file
  BOT Transportation implementation

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

#include "UsbMassStoragePei.h"

#pragma pack(1)
//
// Bulk Only device protocol
//
typedef struct {
  UINT32        dCBWSignature;
  UINT32        dCBWTag;
  UINT32        dCBWDataTransferLength;
  UINT8         bmCBWFlags;
  UINT8         bCBWLUN;
  UINT8         bCBWCBLength;
  UINT8         CBWCB[16];
} CBW;

typedef struct {
  UINT32        dCSWSignature;
  UINT32        dCSWTag;
  UINT32        dCSWDataResidue;
  UINT8         bCSWStatus;
} CSW;

#pragma pack()
//
// Status code, see Usb Bot device spec
//
#define CSWSIG  0x53425355
#define CBWSIG  0x43425355

/**

  Reset the given usb device

  @param  PeiServices           General purpose services that are available to every PEIM.
  @param  PeiBlkIoDev           Indicates the PEI_BLKIO_DEVICE instance.

  @retval EFI_SUCCESS           Failed to reset the given usb device.
  @retval EFI_INVALID_PARAMETER Can not get usb io ppi.

**/
STATIC
EFI_STATUS
BotRecoveryReset (
  IN  EFI_PEI_SERVICES          **PeiServices,
  IN  PEI_BLKIO_DEVICE          *PeiBlkIoDev
  )
{
  EFI_USB_DEVICE_REQUEST  DevReq;
  UINT32                  Timeout;
  PEI_USB3_IO_PPI         *UsbIoPpi;
  UINT8                   EndpointAddr;
  EFI_STATUS              Status;

  UsbIoPpi = PeiBlkIoDev->UsbIoPpi;

  if (UsbIoPpi == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem (&DevReq, sizeof (EFI_USB_DEVICE_REQUEST));

  DevReq.RequestType  = 0x21;
  DevReq.Request      = 0xFF;
  DevReq.Value        = 0;
  DevReq.Index        = 0;
  DevReq.Length       = 0;

  Timeout             = 3000;

  Status = UsbIoPpi->UsbControlTransfer (
                       PeiServices,
                       UsbIoPpi,
                       &DevReq,
                       EfiUsbNoData,
                       Timeout,
                       NULL,
                       0
                       );

  //
  // clear bulk in endpoint stall feature
  //
  EndpointAddr = (PeiBlkIoDev->BulkInEndpoint)->EndpointAddress;
  UsbIoPpi->UsbClearEndpointHalt (PeiServices, UsbIoPpi, EndpointAddr);

  //
  // clear bulk out endpoint stall feature
  //
  EndpointAddr = (PeiBlkIoDev->BulkOutEndpoint)->EndpointAddress;
  UsbIoPpi->UsbClearEndpointHalt (PeiServices, UsbIoPpi, EndpointAddr);

  return Status;
}

/**

  Send the command to the device using Bulk-Out endpoint.

  @param  PeiServices           General purpose services that are available to every PEIM.
  @param  PeiBlkIoDev           Indicates the PEI_BLKIO_DEVICE instance.
  @param  Command               The command to transfer to device.
  @param  CommandSize           The length of the command.
  @param  DataTransferLength    The expected length of the data.
  @param  Direction             The direction of the data.
  @param  Timeout               Indicates the maximum time, in millisecond, which the transfer is allowed to complete.

  @retval EFI_SUCCESS           Successful to send the command to device.
  @retval EFI_DEVICE_ERROR      Failed to send the command to device.

**/
STATIC
EFI_STATUS
BotCommandPhase (
  IN  EFI_PEI_SERVICES          **PeiServices,
  IN  PEI_BLKIO_DEVICE          *PeiBlkIoDev,
  IN  VOID                      *Command,
  IN  UINT8                     CommandSize,
  IN  UINT32                    DataTransferLength,
  IN  EFI_USB_DATA_DIRECTION    Direction,
  IN  UINT16                    Timeout
  )
{
  CBW             cbw;
  EFI_STATUS      Status;
  PEI_USB3_IO_PPI *UsbIoPpi;
  UINTN           DataSize;

  UsbIoPpi = PeiBlkIoDev->UsbIoPpi;

  ZeroMem (&cbw, sizeof (CBW));

  //
  // Fill the command block, detailed see BOT spec
  //
  cbw.dCBWSignature           = CBWSIG;
  cbw.dCBWTag                 = PeiBlkIoDev->CbwTag;
  cbw.dCBWDataTransferLength  = DataTransferLength;
  cbw.bmCBWFlags              = (UINT8) ((Direction == EfiUsbDataIn) ? 0x80 : 0);
  cbw.bCBWLUN                 = 0;
  cbw.bCBWCBLength            = CommandSize;

  CopyMem (cbw.CBWCB, Command, CommandSize);

  DataSize = sizeof (CBW);

  Status = UsbIoPpi->UsbBulkTransfer (
                       PeiServices,
                       UsbIoPpi,
                       (PeiBlkIoDev->BulkOutEndpoint)->EndpointAddress,
                       (UINT8 *) &cbw,
                       &DataSize,
                       Timeout
                       );
  if (EFI_ERROR (Status)) {
    //
    // Command phase fail, we need to recovery reset this device
    //
    BotRecoveryReset (PeiServices, PeiBlkIoDev);
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

/**

  Transfer the data between the device and host.

  @param  PeiServices           General purpose services that are available to every PEIM.
  @param  PeiBlkIoDev           Indicates the PEI_BLKIO_DEVICE instance.
  @param  DataSize              The length of the data.
  @param  DataBuffer            The pointer to the data.
  @param  Direction             The direction of the data.
  @param  Timeout               Indicates the maximum time, in millisecond, which the transfer is allowed to complete.

  @retval EFI_SUCCESS           Successful to send the data to device.
  @retval EFI_DEVICE_ERROR      Failed to send the data to device.

**/
STATIC
EFI_STATUS
BotDataPhase (
  IN  EFI_PEI_SERVICES          **PeiServices,
  IN  PEI_BLKIO_DEVICE          *PeiBlkIoDev,
  IN  UINT32                    *DataSize,
  IN  OUT VOID                  *DataBuffer,
  IN  EFI_USB_DATA_DIRECTION    Direction,
  IN  UINT16                    Timeout
  )
{
  EFI_STATUS      Status;
  PEI_USB3_IO_PPI *UsbIoPpi;
  UINT8           EndpointAddr;
  UINTN           Remain;
  UINTN           Increment;
  UINT32          MaxPacketLen;
  UINT8           *BufferPtr;
  UINTN           TransferredSize;

  UsbIoPpi        = PeiBlkIoDev->UsbIoPpi;

  Remain          = *DataSize;
  BufferPtr       = (UINT8 *) DataBuffer;
  TransferredSize = 0;

  //
  // retrieve the the max packet length of the given endpoint
  //
  if (Direction == EfiUsbDataIn) {
    MaxPacketLen  = (PeiBlkIoDev->BulkInEndpoint)->MaxPacketSize;
    EndpointAddr  = (PeiBlkIoDev->BulkInEndpoint)->EndpointAddress;
  } else {
    MaxPacketLen  = (PeiBlkIoDev->BulkOutEndpoint)->MaxPacketSize;
    EndpointAddr  = (PeiBlkIoDev->BulkOutEndpoint)->EndpointAddress;
  }

  while (Remain > 0) {
    //
    // Using 15 packets to avoid Bitstuff error
    //
    if (Remain > 16 * MaxPacketLen) {
      Increment = 16 * MaxPacketLen;
    } else {
      Increment = Remain;
    }

    Status = UsbIoPpi->UsbBulkTransfer (
                         PeiServices,
                         UsbIoPpi,
                         EndpointAddr,
                         BufferPtr,
                         &Increment,
                         Timeout
                         );

    TransferredSize += Increment;

    if (Status == EFI_DEVICE_ERROR || (Status == EFI_TIMEOUT && PeiBlkIoDev->BypassCswTimeout)) {
      UsbIoPpi->UsbClearEndpointHalt (PeiServices, UsbIoPpi, EndpointAddr);
      if (PeiBlkIoDev->BypassCswTimeout) {
        if (PeiBlkIoDev->BypassCswTimeout == 1) {
          return EFI_NOT_FOUND;
        }
        continue;
      } else return Status;
    }

    BufferPtr += Increment;
    Remain -= Increment;
  }

  *DataSize = (UINT32) TransferredSize;

  return EFI_SUCCESS;
}

/**

  Get the command execution status from device.

  @param  PeiServices           General purpose services that are available to every PEIM.
  @param  PeiBlkIoDev           Indicates the PEI_BLKIO_DEVICE instance.
  @param  TransferStatus        The status of the transaction.
  @param  Timeout               Indicates the maximum time, in millisecond, which the transfer is allowed to complete.

  @retval EFI_SUCCESS           Successful to get the status of device.
  @retval EFI_DEVICE_ERROR      Failed to get the status of device.

**/
STATIC
EFI_STATUS
BotStatusPhase (
  IN  EFI_PEI_SERVICES          **PeiServices,
  IN  PEI_BLKIO_DEVICE            *PeiBlkIoDev,
  OUT UINT8                     *TransferStatus,
  IN  UINT16                    Timeout
  )
{
  CSW             csw;
  EFI_STATUS      Status;
  PEI_USB3_IO_PPI *UsbIoPpi;
  UINT8           EndpointAddr;
  UINTN           DataSize;

  UsbIoPpi = PeiBlkIoDev->UsbIoPpi;

  ZeroMem (&csw, sizeof (CSW));

  EndpointAddr  = (PeiBlkIoDev->BulkInEndpoint)->EndpointAddress;

Retry:
  DataSize      = sizeof (CSW);

  //
  // Get the status field from bulk transfer
  //
  Status = UsbIoPpi->UsbBulkTransfer (
                       PeiServices,
                       UsbIoPpi,
                       EndpointAddr,
                       &csw,
                       &DataSize,
                       Timeout
                       );
  if (Status == EFI_DEVICE_ERROR || (Status == EFI_TIMEOUT && PeiBlkIoDev->BypassCswTimeout)) {
    UsbIoPpi->UsbClearEndpointHalt (PeiServices, UsbIoPpi, EndpointAddr);
    if (PeiBlkIoDev->BypassCswTimeout) {
      if (PeiBlkIoDev->BypassCswTimeout == 1) {
        return EFI_NOT_FOUND;
      }
      goto Retry;
    } else return Status;
  }
  //
  // Following situations will be treated as valid CSW
  // 1. Signature not equal to "USBS" but the tag equal to CBW's tag
  // 2. Both of signature and tag equal to zero
  //
  if (csw.dCSWTag != PeiBlkIoDev->CbwTag && !(csw.dCSWSignature == 0 && csw.dCSWTag == 0)) {
    return EFI_DEVICE_ERROR;
  } else {
    *TransferStatus = csw.bCSWStatus;    
  }
  //
  //The tag is increased even there is an error.
  //
  PeiBlkIoDev->CbwTag++;
  return EFI_SUCCESS;
}

/**

  Send ATAPI command using BOT protocol.

  @param  PeiServices           General purpose services that are available to every PEIM.
  @param  PeiBlkIoDev           Indicates the PEI_BLKIO_DEVICE instance.
  @param  Command               The command to be sent to ATAPI device.
  @param  CommandSize           The length of the data to be sent.
  @param  DataBuffer            The pointer to the data.
  @param  BufferLength          The length of the data.
  @param  Direction             The direction of the data.
  @param  TimeOutInMilliSeconds Indicates the maximum time, in millisecond, which the transfer is allowed to complete.

  @retval EFI_SUCCESS           Command executed successfully.
  @retval EFI_DEVICE_ERROR      Some device errors happen.

**/
EFI_STATUS
PeiBotAtapiCommand (
  IN  EFI_PEI_SERVICES            **PeiServices,
  IN  PEI_BLKIO_DEVICE            *PeiBlkIoDev,
  IN  VOID                        *Command,
  IN  UINT8                       CommandSize,
  IN  VOID                        *DataBuffer,
  IN  UINT32                      BufferLength,
  IN  EFI_USB_DATA_DIRECTION      Direction,
  IN  UINT16                      TimeOutInMilliSeconds
  )
{
  EFI_STATUS  Status;
  EFI_STATUS  BotDataStatus;
  UINT8       TransferStatus;
  UINT32      BufferSize;

  BotDataStatus  = EFI_SUCCESS;
  TransferStatus = 0;
  //
  // First send ATAPI command through Bot
  //
  Status = BotCommandPhase (
             PeiServices,
             PeiBlkIoDev,
             Command,
             CommandSize,
             BufferLength,
             Direction,
             TimeOutInMilliSeconds
             );

  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }
  //
  // Send/Get Data if there is a Data Stage
  //
  switch (Direction) {
  case EfiUsbDataIn:
  case EfiUsbDataOut:
    //
    // Some devices needs extremely long stall on first Read10 command when
    // the SATA "Standby Immediately" been set to the device
    //
    if (!PeiBlkIoDev->FirstReadPassed && *(UINT8*)Command == 0x28) {
      if (TimeOutInMilliSeconds < USB_BOT_DATA_TIMEOUT) TimeOutInMilliSeconds = USB_BOT_DATA_TIMEOUT;
      PeiBlkIoDev->FirstReadPassed = TRUE;
    }
    //
    // Some ODD devices needs extremely long stall on first Read Capacity command
    //
    if (!PeiBlkIoDev->FirstReadCapPassed && *(UINT8*)Command == 0x25) {
      if (TimeOutInMilliSeconds < USB_BOT_DATA_TIMEOUT) TimeOutInMilliSeconds = USB_BOT_DATA_TIMEOUT;
      PeiBlkIoDev->FirstReadCapPassed = TRUE;
    }
    //
    // Some devices needs extremely long stall on first RequestSense command when
    // last command got Command Failed result
    //
    if (!PeiBlkIoDev->FirstRequestSensePassed && *(UINT8*)Command == 0x03) {
      if (TimeOutInMilliSeconds < USB_BOT_DATA_TIMEOUT) TimeOutInMilliSeconds = USB_BOT_DATA_TIMEOUT;
      PeiBlkIoDev->FirstRequestSensePassed = TRUE;
    }
    //
    // Some devices needs extremely long stall on first Inquiry command
    //
    if (!PeiBlkIoDev->FirstInquiryPassed && *(UINT8*)Command == 0x12 && !PeiBlkIoDev->BypassCswTimeout) {
      if (TimeOutInMilliSeconds < USB_BOT_DATA_TIMEOUT) TimeOutInMilliSeconds = USB_BOT_DATA_TIMEOUT;
      PeiBlkIoDev->FirstInquiryPassed = TRUE;
    }

    BufferSize = BufferLength;

    BotDataStatus = BotDataPhase (
                      PeiServices,
                      PeiBlkIoDev,
                      &BufferSize,
                      DataBuffer,
                      Direction,
                      TimeOutInMilliSeconds
                      );
    break;

  case EfiUsbNoData:
    break;
  }
  //
  // Some devices needs extremely long stall on first BOT command to waiting
  // for firmware startup during cold boot
  //
  TimeOutInMilliSeconds = USB_BOT_CSW_TIMEOUT;
  if (PeiBlkIoDev->CbwTag == 1 && !PeiBlkIoDev->BypassCswTimeout) {
    TimeOutInMilliSeconds = USB_BOT_CSW_TIMEOUT * 2;
  }
  //
  // Status Phase
  //
  Status = BotStatusPhase (
             PeiServices,
             PeiBlkIoDev,
             &TransferStatus,
             TimeOutInMilliSeconds
             );
  if (EFI_ERROR (Status)) {
    BotRecoveryReset (PeiServices, PeiBlkIoDev);
    return EFI_DEVICE_ERROR;
  }
  if (TransferStatus == 0x01) {
    return EFI_DEVICE_ERROR;
  }
  return BotDataStatus;
}
