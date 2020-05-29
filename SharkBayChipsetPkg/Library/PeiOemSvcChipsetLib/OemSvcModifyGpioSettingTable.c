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
 This function offers an interface to Dynamically modify
 gChipsetPkgTokenSpaceGuid.PcdPeiGpioTable or change GPIO setting procedure.

 @param[in, out]    GpioTable           On entry, points to a structure that specifies the GPIO setting. 
                                        On exit, points to the updated structure. 
                                        The default value is decided by gChipsetPkgTokenSpaceGuid.PcdPeiGpioTable1.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcModifyGpioSettingTable (
  IN OUT  GPIO_SETTINGS_TABLE  *GpioTable
    )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/
  return EFI_UNSUPPORTED;
}
//[-end-121128-IB05280020-modify]//
