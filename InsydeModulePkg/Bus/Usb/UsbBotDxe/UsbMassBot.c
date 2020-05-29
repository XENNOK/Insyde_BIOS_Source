/** @file
  USB Mass Storage BOT Driver

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

#include "UsbMassBot.h"

#ifndef MDEPKG_NDEBUG
UINTN mUsbBotInfo  = EFI_D_INFO;
UINTN mUsbBotError = EFI_D_ERROR;
#endif

//
// Function prototypes
//
EFI_STATUS
EFIAPI
UsbMassBotDriverEntryPoint (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  );

STATIC EFI_GUID                         mUsbMassTransportProtocolGuid = USB_MASS_TRANSPORT_PROTOCOL_GUID;
STATIC EFI_USB_CORE_PROTOCOL            *mUsbCore;
STATIC USB_MASS_TRANSPORT_PROTOCOL      *mUsbBotTransport;
STATIC UINT16                           mBypassCswTimeoutDevices[] = {
  0x090c, 0x1000, 0xffff,      // USB 1.1 "USB CYBER DISK" thumb drive with retry
  0x08ec, 0x0015, 0xffff,      // USB 2.0 "I-O DATA USB Flash Disk" thumb drive with retry
  0x1f75, 0x0902, 0x0001,      // USB 3.0 "SP Silcon Power 16GB" thumb drive without retry
  0x054c, 0x05b9, 0x0001,      // USB 3.0 "SONY USM-Q 32GB" thumb drive without retry
  0x125F, 0x105B, 0x0001,      // USB 3.0 "ADATA N005" thumb drive without retry
  0x8564, 0x1000, 0x0001,      // USB 3.0 "Transcend JetFlash 16G" thumb drive without retry
  0x0000, 0x0000, 0x0000,
};

/**

  Register protocol for this driver.

  @param  ImageHandle           EFI_HANDLE
  @param  SystemTable           EFI_SYSTEM_TABLE pointer

  @retval EFI_SUCCESS           Driver loaded
  @retval other                 Driver not loaded

**/
EFI_STATUS
EFIAPI
UsbMassBotDriverEntryPoint (
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
                         sizeof (USB_MASS_TRANSPORT_PROTOCOL),
                         ALIGNMENT_32,
                         (VOID **)&mUsbBotTransport
                         );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    mUsbBotTransport->Protocol    = USB_MASS_STORE_BOT;
    mUsbBotTransport->Init        = UsbBotInit;
    mUsbBotTransport->ExecCommand = UsbBotExecCommand;
    mUsbBotTransport->Reset       = UsbBotResetDevice;
    mUsbBotTransport->Fini        = UsbBotFini;
    //
    // Pass in a NULL to install to a new handle
    //
    Handle = NULL;
    Status = gBS->InstallProtocolInterface (
                    &Handle,
                    &mUsbMassTransportProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    mUsbBotTransport
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    //
    // Register module in DXE instance
    //
    mUsbCore->ModuleRegistration (ImageHandle);
  } else {
    //
    // SMM instance, disable DEBUG message out
    //
#ifndef MDEPKG_NDEBUG
    mUsbBotInfo         = 0;
    mUsbBotError        = 0;
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
      if (Transport->Protocol == USB_MASS_STORE_BOT) {
        mUsbBotTransport = Transport;
        break;
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
                &mUsbBotTransport->Init,
                4
                );
    mUsbCore->InsertAddressConvertTable (
                ACT_INSTANCE_BODY,
                mUsbBotTransport,
                sizeof (USB_MASS_TRANSPORT_PROTOCOL)
                );
    mUsbCore->InsertAddressConvertTable (
                ACT_INSTANCE_POINTER,
                &mUsbBotTransport,
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

  Initialize the USB mass storage class BOT transport protocol.
  It will save its context which is a USB_BOT_PROTOCOL structure
  in the Context if Context isn't NULL.

  @param  UsbIo                 The USB IO protocol to use
  @param  Controller            The controller to init
  @param  Context               The variable to save the context to

  @retval EFI_SUCCESS           The device is supported and protocol initialized.
  @retval EFI_OUT_OF_RESOURCES  Failed to allocate memory
  @retval EFI_UNSUPPORTED       The transport protocol doesn't support the device.

**/
STATIC
EFI_STATUS
UsbBotInit (
  IN  EFI_USB_IO_PROTOCOL       * UsbIo,
  IN  EFI_HANDLE                Controller,
  OUT VOID                      **Context OPTIONAL
  )
{
  USB_BOT_PROTOCOL              *UsbBot;
  EFI_USB_INTERFACE_DESCRIPTOR  *Interface;
  EFI_USB_ENDPOINT_DESCRIPTOR   EndPoint;
  EFI_USB_DEVICE_DESCRIPTOR     DeviceDescriptor;
  EFI_STATUS                    Status;
  UINT8                         Index;
 
  //
  // Allocate the BOT context, append two endpoint descriptors to it
  //
  Status = mUsbCore->AllocateBuffer (
                       sizeof (USB_BOT_PROTOCOL) + 2 * sizeof (EFI_USB_ENDPOINT_DESCRIPTOR),
                       ALIGNMENT_32,
                       (VOID **)&UsbBot
                       );
                    
  if (EFI_ERROR (Status)) {
    return EFI_OUT_OF_RESOURCES;
  }

  UsbBot->UsbIo   = UsbIo;
  UsbBot->UsbCore = mUsbCore;
  
  //
  // Get the interface descriptor and validate that it
  // is a USB MSC BOT interface.
  //
  Status = UsbIo->UsbGetInterfaceDescriptor (UsbIo, &UsbBot->Interface);

  if (EFI_ERROR (Status)) {
    DEBUG ((mUsbBotError, "UsbBotInit: Get invalid BOT interface (%r)\n", Status));
    goto ON_ERROR;
  }

  Interface = &UsbBot->Interface;
  //
  // Locate and save the first bulk-in and bulk-out endpoint
  //
  for (Index = 0; Index < Interface->NumEndpoints; Index++) {
    Status = UsbIo->UsbGetEndpointDescriptor (UsbIo, Index, &EndPoint);

    if (EFI_ERROR (Status) || !USB_IS_BULK_ENDPOINT (EndPoint.Attributes)) {
      continue;
    }

    if (USB_IS_IN_ENDPOINT (EndPoint.EndpointAddress) && 
       (UsbBot->BulkInEndpoint == NULL)) {
       
      UsbBot->BulkInEndpoint  = (EFI_USB_ENDPOINT_DESCRIPTOR *) (UsbBot + 1);
      *UsbBot->BulkInEndpoint = EndPoint;
    }

    if (USB_IS_OUT_ENDPOINT (EndPoint.EndpointAddress) && 
       (UsbBot->BulkOutEndpoint == NULL)) {
       
      UsbBot->BulkOutEndpoint   = (EFI_USB_ENDPOINT_DESCRIPTOR *) (UsbBot + 1) + 1;
      *UsbBot->BulkOutEndpoint  = EndPoint;
    }
  }
  
  if ((UsbBot->BulkInEndpoint == NULL) || (UsbBot->BulkOutEndpoint == NULL)) {
    DEBUG ((mUsbBotError, "UsbBotInit: In/Out Endpoint invalid\n"));
    Status = EFI_UNSUPPORTED;
    goto ON_ERROR;
  }
  //
  // Inspect specific device to bypass CSW timeout
  //
  Status = UsbIo->UsbGetDeviceDescriptor (UsbIo, &DeviceDescriptor);
  if (EFI_ERROR (Status)) {
    goto ON_ERROR;
  }
  for (Index = 0; mBypassCswTimeoutDevices[Index] != 0; Index += 3) {
    if (DeviceDescriptor.IdVendor == mBypassCswTimeoutDevices[Index] && 
        DeviceDescriptor.IdProduct == mBypassCswTimeoutDevices[Index + 1]) {
      UsbBot->BypassCswTimeout = mBypassCswTimeoutDevices[Index + 2];
    }
  }
  //
  // The USB BOT protocol uses dCBWTag to match the CBW and CSW.
  //
  UsbBot->CbwTag = 0x01;
  if (Context != NULL) {
    //
    // Setup the data for UsbLegacy
    //
    CopyMem(&UsbBot->Transport, mUsbBotTransport, sizeof(USB_MASS_TRANSPORT_PROTOCOL));
    //
    // Setup Smm address convert table for Smm security policy
    //
    mUsbCore->InsertAddressConvertTable (
                ACT_FUNCTION_POINTER,
                &UsbBot->Transport.Init,
                4
                );
    mUsbCore->InsertAddressConvertTable (
                ACT_INSTANCE_BODY,
                UsbBot,
                sizeof (USB_BOT_PROTOCOL) + 2 * sizeof (EFI_USB_ENDPOINT_DESCRIPTOR)
                );
    mUsbCore->InsertAddressConvertTable (
                ACT_INSTANCE_POINTER,
                &UsbBot->UsbCore,
                1
                );
    mUsbCore->InsertAddressConvertTable (
                ACT_INSTANCE_POINTER,
                &UsbBot->BulkInEndpoint,
                1
                );
    mUsbCore->InsertAddressConvertTable (
                ACT_INSTANCE_POINTER,
                &UsbBot->BulkOutEndpoint,
                1
                );
    mUsbCore->InsertAddressConvertTable (
                ACT_INSTANCE_POINTER,
                &UsbBot->UsbIo,
                1
                );
    *Context = UsbBot;
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
              sizeof (USB_BOT_PROTOCOL) + 2 * sizeof (EFI_USB_ENDPOINT_DESCRIPTOR),
              UsbBot
              );
  return Status;
}

/**

  Send the command to the device using Bulk-Out endpoint

  @param  UsbBot                The USB BOT device
  @param  Cmd                   The command to transfer to device
  @param  CmdLen                the length of the command
  @param  DataDir               The direction of the data
  @param  TransLen              The expected length of the data

  @retval EFI_SUCCESS           The command is sent to the device.
  @retval EFI_NOT_READY         The device return NAK to the transfer
  @retval Others                Failed to send the command to device

**/
STATIC
EFI_STATUS
UsbBotSendCommand (
  IN USB_BOT_PROTOCOL         *UsbBot,
  IN UINT8                    *Cmd,
  IN UINT8                    CmdLen,
  IN EFI_USB_DATA_DIRECTION   DataDir,
  IN UINT32                   TransLen
  )
{
  USB_BOT_CBW               Cbw;
  EFI_STATUS                Status;
  UINT32                    Result;
  UINTN                     DataLen;
  UINTN                     Timeout;

  ASSERT ((CmdLen > 0) && (CmdLen <= USB_BOT_MAX_CMDLEN));

  //
  // Check is device been detached
  //
  if (mUsbCore->CheckDeviceDetached(UsbBot->UsbIo) == EFI_SUCCESS) {
    return EFI_NOT_FOUND;
  }
  //
  // Fill in the CSW.
  //
  Cbw.Signature = USB_BOT_CBW_SIGNATURE;
  Cbw.Tag       = UsbBot->CbwTag;
  Cbw.DataLen   = TransLen;
  Cbw.Flag      = ((DataDir == EfiUsbDataIn) ? 0x80 : 0);
  Cbw.Lun       = Cmd[1] >> 5;
  Cbw.CmdLen    = CmdLen;

  ZeroMem (Cbw.CmdBlock, USB_BOT_MAX_CMDLEN);
  CopyMem (Cbw.CmdBlock, Cmd, CmdLen);

  Result        = 0;
  DataLen       = sizeof (USB_BOT_CBW);
  Timeout       = USB_BOT_CBW_TIMEOUT / USB_MASS_STALL_1_MS;

  //
  // Use the UsbIo to send the command to the device. The default
  // time out is enough.
  //
  Status = UsbBot->UsbIo->UsbBulkTransfer (
                            UsbBot->UsbIo,
                            UsbBot->BulkOutEndpoint->EndpointAddress,
                            &Cbw,
                            &DataLen,
                            Timeout,
                            &Result
                            );
  //
  // Respond to Bulk-Out endpoint stall with a Reset Recovery,
  // see the spec section 5.3.1
  //
  if (EFI_ERROR (Status)) {
    if (USB_IS_ERROR (Result, EFI_USB_ERR_STALL) && DataDir == EfiUsbDataOut) {
      UsbBotResetDevice (UsbBot, FALSE);
    } else if (USB_IS_ERROR (Result, EFI_USB_ERR_NAK)) {
      Status = EFI_NOT_READY;
    }
  }

  return Status;
}

/**

  Transfer the data between the device and host. BOT transfer
  is composed of three phase, command, data, and status.

  @param  UsbBot                The USB BOT device
  @param  DataDir               The direction of the data
  @param  Data                  The buffer to hold data
  @param  TransLen              The expected length of the data
  @param  Timeout               The time to wait the command to complete

  @param  EFI_SUCCESS           The data is transferred
  @param  Others                Failed to transfer data

**/
STATIC
EFI_STATUS
UsbBotDataTransfer (
  IN USB_BOT_PROTOCOL         *UsbBot,
  IN EFI_USB_DATA_DIRECTION   DataDir,
  IN OUT UINT8                *Data,
  IN OUT UINTN                *TransLen,
  IN UINT32                   Timeout
  )
{
  EFI_USB_ENDPOINT_DESCRIPTOR *Endpoint;
  EFI_STATUS                  Status;
  EFI_STATUS                  Status1;
  UINT32                      Result;

  //
  // Check is device been detached
  //
  if (mUsbCore->CheckDeviceDetached(UsbBot->UsbIo) == EFI_SUCCESS) {
    return EFI_NOT_FOUND;
  }
  //
  // It's OK if no data to transfer
  //
  if ((DataDir == EfiUsbNoData) || (*TransLen == 0)) {
    return EFI_SUCCESS;
  }
  
  //
  // Select the endpoint then issue the transfer
  //
  if (DataDir == EfiUsbDataIn) {
    Endpoint = UsbBot->BulkInEndpoint;
  } else {
    Endpoint = UsbBot->BulkOutEndpoint;
  }

  Result  = 0;
  Timeout = Timeout / USB_MASS_STALL_1_MS;

  Status = UsbBot->UsbIo->UsbBulkTransfer (
                            UsbBot->UsbIo,
                            Endpoint->EndpointAddress,
                            Data,
                            TransLen,
                            Timeout,
                            &Result
                            );
  if (EFI_ERROR (Status)) {
    DEBUG ((mUsbBotError, "UsbBotDataTransfer: (%r)\n", Status));
    if (USB_IS_ERROR (Result, EFI_USB_ERR_STALL) || (Status == EFI_TIMEOUT && UsbBot->BypassCswTimeout)) {
      DEBUG ((mUsbBotError, "UsbBotDataTransfer: DataIn Stall\n"));
      Status1 = UsbBot->UsbCore->UsbClearDeviceFeature(
                                   UsbBot->UsbIo,
                                   USB_TARGET_ENDPOINT,
                                   USB_FEATURE_ENDPOINT_HALT,
                                   Endpoint->EndpointAddress,
                                   &Result
                                   );
      if (EFI_ERROR (Status1)) {
        //
        // Serious error occurred, given EFI_NO_RESPONSE to skip status phase
        //
        Status = EFI_NO_RESPONSE;
      } 
      //
      // Restore flags when something wrong
      //
      UsbBot->FirstReadPassed         = FALSE;
      UsbBot->FirstReadCapPassed      = FALSE;
      UsbBot->FirstRequestSensePassed = FALSE;
      UsbBot->FirstInquiryPassed      = FALSE;
      if (UsbBot->BypassCswTimeout) {
        if (UsbBot->BypassCswTimeout == 1) {
          return EFI_NOT_FOUND;
        }
      }
    } else if (USB_IS_ERROR (Result, EFI_USB_ERR_NAK)) {
      Status = EFI_NOT_READY;
    }
  }

  return Status;
}

/**

  Get the command execution status from device. BOT transfer is
  composed of three phase, command, data, and status. 

  This function return the transfer status of the BOT's CSW status,
  and return the high level command execution result in Result. So
  even it returns EFI_SUCCESS, the command may still have failed.

  @param  UsbBot                The USB BOT device
  @param  TransLen              The expected length of the data
  @param  CmdStatus             The result of the command execution.
  @param  Timeout               The time to wait the command to complete

  @retval EFI_SUCCESS           Command execute result is retrieved and in the Result.
  @retval EFI_DEVICE_ERROR      Failed to retrieve the command execute result

**/
STATIC
EFI_STATUS
UsbBotGetStatus (
  IN  USB_BOT_PROTOCOL      *UsbBot,
  IN  UINT32                TransLen,
  OUT UINT8                 *CmdStatus,
  IN  UINT32                Timeout
  )
{
  USB_BOT_CSW               Csw;
  UINTN                     Len;
  UINT8                     Endpoint;
  EFI_STATUS                Status;
  UINT32                    Result;
  EFI_USB_IO_PROTOCOL       *UsbIo;
  UINT32                    Index;
  
  *CmdStatus = USB_BOT_COMMAND_ERROR;
  Status     = EFI_DEVICE_ERROR;
  Endpoint   = UsbBot->BulkInEndpoint->EndpointAddress;
  UsbIo      = UsbBot->UsbIo;
  Timeout    = (Timeout / USB_MASS_STALL_1_MS) / USB_BOT_GET_STATUS_RETRY;

  for (Index = 0; Index < USB_BOT_GET_STATUS_RETRY; Index ++) {
    //
    // Check is device been detached
    //
    if (mUsbCore->CheckDeviceDetached(UsbBot->UsbIo) == EFI_SUCCESS) {
      return EFI_NOT_FOUND;
    }
    //
    // Attemp to the read CSW from bulk in endpoint
    //
    ZeroMem (&Csw, sizeof (USB_BOT_CSW));
    Result = 0;
    Len    = sizeof (USB_BOT_CSW);
    Status = UsbIo->UsbBulkTransfer (
                      UsbIo,
                      Endpoint,
                      &Csw,
                      &Len,
                      Timeout,
                      &Result
                      );
    if (EFI_ERROR(Status)) {
      DEBUG ((mUsbBotError, "UsbBotGetStatus (%r)\n", Status));
      if (USB_IS_ERROR (Result, EFI_USB_ERR_STALL) || (Status == EFI_TIMEOUT && UsbBot->BypassCswTimeout)) {
        DEBUG ((mUsbBotError, "UsbBotGetStatus: DataIn Stall\n"));
        UsbBot->UsbCore->UsbClearDeviceFeature(
                           UsbBot->UsbIo,
                           USB_TARGET_ENDPOINT,
                           USB_FEATURE_ENDPOINT_HALT,
                           Endpoint,
                           &Result
                           );
        //
        // Restore flags when something wrong
        //
        UsbBot->FirstReadPassed         = FALSE;
        UsbBot->FirstReadCapPassed      = FALSE;
        UsbBot->FirstRequestSensePassed = FALSE;
        UsbBot->FirstInquiryPassed      = FALSE;
        if (UsbBot->BypassCswTimeout) {
          if (UsbBot->BypassCswTimeout == 1) {
            return EFI_NOT_FOUND;
          }
          break;
        }
      }
      continue;
    }
    //
    // Following situations will be treated as valid CSW
    // 1. Signature not equal to "USBS" but the tag equal to CBW's tag
    // 2. Both of signature and tag equal to zero
    //
    if (Len != sizeof (USB_BOT_CSW)) {
      //
      // Something wrong, return command error
      //
      *CmdStatus = USB_MASS_CMD_PHASE_ERROR;
    } else if (Csw.Tag != UsbBot->CbwTag && !(Csw.Signature == 0 && Csw.Tag == 0)) {
      //
      // Invalid Csw need perform reset recovery
      //
      DEBUG ((mUsbBotError, "UsbBotGetStatus: Device return a invalid signature\n"));
      Status = UsbBotResetDevice (UsbBot, FALSE);
    } else if (Csw.CmdStatus == USB_BOT_COMMAND_ERROR) {
      //
      // Respond phase error need perform reset recovery
      //
      DEBUG ((mUsbBotError, "UsbBotGetStatus: Device return a phase error\n"));
      Status = UsbBotResetDevice (UsbBot, FALSE);
      *CmdStatus = Csw.CmdStatus;
    } else {
      *CmdStatus = Csw.CmdStatus;
    }
    break;
  }
  //
  //The tag is increased even there is an error.
  //
  UsbBot->CbwTag++;

  return Status;
}

/**

  Call the Usb mass storage class transport protocol to issue
  the command/data/status circle to execute the commands

  @param  Context               The context of the BOT protocol, that is, USB_BOT_PROTOCOL
  @param  Cmd                   The high level command
  @param  CmdLen                The command length
  @param  DataDir               The direction of the data transfer
  @param  Data                  The buffer to hold data
  @param  DataLen               The length of the data
  @param  Timeout               The time to wait command 
  @param  CmdStatus             The result of high level command execution

  @retval EFI_SUCCESS           The command is executed OK, and result in CmdStatus
  @retval EFI_DEVICE_ERROR      Failed to excute command

**/
STATIC
EFI_STATUS
UsbBotExecCommand (
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
  USB_BOT_PROTOCOL          *UsbBot;
  EFI_STATUS                Status;
  UINTN                     TransLen;
  UINT8                     Result;

  *CmdStatus  = USB_MASS_CMD_FAIL;
  UsbBot      = (USB_BOT_PROTOCOL *) Context;

  //
  // Send the command to the device. Return immediately if device
  // rejects the command.
  //
  Status = UsbBotSendCommand (UsbBot, Cmd, CmdLen, DataDir, DataLen);
  if (EFI_ERROR (Status)) {
    DEBUG ((mUsbBotError, "UsbBotExecCommand: UsbBotSendCommand (%r)\n", Status));
    return Status;
  }
  //
  // Some devices needs extremely long stall on first Read10 command when
  // the SATA "Standby Immediately" been set to the device
  //
  if (!UsbBot->FirstReadPassed && *(UINT8*)Cmd == 0x28) {
    if (Timeout < USB_BOT_DATA_TIMEOUT) Timeout = USB_BOT_DATA_TIMEOUT;
    UsbBot->FirstReadPassed = TRUE;
  }
  //
  // Some ODD devices needs extremely long stall on first Read Capacity command
  //
  if (!UsbBot->FirstReadCapPassed && *(UINT8*)Cmd == 0x25) {
    if (Timeout < USB_BOT_DATA_TIMEOUT) Timeout = USB_BOT_DATA_TIMEOUT;
    UsbBot->FirstReadCapPassed = TRUE;
  }
  //
  // Some devices needs extremely long stall on first RequestSense command when
  // last command got Command Failed result
  //
  if (!UsbBot->FirstRequestSensePassed && *(UINT8*)Cmd == 0x03) {
    if (Timeout < USB_BOT_DATA_TIMEOUT) Timeout = USB_BOT_DATA_TIMEOUT;
    UsbBot->FirstRequestSensePassed = TRUE;
  }
  //
  // Some devices needs extremely long stall on first Inquery command when
  // last command got Command Failed result
  //
  if (!UsbBot->FirstInquiryPassed && *(UINT8*)Cmd == 0x12 && !UsbBot->BypassCswTimeout) {
    if (Timeout < USB_BOT_DATA_TIMEOUT) Timeout = USB_BOT_DATA_TIMEOUT;
    UsbBot->FirstInquiryPassed = TRUE;
  }
  //
  // Transfer the data. Don't return immediately even data transfer
  // failed. The host should attempt to receive the CSW no matter
  // whether it succeeds or failed.
  //
  TransLen = (UINTN) DataLen;
  Status = UsbBotDataTransfer (UsbBot, DataDir, Data, &TransLen, Timeout);
  if (Status == EFI_NOT_FOUND) {
    return Status;
  }
  //
  // No response on data phase, skip status phase to save time on retry
  //
  if (Status == EFI_NO_RESPONSE) {
    *CmdStatus = USB_MASS_CMD_PHASE_ERROR;
    return Status;
  }
  //
  // Issue port reset if we got timeout error on Inquiry command to workaround some incompatible HDD 
  //
  if (Status == EFI_TIMEOUT && *(UINT8*)Cmd == 0x12) {
    UsbBotResetDevice (UsbBot, TRUE);
    return Status;
  }
  //
  // Some devices needs extremely long stall on first BOT command to waiting
  // for firmware startup during cold boot
  //
  Timeout = USB_BOT_CSW_TIMEOUT;
  if (UsbBot->CbwTag == 1 && !UsbBot->BypassCswTimeout) {
    Timeout = USB_BOT_CSW_TIMEOUT * 2;
  }
  //
  // Get the status, if that succeeds, interpret the result
  //
  Status = UsbBotGetStatus (UsbBot, DataLen, &Result, Timeout);
  if (EFI_ERROR (Status)) {
    DEBUG ((mUsbBotError, "UsbBotExecCommand: UsbBotGetStatus (%r)\n", Status));
    return Status;
  }

  if (Result == 0) {
    *CmdStatus = USB_MASS_CMD_SUCCESS;
  } else if (Result == USB_MASS_CMD_PHASE_ERROR) {
    *CmdStatus = USB_MASS_CMD_PHASE_ERROR;
  }

  return EFI_SUCCESS;
}

/**

  Reset the mass storage device by BOT protocol

  @param  Context               The context of the BOT protocol, that is, USB_BOT_PROTOCOL

  @param  EFI_SUCCESS           The device is reset
  @param  Others                Failed to reset the device.

**/
STATIC
EFI_STATUS
UsbBotResetDevice (
  IN  VOID                    *Context,
  IN  BOOLEAN                  ExtendedVerification
  )
{
  USB_BOT_PROTOCOL        *UsbBot;
  EFI_USB_DEVICE_REQUEST  Request;
  EFI_STATUS              Status;
  UINT32                  Result;
  UINT32                  Timeout;

  UsbBot = (USB_BOT_PROTOCOL *) Context;
  //
  // Check is device been detached
  //
  if (mUsbCore->CheckDeviceDetached(UsbBot->UsbIo) == EFI_SUCCESS) {
    return EFI_NOT_FOUND;
  }
  if (ExtendedVerification) {
    //
    // If we need to do strictly reset, reset its parent hub port
    //
    Status = UsbBot->UsbIo->UsbPortReset (UsbBot->UsbIo);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  //
  // Issue a class specific "Bulk-Only Mass Storage Reset reqest.
  // See the spec section 3.1
  //
  Request.RequestType = 0x21;
  Request.Request     = USB_BOT_RESET_REQUEST;
  Request.Value       = 0;
  Request.Index       = UsbBot->Interface.InterfaceNumber;
  Request.Length      = 0;
  Timeout             = USB_BOT_RESET_TIMEOUT / USB_MASS_STALL_1_MS;

  Status = UsbBot->UsbIo->UsbControlTransfer (
                            UsbBot->UsbIo,
                            &Request,
                            EfiUsbNoData,
                            Timeout,
                            NULL,
                            0,
                            &Result
                            );

  if (EFI_ERROR (Status)) {
    DEBUG ((mUsbBotError, "UsbBotResetDevice: (%r)\n", Status));
    return Status;
  }

  //
  // The device shall NAK the host's request until the reset is
  // complete. We can use this to sync the device and host. For
  // now just stall 100ms to wait the device.
  //
  UsbBot->UsbCore->Stall (USB_BOT_RESET_STALL);

  //
  // Clear the Bulk-In and Bulk-Out stall condition.
  //
  UsbBot->UsbCore->UsbClearDeviceFeature(
                     UsbBot->UsbIo,
                     USB_TARGET_ENDPOINT,
                     USB_FEATURE_ENDPOINT_HALT,
                     UsbBot->BulkInEndpoint->EndpointAddress,
                     &Result
                     );
  //
  // A stall between Bulk-In and Bulk-Out for some smart phone device
  //
  UsbBot->UsbCore->Stall (USB_BOT_RESET_STALL / 2);
  
  UsbBot->UsbCore->UsbClearDeviceFeature(
                     UsbBot->UsbIo,
                     USB_TARGET_ENDPOINT,
                     USB_FEATURE_ENDPOINT_HALT,
                     UsbBot->BulkOutEndpoint->EndpointAddress,
                     &Result
                     );
  return Status;
}

/**

  Clean up the resource used by this BOT protocol

  @param  Context               The context of the BOT protocol, that is, USB_BOT_PROTOCOL

  @retval EFI_SUCCESS           The resource is cleaned up.

**/
STATIC
EFI_STATUS
UsbBotFini (
  IN  VOID                    *Context
  )
{
  USB_BOT_PROTOCOL        *UsbBot;

  UsbBot = (USB_BOT_PROTOCOL *) Context;
  UsbBot->UsbCore->FreeBuffer (
                     sizeof (USB_BOT_PROTOCOL) + 2 * sizeof (EFI_USB_ENDPOINT_DESCRIPTOR),
                     UsbBot
                     );
  //
  // Remove Smm address convert table
  //
  UsbBot->UsbCore->RemoveAddressConvertTable (
                     ACT_FUNCTION_POINTER,
                     &UsbBot->Transport.Init
                     );
  UsbBot->UsbCore->RemoveAddressConvertTable (
                     ACT_INSTANCE_BODY,
                     UsbBot
                     );
  UsbBot->UsbCore->RemoveAddressConvertTable (
                     ACT_INSTANCE_POINTER,
                     &UsbBot->UsbCore
                     );
  UsbBot->UsbCore->RemoveAddressConvertTable (
                     ACT_INSTANCE_POINTER,
                     &UsbBot->BulkInEndpoint
                     );
  UsbBot->UsbCore->RemoveAddressConvertTable (
                     ACT_INSTANCE_POINTER,
                     &UsbBot->BulkOutEndpoint
                     );
  UsbBot->UsbCore->RemoveAddressConvertTable (
                     ACT_INSTANCE_POINTER,
                     &UsbBot->UsbIo
                     );
  return EFI_SUCCESS;
}
