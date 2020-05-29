/** @file

connect to IsaBus

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "IsaAcpiDriver.h"

ISA_BRIDGE_INFO            mIsaBridgeInfo;

//
//  Lpc Driver Global Variables
//
static EFI_DRIVER_BINDING_PROTOCOL mIsaAcpiDriver = {
  IsaAcpiDriverSupported,
  IsaAcpiDriverStart,
  IsaAcpiDriverStop,
  1,
  NULL,
  NULL
};

/**
  The entry point of the Lpc driver.  

  @param[in]  ImageHandle       Pointer to the loaded image protocol for this driver.
  @param[in]  SystemTable       Pointer to the EFI System Table.

  @retval     EFI_SUCCESS       Function complete successfully.  
**/
EFI_STATUS
EFIAPI
IsaAcpiDriverEntryPoint (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS                      Status;

  Status = EfiLibInstallDriverBinding (ImageHandle, SystemTable, &mIsaAcpiDriver, ImageHandle);
  
  if (EFI_ERROR(Status)) {
    return Status;
  }

  //
  // To get function and device through library
  //
  DxeCsSvcSetIsaInfo (&(mIsaBridgeInfo.Func), &(mIsaBridgeInfo.Dev));

  return Status;
}

/**
  Test to see if this driver supports ControllerHandle. 

  @param[in]  This                  Protocol instance pointer.
  @param[in]  Controller            Handle of device to test.
  @param[in]  RemainingDevicePath   Optional parameter use to pick a specific child device to start.

  @retval EFI_SUCCESS               Success.
  @retval EFI_UNSUPPORTED           This driver does not support this device. 
**/
EFI_STATUS
EFIAPI
IsaAcpiDriverSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  )
{
  EFI_STATUS                Status;
  EFI_PCI_IO_PROTOCOL       *PciIo;
  EFI_DEVICE_PATH_PROTOCOL  *IsaBridgeDevicePath;
  ACPI_HID_DEVICE_PATH      *AcpiNode;
  PCI_DEVICE_PATH           *PciNode;
  PCI_TYPE00                Pci;
  UINTN                     SegmentNumber;
  UINTN                     BusNumber;
  UINTN                     DeviceNumber;
  UINTN                     FunctionNumber;

  //
  // Get the ISA bridge's Device Path and test it
  // the following code is specific
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiDevicePathProtocolGuid,
                  (VOID **) &IsaBridgeDevicePath,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = EFI_SUCCESS;
  AcpiNode =  (ACPI_HID_DEVICE_PATH *) IsaBridgeDevicePath;
  if (AcpiNode->Header.Type != ACPI_DEVICE_PATH ||
      AcpiNode->Header.SubType != ACPI_DP ||
      DevicePathNodeLength (&AcpiNode->Header) != sizeof (ACPI_HID_DEVICE_PATH) ||
      AcpiNode->HID != EISA_PNP_ID(0x0A03) ||
      AcpiNode->UID != 0 ) {
      
    Status = EFI_UNSUPPORTED;
  } else {
    //
    // Get the next node
    //
    IsaBridgeDevicePath = NextDevicePathNode (IsaBridgeDevicePath);
    PciNode  = (PCI_DEVICE_PATH *)IsaBridgeDevicePath;
    if (PciNode->Header.Type != HARDWARE_DEVICE_PATH ||
        PciNode->Header.SubType != HW_PCI_DP ||
        DevicePathNodeLength (&PciNode->Header) != sizeof (PCI_DEVICE_PATH) ||
        PciNode->Function != mIsaBridgeInfo.Func ||
        PciNode->Device != mIsaBridgeInfo.Dev ) {
      Status = EFI_UNSUPPORTED;
    }
  }

  gBS->CloseProtocol (
         Controller,
         &gEfiDevicePathProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  //
  // Get PciIo protocol instance
  //
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

  Status = PciIo->Pci.Read (
                    PciIo,
                    EfiPciIoWidthUint32,
                    0,
                    sizeof(Pci) / sizeof(UINT32),
                    &Pci
                    );

  if (!EFI_ERROR (Status)) {
    Status = EFI_UNSUPPORTED;

    if ((Pci.Hdr.Command & 0x03) == 0x03) {
      if (Pci.Hdr.ClassCode[2] == PCI_CLASS_BRIDGE) {
        //
        // See if this is a standard PCI to ISA Bridge from the Base Code and Class Code
        //
        if (Pci.Hdr.ClassCode[1] == PCI_CLASS_ISA) {
          Status = EFI_SUCCESS;
        }

        //
        // See if this is an Intel PCI to ISA bridge in Positive Decode Mode (only for Intel specific ISA bridge)
        //
        if ((Pci.Hdr.ClassCode[1] == PCI_CLASS_ISA_POSITIVE_DECODE) && (Pci.Hdr.VendorId == 0x8086)) {
          //
          // See if this is on Function #0 to avoid false positives on
          // PCI_CLASS_BRIDGE_OTHER that has the same value as
          // PCI_CLASS_BRIDGE_ISA_PDECODE
          //
          Status = PciIo->GetLocation (
                            PciIo,
                            &SegmentNumber,
                            &BusNumber,
                            &DeviceNumber,
                            &FunctionNumber
                            );
          if (!EFI_ERROR (Status) && FunctionNumber == 0) {
            Status = EFI_SUCCESS;
          } else {
            Status = EFI_UNSUPPORTED;
          }
        }
      }
    }
  }

  gBS->CloseProtocol (
         Controller,
         &gEfiPciIoProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  return Status;
}

/**
  Start this driver on ControllerHandle. 

  @param[in]  This                  Protocol instance pointer.
  @param[in]  Controller            Handle of device to test.
  @param[in]  RemainingDevicePath   Optional parameter use to pick a specific child device to start.

  @retval     EFI_SUCCESS           Success.
  @retval     EFI_UNSUPPORTED       This driver does not support this device. 
**/
EFI_STATUS
EFIAPI
IsaAcpiDriverStart (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  )
{
  EFI_STATUS                            Status;
  EFI_PCI_IO_PROTOCOL                   *PciIo;
  LPC_DEV                               *LpcDev;
  EFI_ISA_ACPI_PROTOCOL                 *IsaAcpi = NULL;  

  PostCode (BDS_START_ISA_ACPI_CONTROLLER); //PostCode = 0x20, SIO controller initial

  LpcDev = NULL;

  //
  // Get Pci IO
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiPciIoProtocolGuid,
                  (VOID **)&PciIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );

  if (EFI_ERROR (Status) && Status != EFI_ALREADY_STARTED) {
    return Status;
  }

  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  sizeof(LPC_DEV),
                  (VOID **) &LpcDev
                  );

  if (EFI_ERROR (Status)) {
    //
    // Close PciIo protocol
    //
    gBS->CloseProtocol (
           Controller,
           &gEfiPciIoProtocolGuid,
           This->DriverBindingHandle,
           Controller
           );
    return EFI_OUT_OF_RESOURCES;
  }

  ZeroMem (LpcDev, sizeof (LPC_DEV));

  LpcDev->Signature = LPC_DEV_SIGNATURE;
  LpcDev->Handle    = Controller;
  LpcDev->PciIo     = PciIo;

  LpcInterfaceInit (IsaAcpi);

  //
  // IsaAcpi interface
  //
  (LpcDev -> IsaAcpi).DeviceEnumerate = IsaDeviceEnumerate;
  (LpcDev -> IsaAcpi).SetPower        = IsaDeviceSetPower;
  (LpcDev -> IsaAcpi).GetCurResource  = IsaGetCurrentResource;
  (LpcDev -> IsaAcpi).GetPosResource  = IsaGetPossibleResource;
  (LpcDev -> IsaAcpi).SetResource     = IsaSetResource;
  (LpcDev -> IsaAcpi).EnableDevice    = IsaEnableDevice;
  (LpcDev -> IsaAcpi).InitDevice      = IsaInitDevice;
  (LpcDev -> IsaAcpi).InterfaceInit   = LpcInterfaceInit;

  //
  // Install IsaAcpi interface, the Sio interface is not installed!
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Controller,
                  &gEfiIsaAcpiProtocolGuid,
                  &LpcDev->IsaAcpi,
                  NULL
                  );
                  
  if (EFI_ERROR (Status)) {
    gBS->CloseProtocol (
           Controller,
           &gEfiPciIoProtocolGuid,
           This->DriverBindingHandle,
           Controller
           );
    gBS->FreePool (
           LpcDev
           );
    return Status;
  }

  return EFI_SUCCESS;
}

