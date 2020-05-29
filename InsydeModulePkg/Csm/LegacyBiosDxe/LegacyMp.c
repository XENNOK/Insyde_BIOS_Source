/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "LegacyBiosInterface.h"

#include <Protocol/MpService.h>

#include <OemServices/Kernel.h>
#include <Library/DxeOemSvcKernelLib.h>
#include "KernelSetupConfig.h"

#include <IndustryStandard/Pci.h>
#include <IndustryStandard/Acpi10.h>

#include <Protocol/PciIo.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/LegacyBiosPlatform.h>
#include <Register/LocalApic.h>

typedef struct {
  UINT32  RegEax;
  UINT32  RegEbx;
  UINT32  RegEcx;
  UINT32  RegEdx;
} EFI_CPUID_REGISTER;

typedef struct {
  UINTN  Seg;
  UINTN  Bus;
  UINTN  Dev;
  UINTN  Fun;
} EFI_PCI_LOCATION;

typedef struct {
  EFI_HANDLE          Handle;
  PCI_TYPE01          ConfigHeader;
  EFI_PCI_LOCATION    Location;
} EFI_PCI_BRIDGE_INFO;

BOOLEAN            mLegacyMpInstalled = FALSE;

EFI_LEGACY_MP_TABLE_ENTRY_EXT_SYS_ADDR_SPACE_MAPPING mExtSysAddrMapping[] = {
  {0x80, 0x14, 0, 0, 0, 0x1000}
};

EFI_LEGACY_MP_TABLE_ENTRY_EXT_COMPAT_BUS_ADDR_SPACE_MODIFIER mExtBusAddrModifier[] = {
  {0x82, 0x8, 0x0, {0, 0}, 0},
  {0x82, 0x8, 0x0, {0, 0}, 1}
};

typedef struct {
  EFI_PROCESSOR_INFORMATION  Info;
  BOOLEAN                    Valid;
} LEGACY_MP_TABLE_PROCESSOR_INFORMATION;

/**
  Get Bus number from root bridge.

  @param  PciRootBridgeIo    This is a PciRootBridgeIo protocol
  @param  BusId                  Bus Number

  @retval EFI_SUCCESS         Retrieve the Minmal Bus number sucess;

**/
EFI_STATUS
GetBusNumber (
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *PciRootBridgeIo,
  OUT UINT8                                  *BusId
  )
{
  UINT8                                   *pData;  
      
  if (PciRootBridgeIo == NULL) {
    return EFI_ABORTED;
  }

  PciRootBridgeIo->Configuration (PciRootBridgeIo, (VOID **)&pData);
  while (!(*pData == ADDRESS_SPACE_DESCRIPTOR_END_TAG || \
         *pData != ADDRESS_SPACE_DESCRIPTOR_HEAD_TAG)) {

    if (*(UINT64 *)(pData + ADDRESS_SPACE_DESCRIPTOR_ADDR_LENGTH_OFFSET) != 0) {  
      if ((*(pData + ADDRESS_SPACE_DESCRIPTOR_RESOURCE_TYPE_OFFSET)) == ACPI_ADDRESS_SPACE_TYPE_BUS) {
        *BusId = (UINT8)(*(UINT64 *)(pData + ADDRESS_SPACE_DESCRIPTOR_ADDR_RANGE_MIN_OFFSET));
        break;
      }
    }
    pData += ADDRESS_SPACE_DESCRIPTOR_TOTAL_LENGTH;
  }

  return EFI_SUCCESS;
}


/**
  To check current Bus entry is for Legacy Bus or not

  @param  pEntry                 The data pointer of Entry

  @retval TRUE                   Current Bus entry is for Legacy Bus
  @retval FALSE                  Current Bus entry is not for Legacy Bus

**/
BOOLEAN
IsLegacyBus(
  IN EFI_LEGACY_MP_TABLE_ENTRY_BUS *pEntry
  )
{
  return (AsciiStrnCmp (pEntry->TypeString, EFI_LEGACY_MP_TABLE_ENTRY_BUS_STRING_ISA, sizeof(pEntry->TypeString)) == 0 ||
          AsciiStrnCmp (pEntry->TypeString, EFI_LEGACY_MP_TABLE_ENTRY_BUS_STRING_EISA, sizeof(pEntry->TypeString)) == 0 );
}


/**
  Assign a Bus ID for Legacy Bus

  @param  pStartEntry            The data pointer of Entry
  @param  BusCount               The number of Bus Entries

**/
VOID
AssignLegacyBusId (
  IN EFI_LEGACY_MP_TABLE_ENTRY_BUS          *pStartEntry,
  IN UINTN                                  BusCount
  )
{
  UINTN                                     Index;
  UINT8                                     LastId;
  EFI_LEGACY_MP_TABLE_ENTRY_BUS             *pEntry;

  LastId = 0;
  pEntry = pStartEntry;
  for (Index = 0; Index < BusCount; Index++) {
    if (!IsLegacyBus (pEntry)) {
      if (pEntry->Id > LastId ) {
        LastId = pEntry->Id;
      }
    }
    pEntry++;
  }

  pEntry = pStartEntry;
  for (Index = 0; Index < BusCount; Index++) {
    if (IsLegacyBus (pEntry)) {
      LastId++;
      pEntry->Id = LastId;
    }
    pEntry++;
  }

}

/**
  Insert a new bus entry into MP table in increasing order

  @param  pEntry                 The data pointer of Entry
  @param  TotalEntries         The Totol number of BusEntries that Built.
  @param  EntryToIns           One item is ready to insert the Bus Entry

  @retval EFI_SUCCESS            Update entry success

**/
EFI_STATUS
InsertBusEntries (
  IN     EFI_LEGACY_MP_TABLE_ENTRY_BUS          *BusStartEntry,
  IN OUT UINT8                                  *BusCount,
  IN     EFI_LEGACY_MP_TABLE_ENTRY_BUS          *EntryToIns
  )
{
  UINTN                                     Index;
  UINTN                                     Index2;
  UINT8                                     BuiltEntry;
  EFI_LEGACY_MP_TABLE_ENTRY_BUS             *PosToIns;

  BuiltEntry = *BusCount;
  (*BusCount)++;

  if (IsLegacyBus (EntryToIns)) {

    PosToIns = (BusStartEntry + BuiltEntry);

    CopyMem (
      PosToIns,
      EntryToIns,
      sizeof(EFI_LEGACY_MP_TABLE_ENTRY_BUS));

    return EFI_SUCCESS;

  }

  if (BuiltEntry == 0) {
    CopyMem (BusStartEntry, EntryToIns, sizeof(EFI_LEGACY_MP_TABLE_ENTRY_BUS));
    return EFI_SUCCESS;
  }

  for (Index = 0; Index < BuiltEntry; Index++) {
    PosToIns = (BusStartEntry + Index);
    if (IsLegacyBus (PosToIns)) {
      break;
    }

    if (EntryToIns->Id < PosToIns->Id) {
      break;
    }
  }

  for (Index2 = BuiltEntry; Index2 > Index; Index2--) {
    PosToIns = (BusStartEntry + Index2);
    CopyMem (PosToIns, PosToIns - 1, sizeof(EFI_LEGACY_MP_TABLE_ENTRY_BUS));
  }
  PosToIns = (BusStartEntry + Index2);
  CopyMem (PosToIns, EntryToIns, sizeof(EFI_LEGACY_MP_TABLE_ENTRY_BUS));

  return EFI_SUCCESS;
}

