/** @file
    This function offers an interface to Dynamically modify gChipsetPkgTokenSpaceGuid.PcdPeiGpioTable.

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

#include <Library/PeiOemSvcChipsetLib.h>

//[-start-121128-IB05280020-modify]//
/**
 This function offers an interface to Dynamically modify gChipsetPkgTokenSpaceGuid.PcdPeiSioGpioTable,
 gChipsetPkgTokenSpaceGuid.PcdPeiSioTable or change procedure about SIO initial.

 @param[in, out]    SioTable            On entry, points to a structure that specifies the SIO register and value. 
                                        On exit, points to the updated structure. 
                                        The default value is decided by gChipsetPkgTokenSpaceGuid.PcdPeiSioTable.
 @param[in, out]    SioGpioTable        On entry, points to a structure that specifies the SIO GPIO register and value.
                                        On exit, points to the updated structure.
                                        The default value is decided by gChipsetPkgTokenSpaceGuid.PcdPeiSioGpioTable1.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcHookInitSio (
  IN OUT EFI_SIO_TABLE       *SioTable,
  IN OUT EFI_SIO_GPIO_TABLE  *SioGpioTable
    )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/
  return EFI_UNSUPPORTED;
}
//[-end-121128-IB05280020-modify]//
