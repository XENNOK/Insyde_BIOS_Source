/** @file
  PCI Serial driver for standard UARTS on an PCI bus.

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


#include "PciSerial.h"

VOID
PciSerialFailSaveBaudRate (
  IN SERIAL_DEV                     *SerialDevice
  );

EFI_DRIVER_BINDING_PROTOCOL gPciSerialControllerDriver = {
  PciSerialControllerDriverSupported,
  PciSerialControllerDriverStart,
  PciSerialControllerDriverStop,
  0x10,
  NULL,
  NULL
};

SERIAL_DEV  gPciSerialDevTempate = {
  SERIAL_DEV_SIGNATURE,
  NULL, 
  { // SerialIo
    SERIAL_IO_INTERFACE_REVISION,
    PciSerialReset,
    PciSerialSetAttributes,
    PciSerialSetControl,
    PciSerialGetControl,
    PciSerialWrite,
    PciSerialRead,
    NULL
  },
  { // SerialMode
    SERIAL_PORT_DEFAULT_CONTROL_MASK,
    SERIAL_PORT_DEFAULT_TIMEOUT,
    FixedPcdGet64 (PcdUartDefaultBaudRate),     // BaudRate
    SERIAL_PORT_DEFAULT_RECEIVE_FIFO_DEPTH,
    FixedPcdGet8 (PcdUartDefaultDataBits),      // DataBits
    FixedPcdGet8 (PcdUartDefaultParity),        // Parity
    FixedPcdGet8 (PcdUartDefaultStopBits)       // StopBits
  },
  NULL,
  NULL,
  { // UartDevicePath
    {
      MESSAGING_DEVICE_PATH,
      MSG_UART_DP,
      {
        (UINT8) (sizeof (UART_DEVICE_PATH)),
        (UINT8) ((sizeof (UART_DEVICE_PATH)) >> 8)
      }
    },
    0,
    FixedPcdGet64 (PcdUartDefaultBaudRate),    
    FixedPcdGet8 (PcdUartDefaultDataBits),
    FixedPcdGet8 (PcdUartDefaultParity),
    FixedPcdGet8 (PcdUartDefaultStopBits)
  },
  NULL,
  0,    //BaseAddress
  {
    0,
    0,
    SERIAL_MAX_BUFFER_SIZE,
    { 0 }
  },
  {
    0,
    0,
    SERIAL_MAX_BUFFER_SIZE,
    { 0 }
  },
  FALSE,
  FALSE,
  UART16550A,
  NULL
};

UINT64 BaudRate[]     = {115200, 57600, 38400, 19200, 9600};
CHAR8  EscReset[]     = {0x1B, 0x63};
CHAR8  EscGetStatus[] = {0x1B, 0x5B, 0x35, 0x6E};

/**

  Based on CrPolicy to determine inputed device is support
  Console Redirection feature or not.

  @param[in]  CrPolicy                Pointer to ConsoleRedirection policy protocol
  @param[in]  DevicePath            Pointer to device path protocol 

  @retval  TRUE                         The Device support Console Redirection feature
  @retval  FALSE                        The Device unsupport Console Redirection feature

**/
BOOLEAN
PciSerialIsCrSupport (
  IN  EFI_CR_POLICY_PROTOCOL      *CRPolicy,
  IN  EFI_DEVICE_PATH_PROTOCOL    *DevicePath
  )
{
  EFI_STATUS  Status;
  UINTN       DevicePathSize;
  UINTN       Index; 
  UINTN       Start;
  UINTN       CrDevCount;
  UINTN       Bus;
  UINTN       Dev;
  UINTN       Func;

  CrDevCount = 0;  
  DevicePathSize = GetDevicePathSize (DevicePath);
  if (DevicePathSize == 0) {
    return FALSE;
  }
  
  CrDevCount = CRPolicy->CRSerialDevData.IsaDevCount + 
               CRPolicy->CRSerialDevData.PciDevCount + 
               CRPolicy->CRSerialDevData.UsbDevCount;
  
  Start = CRPolicy->CRSerialDevData.IsaDevCount;
  Status = CRGetPciDevLocation (DevicePath, &Bus, &Dev, &Func);
  if (EFI_ERROR(Status)) {
    return FALSE;
  }
  
  for (Index = Start; Index < CrDevCount; Index++) {
    if (CRPolicy->CRSerialDevData.CRDevice[Index].Type == PCI_SERIAL_DEVICE) {
      if (Bus == CRPolicy->CRSerialDevData.CRDevice[Index].Device.PciSerial.Bus &&
          Dev == CRPolicy->CRSerialDevData.CRDevice[Index].Device.PciSerial.Device &&
          Func == CRPolicy->CRSerialDevData.CRDevice[Index].Device.PciSerial.Function) {
        return CRPolicy->CRSerialDevData.CRDevice[Index].CrDevAttr.PortEnable;
      }
    }
  }

  return FALSE;
}


