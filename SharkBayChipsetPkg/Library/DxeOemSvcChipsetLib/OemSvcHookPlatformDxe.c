/** @file
  This function provides an interface to hook before and after DxePlatformEntryPoint (PlatformDxe.inf).

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

#include <Library/DxeOemSvcChipsetLib.h>

/**
 This function provides an interface to hook before and after DxePlatformEntryPoint (PlatformDxe.inf).PlatformDxe.inf
 is responsible for setting up any platform specific policy or initialization information.

 @param[in out]     *SetupVariable      On entry, points to CHIPSET_CONFIGURATION instance.
                                        On exit , points to updated CHIPSET_CONFIGURATION instance.
 @param[in out]     *PciRootBridgeIo    On entry, points to EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL instance.
                                        On exit , points to updated EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL instance.
 @param[in out]     Flag                TRUE : Before DxePlatformEntryPoint.
                                        FALSE: After DxePlatformEntryPoint.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcHookPlatformDxe (
  IN  CHIPSET_CONFIGURATION             *SetupVariable,
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *PciRootBridgeIo,
  IN  BOOLEAN                          Flag
  )
{
  if ( Flag )
  {
    //Operation 1
    //Add OEM operation before chipset operation here.
  } else {
    //Operation 2
    //Add OEM operation after chipset operation here.
  }
  return EFI_UNSUPPORTED;
}