/**
  Creat one Bus Item into BusEntry

  Arguments:
  @param  pEntry                  The data pointer of Entry
  @param  TotalEntries          The Totol number of BusEntries that Built.
  @param  BusId                  Bus Number
  @param  DeviceType           A device type obtains from ClassCode

  @retval EFI_SUCCESS            Update entry success

**/
EFI_STATUS
CreateOneBusEntry (
  IN OUT EFI_LEGACY_MP_TABLE_ENTRY_BUS          *BusStartEntry,
  IN OUT UINT8                                  *BusCount,
  IN     UINT8                                  BusId,
  IN     UINT8                                  DeviceType
)
{
  EFI_LEGACY_MP_TABLE_ENTRY_BUS           TempEntry;

  TempEntry.EntryType = EFI_LEGACY_MP_TABLE_ENTRY_TYPE_BUS;
  TempEntry.Id        = BusId;

  switch (DeviceType) {

    case PCI_CLASS_BRIDGE_ISA:
      //
      // ISA
      //
      CopyMem (
        TempEntry.TypeString,
        EFI_LEGACY_MP_TABLE_ENTRY_BUS_STRING_ISA,
        sizeof(TempEntry.TypeString));
      //
      // Assign 0 for legacy bus, it will be re-assign to proper bus id at the end
      //
      TempEntry.Id = 0;
      break;

    case PCI_CLASS_BRIDGE_EISA:
      //
      // EISA
      //
      CopyMem (
        TempEntry.TypeString,
        EFI_LEGACY_MP_TABLE_ENTRY_BUS_STRING_EISA,
        sizeof(TempEntry.TypeString));

      TempEntry.Id = 0;
      break;

    case PCI_CLASS_BRIDGE_HOST:
    case PCI_CLASS_BRIDGE_P2P:
      //
      // P2P
      //
      CopyMem (
        TempEntry.TypeString,
        EFI_LEGACY_MP_TABLE_ENTRY_BUS_STRING_PCI,
        sizeof(TempEntry.TypeString));

      break;

    default:
      //
      // Unsupported
      //
      return EFI_UNSUPPORTED;

    }

    InsertBusEntries (BusStartEntry, BusCount, &TempEntry);

  return EFI_SUCCESS;
}


/**
  Update the Processor Entries, Report Cpu in order, by Modern or Legacy ordering

  @param  pEntry                 The data pointer of Entry
  @param  TotalEntries           Return the total entry number in MP Table
  @param  ApicBaseAddress        The base address of Local Apic

  @retval EFI_SUCCESS            Update entry success

**/
EFI_STATUS
UpdateProcessorEntries (
  IN OUT UINT8                                  **pEntry,
  IN OUT UINTN                                  *TotalEntries,
  IN UINT32                                     ApicBaseAddress
  )
{
  EFI_STATUS                             Status;
  EFI_LEGACY_MP_TABLE_ENTRY_PROCESSOR    *ProcessEntry;
  EFI_CPUID_REGISTER                     CpuidRegisters;
  UINT32                                 ApicVersion32;
  UINT8                                  Version;
  EFI_MP_SERVICES_PROTOCOL               *MpService;
  UINTN                                  NumberOfCPUs;
  UINTN                                  NumberOfEnabledCPUs;
  EFI_PROCESSOR_INFORMATION              MpContext;
  EFI_PROCESSOR_INFORMATION              *MpArray;
  UINTN                                  ArraySize;
  UINTN                                  TotalCpu;
  UINTN                                  Index;
  UINTN                                  VarSize;
  UINT8                                  LegacyModern;
  EFI_GUID                               SystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;
  LEGACY_MP_TABLE_PROCESSOR_INFORMATION  *Cpu;
  LEGACY_MP_TABLE_PROCESSOR_INFORMATION  *pData;
  UINT32                                 MaxPkg;
  UINT32                                 MaxCore;
  UINT32                                 MaxThread;
  UINT32                                 PkgIdx;
  UINT32                                 CoreIdx;

  NumberOfEnabledCPUs = 0;
  NumberOfCPUs        = 0;
  VarSize             = 1;
  TotalCpu            = 0;
  ArraySize           = 0;
  MaxPkg              = 0;
  MaxCore             = 0;
  MaxThread           = 0;
  MpArray             = NULL;
  Cpu                 = NULL;

  ProcessEntry = (EFI_LEGACY_MP_TABLE_ENTRY_PROCESSOR *)(*pEntry);

  Status = gBS->LocateProtocol (
                  &gEfiMpServiceProtocolGuid,
                  NULL,
                  (VOID **)&MpService);

  if (Status != EFI_SUCCESS) {
    return Status;
  }

  //
  // Determine the number of processors
  //
  Status = MpService->GetNumberOfProcessors (MpService, &NumberOfCPUs, &NumberOfEnabledCPUs);

  if (Status != EFI_SUCCESS) {
    return Status;
  }

  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  sizeof(EFI_PROCESSOR_INFORMATION) * NumberOfCPUs,
                  (VOID **)&MpArray);

  if (Status != EFI_SUCCESS) {
    return Status;
  }

  ZeroMem (MpArray, sizeof(EFI_PROCESSOR_INFORMATION) * NumberOfCPUs);

  ApicVersion32 = *(UINT32 *)(UINTN)(ApicBaseAddress + APIC_REGISTER_APIC_VERSION_OFFSET);
  Version = (UINT8)(ApicVersion32 & 0xFF);
  AsmCpuid (CPUID_VERSION_INFO, &CpuidRegisters.RegEax, &CpuidRegisters.RegEbx, &CpuidRegisters.RegEcx, &CpuidRegisters.RegEdx);

  for (Index = 0; Index < NumberOfCPUs; Index++) {

    Status = MpService->GetProcessorInfo (
                          MpService,
                          Index,
                          &MpContext);

    if (EFI_ERROR(Status)) {
      goto Entry_Error;
    }

    //
    // Collecting all processor's infomation
    //
    CopyMem (
      &MpArray[Index],
      &MpContext,
      sizeof(EFI_PROCESSOR_INFORMATION));
    //
    // Determine maximum Package, Core, Thread number by Location info
    //
    if (MpContext.Location.Package > MaxPkg) {
      MaxPkg = MpContext.Location.Package;
    }
    if (MpContext.Location.Core > MaxCore) {
      MaxCore = MpContext.Location.Core;
    }
    if (MpContext.Location.Thread > MaxThread) {
      MaxThread = MpContext.Location.Thread;
    }
  }

  MaxPkg++;
  MaxCore++;
  MaxThread++;

  ArraySize = (UINTN)(sizeof(LEGACY_MP_TABLE_PROCESSOR_INFORMATION)*(MaxPkg)*(MaxCore)*(MaxThread));

  Status = gBS->AllocatePool (EfiBootServicesData, ArraySize, (void *)&Cpu);
  if (EFI_ERROR(Status)) {
    goto Entry_Error;
  }

  ZeroMem (Cpu, ArraySize);

  for (Index = 0; Index < NumberOfCPUs; Index++) {

    pData = Cpu + (MpArray[Index].Location.Package * MaxCore * MaxThread) \
                + (MpArray[Index].Location.Core * MaxThread) \
                + MpArray[Index].Location.Thread;

    if (pData->Valid) {
      //
      // Skip if there is a valid info exist in same location.
      //
      continue;
    }
    CopyMem (&pData->Info, 
             &MpArray[Index], 
             sizeof(EFI_PROCESSOR_INFORMATION));
    pData->Valid = TRUE;
  }

  //
  // Determine what ordering type is used, by variable "MpTableCpuOrdering".
  // If platform does not has this variable, handle it as  Modern ordering.
  //
  Status = gRT->GetVariable (
                  L"MpTableCpuOrdering",
                  &SystemConfigurationGuid,
                  NULL,
                  &VarSize,
                  &LegacyModern);

  if (LegacyModern == MADT_MODERN_MODE || Status != EFI_SUCCESS) {
    //
    // By Modern ordering
    //
    for (PkgIdx = 0; PkgIdx < MaxPkg; PkgIdx++) {
      for (CoreIdx = 0; CoreIdx < MaxCore; CoreIdx++) {

        pData = Cpu + (PkgIdx*MaxCore*MaxThread) + (CoreIdx*MaxThread);

        if (!(pData->Info.StatusFlag & PROCESSOR_HEALTH_STATUS_BIT) || !pData->Valid) {
          //
          // Don't report unhealthy processor or data is invalid
          //
          continue;
        }

        ProcessEntry->Ver = Version;

        CopyMem (
          &(ProcessEntry->Signature),
          &(CpuidRegisters.RegEax),
          sizeof(UINT32));

        CopyMem (
          &(ProcessEntry->Features),
          &(CpuidRegisters.RegEdx),
          sizeof(UINT32));

        ProcessEntry->EntryType = EFI_LEGACY_MP_TABLE_ENTRY_TYPE_PROCESSOR;

        ProcessEntry->Id = (UINT8)pData->Info.ProcessorId;
        ProcessEntry->Flags.Bsp = pData->Info.StatusFlag & PROCESSOR_AS_BSP_BIT;
        ProcessEntry->Flags.Enabled = (pData->Info.StatusFlag & PROCESSOR_ENABLED_BIT) >> 1;

        ProcessEntry++;
        TotalCpu++;
      }
    }
  } else {
    //
    // By Legacy ordering
    //
    for (CoreIdx = 0; CoreIdx < MaxCore; CoreIdx++) {
      for (PkgIdx = 0; PkgIdx < MaxPkg; PkgIdx++) {
        
        pData = Cpu + (PkgIdx*MaxCore*MaxThread) + (CoreIdx*MaxThread);

        if (!(pData->Info.StatusFlag & PROCESSOR_HEALTH_STATUS_BIT) || !pData->Valid) {
          //
          // Don't report unhealthy processor or data is invalid
          //
          continue;
        }

        ProcessEntry->Ver = Version;

        CopyMem (
          &(ProcessEntry->Signature),
          &(CpuidRegisters.RegEax),
          sizeof(UINT32));

        CopyMem (
          &(ProcessEntry->Features),
          &(CpuidRegisters.RegEdx),
          sizeof(UINT32));

        ProcessEntry->EntryType = EFI_LEGACY_MP_TABLE_ENTRY_TYPE_PROCESSOR;

        ProcessEntry->Id = (UINT8)pData->Info.ProcessorId;
        ProcessEntry->Flags.Bsp = pData->Info.StatusFlag & PROCESSOR_AS_BSP_BIT;
        ProcessEntry->Flags.Enabled = (pData->Info.StatusFlag & PROCESSOR_ENABLED_BIT) >> 1;

        ProcessEntry++;
        TotalCpu++;
      }
    }
  }

  *TotalEntries += TotalCpu;
  (*pEntry) = (UINT8 *)ProcessEntry;

  Status = EFI_SUCCESS;

