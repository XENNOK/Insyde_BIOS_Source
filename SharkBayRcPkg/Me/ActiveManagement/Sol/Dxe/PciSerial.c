/** @file

  PCI Serial driver for standard UARTS on an PCI bus.
  Customized for Intel AMT SErial OVer LAN (82573E-Tekoa) 16550 UART Driver.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "PciSerial.h"
#include <MeAccess.h>
///
/// PCI Serial Driver Global Variables
///
EFI_DRIVER_BINDING_PROTOCOL mPciSerialControllerDriverBinding = {
  PciSerialControllerDriverSupported,
  PciSerialControllerDriverStart,
  PciSerialControllerDriverStop,
  0x10,
  NULL,
  NULL
};

/**
  This function checks to see if the driver supports a device specified by
  "Controller handle" parameter. It is called by DXE Core StartImage() or
  ConnectController() routines. The driver uses 'device path' and/or
  'services' from the Bus I/O abstraction attached to the controller handle
  to determine if the driver support this controller handle.

  Note: In the BDS (Boot Device Selection) phase, the DXE core enumerate all
  devices (or, controller) and assigns GUIDs to them.

  @param[in] This                 a pointer points to the Binding Protocol instance
  @param[in] Controller           The handle of controller to be tested.
  @param[in] RemainingDevicePath  A pointer to the device path. Ignored by device
                                  driver but used by bus driver

  @retval EFI_SUCCESS             Have device to support
  @retval EFI_NOT_FOUND           The device doesn't support or relative environment not ready
**/
EFI_STATUS
EFIAPI
PciSerialControllerDriverSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  )
{
  EFI_STATUS                Status;
  EFI_DEVICE_PATH_PROTOCOL  *ParentDevicePath;
  EFI_PCI_IO_PROTOCOL       *PciIo;
  UART_DEVICE_PATH          UartNode;
  UINT16                    Buffer[2];
  UINT16                    Temp;
  UINT8                     *ByteBuffer;

  ///
  /// Init AMT library
  ///
  Status = AmtLibInit ();

  if (EFI_ERROR (Status)) {
    return Status;
  }
  ///
  /// We don't want the SOL Controller enabled unless
  /// there is an remote control request. The AMT usage model
  /// dictates this. Thus here we check for the ASF Remote
  /// Control command wants SOL before we start the controller.
  ///
  if (ActiveManagementEnableSol () == FALSE) {
    return EFI_NOT_FOUND;
  }
  ///
  /// Open the IO Abstraction(s) needed to perform the supported test
  ///
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiDevicePathProtocolGuid,
                  (VOID **) &ParentDevicePath,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (Status == EFI_ALREADY_STARTED) {
    return EFI_SUCCESS;
  }

  if (EFI_ERROR (Status)) {
    return Status;
  }

  gBS->CloseProtocol (
        Controller,
        &gEfiDevicePathProtocolGuid,
        This->DriverBindingHandle,
        Controller
        );

  ///
  /// Now test the EfiPciIoProtocol
  ///
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiPciIoProtocolGuid,
                  (VOID **) &PciIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );

  if (Status == EFI_ALREADY_STARTED) {
    return EFI_SUCCESS;
  }

  if (EFI_ERROR (Status)) {
    return Status;
  }
  ///
  /// Use the PCI I/O Protocol to see if Controller is standard ISA UART that
  /// can be managed by this driver.
  ///
  Status = EFI_SUCCESS;

  ///
  /// Looks for a PCI CLASS / SUBCLASS / INTERFACE of 0x07 / 0x00 / 0x02
  /// To allow supportting all PCI Devices that are 16550 compatible UARTS.
  ///
  ///  This is point where Tekoa iAMT SOL support enabling via
  ///  Get Boot Options ASF info is used to customize PCISerial
  ///  to work for iAMT.
  ///
  ///  Also if want general PCI Serial com devices to work as well
  ///  can duplicate this driver one for tekoa and iAMT and the other
  ///  for general serial devices.
  ///
  Status = PciIo->Pci.Read (
                        PciIo,
                        EfiPciIoWidthUint32,
                        (UINT32) PCI_VENDOR_ID_OFFSET,
                        (UINTN) 1,
                        (VOID *) &Buffer
                        );
  ///
  /// If Buffer is Not Valid (no setup info in NVRAM/FLASH) use auto mode for PTBx cfg.
  ///
  if ((Buffer[0] != V_ME_SOL_VENDOR_ID) || !IS_PCH_LPT_SOL_DEVICE_ID(Buffer[1])
  ) {
    Status = EFI_UNSUPPORTED;
    goto Error;
  }

  Status = PciIo->Pci.Read (
                        PciIo,
                        EfiPciIoWidthUint32,
                        (UINT32) PCI_REVISION_ID_OFFSET,
                        (UINTN) 1,
                        (VOID *) &Buffer
                        );
  ByteBuffer = (UINT8 *) Buffer;
  if ((ByteBuffer[3] != PCI_CLASS_SCC) || (ByteBuffer[2] != PCI_SUBCLASS_SERIAL) || (ByteBuffer[1] != PCI_IF_16550)) {
    Status = EFI_UNSUPPORTED;
    goto Error;
  }
  ///
  /// Make sure the PCI io space is enabled
  ///
  Temp = 0x0003;
  PciIo->Pci.Write (
              PciIo,
              EfiPciIoWidthUint16,
              PCI_COMMAND_OFFSET,
              0x01,
              (VOID*) &Temp
              );

  ///
  /// Make sure RemainingDevicePath is valid
  ///
  if (RemainingDevicePath != NULL) {
    Status = EFI_UNSUPPORTED;
    CopyMem (&UartNode, (UART_DEVICE_PATH *) RemainingDevicePath, sizeof (UART_DEVICE_PATH));
    if (UartNode.Header.Type != MESSAGING_DEVICE_PATH ||
        UartNode.Header.SubType != MSG_UART_DP ||
        DevicePathNodeLength ((EFI_DEVICE_PATH_PROTOCOL *) &UartNode) != sizeof (UART_DEVICE_PATH)
        ) {
      goto Error;
    }

    if (UartNode.BaudRate > SERIAL_PORT_MAX_BAUD_RATE) {
      goto Error;
    }

    if (UartNode.Parity < NoParity || UartNode.Parity > SpaceParity) {
      goto Error;
    }

    if (UartNode.DataBits < 5 || UartNode.DataBits > 8) {
      goto Error;
    }

    if (UartNode.StopBits < OneStopBit || UartNode.StopBits > TwoStopBits) {
      goto Error;
    }

    if ((UartNode.DataBits == 5) && (UartNode.StopBits == TwoStopBits)) {
      goto Error;
    }

    if ((UartNode.DataBits >= 6) && (UartNode.DataBits <= 8) && (UartNode.StopBits == OneFiveStopBits)) {
      goto Error;
    }

    Status = EFI_SUCCESS;
  }

Error:
  ///
  /// Close the I/O Abstraction(s) used to perform the supported test
  ///
  gBS->CloseProtocol (
        Controller,
        &gEfiPciIoProtocolGuid,
        This->DriverBindingHandle,
        Controller
        );
  return Status;
}

