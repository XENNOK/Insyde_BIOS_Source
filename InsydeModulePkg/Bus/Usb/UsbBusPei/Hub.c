/** @file
  Usb Hub Request Support In PEI Phase

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

#include "UsbBus.h"
#include "Hub.h"
#include "UsbHelper.h"

/**

  Get a given hub port status

  @param  PeiServices           EFI_PEI_SERVICES
  @param  UsbIoPpi              PEI_USB3_IO_PPI instance
  @param  Port                  Usb hub port number (starting from 1).
  @param  PortStatus            Current Hub port status and change status.
  
  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR
  @retval EFI_TIME_OUT
  @retval EFI_INVALID_PARAMETER
  
**/
EFI_STATUS
PeiHubGetPortStatus (
  IN  EFI_PEI_SERVICES    **PeiServices,
  IN  PEI_USB3_IO_PPI     *UsbIoPpi,
  IN  UINT8               Port, 
  OUT UINT32              *PortStatus 
  ) 
{
  EFI_USB_DEVICE_REQUEST  DeviceRequest;
  EFI_STATUS              Status;
  PEI_USB_DEVICE          *PeiUsbDev;
  UINT32                  HubPortStatus;
  
  PeiUsbDev = PEI_USB_DEVICE_FROM_THIS (UsbIoPpi);

  ZeroMem (&DeviceRequest, sizeof (EFI_USB_DEVICE_REQUEST));

  //
  // Fill Device request packet
  //
  DeviceRequest.RequestType = HUB_GET_PORT_STATUS_REQ_TYPE;
  DeviceRequest.Request     = HUB_GET_PORT_STATUS;
  DeviceRequest.Index       = Port;
  DeviceRequest.Length      = sizeof (UINT32);


  Status = UsbIoPpi->UsbControlTransfer (
                       PeiServices,
                       UsbIoPpi,
                       &DeviceRequest,
                       EfiUsbDataIn,
                       TIMEOUT_VALUE,
                       PortStatus,
                       sizeof (UINT32)
                       );
  
  if (!EFI_ERROR (Status)) {
    //
    // SuperSpeed Hub Status Conversion
    //
    if (PeiUsbDev->DeviceSpeed == EFI_USB_SPEED_SUPER) {
      HubPortStatus = *PortStatus;
      (*PortStatus) &= ~0x0000FFE0;
      if (HubPortStatus & USB_PORT_STAT_SUPER_SPEED_POWER) {
        //
        // Convert Port Power Status
        //
        (*PortStatus) |= USB_PORT_STAT_POWER;
      }
      if ((HubPortStatus & USB_PORT_STAT_SUPER_SPEED_MASK) == 0) {
        //
        // Convert Port Speed Status
        //
        (*PortStatus) |= USB_PORT_STAT_SUPER_SPEED;
      }
    }
  }
  return Status;
} 

/**

  Set specified feature to a give hub port

  @param  PeiServices           EFI_PEI_SERVICES  
  @param  UsbIoPpi              EFI_USB_IO_PROTOCOL instance
  @param  Port                  Usb hub port number (starting from 1).
  @param  Value                 New feature value.

  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR
  @retval EFI_TIME_OUT
  @retval EFI_INVALID_PARAMETER

**/
EFI_STATUS 
PeiHubSetPortFeature ( 
  IN EFI_PEI_SERVICES    **PeiServices,
  IN PEI_USB3_IO_PPI     *UsbIoPpi,
  IN UINT8               Port, 
  IN UINT8               Value 
  )
{
  EFI_USB_DEVICE_REQUEST      DeviceRequest;

  ZeroMem (&DeviceRequest, sizeof (EFI_USB_DEVICE_REQUEST));
  
  //
  // Fill Device request packet
  //
  DeviceRequest.RequestType = HUB_SET_PORT_FEATURE_REQ_TYPE;
  DeviceRequest.Request     = HUB_SET_PORT_FEATURE;
  DeviceRequest.Value       = Value;
  DeviceRequest.Index       = Port;

  return UsbIoPpi->UsbControlTransfer (
                     PeiServices,
                     UsbIoPpi,
                     &DeviceRequest,
                     EfiUsbNoData,
                     TIMEOUT_VALUE,
                     NULL,
                     0
                     );

}

