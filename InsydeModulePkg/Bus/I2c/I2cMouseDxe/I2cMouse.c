/** @file
  I2C Mouse driver

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "I2cMouse.h"

/**
  Get HID Descriptor

  @param [in]  I2cIo            Protocol instance pointer.
  @param [in]  HidDescReg       HID descriptor register
  @param [out] HidDescriptor    Point to HID Descriptor

  @retval EFI_SUCCESS           This driver supports this device.
  @retval EFI_DEVICE_ERROR      No ack for this command

**/
STATIC
EFI_STATUS
GetHidDescriptor (
  IN   EFI_I2C_IO_PROTOCOL      *I2cIo,
  IN   UINT32                   HidDescReg,
  OUT  HID_DESCRIPTOR           *HidDescriptor
  )
{
  EFI_STATUS                    Status;
  EFI_I2C_REQUEST_PACKET        *RequestPacket;
  EFI_I2C_OPERATION             *Operation;
  UINT8                         Buffer[sizeof(EFI_I2C_REQUEST_PACKET) + sizeof(EFI_I2C_OPERATION)];
  
  RequestPacket = (EFI_I2C_REQUEST_PACKET*)Buffer;
  Operation   = RequestPacket->Operation;
  RequestPacket->OperationCount = 2;
  Operation[0].Flags            = 0;
  Operation[0].LengthInBytes    = sizeof(UINT16);
  Operation[0].Buffer           = (UINT8*)&HidDescReg;
  Operation[1].Flags            = I2C_FLAG_READ;
  Operation[1].LengthInBytes    = sizeof(HID_DESCRIPTOR);
  Operation[1].Buffer           = (UINT8*)HidDescriptor;

  Status = I2cIo->QueueRequest (
                    I2cIo,
                    0,
                    NULL,
                    RequestPacket,
                    NULL
                    );
  return Status;
}

