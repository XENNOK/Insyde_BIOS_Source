#include "_test.h"

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
                                AllHandles,
                                NULL,
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
                                    gImageHandle,
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
    Print(L"%r\n");
    return Status;
  }

  //
  // Read class code's base class
  //
  Status = PciIo->Pci.Read (
                        PciIo,
                        EfiPciIoWidthUint8,
                        0xb,
                        1,
                        &ClassCode
                        );
  if (ClassCode == 0x4) {
    Status = EFI_SUCCESS;
    Print(L"%r\n", Status);
  } else {
    Status = EFI_UNSUPPORTED;
    Print(L"%r\n", Status);
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
  PciAudioDevice = AllocateZeroPool (sizeof (PCI_AUDIO_DEV));

  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiPciIoProtocolGuid,
                  (VOID **) &PciIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) {
    Print(L"%r\n", Status);
    return Status;
  }

  PciAudioDevice->PciIo     = PciIo;
  PciAudioDevice->Signature = PCI_AUDIO_DEV_SIGNATURE;


  // // test
  // PciData8 = 0;
  // Offset = 0xb;
  // for (Bus = 0; Bus <= PCI_MAX_BUS; Bus++) {
  //   for (Device = 0; Device <= PCI_MAX_DEVICE; Device++) {
  //     for (Func = 0; Func <= PCI_MAX_FUNC; Func++) {
  //       PciAddress = EFI_PCI_ADDRESS (Bus, Device, Func, Offset);
  //       Status = PciRootBridgeIo->Pci.Read (
  //                                       PciRootBridgeIo,
  //                                       EfiPciWidthUint8,
  //                                       PciAddress,
  //                                       1,
  //                                       &PciData8
  //                                       );
  //       if (PciData8 == 0x04) {
  //         Status = EFI_SUCCESS;
  //         Print(L"%r\n", Status);
  //         Status = gBS->InstallProtocolInterface (
  //                 &Controller,
  //                 &gEfiPciAudioProtocolGuid,
  //                 EFI_NATIVE_INTERFACE,
  //                 &PciAudioDevice->PciAudioProtocol
  //                 );
  //       } else {
  //         Status = EFI_UNSUPPORTED;
  //         Print(L"%r\n", Status);
  //       }
  //     }
  //   }
  // }  

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
           &gEfiPciAudioProtocolGuid,
           This->DriverBindingHandle,
           Controller
           );
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
  EFI_PCI_IO_PROTOCOL              *PciIo;
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

  PciIo          = PciAudioDevice->PciIo;

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