/**
  The module Entry Point for module PciSerial.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
PciSerialControllerDriverEntryPoint(
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS              Status;

  //
  // Install driver model protocol(s).
  //
  Status = EfiLibInstallDriverBindingComponentName2 (
             ImageHandle,
             SystemTable,
             &gPciSerialControllerDriver, 
             ImageHandle,
             &gPciSerialComponentName,
             &gPciSerialComponentName2
             );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

/**
  Test controller is a PciSerial Controller.

  @param This                 Pointer of EFI_DRIVER_BINDING_PROTOCOL
  @param Controller           driver's controller
  @param RemainingDevicePath  children device path

  @retval EFI_UNSUPPORTED This driver does not support this device
  @retval EFI_SUCCESS     This driver supports this device
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
  union {
    UINT8                     ByteBuf[4];
    UINT16                    WordBuf[2];
  }                         Buffer;
  UINT16                    Temp;

  //
  // Open the IO Abstraction(s) needed to perform the supported test
  //
  Status = gBS->OpenProtocol (
                  Controller,   
                  &gEfiDevicePathProtocolGuid,  
                  (VOID **)&ParentDevicePath,
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

  Status = gBS->OpenProtocol (
                  Controller,  
                  &gEfiPciIoProtocolGuid, 
                  (VOID **)&PciIo,
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

  //
  // Use the PCI I/O Protocol to see if Controller is standard ISA UART that
  // can be managed by this driver.
  //
  Status = EFI_SUCCESS;

  // 
  // Looks for a PCI CLASS / SUBCLASS / INTERFACE of 0x07 / 0x00 / 0x02
  // To allow supportting all PCI Devices that are 16550 compatible UARTS.
  //
  // Also if want general PCI Serial com devices to work as well
  // can duplicate this driver one for Tekoa and AMT and the other
  // for general serial devices.
  //
  Status = PciIo->Pci.Read (
                        PciIo,
                        EfiPciIoWidthUint32,
                        (UINT32) PCI_REVISION_ID_OFFSET,
                        (UINTN) 1,
                        (VOID *) &Buffer
                        );
  //
  // Check Pci Serial Device Class code
  //
  if ((Buffer.ByteBuf[3] != PCI_CLASS_SCC) ||
      (Buffer.ByteBuf[2] != PCI_SUBCLASS_SERIAL) ||
      (Buffer.ByteBuf[1] != PCI_IF_16550)) {
    Status = EFI_UNSUPPORTED;
    goto Error;
  }

  //
  // Make sure the PCI io space is enabled
  //
  Temp = 0x0003;
  PciIo->Pci.Write (
               PciIo,
               EfiPciIoWidthUint16,
               0x04,
               0x01,
               &Temp		
               );

  //
  // Make sure RemainingDevicePath is valid
  //
  if (RemainingDevicePath != NULL) {
    Status = EFI_UNSUPPORTED;
    CopyMem (&UartNode, (UART_DEVICE_PATH *)RemainingDevicePath, sizeof (UART_DEVICE_PATH));
    if (UartNode.Header.Type != MESSAGING_DEVICE_PATH ||
        UartNode.Header.SubType != MSG_UART_DP ||
        DevicePathNodeLength((EFI_DEVICE_PATH_PROTOCOL *)&UartNode) != sizeof(UART_DEVICE_PATH)) {
      goto Error;
    }
    if (UartNode.BaudRate < 0 || UartNode.BaudRate > SERIAL_PORT_MAX_BAUD_RATE) {
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
    if ((UartNode.DataBits >= 6) && (UartNode.DataBits <=8) && (UartNode.StopBits == OneFiveStopBits)) {
      goto Error;
    }
    Status = EFI_SUCCESS;
  }

Error:
  //
  // Close the I/O Abstraction(s) used to perform the supported test
  //
  gBS->CloseProtocol (
         Controller,  
         &gEfiPciIoProtocolGuid, 
         This->DriverBindingHandle,   
         Controller   
         );
  return Status;
}

/**
  Start this driver on ControllerHandle by opening a PciIo protocol, creating
  SERIAL_DEV device and install gEfiSerialIoProtocolGuid
  finally.

  @param  This                 Protocol instance pointer.
  @param  ControllerHandle     Handle of device to bind driver to
  @param  RemainingDevicePath  Optional parameter use to pick a specific child
                               device to start.

  @retval EFI_SUCCESS          This driver is added to ControllerHandle
  @retval EFI_ALREADY_STARTED  This driver is already running on ControllerHandle
  @retval other                This driver does not support this device

**/
EFI_STATUS
EFIAPI
PciSerialControllerDriverStart (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  )
{
  EFI_STATUS                           Status;
  EFI_PCI_IO_PROTOCOL                  *PciIo;
  SERIAL_DEV                           *SerialDevice;
  UINTN                                Index;
  UART_DEVICE_PATH                     Node;
  EFI_DEVICE_PATH_PROTOCOL             *ParentDevicePath;
  EFI_OPEN_PROTOCOL_INFORMATION_ENTRY  *OpenInfoBuffer;
  UINTN                                EntryCount;
  EFI_SERIAL_IO_PROTOCOL               *SerialIo;
  UINT64                               *Supports;
  UINT64                               Temp;
  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR    *Ptr;
  VOID                                 **Resources;
  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR    Temp1;
  EFI_CR_POLICY_PROTOCOL               *CRPolicy;
  BOOLEAN                              CREnable;
  BOOLEAN                              CRFailSave;

  SerialDevice = NULL;

  //
  // Get the Parent Device Path
  //
  Status = gBS->OpenProtocol (
                  Controller,   
                  &gEfiDevicePathProtocolGuid,  
                  (VOID **)&ParentDevicePath,
                  This->DriverBindingHandle,   
                  Controller,   
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status) && Status != EFI_ALREADY_STARTED) {
    return Status;
  }                   

  //
  // Get the CR Policy
  //
  CREnable   = FALSE;
  CRFailSave = FALSE;
  Status = gBS->LocateProtocol (&gEfiCRPolicyProtocolGuid, NULL, (VOID **)&CRPolicy);
  if(Status == EFI_SUCCESS) {
    CREnable   = CRPolicy->CREnable;
    CRFailSave = CRPolicy->CRFailSaveBaudrate;
  }

  //
  // Grab the IO abstraction we need to get any work done
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
    goto Error;
  }
  
  if (Status == EFI_ALREADY_STARTED) {

    if (RemainingDevicePath == NULL || IsDevicePathEnd (RemainingDevicePath)) {
      //
      // If RemainingDevicePath is NULL or is the End of Device Path Node
      //
      return EFI_SUCCESS;
    }

    //
    // Make sure a child handle does not already exist.  This driver can only 
    // produce one child per serial port.
    //
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
                        (VOID **)&SerialIo,
                        This->DriverBindingHandle,   
                        Controller,
                        EFI_OPEN_PROTOCOL_GET_PROTOCOL
                        );
        if (!EFI_ERROR (Status)) {
          CopyMem (&Node, RemainingDevicePath, sizeof (UART_DEVICE_PATH));
          SerialDevice = SERIAL_DEV_FROM_THIS (SerialIo);
          
          if (CREnable == TRUE && CRFailSave == TRUE && PciSerialIsCrSupport (CRPolicy, ParentDevicePath)) {
            //
            // Confirm baudrate of remote site and then update to SerailDevice  
            //
            PciSerialFailSaveBaudRate (SerialDevice);
          } else {
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
        }
        break;
      }
    }
    FreePool (OpenInfoBuffer);
    return Status;
  }

  //
  // Initialize the serial device instance
  //
  SerialDevice = AllocateCopyPool (sizeof (SERIAL_DEV), &gPciSerialDevTempate);
  if (SerialDevice == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Error;
  }

  SerialDevice->SerialIo.Mode       = &(SerialDevice->SerialMode);
  SerialDevice->PciIo               = PciIo;
  SerialDevice->ParentDevicePath    = ParentDevicePath;

  //
  // Check if RemainingDevicePath is NULL, 
  // if yes, use the values from the gSerialDevTempate as no remaining device path was
  // passed in.
  //
  if (RemainingDevicePath != NULL) {
    //
    // If RemainingDevicePath isn't NULL, 
    // match the configuration of the RemainingDevicePath. IsHandleSupported()
    // already checked to make sure the RemainingDevicePath contains settings
    // that we can support.
    //
    CopyMem (&SerialDevice->UartDevicePath, RemainingDevicePath, sizeof (UART_DEVICE_PATH));
  }

  AddName (SerialDevice, PciIo);

  Ptr = &Temp1;
  Resources = (VOID **)&Ptr;
  Supports = &Temp;
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
      SerialDevice->BarIndex = (UINT16) Index;
      Status = EFI_SUCCESS;
      break;
    }
  }

  //
  // Report status code the serial present
  //
  REPORT_STATUS_CODE_WITH_DEVICE_PATH (
    EFI_PROGRESS_CODE,
    EFI_P_PC_PRESENCE_DETECT | EFI_PERIPHERAL_SERIAL_PORT,
    ParentDevicePath
    );

  if (!PciSerialPortPresent (SerialDevice)) {
    Status = EFI_DEVICE_ERROR;
    REPORT_STATUS_CODE_WITH_DEVICE_PATH (
      EFI_ERROR_CODE,
      EFI_P_EC_NOT_DETECTED | EFI_PERIPHERAL_SERIAL_PORT,
      ParentDevicePath
      );
    goto Error;
  }


  //
  // Build the device path by appending the UART node to the ParentDevicePath 
  // from the WinNtIo handle. The Uart setings are zero here, since 
  // SetAttribute() will update them to match the current setings.
  //
  SerialDevice->DevicePath = AppendDevicePathNode (
                               ParentDevicePath, 
                               (EFI_DEVICE_PATH_PROTOCOL *)&SerialDevice->UartDevicePath
                               );
  
  if (SerialDevice->DevicePath == NULL) {
    Status =  EFI_DEVICE_ERROR;
    goto Error;
  }                        

  //
  // Fill in Serial I/O Mode structure based on either the RemainingDevicePath or defaults.
  //
  SerialDevice->SerialMode.BaudRate = SerialDevice->UartDevicePath.BaudRate;
  SerialDevice->SerialMode.DataBits = SerialDevice->UartDevicePath.DataBits;
  SerialDevice->SerialMode.Parity   = SerialDevice->UartDevicePath.Parity;
  SerialDevice->SerialMode.StopBits = SerialDevice->UartDevicePath.StopBits;

  //
  // Issue a reset to initialize the COM port
  //
  Status = SerialDevice->SerialIo.Reset (&SerialDevice->SerialIo);
  if (EFI_ERROR (Status)) {
    Status = EFI_SUCCESS;
    goto Error;
  }

  //
  // Confirm baudrate of remote site and then update to SerailDevice  
  //
  if (CREnable == TRUE && CRFailSave == TRUE && PciSerialIsCrSupport (CRPolicy, ParentDevicePath)) {
      PciSerialFailSaveBaudRate (SerialDevice);
  } 

  //
  // Install protocol interfaces for the serial device.
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &SerialDevice->Handle,            
                  &gEfiDevicePathProtocolGuid, SerialDevice->DevicePath,
                  &gEfiSerialIoProtocolGuid,   &SerialDevice->SerialIo,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    goto Error;
  }

  //
  // Open For Child Device
  //
  Status = gBS->OpenProtocol (
                  Controller,   
                  &gEfiPciIoProtocolGuid,  
                  (VOID **)&PciIo,
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
        gBS->FreePool (SerialDevice->DevicePath);
      }
      FreeUnicodeStringTable (SerialDevice->ControllerNameTable);
      gBS->FreePool (SerialDevice);     
    }
  }
  return Status;
}

