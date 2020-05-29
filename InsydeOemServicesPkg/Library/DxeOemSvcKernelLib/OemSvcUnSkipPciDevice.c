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

#include <Library/DxeOemSvcKernelLib.h>

/*++

Todo:
  Define the relateaed data.
  
PCI_SKIP_TABLE      mPciSkipTable[] = {
  {
    VID,
    DID
  },  
  {
    0xffff,
    0xffff
  }
};

--*/

/**
  To handle a special case, more than 1 PCI devices which has the same VID/DID exist on the system but not all of them has to be skipped. 
  So provide an OEM service, let project owner to decide if this device has to be skipped or not. 
  If EFI_SUCCESS return, means this device should be un-skipped, otherwise, this device has to be skip. 
  This OEM service will only be invoked if the PCI device is going to be skipped according to PCI Skip Table, VID/DID information.

  @param[in]  Bus	  	        Bus number.  
  @param[in]  Device    	        Device number. 
  @param[in]  Function    	        Function number.
  @param[in]  VendorId    	        Device vendor ID.
  @param[in]  DeviceId    	        Device ID.
  
  @retval     EFI_UNSUPPORTED           Returns unsupported by default.
  @retval     EFI_SUCCESS               Device should be un-skipped.
  @retval     Others                    Device should be skipped.
**/
EFI_STATUS 
OemSvcUnSkipPciDevice (
  IN UINT8                          Bus,
  IN UINT8                          Device,
  IN UINT8                          Function,
  IN UINT16                         VendorId,
  IN UINT16                         DeviceId
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/

  return EFI_UNSUPPORTED;
}
