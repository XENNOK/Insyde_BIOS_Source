/** @file
  Usb Hub Request

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

#include "UsbBus.h"
#include "Hub.h"

typedef struct {
  UINT16        IdVendor;
  UINT16        IdProduct;
  UINT32        Delay;
} HUB_TABLE;

HUB_TABLE HubDelayTable[] = {
  { 0x8087, 0xffff, 0    },   // Intel virtual HUB without delay
  { 0x0424, 0x2514, 0    },   // WhitneyPoint virtual HUB without delay
  { 0x05e3, 0x0612, 3000 },   // Gyroscope GL3520M-OSY10 USB 3.0 HUB with 3 seconds delay to waiting for downstream port ready
  { 0x2109, 0xffff, 3000 },   // VIA USB 3.0 HUB with 3 seconds delay to waiting for downstream port ready
  { 0x0000, 0x0000, 0    }    // End of list
};

/**

  Performing HUB delay after port reset

  @param  UsbDevice             Pointer of USB_IO_DEVICE
  @param  Delay                 Delay in millisecond
  
**/
STATIC
UINTN
HubDelay (
  IN  USB_IO_DEVICE             *UsbDevice,
  IN  UINTN                     Delay
  )
{
  UINTN Index;
  
  for (Index = 0; HubDelayTable[Index].IdVendor != 0; Index ++) {
    if (UsbDevice->DeviceSpeed == EFI_USB_SPEED_HIGH &&
        (HubDelayTable[Index].IdVendor == 0xffff || HubDelayTable[Index].IdVendor == UsbDevice->DeviceDescriptor.IdVendor) &&
        (HubDelayTable[Index].IdProduct == 0xffff || HubDelayTable[Index].IdProduct == UsbDevice->DeviceDescriptor.IdProduct)) {
      if (Delay == 0) return HubDelayTable[Index].Delay;
      Delay = HubDelayTable[Index].Delay;
      break;
    }
  }
  if (Delay) mPrivate->UsbCore->Stall (Delay * 1000);
  return Delay;
}

/**

  Get a given hub port status

  @param  UsbIo                 EFI_USB_IO_PROTOCOL instance
  @param  Port                  Usb hub port number (starting from 1).
  @param  PortStatus            Current Hub port status and change status.
  
  @retval EFI_SUCCESS
  @retval EFI_DEVICE
  @retval EFI_TIME_OUT
  @retval EFI_INVALID_PARAMETER
  
**/
EFI_STATUS
HubGetPortStatus (
  IN  EFI_USB_IO_PROTOCOL       *UsbIo,
  IN  UINT8                     Port,
  OUT UINT32                    *PortStatus
  )
{
  EFI_USB_DEVICE_REQUEST  DevReq;
  EFI_STATUS              EfiStatus;
  UINT32                  UsbStatus;
  UINT32                  Timeout;

  ZeroMem (&DevReq, sizeof (EFI_USB_DEVICE_REQUEST));

  //
  // Fill Device request packet
  //
  DevReq.RequestType  = HUB_GET_PORT_STATUS_REQ_TYPE;
  DevReq.Request      = HUB_GET_PORT_STATUS;
  DevReq.Value        = 0;
  DevReq.Index        = Port;
  DevReq.Length       = sizeof (UINT32);

  Timeout             = 3000;

  EfiStatus = UsbIo->UsbControlTransfer (
                      UsbIo,
                      &DevReq,
                      EfiUsbDataIn,
                      Timeout,
                      PortStatus,
                      sizeof (UINT32),
                      &UsbStatus
                      );

  return EfiStatus;
}

/**

  Set specified feature to a give hub port

  @param  UsbIo                 EFI_USB_IO_PROTOCOL instance
  @param  Port                  Usb hub port number (starting from 1).
  @param  Value                 New feature value.

  @retval EFI_SUCCESS
  @retval EFI_DEVICE
  @retval EFI_TIME_OUT
  @retval EFI_INVALID_PARAMETER

**/
EFI_STATUS
HubSetPortFeature (
  IN EFI_USB_IO_PROTOCOL        *UsbIo,
  IN UINT8                      Port,
  IN UINT8                      Value
  )
{
  EFI_USB_DEVICE_REQUEST  DevReq;
  EFI_STATUS              EfiStatus;
  UINT32                  UsbStatus;
  UINT32                  Timeout;

  ZeroMem (&DevReq, sizeof (EFI_USB_DEVICE_REQUEST));

  //
  // Fill Device request packet
  //
  DevReq.RequestType  = HUB_SET_PORT_FEATURE_REQ_TYPE;
  DevReq.Request      = HUB_SET_PORT_FEATURE;
  DevReq.Value        = Value;
  DevReq.Index        = Port;
  DevReq.Length       = 0;

  Timeout             = 3000;
  EfiStatus = UsbIo->UsbControlTransfer (
                      UsbIo,
                      &DevReq,
                      EfiUsbNoData,
                      Timeout,
                      NULL,
                      0,
                      &UsbStatus
                      );

  return EfiStatus;
}

