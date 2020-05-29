/** @file
 Console Redirection BDS library defines general interfaces for CrHook driver

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

#ifndef _CR_BDS_LIB_H_
#define _CR_BDS_LIB_H_

#include <IndustryStandard/Pci.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/DevicePathLib.h>
#include <Library/DebugLib.h>
#include <Library/VariableLib.h>

#include <Protocol/TerminalEscCode.h>
#include <Protocol/CRPolicy.h>
#include <Protocol/PciIo.h>
#include <Protocol/SerialIo.h>
#include <Protocol/IsaAcpi.h>
#include <Protocol/DevicePath.h>

#include <Guid/GlobalVariable.h>
#include <Guid/DebugMask.h>

//
// CRBds Console functions
//
VOID *
EFIAPI
CRBdsLibGetVariableAndSize (
  IN  CHAR16              *Name,
  IN  EFI_GUID            *VendorGuid,
  OUT UINTN               *VariableSize
  );


EFI_STATUS
CRUpdateConsoleVariable (
  IN  CHAR16                    *ConVarName,
  IN  EFI_GUID                  *ConVarGuid,
  IN  EFI_DEVICE_PATH_PROTOCOL  *CustomizedConDevicePath,
  IN  EFI_DEVICE_PATH_PROTOCOL  *ExclusiveDevicePath
  );

//
// CRBds Connect functions
//
EFI_STATUS
EFIAPI
CRBdsLibConnectDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePathToConnect
  );


EFI_STATUS
EFIAPI
CRBdsLibConnectUsbDevByShortFormDP(
  IN UINT8                      HostControllerPI,
  IN EFI_DEVICE_PATH_PROTOCOL   *RemainingDevicePath
  );

//
// CRBds Misc functions
//
EFI_STATUS
CRLocateDeviceHandleBuffer (
  IN UINT8          Class,
  IN UINT8          SubClass,
  IN UINT8          Interface,
  OUT UINTN         *HandleCount,
  OUT EFI_HANDLE    **AHandleBuffer
  );


BOOLEAN
CRIsIsaSerialPortDevicePathInstance (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath,
  IN UINT8                     SerialPort
  );


BOOLEAN
CRIsIsaSerialDevicePathInstance (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  );


BOOLEAN
CRIsIsaSerialDevicePathInstance (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  );


BOOLEAN
CRIsPciSerialDevicePathInstance (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  );


BOOLEAN
CRIsUsbSerialDevicePathInstance (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  );


ACPI_HID_DEVICE_PATH *
CRLocateIsaSerialDevicePathNode (
  IN EFI_DEVICE_PATH_PROTOCOL   *DevicePath
  );


EFI_STATUS
CRExtractUartPciDevPath (
  IN EFI_DEVICE_PATH_PROTOCOL     *Src,
  OUT EFI_DEVICE_PATH_PROTOCOL    **DevicePath
  );


EFI_DEVICE_PATH_PROTOCOL *
CRLocateDevicePathNode(
  IN OUT EFI_DEVICE_PATH_PROTOCOL   **DevicePath,
  IN UINT8                          DevPathType,
  IN UINT8                          DevPathSubType
  );


EFI_DEVICE_PATH_PROTOCOL  *
CRAppendMultiDevicePathNode (
  EFI_DEVICE_PATH_PROTOCOL  *SrcDevicePath,
  UINTN                     NumOfArgs,
  ...
  );


BOOLEAN
CREqualDevicePath (
  IN  EFI_DEVICE_PATH_PROTOCOL  *DevicePath1,
  IN  EFI_DEVICE_PATH_PROTOCOL  *DevicePath2
  );


BOOLEAN
CREqualDevicePath (
  IN  EFI_DEVICE_PATH_PROTOCOL  *DevicePath1,
  IN  EFI_DEVICE_PATH_PROTOCOL  *DevicePath2
  );


BOOLEAN
EFIAPI
CRBdsLibMatchDevicePaths (
  IN  EFI_DEVICE_PATH_PROTOCOL  *Multi,
  IN  EFI_DEVICE_PATH_PROTOCOL  *Single
  );


EFI_STATUS
CRGetIsaSerialResource (
  IN UINT32   ComPortNumber,
  OUT UINT16  *PortAddress,
  OUT UINT8   *IRQ
  );


EFI_STATUS
CRGetIsaSerialPortNumber (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath,
  OUT UINT32                   *PortNumber
  );


EFI_STATUS
CRGetPciDevLocation (
  EFI_DEVICE_PATH_PROTOCOL    *DevicePath,
  UINTN                       *Bus,
  UINTN                       *Dev,
  UINTN                       *Func
  );


EFI_STATUS
CRGetPciSerialLocation (
  EFI_DEVICE_PATH_PROTOCOL    *DevicePath,
  UINTN                       *Bus,
  UINTN                       *Dev,
  UINTN                       *Func
  );


EFI_DEVICE_PATH_PROTOCOL *
CRGetPlatformISABridgeDevPath(
  VOID
  );

BOOLEAN
CRIsTerminalDevicePathInstance (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  );

#endif