/**
  Stop this driver on ControllerHandle. Support stoping any child handles
  created by this driver.

  @param  This              Protocol instance pointer.
  @param  ControllerHandle  Handle of device to stop driver on
  @param  NumberOfChildren  Number of Handles in ChildHandleBuffer. If number of
                            children is zero stop the entire bus driver.
  @param  ChildHandleBuffer List of Child Handles to Stop.

  @retval EFI_SUCCESS       This driver is removed ControllerHandle
  @retval other             This driver was not removed from this device

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
  EFI_STATUS                    Status;
  UINTN                         Index;
  BOOLEAN                       AllChildrenStopped;
  EFI_SERIAL_IO_PROTOCOL        *SerialIo;
  SERIAL_DEV                    *SerialDevice;
  EFI_PCI_IO_PROTOCOL           *PciIo;
  EFI_DEVICE_PATH_PROTOCOL      *DevicePath;

  Status = gBS->HandleProtocol (
                 Controller, 
                 &gEfiDevicePathProtocolGuid, 
                 (VOID **)&DevicePath
                 );

  //
  // Report the status code disable the serial
  //
  REPORT_STATUS_CODE_WITH_DEVICE_PATH (
    EFI_PROGRESS_CODE,
    EFI_P_PC_DISABLE | EFI_PERIPHERAL_SERIAL_PORT,
    DevicePath
    );

  //
  // Complete all outstanding transactions to Controller.
  // Don't allow any new transaction to Controller to be started.
  //

  if (NumberOfChildren == 0) {
    //
    // Close the bus driver
    //
    Status = gBS->CloseProtocol (
                     Controller, 
                     &gEfiPciIoProtocolGuid, 
                     This->DriverBindingHandle, 
                     Controller
                     );

    Status =  gBS->CloseProtocol (
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
                    (VOID **)&SerialIo,
                    This->DriverBindingHandle,   
                    Controller,   
                    EFI_OPEN_PROTOCOL_GET_PROTOCOL
                    );
    if (!EFI_ERROR(Status)) {

      SerialDevice = SERIAL_DEV_FROM_THIS (SerialIo);

      Status =  gBS->CloseProtocol (
                       Controller, 
                       &gEfiPciIoProtocolGuid, 
                       This->DriverBindingHandle, 
                       ChildHandleBuffer[Index]
                       );

      Status = gBS->UninstallMultipleProtocolInterfaces (
                 ChildHandleBuffer[Index], 
                 &gEfiDevicePathProtocolGuid,      SerialDevice->DevicePath,
                 &gEfiSerialIoProtocolGuid,        &SerialDevice->SerialIo,
                 NULL
                 );
      if (EFI_ERROR (Status)) {
        gBS->OpenProtocol (
               Controller,   
               &gEfiPciIoProtocolGuid,  
               (VOID **)&PciIo,
               This->DriverBindingHandle,   
               ChildHandleBuffer[Index],
               EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER
               );
      } else {
        if (SerialDevice->DevicePath) {
          gBS->FreePool (SerialDevice->DevicePath);
        }
        FreeUnicodeStringTable (SerialDevice->ControllerNameTable);
        gBS->FreePool (SerialDevice);
      }
    }

    if (EFI_ERROR(Status)) {
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
 
  @param[in]  Fifo  A pointer to the Data Structure SERIAL_DEV_FIFO
   
  @retval  TRUE   the FIFO is full
  @retval  FALSE  the FIFO is not full

**/
BOOLEAN
PciSerialFifoFull(
  IN SERIAL_DEV_FIFO *Fifo
)
{
  if (Fifo->Surplus == 0) {
    return TRUE;
  }
  
  return FALSE;
}

/**
  Detect whether specific FIFO is empty or not

  @param[in]   Fifo   A pointer to the Data Structure SERIAL_DEV_FIFO
  
  @retval   TRUE      the FIFO is empty
  @retval   FALSE     the FIFO is not empty

**/
BOOLEAN
PciSerialFifoEmpty(
  IN SERIAL_DEV_FIFO *Fifo
)
{
  if (Fifo->Surplus == SERIAL_MAX_BUFFER_SIZE ) {
    return TRUE;
  }
  
  return FALSE;
}

/**
  Add data to specific FIFO

  @param[in]   Fifo      A pointer to the Data Structure SERIAL_DEV_FIFO
  @param[in]   Data      the data added to FIFO  

  @retval   EFI_SUCCESS  Add data to specific FIFO successfully
  @retval   EFI_OUT_RESOURCE  Failed to add data because FIFO is already full 

**/
EFI_STATUS
PciSerialFifoAdd(
  IN SERIAL_DEV_FIFO *Fifo,
  IN UINT8           Data
)
{
  //
  //if FIFO full can not add data
  //
  if (PciSerialFifoFull(Fifo)) {
    return EFI_OUT_OF_RESOURCES;
  }
  
  //
  //FIFO is not full can add data
  //
  Fifo->Data[Fifo->Last] = Data;
  Fifo->Surplus --;
  Fifo->Last ++;
  if (Fifo->Last == SERIAL_MAX_BUFFER_SIZE) {
    Fifo->Last = 0;
  }
  
  return EFI_SUCCESS;
}