/**

  Clear a specified feature of a given hub port

  @param  UsbIo                 EFI_USB_IO_PROTOCOL instance
  @param  Port                  Usb hub port number (starting from 1).
  @param  Value                 Feature value that will be cleared from
                                that hub port.

  @retval EFI_SUCCESS
  @retval EFI_DEVICE
  @retval EFI_TIME_OUT
  @retval EFI_INVALID_PARAMETER

**/
EFI_STATUS
HubClearPortFeature (
  IN EFI_USB_IO_PROTOCOL        *UsbIo,
  IN UINT8                      Port,
  IN UINT8                      Value
  )
{
  EFI_USB_DEVICE_REQUEST  DevReq;
  EFI_STATUS              EfiStatus;
  UINT32                  UsbStatus;
  UINT32                  Timeout;

  ZeroMem (&DevReq, sizeof (EFI_USB_DEVICE_REQUEST));

  //
  // Fill Device request packet
  //
  DevReq.RequestType  = HUB_CLEAR_FEATURE_PORT_REQ_TYPE;
  DevReq.Request      = HUB_CLEAR_FEATURE_PORT;
  DevReq.Value        = Value;
  DevReq.Index        = Port;
  DevReq.Length       = 0;

  Timeout             = 3000;
  EfiStatus = UsbIo->UsbControlTransfer (
                      UsbIo,
                      &DevReq,
                      EfiUsbNoData,
                      Timeout,
                      NULL,
                      0,
                      &UsbStatus
                      );

  return EfiStatus;
}

/**

  Get Hub Status

  @param  UsbIo                 EFI_USB_IO_PROTOCOL instance
  @param  HubStatus             Current Hub status and change status.

  @retval EFI_SUCCESS
  @retval EFI_DEVICE
  @retval EFI_TIME_OUT

**/
EFI_STATUS
HubGetHubStatus (
  IN  EFI_USB_IO_PROTOCOL       *UsbIo,
  OUT UINT32                    *HubStatus
  )
{
  EFI_USB_DEVICE_REQUEST  DevReq;
  EFI_STATUS              EfiStatus;
  UINT32                  UsbStatus;
  UINT32                  Timeout;

  ZeroMem (&DevReq, sizeof (EFI_USB_DEVICE_REQUEST));

  //
  // Fill Device request packet
  //
  DevReq.RequestType  = HUB_GET_HUB_STATUS_REQ_TYPE;
  DevReq.Request      = HUB_GET_HUB_STATUS;
  DevReq.Value        = 0;
  DevReq.Index        = 0;
  DevReq.Length       = sizeof (UINT32);

  Timeout             = 3000;
  EfiStatus = UsbIo->UsbControlTransfer (
                      UsbIo,
                      &DevReq,
                      EfiUsbDataIn,
                      Timeout,
                      HubStatus,
                      sizeof (UINT32),
                      &UsbStatus
                      );

  return EfiStatus;
}

/**

  Set a specified feature to the hub

  @param  UsbIo                 EFI_USB_IO_PROTOCOL instance
  @param  Value                 Feature value that will be set to the hub.

  @retval EFI_SUCCESS
  @retval EFI_DEVICE
  @retval EFI_TIME_OUT

**/
EFI_STATUS
HubSetHubFeature (
  IN EFI_USB_IO_PROTOCOL        *UsbIo,
  IN UINT8                      Value
  )
{
  EFI_USB_DEVICE_REQUEST  DevReq;
  EFI_STATUS              EfiStatus;
  UINT32                  UsbStatus;
  UINT32                  Timeout;

  ZeroMem (&DevReq, sizeof (EFI_USB_DEVICE_REQUEST));

  //
  // Fill Device request packet
  //
  DevReq.RequestType  = HUB_SET_HUB_FEATURE_REQ_TYPE;
  DevReq.Request      = HUB_SET_HUB_FEATURE;
  DevReq.Value        = Value;
  DevReq.Index        = 0;
  DevReq.Length       = 0;

  Timeout             = 3000;
  EfiStatus = UsbIo->UsbControlTransfer (
                      UsbIo,
                      &DevReq,
                      EfiUsbNoData,
                      Timeout,
                      NULL,
                      0,
                      &UsbStatus
                      );

  return EfiStatus;
}

