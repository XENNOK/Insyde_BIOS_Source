/** @file
  Implementation of the USB mass storage Control/Bulk/Interrupt transpor.
  Notice: it is being obseleted by the standard body in favor of the BOT
  (Bulk-Only Transport).

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

#include "UsbMassCbi.h"

#ifndef MDEPKG_NDEBUG
UINTN mUsbCbiInfo  = EFI_D_INFO;
UINTN mUsbCbiError = EFI_D_ERROR;
#endif

//
// Function prototypes
//
STATIC
EFI_STATUS
UsbCbi0Init (
  IN  EFI_USB_IO_PROTOCOL     *UsbIo,
  IN  EFI_HANDLE              Controller,
  OUT VOID                    **Context
  );

STATIC
EFI_STATUS
UsbCbi1Init (
  IN  EFI_USB_IO_PROTOCOL     *UsbIo,
  IN  EFI_HANDLE              Controller,
  OUT VOID                    **Context
  );

EFI_STATUS
EFIAPI
UsbMassBotDriverEntryPoint (
  IN EFI_HANDLE               ImageHandle,
  IN EFI_SYSTEM_TABLE         *SystemTable
  );

STATIC EFI_GUID                         mUsbMassTransportProtocolGuid = USB_MASS_TRANSPORT_PROTOCOL_GUID;
STATIC EFI_USB_CORE_PROTOCOL            *mUsbCore;
STATIC USB_MASS_TRANSPORT_PROTOCOL      *mUsbCbi0Transport;
STATIC USB_MASS_TRANSPORT_PROTOCOL      *mUsbCbi1Transport;

/**

  Register protocol for this driver.

  @param  ImageHandle           EFI_HANDLE
  @param  SystemTable           EFI_SYSTEM_TABLE pointer

  @retval EFI_SUCCESS           Driver loaded
  @retval other                 Driver not loaded

**/
EFI_STATUS
EFIAPI
UsbMassCbiDriverEntryPoint (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS                    Status;
  BOOLEAN                       InSmram;
  EFI_HANDLE                    Handle;
  UINTN                         Index;
  UINTN                         HandleCount;
  EFI_HANDLE                    *HandleBuffer;
  USB_MASS_TRANSPORT_PROTOCOL   *Transport;
  //
  // Locate UsbCore protocol
  //
  Status = gBS->LocateProtocol (
                  &gEfiUsbCoreProtocolGuid,
                  NULL,
                  (VOID **)&mUsbCore
                  );
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }
  //
  // Check the phase of instance
  //
  mUsbCore->IsInSmm (&InSmram);
  if (!InSmram) {
    //
    // Now in boot service, install instance
    //
    Status = mUsbCore->AllocateBuffer (
                         sizeof (USB_MASS_TRANSPORT_PROTOCOL) * 2,
                         ALIGNMENT_32,
                         (VOID **)&mUsbCbi0Transport
                         );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    mUsbCbi0Transport->Protocol    = USB_MASS_STORE_CBI0;
    mUsbCbi0Transport->Init        = UsbCbi0Init;
    mUsbCbi0Transport->ExecCommand = UsbCbiExecCommand;
    mUsbCbi0Transport->Reset       = UsbCbiResetDevice;
    mUsbCbi0Transport->Fini        = UsbCbiFini;
    mUsbCbi1Transport              = mUsbCbi0Transport + 1;
    mUsbCbi1Transport->Protocol    = USB_MASS_STORE_CBI1;
    mUsbCbi1Transport->Init        = UsbCbi1Init;
    mUsbCbi1Transport->ExecCommand = UsbCbiExecCommand;
    mUsbCbi1Transport->Reset       = UsbCbiResetDevice;
    mUsbCbi1Transport->Fini        = UsbCbiFini;
    //
    // Pass in a NULL to install to a new handle
    //
    Handle = NULL;
    Status = gBS->InstallProtocolInterface (
                    &Handle,
                    &mUsbMassTransportProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    mUsbCbi0Transport
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    Handle = NULL;
    Status = gBS->InstallProtocolInterface (
                    &Handle,
                    &mUsbMassTransportProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    mUsbCbi1Transport
                    );
    //
    // Register module in DXE instance
    //
    mUsbCore->ModuleRegistration (ImageHandle);
  } else {
    //
    // SMM instance, disable DEBUG message out
    //
#ifndef MDEPKG_NDEBUG
    mUsbCbiInfo         = 0;
    mUsbCbiError        = 0;
#endif
    //
    // Now in SMM, get instance installed by boot service
    //
    Status = gBS->LocateHandleBuffer (
                    ByProtocol,
                    &mUsbMassTransportProtocolGuid,
                    NULL,
                    &HandleCount,
                    &HandleBuffer
                    );
    if (EFI_ERROR(Status)) {
      return Status;
    }
    Transport = NULL;
    for (Index = 0; Index < HandleCount; Index ++) {
      Status = gBS->HandleProtocol (
                      HandleBuffer[Index],
                      &mUsbMassTransportProtocolGuid,
                      (VOID **)&Transport
                      );
      if (EFI_ERROR(Status)) {
        continue;
      }
      //
      // Setup appropriate variable in the Smm region
      //
      if (Transport->Protocol == USB_MASS_STORE_CBI0) {
        mUsbCbi0Transport = Transport;
      } else if (Transport->Protocol == USB_MASS_STORE_CBI1) {
        mUsbCbi1Transport = Transport;
      }
    }
    gBS->FreePool (HandleBuffer);
    //
    // Register module in SMM instance
    //
    mUsbCore->ModuleRegistration (ImageHandle);
    //
    // Setup Smm address convert table for Smm security policy
    //
    mUsbCore->InsertAddressConvertTable (
                ACT_FUNCTION_POINTER,
                &mUsbCbi0Transport->Init,
                4
                );
    mUsbCore->InsertAddressConvertTable (
                ACT_FUNCTION_POINTER,
                &mUsbCbi1Transport->Init,
                4
                );
    mUsbCore->InsertAddressConvertTable (
                ACT_INSTANCE_BODY,
                mUsbCbi0Transport,
                sizeof (USB_MASS_TRANSPORT_PROTOCOL) * 2
                );
    mUsbCore->InsertAddressConvertTable (
                ACT_INSTANCE_POINTER,
                &mUsbCbi0Transport,
                1
                );
    mUsbCore->InsertAddressConvertTable (
                ACT_INSTANCE_POINTER,
                &mUsbCbi1Transport,
                1
                );
    mUsbCore->InsertAddressConvertTable (
                ACT_INSTANCE_POINTER,
                &mUsbCore,
                1
                );
  }
  return Status;
}

