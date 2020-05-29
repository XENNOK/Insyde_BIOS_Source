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

#include "LegacyBiosPlatformDxe.h"
#include <Library/KernelConfigLib.h>

#include <Protocol/LegacyInterrupt.h>
#include <Library/DxeChipsetSvcLib.h>

//
// PIRQ_A, PIRQ_B....PIRQ_H, total 8 PIRQs
//
#define MaxPIRQIndex              8
#define ErrorSlotNum              0xffff
#define PIRQ_DATA_NOT_SET(a)     ((a == 0) || (a == 0x1F))
#define MULTILAYER_VIRTUAL_BUS    0x80

BOOLEAN                              mGetRoutingInfo = FALSE;
IRQ_ROUTING_TABLE                   *mIrqRoutingTableInfoPtr;
LEGACY_MODIFY_PIR_TABLE             *mVirtualBusTablePtr;
UINT8                                mVirtualBusTableEntryNumber;
UINT8                               *mPirqPriorityTablePtr;
UINT8                                mPirqPriorityTableEntryNumber;
EFI_LEGACY_IRQ_PRIORITY_TABLE_ENTRY *mIrqPoolTablePtr;
UINT8                                mIrqPoolTableNumber;

static
EFI_STATUS
CheckIRQ (
);

static
EFI_STATUS
ModifyPirTable (
);

static
UINTN
SlotNumToPIrq (
  IN UINTN                              SlotNum
);

static
VOID
MarkIrqAsLegacyUsed (
  IN UINT8        IrqNumber
  );

/**
  Returns information associated with PCI IRQ routing.
  This function returns the following information associated with PCI IRQ routing:
    * An IRQ routing table and number of entries in the table.
    * The $PIR table and its size.
    * A list of PCI IRQs and the priority order to assign them.

  @param  This                    The protocol instance pointer.
  @param  RoutingTable            The pointer to PCI IRQ Routing table.
                                  This location is the $PIR table minus the header.
  @param  RoutingTableEntries     The number of entries in table.
  @param  LocalPirqTable          $PIR table.
  @param  PirqTableSize           $PIR table size.
  @param  LocalIrqPriorityTable   A list of interrupts in priority order to assign.
  @param  IrqPriorityTableEntries The number of entries in the priority table.

  @retval EFI_SUCCESS           Data was successfully returned.

**/
EFI_STATUS
EFIAPI
GetRoutingTable (
  IN  EFI_LEGACY_BIOS_PLATFORM_PROTOCOL           *This,
  OUT VOID                                        **RoutingTable,
  OUT UINTN                                       *RoutingTableEntries,
  OUT VOID                                        **LocalPirqTable, OPTIONAL
  OUT UINTN                                       *PirqTableSize, OPTIONAL
  OUT VOID                                        **LocalIrqPriorityTable, OPTIONAL
  OUT UINTN                                       *IrqPriorityTableEntries OPTIONAL
  )
{
  UINT16                        PTableSize;
  UINT32                        Index;
  UINT8                         Bus;
  UINT8                         Device;
  UINT8                         Function;
  UINT8                         Checksum;
  UINT8                         *Ptr;
  EFI_STATUS                    Status;
  EFI_LEGACY_INTERRUPT_PROTOCOL *LegacyInterrupt;

  Checksum = 0;

  if (!mGetRoutingInfo) {
    Status = ModifyPirTable ();
    if (Status != EFI_SUCCESS) {
      return EFI_UNSUPPORTED;
    }  
  }

  if (LocalPirqTable != NULL) {
    PTableSize = sizeof (EFI_LEGACY_PIRQ_TABLE_HEADER) +
                 sizeof (EFI_LEGACY_IRQ_ROUTING_ENTRY) *
                 (UINT16)mIrqRoutingTableInfoPtr->MaxRoutingTableCount;

    Status = gBS->LocateProtocol (
                    &gEfiLegacyInterruptProtocolGuid,
                    NULL,
                    (VOID **)&LegacyInterrupt
                    );

    ASSERT_EFI_ERROR (Status);
    LegacyInterrupt->GetLocation (
                       LegacyInterrupt,
                       &Bus,
                       &Device,
                       &Function
                       );

    //
    // Update fields in $PIR table header
    //
    mIrqRoutingTableInfoPtr->IrqRoutingTableHeaderPtr->TableSize = PTableSize;
    mIrqRoutingTableInfoPtr->IrqRoutingTableHeaderPtr->Bus = Bus;
    mIrqRoutingTableInfoPtr->IrqRoutingTableHeaderPtr->DevFun = (UINT8) ((Device << 3) + Function);
    Ptr = (UINT8 *) (mIrqRoutingTableInfoPtr->IrqRoutingTableHeaderPtr);
    for (Index = 0; Index < PTableSize; Index++) {
      Checksum = (UINT8) (Checksum + (UINT8) *Ptr);
      Ptr += 1;
    }

    Checksum                          = (UINT8) (0x00 - Checksum);
    mIrqRoutingTableInfoPtr->IrqRoutingTableHeaderPtr->Checksum = Checksum; 
    *LocalPirqTable                   = (VOID *) (mIrqRoutingTableInfoPtr->IrqRoutingTableHeaderPtr);
    *PirqTableSize                    = PTableSize;
  }

  *RoutingTable        = mIrqRoutingTableInfoPtr->IrqRoutingTablePtr;   
  *RoutingTableEntries = mIrqRoutingTableInfoPtr->MaxRoutingTableCount;  
  if (LocalIrqPriorityTable != NULL) {
  *LocalIrqPriorityTable    = mIrqPoolTablePtr ;
  *IrqPriorityTableEntries  = mIrqPoolTableNumber;
  }

  return EFI_SUCCESS;
}