/**

  Set a specified feature to the hub

  @param  UsbIo                 EFI_USB_IO_PROTOCOL instance
  @param  Value                 Feature value that will be cleared from the hub.

  @retval EFI_SUCCESS
  @retval EFI_DEVICE
  @retval EFI_TIME_OUT
  
**/
EFI_STATUS
HubClearHubFeature (
  IN EFI_USB_IO_PROTOCOL        *UsbIo,
  IN UINT8                      Value
  )
{
  EFI_USB_DEVICE_REQUEST  DevReq;
  EFI_STATUS              EfiStatus;
  UINT32                  UsbStatus;
  UINT32                  Timeout;

  ZeroMem (&DevReq, sizeof (EFI_USB_DEVICE_REQUEST));

  //
  // Fill Device request packet
  //
  DevReq.RequestType  = HUB_CLEAR_FEATURE_REQ_TYPE;
  DevReq.Request      = HUB_CLEAR_FEATURE;
  DevReq.Value        = Value;
  DevReq.Index        = 0;
  DevReq.Length       = 0;

  Timeout             = 3000;
  EfiStatus = UsbIo->UsbControlTransfer (
                      UsbIo,
                      &DevReq,
                      EfiUsbNoData,
                      Timeout,
                      NULL,
                      0,
                      &UsbStatus
                      );

  return EfiStatus;

}

/**

  Get the hub descriptor

  @param  UsbIo                 EFI_USB_IO_PROTOCOL instance
  @param  DescriptorSize        The length of Hub Descriptor buffer.
  @param  HubDescriptor         Caller allocated buffer to store the hub descriptor
                                if successfully returned.
                            
  @retval EFI_SUCCESS
  @retval EFI_DEVICE
  @retval EFI_TIME_OUT

**/
EFI_STATUS
GetHubDescriptor (
  IN  EFI_USB_IO_PROTOCOL       *UsbIo,
  IN  UINTN                     DescriptorSize,
  OUT EFI_USB_HUB_DESCRIPTOR    *HubDescriptor
  )
{
  EFI_USB_DEVICE_REQUEST  DevReq;
  EFI_STATUS              EfiStatus;
  UINT32                  UsbStatus;
  UINT32                  Timeout;
  USB_IO_CONTROLLER_DEVICE *UsbIoController;

  UsbIoController = USB_IO_CONTROLLER_DEVICE_FROM_USB_IO_THIS (UsbIo);

  ZeroMem (&DevReq, sizeof (EFI_USB_DEVICE_REQUEST));

  //
  // Fill Device request packet
  //
  DevReq.RequestType  = USB_REQ_TYPE_CLASS | USB_TARGET_DEVICE | USB_ENDPOINT_DIR_IN;
  DevReq.Request      = HUB_GET_DESCRIPTOR;
  DevReq.Value        = ((UsbIoController->UsbDevice->DeviceSpeed == EFI_USB_SPEED_SUPER) ? USB_DESC_TYPE_SSHUB : USB_DESC_TYPE_HUB) << 8;
  DevReq.Index        = 0;
  DevReq.Length       = (UINT16) DescriptorSize;

  Timeout             = 3000;
  EfiStatus = UsbIo->UsbControlTransfer (
                      UsbIo,
                      &DevReq,
                      EfiUsbDataIn,
                      Timeout,
                      HubDescriptor,
                      (UINT16) DescriptorSize,
                      &UsbStatus
                      );

  return EfiStatus;

}

/**

  Set the hub depth for super speed hub

  @param  UsbIo                 EFI_USB_IO_PROTOCOL instance
  @param  Depth                 The depth of hub
                            
  @retval EFI_SUCCESS
  @retval EFI_DEVICE
  @retval EFI_TIME_OUT

**/
STATIC
EFI_STATUS
SetHubDepth (
  IN  EFI_USB_IO_PROTOCOL       *UsbIo,
  IN  UINT8                     Depth
  )
{
  EFI_USB_DEVICE_REQUEST  DevReq;
  EFI_STATUS              EfiStatus;
  UINT32                  UsbStatus;
  UINT32                  Timeout;

  ZeroMem (&DevReq, sizeof (EFI_USB_DEVICE_REQUEST));

  //
  // Fill Device request packet
  //
  DevReq.RequestType  = HUB_SET_HUB_DEPTH_REQ_TYPE;
  DevReq.Request      = HUB_SET_HUB_DEPTH;
  DevReq.Value        = Depth;
  DevReq.Index        = 0;
  DevReq.Length       = 0;

  Timeout             = 3000;
  EfiStatus = UsbIo->UsbControlTransfer (
                      UsbIo,
                      &DevReq,
                      EfiUsbNoData,
                      Timeout,
                      NULL,
                      0,
                      &UsbStatus
                      );

  return EfiStatus;
}