/**

  Initialize the USB mass storage class CBI transport protocol.
  If Context isn't NULL, it will save its context in it.

  @param  UsbIo                 The USB IO to use
  @param  Controller            The device controller
  @param  Context               The variable to save context in

  @retval EFI_OUT_OF_RESOURCES  Failed to allocate memory
  @retval EFI_UNSUPPORTED       The device isn't supported
  @retval EFI_SUCCESS           The CBI protocol is initialized.

**/
STATIC
EFI_STATUS
UsbCbiInit (
  IN  UINT8                 InterfaceProtocol,
  IN  EFI_USB_IO_PROTOCOL   *UsbIo,
  IN  EFI_HANDLE            Controller,
  OUT VOID                  **Context       OPTIONAL
  )
{
  USB_CBI_PROTOCOL              *UsbCbi;
  EFI_USB_INTERFACE_DESCRIPTOR  *Interface;
  EFI_USB_ENDPOINT_DESCRIPTOR   EndPoint;
  EFI_STATUS                    Status;
  UINT8                         Index;
 
  //
  // Allocate the CBI context
  //
  Status = mUsbCore->AllocateBuffer(
                       sizeof (USB_CBI_PROTOCOL) + 3 * sizeof (EFI_USB_ENDPOINT_DESCRIPTOR),
                       ALIGNMENT_32,
                       (VOID **)&UsbCbi
                       );

  if (UsbCbi == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  UsbCbi->UsbIo   = UsbIo;
  UsbCbi->UsbCore = mUsbCore;

  //
  // Get the interface descriptor and validate that it is a USB mass
  // storage class CBI interface.
  //
  Status = UsbIo->UsbGetInterfaceDescriptor (UsbIo, &UsbCbi->Interface);
  if (EFI_ERROR (Status)) {
    goto ON_ERROR;
  }

  Interface = &UsbCbi->Interface;
  //
  // Locate and save the bulk-in, bulk-out, and interrupt endpoint
  //
  for (Index = 0; Index < Interface->NumEndpoints; Index++) {
    Status = UsbIo->UsbGetEndpointDescriptor (UsbIo, Index, &EndPoint);
    if (EFI_ERROR (Status)) {
      continue;
    }

    if (USB_IS_BULK_ENDPOINT (EndPoint.Attributes)) {
      //
      // Use the first Bulk-In and Bulk-Out endpoints
      //
      if (USB_IS_IN_ENDPOINT (EndPoint.EndpointAddress) && 
         (UsbCbi->BulkInEndpoint == NULL)) {

        UsbCbi->BulkInEndpoint  = (EFI_USB_ENDPOINT_DESCRIPTOR *) (UsbCbi + 1);
        *UsbCbi->BulkInEndpoint = EndPoint;
      }

      if (USB_IS_OUT_ENDPOINT (EndPoint.EndpointAddress) && 
         (UsbCbi->BulkOutEndpoint == NULL)) {

        UsbCbi->BulkOutEndpoint   = (EFI_USB_ENDPOINT_DESCRIPTOR *) (UsbCbi + 1) + 1;
        *UsbCbi->BulkOutEndpoint  = EndPoint;
      }

    } else if (USB_IS_INTERRUPT_ENDPOINT (EndPoint.Attributes)) {
      //
      // Use the first interrupt endpoint if it is CBI0
      //
      if ((InterfaceProtocol == USB_MASS_STORE_CBI0) && 
          (UsbCbi->InterruptEndpoint == NULL)) {

        UsbCbi->InterruptEndpoint   = (EFI_USB_ENDPOINT_DESCRIPTOR *) (UsbCbi + 1) + 2;
        *UsbCbi->InterruptEndpoint  = EndPoint;
      }
    }
  }

  if ((UsbCbi->BulkInEndpoint == NULL) 
      || (UsbCbi->BulkOutEndpoint == NULL) 
      || ((InterfaceProtocol == USB_MASS_STORE_CBI0) 
          && (UsbCbi->InterruptEndpoint == NULL))) {
    Status = EFI_UNSUPPORTED;
    goto ON_ERROR;
  }

  if (Context != NULL) {
    //
    // Setup the data for UsbLegacy
    //
    CopyMem(
      &UsbCbi->Transport,
      (InterfaceProtocol == USB_MASS_STORE_CBI0) ? mUsbCbi0Transport : mUsbCbi0Transport,
      sizeof(USB_MASS_TRANSPORT_PROTOCOL)
      );
    //
    // Setup Smm address convert table for Smm security policy
    //
    mUsbCore->InsertAddressConvertTable (
                ACT_FUNCTION_POINTER,
                &UsbCbi->Transport.Init,
                4
                );
    mUsbCore->InsertAddressConvertTable (
                ACT_INSTANCE_BODY,
                UsbCbi,
                sizeof (USB_CBI_PROTOCOL) + 3 * sizeof (EFI_USB_ENDPOINT_DESCRIPTOR)
                );
    mUsbCore->InsertAddressConvertTable (
                ACT_INSTANCE_POINTER,
                &UsbCbi->UsbCore,
                1
                );
    mUsbCore->InsertAddressConvertTable (
                ACT_INSTANCE_POINTER,
                &UsbCbi->BulkInEndpoint,
                1
                );
    mUsbCore->InsertAddressConvertTable (
                ACT_INSTANCE_POINTER,
                &UsbCbi->BulkOutEndpoint,
                1
                );
    mUsbCore->InsertAddressConvertTable (
                ACT_INSTANCE_POINTER,
                &UsbCbi->InterruptEndpoint,
                1
                );
    mUsbCore->InsertAddressConvertTable (
                ACT_INSTANCE_POINTER,
                &UsbCbi->UsbIo,
                1
                );
    *Context = UsbCbi;
  } else {
    //
    // Free the instance due to the calling purpose is just to check is the device matches the protocol
    //
    Status = EFI_SUCCESS;
    goto ON_ERROR;
  }
  return EFI_SUCCESS;

ON_ERROR:
  mUsbCore->FreeBuffer (
              sizeof (USB_CBI_PROTOCOL) + 3 * sizeof (EFI_USB_ENDPOINT_DESCRIPTOR),
              UsbCbi
              );
  return Status;
}

/**

  Initialize the USB mass storage class CBI transport protocol.
  If Context isn't NULL, it will save its context in it.

  @param  UsbIo                 The USB IO to use
  @param  Controller            The device controller
  @param  Context               The variable to save context in

  @retval EFI_OUT_OF_RESOURCES  Failed to allocate memory
  @retval EFI_UNSUPPORTED       The device isn't supported
  @retval EFI_SUCCESS           The CBI protocol is initialized.

**/
STATIC
EFI_STATUS
UsbCbi0Init (
  IN  EFI_USB_IO_PROTOCOL   *UsbIo,
  IN  EFI_HANDLE            Controller,
  OUT VOID                  **Context       OPTIONAL
  )
{
  return UsbCbiInit(
           USB_MASS_STORE_CBI0,
           UsbIo,
           Controller,
           Context
           );
}

/**

  Initialize the USB mass storage class CBI transport protocol.
  If Context isn't NULL, it will save its context in it.

  @param  UsbIo                 The USB IO to use
  @param  Controller            The device controller
  @param  Context               The variable to save context in

  @retval EFI_OUT_OF_RESOURCES  Failed to allocate memory
  @retval EFI_UNSUPPORTED       The device isn't supported
  @retval EFI_SUCCESS           The CBI protocol is initialized.

**/
STATIC
EFI_STATUS
UsbCbi1Init (
  IN  EFI_USB_IO_PROTOCOL   *UsbIo,
  IN  EFI_HANDLE            Controller,
  OUT VOID                  **Context       OPTIONAL
  )
{
  return UsbCbiInit(
           USB_MASS_STORE_CBI1,
           UsbIo,
           Controller,
           Context
           );
}

/**

  Send the command to the device using class specific control transfer.

  @param  UsbCbi                The USB CBI protocol
  @param  Cmd                   The high level command to transfer to device
  @param  CmdLen                The length of the command
  @param  Timeout               The time to wait the command to finish

  @retval EFI_SUCCESS           The command is transferred to device
  @retval Others                The command failed to transfer to device

**/
STATIC
EFI_STATUS
UsbCbiSendCommand (
  IN USB_CBI_PROTOCOL       *UsbCbi,
  IN UINT8                  *Cmd,
  IN UINT8                  CmdLen,
  IN UINT32                 Timeout
  )
{
  EFI_USB_DEVICE_REQUEST  Request;
  EFI_STATUS              Status;
  UINT32                  TransStatus;
  UINTN                   DataLen;
  INTN                    Retry;

  //
  // Fill in the device request, CBI use the "Accept Device-Specific
  // Cmd" (ADSC) class specific request to send commands
  //
  Request.RequestType = 0x21;
  Request.Request     = 0;
  Request.Value       = 0;
  Request.Index       = UsbCbi->Interface.InterfaceNumber;
  Request.Length      = CmdLen;

  Status              = EFI_SUCCESS;
  Timeout             = Timeout / USB_MASS_STALL_1_MS;

  for (Retry = 0; Retry < USB_CBI_MAX_RETRY; Retry++) {
    //
    // Check is device been detached
    //
    if (mUsbCore->CheckDeviceDetached(UsbCbi->UsbIo) == EFI_SUCCESS) {
      return EFI_NOT_FOUND;
    }
    //
    // Use the UsbIo to send the command to the device
    //
    TransStatus = 0;
    DataLen     = CmdLen;

    Status = UsbCbi->UsbIo->UsbControlTransfer (
                              UsbCbi->UsbIo,
                              &Request,
                              EfiUsbDataOut,
                              Timeout,
                              Cmd,
                              DataLen,
                              &TransStatus
                              );
    //
    // The device can fail the command by STALL the control endpoint.
    // It can delay the command by NAK the data or status stage, this
    // is a "class-specific exemption to the USB specification". Retry
    // if the command is NAKed.
    //
    if (EFI_ERROR (Status) && (TransStatus == EFI_USB_ERR_NAK)) {
      continue;
    }
    
    break;
  }

  return Status;
}

/**

  Transfer data between the device and host. The CBI contains three phase,
  command, data, and status. This is data phase.

  @param  UsbCbi                The USB CBI device
  @param  DataDir               The direction of the data transfer
  @param  Data                  The buffer to hold the data
  @param  TransLen              The expected transfer length
  @param  Timeout               The time to wait the command to execute

  @retval EFI_SUCCESS           The data transfer succeeded
  @retval Others                Failed to transfer all the data

**/
STATIC
EFI_STATUS
UsbCbiDataTransfer (
  IN USB_CBI_PROTOCOL         *UsbCbi,
  IN EFI_USB_DATA_DIRECTION   DataDir,
  IN OUT UINT8                *Data,
  IN OUT UINTN                *TransLen,
  IN UINT32                   Timeout
  )
{
  EFI_USB_ENDPOINT_DESCRIPTOR *Endpoint;
  EFI_STATUS                  Status;
  UINT32                      TransStatus;
  UINTN                       Remain;
  UINTN                       Increment;
  UINT8                       *Next;
  UINTN                       Retry;

  //
  // It's OK if no data to transfer
  //
  if ((DataDir == EfiUsbNoData) || (*TransLen == 0)) {
    //
    // Stall 0.5ms for specific device compatibility(0.3ms minimum)
    //
    UsbCbi->UsbCore->Stall(500);
    return EFI_SUCCESS;
  }
  
  //
  // Select the endpoint then issue the transfer
  //
  if (DataDir == EfiUsbDataIn) {
    Endpoint = UsbCbi->BulkInEndpoint;
  } else {
    Endpoint = UsbCbi->BulkOutEndpoint;
  }

  Next    = Data;
  Remain  = *TransLen;
  Retry   = 0;
  Status  = EFI_SUCCESS;
  Timeout = Timeout / USB_MASS_STALL_1_MS;

  //
  // Transfer the data, if the device returns NAK, retry it. 
  //
  while (Remain > 0) {
    //
    // Check is device been detached
    //
    if (mUsbCore->CheckDeviceDetached(UsbCbi->UsbIo) == EFI_SUCCESS) {
      return EFI_NOT_FOUND;
    }
    TransStatus = 0;

    if (Remain > (UINTN) USB_CBI_MAX_PACKET_NUM * Endpoint->MaxPacketSize) {
      Increment = USB_CBI_MAX_PACKET_NUM * Endpoint->MaxPacketSize;
    } else {
      Increment = Remain;
    }

    Status = UsbCbi->UsbIo->UsbBulkTransfer (
                              UsbCbi->UsbIo,
                              Endpoint->EndpointAddress,
                              Next,
                              &Increment,
                              Timeout,
                              &TransStatus
                              );
    if (EFI_ERROR (Status)) {
      if (TransStatus == EFI_USB_ERR_NAK) {
        //
        // The device can NAK the host if either the data/buffer isn't
        // aviable or the command is in-progress. The data can be partly
        // transferred. The transfer is aborted if several succssive data
        // transfer commands are NAKed.
        //
        if (Increment == 0) {
          if (++Retry > USB_CBI_MAX_RETRY) {
            goto ON_EXIT;
          }
          
        } else {
          Next   += Increment;
          Remain -= Increment;
          Retry   = 0;
        }

        continue;
      }
      
      //
      // The device can fail the command by STALL the bulk endpoint.
      // Clear the stall if that is the case.
      //
      if (TransStatus == EFI_USB_ERR_STALL) {
        UsbCbi->UsbCore->UsbClearDeviceFeature(
                           UsbCbi->UsbIo,
                           USB_TARGET_ENDPOINT,
                           USB_FEATURE_ENDPOINT_HALT,
                           Endpoint->EndpointAddress,
                           &TransStatus
                           );
      }

      goto ON_EXIT;
    }

    Next += Increment;
    Remain -= Increment;
  }

ON_EXIT:
  *TransLen -= Remain;
  return Status;
}

/**

  Get the result of high level command execution from interrupt
  endpoint. This function returns the USB transfer status, and 
  put the high level command execution result in Result.

  @param  UsbCbi                The USB CBI protocol
  @param  Timeout               The time to wait the command to execute
  @param  Result                Result status

  @retval EFI_SUCCESS           The high level command execution result is retrieved in Result.
  @retval Others                Failed to retrieve the result.

**/
STATIC
EFI_STATUS
UsbCbiGetStatus (
  IN  USB_CBI_PROTOCOL        *UsbCbi,
  IN  UINT32                  Timeout,
  OUT USB_CBI_STATUS          *Result
  )
{
  UINTN                     Len;
  UINT8                     Endpoint;
  EFI_STATUS                Status;
  UINT32                    TransStatus;
  INTN                      Retry;

  Endpoint  = UsbCbi->InterruptEndpoint->EndpointAddress;
  Status    = EFI_SUCCESS;
  Timeout   = Timeout / USB_MASS_STALL_1_MS;

  //
  // Attemp to the read the result from interrupt endpoint
  //
  for (Retry = 0; Retry < USB_CBI_MAX_RETRY; Retry++) {
    //
    // Check is device been detached
    //
    if (mUsbCore->CheckDeviceDetached(UsbCbi->UsbIo) == EFI_SUCCESS) {
      return EFI_NOT_FOUND;
    }
    TransStatus = 0;
    Len         = sizeof (USB_CBI_STATUS);
    
    Status = UsbCbi->UsbIo->UsbSyncInterruptTransfer (
                              UsbCbi->UsbIo,
                              Endpoint,
                              Result,
                              &Len,
                              Timeout,
                              &TransStatus
                              );
    //
    // The CBI can NAK the interrupt endpoint if the command is in-progress.
    //
    if (EFI_ERROR (Status) && (TransStatus == EFI_USB_ERR_NAK)) {
      continue;
    }

    break;
  }

  return Status;
}

/**

  Execute USB mass storage command through the CBI0/CBI1 transport protocol

  @param  Context               The USB CBI device
  @param  Cmd                   The command to transfer to device
  @param  CmdLen                The length of the command
  @param  DataDir               The direction of data transfer
  @param  Data                  The buffer to hold the data
  @param  DataLen               The length of the buffer
  @param  Timeout               The time to wait 
  @param  CmdStatus             The result of the command execution

  @retval EFI_SUCCESS           The command is executed OK and result in CmdStatus.
  @retval EFI_DEVICE_ERROR      Failed to execute the command

**/
STATIC
EFI_STATUS
UsbCbiExecCommand (
  IN  VOID                    *Context,
  IN  VOID                    *Cmd,
  IN  UINT8                   CmdLen,
  IN  EFI_USB_DATA_DIRECTION  DataDir,
  IN  VOID                    *Data,
  IN  UINT32                  DataLen,
  IN  UINT32                  Timeout,
  OUT UINT32                  *CmdStatus
  )
{
  USB_CBI_PROTOCOL          *UsbCbi;
  USB_CBI_STATUS            Result;
  EFI_STATUS                Status;
  UINTN                     TransLen;

  *CmdStatus  = USB_MASS_CMD_SUCCESS;
  UsbCbi      = (USB_CBI_PROTOCOL *) Context;

  //
  // Send the command to the device. Return immediately if device
  // rejects the command.
  //
  Status = UsbCbiSendCommand (UsbCbi, Cmd, CmdLen, Timeout);
  if (EFI_ERROR (Status)) {
    DEBUG ((mUsbCbiError, "UsbCbiExecCommand: UsbCbiSendCommand (%r)\n",Status));
    return Status;
  }
  
  //
  // Transfer the data, return this status if no interrupt endpoint
  // is used to report the transfer status.
  //
  TransLen = (UINTN) DataLen;
  
  Status   = UsbCbiDataTransfer (UsbCbi, DataDir, Data, &TransLen, Timeout);
  if (EFI_ERROR (Status) || UsbCbi->InterruptEndpoint == NULL) {
    DEBUG ((mUsbCbiError, "UsbCbiExecCommand: UsbCbiDataTransfer (%r)\n",Status));
    return Status;
  }
  
  if (UsbCbi->Transport.Protocol == USB_MASS_STORE_CBI1) {
    //
    // No completion interrupt required
    //
    return EFI_SUCCESS;
  }
  //
  // Get the status for CBI0, if that succeeds, interpret the result
  //
  Status = UsbCbiGetStatus (UsbCbi, Timeout, &Result);
  if (EFI_ERROR (Status)) {
    DEBUG ((mUsbCbiError, "UsbCbiExecCommand: UsbCbiGetStatus (%r)\n",Status));
    return EFI_DEVICE_ERROR;
  }

  if (UsbCbi->Interface.InterfaceSubClass == USB_MASS_STORE_UFI) {
    //
    // For UFI device, ASC and ASCQ are returned.
    // 
    if (Result.Type != 0) {
      *CmdStatus = USB_MASS_CMD_FAIL;
    }

  } else {
    //
    // Check page 27, CBI spec 1.1 for vaious reture status.
    //
    switch (Result.Value & 0x03) {
    case 0x00:
      //
      // Pass
      //
      *CmdStatus = USB_MASS_CMD_SUCCESS;
      break;

    case 0x02:
      //
      // Phase Error, response with reset. Fall through to Fail.
      //
      UsbCbiResetDevice (UsbCbi, FALSE);
      *CmdStatus = USB_MASS_CMD_PHASE_ERROR;
      break;
      
    case 0x01:
      //
      // Fail
      //
      *CmdStatus = USB_MASS_CMD_FAIL;
      break;

    case 0x03:
      //
      // Persistent Fail, need to send REQUEST SENSE.
      //
      *CmdStatus = USB_MASS_CMD_PERSISTENT;
      break;
    }
  }
  return EFI_SUCCESS;
}

/**

  Call the Usb mass storage class transport protocol to
  reset the device. The reset is defined as a Non-Data
  command. Don't use UsbCbiExecCommand to send the command
  to device because that may introduce recursive loop.

  @param  Context               The USB CBI device protocol

  @retval EFI_SUCCESS           the device is reset
  @retval Others                Failed to reset the device

**/
STATIC
EFI_STATUS
UsbCbiResetDevice (
  IN  VOID                    *Context,
  IN  BOOLEAN                  ExtendedVerification
  )
{
  UINT8                     ResetCmd[USB_CBI_RESET_CMD_LEN];
  USB_CBI_PROTOCOL          *UsbCbi;
  USB_CBI_STATUS            Result;
  EFI_STATUS                Status;
  UINT32                    Timeout;
  UINT32                    ClearResult;

  UsbCbi = (USB_CBI_PROTOCOL *) Context;

  //
  // Check is device been detached
  //
  if (mUsbCore->CheckDeviceDetached(UsbCbi->UsbIo) == EFI_SUCCESS) {
    return EFI_NOT_FOUND;
  }
  //
  // Fill in the reset command.
  //
  SetMem (ResetCmd, USB_CBI_RESET_CMD_LEN, 0xFF);

  ResetCmd[0] = 0x1D;
  ResetCmd[1] = 0x04;
  Timeout     = USB_CBI_RESET_TIMEOUT / USB_MASS_STALL_1_MS;

  //
  // Send the command to the device. Don't use UsbCbiExecCommand here.
  //
  Status = UsbCbiSendCommand (UsbCbi, ResetCmd, USB_CBI_RESET_CMD_LEN, Timeout);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  //
  // Just retrieve the status and ignore that. Then stall 
  // 50ms to wait it complete
  //
  UsbCbiGetStatus (UsbCbi, Timeout, &Result);
  UsbCbi->UsbCore->Stall (50 * 1000);

  //
  // Clear the Bulk-In and Bulk-Out stall condition and
  // init data toggle.
  //
  UsbCbi->UsbCore->UsbClearDeviceFeature(
                     UsbCbi->UsbIo,
                     USB_TARGET_ENDPOINT,
                     USB_FEATURE_ENDPOINT_HALT,
                     UsbCbi->BulkInEndpoint->EndpointAddress,
                     &ClearResult
                     );
  UsbCbi->UsbCore->UsbClearDeviceFeature(
                     UsbCbi->UsbIo,
                     USB_TARGET_ENDPOINT,
                     USB_FEATURE_ENDPOINT_HALT,
                     UsbCbi->BulkOutEndpoint->EndpointAddress,
                     &ClearResult
                     );
  return Status;
}

/*++

  Clean up the CBI protocol's resource

  @param  Context               The CBI protocol 

  @retval EFI_SUCCESS           The resource is cleaned up.

--*/
STATIC
EFI_STATUS
UsbCbiFini (
  IN  VOID                   *Context
  )
{
  USB_CBI_PROTOCOL        *UsbCbi;

  UsbCbi = (USB_CBI_PROTOCOL *) Context;
  UsbCbi->UsbCore->FreeBuffer (
                     sizeof (USB_CBI_PROTOCOL) + 3 * sizeof (EFI_USB_ENDPOINT_DESCRIPTOR),
                     UsbCbi
                     );
  //
  // Remove Smm address convert table
  //
  UsbCbi->UsbCore->RemoveAddressConvertTable (
                     ACT_FUNCTION_POINTER,
                     &UsbCbi->Transport.Init
                     );
  UsbCbi->UsbCore->RemoveAddressConvertTable (
                     ACT_INSTANCE_BODY,
                     UsbCbi
                     );
  UsbCbi->UsbCore->RemoveAddressConvertTable (
                     ACT_INSTANCE_POINTER,
                     &UsbCbi->UsbCore
                     );
  UsbCbi->UsbCore->RemoveAddressConvertTable (
                     ACT_INSTANCE_POINTER,
                     &UsbCbi->BulkInEndpoint
                     );
  UsbCbi->UsbCore->RemoveAddressConvertTable (
                     ACT_INSTANCE_POINTER,
                     &UsbCbi->BulkOutEndpoint
                     );
  UsbCbi->UsbCore->RemoveAddressConvertTable (
                     ACT_INSTANCE_POINTER,
                     &UsbCbi->InterruptEndpoint
                     );
  UsbCbi->UsbCore->RemoveAddressConvertTable (
                     ACT_INSTANCE_POINTER,
                     &UsbCbi->UsbIo
                     );
  return EFI_SUCCESS;
}
