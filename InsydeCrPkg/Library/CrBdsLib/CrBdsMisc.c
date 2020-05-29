/** @file
  CrBdsLib.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/


#include <Library/CrBdsLib.h>

#define IsUartDevicePathType(a)      ( (a)->Type == MESSAGING_DEVICE_PATH )
#define IsUartDevicePathSubType(a)   ( (a)->SubType == MSG_UART_DP )
#define IsUartDevicePathSize(a)      ( DevicePathNodeLength(a) == sizeof(UART_DEVICE_PATH))
#define IsUartDevicePathNode(a)      ( IsUartDevicePathType(a) && IsUartDevicePathSubType(a) && IsUartDevicePathSize(a))

#define ISA_BRIDGE_BASE_CLASS   0x06
#define ISA_BRIDGE_SUB_CLASS    0x01
#define ISA_BRIDGE_INTERFACE    0x00

#define EISA_SERIAL_DEVICE_ID   0x0501

static EFI_DEVICE_PATH_PROTOCOL mEnd = {
  END_DEVICE_PATH_TYPE,
  END_ENTIRE_DEVICE_PATH_SUBTYPE,
  END_DEVICE_PATH_LENGTH,
  0
};


EFI_STATUS
CRLocateDeviceHandleBuffer (
  IN UINT8          Class,
  IN UINT8          SubClass,
  IN UINT8          Interface,
  OUT UINTN         *HandleCount,
  OUT EFI_HANDLE    **AHandleBuffer
  )
{
  EFI_STATUS            Status;
  EFI_HANDLE            *HandleBuffer;
  EFI_HANDLE            *PciHandleBuffer;
  UINTN                 NumberOfHandles;
  UINTN                 Index;
  EFI_PCI_IO_PROTOCOL   *PciIo;
  PCI_TYPE00            PciConfig;
  UINTN                 BufferSize;

  // check parameter
  if (HandleCount == NULL || AHandleBuffer == NULL)
    return EFI_INVALID_PARAMETER;

  // get all Pci device handle
  Status = gBS->LocateHandleBuffer ( ByProtocol, &gEfiPciIoProtocolGuid, NULL, &NumberOfHandles, &PciHandleBuffer);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  // Allocate memory for HandleBuffer
  BufferSize = NumberOfHandles * sizeof(EFI_HANDLE);
  Status = gBS->AllocatePool (EfiBootServicesData, BufferSize, (VOID **)&HandleBuffer);
  if (EFI_ERROR(Status)) {
    gBS->FreePool (PciHandleBuffer);
    return Status;
  }

  // Preset return value
  *HandleCount = 0;
  *AHandleBuffer = HandleBuffer;

  // Count device of match pass in
  for (Index = 0; Index < NumberOfHandles; Index++) {

    Status = gBS->HandleProtocol (PciHandleBuffer[Index], &gEfiPciIoProtocolGuid, (VOID **)&PciIo);
    if (EFI_ERROR(Status))
      continue;

    Status = PciIo->Pci.Read (PciIo, EfiPciIoWidthUint32, 0, sizeof(PCI_TYPE00)/sizeof(UINT32), &PciConfig);

    if ( EFI_ERROR(Status) ||
        (PciConfig.Hdr.ClassCode[2] != Class) ||
        (PciConfig.Hdr.ClassCode[1] != SubClass) ||
        (PciConfig.Hdr.ClassCode[0] != Interface))
      continue;

    *HandleBuffer = PciHandleBuffer[Index];
    HandleBuffer++;
    (*HandleCount)++;
  }

  gBS->FreePool (PciHandleBuffer);

  // Has no device
  if (*HandleCount == 0) {
    gBS->FreePool (HandleBuffer);
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}


/**
 Check whether a give device path is an ISA serial device with specified serial port number

 @param [in] DevicePath         The device path to be processed
 @param [in] SerialPort         The serial port number

 @return TRUE                   The device path is a serial device with the given serial port number
 @retval FALSE                  The device path is not a serial device with the given serial port number

**/
BOOLEAN
CRIsIsaSerialPortDevicePathInstance (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath,
  IN UINT8                     SerialPort
)
{
  ACPI_HID_DEVICE_PATH      *SerialDevPath;

  if (DevicePath == NULL)
    return FALSE;

  SerialDevPath = CRLocateIsaSerialDevicePathNode (DevicePath);

  if (SerialDevPath != NULL)
    if (SerialDevPath->UID == SerialPort)
      return TRUE;

  return FALSE;
}