/**
  Translates the given PIRQ accounting for bridge.
  This function translates the given PIRQ back through all buses, if required,
  and returns the true PIRQ and associated IRQ.

  @param  This                  The protocol instance pointer.
  @param  PciBus                The PCI bus number for this device.
  @param  PciDevice             The PCI device number for this device.
  @param  PciFunction           The PCI function number for this device.
  @param  Pirq                  Input is PIRQ reported by device, and output is true PIRQ.
  @param  PciIrq                The IRQ already assigned to the PIRQ, or the IRQ to be
                                assigned to the PIRQ.

  @retval EFI_SUCCESS           The PIRQ was translated.

**/
EFI_STATUS
EFIAPI
TranslatePirq (
  IN EFI_LEGACY_BIOS_PLATFORM_PROTOCOL            *This,
  IN UINTN                                        PciBus,
  IN UINTN                                        PciDevice,
  IN UINTN                                        PciFunction,
  IN OUT UINT8                                    *Pirq,
  OUT UINT8                                       *PciIrq
  )
{
  EFI_LEGACY_INTERRUPT_PROTOCOL *LegacyInterrupt;
  EFI_STATUS                    Status;
  UINTN                         Index;
  UINTN                         Index1;
  UINT8                         LocalPirq;
  UINT8                         PirqData;
  UINTN                         MaxIrqEntriesCount;
  EFI_LEGACY_IRQ_ROUTING_ENTRY  *IrqEntryTablePtr;
  UINT8                         *PirqLinkValuePtr;
  UINT8                         PirqIndex;

  if (!mGetRoutingInfo) {
    Status = ModifyPirTable ();
    if (Status != EFI_SUCCESS) { 
      return EFI_UNSUPPORTED;
    }  
  }

  Status = gBS->LocateProtocol (
                  &gEfiLegacyInterruptProtocolGuid,
                  NULL,
                  (VOID **)&LegacyInterrupt
                  );
  ASSERT_EFI_ERROR (Status);

  MaxIrqEntriesCount = mIrqRoutingTableInfoPtr->MaxRoutingTableCount;
  IrqEntryTablePtr = mIrqRoutingTableInfoPtr->IrqRoutingTablePtr;
  PirqLinkValuePtr = mIrqRoutingTableInfoPtr->PirqLinkValuePtr;
  LocalPirq = (UINT8) (*Pirq);

  Status = EFI_NOT_FOUND;
  for (Index = 0; Index < MaxIrqEntriesCount; Index++) {
    if ((IrqEntryTablePtr[Index].Bus == PciBus) &&
        (IrqEntryTablePtr[Index].Device == PciDevice)
        ) {
      for (PirqIndex = 0; PirqIndex < MaxPIRQIndex; PirqIndex++) {
        if (IrqEntryTablePtr[Index].PirqEntry[LocalPirq].Pirq == PirqLinkValuePtr[PirqIndex]) {
          LocalPirq = PirqIndex;
          break;
        }
      }
      LegacyInterrupt->ReadPirq (LegacyInterrupt, LocalPirq, &PirqData);
      //
      // In order to make the following code more readable, macros are used
      //
      #define CURRENT_IRQ      (mPirqPriorityTablePtr[LocalPirq])
      #define CHECKED_IRQ      (mIrqPoolTablePtr [Index1].Irq)
      #define CHECKED_IRQ_USED (mIrqPoolTablePtr [Index1].Used)
      //
      // Try to select a IRQ (1~15) for the PIRQ
      //
      //
      // Different platforms has different default value of PirqData.
      // So, PIRQ_DATA_NOT_SET should be defined to be the default value returned from ReadPirq in platform layer.
      //
      while (PIRQ_DATA_NOT_SET (PirqData)) {     // Loop until PirqData is choosed
        if (CURRENT_IRQ != 0) {   // This might be the IRQ we could choose
          for (Index1 = 0; Index1 < mIrqPoolTableNumber; Index1++) {
            //
            // This loop is to make sure the selected CURRENT_IRQ existed in IrqPriorityTable[]
            //
            if (CURRENT_IRQ == CHECKED_IRQ) {
              break;
            }
            if (CHECKED_IRQ == 0) {
              CHECKED_IRQ = CURRENT_IRQ;
              break;
            }
          }
          if (Index1 < mIrqPoolTableNumber) {
            if (CHECKED_IRQ_USED == LEGACY_USED) {
              //
              // This IRQ is already used by legacy devices
              // Clean the priority IRQ
              //
              CURRENT_IRQ = 0;
              continue;
            }
            PirqData = CURRENT_IRQ;
            CHECKED_IRQ_USED = PCI_USED;
          }
        } else {
          //
          // Select a IRQ from IrqPriorityTable[]
          //
          for (Index1 = 0; Index1 < mIrqPoolTableNumber; Index1++) {
            if ((CHECKED_IRQ != 0) && (CHECKED_IRQ_USED == PCI_UNUSED)) {
              PirqData = CHECKED_IRQ;
              CHECKED_IRQ_USED = PCI_USED;
              break;
            }
          }
        }
        if (PIRQ_DATA_NOT_SET (PirqData)) {
          //
          // No unused interrpts, so start reusing them by clear CHECKED_IRQ_USED
          //
          for (Index1 = 0; Index1 < mIrqPoolTableNumber; Index1++) {
            if (CHECKED_IRQ_USED == PCI_USED) {
              CHECKED_IRQ_USED = PCI_UNUSED;
            }
          }
        }
      }
      *PciIrq = PirqData;
      *Pirq   = LocalPirq;
      //
      // It is supposed there is only one bus/dev defined in the routing table
      //
      Status = EFI_SUCCESS;
      break;
    }
  }
  return Status;
}