Entry_Error:

  if (MpArray != NULL) {
    FreePool (MpArray);
  }
  if (Cpu != NULL) {
    FreePool (Cpu);
  }

  return Status;
}

/**
  Update the Bus Entries

  Arguments:
  @param  pEntry                 The data pointer of Entry
  @param  TotalEntries           Return the total entry number in MP Table
  @param  HandleCount            The total number of Handle with Pci IO protocol
  @param  HandleBuffer           The Buffer's pointer

  @retval EFI_SUCCESS            Update entry success

**/
EFI_STATUS
UpdateBusEntries (
  IN OUT UINT8                                  **pEntry,
  IN OUT UINTN                                  *TotalEntries,
  IN     UINTN                                  PciBridgeInfoCount,
  IN     EFI_PCI_BRIDGE_INFO                    *PciBridgeInfoBuffer,
  IN     UINTN                                  RootBridgeHandleCount,
  IN     EFI_HANDLE                             *RootBridgeHandleBuffer
  )
{
  EFI_STATUS                              Status;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL         *PciRootBridgeIo;
  UINTN                                   Index;
  UINT8                                   DeviceType;
  UINT8                                   BusId;    
  UINT8                                   BusCount;
  EFI_LEGACY_MP_TABLE_ENTRY_BUS           *BusStartEntry;

  BusCount = 0;
  BusStartEntry = (EFI_LEGACY_MP_TABLE_ENTRY_BUS *)(*pEntry);

  if (PciBridgeInfoBuffer != NULL) {
    for (Index = 0; Index < PciBridgeInfoCount; Index++) {
      
      if (PciBridgeInfoBuffer[Index].ConfigHeader.Hdr.ClassCode[2] != PCI_CLASS_BRIDGE) {
        continue;
      }
      
      if (PciBridgeInfoBuffer[Index].ConfigHeader.Hdr.ClassCode[1] == PCI_CLASS_BRIDGE_HOST) {
        continue;
      }

      DeviceType = PciBridgeInfoBuffer[Index].ConfigHeader.Hdr.ClassCode[1];
      BusId      = PciBridgeInfoBuffer[Index].ConfigHeader.Bridge.SecondaryBus;

      CreateOneBusEntry(BusStartEntry, &BusCount, BusId, DeviceType);
    }
  }

  if (RootBridgeHandleBuffer != NULL) {
    for (Index = 0; Index < RootBridgeHandleCount; Index++) {
        
      Status = gBS->HandleProtocol (
                      RootBridgeHandleBuffer[Index],
                      &gEfiPciRootBridgeIoProtocolGuid,
                      (VOID **)&PciRootBridgeIo);
      if (EFI_ERROR (Status)) {
        continue;
      }
      
      Status = GetBusNumber (PciRootBridgeIo, &BusId);
      if (EFI_ERROR(Status)) {
        continue;
      }
  
      DeviceType = PCI_CLASS_BRIDGE_HOST;
      CreateOneBusEntry(BusStartEntry, &BusCount, BusId, DeviceType);
    }  
  }  
  
  //
  // Re-assign to proper bus id
  //
  AssignLegacyBusId (BusStartEntry, BusCount);

  *TotalEntries += BusCount;
  (*pEntry) = (UINT8 *)(BusStartEntry + BusCount);

  return EFI_SUCCESS;
}

/**
  Update the I/O APIC Entries

  @param  pEntry                 The data pointer of Entry
  @param  TotalEntries           Return the total entry number in MP Table

  @retval EFI_SUCCESS            Update entry success

**/
EFI_STATUS
UpdateIoApicEntries (
  IN OUT UINT8                                  **pEntry,
  IN OUT UINTN                                  *TotalEntries,
  IN UINTN                                      NumOfIoApic,
  IN EFI_LEGACY_MP_TABLE_ENTRY_IOAPIC           *pIoApicEntry
  )
{
  EFI_LEGACY_MP_TABLE_ENTRY_IOAPIC        *IoApicEntry;
  UINTN                                   Index;

  if (pIoApicEntry == NULL) {
    return EFI_SUCCESS;
  }

  IoApicEntry = (EFI_LEGACY_MP_TABLE_ENTRY_IOAPIC *)(*pEntry);

  for (Index = 0; Index < NumOfIoApic; Index++) {

    if (pIoApicEntry->EntryType != EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IOAPIC) {
      pIoApicEntry++;
      continue;
    }

    CopyMem (
      IoApicEntry,
      pIoApicEntry,
      sizeof(EFI_LEGACY_MP_TABLE_ENTRY_IOAPIC));

    pIoApicEntry++;
    (*TotalEntries)++;
    IoApicEntry++;
  }

  *pEntry = (UINT8 *)IoApicEntry;

  return EFI_SUCCESS;
}

/**
  Check the PCI bridge exist or not.

  @param  pIoApicIntEntrySlot Slot entry
  @param  BusStartEntries       The pointer of first Bus entry
  @param  PciBridgeInfoCount  Array size of the PciBridgeInfo array
  @param  PciBridgeInfoBuffer  A array carrying on PciBridgeInfo.
  @param  Index                     Position in the PciBridgeInfo array.

  @retval EFI_SUCCESS            Find a legacy bus entry
  @retval EFI_NOT_FOUND       does not find a legacy bus entry
**/
EFI_STATUS
CheckDeviceStatus (
  IN MP_TABLE_ENTRY_IO_INT_SLOT                 *pIoApicIntEntrySlot,
  IN UINTN                                      PciBridgeInfoCount,
  IN EFI_PCI_BRIDGE_INFO                        *PciBridgeInfoBuffer,
  OUT UINTN                                     *PciBridgeInfoIndex
  )
{
  UINTN       FirstCell, LastCell;
  UINTN       HandleAddress, EntryAddress;
  UINTN       CurrentMid, FormerMid;

  EntryAddress  = EFI_PCI_ADDRESS (pIoApicIntEntrySlot->BridgeBus, pIoApicIntEntrySlot->BridgeDev, pIoApicIntEntrySlot->BridgeFunc, 0);
  
  LastCell = PciBridgeInfoCount - 1;
  FirstCell = 0;
  CurrentMid = (FirstCell + LastCell) / 2;
  FormerMid = FirstCell;
  
  do {
    FormerMid = CurrentMid;
    HandleAddress = EFI_PCI_ADDRESS (PciBridgeInfoBuffer[CurrentMid].Location.Bus, PciBridgeInfoBuffer[CurrentMid].Location.Dev, PciBridgeInfoBuffer[CurrentMid].Location.Fun, 0);
    
    if (EntryAddress == HandleAddress) {
      *PciBridgeInfoIndex = CurrentMid;
      return EFI_SUCCESS;
    }
    else if(EntryAddress > HandleAddress) {
      FirstCell = CurrentMid + 1;
    }
    else {
      LastCell = CurrentMid - 1;
    }
    
    CurrentMid = (FirstCell + LastCell) / 2;
  } while (FormerMid != CurrentMid);

  return EFI_NOT_FOUND;
}

