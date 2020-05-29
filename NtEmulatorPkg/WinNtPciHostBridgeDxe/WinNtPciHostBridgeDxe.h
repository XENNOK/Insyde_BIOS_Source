/** @file
  WinNtPciHostBridgeDxe drvier definitions.

;******************************************************************************
;* Copyright (c) 2015, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
/*++

Copyright (c)  1999 - 2001 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  WinNtPciHostBridgeDxe.h

Abstract:

Revision History

--*/

#ifndef _WIN_NT_PCI_HOST_BRIDGE_DXE_H_
#define _WIN_NT_PCI_HOST_BRIDGE_DXE_H_

#include <Uefi.h>

#include <IndustryStandard/Pci22.h>
#include <IndustryStandard/Acpi.h>

#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>

//
// Driver Consumed Protocol Prototypes
//
#include <Protocol/Metronome.h>

//
// Driver Produced Protocol Prototypes
//
#include <Protocol/DevicePath.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/PciHostBridgeResourceAllocation.h>

#define EFI_LIST_ENTRY                  LIST_ENTRY

//
// Task priority level
//
#define EFI_TPL_APPLICATION 4
#define EFI_TPL_CALLBACK    8
#define EFI_TPL_NOTIFY      16
#define EFI_TPL_HIGH_LEVEL  31

//
// PCI Root Bridge Device Path Instance Type
//
typedef struct {
  ACPI_HID_DEVICE_PATH      AcpiDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  EndDevicePath;
} EFI_PCI_ROOT_BRIDGE_DEVICE_PATH;

//
// PCI Host Bridge Resource Allocation Protocol Device Structure
//
#define HOST_BRIDGE_NUMBER  1

#define WINNT_PCI_HOST_BRIDGE_SIGNATURE  SIGNATURE_32 ('h', 'o', 's', 't')
typedef struct {
  UINTN               Signature;
  EFI_HANDLE          HostBridgeHandle;
  UINTN               RootBridgeNumber;
  EFI_LIST_ENTRY      Head;
  BOOLEAN             ResourceSubmited;  
  BOOLEAN             CanRestarted;  
  EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL        ResAlloc;
} WINNT_PCI_HOST_BRIDGE_INSTANCE;

#define INSTANCE_FROM_RESOURCE_ALLOCATION_THIS(a) \
  CR(a, WINNT_PCI_HOST_BRIDGE_INSTANCE, ResAlloc, WINNT_PCI_HOST_BRIDGE_SIGNATURE)

//
// PCI Root Bridge I/O Protocol Device Structure
//

typedef struct {
  UINTN  BusBase;
  UINTN  BusLimit;
  
  UINTN  MemBase;
  UINTN  MemLimit;
  
  UINTN  IoBase;
  UINTN  IoLimit;
} PCI_ROOT_BRIDGE_RESOURCE_APPETURE;

typedef enum {
  TypeIo = 0,
  TypeMem32,
  TypePMem32,
  TypeMem64,
  TypePMem64,
  TypeBus,
  TypeMax
} PCI_RESOURCE_TYPE;

typedef enum {
  ResNone = 0,
  ResRequested,
  ResAllocated,
  ResStatusMax
} RES_STATUS;

typedef struct {
  PCI_RESOURCE_TYPE Type;
  UINTN             Base;
  UINTN             Length;
  RES_STATUS        Status;
} PCI_RES_NODE;

#define WINNT_PCI_ROOT_BRIDGE_SIGNATURE  SIGNATURE_32 ('n', '2', 'p', 'b')

typedef struct {
  UINT32                 Signature;
  EFI_LIST_ENTRY         Link;
  EFI_HANDLE             Handle;
  UINT64                 RootBridgeAttrib;
  UINT64                 Attributes;
  
  //
  // Specific for ea815: Bus, I/O, Mem
  //
  PCI_RES_NODE           ResAllocNode[6];
  
  //
  // Addressing for Memory and I/O and Bus arrange
  //
  UINTN                  BusBase;
  UINTN                  MemBase;     
  UINTN                  IoBase; 
  UINTN                  BusLimit;     
  UINTN                  MemLimit;    
  UINTN                  IoLimit;     

  EFI_LOCK               PciLock;
  UINTN                  PciAddress;
  UINTN                  PciData;
  
  EFI_DEVICE_PATH_PROTOCOL                *DevicePath;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL         Io;
} WINNT_PCI_ROOT_BRIDGE_INSTANCE;

#define DRIVER_INSTANCE_FROM_PCI_ROOT_BRIDGE_IO_THIS(a) \
  CR(a, WINNT_PCI_ROOT_BRIDGE_INSTANCE, Io, WINNT_PCI_ROOT_BRIDGE_SIGNATURE)

#define DRIVER_INSTANCE_FROM_LIST_ENTRY(a) \
  CR(a, WINNT_PCI_ROOT_BRIDGE_INSTANCE, Link, WINNT_PCI_ROOT_BRIDGE_SIGNATURE)

//
//
//

