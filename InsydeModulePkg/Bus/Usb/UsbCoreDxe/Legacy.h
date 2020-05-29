/** @file
  Legacy Functions Header for USB

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

#ifndef _LEGACY_H
#define _LEGACY_H

EFI_STATUS
EFIAPI
GetLegacySupportProvider (
  IN     EFI_USB3_HC_PROTOCOL           **Usb3Hc
  );

EFI_STATUS
EFIAPI
InsertKbcKeyCode (
  IN     UINT8                          QueueSlot,
  IN     UINT8                          *KeyCode,
  IN     UINTN                          Length
  );

EFI_STATUS
EFIAPI
KbcTrapProcessor (
  IN     UINTN                          TrapType,
  IN     UINT8                          Data
  );

EFI_STATUS
EFIAPI
InsertLegacySupportProvider (
  IN     EFI_USB3_HC_PROTOCOL           *Usb3Hc
  );

EFI_STATUS
EFIAPI
RemoveLegacySupportProvider (
  IN     EFI_USB3_HC_PROTOCOL           *Usb3Hc
  );

EFI_STATUS
EFIAPI
InsertPeriodicTimer (
  IN     UINTN                          Type,
  IN     EFI_USB3_HC_CALLBACK           Callback,
  IN     VOID                           *Context,
  IN     UINTN                          MilliSecond,
  IN OUT VOID                           **Handle
  );

EFI_STATUS
EFIAPI
RemovePeriodicTimer (
  IN     VOID                           *Handle
  );

EFI_STATUS
EFIAPI
InsertPeriodicTimerProvider (
  IN     EFI_USB3_HC_PROTOCOL           *Usb3Hc
  );

EFI_STATUS
EFIAPI
RemovePeriodicTimerProvider (
  IN     EFI_USB3_HC_PROTOCOL           *Usb3Hc
  );

EFI_STATUS
EFIAPI
EnterCriticalSection (
  );

EFI_STATUS
EFIAPI
LeaveCriticalSection (
  );

EFI_STATUS
EFIAPI
ProcessDeferredProcedure (
  );

EFI_STATUS
EFIAPI
DispatchHcCallback (
  IN     EFI_USB3_HC_CALLBACK           Callback,
  IN     UINTN                          Event,
  IN     VOID                           *Context
  );

EFI_STATUS
EFIAPI
RegisterUsbBindingProtocol (
  IN     EFI_USB_BINDING_SUPPORTED      Support,
  IN     EFI_USB_BINDING_START          Start,
  IN     EFI_USB_BINDING_STOP           Stop
  );

EFI_STATUS
EFIAPI 
GetUsbDevices (
  IN     UINTN                          Type,
  IN OUT UINTN                          *Count,
  IN OUT USB_DEVICE                     ***Devices
  );

EFI_STATUS
EFIAPI
InsertUsbDevice (
  IN     USB_DEVICE                     *UsbDevice
  );
  
EFI_STATUS
EFIAPI 
RemoveUsbDevice (
  IN     EFI_USB_IO_PROTOCOL            *UsbIo
  );

EFI_STATUS
EFIAPI
ConnectUsbDevices (
  IN     EFI_USB_IO_PROTOCOL            *UsbIo,
  IN     EFI_DEVICE_PATH_PROTOCOL       *DevicePath
  );

EFI_STATUS
EFIAPI
DisconnectUsbDevices (
  IN     EFI_USB_IO_PROTOCOL            *UsbIo
  );

VOID
EFIAPI
NonSmmCallback (
  IN    EFI_EVENT                       Event,
  IN    VOID                            *Context
  );

EFI_STATUS
EFIAPI
RegisterNonSmmCallback (
  IN     EFI_USB3_HC_CALLBACK           Callback,
  IN     UINTN                          Event,
  IN     VOID                           *Context
  );

EFI_STATUS
EFIAPI
CheckDeviceDetached (
  IN     EFI_USB_IO_PROTOCOL            *UsbIo
  );

EFI_STATUS
EFIAPI 
InsertAddressConvertTable (
  IN     UINTN                          Act,
  IN     VOID                           *BaseAddress,
  IN     UINTN                          Count
  );

EFI_STATUS
EFIAPI
RemoveAddressConvertTable (
  IN     UINTN                          Act,
  IN     VOID                           *BaseAddress
  );

EFI_STATUS
EFIAPI
AddressConvert (
  IN     UINTN                          Type,
  IN     VOID                           *Address,
  OUT    VOID                           **ConvertedAddress
  );

EFI_STATUS
EFIAPI
AddressPatching (
  );

#endif