/**
  Modifies the IrqRoutingEntry to reflect correct bus.

**/
static
EFI_STATUS
ModifyPirTable (
)
{
  UINT64                          Address;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *PciRootBridgeIo;
  EFI_LEGACY_IRQ_ROUTING_ENTRY    *IrqRoutingPtr;
  UINTN                           Index;
  UINTN                           Index1;
  UINT8                           TempData;
  UINTN                           IndexTempEntry, TempEntrySize;
  EFI_LEGACY_IRQ_ROUTING_ENTRY    *TempRoutingEntry;
  UINT8                           ReScan = FALSE;

  if ((mIrqRoutingTableInfoPtr == NULL) || (mVirtualBusTablePtr == NULL) || (mVirtualBusTableEntryNumber == 0)){
    return EFI_UNSUPPORTED;
  }
  
  IrqRoutingPtr = mIrqRoutingTableInfoPtr->IrqRoutingTablePtr;

  gBS->LocateProtocol (
         &gEfiPciRootBridgeIoProtocolGuid,
         NULL,
         (VOID **)&PciRootBridgeIo
         );

  //
  // Firstly, changing the multilayer virtual bus number in the table - mVirtualBusTablePtr
  //
  for (Index1 = 0; Index1 < mVirtualBusTableEntryNumber; Index1++) {
    //
    // Find an entry with real BridgeBus number
    //
    if (ReScan == TRUE) {
      ReScan = FALSE;
      Index1 = 0;
    }

    if ((mVirtualBusTablePtr[Index1].VirtualSecondaryBus & MULTILAYER_VIRTUAL_BUS) != 0) {
      continue;
    }
    Address = EFI_PCI_ADDRESS (
              mVirtualBusTablePtr[Index1].BridgeBus,
              mVirtualBusTablePtr[Index1].BridgeDevice,
              mVirtualBusTablePtr[Index1].BridgeFunction,
                PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET
                );
    PciRootBridgeIo->Pci.Read (
                           PciRootBridgeIo,
                           EfiPciWidthUint8,
                           Address,
                           1,
                           &TempData
                           );
    if (TempData == 0xFF) {
      continue;
    }
    for (Index = 0; Index < mVirtualBusTableEntryNumber; Index++) {
      //
      // Check the multilayer virtual bus entry and update it if the real bus number is found
      //
      if ((mVirtualBusTablePtr[Index].VirtualSecondaryBus & MULTILAYER_VIRTUAL_BUS) == 0) {
        continue;
      }
      if (mVirtualBusTablePtr[Index].BridgeBus != mVirtualBusTablePtr[Index1].VirtualSecondaryBus) {
        continue;
      }
      mVirtualBusTablePtr[Index].VirtualSecondaryBus &= (0xFF - MULTILAYER_VIRTUAL_BUS);
      mVirtualBusTablePtr[Index].BridgeBus = TempData;

      //
      //  Rescan to avoid order dependence.
      //
      ReScan = TRUE;
      break;
    }
  }

  for (Index1 = 0; Index1 < mVirtualBusTableEntryNumber; Index1++) {
    Address = EFI_PCI_ADDRESS (
              mVirtualBusTablePtr[Index1].BridgeBus,
              mVirtualBusTablePtr[Index1].BridgeDevice,
              mVirtualBusTablePtr[Index1].BridgeFunction,
                PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET
                );

    PciRootBridgeIo->Pci.Read (
                           PciRootBridgeIo,
                           EfiPciWidthUint8,
                           Address,
                           1,
                           &TempData
                           );
    for (Index = 0; Index < mIrqRoutingTableInfoPtr->MaxRoutingTableCount; Index++) {
      //
      // Verify virtual device, if it's bridge existed -> modify it's virtual bus number to real number (PIRQ Table)
      //
      if ((IrqRoutingPtr[Index].Bus == mVirtualBusTablePtr[Index1].VirtualSecondaryBus ) &&
          (IrqRoutingPtr[Index].Reserved == 0xFF)) {
        if (TempData != 0xFF) {
          //
          // Bridge exist, modify the virtual bus to a real one.
          //
          IrqRoutingPtr[Index].Bus = TempData;    // real bus number come from it's bridge register: "Secondary Bus"
          IrqRoutingPtr[Index].Reserved = 0x00;   // this virtual device has been processed done.
        } else {
          //
          // Bridge not exist, mark it and remove it from PIRQ Table later.
          //
          IrqRoutingPtr[Index].Reserved = 0xDD;
        }

      }
    }
  }

  for (Index = 0; Index < mIrqRoutingTableInfoPtr->MaxRoutingTableCount; Index++) {
    if (IrqRoutingPtr[Index].Reserved == 0xFF) {
      DEBUG ((DEBUG_INFO, "IRQ Routing Table error\n"));
    }
  }

  //
  //Set priority IRQ routing for PCI slots
  //
  CheckIRQ();

  //
  // Remove nonexistent virtual device from mIrqRoutingHeader
  //
  TempEntrySize = mIrqRoutingTableInfoPtr->MaxRoutingTableCount * sizeof (EFI_LEGACY_IRQ_ROUTING_ENTRY);
  TempRoutingEntry = AllocateZeroPool (TempEntrySize);
  if (TempRoutingEntry == NULL) {
    ASSERT_EFI_ERROR (TempRoutingEntry == NULL);
    return EFI_OUT_OF_RESOURCES;
  }

  for (Index = IndexTempEntry = 0; Index < mIrqRoutingTableInfoPtr->MaxRoutingTableCount; Index++) {
    if (IrqRoutingPtr[Index].Reserved == 0x00) {
      TempRoutingEntry[IndexTempEntry] = IrqRoutingPtr[Index];
      IndexTempEntry++;
    }
  }
  CopyMem ((VOID *) (UINTN)IrqRoutingPtr, TempRoutingEntry, TempEntrySize);  // Cover whole original RountingEntry Table
  mIrqRoutingTableInfoPtr->MaxRoutingTableCount = IndexTempEntry;
  gBS->FreePool (TempRoutingEntry);
  //
  // Set the flag to indicate the initialization finished
  //
  mGetRoutingInfo = TRUE;
  return EFI_SUCCESS;
}