/**
  This routine is called right after the .Supported() called
  and return EFI_SUCCESS. Notes: The supported protocols are
  checked but the Protocols are closed.

  @param[in] This                 A pointer points to the Binding Protocol instance
  @param[in] Controller           The handle of controller to be tested. Parameter
                                  passed by the caller
  @param[in] RemainingDevicePath  A pointer to the device path. Should be ignored by
                                  device driver

  @retval EFI_SUCCESS             The driver ready and initial complete.
  @retval Other                   The device doesn't initial.
**/
EFI_STATUS
EFIAPI
PciSerialControllerDriverStart (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  )
{
  EFI_STATUS                          Status;
  EFI_PCI_IO_PROTOCOL                 *PciIo;
  SERIAL_DEV                          *SerialDevice;
  UINTN                               Index;
  UART_DEVICE_PATH                    Node;
  EFI_DEVICE_PATH_PROTOCOL            *ParentDevicePath;
  CHAR16                              SerialPortName[sizeof (PCI_SERIAL_PORT_NAME)];
  EFI_OPEN_PROTOCOL_INFORMATION_ENTRY *OpenInfoBuffer;
  UINTN                               EntryCount;
  EFI_SERIAL_IO_PROTOCOL              *SerialIo;
  UINT64                              *Supports;
  UINT64                              Temp;
  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR   *Ptr;
  VOID                                **Resources;
  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR   Temp1;

  SerialDevice = NULL;

  ///
  /// Get the Parent Device Path
  ///
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiDevicePathProtocolGuid,
                  (VOID **) &ParentDevicePath,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status) && Status != EFI_ALREADY_STARTED) {
    return Status;
  }
  ///
  /// Grab the IO abstraction we need to get any work done
  ///
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiPciIoProtocolGuid,
                  (VOID **) &PciIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status) && Status != EFI_ALREADY_STARTED) {
    goto Error;
  }

  if (Status == EFI_ALREADY_STARTED) {

    if (RemainingDevicePath == NULL) {
      return EFI_SUCCESS;
    }
    ///
    /// Make sure a child handle does not already exist.  This driver can only
    /// produce one child per serial port.
    ///
    Status = gBS->OpenProtocolInformation (
                    Controller,
                    &gEfiPciIoProtocolGuid,
                    &OpenInfoBuffer,
                    &EntryCount
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    Status = EFI_ALREADY_STARTED;
    for (Index = 0; Index < EntryCount; Index++) {
      if (OpenInfoBuffer[Index].Attributes & EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER) {
        Status = gBS->OpenProtocol (
                        OpenInfoBuffer[Index].ControllerHandle,
                        &gEfiSerialIoProtocolGuid,
                        (VOID **) &SerialIo,
                        This->DriverBindingHandle,
                        Controller,
                        EFI_OPEN_PROTOCOL_GET_PROTOCOL
                        );
        if (!EFI_ERROR (Status)) {
          CopyMem (&Node, RemainingDevicePath, sizeof (UART_DEVICE_PATH));
          Status = SerialIo->SetAttributes (
                              SerialIo,
                              Node.BaudRate,
                              SerialIo->Mode->ReceiveFifoDepth,
                              SerialIo->Mode->Timeout,
                              Node.Parity,
                              Node.DataBits,
                              Node.StopBits
                              );
        }
        break;
      }
    }

    FreePool (OpenInfoBuffer);
    return Status;
  }
  ///
  /// Initialize the serial device instance
  ///
  SerialDevice = AllocatePool (sizeof (SERIAL_DEV));
  if (SerialDevice == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Error;
  }

  ZeroMem (SerialDevice, sizeof (SERIAL_DEV));

  SerialDevice->PciIo               = PciIo;
  SerialDevice->ParentDevicePath    = ParentDevicePath;
  SerialDevice->ControllerNameTable = NULL;

  StrCpy (SerialPortName, L"PCI Serial Port # ");
  SerialPortName[sizeof (PCI_SERIAL_PORT_NAME) - 2] = (CHAR16) (L'0');
  AddUnicodeString (
    "eng",
    mPciSerialComponentName.SupportedLanguages,
    &SerialDevice->ControllerNameTable,
    (CHAR16 *) SerialPortName
    );

  Ptr       = &Temp1;
  Resources = (VOID **) &Ptr;
  Supports  = &Temp;
  *Supports = 0x01;
  for (Index = 0; Index < PCI_MAX_BAR; Index++) {
    Status = SerialDevice->PciIo->GetBarAttributes (
                                    PciIo,
                                    (UINT8) Index,
                                    Supports,
                                    Resources
                                    );
    Ptr = *Resources;
    if (Ptr->ResType == ACPI_ADDRESS_SPACE_TYPE_IO) {
      SerialDevice->BarIndex  = (UINT16) Index;
      Status                  = EFI_SUCCESS;
      break;
    }
  }

  if (PciSerialPortPresent (SerialDevice) != TRUE) {
    Status = EFI_DEVICE_ERROR;
    goto Error;
  }

  SerialDevice->Signature               = SERIAL_DEV_SIGNATURE;
  SerialDevice->Type                    = UART16450;
  SerialDevice->SoftwareLoopbackEnable  = FALSE;
  SerialDevice->HardwareFlowControl     = FALSE;
  SerialDevice->Handle                  = NULL;
  SerialDevice->Receive.First           = 0;
  SerialDevice->Receive.Last            = 0;
  SerialDevice->Receive.Surplus         = SERIAL_MAX_BUFFER_SIZE;
  SerialDevice->Transmit.First          = 0;
  SerialDevice->Transmit.Last           = 0;
  SerialDevice->Transmit.Surplus        = SERIAL_MAX_BUFFER_SIZE;

  ///
  /// Serial I/O
  ///
  SerialDevice->SerialIo.Revision       = EFI_SERIAL_IO_PROTOCOL_REVISION;
  SerialDevice->SerialIo.Reset          = PciSerialReset;
  SerialDevice->SerialIo.SetAttributes  = PciSerialSetAttributes;
  SerialDevice->SerialIo.SetControl     = PciSerialSetControl;
  SerialDevice->SerialIo.GetControl     = PciSerialGetControl;
  SerialDevice->SerialIo.Write          = PciSerialWrite;
  SerialDevice->SerialIo.Read           = PciSerialRead;
  SerialDevice->SerialIo.Mode           = &(SerialDevice->SerialMode);

  if (RemainingDevicePath != NULL) {
    ///
    /// Match the configuration of the RemainingDevicePath. IsHandleSupported()
    /// already checked to make sure the RemainingDevicePath contains settings
    /// that we can support.
    ///
    CopyMem (&SerialDevice->UartDevicePath, RemainingDevicePath, sizeof (UART_DEVICE_PATH));
  } else {
    ///
    /// Build the device path by appending the UART node to the ParentDevicePath
    /// from the WinNtIo handle. The Uart setings are zero here, since
    /// SetAttribute() will update them to match the default setings.
    ///
    ZeroMem (&SerialDevice->UartDevicePath, sizeof (UART_DEVICE_PATH));
    SerialDevice->UartDevicePath.Header.Type    = MESSAGING_DEVICE_PATH;
    SerialDevice->UartDevicePath.Header.SubType = MSG_UART_DP;
    SetDevicePathNodeLength ((EFI_DEVICE_PATH_PROTOCOL *) &SerialDevice->UartDevicePath, sizeof (UART_DEVICE_PATH));
  }
  ///
  /// Build the device path by appending the UART node to the ParentDevicePath
  /// from the WinNtIo handle. The Uart setings are zero here, since
  /// SetAttribute() will update them to match the current setings.
  ///
  SerialDevice->DevicePath = AppendDevicePathNode (
                              ParentDevicePath,
                              (EFI_DEVICE_PATH_PROTOCOL *) &SerialDevice->UartDevicePath
                              );

  if (SerialDevice->DevicePath == NULL) {
    Status = EFI_DEVICE_ERROR;
    goto Error;
  }
  ///
  /// Fill in Serial I/O Mode structure based on either the RemainingDevicePath or defaults.
  ///
  SerialDevice->SerialMode.ControlMask      = SERIAL_PORT_DEFAULT_CONTROL_MASK;
  SerialDevice->SerialMode.Timeout          = SERIAL_PORT_DEFAULT_TIMEOUT;
  SerialDevice->SerialMode.BaudRate         = SerialDevice->UartDevicePath.BaudRate;
  SerialDevice->SerialMode.ReceiveFifoDepth = SERIAL_PORT_DEFAULT_RECEIVE_FIFO_DEPTH;
  SerialDevice->SerialMode.DataBits         = SerialDevice->UartDevicePath.DataBits;
  SerialDevice->SerialMode.Parity           = SerialDevice->UartDevicePath.Parity;
  SerialDevice->SerialMode.StopBits         = SerialDevice->UartDevicePath.StopBits;

  ///
  /// Issue a reset to initialize the COM port
  ///
  Status = SerialDevice->SerialIo.Reset (&SerialDevice->SerialIo);
  if (EFI_ERROR (Status)) {
    Status = EFI_SUCCESS;
    goto Error;
  }
  ///
  /// Install protocol interfaces for the serial device.
  ///
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &SerialDevice->Handle,
                  &gEfiDevicePathProtocolGuid,
                  SerialDevice->DevicePath,
                  &gEfiSerialIoProtocolGuid,
                  &SerialDevice->SerialIo,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    goto Error;
  }
  ///
  /// Open For Child Device
  ///
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiPciIoProtocolGuid,
                  (VOID **) &PciIo,
                  This->DriverBindingHandle,
                  SerialDevice->Handle,
                  EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER
                  );