/**
 Check whether a give device path is an ISA serial device

 @param [in] DevicePath         The device path to be processed

 @return TRUE                   The device path is an ISA serial device
 @retval FALSE                  The device path is not an ISA serial device

**/
BOOLEAN
CRIsIsaSerialDevicePathInstance (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  )
{
  ACPI_HID_DEVICE_PATH      *SerialDevPath;

  if (DevicePath == NULL)
    return FALSE;

  SerialDevPath = CRLocateIsaSerialDevicePathNode (DevicePath);

  if (SerialDevPath != NULL)
    return TRUE;

  return FALSE;
}

/**
 Check whether a give device path is a PCI serial device

 @param [in] DevicePath         The device path to be processed

 @return TRUE                   The device path is a PCI serial device
 @retval FALSE                  The device path is not a PCI serial device

**/
BOOLEAN
CRIsPciSerialDevicePathInstance (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL  *DevPath;
  PCI_DEVICE_PATH           *PciDevPath;

  if (DevicePath == NULL)
    return FALSE;

  DevPath = DevicePath;

  PciDevPath = (PCI_DEVICE_PATH *)CRLocateDevicePathNode (&DevPath, HARDWARE_DEVICE_PATH, HW_PCI_DP);

  while (PciDevPath != NULL) {

    if (DevPath->Type == MESSAGING_DEVICE_PATH &&
        DevPath->SubType == MSG_UART_DP)
      return TRUE;

    PciDevPath = (PCI_DEVICE_PATH *) CRLocateDevicePathNode (&DevPath, HARDWARE_DEVICE_PATH, HW_PCI_DP);
  }

  return FALSE;
}


BOOLEAN
CRIsUsbSerialDevicePathInstance (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL  *DevPath;
  USB_DEVICE_PATH           *UsbDevPath;

  if (DevicePath == NULL)
    return FALSE;

  DevPath = DevicePath;

  UsbDevPath = (USB_DEVICE_PATH *)CRLocateDevicePathNode (&DevPath, MESSAGING_DEVICE_PATH, MSG_USB_DP);

  while (UsbDevPath != NULL) {

    //
    // Confirm next node of  UsbDevPath was UART node
    //
    if (DevPath->Type == MESSAGING_DEVICE_PATH && 
        DevPath->SubType == MSG_UART_DP)
      return TRUE;

    UsbDevPath = (USB_DEVICE_PATH *) CRLocateDevicePathNode (&DevPath, MESSAGING_DEVICE_PATH, MSG_USB_DP);
  }

  return FALSE;
}


/**
 Get the ISA serial device path node for given device path

 @param [in] DevicePath         The device path to be processed

 @return  The retrieved ISA serial device path
 @retval NULL                   The ISA serial device path is not found

**/
ACPI_HID_DEVICE_PATH *
CRLocateIsaSerialDevicePathNode (
  IN EFI_DEVICE_PATH_PROTOCOL   *DevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL  *DevPath;
  ACPI_HID_DEVICE_PATH      *AcpiDevPath;

  if (DevicePath == NULL)
    return NULL;

  DevPath = DevicePath;

  AcpiDevPath = (ACPI_HID_DEVICE_PATH *) CRLocateDevicePathNode (&DevPath, ACPI_DEVICE_PATH, ACPI_DP);

  while (AcpiDevPath != NULL) {

    if (AcpiDevPath->HID == EISA_PNP_ID(EISA_SERIAL_DEVICE_ID))
      return AcpiDevPath;

    AcpiDevPath = (ACPI_HID_DEVICE_PATH *) CRLocateDevicePathNode (&DevPath, ACPI_DEVICE_PATH, ACPI_DP);
  }

  return NULL;
}


