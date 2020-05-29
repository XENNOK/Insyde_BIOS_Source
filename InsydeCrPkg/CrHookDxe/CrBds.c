/** @file
  CrHookDxe driver for BDS phase module
  
;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
#include "CrBds.h"

#include <IndustryStandard/Pci.h>

#include <Guid/GlobalVariable.h>
#include <Guid/DebugMask.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/DevicePathLib.h>
#include <Library/DebugLib.h>
#include <Library/VariableLib.h>
#include <Library/CrVfrConfigLib.h>
#include <Library/CrBdsLib.h>

#include <Protocol/DevicePath.h>
#include <Protocol/TerminalEscCode.h>
#include <Protocol/CRPolicy.h>
#include <Protocol/PciIo.h>
#include <Protocol/SerialIo.h>
#include <Protocol/IsaAcpi.h>
#include <Protocol/UsbIo.h>
#include <Protocol/UsbSerialControllerIo.h>
#include <Protocol/CrSrvManager.h>


#define EISA_SERIAL_DEVICE_ID   0x0501

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
  TERMINAL_DEVICE           Device[1];
} TERMINAL_DEVICE_MANAGER;

EFI_CONSOLE_REDIRECTION_SERVICE_PROTOCOL  mCrService = {
  NULL,
  DisconnectCrTerminal
  };

EFI_CONSOLE_REDIRECTION_BDS_HOOK_PROTOCOL  mCrBdscr = {
  PlatformBdsConnectCrTerminal
};

CHAR16    *mConInVarName;
CHAR16    *mConOutVarName;
EFI_GUID  *mConInVarGuid;
EFI_GUID  *mConOutVarGuid;

EFI_GUID  mVt100Guid            = DEVICE_PATH_MESSAGING_VT_100;
EFI_GUID  mVt100PlusGuid        = DEVICE_PATH_MESSAGING_VT_100_PLUS;
EFI_GUID  mVt100Utf8Guid        = DEVICE_PATH_MESSAGING_VT_UTF8;
EFI_GUID  mPcAnsiGuid           = DEVICE_PATH_MESSAGING_PC_ANSI;

static UINT32 mCrBaudRateTable [] = {
  1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200
};

static EFI_GUID mCrTerminalGuidTable [] = {
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

static VENDOR_DEVICE_PATH mTerminalNode = {
  MESSAGING_DEVICE_PATH,
  MSG_VENDOR_DP,
  (UINT8) (sizeof (VENDOR_DEVICE_PATH)),
  (UINT8) ((sizeof (VENDOR_DEVICE_PATH)) >> 8),
  DEVICE_PATH_MESSAGING_PC_ANSI
};

EFI_DEVICE_PATH_PROTOCOL *gPlatformTerminallDevice;

static ACPI_HID_DEVICE_PATH mIsaSerialNode = {
  ACPI_DEVICE_PATH,
  ACPI_DP,
  (UINT8) (sizeof (ACPI_HID_DEVICE_PATH)),
  (UINT8) ((sizeof (ACPI_HID_DEVICE_PATH)) >> 8),
  EISA_PNP_ID(0x0501),
  0
};

static USB_DEVICE_PATH mUsbNode = {
  MESSAGING_DEVICE_PATH,
  MSG_USB_DP,
  (UINT8) (sizeof (USB_DEVICE_PATH)),
  (UINT8) ((sizeof (USB_DEVICE_PATH)) >> 8),
  1,
  0
};

/**

  Based on Terminal Device Manager to remove unsupport CR feature terminal
  device from specific variable

  @param        TDManager            Pointer to Terminal Device Manager structure
  @param        VarName              The Name of specific Variable  
  @param        VarGuid              The Guid of specific Variable
  
  @retval       EFI_SUCCESS          Variable update success
  @retval       others               Variable update fail

**/
EFI_STATUS
RemoveNonCrTerminalDevFromVariable (
  IN  TERMINAL_DEVICE_MANAGER   *TDManager,
  IN  CHAR16                    *VarName,
  IN  EFI_GUID                  *VarGuid
  )
{
  EFI_DEVICE_PATH_PROTOCOL    *VarConsole;
  EFI_DEVICE_PATH_PROTOCOL    *CopyOfDevicePath;
  EFI_DEVICE_PATH_PROTOCOL    *Instance;
  EFI_DEVICE_PATH_PROTOCOL    *Next;  
  UINTN                       Size;
  UINTN                       Index;
  BOOLEAN                     RemoveDevPath;

  VarConsole = CommonGetVariableData (VarName, VarGuid);
  if (VarConsole != NULL) {
    
    CopyOfDevicePath = VarConsole;
    do {
      RemoveDevPath = FALSE;
      //
      // Check every instance of the console variable
      //
      Instance  = GetNextDevicePathInstance (&CopyOfDevicePath, &Size);
      if (Instance != NULL) {
        Next = Instance;
        while (!IsDevicePathEndType (Next)) {
          Next = NextDevicePathNode (Next);
        }

        SetDevicePathEndNode (Next);

        //
        // Find Terminal device path
        //
        if (CRIsTerminalDevicePathInstance (Instance) == TRUE) {
          RemoveDevPath = TRUE;
          for (Index = 0; Index < TDManager->DeviceCount; Index++) {
            if (CREqualDevicePath (Instance, TDManager->Device[Index].DevicePath) == TRUE) {
              RemoveDevPath = FALSE;
            }
          }
        }

        if (RemoveDevPath) {
          //
          // Remove Instance from Variable
          //
          CRUpdateConsoleVariable (VarName, VarGuid, NULL, Instance);
        }
        
        FreePool(Instance);
      }
    } while (CopyOfDevicePath != NULL);

    FreePool(VarConsole);
  }

  return EFI_SUCCESS;
}