/**
  Search the first legacy Bus entry

  @param  BusStartEntries       The pointer of first Bus entry
  @param  PciBridgeInfoCount  This is a array size of the PciBridgeInfo array
  @param  PciBridgeInfoBuffer  This is a array carrying on PciBridgeInfo.
  @param  LegacyBusId           The bus id of Legacy bus.

  @retval EFI_SUCCESS            Find a legacy bus entry
  @retval EFI_NOT_FOUND       does not find a legacy bus entry
**/
EFI_STATUS
LocateLegacyBus (
  IN  EFI_LEGACY_MP_TABLE_ENTRY_BUS              *BusStartEntries,
  IN  UINTN                                      PciBridgeInfoCount,
  IN  EFI_PCI_BRIDGE_INFO                        *PciBridgeInfoBuffer,
  OUT UINT8                                      *LegacyBusId
)
{
  UINTN                                   Index;
  EFI_LEGACY_MP_TABLE_ENTRY_BUS           *BusEntries;
  
  //
  // Legacy Bus
  //
  for (Index = 0; Index < PciBridgeInfoCount; Index++) {

    if (PciBridgeInfoBuffer[Index].ConfigHeader.Hdr.ClassCode[2] != PCI_CLASS_BRIDGE) {
      continue;
    }

    BusEntries = BusStartEntries;
    switch (PciBridgeInfoBuffer[Index].ConfigHeader.Hdr.ClassCode[1]) {

    case PCI_CLASS_BRIDGE_EISA:
      while (BusEntries->EntryType == EFI_LEGACY_MP_TABLE_ENTRY_TYPE_BUS) {
        if (AsciiStrnCmp (BusEntries->TypeString, EFI_LEGACY_MP_TABLE_ENTRY_BUS_STRING_EISA, sizeof(BusEntries->TypeString)) == 0) {
          *LegacyBusId = BusEntries->Id;
          return EFI_SUCCESS;
        }
        BusEntries++;
      }
      break;

    case PCI_CLASS_BRIDGE_ISA:
      while (BusEntries->EntryType == EFI_LEGACY_MP_TABLE_ENTRY_TYPE_BUS) {
        if (AsciiStrnCmp (BusEntries->TypeString, EFI_LEGACY_MP_TABLE_ENTRY_BUS_STRING_ISA, sizeof(BusEntries->TypeString)) == 0) {
          *LegacyBusId = BusEntries->Id;
          return EFI_SUCCESS;
          break;
        }
        BusEntries++;
      }
      break;

    default:
      //
      // Non-Legacy
      //
      break;

    }
  }

  return EFI_NOT_FOUND;
}


/**
  Update the I/O Interrupt Assignment Entries

  @param  pEntry                             The data pointer of Entry
  @param  TotalEntries                     Return the total entry number in MP Table
  @param  BusStartEntries                 The pointer of first Bus entry
  @param  PciBridgeInfoCount            This is a array size of the PciBridgeInfo array
  @param  PciBridgeInfoBuffer            This is a array carrying on PciBridgeInfo.
  @param  NumOfIoApic                     The quantity of IoApic entry
  @param  pIoApicEntry                       The array of IoApic Entry 
  @param  NumOfIoApicIntLegacy         The quantity of Interrupt Entry on legacy bus.
  @param  pIoApicIntEntryLegacyHead   The array of Interrupt Entry on legacy bus.
  @param  NumOfIoApicInt                   The quantity of Interrupt Entry on the first bus.
  @param  pIoApicIntEntryHead            The array of Interrupt Entry on the first bus.
  @param  NumOfIoApicIntSlot             The quantity of Interrupt Entry on the slot 
  @param  pIoApicIntEntrySlotHead       The array of Interrupt Entry on the slot 

  @retval EFI_SUCCESS            Update entry success
  
**/
EFI_STATUS
UpdateIoApicIntEntries (
  IN OUT UINT8                                  **pEntry,
  IN OUT UINTN                                  *TotalEntries,
  IN EFI_LEGACY_MP_TABLE_ENTRY_BUS              *BusStartEntries,
  IN UINTN                                      PciBridgeInfoCount,
  IN EFI_PCI_BRIDGE_INFO                        *PciBridgeInfoBuffer,
  IN UINTN                                      NumOfIoApic,
  IN EFI_LEGACY_MP_TABLE_ENTRY_IOAPIC           *pIoApicEntry,
  IN UINTN                                      NumOfIoApicIntLegacy,
  IN EFI_LEGACY_MP_TABLE_ENTRY_IO_INT           *pIoApicIntEntryLegacyHead,
  IN UINTN                                      NumOfIoApicInt,
  IN EFI_LEGACY_MP_TABLE_ENTRY_IO_INT           *pIoApicIntEntryHead,
  IN UINTN                                      NumOfIoApicIntSlot,
  IN MP_TABLE_ENTRY_IO_INT_SLOT                *pIoApicIntEntrySlotHead
  )
{
  EFI_STATUS                              LegacyBusStatus, DeviceStatus;
  EFI_LEGACY_MP_TABLE_ENTRY_IO_INT        *IoApicIntEntry;
  EFI_LEGACY_MP_TABLE_ENTRY_IO_INT        *pIoApicIntEntry;
  MP_TABLE_ENTRY_IO_INT_SLOT              *pIoApicIntEntrySlot;
  UINTN                                   EntryIndex;
  UINTN                                   PciBridgeInfoIndex;
  UINT8                                   LegacyBusId;
  EFI_LEGACY_MP_TABLE_ENTRY_IO_INT        *pIoApicIntEntryLegacy;
  UINTN                                   IoApicIndex;
  UINTN                                   CommonCount, CommonIndex;
  UINTN                                   FormerAddress, CurrentAddress;

  LegacyBusId = 0;

  if (pIoApicEntry == NULL || pIoApicIntEntryLegacyHead == NULL || \
    pIoApicIntEntryHead == NULL || pIoApicIntEntrySlotHead == NULL) {
    return EFI_SUCCESS;
  }

  IoApicIntEntry = (EFI_LEGACY_MP_TABLE_ENTRY_IO_INT *)(*pEntry);

  //
  // Legacy Bus
  //
  LegacyBusStatus = LocateLegacyBus (
                      BusStartEntries, 
                      PciBridgeInfoCount, 
                      PciBridgeInfoBuffer, 
                      &LegacyBusId);
  
  for (IoApicIndex = 0; IoApicIndex < NumOfIoApic; IoApicIndex++) {
    pIoApicIntEntryLegacy = pIoApicIntEntryLegacyHead;
    if (!EFI_ERROR(LegacyBusStatus) && NumOfIoApicIntLegacy > 0) {      
      for (EntryIndex = 0; EntryIndex < NumOfIoApicIntLegacy; EntryIndex++) {
        if (pIoApicIntEntryLegacy->EntryType != EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT || \
            pIoApicIntEntryLegacy->DestApicId != pIoApicEntry[IoApicIndex].Id) {
            
          pIoApicIntEntryLegacy++;
          continue;
        }
            
        CopyMem (
          IoApicIntEntry,
          pIoApicIntEntryLegacy,
          sizeof(EFI_LEGACY_MP_TABLE_ENTRY_IO_INT));

        IoApicIntEntry->SourceBusId = LegacyBusId;
        pIoApicIntEntryLegacy++;
        (*TotalEntries)++;
        IoApicIntEntry++;
      }

    }
    (*pEntry) = (UINT8 *)(IoApicIntEntry);

    //
    // Built-in
    //
    pIoApicIntEntry = pIoApicIntEntryHead;
    for (EntryIndex = 0; EntryIndex < NumOfIoApicInt; EntryIndex++) {
      if (pIoApicIntEntry->EntryType != EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT || \
          pIoApicIntEntry->DestApicId != pIoApicEntry[IoApicIndex].Id) {
          
        pIoApicIntEntry++;
        continue;
      }

      CopyMem (
        IoApicIntEntry,
        pIoApicIntEntry,
        sizeof(EFI_LEGACY_MP_TABLE_ENTRY_IO_INT));

      pIoApicIntEntry++;
      (*TotalEntries)++;
      IoApicIntEntry++;
    }

    (*pEntry) = (UINT8 *)(IoApicIntEntry);

    //
    // Pci Slot
    //
    if (NumOfIoApicIntSlot == 0) {
      return EFI_SUCCESS;
    }

    pIoApicIntEntrySlot = pIoApicIntEntrySlotHead;
    
    for (EntryIndex = 0; EntryIndex < NumOfIoApicIntSlot; EntryIndex += CommonCount) {

      CommonCount = 1;
      
      if (pIoApicIntEntrySlot->SlotIrqTable.DestApicId != pIoApicEntry[IoApicIndex].Id) {
        pIoApicIntEntrySlot++;
        continue;
      }

      DeviceStatus = CheckDeviceStatus (pIoApicIntEntrySlot, PciBridgeInfoCount, PciBridgeInfoBuffer, &PciBridgeInfoIndex);
           
      FormerAddress = (UINTN) EFI_PCI_ADDRESS (pIoApicIntEntrySlot[0].BridgeBus, pIoApicIntEntrySlot[0].BridgeDev, pIoApicIntEntrySlot[0].BridgeFunc, pIoApicIntEntrySlot[0].SlotIrqTable.SourceBusId);
      CurrentAddress = FormerAddress;
      while ((EntryIndex + CommonCount < NumOfIoApicIntSlot) && (FormerAddress == CurrentAddress)) {
        CurrentAddress = (UINTN) EFI_PCI_ADDRESS (pIoApicIntEntrySlot[CommonCount].BridgeBus, pIoApicIntEntrySlot[CommonCount].BridgeDev, pIoApicIntEntrySlot[CommonCount].BridgeFunc, pIoApicIntEntrySlot[CommonCount].SlotIrqTable.SourceBusId);
        if (FormerAddress == CurrentAddress) {
          CommonCount++;
        }
      }
      
      if (!EFI_ERROR(DeviceStatus)) {
        for (CommonIndex = 0; CommonIndex < CommonCount; CommonIndex++) {
          pIoApicIntEntrySlot->SlotIrqTable.SourceBusId = PciBridgeInfoBuffer[PciBridgeInfoIndex].ConfigHeader.Bridge.SecondaryBus;

          CopyMem (
            IoApicIntEntry,
            &pIoApicIntEntrySlot->SlotIrqTable,
            sizeof(EFI_LEGACY_MP_TABLE_ENTRY_IO_INT));

          (*TotalEntries)++;
          IoApicIntEntry++;
          pIoApicIntEntrySlot++;
        }
        
      }
      else {
        pIoApicIntEntrySlot += CommonCount;
      }

    }
  }

  (*pEntry) = (UINT8 *)(IoApicIntEntry);

  return EFI_SUCCESS;
}

