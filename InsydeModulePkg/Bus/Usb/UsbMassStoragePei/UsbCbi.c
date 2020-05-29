/** @file
  CBI Transportation implementation

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

typedef struct
{
  UINT8         bType;
  UINT8         bValue;
} INTERRUPT_DATA_BLOCK;

/**

  Send the command to the device using control endpoint.

  @param  PeiServices           General purpose services that are available to every PEIM.
  @param  PeiBlkIoDev           Indicates the PEI_BLKIO_DEVICE instance.
  @param  Command               The command to transfer to device.
  @param  CommandSize           The length of the command.
  @param  Timeout               Indicates the maximum time, in millisecond, which the transfer is allowed to complete.

  @retval EFI_SUCCESS           Successful to send the command to device.
  @retval EFI_DEVICE_ERROR      Failed to send the command to device.

**/
STATIC
EFI_STATUS
CbiCommandPhase (
  IN  EFI_PEI_SERVICES          **PeiServices,
  IN  PEI_BLKIO_DEVICE          *PeiBlkIoDev,
  IN  VOID                      *Command,
  IN  UINT8                     CommandSize,
  IN  UINT16                    Timeout
)
{
  EFI_STATUS              Status;
  PEI_USB3_IO_PPI         *UsbIoPpi;
  EFI_USB_DEVICE_REQUEST  Request;

  UsbIoPpi = PeiBlkIoDev->UsbIoPpi;

  ZeroMem (&Request, sizeof(EFI_USB_DEVICE_REQUEST));

  //
  // Device request see CBI specification
  //
  Request.RequestType = 0x21;
  Request.Request = 0x00;
  Request.Value = 0 ;
  Request.Index = 0 ;
  Request.Length = CommandSize;

  Status = UsbIoPpi->UsbControlTransfer(
                       PeiServices,
                       UsbIoPpi,
                       &Request,
                       EfiUsbDataOut,
                       Timeout,
                       Command,
                       CommandSize
                       );

  if (EFI_ERROR(Status)) {
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
CbiDataPhase (
  IN  EFI_PEI_SERVICES          **PeiServices,
  IN  PEI_BLKIO_DEVICE          *PeiBlkIoDev,
  IN  UINT32                    *DataSize,
  IN  OUT VOID                  *DataBuffer,
  IN  EFI_USB_DATA_DIRECTION    Direction,
  IN  UINT16                    Timeout
)
{
  EFI_STATUS          Status;
  PEI_USB3_IO_PPI     *UsbIoPpi;
  UINT8               EndpointAddr;
  UINTN               Remain;
  UINTN               Increment;
  UINT32              MaxPacketLen;
  UINT8               *BufferPtr;
  UINTN               TransferredSize;

  UsbIoPpi = PeiBlkIoDev->UsbIoPpi;

  Remain    = *DataSize;
  BufferPtr = (UINT8 *)DataBuffer;
  TransferredSize = 0;

  //
  // retrieve the the max packet length of the given endpoint
  //
  if ( Direction == EfiUsbDataIn ) {
    MaxPacketLen = (PeiBlkIoDev->BulkInEndpoint)->MaxPacketSize;
    EndpointAddr = (PeiBlkIoDev->BulkInEndpoint)->EndpointAddress;
  } else {
    MaxPacketLen = (PeiBlkIoDev->BulkOutEndpoint)->MaxPacketSize;
    EndpointAddr = (PeiBlkIoDev->BulkOutEndpoint)->EndpointAddress;
  }

  while ( Remain > 0 ) {
    //
    // Using 15 packets to avoid Bitstuff error
    //
    if ( Remain > 16 * MaxPacketLen) {
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

    if (EFI_ERROR (Status)) {
      UsbIoPpi->UsbClearEndpointHalt (PeiServices, UsbIoPpi, EndpointAddr);
      return Status;
    }

    BufferPtr += Increment;
    Remain -= Increment;
  }

  *DataSize = TransferredSize;

  return EFI_SUCCESS;
}

/**

  Get the command execution status from device.

  @param  PeiServices           General purpose services that are available to every PEIM.
  @param  PeiBlkIoDev           Indicates the PEI_BLKIO_DEVICE instance.
  @param  InterruptDataBlock    The status of the transaction.
  @param  Timeout               Indicates the maximum time, in millisecond, which the transfer is allowed to complete.

  @retval EFI_SUCCESS           Successful to get the status of device.
  @retval EFI_DEVICE_ERROR      Failed to get the status of device.

**/
STATIC
EFI_STATUS
CbiStatusPhase (
  IN  EFI_PEI_SERVICES          **PeiServices,
  IN  PEI_BLKIO_DEVICE          *PeiBlkIoDev,
  OUT INTERRUPT_DATA_BLOCK      *InterruptDataBlock,
  IN  UINT16                    Timeout
)
{
  EFI_STATUS              Status;
  PEI_USB3_IO_PPI         *UsbIoPpi;
  UINT8                   EndpointAddr;
  UINTN                   InterruptDataBlockLength;

  UsbIoPpi = PeiBlkIoDev->UsbIoPpi;

  ZeroMem (InterruptDataBlock,sizeof (INTERRUPT_DATA_BLOCK));

  EndpointAddr = (PeiBlkIoDev->InterruptEndpoint)->EndpointAddress;

  InterruptDataBlockLength = sizeof(INTERRUPT_DATA_BLOCK);

  //
  // Get the status field from Interrupt transfer
  //
  Status = UsbIoPpi->UsbSyncInterruptTransfer(
                       PeiServices,
                       UsbIoPpi,
                       EndpointAddr,
                       InterruptDataBlock,
                       &InterruptDataBlockLength,
                       Timeout
                       );
  if (EFI_ERROR(Status)) {
    UsbIoPpi->UsbClearEndpointHalt (PeiServices, UsbIoPpi, EndpointAddr);
    PeiBlkIoDev->StallPpi->Stall (
                             (CONST EFI_PEI_SERVICES **)PeiServices,
                             PeiBlkIoDev->StallPpi,
                             10000
                             );
    return Status;
  }

  return EFI_SUCCESS;
}

/**

  Send ATAPI command using CBI protocol.

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
PeiCbiAtapiCommand (
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
  EFI_STATUS              Status;
  EFI_STATUS              CbiDataStatus = EFI_SUCCESS;
  UINT32                  BufferSize;
  INTERRUPT_DATA_BLOCK    InterruptDataBlock;
  //
  // First send ATAPI command through Bot
  //
  Status = CbiCommandPhase(
             PeiServices,
             PeiBlkIoDev,
             Command,
             CommandSize,
             TimeOutInMilliSeconds
             );

  if(EFI_ERROR(Status)) {
    return EFI_DEVICE_ERROR;
  }
  //
  // Send/Get Data if there is a Data Stage
  //
  switch (Direction)
  {
  case EfiUsbDataIn:
  case EfiUsbDataOut:
    BufferSize = BufferLength;

    CbiDataStatus = CbiDataPhase(
                      PeiServices,
                      PeiBlkIoDev,
                      &BufferSize,
                      DataBuffer,
                      Direction,
                      TimeOutInMilliSeconds
                      );
    break;

  case EfiUsbNoData:
    break ;
  }
  if(EFI_ERROR(CbiDataStatus)) {
    return EFI_DEVICE_ERROR;
  }
  //
  // Status Phase
  //
  Status = CbiStatusPhase(
             PeiServices,
             PeiBlkIoDev,
             &InterruptDataBlock,
             TimeOutInMilliSeconds
             );
  if (EFI_ERROR(Status)) {
    return EFI_DEVICE_ERROR;
  }
  return Status;
}