/**
  Start this driver on ControllerHandle. 

  @param[in]  This                  Stop this driver on ControllerHandle.
  @param[in]  Controller            Handle of device to test.
  @param[in]  RemainingDevicePath   Optional parameter use to pick a specific child device to start.

  @retval     EFI_SUCCESS           Success.
  @retval     EFI_UNSUPPORTED       This driver does not support this device. 
**/
EFI_STATUS
EFIAPI
IsaAcpiDriverStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN  EFI_HANDLE                     Controller,
  IN  UINTN                          NumberOfChildren,
  IN  EFI_HANDLE                     *ChildHandleBuffer
  )
{
  EFI_STATUS             Status;
  EFI_ISA_ACPI_PROTOCOL  *IsaAcpi;
  LPC_DEV                *LpcDev;

  //
  // Get EFI_ISA_ACPI_PROTOCOL interface
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiIsaAcpiProtocolGuid,
                  (VOID **) &IsaAcpi,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  LpcDev = LPC_ISA_ACPI_FROM_THIS (IsaAcpi);

  //
  // Uninstall protocol interface: EFI_ISA_ACPI_PROTOCOL
  //
  Status = gBS->UninstallProtocolInterface (
                  Controller,
                  &gEfiIsaAcpiProtocolGuid,
                  &LpcDev->IsaAcpi
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

  gBS->FreePool (
         LpcDev
         );

  return EFI_SUCCESS;
}

