/** @file
  Header file for the GetRoutingTable Driver.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _IRQ_ROUTING_INFORMATION_H
#define _IRQ_ROUTING_INFORMATION_H

#include <Protocol/LegacyBiosPlatform.h>

#pragma pack(1) 
typedef struct {
  UINT8                    BridgeBus;
  UINT8                    BridgeDevice;
  UINT8                    BridgeFunction;
  UINT8                    VirtualSecondaryBus;
} LEGACY_MODIFY_PIR_TABLE;
#pragma pack()

typedef struct {
  EFI_LEGACY_PIRQ_TABLE_HEADER   *IrqRoutingTableHeaderPtr;
  EFI_LEGACY_IRQ_ROUTING_ENTRY   *IrqRoutingTablePtr;
  UINTN                          MaxRoutingTableCount;
  UINTN                          SlotIndex;
  UINTN                          MaxPciSlot;
  // The Data structure represents the link value of PIRQA to PIRQH
  // Example:
  //   if the contents of this array is {0x60, 0x61, 0x62, 0x63, 0x68,0x69, 0x6A, 0x6B},
  //   LinkValue of PIRQA/B/C/D/E/F/G/H are 0x60/0x61/0x62/0x63/0x68/0x69/0x6A/0x6B respectively.
  //
  UINT8                          *PirqLinkValuePtr;
} IRQ_ROUTING_TABLE;

#endif
