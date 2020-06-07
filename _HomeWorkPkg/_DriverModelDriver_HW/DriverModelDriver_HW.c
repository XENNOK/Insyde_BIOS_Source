/** @file

  Driver Model Driver Homework
  
;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corp.
;*
;******************************************************************************
*/

#include "DriverModelDriver_HW.h"

EFI_STATUS
EFIAPI 
DMDSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL   *This,
  IN EFI_HANDLE                    ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL      *RemainingDevicePath
  );

EFI_STATUS
EFIAPI
DMDStart (
  IN EFI_DRIVER_BINDING_PROTOCOL   *This,
  IN EFI_HANDLE                    ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL      *RemainingDevicePath
  );

EFI_STATUS
EFIAPI
DMDStop (
  IN EFI_DRIVER_BINDING_PROTOCOL   *This,
  IN EFI_HANDLE                    ControllerHandle,
  IN UINTN                         NumberOfChildren,
  IN EFI_HANDLE                    *ChildHandleBuffer
  );

EFI_DRIVER_BINDING_PROTOCOL  gEfiDriverBinding = {
  DMDSupported,
  DMDStart,
  DMDStop,
  0x10,
  NULL,
  NULL
};

EFI_STATUS 
ProtocolUnLoad (
  IN EFI_HANDLE       ImageHandle
)
{
  EFI_STATUS Status;
  
   Status = gBS->UninstallMultipleProtocolInterfaces (
                  ImageHandle,
                  &gEfiDriverBindingProtocolGuid,
                  &gEfiDriverBinding,
                  &gEfiComponentNameProtocolGuid,
                  &gDMDComponentName,
                  &gEfiComponentName2ProtocolGuid,
                  &gDMDComponentName2
                  );

  if (EFI_ERROR (Status)) {
     return Status;
  }
  return Status;
}

EFI_STATUS
HydraMain (
  IN EFI_HANDLE       ImageHandle, 
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS Status;
  EFI_LOADED_IMAGE_PROTOCOL      *ImageInterface;

  // Status = gBS->HandleProtocol (
  //                 ImageHandle,
  //                 &gEfiLoadedImageProtocolGuid,
  //                 (VOID **)&ImageInterface
  //                 );
  // if (Status == EFI_SUCCESS) 
  //   ImageInterface->Unload = ProtocolUnLoad;

  Status = EfiLibInstallDriverBindingComponentName2 (
            ImageInterface->DeviceHandle,
            SystemTable,
            &gEfiDriverBinding,
            ImageHandle,
            &gDMDComponentName,
            &gDMDComponentName2
            );
}


EFI_STATUS
EFIAPI 
DMDSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL   *This,
  IN EFI_HANDLE                    ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL      *RemainingDevicePath
  )
{
  EFI_STATUS                             Status;
  EFI_PCI_IO_PROTOCOL                    *PciIo;
  PCI_TYPE00                             PciData;

  Status = gBS->OpenProtocol (
                  ControllerHandle,
                  &gEfiPciIoProtocolGuid,
                  (VOID **)&PciIo,
                  This->DriverBindingHandle,
                  ControllerHandle,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );

  if (EFI_ERROR (Status) && (Status != EFI_ALREADY_STARTED)) {
    // Print (L"DMDSupported Open gEfiPciIoProtocolGuid %r\n",Status);
    goto ERROR_EXIT;
  }

  if (Status == EFI_ALREADY_STARTED) {
    // Print (L"DMDSupported Open gEfiPciIoProtocolGuid %r\n",Status);
    goto ERROR_EXIT;
  }
  Status = PciIo->Pci.Read (
                        PciIo,
                        EfiPciIoWidthUint8,
                        0,
                        sizeof (PciData.Hdr),
                        &(PciData.Hdr)
                        );
  if (EFI_ERROR (Status)) {
    goto ERROR_EXIT;
    // Print (L"DMDSupported ERROR PciIo %r\n",Status);
  } else if ((PciData.Hdr.ClassCode[2] == PCI_CLASS_MEDIA) && 
             (PciData.Hdr.ClassCode[1] == AUDIO_DEVICE) ) {
    // Print (L"DMDSupported Audio PciIo %r\n",Status);
    goto ERROR_EXIT;
  }

ERROR_EXIT:
  gBS->CloseProtocol (
           ControllerHandle,
           &gEfiPciIoProtocolGuid,
           This->DriverBindingHandle,
           ControllerHandle
           );

return Status;  
}


EFI_STATUS
EFIAPI
 DMDStart (
  IN EFI_DRIVER_BINDING_PROTOCOL       *This,
  IN EFI_HANDLE                        ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL          *RemainingDevicePath
  )
{
  EFI_STATUS                 Status;
  EFI_PCI_IO_PROTOCOL        *PciIo;

  Status = gBS->OpenProtocol (
                  ControllerHandle,
                  &gEfiPciIoProtocolGuid,
                  (VOID **)&PciIo,
                  This->DriverBindingHandle,
                  ControllerHandle,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );

  if (EFI_ERROR (Status) && (Status != EFI_ALREADY_STARTED)) {
    // Print (L"DMDStart gEfiPciIoProtocolGuid %r\n",Status);
    // return EFI_UNSUPPORTED; 
    
  }

  if (Status == EFI_ALREADY_STARTED) {
    // Print (L"DMDStart gEfiPciIoProtocolGuid %r\n",Status);
    // return EFI_ALREADY_STARTED;
    goto ERROR_EXIT;
  }

  ERROR_EXIT:
    gBS->CloseProtocol (
           ControllerHandle,
           &gEfiPciIoProtocolGuid,
           This->DriverBindingHandle,
           ControllerHandle
           );

  return Status;  
}

EFI_STATUS
EFIAPI
DMDStop (
  IN EFI_DRIVER_BINDING_PROTOCOL   *This,
  IN EFI_HANDLE                    ControllerHandle,
  IN UINTN                         NumberOfChildren,
  IN EFI_HANDLE                    *ChildHandleBuffer
  )
{
  EFI_STATUS                             Status;

  Status = gBS->CloseProtocol (
           ControllerHandle,
           &gEfiPciIoProtocolGuid,
           This->DriverBindingHandle,
           ControllerHandle
           );
  // Print (L"DMDStop gEf4iPciIoProtocolGuid %r\n",Status);
  return Status;
}