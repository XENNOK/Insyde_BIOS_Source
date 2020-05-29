/** @file

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

/*++

Module Name:

  AspmOverrideDxe.h

Abstract:

  A DXE driver for override the device ASPM setting.

--*/

#ifndef _ASPM_OVERRIDE_H_
#define _ASPM_OVERRIDE_H_

#include <Uefi.h>
#include <IndustryStandard/Pci.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/SmmRuntime.h>
#include <Protocol/OverrideAspm.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/SmmControl2.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmOemSvcKernelLib.h>
#include <SmiTable.h>

#define ASPM_DISABLE               0
#define ASPM_L0S                   1
#define ASPM_L1                    2
#define ASPM_L0S_L1_ENABLE         3

#define LINK_CONTROL_REG           0x10

typedef struct {
  UINT8               Dev;
  UINT8               Func;
  UINT8               SecondaryBusNum;
} PCIE_BRIDGE_MAP_TABLE;

typedef struct {
  UINT8               Dev;
  UINT8               Fnc;
  UINT16              VenderId;
  UINT16              DeviceId;
  UINT8               RevId;
} PCIE_LOCATE;

typedef struct {
  UINT8               Bus;
  PCIE_LOCATE         PcieDev[0x40];   //default max devices per bus  = 0x40 (64)
} PCIE_DEVICE_MAP_TABLE;

//
// Function Definition
//
UINT16
FindLinkCtrlOffset (
  IN UINT8     Bus,
  IN UINT8     Dev,
  IN UINT8     Fun
  );

EFI_STATUS
OverrideAspmFunction (
  VOID
  );

EFI_STATUS
OverrideDevice (
  IN PCIE_ASPM_DEV_INFO             *PcieAspmDevs
  );

VOID
EFIAPI
AspmOverrideReadyToBootCallBack (
  IN EFI_EVENT                      Event,
  IN VOID                           *Context
  );

EFI_STATUS
EFIAPI
SmmOverrideAspmFunction (
  IN EFI_HANDLE                  DispatchHandle,
  IN CONST VOID                  *Context,
  IN OUT VOID                    *CommBuffer,
  IN OUT UINTN                   *CommBufferSize
  );

BOOLEAN
CreatPCIeBridgeDeviceMap (
  VOID
  );

BOOLEAN
SearchPCIeBridge (
  VOID
  );

VOID
SearchPCIeDevice (
  VOID
  );

EFI_STATUS
EcpAspmOverrideSupported (
  VOID
  );

#endif
