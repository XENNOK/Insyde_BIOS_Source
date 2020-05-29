/** @file
  Define the PCI devices which be skipped add to the PCI root bridge.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "DxeOemServicesThunk.h"
#include <Library/DxeOemSvcKernelLib.h>


/**
  This OemService provides OEM to define the PCI devices which be skipped add to the PCI root bridge. 
  OEM can implement this function by adding the Vendor ID and Device ID of skipping device to PciSkipTable.

  @param  *PciSkipTableCount    Point to the count of PCI skipping devices.
  @param  *PciSkipTable         Point to the PCI Skip Table.
  
  @retval EFI_SUCCESS           Install PCI skip table success.
  @retval Others                Base on OEM design.
**/
EFI_STATUS 
OemSvcInstallPciSkipTableThunk (
  IN OEM_SERVICES_PROTOCOL              *OemServices,
  IN  UINTN                             NumOfArgs,
  ...
  )
{
  VA_LIST                               Marker;
  UINTN                                 *PciSkipTableCount;
  PCI_SKIP_TABLE                        **PciSkipTable;
  EFI_STATUS                            Status;

  VA_START (Marker, NumOfArgs);
  PciSkipTableCount = VA_ARG (Marker, UINTN *);
  PciSkipTable      = VA_ARG (Marker, PCI_SKIP_TABLE **);
  VA_END (Marker);

  //
  // make a call to DxeOemSvcKernelLib internally
  //
  Status = OemSvcInstallPciSkipTable (
             PciSkipTableCount,
             PciSkipTable
             );

  return Status;
}

