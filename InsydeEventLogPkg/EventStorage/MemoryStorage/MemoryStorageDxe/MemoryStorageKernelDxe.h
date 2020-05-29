/** @file

  Header file of Memory Storage Kernel DXE implementation.

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/


#ifndef _MEMORY_STORAGE_KERNEL_DXE_H_
#define _MEMORY_STORAGE_KERNEL_DXE_H_

//
// Statements that include other header files
//
#include <Protocol/MemoryStorage.h>
#include <Protocol/LegacyBios.h>
#include <Protocol/LegacyRegion.h>
#include <Protocol/VariableWrite.h>
#include <Guid/MemoryStorageVariable.h>
//[-start-140306-IB08400249-add]//
#include <Guid/EventStorageVariable.h>
//[-end-140306-IB08400249-add]//
#include <Uefi/UefiBaseType.h>
#include <Uefi/UefiSpec.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/CmosLib.h>
#include <Library/UefiLib.h>

#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>


//
// Define Memory Storage ("_MEMS_") signatures
//
#define MEMORY_STORAGE_INSTANCE_SIGNATURE SIGNATURE_64('_', 'M', 'E', 'M', 'S', '_', 0, 0)

//
// Define Memory Storage Entry ("_MEMSE_") signatures
//
#define MEMORY_STORAGE_ENTRY_SIGNATURE SIGNATURE_64('_', 'M', 'E', 'M', 'S', 'E', '_', 0)


//
// Default Memory Size = 32K Byte
//
#define MEMORY_STORAGE_SIZE                     0x8000

typedef struct {
  UINT64                  MemStorageSignature;      // '_MEMS_'"
  EFI_PHYSICAL_ADDRESS    MemStorageHeadAddr;       // Memory Storage Entry Address
} MEMORY_STORAGE_F_SEGMENT_STRUCTURE;

typedef struct {
  UINT64                  MemStorageEntrySignature;      // '_MEMSE_'"
  EFI_PHYSICAL_ADDRESS    MemStorageCurrentLogAddr;
  EFI_PHYSICAL_ADDRESS    MemStorageEntryAddr;
  UINT64                  MemStorageSize;
} MEMORY_STORAGE_HEAD_STRUCTURE;

VOID
EFIAPI
InstalledVarNotificationFunction (
  IN  EFI_EVENT       Event,
  IN  VOID            *Context
  );

UINT32
EFIAPI
GetFSegmentTableAddr (
  IN  UINT16      Size
  );

VOID
EFIAPI
PublishMemStorageSignature (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  );

EFI_STATUS
EFIAPI
WriteEventToMemStorage (
  IN  UINT8                             SensorType,
  IN  UINT8                             SensorNum,
  IN  UINT8                             EventType,
  IN  UINTN                             OptionDataSize,
  IN  UINT8                             *OptionLogData
  );

EFI_STATUS
EFIAPI
EfiClearMemStorageEvent (
  VOID
  );

EFI_STATUS
EFIAPI
EfiReadNextMemStorageEvent (
  IN OUT VOID                             **MemStorageListAddress
  );

EFI_STATUS
EFIAPI
OverWriteEventToMemStorage (
  IN  MEMORY_STORAGE_ORGANIZATION             *InputBuffer
  );

#endif