/**

  Clear a specified feature of a given hub port

  @param  PeiServices           EFI_PEI_SERVICES
  @param  UsbIoPpi              EFI_USB_IO_PROTOCOL instance
  @param  Port                  Usb hub port number (starting from 1).
  @param  Value                 Feature value that will be cleared from
                                that hub port.

  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR
  @retval EFI_TIME_OUT
  @retval EFI_INVALID_PARAMETER

**/
EFI_STATUS
PeiHubClearPortFeature (
  IN EFI_PEI_SERVICES    **PeiServices,
  IN PEI_USB3_IO_PPI     *UsbIoPpi,
  IN UINT8               Port, 
  IN UINT8               Value
  )
{
  EFI_USB_DEVICE_REQUEST      DeviceRequest;

  ZeroMem (&DeviceRequest, sizeof (EFI_USB_DEVICE_REQUEST));
  
  //
  // Fill Device request packet
  //
  DeviceRequest.RequestType = HUB_CLEAR_FEATURE_PORT_REQ_TYPE;
  DeviceRequest.Request     = HUB_CLEAR_FEATURE_PORT;
  DeviceRequest.Value       = Value;
  DeviceRequest.Index       = Port;

  return UsbIoPpi->UsbControlTransfer (
                     PeiServices,
                     UsbIoPpi,
                     &DeviceRequest,
                     EfiUsbNoData,
                     TIMEOUT_VALUE,
                     NULL,
                     0
                     );

}

/*++

  Get Hub Status

  @param  PeiServices           EFI_PEI_SERVICES
  @param  UsbIoPpi              EFI_USB_IO_PROTOCOL instance
  @param  HubStatus             Current Hub status and change status.

  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR
  @retval EFI_TIME_OUT

--*/
EFI_STATUS
PeiHubGetHubStatus (
  IN  EFI_PEI_SERVICES    **PeiServices,
  IN  PEI_USB3_IO_PPI     *UsbIoPpi,
  OUT UINT32              *HubStatus 
  ) 
{
  EFI_USB_DEVICE_REQUEST  DeviceRequest;
    
  ZeroMem (&DeviceRequest, sizeof (EFI_USB_DEVICE_REQUEST));
  
  //
  // Fill Device request packet
  //
  DeviceRequest.RequestType = HUB_GET_HUB_STATUS_REQ_TYPE;
  DeviceRequest.Request     = HUB_GET_HUB_STATUS;
  DeviceRequest.Length      = sizeof (UINT32);

  return UsbIoPpi->UsbControlTransfer (
                     PeiServices,
                     UsbIoPpi,
                     &DeviceRequest,
                     EfiUsbDataIn,
                     TIMEOUT_VALUE,
                     HubStatus,
                     sizeof (UINT32)
                     );

}

/**

  Set a specified feature to the hub

  @param  PeiServices           EFI_PEI_SERVICES 
  @param  UsbIoPpi              EFI_USB_IO_PROTOCOL instance
  @param  Value                 Feature value that will be set to the hub.

  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR
  @retval EFI_TIME_OUT

**/
EFI_STATUS
PeiHubSetHubFeature (
  IN EFI_PEI_SERVICES    **PeiServices,
  IN PEI_USB3_IO_PPI     *UsbIoPpi,
  IN UINT8               Value
  )
{
  EFI_USB_DEVICE_REQUEST      DeviceRequest;
  
  ZeroMem (&DeviceRequest, sizeof (EFI_USB_DEVICE_REQUEST));

  //
  // Fill Device request packet
  //
  DeviceRequest.RequestType = HUB_SET_HUB_FEATURE_REQ_TYPE;
  DeviceRequest.Request     = HUB_SET_HUB_FEATURE;
  DeviceRequest.Value       = Value;

  return UsbIoPpi->UsbControlTransfer (
                     PeiServices,
                     UsbIoPpi,
                     &DeviceRequest,
                     EfiUsbNoData,
                     TIMEOUT_VALUE,
                     NULL,
                     0
                     );

}

/**

  Set a specified feature to the hub

  @param  PeiServices           EFI_PEI_SERVICES
  @param  UsbIoPpi              EFI_USB_IO_PROTOCOL instance
  @param  Value                 Feature value that will be cleared from the hub.

  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR
  @retval EFI_TIME_OUT

**/
EFI_STATUS
PeiHubClearHubFeature (
  IN EFI_PEI_SERVICES    **PeiServices,
  IN PEI_USB3_IO_PPI     *UsbIoPpi,
  IN UINT8               Value
  )
{
  EFI_USB_DEVICE_REQUEST      DeviceRequest;

  ZeroMem (&DeviceRequest, sizeof (EFI_USB_DEVICE_REQUEST));

  //
  // Fill Device request packet
  //
  DeviceRequest.RequestType = HUB_CLEAR_FEATURE_REQ_TYPE;
  DeviceRequest.Request     = HUB_CLEAR_FEATURE;
  DeviceRequest.Value       = Value;

  return  UsbIoPpi->UsbControlTransfer (
                      PeiServices,
                      UsbIoPpi,
                      &DeviceRequest,
                      EfiUsbNoData,
                      TIMEOUT_VALUE,
                      NULL,
                      0
                      );

}