/**
  Update the Local Interrupt Assignment Entries

  @param  pEntry                 The data pointer of Entry
  @param  TotalEntries           Return the total entry number in MP Table

  @retval EFI_SUCCESS            Update entry success

**/
EFI_STATUS
UpdateLocalApicIntEntries (
  IN OUT UINT8                                  **pEntry,
  IN OUT UINTN                                  *TotalEntries,
  IN UINTN                                      NumOfLocalApicInt,
  IN EFI_LEGACY_MP_TABLE_ENTRY_LOCAL_INT        *pLocalApicIntEntry
  )
{
  EFI_LEGACY_MP_TABLE_ENTRY_LOCAL_INT     *LocalApicIntEntry;
  UINTN                                   Index;

  if (pLocalApicIntEntry == NULL) {
    return EFI_SUCCESS;
  }

  LocalApicIntEntry = (EFI_LEGACY_MP_TABLE_ENTRY_LOCAL_INT *)(*pEntry);

  for (Index = 0; Index < NumOfLocalApicInt; Index++) {

    if (pLocalApicIntEntry->EntryType != EFI_LEGACY_MP_TABLE_ENTRY_TYPE_LOCAL_INT) {
      pLocalApicIntEntry++;
      continue;
    }

    CopyMem (
      LocalApicIntEntry,
      pLocalApicIntEntry,
      sizeof(EFI_LEGACY_MP_TABLE_ENTRY_LOCAL_INT));

    pLocalApicIntEntry++;
    (*TotalEntries)++;
    LocalApicIntEntry++;

  }

  (*pEntry) = (UINT8 *)(LocalApicIntEntry);

  return EFI_SUCCESS;
}

/**
  Update the extended System Address Space Mapping Entries

  @param  pEntry                 The data pointer of Entry
  @param  HandleCount            The total number of Handles with Pci IO protocol
  @param  HandleBuffer           The Buffer's pointer

  @retval EFI_SUCCESS            Update entry success

**/
EFI_STATUS
UpdateExtSysAddrMappingEntries (
  IN OUT UINT8                                  **pEntry,
  IN UINTN                                      BridgeHandleCount,
  IN EFI_HANDLE                                 *BridgeHandleBuffer
  )
{
  EFI_STATUS                              Status;
  UINTN                                   Index;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL         *PciRootBridgeIo;
  UINT8                                   *pData;
  UINTN                                   NumOfExtSysAddrMapping;
  EFI_LEGACY_MP_TABLE_ENTRY_EXT_SYS_ADDR_SPACE_MAPPING *pExtBusAddrMapping;
  EFI_LEGACY_MP_TABLE_ENTRY_EXT_SYS_ADDR_SPACE_MAPPING *TempEntry;

  TempEntry = (EFI_LEGACY_MP_TABLE_ENTRY_EXT_SYS_ADDR_SPACE_MAPPING *)(*pEntry);

  NumOfExtSysAddrMapping = sizeof(mExtSysAddrMapping) / sizeof(mExtSysAddrMapping[0]);
  pExtBusAddrMapping = &mExtSysAddrMapping[0];

  for (Index = 0; Index < NumOfExtSysAddrMapping; Index++) {

    if (pExtBusAddrMapping->EntryType != EFI_LEGACY_MP_TABLE_ENTRY_EXT_TYPE_SYS_ADDR_SPACE_MAPPING) {
      pExtBusAddrMapping++;
      continue;
    }

    CopyMem (
      TempEntry,
      pExtBusAddrMapping,
      sizeof(EFI_LEGACY_MP_TABLE_ENTRY_EXT_SYS_ADDR_SPACE_MAPPING));
    pExtBusAddrMapping++;
    TempEntry++;
  }

  for (Index = 0; Index < BridgeHandleCount; Index++) {

    Status = gBS->HandleProtocol (
                    BridgeHandleBuffer[Index],
                    &gEfiPciRootBridgeIoProtocolGuid,
                    (VOID **)&PciRootBridgeIo);

    if (EFI_ERROR (Status)) {
      return EFI_NOT_FOUND;
    }

    PciRootBridgeIo->Configuration (PciRootBridgeIo, (VOID **)&pData);
    while (!(*pData == ADDRESS_SPACE_DESCRIPTOR_END_TAG || \
            *pData != ADDRESS_SPACE_DESCRIPTOR_HEAD_TAG)) {

      if (*(UINT64 *)(pData + ADDRESS_SPACE_DESCRIPTOR_ADDR_LENGTH_OFFSET) != 0) {
        
        TempEntry->EntryType = EFI_LEGACY_MP_TABLE_ENTRY_EXT_TYPE_SYS_ADDR_SPACE_MAPPING;
        TempEntry->Length = sizeof(EFI_LEGACY_MP_TABLE_ENTRY_EXT_SYS_ADDR_SPACE_MAPPING);
        Status = GetBusNumber (PciRootBridgeIo, &TempEntry->BusId);
        if (EFI_ERROR(Status)) {
          continue;
        }

        switch (*(pData + ADDRESS_SPACE_DESCRIPTOR_RESOURCE_TYPE_OFFSET)) {

        case ACPI_ADDRESS_SPACE_TYPE_MEM:
          TempEntry->AddressType = EfiLegacyMpTableEntryExtSysAddrSpaceMappingMemory;
          break;

        case ACPI_ADDRESS_SPACE_TYPE_IO:
          TempEntry->AddressType = EfiLegacyMpTableEntryExtSysAddrSpaceMappingIo;
          break;
          
        default:
          pData += ADDRESS_SPACE_DESCRIPTOR_TOTAL_LENGTH;
          continue;

        }
        TempEntry->AddressBase = *(UINT64 *)(pData + ADDRESS_SPACE_DESCRIPTOR_ADDR_RANGE_MIN_OFFSET);
        TempEntry->AddressLength = *(UINT64 *)(pData + ADDRESS_SPACE_DESCRIPTOR_ADDR_LENGTH_OFFSET);
        TempEntry++;

      }
      pData += ADDRESS_SPACE_DESCRIPTOR_TOTAL_LENGTH;

    }
  }

  (*pEntry) = (UINT8 *)(TempEntry);

  return EFI_SUCCESS;

}

