/** @file
  Console redirection implementation

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

#include "BdsCR.h"

#define IsUartDevicePathType(a)      ( (a)->Type == MESSAGING_DEVICE_PATH )
#define IsUartDevicePathSubType(a)   ( (a)->SubType == MSG_UART_DP )
#define IsUartDevicePathSize(a)      ( DevicePathNodeLength(a) == sizeof(UART_DEVICE_PATH))
#define IsUartDevicePathNode(a)      ( IsUartDevicePathType(a) && IsUartDevicePathSubType(a) && IsUartDevicePathSize(a))

#define EISA_SERIAL_DEVICE_ID    0x0501

typedef struct {
  EFI_HANDLE                  Handle;
  EFI_DEVICE_PATH_PROTOCOL    *DevicePath;
  UINT8                       DeviceType;
  UINTN                       Bus;
  UINTN                       Dev;
  UINTN                       Func;
  UINT16                      PortAddr;
  UINT16                      IRQ;
} TERMINAL_DEVICE;

typedef struct {
  EFI_CR_POLICY_PROTOCOL    *CRPolicy;
  UINT16                    DeviceCount;
  TERMINAL_DEVICE             Device[1];
} TERMINAL_DEVICE_MANAGER;

typedef enum {
  TDM_EQUAL,
  TDM_NOT_EQUAL,
  TDM_DEVICE_EXIST,
  TDM_DEVICE_NOT_EXIST
} TDM_STATUS;

EFI_CONSOLE_REDIRECTION_SERVICE_PROTOCOL  mCRService = { 
  NULL,
  DisconnectCrsTerminal
  };
  
EFI_GUID  mVt100Guid            = DEVICE_PATH_MESSAGING_VT_100;
EFI_GUID  mVt100PlusGuid        = DEVICE_PATH_MESSAGING_VT_100_PLUS;
EFI_GUID  mVt100Utf8Guid        = DEVICE_PATH_MESSAGING_VT_UTF8;
EFI_GUID  mPcAnsiGuid           = DEVICE_PATH_MESSAGING_PC_ANSI;

static UINT32 mCRBaudRateTable [] = {
  1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200
};

static EFI_GUID mCRTerminalGuidTable [] = {
  DEVICE_PATH_MESSAGING_VT_100,
  DEVICE_PATH_MESSAGING_VT_100_PLUS,
  DEVICE_PATH_MESSAGING_VT_UTF8,
  DEVICE_PATH_MESSAGING_PC_ANSI
};

static UART_DEVICE_PATH  mUartNode = {
  MESSAGING_DEVICE_PATH,
  MSG_UART_DP,
  (UINT8) (sizeof (UART_DEVICE_PATH)),
  (UINT8) ((sizeof (UART_DEVICE_PATH)) >> 8),
  0,
  115200,
  8,
  1,
  1
};

//static UART_FLOW_CONTROL_DEVICE_PATH mFlowControlNode = {
//  MESSAGING_DEVICE_PATH,
//  MSG_VENDOR_DP,
//  (UINT8) (sizeof (UART_FLOW_CONTROL_DEVICE_PATH)),
//  (UINT8) ((sizeof (UART_FLOW_CONTROL_DEVICE_PATH)) >> 8),
//  DEVICE_PATH_MESSAGING_UART_FLOW_CONTROL,
//  0
//};

static VENDOR_DEVICE_PATH mTerminalNode = {
  MESSAGING_DEVICE_PATH,
  MSG_VENDOR_DP,
  (UINT8) (sizeof (VENDOR_DEVICE_PATH)),
  (UINT8) ((sizeof (VENDOR_DEVICE_PATH)) >> 8),
  DEVICE_PATH_MESSAGING_PC_ANSI
};

static EFI_DEVICE_PATH_PROTOCOL mEnd = {
  END_DEVICE_PATH_TYPE,
  END_ENTIRE_DEVICE_PATH_SUBTYPE,
  END_DEVICE_PATH_LENGTH,
  0
};

PLATFORM_ISABRIDGE_DEVICE_PATH gPlatformIsaBridge = {
  // PciRootBridge
  gPciRootBridge,

  // IsaBridge
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    0,
    0x1f
  },

  // end
  gEndEntire
};

PLATFORM_TERMINAL_DEVICE_PATH gPlatformTerminallDevice = {
  // PciRootBridge
  gPciRootBridge,

  // IsaBridge
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    0,
    0x1f
  },

  // IsaSerial
  {
    ACPI_DEVICE_PATH,
    ACPI_DP,
    (UINT8) (sizeof (ACPI_HID_DEVICE_PATH)),
    (UINT8) ((sizeof (ACPI_HID_DEVICE_PATH)) >> 8),
    EISA_PNP_ID(0x0501),
    0
  },

  // Uart device
  {
    MESSAGING_DEVICE_PATH,
    MSG_UART_DP,
    (UINT8) (sizeof (UART_DEVICE_PATH)),
    (UINT8) ((sizeof (UART_DEVICE_PATH)) >> 8),
    0,
    115200,
    8,
    1,
    1
  },

  // Terminal device
  {
    MESSAGING_DEVICE_PATH,
    MSG_VENDOR_DP,
    (UINT8) (sizeof (VENDOR_DEVICE_PATH)),
    (UINT8) ((sizeof (VENDOR_DEVICE_PATH)) >> 8),
    DEVICE_PATH_MESSAGING_PC_ANSI
  },
  
  // end
  gEndEntire
};

EFI_DEVICE_PATH_PROTOCOL *
LocateDevicePathNode(
  IN OUT EFI_DEVICE_PATH_PROTOCOL   **DevicePath,
  IN UINT8                          DevPathType,
  IN UINT8                          DevPathSubType
  )
{
  EFI_DEVICE_PATH_PROTOCOL  *DevPath;

  if (*DevicePath == NULL) {
    return NULL;
  }

  DevPath = *DevicePath;

  while (!IsDevicePathEnd (DevPath)) {
    if (DevPath->Type == DevPathType && DevPath->SubType == DevPathSubType) {
      *DevicePath = NextDevicePathNode (DevPath);
      return DevPath;
    }
    DevPath = NextDevicePathNode(DevPath);
  }

  *DevicePath = NULL;

  if (DevPathType == END_DEVICE_PATH_TYPE && DevPathSubType == END_ENTIRE_DEVICE_PATH_SUBTYPE) {
    return DevPath;
  }

  return NULL;
}

ACPI_HID_DEVICE_PATH *
LocateIsaSerialDevicePathNode (
  IN EFI_DEVICE_PATH_PROTOCOL   *DevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL  *DevPath;
  ACPI_HID_DEVICE_PATH      *AcpiDevPath;

  if (DevicePath == NULL)
    return NULL;

  DevPath = DevicePath;

  AcpiDevPath = (ACPI_HID_DEVICE_PATH *) LocateDevicePathNode (&DevPath, ACPI_DEVICE_PATH, ACPI_DP);

  while (AcpiDevPath != NULL) {
    if (AcpiDevPath->HID == EISA_PNP_ID(EISA_SERIAL_DEVICE_ID))
      return AcpiDevPath;

    AcpiDevPath = (ACPI_HID_DEVICE_PATH *) LocateDevicePathNode (&DevPath, ACPI_DEVICE_PATH, ACPI_DP);
  }

  return NULL;
}

BOOLEAN
IsIsaSerialPortDevicePathInstance (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath,
  IN UINT8                     SerialPort
  )
{
  ACPI_HID_DEVICE_PATH      *SerialDevPath;

  if (DevicePath == NULL)
    return FALSE;

  SerialDevPath = LocateIsaSerialDevicePathNode (DevicePath);

  if (SerialDevPath != NULL)
    if (SerialDevPath->UID == SerialPort)
      return TRUE;

  return FALSE;
}


BOOLEAN
IsIsaSerialDevicePathInstance (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  )
{
  ACPI_HID_DEVICE_PATH      *SerialDevPath;

  if (DevicePath == NULL) {
    return FALSE;
  }

  SerialDevPath = LocateIsaSerialDevicePathNode (DevicePath);

  if (SerialDevPath != NULL) {
    return TRUE;
  }

  return FALSE;
}

BOOLEAN
IsPciSerialDevicePathInstance (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL  *DevPath;
  PCI_DEVICE_PATH           *PciDevPath;

  if (DevicePath == NULL)
    return FALSE;

  DevPath = DevicePath;

  PciDevPath = (PCI_DEVICE_PATH *)LocateDevicePathNode (&DevPath, HARDWARE_DEVICE_PATH, HW_PCI_DP);

  while (PciDevPath != NULL) {
    if (DevPath->Type == MESSAGING_DEVICE_PATH && 
        DevPath->SubType == MSG_UART_DP) {
      return TRUE;
    }

    PciDevPath = (PCI_DEVICE_PATH *) LocateDevicePathNode (&DevPath, HARDWARE_DEVICE_PATH, HW_PCI_DP);
  }

  return FALSE;
}

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

  if (DevicePathSize1 != DevicePathSize2) {
    return FALSE;
  }

  if (DevicePathSize1 == 0) {
    return TRUE;
  }

  Value = CompareMem (DevicePath1, DevicePath2, DevicePathSize1);

  if (Value == 0) {
    return TRUE;
  }

  return FALSE;
}

EFI_STATUS
CRUpdateConsoleVariable (
  IN  CHAR16                    *ConVarName,
  IN  EFI_DEVICE_PATH_PROTOCOL  *CustomizedConDevicePath,
  IN  EFI_DEVICE_PATH_PROTOCOL  *ExclusiveDevicePath
  )
{
  EFI_STATUS                Status;
  UINTN                     DevicePathSize;
  BOOLEAN                   VarChanged;
  EFI_DEVICE_PATH_PROTOCOL  *VarConsole;
  EFI_DEVICE_PATH_PROTOCOL  *OldVarConsole;
  EFI_DEVICE_PATH_PROTOCOL  *NewDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *PrevDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *VarConsoleInstance;
  EFI_DEVICE_PATH_PROTOCOL  *CustomizedInstance;


  VarChanged      = FALSE;
  VarConsole      = NULL;
  DevicePathSize  = 0;
  NewDevicePath   = NULL;
  Status          = EFI_UNSUPPORTED;

  if (CustomizedConDevicePath == ExclusiveDevicePath) {
    return EFI_UNSUPPORTED;
  }

  VarConsole = BdsLibGetVariableAndSize (
                 ConVarName,
                 &gEfiGenericVariableGuid,
                 &DevicePathSize
                 );

  OldVarConsole = VarConsole;

  if (ExclusiveDevicePath != NULL && VarConsole != NULL) {
    while (VarConsole != NULL) {
      VarConsoleInstance = GetNextDevicePathInstance (&VarConsole, &DevicePathSize);

      if (!BdsLibMatchDevicePaths (ExclusiveDevicePath, VarConsoleInstance)) {
        PrevDevicePath = NewDevicePath;
        NewDevicePath = AppendDevicePathInstance (PrevDevicePath, VarConsoleInstance);
        if  (PrevDevicePath != NULL)
          gBS->FreePool (PrevDevicePath);
      }
      else {
        VarChanged = TRUE;
      }
      gBS->FreePool (VarConsoleInstance);
    }
  } else {
    NewDevicePath = DuplicateDevicePath (VarConsole);
  }

  while (CustomizedConDevicePath != NULL) {
    CustomizedInstance = GetNextDevicePathInstance (&CustomizedConDevicePath, &DevicePathSize);

    if (!BdsLibMatchDevicePaths (NewDevicePath, CustomizedInstance)) {
      VarChanged = TRUE;
      PrevDevicePath = NewDevicePath;
      NewDevicePath = AppendDevicePathInstance (PrevDevicePath, CustomizedInstance);
      if (PrevDevicePath != NULL) {
        gBS->FreePool (PrevDevicePath);
      }
    }

    gBS->FreePool (CustomizedInstance);
  }

  if (OldVarConsole != NULL) {
    gBS->FreePool (OldVarConsole);
  }

  //
  // Update the variable of the default console
  //
  if (VarChanged) {
    UINT32    Attributes;
    CHAR16    *StringPtr;

    StringPtr = ConVarName;

    //
    // If the variable includes "Dev" at last, we consider
    // it does not support NV attribute.
    //
    while (*StringPtr != L'\0') {
      StringPtr++;
    }

    if ((((INTN)((UINTN)StringPtr - (UINTN)ConVarName) / sizeof (CHAR16)) > 3) && 
        (*(StringPtr - 3) == L'D') && 
        (*(StringPtr - 2) == L'e') && 
        (*(StringPtr - 1) == L'v')
       ) {
      Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS;
    } else {
      Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE;
    }

    gRT->SetVariable (
          ConVarName,
          &gEfiGenericVariableGuid,
          Attributes,
          GetDevicePathSize (NewDevicePath),
          NewDevicePath
          );

    return EFI_SUCCESS;
  }

  return EFI_ABORTED;
}

EFI_STATUS
GetIsaSerialResource (
  IN UINT32   ComPortNumber,
  OUT UINT16  *PortAddress,
  OUT UINT8   *IRQ
  )
{
  EFI_STATUS                                Status;
#ifdef  MULTI_SUPER_IO_SUPPORT
    EFI_MSIO_ISA_ACPI_PROTOCOL              *IsaAcpi;
    EFI_MSIO_ISA_ACPI_DEVICE_ID             *IsaDevice;
    EFI_MSIO_ISA_ACPI_RESOURCE_LIST         *ResourceList;
#else
    EFI_ISA_ACPI_PROTOCOL                   *IsaAcpi;
    EFI_ISA_ACPI_DEVICE_ID                  *IsaDevice;
    EFI_ISA_ACPI_RESOURCE_LIST              *ResourceList;
#endif
  UINTN                                     ResourceIndex;
  EFI_ISA_ACPI_RESOURCE                     *IoResource;
  EFI_ISA_ACPI_RESOURCE                     *InterruptResource;

#ifdef MULTI_SUPER_IO_SUPPORT
  Status = gBS->LocateProtocol (&gEfiMsioIsaAcpiProtocolGuid,
                                  NULL,
                                  (VOID **)&IsaAcpi
                                  );
#else
  Status = gBS->LocateProtocol (&gEfiIsaAcpiProtocolGuid,
                                  NULL,
                                  (VOID **)&IsaAcpi
                                  );
#endif
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


EFI_STATUS
GetIsaSerialPortNumber (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath,
  OUT UINT32                   *PortNumber
  )
{
  ACPI_HID_DEVICE_PATH      *SerialDevPath;

  SerialDevPath = LocateIsaSerialDevicePathNode (DevicePath);

  if (SerialDevPath == NULL) {
    return EFI_NOT_FOUND;
  }

  *PortNumber = SerialDevPath->UID;

  return EFI_SUCCESS;
}


EFI_DEVICE_PATH_PROTOCOL  *
AppendMultiDevicePathNode (
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

  // Copy End Node
  CopyMem (DevPathPtr, &mEnd, sizeof(EFI_DEVICE_PATH_PROTOCOL));

  return NewDevicePath;
}

EFI_STATUS
ExtractUartPciDevPath (
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
  PciDevPath = (PCI_DEVICE_PATH *) LocateDevicePathNode (&PtrDevPath, HARDWARE_DEVICE_PATH, HW_PCI_DP);

  while (PciDevPath != NULL) {
    if (IsUartDevicePathNode(PtrDevPath)) {
      CopyMem (&TempNode, PtrDevPath, sizeof(EFI_DEVICE_PATH_PROTOCOL));
      CopyMem (PtrDevPath, &mEnd, sizeof(EFI_DEVICE_PATH_PROTOCOL));
      *DevicePath = DuplicateDevicePath(Src);
      CopyMem (PtrDevPath, &TempNode, sizeof(EFI_DEVICE_PATH_PROTOCOL));

      return EFI_SUCCESS;
    }
    PciDevPath = (PCI_DEVICE_PATH *) LocateDevicePathNode (&PtrDevPath, HARDWARE_DEVICE_PATH, HW_PCI_DP);
  }

  return EFI_NOT_FOUND;
}

EFI_STATUS
GetPciSerialLocation (
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

  Status = ExtractUartPciDevPath(DevicePath, &TempDevPath);
  
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


EFI_STATUS
LocateDeviceHandleBuffer (
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

VOID
UpdateTerminalDevPath (
  PLATFORM_TERMINAL_DEVICE_PATH   *ADevicePath,
  UINT32                          UID,
  EFI_CR_POLICY_PROTOCOL          *CRPolicy
  )
{
  EFI_STATUS                  Status;
  UINTN                       Index;
  UINTN                       HandleCount;
  EFI_HANDLE                  *HandleBuffer;
  EFI_DEVICE_PATH_PROTOCOL    *DevicePath;
  UART_DEVICE_PATH            *UartDevPath;

  ADevicePath->Uart.BaudRate = mCRBaudRateTable[CRPolicy->CRBaudRate];
  ADevicePath->Uart.DataBits = CRPolicy->CRDataBits;
  ADevicePath->Uart.StopBits = CRPolicy->CRStopBits;
  ADevicePath->Uart.Parity = CRPolicy->CRParity;

  ADevicePath->Terminal.Guid = mCRTerminalGuidTable[CRPolicy->CRTerminalType];

  Status = gBS->LocateHandleBuffer (ByProtocol, &gEfiSerialIoProtocolGuid, NULL, &HandleCount, &HandleBuffer);
  if (EFI_ERROR(Status))
    return;

  for (Index=0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (HandleBuffer[Index], &gEfiDevicePathProtocolGuid, (VOID **)&DevicePath);
    if (EFI_ERROR(Status))
      continue;

    if (IsIsaSerialPortDevicePathInstance(DevicePath, (UINT8)UID)) {
      UartDevPath = (UART_DEVICE_PATH *) LocateDevicePathNode(&DevicePath, MESSAGING_DEVICE_PATH, MSG_UART_DP);
      if (UartDevPath != NULL) {
        ADevicePath->Uart.BaudRate = UartDevPath->BaudRate;
      }
    }
  }  
}

EFI_STATUS
CRConnectDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePathToConnect
  )
{
  EFI_STATUS                Status;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *CopyOfDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *Instance;
  EFI_DEVICE_PATH_PROTOCOL  *RemainingDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *Next;
  EFI_HANDLE                Handle;
  EFI_HANDLE                PreviousHandle;
  UINTN                     Size;

  if (DevicePathToConnect == NULL) {
    return EFI_SUCCESS;
  }

  DevicePath        = DuplicateDevicePath (DevicePathToConnect);
  CopyOfDevicePath  = DevicePath;
  if (DevicePath == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  do {
    Instance  = GetNextDevicePathInstance (&DevicePath, &Size);
    Next      = Instance;
    while (!IsDevicePathEndType (Next)) {
      Next = NextDevicePathNode (Next);
    }

    SetDevicePathEndNode (Next);

    PreviousHandle = NULL;
    do {
      RemainingDevicePath = Instance;
      Status              = gBS->LocateDevicePath (&gEfiDevicePathProtocolGuid, &RemainingDevicePath, &Handle);

      if (!EFI_ERROR (Status)) {
        if (Handle == PreviousHandle) {
          Status = EFI_NOT_FOUND;
        }

        if (!EFI_ERROR (Status)) {
          PreviousHandle = Handle;
          gBS->ConnectController (Handle, NULL, RemainingDevicePath, FALSE);
        }
      }
    } while (!EFI_ERROR (Status) && !IsDevicePathEnd (RemainingDevicePath));
  } while (DevicePath != NULL);

  if (CopyOfDevicePath != NULL) {
    gBS->FreePool (CopyOfDevicePath);
  }
  
  return Status;
}


EFI_STATUS
ConnectIsaTerminalDevice (
  EFI_CR_POLICY_PROTOCOL      *CRPolicy
  )
{
#ifdef  MULTI_SUPER_IO_SUPPORT
  EFI_MSIO_ISA_ACPI_PROTOCOL        *IsaAcpi;
  EFI_MSIO_ISA_ACPI_DEVICE_ID       *IsaDevice;
  EFI_MSIO_ISA_ACPI_RESOURCE_LIST   *ResourceList;
#else
  EFI_ISA_ACPI_PROTOCOL             *IsaAcpi;
  EFI_ISA_ACPI_DEVICE_ID            *IsaDevice;
  EFI_ISA_ACPI_RESOURCE_LIST        *ResourceList;
#endif
  EFI_STATUS                        Status;
  EFI_STATUS                        Result = EFI_UNSUPPORTED;
  BOOLEAN                           DoConnect;
  //UINTN                             Index;

  // Locate IsaAcpi protocol
#ifdef MULTI_SUPER_IO_SUPPORT
  Status = gBS->LocateProtocol (&gEfiMsioIsaAcpiProtocolGuid,
                                  NULL,
                                  (VOID **)&IsaAcpi
                                  );
#else
  Status = gBS->LocateProtocol (&gEfiIsaAcpiProtocolGuid,
                                  NULL,
                                  (VOID **)&IsaAcpi
                                  );
#endif
  if (EFI_ERROR(Status))
    return Status;

  // 
  // Do 2 phase connect.  The first phase connect serial device. Second phase connect with terminal
  // Serial driver maybe connect device with different Barurate.
  //
  //for (Index=0; Index < 2; Index++) {

  IsaDevice = NULL;
    
  // fine isa serial device
  do {
    Status = IsaAcpi->DeviceEnumerate (IsaAcpi, &IsaDevice);
    if (EFI_ERROR (Status)) {
      break;
    }

    // get isa device
    ResourceList = NULL;
    Status = IsaAcpi->GetCurResource (IsaAcpi, IsaDevice, &ResourceList);
    if (EFI_ERROR (Status)) {
      continue;
    }

    // connect isa serial device
    if (ResourceList->Device.HID == EISA_PNP_ID(EISA_SERIAL_DEVICE_ID)) {

      DoConnect = FALSE;
      
      if (CRPolicy->CRSerialPort == CR_ALL_PORTS){
        DoConnect = TRUE;
      }
      else if (ResourceList->Device.UID == CRPolicy->CRSerialPort){
        DoConnect = TRUE;
      }

      if(DoConnect == TRUE) {
        gPlatformTerminallDevice.IsaSerial.UID = ResourceList->Device.UID;
        UpdateTerminalDevPath(&gPlatformTerminallDevice, ResourceList->Device.UID, CRPolicy);
        Status = BdsLibConnectDevicePath((EFI_DEVICE_PATH_PROTOCOL *)&gPlatformTerminallDevice);
        if (Status == EFI_SUCCESS)
          Result = EFI_SUCCESS;
      }
    } // connect serial device
  } while (TRUE);
  
  return Result;
}

EFI_STATUS
ConnectPciTerminalDevice (
  EFI_CR_POLICY_PROTOCOL      *CRPolicy
  )
{
  EFI_STATUS                  Status;
  EFI_STATUS                  Result = EFI_NOT_FOUND;
  UINTN                       HandleCount;
  EFI_HANDLE                  *AHandleBuffer;
  UINTN                       Index;
  EFI_DEVICE_PATH_PROTOCOL    *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL    *TerminalDevPath;
  VENDOR_DEVICE_PATH          *SCUTermNode;

  Status = LocateDeviceHandleBuffer (PCI_CLASS_SCC, PCI_SUBCLASS_SERIAL, PCI_IF_16550, &HandleCount, &AHandleBuffer);
  if (EFI_ERROR(Status))
    return Status;

  SCUTermNode = AllocatePool (sizeof (VENDOR_DEVICE_PATH));
  CopyMem (SCUTermNode, &mTerminalNode, sizeof (VENDOR_DEVICE_PATH));
  CopyMem (&(SCUTermNode->Guid), &(mCRTerminalGuidTable[CRPolicy->CRTerminalType]), sizeof (EFI_GUID));
  
  //
  // connect all pci uart device
  //
  for (Index=0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol(AHandleBuffer[Index], &gEfiDevicePathProtocolGuid, (VOID **)&DevicePath);
    if (EFI_ERROR(Status))
      continue;

    TerminalDevPath = AppendMultiDevicePathNode(DevicePath, 2, &mUartNode, SCUTermNode);
    if (TerminalDevPath == NULL) {
      gBS->FreePool (AHandleBuffer);
      return EFI_OUT_OF_RESOURCES;
    }

    Status = BdsLibConnectDevicePath (TerminalDevPath);
    gBS->FreePool (TerminalDevPath);
    
    if (EFI_ERROR(Status))
      continue;

    Result = EFI_SUCCESS;
  }
  
  if (SCUTermNode != NULL) {
    gBS->FreePool (SCUTermNode);
  }
  gBS->FreePool (AHandleBuffer);
  
  return Result;
}

EFI_STATUS
ConnectTerminalDevice (
  EFI_CR_POLICY_PROTOCOL    *CRPolicy
  )
{
  EFI_STATUS    Status;
  EFI_STATUS    Result;
  UINT8         CRSerialPort;
  
  Result = EFI_NOT_FOUND;

  //
  // When use Isa serial port or All serial port to be console redirection that must
  // connect LPC first.
  //
  CRSerialPort = CRPolicy->CRSerialPort;

  //
  // Force LPC device connect with IsaAcpi Driver for get serial port resource
  //
  Status = BdsLibConnectDevicePath ((EFI_DEVICE_PATH_PROTOCOL *)&gPlatformIsaBridge);

  //
  // connect ISA serial device
  //
  if (CRSerialPort == CR_ALL_PORTS || CRSerialPort <= CR_ISA_SERIAL_PORTD) {
    Result = ConnectIsaTerminalDevice (CRPolicy);
  }

  // connect PCI serial device
  if (CRSerialPort >= CR_PCI_SERIAL_ONLY) {
    Status = ConnectPciTerminalDevice(CRPolicy);
    if (Status == EFI_SUCCESS)
      Result = EFI_SUCCESS;
  }

  return Result;
}

//
// Terminal Device Manager Method
//
BOOLEAN
TDM_VerifyConsoleVariable (
  IN TERMINAL_DEVICE_MANAGER    *TDManager,
  IN CHAR16                     *ConVarName
  )
{
  UINTN                       Index;
  UINTN                       VarSize;
  EFI_DEVICE_PATH_PROTOCOL    *VarConsole;
  
  VarConsole = BdsLibGetVariableAndSize (
                 ConVarName,
                 &gEfiGenericVariableGuid,
                 &VarSize
                 );
  
  for (Index=0; Index < TDManager->DeviceCount; Index++) {    
    if (BdsLibMatchDevicePaths (VarConsole, TDManager->Device[Index].DevicePath) == FALSE)
      return FALSE;
  }

  return TRUE;
}

EFI_CONSOLE_REDIRECTION_INFO *
TDM_BuildCRInfo (
  IN TERMINAL_DEVICE_MANAGER    *TDManager,
  IN BOOLEAN                    Headless
  )
{
  UINTN                           Size;
  EFI_CONSOLE_REDIRECTION_INFO    *CRInfo;
  UINTN                           Index;
  TERMINAL_DEVICE                 *Device;
  CR_ISA_SERIAL_DEVICE            *IsaSerial;
  CR_PCI_SERIAL_DEVICE            *PciSerial;
  EFI_DEVICE_PATH_PROTOCOL        *DevPath;
  UART_DEVICE_PATH                *UartDevPath;

  if (TDManager->DeviceCount == 0)
    return NULL;

  // caculate the CRInfo size
  Size = sizeof(EFI_CONSOLE_REDIRECTION_INFO) + ((TDManager->DeviceCount-1) * sizeof (EFI_CONSOLE_REDIRECTION_DEVICE));

  // allocate memory for CRInfo
  CRInfo = AllocateZeroPool (Size);
  if (CRInfo == NULL) {
    return NULL;
  }

  CRInfo->Headless = Headless;
  CRInfo->DeviceCount = (UINT8) TDManager->DeviceCount;

  //
  // Fill out CRDevice array
  //
  for (Index=0; Index < TDManager->DeviceCount; Index++) {

    Device = &TDManager->Device[Index];
    CRInfo->CRDevice[Index].DevicePath = Device->DevicePath;

    // set BaudRateDivisor
    DevPath = Device->DevicePath;
    UartDevPath = (UART_DEVICE_PATH *) LocateDevicePathNode(&DevPath, MESSAGING_DEVICE_PATH, MSG_UART_DP);
    CRInfo->CRDevice[Index].BaudRateDivisor = (UINT8)(115200/UartDevPath->BaudRate);

    // Isa serial device
    if (Device->DeviceType == ISA_SERIAL_DEVICE) {
      IsaSerial = &CRInfo->CRDevice[Index].Device.IsaSerial;
      CRInfo->CRDevice[Index].Type = ISA_SERIAL_DEVICE;
      //IsaSerial->PortNumber = Device->Port;
      IsaSerial->ComPortAddress = Device->PortAddr;
      IsaSerial->ComPortIrq = (UINT8) Device->IRQ;
    }
    // Pci serial device
    else {
      PciSerial = &CRInfo->CRDevice[Index].Device.PciSerial;
      CRInfo->CRDevice[Index].Type = PCI_SERIAL_DEVICE;
      PciSerial->Bus = (UINT8) Device->Bus;
      PciSerial->Device = (UINT8) Device->Dev;
      PciSerial->Function = (UINT8) Device->Func;
    }
  }

  return CRInfo;
}

EFI_DEVICE_PATH_PROTOCOL *
TDM_BuildCRDevicePath (
  TERMINAL_DEVICE_MANAGER   *TDManager
  )
{
  UINTN                       Index;
  EFI_DEVICE_PATH_PROTOCOL    *DevPath = NULL;
  EFI_DEVICE_PATH_PROTOCOL    *PrevDevPath = NULL;
  
  for (Index=0; Index < TDManager->DeviceCount; Index++) {  
    DevPath = AppendDevicePathInstance (PrevDevPath, TDManager->Device[Index].DevicePath);
    
    if (PrevDevPath != NULL) {
      gBS->FreePool (PrevDevPath);
    }

    PrevDevPath = DevPath;
  }

  return DevPath;
}

EFI_STATUS
TDM_UpdateConVar (
  IN TERMINAL_DEVICE_MANAGER          *TDManager
  )
{
  EFI_STATUS                  Status;
  UINTN                       VarSize;
  //EFI_DEVICE_PATH_PROTOCOL    *SelectedSerialDevicePaths;
  EFI_DEVICE_PATH_PROTOCOL    *CRDevicePath;
  EFI_DEVICE_PATH_PROTOCOL    *NewCRDevicePath;

  // Get CRDevicePath from Variable;
  CRDevicePath = BdsLibGetVariableAndSize (L"CRDevicePath", &gEfiGenericVariableGuid, &VarSize);

  // C.R. is disabled.
  if (TDManager->DeviceCount == 0) {

    //   Delete C.R. device path and Variable
    if(CRDevicePath != NULL) {
      CRUpdateConsoleVariable (CON_IN_CANDIDATE_NAME, NULL, CRDevicePath);
      CRUpdateConsoleVariable (CON_OUT_CANDIDATE_NAME, NULL, CRDevicePath);

      // Delete TDManager;
      VarSize = 0;
      Status = gRT->SetVariable (
                      L"CRDevicePath",
                      &gEfiGenericVariableGuid,
                      EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                      VarSize,
                      CRDevicePath
                      );
    }

  }
  // C.R. is enabled
  else {
    NewCRDevicePath = TDM_BuildCRDevicePath (TDManager);

    // C.R. device is not change.  Just add-in.
    if (CREqualDevicePath(NewCRDevicePath, CRDevicePath) == TRUE) {

      // Update ConIn, ConOut variable
      CRUpdateConsoleVariable (CON_IN_CANDIDATE_NAME, NewCRDevicePath, NULL);
      CRUpdateConsoleVariable (CON_OUT_CANDIDATE_NAME, NewCRDevicePath, NULL);
    }

    // C.R. device is changed. Do add-in and remove.
    else {
    
      // Update ConIn, ConOut variable
      CRUpdateConsoleVariable (CON_IN_CANDIDATE_NAME, NewCRDevicePath, CRDevicePath);
      CRUpdateConsoleVariable (CON_OUT_CANDIDATE_NAME, NewCRDevicePath, CRDevicePath);
      
      // Set CRDevicePath;
      VarSize = GetDevicePathSize (NewCRDevicePath),
      Status = gRT->SetVariable (
                      L"CRDevicePath",
                      &gEfiGenericVariableGuid,
                      EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                      VarSize,
                      NewCRDevicePath
                      );
    }
                  
    gBS->FreePool (NewCRDevicePath);
  }

  return EFI_SUCCESS;
}


TERMINAL_DEVICE_MANAGER *
TDM_Create ( 
  EFI_CR_POLICY_PROTOCOL  *CRPolicy
  )
{
  EFI_STATUS                  Status;
  UINTN                       BufferSize;
  UINTN                       HandleCount;
  EFI_HANDLE                  *HandleBuffer;
  TERMINAL_DEVICE_MANAGER     *TDManager;
  UINTN                       Index;
  UINTN                       DevCount;
  EFI_DEVICE_PATH_PROTOCOL    *DevicePath;
  BOOLEAN                     DoAddDevice;
  UINT8                       DeviceType;
  UINT32                      PortNumber;
  UINT16                      PortAddr;
  UINT8                       IRQ;
  UINTN                       Bus, Dev, Func;

  // locate all Terminal handle
  Status = gBS->LocateHandleBuffer (ByProtocol, &gEfiSimpleTextOutProtocolGuid, NULL, &HandleCount, &HandleBuffer);

  // Has not Terminal  or CR disable, We still make Empty TDM object
  if (EFI_ERROR(Status) || CRPolicy->CREnable == FALSE) {
    BufferSize = sizeof(TERMINAL_DEVICE_MANAGER);
    Status = gBS->AllocatePool (EfiBootServicesData, BufferSize, (VOID **)&TDManager);
    if (EFI_ERROR(Status))
      return NULL;

    TDManager->CRPolicy = CRPolicy;
    TDManager->DeviceCount = 0;
    
    return TDManager;
  }
  
  // allocate memory for TDManager
  BufferSize = sizeof(TERMINAL_DEVICE_MANAGER) + sizeof(TERMINAL_DEVICE) * (HandleCount - 1);
  Status = gBS->AllocatePool (EfiBootServicesData, BufferSize, (VOID **)&TDManager);
  if (EFI_ERROR(Status))
    return NULL;

  // set CRPolicy
  TDManager->CRPolicy = CRPolicy;

  // set each SerialDevice
  for (DevCount=0, Index=0; Index < HandleCount; Index++) {

    Status = gBS->HandleProtocol (HandleBuffer[Index], &gEfiDevicePathProtocolGuid, (VOID **)&DevicePath);
    if (Status != EFI_SUCCESS)
      continue;

    // detect device type
    if (IsIsaSerialDevicePathInstance(DevicePath))
      DeviceType = ISA_SERIAL_DEVICE;
    else if (IsPciSerialDevicePathInstance(DevicePath))
      DeviceType = PCI_SERIAL_DEVICE;
    else {
      continue;
    }
    
    // reset DoAddDevice flag
    DoAddDevice = FALSE;

    // deside add or not add the device
    if (CRPolicy->CRSerialPort == CR_ALL_PORTS) {
      DoAddDevice = TRUE;
    }
    else if (CRPolicy->CRSerialPort == CR_PCI_SERIAL_ONLY && DeviceType == PCI_SERIAL_DEVICE){
      DoAddDevice = TRUE;
    }
    else if (CRPolicy->CRSerialPort <= CR_ISA_SERIAL_PORTD && DeviceType == ISA_SERIAL_DEVICE) {
      if (IsIsaSerialPortDevicePathInstance(DevicePath, CRPolicy->CRSerialPort))
        DoAddDevice = TRUE;
    }

    // add the serial device
    if (DoAddDevice == TRUE) {  
      TDManager->Device[DevCount].Handle = HandleBuffer[Index];
      TDManager->Device[DevCount].DevicePath = DevicePath;
      TDManager->Device[DevCount].DeviceType = DeviceType;
      
      if (DeviceType == ISA_SERIAL_DEVICE) {
        GetIsaSerialPortNumber (DevicePath, &PortNumber);
        GetIsaSerialResource(PortNumber, &PortAddr, &IRQ);
        TDManager->Device[DevCount].PortAddr = PortAddr;
        TDManager->Device[DevCount].IRQ = IRQ;
        DevCount++;
      }
      else if (DeviceType == PCI_SERIAL_DEVICE) {
        Status = GetPciSerialLocation (DevicePath, &Bus, &Dev, &Func);
        if (Status == EFI_SUCCESS) {
          TDManager->Device[DevCount].Bus = Bus;
          TDManager->Device[DevCount].Dev = Dev;
          TDManager->Device[DevCount].Func = Func;
          DevCount++;
        }
      }
    }
  }

  TDManager->DeviceCount = (UINT16) DevCount;
  
  return TDManager;
}

VOID
TDM_Destory (
  TERMINAL_DEVICE_MANAGER   *TDManager
  )
{
  if (TDManager != NULL) {
    gBS->FreePool (TDManager);
  }
}


/**
  Disconnect console redirection terminal controller.

  @param  This              A pointer to the EFI_CONSOLE_REDIRECTION_SERVICE_PROTOCOL instance.
  @param  TerminalNumber    Specify the terminal index that will be disconnect. The first index number is 0.

  @retval EFI_STATUS        

**/
EFI_STATUS
DisconnectCrsTerminal (
  EFI_CONSOLE_REDIRECTION_SERVICE_PROTOCOL  *This,
  UINTN                                     TerminalNumber
  )  
{
  EFI_STATUS                      Status;
  EFI_CONSOLE_REDIRECTION_INFO    *CRInfo;
  EFI_DEVICE_PATH_PROTOCOL        *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL        *NewDevicePath;
  EFI_DEVICE_PATH_PROTOCOL        *CRDevicePathInstance;
  UINTN                           Size;
  EFI_HANDLE                      ControllerHandle;
  
  CRInfo = This->CRInfo;
  
  if (TerminalNumber >= CRInfo->DeviceCount)
    return EFI_INVALID_PARAMETER;

  DevicePath = CRInfo->CRDevice[TerminalNumber].DevicePath;
  
  NewDevicePath = GetNextDevicePathInstance(&DevicePath, &Size);

  CRDevicePathInstance = NewDevicePath;
  
  Status = gBS->LocateDevicePath (&gEfiDevicePathProtocolGuid , &CRDevicePathInstance, &ControllerHandle);

  if (!EFI_ERROR (Status) && IsDevicePathEnd (CRDevicePathInstance)) {
    Status = gBS->DisconnectController (ControllerHandle, NULL, NULL);
    gBS->FreePool (NewDevicePath);
  }
  return Status;
}