/**

  Based on CrPolicy to determine inputed device is support
  Console Redirection feature or not.

  @param  CrPolicy                Pointer to ConsoleRedirection policy protocol
  @param  DevicePath            Pointer to device path protocol 
  @param  Type                    Indicate serial device type
  @param  Index                   The index of CrSerialDevice in CrPolicy protocol

  @retval  TRUE                    The Device support Console Redirection feature
  @retval  FALSE                   The Device unsupport Console Redirection feature

**/
BOOLEAN
IsCrSupport (
  IN EFI_CR_POLICY_PROTOCOL      *CrPolicy,
  IN EFI_DEVICE_PATH_PROTOCOL    *DevicePath,
  IN CR_SERIAL_DEVICE_TYPE       Type,
  IN OUT UINTN                   *CrDeviceIndex
  )
{
  EFI_STATUS    Status;
  UINT32        PortNumber;
  UINTN         DevicePathSize;
  UINTN         Index; 
  UINTN         Start;
  UINTN         CrDevCount;
  UINTN         Bus;
  UINTN         Dev;
  UINTN         Func;

  
  DevicePathSize = GetDevicePathSize (DevicePath);
  if (DevicePathSize == 0) {
    return FALSE;
  }

  CrDevCount = 0;
  CrDevCount = CrPolicy->CRSerialDevData.IsaDevCount + 
               CrPolicy->CRSerialDevData.PciDevCount + 
               CrPolicy->CRSerialDevData.UsbDevCount;
  
  if (CRIsIsaSerialDevicePathInstance (DevicePath) || Type == ISA_SERIAL_DEVICE) {
    //
    // ISA serial
    //
    Status = CRGetIsaSerialPortNumber (DevicePath, &PortNumber);  
    if (!EFI_ERROR (Status)) {
      if (CrPolicy->CRSerialDevData.CRDevice[PortNumber].Type == ISA_SERIAL_DEVICE) {
        *CrDeviceIndex = PortNumber;
        return CrPolicy->CRSerialDevData.CRDevice[PortNumber].CrDevAttr.PortEnable;
      }
    }
  } else if (CRIsPciSerialDevicePathInstance (DevicePath) || Type == PCI_SERIAL_DEVICE) {    
    //
    // PCI serial
    //
    Start = CrPolicy->CRSerialDevData.IsaDevCount;
    if (CRIsPciSerialDevicePathInstance(DevicePath)) {
      Status = CRGetPciSerialLocation (DevicePath, &Bus, &Dev, &Func);
    } else { 
      Status = CRGetPciDevLocation (DevicePath, &Bus, &Dev, &Func);
    }
    
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
    
    for (Index = Start; Index < CrDevCount; Index++) {
      if (CrPolicy->CRSerialDevData.CRDevice[Index].Type == PCI_SERIAL_DEVICE) {
        if (Bus == CrPolicy->CRSerialDevData.CRDevice[Index].Device.PciSerial.Bus &&
            Dev == CrPolicy->CRSerialDevData.CRDevice[Index].Device.PciSerial.Device &&
            Func == CrPolicy->CRSerialDevData.CRDevice[Index].Device.PciSerial.Function) {
          *CrDeviceIndex = Index;
          return CrPolicy->CRSerialDevData.CRDevice[Index].CrDevAttr.PortEnable;
        }
      }
    }
  } else if (CRIsUsbSerialDevicePathInstance (DevicePath) || Type == USB_SERIAL_DEVICE) {
    //
    // USB serial
    //
    Start = CrPolicy->CRSerialDevData.IsaDevCount + CrPolicy->CRSerialDevData.PciDevCount;
    for (Index = Start; Index < CrDevCount; Index++) {
      if (CrPolicy->CRSerialDevData.CRDevice[Index].Type == USB_SERIAL_DEVICE) {
        return CrPolicy->CRSerialDevData.CRDevice[Index].CrDevAttr.PortEnable;
      }
    }
  }
  
  return FALSE;
}


/**

  Based on CrPolicy to update Console Redirection USB Terminal device path.

  @param  DevicePath         Pointer to device path protocol
  @param  CrPolicy              Pointer to ConsoleRedirection policy protocol

  @retval Pointer to device path protocol

**/
EFI_DEVICE_PATH_PROTOCOL *
UsbUpdateTerminalDevPath (
  IN EFI_DEVICE_PATH_PROTOCOL    *DevicePath,
  IN EFI_CR_POLICY_PROTOCOL      *CrPolicy
  )
{
  UINT32    BaudRate;
  UINT8     TerminalType;
  UINT8     DataBits;
  UINT8     StopBits;
  UINT8     Parity;
  UINTN     UsbIndex;

  TerminalType = 0;
  DataBits = 0;
  StopBits = 0;
  Parity   = 0;
  BaudRate = 0;
  UsbIndex = CrPolicy->CRSerialDevData.IsaDevCount + CrPolicy->CRSerialDevData.PciDevCount;
    
  if (CrPolicy->CRSerialDevData.CRDevice[UsbIndex].CrDevAttr.UseGlobalSetting) {
    BaudRate = mCrBaudRateTable[CrPolicy->CRBaudRate];
    DataBits = CrPolicy->CRDataBits;
    StopBits = CrPolicy->CRStopBits;
    Parity   = CrPolicy->CRParity;
    TerminalType = CrPolicy->CRTerminalType;
  } else {
    BaudRate = mCrBaudRateTable[CrPolicy->CRSerialDevData.CRDevice[UsbIndex].CrDevAttr.BaudRate];
    DataBits = CrPolicy->CRSerialDevData.CRDevice[UsbIndex].CrDevAttr.DataBits;
    StopBits = CrPolicy->CRSerialDevData.CRDevice[UsbIndex].CrDevAttr.StopBits;
    Parity   = CrPolicy->CRSerialDevData.CRDevice[UsbIndex].CrDevAttr.Parity;
    TerminalType = CrPolicy->CRSerialDevData.CRDevice[UsbIndex].CrDevAttr.TerminalType;
  }
  
  mUartNode.BaudRate = BaudRate;
  mUartNode.DataBits = DataBits;
  mUartNode.StopBits = StopBits;
  mUartNode.Parity   = Parity;

  CopyMem (&(mTerminalNode.Guid), &(mCrTerminalGuidTable[TerminalType]), sizeof (EFI_GUID));

  return CRAppendMultiDevicePathNode (DevicePath, 2, &mUartNode, &mTerminalNode);

}