/**
  Remove data from specific FIFO

  @param[in]   Fifo       A pointer to the Data Structure SERIAL_DEV_FIFO
  @param[in]   Data       the data removed from FIFO  

  @retval   EFI_SUCCESS   Remove data from specific FIFO successfully
  @retval   EFI_OUT_RESOURCE  Failed to remove data because FIFO is empty

**/
EFI_STATUS
PciSerialFifoRemove(
  IN  SERIAL_DEV_FIFO *Fifo,
  OUT UINT8           *Data
)
{
  //
  //if FIFO is empty, no data can remove
  //
  if (PciSerialFifoEmpty(Fifo)) {
    return EFI_OUT_OF_RESOURCES;
  }
  
  //
  //FIFO is not empty, can remove data
  //
  *Data = Fifo->Data[Fifo->First];
  Fifo->Surplus ++;
  Fifo->First ++;
  if (Fifo->First == SERIAL_MAX_BUFFER_SIZE) {
    Fifo->First = 0;
  }
  
  return EFI_SUCCESS;
}

/**
  Reads and writes all avaliable data.
  
  @param[in] SerialDevice       The device to flush

  @retval EFI_SUCCESS        Data was read/written successfully.
  @retval EFI_OUT_RESOURCE   Failed because software receive FIFO is full.  Note, when
                                this happens, pending writes are not done.

**/
EFI_STATUS
PciSerialReceiveTransmit (
  IN SERIAL_DEV *SerialDevice
  )
{
  SERIAL_PORT_LSR Lsr;
  UINT8           Data;
  BOOLEAN         ReceiveFifoFull;
  SERIAL_PORT_MSR Msr;
  SERIAL_PORT_MCR Mcr;
  UINTN           TimeOut;
  
  Data = 0;

  //
  // Begin the read or write
  //
  if (SerialDevice->SoftwareLoopbackEnable) {
    do {
      ReceiveFifoFull = PciSerialFifoFull (&SerialDevice->Receive);
      if (!PciSerialFifoEmpty (&SerialDevice->Transmit)) {
        PciSerialFifoRemove (&SerialDevice->Transmit,&Data);
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
      //
      // This is required for NT to avoid a forever-spin...
      // This would be better if READ_LSR was a polling operation 
      // that would timeout.  
      //
      Lsr.Bits.THRE = 1;
#endif
      //
      // Flush incomming data to prevent a an overrun during a long write
      //
      if (Lsr.Bits.DR && !ReceiveFifoFull) {
        ReceiveFifoFull = PciSerialFifoFull (&SerialDevice->Receive);
        if (!ReceiveFifoFull) {
          if (Lsr.Bits.FIFOE || Lsr.Bits.OE || Lsr.Bits.PE || Lsr.Bits.FE || Lsr.Bits.BI) {
            if (Lsr.Bits.FIFOE || Lsr.Bits.PE || Lsr.Bits.FE || Lsr.Bits.BI) {
              Data = READ_RBR (SerialDevice->PciIo, SerialDevice->BarIndex);
              continue;
            }
          }

          //
          // Make sure the receive data will not be missed, Assert DTR
          //
          if (SerialDevice->HardwareFlowControl) {
            Mcr.Data = READ_MCR(SerialDevice->PciIo, SerialDevice->BarIndex);
            Mcr.Bits.DTRC &= 0;
            WRITE_MCR(SerialDevice->PciIo, SerialDevice->BarIndex, Mcr.Data);
          }

          Data = READ_RBR (SerialDevice->PciIo, SerialDevice->BarIndex);

          //
          // Deassert DTR
          //
          if (SerialDevice->HardwareFlowControl) {
            Mcr.Data = READ_MCR(SerialDevice->PciIo, SerialDevice->BarIndex);
            Mcr.Bits.DTRC |= 1;
            WRITE_MCR(SerialDevice->PciIo, SerialDevice->BarIndex, Mcr.Data);
          }
          
          PciSerialFifoAdd(&SerialDevice->Receive, Data);

          continue;
        }
      }

      //
      // Do the write
      //
      if (Lsr.Bits.THRE && !PciSerialFifoEmpty (&SerialDevice->Transmit)) {
        //
        // Make sure the transmit data will not be missed
        //
        if (SerialDevice->HardwareFlowControl) {
          //
          // Send RTS
          //
          Mcr.Data = READ_MCR(SerialDevice->PciIo, SerialDevice->BarIndex);
          Mcr.Bits.RTS |= 1;
          WRITE_MCR(SerialDevice->PciIo, SerialDevice->BarIndex, Mcr.Data);
          //
          // Wait for CTS
          //
          TimeOut = 0;
          Msr.Data = READ_MSR(SerialDevice->PciIo, SerialDevice->BarIndex);
          while (!Msr.Bits.CTS) {
            gBS->Stall (TIMEOUT_STALL_INTERVAL);
            TimeOut ++;
            if (TimeOut > 5) {
              break;
            }
            Msr.Data = READ_MSR(SerialDevice->PciIo, SerialDevice->BarIndex);
          }
          if (Msr.Bits.CTS) {
            PciSerialFifoRemove (&SerialDevice->Transmit,&Data);
            WRITE_THR (SerialDevice->PciIo, SerialDevice->BarIndex,Data);
          }
        }

        //
        // write the data out
        //
        if (!SerialDevice->HardwareFlowControl) {
          PciSerialFifoRemove (&SerialDevice->Transmit,&Data);
          WRITE_THR (SerialDevice->PciIo, SerialDevice->BarIndex,Data);
        }

        //
        // Make sure the transmit data will not be missed
        //
        if (SerialDevice->HardwareFlowControl) {
          //
          // Assert RTS
          //
          Mcr.Data = READ_MCR(SerialDevice->PciIo, SerialDevice->BarIndex);
          Mcr.Bits.RTS &= 0;
          WRITE_MCR(SerialDevice->PciIo, SerialDevice->BarIndex, Mcr.Data);
        }
      }
    } while (Lsr.Bits.THRE && !PciSerialFifoEmpty (&SerialDevice->Transmit));
  }
  return EFI_SUCCESS;
}

//
//Interface Functions
//

/**
  Reset serial device 
  
  @param[in] This  Pointer to EFI_SERIAL_IO_PROTOCOL   

  @retval EFI_SUCCESS         Reset successfully
  @retval EFI_DEVICE_ERROR    Failed to reset 

**/
EFI_STATUS
EFIAPI
PciSerialReset (
  IN EFI_SERIAL_IO_PROTOCOL  *This
  )
{
  EFI_STATUS      Status;
  SERIAL_DEV      *SerialDevice;
  SERIAL_PORT_LCR Lcr;
  SERIAL_PORT_IER Ier;
  SERIAL_PORT_MCR Mcr;
  SERIAL_PORT_FCR Fcr;
  EFI_TPL         Tpl;   

  SerialDevice = SERIAL_DEV_FROM_THIS (This);

  Tpl = gBS->RaiseTPL( TPL_NOTIFY );         

  //
  // Make sure DLAB is 0.
  //
  Lcr.Data = READ_LCR (SerialDevice->PciIo, SerialDevice->BarIndex);
  Lcr.Bits.DLAB = 0;
  WRITE_LCR(SerialDevice->PciIo, SerialDevice->BarIndex, Lcr.Data);
  
  //
  // Turn off all interrupts
  //
  Ier.Data = READ_IER(SerialDevice->PciIo, SerialDevice->BarIndex);
  Ier.Bits.RAVIE = 0;
  Ier.Bits.THEIE = 0;
  Ier.Bits.RIE   = 0;
  Ier.Bits.MIE   = 0;
  WRITE_IER(SerialDevice->PciIo, SerialDevice->BarIndex, Ier.Data); 

  //
  //Disable the FIFO.
  //
  Fcr.Bits.TRFIFOE = 0;
  WRITE_FCR(SerialDevice->PciIo, SerialDevice->BarIndex, Fcr.Data);

  //
  // Turn off loopback and disable device interrupt.
  //
  Mcr.Data = READ_MCR(SerialDevice->PciIo, SerialDevice->BarIndex);
  Mcr.Bits.OUT1 = 0;
  Mcr.Bits.OUT2 = 0;
  Mcr.Bits.LME  = 0;
  WRITE_MCR(SerialDevice->PciIo, SerialDevice->BarIndex, Mcr.Data);

  //
  // Clear the scratch pad register
  //
  WRITE_SCR(SerialDevice->PciIo, SerialDevice->BarIndex, 0);

  //
  // Go set the current attributes
  //
  Status = This->SetAttributes(
                   This,
                   This->Mode->BaudRate,
                   This->Mode->ReceiveFifoDepth,
                   This->Mode->Timeout,
                   This->Mode->Parity,
                   (UINT8)This->Mode->DataBits,
                   This->Mode->StopBits
                   );
  
  if (EFI_ERROR(Status)) {
    gBS->RestoreTPL( Tpl );
    return EFI_DEVICE_ERROR;
  }

  //
  // Go set the current control bits
  //
  Status = This->SetControl(
                   This,
                   This->Mode->ControlMask
                   );

  if (EFI_ERROR(Status)) {
    gBS->RestoreTPL( Tpl );
    return EFI_DEVICE_ERROR;
  }

  //
  // for 16550A enable FIFO, 16550 disable FIFO
  //
  Fcr.Bits.TRFIFOE = 1;
  Fcr.Bits.RESETRF = 1;
  Fcr.Bits.RESETTF = 1;  
  WRITE_FCR(SerialDevice->PciIo, SerialDevice->BarIndex, Fcr.Data);
  
  //
  // Reset the software FIFO
  //
  SerialDevice->Receive.First    = 0;
  SerialDevice->Receive.Last     = 0;
  SerialDevice->Receive.Surplus  = SERIAL_MAX_BUFFER_SIZE;
  SerialDevice->Transmit.First   = 0;
  SerialDevice->Transmit.Last    = 0;
  SerialDevice->Transmit.Surplus = SERIAL_MAX_BUFFER_SIZE;
  
  gBS->RestoreTPL( Tpl );

  //
  // Device reset is complete
  //
  return EFI_SUCCESS;
}

/**
  Set new attributes to a serial device 

  @param[in] This       Pointer to EFI_SERIAL_IO_PROTOCOL    
  @param[in] BaudRate   The baudrate of the serial device
  @param[in] ReceiveFifoDepth   
  @param[in] Timeout    The request timeout for a single char
  @param[in] Parity     The type of parity used in serial device
  @param[in] DataBits   Number of databits used in serial device
  @param[in] StopBits   Number of stopbits used in serial device

  @retval EFI_SUCCESS             The new attributes were set 
  @retval EFI_INVALID_PARAMETERS  One or more attributes have an unsupported value
  @retval EFI_UNSUPPORTED         Data Bits can not set to 5 or 6
  @retval EFI_DEVICE_ERROR        The serial device is not functioning correctly (no return)

**/
EFI_STATUS
EFIAPI
PciSerialSetAttributes (
  IN EFI_SERIAL_IO_PROTOCOL  *This,
  IN UINT64                  BaudRate,
  IN UINT32                  ReceiveFifoDepth,
  IN UINT32                  Timeout,
  IN EFI_PARITY_TYPE         Parity,
  IN UINT8                   DataBits,
  IN EFI_STOP_BITS_TYPE      StopBits
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

  Timeout *= 3;

  //
  // Check for default settings and fill in actual values.
  //
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
  
  //
  // 5 and 6 data bits can not be verified on a 16550A UART
  // Return EFI_INVALID_PARAMETER if an attempt is made to use these settings.
  //

  if ((DataBits == 5) || (DataBits == 6)) {
    return EFI_INVALID_PARAMETER;
  }
  
  //
  // Make sure all parameters are valid
  //
  
  if ((BaudRate > SERIAL_PORT_MAX_BAUD_RATE) || (BaudRate < SERIAL_PORT_MIN_BAUD_RATE)) { 
    return EFI_INVALID_PARAMETER;
  }
  
  //
  // 50,75,110,134,150,300,600,1200,1800,2000,2400,3600,4800,7200,9600,19200,
  // 38400,57600,115200
  //
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
  
  //
  //for DataBits = 6,7,8, StopBits can not set OneFiveStopBits
  //
  if ((DataBits >= 6) && (DataBits <=8) && (StopBits == OneFiveStopBits)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // See if the new attributes already match the current attributes
  //
  if (SerialDevice->UartDevicePath.BaudRate     == BaudRate         &&
      SerialDevice->UartDevicePath.DataBits     == DataBits         &&
      SerialDevice->UartDevicePath.Parity       == Parity           &&
      SerialDevice->UartDevicePath.StopBits     == StopBits         &&
      SerialDevice->SerialMode.ReceiveFifoDepth == ReceiveFifoDepth &&
      SerialDevice->SerialMode.Timeout          == Timeout             ) {
    return EFI_SUCCESS;
  }

  //
  // Compute divisor use to program the baud rate using a round determination
  //                 
  Divisor = (UINT32) DivU64x32Remainder (SERIAL_PORT_INPUT_CLOCK ,((UINT32)BaudRate * 16), &Remained);
  if ( Remained ) {
    Divisor += 1;
  }
  
  if ((Divisor == 0) || (Divisor & 0xffff0000)) {
    return EFI_INVALID_PARAMETER;
  }
  
  Tpl = gBS->RaiseTPL( TPL_NOTIFY );
  
  //
  // Compute the actual baud rate that the serial port will be programmed for.
  //
  BaudRate = SERIAL_PORT_INPUT_CLOCK / Divisor / 16;
  
  //
  // Put serial port on Divisor Latch Mode
  //
  Lcr.Data = READ_LCR(SerialDevice->PciIo, SerialDevice->BarIndex);
  Lcr.Bits.DLAB = 1;
  WRITE_LCR(SerialDevice->PciIo, SerialDevice->BarIndex, Lcr.Data);

  //
  // Write the divisor to the serial port
  //
  WRITE_DLL(SerialDevice->PciIo, SerialDevice->BarIndex, (UINT8)(Divisor & 0xff));
  WRITE_DLM(SerialDevice->PciIo, SerialDevice->BarIndex, (UINT8)((Divisor>>8) & 0xff));

  //
  // Put serial port back in normal mode and set remaining attributes.
  //
  Lcr.Bits.DLAB = 0;

  switch (Parity) {
    case NoParity:
      Lcr.Bits.PAREN   = 0;
      Lcr.Bits.EVENPAR = 0;
      Lcr.Bits.STICPAR = 0;
      break;
    case EvenParity:
      Lcr.Bits.PAREN   = 1;
      Lcr.Bits.EVENPAR = 1;
      Lcr.Bits.STICPAR = 0;
      break;
    case OddParity:
      Lcr.Bits.PAREN   = 1;
      Lcr.Bits.EVENPAR = 0;
      Lcr.Bits.STICPAR = 0;
      break;
    case SpaceParity:
      Lcr.Bits.PAREN   = 1;
      Lcr.Bits.EVENPAR = 1;
      Lcr.Bits.STICPAR = 1;
      break;
    case MarkParity:
      Lcr.Bits.PAREN   = 1;
      Lcr.Bits.EVENPAR = 0;
      Lcr.Bits.STICPAR = 1;
      break;
    default:
      break;
  }

  switch (StopBits) {
    case OneStopBit :
      Lcr.Bits.STOPB = 0;
      break;
    case OneFiveStopBits :
    case TwoStopBits :
      Lcr.Bits.STOPB = 1;
      break;
    default:
      break;
  }

  //
  //DataBits
  //
  Lcr.Bits.SERIALDB = (UINT8)((DataBits - 5) & 0x03);
  WRITE_LCR(SerialDevice->PciIo, SerialDevice->BarIndex, Lcr.Data);

  //
  //Set the Serial I/O mode
  //
  This->Mode->BaudRate         = BaudRate;
  This->Mode->ReceiveFifoDepth = ReceiveFifoDepth;
  This->Mode->Timeout          = Timeout;
  This->Mode->Parity           = Parity;
  This->Mode->DataBits         = DataBits;
  This->Mode->StopBits         = StopBits;

  //
  // See if Device Path Node has actually changed
  //
  if (SerialDevice->UartDevicePath.BaudRate     == BaudRate &&
      SerialDevice->UartDevicePath.DataBits     == DataBits &&
      SerialDevice->UartDevicePath.Parity       == Parity   &&
      SerialDevice->UartDevicePath.StopBits     == StopBits    ) {
    gBS->RestoreTPL( Tpl );
    return EFI_SUCCESS;
  }

  //
  // Update the device path
  //
  SerialDevice->UartDevicePath.BaudRate = BaudRate;
  SerialDevice->UartDevicePath.DataBits = DataBits;
  SerialDevice->UartDevicePath.Parity   = (UINT8)Parity;
  SerialDevice->UartDevicePath.StopBits = (UINT8)StopBits;

  NewDevicePath = AppendDevicePathNode (
                    SerialDevice->ParentDevicePath,
                    (EFI_DEVICE_PATH_PROTOCOL *)&SerialDevice->UartDevicePath
                    );
  if (NewDevicePath == NULL) {
    gBS->RestoreTPL( Tpl );
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
      gBS->RestoreTPL( Tpl );
      return Status;
    }
  }

  if (SerialDevice->DevicePath) {
    gBS->FreePool (SerialDevice->DevicePath);
  }
  SerialDevice->DevicePath = NewDevicePath;

  gBS->RestoreTPL( Tpl );

  return EFI_SUCCESS;
}

/**
  Set ControlBits 

  @param[in] This      Pointer to EFI_SERIAL_IO_PROTOCOL    
  @param[in] Control   Control bits that can be settable   

  @retval EFI_SUCCESS      New Control bits were set successfully
  @retval EFI_UNSUPPORTED  The Control bits wanted to set are not supported

**/
EFI_STATUS
EFIAPI
PciSerialSetControl (
  IN EFI_SERIAL_IO_PROTOCOL  *This,
  IN UINT32               Control
  )
{
  SERIAL_DEV      *SerialDevice;
  SERIAL_PORT_MCR Mcr;
  EFI_TPL         Tpl;

  //
  // The control bits that can be set are :
  //     EFI_SERIAL_DATA_TERMINAL_READY: 0x0001  // WO
  //     EFI_SERIAL_REQUEST_TO_SEND: 0x0002  // WO
  //     EFI_SERIAL_HARDWARE_LOOPBACK_ENABLE: 0x1000  // RW
  //     EFI_SERIAL_SOFTWARE_LOOPBACK_ENABLE: 0x2000  // RW
  //
  
  SerialDevice = SERIAL_DEV_FROM_THIS (This);

  //
  // first determine the parameter is invalid
  //
  if (Control & 0xffff8ffc) {
    return EFI_UNSUPPORTED;
  }
  
  Tpl = gBS->RaiseTPL( TPL_NOTIFY );
  
  Mcr.Data = READ_MCR(SerialDevice->PciIo, SerialDevice->BarIndex);
  Mcr.Bits.DTRC = 0;
  Mcr.Bits.RTS  = 0;
  Mcr.Bits.LME  = 0;
  SerialDevice->SoftwareLoopbackEnable = FALSE;
  SerialDevice->HardwareFlowControl = FALSE;

  if (Control & EFI_SERIAL_DATA_TERMINAL_READY) {
    Mcr.Bits.DTRC = 1;
  }
  
  if (Control & EFI_SERIAL_REQUEST_TO_SEND) {
    Mcr.Bits.RTS  = 1;
  }
  
  if (Control & EFI_SERIAL_HARDWARE_LOOPBACK_ENABLE) {
    Mcr.Bits.LME  = 1;
  }
  
  if (Control & EFI_SERIAL_HARDWARE_FLOW_CONTROL_ENABLE) {
    SerialDevice->HardwareFlowControl = TRUE;
  }
  
  WRITE_MCR(SerialDevice->PciIo, SerialDevice->BarIndex, Mcr.Data);
  
  if (Control & EFI_SERIAL_SOFTWARE_LOOPBACK_ENABLE) {
    SerialDevice->SoftwareLoopbackEnable = TRUE;
  }
  
  gBS->RestoreTPL( Tpl );
   
  return EFI_SUCCESS;
}

/**
  Get ControlBits 

  @param[in]    This          Pointer to EFI_SERIAL_IO_PROTOCOL   
  @param[in]    Control       Control signals of the serial device  
  
  @retval       EFI_SUCCESS   Get Control signals successfully

**/
EFI_STATUS
EFIAPI
PciSerialGetControl (
  IN EFI_SERIAL_IO_PROTOCOL  *This,
  OUT UINT32              *Control
  )
{
  SERIAL_DEV      *SerialDevice;
  SERIAL_PORT_MSR Msr;
  SERIAL_PORT_MCR Mcr;
  EFI_TPL         Tpl;
  
  Tpl = gBS->RaiseTPL( TPL_NOTIFY );

  SerialDevice = SERIAL_DEV_FROM_THIS (This);

  *Control = 0;

  //
  // Read the Modem Status Register
  //

  Msr.Data = READ_MSR(SerialDevice->PciIo, SerialDevice->BarIndex);

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

  //
  // Read the Modem Control Register
  //

  Mcr.Data = READ_MCR(SerialDevice->PciIo, SerialDevice->BarIndex);

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

  //
  // See if the Transmit FIFO is empty
  //
  PciSerialReceiveTransmit (SerialDevice);

  if (PciSerialFifoEmpty (&SerialDevice->Transmit)) {
    *Control |= EFI_SERIAL_OUTPUT_BUFFER_EMPTY;
  }

  //
  // See if the Receive FIFO is empty.
  //
  PciSerialReceiveTransmit (SerialDevice);

  if (PciSerialFifoEmpty (&SerialDevice->Receive)) {
    *Control |= EFI_SERIAL_INPUT_BUFFER_EMPTY;
  }

  if (SerialDevice->SoftwareLoopbackEnable) {
    *Control |= EFI_SERIAL_SOFTWARE_LOOPBACK_ENABLE;
  }
  
  gBS->RestoreTPL( Tpl );
  
  return EFI_SUCCESS;   
}

/**
  Write the specified number of bytes to serial device 

  @param[in]  EFI_SERIAL_IO_PROTOCOL    Pointer to EFI_SERIAL_IO_PROTOCOL    
  @param[in]  UINTN                     On input the size of Buffer, on output the amount of 
                                        data actually written
  @param[in]  VOID                      The buffer of data to write    


  @retval     EFI_SUCCESS               The data were written successfully
  @retval     EFI_DEVICE_ERROR          The device reported an error
  @retval     EFI_TIMEOUT               The write operation was stopped due to timeout

**/
EFI_STATUS
EFIAPI
PciSerialWrite (
  IN EFI_SERIAL_IO_PROTOCOL  *This,
  IN OUT UINTN               *BufferSize,
  IN VOID                    *Buffer
  )
{
  SERIAL_DEV *SerialDevice;
  UINT8      *CharBuffer;
  UINT32     Index;
  UINTN      Elapsed;
  UINTN      ActualWrite;
  EFI_TPL    Tpl;  
  
  SerialDevice = SERIAL_DEV_FROM_THIS (This);
  Elapsed = 0;
  ActualWrite = 0;
  
  if (*BufferSize == 0) {
    return EFI_SUCCESS;
  }
  
  if (!Buffer) {
    return EFI_DEVICE_ERROR;
  }
  
  Tpl = gBS->RaiseTPL( TPL_NOTIFY );
    
  CharBuffer = (UINT8 *)Buffer;
  
  for (Index = 0; Index < *BufferSize; Index ++) {
    PciSerialFifoAdd(&SerialDevice->Transmit, CharBuffer[Index]);
  
    while (PciSerialReceiveTransmit (SerialDevice) != EFI_SUCCESS ||
         PciSerialFifoEmpty (&SerialDevice->Transmit) == FALSE) {

      //
      //  Unsuccessful write so check if timeout has expired, if not,
      //  stall for a bit, increment time elapsed, and try again
      //
      
      if ( Elapsed >= This->Mode->Timeout ) {
        *BufferSize = ActualWrite ; 
        gBS->RestoreTPL( Tpl );       
        return EFI_TIMEOUT;
      }
      
      gBS->Stall( TIMEOUT_STALL_INTERVAL );
      
      Elapsed += TIMEOUT_STALL_INTERVAL;
    } //end while
    
    ActualWrite ++; 
    //
    //  Successful write so reset timeout
    //
    Elapsed = 0;
      
  } //end for
  
  gBS->RestoreTPL( Tpl );
  
  return EFI_SUCCESS;
}

/**
  Read the specified number of bytes from serial device 

  @param[in]  EFI_SERIAL_IO_PROTOCOL *  Pointer to EFI_SERIAL_IO_PROTOCOL    
  @param[in]  UINTN *                   On input the size of Buffer, on output the amount of 
                                        data returned in buffer
  @param[in]  VOID *                    The buffer to return the data into 

  @retval     EFI_SUCCESS:  The data were read successfully
  @retval     EFI_DEVICE_ERROR: The device reported an error
  @retval     EFI_TIMEOUT:  The read operation was stopped due to timeout


**/
EFI_STATUS
EFIAPI
PciSerialRead (
  IN EFI_SERIAL_IO_PROTOCOL  *This,
  IN OUT UINTN               *BufferSize,
  OUT VOID                   *Buffer
  )
{
  SERIAL_DEV      *SerialDevice;
  UINT32          Index;
  UINT8           *CharBuffer;
  UINTN           Elapsed; 
  EFI_STATUS      Status;
  EFI_TPL         Tpl;
  
  SerialDevice = SERIAL_DEV_FROM_THIS (This);
  Elapsed = 0;
  
  if (*BufferSize == 0) {
    return EFI_SUCCESS;
  }
  
  if (!Buffer) {
    return EFI_DEVICE_ERROR;
  }
  
  Tpl = gBS->RaiseTPL( TPL_NOTIFY );
  
  Status = PciSerialReceiveTransmit (SerialDevice);
  
  if (EFI_ERROR(Status)) {
    *BufferSize = 0;

    gBS->RestoreTPL( Tpl );          

    return EFI_DEVICE_ERROR;
  }
  
  CharBuffer = (UINT8 *)Buffer;
  for(Index=0;Index<*BufferSize;Index++) {
    while (PciSerialFifoRemove(&SerialDevice->Receive, &(CharBuffer[Index])) != EFI_SUCCESS) {

      //
      //  Unsuccessful read so check if timeout has expired, if not,
      //  stall for a bit, increment time elapsed, and try again
      //  Need this time out to get conspliter to work.
      //

      if ( Elapsed >= This->Mode->Timeout ) {
        *BufferSize = Index;
        gBS->RestoreTPL( Tpl );
        return EFI_TIMEOUT;
      }
      
      gBS->Stall( TIMEOUT_STALL_INTERVAL );            
      Elapsed += TIMEOUT_STALL_INTERVAL;
      
      Status = PciSerialReceiveTransmit (SerialDevice);  
      if (Status == EFI_DEVICE_ERROR) {
        *BufferSize = Index;
        gBS->RestoreTPL( Tpl );
        return EFI_DEVICE_ERROR;
      }
    }// end while

    //  Successful read so reset timeout
    Elapsed = 0;
  }// end for
  
  PciSerialReceiveTransmit (SerialDevice);
  
  gBS->RestoreTPL( Tpl );
  
  return EFI_SUCCESS;
}

/**
          
  Check pci serial port present or not.
   
  @param[in] SerialDevice  Serial Device

  @retval    TRUE          Serial port present.
  @retval    FALSE         Serial port not present

**/
BOOLEAN
PciSerialPortPresent(
  IN SERIAL_DEV *SerialDevice
  )
{
  SERIAL_PORT_FCR Fcr;
  UINT8           Reg_MCR;
  UINT8           Reg_LCR;
  UINT8           Temp;
  BOOLEAN         Status;
  
  
  Status  = TRUE;
  
  //
  // Save SCR reg
  //
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
  
  //
  // Restore SCR
  //
  WRITE_SCR (SerialDevice->PciIo, SerialDevice->BarIndex, Temp);

  //
  // Save MCR and LCR Regs
  //
  Reg_MCR = READ_MCR (SerialDevice->PciIo, SerialDevice->BarIndex);
  Reg_LCR = READ_LCR (SerialDevice->PciIo, SerialDevice->BarIndex);

  //
  // Set word length as 8 bits
  //
  Temp = Reg_LCR;
  Temp |= LCR_DATA_LENGTH;
  WRITE_LCR (SerialDevice->PciIo, SerialDevice->BarIndex, Temp);

  //
  // Enable and Clear FIFO
  //
  Fcr.Bits.TRFIFOE  = 1;
  Fcr.Bits.RESETRF  = 1;
  Fcr.Bits.RESETTF  = 1;
  WRITE_FCR (SerialDevice->PciIo, SerialDevice->BarIndex, Fcr.Data);

  //
  // Make sure it was serial IO space
  //
  WRITE_THR (SerialDevice->PciIo, SerialDevice->BarIndex, 0x55);
  gBS->Stall (SERIAL_ACCESS_STALL); //100 us
  if (READ_RBR (SerialDevice->PciIo, SerialDevice->BarIndex) != 0x55) {

    //
    // Enable LoopBack bit
    //
    Temp = Reg_MCR;
    Temp |= MCR_LOOP_BACK_EN;
    WRITE_MCR (SerialDevice->PciIo, SerialDevice->BarIndex, Temp);

    WRITE_THR (SerialDevice->PciIo, SerialDevice->BarIndex, 0x55);
    gBS->Stall (SERIAL_ACCESS_STALL); //100 us
    if (READ_RBR (SerialDevice->PciIo, SerialDevice->BarIndex) != 0x55)
      Status = FALSE;

    WRITE_THR (SerialDevice->PciIo, SerialDevice->BarIndex, 0xAA);
    gBS->Stall (SERIAL_ACCESS_STALL); //100 us
    if (READ_RBR (SerialDevice->PciIo, SerialDevice->BarIndex) != 0xAA)
      Status = FALSE;

  }

  //
  // Restore MCR and LCR Regs
  //
  WRITE_MCR (SerialDevice->PciIo, SerialDevice->BarIndex, Reg_MCR);
  WRITE_LCR (SerialDevice->PciIo, SerialDevice->BarIndex, Reg_LCR);
  return Status;
}

/**
          
  Providing PciIo read for serial register I/O macros       
  
  @param[in]  PciIo       A pointer to the EFI_PCI_IO_PROTOCOL instance.
  @param[in]  BarIndex    The BAR index of the standard PCI Configuration header to use as the
                          base address for the memory or I/O operation to perform.
  @param[in]  Offset      The offset within the selected BAR to start the memory or I/O operation.

  @retval    VOID

**/
UINT8 
PciSerialReadPort (
  IN EFI_PCI_IO_PROTOCOL   *PciIo,
  IN UINT16                BarIndex,
  IN UINT16                Offset
  )
{
  UINT8        Data;

  //
  // Use PciIo to access IO
  //
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
          
  Providing PciIo write for serial register I/O macros       
  
  @param[in]  PciIo       A pointer to the EFI_PCI_IO_PROTOCOL instance.
  @param[in]  BarIndex    The BAR index of the standard PCI Configuration header to use as the
                          base address for the memory or I/O operation to perform.
  @param[in]  Offset      The offset within the selected BAR to start the memory or I/O operation.
  @param[in]  Buffer      For read operations, the destination buffer to store the results. For write
                          operations, the source buffer to write data from.

  @retval    VOID

**/
VOID 
PciSerialWritePort (
  IN EFI_PCI_IO_PROTOCOL *PciIo,
  IN UINT16              BarIndex,
  IN UINT16              Offset,
  IN UINT8               Data
  )
{
  //
  // Use PciIo to access IO
  //
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
          
  Confirm baudrate of remote site and then update to SerailDevice  
          
  @param[in] SerialDevice  Serial Device

  @retval    VOID

**/
VOID
PciSerialFailSaveBaudRate (
  SERIAL_DEV        *SerialDevice
  )
{
  EFI_STATUS    Status;
  UINT8         Index;
  CHAR8         EscRetStatus[16];
  UINTN         StringLen;
  UINT64        OldBaudRate;

  EFI_DEVICE_PATH_PROTOCOL  *NewDevicePath;
  UART_DEVICE_PATH          OldUartDevPath;
  EFI_SERIAL_IO_PROTOCOL    *SerialIo;
  EFI_SERIAL_IO_MODE        *SerialMode;

  SerialIo = &SerialDevice->SerialIo;
  OldUartDevPath = SerialDevice->UartDevicePath;
  SerialMode = &SerialDevice->SerialMode;
  OldBaudRate = SerialMode->BaudRate;

  //
  // make the old baudrate as the first.
  //
  for (Index=0; Index < sizeof(BaudRate)/sizeof(UINT64); Index++){
    if (OldBaudRate == BaudRate[Index]) {
      BaudRate[Index] = BaudRate[0];
      BaudRate[0] = OldBaudRate;
    }
  }
  
  StringLen = AsciiStrLen(EscGetStatus);
  
  for (Index=0; Index < sizeof(BaudRate)/sizeof(UINT64); Index++) {
    SerialMode->BaudRate = BaudRate[Index];
    SerialDevice->UartDevicePath.BaudRate = BaudRate[Index];
    Status = SerialIo->Reset (SerialIo);

    StringLen = 4;
    Status = SerialIo->Write (SerialIo, &StringLen, EscGetStatus);
    gBS->Stall (100000);
    
    StringLen = 4;
    SerialIo->Read (SerialIo, &StringLen, EscRetStatus);
    if (CompareMem(EscRetStatus, "\x1b[0n",4) == 0) {
      if (CompareMem(&OldUartDevPath, &SerialDevice->UartDevicePath, sizeof(UART_DEVICE_PATH)) != 0) {

        //
        // Reset Terminal, Send out "<ESC>c"
        //
        StringLen = 2;
        Status = SerialIo->Write (SerialIo, &StringLen, EscReset);
        gBS->Stall (100000);
        
        NewDevicePath = AppendDevicePathNode (
                          SerialDevice->ParentDevicePath,
                          (EFI_DEVICE_PATH_PROTOCOL *) &SerialDevice->UartDevicePath
                          );
                          
        Status = gBS->ReinstallProtocolInterface (
                        SerialDevice->Handle,
                        &gEfiDevicePathProtocolGuid,
                        SerialDevice->DevicePath,
                        NewDevicePath
                        );
                        
        if (SerialDevice->DevicePath) {
          gBS->FreePool (SerialDevice->DevicePath);
        }
        
        SerialDevice->DevicePath = NewDevicePath;
                        
      }
      return;
    }
  }

  SerialMode->BaudRate = OldBaudRate;
  SerialIo->Reset (SerialIo);
}

