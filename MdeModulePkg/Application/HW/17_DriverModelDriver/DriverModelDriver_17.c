/** @file
  DriverModelDriver_17 C Source File

;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "DriverModelDriver_17.h"

EFI_DRIVER_BINDING_PROTOCOL gPciAudioDriverBinding = {
  PciAudioDriverBindingSupported,
  PciAudioDriverBindingStart,
  PciAudioDriverBindingStop,
  0xa,
  NULL,
  NULL
};

EFI_STATUS
EFIAPI
PciAudioUnload (
  IN EFI_HANDLE           ImageHandle
  )
{
  EFI_STATUS Status;
  EFI_HANDLE *HandleBuffer;
  UINTN HandleCount;
  UINTN Index;

  Status = gBS->LocateHandleBuffer (
                                ByProtocol,
                                &gEfiPciAudioProtocolGuid,
                                NULL,
                                &HandleCount,
                                &HandleBuffer
                                );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->DisconnectController (
                                    HandleBuffer[Index],
                                    ImageHandle,
                                    NULL
                                    );
  }

  FreePool (HandleBuffer);

  //
  // Uninstall protocols installed in the driver entry point
  //
  Status = gBS->UninstallMultipleProtocolInterfaces (
                      ImageHandle,
                      &gEfiDriverBindingProtocolGuid, &gPciAudioDriverBinding,
                      &gEfiComponentNameProtocolGuid, &gPciAudioComponentName,
                      &gEfiComponentName2ProtocolGuid, &gPciAudioComponentName2,
                      NULL
                      );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Do any additional cleanup that is required for this driver
  //
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
PciAudioDriverBindingEntryPoint (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS              Status;

  Status = EfiLibInstallDriverBindingComponentName2 (
             ImageHandle,
             SystemTable,
             &gPciAudioDriverBinding,
             ImageHandle,
             &gPciAudioComponentName,
             &gPciAudioComponentName2
             );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
PciAudioDriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  )
{
  EFI_STATUS                       Status;
  EFI_PCI_IO_PROTOCOL              *PciIo;
  UINT8                            ClassCode;

  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiPciIoProtocolGuid,
                  (VOID **) &PciIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Read class code's base class
  //
  Status = PciIo->Pci.Read (
                        PciIo,
                        EfiPciIoWidthUint8,
                        CLASS_CODE_OFFSET,
                        1,
                        &ClassCode
                        );
  if (ClassCode == CLASS_CODE_OF_AUDIO_DEVICE) {
    Status = EFI_SUCCESS;
  } else {
    Status = EFI_UNSUPPORTED;
  } 

  gBS->CloseProtocol (
        Controller,
        &gEfiPciIoProtocolGuid,
        This->DriverBindingHandle,
        Controller
        );

  return Status;
}

EFI_STATUS
EFIAPI
PciAudioDriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  )
{
  EFI_STATUS                       Status;
  EFI_PCI_IO_PROTOCOL              *PciIo;
  PCI_AUDIO_DEV                    *PciAudioDevice;
  
  PciIo = NULL;

  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiPciIoProtocolGuid,
                  (VOID **) &PciIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  PciAudioDevice = AllocateZeroPool (sizeof (PCI_AUDIO_DEV));
  PciAudioDevice->PciIo     = PciIo;
  PciAudioDevice->Signature = PCI_AUDIO_DEV_SIGNATURE; 

  //
  // Install dummy protocol
  //
  Status = gBS->InstallProtocolInterface (
                  &Controller,
                  &gEfiPciAudioProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &PciAudioDevice->PciAudioProtocol
                  );

  if (EFI_ERROR (Status)) {
    gBS->CloseProtocol (
                    Controller,
                    &gEfiPciIoProtocolGuid,
                    This->DriverBindingHandle,
                    Controller
                    );
    gBS->FreePool(PciAudioDevice);
  }
  
  return Status;
}

EFI_STATUS
EFIAPI
PciAudioDriverBindingStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL   *This,
  IN  EFI_HANDLE                    Controller,
  IN  UINTN                         NumberOfChildren,
  IN  EFI_HANDLE                    *ChildHandleBuffer
  )
{
  EFI_STATUS                       Status;
  // EFI_PCI_IO_PROTOCOL              *PciIo;
  PCI_AUDIO_PROTOCOL               *PciAudioProtocol;
  PCI_AUDIO_DEV                    *PciAudioDevice;

  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiPciAudioProtocolGuid,
                  (VOID **) &PciAudioProtocol,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );

  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  PciAudioDevice = PCI_AUDIO_DEV_FROM_AUDIO_PROTOCOL(PciAudioProtocol);

  // PciIo          = PciAudioDevice->PciIo;

  Status = gBS->UninstallProtocolInterface (
                  Controller,
                  &gEfiPciAudioProtocolGuid,
                  &PciAudioDevice->PciAudioProtocol
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  gBS->CloseProtocol (
         Controller,
         &gEfiPciIoProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  if (PciAudioDevice->ControllerNameTable != NULL) {
    FreeUnicodeStringTable (PciAudioDevice->ControllerNameTable);
  }

  FreePool (PciAudioDevice);

  return EFI_SUCCESS;
}