/**

  Update UART device path by parent device information.

  @param  ParentHandle         Device handle 
  @param  UartDevicePath      Pointer to UART device path protocol

**/
VOID
PciUpdateTerminalDevPath (
  IN EFI_HANDLE              ParentHandle,
  IN OUT UART_DEVICE_PATH    *UartDevicePath
  )
{
  EFI_STATUS                             Status;
  EFI_DEVICE_PATH_PROTOCOL               *DevicePath;
  UART_DEVICE_PATH                       *UartDevPath;
  EFI_OPEN_PROTOCOL_INFORMATION_ENTRY    *OpenInfoBuffer;
  UINTN                                  EntryCount;
  UINTN                                  Index;

  //
  // Get child handle's UART device path and update it to DevicePath
  //
  Status = gBS->OpenProtocolInformation (
                  ParentHandle,
                  &gEfiPciIoProtocolGuid,
                  &OpenInfoBuffer,
                  &EntryCount
                  );
  
  if (!EFI_ERROR (Status)) {
    for (Index = 0; Index < EntryCount; Index++) {
      if (OpenInfoBuffer[Index].Attributes & EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER) {
        Status = gBS->HandleProtocol (
                        OpenInfoBuffer[Index].ControllerHandle,
                        &gEfiDevicePathProtocolGuid,
                        (VOID **)&DevicePath
                        );
        if (CRIsPciSerialDevicePathInstance (DevicePath)) {
          UartDevPath = (UART_DEVICE_PATH *) CRLocateDevicePathNode (&DevicePath, MESSAGING_DEVICE_PATH, MSG_UART_DP);
          if (UartDevPath != NULL) {
            UartDevicePath->BaudRate = UartDevPath->BaudRate;
            break;
          }
        }
      }
    }
    gBS->FreePool (OpenInfoBuffer);
  }
  
}


/**

  Based on CrPolicy to update Console Redirection Terminal device path.

  @param  DevicePath         Pointer to device path protocol
  @param  UID                   ACPI UID of UART 
  @param  CrPolicy              Pointer to ConsoleRedirection policy protocol

**/
VOID
UpdateTerminalDevPath (
  IN OUT EFI_DEVICE_PATH_PROTOCOL    *TerminalDevicePath,
  IN UINT8                           UID,
  IN EFI_CR_POLICY_PROTOCOL          *CrPolicy
  )
{
  EFI_STATUS                  Status;
  EFI_DEVICE_PATH_PROTOCOL    *DevicePath;
  UART_DEVICE_PATH            *UartDevPath;
  EFI_DEVICE_PATH_PROTOCOL    *DevPath;
  UART_DEVICE_PATH            *UartNode;
  VENDOR_DEVICE_PATH          *TerminalNode;
  EFI_HANDLE                  *HandleBuffer;
  UINTN                       Index;
  UINTN                       HandleCount;
  UINT8                       TerminalType;
  UINT8                       DataBits;
  UINT8                       StopBits;
  UINT8                       Parity;
  UINT32                      BaudRate;


  if (CrPolicy->CRSerialDevData.CRDevice[UID].CrDevAttr.UseGlobalSetting) {
    BaudRate = mCrBaudRateTable[CrPolicy->CRBaudRate];
    DataBits = CrPolicy->CRDataBits;
    StopBits = CrPolicy->CRStopBits;
    Parity   = CrPolicy->CRParity;
    TerminalType = CrPolicy->CRTerminalType;
  } else {
    BaudRate = mCrBaudRateTable[CrPolicy->CRSerialDevData.CRDevice[UID].CrDevAttr.BaudRate];
    DataBits = CrPolicy->CRSerialDevData.CRDevice[UID].CrDevAttr.DataBits;
    StopBits = CrPolicy->CRSerialDevData.CRDevice[UID].CrDevAttr.StopBits;
    Parity   = CrPolicy->CRSerialDevData.CRDevice[UID].CrDevAttr.Parity;
    TerminalType = CrPolicy->CRSerialDevData.CRDevice[UID].CrDevAttr.TerminalType;
  }

  //
  // Update DevicePath Serial node
  //
  DevPath = TerminalDevicePath;
  UartNode = (UART_DEVICE_PATH *) CRLocateDevicePathNode (&DevPath, MESSAGING_DEVICE_PATH, MSG_UART_DP);
  if (UartNode != NULL) {
    UartNode->BaudRate = BaudRate;
    UartNode->DataBits = DataBits;
    UartNode->StopBits = StopBits;
    UartNode->Parity   = Parity;
  }

  //
  // Update DevicePath Terminal node
  //
  DevPath = TerminalDevicePath;
  TerminalNode = (VENDOR_DEVICE_PATH *) CRLocateDevicePathNode (&DevPath, MESSAGING_DEVICE_PATH, MSG_VENDOR_DP);
  if (TerminalNode != NULL) {
    TerminalNode->Guid = mCrTerminalGuidTable[TerminalType];
  }

  Status = gBS->LocateHandleBuffer (ByProtocol, &gEfiSerialIoProtocolGuid, NULL, &HandleCount, &HandleBuffer);
  if (EFI_ERROR(Status)) {
    return;
  }
  
  for (Index=0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (HandleBuffer[Index], &gEfiDevicePathProtocolGuid, (VOID **)&DevicePath);
    if (EFI_ERROR(Status)) {
      continue;
    }
    
    if (CRIsIsaSerialPortDevicePathInstance (DevicePath, (UINT8)UID)) {
      UartDevPath = (UART_DEVICE_PATH *) CRLocateDevicePathNode (&DevicePath, MESSAGING_DEVICE_PATH, MSG_UART_DP);
      if ((UartDevPath != NULL) && (UartNode != NULL)) {
        UartNode->BaudRate = UartDevPath->BaudRate;
      }
    }
  }  
  
}