/**

  Get the hub descriptor

  @param  PeiServices           PeiGetHubDescriptor
  @param  UsbIoPpi              EFI_USB_IO_PROTOCOL instance
  @param  DescriptorSize        The length of Hub Descriptor buffer.
  @param  HubDescriptor         Caller allocated buffer to store the hub descriptor
                                if successfully returned.
                            
  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR
  @retval EFI_TIME_OUT

**/
EFI_STATUS
PeiGetHubDescriptor (
  IN  EFI_PEI_SERVICES          **PeiServices,
  IN  PEI_USB3_IO_PPI           *UsbIoPpi,
  IN  UINTN                     DescriptorSize,
  OUT EFI_USB_HUB_DESCRIPTOR    *HubDescriptor
  )
{
  EFI_USB_DEVICE_REQUEST      DevReq;
  PEI_USB_DEVICE              *PeiUsbDev;
  
  PeiUsbDev = PEI_USB_DEVICE_FROM_THIS (UsbIoPpi);
  
  ZeroMem (&DevReq, sizeof (EFI_USB_DEVICE_REQUEST));

  //
  // Fill Device request packet
  //
  DevReq.RequestType = USB_REQ_TYPE_CLASS | USB_TARGET_DEVICE | USB_ENDPOINT_DIR_IN;
  DevReq.Request     = HUB_GET_DESCRIPTOR;
  DevReq.Value       = ((PeiUsbDev->DeviceSpeed == EFI_USB_SPEED_SUPER) ? USB_DESC_TYPE_SSHUB : USB_DESC_TYPE_HUB) << 8;
  DevReq.Length      = (UINT16)DescriptorSize;

  return  UsbIoPpi->UsbControlTransfer (
                      PeiServices,
                      UsbIoPpi,
                      &DevReq,
                      EfiUsbDataIn,
                      TIMEOUT_VALUE,
                      HubDescriptor,
                      (UINT16)DescriptorSize
                      );


}

/**

  Set the hub depth for super speed hub

  @param  PeiServices           PeiGetHubDescriptor
  @param  UsbIoPpi              EFI_USB_IO_PROTOCOL instance
  @param  Depth                 The depth of hub
                            
  @retval EFI_SUCCESS
  @retval EFI_DEVICE
  @retval EFI_TIME_OUT

**/
STATIC
EFI_STATUS
SetHubDepth (
  IN EFI_PEI_SERVICES    **PeiServices,
  IN PEI_USB3_IO_PPI     *UsbIoPpi,
  IN UINT8               Depth
  )
{
  EFI_USB_DEVICE_REQUEST      DeviceRequest;
  
  ZeroMem (&DeviceRequest, sizeof (EFI_USB_DEVICE_REQUEST));

  //
  // Fill Device request packet
  //
  DeviceRequest.RequestType = HUB_SET_HUB_DEPTH_REQ_TYPE;
  DeviceRequest.Request     = HUB_SET_HUB_DEPTH;
  DeviceRequest.Value       = Depth;

  return UsbIoPpi->UsbControlTransfer (
                     PeiServices,
                     UsbIoPpi,
                     &DeviceRequest,
                     EfiUsbNoData,
                     TIMEOUT_VALUE,
                     NULL,
                     0
                     );
}