/**
 Extract the UART PCI device path from a given device path

 @param[in] Src                 The device path to be processed
 @param[out] DevicePath         The result UART PCI device path

 @retval EFI_SUCCESS            The UART PCI device path is successfully retrieved
 @retval others                 Failed to retrieved the UART PCI device path

**/
EFI_STATUS
CRExtractUartPciDevPath (
  IN EFI_DEVICE_PATH_PROTOCOL     *Src,
  OUT EFI_DEVICE_PATH_PROTOCOL    **DevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL    *PtrDevPath;
  PCI_DEVICE_PATH             *PciDevPath;
  EFI_DEVICE_PATH_PROTOCOL    TempNode;

  if (DevicePath == NULL)
    return EFI_INVALID_PARAMETER;

  PtrDevPath = Src;

  PciDevPath = (PCI_DEVICE_PATH *) CRLocateDevicePathNode (&PtrDevPath, HARDWARE_DEVICE_PATH, HW_PCI_DP);

  while (PciDevPath != NULL) {

    if (IsUartDevicePathNode(PtrDevPath)) {
      CopyMem (&TempNode, PtrDevPath, sizeof(EFI_DEVICE_PATH_PROTOCOL));
      CopyMem (PtrDevPath, &mEnd, sizeof(EFI_DEVICE_PATH_PROTOCOL));
      *DevicePath = DuplicateDevicePath(Src);
      CopyMem (PtrDevPath, &TempNode, sizeof(EFI_DEVICE_PATH_PROTOCOL));

      return EFI_SUCCESS;
    }
    PciDevPath = (PCI_DEVICE_PATH *) CRLocateDevicePathNode (&PtrDevPath, HARDWARE_DEVICE_PATH, HW_PCI_DP);
  }

  return EFI_NOT_FOUND;

}


/**
 Get the device path node for given device path type and device path sub-type

 @param [in] DevicePath         The device path to be processed
 @param [in] DevicePathType     The device path type
 @param [in] DevicePathSubType  The device path sub type

 @return  The retrieved device path node with the given DevicePathType and DevicePathSubType

**/
EFI_DEVICE_PATH_PROTOCOL *
CRLocateDevicePathNode(
  IN OUT EFI_DEVICE_PATH_PROTOCOL   **DevicePath,
  IN UINT8                          DevPathType,
  IN UINT8                          DevPathSubType
  )
{
  EFI_DEVICE_PATH_PROTOCOL  *DevPath;

  if (*DevicePath == NULL)
    return NULL;

  DevPath = *DevicePath;

  while (!IsDevicePathEnd(DevPath)) {
    if (DevPath->Type == DevPathType && DevPath->SubType == DevPathSubType) {
      *DevicePath = NextDevicePathNode (DevPath);
      return DevPath;
    }
    DevPath = NextDevicePathNode(DevPath);
  }

  *DevicePath = NULL;

  if (DevPathType == END_DEVICE_PATH_TYPE && DevPathSubType == END_ENTIRE_DEVICE_PATH_SUBTYPE)
    return DevPath;

  return NULL;

}