EFI_STATUS
CreateOneExtBusHierarchyDscEntry (
  IN OUT EFI_LEGACY_MP_TABLE_ENTRY_EXT_BUS_HIERARCHY **TempEntry,
  IN     UINT8                                       ParentBus,
  IN     UINT8                                       BusId
)
{
  (*TempEntry)->EntryType = EFI_LEGACY_MP_TABLE_ENTRY_EXT_TYPE_BUS_HIERARCHY;
  (*TempEntry)->Length = sizeof(EFI_LEGACY_MP_TABLE_ENTRY_EXT_BUS_HIERARCHY);
  (*TempEntry)->ParentBus = (UINT8)ParentBus;
  (*TempEntry)->BusId = (UINT8)BusId;
  (*TempEntry)->BusInfo.SubtractiveDecode = 1;

  (*TempEntry)++;

  return EFI_SUCCESS;
}

/**
  Update the extended Bus Hierarchy Descriptor Entry

  @param  pEntry                 The data pointer of Entry
  @param  BusStartEntries        The pointer of first Bus entry
  @param  HandleCount            The total number of Handles with Pci IO protocol
  @param  HandleBuffer           The Buffer's pointer

  @retval EFI_SUCCESS            Update entry success

**/
EFI_STATUS
UpdateExtBusHierarchyDscEntries (
  IN OUT UINT8                                  **pEntry,
  IN EFI_LEGACY_MP_TABLE_ENTRY_BUS              *BusStartEntries,
  IN UINTN                                      PciBridgeInfoCount,
  IN EFI_PCI_BRIDGE_INFO                        *PciBridgeInfoBuffer
  )
{
  UINTN                                       Index;
  EFI_LEGACY_MP_TABLE_ENTRY_EXT_BUS_HIERARCHY *TempEntry;
  EFI_LEGACY_MP_TABLE_ENTRY_BUS               *BusEntries;

  if (PciBridgeInfoCount == 0) {
    return EFI_SUCCESS;
  }

  TempEntry = (EFI_LEGACY_MP_TABLE_ENTRY_EXT_BUS_HIERARCHY *)(*pEntry);

  for (Index = 0; Index < PciBridgeInfoCount; Index++) {
    
    if (PciBridgeInfoBuffer[Index].ConfigHeader.Hdr.ClassCode[2] != PCI_CLASS_BRIDGE) {
      continue;
    }

    BusEntries = BusStartEntries;
    switch (PciBridgeInfoBuffer[Index].ConfigHeader.Hdr.ClassCode[1]) {

    case PCI_CLASS_BRIDGE_EISA:
      while (BusEntries->EntryType == EFI_LEGACY_MP_TABLE_ENTRY_TYPE_BUS) {
        if (AsciiStrnCmp (BusEntries->TypeString, EFI_LEGACY_MP_TABLE_ENTRY_BUS_STRING_EISA, sizeof(BusEntries->TypeString)) == 0) {
          CreateOneExtBusHierarchyDscEntry (
              &TempEntry, 
              (UINT8)PciBridgeInfoBuffer[Index].Location.Bus,
              (UINT8) BusEntries->Id
              );
          break;
        }
        BusEntries++;
      }
      break;

    case PCI_CLASS_BRIDGE_ISA:
      while (BusEntries->EntryType == EFI_LEGACY_MP_TABLE_ENTRY_TYPE_BUS) {
        if (AsciiStrnCmp (BusEntries->TypeString, EFI_LEGACY_MP_TABLE_ENTRY_BUS_STRING_ISA, sizeof(BusEntries->TypeString)) == 0) {
          CreateOneExtBusHierarchyDscEntry (
              &TempEntry, 
              (UINT8)PciBridgeInfoBuffer[Index].Location.Bus,
              (UINT8) BusEntries->Id
              );
          break;
        }
        BusEntries++;
      }
      break;

    case PCI_CLASS_BRIDGE_P2P:

      if (PciBridgeInfoBuffer[Index].ConfigHeader.Hdr.ClassCode[0] == 0x01) {
        CreateOneExtBusHierarchyDscEntry (
            &TempEntry, 
            (UINT8)PciBridgeInfoBuffer[Index].Location.Bus,
            (UINT8) PciBridgeInfoBuffer[Index].ConfigHeader.Bridge.SecondaryBus
            );
      }

      break;

    default:
      //
      // Unsupported
      //
      break;

    }
  }

  (*pEntry) = (UINT8 *)(TempEntry);

  return EFI_SUCCESS;
}

/**
  Update the extended Compatibility Bus Address Space Modifier Entry

  @param  pEntry                 The data pointer of Entry

  @retval EFI_SUCCESS            Update entry success

**/
EFI_STATUS
UpdateExtCompatBusAddrModifierEntries (
  IN OUT UINT8                            **pEntry
  )
{
  UINTN                                   Index;
  UINTN                                   NumOfExtBusAddrModifier;
  EFI_LEGACY_MP_TABLE_ENTRY_EXT_COMPAT_BUS_ADDR_SPACE_MODIFIER *pExtBusAddrModifier;
  EFI_LEGACY_MP_TABLE_ENTRY_EXT_COMPAT_BUS_ADDR_SPACE_MODIFIER *ExtBusAddrModifier;

  ExtBusAddrModifier = (EFI_LEGACY_MP_TABLE_ENTRY_EXT_COMPAT_BUS_ADDR_SPACE_MODIFIER *)(*pEntry);

  NumOfExtBusAddrModifier = sizeof(mExtBusAddrModifier) / sizeof(mExtBusAddrModifier[0]);
  pExtBusAddrModifier = &mExtBusAddrModifier[0];

  for (Index = 0; Index < NumOfExtBusAddrModifier; Index++) {

    if (pExtBusAddrModifier->EntryType != EFI_LEGACY_MP_TABLE_ENTRY_EXT_TYPE_COMPAT_BUS_ADDR_SPACE_MODIFIER) {
      pExtBusAddrModifier++;
      continue;
    }

    CopyMem (ExtBusAddrModifier, pExtBusAddrModifier, sizeof(EFI_LEGACY_MP_TABLE_ENTRY_EXT_COMPAT_BUS_ADDR_SPACE_MODIFIER));
    pExtBusAddrModifier++;
    ExtBusAddrModifier++;
  }

  (*pEntry) = (UINT8 *)(ExtBusAddrModifier);

  return EFI_SUCCESS;
}

