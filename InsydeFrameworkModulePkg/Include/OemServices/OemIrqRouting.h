//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _OEM_IRQ_ROUTING_H_
#define _OEM_IRQ_ROUTING_H_

//
// Define PIR table structures
//
#pragma pack (1)  
typedef struct {
  UINT32  Signature;
  UINT8   MinorVersion;
  UINT8   MajorVersion;
  UINT16  TableSize;
  UINT8   Bus;
  UINT8   DevFun;
  UINT16  PciOnlyIrq;
  UINT16  CompatibleVid;
  UINT16  CompatibleDid;
  UINT32  Miniport;
  UINT8   Reserved[11];
  UINT8   Checksum;
} EFI_LEGACY_PIRQ_TABLE_HEADER;

typedef struct {
  UINT8   Pirq;
  UINT16  IrqMask;
} EFI_LEGACY_PIRQ_ENTRY;

typedef struct {
  UINT8                 Bus;
  UINT8                 Device;
  EFI_LEGACY_PIRQ_ENTRY PirqEntry[4];
  UINT8                 Slot;
  UINT8                 Reserved;
} EFI_LEGACY_IRQ_ROUTING_ENTRY;
#pragma pack ()  

typedef struct _IRQ_ROUTING_TABLE {
  EFI_LEGACY_PIRQ_TABLE_HEADER   *IrqRoutingTableHeaderPtr;
  EFI_LEGACY_IRQ_ROUTING_ENTRY   *IrqRoutingTablePtr;
  UINTN                          MaxRoutingTableCount;
  UINTN                          SlotIndex;        
  UINTN                          MaxPciSlot;       
  UINT8                          *PirqLinkValuePtr;
} IRQ_ROUTING_TABLE;

#endif