/**

  Connect an ISA terminal device with the give policy

  @param CrPolicy                  The give Console Redirection policy

  @retval  EFI_SUCCESS           The ISA terminal device is successfully connected
  @retval  others                    Failed to connect the ISA terminal device

**/
EFI_STATUS
ConnectIsaTerminalDevice (
  IN EFI_CR_POLICY_PROTOCOL    *CrPolicy
  )
{
  EFI_STATUS                    Status;
  EFI_STATUS                    Result;
  EFI_ISA_ACPI_PROTOCOL         *IsaAcpi;
  EFI_ISA_ACPI_DEVICE_ID        *IsaDevice;
  EFI_ISA_ACPI_RESOURCE_LIST    *ResourceList;
  EFI_DEVICE_PATH_PROTOCOL      *IsaBridgeDevPath;
  ACPI_HID_DEVICE_PATH          *AcpiDevPath;
  BOOLEAN                       DoConnect;
  UINTN                         Index;
  UINT8                         UID;


  //
  // Get platform ISA Bridge DevPath
  //
  IsaBridgeDevPath = CRGetPlatformISABridgeDevPath ();
  if (IsaBridgeDevPath == NULL) {
    return EFI_UNSUPPORTED;
  }
  
  //
  // Force LPC device connect with IsaAcpi Driver for get serial port resource
  //
  Status = CRBdsLibConnectDevicePath (IsaBridgeDevPath);

  gPlatformTerminallDevice = CRAppendMultiDevicePathNode (
                               IsaBridgeDevPath, 
                               3, 
                               &mIsaSerialNode,
                               &mUartNode,
                               &mTerminalNode
                               );

  Status = gBS->LocateProtocol (&gEfiIsaAcpiProtocolGuid, NULL, (VOID **)&IsaAcpi);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  
  //
  // Do 2 phase connect.  The first phase connect serial device. Second phase connect with terminal
  // Serial driver maybe connect device with different Barurate.
  //
  Result = EFI_UNSUPPORTED;
  for (Index = 0; Index < 2; Index++) {

    IsaDevice = NULL;
    //
    // Find ISA serial device
    //
    do {
      Status = IsaAcpi->DeviceEnumerate (IsaAcpi, &IsaDevice);
      if (EFI_ERROR (Status)) {
        break;
      }

      //
      // Get ISA device
      //
      ResourceList = NULL;
      Status = IsaAcpi->GetCurResource (IsaAcpi, IsaDevice, &ResourceList);
      if (EFI_ERROR (Status)) {
        continue;
      }

      //
      // Connect ISA serial device
      //
      if (ResourceList->Device.HID == EISA_PNP_ID(EISA_SERIAL_DEVICE_ID)) {
        DoConnect = FALSE;
        UID = (UINT8)ResourceList->Device.UID;
      
        if (CrPolicy->CRSerialDevData.CRDevice[UID].CrDevAttr.PortEnable){
          DoConnect = TRUE;
        }

        if(DoConnect == TRUE) {
          AcpiDevPath = CRLocateIsaSerialDevicePathNode (gPlatformTerminallDevice);
          if (AcpiDevPath != NULL) {
            AcpiDevPath->UID = ResourceList->Device.UID;
          }
          UpdateTerminalDevPath (gPlatformTerminallDevice, (UINT8)ResourceList->Device.UID, CrPolicy);
          Status = CRBdsLibConnectDevicePath (gPlatformTerminallDevice);
          if (Status == EFI_SUCCESS) {
            Result = EFI_SUCCESS;
          }  
        }
      } 
    } while (TRUE);
  }
  
  return Result;
}