/*
  Retrieve Bridge Handle from PciIo Handle Buffer.

  @param  **BridgeHandleBuffer       return the bridge handle buffer
  @param  BridgeHandleCount          return the number of bridge handle.

  @retval EFI_SUCCESS                    Create Buffer success
  @retval EFI_OUT_OF_RESOURCES   Not enought momory space.
  @retval EFI_NOT_FOUND               Locate PciIo failure.

**/
EFI_STATUS
LocateBridgeHandleBuffer (
  OUT EFI_PCI_BRIDGE_INFO                **BridgeHandleBuffer,
  OUT UINTN                              *BridgeHandleCount
  )
{
  EFI_STATUS                             Status;
  UINTN                                  Index;
  UINTN                                  PciIoHandleCount;
  EFI_HANDLE                             *PciIoHandleBuffer = NULL;
  EFI_PCI_IO_PROTOCOL                    *PciIo;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &PciIoHandleCount,
                  &PciIoHandleBuffer);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  *BridgeHandleCount = 0;
  Status = gBS->AllocatePool (EfiBootServicesData, PciIoHandleCount * sizeof (EFI_PCI_BRIDGE_INFO), (VOID **)BridgeHandleBuffer);
  ASSERT (BridgeHandleBuffer != NULL);

  for (Index = 0; Index < PciIoHandleCount; Index++) {

    Status = gBS->HandleProtocol (
                    PciIoHandleBuffer[Index],
                    &gEfiPciIoProtocolGuid,
                    (VOID **)&PciIo);

    if (EFI_ERROR (Status)) {
      continue;
    }

    PciIo->Pci.Read (
                 PciIo,
                 EfiPciIoWidthUint32,
                 0,
                 sizeof((*BridgeHandleBuffer)[*BridgeHandleCount].ConfigHeader) / sizeof(UINT32),
                 &(*BridgeHandleBuffer)[*BridgeHandleCount].ConfigHeader);
  
    if ((*BridgeHandleBuffer)[*BridgeHandleCount].ConfigHeader.Hdr.ClassCode[2] != PCI_CLASS_BRIDGE) {
      continue;
    }

    (*BridgeHandleBuffer)[*BridgeHandleCount].Handle = PciIoHandleBuffer[Index];

    PciIo->GetLocation (
             PciIo,
             &(*BridgeHandleBuffer)[*BridgeHandleCount].Location.Seg,
             &(*BridgeHandleBuffer)[*BridgeHandleCount].Location.Bus,
             &(*BridgeHandleBuffer)[*BridgeHandleCount].Location.Dev,
             &(*BridgeHandleBuffer)[*BridgeHandleCount].Location.Fun
             );
    
    (*BridgeHandleCount)++;
  }

  if (PciIoHandleBuffer != NULL) {
    FreePool (PciIoHandleBuffer);
  }

  return EFI_SUCCESS;
}