EFI_STATUS
PlatformBDSConnectCRTerminal (
  IN BOOLEAN     Headless
  )
{
  EFI_STATUS                      Status;
  EFI_CONSOLE_REDIRECTION_INFO    *CRInfo = NULL;
  EFI_CR_POLICY_PROTOCOL          *CRPolicy;
  TERMINAL_DEVICE_MANAGER         *TDManager;


  // Get Console Redirection Policy
  Status = gBS->LocateProtocol (&gEfiCRPolicyProtocolGuid, NULL, (VOID **)&CRPolicy);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  // if headless Force console redireciton enable.
  if (Headless) {
    CRPolicy->CREnable = CR_ENABLE;
//[-start-130618-IB05400417-add]//
    PcdSetBool (PcdHeadlessSystem, TRUE);
//[-end-130618-IB05400417-add]//
  }

  // connect C.R. terminal device
  if (CRPolicy->CREnable == CR_ENABLE) {
    Status = ConnectTerminalDevice (CRPolicy);
  }

  // create Terminal device manager
  TDManager = TDM_Create(CRPolicy);

  if (TDManager == NULL) {
    ASSERT(FALSE);
    return EFI_OUT_OF_RESOURCES;
  }

  if (TDManager->DeviceCount != 0) {
    // Build CRInfo structure
    CRInfo = TDM_BuildCRInfo (TDManager, Headless);
  }
    
  //  Set ConIn, ConOut and ConsoleRedirection Variable
  TDM_UpdateConVar (TDManager);
    
  //
  // Install Console Redirection Information
  //
  if (CRInfo != NULL) {
    mCRService.CRInfo = CRInfo;
    gBS->InstallProtocolInterface (
           &gImageHandle,
           &gConsoleRedirectionServiceProtocolGuid,
           EFI_NATIVE_INTERFACE,
           &mCRService
           );                        
  }

  // Free resource that we allocated.
  TDM_Destory (TDManager);
  
  return EFI_SUCCESS;
}

//
// This is a fack function that is for compiler not report error when console_redirection_support is off.
// Reserve it, don't remove.
//
void FakeFunction (
  void
  )
{
}