EFI_DEVICE_PATH_PROTOCOL  *
CRAppendMultiDevicePathNode (
  EFI_DEVICE_PATH_PROTOCOL  *SrcDevicePath,
  UINTN                     NumOfArgs,
  ...
  )
{
  VA_LIST                   Marker;
  UINTN                     Index;
  UINTN                     NewSize;
  UINTN                     SrcSize;
  EFI_DEVICE_PATH_PROTOCOL  *DevPathNode;
  EFI_DEVICE_PATH_PROTOCOL  *DevPathPtr;
  EFI_DEVICE_PATH_PROTOCOL  *NewDevicePath;

  SrcSize = GetDevicePathSize(SrcDevicePath);
  NewSize = SrcSize;

  VA_START (Marker, NumOfArgs);

  // Caculate the new device path size
  for (Index = 0; Index < NumOfArgs; Index++) {
    DevPathNode = VA_ARG (Marker, EFI_DEVICE_PATH_PROTOCOL *);
    NewSize += DevicePathNodeLength (DevPathNode);
  }

  VA_END(Marker);

  NewDevicePath = (EFI_DEVICE_PATH_PROTOCOL *) AllocateCopyPool (NewSize, SrcDevicePath);

  DevPathPtr = (EFI_DEVICE_PATH_PROTOCOL *)((UINTN)NewDevicePath + SrcSize - sizeof(EFI_DEVICE_PATH_PROTOCOL));

  VA_START (Marker, NumOfArgs);

  // Copy node to DevicePath
  for (Index = 0;  Index < NumOfArgs; Index++) {
    DevPathNode = VA_ARG (Marker, EFI_DEVICE_PATH_PROTOCOL *);
    CopyMem (DevPathPtr, DevPathNode, DevicePathNodeLength (DevPathNode));
    DevPathPtr = (EFI_DEVICE_PATH_PROTOCOL *)((UINT8*)DevPathPtr + DevicePathNodeLength (DevPathNode));
  }
  //
  // Copy End Node
  //
  CopyMem (DevPathPtr, &mEnd, sizeof(EFI_DEVICE_PATH_PROTOCOL));

  return NewDevicePath;

}


/**
 Compare two device path instances

 @param [in] DevicePath1        The first device path to be compared
 @param [in] DevicePath2        The second device path to be compared

 @return TRUE                   The two device paths are the same
 @retval FALSE                  The two device paths are not the same

**/
BOOLEAN
CREqualDevicePath (
  IN  EFI_DEVICE_PATH_PROTOCOL  *DevicePath1,
  IN  EFI_DEVICE_PATH_PROTOCOL  *DevicePath2
  )
{
  UINTN     DevicePathSize1;
  UINTN     DevicePathSize2;
  UINTN     Value;

  DevicePathSize1 = GetDevicePathSize (DevicePath1);
  DevicePathSize2 = GetDevicePathSize (DevicePath2);

  if (DevicePathSize1 != DevicePathSize2)
    return FALSE;

  if (DevicePathSize1 == 0)
    return TRUE;

  Value = CompareMem (DevicePath1, DevicePath2, DevicePathSize1);

  if (Value == 0)
    return TRUE;

  return FALSE;
}


/**
  Delete the instance in Multi which matches partly with Single instance

  @param  Multi                 A pointer to a multi-instance device path data
                                structure.
  @param  Single                A pointer to a single-instance device path data
                                structure.

  @return This function will remove the device path instances in Multi which partly
          match with the Single, and return the result device path. If there is no
          remaining device path as a result, this function will return NULL.

**/
EFI_DEVICE_PATH_PROTOCOL *
EFIAPI
CRBdsLibDelPartMatchInstance (
  IN     EFI_DEVICE_PATH_PROTOCOL  *Multi,
  IN     EFI_DEVICE_PATH_PROTOCOL  *Single
  )
{
  EFI_DEVICE_PATH_PROTOCOL  *Instance;
  EFI_DEVICE_PATH_PROTOCOL  *NewDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *TempNewDevicePath;
  UINTN                     InstanceSize;
  UINTN                     SingleDpSize;
  UINTN                     Size;

  NewDevicePath     = NULL;
  TempNewDevicePath = NULL;

  if (Multi == NULL || Single == NULL) {
    return Multi;
  }

  Instance        =  GetNextDevicePathInstance (&Multi, &InstanceSize);
  SingleDpSize    =  GetDevicePathSize (Single) - END_DEVICE_PATH_LENGTH;
  InstanceSize    -= END_DEVICE_PATH_LENGTH;

  while (Instance != NULL) {

    Size = (SingleDpSize < InstanceSize) ? SingleDpSize : InstanceSize;

    if ((CompareMem (Instance, Single, Size) != 0)) {
      //
      // Append the device path instance which does not match with Single
      //
      TempNewDevicePath = NewDevicePath;
      NewDevicePath = AppendDevicePathInstance (NewDevicePath, Instance);
      if (TempNewDevicePath != NULL) {
        FreePool(TempNewDevicePath);
      }
    }
    FreePool(Instance);
    Instance = GetNextDevicePathInstance (&Multi, &InstanceSize);
    InstanceSize  -= END_DEVICE_PATH_LENGTH;
  }

  return NewDevicePath;
}