/**
  Check the IRQ setting of Pci slot in "Setup" Value, and record it.

**/
static
EFI_STATUS
CheckIRQ (
  )
{
  EFI_STATUS                                      Status;
  KERNEL_CONFIGURATION                            KernelConfiguration;
  UINT8                                           ByteBuffer;
  UINTN                                           SlotNum;
  //
  // Parallel, Serial A, Serial B, IRQ setting
  //
  UINT8                                           IsaIRQ[2] = {0, 0};
  UINTN                                           Index;

  Status = GetKernelConfiguration (&KernelConfiguration);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  //  Check Serial Port, IR, Parallel Port IRQ Setting PIrqPriority table
  //
  if (KernelConfiguration.ComPortA) {
    IsaIRQ[0] = KernelConfiguration.ComPortAInterrupt;
    MarkIrqAsLegacyUsed (KernelConfiguration.ComPortAInterrupt);
  }
  if (KernelConfiguration.ComPortB) {
    IsaIRQ[1] = KernelConfiguration.ComPortBInterrupt;
    MarkIrqAsLegacyUsed (KernelConfiguration.ComPortBInterrupt);
  }
  if (KernelConfiguration.Lpt1) {
    MarkIrqAsLegacyUsed (KernelConfiguration.Lpt1Interrupt);
  }
  if (KernelConfiguration.Lpt2) {
    MarkIrqAsLegacyUsed (KernelConfiguration.Lpt2Interrupt);
  }

  //
  //  Record the PCI slot IRQ priority in
  //
  for (SlotNum = 0; SlotNum < 8; SlotNum++) {
    ByteBuffer = *(&KernelConfiguration.PciSlot1 + SlotNum);
    if ((ByteBuffer == 0) || (ByteBuffer == IsaIRQ[0]) || (ByteBuffer == IsaIRQ[1])) {
      //
      // No IRQ Priority setting or Used by Isa devices
      //
      continue;
    }
    //
    // Check if IRQ be other PCI slot used
    //
    for (Index = 0; Index < MaxPIRQIndex; Index++) {
      if (ByteBuffer == mPirqPriorityTablePtr[Index]) {
        break;
      }
    }
    if (Index < MaxPIRQIndex) {
      continue;
    }

    //
    // change slot number to PIRQ
    //
    Index = SlotNumToPIrq (SlotNum);
    if (Index == ErrorSlotNum) {
      //
      //This Pci Slot does not have Devices.
      //
      continue;
    }
    //
    // Record PCI slot PIRQ Priority setting
    //
    mPirqPriorityTablePtr[Index] = ByteBuffer;
  }

  return EFI_SUCCESS;
}