Error:
  if (EFI_ERROR (Status)) {
    gBS->CloseProtocol (
          Controller,
          &gEfiDevicePathProtocolGuid,
          This->DriverBindingHandle,
          Controller
          );
    gBS->CloseProtocol (
          Controller,
          &gEfiPciIoProtocolGuid,
          This->DriverBindingHandle,
          Controller
          );
    if (SerialDevice) {
      if (SerialDevice->DevicePath) {
        FreePool (SerialDevice->DevicePath);
      }

      FreeUnicodeStringTable (SerialDevice->ControllerNameTable);
      FreePool (SerialDevice);
    }
  }

  return Status;
}

/**
  Stop.

  @param[in] This                 Pointer to driver binding protocol
  @param[in] Controller           Controller handle to connect
  @param[in] NumberOfChildren     Number of children handle created by this driver
  @param[in] ChildHandleBuffer    Buffer containing child handle created

  @retval EFI_SUCCESS             Driver disconnected successfully from controller
  @retval EFI_DEVICE_ERROR        Cannot find BIOS_VIDEO_DEV structure
**/
EFI_STATUS
EFIAPI
PciSerialControllerDriverStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN  EFI_HANDLE                     Controller,
  IN  UINTN                          NumberOfChildren,
  IN  EFI_HANDLE                     *ChildHandleBuffer
  )
{
  EFI_STATUS                Status;
  UINTN                     Index;
  BOOLEAN                   AllChildrenStopped;
  EFI_SERIAL_IO_PROTOCOL    *SerialIo;
  SERIAL_DEV                *SerialDevice;
  EFI_PCI_IO_PROTOCOL       *PciIo;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;

  Status = gBS->HandleProtocol (
                  Controller,
                  &gEfiDevicePathProtocolGuid,
                  (VOID **) &DevicePath
                  );

  ///
  /// Complete all outstanding transactions to Controller.
  /// Don't allow any new transaction to Controller to be started.
  ///
  if (NumberOfChildren == 0) {
    ///
    /// Close the bus driver
    ///
    Status = gBS->CloseProtocol (
                    Controller,
                    &gEfiPciIoProtocolGuid,
                    This->DriverBindingHandle,
                    Controller
                    );

    Status = gBS->CloseProtocol (
                    Controller,
                    &gEfiDevicePathProtocolGuid,
                    This->DriverBindingHandle,
                    Controller
                    );
    return Status;
  }

  AllChildrenStopped = TRUE;

  for (Index = 0; Index < NumberOfChildren; Index++) {
    Status = gBS->OpenProtocol (
                    ChildHandleBuffer[Index],
                    &gEfiSerialIoProtocolGuid,
                    (VOID **) &SerialIo,
                    This->DriverBindingHandle,
                    Controller,
                    EFI_OPEN_PROTOCOL_GET_PROTOCOL
                    );
    if (!EFI_ERROR (Status)) {

      SerialDevice = SERIAL_DEV_FROM_THIS (SerialIo);

      Status = gBS->CloseProtocol (
                      Controller,
                      &gEfiPciIoProtocolGuid,
                      This->DriverBindingHandle,
                      ChildHandleBuffer[Index]
                      );

      Status = gBS->UninstallMultipleProtocolInterfaces (
                      ChildHandleBuffer[Index],
                      &gEfiDevicePathProtocolGuid,
                      SerialDevice->DevicePath,
                      &gEfiSerialIoProtocolGuid,
                      &SerialDevice->SerialIo,
                      NULL
                      );
      if (EFI_ERROR (Status)) {
        gBS->OpenProtocol (
              Controller,
              &gEfiPciIoProtocolGuid,
              (VOID **) &PciIo,
              This->DriverBindingHandle,
              ChildHandleBuffer[Index],
              EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER
              );
      } else {
        if (SerialDevice->DevicePath) {
          FreePool (SerialDevice->DevicePath);
        }

        FreeUnicodeStringTable (SerialDevice->ControllerNameTable);
        FreePool (SerialDevice);
      }
    }

    if (EFI_ERROR (Status)) {
      AllChildrenStopped = FALSE;
    }
  }

  if (AllChildrenStopped == FALSE) {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

/**
  Detect whether specific FIFO is full or not

  @param[in] Fifo                 A pointer to the Data Structure SERIAL_DEV_FIFO

  @retval TRUE                    The FIFO is full
  @retval FALSE                   The FIFO is not full
**/
BOOLEAN
PciSerialFifoFull (
  IN SERIAL_DEV_FIFO              *Fifo
  )
{
  if (Fifo->Surplus == 0) {
    return TRUE;
  }

  return FALSE;
}

/**
  Detect whether specific FIFO is empty or not

  @param[in] Fifo                 A pointer to the Data Structure SERIAL_DEV_FIFO

  @retval TRUE                    The FIFO is empty
  @retval FALSE                   The FIFO is not empty
**/
BOOLEAN
PciSerialFifoEmpty (
  IN SERIAL_DEV_FIFO              *Fifo
  )
{
  if (Fifo->Surplus == SERIAL_MAX_BUFFER_SIZE) {
    return TRUE;
  }

  return FALSE;
}

/**
  Add data to specific FIFO

  @param[in] Fifo                 A pointer to the Data Structure SERIAL_DEV_FIFO
  @param[in] Data                 The data added to FIFO

  @retval EFI_SUCCESS             Add data to specific FIFO successfully
  @retval EFI_OUT_OF_RESOURCES    Failed to add data because FIFO is already full
**/
EFI_STATUS
PciSerialFifoAdd (
  IN SERIAL_DEV_FIFO              *Fifo,
  IN UINT8                        Data
  )
{
  ///
  /// if FIFO full can not add data
  ///
  if (PciSerialFifoFull (Fifo)) {
    return EFI_OUT_OF_RESOURCES;
  }
  ///
  /// FIFO is not full can add data
  ///
  Fifo->Data[Fifo->Last] = Data;
  Fifo->Surplus--;
  Fifo->Last++;
  if (Fifo->Last == SERIAL_MAX_BUFFER_SIZE) {
    Fifo->Last = 0;
  }

  return EFI_SUCCESS;
}

/**
  Remove data from specific FIFO

  @param[in] Fifo                 A pointer to the Data Structure SERIAL_DEV_FIFO
  @param[in] Data                 The data removed from FIFO

  @retval EFI_SUCCESS             Remove data from specific FIFO successfully
  @retval EFI_OUT_OF_RESOURCES    Failed to remove data because FIFO is empty
**/
EFI_STATUS
PciSerialFifoRemove (
  IN  SERIAL_DEV_FIFO             *Fifo,
  OUT UINT8                       *Data
  )
{
  ///
  /// if FIFO is empty, no data can remove
  ///
  if (PciSerialFifoEmpty (Fifo)) {
    return EFI_OUT_OF_RESOURCES;
  }
  ///
  /// FIFO is not empty, can remove data
  ///
  *Data = Fifo->Data[Fifo->First];
  Fifo->Surplus++;
  Fifo->First++;
  if (Fifo->First == SERIAL_MAX_BUFFER_SIZE) {
    Fifo->First = 0;
  }

  return EFI_SUCCESS;
}

/**
  Reads and writes all avaliable data.

  @param[in] SerialDevice         The device to flush

  @retval EFI_SUCCESS             Data was read/written successfully.
  @retval EFI_OUT_OF_RESOURCES    Failed because software receive FIFO is full. Note, when
                                  this happens, pending writes are not done.
**/
EFI_STATUS
PciSerialReceiveTransmit (
  IN SERIAL_DEV                   *SerialDevice
  )
{
  SERIAL_PORT_LSR Lsr;
  UINT8           Data;
  BOOLEAN         ReceiveFifoFull;
  SERIAL_PORT_MSR Msr;
  SERIAL_PORT_MCR Mcr;
  UINTN           TimeOut;

  Data = 0;

  ///
  /// Begin the read or write
  ///
  if (SerialDevice->SoftwareLoopbackEnable) {
    do {
      ReceiveFifoFull = PciSerialFifoFull (&SerialDevice->Receive);
      if (!PciSerialFifoEmpty (&SerialDevice->Transmit)) {
        PciSerialFifoRemove (&SerialDevice->Transmit, &Data);
        if (ReceiveFifoFull) {
          return EFI_OUT_OF_RESOURCES;
        }

        PciSerialFifoAdd (&SerialDevice->Receive, Data);
      }
    } while (!PciSerialFifoEmpty (&SerialDevice->Transmit));
  } else {
    ReceiveFifoFull = PciSerialFifoFull (&SerialDevice->Receive);
    do {
      Lsr.Data = READ_LSR (SerialDevice->PciIo, SerialDevice->BarIndex);
#ifdef EFI_NT_EMULATOR
      ///
      /// This is required for NT to avoid a forever-spin...
      /// This would be better if READ_LSR was a polling operation
      /// that would timeout.
      ///
      Lsr.Bits.THRE = 1;
#endif
      ///
      /// Flush incomming data to prevent a an overrun during a long write
      ///
      if (Lsr.Bits.DR && !ReceiveFifoFull) {
        ReceiveFifoFull = PciSerialFifoFull (&SerialDevice->Receive);
        if (!ReceiveFifoFull) {
          if (Lsr.Bits.FIFOE || Lsr.Bits.OE || Lsr.Bits.PE || Lsr.Bits.FE || Lsr.Bits.BI) {
            if (Lsr.Bits.FIFOE || Lsr.Bits.PE || Lsr.Bits.FE || Lsr.Bits.BI) {
              Data = READ_RBR (SerialDevice->PciIo, SerialDevice->BarIndex);
              continue;
            }
          }
          ///
          /// Make sure the receive data will not be missed, Assert DTR
          ///
          if (SerialDevice->HardwareFlowControl) {
            Mcr.Data = READ_MCR (SerialDevice->PciIo, SerialDevice->BarIndex);
            Mcr.Bits.DTRC &= 0;
            WRITE_MCR (SerialDevice->PciIo, SerialDevice->BarIndex, Mcr.Data);
          }

          Data = READ_RBR (SerialDevice->PciIo, SerialDevice->BarIndex);

          ///
          /// Deassert DTR
          ///
          if (SerialDevice->HardwareFlowControl) {
            Mcr.Data = READ_MCR (SerialDevice->PciIo, SerialDevice->BarIndex);
            Mcr.Bits.DTRC |= 1;
            WRITE_MCR (SerialDevice->PciIo, SerialDevice->BarIndex, Mcr.Data);
          }

          PciSerialFifoAdd (&SerialDevice->Receive, Data);

          continue;
        }
      }
      ///
      /// Do the write
      ///
      if (Lsr.Bits.THRE && !PciSerialFifoEmpty (&SerialDevice->Transmit)) {
        ///
        /// Make sure the transmit data will not be missed
        ///
        if (SerialDevice->HardwareFlowControl) {
          ///
          /// Send RTS
          ///
          Mcr.Data = READ_MCR (SerialDevice->PciIo, SerialDevice->BarIndex);
          Mcr.Bits.RTS |= 1;
          WRITE_MCR (SerialDevice->PciIo, SerialDevice->BarIndex, Mcr.Data);
          ///
          /// Wait for CTS
          ///
          TimeOut   = 0;
          Msr.Data  = READ_MSR (SerialDevice->PciIo, SerialDevice->BarIndex);
          while (!Msr.Bits.CTS) {
            gBS->Stall (TIMEOUT_STALL_INTERVAL);
            TimeOut++;
            if (TimeOut > 5) {
              break;
            }

            Msr.Data = READ_MSR (SerialDevice->PciIo, SerialDevice->BarIndex);
          }

          if (Msr.Bits.CTS) {
            PciSerialFifoRemove (&SerialDevice->Transmit, &Data);
            WRITE_THR (SerialDevice->PciIo, SerialDevice->BarIndex, Data);
          }
        }
        ///
        /// write the data out
        ///
        if (!SerialDevice->HardwareFlowControl) {
          PciSerialFifoRemove (&SerialDevice->Transmit, &Data);
          WRITE_THR (SerialDevice->PciIo, SerialDevice->BarIndex, Data);
        }
        ///
        /// Make sure the transmit data will not be missed
        ///
        if (SerialDevice->HardwareFlowControl) {
          ///
          /// Assert RTS
          ///
          Mcr.Data = READ_MCR (SerialDevice->PciIo, SerialDevice->BarIndex);
          Mcr.Bits.RTS &= 0;
          WRITE_MCR (SerialDevice->PciIo, SerialDevice->BarIndex, Mcr.Data);
        }
      }
    } while (Lsr.Bits.THRE && !PciSerialFifoEmpty (&SerialDevice->Transmit));
  }

  return EFI_SUCCESS;
}

///
/// Interface Functions
///

/**
  Reset serial device

  @param[in] This                 Pointer to EFI_SERIAL_IO_PROTOCOL

  @retval EFI_SUCCESS             Reset successfully
  @retval EFI_DEVICE_ERROR        Failed to reset
**/
EFI_STATUS
EFIAPI
PciSerialReset (
  IN EFI_SERIAL_IO_PROTOCOL       *This
  )
{
  EFI_STATUS      Status;
  SERIAL_DEV      *SerialDevice;
  SERIAL_PORT_LCR Lcr;
  SERIAL_PORT_IER Ier;
  SERIAL_PORT_MCR Mcr;
  SERIAL_PORT_FCR Fcr;
  EFI_TPL         Tpl;

  SerialDevice  = SERIAL_DEV_FROM_THIS (This);

  Tpl           = gBS->RaiseTPL (TPL_NOTIFY);

  ///
  /// Make sure DLAB is 0.
  ///
  Lcr.Data      = READ_LCR (SerialDevice->PciIo, SerialDevice->BarIndex);
  Lcr.Bits.DLAB = 0;
  WRITE_LCR (SerialDevice->PciIo, SerialDevice->BarIndex, Lcr.Data);

  ///
  /// Turn off all interrupts
  ///
  Ier.Data        = READ_IER (SerialDevice->PciIo, SerialDevice->BarIndex);
  Ier.Bits.RAVIE  = 0;
  Ier.Bits.THEIE  = 0;
  Ier.Bits.RIE    = 0;
  Ier.Bits.MIE    = 0;
  WRITE_IER (SerialDevice->PciIo, SerialDevice->BarIndex, Ier.Data);

  ///
  /// Disable the FIFO.
  ///
  Fcr.Bits.TRFIFOE = 0;
  WRITE_FCR (SerialDevice->PciIo, SerialDevice->BarIndex, Fcr.Data);

  ///
  /// Turn off loopback and disable device interrupt.
  ///
  Mcr.Data      = READ_MCR (SerialDevice->PciIo, SerialDevice->BarIndex);
  Mcr.Bits.OUT1 = 0;
  Mcr.Bits.OUT2 = 0;
  Mcr.Bits.LME  = 0;
  WRITE_MCR (SerialDevice->PciIo, SerialDevice->BarIndex, Mcr.Data);

  ///
  /// Clear the scratch pad register
  ///
  WRITE_SCR (SerialDevice->PciIo, SerialDevice->BarIndex, 0);

  ///
  /// Go set the current attributes
  ///
  Status = This->SetAttributes (
                  This,
                  This->Mode->BaudRate,
                  This->Mode->ReceiveFifoDepth,
                  This->Mode->Timeout,
                  This->Mode->Parity,
                  (UINT8) This->Mode->DataBits,
                  This->Mode->StopBits
                  );

  if (EFI_ERROR (Status)) {
    gBS->RestoreTPL (Tpl);
    return EFI_DEVICE_ERROR;
  }
  ///
  /// Go set the current control bits
  ///
  Status = This->SetControl (
                  This,
                  This->Mode->ControlMask
                  );

  if (EFI_ERROR (Status)) {
    gBS->RestoreTPL (Tpl);
    return EFI_DEVICE_ERROR;
  }

  gBS->RestoreTPL (Tpl);

  ///
  /// Device reset is complete
  ///
  return EFI_SUCCESS;
}

/**
  Set new attributes to a serial device

  @param[in] This                 Pointer to EFI_SERIAL_IO_PROTOCOL
  @param[in] BaudRate             The baudrate of the serial device
  @param[in] ReceiveFifoDepth     Fifo depth
  @param[in] Timeout              The request timeout for a single char
  @param[in] Parity               The type of parity used in serial device
  @param[in] DataBits             Number of databits used in serial device
  @param[in] StopBits             Number of stopbits used in serial device

  @retval EFI_SUCCESS             The new attributes were set
  @retval EFI_INVALID_PARAMETERS  One or more attributes have an unsupported value
  @exception EFI_UNSUPPORTED      Data Bits can not set to 5 or 6
  @retval EFI_DEVICE_ERROR        The serial device is not functioning correctly (no return)
**/
EFI_STATUS
EFIAPI
PciSerialSetAttributes (
  IN EFI_SERIAL_IO_PROTOCOL       *This,
  IN UINT64                       BaudRate,
  IN UINT32                       ReceiveFifoDepth,
  IN UINT32                       Timeout,
  IN EFI_PARITY_TYPE              Parity,
  IN UINT8                        DataBits,
  IN EFI_STOP_BITS_TYPE           StopBits
  )
{
  EFI_STATUS                Status;
  SERIAL_DEV                *SerialDevice;
  UINT32                    Divisor;
  UINT32                    Remained;
  SERIAL_PORT_LCR           Lcr;
  EFI_DEVICE_PATH_PROTOCOL  *NewDevicePath;
  EFI_TPL                   Tpl;

  SerialDevice = SERIAL_DEV_FROM_THIS (This);

  ///
  /// DEBUG ((EFI_D_ERROR, "Info: Timeout = %d\n", Timeout));
  ///
  /// Increase timeout by a factor of 3 to fix character drop-out with SOL.
  ///
  Timeout = Timeout * 100;

  ///
  /// Check for default settings and fill in actual values.
  ///
  if (BaudRate == 0) {
    BaudRate = SERIAL_PORT_DEFAULT_BAUD_RATE;
  }

  if (ReceiveFifoDepth == 0) {
    ReceiveFifoDepth = SERIAL_PORT_DEFAULT_RECEIVE_FIFO_DEPTH;
  }

  if (Timeout == 0) {
    Timeout = SERIAL_PORT_DEFAULT_TIMEOUT;
  }

  if (Parity == DefaultParity) {
    Parity = SERIAL_PORT_DEFAULT_PARITY;
  }

  if (DataBits == 0) {
    DataBits = SERIAL_PORT_DEFAULT_DATA_BITS;
  }

  if (StopBits == DefaultStopBits) {
    StopBits = SERIAL_PORT_DEFAULT_STOP_BITS;
  }
  ///
  /// 5 and 6 data bits can not be verified on a 16550A UART
  /// Return EFI_INVALID_PARAMETER if an attempt is made to use these settings.
  ///
  if ((DataBits == 5) || (DataBits == 6)) {
    return EFI_INVALID_PARAMETER;
  }
  ///
  /// Make sure all parameters are valid
  ///
  if ((BaudRate > SERIAL_PORT_MAX_BAUD_RATE) || (BaudRate < SERIAL_PORT_MIN_BAUD_RATE)) {
    return EFI_INVALID_PARAMETER;
  }
  ///
  /// 50,75,110,134,150,300,600,1200,1800,2000,2400,3600,4800,7200,9600,19200,
  /// 38400,57600,115200
  ///
  if (BaudRate < 75) {
    BaudRate = 50;
  } else if (BaudRate < 110) {
    BaudRate = 75;
  } else if (BaudRate < 134) {
    BaudRate = 110;
  } else if (BaudRate < 150) {
    BaudRate = 134;
  } else if (BaudRate < 300) {
    BaudRate = 150;
  } else if (BaudRate < 600) {
    BaudRate = 300;
  } else if (BaudRate < 1200) {
    BaudRate = 600;
  } else if (BaudRate < 1800) {
    BaudRate = 1200;
  } else if (BaudRate < 2000) {
    BaudRate = 1800;
  } else if (BaudRate < 2400) {
    BaudRate = 2000;
  } else if (BaudRate < 3600) {
    BaudRate = 2400;
  } else if (BaudRate < 4800) {
    BaudRate = 3600;
  } else if (BaudRate < 7200) {
    BaudRate = 4800;
  } else if (BaudRate < 9600) {
    BaudRate = 7200;
  } else if (BaudRate < 19200) {
    BaudRate = 9600;
  } else if (BaudRate < 38400) {
    BaudRate = 19200;
  } else if (BaudRate < 57600) {
    BaudRate = 38400;
  } else if (BaudRate < 115200) {
    BaudRate = 57600;
  } else if (BaudRate <= SERIAL_PORT_MAX_BAUD_RATE) {
    BaudRate = 115200;
  }

  if ((ReceiveFifoDepth < 1) || (ReceiveFifoDepth > SERIAL_PORT_MAX_RECEIVE_FIFO_DEPTH)) {
    return EFI_INVALID_PARAMETER;
  }

  if ((Timeout < SERIAL_PORT_MIN_TIMEOUT) || (Timeout > SERIAL_PORT_MAX_TIMEOUT)) {
    return EFI_INVALID_PARAMETER;
  }

  if ((Parity < NoParity) || (Parity > SpaceParity)) {
    return EFI_INVALID_PARAMETER;
  }

  if ((DataBits < 5) || (DataBits > 8)) {
    return EFI_INVALID_PARAMETER;
  }

  if ((StopBits < OneStopBit) || (StopBits > TwoStopBits)) {
    return EFI_INVALID_PARAMETER;
  }
  ///
  /// for DataBits = 5, StopBits can not set TwoStopBits
  ///
  /// if ((DataBits == 5) && (StopBits == TwoStopBits)) {
  ///  return EFI_INVALID_PARAMETER;
  /// }
  ///
  /// for DataBits = 6,7,8, StopBits can not set OneFiveStopBits
  ///
  if ((DataBits >= 6) && (DataBits <= 8) && (StopBits == OneFiveStopBits)) {
    return EFI_INVALID_PARAMETER;
  }
  ///
  /// See if the new attributes already match the current attributes
  ///
  if (SerialDevice->UartDevicePath.BaudRate == BaudRate &&
      SerialDevice->UartDevicePath.DataBits == DataBits &&
      SerialDevice->UartDevicePath.Parity == Parity &&
      SerialDevice->UartDevicePath.StopBits == StopBits &&
      SerialDevice->SerialMode.ReceiveFifoDepth == ReceiveFifoDepth &&
      SerialDevice->SerialMode.Timeout == Timeout
      ) {
    return EFI_SUCCESS;
  }
  ///
  /// Compute divisor use to program the baud rate using a round determination
  ///
  Divisor = (UINT32) DivU64x32Remainder (SERIAL_PORT_INPUT_CLOCK, ((UINT32) BaudRate * 16), &Remained);
  if (Remained) {
    Divisor += 1;
  }

  if ((Divisor == 0) || (Divisor & 0xffff0000)) {
    return EFI_INVALID_PARAMETER;
  }

  Tpl = gBS->RaiseTPL (TPL_NOTIFY);

  ///
  /// Compute the actual baud rate that the serial port will be programmed for.
  ///
  BaudRate = SERIAL_PORT_INPUT_CLOCK / Divisor / 16;

  ///
  /// Put serial port on Divisor Latch Mode
  ///
  Lcr.Data      = READ_LCR (SerialDevice->PciIo, SerialDevice->BarIndex);
  Lcr.Bits.DLAB = 1;
  WRITE_LCR (SerialDevice->PciIo, SerialDevice->BarIndex, Lcr.Data);

  ///
  /// Write the divisor to the serial port
  ///
  WRITE_DLL (SerialDevice->PciIo, SerialDevice->BarIndex, (UINT8) (Divisor & 0xff));
  WRITE_DLM (SerialDevice->PciIo, SerialDevice->BarIndex, (UINT8) ((Divisor >> 8) & 0xff));

  ///
  /// Put serial port back in normal mode and set remaining attributes.
  ///
  Lcr.Bits.DLAB = 0;

  switch (Parity) {
  case NoParity:
    Lcr.Bits.PAREN    = 0;
    Lcr.Bits.EVENPAR  = 0;
    Lcr.Bits.STICPAR  = 0;
    break;

  case EvenParity:
    Lcr.Bits.PAREN    = 1;
    Lcr.Bits.EVENPAR  = 1;
    Lcr.Bits.STICPAR  = 0;
    break;

  case OddParity:
    Lcr.Bits.PAREN    = 1;
    Lcr.Bits.EVENPAR  = 0;
    Lcr.Bits.STICPAR  = 0;
    break;

  case SpaceParity:
    Lcr.Bits.PAREN    = 1;
    Lcr.Bits.EVENPAR  = 1;
    Lcr.Bits.STICPAR  = 1;
    break;

  case MarkParity:
    Lcr.Bits.PAREN    = 1;
    Lcr.Bits.EVENPAR  = 0;
    Lcr.Bits.STICPAR  = 1;
    break;
  default:
    break;
  }

  switch (StopBits) {
  case OneStopBit:
    Lcr.Bits.STOPB = 0;
    break;

  case OneFiveStopBits:
  case TwoStopBits:
    Lcr.Bits.STOPB = 1;
    break;
  default:
    break;
  }
  ///
  /// DataBits
  ///
  Lcr.Bits.SERIALDB = (UINT8) ((DataBits - 5) & 0x03);
  WRITE_LCR (SerialDevice->PciIo, SerialDevice->BarIndex, Lcr.Data);

  ///
  /// Set the Serial I/O mode
  ///
  This->Mode->BaudRate          = BaudRate;
  This->Mode->ReceiveFifoDepth  = ReceiveFifoDepth;
  This->Mode->Timeout           = Timeout;
  This->Mode->Parity            = Parity;
  This->Mode->DataBits          = DataBits;
  This->Mode->StopBits          = StopBits;

  ///
  /// See if Device Path Node has actually changed
  ///
  if (SerialDevice->UartDevicePath.BaudRate == BaudRate &&
      SerialDevice->UartDevicePath.DataBits == DataBits &&
      SerialDevice->UartDevicePath.Parity == Parity &&
      SerialDevice->UartDevicePath.StopBits == StopBits
      ) {
    gBS->RestoreTPL (Tpl);
    return EFI_SUCCESS;
  }
  ///
  /// Update the device path
  ///
  SerialDevice->UartDevicePath.BaudRate = BaudRate;
  SerialDevice->UartDevicePath.DataBits = DataBits;
  SerialDevice->UartDevicePath.Parity   = (UINT8) Parity;
  SerialDevice->UartDevicePath.StopBits = (UINT8) StopBits;

  NewDevicePath = AppendDevicePathNode (
                    SerialDevice->ParentDevicePath,
                    (EFI_DEVICE_PATH_PROTOCOL *) &SerialDevice->UartDevicePath
                    );
  if (NewDevicePath == NULL) {
    gBS->RestoreTPL (Tpl);
    return EFI_DEVICE_ERROR;
  }

  if (SerialDevice->Handle != NULL) {
    Status = gBS->ReinstallProtocolInterface (
                    SerialDevice->Handle,
                    &gEfiDevicePathProtocolGuid,
                    SerialDevice->DevicePath,
                    NewDevicePath
                    );
    if (EFI_ERROR (Status)) {
      gBS->RestoreTPL (Tpl);
      return Status;
    }
  }

  if (SerialDevice->DevicePath) {
    FreePool (SerialDevice->DevicePath);
  }

  SerialDevice->DevicePath = NewDevicePath;

  gBS->RestoreTPL (Tpl);

  return EFI_SUCCESS;
}

/**
  Set ControlBits

  @param[in] This                 Pointer to EFI_SERIAL_IO_PROTOCOL
  @param[in] Control              Control bits that can be settable

  @retval EFI_SUCCESS             New Control bits were set successfully
  @retval EFI_UNSUPPORTED         The Control bits wanted to set are not supported
**/
EFI_STATUS
EFIAPI
PciSerialSetControl (
  IN EFI_SERIAL_IO_PROTOCOL       *This,
  IN UINT32                       Control
  )
{
  SERIAL_DEV      *SerialDevice;
  SERIAL_PORT_MCR Mcr;
  EFI_TPL         Tpl;

  ///
  /// The control bits that can be set are :
  ///     EFI_SERIAL_DATA_TERMINAL_READY: 0x0001      // WO
  ///     EFI_SERIAL_REQUEST_TO_SEND: 0x0002          // WO
  ///     EFI_SERIAL_HARDWARE_LOOPBACK_ENABLE: 0x1000 // RW
  ///     EFI_SERIAL_SOFTWARE_LOOPBACK_ENABLE: 0x2000 // RW
  ///
  SerialDevice = SERIAL_DEV_FROM_THIS (This);

  ///
  /// first determine the parameter is invalid
  ///
  if (Control & 0xffff8ffc) {
    return EFI_UNSUPPORTED;
  }

  Tpl = gBS->RaiseTPL (TPL_NOTIFY);

  Mcr.Data = READ_MCR (SerialDevice->PciIo, SerialDevice->BarIndex);
  Mcr.Bits.DTRC = 0;
  Mcr.Bits.RTS = 0;
  Mcr.Bits.LME = 0;
  SerialDevice->SoftwareLoopbackEnable = FALSE;
  SerialDevice->HardwareFlowControl = FALSE;

  if (Control & EFI_SERIAL_DATA_TERMINAL_READY) {
    Mcr.Bits.DTRC = 1;
  }

  if (Control & EFI_SERIAL_REQUEST_TO_SEND) {
    Mcr.Bits.RTS = 1;
  }

  if (Control & EFI_SERIAL_HARDWARE_LOOPBACK_ENABLE) {
    Mcr.Bits.LME = 1;
  }

  if (Control & EFI_SERIAL_HARDWARE_FLOW_CONTROL_ENABLE) {
    SerialDevice->HardwareFlowControl = TRUE;
  }

  WRITE_MCR (SerialDevice->PciIo, SerialDevice->BarIndex, Mcr.Data);

  if (Control & EFI_SERIAL_SOFTWARE_LOOPBACK_ENABLE) {
    SerialDevice->SoftwareLoopbackEnable = TRUE;
  }

  gBS->RestoreTPL (Tpl);

  return EFI_SUCCESS;
}

/**
  Get ControlBits

  @param[in] This                 Pointer to EFI_SERIAL_IO_PROTOCOL
  @param[in] Control              Control signals of the serial device

  @retval EFI_SUCCESS             Get Control signals successfully
**/
EFI_STATUS
EFIAPI
PciSerialGetControl (
  IN EFI_SERIAL_IO_PROTOCOL       *This,
  OUT UINT32                      *Control
  )
{
  SERIAL_DEV      *SerialDevice;
  SERIAL_PORT_MSR Msr;
  SERIAL_PORT_MCR Mcr;
  EFI_TPL         Tpl;

  Tpl           = gBS->RaiseTPL (TPL_NOTIFY);

  SerialDevice  = SERIAL_DEV_FROM_THIS (This);

  *Control      = 0;

  ///
  /// Read the Modem Status Register
  ///
  Msr.Data = READ_MSR (SerialDevice->PciIo, SerialDevice->BarIndex);

  if (Msr.Bits.CTS) {
    *Control |= EFI_SERIAL_CLEAR_TO_SEND;
  }

  if (Msr.Bits.DSR) {
    *Control |= EFI_SERIAL_DATA_SET_READY;
  }

  if (Msr.Bits.RI) {
    *Control |= EFI_SERIAL_RING_INDICATE;
  }

  if (Msr.Bits.DCD) {
    *Control |= EFI_SERIAL_CARRIER_DETECT;
  }
  ///
  /// Read the Modem Control Register
  ///
  Mcr.Data = READ_MCR (SerialDevice->PciIo, SerialDevice->BarIndex);

  if (Mcr.Bits.DTRC) {
    *Control |= EFI_SERIAL_DATA_TERMINAL_READY;
  }

  if (Mcr.Bits.RTS) {
    *Control |= EFI_SERIAL_REQUEST_TO_SEND;
  }

  if (Mcr.Bits.LME) {
    *Control |= EFI_SERIAL_HARDWARE_LOOPBACK_ENABLE;
  }

  if (SerialDevice->HardwareFlowControl) {
    *Control |= EFI_SERIAL_HARDWARE_FLOW_CONTROL_ENABLE;
  }
  ///
  /// See if the Transmit FIFO is empty
  ///
  PciSerialReceiveTransmit (SerialDevice);

  if (PciSerialFifoEmpty (&SerialDevice->Transmit)) {
    *Control |= EFI_SERIAL_OUTPUT_BUFFER_EMPTY;
  }
  ///
  /// See if the Receive FIFO is empty.
  ///
  PciSerialReceiveTransmit (SerialDevice);

  if (PciSerialFifoEmpty (&SerialDevice->Receive)) {
    *Control |= EFI_SERIAL_INPUT_BUFFER_EMPTY;
  }

  if (SerialDevice->SoftwareLoopbackEnable) {
    *Control |= EFI_SERIAL_SOFTWARE_LOOPBACK_ENABLE;
  }

  gBS->RestoreTPL (Tpl);

  return EFI_SUCCESS;
}

/**
  Write the specified number of bytes to serial device

  @param[in] This                 Pointer to EFI_SERIAL_IO_PROTOCOL
  @param[in] BufferSize           On input the size of Buffer, on output the amount of data actually written
  @param[in] Buffer               The buffer of data to write

  @retval EFI_SUCCESS             The data were written successfully
  @retval EFI_DEVICE_ERROR        The device reported an error
  @retval EFI_TIMEOUT             The write operation was stopped due to timeout
**/
EFI_STATUS
EFIAPI
PciSerialWrite (
  IN EFI_SERIAL_IO_PROTOCOL       *This,
  IN OUT UINTN                    *BufferSize,
  IN VOID                         *Buffer
  )
{
  SERIAL_DEV      *SerialDevice;
  UINT8           *CharBuffer;
  UINT32          Index;
  UINTN           Elapsed;
  UINTN           ActualWrite;
  EFI_TPL         Tpl;
  SERIAL_PORT_MCR Mcr;

  SerialDevice  = SERIAL_DEV_FROM_THIS (This);
  Elapsed       = 0;
  ActualWrite   = 0;

  if (*BufferSize == 0) {
    return EFI_SUCCESS;
  }

  if (!Buffer) {
    return EFI_DEVICE_ERROR;
  }

  Tpl         = gBS->RaiseTPL (TPL_NOTIFY);

  CharBuffer  = (UINT8 *) Buffer;

  for (Index = 0; Index < *BufferSize; Index++) {
    PciSerialFifoAdd (&SerialDevice->Transmit, CharBuffer[Index]);

    while
    (
      PciSerialReceiveTransmit (SerialDevice) != EFI_SUCCESS ||
      PciSerialFifoEmpty (&SerialDevice->Transmit) == FALSE
    ) {
      ///
      ///  Unsuccessful write so check if timeout has expired, if not,
      ///  stall for a bit, increment time elapsed, and try again
      ///
      if (Elapsed >= This->Mode->Timeout) {
        *BufferSize = ActualWrite;
        if (PciSerialFifoEmpty (&SerialDevice->Transmit)) {
          gBS->RestoreTPL (Tpl);
          return EFI_TIMEOUT;
        }
      }

      gBS->Stall (TIMEOUT_STALL_INTERVAL);

      Elapsed += TIMEOUT_STALL_INTERVAL;
    } // end while
    ActualWrite++;
    ///
    ///  Successful write so reset timeout
    ///
    Elapsed = 0;

  } // end for
  ///
  /// FW expects DTR bit to be SET before sending data. So enable DTR bit always.
  ///
  Mcr.Data = READ_MCR (SerialDevice->PciIo, SerialDevice->BarIndex);
  Mcr.Bits.DTRC |= 1;
  WRITE_MCR (SerialDevice->PciIo, SerialDevice->BarIndex, Mcr.Data);

  gBS->RestoreTPL (Tpl);

  return EFI_SUCCESS;
}

/**
  Read the specified number of bytes from serial device

  @param[in] This                 Pointer to EFI_SERIAL_IO_PROTOCOL
  @param[in] BufferSize           On input the size of Buffer, on output the amount of data returned in buffer
  @param[in] Buffer               The buffer to return the data into

  @retval EFI_SUCCESS             The data were read successfully
  @retval EFI_DEVICE_ERROR        The device reported an error
  @retval EFI_TIMEOUT             The read operation was stopped due to timeout
**/
EFI_STATUS
EFIAPI
PciSerialRead (
  IN EFI_SERIAL_IO_PROTOCOL       *This,
  IN OUT UINTN                    *BufferSize,
  OUT VOID                        *Buffer
  )
{
  SERIAL_DEV  *SerialDevice;
  UINT32      Index;
  UINT8       *CharBuffer;
  UINTN       Elapsed;
  EFI_STATUS  Status;
  EFI_TPL     Tpl;

  SerialDevice  = SERIAL_DEV_FROM_THIS (This);
  Elapsed       = 0;

  if (*BufferSize == 0) {
    return EFI_SUCCESS;
  }

  if (!Buffer) {
    return EFI_DEVICE_ERROR;
  }
  ///
  /// SerialDevice->Receive.First = 0;
  /// SerialDevice->Receive.Last  = 0;
  /// SerialDevice->Receive.Surplus = SERIAL_MAX_BUFFER_SIZE;
  ///
  Tpl     = gBS->RaiseTPL (TPL_NOTIFY);

  Status  = PciSerialReceiveTransmit (SerialDevice);

  if (EFI_ERROR (Status)) {
    *BufferSize = 0;

    gBS->RestoreTPL (Tpl);

    return EFI_DEVICE_ERROR;
  }

  CharBuffer = (UINT8 *) Buffer;
  for (Index = 0; Index < *BufferSize; Index++) {
    while (PciSerialFifoRemove (&SerialDevice->Receive, &(CharBuffer[Index])) != EFI_SUCCESS) {
      ///
      ///  Unsuccessful read so check if timeout has expired, if not,
      ///  stall for a bit, increment time elapsed, and try again
      ///  Need this time out to get conspliter to work.
      ///
      if (Elapsed >= This->Mode->Timeout) {
        *BufferSize = Index;
        gBS->RestoreTPL (Tpl);
        return EFI_TIMEOUT;
      }

      gBS->Stall (TIMEOUT_STALL_INTERVAL);
      Elapsed += TIMEOUT_STALL_INTERVAL;

      Status = PciSerialReceiveTransmit (SerialDevice);
      if (Status == EFI_DEVICE_ERROR) {
        *BufferSize = Index;
        gBS->RestoreTPL (Tpl);
        return EFI_DEVICE_ERROR;
      }
    } // end while
    ///
    ///  Successful read so reset timeout
    ///
    Elapsed = 0;
  } // end for
  PciSerialReceiveTransmit (SerialDevice);

  gBS->RestoreTPL (Tpl);

  return EFI_SUCCESS;
}

/**
  Check serial port status.

  @param[in] SerialDevice         The serial device instance

  @retval True                    It is present.
  @retval False                   No present.
**/
BOOLEAN
PciSerialPortPresent (
  IN SERIAL_DEV                   *SerialDevice
  )
{
  UINT8   Temp;
  BOOLEAN Status;

  Status = TRUE;

  ///
  /// Save SCR reg
  ///
  Temp = READ_SCR (SerialDevice->PciIo, SerialDevice->BarIndex);
  WRITE_SCR (SerialDevice->PciIo, SerialDevice->BarIndex, 0xAA);

  if (READ_SCR (SerialDevice->PciIo, SerialDevice->BarIndex) != 0xAA) {
#ifndef EFI_NT_EMULATOR
    Status = FALSE;
#endif
  }

  WRITE_SCR (SerialDevice->PciIo, SerialDevice->BarIndex, 0x55);

  if (READ_SCR (SerialDevice->PciIo, SerialDevice->BarIndex) != 0x55) {
#ifndef EFI_NT_EMULATOR
    Status = FALSE;
#endif
  }
  ///
  /// Restore SCR
  ///
  WRITE_SCR (SerialDevice->PciIo, SerialDevice->BarIndex, Temp);
  return Status;
}

/**
  PCI I/O read for byte only

  @param[in] PciIo                Pointer of Pci IO protocol
  @param[in] BarIndex             Index of the BAR within PCI device
  @param[in] Offset               Offset of the BARIndex within PCI device

  @retval                         Return value read
**/
UINT8
PciSerialReadPort (
  IN EFI_PCI_IO_PROTOCOL          *PciIo,
  IN UINT16                       BarIndex,
  IN UINT16                       Offset
  )
{
  UINT8 Data;

  ///
  /// Use PciIo to access IO
  ///
  PciIo->Io.Read (
              PciIo,
              EfiPciIoWidthUint8,
              (UINT8) BarIndex,
              (UINT16) Offset,
              (UINTN) 1,
              &Data
              );
  return Data;
}

/**
  PCI I/O - write a byte

  @param[in] PciIo                Pointer of Pci IO protocol
  @param[in] BarIndex             Index of the BAR within PCI device
  @param[in] Offset               Offset of the BARIndex within PCI device
  @param[in] Data                 Written value
**/
VOID
PciSerialWritePort (
  IN EFI_PCI_IO_PROTOCOL          *PciIo,
  IN UINT16                       BarIndex,
  IN UINT16                       Offset,
  IN UINT8                        Data
  )
{
  ///
  /// Use PciIo to access IO
  ///
  PciIo->Io.Write (
              PciIo,
              EfiPciIoWidthUint8,
              (UINT8) BarIndex,
              (UINT16) Offset,
              (UINTN) 1,
              &Data
              );
}

/**
  Sol driver entry

  @param[in] ImageHandle          Handle for this drivers loaded image protocol.
  @param[in] SystemTable          EFI system table.

  @retval EFI_SUCCESS             Always return EFI_SUCCESS
**/
EFI_STATUS
EFIAPI
PciSerialControllerDriverEntryPoint (
  IN EFI_HANDLE                   ImageHandle,
  IN EFI_SYSTEM_TABLE             *SystemTable
  )
{
  EFI_STATUS                           Status;
  
  Status = EfiLibInstallDriverBindingComponentName2 (
             ImageHandle,
             SystemTable,
             &mPciSerialControllerDriverBinding,
             ImageHandle,
             &mPciSerialComponentName,
             &mPciSerialComponentName2
             );
  ASSERT_EFI_ERROR (Status);

  return Status;
}