/**

  Configure the hub

  @param  HubController         Indicating the hub controller device that
                                will be configured
                              
  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR
  
**/
EFI_STATUS
DoHubConfig (
  IN USB_IO_CONTROLLER_DEVICE   *HubController
  )
{
  EFI_USB_IO_PROTOCOL     *UsbIo;
  EFI_USB_HUB_DESCRIPTOR  HubDescriptor;
  EFI_STATUS              Status;
  EFI_USB_HUB_STATUS      HubStatus;
  UINTN                   Index;

  UsbIo = &HubController->UsbIo;

  ZeroMem (&HubDescriptor, sizeof (HubDescriptor));

  //
  // First get the hub descriptor length
  //
  Status = GetHubDescriptor (UsbIo, 2, &HubDescriptor);
  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }
    
  //
  // First get the whole descriptor, then
  // get the number of hub ports
  //
  Status = GetHubDescriptor (
            UsbIo,
            HubDescriptor.Length,
            &HubDescriptor
            );
  if (EFI_ERROR (Status)) {
    DEBUG ((gUSBErrorLevel, "Get hub descriptor fail\n"));
    return EFI_DEVICE_ERROR;
  }

  HubController->DownstreamPorts  = HubDescriptor.NbrPorts;
  //
  //  Power all the hub ports
  //
  for (Index = 0; Index < HubController->DownstreamPorts; Index++) {
    Status = HubSetPortFeature (
              UsbIo,
              (UINT8) (Index + 1),
              EfiUsbPortPower
              );
    if (EFI_ERROR (Status)) {
      continue;
    }
    if (Index < (UINTN) HubController->DownstreamPorts - 1 && HubDelay (HubController->UsbDevice, 0) != 0) {
      //
      // Make 1ms stall between set each of port power to workaround specific device firmware fatal
      //
      mPrivate->UsbCore->Stall (1000);
    }
  }
  //
  // Performing HUB delay after hub port reset
  //
  HubDelay (HubController->UsbDevice, HubDescriptor.PwrOn2PwrGood + 200);
  //
  // Clear Hub Status Change
  //
  Status = HubGetHubStatus (UsbIo, (UINT32 *) &HubStatus);
  if (EFI_ERROR (Status)) {
    DEBUG ((gUSBErrorLevel, "Get hub status fail\n"));
    return EFI_DEVICE_ERROR;
  } else {
    //
    // Hub power supply change happens
    //
    if (HubStatus.HubChange & HUB_CHANGE_LOCAL_POWER) {
      HubClearHubFeature (UsbIo, C_HUB_LOCAL_POWER);
    }
    //
    // Hub change overcurrent happens
    //
    if (HubStatus.HubChange & HUB_CHANGE_OVERCURRENT) {
      HubClearHubFeature (UsbIo, C_HUB_OVER_CURRENT);
    }
  }
  //
  // Set Hub Depth for Super Speed Hub
  //
  if (HubDescriptor.DescriptorType == USB_DESC_TYPE_SSHUB) {
    Status = SetHubDepth (
               UsbIo,
               HubController->UsbDevice->HubDepth
               );
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }
  return EFI_SUCCESS;

}

/**

  Tell if a usb controller is a hub controller.
  
  @param  Dev                   UsbIoController device structure.
  
  @retval TRUE
  @retval FALSE

**/
BOOLEAN
IsHub (
  IN USB_IO_CONTROLLER_DEVICE   *Dev
  )
{
  EFI_USB_INTERFACE_DESCRIPTOR  Interface;
  EFI_USB_IO_PROTOCOL           *UsbIo;
  EFI_USB_ENDPOINT_DESCRIPTOR   EndpointDescriptor;
  UINT8                         Index;

  if (Dev == NULL) {
    return FALSE;
  }

  UsbIo = &Dev->UsbIo;

  UsbIo->UsbGetInterfaceDescriptor (
           UsbIo,
           &Interface
           );

  //
  // Check classcode
  //
  if (Interface.InterfaceClass != CLASS_CODE_HUB) {
    return FALSE;
  }
  
  //
  // Check protocol
  //
  if (Interface.InterfaceSubClass != SUB_CLASS_CODE_HUB &&
      Interface.InterfaceSubClass != SUB_CLASS_CODE_HUB1) {
    return FALSE;
  }

  for (Index = 0; Index < Interface.NumEndpoints; Index++) {
    UsbIo->UsbGetEndpointDescriptor (
             UsbIo,
             Index,
             &EndpointDescriptor
             );

    if ((EndpointDescriptor.EndpointAddress & 0x80) == 0) {
      continue;
    }

    if (EndpointDescriptor.Attributes != 0x03 && EndpointDescriptor.Attributes != 0x13) {
      continue;
    }

    Dev->HubEndpointAddress = EndpointDescriptor.EndpointAddress;
    Dev->MultiTT            = (Interface.InterfaceSubClass == SUB_CLASS_CODE_HUB1) ? 1 : 0;
    return TRUE;
  }

  return FALSE;
}