/*
  Create the Mp table in E F -Segment

  @param  Private                Legacy  BIOS Instance data

  @retval EFI_SUCCESS            Create Table success
  @retval EFI_OUT_OF_RESOURCES   Not enought momory space.

**/
EFI_STATUS
CreateMpTable (
  IN  LEGACY_BIOS_INSTANCE                  *Private
  )
{
  EFI_STATUS                             Status;
  EFI_IA32_REGISTER_SET                  Regs;
  EFI_LEGACY_MP_TABLE_FLOATING_POINTER   *TempMpTablePointer;
  EFI_LEGACY_MP_TABLE_HEADER             *PcmpEntry;
  UINT8                                  *TempMpTable;
  UINT8                                  *pEntry;
  UINT16                                 MpTableTotalSize;
  UINT16                                 ExtTableLength;
  UINT16                                 BaseTableLength;
  UINT8                                  CheckSum;
  UINT16                                 Index;
  UINT8                                  *PcmpTableAddress;
  UINT8                                  *MpTablePtrAddress;
  UINT8                                  *ExtEntriesPtrAddr;
  UINT8                                  *BaseEntriesPtrAddr;
  UINTN                                  TotalEntries;
  UINT64                                 ApicBaseReg;
  UINTN                                  PciBridgeInfoCount;
  EFI_PCI_BRIDGE_INFO                    *PciBridgeInfoBuffer;
  UINTN                                  RootBridgeHandleCount;
  EFI_HANDLE                             *RootBridgeHandleBuffer;
  UINTN                                  CharNumOfOemIdString;
  CHAR8                                  *pOemIdStringEntry;
  UINTN                                  CharNumOfProductIdString;
  CHAR8                                  *pProductIdStringEntry;
  UINTN                                  NumOfIoApic;
  EFI_LEGACY_MP_TABLE_ENTRY_IOAPIC       *pIoApicEntry;
  UINTN                                  NumOfIoApicIntLegacy;
  EFI_LEGACY_MP_TABLE_ENTRY_IO_INT       *pIoApicIntEntryLegacyHead;
  UINTN                                  NumOfIoApicInt;
  EFI_LEGACY_MP_TABLE_ENTRY_IO_INT       *pIoApicIntEntryHead;
  UINTN                                  NumOfIoApicIntSlot;
  MP_TABLE_ENTRY_IO_INT_SLOT             *pIoApicIntEntrySlotHead;
  UINTN                                  NumOfLocalApicInt;
  EFI_LEGACY_MP_TABLE_ENTRY_LOCAL_INT    *pLocalApicIntEntry;

  if (mLegacyMpInstalled) {
    return EFI_SUCCESS;
  }

  TempMpTablePointer        = NULL;
  TempMpTable               = NULL;
  PciBridgeInfoBuffer       = NULL;
  RootBridgeHandleBuffer    = NULL;
  pOemIdStringEntry         = NULL;
  pProductIdStringEntry     = NULL;
  pIoApicEntry              = NULL;
  pIoApicIntEntryLegacyHead = NULL;
  pIoApicIntEntryHead       = NULL;
  pIoApicIntEntrySlotHead   = NULL;
  pLocalApicIntEntry        = NULL;
  MpTableTotalSize          = 0;
  ExtTableLength            = 0;
  BaseTableLength           = 0;
  TotalEntries              = 0;
  //
  // Allocate temporary MP Table Pointer structure
  //
  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  sizeof(EFI_LEGACY_MP_TABLE_FLOATING_POINTER),
                  (VOID **)&TempMpTablePointer);
  if (Status != EFI_SUCCESS) {
    goto Error;
  }

  ZeroMem (TempMpTablePointer, sizeof(EFI_LEGACY_MP_TABLE_FLOATING_POINTER));

  //
  // Update MP Table Pointer structure
  //
  TempMpTablePointer->Signature = EFI_LEGACY_MP_TABLE_FLOATING_POINTER_SIGNATURE;

  TempMpTablePointer->Length = sizeof(EFI_LEGACY_MP_TABLE_FLOATING_POINTER) / 16;

  TempMpTablePointer->SpecRev = EFI_LEGACY_MP_TABLE_REV_1_4;

  //
  // Allocate temporary MP Table
  //
  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  (1 << 16),
                  (VOID **)&TempMpTable);
  if (Status != EFI_SUCCESS) {
    goto Error;
  }

  ZeroMem (TempMpTable, (1 << 16));
  PcmpEntry = (EFI_LEGACY_MP_TABLE_HEADER *)TempMpTable;

  //
  // Update MP Table Pointer structure
  //
  PcmpEntry->Signature = EFI_LEGACY_MP_TABLE_HEADER_SIGNATURE;

  ApicBaseReg = AsmReadMsr64 (MSR_IA32_APIC_BASE_ADDRESS);
  PcmpEntry->LocalApicAddress = (UINT32)(ApicBaseReg & 0xffffff000);

  PcmpEntry->SpecRev = TempMpTablePointer->SpecRev;

  Status = OemSvcPrepareInstallMpTable (
             &CharNumOfOemIdString,
             &pOemIdStringEntry,
             &CharNumOfProductIdString,
             &pProductIdStringEntry,
             &NumOfIoApic,
             &pIoApicEntry,
             &NumOfIoApicIntLegacy,
             &pIoApicIntEntryLegacyHead,
             &NumOfIoApicInt,
             &pIoApicIntEntryHead,
             &NumOfIoApicIntSlot,
             &pIoApicIntEntrySlotHead,
             &NumOfLocalApicInt,
             &pLocalApicIntEntry
             );

  if (Status != EFI_MEDIA_CHANGED) {
    return Status;
  }
  CopyMem (PcmpEntry->OemId, pOemIdStringEntry, sizeof(PcmpEntry->OemId));
  CopyMem (PcmpEntry->OemProductId, pProductIdStringEntry, sizeof(PcmpEntry->OemProductId));

  pEntry = (UINT8 *)(PcmpEntry + 1);

  Status = LocateBridgeHandleBuffer (&PciBridgeInfoBuffer, &PciBridgeInfoCount);

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciRootBridgeIoProtocolGuid,
                  NULL,
                  &RootBridgeHandleCount,
                  &RootBridgeHandleBuffer);
  if (EFI_ERROR (Status)) {
    goto Error;
  }
  
  //
  // Base Table Entries
  //
  Status = UpdateProcessorEntries (
             &pEntry,
             &TotalEntries,
             PcmpEntry->LocalApicAddress
             );

  if (EFI_ERROR (Status)) {
    goto Error;
  }
  BaseEntriesPtrAddr = pEntry;

  Status = UpdateBusEntries (
             &pEntry, 
             &TotalEntries, 
             PciBridgeInfoCount, 
             PciBridgeInfoBuffer, 
             RootBridgeHandleCount, 
             RootBridgeHandleBuffer
             );
  
  if (EFI_ERROR (Status)) {
    goto Error;
  }

  Status = UpdateIoApicEntries (&pEntry, &TotalEntries, NumOfIoApic, pIoApicEntry);
  if (EFI_ERROR (Status)) {
    goto Error;
  }

  Status = UpdateIoApicIntEntries (
             &pEntry,
             &TotalEntries,
             (EFI_LEGACY_MP_TABLE_ENTRY_BUS *)BaseEntriesPtrAddr,
             PciBridgeInfoCount, 
             PciBridgeInfoBuffer,
             NumOfIoApic,
             pIoApicEntry,
             NumOfIoApicIntLegacy,
             pIoApicIntEntryLegacyHead,
             NumOfIoApicInt,
             pIoApicIntEntryHead,
             NumOfIoApicIntSlot,
             pIoApicIntEntrySlotHead
             );
  if (EFI_ERROR (Status)) {
    goto Error;
  }

  Status = UpdateLocalApicIntEntries (&pEntry, &TotalEntries, NumOfLocalApicInt, pLocalApicIntEntry);
  if (EFI_ERROR (Status)) {
    goto Error;
  }

  ExtEntriesPtrAddr = pEntry;
  BaseTableLength = (UINT16)(ExtEntriesPtrAddr - (UINT8 *)PcmpEntry);

  //
  // Extended Table Entries
  //
  Status = UpdateExtSysAddrMappingEntries (&pEntry, RootBridgeHandleCount, RootBridgeHandleBuffer);
  if (EFI_ERROR (Status)) {
    goto Error;
  }

  Status = UpdateExtBusHierarchyDscEntries (
             &pEntry,
             (EFI_LEGACY_MP_TABLE_ENTRY_BUS *)BaseEntriesPtrAddr,
             PciBridgeInfoCount, 
             PciBridgeInfoBuffer);
  if (EFI_ERROR (Status)) {
    goto Error;
  }

  Status = UpdateExtCompatBusAddrModifierEntries (&pEntry);
  if (EFI_ERROR (Status)) {
    goto Error;
  }

  //
  // Assume E F segment already unlock.
  //

  PcmpEntry->EntryCount = (UINT16)TotalEntries;

  ExtTableLength = (UINT16)(pEntry - ExtEntriesPtrAddr);

  MpTableTotalSize = (UINT16)(pEntry - TempMpTable);

  PcmpEntry->BaseTableLength = BaseTableLength;

  PcmpEntry->ExtendedTableLength = ExtTableLength;

  CheckSum = 0;
  for (Index = 0; Index < ExtTableLength; Index++) {
    CheckSum = (UINT8)(CheckSum + ((UINT8*)(ExtEntriesPtrAddr))[Index]);
  }
  PcmpEntry->ExtendedChecksum = (UINT8)(0 - CheckSum);

  CheckSum = 0;
  for (Index = 0; Index < BaseTableLength; Index++) {
    CheckSum = (UINT8)(CheckSum + ((UINT8*)(PcmpEntry))[Index]);
  }
  PcmpEntry->Checksum = (UINT8)(0 - CheckSum);

  //
  // Get MP table address and fill data
  //
  ZeroMem (&Regs, sizeof (EFI_IA32_REGISTER_SET));

  Regs.X.AX = Legacy16GetTableAddress;
  Regs.X.CX = (UINT16)MpTableTotalSize;
  Regs.X.BX = 1;
  LegacyBiosFarCall86 (
    &Private->LegacyBios,
    Private->Legacy16CallSegment,
    Private->Legacy16CallOffset,
    &Regs,
    NULL,
    0);
  if (Regs.X.AX != 0) {
    Regs.X.AX = Legacy16GetTableAddress;
    Regs.X.CX = (UINT16)MpTableTotalSize;
    Regs.X.BX = 2;
    LegacyBiosFarCall86 (
      &Private->LegacyBios,
      Private->Legacy16CallSegment,
      Private->Legacy16CallOffset,
      &Regs,
      NULL,
      0);
    if (Regs.X.AX != 0) {
      Status = EFI_OUT_OF_RESOURCES;
      goto Error;
    }
  }

  PcmpTableAddress = (UINT8 *)(UINTN)(Regs.X.DS*16 + Regs.X.BX);

  CopyMem ((VOID *)PcmpTableAddress, (VOID *)TempMpTable, (UINTN)MpTableTotalSize);

  TempMpTablePointer->PhysicalAddress = (UINT32)(UINTN)PcmpTableAddress;

  TempMpTablePointer->Checksum = 0;
  CheckSum = 0;
  for (Index = 0; Index < sizeof(EFI_LEGACY_MP_TABLE_FLOATING_POINTER); Index++) {
    CheckSum = (UINT8)(CheckSum + ((UINT8*)(TempMpTablePointer))[Index]);
  }

  TempMpTablePointer->Checksum = (UINT8)(0 - CheckSum);

  //
  // Get MP table floating pointer address and fill data
  //
  ZeroMem (&Regs, sizeof (EFI_IA32_REGISTER_SET));

  Regs.X.AX = Legacy16GetTableAddress;
  Regs.X.CX = (UINT16)sizeof(EFI_LEGACY_MP_TABLE_FLOATING_POINTER);
  Regs.X.BX = 1;
  LegacyBiosFarCall86 (
    &Private->LegacyBios,
    Private->Legacy16CallSegment,
    Private->Legacy16CallOffset,
    &Regs,
    NULL,
    0);

  if (Regs.X.AX != 0) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Error;
  }

  MpTablePtrAddress = (UINT8 *)(UINTN)(Regs.X.DS*16 + Regs.X.BX);

  CopyMem ((VOID *)MpTablePtrAddress, (VOID *)TempMpTablePointer, (UINTN)sizeof(EFI_LEGACY_MP_TABLE_FLOATING_POINTER));

  mLegacyMpInstalled = TRUE;
  Status = EFI_SUCCESS;
Error:
  if (PciBridgeInfoBuffer != NULL) {
    FreePool (PciBridgeInfoBuffer);
  }
  if (RootBridgeHandleBuffer != NULL) {
    FreePool (RootBridgeHandleBuffer);
  }
  if (TempMpTablePointer != NULL) {
    FreePool (TempMpTablePointer);
  }
  if (TempMpTable != NULL) {
    FreePool (TempMpTable);
  }
  return Status;
}