/**

  Configure the hub

  @param  PeiUsbDevice          Indicating the hub controller device that
                                will be configured
                              
  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR
  
**/
EFI_STATUS
DoHubConfig (
  IN PEI_USB_DEVICE      *PeiUsbDevice
  )
{
  EFI_PEI_SERVICES        **PeiServices;
  EFI_USB_HUB_DESCRIPTOR  HubDescriptor;
  EFI_STATUS              Status;
  EFI_USB_HUB_STATUS      HubStatus;
  UINTN                   i;
  UINT32                  PortStatus;
  PEI_USB3_IO_PPI         *UsbIoPpi;

  ZeroMem (&HubDescriptor, sizeof (HubDescriptor));
  PeiServices = PeiUsbDevice->PeiServices;
  UsbIoPpi    = &PeiUsbDevice->UsbIoPpi;

  //
  // First get the hub descriptor length
  //
  Status = PeiGetHubDescriptor (
             PeiServices,
             UsbIoPpi,
             2,
             &HubDescriptor
             );
  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }
  //
  // First get the whole descriptor, then
  // get the number of hub ports
  //
  Status = PeiGetHubDescriptor (
             PeiServices,
             UsbIoPpi,
             HubDescriptor.Length,
             &HubDescriptor
             );
  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }

  PeiUsbDevice->DownStreamPortNo = HubDescriptor.NbrPorts;

  Status = PeiHubGetHubStatus (
             PeiServices,
             UsbIoPpi,
             (UINT32 *) &HubStatus
             );

  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }
  //
  //  Get all hub ports status
  //
  for (i = 0; i < PeiUsbDevice->DownStreamPortNo; i++) {

    Status = PeiHubGetPortStatus (
               PeiServices,
               UsbIoPpi,
               (UINT8) (i + 1),
               &PortStatus
               );
    if (EFI_ERROR (Status)) {
      continue;
    }
  }
  //
  //  Power all the hub ports
  //
  for (i = 0; i < PeiUsbDevice->DownStreamPortNo; i++) {
    Status = PeiHubSetPortFeature (
               PeiServices,
               UsbIoPpi,
               (UINT8) (i + 1),
               EfiUsbPortPower
               );
    if (EFI_ERROR (Status)) {
      continue;
    }
  }
  //
  // Stall HubDescriptor.PwrOn2PwrGood plus 200 millisecond for hub ports power on
  //
  PeiUsbDevice->StallPpi->Stall (
                            (CONST EFI_PEI_SERVICES **)PeiServices,
                            PeiUsbDevice->StallPpi,
                            (HubDescriptor.PwrOn2PwrGood + 200) * 1000
                            );
  //
  // Clear Hub Status Change
  //
  Status = PeiHubGetHubStatus (
             PeiServices,
             UsbIoPpi,
             (UINT32 *) &HubStatus
             );
  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  } else {
    //
    // Hub power supply change happens
    //
    if (HubStatus.HubChange & HUB_CHANGE_LOCAL_POWER) {
      PeiHubClearHubFeature (
        PeiServices,
        UsbIoPpi,
        C_HUB_LOCAL_POWER
        );
    }
    //
    // Hub change overcurrent happens
    //
    if (HubStatus.HubChange & HUB_CHANGE_OVERCURRENT) {
      PeiHubClearHubFeature (
        PeiServices,
        UsbIoPpi,
        C_HUB_OVER_CURRENT
        );
    }
  }
  //
  // Set Hub Depth for Super Speed Hub
  //
  if (HubDescriptor.DescriptorType == USB_DESC_TYPE_SSHUB) {
    Status = SetHubDepth (
               PeiServices,
               UsbIoPpi,
               PeiUsbDevice->HubDepth
               );
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }
  return EFI_SUCCESS;
}

/**

  Reset Hub Port

  @param  PeiUsbDevice          Indicating the hub controller device that
                                will be configured
**/
VOID
ResetHubPort (
  IN PEI_USB_DEVICE      *PeiUsbDevice,
  IN UINT8               PortNum
  )
{
  EFI_PEI_SERVICES    **PeiServices;
  EFI_USB_PORT_STATUS HubPortStatus;
  PEI_USB3_IO_PPI     *UsbIoPpi;
  UINT8               n;

  PeiServices = PeiUsbDevice->PeiServices;
  UsbIoPpi    = &PeiUsbDevice->UsbIoPpi;
  PeiUsbDevice->StallPpi->Stall (
                            (CONST EFI_PEI_SERVICES **)PeiServices,
                            PeiUsbDevice->StallPpi,
                            100 * 1000
                            );   
  //
  // reset root port
  //
  PeiHubSetPortFeature (
    PeiServices,
    UsbIoPpi,
    PortNum,
    EfiUsbPortReset
    );

  //
  // Drive the reset signal for at least 10ms. Check USB 2.0 Spec
  // section 7.1.7.5 for timing requirements.
  //
  PeiUsbDevice->StallPpi->Stall (
                            (CONST EFI_PEI_SERVICES **)PeiServices,
                            PeiUsbDevice->StallPpi,
                            10 * 1000
                            );          

  n = 10;
  do {
    PeiHubGetPortStatus (
      PeiServices,
      UsbIoPpi,
      PortNum,
      (UINT32 *) &HubPortStatus
      );
    PeiUsbDevice->StallPpi->Stall (
                              (CONST EFI_PEI_SERVICES **)PeiServices,
                              PeiUsbDevice->StallPpi,
                              2 * 1000
                              );
    n -= 1;
  } while ((HubPortStatus.PortChangeStatus & USB_PORT_STAT_C_RESET) == 0 && n > 0);

  //
  // USB hub will clear RESET bit if reset is actually finished.
  //

  PeiHubClearPortFeature (
    PeiServices,
    UsbIoPpi,
    PortNum,
    EfiUsbPortResetChange
    );

  PeiHubClearPortFeature (
    PeiServices,
    UsbIoPpi,
    PortNum,
    EfiUsbPortConnectChange
    );

  n = 10;
  do {
    PeiHubGetPortStatus (
      PeiServices,
      UsbIoPpi,
      PortNum,
      (UINT32 *) &HubPortStatus
      );
    PeiUsbDevice->StallPpi->Stall (
                              (CONST EFI_PEI_SERVICES **)PeiServices,
                              PeiUsbDevice->StallPpi,
                              2 * 1000
                              );
    n -= 1;
  } while ((HubPortStatus.PortChangeStatus & USB_PORT_STAT_C_RESET) != 0 && n > 0);

}
