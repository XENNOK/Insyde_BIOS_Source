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
  To handle a special case, more than 1 PCI devices which has the same VID/DID exist on the system but not all of them has to be skipped. 
  So provide an OEM service, let project owner to decide if this device has to be skipped or not. 
  If EFI_SUCCESS return, means this device should be un-skipped, otherwise, this device has to be skip. 
  This OEM service will only be invoked if the PCI device is going to be skipped according to PCI Skip Table, VID/DID information.

  @param  Bus	  	            Bus number.  
  @param  Device    	        Device number. 
  @param  Function    	        Function number.
  @param  VendorId    	        Device vendor ID.
  @param  DeviceId    	        Device ID.
  
  @retval EFI_SUCCESS           Device should be un-skipped.
  @retval Others                Device should be skipped.
**/
EFI_STATUS 
OemSvcUnSkipPciDeviceThunk (
  IN  OEM_SERVICES_PROTOCOL             *OemServices,
  IN  UINTN                             NumOfArgs,
  ...
  )
{
  VA_LIST                               Marker;
  UINT8                                 Bus;
  UINT8                                 Device;
  UINT8                                 Function;
  UINT16                                VendorId;
  UINT16                                DeviceId;
  EFI_STATUS                            Status;

  VA_START (Marker, NumOfArgs);
  Bus      = VA_ARG (Marker, UINT8);
  Device   = VA_ARG (Marker, UINT8);
  Function = VA_ARG (Marker, UINT8);
  VendorId = VA_ARG (Marker, UINT16);
  DeviceId = VA_ARG (Marker, UINT16);
  VA_END (Marker);

  //
  // make a call to DxeOemSvcKernelLib internally
  //
  Status = OemSvcUnSkipPciDevice (
             Bus,
             Device,
             Function,
             VendorId,
             DeviceId
             );

  return Status;
}
