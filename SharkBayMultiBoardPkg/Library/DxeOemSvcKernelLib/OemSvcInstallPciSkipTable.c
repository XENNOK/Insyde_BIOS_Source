/** @file
  Define the PCI devices which be skipped add to the PCI root bridge.

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

#include <Library/DxeOemSvcKernelLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>

//
// module variables
//
PCI_SKIP_TABLE      mPciSkipTable[] = {
  {
    0x8086,
    0x3B53
  },  
//[-start-130513-IB10300037-remove]//
////[-start-120518-IB03780440-add]//
////#ifdef THUNDERBOLT_SUPPORT
//  {
//    0x8086,
//    0x1513
//  },
//  {
//    0x8086,
//    0x1547
//  },
//  {
//    0x8086,
//    0x1548
//  },
//  {
//    0x8086,
//    0x1549
//  },
//  {
//    0x8086,
//    0x154A
//  },
//  {
//    0x8086,
//    0x154B
//  },
////#endif
////[-end-120518-IB03780440-add]//
//[-end-130513-IB10300037-remove]//
  {
    0xffff,
    0xffff
  }
};

/**
  This OemService provides OEM to define the PCI devices which be skipped add to the PCI root bridge. 
  OEM can implement this function by adding the Vendor ID and Device ID of skipping device to PciSkipTable.

  @param[out]  *PciSkipTableCount    Point to the count of PCI skipping devices.
  @param[out]  *PciSkipTable         Point to the PCI Skip Table.
  
  @retval      EFI_MEDIA_CHANGED     Install PCI skip table success.
  @retval      Others                Base on OEM design.
**/
EFI_STATUS 
OemSvcInstallPciSkipTable (
  OUT UINTN                          *PciSkipTableCount,
  OUT PCI_SKIP_TABLE                 **PciSkipTable
  )
{
  *PciSkipTableCount = (sizeof(mPciSkipTable) / sizeof(PCI_SKIP_TABLE));
  (*PciSkipTable)   = AllocateZeroPool (sizeof (mPciSkipTable));
  CopyMem ((*PciSkipTable), mPciSkipTable, sizeof (mPciSkipTable));

  return EFI_MEDIA_CHANGED;
}

