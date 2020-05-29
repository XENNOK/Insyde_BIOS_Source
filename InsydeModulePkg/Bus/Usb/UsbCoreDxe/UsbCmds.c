/** @file
  Common Command for USB

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "UsbCoreDxe.h"
#include "UsbCmds.h"

/**

  Usb Get Descriptor

  @param  UsbIo                 EFI_USB_IO_PROTOCOL
  @param  Value                 Device Request Value
  @param  Index                 Device Request Index 
  @param  DescriptorLength      Descriptor Length
  @param  Descriptor            Descriptor buffer to contain result
  @param  Status                Transfer Status

  @retval EFI_INVALID_PARAMETER Parameter is error
  @retval EFI_SUCCESS           Success
  @retval EFI_TIMEOUT           Device has no response 

**/
EFI_STATUS
EFIAPI
UsbGetDescriptor (
  IN  EFI_USB_IO_PROTOCOL     *UsbIo,
  IN  UINT16                  Value,
  IN  UINT16                  Index,
  IN  UINT16                  DescriptorLength,
  OUT VOID                    *Descriptor,
  OUT UINT32                  *Status
  )
{
  EFI_USB_DEVICE_REQUEST  DevReq;

  if (UsbIo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem (&DevReq, sizeof (EFI_USB_DEVICE_REQUEST));

  DevReq.RequestType  = USB_DEV_GET_DESCRIPTOR_REQ_TYPE;
  DevReq.Request      = USB_DEV_GET_DESCRIPTOR;
  DevReq.Value        = Value;
  DevReq.Index        = Index;
  DevReq.Length       = DescriptorLength;

  return UsbIo->UsbControlTransfer (
                  UsbIo,
                  &DevReq,
                  EfiUsbDataIn,
                  TIMEOUT_VALUE,
                  Descriptor,
                  DescriptorLength,
                  Status
                  );
}

/**

  Usb Set Descriptor

  @param  UsbIo                 EFI_USB_IO_PROTOCOL
  @param  Value                 Device Request Value
  @param  Index                 Device Request Index 
  @param  DescriptorLength      Descriptor Length
  @param  Descriptor            Descriptor buffer to set
  @param  Status                Transfer Status

  @retval EFI_INVALID_PARAMETER Parameter is error
  @retval EFI_SUCCESS           Success
  @retval EFI_TIMEOUT           Device has no response 

**/
EFI_STATUS
EFIAPI
UsbSetDescriptor (
  IN  EFI_USB_IO_PROTOCOL     *UsbIo,
  IN  UINT16                  Value,
  IN  UINT16                  Index,
  IN  UINT16                  DescriptorLength,
  IN  VOID                    *Descriptor,
  OUT UINT32                  *Status
  )
{
  EFI_USB_DEVICE_REQUEST  DevReq;

  if (UsbIo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem (&DevReq, sizeof (EFI_USB_DEVICE_REQUEST));

  DevReq.RequestType  = USB_DEV_SET_DESCRIPTOR_REQ_TYPE;
  DevReq.Request      = USB_DEV_SET_DESCRIPTOR;
  DevReq.Value        = Value;
  DevReq.Index        = Index;
  DevReq.Length       = DescriptorLength;

  return UsbIo->UsbControlTransfer (
                  UsbIo,
                  &DevReq,
                  EfiUsbDataOut,
                  TIMEOUT_VALUE,
                  Descriptor,
                  DescriptorLength,
                  Status
                  );
}

/**

  Usb Get Device Interface

  @param  UsbIo                 EFI_USB_IO_PROTOCOL
  @param  Index                 Interface index value
  @param  AltSetting            Alternate setting
  @param  Status                Trasnsfer status

  @retval EFI_INVALID_PARAMETER Parameter is error
  @retval EFI_SUCCESS           Success
  @retval EFI_TIMEOUT           Device has no response 

**/
EFI_STATUS
EFIAPI
UsbGetDeviceInterface (
  IN  EFI_USB_IO_PROTOCOL     *UsbIo,
  IN  UINT16                  Index,
  OUT UINT8                   *AltSetting,
  OUT UINT32                  *Status
  )
{
  EFI_USB_DEVICE_REQUEST  DevReq;

  if (UsbIo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem (&DevReq, sizeof (EFI_USB_DEVICE_REQUEST));

  DevReq.RequestType  = USB_DEV_GET_INTERFACE_REQ_TYPE;
  DevReq.Request      = USB_DEV_GET_INTERFACE;
  DevReq.Index        = Index;
  DevReq.Length       = 1;

  return UsbIo->UsbControlTransfer (
                  UsbIo,
                  &DevReq,
                  EfiUsbDataIn,
                  TIMEOUT_VALUE,
                  AltSetting,
                  1,
                  Status
                  );
}

/**

  Usb Set Device Interface

  @param  UsbIo                 EFI_USB_IO_PROTOCOL
  @param  InterfaceNo           Interface Number
  @param  AltSetting            Alternate setting
  @param  Status                Trasnsfer status

  @retval EFI_INVALID_PARAMETER Parameter is error
  @retval EFI_SUCCESS           Success
  @retval EFI_TIMEOUT           Device has no response 

**/
EFI_STATUS
EFIAPI
UsbSetDeviceInterface (
  IN  EFI_USB_IO_PROTOCOL     *UsbIo,
  IN  UINT16                  InterfaceNo,
  IN  UINT16                  AltSetting,
  OUT UINT32                  *Status
  )
{
  EFI_USB_DEVICE_REQUEST  DevReq;

  if (UsbIo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem (&DevReq, sizeof (EFI_USB_DEVICE_REQUEST));

  DevReq.RequestType  = USB_DEV_SET_INTERFACE_REQ_TYPE;
  DevReq.Request      = USB_DEV_SET_INTERFACE;
  DevReq.Value        = AltSetting;
  DevReq.Index        = InterfaceNo;
 

  return UsbIo->UsbControlTransfer (
                  UsbIo,
                  &DevReq,
                  EfiUsbNoData,
                  TIMEOUT_VALUE,
                  NULL,
                  0,
                  Status
                  );
}

/**

  Usb Get Device Configuration

  @param  UsbIo                 EFI_USB_IO_PROTOCOL
  @param  ConfigValue           Config Value
  @param  Status                Transfer Status

  @retval EFI_INVALID_PARAMETER Parameter is error
  @retval EFI_SUCCESS           Success
  @retval EFI_TIMEOUT           Device has no response 

**/
EFI_STATUS
EFIAPI
UsbGetDeviceConfiguration (
  IN  EFI_USB_IO_PROTOCOL     *UsbIo,
  OUT UINT8                   *ConfigValue,
  OUT UINT32                  *Status
  )
{
  EFI_USB_DEVICE_REQUEST  DevReq;

  if (UsbIo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem (&DevReq, sizeof (EFI_USB_DEVICE_REQUEST));

  DevReq.RequestType  = USB_DEV_GET_CONFIGURATION_REQ_TYPE;
  DevReq.Request      = USB_DEV_GET_CONFIGURATION;
  DevReq.Length       = 1;

  return UsbIo->UsbControlTransfer (
                  UsbIo,
                  &DevReq,
                  EfiUsbDataIn,
                  TIMEOUT_VALUE,
                  ConfigValue,
                  1,
                  Status
                  );
}

/**

  Usb Set Device Configuration

  @param  UsbIo                 EFI_USB_IO_PROTOCOL
  @param  Value                 Configuration Value to set
  @param  Status                Transfer status

  @retval EFI_INVALID_PARAMETER Parameter is error
  @retval EFI_SUCCESS           Success
  @retval EFI_TIMEOUT           Device has no response 

**/
EFI_STATUS
EFIAPI
UsbSetDeviceConfiguration (
  IN  EFI_USB_IO_PROTOCOL     *UsbIo,
  IN  UINT16                  Value,
  OUT UINT32                  *Status
  )
{
  EFI_USB_DEVICE_REQUEST  DevReq;

  if (UsbIo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem (&DevReq, sizeof (EFI_USB_DEVICE_REQUEST));

  DevReq.RequestType  = USB_DEV_SET_CONFIGURATION_REQ_TYPE;
  DevReq.Request      = USB_DEV_SET_CONFIGURATION;
  DevReq.Value        = Value;
 
  return UsbIo->UsbControlTransfer (
                  UsbIo,
                  &DevReq,
                  EfiUsbNoData,
                  TIMEOUT_VALUE,
                  NULL,
                  0,
                  Status
                  );
}

/**

  Usb Set Device Feature

  @param  UsbIo                 EFI_USB_IO_PROTOCOL
  @param  Recipient             Interface/Device/Endpoint
  @param  Value                 Request value
  @param  Target                Request Index
  @param  Status                Transfer status

  @retval EFI_INVALID_PARAMETER Parameter is error
  @retval EFI_SUCCESS           Success
  @retval EFI_TIMEOUT           Device has no response 

**/
EFI_STATUS
EFIAPI
UsbSetDeviceFeature (
  IN  EFI_USB_IO_PROTOCOL     *UsbIo,
  IN  UINT8                   Recipient,
  IN  UINT16                  Value,
  IN  UINT16                  Target,
  OUT UINT32                  *Status
  )
{
  EFI_USB_DEVICE_REQUEST  DevReq;

  if (UsbIo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem (&DevReq, sizeof (EFI_USB_DEVICE_REQUEST));
  //
  // Fill device request, see USB1.1 spec
  //
  DevReq.RequestType = Recipient;
  DevReq.Request     = USB_DEV_SET_FEATURE;
  DevReq.Value       = Value;
  DevReq.Index       = Target;


  return UsbIo->UsbControlTransfer (
                  UsbIo,
                  &DevReq,
                  EfiUsbNoData,
                  TIMEOUT_VALUE,
                  NULL,
                  0,
                  Status
                  );
}

/**

  Usb Clear Device Feature

  @param  UsbIo                 EFI_USB_IO_PROTOCOL
  @param  Recipient             Interface/Device/Endpoint
  @param  Value                 Request value
  @param  Target                Request Index
  @param  Status                Transfer status

  @retval EFI_INVALID_PARAMETER Parameter is error
  @retval EFI_SUCCESS           Success
  @retval EFI_TIMEOUT           Device has no response 

**/
EFI_STATUS
EFIAPI
UsbClearDeviceFeature (
  IN  EFI_USB_IO_PROTOCOL     *UsbIo,
  IN  UINT8                   Recipient,
  IN  UINT16                  Value,
  IN  UINT16                  Target,
  OUT UINT32                  *Status
  )
{
  EFI_USB_DEVICE_REQUEST  DevReq;

  if (UsbIo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem (&DevReq, sizeof (EFI_USB_DEVICE_REQUEST));
  //
  // Fill device request, see USB1.1 spec
  //
  DevReq.RequestType = Recipient;
  DevReq.Request     = USB_DEV_CLEAR_FEATURE;
  DevReq.Value       = Value;
  DevReq.Index       = Target;


  return UsbIo->UsbControlTransfer (
                  UsbIo,
                  &DevReq,
                  EfiUsbNoData,
                  TIMEOUT_VALUE,
                  NULL,
                  0,
                  Status
                  );
}

/**

  Usb Get Device Status

  @param  UsbIo                 EFI_USB_IO_PROTOCOL
  @param  Recipient             Interface/Device/Endpoint
  @param  Target                Request index
  @param  DevStatus             Device status
  @param  Status                Transfer status

  @retval EFI_INVALID_PARAMETER Parameter is error
  @retval EFI_SUCCESS           Success
  @retval EFI_TIMEOUT           Device has no response 

**/
EFI_STATUS
EFIAPI
UsbGetDeviceStatus (
  IN  EFI_USB_IO_PROTOCOL     *UsbIo,
  IN  UINT8                   Recipient,
  IN  UINT16                  Target,
  OUT UINT16                  *DevStatus,
  OUT UINT32                  *Status
  )
{
  EFI_USB_DEVICE_REQUEST  DevReq;

  if (UsbIo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem (&DevReq, sizeof (EFI_USB_DEVICE_REQUEST));
  //
  // Fill device request, see USB1.1 spec
  //
  DevReq.RequestType = Recipient | USB_ENDPOINT_DIR_IN;
  DevReq.Request     = USB_DEV_GET_STATUS;
  DevReq.Value       = 0;
  DevReq.Index       = Target;
  DevReq.Length      = 2;

  return UsbIo->UsbControlTransfer (
                  UsbIo,
                  &DevReq,
                  EfiUsbDataIn,
                  TIMEOUT_VALUE,
                  DevStatus,
                  2,
                  Status
                  );
}

/**

  Usb Get String

  @param  UsbIo                 EFI_USB_IO_PROTOCOL
  @param  LangID                Language ID
  @param  Index                 Request index
  @param  Buf                   Buffer to store string
  @param  BufSize               Buffer size
  @param  Status                Transfer status

  @retval EFI_INVALID_PARAMETER Parameter is error
  @retval EFI_SUCCESS           Success
  @retval EFI_TIMEOUT           Device has no response 

**/
EFI_STATUS
EFIAPI
UsbGetString (
  IN  EFI_USB_IO_PROTOCOL     *UsbIo,
  IN  UINT16                  LangID,
  IN  UINT8                   Index,
  IN  VOID                    *Buf,
  IN  UINTN                   BufSize,
  OUT UINT32                  *Status
  )
{
  UINT16  Value;

  if (UsbIo == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Fill value, see USB1.1 spec
  //
  Value = (UINT16) ((USB_DESC_TYPE_STRING << 8) | Index);

  return UsbGetDescriptor (
           UsbIo,
           Value,
           LangID,
           (UINT16) BufSize,
           Buf,
           Status
           );
}

/**

  Clear endpoint stall

  @param  UsbIo                 EFI_USB_IO_PROTOCOL
  @param  EndpointNo            Endpoint Number
  @param  Status                Transfer Status

  @retval EFI_NOT_FOUND         Can't find the Endpoint
  @retval EFI_DEVICE_ERROR      Hardware error
  @retval EFI_SUCCESS           Success

**/
EFI_STATUS
EFIAPI
UsbClearEndpointHalt (
  IN  EFI_USB_IO_PROTOCOL     *UsbIo,
  IN  UINT8                   EndpointNo,
  OUT UINT32                  *Status
  )
{
  EFI_STATUS                    Result;
  EFI_USB_ENDPOINT_DESCRIPTOR   EndpointDescriptor;
  EFI_USB_INTERFACE_DESCRIPTOR  InterfaceDescriptor;
  UINT8                         Index;

  ZeroMem (&EndpointDescriptor, sizeof (EFI_USB_ENDPOINT_DESCRIPTOR));
  //
  // First seach the endpoint descriptor for that endpoint addr
  //
  Result = UsbIo->UsbGetInterfaceDescriptor (
                    UsbIo,
                    &InterfaceDescriptor
                    );
  if (EFI_ERROR (Result)) {
    return Result;
  }

  for (Index = 0; Index < InterfaceDescriptor.NumEndpoints; Index++) {
    Result = UsbIo->UsbGetEndpointDescriptor (
                      UsbIo,
                      Index,
                      &EndpointDescriptor
                      );
    if (EFI_ERROR (Result)) {
      continue;
    }

    if (EndpointDescriptor.EndpointAddress == EndpointNo) {
      break;
    }
  }

  if (Index == InterfaceDescriptor.NumEndpoints) {
    //
    // No such endpoint
    //
    return EFI_NOT_FOUND;
  }

  Result = UsbClearDeviceFeature (
             UsbIo,
             USB_TARGET_ENDPOINT,
             USB_FEATURE_ENDPOINT_HALT,
             EndpointDescriptor.EndpointAddress,
             Status
             );

  return Result;
}

/**

  Get Hid Descriptor

  @param  UsbIo                 EFI_USB_IO_PROTOCOL
  @param  InterfaceNum          Hid interface number
  @param  HidDescriptor         Caller allocated buffer to store Usb hid descriptor
                                if successfully returned.

  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR
  @retval EFI_TIMEOUT

**/
EFI_STATUS
EFIAPI
UsbGetHidDescriptor (
  IN  EFI_USB_IO_PROTOCOL        *UsbIo,
  IN  UINT8                      InterfaceNum,
  OUT VOID                       *HidDescriptor
  )
{
  UINT32                  Status;
  HID_DESCRIPTOR_TOKEN    *HidDescriptorToken;
  EFI_STATUS              Result;
  EFI_USB_DEVICE_REQUEST  Request;
  //
  // In order to workaround some HID device failed on getting HID descriptor for
  // interface number large than 0, using internal data registered by Usb bus driver
  // instead of actual control transfer
  //
  if (!IsListEmpty(&mPrivate->HidDescriptorListHead)) {
    HidDescriptorToken = (HID_DESCRIPTOR_TOKEN*)&mPrivate->HidDescriptorListHead;
    do {
      HidDescriptorToken = (HID_DESCRIPTOR_TOKEN*)GetFirstNode((LIST_ENTRY*)HidDescriptorToken);
      if (HidDescriptorToken->UsbIo == UsbIo) {
        CopyMem (HidDescriptor, &HidDescriptorToken->HidDescriptor, sizeof (EFI_USB_HID_DESCRIPTOR));
        return EFI_SUCCESS;
      }
    } while (!IsNodeAtEnd(&mPrivate->HidDescriptorListHead, (LIST_ENTRY*)HidDescriptorToken));
  }
  Request.RequestType = 0x81;
  Request.Request     = 0x06;
  Request.Value       = (UINT16) (USB_DESC_TYPE_HID << 8);
  Request.Index       = InterfaceNum;
  Request.Length      = 9;
  Result = UsbIo->UsbControlTransfer (
                    UsbIo,
                    &Request,
                    EfiUsbDataIn,
                    TIMEOUT_VALUE,
                    HidDescriptor,
                    9,
                    &Status
                    );
  return Result;
}

/**

  Get Report Class descriptor

  @param  UsbIo                 EFI_USB_IO_PROTOCOL.
  @param  InterfaceNum          Report interface number.
  @param  DescriptorSize        Length of DescriptorBuffer.
  @param  DescriptorBuffer      Caller allocated buffer to store Usb report descriptor
                                if successfully returned.

  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR
  @retval EFI_TIMEOUT

**/
EFI_STATUS
EFIAPI
UsbGetReportDescriptor (
  IN  EFI_USB_IO_PROTOCOL     *UsbIo,
  IN  UINT8                   InterfaceNum,
  IN  UINT16                  DescriptorSize,
  OUT UINT8                   *DescriptorBuffer
  )
{
  UINT32                  Status;
  HID_DESCRIPTOR_TOKEN    *HidDescriptorToken;
  BOOLEAN                 RecordFound;
  UINT8                   *ReportDescriptor;
  EFI_STATUS              Result;
  EFI_USB_DEVICE_REQUEST  Request;
  //
  // In order to reduce the communication using internal cached data instead of actual control transfer
  //
  RecordFound        = FALSE;
  ReportDescriptor   = NULL;
  if (!IsListEmpty(&mPrivate->HidDescriptorListHead)) {
    HidDescriptorToken = (HID_DESCRIPTOR_TOKEN*)&mPrivate->HidDescriptorListHead;
    do {
      HidDescriptorToken = (HID_DESCRIPTOR_TOKEN*)GetFirstNode((LIST_ENTRY*)HidDescriptorToken);
      if (HidDescriptorToken->UsbIo == UsbIo) {
        //
        // Get the report descriptor record
        //
        ReportDescriptor = (UINT8*)((UINTN)&HidDescriptorToken->HidDescriptor + HidDescriptorToken->HidDescriptor.Length);
        if (*(UINT32*)ReportDescriptor != 0) {
          CopyMem (
            DescriptorBuffer,
            ReportDescriptor,
            DescriptorSize
            );
          return EFI_SUCCESS;
        } else {
          RecordFound = TRUE;
        }
        break;
      }
    } while (!IsNodeAtEnd(&mPrivate->HidDescriptorListHead, (LIST_ENTRY*)HidDescriptorToken));
  }
  //
  // Fill Device request packet
  //
  Request.RequestType = 0x81;
  Request.Request     = 0x06;
  Request.Value       = (UINT16) (USB_DESC_TYPE_REPORT << 8);
  Request.Index       = InterfaceNum;
  Request.Length      = DescriptorSize;
  Result = UsbIo->UsbControlTransfer (
                    UsbIo,
                    &Request,
                    EfiUsbDataIn,
                    TIMEOUT_VALUE,
                    DescriptorBuffer,
                    DescriptorSize,
                    &Status
                    );
  if (RecordFound) {
    //
    // Update report descriptor into token
    //
    CopyMem (
      ReportDescriptor,
      DescriptorBuffer,
      DescriptorSize
      );
  }
  return Result;
}

/**

  Get Hid Protocol Request

  @param  UsbIo                 EFI_USB_IO_PROTOCOL
  @param  Interface             Which interface the caller wants to get protocol
  @param  Protocol              Protocol value returned.

  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR
  @retval EFI_TIMEOUT

**/
EFI_STATUS
EFIAPI
UsbGetProtocolRequest (
  IN EFI_USB_IO_PROTOCOL     *UsbIo,
  IN UINT8                   Interface,
  IN UINT8                   *Protocol
  )
{
  UINT32                  Status;
  EFI_STATUS              Result;
  EFI_USB_DEVICE_REQUEST  Request;

  //
  // Fill Device request packet
  //
  Request.RequestType = 0xa1;
  //
  // 10100001b;
  //
  Request.Request = EFI_USB_GET_PROTOCOL_REQUEST;
  Request.Value   = 0;
  Request.Index   = Interface;
  Request.Length  = 1;

  Result = UsbIo->UsbControlTransfer (
                    UsbIo,
                    &Request,
                    EfiUsbDataIn,
                    TIMEOUT_VALUE,
                    Protocol,
                    sizeof (UINT8),
                    &Status
                    );

  return Result;
}

/**

  Set Hid Protocol Request

  @param  UsbIo                 EFI_USB_IO_PROTOCOL
  @param  Interface             Which interface the caller wants to set protocol
  @param  Protocol              Protocol value the caller wants to set.

  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR
  @retval EFI_TIMEOUT

**/
EFI_STATUS
EFIAPI
UsbSetProtocolRequest (
  IN EFI_USB_IO_PROTOCOL     *UsbIo,
  IN UINT8                   Interface,
  IN UINT8                   Protocol
  )
{
  UINT32                  Status;
  EFI_STATUS              Result;
  EFI_USB_DEVICE_REQUEST  Request;

  //
  // Fill Device request packet
  //
  Request.RequestType = 0x21;
  //
  // 00100001b;
  //
  Request.Request = EFI_USB_SET_PROTOCOL_REQUEST;
  Request.Value   = Protocol;
  Request.Index   = Interface;
  Request.Length  = 0;

  Result = UsbIo->UsbControlTransfer (
                    UsbIo,
                    &Request,
                    EfiUsbNoData,
                    TIMEOUT_VALUE,
                    NULL,
                    0,
                    &Status
                    );
  return Result;
}

/**

  Set Idel request.

  @param  UsbIo                 EFI_USB_IO_PROTOCOL
  @param  Interface             Which interface the caller wants to set.
  @param  ReportId              Which report the caller wants to set.
  @param  Duration              Idle rate the caller wants to set.

  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR
  @retval EFI_TIMEOUT

**/
EFI_STATUS
EFIAPI
UsbSetIdleRequest (
  IN EFI_USB_IO_PROTOCOL     *UsbIo,
  IN UINT8                   Interface,
  IN UINT8                   ReportId,
  IN UINT8                   Duration
  )
{
  UINT32                  Status;
  EFI_STATUS              Result;
  EFI_USB_DEVICE_REQUEST  Request;

  //
  // Fill Device request packet
  //
  Request.RequestType = 0x21;
  //
  // 00100001b;
  //
  Request.Request = EFI_USB_SET_IDLE_REQUEST;
  Request.Value   = (UINT16) ((Duration << 8) | ReportId);
  Request.Index   = Interface;
  Request.Length  = 0;

  Result = UsbIo->UsbControlTransfer (
                    UsbIo,
                    &Request,
                    EfiUsbNoData,
                    TIMEOUT_VALUE,
                    NULL,
                    0,
                    &Status
                    );
  return Result;
}

/**

  Get Idel request.

  @param  UsbIo                 EFI_USB_IO_PROTOCOL
  @param  Interface             Which interface the caller wants to get.
  @param  ReportId              Which report the caller wants to get.
  @param  Duration              Idle rate the caller wants to get.

  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR
  @retval EFI_TIMEOUT

**/
EFI_STATUS
EFIAPI
UsbGetIdleRequest (
  IN  EFI_USB_IO_PROTOCOL     *UsbIo,
  IN  UINT8                   Interface,
  IN  UINT8                   ReportId,
  OUT UINT8                   *Duration
  )
{
  UINT32                  Status;
  EFI_STATUS              Result;
  EFI_USB_DEVICE_REQUEST  Request;

  //
  // Fill Device request packet
  //
  Request.RequestType = 0xa1;
  //
  // 10100001b;
  //
  Request.Request = EFI_USB_GET_IDLE_REQUEST;
  Request.Value   = ReportId;
  Request.Index   = Interface;
  Request.Length  = 1;

  Result = UsbIo->UsbControlTransfer (
                    UsbIo,
                    &Request,
                    EfiUsbDataIn,
                    TIMEOUT_VALUE,
                    Duration,
                    1,
                    &Status
                    );

  return Result;
}

/**

  Hid Set Report request.

  @param  UsbIo                 EFI_USB_IO_PROTOCOL
  @param  Interface             Which interface the caller wants to set.
  @param  ReportId              Which report the caller wants to set.
  @param  ReportType            Type of report.
  @param  ReportLen             Length of report descriptor.
  @param  Report                Report Descriptor buffer.

  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR
  @retval EFI_TIMEOUT

**/
EFI_STATUS
EFIAPI
UsbSetReportRequest (
  IN EFI_USB_IO_PROTOCOL     *UsbIo,
  IN UINT8                   Interface,
  IN UINT8                   ReportId,
  IN UINT8                   ReportType,
  IN UINT16                  ReportLen,
  IN UINT8                   *Report
  )
{
  UINT32                  Status;
  EFI_STATUS              Result;
  EFI_USB_DEVICE_REQUEST  Request;

  //
  // Fill Device request packet
  //
  Request.RequestType = 0x21;
  //
  // 00100001b;
  //
  Request.Request = EFI_USB_SET_REPORT_REQUEST;
  Request.Value   = (UINT16) ((ReportType << 8) | ReportId);
  Request.Index   = Interface;
  Request.Length  = ReportLen;

  Result = UsbIo->UsbControlTransfer (
                    UsbIo,
                    &Request,
                    EfiUsbDataOut,
                    TIMEOUT_VALUE,
                    Report,
                    ReportLen,
                    &Status
                    );

  return Result;
}

/**

  Hid Set Report request.

  @param  UsbIo                 EFI_USB_IO_PROTOCOL
  @param  Interface             Which interface the caller wants to set.
  @param  ReportId              Which report the caller wants to set.
  @param  ReportType            Type of report.
  @param  ReportLen             Length of report descriptor.
  @param  Report                Caller allocated buffer to store Report Descriptor.

  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR
  @retval EFI_TIMEOUT

**/
EFI_STATUS
EFIAPI
UsbGetReportRequest (
  IN EFI_USB_IO_PROTOCOL     *UsbIo,
  IN UINT8                   Interface,
  IN UINT8                   ReportId,
  IN UINT8                   ReportType,
  IN UINT16                  ReportLen,
  IN UINT8                   *Report
  )
{
  UINT32                  Status;
  EFI_STATUS              Result;
  EFI_USB_DEVICE_REQUEST  Request;

  //
  // Fill Device request packet
  //
  Request.RequestType = 0xa1;
  //
  // 10100001b;
  //
  Request.Request = EFI_USB_GET_REPORT_REQUEST;
  Request.Value   = (UINT16) ((ReportType << 8) | ReportId);
  Request.Index   = Interface;
  Request.Length  = ReportLen;

  Result = UsbIo->UsbControlTransfer (
                    UsbIo,
                    &Request,
                    EfiUsbDataIn,
                    TIMEOUT_VALUE,
                    Report,
                    ReportLen,
                    &Status
                    );

  return Result;
}