/**
  Function compares a device path data structure to that of all the nodes of a
  second device path instance.

  @param  Multi                 A pointer to a multi-instance device path data
                                structure.
  @param  Single                A pointer to a single-instance device path data
                                structure.

  @retval TRUE                  If the Single device path is contained within Multi device path.
  @retval FALSE                 The Single device path is not match within Multi device path.

**/
BOOLEAN
EFIAPI
CRBdsLibMatchDevicePaths (
  IN  EFI_DEVICE_PATH_PROTOCOL  *Multi,
  IN  EFI_DEVICE_PATH_PROTOCOL  *Single
  )
{
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePathInst;
  UINTN                     Size;

  if (Multi == NULL || Single  == NULL) {
    return FALSE;
  }

  DevicePath      = Multi;
  DevicePathInst  = GetNextDevicePathInstance (&DevicePath, &Size);

  //
  // Search for the match of 'Single' in 'Multi'
  //
  while (DevicePathInst != NULL) {
    //
    // If the single device path is found in multiple device paths,
    // return success
    //
    if (CompareMem (Single, DevicePathInst, Size) == 0) {
      FreePool (DevicePathInst);
      return TRUE;
    }

    FreePool (DevicePathInst);
    DevicePathInst = GetNextDevicePathInstance (&DevicePath, &Size);
  }

  return FALSE;
}


/**
 Get COM port address and IRQ number for a given COM port number

 @param[in] ComPortNumber       The given COM port number
 @param[out] PortAddress        The port address of the specified COM port
 @param[out] IRQ                The IRQ address of the specified COM port

 @retval EFI_SUCCESS            The serial device resource is successfull retrieved
 @return others                 Failed to get the serial device resource

**/
EFI_STATUS
CRGetIsaSerialResource (
  IN UINT32   ComPortNumber,
  OUT UINT16  *PortAddress,
  OUT UINT8   *IRQ
  )
{
  EFI_STATUS                                Status;

  EFI_ISA_ACPI_PROTOCOL                   *IsaAcpi;
  EFI_ISA_ACPI_DEVICE_ID                  *IsaDevice;
  EFI_ISA_ACPI_RESOURCE_LIST              *ResourceList;

  UINTN                                     ResourceIndex;
  EFI_ISA_ACPI_RESOURCE                     *IoResource;
  EFI_ISA_ACPI_RESOURCE                     *InterruptResource;

  Status = gBS->LocateProtocol (&gEfiIsaAcpiProtocolGuid,
                                  NULL,
                                  (VOID **)&IsaAcpi
                                  );
  if (EFI_ERROR(Status)) {
    return EFI_UNSUPPORTED;
  }

  //
  // Extract UART Resource form IsaAcpi protocol
  //
  IsaDevice = NULL;
  do {
    Status = IsaAcpi->DeviceEnumerate (IsaAcpi, &IsaDevice);
    if (EFI_ERROR (Status)) {
      return EFI_UNSUPPORTED;
    }

    ResourceList = NULL;
    Status = IsaAcpi->GetCurResource (IsaAcpi, IsaDevice, &ResourceList);
    if (EFI_ERROR (Status)) {
      continue;
    }

    // Exam the device is a Uart device
    if ((ResourceList->Device.HID == EISA_PNP_ID(EISA_SERIAL_DEVICE_ID)) && (ResourceList->Device.UID == ComPortNumber)) {
      IoResource        = NULL;
      InterruptResource = NULL;
      for (ResourceIndex = 0;
           ResourceList->ResourceItem[ResourceIndex].Type != EfiIsaAcpiResourceEndOfList;
           ResourceIndex++
          ) {
        switch (ResourceList->ResourceItem[ResourceIndex].Type) {
        case EfiIsaAcpiResourceIo:
          IoResource = &ResourceList->ResourceItem[ResourceIndex];
          break;

        case EfiIsaAcpiResourceInterrupt:
          InterruptResource = &ResourceList->ResourceItem[ResourceIndex];
          break;
        }
      }

      *PortAddress = (UINT16)IoResource->StartRange;
      *IRQ = (UINT8)InterruptResource->StartRange;
      return EFI_SUCCESS;
    }

  } while (TRUE);
}


