/** @file
  PEI USB helper function header

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

#ifndef _PEI_USB_HELPER_H
#define _PEI_USB_HELPER_H

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
  );

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
  );

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
  );

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
  );

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
  );

/**

  Tell if the connected device is a low device.

  @param  PortStatus            The status value of that port.

  @retval TRUE
  @retval FALSE

**/
BOOLEAN
IsPortLowSpeedDeviceAttached (
  IN UINT16  PortStatus
  );

/**

  Tell if there is a Connect Change status in that port.

  @param  PortChangeStatus      The status value of that port.

  @retval TRUE
  @retval FALSE

**/
BOOLEAN
IsPortConnectChange (
  IN UINT16  PortChangeStatus
  );

#endif