/**
  Get Report Descriptor

  @param [in]  I2cIo            Protocol instance pointer.
  @param [in]  HidDescriptor    Point to HID Descriptor
  @param [out] ReportDesc       Point to Report Descriptor

  @retval EFI_SUCCESS           This driver supports this device.
  @retval EFI_DEVICE_ERROR      No ack for this command
  @retval EFI_INVALID_PARAMETER ReportDesc == NULL
  @retval EFI_OUT_OF_RESOURCES  Memory Overflow
  
**/
STATIC
EFI_STATUS
GetReportDescriptor (
  IN   EFI_I2C_IO_PROTOCOL      *I2cIo,
  IN   HID_DESCRIPTOR           *HidDescriptor,  
  OUT  UINT8                    **ReportDesc
  )
{
  EFI_I2C_REQUEST_PACKET        *RequestPacket;
  EFI_I2C_OPERATION             *Operation;
  UINT8                         Buffer[sizeof(EFI_I2C_REQUEST_PACKET) + sizeof(EFI_I2C_OPERATION)];
  UINT16                        ReportDescReg;
  EFI_STATUS                    Status;
  
  if (ReportDesc == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  
  *ReportDesc = AllocateZeroPool (HidDescriptor->ReportDescLength);

  if (*ReportDesc == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Get the Report Descriptor
  //
  RequestPacket = (EFI_I2C_REQUEST_PACKET*)Buffer;
  Operation                     = RequestPacket->Operation;
  ReportDescReg                 = HidDescriptor->ReportDescRegister;
  RequestPacket->OperationCount = 2;
  Operation[0].Flags            = 0;
  Operation[0].LengthInBytes    = sizeof(UINT16);
  Operation[0].Buffer           = (UINT8*)&ReportDescReg;
  Operation[1].Flags            = I2C_FLAG_READ;
  Operation[1].LengthInBytes    = HidDescriptor->ReportDescLength;
  Operation[1].Buffer           = *ReportDesc;

  Status = I2cIo->QueueRequest (
                    I2cIo,
                    0,
                    NULL,
                    RequestPacket,
                    NULL
                    );
  return Status;
}

/**

  Set the power state
  
  @param  MouseDev              Mouse instance
  @param  PowerState            Power state

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
SetPower (
  IN  I2C_MOUSE_DEV             *MouseDev,
  IN  UINT16                    PowerState
  )
{
  UINT16                        Request[2];
  EFI_STATUS                    Status;
  EFI_I2C_REQUEST_PACKET        RequestPacket;
  EFI_I2C_IO_PROTOCOL           *I2cIo;

  if ((MouseDev == NULL) ||
      ((I2cIo = MouseDev->I2cIo) == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Set the power state
  //
  Request[0] = MouseDev->HidDescriptor.CommandRegister;
  Request[1] = SET_POWER | PowerState;
  RequestPacket.OperationCount             = 1;
  RequestPacket.Operation[0].Flags         = 0;
  RequestPacket.Operation[0].LengthInBytes = 4;
  RequestPacket.Operation[0].Buffer        = (UINT8*)Request;

  Status = I2cIo->QueueRequest (
                    I2cIo,
                    0,
                    NULL,
                    &RequestPacket,
                    NULL
                    );
  return Status;
}

/**

  Reset the device
  
  @param  MouseDev              Mouse instance

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
ResetDevice (
  IN  I2C_MOUSE_DEV             *MouseDev
  )
{
  UINT16                        Request[2];
  EFI_STATUS                    Status;
  EFI_I2C_REQUEST_PACKET        RequestPacket;
  EFI_I2C_IO_PROTOCOL           *I2cIo;
  
  if ((MouseDev == NULL) ||
      ((I2cIo = MouseDev->I2cIo) == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Reset the device
  //
  Request[0] = MouseDev->HidDescriptor.CommandRegister;
  Request[1] = RESET;
  RequestPacket.OperationCount             = 1;
  RequestPacket.Operation[0].Flags         = 0;
  RequestPacket.Operation[0].LengthInBytes = 4;
  RequestPacket.Operation[0].Buffer        = (UINT8*)Request;

  Status = I2cIo->QueueRequest (
                    I2cIo,
                    0,
                    NULL,
                    &RequestPacket,
                    NULL
                    );
  
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Receive the status with flush the transmission buffer 
  //
  RequestPacket.OperationCount             = 1;
  RequestPacket.Operation[0].Flags         = I2C_FLAG_READ;
  RequestPacket.Operation[0].LengthInBytes = 2;
  RequestPacket.Operation[0].Buffer        = (UINT8*)Request;

  Status = I2cIo->QueueRequest (
                    I2cIo,
                    0,
                    NULL,
                    &RequestPacket,
                    NULL
                    );
  return Status;
}

/**

  Flush data buffer before operation
  
  @param  MouseDev              Mouse instance

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
FlushDataBuffer (
  IN  I2C_MOUSE_DEV             *MouseDev
  )
{
  EFI_STATUS                    Status;
  EFI_I2C_REQUEST_PACKET        RequestPacket;
  EFI_I2C_IO_PROTOCOL           *I2cIo;
  
  if ((MouseDev == NULL) ||
      ((I2cIo = MouseDev->I2cIo) == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Receive the status with flush the transmission buffer 
  //
  RequestPacket.OperationCount             = 1;
  RequestPacket.Operation[0].Flags         = I2C_FLAG_READ;
  RequestPacket.Operation[0].LengthInBytes = MouseDev->HidDescriptor.MaxInputLength;
  RequestPacket.Operation[0].Buffer        = MouseDev->ReportData;

  Status = I2cIo->QueueRequest (
                    I2cIo,
                    0,
                    NULL,
                    &RequestPacket,
                    NULL
                    );
  return Status;
}

/**

  It is called whenever there is data received from async interrupt
  transfer.

  @param  Event                 Wait Event
  @param  Context               Passed parameter to event handler

**/
STATIC
VOID
EFIAPI
I2cMouseInterruptOnCompleted (
  IN  EFI_EVENT                 Event,
  IN  VOID                      *Context
  )
{
  I2C_MOUSE_DEV                 *MouseDev;
  UINTN                         DataLength;
  UINT8                         *ReportData;
  UINTN                         Index;
  EFI_I2C_IO_PROTOCOL           *I2cIo;
  EFI_I2C_REQUEST_PACKET        RequestPacket;

  MouseDev   = (I2C_MOUSE_DEV*)Context;
  I2cIo      = MouseDev->I2cIo;
  DataLength = *(UINT16*)MouseDev->ReportData;
  if (MouseDev->TransferResult != EFI_SUCCESS || DataLength == 0) goto Exit;
  ReportData = MouseDev->ReportData + sizeof (UINT16);
  DataLength -= sizeof (UINT16);
  if (MouseDev->ReportId) {
    //
    // Searching for matched report field
    //
    for (Index = 0; Index < MouseDev->ReportFieldInfo.Total; Index ++) {
      if (MouseDev->ReportFieldInfo.ReportGroup[Index].Id == *ReportData && 
          MouseDev->ReportFieldInfo.ReportGroup[Index].DataType == HID_MAIN_ITEM_TAG_INPUT) {
         ReportData ++;
         DataLength --;
         break;
      }
    }
    if (Index == MouseDev->ReportFieldInfo.Total) goto Exit;
  }
  MouseDev->StateChanged = TRUE;
  //
  // Check mouse data
  //
  MouseDev->State.LeftButton  = (BOOLEAN) (*(UINT8 *) ReportData & 0x01);
  MouseDev->State.RightButton = (BOOLEAN) (*(UINT8 *) ReportData & 0x02);
  MouseDev->State.RelativeMovementX += *((INT8 *) ReportData + 1);
  MouseDev->State.RelativeMovementY += *((INT8 *) ReportData + 2);
  if (DataLength > 3) {
    MouseDev->State.RelativeMovementZ += *((INT8 *) ReportData + 3);
  }
Exit:
  //
  // Starting next transmission
  //
  RequestPacket.OperationCount             = 1;
  RequestPacket.Operation[0].Flags         = I2C_FLAG_READ;
  RequestPacket.Operation[0].LengthInBytes = MouseDev->HidDescriptor.MaxInputLength;
  RequestPacket.Operation[0].Buffer        = MouseDev->ReportData;

  I2cIo->QueueRequest (
           I2cIo,
           0,
           MouseDev->OnCompleteEvent,
           &RequestPacket,
           &MouseDev->TransferResult
           );
}

/**

  Get the mouse state, see SIMPLE POINTER PROTOCOL.
  
  @param  This                  Protocol instance pointer.
  @param  MouseState            Current mouse state
  
  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR
  @retval EFI_NOT_READY

**/
STATIC
EFI_STATUS
EFIAPI
I2cGetMouseState (
  IN   EFI_SIMPLE_POINTER_PROTOCOL      *This,
  OUT  EFI_SIMPLE_POINTER_STATE         *MouseState
  )
{
  I2C_MOUSE_DEV *MouseDev;

  if (MouseState == NULL) {
    return EFI_DEVICE_ERROR;
  }
  MouseDev = I2C_MOUSE_DEV_FROM_SIMPLE_POINTER_PROTOCOL (This);
  if (!MouseDev->StateChanged) {
    return EFI_NOT_READY;
  }
  CopyMem (MouseState, &MouseDev->State, sizeof (EFI_SIMPLE_POINTER_STATE));
  //
  // Clear previous move state
  //
  MouseDev->State.RelativeMovementX = 0;
  MouseDev->State.RelativeMovementY = 0;
  MouseDev->State.RelativeMovementZ = 0;
  MouseDev->StateChanged            = FALSE;
  return EFI_SUCCESS;
}

/**

  Reset the mouse device, see SIMPLE POINTER PROTOCOL.
  
  @param  This                  Protocol instance pointer.
  @param  ExtendedVerification  Ignored here
  
  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
EFIAPI
I2cMouseReset (
  IN EFI_SIMPLE_POINTER_PROTOCOL        *This,
  IN BOOLEAN                            ExtendedVerification
  )
{
  I2C_MOUSE_DEV       *MouseDev;

  MouseDev = I2C_MOUSE_DEV_FROM_SIMPLE_POINTER_PROTOCOL (This);
  ZeroMem ( &MouseDev->State, sizeof (EFI_SIMPLE_POINTER_STATE));
  MouseDev->StateChanged = FALSE;
  return EFI_SUCCESS;
}

/**

  Event notification function for SIMPLE_POINTER.WaitForInput event
  Signal the event if there is input from mouse

  @param  Event                 Wait Event
  @param  Context               Passed parameter to event handler

**/
STATIC
VOID
EFIAPI
MouseWaitForInput (
  IN  EFI_EVENT                 Event,
  IN  VOID                      *Context
  )
{
  I2C_MOUSE_DEV *MouseDev;

  MouseDev = (I2C_MOUSE_DEV*) Context;
  //
  // Someone is waiting on the mouse event, if there's
  // input from mouse, signal the event
  //
  if (MouseDev->StateChanged) {
    gBS->SignalEvent (Event);
  }
}

/**

  Free the resource allocated by I2C Mouse instance
  
  @param  MouseDev              I2C Mouse instance
  @param  Controller            I2C I/O Controller Handle

**/
STATIC
VOID
FreeResource (
  IN  I2C_MOUSE_DEV             *MouseDev,
  IN  EFI_HANDLE                Controller
  )
{
  //
  // Always close protocol even the protocol has not being opened
  //
  gBS->CloseProtocol (
         Controller,
         &gEfiI2cIoProtocolGuid,
         gI2cMouseDriverBinding.DriverBindingHandle,
         Controller
         );
  //
  // Free resources in the instance
  //
  if (MouseDev) {
    if (MouseDev->OnCompleteEvent) {
      gBS->CloseEvent (MouseDev->OnCompleteEvent);
    }
    if (MouseDev->SimplePointerProtocol.WaitForInput) {
      gBS->CloseEvent (MouseDev->SimplePointerProtocol.WaitForInput);
    }
    gBS->UninstallProtocolInterface (
           Controller,
           &gEfiSimplePointerProtocolGuid,
           &MouseDev->SimplePointerProtocol
           );
    if (MouseDev->ReportData) {
      FreePool (MouseDev->ReportData);
    }
    if (MouseDev->ControllerNameTable) {
      FreeUnicodeStringTable (MouseDev->ControllerNameTable);
    }
    FreePool (MouseDev);
  }
}

/**
  Verify the controller type

  This routine determines if an I2C Mouse is available.

  This routine is called by the UEFI driver framework during connect
  processing.

  @param [in] DriverBinding             Protocol instance pointer.
  @param [in] Controller                Handle of device to test.
  @param [in] RemainingDevicePath       Not used.

  @retval EFI_SUCCESS                   This driver supports this device.
  @retval other                         This driver does not support this device.

**/
EFI_STATUS
EFIAPI
I2cMouseDriverSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL        *This,
  IN EFI_HANDLE                         Controller,
  IN EFI_DEVICE_PATH_PROTOCOL           *RemainingDevicePath
  )
{
  EFI_STATUS                            Status;
  EFI_I2C_IO_PROTOCOL                   *I2cIo;
  EFI_ADAPTER_INFORMATION_PROTOCOL      *Aip;
  H2O_I2C_HID_DEVICE                    *I2cHidDevice;
  UINTN                                 DataLength;
  //
  // Determine if the I2C I/O is available
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiI2cIoProtocolGuid,
                  (VOID**) &I2cIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR ( Status )) {
    return Status;
  }
  //
  //  The I2C I/O is available
  //
  gBS->CloseProtocol (
         Controller,
         &gEfiI2cIoProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );
  //
  // Inspect the H2O I2C HID specific data in the controller
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiAdapterInformationProtocolGuid,
                  (VOID**) &Aip,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Status = Aip->GetInformation (
                  Aip,
                  &gI2cHidDeviceInfoGuid,
                  (VOID**) &I2cHidDevice,
                  &DataLength
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  if (I2cHidDevice->ClassType != POINTER_CLASS &&
      I2cHidDevice->ClassType != MOUSE_CLASS) {
    Status = EFI_NOT_FOUND;
  }
  FreePool (I2cHidDevice);
  return Status;
}

/**
  Connect to the I2C Mouse

  This routine initializes an instance of the I2C driver for this
  controller.

  This routine is called by the UEFI driver framework during connect
  processing if the controller passes the tests in I2cBusDriverSupported.

  @param [in] DriverBinding             Protocol instance pointer.
  @param [in] Controller                Handle of device to work with.
  @param [in] RemainingDevicePath       Not used, always produce all possible children.

  @retval EFI_SUCCESS                   This driver is added to Controller.
  @retval other                         This driver does not support this device.

**/
EFI_STATUS
EFIAPI
I2cMouseDriverStart (
  IN EFI_DRIVER_BINDING_PROTOCOL        *This,
  IN EFI_HANDLE                         Controller,
  IN EFI_DEVICE_PATH_PROTOCOL           *RemainingDevicePath
  )
{
  EFI_STATUS                            Status;
  I2C_MOUSE_DEV                         *MouseDev;
  EFI_I2C_IO_PROTOCOL                   *I2cIo;
  EFI_ADAPTER_INFORMATION_PROTOCOL      *Aip;
  H2O_I2C_HID_DEVICE                    *I2cHidDevice;
  UINTN                                 DataLength;
  UINT8                                 *ReportDescriptor;
  UINTN                                 Index;  
  REPORT_GROUP                          *ReportGroup;
  EFI_I2C_REQUEST_PACKET                RequestPacket;
  
  MouseDev = NULL;
  //
  // Occupy the I2C I/O protocol
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiI2cIoProtocolGuid,
                  (VOID**) &I2cIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Get the H2O I2C HID specific data in the controller
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiAdapterInformationProtocolGuid,
                  (VOID**) &Aip,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    goto Error;
  }
  Status = Aip->GetInformation (
                  Aip,
                  &gI2cHidDeviceInfoGuid,
                  (VOID**) &I2cHidDevice,
                  &DataLength
                  );
  if (EFI_ERROR (Status)) {
    goto Error;
  }
  MouseDev = AllocateZeroPool (sizeof (I2C_MOUSE_DEV));
  if (MouseDev == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Error;
  }
  MouseDev->Signature   = I2C_MOUSE_DEV_SIGNATURE;
  MouseDev->I2cIo       = I2cIo;
  CopyMem (&MouseDev->I2cHid, I2cHidDevice, I2cHidDevice->Length);
  FreePool (I2cHidDevice);
  //
  // Get HID descriptor
  //
  Status = GetHidDescriptor (I2cIo, MouseDev->I2cHid.DescReg, &MouseDev->HidDescriptor);
  if (EFI_ERROR (Status)) {
    goto Error;
  }
  if (MouseDev->HidDescriptor.HIDDescLength != sizeof(HID_DESCRIPTOR)) {
    Status = EFI_DEVICE_ERROR;
    goto Error;
  }
  MouseDev->ReportData = AllocateZeroPool (MouseDev->HidDescriptor.MaxInputLength);
  if (MouseDev->ReportData == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Error;
  }
  //
  // Power on the device
  //
  SetPower (MouseDev, POWER_ON);
  //
  // Reset the device
  //
  ResetDevice (MouseDev);
  //
  // Flush the data buffer before operation
  //
  FlushDataBuffer (MouseDev);
  //
  // Get report descriptor
  //
  Status = GetReportDescriptor ( I2cIo, &MouseDev->HidDescriptor, &ReportDescriptor);
  if (EFI_ERROR (Status)) {
    goto Error;
  }
  Status = ParseReportDescriptor (
             ReportDescriptor,
             MouseDev->HidDescriptor.ReportDescLength,
             &MouseDev->ReportFieldInfo
             );
  FreePool (ReportDescriptor);
  if (EFI_ERROR(Status)) {
    goto Error;
  }
  //
  // Searching for mouse report
  //
  for (Index = 0, ReportGroup = NULL; Index < MouseDev->ReportFieldInfo.Total; Index ++) {
    if (MouseDev->ReportFieldInfo.ReportGroup[Index].DataValid &&
        (MouseDev->ReportFieldInfo.ReportGroup[Index].DataClass == MOUSE_CLASS || 
         MouseDev->ReportFieldInfo.ReportGroup[Index].DataClass == POINTER_CLASS) &&
        MouseDev->ReportFieldInfo.ReportGroup[Index].DataType == HID_MAIN_ITEM_TAG_INPUT &&
        MouseDev->ReportFieldInfo.ReportGroup[Index].DataSize > 0) {
      ReportGroup =  &MouseDev->ReportFieldInfo.ReportGroup[Index];
      break;
    }
  }
  if (ReportGroup == NULL) { 
    Status = EFI_UNSUPPORTED;
    goto Error;
  }
  if (MouseDev->NumberOfButtons >= 1) {
    MouseDev->Mode.LeftButton = TRUE;
  }
  if (MouseDev->NumberOfButtons > 1) {
    MouseDev->Mode.RightButton = TRUE;
  }
  MouseDev->ReportId         = ReportGroup->Id;
  MouseDev->Mode.ResolutionX = 8;
  MouseDev->Mode.ResolutionY = 8;
  MouseDev->Mode.ResolutionZ = 0;
  MouseDev->SimplePointerProtocol.GetState  = I2cGetMouseState;
  MouseDev->SimplePointerProtocol.Reset     = I2cMouseReset;
  MouseDev->SimplePointerProtocol.Mode      = &MouseDev->Mode;
  Status = gBS->CreateEvent (
                  EVT_NOTIFY_WAIT,
                  TPL_NOTIFY,
                  MouseWaitForInput,
                  MouseDev,
                  &MouseDev->SimplePointerProtocol.WaitForInput
                  );
  if (EFI_ERROR (Status)) {
    goto Error;
  }
  //
  // Install Simple Pointer Protocol for the I2C mouse device.
  //
  Status = gBS->InstallProtocolInterface (
                  &Controller,
                  &gEfiSimplePointerProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &MouseDev->SimplePointerProtocol
                  );
  if (EFI_ERROR (Status)) {
    goto Error;
  }
  //
  // Create event for interrupt on completed handler
  //
  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  I2cMouseInterruptOnCompleted,
                  MouseDev,
                  &MouseDev->OnCompleteEvent
                  );
  if (EFI_ERROR (Status)) {
    goto Error;
  }
  //
  // Allocate and setup controller name table for ComponentName/2 protocol
  //
  AddUnicodeString2 (
    LANGUAGE_CODE_ENGLISH_ISO639,
    gI2cMouseComponentName.SupportedLanguages,
    &MouseDev->ControllerNameTable,
    CONTROLLER_DRIVER_NAME,
    TRUE
    );
  AddUnicodeString2 (
    LANGUAGE_CODE_ENGLISH_RFC4646,
    gI2cMouseComponentName2.SupportedLanguages,
    &MouseDev->ControllerNameTable,
    CONTROLLER_DRIVER_NAME,
    FALSE
    );
  //
  // Issue async I2C request for data transfer
  //
  RequestPacket.OperationCount             = 1;
  RequestPacket.Operation[0].Flags         = I2C_FLAG_READ;
  RequestPacket.Operation[0].LengthInBytes = MouseDev->HidDescriptor.MaxInputLength;
  RequestPacket.Operation[0].Buffer        = MouseDev->ReportData;

  Status = I2cIo->QueueRequest (
                    I2cIo,
                    0,
                    MouseDev->OnCompleteEvent,
                    &RequestPacket,
                    &MouseDev->TransferResult
                    );
