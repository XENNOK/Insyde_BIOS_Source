/** @file
 DXE Chipset Services Library.
	
 This file contains only one function that is IrqRoutingInformation().
 The function DxeCsSvcIrqRoutingInformation() use chipset services to return 
 four table for InitGetRoutingTable ().
	
***************************************************************************
* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#include <Library/BaseLib.h>
#include <Library/PcdLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <IrqRoutingInformation.h>
//[-start-140612-IB06720257-add]//
#include <ChipsetIrqRoutingEntry.h>
//[-end-140612-IB06720257-add]//

/**
 Provide IRQ Routing table according to platform specific information.

 @param[OUT]      VirtualBusTablePtr           A pointer to the VirtualBusTable that list all bridges in this platform 
                                               and bridges' virtual secondary bus.
 @param[OUT]      VirtualBusTableEntryNumber   A pointer to the VirtualBusTable entry number.
 @param[OUT]      IrqPoolTablePtr,             A pointer to the IRQ Pool Table.
 @param[OUT]      IrqPoolTableNumber,          A pointer to the IRQ Pool Table entry number.
 @param[OUT]      PirqPriorityTable,           A pointer to the PIRQ priority used Table.
 @param[OUT]      PirqPriorityTableEntryNumber A pointer to the PIRQ priority used Table entry number.
 @param[OUT]      IrqTablePtr                  A pointer to the IRQ routing Table.

 @retval          EFI_SUCCESS                  Get Platform specific IRQ Routing Info successfully..
 @retval          EFI_UNSUPPORTED              If the function is not implemented.
*/
EFI_STATUS
IrqRoutingInformation (
  OUT LEGACY_MODIFY_PIR_TABLE             **VirtualBusTablePtr,
  OUT UINT8                               *VirtualBusTableEntryNumber,
  OUT EFI_LEGACY_IRQ_PRIORITY_TABLE_ENTRY **IrqPoolTablePtr,
  OUT UINT8                               *IrqPoolTableNumber,
  OUT UINT8                               **PirqPriorityTablePtr,
  OUT UINT8                               *PirqPriorityTableEntryNumber,
  OUT IRQ_ROUTING_TABLE                   **IrqRoutingTableInfoPtr
)
{
//[-start-140612-IB06720257-modify]//
  UINTN  RoutingTableCount;
  UINTN  Index;
  EFI_LEGACY_IRQ_ROUTING_ENTRY_AND_IP_REGISTER  *RoutingTableAndIPRegisterPtrTemp;
  EFI_LEGACY_IRQ_ROUTING_ENTRY  *IrqRoutingEntryPtr;
  EFI_LEGACY_PIRQ_TABLE_HEADER  *IrqRoutingTableHeaderPtr;
  EFI_LEGACY_PIRQ_TABLE_HEADER  IrqRoutingHeader = {
   0x52495024, 00, 01, 0000, 00, 00, 0000, 0x8086, 0x122e,
   00000000, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00
  };

  RoutingTableAndIPRegisterPtrTemp = NULL;
  IrqRoutingEntryPtr = NULL;
  RoutingTableCount = 0;
  Index  = 0;

  *VirtualBusTablePtr              = PcdGetExPtr (&gChipsetPkgTokenSpaceGuid, PcdVirtualBusTable);
  *VirtualBusTableEntryNumber      = (UINT8)(LibPcdGetExSize (&gChipsetPkgTokenSpaceGuid, PcdToken (PcdVirtualBusTable)) / sizeof (LEGACY_MODIFY_PIR_TABLE));
  *IrqPoolTablePtr                 = PcdGetExPtr (&gChipsetPkgTokenSpaceGuid, PcdIrqPoolTable);
  *IrqPoolTableNumber              = (UINT8)(LibPcdGetExSize (&gChipsetPkgTokenSpaceGuid, PcdToken (PcdIrqPoolTable)) / sizeof (EFI_LEGACY_IRQ_PRIORITY_TABLE_ENTRY));
  *PirqPriorityTablePtr            = PcdGetExPtr (&gChipsetPkgTokenSpaceGuid, PcdPirqPriorityTable);
  *PirqPriorityTableEntryNumber    = (UINT8)(LibPcdGetExSize (&gChipsetPkgTokenSpaceGuid, PcdToken (PcdPirqPriorityTable)) / sizeof (UINT8));
  RoutingTableAndIPRegisterPtrTemp = PcdGetExPtr (&gChipsetPkgTokenSpaceGuid, PcdControllerDeviceIrqRoutingEntry);
  RoutingTableCount                = (UINT8)(LibPcdGetExSize (&gChipsetPkgTokenSpaceGuid, PcdToken (PcdControllerDeviceIrqRoutingEntry)) / sizeof (EFI_LEGACY_IRQ_ROUTING_ENTRY_AND_IP_REGISTER));

  (*IrqRoutingTableInfoPtr) = AllocateZeroPool (sizeof (IRQ_ROUTING_TABLE));
  if ((*IrqRoutingTableInfoPtr) == NULL) {
    return EFI_UNSUPPORTED;
  }

  //
  // Allocate a Buffer to assemble IrqRouting Header and  PcdControllerDeviceIrqRoutingEntry.
  //
//  RoutingTableCount      = (LibPcdGetExSize (&gChipsetPkgTokenSpaceGuid, PcdToken (PcdControllerDeviceIrqRoutingEntry)) / sizeof (EFI_LEGACY_IRQ_ROUTING_ENTRY));
  
  IrqRoutingTableHeaderPtr = AllocateZeroPool (sizeof (EFI_LEGACY_PIRQ_TABLE_HEADER) + sizeof (EFI_LEGACY_IRQ_ROUTING_ENTRY) * RoutingTableCount);
  if (IrqRoutingTableHeaderPtr == NULL) {
    FreePool(*IrqRoutingTableInfoPtr);
    return EFI_UNSUPPORTED;
  }

//  CopyMem ( IrqRoutingTableHeaderPtrTemp, 
//            &IrqRoutingHeader, 
//            sizeof(EFI_LEGACY_PIRQ_TABLE_HEADER));
  (*IrqRoutingTableInfoPtr)->IrqRoutingTableHeaderPtr = CopyMem (IrqRoutingTableHeaderPtr, &IrqRoutingHeader, sizeof(EFI_LEGACY_PIRQ_TABLE_HEADER));

//  CopyMem ((IrqRoutingTableHeaderPtrTemp + 1),
//           PcdGetExPtr (&gChipsetPkgTokenSpaceGuid, PcdControllerDeviceIrqRoutingEntry),
//           sizeof (EFI_LEGACY_IRQ_ROUTING_ENTRY) * RoutingTableCount);
  
  IrqRoutingEntryPtr = (EFI_LEGACY_IRQ_ROUTING_ENTRY *)(IrqRoutingTableHeaderPtr + 1);
  
  for (Index = 0; Index < RoutingTableCount; Index++) {    
    IrqRoutingEntryPtr[Index].Bus = RoutingTableAndIPRegisterPtrTemp[Index].LeagcyIrqRoutingEntry.Bus;
    IrqRoutingEntryPtr[Index].Device = RoutingTableAndIPRegisterPtrTemp[Index].LeagcyIrqRoutingEntry.Device;
    IrqRoutingEntryPtr[Index].PirqEntry[0].Pirq    = RoutingTableAndIPRegisterPtrTemp[Index].LeagcyIrqRoutingEntry.PirqEntry[0].Pirq   ;
    IrqRoutingEntryPtr[Index].PirqEntry[0].IrqMask = RoutingTableAndIPRegisterPtrTemp[Index].LeagcyIrqRoutingEntry.PirqEntry[0].IrqMask;
    IrqRoutingEntryPtr[Index].PirqEntry[1].Pirq    = RoutingTableAndIPRegisterPtrTemp[Index].LeagcyIrqRoutingEntry.PirqEntry[1].Pirq   ;
    IrqRoutingEntryPtr[Index].PirqEntry[1].IrqMask = RoutingTableAndIPRegisterPtrTemp[Index].LeagcyIrqRoutingEntry.PirqEntry[1].IrqMask;
    IrqRoutingEntryPtr[Index].PirqEntry[2].Pirq    = RoutingTableAndIPRegisterPtrTemp[Index].LeagcyIrqRoutingEntry.PirqEntry[2].Pirq   ;
    IrqRoutingEntryPtr[Index].PirqEntry[2].IrqMask = RoutingTableAndIPRegisterPtrTemp[Index].LeagcyIrqRoutingEntry.PirqEntry[2].IrqMask;
    IrqRoutingEntryPtr[Index].PirqEntry[3].Pirq    = RoutingTableAndIPRegisterPtrTemp[Index].LeagcyIrqRoutingEntry.PirqEntry[3].Pirq   ;
    IrqRoutingEntryPtr[Index].PirqEntry[3].IrqMask = RoutingTableAndIPRegisterPtrTemp[Index].LeagcyIrqRoutingEntry.PirqEntry[3].IrqMask;
    IrqRoutingEntryPtr[Index].Slot = RoutingTableAndIPRegisterPtrTemp[Index].LeagcyIrqRoutingEntry.Slot;
    IrqRoutingEntryPtr[Index].Reserved = RoutingTableAndIPRegisterPtrTemp[Index].LeagcyIrqRoutingEntry.Reserved;
  }

  (*IrqRoutingTableInfoPtr)->IrqRoutingTablePtr   = IrqRoutingEntryPtr;
  (*IrqRoutingTableInfoPtr)->MaxRoutingTableCount = RoutingTableCount;
  (*IrqRoutingTableInfoPtr)->PirqLinkValuePtr     = (UINT8 *)PcdGetExPtr (&gChipsetPkgTokenSpaceGuid, PcdPirqLinkValueArray);
//[-end-140612-IB06720257-modify]//
  return EFI_SUCCESS;  
}
