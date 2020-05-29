/** @file
  Serial driver for standard UARTS on an ISA bus.

Copyright (c) 2006 - 2010, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/


#ifndef _SNAP_LIB_H_
#define _SNAP_LIB_H_

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DevicePathLib.h>
#include <Library/UefiLib.h>
#include <Guid/DebugMask.h>
#include <Protocol/ConsoleControl.h>
#include <Protocol/GraphicsOutput.h>

#define VTS_LEFT_ALIGN      0
#define VTS_RIGHT_ALIGN     1
#define VTS_LEAD_0          2

#define TICKS_PER_MS        10000U

typedef enum {
  DT_IDE = 0,
  DT_SCSI,
  DT_USB,
  DT_AHCI,
  DT_UNKNOW
} DISK_TYPE;

typedef struct {
  UINT8             Pdt;            // Peripheral Device Type (low 5 bits)
  UINT8             Removable;      // Removable Media (highest bit)
  UINT8             Reserved0[2];
  UINT8             AddLen;         // Additional length
  UINT8             Reserved1[3];
  UINT8             VendorID[8];
  UINT8             ProductID[16];
  UINT8             ProductRevision[4];
} USB_BOOT_INQUIRY_DATA;

//
// This funciton implement in PrintLib.lib
//
UINTN
VSPrint (
  OUT CHAR16        *StartOfBuffer,
  IN  UINTN         BufferSize,
  IN  CONST CHAR16  *FormatString,
  IN  VA_LIST       Marker
  );

VOID
ValueToString (
  UINTN     Value,
  UINTN     Digitals,
  CHAR16    *Buffer,
  UINTN     Flags
  );

DISK_TYPE
GetDiskInfoType (
  EFI_HANDLE      Handle
  );

CHAR16 *
GetVolumnName (
  EFI_HANDLE    FsHandle
  );

CHAR16 *
GetVolumnSizeAsString (
  EFI_HANDLE    FsHandle
  );

CHAR16 *
GetDiskInfoName (
  EFI_HANDLE      FsHandle
  );

EFI_STATUS
GetConSplitterVgaHandle (
  EFI_HANDLE      *VgaHandle
  );

EFI_STATUS
GetActiveVgaHandle (
  EFI_HANDLE      *VgaHandle
  );

EFI_STATUS
GetActiveVgaGop (
  EFI_GRAPHICS_OUTPUT_PROTOCOL    **Gop
  );

EFI_STATUS
GetParentHandle (
  EFI_HANDLE      ChildHandle,
  EFI_HANDLE      *ParentHandle
  );

BOOLEAN
IsRecordableDevice (
  EFI_HANDLE    Handle
  );

VOID
GetKey (
  OUT EFI_INPUT_KEY      *Key
);

EFI_STATUS
LocateDriverIndex (
  EFI_GUID        *ProtocolGuid
  );

#endif  // _SNAP_LIB_H_
