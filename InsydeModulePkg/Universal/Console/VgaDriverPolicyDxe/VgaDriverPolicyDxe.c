/** @file
  Vga Driver Policy Driver

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "VgaDriverPolicyDxe.h"

EFI_DRIVER_BINDING_PROTOCOL gVgaDriverPolicyDriverBinding = {
  VgaDriverPolicyBindingSupported,
  VgaDriverPolicyBindingStart,
  VgaDriverPolicyBindingStop,
  0x20,
  NULL,
  NULL
};

/**
  Supported.

  @param  This                   Pointer to driver binding protocol
  @param  Controller             Controller handle to connect
  @param  RemainingDevicePath    A pointer to the remaining portion of a device
                                 path

  @retval EFI_STATUS             EFI_SUCCESS:This controller can be managed by this
                                 driver, Otherwise, this controller cannot be
                                 managed by this driver

**/
EFI_STATUS
EFIAPI
VgaDriverPolicyBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL          *This,
  IN EFI_HANDLE                           Controller,
  IN EFI_DEVICE_PATH_PROTOCOL             *RemainingDevicePath
  )
{
  EFI_PCI_IO_PROTOCOL                     *PciIo;
  PCI_TYPE00                              Pci;
  EFI_STATUS                              Status;

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
  // See if this is a PCI Graphics Controller by looking at the Command register and
  // Class Code Register
  //
  Status = PciIo->Pci.Read (
                        PciIo,
                        EfiPciIoWidthUint32,
                        0,
                        sizeof (Pci) / sizeof (UINT32),
                        &Pci
                        );
  if (EFI_ERROR (Status)) {
    Status = EFI_UNSUPPORTED;
    goto Done;
  }

  Status = EFI_UNSUPPORTED;
  if (Pci.Hdr.ClassCode[2] == 0x03 || (Pci.Hdr.ClassCode[2] == 0x00 && Pci.Hdr.ClassCode[1] == 0x01)) {
    Status = EFI_SUCCESS;
  }

Done:
  gBS->CloseProtocol (
         Controller,
         &gEfiPciIoProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  return Status;
}

/**
  Install Graphics Output Protocol onto VGA device handles.

  @param  This                   Pointer to driver binding protocol
  @param  Controller             Controller handle to connect
  @param  RemainingDevicePath    A pointer to the remaining portion of a device
                                 path

  @return EFI_STATUS

**/
EFI_STATUS
EFIAPI
VgaDriverPolicyBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL          *This,
  IN EFI_HANDLE                           Controller,
  IN EFI_DEVICE_PATH_PROTOCOL             *RemainingDevicePath
  )
{
  EFI_STATUS                              Status;
  KERNEL_CONFIGURATION                    SystemConfiguration;
  EFI_PCI_IO_PROTOCOL                     *PciIo;
  EFI_DEVICE_PATH_PROTOCOL                *ControllerDevicePath;
  EFI_DEVICE_PATH_PROTOCOL                *ActiveVgaDevicePath;

  Status = GetKernelConfiguration (&SystemConfiguration);

  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (SystemConfiguration.UefiDualVgaControllers == DUAL_VGA_CONTROLLER_ENABLE) {
    return EFI_UNSUPPORTED;
  }

  ActiveVgaDevicePath = VgaDriverPolicyGetVariable (ACTIVE_VGA_VAR_NAME);
  if (ActiveVgaDevicePath == NULL) {
    return EFI_UNSUPPORTED;
  }

  //
  // Check Controller by ACTIVE_VGA_VAR_NAME variable.
  //
  Status = gBS->HandleProtocol (
                  Controller,
                  &gEfiDevicePathProtocolGuid,
                  (VOID **) &ControllerDevicePath
                  );
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  if (VgaDriverPolicyMatchDevicePaths (ActiveVgaDevicePath, ControllerDevicePath)) {
    return EFI_UNSUPPORTED;
  } else {
    //
    // Lock the Vga controller by driver.
    //
    Status = gBS->OpenProtocol (
                    Controller,
                    &gEfiPciIoProtocolGuid,
                    (VOID **) &PciIo,
                    This->DriverBindingHandle,
                    Controller,
                    EFI_OPEN_PROTOCOL_BY_DRIVER
                    );
  }

  return Status;
}

