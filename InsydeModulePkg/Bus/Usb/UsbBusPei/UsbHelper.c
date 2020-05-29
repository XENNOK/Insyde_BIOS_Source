/** @file
  PEI USB helper function

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

/**

  Pei Usb Get Descriptor

  @param  PeiServices           EFI_PEI_SERVICES
  @param  UsbIoPpi              PEI_USB3_IO_PPI
  @param  Value                 Request Value
  @param  Index                 Request Index
  @param  DescriptorLength      Descriptor Length
  @param  Descriptor            Descriptor

  @retval EFI_SUCCESS           Success

**/
EFI_STATUS
PeiUsbGetDescriptor (
  IN  EFI_PEI_SERVICES         **PeiServices,
  IN  PEI_USB3_IO_PPI          *UsbIoPpi,
  IN  UINT16                   Value,
  IN  UINT16                   Index,
  IN  UINT16                   DescriptorLength,
  OUT VOID                     *Descriptor
  )
{
  EFI_USB_DEVICE_REQUEST  DevReq;

  DevReq.RequestType  = USB_DEV_GET_DESCRIPTOR_REQ_TYPE;
  DevReq.Request      = USB_DEV_GET_DESCRIPTOR;
  DevReq.Value        = Value;
  DevReq.Index        = Index;
  DevReq.Length       = DescriptorLength;

  return UsbIoPpi->UsbControlTransfer (
                     PeiServices,
                     UsbIoPpi,
                     &DevReq,
                     EfiUsbDataIn,
                     TIMEOUT_VALUE,
                     Descriptor,
                     DescriptorLength
                     );
}

/**

  Pei Set Address

  @param  PeiServices           EFI_PEI_SERVICES
  @param  UsbIoPpi              PEI_USB3_IO_PPI
  @param  AddressValue          AddressValue
 
  @retval EFI_SUCCESS           Success

**/
EFI_STATUS
PeiUsbSetDeviceAddress (
  IN EFI_PEI_SERVICES         **PeiServices,
  IN PEI_USB3_IO_PPI          *UsbIoPpi,
  IN UINT16                   AddressValue
  )
{
  EFI_USB_DEVICE_REQUEST  DevReq;

  DevReq.RequestType  = USB_DEV_SET_ADDRESS_REQ_TYPE;
  DevReq.Request      = USB_DEV_SET_ADDRESS;
  DevReq.Value        = AddressValue;
  DevReq.Index        = 0;
  DevReq.Length       = 0;

  return UsbIoPpi->UsbControlTransfer (
                     PeiServices,
                     UsbIoPpi,
                     &DevReq,
                     EfiUsbNoData,
                     TIMEOUT_VALUE,
                     NULL,
                     0
                     );

}

/**

  Pei Usb Clear Device Feature

  @param  PeiServices           EFI_PEI_SERVICES
  @param  UsbIoPpi              PEI_USB3_IO_PPI
  @param  Recipient             Device/Interface/Endpoint
  @param  Value                 Request Value
  @param  Target                Request Index

  @retval EFI_SUCCESS           Success

**/
EFI_STATUS
PeiUsbClearDeviceFeature (
  IN EFI_PEI_SERVICES         **PeiServices,
  IN PEI_USB3_IO_PPI          *UsbIoPpi,
  IN UINT8                    Recipient,
  IN UINT16                   Value,
  IN UINT16                   Target
  )
{
  EFI_USB_DEVICE_REQUEST  DevReq;

  DevReq.RequestType  = Recipient;
  DevReq.Request      = USB_DEV_CLEAR_FEATURE;
  DevReq.Value        = Value;
  DevReq.Index        = Target;
  DevReq.Length       = 0;

  return UsbIoPpi->UsbControlTransfer (
                     PeiServices,
                     UsbIoPpi,
                     &DevReq,
                     EfiUsbNoData,
                     TIMEOUT_VALUE,
                     NULL,
                     0
                     );

}

/**

  Pei Usb Set Configuration to 1

  @param  PeiServices           EFI_PEI_SERVICES
  @param  UsbIoPpi              PEI_USB3_IO_PPI
 
  @retval EFI_SUCCESS           Success

**/
EFI_STATUS
PeiUsbSetConfiguration (
  IN EFI_PEI_SERVICES         **PeiServices,
  IN PEI_USB3_IO_PPI          *UsbIoPpi
  )
{
  EFI_USB_DEVICE_REQUEST  DevReq;
  ZeroMem (&DevReq, sizeof (EFI_USB_DEVICE_REQUEST));

  DevReq.RequestType  = USB_DEV_SET_CONFIGURATION_REQ_TYPE;
  DevReq.Request      = USB_DEV_SET_CONFIGURATION;
  DevReq.Value        = 1;
  
  return UsbIoPpi->UsbControlTransfer (
                     PeiServices,
                     UsbIoPpi,
                     &DevReq,
                     EfiUsbNoData,
                     TIMEOUT_VALUE,
                     NULL,
                     0
                     );
}

/**

  Tell if there is a device connected to that port according to
  the Port Status.

  @param  PortStatus            The status value of that port.

  @retval TRUE
  @retval FALSE

**/
BOOLEAN
IsPortConnect (
  IN UINT16  PortStatus
  )
{
  //
  // return the bit 0 value of PortStatus
  //
  if ((PortStatus & USB_PORT_STAT_CONNECTION) != 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**

  Tell if the connected device is a low device.

  @param  PortStatus            The status value of that port.

  @retval TRUE
  @retval FALSE

**/
BOOLEAN
IsPortLowSpeedDeviceAttached (
  IN UINT16  PortStatus
  )
{
  //
  // return the bit 9 value of PortStatus
  //
  if ((PortStatus & USB_PORT_STAT_LOW_SPEED) != 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**

  Tell if there is a Connect Change status in that port.

  @param  PortChangeStatus      The status value of that port.

  @retval TRUE
  @retval FALSE

**/
BOOLEAN
IsPortConnectChange (
  IN UINT16  PortChangeStatus
  )
{
  //
  // return the bit 0 value of PortChangeStatus
  //
  if ((PortChangeStatus & USB_PORT_STAT_C_CONNECTION) != 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}