/**
 Get the serial port number of an ISA serial device from its device path

 @param[in] DevicePath          The device path to be processed
 @param[out] PortNumber         The result serial port number

 @retval EFI_SUCCESS            The serial port number is successfull retrieved
 @retval EFI_NOT_FOUND          Unable to find the serial port number

**/
EFI_STATUS
CRGetIsaSerialPortNumber (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath,
  OUT UINT32                   *PortNumber
  )
{
  ACPI_HID_DEVICE_PATH      *SerialDevPath;

  SerialDevPath = CRLocateIsaSerialDevicePathNode (DevicePath);

  if (SerialDevPath == NULL)
    return EFI_NOT_FOUND;

  *PortNumber = SerialDevPath->UID;

  return EFI_SUCCESS;

}

/**
 Get the Bus/Dev/Func of a PCI serial device from its device path

 @param[in] DevicePath          The device path to be processed
 @param[out] Bus                The Bus number of the PCI serial device
 @param[out] Dev                The Device number of the PCI serial device
 @param[out] Func               The Function number of the PCI serial device

 @retval EFI_SUCCESS            The device location is successfully retrieved
 @return others                 Failed to get the device location

**/
EFI_STATUS
CRGetPciDevLocation (
  EFI_DEVICE_PATH_PROTOCOL    *DevicePath,
  UINTN                       *Bus,
  UINTN                       *Dev,
  UINTN                       *Func
  )
{
  EFI_STATUS                  Status;
  EFI_PCI_IO_PROTOCOL         *PciIo;
  EFI_DEVICE_PATH_PROTOCOL    *PtrDevPath;
  EFI_DEVICE_PATH_PROTOCOL    *TempDevPath;
  EFI_HANDLE                  Handle;
  UINTN                       Segment;

  TempDevPath = DuplicateDevicePath(DevicePath);

  PtrDevPath = TempDevPath;
  Status = gBS->LocateDevicePath (&gEfiPciIoProtocolGuid, &PtrDevPath, &Handle);

  if (Status == EFI_SUCCESS && IsDevicePathEnd(PtrDevPath)) {
    Status = gBS->HandleProtocol (Handle, &gEfiPciIoProtocolGuid, (VOID **)&PciIo);
    PciIo->GetLocation ( PciIo, &Segment, Bus, Dev, Func);
  }

  gBS->FreePool (TempDevPath);
  return Status;
  
}



/**
 Get the Bus/Dev/Func of a PCI serial device from its device path

 @param[in] DevicePath          The device path to be processed
 @param[out] Bus                The Bus number of the PCI serial device
 @param[out] Dev                The Device number of the PCI serial device
 @param[out] Func               The Function number of the PCI serial device

 @retval EFI_SUCCESS            The device location is successfully retrieved
 @return others                 Failed to get the device location

**/
EFI_STATUS
CRGetPciSerialLocation (
  EFI_DEVICE_PATH_PROTOCOL    *DevicePath,
  UINTN                       *Bus,
  UINTN                       *Dev,
  UINTN                       *Func
  )
{
  EFI_STATUS                  Status;
  EFI_PCI_IO_PROTOCOL         *PciIo;
  EFI_DEVICE_PATH_PROTOCOL    *PtrDevPath;
  EFI_DEVICE_PATH_PROTOCOL    *TempDevPath;
  EFI_HANDLE                  Handle;
  UINTN                       Segment;

  Status = CRExtractUartPciDevPath(DevicePath, &TempDevPath);

  if (Status == EFI_SUCCESS) {

    PtrDevPath = TempDevPath;
    Status = gBS->LocateDevicePath (&gEfiPciIoProtocolGuid, &PtrDevPath, &Handle);

    if (Status == EFI_SUCCESS && IsDevicePathEnd(PtrDevPath)) {
      Status = gBS->HandleProtocol (Handle, &gEfiPciIoProtocolGuid, (VOID **)&PciIo);
      PciIo->GetLocation ( PciIo, &Segment, Bus, Dev, Func);
    }
    gBS->FreePool (TempDevPath);
    return EFI_SUCCESS;
  }

  return EFI_NOT_FOUND;
}