/**

  Connect a PCI terminal device with the give policy

  @param CrPolicy           The give Console Redirection policy

  @retval EFI_SUCCESS           The PCI terminal device is successfully connected
  @retval others                    Failed to connect the PCI terminal device

**/
EFI_STATUS
ConnectPciTerminalDevice (
  IN EFI_CR_POLICY_PROTOCOL      *CrPolicy
  )
{
  EFI_STATUS                  Status;
  EFI_STATUS                  Result;
  EFI_HANDLE                  *HandleBuffer;
  EFI_DEVICE_PATH_PROTOCOL    *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL    *TerminalDevPath;
  UART_DEVICE_PATH            UartDevPath;
  UINT32                      TerminalType;
  BOOLEAN                     DoConnect;
  UINTN                       Start; 
  UINTN                       CrDevCount;
  UINTN                       HandleCount;
  UINTN                       PciUartIdx;
  UINTN                       CrDevIdx;
  

  HandleBuffer = NULL;
  Status = CRLocateDeviceHandleBuffer (PCI_CLASS_SCC, PCI_SUBCLASS_SERIAL, PCI_IF_16550, &HandleCount, &HandleBuffer);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  //
  // Connect all PCI UART device
  //
  Result = EFI_NOT_FOUND;
  for (PciUartIdx = 0; PciUartIdx < HandleCount; PciUartIdx++) {
    
    //
    // Set UartDevPath as default UART DevicePath
    //
    UartDevPath = mUartNode;

    Status = gBS->HandleProtocol (HandleBuffer[PciUartIdx], &gEfiDevicePathProtocolGuid, (VOID **)&DevicePath);
    if (EFI_ERROR (Status)) {
      continue;
    }
    
    //
    // Base on CrPolicy, confirm current PCI UART device need support ConsoleRedirection feature.
    //
    DoConnect = FALSE;
    Start = CrPolicy->CRSerialDevData.IsaDevCount;
    CrDevCount = CrPolicy->CRSerialDevData.IsaDevCount + 
                 CrPolicy->CRSerialDevData.PciDevCount + 
                 CrPolicy->CRSerialDevData.UsbDevCount;
    if (IsCrSupport (CrPolicy, DevicePath, PCI_SERIAL_DEVICE, &CrDevIdx)) {
      if (CrPolicy->CRSerialDevData.CRDevice[CrDevIdx].CrDevAttr.UseGlobalSetting) {
        UartDevPath.BaudRate = mCrBaudRateTable[CrPolicy->CRBaudRate];
        UartDevPath.DataBits = CrPolicy->CRDataBits;
        UartDevPath.StopBits = CrPolicy->CRStopBits;
        UartDevPath.Parity   = CrPolicy->CRParity;
        TerminalType = CrPolicy->CRTerminalType;
      } else {
        UartDevPath.BaudRate = mCrBaudRateTable[CrPolicy->CRSerialDevData.CRDevice[CrDevIdx].CrDevAttr.BaudRate];
        UartDevPath.DataBits = CrPolicy->CRSerialDevData.CRDevice[CrDevIdx].CrDevAttr.DataBits;
        UartDevPath.StopBits = CrPolicy->CRSerialDevData.CRDevice[CrDevIdx].CrDevAttr.StopBits;
        UartDevPath.Parity   = CrPolicy->CRSerialDevData.CRDevice[CrDevIdx].CrDevAttr.Parity;
        TerminalType = CrPolicy->CRSerialDevData.CRDevice[CrDevIdx].CrDevAttr.TerminalType;
      }
        CopyMem (&(mTerminalNode.Guid), &(mCrTerminalGuidTable[TerminalType]), sizeof (EFI_GUID));
        DoConnect = TRUE; 
    }

    if (DoConnect == FALSE) { 
      continue;
    }
    
    //
    // Build expect device path for connection
    //
    TerminalDevPath = CRAppendMultiDevicePathNode (DevicePath, 2, &UartDevPath, &mTerminalNode);

    if (TerminalDevPath == NULL) {
      gBS->FreePool (HandleBuffer);
      return EFI_OUT_OF_RESOURCES;
    }

    Status = CRBdsLibConnectDevicePath (TerminalDevPath);
    gBS->FreePool (TerminalDevPath);

    //
    // Connect fail need to connect with new DevPath again
    //
    if (EFI_ERROR (Status)) {
      PciUpdateTerminalDevPath (HandleBuffer[PciUartIdx], &UartDevPath);
      TerminalDevPath = CRAppendMultiDevicePathNode (DevicePath, 2, &UartDevPath, &mTerminalNode);
      if (TerminalDevPath == NULL) {
        gBS->FreePool (HandleBuffer);
        return EFI_OUT_OF_RESOURCES;
      }
      Status = CRBdsLibConnectDevicePath (TerminalDevPath);
      gBS->FreePool (TerminalDevPath);
      if (EFI_ERROR(Status)) {
        continue;
      }
    }

    Result = EFI_SUCCESS;
  }

  gBS->FreePool (HandleBuffer);
  
  return Result;
}


