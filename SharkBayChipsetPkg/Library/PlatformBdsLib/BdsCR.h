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

#ifndef _BDSCR_H
#define _BDSCR_H

#include "BdsPlatform.h"

typedef enum {
  CR_OP_CHECK,
  CR_OP_UPDATE,
} CR_VAR_OPERATION;

typedef struct {
  ACPI_HID_DEVICE_PATH                PciRootBridge;
  PCI_DEVICE_PATH                     IsaBridge;
  EFI_DEVICE_PATH_PROTOCOL            End;
} PLATFORM_ISABRIDGE_DEVICE_PATH;

typedef struct {
  ACPI_HID_DEVICE_PATH                PciRootBridge;
  PCI_DEVICE_PATH                     IsaBridge;
  ACPI_HID_DEVICE_PATH                IsaSerial;
  UART_DEVICE_PATH                    Uart;
  VENDOR_DEVICE_PATH                  Terminal;
  EFI_DEVICE_PATH_PROTOCOL            End;
} PLATFORM_TERMINAL_DEVICE_PATH;

EFI_DEVICE_PATH_PROTOCOL *
LocateDevicePathNode(  
  IN OUT EFI_DEVICE_PATH_PROTOCOL   **DevicePath,
  IN UINT8                          DevPathType,
  IN UINT8                          DevPathSubType
  );

ACPI_HID_DEVICE_PATH *
LocateSerialDevicePathNode (
  IN OUT EFI_DEVICE_PATH_PROTOCOL   *DevicePath
  );

BOOLEAN
IsIsaSerialDevicePathInstance (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  );

BOOLEAN
IsIsaSerialPortDevicePathInstance (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath,
  IN UINT8                     SerialPort
  );

EFI_STATUS
GetIsaSerialPortNumber (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath,
  OUT UINT32                   *PortNumber
);

EFI_DEVICE_PATH_PROTOCOL  *
AppendMultiDevicePathNode (
  EFI_DEVICE_PATH_PROTOCOL  *SrcDevicePath,
  UINTN                     NumOfArgs,
  ...
  );

EFI_STATUS
GetAllPciSerialDevicePath (
  IN OUT EFI_DEVICE_PATH_PROTOCOL  **PciDevicePath,
  IN OUT UINTN *Size
  );

EFI_STATUS
GetAllIsaSerialDevicePath (
  IN OUT EFI_DEVICE_PATH_PROTOCOL  **AllIsaSerialDevicePaths,
  OUT UINTN *SerialDeviceCount
  );

EFI_STATUS
GetAllSerialDevicePath (
  IN OUT EFI_DEVICE_PATH_PROTOCOL  **AllSerialDevicePaths
  );

EFI_STATUS
GetSelectedSerialDevicePath (
  IN UINT8                         SerialPort,
  OUT EFI_DEVICE_PATH_PROTOCOL     **SelectedSerialDevicePaths,
  OUT UINT8                        *Count
  );

EFI_STATUS
GetIsaSerialResource (
  IN UINT32   ComPortNumber,
  OUT UINT16  *PortAddress,
  OUT UINT8   *IRQ
  );  

EFI_STATUS
InitializeCRSInstance (
  IN EFI_CONSOLE_REDIRECTION_INFO   *pInstance,
  IN UINT8                          DeviceCount,
  IN BOOLEAN                        Headless
  );
  
EFI_STATUS
CRUpdateConsoleVariable (
  IN  CHAR16                    *ConVarName,
  IN  EFI_DEVICE_PATH_PROTOCOL  *CustomizedConDevicePath,
  IN  EFI_DEVICE_PATH_PROTOCOL  *ExclusiveDevicePath
  );

EFI_STATUS
PlatformBDSConnectCRTerminal (
  IN BOOLEAN     Headless
  );

EFI_STATUS
DisconnectCrsTerminal (
  EFI_CONSOLE_REDIRECTION_SERVICE_PROTOCOL  *This,
  UINTN                                     TerminalNumber
  );

#endif // _BDSCR_H