/**
  Check the InterruptPin of Pci slot

  @param  SlotNum                 which slot

  @retval ErrorSlotNum            No this device or error
  @retval others                  InterruptPin (0=PIRQ_A, 1=PIRQ_B......)

**/
static
UINTN
SlotNumToPIrq (
  IN UINTN                             SlotNum
)
{
  UINT8                                 Buffer;
  UINT64                                Address;
  PCI_TYPE00                            PciConfigHeader;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL       *PciRootBridgeIo;
  EFI_STATUS                            Status;
  UINT8                                 PirqIndex;
  EFI_LEGACY_IRQ_ROUTING_ENTRY          *RoutingTable;
  UINTN                                 RoutingTableIndex;

  Status = gBS->LocateProtocol (&gEfiPciRootBridgeIoProtocolGuid, NULL, (VOID **)&PciRootBridgeIo);
  if (Status != EFI_SUCCESS) {
    return ErrorSlotNum;
  }


  RoutingTableIndex = 0;
  RoutingTable      = mIrqRoutingTableInfoPtr->IrqRoutingTablePtr;   

  //
  // Go through Routing Table and find out PCI slot IRQ routing Entry.
  //
  while (RoutingTableIndex < mIrqRoutingTableInfoPtr->MaxRoutingTableCount) {
    if (RoutingTable[RoutingTableIndex].Slot == SlotNum ) {
      break;
    }
    RoutingTableIndex++;
  } 


  Address = EFI_PCI_ADDRESS (
              mIrqRoutingTableInfoPtr->IrqRoutingTablePtr[RoutingTableIndex].Bus,
              ((mIrqRoutingTableInfoPtr->IrqRoutingTablePtr[RoutingTableIndex].Device >> 3) & 0x1f),
              0,
              0);

  Status = PciRootBridgeIo->Pci.Read (
                                  PciRootBridgeIo,
                                  EfiPciIoWidthUint32,
                                  Address,
                                  sizeof (PciConfigHeader)/sizeof (UINT32),
                                  &PciConfigHeader);

  if ((Status != EFI_SUCCESS) || (PciConfigHeader.Hdr.VendorId == 0xffff)) {
    return ErrorSlotNum;
  }

  Buffer = PciConfigHeader.Device.InterruptPin;
  Buffer--;
  for (PirqIndex = 0; PirqIndex < MaxPIRQIndex; PirqIndex++) {
    if (mIrqRoutingTableInfoPtr->IrqRoutingTablePtr[RoutingTableIndex].PirqEntry[Buffer].Pirq == mIrqRoutingTableInfoPtr->PirqLinkValuePtr[PirqIndex]) {
      Buffer = PirqIndex;
      break;
    }
  }

  return Buffer;
}