EFI_STATUS
WinNtRootBridgeConstructor (
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL    *Protocol,
  IN EFI_HANDLE                         HostBridgeHandle,
  IN UINT64                             Attri,
  IN PCI_ROOT_BRIDGE_RESOURCE_APPETURE  ResAppeture
  );

//
//  PCI Host Bridge Resource Allocation Protocol function prototypes
//
EFI_STATUS
NotifyPhase(
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PHASE    Phase
  );

EFI_STATUS
GetNextRootBridge(
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN OUT EFI_HANDLE                                   *RootBridgeHandle
  );
  
EFI_STATUS
GetAttributes(
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN EFI_HANDLE                                       RootBridgeHandle,
  OUT UINT64                                          *Attributes
  );
  
EFI_STATUS
StartBusEnumeration(
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN EFI_HANDLE                                       RootBridgeHandle,
  OUT VOID                                            **Configuration
  );
  
EFI_STATUS
SetBusNumbers(
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN EFI_HANDLE                                       RootBridgeHandle,
  IN VOID                                             *Configuration
  );
  
EFI_STATUS
SubmitResources(
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN EFI_HANDLE                                       RootBridgeHandle,
  IN VOID                                             *Configuration
  );
  
EFI_STATUS
GetProposedResources(
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN EFI_HANDLE                                       RootBridgeHandle,
  OUT VOID                                            **Configuration
  );

EFI_STATUS
PreprocessController (
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL  *This,
  IN  EFI_HANDLE                                                RootBridgeHandle,
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS                PciAddress,
  IN  EFI_PCI_CONTROLLER_RESOURCE_ALLOCATION_PHASE               Phase
  );

//
// PCI Root Bridge I/O Protocol Member Function Prototypes
//
EFI_STATUS
RootBridgeIoPollMem ( 
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN  UINT64                                 Address,
  IN  UINT64                                 Mask,
  IN  UINT64                                 Value,
  IN  UINT64                                 Delay,
  OUT UINT64                                 *Result
  );
  
EFI_STATUS
RootBridgeIoPollIo ( 
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN  UINT64                                 Address,
  IN  UINT64                                 Mask,
  IN  UINT64                                 Value,
  IN  UINT64                                 Delay,
  OUT UINT64                                 *Result
  );
  
EFI_STATUS
RootBridgeIoMemRead (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64                                 Address,
  IN     UINTN                                  Count,
  IN OUT VOID                                   *Buffer
  );

EFI_STATUS
RootBridgeIoMemWrite (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64                                 Address,
  IN     UINTN                                  Count,
  IN OUT VOID                                   *Buffer
  );

EFI_STATUS
RootBridgeIoIoRead (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64                                 UserAddress,
  IN     UINTN                                  Count,
  IN OUT VOID                                   *UserBuffer
  );

EFI_STATUS
RootBridgeIoIoWrite (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64                                 UserAddress,
  IN     UINTN                                  Count,
  IN OUT VOID                                   *UserBuffer
  );

EFI_STATUS
RootBridgeIoPciRead (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64                                 Address,
  IN     UINTN                                  Count,
  IN OUT VOID                                   *Buffer
  );

EFI_STATUS
RootBridgeIoPciWrite (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64                                 Address,
  IN     UINTN                                  Count,
  IN OUT VOID                                   *Buffer
  );

EFI_STATUS
RootBridgeIoCopyMem (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL          *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    Width,
  IN     UINT64                                   DestAddress,
  IN     UINT64                                   SrcAddress,
  IN     UINTN                                    Count
  );

EFI_STATUS
RootBridgeIoMap (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL            *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_OPERATION  Operation,
  IN     VOID                                       *HostAddress,
  IN OUT UINTN                                      *NumberOfBytes,
  OUT    EFI_PHYSICAL_ADDRESS                       *DeviceAddress,
  OUT    VOID                                       **Mapping
  );

EFI_STATUS
RootBridgeIoUnmap (
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
  IN  VOID                             *Mapping
  );

EFI_STATUS
RootBridgeIoAllocateBuffer (
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
  IN  EFI_ALLOCATE_TYPE                Type,
  IN  EFI_MEMORY_TYPE                  MemoryType,
  IN  UINTN                            Pages,
  OUT VOID                             **HostAddress,
  IN  UINT64                           Attributes
  );

EFI_STATUS
RootBridgeIoFreeBuffer (
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
  IN  UINTN                            Pages,
  OUT VOID                             *HostAddress
  );

EFI_STATUS
RootBridgeIoFlush (
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This
  );

EFI_STATUS
RootBridgeIoGetAttributes (
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
  OUT UINT64                           *Supported,
  OUT UINT64                           *Attributes
  );

EFI_STATUS
RootBridgeIoSetAttributes (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
  IN     UINT64                           Attributes,
  IN OUT UINT64                           *ResourceBase,
  IN OUT UINT64                           *ResourceLength
  ); 

EFI_STATUS
RootBridgeIoConfiguration (
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
  OUT VOID                             **Resources
  );
 
#endif