EFI_DEVICE_PATH_PROTOCOL *
CRGetPlatformISABridgeDevPath(
  VOID
  )
{
  EFI_DEVICE_PATH_PROTOCOL    *DevPath;
  EFI_PCI_IO_PROTOCOL         *PciIo;
  EFI_HANDLE                  *HandleBuffer;
  EFI_STATUS                  Status;
  UINT8                       ByteBuf[4];
  UINTN                       HandleCount;
  UINTN                       Index;

  //
  // Get Platform ISA Bridge Device Path
  //
  Status = gBS->LocateHandleBuffer (ByProtocol, &gEfiPciIoProtocolGuid, NULL, &HandleCount, &HandleBuffer);
  if (EFI_ERROR(Status))
    return NULL;

  for (Index=0; Index < HandleCount; Index++) {

    Status = gBS->HandleProtocol (HandleBuffer[Index], &gEfiPciIoProtocolGuid, (VOID **)&PciIo);
    if (EFI_ERROR(Status))
      continue;

    Status = PciIo->Pci.Read (
                        PciIo,
                        EfiPciIoWidthUint32,
                        (UINT32) PCI_REVISION_ID_OFFSET,
                        (UINTN) 1,
                        (VOID *) ByteBuf
                        );
    //
    // Looks for a ISA bridge BASECLASS / SUBCLASS / INTERFACE of 0x06 / 0x01 / 0x00
    //
    if((ByteBuf[3] == ISA_BRIDGE_BASE_CLASS) &&
       (ByteBuf[2] == ISA_BRIDGE_SUB_CLASS) &&
       (ByteBuf[1] == ISA_BRIDGE_INTERFACE)) {

      Status = gBS->HandleProtocol (HandleBuffer[Index], &gEfiDevicePathProtocolGuid, (VOID **)&DevPath);
      if (!EFI_ERROR(Status))
        return DevPath;
    }
  }

  return NULL;
}

/**
 Check whether a given device path is a Terminal device

 @param [in] DevicePath         The device path to be processed

 @return TRUE                   The device path is a Terminal device
 @retval FALSE                  The device path is not a Terminal device

**/
BOOLEAN
CRIsTerminalDevicePathInstance (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL  *DevPath;
  VENDOR_DEVICE_PATH        *Node;

  if (DevicePath == NULL)
    return FALSE;

  DevPath = DevicePath;

  Node = (VENDOR_DEVICE_PATH *)CRLocateDevicePathNode (&DevPath, MESSAGING_DEVICE_PATH, MSG_VENDOR_DP);

  while (Node != NULL) {

    if (CompareGuid (&Node->Guid, &gEfiPcAnsiGuid)) {
      return TRUE;
    } else if (CompareGuid (&Node->Guid, &gEfiVT100Guid)) {
      return TRUE;
    } else if (CompareGuid (&Node->Guid, &gEfiVT100PlusGuid)) {
      return TRUE;
    } else if (CompareGuid (&Node->Guid, &gEfiVTUTF8Guid)) {
      return TRUE;
    }
    
    Node = (VENDOR_DEVICE_PATH *) CRLocateDevicePathNode (&DevPath, MESSAGING_DEVICE_PATH, MSG_VENDOR_DP);
  }

  return FALSE;
}