/**
  Mark the designate Irq as Legacy Used

  @param  IrqNumber               The IRQ which been used as Legacy IRQ.

**/
static
VOID
MarkIrqAsLegacyUsed (
  IN UINT8        IrqNumber
  )
{
  UINTN   Index;

  Index = 0;

  for (Index = 0; Index < mIrqPoolTableNumber; Index++) {
    if (mIrqPoolTablePtr [Index].Irq == IrqNumber) {
      mIrqPoolTablePtr [Index].Used = LEGACY_USED;
      break;
    }
  }

  return ;
}

/**
  Retrieve platform specific IRQ Routing Information from ChipsetSvc
  and provide these information for GetRoutingTable () and TranslatePirq ().

  @retval EFI_UNSUPPORTED         Get Routing Information fail.
  @retval EFI_SUCCESS             Get Routing Information completely.
  
**/
EFI_STATUS
InitIrqRoutingInformation (
)
{
  EFI_STATUS Status;

  mVirtualBusTablePtr             = NULL;
  mVirtualBusTableEntryNumber     = 0;
  mIrqPoolTablePtr                = NULL;
  mIrqPoolTableNumber             = 0;
  mPirqPriorityTablePtr           = NULL;
  mPirqPriorityTableEntryNumber   = 0;
  mIrqRoutingTableInfoPtr         = NULL;

  Status = DxeCsSvcIrqRoutingInformation( &mVirtualBusTablePtr,    &mVirtualBusTableEntryNumber, 
                                          &mIrqPoolTablePtr,       &mIrqPoolTableNumber, 
                                          &mPirqPriorityTablePtr,  &mPirqPriorityTableEntryNumber,
                                          &mIrqRoutingTableInfoPtr
                                         );   
  return Status;
}