/**
  Stop.

  @param  This                   Pointer to driver binding protocol
  @param  Controller             Controller handle to connect
  @param  NumberOfChildren       Number of children handle created by this driver
  @param  ChildHandleBuffer      Buffer containing child handle created

  @retval EFI_SUCCESS            Driver disconnected successfully from controller
  @retval EFI_UNSUPPORTED        Driver cannot be disconnected

**/
EFI_STATUS
EFIAPI
VgaDriverPolicyBindingStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL         *This,
  IN  EFI_HANDLE                          Controller,
  IN  UINTN                               NumberOfChildren,
  IN  EFI_HANDLE                          *ChildHandleBuffer
  )
{
  EFI_STATUS                              Status;
  EFI_PCI_IO_PROTOCOL                     *PciIo;

  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiPciIoProtocolGuid,
                  (VOID **) &PciIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  gBS->CloseProtocol (
         Controller,
         &gEfiPciIoProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  return EFI_SUCCESS;
}

/**

  Read the EFI variable (Name) and return a dynamically allocated
  buffer, and the size of the buffer. On failure return NULL.

  @param  Name                   String part of EFI variable name

  @return A pointer to the dynamically allocated memory that contains
          a copy of the EFI variable. Caller is repsoncible freeing
          the buffer.
          A null pointer if a variable is not found

**/
VOID *
VgaDriverPolicyGetVariable (
  IN  CHAR16                              *Name
  )
{
  return CommonGetVariableData (Name, &gEfiGenericVariableGuid);
}

/**

  Function compares a device path data structure to that of all the nodes of a
  second device path instance.

  @param  Multi                  A pointer to a multi-instance device path
                                 data structure.
  @param  Single                 A pointer to a single-instance device path
                                 data structure

  @return TRUE                   If the Single is contained within Multi
          FALSE                  The Single is not match within Multi

**/

BOOLEAN
VgaDriverPolicyMatchDevicePaths (
  IN  EFI_DEVICE_PATH_PROTOCOL            *Multi,
  IN  EFI_DEVICE_PATH_PROTOCOL            *Single
  )
{
  EFI_DEVICE_PATH_PROTOCOL                *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL                *DevicePathInst;
  UINTN                                   Size;

  if (!Multi || !Single) {
    return FALSE;
  }

  DevicePath      = Multi;
  DevicePathInst  = GetNextDevicePathInstance (&DevicePath, &Size);
  Size -= sizeof (EFI_DEVICE_PATH_PROTOCOL);

  //
  // Search for the match of 'Single' in 'Multi'
  //
  while (DevicePathInst != NULL) {
    //
    // If the single device path is found in multiple device paths,
    // return success
    //
    if (Size == 0) {
      return FALSE;
    }

    if (CompareMem (Single, DevicePathInst, Size) == 0) {
      return TRUE;
    }

    gBS->FreePool (DevicePathInst);
    DevicePathInst = GetNextDevicePathInstance (&DevicePath, &Size);
    Size -= sizeof (EFI_DEVICE_PATH_PROTOCOL);
  }

  return FALSE;
}

/**
  The user Entry Point for module UefiBiosVideo. The user code starts with this function.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
VgaDriverPolicyEntryPoint (
  IN EFI_HANDLE                            ImageHandle,
  IN EFI_SYSTEM_TABLE                      *SystemTable
  )
{
  EFI_STATUS                               Status;

  //
  // Install driver model protocol(s).
  //
  Status = EfiLibInstallDriverBindingComponentName2 (
             ImageHandle,
             SystemTable,
             &gVgaDriverPolicyDriverBinding,
             ImageHandle,
             &gVgaDriverPolicyComponentName,
             &gVgaDriverPolicyComponentName2
             );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

