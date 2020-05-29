/** @file
  Provide OEM to modify the PCI IRQ routing table.

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "DxeOemServicesThunk.h"
#include <Library/DxeChipsetSvcLib.h>
#include <IrqRoutingInformation.h>

/**
  This OemService provides the user to modify the PCI IRQ routing table. 
  This table includes the IRQ table header and IRQ routing entry. 
  OEM modifies this table bases on the platform specification.

  @param  **IrqRoutingTable     A pointer to the structure of PCI IRQ routing table.

  @retval EFI_SUCCESS           Get PCI IRQ routing table success.
  @retval Others                Base on OEM design.
**/
EFI_STATUS
OemSvcInstallIrqRoutingTableThunk (
  IN OEM_SERVICES_PROTOCOL              *OemServices,
  IN  UINTN                             NumOfArgs,
  ...
  )
{
  VA_LIST                               Marker;
  IRQ_ROUTING_TABLE                     **IrqRoutingTable;
  EFI_STATUS                            Status;
  LEGACY_MODIFY_PIR_TABLE               *VirtualBusTablePtr;
  UINT8                                 VirtualBusTableEntryNumber;
  UINT8                                 *PirqPriorityTablePtr;
  UINT8                                 PirqPriorityTableEntryNumber;
  EFI_LEGACY_IRQ_PRIORITY_TABLE_ENTRY   *IrqPoolTablePtr;
  UINT8                                 IrqPoolTableNumber;
  VA_START (Marker, NumOfArgs);
  IrqRoutingTable = VA_ARG (Marker, IRQ_ROUTING_TABLE **);
  VA_END (Marker);

  VirtualBusTablePtr             = NULL;
  VirtualBusTableEntryNumber     = 0;
  IrqPoolTablePtr                = NULL;
  IrqPoolTableNumber             = 0;
  PirqPriorityTablePtr           = NULL;
  PirqPriorityTableEntryNumber   = 0;

  //
  // make a call to DxeCsSvc internally
  //
  Status = DxeCsSvcIrqRoutingInformation( &VirtualBusTablePtr,    &VirtualBusTableEntryNumber, 
                                          &IrqPoolTablePtr,       &IrqPoolTableNumber, 
                                          &PirqPriorityTablePtr,  &PirqPriorityTableEntryNumber,
                                          IrqRoutingTable
                                         );   

  return Status;
}
