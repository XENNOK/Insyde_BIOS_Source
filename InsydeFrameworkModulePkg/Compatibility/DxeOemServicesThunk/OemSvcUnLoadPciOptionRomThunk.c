/** @file
  Provide OEM to install the PCI Option ROM table and Non-PCI Option ROM table.

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
  To handle a special case, more than 1 PCI devices which has the same VID/DID 
  exist on the system but not all of them has PCI option ROM to be loaded. 
  So provide an OEM service, let project owner to decide if this device has 
  PCI Option ROM to be loaded or not. if SkipGetPciRom is TRUE, 
  means there is no option ROM for this device, otherwise, 
  option ROM described on PCI Option ROM table will be loaded for this device. 
  This OEM service will only be invoked if the VID/DID of PCI device could be found in PCI Option ROM Table.	

  @param  Segment               Segment umber.
  @param  Bus                   Bus umber.
  @param  Device                Device umber.
  @param  Function              Function umber.
  @param  VendorId              Device vendor ID.
  @param  DeviceId              Device ID.
  @param  *SkipGetPciRom        If SkipGetPciRom == TRUE means that there is no 
			                    ROM in this device.
  
  @retval EFI_SUCCESS           Always returns success.
  @retval Others                Based on OEM design.
**/
EFI_STATUS
OemSvcUnLoadPciOptionRomThunk (
  IN  OEM_SERVICES_PROTOCOL             *OemServices,
  IN  UINTN                             NumOfArgs,
  ...
  )
{
  VA_LIST                               Marker;
  UINTN                                 Segment;
  UINTN                                 Bus;
  UINTN                                 Device;
  UINTN                                 Function;
  UINT16                                VendorId;
  UINT16                                DeviceId;
  BOOLEAN                               *SkipGetPciRom;
  EFI_STATUS                            Status;

  VA_START (Marker, NumOfArgs);
  
  Segment       = VA_ARG (Marker, UINTN);
  Bus           = VA_ARG (Marker, UINTN);
  Device        = VA_ARG (Marker, UINTN);
  Function      = VA_ARG (Marker, UINTN);
  VendorId      = VA_ARG (Marker, UINT16);
  DeviceId      = VA_ARG (Marker, UINT16);
  SkipGetPciRom = VA_ARG (Marker, BOOLEAN*);
  
  VA_END (Marker);

  //
  // make a call to DxeOemSvcKernelLib internally
  //
  Status = OemSvcSkipLoadPciOptionRom (
             Segment,
             Bus,
             Device,
             Function,
             VendorId,
             DeviceId,
             SkipGetPciRom
             );

  return Status;
}