/**

  Connect a USB terminal device with the give policy

  @param CrPolicy                The give Console Redirection policy

  @retval EFI_SUCCESS           The USB terminal device is successfully connected
  @retval others                    Failed to connect the USB terminal device

**/
EFI_STATUS
ConnectUsbTerminalDevice (
  IN EFI_CR_POLICY_PROTOCOL    *CrPolicy
  )
{
  EFI_STATUS                  Status;
  EFI_DEVICE_PATH_PROTOCOL    *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL    *ExpectDPath;  
  EFI_HANDLE                  *UsbHandleBuffer;
  UINTN                       UsbHandleCount;
  UINTN                       Index;

  //
  // Get all USB2 controller device handle
  //
  Status= EFI_NOT_FOUND;
  UsbHandleBuffer = NULL;
  Status = CRLocateDeviceHandleBuffer (
             PCI_CLASS_SERIAL, 
             PCI_CLASS_SERIAL_USB, 
             PCI_IF_EHCI, 
             &UsbHandleCount, 
             &UsbHandleBuffer
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  for (Index = 0; Index < UsbHandleCount; Index++) {
    //
    // Connect USB host controller 
    //    
    gBS->ConnectController (UsbHandleBuffer[Index], NULL, NULL, FALSE);
  }
  
  //
  // Locate C.R device
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol, 
                  &gUsbSerialControllerIoProtocolGuid, 
                  NULL, 
                  &UsbHandleCount, 
                  &UsbHandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
    
  for (Index = 0; Index < UsbHandleCount; Index++) {
    DevicePath = NULL;
    ExpectDPath = NULL;
    Status = gBS->HandleProtocol(UsbHandleBuffer[Index], &gEfiDevicePathProtocolGuid, &DevicePath);
    if (EFI_ERROR (Status)){
      continue;
    }

    //
    // Build expect C.R usb device path
    //
    ExpectDPath = UsbUpdateTerminalDevPath (DevicePath, CrPolicy);

    //
    // Disconnect unexpect C.R device
    //
    gBS->DisconnectController (UsbHandleBuffer[Index], NULL, NULL);
        
    //
    // Connect expect C.R device.
    //
    Status = CRBdsLibConnectDevicePath (ExpectDPath);
    
    gBS->FreePool (ExpectDPath);
    
  }
  gBS->FreePool (UsbHandleBuffer);
  
  return EFI_SUCCESS;
}


/**

  Connect a ConsoleRedirection Terminal device with the give policy

  @param CrPolicy             The give Console Redirection policy

  @retval EFI_SUCCESS       The PCI terminal device is successfully connected
  @return others                Failed to connect the terminal device

**/
EFI_STATUS
ConnectTerminalDevice (
  IN EFI_CR_POLICY_PROTOCOL    *CrPolicy
  )
{
  EFI_STATUS    Status;
  EFI_STATUS    Result;

  Result = EFI_NOT_FOUND;

  //
  // Connect ISA serial device
  //
  if (CrPolicy->CRSerialDevData.IsaDevCount > 0) {
    Result = ConnectIsaTerminalDevice (CrPolicy);
  }

  //
  // Connect PCI serial device
  //
  if (CrPolicy->CRSerialDevData.PciDevCount > 0) {
    Status = ConnectPciTerminalDevice (CrPolicy);
    if (Status == EFI_SUCCESS)
      Result = EFI_SUCCESS;
  }

  //
  // Connect USB serial device
  //
  if (CrPolicy->CRSerialDevData.UsbDevCount > 0) {
    Result = ConnectUsbTerminalDevice (CrPolicy);
  }  

  return Result;
}


/**

  Build ConsoleRedirection information with Terminal Device Manager

  @param  TDManager         Pointer to Terminal Device Manager structure
  @param  Headless            if TRUE indicate system wirh VGA. if FALSE indicate system without VGA. 

  @retval Pointer to Console Redirection information structure

**/
EFI_CONSOLE_REDIRECTION_INFO*
TdmBuildCrInfo (
  IN TERMINAL_DEVICE_MANAGER    *TDManager,
  IN BOOLEAN                    Headless
  )
{
  UINTN                           Size;
  EFI_CONSOLE_REDIRECTION_INFO    *CRInfo;
  CR_ISA_SERIAL_DEVICE            *IsaSerial;
  CR_PCI_SERIAL_DEVICE            *PciSerial;
  EFI_DEVICE_PATH_PROTOCOL        *DevPath;
  UART_DEVICE_PATH                *UartDevPath;  
  TERMINAL_DEVICE                 *Device;
  UINTN                           NonUSBSerialCount;
  UINTN                           Index;

  
  if (TDManager->DeviceCount == 0) {
    return NULL;
  }

  //
  // Caculate the CRInfo size
  //
  NonUSBSerialCount = 0;
  for (Index = 0; Index < TDManager->DeviceCount; Index++) {
    if (TDManager->Device[Index].DeviceType == USB_SERIAL_DEVICE) {
      continue;
    } else {
      NonUSBSerialCount++;
    }
  }    

  //
  // If only USB serial device, we don't need create CRInfo 
  //
  if (NonUSBSerialCount == 0) {
    return NULL;
  }
  
  Size = sizeof(EFI_CONSOLE_REDIRECTION_INFO) + ((NonUSBSerialCount) * sizeof (EFI_CONSOLE_REDIRECTION_DEVICE));

  //
  // Allocate memory for CRInfo
  //
  CRInfo = AllocateZeroPool (Size);
  if (CRInfo == NULL) {
    return NULL;
  }

  CRInfo->Headless = Headless;
  CRInfo->DeviceCount = (UINT8) NonUSBSerialCount;

  //
  // Fill out CRDevice array
  //
  for (Index = 0; Index < TDManager->DeviceCount; Index++) {

    Device = &TDManager->Device[Index];
    CRInfo->CRDevice[Index].DevicePath = Device->DevicePath;

    //
    // Set BaudRateDivisor
    //
    DevPath = Device->DevicePath;
    UartDevPath = (UART_DEVICE_PATH *) CRLocateDevicePathNode(&DevPath, MESSAGING_DEVICE_PATH, MSG_UART_DP);
    CRInfo->CRDevice[Index].BaudRateDivisor = (UINT8)(115200 / (UINTN)UartDevPath->BaudRate);

    //
    // ISA serial device
    //
    if (Device->DeviceType == ISA_SERIAL_DEVICE) {
      IsaSerial = &CRInfo->CRDevice[Index].Device.IsaSerial;
      CRInfo->CRDevice[Index].Type = ISA_SERIAL_DEVICE;
      IsaSerial->ComPortAddress = Device->PortAddr;
      IsaSerial->ComPortIrq = (UINT8) Device->IRQ;
    }	else if (Device->DeviceType == USB_SERIAL_DEVICE) {
      //
      // Ignore USB serial device due to this type not supported in CROpROM
      //
      continue;
    } else {
      //
      // PCI serial device
      //
      PciSerial = &CRInfo->CRDevice[Index].Device.PciSerial;
      CRInfo->CRDevice[Index].Type = PCI_SERIAL_DEVICE;
      PciSerial->Bus = (UINT8) Device->Bus;
      PciSerial->Device = (UINT8) Device->Dev;
      PciSerial->Function = (UINT8) Device->Func;
    }
  }

  return CRInfo;
}


/**

  Base on Terminal Device Manager to build devicepath of ConsoleRedirection device

  @param  TDManager         Pointer to Terminal Device Manager structure

  @retval Pointer to device path protocol

**/
EFI_DEVICE_PATH_PROTOCOL*
TdmBuildCrDevicePath (
  IN TERMINAL_DEVICE_MANAGER    *TDManager
  )
{
  EFI_DEVICE_PATH_PROTOCOL    *DevPath;
  EFI_DEVICE_PATH_PROTOCOL    *PrevDevPath;
  UINTN                       Index;

  DevPath = NULL;
  PrevDevPath = NULL;
  for (Index = 0; Index < TDManager->DeviceCount; Index++) {

    DevPath = AppendDevicePathInstance (PrevDevPath, TDManager->Device[Index].DevicePath);

    if (PrevDevPath != NULL) {
      gBS->FreePool (PrevDevPath);
    }
    
    PrevDevPath = DevPath;
  }

  return DevPath;
}


/**

  Base on Terminal Device Manager to update console variables

  @param  TDManager            Pointer to Terminal Device Manager structure

  @retval  EFI_SUCCESS           Variable update success
  @retval  others                     Variable update fail

**/
EFI_STATUS
TdmUpdateConVar (
  IN TERMINAL_DEVICE_MANAGER    *TDManager
  )
{
  EFI_STATUS                  Status;
  EFI_DEVICE_PATH_PROTOCOL    *CRDevicePath;
  EFI_DEVICE_PATH_PROTOCOL    *NewCRDevicePath;

  //
  // Update ConIn and ConOut candidate variable
  //
  RemoveNonCrTerminalDevFromVariable (TDManager, mConInVarName, mConInVarGuid);
  RemoveNonCrTerminalDevFromVariable (TDManager, mConOutVarName, mConOutVarGuid);
  
  //
  // Get CRDevicePath from Variable;
  //
  CRDevicePath = CommonGetVariableData (L"CRDevicePath", &gEfiGenericVariableGuid);

  //
  // Console Redirection is disabled.
  //
  if (TDManager->DeviceCount == 0) {

    //
    //   Delete Console Redirection device path and Variable
    //
    if(CRDevicePath != NULL) {
      CRUpdateConsoleVariable (mConInVarName, mConInVarGuid, NULL, CRDevicePath);
      CRUpdateConsoleVariable (mConOutVarName, mConOutVarGuid, NULL, CRDevicePath);
      //
      // Delete TDManager;
      //
      Status = CommonSetVariable (
                 L"CRDevicePath",
                 &gEfiGenericVariableGuid,
                 EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                 0,
                 CRDevicePath
                 );
    }

  } else {
    //
    // Console Redirection is enabled
    //
    NewCRDevicePath = TdmBuildCrDevicePath (TDManager);

    //
    // Console Redirection device is not change.  Just add-in.
    //
    if (CREqualDevicePath (NewCRDevicePath, CRDevicePath) == TRUE) {
      //
      // Update ConIn, ConOut variable
      //
      CRUpdateConsoleVariable (mConInVarName, mConInVarGuid, NewCRDevicePath, NULL);
      CRUpdateConsoleVariable (mConOutVarName, mConOutVarGuid, NewCRDevicePath, NULL);
    } else {
      //
      // Update ConIn, ConOut variable
      //
      CRUpdateConsoleVariable (mConInVarName, mConInVarGuid, NewCRDevicePath, CRDevicePath);
      CRUpdateConsoleVariable (mConOutVarName, mConOutVarGuid, NewCRDevicePath, CRDevicePath);

      //
      // Set CRDevicePath;
      //
      Status = CommonSetVariable (
                 L"CRDevicePath",
                 &gEfiGenericVariableGuid,
                 EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                 GetDevicePathSize (NewCRDevicePath),
                 NewCRDevicePath
                 );
    }

    gBS->FreePool (NewCRDevicePath);
  }

  return EFI_SUCCESS;
}


/**

  Create Terminal Device Manager

  @param CrPolicy     The give console redirection policy 

  @retval   Pointer to Terminal Device Manager structure

**/
TERMINAL_DEVICE_MANAGER*
TdmCreate (
  IN EFI_CR_POLICY_PROTOCOL    *CrPolicy
  )
{
  EFI_STATUS                  Status;
  EFI_HANDLE                  *HandleBuffer;
  UINTN                       BufferSize;
  UINTN                       HandleCount;
  TERMINAL_DEVICE_MANAGER     *TDManager;
  EFI_DEVICE_PATH_PROTOCOL    *DevicePath;
  UINTN                       DevCount;    
  BOOLEAN                     DoAddDevice;
  UINT8                       DeviceType;
  UINT32                      PortNumber;
  UINT16                      PortAddr;
  UINT8                       IRQ;
  UINTN                       Bus;
  UINTN                       Dev;
  UINTN                       Func;
  UINTN                       Index;
  UINTN                       CrDeviceIndex;

  //
  // Locate all Terminal handle
  //
  Status = gBS->LocateHandleBuffer (ByProtocol, &gTerminalEscCodeProtocolGuid, NULL, &HandleCount, &HandleBuffer);

  //
  // Has not Terminal  or Console Redirection disable, We still make Empty TDM object
  //
  if (EFI_ERROR(Status) || CrPolicy->CREnable == FALSE) {
    BufferSize = sizeof (TERMINAL_DEVICE_MANAGER);
    Status = gBS->AllocatePool (EfiBootServicesData, BufferSize, (VOID **)&TDManager);
    if (EFI_ERROR (Status)) {
      return NULL;
    }
    
    TDManager->CRPolicy = CrPolicy;
    TDManager->DeviceCount = 0;

    return TDManager;
  }

  //
  // Allocate memory for TDManager
  //
  BufferSize = sizeof (TERMINAL_DEVICE_MANAGER) + sizeof(TERMINAL_DEVICE) * (HandleCount - 1);
  Status = gBS->AllocatePool (EfiBootServicesData, BufferSize, (VOID **)&TDManager);
  if (EFI_ERROR (Status)) {
    return NULL;
  }
  
  //
  // Set CRPolicy
  //
  TDManager->CRPolicy = CrPolicy;

  //
  // Set each SerialDevice
  //
  for (DevCount = 0, Index = 0; Index < HandleCount; Index++) {

    Status = gBS->HandleProtocol (HandleBuffer[Index], &gEfiDevicePathProtocolGuid, (VOID **)&DevicePath);
    if (Status != EFI_SUCCESS) {
      continue;
    }
    
    //
    // Reset DoAddDevice flag
    //
    DoAddDevice = FALSE;

    //
    // Decision to add or not add the device
    //
    if (Status == EFI_SUCCESS) {
      if (IsCrSupport (CrPolicy, DevicePath, UNKNOW_SERIAL_DEVICE, &CrDeviceIndex)) {      
        //
        // CROpROM don't support Usb serial, so don't add usb serial to TDM 
        //
        if (CrPolicy->CRSerialDevData.CRDevice[CrDeviceIndex].Type != USB_SERIAL_DEVICE) {
          DoAddDevice = TRUE;
        }
      }
    }
    //
    // Add the serial device
    //
    if (DoAddDevice == TRUE) {
      //
      // Detect device type
      //
      if (CRIsIsaSerialDevicePathInstance (DevicePath)) {
        DeviceType = ISA_SERIAL_DEVICE;
      } else if (CRIsPciSerialDevicePathInstance (DevicePath)) {
        DeviceType = PCI_SERIAL_DEVICE;   
      } else {
        continue;
      }
      
      TDManager->Device[DevCount].Handle = HandleBuffer[Index];
      TDManager->Device[DevCount].DevicePath = DevicePath;
      TDManager->Device[DevCount].DeviceType = DeviceType;

      if (DeviceType == ISA_SERIAL_DEVICE) {
        CRGetIsaSerialPortNumber (DevicePath, &PortNumber);
        CRGetIsaSerialResource (PortNumber, &PortAddr, &IRQ);
        TDManager->Device[DevCount].PortAddr = PortAddr;
        TDManager->Device[DevCount].IRQ = IRQ;
        DevCount++;
      } else if (DeviceType == PCI_SERIAL_DEVICE) {
        if (CRIsPciSerialDevicePathInstance (DevicePath)) {
          Status = CRGetPciSerialLocation (DevicePath, &Bus, &Dev, &Func);
        } else { 
          Status = CRGetPciDevLocation (DevicePath, &Bus, &Dev, &Func);
        }
        
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


/**

  Destroy Terminal Device Manager

  @param  TDManager            Pointer to Terminal Device Manager structure 

**/
VOID
TdmDestroy (
  IN TERMINAL_DEVICE_MANAGER    *TDManager
  )
{
  if (TDManager != NULL) {
    gBS->FreePool (TDManager);
  }
}


/**

 Disconnect Console Redirection Terminal controller.

 @param        This                    A pointer to the EFI_CONSOLE_REDIRECTION_SERVICE_PROTOCOL instance.
 @param        TerminalNumber   Specify the terminal index that will be disconnect. The first index number is 0.

 @retval         EFI_SUCCESS                    Disconnect success
 @retval         EFI_INVALID_PARAMETER   The Terminal number is invalid
 @retval         others                              Disconnect fail
 
**/
EFI_STATUS
DisconnectCrTerminal (
  IN EFI_CONSOLE_REDIRECTION_SERVICE_PROTOCOL    *This,
  IN UINTN                                       TerminalNumber
  )
{
  EFI_STATUS                      Status;
  EFI_CONSOLE_REDIRECTION_INFO    *CrInfo;
  EFI_DEVICE_PATH_PROTOCOL        *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL        *NewDevicePath;
  EFI_DEVICE_PATH_PROTOCOL        *CRDevicePathInstance;
  EFI_HANDLE                      ControllerHandle;
  UINTN                           Size;

  CrInfo = This->CRInfo;

  if (TerminalNumber >= CrInfo->DeviceCount) {
    return EFI_INVALID_PARAMETER;
  }

  DevicePath = CrInfo->CRDevice[TerminalNumber].DevicePath;

  NewDevicePath = GetNextDevicePathInstance (&DevicePath, &Size);

  CRDevicePathInstance = NewDevicePath;

  Status = gBS->LocateDevicePath (&gEfiDevicePathProtocolGuid , &CRDevicePathInstance, &ControllerHandle);

  if (!EFI_ERROR (Status) && IsDevicePathEnd (CRDevicePathInstance)) {
    Status = gBS->DisconnectController (ControllerHandle, NULL, NULL);
    gBS->FreePool (NewDevicePath);
  }
  
  return Status;
}


/**

 Connect Console Redirection Terminal controller.

 @param        ActVgaStatus       VGA connect status in BDS phase
 @param        ConInVarName     The Variable Name of ConIn that Console Redirection
                                              Terminal device path will update to it
 @param        ConInVarGuid       The Variable Guid of ConIn that Console Redirection
                                              Terminal device path will update to it
 @param        ConOutVarName   The Variable Name of ConOut that Console Redirection
                                              Terminal device path will update to it
 @param        ConOutVarGuid     The Variable Guid of ConOut that Console Redirection
                                              Terminal device path will update to it

 @retval         EFI_SUCCESS       Connect success
 @retval         others                 Connect fail

**/
EFI_STATUS
PlatformBdsConnectCrTerminal (
  IN  EFI_STATUS    ActVgaStatus,
  IN  CHAR16        *ConInVarName,
  IN  EFI_GUID      *ConInVarGuid,
  IN  CHAR16        *ConOutVarName,
  IN  EFI_GUID      *ConOutVarGuid
  )
{
  EFI_STATUS                      Status;
  EFI_CONSOLE_REDIRECTION_INFO    *CrInfo;
  EFI_CR_POLICY_PROTOCOL          *CrPolicy;
  TERMINAL_DEVICE_MANAGER         *TDManager;
  BOOLEAN                         Headless;

  mConInVarName  = ConInVarName;
  mConOutVarName = ConOutVarName;
  mConInVarGuid  = ConInVarGuid;
  mConOutVarGuid = ConOutVarGuid;
 
  //
  // Get Console Redirection Policy
  //
  Status = gBS->LocateProtocol (&gEfiCRPolicyProtocolGuid, NULL, (VOID **)&CrPolicy);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  
  //
  // Init CRPolicy with CrIfrNvData
  //
  CRBdsScuInit (CrPolicy);

  Headless  = (ActVgaStatus == EFI_SUCCESS) ? FALSE : TRUE;

  //
  // If headless Force console redireciton enable.
  //
  if (Headless) {
    CrPolicy->CREnable = CR_ENABLE;
  }

  //
  // Connect C.R. terminal device
  //
  if (CrPolicy->CREnable == CR_ENABLE) {
    Status = ConnectTerminalDevice (CrPolicy);
  }

  //
  // Create Terminal device manager
  //
  TDManager = TdmCreate(CrPolicy);

  if (TDManager == NULL) {
    ASSERT(FALSE);
    return EFI_OUT_OF_RESOURCES;
  }

  CrInfo = NULL;
  if (TDManager->DeviceCount != 0) {
    CrInfo = TdmBuildCrInfo (TDManager, Headless);
  }

  //
  //  Set ConIn, ConOut and ConsoleRedirection Variable
  //
  TdmUpdateConVar (TDManager);

  if (CrPolicy->CREnable == CR_ENABLE) {
    //
    //If CR Enable, always install CRService protocol for support hotplug termnal device feature
    //
    mCrService.CRInfo = CrInfo;
    gBS->InstallProtocolInterface (
           &mCrHookImageHandle,
           &gConsoleRedirectionServiceProtocolGuid,
           EFI_NATIVE_INTERFACE,
           &mCrService
           );
  }

  TdmDestroy (TDManager);

  return EFI_SUCCESS;
}
