/** @file
  The EFI Legacy BIOS Patform Protocol member function

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _LEGACY_BIOS_PLATFORM_DXE_H_
#define _LEGACY_BIOS_PLATFORM_DXE_H_

#include <Base.h>
#include <IndustryStandard/Pci.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DxeOemSvcKernelLib.h>
#include <Library/VariableLib.h>
#include <Protocol/LegacyBios.h>
#include <Protocol/LegacyBiosPlatform.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/SetupUtility.h>
#include <Protocol/PciIo.h>
#include <Protocol/SdPassThru.h>
#include <Guid/DebugMask.h>
#include <Guid/GlobalVariable.h>

#define F0000Region 0x01
#define E0000Region 0x02

#pragma pack(1)
///
/// This structure assumes both port and data sizes are 1. SmmAttribute must be
/// properly to reflect that assumption.
///
typedef struct {
  ///
  /// Describes the access mechanism, SmmPort, and SmmData sizes. Type
  /// SMM_ATTRIBUTES is defined below.
  ///
  SMM_ATTRIBUTES                    SmmAttributes;

  ///
  /// Function Soft SMI is to perform. Type SMM_FUNCTION is defined below.
  ///
  SMM_FUNCTION                      SmmFunction;

  ///
  /// SmmPort size depends upon SmmAttributes and ranges from2 bytes to 16 bytes.
  ///
  UINT16                             SmmPort;

  ///
  /// SmmData size depends upon SmmAttributes and ranges from2 bytes to 16 bytes.
  ///
  UINT8                             SmmData;
} SMM_ENTRY_PORT16;

///
/// SMM_TABLE for 16 bit port
///
typedef struct {
  UINT16                            NumSmmEntries;    ///< Number of entries represented by SmmEntry.
  SMM_ENTRY_PORT16                  SmmEntry;         ///< One entry per function. Type SMM_ENTRY is defined below.
} SMM_TABLE_PORT16;

#pragma pack()

typedef struct {
  UINT32                            Signature;
  EFI_HANDLE                        Handle;
  EFI_LEGACY_BIOS_PLATFORM_PROTOCOL LegacyBiosPlatform;
  EFI_HANDLE                        ImageHandle;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL   *PciRootBridgeIo;
} LEGACY_BIOS_PLATFORM_INSTANCE;

EFI_STATUS
EFIAPI
PlatformHooks (
  IN      EFI_LEGACY_BIOS_PLATFORM_PROTOCOL     *This,
  IN      EFI_GET_PLATFORM_HOOK_MODE            Mode,
  IN      UINT16                                Type,
  OUT     EFI_HANDLE                            DeviceHandle, OPTIONAL
  IN OUT  UINTN                                 *Shadowaddress, OPTIONAL
  IN      EFI_COMPATIBILITY16_TABLE             *Compatibility16Table, OPTIONAL
  OUT     VOID                                  **AdditionalData OPTIONAL
  );

EFI_STATUS
EFIAPI
GetRoutingTable (
  IN  EFI_LEGACY_BIOS_PLATFORM_PROTOCOL           * This,
  OUT VOID                                        **RoutingTable,
  OUT UINTN                                       *RoutingTableEntries,
  OUT VOID                                        **LocalPirqTable, OPTIONAL
  OUT UINTN                                       *PirqTableSize, OPTIONAL
  OUT VOID                                        **LocalIrqPriorityTable, OPTIONAL
  OUT UINTN                                       *IrqPriorityTableEntries OPTIONAL
  );

EFI_STATUS
EFIAPI
TranslatePirq (
  IN  EFI_LEGACY_BIOS_PLATFORM_PROTOCOL           *This,
  IN UINTN                                        PciBus,
  IN UINTN                                        PciDevice,
  IN UINTN                                        PciFunction,
  IN OUT UINT8                                    *Pirq,
  OUT UINT8                                       *PciIrq
  );

EFI_STATUS
EFIAPI
GetPlatformInfo (
  IN EFI_LEGACY_BIOS_PLATFORM_PROTOCOL            *This,
  IN EFI_GET_PLATFORM_INFO_MODE                   Mode,
  OUT VOID                                        **Table,
  OUT UINTN                                       *TableSize,
  OUT UINTN                                       *Location,
  OUT UINTN                                       *Alignment,
  IN  UINT16                                      LegacySegment,
  IN  UINT16                                      LegacyOffset
  );

EFI_STATUS
EFIAPI
SmmInit (
  IN  EFI_LEGACY_BIOS_PLATFORM_PROTOCOL           *This,
  IN  VOID                                        *EfiToLegacy16BootTable
  );

EFI_STATUS
EFIAPI
PrepareToBoot (
  IN  EFI_LEGACY_BIOS_PLATFORM_PROTOCOL           *This,
  IN  BBS_BBS_DEVICE_PATH                         *BbsDevicePath,
  IN  VOID                                        *BbsTable,
  IN  UINT32                                      LoadOptionsSize,
  IN  VOID                                        *LoadOptions,
  IN  EFI_TO_COMPATIBILITY16_BOOT_TABLE           *EfiToLegacy16BootTable
  );

EFI_STATUS
EFIAPI
GetPlatformHandle (
  IN  EFI_LEGACY_BIOS_PLATFORM_PROTOCOL           *This,
  IN  EFI_GET_PLATFORM_HANDLE_MODE                Mode,
  IN  UINT16                                      Type,
  OUT EFI_HANDLE                                  **HandleBuffer,
  OUT UINTN                                       *HandleCount,
  OUT VOID                                        **AdditionalData OPTIONAL
  );

#define LEGACY_BIOS_PLATFORM_INSTANCE_SIGNATURE SIGNATURE_32 ('P', 'B', 'I', 'O')

#define LEGACY_BIOS_PLATFORM_INSTANCE_FROM_THIS(this) \
  CR ( \
  this, \
  LEGACY_BIOS_PLATFORM_INSTANCE, \
  LegacyBiosPlatform, \
  LEGACY_BIOS_PLATFORM_INSTANCE_SIGNATURE \
  )
#endif