Error:
  if (EFI_ERROR (Status)) {
    FreeResource (MouseDev, Controller);
  }
  return Status;
}


/**
  Disconnect from the I2C host controller.

  This routine disconnects from the Mouse

  This routine is called by DriverUnload when the Mouse
  is being unloaded.

  @param [in] DriverBinding     Protocol instance pointer.
  @param [in] Controller        Handle of device to stop driver on.
  @param [in] NumberOfChildren  How many children need to be stopped.
  @param [in] ChildHandleBuffer Not used.

  @retval EFI_SUCCESS           This driver is removed Controller.
  @retval EFI_DEVICE_ERROR      The device could not be stopped due to a device error.
  @retval other                 This driver was not removed from this device.

**/
EFI_STATUS
EFIAPI
I2cMouseDriverStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL       *This,
  IN  EFI_HANDLE                        Controller,
  IN  UINTN                             NumberOfChildren,
  IN  EFI_HANDLE                        *ChildHandleBuffer
  )
{
  EFI_STATUS                    Status;
  I2C_MOUSE_DEV                 *MouseDev;
  EFI_SIMPLE_POINTER_PROTOCOL   *SimplePointerProtocol;
  //
  //  Disconnect any connected drivers and locate the context
  //  structure
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiSimplePointerProtocolGuid,
                  (VOID**) &SimplePointerProtocol,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  MouseDev = I2C_MOUSE_DEV_FROM_SIMPLE_POINTER_PROTOCOL (SimplePointerProtocol);
  FreeResource (MouseDev, Controller);
  return Status;
}

/**

  Driver binding protocol support

**/
EFI_DRIVER_BINDING_PROTOCOL gI2cMouseDriverBinding = {
  I2cMouseDriverSupported,
  I2cMouseDriverStart,
  I2cMouseDriverStop,
  0x12,
  NULL,
  NULL
};

/**

  Entry point for EFI drivers.

  @param  ImageHandle           EFI_HANDLE
  @param  SystemTable           EFI_SYSTEM_TABLE

  @retval EFI_SUCCESS
  @retval others

**/
EFI_STATUS
EFIAPI
I2cMouseEntryPoint (
  IN EFI_HANDLE                 ImageHandle,
  IN EFI_SYSTEM_TABLE           *SystemTable
  )
{
  //
  // Install binding protocols
  //
  return EfiLibInstallDriverBindingComponentName2 (
           ImageHandle,
           SystemTable,
           &gI2cMouseDriverBinding,
           ImageHandle,
           &gI2cMouseComponentName,
           &gI2cMouseComponentName2
           );
}